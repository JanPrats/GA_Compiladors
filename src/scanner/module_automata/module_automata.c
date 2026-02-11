#include "module_automata.h"
#include <string.h>

bool automata_is_accepting(const AutomataDFA *a, int st)
{
    for (int i = 0; i < MAX_STATES && a->accepting_states[i] != -1; i++)
    {
        if (a->accepting_states[i] == st)
            return true;
    }
    return false;
}

/**
 * Actualitza l'estat d'un autòmata amb el caràcter llegit
 * @param a Punter a l'autòmata DFA
 * @param c Caràcter a processar
 * @param lookahead_char Següent caràcter (per verificar lookahead)
 * @return Category del token si acceptat, 0 si rebutjat, -1 si continua processant
 */
static int update_automata(AutomataDFA *a, char c)
{
    // Buscar l'índex del caràcter c dins l'alfabet de l'autòmata
    int idx = -1;
    for (int i = 0; i < MAX_ALPHABET_SIZE && a->alphabet[i] != '\0'; i++)
    {
        if (a->alphabet[i] == c)
        {
            idx = i; // Índex del símbol a la matriu
            break;
        }
    }

    // Si el caràcter no està a l'alfabet, l'autòmata rebutja
    if (idx == -1)
        return 0;

    // Obtenir el nou estat segons la matriu de transició
    int new_state = a->matrix.states_rows[a->current_state].new_state[idx];

    // Si la transició és 0, no hi ha estat vàlid → rebutjat
    if (new_state == 0)
        return 0;

    // Actualitzar estat actual
    a->current_state = new_state;

    // Afegir el caràcter al buffer
    a->buffer[a->buffer_len++] = c;

    // Comprovar si el nou estat és d'acceptació
    for (int i = 0; i < MAX_STATES && a->accepting_states[i] != 0; i++)
    {
        if (a->accepting_states[i] == new_state)
        {
            return a->type; // Estat d'acceptació → retornar categoria
        }
    }

    // Continua processant
    return -1;
}

/**
 * Driver que processa un fitxer amb múltiples autòmata DFA
 * Escriu tokens reconeguts i no reconeguts en fitxers separats
 */
void automata_driver(FILE *input_file, FILE *recognized_file, FILE *unrecognized_file, AutomataDFA *automata_list, int num_automata)
{
    char c; // Caràcter llegit del fitxer

    while ((c = fgetc(input_file)) != EOF)
    {
        bool accepted = false; // Indica si algun autòmata ha acceptat aquest token

        // Iterar sobre tots els autòmata
        for (int i = 0; i < num_automata; i++)
        {
            if (!automata_list[i].dont_look_anymore)
            { // Només processar actius
                int value = update_automata(&automata_list[i], c);

                if (value > 0)
                {
                    // L'autòmata ha arribat a un estat d'acceptació
                    // Escriure el token reconegut al fitxer corresponent
                    fprintf(recognized_file, "<%.*s, %d>\n",
                            automata_list[i].buffer_len,
                            automata_list[i].buffer,
                            value);

                    accepted = true; // Marcar que s'ha acceptat un token
                    break;           // Reiniciarem tots els autòmata després
                }
                else if (value == 0)
                {
                    // L'autòmata ha rebutjat, marcar com a "no mirar més"
                    automata_list[i].dont_look_anymore = true;
                    automata_list[i].buffer_len = 0;
                }
                // value == -1 → continua processant, no fem res
            }
        }

        if (accepted)
        {
            // Reinicialitzar tots els autòmata per començar un nou token
            for (int i = 0; i < num_automata; i++)
            {
                automata_list[i].current_state = automata_list[i].start_state;
                automata_list[i].buffer_len = 0;
                automata_list[i].dont_look_anymore = false;
            }
        }
        else
        {
            // Comprovar si tots els autòmata han rebutjat el caràcter
            bool all_done = true;
            for (int i = 0; i < num_automata; i++)
            {
                if (!automata_list[i].dont_look_anymore)
                {
                    all_done = false;
                    break;
                }
            }

            if (all_done)
            {
                // Escriure el caràcter com a no reconegut en el fitxer corresponent
                fprintf(unrecognized_file, "<%c, %d>\n", c, CAT_NONRECOGNIZED);

                // Reinicialitzar tots els autòmata per començar de nou
                for (int i = 0; i < num_automata; i++)
                {
                    automata_list[i].current_state = automata_list[i].start_state;
                    automata_list[i].buffer_len = 0;
                    automata_list[i].dont_look_anymore = false;
                }
            }
        }
    }
}