#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[])
{
  std::ifstream file;
  std::istream* input = std::addressof(std::cin);
  if (argc > 1) {
    file.open(argv[1]);
    if (!file.is_open()) {
      std::cerr << "cannot open input file\n";
      return 1;
    }
    input = std::addressof(file);
  }

  std::string line;
  while (std::getline(*input, line)) {
    if (line.empty()) {
      continue;
    }
  }

  return 0;
}
