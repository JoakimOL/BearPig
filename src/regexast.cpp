#include "libbearpig/regexast.h"

namespace bp {
GroupExp::GroupExp() = default;
GroupExp::~GroupExp() = default;
GroupExp::GroupExp(GroupExp &&other) : subExp(std::move(other.subExp)) {}
} // namespace bp
