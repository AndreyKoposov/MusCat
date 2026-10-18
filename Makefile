# --- Compilers and flags ---
СС		:= gcc
CFLAGS	:= -Wall -Wextra -pedantic -std=c17 -g -O0 -Iinclude
AR		:= ar
ARFLAGS := rcs

# --- Directories ---
SRC_DIR	  := src
TEST_DIR  := tests
BUILD_DIR := build

# --- Files ---
LIB_SRC := ${SRC_DIR}/server/core.c
LIB_OBJ := ${BUILD_DIR}/core.o
LIB     := $(BUILD_DIR)/libserver.a

APP_SRC	:= $(SRC_DIR)/main.c
APP_OBJ	:= $(BUILD_DIR)/main.o
APP 	:= $(BUILD_DIR)/muscat

TEST_SRC := $(SRC_DIR)/test.c
TEST_OBJ := $(BUILD_DIR)/test.o
TEST 	 := $(BUILD_DIR)/test

TEST_LIBS := -lcunit

# --- Targets ---

.PHONY: all lib app test clean run

all: app

# --- Static lib ---
lib: $(LIB)

$(LIB): $(LIB_OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(LIB_OBJ): $(LIB_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Main app ---
app: $(APP)

$(APP): $(APP_OBJ) $(LIB)
	$(CC) $^ -o $@

$(APP_OBJ): $(APP_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Tests ---
test: $(TEST)
	./$(TEST)

$(TEST): $(TEST_OBJ) $(LIB)
	$(CC) $^ $(TEST_LIBS) -o $@

$(TEST_OBJ): $(TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Create build dir ---
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --- Run app ---
run: app
	./$(APP)

# --- Clear ---
clean:
	rm -rf $(BUILD_DIR)