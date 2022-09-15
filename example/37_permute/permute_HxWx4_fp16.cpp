// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2022, Advanced Micro Devices, Inc. All rights reserved.

#include "common.hpp"

using DataType   = F16;
using BundleType = F64;

static_assert(sizeof(BundleType) % sizeof(DataType) == 0);

// clang-format off
using DevicePermuteInstance = ck::tensor_operation::device::DevicePermute
// ######|     InData|    OutData| Elementwise| NumDim| Block|  NPer|  HPer|  WPer|   InBlock|      InBlockTransfer|           InBlockTransfer|       Src|       Dst|             Src|             Dst|
// ######|       Type|       Type|   Operation|       |  Size| Block| Block| Block| LdsExtraW| ThreadClusterLengths| ThreadClusterArrangeOrder| VectorDim| VectorDim| ScalarPerVector| ScalarPerVector|
// ######|           |           |            |       |      |      |      |      |          |                     |                          |          |          |                |                |
// ######|           |           |            |       |      |      |      |      |          |                     |                          |          |          |                |                |
         < BundleType, BundleType, PassThrough,      3,   256,     1,    32,    32,         5,         S<1, 32,  8>,                S<0, 1, 2>,         2,         1,               4,               1>;
// clang-format on

#include "run_permute_bundle_example.inc"

int main() { return !run_permute_bundle_example({1, 80, 16000}, {0, 2, 1}); }