// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Sebastian Hollersbacher

#pragma once

#include <memory>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>
#include <vector>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace m3t {

using Server = websocketpp::server<websocketpp::config::asio>;

class WebSocket {
 public:
  explicit WebSocket(int port = 9002);

  ~WebSocket();

  // Take latest images and copy them to current ones
  void CaptureImage();

  cv::Mat GetRGB() const;
  cv::Mat GetDepth() const;

  // Send coordinates to connected client
  void SendCoordinates(const std::string& coords);

 private:
  int port_;
  Server ws_server_;
  std::thread server_thread_;
  mutable std::mutex mtx_;

  cv::Mat rgb_image_ = cv::Mat::zeros(480, 640, CV_8UC3);
  cv::Mat depth_image_;

  cv::Mat latest_rgb_image_;
  cv::Mat latest_depth_image_;

  websocketpp::connection_hdl last_hdl_;

  void onMessage(websocketpp::connection_hdl hdl, Server::message_ptr msg);
};

}  // namespace m3t
