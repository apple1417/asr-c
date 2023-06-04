#include "utils/pch.h"
#include "utils/read_mem.h"
#include "utils/asr_extensions.h"

namespace asr_utils {
inline namespace v0 {

Address read_offset32(ProcessId process, Address address) {
    return read_mem<uint32_t>(process, address);
}
Address read_offset64(ProcessId process, Address address) {
    int32_t offset{};
    if (!process_read<int32_t>(process, address, &offset)) {
        return 0;
    }
    return address + static_cast<Address>(offset) + sizeof(offset);
}

}  // namespace v0
}  // namespace asr_utils
