#define _CRT_SECURE_NO_WARNINGS

//#define PHOTONMP

// Includes
#include "..\\..\\..\\..\\GameGuru\Include\preprocessor-flags.h"
#include "SteamCheckForWorkshop.h"
#include <stdio.h>
#include "SteamCommands.h"
#include <string>
#include <CFileC.h>

extern "C" 
{
	FILE* GG_fopen( const char* filename, const char* mode ); 
	int GG_FileExists( const char* filename );
}

// Globals
LPSTR g_pRootFolder = NULL;

void SetWorkshopFolder ( LPSTR pFolder ) 
{ 
	// used to reference levelbank\testmap when loading custom content from FPM load area
	if ( g_pRootFolder == NULL )
	{
		g_pRootFolder = new char[2048];
		strcpy ( g_pRootFolder, pFolder ); 
	}
}

bool CheckForWorkshopFile ( LPSTR VirtualFilename )
{
	// some quick rejections
	if ( !VirtualFilename ) return false;
	if ( strlen ( VirtualFilename ) < 3 ) return false;
	char* tempCharPointerCheck = NULL;
	tempCharPointerCheck = strrchr( VirtualFilename, '\\' );
	if ( tempCharPointerCheck == VirtualFilename+strlen(VirtualFilename)-1 ) return false;
	if ( VirtualFilename[0] == '.' ) return false;
	if ( strnicmp (VirtualFilename + strlen(VirtualFilename) - 4, ".fpm", 4) == NULL) return false;

	// encrypted file check
	char szEncryptedFilename[_MAX_PATH];
	char szEncryptedFilenameFolder[_MAX_PATH];
	char* tempCharPointer = NULL;
	strcpy ( szEncryptedFilenameFolder, VirtualFilename );

	// replace and forward slashes with backslash
	for ( int c = 0 ; c < (int)strlen(szEncryptedFilenameFolder); c++ )
	{
		if ( szEncryptedFilenameFolder[c] == '/' ) 
			szEncryptedFilenameFolder[c] = '\\';
	}

	// assemble _e_ filename
	tempCharPointer = strrchr( szEncryptedFilenameFolder, '\\' );
	if ( tempCharPointer && tempCharPointer != szEncryptedFilenameFolder+strlen(szEncryptedFilenameFolder)-1 )
	{
		tempCharPointer[0] = 0;
		sprintf ( szEncryptedFilename , "%s\\_e_%s" , szEncryptedFilenameFolder , tempCharPointer+1 );
	}
	else
	{
		sprintf ( szEncryptedFilename , "_e_%s" , szEncryptedFilenameFolder );
	}

	// check it exists
	if ( GG_FileExists( szEncryptedFilename ) )
	{
		extern bool bCalledFromWickedLoadImage;
		if (bCalledFromWickedLoadImage)
		{
			//PE: Prefer dds over png. (color.png,basecolor.png...)
			const char last = szEncryptedFilename[strlen(szEncryptedFilename) - 3];
			if (last == 'p' || last == 'P') //PE: Quick png check.
			{
				const char* pestrcasestr(const char* arg1, const char* arg2);
				if (pestrcasestr(szEncryptedFilename, "color.png")
					|| pestrcasestr(szEncryptedFilename, "directx.png")
					|| pestrcasestr(szEncryptedFilename, "normal.png")
					|| pestrcasestr(szEncryptedFilename, "metallic.png")
					|| pestrcasestr(szEncryptedFilename, "_arm.png")
					|| pestrcasestr(szEncryptedFilename, "emissive.png")
					|| pestrcasestr(szEncryptedFilename, "baseColor.png")
					|| pestrcasestr(szEncryptedFilename, "surface.png")
					|| pestrcasestr(szEncryptedFilename, "normalmap.png")
					|| pestrcasestr(szEncryptedFilename, "albedo.png"))
				{
					//PE: Check if a .dds version exists. and if so use it.
					std::string checkfile = szEncryptedFilename;
					checkfile[checkfile.length() - 1] = 's';
					checkfile[checkfile.length() - 2] = 'd';
					checkfile[checkfile.length() - 3] = 'd';
					if (GG_FileExists(checkfile.c_str()))
					{
						strcpy(szEncryptedFilename, checkfile.c_str());
					}
				}
			}
		}
		strcpy ( VirtualFilename , szEncryptedFilename );
		return true;
	}
	else
	{
		extern bool bCalledFromWickedLoadImage;
		if (bCalledFromWickedLoadImage)
		{
			//PE: Prefer dds over png.
			const char last = VirtualFilename[strlen(VirtualFilename) - 3];
			if (last == 'p' || last == 'P') //PE: Quick png check.
			{
				std::string checkfile = VirtualFilename;
				const char* pestrcasestr(const char* arg1, const char* arg2);
				if (pestrcasestr(szEncryptedFilename, "color.png")
					|| pestrcasestr(szEncryptedFilename, "directx.png")
					|| pestrcasestr(szEncryptedFilename, "normal.png")
					|| pestrcasestr(szEncryptedFilename, "metallic.png")
					|| pestrcasestr(szEncryptedFilename, "_arm.png")
					|| pestrcasestr(szEncryptedFilename, "emissive.png")
					|| pestrcasestr(szEncryptedFilename, "baseColor.png")
					|| pestrcasestr(szEncryptedFilename, "surface.png")
					|| pestrcasestr(szEncryptedFilename, "normalmap.png")
					|| pestrcasestr(szEncryptedFilename, "albedo.png"))
				{
					//PE: Check if a .dds version exists. and if so use it.
					checkfile[checkfile.length() - 1] = 's';
					checkfile[checkfile.length() - 2] = 'd';
					checkfile[checkfile.length() - 3] = 'd';
					if (GG_FileExists(checkfile.c_str()))
					{
						strcpy(VirtualFilename, checkfile.c_str());
						return true;
					}
				}
			}
		}
	}
	// end of encrypted file check

	#ifdef PHOTONMP
		// Photon has no workshop support 
		if ( VirtualFilename && strlen ( VirtualFilename ) > 0 )
		{
			// but can load in custom content from levelbank\testmap if transported by FPM
			// clean file reference

			//PE: Got pOneFilePath = 'd:\dev\GameGuru MAX\Max\Files\levelbank\testmap\CUSTOM_D:_MAX-DocWrite_Files_thumbbank_user_synty_vikings_SM_Prop_Gravestone_02_Snow512x288.jpg'
			//PE: And a crash.
			//PE: Got anohter crash without a ':' = 'C:\Users\Preben\Documents\GameGuruApps\GameGuruMAX\Files\levelbank\testmap\CUSTOM_D__MAX-DocWrite_Files_thumbbank_user_synty_vikings_SM_Env_Path_Cobble_Stone_01512x288.jpg'
			//PE: Try to disable if we got a ':'.
			bool bValid = true;
			if (strstr(VirtualFilename, ":"))
			{
				bValid = false;
			}
			if (bValid)
			{
				LPSTR pOneFiledStr = new char[10 + strlen(VirtualFilename) + 1];
				strcpy(pOneFiledStr, "CUSTOM_");
				int nnn = 7;
				for (int n = 0; n < strlen(VirtualFilename); n++)
				{
					if (VirtualFilename[n] == '\\' && VirtualFilename[n + 1] == '\\') n++; // skip duplicate backslashes
					if (VirtualFilename[n] == '\\')
						pOneFiledStr[nnn++] = '_';
					else if (VirtualFilename[n] == ':') //Make sure we dont have a : , it will crash later.
						pOneFiledStr[nnn++] = '_';
					else
						pOneFiledStr[nnn++] = VirtualFilename[n];
				}
				pOneFiledStr[nnn] = 0;

				// attempt to load onefile reference from levelbank\testmap
				if (g_pRootFolder)
				{
					LPSTR plevelBankTestMapRef = "\\Files\\levelbank\\testmap\\";
					LPSTR pOneFilePath = new char[strlen(g_pRootFolder) + strlen(plevelBankTestMapRef) + strlen(pOneFiledStr) + 10];
					strcpy(pOneFilePath, g_pRootFolder);
					strcat(pOneFilePath, plevelBankTestMapRef);
					strcat(pOneFilePath, pOneFiledStr);
					if ( GG_FileExists(pOneFilePath) )
					{
						if ((strlen(pOneFilePath) + 5) < _MAX_PATH) strcpy(VirtualFilename, pOneFilePath);
						delete pOneFilePath;
						return true;
					}
					delete pOneFilePath;
				}
			}
		}

	#else
	#ifndef NOSTEAMORVIDEO
		char szWorkshopFilename[_MAX_PATH];
		char szWorkshopFilenameFolder[_MAX_PATH];
		char szWorkShopItemPath[_MAX_PATH];
		SteamGetWorkshopItemPathDLL(szWorkShopItemPath);
		//strcpy ( szWorkShopItemPath,"D:\\Games\\Steam\\steamapps\\workshop\\content\\266310\\378822626");
		// If the string is empty then there is no active workshop item, so we can return
		if ( strcmp ( szWorkShopItemPath , "" ) == 0 ) return false;
		tempCharPointer = NULL;
		strcpy ( szWorkshopFilenameFolder, VirtualFilename );

		// only check if the workshop item path isnt blank
		if ( strcmp ( szWorkShopItemPath , "" ) )
		{
			// replace and forward slashes with backslash
			for ( unsigned int c = 0 ; c < strlen(szWorkshopFilenameFolder); c++ )
			{
				if ( szWorkshopFilenameFolder[c] == '/' )
					szWorkshopFilenameFolder[c] = '\\';

				if ( szWorkshopFilenameFolder[c] == '_' )
					szWorkshopFilenameFolder[c] = '@';
			}

			// strip off any path to files folder
			bool found = true;
			while ( found )
			{
			char* stripped = strstr ( szWorkshopFilenameFolder , "Files\\" );
			if ( !stripped )
				stripped = strstr ( szWorkshopFilenameFolder , "files\\" );

			if ( stripped )
				strcpy ( szWorkshopFilenameFolder , stripped+6 );
			else
				found = false;
			}

			bool last = false;
			char tempstring[MAX_PATH];
			strcpy ( tempstring, szWorkshopFilenameFolder);
			strcpy ( szWorkshopFilenameFolder , "" );
			// replace and forward slashes with backslash
			for ( unsigned int c = 0 ; c < strlen(tempstring); c++ )
			{
				if ( tempstring[c] == '/' || tempstring[c] == '\\' ) 
				{
					if ( last == false )
					{
						strcat ( szWorkshopFilenameFolder , "_" );
						last = true;
					}
				}
				else
				{
					strcat ( szWorkshopFilenameFolder , " " );
					szWorkshopFilenameFolder[strlen(szWorkshopFilenameFolder)-1] = tempstring[c];
					last = false;
				}
			}

			//NEED TO CHECK IF THE FILE EXISTS FIRST, IF IT DOES WE COPY IT
			char szTempName[_MAX_PATH];
			strcpy ( szTempName , szWorkShopItemPath );
			strcat ( szTempName , "\\" );
			strcat ( szTempName , szWorkshopFilenameFolder );

			if ( GG_FileExists(szTempName) )
			{
				int szTempNamelength = strlen(szTempName);
				int virtualfilelength = strlen(VirtualFilename);				
				strcpy ( VirtualFilename , szTempName );
				return true;
			}
			else // check for encrypted version
			{
				char* tempCharPointer = NULL;

				tempCharPointer = strrchr( szTempName, '\\' );
				if ( tempCharPointer && tempCharPointer != szTempName+strlen(szTempName)-1 )
				{
					tempCharPointer[0] = 0;
					sprintf ( szWorkshopFilename , "%s\\_w_%s" , szTempName , tempCharPointer+1 );
				}
				else
				{
					sprintf ( szWorkshopFilename , "_w_%s" , szTempName );
				}
				if ( GG_FileExists(szWorkshopFilename) )
				{
					strcpy ( VirtualFilename , szWorkshopFilename );
					return true;
				}
			}
		}
	#endif
	#endif
	return false;
}

