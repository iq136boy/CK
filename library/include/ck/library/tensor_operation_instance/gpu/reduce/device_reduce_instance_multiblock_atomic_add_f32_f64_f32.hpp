// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2022, Advanced Micro Devices, Inc. All rights reserved.

#pragma once

#include "ck/utility/data_type.hpp"
#include "ck/utility/reduction_enums.hpp"

#include "ck/library/tensor_operation_instance/gpu/reduce/device_reduce_instance_multiblock_atomic_add.hpp"

namespace ck {
namespace tensor_operation {
namespace device {
namespace instance {

// clang-format off
// InDataType | AccDataType | OutDataType | ReduceOpId | PropagateNan | UseIndex | Rank | NumReduceDim 
ADD_MULTIBLOCK_ATOMIC_ADD_INST_REF(float, double, float, ReduceTensorOp::ADD, false, false, 4, 3); // for ADD
ADD_MULTIBLOCK_ATOMIC_ADD_INST_REF(float, double, float, ReduceTensorOp::ADD, false, false, 4, 4);
ADD_MULTIBLOCK_ATOMIC_ADD_INST_REF(float, double, float, ReduceTensorOp::ADD, false, false, 4, 1);
ADD_MULTIBLOCK_ATOMIC_ADD_INST_REF(float, double, float, ReduceTensorOp::ADD, false, false, 2, 1);
ADD_MULTIBLOCK_ATOMIC_ADD_INST_REF(float, double, float, ReduceTensorOp::AVG, false, false, 4, 3); // for AVG
ADD_MULTIBLOCK_ATOMIC_ADD_INST_REF(float, double, float, ReduceTensorOp::AVG, false, false, 4, 4);       
ADD_MULTIBLOCK_ATOMIC_ADD_INST_REF(float, double, float, ReduceTensorOp::AVG, false, false, 4, 1);       
ADD_MULTIBLOCK_ATOMIC_ADD_INST_REF(float, double, float, ReduceTensorOp::AVG, false, false, 2, 1);
// clang-format on

} // namespace instance
} // namespace device
} // namespace tensor_operation
} // namespace ck
