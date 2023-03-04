CC=g++

RM=del

CFLAGS=-Wall -O

LDFLAGS=-I include -L lib

EXEC=game\Atlantis.exe

SRC=$(wildcard src/*.cpp)

METADAT=src/metadata/atlantis_metadata.res

OBJ=$(SRC:.cpp=.o)

$(info Src find : $(SRC))
$(info Obj find : $(OBJ))

LIB= -lmingw32 \
	 -static-libstdc++ \
	 -lSDL2main \
	 -lSDL2 \
	 -lSDL2_ttf \
	 -lSDL2_mixer \
	 -lSDL2_image



all: cleanAll $(EXEC) clean


$(EXEC): $(OBJ) $(METADAT)
	$(info Assemble the exe $@ ...)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIB)

%.o: %.cpp
	$(info Compilation of $@ ...)
	@$(CC) $(LIB) -o $@ -c $< $(CFLAGS) $(LIB)

%.res : %.rc
	$(info Compilation of $@ ...)
	@windres $< -O coff -o $@

clean:
	-$(RM) src\*.o

cleanAll:
	-$(RM) src\*.o
	-$(RM) $(EXEC)

#EoF