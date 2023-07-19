
#ifndef CODE_PROMPT_H
#define CODE_PROMPT_H

#include <string>
#include <vector>
#include "Node.h"
#include <iostream>
class Prompt {
public:
  static  void ShowMissingArgument(int argc);

    template<typename T>
    static void print(const T  container,const std::string sep=" "){
        std::cout << "{ ";
        for (auto& element : container) {
            std::cout << element << sep <<std::flush;
        }
        std::cout << "}"<<std::endl;
    }

    static void print(const std::vector<std::string> container,const std::string sep=" "){
        std::cout << "{ ";
        for (auto& element : container) {
            std::cout << element << sep <<std::flush;
        }
        std::cout << "}"<<std::endl;
    }
    static void print(const std::vector<Customer*> & container,const std::string sep=" "){
        std::cout << "{ ";
        for (auto& element : container) {
            std::cout << element->custID << sep <<std::flush;
        }
        std::cout << "}"<<std::endl;
    }

};


#endif //CODE_PROMPT_H
