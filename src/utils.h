#pragma once
#include <string>
#include <vector>

namespace ev {
std::string readFile(const char* filePath);

template <typename T>
int inline max_element_index(std::vector<T> vec) {
  return std::max_element(begin(vec), end(vec)) - begin(vec);
}

}  // namespace ev
