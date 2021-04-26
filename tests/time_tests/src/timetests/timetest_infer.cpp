// Copyright (C) 2018-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <inference_engine.hpp>
#include <iostream>

#include "common.h"
#include "timetests_helper/timer.h"
#include "timetests_helper/utils.h"
using namespace InferenceEngine;


/**
 * @brief Function that contain executable pipeline which will be called from
 * main(). The function should not throw any exceptions and responsible for
 * handling it by itself.
 */
int runPipeline(const std::string &model, const std::string &device) {
  auto pipeline = [](const std::string &model, const std::string &device) {
    Core ie;
    CNNNetwork cnnNetwork;
    ExecutableNetwork exeNetwork;
    InferRequest inferRequest;
    size_t batchSize = 0;

    {
      SCOPED_TIMER(first_inference_latency, 1);
      {
        SCOPED_TIMER(load_plugin, 2);
        ie.GetVersions(device);
      }
      {
        SCOPED_TIMER(create_exenetwork, 3);
        if (TimeTest::fileExt(model) == "blob") {
          SCOPED_TIMER(import_network, 4);
          exeNetwork = ie.ImportNetwork(model, device);
        }
        else {
          {
            SCOPED_TIMER(read_network, 5);
            cnnNetwork = ie.ReadNetwork(model);
            batchSize = cnnNetwork.getBatchSize();
          }

          {
            SCOPED_TIMER(load_network, 6);
            exeNetwork = ie.LoadNetwork(cnnNetwork, device);
          }
        }
      }
    }

    {
      SCOPED_TIMER(first_inference, 7);
      inferRequest = exeNetwork.CreateInferRequest();

      {
        SCOPED_TIMER(fill_inputs, 8)
        batchSize = batchSize != 0 ? batchSize : 1;
        const InferenceEngine::ConstInputsDataMap inputsInfo(exeNetwork.GetInputsInfo());
        fillBlobs(inferRequest, inputsInfo, batchSize);
      }
      inferRequest.Infer();
    }
  };

  try {
    pipeline(model, device);
  } catch (const InferenceEngine::Exception &iex) {
    std::cerr
        << "Inference Engine pipeline failed with Inference Engine exception:\n"
        << iex.what();
    return 1;
  } catch (const std::exception &ex) {
    std::cerr << "Inference Engine pipeline failed with exception:\n"
              << ex.what();
    return 2;
  } catch (...) {
    std::cerr << "Inference Engine pipeline failed\n";
    return 3;
  }
  return 0;
}
