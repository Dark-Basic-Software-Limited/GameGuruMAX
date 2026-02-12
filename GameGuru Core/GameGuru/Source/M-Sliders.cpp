//----------------------------------------------------
//--- GAMEGURU - M-Sliders
//----------------------------------------------------

// Globals
#include "stdafx.h"
#include "gameguru.h"

#include "..\Imgui\imgui.h"
#include "..\Imgui\imgui_impl_win32.h"
#include "..\Imgui\imgui_gg_dx11.h"

#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

// Prototypes
void set_inputsys_mclick(int value);

// Externals
extern UINT g_StereoEyeToggle;

// 
//  Menu, Display & Sliders Module
// 

void sliders_init ( void )
{
	//  load images for slider resources
	//t.timgbase=g.slidersmenuimageoffset;

	//  Multiple panels allowed
	g.slidersmenumax=0;

	//  Weapons in-game panel
	++g.slidersmenumax;
	t.slidersmenunames.weapon=g.slidersmenumax;
	// no VR to worry about for EA version of MAX
	t.slidersmenu[g.slidersmenumax].tabpage = 0;

	// Player status in-game panel
	++g.slidersmenumax;
	t.slidersmenunames.player=g.slidersmenumax;
	if (t.player[1].health == 99999)
		t.slidersmenu[g.slidersmenumax].tabpage = -10;
	else
		t.slidersmenu[g.slidersmenumax].tabpage = 0;
	t.slidersmenu[g.slidersmenumax].title_s="";
	t.slidersmenu[g.slidersmenumax].thighlight=-1;
	t.slidersmenu[g.slidersmenumax].titlemargin=20;
	t.slidersmenu[g.slidersmenumax].leftmargin=25;
	t.slidersmenu[g.slidersmenumax].itemcount = 2;

	// reserve old lives value for something else (perhaps armour/shield for player)?
	t.slidersmenuvalue[g.slidersmenumax][1].name_s = "";
	t.slidersmenuvalue[g.slidersmenumax][1].value = 0;
	t.slidersmenuvalue[g.slidersmenumax][1].readmodeindex = 5;

	t.slidersmenuvalue[g.slidersmenumax][2].name_s = "Health";
	t.slidersmenuvalue[g.slidersmenumax][2].value = t.player[1].health;
	t.slidersmenuvalue[g.slidersmenumax][2].readmodeindex = 4;
	t.slidersmenu[g.slidersmenumax].panelheight=(t.slidersmenu[g.slidersmenumax].itemcount*38)-40;
	t.slidersmenu[g.slidersmenumax].ttop=GetDisplayHeight()-146;
	t.slidersmenu[g.slidersmenumax].tleft=GetDisplayWidth()-16-230;
	t.slidersmenu[g.slidersmenumax].customimage=g.slidersmenuimageoffset+51;
	t.slidersmenu[g.slidersmenumax].customimagetype=2;
	t.slidersmenu[g.slidersmenumax].customimagesubtype=0;
}

void sliders_free ( void )
{
	// no need to free as will be returning to in-game test
	g.slidersmenumax = 0;
}

void sliders_loop ( void )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	void tab_tab_visuals(int iPage, int iMode);
	extern bool bImGuiInTestGame;
	if (g.tabmode == 2) {
		if (bImGuiInTestGame) {
			tab_tab_visuals(1, 1);
		}
	}
	if (g.tabmode == 1) {
		if (bImGuiInTestGame) {
			tab_tab_visuals(2, 1);
		}
	}
	if (g.tabmode == 0) {
		if (bImGuiInTestGame) {
			//wiProfiler::ResetPeek(); // Not available in current WickedEngine
		}
	}
	return;
}

void sliders_readall ( void )
{
	if ( t.slidersmenuindex == t.slidersmenunames.weapon ) 
	{
		if ( g.weaponammoindex>0 ) 
		{
			t.slidersmenuvalue[t.slidersmenuindex][1].value=t.weaponammo[g.weaponammoindex+g.ammooffset];
			t.tfiremode=t.gun[t.gunid].settings.alternate;
			t.tpool=g.firemodes[t.gunid][t.tfiremode].settings.poolindex;
			if (  t.tpool == 0  )  t.tammo = t.weaponclipammo[g.weaponammoindex+g.ammooffset]; else t.tammo = t.ammopool[t.tpool].ammo;
			t.slidersmenuvalue[t.slidersmenuindex][2].value=t.tammo;
		}
		else
		{
			t.slidersmenuvalue[t.slidersmenuindex][1].value=0;
			t.slidersmenuvalue[t.slidersmenuindex][2].value=0;
		}
	}
	if (  t.slidersmenuindex == t.slidersmenunames.player ) 
	{
		//  Player readout
		// figure something out for this extra reserved place in the HUD
		t.slidersmenuvalue[t.slidersmenuindex][1].value = 0;
		t.slidersmenuvalue[t.slidersmenuindex][2].value=t.player[1].health;
	}
}

