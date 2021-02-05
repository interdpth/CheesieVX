#pragma once
#include <vector>
#include "RomClass.h"
#include "Compresch/CompreschMain.h"
#include "MemFile.h"
#include "Backgrounds.h"
#include "Logger.h"
#include <map>
using namespace std;
#define LC_NOBANK  0x00
#define LC_LOROM   0x01  //LoROM
#define LC_HIROM   0x02  //HiROM
#define LC_EXHIROM 0x04  //Extended HiROM
#define LC_EXLOROM 0x08  //Extended LoROM
#define LC_LOROM_2 0x10  //LoROM, always converts to 80:8000 map
#define LC_HIROM_2 0x20  //HiROM, always converts to 40:0000 map up till 70:0000
#define LC_EXROM   0x04  //same as LC_EXHIROM (depreciated)

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned long
#ifndef MDBSTUFF
#define MDBSTUFF
class block
{
public:
	unsigned short map1, map2, map3, map4;
	block(unsigned short a, unsigned short b, unsigned short c, unsigned short d);
};
struct MDB_Header{
	unsigned char  unknown1;
	unsigned char  Area;
	unsigned char  Xpos;
	unsigned char  Ypos;
	unsigned char  Width;
	unsigned char  Height;
	unsigned char  Unknown1;
	unsigned char  Unknown2;
	unsigned char  Unknown3;
	unsigned short Pdoorout;
};
#define u8 unsigned char
struct MDB_Roomstate{
	u32 Roommap_p;
	u8 GraphicsSet;
	u8 MusicTrack;
	u8 MusicControl;
	u16 Fx1_p;
	u16 Roompop_p; //Room population
	u16 Enemyset_p;
	u16 Layer2_Scr;
	u16 Mdb_Scroll_p;
	u16 Unknown;
	u16 Fx2_p;
	u16 Plm_p;
	u16 Bgdata_p; //Background data
	u16 LayersHandling;
};
struct tTSA {
	int max;
	unsigned short ID;
	std::vector<unsigned short> nTSA;
};

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned long
#include "GBA.h"
class SMClass{
public:
	int ExportTilemap();
	std::vector<u16> Map; 
	char exporthPath[1024];
	int GrabRoom();
	unsigned long _currentAddress;
	vector<unsigned char> specialbggfx;
	//Vars
	int MageExport(int Area, int Room, bool IsMf);
	void SMBG2GBATroid(vector<u16>* buffer);
	int iRoomState; //Current room state
	std::vector<MDB_Roomstate> RoomStates;
	std::vector<u32> RoomStatePointers;
	long SMDecomp(u32 offset,vector<u8>* buffer);
	//long SMDecomp(u32 offset);
	RomClass System;
	CompreschMain Compression;
	unsigned short snes2gba_tilemap(uint16_t tile);
	bool CompareTiles(unsigned char* srcTile, unsigned char* chkTile);
	unsigned short TileExists(unsigned char* srcTile, vector<unsigned char*>*theTiles);
	void UpdateTileTable(unsigned short oldMapping, unsigned short newMapping);
	u32 BytesToOff(char* bytes);
	//Functions
	int QuantifyTable();
	int RemapRoomtiles();
	SMClass(char* FilePath);
	int BlitToBB();
	int Setup();
	int LoadHeader(u32 Address);
	int DrawRoom(wxMemoryDC* dst, wxMemoryDC* src, BG* bg,wxRasterOperationMode rop=wxCOPY);
	int LoadMDB_StateSelect(u32 Address);
	MDB_Header RoomHeader;
	int LoadMDB_Roomstate(u32 Address, MDB_Roomstate* OutputMDB_Roomstate);
	~SMClass();
	int SNES2GBA();
	int gfxSet;
	int GrabTileset(int GraphicsSet);
	u32 ReadPointer(u32 offset);
	std::vector<u8> Tiles;
	std::vector<COLOR> Pal;
	int SNES2GBA(u8* pnt);
	tTSA TSA;
	u32 Pnt2Off(u32 pointer);
	int FindHeaders();
	void GrabBG(vector<u8>* buffer);

	//EXPORTING OCE
	bool hasBackground = false;
	bool BGISBG3 = false;
	void AddGBAPadding(unsigned char width, unsigned char height, vector<unsigned short>* buff);
	int ExportTileset();
	int ExportTileTable();
	int ReduceToVisibleTiles();
	int MappingExists(unsigned short* tiles, vector<unsigned short*>*newMap);
	bool CompareTileTables(unsigned short* tiles, unsigned short* tiles2);
	map<int, int> newTileTableMappingIndexes;
	vector<unsigned short*> newMapData;
	tTSA gbaTroid;
	map<int, int> newTileMappingIndexes;
	void Remap(BG* background);
	void QuantifyMapTiles(BG* background);
	Backgrounds* theBgs;
};


#endif 

