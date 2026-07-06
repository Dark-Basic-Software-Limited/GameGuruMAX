//----------------------------------------------------
//--- GAMEGURU - Welcome System
//----------------------------------------------------

// Includes
#include "stdafx.h"
#include "gameguru.h"
#include "M-WelcomeSystem.h"
#include "..\..\Dark Basic Public Shared\Include\CInputC.h"
#include <time.h>
#include <wininet.h>
#include <mmsystem.h>

//PE: GameGuru IMGUI.
#include "..\Imgui\imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "..\Imgui\imgui_internal.h"
#include "..\Imgui\imgui_impl_win32.h"
#include "..\Imgui\imgui_gg_dx11.h"
#include <algorithm>
#include <string>
#include <time.h>

// Externals
extern int g_trialStampDaysLeft;
extern char g_trialDiscountExpires[1024];
extern char g_trialDiscountCode[1024];

// Prototypes
UINT OpenURLForDataOrFile ( LPSTR pDataReturned, DWORD* pReturnDataSize, LPSTR pUniqueCode, LPSTR pVerb, LPSTR urlWhere, LPSTR pLocalFileForImageOrNews );
void CleanStringOfEscapeSlashes ( char* pText );
void set_inputsys_mclick(int value);

// Globals
extern char g_pCloudKeyErrorString[10240];
extern char g_pCloudKeyExpiresDate[11];
extern bool g_bCloudKeyIsHomeEdition;
int g_welcomesystemclosedown = 0;
struct welcomepagetype
{
	int iTopLeftX;
	int iTopLeftY;
	int iPageWidth;
	int iPageHeight;
	int iCenterX;
	int iCenterY;
	float fPercToPageX;
	float fPercToPageY;
};
welcomepagetype g_welcome;
int g_iWelcomeLoopPage = 0;

struct welcomebuttontype
{
	int x1, y1, x2, y2;
	int alpha;
};
welcomebuttontype g_welcomebutton[20];
int g_welcomebuttoncount = 0;
bool gbWelcomeSystemActive = false;
int g_welcomeCycle = 0;
char g_welcomeText[1024];
char g_welcomeImageUrl[1024];
char g_welcomeLinkUrl[1024];
bool bReadyForInput = false;

void welcome_loadasset ( cstr welcomePath, LPSTR pImageFilename, int iImageID )
{
	cstr imageFile = welcomePath+pImageFilename; 
	LoadImage ( imageFile.Get(), iImageID );
}

void welcome_waitfornoinput ( void )
{
	OpenFileMap (  1, "FPSEXCHANGE" );
	SetEventAndWait (  1 );
	do
	{
		t.inputsys.kscancode=ScanCode();
		set_inputsys_mclick(MouseClick());// t.inputsys.mclick = MouseClick();
		t.terrain.gameplaycamera=0;
		// stretch anim backdrop to size of client window
		Sprite ( 123, -100000, -100000, g.editorimagesoffset+12 );
		SizeSprite ( 123, GetChildWindowWidth(0)+1, GetChildWindowHeight(0)+11 );
		PasteSprite ( 123, 0, 0 );
		FastSync (  );
	} while ( !( t.inputsys.kscancode <= 3 && t.inputsys.mclick == 0 ) );  //PE: We can keep getting virtual keys <= 3.
}

void welcome_init ( int iLoadingPart )
{
	// iLoadingPart:
	// 1-only load anim backdrop (quick load and show)
	// 2-load rest of core gfx for welcome system
	// 0-load MAIN gfx

	// welcome system is active
	gbWelcomeSystemActive = true;

	// load assets for welcome system (can use g.editorimagesoffset+8 through +60)
	// in addition, cannot have 10, 13, 14, 18, 21, 22, 23, 26 (from editor core)
	cstr welcomePath = cstr("languagebank\\")+g.language_s+cstr("\\artwork\\");

	// using legacy image loading
	image_setlegacyimageloading(true);

	// load only backdrop for quick refresh when launching
	if ( iLoadingPart == 1 )
	{
		// only interested in anim backdrop for now
		welcome_loadasset ( welcomePath, "welcome\\animated-backdrop.png", g.editorimagesoffset+12 );
		welcome_loadasset ( welcomePath, "welcome-assets\\splash-logo.bmp", g.editorimagesoffset+41 );
	}
	if ( iLoadingPart == 2 )
	{
		// what you get
		welcome_loadasset ( welcomePath, "welcome\\welcome-page.png", g.editorimagesoffset+8 );
		welcome_loadasset ( welcomePath, "welcome-assets\\product-logo.png", g.editorimagesoffset+9 );
		welcome_loadasset ( welcomePath, "welcome\\animated-backdrop.png", g.editorimagesoffset+12 );

		// load in 3x3 pieces
		welcome_loadasset ( welcomePath, "welcome\\welcome-line-1.png", g.editorimagesoffset+31 );
		welcome_loadasset ( welcomePath, "welcome\\welcome-line-2.png", g.editorimagesoffset+32 );
		welcome_loadasset ( welcomePath, "welcome\\welcome-line-3.png", g.editorimagesoffset+33 );
		welcome_loadasset ( welcomePath, "welcome\\welcome-line-4.png", g.editorimagesoffset+34 );
		welcome_loadasset ( welcomePath, "welcome\\welcome-line-5.png", g.editorimagesoffset+35 );
		welcome_loadasset ( welcomePath, "welcome\\welcome-line-6.png", g.editorimagesoffset+36 );
		welcome_loadasset ( welcomePath, "welcome\\welcome-line-7.png", g.editorimagesoffset+37 );
		welcome_loadasset ( welcomePath, "welcome\\welcome-line-8.png", g.editorimagesoffset+38 );
		welcome_loadasset ( welcomePath, "welcome\\welcome-line-9.png", g.editorimagesoffset+39 );
	}
	if ( iLoadingPart == 0 )
	{
		// load tick/bits assets
		welcome_loadasset ( welcomePath, "welcome\\welcome-tick.png", g.editorimagesoffset+40 );

		// welcome main page
		welcome_loadasset ( welcomePath, "welcome\\welcome-learn.png", g.editorimagesoffset+56 );
		welcome_loadasset ( welcomePath, "welcome\\welcome-play.png", g.editorimagesoffset+57 );
		welcome_loadasset ( welcomePath, "welcome\\welcome-create.png", g.editorimagesoffset+58 );

		// screenshot thumb for sample level
		welcome_loadasset ( welcomePath, "welcome-assets\\product-example.png", g.editorimagesoffset+59 );

		// image if free weekend used
		// reserved  g.editorimagesoffset+61  test game loaded arrow
		if ( g.iFreeVersionModeActive == 1 )
		{
			// only used for free weekend build version
			welcome_loadasset ( welcomePath, "welcome-assets\\free-weekend.png", g.editorimagesoffset+60 );
			welcome_loadasset ( welcomePath, "welcome-assets\\free-weekend-prompt.png", g.editorimagesoffset+62 );
			welcome_loadasset ( welcomePath, "welcome-assets\\free-weekend-click.png", g.editorimagesoffset+63 );
		}
		if ( g.iFreeVersionModeActive == 2 )
		{
			// only used for free trial version
			welcome_loadasset ( welcomePath, "welcome-assets\\free-trial.png", g.editorimagesoffset+60 );
			#ifdef STEAMTRIAL
			 welcome_loadasset(welcomePath, "welcome-assets\\free-steam-trial-prompt.png", g.editorimagesoffset + 62);
			#else
			 welcome_loadasset ( welcomePath, "welcome-assets\\free-trial-prompt.png", g.editorimagesoffset+62 );
			#endif
			welcome_loadasset ( welcomePath, "welcome-assets\\free-trial-click.png", g.editorimagesoffset+63 );
		}
		
		// announcement system
		if (ImageExist(g.editorimagesoffset + 64) == 0)
		{
			 welcome_loadasset(welcomePath, "welcome-assets\\gameguru-news-banner.png", g.editorimagesoffset + 64);
		}

		// registered trade mark
		welcome_loadasset(welcomePath, "registered.png", g.editorimagesoffset + 65);
		welcome_loadasset(welcomePath, "registeredsmall.png", g.editorimagesoffset + 66);
	}

	// using legacy image loading
	image_setlegacyimageloading(false);

	// calculate page dimensions and useful vars
	memset ( &g_welcome, 0, sizeof(g_welcome) );
	g_welcome.iPageWidth = ImageWidth ( g.editorimagesoffset+8 );
	g_welcome.iPageHeight = ImageHeight ( g.editorimagesoffset+8 );
	g_welcome.iCenterX = g_pGlob->iScreenWidth/2;
	g_welcome.iCenterY = g_pGlob->iScreenHeight/2;
	g_welcome.iTopLeftX = g_welcome.iCenterX-(g_welcome.iPageWidth/2);
	g_welcome.iTopLeftY = g_welcome.iCenterY-(g_welcome.iPageHeight/2);
	g_welcome.fPercToPageX = g_welcome.iPageWidth / 100.0f;
	g_welcome.fPercToPageY = g_welcome.iPageHeight / 100.0f;

	// master flag to close down welcome system from anywhere within init/free sequence
	g_welcomesystemclosedown = 0;
}

void welcome_free ( void )
{
	// free welcome assets (save memory)
	for ( int iID = 30; iID <= 60; iID++ )
		if ( iID != 12 )
			if ( GetImageExistEx ( g.editorimagesoffset + iID ) == 1 )
				DeleteImage ( g.editorimagesoffset + iID );

	// welcome system not active
	gbWelcomeSystemActive = false;

	// if triggered a load, do it here, just before delete backdrop
	if ( strlen(t.tlevelautoload_s.Get()) > 0 )
	{
		// just render backdrop for the load pause 
		welcome_staticbackdrop();

		//  ask to save first if modified project open
		t.editorcanceltask=0;
		if ( g.projectmodified == 1 ) 
		{
			// If project modified, ask if want to save first
			gridedit_intercept_savefirst ( );
		}
		if ( t.editorcanceltask == 0 ) 
		{
			if ( t.tlevelautoload_s != "" ) 
			{
				if ( cstr(Lower(Right(t.tlevelautoload_s.Get(),4))) == ".fpm" ) 
				{
					// leave dialog for the load
					g.projectfilename_s=t.tlevelautoload_s;
					gridedit_load_map ( );
					g.showtestlevelclickprompt = timeGetTime() + 5000;

					// prevent brand new level from being wiped
					t.bIgnoreFirstCallToNewLevel = true;
				}
			}
		}
	}

	// now delete backdrop
	if ( GetImageExistEx ( g.editorimagesoffset + 12 ) == 1 ) DeleteImage ( g.editorimagesoffset + 12 );
}

void welcome_staticbackdrop ( void )
{
	// No static backdrop in MAX
}

void welcome_updatebackdrop ( char* pText )
{
	// No backdrop in MAX
}

void welcome_converttopagecoords ( float& fX1, float& fY1 )
{
	fX1 *= g_welcome.fPercToPageX;
	fY1 *= g_welcome.fPercToPageY;
}

void welcome_converttopagecoords ( float& fX1, float& fY1, float& fX2, float& fY2 )
{
	fX1 *= g_welcome.fPercToPageX;
	fY1 *= g_welcome.fPercToPageY;
	fX2 *= g_welcome.fPercToPageX;
	fY2 *= g_welcome.fPercToPageY;
}

void welcome_drawbox_core ( int iButtonID, float fX1, float fY1, float fX2, float fY2, int iVisible )
{
	// use 3x3 pieces to form a variable sized box
	welcome_converttopagecoords ( fX1, fY1, fX2, fY2 );
	int iBoxX1 = (g_welcome.iTopLeftX + fX1)-8;
	int iBoxY1 = (g_welcome.iTopLeftY + fY1)-8;
	int iBoxX2 = (g_welcome.iTopLeftX + fX2);
	int iBoxY2 = (g_welcome.iTopLeftY + fY2);
	if ( iVisible == 1 )
	{
		PasteImage ( g.editorimagesoffset+31, iBoxX1, iBoxY1, 1 );
		for ( int iX = iBoxX1+8; iX < iBoxX2; iX+=8 )
		{
			PasteImage ( g.editorimagesoffset+32, iX, iBoxY1, 1 );
			PasteImage ( g.editorimagesoffset+38, iX, iBoxY2, 1 );
		}
		PasteImage ( g.editorimagesoffset+33, iBoxX2, iBoxY1, 1 );
		for ( int iY = iBoxY1+8; iY < iBoxY2; iY+=8 )
		{
			PasteImage ( g.editorimagesoffset+34, iBoxX1, iY, 1 );
			PasteImage ( g.editorimagesoffset+36, iBoxX2, iY, 1 );
		}
		PasteImage ( g.editorimagesoffset+37, iBoxX1, iBoxY2, 1 );
		PasteImage ( g.editorimagesoffset+39, iBoxX2, iBoxY2, 1 );
	}

	// assign button if one
	if ( iButtonID > 0 )
	{
		if ( iButtonID > g_welcomebuttoncount ) g_welcomebuttoncount = iButtonID;
		g_welcomebutton[iButtonID].x1 = iBoxX1;
		g_welcomebutton[iButtonID].y1 = iBoxY1;
		g_welcomebutton[iButtonID].x2 = iBoxX2;
		g_welcomebutton[iButtonID].y2 = iBoxY2;
		g_welcomebutton[iButtonID].alpha = 192;
	}
}

void welcome_drawbox ( int iButtonID, float fX1, float fY1, float fX2, float fY2 )
{
	welcome_drawbox_core ( iButtonID, fX1, fY1, fX2, fY2, 1 );
}

void welcome_drawbox_hide ( int iButtonID, float fX1, float fY1, float fX2, float fY2 )
{
	welcome_drawbox_core ( iButtonID, fX1, fY1, fX2, fY2, 0 );
}

void welcome_text ( LPSTR pText, int iFontType, float fX, float fY, float fAlpha, bool bMultiline, bool bLeftAligned )
{
	welcome_converttopagecoords ( fX, fY );
	int iNextLine = 0;
	bool bHaveALine = true;
	char pLineData[10240];
	char pThisLine[10240];
	char pWorkLine[10240];
	strcpy ( pLineData, pText );
	while ( bHaveALine==true )
	{
		bHaveALine = false;
		strcpy ( pThisLine, pLineData );
		if ( bMultiline == true )
		{
			for ( int n = 0; n < strlen(pLineData); n++ )
			{
				if ( pLineData[n] == '\n' )
				{
					strcpy ( pThisLine, pLineData );
					pThisLine[n] = 0;
					strcpy ( pWorkLine, pLineData+n+1 );
					strcpy ( pLineData, pWorkLine );
					bHaveALine = true;
					break;
				}
			}
		}
		int iTextRealHeight = getbitmapfontheight(iFontType);
		float fTextWidth = getbitmapfontwidth(pThisLine,iFontType) / g_welcome.fPercToPageX;
		float fTextHeight = iTextRealHeight / g_welcome.fPercToPageY;
		int iTextX = (g_welcome.iTopLeftX + fX);
		int iTextY = (g_welcome.iTopLeftY + fY + 1) - (iTextRealHeight/2.0f);
		if ( bLeftAligned == true )
			pastebitmapfont ( pThisLine, iTextX, iTextY+(iNextLine*iTextRealHeight), iFontType, fAlpha);
		else
			pastebitmapfontcenter ( pThisLine, iTextX, iTextY+(iNextLine*iTextRealHeight), iFontType, fAlpha);
		iNextLine++;
	}
}

