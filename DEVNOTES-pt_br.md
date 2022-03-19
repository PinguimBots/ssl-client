# Assuntos

- [Assuntos](#assuntos)
- [Sobre a aplicação](#sobre-a-aplicação)
  - [O `main()`](#o-main)
  - [O que faz o `subsystem_manager` ?](#o-que-faz-o-subsystem_manager-)
  - [E o `mario` ?](#e-o-mario-)
- [Subsistemas](#subsistemas)
  - [Como faço para o `manager` mostrar meu subsistema ?](#como-faço-para-o-manager-mostrar-meu-subsistema-)

# Sobre a aplicação

## O `main()`

O nosso `main()` é bem simples a sua função é somente montar
o estágio, iniciar o loop da aplicação e, quando sair,
desmontar o estágio.

Mais especificamente, vamos iniciar o `ImGui` (biblioteca de
interface gráfica) com o [`mario`](#e-o-mario-), criar um
`subsystem_manager` e entrar num loop.

No loop vamos devemos apenas:
- Ver se temos que sair do loop
- Comunicar as vontades da aplicação para o `mario`
- Deixar o `subsystem_manager` fazer as coisas dele
- E renderizar o frame

E só, nada mais.

## O que faz o `subsystem_manager` ?

O `manager` instancia, alimenta com informações e chama os
subsistemas da aplicação.

Ele realmente é bem simples também e so serve criar e destruir
os subsistemas quando é necessário.

## E o `mario` ?

O `mario` é o nosso encanador, ele conecta (e desconecta) a
tubulação pra fazer o `ImGui` funcionar sem problemas.

Você provavelmente nunca terá que olhar pro `mario`, mas como
curiosidade essas são as coisas que ele faz:
- Inicializa o OpenGl (glew)
- Inicializa o SDL
- Inicializa o SDL_GL
- Cria o contexto do SDL
- Cria a janela
- Inicializa o ImGui
- Conecta o ImGui ao SDL e OpenGl

Como essas coisas não podem ficar sendo feitas e desfeitas
o `mario` é um *singleton*, então se quisermos que ele faça alguma
coisa em especial vamos ter que passar pelo seu dono, o `main()`.

# Subsistemas

Existem 4 tipos de subsistemas: `input`, `logic`, `ouput` e
`misc`.

`input`, `logic` e `subsystem` formam uma *pipeline*, em que
cada parte pega o resultado da parte anterior e faz alguma coisa
com ele. O `manager` só pode criar 1 de cada um desses tipos de
subsistema.

Já o `misc` é apenas um subsistema solto que quer fazer parte do
loop da aplicação, o `manager` pode fazer quantos quiser (mas
um de cada tipo).

Os `misc` são tipo serviços, podemos usar para ajudar em algum
lugar que precise, inicializar algo fora do *pipeline* ou para
debugar alguma coisa.

Por exemplo, podemos fazer um subsistema `misc` de *logging*, dai
implementamos uma funcao `log()` que redireciona as suas chamadas
para esse subsistema, mostrando os *logs* na sua própria janela.

## Como faço para o `manager` mostrar meu subsistema ?

Para o subsistema ficar visível para o `manager` ele deve ser
registrado no `registrar`, que é como se fosse um escrivão.

Isso deve acontecer antes do `main()`, então tem que ser feito
de forma estática.

O nosso amigo `registrar.hpp` já nos da uma forma fácil de fazer isso:

<table>
<tr>

<th>

`pinguim/app/subsystems/registrar.hpp`

</th><th>

`pinguim/app/subsystems/input/meu_novo_subsistema_de_input.cpp`

</th></tr>
<tr>

<td>

```cpp
#define PINGUIM_APP_REGISTER_INPUT_SUBSYSTEM(class, name) \
    // Implementação aqui
#define PINGUIM_APP_REGISTER_LOGIC_SUBSYSTEM(class, name) \
    // Implementação aqui
#define PINGUIM_APP_REGISTER_OUTPUT_SUBSYSTEM(class, name) \
    // Implementação aqui
#define PINGUIM_APP_REGISTER_MISC_SUBSYSTEM(class, name) \
    // Implementação aqui
```

</td><td>

```cpp
#include "pinguim/app/subsystems/input/meu_novo_subsistema_de_input.hpp"

#include "pinguim/app/subsystems/registrar.hpp"

PINGUIM_APP_REGISTER_INPUT_SUBSYSTEM(
    pinguim::app::subsystems::input::meu_novo_subsistema_de_input,
    "Nome que vai aparecer no dropdown"
);

// Implementação da classe...
```

</td></tr>
</table>

Fica convencionado que vamos botar o macro sempre no topo do `.cpp`
depois de todos os `#includes`.

> Fica por curiosidade olhar a implementação do macro em
> `registrar.hpp` e a implementação do `manager` em
> `manager.hpp` para tentar entender como as peças funcionam juntas.
>
> Duas dicas: `registrar::instance()` retorna um *singleton*
> e as funções `registrar::register_*_subsystem()` recebem
> uma *factory*.
