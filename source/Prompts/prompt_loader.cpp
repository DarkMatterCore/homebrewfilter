
#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"
#include "menu.h"
#include "Tools/SelectIos.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();

int
loaderPrompt()
{
	bool stop = false;
	int menu = 0;
	listIOS();

	GuiWindow promptWindow(520,360);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiTrigger trigA;
	GuiTrigger trigB;
	GuiTrigger trigHOME;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	trigB.SetButtonOnlyTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);
	trigHOME.SetButtonOnlyTrigger(-1, WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME, PAD_BUTTON_START);

	GuiText titleTxt(tr("External Loaders"), 26, (GXColor){Theme.title_1, Theme.title_2, Theme.title_3, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0,40);

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);

 	// Buttons data
	GuiImageData btn(Theme.button);
	GuiImage nandemuImg(&btn);
	GuiImage backImg(&btn);

	// Buttons over data
	GuiImageData btn_over(Theme.button_focus);
	GuiImage nandemuImgOver(&btn_over);
	GuiImage backImgOver(&btn_over);

	GuiText nandemuTxt(tr("Launch NandEmu (Uniiloader)"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton nandemu(btn.GetWidth(), btn.GetHeight());
	nandemu.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	nandemu.SetPosition(0, 75);
	nandemu.SetLabel(&nandemuTxt);
	nandemu.SetImage(&nandemuImg);
	nandemu.SetImageOver(&nandemuImgOver);
	nandemu.SetTrigger(&trigA);

	GuiText backTxt(tr("Back"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton back(btn.GetWidth(), btn.GetHeight());
	back.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	back.SetPosition(0, 270);
	back.SetLabel(&backTxt);
	back.SetImage(&backImg);
	back.SetImageOver(&backImgOver);
	back.SetTrigger(&trigA);
	back.SetTrigger(&trigB);

	GuiButton back2(0, 0);
	back2.SetTrigger(&trigHOME);


	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
	if(get_nandemu())
		promptWindow.Append(&nandemu);
	promptWindow.Append(&back);
	promptWindow.Append(&back2);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(!stop)
	{
		usleep(100);

		if(nandemu.GetState() == STATE_CLICKED)
		{
			set_nandemu(2);
			menu = MENU_EXIT;
			stop = true;
		}

		if(back.GetState() == STATE_CLICKED || back2.GetState() == STATE_CLICKED)
			stop = true;
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	return menu;
}
