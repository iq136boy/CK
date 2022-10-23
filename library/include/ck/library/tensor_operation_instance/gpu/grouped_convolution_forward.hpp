// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2022, Advanced Micro Devices, Inc. All rights reserved.

#pragma once

#include <cstdlib>

#include "ck/ck.hpp"
#include "ck/tensor_operation/gpu/device/tensor_layout.hpp"
#include "ck/tensor_operation/gpu/device/device_grouped_conv_fwd.hpp"
#include "ck/tensor_operation/gpu/element/element_wise_operation.hpp"

#include "ck/library/tensor_operation_instance/device_operation_instance_factory.hpp"

namespace ck {
namespace tensor_operation {
namespace device {
namespace instance {

// grouped conv1d forward, GNWC/GKXC/GNWK
void add_device_grouped_conv1d_fwd_xdl_gnwc_gkxc_gnwk_bf16_instances(
    std::vector<std::unique_ptr<DeviceGroupedConvFwd<1,
                                                     GNWC,
                                                     GKXC,
                                                     GNWK,
                                                     BF16,
                                                     BF16,
                                                     BF16,
                                                     PassThrough,
                                                     PassThrough,
                                                     PassThrough>>>& instances);

void add_device_grouped_conv1d_fwd_xdl_gnwc_gkxc_gnwk_f16_instances(
    std::vector<std::unique_ptr<DeviceGroupedConvFwd<1,
                                                     GNWC,
                                                     GKXC,
                                                     GNWK,
                                                     F16,
                                                     F16,
                                                     F16,
                                                     PassThrough,
                                                     PassThrough,
                                                     PassThrough>>>& instances);

void add_device_grouped_conv1d_fwd_xdl_gnwc_gkxc_gnwk_f32_instances(
    std::vector<std::unique_ptr<DeviceGroupedConvFwd<1,
                                                     GNWC,
                                                     GKXC,
                                                     GNWK,
                                                     F32,
                                                     F32,
                                                     F32,
                                                     PassThrough,
                                                     PassThrough,
                                                     PassThrough>>>& instances);

void add_device_grouped_conv1d_fwd_xdl_gnwc_gkxc_gnwk_int8_instances(
    std::vector<std::unique_ptr<DeviceGroupedConvFwd<1,
                                                     GNWC,
                                                     GKXC,
                                                     GNWK,
                                                     int8_t,
                                                     int8_t,
                                                     int8_t,
                                                     PassThrough,
                                                     PassThrough,
                                                     PassThrough>>>& instances);

// grouped conv2d forward, GNHWC/GKYXC/GNHWK
void add_device_grouped_conv1d_fwd_xdl_gnhwc_gkyxc_gnhwk_bf16_instances(
    std::vector<std::unique_ptr<DeviceGroupedConvFwd<2,
                                                     GNHWC,
                                                     GKYXC,
                                                     GNHWK,
                                                     BF16,
                                                     BF16,
                                                     BF16,
                                                     PassThrough,
                                                     PassThrough,
                                                     PassThrough>>>& instances);

void add_device_grouped_conv2d_fwd_xdl_gnhwc_gkyxc_gnhwk_f16_instances(
    std::vector<std::unique_ptr<DeviceGroupedConvFwd<2,
                                                     GNHWC,
                                                     GKYXC,
                                                     GNHWK,
                                                     F16,
                                                     F16,
                                                     F16,
                                                     PassThrough,
                                                     PassThrough,
                                                     PassThrough>>>& instances);

void add_device_grouped_conv2d_fwd_xdl_gnhwc_gkyxc_gnhwk_f32_instances(
    std::vector<std::unique_ptr<DeviceGroupedConvFwd<2,
                                                     GNHWC,
                                                     GKYXC,
                                                     GNHWK,
                                                     F32,
                                                     F32,
                                                     F32,
                                                     PassThrough,
                                                     PassThrough,
                                                     PassThrough>>>& instances);

void add_device_grouped_conv2d_fwd_xdl_gnhwc_gkyxc_gnhwk_int8_instances(
    std::vector<std::unique_ptr<DeviceGroupedConvFwd<2,
                                                     GNHWC,
                                                     GKYXC,
                                                     GNHWK,
                                                     int8_t,
                                                     int8_t,
                                                     int8_t,
                                                     PassThrough,
                                                     PassThrough,
                                                     PassThrough>>>& instances);

// grouped conv2d forward, NHWGC/GKYXC/NHWGK
void add_device_grouped_conv2d_fwd_xdl_nhwgc_gkyxc_nhwgk_f16_instances(
    std::vector<std::unique_ptr<DeviceGroupedConvFwd<2,
                                                     NHWGC,
                                                     GKYXC,
                                                     NHWGK,
                                                     F16,
                                                     F16,
                                                     F16,
                                                     PassThrough,
                                                     PassThrough,
                                                     PassThrough>>>& instances);

// grouped conv3d forward, GNDHWC/GKZYXC/GNDHWK
void add_device_grouped_conv3d_fwd_xdl_gndhwc_gkzyxc_gndhwk_bf16_instances(
    std::vector<std::unique_ptr<DeviceGroupedConvFwd<3,
                                                     GNDHWC,
                                                     GKZYXC,
                                                     GNDHWK,
                                                     BF16,
                                                     BF16,
                                                     BF16,
                                                     PassThrough,
                                                     PassThrough,
                                                     PassThrough>>>& instances);

void add_device_grouped_conv3d_fwd_xdl_gndhwc_gkzyxc_gndhwk_f16_instances(
    std::vector<std::unique_ptr<DeviceGroupedConvFwd<3,
                                                     GNDHWC,
                                                     GKZYXC,
                                                     GNDHWK,
                                                     F16,
                                                     F16,
                                                     F16,
                                                     PassThrough,
                                                     PassThrough,
                                                     PassThrough>>>& instances);

void add_device_grouped_conv3d_fwd_xdl_gndhwc_gkzyxc_gndhwk_f32_instances(
    std::vector<std::unique_ptr<DeviceGroupedConvFwd<3,
                                                     GNDHWC,
                                                     GKZYXC,
                                                     GNDHWK,
                                                     F32,
                                                     F32,
                                                     F32,
                                                     PassThrough,
                                                     PassThrough,
                                                     PassThrough>>>& instances);

void add_device_grouped_conv3d_fwd_xdl_gndhwc_gkzyxc_gndhwk_int8_instances(
    std::vector<std::unique_ptr<DeviceGroupedConvFwd<3,
                                                     GNDHWC,
                                                     GKZYXC,
                                                     GNDHWK,
                                                     int8_t,
                                                     int8_t,
                                                     int8_t,
                                                     PassThrough,
                                                     PassThrough,
                                                     PassThrough>>>& instances);

template <ck::index_t NumDimSpatial,
          typename InLayout,
          typename WeiLayout,
          typename OutLayout,
          typename InDataType,
          typename WeiDataType,
          typename OutDataType>
struct DeviceOperationInstanceFactory<ck::tensor_operation::device::DeviceGroupedConvFwd<
    NumDimSpatial,
    InLayout,
    WeiLayout,
    OutLayout,
    InDataType,
    WeiDataType,
    OutDataType,
    ck::tensor_operation::element_wise::PassThrough,
    ck::tensor_operation::element_wise::PassThrough,
    ck::tensor_operation::element_wise::PassThrough>>
{
    using DeviceOp = DeviceGroupedConvFwd<NumDimSpatial,
                                          InLayout,
                                          WeiLayout,
                                          OutLayout,
                                          InDataType,
                                          WeiDataType,
                                          OutDataType,
                                          ck::tensor_operation::element_wise::PassThrough,
                                          ck::tensor_operation::element_wise::PassThrough,
                                          ck::tensor_operation::element_wise::PassThrough>;

    static auto GetInstances()
    {
        std::vector<std::unique_ptr<DeviceOp>> op_ptrs;

        if constexpr(NumDimSpatial == 1 && is_same_v<InLayout, GNWC> &&
                     is_same_v<WeiLayout, GKXC> && is_same_v<OutLayout, GNWK>)
        {
            if constexpr(is_same_v<InDataType, float> && is_same_v<WeiDataType, float> &&
                         is_same_v<OutDataType, float>)
            {
                add_device_grouped_conv1d_fwd_xdl_gnwc_gkxc_gnwk_f32_instances(op_ptrs);
            }
            else if constexpr(is_same_v<InDataType, half_t> && is_same_v<WeiDataType, half_t> &&
                              is_same_v<OutDataType, half_t>)
            {
                add_device_grouped_conv1d_fwd_xdl_gnwc_gkxc_gnwk_f16_instances(op_ptrs);
            }
            else if constexpr(is_same_v<InDataType, ck::bhalf_t> &&
                              is_same_v<WeiDataType, ck::bhalf_t> &&
                              is_same_v<OutDataType, ck::bhalf_t>)
            {
                add_device_grouped_conv1d_fwd_xdl_gnwc_gkxc_gnwk_bf16_instances(op_ptrs);
            }
            else if constexpr(is_same_v<InDataType, int8_t> && is_same_v<WeiDataType, int8_t> &&
                              is_same_v<OutDataType, int8_t>)
            {
                add_device_grouped_conv1d_fwd_xdl_gnwc_gkxc_gnwk_int8_instances(op_ptrs);
            }
        }
        else if constexpr(NumDimSpatial == 2 && is_same_v<InLayout, GNHWC> &&
                          is_same_v<WeiLayout, GKYXC> && is_same_v<OutLayout, GNHWK>)
        {
            if constexpr(is_same_v<InDataType, float> && is_same_v<WeiDataType, float> &&
                         is_same_v<OutDataType, float>)
            {
                add_device_grouped_conv2d_fwd_xdl_gnhwc_gkyxc_gnhwk_f32_instances(op_ptrs);
            }
            else if constexpr(is_same_v<InDataType, half_t> && is_same_v<WeiDataType, half_t> &&
                              is_same_v<OutDataType, half_t>)
            {
                add_device_grouped_conv2d_fwd_xdl_gnhwc_gkyxc_gnhwk_f16_instances(op_ptrs);
            }
            else if constexpr(is_same_v<InDataType, ck::bhalf_t> &&
                              is_same_v<WeiDataType, ck::bhalf_t> &&
                              is_same_v<OutDataType, ck::bhalf_t>)
            {
                add_device_grouped_conv1d_fwd_xdl_gnhwc_gkyxc_gnhwk_bf16_instances(op_ptrs);
            }
            else if constexpr(is_same_v<InDataType, int8_t> && is_same_v<WeiDataType, int8_t> &&
                              is_same_v<OutDataType, int8_t>)
            {
                add_device_grouped_conv2d_fwd_xdl_gnhwc_gkyxc_gnhwk_int8_instances(op_ptrs);
            }
        }
        else if constexpr(NumDimSpatial == 2 && is_same_v<InLayout, NHWGC> &&
                          is_same_v<WeiLayout, GKYXC> && is_same_v<OutLayout, NHWGK>)
        {
            if constexpr(is_same_v<InDataType, float> && is_same_v<WeiDataType, float> &&
                         is_same_v<OutDataType, float>)
            {
                // no instance
            }
            else if constexpr(is_same_v<InDataType, half_t> && is_same_v<WeiDataType, half_t> &&
                              is_same_v<OutDataType, half_t>)
            {
                add_device_grouped_conv2d_fwd_xdl_nhwgc_gkyxc_nhwgk_f16_instances(op_ptrs);
            }
            else if constexpr(is_same_v<InDataType, ck::bhalf_t> &&
                              is_same_v<WeiDataType, ck::bhalf_t> &&
                              is_same_v<OutDataType, ck::bhalf_t>)
            {
                // no instance
            }
            else if constexpr(is_same_v<InDataType, int8_t> && is_same_v<WeiDataType, int8_t> &&
                              is_same_v<OutDataType, int8_t>)
            {
                // no instance
            }
        }
        else if constexpr(NumDimSpatial == 3 && is_same_v<InLayout, GNDHWC> &&
                          is_same_v<WeiLayout, GKZYXC> && is_same_v<OutLayout, GNDHWK>)
        {
            if constexpr(is_same_v<InDataType, float> && is_same_v<WeiDataType, float> &&
                         is_same_v<OutDataType, float>)
            {
                add_device_grouped_conv3d_fwd_xdl_gndhwc_gkzyxc_gndhwk_f32_instances(op_ptrs);
            }
            else if constexpr(is_same_v<InDataType, half_t> && is_same_v<WeiDataType, half_t> &&
                              is_same_v<OutDataType, half_t>)
            {
                add_device_grouped_conv3d_fwd_xdl_gndhwc_gkzyxc_gndhwk_f16_instances(op_ptrs);
            }
            else if constexpr(is_same_v<InDataType, ck::bhalf_t> &&
                              is_same_v<WeiDataType, ck::bhalf_t> &&
                              is_same_v<OutDataType, ck::bhalf_t>)
            {
                add_device_grouped_conv3d_fwd_xdl_gndhwc_gkzyxc_gndhwk_bf16_instances(op_ptrs);
            }
            else if constexpr(is_same_v<InDataType, int8_t> && is_same_v<WeiDataType, int8_t> &&
                              is_same_v<OutDataType, int8_t>)
            {
                add_device_grouped_conv3d_fwd_xdl_gndhwc_gkzyxc_gndhwk_int8_instances(op_ptrs);
            }
        }

        return op_ptrs;
    }
};

} // namespace instance
} // namespace device
} // namespace tensor_operation
} // namespace ck
