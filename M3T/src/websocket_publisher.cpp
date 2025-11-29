// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Sebastian Hollersbacher

#include <m3t/websocket_publisher.h>

namespace m3t {

WebSocketPublisher::WebSocketPublisher(
    const std::string &name, const std::filesystem::path &metafile_path,
    const std::shared_ptr<WebSocket> &websocket_ptr)
    : Publisher{name, metafile_path}, websocket_ptr_{websocket_ptr} {}

bool WebSocketPublisher::SetUp() {
  if (!websocket_ptr_) {
    std::cerr << "WebSocket pointer not set for Publisher " << name_
              << std::endl;
    return false;
  }

  set_up_ = true;
  return true;
}

bool WebSocketPublisher::UpdatePublisher(int iteration) {
  for (const auto &body_ptr : body_ptrs_) {
    const m3t::Transform3fA &pose = body_ptr->body2world_pose();
    Eigen::Vector3f t = pose.translation();
    Eigen::Matrix3f R = pose.rotation();
    Eigen::Quaternionf q(R);

    std::ostringstream ss;
    ss << "T " << t.x() << " " << t.y() << " " << t.z() << "\n"
       << "R " << q.x() << " " << q.y() << " " << q.z() << " " << q.w();

    websocket_ptr_->SendCoordinates(ss.str());
  }

  return true;
}

bool WebSocketPublisher::AddBody(const std::shared_ptr<Body> &body_ptr) {
  if (!body_ptr) {
    return false;
  }

  body_ptrs_.push_back(body_ptr);
  return true;
}

void WebSocketPublisher::ClearBodies() { body_ptrs_.clear(); }

}  // namespace m3t
