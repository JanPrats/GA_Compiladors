/*
 * -----------------------------------------------------------------------------
 * config.c
 *
 * Configuration and global state management for the Scanner module.
 * Manages token storage, buffer operations, and token categorization.
 *
 * Key Responsibilities:
 * - Global status variable initialization and management
 * - Token list management (adding, counting, storing tokens)
 * - Token buffer operations (creation, clearing, character addition)
 * - Category to string conversion for output formatting
 * - Token output to file and token list storage
 *
 * Token Categories Handled:
 * - Numbers, Identifiers, Keywords, Types
 * - Literals, Operators, Special Characters
 * - Non-recognized tokens
 *
 * Team: GA
 * Contributor/s: Pol García
 * -----------------------------------------------------------------------------
 */

#include "./main.h"
#include "config.h"

Status status;  // ← THIS allocates the memory (only once)

//Our general functions
const char* category_to_string(Category cat) {
    switch (cat) {
        case CAT_NUMBER:        return "NUMBER";
        case CAT_IDENTIFIER:    return "IDENTIFIER";
        case CAT_KEYWORD:       return "KEYWORD";
        case CAT_TYPE:          return "TYPE";
        case CAT_LITERAL:       return "LITERAL";
        case CAT_OPERATOR:      return "OPERATOR";
        case CAT_SPECIALCHAR:   return "SPECIALCHAR";
        case CAT_NONRECOGNIZED: return "NONRECOGNIZED";
        default:                return "UNKNOWN";
    }
}

Category string_to_category(const char* str) {
    if (strcmp(str, "CAT_NUMBER")       == 0) return CAT_NUMBER;
    if (strcmp(str, "CAT_IDENTIFIER")   == 0) return CAT_IDENTIFIER;
    if (strcmp(str, "CAT_KEYWORD")      == 0) return CAT_KEYWORD;
    if (strcmp(str, "CAT_TYPE")         == 0) return CAT_TYPE;
    if (strcmp(str, "CAT_LITERAL")      == 0) return CAT_LITERAL;
    if (strcmp(str, "CAT_OPERATOR")     == 0) return CAT_OPERATOR;
    if (strcmp(str, "CAT_SPECIALCHAR")  == 0) return CAT_SPECIALCHAR;
    return CAT_NONRECOGNIZED;
}

void add_token_to_list(char* lexeme, Category cat) {
    if (status.all_tokens.count >= MAX_TOKENS) {
        fprintf(stderr, "WARNING: Maximum token count reached, token discarded\n");
        return;
    }

    Token *t = &status.all_tokens.tokens[status.all_tokens.count];

    strncpy(t->lexeme, lexeme, MAX_TOKEN_NAME - 1);
    t->lexeme[MAX_TOKEN_NAME - 1] = '\0'; // ensure termination
 
    t->cat = cat;
    t->line = status.line; //will always be 0, we don't use it I think

    status.all_tokens.count++;
}

void buffer_clear(BufferAuto *buffer) {
    buffer->len = 0;
    buffer->lexeme[0] = '\0';
}

void buffer_add(BufferAuto *buffer, char c) {
    if (buffer->len < MAX_TOKEN_NAME - 1) {
        buffer->lexeme[buffer->len++] = c;
        buffer->lexeme[buffer->len] = '\0';
    } else {
        fprintf(stderr, "WARNING: Token too long, character discarded\n");
    }
}

void buffer_append(BufferAuto *dest, const BufferAuto *src) { //Does not delete the contents of the original buffer
    int i = 0;
    while (i < src->len && dest->len < MAX_TOKEN_NAME - 1) {
        dest->lexeme[dest->len++] = src->lexeme[i++];
    }
    dest->lexeme[dest->len] = '\0';
}

void buffer_move_append(BufferAuto *dest, BufferAuto *src) { //Does "delete" it, like it moves the content from one to another
    int i = 0;
    while (i < src->len && dest->len < MAX_TOKEN_NAME - 1) {
        dest->lexeme[dest->len++] = src->lexeme[i++];
    }
    dest->lexeme[dest->len] = '\0';

    buffer_clear(src);
}

