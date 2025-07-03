EXE = find-repos
SRC = main.c
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic -O2

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

.PHONY: install
install: $(EXE)
	install -m 755 $(EXE) $(PREFIX)/bin/
