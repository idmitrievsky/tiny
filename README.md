# tiny
C++ implementation of slightly modified TINY language

    program := decls main

    decls := π
    decls := decl decls
    decl := `let` var-list
    var-list := var var-list-tail
    var-list-tail := π
    var-list-tail := `,` var var-list-tail
    var := ident var-opt
    var-opt := π
    var-opt := `=` num

    main := `begin` block `end`
    block := stmt block-tail
    block-tail := π
    block-tail := stmt block-tail

    stmt := if
    stmt := while
    stmt := assignment
    stmt := print

    assignment := ident `=` bool-exp

    if := `if` bool-exp block else-block `end`
    else-block := π
    else-block := `else` block

    while := `while` bool-exp block `end`

    print := `print` bool-exp print-tail
    print-tail := π
    print-tail := `,` bool-exp print-tail

    exp := term exp-tail
    exp-tail := π
    exp-tail := addop term exp-tail
    term := factor term-tail
    term-tail := π
    term-tail := mulop factor term-tail
    factor := ident
    factor := num
    factor := `(` exp `)`

    bool-exp ::= bool-term bool-exp-tail
    bool-exp-tail := π
    bool-exp-tail := orop bool-term bool-exp-tail
    bool-term := not-factor bool-term-tail
    bool-term-tail := π
    bool-term-tail := andop not-factor bool-term-tail
    not-factor := not-factor-opt relation
    not-factor-opt := π
    not-factor-opt := `!`
    relation := exp relation-tail
    relation-tail := π
    relation-tail := relop exp relation-tail

    addop := `+`
    addop := `-`

    mulop := `*`
    mulop := `/`

    orop := `|`
    andop := `&`

    relop := `<`
    relop := `<=`
    relop := `>`
    relop := `>=`
    relop := `=`
    relop := `#`

