# Data Structures Diagram - C Preprocessor

## Overview
Aquest diagrama mostra les estructures de dades principals utilitzades al preprocessor de C i les seves relacions.

```
┌─────────────────────────────────────────────────────────────────────┐
│                          PREPROCESSOR                               │
└─────────────────────────────────────────────────────────────────────┘
                                  │
                                  │
                    ┌─────────────┴─────────────┐
                    │                           │
                    ▼                           ▼
        ┌────────────────────┐      ┌──────────────────┐
        │    ParserState     │      │     ArgFlags     │
        └────────────────────┘      └──────────────────┘
                    │
                    │ contains
                    │
                    ▼
        ┌────────────────────┐
        │    MacroDict       │
        └────────────────────┘
                    │
                    │ contains array of
                    │
                    ▼
        ┌────────────────────┐
        │    MacroEntry      │
        └────────────────────┘
```

---

## 1. **ArgFlags** - Arguments Configuration

```c
typedef struct ArgFlags {
    bool remove_comments;           // -c flag
    bool process_directives;        // -d flag
    bool show_help;                 // -help flag
    char ifile[MAX_FILENAME];       // input filename (512)
    char ofile[MAX_FILENAME];       // output filename (512)
} ArgFlags;
```

**Purpose:** Emmagatzema la configuració dels arguments de línia de comandes.

**Attributes:**
- `remove_comments`: Flag per eliminar comentaris
- `process_directives`: Flag per processar directives
- `show_help`: Flag per mostrar ajuda
- `ifile`: Nom del fitxer d'entrada
- `ofile`: Nom del fitxer de sortida

---

## 2. **MacroEntry** - Single Macro Definition

```c
typedef struct MacroEntry {
    char name[MAX_MACRO_NAME];      // 256 chars
    char value[MAX_MACRO_VALUE];    // 1024 chars
    bool is_defined;
} MacroEntry;
```

