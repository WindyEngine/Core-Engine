#pragma once

#include <core/module/module.hpp>

#include <cstdint>


namespace Windy::Core::Module {
    using ModuleId = uint64_t;

    class ModuleRegistry {
    public:
        std::unordered_map<std::string, std::shared_ptr<Module>> modules;
        void registerModule(std::shared_ptr<Module> module);
    };
}