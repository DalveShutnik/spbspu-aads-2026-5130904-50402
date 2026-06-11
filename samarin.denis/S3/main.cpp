#include <fstream>
#include <iostream>

int main(int argc, char* argv[])
{
  if (argc < 2) {
    std::cerr << "no input file provided\n";
    return 1;
  }
  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "cannot open input file\n";
    return 1;
  }
  return 0;
}