void sliders_draw ( void )
{
	// LUA PROMPT IMAGES
	if (  t.promptimage.show == 1 && t.conkit.editmodeactive == 0 ) 
	{
		// image prompt 2D
		if (  ImageExist(t.promptimage.img) == 1 ) 
		{
			if (  t.promptimage.alignment == 0 ) 
			{
				t.tx_f=ImageWidth(t.promptimage.img)/2.0;
				t.ty_f=ImageHeight(t.promptimage.img)/2.0;
			}
			else
			{
				t.tx_f=0 ; t.ty_f=0;
			}
			t.tx_f=((GetDisplayWidth()/100.0)*t.promptimage.x)-t.tx_f;
			t.ty_f=((GetDisplayHeight()/100.0)*t.promptimage.y)-t.ty_f;
			PasteImage (  t.promptimage.img,t.tx_f,t.ty_f,1 );
		}
	}

	//  new crosshair system (avoids motion blur issue)
	if (  t.gunid>0 && t.postprocessings.fadeinvalue_f == 1.0 ) 
	{
		static float timer = 0.0f;
		if ( g.firemodes[t.gunid][g.firemode].settings.zoomhidecrosshair == 0 && t.gunzoommode > 0 )
		{
			t.timg=t.gun[t.gunid].settings.crosshairimg;
			timer += ImGui::GetIO().DeltaTime;
			if (  t.timg>0 && timer > 0.155f) // If keeping the crosshair as a replacement for red dot, it would be best to store the timer length/frame to trigger crosshair in the gun spec.
			{
				if (  ImageExist(t.timg) == 1 ) 
				{
					t.tokay=1;
					if (  t.playercontrol.thirdperson.enabled == 1 && t.playercontrol.thirdperson.camerareticle == 0 ) 
					{
						t.tokay=0;
					}
					if (  t.tokay == 1 ) 
					{
						t.timgx=(GetDisplayWidth()-ImageWidth(t.timg))/2;
						if (  t.playercontrol.thirdperson.enabled == 1 ) 
						{
							t.timgy=(GetDisplayHeight()-ImageHeight(t.timg))/4;
							PasteImage (  t.timg,t.timgx,t.timgy,1 );
						}
						else
						{
							t.timgy=(GetDisplayHeight()-ImageHeight(t.timg))/2;
							PasteImage (  t.timg,t.timgx,t.timgy,1 );
						}
					}
				}
			}
		}
		else
		{
			timer = 0.0f;
		}
	}

	//  LUA PROMPT on HUD (set from within script)
	static cstr HUDPromptUsing_s = "";
	bool bHUDPromptCurrentlyActive = false;
	if (  t.luaglobal.scriptprompttime>0 && t.conkit.editmodeactive == 0 ) 
	{
		if ( (DWORD)(MAXTimer())<t.luaglobal.scriptprompttime+500 ) 
		{
			t.tscriptprompttextsize=t.luaglobal.scriptprompttextsize;
			t.tscriptprompttextsizeyoffset=t.tscriptprompttextsize*20;
			if (  t.tscriptprompttextsize == 0 ) { t.tscriptprompttextsize = 3  ; t.tscriptprompttextsizeyoffset = 50; }
			// attempt to send prompt to HUD Screeen "prompt:main"
			int nodeid = t.game.ingameHUDScreen;
			for (int iElementID = 0; iElementID < STORYBOARD_MAXWIDGETS; iElementID++)
			{
				extern StoryboardStruct Storyboard;
				if (Storyboard.Nodes[nodeid].widget_used[iElementID] != 0)
				{
					LPSTR pLabel = "prompt:main";
					if (stricmp(Storyboard.Nodes[nodeid].widget_label[iElementID], pLabel) == NULL)
					{
						char pUserDefinedGlobal[MAX_PATH];
						sprintf(pUserDefinedGlobal, "g_UserGlobal['%s']", pLabel);
						LuaSetString(pUserDefinedGlobal, t.luaglobal.scriptprompt_s.Get());
						HUDPromptUsing_s = pUserDefinedGlobal;
						bHUDPromptCurrentlyActive = true;
						break;
					}
				}
			}
			if (bHUDPromptCurrentlyActive == false)
			{
				// default to legacy bitmap font text system
				t.tscriptpromptx = (GetDisplayWidth() - getbitmapfontwidth(t.luaglobal.scriptprompt_s.Get(), t.tscriptprompttextsize)) / 2;
				pastebitmapfont(t.luaglobal.scriptprompt_s.Get(), t.tscriptpromptx, GetDisplayHeight() - t.tscriptprompttextsizeyoffset, t.tscriptprompttextsize, 255);
			}
		}
		else
		{
			t.luaglobal.scriptprompttime=0;
		}
	}
	if (bHUDPromptCurrentlyActive == false && strlen(HUDPromptUsing_s.Get()) > 0)
	{
		// remove string when prompt goes away
		LuaSetString(HUDPromptUsing_s.Get(), "");
		HUDPromptUsing_s = "";
	}

	// manage prompt 3D
	if ( t.luaglobal.scriptprompt3dtime>0 && t.conkit.editmodeactive == 0 ) 
	{
		if ( (DWORD)(MAXTimer())<t.luaglobal.scriptprompt3dtime+500 ) 
			lua_updateprompt3d();
		else
			lua_hideprompt3d();
	}

	// disable all HUDs if meet condition
	bool bAllowFPShileHUDSHidden = false;
	t.tokay=1;
	if ( g.globals.riftmode > 0 )  t.tokay = 0;
	if ( t.conkit.editmodeactive == 1 )  t.tokay = 0;
	if ( t.game.gameisexe == 1 )
	{
		if ( ( g.tabmodehidehuds == 1 || g.ghideallhuds == 1 ) && g.lowfpswarning != 1 && g.tabmode == 0 )  
		{
			bAllowFPShileHUDSHidden = true;
			t.tokay = 0;
		}
	}
	else
	{
		if ((g.tabmodehidehuds == 1 || g.ghideallhuds == 1) && g.lowfpswarning != 1 && g.tabmodeshowfps != 1 && g.tabmode == 0) t.tokay = 0;
	}
	if ( t.importer.importerActive == 1 ) t.tokay = 1;
	if ( t.game.runasmultiplayer == 1 ) g.ghardwareinfomode = 0;

	// also disable if in standalone and trying to use HUD views
	if ( t.game.gameisexe == 1 && g.gprofileinstandalone == 0 ) 
	{
		g.ghardwareinfomode = 0;
		g.tabmode = 0;
	}

	// Special tab mode to show JUST FPS score
	if ( (t.tokay == 1 || bAllowFPShileHUDSHidden == true ) && g.tabmode == 0 && g.tabmodeshowfps != 0 ) 
	{
		pastebitmapfont( cstr(cstr(Str(GetDisplayFPS()))+"fps").Get(),8,8,1,255);
	}

	// If HUDs allowed
	if ( t.tokay == 1 ) 
	{
		//  Special hardware info mode and controls
		if (  g.tabmodeshowfps == 1 && g.ghardwareinfomode != 0 ) 
		{
			//  Gather information
			if (  t.hardwareinfoglobals.gotgraphicsinfo == 0 ) 
			{
				t.hardwareinfoglobals.card_s=CurrentGraphicsCard();
				t.hardwareinfoglobals.dxversion_s=GetDXName();
				t.hardwareinfoglobals.monitorrefresh_s=GetDirectRefreshRate();
				t.hardwareinfoglobals.gotgraphicsinfo=1;
			}

			//  Show in realtime (F11 panel)
			pastebitmapfontcenter( cstr("Hardware Info Mode").Get(),GetDisplayWidth()/2,GetDisplayHeight()-90,1,255);

			// Resolution
			cstr pHardwareDetailInfo;
			pHardwareDetailInfo = "Graphics Card:";
			pHardwareDetailInfo += wi::graphics::GetDevice()->GetGraphicsCardName();
			pHardwareDetailInfo += "  Resolution:";
			pHardwareDetailInfo += cstr(Str(GetDisplayWidth())) + "x" + cstr(Str(GetDisplayHeight()));
			pastebitmapfontcenter(pHardwareDetailInfo.Get(),GetDisplayWidth()/2,GetDisplayHeight()-55,1,255);

			//  No more slider art in hardware info debug mode
			return;
		}
	}

	// GURU MEDITATION : Internal Errors From LUA (helps users work out any of their logic issues)
	if (t.luaglobal.gurumeditationprompttime > 0 && t.conkit.editmodeactive == 0)
	{
		if ((DWORD)(MAXTimer()) < t.luaglobal.gurumeditationprompttime + 5000)
		{
			int tgurumeditationprompttextsize = 5; // large!
			int tgurumeditationpromptx = (GetDisplayWidth() - getbitmapfontwidth(t.luaglobal.gurumeditationprompt_s.Get(), tgurumeditationprompttextsize)) / 2;
			pastebitmapfont(t.luaglobal.gurumeditationprompt_s.Get(), tgurumeditationpromptx, GetDisplayHeight()/2, tgurumeditationprompttextsize, 255);
		}
		else
		{
			t.luaglobal.gurumeditationprompttime = 0;
		}
	}

	// 100316 - draw any scope HUD first (moved out of condition below so it ALWAYS renders for the game if active)
	sliders_scope_draw ( );

	// 080917 - collects sprite immediate draw calls (text,images) so can do in one go 
	ResetSpriteBatcher();

	// only if HUDs allowed to be drawn
	if (  t.tokay == 1 ) 
	{
		// Tabs
		importer_tabs_draw ( );

		//  draw dropdown panel
		if (  t.slidersdropdown>0 ) 
		{
			t.slidersmenuindex=t.slidersdropdown;
			t.timgbase=g.slidersmenuimageoffset;
			t.rmi=t.slidersmenu[t.slidersmenuindex].thighlight-1;

			//  Ensure drop downs dont go off screen in the importer and create a drop down list
			if ( t.importer.importerActive == 1 )
			{
				t.tcol = t.col;
				t.tlistmax  =  t.slidersdropdownmax ; if (  t.tlistmax > 10  )  t.tlistmax  =  10;
				t.tboxoverflow = (t.slidersdropdowntop+10+(t.tlistmax*16)) - GetChildWindowHeight();
				if (  t.tboxoverflow > 0  )  t.slidersdropdowntop -= t.tboxoverflow;

				if (  t.inputsys.ymouse*t.tadjustedtoimporterybase_f  <=  t.slidersdropdowntop+(t.importer.dropDownListNumber*16) ) 
				{
					if (  MAXTimer() - t.importer.oldTime > 100 ) 
					{
						--t.importer.dropDownListNumber;
						if (  t.importer.dropDownListNumber < 0 ) 
						{
							t.importer.dropDownListNumber = 0;
						}
						else
						{
							t.slidersdropdowntop+=16;
						}
						t.importer.oldTime = MAXTimer();
					}
				}
				if (  t.inputsys.ymouse*t.tadjustedtoimporterybase_f  >=  t.slidersdropdowntop+10+(t.tlistmax*16)+(t.importer.dropDownListNumber*16) ) 
				{
					if (  MAXTimer() - t.importer.oldTime > 100 ) 
					{
						++t.importer.dropDownListNumber;
						if (  t.importer.dropDownListNumber > t.slidersdropdownmax-9 ) 
						{
							t.importer.dropDownListNumber = t.slidersdropdownmax-9;
						}
						else
						{
							t.slidersdropdowntop -= 16;
						}
						t.importer.oldTime = MAXTimer();
					}
				}

				GGBoxGradient (  t.slidersdropdownleft,t.slidersdropdowntop+(t.importer.dropDownListNumber*16),t.slidersdropdownleft+128,t.slidersdropdowntop+(t.importer.dropDownListNumber*16)+10+(t.tlistmax*16),192,0,0,0);//t.col,t.col,t.col,t.col );
				if (  t.slidersdropdownindex >= 0 ) 
				{
					GGBoxGradient (  t.slidersdropdownleft,t.slidersdropdowntop+5+(t.slidersdropdownindex*16),t.slidersdropdownleft+128,t.slidersdropdowntop+5+(t.slidersdropdownindex*16)+16,192,180,180,192);//t.col,t.col,t.col,t.col );
				}
				for ( t.tn = t.importer.dropDownListNumber ; t.tn<=  t.importer.dropDownListNumber+t.tlistmax-1; t.tn++ )
				{
					t.slidersmenuvaluechoice=t.slidersdropdownchoice;
					t.slidersmenuvalueindex=1+t.tn;
					sliders_getnamefromvalue ( );
					pastebitmapfont(t.slidervaluename_s.Get(),t.slidersdropdownleft+10,t.slidersdropdowntop+5+(t.tn*16),2,255);
				}

				if (  t.importer.dropDownListNumber > 0 ) 
				{
					GGBoxGradient (  t.slidersdropdownleft,t.slidersdropdowntop+(t.importer.dropDownListNumber*16)-16,t.slidersdropdownleft+128,t.slidersdropdowntop+(t.importer.dropDownListNumber*16),192,0,0,0);//t.tcol,t.tcol,t.tcol,t.tcol );
					pastebitmapfont("<",t.slidersdropdownleft+10,t.slidersdropdowntop+(t.importer.dropDownListNumber*16)-16,2,255);
				}
				if (  t.importer.dropDownListNumber+10 < t.slidersdropdownmax ) 
				{
					GGBoxGradient (  t.slidersdropdownleft,t.slidersdropdowntop+(t.importer.dropDownListNumber*16)+10+(t.tlistmax*16),t.slidersdropdownleft+128,t.slidersdropdowntop+(t.importer.dropDownListNumber*16)+10+(t.tlistmax*16)+16,192,0,0,0);//t.tcol,t.tcol,t.tcol,t.tcol );
					pastebitmapfont(">",t.slidersdropdownleft+10,t.slidersdropdowntop+(t.importer.dropDownListNumber*16)+10+(t.tlistmax*16),2,255);
				}
			}
			else
			{
				GGBoxGradient (  t.slidersdropdownleft,t.slidersdropdowntop,t.slidersdropdownleft+128,t.slidersdropdowntop+10+(t.slidersdropdownmax*16),192,0,0,0);//t.col,t.col,t.col,t.col );
				if (  t.slidersdropdownindex >= 0 ) 
				{
					GGBoxGradient (  t.slidersdropdownleft,t.slidersdropdowntop+5+(t.slidersdropdownindex*16),t.slidersdropdownleft+128,t.slidersdropdowntop+5+(t.slidersdropdownindex*16)+16,192,180,180,192);//t.col,t.col,t.col,t.col );
				}
				for ( t.tn = 0 ; t.tn<=  t.slidersdropdownmax-1; t.tn++ )
				{
					t.slidersmenuvaluechoice=t.slidersdropdownchoice;
					t.slidersmenuvalueindex=g.slidersmenudropdownscroll_f+t.tn;
					if (  t.slidersmenuvalueindex <= t.slidersdropdownmax ) 
					{
						sliders_getnamefromvalue ( );
					}
					else
					{
						t.slidervaluename_s="";
					}
					pastebitmapfont(t.slidervaluename_s.Get(),t.slidersdropdownleft+10,t.slidersdropdowntop+5+(t.tn*16),2,255);
				}
			}
		}
	}

	// 080917 - renders all sprites put into batcher (fewer draw calls) 
	DrawSpriteBatcher();
}

