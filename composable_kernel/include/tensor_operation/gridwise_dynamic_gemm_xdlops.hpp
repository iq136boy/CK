#ifndef CK_GRIDWISE_DYNAMIC_GEMM_XDLOPS_HPP
#define CK_GRIDWISE_DYNAMIC_GEMM_XDLOPS_HPP

#include "common_header.hpp"
#include "dynamic_multi_index_transform_helper.hpp"
#include "dynamic_tensor_descriptor.hpp"
#include "dynamic_tensor_descriptor_helper.hpp"
#include "blockwise_gemm_v2.hpp"
#include "blockwise_dynamic_tensor_slice_transfer.hpp"
#include "threadwise_dynamic_tensor_slice_transfer.hpp"
#include "threadwise_dynamic_tensor_slice_set.hpp"

namespace ck {

template <typename GridwiseGemm,
          typename AGlobalDesc,
          typename FloatA,
          typename BGlobalDesc,
          typename FloatB,
          typename CGlobalDesc,
          typename FloatC,
          typename CBlockClusterDesc,
          bool HasMainKBlockLoop,
          bool HasDoubleTailKBlockLoop>
__global__ void kernel_dynamic_gemm_xdlops_v1(const AGlobalDesc a_k_m_global_desc,
                                              const FloatA* __restrict__ p_a_global,
                                              const BGlobalDesc b_k_n_global_desc,
                                              const FloatB* __restrict__ p_b_global,
                                              const CGlobalDesc c_m0_m1_n0_n1_global_desc,
                                              FloatC* __restrict__ p_c_global,
                                              const CBlockClusterDesc c_block_cluster_desc)
{
    GridwiseGemm{}.Run(a_k_m_global_desc,
                       p_a_global,
                       b_k_n_global_desc,
                       p_b_global,
                       c_m0_m1_n0_n1_global_desc,
                       p_c_global,
                       c_block_cluster_desc,
                       integral_constant<bool, HasMainKBlockLoop>{},
                       integral_constant<bool, HasDoubleTailKBlockLoop>{});
}

template <index_t BlockSize,
          typename FloatAB,
          typename FloatAcc,
          typename FloatC,
          InMemoryDataOperation CGlobalMemoryDataOperation,
          typename AGlobalDesc,
          typename BGlobalDesc,
          typename CGlobalDesc,
          typename CBlockClusterDesc,
          index_t MPerBlock,
          index_t NPerBlock,
          index_t KPerBlock,
          index_t MPerThread,
          index_t NPerThread,
          index_t KPerThread,
          index_t MLevel0Cluster,
          index_t NLevel0Cluster,
          index_t MLevel1Cluster,
          index_t NLevel1Cluster,
          typename ABlockTransferThreadSliceLengths_K_M,
          typename ABlockTransferThreadClusterLengths_K_M,
          typename ABlockTransferThreadClusterArrangeOrder,
          typename ABlockTransferSrcAccessOrder,
          index_t ABlockTransferSrcVectorDim,
          index_t ABlockTransferSrcScalarPerVector,
          index_t ABlockTransferDstScalarPerVector_M,
          bool AThreadTransferSrcResetCoordinateAfterRun,
          typename BBlockTransferThreadSliceLengths_K_N,
          typename BBlockTransferThreadClusterLengths_K_N,
          typename BBlockTransferThreadClusterArrangeOrder,
          typename BBlockTransferSrcAccessOrder,
          index_t BBlockTransferSrcVectorDim,
          index_t BBlockTransferSrcScalarPerVector,
          index_t BBlockTransferDstScalarPerVector_N,
          bool BThreadTransferSrcResetCoordinateAfterRun,
          typename CThreadTransferSrcDstAccessOrder,
          index_t CThreadTransferSrcDstVectorDim,
          index_t CThreadTransferDstScalarPerVector,
          typename AGlobalIteratorHacks,
          typename BGlobalIteratorHacks,
          typename CGlobalIteratorHacks,
          typename AGlobalMoveSliceWindowIteratorHacks,
          typename BGlobalMoveSliceWindowIteratorHacks>
struct GridwiseDynamicGemm_km_kn_m0m1n0n1_xdlops_v1
{
    __host__ __device__ static constexpr index_t GetSharedMemoryNumberOfByte()
    {
        constexpr auto max_lds_align = math::lcm(Number<ABlockTransferDstScalarPerVector_M>{},
                                                 Number<BBlockTransferDstScalarPerVector_N>{},
                                                 Number<MPerThread>{},
                                                 Number<NPerThread>{});

        // A matrix in LDS memory, dst of blockwise copy
        //   be careful of LDS alignment
        constexpr auto a_k_m_block_desc = make_dynamic_naive_tensor_descriptor_aligned_v2(
            make_tuple(Number<KPerBlock>{}, Number<MPerBlock>{}), max_lds_align);

        // B matrix in LDS memory, dst of blockwise copy
        //   be careful of LDS alignment
        constexpr auto b_k_n_block_desc = make_dynamic_naive_tensor_descriptor_aligned_v2(
            make_tuple(Number<KPerBlock>{}, Number<NPerBlock>{}), max_lds_align);

        // LDS allocation for A and B: be careful of alignment
        constexpr auto a_block_space_size =
            math::integer_least_multiple(a_k_m_block_desc.GetElementSpaceSize(), max_lds_align);

        constexpr auto b_block_space_size =
            math::integer_least_multiple(b_k_n_block_desc.GetElementSpaceSize(), max_lds_align);

        return 2 * (a_block_space_size + b_block_space_size) * sizeof(FloatAB);
    }

