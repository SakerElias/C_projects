CFLAGS = -Wall -Wextra -ggdb3  -Iinclude
LFLAGS = -ldl -lm

FUNC_TESTS_SRC = $(wildcard prog/*.c)
FUNC_TESTS_BIN = $(subst prog, bin, $(FUNC_TESTS_SRC:%.c=%.exe))

UNIT_TESTS_SRC = $(wildcard tests/*.c)
UNIT_TESTS_BIN = $(subst tests, bin/tests, $(UNIT_TESTS_SRC:%.c=%.exe))

UNIT_TESTS_HOWTO_SRC = $(wildcard tests/howto/*.c)
UNIT_TESTS_HOWTO_BIN = $(subst tests/howto, bin/tests/howto, $(UNIT_TESTS_HOWTO_SRC:%.c=%.exe))

SRC = $(wildcard src/*.c)
OBJ = $(SRC:%.c=%.o)
INCLUDES = $(wildcard include/*/*.h)

all : $(FUNC_TESTS_BIN) $(UNIT_TESTS_BIN) $(UNIT_TESTS_HOWTO_BIN)

bin/%.exe : prog/%.o $(OBJ) $(INCLUDES)
	@mkdir -p bin
	$(CC) $(OBJ) $< $(LFLAGS) -o $@

bin/tests/%.exe : tests/%.o $(OBJ) $(INCLUDES)
	@mkdir -p bin/tests/howto
	$(CC) $(OBJ) $< $(LFLAGS) -o $@

clean :
	$(RM) $(OBJ)
	$(RM) $(FUNC_TESTS_BIN) $(UNIT_TESTS_BIN) $(UNIT_TESTS_HOWTO_BIN)
	find . -name "*~" -delete
