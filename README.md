# COMPILADORS
  ## 1. Team leaders of each TM

- P1: Jan Prats Soler - u213927
- P2: Andrea Salló Ribas - u214974
- P3: Gorka Hernández - u214951
- P4: Marc Rodríguez Vitolo - u215105
- P5: Clara Serra Borràs - u204635
- P6: Pol García López - u214574
  
---

## 2. Overview

This project template is designed to facilitate modular C development with CMake and VS Code 
with github source control and testing with actions. It enables individual module development,
isolated testing, and easy integration into a complete environment that creates several 
executables (one to run the program, and a separate one to test each module in isolation) 
in separete run and debug modes. So the project has several main functions.

The testing is integrated with the github actions so it is automatically run everytime 
push or pull request is done. The output logs of automatic runs are stored in the logs 
directory. The log files are named with the time to avoid overwritting the different runs files.

---

## Repository Organization Across Practices

All three practices live in the **same repository**, each in a **dedicated sub-folder** under `src/`. No separate branches or separate repositories are used. This keeps the full compilation pipeline visible in one place while keeping each practice's code isolated.

| Practice | Folder | Description |
|----------|--------|-------------|
| P1 – Preprocessor | `src/preprocessor/` | C preprocessor: handles `#include`, `#define`, macros, `#ifdef/#endif`, comment removal |
| P2 – Scanner | `src/scanner/` | Lexical analyser: tokenises C source files using DFA automata, produces `.cscn` token files |
| P3 – Parser | `src/parser/` | Bottom-up parser: shift/reduce automaton engine that parses arithmetic expressions from a token file |

---

## 3. Project Structure
```
├── .git/                   # Git metadata (commits, branches, history)
├── .gitignore              # Files/directories excluded from source control
├── CMakeLists.txt          # Root build configuration generating all targets
├── Diagrames de Flux/      # Flowcharts and architectural diagrams
├── README.md               # Project documentation and onboarding guide
├── gitlogs/                # Archived Git logs for auditing
├── logs/                   # Execution logs (optional to track runs)
├── build/                  # Local CMake build output (ignored by Git)
├── src/                    # Application source code
│   ├── CMakeLists.txt          # Aggregates module executables/libraries
│   ├── input-example.c         # Sample input program for testing
│   ├── input-example.h         # Header associated with the example input
│   ├── main.c                  # Main entry point (orchestrates preprocessing)
│   ├── main.h                  # Shared definitions and includes for main
│   ├── utils_files.c           # File utility helpers (paths, logging, etc.)
│   ├── utils_files.h           # Header for file utility helpers
│   ├── module_args/            # Command-line argument processing module
│   │   ├── CMakeLists.txt          # Build rules for module_args
│   │   ├── module_args.c           # Implementation of argument parsing
│   │   └── module_args.h           # Interface for argument parsing
│   ├── module_comments_remove/ # Comment stripping module
│   │   ├── CMakeLists.txt          # Build rules for comment removal
│   │   ├── module_comments_remove.c# Logic to detect/remove comments
│   │   └── module_comments_remove.h# Interface for comment removal
│   ├── module_define/          # #define processing and macro storage
│   │   ├── CMakeLists.txt          # Build rules for module_define
│   │   ├── module_define.c         # Implementation of macro definitions
│   │   └── module_define.h         # Interface for defining/substituting macros
│   ├── module_errors/          # Error reporting infrastructure
│   │   ├── CMakeLists.txt          # Build rules for module_errors
│   │   ├── module_errors.c         # Error logging/aggregation logic
│   │   └── module_errors.h         # Interface for reporting errors/warnings
│   ├── module_ifdef_endif/     # #ifdef / #ifndef conditional logic
│   │   ├── CMakeLists.txt          # Build rules for conditional module
│   │   ├── module_ifdef_endif.c    # Implementation of conditional parsing
│   │   └── module_ifdef_endif.h    # Interface for conditional handling
│   ├── module_include/         # #include directive processing
│   │   ├── CMakeLists.txt          # Build rules for include module
│   │   ├── module_include.c        # Implementation of recursive includes
│   │   └── module_include.h        # Interface for include handling
│   ├── module_macros/          # Macro expansion utilities
│   │   ├── CMakeLists.txt          # Build rules for macro module
│   │   ├── module_macros.c         # Implementation of macro substitution
│   │   └── module_macros.h         # Interface for macro substitution
│   ├── module_parser/          # Core parsing engine
│   │   ├── CMakeLists.txt          # Build rules for parser module
│   │   ├── module_parser.c         # Implementation of parse_until and helpers
│   │   └── module_parser.h         # Parser state definition and prototypes
│   └── module_2/               # Skeleton/example module
│       ├── CMakeLists.txt          # Build rules for module_2
│       ├── module_2.c              # Example functionality placeholder
│       └── module_2.h              # Header for module_2
└── tests/                   # Standalone executables for module tests
    ├── CMakeLists.txt          # Builds each test target
    ├── test_module.h           # Shared testing helpers/macros
    ├── test_module_2.c         # Test harness for module_2
    ├── test_module_2.h         # Test-specific declarations for module_2
    ├── test_module_args.c      # Test harness for module_args
    └── test_module_args.h      # Test-specific declarations for module_args
```

