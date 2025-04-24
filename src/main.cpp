#include <core/types.hpp>
#include <renderer/window.hpp>
#include <resource_management/resource_manager.hpp>
#include <renderer/helloTriangle.hpp>

using namespace engine;
using namespace engine::rendering;

int main() {

  /* Resource Management Shit */
  using namespace engine::resource_management;



  //std::cout << "\nTesting\n" << std::endl;
  //Shader* shader = manager.loadShader("/home/Zenjar/Documents/projects/graphics_assets/shader.vert", "/home/Zenjar/Documents/projects/graphics_assets/shader.frag").get();
  //std::cout << "\nEnd Testing\n" << std::endl;


#ifdef ENGINE_COMPILE_OPENGL
    std::cout << "Initializing OpenGL window..." << std::endl;

    openGLTriangle();

    std::cout << "Window closed successfully." << std::endl;
#endif

  return 0;
}
