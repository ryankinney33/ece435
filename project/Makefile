# Directory Information
SRC_DIR := src
OBJ_DIR := build
INC_DIR := include

# source and object files
BIN := battleship
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Flags
CPPFLAGS := -I$(INC_DIR) -MMD -MP
CFLAGS := -Wall -g
LDLIBS := -lncurses

# Build rules
.PHONY: all clean
all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $@

clean:
	@$(RM) -rv $(BIN) $(OBJ_DIR)

-include $(OBJ:.o=.d)
