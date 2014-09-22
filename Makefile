CC = g++ -g

CC_DEBUG = @$(CC) -D_DEBUG
CC_RELEASE = @$(CC) -O3 -DNDEBUG

FRAMEWORK = -framework GLUT -framework OpenGL

all: debug full

debug : PA1.cpp
	@$(CC_DEBUG) -Wall PA1.cpp -o DEBUG -Wno-deprecated-declarations $(FRAMEWORK)

full : PA1.cpp
	@$(CC_RELEASE) -Wall PA1.cpp -o PA1 -Wno-deprecated-declarations $(FRAMEWORK)

clean:
	@rm -rf PA1 DEBUG *.dSYM

clear:
	@rm -rf PA1 DEBUG *.dSYM
	
.PHONY: all clean debug full