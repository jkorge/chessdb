SRCDIR = ./src
INCDIR = $(SRCDIR)/include
LIBDIR = ./lib
BINDIR = ./bin
OBJDIR = ./obj

SRCCHESS = $(filter-out $(SRCDIR)/parsestream.cpp $(SRCDIR)/engine.cpp, $(wildcard $(SRCDIR)/*.cpp))
OBJSCHESS = $(SRCCHESS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
LIBCHESS = $(LIBDIR)/libchess.a

SRCPARSE = $(SRCDIR)/parsestream.cpp
OBJSPARSE = $(SRCPARSE:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
LIBPARSE = $(LIBDIR)/libparsestream.a

SRCENGINE = $(SRCDIR)/engine.cpp
HDRENGINE = $(INCDIR)/engine.hpp
ifeq ($(OS),Windows_NT)
	ENGINE = $(BINDIR)/engine.exe
else
	ENGINE = $(BINDIR)/engine
endif

CXX = g++
CXXFLAGS = -std=c++17 -O3 -Winline -mlzcnt -mbmi -mpopcnt -march=x86-64 -flto -fno-fat-lto-objects
LDFLAGS = -Llib -lchess -lparsestream

$(ENGINE): $(HDRENGINE) $(SRCENGINE) $(LIBPARSE) $(LIBCHESS) | $(BINDIR)
ifeq ($(OS), Windows_NT)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCENGINE) icon.res $(LDFLAGS)
else
	$(CXX) $(CXXFLAGS) -o $@ $(SRCENGINE) $(LDFLAGS) -lpthread
endif

$(OBJDIR):
	mkdir $@

$(LIBDIR):
	mkdir $@

$(BINDIR):
	mkdir $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCDIR)/%.hpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

obj/chessdb.o: obj/chess.o $(LIBPARSE)

$(LIBPARSE): $(OBJSPARSE) | $(LIBDIR)
	ar crs $(LIBPARSE) $(OBJSPARSE)

$(LIBCHESS): $(OBJSCHESS) | $(LIBDIR)
	ar crs $(LIBCHESS) $(OBJSCHESS)

clean:
ifeq ($(OS),Windows_NT)
	-del /q obj\*.o
	-del /q lib\*.a
	-del /q bin\*.exe
else
	-rm -f obj/*.o
	-rm -f bin/*
	-rm -f lib/*.a
endif

parse: $(LIBPARSE)

chess: $(LIBCHESS)

engine: $(INCDIR)/table.hpp $(ENGINE)

.PHONY: parse chess engine