# Assumes the presence of a RISC-V environment with Vector

CC = gcc
CFLAGS = -march=rv64gcv -O3 -Wall -Wno-unused
NO_AUTOVEC_SRCS = sad_scalar

# Output dirs
OBJ_DIR = out/obj
ASM_DIR = out/asm
BIN_DIR = out

SRC = rvv_sad_sandbox.c $(NO_AUTOVEC_SRCS).c sad_rvv.c sad_autovec.c
ASM = $(SRC:%.c=$(ASM_DIR)/%.s)
OBJ = $(SRC:%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/rvv_sad_sandbox

.PHONY: all profile clean

all: $(TARGET)
profile: $(TARGET)

# Target-specific compile flags.
profile: CFLAGS += -pg
profile: LDFLAGS += -pg
$(OBJ_DIR)/$(NO_AUTOVEC_SRCS).o: CFLAGS += -fno-tree-vectorize
$(ASM_DIR)/$(NO_AUTOVEC_SRCS).s: CFLAGS += -fno-tree-vectorize

$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $(OBJ) -o $(TARGET)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

x86: $(SRC) | $(BIN_DIR)
	$(CC) -O3 -Wall -o $(TARGET) $(SRC)

asm: $(ASM_DIR) | $(ASM)

$(ASM_DIR)/%.s: %.c
	$(CC) $(CFLAGS) -S $< -o $@

$(OBJ_DIR) $(BIN_DIR) $(ASM_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR) $(ASM_DIR)
