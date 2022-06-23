#ifndef DEVICE_MULTIPLE_REDUCE_HPP
#define DEVICE_MULTIPLE_REDUCE_HPP

#include <vector>
#include <memory>
#include <iostream>

#include "common_header.hpp"
#include "device_base.hpp"
#include "reduction_enums.hpp"

namespace ck {
namespace tensor_operation {
namespace device {

template <index_t NumReduction,
          typename InElementwiseOperationTuple,
          typename AccElementwiseOperationTuple>
struct DeviceMultipleReduce : public BaseOperator
{
    virtual std::unique_ptr<BaseArgument>
    MakeArgumentPointer(const std::vector<index_t> inLengths,
                        const std::vector<index_t> inStrides,
                        const std::vector<index_t> outLengths,
                        const std::vector<index_t> outStrides,
                        const std::vector<int> reduceDims,
                        const std::array<float, NumReduction> alpha_values,
                        const std::array<float, NumReduction> beta_values,
                        const void* in_dev,
                        const std::array<void*, NumReduction> out_dev_buffers,
                        const InElementwiseOperationTuple in_elementwise_op_tuple,
                        const AccElementwiseOperationTuple acc_elementwise_op_tuple) = 0;

    virtual std::unique_ptr<BaseInvoker> MakeInvokerPointer() = 0;
};

template <index_t NumReduction,
          typename InElementwiseOperationTuple,
          typename AccElementwiseOperationTuple>
using DeviceMultipleReducePtr = std::unique_ptr<
    DeviceMultipleReduce<NumReduction, InElementwiseOperationTuple, AccElementwiseOperationTuple>>;

} // namespace device
} // namespace tensor_operation
} // namespace ck
#endif