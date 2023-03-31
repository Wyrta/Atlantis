CC=g++
RM=del
ZIP=7z


CFLAGS=-Wall -O

LDFLAGS=-I include -L lib lib/libstdc++.a

EXEC=game\Atlantis.exe
VER=23w13

SRC=$(wildcard src/*.cpp)

METADAT=src/metadata/atlantis_metadata.res

OBJ=$(SRC:.cpp=.o)

$(info Src find : $(SRC))
$(info Obj find : $(OBJ))

LIB= -lmingw32 \
	 -lSDL2main \
	 -lSDL2 \
	 -lSDL2_ttf \
	 -lSDL2_mixer \
	 -lSDL2_image


all: cleanAll $(EXEC) clean

zip: $(EXEC) clean
	-$(RM) $(VER).zip
	$(ZIP) a -tzip $(VER).zip game/


$(EXEC): $(OBJ) $(METADAT)
	$(info Assemble $@ ...)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIB)

%.o: %.cpp
	$(info $@ ...)
	@$(CC) -o $@ -c $< $(CFLAGS) $(LIB)

%.res : %.rc
	$(info $@ ...)
	@windres $< -O coff -o $@

clean:
	-$(RM) src\*.o

cleanAll:
	-$(RM) src\*.o
	-$(RM) $(EXEC)

#EoF