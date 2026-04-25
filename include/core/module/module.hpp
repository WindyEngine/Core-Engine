#pragma once

#include <core/module/context.hpp>

#include <string>
#include <vector>


namespace Windy::Core::Module {
    class Module {
    public:
        virtual ~Module() = default;

        virtual std::string name() const = 0;
        virtual std::vector<std::string> dependencies() const = 0;

        virtual bool startup(Context& ctx) = 0;
        virtual void shutdown(Context& ctx) = 0;
    };
}