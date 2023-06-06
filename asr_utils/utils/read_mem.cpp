#include "utils/pch.h"
#include "utils/read_mem.h"
#include "utils/asr_extensions.h"

namespace asr_utils {
inline namespace v0 {

namespace {

template <typename T>
Address read_address(const ProcessInfo& process, Address address) {
    auto new_addr = read_mem<T>(process, address);
    return std::endian::native != process.endianness ? swap_endianness(new_addr) : new_addr;
}

}  // namespace

Address read_address(const ProcessInfo& process, Address address) {
    return process.is_64_bit ? read_address<uint64_t>(process, address)
                             : read_address<uint32_t>(process, address);
}

Address read_x86_offset(const ProcessInfo& process, Address address) {
    return process.is_64_bit ? read_x86_offset64(process, address)
                             : read_x86_offset32(process, address);
}

static_assert(std::endian::native == std::endian::little);
Address read_x86_offset32(ProcessId process, Address address) {
    uint32_t new_addr{};
    if (!process_read<uint32_t>(process, address, new_addr)) {
        return 0;
    }
    return std::endian::native != std::endian::little ? swap_endianness(new_addr) : new_addr;
}
Address read_x86_offset64(ProcessId process, Address address) {
    int32_t offset{};
    if (!process_read<int32_t>(process, address, offset)) {
        return 0;
    }
    if (std::endian::native != std::endian::little) {
        offset = swap_endianness(offset);
    }
    return address + static_cast<Address>(offset) + sizeof(offset);
}

}  // namespace v0
}  // namespace asr_utils
