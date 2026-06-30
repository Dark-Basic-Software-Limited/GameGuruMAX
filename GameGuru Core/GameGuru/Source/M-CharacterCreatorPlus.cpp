//----------------------------------------------------
//--- GAMEGURU - M-CharacterCreatorPlus
//----------------------------------------------------

// Includes
#include "stdafx.h"
#include "gameguru.h"
#include "M-CharacterCreatorPlusTTS.h"
#include "CCameraC.h"

//PE: GameGuru IMGUI.
#include "..\Imgui\imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "..\Imgui\imgui_internal.h"
#include "..\Imgui\imgui_impl_win32.h"
#include "..\Imgui\imgui_gg_dx11.h"

#include "master.h"
extern Master master;

// Defines
#define CCPMODELEXT ".dbo"

// Globals
bool g_bCharacterCreatorPlusActivated = false;
sCharacterCreatorPlus g_CharacterCreatorPlus;
bool g_bCharacterCreatorPrepAnims = false;

bool g_bLegsChangeCascade = false;
bool g_bFeetChangeCascade = false;

bool g_charactercreatorplus_preloading = false;
char g_charactercreatorplus_path[MAX_PATH];
int g_charactercreatorplus_part = 0;
char g_charactercreatorplus_tag[MAX_PATH];

// <mesh name, path>
static std::map<std::string, std::string> CharacterCreatorHeadGear_s;
static std::map<std::string, std::string> CharacterCreatorHair_s;
static std::map<std::string, std::string> CharacterCreatorHead_s;
static std::map<std::string, std::string> CharacterCreatorEyeglasses_s;
static std::map<std::string, std::string> CharacterCreatorFacialHair_s;
static std::map<std::string, std::string> CharacterCreatorBody_s;
static std::map<std::string, std::string> CharacterCreatorLegs_s;
static std::map<std::string, std::string> CharacterCreatorFeet_s;

static std::map<std::string, std::string> CharacterCreatorAccessory1_s;
static std::map<std::string, std::string> CharacterCreatorAccessory2_s;


// <mesh name, name shown to user>
static std::map<std::string, std::string> g_charactercreatorplus_annotation_list;
static std::map<std::string, std::string> CharacterCreatorAnnotatedHeadGear_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedHair_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedHead_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedEyeglasses_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedFacialHair_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedBody_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedFeet_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedLegs_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedAccessory1_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedAccessory2_s;




static std::map<std::string, std::string> g_charactercreatorplus_annotationtag_list;
static std::map<std::string, std::string> CharacterCreatorAnnotatedTagHeadGear_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedTagHair_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedTagHead_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedTagEyeglasses_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedTagFacialHair_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedTagBody_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedTagFeet_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedTagLegs_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedTagAccessory1_s;
static std::map<std::string, std::string> CharacterCreatorAnnotatedTagAccessory2_s;


std::vector<sCharacterType> g_CharacterType;
char pCharacterTypeDropDownList[32][260];
std::vector<int> g_CharacterTypeRoomPref;

std::vector<sRoomType> g_RoomType;
char pRoomTypeDropDownList[32][260];

std::vector<AutoSwapData*> g_headGearMandatorySwaps;
AutoSwapData* g_previousAutoSwap = nullptr;
#define MAXPARTICONS 100
int g_iPartsThatNeedReloaded[10] = { 0 };
int g_iPartsIconsIDs[32][10][MAXPARTICONS];
bool g_bPartIconsInit = false;
char g_SkinTextureStorage[MAX_PATH]; // Stores the skin tone texture when the face is changed. Referenced when changing the face to fit certain headgear.
std::vector<char*> g_restrictedParts;
float g_fLockerRoomOffset = 2.0f;
float g_fCCPZoom = 72.0f;
std::array<std::string, 10> g_maleStorage;
std::array<std::string, 10> g_femaleStorage;
std::array<std::string, 10> g_zombieStorage;
std::array<std::string, 10> g_genericStorage;
AutoSwapData* g_pLastHeadgearAutoSwap = nullptr;
int g_iPreviousCategorySelection = -1;
CameraTransition* g_pCurrentTransition = nullptr;
CameraTransition g_HeadTransition;
CameraTransition g_UpperBodyTransition;
CameraTransition g_LowerBodyTransition;
CameraTransition g_ZombieHeadTransition;
CameraTransition g_ZombieBodyTransition;
CameraTransition* g_pLastKnownTransition = nullptr;
GGVECTOR3 g_DefaultCamPosition;
GGVECTOR3 g_CurrentCamPosition;
GGVECTOR3 g_DefaultCamAngle;
GGVECTOR3 g_CurrentCamAngle;
void charactercreatorplus_dozoom();
int item_current_type_selection = 5;
uint32_t iLightIndex = -1;
std::vector<const char*> g_MeshesThatNeedDoubleSided;

static std::map<std::string, std::string> CharacterCreatorType_s;
int iDressRoom = 0, iCharObj = 0, iCharObjHeadGear = 0, iCharObjHair = 0, iCharObjHead = 0, iCharObjAccessory1 = 0, iCharObjAccessory2 = 0, iCharObjEyeglasses = 0, iCharObjFacialHair = 0, iCharObjLegs = 0, iCharObjFeet = 0;


bool bCharObjVisible = false;
char CCP_Room[260] = "lockers";
char CCP_Type[260] = "adult male";
char CCP_Name[260] = "\0";
static char CCP_Script[260] = "people\\patrol.lua";
char CCP_Path[260] = "entitybank\\user\\charactercreatorplus\\";
char CCP_SpeakText[1024] = "Hello there, I am a new character!\n";
wchar_t CCP_SpeakText_w[1024];

int CCP_Speak_Rate = 0;
char cSelectedLegsFilter[260] = "\0";
char cSelectedFeetFilter[260] = "\0";
char cSelectedICCode[260] = "\0";
char cSelectedHeadGear[260] = "\0";
char cSelectedHair[260] = "\0";
char cSelectedHead[260] = "\0";
char cSelectedEyeglasses[260] = "\0";
char cSelectedAccessory1[260] = "\0";
char cSelectedAccessory2[260] = "\0";

char cSelectedFacialHair[260] = "\0";
char cSelectedBody[260] = "\0";
char cSelectedLegs[260] = "\0";
char cSelectedFeet[260] = "\0";
ISpObjectToken * CCP_SelectedToken = 0;
LPSTR pCCPVoiceSet = "";
ImVec4 vColorSelected[5];
float oldx_f, oldy_f, oldz_f, oldangx_f, oldangy_f;
float editoroldx_f=0, editoroldy_f, editoroldz_f, editoroldangx_f, editoroldangy_f, editoroldmode_f, oldtcameraviewmode=1;
int iDelayThumbs = 99; //0; //Icon removed
int iDelayExecute = 0;
int iThumbsOffsetY = 0;
float fCharObjectY = 600.0f;
float ccpTargetX, ccpTargetY, ccpTargetZ, ccpTargetAX, ccpTargetAY , dressroomTargetAY;
float ccpObjTargetX, ccpObjTargetY, ccpObjTargetZ, ccpObjTargetAX, ccpObjTargetAY, ccpObjTargetAZ;
float fCCPRotateY = 0.0f;
entityeleproftype g_grideleprof_holdchoices;

extern bool bTriggerMessage;
extern char cTriggerMessage[MAX_PATH];
void DisplaySmallImGuiMessage(char *text);
bool bMessageDisplayed = false;

bool g_bCharacterCreatorTypesInit = false;

extern preferences pref;

void charactercreatorplus_populatechartypes (void)
{
	if (g_bCharacterCreatorTypesInit == false)
	{
		// store current dir
		cstr pOldDir = GetDir();
		GG_SetWritablesToRoot(1);

		// collect rooms
		sRoomType roomtypeitem;
		strcpy (roomtypeitem.pPartsFolder, "locker room");  g_RoomType.push_back(roomtypeitem); //0
		strcpy (roomtypeitem.pPartsFolder, "pine dressing room");  g_RoomType.push_back(roomtypeitem); //1
		strcpy (roomtypeitem.pPartsFolder, "zombie locker room");  g_RoomType.push_back(roomtypeitem); //2
		SetDir("charactercreatorplus\\rooms");
		ChecklistForFiles();
		for (t.c = 1; t.c <= ChecklistQuantity(); t.c++)
		{
			t.tfile_s = ChecklistString(t.c);
			LPSTR pThisFile = t.tfile_s.Get();
			if (Len(pThisFile) > 2)
			{
				if (stricmp(pThisFile, "locker room") == NULL || stricmp(pThisFile, "pine dressing room") == NULL || stricmp(pThisFile, "zombie locker room") == NULL)
				{
					// already have these from above
				}
				else
				{
					char pCheckRoomFile[MAX_PATH];
					sprintf(pCheckRoomFile, "%s\\room.dbo", t.tfile_s.Get());
					if (FileExist (pCheckRoomFile) == 1)
					{
						strcpy (roomtypeitem.pPartsFolder, t.tfile_s.Get());
						g_RoomType.push_back(roomtypeitem);
					}
				}
			}
		}
		// make dropdown string array from above
		int n = 0;
		for (; n < g_RoomType.size(); n++)
		{
			bool bCapitaliseOnNewWord = true;
			strcpy (pRoomTypeDropDownList[n], g_RoomType[n].pPartsFolder);
			for (int t = 0; t < strlen(pRoomTypeDropDownList[n]); t++)
			{
				if (pRoomTypeDropDownList[n][t] >= 'a' && pRoomTypeDropDownList[n][t] <= 'z')
				{
					if (bCapitaliseOnNewWord == true)
					{
						pRoomTypeDropDownList[n][t] -= 32;
					}
				}
				bCapitaliseOnNewWord = false;
				if (pRoomTypeDropDownList[n][t] == ' ') bCapitaliseOnNewWord = true;
			}
		}
		for (; n < 32; n++) strcpy(pRoomTypeDropDownList[n], "");
		SetDir(pOldDir.Get());

		// gather character creator types database
		sCharacterType chartypeitem;
		strcpy (chartypeitem.pPartsFolder, "adult male");  g_CharacterType.push_back(chartypeitem); g_CharacterTypeRoomPref.push_back(0);
		strcpy (chartypeitem.pPartsFolder, "adult female");  g_CharacterType.push_back(chartypeitem); g_CharacterTypeRoomPref.push_back(1);
		strcpy (chartypeitem.pPartsFolder, "zombie male");  g_CharacterType.push_back(chartypeitem); g_CharacterTypeRoomPref.push_back(2);
		strcpy (chartypeitem.pPartsFolder, "zombie female");  g_CharacterType.push_back(chartypeitem); g_CharacterTypeRoomPref.push_back(2);
		SetDir("charactercreatorplus\\parts");
		ChecklistForFiles();
		// first pass for annotations
		for (t.c = 1; t.c <= ChecklistQuantity(); t.c++)
		{
			t.tfile_s = ChecklistString(t.c);
			LPSTR pThisFile = t.tfile_s.Get();
			if (Len(pThisFile) > 2)
			{
				if (stricmp(pThisFile, "adult male") == NULL || stricmp(pThisFile, "adult female") == NULL
				||  stricmp(pThisFile, "zombie male") == NULL || stricmp(pThisFile, "zombie female") == NULL)
				{
					// already have these from above
				}
				else
				{
					// confirm a real body parts folder
					char pCheckAnnotFile[MAX_PATH];
					sprintf(pCheckAnnotFile, "%s\\annotates.txt", t.tfile_s.Get());
					if (FileExist (pCheckAnnotFile) == 1)
					{
						sCharacterType chartypeitem;
						strcpy (chartypeitem.pPartsFolder, t.tfile_s.Get());
						g_CharacterType.push_back(chartypeitem);
						g_CharacterTypeRoomPref.push_back(0);
					}
				}
			}
		}
		// second pass for roomprefs
		for (t.c = 1; t.c <= ChecklistQuantity(); t.c++)
		{
			t.tfile_s = ChecklistString(t.c);
			LPSTR pThisCharFile = t.tfile_s.Get();
			if (Len(pThisCharFile) > 2)
			{
				char pCheckRoomPrefFile[MAX_PATH];
				sprintf(pCheckRoomPrefFile, "%s\\roompref.txt", pThisCharFile);
				if (FileExist (pCheckRoomPrefFile) == 1)
				{
					OpenToRead(1, pCheckRoomPrefFile);
					LPSTR pRoomPref = ReadString(1);
					CloseFile(1);
					for (int n = 0; n < g_CharacterType.size(); n++)
					{
						if (stricmp(g_CharacterType[n].pPartsFolder, pThisCharFile) == NULL)
						{
							// find pRoomPref string in room types list
							for (int r = 0; r < g_RoomType.size(); r++)
							{
								if (stricmp(g_RoomType[r].pPartsFolder, pRoomPref) == NULL)
								{
									g_CharacterTypeRoomPref[n] = r;
									break;
								}
							}
						}
					}
				}
			}
		}
		// make dropdown string array from above
		n = 0;
		for (; n < g_CharacterType.size(); n++)
		{
			bool bCapitaliseOnNewWord = true;
			strcpy (pCharacterTypeDropDownList[n], g_CharacterType[n].pPartsFolder);
			for (int t = 0; t < strlen(pCharacterTypeDropDownList[n]); t++)
			{
				if (pCharacterTypeDropDownList[n][t] >= 'a' && pCharacterTypeDropDownList[n][t] <= 'z')
				{
					if (bCapitaliseOnNewWord == true)
					{
						pCharacterTypeDropDownList[n][t] -= 32;
					}
				}
				bCapitaliseOnNewWord = false;
				if (pCharacterTypeDropDownList[n][t] == ' ') bCapitaliseOnNewWord = true;
			}
		}
		for (; n < 32; n++) strcpy(pCharacterTypeDropDownList[n], "");

		// completed init
		SetDir(pOldDir.Get());
		GG_SetWritablesToRoot(0);
		g_bCharacterCreatorTypesInit = true;
	}
}

void charactercreatorplus_preloadinitialcharacter ( void )
{
	return; //PE: Disabled until we can do multiply thread loads.
}

void charactercreatorplus_copyselections(std::array<std::string, 10>& storage)
{
	// 0: Head Gear
	// 1: Hair
	// 2: Head
	// 3: Eye Glasses
	// 4: Facial Hair
	// 5: Body
	// 6: Legs
	// 7: Feet
	storage[0] = std::string(cSelectedHeadGear);
	storage[1] = std::string(cSelectedHair);
	storage[2] = std::string(cSelectedHead);
	storage[3] = std::string(cSelectedEyeglasses);
	storage[4] = std::string(cSelectedFacialHair);
	storage[5] = std::string(cSelectedBody);
	storage[6] = std::string(cSelectedLegs);
	storage[7] = std::string(cSelectedFeet);
	storage[8] = std::string(cSelectedAccessory1);
	storage[9] = std::string(cSelectedAccessory2);
}

void charactercreatorplus_GetDefaultCharacterPartNum (int iBase, int iPart, LPSTR pPartNumStr, LPSTR pPartNumVariantStr = NULL)
{
	LPSTR pPart = "";
	LPSTR pPartVariant = "";
	if (iBase == 1)
	{
		// male
		if (iPart == 1) { pPart = "13"; pPartVariant = "13c"; }
		if (iPart == 2) { pPart = "07"; pPartVariant = "07"; }
		if (iPart == 3) { pPart = "12"; pPartVariant = "12c"; }
		if (iPart == 4) { pPart = "10"; pPartVariant = "10"; }
		if (iPart == 5) { pPart = "10"; pPartVariant = "10"; }
	}
	if (iBase == 2)
	{
		// female
		if (iPart == 1) { pPart = "07"; pPartVariant = "07"; }
		if (iPart == 2) { pPart = "07"; pPartVariant = "07"; }
		if (iPart == 3) { pPart = "07"; pPartVariant = "07"; }
		if (iPart == 4) { pPart = "05"; pPartVariant = "05"; }
		if (iPart == 5) { pPart = "06"; pPartVariant = "06"; }
	}
	if (iBase == 3)
	{
		// zombie male
		if (iPart == 1) { pPart = "01"; pPartVariant = "01"; }
		if (iPart == 2) { pPart = "01"; pPartVariant = "01"; }
		if (iPart == 3) { pPart = "01"; pPartVariant = "01"; }
		if (iPart == 4) { pPart = "01"; pPartVariant = "01"; }
		if (iPart == 5) { pPart = "01"; pPartVariant = "01"; }
	}
	if (iBase == 4)
	{
		// zombie female
		if (iPart == 1) { pPart = "01"; pPartVariant = "01"; }
		if (iPart == 2) { pPart = "01"; pPartVariant = "01"; }
		if (iPart == 3) { pPart = "01"; pPartVariant = "01"; }
		if (iPart == 4) { pPart = "01"; pPartVariant = "01"; }
		if (iPart == 5) { pPart = "01"; pPartVariant = "01"; }
	}
	if (iBase > 4)
	{
		// custom
		if (iPart == 1) { pPart = "01"; pPartVariant = "01"; }
		if (iPart == 2) { pPart = "01"; pPartVariant = "01"; } 
		if (iPart == 3) { pPart = "01"; pPartVariant = "01"; }
		if (iPart == 4) { pPart = "01"; pPartVariant = "01"; }
	}
	strcpy (pPartNumStr, pPart);
	if ( pPartNumVariantStr ) strcpy (pPartNumVariantStr, pPartVariant);
}

void charactercreatorplus_preloadallcharacterbasedefaults(void)
{
	int iBaseCount = g_CharacterType.size();
	for (int base = 1; base <= iBaseCount; base++)
	{
		LPSTR pBase = NULL;
		char pRelFile[MAX_PATH];
		pBase = g_CharacterType[base-1].pPartsFolder;

		//PE: Only preload from the current selected Type (when changing type we reset release the old textures anyway).
		if( pestrcasestr(CCP_Type,pBase))
		{
			for (int part = 1; part <= 5; part++)
			{
				char pPart[1024];
				strcpy(pPart, "");
				char pPartNum[32];
				charactercreatorplus_GetDefaultCharacterPartNum(base, part, pPartNum);
				if (part == 1) sprintf(pPart, "body %s", pPartNum);
				if (part == 2) sprintf(pPart, "head %s", pPartNum);
				if (part == 3) sprintf(pPart, "legs %s", pPartNum);
				if (part == 4) sprintf(pPart, "feet %s", pPartNum);
				if (part == 5) sprintf(pPart, "hair %s", pPartNum);
				int count_textures_types = 6;
				count_textures_types = 3; //The others are not in wicked.
				for (int item = 1; item <= count_textures_types; item++)
				{
					LPSTR pItem = NULL;
					if (item == 1) pItem = "_color.dds";
					if (item == 2) pItem = "_normal.dds";
					if (item == 3) pItem = "_mask.dds";
					if (item == 4) pItem = "_ao.dds";
					if (item == 5) pItem = "_metalness.dds";
					if (item == 6) pItem = "_gloss.dds";
					sprintf(pRelFile, "charactercreatorplus\\parts\\%s\\%s %s%s", pBase, pBase, pPart, pItem);
					image_preload_files_add(pRelFile);
				}
			}
		}
	}
}

void charactercreatorplus_preloadallcharacterpartchoices ( void )
{
	image_preload_files_start();
	static std::map<std::string, std::string> CharacterCreatorCurrent_s;
	for (int part_loop = 0; part_loop < 10; part_loop++) 
	{
		if (part_loop == 0) CharacterCreatorCurrent_s = CharacterCreatorHeadGear_s;
		if (part_loop == 1) CharacterCreatorCurrent_s = CharacterCreatorHair_s;
		if (part_loop == 2) CharacterCreatorCurrent_s = CharacterCreatorHead_s;
		if (part_loop == 3) CharacterCreatorCurrent_s = CharacterCreatorEyeglasses_s;
		if (part_loop == 4) CharacterCreatorCurrent_s = CharacterCreatorFacialHair_s;
		if (part_loop == 5) CharacterCreatorCurrent_s = CharacterCreatorBody_s;
		if (part_loop == 6) CharacterCreatorCurrent_s = CharacterCreatorLegs_s;
		if (part_loop == 7) CharacterCreatorCurrent_s = CharacterCreatorFeet_s;
		if (part_loop == 8) CharacterCreatorCurrent_s = CharacterCreatorAccessory1_s;
		if (part_loop == 9) CharacterCreatorCurrent_s = CharacterCreatorAccessory2_s;
		
		if (!CharacterCreatorCurrent_s.empty())
		{
			for (std::map<std::string, std::string>::iterator it = CharacterCreatorCurrent_s.begin(); it != CharacterCreatorCurrent_s.end(); ++it)
			{
				std::string full_path = it->second;
				std::string name = it->first;
				char pFullBaseFilename[2048];
				strcpy ( pFullBaseFilename, full_path.c_str() );
				strcat ( pFullBaseFilename, name.c_str());

				// ignore None entries in the list
				if (strnicmp(name.c_str(), "None", 4) != NULL)
				{
					// detect color variant
					char pFullBaseVariantFilename[2048];
					strcpy(pFullBaseVariantFilename, pFullBaseFilename);
					char pLastLetter = pFullBaseFilename[strlen(pFullBaseFilename) - 1];
					if (pLastLetter >= 'a' && pLastLetter <= 'z')
					{
						// using a color variant
						strcpy(pFullBaseVariantFilename, pFullBaseFilename);
						pFullBaseFilename[strlen(pFullBaseFilename) - 1] = 0;
					}
					char pWorkFile[2038];
					strcpy(pWorkFile, pFullBaseFilename); strcat(pWorkFile, CCPMODELEXT);
					object_preload_files_add(pWorkFile);
					strcpy(pWorkFile, pFullBaseFilename); strcat(pWorkFile, "_normal.dds");
					image_preload_files_add(pWorkFile);
					strcpy(pWorkFile, pFullBaseFilename); strcat(pWorkFile, "_mask.dds");
					image_preload_files_add(pWorkFile);
				}
			}
		}
	}
	// also add in any base model defaults
	charactercreatorplus_preloadallcharacterbasedefaults();
	image_preload_files_finish();
}

void charactercreatorplus_preparechange(char *path, int part, char* tag)
{
	g_charactercreatorplus_preloading = true;
	strcpy(g_charactercreatorplus_path, path);
	g_charactercreatorplus_part = part;
	strcpy(g_charactercreatorplus_tag, tag);
	DisplaySmallImGuiMessage("Loading ...");
}

void charactercreatorplus_waitforpreptofinish(void)
{
	if (g_charactercreatorplus_preloading == true)
	{
		DisplaySmallImGuiMessage("Loading ...");
	}
	if (g_charactercreatorplus_preloading == true)
	{
		if (image_preload_files_in_progress()==false && object_preload_files_in_progress()==false)
		{
			image_preload_files_wait();
			image_preload_files_reset(); //PE: Make sure to free textures, as the list will get cleared later.
			object_preload_files_wait();
			charactercreatorplus_change(g_charactercreatorplus_path, g_charactercreatorplus_part, g_charactercreatorplus_tag);
			charactercreatorplus_preloadallcharacterpartchoices();
			g_charactercreatorplus_preloading = false;
		}
	}
}

void charactercreatorplus_refreshskincolor(void)
{
	// wicked not using custom character shader, so create a new albedo/color texture
	// using template mask and skin color texture to produce correct albedo representing skin
	// only needed to do for body, legs and feet
	int iCharTexture = g.charactercreatorEditorImageoffset + 1;
	int iCharLegsTexture = g.charactercreatorEditorImageoffset + 61;
	int iCharFeetTexture = g.charactercreatorEditorImageoffset + 71;
	int iCharSkinTexture = g.charactercreatorEditorImageoffset + 101;
	
	// find a free work memblock
	int iMemblockAlbedoID = 32; while (MemblockExist(iMemblockAlbedoID) == 1) iMemblockAlbedoID++;
	int iMemblockMaskID = 33; while (MemblockExist(iMemblockMaskID) == 1) iMemblockMaskID++;
	int iMemblockSkinID = 34; while (MemblockExist(iMemblockSkinID) == 1) iMemblockSkinID++;

	// for each relevant body part
	for ( int partnum = 0; partnum < 3; partnum++ )
	{
		// determine which body part to work on
		int iPartObj, iAlbedoTexture;
		if (partnum == 0) { iPartObj = iCharObj; iAlbedoTexture = iCharTexture + 0; }
		if (partnum == 1) { iPartObj = iCharObjLegs; iAlbedoTexture = iCharLegsTexture + 0; }
		if (partnum == 2) { iPartObj = iCharObjFeet; iAlbedoTexture = iCharFeetTexture + 0; }
		int iMaskTexture;
		if (partnum == 0) { iMaskTexture = iCharTexture + 1; }
		if (partnum == 1) { iMaskTexture = iCharLegsTexture + 1; }
		if (partnum == 2) { iMaskTexture = iCharFeetTexture + 1; }

		if (!ImageExist(iAlbedoTexture))
		{
			printf("tmp");
		}

		// ensure mask available
		if ( ImageExist(iMaskTexture))
		{
			// make sure images are not compressed (i.e. DXT1-5), but regular XRGB so we can read them
			char pNewTempAlbedoTextureFile[MAX_PATH];
			sprintf(pNewTempAlbedoTextureFile, "charactercreatorplus\\skins\\tempfinalalbedo.png");
			GG_GetRealPath(pNewTempAlbedoTextureFile, 1);

			//PE: No need if already in DXGI_FORMAT_R8G8B8A8_UNORM
			int imgformat = ImageFormat(iAlbedoTexture);
			if (imgformat != DXGIFORMATR8G8B8A8UNORM)
			{
				SaveImage(pNewTempAlbedoTextureFile, iAlbedoTexture);
				DeleteImage(iAlbedoTexture);
				LoadImage(pNewTempAlbedoTextureFile, iAlbedoTexture);
			}
			char pNewTempMaskTextureFile[MAX_PATH];
			sprintf(pNewTempMaskTextureFile, "charactercreatorplus\\skins\\tempfinalmask.png");
			GG_GetRealPath(pNewTempMaskTextureFile, 1);
			
			//PE: No need if already in DXGI_FORMAT_R8G8B8A8_UNORM
			imgformat = ImageFormat(iMaskTexture);
			if (imgformat != DXGIFORMATR8G8B8A8UNORM)
			{
				SaveImage(pNewTempMaskTextureFile, iMaskTexture);
				DeleteImage(iMaskTexture);
				LoadImage(pNewTempMaskTextureFile, iMaskTexture);
			}

			// load image data into appropriate memblocks
			if (MemblockExist(iMemblockAlbedoID) == 1) DeleteMemblock(iMemblockAlbedoID);
			if (MemblockExist(iMemblockMaskID) == 1) DeleteMemblock(iMemblockMaskID);
			if (MemblockExist(iMemblockSkinID) == 1) DeleteMemblock(iMemblockSkinID);
			CreateMemblockFromImage(iMemblockAlbedoID, iAlbedoTexture);
			CreateMemblockFromImage(iMemblockMaskID, iMaskTexture);
			CreateMemblockFromImage(iMemblockSkinID, iCharSkinTexture);

			if (MemblockExist(iMemblockAlbedoID) == 0)
			{
				//PE: Fail.
				int imgformat = ImageFormat(iAlbedoTexture);
				if (!ImageExist(iAlbedoTexture))
				{
					printf("tmp");
				}
			}
			// skin mask may have different resolution
			int imgSkinMaskWidth = ReadMemblockDWord(iMemblockMaskID, 0);
			int imgSkinMaskHeight = ReadMemblockDWord(iMemblockMaskID, 4);
			int imgSkinMaskDepth = ReadMemblockDWord(iMemblockMaskID, 8);
			int imgSkinMaskSize = imgSkinMaskWidth * imgSkinMaskHeight * imgSkinMaskDepth;
			int imgSkinMaskOffset = 4 * 3;

			// and skin ref may be different size also (usually 2K)
			int imgSkinWidth = ReadMemblockDWord(iMemblockSkinID, 0);
			int imgSkinHeight = ReadMemblockDWord(iMemblockSkinID, 4);
			int imgSkinDepth = ReadMemblockDWord(iMemblockSkinID, 8);
			int imgSkinSize = imgSkinWidth * imgSkinHeight * imgSkinDepth;
			int imgSkinOffset = 4 * 3;

			// create new albedo from mask and skin texture
			int imgWidth = ReadMemblockDWord(iMemblockAlbedoID, 0);
			int imgHeight = ReadMemblockDWord(iMemblockAlbedoID, 4);
			int imgDepth = ReadMemblockDWord(iMemblockAlbedoID, 8);
			int imgSize = imgWidth * imgHeight * imgDepth;
			int imgOffset = 4 * 3;

			// used to calculate actual skinmask coords offset
			float imgSkinMaskXDiv = (float)imgSkinMaskWidth / (float)imgWidth;
			float imgSkinMaskYDiv = (float)imgSkinMaskHeight / (float)imgHeight;
			float imgSkinXDiv = (float)imgSkinWidth / (float)imgWidth;
			float imgSkinYDiv = (float)imgSkinHeight / (float)imgHeight;

			// replace all skin pixels using mask
			for (int y = 0; y < imgHeight - 1; y++)
			{
				for (int x = 0; x < imgWidth - 1; x++)
				{
					// skin mask can be different size than color texture (i.e. 1K mask + 4K color texture)
					if (imgSize != imgSkinMaskSize)
					{
						int iXOffset = x*imgSkinMaskXDiv;
						int iYOffset = y*imgSkinMaskYDiv;
						int iXYOffset = iXOffset + (iYOffset*(imgSkinMaskWidth-1));
						imgSkinMaskOffset = (4 * 3) + (iXYOffset *4);
					}
					else
					{
						imgSkinMaskOffset = imgOffset;
					}
					if (imgSize != imgSkinSize)
					{
						int iXOffset = x * imgSkinXDiv;
						int iYOffset = y * imgSkinYDiv;
						int iXYOffset = iXOffset + (iYOffset*(imgSkinWidth-1));
						imgSkinOffset = (4 * 3) + (iXYOffset * 4);
					}
					else
					{
						imgSkinOffset = imgOffset;
					}
					int pixelCol = ReadMemblockDWord(iMemblockAlbedoID, imgOffset);
					int maskColR = ReadMemblockByte(iMemblockMaskID, imgSkinMaskOffset+0);
					int maskColG = ReadMemblockByte(iMemblockMaskID, imgSkinMaskOffset+1);
					int maskColB = ReadMemblockByte(iMemblockMaskID, imgSkinMaskOffset+2);
					int maskColA = ReadMemblockByte(iMemblockMaskID, imgSkinMaskOffset+3);
					int skinCol = ReadMemblockDWord(iMemblockSkinID, imgSkinOffset);
					if (maskColR != 255 || maskColG != 255 || maskColB != 255 || maskColA != 255)
					{
						// for some reason detecting red at zero did not work
						// need some more time to find out why the mask memblock returning strange values
						// i.e. 255,255,247,255 for the non-red part (255,255,255,255 for the rest)
					}
					else
					{
						// if mask red channel full, allow skin pixel to bake into main character texture
						int iA = ((DWORD)skinCol >> 24) & 0xFF;
						int iB = ((DWORD)skinCol >> 16) & 0xFF;
						int iG = ((DWORD)skinCol >> 8) & 0xFF;
						int iR = ((DWORD)skinCol >> 0) & 0xFF;
						//PE: The problem was CreateImageFromMemblock+SaveImage would switch the colors each time it was called :)
						DWORD newSkinCol = (iA << 24) + (iB << 16) + (iG << 8) + iR; // 
						WriteMemblockDWord(iMemblockAlbedoID, imgOffset, newSkinCol);
					}
					imgOffset = imgOffset + 4;
				}
			}
			
			// delete temp files
			DeleteFileA(pNewTempAlbedoTextureFile);
			DeleteFileA(pNewTempMaskTextureFile);

			// save the file (so can be loaded by wicked later)
			sprintf(pNewTempAlbedoTextureFile, "charactercreatorplus\\skins\\tempfinalalbedo%d.dds", partnum);
			GG_GetRealPath(pNewTempAlbedoTextureFile, 1);
			DeleteImage(iAlbedoTexture);

			extern bool g_bUseRGBAFormat;
			g_bUseRGBAFormat = true;
			CreateImageFromMemblock(iAlbedoTexture, iMemblockAlbedoID);

			SetImageName(iAlbedoTexture, pNewTempAlbedoTextureFile);

			//PE: CreateImageFromMemblock always convert to DXGI_FORMAT_B8G8R8A8_UNORM.
			//PE: We are now in DXGI_FORMAT_B8G8R8A8_UNORM. CreateImageFromMemblock just do a direct copy without color swap.
			//PE: Problem: Now that we are in DXGI_FORMAT_B8G8R8A8_UNORM and want to save as DXGI_FORMAT_R8G8B8A8_UNORM
			//PE: SaveImage will swap around the colors, thats a problem. each time this is called colors are swapped.
			//PE: Use g_bUseRGBAFormat=true to prevent this.
			g_bUseRGBAFormat = false;

			SaveImage(pNewTempAlbedoTextureFile, iAlbedoTexture);

			// rare event where a texture file can be different things , so ensure
			// the image manager deletes the image entry for this image before a new attempt 
			// to load it happens
			WickedCall_DeleteImage(pNewTempAlbedoTextureFile);

			// load the file and apply to the object
			TextureObject(iPartObj, 0, iAlbedoTexture);
		}
	}
	if (MemblockExist(iMemblockAlbedoID) == 1) DeleteMemblock(iMemblockAlbedoID);
	if (MemblockExist(iMemblockMaskID) == 1) DeleteMemblock(iMemblockMaskID);
	if (MemblockExist(iMemblockSkinID) == 1) DeleteMemblock(iMemblockSkinID);
}