---

## 4. Tools and Configuration

### CMake

- **CMake** is used as the build system generator.
- The project uses a **modular CMake setup**, with one `CMakeLists.txt` per module for isolated compilation.
- The top-level `CMakeLists.txt` ties all modules together and builds the main executable.
- Unit tests for each module are built as separate executables under `tests/`.

### VS Code Extensions

Recommended extensions for a smooth experience:

- **CMake Tools** (twxs.cmake) — CMake integration and build support.
- **C/C++** (ms-vscode.cpptools) — IntelliSense, debugging, and code browsing.
- **Code Runner** (formulahendry.code-runner) — quick code execution.
- **GitHub Copilot** (GitHub.copilot) — AI-assisted coding.
- **Git Graph** (mhutchie.git-graph) — visual git history.

### CMake Kits (`cmake-kits.json`)

- Preconfigures the MSYS2 UCRT64 toolchain:
  - Compiler: `gcc.exe` from MSYS2.
  - Generator: `MinGW Makefiles`.

This allows to select the proper compiler and generator in VS Code easily.

---

## 5. Building the Project

1. Open the project folder in VS Code.
2. Select the CMake Kit `MSYS2 UCRT64 (preconfigured)` via the CMake extension.
3. Configure the project (usually done automatically on open).
4. Build the project using the **CMake Build** command or the default build task.
5. Run or debug the main executable or individual module tests from the VS Code Run panel or tasks.

---

## 6. Running and Debugging

- Debug configurations (`launch.json`) are provided for:
  - The main program (`modules_template_main.exe`).
  - Each module test executable (`test_module_args.exe`, `test_module_2.exe`).
- These use **GDB** from the MSYS2 toolchain and are preconfigured for easy debugging in VS Code.

### How to run P3

```bash
Run the parser on the token file
$ ./parser  <input.cscn>  <language.txt>
or
$ ./parser  <input.c>  <language.txt>    
```

---

## 7. How to Extend the Template

- Add new modules by creating a new folder under `src/` with its own `CMakeLists.txt`.
- Add corresponding test files under `tests/` and update the tests `CMakeLists.txt`.
- Extend `launch.json` and `tasks.json` to include debugging and build tasks for new modules.

---

## 8. Notes

- The `build/` directory is ignored by git to keep build artifacts out of source control.
- The `logs/` directory can be ignored by git depending if you want to share your output files to the team.
- Environment assumes MSYS2 installed with UCRT64 toolchain available and added to PATH.
- The configuration aims for minimal manual setup to reduce friction between team contributors.
