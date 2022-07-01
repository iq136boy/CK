// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2022, Advanced Micro Devices, Inc. All rights reserved.

#include <cstdlib>

#include "ck/ck.hpp"
#include "ck/utility/reduction_operator.hpp"
#include "ck/tensor_operation/gpu/device/tensor_layout.hpp"
#include "ck/tensor_operation/gpu/device/gemm_specialization.hpp"
#include "ck/tensor_operation/gpu/device/device_gemm_bias_add_reduce_xdl_cshuffle.hpp"
#include "ck/tensor_operation/gpu/element/element_wise_operation.hpp"
#include "ck/library/tensor_operation_instance/add_device_operation_instance.hpp"

namespace ck {
namespace tensor_operation {
namespace device {
namespace instance {

using F16              = ck::half_t;
using F32              = float;
using ReducePtrsGlobal = ck::Tuple<F32*, F32*>;

using Row = ck::tensor_layout::gemm::RowMajor;
using Col = ck::tensor_layout::gemm::ColumnMajor;

template <ck::index_t... Is>
using S = ck::Sequence<Is...>;

using PassThrough = ck::tensor_operation::element_wise::PassThrough;
using ReduceSum   = ck::reduce::Add;
using ReduceOps   = ck::Tuple<ReduceSum, ReduceSum>;

using Div                 = ck::tensor_operation::element_wise::UnaryDivide;
using Identity            = ck::tensor_operation::element_wise::PassThrough;
using Square              = ck::tensor_operation::element_wise::UnarySquare;
using ReduceInElementOps  = ck::Tuple<Identity, Square>;
using ReduceOutElementOps = ck::Tuple<Div, Div>;

using ReduceMemOp = ck::InMemoryDataOperationEnumSequence<ck::InMemoryDataOperationEnum::AtomicAdd,
                                                          ck::InMemoryDataOperationEnum::AtomicAdd>;

static constexpr auto GemmDefault = ck::tensor_operation::device::GemmSpecialization::Default;

// c[m, n] = a[m, k] * b[n, k]
using device_gemm_bias_add_mean_squaremean_xdl_cshuffle_f16_f16_f16_f16_f16_f32_f32_mk_nk_mn_instances =
    std::tuple<
        // clang-format off
        //##################################| ALayout| BLayout| CLayout|AData| BData| CData|C0Data|C1Data|  GemmAcc| CShuffle| ReduceAcc|         DData|           A|           B|           C|          C1|         Dxs|    DxsInEleOp|    DxsAccEleOp|           D|           GEMM| NumGemmK| Block|  MPer|  NPer|  KPer| AK1| BK1| MPer| NPer| MXdl| NXdl|  ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockTransfer| ABlockLds|  BBlockTransfer| BBlockTransfer| BBlockTransfer| BlockTransfer| BBlockTransfer| BBlockTransfer| BBlockLds|    CShuffle|    CShuffle| CBlockTransferClusterLengths|  CBlockTransfer|              CReduce| CReduceThreadLds2VGprCopy| CReduceThreadVgpr2GlobalCopy|
        //##################################|        |        |        | Type|  Type|  Type|  Type|  Type| DataType| DataType|  DataType|    Type Tuple| Elementwise| Elementwise| Elementwise| Elementwise|      Reduce|              |               |  MemoryData| Spacialization| Prefetch|  Size| Block| Block| Block|    |    |  XDL|  XDL|  Per|  Per|   ThreadCluster|  ThreadCluster| SrcAccessOrder|   SrcVectorDim|      SrcScalar|      DstScalar| AddExtraM|   ThreadCluster|  ThreadCluster| SrcAccessOrder|  SrcVectorDim|      SrcScalar|      DstScalar| AddExtraN| MXdlPerWave| NXdlPerWave|            _MBlock_MPerBlock| ScalarPerVector| ThreadClusterLengths|     SrcDstScalarPerVector|        SrcDstScalarPerVector|
        //##################################|        |        |        |     |      |      |      |      |         |         |          |              |   Operation|   Operation|   Operation|   Operation|   Operation|              |               |   Operation|               |    Stage|      |      |      |      |    |    |     |     | Wave| Wave| Lengths_K0_M_K1|   ArrangeOrder|               |               |      PerVector|   PerVector_K1|          | Lengths_K0_N_K1|   ArrangeOrder|               |              |      PerVector|   PerVector_K1|          |  PerShuffle|  PerShuffle|            _NBlock_NPerBlock|      _NPerBlock| _MPerBlock_NPerBlock|                _NPerBlock|                   _MPerBlock|
        //##################################|        |        |        |     |      |      |      |      |         |         |          |              |            |            |            |            |            |              |               |            |               |         |      |      |      |      |    |    |     |     |     |     |                |               |               |               |               |               |          |                |               |               |              |               |               |          |            |            |                             |                |                     |                          |                             |
        DeviceGemmBiasAddReduce_Xdl_CShuffle<     Row,     Col,     Row,  F16,   F16,   F16,   F16,   F16,      F32,      F32,       F32,   ReducePtrsGlobal, PassThrough, PassThrough, PassThrough, PassThrough,   ReduceOps, ReduceInElementOps, ReduceOutElementOps, ReduceMemOp,    GemmDefault,        1,   256,   256,   128,    32,   8,   8,   32,   32,    4,    2,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,      true,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,      true,           1,           1,               S<1, 32, 1, 8>,               8,             S<64, 4>,                         4,                            1>,
        DeviceGemmBiasAddReduce_Xdl_CShuffle<     Row,     Col,     Row,  F16,   F16,   F16,   F16,   F16,      F32,      F32,       F32,   ReducePtrsGlobal, PassThrough, PassThrough, PassThrough, PassThrough,   ReduceOps, ReduceInElementOps, ReduceOutElementOps, ReduceMemOp,    GemmDefault,        1,   256,   128,   256,    32,   8,   8,   32,   32,    2,    4,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,      true,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,      true,           1,           1,               S<1, 32, 1, 8>,               8,             S<64, 4>,                         4,                            1>,
        DeviceGemmBiasAddReduce_Xdl_CShuffle<     Row,     Col,     Row,  F16,   F16,   F16,   F16,   F16,      F32,      F32,       F32,   ReducePtrsGlobal, PassThrough, PassThrough, PassThrough, PassThrough,   ReduceOps, ReduceInElementOps, ReduceOutElementOps, ReduceMemOp,    GemmDefault,        1,   128,   128,   128,    32,   8,   8,   32,   32,    4,    2,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,      true,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,      true,           1,           1,               S<1, 16, 1, 8>,               8,             S<32, 4>,                         4,                            1>,
        DeviceGemmBiasAddReduce_Xdl_CShuffle<     Row,     Col,     Row,  F16,   F16,   F16,   F16,   F16,      F32,      F32,       F32,   ReducePtrsGlobal, PassThrough, PassThrough, PassThrough, PassThrough,   ReduceOps, ReduceInElementOps, ReduceOutElementOps, ReduceMemOp,    GemmDefault,        1,   256,   128,   128,    32,   8,   8,   32,   32,    2,    2,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,      true,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,      true,           1,           1,               S<1, 32, 1, 8>,               8,             S<64, 4>,                         4,                            1>,
        DeviceGemmBiasAddReduce_Xdl_CShuffle<     Row,     Col,     Row,  F16,   F16,   F16,   F16,   F16,      F32,      F32,       F32,   ReducePtrsGlobal, PassThrough, PassThrough, PassThrough, PassThrough,   ReduceOps, ReduceInElementOps, ReduceOutElementOps, ReduceMemOp,    GemmDefault,        1,   128,   128,    64,    32,   8,   8,   32,   32,    2,    2,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,      true,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,      true,           1,           1,               S<1, 32, 1, 4>,               8,             S<64, 2>,                         4,                            1>,
        DeviceGemmBiasAddReduce_Xdl_CShuffle<     Row,     Col,     Row,  F16,   F16,   F16,   F16,   F16,      F32,      F32,       F32,   ReducePtrsGlobal, PassThrough, PassThrough, PassThrough, PassThrough,   ReduceOps, ReduceInElementOps, ReduceOutElementOps, ReduceMemOp,    GemmDefault,        1,   128,    64,   128,    32,   8,   8,   32,   32,    2,    2,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,      true,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,      true,           1,           1,               S<1, 16, 1, 8>,               8,             S<32, 4>,                         4,                            1>,
        DeviceGemmBiasAddReduce_Xdl_CShuffle<     Row,     Col,     Row,  F16,   F16,   F16,   F16,   F16,      F32,      F32,       F32,   ReducePtrsGlobal, PassThrough, PassThrough, PassThrough, PassThrough,   ReduceOps, ReduceInElementOps, ReduceOutElementOps, ReduceMemOp,    GemmDefault,        1,    64,    64,    64,    32,   8,   8,   32,   32,    2,    2,     S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,      true,     S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,      true,           1,           1,               S<1, 16, 1, 4>,               8,             S<32, 2>,                         4,                            1>,
        DeviceGemmBiasAddReduce_Xdl_CShuffle<     Row,     Col,     Row,  F16,   F16,   F16,   F16,   F16,      F32,      F32,       F32,   ReducePtrsGlobal, PassThrough, PassThrough, PassThrough, PassThrough,   ReduceOps, ReduceInElementOps, ReduceOutElementOps, ReduceMemOp,    GemmDefault,        1,   256,   128,    64,    32,   8,   8,   32,   32,    2,    1,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,      true,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,      true,           1,           1,               S<1, 32, 1, 8>,               8,             S<64, 4>,                         4,                            1>,
        DeviceGemmBiasAddReduce_Xdl_CShuffle<     Row,     Col,     Row,  F16,   F16,   F16,   F16,   F16,      F32,      F32,       F32,   ReducePtrsGlobal, PassThrough, PassThrough, PassThrough, PassThrough,   ReduceOps, ReduceInElementOps, ReduceOutElementOps, ReduceMemOp,    GemmDefault,        1,   256,    64,   128,    32,   8,   8,   32,   32,    1,    2,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,      true,     S<4, 64, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,      true,           1,           1,               S<1, 32, 1, 8>,               8,             S<64, 4>,                         4,                            1>,
        DeviceGemmBiasAddReduce_Xdl_CShuffle<     Row,     Col,     Row,  F16,   F16,   F16,   F16,   F16,      F32,      F32,       F32,   ReducePtrsGlobal, PassThrough, PassThrough, PassThrough, PassThrough,   ReduceOps, ReduceInElementOps, ReduceOutElementOps, ReduceMemOp,    GemmDefault,        1,   128,   128,    32,    32,   8,   8,   32,   32,    2,    1,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,      true,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,      true,           1,           1,               S<1, 32, 1, 4>,               8,             S<64, 2>,                         4,                            1>,
        DeviceGemmBiasAddReduce_Xdl_CShuffle<     Row,     Col,     Row,  F16,   F16,   F16,   F16,   F16,      F32,      F32,       F32,   ReducePtrsGlobal, PassThrough, PassThrough, PassThrough, PassThrough,   ReduceOps, ReduceInElementOps, ReduceOutElementOps, ReduceMemOp,    GemmDefault,        1,   128,    32,   128,    32,   8,   8,   32,   32,    1,    2,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,      true,     S<4, 32, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,      true,           1,           1,               S<1, 16, 1, 8>,               8,             S<32, 4>,                         4,                            1>,
        DeviceGemmBiasAddReduce_Xdl_CShuffle<     Row,     Col,     Row,  F16,   F16,   F16,   F16,   F16,      F32,      F32,       F32,   ReducePtrsGlobal, PassThrough, PassThrough, PassThrough, PassThrough,   ReduceOps, ReduceInElementOps, ReduceOutElementOps, ReduceMemOp,    GemmDefault,        1,    64,    64,    32,    32,   8,   8,   32,   32,    2,    1,     S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,      true,     S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,      true,           1,           1,               S<1, 16, 1, 4>,               8,             S<32, 2>,                         4,                            1>,
        DeviceGemmBiasAddReduce_Xdl_CShuffle<     Row,     Col,     Row,  F16,   F16,   F16,   F16,   F16,      F32,      F32,       F32,   ReducePtrsGlobal, PassThrough, PassThrough, PassThrough, PassThrough,   ReduceOps, ReduceInElementOps, ReduceOutElementOps, ReduceMemOp,    GemmDefault,        1,    64,    32,    64,    32,   8,   8,   32,   32,    1,    2,     S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,              2,              8,              8,      true,     S<4, 16, 1>,     S<1, 0, 2>,     S<1, 0, 2>,             2,              8,              8,      true,           1,           1,               S<1, 16, 1, 4>,               8,             S<32, 2>,                         4,                            1>
        // clang-format on
        >;

void add_device_gemm_bias_add_mean_squaremean_xdl_cshuffle_f16_f16_f16_f16_f16_f32_f32_mk_nk_mn_instances(
    std::vector<DeviceGemmReducePtr<1, ReduceOps::Size()>>& instances)
{
    add_device_operation_instances(
        instances,
        device_gemm_bias_add_mean_squaremean_xdl_cshuffle_f16_f16_f16_f16_f16_f32_f32_mk_nk_mn_instances{});
}

} // namespace instance
} // namespace device
} // namespace tensor_operation
} // namespace ck
