#include "Backgrounds.h"

void Backgrounds::Export(MemFile* thisFile, int* dstArry)
{
	BG* array2[5]{		bg0,		bg1,		bg2,		clip,		bg3	};
	for (int i = 0; i < 5; i++)
	{
		if (array2[i] != NULL)
		{
			dstArry[i] = thisFile->GetIndex();
			array2[i]->prop = 0x10;
			array2[i]->Export(thisFile);
			thisFile->Align(4);
		}
	}
}

Backgrounds::Backgrounds(int width, int height)
{

	bg0 = NULL;// new BG(width, height);

	bg1 = new BG(width, height);

	bg2 = new BG(width, height);

	bg3 = NULL;// new BG(width, height);

	clip = new BG(width, height);
}


Backgrounds::~Backgrounds()
{
	delete bg0;

	delete  bg1;

	delete  bg2;

	delete  bg3;

	delete  clip;
}