void sliders_getchoice ( void )
{
	//  work out number of items in dropdown Box (  )
	t.sliderschoicemax=0;
	if (  t.slidersmenuvaluechoice == 1  )  t.sliderschoicemax = g.skymax;
	if (  t.slidersmenuvaluechoice == 2  )  t.sliderschoicemax = g.terrainstylemax;
	if (  t.slidersmenuvaluechoice == 3  )  t.sliderschoicemax = g.vegstylemax;
	if (  t.slidersmenuvaluechoice == 4  )  t.sliderschoicemax = 4;
	if (  t.slidersmenuvaluechoice == 5  )  t.sliderschoicemax = 3;
	if (  t.slidersmenuvaluechoice == 6  )  t.sliderschoicemax = 4; // grass techniques
	if (  t.slidersmenuvaluechoice == 7  )  t.sliderschoicemax = 2;//3; // 150917 - added PBR - now using pbroverride
	if (t.slidersmenuvaluechoice == 40)  t.sliderschoicemax = 5;//Weather

	//  If we are in the character kit
	if (  t.slidersmenuvaluechoice == 51  )  t.sliderschoicemax = g.characterkitbodymax;
	if (  t.slidersmenuvaluechoice == 52  )  t.sliderschoicemax = g.characterkitheadmax;
	if (  t.slidersmenuvaluechoice == 53  )  t.sliderschoicemax = t.characterkitfacialhairmax;
	if (  t.slidersmenuvaluechoice == 54 ) 
	{
		if (  t.characterkitcontrol.isMale  ==  1  )  t.sliderschoicemax = t.characterkithatmax; else t.sliderschoicemax = t.characterkitfemalehatmax;
	}
	if (  t.slidersmenuvaluechoice == 55  )  t.sliderschoicemax = t.characterkitweaponmax;
	if (  t.slidersmenuvaluechoice == 56  )  t.sliderschoicemax = t.characterkitprofilemax;

	//  (Dave) If we are in the importer then we can run this code
	if (  t.importer.importerActive  ==  1 ) 
	{
		if (  t.slidersmenuvaluechoice == 101  )  t.sliderschoicemax  =  t.importer.shaderFileCount;
		if (  t.slidersmenuvaluechoice >= 102 && t.slidersmenuvaluechoice<107  )  t.sliderschoicemax  =  2;
		if (  t.slidersmenuvaluechoice == 107  )  t.sliderschoicemax  =  5;
		if (  t.slidersmenuvaluechoice == 109  )  t.sliderschoicemax  =  4;
		if (  t.slidersmenuvaluechoice == 110  )  t.sliderschoicemax  =  6;
		if (  t.slidersmenuvaluechoice == 111  )  t.sliderschoicemax  =  3;
		if (  t.slidersmenuvaluechoice == 112  )  t.sliderschoicemax  =  4;
		if (  t.slidersmenuvaluechoice == 113  )  t.sliderschoicemax  =  2;
		if (  t.slidersmenuvaluechoice == 114  )  t.sliderschoicemax  =  t.importer.scriptFileCount;
		if (  t.slidersmenuvaluechoice == 115  )  t.sliderschoicemax  =  2;
		if (  t.slidersmenuvaluechoice == 116  )  t.sliderschoicemax  =  3;
		if (  t.slidersmenuvaluechoice == 117  )  t.sliderschoicemax  =  3;
	}
}