void welcome_textinbox_fixedwidth ( int iButtonID, LPSTR pText, int iFontType, float fX, float fY, float fAlpha, float fOptionalFixedWidth )
{
	float fTextWidth = getbitmapfontwidth(pText,iFontType) / g_welcome.fPercToPageX;
	float fTextHeight = getbitmapfontheight(iFontType) / g_welcome.fPercToPageY;
	float fBoxWidth = fTextWidth;
	if (fOptionalFixedWidth > 0.0f) fBoxWidth = fOptionalFixedWidth;
	float fX1 = fX - fBoxWidth;
	float fY1 = fY - (fTextHeight/2);
	float fX2 = fX + fBoxWidth;
	float fY2 = fY + (fTextHeight/2);
	if ( fAlpha >= 0 )
		welcome_drawbox ( iButtonID, fX1, fY1, fX2, fY2 );
	else
	{
		welcome_drawbox_hide ( iButtonID, fX1, fY1, fX2, fY2 );
		fAlpha = 0.0f;
	}
	if ( iButtonID == 0 ) fAlpha = 32;
	welcome_text ( pText, iFontType, fX, fY, fAlpha, false, false );
}

void welcome_textinbox (int iButtonID, LPSTR pText, int iFontType, float fX, float fY, float fAlpha)
{
	welcome_textinbox_fixedwidth (iButtonID, pText, iFontType, fX, fY, fAlpha, 0.0f);
}

void welcome_drawimage ( int iImageID, float fX, float fY, bool bLeftAligned )
{
	welcome_converttopagecoords ( fX, fY );
	int iX = g_welcome.iTopLeftX + fX;
	int iY = g_welcome.iTopLeftY + fY;
	if ( bLeftAligned == false ) iX -= (ImageWidth(iImageID)/2.0f);
	PasteImage ( iImageID, iX, iY );
}

void welcome_drawrotatedimage ( int iImageID, float fX, float fY, float fAngle, int iOffsetX, int iOffsetY, bool bLeftAligned )
{
	welcome_converttopagecoords ( fX, fY );
	int iX = g_welcome.iTopLeftX + fX;
	int iY = g_welcome.iTopLeftY + fY;
	Sprite ( 123, -10000, -10000, iImageID );
	SizeSprite ( 123, ImageWidth(iImageID), ImageHeight(iImageID) );
	OffsetSprite ( 123, iOffsetX, iOffsetY );
	RotateSprite ( 123, fAngle );
	if ( bLeftAligned == false ) iX -= (ImageWidth(iImageID)/2.0f);
	PasteSprite ( 123, iX, iY );
}

// SERIAL CODE PAGE

struct welcomeserialcodetype
{
	int iNextButtonID;
	char pCode[23];
	int iKeyPressed;
	int iAfterError;
	DWORD dwCodeEntryTimeStamp;
	bool bCodeEntryCursor;
};
welcomeserialcodetype g_welcomeserialcode;

void welcome_serialcode_init ( void )
{
	memset ( &g_welcomeserialcode, 0, sizeof(g_welcomeserialcode) );
	g_welcomeserialcode.iNextButtonID = 1;
	strcpy ( g_welcomeserialcode.pCode, "" );
	g_welcomeserialcode.iKeyPressed = 1;
	g_welcomeserialcode.iAfterError = 0;
}

void welcome_serialcode_page ( int iHighlightingButton )
{
	// string variants
	int iCodeDigitsRequiredCount = 0;
	#ifdef CLOUDKEYSYSTEM
		LPSTR pCodeTitle = "CLOUD KEY ENTRY\n";
		LPSTR pCodeEnterText = "Enter the cloud key (XXXXX-XXXXX-XXXXX-XXXXX) to activate";
		LPSTR pCodeWrong = g_pCloudKeyErrorString;//"Cloud key has been entered incorrectly or has expired.";
		iCodeDigitsRequiredCount = 23;
		if (g.bCleverbooksBundleMode == true)
		{
			pCodeTitle = "CLEVERBOOKS BUNDLE ACCESS\n";
			pCodeEnterText = "Enter the username and password of your Cleverbooks account to activate";
			pCodeWrong = g_pCloudKeyErrorString;
		}
	#else
		LPSTR pCodeTitle = "SERIAL CODE ENTRY\n";
		LPSTR pCodeEnterText = "Please enter the 22-digit serial code to activate this software.";
		LPSTR pCodeWrong = "Serial code has been entered incorrectly or has expired.";
		iCodeDigitsRequiredCount = 22;
	#endif
	welcome_text ( pCodeTitle, 1, 50, 28, 192, true, false );

	// cleverbooks or regular cloud/.serial key entry
	int iConfirmID = 0;
	if (g.bCleverbooksBundleMode == true)
	{
		// draw page for cleverbooks USERNAME and PASSWORD entry
		welcome_drawrotatedimage ( g.editorimagesoffset+9, 50, 5, 0, 0, 0, false );
		welcome_drawbox ( 0, 10, 23, 90, 81 );		
		welcome_text ( pCodeTitle, 1, 50, 28, 192, true, false );

		if (g_welcomeserialcode.iAfterError > 1)
		{
			welcome_text (pCodeWrong, 1, 50, 50, 192, true, false);
			int iOkayID = 3; welcome_textinbox (iOkayID, "TRY AGAIN", 1, 50, 60, g_welcomebutton[iOkayID].alpha);
			int iUserNameID = 4; welcome_textinbox_fixedwidth (iUserNameID, "", 1, 50, 150, 192, 30);
			int iPasswordID = 5; welcome_textinbox_fixedwidth (iPasswordID, "", 1, 50, 160, 192, 30);
		}
		else
		{
			welcome_text (pCodeEnterText, 1, 50, 38, 192, true, false);

			// cursor flash
			if (Timer() > g_welcomeserialcode.dwCodeEntryTimeStamp)
			{
				g_welcomeserialcode.dwCodeEntryTimeStamp = Timer() + 500;
				if (g_welcomeserialcode.bCodeEntryCursor == true)
					g_welcomeserialcode.bCodeEntryCursor = false;
				else
					g_welcomeserialcode.bCodeEntryCursor = true;
			}

			// username
			char pFinalCodeDisplay[1024];
			strcpy (pFinalCodeDisplay, g.cleverbooksmodeusername.Get());
			if (g.cleverbooksmodeentryindex == 0)
			{
				if (g_welcomeserialcode.bCodeEntryCursor == true)
					strcat (pFinalCodeDisplay, "|");
				else
					strcat (pFinalCodeDisplay, " ");
			}
			int iUserNameID = 4; welcome_textinbox_fixedwidth (iUserNameID, pFinalCodeDisplay, 1, 50, 50, 192, 30);

			// password
			strcpy (pFinalCodeDisplay, g.cleverbooksmodepassword.Get());
			if (g.cleverbooksmodeentryindex == 1)
			{
				if (g_welcomeserialcode.bCodeEntryCursor == true)
					strcat (pFinalCodeDisplay, "|");
				else
					strcat (pFinalCodeDisplay, " ");
			}
			int iPasswordID = 5; welcome_textinbox_fixedwidth (iPasswordID, pFinalCodeDisplay, 1, 50, 60, 192, 30);
		}

		welcome_text ( "You can use CTRL+V to paste contents of clipboard", 1, 50, 75, 192, true, false );
		iConfirmID = g_welcomeserialcode.iNextButtonID;
		welcome_textinbox (iConfirmID, "CONFIRM", 1, 50, 90, g_welcomebutton[iConfirmID].alpha);
		int iQuitID = 2; welcome_textinbox ( iQuitID, "QUIT", 1, 85, 90, g_welcomebutton[iQuitID].alpha );
	}
	else
	{
		// draw page
		welcome_drawrotatedimage (g.editorimagesoffset + 9, 50, 5, 0, 0, 0, false);
		welcome_drawbox (0, 10, 23, 90, 81);
		welcome_text (pCodeTitle, 1, 50, 28, 192, true, false);
		if (g_welcomeserialcode.iAfterError > 1)
		{
			welcome_text (pCodeWrong, 1, 50, 50, 192, true, false);
			int iOkayID = 3; welcome_textinbox (iOkayID, "TRY AGAIN", 1, 50, 70, g_welcomebutton[iOkayID].alpha);
		}
		else
		{
			welcome_text (pCodeEnterText, 1, 50, 43, 192, true, false);
			welcome_drawbox (0, 30, 50, 70, 60);
			char pFinalCodeDisplay[1024];
			strcpy (pFinalCodeDisplay, g_welcomeserialcode.pCode);
			if (Timer() > g_welcomeserialcode.dwCodeEntryTimeStamp)
			{
				g_welcomeserialcode.dwCodeEntryTimeStamp = Timer() + 500;
				if (g_welcomeserialcode.bCodeEntryCursor == true)
					g_welcomeserialcode.bCodeEntryCursor = false;
				else
					g_welcomeserialcode.bCodeEntryCursor = true;
			}
			if (g_welcomeserialcode.bCodeEntryCursor == true)
				strcat (pFinalCodeDisplay, "|");
			else
				strcat (pFinalCodeDisplay, " ");
			welcome_text (pFinalCodeDisplay, 1, 50, 55, 192, true, false);
			welcome_text ("You can use CTRL+V to paste contents of clipboard", 1, 50, 65, 192, true, false);
			iConfirmID = g_welcomeserialcode.iNextButtonID; if (strlen(g_welcomeserialcode.pCode) < iCodeDigitsRequiredCount) iConfirmID = 0;
			welcome_textinbox (iConfirmID, "CONFIRM", 1, 50, 90, g_welcomebutton[iConfirmID].alpha);
			if (g_welcomeserialcode.iAfterError == 1)
			{
				int iQuitID = 2; welcome_textinbox (iQuitID, "QUIT", 1, 85, 90, g_welcomebutton[iQuitID].alpha);
			}
		}
	}

	// serial code entry control
	int iKeyScanCode = ScanCode();
	if ( iKeyScanCode == 0 ) g_welcomeserialcode.iKeyPressed = 0;
	if ( iKeyScanCode > 0 && g_welcomeserialcode.iKeyPressed == 0)
	{
		unsigned char c = InKey();
		char pRealChar = 0;
		LPSTR pEntryStr = Entry();
		if (strlen(pEntryStr) > 0)
		{
			pRealChar = pEntryStr[0];
			ClearEntryBuffer();
		}
		if (g.bCleverbooksBundleMode == true)
		{
			// cleverbooks auth username and pasword
			bool bAssumeItIsACharacter = true;
			if (iKeyScanCode == 28 || iKeyScanCode == 15)
			{
				g_welcomeserialcode.iKeyPressed = 1;
				if (g.cleverbooksmodeentryindex == 0)
					g.cleverbooksmodeentryindex = 1;
				else
					g.cleverbooksmodeentryindex = 0;
				bAssumeItIsACharacter = false;
			}
			if ( iKeyScanCode == 14 )
			{
				if (g.cleverbooksmodeentryindex == 0)
				{
					if (strlen(g.cleverbooksmodeusername.Get()) > 0)
					{
						g_welcomeserialcode.iKeyPressed = 1;
						g.cleverbooksmodeusername = Left(g.cleverbooksmodeusername.Get(), Len(g.cleverbooksmodeusername.Get()) - 1);
					}
				}
				if (g.cleverbooksmodeentryindex == 1)
				{
					if (strlen(g.cleverbooksmodepassword.Get()) > 0)
					{
						g_welcomeserialcode.iKeyPressed = 1;
						g.cleverbooksmodepassword = Left(g.cleverbooksmodepassword.Get(), Len(g.cleverbooksmodepassword.Get()) - 1);
					}
				}
				bAssumeItIsACharacter = false;
			}
			if (iKeyScanCode == 29 && c == 118)
			{
				g_welcomeserialcode.iKeyPressed = 1;
				if (g.cleverbooksmodeentryindex == 0) g.cleverbooksmodeusername = "invalid data";
				if (g.cleverbooksmodeentryindex == 1) g.cleverbooksmodepassword = "invalid data";
				LPSTR pClipboardData = (LPSTR)GetClipboard(NULL);
				if (pClipboardData)
				{
					DWORD dwClipboardSize = strlen(pClipboardData);
					LPSTR pBuffer = new char[dwClipboardSize+1];
					memset (pBuffer, 0, sizeof(pBuffer));
					strcpy (pBuffer, (LPSTR)GetClipboard(NULL));
					if (g.cleverbooksmodeentryindex == 0) g.cleverbooksmodeusername = pBuffer;
					if (g.cleverbooksmodeentryindex == 1) g.cleverbooksmodepassword = pBuffer;
					SAFE_DELETE(pBuffer);
				}
				bAssumeItIsACharacter = false;
			}
			if ( bAssumeItIsACharacter == true && pRealChar > 0 )
			{
				g_welcomeserialcode.iKeyPressed = 1;
				if (g.cleverbooksmodeentryindex == 0 && stricmp ( g.cleverbooksmodeusername.Get(), "username" ) == NULL ) g.cleverbooksmodeusername = "";
				if (g.cleverbooksmodeentryindex == 1 && stricmp ( g.cleverbooksmodepassword.Get(), "password" ) == NULL ) g.cleverbooksmodepassword = "";
				if (g.cleverbooksmodeentryindex == 0) g.cleverbooksmodeusername = g.cleverbooksmodeusername + Chr(pRealChar);
				if (g.cleverbooksmodeentryindex == 1) g.cleverbooksmodepassword = g.cleverbooksmodepassword + Chr(pRealChar);
			}
		}
		else
		{
			// regular key entry
			if ( (c >= 48 && c <= 57) || (c >= 97 && c <= 122) || c == 45)
			{
				g_welcomeserialcode.iKeyPressed = 1;
				if (strlen(g_welcomeserialcode.pCode) < iCodeDigitsRequiredCount)
				{
					strcat (g_welcomeserialcode.pCode, Chr(c));
					strupr (g_welcomeserialcode.pCode);
				}
			}
			if ( iKeyScanCode == 14 )
			{
				if ( strlen(g_welcomeserialcode.pCode) > 0 )
				{
					g_welcomeserialcode.iKeyPressed = 1;
					g_welcomeserialcode.pCode[strlen(g_welcomeserialcode.pCode)-1] = 0;
				}
			}
			if (iKeyScanCode == 29 && c == 118)
			{
				g_welcomeserialcode.iKeyPressed = 1;
				strcpy (g_welcomeserialcode.pCode, "invalid data");
				LPSTR pClipboardData = (LPSTR)GetClipboard(NULL);
				if (pClipboardData)
				{
					DWORD dwClipboardSize = strlen(pClipboardData);
					if (dwClipboardSize <= iCodeDigitsRequiredCount)
					{
						LPSTR pBuffer = new char[24];//dwClipboardSize+1];
						memset (pBuffer, 0, sizeof(pBuffer));
						strcpy (pBuffer, (LPSTR)GetClipboard(NULL));
						pBuffer[iCodeDigitsRequiredCount] = 0;
						strcpy (g_welcomeserialcode.pCode, pBuffer);
						SAFE_DELETE(pBuffer);
					}
				}
			}
		}
	}

	// control page
	if ( t.inputsys.mclick == 1 ) 
	{
		if ( iHighlightingButton == 1 && iConfirmID > 0 ) 
		{
			if (g.bCleverbooksBundleMode == true)
			{
				// check if auth valid
				if (common_isserialcodevalid(g.cleverbooksmodepassword.Get(), g.cleverbooksmodeusername.Get()) == 1)
				{
					common_writeserialcode(g.cleverbooksmodepassword.Get(), g.cleverbooksmodeusername.Get());
					g.iTriggerSoftwareToQuit = 0;
					t.tclosequick = 1;
				}
				else
				{
					// serial code is invalid or expired
					g_welcomeserialcode.iAfterError = 2;
				}
			}
			else
			{
				// check if serial code valid
				if (common_isserialcodevalid(g_welcomeserialcode.pCode, NULL) == 1)
				{
					// write correct serial code to VRQ file
					common_writeserialcode(g_welcomeserialcode.pCode,NULL);

					// and ensure we can pass to active app
					g.iTriggerSoftwareToQuit = 0;

					// and continue
					t.tclosequick = 1;
				}
				else
				{
					// serial code is invalid or expired
					g_welcomeserialcode.iAfterError = 2;
				}
			}
		}
		if ( iHighlightingButton == 2 ) 
		{
			// signal GameGuru to close down immediately
			g.iTriggerSoftwareToQuit = 1;
			t.tclosequick = 1;
		}
		if ( iHighlightingButton == 3 ) 
		{
			g_welcomeserialcode.iAfterError = 1;
		}
		if (g.bCleverbooksBundleMode == true)
		{
			// select username or password field
			if (iHighlightingButton == 4) g.cleverbooksmodeentryindex = 0;
			if (iHighlightingButton == 5) g.cleverbooksmodeentryindex = 1;
		}
	}
}

