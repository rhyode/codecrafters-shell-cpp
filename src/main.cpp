#include <unistd.h>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

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
    if ((params.at(0) == ''') && (params.at(params.size() - 1) == '''))
    {
        result = params.substr(1, params.size() - 1);
        result = result.substr(0, result.size() - 1);
    }
    else
    {
        bool space_found = false;
        bool apos_start = false;
        for (auto c : params)
        {
            if (c == ' ' && !apos_start)
            {
                if (!space_found)
                    space_found = true;
                else
                    continue;
            }
            else if (space_found)
                space_found = false;
            if (c == '"')
            {
                apos_start = !apos_start;
                continue;
            }
            result += c;
        }
    }
    return result;
}

int main()
{
    string env_p = string(getenv("PATH"));
    while (true)
    {
        cout << unitbuf;
        cerr << unitbuf;
        string input;
        cout << "$ ";
        getline(cin, input);
        string exec_name = input.substr(0, input.find(' '));
        string params = input.substr(input.find(' ') + 1,
                                     input.size() - input.find(' ') + 1);
        if (input == "exit 0")
            return 0;
        if (exec_name == "echo")
        {
            cout << EchoMessage(params) << endl;
            continue;
        }
        if (exec_name == "type" && ((params == "echo") || (params == "exit") ||
                                    (params == "type") || (params == "pwd")))
        {
            cout << params << " is a shell builtin" << endl;
            continue;
        }
        if (exec_name == "type")
        {
            string exec_path = SearchExecutable(params, env_p);
            if (exec_path == "")
                cout << params << ": not found" << endl;
            else
            {
                cout << params << " is " << exec_path << endl;
            }
            continue;
        }
        if (exec_name == "cd")
        {
            if (params == "~")
            {
                chdir(getenv("HOME"));
                continue;
            }
            if (fs::exists(fs::path(params)))
            {
                chdir(params.c_str());
                continue;
            }
            cout << exec_name << ": " << params
                 << ": No such file or directory" << endl;
            continue;
        }
        if (SearchExecutable(exec_name, env_p) != "")
        {
            system(input.c_str());
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
