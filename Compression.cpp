#include "Compression.h"


long            Compression::RLECompress(unsigned char cmp[], unsigned char uncmp[], unsigned long sze)
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

Compression::Compression()
{
}


Compression::~Compression()
{
}