void init_status_prs(void){
    status.oform = RELEASE;
    status.debug = 0;
    status.help = false;
    
    status.ifile_name[0] = '\0';
    status.ofile_name[0] = '\0';
    status.ifile = NULL;
    status.ofile = NULL;
    status.error_file = stdout;

    status.line = 1;
    status.first_token_in_line = true;
    status.line_has_tokens = false;
    status.all_tokens.count = 0;
    status.all_tokens.pos = 0;
}

/* Not used yet
ActionSkip skip_nonchars(char c, char lookahead){
    ActionSkip action = {0};  // Initialize all members to 0
    bool saw_newline = false;
    
    while (c == SPACE_CHAR || c == TAB_CHAR || c == END_OF_LINE || c == CARRIAGE_RETURN) {
        if (c == END_OF_LINE || c == CARRIAGE_RETURN) {
            status.line++;
            saw_newline = true;  // Track that we saw a newline
        }
        c = lookahead;
        if (lookahead != EOF) {
            lookahead = fgetc(status.ifile);
        }
    }
    
    if (c == EOF) {
        action.c = EOF;
        action.lookahead = EOF;
        action.to_do = EOF_RETURN;
    } else {
        action.c = c;
        action.lookahead = lookahead;
        if (saw_newline){
            action.to_do = action.to_do = EOL_RETURN;
        } else {
            action.to_do = action.to_do = CORRECT_RETURN;
        }
    }
    return action;
}

*/

// Parser


//Stack Functions
void initialize_stack(Stack *stack, AutomataDFA dfa) {
    stack->top = -1;
    RuleItem bottom = {0};  // Zero-initialize to avoid garbage
    bottom.type = TERMINAL_SYMBOL;
    strcpy(bottom.token.lexeme, EPSILON);
    push_stack(stack, bottom, dfa.start_state);
}

void push_stack(Stack *stack, RuleItem symbol, int state) {
    if (is_full_stack(stack)) {
        fprintf(stderr, "Stack overflow\n");
        return;
    }

    stack->top++;
    stack->elements[stack->top].symbol = symbol;   // struct copy
    stack->elements[stack->top].state = state;
}

StackElement pop_stack(Stack *stack) {
    if (is_empty_stack(stack)) {
        fprintf(stderr, "Stack underflow\n");
        StackElement empty = {0};
        return empty;
    }

    StackElement stel = stack->elements[stack->top];
    stack->top--;
    return stel;
}

StackElement peek_stack(const Stack *stack){ //No treu el ultim element
    if (is_empty_stack(stack)){
        fprintf(stderr, "Peek on empty stack\n");
        StackElement empty = {0};
        return empty;
    }

    return stack->elements[stack->top];
}

bool is_empty_stack(const Stack *stack) {
    return stack->top < 0;
}

bool is_full_stack(const Stack *stack) {
    return stack->top >= MAX_STACK_SIZE - 1;
}

void destroy_stack(Stack* stack) {
    if (stack == NULL) return;
    if (stack->elements != NULL) {   // or whatever your internal array is called
        free(stack->elements);
    }
    stack->top = -1;
}


//Functions that were here in the template

void split_path(const char *fullpath, char *path, char *filename, char *extension) {
    const char *last_slash = strrchr(fullpath, '/');
    const char *last_dot = strrchr(fullpath, '.');

    if (last_slash) {
        size_t path_len = last_slash - fullpath + 1;
        strncpy(path, fullpath, path_len);
        path[path_len] = '\0';
        strcpy(filename, last_slash + 1);
    } else {
        path[0] = '\0';
        strcpy(filename, fullpath);
    }

    if (last_dot && last_dot > last_slash) {
        strcpy(extension, last_dot + 1);
        filename[last_dot - last_slash - 1] = '\0';  // remove extension from filename
    } else {
        extension[0] = '\0';
    }
}

void generate_timestamped_log_filename(const char* base_name, char* output, size_t maxlen) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char path[MAXFILENAME], filename[MAXFILENAME], extension[MAXFILEEXT];

    split_path(base_name, path, filename, extension);

    if(extension == NULL || strlen(extension) == 0) {
        snprintf(extension, sizeof(extension), "log"); // Default extension if none provided
    }

    // Format: yyyy_mm_dd_hh_mm_base
    snprintf(output, maxlen, "%s%04d_%02d_%02d_%02d_%02d_%s.%s",
             PATHDIRLOGS, // path
             t->tm_year + 1900,
             t->tm_mon + 1,
             t->tm_mday,
             t->tm_hour,
             t->tm_min, 
             filename,
             extension);

    fprintf(status.ofile, "Generated log filename (with time stamp): %s\n", output);

}

