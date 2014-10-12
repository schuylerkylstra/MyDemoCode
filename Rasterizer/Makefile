CC = g++ -g

CC_DEBUG = @$(CC) -D_DEBUG
CC_RELEASE = @$(CC) -DNDEBUG

FRAMEWORK = -framework GLUT -framework OpenGL

all: debug full

debug : PA2.cpp
	@$(CC_DEBUG) -Wall PA2.cpp -o DEBUG -Wno-deprecated-declarations $(FRAMEWORK)

full : PA2.cpp
	@$(CC_RELEASE) -Wall PA2.cpp -o PA2 -Wno-deprecated-declarations $(FRAMEWORK)

clean:
	@rm -rf PA2 DEBUG *.dSYM

clear:
	@rm -rf PA2 DEBUG *.dSYM
	
.PHONY: all clean debug full