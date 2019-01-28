CFLAGS=-std=gnu++98 -Wall -Werror -pedantic
all: myShell testArgu testError Repeat
.PHONY: clean

myShell: StringMath.o MyVar.o Command.o myShell.o
	g++ -o myShell StringMath.o MyVar.o Command.o myShell.o
StringMath.o: StringMath.cpp StringMath.h
	g++ $(CFLAGS) -c StringMath.cpp
MyVar.o: MyVar.cpp MyVar.h
	g++ $(CFLAGS) -c MyVar.cpp
Command.o: Command.cpp Command.h MyVar.h
	g++ $(CFLAGS) -c Command.cpp
myShell.o: myShell.cpp myShell.h StringMath.h MyVar.h Command.h
	g++ $(CFLAGS) -c myShell.cpp

testArgu: testArgu.cpp
	g++ $(CFLAGS) -o testArgu testArgu.cpp

testError: testError.cpp
	g++ $(CFLAGS) -o testError testError.cpp

Repeat: Repeat.cpp
	g++ $(CFLAGS) -o Repeat Repeat.cpp
clean:
	rm *.o
