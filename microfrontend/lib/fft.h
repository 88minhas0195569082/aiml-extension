/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef TENSORFLOW_LITE_EXPERIMENTAL_MICROFRONTEND_LIB_FFT_H_
#define TENSORFLOW_LITE_EXPERIMENTAL_MICROFRONTEND_LIB_FFT_H_

#include <stdint.h>
#include <stdlib.h>

#include "microfrontend/lib/utils.h"

#ifdef __cplusplus
extern "C" {
#endif

struct complex_int16_t {
  int16_t real;
  int16_t imag;
};

struct FftState {
  int16_t* input;
  struct complex_int16_t* output;
  size_t fft_size;
  size_t input_size;
  void* scratch;
  size_t scratch_size;
};

 void FftCompute(struct FftState* state, const int16_t* input,
                int input_scale_shift);

 void FftInit(struct FftState* state);

 void FftReset(struct FftState* state);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // TENSORFLOW_LITE_EXPERIMENTAL_MICROFRONTEND_LIB_FFT_H_
