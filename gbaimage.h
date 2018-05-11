
#ifndef __GBAIMAGE_H__
#define __GBAIMAGE_H__

///////////////////////////////////////
#include "gbatypes.h"
#include "gbatiles.h"
#include <wx/wx.h>
///////////////////////////////////////
class GBAImage
{
public:
	enum
	{
		FLIPX = 0x01,
		FLIPY = 0x02,
		NOTRANS = 0x04
	};

	GBAImage()  { Zero();       }
	~GBAImage() { Destroy();    }

	// build / teardown
	void        Create(int width,int height);
	void        Destroy();

	// palette
	void        SetPalette(const COLOR* pal,int first = 0,int count = 256);

	// drawing GBA tiles to this image
	void        DrawTile(unsigned char* tile,int dstx,int dsty,int palette,int flgs);
	void        DrawTile(GBATiles& tilebuf,int dstx,int dsty,int pftt);

	// drawing this image to wx 
	wxImage     GetImage();
	wxImage     GetImage(wxCoord x,wxCoord y,wxCoord wd,wxCoord ht);
	void        BlitToDC(wxDC* dst,wxCoord xdst,wxCoord ydst,wxCoord wd,wxCoord ht,wxCoord xsrc,wxCoord ysrc,int logicalFunc = wxCOPY);
	unsigned char*         pImage;
	wxMemoryDC* pDC;
protected:
	void        Zero();
	void        ValidateDC();

	int         nWd;
	int         nHt;
	COLOR       mPalette[256];


	bool        bDCValid;

private:
	GBAImage(const GBAImage&);
	GBAImage& operator = (const GBAImage&);
};

#endif //  __GBAIMAGE_H__