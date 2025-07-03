EXE = find-repos
SRC = main.c
CFLAGS = -Wall -Werror -pedantic

.PHONY: all
all: $(EXE)

$(EXE): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	$(RM) $(EXE)

.PHONY: format
format:
	clang-format -i $(SRC)
