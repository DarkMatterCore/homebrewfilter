#include <unistd.h>
#include <algorithm>

#include "libwiigui/gui.h"
#include "main.h"
#include "menu.h"
#include "Prompts/prompts.h"
#include "Network/network.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();
extern void HaltResumeGui();

extern bool runaway;

/****************************************************************************
 * MenuSettings
 ***************************************************************************/
int MenuSettingsNetwork()
{
	int menu = MENU_NONE;

	int ret = -1;
	int i = 0;
	int focus = 0;

	int network = Options.temp_network;
	int wifigecko = Options.temp_wifigecko;
	int newrevtext = Options.temp_newrevtext;

	OptionList options;

	sprintf(options.name[i++], tr("Auto Connect"));
	sprintf(options.name[i++], tr("Wifi Debug Log"));
	sprintf(options.name[i++], tr("Update Info"));
	options.length = i;

	GuiImageData bgImgData(Theme.background);
	GuiImageData btnOutline(Theme.button_small);
	GuiImageData btnOutlineOver(Theme.button_small_focus);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiImage bgImg(&bgImgData);

	GuiText titleTxt(tr("Network Settings"), 28, (GXColor){Theme.title_1, Theme.title_2, Theme.title_3, 255});
	titleTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	titleTxt.SetPosition(50,50);

	GuiText okBtnTxt(tr("OK"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiImage okBtnImg(&btnOutline);
	GuiImage okBtnImgOver(&btnOutlineOver);
	GuiButton okBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	okBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	okBtn.SetPosition(100, -35);
	okBtn.SetLabel(&okBtnTxt);
	okBtn.SetImage(&okBtnImg);
	okBtn.SetImageOver(&okBtnImgOver);
	okBtn.SetTrigger(&trigA);
	okBtn.SetEffectGrow();

	GuiText backBtnTxt(tr("Stop"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiImage backBtnImg(&btnOutline);
	GuiImage backBtnImgOver(&btnOutlineOver);
	GuiButton backBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	backBtn.SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
	backBtn.SetPosition(-100, -35);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetEffectGrow();

	GuiOptionBrowser optionBrowser(552, 248, &options);
	optionBrowser.SetPosition(0, 108);
	optionBrowser.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	optionBrowser.SetCol2Position(340);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&bgImg);
	w.Append(&titleTxt);
	w.Append(&okBtn);
	w.Append(&backBtn);
	mainWindow->Append(&w);
	mainWindow->Append(&optionBrowser);

	mainWindow->ChangeFocus(&optionBrowser);
	ResumeGui();

	int change = 0;
	while(menu == MENU_NONE)
	{
		usleep(100);

		ret = optionBrowser.GetChangedOption();

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT) || PAD_ButtonsDown(0) & PAD_BUTTON_RIGHT
		  || WUPC_ButtonsDown(0) & WPAD_CLASSIC_BUTTON_RIGHT)
		{
			change = 0;
			switch (ret)
			{
				case 0:
					change = network;
					change++;
					if(change > 1)
						change = 1;
					network = change;
					break;

				case 1:
					change = wifigecko;
					change++;
					if(change > 1)
						change = 1;
					wifigecko = change;
					break;

				case 2:
					change = newrevtext;
					change++;
					if(change > 1)
						change = 1;
					newrevtext = change;
					break;
			}
			HaltResumeGui();
		}

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_LEFT) || PAD_ButtonsDown(0) & PAD_BUTTON_LEFT
		  || WUPC_ButtonsDown(0) & WPAD_CLASSIC_BUTTON_LEFT)
		{
			change = 0;
			switch (ret)
			{
				case 0:
					change = network;
					change--;
					if(change < 0)
						change = 0;
					network = change;
					break;

				case 1:
					change = wifigecko;
					change--;
					if(change < 0)
						change = 0;
					wifigecko = change;
					break;

				case 2:
					change = newrevtext;
					change--;
					if(change < 0)
						change = 0;
					newrevtext = change;
					break;
			}
			HaltResumeGui();
		}

		if(change != -1)
		{
			change = -1;

			if(network == 0)
				sprintf (options.value[0], tr("No"));
			else
				sprintf (options.value[0], tr("Yes"));

			if(wifigecko == 0)
				sprintf (options.value[1], tr("No"));
			else
				sprintf (options.value[1], tr("Yes"));

			if(newrevtext == 0)
				sprintf (options.value[2], tr("No"));
			else
				sprintf (options.value[2], tr("Yes"));

			optionBrowser.TriggerUpdate();
		}

		if(optionBrowser.GetClickedOption() != -1)
			optionBrowser.TriggerUpdate();

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B) || PAD_ButtonsDown(0) & PAD_BUTTON_B
		   || WUPC_ButtonsDown(0) & WPAD_CLASSIC_BUTTON_B)
		{
			if(focus == 0)
			{
				focus = 1;
				mainWindow->ChangeFocus(&w);
			}
			else
			{
				focus = 0;
				mainWindow->ChangeFocus(&optionBrowser);
			}
			HaltResumeGui();
		}

		if(okBtn.GetState() == STATE_CLICKED)
		{
			Options.temp_last_setting = 1;
			if (Options.temp_network != network)
			{
				if(network == 1)
				{
					ResumeNetworkThread();
				}
				else
				{
					HaltNetworkThread();
				}
			}
			Options.temp_network = network;
			Options.temp_wifigecko = wifigecko;
			Options.temp_newrevtext = newrevtext;
			menu = MENU_SETTINGS_FILE;
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			Options.temp_last_setting = 1;
			menu = MENU_SETTINGS_FILE;
		}

		if(runaway == true)
		{
			Options.temp_last_setting = 1;
			menu = MENU_SETTINGS_FILE;
		}
	}
	HaltGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	return menu;
}

