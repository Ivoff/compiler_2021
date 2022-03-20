# Compilador 1 & 2
Frontend de compilador faltando análise semântica e geração de código intermediário.
A ideia a princípio era fazer um compilador top-down iterativo de uma linguagem LL1. 
Porém, não deu, então fiz a análise sintática iterativa usando uma tabela de parse e a análise semântica usando recursão.
Uma escolha que se tornou um tanto quanto agradável, já que eu tinha sempre um nó para passar para um função e não precisava montar os token e analisar as regras semantias simultaneamente.
O compilador permite você ver a tal da árvore e ,ainda por cima, anotada. Olha só! É bom demais para acreditar né?

## TODO:

- [ ] terminar comp2
   - [x] atualizar conjuntos First e Follow
      - [x] documentar o que foi alterado
   - [x] atualizar tabela de parse
   - [ ] atualizar regras semanticas
   - [ ] implementar conversor de quadrupla pra código objeto de máquina hipotética
- [ ] fazer umas mensagens de erro melhores
- [ ] remover coisas inúteis do código como o id dos tokens já que o parse é feito diretamente comparando o próprio lexema
   - [ ] ver se tem como otimizar o parsing ou a análise semantica implementando ids para os tokens apropriadamente
- [ ] arrumar documentação
   - [ ] automato não reconhece lexemas relativos a expressões condicionais como if, else, then, while e etc
   - [ ] uml desatualizado

---

# Build
```
mkdir build
make
make run
```
caso necessário por motivos de debug ou qualquer outra coisa
```
make && make run > output
```
# Gramática
```
<programa> -> program ident <corpo> .
<corpo> -> <dc> begin <comandos> end
<dc> -> <dc_v> <mais_dc>  | λ
<mais_dc> -> ; <dc> | λ
<dc_v> ->  <tipo_var> : <variaveis>
<tipo_var> -> real | integer
<variaveis> -> ident <mais_var>
<mais_var> -> , <variaveis> | λ
<comandos> -> <comando> <mais_comandos>
<mais_comandos> -> ; <comandos> | λ
<comando> -> read (ident) |
			 write (ident) |
			 ident := <expressao> |
			 if <condicao> then <comandos> <pfalsa> $							
<condicao> -> <expressao> <relacao> <expressao>
<relacao> -> = | <> | >= | <= | > | <
<expressao> -> <termo> <outros_termos>
<termo> -> <op_un> <fator> <mais_fatores>
<op_un> -> - | λ
<fator> -> ident | numero_int | numero_real | (<expressao>)
<outros_termos> -> <op_ad> <termo> <outros_termos> | λ
<op_ad> -> + | -
<mais_fatores> -> <op_mul> <fator> <mais_fatores> | λ
<op_mul> -> * | /
<pfalsa> -> else <comandos> | λ
```

# Diagrama de transição
![Diagrama de transição](misc/automata.png "Diagrama de transição para reconhecimento de tokens")

