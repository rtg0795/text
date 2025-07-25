// Copyright 2025 TF.Text Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef THIRD_PARTY_TENSORFLOW_TEXT_CORE_KERNELS_NGRAMS_TFLITE_H_
#define THIRD_PARTY_TENSORFLOW_TEXT_CORE_KERNELS_NGRAMS_TFLITE_H_

/* Copyright 2021 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/mutable_op_resolver.h"

namespace tflite {
namespace ops {
namespace custom {
namespace text {

// Adds the Ngrams custom op to an op resolver.
// This function can be loaded using dlopen.  Since C++ function names get
// mangled, declare this function as extern C, so its name is unchanged.
extern "C" void AddNgramsStringJoin(MutableOpResolver* resolver);

TfLiteRegistration* Register_TFText_NgramsStringJoin();

}  // namespace text
}  // namespace custom
}  // namespace ops
}  // namespace tflite

#endif  // THIRD_PARTY_TENSORFLOW_TEXT_CORE_KERNELS_NGRAMS_TFLITE_H_
