CC = g++
all:    project 

project: project.cpp calculator.o Poly.o Big_int.o calculator.h Poly.h Big_int.h
	$(CC) -o project project.cpp calculator.o Poly.o Big_int.o
	

calculator.o: calculator.cpp calculator.h Poly.o Poly.h 
	$(CC) -c calculator.cpp
	
Poly.o: Poly.cpp Big_int.o Poly.h Big_int.h
	$(CC) -c Poly.cpp
	
Big_int.o: Big_int.cpp Big_int.h
	$(CC) -c Big_int.cpp

clean:
	rm -f core *.o project 
