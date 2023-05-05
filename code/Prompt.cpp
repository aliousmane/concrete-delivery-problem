
#include "Prompt.h"

#include <string>
#include <iostream>

using namespace std;
  void Prompt::ShowMissingArgument(int argc){
      std::cout << "Please provide some arguments " << std::endl;
      std::cout << "Argument 1: Instance file" << std::endl;
      std::cout << (2 - argc) << " Argument(s) missing: " << std::endl;
}
