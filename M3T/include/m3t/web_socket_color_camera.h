// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Sebastian Hollersbacher

#pragma once

#include <m3t/camera.h>
#include <m3t/websocket.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <filesystem>
#include <iostream>

namespace m3t {

class WebSocketColorCamera : public m3t::ColorCamera {
 public:
  WebSocketColorCamera(const std::string &name,
                 const std::filesystem::path &metafile_path, 
                 const std::shared_ptr<WebSocket> &websocket_ptr);

  bool SetUp() override;
  bool UpdateImage(bool synchronized) override;

 private:
  std::shared_ptr<WebSocket> websocket_ptr_;
};

}  // namespace m3t
