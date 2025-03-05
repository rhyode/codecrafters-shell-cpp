#include "utility.hpp"
#include <regex>
#include <cstdlib>
#include <filesystem>

namespace utility {
    std::string colourize(const std::string& text, cc color) {
        switch (color) {
            case cc::RED: return "\033[31m" + text + "\033[0m";
            default: return text;
        }
    }

    std::vector<std::string> split(const std::string& input, const std::string& regex) {
        std::regex re(regex);
        std::sregex_token_iterator first{input.begin(), input.end(), re, -1}, last;
        return {first, last};
    }

    std::optional<std::string> searchForExecutableInPathDirs(const std::string& command) {
        if (const char* path = std::getenv("PATH")) {
            std::string pathStr(path);
            std::vector<std::string> paths = split(pathStr, ":");
            
            for (const auto& dir : paths) {
                std::filesystem::path fullPath = std::filesystem::path(dir) / command;
                if (std::filesystem::exists(fullPath) && std::filesystem::is_regular_file(fullPath)) {
                    return fullPath.string();
                }
            }
        }
        return std::nullopt;
    }

    int executeShellCommand(const std::string& command) {
        return system(command.c_str());
    }
}