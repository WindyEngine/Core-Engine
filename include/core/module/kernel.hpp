#pragma once

#include <core/module/registry.hpp>


namespace Windy::Core::Module {
    class Kernel {
    private:
        Context ctx;
        std::vector<std::shared_ptr<Module>> ordered;
        std::vector<std::shared_ptr<Module>> started;

        void rollback();
    
    public:
        void boot(ModuleRegistry& registry);

        void run();

        void shutdown();
    };
}