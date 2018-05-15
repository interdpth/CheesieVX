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
long            RomClass::UncompRle(unsigned short BufferSizeSwitch, unsigned char * arg1, unsigned char * arg2, long* compsize)
{

	unsigned char             *src = (unsigned char *)arg1, *dst = (unsigned char *)arg2;

	unsigned long             jj, ii, nn;
	unsigned long             ctrl, type;

	type = 0;
	nn = 0x3000;

	switch (BufferSizeSwitch)
	{
	case 0:
		type = 0x0800;
		break;
	case 3:
		type = 0x2000;
		break;
	default:
		type = 0x1000;
	}

	nn = 0x2000;

	// DMAFill(3, 0, arg2, nn, 16);

	for (ii = 0; ii < 2; ii++)
	{
		dst = (unsigned char *)& arg2[ii];
		if (*src++ == 1) // 8 control field

		{
			while (1)
			{
				ctrl = *src++;
				if (ctrl == 0)
					break;

				else if (ctrl & 0x80)
				{
					nn = ctrl & 0x7F;
					if (*src)
						for (jj = 0; jj < nn; jj++)
							dst[2 * jj] = *src;
					src++;
				}
				else
				{
					nn = ctrl;
					for (jj = 0; jj < nn; jj++)
						dst[2 * jj] = src[jj];
					src += nn;
				}

				dst += 2 * nn;
			}
		}
		else // RLE16

		{
			while (1)
			{
				ctrl = src[0] << 8 | src[1];
				src += 2;
				if (ctrl == 0)
					break;

				if (ctrl & 0x8000)
				{
					nn = ctrl & 0x7FFF;
					if (*src)
						for (jj = 0; jj < nn; jj++)
							dst[2 * jj] = *src;
					src++;
				}
				else
				{
					nn = ctrl;
					for (jj = 0; jj < nn; jj++)
						dst[2 * jj] = src[jj];
					src += nn;
				}
				dst += 2 * nn;
			}
		}
	}
	*compsize = src - arg1;
	return type;
}


long           RomClass::compress(unsigned char cmp[], unsigned char uncmp[], unsigned long sze)
{
	// int sze = 3;

	unsigned long             cmpptr = 0;
	unsigned long             sizeptr = 0;
	unsigned long             dataptr = 0;
	unsigned long             rl = 0;

	unsigned long             samebytecount = 0;

	for (int j = 0; j < 2; j++)
	{
		cmp[cmpptr++] = 1;
		while (sze > dataptr)
		{
			rl = 2;
			while (uncmp[dataptr] == uncmp[dataptr + 2] && uncmp[dataptr + 2] == uncmp[dataptr + 4] && rl < 0x7F && sze > dataptr)
			{

				dataptr += 2;
				rl++;
			}

			if (rl > 2)
			{
				// printf("run found s=%02X\n", rl);
				cmp[cmpptr++] = (unsigned char)((rl | 0x80) & 0xFF);
				cmp[cmpptr++] = uncmp[dataptr];
				dataptr += 4;
			}
			sizeptr = cmpptr;
			cmpptr++;

			rl = 0;
			while ((uncmp[dataptr] != uncmp[dataptr + 2] || uncmp[dataptr + 2] != uncmp[dataptr + 4]) && rl < 0x7F && sze > dataptr)
			{

				cmp[cmpptr++] = uncmp[dataptr];
				dataptr += 2;
				rl++;
			}
			// printf("norun found s=%02X\n", rl);
			if (rl != 0)
				cmp[sizeptr] = (unsigned char)rl;
			else
				cmpptr--;
		}
		cmp[cmpptr++] = 0;

		dataptr = 1;
	}

	return cmpptr;

}




