// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Sebastian Hollersbacher

#pragma once

#include <m3t/publisher.h>
#include <m3t/body.h>
#include <m3t/websocket.h>
#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

namespace m3t {

class WebSocketPublisher : public m3t::Publisher {
 public:
  WebSocketPublisher(const std::string &name,
                     const std::filesystem::path &metafile_path,
                     const std::shared_ptr<WebSocket> &websocket_ptr);

  bool SetUp() override;
  bool UpdatePublisher(int iteration) override;

  bool AddBody(const std::shared_ptr<Body> &body_ptr);
  void ClearBodies();

 private:
  std::shared_ptr<WebSocket> websocket_ptr_;
  std::vector<std::shared_ptr<Body>> body_ptrs_;
};

}  // namespace m3t
