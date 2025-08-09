EXE = find-repos
SRC = main.c

# Use DEBUG=1 for a debug build with sanitisers
ifdef DEBUG
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic -g -O0 \
         -fsanitize=address -fsanitize=undefined \
         -fno-omit-frame-pointer -fno-optimize-sibling-calls
else
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic -O2
endif

.PHONY: all
all: $(EXE)

$(EXE): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: debug
debug:
	$(MAKE) DEBUG=1 $(EXE)

.PHONY: clean
clean:
	$(RM) $(EXE)

.PHONY: format
format:
	clang-format -i $(SRC)

.PHONY: install
install: $(EXE)
	install -m 755 $(EXE) $(PREFIX)/bin/
