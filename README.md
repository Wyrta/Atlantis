# Atlantis project
An RPG pokemon like but with anime girls 

# Tile map sources
https://www.deviantart.com/chaoticcherrycake
- https://www.deviantart.com/chaoticcherrycake/art/Free-Tileset-v0-1-8-828715971

https://www.spriters-resource.com/game_boy_advance/pokemonemerald/

# TODO
- multiligns text 
- change map
- zoom (modify tile dim)
- dialog choice > "XX;A QUESTION [XY/YX/YY]" for each choice print dialog XY YX or YY (may have more than 3) 
- improve player movement : if already moving dont apply ignoreMove
- System class to store some info : performance graph (tick missed..), save/load function
- dynamic map loader (use <Tile>vector)
	tile ex : X,Y,NAME,DIR	=> 0,0,GRASS,ALL
	map  ex : TILE;TILE;    => 0,0,GRASS,ALL;0,1,GRASS,ALL;1,0,GRASS,ALL;1,1,GRASS,ALL;
							   