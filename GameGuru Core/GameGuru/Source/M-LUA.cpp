//----------------------------------------------------
//--- GAMEGURU - M-LUA
//----------------------------------------------------

// Includes
#include "stdafx.h"
#include "gameguru.h"
#include "GGTerrain/GGTerrain.h"

#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

// Externs
#ifdef VRTECH
extern GGVR_PlayerData GGVR_Player;
#endif

// 
//  LUA Module
// 

void lua_init ( void )
{
	//  Clear lua bank
	g.luabankmax=0 ; Dim (  t.luabank_s,g.luabankmax  );

	//  Load the common scripts
	if (  g.luabankmax == 0 ) 
	{
		g.luabankmax=2;
		Dim (  t.luabank_s,g.luabankmax  );
		t.tfile_s="scriptbank\\global.lua";
		t.luabank_s[1]=t.tfile_s ; t.r=LoadLua(t.tfile_s.Get());
		t.strwork = "" ; t.strwork = t.strwork + "Loaded "+t.tfile_s;
		timestampactivity(0, t.strwork.Get() );
		//t.tfile_s="scriptbank\\music.lua";
		//t.luabank_s[2]=t.tfile_s ; t.r=LoadLua(t.tfile_s.Get());
		//t.strwork = "" ; t.strwork = t.strwork + "Loaded "+t.tfile_s;
		//timestampactivity(0, t.strwork.Get() );
	}

	//  Ensure entity elements are set to a LUA first run state
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.entityelement[t.e].lua.firsttime=0;
	}

	//  Reset image system
	for ( t.t = 0 ; t.t<=  99; t.t++ )
	{
		if (  ImageExist(g.promptimageimageoffset+t.t) == 1  )  DeleteImage (  g.promptimageimageoffset+t.t );
	}
	t.promptimage.alignment=0;
	t.promptimage.x=0;
	t.promptimage.y=0;
	t.promptimage.img=0;
	t.promptimage.show=0;

	//  Each time game is paused, add up so we can 'freeze' the LUA Timer value
	t.aisystem.cumilativepauses=0;

	// 100316 - ensure GameLoopInit is called at start of each game session
	t.playercontrol.gameloopinitflag = 10;

	// flags to reset before LUA activity starts
	g.luaactivatemouse = 0;
	g.luacameraoverride = 0;

	// reset some LUA globals
	g.projectileEventType_explosion = 0;
	g.projectileEventType_name = "";
	g.projectileEventType_x = 0;
	g.projectileEventType_y = 0;
	g.projectileEventType_z = 0;
	g.projectileEventType_radius = 0;
	g.projectileEventType_damage = 0;
	g.projectileEventType_entityhit = 0;

	//LB: more clearances
	t.plrkeyForceKeystate = 0;

	// Clear player inventory uere (needs to be move game init, otherwise plr inv will disappear for each new level)
	t.inventoryContainers.clear();
	t.inventoryContainers.push_back("inventory:player");
	t.inventoryContainers.push_back("inventory:hotkeys");
	for ( int n=0; n<MAX_INVENTORY_CONTAINERS;n++)
		t.inventoryContainer[n].clear();

	// reset prompt at VERY start so restoregame script can output debug prompts in standalone
	t.luaglobal.scriptprompttype = 0;
	t.luaglobal.scriptprompt_s = "";
	t.luaglobal.scriptprompttime = 0;
	t.luaglobal.scriptprompttextsize = 0;
	t.luaglobal.scriptprompt3dtime = 0;
	strcpy (t.luaglobal.scriptprompt3dtext, "");
}

void lua_loadscriptin ( void )
{
	// gets entity ready to run AI system
	if ( t.e > 0 ) 
	{
		if ( Len(t.entityelement[t.e].eleprof.aimain_s.Get())>0 ) 
		{
			t.tscriptname_s=t.entityelement[t.e].eleprof.aimain_s;
			if (  strcmp ( Right(t.tscriptname_s.Get(),4) , ".fpi" ) == 0  ) { t.strwork = "" ; t.strwork = t.strwork + Left(t.tscriptname_s.Get(),Len(t.tscriptname_s.Get())-4)+".lua" ; t.tscriptname_s = t.strwork; }
			if ( strcmp ( Lower(Right(t.tscriptname_s.Get(),4)) , ".lua" ) != 0 ) 
			{
				t.tscriptname_s=t.tscriptname_s+".lua";
			}

			cstr script_name = "";
			//if (strnicmp(t.tscriptname_s.Get(), "projectbank", 11) != NULL) 
			script_name = "scriptbank\\";
			script_name += t.tscriptname_s;

			t.tfile_s = ""; t.tfile_s = t.tfile_s + script_name;// "scriptbank\\" + t.tscriptname_s;
			if (FileExist(t.tfile_s.Get()) == 0)
			{
				//LB: scripts can be moved and so not found, in this case have the engine find the parent behavior and load that instead
				int entid = t.entityelement[t.e].bankindex;
				t.tscriptname_s = t.entityprofile[entid].aimain_s;

				script_name = "";
				//if (strnicmp(t.tscriptname_s.Get(), "projectbank", 11) != NULL) 
				script_name = "scriptbank\\";
				script_name += t.tscriptname_s;

				t.tfile_s = ""; t.tfile_s = t.tfile_s + script_name;// "scriptbank\\" + t.tscriptname_s;
			}
			if ( FileExist(t.tfile_s.Get()) == 1 )
			{
				t.tfound=0;
				for ( t.i = 1 ; t.i<=  g.luabankmax; t.i++ )
				{
					if (  t.luabank_s[t.i] == t.tfile_s  )  t.tfound = 1;
				}
				if (  t.tfound == 0 ) 
				{
					t.r=LoadLua(t.tfile_s.Get());
					if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );
					if (  t.r == 0 ) 
					{
						t.strwork = "" ; t.strwork = t.strwork + "Loaded "+t.tfile_s;
						timestampactivity(0, t.strwork.Get() );
						++g.luabankmax;
						Dim (  t.luabank_s,g.luabankmax  );
						t.luabank_s[g.luabankmax]=t.tfile_s;
						t.tfound=1;
					}
					else
					{
						t.strwork = ""; t.strwork = t.strwork + "Failed to LoadLua ( " + t.tfile_s + " ):"+Str(t.r);
						timestampactivity(0, t.strwork.Get() );
						//       `1; Error occurred running the script
						//       `2; Syntax error
						//       `3; Required memory could not be allocated
						//       `4; Error with error reporting mechanism. (Don't ask!)
						//       `5; Error reading or opening script file (right filename?)
					}
				}
				else
				{
					//  already loaded previously
				}
				if (  t.tfound == 1 ) 
				{
					t.entityelement[t.e].eleprof.aimainname_s=Left(t.tscriptname_s.Get(),Len(t.tscriptname_s.Get())-4);
					t.entityelement[t.e].eleprof.aimain=1;
					t.entityelement[t.e].eleprof.aipreexit=-1;
					t.entityelement[t.e].lua.plrinzone=-1;
					t.entityelement[t.e].lua.entityinzone=0;
					t.entityelement[t.e].lua.flagschanged=1;
					t.entityelement[t.e].lua.dynamicavoidance=0;
					t.entityelement[t.e].lua.dynamicavoidancestuckclock = 0.0f;
					t.entityelement[t.e].lua.interuptpath = 0;
				}
			}
		}
	}
}

void lua_scanandloadactivescripts ( void )
{
	//  Scan all active entities and load in used scripts
	//  no nesting as cannot resolve folder nests with global naming convention
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.entityelement[t.e].eleprof.aimain=0;
		t.entityelement[t.e].eleprof.aipreexit=-1;
		if (  t.entityelement[t.e].bankindex>0 && t.entityelement[t.e].staticflag == 0 ) 
		{
			//  AI MAIN SCRIPT
			lua_loadscriptin ( );
		}
	}
}

