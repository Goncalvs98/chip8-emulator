# Nome do seu executável
TARGET = chip8

# Compilador: clang (do toolchain MinGW)
CC = clang

# --- Configuração Específica do Windows ---
# Caminhos para sua instalação manual do SDL2
SDL_INCLUDE_PATH = -IC:/SDL2/include
SDL_LIB_PATH = -LC:/SDL2/lib
# ------------------------------------------

# Flags do Compilador
# -g: Adiciona símbolos de debug
# -Wall -Wextra -Wpedantic: Warnings rigorosos
# -std=c11: Especifica o padrão C11
# $(SDL_INCLUDE_PATH): Adiciona o path de include do SDL2
CFLAGS = -g -Wall -Wextra -Wpedantic -std=c11 -Isrc $(SDL_INCLUDE_PATH)

# Flags do Linker
# $(SDL_LIB_PATH): Adiciona o path da biblioteca do SDL2
LDFLAGS = $(SDL_LIB_PATH) -lSDL2 -Wl,--subsystem,console

# Arquivos fonte
SOURCES = src/main.c src/chip8.c

# Converte arquivos .c em arquivos .o (objeto)
OBJECTS = $(SOURCES:.c=.o)

# Regra padrão: compilar tudo
all: $(TARGET)

# Regra para linkar o executável final
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Regra para compilar cada arquivo .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpar os arquivos compilados
clean:
	-del src\*.o $(TARGET).exe 2>NUL || rm -f src/*.o $(TARGET)
# Usa 'del' no Windows e 'rm' no Linux/macOS
# O '-' ignora erros se os arquivos não existirem