void charactercreatorplus_loadccimages(LPSTR pVariantColorPartFile, LPSTR pPartFile, int iTextureBase)
{
	// variant color can be different from rest of texture files (03a, 03b, etc)
	cstr pVariantColorPath = pVariantColorPartFile;
	cstr pPath = pPartFile;

	// Load needed albedo and normal textures, and emissive if present
	LoadImage(cstr(pVariantColorPath + "_color.dds").Get(), iTextureBase + 0);
	LoadImage(cstr(pPath + "_mask.dds").Get(), iTextureBase + 1); // was +5 in VRQV3 (now taken by emissive below)
	LoadImage(cstr(pPath + "_normal.dds").Get(), iTextureBase + 2);
	LoadImage(cstr(pPath + "_emissive.dds").Get(), iTextureBase + 5);

	// LB: Generate required surface texture file if not exist
	cstr pAOFile = pPath + "_ao.dds";
	cstr pGlossFile = pPath + "_gloss.dds";
	cstr pMetalessFile = pPath + "_metalness.dds";
	cstr pSurfaceFile = pPath + "_surface.dds";
	if (FileExist(pSurfaceFile.Get()) == 0)
	{
		// need to generate surface file, and then load it
		if (FileExist(pAOFile.Get()) == 0) pAOFile = "effectbank\\reloaded\\media\\blank_O.dds";
		ImageCreateSurfaceTexture(pSurfaceFile.Get(), pAOFile.Get(), pGlossFile.Get(), pMetalessFile.Get());
	}
	LoadImage(pSurfaceFile.Get(), iTextureBase + 6);
}

void charactercreatorplus_textureccimages(int iThisObj, int iTextureBase)
{
	// LB: Paul's new arrangement replaces old layout
	TextureObject(iThisObj, GG_MESH_TEXTURE_SURFACE, iTextureBase + 6);
	TextureObject(iThisObj, GG_MESH_TEXTURE_DIFFUSE, iTextureBase + 0);
	TextureObject(iThisObj, GG_MESH_TEXTURE_NORMAL, iTextureBase + 2);
	TextureObject(iThisObj, GG_MESH_TEXTURE_EMISSIVE, iTextureBase + 5);
}

void charactercreatorplus_change(char *path, int part, char* tag)
{
	// need legacy loading for image reskinning work
	image_setlegacyimageloading(true);

	// part = 67 is a special code to update both legs and feet (for cascade system when body/legs force other part changes)
	int iCharTexture = g.charactercreatorEditorImageoffset + 1;
	int iCharHeadGearTexture = g.charactercreatorEditorImageoffset + 11;
	int iCharHairTexture = g.charactercreatorEditorImageoffset + 21;
	int iCharHeadTexture = g.charactercreatorEditorImageoffset + 31;
	int iCharEyeglassesTexture = g.charactercreatorEditorImageoffset + 41;
	int iCharFacialHairTexture = g.charactercreatorEditorImageoffset + 51;
	int iCharLegsTexture = g.charactercreatorEditorImageoffset + 61;
	int iCharFeetTexture = g.charactercreatorEditorImageoffset + 71;
	int iCharAccessory1Texture = g.charactercreatorEditorImageoffset + 81;
	int iCharAccessory2Texture = g.charactercreatorEditorImageoffset + 91;

	// skin override texture
	int iCharSkinTexture = g.charactercreatorEditorImageoffset + 101;

	// free main character object to recreate here
	if (ObjectExist(iCharObj)) DeleteObject(iCharObj);

	// final part name
	cstr final_name = path, tmp;

	// Make sure that there is always a valid skin texture for auto-swapping.
	if (strlen(g_SkinTextureStorage) == 0)
		strcpy(g_SkinTextureStorage, cstr(final_name + cSelectedHead).Get());

	char pPreviousHeadChoice[260];

	if (part == 0 || part == 2)
	{
		if (part == 0)
		{
			// User chose new headgear, so revert any swapped parts back to their original state.
			if (g_previousAutoSwap)
				charactercreatorplus_restoreswappedparts();
		}

		g_pLastHeadgearAutoSwap = nullptr;
	}

	// Check for any parts that would clip through the newly chosen part, and swap as needed.
	charactercreatorplus_performautoswap(part);

	if (g_previousAutoSwap != nullptr)
	{
		// Check if the part that is being changed was part of an auto-swap, or chosen specifically by the user.
		int iUserChoseCategory = -1;
		for (int i = 0; i < 10; i++)
		{
			// Find the category in the previous auto-swap data that needs updating.
			if (part == i && g_iPartsThatNeedReloaded[i] == 0)
			{
				iUserChoseCategory = i;
				break;
			}
		}

		// If the user chose this part specifically, ensure that the previous auto-swap data is updated to reflect this...
		// ...This is done because every time a new auto-swap is performed, the previous swap is undone, to ensure the user does not lose the choice they originally made.
		if (iUserChoseCategory >= 0)
		{
			char* pPartToSwap = nullptr;
			for (int i = 0; i < g_previousAutoSwap->requiredSwapCategories.size(); i++)
			{
				if (g_previousAutoSwap->requiredSwapCategories[i] == iUserChoseCategory)
				{
					// Get the correct part type to swap.
					switch (g_previousAutoSwap->requiredSwapCategories[i])
					{
					case 0: pPartToSwap = cSelectedHeadGear; break;
					case 1: pPartToSwap = cSelectedHair; break;
					case 2: pPartToSwap = cSelectedHead; break;
					case 3: pPartToSwap = cSelectedEyeglasses; break;
					case 4: pPartToSwap = cSelectedFacialHair; break;
					case 5: pPartToSwap = cSelectedBody; break;
					case 6: pPartToSwap = cSelectedLegs; break;
					case 7: pPartToSwap = cSelectedFeet; break;
					case 8: pPartToSwap = cSelectedAccessory1; break;
					case 9: pPartToSwap = cSelectedAccessory2; break;
					}
					g_previousAutoSwap->swappedPartNames[i] = pPartToSwap;
					break;
				}
			}

			// User chose a new head, if there is an active headgear auto-swap, then reswap the head if needed.
			if (iUserChoseCategory > 0 && iUserChoseCategory < 5)
			{
				for (int i = 0; i < g_previousAutoSwap->requiredSwapCategories.size(); i++)
				{
					if (g_previousAutoSwap->requiredSwapCategories[i] == 2)
					{
						// The auto-swap data contained a head, so ensure we re-auto-swap if possible.
						for (const auto& annotation : CharacterCreatorAnnotatedHead_s)
						{
							if (strstr(annotation.second.c_str(), g_previousAutoSwap->requiredSwapNames[i].c_str()))
							{
								strcpy(pPreviousHeadChoice, cSelectedHead);
								strcpy(cSelectedHead, annotation.first.c_str());
								g_pLastHeadgearAutoSwap = g_previousAutoSwap;
							}
						}
						g_iPartsThatNeedReloaded[2] = 1;
						break;
					}
				}
			}
		}
	}

	// detect color variant
	int iEnd = 0;
	char cLast = 0;
	char cUseHeadGear[260];
	char cUseHair[260];
	char cUseHead[260];
	char cUseEyeglasses[260];
	char cUseFacialHair[260];
	char cUseBody[260];
	char cUseLegs[260];
	char cUseFeet[260];
	char cUseAccessory1[260];
	char cUseAccessory2[260];
	strcpy(cUseHeadGear, cSelectedHeadGear);
	strcpy(cUseHair, cSelectedHair);
	strcpy(cUseHead, cSelectedHead);
	strcpy(cUseEyeglasses, cSelectedEyeglasses);
	strcpy(cUseFacialHair, cSelectedFacialHair);
	strcpy(cUseBody, cSelectedBody);
	strcpy(cUseLegs, cSelectedLegs);
	strcpy(cUseFeet, cSelectedFeet);
	strcpy(cUseAccessory1, cSelectedAccessory1);
	strcpy(cUseAccessory2, cSelectedAccessory2);

	char cSelectedVariantHeadGear[260];
	char cSelectedVariantHair[260];
	char cSelectedVariantHead[260];
	char cSelectedVariantEyeglasses[260];
	char cSelectedVariantFacialHair[260];
	char cSelectedVariantBody[260];
	char cSelectedVariantLegs[260];
	char cSelectedVariantFeet[260];
	char cSelectedVariantAccessory1[260];
	char cSelectedVariantAccessory2[260];

	strcpy(cSelectedVariantHeadGear, cSelectedHeadGear);
	strcpy(cSelectedVariantHair, cSelectedHair);
	strcpy(cSelectedVariantHead, cSelectedHead);
	strcpy(cSelectedVariantEyeglasses, cSelectedEyeglasses);
	strcpy(cSelectedVariantFacialHair, cSelectedFacialHair);
	strcpy(cSelectedVariantBody, cSelectedBody);
	strcpy(cSelectedVariantLegs, cSelectedLegs);
	strcpy(cSelectedVariantFeet, cSelectedFeet);
	strcpy(cSelectedVariantAccessory1, cSelectedAccessory1);
	strcpy(cSelectedVariantAccessory2, cSelectedAccessory2);

	iEnd = strlen(cSelectedHeadGear) - 1; cLast = cSelectedHeadGear[iEnd]; if (iEnd > 3 && cLast >= 'a' && cLast <= 'z') { cUseHeadGear[iEnd] = 0; }
	iEnd = strlen(cSelectedHair) - 1; cLast = cSelectedHair[iEnd]; if (iEnd > 3 && cLast >= 'a' && cLast <= 'z') { cUseHair[iEnd] = 0; }
	iEnd = strlen(cSelectedHead) - 1; cLast = cSelectedHead[iEnd]; if (iEnd > 3 && cLast >= 'a' && cLast <= 'z') { cUseHead[iEnd] = 0; }
	iEnd = strlen(cSelectedEyeglasses) - 1; cLast = cSelectedEyeglasses[iEnd]; if (iEnd > 3 && cLast >= 'a' && cLast <= 'z') { cUseEyeglasses[iEnd] = 0; }
	iEnd = strlen(cSelectedFacialHair) - 1; cLast = cSelectedFacialHair[iEnd]; if (iEnd > 3 && cLast >= 'a' && cLast <= 'z') { cUseFacialHair[iEnd] = 0; }
	iEnd = strlen(cSelectedBody) - 1; cLast = cSelectedBody[iEnd]; if (iEnd > 3 && cLast >= 'a' && cLast <= 'z') { cUseBody[iEnd] = 0; }
	iEnd = strlen(cSelectedLegs) - 1; cLast = cSelectedLegs[iEnd]; if (iEnd > 3 && cLast >= 'a' && cLast <= 'z') { cUseLegs[iEnd] = 0; }
	iEnd = strlen(cSelectedFeet) - 1; cLast = cSelectedFeet[iEnd]; if (iEnd > 3 && cLast >= 'a' && cLast <= 'z') { cUseFeet[iEnd] = 0; }
	iEnd = strlen(cSelectedAccessory1) - 1; cLast = cSelectedAccessory1[iEnd]; if (iEnd > 3 && cLast >= 'a' && cLast <= 'z') { cUseAccessory1[iEnd] = 0; }
	iEnd = strlen(cSelectedAccessory2) - 1; cLast = cSelectedAccessory2[iEnd]; if (iEnd > 3 && cLast >= 'a' && cLast <= 'z') { cUseAccessory2[iEnd] = 0; }

	//Load all objects.
	if (strnicmp(cUseHeadGear, "None", 4) != NULL)
	{
		tmp = final_name + cUseHeadGear + CCPMODELEXT;
		LoadObject(tmp.Get(), iCharObjHeadGear);

		// Make the balaclava double-sided to prevent users seeing gaps at the eyes.
		if (strcmp(cSelectedHeadGear, "adult male headgear 15") == NULL)
		{
			sObject* pObject = GetObjectData(iCharObjHeadGear);
			if (pObject) pObject->ppMeshList[0]->bCull = false;
		}
	}
	else
	{
		if (ObjectExist(iCharObjHeadGear)) DeleteObject(iCharObjHeadGear);
	}
	if (strnicmp(cUseHair, "None", 4) != NULL)
	{
		tmp = final_name + cUseHair + CCPMODELEXT;
		LoadObject(tmp.Get(), iCharObjHair);
	}
	else
	{
		if (ObjectExist(iCharObjHair)) DeleteObject(iCharObjHair);
	}
	tmp = final_name + cUseHead + CCPMODELEXT;
	LoadObject(tmp.Get(), iCharObjHead);

	if (strnicmp(cUseEyeglasses, "None", 4) != NULL)
	{
		tmp = final_name + cUseEyeglasses + CCPMODELEXT;
		LoadObject(tmp.Get(), iCharObjEyeglasses);
	}
	else
	{
		if (ObjectExist(iCharObjEyeglasses)) DeleteObject(iCharObjEyeglasses);
	}

	if (strnicmp(cUseAccessory1, "None", 4) != NULL)
	{
		tmp = final_name + cUseAccessory1 + CCPMODELEXT;
		LoadObject(tmp.Get(), iCharObjAccessory1);
	}
	else
	{
		if (ObjectExist(iCharObjAccessory1)) DeleteObject(iCharObjAccessory1);
	}
	if (strnicmp(cUseAccessory2, "None", 4) != NULL)
	{
		tmp = final_name + cUseAccessory2 + CCPMODELEXT;
		LoadObject(tmp.Get(), iCharObjAccessory2);
	}
	else
	{
		if (ObjectExist(iCharObjAccessory2)) DeleteObject(iCharObjAccessory2);
	}

	if (strnicmp(cUseFacialHair, "None", 4) != NULL)
	{
		tmp = final_name + cUseFacialHair + CCPMODELEXT;
		LoadObject(tmp.Get(), iCharObjFacialHair);
	}
	else
	{
		if (ObjectExist(iCharObjFacialHair)) DeleteObject(iCharObjFacialHair);
	}

	// BODY
	tmp = final_name + cUseBody + CCPMODELEXT;
	LoadObject(tmp.Get(), iCharObj);

	// LEGS
	tmp = final_name + cUseLegs + CCPMODELEXT;
	LoadObject(tmp.Get(), iCharObjLegs);

	// FEET
	tmp = final_name + cUseFeet + CCPMODELEXT;
	LoadObject(tmp.Get(), iCharObjFeet);

	// load all required textures
	if ((part == 0 || g_iPartsThatNeedReloaded[0] == 1 || part == -1) && ObjectExist(iCharObjHeadGear) == 1)
	{
		// HeadGear
		for (int a = 0; a < 5; a++) if (GetImageExistEx(iCharHeadGearTexture + a)) DeleteImage(iCharHeadGearTexture + a);

		// load textures
		cstr VariantColorPath_s = final_name + cSelectedVariantHeadGear;
		cstr Path_s = final_name + cUseHeadGear;
		charactercreatorplus_loadccimages(VariantColorPath_s.Get(), Path_s.Get(), iCharHeadGearTexture);
	}
	if ((part == 1 || g_iPartsThatNeedReloaded[1] == 1 || part == -1) && ObjectExist(iCharObjHair) == 1)
	{
		// Hair
		for (int a = 0; a < 5; a++) if (GetImageExistEx(iCharHairTexture + a)) DeleteImage(iCharHairTexture + a);

		// load textures
		cstr VariantColorPath_s = final_name + cSelectedVariantHair;
		cstr Path_s = final_name + cUseHair;
		charactercreatorplus_loadccimages(VariantColorPath_s.Get(), Path_s.Get(), iCharHairTexture);
	}
	if ((part == 2 || g_iPartsThatNeedReloaded[2] == 1 || part == -1) && ObjectExist(iCharObjHead) == 1)
	{
		// Head
		for (int a = 0; a < 5; a++) if (GetImageExistEx(iCharHeadTexture + a)) DeleteImage(iCharHeadTexture + a);

		// load textures
		cstr VariantColorPath_s = final_name + cSelectedVariantHead;
		cstr Path_s = final_name + cUseHead;

		// Head is being changed as part of an auto-swap, so the skin tone must be matched to whatever it was before.
		if (part != 2 && g_iPartsThatNeedReloaded[2] == 1)
			strcpy(VariantColorPath_s.Get(), g_SkinTextureStorage);
		else if (part == 2 && g_iPartsThatNeedReloaded[2] == 1)
		{
			// The head was chosen specifically by the user, but currently a headgear that requires an auto swap is equipped.
			VariantColorPath_s = cstr(final_name + pPreviousHeadChoice);
			strcpy(g_SkinTextureStorage, VariantColorPath_s.Get());
		}
		else
			strcpy(g_SkinTextureStorage, VariantColorPath_s.Get());

		charactercreatorplus_loadccimages(VariantColorPath_s.Get(), Path_s.Get(), iCharHeadTexture);

		// in addition, load in skin type to apply to rest of body
		if (tag && strnicmp(tag, "IC", 2) == NULL)
		{
			if (GetImageExistEx(iCharSkinTexture) == 1) DeleteImage(iCharSkinTexture);
			char pRelPathToSkinTexture[MAX_PATH];
			sprintf(pRelPathToSkinTexture, "charactercreatorplus\\skins\\%s.png", tag);
			LoadImage(pRelPathToSkinTexture, iCharSkinTexture);
		}
	}
	if ((part == 3 || g_iPartsThatNeedReloaded[3] == 1 || part == -1) && ObjectExist(iCharObjEyeglasses) == 1)
	{
		// Eyeglasses
		for (int a = 0; a < 5; a++) if (GetImageExistEx(iCharEyeglassesTexture + a)) DeleteImage(iCharEyeglassesTexture + a);

		// load textures
		cstr VariantColorPath_s = final_name + cSelectedVariantEyeglasses;
		cstr Path_s = final_name + cUseEyeglasses;
		charactercreatorplus_loadccimages(VariantColorPath_s.Get(), Path_s.Get(), iCharEyeglassesTexture);
	}

	if ((part == 8 || g_iPartsThatNeedReloaded[8] == 1 || part == -1) && ObjectExist(iCharObjAccessory1) == 1)
	{
		// Accessory1
		for (int a = 0; a < 5; a++) if (GetImageExistEx(iCharAccessory1Texture + a)) DeleteImage(iCharAccessory1Texture + a);

		// load textures
		cstr VariantColorPath_s = final_name + cSelectedVariantAccessory1;
		cstr Path_s = final_name + cUseAccessory1;
		charactercreatorplus_loadccimages(VariantColorPath_s.Get(), Path_s.Get(), iCharAccessory1Texture);
	}
	if ((part == 9 || g_iPartsThatNeedReloaded[9] == 1 || part == -1) && ObjectExist(iCharObjAccessory2) == 1)
	{
		// Accessory1
		for (int a = 0; a < 5; a++) if (GetImageExistEx(iCharAccessory2Texture + a)) DeleteImage(iCharAccessory2Texture + a);

		// load textures
		cstr VariantColorPath_s = final_name + cSelectedVariantAccessory2;
		cstr Path_s = final_name + cUseAccessory2;
		charactercreatorplus_loadccimages(VariantColorPath_s.Get(), Path_s.Get(), iCharAccessory2Texture);
	}

	if ((part == 4 || g_iPartsThatNeedReloaded[4] == 1 || part == -1) && ObjectExist(iCharObjFacialHair) == 1)
	{
		// Facial Hair
		for (int a = 0; a < 5; a++)	if (GetImageExistEx(iCharFacialHairTexture + a)) DeleteImage(iCharFacialHairTexture + a);

		// load textures
		cstr VariantColorPath_s = final_name + cSelectedVariantFacialHair;
		cstr Path_s = final_name + cUseFacialHair;
		charactercreatorplus_loadccimages(VariantColorPath_s.Get(), Path_s.Get(), iCharFacialHairTexture);
	}
	if ((part == 5 || g_iPartsThatNeedReloaded[5] == 1 || part == -1) && ObjectExist(iCharObj) == 1)
	{
		// Body
		for (int a = 0; a < 6; a++) if (GetImageExistEx(iCharTexture + a)) DeleteImage(iCharTexture + a);

		// load textures
		cstr VariantColorPath_s = final_name + cSelectedVariantBody;
		cstr Path_s = final_name + cUseBody;
		charactercreatorplus_loadccimages(VariantColorPath_s.Get(), Path_s.Get(), iCharTexture);

		// and need mask for reskin
		tmp = final_name + cUseBody + "_mask.dds";
		LoadImage(tmp.Get(), iCharTexture + 1);
		if (ImageExist(iCharTexture + 1) == 0)
		{
			// remove color-specific from part and look for base mask for this body part
			char pCropPartFile[MAX_PATH];
			strcpy(pCropPartFile, cUseBody);
			LPSTR pLastChar = pCropPartFile + strlen(pCropPartFile) - 1;
			if (*pLastChar >= 'a' && *pLastChar <= 'z') *pLastChar = 0;
			LoadImage(cstr(cstr(final_name) + pCropPartFile + "_mask.dds").Get(), iCharTexture + 1);
		}
	}
	if ((part == 6 || g_iPartsThatNeedReloaded[6] == 1 || part == -1 || part == 67) && ObjectExist(iCharObjLegs) == 1)
	{
		// Legs
		for (int a = 0; a < 6; a++) if (GetImageExistEx(iCharLegsTexture + a)) DeleteImage(iCharLegsTexture + a);

		// load textures
		cstr VariantColorPath_s = final_name + cSelectedVariantLegs;
		cstr Path_s = final_name + cUseLegs;
		charactercreatorplus_loadccimages(VariantColorPath_s.Get(), Path_s.Get(), iCharLegsTexture);

		// and need mask for reskin
		tmp = final_name + cUseLegs + "_mask.dds";
		LoadImage(tmp.Get(), iCharLegsTexture + 1);
		if (ImageExist(iCharLegsTexture + 1) == 0)
		{
			// remove color-specific from part and look for base mask for this body part
			char pCropPartFile[MAX_PATH];
			strcpy(pCropPartFile, cUseLegs);
			LPSTR pLastChar = pCropPartFile + strlen(pCropPartFile) - 1;
			if (*pLastChar >= 'a' && *pLastChar <= 'z') *pLastChar = 0;
			LoadImage(cstr(cstr(final_name) + pCropPartFile + "_mask.dds").Get(), iCharLegsTexture + 1);
		}
	}
	if ((part == 7 || g_iPartsThatNeedReloaded[7] == 1 || part == -1 || part == 67) && ObjectExist(iCharObjFeet) == 1)
	{
		// Feet
		for (int a = 0; a < 6; a++) if (GetImageExistEx(iCharFeetTexture + a)) DeleteImage(iCharFeetTexture + a);

		// load textures
		cstr VariantColorPath_s = final_name + cSelectedVariantFeet;
		cstr Path_s = final_name + cUseFeet;
		charactercreatorplus_loadccimages(VariantColorPath_s.Get(), Path_s.Get(), iCharFeetTexture);

		// and need mask for reskin
		tmp = final_name + cUseFeet + "_mask.dds";
		LoadImage(tmp.Get(), iCharFeetTexture + 1);
		if (ImageExist(iCharFeetTexture + 1) == 0)
		{
			// remove color-specific from part and look for base mask for this body part
			char pCropPartFile[MAX_PATH];
			strcpy(pCropPartFile, cUseFeet);
			LPSTR pLastChar = pCropPartFile + strlen(pCropPartFile) - 1;
			if (*pLastChar >= 'a' && *pLastChar <= 'z') *pLastChar = 0;
			LoadImage(cstr(cstr(final_name) + pCropPartFile + "_mask.dds").Get(), iCharFeetTexture + 1);
		}
	}

	// texture parts
	if (ObjectExist(iCharObjHeadGear) == 1) charactercreatorplus_textureccimages(iCharObjHeadGear, iCharHeadGearTexture);
	if (ObjectExist(iCharObjHair) == 1)	charactercreatorplus_textureccimages(iCharObjHair, iCharHairTexture);
	charactercreatorplus_textureccimages(iCharObjHead, iCharHeadTexture);
	if (ObjectExist(iCharObjEyeglasses) == 1) charactercreatorplus_textureccimages(iCharObjEyeglasses, iCharEyeglassesTexture);
	if (ObjectExist(iCharObjAccessory1) == 1) charactercreatorplus_textureccimages(iCharObjAccessory1, iCharAccessory1Texture);
	if (ObjectExist(iCharObjAccessory2) == 1) charactercreatorplus_textureccimages(iCharObjAccessory2, iCharAccessory2Texture);
	if (ObjectExist(iCharObjFacialHair) == 1) charactercreatorplus_textureccimages(iCharObjFacialHair, iCharFacialHairTexture);
	charactercreatorplus_textureccimages(iCharObj, iCharTexture);
	charactercreatorplus_textureccimages(iCharObjLegs, iCharLegsTexture);
	charactercreatorplus_textureccimages(iCharObjFeet, iCharFeetTexture);

	// wicked not using custom character shader, so create a new albedo/color texture
	// using template mask and skin color texture to produce correct albedo representing skin
	// only needed to do for body, legs and feet
	if (stricmp(CCP_Type, "zombie male") != NULL && stricmp(CCP_Type, "zombie female") != NULL)
	{
		charactercreatorplus_refreshskincolor();
	}

	// remove wicked resources before modifying objects below
	if (ObjectExist(iCharObj)) WickedCall_RemoveObject(GetObjectData(iCharObj));
	if (ObjectExist(iCharObjHeadGear)) WickedCall_RemoveObject(GetObjectData(iCharObjHeadGear));
	if (ObjectExist(iCharObjHead)) WickedCall_RemoveObject(GetObjectData(iCharObjHead));
	if (ObjectExist(iCharObjLegs)) WickedCall_RemoveObject(GetObjectData(iCharObjLegs));
	if (ObjectExist(iCharObjFeet)) WickedCall_RemoveObject(GetObjectData(iCharObjFeet));
	if (ObjectExist(iCharObjHair)) WickedCall_RemoveObject(GetObjectData(iCharObjHair));
	if (ObjectExist(iCharObjEyeglasses)) WickedCall_RemoveObject(GetObjectData(iCharObjEyeglasses));
	if (ObjectExist(iCharObjFacialHair)) WickedCall_RemoveObject(GetObjectData(iCharObjFacialHair));
	if (ObjectExist(iCharObjAccessory1)) WickedCall_RemoveObject(GetObjectData(iCharObjAccessory1));
	if (ObjectExist(iCharObjAccessory2)) WickedCall_RemoveObject(GetObjectData(iCharObjAccessory2));

	// stitch model together
	if (ObjectExist(iCharObjHeadGear) == 1)
	{
		// pregvents ugly shadow - can remove this when increase shadow quality on character renderings!
		SetObjectTransparency(iCharObjHeadGear, 2);
		StealMeshesFromObject(iCharObj, iCharObjHeadGear);
		DeleteObject(iCharObjHeadGear);
	}
	StealMeshesFromObject(iCharObj, iCharObjHead);
	DeleteObject(iCharObjHead);
	StealMeshesFromObject(iCharObj, iCharObjLegs);
	DeleteObject(iCharObjLegs);
	StealMeshesFromObject(iCharObj, iCharObjFeet);
	DeleteObject(iCharObjFeet);
	if (ObjectExist(iCharObjHair) == 1)
	{
		// ensure hair has no culling and semi-transparent
		SetObjectTransparency(iCharObjHair, 2);
		SetObjectCull(iCharObjHair, 0);
		StealMeshesFromObject(iCharObj, iCharObjHair);
		DeleteObject(iCharObjHair);
	}
	if (ObjectExist(iCharObjEyeglasses) == 1)
	{
		SetObjectCull(iCharObjEyeglasses, 0);
		SetObjectTransparency(iCharObjEyeglasses, 2);
		StealMeshesFromObject(iCharObj, iCharObjEyeglasses);
		DeleteObject(iCharObjEyeglasses);
	}

	if (ObjectExist(iCharObjAccessory1) == 1)
	{
		//SetObjectCull(iCharObjAccessory1, 0);
		//SetObjectTransparency(iCharObjAccessory1, 2);
		StealMeshesFromObject(iCharObj, iCharObjAccessory1);
		DeleteObject(iCharObjAccessory1);
	}
	if (ObjectExist(iCharObjAccessory2) == 1)
	{
		//SetObjectCull(iCharObjAccessory2, 0);
		//SetObjectTransparency(iCharObjAccessory2, 2);
		StealMeshesFromObject(iCharObj, iCharObjAccessory2);
		DeleteObject(iCharObjAccessory2);
	}

	if (ObjectExist(iCharObjFacialHair) == 1)
	{
		SetObjectTransparency(iCharObjFacialHair, 2);
		SetObjectCull(iCharObjFacialHair, 0);
		StealMeshesFromObject(iCharObj, iCharObjFacialHair);
		DeleteObject(iCharObjFacialHair);
	}

	// as character parts have no animations, wipe out ones they do have
	// and replace with the latest animation set for this base mesh (pistol is default)

	tmp = final_name + "default animations" + CCPMODELEXT;
	
	//PE: Was moved from "charactercreatorplus\parts\adult male\default animations.dbo" to "charactercreatorplus\\animations\\sets\\%s\\default animations.dbo"
	char pPathToWeaponAnim[MAX_PATH];
	sprintf(pPathToWeaponAnim, "charactercreatorplus\\animations\\sets\\%s\\default animations.dbo", CCP_Type);
	if (FileExist(pPathToWeaponAnim) == 1)
	{
		AppendObject(pPathToWeaponAnim, iCharObj, 0);
	}
	else if (FileExist(tmp.Get()))
	{
		AppendObject(tmp.Get(), iCharObj, 0);
	}
	// and trigger animation to be prepped
	g_bCharacterCreatorPrepAnims = true;

	// some DBOs are created with BLACK base color, so for character creator
	// objects set them always to WHITE with full ALPHA
	SetObjectDiffuseEx(iCharObj, 0xFFFFFFFF, 0);

	// this sequence is duplicated during the init, see if can merge into single function at some point!
	// character gone through extensive changes, ensure wicked is updated
	sObject* pObjectToRecreateInWicked = GetObjectData(iCharObj);
	WickedCall_AddObject(pObjectToRecreateInWicked);
	WickedCall_UpdateObject(pObjectToRecreateInWicked);
	WickedCall_TextureObject(pObjectToRecreateInWicked, NULL);

	// Change any mesh settings so they display correctly.
	for (int iMeshIndex = 0; iMeshIndex < pObjectToRecreateInWicked->iMeshCount; iMeshIndex++)
	{
		for (int i = 0; i < g_MeshesThatNeedDoubleSided.size(); i++)
		{
			if (strstr(pObjectToRecreateInWicked->ppMeshList[iMeshIndex]->pTextures[1].pName, g_MeshesThatNeedDoubleSided[i]))
			{
				sMesh* pMesh = pObjectToRecreateInWicked->ppMeshList[iMeshIndex];
				pMesh->bTransparency = false;
				pMesh->bCull = false;

				wiScene::MeshComponent* pMeshComponent = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);

				if (pMeshComponent)
				{
					pMeshComponent->SetDoubleSided(true);
					uint64_t materialEntity = pMeshComponent->subsets[0].materialID;
					wiScene::MaterialComponent* pMeshMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
					pMeshMaterial->alphaRef = 0.5f;
					pMeshMaterial->SetDirty();
					pMeshMaterial->SetDoubleSided();
					WickedCall_UpdateObject(pObjectToRecreateInWicked);
					pMeshMaterial->userBlendMode = BLENDMODE::BLENDMODE_OPAQUE;
				}

				break;
			}
		}

		// and set character to use full reflectance (as all body parts have alpha data in surface texture)
		// LB: reduced from 1.0 to 0.04 to reduce washed out look on no/minimal normal maps 
		//LB: Changed back to 0.04 reflectance to solve washed out look on surfaces with no/minimal normal map
		//WickedCall_SetReflectance(pObjectToRecreateInWicked->ppMeshList[iMeshIndex], 1.0f);
		char* pNameFromTexture = pObjectToRecreateInWicked->ppMeshList[iMeshIndex]->pTextures[0].pName;
		bool bNeedReflectance = false;
		if (pNameFromTexture && pestrcasestr(pNameFromTexture, "glasses")) bNeedReflectance = true;
		if (bNeedReflectance)
			WickedCall_SetReflectance(pObjectToRecreateInWicked->ppMeshList[iMeshIndex], 1.0f);
		else
			WickedCall_SetReflectance(pObjectToRecreateInWicked->ppMeshList[iMeshIndex], 0.04f);
	}

	for (int i = 0; i < 10; i++) g_iPartsThatNeedReloaded[i] = 0;

	// position final stitched character in scene
	float terrain_height = BT_GetGroundHeight(t.terrain.TerrainID, GGORIGIN_X, GGORIGIN_Z, 1);
	fCharObjectY = terrain_height;
	PositionObject(iCharObj, ccpObjTargetX, ccpObjTargetY, ccpObjTargetZ);
	RotateObject(iCharObj, ccpObjTargetAX, ccpObjTargetAY, ccpObjTargetAZ);
	SetObjectArtFlags(iCharObj, (1 << 1) + (0), 0);
	LoopObject(iCharObj, 15, 55);

	// set object default animation speed
	SetObjectSpeed(iCharObj, 100);

	// finished legacy loading requirements
	image_setlegacyimageloading(false);

	// Store the choices for each character so that they can be remembered after the user switches base type.
	if (stricmp(CCP_Type, "Adult Male") == NULL)
	{
		charactercreatorplus_copyselections(g_maleStorage);
	}
	else if (stricmp(CCP_Type, "Adult Female") == NULL)
	{
		charactercreatorplus_copyselections(g_femaleStorage);
	}
	else if (stricmp(CCP_Type, "Zombie") == NULL)
	{
		charactercreatorplus_copyselections(g_zombieStorage);
	}
	else
	{
		charactercreatorplus_copyselections(g_genericStorage);
	}

	if ((part == 0 || part == 2) && g_pLastHeadgearAutoSwap)
	{
		char cName[64];
		// Get the name of the currently selected head.
		for (const auto& annotation : CharacterCreatorAnnotatedHeadGear_s)
		{
			if (stricmp(annotation.first.c_str(), cSelectedHeadGear) == NULL)
			{
				strcpy(cName, annotation.second.c_str());
			}
		}

		// Check to see if the head was auto-swapped, if so we need to change the selected head, to hide the swap.
		for (int i = 0; i < g_pLastHeadgearAutoSwap->requiredSwapCategories.size(); i++)
		{
			if (g_pLastHeadgearAutoSwap->requiredSwapCategories[i] == 2)
				strcpy(cSelectedHead, g_pLastHeadgearAutoSwap->swappedPartNames[i].c_str());
		}
	}
}

