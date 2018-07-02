#include "gbaimage.h"
#include "gbatiles.h"
#include "gbatypes.h"
#include <wx/dcmemory.h>
#include <vector>
using namespace std;
#ifndef game_imgage_h
#define game_imgage_h

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned long
enum IMAGEID {
	MAP,
	TILESET
};
class gameImages{//Store buffers and shit in here. 
public:
	GBAImage img;
	wxMemoryDC* mDC;
	vector<u8> tiles;
	vector<COLOR>palette;
	vector<u16> TSA;

	void Set(vector<u8>* buffer);
	void Set(vector<COLOR>* buffer);
	void Set(vector<u16>* buffer);
	void MakeImage(GBAImage* theimage,vector<u16>* TileIDs,int x,int y,int width, int height);// <--Compile the above vars into a bitmap, can be used to draw a single block
	gameImages(wxMemoryDC* pnt);
};



class clsEditing{//Tmp name
public:
	gameImages img;
	//RightClick();
	//LeftClick();
	//MouseMove();
protected:
	int isDown;
	int isUp;

};
#endif