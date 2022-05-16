#ifndef DEVICE_BASE_CPU_HPP
#define DEVICE_BASE_CPU_HPP

#include <string>
#include "stream_config.hpp"

namespace ck {
namespace tensor_operation {
namespace cpu {
namespace device {

struct BaseArgument
{
    BaseArgument()                    = default;
    BaseArgument(const BaseArgument&) = default;
    BaseArgument& operator=(const BaseArgument&) = default;

    virtual ~BaseArgument() {}
};

struct BaseInvoker
{
    BaseInvoker()                   = default;
    BaseInvoker(const BaseInvoker&) = default;
    BaseInvoker& operator=(const BaseInvoker&) = default;

    virtual float Run(const BaseArgument*, const StreamConfig& = StreamConfig{}, int = 1) = 0;

    virtual ~BaseInvoker() {}
};

struct BaseOperator
{
    BaseOperator()                    = default;
    BaseOperator(const BaseOperator&) = default;
    BaseOperator& operator=(const BaseOperator&) = default;

    virtual bool IsSupportedArgument(const BaseArgument*) = 0;
    virtual std::string GetTypeString() const             = 0;

    virtual ~BaseOperator() {}
};

} // namespace device
} // namespace cpu
} // namespace tensor_operation
} // namespace ck
#endif