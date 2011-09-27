
#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();
extern void HaltResumeGui();

/****************************************************************************
 * Kategorie entfernen
 ***************************************************************************/
string eraseCategory()
{
	bool stop = true;
	int kategorieInt = 1;
	string kategoriename = AvailableCategory.categories[kategorieInt];
	
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	GuiTrigger trigB;
	trigB.SetButtonOnlyTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);

	GuiWindow promptWindow(520,360);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);
	GuiImageData btnOutline(Theme.button_small);
	GuiImageData btnOutlineOver(Theme.button_small_focus);

	GuiText zeile1Txt(tr("Remove Category"), 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	zeile1Txt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	zeile1Txt.SetPosition(0, -100);

	GuiText zeile2Txt(AvailableCategory.categories[1].c_str(), 24, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	zeile2Txt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	zeile2Txt.SetPosition(0, -10);
	
	GuiText zeile4Txt(tr("Select Category   (-/+)"), 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	zeile4Txt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	zeile4Txt.SetPosition(0, 60);

	GuiText okTxt(tr("Yes"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiImage okImg(&btnOutline);
	GuiImage okImgOver(&btnOutlineOver);
	GuiButton ok(btnOutline.GetWidth(), btnOutline.GetHeight());
	ok.SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
	ok.SetPosition(-120, -25);
	ok.SetLabel(&okTxt);
	ok.SetImage(&okImg);
	ok.SetImageOver(&okImgOver);
	ok.SetTrigger(&trigA);

	GuiText backTxt(tr("No"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiImage backImg(&btnOutline);
	GuiImage backImgOver(&btnOutlineOver);
	GuiButton back(btnOutline.GetWidth(), btnOutline.GetHeight());
	back.SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
	back.SetPosition(120, -25);
	back.SetLabel(&backTxt);
	back.SetImage(&backImg);
	back.SetImageOver(&backImgOver);
	back.SetTrigger(&trigA);
	back.SetTrigger(&trigB);
	back.SetState(STATE_SELECTED);

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&zeile1Txt);
	promptWindow.Append(&zeile2Txt);
	promptWindow.Append(&zeile4Txt);
	promptWindow.Append(&ok);
	promptWindow.Append(&back);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(stop)
	{
		usleep(100);

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS) || PAD_ButtonsDown(0) & PAD_TRIGGER_R)
		{
			kategorieInt++;
			if ( kategorieInt >= (signed)AvailableCategory.categories.size() )
				kategorieInt = 1;
			
			kategoriename = AvailableCategory.categories[kategorieInt];
			zeile2Txt.SetText(kategoriename.c_str());
			HaltResumeGui();
		}
		
		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS) || PAD_ButtonsDown(0) & PAD_TRIGGER_L)
		{
			kategorieInt--;
			if ( kategorieInt < 1 )
				kategorieInt = AvailableCategory.categories.size() -1;
			
			kategoriename = AvailableCategory.categories[kategorieInt];
			zeile2Txt.SetText(kategoriename.c_str());
			HaltResumeGui();
		}
		
		if(ok.GetState() == STATE_CLICKED)
			stop = false;
		
		if(back.GetState() == STATE_CLICKED)
		{
			kategoriename = "NULL";
			stop = false;
		}
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	
	return kategoriename;
}

