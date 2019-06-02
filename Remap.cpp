#include "SMClass.h"
#include "BG.h"
#include <map>
using namespace std;
map<int, int> flipMapping;


void AddGBAPadding(int width, int height, vector<unsigned short>* buff)
{
	//make new buffer 
	vector<unsigned short> newBuffer;
	
	int newWidth = width + 4;//left and right padding
	int newHeight = height + 4;//top and bottom padding
	newBuffer.resize(newWidth * newHeight);

	//for (int x = 0; x < width; x++)
	//{
		for (int y = 0; y < height; y++)
		{
		//	int newBufX = x + 2;
			int newBufY = y + 2;
			 //= buff->at(x + y*width);
			memcpy(&newBuffer[newBufY*(width) + 2 ], &buff->at(y*width), 2 * (width) - 4);
		}
	//}

	*buff = newBuffer;


}

void SMClass::QuantifyMapTiles(BG* background)
{
	int  Width = RoomHeader.Width * 16;
	int Height = RoomHeader.Height * 16;
	AddGBAPadding(Width,Height, &background->blocks);
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
			unsigned short		keyIndex = (curTile & 0x3ff);
			unsigned short		flip = (curTile & 0xC00) >> 8;
			unsigned short clipdata = curTile & 0x1000 >> 0xC;

			if (flip == 0)
			{
				continue;
			}
			if (keyIndex == 0xC)
			{
				keyIndex = 0xC;
			}
			memcpy(tsa, &gbaTroid.nTSA[keyIndex * 4], 4 * 2);
			int tileCounter = 0;
			//Apply flip transformations
			int newTile = 0;
			unsigned short tmpMappings[4] = { 0 };
			char flipType[32];
			switch (flip) {

			case 0x4:
				sprintf(flipType, "Horizontal");
				for (int tileCounter = 0; tileCounter < 4; tileCounter++)
				{
					tsa[tileCounter] |= 0x400;
				}

				tmpMappings[0] = tsa[tileCounter + 1];
				tmpMappings[1] = tsa[tileCounter];
				tmpMappings[2] = tsa[tileCounter + 3];
				tmpMappings[3] = tsa[tileCounter + 2];


				break;
			case 0x8:
				sprintf(flipType, "Vertical");
				for (int tileCounter = 0; tileCounter < 4; tileCounter++)
				{
					tsa[tileCounter] |= 0x800;
				}

				tmpMappings[0] = tsa[tileCounter + 2];
				tmpMappings[1] = tsa[tileCounter + 3];
				tmpMappings[2] = tsa[tileCounter];
				tmpMappings[3] = tsa[tileCounter + 1];

				break;
			case 0xC:
				sprintf(flipType, "Vertical and Horizontal");
				for (int tileCounter = 0; tileCounter < 4; tileCounter++)
				{
					tsa[tileCounter] |= 0xC00;
				}
				tmpMappings[0] = tsa[tileCounter + 3];
				tmpMappings[1] = tsa[tileCounter + 2];
				tmpMappings[2] = tsa[tileCounter + 1];
				tmpMappings[3] = tsa[tileCounter];

				break;
			}


			mappingId = MappingExists(tmpMappings, &newMapData);
			if (mappingId == 0xbaad)
			{
				for (int tileCounter = 0; tileCounter < 4; tileCounter++)
				{
					gbaTroid.nTSA.push_back(tmpMappings[tileCounter]);
				}
				mappingId = (gbaTroid.nTSA.size() / 4) - 1;
			}

			char debug[1024];
			sprintf(debug, "Inserted %x new orientation %s mappingId %x", keyIndex, flipType, mappingId);
			Logger::log->LogIt(Logger::DEBUG, debug);

			flipMapping[(flip * 0x100) | keyIndex] = mappingId;
		}
	}

}

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
	bool isZM = true;
	for (int thisY = 0; thisY < Height; thisY++) {

		for (int thisX = 0; thisX < (Width); thisX++) {// from here if something is enabled then draw it 
			unsigned short	hflip = 1;
			unsigned short		vflip = 1;
			unsigned short		curTile = TileBuf2D[thisX + (thisY * Width)];
			unsigned short		TILE = (curTile & 0xC00) | (curTile & 0x3ff);
			unsigned short clipdata = (curTile & 0xF000) >> 0xC;

			switch (clipdata)
			{

			case 0:
			case 2:
			case 4:
			case 6:
				clipdata = 0;
				break;
			case 9:
				clipdata = 0x20;
				break;

			default:
				clipdata = 0x10;
				break;
			}
			theBgs->clip->layer = Layer::Clip;
			int sz = theBgs->clip->blocks.size();
			theBgs->clip->blocks[thisX + (thisY * Width)] = clipdata;

			int keyIndex = 0xBAAD;

			if (curTile & 0xC00)
			{
				for (auto &i : flipMapping)
				{
					if (i.first == TILE) {
						TILE = i.second;
						break; // to stop searching
					}
				}
			}

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
				keyIndex = blockCount - 1;
			}
			background->blocks[thisX + (thisY * Width)] = keyIndex;

		}
	}


}

