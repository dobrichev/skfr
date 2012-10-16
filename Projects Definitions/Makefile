CXX = g++ 
CXXFLAGS = -fopenmp -O3 -march=native

LDFLAGS = -lstdc++ -lgomp

skfr: utilities.o opsudo.o puzzle.o fsss.o t_128.o flog.o skfr.o ratingengine.o bitfields.o

clean:
	rm -f *.o
