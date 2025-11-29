// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Sebastian Hollersbacher

#include "m3t/websocket.h"

namespace m3t {

WebSocket::WebSocket(int port) : port_{port} {
  ws_server_.init_asio();

  ws_server_.set_open_handler([this](websocketpp::connection_hdl hdl) {
    std::cout << "[WebSocket] Client connected" << std::endl;
    last_hdl_ = hdl;
  });

  ws_server_.set_close_handler([this](websocketpp::connection_hdl hdl) {
    std::cout << "[WebSocket] Client disconnected" << std::endl;
  });

  ws_server_.set_message_handler(
      [this](websocketpp::connection_hdl hdl, Server::message_ptr msg) {
        onMessage(hdl, msg);
      });

  ws_server_.listen(port_);
  ws_server_.start_accept();

  std::cout << "[WebSocket] Listening on port " << port_ << std::endl;

  // background thread for receiving images
  server_thread_ = std::thread([this]() { ws_server_.run(); });
}

WebSocket::~WebSocket() {
  std::cout << "[WebSocket] Shutting down..." << std::endl;

  try {
    ws_server_.stop_listening();
    ws_server_.stop();
    if (server_thread_.joinable()) {
      server_thread_.join();
    }
  } catch (const std::exception& e) {
    std::cerr << "[WebSocket] Error during shutdown: " << e.what() << std::endl;
  }
}

void WebSocket::onMessage(websocketpp::connection_hdl hdl,
                          Server::message_ptr msg) {
  try {
    if (msg->get_opcode() != websocketpp::frame::opcode::binary) {
      std::cout << "[WebSocket] Received non-binary message: "
                << msg->get_payload() << std::endl;
      return;
    }

    // Decode payload outside lock (slow operation)
    const std::string& payload = msg->get_payload();
    std::cout << "[WebSocket] Payload size: " << payload.size() << " bytes" << std::endl;

    std::vector<uchar> buffer(payload.begin(), payload.end());
    cv::Mat decoded = cv::imdecode(buffer, cv::IMREAD_COLOR);

    std::cout << "[WebSocket] Decoded image "
              << decoded.cols << "x" << decoded.rows << std::endl;
    if (decoded.empty()) {
      std::cerr << "[WebSocket] Failed to decode received image" << std::endl;
      return;
    }
    
    {
      std::lock_guard<std::mutex> lock(mtx_);
      latest_rgb_image_ = std::move(decoded);
    }
  } catch (const std::exception& e) {
    std::cerr << "[WebSocket] onMessage error: " << e.what() << std::endl;
  }
}

// Capture latest images into current buffers
void WebSocket::CaptureImage() {
  std::lock_guard<std::mutex> lock(mtx_);
  if (!latest_rgb_image_.empty()) {
    rgb_image_ = latest_rgb_image_;
  }
  if (!latest_depth_image_.empty()) {
    depth_image_ = latest_depth_image_;
  }
}

// Return the latest RGB frame
cv::Mat WebSocket::GetRGB() const {
  std::lock_guard<std::mutex> lock(mtx_);
  return rgb_image_.clone();
}

// Return the latest depth frame (if supported)
cv::Mat WebSocket::GetDepth() const {
  std::lock_guard<std::mutex> lock(mtx_);
  return depth_image_.clone();
}

// Send coordinates back to the connected client
void WebSocket::SendCoordinates(const std::string& coords) {
  std::lock_guard<std::mutex> lock(mtx_);
  try {
    if (last_hdl_.lock().get() == nullptr) {
      return;
    }
    ws_server_.send(last_hdl_, coords, websocketpp::frame::opcode::text);
  } catch (const websocketpp::exception& e) {
    std::cerr << "[WebSocket] Send failed: " << e.what() << std::endl;
  }
}

}  // namespace m3t
