# COMPILADORS

## 1. Team — Leaders per pràctica

- P1: Jan Prats Soler u213927 
- P2: Andrea Salló Ribas u214974 
- P3: Gorka Hernández u214951
- P4: Marc Rodríguez Vitolo u215105 
- P5: Clara Serra Borràs u204635 
- P6: Pol García López u214574 

## 2. Overview

This project implements a full **C compilation pipeline** split across three practices: a **preprocessor** (P1), a **lexical scanner** (P2), and a **bottom-up parser** (P3). All three practices live in the **same repository**, each in a dedicated subfolder under `src/`.

This project template is designed to facilitate modular C development with CMake and VS Code 
with github source control and testing with actions. It enables individual module development,
isolated testing, and easy integration into a complete environment that creates several 
executables (one to run the program, and a separate one to test each module in isolation) 
in separete run and debug modes. So the project has several main functions.

The testing is integrated with the github actions so it is automatically run everytime 
push or pull request is done. The output logs of automatic runs are stored in the logs 
directory. The log files are named with the time to avoid overwritting the different runs files.

---

## 3. Repository Organisation

All practices share the same repo — no separate branches or separate repositories are used.

| Practice | Folder | Description |
|----------|--------|-------------|
| P1 – Preprocessor | `src/preprocessor/` | Handles `#include`, `#define`, macros, `#ifdef/#endif`, comment removal |
| P2 – Scanner | `src/scanner/` | Tokenises C source files using DFA automata, produces `.cscn` token files |
| P3 – Parser | `src/parser/` | Bottom-up shift/reduce automaton that parses arithmetic expressions from a token file |

---

## 4. Project Structure

```
├── 
├── .gitignore                      # Excludes build/, logs/, executables, etc.
├── CMakeLists.txt                  # Root CMake: enables testing, adds src/ and tests/
├── README.md                       # This file
├── gitlogs/                        # Timestamped logs from GitHub Actions runs
├── logs/                           # Local execution logs (gitignored)
├── Diagrames de Flux/              # Flowcharts and architectural diagrams P1
│
├── src/
│   ├── CMakeLists.txt              # Adds preprocessor/, scanner/, parser/ subdirs
│   │
│   ├── preprocessor/               # P1 — C Preprocessor
│   │   ├── CMakeLists.txt          # Builds preprocessor executable + module libs
│   │   ├── main.c                  # Entry point: argument parsing → init → parse_until → cleanup
│   │   ├── main.h                  # Includes all preprocessor module headers
│   │   ├── utils_files.c           # File path utilities, timestamped log generation
│   │   ├── utils_files.h           # Header for file utilities
│   │   ├── module_args/            # CLI argument parsing (-c, -d, -help flags)
│   │   │   ├── CMakeLists.txt
│   │   │   ├── module_args.c
│   │   │   └── module_args.h
│   │   ├── module_comments_remove/ # Strips // and /* */ comments from source
│   │   │   ├── CMakeLists.txt
│   │   │   ├── module_comments_remove.c
│   │   │   └── module_comments_remove.h
│   │   ├── module_define/          # Processes #define and substitutes macros
│   │   │   ├── CMakeLists.txt
│   │   │   ├── module_define.c
│   │   │   └── module_define.h
│   │   ├── module_errors/          # Error and warning tracking/reporting
│   │   │   ├── CMakeLists.txt
│   │   │   ├── module_errors.c
│   │   │   └── module_errors.h
│   │   ├── module_ifdef_endif/     # Handles #ifdef / #ifndef / #endif directives
│   │   │   ├── CMakeLists.txt
│   │   │   ├── module_ifdef_endif.c
│   │   │   └── module_ifdef_endif.h
│   │   ├── module_include/         # Recursive #include directive processing
│   │   │   ├── CMakeLists.txt
│   │   │   ├── module_include.c
│   │   │   └── module_include.h
│   │   ├── module_macros/          # Macro expansion utilities
│   │   │   ├── CMakeLists.txt
│   │   │   ├── module_macros.c
│   │   │   └── module_macros.h
│   │   └── module_parser/          # Core parse loop (parse_until, read_char, peek_char)
│   │       ├── CMakeLists.txt
│   │       ├── module_parser.c
│   │       └── module_parser.h     # ParserState, MacroDict, ArgFlags structs
│   │
│   ├── scanner/                    # P2 — Lexical Scanner
│   │   ├── CMakeLists.txt          # Builds scanner executable + module libs
│   │   ├── main.c                  # Entry point: init → automata → driver → finalize
│   │   ├── main.h                  # Includes config, automata, error, init headers
│   │   ├── config.h                # Global Status struct, token categories, DFA types
│   │   ├── config.c                # Status allocation, category_to_string, token list ops
│   │   ├── count.h                 # Operation counter macros (COUNT_COMP, COUNT_IO, etc.)
│   │   ├── module_automata/        # DFA engine: transitions, acceptance, driver loop
│   │   │   ├── CMakeLists.txt
│   │   │   ├── module_automata.c   # search_column, is_accepting_state, automata_driver
│   │   │   ├── module_automata.h
│   │   │   └── define_automata.c   # DFA definitions for each token category
│   │   ├── module_error/           # Error/warning tracking for scanner step
│   │   │   ├── CMakeLists.txt
│   │   │   ├── module_error.c
│   │   │   └── module_error.h
│   │   └── module_init/            # Argument parsing, status init, automata init
│   │       ├── CMakeLists.txt
│   │       ├── module_init.c
│   │       └── module_init.h
│   │
│   └── parser/                     # P3 — Bottom-up Parser
│       ├── CMakeLists.txt          # Builds parser executable
│       ├── main.c                  # Entry point: load tokens → init SRA → parse
│       ├── main.h                  # Includes config, load_tokens, module_parser
│       ├── config.h                # Global status, token list, stack, language types
│       ├── config.c                # split_path, generate log filename, set_output_test_file
│       ├── language.txt
│       ├── language2.txt
│       ├── load_tokens/            # Loads .cscn token file into status.all_tokens
│       │   ├── module_load_tokens.c  # Parses <lexeme, CATEGORY> entries; also runs scanner in-memory for .c input
│       │   └── module_load_tokens.h
│       └── module_parser/          # Shift-reduce automaton engine
│           ├── module_parser.c
│           └── module_parser.h
│
└── tests/                          # Standalone module test executables
    ├── CMakeLists.txt
    ├── test_module.h               # Shared test helpers/macros
    ├── test_module_args.c          # Tests for module_args (preprocessor)
    └── test_module_args.h
```

