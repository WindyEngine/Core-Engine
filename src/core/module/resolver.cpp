#include <core/module/resolver.hpp>

#include <unordered_set>
#include <functional>


using namespace Windy::Core::Module;


std::vector<std::shared_ptr<Module>> DependencyResolver::resolve(
    const std::unordered_map<std::string, std::shared_ptr<Module>>& modules
) {
    std::vector<std::shared_ptr<Module>> result;
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> visiting;

    std::function<void (const std::string&)> dfs = [&](const std::string& name) {
        if (visited.count(name)) return;
        if (visiting.count(name)) {
            throw std::runtime_error("Cycle detected at " + name);
        }

        visiting.insert(name);

        auto it = modules.find(name);
        if (it == modules.end()) {
            throw std::runtime_error("Missing module: " + name);
        }

        for (const auto& dep : it->second->dependencies()) {
            dfs(dep);
        }

        visiting.erase(name);
        visited.insert(name);
        result.push_back(it->second);
    };

    for (const auto& [name, _] : modules) {
        dfs(name);
    }

    return result;
}