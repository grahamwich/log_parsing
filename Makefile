CC = g++
CFLAGS = -g --std=c++14 -Wall -Werror -pedantic
LIB = -lboost_unit_test_framework -lboost_regex -lboost_date_time
DEPS = 

all: main lint

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $< $(LIB)

main: main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

lint: 
	cpplint --filter=-runtime/references,-build/c++11,-build/include_subdir, --root=. *.cpp

clean:
	rm *.o main
