#include <core/engine.hpp>


using namespace Windy::Core;


void Engine::init() {
    kernel.boot(moduleRegistry);
}

void Engine::shutdown() {
    kernel.shutdown();
}