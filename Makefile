CC       := gcc
CFLAGS   := -Wall -Wextra -pedantic -std=c17 -g -O0 -Iinclude -MMD -MP
CFLAGS_T := -Wall -Wextra -pedantic -std=c17 -g -O0 -Iinclude -Itests -MMD -MP
AR       := ar
ARFLAGS  := rcs

SRC_DIR   := src
TEST_DIR  := tests
BUILD_DIR := build

LIB_SRC := $(SRC_DIR)/server.c $(SRC_DIR)/args_parse.c $(SRC_DIR)/http.c
LIB_OBJ := $(BUILD_DIR)/server.o $(BUILD_DIR)/args_parse.o $(BUILD_DIR)/http.o
LIB     := $(BUILD_DIR)/libserver.a

APP_SRC := $(SRC_DIR)/main.c
APP_OBJ := $(BUILD_DIR)/main.o
APP     := $(BUILD_DIR)/muscat

TEST_SRC := $(TEST_DIR)/test.c $(TEST_DIR)/test_server.c
TEST_OBJ := $(BUILD_DIR)/test.o $(BUILD_DIR)/test_server.o
TEST     := $(BUILD_DIR)/test

TEST_LIBS := -lcunit

.PHONY: all lib app test clean run

all: app

# --- Pattern rules ---
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS_T) -c $< -o $@

# --- Static lib ---
lib: $(LIB)
$(LIB): $(LIB_OBJ)
	$(AR) $(ARFLAGS) $@ $^

# --- App ---
app: $(APP)
$(APP): $(APP_OBJ) $(LIB)
	$(CC) $^ -o $@

# --- Tests ---
test: $(TEST)
	./$(TEST)
$(TEST): $(TEST_OBJ) $(LIB)
	$(CC) $^ $(TEST_LIBS) -o $@

# --- Run ---
run: app
	./$(APP)

# --- Build dir ---
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# --- Deps ---
-include $(wildcard $(BUILD_DIR)/*.d)

# --- Clean ---
clean:
	rm -rf $(BUILD_DIR)