void charactercreatorplus_loadannotationlist ( void )
{
	std::vector<std::string> annotatesFiles;

	// Find all of the annotates.txt files.
	for (int c = 1; c <= ChecklistQuantity(); c++)
	{
		cStr tfile_s = Lower(ChecklistString(c));
		if (tfile_s != "." && tfile_s != "..")
		{
			if (strstr(tfile_s.Get(), "annotates"))
			{
				if (strcmp(Right(tfile_s.Get(), 4), ".txt") == 0)
				{
					std::string file(tfile_s.Get());
					annotatesFiles.push_back(file);
				}
			}
		}
	}

	g_charactercreatorplus_annotation_list.clear();
	g_charactercreatorplus_annotationtag_list.clear();

	// Extract everything from all the annotates files.
	for (int i = 0; i < annotatesFiles.size(); i++)
	{
		LPSTR pFilename = (LPSTR)annotatesFiles[i].c_str();
		if (FileExist(pFilename) == 1)
		{
			OpenToRead(1, pFilename);
			while (FileEnd(1) == 0)
			{
				// get line by line
				cstr line_s = ReadString(1);
				LPSTR pLine = line_s.Get();

				// get field name
				char pFieldName[260];
				char pFieldValue[260];
				strcpy(pFieldName, pLine);
				LPSTR pEquals = strstr(pFieldName, "=");
				if (pEquals)
				{
					LPSTR pEqualsPos = pEquals;

					// field name - eat spaces at end of field name
					pEquals--;
					while (pEquals > pFieldName && *pEquals == 32) pEquals--;
					*(pEquals + 1) = 0;

					// rest is field value
					strcpy(pFieldValue, pEqualsPos + 2);

					// strip off any tags
					char tag[260];
					strcpy(tag, "");
					for (int tagn = 0; tagn < strlen(pFieldValue); tagn++)
					{
						if (pFieldValue[tagn] == '[')
						{
							strcpy(tag, pFieldValue + tagn + 1);
							pFieldValue[tagn] = 0;
						}
					}
					while (strlen(tag) > 0 && (tag[strlen(tag) - 1] == ' ' || tag[strlen(tag) - 1] == ']')) tag[strlen(tag) - 1] = 0;

					// add to good list of annotations
					g_charactercreatorplus_annotation_list.insert(std::make_pair(pFieldName, pFieldValue));
					g_charactercreatorplus_annotationtag_list.insert(std::make_pair(pFieldName, tag));
				}
			}

			// close file handling
			CloseFile(1);
		}
	}
}

LPSTR charactercreatorplus_findannotation ( LPSTR pSearchStr )
{
	LPSTR pNewString = NULL;
	for ( std::map<std::string, std::string>::iterator it = g_charactercreatorplus_annotation_list.begin(); it != g_charactercreatorplus_annotation_list.end(); ++it)
	{
		std::string field = it->first;
		if ( stricmp ( field.c_str(), pSearchStr ) == NULL )
		{
			pNewString = (LPSTR)it->second.c_str();
			break;
		}
	}
	return pNewString;
}

LPSTR charactercreatorplus_findannotationtag ( LPSTR pSearchStr )
{
	LPSTR pNewString = NULL;
	for ( std::map<std::string, std::string>::iterator it = g_charactercreatorplus_annotationtag_list.begin(); it != g_charactercreatorplus_annotationtag_list.end(); ++it)
	{
		std::string field = it->first;
		if ( stricmp ( field.c_str(), pSearchStr ) == NULL )
		{
			pNewString = (LPSTR)it->second.c_str();
			break;
		}
	}
	return pNewString;
}

void charactercreatorplus_extractpartnumberandvariation(const char* source, char* number, char* variation)
{
	char temp[5];

	if (stricmp(source, "None") == NULL || strlen(source) == 0)
	{
		number[0] = 0;
		variation[0] = 0;
		return;
	}

	for (int i = strlen(source) - 1; i >= 0; i--)
	{
		if (source[i] == ' ')
		{
			strcpy(temp, source + i + 1);
			break;
		}
	}

	if (strlen(temp) > 0)
	{
		strcpy(variation, temp);

		if (strlen(temp) == 3)
		{
			temp[2] = 0;
		}
		
		strcpy(number, temp);
	}
}

int GetBaseValueFromCCPType(LPSTR pCCP_Type)
{
	for (int n = 0; n < g_CharacterType.size(); n++)
	{
		if (stricmp (pCCP_Type, g_CharacterType[n].pPartsFolder) == NULL)
		{
			return 1 + n;
		}
	}
	return 1;
}

