#pragma once

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <stdexcept>


namespace Windy::Core::Module {
    class Context {
        std::unordered_map<std::type_index, std::shared_ptr<void>> services;

    public:
        template<typename T>
        void set(std::shared_ptr<T> service) {
            services[typeid(T)] = service;
        }

        template<typename T>
        std::shared_ptr<T> get() {
            auto it = services.find(typeid(T));
            if (it == services.end()) {
                throw std::runtime_error("Service not found");
            }
            return std::static_pointer_cast<T>(it->second);
        }
    };
}