void lua_free ( void )
{
	//  Reset entire LUA environment
	LuaReset (  );

	//  Clear lua bank
	for ( t.i = 1 ; t.i <= g.luabankmax ; t.i++ ) t.luabank_s[t.i]="" ; 
	g.luabankmax=0;
}

void lua_initscript ( void )
{
	// call the INIT functions of all entities
	if ( t.entityelement[t.e].active != 0 || t.entityelement[t.e].eleprof.spawnatstart == 0 )
	{
		//PE: Particles also need to have updated status before first call to _main.
		if (  t.entityelement[t.e].eleprof.aimain == 1 || t.entityprofile[t.entityelement[t.e].bankindex].ismarker == 10)
		{
			// 151016 - need to ensure g_Entity globals are in place BEFORE INIT, so call update function
			t.tfrm=0; t.tobj = t.entityelement[t.e].obj;
			lua_ensureentityglobalarrayisinitialised();

			// first try initialising with a name string
			t.strwork = ""; t.strwork = t.strwork + t.entityelement[t.e].eleprof.aimainname_s.Get()+"_init_name";
			LuaSetFunction ( t.strwork.Get() ,2,0 );
			t.tentityname_s = t.entityelement[t.e].eleprof.name_s;
			LuaPushInt (  t.e  ); LuaPushString (  t.tentityname_s.Get()  );
			LuaCallSilent (  );

			// then try passing in location of this script (needed for relative discovery of BYC file!)
			t.strwork = ""; t.strwork = t.strwork + t.entityelement[t.e].eleprof.aimainname_s.Get() + "_init_file";
			LuaSetFunction (t.strwork.Get(), 2, 0);
			char pRemoveLUAEXT[MAX_PATH];
			strcpy(pRemoveLUAEXT, t.entityelement[t.e].eleprof.aimain_s.Get());
			if (strlen(pRemoveLUAEXT) > 4)
			{
				pRemoveLUAEXT[strlen(pRemoveLUAEXT) - 4] = 0;
			}
			LuaPushInt (t.e); 
			LuaPushString(pRemoveLUAEXT);
			LuaCallSilent ();

			// then try initialising without the name parameter
			t.strwork = ""; t.strwork = t.strwork + t.entityelement[t.e].eleprof.aimainname_s.Get()+"_init";
			LuaSetFunction ( t.strwork.Get() ,1,0 );
			LuaPushInt (  t.e  );
			LuaCallSilent (  );

			//Check if we use properties variables.
			char tmp[MAX_PATH];
			strcpy(tmp, t.entityelement[t.e].eleprof.aimainname_s.Get());
			char* pFindSlash = strrchr(tmp, '\\');
			if (pFindSlash)
				strcpy(tmp, pFindSlash + 1);
			strcat(tmp, "_properties(");
			if (pestrcasestr(t.entityelement[t.e].eleprof.soundset4_s.Get(), tmp)) 
			{
				//Found one , parse and sent variables to script.
				lua_execute_properties_variable(t.entityelement[t.e].eleprof.soundset4_s.Get());
			}
		}
	}
}

#ifdef VRTECH
void lua_execute_properties_variable(char *string)
{
	char tmp[4096];
	ZeroMemory(tmp, 4095);
	strcpy(tmp, string); //dont change original.
	char *cmd, *value;
	bool bFunctionSet = false;
	
	char *find = (char *)pestrcasestr(tmp, "(");
	int params = 0;
	while ((find = (char *)pestrcasestr(find, "\""))) {
		find++;
		params++;
	}
	params *= 0.5;

	find = (char *) pestrcasestr(tmp, "(");
	if (find) {
		cmd = &tmp[0];
		find[0] = 0;
		find++;
		while ((find = (char *)pestrcasestr(find, "\""))) {
			char *find2 = (char *)pestrcasestr(find+1, "\"");
			if (find2) {
				int type = find[-1] - '0';
				find2[0] = 0;

				if (type >= 0 && type <= 9) {
					if (!bFunctionSet) {
						LuaSetFunction(cmd, params+1, 0);
						LuaPushInt(t.e);
						bFunctionSet = true;
					}
					if (type == 1) {
						LuaPushFloat( atof(find+1) );
					}
					else if (type == 2 || type == 7) {
						LuaPushString(find+1);
					}
					else {
						//This include type == 3 bool. sent as int.
						LuaPushInt(atoi(find+1));
					}
				}
				find = find2 + 1;
			}
			else {
				find++;
			}
		}
		if (bFunctionSet) {
			LuaCallSilent();
		}
	}
}
#endif

void lua_launchallinitscripts ( void )
{
	// call the INIT function of the GLOBAL GAMELOOP INIT
	if ( t.playercontrol.gameloopinitflag == 10 )
	{
		// when game is first started a new, erase all temp level states and player global stats in LUA
		if (t.game.firstlevelinitializesanygameprojectlua == 123)
		{
			t.game.firstlevelinitializesanygameprojectlua = 0;
			//did not locate the files to be deleted
			//LuaSetFunction ("GameLoopClearGlobalStates", 0, 0);
			//LuaCall();
			for (int i = -1; i < 999; i++)
			{
				cstr pFile = "";
				if ( i == -1) pFile = "savegames\\gameslot0-globals.dat";
				else pFile = cstr("savegames\\gameslot0-") + cstr(i) + ".dat";
				char pRealFile[MAX_PATH];
				strcpy(pRealFile, pFile.Get());
				GG_GetRealPath(pRealFile, 0);
				if(FileExist(pRealFile)==1)
				{
					DeleteFileA(pRealFile);
				}
			}
		}

		// calls Init once per game (level?)
		LuaSetFunction ( "GameLoopInit", 5, 0 );
		extern bool bInvulnerableMode;
		LuaPushInt ( (int)bInvulnerableMode );
		LuaPushInt ( t.playercontrol.startstrength );
		LuaPushInt ( t.playercontrol.regenrate );
		LuaPushInt ( t.playercontrol.regenspeed ); 
		LuaPushInt ( t.playercontrol.regendelay ); 
		LuaCall();

		// sets a decrement of 9 cycles before calling GameLoop function
		t.playercontrol.gameloopinitflag = 9;
	}

	//  launch scripts attached to entities
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		if (  t.entityelement[t.e].bankindex>0 ) 
		{
			lua_initscript ( );
		}
	}

	// launch music script
	//LuaSetFunction ( "music_init", 0, 0 );
	//LuaCallSilent ( );

	#ifdef WICKEDENGINE
	// clear entity vis list for new test level/game level run
	entity_lua_getentityplrvisible_clear();
	#endif
}

