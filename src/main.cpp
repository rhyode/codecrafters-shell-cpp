
#include <iostream>
using namespace std;
int main() {
    // Flush after every std::cout / std:cerr
    cout << unitbuf;
    cerr << unitbuf;
    string input;

    while(true) {
        cout << "$ ";
        getline(cin, input);
        if(input=="exit 0") break;
        
        string dec=input.substr(0,4);
        if(dec=="echo") {
            if(input.length() > 5) {
                cout << input.substr(5) << endl;
            }
        }
        else if(dec=="type") {
            if(input.length() > 5) {
                string type = input.substr(5);
                if(type=="echo"||type=="exit"||type=="type") 
                    cout << type << " is a shell builtin" << endl;
                else 
                    cout << type  << ": not found" << endl;
            }
        }
        else {
            cout << input << ": command not found" << endl;
        }
    }
}
