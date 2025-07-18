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

This file has been modified by Silicon Labs.
==============================================================================*/
#include "microfrontend/lib/frontend_util.h"
#include "microfrontend/lib/dc_notch_filter.h"
#include <stdio.h>
#include <string.h>

#include "microfrontend/lib/bits.h"

void FrontendFillConfigWithDefaults(struct FrontendConfig* config) {
  WindowFillConfigWithDefaults(&config->window);
  FilterbankFillConfigWithDefaults(&config->filterbank);
  NoiseReductionFillConfigWithDefaults(&config->noise_reduction);
  PcanGainControlFillConfigWithDefaults(&config->pcan_gain_control);
  LogScaleFillConfigWithDefaults(&config->log_scale);
  ActivityDetectionFillConfigWithDefaults(&config->activity_detection);
  DcNotchFilterFillConfigWithDefaults(&config->dc_notch_filter);
}

int FrontendPopulateState(const struct FrontendConfig* config,
                          struct FrontendState* state, int sample_rate) {

  if(!config->noise_reduction.enable_noise_reduction && config->pcan_gain_control.enable_pcan)
  {
    fprintf(stderr, "PCAN must be disabled if noise reduction is disabled");
    return 0;
  }

  memset(state, 0, sizeof(*state));

  if (!WindowPopulateState(&config->window, &state->window, sample_rate)) {
    fprintf(stderr, "Failed to populate window state\n");
    return 0;
  }

  if (sli_ml_fft_init(&state->fft, state->window.size) != SL_STATUS_OK){
    fprintf(stderr, "Failed to initialize FFT\n");
    return 0;
  }

  if (!FilterbankPopulateState(&config->filterbank, &state->filterbank,
                               sample_rate, state->fft.fft_size / 2 + 1)) {
    fprintf(stderr, "Failed to populate filterbank state\n");
    return 0;
  }

  if (!NoiseReductionPopulateState(&config->noise_reduction,
                                   &state->noise_reduction,
                                   state->filterbank.num_channels)) {
    fprintf(stderr, "Failed to populate noise reduction state\n");
    return 0;
  }

  ActivityDetectionConfig(&config->activity_detection, config->filterbank.num_channels, &state->activity_detection);

  DcNotchFilterConfig(&config->dc_notch_filter, &state->dc_notch_filter);
  int input_correction_bits =
      MostSignificantBit32(state->fft.fft_size) - 1 - (kFilterbankBits / 2);
  if (!PcanGainControlPopulateState(
          &config->pcan_gain_control, &state->pcan_gain_control,
          state->noise_reduction.estimate, state->filterbank.num_channels,
          state->noise_reduction.smoothing_bits, input_correction_bits)) {
    fprintf(stderr, "Failed to populate pcan gain control state\n");
    return 0;
  }

  if (!LogScalePopulateState(&config->log_scale, &state->log_scale)) {
    fprintf(stderr, "Failed to populate log scale state\n");
    return 0;
  }

  FrontendReset(state);

  // All good, return a true value.
  return 1;
}

void FrontendFreeStateContents(struct FrontendState* state) {
  WindowFreeStateContents(&state->window);
  sli_ml_fft_deinit(&state->fft);
  FilterbankFreeStateContents(&state->filterbank);
  NoiseReductionFreeStateContents(&state->noise_reduction);
  PcanGainControlFreeStateContents(&state->pcan_gain_control);
}
