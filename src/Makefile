UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
    CFLAGS += -D__linux__ -Wall -Werror -Wextra -std=c11 -g
    TEST_LIBS = -lcheck_pic -lm -lpthread -lrt -lsubunit
endif

ifeq ($(UNAME_S), Darwin)
    CFLAGS += -D__APPLE__ -Wall -Werror -Wextra -std=c11 -g
    TEST_LIBS = -lcheck -lm -lpthread
endif


CC = gcc
LINK_FLAGS = --coverage
SRC = s21_decimal.c
OBJ = $(SRC:.c=.o)
OBJ_COV = $(SRC:.c=.cov.o)
TEST_SRC = test.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_OBJ_COV = $(TEST_SRC:.c=.cov.o)

.PHONY: all test run_test gcov_report clangn clangi cpp leaks valgrind clean rebuild

all: s21_decimal.a

s21_decimal.a: $(OBJ)
	ar rcs $@ $^
	ranlib $@

s21_decimal_cov.a: $(OBJ_COV)
	ar rcs $@ $^
	ranlib $@

%.o: %.c
	$(CC) $(CFLAGS) -g -c $< -o $@

%.cov.o: %.c
	$(CC) $(CFLAGS) $(LINK_FLAGS) -c $< -o $@

test: $(TEST_OBJ) s21_decimal.a
	$(CC) $(CFLAGS) $^ -o $@ $(TEST_LIBS)
	./test

run_test: $(TEST_OBJ_COV) s21_decimal_cov.a
	$(CC) $(CFLAGS) $^ -o $@ $(TEST_LIBS) $(LINK_FLAGS)
	./run_test

debug_test:
	$(CC) $(CFLAGS) -g -O0 $(SRC) test_debug.c -o test $(TEST_LIBS)

gcov_report: run_test
	lcov --capture --directory . --output-file coverage.info --ignore-errors source
	genhtml coverage.info --output-directory report
	open report/index.html || xdg-open report/index.html

clangn:
	clang-format -n *.h *.c

clangi:
	clang-format -i *.h *.c

cpp:
	cppcheck --enable=all --suppress=missingIncludeSystem s21_decimal.c test.c

leaks: 
	leaks -atExit -- ./test

valgrind: test
	valgrind --trace-children=yes --track-origins=yes --track-fds=yes --tool=memcheck \
	--leak-check=full --show-leak-kinds=all --log-file="./report.log" ./test

clean:
	rm -f *.o *.a *.gcno *.gcda *.info *.log test run_test
	rm -rf report

rebuild: clean all