void charactercreatorplus_refreshtype(void)
{
	// need legacy loading for image reskinning work
	image_setlegacyimageloading(true);

	// ensure we have all textures of character creator init before we start
	image_preload_files_wait();
	object_preload_files_wait();

	// reset character base mesh and textures based on type stored in CCP_Type
	CharacterCreatorHeadGear_s.clear();
	CharacterCreatorHair_s.clear();
	CharacterCreatorHead_s.clear();
	CharacterCreatorEyeglasses_s.clear();
	CharacterCreatorFacialHair_s.clear();
	CharacterCreatorBody_s.clear();
	CharacterCreatorLegs_s.clear();
	CharacterCreatorFeet_s.clear();
	CharacterCreatorAccessory1_s.clear();
	CharacterCreatorAccessory2_s.clear();

	CharacterCreatorAnnotatedHeadGear_s.clear();
	CharacterCreatorAnnotatedHair_s.clear();
	CharacterCreatorAnnotatedHead_s.clear();
	CharacterCreatorAnnotatedEyeglasses_s.clear();
	CharacterCreatorAnnotatedFacialHair_s.clear();
	CharacterCreatorAnnotatedBody_s.clear();
	CharacterCreatorAnnotatedLegs_s.clear();
	CharacterCreatorAnnotatedFeet_s.clear();
	CharacterCreatorAnnotatedAccessory1_s.clear();
	CharacterCreatorAnnotatedAccessory2_s.clear();


	CharacterCreatorAnnotatedTagHeadGear_s.clear();
	CharacterCreatorAnnotatedTagHair_s.clear();
	CharacterCreatorAnnotatedTagHead_s.clear();
	CharacterCreatorAnnotatedTagEyeglasses_s.clear();
	CharacterCreatorAnnotatedTagFacialHair_s.clear();
	CharacterCreatorAnnotatedTagBody_s.clear();
	CharacterCreatorAnnotatedTagLegs_s.clear();
	CharacterCreatorAnnotatedTagFeet_s.clear();
	CharacterCreatorAnnotatedTagAccessory1_s.clear();
	CharacterCreatorAnnotatedTagAccessory2_s.clear();

	// choose voice based on CCP type
	pCCPVoiceSet = "";
	if (stricmp (CCP_Type, "adult male") == NULL) pCCPVoiceSet = "David";
	if (stricmp (CCP_Type, "adult female") == NULL) pCCPVoiceSet = "Hazel";
	for (int i = 0; i < g_voiceList_s.size(); i++)
	{
		if (stricmp (CCP_Type, "adult male") == NULL && strstr (g_voiceList_s[i].Get(), "David") != NULL)
		{
			pCCPVoiceSet = g_voiceList_s[i].Get();
			break;
		}
		if (stricmp (CCP_Type, "adult female") == NULL && strstr (g_voiceList_s[i].Get(), "Hazel") != NULL)
		{
			pCCPVoiceSet = g_voiceList_s[i].Get();
			break;
		}
	}

	// work out path to parts
	char pPartsPath[260];
	strcpy(pPartsPath, "charactercreatorplus\\parts\\");
	strcat(pPartsPath, CCP_Type);
	strcat(pPartsPath, "\\");

	// some lists can have a NONE value, which does not need a part model
	CharacterCreatorHeadGear_s.insert(std::make_pair("None", pPartsPath));
	CharacterCreatorHair_s.insert(std::make_pair("None", pPartsPath));
	CharacterCreatorEyeglasses_s.insert(std::make_pair("None", pPartsPath));
	CharacterCreatorFacialHair_s.insert(std::make_pair("None", pPartsPath));
	CharacterCreatorAccessory1_s.insert(std::make_pair("None", pPartsPath));
	CharacterCreatorAccessory2_s.insert(std::make_pair("None", pPartsPath));

	CharacterCreatorAnnotatedHeadGear_s.insert(std::make_pair("", "None"));
	CharacterCreatorAnnotatedHair_s.insert(std::make_pair("", "None"));
	CharacterCreatorAnnotatedEyeglasses_s.insert(std::make_pair("", "None"));
	CharacterCreatorAnnotatedFacialHair_s.insert(std::make_pair("", "None"));
	CharacterCreatorAnnotatedAccessory1_s.insert(std::make_pair("", "None"));
	CharacterCreatorAnnotatedAccessory2_s.insert(std::make_pair("", "None"));

	// ensure annotates and tags size matches
	CharacterCreatorAnnotatedTagHeadGear_s.insert(std::make_pair("", ""));
	CharacterCreatorAnnotatedTagHair_s.insert(std::make_pair("", ""));
	CharacterCreatorAnnotatedTagEyeglasses_s.insert(std::make_pair("", ""));
	CharacterCreatorAnnotatedTagFacialHair_s.insert(std::make_pair("", ""));
	CharacterCreatorAnnotatedTagAccessory1_s.insert(std::make_pair("", ""));
	CharacterCreatorAnnotatedTagAccessory2_s.insert(std::make_pair("", ""));

	// free any old character objects
	iCharObj = g.characterkitobjectoffset + 1;
	iCharObjHeadGear = g.characterkitobjectoffset + 2;
	iCharObjHair = g.characterkitobjectoffset + 3;
	iCharObjHead = g.characterkitobjectoffset + 4;
	iCharObjEyeglasses = g.characterkitobjectoffset + 5;
	iCharObjFacialHair = g.characterkitobjectoffset + 6;
	iCharObjLegs = g.characterkitobjectoffset + 7;
	iCharObjFeet = g.characterkitobjectoffset + 8;
	iCharObjAccessory1 = g.characterkitobjectoffset + 9;
	iCharObjAccessory2 = g.characterkitobjectoffset + 10;

	if (ObjectExist(iCharObj) == 1) DeleteObject(iCharObj);
	if (ObjectExist(iCharObjHeadGear) == 1) DeleteObject(iCharObjHeadGear);
	if (ObjectExist(iCharObjHair) == 1) DeleteObject(iCharObjHair);
	if (ObjectExist(iCharObjHead) == 1) DeleteObject(iCharObjHead);
	if (ObjectExist(iCharObjEyeglasses) == 1) DeleteObject(iCharObjEyeglasses);
	if (ObjectExist(iCharObjFacialHair) == 1) DeleteObject(iCharObjFacialHair);
	if (ObjectExist(iCharObjLegs) == 1) DeleteObject(iCharObjLegs);
	if (ObjectExist(iCharObjFeet) == 1) DeleteObject(iCharObjFeet);
	if (ObjectExist(iCharObjAccessory1) == 1) DeleteObject(iCharObjAccessory1);
	if (ObjectExist(iCharObjAccessory2) == 1) DeleteObject(iCharObjAccessory2);

	// default body part choices
	strcpy(cSelectedFeetFilter, "");
	strcpy(cSelectedICCode, "IC1a");
	strcpy(cSelectedHeadGear, "None");
	strcpy(cSelectedHair, "");
	strcpy(cSelectedHead, "");
	strcpy(cSelectedEyeglasses, "None");
	strcpy(cSelectedFacialHair, "None");
	strcpy(cSelectedBody, "");
	strcpy(cSelectedLegs, "");
	strcpy(cSelectedFeet, "");
	strcpy(cSelectedAccessory1, "None");
	strcpy(cSelectedAccessory2, "None");

	// which legs part to use
	strcpy(cSelectedLegsFilter, "");
	LPSTR pOptionalLegsChoice = "02"; 

	// scan for all character parts
	cstr olddir_s = GetDir();
	char pTempStr[1024];

	//PE: Load parts avaiable from document folder.
	for (int i = 0; i < 2; i++)
	{
		bool bActive = false;
		if (i == 0)
		{
			strcpy(pTempStr, "charactercreatorplus\\parts\\");
			strcat(pTempStr, CCP_Type);
			SetDir(pTempStr);
			bActive = true;
		}
		else
		{
			extern char szWriteDir[MAX_PATH];
			extern char szBeforeChangeWriteDir[MAX_PATH];
			//PE: WriteDir contain ducument folder and or remoteproject folder.
			if (strlen(szWriteDir) > 0)
			{
				strcpy(pTempStr, szWriteDir);
				strcat(pTempStr, "Files\\");
				strcat(pTempStr, "charactercreatorplus\\parts\\");
				strcat(pTempStr, CCP_Type);
				if (PathExist(pTempStr))
				{
					SetDir(pTempStr);
					bActive = true;
				}
				else
					bActive = false;
			}
		}
		if (bActive)
		{
			ChecklistForFiles();
			charactercreatorplus_loadannotationlist();
			for (int c = 1; c <= ChecklistQuantity(); c++)
			{
				cStr tfile_s = Lower(ChecklistString(c));
				if (tfile_s != "." && tfile_s != "..")
				{
					char* find = NULL;
					if (strcmp(Right(tfile_s.Get(), 10), "_color.dds") == 0)
					{
						// base filename
						char tmp[260];
						strcpy(tmp, tfile_s.Get());
						tmp[strlen(tmp) - 10] = 0; // remove _color.dds

						// determine which list it goes into
						if (pestrcasestr(tfile_s.Get(), " body "))
						{
							CharacterCreatorBody_s.insert(std::make_pair(tmp, pPartsPath));
							LPSTR pAnnotatedLabel = charactercreatorplus_findannotation(tmp);
							if (pAnnotatedLabel)
								CharacterCreatorAnnotatedBody_s.insert(std::make_pair(tmp, pAnnotatedLabel));
							else
								CharacterCreatorAnnotatedBody_s.insert(std::make_pair(tmp, tmp));
							LPSTR pAnnotatedTagLabel = charactercreatorplus_findannotationtag(tmp);
							if (pAnnotatedTagLabel)
								CharacterCreatorAnnotatedTagBody_s.insert(std::make_pair(tmp, pAnnotatedTagLabel));
							else
								CharacterCreatorAnnotatedTagBody_s.insert(std::make_pair(tmp, ""));

							bool bBodyFilterThisOut = false;
							if (stricmp(CCP_Type, "zombie male") != NULL && stricmp(CCP_Type, "zombie female") != NULL)
							{
								if (tmp[strlen(tmp) - 1] != '1' && tmp[strlen(tmp) - 1] != 'a') bBodyFilterThisOut = true;
							}
							if (strlen(cSelectedBody) == 0 && bBodyFilterThisOut == false)
							{
								// for body, do we need no legs?
								if (pAnnotatedTagLabel)
								{
									if (strstr(pAnnotatedTagLabel, "No Legs") != NULL)
									{
										strcpy(cSelectedLegsFilter, "No Legs");
										pOptionalLegsChoice = "01";
									}
								}

								// and this is the default body
								strcpy(cSelectedBody, tmp);
							}
						}
						else if (pestrcasestr(tfile_s.Get(), " feet "))
						{
							CharacterCreatorFeet_s.insert(std::make_pair(tmp, pPartsPath));
							LPSTR pAnnotatedLabel = charactercreatorplus_findannotation(tmp);
							if (pAnnotatedLabel)
								CharacterCreatorAnnotatedFeet_s.insert(std::make_pair(tmp, pAnnotatedLabel));
							else
								CharacterCreatorAnnotatedFeet_s.insert(std::make_pair(tmp, tmp));
							LPSTR pAnnotatedTagLabel = charactercreatorplus_findannotationtag(tmp);
							if (pAnnotatedTagLabel)
								CharacterCreatorAnnotatedTagFeet_s.insert(std::make_pair(tmp, pAnnotatedTagLabel));
							else
								CharacterCreatorAnnotatedTagFeet_s.insert(std::make_pair(tmp, ""));
							if (strlen(cSelectedFeet) == 0) strcpy(cSelectedFeet, tmp);
						}
						else if (pestrcasestr(tfile_s.Get(), " hair "))
						{
							CharacterCreatorHair_s.insert(std::make_pair(tmp, pPartsPath));
							LPSTR pAnnotatedLabel = charactercreatorplus_findannotation(tmp);
							if (pAnnotatedLabel)
								CharacterCreatorAnnotatedHair_s.insert(std::make_pair(tmp, pAnnotatedLabel));
							else
								CharacterCreatorAnnotatedHair_s.insert(std::make_pair(tmp, tmp));
							LPSTR pAnnotatedTagLabel = charactercreatorplus_findannotationtag(tmp);
							if (pAnnotatedTagLabel)
								CharacterCreatorAnnotatedTagHair_s.insert(std::make_pair(tmp, pAnnotatedTagLabel));
							else
								CharacterCreatorAnnotatedTagHair_s.insert(std::make_pair(tmp, ""));
							if (strlen(cSelectedHair) == 0) strcpy(cSelectedHair, tmp);
						}
						else if (pestrcasestr(tfile_s.Get(), " head "))
						{
							CharacterCreatorHead_s.insert(std::make_pair(tmp, pPartsPath));
							LPSTR pAnnotatedLabel = charactercreatorplus_findannotation(tmp);
							if (pAnnotatedLabel)
								CharacterCreatorAnnotatedHead_s.insert(std::make_pair(tmp, pAnnotatedLabel));
							else
								CharacterCreatorAnnotatedHead_s.insert(std::make_pair(tmp, tmp));
							LPSTR pAnnotatedTagLabel = charactercreatorplus_findannotationtag(tmp);
							if (pAnnotatedTagLabel)
								CharacterCreatorAnnotatedTagHead_s.insert(std::make_pair(tmp, pAnnotatedTagLabel));
							else
								CharacterCreatorAnnotatedTagHead_s.insert(std::make_pair(tmp, ""));
							if (strlen(cSelectedHead) == 0) strcpy(cSelectedHead, tmp);
						}
						else if (pestrcasestr(tfile_s.Get(), " legs "))
						{
							CharacterCreatorLegs_s.insert(std::make_pair(tmp, pPartsPath));
							LPSTR pAnnotatedLabel = charactercreatorplus_findannotation(tmp);
							if (pAnnotatedLabel)
								CharacterCreatorAnnotatedLegs_s.insert(std::make_pair(tmp, pAnnotatedLabel));
							else
								CharacterCreatorAnnotatedLegs_s.insert(std::make_pair(tmp, tmp));
							LPSTR pAnnotatedTagLabel = charactercreatorplus_findannotationtag(tmp);
							if (pAnnotatedTagLabel)
								CharacterCreatorAnnotatedTagLegs_s.insert(std::make_pair(tmp, pAnnotatedTagLabel));
							else
								CharacterCreatorAnnotatedTagLegs_s.insert(std::make_pair(tmp, ""));

							// if require no legs, always choose 01 for default
							if (strlen(cSelectedLegs) == 0)
							{
								bool bLegsFilterThisOut = false;
								if (stricmp(CCP_Type, "zombie male") != NULL && stricmp(CCP_Type, "zombie female") != NULL)
								{
									bLegsFilterThisOut = true;
									if (strstr(pOptionalLegsChoice, "01") != NULL)
									{
										if (strlen(cSelectedLegs) == 0 && tmp[strlen(tmp) - 1] == '1') bLegsFilterThisOut = false;
									}
									else
									{
										if (strlen(cSelectedLegs) == 0 && tmp[strlen(tmp) - 1] != '1') bLegsFilterThisOut = false;
									}
								}
								if (bLegsFilterThisOut == false)
								{
									strcpy(cSelectedLegs, tmp);
								}
							}
						}
						else if (pestrcasestr(tfile_s.Get(), " headgear "))
						{
							CharacterCreatorHeadGear_s.insert(std::make_pair(tmp, pPartsPath));
							LPSTR pAnnotatedLabel = charactercreatorplus_findannotation(tmp);
							if (pAnnotatedLabel)
								CharacterCreatorAnnotatedHeadGear_s.insert(std::make_pair(tmp, pAnnotatedLabel));
							else
								CharacterCreatorAnnotatedHeadGear_s.insert(std::make_pair(tmp, tmp));
							LPSTR pAnnotatedTagLabel = charactercreatorplus_findannotationtag(tmp);
							if (pAnnotatedTagLabel)
								CharacterCreatorAnnotatedTagHeadGear_s.insert(std::make_pair(tmp, pAnnotatedTagLabel));
							else
								CharacterCreatorAnnotatedTagHeadGear_s.insert(std::make_pair(tmp, ""));
							if (strlen(cSelectedHeadGear) == 0) strcpy(cSelectedHeadGear, tmp);
						}
						else if (pestrcasestr(tfile_s.Get(), " facialhair "))
						{
							CharacterCreatorFacialHair_s.insert(std::make_pair(tmp, pPartsPath));
							LPSTR pAnnotatedLabel = charactercreatorplus_findannotation(tmp);
							if (pAnnotatedLabel)
								CharacterCreatorAnnotatedFacialHair_s.insert(std::make_pair(tmp, pAnnotatedLabel));
							else
								CharacterCreatorAnnotatedFacialHair_s.insert(std::make_pair(tmp, tmp));
							LPSTR pAnnotatedTagLabel = charactercreatorplus_findannotationtag(tmp);
							if (pAnnotatedTagLabel)
								CharacterCreatorAnnotatedTagFacialHair_s.insert(std::make_pair(tmp, pAnnotatedTagLabel));
							else
								CharacterCreatorAnnotatedTagFacialHair_s.insert(std::make_pair(tmp, ""));
							if (strlen(cSelectedFacialHair) == 0) strcpy(cSelectedFacialHair, tmp);
						}
						else if (pestrcasestr(tfile_s.Get(), " eyeglasses "))
						{
							CharacterCreatorEyeglasses_s.insert(std::make_pair(tmp, pPartsPath));
							LPSTR pAnnotatedLabel = charactercreatorplus_findannotation(tmp);
							if (pAnnotatedLabel)
								CharacterCreatorAnnotatedEyeglasses_s.insert(std::make_pair(tmp, pAnnotatedLabel));
							else
								CharacterCreatorAnnotatedEyeglasses_s.insert(std::make_pair(tmp, tmp));
							LPSTR pAnnotatedTagLabel = charactercreatorplus_findannotationtag(tmp);
							if (pAnnotatedTagLabel)
								CharacterCreatorAnnotatedTagEyeglasses_s.insert(std::make_pair(tmp, pAnnotatedTagLabel));
							else
								CharacterCreatorAnnotatedTagEyeglasses_s.insert(std::make_pair(tmp, ""));
							if (strlen(cSelectedEyeglasses) == 0) strcpy(cSelectedEyeglasses, tmp);
						}
						else if (pestrcasestr(tfile_s.Get(), " accessory1 "))
						{
							CharacterCreatorAccessory1_s.insert(std::make_pair(tmp, pPartsPath));
							LPSTR pAnnotatedLabel = charactercreatorplus_findannotation(tmp);
							if (pAnnotatedLabel)
								CharacterCreatorAnnotatedAccessory1_s.insert(std::make_pair(tmp, pAnnotatedLabel));
							else
								CharacterCreatorAnnotatedAccessory1_s.insert(std::make_pair(tmp, tmp));
							LPSTR pAnnotatedTagLabel = charactercreatorplus_findannotationtag(tmp);
							if (pAnnotatedTagLabel)
								CharacterCreatorAnnotatedTagAccessory1_s.insert(std::make_pair(tmp, pAnnotatedTagLabel));
							else
								CharacterCreatorAnnotatedTagAccessory1_s.insert(std::make_pair(tmp, ""));
							if (strlen(cSelectedAccessory1) == 0) strcpy(cSelectedAccessory1, tmp);
						}
						else if (pestrcasestr(tfile_s.Get(), " accessory2 "))
						{
							CharacterCreatorAccessory2_s.insert(std::make_pair(tmp, pPartsPath));
							LPSTR pAnnotatedLabel = charactercreatorplus_findannotation(tmp);
							if (pAnnotatedLabel)
								CharacterCreatorAnnotatedAccessory2_s.insert(std::make_pair(tmp, pAnnotatedLabel));
							else
								CharacterCreatorAnnotatedAccessory2_s.insert(std::make_pair(tmp, tmp));
							LPSTR pAnnotatedTagLabel = charactercreatorplus_findannotationtag(tmp);
							if (pAnnotatedTagLabel)
								CharacterCreatorAnnotatedTagAccessory2_s.insert(std::make_pair(tmp, pAnnotatedTagLabel));
							else
								CharacterCreatorAnnotatedTagAccessory2_s.insert(std::make_pair(tmp, ""));
							if (strlen(cSelectedAccessory2) == 0) strcpy(cSelectedAccessory2, tmp);
						}
					}
				}
			}
		}
	}
	SetDir(olddir_s.Get());

	// if have default selection, set the choices to those
	static bool bRefreshedToCustomVariant = false;
	int iBase = 0;
	std::array<std::string, 10>* storage = nullptr;
	if (stricmp(CCP_Type, "adult male") == NULL)
	{
		iBase = 1;
		bRefreshedToCustomVariant = false;
		if(g_maleStorage[0].length() > 0)
			storage = &g_maleStorage;
		g_fCCPZoom = 73.0f;
	}
	if (stricmp(CCP_Type, "adult female") == NULL) 
	{
		iBase = 2;
		bRefreshedToCustomVariant = false;
		if (g_femaleStorage[0].length() > 0)
			storage = &g_femaleStorage;
		g_fCCPZoom = 73.0f;
	}
	if (stricmp(CCP_Type, "zombie male") == NULL)
	{
		iBase = 3;
		bRefreshedToCustomVariant = false;
		if (g_zombieStorage[0].length() > 0)
			storage = &g_zombieStorage;
		g_fCCPZoom = 67.0f;
	}
	if (stricmp(CCP_Type, "zombie female") == NULL)
	{
		iBase = 4;
		bRefreshedToCustomVariant = false;
		if (g_zombieStorage[0].length() > 0)
			storage = &g_zombieStorage;
		g_fCCPZoom = 67.0f;
	}
	if (iBase == 0)
	{
		// search for custom char type
		iBase = GetBaseValueFromCCPType(CCP_Type);
		if (iBase > 0)
		{
			if (g_genericStorage[0].length() > 0)
			{
				// but ONLY if previous refresh was a NON-custom (as there is only ONE genericStorage slot, i.e. Goblin variant can corrupt Low Poly choices)
				if (bRefreshedToCustomVariant == false)
				{
					storage = &g_genericStorage;
				}
			}
			g_fCCPZoom = 67.0f;
		}
		else
		{
			iBase = 1;
		}
		bRefreshedToCustomVariant = true;
	}
	if (!storage)
	{
		char pDefault[32];
		char pDefaultVariant[32];
		strcpy(pDefaultVariant, "");
		charactercreatorplus_GetDefaultCharacterPartNum(iBase, 1, pDefault, pDefaultVariant);
		sprintf(cSelectedBody, "%s body %s", CCP_Type, pDefaultVariant);
		charactercreatorplus_GetDefaultCharacterPartNum(iBase, 2, pDefault, pDefaultVariant);
		sprintf(cSelectedHead, "%s head %s", CCP_Type, pDefaultVariant);
		charactercreatorplus_GetDefaultCharacterPartNum(iBase, 3, pDefault, pDefaultVariant);
		sprintf(cSelectedLegs, "%s legs %s", CCP_Type, pDefaultVariant);
		charactercreatorplus_GetDefaultCharacterPartNum(iBase, 4, pDefault, pDefaultVariant);
		sprintf(cSelectedFeet, "%s feet %s", CCP_Type, pDefaultVariant);
		charactercreatorplus_GetDefaultCharacterPartNum(iBase, 5, pDefault, pDefaultVariant);
		if(strlen(pDefaultVariant)>0)
			sprintf(cSelectedHair, "%s hair %s", CCP_Type, pDefaultVariant);
		else
			strcpy(cSelectedHair, "None");
	}
	else
	{
		// 0: Head Gear
		// 1: Hair
		// 2: Head
		// 3: Eye Glasses
		// 4: Facial Hair
		// 5: Body
		// 6: Legs
		// 7: Feet
		std::array<std::string, 10>& parts = *storage;
		strcpy(cSelectedHeadGear, parts[0].c_str());
		strcpy(cSelectedHair, parts[1].c_str());
		strcpy(cSelectedHead, parts[2].c_str());
		strcpy(cSelectedEyeglasses, parts[3].c_str());
		strcpy(cSelectedFacialHair, parts[4].c_str());
		strcpy(cSelectedBody, parts[5].c_str());
		strcpy(cSelectedLegs, parts[6].c_str());
		strcpy(cSelectedFeet, parts[7].c_str());
		strcpy(cSelectedAccessory1, parts[8].c_str());
		strcpy(cSelectedAccessory2, parts[9].c_str());
	
	}

	charactercreatorplus_initautoswaps();
	//g_pLastKnownTransition = &g_HeadTransition;
	if (iBase == 3 || iBase == 4)
	{
		g_pLastKnownTransition = &g_ZombieBodyTransition;
	}
	else
	{
		g_pLastKnownTransition = &g_UpperBodyTransition;
	}

	// configure default weapon choices
	g_grideleprof_holdchoices.hasweapon_s = "enhanced\\Mk19T";
	g_grideleprof_holdchoices.cantakeweapon = 0;
	g_grideleprof_holdchoices.quantity = 8;

	// output location for character
	LPSTR pCharacterFinal = "entitybank\\user\\charactercreatorplus\\character.dbo";

	// reset color selections
	for (int a = 0; a < 5; a++) vColorSelected[a] = ImVec4(0.0, 0.0, 0.0, 1.0);

	// generic textures
	int iCharTextureWhite = g.charactercreatorEditorImageoffset + 0;
	LoadImage("effectbank\\reloaded\\media\\blank_O.DDS", iCharTextureWhite);
	int iCharTextureBlack = g.charactercreatorEditorImageoffset + 1;
	if (!GetImageExistEx(iCharTextureBlack)) LoadImage("effectbank\\reloaded\\media\\blank_black.dds", iCharTextureBlack);

	// work out path to part files
	strcpy(pTempStr, "charactercreatorplus\\parts\\");
	strcat(pTempStr, CCP_Type);
	strcat(pTempStr, "\\");
	strcat(pTempStr, CCP_Type);
	strcat(pTempStr, " ");

	// texture index references
	int iCharTexture = g.charactercreatorEditorImageoffset + 1;
	int iCharHeadGearTexture = g.charactercreatorEditorImageoffset + 11;
	int iCharHairTexture = g.charactercreatorEditorImageoffset + 21;
	int iCharHeadTexture = g.charactercreatorEditorImageoffset + 31;
	int iCharEyeglassesTexture = g.charactercreatorEditorImageoffset + 41;
	int iCharFacialHairTexture = g.charactercreatorEditorImageoffset + 51;
	int iCharLegsTexture = g.charactercreatorEditorImageoffset + 61;
	int iCharFeetTexture = g.charactercreatorEditorImageoffset + 71;
	int iCharAccessory1Texture = g.charactercreatorEditorImageoffset + 81;
	int iCharAccessory2Texture = g.charactercreatorEditorImageoffset + 91;

	// load default skin type texture IC1a
	std::map<std::string, std::string>::iterator it = CharacterCreatorAnnotatedTagHead_s.begin();
	LPSTR pCorrectICCode = "IC1c"; // default
	for (it = CharacterCreatorAnnotatedTagHead_s.begin(); it != CharacterCreatorAnnotatedTagHead_s.end(); ++it)
	{
		std::string thistag = it->first;
		if (strnicmp (thistag.c_str(), cSelectedHead, strlen(cSelectedHead)) == NULL)
		{
			// found correct skin for this head
			LPSTR pNewSkinCode = (LPSTR)it->second.c_str();
			if (strlen(pNewSkinCode) > 0) pCorrectICCode = pNewSkinCode;
			break;
		}
	}
	int iCharSkinTexture = g.charactercreatorEditorImageoffset + 101;
	if (GetImageExistEx(iCharSkinTexture) == 1) DeleteImage(iCharSkinTexture);
	char pRelPathToSkinTexture[MAX_PATH];
	sprintf(pRelPathToSkinTexture, "charactercreatorplus\\skins\\%s.png", pCorrectICCode);
	LoadImage(pRelPathToSkinTexture, iCharSkinTexture);

	// Default body
	bool bNonStandardCharacter = false;
	char pPartNumStr[32];
	char pPartNumVariantStr[32];
	char pDefaultBody[32];
	char pDefaultBodyVariant[32];
	char pDefaultHair[32];
	char pDefaultHairVariant[32];
	char pDefaultHead[32];
	char pDefaultHeadVariant[32];
	char pDefaultLegs[32];
	char pDefaultLegsVariant[32];
	char pDefaultFeet[32];
	char pDefaultFeetVariant[32];

	char pDefaultHeadgear[32] = { 0 };
	char pDefaultHeadgearVariant[32] = { 0 };
	char pDefaultGlasses[32] = { 0 };
	char pDefaultGlassesVariant[32] = { 0 };
	char pDefaultFacialHair[32] = { 0 };
	char pDefaultFacialHairVariant[32] = { 0 };

	char pDefaultAccessory1[32] = { 0 };
	char pDefaultAccessory2[32] = { 0 };
	char pDefaultAccessory1Variant[32] = { 0 };
	char pDefaultAccessory2Variant[32] = { 0 };

	// Need to add default headgear, glasses and facial hair.
	iBase = 0;
	if (stricmp(CCP_Type, "adult male") == NULL) iBase = 1;
	if (stricmp(CCP_Type, "adult female") == NULL) iBase = 2;
	if (stricmp(CCP_Type, "zombie male") == NULL) iBase = 3;
	if (stricmp(CCP_Type, "zombie female") == NULL) iBase = 4;
	if (iBase == 0)	iBase = GetBaseValueFromCCPType(CCP_Type);
	if (!storage)
	{
		charactercreatorplus_GetDefaultCharacterPartNum(iBase, 1, pPartNumStr, pPartNumVariantStr);
		strcpy(pDefaultBody, pPartNumStr);
		strcpy(pDefaultBodyVariant, pPartNumVariantStr);

		charactercreatorplus_GetDefaultCharacterPartNum(iBase, 5, pPartNumStr, pPartNumVariantStr);
		strcpy(pDefaultHair, pPartNumStr);
		strcpy(pDefaultHairVariant, pPartNumVariantStr);

		charactercreatorplus_GetDefaultCharacterPartNum(iBase, 2, pPartNumStr, pPartNumVariantStr);
		strcpy(pDefaultHead, pPartNumStr);
		strcpy(pDefaultHeadVariant, pPartNumVariantStr);

		charactercreatorplus_GetDefaultCharacterPartNum(iBase, 3, pPartNumStr, pPartNumVariantStr);
		strcpy(pDefaultLegs, pPartNumStr);
		strcpy(pDefaultLegsVariant, pPartNumVariantStr);

		charactercreatorplus_GetDefaultCharacterPartNum(iBase, 4, pPartNumStr, pPartNumVariantStr);
		strcpy(pDefaultFeet, pPartNumStr);
		strcpy(pDefaultFeetVariant, pPartNumVariantStr);
	}
	else
	{
		// Instead of using defaults, use the users previous choice for this base type.
		std::array<std::string, 10>& parts = *storage;
		charactercreatorplus_extractpartnumberandvariation(parts[0].c_str(), pDefaultHeadgear, pDefaultHeadgearVariant);
		charactercreatorplus_extractpartnumberandvariation(parts[1].c_str(), pDefaultHair, pDefaultHairVariant);
		charactercreatorplus_extractpartnumberandvariation(parts[2].c_str(), pDefaultHead, pDefaultHeadVariant);
		charactercreatorplus_extractpartnumberandvariation(parts[3].c_str(), pDefaultGlasses, pDefaultGlassesVariant);
		charactercreatorplus_extractpartnumberandvariation(parts[4].c_str(), pDefaultFacialHair, pDefaultFacialHairVariant);
		charactercreatorplus_extractpartnumberandvariation(parts[5].c_str(), pDefaultBody, pDefaultBodyVariant);
		charactercreatorplus_extractpartnumberandvariation(parts[6].c_str(), pDefaultLegs, pDefaultLegsVariant);
		charactercreatorplus_extractpartnumberandvariation(parts[7].c_str(), pDefaultFeet, pDefaultFeetVariant);
		charactercreatorplus_extractpartnumberandvariation(parts[8].c_str(), pDefaultAccessory1, pDefaultAccessory1Variant);
		charactercreatorplus_extractpartnumberandvariation(parts[9].c_str(), pDefaultAccessory2, pDefaultAccessory2Variant);
	}

	// load all body parts
	for (int iPartID = 0; iPartID < 10; iPartID++)
	{
		// work out which part
		int iThisObj = 0;
		int iThisTexture = 0;
		LPSTR pPartName = "";
		LPSTR pPartNum = "";
		LPSTR pPartNumVariant = "";
		if (iPartID == 0) { iThisObj = iCharObj; iThisTexture = iCharTexture; pPartName = "body"; pPartNum = pDefaultBody; pPartNumVariant = pDefaultBodyVariant; }
		if (iPartID == 1) 
		{ 
			if (storage)
			{
				char* pHeadgear = "";
				if (strlen(pDefaultHeadgear) > 0)
					pHeadgear = "headgear";
				iThisObj = iCharObjHeadGear; iThisTexture = iCharHeadGearTexture; pPartName = pHeadgear; pPartNum = pDefaultHeadgear; pPartNumVariant = pDefaultHeadgearVariant;
			}
			else
			{
				iThisObj = iCharObjHeadGear; iThisTexture = iCharHeadGearTexture; pPartName = ""; pPartNum = ""; pPartNumVariant = "";
			}
		}
		if (iPartID == 2) 
		{ 
			iThisObj = iCharObjHair; 
			iThisTexture = iCharHairTexture; 
			pPartName = "hair"; pPartNum = pDefaultHair; 
			pPartNumVariant = pDefaultHairVariant; 
			if (strlen(pPartNum) == 0)
				pPartName = "";

		}
		if (iPartID == 3) { iThisObj = iCharObjHead; iThisTexture = iCharHeadTexture; pPartName = "head"; pPartNum = pDefaultHead; pPartNumVariant = pDefaultHeadVariant; }
		if (iPartID == 4) 
		{ 
			if (storage)
			{
				char* pGlasses = "";
				if (strlen(pDefaultGlasses) > 0)
					pGlasses = "eyeglasses";
				iThisObj = iCharObjEyeglasses; iThisTexture = iCharEyeglassesTexture; pPartName = pGlasses; pPartNum = pDefaultGlasses; pPartNumVariant = pDefaultGlassesVariant;
			}
			else
			{
				iThisObj = iCharObjEyeglasses; iThisTexture = iCharEyeglassesTexture; pPartName = ""; pPartNum = ""; pPartNumVariant = "";
			}
		}
		if (iPartID == 8)
		{
			if (storage)
			{
				char* pAccessory = "";
				if (strlen(pDefaultAccessory1) > 0)
					pAccessory = "accessory1";
				iThisObj = iCharObjAccessory1; iThisTexture = iCharAccessory1Texture; pPartName = pAccessory; pPartNum = pDefaultAccessory1; pPartNumVariant = pDefaultAccessory1Variant;
			}
			else
			{
				iThisObj = iCharObjAccessory1; iThisTexture = iCharAccessory1Texture; pPartName = ""; pPartNum = ""; pPartNumVariant = "";
			}
		}
		if (iPartID == 9)
		{
			if (storage)
			{
				char* pAccessory = "";
				if (strlen(pDefaultAccessory2) > 0)
					pAccessory = "accessory2";
				iThisObj = iCharObjAccessory1; iThisTexture = iCharAccessory2Texture; pPartName = pAccessory; pPartNum = pDefaultAccessory2; pPartNumVariant = pDefaultAccessory2Variant;
			}
			else
			{
				iThisObj = iCharObjAccessory2; iThisTexture = iCharAccessory2Texture; pPartName = ""; pPartNum = ""; pPartNumVariant = "";
			}
		}
		if (iPartID == 5) 
		{ 
			if (storage)
			{
				char* pFacialHair = "";
				if (strlen(pDefaultFacialHair) > 0)
					pFacialHair = "facialhair";
				iThisObj = iCharObjFacialHair; iThisTexture = iCharFacialHairTexture; pPartName = pFacialHair; pPartNum = pDefaultFacialHair; pPartNumVariant = pDefaultFacialHairVariant;
			}
			else
			{
				iThisObj = iCharObjFacialHair; iThisTexture = iCharFacialHairTexture; pPartName = ""; pPartNum = ""; pPartNumVariant = "";
			}
		}
		if (iPartID == 6) 
		{ 
			iThisObj = iCharObjLegs; 
			iThisTexture = iCharLegsTexture; 
			pPartName = "legs"; 
			pPartNum = pDefaultLegs; 
			pPartNumVariant = pDefaultLegsVariant; 	
			std::map<std::string, std::string>::iterator it = CharacterCreatorAnnotatedTagBody_s.begin();
			LPSTR pCorrpBodyNoLegsTag = (LPSTR)it->second.c_str();
			if ( strstr ( pCorrpBodyNoLegsTag, "No Legs" ) != NULL )
				pPartNum = "01"; 
		}
		if (iPartID == 7) { iThisObj = iCharObjFeet; iThisTexture = iCharFeetTexture; pPartName = "feet"; pPartNum = pDefaultFeet; pPartNumVariant = pDefaultFeetVariant; }

		// load part object, textures and apply effect
		if (strlen(pPartName) > 0)
		{
			char pPartFile[260];
			strcpy(pPartFile, pPartName);
			strcat(pPartFile, " ");
			strcat(pPartFile, pPartNum); // i.e. 01
			LoadObject(cstr(cstr(pTempStr) + pPartFile + CCPMODELEXT).Get(), iThisObj);
			char pVariantPartFile[260];
			strcpy(pVariantPartFile, pPartName);
			strcat(pVariantPartFile, " ");
			strcat(pVariantPartFile, pPartNumVariant); // i.e. 01c
			cstr pPathVariant = cstr(pTempStr) + pVariantPartFile;
			cstr pPath = cstr(pTempStr) + pPartFile;
			charactercreatorplus_loadccimages(pPathVariant.Get(), pPath.Get(), iThisTexture);
			charactercreatorplus_textureccimages(iThisObj, iThisTexture);
			// and need mask for reskin
			if (ImageExist(iThisTexture + 1) == 1) DeleteImage(iThisTexture + 1);
			LoadImage(cstr(cstr(pTempStr) + pPartFile + "_mask.dds").Get(), iThisTexture + 1);
			if (ImageExist(iThisTexture + 1) == 0)
			{
				// remove color-specific from part and look for base mask for this body part
				char pCropPartFile[MAX_PATH];
				strcpy(pCropPartFile, pPartFile);
				LPSTR pLastChar = pCropPartFile + strlen(pCropPartFile) - 1;
				if (*pLastChar >= 'a' && *pLastChar <= 'z') *pLastChar = 0;
				LoadImage(cstr(cstr(pTempStr) + pCropPartFile + "_mask.dds").Get(), iThisTexture + 1);
			}
		}
	}

	// wicked not using custom character shader, so create a new albedo/color texture
	// using template mask and skin color texture to produce correct albedo representing skin
	// only needed to do for body, legs and feet
	if (bNonStandardCharacter == false)
	{
		charactercreatorplus_refreshskincolor();
	}

	// remove wicked resources before modifying objects below
	if ( ObjectExist(iCharObj) ) WickedCall_RemoveObject( GetObjectData(iCharObj) );
	if ( ObjectExist(iCharObjHeadGear) ) WickedCall_RemoveObject( GetObjectData(iCharObjHeadGear) );
	if ( ObjectExist(iCharObjHead) ) WickedCall_RemoveObject( GetObjectData(iCharObjHead) );
	if ( ObjectExist(iCharObjLegs) ) WickedCall_RemoveObject( GetObjectData(iCharObjLegs) );
	if ( ObjectExist(iCharObjFeet) ) WickedCall_RemoveObject( GetObjectData(iCharObjFeet) );
	if ( ObjectExist(iCharObjHair) ) WickedCall_RemoveObject( GetObjectData(iCharObjHair) );
	if ( ObjectExist(iCharObjEyeglasses) ) WickedCall_RemoveObject( GetObjectData(iCharObjEyeglasses) );
	if ( ObjectExist(iCharObjFacialHair) ) WickedCall_RemoveObject( GetObjectData(iCharObjFacialHair) );
	if (ObjectExist(iCharObjAccessory1)) WickedCall_RemoveObject(GetObjectData(iCharObjAccessory1));
	if (ObjectExist(iCharObjAccessory2)) WickedCall_RemoveObject(GetObjectData(iCharObjAccessory2));

	// meshes are useless once they have been stolen from (preload system allows fresh loading to be near instant however)
	if (ObjectExist(iCharObjHeadGear) == 1)
	{
		StealMeshesFromObject(iCharObj, iCharObjHeadGear);
		DeleteObject(iCharObjHeadGear);
	}
	StealMeshesFromObject(iCharObj, iCharObjHead);
	DeleteObject(iCharObjHead);
	StealMeshesFromObject(iCharObj, iCharObjLegs);
	DeleteObject(iCharObjLegs);
	StealMeshesFromObject(iCharObj, iCharObjFeet);
	DeleteObject(iCharObjFeet);
	if (ObjectExist(iCharObjHair) == 1)
	{
		// ensure hair has no culling and semi-transparent
		SetObjectTransparency(iCharObjHair, 2);
		SetObjectCull(iCharObjHair, 0);
		// wicked handles this differently
		StealMeshesFromObject(iCharObj, iCharObjHair);
		DeleteObject(iCharObjHair);
	}
	if (ObjectExist(iCharObjEyeglasses) == 1)
	{
		SetObjectCull(iCharObjEyeglasses, 0);
		DisableObjectZWriteEx(iCharObjEyeglasses, true);
		SetObjectTransparency(iCharObjEyeglasses, 2);
		StealMeshesFromObject(iCharObj, iCharObjEyeglasses);
		DeleteObject(iCharObjEyeglasses);
	}
	if (ObjectExist(iCharObjFacialHair) == 1)
	{
		SetObjectTransparency(iCharObjFacialHair, 2);
		SetObjectCull(iCharObjFacialHair, 0);
		StealMeshesFromObject(iCharObj, iCharObjFacialHair);
		DeleteObject(iCharObjFacialHair);
	}

	if (ObjectExist(iCharObjAccessory1) == 1)
	{
		StealMeshesFromObject(iCharObj, iCharObjAccessory1);
		DeleteObject(iCharObjAccessory1);
	}
	if (ObjectExist(iCharObjAccessory2) == 1)
	{
		StealMeshesFromObject(iCharObj, iCharObjAccessory2);
		DeleteObject(iCharObjAccessory2);
	}

	// as character parts have no animations, wipe out ones they do have
	// and replace with the latest animation set for this base mesh
	cstr final_name = "charactercreatorplus\\animations\\sets\\";
	final_name = final_name + CCP_Type;
	final_name = final_name +"\\default animations" + CCPMODELEXT;
	if (FileExist(final_name.Get()))
	{
		AppendObject(final_name.Get(), iCharObj, 0);
	}
	// and trigger animation to be prepped
	g_bCharacterCreatorPrepAnims = true;

	// some DBOs are created with BLACK base color, so for character creator
	// objects set them always to WHITE with full ALPHA
	SetObjectDiffuseEx(iCharObj, 0xFFFFFFFF, 0);

	// character gone through extensive changes, ensure wicked is updated
	sObject* pObjectToRecreateInWicked = GetObjectData(iCharObj);
	WickedCall_AddObject( pObjectToRecreateInWicked );
	WickedCall_UpdateObject ( pObjectToRecreateInWicked );
	WickedCall_TextureObject ( pObjectToRecreateInWicked, NULL);
	// and set character to use full reflectance (as all body parts have alpha data in surface texture)
	// PE: reduced from 1.0 to 0.002 as it gives a bluish shine on the edges of the ccp.
	for (int iMeshIndex = 0; iMeshIndex < pObjectToRecreateInWicked->iMeshCount; iMeshIndex++)
	{
		//LB: Changed back to 0.04 reflectance to solve washed out look on surfaces with no/minimal normal map
		//WickedCall_SetReflectance(pObjectToRecreateInWicked->ppMeshList[iMeshIndex], 1.0f);
		char * pNameFromTexture = pObjectToRecreateInWicked->ppMeshList[iMeshIndex]->pTextures[0].pName;
		bool bNeedReflectance = false;
		if (pNameFromTexture && pestrcasestr(pNameFromTexture, "glasses")) bNeedReflectance = true;
		if (bNeedReflectance)
			WickedCall_SetReflectance(pObjectToRecreateInWicked->ppMeshList[iMeshIndex], 1.0f);
		else
			WickedCall_SetReflectance(pObjectToRecreateInWicked->ppMeshList[iMeshIndex], 0.04f);
	}

	// place character in scene
	float terrain_height = BT_GetGroundHeight(t.terrain.TerrainID, GGORIGIN_X, GGORIGIN_Z, 1);
	fCharObjectY = terrain_height;
	PositionObject(iCharObj, ccpObjTargetX, ccpObjTargetY, ccpObjTargetZ);
	RotateObject(iCharObj, ccpObjTargetAX, ccpObjTargetAY, ccpObjTargetAZ);
	SetObjectArtFlags(iCharObj, (1 << 1) + (0), 0);
	LoopObject(iCharObj, 15, 55);

	// set default object animation speed
	SetObjectSpeed(iCharObj, 100);

	// if it highly likely new users will go from male, female, girl and boy,
	// so anticipate this by preloading the default sets of each just after a type selected
	charactercreatorplus_preloadallcharacterpartchoices(); 

	// finished legacy loading
	image_setlegacyimageloading(false);

	g_MeshesThatNeedDoubleSided.clear();
	if (stricmp(CCP_Type, "adult male") == 0)
	{
		g_MeshesThatNeedDoubleSided.push_back("adult male headgear 18");
		g_MeshesThatNeedDoubleSided.push_back("adult male body 20");
		g_MeshesThatNeedDoubleSided.push_back("adult male legs 16");
		g_MeshesThatNeedDoubleSided.push_back("adult male legs 17");
		g_MeshesThatNeedDoubleSided.push_back("adult male legs 18");
		g_MeshesThatNeedDoubleSided.push_back("adult male headgear 20");
		g_MeshesThatNeedDoubleSided.push_back("adult male headgear 19");
		g_MeshesThatNeedDoubleSided.push_back("adult male body 21");
	}
	else if (stricmp(CCP_Type, "adult female") == 0)
	{
		g_MeshesThatNeedDoubleSided.push_back("adult female headgear 11");
		g_MeshesThatNeedDoubleSided.push_back("adult female body 14");
	}

	// Set default character behavior
	char script[260];
	switch (iBase)
	{
		case 1: strcpy(script, "character_attack.lua");
			break;
		case 2: strcpy(script, "character_attack.lua");
			break;
		case 3: strcpy(script, "zombie_attack.lua");
			break;
		case 4: strcpy(script, "zombie_attack.lua");
			break;
		default: strcpy(script, "patrol.lua");
			break;
	}
	strcpy(CCP_Script, script);
}

void charactercreatorplus_init(void)
{
	// Initialisation prompt 
	timestampactivity ( 0, "Start character creator plus initialisation" );
	g_CharacterCreatorPlus.bInitialised = true;

	// create voice list for choices
	pCCPVoiceSet = "";
	CCP_SelectedToken = 0;
	if (CreateListOfVoices() > 0)
	{
		pCCPVoiceSet = g_voiceList_s[0].Get();
		CCP_SelectedToken = g_voicetoken[0];
	}

	// hide other editor stuff
	terrain_paintselector_hide();
	waypoint_hideall ( );

	// load in base mesh list, objects and texture for initial character
	charactercreatorplus_refreshtype();

	// finished
	timestampactivity ( 0, "Finished character creator plus initialisation" );
}

void charactercreatorplus_free(void)
{
	// show editor stuff
	waypoint_showall();

	// deactivate character creator plus
	g_bCharacterCreatorPlusActivated = false;
}