**Purpose:** Representa una única definició de macro (#define).

**Attributes:**
- `name`: Nom de la macro
- `value`: Valor de substitució de la macro
- `is_defined`: Indica si la macro està definida

---

## 3. **MacroDict** - Macro Dictionary

```c
typedef struct MacroDict {
    MacroEntry entries[MAX_MACROS]; // Array of 1024 entries
    int count;
} MacroDict;
```

**Purpose:** Diccionari que conté totes les macros definides.

**Attributes:**
- `entries`: Array estàtic de MacroEntry (màxim 1024 macros)
- `count`: Comptador del nombre de macros actuals

**Operations:**
- Cerca de macros per nom
- Afegir noves macros
- Comprovar si una macro està definida

---

## 4. **ParserState** - Main Parser State

```c
typedef struct ParserState {
    FILE* current_file;             // Input file handle
    FILE* output_file;              // Output file handle
    char current_filename[MAX_FILENAME];  // 512 chars
    int current_line;
    MacroDict* macro_dict;          // Pointer to macro dictionary
    bool remove_comments;           // -c flag
    bool process_directives;        // -d flag
    bool in_string;                 // Inside string literal
    bool in_comment;                // Inside comment block
    char lookahead;                 // Next character (lookahead)
    bool has_lookahead;             // Lookahead buffer status
} ParserState;
```

**Purpose:** Estat principal del parser, conté tota la informació necessària per al processament.

**Attributes:**
- **File Management:**
  - `current_file`: Handle del fitxer actual d'entrada
  - `output_file`: Handle del fitxer de sortida
  - `current_filename`: Nom del fitxer actual
  - `current_line`: Número de línia actual

- **Macro Management:**
  - `macro_dict`: Punter al diccionari de macros

- **Processing Flags:**
  - `remove_comments`: Si s'han d'eliminar comentaris
  - `process_directives`: Si s'han de processar directives

- **Parser State:**
  - `in_string`: Indica si estem dins d'una cadena de text
  - `in_comment`: Indica si estem dins d'un comentari
  - `lookahead`: Caràcter següent (per lookahead parsing)
  - `has_lookahead`: Indica si hi ha un caràcter al buffer lookahead

---

## Relationships Between Structures

### Hierarchy
```
ArgFlags (Configuration)
    │
    └──> Used to initialize ──> ParserState (Main State)
                                      │
                                      │ owns
                                      ▼
                                  MacroDict (Dictionary)
                                      │
                                      │ contains array of
                                      ▼
                                  MacroEntry[] (Macro Definitions)
```

### Flow Diagram
```
┌──────────────┐
│ Command Line │
│  Arguments   │
└──────┬───────┘
       │
       ▼
┌──────────────┐     init_parser()      ┌──────────────────┐
│   ArgFlags   │ ──────────────────────> │  ParserState     │
└──────────────┘                         └──────┬───────────┘
                                                │
                                                │ creates/owns
                                                ▼
                                         ┌──────────────────┐
                                         │   MacroDict      │
                                         │                  │
                                         │  ┌─────────────┐ │
                                         │  │ MacroEntry  │ │
                                         │  │ MacroEntry  │ │
                                         │  │    ...      │ │
                                         │  │ MacroEntry  │ │
                                         │  └─────────────┘ │
                                         └──────────────────┘
```

---

## Memory Layout

### Static Arrays (Stack Allocation)
```
MacroDict:
┌────────────────────────────────────┐
│ entries[0]                         │ ──> MacroEntry (name[256], value[1024], is_defined)
│ entries[1]                         │ ──> MacroEntry
│ entries[2]                         │ ──> MacroEntry
│ ...                                │
│ entries[1023]                      │ ──> MacroEntry
│ count = n                          │
└────────────────────────────────────┘
```

### ParserState Memory Model
```
ParserState:
┌────────────────────────────────────┐
│ FILE* current_file                 │ ──> Points to input file
│ FILE* output_file                  │ ──> Points to output file
│ char current_filename[512]         │ ──> Static array
│ int current_line                   │
│ MacroDict* macro_dict              │ ──> Points to MacroDict
│ bool flags...                      │
│ char lookahead                     │
│ bool has_lookahead                 │
└────────────────────────────────────┘
```

---

## Data Flow in Processing

```
1. Input File
      │
      ▼
2. read_char() / peek_char()
      │
      ▼
3. ParserState
      │
      ├──> Check in_string / in_comment
      │
      ├──> read_word()
      │         │
      │         ▼
      │    is_directive()?
      │         │
      │         ├──> #define ──> process_define() ──> Add to MacroDict
      │         │
      │         ├──> #include ──> process_include()
      │         │
      │         └──> #ifdef / #endif ──> Conditional Processing
      │
      └──> substitute_macro()
                │
                ▼
           Search in MacroDict
                │
                ▼
           Replace if found
                │
                ▼
4. Output File
```

---

## Module Interactions

```
┌─────────────────┐
│  module_args    │ ──> Creates ArgFlags
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ module_parser   │ ──> Creates ParserState + MacroDict
└────────┬────────┘
         │
         ├──> module_comments_remove (uses ParserState)
         │
         ├──> module_define (uses ParserState, modifies MacroDict)
         │
         ├──> module_include (uses ParserState)
         │
         ├──> module_ifdef_endif (uses ParserState, reads MacroDict)
         │
         └──> module_macros (uses ParserState, reads MacroDict)
```

---

## Key Constants

```c
#define MAX_FILENAME 512        // Max file path length
#define MAX_MACRO_NAME 256      // Max macro name length
#define MAX_MACRO_VALUE 1024    // Max macro value length
#define MAX_MACROS 1024         // Max number of macros
#define MAX_LINE_LENGTH 4096    // Max line length
```

---

## Summary

El preprocessor utilitza 4 estructures de dades principals:

1. **ArgFlags**: Configuració d'arguments (entrada/sortida, flags)
2. **MacroEntry**: Definició individual d'una macro
3. **MacroDict**: Col·lecció de totes les macros definides
4. **ParserState**: Estat complet del parser amb context de fitxers, macros i parsing

La jerarquia és: **ArgFlags** → inicialitza **ParserState** → que conté **MacroDict** → que conté array de **MacroEntry**.
