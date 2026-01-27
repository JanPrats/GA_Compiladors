# Diagrama Input/Output dels Mòduls del Preprocessador

## Flux General 

```
+---------------------+
|  User / Command Line |
+----------+----------+
           |
           v (argc, argv)
+---------------------------------------------------------------+
|                        MAIN PROGRAM                           |
|                        (src/main.c)                           |
|  1. Init Errors                                               |
|  2. Process Args  <----->  [ Module Args ]                    |
|  3. Init Parser                                               |
|  4. Run Loop      ------>  [ Module Parser ]                  |
+---------------------------------------------------------------+
                                  |
                                  v (ParserState)
+---------------------------------------------------------------+
|                      MODULE PARSER                            |
|                  (src/module_parser.c)                        |
|                                                               |
|   +-------------------+      +----------------------------+   |
|   |   Input Stream    |----->|   Token Detection Logic    |   |
|   +-------------------+      +-------------+--------------+   |
|                                            |                  |
|          +---------punter enviat apuntant dsp de:--+          |
|          |                 |               |       |          |
|   (// or /*)        (#define)        (#include)   (#ifdef)    |
| +----------------+ +-------------+ +----------+ +-----------+ |
| | Module Comments| |Module Define| |Mod Include| |Mod Ifdef | |
| +----------------+ +-------------+ +----------+ +-----------+ |
|          |                 |              |           |       |
|          +-----------------+--------------+-----------+       |
|                            |                                  |
|                            v                                  |
|                  +-------------------+                        |
|                  |  copy output file |                        |
|                  |  if need it       |                        |
|                  +-------------------+                        |
+---------------------------------------------------------------+
           |
           v
    +-------------+
    | Output File |
    +-------------+
```
## Flux Arguments 

```
INPUT: .exe -flag file.c
         ↓
    [MODULE ARGS]
         ↓
OUTPUT: ArgFlags + Input/Output filenames
```

---

## Detall per Mòdul

### 1. **MODULE ARGS** (module_arguments)
**Entrada:**
- `argc, argv` (arguments de la línia de comandaments)
- Exemple: `.exe -flag file.c `

**Sortida:**
- `ArgFlags struct` amb:
  - `ifile`: nom del fitxer d'entrada
  - `ofile`: nom del fitxer de sortida
  - `remove_comments`: flag -c (bool)
  - `process_directives`: flag -d (bool)
  - `show_help`: flag -help (bool)

---

