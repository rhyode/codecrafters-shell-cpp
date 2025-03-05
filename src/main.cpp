#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;
namespace fs = std::filesystem;

string SearchExecutable(const string& command, const string& path_env) {
    stringstream ss(path_env);
    string path;
    while (getline(ss, path, ':')) {
        fs::path full_path = fs::path(path) / command;
        if (fs::exists(full_path) && fs::is_regular_file(full_path)) {
            auto perms = fs::status(full_path).permissions();
            if ((perms & fs::perms::owner_exec) != fs::perms::none ||
                (perms & fs::perms::group_exec) != fs::perms::none ||
                (perms & fs::perms::others_exec) != fs::perms::none) {
                return full_path.string();
            }
        }
    }
    return "";
}

string ProcessDoubleQuotedString(const string& input, size_t& i) {
    string result;
    i++; // Skip opening quote
    while (i < input.length() && input[i] != '"') {
        if (input[i] == '\\' && i + 1 < input.length()) {
            char next = input[i + 1];
            if (next == '\\' || next == '"' || next == '$') {
                result += next;
                i += 2;
            } else {
                result += input[i];
                i++;
            }
        } else {
            result += input[i];
            i++;
        }
    }
    return result;
}

string ProcessBackslashes(const string& input) {
    string result;
    for (size_t i = 0; i < input.length(); i++) {
        if (input[i] == '\\') {
            if (i + 1 < input.length()) {
                char next = input[i + 1];
                if (next == '\\') {
                    // For escaped backslash, skip both
                    i++;
                    continue;
                } else if (next == '\'' || next == '"') {
                    // For escaped quotes, just add the quote
                    result += next;
                    i++;
                } else {
                    // For other escaped characters, add the character
                    result += next;
                    i++;
                }
            }
        } else {
            result += input[i];
        }
    }
    return result;
}

vector<string> ParseCommand(const string& input) {
    vector<string> args;
    string current_arg;
    bool in_single_quotes = false;
    bool in_double_quotes = false;
    
    for (size_t i = 0; i < input.length(); i++) {
        if (!in_single_quotes && input[i] == '"') {
            in_double_quotes = !in_double_quotes;
            continue;
        }
        if (!in_double_quotes && input[i] == '\'') {
            in_single_quotes = !in_single_quotes;
            continue;
        }
        
        if (!in_single_quotes && !in_double_quotes && isspace(input[i])) {
            if (!current_arg.empty()) {
                args.push_back(ProcessBackslashes(current_arg));
                current_arg.clear();
            }
        } else {
            current_arg += input[i];
        }
    }
    
    if (!current_arg.empty()) {
        args.push_back(ProcessBackslashes(current_arg));
    }
    
    return args;
}

int main() {
    cout << unitbuf;
    cerr << unitbuf;
    
    while (true) {
        cout << "$ ";
        string line;
        if (!getline(cin, line)) break;
        
        if (line.empty()) continue;
        
        // Extract command name and arguments
        string command = line;
        string arg;
        size_t space_pos = line.find(' ');
        if (space_pos != string::npos) {
            command = line.substr(0, space_pos);
            arg = line.substr(space_pos + 1);
        }
        
        // Handle exit command
        if (command == "exit" && arg == "0") {
            return 0;
        }
        
        // Handle echo command
        if (command == "echo") {
            vector<string> parsed_args = ParseCommand(line);
            if (parsed_args.size() > 1) {
                for (size_t i = 1; i < parsed_args.size(); i++) {
                    cout << parsed_args[i];
                    if (i < parsed_args.size() - 1) cout << " ";
                }
            }
            cout << endl;
            continue;
        }
        
        // Handle type command
        if (command == "type") {
            if (!arg.empty()) {
                if (arg == "echo" || arg == "exit" || arg == "type") {
                    cout << arg << " is a shell builtin" << endl;
                } else {
                    string path_env = getenv("PATH");
                    string exec_path = SearchExecutable(arg, path_env);
                    if (!exec_path.empty()) {
                        cout << arg << " is " << exec_path << endl;
                    } else {
                        cout << arg << ": not found" << endl;
                    }
                }
            }
            continue;
        }
        
        // Handle pwd command
        if (command == "pwd") {
            cout << fs::current_path().string() << endl;
            continue;
        }
        
        // Handle cd command
        if (command == "cd") {
            if (!arg.empty()) {
                fs::path target_path = fs::path(arg);
                try {
                    if (fs::exists(target_path)) {
                        if (chdir(target_path.c_str()) != 0) {
                            cout << "cd: " << arg << ": No such file or directory" << endl;
                        }
                    } else {
                        cout << "cd: " << arg << ": No such file or directory" << endl;
                    }
                } catch (const fs::filesystem_error& e) {
                    cout << "cd: " << arg << ": No such file or directory" << endl;
                }
            }
            continue;
        }
        
        // Handle external commands
        string path_env = getenv("PATH");
        string exec_path = SearchExecutable(command, path_env);
        if (!exec_path.empty()) {
            pid_t pid = fork();
            if (pid == 0) {
                // Child process
                vector<string> args = ParseCommand(line);
                vector<char*> c_args;
                for (const string& arg : args) {
                    c_args.push_back(const_cast<char*>(arg.c_str()));
                }
                c_args.push_back(nullptr);
                
                execv(exec_path.c_str(), c_args.data());
                exit(1);
            } else if (pid > 0) {
                // Parent process
                int status;
                waitpid(pid, &status, 0);
                continue;
            }
        }
        
        cout << command << ": command not found" << endl;
    }
    return 0;  // Added back the return statement
}