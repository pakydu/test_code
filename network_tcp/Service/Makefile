PROJECT_DIR = $(pwd)
PROJECT_TARGS = NetworkService
PROJECT_INCS =

PROJECT_INC_H = $(shell find $(PROJECT_DIR) -name '*.h')

PROJECT_INC_PATH = $(dir $(PROJECT_INC_H))


PROJECT_INCS += $(foreach tmp, $(PROJECT_INC_PATH), -I$(tmp))
CFLAGS += $(PROJECT_INCS)

PROJECT_SRC = $(shell find $(PROJECT_DIR) -name '*.c')

OBJ := $(patsubst %.c, %.o, $(PROJECT_SRC))


.PHONY: clean

$(PROJECT_TARGS): $(OBJ)
	echo "Building targs - $@"
	cc -o $@ $^

%.o:%.c
	echo "CC [C] - $@"
	cc -c $(CFLAGS) -o $@ $<

clean:
	echo "clean project $(PROJECT_TARGS)"
	rm -f $(PROJECT_TARGS) $(OBJ)
