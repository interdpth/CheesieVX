#ifndef EXT
#define EXT extern
#endif

//#include "wxhdr.h"
#include <wx/wx.h>
#include <stdio.h>
#include "wxDeclarations.h"
#include "app.h"
#include "mainpanel.h"
#include "picWindow.h"
#include "menu.h"
#include <vector>
#include "RomClass.h"
#include "SMClass.h"
#include <wx/bitmap.h>

#define u8  unsigned char
#define u16 unsigned short
#define u32 unsigned long


#ifndef DEMAP
#define DEMAP 

#endif 
EXT u16 Multiplier;
int SetCustom();
EXT int Trainer;

//LRESULT CALLBACK PokeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

