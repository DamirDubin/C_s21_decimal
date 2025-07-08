UNAME_S := $(shell uname -s)
TEST_LIBS = -lm -lpthread

ifeq ($(UNAME_S), Linux)
    TEST_LIBS += -lcheck_pic -lrt -lsubunit
endif

ifeq ($(UNAME_S), Darwin)
    TEST_LIBS += -lcheck
endif

CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11 -pedantic
LINK_FLAGS = --coverage
SRC = s21_decimal.c
OBJ = $(SRC:.c=.o)
OBJ_COV = $(SRC:.c=.cov.o)
TEST_SRC = test.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_OBJ_COV = $(TEST_SRC:.c=.cov.o)

.PHONY: all test run_test gcov_report clangn clangi cpp leaks valgrind clean rebuild

all: s21_decimal.a test

s21_decimal.a: $(OBJ)
	ar rcs $@ $^

s21_decimal_cov.a: $(OBJ_COV)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.cov.o: %.c
	$(CC) $(CFLAGS) $(LINK_FLAGS) -c $< -o $@

test: $(TEST_OBJ) s21_decimal.a
	$(CC) $(CFLAGS) $^ -o $@ $(TEST_LIBS)
	./test

run_test: $(TEST_OBJ_COV) s21_decimal_cov.a
	$(CC) $(CFLAGS) $^ -o $@ $(TEST_LIBS) $(LINK_FLAGS)
	./run_test

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