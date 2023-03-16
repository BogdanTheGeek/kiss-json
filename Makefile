CC=cc
IDIR=./
CFLAGS=-Wall -pedantic -I$(IDIR)
SRC:=$(wildcard *.c)
OBJ:=$(patsubst %.c,%.o,$(SRC))
OBJ:= $(filter-out main.o,$(OBJ))

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


.PHONY: clean
clean:
	rm -f *.o main
	@echo "Everything Clean!"

.PHONY: format
format:
	@clang-format --style=file -i *.c *.h
	@echo "$(JAZZ)Formatted!$(RESET)"