# Conjuntos First e Follow
```
First(<programa>) = {program}
First(<corpo>) = First(<dc>) U {begin} #dc can be null
                 First(<dc_v>) 
                 First(<tipo_var>)
                 {real, integer, begin}
First(<dc>) = First(<dc_v>) U {&}
              First(<tipo_var>)
              {real, integer, &}
First(<mais_dc>) = {;, &}
First(<dc_v>) = First(<tipo_var>)
                {real, integer}
First(<tipo_var>) = {real, integer}
First(<variaveis>) = {ident}
First(<mais_var>) = {,, &} #primeira vírgula é terminal
First(<comandos>) = First(<comando>)
                    {read, write, ident, if}
First(<mais_comandos>) = {;, &}
First(<comando>) = {read, write, ident, if}
First(<condicao>) = First(<expressao>)
                    First(<termo>)
                    First(<op_un>) U First(<fator>)
                    {-, ident, numero_int, numero_real, (}
First(<relacao>) = {=, <>, >=, <=, >, <}
First(<expressao>) = First(<termo>)
                     First(<op_un>) U First(<fator>)
                     {-, ident, numero_int, numero_real, (}
First(<termo>) = First(<op_un>) U First(<fator>)
                 {-, ident, numero_int, numero_real, (}
First(<op_un>) = {-, &}
First(<fator>) = {ident, numero_int, numero_real, (}
First(<outros_termos>) = First(op_ad)
                         {+, -, &}
First(<op_ad>) = {+, -}
First(<mais_fatores>) = First(<op_mul>)
                        {*, /, &}
First(<op_mul>) = {*, /}
First(<pfalsa>) = {else, &}

Follow(<programa>) = {EOF}
Follow(<corpo>) = {.}
Follow(<dc>) = {begin} U Follow(<mais_dc>)
             = {begin} U Follow(<dc>)
             = {begin}
Follow(<mais_dc>) = Follow(<dc>)
                  = {begin}
Follow(<dc_v>) = First(<mais_dc>) U Follow(<mais_dc>)
               = {;, begin}
Follow(<tipo_var>) = {:}
Follow(<variaveis>) = Follow(<dc_v>) U Follow(<mais_var>)
                    = {;, begin} U Follow(<variaveis>)
                    = {;, begin} U Follow(<dc_v>)
                    = {;, begin}
Follow(<mais_var>) = Follow(<variaveis>)
                   = {;, begin}
Follow(<comandos>) = Follow(<mais_comandos>) U First(<pfalsa>) U Follow(<pfalsa>) U {end}
                   = Follow(<comandos>) U {else} U {$} U {end}
                   = {else, $, end}
Follow(<mais_comandos>) = Follow(<comandos>)
                        = {else, $, end}
Follow(<comando>) = First(<mais_comandos>)
                  = {;} U Follow(<mais_comandos>)
                  = {;, else, $, end}
Follow(<condicao>) = {then}
Follow(<relacao>) = First(expressao)
                  = {-, ident, numero_int, numero_real, (}
Follow(<expressao>) = First(<relacao>) U Follow(<condicao>) U Follow(<comando>) U {)}
                    = {=, <>, >=, <=, >, <} U {then} U {;, else, $, end} U {)}
                    = {=, <>, >=, <=, >, <, then, ;, else, $, end, )}
Follow(<termo>) = First(<outros_termos>)
                = {+, -} U Follow(<outros_termos>)
                = {+, -, =, <>, >=, <=, >, <, then, ;, else, $, end, )}
Follow(<op_un>) = First(<fator>)
                = {ident, numero_int, numero_real, (}
Follow(<fator>) = First(<mais_fatores>)
                = {*, /}
Follow(<outros_termos>) = Follow(<expressao>)
                        = {=, <>, >=, <=, >, <, then, ;, else, $, end, )}
Follow(<op_ad>) = First(<termo>)
                = {-, ident, numero_int, numero_real, (}
Follow(<mais_fatores>) = Follow(<termo>)
                       = {+, -, =, <>, >=, <=, >, <, then, ;, else, $, end, )}
Follow(<op_mul>) = First(<fator>)
                 = {ident, numero_int, numero_real, (}
Follow(<pfalsa>) = {$}
```

