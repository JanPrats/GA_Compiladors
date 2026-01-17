#include <stdio.h>
#include <stdbool.h>
#include "./module_parser.h"

/*
==========================================================
 INTERFÍCIE DEL MÒDUL DE COMENTARIS
==========================================================
El parser NO sap com funcionen els comentaris.
Només sap que existeix aquesta funció.
*/
bool comment_handle(char first, bool flag_c);

/*
==========================================================
 VARIABLES GLOBALS DEL PARSER
==========================================================
*/
static FILE *g_input_file = NULL;
static FILE *g_output_file = NULL;

/*
==========================================================
 parser_init
==========================================================
Inicialitza el parser amb els fitxers d'entrada i sortida
*/
void parser_init(FILE *input, FILE *output)
{
    g_input_file = input;
    g_output_file = output;
}

/*
==========================================================
 parser_next_char
==========================================================
Retorna el següent caràcter del fitxer d’entrada
i AVANÇA el cursor.
*/
char parser_next_char(void)
{
    int c;

    if (!g_input_file)
        return EOF;

    c = fgetc(g_input_file);
    return (char)c;
}

/*
==========================================================
 parser_copy_char
==========================================================
Escriu un caràcter al fitxer de sortida
*/
void parser_copy_char(char c)
{
    if (g_output_file)
        fputc(c, g_output_file);
}

/*
==========================================================
 parser_run
==========================================================
Bucle principal del parser

PARAMETRES:
- flag_c : indica si s'han de copiar els comentaris (-c)
*/
void parser_run(bool flag_c)
{
    char c;

    while ((c = parser_next_char()) != EOF)
    {
        // Si trobem '/', pot ser inici de comentari
        if (c == '/')
        {
            /*
            Si comment_handle retorna true:
            - el mòdul de comentaris ja ha consumit TOT el comentari
            - el parser NO ha de fer res més
            */
            if (comment_handle(c, flag_c))
                continue;

            /*
            No era un comentari
            → el '/' forma part del codi normal
            */
            parser_copy_char(c);
        }
        else
        {
            // Caràcter normal
            parser_copy_char(c);
        }
    }
}
