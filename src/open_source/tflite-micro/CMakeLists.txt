#===============================================================================
# @brief mqtt cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "tflite-micro")

set(TENSORFLOW_ROOT
    ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
    # ${TENSORFLOW_ROOT}/examples/person_detection/model_settings.cc
    # ${TENSORFLOW_ROOT}/examples/person_detection/image_provider.cc
    # ${TENSORFLOW_ROOT}/examples/person_detection/main_functions.cc
    # ${TENSORFLOW_ROOT}/examples/person_detection/main.cc
    # ${TENSORFLOW_ROOT}/examples/person_detection/detection_responder.cc
    # ${TENSORFLOW_ROOT}/examples/micro_speech/models/micro_speech_quantized_model_data.cc
    # ${TENSORFLOW_ROOT}/examples/micro_speech/models/audio_preprocessor_int8_model_data.cc
    # ${TENSORFLOW_ROOT}/examples/micro_speech/testdata/noise_1000ms_audio_data.cc
    # ${TENSORFLOW_ROOT}/examples/micro_speech/testdata/no_1000ms_audio_data.cc
    # ${TENSORFLOW_ROOT}/examples/micro_speech/testdata/yes_1000ms_audio_data.cc
    # ${TENSORFLOW_ROOT}/examples/micro_speech/testdata/silence_1000ms_audio_data.cc
    # ${TENSORFLOW_ROOT}/examples/micro_speech/testdata/no_30ms_audio_data.cc
    # ${TENSORFLOW_ROOT}/examples/micro_speech/testdata/yes_30ms_audio_data.cc
    # ${TENSORFLOW_ROOT}/examples/micro_speech/micro_speech_test.cc
    ${TENSORFLOW_ROOT}/examples/hello_world/models/hello_world_float_model_data.cc
    ${TENSORFLOW_ROOT}/examples/hello_world/models/hello_world_int8_model_data.cc
    ${TENSORFLOW_ROOT}/examples/hello_world/hello_world_test.cc
    ${TENSORFLOW_ROOT}/adapte/debug_log.cc
    ${TENSORFLOW_ROOT}/adapte/micro_time.cc
    ${TENSORFLOW_ROOT}/adapte/system_setup.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/delay.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/energy.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/fft_auto_scale_kernel.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/fft_auto_scale_common.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/filter_bank.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/filter_bank_log.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/filter_bank_square_root.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/filter_bank_square_root_common.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/filter_bank_spectral_subtraction.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/framer.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/irfft.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/rfft.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/stacker.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/overlap_add.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/pcan.cc
    ${TENSORFLOW_ROOT}/signal/micro/kernels/window.cc
    ${TENSORFLOW_ROOT}/signal/src/circular_buffer.cc
    ${TENSORFLOW_ROOT}/signal/src/energy.cc
    ${TENSORFLOW_ROOT}/signal/src/fft_auto_scale.cc
    ${TENSORFLOW_ROOT}/signal/src/filter_bank.cc
    ${TENSORFLOW_ROOT}/signal/src/filter_bank_log.cc
    ${TENSORFLOW_ROOT}/signal/src/filter_bank_square_root.cc
    ${TENSORFLOW_ROOT}/signal/src/filter_bank_spectral_subtraction.cc
    ${TENSORFLOW_ROOT}/signal/src/irfft_float.cc
    ${TENSORFLOW_ROOT}/signal/src/irfft_int16.cc
    ${TENSORFLOW_ROOT}/signal/src/irfft_int32.cc
    ${TENSORFLOW_ROOT}/signal/src/log.cc
    ${TENSORFLOW_ROOT}/signal/src/max_abs.cc
    ${TENSORFLOW_ROOT}/signal/src/msb_32.cc
    ${TENSORFLOW_ROOT}/signal/src/msb_64.cc
    ${TENSORFLOW_ROOT}/signal/src/overlap_add.cc
    ${TENSORFLOW_ROOT}/signal/src/pcan_argc_fixed.cc
    ${TENSORFLOW_ROOT}/signal/src/rfft_float.cc
    ${TENSORFLOW_ROOT}/signal/src/rfft_int16.cc
    ${TENSORFLOW_ROOT}/signal/src/rfft_int32.cc
    ${TENSORFLOW_ROOT}/signal/src/square_root_32.cc
    ${TENSORFLOW_ROOT}/signal/src/square_root_64.cc
    ${TENSORFLOW_ROOT}/signal/src/window.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/activations.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/activations_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/add.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/add_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/add_n.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/arg_min_max.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/assign_variable.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/batch_matmul.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/batch_matmul_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/batch_to_space_nd.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/broadcast_args.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/broadcast_to.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/call_once.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/cast.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/ceil.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/circular_buffer.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/circular_buffer_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/comparisons.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/concatenation.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/conv.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/conv_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/cumsum.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/decompress.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/decompress_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/depth_to_space.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/depthwise_conv.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/depthwise_conv_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/dequantize.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/dequantize_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/detection_postprocess.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/div.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/elementwise.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/elu.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/embedding_lookup.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/ethosu.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/exp.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/expand_dims.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/fill.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/floor.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/floor_div.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/floor_mod.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/fully_connected.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/fully_connected_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/gather.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/gather_nd.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/hard_swish.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/hard_swish_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/if.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/kernel_runner.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/kernel_util.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/l2norm.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/l2_pool_2d.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/leaky_relu.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/leaky_relu_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/logical.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/logical_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/logistic.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/logistic_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/log_softmax.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/lstm_eval.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/lstm_eval_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/maximum_minimum.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/micro_tensor_utils.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/mirror_pad.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/mul.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/mul_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/neg.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/pack.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/pad.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/pooling.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/pooling_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/prelu.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/prelu_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/quantize.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/quantize_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/read_variable.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/reduce.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/reduce_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/reshape.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/reshape_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/resize_bilinear.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/resize_nearest_neighbor.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/round.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/select.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/shape.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/slice.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/softmax.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/softmax_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/space_to_batch_nd.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/space_to_depth.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/split.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/split_v.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/squared_difference.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/squeeze.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/strided_slice.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/strided_slice_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/sub.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/sub_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/svdf.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/svdf_common.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/tanh.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/transpose.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/transpose_conv.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/unidirectional_sequence_lstm.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/unpack.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/var_handle.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/while.cc
    ${TENSORFLOW_ROOT}/tensorflow/lite/micro/kernels/zeros_like.cc
)

set(PUBLIC_HEADER
    ${TENSORFLOW_ROOT}
)

set(PRIVATE_HEADER
    ${TENSORFLOW_ROOT}/third_party
    ${TENSORFLOW_ROOT}/third_party/kissfft
    ${TENSORFLOW_ROOT}/third_party/flatbuffers/include
    ${TENSORFLOW_ROOT}/third_party/ruy
    ${TENSORFLOW_ROOT}/third_party/gemmlowp
    ${TENSORFLOW_ROOT}/examples/hello_world
)

set(PRIVATE_DEFINES
    IOT_LITEOS_ADAPT
    NO_PERSISTENCE
    COMPAT_CMSIS
    NOSTACKTRACE
    MBEDTLS
    MBEDTLS_USE_CRT
    HIGH_PERFORMANCE
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(REWRITE_CCFLAGS
    true
)

set(COMPONENT_CCFLAGS
    -O3 -DNDEBUG --std=c++17 -g -DTF_LITE_STATIC_MEMORY -lm -fno-rtti -fno-exceptions -fno-threadsafe-statics -Wnon-virtual-dtor -fno-unwind-tables
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
