#include <fstream>
#include <iomanip>
#include <resource_management/file.hpp>
#include <sstream>


std::string engine::resource_management::readFile(std::string path) {
  std::ifstream file(path);
  std::ostringstream oss;
  oss << file.rdbuf();
  return oss.str();
}
