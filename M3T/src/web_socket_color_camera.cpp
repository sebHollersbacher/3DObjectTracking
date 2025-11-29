// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Sebastian Hollersbacher

#include <m3t/web_socket_color_camera.h>

namespace m3t {

WebSocketColorCamera::WebSocketColorCamera(
    const std::string &name,
    const std::filesystem::path &metafile_path,
    const std::shared_ptr<WebSocket> &websocket_ptr)
    : ColorCamera{name, metafile_path}, websocket_ptr_{websocket_ptr} {
}

// Set up the camera
bool WebSocketColorCamera::SetUp() {
  set_up_ = false;
  
  cv::FileStorage fs;
  if (!OpenYamlFileStorage(metafile_path_, &fs)) return false;
  if (!ReadRequiredValueFromYaml(fs, "intrinsics", &intrinsics_)) {
    std::cerr << "Could not read all required body parameters from "
              << metafile_path_ << std::endl;
    return false;
  }
  fs.release();

  if (!websocket_ptr_) {
    std::cerr << "WebSocket pointer not set for camera " << name_ << std::endl;
    return false;
  }

  SaveMetaDataIfDesired();
  set_up_ = true;

  return UpdateImage(true);
}

bool WebSocketColorCamera::UpdateImage(bool synchronized) {
  if (!set_up_) {
    std::cerr << "Camera " << name_ << " not set up yet" << std::endl;
    return false;
  }

  if (!websocket_ptr_) {
    std::cerr << "WebSocket pointer is null for camera " << name_ << std::endl;
    return false;
  }

  websocket_ptr_->CaptureImage();
  image_ = websocket_ptr_->GetRGB();
  return true;
}

}  // namespace m3t
