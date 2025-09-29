program teste_tres;

var
  a : integer;
  b := 20; { erro de atribuição aqui, mas o léxico vê := }
  c : char; $ erro de caractere
begin
  a := (b + 5)^2;
  imprimir('teste de string... );
end.