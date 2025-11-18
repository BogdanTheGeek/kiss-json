CC:= cc
INC:= ./
SRC:= $(wildcard *.c)
OBJ:= $(patsubst %.c,%.o,$(SRC))
OBJ:= $(filter-out main.o,$(OBJ))
OBJ:= $(filter-out test.o,$(OBJ))

include Colour.mk
include Flags.mk

.PHONY: all
all: format main test.bin
	@echo "Done!"

main: $(OBJ) main.c
	@echo "$(WARNING)Building: $@ $(RESET)"
	@$(CC) -o $@ $^ $(CFLAGS)
	@echo "$(SUCCESS)$@: done!$(RESET)"

kJSON_small.o: kJSON.c kJSON.h
	@echo "$(WARNING)Building object $@ $(RESET)"
	@$(CC) -o $@ -c $< $(CFLAGS) -DCONFIG_KJSON_SMALLEST=1
	@echo "$(SUCCESS)$@: done!$(RESET)"

kJSON_large.o: kJSON.c kJSON.h
	@echo "$(WARNING)Building object $@ $(RESET)"
	@$(CC) -o $@ -c $< $(CFLAGS) -DCONFIG_KJSON_SMALLEST=0
	@echo "$(SUCCESS)$@: done!$(RESET)"

test_small.bin: kJSON_small.o test.c
	@echo "$(WARNING)Building: $@ $(RESET)"
	@$(CC) -o $@ $^ $(CFLAGS) -DCONFIG_KJSON_SMALLEST=1
	@echo "$(SUCCESS)$@: done!$(RESET)"

test_large.bin: kJSON_large.o test.c
	@echo "$(WARNING)Building: $@ $(RESET)"
	@$(CC) -o $@ $^ $(CFLAGS) -DCONFIG_KJSON_SMALLEST=0
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
test: test_small.bin test_large.bin
	@chmod +x test_small.bin
	@chmod +x test_large.bin
	@./test_small.bin && echo "$(SUCCESS)Small config PASS!$(RESET)" || echo "$(ERROR)Small config FAIL!$(RESET)"
	@./test_large.bin && echo "$(SUCCESS)Large config PASS!$(RESET)" || echo "$(ERROR)Large config FAIL!$(RESET)"

.PHONY: clean
clean:
	rm -f *.o main
	rm -rf *.dSYM
	rm -rf *.bin
	@echo "Everything Clean!"

.PHONY: format
format:
	@clang-format --style=file -i *.c *.h
	@echo "$(JAZZ)Formatted!$(RESET)"


