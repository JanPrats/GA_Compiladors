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
#include "count.h"

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

void add_token_to_list(char* lexeme, Category cat) {
    if (status.all_tokens.count >= MAX_TOKENS) {
        report_warning("Maximum token count reached, token discarded", status.line, SCANNER_STEP);
        return;
    }

    Token *t = &status.all_tokens.tokens[status.all_tokens.count];

    strncpy(t->lexeme, lexeme, MAX_TOKEN_NAME - 1);
    t->lexeme[MAX_TOKEN_NAME - 1] = '\0';  // ensure termination

    t->cat = cat;
    t->line = status.line;

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
        report_warning("Token too long, character discarded", status.line, SCANNER_STEP);
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