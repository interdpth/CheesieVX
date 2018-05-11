#pragma once
struct RHeader {
	unsigned char bTileset;
	unsigned char bBg0;
	unsigned char bBg1;
	unsigned char bBg2;
	unsigned long lBg3;
	unsigned long lForeground;
	unsigned long lLevelData;
	unsigned long lBackLayer;
	unsigned long lClipData;
	unsigned long lBackgroundTSA;
	unsigned char bUnknown1;
	unsigned char TransForeground;//Foreground Transparency
	unsigned short iSep1;
	unsigned long lSpritePointer;
	unsigned char bSpriteIndex1;
	unsigned char bEventSwitch;
	unsigned short iSep2;
	unsigned long lSpritePointer2;
	unsigned char bSpriteIndex2;
	unsigned char bEventSwitch2;
	unsigned short iSeperator;
	unsigned long lSpritePointer3;
	unsigned char bSpriteIndex3;
	unsigned char bMiniMapRoomX;
	unsigned char bMiniMapRoomY;
	unsigned char bEffect;
	unsigned char bSceneryYPos;
	unsigned char bNothing;
	unsigned char bMusic;
	unsigned char blank;
};

struct sTilesetBlockStructure {
	int max;
	unsigned short ID;
	unsigned short nTSA[0x1080];
};

struct gTileData 
{
	long gTiles;
	long pTiles;
	long gBackground;
	long TSAMap;
	unsigned char animTileset;
	unsigned char animPal;
	unsigned short filler;
};

