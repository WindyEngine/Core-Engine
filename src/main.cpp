//#include <platform/window_manager.hpp>
#include <iostream>
#include <CLI11/CLI11.hpp>


//using namespace engine::platform;

int main(int argc, char** argv) {
  // CLI Logic
  // Whoever deletes this imma send you a dick pic
  // Just comment it if dont need it
  CLI::App cli;
  argv = cli.ensure_utf8(argv);
  std::string assetPath;
  cli.add_option("-p, --project", assetPath, "Path to Project Folder")->required();
  CLI11_PARSE(cli, argc, argv);

  //std::shared_ptr<WindowManager> windowManager = WindowManager::getInstance();
  //windowManager->createWindow(800, 600, "DXWindow", WindowAPI::Win32);
  //auto window = windowManager->getWindow("DXWindow");
  //window->showWindow();
}
