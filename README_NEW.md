# CodeCrafters Shell in C++

A POSIX-compliant shell implementation built as part of the [CodeCrafters Shell Challenge](https://app.codecrafters.io/courses/shell/overview). This shell supports advanced features including built-in commands, external program execution, quote handling, and more.

## Features

✅ **Complete REPL (Read-Eval-Print Loop)**
- Interactive shell prompt (`$ `)
- Command parsing and execution
- Proper error handling

✅ **Built-in Commands**
- `echo` - Display text with argument support
- `pwd` - Print working directory
- `cd` - Change directory (supports `~` for home)
- `type` - Identify command types (builtin vs external)
- `exit` - Exit shell with optional exit codes

✅ **External Program Execution**
- PATH-based binary lookup
- Fork/exec process management
- Environment variable passing
- Exit status handling

✅ **Advanced Command Parsing**
- Double quote support (`"quoted strings"`)
- Escape sequence handling (`\` escapes)
- Space-separated argument parsing
- Proper tokenization

✅ **Environment Management**
- PATH, PWD, HOME variable handling
- Environment variable inheritance
- Dynamic environment updates

## Building and Running

### Prerequisites
- CMake 3.13+
- C++23 compatible compiler (GCC 11+ or Clang 14+)

### Build Instructions
```bash
# Configure the build
cmake -B build -S .

# Build the project
cmake --build ./build

# Run the shell
./build/shell
```

Or use the provided script:
```bash
./your_program.sh
```

## Usage Examples

```bash
$ echo Hello, World!
Hello, World!

$ pwd
/Users/tanmay/Documents/code

$ cd ~
$ pwd
/Users/tanmay

$ type echo
echo is a shell builtin

$ type ls
ls is /bin/ls

$ ls -la
# (executes external ls command)

$ exit 0
```

## Architecture

The shell is implemented in modern C++23 with the following key components:

- **Command Parser**: Handles tokenization, quoting, and escaping
- **Built-in Commands**: Dedicated functions for shell builtins
- **External Execution**: Fork/exec model for external programs
- **Environment Management**: Dynamic environment variable handling
- **Error Handling**: Proper error codes and messages

## File Structure

```
├── src/
│   ├── main.cpp      # Main shell implementation
│   ├── utility.cpp   # Utility functions
│   └── utility.hpp   # Utility headers
├── CMakeLists.txt    # Build configuration
└── your_program.sh   # Build and run script
```

## CodeCrafters Progress

This implementation successfully passes multiple CodeCrafters shell stages:
- ✅ Basic REPL
- ✅ Handle unknown commands
- ✅ Exit command with codes
- ✅ Echo command
- ✅ Type command (builtins and externals)
- ✅ PWD command
- ✅ CD command with home directory support
- ✅ Advanced quote and escape handling

## Development

The shell was developed incrementally through the CodeCrafters challenge, with each stage adding new functionality. The codebase demonstrates:

- Modern C++ practices (C++23 features)
- POSIX system call usage
- Memory-safe string handling
- Robust error handling
- Clean separation of concerns

## License

This project was created as part of the CodeCrafters learning platform. Feel free to use it as a reference for your own shell implementation!

---

**Author**: [rhyode](https://github.com/rhyode)  
**Platform**: CodeCrafters Shell Challenge  
**Language**: C++23
