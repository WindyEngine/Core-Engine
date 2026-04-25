#include <core/module/kernel.hpp>


namespace Windy::Core {
    class Engine {
    private:
        Module::ModuleRegistry moduleRegistry;
        Module::Kernel kernel;
    
    public:
        void init();
        void shutdown();
    };
}