CC = g++

PROG = prog


SRCS = main.cpp

CFLAGS = -Wall
CFLAGS += `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`
LIBS += -lGL -lGLU -lglut -lXmu -lXi -lstdc++ -lm

all: $(PROG)

$(PROG):	$(SRCS)
		$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
