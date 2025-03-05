#pragma once
#include <string>
#include <vector>
#include <optional>
#include <iostream>

// Define DEBUG_LOG macro
#ifdef DEBUG
    #define DEBUG_LOG(x) std::cerr << x << std::endl
#else
    #define DEBUG_LOG(x)
#endif

namespace utility {
    enum class cc { RED };  // Color codes enum

    std::string colourize(const std::string& text, cc color);
    std::vector<std::string> split(const std::string& input, const std::string& regex);
    std::optional<std::string> searchForExecutableInPathDirs(const std::string& command);
    int executeShellCommand(const std::string& command);
}