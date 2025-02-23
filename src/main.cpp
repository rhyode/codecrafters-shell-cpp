#include <iostream>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::string input;

  // Uncomment this block to pass the first stage
   while(true){
 	 std::cout << "$ ";
 	 std::getline(std::cin, input);
  	 std::cout<<input<<": command not found"<<std::endl;
	}
}
