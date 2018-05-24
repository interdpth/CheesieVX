#include "SMClass.h"


int SMClass::ExportTileTable()
{
	gbaTroid.max = TSA.nTSA.size() / 8;
	gbaTroid.nTSA.resize(TSA.nTSA.size());
	FILE* fp2 = fopen("zmcretiletable", "r+b");
	if (fp2)
	{
		fread((unsigned char*)&gbaTroid.nTSA[0], 2, 0x140, fp2);
		fclose(fp2);
	}
	char moreDebugText[512] = { 0 };
	sprintf(moreDebugText, "Begin tile table remapping");
	Logger::log->LogIt(Logger::DEBUG, moreDebugText);
	for (int tsaCounter = 0; tsaCounter < TSA.nTSA.size(); tsaCounter++)
	{
		unsigned short pftt = TSA.nTSA[tsaCounter];
		gbaTroid.nTSA[tsaCounter] = snes2gba_tilemap(pftt);
	}

	return 0;
}

bool SMClass::CompareTileTables(unsigned short* tiles, unsigned short* tiles2)
{
	return memcmp(tiles, tiles2, 8) == 0;	
}

int SMClass::MappingExists(unsigned short* tiles, vector<unsigned short*>*newMap)
{
	for (int mappingCounter = 0; mappingCounter < newMap->size(); mappingCounter++)
	{
		if (CompareTileTables(tiles, newMap->at(mappingCounter)))
		{
			return mappingCounter;
		}
	}
	return 0xBAAD;
}

int SMClass::QuantifyTable()
{
	Logger* newLog = new Logger("tiletableremap.txt");
	newTileTableMapping.clear();
	newLog->LogIt(Logger::DEBUG, "QUANTIFYING TABLE");
	vector<unsigned short*> tiletableMappings;
	vector<unsigned short*> newtiletableMappings;
	int gbaTroidsize = gbaTroid.nTSA.size();
	int oldBlocks = gbaTroidsize / 4;
	for (int blockCounter = 0; blockCounter < gbaTroid.nTSA.size() / 4; blockCounter++)
	{
		unsigned short* theseBlocks = new unsigned short[4];
		memcpy(theseBlocks, &gbaTroid.nTSA[blockCounter * 4], 8);
		tiletableMappings.push_back(theseBlocks);
	}

	char debugText[512] = { 0 };
	sprintf(debugText, "%x tiles in the table", tiletableMappings.size());
	newLog->LogIt(Logger::DEBUG, debugText);
	for (int blockCounter = 0; blockCounter < tiletableMappings.size(); blockCounter++)
	{
		unsigned short* newData = new unsigned short[4];
		memcpy(newData, tiletableMappings[blockCounter], 8);
		int mappingId = MappingExists(newData, &newtiletableMappings);
		
		if (mappingId == 0xBAAD)
		{
			sprintf(debugText, "Found new tile", blockCounter);

			Logger::log->LogIt(Logger::DEBUG, debugText);

			newtiletableMappings.push_back(newData);
			newTileTableMapping[blockCounter] = newtiletableMappings.size() - 1;
			sprintf(debugText, "Inserting block ID %x -> %x : %x %x %x %x \n", blockCounter, newTileTableMapping[blockCounter], newData[0], newData[1], newData[2], newData[3]);
			
			newLog->LogIt(Logger::DEBUG, debugText);

		}
		else
		{
			int keyIndex = 0xBAAD;
			for (auto &i : newTileTableMapping)
			{
				if (i.second == mappingId) {
					keyIndex = i.first;
					break; // to stop searching
				}
			}

			if (keyIndex == 0xBAAD)
			{
				keyIndex = 0xBAAD;
			}
			sprintf(debugText, "Found duplicate id %x tilemapping is %x keyIndex is %x", blockCounter, mappingId, keyIndex);
			newLog->LogIt(Logger::DEBUG, debugText);
			newTileTableMapping[blockCounter] = keyIndex;
			sprintf(debugText, "Remapping from tile no %x -> mappingid %x -> lookedup index %x -> %x\n", blockCounter, mappingId, keyIndex, newTileTableMapping[blockCounter]);
			newLog->LogIt(Logger::DEBUG, debugText);
		}
	}

	gbaTroid.nTSA.clear();

	sprintf(debugText, "%x tiles in the table", newtiletableMappings.size());
	newLog->LogIt(Logger::DEBUG, debugText);

	for (auto &i : newTileTableMapping)
	{
		sprintf(debugText, "tile %x = %x tile in the table", i.first, i.second);
		newLog->LogIt(Logger::DEBUG, debugText);
	}
	
	


	for (int blockCounter = 0; blockCounter < newtiletableMappings.size(); blockCounter++)
	{
		unsigned short tilemap[4] = { 0 };
		
		memcpy(tilemap, newtiletableMappings.at(blockCounter), 8);

		for (int i = 0; i < 4; i++)
		{
			gbaTroid.nTSA.push_back(tilemap[i]);
		}
		delete[] newtiletableMappings[blockCounter];
	}

	sprintf(debugText, "%x tiles in the table", newtiletableMappings.size());
	newLog->LogIt(Logger::DEBUG, debugText);

	for (int blockCounter = 0; blockCounter < gbaTroid.nTSA.size() / 4; blockCounter++)
	{

		delete[] tiletableMappings[blockCounter];
	}
	delete newLog;
	return 0;
}
int SMClass::RemapRoomtiles()
{
	int  Width = RoomHeader.Width * 16;
	int Height = RoomHeader.Height * 16;
	u16* TileBuf2D = &Map[1];
	unsigned short tsa[4];
	for (int thisY = 0; thisY < Height; thisY++) {

		for (int thisX = 0; thisX < (Width); thisX++) {// from here if something is enabled then draw it 
			unsigned short	hflip = 1;
			unsigned short		vflip = 1;
			unsigned short		curTile = TileBuf2D[thisX + (thisY * Width)];
			unsigned short		TILE = (curTile & 0x3ff);
			unsigned short		flip = (curTile & 0xC00) >> 8;
			unsigned short clipdata = curTile & 0x1000 >> 0xC;

			memcpy(&gbaTroid.nTSA[curTile], tsa, 4 * 2);
			int tileCounter = 0;
			//Apply flip transformations
			switch (flip) {

			case 0x4:
				for (int tileCounter = 0; tileCounter < 4; tileCounter++)
				{
					tsa[tileCounter] = ~0x400;
				}
				break;
			case 0x8:
				for (int tileCounter = 0; tileCounter < 4; tileCounter++)
				{
					tsa[tileCounter] = ~0x800;
				}
				break;
			case 0xC:
				for (int tileCounter = 0; tileCounter < 4; tileCounter++)
				{
					tsa[tileCounter] = ~0xC00;
				}
				break;

			}


		}
	}
	return 0;
}
