# PParser
## Small and Simple Parser Generator for C

PParser is a parser generator inspired by [ocamlyacc](https://ocaml.org/manual/5.2/lexyacc.html).

### Example
```
// Rule that recognizes an identfier and returns its token
// Here, {TOK_IDENTIFIER} means that you recognize a token
// which kind is TOK_IDENTIFIER.

identifier: {TOK_IDENTIFIER} => {
  return %{0%};
}


// Rule for a return statement. Here, there must be a token
// which lexeme is 'return' followed by something that is
// recognized as an expr, which as indicated by the '%' is
// another rule. The order of definition of the rules does
// not matter.

return_stmt: 'return' %expr => {
  return new_return_stmt(%{0%});
}

// Rule for a leaf, this uses multiple rules candidates
// as indicated by the '|', reminescent of ocaml's
// pattern matching.
leaf:
  | {IDENTIFIER} => { return new_identifier(%{0%}); }
  | {STRLIT}     => { return new_strlit(%{0%}); }
  | {CHARLIT}    => { return new_charlit(%{0%}); }
  | {INTLIT}     => { return new_intlit(%{0%}); }
  | {FLOATLIT}   => { return new_floatlit(%{0%}); }


// In case you want more freedom and want to interop directly
// with the lexer (can be useful when dealing with lists of asts,
// etc... you can use a blank rule candidate and use the lexer as
// is. Do not forget to set the *worked flag especially if you
// have multiple rule candidates. You can have multiple blank
// rules candidates in a rule. The first one for which the worked
// flag is set to true in the order of definition will be the
// value returned by the rule.

more_freedom: => {
  token_t tok = next(l);
  if(tok.kind == KEY_VAR) {
    // do something...
  } else {
    // do something else...
  }
  // if everything worked, set the worked parameter as non-zero
  // otherwise make it a zero.
  *worked = 1;
  return ptr;
} 
```

For a working example, see [toylang](https://github.com/Paul-Passeron/toylang), a proof of concept that uses pparser.

### Installation

#### Using projman
```
git clone https://github.com/Paul-Passeron/pparser.git
cd pparser
sudo projman -f -i
```

#### Using make
```
git clone https://github.com/Paul-Passeron/pparser.git
cd pparser
make
sudo make install
```
### Usage
```
pparser <file> [generated .c folder] [generated .h folder] [generated_name]
```