// Function to set the output file for test results
// If the filename is "stdout", it will use stdout, otherwise it will open the specified filename
// It adds the timestamp to the filename if it is not "stdout"
FILE* set_output_test_file(const char* filename) {
    FILE *ofile = stdout;
    char timestamped_filename[MAXFILENAME];

    if (strcmp(filename, "stdout") != 0) {
        fprintf(ofile, "Machine remote time ");
        generate_timestamped_log_filename(filename, timestamped_filename, sizeof(timestamped_filename));

        // Set the time zone to Europe/Madrid: 
        // (i.e. fake it as GMT-3 if Madrid is in GMT+2 summer time)
        // When run in github actions the server is in another time zone
        // We want timestamp related to our time
        // Replace lines 66-68 with:
        #ifdef _WIN32
            _putenv("TZ=GMT-2");
            _tzset();
        #else
            setenv("TZ", "GMT-2", 1);
            tzset();
        #endif
        generate_timestamped_log_filename(filename, timestamped_filename, sizeof(timestamped_filename));
        filename = timestamped_filename;

        ofile = fopen(filename, "a"); // Tasks can be fast, so they are appended to the same file if it is the same minute
        if (ofile == NULL) {
            fprintf(stderr, "Error opening output file %s. Check if subdirectory exists, otherwise create it and run again\n", filename);
            ofile = stdout;
        }
    }
    if(ofile == stdout){
        printf("See log of execution in stdout (filename %s)\n", filename);
        fprintf(ofile, "See log of execution in stdout (filename %s)\n", filename);
    }
    else{
        printf("See log of execution in file %s\n", filename);
        fprintf(ofile, "See log of execution in file %s\n", filename);
    }
    fflush(ofile);
    return ofile;
}

/* -----------------------------------------------------------------------
 * stack_to_string
 * Reads the stack from bottom (index 0) to top and concatenates the
 * lexemes of each RuleItem token, separated by spaces.
 * Result is written into `output` (size `output_size`).
 * ----------------------------------------------------------------------- */
void stack_to_string(const Stack *stack, char *output, size_t output_size) {
    if (stack == NULL || output == NULL || output_size == 0) {
        if (output && output_size > 0) output[0] = '\0';
        return;
    }

    output[0] = '\0';
    size_t current_len = 0;

    // Read from bottom (0) to top
    for (int i = 0; i <= stack->top; i++) {
        const char *lexeme = stack->elements[i].symbol.token.lexeme;
        
        // Add space if not the first element
        if (i > 0 && current_len < output_size - 1) {
            output[current_len++] = ' ';
            output[current_len] = '\0';
        }
        
        // Append lexeme to output
        int lexeme_len = strlen(lexeme);
        if (current_len + lexeme_len < output_size - 1) {
            strncpy(&output[current_len], lexeme, output_size - current_len - 1);
            current_len += lexeme_len;
            output[current_len] = '\0';
        } else {
            // Buffer too small, truncate
            break;
        }
    }
}

/* -----------------------------------------------------------------------
 * action_to_string
 * Converts a ParseAction to its string representation:
 * - SHIFT n  -> "Sn"
 * - REDUCE n -> "Rn"
 * - ACCEPT   -> "Accept"
 * - REJECT   -> "Reject"
 * - ERROR    -> "Error"
 * Result is written into `output` (size `output_size`).
 * ----------------------------------------------------------------------- */
void action_to_string(ParseAction action, char *output, size_t output_size) {
    if (output == NULL || output_size == 0) return;

    switch (action.type) {
        case ACTION_SHIFT:
            snprintf(output, output_size, "S%d", action.value);
            break;
        case ACTION_REDUCE:
            snprintf(output, output_size, "R%d", action.value);
            break;
        case ACTION_ACCEPT:
            snprintf(output, output_size, "Accept");
            break;
        case ACTION_REJECT:
            snprintf(output, output_size, "Reject");
            break;
        case ACTION_ERROR:
            snprintf(output, output_size, "Error");
            break;
        case ACTION_GOTO:
            snprintf(output, output_size, "G%d", action.value);
            break;
        default:
            snprintf(output, output_size, "Unknown");
            break;
    }
}

