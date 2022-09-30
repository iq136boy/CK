// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2022, Advanced Micro Devices, Inc. All rights reserved.

#include <vector>

#include "ck/library/tensor_operation_instance/add_device_operation_instance.hpp"
#include "ck/library/tensor_operation_instance/gpu/softmax/device_softmax_f16_f16_instance_rank3_reduce2.hpp"
#include "ck/library/tensor_operation_instance/gpu/softmax/device_softmax_f16_f16_instance_type.hpp"

namespace ck {
namespace tensor_operation {
namespace device {
namespace instance {

namespace {
using F16  = ck::half_t;
using F32  = float;
using Pass = ck::tensor_operation::element_wise::PassThrough;
} // namespace

static constexpr index_t RANK = 3;

void add_device_softmax_f16_f16_rank3_reduce2_instances(
    std::vector<DeviceSoftmaxPtr<F16, F32, F16, Pass, Pass, RANK>>& instances)
{
    add_device_operation_instances(instances, device_softmax_f16_f16_instances<RANK, 2>{});
}

} // namespace instance
} // namespace device
} // namespace tensor_operation
} // namespace ck
