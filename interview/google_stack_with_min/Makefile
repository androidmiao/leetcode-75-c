# Makefile — google_stack_with_min
#
# 主要目標：
#   make test      對 solution.c 跑 tests.c（ASan + UBSan）
#   make demo      對 original.c 跑 bug_demo.c 的所有示範
#   make warnings  顯示 original.c vs solution.c 的編譯器警告
#   make analyze   gcc -fanalyzer 靜態分析
#   make all       全部跑一遍
#   make clean

CC      = gcc
CSTD    = -std=c11
WARN    = -Wall -Wextra -Wpedantic -Wuninitialized
SAN     = -fsanitize=address,undefined -fno-omit-frame-pointer
DBG     = -g
OPT_W   = -O2

BIN     = build
SOL_LIB = -DSOLUTION_LIB

.PHONY: all test demo warnings analyze clean

all: test demo warnings analyze

$(BIN):
	mkdir -p $(BIN)

# --- 1. 測試（對 solution.c）-----------------------------------------------
test: $(BIN)/tests
	@echo "=== Run tests against solution.c (ASan + UBSan) ==="
	@./$(BIN)/tests

$(BIN)/tests: tests.c solution.c | $(BIN)
	$(CC) $(CSTD) $(WARN) $(SAN) $(DBG) $(SOL_LIB) $^ -o $@

# --- 2. Bug 示範（對 original.c）-------------------------------------------
demo: $(BIN)/bug_demo
	@echo "=== Demonstrate bugs in original.c ==="
	@./$(BIN)/bug_demo all

$(BIN)/bug_demo: bug_demo.c original.c | $(BIN)
	$(CC) $(CSTD) $(WARN) $(SAN) $(DBG) $^ -o $@

# --- 3. 編譯器警告：original 與 solution 對照 ------------------------------
warnings:
	@echo "=== Warnings on original.c (-Wall -Wextra -Wuninitialized -O2) ==="
	-@$(CC) $(CSTD) $(WARN) $(OPT_W) -c original.c -o /dev/null 2>&1 || true
	@echo
	@echo "=== Warnings on solution.c (should be clean) ==="
	-@$(CC) $(CSTD) $(WARN) $(OPT_W) $(SOL_LIB) -c solution.c -o /dev/null 2>&1 || true
	@echo "(no output above means no warnings)"

# --- 4. 靜態分析 -----------------------------------------------------------
analyze:
	@echo "=== gcc -fanalyzer on original.c ==="
	-@$(CC) $(CSTD) -fanalyzer -c original.c -o /dev/null 2>&1 || true
	@echo
	@echo "=== gcc -fanalyzer on solution.c ==="
	-@$(CC) $(CSTD) -fanalyzer $(SOL_LIB) -c solution.c -o /dev/null 2>&1 || true

clean:
	rm -rf $(BIN)
