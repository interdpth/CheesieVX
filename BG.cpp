#include "BG.h"


void BG::Export(MemFile* theFile)
{
	if (prop == 0x10)
	{
		int num = (int)(width * height * 2);
		MemFile* memoryFile = new MemFile(num);
		for (int i = 0; i < (int)height; i++)
		{
			for (int j = 0; j < (int)width; j++)
			{
				unsigned short num2 = blocks[j, i];
				memoryFile->fputc((byte)num2);
				memoryFile->fputc((byte)(num2 >> 8));
			}
		}

		int loopCounter = (((width * height) * 2) / 0x1800) + 1;

		int newWidth = width / loopCounter;
		int newHeight = height / loopCounter;
		for (int i = 0; i<loopCounter; i++)
		{
			unsigned char*  compBuffer = new unsigned char[64691];
			memset(compBuffer, 0, 64691);
			int count = newWidth*newHeight;
			unsigned short* block = new unsigned short[count];
			
			memcpy(block, &blocks[ loopCounter *count], count*2);

			int compLen = RomClass::compress(compBuffer, (unsigned char*)block, count*2);
			
			MemFile* thelayer = new MemFile (2 + (newWidth * newHeight) * 2);
			thelayer->seek(0);
			thelayer->fputc(newWidth);
			thelayer->fputc(newHeight);
			thelayer->fwrite(compBuffer, sizeof(char), compLen + 4, true);

			char fn[512] = { 0 };
			sprintf(fn, "bg%d_section_%d.rlebg", (int)layer,i);
			thelayer->Save(fn);
			delete thelayer;
			delete[] compBuffer;
			delete[] block;
		}

		//theFile->fputc(width);
		//theFile->fputc(height);
		//int compLen = RomClass::compress(compBuffer, (unsigned char*)&blocks.front(), (width * height) * 2);
		//
		//MemFile* thelayer = new MemFile (2 + (width * height) * 2);
		//thelayer->seek(0);

		////RomClass::UncompRle(3, unsigned char * arg1, unsigned char * arg2, compLen);

		//theFile->fwrite(compBuffer, sizeof(char), compLen+4, true);
		//thelayer->fputc(width);
		//thelayer->fputc(height);
		//thelayer->fwrite(compBuffer, sizeof(char), compLen + 4, true);

		//char fn[512] = { 0 };
		//sprintf(fn, "bg%d.rlebg", (int)layer);
		//thelayer->Save(fn);
	}
	/*else if (this.IsLZ77)
	{
		this.WriteLZ77(dst, false, false, true);
	}*/
}
BG::BG(int dwidth, int dheight)
{
	 width = dwidth;;

	height=dheight;

	blocks.resize(width*height);
}


BG::~BG()
{
}