---

## 5. How to Build

### Steps

1. Open the project folder in VS Code.
2. Select the CMake Kit `MSYS2 UCRT64 (preconfigured)` via the CMake extension.
3. Configure the project (usually done automatically on open).
4. Build the project using the **CMake Build** command or the default build task.
5. Run or debug the main executable or individual module tests from the VS Code Run panel or tasks.

---

## 6. How to Run Each Practice

### P1 — Preprocessor

```bash
./preprocessor <input_file.c> [-flags] 
```


| Flag | Effect |
|------|--------|
| `-c` | Remove comments from source code (default if no flag given) |
| `-d` | Process directives (`#include`, `#define`, `#ifdef`, etc.) |
| `-all` | Enable both `-c` and `-d` |
| `-help` | Show usage information |

### P2 — Scanner

```bash
./scanner <input_file.c>
```

Output is written to `<input_file.c>scn` (e.g. `example.c` → `example.cscn`).
Each token is written as `<lexeme, CATEGORY>`.

### P3 — Parser

```bash
./parser <input.cscn> <language.txt>
# or directly from a .c source file:
./parser <input.c>    <language.txt>
```

When given a `.c` file the parser runs the scanner in-memory first, then parses the resulting token stream.

---

## 7. Others

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

## Running and Debugging

- Debug configurations (`launch.json`) are provided for:
  - The main program (`modules_template_main.exe`).
  - Each module test executable (`test_module_args.exe`, `test_module_2.exe`).
- These use **GDB** from the MSYS2 toolchain and are preconfigured for easy debugging in VS Code.

---

## Notes

- The `build/` directory is ignored by git to keep build artifacts out of source control.
- The `logs/` directory can be ignored by git depending if you want to share your output files to the team.
- Environment assumes MSYS2 installed with UCRT64 toolchain available and added to PATH.
- The configuration aims for minimal manual setup to reduce friction between team contributors.
