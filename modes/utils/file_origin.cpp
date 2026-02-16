#pragma once

#include "../../buffer.cpp"
#include <filesystem>
#include <fstream>
#include <ostream>

class FileOrigin {
public:
  filesystem::path origin_path;

  FileOrigin(string rel_path) { origin_path = filesystem::absolute(rel_path); }

  void set_origin(string rel_path) {
    origin_path = filesystem::absolute(rel_path);
  };

  void save(Buffer &buf) {
    std::ofstream out(origin_path);
    if (out.is_open()) {
      for (auto line : buf.lines) {
        out << line.full_string << std::endl;
      }
    }
    out.close();
  }
};