bool charactercreatorplus_savecharacterentity ( int iCharObj, LPSTR pOptionalDBOSaveFile, int iThumbnailImage )
{
	// saves character FPE, DBO and BMP from current character
	char pEntityName[1024];
	cstr FPEFile_s, BMPFile_s, DBOFile_s;
	strcpy ( pEntityName, "" );
	if ( pOptionalDBOSaveFile != NULL )
	{
		strcpy ( pEntityName, pOptionalDBOSaveFile );
		pEntityName[strlen(pEntityName)-4] = 0;
		FPEFile_s = cstr(pEntityName)+".fpe";
		BMPFile_s = cstr(pEntityName)+".bmp";
		DBOFile_s = cstr(pEntityName)+".dbo";
	}

	// store old dir
	cstr olddir_s = GetDir();

	// check if user folder exists, if not create it
	if ( PathExist( cstr( g.fpscrootdir_s+"\\Files\\entitybank\\user").Get() )  ==  0 ) 
	{
		MakeDirectory ( cstr(g.fpscrootdir_s+"\\Files\\entitybank\\user").Get() );
	}
	if ( PathExist( cstr(g.fpscrootdir_s+"\\Files\\entitybank\\user\\charactercreatorplus").Get() )  ==  0 ) 
	{
		MakeDirectory ( cstr(g.fpscrootdir_s+"\\Files\\entitybank\\user\\charactercreatorplus").Get() );
	}

	// allow mouse for file dialog
	ShowMouse (  );

	// save dialog
	cstr SaveFile_s = "";
	if ( pOptionalDBOSaveFile == NULL )
	{
		int iInSaveDialog = 1;
		while ( iInSaveDialog == 1 ) 
		{
			SaveFile_s = openFileBox("FPSC Entity (.fpe)|*.fpe|All Files|*.*|", cstr(g.fpscrootdir_s+"\\Files\\entitybank\\user\\charactercreatorplus\\").Get(), "Save Character", ".fpe", CHARACTERKITSAVEFILE);
			if ( SaveFile_s == "Error" ) return false;
			iInSaveDialog = 0;
		}
		strcpy ( pEntityName, SaveFile_s.Get() );
		pEntityName[strlen(pEntityName)-4] = 0;
		FPEFile_s = cstr(pEntityName)+".fpe";
		BMPFile_s = cstr(pEntityName)+".bmp";
		DBOFile_s = cstr(pEntityName)+".dbo";
	}

	// get character name
	cstr CharacterName_s = "";
	for ( int n = strlen(pEntityName); n >= 1; n-- )
	{
		if ( pEntityName[n] == '\\' || pEntityName[n] == '/' ) 
		{
			CharacterName_s = pEntityName + n + 1;
			break;
		}
	}

	// wicked uses non-cusotm shader for character skin, so copy the prebaked skin to the export area as the required
	// amended textures for the character (better as it does not put extra burden on the shader!)
	// go through all meshes
	sObject* pSaveObject = GetObjectData(iCharObj);
	for ( int iM = 0; iM<pSaveObject->iMeshCount; iM++)
	{
		// for each valid mesh
		sMesh* pMesh = pSaveObject->ppMeshList[iM];
		if (pMesh)
		{
			// scan to see if a modified albedo is being used
			LPSTR pMeshAlbedoTexture = pMesh->pTextures[0].pName;
			if (pMeshAlbedoTexture && strstr(pMeshAlbedoTexture,"charactercreatorplus\\skins")!=NULL)
			{
				// if so, copy it to the export area under the same name as the new entity name
				char pExt[MAX_PATH];
				strcpy(pExt, pMeshAlbedoTexture);
				strrev(pExt);
				pExt[5] = 0;
				strrev(pExt);
				cstr sColorFile = cstr(pEntityName) + pExt;

				// but to real path
				char pRealColorFile[MAX_PATH];
				strcpy(pRealColorFile, sColorFile.Get());
				GG_GetRealPath(pRealColorFile, 1);
				CopyFileA(pMeshAlbedoTexture, pRealColorFile, FALSE);

				// and then redirect the object to that texture, so when it is saved, it points
				// to the local texture, and not the temp one created inside "charactercreatorplus\skins"
				strcpy ( pMesh->pTextures[0].pName, sColorFile.Get() );
			}
		}
	}

	// before save, match created animsets to actual DBO structure
	extern void UpdateObjectWithAnimSlotList (sObject* pObject);
	UpdateObjectWithAnimSlotList(pSaveObject);

	// save DBO at specified location
	if ( FileExist(DBOFile_s.Get()) == 1 ) DeleteAFile ( DBOFile_s.Get() );
	SaveObject ( DBOFile_s.Get(), iCharObj );

	// character template
	cstr copyFrom_s = g.fpscrootdir_s+"\\Files\\charactercreatorplus\\Uber Character.fpe";

	// prepare destination file
    cstr copyTo_s = FPEFile_s;
	if ( FileExist(FPEFile_s.Get()) == 1 ) DeleteAFile ( FPEFile_s.Get() );
	if ( FileOpen(1) == 1 ) CloseFile(1);
	if ( FileOpen(2) == 1 ) CloseFile(2);
	OpenToRead ( 1, copyFrom_s.Get() );
	OpenToWrite ( 2, copyTo_s.Get() );

	// go through all source FPE
	int iCount = 0;
	while ( FileEnd(1) == 0 ) 
	{
		// get line by line
		bool bSkipWritingReadLine = false;
		cstr line_s = ReadString ( 1 );

		// update description
		if ( cstr(Lower(Left(line_s.Get(),4))) == "desc" ) line_s = cstr("desc             = ") + CharacterName_s;

		// write out how this character was made up		
		if ( cstr(Lower(Left(line_s.Get(),11))) == "ccpassembly" )
		{
			cstr pCCPAssemblyString = "";
			for (int partscan = 0; partscan < 10; partscan++)
			{
				char pTrunc[260];
				if (partscan == 0) strcpy(pTrunc, cSelectedHeadGear);
				if (partscan == 1) strcpy(pTrunc, cSelectedHair);
				if (partscan == 2)
				{
					strcpy(pTrunc, cSelectedHead);
					strcat(pTrunc, " [");
					strcat(pTrunc, cSelectedICCode);
					strcat(pTrunc, "]");
				}
				if (partscan == 3) strcpy(pTrunc, cSelectedEyeglasses);
				if (partscan == 4) strcpy(pTrunc, cSelectedFacialHair);
				if (partscan == 5) strcpy(pTrunc, cSelectedBody);
				if (partscan == 6) strcpy(pTrunc, cSelectedLegs);
				if (partscan == 7) strcpy(pTrunc, cSelectedFeet);
				if (partscan == 8) strcpy(pTrunc, cSelectedAccessory1);
				if (partscan == 9) strcpy(pTrunc, cSelectedAccessory2);

				if (stricmp(pTrunc, "none") != NULL)
				{
					pCCPAssemblyString += pTrunc;
					if (partscan < 9) pCCPAssemblyString += ",";
				}
			}
			WriteString ( 2, cstr(cstr("ccpassembly      = ") + pCCPAssemblyString).Get() );
			bSkipWritingReadLine = true;
		}

		// replace some fields for destination
		if ( cstr(Lower(Left(line_s.Get(),6))) == "aimain" )
		{
			// main behavior
			WriteString ( 2, cstr(cstr("aimain           = ") + g_CharacterCreatorPlus.obj.settings.script_s).Get() );
			WriteString ( 2, cstr(cstr("voice            = ") + g_CharacterCreatorPlus.obj.settings.voice_s).Get() );
			WriteString ( 2, cstr(cstr("speakrate        = ") + cstr(g_CharacterCreatorPlus.obj.settings.iSpeakRate)).Get() );

			// behavior
			if (stricmp (g_CharacterCreatorPlus.obj.settings.script_s.Get(), "people\\zombie_attack.lua") == NULL)
			{
				// sounds
				WriteString (2, "soundset         = audiobank\\characters\\zombie\\ZombieMunchRatLoop.wav");
				WriteString (2, "soundset1        = audiobank\\characters\\zombie\\ZombieRaiseHead.wav");
				WriteString (2, "soundset2        = audiobank\\characters\\zombie\\ZombieLungeAlt.wav");
				WriteString (2, "soundset3        = audiobank\\characters\\zombie\\ZombieDyingAlt.wav");
			}
			else
			{
				if (strstr (g_CharacterCreatorPlus.obj.settings.script_s.Get(), "attack"))
				{
					if (stricmp (g_CharacterCreatorPlus.obj.settings.voice_s.Get(), "Microsoft David Desktop - English (United States)") == NULL || strstr(g_CharacterCreatorPlus.obj.settings.voice_s.Get(), "David"))
					{
						WriteString (2, "soundset         = audiobank\\characters\\human\\male\\enemies_heard_something2.wav");
						WriteString (2, "soundset1        = audiobank\\characters\\human\\male\\enemyseesplayer1.wav");
						WriteString (2, "soundset2        = audiobank\\characters\\human\\male\\enemy_lost_sight1.wav");
						WriteString (2, "soundset3        = audiobank\\characters\\human\\male\\enemy_kills_player1.wav");
					}
					else
					{
						WriteString (2, "soundset         = audiobank\\characters\\human\\female\\enemies_heard_something3.wav");
						WriteString (2, "soundset1        = audiobank\\characters\\human\\female\\enemyseesplayer5.wav");
						WriteString (2, "soundset2        = audiobank\\characters\\human\\female\\enemy_lost_sight2.wav");
						WriteString (2, "soundset3        = audiobank\\characters\\human\\female\\enemy_kills_player1.wav");
					}
					// weapon details
					char pWeaponData[MAX_PATH];
					sprintf(pWeaponData, "hasweapon        = %s", g_grideleprof_holdchoices.hasweapon_s.Get()); WriteString (2, pWeaponData);
					sprintf(pWeaponData, "cantakeweapon    = %d", g_grideleprof_holdchoices.cantakeweapon); WriteString (2, pWeaponData);
					sprintf(pWeaponData, "quantity         = %d", g_grideleprof_holdchoices.quantity); WriteString (2, pWeaponData);
				}
				else
				{
					WriteString (2, "hasweapon        =");
					WriteString (2, "cantakeweapon    = 0");
					WriteString (2, "quantity         = 0");
				}
			}
			WriteString (2, "ragdoll          = 1");
			WriteString (2, "");
			WriteString (2, "; flesh");
			WriteString (2, "decalmax         = 1");
			WriteString (2, "decal0           = blood");
			WriteString (2, "strength         = 500");
			WriteString (2, "materialindex    = 6");
			bSkipWritingReadLine = true;
		}
		if ( cstr(Lower(Left(line_s.Get(),5))) == "model" )
		{
			WriteString ( 2, cstr(cstr("model            = ") + CharacterName_s + ".dbo").Get() );
			bSkipWritingReadLine = true;
		}
		if ( cstr(Lower(Left(line_s.Get(),8))) == "textured" )
		{
			WriteString ( 2, cstr(cstr("textured         = ") + "").Get() );
			bSkipWritingReadLine = true;
			//PE: Missing "textureref1 = " for save standalone.
		}
		if (cstr(Lower(Left(line_s.Get(), 5))) == "anim0")
		{
			int iIdleStart = 0;
			int iIdleFinish = 0;
			extern std::vector<sAnimSlotStruct> g_pAnimSlotList;
			for (int slot = 0; slot < (int)g_pAnimSlotList.size(); slot++)
			{
				char pLwr[MAX_PATH];
				strcpy (pLwr, g_pAnimSlotList[slot].pName);
				strlwr(pLwr);
				if (strstr (pLwr, "idle") != NULL)
				{
					iIdleStart = g_pAnimSlotList[slot].fStart;
					iIdleFinish = g_pAnimSlotList[slot].fFinish;
					break;
				}
			}
			char pCorrectIdleAnim[MAX_PATH];
			sprintf(pCorrectIdleAnim, "anim0            = %d,%d", iIdleStart, iIdleFinish);
			WriteString (2, pCorrectIdleAnim);
			bSkipWritingReadLine = true;
		}
		
		// write line (changed or not) to the destination FPE
		if ( bSkipWritingReadLine == false )
		{
			WriteString ( 2, line_s.Get() );
		}
	}

	// Prevent the light above the character from influencing the thumbnail.
	if (iLightIndex >= 0)
	{
		WickedCall_DeleteLight(iLightIndex);
		iLightIndex = -1;
	}

	// additionally, to retain material settings per mesh, save out mesh settings
	extern void imporer_save_multimeshsection(sObject* pObject, int iFileIndex);
	imporer_save_multimeshsection(pSaveObject, 2);

	// close file handling
	CloseFile ( 1 );
	CloseFile ( 2 );
	
	// save thumbnail file
	if (iThumbnailImage > 0)
	{
		if (ImageExist(iThumbnailImage) == 0)
		{
			image_setlegacyimageloading(true);
			LoadImage("editors\\uiv3\\ThumbnailTemplate.png", iThumbnailImage);
			image_setlegacyimageloading(false);
		}
		char pRealBMPFile[MAX_PATH];
		strcpy(pRealBMPFile, BMPFile_s.Get());
		GG_GetRealPath(pRealBMPFile, 1);
		if (FileExist(pRealBMPFile) == 1) DeleteAFile(pRealBMPFile);
		SaveImage(pRealBMPFile, iThumbnailImage);
	}

	// restore old dir
	SetDir ( olddir_s.Get() );

	// success
	return true;
}

void charactercreatorplus_loop(void)
{
	if ( g_CharacterCreatorPlus.bInitialised == false )
	{
		// set up for character editing
		charactercreatorplus_init();
	}
	else
	{
		if (t.inputsys.wheelmousemove != 0.0f)
		{
			g_fCCPZoom += (t.inputsys.wheelmousemove * ImGui::GetIO().DeltaTime * 250.0f);

			if (g_fCCPZoom > 100.0f)
				g_fCCPZoom = 100.0f;
			else if (g_fCCPZoom < 0.0f)
				g_fCCPZoom = 0.0f;

			charactercreatorplus_dozoom();
		}
	}
}

// Character Creator for Multiplayer

void characterkitplus_chooseOnlineAvatar ( void )
{
	// new 'simple' method, select from file requester 
	char pOldDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, pOldDir);
	SetDir(g.fpscrootdir_s.Get());
	SetDir("Files\\entitybank\\user\\charactercreatorplus");
	char pCCPDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, pCCPDir);
	t.tpickedAvatar_s = openFileBox("Character Files|*.bmp|", pCCPDir, "Select Character", ".bmp", 1);
	if (t.tpickedAvatar_s == "Error")
	{
		SetDir(pOldDir);
		return;
	}
	if (FileExist(t.tpickedAvatar_s.Get()) == 1)
	{
		t.tpickedAvatar_s = Right(t.tpickedAvatar_s.Get(), (Len(t.tpickedAvatar_s.Get()) - Len(pCCPDir))-1 );
		t.tpickedAvatar_s = Left(t.tpickedAvatar_s.Get(), Len(t.tpickedAvatar_s.Get()) - 4);
		SetDir(pOldDir);
		characterkitplus_saveAvatarInfo();
	}
	SetDir(pOldDir);
}

void characterkitplus_saveAvatarInfo ( void )
{
	t.tavatarstring_s = "";
	if ( FileExist( cstr(cstr("entitybank\\user\\charactercreatorplus\\")+t.tpickedAvatar_s+".fpe").Get() )  ==  1 ) 
	{
		// grab the character creator string
		t.tavatarstring_s = "";
		if ( FileOpen(1) == 1 ) CloseFile ( 1 );
		OpenToRead ( 1, cstr(cstr("entitybank\\user\\charactercreatorplus\\")+t.tpickedAvatar_s+".fpe").Get() );
		while ( FileEnd(1) == 0 ) 
		{
			t.tline_s = ReadString ( 1 );
			t.tcciStat_s = Lower(FirstToken( t.tline_s.Get(), " "));
			if ( t.tcciStat_s == "ccpassembly" ) 
			{
				LPSTR pStr = t.tline_s.Get();
				for (int n = 0; n < strlen(pStr); n++)
				{
					if (pStr[n] == '=')
					{
						n++; if (pStr[n] == ' ') n++;
						t.tavatarstring_s = pStr + n;
					}
				}
				break;
			}
		}
		CloseFile (  1 );

		// write out multiplayeravatar.dat file
		if ( FileExist( cstr(g.fpscrootdir_s + "\\multiplayeravatar.dat").Get() ) == 1 ) DeleteAFile ( cstr(g.fpscrootdir_s + "\\multiplayeravatar.dat").Get() );
		OpenToWrite ( 1, cstr(g.fpscrootdir_s + "\\multiplayeravatar.dat").Get() );
		WriteString ( 1,t.tavatarstring_s.Get() );
		WriteString ( 1,t.tpickedAvatar_s.Get() );
		CloseFile ( 1 );

		// load in the avatar for multiplayer
		characterkitplus_loadMyAvatarInfo();
	}
}

void characterkitplus_checkAvatarExists ( void )
{
	if ( FileOpen(1) == 1 ) CloseFile ( 1 );
	if ( FileExist( cstr(g.fpscrootdir_s + "\\multiplayeravatar.dat").Get() ) == 1 ) 
	{
		OpenToRead ( 1, cstr (g.fpscrootdir_s + "\\multiplayeravatar.dat").Get() );
		g.mp.myAvatar_s = ReadString ( 1 );
		g.mp.myAvatarHeadTexture_s = ReadString ( 1 );
		g.mp.myAvatarName_s = g.mp.myAvatarHeadTexture_s;
		CloseFile (  1 );
	}
}

void characterkitplus_loadMyAvatarInfo ( void )
{
	// blank out the data first
	g.mp.myAvatar_s = "";
	g.mp.myAvatarName_s = "";
	g.mp.myAvatarHeadTexture_s = "";
	g.mp.haveSentMyAvatar = 0;
	t.bTriggerAvatarRescanAndLoad = false;
	for ( t.c = 0 ; t.c <= MP_MAX_NUMBER_OF_PLAYERS-1; t.c++ )
	{
		t.mp_playerAvatars_s[t.c] = "";
		t.mp_playerAvatarOwners_s[t.c] = "";
		t.mp_playerAvatarLoaded[t.c] = false;
	}

	// open multiplayer avatar file
	if ( FileExist( cstr(g.fpscrootdir_s + "\\multiplayeravatar.dat").Get() ) == 1 ) 
	{
		OpenToRead ( 1, cstr (g.fpscrootdir_s + "\\multiplayeravatar.dat").Get() );
		g.mp.myAvatar_s = ReadString ( 1 );
		g.mp.myAvatarHeadTexture_s = ReadString ( 1 );
		g.mp.myAvatarName_s = g.mp.myAvatarHeadTexture_s;
		CloseFile ( 1 );

		if ( t.tShowAvatarSprite == 1 ) 
		{
			t.tShowAvatarSprite = 0;
			if ( g.charactercreatorEditorImageoffset > 1 ) 
			{
				if ( ImageExist(g.charactercreatorEditorImageoffset) == 1 ) DeleteImage ( g.charactercreatorEditorImageoffset );
				if ( FileExist( cstr(g.fpscrootdir_s+"\\Files\\entitybank\\user\\charactercreatorplus\\"+g.mp.myAvatarName_s+".bmp").Get() ) == 1 ) 
				{
					LoadImage ( cstr(g.fpscrootdir_s+"\\Files\\entitybank\\user\\charactercreatorplus\\"+g.mp.myAvatarName_s+".bmp").Get(), g.charactercreatorEditorImageoffset );
				}
			}
		}
	}
}

void characterkitplus_makeMultiplayerCharacterCreatorAvatar ( void )
{
	// delete it if it exists (it shouldn't, but just in case)
	if ( FileExist(t.avatarFile_s.Get()) == 1 ) DeleteAFile ( t.avatarFile_s.Get() );

	// Store old dir
	t.tolddir_s=GetDir();

	// Check if user folder exists, if not create it
	if ( PathExist( cstr(g.fpscrootdir_s+"\\Files\\entitybank\\user").Get() ) == 0 ) 
	{
		MakeDirectory ( cstr(g.fpscrootdir_s+"\\Files\\entitybank\\user").Get() );
	}
	if ( PathExist( cstr(g.fpscrootdir_s+"\\Files\\entitybank\\user\\charactercreatorplus").Get() ) == 0 ) 
	{
		MakeDirectory ( cstr(g.fpscrootdir_s+"\\Files\\entitybank\\user\\charactercreatorplus").Get() );
	}

	// create correct name
	t.tSaveFile_s = t.avatarFile_s;
	t.tname_s = t.tSaveFile_s;
	if ( cstr(Lower(Right(t.tname_s.Get(),4))) == ".fpe"  )  t.tname_s = Left(t.tname_s.Get(),Len(t.tname_s.Get())-4);
	for ( t.tloop = Len(t.tname_s.Get()) ; t.tloop >= 1 ; t.tloop+= -1 )
	{
		if ( cstr(Mid(t.tname_s.Get(),t.tloop)) == "\\" || cstr(Mid(t.tname_s.Get(),t.tloop)) == "/" ) 
		{
			t.tname_s = Right(t.tname_s.Get(),Len(t.tname_s.Get())- t.tloop);
			break;
		}
	}

	// template reference
	t.tcopyfrom_s = g.fpscrootdir_s+"\\Files\\charactercreatorplus\\Uber Character.fpe";
	t.tcopyto_s = t.tSaveFile_s;
	if ( cstr(Lower(Right(t.tcopyto_s.Get(),4))) != ".fpe" ) t.tcopyto_s = t.tcopyto_s + ".fpe";

	// now modify the copy
	if ( FileOpen(1) ==  1 ) CloseFile ( 1 );
	if ( FileOpen(2) ==  1 ) CloseFile ( 2 );
	OpenToRead ( 1, t.tcopyfrom_s.Get() );
	OpenToWrite ( 2, t.tcopyto_s.Get() );
	while ( FileEnd(1) == 0 ) 
	{
		// line by line
		t.ts_s = ReadString ( 1 );
		if ( cstr(Lower(Left(t.ts_s.Get(),4))) == "desc" ) t.ts_s = cstr("desc           =  ") + t.tname_s;

		// replace model specified 
		if ( cstr(Lower(Left(t.ts_s.Get(),5))) == "model" ) t.ts_s = cstr("model          =  ") + t.tname_s + ".dbo";

		// replace ccpassembly field
		if ( cstr(Lower(Left(t.ts_s.Get(),11))) == "ccpassembly" ) t.ts_s = cstr(cstr("ccpassembly      = ") + t.avatarString_s).Get();

		// write back out
		WriteString ( 2, t.ts_s.Get() );
	}
	CloseFile ( 1 );
	CloseFile ( 2 );

	// created FPE, but have no DBO, so need to create one here
	char pTmpDBO[MAX_PATH];
	strcpy(pTmpDBO, t.tcopyto_s.Get());
	pTmpDBO[strlen(pTmpDBO)-4] = 0;
	strcat(pTmpDBO, ".dbo");

	// setup character parts, then trigger its creation (adult male hair 11,adult male head 01,adult male body 03,adult male legs 04e,adult male feet 04)
	iCharObj = g.characterkitobjectoffset + 1;
	iCharObjHeadGear = g.characterkitobjectoffset + 2;
	iCharObjHair = g.characterkitobjectoffset + 3;
	iCharObjHead = g.characterkitobjectoffset + 4;
	iCharObjEyeglasses = g.characterkitobjectoffset + 5;
	iCharObjFacialHair = g.characterkitobjectoffset + 6;
	iCharObjLegs = g.characterkitobjectoffset + 7;
	iCharObjFeet = g.characterkitobjectoffset + 8;
	iCharObjAccessory1 = g.characterkitobjectoffset + 9;
	iCharObjAccessory2 = g.characterkitobjectoffset + 10;


	strcpy(cSelectedLegsFilter, "");
	strcpy(cSelectedFeetFilter, "");
	strcpy(cSelectedICCode, "IC1a");
	strcpy(cSelectedHeadGear, "None");
	strcpy(cSelectedHair, "None");
	strcpy(cSelectedHead, "None");
	strcpy(cSelectedEyeglasses, "None");
	strcpy(cSelectedFacialHair, "None");
	strcpy(cSelectedBody, "None");
	strcpy(cSelectedLegs, "None");
	strcpy(cSelectedFeet, "None");
	strcpy(cSelectedAccessory1, "None");
	strcpy(cSelectedAccessory2, "None");
	
	char pICTag[MAX_PATH];
	strcpy(pICTag, "IC1a");
	char pBasePath[MAX_PATH];
	strcpy(pBasePath, "");
	char pAvatarAssembly[MAX_PATH];
	strcpy(pAvatarAssembly, t.avatarString_s.Get());
	int n = 0;
	while ( n <= strlen(pAvatarAssembly) )
	{
		bool bLastItem = false;
		if (n == strlen(pAvatarAssembly)) bLastItem = true;
		if ((n < strlen(pAvatarAssembly) && pAvatarAssembly[n] == ',') || bLastItem == true)
		{
			// get each part name
			char pPartName[MAX_PATH];
			strcpy(pPartName, pAvatarAssembly);
			pPartName[n] = 0;

			// find the part type
			if (strstr(pPartName, "headgear") != 0) strcpy(cSelectedHeadGear, pPartName);
			if (strstr(pPartName, "hair") != 0) strcpy(cSelectedHair, pPartName);
			if (strstr(pPartName, "head") != 0)
			{
				// head assembly name can include IC code (for avatar recreation)
				// get base
				strcpy(pBasePath, "charactercreatorplus\\parts\\");
				strcat(pBasePath, pPartName);
				LPSTR pHeadToken = strstr(pBasePath, " head");
				if (pHeadToken) *pHeadToken = 0;
				strcat(pBasePath, "\\");

				// find IC part
				LPSTR pICToken = strstr(pPartName, "[");
				if (pICToken)
				{
					// cut off IC part for selectedhead string
					strcpy(cSelectedHead, pPartName);
					LPSTR pICCutOff = strstr(cSelectedHead, " [");
					if (pICCutOff) *pICCutOff = 0;

					// and keep the IC part for the tag
					strcpy(pICTag, pICToken + 1);
					if (pICTag[strlen(pICTag) - 1] == ']')
						pICTag[strlen(pICTag) - 1] = 0;
				}
				else
				{
					strcpy(cSelectedHead, pPartName);
				}
			}
			if (strstr(pPartName, "eyeglasses") != 0) strcpy(cSelectedEyeglasses, pPartName);
			if (strstr(pPartName, "facialhair") != 0) strcpy(cSelectedFacialHair, pPartName);
			if (strstr(pPartName, "body") != 0) strcpy(cSelectedBody, pPartName);
			if (strstr(pPartName, "legs") != 0) strcpy(cSelectedLegs, pPartName);
			if (strstr(pPartName, "feet") != 0) strcpy(cSelectedFeet, pPartName);
			if (strstr(pPartName, "accessory1") != 0) strcpy(cSelectedAccessory1, pPartName);
			if (strstr(pPartName, "accessory2") != 0) strcpy(cSelectedAccessory2, pPartName);

			// prepare to get next one
			if (bLastItem == false)
			{
				strcpy(pAvatarAssembly, pAvatarAssembly + n + 1);
				n = 0;
			}
			else
			{
				strcpy(pAvatarAssembly, "");
				n = 1;
			}
		}
		else
		{
			n++;
		}
	}
	// now make a character object from parts
	image_preload_files_wait();
	object_preload_files_wait();
	charactercreatorplus_change(pBasePath, -1, pICTag);
	if (ObjectExist(iCharObj) == 1)
	{
		// save DBO at specified location
		if (FileExist(pTmpDBO) == 1) DeleteAFile(pTmpDBO);
		SaveObject(pTmpDBO, iCharObj);

		// and finally delete the unneeded object
		DeleteObject(iCharObj);
	}

	// Restore old dir
	SetDir(t.tolddir_s.Get());
}

void characterkitplus_removeMultiplayerCharacterCreatorAvatar(void)
{
	// remove temp FPE file
	if (FileExist(t.avatarFile_s.Get()) == 1) DeleteAFile(t.avatarFile_s.Get());

	// remove temp DBO file
	char pTmpDBO[MAX_PATH];
	strcpy(pTmpDBO, t.avatarFile_s.Get());
	pTmpDBO[strlen(pTmpDBO) - 4] = 0;
	strcat(pTmpDBO, ".dbo");
	if (FileExist(pTmpDBO) == 1) DeleteAFile(pTmpDBO);
}

void charactercreatorplus_initautoswaps()
{
	// Clear any existing auto swap data.
	g_headGearMandatorySwaps.clear();
	g_previousAutoSwap = nullptr;
	g_SkinTextureStorage[0] = 0;

	std::vector<std::string> autoSwapFiles;

	// Determine the directory to search for auto swap files, based on the currently selected base type.
	cstr olddir_s = GetDir();
	char newDirectory[260] = { 0 };

	if (stricmp(CCP_Type, "adult male") == NULL)
	{
		strcpy(newDirectory, "charactercreatorplus\\parts\\adult male");
	}
	else if (stricmp(CCP_Type, "adult female") == NULL)
	{
		strcpy(newDirectory, "charactercreatorplus\\parts\\adult female");
	}
	else if (stricmp(CCP_Type, "zombie male") == NULL)
	{
		strcpy(newDirectory, "charactercreatorplus\\parts\\zombie male");
	}
	else if (stricmp(CCP_Type, "zombie female") == NULL)
	{
		strcpy(newDirectory, "charactercreatorplus\\parts\\zombie female");
	}
	else
	{
		int iBaseValue = GetBaseValueFromCCPType(CCP_Type);
		if (iBaseValue > 1)
		{
			strcpy(newDirectory, "charactercreatorplus\\parts\\");
			strcat(newDirectory, g_CharacterType[iBaseValue-1].pPartsFolder);
		}
	}
	if (strlen(newDirectory) > 0)
	{
		SetDir(newDirectory);

		// Find all of the <abc>autoswaps.txt files.
		for (int c = 1; c <= ChecklistQuantity(); c++)
		{
			cStr tfile_s = Lower(ChecklistString(c));
			if (tfile_s != "." && tfile_s != "..")
			{
				if (strstr(tfile_s.Get(), "autoswaps"))
				{
					if (strcmp(Right(tfile_s.Get(), 4), ".txt") == 0)
					{
						std::string file(tfile_s.Get());
						autoSwapFiles.push_back(file);
					}
				}
			}
		}

		for (int i = 0; i < autoSwapFiles.size(); i++)
		{
			charactercreatorplus_getautoswapdata((char*)autoSwapFiles[i].c_str());
		}

		SetDir(olddir_s.Get());

	}
}

