#pragma once

#include <memory>
#include <resource_management/resource.hpp>
#include <string>
#include <filewatch/FileWatch.hpp>


namespace engine::resource_management {

// File class that extends Resource, adding hot-reloading support
class File : public Resource {
private:
  std::unique_ptr<filewatch::FileWatch<std::string>> _watcher;  // File watcher for hot-reloading

  void load() override;   // Load the file
  void unload() override; // Unload  the file

public:
  std::string path;         // File path
  bool hot_reload = false;  // Hot-Reload flag

  File(const std::string path, std::string name = "", bool hot_reload = false, bool lazy = true); // Constructor
  ~File();  // Deconstrunctor

  // Hot Reloading Methods
  void watch(); // Start watching the file for changes
  void stopWatching();  // Stop watching the file for changes

  std::string read(); // Read the file content
};

}
