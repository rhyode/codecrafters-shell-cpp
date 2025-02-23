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
	 string dec=input.substr(0,4);
	 if(dec=="echo"){
	 	if(input.length()>5){
			cout<<input.substr(5)<<endl;
		}
	 }
	else{
  	 std::cout<<input<<": command not found"<<std::endl;
	}	
}
}
