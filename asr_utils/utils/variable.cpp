#include "utils/pch.h"
#include "utils/variable.h"
#include "utils/asr_extensions.h"


namespace asr_utils {
inline namespace v0 {

template <>
template <>
Variable<int>::operator std::string(void) const {
    return std::to_string(this->value_internal);
}
template <>
template <>
Variable<long>::operator std::string(void) const {
    return std::to_string(this->value_internal);
}
template <>
template <>
Variable<long long>::operator std::string(void) const {
    return std::to_string(this->value_internal);
}
template <>
template <>
Variable<unsigned>::operator std::string(void) const {
    return std::to_string(this->value_internal);
}
template <>
template <>
Variable<unsigned long>::operator std::string(void) const {
    return std::to_string(this->value_internal);
}
template <>
template <>
Variable<unsigned long long>::operator std::string(void) const {
    return std::to_string(this->value_internal);
}
template <>
template <>
Variable<float>::operator std::string(void) const {
    return std::to_string(this->value_internal);
}
template <>
template <>
Variable<double>::operator std::string(void) const {
    return std::to_string(this->value_internal);
}
template <>
template <>
Variable<long double>::operator std::string(void) const {
    return std::to_string(this->value_internal);
}

}  // namespace v0
}  // namespace asr_utils
