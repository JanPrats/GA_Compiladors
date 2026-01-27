# Input/Output Diagram - C Preprocessor

## Overview
Aquest diagrama mostra el flux complet d'entrada/sortida del preprocessor, des dels arguments de línia de comandes fins a la generació del fitxer de sortida processat.

---

## Main Input/Output Flow

```
┌──────────────────────────────────────────────────────────────────────┐
│                         PREPROCESSOR SYSTEM                          │
└──────────────────────────────────────────────────────────────────────┘

INPUT                           PROCESSING                      OUTPUT
═════                           ══════════                      ══════

┌─────────────────┐
│ Command Line    │
│   Arguments     │
│                 │
│  ./preproc      │
│    -c           │──┐
│    -d           │  │
│    input.c      │  │
│    output.c     │  │
└─────────────────┘  │
                     │
┌─────────────────┐  │![alt text](image.png)
│  Input File     │  │         ┌──────────────────┐
│   (source.c)    │──┤────────>│  module_args     │
│                 │  │         │                  │
│ - C code        │  │         │ Parse arguments  │
│ - #include      │  │         │ Create ArgFlags  │
│ - #define       │  │         └────────┬─────────┘
│ - #ifdef/endif  │  │                  │
│ - Macros        │  │                  ▼
│ - Comments      │  │         ┌──────────────────┐
└─────────────────┘  │         │  init_parser()   │
                     │         │                  │
┌─────────────────┐  │         │ Create           │
│ Included Files  │  │         │ ParserState +    │
│  (#include)     │──┘         │ MacroDict        │
│                 │            └────────┬─────────┘
│ - header.h      │                     │
│ - library.h     │                     ▼
└─────────────────┘            ┌──────────────────┐
                               │  parse_until()   │
                               │                  │
                               │  Main Parser     │
                               │  Loop            │
                               └────────┬─────────┘
                                        │
                     ┌──────────────────┼──────────────────┐
                     │                  │                  │
                     ▼                  ▼                  ▼
          ┌──────────────────┐ ┌──────────────┐  ┌──────────────┐
          │ Comments Remove  │ │   Includes   │  │   Defines    │
          │                  │ │              │  │              │
          │ Remove // and    │ │ Process      │  │ Process      │
          │ /* */ comments   │ │ #include     │  │ #define      │
          │ (if -c flag)     │ │ directives   │  │ directives   │
          └──────────────────┘ └──────────────┘  └──────────────┘
                     │                  │                  │
                     └──────────────────┼──────────────────┘
                                        │
                     ┌──────────────────┼──────────────────┐
                     │                  │                  │
                     ▼                  ▼                  ▼
          ┌──────────────────┐ ┌──────────────┐  ┌──────────────┐
          │  #ifdef/#endif   │ │    Macros    │  │    Errors    │
          │                  │ │              │  │              │
          │ Conditional      │ │ Expand       │  │ Collect and  │
          │ compilation      │ │ macro calls  │  │ report       │
          │ (if -d flag)     │ │              │  │ errors       │
          └──────────────────┘ └──────────────┘  └──────────────┘
                                        │
                                        ▼
                               ┌──────────────────┐
                               │  Output File     │
                               │                  │
                               │ Write processed  │
                               │ code             │
                               └────────┬─────────┘
                                        │
                                        ▼
                             ┌─────────────────────┐
                             │   Output File       │
                             │   (processed.c)     │
                             │                     │
                             │ - Expanded macros   │
                             │ - Included headers  │
                             │ - No comments (if)  │
                             │ - Conditional code  │
                             └─────────────────────┘
                                        │
                                        ▼
                             ┌─────────────────────┐
                             │ Console (stdout)    │
                             │                     │
                             │ - Status messages   │
                             │ - Error reports     │
                             │ - Debug info        │
                             └─────────────────────┘
```

---

## Detailed Input Specifications

### 1. Command Line Arguments

```
./preprocessor [FLAGS] <input_file> [output_file]

FLAGS:
  -c              Remove comments from source code
  -d              Process preprocessor directives
  -help           Show help message

EXAMPLES:
  ./preprocessor -c -d input.c output.c
  ./preprocessor -c input.c
  ./preprocessor -help
```

**Input Processing:**
```
module_args:
  ├─> Parse command line
  ├─> Validate flags
  ├─> Extract input filename
  ├─> Extract output filename (default: based on input)
  └─> Create ArgFlags structure
```

### 2. Input Files

#### Main Input File
```c
// Example: input.c
#include <stdio.h>
#include "myheader.h"

#define MAX 100
#define MIN(a,b) ((a)<(b)?(a):(b))

#ifdef DEBUG
    printf("Debug mode\n");
#endif

int main() {
    int x = MAX;  // Will be expanded to: int x = 100;
    // This is a comment - will be removed if -c flag
    return 0;
}
```

