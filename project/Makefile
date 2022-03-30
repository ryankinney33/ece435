# Directory Information
SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin
INC_DIR := include
BIN := chess

# source and object files
TARGET := $(BIN_DIR)/$(BIN)
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Flags
CPPFLAGS := -I$(INC_DIR) -MMD -MP
CFLAGS := -Wall -g
LDLIBS := -lncurses

# Build rules
.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDLIBS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) | $(OBJ_DIR):
	@mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)
