
#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>

using namespace std;
namespace fs = std::filesystem;

string find_command(const string& cmd) {
    char* path = getenv("PATH");
    if (!path) return "";
    
    string pathStr(path);
    size_t pos = 0;
    string delimiter = ":";
    
    while ((pos = pathStr.find(delimiter)) != string::npos) {
        string dir = pathStr.substr(0, pos);
        string fullPath = dir + "/" + cmd;
        
        if (fs::exists(fullPath)) {
            return fullPath;
        }
        pathStr.erase(0, pos + delimiter.length());
    }
    
    // Check the last directory
    string fullPath = pathStr + "/" + cmd;
    if (fs::exists(fullPath)) {
        return fullPath;
    }
    
    return "";
}



int main() {
    // Flush after
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
                string cmd = input.substr(5);
                if(cmd=="echo"||cmd=="exit"||cmd=="type") 
                    cout << cmd << " is a shell builtin" << endl;
                else{
		    string path = find_command(cmd);
		    if(!path.empty()) cout << cmd << "is" << path << endl;
		    else cout << cmd << ": not found" << endl;
		} 
                    
            }
        }
        else {
            cout << input << ": command not found" << endl;
        }
    }
}
