
#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;
namespace fs = std::filesystem;

vector<string> parse_command(const string& input) {
    vector<string> args;
    string current_arg;
    bool in_quotes = false;
    
    for(size_t i = 0; i < input.length(); i++) {
        if(input[i] == '\'') {
            in_quotes = !in_quotes;
            continue;
        }
        
        if(!in_quotes && isspace(input[i])) {
            if(!current_arg.empty()) {
                args.push_back(current_arg);
                current_arg.clear();
            }
        } else {
            current_arg += input[i];
        }
    }
    
    if(!current_arg.empty()) {
        args.push_back(current_arg);
    }
    
    return args;
}


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
    cout << unitbuf;
    cerr << unitbuf;
    string input;

    while(true) {
        cout << "$ ";
        getline(cin, input);
        if(input=="exit 0") break;
        
        // Parse the input into arguments
        vector<string> args = parse_command(input);
        if(args.empty()) continue;

        if(args[0]=="cd") {
            if(args.size() < 2) continue;
            string target_path = args[1];
            if (target_path == "~") {
                char* home = getenv("HOME");
                if (home) target_path = home;
            }
            if(!fs::exists(target_path)) {
                cout << "cd: " << args[1] << ": No such file or directory" << endl;
            } else {
                fs::current_path(target_path);
            }
        }
        else if(args[0]=="pwd") {
            cout << fs::current_path().string() << endl;
        }
        
        else if(args[0]=="echo") {
            // Skip the command name and print the rest
            for(size_t i = 1; i < args.size(); i++) {
                cout << args[i] << " ";
            }
            cout << endl;
        }
        else if(args[0]=="type") {
            if(args.size() > 1) {
                string cmd = args[1];
                if(cmd=="echo"||cmd=="exit"||cmd=="type"||cmd=="pwd") 
                    cout << cmd << " is a shell builtin" << endl;
                else{
                    string path = find_command(cmd);
                    if(!path.empty()) cout << cmd << " is " << path << endl;
                    else cout << cmd << ": not found" << endl;
                }
            }
        }
        else {
            // Try to execute as external command
            string path = find_command(args[0]);
            if(!path.empty()) {
                pid_t pid = fork();
                if(pid == 0) {
                    // Child process
                    vector<char*> c_args;
                    for(const string& arg : args) {
                        c_args.push_back(const_cast<char*>(arg.c_str()));
                    }
                    c_args.push_back(nullptr);
                    
                    execv(path.c_str(), c_args.data());
                    exit(1); // If execv fails
                } else if(pid > 0) {
                    // Parent process
                    int status;
                    waitpid(pid, &status, 0);
                }
            } else {
                cout << args[0] << ": command not found" << endl;
            }
        }
    }
}