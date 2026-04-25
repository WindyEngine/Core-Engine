#include <core/module/registry.hpp>


using namespace Windy::Core::Module;


void ModuleRegistry::registerModule(std::shared_ptr<Module> module) {
    modules[module->name()] = module;
}