int current_dress_room = -999;
void change_dress_room(int charactertypeindex)
{
	// if charactertypeindex is negative, specifying roomID!
	int room = fabs(charactertypeindex);
	if (charactertypeindex>=0) room = g_CharacterTypeRoomPref[charactertypeindex];
	if (room != current_dress_room)
	{
		current_dress_room = room;
		strcpy(CCP_Room, g_RoomType[current_dress_room].pPartsFolder);

		iDressRoom = g.characterkitobjectoffset + 16;
		int iDressRoomImage = g.charactercreatorEditorImageoffset + 121;
		if (ObjectExist(iDressRoom)) DeleteObject(iDressRoom);
		if (ImageExist(iDressRoomImage)) DeleteImage(iDressRoomImage);

		// Choose the visual settings based on the dress room.
		visualsdatastoragetype newVisuals;
		if (iLightIndex >= 0)
		{
			WickedCall_DeleteLight(iLightIndex);
			iLightIndex = -1;
		}
		
		wiScene::MaterialComponent::SHADERTYPE shadertype = wiScene::MaterialComponent::SHADERTYPE_PBR;

		// switch to new room
		LPSTR pRomFolder = g_RoomType[current_dress_room].pPartsFolder;
		char pRoomFilePath[MAX_PATH];
		sprintf(pRoomFilePath, "charactercreatorplus\\rooms\\%s\\room.dbo", pRomFolder); LoadObject(pRoomFilePath, iDressRoom);
		sprintf(pRoomFilePath, "charactercreatorplus\\rooms\\%s\\room_color.dds", pRomFolder); LoadImage(pRoomFilePath, iDressRoomImage);
		newVisuals.SunIntensity_f = 0.0f;
		wiScene::Scene& scene = wiScene::GetScene();
		iLightIndex = scene.Entity_CreateLight("CCPLight", XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), 7, 550, wiScene::LightComponent::LightType::POINT);
		WickedCall_UpdateLight(iLightIndex, g_HeadTransition.to[0] + 100, g_HeadTransition.to[1] + 40, g_HeadTransition.to[2] - 110, 0, 0, 0, 550, 0, 255, 255, 255, true);

		// The current settings will not be stored, as they are already stored in t.visualsStorage, so create throwaway object.
		visualsdatastoragetype throwaway;
		set_temp_visuals(t.visuals, throwaway, newVisuals);
		visuals_loop();
		TextureObject(iDressRoom, 0, iDressRoomImage);

		sObject* pObject = GetObjectData(iDressRoom);
		if (pObject)
		{
			WickedCall_SetObjectCastShadows(pObject, false);
			for (int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++)
			{
				sMesh* pMesh = pObject->ppMeshList[iMesh];
				if (pMesh)
				{
					//SHADERTYPE_UNLIT need higher diffuse values ?
					pMesh->mMaterial.Diffuse.r = 1.0f;
					pMesh->mMaterial.Diffuse.g = 1.0f;
					pMesh->mMaterial.Diffuse.b = 1.0f;
					pMesh->mMaterial.Diffuse.a = 1.0f;
					wiScene::MeshComponent* mesh = wiScene::GetScene().meshes.GetComponent(pMesh->wickedmeshindex);
					if (mesh)
					{
						uint64_t materialEntity = mesh->subsets[0].materialID;
						wiScene::MaterialComponent* pObjectMaterial = wiScene::GetScene().materials.GetComponent(materialEntity);
						if (pObjectMaterial)
						{
							pObjectMaterial->SetReflectance(0.0f);
							pObjectMaterial->shaderType = shadertype;
							
							//PE: Also ignoes all other material settings , so its perfect.
							pObjectMaterial->SetDirty(true);
						}
					}
					WickedCall_SetMeshMaterial(pMesh,true);
				}
			}
			WickedCall_SetObjectMetalness(pObject, 0.0f);
			WickedCall_SetObjectRoughness(pObject, 0.0f);
		}
	}
}