// WHAT YOU GET PAGE

struct welcomewhatyougettype
{
	int iNextButtonID;
	char pLines[10][1024];
};
welcomewhatyougettype g_welcomewhatyouget;

void welcome_whatyouget_init ( void )
{
	memset ( &g_welcomewhatyouget, 0, sizeof(g_welcomewhatyouget) );
	g_welcomewhatyouget.iNextButtonID = 1;

	// load text for product
	cstr productIntroPath = cstr("languagebank\\")+g.language_s+cstr("\\artwork\\welcome-assets\\product-intro.txt");
	OpenToRead ( 1, productIntroPath.Get() );
	for ( int iLine = 0; iLine < 10; iLine++ )
	{
		LPSTR pLineData = ReadString ( 1 );
		strcpy ( g_welcomewhatyouget.pLines[iLine], pLineData );
	}
	CloseFile ( 1 );
}

void welcome_whatyouget_page ( int iHighlightingButton )
{
	// draw page
	int iID = 0;
	welcome_drawrotatedimage ( g.editorimagesoffset+9, 50, 5, 0, 0, 0, false );
	welcome_drawbox ( 0, 10, 23, 90, 81 );
	for ( int iLine = 0; iLine < 10; iLine++ )
	{
		welcome_text ( g_welcomewhatyouget.pLines[iLine], 1, 50, 28+(iLine*5), 192, true, false );
	}
	iID = g_welcomewhatyouget.iNextButtonID; welcome_textinbox ( iID, "NEXT", 1, 50, 90, g_welcomebutton[iID].alpha );

	// control page
	if ( t.inputsys.mclick == 1 ) 
	{
		if ( iHighlightingButton == 1 ) t.tclosequick = 1;
	}
}

// MAIN PAGE

struct welcomemaintype
{
	int iTickToHide;
};
welcomemaintype g_welcomemain;

void welcome_main_init ( void )
{
	memset ( &g_welcomemain, 0, sizeof(g_welcomemain) );
	g_welcomemain.iTickToHide = 1;
}

void welcome_main_page ( int iHighlightingButton )
{
	// draw page
	int iID = 0;
	welcome_text ( "WELCOME TO GAMEGURU", 5, 50, 10, 255, false, false );
	welcome_text ( "Would you like to learn more about GameGuru, load a sample level to play\nor go directly to the editor and start creating?", 1, 50, 72, 192, true, false );
	iID = 4; welcome_drawbox ( iID, 10, 90, 11.5f, 92 );
	if ( g.gshowonstartup != 0 ) welcome_drawrotatedimage ( g.editorimagesoffset+40, 10.75f, 88.5f, 0, 0, 0, false );
	welcome_text ( "Click to skip welcome dialog in future", 1, 13.5f, 91.0f, 255, false, true );
	welcome_drawrotatedimage ( g.editorimagesoffset+56, 22.5f, 22, 0, 0, 0, false );
	welcome_drawrotatedimage ( g.editorimagesoffset+57, 50.0f, 22, 0, 0, 0, false );
	welcome_drawrotatedimage ( g.editorimagesoffset+58, 77.5f, 22, 0, 0, 0, false );
	iID = 1; welcome_textinbox ( iID, "USER GUIDE", 1, 22.5f, 60, g_welcomebutton[iID].alpha );
	iID = 2; welcome_textinbox ( iID, "PLAY", 1, 50, 60, g_welcomebutton[iID].alpha );
	iID = 3; welcome_textinbox ( iID, "CREATE", 1, 77.5f, 60, g_welcomebutton[iID].alpha );

	// control page
#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
	//PE: t.inputsys.mclickreleasestate is used in so many places so:
	if (t.inputsys.mclick == 0)
		bReadyForInput = true;
	if (t.inputsys.mclick == 1 && bReadyForInput)
#else
	if (t.inputsys.mclick == 1 && t.inputsys.mclickreleasestate == 0)
#endif
	{
		if ( iHighlightingButton == 1 ) 
		{
			// read the PDF manual
			cstr pPDFPath = g.fpscrootdir_s + "\\Files\\languagebank\\english\\artwork\\GameGuru - Getting Started Guide.pdf";
			ExecuteFile ( pPDFPath.Get(), "" , "", 0 );
			welcome_waitfornoinput();
		}
		if ( iHighlightingButton == 2 ) 
		{
			// jump into play page
			welcome_waitfornoinput();
			welcome_setuppage ( WELCOME_PLAY );
			welcome_runloop ( WELCOME_PLAY );
			welcome_waitfornoinput();
			welcome_setuppage ( WELCOME_MAIN );
			welcome_staticbackdrop();
			iHighlightingButton = 0;
			// stretch anim backdrop to size of client window
			Sprite ( 123, -100000, -100000, g.editorimagesoffset+12 );
			SizeSprite ( 123, GetChildWindowWidth(0)+1, GetChildWindowHeight(0)+11 );
			PasteSprite ( 123, 0, 0 );
		}
		if ( iHighlightingButton == 3 ) 
		{
			// go to editor in create mode
			t.tclosequick = 1;
		}
		if ( iHighlightingButton == 4 ) 
		{
#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
			bReadyForInput = false;
#endif
			// toggle startup flag
			set_inputsys_mclick(0);// t.inputsys.mclick = 0;
			t.inputsys.mclickreleasestate = 1;
			g.gshowonstartup = 1 - g.gshowonstartup;

			// save setting
			t.tfile_s = g.fpscrootdir_s+"\\showonstartup.ini";
			DeleteAFile ( t.tfile_s.Get() );
			if ( FileOpen(1) ==  1 ) CloseFile ( 1 );
			OpenToWrite ( 1, t.tfile_s.Get() );
			WriteString ( 1, cstr(g.gshowonstartup).Get() );
			WriteString ( 1, "10" );
			for ( int n = 0; n < 10; n++ )
			{
				WriteString ( 1, "0" );
			}
			CloseFile (  1 );
		}
	}
}

void welcome_mainvr_init ( void )
{
	memset ( &g_welcomemain, 0, sizeof(g_welcomemain) );
	g_welcomemain.iTickToHide = 1;
}

void welcome_mainvr_page ( int iHighlightingButton )
{
	// draw page
	int iID = 0;
	welcome_text ( "WELCOME TO GAMEGURU MAX", 5, 50, 10, 255, false, false );
	welcome_text ( "GameGuru MAX is still in development - this is an Alpha version", 1, 50, 18, 192, true, false );
	welcome_text ( "Look out for regular updates!", 1, 50, 23, 192, true, false );
	welcome_text ( "Would you like to learn more about GameGuru MAX\nor go directly to the editor and start creating?", 1, 50, 72, 192, true, false );
	iID = 4; welcome_drawbox ( iID, 10, 90, 11.5f, 92 );
	if ( g.gshowonstartup != 0 ) welcome_drawrotatedimage ( g.editorimagesoffset+40, 10.75f, 88.5f, 0, 0, 0, false );
	welcome_text ( "Untick to skip welcome dialog in future", 1, 13.5f, 91.0f, 255, false, true );
	welcome_drawrotatedimage ( g.editorimagesoffset+56, 37.5f, 30, 0, 0, 0, false );
	welcome_drawrotatedimage ( g.editorimagesoffset+58, 62.5f, 30, 0, 0, 0, false );
	iID = 1; welcome_textinbox ( iID, "USER GUIDE", 1, 37.5f, 60, g_welcomebutton[iID].alpha );
	iID = 3; welcome_textinbox ( iID, "CREATE", 1, 62.5f, 60, g_welcomebutton[iID].alpha );

	// control page
	#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
	if (t.inputsys.mclick == 0) bReadyForInput = true;
	if (t.inputsys.mclick == 1 && bReadyForInput)
	#else
	if ( t.inputsys.mclick == 1 && t.inputsys.mclickreleasestate == 0 ) 
	#endif
	{
		if ( iHighlightingButton == 1 ) 
		{
			// read the PDF manual
			cstr pPDFPath = g.fpscrootdir_s + "\\Files\\languagebank\\english\\artwork\\Getting Started Guide.pdf";
			ExecuteFile ( pPDFPath.Get(), "" , "", 0 );
			welcome_waitfornoinput();
		}
		if ( iHighlightingButton == 3 ) 
		{
			// go to editor in create mode
			t.tclosequick = 1;
		}
		if ( iHighlightingButton == 4 ) 
		{
			#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
			bReadyForInput = false;
			#endif
			// toggle startup flag
			set_inputsys_mclick(0);// t.inputsys.mclick = 0;
			t.inputsys.mclickreleasestate = 1;
			g.gshowonstartup = 1 - g.gshowonstartup;

			// save setting
			t.tfile_s = g.fpscrootdir_s+"\\showonstartup.ini";
			char pRealStartupFile[MAX_PATH];
			strcpy(pRealStartupFile, t.tfile_s.Get());
			GG_GetRealPath(pRealStartupFile, 1);
			t.tfile_s = pRealStartupFile;
			DeleteAFile ( t.tfile_s.Get() );
			if ( FileOpen(1) ==  1 ) CloseFile ( 1 );
			OpenToWrite ( 1, t.tfile_s.Get() );
			WriteString ( 1, cstr(g.gshowonstartup).Get() );
			CloseFile (  1 );
		}
	}
}

// PLAY PAGE

struct welcomeplaytype
{
	int iBackButtonID;
};
welcomeplaytype g_welcomeplay;

void welcome_play_init ( void )
{
	memset ( &g_welcomeplay, 0, sizeof(g_welcomeplay) );
	g_welcomeplay.iBackButtonID = 1;
}

int welcome_play_page ( int iHighlightingButton )
{
	// draw page
	int iID = 0;
	welcome_text ( "PLAY A SAMPLE LEVEL", 5, 50, 10, 255, false, false );
	welcome_drawrotatedimage ( g.editorimagesoffset+59, 50, 20, 0, 0, 0, false );
	welcome_text ( "THE BIG ESCAPE", 2, 50, 18, 255, false, false );
	iID = 2; welcome_textinbox ( iID, "LOAD", 1, 50, 60, g_welcomebutton[iID].alpha );
	welcome_text ( "This sample level has been created to demonstrate the type\nof game levels you can create using the stock assets that come with GameGuru.", 1, 50, 72, 192, true, false );
	iID = g_welcomeplay.iBackButtonID; welcome_textinbox ( iID, "BACK", 1, 50, 90, g_welcomebutton[iID].alpha );

	// control page
	if ( t.inputsys.mclick == 1 ) 
	{
		if ( iHighlightingButton == 1 ) 
		{
			// back
			t.tclosequick = 1;
		}
		if ( iHighlightingButton == 2 )
		{
			// load a game level
			t.tlevelautoload_s="The Big Escape.fpm";
			t.tlevelautoload_s=g.mysystem.mapbankAbs_s+t.tlevelautoload_s;

			// force immediate quit of welcome system
			return 1;
		}
	}

	// normal continue
	return 0;
}

void welcome_exitapp_init ( void )
{
}

void welcome_exitapp_page ( int iHighlightingButton )
{
	// draw page
	welcome_drawbox ( 0, 10, 23, 90, 81 );
	if ( g.iTriggerSoftwareToQuit == 1 )
	{
		welcome_text ( "Software Not Validated", 1, 50, 2+(2*5), 192, true, false );
		welcome_text ( "Use the FILE > Exit function or Close Button to exit app", 1, 50, 15+(15*5), 192, true, false );
	}
	if ( g.iTriggerSoftwareToQuit == 2 )
	{
		welcome_text ( "Steam not running or free weekend is over", 1, 50, 2+(2*5), 192, true, false );
		if ( g_welcomeCycle >=0 )
		{
			g_welcomeCycle++; if ( g_welcomeCycle > 50 ) g_welcomeCycle = 0;
		}
		if ( g_welcomeCycle > 25 )
			welcome_drawimage ( g.editorimagesoffset+60, 50, 30, false );
		else
			welcome_drawimage ( g.editorimagesoffset+63, 50, 30, false );
		welcome_text ( "Use the FILE > Exit function or Close Button to exit app", 1, 50, 15+(15*5), 192, true, false );
		welcome_textinbox ( 1, "GET YOUR VERY OWN VERSION HERE", 3, 50, 66, -1 );
	}
	if ( g.iTriggerSoftwareToQuit == 3 )
	{
		welcome_text ( "All DirectX 11 Shaders Updated", 1, 50, 28+(2*5), 192, true, false );
		welcome_text ( "You may change 'forceloadtestgameshaders' back to zero", 1, 50, 28+(3*5), 192, true, false );
		welcome_text ( "Use the FILE > Exit function or Close Button to exit app", 1, 50, 28+(6*5), 192, true, false );
	}
	if ( g.iTriggerSoftwareToQuit == 4 )
	{
		welcome_text ( "No Internet Connection", 1, 50, 2+(2*5), 192, true, false );
		welcome_text ( "You need a connection to confirm the software license", 1, 50, 28+(3*5), 192, true, false );
		welcome_text ( "Use the FILE > Exit function or Close Button to exit app", 1, 50, 15+(15*5), 192, true, false );
	}

	// control page
	if ( t.inputsys.mclick == 1 ) 
	{
		if ( iHighlightingButton == 1 && g_welcomeCycle >= 0 ) 
		{
			// go to Steam's GameGuru Page
			ExecuteFile ( "https://store.steampowered.com/app/266310/GameGuru/","","",0 );
			g_welcomeCycle = -1;
		}
	}
}

void welcome_freeintroapp_init ( void )
{
	g_welcomeCycle = 0;
}

void welcome_freeintroapp_page ( int iHighlightingButton )
{
	// draw page
	welcome_drawimage ( g.editorimagesoffset+62, 50, 30, false );
	welcome_text ( "Steam Free Weekend Version", 5, 50, 15, 192, true, false );
	welcome_textinbox ( 1, "CONTINUE CONTINUE", 3, 50, 69-10, -1 );
	welcome_textinbox ( 2, "GET YOUR OWN VERZ", 3, 50, 69, -1 );

	// control page
	if ( t.inputsys.mclick == 1 ) 
	{
		if ( iHighlightingButton == 1 ) 
		{
			t.tclosequick = 1;
		}
		if ( iHighlightingButton == 2 && g_welcomeCycle == 0 ) 
		{
			// go to Steam's GameGuru Page
			ExecuteFile ( "https://store.steampowered.com/app/266310/GameGuru/","","",0 );
			g_welcomeCycle = -1;
			t.tclosequick = 1;
		}
	}
}