### 2. **PARSER** (module_parser)
**Entrada:**  per inicilaitzar
- `ifile` (fitxer d'entrada)
- `ofile` (fitxer de sortida)
- `ArgFlags` (flags de configuració)

**Sortida:**
- Cap de directa. Escriu el fitxer processat a `ofile`
- `ParserState` (estat intern del parser)

**Responsabilitats:**
- Llegeix el fitxer caràcter per caràcter
- Detecta directives preprocessador
- Delega a mòduls especificats según la directiva trobada

---

### 3. **MODULE COMMENTS_REMOVE** 
**Entrada:**
- Caràcters llegits pel parser, punter després d'haver detctat `//` o `/*`

**Sortida:**
- Escriu al fitxer de sortida si flag per borrar

---

### 4. **MODULE INCLUDE** 
**Entrada:**
- Després de directiva `#include`
- Recursiu: pot processar directives dins del fitxer incluït

**Sortida:**
- No retorna res
- Escriu al fitxer de sortida:
  - El contingut del fitxer incluït (processat recursivament)
  - Sense la línea `#include` original

---

### 5. **MODULE DEFINE**
**Entrada:**
- Punter de després de directiva `#define`
- Llegeix el nom del macro i el seu valor

**Sortida:**
- Emmagatzema el macro a `MacroDict`
- No escriu al fitxer de sortida (la directiva `#define` es descarta)
- Usa el macro per a substitucions posteriores, retorna value 

---

### 6. **MODULE IFDEF/ENDIF** 
**Entrada:**
- Punter de després de directiva
- Comprova a `MacroDict` si el macro està definit
- Processa recursivament fins trobar `#else` o `#endif`

**Sortida:**
- No retorna res
- Escriu al fitxer de sortida:
  - Si la condició és certa: escriu el bloc `#ifdef`
  - Si la condició és falsa: descarta el bloc (o escriu el `#else`)
- Les directives `#ifdef`, `#else`, `#endif` NO es copien

---

### 7. MODULE ERRORS (module_errors)
**Entrada:**

- ErrorLevel (tipus d'error: WARNING o ERROR)
- filename (nom del fitxer on s'ha detectat l'error)
- line (número de línia on s'ha produït)
- message (missatge descriptiu de l'error/avís)

**Sortida:**

Cap retorn directe. Escriu a stderr:

- Missatges formatats amb "WARNING:" o "ERROR:" segons el tipus
- Format: ERROR/WARNING: filename:line: message
- Resum final al finalitzar el preprocessament

## Flux Complet d'Exemple

```
INPUT: command line

                          ┌──────────────────────┐
                          │   MODULE ARGS        │
                          │ processa: -c -d      │
                          │ input: input.c       │
                          │ output: output_pp.c  │
                          └──────────┬───────────┘
                                     │
                                     ▼
                          ┌──────────────────────┐
                          │   PARSER INIT        │
                          │ obri fitxers         │
                          │ crea ParserState     │
                          └──────────┬───────────┘
                                     │
                                     ▼
                          ┌──────────────────────────────────┐
                          │   PARSE UNTIL (main loop)        │
                          │ llegeix char per char            │
                          │                                  │
                          │  Detecta:                        │
                          │  - '#include' → INCLUDE          │
                          │  - '#define' → DEFINE            │
                          │  - '#ifdef' → IFDEF/ENDIF        │
                          │  - '//' o '/*' → COMMENTS        │
                          │  - substitució macro             │
                          │  - altres → escriu output        │
                          │                                  │
                          │ Crides recursives als mòduls    │
                          └──────────┬───────────────────────┘
                                                │
                               ┌────┬────┬──────┴──────┬────┐
                               ▼    ▼    ▼             ▼    ▼
            ┌──────────────────┐  ┌──────────────────┐        ┌──────────────────┐
            │COMMENTS_REMOVE   │  │  INCLUDE         │        │   ERRORS         │
            │Si: preserva      │  │ Llegeix: "file.h"│        │ Detecta errors   │
            │Si no :descarta   │  │  Processa        │        | Acumula comptador|
            │No retorna res    │  │  recursivament   │        │ Informa stderr   │
            │Escriu output     │  │  No retorna res  │        │ report_error()   │
            │                  │  │  Escriu output   │        │                  │
            └──────────────────┘  └──────────────────┘        └──────────────────┘
            ┌──────────────────┐  ┌──────────────────┐
            │  DEFINE          │  │  IFDEF/ENDIF     │
            │Llegeix:MACRO valor│ │  Check MacroDict │
            │No retorna o macro│  │  Si cert: inclou │
            │Guarda MacroDict  │  │  Si fals: descarta
            │No escriu output  │  │  No retorna res  │
            │                  │  │  Escriu output   │
            └──────────────────┘  └──────────────────┘
                                     │
                                     ▼
                          ┌──────────────────────────┐
                          │   OUTPUT FILE GENERATED  │
                          │   (output_pp.c)          │
                          └──────────────────────────┘
```

---


## Notes Importants

1. **Cap mòdul retorna dades pròpiament dit** - Tot s'escriu als fitxers o es guarda a estructures globals
2. **El parser és el "controlador"** - Llija el fitxer i delega a cada mòdul
3. **Els mòduls NO retornen strings o arrays** - Retornen códis d'error (int)
4. **La MacroDict és compartida** - Tots els mòduls accedeixen a la mateixa `ParserState->macro_dict`
5. **Les directives no van al output** - `#include`, `#define`, `#ifdef` es processen però NO es copien al fitxer de sortida



