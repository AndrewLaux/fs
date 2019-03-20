

test:
	g++ -g -c fs.cpp disk.c
	g++ -g -o test.exe test.cpp fs.o disk.o
	g++ -g -o a offset_test.c fs.o disk.o

fs.o:
	g++ -c fs.cpp disk.c
	
clean:
	rm -f *.o *.exe *.drive