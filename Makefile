BINDIR=$(CURDIR)/bin
SYSTEMDIR=$(CURDIR)/include

EXEC=svtcp
SOURCE=			src/main.cc 		\
				    src/tcpsock.cc	\

CC=/usr/bin/gcc
CXX=/usr/bin/g++
CXXFLAGS=-std=c++14 -pthread -fconcepts

default: install run

run: 
	@$(BINDIR)/$(EXEC)

install:
	@mkdir -p $(BINDIR)
	@$(CXX) $(CXXFLAGS) -I $(SYSTEMDIR) -o $(BINDIR)/$(EXEC) $(SOURCE)

clean:
	@rm -rf $(BINDIR)
