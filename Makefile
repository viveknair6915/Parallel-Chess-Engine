CC = gcc
CFLAGS = -fopenmp -O2
SRC = src/seq_vs_paral.c
TARGET = chess

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET) results.txt 