INC := $(INC)
INC := $(addprefix -I,$(INC))
CFLAGS :=-g -O0 -Wall -Wextra -Wmissing-declarations -Wconversion -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -Wsuggest-attribute=const -Wunknown-warning-option -Wunused -Wuninitialized -Wno-unknown-warning-option $(INC)

