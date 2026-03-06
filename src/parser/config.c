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
 * Contributor/s: Pol García, Jan Prats
 * -----------------------------------------------------------------------------
 */

#include "./main.h"
#include "config.h"

Status status;  // ← THIS allocates the memory (only once)

//Our general functions
const char* category_to_string(Category cat) {
    switch (cat) {
        case CAT_NUMBER:        return "CAT_NUMBER";
        case CAT_IDENTIFIER:    return "CAT_IDENTIFIER";
        case CAT_KEYWORD:       return "CAT_KEYWORD";
        case CAT_TYPE:          return "CAT_TYPE";
        case CAT_LITERAL:       return "CAT_LITERAL";
        case CAT_OPERATOR:      return "CAT_OPERATOR";
        case CAT_SPECIALCHAR:   return "CAT_SPECIALCHAR";
        case CAT_NONRECOGNIZED: return "CAT_NONRECOGNIZED";
        default:                return "CAT_UNKNOWN";
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
    return CAT_INDIFERENT;
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
    // elements is a fixed array inside the struct, nothing to free internally
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
 *   TERMINALS: (term1, col_of_term1) (term2, col_of_term2) ...
 *   NON_TERMINALS: (nonterm1, col_of_nonterm1) (nonterm2, col_of_nonterm2) ...
 *   START_SYMBOL: symbol
 *   PRODUCTIONS:
 *   rule_id: lhs[any] -> rhs[any]
 *   ...
 * Retorna CORRECT_RETURN si té èxit, ERROR_RETURN si falla.
 * ----------------------------------------------------------------------- */
int load_language(const char* filename, LanguageV2* lang) {

    if (!lang) return ERROR_RETURN;

    FILE* f = fopen(filename, "r");
    if (!f) return ERROR_RETURN;

    // -------------------------------------------------------------------------
    // Allocate SRA, DFA and Stack up front so we can fill them as we parse
    // -------------------------------------------------------------------------
    lang->sra = (AutomataSRA*)malloc(sizeof(AutomataSRA));
    if (!lang->sra) { fclose(f); return ERROR_RETURN; }
    memset(lang->sra, 0, sizeof(AutomataSRA));

    lang->sra->dfa = (AutomataDFA*)malloc(sizeof(AutomataDFA));
    if (!lang->sra->dfa) { free(lang->sra); fclose(f); return ERROR_RETURN; }
    memset(lang->sra->dfa, 0, sizeof(AutomataDFA));

    lang->sra->stack = (Stack*)malloc(sizeof(Stack));
    if (!lang->sra->stack) { free(lang->sra->dfa); free(lang->sra); fclose(f); return ERROR_RETURN; }
    memset(lang->sra->stack, 0, sizeof(Stack));

    // Init counters
    lang->num_terminals    = 0;
    lang->num_nonterminals = 0;
    lang->num_productions  = 0;
    memset(lang->start_symbol, 0, MAX_TOKEN_NAME);
    memset(&lang->sra->table, 0, sizeof(ParseTable));

    int production_idx  = 0;
    int in_action_table = 0;
    int in_goto_table   = 0;
    int in_productions  = 0;

    char line[MAX_LINE_LENGTH];

    // -------------------------------------------------------------------------
    // Helper lambda-style macro: find a terminal RuleItem by lexeme
    // -------------------------------------------------------------------------
    // (implemented as inline search below where needed)

    while (fgets(line, MAX_LINE_LENGTH, f)) {

        // Strip trailing newline / carriage return
        size_t len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = '\0';

        // Skip blank lines and comment lines
        if (len == 0 || line[0] == '#' || line[0] == ';') continue;

        // ---- Section headers ------------------------------------------------
        if (strncmp(line, "[ACTION_TABLE]", 14) == 0) {
            in_action_table = 1; in_goto_table = 0; in_productions = 0;
            continue;
        }
        if (strncmp(line, "[GOTO_TABLE]", 12) == 0) {
            in_action_table = 0; in_goto_table = 1; in_productions = 0;
            continue;
        }

        // ---- START_SYMBOL ---------------------------------------------------
        if (strncmp(line, "START_SYMBOL:", 13) == 0) {
            char* sym = line + 13;
            while (*sym == ' ' || *sym == '\t') sym++;
            strncpy(lang->start_symbol, sym, MAX_TOKEN_NAME - 1);
            lang->start_symbol[MAX_TOKEN_NAME - 1] = '\0';
            continue;
        }

        // ---- NON_TERMINALS: ("sym",col) ("sym",col) ... ---------------------
        if (strncmp(line, "NON_TERMINALS:", 14) == 0) {
            char* p = line + 14;
            while (*p) {
                // find opening quote
                char* q1 = strchr(p, '"');
                if (!q1) break;
                char* q2 = strchr(q1 + 1, '"');
                if (!q2) break;
                // extract symbol name
                int slen = (int)(q2 - q1 - 1);
                char sym[MAX_TOKEN_NAME];
                strncpy(sym, q1 + 1, slen);
                sym[slen] = '\0';
                // find comma and column number after closing quote
                char* comma = strchr(q2 + 1, ',');
                if (!comma) break;
                int col = atoi(comma + 1);

                // Build RuleItem
                int idx = lang->num_nonterminals;
                lang->nonterminals[idx][0].type            = NON_TERMINAL_SYMBOL;
                lang->nonterminals[idx][0].token.cat       = CAT_INDIFERENT;
                lang->nonterminals[idx][0].token.line      = 0;
                lang->nonterminals[idx][0].column          = col;
                strncpy(lang->nonterminals[idx][0].token.lexeme, sym, MAX_TOKEN_NAME - 1);
                lang->nonterminals[idx][0].token.lexeme[MAX_TOKEN_NAME - 1] = '\0';
                lang->num_nonterminals++;

                // advance past closing ')' of this tuple
                char* rp = strchr(q2 + 1, ')');
                p = rp ? rp + 1 : q2 + 1;
            }
            continue;
        }

        // ---- TERMINALS: ("sym",col) or (CAT_XXX,col) -----------------------
        if (strncmp(line, "TERMINALS:", 10) == 0) {
            char* p = line + 10;
            while (*p) {
                char* lp = strchr(p, '(');
                if (!lp) break;
                lp++; // move past '('

                char* closing_quote = strchr(lp, '"');
                if (!closing_quote) break;
                closing_quote = strchr(closing_quote + 1, '"'); // find the second quote (closing one)
                if (!closing_quote) break;
                char* rp = strchr(closing_quote, ')');  // <-- REPLACED LINE
                if (!rp) break;

                // extract everything inside the parentheses
                int tlen = (int)(rp - lp);
                char tuple[MAX_TOKEN_NAME];
                strncpy(tuple, lp, tlen);
                tuple[tlen] = '\0';

                // find the last comma to split symbol from column
                char* last_comma = strrchr(tuple, ',');
                if (!last_comma) { p = rp + 1; continue; }

                int col = atoi(last_comma + 1);
                *last_comma = '\0'; // tuple now holds only the symbol part

                // strip surrounding quotes if present
                char sym[MAX_TOKEN_NAME];
                char* sym_src = tuple;
                while (*sym_src == ' ' || *sym_src == '\t') sym_src++;
                if (*sym_src == '"') {
                    sym_src++;
                    char* eq = strrchr(sym_src, '"');
                    if (eq) *eq = '\0';
                }
                strncpy(sym, sym_src, MAX_TOKEN_NAME - 1);
                sym[MAX_TOKEN_NAME - 1] = '\0';
                // trim trailing spaces
                int sl = (int)strlen(sym);
                while (sl > 0 && (sym[sl-1] == ' ' || sym[sl-1] == '\t')) sym[--sl] = '\0';

                int idx = lang->num_terminals;
                lang->terminals[idx].type       = TERMINAL_SYMBOL;
                lang->terminals[idx].token.line = 0;
                lang->terminals[idx].column     = col;

                // Check if it looks like a CAT_XXX category token
                Category detected = string_to_category(sym);
                if (detected != CAT_INDIFERENT) {
                    // e.g. (CAT_NUMBER, 5)  -->  (TERMINAL, ("CAT_NUMBER", CAT_NUMBER, 0), 5)
                    lang->terminals[idx].token.cat = detected;
                    strncpy(lang->terminals[idx].token.lexeme, sym, MAX_TOKEN_NAME - 1);
                    lang->terminals[idx].token.lexeme[MAX_TOKEN_NAME - 1] = '\0';
                } else {
                    // Regular terminal symbol, CAT_INDIFERENT, match by lexeme
                    lang->terminals[idx].token.cat = CAT_INDIFERENT;
                    // Replace bare '$' with EOF_TOKEN_LEXEME string
                    if (strcmp(sym, "$") == 0) {
                        strncpy(lang->terminals[idx].token.lexeme, "$end", MAX_TOKEN_NAME - 1);
                    } else {
                        strncpy(lang->terminals[idx].token.lexeme, sym, MAX_TOKEN_NAME - 1);
                    }
                    lang->terminals[idx].token.lexeme[MAX_TOKEN_NAME - 1] = '\0';
                }

                lang->num_terminals++;
                p = rp + 1;
            }
            continue;
        }

        // ---- PRODUCTIONS: section header ------------------------------------
        if (strncmp(line, "[PRODUCTIONS]", 13) == 0) {
            in_action_table = 0; in_goto_table = 0; in_productions = 1;
            continue;
        }

        // ---- ACTION TABLE rows: state | cell cell cell ... ------------------
        if (in_action_table && isdigit((unsigned char)line[0])) {
            char row[MAX_LINE_LENGTH];
            strncpy(row, line, MAX_LINE_LENGTH - 1);
            row[MAX_LINE_LENGTH - 1] = '\0';

            char* tok = strtok(row, " \t|");
            int state = atoi(tok);
            if (state >= MAX_STATES) continue;

            int col = 0;
            while ((tok = strtok(NULL, " \t|")) && col < MAX_ALPHABET_SIZE) {
                ParseAction act;
                act.type  = ACTION_ERROR;
                act.value = 0;

                if (tok[0] == 's' || tok[0] == 'S') {
                    act.type  = ACTION_SHIFT;
                    act.value = atoi(tok + 1);
                } else if (tok[0] == 'r' || tok[0] == 'R') {
                    act.type  = ACTION_REDUCE;
                    act.value = atoi(tok + 1);
                } else if (strncmp(tok, "acc", 3) == 0) {
                    act.type  = ACTION_ACCEPT;
                } else { // err
                    act.type  = ACTION_ERROR;
                }

                lang->sra->table.cells[state][col] = act;
                col++;
            }
            if (state + 1 > lang->sra->table.num_states)
                lang->sra->table.num_states = state + 1;
            if (col > lang->sra->table.num_symbols)
                lang->sra->table.num_symbols = col;
            continue;
        }

        // ---- GOTO TABLE rows: state | cell cell cell ... --------------------
        if (in_goto_table && isdigit((unsigned char)line[0])) {
            char row[MAX_LINE_LENGTH];
            strncpy(row, line, MAX_LINE_LENGTH - 1);
            row[MAX_LINE_LENGTH - 1] = '\0';

            char* tok = strtok(row, " \t|");
            int state = atoi(tok);
            if (state >= MAX_STATES) continue;

            // GOTO columns correspond to non-terminals in order
            // We store them in dfa->matrix so that:
            //   dfa->matrix.states_rows[state].new_state[nt_col] = next_state
            // where nt_col is the column field of nonterminals[i][0]
            int nt_idx = 0;
            while ((tok = strtok(NULL, " \t|")) && nt_idx < lang->num_nonterminals) {
                if (strncmp(tok, "err", 3) != 0) {
                    int next_state = atoi(tok);
                    int nt_col = lang->nonterminals[nt_idx][0].column;
                    lang->sra->dfa->matrix.states_rows[state].new_state[nt_col] = next_state;
                }
                nt_idx++;
            }
            continue;
        }

        // ---- PRODUCTION RULES: id: lhs -> rhs ... ---------------------------
        if (in_productions && production_idx < MAX_PRODUCTIONS && len > 0) {
            char rule_copy[MAX_LINE_LENGTH];
            strncpy(rule_copy, line, MAX_LINE_LENGTH - 1);
            rule_copy[MAX_LINE_LENGTH - 1] = '\0';

            // rule_id  (everything before '|')
            char* pipe = strchr(rule_copy, '|');
            if (!pipe) continue;
            *pipe = '\0';
            lang->productions[production_idx].rule_id = atoi(rule_copy);

            // everything after '|'
            char* rest = pipe + 1;
            while (*rest == ' ' || *rest == '\t') rest++;

            // split on "->" to get lhs_str and rhs_str
            char* arrow = strstr(rest, "->");
            if (!arrow) continue;
            *arrow = '\0';
            char* rhs_str = arrow + 2;

            // --- LHS (single non-terminal) -----------------------------------
            char lhs_lex[MAX_TOKEN_NAME];
            strncpy(lhs_lex, rest, MAX_TOKEN_NAME - 1);
            lhs_lex[MAX_TOKEN_NAME - 1] = '\0';
            // trim
            int ll = (int)strlen(lhs_lex);
            while (ll > 0 && (lhs_lex[ll-1] == ' ' || lhs_lex[ll-1] == '\t')) lhs_lex[--ll] = '\0';
            char* lhs_start = lhs_lex;
            while (*lhs_start == ' ' || *lhs_start == '\t') lhs_start++;

            // Search vocab for this lexeme
            int found = 0;
            for (int i = 0; i < lang->num_nonterminals && !found; i++) {
                if (strcmp(lang->nonterminals[i][0].token.lexeme, lhs_start) == 0) {
                    lang->productions[production_idx].lhs[0] = lang->nonterminals[i][0];
                    found = 1;
                }
            }
            for (int i = 0; i < lang->num_terminals && !found; i++) {
                if (strcmp(lang->terminals[i].token.lexeme, lhs_start) == 0) {
                    lang->productions[production_idx].lhs[0] = lang->terminals[i];
                    found = 1;
                }
            }
            if (!found) {
                // fallback: create a bare NT item
                lang->productions[production_idx].lhs[0].type            = NON_TERMINAL_SYMBOL;
                lang->productions[production_idx].lhs[0].token.cat       = CAT_INDIFERENT;
                lang->productions[production_idx].lhs[0].token.line      = 0;
                lang->productions[production_idx].lhs[0].column          = -1;
                strncpy(lang->productions[production_idx].lhs[0].token.lexeme, lhs_start, MAX_TOKEN_NAME - 1);
                lang->productions[production_idx].lhs[0].token.lexeme[MAX_TOKEN_NAME - 1] = '\0';
            }
            lang->productions[production_idx].lhs_length = 1;

            // --- RHS (zero or more symbols) ----------------------------------
            int rhs_idx = 0;
            char* sym_tok = strtok(rhs_str, " \t");
            while (sym_tok && rhs_idx < MAX_RHS_LENGTH) {
                int sym_found = 0;
                // search non-terminals first
                for (int i = 0; i < lang->num_nonterminals && !sym_found; i++) {
                    if (strcmp(lang->nonterminals[i][0].token.lexeme, sym_tok) == 0) {
                        lang->productions[production_idx].rhs[rhs_idx] = lang->nonterminals[i][0];
                        sym_found = 1;
                    }
                }
                // then terminals (match by lexeme; CAT_XXX tokens are also matched by lexeme)
                for (int i = 0; i < lang->num_terminals && !sym_found; i++) {
                    if (strcmp(lang->terminals[i].token.lexeme, sym_tok) == 0) {
                        lang->productions[production_idx].rhs[rhs_idx] = lang->terminals[i];
                        sym_found = 1;
                    }
                }
                if (!sym_found) {
                    // fallback
                    lang->productions[production_idx].rhs[rhs_idx].type            = TERMINAL_SYMBOL;
                    lang->productions[production_idx].rhs[rhs_idx].token.cat       = CAT_INDIFERENT;
                    lang->productions[production_idx].rhs[rhs_idx].token.line      = 0;
                    lang->productions[production_idx].rhs[rhs_idx].column          = -1;
                    strncpy(lang->productions[production_idx].rhs[rhs_idx].token.lexeme, sym_tok, MAX_TOKEN_NAME - 1);
                    lang->productions[production_idx].rhs[rhs_idx].token.lexeme[MAX_TOKEN_NAME - 1] = '\0';
                }
                rhs_idx++;
                sym_tok = strtok(NULL, " \t");
            }
            lang->productions[production_idx].rhs_length = rhs_idx;
            production_idx++;
        }
    }

    lang->num_productions = production_idx;
    fclose(f);
    return CORRECT_RETURN;
}