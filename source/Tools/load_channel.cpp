
#include <stdio.h>
#include <gccore.h>
#include <malloc.h>
#include <string.h>
#include <string>
#include <vector>
#include "filelist.h"
using namespace std;

string HBC_version;

template <class pointer>
s8 free_pointer(pointer*& ptr)
{
	if(ptr != NULL)
	{
		if( sizeof(pointer) > 0 )
		{
			//protection incase empty type was given
			memset(ptr,0,sizeof(pointer));
		}
		free(ptr); 
		ptr = NULL;
		return 0;
	}
	else
	{
		return -1;
	}
}

char * DetectHBC_version(const tmd *t)
{
	static char desc[10];
	if (t->title_version) 
		snprintf(desc, sizeof(desc), "%d.%d", t->title_version >> 8, t->title_version & 0xFF);
/*  if (t->sys_version)
		snprintf(desc, sizeof(desc), "%s FW: IOS%llu ", desc, t->sys_version & 0xff);
*/
	return desc;
}

u64 DetectHBC()
{
    u64 *list;
    u32 titlecount;
    s32 ret;
	
	vector<u64>	TitleID;
	TitleID.push_back(0x0001000148415858LL);	//HAXX
	TitleID.push_back(0x000100014A4F4449LL);	//JODI
	TitleID.push_back(0x00010001AF1BF516LL);	//>=0.7
	

    ret = ES_GetNumTitles(&titlecount);
    if(ret < 0)
	{
		printf("DetectHBC: ES_GetNumTitles Fehler\n");
		return 0;
	}

    list = (u64*)memalign(32, titlecount * sizeof(u64) + 32);

	ret = ES_GetTitles(list, titlecount);
	if(ret < 0) {
		printf("DetectHBC: ES_GetTitles Error\n");
		free_pointer(list);
		return 0;
    }
	ret = 0;
	
	//lets check for known HBC title id's.
    for(u32 i=0; i<titlecount; i++) 
	{
		u32 tmd_size;
		ES_GetStoredTMDSize(list[i], &tmd_size);
		static u8 tmd_buf[MAX_SIGNED_TMD_SIZE] ATTRIBUTE_ALIGN(32);
		signed_blob *s_tmd = (signed_blob *)tmd_buf;
		ES_GetStoredTMD(list[i], s_tmd, tmd_size);

		if (list[i] == TitleID[0])	//HAXX
        {
			if (ret < 1)
			{
				ret = 1;
				HBC_version = "HAXX";
			}
        }
		if (list[i] == TitleID[1])	//JODI
		{
			if (ret < 2)
			{
				ret = 2;
				HBC_version = "JODI";
			}
		}
 		if (list[i] == TitleID[2])	//>=0.7
		{
			if(ret < 3)
			{
				ret = 3;
				HBC_version = DetectHBC_version((tmd *)SIGNATURE_PAYLOAD(s_tmd));
			}
		}
    }
	free_pointer(list);
    if(ret != 0)
		return TitleID[ret -1];
	else
		printf("ERROR_FIND_HBC\n");

	return 0;
}

void LoadHBC()
{
	u64 TitleID = DetectHBC();
	if(TitleID != 0)
	{
		u32 cnt ATTRIBUTE_ALIGN(32);
		ES_GetNumTicketViews(TitleID, &cnt);
		tikview *views = (tikview *)memalign( 32, sizeof(tikview)*cnt );
		ES_GetTicketViews(TitleID, views, cnt);
		ES_LaunchTitle(TitleID, &views[0]);
	}
	else
	{
		//well that went wrong
		printf("ERROR_BOOT_HBC\n");
	}
	
}

bool CheckAppFound(u64 title)
{

	char tmd[ISFS_MAXPATH];

	u32 high = (u32)(title >> 32);
	u32 low  = (u32)(title & 0xFFFFFFFF);

	sprintf(tmd, "/title/%08x/%08x/content/00000000.app", high, low);

	s32 check = ISFS_Open(tmd, ISFS_OPEN_READ);
	if(check == -102 || check > 0)
	{
		ISFS_Close(check);
		return true;
	}

	return false;
}

u8 hbcStubAvailable()
{
	char * sig = (char *)0x80001804;
	return (
        sig[0] == 'S' &&
        sig[1] == 'T' &&
        sig[2] == 'U' &&
        sig[3] == 'B' &&
        sig[4] == 'H' &&
        sig[5] == 'A' &&
        sig[6] == 'X' &&
        sig[7] == 'X') ? 1 : 0;
}

int DetectHBF()
{
	u64 *list;
    u32 titlecount;
    int ret;
	
	vector<u64>	TitleID;
	TitleID.push_back(0x0001000154484246LL);	//THBF
//	TitleID.push_back(0x0001000148424630LL);	//HBF0

    ret = ES_GetNumTitles(&titlecount);
    if(ret < 0)
	{
		printf("DetectHBF: ES_GetNumTitles Fehler\n");
		return 0;
	}

    list = (u64*)memalign(32, titlecount * sizeof(u64) + 32);

	ret = ES_GetTitles(list, titlecount);
	if(ret < 0) {
		printf("DetectHBF: ES_GetTitles Error\n");
		free_pointer(list);
		return 0;
    }
	ret = 0;
	
	//lets check for known HBF title id's.
    for(u32 i=0; i<titlecount; i++) 
	{
		u32 tmd_size;
		ES_GetStoredTMDSize(list[i], &tmd_size);
		static u8 tmd_buf[MAX_SIGNED_TMD_SIZE] ATTRIBUTE_ALIGN(32);
		signed_blob *s_tmd = (signed_blob *)tmd_buf;
		ES_GetStoredTMD(list[i], s_tmd, tmd_size);

		if (list[i] == TitleID[0] && CheckAppFound(list[i]))	//THBF
        {
			if (ret < 1)
				ret = 1;
		}
/*		if (list[i] == TitleID[1] && CheckAppFound(list[i]))	//HBF0
        {
			if (ret < 2)
				ret = 2;
		}
*/	}
	free_pointer(list);
    if(ret != 0)
	{
		memcpy((void*)0x80001800, stub_bin, stub_bin_size);
		DCFlushRange((void*)0x80001800,stub_bin_size);
		
		hbcStubAvailable();
		return ret;
	}

	return 0;
}

void LoadHBF()
{
	int ret = DetectHBF();
	if(ret == 1)
	{
		*(vu16*)0x800024CA = 0x5448;//"TH";
		*(vu16*)0x800024D2 = 0x4246;//"BF";
	}
/*	else if(ret == 2)
	{
		*(vu16*)0x800024CA = 0x4842;//"HB";
		*(vu16*)0x800024D2 = 0x4630;//"F0";
	}
*/
}