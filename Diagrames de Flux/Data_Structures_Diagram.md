# Data Structures Diagram
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

Emmagatzema la configuració dels arguments de línia de comandes.

```c
typedef struct ArgFlags {
    bool remove_comments;           // -c flag
    bool process_directives;        // -d flag
    bool show_help;                 // -help flag
    char ifile[MAX_FILENAME];       // input filename (512)
    char ofile[MAX_FILENAME];       // output filename (512)
} ArgFlags;
```
- `remove_comments`: Flag per eliminar comentaris
- `process_directives`: Flag per processar directives
- `show_help`: Flag per mostrar ajuda
- `ifile`: Nom del fitxer d'entrada
- `ofile`: Nom del fitxer de sortida

---

## 2. **MacroEntry** - Single Macro Definition

Representa una única definició de macro (#define).

```c
typedef struct MacroEntry {
    char name[MAX_MACRO_NAME];      // 256 chars
    char value[MAX_MACRO_VALUE];    // 1024 chars
    bool is_defined;
} MacroEntry;
```
- `name`: Nom de la macro
- `value`: Valor de substitució de la macro
- `is_defined`: Indica si la macro està definida

---

## 3. **MacroDict** - Macro Dictionary
Diccionari que conté totes les macros definides.
```c
typedef struct MacroDict {
    MacroEntry entries[MAX_MACROS]; // Array of 1024 entries
    int count;
} MacroDict;
```
- `entries`: Array estàtic de MacroEntry (màxim 1024 macros)
- `count`: Comptador del nombre de macros actuals

---

## 4. **ParserState** - Main Parser State

Estat principal del parser, conté tota la informació necessària per al processament.
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
  - `current_file`: Handle del fitxer actual d'entrada
  - `output_file`: Handle del fitxer de sortida
  - `current_filename`: Nom del fitxer actual
  - `current_line`: Número de línia actual
  - `macro_dict`: Punter al diccionari de macros
  - `remove_comments`: Si s'han d'eliminar comentaris
  - `process_directives`: Si s'han de processar directives
  - `in_string`: Indica si estem dins d'una cadena de text
  - `in_comment`: Indica si estem dins d'un comentari
  - `lookahead`: Caràcter següent (per lookahead parsing)
  - `has_lookahead`: Indica si hi ha un caràcter al buffer lookahead

---

## Relationships Between Structures
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

