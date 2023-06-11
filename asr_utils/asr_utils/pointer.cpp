#include "asr_utils/pch.h"
#include "asr_utils/pointer.h"
#include "asr_utils/read_mem.h"

namespace asr_utils {
inline namespace v0 {

Address DeepPointer::dereference(const ProcessInfo& process) const {
    auto addr = read_address(process, this->base);
    if (this->offsets.empty()) {
        return addr;
    }

    if (addr == 0) {
        return 0;
    }

    for (size_t i = 0; i < (this->offsets.size() - 1); i++) {
        addr = read_address(process, addr + this->offsets[i]);
        if (addr == 0) {
            return 0;
        }
    }

    return addr + this->offsets.back();
}

}  // namespace v0
}  // namespace asr_utils