void lua_loop_begin ( void )
{
	// Spawn 1 item from the queue
	entity_lua_spawnifusedfromqueue();
	entity_lua_activateifusedfromqueue();

	// Write LUA globals
	LuaSetInt (  "g_GameStateChange", t.luaglobal.gamestatechange );
	if ( ObjectExist(t.aisystem.objectstartindex)==1 )
	{
		LuaSetFloat (  "g_PlayerPosX",ObjectPositionX(t.aisystem.objectstartindex) );
		LuaSetFloat (  "g_PlayerPosY",ObjectPositionY(t.aisystem.objectstartindex) );
		LuaSetFloat (  "g_PlayerPosZ",ObjectPositionZ(t.aisystem.objectstartindex) );
	}
	LuaSetFloat ( "g_PlayerAngX", wrapangleoffset(CameraAngleX(0)) );
	LuaSetFloat ( "g_PlayerAngY", wrapangleoffset(CameraAngleY(0)) );
	LuaSetFloat ( "g_PlayerAngZ", wrapangleoffset(CameraAngleZ(0)) );
	LuaSetInt (  "g_PlayerObjNo", t.aisystem.objectstartindex );
	LuaSetInt (  "g_PlayerHealth", t.player[t.plrid].health );

	//extern int g_iUserGlobal;
	//LuaSetInt ("g_UserGlobal", g_iUserGlobal);
	//g_iUserGlobal = LuaGetInt ("g_UserGlobal");

	LuaSetInt (  "g_PlayerLives", t.player[t.plrid].lives );
	LuaSetFloat (  "g_PlayerFlashlight", t.playerlight.flashlightcontrol_f );
	LuaSetInt (  "g_PlayerGunCount", t.guncollectedcount );
	LuaSetInt("g_PlayerGunID", t.gunid);
	if ( t.gunid > 0 )
		LuaSetString("g_PlayerGunName", t.gun[t.gunid].name_s.Get());
	else
		LuaSetString("g_PlayerGunName", "");
	LuaSetInt (  "g_PlayerGunMode", t.gunmode );
	int iGunIsFiring = 0;
	if ( t.gunmode >= 101 && t.gunmode <= 120 ) iGunIsFiring = 1;
	if ( t.gunmode >= 1020 && t.gunmode <= 1023 ) iGunIsFiring = 2;
	LuaSetInt (  "g_PlayerGunFired", iGunIsFiring );
	LuaSetInt (  "g_PlayerGunAmmoCount", t.slidersmenuvalue[t.slidersmenunames.weapon][1].value );
	LuaSetInt (  "g_PlayerGunClipCount", t.slidersmenuvalue[t.slidersmenunames.weapon][2].value );

	//PE: These hardcoded and new "Total Ammo Remaining" , are missing in g_UserGlobal.
	char pUserDefinedGlobal[256];
	sprintf(pUserDefinedGlobal, "g_UserGlobal['%s']", "Gun Ammo Remaining");
	LuaSetInt(pUserDefinedGlobal, t.slidersmenuvalue[t.slidersmenunames.weapon][1].value + t.slidersmenuvalue[t.slidersmenunames.weapon][2].value);
	sprintf(pUserDefinedGlobal, "g_UserGlobal['%s']", "Health Remaining");
	LuaSetInt(pUserDefinedGlobal, t.player[t.plrid].health);
	sprintf(pUserDefinedGlobal, "g_UserGlobal['%s']", "Lives Remaining");
	LuaSetInt(pUserDefinedGlobal, t.player[t.plrid].lives);

	LuaSetInt (  "g_PlayerGunZoomed", t.gunzoommode );
	LuaSetInt (  "g_Time", Timer()-t.aisystem.cumilativepauses );
	LuaSetFloat (  "g_TimeElapsed", g.timeelapsed_f );
	LuaSetInt (  "g_PlayerThirdPerson", t.playercontrol.thirdperson.enabled );
	LuaSetInt (  "g_PlayerController", g.gxbox );
	//int iPlayerFOVPerc = (((t.visuals.CameraFOV_f*t.visuals.CameraASPECT_f)-20.0)/180.0)*100.0; match with SetPlayerFOV calc!
	//t.visuals.CameraFOV_f = (20 + ((g_fLastKnownFOVin + 0.0) / 114.0) * 180.0) / t.visuals.CameraASPECT_f;
	int iPlayerFOVPerc = (((t.visuals.CameraFOV_f * t.visuals.CameraASPECT_f) - 20.0) / 180.0) * 114.0f;// 100.0;
	LuaSetInt (  "g_PlayerFOV", iPlayerFOVPerc );
	LuaSetInt (  "g_PlayerLastHitTime", t.playercontrol.regentime );
	LuaSetInt (  "g_PlayerDeadTime", t.playercontrol.deadtime );

	//  Quick detection of E key
	if (  t.aisystem.processplayerlogic == 1 ) 
	{
		if (  t.player[t.plrid].health>0  )  t.tKeyPressE = KeyState(18); else t.tKeyPressE = 0;
	}
	else
	{
		t.tKeyPressE=KeyState(g.keymap[18]);
	}
	if ( g.gxbox == 1 ) 
	{
		if ( JoystickFireD() == 1 )  
			t.tKeyPressE = 1;
	}
	#ifdef VRTECH
	if ( g.vrglobals.GGVREnabled > 0 && g.vrglobals.GGVRUsingVRSystem == 1 )
	{
		if ( GGVR_RightController_Trigger() > 0.9f )
			t.tKeyPressE = 1;
	}
	#endif
	LuaSetInt ( "g_KeyPressE",t.tKeyPressE );
	LuaSetInt ( "g_KeyPressQ",KeyState(g.keymap[16]) );

	// 241115 - other common keys which might require SIMULTANEOUS detection (bot control)
	LuaSetInt ( "g_KeyPressW", KeyState(g.keymap[17]) );
	LuaSetInt ( "g_KeyPressA", KeyState(g.keymap[30]) );
	LuaSetInt ( "g_KeyPressS", KeyState(g.keymap[31]) );
	LuaSetInt ( "g_KeyPressD", KeyState(g.keymap[32]) );
	LuaSetInt ( "g_KeyPressR", KeyState(g.keymap[19]) );
	LuaSetInt ( "g_KeyPressF", KeyState(g.keymap[33]) );
	LuaSetInt ( "g_KeyPressC", KeyState(g.keymap[46]) );
	//LuaSetInt ( "g_KeyPressJ", !!done in player control code!! );
	LuaSetInt ( "g_KeyPressSPACE", KeyState(g.keymap[57]) );

	// shift key for running/etc
	#ifdef VRTECH
	int tKeyPressShift = 0;
	if ( KeyState(g.keymap[42]) ) tKeyPressShift = 1;
	if ( KeyState(g.keymap[54]) ) tKeyPressShift = 1;
	if ( g.vrglobals.GGVREnabled > 0 && g.vrglobals.GGVRUsingVRSystem == 1 )
	{
		if ( GGVR_RightController_Grip() == 1 )
			tKeyPressShift = 1;
	}
	LuaSetInt ( "g_KeyPressSHIFT", tKeyPressShift );
	#else
	LuaSetInt ( "g_KeyPressSHIFT", KeyState(g.keymap[42]) | KeyState(g.keymap[54]) );
	#endif
	#ifdef WICKEDENGINE
	static int iResetMouseWheel = 0;
	#endif
	if ( g.luaactivatemouse != 0 )
	{
		// remains one (unless in-game menu, in which case it is set to 2 below when in standalone in-game menu (and read by playe control script to disable plr input for mouse data))
		g.luaactivatemouse = 1;

		g.LUAMouseX += MouseMoveX();
		g.LUAMouseY += MouseMoveY();
		if ( g.LUAMouseX < 0.0f ) g.LUAMouseX = 0.0f;
		if ( g.LUAMouseY < 0.0f ) g.LUAMouseY = 0.0f;
		if ( g.LUAMouseX >= GetDisplayWidth() ) g.LUAMouseX = GetDisplayWidth()-1;
		if ( g.LUAMouseY >= GetDisplayHeight() ) g.LUAMouseY = GetDisplayHeight()-1;

		// comes from mouse or VR Controller laser
		float fFinalPercX = ( g.LUAMouseX / GetDisplayWidth() ) * 100.0f;
		float fFinalPercY = ( g.LUAMouseY / GetDisplayHeight() ) * 100.0f;

		//PE: bug , we can only use MouseMoveZ() one time , as it return the delta and then reset.
		float fFinalWheel = MouseMoveZ();

		LuaSetInt("g_MouseWheel", (int) fFinalWheel ); //PE: Moved here so delta is not lost.

		#ifdef WICKEDENGINE
		iResetMouseWheel = 6; //PE: Reset when we leave menu and go back to the game. need 6 frame before everything is in sync.
		extern DBPRO_GLOBAL int			g_iMouseLocalZ;
		g_iMouseLocalZ = 0;
		#endif

		#ifdef VRTECH
		if (g.vrglobals.GGVREnabled > 0 && g.vrglobals.GGVRUsingVRSystem == 1)
		{
			// also only do this if NOT in in-game menu
			bool bUsingInGameMenu = false;
			extern int g_iInGameMenuState;
			if (t.game.gameisexe == 1 && g_iInGameMenuState == 1)
			{
				bUsingInGameMenu = true;
				g.luaactivatemouse = 2;
			}
			if (GGVR_Player.LaserGuideActive > 0 && bUsingInGameMenu==false)
			{
				fFinalPercX = -10000.0f;
				fFinalPercY = -10000.0f;
				fFinalWheel = 0.0f;
				sObject* pLaserObject = g_ObjectList [ GGVR_Player.LaserGuideActive ];
				WickedCall_SetObjectCastShadows(pLaserObject, false);
				MoveObject(GGVR_Player.LaserGuideActive, -100);
				float fX = pLaserObject->position.vecPosition.x;
				float fY = pLaserObject->position.vecPosition.y;
				float fZ = pLaserObject->position.vecPosition.z;
				MoveObject(GGVR_Player.LaserGuideActive, 200);
				float fNewX = pLaserObject->position.vecPosition.x;
				float fNewY = pLaserObject->position.vecPosition.y;
				float fNewZ = pLaserObject->position.vecPosition.z;
				MoveObject(GGVR_Player.LaserGuideActive, -100);
				int tthitvalue = IntersectAllEx(g.luadrawredirectobjectoffset, g.luadrawredirectobjectoffset, fX, fY, fZ, fNewX, fNewY, fNewZ, 0, 0, 0, 0, 1, false);
				if (tthitvalue != 0)
				{
					// determine Y coordinate on VR Screen
					sObject* pObject = g_ObjectList [ g.luadrawredirectobjectoffset ];
					float fVRScreenHeight = GetDisplayHeight() / 10.0f;
					float fObjWorldPosY = (pObject->position.vecPosition.y)-(fVRScreenHeight/2.0f);
					fFinalPercY = 100.0f-(((ChecklistFValueB(6)-fObjWorldPosY)/fVRScreenHeight)*100.0f);

					// determine X coordinate on VR Screen
					float fVRScreenWidth = GetDisplayWidth() / 10.0f;
					float fLeftSideX = pObject->position.vecPosition.x - Cos(pObject->position.vecRotate.y)*(fVRScreenWidth/2.0f);
					float fLeftSideZ = pObject->position.vecPosition.z + Sin(pObject->position.vecRotate.y)*(fVRScreenWidth/2.0f);
					float fRightSideX = pObject->position.vecPosition.x + Cos(pObject->position.vecRotate.y)*(fVRScreenWidth/2.0f);
					float fRightSideZ = pObject->position.vecPosition.z - Sin(pObject->position.vecRotate.y)*(fVRScreenWidth/2.0f);
					if (fRightSideX < fLeftSideX)
					{
						float fStore = fLeftSideX;
						fLeftSideX = fRightSideX;
						fRightSideX = fStore;
					}
					if (fRightSideZ < fLeftSideZ)
					{
						float fStore = fLeftSideZ;
						fLeftSideZ = fRightSideZ;
						fRightSideZ = fStore;
					}
					float fLengthX = fabs(fRightSideX - fLeftSideX);
					float fLengthZ = fabs(fRightSideZ - fLeftSideZ);
					float fIntersetX = ChecklistFValueA(6);
					float fIntersetZ = ChecklistFValueC(6);
					fIntersetX -= fLeftSideX;
					fIntersetZ -= fLeftSideZ;
					if (fLengthX > fLengthZ)
					{
						fIntersetX /= fLengthX;
						fFinalPercX = fIntersetX*100.0f;
					}
					else
					{
						fIntersetZ /= fLengthZ;
						fFinalPercX = fIntersetZ*100.0f;
					}
					float fDiff = WrapValue(pObject->position.vecRotate.y) - WrapValue(t.camangy_f);
					if (fDiff > 360.0f) fDiff -= 360.0f;
					if (fDiff < -360.0f) fDiff += 360.0f;
					if (fabs(fDiff) >= 90.0f && fabs(fDiff) < 270.0f)
					{
						// reverse X if looking at entity from behind
						fFinalPercX = 100.0f-fFinalPercX;
					}
					fFinalWheel = 0.0f;
				}
			}
		}
		#endif
		#ifdef STORYBOARD 
		extern float LuaMousePosPercentX, LuaMousePosX, LuaMousePosPercentY, LuaMousePosY;
		LuaMousePosX = g.LUAMouseX;
		LuaMousePosY = g.LUAMouseY;
		LuaMousePosPercentX = fFinalPercX;
		LuaMousePosPercentY = fFinalPercY;
		#endif
		LuaSetFloat("g_MouseX", fFinalPercX);
		LuaSetFloat("g_MouseY", fFinalPercY);

		// 310316 - need to keep real mouse fixed (or it clicks things in other monitors)
		HWND hForeWnd = GetForegroundWindow();
		HWND hThisWnd = g_pGlob->hWnd;
		extern bool g_bClipInForce;
		bool bAltKey = (::GetKeyState(VK_MENU) & 0x8000) != 0;
		if (hThisWnd == hForeWnd && bAltKey == false ) // also disengage when ALT pressed (anticipating an ALT+TAB freedom!)
		{
			//PE: Above dont work, try this - make sure mouse dont leave the current window.
			RECT r;
			GetWindowRect(g_pGlob->hWnd, &r);
			ClipCursor(&r);
			g_bClipInForce = true;
			if (g_bClipInForce == true) SetCursorPos(320, 240);
		}
		else
		{
			// no forced pointer and remove the clip!
			if (g_bClipInForce == true)
			{
				ClipCursor(NULL);
				g_bClipInForce = false;
			}
		}
	}
	else
	{
		//230216 - to help scripting, relay absolute values if not in mouse active mode
		LuaSetFloat ( "g_MouseX", -1.0f );
		LuaSetFloat ( "g_MouseY", -1.0f );
		extern DBPRO_GLOBAL int			g_iMouseLocalZ;
		if (iResetMouseWheel > 0)
		{
			//PE: Reset g_iMouseLocalZ when we return to game, so we dont get a mouse wheel input. script use "last input".
			g_iMouseLocalZ = 0;
			iResetMouseWheel--;
			LuaSetInt("g_MouseWheel", 0);
		}
		else
		{
			LuaSetInt("g_MouseWheel", MouseZ());
			//PE: We reset here , so the next 4 calls to UpdateMouse (before getting here again) can update g_iMouseLocalZ.
			g_iMouseLocalZ = 0;
		}
		extern float LuaMousePosPercentX, LuaMousePosX, LuaMousePosPercentY, LuaMousePosY;
		LuaMousePosX = -1.0f;
		LuaMousePosY = -1.0f;
		LuaMousePosPercentX = -1.0f;
		LuaMousePosPercentY = -1.0f;
	}

	int iMouseClickState = 0;
	if (g.vrglobals.GGVREnabled > 0 && g.vrglobals.GGVRUsingVRSystem == 1 && GGVR_RightController_Trigger() > 0.9f)
	{
		iMouseClickState = 1;
	}
	// allowed to detect mouse press even if in VR mode 
	if ( iMouseClickState==0 ) 
	{
		iMouseClickState = MouseClick();
	}
	LuaSetInt("g_MouseClick", iMouseClickState);

	#ifdef STORYBOARD 
	extern int LuaMouseClick;
	LuaMouseClick = iMouseClickState;
	#endif

	// continuing settinf of LUA globals
	LuaSetInt ( "g_EntityElementMax", g.entityelementlist);

	LuaSetInt("g_PlayerUnderwaterMode", g.underwatermode); // PE: underwater mode active.

	// 020316 - call the global loop once per cycle (for things like loading game states)
	if ( t.playercontrol.gameloopinitflag > 0 ) t.playercontrol.gameloopinitflag--;
	if ( t.playercontrol.gameloopinitflag == 0 )
	{
		// NOTE: Not entirely happy with a call which effectively 'loads from buffer' all the time
		t.tnothing = LuaExecute( cstr ( cstr("GlobalLoop(") + cstr(t.game.gameloop) + cstr(")") ).Get() );
	}

	// 170215 - use regular way to assign globals, not above execute approach
	t.tscan = ScanCode();
	LuaSetInt("g_Scancode", t.tscan);
	if (  t.tscan  ==  0 || t.tscan  ==  54 || t.tscan  ==  29 || t.tscan  ==  56 || t.tscan  ==  184 || t.tscan  ==  157 || t.tscan  ==  58 || t.tscan  ==  15 ) 
	{
		t.strwork = "";
	}
	else
	{
		t.tchar_s = Inkey();
		if (  t.tchar_s  ==  Chr(34)  )  t.tchar_s  =  cstr("\\") + Chr(34);
		if (  t.tchar_s  ==  "\\"  )  t.tchar_s  =  "\\\\";
		if (  Asc(t.tchar_s.Get()) < 32 || Asc(t.tchar_s.Get()) > 126  )  t.tchar_s  =  "";
		t.strwork = t.tchar_s;
	}
	LuaSetString("g_InKey", t.strwork.Get());
	//PE: Got a exception , g.projectfilename_s (mem read exception) was never set in standalone.
	if (g.projectfilename_s.Len() > 0)
	{
		LuaSetString("g_LevelFilename", g.projectfilename_s.Get() + strlen("mapbank\\"));
	}
	else
	{
		g.projectfilename_s = "";
		LuaSetString("g_LevelFilename", g.projectfilename_s.Get());
	}
	LuaSetInt("g_LevelTerrainSize", GGTerrain::ggterrain_global_render_params2.editable_size);

	// pass in values from projectileexplosionevents
	LuaSetInt("g_projectileevent_explosion", g.projectileEventType_explosion);
	LuaSetString("g_projectileevent_name", g.projectileEventType_name.Get());
	LuaSetInt("g_projectileevent_x", g.projectileEventType_x);
	LuaSetInt("g_projectileevent_y", g.projectileEventType_y);
	LuaSetInt("g_projectileevent_z", g.projectileEventType_z);
	LuaSetInt("g_projectileevent_radius", g.projectileEventType_radius);
	LuaSetInt("g_projectileevent_damage", g.projectileEventType_damage);
	LuaSetInt("g_projectileevent_entityhit", g.projectileEventType_entityhit);

	// at new LUA loop start, can activate any newly created spawned entities currently in limbo
	bool bBringNewOnesToLife = false;
	for ( int e = 1; e <= g.entityelementlist; e++)
	{
		if (t.entityelement[e].active == 0 && t.entityelement[e].lua.flagschanged == 123 )
		{
			t.entityelement[e].lua.flagschanged = 1;
			t.entityelement[e].active = 2; // set to 1 inside bringnewent func below (except for shop objects - see code)
			bBringNewOnesToLife = true;
		}
	}
	if (bBringNewOnesToLife == true)
	{
		entity_bringnewentitiestolife(true);
		LuaSetInt("g_NewEntitiesHaveBeenSpawnedInGame", 1);
	}
}