# Tabela de parse
|NAO TERMINAL |.  |end                 |real                                |integer                             |read                                   |write                                  |if                                                   |(                                              |ident                                          |)                   |:= |;                              |:  |program                              |,                          |then                |$                   |=                   |<\>                  |>=                  |<=                  |\>                   |<                   |-                                                 |numero_int                                     |numero_real                                    |+                                                 |*                                                |/                                                |else                       |begin                               |EOF|
|-------------|---|--------------------|------------------------------------|------------------------------------|---------------------------------------|---------------------------------------|-----------------------------------------------------|-----------------------------------------------|-----------------------------------------------|--------------------|---|-------------------------------|---|-------------------------------------|---------------------------|--------------------|--------------------|--------------------|--------------------|--------------------|--------------------|--------------------|--------------------|--------------------------------------------------|-----------------------------------------------|-----------------------------------------------|--------------------------------------------------|-------------------------------------------------|-------------------------------------------------|---------------------------|------------------------------------|---|
|programa     |   |                    |                                    |                                    |                                       |                                       |                                                     |                                               |                                               |                    |   |                               |   |<programa\> -> program ident <corpo\> .|                           |                    |                    |                    |                    |                    |                    |                    |                    |                                                  |                                               |                                               |                                                  |                                                 |                                                 |                           |                                    |   |
|corpo        |   |                    |<corpo\> -> <dc\> begin <comandos\> end|<corpo\> -> <dc\> begin <comandos\> end|                                       |                                       |                                                     |                                               |                                               |                    |   |                               |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |                                                  |                                               |                                               |                                                  |                                                 |                                                 |                           |<corpo\> -> <dc\> begin <comandos\> end|   |
|dc           |   |                    |<dc\> -> <dc_v\> <mais_dc\>            |<dc\> -> <dc_v\> <mais_dc\>            |                                       |                                       |                                                     |                                               |                                               |                    |   |<dc\> -> λ                      |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |                                                  |                                               |                                               |                                                  |                                                 |                                                 |                           |<dc\> -> λ                           |   |
|mais_dc      |   |                    |                                    |                                    |                                       |                                       |                                                     |                                               |                                               |                    |   |<mais_dc\> -> ; <dc\>            |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |                                                  |                                               |                                               |                                                  |                                                 |                                                 |                           |<mais_dc\> -> λ                      |   |
|dc_v         |   |                    |<dc_v\> ->  <tipo_var\> : <variaveis\> |<dc_v\> ->  <tipo_var\> : <variaveis\> |                                       |                                       |                                                     |                                               |                                               |                    |   |                               |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |                                                  |                                               |                                               |                                                  |                                                 |                                                 |                           |                                    |   |
|tipo_var     |   |                    |<tipo_var\> -> real                  |<tipo_var\> -> integer               |                                       |                                       |                                                     |                                               |                                               |                    |   |                               |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |                                                  |                                               |                                               |                                                  |                                                 |                                                 |                           |                                    |   |
|variaveis    |   |                    |                                    |                                    |                                       |                                       |                                                     |                                               |<variaveis\> -> ident <mais_var\>                |                    |   |                               |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |                                                  |                                               |                                               |                                                  |                                                 |                                                 |                           |                                    |   |
|mais_var     |   |                    |                                    |                                    |                                       |                                       |                                                     |                                               |                                               |                    |   |<mais_var\> -> λ                |   |                                     |<mais_var\> -> , <variaveis\>|                    |                    |                    |                    |                    |                    |                    |                    |                                                  |                                               |                                               |                                                  |                                                 |                                                 |                           |<mais_var\> -> λ                     |   |
|comandos     |   |                    |                                    |                                    |<comandos\> -> <comando\> <mais_comandos\>|<comandos\> -> <comando\> <mais_comandos\>|<comandos\> -> <comando\> <mais_comandos\>              |                                               |<comandos\> -> <comando\> <mais_comandos\>        |                    |   |                               |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |                                                  |                                               |                                               |                                                  |                                                 |                                                 |                           |                                    |   |
|mais_comandos|   |<mais_comandos\> -> λ|                                    |                                    |                                       |                                       |                                                     |                                               |                                               |                    |   |<mais_comandos\> -> ; <comandos\>|   |                                     |                           |                    |<mais_comandos\> -> λ|                    |                    |                    |                    |                    |                    |                                                  |                                               |                                               |                                                  |                                                 |                                                 |<mais_comandos\> -> λ       |                                    |   |
|comando      |   |                    |                                    |                                    |<comando\> -> read (ident)              |<comando\> -> write (ident)             |<comando\> -> if <condicao\> then <comandos\> <pfalsa\> $|                                               |<comando\> -> ident := <expressao\>              |                    |   |                               |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |                                                  |                                               |                                               |                                                  |                                                 |                                                 |                           |                                    |   |
|condicao     |   |                    |                                    |                                    |                                       |                                       |                                                     |<condicao\> -> <expressao\> <relacao\> <expressao\>|<condicao\> -> <expressao\> <relacao\> <expressao\>|                    |   |                               |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |<condicao\> -> <expressao\> <relacao\> <expressao\>   |<condicao\> -> <expressao\> <relacao\> <expressao\>|<condicao\> -> <expressao\> <relacao\> <expressao\>|                                                  |                                                 |                                                 |                           |                                    |   |
|relacao      |   |                    |                                    |                                    |                                       |                                       |                                                     |                                               |                                               |                    |   |                               |   |                                     |                           |                    |                    |<relacao\> -> =      |<relacao\> -> <\>     |<relacao\> -> >=     |<relacao\> -> <=     |<relacao\> -> >      |<relacao\> -> <      |                                                  |                                               |                                               |                                                  |                                                 |                                                 |                           |                                    |   |
|expressao    |   |                    |                                    |                                    |                                       |                                       |                                                     |<expressao\> -> <termo\> <outros_termos\>         |<expressao\> -> <termo\> <outros_termos\>         |                    |   |                               |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |<expressao\> -> <termo\> <outros_termos\>            |<expressao\> -> <termo\> <outros_termos\>         |<expressao\> -> <termo\> <outros_termos\>         |                                                  |                                                 |                                                 |                           |                                    |   |
|termo        |   |                    |                                    |                                    |                                       |                                       |                                                     |<termo\> -> <op_un\> <fator\> <mais_fatores\>      |<termo\> -> <op_un\> <fator\> <mais_fatores\>      |                    |   |                               |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |<termo\> -> <op_un\> <fator\> <mais_fatores\>         |<termo\> -> <op_un\> <fator\> <mais_fatores\>      |<termo\> -> <op_un\> <fator\> <mais_fatores\>      |                                                  |                                                 |                                                 |                           |                                    |   |
|op_un        |   |                    |                                    |                                    |                                       |                                       |                                                     |<op_un\> -> λ                                   |<op_un\> -> λ                                   |                    |   |                               |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |<op_un\> -> -                                      |<op_un\> -> λ                                   |<op_un\> -> λ                                   |                                                  |                                                 |                                                 |                           |                                    |   |
|fator        |   |                    |                                    |                                    |                                       |                                       |                                                     |<fator\> -> (<expressao\>)                       |<fator\> -> ident                               |                    |   |                               |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |                                                  |<fator\> -> numero_int                          |<fator\> -> numero_real                         |                                                  |                                                 |                                                 |                           |                                    |   |
|outros_termos|   |<outros_termos\> -> λ|                                    |                                    |                                       |                                       |                                                     |                                               |                                               |<outros_termos\> -> λ|   |<outros_termos\> -> λ           |   |                                     |                           |<outros_termos\> -> λ|<outros_termos\> -> λ|<outros_termos\> -> λ|<outros_termos\> -> λ|<outros_termos\> -> λ|<outros_termos\> -> λ|<outros_termos\> -> λ|<outros_termos\> -> λ|<outros_termos\> -> <op_ad\> <termo\> <outros_termos\>|                                               |                                               |<outros_termos\> -> <op_ad\> <termo\> <outros_termos\>|                                                 |                                                 |<outros_termos\> -> λ       |                                    |   |
|op_ad        |   |                    |                                    |                                    |                                       |                                       |                                                     |                                               |                                               |                    |   |                               |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |<op_ad\> -> -                                      |                                               |                                               |<op_ad\> -> +                                      |                                                 |                                                 |                           |                                    |   |
|mais_fatores |   |<mais_fatores\> -> λ |                                    |                                    |                                       |                                       |                                                     |                                               |                                               |<mais_fatores\> -> λ |   |<mais_fatores\> -> λ            |   |                                     |                           |<mais_fatores\> -> λ |<mais_fatores\> -> λ |<mais_fatores\> -> λ |<mais_fatores\> -> λ |<mais_fatores\> -> λ |<mais_fatores\> -> λ |<mais_fatores\> -> λ |<mais_fatores\> -> λ |<mais_fatores\> -> λ                               |                                               |                                               |<mais_fatores\> -> λ                               |<mais_fatores\> -> <op_mul\> <fator\> <mais_fatores\>|<mais_fatores\> -> <op_mul\> <fator\> <mais_fatores\>|<mais_fatores\> -> λ        |                                    |   |
|op_mul       |   |                    |                                    |                                    |                                       |                                       |                                                     |                                               |                                               |                    |   |                               |   |                                     |                           |                    |                    |                    |                    |                    |                    |                    |                    |                                                  |                                               |                                               |                                                  |<op_mul\> -> *                                    |<op_mul\> -> /                                    |                           |                                    |   |
|pfalsa       |   |                    |                                    |                                    |                                       |                                       |                                                     |                                               |                                               |                    |   |                               |   |                                     |                           |                    |<pfalsa\> -> λ       |                    |                    |                    |                    |                    |                    |                                                  |                                               |                                               |                                                  |                                                 |                                                 |<pfalsa\> -> else <comandos\>|                                    |   |

