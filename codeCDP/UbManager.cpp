
#include "UbManager.h"

#include <map>
#include <string>

 std::map<std::string, double> UbManager::bounds = std::map<std::string, double>() ;


void UbManager::Load(std::string const &filename) {
  FILE* f = fopen(filename.c_str(), "r");
  if (f == nullptr) {
    printf("UbManager::Load\nError at the opening of the file:%s\n", filename.c_str());
    return;
  }
  int nb = 0;
  fscanf(f, "%d\n", &nb);

  for (int i = 0; i < nb; i++) {
    char name[40];
    double bound;
    int r = fscanf(f, "%s %lf\n", name, &bound);
    if (r == -1)
      break;
    std::string st(name);
    if (UbManager::bounds.find(st) != UbManager::bounds.end()) {
      printf("%s bound:%lf vc:\n", name, bound);
      exit(1);
    }
      UbManager::bounds[st] = bound;
  }
}