void welcome_freetrialexitapp_init ( void )
{
}

void welcome_freetrialexitapp_page ( int iHighlightingButton )
{
	// draw page
	welcome_drawbox ( 0, 10, 23, 90, 81 );
	if ( g.iTriggerSoftwareToQuit == 2 )
	{
		welcome_text ( "Free Trial Has Expired", 1, 50, 2+(2*5), 192, true, false );
		if ( g_welcomeCycle >=0 )
		{
			g_welcomeCycle++; if ( g_welcomeCycle > 50 ) g_welcomeCycle = 0;
		}
		if ( g_welcomeCycle > 25 )
			welcome_drawimage ( g.editorimagesoffset+60, 50, 30, false );
		else
			welcome_drawimage ( g.editorimagesoffset+63, 50, 30, false );
		welcome_text ( "Use the FILE > Exit function or Close Button to exit app", 1, 50, 15+(15*5), 192, true, false );
		welcome_textinbox ( 1, "GET YOUR VERY OWN VERSION HERE", 3, 50, 66, -1 );
	}
	if ( g.iTriggerSoftwareToQuit == 3 )
	{
		welcome_text ( "All DirectX 11 Shaders Updated", 1, 50, 28+(2*5), 192, true, false );
		welcome_text ( "You may change 'forceloadtestgameshaders' back to zero", 1, 50, 28+(3*5), 192, true, false );
		welcome_text ( "Use the FILE > Exit function or Close Button to exit app", 1, 50, 28+(6*5), 192, true, false );
	}

	// control page
	if ( t.inputsys.mclick == 1 ) 
	{
		if ( iHighlightingButton == 1 && g_welcomeCycle >= 0 ) 
		{
			// go to GameGuru Page
			#ifdef STEAMTRIAL
			 ExecuteFile("http://bit.ly/336P3lo", "", "", 0);
			#else
			 ExecuteFile ( "http://bit.ly/2M6GfX8","","",0 );
			#endif
			g_welcomeCycle = -1;
		}
	}
}

void welcome_freetrialintroapp_init ( void )
{
	g_welcomeCycle = 0;
}

void welcome_freetrialintroapp_page ( int iHighlightingButton )
{
	// draw page
	welcome_drawimage ( g.editorimagesoffset+62, 50, 30, false );
	welcome_textinbox ( 1, "CONTINUE CONTINUE", 3, 50, 69-10, -1 );
	welcome_textinbox ( 2, "GET YOUR OWN VERZ", 3, 50, 69, -1 );

	// top prompt
	if (g.iFreeVersionModeActiveDuringEditor == 1)
	{
		// inside editor reminder
		char pShowReminder[2][1024];
		sprintf(pShowReminder[0], "Enjoying GameGuru? How about");
		sprintf(pShowReminder[1], "checking out this amazing discount?");
		welcome_text ( pShowReminder[0], 4, 50, 15-3, 192, true, false );
		welcome_text ( pShowReminder[1], 4, 50, 15+5, 192, true, false );
	}
	else
	{
		// days left
		char pShowDaysLeft[1024];
		if (g_trialStampDaysLeft == 1)
			sprintf(pShowDaysLeft, "Last Day of Free Trial", g_trialStampDaysLeft);
		else
			sprintf(pShowDaysLeft, "%d Days Left of Free Trial", g_trialStampDaysLeft);
		welcome_text ( pShowDaysLeft, 5, 50, 15, 192, true, false );
	}

	// expiry of discount code info
	char pDiscountNotice[1024];
	#ifdef STEAMTRIAL
	 sprintf ( pDiscountNotice, "* Best Discount Right Now On Steam" );
	#else
	 sprintf(pDiscountNotice, "* Discount Code Expires: %s", g_trialDiscountExpires);
	#endif
	welcome_text ( pDiscountNotice, 1, 50, 78, 192, true, false );

	// control page
	if ( t.inputsys.mclick == 1 ) 
	{
		if ( iHighlightingButton == 1 ) 
		{
			t.tclosequick = 1;
		}
		if (iHighlightingButton == 2 && g_welcomeCycle == 0)
		{
			// go to GameGuru Order Page with 50% discount
			char pGoToLinkString[1024];
			#ifdef STEAMTRIAL
			 // interrogate TGC server to find current promotion, then send the user to that promotion
			 welcome_findbestpromotion(pGoToLinkString);
			 CleanStringOfEscapeSlashes(pGoToLinkString);
			#else
			 sprintf(pGoToLinkString, "https://gameguru.thegamecreators.com/basket/add/17473/10723?discount_code=%s", g_trialDiscountCode);
			#endif
			ExecuteFile ( pGoToLinkString,"","",0 );
			g_welcomeCycle = -1;
			t.tclosequick = 1;
		}
	}
}

void CleanStringOfEscapeSlashes ( char* pText )
{
	char *str = pText;
	char *str2 = pText;
	while ( *str )
	{
		if ( *str == '"' )
		{
			str++;
		}
		else
		{
			if ( *str == '\\' )
			{
				str++;
				switch( *str )
				{
					case 'n': *str2 = '\n'; break;
					case 'r': *str2 = '\r'; break;
					case '"': *str2 = '"'; break;
					case 'b': *str2 = '\b'; break;
					case 'f': *str2 = '\f'; break;
					case 't': *str2 = '\t'; break;
					case '/': *str2 = '/'; break;
					case '\\': *str2 = '\\'; break;
					default: *str2 = *str;
				}
			}
			else
			{
				*str2 = *str;
			}
			str++;
			str2++;
		}
	}
	*str2 = 0;
}

UINT OpenURLForDataOrFile ( LPSTR pDataReturned, DWORD* pReturnDataSize, LPSTR pUniqueCode, LPSTR pVerb, LPSTR urlWhere, LPSTR pLocalFileForImageOrNews )
{
	UINT iError = 0;
	unsigned int dwDataLength = 0;
	HINTERNET m_hInet = InternetOpenA( "InternetConnection", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if ( m_hInet == NULL )
	{
		iError = GetLastError( );
	}
	else
	{
		unsigned short wHTTPType = INTERNET_DEFAULT_HTTPS_PORT;
		HINTERNET m_hInetConnect = InternetConnectA( m_hInet, "www.thegamecreators.com", wHTTPType, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0 );
		if ( m_hInetConnect == NULL )
		{
			iError = GetLastError( );
		}
		else
		{
			int m_iTimeout = 2000;
			InternetSetOption( m_hInetConnect, INTERNET_OPTION_CONNECT_TIMEOUT, (void*)&m_iTimeout, sizeof(m_iTimeout) );  
			HINTERNET hHttpRequest = HttpOpenRequestA( m_hInetConnect, pVerb, urlWhere, "HTTP/1.1", NULL, NULL, INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE, 0 );
			if ( hHttpRequest == NULL )
			{
				iError = GetLastError( );
			}
			else
			{
				HttpAddRequestHeadersA( hHttpRequest, "Content-Type: application/x-www-form-urlencoded", -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE );
				int bSendResult = 0;
				FILE* fpImageLocalFile = NULL;
				if ( pLocalFileForImageOrNews == NULL )
				{
					// News
					char m_szPostData[1024];
					strcpy ( m_szPostData, "k=vIo3sc2z" );
				     strcat ( m_szPostData, "&app=gamegurumax" );
				    extern int g_iSpecialIDEForViewingTestAnnouncements;
					if ( g_iSpecialIDEForViewingTestAnnouncements == 2 )
					{
						strcat(m_szPostData, "&test=true");
					}
					strcat ( m_szPostData, "&uid=" );
					strcat ( m_szPostData, pUniqueCode );
					bSendResult = HttpSendRequest( hHttpRequest, NULL, -1, (void*)(m_szPostData), strlen(m_szPostData) );
				}
				else
				{
					// Image URL, open local file for writing
					bSendResult = HttpSendRequest( hHttpRequest, NULL, -1, NULL, NULL );
					fpImageLocalFile = GG_fopen( pLocalFileForImageOrNews , "wb" );
				}
				if ( bSendResult == 0 )
				{
					iError = GetLastError( );
				}
				else
				{
					int m_iStatusCode = 0;
					char m_szContentType[150];
					unsigned int dwBufferSize = sizeof(int);
					unsigned int dwHeaderIndex = 0;
					HttpQueryInfo( hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, (void*)&m_iStatusCode, (LPDWORD)&dwBufferSize, (LPDWORD)&dwHeaderIndex );
					dwHeaderIndex = 0;
					unsigned int dwContentLength = 0;
					HttpQueryInfo( hHttpRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (void*)&dwContentLength, (LPDWORD)&dwBufferSize, (LPDWORD)&dwHeaderIndex );
					dwHeaderIndex = 0;
					unsigned int ContentTypeLength = 150;
					HttpQueryInfo( hHttpRequest, HTTP_QUERY_CONTENT_TYPE, (void*)m_szContentType, (LPDWORD)&ContentTypeLength, (LPDWORD)&dwHeaderIndex );
					char pBuffer[ 20000 ];
					for(;;)
					{
						unsigned int written = 0;
						if( !InternetReadFile( hHttpRequest, (void*) pBuffer, 2000, (LPDWORD)&written ) )
						{
							// error
						}
						if ( written == 0 ) break;
						if ( fpImageLocalFile )
						{
							// write direct to local image file
							fwrite(pBuffer, 1, written, fpImageLocalFile );
						}
						else
						{
							// comple news for return string
							if ( dwDataLength + written > 10240 ) written = 10240 - dwDataLength;
							memcpy( pDataReturned + dwDataLength, pBuffer, written );
							dwDataLength = dwDataLength + written;
							if ( dwDataLength >= 10240 ) break;
						}
					}
					InternetCloseHandle( hHttpRequest );
				}
				if ( fpImageLocalFile )
				{
					fclose(fpImageLocalFile);
					fpImageLocalFile = NULL;
				}
			}
			InternetCloseHandle( m_hInetConnect );
		}
		InternetCloseHandle( m_hInet );
	}
	if ( iError > 0 )
	{
		char *szError = 0;
		if ( iError > 12000 && iError < 12174 ) 
			FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE, GetModuleHandleA("wininet.dll"), iError, 0, (char*)&szError, 0, 0 );
		else 
			FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, iError, 0, (char*)&szError, 0, 0 );
		if ( szError )
		{
			LocalFree( szError );
		}
	}

	// complete
	*pReturnDataSize = dwDataLength;
	return iError;
}

bool g_bRunningForFirstTime = false;
int g_iSpecialIDEForViewingTestAnnouncements = 0;

#define ONLYSHOWCHANGELOGONETIME
bool welcome_get_change_log(void)
{
	FILE *file = NULL;
	// copied from above
	char pNewsText[10240];
	strcpy(pNewsText, "");
	char pURLText[10240];
	strcpy(pURLText, "");

	// also show announcement each time a build changes
	char pChangeLogFile[1024];
	strcpy(pChangeLogFile, g.fpscrootdir_s.Get());
	strcat(pChangeLogFile, "\\changelog.txt");
	if (FileExist(pChangeLogFile) == 1)
	{
		extern preferences pref;
		struct stat sb;
		time_t ftime;
		if (stat(pChangeLogFile, &sb) == 0) {
			ftime = sb.st_mtime;
		}

		bool bFirstTime = true;
		extern bool bAddWhatNewToMenu;
		bAddWhatNewToMenu = true; //PE: Can always trigger from menu.
		#ifndef ONLYSHOWCHANGELOGONETIME
		bFirstTime = false;
		#else
		static bool bOnFirstCall = true;
		if (bOnFirstCall && ftime != pref.changelog_ftime)
		{
			bFirstTime = false;
		}
		if (!bOnFirstCall) bFirstTime = false; //PE: Second call always open, called from menu.
		bOnFirstCall = false;
		#endif

		if (!bFirstTime)
		{
			pref.changelog_ftime = ftime;

			memset(pNewsText, 0, sizeof(pNewsText));
			file = GG_fopen(pChangeLogFile, "rb");
			if (file != NULL)
			{
				fread(pNewsText, 1, 9999, file);
				fclose(file);
			}
			LPSTR pSrc = pNewsText;
			LPSTR pDst = pNewsText;
			for (LPSTR pSrc = pNewsText; pSrc <= pNewsText + strlen(pNewsText); pSrc++)
			{
				if (*pSrc != '\n' && *pSrc != '\r') { *pDst = *pSrc; pDst++; }
			}
			strcpy(pURLText, "https://www.game-guru.com");
			if (g_iSpecialIDEForViewingTestAnnouncements == 0)
			{
				// only public build users get set to 1 (internal testers already have this set to 2)
				g_iSpecialIDEForViewingTestAnnouncements = 1;
			}
			strcpy(g_welcomeText, pNewsText);
			strcpy(g_welcomeLinkUrl, pURLText);
			strcpy(g_welcomeImageUrl, "");

			// overwrite default with announcement image
			image_setlegacyimageloading(true);
			LoadImage(g_welcomeImageUrl, g.editorimagesoffset + 64);
			if (ImageExist(g.editorimagesoffset + 64) == 0)
			{
				// sometimes the image is download corrupt!
				strcpy(g_welcomeImageUrl, g.fpscrootdir_s.Get());
				strcat(g_welcomeImageUrl, "\\Files\\languagebank\\english\\artwork\\welcome-assets\\gameguru-news-banner.jpg");
				LoadImage(g_welcomeImageUrl, g.editorimagesoffset + 64);
			}
			image_setlegacyimageloading(false);
			g_welcomeCycle = 0; if (strlen(g_welcomeLinkUrl) > 0) g_welcomeCycle = 1;

			return true;
		}
	}
	return false;
}

bool welcome_announcements_init ( void )
{
	// grab news from server
	bool bNewsIsAvailable = false;
	strcpy ( g_welcomeText, "" );
	strcpy ( g_welcomeImageUrl, "" );
	strcpy ( g_welcomeLinkUrl, "" );

	// generate unique code for AGK install if none available
	char pUniqueCodeFile[ 1024 ];
	strcpy( pUniqueCodeFile, g.fpscrootdir_s.Get() );
	strcat( pUniqueCodeFile, "\\installcode.dat" );
	char pUniqueCode[33];
	memset ( pUniqueCode, 33, 0 );
	FILE *file = GG_fopen(pUniqueCodeFile, "r");
	if ( file == NULL )
	{
		// generate
		time_t mtime;
		mtime = time(0);
		srand(mtime);
		int n = 0;
		for (; n < 32; n++ ) 
		{
			pUniqueCode[n] = 65+(rand()%22);
		}
		pUniqueCode[32] = 0;
		FILE* fp = GG_fopen( pUniqueCodeFile , "w" );
		fwrite(pUniqueCode , 1 , 32 , fp );
		fclose(fp);

		// if generate for first time, must be first time run
		g_bRunningForFirstTime = true;
	}
	else
	{
		// read
		fread(pUniqueCode, 1, 32, file);
		fclose(file);
	}
	pUniqueCode[32] = 0;

	// when the new auto updater is used, it can log in, and if
	// that user is a test user, it creates SHOWTEXT.dat so
	// the following code can be in test mode automagically

	// are we a special IDE?
	char pSpecialIDETestFile[2048];
	strcpy ( pSpecialIDETestFile, g.fpscrootdir_s.Get() );
	strcat ( pSpecialIDETestFile, "\\SHOWINTERNAL.dat" );
	FILE* showtestfile = GG_fopen(pSpecialIDETestFile, "r");
	if ( showtestfile != NULL )
	{
		// special mode 2 for ONLY THOSE WITH "SHOWINTERNAL" created by AUTOUPDATER which knows about internal testers
		g_iSpecialIDEForViewingTestAnnouncements = 2;
		fclose(showtestfile);
	}				

	#define ALWAYSSHOWANNOUNCEMENTIFBUILD

	// false will skip the dialog altogether
	return false;// bNewsIsAvailable;
}

