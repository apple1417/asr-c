#include "utils/pch.h"
#include "utils/process_info.h"
#include "utils/asr_extensions.h"
#include "utils/read_mem.h"

namespace asr_utils {
inline namespace v0 {

namespace {

// ASR makes the boggling decision not to expose pointer width.
// This means we need to work it out by parsing the executable header.

enum class Architecture {
    UNKNOWN,
    X64,
    X32,
};

enum class Endianness {
    UNKNOWN,
    BIG,
    LITTLE,
};

const constexpr auto DOS_HEADER_MAGIC_LFA_NEW_PADDING = 0x3A;

struct DOSHeader {
    uint8_t e_magic[2];
    uint8_t padding[DOS_HEADER_MAGIC_LFA_NEW_PADDING];
    uint16_t e_lfa_new;
};

struct NTHeader {
    uint8_t sig[4];
    uint16_t machine;
};

const constexpr auto DOS_HEADER_MAGIC = "MZ";
const constexpr auto NT_HEADER_SIG = "PE\0\0";

const constexpr auto IMAGE_FILE_MACHINE_I386 = 0x14C;
const constexpr auto IMAGE_FILE_MACHINE_AMD64 = 0x8664;

/**
 * @brief Tries to extract the architecture + endianness of a PE.
 *
 * @param process The progress to examine.
 * @param main_module The process's main module address.
 * @return A pair of the extracted architecture and endianness. Both unknown if failed to parse.
 */
std::pair<Architecture, Endianness> try_parse_pe(ProcessId process, Address main_module) {
    DOSHeader dos{};
    if (!process_read(process, main_module, &dos)) {
        return {Architecture::UNKNOWN, Endianness::UNKNOWN};
    }
    if (memcmp(&dos.e_magic[0], DOS_HEADER_MAGIC, sizeof(dos.e_magic)) != 0) {
        return {Architecture::UNKNOWN, Endianness::UNKNOWN};
    }
    if (std::endian::native != std::endian::little) {
        dos.e_lfa_new = swap_endianness(dos.e_lfa_new);
    }

    // At this point, we can be pretty sure we've got a PE, so start printing error messages

    NTHeader nt_header{};
    if (!process_read(process, main_module + dos.e_lfa_new, &nt_header)) {
        runtime_print_message("Failed to read NT header");
        return {Architecture::UNKNOWN, Endianness::UNKNOWN};
    }
    if (memcmp(&nt_header.sig[0], NT_HEADER_SIG, sizeof(nt_header.sig)) != 0) {
        runtime_print_message("PE has invalid NT signature {:x}{:x}{:x}{:x}", nt_header.sig[0],
                              nt_header.sig[1], nt_header.sig[2], nt_header.sig[3]);
        return {Architecture::UNKNOWN, Endianness::UNKNOWN};
    }

    if (std::endian::native != std::endian::little) {
        nt_header.machine = swap_endianness(nt_header.machine);
    }

    if (nt_header.machine == IMAGE_FILE_MACHINE_AMD64) {
        return {Architecture::X64, Endianness::LITTLE};
    }
    if (nt_header.machine == IMAGE_FILE_MACHINE_I386) {
        return {Architecture::X32, Endianness::LITTLE};
    }

    runtime_print_message("PE has unknown machine type {:x}", nt_header.machine);
    return {Architecture::UNKNOWN, Endianness::UNKNOWN};
}

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

/**
 * @brief Tries to extract the architecture + endianness of an ELF executable.
 *
 * @param process The progress to examine.
 * @param main_module The process's main module address.
 * @return A pair of the extracted architecture and endianness. Both unknown if failed to parse.
 */
std::pair<Architecture, Endianness> try_parse_elf(ProcessId process, Address main_module) {
    ELFHeader header{};
    if (!process_read(process, main_module, &header)) {
        return {Architecture::UNKNOWN, Endianness::UNKNOWN};
    }
    if (memcmp(&header.ei_mag[0], ELFMAG, sizeof(header.ei_mag)) != 0) {
        return {Architecture::UNKNOWN, Endianness::UNKNOWN};
    }

    Architecture arch = Architecture::UNKNOWN;
    switch (header.ei_class) {
        case ELFCLASS32:
            arch = Architecture::X32;
            break;
        case ELFCLASS64:
            arch = Architecture::X64;
            break;
    }

    Endianness endian = Endianness::UNKNOWN;
    switch (header.ei_data) {
        case ELFDATA2LSB:
            endian = Endianness::LITTLE;
            break;
        case ELFCLASS64:
            endian = Endianness::BIG;
            break;
    }

    return {arch, endian};
}

}  // namespace

ProcessInfo::ProcessInfo(ProcessId pid) : pid(pid), exe_path(std::move(process_get_path(pid))) {
    // Strictly speaking, ASR's supposed to use unix paths, but let's err on the side of caution
    // Would prefer to use std::filesystem, but that's still disabled in wasi
    auto executable_name = exe_path.substr(exe_path.find_last_of("/\\") + 1);

    this->main_module = process_get_module_address(this->pid, executable_name);
    this->main_module_size = process_get_module_size(this->pid, executable_name);

    auto [arch, endian] = try_parse_pe(this->pid, this->main_module);
    if (arch == Architecture::UNKNOWN && endian == Endianness::UNKNOWN) {
        std::tie(arch, endian) = try_parse_elf(this->pid, this->main_module);
    }

    if (arch == Architecture::UNKNOWN) {
        runtime_print_message("Unable to determine pointer size of executable '{}'!",
                              executable_name);
    } else {
        this->is_64_bit = arch == Architecture::X64;
    }

    if (endian == Endianness::UNKNOWN) {
        runtime_print_message("Unable to determine endianness of executable '{}'!",
                              executable_name);
    } else {
        this->endianness = endian == Endianness::LITTLE ? std::endian::little : std::endian::big;
    }
}

ProcessInfo::operator ProcessId() const {
    return this->pid;
}

}  // namespace v0
}  // namespace asr_utils
