/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */
/*******************************************************
 * Copyright (c) 2017, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

// Reference
// Keras - https://git.io/fNxKN
// PyTorch - https://git.io/fNx6T

#include <cmath>

#include "flashlight/fl/autograd/Functions.h"
#include "flashlight/fl/nn/Init.h"
#include "flashlight/fl/nn/Utils.h"

namespace af {
af::array uniform(af::dim4 shape, double min, double max, af::dtype type) {
  af::array result = af::randu(shape, type);
  result = (max - min) * result + min;
  return result;
}
af::array normal(af::dim4 shape, double stdv, double mean, af::dtype type) {
  af::array result = af::randn(shape, type);
  result = stdv * result + mean;
  return result;
}

af::array kaimingUniform(
    af::dim4 shape,
    int fanIn,
    af::dtype type /* = af::dtype::f32 */) {
  double stdv = std::sqrt(1.0 / static_cast<double>(fanIn));
  double limit = std::sqrt(3.0) * stdv;
  return uniform(shape, -limit, limit, type);
}

af::array kaimingNormal(
    af::dim4 shape,
    int fanIn,
    af::dtype type /* = af::dtype::f32 */) {
  double stdv = std::sqrt(1.0 / static_cast<double>(fanIn));
  return normal(shape, stdv, 0, type);
}

af::array glorotUniform(
    af::dim4 shape,
    int fanIn,
    int fanOut,
    af::dtype type /* = af::dtype::f32 */) {
  double stdv = std::sqrt(2.0 / static_cast<double>(fanIn + fanOut));
  double limit = std::sqrt(3.0) * stdv;
  return uniform(shape, -limit, limit, type);
}

af::array glorotNormal(
    af::dim4 shape,
    int fanIn,
    int fanOut,
    af::dtype type /* = af::dtype::f32 */) {
  double stdv = std::sqrt(2.0 / static_cast<double>(fanIn + fanOut));
  return normal(shape, stdv, 0, type);
}

} // namespace af

namespace fl {

Variable input(const af::array& arr) {
  return Variable(arr, false);
}

Variable noGrad(const af::array& arr) {
  return Variable(arr, false);
}

Variable param(const af::array& arr) {
  return Variable(arr, true);
}

Variable constant(
    double val,
    int outputSize,
    int inputSize,
    af::dtype type,
    bool calcGrad) {
  return constant(val, af::dim4(outputSize, inputSize), type, calcGrad);
}

Variable constant(double val, af::dim4 dims, af::dtype type, bool calcGrad) {
  return Variable(af::constant(val, dims, type), calcGrad);
}

Variable
identity(int outputSize, int inputSize, af::dtype type, bool calcGrad) {
  return identity(af::dim4(outputSize, inputSize), type, calcGrad);
}

Variable identity(af::dim4 dims, af::dtype type, bool calcGrad) {
  return Variable(af::identity(dims, type), calcGrad);
}

Variable uniform(
    int outputSize,
    int inputSize,
    double min,
    double max,
    af::dtype type,
    bool calcGrad) {
  return uniform(af::dim4(outputSize, inputSize), min, max, type, calcGrad);
}

Variable
uniform(af::dim4 dims, double min, double max, af::dtype type, bool calcGrad) {
  return Variable(af::uniform(dims, min, max, type), calcGrad);
}

Variable normal(
    int outputSize,
    int inputSize,
    double stdv,
    double mean,
    af::dtype type,
    bool calcGrad) {
  return normal(af::dim4(outputSize, inputSize), stdv, mean, type, calcGrad);
}

Variable
normal(af::dim4 dims, double stdv, double mean, af::dtype type, bool calcGrad) {
  return Variable(af::normal(dims, stdv, mean, type), calcGrad);
}

Variable kaimingUniform(
    af::dim4 shape,
    int fanIn,
    af::dtype type /* = af::dtype::f32 */,
    bool calcGrad /* = true */) {
  return Variable(af::kaimingUniform(shape, fanIn, type), calcGrad);
}

Variable kaimingNormal(
    af::dim4 shape,
    int fanIn,
    af::dtype type /* = af::dtype::f32 */,
    bool calcGrad /* = true */) {
  return Variable(af::kaimingNormal(shape, fanIn, type), calcGrad);
}

Variable glorotUniform(
    af::dim4 shape,
    int fanIn,
    int fanOut,
    af::dtype type /* = af::dtype::f32 */,
    bool calcGrad /* = true */) {
  return Variable(af::glorotUniform(shape, fanIn, fanOut, type), calcGrad);
}

Variable glorotNormal(
    af::dim4 shape,
    int fanIn,
    int fanOut,
    af::dtype type /* = af::dtype::f32 */,
    bool calcGrad /* = true */) {
  return Variable(af::glorotNormal(shape, fanIn, fanOut, type), calcGrad);
}

Variable truncNormal(
    af::dim4 shape,
    double stdv,
    double mean,
    double minCufOff,
    double maxCutOff,
    af::dtype type,
    bool calcGrad) {
  // Rigorous implementation
  // auto normCdf = [](double x) {
  //   return (1. + std::erf(x / std::sqrt(2.))) / 2.;
  // };

  // auto l = 2 * normCdf((minCufOff - mean) / stdv) - 1;
  // auto u = 2 * normCdf((maxCutOff - mean) / stdv) - 1;

  // auto result = af::randu(shape, type) * (u - l) + l;
  // result = af::erfinv(result);
  // result = mean + result * (stdv * std::sqrt(2.));
  // result = af::clamp(result, minCufOff, maxCutOff);

  // Funky implementation
  af::array result = mean + af::randn(shape, type) * stdv;
  result = af::clamp(result, minCufOff, maxCutOff);

  return Variable(result, calcGrad);
}

} // namespace fl
