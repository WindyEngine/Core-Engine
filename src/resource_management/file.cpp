#include <fstream>
#include <sstream>

#include <resource_management/file.hpp>


std::string engine::resource_management::readFile(std::string path) {
  std::ifstream file(path);
  std::ostringstream oss;
  oss << file.rdbuf();
  return oss.str();
}