void welcome_announcements_steamreview_page(int iHighlightingButton)
{
	// draw page
	int iID = 0;
	welcome_text("Considered Writing A Review?", 5, 50, 8, 192, true, false);
	if (ImageExist(g.editorimagesoffset + 64) == 1)
	{
		welcome_drawrotatedimage(g.editorimagesoffset + 64, 50, 18, 0, 0, 0, false);
	}
	welcome_text("If you are enjoying GameGuru so far, it would be great if you could", 1, 50, 56, 192, true, false);
	welcome_text("write a Steam review and let others know about your experiences.", 1, 50, 61, 192, true, false);
	iID = 1; welcome_textinbox(iID, "MAKE GAMES", 1, 100 - 35, 85, g_welcomebutton[iID].alpha);
	iID = 2; welcome_textinbox(iID, "GOTO STEAM", 1, 35, 85, g_welcomebutton[iID].alpha);

	// control page
	if (t.inputsys.mclick == 1 && t.inputsys.mclickreleasestate == 0)
	{
		if (iHighlightingButton == 1)
		{
			t.tclosequick = 1;
		}
		if (iHighlightingButton == 2)
		{
			// jump to announcement link if there is one
			ExecuteFile("https://steamcommunity.com/app/266310/reviews/", "", "", 0);
			t.tclosequick = 1;
		}
	}
}

void welcome_announcements_page(int iHighlightingButton)
{
	// announcement system can also offer user option to update
	extern bool g_bOfferLatestUpdate;
	if (g_bOfferLatestUpdate==true)
	{
		PasteImage(g.editorimagesoffset + 8, g_welcome.iTopLeftX, g_welcome.iTopLeftY+50);
	}

	// draw page
	int iID = 0;
	if (ImageExist(g.editorimagesoffset + 64) == 1)
	{
		welcome_drawrotatedimage(g.editorimagesoffset + 64, 50, 8, 0, 0, 0, false);
	}

	// regular announcement
	int iCharCount = 0;
	int iLineIndex = 1;
	char pTextToShow[1024];
	strcpy(pTextToShow, g_welcomeText);
	bool bBulletPointMode = false;
	int iTextWidthInCharacters = 65;
	if (strnicmp(pTextToShow, "GameGuru MAX Build", 18) == NULL)
	{
		bBulletPointMode = true;
		iTextWidthInCharacters = 110;
	}
	char pLine[12][1024];
	for (int n = 0; n < 12; n++) strcpy(pLine[n], "");
	char pWorkLine[1024]; strcpy(pWorkLine, "");
	bool bDoingBulletPoints = false;
	for (int n = 0; n < strlen(pTextToShow); n++)
	{
		iCharCount++;
		bool bBulletPointHeader = false;
		bool bBulletPointFooter = false;
		if (bBulletPointMode == true)
		{
			if (pTextToShow[n] == ':') bBulletPointHeader = true;
			if (pTextToShow[n] == '.' && pTextToShow[n + 1] == '.') bBulletPointFooter = true;
		}
		if (iCharCount > iTextWidthInCharacters && (pTextToShow[n] == ' ' || pTextToShow[n] == ',' || pTextToShow[n] == '.' || pTextToShow[n] == '"')
			|| (bBulletPointMode == true && ((pTextToShow[n] == '.' && pTextToShow[n + 1] == ' ') || bBulletPointHeader == true || bBulletPointFooter == true)))
		{
			if (bDoingBulletPoints == true)
			{
				strcpy(pLine[iLineIndex - 1], "* ");
				strcat(pLine[iLineIndex - 1], pTextToShow);
				pLine[iLineIndex - 1][n + 1 + 1] = 0;// 2] = 0;
			}
			else
			{
				strcpy(pLine[iLineIndex - 1], pTextToShow);
				pLine[iLineIndex - 1][n + 1] = 0;
			}
			strcpy(pWorkLine, pTextToShow);
			int iChopExtra = 0; if (bBulletPointFooter == true) iChopExtra = 1;
			strcpy(pTextToShow, pWorkLine + n + iChopExtra + 1); n = 0;
			iCharCount = 0;
			iLineIndex++;
			if (bBulletPointHeader == true) { iLineIndex++; bDoingBulletPoints = true; }
			if (bBulletPointFooter == true) { iLineIndex++; bDoingBulletPoints = false; }
			if (iLineIndex == 12) break;
		}
	}
	if (iLineIndex > 1) strcpy(pLine[iLineIndex - 1], pTextToShow);
	// two sizes, large and small
	int iChosenTextSize = 1;
	float fStartY = 50.0f;
	float fSpacing = 6.0f;
	if (iLineIndex > 5 || bBulletPointMode == true)
	{
		fStartY = 46.0f;
		iChosenTextSize = 8;
		fSpacing = 3.0f;
	}
	for (int n = 0; n < 12; n++)
	{
		welcome_text(pLine[n], iChosenTextSize, 50, fStartY + (n*fSpacing), 192, true, false);
	}

	// regular CONTIUNUE(LINK) or UPDATE OFFER
	if (g_bOfferLatestUpdate == true)
	{
		// offer latest update
		if (g_iSpecialIDEForViewingTestAnnouncements == 2 )
			welcome_text("As an internal tester, do you want to check for changes to this version?", 1, 50, 88, 192, true, false);
		else
			welcome_text("Do you want to update to the latest version?", 1, 50, 88, 192, true, false);

		iID = 1; welcome_textinbox(iID, "NO", 1, 55, 97, g_welcomebutton[iID].alpha);
		iID = 2; welcome_textinbox(iID, "YES", 1, 100 - 55, 97, g_welcomebutton[iID].alpha);
		if (t.inputsys.mclick == 1 && t.inputsys.mclickreleasestate == 0)
		{
			if (iHighlightingButton == 1)
			{
				t.tclosequick = 1;
			}
			if (iHighlightingButton == 2)
			{
				// launch auto updater
				ExecuteFile("..\\..\\GameGuru MAX Updater.exe", "", "", 0);
				// and quit software so autp updater can update all files (unlocked)
				extern bool g_bCascadeQuitFlag;
				g_bCascadeQuitFlag = true;
				t.tclosequick = 1;
			}
		}
	}
	else
	{
		if (g_welcomeCycle == 0 || bBulletPointMode == true)
		{
			iID = 1; welcome_textinbox(iID, "CONTINUE", 1, 50, 90, g_welcomebutton[iID].alpha);
		}
		else
		{
			iID = 1; welcome_textinbox(iID, "CONTINUE", 1, 100 - 35, 90, g_welcomebutton[iID].alpha);
			iID = 2; welcome_textinbox(iID, "GOTO LINK", 1, 35, 90, g_welcomebutton[iID].alpha);
		}

		// control page
		//PE: Take input directly so cant be blocked.
		if (ImGui::IsMouseDown(0) && t.inputsys.mclickreleasestate == 0)
		{
			if (iHighlightingButton == 1)
			{
				t.tclosequick = 1;
			}
			if (iHighlightingButton == 2)
			{
				// jump to announcement link if there is one
				ExecuteFile(g_welcomeLinkUrl, "", "", 0);
				t.tclosequick = 1;
			}
		}
	}
}

void welcome_savestandalone_init ( void )
{
	g_welcomeCycle = 0;
}

void welcome_savestandalone_page ( int iHighlightingButton )
{
	// draw page
	int iID = 0;
	welcome_text ( "Save Standalone", 5, 50, 15, 192, true, false );
	if ( g_welcomeCycle == 0 )
	{
		welcome_text ( "Choose where you would like your standalone to be saved", 1, 50, 30, 192, true, false );
		welcome_drawbox ( 0, 10, 37, 74, 43 );
		welcome_text ( g.exedir_s.Get(), 1, 42, 40, 192, true, false );
		iID = 3; welcome_textinbox ( iID, "CHANGE", 1, 85, 40, g_welcomebutton[iID].alpha );
		// save standalone button
		iID = 2; welcome_textinbox ( iID, "SAVE STANDALONE", 1, 50, 65, g_welcomebutton[iID].alpha );
		iID = 1; welcome_textinbox ( iID, "EXIT", 1, 50, 85, g_welcomebutton[iID].alpha );
	}
	else
	{
		welcome_text ( "Standalone Folder", 1, 50, 30, 192, true, false );
		welcome_drawbox ( 0, 10, 37, 90, 43 );
		welcome_text ( g.exedir_s.Get(), 1, 50, 40, 192, true, false );
		// show standalone progress
		welcome_text ( "Exporting your game as a standalone executable and encrypting media files", 1, 50, 60, 192, true, false );
		welcome_drawbox ( 0, 10, 65, 90, 70 );
		int iPercentageComplete = (int)mapfile_savestandalone_getprogress();
		char pProgressString[1024];
		char percentsymbol[2];
		percentsymbol[0] = 37;
		percentsymbol[1] = 0;
		sprintf ( pProgressString, "Progress Complete : %d%s", iPercentageComplete, percentsymbol );
		welcome_text ( pProgressString, 1, 50, 67.5f, 192, true, false );
		if ( iPercentageComplete > 0 && iPercentageComplete < 100 )
		{
			// only offer CANCEL button if in progress
			iID = 1; welcome_textinbox ( iID, "CANCEL", 1, 50, 85, g_welcomebutton[iID].alpha );
		}
	}

	// control standalone saving
	if ( g_welcomeCycle == 2 )
	{
		// start save standalone creation
		mapfile_savestandalone_start();
		g_welcomeCycle = 3;
	}
	if ( g_welcomeCycle == 3 )
	{
		// run standalone creation calls
		if ( mapfile_savestandalone_continue() == 1 )
		{
			// complete standalone creation
			g_welcomeCycle = 4;
		}
	}
	if ( g_welcomeCycle == 4 )
	{
		// complete standalone creation
		mapfile_savestandalone_finish();
		g_welcomeCycle = 0;
		t.tclosequick = 1;
	}
	if ( g_welcomeCycle == 5 )
	{
		// cancel standalone creation
		mapfile_savestandalone_restoreandclose();
		g_welcomeCycle = 0;
		t.tclosequick = 1;
	}
	if ( g_welcomeCycle == 1 ) g_welcomeCycle = 2;

	// control page
	if ( t.inputsys.mclick == 1 ) 
	{
		if ( iHighlightingButton == 1 ) 
		{
			if ( g_welcomeCycle == 0 )
			{
				// exit
				t.tclosequick = 1;
			}
			else
			{
				// force cancel stage
				if ( g_welcomeCycle == 3 ) g_welcomeCycle = 5;
			}
		}
		if ( iHighlightingButton == 2 ) 
		{
			// save standalone start
			g_welcomeCycle = 1;
		}
		if ( iHighlightingButton == 3 ) 
		{
			// change location of save standalone game
			OpenFileMap ( 1,"FPSEXCHANGE" );
			SetFileMapString ( 1, 1000, g.exedir_s.Get() );
			SetFileMapDWORD ( 1, 424, 2 );
			SetEventAndWait ( 1 );
			while ( GetFileMapDWORD(1, 424) == 2 ) 
			{
				SetEventAndWait ( 1 );
			}
			t.returnstring_s = GetFileMapString(1, 1000);
			if ( strlen(t.returnstring_s.Get()) > 0 )
				g.exedir_s = t.returnstring_s;
		}
	}
}

// WORKER FUNCTIONS

bool welcome_setuppage ( int iPageIndex )
{
	// clear data vars
	memset ( g_welcomebutton, 0, sizeof(g_welcomebutton) );
	g_welcomebuttoncount = 0;

	// clear page specific globals
	if ( iPageIndex == WELCOME_SERIALCODE ) welcome_serialcode_init();
	if ( iPageIndex == WELCOME_WHATYOUGET ) welcome_whatyouget_init();
	//if ( iPageIndex == WELCOME_CHANGELOG ) welcome_changelog_init();
	if ( iPageIndex == WELCOME_MAIN ) welcome_main_init();
	if ( iPageIndex == WELCOME_MAINVR ) welcome_mainvr_init();
	if ( iPageIndex == WELCOME_PLAY ) welcome_play_init();
	if ( iPageIndex == WELCOME_EXITAPP ) welcome_exitapp_init();
	if ( iPageIndex == WELCOME_FREEINTROAPP ) welcome_freeintroapp_init();
	if ( iPageIndex == WELCOME_ANNOUNCEMENTS ) return welcome_announcements_init();
	if ( iPageIndex == WELCOME_SAVESTANDALONE ) welcome_savestandalone_init();
	if ( iPageIndex == WELCOME_FREETRIALINTROAPP ) welcome_freetrialintroapp_init();
	if ( iPageIndex == WELCOME_FREETRIALEXITAPP ) welcome_freetrialexitapp_init();

	// normally success
	return true;
}

void welcome_runloop ( int iPageIndex )
{
	// code moved to welcome_cycle
	g_iWelcomeLoopPage = iPageIndex;

	// run loop when in welcome page
	t.tclicked = 0; t.tclosequick = 0;
	t.lastmousex = MouseX(); t.lastmousey = MouseY();
	t.inputsys.mclickreleasestate = 0;
}

