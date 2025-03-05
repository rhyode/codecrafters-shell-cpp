#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <unordered_set>
// #include <sys/socket.h>
#include <unistd.h>
#include "utility.hpp"
 
void commandType(const std::vector<std::string>& command, int& exitStatus) {
  std::unordered_set<std::string> builtinCmdSet{"type", "exit", "echo", "pwd"};
  auto it = builtinCmdSet.find(command[0]);
  if (it != builtinCmdSet.end()) {  // command is builtin
    std::cout << command[0] << " is a shell builtin\n";
    exitStatus = 0;
  }
  else {
    auto filepath = utility::searchForExecutableInPathDirs(command[0]);
    if (filepath) {
      std::cout << command[0] << " is " << *filepath << "\n";
      exitStatus = 0;
    }
    else {
        std::cout << command[0] << ":" << " not found\n";
        exitStatus = 1;
    }
  }
}
void getParameters(std::string& input, std::vector<std::string>& commandParameters) {
  bool singleQuoteStart = false;  // toggle to true when single quote starts and false when single quote ends
  bool doubleQuoteStart = false;  // toggle to true when double quote starts and false when double quote ends
  std::ostringstream oss;
  // input = "echo \"shell test\" \"testscript\"";
  // DEBUG_LOG("input = " + input);
  commandParameters.clear();
  size_t pos = input.find(" ");
  if (pos == std::string::npos) {
    return;
  }
  // DEBUG_LOG("pos = " + std::to_string(pos) + " (position for 1st space)");
  pos++;
  while (pos < input.length()) {
    if ((!singleQuoteStart) && ('\'' == input[pos])) {
      // single quote started
      /* Enclosing characters in single quotes (‘'’) preserves the literal
         value of each character within the quotes. A single quote may not
         occur between single quotes, even when preceded by a backslash.
       */
      //  DEBUG_LOG("in single quote start, pos = " + std::to_string(pos));
      singleQuoteStart = true;
      // singleQuoteEnd = false;
      auto endQuote = input.find("'", pos+1);
      // DEBUG_LOG("before endQuote = " + std::to_string(endQuote));
      if (endQuote == std::string::npos) {
        endQuote = input.length();
      }
      // DEBUG_LOG("after endQuote = " + std::to_string(endQuote));
      oss.clear();
      oss.str("");
      for (pos++; pos < endQuote; pos++) {
        oss << input[pos];
      }
      singleQuoteStart = false;
      commandParameters.push_back(oss.str());
      // DEBUG_LOG("pos = " + std::to_string(pos) + ", commandParameters.size() = " + std::to_string(commandParameters.size()));
      oss.clear();
      oss.str("");
      pos++; // skip ending single quote
      // std::cout << "\n";
    }
    // else if (singleQuoteStart && ('\'' == input[pos])) {
    //   // single quote ended
    //   singleQuoteStart = false;
    //   singleQuoteEnd = true;
    // }
    else if ((!doubleQuoteStart) && ('"' == input[pos])) {
      doubleQuoteStart = true;
      auto endQuote = input.find("\"", pos+1);
      // DEBUG_LOG("before endQuote = " + std::to_string(endQuote));
      if (endQuote == std::string::npos) {
        endQuote = input.length();
      }
      // DEBUG_LOG("after endQuote = " + std::to_string(endQuote));
      oss.clear();
      oss.str("");
      for (pos++; pos < endQuote; pos++) {
        if (pos+1 < input.length() && input[pos] == '\\' &&  (input[pos+1]== '$' || input[pos+1]=='`' || input[pos+1]=='\\')) {
          oss << input[++pos];
        }
        else {
          oss << input[pos];
        }
      }
      doubleQuoteStart = false;
      // std::cout << "before, commandParameters = ";
      // for (int i = 0; i < commandParameters.size(); i++) {
      //   char ch = (i==(commandParameters.size()-1)) ? '\n' : ' ';
      //   std::cout << commandParameters[i] << ch;
      // }
      commandParameters.push_back(oss.str());
      // DEBUG_LOG("pos = " + std::to_string(pos) + ", commandParameters.size() = " + std::to_string(commandParameters.size()));
      oss.clear();
      oss.str("");
      pos++; // go past the ending double quote
      // std::cout << "after, commandParameters = ";
      // for (int i = 0; i < commandParameters.size(); i++) {
      //   char ch = (i==(commandParameters.size()-1)) ? '\n' : ' ';
      //   std::cout << commandParameters[i] << ch;
      // }
    }
    else if ('\\' == input[pos] && (pos + 1 < input.length()) /* && ' ' == input[pos+1] */) {
      oss << input[++pos];
      pos++;
    }
    else if (' ' == input[pos] && (!singleQuoteStart) && (!doubleQuoteStart)) {
      // DEBUG_LOG("pos = " + std::to_string(pos) + ", skipping space");
      pos++;
    }
    else {
      // simple command without single/double quotes
      oss.clear();
      oss.str("");
      while(pos < input.length()) {
        if (input[pos] == ' ') {
          pos++;
          break;
        }
        else if ('\\' == input[pos] && (pos + 1 < input.length()) /* && ' ' == input[pos+1] */) {
          oss << input[++pos];
          ++pos;
        }
        else if(input[pos] == '\'' || input[pos]=='"') {
          break;
        }
        else {
          oss << input[pos++];
        }
      }
      commandParameters.push_back(oss.str());
    }
  }
  // DEBUG_LOG("commandParameters.size() = " + std::to_string(commandParameters.size()));
  // for (auto& c : commandParameters) {
  //   std::cerr << "|" << c << "|\n";
  // }
  return;
}
bool REPL(int& exitStatus) {
  bool isTerminate = false;
  // read 
  std::cout << "$ ";
  std::string input;
  std::getline(std::cin, input);
  // input = "echo \"before\\   after\"";
  // evaluate and print
  // std::vector<std::string> command = utility::split(input, "[ ]+");
  std::vector<std::string> command; // = utility::split(input, "[ ]+");
// DEBUG_LOG ("command[0]= " + command[0]); 
  if (0 == input.find("type")) {
    getParameters(input, command);
    commandType(command, exitStatus);
  }
  else if (0 == input.find("exit")) {
    command = utility::split(input, "[ ]+");
    exitStatus = std::stoi(command[1]);
    isTerminate = true;
  }
  else if (0 == input.find("cat")) {
    getParameters(input, command);
    for(auto& filename : command) {
      std::ifstream fin(filename);
      if(!fin) {
        std::cerr << "cat: error opening file: " << filename;
        continue;
      }
      std::string line;
      while(std::getline(fin, line)) {
        std::cout << line;
      }
      fin.close();
    }
    std::cout << "\n";
  }
  else if (0 == input.find("echo")) {
    command.clear();
    std::ostringstream oss;
    // oss << "\nbefore command : " << command.size() << "\n";
    // for (auto&c : command) {
    //   oss << "|" << c << "|\n";
    // }
    // DEBUG_LOG(oss.str());
    getParameters(input, command);
    // oss.clear();
    // oss.str("");
    // oss << "\nparsed commands : " << command.size() << "\n";
    // for (auto&c : command) {
    //   oss << "|" << c << "|\n";
    // }
    // DEBUG_LOG(oss.str());
    
    if (command.size() < 1) {
      DEBUG_LOG(utility::colourize("Few arguments provided for echo command", utility::cc::RED));
      std::cout << std::endl;
      exitStatus = 1;
    }
    for (int i = 0; i < command.size(); i++) {
      char ch = (i==(command.size()-1)) ? '\n' : ' ';
      std::cout << command[i] << ch;
    }
    // auto startQuote = input.find("'");
    // if (startQuote != std::string::npos) {
      // std::cout << "Quoted : ";
      // handle quoted parameter
     /*  auto endQuote = input.find("'", startQuote+1);
      if (endQuote == std::string::npos) {
        endQuote = input.length();
      }
      */ // std::cout << startQuote << " " << endQuote;
/*       for (int i = startQuote+1; i < endQuote; i++) {
        std::cout << input[i];
      }
      std::cout << "\n"; */
    // }
    /* else { */
      // std::cout << "not quoted ; " << "command.size()=" << command.size();
      // for (auto& c : command) {
        // std::cout << "\'" << c << "'" << "\n";
      // }
     /*  for(int i = 1; i < command.size(); i++) {
        char ch = i == command.size()-1 ? '\n' : ' ';
        std::cout << command[i] << ch;
      }
    } */
   /*  exitStatus = 0;
 */  }
  else if (0 == input.find("pwd")) {
    // std::filesystem::path cwd = std::filesystem::current_path();
    // std::cout << cwd.string() << "\n";
    std::string cmd = "pwd";
    exitStatus = utility::executeShellCommand(cmd);
  }
  else if (0 == input.find("cd")) {
    // exitStatus = utility::executeShellCommand(input);
    getParameters(input, command);  // here cd gets removed from command vector
    std::string path = command[0];
    if (path == "~") {
      path = std::getenv("HOME");
    }
    try {
      std::filesystem::current_path(path);
      exitStatus = 0;
    }
    catch(const std::filesystem::filesystem_error& e) {
      // std::cerr << e.what();
      std::cout << "cd: " << command[0] << ": No such file or directory\n";
      exitStatus = 1;
    }
  }
  else {
    command = utility::split(input, "[ ]+");
    auto filepath = utility::searchForExecutableInPathDirs(command[0]);
    if (filepath) {
      auto pos = input.find(command[0]);
      // DEBUG_LOG("pos=" + std::to_string(pos));
      if (pos != std::string::npos) {
        input.replace(pos, command[0].length(), *filepath);
        if(input.find ("my_exe") == 0) 
          DEBUG_LOG("command is myexe James");
        exitStatus = utility::executeShellCommand(input);
      }
      else {
        std::cout << command[0] << ": command not found\n";
        exitStatus = 1;
      }
    }
    else {
      std::cout << command[0] << ": command not found\n";
      exitStatus = 1;
    }
  }
  return isTerminate;
}
int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  bool isTerminate = false;
  int exitStatus = -1;
  while(!isTerminate) {
    isTerminate = REPL(exitStatus);
  }
  return exitStatus;
}