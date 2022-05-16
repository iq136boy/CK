#pragma once
#include "config.hpp"
#include "device.hpp"
#include "host_tensor.hpp"
#include "host_tensor_generator.hpp"
#include "tensor_layout.hpp"
#include "device_tensor.hpp"
#include "device_convnd_fwd_avx2_nhwc_kyxc_nhwk.hpp"
#include "element_wise_operation_cpu.hpp"
#include "reference_conv_fwd.hpp"

namespace ck {
namespace tensor_operation {
namespace cpu {
namespace device {
namespace device_conv2d_fwd_avx2_instance {

void add_device_conv2d_fwd_avx2_nhwc_kyxc_nhwk(
    std::vector<DeviceConvFwdPtr<PassThrough, PassThrough, PassThrough>>& instances);

void add_device_conv2d_fwd_avx2_nhwc_kyxc_nhwk_local_c(
    std::vector<DeviceConvFwdPtr<PassThrough, PassThrough, PassThrough>>& instances);

void add_device_conv2d_fwd_avx2_nhwc_kyxc_nhwk_mt(
    std::vector<DeviceConvFwdPtr<PassThrough, PassThrough, PassThrough>>& instances);

void add_device_conv2d_fwd_avx2_nhwc_kyxc_nhwk_relu(
    std::vector<DeviceConvFwdPtr<PassThrough, PassThrough, Relu>>& instances);

void add_device_conv2d_fwd_avx2_nhwc_kyxc_nhwk_local_c_relu(
    std::vector<DeviceConvFwdPtr<PassThrough, PassThrough, Relu>>& instances);

void add_device_conv2d_fwd_avx2_nhwc_kyxc_nhwk_mt_relu(
    std::vector<DeviceConvFwdPtr<PassThrough, PassThrough, Relu>>& instances);

} // namespace device_conv2d_fwd_avx2_instance
} // namespace device
} // namespace cpu
} // namespace tensor_operation
} // namespace ck

