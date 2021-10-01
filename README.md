# Título
Frontend de compilador faltando análise semântica e geração de código intermediário.

## TODO:
```
//TODO:
```

# Build
```
mkdir build
make
make run
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

# Regras semânticas (incompletas)
```
<programa> -> program ident <corpo> .

<corpo> -> <dc> begin <comandos> end

<dc> -> <dc_v> <mais_dc>
<dc> -> λ

<mais_dc> -> ; <dc> 
<mais_dc> -> λ

<dc_v> ->  <tipo_var> {<variaveis>.inh = <tipo_var>.syn}
           : 
           <variaveis>

<tipo_var> -> real {<tipo_var>.syn = "real"}
<tipo_var> -> integer {<tipo_var>.syn = "real"}

<variaveis> -> ident {
                        addEntry(ident.syn, <variaveis>.inh);
                        addCode("ALME", "0.0", "", ident.syn);
                        <mais_var>.inh = <variaveis>.inh
                     }
               <mais_var>

<mais_var> -> , {<variaveis>.inh = <mais_var>.inh} <variaveis>
<mais_var> -> λ

<comandos> -> <comando> <mais_comandos>

<mais_comandos> -> ; <comandos>
<mais_comandos> -> λ

<comando> -> read (ident) {
                              addCode("read", "", "", ident.syn)
                          }
<comando> -> write (ident) {
                              addCode("write", "", "", ident.syn)
                           }
<comando> -> ident := <expressao> {
                                     if(<expressao>.syn)
                                  }
<comando> -> if <condicao> then <comandos> <pfalsa> $

<condicao> -> <expressao> <relacao> <expressao>

<relacao> -> =
<relacao> -> <>
<relacao> -> >=
<relacao> -> <=
<relacao> -> >
<relacao> -> <

<expressao> -> <termo> { <outros_termos>.inh = <termo>.syn }
               <outros_termos> { <expressao>.syn = <outros_termos>.syn }

<termo> -> <op_un> { <fator>.inh = <op_un>.val } 
           <fator> { <mais_fatores>.inh = <fator>.inh * <fator>.syn } 
           <mais_fatores> { <termo>.syn = <mais_fatores>.syn }

<op_un> -> - { <op_un>.val = -1}
<op_un> -> λ { <op_un>.val = 1}

<fator> -> ident         { <fator>.syn = ident.syn }
<fator> -> numero_int    { <fator>.syn = numero_int.lexval }
<fator> -> numero_real   { <fator>.syn = numero_real.lexval }
<fator> -> (<expressao>) { <fator>.syn = <expressao>.syn }

<outros_termos> -> <op_ad> { <outros_termos>.op = <op_ad>.lexval }
                   <termo> {
                              <outros_termos>.inh = geraTemp();
                              code(<outros_termos>.op, <outros_termos>.inh, <termo>.syn, <outros_termos>.inh)
                           }
                   <outros_termos₁> { <outros_termos>.syn = <outros_termos₁>.syn }
<outros_termos> -> λ { <outros_termos>.syn = <outros_termos>.inh }

<op_ad> -> + { <op_ad>.lexval = "+" }
<op_ad> -> - { <op_ad>.lexval = "-" }

.inh = <fator>
<mais_fatores> -> <op_mul> { <mais_fatores>.op = <op_mul>.lexval }
                  <fator> { <mais_fatores₁>.inh = <fator>.syn }
                  <mais_fatores₁> {
                                    <mais_fatores>.syn = geraTemp();
                                    code(<mais_fatores>.op, <mais_fatores>.inh, <mais_fatores₁>.syn, <mais_fatores>.syn);
                                  }
<mais_fatores> -> λ { <mais_fatores>.syn = <mais_fatores>.inh }

<op_mul> -> * { op_mul.lexval = "*" }
<op_mul> -> / { op_mul.lexval = "/" }

<pfalsa> -> else <comandos>
<pfalsa> -> λ
```