bool welcome_cycle(void)
{
	// carry page running from store
	int iPageIndex = g_iWelcomeLoopPage;
	if (iPageIndex == 0)
	{
		// if no page in effect, end looping
		t.tclosequick = 0;
		return true;
	}

	//Add a modal window so cant click anything else then this screen:
	ImGui::OpenPopup("##HiddenModalWindow");
	ImGui::SetNextWindowPos(ImVec2(-100.0, 1) + ImGui::GetMainViewport()->Pos, ImGuiCond_Always);
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowViewport(viewport->ID);
	bool bHiddenActive = true;
	if (ImGui::BeginPopupModal("##HiddenModalWindow", &bHiddenActive, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::Text("");
		ImGui::EndPopup();
	}

	// quit state
	int iQuitState = 0;
	
	// exit conditions
	if ( iPageIndex == WELCOME_EXITAPP ||  iPageIndex == WELCOME_FREETRIALEXITAPP )
	{
		// can never leave this loop!
	}
	else
	{
		if ( EscapeKey() == 0 && t.tclosequick == 0 && g_welcomesystemclosedown == 0 )
		{
			// normal running
		}
		else
		{
			// leave loop, which was escaped or triggered
			iQuitState = 1;// bStayInsideLoop = false;

			extern bool bBlockImGuiUntilNewFrame;
			extern bool bImGuiRenderWithNoCustomTextures;
			bBlockImGuiUntilNewFrame = true;
			bImGuiRenderWithNoCustomTextures = true; //PE: We still need to use AddRectFilled, but cant have any custom textures or we crash. without we can see 3D from wicked for one frame.
		}
	}
	
	if ( 1 )
	{
		if ( t.inputsys.mclick == 0 ) t.inputsys.mclickreleasestate = 0;
		if ( t.inputsys.ignoreeditorintermination == 0 )
		{
			if ( GetFileMapDWORD( 1, 908 ) == 1 )  iQuitState=1;
		}
		if ( iPageIndex != WELCOME_EXITAPP )
		{
			if ( GetFileMapDWORD( 1, 516 ) > 0 )  iQuitState = 1;
			if ( GetFileMapDWORD( 1, 400 ) == 1 ) { t.interactive.active = 0  ; iQuitState = 1; }
			if ( GetFileMapDWORD( 1, 404 ) == 1 ) { t.interactive.active = 0  ; iQuitState = 1; }
			if ( GetFileMapDWORD( 1, 408 ) == 1 ) { t.interactive.active = 0  ; iQuitState = 1; }
			if ( GetFileMapDWORD( 1, 434 ) == 1 ) { t.interactive.active = 0  ; iQuitState = 1; }
			if ( GetFileMapDWORD( 1, 758 ) != 0 ) { t.interactive.active = 0  ; iQuitState = 1; }
			if ( GetFileMapDWORD( 1, 762 ) != 0 ) { t.interactive.active = 0  ; iQuitState = 1; }
		}

		// paste page panel
		if (ImageExist(g.editorimagesoffset + 8) == 1)
		{
			PasteImage (g.editorimagesoffset + 8, g_welcome.iTopLeftX, g_welcome.iTopLeftY);
		}

		// get mouse coordinate for control
#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
		//PE: Get input directly so other windows cant block button clicks.
		float tinputsystemXmouse = ((float)GetChildWindowWidth(-1) / (float)GetDisplayWidth()) * ImGui::GetMousePos().x;
		float tinputsystemYmouse = ((float)GetChildWindowHeight(-1) / (float)GetDisplayHeight()) * ImGui::GetMousePos().y;
#else
		float tinputsystemXmouse = t.inputsys.xmouse + 0.0;
		float tinputsystemYmouse = t.inputsys.ymouse - 30.0;
#endif

		// highlight hover over a button
		int iHighlightingButton = -1;
		for ( int iButton = 1; iButton <= g_welcomebuttoncount; iButton++ )
		{
			g_welcomebutton[iButton].alpha = 128;
			if (tinputsystemXmouse >= g_welcomebutton[iButton].x1 && tinputsystemXmouse <= g_welcomebutton[iButton].x2)
			{
				if (tinputsystemYmouse >= g_welcomebutton[iButton].y1 && tinputsystemYmouse <= g_welcomebutton[iButton].y2)
				{
					iHighlightingButton = iButton;
					g_welcomebutton[iHighlightingButton].alpha = 255;
				}
			}
		}

		// display correct page
		if ( iPageIndex == WELCOME_SERIALCODE ) welcome_serialcode_page ( iHighlightingButton );
		if ( iPageIndex == WELCOME_WHATYOUGET ) welcome_whatyouget_page ( iHighlightingButton );
		if ( iPageIndex == WELCOME_MAIN ) welcome_main_page ( iHighlightingButton );
		if ( iPageIndex == WELCOME_MAINVR ) welcome_mainvr_page ( iHighlightingButton );
		if ( iPageIndex == WELCOME_PLAY ) g_welcomesystemclosedown = welcome_play_page ( iHighlightingButton );
		if ( iPageIndex == WELCOME_EXITAPP ) welcome_exitapp_page ( iHighlightingButton );
		if ( iPageIndex == WELCOME_FREEINTROAPP ) welcome_freeintroapp_page ( iHighlightingButton );
		if ( iPageIndex == WELCOME_ANNOUNCEMENTS ) welcome_announcements_page ( iHighlightingButton );
		if ( iPageIndex == WELCOME_SAVESTANDALONE ) welcome_savestandalone_page ( iHighlightingButton );
		if ( iPageIndex == WELCOME_FREETRIALINTROAPP ) welcome_freetrialintroapp_page ( iHighlightingButton );
		if ( iPageIndex == WELCOME_FREETRIALEXITAPP ) welcome_freetrialexitapp_page ( iHighlightingButton );
	}

	// quit state symbolises the end of the 'internal loop'
	if (iQuitState == 1)
	{
		// when leave a page, ensure we are allowed to leave!
		if (g.iTriggerSoftwareToQuit != 0)
		{
			// can now instantly leave!
			//welcome_show(WELCOME_EXITAPP);
			PostQuitMessage(0);
		}
		else
		{
			t.tclosequick = 0;
			return true;
		}
	}

	// continue internal loop
	return false;
}

void welcome_show ( int iPageIndex )
{
	if ( welcome_setuppage ( iPageIndex ) == true )
	{
		welcome_runloop ( iPageIndex );
	}
}


//PE: added download store here , can be moved later.
int iDownloadStoreProgress = 0;
bool bDownloadStoreError = false;
char cDownloadStoreError[8192];
char cDownloadStoreSessionToken[256];
char cDownloadStoreLoginUrl[256];
char cDownloadStoreUserId[256];
char cDownloadStoreUserHash[256];

extern bool bDownloadStore_Window;
#define MAXSTOREDATESIZE 131072
char pDataReturned[MAXSTOREDATESIZE+8];
char pDatatmp[MAXSTOREDATESIZE + 8];
DWORD dwDataReturnedSize = 0;
float fCheckForLoginTimer = 0;
int iCheckForLoginCount = 0;
bool bLoginButtonClicked = false;
char pDownloadFile[1024];
LPSTR pDownloadStoreData = NULL;
DWORD dwDownloadStoreDataSize = 0;
LPSTR pDownloadStoreChecksumFile = NULL;
struct StoreItems
{
	int id;
	cStr name;
	cStr url;
	cStr data;
	cStr checksum;
};
typedef std::map<int, StoreItems *>   mDownList;
typedef mDownList::iterator	mitDownList;
mDownList download_list;
int files_updated = 0, total_files = 0, files_downloaded=0;
int real_files_updated = 0, real_total_files = 0, real_files_downloaded = 0;
bool bPrintFirstEntry = true;
char StoreWriteFolder[MAX_PATH];
char StoreDocWriteFolder[MAX_PATH];
char StoreAppWriteFolder[MAX_PATH];
int iDownloadLocation = 0;

#include "Common-Keys.h"

#if __has_include("D:\\DEV\\secret-token.h")
#include "D:\\DEV\\secret-token.h"
#else
#define SECRET_TOKEN "none"
#endif


void imgui_download_store( void )
{
	if (!bDownloadStore_Window) return;

	char secret_token[MAX_PATH];
	strcpy(secret_token, SECRET_TOKEN);

	if (iDownloadStoreProgress == 0)
	{
		if (pDownloadStoreData) 
		{
			delete[] pDownloadStoreData;
			pDownloadStoreData = NULL;
			dwDownloadStoreDataSize = 0;
		}
		if (pDownloadStoreChecksumFile) 
		{
			delete[] pDownloadStoreChecksumFile;
			pDownloadStoreChecksumFile = NULL;
		}
		total_files = 0;
		files_updated = 0;
		files_downloaded = 0;

		real_total_files = 0;
		real_files_updated = 0;
		real_files_downloaded = 0;
		bPrintFirstEntry = true;

		//PE: Get Login information
		strcpy(cDownloadStoreSessionToken, "");
		strcpy(cDownloadStoreLoginUrl, "");
		memset(pDataReturned, 0, sizeof(pDataReturned));
		dwDataReturnedSize = 0;
		char cUrl[256];
		sprintf(cUrl, "/api/auth/session/create");

		UINT iError = StoreOpenURLForDataOrFile(NULL, pDataReturned, &dwDataReturnedSize, "", "GET", cUrl, NULL);
		if (iError <= 0 && *pDataReturned != 0 && strchr(pDataReturned, '{') != 0)
		{
			// break up response string
			char* pChop;
			strcpy(pDatatmp, "");
			if (( pChop = (char *) pestrcasestr(pDataReturned, "\"success\":true")) != NULL)
			{
				// success
				pChop += strlen("\"success\":true");
				char pSearchForToken[256];
				strcpy(pSearchForToken, "\"login_url\"\0");
				pChop = strstr(pChop, pSearchForToken);
				if (pChop) 
				{
					pChop += strlen(pSearchForToken)+2;
					strcpy(pDatatmp, pChop);
					char* pFindEnd = strstr(pDatatmp, "\"\0");
					if (pFindEnd)
					{
						pDatatmp[pFindEnd - pDatatmp] = 0;
						if (strlen(pDatatmp) < 256) strcpy(cDownloadStoreLoginUrl, pDatatmp);
						pChop = pFindEnd + 1;
					}
				}
				strcpy(pSearchForToken, "\"session_token\"\0");
				pChop = strstr(pChop, pSearchForToken);
				if (pChop) 
				{
					pChop += strlen(pSearchForToken) + 2;
					strcpy(pDatatmp, pChop);
					char* pFindEnd = strstr(pDatatmp, "\"\0");
					pDatatmp[pFindEnd - pDatatmp] = 0;
					if (strlen(pDatatmp) < 256) strcpy(cDownloadStoreSessionToken, pDatatmp);
					pChop = pFindEnd + 1;
				}
			}
		}

		if (strlen(cDownloadStoreLoginUrl) <= 0)
		{
			bDownloadStoreError = true;
		}
		iDownloadStoreProgress++;
	}

	if (iDownloadStoreProgress == 2 && bLoginButtonClicked)
	{
		//PE: Check if user had been logged in. only every 3 sec.
		if (fCheckForLoginTimer < Timer()) 
		{
			fCheckForLoginTimer = Timer() + 3000;
			iCheckForLoginCount++;
			memset(pDataReturned, 0, sizeof(pDataReturned));
			dwDataReturnedSize = 0;
			char cUrl[256];
			sprintf(cUrl, "/api/auth/session/status?token=%s", cDownloadStoreSessionToken);
			UINT iError = StoreOpenURLForDataOrFile(NULL, pDataReturned, &dwDataReturnedSize, "", "GET", cUrl, NULL);
			if (iError <= 0 && *pDataReturned != 0 && strchr(pDataReturned, '{') != 0)
			{
				// break up response string
				char* pChop;
				strcpy(pDatatmp, "");
				strcpy(cDownloadStoreUserHash, "");
				strcpy(cDownloadStoreUserId, "");
				if ((pChop = (char *)pestrcasestr(pDataReturned, "\"success\":true")) != NULL)
				{
					// success
					pChop += strlen("\"success\":true");
					char pSearchForToken[256];
					strcpy(pSearchForToken, "\"logged_in\"\0");
					char LoggedIn[256];
					pChop = strstr(pChop, pSearchForToken);
					if (pChop) 
					{
						pChop += strlen(pSearchForToken) + 1;
						strcpy(pDatatmp, pChop);
						char* pFindEnd = strstr(pDatatmp, ",\"\0");
						if (!pFindEnd)
							pFindEnd = strstr(pDatatmp, "\"\0");
						if (pFindEnd)
						{
							pDatatmp[pFindEnd - pDatatmp] = 0;
							if (strlen(pDatatmp) < 256) strcpy(LoggedIn, pDatatmp);
							pChop = pFindEnd+1;
						}
					}
					//Are we logged in ?
					if (pestrcasestr(LoggedIn, "true") || pestrcasestr(LoggedIn, "1"))
					{
						strcpy(pSearchForToken, "user_id\"\0");
						pChop = strstr(pChop, pSearchForToken);
						if (pChop) 
						{
							pChop += strlen(pSearchForToken) + 1;
							strcpy(pDatatmp, pChop);
							char* pFindEnd = strstr(pDatatmp, ",\"\0");
							if(!pFindEnd)
								pFindEnd = strstr(pDatatmp, "\"\0");
							if (pFindEnd) 
							{
								pDatatmp[pFindEnd - pDatatmp] = 0;
								if (strlen(pDatatmp) < 256) strcpy(cDownloadStoreUserId, pDatatmp);
								pChop = pFindEnd + 1;
							}
						}

						if (pChop) 
						{
							strcpy(pSearchForToken, "user_hash\"\0");
							pChop = strstr(pChop, pSearchForToken);
							if (pChop) 
							{
								pChop += strlen(pSearchForToken) + 2;
								strcpy(pDatatmp, pChop);
								char* pFindEnd = strstr(pDatatmp, "\"\0");
								if (pFindEnd) 
								{
									pDatatmp[pFindEnd - pDatatmp] = 0;
									if (strlen(pDatatmp) < 256) strcpy(cDownloadStoreUserHash, pDatatmp);
									pChop = pFindEnd + 1;
								}
							}
						}
						if (strlen(cDownloadStoreUserId) <= 0) 
						{
							//PE: Failed.
							bDownloadStoreError = true;
						}
						else
						{
							iDownloadStoreProgress++;
						}
					}
				}
			}
		}
	}

	if (iDownloadStoreProgress == 4 && !bDownloadStoreError )
	{
		sprintf(pDownloadFile, "downloads\\storedownload.lst");

		if (FileExist(pDownloadFile) == 1)
			DeleteAFile(pDownloadFile);

		memset(pDataReturned, 0, sizeof(pDataReturned));
		dwDataReturnedSize = 0;
		char cUrl[1024];

		sprintf(cUrl, "/api/purchases/download?user_id=%s&hash=%s&app=GGMax", cDownloadStoreUserId, cDownloadStoreUserHash);
		UINT iError = StoreOpenURLForDataOrFile(NULL, pDataReturned, &dwDataReturnedSize, "", "GET", cUrl, pDownloadFile);
		if (iError > 0)
		{
			//PE:
			if (iError == ERROR_ALREADY_EXISTS)
			{
				//PE: This some times happens for some reason.
				//PE: Retry seams to fix it so.
				static int iRetryDownloadList = 0;
				if (iRetryDownloadList++ < 3) 
				{
					iDownloadStoreProgress = 0;
				}
				else 
				{
					bDownloadStoreError = true;
				}
			}
			else
				bDownloadStoreError = true;
		}
	}

	if (bDownloadStore_Window)
	{
		ImGui::SetNextWindowSize(ImVec2(50 * ImGui::GetFontSize(), 35 * ImGui::GetFontSize()), ImGuiCond_Once);
		ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
		ImGui::Begin("Game Creator Store Downloader##DownloadStoreWindow", &bDownloadStore_Window, 0);

		// A: present user with sequence they should follow in order (top to bottom)
		ImGui::Indent(10);
		ImGui::SetWindowFontScale(0.2);
		ImGui::Text("");
		ImGui::SetWindowFontScale(1.25);
		ImGui::Text("About This Utility");
		ImGui::SetWindowFontScale(1.0);
		// logo
		ImVec2 vStoreCurPos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(vStoreCurPos + ImVec2(475, -10));
		ImGui::ImgBtn(MARKETPLACE_GCSTORE, ImVec2(460 / 2, 215 / 2), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), 0, 0, 0, 0, false, false, false, false, false, false);
		ImGui::SetCursorPos(vStoreCurPos + ImVec2(475, (215 / 2)-5));
		if (ImGui::StyleButton("Register", ImVec2(460 / 2, 0)))
		{
			ExecuteFile("https://gamecreator.store/register", "", "", 0);
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click here to register a new account on the Game Creator Store");
		ImGui::SetCursorPos(vStoreCurPos);
		ImGui::Text("This is the Game Creator Store Downloader. You use this tool to download your");
		ImGui::Text("purchases from the store server to your local GameGuru MAX asset library.");
		ImGui::Text("");
		ImGui::SetWindowFontScale(1.25);
		ImGui::Text("How to use");
		ImGui::SetWindowFontScale(1.0);
		ImGui::Text("  - You must have an existing Game Creator Store account");
		ImGui::Text("  - New users can sign up to the store by clicking the Register button");
		ImGui::Text("  - Click the Log in button to sign into your store account");
		ImGui::Text("  - Return to this screen where you will be prompted to download your assets");
		ImGui::Separator();

		// now control the download sequence
		bool bGetNextStep = false;
		ImGui::SetWindowFontScale(1.25);
		if (bDownloadStoreError)
		{
			ImGui::Text("Something Went Wrong!");
		}
		else if (iDownloadStoreProgress == 1)
		{
			ImGui::Text("Getting Login Information");
			if (strlen(cDownloadStoreLoginUrl) > 0 && strlen(cDownloadStoreSessionToken) > 0) 
			{
				bGetNextStep = true;
				bLoginButtonClicked = false;
				iCheckForLoginCount = 0;
			}
		}
		else if (iDownloadStoreProgress == 2 && !bLoginButtonClicked)
		{
			ImGui::Text("You need to Login");
			if (strlen(cDownloadStoreUserId) > 0 && strlen(cDownloadStoreUserHash) > 0)
				bGetNextStep = true;
		}
		else if (iDownloadStoreProgress == 2 && bLoginButtonClicked)
		{
			ImGui::Text("Waiting For Login (%ld)", iCheckForLoginCount);
			if (strlen(cDownloadStoreUserId) > 0 && strlen(cDownloadStoreUserHash) > 0)
				bGetNextStep = true;
		}
		else if (iDownloadStoreProgress == 3)
		{
			ImGui::Text("Downloading List...");
			if (strlen(cDownloadStoreUserId) > 0 && strlen(cDownloadStoreUserHash) > 0)
				bGetNextStep = true;
		}
		else if (iDownloadStoreProgress == 4)
		{
			ImGui::Text("Loading Download List!");
			if (FileExist(pDownloadFile) == 1)
			{
				bGetNextStep = true;
			}
			else 
			{
				strcpy(cDownloadStoreError, "Error: Cant locate download list.");
				bDownloadStoreError = true;
			}
		}
		else if (iDownloadStoreProgress == 5)
		{
			if (pDownloadStoreData) 
			{
				delete[] pDownloadStoreData;
				pDownloadStoreData = NULL;
				dwDownloadStoreDataSize = 0;
			}

			if (pDownloadStoreChecksumFile) 
			{
				delete[] pDownloadStoreChecksumFile;
				pDownloadStoreChecksumFile = NULL;
			}

			ImGui::Text("Parsing Download List!");
			HANDLE hFile = GG_CreateFile("downloads\\storedownload.lst", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				DWORD readen;
				DWORD dwDataSize = GetFileSize(hFile, 0);
				pDownloadStoreData = new char[dwDataSize + 20];
				dwDownloadStoreDataSize = dwDataSize;
				if (pDownloadStoreData)
				{
					ReadFile(hFile, pDownloadStoreData, dwDataSize, &readen, FALSE);
					pDownloadStoreData[dwDataSize] = 0;
					pDownloadStoreData[dwDataSize + 1] = 0;
					CloseHandle(hFile);
					bGetNextStep = true;

					//Also get checksum file.
					//We have 2 locations to download to and have checksums for.
					//PE: Use same checksum file so it dont matter if you download local or in doc folder.
					cStr checksumfile = "downloads\\storechecksum.lst";
					hFile = GG_CreateFile(checksumfile.Get(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile != INVALID_HANDLE_VALUE)
					{
						dwDataSize = GetFileSize(hFile, 0);
						pDownloadStoreChecksumFile = new char[dwDataSize + 20];
						if (pDownloadStoreChecksumFile)
						{
							ReadFile(hFile, pDownloadStoreChecksumFile, dwDataSize, &readen, FALSE);
							pDownloadStoreChecksumFile[dwDataSize] = 0;
							pDownloadStoreChecksumFile[dwDataSize + 1] = 0;
							CloseHandle(hFile);
						}
					}
				}
				else 
				{
					strcpy(cDownloadStoreError, "Error: Cant allocate memory for download list.");
					bDownloadStoreError = true;
				}
			}
		}
		else if (iDownloadStoreProgress == 6)
		{
			ImGui::Text("Parsing Download List!");

			files_updated = 0;
			total_files = 0;
			real_total_files = 0;
			real_files_updated = 0;

			download_list.clear();
			char pSearchForToken[256];
			char *progress = pDownloadStoreData;

			while (progress = strstr(progress, "\"id\":"))
			{
				StoreItems * si = new StoreItems;
				int id = 0;

				int iValidEntries = 0;
				char * cEndString = strstr(progress,"\"filesize\":");
				if (cEndString)
				{
					strcpy(pSearchForToken, "\"id\"\0");
					if (progress) 
					{
						progress += strlen(pSearchForToken) + 1;
						char* pFindEnd = strstr(progress, ",\"\0");
						if (!pFindEnd)
							pFindEnd = strstr(progress, "\"\0");
						if (pFindEnd) 
						{
							char cTmp = progress[pFindEnd - progress];
							progress[pFindEnd - progress] = 0;
							if (strlen(progress) < 256) 
							{
								si->id = atoi(progress);
								iValidEntries++;
							}
							progress[pFindEnd - progress] = cTmp;
							progress = pFindEnd + 1;
						}
					}

					strcpy(pSearchForToken, "\"name\"\0");
					progress = strstr(progress, pSearchForToken);
					if (progress) 
					{
						progress += strlen(pSearchForToken) + 2;
						char* pFindEnd = strstr(progress, "\"\0");
						if (pFindEnd) 
						{
							char cTmp = progress[pFindEnd - progress];
							progress[pFindEnd - progress] = 0;
							if (strlen(progress) < 256) 
							{
								si->name = progress;
								iValidEntries++;
							}
							progress[pFindEnd - progress] = cTmp;
							progress = pFindEnd + 1;
						}
					}

					strcpy(pSearchForToken, "\"url\"\0");
					progress = strstr(progress, pSearchForToken);
					if (progress) 
					{
						progress += strlen(pSearchForToken) + 2;
						char* pFindEnd = strstr(progress, "\"\0");
						if (pFindEnd) 
						{
							char cTmp = progress[pFindEnd - progress];
							progress[pFindEnd - progress] = 0;
							if (strlen(progress) < 2048)//256) newer URLs are OOOOGE!
							{
								si->url = progress;
								iValidEntries++;
							}
							progress[pFindEnd - progress] = cTmp;
							progress = pFindEnd + 1;
						}
					}

					strcpy(pSearchForToken, "\"files\"\0");
					progress = strstr(progress, pSearchForToken);
					if (progress) 
					{
						progress += strlen(pSearchForToken) + 2;
						char* pFindEnd = strstr(progress, "}]\0");
						if (pFindEnd) 
						{
							char cTmp = progress[pFindEnd - progress];
							progress[pFindEnd - progress] = 0;
							if (strlen(progress) < dwDownloadStoreDataSize)//8192) some items can have a HUGE number of files!!
							{
								si->data = progress;
								iValidEntries++;
							}
							progress[pFindEnd - progress] = cTmp;
							progress = pFindEnd + 1;
						}
					}

					strcpy(pSearchForToken, "\"checksum\"\0");
					progress = strstr(progress, pSearchForToken);
					if (progress) 
					{
						progress += strlen(pSearchForToken) + 2;
						char* pFindEnd = strstr(progress, "\"\0");
						if (pFindEnd) 
						{
							char cTmp = progress[pFindEnd - progress];
							progress[pFindEnd - progress] = 0;
							if (strlen(progress) < 256) 
							{
								si->checksum = progress;
								iValidEntries++;
							}
							progress[pFindEnd - progress] = cTmp;
							progress = pFindEnd + 1;
						}
					}

					if (iValidEntries == 5) 
					{
						download_list.insert(std::make_pair(total_files++, si));

						//Files in pack.
						int iRealFiles = 0;
						char *pDate = si->data.Get();
						while (pDate = strstr(pDate, "\"zip\":"))
						{
							pDate += 6;
							iRealFiles++;
						}
						real_total_files += iRealFiles;

						//Check if we need to update this file.
						if (pDownloadStoreChecksumFile)
						{
							if (!strstr(pDownloadStoreChecksumFile, si->checksum.Get())) 
							{
								files_updated++;
								real_files_updated += iRealFiles;
							}
						}
						else 
						{
							files_updated++;
							real_files_updated += iRealFiles;
						}

					}
					else 
					{
						delete si;
					}
				}
				else 
				{
					progress++;
				}
			}
			files_downloaded = 0;
			real_files_downloaded = 0;
			bGetNextStep = true;
		}
		else if (iDownloadStoreProgress == 7)
		{
			if (download_list.size() > 0)
			{
				ImGui::Text("Logged in - You have %ld purchased item (MAX compatible)", download_list.size());// real_total_files);
			}
			else 
			{
				strcpy(cDownloadStoreError, "Info: Could not find any MAX compatible purchases to download.");
				bDownloadStoreError = true;
			}
		}
		else if (iDownloadStoreProgress == 8)
		{
			ImGui::Text("Downloading...");
		}
		else if (iDownloadStoreProgress == 9)
		{
			ImGui::Text("Download done.");
		}
		ImGui::SetWindowFontScale(1.0);

		// progress bar and further status text
		if (iDownloadStoreProgress == 2)
		{
			float down_gadget_size = ImGui::GetFontSize()*20.0;
			float w = ImGui::GetWindowContentRegionWidth();
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (down_gadget_size*0.5), 0.0f));
			if (ImGui::StyleButton("Log in the Game Creator Store", ImVec2(down_gadget_size, 0)))
			{
				bLoginButtonClicked = true;
				iCheckForLoginCount = 0;
				ExecuteFile(cDownloadStoreLoginUrl, "", "", 0);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click to create a log in session with the store");
		}
		else if (iDownloadStoreProgress > 2 && iDownloadStoreProgress <= 6)
		{
			float fdone = 0.0f +((iDownloadStoreProgress-1)*0.5f);
			if (fdone > 1.0f) fdone = 1.0f;
			ImGui::ProgressBar(fdone, ImVec2(ImGui::GetContentRegionAvail().x - 10, 26), "");
			ImGui::Text("");
		}
		else if (iDownloadStoreProgress == 7 && download_list.size() > 0)
		{
			float donefiles;
			if (real_files_downloaded <= 0)
				donefiles = 0.0f;
			else
				donefiles = (float)real_files_downloaded / real_files_updated;
			char tmp[32];
			bool bAllPurchasesUpToDate = false;
			ImVec2 pOldCursorPos = ImGui::GetCursorPos();
			if (pDownloadStoreChecksumFile && files_updated == 0)
			{
				sprintf(tmp, "All Purchases Up To Date (checksum in 'downloads\\storechecksum.lst' matches last download event)");
				bAllPurchasesUpToDate = true;
			}
			else
			{
				sprintf(tmp, "Purchases (%ld/%ld) - Files (%ld/%ld)", files_downloaded, download_list.size(), real_files_downloaded, real_files_updated);
			}
			ImGui::ProgressBar(donefiles, ImVec2(ImGui::GetContentRegionAvail().x - 10, 26), tmp);

			float down_gadget_size = ImGui::GetFontSize()*10.0;
			float w = ImGui::GetWindowContentRegionWidth();
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (down_gadget_size*0.5), 0.0f));
			cStr cLabel = "Update ";
			cLabel += cStr((int)download_list.size());
			cLabel += " Purchases";
			if (files_downloaded > 0) cLabel = "Continue Download";
			if (bAllPurchasesUpToDate == false || files_downloaded > 0)
			{
				if (ImGui::StyleButton(cLabel.Get(), ImVec2(down_gadget_size, 0)))
				{
					//Start download , one in each sync.
					bPrintFirstEntry = true;
					bGetNextStep = true;
				}
				if (files_downloaded == 0 && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click to download your latest purchases");
			}
			if ((files_updated != total_files && files_downloaded == 0) || bAllPurchasesUpToDate==true )
			{
				float down_gadget_size = ImGui::GetFontSize()*10.0;
				float w = ImGui::GetWindowContentRegionWidth();
				ImGui::SetCursorPos(ImVec2(pOldCursorPos.x,ImGui::GetCursorPos().y) + ImVec2((w*0.5) - (down_gadget_size*0.5), 0.0f));
				if (ImGui::StyleButton("Redownload Everything", ImVec2(down_gadget_size, 0)))
				{
					int iAction = askBoxCancel("This will delete the status of all downloaded files and you must download everything again, are you sure?", "Confirmation"); //1==Yes 2=Cancel 0=No
					if (iAction == 1)
					{
						char checksumfile[MAX_PATH];
						strcpy(checksumfile, "downloads\\storechecksum.lst");
						GG_GetRealPath(&checksumfile[0], 1);

						if (FileExist(checksumfile) == 1)
							DeleteAFile(checksumfile);

						iDownloadStoreProgress = 5; //Read checksum files again.
					}
				}
				if (files_downloaded == 0 && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click to download ALL your previous purchases");
			}
		}
		else if (iDownloadStoreProgress == 8 && download_list.size() > 0)
		{
			// same code as above
			float donefiles;
			if (real_files_downloaded <= 0)
				donefiles = 0.0f;
			else
				donefiles = (float)real_files_downloaded / real_files_updated;
			char tmp[32];
			sprintf(tmp, "Purchases (%ld/%ld) - Files (%ld/%ld)", files_downloaded, download_list.size(), real_files_downloaded, real_files_updated);
			ImGui::ProgressBar(donefiles, ImVec2(ImGui::GetContentRegionAvail().x - 10, 26), tmp);

			ImGui::Text("");
			bool bCancelKeyPressed = false;
			int iRealFilesUpdated = 0;
			if (files_downloaded < files_updated)
			{
				int getting_file = -1;
				for (int i = 0; i < total_files; i++)
				{
					if (pDownloadStoreChecksumFile)
					{
						if (!strstr(pDownloadStoreChecksumFile, download_list[i]->checksum.Get()))
						{
							getting_file++;
						}
					}
					else
					{
						getting_file++;
					}
					if (getting_file >= files_downloaded)
					{
						getting_file = i;
						break;
					}
				}	
				if (getting_file >= 0) 
				{
					ImGui::TextCenter("Downloading: %s - %ld", download_list[getting_file]->name.Get(), download_list[getting_file]->id);

					if (!bPrintFirstEntry)
					{
						char pZipFile[256];
						sprintf(pZipFile, "downloads\\storeitem.zip");

						if (FileExist(pZipFile) == 1)
							DeleteAFile(pZipFile);

						memset(pDataReturned, 0, sizeof(pDataReturned));
						dwDataReturnedSize = 0;
						char cUrl[1024];
						std::string url = download_list[getting_file]->url.Get();
						replaceAll(url, "\\/", "/");
						replaceAll(url, "https://gcs-product-media.fra1.digitaloceanspaces.com", "");
						replaceAll(url, "http://gcs-product-media.fra1.digitaloceanspaces.com", "");
						strcpy(cUrl, url.c_str());
						LPSTR pPassInURL = cUrl;
						// pPassInURL Example = "/data-files/ggmax/36084_2021-09-18_14-15_894962614.zip?X-Amz-Content-Sha256=UNSIGNED-PAYLOAD&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=J7LHIX3DWJWZH2HDPTED%2F20220525%2Ffra1%2Fs3%2Faws4_request&X-Amz-Date=20220525T094657Z&X-Amz-SignedHeaders=host&X-Amz-Expires=3600&X-Amz-Signature=751d98de71f9936efa74b38f0ab236bd9d374a58fbff51af02e5d3c30e1c226f";
						UINT iError = StoreOpenURLForDataOrFile("gcs-product-media.fra1.digitaloceanspaces.com", pDataReturned, &dwDataReturnedSize, "", "GET", pPassInURL, pZipFile);
						if (iError > 0)
						{
							//error
							if (iError == 123456) 
							{
								bCancelKeyPressed = true;
								iDownloadStoreProgress = 7;
							}
							else
							{
								bDownloadStoreError = true;
							}
						}
						else
						{
							bool bFilesUpdated = false;
							char cZipSource[256];
							char cZipDestination[256];
							int iValidEntries = 0;
							char *pDate = download_list[getting_file]->data.Get();

							std::string data = pDate;
							replaceAll(data, "\\/", "/");

							char *pStart = (char *)data.c_str();
							char pSearchForToken[256];
							while (pStart = strstr(pStart, "\"zip\":"))
							{
								iRealFilesUpdated++;
								strcpy(pSearchForToken, "\"zip\":");
								if (pStart) 
								{
									pStart += strlen(pSearchForToken) + 1;
									char* pFindEnd = strstr(pStart, "\"\0");
									if (pFindEnd) 
									{
										char cTmp = pStart[pFindEnd - pStart];
										pStart[pFindEnd - pStart] = 0;
										if (strlen(pStart) < 256) 
										{
											strcpy(cZipSource, pStart);
											iValidEntries++;
										}
										pStart[pFindEnd - pStart] = cTmp;
										pStart = pFindEnd + 1;
									}
									strcpy(pSearchForToken, "\"extract\":");
									pStart = strstr(pStart, pSearchForToken);
									if (pStart) 
									{
										pStart += strlen(pSearchForToken) + 1;
										char* pFindEnd = strstr(pStart, "\"\0");
										if (pFindEnd) 
										{
											char cTmp = pStart[pFindEnd - pStart];
											pStart[pFindEnd - pStart] = 0;
											if (strlen(pStart) < 256) 
											{
												strcpy(cZipDestination, pStart);
												iValidEntries++;
											}
											pStart[pFindEnd - pStart] = cTmp;
											pStart = pFindEnd + 1;
										}
									}
								}
								if (iValidEntries == 2)
								{
									//Process file.
									OpenFileBlockNoPw(pZipFile, 1, "");
									PerformCheckListForFileBlockData(1);
									for (t.i = 1; t.i <= ChecklistQuantity(); t.i++)
									{
										char * pZipName = ChecklistString(t.i);
										if (pZipName && stricmp(pZipName, cZipSource) == 0)
										{
											//Found entry , extract it.
											char FullPath[256];
											strcpy(FullPath, "downloads\\extract\\");
											GG_GetRealPath(&FullPath[0], 1);

											std::string FullFilePath;
											FullFilePath = FullPath;
											FullFilePath.append(pZipName);
											replaceAll(FullFilePath, "/", "\\");

											if (FileExist((char *)FullFilePath.c_str()) == 1)
												DeleteAFile((char *)FullFilePath.c_str());

											ExtractFileFromBlock(1, pZipName, FullPath);

											char cDestinationFullPath[256];
											strcpy(cDestinationFullPath, StoreWriteFolder);
											strcat(cDestinationFullPath, cZipDestination);
											std::string stmp = cDestinationFullPath;
											replaceAll(stmp, "/", "\\");
											strcpy(cDestinationFullPath, stmp.c_str());
											int GG_CreatePath(const char *path);
											GG_CreatePath(cDestinationFullPath);

											if (FileExist((char *)cDestinationFullPath) == 1)
												DeleteAFile((char *)cDestinationFullPath);

											CopyAFile((char *)FullFilePath.c_str(), cDestinationFullPath);

											if (FileExist((char *)cDestinationFullPath) == 1)
											{
												//Success , add checksum to our update file.
												bFilesUpdated = true;
											}
											//Remove tmp unzip file.
											if (FileExist((char *)FullFilePath.c_str()) == 1)
												DeleteAFile((char *)FullFilePath.c_str());
										}
									}
									CloseFileBlock(1);
									iValidEntries = 0;
								}
							}

							if (bFilesUpdated)
							{
								FILE *inputf;
								cStr checksumfile = "downloads\\storechecksum.lst";
								inputf = GG_fopen(checksumfile.Get(), "a+");
								fprintf(inputf, "%s,", download_list[getting_file]->checksum.Get());
								fclose(inputf);
							}
						}
					}
				}
			}
			else 
			{
				//Download done.
				bGetNextStep = true;
			}
			if (!bPrintFirstEntry && !bCancelKeyPressed) 
			{
				files_downloaded++;
				real_files_downloaded+=iRealFilesUpdated;
			}

			ImGui::Text("");
			float down_gadget_size = ImGui::GetFontSize()*14.0;
			float w = ImGui::GetWindowContentRegionWidth();
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (down_gadget_size*0.5), 0.0f));
			if (ImGui::StyleButton("Cancel Download (Hold ESC)", ImVec2(down_gadget_size, 0)))
			{
				iDownloadStoreProgress = 7;
			}
			if (ImGui::IsKeyPressedMap(ImGuiKey_Escape))
			{
				iDownloadStoreProgress = 7;
			}		
			if (bPrintFirstEntry)
			{
				bPrintFirstEntry = false;
			}
		}
		else if (iDownloadStoreProgress == 9 )
		{
			//All done only close buttom left.
		}

		if (bGetNextStep) 
		{
			bDownloadStoreError = false;
			iDownloadStoreProgress++;
		}

		//Debug info , show all informations.
		if (iDownloadStoreProgress > 0 && bDownloadStoreError)
		{
			ImGui::Text("");
			if (bDownloadStoreError)
			{
				ImGui::TextWrapped(cDownloadStoreError);
			}
		}

		// allow user to select WHERE these files will go
		ImGui::Separator();
		static bool first_time = true;
		static bool bWriteFolderPossible = false;
		if (first_time)
		{
			//Check if we have write acces to .exe folder.
			void FileRedirectSetup();
			FileRedirectSetup();
			extern char szRootDir[MAX_PATH];
			extern char szWriteDir[MAX_PATH];
			strcpy(StoreWriteFolder, szRootDir);
			strcat(StoreWriteFolder, "Files\\test.tmp");
			strcpy(StoreDocWriteFolder, szWriteDir);
			strcat(StoreDocWriteFolder, "Files\\");
			strcpy(StoreAppWriteFolder, szRootDir);
			strcat(StoreAppWriteFolder, "Files\\");
			FILE* testFile = fopen(StoreWriteFolder, "w");
			if (testFile)
			{
				fprintf(testFile, "test");
				fclose(testFile);
			}
			if (FileExist(StoreWriteFolder) == 1)
			{
				DeleteAFile(StoreWriteFolder);
				bWriteFolderPossible = true;
			}
			strcpy(StoreWriteFolder, StoreDocWriteFolder);
			iDownloadLocation = 1;
			first_time = false;
		}
		if (iDownloadStoreProgress >= 2 && iDownloadStoreProgress <= 7 && files_downloaded == 0)
		{
			ImGui::SetWindowFontScale(1.25);
			ImGui::Text("Download Folder (Advanced):");
			ImGui::SetWindowFontScale(1.0);
			if (bWriteFolderPossible)
			{
				if (ImGui::RadioButton(StoreAppWriteFolder, &iDownloadLocation, 0))
				{
					strcpy(StoreWriteFolder, StoreAppWriteFolder);
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click to change where your downloads will be stored");
				if (ImGui::RadioButton(StoreDocWriteFolder, &iDownloadLocation, 1))
				{
					strcpy(StoreWriteFolder, StoreDocWriteFolder);
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click to change where your downloads will be stored");			
			}
			else
			{
				ImGui::RadioButton(StoreDocWriteFolder, &iDownloadLocation, 0);
				strcpy(StoreWriteFolder, StoreDocWriteFolder);
				iDownloadLocation = 0;
			}
		}
		ImGui::SetWindowFontScale(1.0);

		ImGui::Separator();
		ImGui::Text("");
		float down_gadget_size = ImGui::GetFontSize()*10.0;
		float w = ImGui::GetWindowContentRegionWidth();
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (down_gadget_size*0.5), 0.0f));
		if (ImGui::StyleButton("Close", ImVec2(down_gadget_size, 0)))
		{
			if (pDownloadStoreData) 
			{
				delete[] pDownloadStoreData;
				pDownloadStoreData = NULL;
				dwDownloadStoreDataSize = 0;
			}
			if (pDownloadStoreChecksumFile) 
			{
				delete[] pDownloadStoreChecksumFile;
				pDownloadStoreChecksumFile = NULL;
			}
			if (total_files > 0) 
			{
				for (int i = 0; i < total_files; i++)
				{
					if (download_list[i])
						delete(download_list[i]);
				}
			}
			download_list.clear();
			bDownloadStore_Window = false;
			extern int g_iRefreshLibraryFolders;
			g_iRefreshLibraryFolders = 2;
		}
		ImGui::Indent(-10);
		ImGui::End();
	}
}

