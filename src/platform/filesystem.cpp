#include <platform/filesystem.hpp>


using namespace Windy::Platform;

bool Filesystem::exists(const String& path) {
    return std::filesystem::exists(path);
}

String Filesystem::normalizePath(const String& path) {
    std::filesystem::path p(path);
    return p.make_preferred().string();
}