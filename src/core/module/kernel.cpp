#include <core/module/kernel.hpp>
#include <core/module/resolver.hpp>


using namespace Windy::Core::Module;


void Kernel::boot(ModuleRegistry& registry) {
    ordered = DependencyResolver::resolve(registry.modules);

    for (auto& module : ordered) {
        if (!module->startup(ctx)) {
            rollback();
            throw std::runtime_error("Startup failed");
        }

        started.push_back(module);
    }
}

void Kernel::shutdown() {
    for (auto it = ordered.rbegin(); it != ordered.rend(); it++) {
        (*it)->shutdown(ctx);
    }
}

void Kernel::rollback() {
    shutdown();
    started.clear();
}