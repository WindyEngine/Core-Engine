#include <platform/window_manager.hpp>

using namespace engine::platform;


int main(int argc, char** argv) {
  std::shared_ptr<WindowManager> windowManager = WindowManager::getInstance();
  windowManager->createWindow(800, 600, "Hello Triangle", WindowAPI::GLFW);
  auto window = windowManager->getWindow("Hello Triangle");
}