void sliders_getnamefromvalue ( void )
{
	//  takes slider panel index, and dropdown index and produces correct name representitives of values
	//  slidersmenuindex, slidersmenuvaluechoice, slidersmenuvalueindex
	t.slidervaluename_s="--";
	if (  t.slidersmenuvaluechoice == 1 ) 
	{
		if (  t.slidersmenuvalueindex <= ArrayCount(t.skybank_s) ) 
		{
			t.slidervaluename_s=t.skybank_s[t.slidersmenuvalueindex];
		}
	}
	if (  t.slidersmenuvaluechoice == 2 ) 
	{
		if (  t.slidersmenuvalueindex <= ArrayCount(t.terrainstylebank_s) ) 
		{
			t.slidervaluename_s=t.terrainstylebank_s[t.slidersmenuvalueindex];
		}
	}
	if (  t.slidersmenuvaluechoice == 3 ) 
	{
		if (  t.slidersmenuvalueindex <= ArrayCount(t.vegstylebank_s) ) 
		{
			t.slidervaluename_s=t.vegstylebank_s[t.slidersmenuvalueindex];
		}
	}
	if (  t.slidersmenuvaluechoice == 4 ) 
	{
		t.slidervaluename_s="";
		if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "HIGHEST";
		if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "HIGH";
		if (  t.slidersmenuvalueindex == 3  )  t.slidervaluename_s = "MEDIUM";
		if (  t.slidersmenuvalueindex == 4  )  t.slidervaluename_s = "LOWEST";
	}
	if (  t.slidersmenuvaluechoice == 5 ) 
	{
		t.slidervaluename_s="";
		if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "HIGHEST";
		if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "MEDIUM";
		if (  t.slidersmenuvalueindex == 3  )  t.slidervaluename_s = "LOWEST";
	}
	if (  t.slidersmenuvaluechoice == 6 ) 
	{
		// grass technique
		t.slidervaluename_s="";
		if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "HIGHEST";
		if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "HIGH";
		if (  t.slidersmenuvalueindex == 3  )  t.slidervaluename_s = "MEDIUM";
		if (  t.slidersmenuvalueindex == 4  )  t.slidervaluename_s = "LOWEST";
	}
	if (  t.slidersmenuvaluechoice == 7 ) 
	{
		t.slidervaluename_s="";
		if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "PRE-BAKE";
		if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "REALTIME";
	}

	if (t.slidersmenuvaluechoice == 40)
	{
		t.slidervaluename_s = "";
		if (t.slidersmenuvalueindex == 1)  t.slidervaluename_s = "None";
		if (t.slidersmenuvalueindex == 2)  t.slidervaluename_s = "Light Rain";
		if (t.slidersmenuvalueindex == 3)  t.slidervaluename_s = "Heavy Rain";
		if (t.slidersmenuvalueindex == 4)  t.slidervaluename_s = "Light Snow";
		if (t.slidersmenuvalueindex == 5)  t.slidervaluename_s = "Heavy Snow";
	}

	//  if we are in the character kit
	if (  t.slidersmenuvaluechoice == 51 ) 
	{
		if (  t.slidersmenuvalueindex <= ArrayCount(t.characterkitbodybank_s) ) 
		{
			t.slidervaluename_s=t.characterkitbodybank_s[t.slidersmenuvalueindex];
		}
	}
	if (  t.slidersmenuvaluechoice == 52 ) 
	{
		if (  t.slidersmenuvalueindex <= ArrayCount(t.characterkitheadbank_s) ) 
		{
			t.slidervaluename_s=t.characterkitheadbank_s[t.slidersmenuvalueindex];
		}
	}
	if (  t.slidersmenuvaluechoice == 53 ) 
	{
		if (  t.slidersmenuvalueindex <= ArrayCount(t.characterkitfacialhairbank_s) ) 
		{
			t.slidervaluename_s=t.characterkitfacialhairbank_s[t.slidersmenuvalueindex];
		}
	}
	if (  t.slidersmenuvaluechoice == 54 ) 
	{
		if (  t.slidersmenuvalueindex <= ArrayCount(t.characterkithatbank_s) ) 
		{
			if (  t.characterkitcontrol.isMale  ==  1 ) 
			{
				t.slidervaluename_s=t.characterkithatbank_s[t.slidersmenuvalueindex];
			}
			else
			{
				t.slidervaluename_s=t.characterkitfemalehatbank_s[t.slidersmenuvalueindex];
			}
		}
	}
	if (  t.slidersmenuvaluechoice == 55 ) 
	{
		if (  t.slidersmenuvalueindex <= ArrayCount(t.characterkitweaponbank_s) ) 
		{
			t.slidervaluename_s=t.characterkitweaponbank_s[t.slidersmenuvalueindex];
		}
	}
	if (  t.slidersmenuvaluechoice == 56 ) 
	{
		if (  t.slidersmenuvalueindex <= ArrayCount(t.characterkitprofilebank_s) ) 
		{
			t.slidervaluename_s=t.characterkitprofilebank_s[t.slidersmenuvalueindex];
		}
	}

	//  (Dave) If we are in the importer then we can run this code
	if (  t.importer.importerActive  ==  1 ) 
	{
		if (  t.slidersmenuvaluechoice == 101 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "entity_basic.fx";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "character_basic.fx";
			if (  t.slidersmenuvalueindex == 3  )  t.slidervaluename_s = "post-bloom.fx";
			if (  t.slidersmenuvalueindex == 4  )  t.slidervaluename_s = "post-none.fx";
			if (  t.slidersmenuvalueindex == 5  )  t.slidervaluename_s = "quad_basic.fx";
			if (  t.slidersmenuvalueindex == 6  )  t.slidervaluename_s = "sky_basic.fx";
			if (  t.slidersmenuvalueindex == 7  )  t.slidervaluename_s = "terrain_basic.fx";
			if (  t.slidersmenuvalueindex == 8  )  t.slidervaluename_s = "vegetation_basic.fx";
			if (  t.slidersmenuvalueindex == 9  )  t.slidervaluename_s = "water_basic.fx";
			if (  t.slidersmenuvalueindex == 10  )  t.slidervaluename_s = "weapon_basic.fx";
			if (  t.slidersmenuvalueindex == 11  )  t.slidervaluename_s = "weapon_legacy.fx";
		}
		if (  t.slidersmenuvaluechoice == 102 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "On";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "Off";
		}
		if (  t.slidersmenuvaluechoice == 103 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "Yes";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "No";
		}
		if (  t.slidersmenuvaluechoice  >= 104 && t.slidersmenuvaluechoice  <=  108 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "Yes";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "No";
		}
	}

	//  (Dave) If we are in the importer then we can run this code
	if (  t.importer.importerActive  ==  1 ) 
	{
		if (  t.slidersmenuvaluechoice == 101 ) 
		{
			for ( int tCount = 1 ; tCount<=  t.importer.shaderFileCount; tCount++ )
			{
				if (  t.slidersmenuvalueindex == tCount  )  t.slidervaluename_s = t.importerShaderFiles[tCount];
			}
		}
		if (  t.slidersmenuvaluechoice == 102 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "On";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "Off";
		}
		if (  t.slidersmenuvaluechoice == 103 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "Yes";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "No";
		}
		if (  t.slidersmenuvaluechoice  >= 104 && t.slidersmenuvaluechoice < 107 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "Yes";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "No";
		}
		if (  t.slidersmenuvaluechoice == 107 ) 
		{
			if (  t.slidersmenuvalueindex  ==  1 ) 
			{
				t.slidervaluename_s="No Objective";
			}
			else
			{
				t.slidervaluename_s=cstr("Objective ") + Str(t.slidersmenuvalueindex-1);
			}
		}
		if (  t.slidersmenuvaluechoice == 109 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "Front";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "Left";
			if (  t.slidersmenuvalueindex == 3  )  t.slidervaluename_s = "Right";
			if (  t.slidersmenuvalueindex == 4  )  t.slidervaluename_s = "Top";
		}
		if (  t.slidersmenuvaluechoice == 110 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "Box";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "Polygon";
			if (  t.slidersmenuvalueindex == 3  )  t.slidervaluename_s = "No collision";
			if (  t.slidersmenuvalueindex == 4  )  t.slidervaluename_s = "Limb One Box";
			if (  t.slidersmenuvalueindex == 5  )  t.slidervaluename_s = "Limb One Poly";
			if (  t.slidersmenuvalueindex == 6  )  t.slidervaluename_s = "Collision Boxes";
		}
		if (  t.slidersmenuvaluechoice == 111 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "None";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "Standard";
			if (  t.slidersmenuvalueindex == 3  )  t.slidervaluename_s = "Render last";
		}
		if (  t.slidersmenuvaluechoice == 112 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "Generic";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "Stone";
			if (  t.slidersmenuvalueindex == 3  )  t.slidervaluename_s = "Metal";
			if (  t.slidersmenuvalueindex == 4  )  t.slidervaluename_s = "Wood";
		}
		if (  t.slidersmenuvaluechoice == 113 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "Yes";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "No";
		}
		if (  t.slidersmenuvaluechoice == 114 ) 
		{
			for ( int tCount = 1 ; tCount<=  t.importer.scriptFileCount; tCount++ )
			{
				if (  t.slidersmenuvalueindex == tCount  )  t.slidervaluename_s = t.importerScriptFiles[tCount];
			}
		}
		if (  t.slidersmenuvaluechoice == 115 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "None";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "Swap Y and Z";
		}
		if (  t.slidersmenuvaluechoice == 116 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "None";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "Center Floor";
			if (  t.slidersmenuvalueindex == 3  )  t.slidervaluename_s = "Center All";
		}
		if ( t.slidersmenuvaluechoice == 117 ) 
		{
			if (  t.slidersmenuvalueindex == 1  )  t.slidervaluename_s = "No";
			if (  t.slidersmenuvalueindex == 2  )  t.slidervaluename_s = "Yes";
			if (  t.slidersmenuvalueindex == 3  )  t.slidervaluename_s = "Use Uber Anims";
		}
	}
}

