#pragma once

#include <core/module/module.hpp>


namespace Windy::Core::Module {
    class DependencyResolver {
    public:
        static std::vector<std::shared_ptr<Module>> resolve(
            const std::unordered_map<std::string, std::shared_ptr<Module>>& modules
        );
    };
}