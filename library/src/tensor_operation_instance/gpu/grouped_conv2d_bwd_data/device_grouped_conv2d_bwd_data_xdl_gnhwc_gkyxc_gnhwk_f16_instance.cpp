// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2022, Advanced Micro Devices, Inc. All rights reserved.

#include <cstdlib>

#include "ck/ck.hpp"
#include "ck/tensor_operation/gpu/device/tensor_layout.hpp"
#include "ck/tensor_operation/gpu/device/impl/device_grouped_conv_bwd_data_multiple_d_xdl_cshuffle_v1.hpp"
#include "ck/tensor_operation/gpu/element/element_wise_operation.hpp"

#include "ck/library/tensor_operation_instance/add_device_operation_instance.hpp"

namespace ck {
namespace tensor_operation {
namespace device {
namespace instance {

using F16 = ck::half_t;
using F32 = float;

using EmptyTuple = ck::Tuple<>;

template <ck::index_t... Is>
using S = ck::Sequence<Is...>;

using GNHWC = ck::tensor_layout::convolution::GNHWC;
using GKYXC = ck::tensor_layout::convolution::GKYXC;
using GNHWK = ck::tensor_layout::convolution::GNHWK;

using PassThrough = ck::tensor_operation::element_wise::PassThrough;

static constexpr auto ConvBwdDataDefault =
    ck::tensor_operation::device::ConvolutionBackwardDataSpecialization::Default;

static constexpr auto ConvBwdDataFilter1x1Stride1Pad0 =
    ck::tensor_operation::device::ConvolutionBackwardDataSpecialization::Filter1x1Stride1Pad0;

using device_grouped_conv2d_bwd_data_xdl_gnhwc_gkyxc_gnhwk_f16_instances =
    std::tuple<
    // clang-format off
        // 1. Default
        // ##############################################|    NDim| ALayout| BLayout|   DsLayout| ELayout| AData| BData| AccData| CShuffle|     DsData| EData| AElementwise| BElementwise| CDEElementwise| ConvolutionBackward| DoPad| DoPad|      NumGemmK| Block|  MPer|  NPer|  KPer| AK1| BK1| MPer| NPer|    MXdl|    NXdl|    ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockLds|    BBlockTransfer| BBlockTransfer| BBlockTransfer| BBlockTransfer| BBlockTransfer| BBlockTransfer| BBlockLds| CShuffleMXdl| CShuffleNXdl|   CDEBlockTransfer| CDEBlockTransfer|
        // ##############################################| Spatial|        |        |           |        |  Type|  Type|    Type| DataType|       Type|  Type|    Operation|    Operation|      Operation|  DataSpecialization| GemmM| GemmN| PrefetchStage|  Size| Block| Block| Block|    |    |  XDL|  XDL| PerWave| PerWave|     ThreadCluster|  ThreadCluster| SrcAccessOrder|   SrcVectorDim|      SrcScalar|      DstScalar|    ExtraM|     ThreadCluster|  ThreadCluster| SrcAccessOrder|   SrcVectorDim|      SrcScalar|      DstScalar|    ExtraN|      PerWave|      PerWave|  _MBlock_MPerBlock|  ScalarPerVector|
        // ##############################################|        |        |        |           |        |      |      |        |         |           |      |             |             |               |                    |      |      |              |      |      |      |      |    |    |     |     |        |        | Lengths_AK0_M_AK1|   ArrangeOrder|               |               |      PerVector|  PerVector_AK1|          | Lengths_BK0_N_BK1|   ArrangeOrder|               |               |      PerVector|  PerVector_BK1|          |   PerShuffle|   PerShuffle|  _NBlock_NPerBlock|       _NPerBlock|
        // ##############################################|        |        |        |           |        |      |      |        |         |           |      |             |             |               |                    |      |      |              |      |      |      |      |    |    |     |     |        |        |                  |               |               |               |               |               |          |                  |               |               |               |               |               |          |             |             |                   |                 |
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataDefault,  true,  true,             1,   256,   256,   128,    32,   8,   8,   32,   32,       4,       2,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 32, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataDefault,  true,  true,             1,   256,   128,   256,    32,   8,   8,   32,   32,       2,       4,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 32, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataDefault,  true,  true,             1,   128,   128,   128,    32,   8,   8,   32,   32,       4,       2,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 16, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataDefault,  true,  true,             1,   256,   128,   128,    32,   8,   8,   32,   32,       2,       2,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 32, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataDefault,  true,  true,             1,   128,   128,    64,    32,   8,   8,   32,   32,       2,       2,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 32, 1, 4>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataDefault,  true,  true,             1,   128,    64,   128,    32,   8,   8,   32,   32,       2,       2,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 16, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataDefault,  true,  true,             1,    64,    64,    64,    32,   8,   8,   32,   32,       2,       2,       S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 16, 1, 4>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataDefault,  true,  true,             1,   256,   128,    64,    32,   8,   8,   32,   32,       2,       1,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 32, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataDefault,  true,  true,             1,   256,    64,   128,    32,   8,   8,   32,   32,       1,       2,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 32, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataDefault,  true,  true,             1,   128,   128,    32,    32,   8,   8,   32,   32,       2,       1,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 32, 1, 4>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataDefault,  true,  true,             1,   128,    32,   128,    32,   8,   8,   32,   32,       1,       2,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 16, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataDefault,  true,  true,             1,    64,    64,    32,    32,   8,   8,   32,   32,       2,       1,       S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 16, 1, 4>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataDefault,  true,  true,             1,    64,    32,    64,    32,   8,   8,   32,   32,       1,       2,       S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 16, 1, 4>,                8>,

