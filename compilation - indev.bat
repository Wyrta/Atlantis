@echo off

@echo Compilation en cours...

g++ src/*.cpp -o game/Atlantis.exe -I include -L lib -lmingw32 -static-libstdc++ -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image

PAUSE