    template <bool HasMainKBlockLoop, bool HasDoubleTailKBlockLoop>
    __device__ void Run(const AGlobalDesc& a_k_m_global_desc,
                        const FloatAB* __restrict__ p_a_global,
                        const BGlobalDesc& b_k_n_global_desc,
                        const FloatAB* __restrict__ p_b_global,
                        const CGlobalDesc& c_m0_m1_n0_n1_global_desc,
                        FloatC* __restrict__ p_c_global,
                        const CBlockClusterDesc& c_block_cluster_desc,
                        FloatAB* __restrict__ p_shared_block,
                        integral_constant<bool, HasMainKBlockLoop>,
                        integral_constant<bool, HasDoubleTailKBlockLoop>) const
    {
        constexpr auto I0 = Number<0>{};
        constexpr auto I1 = Number<1>{};
        constexpr auto I2 = Number<2>{};
        constexpr auto I3 = Number<3>{};

        const auto K = a_k_m_global_desc.GetLength(I0);
        const auto M = a_k_m_global_desc.GetLength(I1);
        const auto N = b_k_n_global_desc.GetLength(I1);

        // divide block work by [M, N]
        const auto block_work_idx =
            c_block_cluster_desc.CalculateBottomIndex(make_multi_index(get_block_1d_id()));

        // HACK: this force m/n_block_data_idx_on_global into SGPR
        const index_t m_block_data_idx_on_global =
            __builtin_amdgcn_readfirstlane(block_work_idx[I0] * MPerBlock);

        const index_t n_block_data_idx_on_global =
            __builtin_amdgcn_readfirstlane(block_work_idx[I1] * NPerBlock);

        // lds max alignment
        constexpr auto max_lds_align = math::lcm(Number<ABlockTransferDstScalarPerVector_M>{},
                                                 Number<BBlockTransferDstScalarPerVector_N>{},
                                                 Number<MPerThread>{},
                                                 Number<NPerThread>{});

        // A matrix in LDS memory, dst of blockwise copy
        //   be careful of LDS alignment
        constexpr auto a_k_m_block_desc = make_dynamic_naive_tensor_descriptor_aligned_v2(
            make_tuple(Number<KPerBlock>{}, Number<MPerBlock>{}), max_lds_align);

        // B matrix in LDS memory, dst of blockwise copy
        //   be careful of LDS alignment
        constexpr auto b_k_n_block_desc = make_dynamic_naive_tensor_descriptor_aligned_v2(
            make_tuple(Number<KPerBlock>{}, Number<NPerBlock>{}), max_lds_align);

        // A matrix blockwise copy
        auto a_blockwise_copy =
            BlockwiseDynamicTensorSliceTransfer_v4<BlockSize,
                                                   InMemoryDataOperation::Set,
                                                   Sequence<KPerBlock, MPerBlock>,
                                                   ABlockTransferThreadSliceLengths_K_M,
                                                   ABlockTransferThreadClusterLengths_K_M,
                                                   ABlockTransferThreadClusterArrangeOrder,
                                                   FloatAB,
                                                   FloatAB,
                                                   decltype(a_k_m_global_desc),
                                                   decltype(a_k_m_block_desc),
                                                   ABlockTransferSrcAccessOrder,
                                                   Sequence<0, 1>,
                                                   ABlockTransferSrcVectorDim,
                                                   1,
                                                   ABlockTransferSrcScalarPerVector,
                                                   ABlockTransferDstScalarPerVector_M,
                                                   AddressSpace::Global,
                                                   AddressSpace::Lds,
                                                   1,
                                                   1,
                                                   AThreadTransferSrcResetCoordinateAfterRun,
                                                   true>(
                a_k_m_global_desc,
                make_multi_index(0, m_block_data_idx_on_global),
                a_k_m_block_desc,
                make_multi_index(0, 0));

        // B matrix blockwise copy
        auto b_blockwise_copy =
            BlockwiseDynamicTensorSliceTransfer_v4<BlockSize,
                                                   InMemoryDataOperation::Set,
                                                   Sequence<KPerBlock, NPerBlock>,
                                                   BBlockTransferThreadSliceLengths_K_N,
                                                   BBlockTransferThreadClusterLengths_K_N,
                                                   BBlockTransferThreadClusterArrangeOrder,
                                                   FloatAB,
                                                   FloatAB,
                                                   decltype(b_k_n_global_desc),
                                                   decltype(b_k_n_block_desc),
                                                   BBlockTransferSrcAccessOrder,
                                                   Sequence<0, 1>,
                                                   BBlockTransferSrcVectorDim,
                                                   1,
                                                   BBlockTransferSrcScalarPerVector,
                                                   BBlockTransferDstScalarPerVector_N,
                                                   AddressSpace::Global,
                                                   AddressSpace::Lds,
                                                   1,
                                                   1,
                                                   BThreadTransferSrcResetCoordinateAfterRun,
                                                   true>(
                b_k_n_global_desc,
                make_multi_index(0, n_block_data_idx_on_global),
                b_k_n_block_desc,
                make_multi_index(0, 0));

        // GEMM definition
        //   c_mtx += transpose(a_mtx) * b_mtx
        //     a_mtx[KPerBlock, MPerBlock] is in LDS
        //     b_mtx[KPerBlocl, NPerBlock] is in LDS
        //     c_mtx[MPerBlock, NPerBlock] is distributed among threads, and saved in
        //       register
        // sanity check
        static_assert(MPerBlock % (MPerThread * MLevel0Cluster * MLevel1Cluster) == 0 &&
                          NPerBlock % (NPerThread * NLevel0Cluster * NLevel1Cluster) == 0,
                      "wrong!");

        constexpr index_t MRepeat = MPerBlock / (MPerThread * MLevel0Cluster * MLevel1Cluster);
        constexpr index_t NRepeat = NPerBlock / (NPerThread * NLevel0Cluster * NLevel1Cluster);

        constexpr auto a_k_m0_m1_block_desc = transform_dynamic_tensor_descriptor(
            a_k_m_block_desc,
            make_tuple(
                make_pass_through_transform(Number<KPerBlock>{}),
                make_unmerge_transform(make_tuple(
                    Number<MRepeat>{}, Number<MPerThread * MLevel0Cluster * MLevel1Cluster>{}))),
            make_tuple(Sequence<0>{}, Sequence<1>{}),
            make_tuple(Sequence<0>{}, Sequence<1, 2>{}));

        constexpr auto b_k_n0_n1_block_desc = transform_dynamic_tensor_descriptor(
            b_k_n_block_desc,
            make_tuple(
                make_pass_through_transform(Number<KPerBlock>{}),
                make_unmerge_transform(make_tuple(
                    Number<NRepeat>{}, Number<NPerThread * NLevel0Cluster * NLevel1Cluster>{}))),
            make_tuple(Sequence<0>{}, Sequence<1>{}),
            make_tuple(Sequence<0>{}, Sequence<1, 2>{}));

        constexpr auto c_m0_m1_n0_n1_thread_desc =
            make_dynamic_naive_tensor_descriptor_packed_v2(make_tuple(
                Number<MRepeat>{}, Number<MPerThread>{}, Number<NRepeat>{}, Number<NPerThread>{}));

        const auto blockwise_gemm =
            BlockwiseGemm_km0m1_kn0n1_m0m1n0n1_v2_pipeline_2x2<BlockSize,
                                                               FloatAB,
                                                               FloatAB,
                                                               FloatAcc,
                                                               decltype(a_k_m0_m1_block_desc),
                                                               decltype(b_k_n0_n1_block_desc),
                                                               decltype(c_m0_m1_n0_n1_thread_desc),
                                                               MPerThread,
                                                               NPerThread,
                                                               KPerThread,
                                                               MLevel0Cluster,
                                                               NLevel0Cluster,
                                                               MLevel1Cluster,
                                                               NLevel1Cluster,
                                                               MPerThread,
                                                               NPerThread>{};

        // LDS allocation for A and B: be careful of alignment
        constexpr auto a_block_space_size =
            math::integer_least_multiple(a_k_m_block_desc.GetElementSpaceSize(), max_lds_align);

        constexpr auto b_block_space_size =
            math::integer_least_multiple(b_k_n_block_desc.GetElementSpaceSize(), max_lds_align);

        FloatAB* p_a_block_double = p_shared_block;
        FloatAB* p_b_block_double = p_shared_block + 2 * a_block_space_size;

        // register allocation for output
        auto c_thread_buf =
            make_static_buffer<FloatAcc>(c_m0_m1_n0_n1_thread_desc.GetElementSpaceSize());

        ThreadwiseDynamicTensorSliceSet_v1<FloatAcc,
                                           decltype(c_m0_m1_n0_n1_thread_desc),
                                           Sequence<MRepeat, MPerThread, NRepeat, NPerThread>>{}
            .Run(c_m0_m1_n0_n1_thread_desc, make_tuple(I0, I0, I0, I0), c_thread_buf, FloatAcc{0});

        constexpr auto a_block_slice_copy_step = make_multi_index(KPerBlock, 0);
        constexpr auto b_block_slice_copy_step = make_multi_index(KPerBlock, 0);

        // hack to control index calculation when iterating over A and B matrix for threadwise copy
        constexpr auto a_k_m_global_iterator_hacks = AGlobalIteratorHacks{};
        constexpr auto b_k_n_global_iterator_hacks = BGlobalIteratorHacks{};

        // hack to control index calculation when move slice window for A and B matrix for
        // threadwise copy
        constexpr auto a_k_m_global_move_slice_window_iterator_hack =
            AGlobalMoveSliceWindowIteratorHacks{};
        constexpr auto b_k_n_global_move_slice_window_iterator_hack =
            BGlobalMoveSliceWindowIteratorHacks{};

        FloatAB* p_a_block_even = p_a_block_double;
        FloatAB* p_b_block_even = p_b_block_double;

        FloatAB* p_a_block_odd = p_a_block_double + a_block_space_size;
        FloatAB* p_b_block_odd = p_b_block_double + b_block_space_size;

        auto a_block_even_buf = make_dynamic_buffer(p_a_block_even);
        auto b_block_even_buf = make_dynamic_buffer(p_b_block_even);

        auto a_block_odd_buf = make_dynamic_buffer(p_a_block_odd);
        auto b_block_odd_buf = make_dynamic_buffer(p_b_block_odd);

        // LDS double buffer: preload data into LDS
        {
            a_blockwise_copy.RunRead(a_k_m_global_desc, p_a_global, a_k_m_global_iterator_hacks);
            b_blockwise_copy.RunRead(b_k_n_global_desc, p_b_global, b_k_n_global_iterator_hacks);

            a_blockwise_copy.RunWrite(a_k_m_block_desc, p_a_block_double);
            b_blockwise_copy.RunWrite(b_k_n_block_desc, p_b_block_double);
        }

        if constexpr(HasMainKBlockLoop)
        {
            index_t k_block_data_begin = 0;

            // LDS double buffer: main body
            // use Do-While loop instead of For loop to simplify control flow
            do
            {
                // even iteration
                a_blockwise_copy.MoveSrcSliceWindow(a_k_m_global_desc,
                                                    a_block_slice_copy_step,
                                                    a_k_m_global_move_slice_window_iterator_hack);
                b_blockwise_copy.MoveSrcSliceWindow(b_k_n_global_desc,
                                                    b_block_slice_copy_step,
                                                    b_k_n_global_move_slice_window_iterator_hack);

                __syncthreads();

                // LDS doubel buffer: load next data from device mem
                a_blockwise_copy.RunRead(
                    a_k_m_global_desc, p_a_global, a_k_m_global_iterator_hacks);
                b_blockwise_copy.RunRead(
                    b_k_n_global_desc, p_b_global, b_k_n_global_iterator_hacks);

                // LDS double buffer: GEMM on current data
                blockwise_gemm.Run(a_block_even_buf, b_block_even_buf, c_thread_buf);

                // LDS double buffer: store next data to LDS
                a_blockwise_copy.RunWrite(a_k_m_block_desc, p_a_block_odd);
                b_blockwise_copy.RunWrite(b_k_n_block_desc, p_b_block_odd);

                // odd iteration
                a_blockwise_copy.MoveSrcSliceWindow(a_k_m_global_desc,
                                                    a_block_slice_copy_step,
                                                    a_k_m_global_move_slice_window_iterator_hack);
                b_blockwise_copy.MoveSrcSliceWindow(b_k_n_global_desc,
                                                    b_block_slice_copy_step,
                                                    b_k_n_global_move_slice_window_iterator_hack);

                __syncthreads();

                // LDS doubel buffer: load next data from device mem
                a_blockwise_copy.RunRead(
                    a_k_m_global_desc, p_a_global, a_k_m_global_iterator_hacks);
                b_blockwise_copy.RunRead(
                    b_k_n_global_desc, p_b_global, b_k_n_global_iterator_hacks);

                // LDS double buffer: GEMM on current data
                blockwise_gemm.Run(a_block_odd_buf, b_block_odd_buf, c_thread_buf);

                // LDS double buffer: store next data to LDS
                a_blockwise_copy.RunWrite(a_k_m_block_desc, p_a_block_even);
                b_blockwise_copy.RunWrite(b_k_n_block_desc, p_b_block_even);

                k_block_data_begin += 2 * KPerBlock;
            } while(k_block_data_begin < K - 2 * KPerBlock);
        }

        // LDS double buffer: tail
        if constexpr(HasDoubleTailKBlockLoop) // if has 2 iteration left
        {
            a_blockwise_copy.MoveSrcSliceWindow(a_k_m_global_desc,
                                                a_block_slice_copy_step,
                                                a_k_m_global_move_slice_window_iterator_hack);
            b_blockwise_copy.MoveSrcSliceWindow(b_k_n_global_desc,
                                                b_block_slice_copy_step,
                                                b_k_n_global_move_slice_window_iterator_hack);

            __syncthreads();

            // LDS double buffer: load last data from device mem
            a_blockwise_copy.RunRead(a_k_m_global_desc, p_a_global, a_k_m_global_iterator_hacks);
            b_blockwise_copy.RunRead(b_k_n_global_desc, p_b_global, b_k_n_global_iterator_hacks);

            // LDS double buffer: GEMM on 2nd-last data
            blockwise_gemm.Run(a_block_even_buf, b_block_even_buf, c_thread_buf);

            // LDS double buffer: store last data to LDS
            a_blockwise_copy.RunWrite(a_k_m_block_desc, p_a_block_double + a_block_space_size);
            b_blockwise_copy.RunWrite(b_k_n_block_desc, p_b_block_double + b_block_space_size);

            __syncthreads();

            // LDS double buffer: GEMM on last data
            blockwise_gemm.Run(a_block_odd_buf, b_block_odd_buf, c_thread_buf);
        }
        else // if has 1 iteration left
        {
            __syncthreads();

            // LDS double buffer: GEMM on last data
            blockwise_gemm.Run(a_block_even_buf, b_block_even_buf, c_thread_buf);
        }

        // output: register to global memory
        {
            constexpr auto M1 = Number<MPerThread * MLevel0Cluster * MLevel1Cluster>{};
            constexpr auto N1 = Number<NPerThread * NLevel0Cluster * NLevel1Cluster>{};

            // hack to control index calculation when iterating over c_m0_m1_n0_n1_global tensor
            constexpr auto c_m0_m1_n0_n1_global_tensor_iterator_hacks = CGlobalIteratorHacks{};

            const auto c_thread_data_idx_on_block =
                blockwise_gemm.CalculateCThreadOriginDataIndex(get_thread_local_1d_id());

            ThreadwiseDynamicTensorSliceTransfer_v1r3<
                FloatAcc,
                FloatC,
                decltype(c_m0_m1_n0_n1_thread_desc),
                decltype(c_m0_m1_n0_n1_global_desc),
                Sequence<MRepeat, MPerThread, NRepeat, NPerThread>,
                CThreadTransferSrcDstAccessOrder,
                CThreadTransferSrcDstVectorDim,
                CThreadTransferDstScalarPerVector,
                AddressSpace::Vgpr,
                AddressSpace::Global,
                CGlobalMemoryDataOperation,
                1,
                true>{
                c_m0_m1_n0_n1_global_desc,
                make_multi_index(m_block_data_idx_on_global / M1 + c_thread_data_idx_on_block[I0],
                                 c_thread_data_idx_on_block[I1],
                                 n_block_data_idx_on_global / N1 + c_thread_data_idx_on_block[I2],
                                 c_thread_data_idx_on_block[I3])}
                .Run(c_m0_m1_n0_n1_thread_desc,
                     make_tuple(I0, I0, I0, I0),
                     c_thread_buf,
                     c_m0_m1_n0_n1_global_desc,
                     p_c_global,
                     c_m0_m1_n0_n1_global_tensor_iterator_hacks);
        }
    }

    template <bool HasMainKBlockLoop, bool HasDoubleTailKBlockLoop>
    __device__ void Run(const AGlobalDesc& a_k_m_global_desc,
                        const FloatAB* __restrict__ p_a_global,
                        const BGlobalDesc& b_k_n_global_desc,
                        const FloatAB* __restrict__ p_b_global,
                        const CGlobalDesc& c_m0_m1_n0_n1_global_desc,
                        FloatC* __restrict__ p_c_global,
                        const CBlockClusterDesc& c_block_cluster_desc,
                        integral_constant<bool, HasMainKBlockLoop>,
                        integral_constant<bool, HasDoubleTailKBlockLoop>) const
    {
        constexpr index_t shared_block_size = GetSharedMemoryNumberOfByte() / sizeof(FloatAB);

        __shared__ FloatAB p_shared_block[shared_block_size];

        Run(a_k_m_global_desc,
            p_a_global,
            b_k_n_global_desc,
            p_b_global,
            c_m0_m1_n0_n1_global_desc,
            p_c_global,
            c_block_cluster_desc,
            p_shared_block,
            integral_constant<bool, HasMainKBlockLoop>{},
            integral_constant<bool, HasDoubleTailKBlockLoop>{});
    }
};

} // namespace ck
#endif
