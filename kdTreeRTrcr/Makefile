CC = g++ -g

CC_DEBUG = @$(CC) -D_DEBUG
CC_RELEASE = @$(CC) -O3 -DNDEBUG

FRAMEWORK = -framework GLUT -framework OpenGL

all: debug full

debug : PA4.cpp
	@$(CC_DEBUG) -Wall PA4.cpp -o DEBUG -Wno-deprecated-declarations $(FRAMEWORK)

full : PA4.cpp
	@$(CC_RELEASE) -Wall PA4.cpp -o PA4 -Wno-deprecated-declarations $(FRAMEWORK)

clean:
	@rm -rf PA4 DEBUG *.dSYM

clear:
	@rm -rf PA4 DEBUG *.dSYM
	
.PHONY: all clean debug full