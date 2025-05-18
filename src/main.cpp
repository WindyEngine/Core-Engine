#include <platform/window_manager.hpp>
#include <iostream>

using namespace engine::platform;

int main(int argc, char** argv) {
  std::shared_ptr<WindowManager> windowManager = WindowManager::getInstance();
  windowManager->createWindow(800, 600, "DXWindow", WindowAPI::Win32);
  auto window = windowManager->getWindow("DXWindow");
  window->showWindow();
}