void charactercreatorplus_imgui_v3(void)
{
	extern bool bImGuiGotFocus;
	extern bool bForceKey;
	extern cstr csForceKey;
	extern bool bEntity_Properties_Window;
	bool once_camera_adjust = false;
	if (g_bCharacterCreatorPlusActivated)
	{
		if (!g_bPartIconsInit)
		{
			for (int b = 0; b < 32; b++)
			{
				for (int i = 0; i < 10; i++)
				{
					for (int a = 0; a < MAXPARTICONS; a++)
						g_iPartsIconsIDs[b][i][a] = -1;
				}
			}
			g_bPartIconsInit = true;
		}

		if (g_CharacterCreatorPlus.bInitialised)
		{
			// handle thread dependent triggers (smooth UI) 
			charactercreatorplus_waitforpreptofinish();

			charactercreatorplus_performcameratransition();

			// handle in-level visuals
			if (!bCharObjVisible)
			{
				oldtcameraviewmode = t.cameraviewmode;
				editoroldmode_f = t.editorfreeflight.mode;
				editoroldx_f = t.editorfreeflight.c.x_f;
				if (editoroldx_f == 0) editoroldx_f = 0.0001;
				editoroldy_f = t.editorfreeflight.c.y_f;
				editoroldz_f = t.editorfreeflight.c.z_f;
				editoroldangx_f = t.editorfreeflight.c.angx_f;
				editoroldangy_f = t.editorfreeflight.c.angy_f;

				t.editorfreeflight.c.angx_f = 16.0;
				t.editorfreeflight.c.angy_f = 315.0;
				RotateCamera(16.0, 315.0, 0.0); //To always get the same light.

				ShowObject(iCharObj);
				bCharObjVisible = true;

				//Hide entities.
				//Exit properties.
				bForceKey = true;
				csForceKey = "e";

				t.inputsys.dowaypointview = 1;
				t.inputsys.domodeentity = 1;

				widget_hide();
				ebe_hide();
				terrain_paintselector_hide();
				t.geditorhighlightingtentityobj = 0;
				t.geditorhighlightingtentityID = 0;
				editor_restoreentityhighlightobj();
				gridedit_clearentityrubberbandlist();
				waypoint_hideall();

				ccpTargetX = t.editorfreeflight.c.x_f;
				ccpTargetY = t.editorfreeflight.c.y_f;
				ccpTargetZ = t.editorfreeflight.c.z_f;
				ccpTargetAX = t.editorfreeflight.c.angx_f;
				ccpTargetAY = t.editorfreeflight.c.angy_f;

				fCharObjectY = 180000.0f;

				float oangx = ObjectAngleX(iCharObj);
				float oangz = ObjectAngleZ(iCharObj);

				//PE: a simple z,x mouse from center of screen.
				float placeatx_f, placeatz_f;
				extern ImVec2 OldrenderTargetSize;
				extern ImVec2 OldrenderTargetPos;
				extern ImVec2 renderTargetAreaSize;
				extern ImVec2 renderTargetAreaPos;
				extern bool bWaypointDrawmode;

				ImVec2 vCenterPos = { (OldrenderTargetSize.x*0.5f) + OldrenderTargetPos.x , (OldrenderTargetSize.y*0.45f) + OldrenderTargetPos.y };

				int omx = t.inputsys.xmouse, omy = t.inputsys.ymouse, oldgridentitysurfacesnap = t.gridentitysurfacesnap, oldonedrag = t.onedrag;;
				bool owdm = bWaypointDrawmode;

				//Always target terrain only.
				float RatioX = ((float)GetDisplayWidth() / (float)renderTargetAreaSize.x) * ((float)GetDisplayWidth() / (float)GetChildWindowWidth(-1));
				float RatioY = ((float)GetDisplayHeight() / (float)renderTargetAreaSize.y) * ((float)GetDisplayHeight() / (float)GetChildWindowHeight(-1));
				t.inputsys.xmouse = (vCenterPos.x - renderTargetAreaPos.x) * RatioX;
				t.inputsys.ymouse = (vCenterPos.y - renderTargetAreaPos.y) * RatioY;

				t.gridentitysurfacesnap = 0; t.onedrag = 0; bWaypointDrawmode = false;

				input_calculatelocalcursor();

				if (!(t.inputsys.picksystemused == 1 || t.inputsys.localcurrentterrainheight_f < 100.0f))
				{
					ccpTargetX = t.inputsys.localx_f;
					ccpTargetZ = t.inputsys.localy_f;
				}

				t.onedrag = oldonedrag;
				bWaypointDrawmode = owdm;
				t.gridentitysurfacesnap = oldgridentitysurfacesnap;
				t.inputsys.xmouse = omx;
				t.inputsys.ymouse = omy;
				//Restore real input.
				input_calculatelocalcursor();

				fCharObjectY = 180000.0f;

				t.editorfreeflight.c.x_f = ccpTargetX;
				t.editorfreeflight.c.z_f = ccpTargetZ;

				SetObjectToCameraOrientation(iCharObj);
				PositionObject(iCharObj, ccpTargetX, fCharObjectY, ccpTargetZ);
				RotateObject(iCharObj, oangx, ObjectAngleY(iCharObj), oangz);
				MoveObject(iCharObj, 130);

				// Set default visuals for the Character Creator, and restore them when leaving.
				visualsdatastoragetype desiredVisuals;
				set_temp_visuals(t.visuals, t.visualsStorage, desiredVisuals);
				visuals_loop();

				fCharObjectY = 180000.0f;
				PositionObject(iCharObj, ObjectPositionX(iCharObj), fCharObjectY, ObjectPositionZ(iCharObj));

				ccpObjTargetX = ObjectPositionX(iCharObj);
				ccpObjTargetY = ObjectPositionY(iCharObj);
				ccpObjTargetZ = ObjectPositionZ(iCharObj);
				ccpObjTargetAX = ObjectAngleX(iCharObj);
				ccpObjTargetAY = ObjectAngleY(iCharObj);
				ccpObjTargetAZ = ObjectAngleZ(iCharObj);

				t.editorfreeflight.mode = 1;
				t.editorfreeflight.c.y_f = fCharObjectY + 60;
				t.editorfreeflight.c.angx_f = 0;
				t.editorfreeflight.s = t.editorfreeflight.c;

				once_camera_adjust = true;

				//  "hide" all entities in map by moving them out the way
				for (t.tcce = 1; t.tcce <= g.entityelementlist; t.tcce++)
				{
					t.tccentid = t.entityelement[t.tcce].bankindex;
					if (t.tccentid > 0)
					{
						t.tccsourceobj = t.entityelement[t.tcce].obj;
						if (ObjectExist(t.tccsourceobj) == 1)
						{
							PositionObject(t.tccsourceobj, 0, 0, 0);
						}
					}
				}

				fCCPRotateY = ccpObjTargetAY = ObjectAngleY(iCharObj);
				fCCPRotateY -= 15.0f; //Turn it a bit.
				//PE: Make sure we are in slider range.
				if (fCCPRotateY < 0.0) fCCPRotateY += 360.0;
				if (fCCPRotateY > 360.0) fCCPRotateY -= 360.0;
				// refresh thumbnail

				ccpObjTargetAY = fCCPRotateY;

				charactercreatorplus_initcameratransitions();

				// Prevent user camera input.
				t.cameraviewmode = 9; //No mode.
				t.editorfreeflight.mode = 1;

				iDressRoom = g.characterkitobjectoffset + 16;
				int iDressRoomImage = g.charactercreatorEditorImageoffset + 121;

				cstr check = "";
				int charactertypeindex = 0;
				for (int i = 0; i < g_CharacterType.size(); i++)
				{
					if (i == 0)
						check = "adult male";
					else if (i == 1)
						check = "adult female";
					else if (i == 2)
						check = "zombie male";
					else if (i == 3)
						check = "zombie female";
					else
					{
						if (i < g_CharacterType.size())
						{
							check = g_CharacterType[i].pPartsFolder;
						}
					}
					if (stricmp(CCP_Type, check.Get()) == 0)
					{
						charactertypeindex = i;
						break;
					}
				}
				change_dress_room(charactertypeindex);
				SetObjectToCameraOrientation(iDressRoom);
				PositionObject(iDressRoom, ccpTargetX, fCharObjectY - g_fLockerRoomOffset, ccpTargetZ);
				dressroomTargetAY = ObjectAngleY(iCharObj);
				RotateObject(iDressRoom, oangx, dressroomTargetAY, oangz);
				MoveObject(iDressRoom, 150);
				ShowObject(iDressRoom);
				PositionObject(iDressRoom, ObjectPositionX(iDressRoom), fCharObjectY - g_fLockerRoomOffset, ObjectPositionZ(iDressRoom));		
				RotateObject(iCharObj, ObjectAngleX(iCharObj), fCCPRotateY, ObjectAngleZ(iCharObj));
			}
		
			//Display sky for better look.
			if (ObjectExist(t.terrain.objectstartindex + 4) == 1)
			{
				PositionObject(t.terrain.objectstartindex + 4, CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0));
				SetAlphaMappingOn(t.terrain.objectstartindex + 4, 100.0*t.sky.alpha1_f);
				ShowObject(t.terrain.objectstartindex + 4);
			}

			if (iDelayExecute == 1)
			{
				//PE: Change type.
				image_preload_files_wait();
				object_preload_files_wait();
				image_preload_files_reset(); //PE: Free all not used images from prev. type.

				charactercreatorplus_refreshtype();
				iDelayExecute = 0;
				ShowObject(iCharObj);
			}
			
			// handle preparing of animation data
			if (g_bCharacterCreatorPrepAnims == true)
			{
				int iUseDefaultNonCombatAnimations = 1;  // Adult Male/Female by default
				if (stricmp(CCP_Type, "zombie male") == NULL) iUseDefaultNonCombatAnimations = 2;
				extern void animsystem_prepareobjectforanimtool(int objectnumber, int iUseDefaultNonCombatAnimations);
				animsystem_prepareobjectforanimtool(iCharObj, iUseDefaultNonCombatAnimations);
				g_bCharacterCreatorPrepAnims = false;
			}

			//Enable this to disable all movement ... when g_bCharacterCreatorPlusActivated
			extern int iGenralWindowsFlags;
			ImGui::Begin("Character Creator##PropertiesWindow", &g_bCharacterCreatorPlusActivated, iGenralWindowsFlags);

			if (once_camera_adjust)
			{
				extern ImVec2 OldrenderTargetSize;
				extern ImVec2 OldrenderTargetPos;
				extern ImVec2 renderTargetAreaSize;
				PositionCamera(t.editorfreeflight.c.x_f, t.editorfreeflight.c.y_f, t.editorfreeflight.c.z_f);
				RotateCamera(t.editorfreeflight.c.angx_f, t.editorfreeflight.c.angy_f, 0);

				float camxadjust = renderTargetAreaSize.x - (ImGui::GetWindowPos().x - OldrenderTargetPos.x);

				if (camxadjust > 100.0f && camxadjust < GetDisplayWidth()) {
					camxadjust -= 100.0;
					camxadjust *= 0.068;
					MoveCameraLeft(g_pGlob->dwCurrentSetCameraID, -camxadjust);
					t.editorfreeflight.c.x_f = CameraPositionX();
					t.editorfreeflight.c.z_f = CameraPositionZ();;
				}
				once_camera_adjust = false;
			}
			int media_icon_size = 64; //96
			float col_start = 80.0f;
			ImGui::PushItemWidth(ImGui::GetFontSize()*10.0);
			
			// ZJ: Made global so when user has auto close enabled, the selection is still known.
			static int ccp_part_selection = 5;
			static int item_current_room_selection = 0;

			extern int iLastOpenHeader;
  																			//if no other headers are open this should be.                                                                 
			if (pref.bAutoClosePropertySections && iLastOpenHeader != 60 && iLastOpenHeader >= 60 && iLastOpenHeader <= 71)
				ImGui::SetNextItemOpen(false, ImGuiCond_Always);

			if (ImGui::StyleCollapsingHeader("Name And Type", ImGuiTreeNodeFlags_DefaultOpen)) {

				iLastOpenHeader = 60;
				float w = ImGui::GetWindowContentRegionWidth();

				ImGui::Indent(10);
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13)); //3
				ImGui::Text("Name");
				ImGui::SameLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
				ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY()));

				ImGui::PushItemWidth(-10);
				ImGui::InputText("##NameCCP", &CCP_Name[0], 250);
				if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;
				if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Character Name");

				ImGui::PopItemWidth();

				// Character Type
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
				ImGui::Text("Type");
				ImGui::SameLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
				ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY()));
				static int g_chartypes_item_count = 0;
				static char** g_chartypes_items = NULL;
				if (g_chartypes_item_count != g_CharacterType.size())
				{
					if (g_chartypes_items)
					{
						for (int i = 0; i < g_chartypes_item_count; i++) SAFE_DELETE(g_chartypes_items[i]);
						SAFE_DELETE(g_chartypes_items);
					}
					g_chartypes_item_count = g_CharacterType.size();
					g_chartypes_items = new char* [g_chartypes_item_count];
					for (int i = 0; i < g_chartypes_item_count; i++)
					{
						g_chartypes_items[i] = new char[256];
						strcpy(g_chartypes_items[i], pCharacterTypeDropDownList[i]);
					}
				}
				for (int i = 0; i < g_chartypes_item_count; i++)
				{
					if (pestrcasestr(CCP_Type, g_chartypes_items[i]))
					{
						item_current_type_selection = i;
						break;
					}
				}
				ImGui::PushItemWidth(-10);
				if (ImGui::Combo("##TypeCCP", &item_current_type_selection, g_chartypes_items, g_chartypes_item_count))
				{
					strcpy(CCP_Type, g_chartypes_items[item_current_type_selection]);
					iThumbsOffsetY = 0;
					if (item_current_type_selection == 2 || item_current_type_selection == 3) iThumbsOffsetY = 50;
					iDelayExecute = 1;
					DisplaySmallImGuiMessage("Loading ...");
					change_dress_room(item_current_type_selection);
					charactercreatorplus_changecameratransition(5);
					ccp_part_selection = 5;
					SetObjectToCameraOrientation(iDressRoom);
					PositionObject(iDressRoom, ccpTargetX, fCharObjectY - g_fLockerRoomOffset, ccpTargetZ);
					RotateObject(iDressRoom, ObjectAngleX(iCharObj), dressroomTargetAY, ObjectAngleZ(iCharObj));
					MoveObject(iDressRoom, 150);
					ShowObject(iDressRoom);
					fCharObjectY = 180000.0f;
					PositionObject(iDressRoom, ObjectPositionX(iDressRoom), fCharObjectY - g_fLockerRoomOffset, ObjectPositionZ(iDressRoom));
					HideObject(iCharObj);
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Character Type");

				// Dressing Room
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
				ImGui::Text("Room");
				ImGui::SameLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
				ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY()));
				static int g_roomtypes_item_count = 0;
				static char** g_roomtypes_items = NULL;
				if (g_roomtypes_item_count != g_RoomType.size())
				{
					if (g_roomtypes_items)
					{
						for (int i = 0; i < g_roomtypes_item_count; i++) SAFE_DELETE(g_roomtypes_items[i]);
						SAFE_DELETE(g_roomtypes_items);
					}
					g_roomtypes_item_count = g_RoomType.size();
					g_roomtypes_items = new char* [g_roomtypes_item_count];
					for (int i = 0; i < g_roomtypes_item_count; i++)
					{
						g_roomtypes_items[i] = new char[256];
						strcpy(g_roomtypes_items[i], pRoomTypeDropDownList[i]);
					}
				}
				for (int i = 0; i < g_roomtypes_item_count; i++)
				{
					if (stricmp(CCP_Room, g_roomtypes_items[i])==NULL)
					{
						item_current_room_selection = i;
						break;
					}
				}
				ImGui::PushItemWidth(-10);
				if (ImGui::Combo("##RoomCCP", &item_current_room_selection, g_roomtypes_items, g_roomtypes_item_count))
				{
					strcpy(CCP_Room, g_roomtypes_items[item_current_room_selection]);
					iThumbsOffsetY = 0;
					iDelayExecute = 1;
					DisplaySmallImGuiMessage("Loading ...");
					change_dress_room(item_current_room_selection * -1);

					// change preferred room for this character type and save locally 
					g_CharacterTypeRoomPref[item_current_type_selection] = item_current_room_selection;
					char pRoomPrefFile[MAX_PATH];
					sprintf(pRoomPrefFile, "charactercreatorplus\\parts\\%s\\roompref.txt", CCP_Type);
					GG_SetWritablesToRoot(1);
					GG_GetRealPath(pRoomPrefFile, 1);
					if (FileExist(pRoomPrefFile)) DeleteFileA(pRoomPrefFile);
					OpenToWrite(1, pRoomPrefFile);
					WriteString(1, CCP_Room);
					CloseFile(1);
					GG_SetWritablesToRoot(0);

					charactercreatorplus_changecameratransition(5);
					SetObjectToCameraOrientation(iDressRoom);
					PositionObject(iDressRoom, ccpTargetX, fCharObjectY - g_fLockerRoomOffset, ccpTargetZ);
					RotateObject(iDressRoom, ObjectAngleX(iCharObj), dressroomTargetAY, ObjectAngleZ(iCharObj));
					MoveObject(iDressRoom, 150);
					ShowObject(iDressRoom);
					fCharObjectY = 180000.0f;
					PositionObject(iDressRoom, ObjectPositionX(iDressRoom), fCharObjectY - g_fLockerRoomOffset, ObjectPositionZ(iDressRoom));
					HideObject(iCharObj);
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Dressing Room");

				ImGui::PopItemWidth();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 8)); //3
				ImGui::Indent(-10);
			}

			bool bZoom = false;

			if (pref.bAutoClosePropertySections && iLastOpenHeader != 61)
				ImGui::SetNextItemOpen(false, ImGuiCond_Always);

			if (ImGui::StyleCollapsingHeader("Customize", ImGuiTreeNodeFlags_DefaultOpen))
			{
				iLastOpenHeader = 61;

				static std::map<std::string, std::string> CharacterCreatorCurrent_s;
				static std::map<std::string, std::string> CharacterCreatorCurrentAnnotated_s;
				static std::map<std::string, std::string> CharacterCreatorCurrentAnnotatedTag_s;
				cstr field_name;
				char* combo_buffer = NULL;
				char* combo_annotated_buffer = NULL;
				int part_number = 0;

				int ccp_part_icons = 10;
				int ccp_part_icons_columns = 5;
				float entity_w = ImGui::GetContentRegionAvailWidth() - 10.0f;
				float fSpacer = 0.0f;
				//New icons.
				float ccp_part_image_size = entity_w / (float)ccp_part_icons_columns;
				ccp_part_image_size -= ((2.0f) * ccp_part_icons_columns) - 2.0f;
				
				//PE: CHECK CCP_ACCESSORY1
				int ccp_part_images[] = { CCP_HEAD, CCP_HAIR, CCP_BEARD, CCP_HAT, CCP_GLASSES, CCP_BODY, CCP_LEGS, CCP_FEET,CCP_ACCESSORY1,CCP_ACCESSORY2 };
				int ccp_part_order[] =  { 2       ,1        ,4         ,0       ,3           ,5        ,6        ,7		,8		,9 };
				cstr ccp_part_tooltip[] = {
					"Head",
					"Hair",
					"Facial Hair",
					"Head Gear",
					"Wearing",
					"Body",
					"Legs",
					"Feet",
					"Accessory1",
					"Accessory2"
				};

				if (ccp_part_selection < 0 || ccp_part_selection >= ccp_part_icons) ccp_part_selection = 0;

				if (strstr(CCP_Type, "Adult Female"))
				{
					ccp_part_tooltip[2] = "Tattoo";
					ccp_part_images[2] = CCP_TATTOO;
				}

				ImVec4 IconColor = ImVec4(1.0, 1.0, 1.0, 1.0);
				ImGui::Indent(4);
				for (int i = 0; i < ccp_part_icons; i++)
				{
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(fSpacer, 0.0f));
					if (ccp_part_selection == i)
					{
						ImVec4 tool_selected_col = ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram];
						ImGuiWindow* window = ImGui::GetCurrentWindow();
						ImVec2 padding = { 3.0, 3.0 };
						const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(ccp_part_image_size, ccp_part_image_size));
						window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
					}
					if (ImGui::ImgBtn(ccp_part_images[i], ImVec2(ccp_part_image_size, ccp_part_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), IconColor, ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, false)) //no bBoostIconColors
					{
						ccp_part_selection = i;

						charactercreatorplus_changecameratransition(ccp_part_selection);

						switch (ccp_part_selection)
						{
						case 0: charactercreatorplus_restrictpart(2);
							break;
						case 1: charactercreatorplus_restrictpart(1);
							break;
						case 2: charactercreatorplus_restrictpart(4);
							break;
						case 3: charactercreatorplus_restrictpart(0);
							break;
						case 4: charactercreatorplus_restrictpart(3);
							break;
						}
					}
					if (ImGui::IsItemHovered() ) ImGui::SetTooltip(ccp_part_tooltip[i].Get());

					ImVec2 restore_cursorpos = ImGui::GetCursorPos();
					if ((i + 1) % ccp_part_icons_columns != 0 && i != ccp_part_icons - 1)
						ImGui::SameLine();
				}
				ImGui::Indent(-4);

				//PE: We dont have the same order as the old, so convert orders.
				int part_loop = ccp_part_order[ccp_part_selection];
				part_number = part_loop;

				if (part_loop == 0)
				{
					CharacterCreatorCurrent_s = CharacterCreatorHeadGear_s;
					CharacterCreatorCurrentAnnotated_s = CharacterCreatorAnnotatedHeadGear_s;
					CharacterCreatorCurrentAnnotatedTag_s = CharacterCreatorAnnotatedTagHeadGear_s;
					field_name = "Head Gear";
					LPSTR pAnnotatedLabel = "None";
					if (strnicmp(cSelectedHeadGear, "None", 4) != NULL) pAnnotatedLabel = charactercreatorplus_findannotation(cSelectedHeadGear);
					combo_buffer = cSelectedHeadGear;
					combo_annotated_buffer = pAnnotatedLabel;
					part_number = part_loop;
				}
				if (part_loop == 1)
				{
					CharacterCreatorCurrent_s = CharacterCreatorHair_s;
					CharacterCreatorCurrentAnnotated_s = CharacterCreatorAnnotatedHair_s;
					CharacterCreatorCurrentAnnotatedTag_s = CharacterCreatorAnnotatedTagHair_s;
					field_name = "Hair";
					LPSTR pAnnotatedLabel = "None";
					if (strnicmp(cSelectedHair, "None", 4) != NULL) pAnnotatedLabel = charactercreatorplus_findannotation(cSelectedHair);
					combo_buffer = cSelectedHair;
					combo_annotated_buffer = pAnnotatedLabel;
					part_number = part_loop;
				}
				if (part_loop == 2)
				{
					CharacterCreatorCurrent_s = CharacterCreatorHead_s;
					CharacterCreatorCurrentAnnotated_s = CharacterCreatorAnnotatedHead_s;
					CharacterCreatorCurrentAnnotatedTag_s = CharacterCreatorAnnotatedTagHead_s;
					field_name = "Head";
					LPSTR pAnnotatedLabel = charactercreatorplus_findannotation(cSelectedHead);
					combo_buffer = cSelectedHead;
					combo_annotated_buffer = pAnnotatedLabel;
					part_number = part_loop;
				}
				if (part_loop == 3)
				{
					CharacterCreatorCurrent_s = CharacterCreatorEyeglasses_s;
					CharacterCreatorCurrentAnnotated_s = CharacterCreatorAnnotatedEyeglasses_s;
					CharacterCreatorCurrentAnnotatedTag_s = CharacterCreatorAnnotatedTagEyeglasses_s;
					field_name = "Wearing";
					LPSTR pAnnotatedLabel = "None";
					if (strnicmp(cSelectedEyeglasses, "None", 4) != NULL) pAnnotatedLabel = charactercreatorplus_findannotation(cSelectedEyeglasses);
					combo_buffer = cSelectedEyeglasses;
					combo_annotated_buffer = pAnnotatedLabel;
					part_number = part_loop;
				}

				if (part_loop == 8)
				{
					CharacterCreatorCurrent_s = CharacterCreatorAccessory1_s;
					CharacterCreatorCurrentAnnotated_s = CharacterCreatorAnnotatedAccessory1_s;
					CharacterCreatorCurrentAnnotatedTag_s = CharacterCreatorAnnotatedTagAccessory1_s;
					field_name = "Accessory";
					LPSTR pAnnotatedLabel = "None";
					if (strnicmp(cSelectedAccessory1, "None", 4) != NULL) pAnnotatedLabel = charactercreatorplus_findannotation(cSelectedAccessory1);
					combo_buffer = cSelectedAccessory1;
					combo_annotated_buffer = pAnnotatedLabel;
					part_number = part_loop;
				}
				if (part_loop == 9)
				{
					CharacterCreatorCurrent_s = CharacterCreatorAccessory2_s;
					CharacterCreatorCurrentAnnotated_s = CharacterCreatorAnnotatedAccessory2_s;
					CharacterCreatorCurrentAnnotatedTag_s = CharacterCreatorAnnotatedTagAccessory2_s;
					field_name = "Accessory";
					LPSTR pAnnotatedLabel = "None";
					if (strnicmp(cSelectedAccessory2, "None", 4) != NULL) pAnnotatedLabel = charactercreatorplus_findannotation(cSelectedAccessory2);
					combo_buffer = cSelectedAccessory2;
					combo_annotated_buffer = pAnnotatedLabel;
					part_number = part_loop;
				}

				if (part_loop == 4)
				{
					CharacterCreatorCurrent_s = CharacterCreatorFacialHair_s;
					CharacterCreatorCurrentAnnotated_s = CharacterCreatorAnnotatedFacialHair_s;
					CharacterCreatorCurrentAnnotatedTag_s = CharacterCreatorAnnotatedTagFacialHair_s;
					if (strstr(CCP_Type, "Adult Female"))
						field_name = "Tattoo";
					else
					field_name = "Facial Hair";
					LPSTR pAnnotatedLabel = "None";
					if (strnicmp(cSelectedFacialHair, "None", 4) != NULL) pAnnotatedLabel = charactercreatorplus_findannotation(cSelectedFacialHair);
					combo_buffer = cSelectedFacialHair;
					combo_annotated_buffer = pAnnotatedLabel;
					part_number = part_loop;
				}
				if (part_loop == 5)
				{
					CharacterCreatorCurrent_s = CharacterCreatorBody_s;
					CharacterCreatorCurrentAnnotated_s = CharacterCreatorAnnotatedBody_s;
					CharacterCreatorCurrentAnnotatedTag_s = CharacterCreatorAnnotatedTagBody_s;
					field_name = "Body";
					LPSTR pAnnotatedLabel = charactercreatorplus_findannotation(cSelectedBody);
					combo_buffer = cSelectedBody;
					combo_annotated_buffer = pAnnotatedLabel;
					part_number = part_loop;
				}
				if (part_loop == 6)
				{
					CharacterCreatorCurrent_s = CharacterCreatorLegs_s;
					CharacterCreatorCurrentAnnotated_s = CharacterCreatorAnnotatedLegs_s;
					CharacterCreatorCurrentAnnotatedTag_s = CharacterCreatorAnnotatedTagLegs_s;
					field_name = "Legs";
					LPSTR pAnnotatedLabel = charactercreatorplus_findannotation(cSelectedLegs);
					bool bAllow = false;
					// before allowng selected legs through, check they comply with our cSelectedLegsFilter filter
					if (strlen(cSelectedLegsFilter) == 0)
					{
						if (item_current_type_selection <= 3 && strnicmp(cSelectedLegs + strlen(cSelectedLegs) - 2, "01", 2) != NULL) bAllow = true;
						if (item_current_type_selection > 3) bAllow = true;
					}
					if (strlen(cSelectedLegsFilter) > 0 && pAnnotatedLabel && strstr(pAnnotatedLabel, cSelectedLegsFilter) != NULL) bAllow = true;
					if (bAllow == true)
					{
						// no filter so allow, or filter matches, so also allow
					}
					else
					{
						// this current legs selection no longer matches filter, so change to one that does
						// starting with the top-most item and working down
						std::map<std::string, std::string>::iterator annotated = CharacterCreatorCurrentAnnotated_s.begin();
						for (std::map<std::string, std::string>::iterator it = CharacterCreatorCurrent_s.begin(); it != CharacterCreatorCurrent_s.end(); ++it)
						{
							std::string thisname = it->first;
							std::string thistag = annotated->second;
							bool bThisAllow = false;
							LPSTR pThisName = (char*)thisname.c_str();
							if (strlen(cSelectedLegsFilter) == 0)
							{
								if (item_current_type_selection <= 3 && strnicmp(pThisName + strlen(pThisName) - 2, "01", 2) != NULL) bThisAllow = true;
								if (item_current_type_selection > 3) bAllow = true;
							}
							if (strlen(cSelectedLegsFilter) > 0 && strstr(thistag.c_str(), cSelectedLegsFilter) != NULL) bThisAllow = true;
							if (bThisAllow == true)
							{
								// found first (or one matching the filter)
								strcpy(cSelectedLegs, thisname.c_str());
								strcpy(cSelectedFeetFilter, "");
								g_bLegsChangeCascade = true;
								g_bFeetChangeCascade = true;
								break;
							}
							annotated++;
						}
						pAnnotatedLabel = charactercreatorplus_findannotation(cSelectedLegs);
					}
					// continue with selected legs as normal now
					combo_buffer = cSelectedLegs;
					combo_annotated_buffer = pAnnotatedLabel;
					part_number = part_loop;
				}
				if (part_loop == 7)
				{
					CharacterCreatorCurrent_s = CharacterCreatorFeet_s;
					CharacterCreatorCurrentAnnotated_s = CharacterCreatorAnnotatedFeet_s;
					CharacterCreatorCurrentAnnotatedTag_s = CharacterCreatorAnnotatedTagFeet_s;
					field_name = "Feet";
					// before allowng selected feet through, check they comply with our filter
					bool bAllow = false;
					LPSTR pAnnotatedLabel = charactercreatorplus_findannotation(cSelectedFeet);
					LPSTR pAnnotatedLabelTag = charactercreatorplus_findannotationtag(cSelectedFeet);
					if (strlen(cSelectedFeetFilter) == 0 && (pAnnotatedLabelTag == NULL || strlen(pAnnotatedLabelTag) == 0)) bAllow = true;
					if (strlen(cSelectedFeetFilter) > 0 && pAnnotatedLabelTag && strstr(pAnnotatedLabelTag, cSelectedFeetFilter) != NULL) bAllow = true;
					if (bAllow == true)
					{
						// no filter so allow, or filter matches, so also allow
					}
					else
					{
						// this current feet selection no longer matches filter, so change to one that does
						// starting with the top-most item and working down
						std::map<std::string, std::string>::iterator annotatedtag = CharacterCreatorCurrentAnnotatedTag_s.begin();
						for (std::map<std::string, std::string>::iterator it = CharacterCreatorCurrent_s.begin(); it != CharacterCreatorCurrent_s.end(); ++it)
						{
							std::string thisname = it->first;
							std::string thistag = annotatedtag->second;
							bool bThisAllow = false;
							LPSTR pThisName = (char*)thisname.c_str();
							if (strlen(cSelectedFeetFilter) == 0 && strlen(thistag.c_str()) == 0) bThisAllow = true;
							if (strlen(cSelectedFeetFilter) > 0 && strstr(thistag.c_str(), cSelectedFeetFilter) != NULL) bThisAllow = true;
							if (bThisAllow == true)
							{
								// found first (or one matching the filter)
								strcpy(cSelectedFeet, thisname.c_str());
								g_bFeetChangeCascade = true;
								break;
							}
							annotatedtag++;
						}
						pAnnotatedLabel = charactercreatorplus_findannotation(cSelectedFeet);
					}
					combo_buffer = cSelectedFeet;
					combo_annotated_buffer = pAnnotatedLabel;
					part_number = part_loop;
				}
				if (!CharacterCreatorCurrent_s.empty() && CharacterCreatorCurrent_s.size() > 1)
				{

					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
					ImGui::TextCenter(field_name.Get());
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));

					float Color_gadget_size = ImGui::GetFontSize()*2.0;


					cstr unique_label = "##CCP";
					unique_label += field_name;

					int iUniqueId = 444444;

					ImGui::Indent(1);
					ImGui::PushItemWidth(-10);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 6.5f));

					static float fCWidth = 0.0;
					float partsheight;

					int rows = CharacterCreatorCurrent_s.size() / 5;
					if (rows * 4 != CharacterCreatorCurrent_s.size())
						rows += 1;
					if (rows > 3) rows = 3;

					partsheight = 60.0f * rows;
					if(rows == 1)
						partsheight = ImGui::GetContentRegionAvail().y / 9.5f * rows;
					else
						partsheight = ImGui::GetContentRegionAvail().y / 10.5f * rows;

					ImGui::BeginChild("##CCP-Parts-Child", ImVec2(ImGui::GetContentRegionAvail().x, partsheight), false, iGenralWindowsFlags | ImGuiWindowFlags_AlwaysVerticalScrollbar);
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

					ImGui::Columns(4, "ccp-parts-columns4", false);  //false no border

					static int iCountUniqueImageIds = CCP_THUMBS;
					int iCountIcons = 0;

					std::map<std::string, std::string>::iterator annotated = CharacterCreatorCurrentAnnotated_s.begin();
					std::map<std::string, std::string>::iterator annotatedtag = CharacterCreatorCurrentAnnotatedTag_s.begin();
					for (std::map<std::string, std::string>::iterator it = CharacterCreatorCurrent_s.begin(); it != CharacterCreatorCurrent_s.end(); ++it)
					{
						std::string full_path = it->second;
						std::string name = it->first;

						// only allow if part has no filter or filter within name
						bool bThisAllow = false;
						if ( part_number == 6 || part_number == 7 )
						{
							LPSTR pThisName = (char*)name.c_str();
							if (part_number == 6)
							{
								// only allow specific legs
								LPSTR pThisAnnotatedName = (char*)annotated->second.c_str();
								if (strlen(cSelectedLegsFilter) == 0)
								{
									if (item_current_type_selection <= 3 && strnicmp(pThisName + strlen(pThisName) - 2, "01", 2) != NULL) bThisAllow = true;
									if (item_current_type_selection > 3) bThisAllow = true;
								}
								if (strlen(cSelectedLegsFilter) > 0 && strstr(pThisAnnotatedName, cSelectedLegsFilter) != NULL) bThisAllow = true;
							}
							if (part_number == 7)
							{
								// only allow specific feet
								LPSTR pThisAnnotatedTagName = (char*)annotatedtag->second.c_str();
								if (strlen(cSelectedFeetFilter) == 0 && (strlen(pThisAnnotatedTagName) == 0)) bThisAllow = true;
								if (strlen(cSelectedFeetFilter) > 0 && strstr(pThisAnnotatedTagName, cSelectedFeetFilter) != NULL) bThisAllow = true;
							}
						}
						else
						{
							// all other parts have a free pass!
							bThisAllow = true;
						}
						if (bThisAllow == true)
						{
							int iTextureID = CCP_EMPTY;
							if (iCountIcons < MAXPARTICONS)
							{
								if (g_iPartsIconsIDs[item_current_type_selection][part_number][iCountIcons] > 0)
								{
									iTextureID = g_iPartsIconsIDs[item_current_type_selection][part_number][iCountIcons];
								}
								else if (g_iPartsIconsIDs[item_current_type_selection][part_number][iCountIcons] == -1)
								{
									bool bValid = false;
									if (item_current_type_selection == 0 && pestrcasestr(full_path.c_str(), "adult male")) bValid = true;
									if (item_current_type_selection == 1 && pestrcasestr(full_path.c_str(), "adult female")) bValid = true;
									if (item_current_type_selection == 2 && pestrcasestr(full_path.c_str(), "zombie male")) bValid = true;
									if (item_current_type_selection == 3 && pestrcasestr(full_path.c_str(), "zombie female")) bValid = true;
									if (item_current_type_selection > 3 && pestrcasestr(full_path.c_str(), g_CharacterType[item_current_type_selection].pPartsFolder)) bValid = true;
									if (bValid)
									{
										//Try loading icon.
										std::string icon_path = full_path + name + ".png";
										image_setlegacyimageloading(true);
										LoadImage((char *)icon_path.c_str(), iCountUniqueImageIds);
										image_setlegacyimageloading(false);
										if (ImageExist(iCountUniqueImageIds))
										{
											g_iPartsIconsIDs[item_current_type_selection][part_number][iCountIcons] = iCountUniqueImageIds++;
										}
										else
										{
											g_iPartsIconsIDs[item_current_type_selection][part_number][iCountIcons] = 0;
										}
									}
								}
							}
							if (strnicmp(name.c_str(), "None", 4) == NULL)
								iTextureID = CCP_NONE;

							// mark the one selected
							bool is_selected = false;
							if (strcmp(name.c_str(), combo_buffer) == 0)
								is_selected = true;

							// the label we see
							std::string annotated_label = annotated->second;
							std::string annotatedtag_label = annotatedtag->second;

							bool bIconRestricted = false;

							// hide the icon for any restricted part (part that doesn't work with another part).
							for (int i = 0; i < g_restrictedParts.size(); i++)
							{
								if (strstr(annotated->second.c_str(), g_restrictedParts[i]))
								{
									bIconRestricted = true;
									break;
								}
							}
							// also hide any parts that are missing their thumbnail 
							if (iTextureID == CCP_EMPTY)
							{
								bIconRestricted = true;
							}

							if (!bIconRestricted)
							{
								ImGui::PushID(iUniqueId++);

								fCWidth = ImGui::GetContentRegionAvailWidth();
								ImGuiWindow* window = ImGui::GetCurrentWindow();
								//Add background rect.
								ImVec4 background_col = ImGui::GetStyle().Colors[ImGuiCol_Button]; //ImVec4(0.5, 0.5, 0.5, 0.6);
								background_col.w = 0.60;
								const ImRect image_bb((window->DC.CursorPos), window->DC.CursorPos + ImVec2(fCWidth, fCWidth));
								window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, ImGui::GetColorU32(background_col), 8.0f, 15);

								if (is_selected)
								{
									ImVec4 tool_selected_col = ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram];
									ImVec2 padding = { 3.0, 3.0 };
									const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(fCWidth, fCWidth));
									window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
								}

								IconColor = ImVec4(1.0, 1.0, 1.0, 1.0);
								ImVec4 HoverColor = ImVec4(0.8f, 0.8f, 0.8f, 0.8f);

								if (iTextureID == CCP_NONE || iTextureID == CCP_EMPTY)
									IconColor.w = 0.5;

								if (iTextureID > 0 && ImGui::ImgBtn(iTextureID, ImVec2(fCWidth, fCWidth), ImVec4(0.0, 0.0, 0.0, 0.0), IconColor, HoverColor, HoverColor, 0, 0, 0, 0, false, false, false, false, true, false)) //no bBoostIconColors
								{
									// we need to wait for any previously requested preloads to exist before we can go on to make the character
									object_preload_files_wait(); // dont need to wait for image preload, image preload is thread safe and can overlap normal DX operations
									// Change Character. full_path.c_str()
									strcpy(combo_buffer, name.c_str());
									// instead of instant change, record change we want and fire off some preloads for smooth UI
									charactercreatorplus_preparechange((char *)full_path.c_str(), part_number, (char *)annotatedtag_label.c_str());
									if (part_number == 2)
									{
										// and set the IC for reference when its time to save the character assembly info
										strcpy(cSelectedICCode, (char *)annotatedtag_label.c_str());
									}
									if (part_number == 5)
									{
										// if body requires NO LEGS (or something else), set this condition for other dropdowns
										// and force any current legs to conform
										strcpy(cSelectedLegsFilter, (char *)annotatedtag_label.c_str());
									}
									if (part_number == 6)
									{
										// if legs requires NO FEET (or something else), set this condition for other dropdowns
										// and force any current feet to conform
										strcpy(cSelectedFeetFilter, (char *)annotatedtag_label.c_str());
									}
								}
								if (ImGui::IsItemHovered())
								{
									// Display an enlarged image of the icon, if the user is hovering over it.
									ImGui::BeginTooltip();
									ImVec4 background_col = ImGui::GetStyle().Colors[ImGuiCol_Button]; //ImVec4(0.5, 0.5, 0.5, 0.6);
									background_col.w = 0.6f;
									//ImGui::ImgBtn(iTextureID, ImVec2(fCWidth * 2.5f, fCWidth * 2.5f), background_col, IconColor, HoverColor, HoverColor, 0, 0, 0, 0, false, false, false, false, true, false);
									ImGui::ImgBtn(iTextureID, ImVec2(fCWidth * 3.0f, fCWidth * 3.0f), background_col, IconColor, HoverColor, HoverColor, 0, 0, 0, 0, false, false, false, false, true, false);
									ImGui::TextCenter(annotated_label.c_str());
									ImGui::EndTooltip();
								}

								ImGui::PopID();

								ImGui::NextColumn();
							}
						}

						// advance annotated list with real item list
						annotated++;
						annotatedtag++;
						iCountIcons++;
					}

					ImGui::Columns(1);
					if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
						// ZJ: Following feedback, the two line gap was removed.
						ImGui::Spacing();

					}
					ImGui::EndChild();

					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
					ImGui::PopItemWidth();
					ImGui::Indent(-1);

					// also update character during a cascade (body changes legs, which changes feet)
					if ((g_bLegsChangeCascade == true || g_bFeetChangeCascade == true) && g_charactercreatorplus_preloading == false)
					{
						// and ensure all thread activity ends before we push this (or it may delay long enough to change base type again!)
						image_preload_files_wait();
						object_preload_files_wait();

						// a faster single pass option
						std::map<std::string, std::string>::iterator it = CharacterCreatorCurrent_s.begin();
						std::string full_path = it->second;
						charactercreatorplus_preparechange((char*)full_path.c_str(), 67, "");
						g_bLegsChangeCascade = false;
						g_bFeetChangeCascade = false;
					}
					CharacterCreatorCurrent_s.clear();
				}


				//	Rotate the character.
				ImGui::TextCenter("Rotate");
				ImGui::Indent(10.0f);
				if (ImGui::MaxSliderInputFloat("##CharacterRotation", &fCCPRotateY, 0.0f, 360.0f, "Rotate Character", 0.0f, 360.0f))
				{
					RotateObject(iCharObj, ObjectAngleX(iCharObj), fCCPRotateY, ObjectAngleZ(iCharObj));
					ccpObjTargetAY = fCCPRotateY;
				}
				ImGui::Indent(-10.0f);

				//	Zoom in/out to character.
				ImGui::TextCenter("Zoom");
				ImGui::Indent(10.0f);
				if (ImGui::MaxSliderInputFloat("##CCPZoom", &g_fCCPZoom, 0.0f, 100.0f, "Adjust Zoom"))
				{
					charactercreatorplus_dozoom();

					bZoom = true;
				}
				ImGui::Indent(-10.0f);

			}

			extern bool bImGuiRenderTargetFocus;
			if (bImGuiRenderTargetFocus && ImGui::IsMouseDown(0) && ImGui::IsMouseDragging(0) && !bZoom)
			{
				//PE: Some object rotate inverted Pivot ?
				fCCPRotateY -= ImGui::GetIO().MouseDelta.x * g.timeelapsed_f;
				if (fCCPRotateY < 0) fCCPRotateY += 360.0;
				if (fCCPRotateY > 360.0) fCCPRotateY -= 360.0;
				RotateObject(iCharObj, ObjectAngleX(iCharObj), fCCPRotateY, ObjectAngleZ(iCharObj));
				ccpObjTargetAY = fCCPRotateY;
			}

			if (!pref.iEnableAdvancedCharacterCreator)
			{
				extern void animsystem_animationtoolsimpleui(int objectnumber);
				animsystem_animationtoolsimpleui(iCharObj);
			}
			else
			{
				extern void animsystem_animationtoolui(int objectnumber);
				animsystem_animationtoolui(iCharObj);
			}

			// All users can specify weapon for character
			if (pref.bAutoClosePropertySections && iLastOpenHeader != 62)
				ImGui::SetNextItemOpen(false, ImGuiCond_Always);

			if (ImGui::StyleCollapsingHeader("Character Details", ImGuiTreeNodeFlags_DefaultOpen))
			{
				iLastOpenHeader = 62;
				ImGui::Indent(10);

				// Weapon Choice
				extern void animsystem_weaponproperty (int, bool, entityeleproftype*, bool, bool);
				int characterbasetype = -1;
				if (stricmp(CCP_Type, "adult male") == NULL) characterbasetype = 0;
				if (stricmp(CCP_Type, "adult female") == NULL) characterbasetype = 1;
				if (stricmp(CCP_Type, "zombie male") == NULL) characterbasetype = 2;
				if (stricmp(CCP_Type, "zombie female") == NULL) characterbasetype = 3;
				if (characterbasetype == -1)
				{
					int iBaseValue = GetBaseValueFromCCPType(CCP_Type);
					if (iBaseValue > 1) characterbasetype = iBaseValue-1;
				}

				// allow all character types to choose weapons and start behaviours, keep things open!
				if (true)//characterbasetype >= 0 && characterbasetype <= 1)
				{
					/* as this does not CHANGE the behaviour property, it was never used
					// Choose weapon.
					bool bFromCharacterCreator = true;
					animsystem_weaponproperty(characterbasetype, bFromCharacterCreator, &g_grideleprof_holdchoices, true, true);
					*/
					// Choose behavior.
					ImGui::TextCenter("Behavior");
					static std::vector<std::string> characterBehaviors;
					static std::vector<std::string> characterBehaviorsDisplay;
					static bool bCollectedScriptNames = false;
					if (!bCollectedScriptNames)
					{
						cstr oldDir = GetDir();
						char newDir[260];
						strcpy(newDir, "scriptbank\\people\\");
						SetDir(newDir);
						ChecklistForFiles();
						for (int c = 1; c <= ChecklistQuantity(); c++)
						{
							cStr tfile_s = Lower(ChecklistString(c));
							if (tfile_s != "." && tfile_s != "..")
							{
								if (strcmp(Right(tfile_s.Get(), 4), ".lua") == 0)
								{
									std::string file(tfile_s.Get());
									characterBehaviors.push_back(file);
									std::string name = file;
									replaceAll(name, "_", " ");
									*(name.data() + name.length() - 4) = 0;
									name[0] = toupper(name[0]);
									for (int i = 1; i < name.length() - 1; i++)
									{
										if (name[i] == ' ')
										{
											name[i + 1] = toupper(name[i + 1]);
										}
									}
									characterBehaviorsDisplay.push_back(name);
								}
							}
						}
						bCollectedScriptNames = true;
						SetDir(oldDir.Get());
					}

					ImGui::PushItemWidth(-10);
					std::string displayName = CCP_Script;
					replaceAll(displayName, "_", " ");
					*(displayName.data() + displayName.length() - 4) = 0;
					displayName[0] = toupper(displayName[0]);
					for (int i = 1; i < displayName.length() - 1; i++)
					{
						if (displayName[i] == ' ')
						{
							displayName[i + 1] = toupper(displayName[i + 1]);
						}
					}
					if (ImGui::BeginCombo("##characterbehaviorcombo", displayName.c_str()))
					{
						for (int i = 0; i < characterBehaviorsDisplay.size(); i++)
						{
							bool bIsSelected = strcmp(CCP_Script, characterBehaviors[i].c_str()) == 0;
							if (ImGui::Selectable(characterBehaviorsDisplay.at(i).c_str(), &bIsSelected))
							{
								strcpy(CCP_Script, characterBehaviors[i].c_str());
							}
						}
						ImGui::EndCombo();
					}
					ImGui::PopItemWidth();
				}

				// CharacterDetails VOICE only in Advanced
				if (pref.iEnableAdvancedCharacterCreator)
				{
					// Voice field removed from CCP.
				}

				//unindent before center.
				ImGui::Indent(-10); 
			}

			if (pref.bAutoClosePropertySections && iLastOpenHeader != 63)
				ImGui::SetNextItemOpen(false, ImGuiCond_Always);

			if (ImGui::StyleCollapsingHeader("Save Character", ImGuiTreeNodeFlags_DefaultOpen)) 
			{
				iLastOpenHeader = 63;
				ImGui::Indent(10);

				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
				ImGui::Text("Path");
				ImGui::SameLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));

				ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY()));

				float path_gadget_size = ImGui::GetFontSize()*2.0;

				ImGui::PushItemWidth(-10 - path_gadget_size);

				ImGui::InputText("##InputPathCCP", &CCP_Path[0], 250);
				if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;
				if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Set Where to Save Your Character");

				ImGui::PopItemWidth();
				//	Let the user know they set an invalid save file path.
				if (ImGui::BeginPopup("##CCPInvalidSavePath"))
				{
					ImGui::Text("Path must be within 'Max\\Files\\entitybank\\user\\'");
					ImGui::EndPopup();
				}

				ImGui::SameLine();
				ImGui::PushItemWidth(path_gadget_size);
				if (ImGui::StyleButton("...##ccppath")) {
					//PE: filedialogs change dir so.
					cStr tOldDir = GetDir();
					char * cFileSelected;
					cstr fulldir = tOldDir + "\\entitybank\\user\\"; //"\\entitybank\\user\\charactercreatorplus\\";
					cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_DIR, "All\0*.*\0", fulldir.Get(), "", true, NULL);

					SetDir(tOldDir.Get());

					if (cFileSelected && strlen(cFileSelected) > 0) {

						//	Check that the new path still contains the entitybank folder.
						char* cCropped = strstr(cFileSelected, "\\entitybank\\user");
						if (cCropped)
						{
							//	New location contains entitybank folder, so change the import path.
							strcpy(CCP_Path, cFileSelected);
						}
						else
						{
							ImGui::OpenPopup("##CCPInvalidSavePath");
						}
					}
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select Where to Save Your Character");

				ImGui::PopItemWidth();

				ImGui::Indent(-10); //unindent before center.
				float save_gadget_size = ImGui::GetFontSize()*10.0;
				float w = ImGui::GetWindowContentRegionWidth();
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (save_gadget_size*0.5), 0.0f));

				if (ImGui::StyleButton("Save Character##butsave", ImVec2(save_gadget_size, 0)))
				{
					// if free trial, no import
					extern bool g_bFreeTrialVersion;
					if (g_bFreeTrialVersion == true)
					{
						extern bool bFreeTrial_Window;
						bFreeTrial_Window = true;
					}
					else
					{
						if (strlen(CCP_Name) > 0)
						{
							if (strlen(CCP_Path) > 0)
							{
								// choose behavior here
								char script[260];
								strcpy(script, "people\\");
								strcat(script, CCP_Script);

								// save character FPE
								g_CharacterCreatorPlus.obj.settings.script_s = script;
								g_CharacterCreatorPlus.obj.settings.voice_s = pCCPVoiceSet;
								g_CharacterCreatorPlus.obj.settings.iSpeakRate = CCP_Speak_Rate;
								int iCharObj = g.characterkitobjectoffset + 1;

								// Character names saved starting with a space don't save correctly. Wipe out spaces before characters.
								int iSpaceBeforeCharCount = 0;
								for (int i = 0; i < strlen(CCP_Name); i++)
								{
									if (CCP_Name[i] == ' ')
										iSpaceBeforeCharCount++;
									else
										break;
								}
								if (iSpaceBeforeCharCount > 0)
								{
									char temp[MAX_PATH];
									strcpy(temp, CCP_Name + iSpaceBeforeCharCount);
									CCP_Name[0] = 0;
									strcpy(CCP_Name, temp);
								}

								cstr pFillFilename = cstr(CCP_Path) + CCP_Name + ".dbo";
								if (charactercreatorplus_savecharacterentity(iCharObj, pFillFilename.Get(), g.importermenuimageoffset + 50) == true)
								{
									strcpy(cTriggerMessage, "Character Saved");
									bTriggerMessage = true;

									extern cstr sGotoPreviewWithFile;
									extern int iGotoPreviewType;
									sGotoPreviewWithFile = cstr(CCP_Path) + CCP_Name + ".fpe";
									char sTmp[MAX_PATH];
									strcpy(sTmp, sGotoPreviewWithFile.Get());
									char *find = (char *)pestrcasestr(sTmp, "entitybank\\");
									if (find && find != &sTmp[0]) strcpy(sTmp, find);
									sGotoPreviewWithFile = sTmp;
									//Only trigger if destination contain entitybank.
									if (find)
									{
										//Exit ccp. and open preview.
										g_bCharacterCreatorPlusActivated = false;
										iGotoPreviewType = 1;
										CCP_Name[0] = 0;
									}
									else
										sGotoPreviewWithFile = "";
								}
							}
							else
							{
								strcpy(cTriggerMessage, "Please select a path where you like the character saved.");
								bTriggerMessage = true;
							}
						}
						else
						{
							strcpy(cTriggerMessage, "You must give your character a name before you can save it.");
							bTriggerMessage = true;
						}
					}
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Save Your Character");
			}

			if (!pref.bHideTutorials)
			{
				#ifndef REMOVED_EARLYACCESS
				if (ImGui::StyleCollapsingHeader("Tutorial (this feature is incomplete)", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Indent(10);
					void SmallTutorialVideo(char *tutorial, char* combo_items[] = NULL, int combo_entries = 0, int iVideoSection = 0, bool bAutoStart = false);
					cstr cShowTutorial = "03 - Add character and set a path";
					char* tutorial_combo_items[] = { "01 - Getting started", "02 - Creating terrain", "03 - Add character and set a path" };
					SmallTutorialVideo(cShowTutorial.Get(), tutorial_combo_items, ARRAYSIZE(tutorial_combo_items), SECTION_CHARACTER_CREATOR);
					float but_gadget_size = ImGui::GetFontSize()*12.0;
					float w = ImGui::GetWindowContentRegionWidth() - 10.0;
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
					#ifdef INCLUDESTEPBYSTEP
					if (ImGui::StyleButton("View Step by Step Tutorial", ImVec2(but_gadget_size, 0)))
					{
						// pre-select tutorial 03
						extern bool bHelpVideo_Window;
						extern bool bHelp_Window;
						extern char cForceTutorialName[1024];
						bHelp_Window = true;
						bHelpVideo_Window = true;
						extern bool bSetTutorialSectionLeft;
						bSetTutorialSectionLeft = false;
						strcpy(cForceTutorialName, cShowTutorial.Get());
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Start Step by Step Tutorial");
					#endif

					ImGui::Indent(-10);
				}
				#endif
			}

			ImGui::PopItemWidth();

			void CheckMinimumDockSpaceSize(float minsize);
			CheckMinimumDockSpaceSize(250.0f);

			if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) 
			{
				//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
				ImGui::Text("");
				ImGui::Text("");
			}

			ImGui::End();
		}
	}
	else
	{
		if (g_CharacterCreatorPlus.bInitialised)
		{
			//Make sure we hide ccp
			if (bCharObjVisible && ObjectExist(iCharObj))
			{
				extern bool g_bShowBones;
				g_bShowBones = false;
				wiRenderer::SetToDrawDebugBoneLines(g_bShowBones);
				//ccp is only hidden so you still se bones, if going to the importer and enable bones, so move it out of the way.
				PositionObject(iCharObj, 500000, 500000, 500000);

				// first, erase preloaded files we dont need any more (and load in basics for when return to CCP)

				image_preload_files_wait();
				image_preload_files_reset();
				object_preload_files_wait(); // If it is still working on loading, it will crash when data is reset.
				object_preload_files_reset();

				// hide character creator model
				HideObject(iCharObj);

				if (ObjectExist(iDressRoom)) HideObject(iDressRoom);

				bCharObjVisible = false;

				t.inputsys.dowaypointview = 0;

				//Restore.
				waypoint_restore();

				t.gridentityhidemarkers = 0;
				editor_updatemarkervisibility();
				editor_refresheditmarkers();

				//  put all entities back where they were
				for (t.tcce = 1; t.tcce <= g.entityelementlist; t.tcce++)
				{
					t.tccentid = t.entityelement[t.tcce].bankindex;
					if (t.tccentid > 0)
					{
						t.tccsourceobj = t.entityelement[t.tcce].obj;
						if (ObjectExist(t.tccsourceobj) == 1)
						{
							PositionObject(t.tccsourceobj, t.entityelement[t.tcce].x, t.entityelement[t.tcce].y, t.entityelement[t.tcce].z);
						}
					}
				}

				//Restore editor camera.
				if (editoroldx_f != 0) //PE: Somehow editoroldx_f was 0 ?
				{
					t.cameraviewmode = oldtcameraviewmode;
					t.editorfreeflight.mode = editoroldmode_f;
					t.editorfreeflight.c.x_f = editoroldx_f;
					t.editorfreeflight.c.y_f = editoroldy_f;
					t.editorfreeflight.c.z_f = editoroldz_f;
					t.editorfreeflight.c.angx_f = editoroldangx_f;
					t.editorfreeflight.c.angy_f = editoroldangy_f;
					PositionCamera(t.editorfreeflight.c.x_f, t.editorfreeflight.c.y_f, t.editorfreeflight.c.z_f);
					RotateCamera(t.editorfreeflight.c.angx_f, t.editorfreeflight.c.angy_f, 0);
				}

				restore_visuals(t.visuals, t.visualsStorage);
				visuals_loop();
			}
		}
	}
}

