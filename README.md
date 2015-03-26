# tiny
C++ implementation of slightly modified TINY language

    program := `program` decls main
    
    decls := {decl}
    decl := `let` var-list
    var-list := var {`,` var}
    var := ident [`=` int]
    
    main := `begin` block `end`
    block := {stmt}
    stmt := if | while | assignment | print
    assignment := ident `=` bool-exp
    if := `if` bool-exp block [`else` block] `end`
    while := `while` bool-exp block `end`
    print := `print` bool-exp {`,` bool-exp}
    
    exp := first-term {addop term}
    first-term := first-factor rest
    term := factor rest
    rest := {mulop factor}
    first-factor := [addop] factor
    factor := ident | num | `(` exp `)`
    
    bool-exp ::= bool-term {orop bool-term}
    bool-term := not-factor {andop not-factor}
    not-factor := [`!`] relation
    relation := exp [relop exp]
    
    addop := `+` | `-`;
    mulop := `*` | `/`;
    orop := `|`;
    andop := `&`;
    relop := `<` | `<=` |  `>` | `>=` | `=` | `#`;
    