# Regras semânticas
```
Legenda de Atibutos:
inh = herdado
syn = sintentizado
lexval = valor em string do lexema do token do nó atual
end_loc = linha da última instrução em endereço de três operandos
cond_loc = linha da instrução de jump JF
goto_loc = linha da instrução de goto, existe somente se o if tem um else
left_op = operador esquerdo
right_op = operador direito
op = operador

<programa> -> program ident <corpo> .

<corpo> ->  <dc> begin 
            <comandos> { addCode("PARA", "", "", "") }
            end

<dc> -> <dc_v> <mais_dc>
<dc> -> λ

<mais_dc> -> ; <dc> 
<mais_dc> -> λ

<dc_v> ->  <tipo_var> {<variaveis>.inh = <tipo_var>.syn}
           : 
           <variaveis>

<tipo_var> -> real {<tipo_var>.syn = "real"}
<tipo_var> -> integer {<tipo_var>.syn = "integer"}

<variaveis> -> ident {
                        addEntry(ident.syn, <variaveis>.inh);
                        addCode("ALME", "0.0", "", ident.syn);
                        <mais_var>.inh = <variaveis>.inh
                     }
               <mais_var>

<mais_var> -> , {<variaveis>.inh = <mais_var>.inh} <variaveis>
<mais_var> -> λ

<comandos> ->  <comando> { <mais_comandos>.inh = <comando>.end_loc }
               <mais_comandos> { <comandos>.end_loc = <mais_comandos>.end_loc }

<mais_comandos> -> ; <comandos> { <mais_comandos>.end_loc = <comandos>.end_loc }
<mais_comandos> -> λ { <mais_comandos>.end_loc = <mais_comandos>.inh }

<comando> -> read (ident) {
                              <comando>.end_loc = addCode("read", "", "", ident.syn)
                          }
<comando> -> write (ident) {
                              <comando>.end_loc = addCode("write", "", "", ident.syn)
                           }
<comando> -> ident := <expressao> {
                                    if(<expressao>.syn.type == ident.type) 
                                    {
                                       ident.val = <expressao>.syn.val
                                       <comando>.end_loc = addCode(":=", ident.val, "",)
                                    }
                                    else {
                                       erro();
                                    }
                                  }
<comando> -> if <condicao> then 
               <comandos>  { 
                              if (<pfalsa>->child(0) == "else")
                                 <condicao>.goto_loc = addCode("goto", last_instruction_line, "", "")
                              <pfalsa>.inh = last_instruction_line
                           }
               <pfalsa> $

<condicao> ->  <expressao> { <condicao>.left_op = <expressao>.syn }
               <relacao> { <condicao>.rel_op = <relacao>.lexval }
               <expressao> { 
                              <condicao>.right_op = <expressao>.syn                               
                              <condicao>.cond_loc = addCode(<condicao>.rel_op, <condicao>.left_op, <condicao>.right_op, geraTemp())
                           }

<pfalsa> -> else <comandos>
<pfalsa> -> λ

<relacao> -> = { <relaval>.lexval = "=" }
<relacao> -> <> { <relaval>.lexval = "<>" }
<relacao> -> >= { <relaval>.lexval = ">=" }
<relacao> -> <= { <relaval>.lexval = "<=" }
<relacao> -> > { <relaval>.lexval = ">" }
<relacao> -> < { <relaval>.lexval = "<" }

<expressao> -> <termo> { <outros_termos>.inh = <termo>.syn }
               <outros_termos> { <expressao>.syn = <outros_termos>.syn }

<termo> -> <op_un> { <fator>.inh = <op_un>.val } 
           <fator> { <mais_fatores>.inh = <fator>.inh * <fator>.syn } 
           <mais_fatores> { <termo>.syn = <mais_fatores>.syn }

<op_un> -> - { <op_un>.val = "-" }
<op_un> -> λ { <op_un>.val = "" }

<fator> -> ident         { <fator>.syn = ident.syn }
<fator> -> numero_int    { <fator>.syn = numero_int.lexval }
<fator> -> numero_real   { <fator>.syn = numero_real.lexval }
<fator> -> (<expressao>) { <fator>.syn = <expressao>.syn }

.inh = <termo>.syn
<outros_termos> -> <op_ad> { <outros_termos>.op = <op_ad>.lexval }
                   <termo> { <outros_termos₁>.inh = <termo>.syn }
                   <outros_termos₁> {
                                       <outros_termos>.syn = geraTemp();
                                       addCode(<outros_termos>.op, <outros_termos>.inh, <outros_termos₁>.syn, <outros_termos>.syn)
                                    }
<outros_termos> -> λ { <outros_termos>.syn = <outros_termos>.inh }

<op_ad> -> + { <op_ad>.lexval = "+" }
<op_ad> -> - { <op_ad>.lexval = "-" }

.inh = <fator>.syn
<mais_fatores> -> <op_mul> { <mais_fatores>.op = <op_mul>.lexval }
                  <fator> { <mais_fatores₁>.inh = <fator>.syn }
                  <mais_fatores₁> {
                                    <mais_fatores>.syn = geraTemp();
                                    addCode(<mais_fatores>.op, <mais_fatores>.inh, <mais_fatores₁>.syn, <mais_fatores>.syn);
                                  }
<mais_fatores> -> λ { <mais_fatores>.syn = <mais_fatores>.inh }

<op_mul> -> * { op_mul.lexval = "*" }
<op_mul> -> / { op_mul.lexval = "/" }
```