#### Included Files (#include)
```c
// Example: myheader.h
#ifndef MYHEADER_H
#define MYHEADER_H

#define VERSION 1.0

void my_function(void);

#endif
```

---

## Processing Stages

### Stage 1: Initialization

```
┌──────────────────────────────────────────────┐
│           init_parser()                      │
│                                              │
│  INPUT:  ArgFlags* flags                     │
│          const char* input_file              │
│          const char* output_file             │
│                                              │
│  ACTIONS:                                    │
│    1. Open input file (read mode)           │
│    2. Open output file (write mode)         │
│    3. Create MacroDict                      │
│    4. Initialize ParserState                │
│    5. Set flags from arguments              │
│                                              │
│  OUTPUT: ParserState* (initialized)          │
└──────────────────────────────────────────────┘
```

### Stage 2: Character-by-Character Processing

```
┌────────────────────────────────────────────────────────────┐
│                    parse_until()                           │
│                                                            │
│  Character Reading:                                        │
│    read_char()  ──> Consume character from input          │
│    peek_char()  ──> Look ahead without consuming          │
│    unread_char() ──> Push back one character              │
│                                                            │
│  Word/Token Reading:                                       │
│    read_word()  ──> Read complete identifier/keyword      │
│    read_line()  ──> Read until end of line                │
│                                                            │
│  State Tracking:                                           │
│    in_string    ──> Are we inside "..." ?                 │
│    in_comment   ──> Are we inside /* ... */ ?             │
│    lookahead    ──> Buffered next character               │
└────────────────────────────────────────────────────────────┘
```

### Stage 3: Directive Processing (if -d flag)

```
┌──────────────────────────────────────────────┐
│  Directive Detection & Processing            │
│                                              │
│  #include "file.h"                           │
│     └──> process_include()                   │
│          ├─> Find file                       │
│          ├─> Open file                       │
│          ├─> Parse recursively               │
│          └─> Insert content                  │
│                                              │
│  #define NAME value                          │
│     └──> process_define()                    │
│          ├─> Extract macro name              │
│          ├─> Extract macro value             │
│          └─> Add to MacroDict                │
│                                              │
│  #ifdef SYMBOL                               │
│  #endif                                      │
│     └──> Conditional processing              │
│          ├─> Check if macro defined          │
│          ├─> Include/exclude block           │
│          └─> Nested ifdef support            │
└──────────────────────────────────────────────┘
```

### Stage 4: Comment Removal (if -c flag)

```
┌──────────────────────────────────────────────┐
│         Comment Detection & Removal          │
│                                              │
│  Single-line comments:                       │
│    // comment text                           │
│    └──> Remove until \n                      │
│                                              │
│  Multi-line comments:                        │
│    /* comment                                │
│       more text                              │
│       more text */                           │
│    └──> Remove entire block                  │
│                                              │
│  Preserve strings:                           │
│    "This // is not a comment"                │
│    └──> Keep intact (in_string flag)         │
└──────────────────────────────────────────────┘
```

### Stage 5: Macro Expansion

```
┌──────────────────────────────────────────────┐
│           Macro Substitution                 │
│                                              │
│  Simple macros:                              │
│    MAX ──> 100                               │
│                                              │
│  Function-like macros:                       │
│    MIN(x,y) ──> ((x)<(y)?(x):(y))            │
│                                              │
│  Process:                                    │
│    1. read_word() ──> identifier             │
│    2. substitute_macro() ──> lookup          │
│    3. Replace with value from MacroDict      │
│    4. Write to output                        │
└──────────────────────────────────────────────┘
```

---

## Output Specifications

### 1. Primary Output File

```c
// Example: output.c (after processing)

// Included content from <stdio.h>
// Included content from "myheader.h"

// Comments removed (if -c)
// Macros expanded:
int main() {
    int x = 100;  // MAX expanded
    return 0;
}
```

**Output Characteristics:**
- ✅ All #include directives resolved and content inserted
- ✅ All #define macros expanded
- ✅ Comments removed (if -c flag)
- ✅ Conditional blocks processed (if -d flag)
- ✅ Valid C source code ready for compilation

### 2. Console Output (stdout/stderr)

```
Starting module args ...
Input file: input.c
Output file: output.c
Flags: remove_comments=1, process_directives=1
Preprocessing...
Preprocessing completed!
Output written to: output.c
```

**Error Messages (stderr):**
```
Error: Could not open file 'missing.h'
Error: Macro already defined 'MAX'
Error: Unmatched #endif directive
```