void lua_quitting ( void )
{
	lua_loop_begin();
	LuaSetFunction ( "GameLoopQuit", 0, 0 );
	LuaCall();
	lua_loop_finish();
}

void lua_updateweaponstats ( void )
{
	for ( int iMode = 1; iMode <= 5 ; iMode++ )
	{
		int iIndexMax = 10;
		if ( iMode >= 3 ) iIndexMax = 20;
		if ( iMode == 5 ) iIndexMax = 100;
		for ( int iIndex = 0; iIndex <= iIndexMax ; iIndex++ )
		{
			int iValue = 0;
			if ( iMode==1 ) iValue = t.weaponslot [ iIndex ].got;
			if ( iMode==2 ) iValue = t.weaponslot [ iIndex ].pref;
			if ( iMode==3 ) iValue = t.weaponammo [ iIndex ];
			if ( iMode==4 ) iValue = t.weaponclipammo [ iIndex ];
			if ( iMode==5 ) iValue = t.ammopool [ iIndex ].ammo;
			LuaSetFunction (  "UpdateWeaponStatsItem", 3, 0 ); LuaPushInt ( iMode ); LuaPushInt ( iIndex ); LuaPushInt ( iValue ); LuaCall (  );
		}
	}
}

void lua_ensureentityglobalarrayisinitialised ( void )
{
	if ( t.entityelement[t.e].lua.firsttime == 0 ) 
	{
		// only occurs once, unless new spawn/etc
		t.entityelement[t.e].lua.firsttime = 1;

		// 300316 - no need for entity details for static scenery entities in LUA
		if ( t.entityelement[t.e].staticflag == 0 ) 
		{
			LuaSetFunction (  "UpdateEntity",22,0 );
			LuaPushInt (  t.e );
			LuaPushInt (  t.tobj );
			LuaPushFloat (  t.entityelement[t.e].x );
			LuaPushFloat (  t.entityelement[t.e].y );
			LuaPushFloat (  t.entityelement[t.e].z );
			LuaPushFloat (  t.entityelement[t.e].rx );
			LuaPushFloat (  t.entityelement[t.e].ry );
			LuaPushFloat (  t.entityelement[t.e].rz );
			LuaPushInt (  t.entityelement[t.e].active );
			LuaPushInt (  t.entityelement[t.e].activated );
			LuaPushInt (  t.entityelement[t.e].collected );
			LuaPushInt (  t.entityelement[t.e].lua.haskey );
			LuaPushInt (  t.entityelement[t.e].lua.plrinzone );
			LuaPushInt (  t.entityelement[t.e].lua.entityinzone );
			LuaPushInt (  t.entityelement[t.e].plrvisible );
			LuaPushInt (  t.entityelement[t.e].lua.animating );
			LuaPushInt (  t.entityelement[t.e].health );
			LuaPushInt (  t.tfrm );
			LuaPushFloat (  t.entityelement[t.e].plrdist );
			LuaPushInt (  t.entityelement[t.e].lua.dynamicavoidance );

			// 201115 - pass in any hit limb name
			LPSTR pLimbByName = "";
			if ( t.entityelement[t.e].detectedlimbhit >=0 )
			{
				int iObjectNumber = g.entitybankoffset + t.entityelement[t.e].bankindex;
				if (ObjectExist(iObjectNumber) == 1)
				{
					if (LimbExist(iObjectNumber, t.entityelement[t.e].detectedlimbhit) == 1)
					{
						// check the object exists
						if (ConfirmObjectAndLimb(iObjectNumber, t.entityelement[t.e].detectedlimbhit))
						{
							// get name of frame
							sObject* pObject = g_ObjectList[iObjectNumber];
							LPSTR pLimbName = pObject->ppFrameList[t.entityelement[t.e].detectedlimbhit]->szName;
							pLimbByName = pLimbName;
						}
					}
				}
			}

			// push remaining and call
			LuaPushString ( pLimbByName );
			LuaPushInt ( t.entityelement[t.e].detectedlimbhit );
			LuaCall (  );

			// update LUA entity vars
			t.entityelement[t.e].lua.dynamicavoidance=0;
			t.entityelement[t.e].lua.dynamicavoidancestuckclock = 0.0f;
			t.entityelement[t.e].lua.interuptpath = 0;
			t.entityelement[t.e].lua.firsttime = 2;
			t.entityelement[t.e].lastx = t.entityelement[t.e].x;
			t.entityelement[t.e].lasty = t.entityelement[t.e].y;
			t.entityelement[t.e].lastz = t.entityelement[t.e].z;
			t.entityelement[t.e].animspeedmod = 1.0f;
		}
	}
}

