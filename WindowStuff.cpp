#include "main.h"
using namespace std;
wxComboBox* hey;
MainPanel* MainPanel::myPanel;
void MainPanel::comboTabAction(wxCommandEvent& event)
{
	int testOffset = -1;
	wxEventType ay = event.GetEventType();
	
		
		RefreshRoom();
		
		// TODO: Send event as well
	    
	
	
}
wxSizer* MainPanel::BuildEditingOptions(){
	wxStaticBoxSizer* mainsiz = new wxStaticBoxSizer(wxHORIZONTAL,this,wxT("Editing Options"));
	chkTileEdit=new wxCheckBox(this, CHKTILEEDIT, wxT("Edit Tiles"));
	chkPropEdit=new wxCheckBox(this, CHKPROPEDIT,wxT("Edit Properties"));
	offsetSelect = new wxComboBox(this, COMBOOFFSET, _T(""), wxDefaultPosition, wxDefaultSize,
		0, NULL, wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB| wxCB_DROPDOWN);
	//offsetSelect->GetEventHandler()->Connect( wxEVT_COMBOBOX,(wxObjectEventFunction)&MainPanel::comboTabAction,this);
	vector<wxString>  b;
	FILE* fp = fopen("mdb.txt", "r");


	if (fp)
	{
		char blah[16];
		while (fgets(blah, 16, fp) != NULL)
		{
			b.push_back(blah);
		}
		
		fclose(fp);
	}

	
	offsetSelect->Insert(b,0);
	offsetSelect->Select(0);
	mainsiz->Add(offsetSelect);
	mainsiz->Add(chkTileEdit);
	mainsiz->Add(chkPropEdit);
	return mainsiz;
	//Make a frame, checkbox for tile editing, checkbox for tile prop editing

}
wxSizer* MainPanel::BuildMap()
{
	// house the whole box in a static box (aka "group box")
	wxStaticBoxSizer* mainsiz = new wxStaticBoxSizer(wxVERTICAL,this,wxT("Map"));

	// start building "rows" of controls.
	//   these rows added to mainsiz one at a time, with 4 pixel spacers between each one
	//   note that while I reuse the 'row' variable, a new sizer is created each
	//   time with 'new'.
	wxBoxSizer* row;
	wxBoxSizer* col;
	//  top row:  name
	TheMapWindow = new Mapclass();
	TheMapWindow->pic= new PicWindow(this,IMAGEID::MAP,512+20,256+20);
	row = new wxBoxSizer(wxHORIZONTAL);

	TheMapWindow->pic->MakeVSB();
	
	TheMapWindow->pic->VSB->SetScrollbar(0,5,16,4);
  
	TheMapWindow->pic->MakeHSB();
	TheMapWindow->pic->HSB->SetScrollbar(0,5,16,4);

	row = new wxBoxSizer(wxHORIZONTAL);
	TheMapWindow->pic->gI=new gameImages(&TheMapWindow->pic->dcPic);
	row->Add(TheMapWindow->pic);
	
	mainsiz->Add(row);
	mainsiz->AddSpacer(4);

	// next row:  2x2 choice box
	// next row:  2x2 choice box
	row = new wxBoxSizer(wxHORIZONTAL);

	row->AddSpacer(8);
	{
		wxBoxSizer* col = new wxBoxSizer(wxHORIZONTAL);
		col->AddStretchSpacer(1);

		//row->Add(col,1,wxALIGN_RIGHT | wxALIGN_TOP|wxEXPAND);

		row->Add(col,3, wxALIGN_BOTTOM);
	}

	mainsiz->Add(row,0,wxEXPAND);



	// that's it, this sizer is complete
	return mainsiz;

}

wxSizer* MainPanel::BuildTileset()
{
	// house the whole box in a static box (aka "group box")
	wxStaticBoxSizer* mainsiz = new wxStaticBoxSizer(wxHORIZONTAL,this,wxT("Tileset"));

	// start building "rows" of controls.
	//   these rows added to mainsiz one at a time, with 4 pixel spacers between each one
	//   note that while I reuse the 'row' variable, a new sizer is created each
	//   time with 'new'.
	wxBoxSizer* row;
	wxBoxSizer* col;
	//  top row:  name
	TheTileset = new Tilesetclass();
	TheTileset->pic= new PicWindow(this,IMAGEID::TILESET,528,128);
	TheTileset->pic->MakeVSB();
	TheTileset->pic->VSB->SetScrollbar(0,5,16,4);

	row = new wxBoxSizer(wxHORIZONTAL);
	TheTileset->pic->gI=new gameImages(&TheTileset->pic->dcPic);

	row->Add(TheTileset->pic);
	
	mainsiz->Add(row);
	mainsiz->AddSpacer(4);

	// next row:  2x2 choice box
	// next row:  2x2 choice box
	row = new wxBoxSizer(wxHORIZONTAL);

	row->AddSpacer(8);
	{
		wxBoxSizer* col = new wxBoxSizer(wxHORIZONTAL);
		col->AddStretchSpacer(1);

		row->Add(col,1, wxALIGN_TOP|wxEXPAND);

		//row->Add(col,3,wxALIGN_RIGHT | wxALIGN_BOTTOM);
	}

	mainsiz->Add(row,0,wxEXPAND);


	// that's it, this sizer is complete
	return mainsiz;
}