namespace ck {
namespace profiler {

#define AVX2_DATA_ALIGNMENT

template <int NDimSpatial,
          typename InDataType,
          typename WeiDataType,
          typename OutDataType,
          typename InLayout,
          typename WeiLayout,
          typename OutLayout>
void profile_conv_cpu_fwd_impl(int do_verification,
                               int init_method,
                               bool do_log,
                               int nrepeat,
                               ck::index_t N,
                               ck::index_t K,
                               ck::index_t C,
                               std::vector<ck::index_t> input_spatial_lengths,
                               std::vector<ck::index_t> filter_spatial_lengths,
                               std::vector<ck::index_t> output_spatial_lengths,
                               std::vector<ck::index_t> conv_filter_strides,
                               std::vector<ck::index_t> conv_filter_dilations,
                               std::vector<ck::index_t> input_left_pads,
                               std::vector<ck::index_t> input_right_pads)
{
    const ck::index_t Y = filter_spatial_lengths[0];
    const ck::index_t X = filter_spatial_lengths[1];

    const ck::index_t Hi = input_spatial_lengths[0];
    const ck::index_t Wi = input_spatial_lengths[1];

    const ck::index_t Ho = output_spatial_lengths[0];
    const ck::index_t Wo = output_spatial_lengths[1];

    auto f_host_tensor_descriptor =
        [](std::size_t N_, std::size_t C_, std::size_t H, std::size_t W, auto layout) {
            if constexpr(is_same<decltype(layout), ck::tensor_layout::convolution::NCHW>::value ||
                         is_same<decltype(layout), ck::tensor_layout::convolution::KCYX>::value ||
                         is_same<decltype(layout), ck::tensor_layout::convolution::NKHW>::value)
            {
                return HostTensorDescriptor(std::vector<std::size_t>({N_, C_, H, W}),
                                            std::vector<std::size_t>({C_ * H * W, H * W, W, 1}));
            }
            else if constexpr(is_same<decltype(layout), tensor_layout::convolution::NHWC>::value ||
                              is_same<decltype(layout), tensor_layout::convolution::KYXC>::value ||
                              is_same<decltype(layout), tensor_layout::convolution::NHWK>::value)
            {
                return HostTensorDescriptor(std::vector<std::size_t>({N_, C_, H, W}),
                                            std::vector<std::size_t>({C_ * H * W, 1, W * C_, C_}));
            }
        };

    Tensor<InDataType> in_n_c_hi_wi(f_host_tensor_descriptor(N, C, Hi, Wi, InLayout{}));
    Tensor<WeiDataType> wei_k_c_y_x(f_host_tensor_descriptor(K, C, Y, X, WeiLayout{}));
    Tensor<OutDataType> out_n_k_ho_wo_host_result(
        f_host_tensor_descriptor(N, K, Ho, Wo, OutLayout{}));
    Tensor<OutDataType> out_n_k_ho_wo_device_result(
        f_host_tensor_descriptor(N, K, Ho, Wo, OutLayout{}));

    std::cout << "in_n_c_hi_wi: " << in_n_c_hi_wi.mDesc << std::endl;
    std::cout << "wei_k_c_y_x: " << wei_k_c_y_x.mDesc << std::endl;
    std::cout << "out_n_k_ho_wo: " << out_n_k_ho_wo_host_result.mDesc << std::endl;

    switch(init_method)
    {
    case 0: break;
    case 1:
        in_n_c_hi_wi.GenerateTensorValue(GeneratorTensor_2<InDataType>{-5, 5});
        wei_k_c_y_x.GenerateTensorValue(GeneratorTensor_2<WeiDataType>{-5, 5});
        break;
    default:
        in_n_c_hi_wi.GenerateTensorValue(GeneratorTensor_3<InDataType>{0.0, 1.0});
        wei_k_c_y_x.GenerateTensorValue(GeneratorTensor_3<WeiDataType>{-0.5, 0.5});
    }

    using InElementOp  = ck::tensor_operation::cpu::element_wise::PassThrough;
    using WeiElementOp = ck::tensor_operation::cpu::element_wise::PassThrough;
    using OutElementOp = ck::tensor_operation::cpu::element_wise::PassThrough;

    const auto in_element_op  = InElementOp{};
    const auto wei_element_op = WeiElementOp{};
    const auto out_element_op = OutElementOp{};

    if(do_verification)
    {
        using ReferenceConvFwdInstance = ck::tensor_operation::host::ReferenceConvFwd<InDataType,
                                                                                      WeiDataType,
                                                                                      OutDataType,
                                                                                      InElementOp,
                                                                                      WeiElementOp,
                                                                                      OutElementOp>;

        auto ref_conv     = ReferenceConvFwdInstance{};
        auto ref_invoker  = ref_conv.MakeInvoker();
        auto ref_argument = ref_conv.MakeArgument(in_n_c_hi_wi,
                                                  wei_k_c_y_x,
                                                  out_n_k_ho_wo_host_result,
                                                  conv_filter_strides,
                                                  conv_filter_dilations,
                                                  input_left_pads,
                                                  input_right_pads,
                                                  in_element_op,
                                                  wei_element_op,
                                                  out_element_op);

        ref_invoker.Run(ref_argument);
    }

    DeviceAlignedMemCPU in_device_buf(sizeof(InDataType) * in_n_c_hi_wi.mDesc.GetElementSpace(),
                                      AVX2_DATA_ALIGNMENT);
    DeviceAlignedMemCPU wei_device_buf(sizeof(WeiDataType) * wei_k_c_y_x.mDesc.GetElementSpace(),
                                       AVX2_DATA_ALIGNMENT);
    DeviceAlignedMemCPU out_device_buf(sizeof(OutDataType) *
                                           out_n_k_ho_wo_device_result.mDesc.GetElementSpace(),
                                       AVX2_DATA_ALIGNMENT);

    in_device_buf.ToDevice(in_n_c_hi_wi.mData.data());
    wei_device_buf.ToDevice(wei_k_c_y_x.mData.data());

    memcpy(in_device_buf.mpDeviceBuf, in_n_c_hi_wi.mData.data(), in_device_buf.mMemSize);
    memcpy(wei_device_buf.mpDeviceBuf, wei_k_c_y_x.mData.data(), wei_device_buf.mMemSize);

    using PassThrough = ck::tensor_operation::cpu::element_wise::PassThrough;

    using DeviceConvFwdNoOpPtr =
        ck::tensor_operation::device::DeviceConvFwdPtr<PassThrough, PassThrough, PassThrough>;

    // add device Conv instances
    std::vector<DeviceConvFwdNoOpPtr> conv_ptrs;

    ck::tensor_operation::cpu::device::device_conv2d_fwd_instance::
        add_device_conv2d_fwd_avx2_nhwc_kyxc_nhwk(conv_ptrs);

    if(conv_ptrs.size() <= 0)
    {
        throw std::runtime_error("wrong! no device Conv instance found");
    }

    std::string best_conv_name;
    float best_ave_time   = 0;
    float best_gflops     = 0;
    float best_gb_per_sec = 0;

    // profile device Conv instances
    for(auto& conv_ptr : conv_ptrs)
    {
        auto argument_ptr = conv_ptr->MakeArgumentPointer(
            static_cast<InDataType*>(in_device_buf.GetDeviceBuffer()),
            static_cast<WeiDataType*>(wei_device_buf.GetDeviceBuffer()),
            static_cast<OutDataType*>(out_device_buf.GetDeviceBuffer()),
            N,
            K,
            C,
            input_spatial_lengths,
            filter_spatial_lengths,
            output_spatial_lengths,
            conv_filter_strides,
            conv_filter_dilations,
            input_left_pads,
            input_right_pads,
            in_element_op,
            wei_element_op,
            out_element_op);

        auto invoker_ptr = conv_ptr->MakeInvokerPointer();

        if(conv_ptr->IsSupportedArgument(argument_ptr.get()))
        {
            std::string conv_name = conv_ptr->GetTypeString();

            float ave_time = invoker_ptr->Run(argument_ptr.get(), nrepeat);

            std::size_t flop = std::size_t(2) * N * K * Ho * Wo * C * Y * X;

            std::size_t num_btype = sizeof(InDataType) * (N * C * Hi * Wi) +
                                    sizeof(WeiDataType) * (K * C * Y * X) +
                                    sizeof(OutDataType) * (N * K * Ho * Wo);

            float gflops = static_cast<float>(flop) / 1.E6 / ave_time;

            float gb_per_sec = num_btype / 1.E6 / ave_time;

            std::cout << "Perf: " << ave_time << " ms, " << gflops << " GFlops, " << gb_per_sec
                      << " GB/s, " << conv_name << std::endl;

            if(gflops > best_gflops)
            {
                best_conv_name  = conv_name;
                best_gflops     = gflops;
                best_ave_time   = ave_time;
                best_gb_per_sec = gb_per_sec;
            }

            if(do_verification)
            {
                memcpy(out_n_k_ho_wo_device_result.mData.data(),
                       out_device_buf.mpDeviceBuf,
                       out_device_buf.mMemSize);

                check_error(out_n_k_ho_wo_host_result, out_n_k_ho_wo_device_result);

                if(do_log)
                {
                    LogRangeAsType<float>(std::cout << "in : ", in_n_c_hi_wi.mData, ",")
                        << std::endl;
                    LogRangeAsType<float>(std::cout << "wei: ", wei_k_c_y_x.mData, ",")
                        << std::endl;
                    LogRangeAsType<float>(
                        std::cout << "out_host  : ", out_n_k_ho_wo_host_result.mData, ",")
                        << std::endl;
                    LogRangeAsType<float>(
                        std::cout << "out_device: ", out_n_k_ho_wo_device_result.mData, ",")
                        << std::endl;
                }
            }
        }
    }

    std::cout << "Best Perf: " << best_ave_time << " ms, " << best_gflops << " GFlops, "
              << best_gb_per_sec << " GB/s, " << best_conv_name << std::endl;
}

} // namespace profiler
} // namespace ck