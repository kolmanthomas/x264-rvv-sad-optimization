# Assumes the presence of a RISC-V environment with Vector

CC = gcc
CFLAGS = -march=rv64gcv -O3 -Wall
SRC = rvv_sad_sandbox.c
TARGET = rvv_sad_sandbox

all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

x86:
	$(CC) -O3 -Wall -o $(TARGET) $(SRC)
