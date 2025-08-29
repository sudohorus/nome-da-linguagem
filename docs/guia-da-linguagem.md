# Guia da Linguagem

## Comentários
- Linha: `// comenta até o fim da linha`
- Bloco: `/* comenta várias linhas */`

## Tipos e variáveis
- Tipos: `int`, `float`, `str`, `auto`
- Declaração:
```txt
int a;
float x;
str nome;
auto z = 10;
```
- Declaração múltipla e inicialização opcional:
```txt
int a, b = 2, c;
```

## Entrada e saída
- Entrada: `read(ident);`
```txt
int idade;
read(idade);
```
- Saída: `print("texto");`
- Interpolação de variáveis com `{nome}` dentro da string:
```txt
print("idade = {idade}");
```

## Expressões
- Aritmético/concatenação: `+` (soma números; caso contrário, concatena strings)
- Comparação: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Lógico: `&&`, `||`, `!`


## Condicionais
```txt
if (condicao) {
  print("then");
} else if (outra) {
  print("else if");
} else {
  print("else");
}
```

- `condicao` aceita expressões numéricas/strings: valores numéricos diferentes de 0 são verdadeiros; strings não vazias também são verdadeiras.

## Medição de tempo
- Ativa a medição do tempo total de execução do arquivo:
```txt
timeexec();
```

## Exemplo completo
```txt
timeexec();
int a, b, c;
print("Digite três inteiros");
read(a); read(b); read(c);
if (a > b && a > c) {
  print("a é maior");
} else if (b > c && b > a) {
  print("b é maior");
} else {
  print("c é maior");
}
```
