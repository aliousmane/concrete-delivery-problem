
#ifndef CODE_PROMPT_H
#define CODE_PROMPT_H

#include <string>
#include <iostream>
class Prompt {
public:
  static  void ShowMissingArgument(int argc);

    template<typename T>
    static void print(const T  container){
        std::cout << "{ ";
        for (auto& element : container) {
            std::cout << element << " "<<std::flush;
        }
        std::cout << "}"<<std::endl;
    }

};


#endif //CODE_PROMPT_H