void sliders_write (bool bOnlyVisualSettings )
{
	//  Control in-game or editing aspect in real-time (takes slidersmenuindex & thighlight)
	if (  t.slidersmenuindex == t.slidersmenunames.performance ) 
	{
		//  Performance Panel
	}
	if (  t.slidersmenuindex == t.slidersmenunames.visuals ) 
	{
		//  Visual Settings
		//  apply constraints if any
		if (  t.slidersmenuvalue[t.slidersmenuindex][1].value>99 ) 
		{
			t.slidersmenuvalue[t.slidersmenuindex][1].value=99;
		}
		if (  t.slidersmenuvalue[t.slidersmenuindex][2].value<t.slidersmenuvalue[t.slidersmenuindex][1].value+1 ) 
		{
			t.slidersmenuvalue[t.slidersmenuindex][2].value=t.slidersmenuvalue[t.slidersmenuindex][1].value+1;
		}
		//  apply setting to engine values
		t.visuals.FogNearest_f=t.slidersmenuvalue[t.slidersmenuindex][1].value*100.0;
		t.visuals.FogDistance_f=t.slidersmenuvalue[t.slidersmenuindex][2].value*500.0;
		t.visuals.FogR_f=t.slidersmenuvalue[t.slidersmenuindex][3].value*2.55;
		t.visuals.FogG_f=t.slidersmenuvalue[t.slidersmenuindex][4].value*2.55;
		t.visuals.FogB_f=t.slidersmenuvalue[t.slidersmenuindex][5].value*2.55;
		t.visuals.FogA_f=t.slidersmenuvalue[t.slidersmenuindex][6].value*2.55;
		t.visuals.AmbienceIntensity_f=t.slidersmenuvalue[t.slidersmenuindex][7].value*2.55;
		t.visuals.AmbienceRed_f=t.slidersmenuvalue[t.slidersmenuindex][8].value*2.55;
		t.visuals.AmbienceGreen_f=t.slidersmenuvalue[t.slidersmenuindex][9].value*2.55;
		t.visuals.AmbienceBlue_f=t.slidersmenuvalue[t.slidersmenuindex][10].value*2.55;
		t.visuals.SurfaceIntensity_f=t.slidersmenuvalue[t.slidersmenuindex][11].value/33.0;
		t.visuals.SurfaceRed_f=t.slidersmenuvalue[t.slidersmenuindex][12].value*2.55;
		t.visuals.SurfaceGreen_f=t.slidersmenuvalue[t.slidersmenuindex][13].value*2.55;
		t.visuals.SurfaceBlue_f=t.slidersmenuvalue[t.slidersmenuindex][14].value*2.55;
		t.visuals.SurfaceSunFactor_f=t.slidersmenuvalue[t.slidersmenuindex][15].value/100.0;
		t.visuals.Specular_f=t.slidersmenuvalue[t.slidersmenuindex][16].value/100.0;
		t.visuals.PostBrightness_f=(t.slidersmenuvalue[t.slidersmenuindex][17].value/100.0)-0.5;
		t.visuals.PostContrast_f=t.slidersmenuvalue[t.slidersmenuindex][18].value/30.0;
		if (!bOnlyVisualSettings)
		{
			t.storeprojectmodified = 1;
			t.visuals.refreshshaders = 1;
		}
	}
	if ( t.slidersmenuindex == t.slidersmenunames.water ) 
	{
		// Water slider
		g.gdefaultwaterheight = t.slidersmenuvalue[t.slidersmenuindex][1].value*10.0;
		t.visuals.WaterRed_f = t.slidersmenuvalue[t.slidersmenuindex][2].value*2.55;
		t.visuals.WaterGreen_f = t.slidersmenuvalue[t.slidersmenuindex][3].value*2.55;
		t.visuals.WaterBlue_f = t.slidersmenuvalue[t.slidersmenuindex][4].value*2.55;
		t.visuals.WaterWaveIntensity_f = t.slidersmenuvalue[t.slidersmenuindex][5].value*2.0;
		t.visuals.WaterTransparancy_f = t.slidersmenuvalue[t.slidersmenuindex][6].value / 100.0;
		t.visuals.WaterReflection_f = t.slidersmenuvalue[t.slidersmenuindex][7].value / 100.0;
		t.visuals.WaterReflectionSparkleIntensity = t.slidersmenuvalue[t.slidersmenuindex][8].value / 5.0;
		t.visuals.WaterFlowDirectionX = (t.slidersmenuvalue[t.slidersmenuindex][9].value - 50) / 10;
		t.visuals.WaterFlowDirectionY = (t.slidersmenuvalue[t.slidersmenuindex][10].value - 50) / 10;
		t.visuals.WaterDistortionWaves = t.slidersmenuvalue[t.slidersmenuindex][11].value / 1000.0;
		t.visuals.WaterSpeed1 = (t.slidersmenuvalue[t.slidersmenuindex][12].value - 100)*-1;
		t.visuals.WaterFlowSpeed = t.slidersmenuvalue[t.slidersmenuindex][13].value / 10.0;
		if (!bOnlyVisualSettings)
		{
			t.storeprojectmodified = 1;
			t.visuals.refreshshaders = 1;
		}
		//set the waterheight (fix for lua water height command to cover stuff in map editor)
		t.terrain.waterliney_f = g.gdefaultwaterheight;
	}
	if (  t.slidersmenuindex == t.slidersmenunames.camera ) 
	{
		//  Camera settings
		if (  t.slidersmenuvalue[t.slidersmenuindex][1].value<1 ) 
		{
			t.slidersmenuvalue[t.slidersmenuindex][1].value=1;
		}
		t.visuals.CameraFAR_f=t.slidersmenuvalue[t.slidersmenuindex][1].value*3000.0;
		t.visuals.CameraFOV_f=(20+((t.slidersmenuvalue[t.slidersmenuindex][2].value+0.0)/100.0)*90.0)/t.visuals.CameraASPECT_f;
		t.visuals.CameraFOVZoomed_f=t.slidersmenuvalue[t.slidersmenuindex][3].value/100.0;
		t.visuals.WeaponFOV_f=(20+((t.slidersmenuvalue[t.slidersmenuindex][4].value+0.0)/100.0)*90.0)/t.visuals.CameraASPECT_f;
		if (!bOnlyVisualSettings)
		{
			t.storeprojectmodified = 1;
			t.visuals.refreshshaders = 1;
		}
	}
	if (  t.slidersmenuindex == t.slidersmenunames.posteffects ) 
	{
		//  Post Effects
		t.visuals.bloommode=t.slidersmenuvalue[t.slidersmenuindex][1].value;
		t.visuals.VignetteRadius_f=t.slidersmenuvalue[t.slidersmenuindex][2].value/100.0;
		t.visuals.VignetteIntensity_f=t.slidersmenuvalue[t.slidersmenuindex][3].value/100.0;
		t.visuals.MotionDistance_f=t.slidersmenuvalue[t.slidersmenuindex][4].value/100.0;
		t.visuals.MotionIntensity_f=t.slidersmenuvalue[t.slidersmenuindex][5].value/100.0;
		t.visuals.DepthOfFieldDistance_f=t.slidersmenuvalue[t.slidersmenuindex][6].value/100.0;
		t.visuals.DepthOfFieldIntensity_f=t.slidersmenuvalue[t.slidersmenuindex][7].value/100.0;
		t.visuals.LightrayLength_f=t.slidersmenuvalue[t.slidersmenuindex][8].value/100.0;
		t.visuals.LightrayQuality_f=t.slidersmenuvalue[t.slidersmenuindex][9].value;
		t.visuals.LightrayDecay_f=t.slidersmenuvalue[t.slidersmenuindex][10].value/100.0;
		t.visuals.SAORadius_f=t.slidersmenuvalue[t.slidersmenuindex][11].value/100.0;
		t.visuals.SAOIntensity_f=t.slidersmenuvalue[t.slidersmenuindex][12].value/100.0;
		t.visuals.LensFlare_f=t.slidersmenuvalue[t.slidersmenuindex][13].value/100.0;
		if (!bOnlyVisualSettings)
		{
			t.storeprojectmodified = 1;
			t.visuals.refreshshaders = 1;
		}
	}
	if (  t.slidersmenuindex == t.slidersmenunames.qualitypanel ) 
	{
		//  Quality panel
		if (  t.slidersmenuvalue[t.slidersmenuindex][1].value>98 ) 
		{
			t.slidersmenuvalue[t.slidersmenuindex][1].value=98;
		}
		if (  t.slidersmenuvalue[t.slidersmenuindex][2].value <= t.slidersmenuvalue[t.slidersmenuindex][1].value ) 
		{
			t.slidersmenuvalue[t.slidersmenuindex][2].value=t.slidersmenuvalue[t.slidersmenuindex][1].value+1;
		}
		if (  t.slidersmenuvalue[t.slidersmenuindex][3].value <= t.slidersmenuvalue[t.slidersmenuindex][2].value ) 
		{
			t.slidersmenuvalue[t.slidersmenuindex][3].value=t.slidersmenuvalue[t.slidersmenuindex][2].value+1;
		}
		t.visuals.TerrainLOD1_f=t.slidersmenuvalue[t.slidersmenuindex][1].value*100.0;
		t.visuals.TerrainLOD2_f=t.slidersmenuvalue[t.slidersmenuindex][2].value*100.0;
		t.visuals.TerrainLOD3_f=t.slidersmenuvalue[t.slidersmenuindex][3].value*100.0;
		t.visuals.TerrainSize_f=t.slidersmenuvalue[t.slidersmenuindex][4].value;
		if (!bOnlyVisualSettings && t.visuals.VegQuantity_f != t.slidersmenuvalue[t.slidersmenuindex][5].value  )  t.visuals.refreshvegetation = 1;
		if (!bOnlyVisualSettings && t.visuals.VegWidth_f != t.slidersmenuvalue[t.slidersmenuindex][6].value  )  t.visuals.refreshvegetation = 1;
		if (!bOnlyVisualSettings && t.visuals.VegHeight_f != t.slidersmenuvalue[t.slidersmenuindex][7].value  )  t.visuals.refreshvegetation = 1;
		t.visuals.VegQuantity_f=t.slidersmenuvalue[t.slidersmenuindex][5].value;
		t.visuals.VegWidth_f=t.slidersmenuvalue[t.slidersmenuindex][6].value;
		t.visuals.VegHeight_f=t.slidersmenuvalue[t.slidersmenuindex][7].value;
		if (!bOnlyVisualSettings)
		{
			t.storeprojectmodified = 1;
			t.visuals.refreshshaders = 1;
		}
	}
	if (  t.slidersmenuindex == t.slidersmenunames.worldpanel ) 
	{
		if (  t.slidersmenuvalue[t.slidersmenuindex][1].value<1  )  t.slidersmenuvalue[t.slidersmenuindex][1].value = 1;
		if (  t.slidersmenuvalue[t.slidersmenuindex][2].value<1  )  t.slidersmenuvalue[t.slidersmenuindex][2].value = 1;
		if (  t.slidersmenuvalue[t.slidersmenuindex][3].value<1  )  t.slidersmenuvalue[t.slidersmenuindex][3].value = 1;
		if (  t.visuals.skyindex != t.slidersmenuvalue[t.slidersmenuindex][1].value ) 
		{
			t.visuals.skyindex = t.slidersmenuvalue[t.slidersmenuindex][1].value;
			if (!bOnlyVisualSettings)
			{
				t.storeprojectmodified = 1;
				t.visuals.refreshskysettings = 1;
				t.visuals.refreshshaders = 1;
			}
		}
		if (  t.visuals.terrainindex != t.slidersmenuvalue[t.slidersmenuindex][2].value ) 
		{
			t.visuals.terrainindex=t.slidersmenuvalue[t.slidersmenuindex][2].value;
			if (!bOnlyVisualSettings)
			{
				t.storeprojectmodified = 1;
				t.visuals.refreshterraintexture = 1;
				t.visuals.refreshshaders = 1;
			}
		}
		if (  t.visuals.vegetationindex != t.slidersmenuvalue[t.slidersmenuindex][3].value ) 
		{
			t.visuals.vegetationindex=t.slidersmenuvalue[t.slidersmenuindex][3].value;
			if (!bOnlyVisualSettings)
			{
				t.storeprojectmodified = 1;
				t.visuals.refreshvegtexture = 1;
			}
		}
		if (t.visuals.iEnvironmentWeather+1 != t.slidersmenuvalue[t.slidersmenuindex][4].value)
		{
			t.visuals.iEnvironmentWeather = t.slidersmenuvalue[t.slidersmenuindex][4].value-1;
			if (!bOnlyVisualSettings)
			{
				t.storeprojectmodified = 1;
			}
		}
	}
	if (  t.slidersmenuindex == t.slidersmenunames.graphicoptions ) 
	{
		//  Graphic Options
		t.visuals.reflectionmode=t.slidersmenuvalue[t.slidersmenuindex][1].value;
		t.visuals.shadowmode=t.slidersmenuvalue[t.slidersmenuindex][2].value;
		t.visuals.lightraymode=t.slidersmenuvalue[t.slidersmenuindex][3].value;
		if (  t.visuals.vegetationmode != t.slidersmenuvalue[t.slidersmenuindex][4].value ) 
		{
			if (!bOnlyVisualSettings)
			{
				t.storeprojectmodified = 1;
				t.visuals.refreshvegetation = 1;
			}
		}
		t.visuals.vegetationmode = t.slidersmenuvalue[t.slidersmenuindex][4].value;
		if (  t.visuals.occlusionvalue != t.slidersmenuvalue[t.slidersmenuindex][5].value ) 
		{
			t.visuals.occlusionvalue = t.slidersmenuvalue[t.slidersmenuindex][5].value;
			if (!bOnlyVisualSettings)
			{
				t.storeprojectmodified = 1;
				CPU3DSetPolyCount(t.visuals.occlusionvalue);
			}
		}
		t.visuals.debugvisualsmode=t.slidersmenuvalue[t.slidersmenuindex][6].value;
		if (!bOnlyVisualSettings)
		{
			t.visuals.refreshshaders = 1;
		}
	}
	if (  t.slidersmenuindex == t.slidersmenunames.shaderoptions ) 
	{
		if (  t.visuals.shaderlevels.terrain != t.slidersmenuvalue[t.slidersmenuindex][1].value ) 
		{
			t.visuals.shaderlevels.terrain=t.slidersmenuvalue[t.slidersmenuindex][1].value;
			if (!bOnlyVisualSettings)
			{
				t.storeprojectmodified = 1;
				t.visuals.refreshshaders = 1;
			}
		}
		if (  t.visuals.shaderlevels.entities != t.slidersmenuvalue[t.slidersmenuindex][2].value ) 
		{
			t.visuals.shaderlevels.entities=t.slidersmenuvalue[t.slidersmenuindex][2].value;
			if (!bOnlyVisualSettings)
			{
				t.storeprojectmodified = 1;
				t.visuals.refreshshaders = 1;
			}
		}
		if (  t.visuals.shaderlevels.vegetation != t.slidersmenuvalue[t.slidersmenuindex][3].value ) 
		{
			t.visuals.shaderlevels.vegetation=t.slidersmenuvalue[t.slidersmenuindex][3].value;
			if (!bOnlyVisualSettings)
			{
				t.storeprojectmodified = 1;
				t.visuals.refreshshaders = 1;
			}
		}
		if (  t.visuals.shaderlevels.lighting != t.slidersmenuvalue[t.slidersmenuindex][4].value ) 
		{
			if (!bOnlyVisualSettings)
			{
				t.storeprojectmodified = 1;
				t.visuals.shaderlevels.lighting = t.slidersmenuvalue[t.slidersmenuindex][4].value;
				//  the above subroutine can change lighting back to REALTIME
				t.slidersmenuvalue[t.slidersmenuindex][4].value = t.visuals.shaderlevels.lighting;
				if (t.slidersmenuvalue[t.slidersmenuindex][4].value >= 2)
				{
					t.slidersmenuvaluechoice = t.slidersmenuvalue[t.slidersmenuindex][4].gadgettypevalue;
					t.slidersmenuvalueindex = t.slidersmenuvalue[t.slidersmenuindex][4].value;
					sliders_getnamefromvalue();
					t.slidersmenuvalue[t.slidersmenuindex][4].value_s = t.slidervaluename_s;
				}
				t.visuals.refreshshaders = 1;
			}
		}
		if (  (t.visuals.DistanceTransitionStart_f != t.slidersmenuvalue[t.slidersmenuindex][5].value*100.0) || (t.visuals.DistanceTransitionRange_f != t.slidersmenuvalue[t.slidersmenuindex][6].value*10.0) ) 
		{
			t.visuals.DistanceTransitionStart_f=t.slidersmenuvalue[t.slidersmenuindex][5].value*100.0;
			t.visuals.DistanceTransitionRange_f=t.slidersmenuvalue[t.slidersmenuindex][6].value*10.0;
			if (!bOnlyVisualSettings)
			{
				t.storeprojectmodified = 1;
				t.visuals.refreshshaders = 1;
			}
		}
	}
	if (  t.slidersmenuindex == t.importer.properties1Index ) 
	{
		if ( t.whichmenuitem==2 )
		{
			// changed shader while in model importer
			importer_changeshader ( t.slidersmenuvalue[t.slidersmenuindex][2].value_s.Get() );
		}
	}
}

