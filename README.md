# Emulador CHIP-8 em C

Um emulador simples para o sistema CHIP-8, escrito em C puro (C11) e usando SDL2 para renderização e input.

Este é um projeto de fim de semana com o objetivo de aprofundar os conhecimentos em C, gerenciamento de memória, operações *bitwise* e arquitetura de computadores.

## Tecnologias

* **Linguagem:** C (padrão C11)
* **Compilador:** `clang` (através do MinGW-w64)
* **Biblioteca Gráfica:** `SDL2`
* **Sistema de Build:** `make` (`mingw32-make`)

## Como Compilar (Windows com MinGW)

Este projeto foi configurado em um ambiente `MinGW-w64` (MSYS2) com `clang`, `mingw32-make` e `SDL2`.

### 1. Dependências

Você precisará ter o `clang` e o `make` instalados no seu ambiente MinGW:
```bash
pacman -S mingw-w64-x86_64-clang
pacman -S mingw-w64-x86_64-make
```

(Lembre-se de criar o alias alias make='mingw32-make' no seu .bashrc)

2. Biblioteca SDL2

O Makefile deste projeto espera encontrar os arquivos de desenvolvimento do SDL2 (versão MinGW 64-bit) nos seguintes diretórios:
    Headers: C:/SDL2/include/
    Bibliotecas: C:/SDL2/lib/

3. Compilando

Depois que as dependências estiverem nos locais corretos, basta executar make:

```Bash
make
```

Isso irá compilar e linkar o executável chip8.exe.

4. Arquivos de Runtime

Para executar, o chip8.exe precisa do SDL2.dll (da pasta bin do SDL2) no mesmo diretório.

Como Usar

O executável espera um argumento: o caminho para a ROM do CHIP-8 que você deseja carregar.

```Bash
./chip8.exe roms/test_opcode.ch8
```

Estado Atual

    [X] Setup do projeto com clang, make e SDL2.

    [X] Abre uma janela gráfica usando SDL2.

    [X] Rotina de inicialização da CPU (limpa memória, carrega fontset).

    [X] Carrega a ROM do disco para a memória simulada.

    [X] Implementar o ciclo Fetch-Decode-Execute.

    [ ] TODO: Implementar os 35 opcodes do CHIP-8.

    [ ] TODO: Renderizar o video_buffer na tela.

    [ ] TODO: Implementar o input do teclado.

    [ ] TODO: Implementar os timers (delay e som).

---

### 3. Comandos do Git para Iniciar

Agora que seus arquivos `.gitignore` e `README.md` estão prontos, abra seu terminal MINGW64 na pasta do projeto e execute os seguintes comandos:

**Passo 1: Inicializar o repositório**
(Cria a pasta oculta `.git` para rastrear as mudanças)

```bash
git init
```

Passo 2: Adicionar todos os arquivos (O git add . vai ler seu .gitignore e não vai adicionar os arquivos .o e .exe. Ele vai adicionar Makefile, README.md, src/ e roms/)

```Bash
git add .
```

Passo 3: Fazer o primeiro commit (Salva a "fotografia" atual do seu projeto na história do Git)

```Bash
git commit -m "Commit inicial - Setup do projeto, 'Hello Window' com SDL2 e carregam
```