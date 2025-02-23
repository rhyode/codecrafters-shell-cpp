#include <iostream>
using namespace std;
int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::string input;

  // Uncomment this block to pass the first stage
   while(true){
 	 std::cout << "$ ";
 	 std::getline(std::cin, input);
	 if(input=="exit 0") break;
	 string dec;
	 dec=input.substring(0,4);
	 if(dec=="echo") cout<<input.substring(5);
  	 std::cout<<input<<": command not found"<<std::endl;
	}
}
