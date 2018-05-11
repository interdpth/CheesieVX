#include "main.h"
#include <vector>
#include "SMClass.h"
#include "gbatypes.h"
using namespace std;
long RomClass::FindFreeSpace(long bNo, u8 bConst)
{ // bNo is how many bytes, bConst is what
	// value to look for 

	// returns offset of "free space" block
	// or -1 if no such block found


	if (!ROM)
	return -1;

	bNo += 0x100; // Look for an extra free 256 bytes. -- still not sure what
	// this does,
	// but whatever

	int             ch;
	long            runstart = -1;
	long            runcount = 0;

	fseek(ROM, 0, SEEK_SET);
checkagain:
	while (1)
	{
		ch = fgetc(ROM); // get character (byte)

		if (ch == EOF) // if end of ROM

		{
			runstart = -1; // eof reached before we found block

			break;

		}
		if ((u8) ch == bConst)
		{
			if (!runcount) // start of run

			runstart = (long) ftell(ROM) - 1;

			++runcount;
			if (runcount >= bNo) // block found

			break;
		} else
		{
			runstart = -1;
			runcount = 0;
		}

	}
	if (runstart == -1)
	{
		fseek(ROM, 0, SEEK_SET);

		goto checkagain;
	}
	if (runstart != 1)
	while (runstart % 4)
	runstart++;
	return runstart;
}

void RomClass::DecodePal(short *palGBA, COLOR* palPC, int numpals, char palpos)
{

	int             i, ii, s = 0;
	long            index = 0;

	for (ii = 0; ii < numpals; ii++)
	{
		index = 0x10 * ii;

		for (i = 0; i < 16; i++)
		{

			palPC[index + i].r = ((palGBA[index + i] & 0x1F) << 3);
			palPC[index + i].g = ((palGBA[index + i] >> 5) & 0x1F) << 3;
			palPC[index + i].b = (((palGBA[index + i] >> 10) & 0x1f) << 3);
			palPC[index + i].a=255;
		}
	}
	return;
}



int RomClass::DrawPic(wxMemoryDC* wxDC, u32 GFX, u32 PAL){
	GBAImage  img;
	GBATiles	tiles;

	int i=0;
	/*
u32 offset=0;
u8 tilebuffer[0x20000]={0};
u8 buffer[0x20000]={0};
short funkehpal[512]={0};
short gbapal[512]={0};
COLOR pcpal[512]={0};

//GET GFX
fseek(ROM,GFX,SEEK_SET);
fread(&offset,1,4,ROM);
if(offset-0x8000000 <0) return 0;
fseek(ROM,offset-0x8000000,SEEK_SET);
fread(buffer,1,0x20000,ROM);
curtrainergfxsz= LZ77UnComp(buffer, tilebuffer);
//GET PALETTE
fseek(ROM,PAL,SEEK_SET);
fread(&offset,1,4,ROM);
if(offset-0x8000000 <0) return 0;
fseek(ROM,offset-0x8000000,SEEK_SET);
fread(buffer,1,512,ROM);
curtrainerpalsz= LZ77UnComp((u8*)buffer, (u8*)gbapal);
//psize = LZ77UnComp(inbuffer, palbuff)



DecodePal(gbapal, pcpal, 16, 0);


tiles.Load(tilebuffer,64);
img.Create(64,64);
for( i= 0; i<64;i++)
	{
img.DrawTile(tiles[i],(i%8)*8,(i/8)*8,0,0);
}
img.SetPalette(pcpal,0,16); 
	img.BlitToDC(wxDC,0,0,64,64,0,0);
	*/
	return 0;
}
RomClass::RomClass(){

}
RomClass::~RomClass(){
}



void* RomClass::DecodeSNESPal(long offset, COLOR* dstPal, int numPals, char palpos, int size, vector<u8>* buffer){

	int             R, g, b, i, ii, s = 0;
	long            index = 0;
	u16             palGBA[512];

	//populate above array very shittly
	u32 red = 0x1F;
	u32 green = 0x1F << 5;
	u32 blue = 0x1F << 10;


	numPals=size/32;
	numPals-=palpos;
	//memcpy(&palGBA[palpos*0x10], buffer, size);
	int count;
	for(i=0;i<buffer->size()/2;i++){
		memcpy(&palGBA[i], &buffer->front()+(i*2),2);
	}


	for (ii = palpos; ii < numPals; ii++)
	{
		index = 0x10 * ii;

		for (i = 0; i < 16; i++)
		{
			
			dstPal[index + i].r =  ( ( ( palGBA[index + i] ) >>  0 ) << 3 ) & 0xF8;
			dstPal[index + i].g =  ( ( ( palGBA[index + i] ) >>  5 ) << 3 ) & 0xF8;
			dstPal[index + i].b =  ( ( ( palGBA[index + i] ) >> 10 ) << 3 ) & 0xF8;			
			dstPal[index + i].a=255;

			
			
		}
	}

	return 0;
}




