**Whileif tokenization sequence (Mermaid)**

```mermaid
sequenceDiagram
    participant Main as main()
    participant Init as init_program()/init_status_scn()
    participant Automata as init_automata()/ALL_AUTOMATA
    participant Driver as automata_driver()
    participant File as status.ifile (fgetc)
    participant Buffer as buffer
    participant KW as keyword_automata
    participant ID as identifier_automata
    participant Out as write_token_to_file_and_list()

    Main->>Init: init_program(argc, argv)
    Init-->>Main: status ready
    Main->>Automata: init_automata(ALL_AUTOMATA)
    Main->>Driver: automata_driver()

    note over Driver,File: input characters: w h i l e i f (no separators)

    Driver->>File: fgetc() -> 'w' (c), lookahead 'h'
    Driver->>Driver: skip_nonchars('w','h')
    Driver->>Buffer: buffer_add('w')

    Driver->>KW: update_automata('w','h')
    Driver->>ID: update_automata('w','h')
    KW-->>Driver: CORRECT_RETURN (still matching 'while...')
    ID-->>Driver: CORRECT_RETURN (identifier continues)

    %% Repeat for letters 'h','i','l','e'
    Driver->>File: fgetc() -> 'h' (c), lookahead 'i'
    Driver->>Buffer: buffer_add('h')
    Driver->>KW: update_automata('h','i')
    Driver->>ID: update_automata('h','i')

    ...

    %% After reading 'e', lookahead is 'i' (letter)
    Driver->>KW: update_automata('e','i')
    KW-->>Driver: CORRECT_RETURN (keyword DID NOT accept because lookahead is letter)
    Driver->>ID: update_automata('e','i')
    ID-->>Driver: CORRECT_RETURN

    %% Continue reading 'i','f' into buffer
    Driver->>File: fgetc() -> 'i' ... 'f'
    Driver->>Buffer: buffer_add('i')
    Driver->>Buffer: buffer_add('f')

    %% If next char is separator (e.g., space or EOF): identifier will accept
    Driver->>File: fgetc() -> ' ' (or EOF) (lookahead==separator)
    Driver->>ID: update_automata( current, lookahead=separator )
    ID-->>Driver: ACCEPT_TOKEN

    Driver->>Out: write_token_to_file_and_list("whileif", CAT_IDENTIFIER)
    Out-->>Driver: write OK; add_token_to_list("whileif", CAT_IDENTIFIER)
    Driver->>Driver: restart_automatas()

    note right of Out: Result: single token <whileif, IDENTIFIER>
```

Short explanation: keyword DFAs require a terminating boundary (lookahead that forces NULL transition) to accept. When two keywords are concatenated without separator, `identifier_automata` greedily consumes the whole run of letters and becomes the accepted token.
