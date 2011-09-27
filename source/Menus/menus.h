
int MenuMain();
int MenuSettings();
int MenuSettingsDisplay();
int MenuSettingsFile();
int MenuSettingsLanguage();
int MenuSettingsFont();
int MenuSettingsTheme();
int MenuSettingsChildlock();
int MenuSettingsNetwork();

bool GetMenuSettingsLanguageDL();
bool GetMenuSettingsFontDL();
bool GetMenuSettingsThemeDL();
void SetMenuSettingsLanguageDL(bool);
void SetMenuSettingsFontDL(bool);
void SetMenuSettingsThemeDL(bool);

enum
{
	THEME,
	LANGUAGE,
	FONT,
	SLIDE_EFFECT,
	CATEGORY_REMEMBER,
	NUMBER_OF_APPS,
	QUICK_START,
	STORAGE_DEVICE,
	DEVICE_ICON,
	CHILDLOCK,
	NAVIGATION,
	DISPLAY,
	NETWORK
};

enum
{
	DISPLAY_NONE,
	DISPLAY_TOP,
	DISPLAY_BOTTOM,
	DISPLAY_LEFT,
	DISPLAY_RIGHT
};