void charactercreatorplus_getautoswapdata(char* filename)
{
	// prepare destination file
	cstr pPath = GetDir();
	pPath += cstr("\\");
	cstr pFile = filename;
	pFile = pPath + pFile;
	LPSTR pFilename = pFile.Get();
	AutoSwapData* pAutoSwapData = nullptr;
	
	if (FileExist(filename) == 1)
	{
		OpenToRead(1, pFilename);
		while (FileEnd(1) == 0)
		{
			// get line by line
			cstr line_s = ReadString(1);
			LPSTR pLine = line_s.Get();

			// get field name
			char pLeft[128];
			char pRight[128];
			bool bIsNewAutoSwap = false;
			char* pEquals = strstr(line_s.Get(), "=");

			if (pEquals)
				bIsNewAutoSwap = true;
			else if (!pEquals)
				pEquals = strstr(line_s.Get(), ":");

			if (!pEquals)
			{
				CloseFile(1);
				return;
			}

			// Separate left and right into category and part.
			strcpy(pRight, pEquals + 2);
			strcpy(pLeft, line_s.Get());
			pLeft[strlen(pLeft) - strlen(pEquals) - 1] = 0;
			
			if (bIsNewAutoSwap)
			{
				// Hit a new auto swap section.
				pAutoSwapData = new AutoSwapData;
				int category = charactercreatorplus_getcategoryindex(pLeft);
				if (category == -1)
				{
					// Received an invalid category.
					delete pAutoSwapData;
					pAutoSwapData = nullptr;
					CloseFile(1);
					return;
				}
				pAutoSwapData->iPartType = category;
				pAutoSwapData->sPartName = pRight;
				charactercreatorplus_storeautoswapdata(pAutoSwapData);
			}
			else
			{
				// Get the items that need to be swapped out in order to fit the equipped clothing.
				int iPartType = charactercreatorplus_getcategoryindex(pLeft);
				std::string name(pRight);
				pAutoSwapData->requiredSwapCategories.push_back(iPartType);
				pAutoSwapData->requiredSwapNames.push_back(name);
			}
		}

		// close file handling
		CloseFile(1);
	}
}

int charactercreatorplus_getcategoryindex(char* category)
{
	if (strcmp(category, "Head Gear") == NULL)
		return 0;
	else if (strcmp(category, "Hair") == NULL)
		return 1;
	else if (strcmp(category, "Head") == NULL)
		return 2;
	else if (strcmp(category, "Wearing") == NULL)
		return 3;
	else if (strcmp(category, "Facial Hair") == NULL)
		return 4;
	else if (strcmp(category, "Body") == NULL)
		return 5;
	else if (strcmp(category, "Legs") == NULL)
		return 6;
	else if (strcmp(category, "Feet") == NULL)
		return 7;
	else if (strcmp(category, "Accessory1") == NULL)
		return 8;
	else if (strcmp(category, "Accessory2") == NULL)
		return 9;
	else return -1;
}

// Add the auto-swap data to the relevant container.
void charactercreatorplus_storeautoswapdata(AutoSwapData* pData)
{
	// 0: Head Gear
	// 1: Hair
	// 2: Head
	// 3: Eye Glasses
	// 4: Facial Hair
	// 5: Body
	// 6: Legs
	// 7: Feet

	if (pData)
	{
		switch (pData->iPartType)
		{
		case 0: g_headGearMandatorySwaps.push_back(pData); break;
		case 1: break; // None needed (yet).
		case 2: break; // None needed (yet).
		case 3: break; // None needed (yet).
		case 4: break; // None needed (yet).
		case 5: break; // None needed (yet).
		case 6: break; // None needed (yet).
		case 7: break; // None needed (yet).
		case 8: break; // None needed (yet).
		case 9: break; // None needed (yet).
		default:break; 
		}
	}
}

void charactercreatorplus_performautoswap(int part)
{
	std::string sPartToSwap = "";
	std::map<std::string, std::string>* pAnnotationData = nullptr;
	std::vector<AutoSwapData*>* pAutoSwapData = nullptr;
	char pHairColour[128];

	// Get the correct part type to swap.
	switch (part)
	{
	case 0: sPartToSwap = cSelectedHeadGear;
		pAnnotationData = &CharacterCreatorAnnotatedHeadGear_s;
		pAutoSwapData = &g_headGearMandatorySwaps;
		break;
	case 1: sPartToSwap = cSelectedHair;
		pAnnotationData = &CharacterCreatorAnnotatedHair_s;
		break;
	case 2: sPartToSwap = cSelectedHead;
		pAnnotationData = &CharacterCreatorAnnotatedHead_s;
		break;
	case 3: sPartToSwap = cSelectedEyeglasses;
		pAnnotationData = &CharacterCreatorAnnotatedEyeglasses_s;
		break;
	case 4: sPartToSwap = cSelectedFacialHair;
		pAnnotationData = &CharacterCreatorAnnotatedFacialHair_s;
		break;
	case 5: sPartToSwap = cSelectedBody;
		pAnnotationData = &CharacterCreatorAnnotatedBody_s;
		break;
	case 6: sPartToSwap = cSelectedLegs;
		pAnnotationData = &CharacterCreatorAnnotatedFeet_s;
		break;
	case 7: sPartToSwap = cSelectedFeet;
		pAnnotationData = &CharacterCreatorAnnotatedLegs_s;
		break;
	case 8: sPartToSwap = cSelectedAccessory1;
		pAnnotationData = &CharacterCreatorAnnotatedAccessory1_s;
		break;
	case 9: sPartToSwap = cSelectedAccessory2;
		pAnnotationData = &CharacterCreatorAnnotatedAccessory2_s;
		break;
	}

	if (!pAutoSwapData || !pAnnotationData || sPartToSwap.length() == 0 || pAutoSwapData->size() == 0)
	{
		return;
	}

	std::string sNewPartUIName = "";

	// Get the name of the part that the user selected (as seen on-screen).
	for (const auto & part : *pAnnotationData)
	{
		if (part.first == sPartToSwap)
		{
			sNewPartUIName = part.second;
			break;
		}
	}

	// Couldn't find a matching name for the part.
	if (sNewPartUIName.length() == 0)
		return;

	// Get the swap data for this part (if any).
	AutoSwapData* pSwapToPerform = nullptr;
	for (const auto & pSwapData : *pAutoSwapData)
	{
		if (!pSwapData)
			continue;

		const char* pNameSegment = strstr(sNewPartUIName.c_str(), pSwapData->sPartName.c_str());
		if (pNameSegment)
		{
			// There are swaps that must take place with the chosen part.
			pSwapToPerform = pSwapData;
			break;
		}
	}

	// No swap needed for the chosen part.
	if (!pSwapToPerform)
		return;

	// Undo the last swap, so that the users original choice is maintained for this new swap.
	charactercreatorplus_restoreswappedparts();

	// Find the required mesh names to swap.
	for(int i = 0 ; i < pSwapToPerform->requiredSwapCategories.size(); i++)
	{
		char* pPartToSwap = nullptr;
		g_iPartsThatNeedReloaded[pSwapToPerform->requiredSwapCategories[i]] = 1;

		// Get the correct part type to swap.
		switch (pSwapToPerform->requiredSwapCategories[i])
		{
		case 0: pPartToSwap = cSelectedHeadGear;
			pAnnotationData = &CharacterCreatorAnnotatedHeadGear_s;
			break;
		case 1: pPartToSwap = cSelectedHair;
			pAnnotationData = &CharacterCreatorAnnotatedHair_s;
			break;
		case 2: pPartToSwap = cSelectedHead;
			pAnnotationData = &CharacterCreatorAnnotatedHead_s;
			break;
		case 3: pPartToSwap = cSelectedEyeglasses;
			pAnnotationData = &CharacterCreatorAnnotatedEyeglasses_s;
			break;
		case 4: pPartToSwap = cSelectedFacialHair;
			pAnnotationData = &CharacterCreatorAnnotatedFacialHair_s;
			break;
		case 5: pPartToSwap = cSelectedBody;
			pAnnotationData = &CharacterCreatorAnnotatedBody_s;
			break;
		case 6: pPartToSwap = cSelectedLegs;
			pAnnotationData = &CharacterCreatorAnnotatedFeet_s;
			break;
		case 7: pPartToSwap = cSelectedFeet;
			pAnnotationData = &CharacterCreatorAnnotatedLegs_s;
			break;
		case 8: pPartToSwap = cSelectedAccessory1;
			pAnnotationData = &CharacterCreatorAnnotatedAccessory1_s;
			break;
		case 9: pPartToSwap = cSelectedAccessory2;
			pAnnotationData = &CharacterCreatorAnnotatedAccessory2_s;
			break;
		}

		if (pPartToSwap == nullptr)
			continue;

		pSwapToPerform->swappedPartNames.push_back(pPartToSwap);

		// Exclude the following heads from being swapped (textures do not share a common structure with the others - so cannot be swapped).
		if (pSwapToPerform->requiredSwapCategories[i] == 2)
		{
			if (strstr(pPartToSwap, "adult male head 09")) continue;
			else if (strstr(pPartToSwap, "adult male head 10")) continue;
			else if (strstr(pPartToSwap, "adult male head 11")) continue;
			else if (strstr(pPartToSwap, "adult male head 12")) continue;
			else if (strstr(pPartToSwap, "adult male head 14")) continue;
		}

		// Get the name of the hair as seen in the UI, to then extract the hair colour.
		if (pSwapToPerform->requiredSwapCategories[i] == 1)
		{
			for (const auto & part : *pAnnotationData)
			{
				if (stricmp(part.first.c_str(), pPartToSwap) == NULL)
				{
					charactercreatorplus_extracthaircolour(part.second.c_str(), &pHairColour[0]);
					break;
				}
			}
		}
	
		char pPartBackup[MAX_PATH];
		strcpy(pPartBackup, pPartToSwap);

		strcpy(pPartToSwap, pSwapToPerform->requiredSwapNames[i].c_str());

		// Store the swap data so the swaps can be undone if the user changes the characters parts.
		g_previousAutoSwap = pSwapToPerform;

		// Store the last headgear autoswap, so that the original ui choice can be maintained.
		if (pSwapToPerform->requiredSwapCategories[i] == 2)
			g_pLastHeadgearAutoSwap = pSwapToPerform;
		
		// There is no mesh for "None", so do not alter pPartToSwap.
		if (strstr(pPartToSwap, "None"))
		{
			continue;
		}

		// Add the previous hair colour so we know which hair to swap to.
		if (pSwapToPerform->requiredSwapCategories[i] == 1)
		{
			if (strlen(pHairColour) > 0)
				strcpy(pPartToSwap + strlen(pPartToSwap), pHairColour);
		}

		int iSearchCounter = 1;
		for (const auto & part : *pAnnotationData)
		{
			// Now that we have the name of the replacement part, we need its mesh name so it can be loaded.
			if (strstr(part.second.c_str(), pPartToSwap))
			{
				strcpy(pPartToSwap, part.first.c_str());
				break;
			}

			iSearchCounter++;
		}

		// Couldn't find a replacement part, so undo changes.
		if (iSearchCounter > pAnnotationData->size())
			strcpy(pPartToSwap, pPartBackup);
	}
}

// Reverse any swaps that were applied on the users previous choice.
void charactercreatorplus_restoreswappedparts()
{
	if (g_previousAutoSwap == nullptr)
		return;

	for (int i = 0; i < g_previousAutoSwap->requiredSwapCategories.size(); i++)
	{
		char* pPartToSwap = nullptr;

		switch (g_previousAutoSwap->requiredSwapCategories[i])
		{
		case 0: pPartToSwap = cSelectedHeadGear;
			break;
		case 1: pPartToSwap = cSelectedHair;
			break;
		case 2: pPartToSwap = cSelectedHead;
			break;
		case 3: pPartToSwap = cSelectedEyeglasses;
			break;
		case 4: pPartToSwap = cSelectedFacialHair;
			break;
		case 5: pPartToSwap = cSelectedBody;
			break;
		case 6: pPartToSwap = cSelectedLegs;
			break;
		case 7: pPartToSwap = cSelectedFeet;
			break;
		case 8: pPartToSwap = cSelectedAccessory1;
			break;
		case 9: pPartToSwap = cSelectedAccessory2;
			break;
		}
		strcpy(pPartToSwap, g_previousAutoSwap->swappedPartNames[i].c_str());
		g_iPartsThatNeedReloaded[g_previousAutoSwap->requiredSwapCategories[i]] = 1;
	}

	g_previousAutoSwap = nullptr;
}

void charactercreatorplus_extracthaircolour(const char* source, char* destination)
{
	if (strstr(source, "Black"))
		strcpy(destination, " Black");
	else if (strstr(source, "Blonde"))
		strcpy(destination, " Blonde");
	else if (strstr(source, "Blond")) // Must be after Blonde.
		strcpy(destination, " Blond");
	else if (strstr(source, "Brown"))
		strcpy(destination, " Brown");
	else if (strstr(source, "Grey"))
		strcpy(destination, " Grey");
	else if (strstr(source, "Red"))
		strcpy(destination, " Red");
	else if (strstr(source, "Purple"))
		strcpy(destination, " Purple");
	else if (strstr(source, "Brunette"))
		strcpy(destination, " Brunette");
	else if (strstr(source, "White"))
		strcpy(destination, " White");
	else if (strstr(source, "Ginger"))
		strcpy(destination, " Red"); // There is only one instance of ginger hair, so caesar red is used when swapping it.
	else if (strstr(source, "Straw"))
		strcpy(destination, " Straw");
	else if (strstr(source, "Dark"))
		strcpy(destination, " Dark");
}

bool charactercreatorplus_checkforautoswapcategory(char* pSwappedPartName, int iSwappedPartCategory, int iCategoryToFind)
{
	std::vector<AutoSwapData*>* pSwapData = nullptr;

	switch (iSwappedPartCategory)
	{
	case 0: pSwapData = &g_headGearMandatorySwaps;
		break;
	}

	if (pSwapData == nullptr)
		return false;

	for (const auto& swap : *pSwapData)
	{
		// Check if the character has any parts equipped from the chosen auto swap category.
		if (strstr(pSwappedPartName, swap->sPartName.c_str()))
		{
			// Now see if there is any auto-swap data for the desired category (another type of part that gets swapped because of iSwappedPartCategory).
			for (int i = 0; i < swap->requiredSwapCategories.size(); i++)
			{
				if (swap->requiredSwapCategories[i] == iCategoryToFind)
				{
					// Found an auto swap with this category.
					return true;
				}
			}
		}
	}

	// Did not find an auto swap for the desired category.
	return false;
}

void charactercreatorplus_restrictpart(int part)
{
	// 0: Head Gear
	// 1: Hair
	// 2: Head
	// 3: Eye Glasses
	// 4: Facial Hair
	// 5: Body
	// 6: Legs
	// 7: Feet

	g_restrictedParts.clear();

	std::map<std::string, std::string>* pAnnotated = nullptr;
	char* pSelectedGear = nullptr;
	int iCategoryToFind = part;
	int iSwappedPartCategory = -1;

	// Restricting headgear is treated specially.
	if (part > 0)
	{
		// Restricting hair, head, glasses or facial hair (all depend on the chosen headgear).
		if (part < 5)
		{
			pAnnotated = &CharacterCreatorAnnotatedHeadGear_s;
			pSelectedGear = cSelectedHeadGear;
			iSwappedPartCategory = 0;
		}

		if (!pSelectedGear || !pAnnotated)
			return;

		char gear[256];

		// Get the annotation for the currently selected gear so we can compare it to the autoswaps.
		for (const auto& annotation : *pAnnotated)
		{
			if (stricmp(annotation.first.c_str(), pSelectedGear) == NULL)
			{
				strcpy(gear, annotation.second.c_str());
				break;
			}
		}

		if (strlen(gear) == 0)
			return;

		bool bRestricted = charactercreatorplus_checkforautoswapcategory(gear, iSwappedPartCategory, iCategoryToFind);

		// No restrictions needed.
		if (!bRestricted)
			return;

		// In future if we add more parts, it would be better to extract them from annotates automatically.
		switch (part)
		{
		case 1:
			if (stricmp(CCP_Type, "Adult Male") == NULL)
			{
				g_restrictedParts.push_back("Parting");
				g_restrictedParts.push_back("Swept");
				g_restrictedParts.push_back("Cropped");
				g_restrictedParts.push_back("Viking");
				g_restrictedParts.push_back("Aztec Bowl");
				g_restrictedParts.push_back("Aztec Ponytail");
			}
			else if (stricmp(CCP_Type, "Adult Female") == NULL)
			{
				g_restrictedParts.push_back("Ponytail");
				g_restrictedParts.push_back("Braids");
				g_restrictedParts.push_back("Wavy");
				g_restrictedParts.push_back("Punk");
			}
			break;

		case 2:
			if (stricmp(CCP_Type, "Adult Male") == NULL)
			{
				g_restrictedParts.push_back("Dark Old");
				g_restrictedParts.push_back("Asian Old");
				g_restrictedParts.push_back("Caucasian Old");
				g_restrictedParts.push_back("Mediterranean Old");
				g_restrictedParts.push_back("Worn");
			}
			break;

		case 3:
			g_restrictedParts.push_back("Vintage Glasses");
			g_restrictedParts.push_back("Sport Sunglasses");
			g_restrictedParts.push_back("Aviator Sunglasses");
			g_restrictedParts.push_back("Round Glasses");

			if (stricmp(CCP_Type, "Adult Female") == NULL)
				g_restrictedParts.push_back("Round Glasses");

			break;

		case 4:
			if (stricmp(CCP_Type, "Adult Male") == NULL)
			{
				g_restrictedParts.push_back("Scruffy");
				g_restrictedParts.push_back("Beard");
				g_restrictedParts.push_back("Moustache");
				g_restrictedParts.push_back("Biker");
			}

			break;
		}
	}
	else if (part == 0 && stricmp(CCP_Type, "Adult Male") == NULL)
	{
		std::vector<char*> headsWithDifferentStructure;
		headsWithDifferentStructure.push_back("Dark Old");
		headsWithDifferentStructure.push_back("Asian Old");
		headsWithDifferentStructure.push_back("Caucasian Old");
		headsWithDifferentStructure.push_back("Mediterranean Old");
		headsWithDifferentStructure.push_back("Worn");

		// Find the mesh name of the currently equipped head.
		char head[256];
		for (const auto& annotation : CharacterCreatorAnnotatedHead_s)
		{
			if (stricmp(annotation.first.c_str(), cSelectedHead) == NULL)
			{
				strcpy(head, annotation.second.c_str());
				break;
			}
		}

		// Check if the equipped head is one of the above with different texture structures, and is incompatible with some headgears.
		bool bHeadHasRestrictions = false;
		for (int i = 0; i < headsWithDifferentStructure.size(); i++)
		{
			if (strstr(head, headsWithDifferentStructure[i]))
			{
				bHeadHasRestrictions = true;
				break;
			}
		}

		if (bHeadHasRestrictions)
		{
			// There is no auto-swap data available for heads.
			// ...So going to search for a head category within each headgear autoswap and if found, it will be restricted.
			for (const auto& swap : g_headGearMandatorySwaps)
			{
				for (int i = 0; i < swap->requiredSwapCategories.size(); i++)
				{
					if (swap->requiredSwapCategories[i] == 2)
					{
						// This headgear auto-swap requires the face to be changed.
						// ... Since the currently equipped face is one of the structurally different ones, it is incompatible with the head gear.
						
						g_restrictedParts.push_back(&swap->sPartName[0]);
						break;
					}
				}
			}
		}
	}
}

void charactercreatorplus_initcameratransitions()
{		
	g_DefaultCamPosition.x = t.editorfreeflight.c.x_f;
	g_DefaultCamPosition.y = t.editorfreeflight.c.y_f;
	g_DefaultCamPosition.z = t.editorfreeflight.c.z_f;
	g_DefaultCamAngle.x = t.editorfreeflight.c.angx_f;
	g_DefaultCamAngle.y = t.editorfreeflight.c.angy_f;
	g_DefaultCamAngle.z = 0;
	g_CurrentCamPosition = g_DefaultCamPosition;
	g_CurrentCamAngle = g_DefaultCamAngle;

	// Set initial conditions for the transition when the ccp is initialised.
	g_pCurrentTransition = &g_UpperBodyTransition;
	g_pLastKnownTransition = &g_UpperBodyTransition;
	g_pCurrentTransition->from[0] = g_DefaultCamPosition.x;
	g_pCurrentTransition->from[1] = g_DefaultCamPosition.y;
	g_pCurrentTransition->from[2] = g_DefaultCamPosition.z;
	g_pCurrentTransition->angFrom[0] = g_DefaultCamAngle.x;
	g_pCurrentTransition->angFrom[1] = g_DefaultCamAngle.y;

	GGVECTOR3 characterPos(ObjectPositionX(iCharObj), ObjectPositionY(iCharObj), ObjectPositionZ(iCharObj));
	float fAngleX = g_DefaultCamAngle.x;
	float fAngleY = g_DefaultCamAngle.y;

	g_HeadTransition.t = 0.0f;
	g_HeadTransition.to[0] = characterPos.x + (g_DefaultCamPosition.x - characterPos.x) * 0.15f;
	g_HeadTransition.to[1] = characterPos.y + 56.5f + (g_DefaultCamPosition.y - characterPos.y) * 0.15f;
	g_HeadTransition.to[2] = characterPos.z + (g_DefaultCamPosition.z - characterPos.z) * 0.15f;
	g_HeadTransition.angTo[0] = fAngleX + 0.0f;
	g_HeadTransition.angTo[1] = fAngleY;

	g_ZombieHeadTransition.t = 0.0f;
	g_ZombieHeadTransition.to[0] = characterPos.x + (g_DefaultCamPosition.x - characterPos.x) * 0.2f + 5.0f;
	g_ZombieHeadTransition.to[1] = characterPos.y + 53.0f + (g_DefaultCamPosition.y - characterPos.y) * 0.1f;
	g_ZombieHeadTransition.to[2] = characterPos.z + (g_DefaultCamPosition.z - characterPos.z) * 0.15f;
	g_ZombieHeadTransition.angTo[0] = fAngleX - 3.0f;
	g_ZombieHeadTransition.angTo[1] = fAngleY;

	g_UpperBodyTransition.t = 0.0f;
	g_UpperBodyTransition.to[0] = characterPos.x + (g_DefaultCamPosition.x - characterPos.x) * 0.2f;
	g_UpperBodyTransition.to[1] = characterPos.y + 45.0f + (g_DefaultCamPosition.y - characterPos.y) * 0.2f;
	g_UpperBodyTransition.to[2] = characterPos.z + (g_DefaultCamPosition.z - characterPos.z) * 0.2f;
	g_UpperBodyTransition.angTo[0] = fAngleX + 10.0f;
	g_UpperBodyTransition.angTo[1] = fAngleY;

	g_ZombieBodyTransition.t = 0.0f;
	g_ZombieBodyTransition.to[0] = characterPos.x + (g_DefaultCamPosition.x - characterPos.x) * 0.2f;
	g_ZombieBodyTransition.to[1] = characterPos.y + 42.0f + (g_DefaultCamPosition.y - characterPos.y) * 0.15f;
	g_ZombieBodyTransition.to[2] = characterPos.z + (g_DefaultCamPosition.z - characterPos.z) * 0.2f;
	g_ZombieBodyTransition.angTo[0] = fAngleX + 10.0f;
	g_ZombieBodyTransition.angTo[1] = fAngleY;

	g_LowerBodyTransition.t = 0.0f;
	g_LowerBodyTransition.to[0] = characterPos.x + (g_DefaultCamPosition.x - characterPos.x) * 0.3f;
	g_LowerBodyTransition.to[1] = characterPos.y + 37.0f + (g_DefaultCamPosition.y - characterPos.y) * 0.3f;
	g_LowerBodyTransition.to[2] = characterPos.z + (g_DefaultCamPosition.z - characterPos.z) * 0.3f;
	g_LowerBodyTransition.angTo[0] = fAngleX + 40.0f;
	g_LowerBodyTransition.angTo[1] = fAngleY;
};

void charactercreatorplus_changecameratransition(int part)
{
	int ccp_part_order[] = { 2       ,1        ,4         ,0       ,3           ,5        ,6        ,7 };

	g_iPreviousCategorySelection = part;

	// Choose which camera transition to use.
	if (ccp_part_order[part] <= 4)
	{
		if(!strstr(CCP_Type, "Zombie"))
			g_pCurrentTransition = &g_HeadTransition;
		else
			g_pCurrentTransition = &g_ZombieHeadTransition;
	}
	else if (ccp_part_order[part] == 5)
	{
		if (!strstr(CCP_Type, "Zombie"))
			g_pCurrentTransition = &g_UpperBodyTransition;
		else
			g_pCurrentTransition = &g_ZombieBodyTransition;
	}
	else if (ccp_part_order[part] <= 7)
		g_pCurrentTransition = &g_LowerBodyTransition;
	else
		g_pCurrentTransition = nullptr;

	// Set the intial conditions for the camera transition.
	if (g_pCurrentTransition)
	{
		g_pCurrentTransition->from[0] = g_CurrentCamPosition.x;
		g_pCurrentTransition->from[1] = g_CurrentCamPosition.y;
		g_pCurrentTransition->from[2] = g_CurrentCamPosition.z;
		g_pCurrentTransition->angFrom[0] = g_CurrentCamAngle.x;
		g_pCurrentTransition->angFrom[1] = g_CurrentCamAngle.y;
		g_pCurrentTransition->t = 0.0f;
	}
}

void charactercreatorplus_performcameratransition(bool bIsZooming)
{
	if (g_pCurrentTransition)
	{
		g_pLastKnownTransition = g_pCurrentTransition;

		// Offset the target away from the character based on the zoom level.
		float fZoomLevel = 1.0f - g_fCCPZoom * 0.01f;
		GGVECTOR3 target(g_pCurrentTransition->to[0] + (g_DefaultCamPosition.x - g_pCurrentTransition->to[0]) * fZoomLevel, g_pCurrentTransition->to[1] + (g_DefaultCamPosition.y - g_pCurrentTransition->to[1]) *fZoomLevel, g_pCurrentTransition->to[2] + (g_DefaultCamPosition.z - g_pCurrentTransition->to[2]) * fZoomLevel);

		// Lerp between the starting and target position to get the desired camera position;
		GGVECTOR3 from(g_pCurrentTransition->from[0], g_pCurrentTransition->from[1], g_pCurrentTransition->from[2]);
		g_CurrentCamPosition = from + ((target - from) * g_pCurrentTransition->t);

		// Lerp between the starting and target rotation to get the desired camera rotation.
		GGVECTOR3 targetRotation(g_pCurrentTransition->angTo[0] + (g_DefaultCamAngle.x - g_pCurrentTransition->angTo[0]) * fZoomLevel, g_pCurrentTransition->angTo[1] + (g_DefaultCamAngle.y - g_pCurrentTransition->angTo[1]) * fZoomLevel, 0);
		GGVECTOR3 fromRotation(g_pCurrentTransition->angFrom[0], g_pCurrentTransition->angFrom[1], 0);
		g_CurrentCamAngle = fromRotation + (targetRotation - fromRotation) * g_pCurrentTransition->t;

		// If the user is zooming, increase the move speed so it feels responsive.
		float fMoveSpeed = 2.0f;
		if (bIsZooming)
			fMoveSpeed = 4.0f;

		// Increment t ready for the next frame.
		float dt = ImGui::GetIO().DeltaTime;
		if (dt > 0.05f) dt = 0.05f;
		g_pCurrentTransition->t += fMoveSpeed * dt;

		// Finished camera transition.
		if (g_pCurrentTransition->t >= 1.0f)
		{
			g_pCurrentTransition->t = 0.0f;
			g_pCurrentTransition = nullptr;
		}

		// Apply the camera changes.
		PositionCamera(g_CurrentCamPosition.x, g_CurrentCamPosition.y, g_CurrentCamPosition.z);
		RotateCamera(g_CurrentCamAngle.x, g_CurrentCamAngle.y, 0.0);
		t.editorfreeflight.c.x_f = g_CurrentCamPosition.x;
		t.editorfreeflight.c.y_f = g_CurrentCamPosition.y;
		t.editorfreeflight.c.z_f = g_CurrentCamPosition.z;
		t.editorfreeflight.c.angx_f = g_CurrentCamAngle.x;
		t.editorfreeflight.c.angy_f = g_CurrentCamAngle.y;
	}
}

void charactercreatorplus_dozoom()
{
	g_pCurrentTransition = g_pLastKnownTransition;
	if (g_pCurrentTransition)
	{
		g_pCurrentTransition->from[0] = g_CurrentCamPosition.x;
		g_pCurrentTransition->from[1] = g_CurrentCamPosition.y;
		g_pCurrentTransition->from[2] = g_CurrentCamPosition.z;
		g_pCurrentTransition->angFrom[0] = g_CurrentCamAngle.x;
		g_pCurrentTransition->angFrom[1] = g_CurrentCamAngle.y;
		charactercreatorplus_performcameratransition(true);
	}
}