bool bCanIUse_E_ = false;

bool CanIUse_E_()
{
	return bCanIUse_E_;
}

void SetCanUse_e_ ( int flag )
{
	if ( flag == 1 ) 
		bCanIUse_E_ = true;
	else
		bCanIUse_E_ = false;
}

bool Steam_CanIUse_W_()
{
	// Check if the functions pointers exist, if they don't - jolly well make them!
	/*if ( Steam_SteamIsWorkshopLoadingOnDLL==NULL )
	{
		// Setup pointers to Steam functions
		SteamMultiplayerModule = LoadLibrary ( "SteamMultiplayer.dll" );

		if ( !SteamMultiplayerModule )
		{
			//MessageBox(NULL, "Unable to find SteamMultiplayer", "SteamMultiplayer Error", NULL);
			return false;
		}

		Steam_SteamGetWorkshopItemPathDLL=(t_SteamGetWorkshopItemPathDLL)GetProcAddress( SteamMultiplayerModule , "?SteamGetWorkshopItemPathDLL@@YAXPAD@Z" );
		Steam_SteamIsWorkshopLoadingOnDLL=(t_SteamIsWorkshopLoadingOnDLL)GetProcAddress ( SteamMultiplayerModule , "?SteamIsWorkshopLoadingOnDLL@@YAHXZ" );	
		// End of setup pointers to steam functions
	}

	if ( !Steam_SteamIsWorkshopLoadingOnDLL || !Steam_SteamGetWorkshopItemPathDLL ) 
		return false;*/	

	#ifndef NOSTEAMORVIDEO
	if ( SteamIsWorkshopLoadingOnDLL() == 1 )
		return true;
	#endif

	return false;
}