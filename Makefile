test: test.cpp
	g++ test.cpp  -Iinclude -Llib -lraylib -o test
	./test





clean: 
	rm *.o 
