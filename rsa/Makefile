compiler = g++
flags = -g -Wall -std=c++11
compile = $(compiler) $(flags)

rsa: rsa.cpp
	$(compile) rsa.cpp -o rsa
.PHONY: clean
clean:
	rm -f *.o rsa
