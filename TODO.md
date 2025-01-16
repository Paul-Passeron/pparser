# TODO

- add builtin parsers for lists for instance

example:

```
arglist: @list elem=(%expr) separator=(',')
program: @list elem=(%decl) separator=(';')
stmtlist: @list elem=(%stmt) separatpr=(';')
compound: '{' stmtlist '}' => {
  ast_t **list = %{1%};
  size_t count = 0;
  while(list[count]){
    count ++;
  }
  return new_compound(list, count);
}
```