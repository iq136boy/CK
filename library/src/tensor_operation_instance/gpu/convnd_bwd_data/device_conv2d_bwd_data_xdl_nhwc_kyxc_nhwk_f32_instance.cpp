// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2022, Advanced Micro Devices, Inc. All rights reserved.

#include <cstdlib>

#include "ck/ck.hpp"
#include "ck/tensor_operation/gpu/device/tensor_layout.hpp"
#include "ck/tensor_operation/gpu/device/device_convnd_bwd_data_xdl_ndhwc_kzyxc_ndhwk.hpp"
#include "ck/tensor_operation/gpu/element/element_wise_operation.hpp"
#include "ck/library/tensor_operation_instance/add_device_operation_instance.hpp"

namespace ck {
namespace tensor_operation {
namespace device {
namespace instance {

using F32 = float;

template <ck::index_t... Is>
using S = ck::Sequence<Is...>;

using PassThrough = ck::tensor_operation::element_wise::PassThrough;
static constexpr auto ConvBwdDataDefault =
    ck::tensor_operation::device::ConvolutionBackwardDataSpecialization::Default;

static constexpr auto ConvBwdDataFilter1x1Stride1Pad0 =
    ck::tensor_operation::device::ConvolutionBackwardDataSpecialization::Filter1x1Stride1Pad0;

// Compilation parameters for in[n, hi, wi, c] * wei[k, y, x, c] = out[n, ho, wo, k]
using device_conv2d_bwd_data_xdl_nhwc_kyxc_nhwk_f32_instances =
    std::tuple<
        // clang-format off
        //#############################################################################| InData| WeiData| OutData| AccData|          In|         Wei|         Out|        ConvBackward|    Num| Block|  MPer|  NPer| K0Per| K1| MPer| NPer| MXdl| NXdl|  ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockLds|  BBlockTransfer| BBlockTransfer| BBlockTransfer| BlockTransfer| BBlockTransfer| BBlockTransfer| BBlockLds| CThreadTransfer| CThreadTransfer|
        //#############################################################################|   Type|    Type|    Type|    Type| Elementwise| Elementwise| Elementwise|                Data|    Dim|  Size| Block| Block| Block|   |  XDL|  XDL|  Per|  Per|   ThreadCluster|  ThreadCluster| SrcAccessOrder|   SrcVectorDim|      SrcScalar|      DstScalar| AddExtraM|   ThreadCluster|  ThreadCluster| SrcAccessOrder|  SrcVectorDim|      SrcScalar|      DstScalar| AddExtraN| SrcDstVectorDim|       DstScalar|
        //#############################################################################|       |        |        |        |   Operation|   Operation|   Operation|      Specialization|Spatial|      |      |      |      |   |     |     | Wave| Wave| Lengths_K0_M_K1|   ArrangeOrder|               |               |      PerVector|   PerVector_K1|          | Lengths_K0_N_K1|   ArrangeOrder|               |              |      PerVector|   PerVector_K1|          |                |       PerVector|
        //#############################################################################|       |        |        |        |            |            |            |                    |       |      |      |      |      |   |     |     |     |     |                |               |               |               |               |               |          |                |               |               |              |               |               |          |                |                |
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataDefault,   2,     256,   256,   128,     4,  4,   32,   32,    4,    2,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 64, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              2,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataDefault,   2,     256,   128,   256,     4,  4,   32,   32,    2,    4,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 64, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              4,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataDefault,   2,     128,   128,   128,     4,  4,   32,   32,    4,    2,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 32, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              4,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataDefault,   2,     256,   128,   128,     4,  4,   32,   32,    2,    2,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 64, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              2,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataDefault,   2,     128,   128,    64,     4,  4,   32,   32,    2,    2,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 32, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              2,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataDefault,   2,     128,    64,   128,     4,  4,   32,   32,    2,    2,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 32, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              4,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataDefault,   2,      64,    64,    64,     4,  4,   32,   32,    2,    2,     S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 16, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              4,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataDefault,   2,     256,   128,    64,     4,  4,   32,   32,    2,    1,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 64, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              1,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataDefault,   2,     256,    64,   128,     4,  4,   32,   32,    1,    2,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 64, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              2,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataDefault,   2,     128,   128,    32,     4,  4,   32,   32,    2,    1,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 32, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              1,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataDefault,   2,     128,    32,   128,     4,  4,   32,   32,    1,    2,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 32, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              4,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataDefault,   2,      64,    64,    32,     4,  4,   32,   32,    2,    1,     S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 16, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              2,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataDefault,   2,      64,    32,    64,     4,  4,   32,   32,    1,    2,     S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 16, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              4,              4,      true,               7,               1>
        // clang-format on
        >;

using device_conv2d_bwd_data_xdl_nhwc_kyxc_nhwk_1x1_s1_p0_f32_instances =
    std::tuple<
        // clang-format off
        //#############################################################################| InData| WeiData| OutData| AccData|          In|         Wei|         Out|                     ConvBackward|    Num| Block|  MPer|  NPer| K0Per| K1| MPer| NPer| MXdl| NXdl|  ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockLds|  BBlockTransfer| BBlockTransfer| BBlockTransfer| BlockTransfer| BBlockTransfer| BBlockTransfer| BBlockLds| CThreadTransfer| CThreadTransfer|
        //#############################################################################|   Type|    Type|    Type|    Type| Elementwise| Elementwise| Elementwise|                             Data|    Dim|  Size| Block| Block| Block|   |  XDL|  XDL|  Per|  Per|   ThreadCluster|  ThreadCluster| SrcAccessOrder|   SrcVectorDim|      SrcScalar|      DstScalar| AddExtraM|   ThreadCluster|  ThreadCluster| SrcAccessOrder|  SrcVectorDim|      SrcScalar|      DstScalar| AddExtraN| SrcDstVectorDim|       DstScalar|
        //#############################################################################|       |        |        |        |   Operation|   Operation|   Operation|                   Specialization|Spatial|      |      |      |      |   |     |     | Wave| Wave| Lengths_K0_M_K1|   ArrangeOrder|               |               |      PerVector|   PerVector_K1|          | Lengths_K0_N_K1|   ArrangeOrder|               |              |      PerVector|   PerVector_K1|          |                |       PerVector|
        //#############################################################################|       |        |        |        |            |            |            |                                 |       |      |      |      |      |   |     |     |     |     |                |               |               |               |               |               |          |                |               |               |              |               |               |          |                |                |
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,   2,     256,   256,   128,     4,  4,   32,   32,    4,    2,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 64, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              2,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,   2,     256,   128,   256,     4,  4,   32,   32,    2,    4,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 64, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              4,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,   2,     128,   128,   128,     4,  4,   32,   32,    4,    2,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 32, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              4,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,   2,     256,   128,   128,     4,  4,   32,   32,    2,    2,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 64, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              2,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,   2,     128,   128,    64,     4,  4,   32,   32,    2,    2,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 32, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              2,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,   2,     128,    64,   128,     4,  4,   32,   32,    2,    2,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 32, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              4,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,   2,      64,    64,    64,     4,  4,   32,   32,    2,    2,     S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 16, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              4,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,   2,     256,   128,    64,     4,  4,   32,   32,    2,    1,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 64, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              1,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,   2,     256,    64,   128,     4,  4,   32,   32,    1,    2,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 64, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              2,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,   2,     128,   128,    32,     4,  4,   32,   32,    2,    1,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 32, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              1,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,   2,     128,    32,   128,     4,  4,   32,   32,    1,    2,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 32, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              4,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,   2,      64,    64,    32,     4,  4,   32,   32,    2,    1,     S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 16, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              2,              4,      true,               7,               1>,
        DeviceConvndBwdDataXdl_Input_N_Di_Hi_Wi_C_Weight_K_Z_Y_X_C_Output_N_Do_Ho_Wo_K<    F32,     F32,     F32,     F32, PassThrough, PassThrough, PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,   2,      64,    32,    64,     4,  4,   32,   32,    1,    2,     S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              4,              4,      true,     S<4, 16, 1>,     S<2, 0, 1>,     S<0, 2, 1>,             1,              4,              4,      true,               7,               1>
        // clang-format on
        >;

void add_device_conv2d_bwd_data_xdl_nhwc_kyxc_nhwk_f32_instances(
    std::vector<DeviceConvBwdDataPtr<PassThrough, PassThrough, PassThrough>>& instances)
{
    add_device_operation_instances(instances,
                                   device_conv2d_bwd_data_xdl_nhwc_kyxc_nhwk_f32_instances{});
    add_device_operation_instances(
        instances, device_conv2d_bwd_data_xdl_nhwc_kyxc_nhwk_1x1_s1_p0_f32_instances{});
}

} // namespace instance
} // namespace device
} // namespace tensor_operation
} // namespace ck