#define TABLEOFPERFORMANCEMAX 5000
int g_iViewPerformanceTimers = 0;
LONGLONG g_tableofperformancetimers[TABLEOFPERFORMANCEMAX];
#define SWITCHTO30FPSRANGE 1000
uint32_t LuaFrameCount = 0;
uint32_t LuaFrameCount2 = 0;
//#pragma optimize("", off)
void lua_loop_allentities ( void )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	LuaFrameCount++;

	bool bMarkerCount = false;
	// Go through all entities with active LUA scripts
	for ( t.e = 1 ; t.e <= g.entityelementlist; t.e++ )
	{
		extern bool bEnable30FpsAnimations;
		if (bEnable30FpsAnimations && LuaFrameCount > 360 && t.entityelement[t.e].plrdist > SWITCHTO30FPSRANGE && t.entityelement[t.e].eleprof.phyalways == 0)
		{
			if ((LuaFrameCount + t.e) % 2 == 0)
				continue;
		}
		// reset performance measure
		if ( t.e < TABLEOFPERFORMANCEMAX) g_tableofperformancetimers[t.e] = 0;

		// this entity
		int thisentid = t.entityelement[t.e].bankindex;
		// LB210325 - why does "eleprof.spawnatstart==0" allow LUA_main to be called? Not active and not 'spawned at start' meansd no logic until something else spawns it (this caused cloned characters to clone the StartScript they moved into to be their main one, ie attack->patrol then stuck there)
		// if ( thisentid>0 && (t.entityelement[t.e].active != 0 || t.entityelement[t.e].lua.flagschanged == 2 || t.entityelement[t.e].eleprof.phyalways != 0 || t.entityelement[t.e].eleprof.spawnatstart==0) ) 
		// and only allow phyalways to work if spawn at start is not 0 (otherwise the always run logic will apply to entities not spawned in the level)
		//if (thisentid > 0 && (t.entityelement[t.e].active != 0 || t.entityelement[t.e].lua.flagschanged == 2 || t.entityelement[t.e].eleprof.phyalways != 0))
		if (thisentid > 0 && (t.entityelement[t.e].active != 0 || t.entityelement[t.e].lua.flagschanged == 2 || (t.entityelement[t.e].eleprof.phyalways != 0 && t.entityelement[t.e].eleprof.spawnatstart != 0)))
		{
			// skip new entities still in spawn activation sequence
			if (t.entityelement[t.e].lua.flagschanged == 123)
				continue;

			//LB: superceded with setting the active to zero when inside a shop/chest (i.e. not player inv/hotkeys)
			// must skip entity element if collected by shop or other container
			// only player and hotkeys collections can run logic!
			//if (t.entityelement[t.e].collected >= 3)
			//	continue;
			// skip entities that are inside shops or chests, ect
			if (t.entityelement[t.e].collected >= 3 && t.entityelement[t.e].active == 0)
				continue;

			// provided by darkai_loop control (avoids desync of use of maximumnonefreezedistance)
			if (t.entityelement[t.e].lua.outofrangefreeze == 1)
			{
				continue;
			}
			if (t.entityprofile[thisentid].ischaracter == 1 )
			{
				t.ttte = t.e; entity_find_charanimindex_fromttte(); // small optimization if we can replace this with storing in entityelement!
				if (t.tcharanimindex > 0)
				{
					if (t.charanimstates[t.tcharanimindex].dormant == 1)
					{
						continue;
					}
				}
			}
			
			// start performance measure
			if (t.e < TABLEOFPERFORMANCEMAX) g_tableofperformancetimers[t.e] = PerformanceTimer();

			// Update entity coordinates with real object coordinates
			t.tfrm=0 ; t.tobj=t.entityelement[t.e].obj;
			if (  t.tobj>0 ) 
			{
				if (  ObjectExist(t.tobj) == 1 ) 
				{
					t.tfrm=GetFrame(t.tobj);
					t.tentid=t.entityelement[t.e].bankindex;
					if (  t.entityprofile[t.tentid].collisionmode != 21 && t.entityprofile[t.tentid].ischaracter != 1 ) 
					{
						//  except entity driven physics objects
						t.entityelement[t.e].x=ObjectPositionX(t.tobj);
						t.entityelement[t.e].y=ObjectPositionY(t.tobj);
						t.entityelement[t.e].z=ObjectPositionZ(t.tobj);
					}
				}
			}

			// Initial population of LUA data
			lua_ensureentityglobalarrayisinitialised();

			// only process logic within plr freeze range
			t.te = t.e; entity_getmaxfreezedistance ( );
			//if (t.entityelement[t.e].plrdist < MAXFREEZEDISTANCE || t.entityelement[t.e].eleprof.phyalways != 0 || t.entityelement[t.e].lua.flagschanged == 2)
			int iDistanceForLogicToBeProcessed = t.maximumnonefreezedistance;
			if (t.entityelement[t.e].eleprof.phyalways == 0 && t.entityprofile[thisentid].ischaracter == 0) iDistanceForLogicToBeProcessed = 750; // use always active if want further than interactive range
			//PE: Markers cant have always active , so process those in intervals.
			t.waypointindex = t.entityelement[t.e].eleprof.trigger.waypointzoneindex;
			if (t.waypointindex > 0 && t.entityelement[t.e].plrdist >= iDistanceForLogicToBeProcessed)
			{
				if (!bMarkerCount)
				{
					//PE: Need own counter here as 30fps counter can make us never hit the below.
					LuaFrameCount2++;
					bMarkerCount = true;
				}
				if ((LuaFrameCount2 + t.e) % 30 == 0) //PE: Only every 0.5 sec.
					iDistanceForLogicToBeProcessed = t.maximumnonefreezedistance * 4; //PE: Expand it a bit for zones as they can be large.
			}

			if (t.entityelement[t.e].plrdist < iDistanceForLogicToBeProcessed || t.entityelement[t.e].eleprof.phyalways != 0 || t.entityelement[t.e].lua.flagschanged == 2)
			{
				//  If entity is waypoint zone, determine if player inside or outside
				if (  t.waypointindex>0 ) 
				{
					// should be the player pos to trigger this, NOT the camera (Thanks AmenMoses!)
					//t.tpointx_f=CameraPositionX(0); t.tpointz_f=CameraPositionZ(0);
					t.tpointx_f = ObjectPositionX(t.aisystem.objectstartindex);
					t.tpointz_f = ObjectPositionZ(t.aisystem.objectstartindex);
					if (  t.waypoint[t.waypointindex].active == 1 )
					{
						if (  t.waypoint[t.waypointindex].style == 2 ) 
						{
							t.tokay = 0; waypoint_ispointinzone ( );
							if (  t.entityelement[t.e].lua.plrinzone != t.tokay ) 
							{
								t.entityelement[t.e].lua.plrinzone=t.tokay;
								t.entityelement[t.e].lua.flagschanged=1;
							}
						}
					}
				}

				//  Detect if USE KEY field entity has been collected
				if (  t.entityelement[t.e].lua.haskey == 0 ) 
				{
					//  check if demilited key
					t.masterkeyname_s=Lower(t.entityelement[t.e].eleprof.usekey_s.Get());
					if (  Len(t.masterkeyname_s.Get())>0 ) 
					{
						t.tmultikey=0;
						for ( t.n = 1 ; t.n<=  Len(t.masterkeyname_s.Get()); t.n++ )
						{
							if (  cstr(Mid(t.masterkeyname_s.Get(),t.n)) == ";" )
							{
								t.tmultikey=1;
							}
						}
						//  Is USEKEY Collected?
						t.tokay=0;
						if (  t.tmultikey == 0 ) 
						{
							//  (SINGLE)
							for ( t.te = 1 ; t.te<=  g.entityelementlist; t.te++ )
							{
								if (  t.entityelement[t.te].collected == 1 ) 
								{
									if (  cstr(Lower(t.entityelement[t.te].eleprof.name_s.Get())) == t.masterkeyname_s ) 
									{
										t.tokay=1 ; break;
									}
								}
							}
						}
						else
						{
							//  (MULTIPLE)
							t.tokay=1;
							t.n=1;
							while (  t.n <= Len(t.masterkeyname_s.Get()) ) 
							{
								t.keyname_s="";
								while (  t.n <= Len(t.masterkeyname_s.Get()) ) 
								{
									if (  cstr(Mid(t.masterkeyname_s.Get(),t.n)) == ";"  )  break;
									t.keyname_s=t.keyname_s+Mid(t.masterkeyname_s.Get(),t.n);
									++t.n;
								}
								//  look for this key
								t.ttokay=0;
								for ( t.te = 1 ; t.te <= g.entityelementlist; t.te++ )
								{
									if (  t.entityelement[t.te].collected == 1 ) 
									{
										if (  cstr(Lower(t.entityelement[t.te].eleprof.name_s.Get())) == t.keyname_s ) 
										{
											t.ttokay=1 ; break;
										}
									}
								}
								//  any key not found means overall master key not valid
								if (  t.ttokay == 0  )  t.tokay = 0;
								++t.n;
							}
						}
						t.entityelement[t.e].lua.haskey=t.tokay;
						t.entityelement[t.e].lua.flagschanged=1;
					}
					else
					{
						//  when door/gate entity does not specify USE KEY, set to -1 to script knows
						//  no key/entity is required here (for additional script behaviours)
						t.entityelement[t.e].lua.haskey=-1;
						t.entityelement[t.e].lua.flagschanged=1;
					}
				}

				// Detect when entity object animation over
				if ( t.entityelement[t.e].lua.animating == 1 ) 
				{
					t.obj=t.entityelement[t.e].obj;
					if ( t.obj>0 ) 
					{
						if ( ObjectExist(t.obj) == 1 ) 
						{
							//PE: Dont interfere with chars, they rely on the status (play anim ending handled with smoothanimupdate for characters)
							//PE: Should only be for lua controlled objects doors ...
							if (t.entityprofile[thisentid].ischaracter != 1)
							{
								//PE: Check if animation is done.
								if (ConfirmObject(t.obj))
								{
									sObject* pObject = g_ObjectList[t.obj];
									WickedCall_CheckAnimationDone(pObject);
								}
							}
							else
							{
								// still want wicked to set the object play flag (passive read)
								if (ConfirmObject(t.obj))
								{
									sObject* pObject = g_ObjectList[t.obj];
									pObject->bAnimPlaying = WickedCall_GetAnimationPlayingState(pObject);
								}
							}
							if ( GetPlaying(t.obj) == 0 && t.smoothanim[t.obj].transition == 0 ) 
							{
								t.entityelement[t.e].lua.animating=0;
								LuaSetFunction (  "UpdateEntityAnimatingFlag",2,0 );
								LuaPushInt (  t.e );
								LuaPushInt (  t.entityelement[t.e].lua.animating );
								LuaCall (  );
							}
						}
					}
				}

				// 0403016 - can call this one last time to refresh LUA global arrays
				bool bSkipLUAScriptEntityRefreshOnly = false;
				if (t.entityelement[t.e].lua.flagschanged == 2)
				{
					t.entityelement[t.e].lua.flagschanged = 1; // do the update now
					bSkipLUAScriptEntityRefreshOnly = true;
				}

				// Update each cycle as entity position, health and GetFrame (  change constantly )
				//if (t.entityelement[t.e].plrdist < iDistanceForLogicToBeProcessed || t.entityprofile[thisentid].ischaracter == 1 || t.entityelement[t.e].eleprof.phyalways != 0)
				if (t.entityelement[t.e].plrdist < iDistanceForLogicToBeProcessed || t.entityelement[t.e].eleprof.phyalways != 0)
				{
					// first quarter of freeze range get full updates - also characters and those with alwaysactive flags
					if (t.entityelement[t.e].eleprof.phyalways != 0 || (t.entityprofile[thisentid].ischaracter == 1 && t.entityelement[t.e].plrdist < 1000))
					{
						// always update each frame - critical logic indicated with always active or clos enough character
						t.entityelement[t.e].lua.flagschanged = 1;
					}
					else
					{
						if (t.entityelement[t.e].plrdist < 300)
						{
							// always update things that are VERY close
							// also added new command "" to ensure things like lifts, ladders are always active
							t.entityelement[t.e].lua.flagschanged = 1;
						}
						else
						{
							// on average, 6 times per second for more mid-distant non critical logic
							if (Rnd(5) == 1) t.entityelement[t.e].lua.flagschanged = 1;
						}
					}
				}
				else
				{
					if (t.entityelement[t.e].eleprof.phyalways != 0)
					{
						// always active, EVEN if really far away
						t.entityelement[t.e].lua.flagschanged = 1;
					}
					else
					{
						//  rest gets updates every now and again based on distance
						if (t.entityelement[t.e].plrdist < MAXFREEZEDISTANCE / 2.0f)
						{
							if (Rnd(25) == 1)  t.entityelement[t.e].lua.flagschanged = 1;
						}
						else
						{
							if (t.entityelement[t.e].plrdist < MAXFREEZEDISTANCE / 1.25f)
							{
								if (Rnd(50) == 1)  t.entityelement[t.e].lua.flagschanged = 1;
							}
						}
					}
				}

				// if game state in progress, do not run any entity logic
				if ( t.luaglobal.gamestatechange == 0 )
				{
					// Called when entity states change
					// this ensures the game loads in _G[x] states BEFORE we start the game scripts
					// to avoid issues such as the start splash appearing when loading mid-way in level from main menu
					if ( t.entityelement[t.e].lua.flagschanged == 1 )
					{
						// do not refresh activated and animating as these are set INSIDE LUA!!
						if ( t.entityelement[t.e].staticflag == 0 && t.entityelement[t.e].lua.firsttime == 2 )
						{
							LuaSetFunction("UpdateEntityRT", 21, 0);
							LuaPushInt (  t.e );
							LuaPushInt (  t.tobj );
							if ( g.mp.endplay == 0 ) // can now run own script in multiplayer || t.game.runasmultiplayer == 0
							{
								LuaPushFloat ( t.entityelement[t.e].x );
								LuaPushFloat ( t.entityelement[t.e].y );
								LuaPushFloat ( t.entityelement[t.e].z );
							}
							else
							{
								LuaPushFloat ( 100000 );
								LuaPushFloat ( 100000 );
								LuaPushFloat ( 100000 );
							}
							LuaPushFloat (  t.entityelement[t.e].rx );
							LuaPushFloat (  t.entityelement[t.e].ry );
							LuaPushFloat (  t.entityelement[t.e].rz );
							LuaPushInt (  t.entityelement[t.e].active );
							LuaPushInt (  t.entityelement[t.e].activated );
							LuaPushInt (  t.entityelement[t.e].collected );
							LuaPushInt (  t.entityelement[t.e].lua.haskey );
							LuaPushInt (  t.entityelement[t.e].lua.plrinzone );
							LuaPushInt (  t.entityelement[t.e].lua.entityinzone );
							LuaPushInt (  t.entityelement[t.e].plrvisible );
							LuaPushInt ( t.entityelement[t.e].health );
							LuaPushInt (  t.tfrm );
							LuaPushFloat (  t.entityelement[t.e].plrdist );
							LuaPushInt (  t.entityelement[t.e].lua.dynamicavoidance );
							
							// 201115 - pass in any hit limb name
							LPSTR pLimbByName = "";
							if ( t.entityelement[t.e].detectedlimbhit >= 0 )
							{
								int iObjectNumber = g.entitybankoffset + t.entityelement[t.e].bankindex;
								if (ObjectExist(iObjectNumber) == 1)
								{
									if (LimbExist(iObjectNumber, t.entityelement[t.e].detectedlimbhit) == 1)
									{
										//PE: Fix for memory leak https://github.com/TheGameCreators/GameGuruRepo/issues/1070
										// check the object exists
										if (ConfirmObjectAndLimb(iObjectNumber, t.entityelement[t.e].detectedlimbhit))
										{
											// get name of frame
											sObject* pObject = g_ObjectList[iObjectNumber];
											LPSTR pLimbName = pObject->ppFrameList[t.entityelement[t.e].detectedlimbhit]->szName;
											pLimbByName = pLimbName;
										}
									}
								}
							}
							LuaPushString ( pLimbByName );
							LuaPushInt ( t.entityelement[t.e].detectedlimbhit );
							LuaCall (  );
							t.entityelement[t.e].lua.flagschanged=0;
						}
					}

					//  Call each cycle
					if (  t.entityelement[t.e].eleprof.aimain == 1 && bSkipLUAScriptEntityRefreshOnly==false ) 
					{
						bool bCanSkipNow = false;
						LPSTR pTestScriptName = t.entityelement[t.e].eleprof.aimainname_s.Get();
						if (strlen(pTestScriptName) == 20)
						{
							if (strcmp(pTestScriptName, "no_behavior_selected") == NULL)
							{
								bCanSkipNow = true;
							}
						}
						else
						{
							if (strlen(pTestScriptName) == 7)
							{
								if (strcmp(pTestScriptName, "default") == NULL)
								{
									bCanSkipNow = true;
								}
							}
						}

						if (  Len(t.entityelement[t.e].eleprof.aimainname_s.Get())>1 ) 
						{
							// can call LUA main function
							if (bCanSkipNow == false )
							{
								if ( t.entityelement[t.e].eleprof.aipreexit >= 1 )
								{
									if ( t.entityelement[t.e].eleprof.aipreexit == 1 )
									{
										t.entityelement[t.e].eleprof.aipreexit = 3;
										t.strwork = cstr(cstr(t.entityelement[t.e].eleprof.aimainname_s.Get())+"_preexit");
										LuaSetFunction ( t.strwork.Get(), 1, 0 );
										LuaPushInt ( t.e ); LuaCall ( );
										if ( t.entityelement[t.e].eleprof.aipreexit == 2 )
										{
											t.v = 0.0f;
											entity_lua_setentityhealth();
										}
									}
								}
								else
								{
									t.strwork = cstr(cstr(t.entityelement[t.e].eleprof.aimainname_s.Get()) + "_main");
									LuaSetFunction (t.strwork.Get(), 1, 0);
									LuaPushInt (t.e); 
									LuaCall ();
								}
							}
						}
					}
				}
			}

			// 090517 - should not depend on scripts being refreshed
			t.entityelement[t.e].lastx = t.entityelement[t.e].x;
			t.entityelement[t.e].lasty = t.entityelement[t.e].y;
			t.entityelement[t.e].lastz = t.entityelement[t.e].z;

			// performance measure of ALL entity logic runs
			if (t.e < TABLEOFPERFORMANCEMAX)
			{
				// record time taken for this entity LUA logic
				g_tableofperformancetimers[t.e] = PerformanceTimer() - g_tableofperformancetimers[t.e];

				// if extreme, and flagged, stop action and view this naughty script!
				if (g.gproducelogfiles == 3)
				{
					int iUAsMillisecond = 30000;
					if (g_tableofperformancetimers[t.e] > iUAsMillisecond)
					{
						g_iViewPerformanceTimers = 1;
					}
				}
			}
		}
	}

	// view LUA logic performance per entity
	if ( g_iViewPerformanceTimers == 1 )
	{
		// Find the top ten worst offenders and list them out
		int iWorstOffenderE[10];
		LONGLONG iWorstOffender[10];
		for (int i = 0; i < 10; i++)
		{
			iWorstOffenderE[i] = 0;
			iWorstOffender[i] = 0;
		}
		for (int e = 1; e <= g.entityelementlist; e++)
		{
			if (e < TABLEOFPERFORMANCEMAX)
			{
				if (t.entityelement[e].active == 0) g_tableofperformancetimers[e] = 0;
				if (g_tableofperformancetimers[e] > iWorstOffender[0])
				{
					iWorstOffenderE[0] = e;
					iWorstOffender[0] = g_tableofperformancetimers[e];
					for (int i = 0; i < 10; i++)
					{
						for (int j = 0; j < 10; j++)
						{
							if (j > i)
							{
								if (iWorstOffender[i] > iWorstOffender[j])
								{
									int iStoreE = iWorstOffenderE[i];
									LONGLONG iStore = iWorstOffender[i];
									iWorstOffenderE[i] = iWorstOffenderE[j];
									iWorstOffender[i] = iWorstOffender[j];
									iWorstOffenderE[j] = iStoreE;
									iWorstOffender[j] = iStore;
								}
							}
						}
					}
				}
			}
		}
		char pShowList[10240];
		strcpy(pShowList, "Top Ten Most Expensive Logic This Cycle:\n\n");
		for (int j = 9; j > 0; j--)
		{
			int e = iWorstOffenderE[j];
			char pMeasure[32];
			sprintf(pMeasure, "%lldu", 100000+iWorstOffender[j]);
			char pThisLine[1024];
			sprintf(pThisLine, "%s : %d = %s\n", pMeasure+1, e, t.entityelement[e].eleprof.aimain_s.Get());
			strcat(pShowList, pThisLine);
		}
		MessageBoxA(NULL, pShowList, "Logic Performance (auto-triggered using 'producelogfiles=3')", MB_OK);
		g_iViewPerformanceTimers = 0;
	}
}
//#pragma optimize("", on)
bool g_WarnOnlyOnce = true;

void lua_loop_finish (void)
{
	// Detect any messges back from LUA engine (actions)
	bool bAnySendMessages = false;
	while (LuaNext())
	{
		t.luaaction_s = LuaMessageDesc();
		bAnySendMessages = true;
	}
	if (bAnySendMessages == true && g_WarnOnlyOnce==true)
	{
		g_WarnOnlyOnce = false;
		MessageBoxA(NULL, "Do Not Use SendMessage any more", t.luaaction_s.Get(), MB_OK);
	}

	// extra stage allowing global to render things LAST (such as in-game HUD screens)
	if (t.playercontrol.gameloopinitflag == 0)
	{
		t.tnothing = LuaExecute(cstr (cstr("GlobalLoopFinish(") + cstr(t.game.gameloop) + cstr(")")).Get());
	}

	// update engine global at end of all LUA activity this cycle
	g.projectileEventType_explosion = LuaGetInt("g_projectileevent_explosion");
}

void lua_loop ( void )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	panel_First2DDrawing();
	lua_loop_begin();
	lua_loop_allentities();
	lua_loop_finish();
	panel_Last2DDrawing();
}

void lua_raycastingwork (void)
{
	// moving and controlling ray casts separate so can test speed of combined work

}
