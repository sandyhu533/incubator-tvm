/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*!
 * \brief CoreML runtime that can run coreml model
 *        containing only tvm PackedFunc.
 * \file coreml_runtime.h
 */
#ifndef TVM_RUNTIME_CONTRIB_COREML_COREML_RUNTIME_H_
#define TVM_RUNTIME_CONTRIB_COREML_COREML_RUNTIME_H_

#import <CoreML/CoreML.h>
#import <Foundation/Foundation.h>

#include <dlpack/dlpack.h>
#include <tvm/runtime/ndarray.h>
#include <tvm/runtime/packed_func.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace tvm {
namespace runtime {

/*!
 * \brief CoreML model.
 */
class CoreMLModel {
 public:
  /*!
   * \brief constructor
   * \param url The directory where compiled models are located.
   */
  explicit CoreMLModel(NSURL* url) {
    url_ = url;
    model_ = [MLModel modelWithContentsOfURL:url error:nil];
    input_dict_ = [NSMutableDictionary dictionary];
    output_ = nil;
  }
  /*!
   * \brief Invoke the coreml prediction.
   */
  void Invoke();
  /*!
   * \brief set input to the model.
   * \param key The input name.
   * \param data_in The input data.
   */
  void SetInput(const std::string& key, DLTensor* data_in);
  /*!
   * \brief Return NDArray for given output index.
   * \param index The output index.
   *
   * \return NDArray corresponding to given output node index.
   */
  NDArray GetOutput(int index) const;
  /*!
   * \brief Return the number of outputs
   *
   * \return The number of outputs
   */
  int GetNumOutputs() const;

  // CoreML model url
  NSURL* url_;
  // CoreML model
  MLModel* model_;
  // CoreML model input dictionary
  NSMutableDictionary<NSString*, id>* input_dict_;
  // CoreML model output
  id<MLFeatureProvider> output_;
};

/*!
 * \brief CoreML runtime.
 *
 *  This runtime can be accessed in various language via
 *  TVM runtime PackedFunc API.
 */
class CoreMLRuntime : public ModuleNode {
 public:
  /*!
   * \brief Get member function to front-end.
   * \param name The name of the function.
   * \param sptr_to_self The pointer to the module node.
   * \return The corresponding member function.
   */
  virtual PackedFunc GetFunction(const std::string& name, const ObjectPtr<Object>& sptr_to_self);

  /*!
   * \brief Serialize the content of the mlmodelc directory and save it to
   *        binary stream.
   * \param stream The binary stream to save to.
   */
  void SaveToBinary(dmlc::Stream* stream) final;

  /*!
   * \return The type key of the executor.
   */
  const char* type_key() const { return "coreml"; }

  /*!
   * \brief Initialize the coreml runtime with coreml model and context.
   * \param model_dir The directory where compiled models are located.
   */
  void Init(const std::string& model_dir);

  /*!
   * \brief Get coreml model.
   * \param model_name The name of the model.
   */
  CoreMLModel& GetModel(const std::string& model_name);

  // Map of the avaiable CoreML models
  std::unordered_map<std::string, std::unique_ptr<CoreMLModel>> model_map_;
};

}  // namespace runtime
}  // namespace tvm

#endif  // TVM_RUNTIME_CONTRIB_COREML_COREML_RUNTIME_H_
