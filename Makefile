IDIR = ./include
ODIR = ./obj
SDIR = ./src
LDIR = ./lib
CC = g++
FLAGS = -I$(SDIR) -I$(IDIR) -std=c++11 -Wall
#FLAGS = -I$(SDIR) -I$(IDIR) -std=c++11 -Wall -O3

SRC = $(wildcard $(SDIR)/*.cpp)
OBJS = $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(SRC)) $(LDIR)/libz3.so
DEPS = $(wildcard $(IDIR)/*.h)

#all: bin/eufi
#all: tests/cong_closure_expl_test
#all: tests/current_progress
#all: tests/parametric_example
all: tests/benchmarks
#all: valgrind_run

# --------------------------------
#  Build

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -g -c -o $@ $< $(FLAGS)

# -----------------------------------------------

#  Binaries

bin/eufiz3: $(OBJS) ./tests/eufiz3.cpp
	$(CC) -g -c -o ./tests/eufiz3.o ./tests/eufiz3.cpp \
		$(FLAGS) /home/jose/Documents/GithubProjects/z3-interp-plus/build/libz3.so
	$(CC) -g -o ./$@ $(OBJS) ./tests/eufiz3.o \
		$(FLAGS) -lpthread /home/jose/Documents/GithubProjects/z3-interp-plus/build/libz3.so
	rm -rf tests/*.o

bin/eufi: $(OBJS) ./tests/eufi.cpp
	$(CC) -g -c -o ./tests/eufi.o ./tests/eufi.cpp \
		$(FLAGS)
	$(CC) -g -o ./$@ $(OBJS) ./tests/eufi.o \
		$(FLAGS) -lpthread
	rm -rf tests/*.o

# -----------------------------------------------
# Tests

.PHONY: tester
tester: $(OBJS)
	$(CC) -g -c -o ./$(param1).o ./$(param1).cpp \
		$(FLAGS) 
	$(CC) -g -o $(param1) $(OBJS) ./$(param1).o \
		-lpthread $(FLAGS)
	./$(param1)
	rm -rf tests/*.o $(param1)

.PHONY: tester_with_threads
tester_with_threads: $(OBJS)
	$(CC) -g -o $(param1) ./$(param1).cpp \
		$(LDIR)/libz3.so \
		-lpthread -pthread $(FLAGS) 
	./$(param1)
	rm -rf $(param1)

.PHONY: tests/current_progress
tests/current_progress: $(OBJS)
	make tester param1=$@

.PHONY: tests/parametric_example
tests/parametric_example:
	make tester_with_threads param1=$@

.PHONY: tests/benchmarks
tests/benchmarks:
	make tester_with_threads param1=$@

.PHONY: valgrind_run
valgrind_run: $(OBJS)
	$(CC) -g -c -o ./tests/current_progress.o ./tests/current_progress.cpp \
		$(FLAGS)
	$(CC) -g -o tests/current_progress $(OBJS) ./tests/current_progress.o \
		-lpthread $(FLAGS)
	valgrind ./tests/current_progress
	rm -rf tests/*.o tests/current_progress

.PHONY: valgrind_output
valgrind_output: $(OBJS)
	$(CC) -g -c -o ./tests/current_progress.o ./tests/current_progress.cpp \
		$(FLAGS)
	$(CC) -g -o tests/current_progress $(OBJS) ./tests/current_progress.o \
		-lpthread $(FLAGS)
	valgrind --log-file="output.txt" ./tests/current_progress
	rm -rf tests/*.o tests/current_progress

.PHONY: tests/cong_closure_expl_test
tests/cong_closure_expl_test: $(OBJS)
	$(CC) -g -c -o tests/TestCongruenceClosureExplain.o \
		tests/TestCongruenceClosureExplain.cpp \
		$(FLAGS) 
	$(CC) -g -c -o ./$@.o ./$@.cpp $(FLAGS)
	$(CC) -g -o $@ $(OBJS) tests/TestCongruenceClosureExplain.o ./$@.o \
		-lpthread $(FLAGS)
	./$@
	rm -rf tests/*.o $@

.PHONY: tests/qf_uf_processing_test
tests/qf_uf_test: $(OBJS)
	$(CC) -g -c -o tests/TestCongruenceClosureExplain.o \
		tests/TestCongruenceClosureExplain.cpp \
		$(FLAGS)
	$(CC) -g -c -o ./$@.o ./$@.cpp $(FLAGS)
	$(CC) -g -o $@ $(OBJS) tests/TestCongruenceClosureExplain.o \
		./$@.o -lpthread $(FLAGS)
	./$@.sh
	rm -rf tests/*.o $@

# --------------------------------------------------------------------------------

notes:
	grep -nr "// [A-Z][A-Za-z0-9]*: "

.PHONY: clean
clean:
	rm -rf $(ODIR)/* 
