CC:= cc
INC:= ./
SRC:= $(wildcard *.c)
OBJ:= $(patsubst %.c,%.o,$(SRC))
OBJ:= $(filter-out main.o,$(OBJ))
OBJ:= $(filter-out test.o,$(OBJ))

include Colour.mk
include Flags.mk

ifeq ($(shell uname -s),Darwin)
	MEMCHECK := leaks --atExit --
else
	MEMCHECK := valgrind --leak-check=full --show-leak-kinds=all
endif

#rules
.PHONY: all
all: format main test.bin
	@echo "Done!"

main: $(OBJ) main.c
	@echo "$(WARNING)Building: $@ $(RESET)"
	@$(CC) -o $@ $^ $(CFLAGS)
	@echo "$(SUCCESS)$@: done!$(RESET)"

test.bin: $(OBJ) test.c
	@echo "$(WARNING)Building: $@ $(RESET)"
	@$(CC) -o $@ $^ $(CFLAGS)
	@echo "$(SUCCESS)$@: done!$(RESET)"

%.o: %.c %.h
	@echo "$(WARNING)Building object $@ $(RESET)"
	@$(CC) -o $@ -c $< $(CFLAGS)
	@echo "$(SUCCESS)$@: done!$(RESET)"

.PHONY: run
run: main
	@chmod +x $<
	@./$<

.PHONY: test
test: test.bin
	@chmod +x $<
	@./$< && echo "$(SUCCESS)PASS!$(RESET)" || echo "$(ERROR)FAIL!$(RESET) For more details, run:\r\n./$<"

.PHONY: memcheck
memcheck: main
	@printf "$(INFO)Memcheck $(TEST_NAME): $(RESET)"
	@chmod +x $<
	@$(MEMCHECK) ./$< &>/dev/null && echo "$(SUCCESS)PASS!$(RESET)" || echo "$(ERROR)FAIL!$(RESET) For more details, run:\r\n$(MEMCHECK) ./$<"

.PHONY: clean
clean:
	rm -f *.o main test.bin
	rm -rf *.dSYM
	@echo "Everything Clean!"

.PHONY: format
format:
	@clang-format --style=file -i *.c *.h
	@echo "$(JAZZ)Formatted!$(RESET)"