# Output de exemplo a partir do input de exemplo
```
000; ALME; 0.0; ; a
001; ALME; 0.0; ; b
002; ALME; 0; ; c
003; ALME; 0; ; d
004; read; ; ; a
005; read; ; ; c
006; *; a; a; t1
007; +; t1; a; t2
008; :=; t2; ; b
009; *; -2; c; t3
010; +; t3; c; t4
011; :=; t4; ; d
012; >; a; b; t5
013; JF; t5; 16; 
014; write; a; ; 
015; goto; 17; ; 
016; write; b; ; 
017; <; d; c; t6
018; JF; t6; 21; 
019; write; d; ; 
020; goto; 22; ; 
021; write; c; ; 
022; PARA; ; ; 

```

# Árvore de parse anotada
obviamente as anotações seguem as regras semânticas
```
<programa>
|- program -> program
|- ident -> teste
|- <corpo>
|- |- <dc>
|- |- |- <dc_v>
|- |- |- |- <tipo_var>
|- |- |- |- [attr] <tipo_var>.syn = real (string)
|- |- |- |- |- real -> real
|- |- |- |- : -> :
|- |- |- |- <variaveis>
|- |- |- |- [attr] <variaveis>.inh = real (string)
|- |- |- |- |- ident -> a
|- |- |- |- |- <mais_var>
|- |- |- |- |- [attr] <mais_var>.inh = real (string)
|- |- |- |- |- |- , -> ,
|- |- |- |- |- |- <variaveis>
|- |- |- |- |- |- [attr] <variaveis>.inh = real (string)
|- |- |- |- |- |- |- ident -> b
|- |- |- |- |- |- |- <mais_var>
|- |- |- |- |- |- |- [attr] <mais_var>.inh = real (string)
|- |- |- |- |- |- |- |- &
|- |- |- <mais_dc>
|- |- |- |- ; -> ;
|- |- |- |- <dc>
|- |- |- |- |- <dc_v>
|- |- |- |- |- |- <tipo_var>
|- |- |- |- |- |- [attr] <tipo_var>.syn = integer (string)
|- |- |- |- |- |- |- integer -> integer
|- |- |- |- |- |- : -> :
|- |- |- |- |- |- <variaveis>
|- |- |- |- |- |- [attr] <variaveis>.inh = integer (string)
|- |- |- |- |- |- |- ident -> c
|- |- |- |- |- |- |- <mais_var>
|- |- |- |- |- |- |- [attr] <mais_var>.inh = integer (string)
|- |- |- |- |- |- |- |- , -> ,
|- |- |- |- |- |- |- |- <variaveis>
|- |- |- |- |- |- |- |- [attr] <variaveis>.inh = integer (string)
|- |- |- |- |- |- |- |- |- ident -> d
|- |- |- |- |- |- |- |- |- <mais_var>
|- |- |- |- |- |- |- |- |- [attr] <mais_var>.inh = integer (string)
|- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- <mais_dc>
|- |- |- |- |- |- ; -> ;
|- |- |- |- |- |- <dc>
|- |- |- |- |- |- |- &
|- |- begin -> begin
|- |- <comandos>
|- |- [attr] <comandos>.end_loc = 4 (integer)
|- |- |- <comando>
|- |- |- [attr] <comando>.end_loc = 4 (integer)
|- |- |- |- read -> read
|- |- |- |- ( -> (
|- |- |- |- ident -> a
|- |- |- |- ) -> )
|- |- |- <mais_comandos>
|- |- |- [attr] <mais_comandos>.end_loc = 4 (integer)
|- |- |- [attr] <mais_comandos>.inh = 4 (integer)
|- |- |- |- ; -> ;
|- |- |- |- <comandos>
|- |- |- |- [attr] <comandos>.end_loc = 5 (integer)
|- |- |- |- |- <comando>
|- |- |- |- |- [attr] <comando>.end_loc = 5 (integer)
|- |- |- |- |- |- read -> read
|- |- |- |- |- |- ( -> (
|- |- |- |- |- |- ident -> c
|- |- |- |- |- |- ) -> )
|- |- |- |- |- <mais_comandos>
|- |- |- |- |- [attr] <mais_comandos>.end_loc = 5 (integer)
|- |- |- |- |- [attr] <mais_comandos>.inh = 5 (integer)
|- |- |- |- |- |- ; -> ;
|- |- |- |- |- |- <comandos>
|- |- |- |- |- |- [attr] <comandos>.end_loc = 8 (integer)
|- |- |- |- |- |- |- <comando>
|- |- |- |- |- |- |- [attr] <comando>.end_loc = 8 (integer)
|- |- |- |- |- |- |- |- ident -> b
|- |- |- |- |- |- |- |- := -> :=
|- |- |- |- |- |- |- |- <expressao>
|- |- |- |- |- |- |- |- [attr] <expressao>.syn = t2 (real)
|- |- |- |- |- |- |- |- |- <termo>
|- |- |- |- |- |- |- |- |- [attr] <termo>.syn = t1 (real)
|- |- |- |- |- |- |- |- |- |- <op_un>
|- |- |- |- |- |- |- |- |- |- [attr] <op_un>.val = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- <fator>
|- |- |- |- |- |- |- |- |- |- [attr] <fator>.inh = {null} (string)
|- |- |- |- |- |- |- |- |- |- [attr] <fator>.syn = a (real)
|- |- |- |- |- |- |- |- |- |- |- ident -> a
|- |- |- |- |- |- |- |- |- |- <mais_fatores>
|- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.inh = a (real)
|- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.op = * (string)
|- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.syn = t1 (real)
|- |- |- |- |- |- |- |- |- |- |- <op_mul>
|- |- |- |- |- |- |- |- |- |- |- [attr] <op_mul>.lexval = * (string)
|- |- |- |- |- |- |- |- |- |- |- |- * -> *
|- |- |- |- |- |- |- |- |- |- |- <fator>
|- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.syn = a (real)
|- |- |- |- |- |- |- |- |- |- |- |- ident -> a
|- |- |- |- |- |- |- |- |- |- |- <mais_fatores>
|- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.inh = a (real)
|- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.syn = a (real)
|- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- <outros_termos>
|- |- |- |- |- |- |- |- |- [attr] <outros_termos>.inh = t1 (real)
|- |- |- |- |- |- |- |- |- [attr] <outros_termos>.op = + (string)
|- |- |- |- |- |- |- |- |- [attr] <outros_termos>.syn = t2 (real)
|- |- |- |- |- |- |- |- |- |- <op_ad>
|- |- |- |- |- |- |- |- |- |- [attr] <op_ad>.lexval = + (string)
|- |- |- |- |- |- |- |- |- |- |- + -> +
|- |- |- |- |- |- |- |- |- |- <termo>
|- |- |- |- |- |- |- |- |- |- [attr] <termo>.syn = a (real)
|- |- |- |- |- |- |- |- |- |- |- <op_un>
|- |- |- |- |- |- |- |- |- |- |- [attr] <op_un>.val = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- <fator>
|- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.inh = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.syn = a (real)
|- |- |- |- |- |- |- |- |- |- |- |- ident -> a
|- |- |- |- |- |- |- |- |- |- |- <mais_fatores>
|- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.inh = a (real)
|- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.syn = a (real)
|- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- <outros_termos>
|- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.inh = a (real)
|- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.syn = a (real)
|- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- <mais_comandos>
|- |- |- |- |- |- |- [attr] <mais_comandos>.end_loc = 8 (integer)
|- |- |- |- |- |- |- [attr] <mais_comandos>.inh = 8 (integer)
|- |- |- |- |- |- |- |- ; -> ;
|- |- |- |- |- |- |- |- <comandos>
|- |- |- |- |- |- |- |- [attr] <comandos>.end_loc = 11 (integer)
|- |- |- |- |- |- |- |- |- <comando>
|- |- |- |- |- |- |- |- |- [attr] <comando>.end_loc = 11 (integer)
|- |- |- |- |- |- |- |- |- |- ident -> d
|- |- |- |- |- |- |- |- |- |- := -> :=
|- |- |- |- |- |- |- |- |- |- <expressao>
|- |- |- |- |- |- |- |- |- |- [attr] <expressao>.syn = t4 (integer)
|- |- |- |- |- |- |- |- |- |- |- <termo>
|- |- |- |- |- |- |- |- |- |- |- [attr] <termo>.syn = t3 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- <op_un>
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <op_un>.val = - (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- - -> -
|- |- |- |- |- |- |- |- |- |- |- |- <fator>
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.inh = - (string)
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.syn = -2 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- numero_int -> 2
|- |- |- |- |- |- |- |- |- |- |- |- <mais_fatores>
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.inh = -2 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.op = * (string)
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.syn = t3 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- <op_mul>
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <op_mul>.lexval = * (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- * -> *
|- |- |- |- |- |- |- |- |- |- |- |- |- <fator>
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.syn = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- ident -> c
|- |- |- |- |- |- |- |- |- |- |- |- |- <mais_fatores>
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.inh = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.syn = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- <outros_termos>
|- |- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.inh = t3 (integer)
|- |- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.op = + (string)
|- |- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.syn = t4 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- <op_ad>
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <op_ad>.lexval = + (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- + -> +
|- |- |- |- |- |- |- |- |- |- |- |- <termo>
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <termo>.syn = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- <op_un>
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <op_un>.val = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- |- <fator>
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.inh = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.syn = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- ident -> c
|- |- |- |- |- |- |- |- |- |- |- |- |- <mais_fatores>
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.inh = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.syn = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- <outros_termos>
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.inh = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.syn = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- <mais_comandos>
|- |- |- |- |- |- |- |- |- [attr] <mais_comandos>.end_loc = 11 (integer)
|- |- |- |- |- |- |- |- |- [attr] <mais_comandos>.inh = 11 (integer)
|- |- |- |- |- |- |- |- |- |- ; -> ;
|- |- |- |- |- |- |- |- |- |- <comandos>
|- |- |- |- |- |- |- |- |- |- [attr] <comandos>.end_loc = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- <comando>
|- |- |- |- |- |- |- |- |- |- |- [attr] <comando>.end_loc = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- if -> if
|- |- |- |- |- |- |- |- |- |- |- |- <condicao>
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <condicao>.goto_loc = 15 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <condicao>.jump_loc = 13 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <condicao>.left_op = a (real)
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <condicao>.rel_op = > (string)
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <condicao>.right_op = b (real)
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <condicao>.syn = t5 (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- <expressao>
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <expressao>.syn = a (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- <termo>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <termo>.syn = a (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <op_un>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <op_un>.val = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <fator>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.inh = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.syn = a (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- ident -> a
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <mais_fatores>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.inh = a (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.syn = a (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- |- |- <outros_termos>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.inh = a (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.syn = a (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- |- <relacao>
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <relacao>.lexval = > (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- > -> >
|- |- |- |- |- |- |- |- |- |- |- |- |- <expressao>
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <expressao>.syn = b (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- <termo>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <termo>.syn = b (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <op_un>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <op_un>.val = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <fator>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.inh = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.syn = b (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- ident -> b
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <mais_fatores>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.inh = b (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.syn = b (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- |- |- <outros_termos>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.inh = b (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.syn = b (real)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- then -> then
|- |- |- |- |- |- |- |- |- |- |- |- <comandos>
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <comandos>.end_loc = 14 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- <comando>
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <comando>.end_loc = 14 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- write -> write
|- |- |- |- |- |- |- |- |- |- |- |- |- |- ( -> (
|- |- |- |- |- |- |- |- |- |- |- |- |- |- ident -> a
|- |- |- |- |- |- |- |- |- |- |- |- |- |- ) -> )
|- |- |- |- |- |- |- |- |- |- |- |- |- <mais_comandos>
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_comandos>.end_loc = 14 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_comandos>.inh = 14 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- <pfalsa>
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <pfalsa>.end_loc = 16 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <pfalsa>.inh = 16 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- else -> else
|- |- |- |- |- |- |- |- |- |- |- |- |- <comandos>
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <comandos>.end_loc = 16 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- <comando>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <comando>.end_loc = 16 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- write -> write
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- ( -> (
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- ident -> b
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- ) -> )
|- |- |- |- |- |- |- |- |- |- |- |- |- |- <mais_comandos>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_comandos>.end_loc = 16 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_comandos>.inh = 16 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- $ -> $
|- |- |- |- |- |- |- |- |- |- |- <mais_comandos>
|- |- |- |- |- |- |- |- |- |- |- [attr] <mais_comandos>.end_loc = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- [attr] <mais_comandos>.inh = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- ; -> ;
|- |- |- |- |- |- |- |- |- |- |- |- <comandos>
|- |- |- |- |- |- |- |- |- |- |- |- [attr] <comandos>.end_loc = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- <comando>
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <comando>.end_loc = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- if -> if
|- |- |- |- |- |- |- |- |- |- |- |- |- |- <condicao>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <condicao>.goto_loc = 20 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <condicao>.jump_loc = 18 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <condicao>.left_op = d (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <condicao>.rel_op = < (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <condicao>.right_op = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <condicao>.syn = t6 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <expressao>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <expressao>.syn = d (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <termo>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <termo>.syn = d (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <op_un>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <op_un>.val = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <fator>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.inh = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.syn = d (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- ident -> d
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <mais_fatores>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.inh = d (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.syn = d (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <outros_termos>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.inh = d (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.syn = d (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <relacao>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <relacao>.lexval = < (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- < -> <
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <expressao>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <expressao>.syn = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <termo>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <termo>.syn = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <op_un>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <op_un>.val = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <fator>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.inh = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <fator>.syn = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- ident -> c
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <mais_fatores>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.inh = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_fatores>.syn = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <outros_termos>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.inh = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <outros_termos>.syn = c (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- |- |- then -> then
|- |- |- |- |- |- |- |- |- |- |- |- |- |- <comandos>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <comandos>.end_loc = 19 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <comando>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <comando>.end_loc = 19 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- write -> write
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- ( -> (
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- ident -> d
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- ) -> )
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <mais_comandos>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_comandos>.end_loc = 19 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_comandos>.inh = 19 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- |- |- <pfalsa>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <pfalsa>.end_loc = 21 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <pfalsa>.inh = 21 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- else -> else
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <comandos>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <comandos>.end_loc = 21 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <comando>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <comando>.end_loc = 21 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- write -> write
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- ( -> (
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- ident -> c
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- ) -> )
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- <mais_comandos>
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_comandos>.end_loc = 21 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_comandos>.inh = 21 (integer)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- |- |- |- |- |- |- |- |- |- |- |- |- $ -> $
|- |- |- |- |- |- |- |- |- |- |- |- |- <mais_comandos>
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_comandos>.end_loc = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- [attr] <mais_comandos>.inh = {null} (string)
|- |- |- |- |- |- |- |- |- |- |- |- |- |- &
|- |- end -> end
|- . -> .
```