UINT StoreOpenURLForDataOrFile(LPSTR pServer, LPSTR pDataReturned, DWORD* pReturnDataSize, LPSTR pPostData, LPSTR pVerb, LPSTR urlWhere, LPSTR pLocalFileForImageOrNews)
{
	// default is main store 
	bool bMainStore = false;
	if (pServer == NULL)
	{
		bMainStore = true;
		pServer = "gamecreator.store";
	}

	UINT iError = 0;
	unsigned int dwDataLength = 0;
	HINTERNET m_hInet = InternetOpenA("InternetConnection", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (m_hInet == NULL)
	{
		iError = GetLastError();
	}
	else
	{
		unsigned short wHTTPType = INTERNET_DEFAULT_HTTPS_PORT;
		HINTERNET m_hInetConnect = InternetConnectA(m_hInet, pServer, wHTTPType, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
		if (m_hInetConnect == NULL)
		{
			iError = GetLastError();
		}
		else
		{
			int m_iTimeout = 5000;
			InternetSetOption(m_hInetConnect, INTERNET_OPTION_CONNECT_TIMEOUT, (void*)&m_iTimeout, sizeof(m_iTimeout));
			unsigned long flags = INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_UNKNOWN_CA | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |
				INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE |
				INTERNET_FLAG_DONT_CACHE |
				INTERNET_FLAG_KEEP_CONNECTION |
				INTERNET_FLAG_NO_UI | INTERNET_FLAG_NO_AUTO_REDIRECT;

			HINTERNET hHttpRequest = HttpOpenRequestA(m_hInetConnect, pVerb, urlWhere, "HTTP/1.1", NULL, NULL, flags, 0);
			if (hHttpRequest == NULL)
			{
				iError = GetLastError();
			}
			else
			{
				if (bMainStore == true)
				{
					char lpszHeaders[4096];
					sprintf(lpszHeaders, "Authorization: Bearer %s", SECRET_TOKEN);
					HttpAddRequestHeadersA(hHttpRequest, lpszHeaders, strlen(lpszHeaders), 0);
				}
				int bSendResult = 0;
				FILE* fpLocalFile = NULL;
				if (pLocalFileForImageOrNews == NULL)
				{
					char m_szPostData[1024];
					if (pPostData && strlen(pPostData) > 0)
					{
						strcpy(m_szPostData, pPostData);
						bSendResult = HttpSendRequest(hHttpRequest, NULL, -1, (void*)(m_szPostData), strlen(m_szPostData));
					}
					else
					{
						bSendResult = HttpSendRequest(hHttpRequest, NULL, -1, NULL, NULL);
					}
				}
				else
				{
					//open local file for writing
					bSendResult = HttpSendRequest(hHttpRequest, NULL, -1, NULL, NULL);
					fpLocalFile = GG_fopen(pLocalFileForImageOrNews, "wb+");
				}
				if (bSendResult == 0)
				{
					iError = GetLastError();
				}
				else
				{
					int m_iStatusCode = 0;
					char m_szContentType[150];
					unsigned int dwBufferSize = sizeof(int);
					unsigned int dwHeaderIndex = 0;
					HttpQueryInfo(hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, (void*)&m_iStatusCode, (LPDWORD)&dwBufferSize, (LPDWORD)&dwHeaderIndex);
					dwHeaderIndex = 0;
					unsigned int dwContentLength = 0;
					HttpQueryInfo(hHttpRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (void*)&dwContentLength, (LPDWORD)&dwBufferSize, (LPDWORD)&dwHeaderIndex);
					dwHeaderIndex = 0;
					unsigned int ContentTypeLength = 150;
					HttpQueryInfo(hHttpRequest, HTTP_QUERY_CONTENT_TYPE, (void*)m_szContentType, (LPDWORD)&ContentTypeLength, (LPDWORD)&dwHeaderIndex);
					char pBuffer[20000];
					for (;;)
					{
						unsigned int written = 0;
						if (!InternetReadFile(hHttpRequest, (void*)pBuffer, 2000, (LPDWORD)&written))
						{
							// error
						}
						if (written == 0) break;
						if (ImGui::IsKeyPressedMap(ImGuiKey_Escape))
						{
							iError = 123456;
							break;
						}
						if (fpLocalFile)
						{
							// write direct to file
							fwrite(pBuffer, 1, written, fpLocalFile);
						}
						else
						{
							if (dwDataLength + written > MAXSTOREDATESIZE) written = MAXSTOREDATESIZE - dwDataLength;
							memcpy(pDataReturned + dwDataLength, pBuffer, written);
							dwDataLength = dwDataLength + written;
							if (dwDataLength >= MAXSTOREDATESIZE) break;
						}
					}
					InternetCloseHandle(hHttpRequest);
				}
				if (fpLocalFile)
				{
					fclose(fpLocalFile);
					fpLocalFile = NULL;
				}
			}
			InternetCloseHandle(m_hInetConnect);
		}
		InternetCloseHandle(m_hInet);
	}
	if (iError > 0 )
	{
		if (iError == 123456)
		{
			strcpy(cDownloadStoreError, "Info: Pressed Cancel (ESC).");
		}
		else
		{
			char *szError = 0;
			if (iError > 12000 && iError < 12174)
				FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE, GetModuleHandleA("wininet.dll"), iError, 0, (char*)&szError, 0, 0);
			else
				FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, iError, 0, (char*)&szError, 0, 0);
			if (szError)
			{
				if (strlen(szError) < 8172)
				{
					strcpy(cDownloadStoreError, "Error: ");
					strcat(cDownloadStoreError, szError);
				}
				else
				{
					sprintf(cDownloadStoreError, "Unknown Error: %d", iError);
				}
				LocalFree(szError);
			}
		}
	}

	//Url decode (not when using files).
	if (dwDataLength > 0 && strlen(pDataReturned) > 0) 
	{
		std::string url_decode = pDataReturned;
		replaceAll(url_decode, "\\/", "/");
		replaceAll(url_decode, "%2F", "/");
		replaceAll(url_decode, "%24" , "$");
		strcpy(pDataReturned, url_decode.c_str());
	}

	// complete
	*pReturnDataSize = dwDataLength;
	return iError;
}