---

## Data Flow Summary

### Character Level Flow
```
Input File ──┐
             │
             ├──> fgetc() ──> read_char() ──┐
             │                               │
Lookahead <──┘                               │
Buffer                                       ▼
                                    ┌──────────────────┐
                                    │  State Machine   │
                                    │                  │
                                    │  • in_string     │
                                    │  • in_comment    │
                                    │  • processing    │
                                    └────────┬─────────┘
                                             │
                                             ▼
                                    ┌──────────────────┐
                                    │  Decision Tree   │
                                    │                  │
                                    │  Comment?        │
                                    │  Directive?      │
                                    │  Macro?          │
                                    │  Regular code?   │
                                    └────────┬─────────┘
                                             │
                                             ▼
Output File <──────────────────────── fputc()/fprintf()
```

### Module Interaction Flow
```
Input ──> module_args ──> ArgFlags
                            │
                            ▼
                     init_parser() ──> ParserState + MacroDict
                            │
                            ▼
                     parse_until() <──┐
                            │          │
        ┌───────────────────┼──────────┴──────────────┐
        │                   │                         │
        ▼                   ▼                         ▼
  Comments Module    Directive Modules         Macro Module
        │                   │                         │
        │            ┌──────┴──────┐                  │
        │            ▼             ▼                  │
        │      Include Module  Define Module          │
        │            │             │                  │
        │            │      ifdef/endif Module        │
        │            │             │                  │
        └────────────┴─────────────┴──────────────────┘
                            │
                            ▼
                     Output File
```

### Memory Flow
```
Stack:                          Heap:
┌──────────────┐               ┌──────────────────┐
│ main()       │               │ ParserState*     │
│   ├─ argc    │               │   ├─ current_file│
│   ├─ argv    │               │   ├─ output_file │
│   └─ result  │               │   └─ macro_dict  │
└──────────────┘               │                  │
                               │ MacroDict*       │
┌──────────────┐               │   ├─ entries[]   │
│ ArgFlags*    │ ─────────────>│   └─ count       │
│   ├─ ifile   │               └──────────────────┘
│   ├─ ofile   │
│   └─ flags   │
└──────────────┘
```

---

## Input/Output Examples

### Example 1: Basic Preprocessing

**Input (input.c):**
```c
#define SIZE 10
int arr[SIZE];
// Comment
```

**Command:**
```bash
./preprocessor -c -d input.c output.c
```

**Output (output.c):**
```c

int arr[10];

```

### Example 2: With Includes

**Input (main.c):**
```c
#include "header.h"
int x = MAX_VALUE;
```

**Input (header.h):**
```c
#define MAX_VALUE 100
```

**Output:**
```c

#define MAX_VALUE 100

int x = 100;
```

### Example 3: Conditional Compilation

**Input:**
```c
#define DEBUG
#ifdef DEBUG
    printf("Debug\n");
#endif
```

**Output:**
```c


    printf("Debug\n");

```

---

## Performance Metrics

### File Processing
```
Input File Size:        10 KB
Number of Lines:        300
Number of Macros:       15
Number of Includes:     5
Processing Time:        < 100ms
Output File Size:       ~12 KB (after expansion)
```

### Memory Usage
```
ParserState:            ~1 KB
MacroDict:              ~1.3 MB (MAX_MACROS * sizeof(MacroEntry))
Buffers:                ~8 KB (line buffers, word buffers)
Total Peak Memory:      ~2 MB
```

---

## Error Handling Flow

```
Error Detection ──┐
                  │
                  ▼
┌─────────────────────────────────────┐
│       module_errors                 │
│                                     │
│  ├─> errors_init()                  │
│  ├─> report_error()                 │
│  ├─> errors_count()                 │
│  └─> errors_finalize()              │
└──────────────┬──────────────────────┘
               │
               ├──> Log to stderr
               │
               └──> Set exit code (return 1)
```

**Example Errors:**
- File not found
- Macro redefinition
- Unmatched #ifdef/#endif
- Invalid directive syntax
- Memory allocation failure

---

## Summary

**Inputs:**
1. Command line arguments (flags, filenames)
2. Input source file (.c)
3. Included header files (.h)

**Processing:**
1. Argument parsing → ArgFlags
2. Parser initialization → ParserState + MacroDict
3. Character-by-character parsing
4. Directive processing (#include, #define, #ifdef)
5. Comment removal (optional)
6. Macro expansion

**Outputs:**
1. Processed source file (expanded, clean)
2. Console messages (status, errors)
3. Error codes (success/failure)

El preprocessor transforma codi C amb directives i macros en codi C net i expandit, llest per ser compilat.
