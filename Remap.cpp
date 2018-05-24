#include "SMClass.h"


void SMClass::Remap()
{
	int  Width = RoomHeader.Width * 16;
	int Height = RoomHeader.Height * 16;
	u16* TileBuf2D = &Map[1];
	int totalCount = gbaTroid.nTSA.size();
	int blockCount = totalCount / 4;
	unsigned short tsa[4];
	for (int thisY = 0; thisY < Height; thisY++) {

		for (int thisX = 0; thisX < (Width); thisX++) {// from here if something is enabled then draw it 
			unsigned short	hflip = 1;
			unsigned short		vflip = 1;
			unsigned short		curTile = TileBuf2D[thisX + (thisY * Width)];
			unsigned short		TILE = (curTile & 0x3ff);
			unsigned short		flip = (curTile & 0xC00) >> 8;
			unsigned short clipdata = curTile & 0x1000 >> 0xC;


			int keyIndex = 0xBAAD;
			for (auto &i : newTileTableMapping)
			{
				if (i.second == TILE) {
					keyIndex = i.first;
					break; // to stop searching
				}
			}

			if (keyIndex == 0xBAAD)
			{
				keyIndex = 0;
			}
			memcpy(&gbaTroid.nTSA[keyIndex * 4], tsa, 4 * 2);
			int tileCounter = 0;
			//Apply flip transformations
			int newTile=0;
			switch (flip) {

			case 0x4:
				for (int tileCounter = 0; tileCounter < 4; tileCounter++)
				{
					tsa[tileCounter] = ~0x400;
					gbaTroid.nTSA.push_back(tsa[tileCounter]);
				}
				newTile = (gbaTroid.nTSA.size() / 4);
				TileBuf2D[thisX + (thisY * Width)] = newTile;
				break;
			case 0x8:
				for (int tileCounter = 0; tileCounter < 4; tileCounter++)
				{
					tsa[tileCounter] = ~0x800;
					gbaTroid.nTSA.push_back(tsa[tileCounter]);
				}
				newTile = (gbaTroid.nTSA.size() / 4);
				TileBuf2D[thisX + (thisY * Width)] = newTile;
				break;
			case 0xC:
				for (int tileCounter = 0; tileCounter < 4; tileCounter++)
				{
					tsa[tileCounter] = ~0xC00;
					gbaTroid.nTSA.push_back(tsa[tileCounter]);
				}
				newTile = (gbaTroid.nTSA.size() / 4);
				TileBuf2D[thisX + (thisY * Width)] = newTile;
				break;
			case 0:

				break;
			}

			
		}
	}

}

