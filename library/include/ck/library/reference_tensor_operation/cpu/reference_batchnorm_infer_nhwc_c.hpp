#ifndef REFERENCE_BATCHNORM_INFER_NHWC_C_HPP
#define REFERENCE_BATCHNORM_INFER_NHWC_C_HPP

#include <iostream>
#include <sstream>
#include <algorithm>
#include "device_batchnorm_infer.hpp"

namespace ck {
namespace tensor_operation {
namespace host {

template <typename InOutDataType, typename AccDataType>
struct ReferenceBatchNormInfer_Input_N_H_W_C_Output_C : public device::DeviceBatchNormInfer
{
    struct Argument : public device::BaseArgument
    {
        Argument(const std::vector<index_t> xyLengths,
                 const std::vector<index_t> xStrides,
                 const std::vector<index_t> yStrides,
                 const std::vector<index_t> bnScaleBiasMeanVarLengths,
                 const std::vector<index_t> bnScaleBiasMeanVarStrides,
                 const InOutDataType* p_x,
                 const AccDataType* bnScale,
                 const AccDataType* bnBias,
                 double epsilon,
                 const AccDataType* estimatedMean,
                 const AccDataType* estimatedVariance,
                 InOutDataType* p_y)
            : p_x_(p_x),
              bnScale_(bnScale),
              bnBias_(bnBias),
              epsilon_(epsilon),
              estimatedMean_(estimatedMean),
              estimatedVariance_(estimatedVariance),
              p_y_(p_y)
        {
            (void)xStrides;
            (void)yStrides;
            (void)bnScaleBiasMeanVarStrides;

            if(xyLengths.size() != 4 || bnScaleBiasMeanVarLengths.size() != 1 ||
               bnScaleBiasMeanVarLengths[0] != xyLengths[3])
                throw std::runtime_error("Invalid tensor dimensions!");

            n = xyLengths[0];
            h = xyLengths[1];
            w = xyLengths[2];
            c = xyLengths[3];
        }

        const InOutDataType* p_x_;
        const AccDataType* bnScale_;
        const AccDataType* bnBias_;

        double epsilon_;

        const AccDataType* estimatedMean_;
        const AccDataType* estimatedVariance_;

        InOutDataType* p_y_;

        index_t n, h, w, c;
    };

    struct Invoker : public device::BaseInvoker
    {
        float Run(const Argument& arg)
        {
            auto thread_reduce_func = [&](auto iC) {
                index_t offset_C     = iC;
                AccDataType mean     = arg.estimatedMean_[offset_C];
                AccDataType variance = arg.estimatedVariance_[offset_C];

                AccDataType invVariance =
                    type_convert<AccDataType>(1.0f) /
                    std::sqrt(type_convert<AccDataType>(arg.epsilon_) + variance);

                // Normalization
                for(index_t iN = 0; iN < arg.n; iN++)
                {
                    index_t offset_N = iN * arg.h * arg.w * arg.c;
                    for(index_t iH = 0; iH < arg.h; iH++)
                    {
                        index_t offset_H = iH * arg.w * arg.c;
                        for(index_t iW = 0; iW < arg.w; iW++)
                        {
                            index_t offset_W = iW * arg.c;

                            auto offset = offset_N + offset_H + offset_W + offset_C;

                            AccDataType x = type_convert<AccDataType>(arg.p_x_[offset]);

                            AccDataType norm_x =
                                arg.bnScale_[iC] * (x - mean) * invVariance + arg.bnBias_[iC];

                            arg.p_y_[offset] = type_convert<InOutDataType>(norm_x);
                        };
                    }
                };
            };

            std::size_t num_thread      = std::thread::hardware_concurrency();
            std::size_t work_per_thread = (arg.c + num_thread - 1) / num_thread;

            std::vector<joinable_thread> threads(num_thread);

            for(std::size_t it = 0; it < num_thread; ++it)
            {
                std::size_t ic_begin = it * work_per_thread;
                std::size_t ic_end = std::min(static_cast<int>((it + 1) * work_per_thread), arg.c);

                auto f = [=] {
                    for(std::size_t ic = ic_begin; ic < ic_end; ++ic)
                    {
                        thread_reduce_func(ic);
                    }
                };

                threads[it] = joinable_thread(f);
            }

            return (0.0f);
        };

        float Run(const device::BaseArgument* p_arg,
                  const StreamConfig& /*stream_config*/ = StreamConfig{}) override
        {
            return Run(*dynamic_cast<const Argument*>(p_arg));
        };
    };

    bool IsSupportedArgument(const device::BaseArgument* p_arg) override
    {
        (void)p_arg;

        return (true);
    };

    std::unique_ptr<device::BaseArgument>
    MakeArgumentPointer(const std::vector<index_t> xyLengths,
                        const std::vector<index_t> xStrides,
                        const std::vector<index_t> yStrides,
                        const std::vector<index_t> bnScaleBiasMeanVarLengths,
                        const std::vector<index_t> bnScaleBiasMeanVarStrides,
                        const void* p_x,
                        const void* bnScale,
                        const void* bnBias,
                        double epsilon,
                        const void* estimatedMean,
                        const void* estimatedVariance,
                        void* p_y) override
    {
        return std::make_unique<Argument>(xyLengths,
                                          xStrides,
                                          yStrides,
                                          bnScaleBiasMeanVarLengths,
                                          bnScaleBiasMeanVarStrides,
                                          static_cast<const InOutDataType*>(p_x),
                                          static_cast<const AccDataType*>(bnScale),
                                          static_cast<const AccDataType*>(bnBias),
                                          epsilon,
                                          static_cast<const AccDataType*>(estimatedMean),
                                          static_cast<const AccDataType*>(estimatedVariance),
                                          static_cast<InOutDataType*>(p_y));
    };

    std::unique_ptr<device::BaseInvoker> MakeInvokerPointer() override
    {
        return std::make_unique<Invoker>();
    };

    std::string GetTypeString() const override
    {
        auto str = std::stringstream();

        // clang-format off
        str << "Reference_BatchNorm_Forward_NHWC_C<" << std::endl;
        // clang-format on

        return str.str();
    }
};

} // namespace host
} // namespace tensor_operation
} // namespace ck
#endif