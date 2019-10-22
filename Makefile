CC = gcc
CFLAGS = -c -Wall
PROFILE_FLAGS = -fprofile-arcs -ftest-coverage
TST_LIBS = -lcheck -lm -lpthread -lrt -lsubunit
COV_LIBS = -lgcov -coverage
SRC_DIR = src
TST_DIR = .tests
INC_DIR = src
SRC_FILES = $(addprefix $(SRC_DIR)/, *.c)
TST_FILES = $(addprefix $(TST_DIR)/, *.c)
GCOV = gcovr
GCONV_FLAGS = -r . --html --html-details

all: coverage_report.html

memory_management.o:  $(SRC_FILES) $(addprefix $(SRC_DIR)/, memory_management.h)
	$(CC) $(CFLAGS) $(PROFILE_FLAGS) $(SRC_FILES)

check_memory.o: $(TST_FILES)
	$(CC) $(CFLAGS) $(PROFILE_FLAGS)  $(TST_FILES) -I$(INC_DIR)
memory_tests: memory_management.o check_memory.o
	$(CC) memory_management.o check_memory.o $(TST_LIBS) $(COV_LIBS)  -o memory_tests

test: memory_tests
	./memory_tests

zip: clean
	zip -r comp2211_cw1.zip ./

coverage_report.html: test
	$(GCOV) $(GCONV_FLAGS) -o coverage_report.html

.PHONY: clean all

clean:
	-rm *.o *.html *.gcda *.gcno *.log *.zip memory_tests 2> /dev/null
