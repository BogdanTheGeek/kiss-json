CC=cc
IDIR=./
CFLAGS :=-g -O0 -Wall -Wextra -Wmissing-declarations -Wconversion -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -Wsuggest-attribute=const -Wunknown-warning-option -Wunused -Wuninitialized -Wno-unknown-warning-option -I$(IDIR)
SRC:=$(wildcard *.c)
OBJ:=$(patsubst %.c,%.o,$(SRC))
OBJ:= $(filter-out main.o,$(OBJ))

ifeq ($(shell uname -s),Darwin)
	MEMCHECK := leaks --atExit --
else
	MEMCHECK := valgrind --leak-check=full --show-leak-kinds=all
endif

# define standard colors
ifneq (,$(findstring 256color,${TERM}))
	FAIL    := $(shell tput -Txterm setaf 1)
	SUCCESS := $(shell tput -Txterm setaf 2)
	WARNING := $(shell tput -Txterm setaf 3)
	JAZZ    := $(shell tput -Txterm setaf 4)
	RESET   := $(shell tput -Txterm sgr0)
else
	FAIL    := ""
	SUCCESS := ""
	WARNING := ""
	JAZZ    := ""
endif

#rules
.PHONY: all
all: format main
	@echo "Done!"

main: $(OBJ) main.c
	@echo "$(WARNING)Building: $@ $(RESET)"
	@$(CC) -o $@ $^ $(CFLAGS)
	@echo "$(SUCCESS)$@: done!$(RESET)"

%.o: %.c %.h
	@echo "$(WARNING)Building object $@ $(RESET)"
	@$(CC) -o $@ -c $< $(CFLAGS)
	@echo "$(SUCCESS)$@: done!$(RESET)"

.PHONY: test
test: main
	@chmod +x $<
	@./$< && echo "$(SUCCESS) PASS!$(RESET)" || echo "$(ERROR) FAIL!$(RESET) For more details, run:\r\n./$<"

.PHONY: memcheck
memcheck: main
	@printf "$(INFO)Memcheck $(TEST_NAME): $(RESET)"
	@chmod +x $<
	@$(MEMCHECK) ./$< &>/dev/null && echo "$(SUCCESS) PASS!$(RESET)" || echo "$(ERROR) FAIL!$(RESET) For more details, run:\r\n$(MEMCHECK) ./$<"

.PHONY: clean
clean:
	rm -f *.o main
	rm -rf *.dSYM
	@echo "Everything Clean!"

.PHONY: format
format:
	@clang-format --style=file -i *.c *.h
	@echo "$(JAZZ)Formatted!$(RESET)"


