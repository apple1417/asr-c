#include "utils/pch.h"
#include "utils/process_info.h"
#include "utils/asr_extensions.h"
#include "utils/read_mem.h"

namespace asr_utils {
inline namespace v0 {

namespace {

/*
ASR does not expose pointer width, so we need to parse the executable header ourself to work it out.

There's a proton bug doing this also lets us fix: Only one 0x1000 byte memory range is reported as
belonging to the executable, which we can overwrite with the image size from the PE header.

Doing this through memory, not filesystem, since it makes for cleaner code, it means we don't have
to rely on WASI, and it actually deals with another proton bug: the reported executable path might
point to wine instead.
*/

const constexpr auto DOS_HEADER_MAGIC_LFA_NEW_PADDING = 0x3A;
const constexpr auto NT_HEADER_MACHINE_SIZEOFIMAGE_PADDING = 0x4A;

struct DOSHeader {
    uint8_t e_magic[2];
    uint8_t padding[DOS_HEADER_MAGIC_LFA_NEW_PADDING];
    uint16_t e_lfa_new;
};

struct NTHeader {
    uint8_t sig[4];
    uint16_t machine;
    uint8_t padding[NT_HEADER_MACHINE_SIZEOFIMAGE_PADDING];
    uint32_t size_of_image;
};

const constexpr auto DOS_HEADER_MAGIC = "MZ";
const constexpr auto NT_HEADER_SIG = "PE\0\0";

const constexpr auto IMAGE_FILE_MACHINE_I386 = 0x14C;
const constexpr auto IMAGE_FILE_MACHINE_AMD64 = 0x8664;

const constexpr auto PROTON_INVALID_MAIN_MODULE_SIZE = 0x1000;

}  // namespace

/**
 * @brief Tries to extract the architecture + endianness of a PE.
 *
 * @param process The progress to examine.
 * @param main_module The process's main module address.
 * @return A pair of the extracted architecture and endianness. Both unknown if failed to parse.
 */
bool ProcessInfo::try_parse_pe(ProcessInfo::InitFlags init) {
    DOSHeader dos{};
    if (!process_read(this->pid, main_module, dos)) {
        return false;
    }
    if (memcmp(&dos.e_magic[0], DOS_HEADER_MAGIC, sizeof(dos.e_magic)) != 0) {
        return false;
    }
    if (std::endian::native != std::endian::little) {
        dos.e_lfa_new = swap_endianness(dos.e_lfa_new);
    }

    // At this point, we can be pretty sure we've got a PE, so start printing error messages

    NTHeader nt_header{};
    if (!process_read(this->pid, main_module + dos.e_lfa_new, nt_header)) {
        runtime_print_message("Failed to read NT header");
        return false;
    }
    if (memcmp(&nt_header.sig[0], NT_HEADER_SIG, sizeof(nt_header.sig)) != 0) {
        runtime_print_message("PE has invalid NT signature {:x}{:x}{:x}{:x}", nt_header.sig[0],
                              nt_header.sig[1], nt_header.sig[2], nt_header.sig[3]);
        return false;
    }

    if ((init & INIT_FIX_PROTON_MAIN_MODULE_SIZE) != 0 && this->main_module_size == PROTON_INVALID_MAIN_MODULE_SIZE
        && runtime_get_os() == "linux") {
        if (std::endian::native != std::endian::little) {
            nt_header.size_of_image = swap_endianness(nt_header.size_of_image);
        }

        this->main_module_size = nt_header.size_of_image;
    }

    if (std::endian::native != std::endian::little) {
        nt_header.machine = swap_endianness(nt_header.machine);
    }

    if (nt_header.machine == IMAGE_FILE_MACHINE_AMD64) {
        this->is_64_bit = true;
        this->endianness = std::endian::little;
        return true;
    }
    if (nt_header.machine == IMAGE_FILE_MACHINE_I386) {
        this->is_64_bit = false;
        this->endianness = std::endian::little;
        return true;
    }

    runtime_print_message("PE has unknown machine type {:x}", nt_header.machine);
    return false;
}

namespace {

struct ELFHeader {
    uint8_t ei_mag[4];
    uint8_t ei_class;
    uint8_t ei_data;
};

const constexpr auto ELFMAG = "\177ELF";
const constexpr auto ELFCLASS32 = 1;
const constexpr auto ELFCLASS64 = 2;
const constexpr auto ELFDATA2LSB = 1;
const constexpr auto ELFDATA2MSB = 2;

}  // namespace

bool ProcessInfo::try_parse_elf(ProcessInfo::InitFlags /*init*/) {
    ELFHeader header{};
    if (!process_read(this->pid, main_module, header)) {
        return false;
    }
    if (memcmp(&header.ei_mag[0], ELFMAG, sizeof(header.ei_mag)) != 0) {
        return false;
    }

    switch (header.ei_class) {
        case ELFCLASS32:
            this->is_64_bit = false;
            break;
        case ELFCLASS64:
            this->is_64_bit = true;
            break;
    }

    switch (header.ei_data) {
        case ELFDATA2LSB:
            this->endianness = std::endian::little;
            break;
        case ELFCLASS64:
            this->endianness = std::endian::big;
            break;
    }

    return true;
}

ProcessInfo::ProcessInfo(ProcessId pid, const std::string_view& main_module, InitFlags init)
    : pid(pid), exe_path(std::move(process_get_path(pid))) {
    // Strictly speaking, ASR's supposed to use unix paths, but let's err on the side of caution
    // Would prefer to use std::filesystem, but that's still disabled in wasi
    std::string main_module_name;
    if (main_module.empty()) {
        main_module_name = exe_path.substr(exe_path.find_last_of("/\\") + 1);
    } else {
        main_module_name = main_module;
    }

    this->main_module = process_get_module_address(this->pid, main_module_name);
    this->main_module_size = process_get_module_size(this->pid, main_module_name);

    if (try_parse_pe(init)) {
        return;
    }
    if (!try_parse_elf(init)) {
        return;
    }

    runtime_print_message(
        "Unable to parse executable header in module '{}', some defaults may be incorrect.",
        main_module_name);
}

ProcessInfo::operator ProcessId() const {
    return this->pid;
}

}  // namespace v0
}  // namespace asr_utils
