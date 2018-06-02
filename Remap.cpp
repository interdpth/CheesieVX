#include "SMClass.h"
#include "BG.h"

void SMClass::Remap(BG* background)
{
	int  Width = RoomHeader.Width * 16;
	int Height = RoomHeader.Height * 16;
	u16* TileBuf2D = &background->blocks.front();
	int totalCount = gbaTroid.nTSA.size();
	int blockCount = totalCount / 4;
	unsigned short tsa[4];
	int mappingId = 0;
	//
	for (int thisY = 0; thisY < Height; thisY++) {

		for (int thisX = 0; thisX < (Width); thisX++) {// from here if something is enabled then draw it 
			unsigned short	hflip = 1;
			unsigned short		vflip = 1;
			unsigned short		curTile = TileBuf2D[thisX + (thisY * Width)];
			unsigned short		TILE = (curTile & 0x3ff);
			unsigned short		flip = (curTile & 0xC00) >> 8;
			unsigned short clipdata = curTile & 0x1000 >> 0xC;

			
			int keyIndex = 0xBAAD;
			for (auto &i : newTileTableMappingIndexes)
			{
				if (i.first == TILE) {
					keyIndex = i.second;
					break; // to stop searching
				}
			}

			if (keyIndex == 0xBAAD)
			{
				keyIndex = 0;
			}
			if (TILE == 0xFF)
			{
				keyIndex = 0x4F;
			}
			background->blocks[thisX + (thisY * Width)] = keyIndex;
		
			memcpy(tsa, &gbaTroid.nTSA[keyIndex*4], 4 * 2);
			int tileCounter = 0;
			//Apply flip transformations
			int newTile=0;
			unsigned short tmpMappings[4]={0};
			switch (flip) {

			case 0x4:
				for (int tileCounter = 0; tileCounter < 4; tileCounter++)
				{
					tsa[tileCounter] ^= 0x400;					
				}

				tmpMappings[0] = tsa[tileCounter + 1];
				tmpMappings[1] = tsa[tileCounter];		
				tmpMappings[2] = tsa[tileCounter + 3];
				tmpMappings[3] = tsa[tileCounter + 2];
				
				mappingId = MappingExists(tmpMappings, &newMapData);
				if (mappingId == 0xbaad)
				{
					for (int tileCounter = 0; tileCounter < 4; tileCounter++)
					{
						gbaTroid.nTSA.push_back(tmpMappings[tileCounter]);
					}
					mappingId = (gbaTroid.nTSA.size() / 4)-1;
				}
				
				background->blocks[thisX + (thisY * Width)] = mappingId;
				break;
			case 0x8:
				for (int tileCounter = 0; tileCounter < 4; tileCounter++)
				{
					tsa[tileCounter] ^= 0x800;					
				}
				
				tmpMappings[0] = tsa[tileCounter + 2];
				tmpMappings[1] = tsa[tileCounter+3];
				tmpMappings[2] = tsa[tileCounter ];
				tmpMappings[3] = tsa[tileCounter + 1];
				mappingId = MappingExists(tmpMappings, &newMapData);
				if (mappingId == 0xbaad)
				{
					for (int tileCounter = 0; tileCounter < 4; tileCounter++)
					{
						gbaTroid.nTSA.push_back(tmpMappings[tileCounter]);
					}
					mappingId = (gbaTroid.nTSA.size() / 4) - 1;
				}

				background->blocks[thisX + (thisY * Width)] = mappingId;
				break;
			case 0xC:
				for (int tileCounter = 0; tileCounter < 4; tileCounter++)
				{
					tsa[tileCounter] ^= 0xC00;					
				}


				tmpMappings[0] = tsa[tileCounter + 3];
				tmpMappings[1] = tsa[tileCounter + 2];
				tmpMappings[2] = tsa[tileCounter+1];
				tmpMappings[3] = tsa[tileCounter];
		
			
				mappingId = MappingExists(tmpMappings, &newMapData);
				if (mappingId == 0xbaad)
				{
					for (int tileCounter = 0; tileCounter < 4; tileCounter++)
					{
						gbaTroid.nTSA.push_back(tmpMappings[tileCounter]);
					}
					mappingId = (gbaTroid.nTSA.size() / 4) - 1;
				}

				background->blocks[thisX + (thisY * Width)] = mappingId;
				break;
			case 0:

				break;
			}

			
		}
	}
	QuantifyTable();
	for (int thisY = 0; thisY < Height; thisY++) {

		for (int thisX = 0; thisX < (Width); thisX++) {// from here if something is enabled then draw it 
			unsigned short	hflip = 1;
			unsigned short		vflip = 1;
			unsigned short		curTile = TileBuf2D[thisX + (thisY * Width)];
			unsigned short		TILE = (curTile & 0x3ff);
			unsigned short		flip = (curTile & 0xC00) >> 8;
			unsigned short clipdata = curTile & 0x1000 >> 0xC;


			int keyIndex = 0xBAAD;
			for (auto &i : newTileTableMappingIndexes)
			{
				if ( i.first== TILE) {
					keyIndex =i.second;
					break; // to stop searching
				}
			}

			if (keyIndex == 0xBAAD)
			{
				keyIndex = 0;
			}
			if (TILE == 0xFF)
			{
				keyIndex = 0x4F;
			}
			background->blocks[thisX + (thisY * Width)] = keyIndex;


		}
	}

}

