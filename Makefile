CXX=g++
CXXFLAGS=-g -DDEBUG

all: dbms

dbms: main.cpp
	$(CXX) main.cpp $(CXXFLAGS) -o dbms

clean:
	rm -f dbms log.txt