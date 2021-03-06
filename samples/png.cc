// Copyright 2019 The Amber Authors.
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

#include "samples/png.h"

#include <cassert>

#include "amber/result.h"
#include "amber/value.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#include "third_party/lodepng/lodepng.h"
#pragma clang diagnostic pop

namespace png {

namespace {

unsigned char byte0(uint32_t word) {
  return static_cast<unsigned char>(word);
}

unsigned char byte1(uint32_t word) {
  return static_cast<unsigned char>(word >> 8);
}

unsigned char byte2(uint32_t word) {
  return static_cast<unsigned char>(word >> 16);
}

unsigned char byte3(uint32_t word) {
  return static_cast<unsigned char>(word >> 24);
}

}  // namespace

std::pair<amber::Result, std::string> ConvertToPNG(
    uint32_t width,
    uint32_t height,
    const std::vector<amber::Value>& values) {
  assert(values.size() == width * height);

  std::vector<unsigned char> data;

  // Prepare data as lodepng expects it
  for (amber::Value value : values) {
    const uint32_t pixel = value.AsUint32();
    data.push_back(byte2(pixel));  // R
    data.push_back(byte1(pixel));  // G
    data.push_back(byte0(pixel));  // B
    data.push_back(byte3(pixel));  // A
  }

  std::vector<unsigned char> png;
  unsigned error = lodepng::encode(png, data, width, height);
  if (error) {
    return std::make_pair(amber::Result("lodepng::encode() returned non-zero"),
                          nullptr);
  }

  return std::make_pair(amber::Result(), std::string(png.begin(), png.end()));
}

}  // namespace png
