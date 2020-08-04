CXX = g++
FLAGS = -Wall -g

main: main.o 
	$(CXX) $(FLAGS) -o main main.o
main.o: main.cpp
	$(CXX) $(FLAGS) -c main.cpp
clean:
	rm -f *.o *.gch main