/* -----------------------------------------------------------------------
 * load_language
 * Carrega la definició del lenguatge des d'un fitxer a una estructura LanguageV2.
 * Format del fitxer:
 *   TERMINALS: term1 term2 ...
 *   NON_TERMINALS: nonterm1 nonterm2 ...
 *   START_SYMBOL: symbol
 *   PRODUCTIONS:
 *   rule_id: lhs -> rhs
 *   ...
 * Retorna CORRECT_RETURN si té èxit, ERROR_RETURN si falla.
 * ----------------------------------------------------------------------- */
int load_language(const char* filename, LanguageV2* lang) {
    // =====================================================================
    // PART 1: VALIDACIÓ I INICIALITZACIÓ
    // =====================================================================
    
    // Validem que la estructura no sigui NULL
    if (!lang) return ERROR_RETURN;
    
    // Obrim el fitxer de configuració del lenguatge
    FILE* f = fopen(filename, "r");
    if (!f) return ERROR_RETURN;

    char line[MAX_LINE_LENGTH];
    // Inicialitzem els comptadors
    lang->num_terminals = 0;
    lang->num_nonterminals = 0;
    lang->num_productions = 0;
    memset(lang->start_symbol, 0, MAX_TOKEN_NAME);

    int production_idx = 0;
    int in_action_table = 0;
    int in_goto_table = 0;

    // Inicialitzem la taula de parseig (ParseTable)
    ParseTable parse_table;
    parse_table.num_states = 0;
    parse_table.num_symbols = 0;
    memset(&parse_table.cells, 0, sizeof(parse_table.cells));

    // =====================================================================
    // PART 2: LECTURA PRINCIPAL DEL FITXER (LÍNIA PER LÍNIA)
    // =====================================================================

    //Llegim el fitxer línia per línia
    while (fgets(line, MAX_LINE_LENGTH, f)) {
        //Eliminem la nova línia al final de la cadena
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

        //Saltem línies buides i comentaris
        if (strlen(line) == 0 || line[0] == '#' || line[0] == ';') continue;

        // =====================================================================
        // PART 3: DETECCIÓ DE SECCIONS
        // =====================================================================
        
        if (strncmp(line, "[ACTION_TABLE]", 14) == 0) {
            in_action_table = 1;
            in_goto_table = 0;
            continue;
        }
        else if (strncmp(line, "[GOTO_TABLE]", 12) == 0) {
            in_action_table = 0;
            in_goto_table = 1;
            continue;
        }

        // =====================================================================
        // PART 4: PARSEIG ACTION_TABLE
        // =====================================================================
        
        if (in_action_table && isdigit((unsigned char)line[0])) {
            int state = atoi(line);
            if (state >= MAX_STATES) continue;
            
            char row_copy[MAX_LINE_LENGTH];
            strncpy(row_copy, line, MAX_LINE_LENGTH - 1);
            row_copy[MAX_LINE_LENGTH - 1] = '\0';

            // Saltem el número del state
            char* token = strtok(row_copy, " \t|");
            int symbol_idx = 0;

            // Processem cada acció per a aquest state
            while ((token = strtok(NULL, " \t|")) && symbol_idx < MAX_ALPHABET_SIZE) {
                ParseAction action;
                action.type = ACTION_ERROR;
                action.value = 0;

                // Parsegem l'acció: s<num>, r<num>, acc, err
                if (token[0] == 's' || token[0] == 'S') {
                    action.type = ACTION_SHIFT;
                    action.value = atoi(token + 1);
                }
                else if (token[0] == 'r' || token[0] == 'R') {
                    action.type = ACTION_REDUCE;
                    action.value = atoi(token + 1);
                }
                else if (strncmp(token, "acc", 3) == 0) {
                    action.type = ACTION_ACCEPT;
                    action.value = 0;
                }
                else if (strncmp(token, "err", 3) == 0) {
                    action.type = ACTION_ERROR;
                    action.value = 0;
                }

                parse_table.cells[state][symbol_idx] = action;
                symbol_idx++;
            }
            parse_table.num_states = (state + 1 > parse_table.num_states) ? state + 1 : parse_table.num_states;
            parse_table.num_symbols = (symbol_idx > parse_table.num_symbols) ? symbol_idx : parse_table.num_symbols;
        }

        // =====================================================================
        // PART 5: PARSEIG GOTO_TABLE
        // =====================================================================
        
        if (in_goto_table && isdigit((unsigned char)line[0])) {
            int state = atoi(line);
            if (state >= MAX_STATES) continue;
            
            char row_copy[MAX_LINE_LENGTH];
            strncpy(row_copy, line, MAX_LINE_LENGTH - 1);
            row_copy[MAX_LINE_LENGTH - 1] = '\0';

            // Saltem el número del state
            char* token = strtok(row_copy, " \t|");
            int symbol_idx = 0;

            // Processem cada goto per a aquest state
            while ((token = strtok(NULL, " \t|")) && symbol_idx < MAX_ALPHABET_SIZE) {
                ParseAction goto_action;
                goto_action.type = ACTION_ERROR;
                goto_action.value = 0;

                // Els gotos són números de state o "err"
                if (strncmp(token, "err", 3) == 0) {
                    goto_action.type = ACTION_ERROR;
                    goto_action.value = 0;
                }
                else if (isdigit((unsigned char)token[0])) {
                    goto_action.type = ACTION_GOTO;
                    goto_action.value = atoi(token);
                }

                // Els gotos es guarden en la columna dels no-terminals
                parse_table.cells[state][lang->num_terminals + symbol_idx] = goto_action;
                symbol_idx++;
            }
        }

        // =====================================================================
        // PART 6: PARSEIG TERMINALS
        // =====================================================================
        
        if (strncmp(line, "TERMINALS:", 10) == 0) {
            //Posem el punter al començament dels terminals (després de "TERMINALS:")
            char* tokens_str = line + 10;
            char* token = strtok(tokens_str, " \t");
            //Afegim cada terminal a la llista
            while (token && lang->num_terminals < MAX_ALPHABET_SIZE) {
                lang->terminals[lang->num_terminals].type = TERMINAL_SYMBOL;
                strncpy(lang->terminals[lang->num_terminals].token.lexeme, token, MAX_TOKEN_NAME - 1);
                lang->terminals[lang->num_terminals].token.lexeme[MAX_TOKEN_NAME - 1] = '\0';
                lang->terminals[lang->num_terminals].token.cat = CAT_INDIFERENT;
                lang->num_terminals++;
                token = strtok(NULL, " \t");
            }
        }
        
        // =====================================================================
        // PART 7: PARSEIG NON_TERMINALS
        // =====================================================================
        
        else if (strncmp(line, "NON_TERMINALS:", 14) == 0) {
            //Posem el punter al començament dels no-terminals
            char* tokens_str = line + 14;
            char* token = strtok(tokens_str, " \t");
            //Afegim cada no-terminal a la llista
            while (token && lang->num_nonterminals < MAX_ALPHABET_SIZE) {
                lang->nonterminals[lang->num_nonterminals][0].type = NON_TERMINAL_SYMBOL;
                strncpy(lang->nonterminals[lang->num_nonterminals][0].token.lexeme, token, MAX_TOKEN_NAME - 1);
                lang->nonterminals[lang->num_nonterminals][0].token.lexeme[MAX_TOKEN_NAME - 1] = '\0';
                lang->nonterminals[lang->num_nonterminals][0].token.cat = CAT_INDIFERENT;
                lang->num_nonterminals++;
                token = strtok(NULL, " \t");
            }
        }
        
        // =====================================================================
        // PART 8: PARSEIG START_SYMBOL
        // =====================================================================
        
        else if (strncmp(line, "START_SYMBOL:", 13) == 0) {
            //Obtenim el símbol inicial (és un únic símbolo no-terminal)
            char* symbol = line + 13;
            //Saltem espais en blanc
            while (*symbol == ' ' || *symbol == '\t') symbol++;
            strncpy(lang->start_symbol, symbol, MAX_TOKEN_NAME - 1);
            lang->start_symbol[MAX_TOKEN_NAME - 1] = '\0';
        }
        
        // =====================================================================
        // PART 9: DETECCIÓ SECCIÓ PRODUCTIONS (MARCA FI DE TAULES)
        // =====================================================================
        
        else if (strncmp(line, "PRODUCTIONS:", 12) == 0) {
            in_action_table = 0;
            in_goto_table = 0;
            continue;  // Saltem la línia de capçalera, les produccions venen després
        }
        
        // =====================================================================
        // PART 10: PARSEIG REGLES DE PRODUCCIÓ
        // =====================================================================
        
        else if (!in_action_table && !in_goto_table && production_idx < MAX_PRODUCTIONS && strlen(line) > 0) {
            //Format esperado: rule_id: lhs -> rhs1 rhs2 ...
            char rule_copy[MAX_LINE_LENGTH];
            strncpy(rule_copy, line, MAX_LINE_LENGTH - 1);
            rule_copy[MAX_LINE_LENGTH - 1] = '\0';

            //Extraem l'ID de la regla (número abans del ":")
            char* rule_id_str = strtok(rule_copy, ":");
            if (!rule_id_str) continue;
            
            lang->productions[production_idx].rule_id = atoi(rule_id_str);

            //Extraem el que ve després del ":" fins al "->"
            char* rest = strtok(NULL, "->");
            if (!rest) continue;

            //Parsegem el LHS (Left-Hand Side) - típicament un únic no-terminal
            char* lhs_token = strtok(rest, " \t");
            if (lhs_token) {
                lang->productions[production_idx].lhs[0].type = NON_TERMINAL_SYMBOL;
                strncpy(lang->productions[production_idx].lhs[0].token.lexeme, lhs_token, MAX_TOKEN_NAME - 1);
                lang->productions[production_idx].lhs[0].token.lexeme[MAX_TOKEN_NAME - 1] = '\0';
                lang->productions[production_idx].lhs[0].token.cat = CAT_INDIFERENT;
                lang->productions[production_idx].lhs_length = 1;
            }

            //Parsegem el RHS (Right-Hand Side) - els símbols de la dreta de la producció
            rest = strtok(NULL, "");
            if (!rest) {
                //Si no hi ha RHS, és una producció epsilon
                lang->productions[production_idx].rhs_length = 0;
            } else {
                int rhs_idx = 0;
                //Extraem cada símbol del RHS
                char* rhs_token = strtok(rest, " \t");
                while (rhs_token && rhs_idx < MAX_RHS_LENGTH) {
                    //Determinem si és terminal o no-terminal basant-nos en si comença amb majúscula
                    RuleItemType item_type = (isupper((unsigned char)rhs_token[0])) ? NON_TERMINAL_SYMBOL : TERMINAL_SYMBOL;
                    
                    lang->productions[production_idx].rhs[rhs_idx].type = item_type;
                    strncpy(lang->productions[production_idx].rhs[rhs_idx].token.lexeme, rhs_token, MAX_TOKEN_NAME - 1);
                    lang->productions[production_idx].rhs[rhs_idx].token.lexeme[MAX_TOKEN_NAME - 1] = '\0';
                    lang->productions[production_idx].rhs[rhs_idx].token.cat = CAT_INDIFERENT;
                    rhs_idx++;
                    rhs_token = strtok(NULL, " \t");
                }
                lang->productions[production_idx].rhs_length = rhs_idx;
            }

            production_idx++;
        }
    }

    // =====================================================================
    // PART 11: FINALITZACIÓ I CREACIÓ DE L'ESTRUCTURA SRA
    // =====================================================================

    // Actualitzem el nombre total de produccions
    lang->num_productions = production_idx;
    
    // Creem l'estructura SRA amb la taula de parseig carregada
    lang->sra = (AutomataSRA*)malloc(sizeof(AutomataSRA));
    if (lang->sra) {
        lang->sra->table = parse_table;
        lang->sra->dfa = NULL;
        lang->sra->stack = NULL;
        lang->sra->tokens = 0;
    }

    fclose(f);
    return CORRECT_RETURN;
}