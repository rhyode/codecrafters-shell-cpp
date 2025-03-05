#include <unistd.h>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std;
namespace fs = std::filesystem;

string SearchExecutable(const string &executable_name, const string &env_p)
{
    stringstream ss(env_p);
    vector<string> paths;
    string p;
    while (getline(ss, p, ':'))
    {
        paths.push_back(p);
    }
    for (const auto &path : paths)
    {
        try
        {
            for (const auto &entry : fs::recursive_directory_iterator(path))
            {
                if (entry.is_regular_file() &&
                    entry.path().filename() == executable_name)
                {
                    auto perms = entry.status().permissions();
                    if ((perms & fs::perms::owner_exec) != fs::perms::none ||
                        (perms & fs::perms::group_exec) != fs::perms::none ||
                        (perms & fs::perms::others_exec) != fs::perms::none)
                    {
                        return entry.path().c_str();
                    }
                }
            }
        }
        catch (const exception &ex)
        {
        }
    }
    return "";
}

string EchoMessage(const string &params)
{
    string result = "";
    bool in_double_quotes = false;
    bool in_single_quotes = false;
    
    for (size_t i = 0; i < params.length(); i++) {
        if (!in_single_quotes && params[i] == '"') {
            in_double_quotes = !in_double_quotes;
            continue;
        }
        if (!in_double_quotes && params[i] == '\'') {
            in_single_quotes = !in_single_quotes;
            continue;
        }
        
        if (params[i] == '\\' && i + 1 < params.length()) {
            if (!in_single_quotes) {
                if (in_double_quotes) {
                    if (params[i + 1] == '\\' || params[i + 1] == '"') {
                        result += params[i + 1];
                    } else {
                        result += '\\';
                        result += params[i + 1];
                    }
                } else {
                    result += params[i + 1];
                }
                i++;
                continue;
            }
        }
        result += params[i];
    }
    
    return result;
}

vector<string> ParseCommand(const string& input) {
    vector<string> args;
    string current_arg;
    bool in_double_quotes = false;
    bool in_single_quotes = false;
    
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
                args.push_back(current_arg);
                current_arg.clear();
            }
        } else {
            if (input[i] == '\\' && i + 1 < input.length()) {
                if (!in_single_quotes) {
                    i++;
                    current_arg += input[i];
                    continue;
                }
            }
            current_arg += input[i];
        }
    }
    
    if (!current_arg.empty()) {
        args.push_back(current_arg);
    }
    
    return args;
}

string ProcessPath(const string& path) {
    string result;
    for (size_t i = 0; i < path.length(); i++) {
        if (path[i] == '\\' && i + 1 < path.length()) {
            i++;
            if (path[i] == 'n') {
                result += '\n';
            } else {
                result += path[i];
            }
        } else {
            result += path[i];
        }
    }
    return result;
}

int main() {
    string env_p = string(getenv("PATH"));
    while (true) {
        cout << unitbuf;
        cerr << unitbuf;
        string input;
        cout << "$ ";
        getline(cin, input);
        
        vector<string> args = ParseCommand(input);
        if (args.empty()) continue;
        
        string exec_name = args[0];
        if (exec_name == "exit" && args.size() > 1 && args[1] == "0")
            return 0;
            
        if (exec_name == "echo") {
            for (size_t i = 1; i < args.size(); i++) {
                cout << args[i];
                if (i < args.size() - 1) cout << " ";
            }
            cout << endl;
            continue;
        }
        
        if (exec_name == "type") {
            if (args.size() > 1) {
                string cmd = args[1];
                if (cmd == "echo" || cmd == "exit" || cmd == "type" || cmd == "pwd") {
                    cout << cmd << " is a shell builtin" << endl;
                } else {
                    string exec_path = SearchExecutable(cmd, env_p);
                    if (exec_path == "") {
                        cout << cmd << ": not found" << endl;
                    } else {
                        cout << cmd << " is " << exec_path << endl;
                    }
                }
            }
            continue;
        }
        
        if (exec_name == "cd") {
            if (args.size() < 2) continue;
            string target_path = args[1];
            if (target_path == "~") {
                chdir(getenv("HOME"));
                continue;
            }
            if (fs::exists(fs::path(target_path))) {
                chdir(target_path.c_str());
                continue;
            }
            cout << exec_name << ": " << target_path
                 << ": No such file or directory" << endl;
            continue;
        }
        if (exec_name == "cat") {
            pid_t pid = fork();
            if (pid == 0) {
                // Child process
                vector<string> processed_paths;
                vector<char*> c_args;
                c_args.push_back(const_cast<char*>(exec_name.c_str()));
                
                for (size_t i = 1; i < args.size(); i++) {
                    processed_paths.push_back(ProcessPath(args[i]));
                    c_args.push_back(const_cast<char*>(processed_paths.back().c_str()));
                }
                c_args.push_back(nullptr);
                
                string cat_path = SearchExecutable("cat", env_p);
                execv(cat_path.c_str(), c_args.data());
                exit(1);
            } else if (pid > 0) {
                // Parent process
                int status;
                waitpid(pid, &status, 0);
            }
            continue;
        }
        
        if (SearchExecutable(exec_name, env_p) != "") {
            pid_t pid = fork();
            if (pid == 0) {
                // Child process
                vector<char*> c_args;
                for (const string& arg : args) {
                    c_args.push_back(const_cast<char*>(arg.c_str()));
                }
                c_args.push_back(nullptr);
                
                execv(SearchExecutable(exec_name, env_p).c_str(), c_args.data());
                exit(1);
            } else if (pid > 0) {
                // Parent process
                int status;
                waitpid(pid, &status, 0);
            }
            continue;
        }
        if (input == "pwd")
        {
            system(input.c_str());
            continue;
        }
        cout << exec_name << ": not found" << endl;
    }
}