        // 2. Filter1x1Stride1Pad0
        // ##############################################|    NDim| ALayout| BLayout|   DsLayout| ELayout| AData| BData| AccData| CShuffle|     DsData| EData| AElementwise| BElementwise| CDEElementwise|              ConvolutionBackward| DoPad| DoPad|      NumGemmK| Block|  MPer|  NPer|  KPer| AK1| BK1| MPer| NPer|    MXdl|    NXdl|    ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockLds|    BBlockTransfer| BBlockTransfer| BBlockTransfer| BBlockTransfer| BBlockTransfer| BBlockTransfer| BBlockLds| CShuffleMXdl| CShuffleNXdl|   CDEBlockTransfer| CDEBlockTransfer|
        // ##############################################| Spatial|        |        |           |        |  Type|  Type|    Type| DataType|       Type|  Type|    Operation|    Operation|      Operation|               DataSpecialization| GemmM| GemmN| PrefetchStage|  Size| Block| Block| Block|    |    |  XDL|  XDL| PerWave| PerWave|     ThreadCluster|  ThreadCluster| SrcAccessOrder|   SrcVectorDim|      SrcScalar|      DstScalar|    ExtraM|     ThreadCluster|  ThreadCluster| SrcAccessOrder|   SrcVectorDim|      SrcScalar|      DstScalar|    ExtraN|      PerWave|      PerWave|  _MBlock_MPerBlock|  ScalarPerVector|
        // ##############################################|        |        |        |           |        |      |      |        |         |           |      |             |             |               |                                 |      |      |              |      |      |      |      |    |    |     |     |        |        | Lengths_AK0_M_AK1|   ArrangeOrder|               |               |      PerVector|  PerVector_AK1|          | Lengths_BK0_N_BK1|   ArrangeOrder|               |               |      PerVector|  PerVector_BK1|          |   PerShuffle|   PerShuffle|  _NBlock_NPerBlock|       _NPerBlock|
        // ##############################################|        |        |        |           |        |      |      |        |         |           |      |             |             |               |                                 |      |      |              |      |      |      |      |    |    |     |     |        |        |                  |               |               |               |               |               |          |                  |               |               |               |               |               |          |             |             |                   |                 |
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,  true,  true,             1,   256,   256,   128,    32,   8,   8,   32,   32,       4,       2,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 32, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,  true,  true,             1,   256,   128,   256,    32,   8,   8,   32,   32,       2,       4,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 32, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,  true,  true,             1,   128,   128,   128,    32,   8,   8,   32,   32,       4,       2,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 16, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,  true,  true,             1,   256,   128,   128,    32,   8,   8,   32,   32,       2,       2,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 32, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,  true,  true,             1,   128,   128,    64,    32,   8,   8,   32,   32,       2,       2,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 32, 1, 4>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,  true,  true,             1,   128,    64,   128,    32,   8,   8,   32,   32,       2,       2,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 16, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,  true,  true,             1,    64,    64,    64,    32,   8,   8,   32,   32,       2,       2,       S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 16, 1, 4>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,  true,  true,             1,   256,   128,    64,    32,   8,   8,   32,   32,       2,       1,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 32, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,  true,  true,             1,   256,    64,   128,    32,   8,   8,   32,   32,       1,       2,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 32, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,  true,  true,             1,   128,   128,    32,    32,   8,   8,   32,   32,       2,       1,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 32, 1, 4>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,  true,  true,             1,   128,    32,   128,    32,   8,   8,   32,   32,       1,       2,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 16, 1, 8>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,  true,  true,             1,    64,    64,    32,    32,   8,   8,   32,   32,       2,       1,       S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 16, 1, 4>,                8>,
        DeviceGroupedConvBwdDataMultipleD_Xdl_CShuffle_v1<       2,   GNHWK,   GKYXC, EmptyTuple,   GNHWC,   F16,   F16,     F32,      F16, EmptyTuple,   F16,  PassThrough,  PassThrough,    PassThrough,  ConvBwdDataFilter1x1Stride1Pad0,  true,  true,             1,    64,    32,    64,    32,   8,   8,   32,   32,       1,       2,       S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,         1,       S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              1,              8,              8,         1,            1,            1,     S<1, 16, 1, 4>,                8>
    // clang-format on
    >;

void add_device_grouped_conv2d_bwd_data_xdl_gnhwc_gkyxc_gnhwk_f16_instances(
    std::vector<std::unique_ptr<DeviceGroupedConvBwdData<2,
                                                  GNHWC,
                                                  GKYXC,
                                                  GNHWK,
                                                  F16,
                                                  F16,
                                                  F16,
                                                  PassThrough,
                                                  PassThrough,
                                                  PassThrough>>>& instances)
{
    add_device_operation_instances(instances,
                                   device_grouped_conv2d_bwd_data_xdl_gnhwc_gkyxc_gnhwk_f16_instances{});
}

} // namespace instance
} // namespace device
} // namespace tensor_operation
} // namespace ck