void sliders_scope_draw ( void )
{
	//  draw scope (before slider panels)
	if (  t.gunzoommode != 0 ) 
	{
		if (  g.firemodes[t.gunid][0].zoomscope>0 ) 
		{
			t.timgbase=g.firemodes[t.gunid][0].zoomscope;
			MAXSprite (  t.timgbase,-10000,-10000,t.timgbase );
			t.timgwidth_f=ImageWidth(t.timgbase) ; t.timgheight_f=ImageHeight(t.timgbase);
			t.timgratio_f=t.timgwidth_f/t.timgheight_f;
			t.tsprwidth_f=GetDisplayHeight()*t.timgratio_f;
			SizeSprite ( t.timgbase, t.tsprwidth_f, GetDisplayHeight()+1 );
			PasteSprite ( t.timgbase, (t.tsprwidth_f-GetDisplayWidth())/-2, 0 );
		}
	}
}

float SlidersAdjustValue ( float value_f, float minFrom_f, float maxFrom_f, float minTo_f, float maxTo_f )
{
	float mappedValue_f = 0;
	//  (Dave) Added to allow custom values
	mappedValue_f = minTo_f + ( maxTo_f - minTo_f ) * ( ( value_f - minFrom_f ) / ( maxFrom_f - minFrom_f ) );
	return mappedValue_f;
}

//prevent that slider bar is in other panels when lua commands are used to set the values
float SlidersCutExtendedValues(float value) 
{
	if (value > 100) return 100;
	else if (value < 0) return 0;
	else return value;
}