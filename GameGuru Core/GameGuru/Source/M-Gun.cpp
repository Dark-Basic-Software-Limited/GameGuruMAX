//----------------------------------------------------
//--- GAMEGURU - M-Gun
//----------------------------------------------------

#include "stdafx.h"
#include "gameguru.h"

// 
//  GUN CODE (common)
// 

bool g_bGunListNeedsRefreshing = true;

int constexpr conststrlen( const char* str )
{
	return *str ? 1 + conststrlen(str + 1) : 0;
}

#define cmpStrConst( str, cmpVal ) \
{ \
	matched = true; \
	if ( strcmp(str, cmpVal) != 0 ) matched = false; \
}

// max length 16
#define cmpNStrConst( str, cmpVal ) \
{ \
	matched = true; \
	int constexpr len = conststrlen( cmpVal ); \
	if ( strncmp(str, cmpVal, len) != 0 ) matched = false; \
}

void gun_loaddata ( void )
{
	// used for new weapon system
	cstr lastsoundanimname_s = "";
	float fLastStartFrameFromAnimName = -1;
	float fLastFinishFrameFromAnimName = -1;
	float fLastFinishSetSizeFromAnimName = 0;
	int iModifyTheLastFinishFrameWhenKnowPerc = 0;

	//  default settings
	t.gun[t.gunid].settings.minpolytrim=0;
	t.gun[t.gunid].projectile_s="";
	t.gun[t.gunid].projectileframe=0;
	t.gun[t.gunid].vweaptex_s="";
	t.gun[t.gunid].boostintensity = 0.0f;

	// reset VR specificgun settings
	t.gun[t.gunid].settings.iVRWeaponMode = 0;
	t.gun[t.gunid].settings.iVRWeaponStaticFrame = 0;
	t.gun[t.gunid].settings.iVRWeaponLimbOfWeapon = 0;
	t.gun[t.gunid].settings.fVRWeaponOffsetX = 0;
	t.gun[t.gunid].settings.fVRWeaponOffsetY = 0;
	t.gun[t.gunid].settings.fVRWeaponOffsetZ = 0;
	t.gun[t.gunid].settings.fVRWeaponScale = 100;
	t.gun[t.gunid].settings.fVRWeaponAngleX = 0;
	t.gun[t.gunid].settings.fVRWeaponAngleY = 0;
	t.gun[t.gunid].settings.fVRWeaponAngleZ = 0;
	
	t.gun[t.gunid].settings.fEmissiveStrength = 1.0f;

	t.gun[t.gunid].settings.tracer_active = false;
	t.gun[t.gunid].settings.tracer_lifetime = 0.15f;
	t.gun[t.gunid].settings.tracer_glow = 8;
	t.gun[t.gunid].settings.tracer_scrollV = 0;
	t.gun[t.gunid].settings.tracer_scaleV = 1.0f;
	t.gun[t.gunid].settings.tracer_width = 1.0f;
	t.gun[t.gunid].settings.tracer_maxlength = 0;
	t.gun[t.gunid].settings.tracer_imagefile = "tracer.dds";
	t.gun[t.gunid].settings.tracer_WPEId = 0;
	t.gun[t.gunid].settings.tracer_colorR = 1.0f;
	t.gun[t.gunid].settings.tracer_colorG = 0.5f;
	t.gun[t.gunid].settings.tracer_colorB = 0.1f;

	t.gun[t.gunid].settings.fake_reload = false;


	// stores legacy ammo/clip values when move slot weapons to player containers for storage
	t.gun[t.gunid].storeammo = 0;
	t.gun[t.gunid].storeclipammo = 0;

	t.gun[t.gunid].legacy_animation_s = "";

	//  two basic firemodes (i.e semi+full auto or shotgun+grenade)
	for ( t.i = 0 ; t.i <= 1; t.i++ )
	{
		//  Default Zoom Walk & Turn
		g.firemodes[t.gunid][t.i].settings.zoomwalkspeed=0.5f;
		g.firemodes[t.gunid][t.i].settings.zoomturnspeed=0.2f;
		g.firemodes[t.gunid][t.i].settings.plrmovespeedmod=1.0f;
		g.firemodes[t.gunid][t.i].settings.plremptyspeedmod=1.0f;
		g.firemodes[t.gunid][t.i].settings.plrturnspeedmod=1.0f;
		g.firemodes[t.gunid][t.i].settings.plrjumpspeedmod=1.0f;
		g.firemodes[t.gunid][t.i].settings.plrreloadspeedmod=1.0f;

		//  Default Run Accuracy (No diff)
		g.firemodes[t.gunid][t.i].settings.runaccuracy = -1;
		g.firemodes[t.gunid][t.i].settings.runanimdelay = 250;

		//  Default Sound Strength
		g.firemodes[t.gunid][t.i].settings.soundstrength=100;

		//  Default equipment settings
		g.firemodes[t.gunid][t.i].settings.equipment=0;
		g.firemodes[t.gunid][t.i].settings.equipmententityelementindex=0;
		g.firemodes[t.gunid][t.i].settings.lockcamera=0;

		//  Reset HUD object rotation
		g.firemodes[t.gunid][t.i].settings.rotx_f=0;
		g.firemodes[t.gunid][t.i].settings.roty_f=0;
		g.firemodes[t.gunid][t.i].settings.rotz_f=0;

		//  resets
		g.firemodes[t.gunid][t.i].settings.reloadspeed=1.0;
		g.firemodes[t.gunid][t.i].settings.decalforward=0;
		g.firemodes[t.gunid][t.i].settings.dofdistance=0;
		g.firemodes[t.gunid][t.i].settings.dofintensity=0;
		g.firemodes[t.gunid][t.i].settings.zoomhidecrosshair=0;

		// by default gunspec defaults there is no spot light for gun
		g.firemodes[t.gunid][t.i].settings.usespotlighting=0;

		// ensure material not ignored by default
		g.firemodes[t.gunid][t.i].settings.ignorematerial = 0;

		// no smoke key yet, and no decal from an earlier load of this gun
		g.firemodes[t.gunid][t.i].settings.smoke=-1;
		g.firemodes[t.gunid][t.i].settings.smokedecal_s="";

		// reset other items
		g.firemodes[t.gunid][t.i].settings.brassdelay = 0;
		g.firemodes[t.gunid][t.i].settings.zoombrassdelay = 0;
		g.firemodes[t.gunid][t.i].settings.doesnotuseammo = 0;

	}

	//PE: We need to always be able to locate animation like 'idle'. so.
	int iCurrentGunObj = t.currentgunobj;
	if (iCurrentGunObj == 0)
		iCurrentGunObj = t.gun[t.gunid].obj;

	//  Load GUNSPEC details (270618 - increased to 1000 lines)
	Dim ( t.data_s, 1000 );
	t.filename_s = "" ; t.filename_s=t.filename_s+"gamecore\\"+g.fpgchuds_s+"\\"+t.gun_s+"\\gunspec.txt";
	if (  FileExist(t.filename_s.Get()) == 0  ) { t.filename_s = "" ; t.filename_s=t.filename_s+ "gamecore\\"+g.fpgchuds_s+"\\"+t.gun_s+"\\spec.txt"; }
	if (  FileExist(t.filename_s.Get()) == 1 ) 
	{
		// if an empty weapon, leave early
		if ( stricmp (t.gun_s.Get(), "Slot Not Used" ) == NULL )
		{
			// mark this as the slot blocker weapon
			t.gun[t.gunid].pathtostockentity_s = "Slot Not Used";
			return;
		}

		LoadArray (  t.filename_s.Get() ,t.data_s );
		for ( t.l = 0 ; t.l <= 999; t.l++ )
		{
			t.line_s=t.data_s[t.l];
			if (  Len(t.line_s.Get())>0 ) 
			{
				if ( t.line_s.Get()[0] != ';' )
				{
					//  take fieldname and value
					for ( t.c = 0 ; t.c <  Len(t.line_s.Get()); t.c++ )
					{
						if (  t.line_s.Get()[t.c] == '=' ) { t.mid = t.c+1  ; break; }
					}
					t.field_s=Lower(removeedgespaces(Left(t.line_s.Get(),t.mid-1)));
					t.value_s=removeedgespaces(Right(t.line_s.Get(),Len(t.line_s.Get())-t.mid));

					// 280618 - remove TAB characters from field (where TAB is used instead of SPACE in describing field)
					for ( t.c = 0 ; t.c < Len(t.field_s.Get()); t.c++ )
					{
						if ( t.field_s.Get()[t.c] == 9 ) { t.field_s.Get()[t.c] = ' '; }
					}

					//  take value 1 and 2 from value
					bool bFoundComma = false;
					for ( t.c = 0 ; t.c <  Len(t.value_s.Get()); t.c++ )
					{
						if (t.value_s.Get()[t.c] == ',') { t.mid = t.c + 1; bFoundComma = true;  break; }
					}
					t.value1=ValF(removeedgespaces(Left(t.value_s.Get(),t.mid-1))) ;
					t.value2=ValF(removeedgespaces(Right(t.value_s.Get(),Len(t.value_s.Get())-t.mid)));

					// string comparison optimization
					alignas(16) char t_field_s[32]; // cmpStrConst limtied to 32 characters
					const char* src = Lower(t.field_s.Get());
					int index1 = 0;
					int index2 = 0;
					bool skipAltSpace = false;
					if ( src[0] == 'a' && src[1] == 'l' && src[2] == 't' && src[3] == ' ' ) skipAltSpace = true;
					while( src[index1] && index2 < 32 )
					{
						if ( index1 != 3 || !skipAltSpace )
						{
							t_field_s[ index2 ] = src[ index1 ];
							index2++;
						}						
						index1++;
					}

					int matchlen = index2;
					while( index2 < 32 ) t_field_s[ index2++ ] = 0;
					bool matched = false;

					// Path to real entity representing gun
					cmpStrConst(t_field_s, "pathtostockentity");
					if (matched) t.gun[t.gunid].pathtostockentity_s = t.value_s;		

					//  Gun basic settings			
					cmpStrConst(t_field_s, "animsetoverride");
					if (matched) t.gun[t.gunid].animsetoverride = t.value_s;
					cmpStrConst(t_field_s, "statuspanelcode");
					if (matched) t.gun[t.gunid].statuspanelcode = t.value1;
					cmpStrConst( t_field_s, "weaponisammo" );
					if( matched )  t.gun[t.gunid].settings.weaponisammo = t.value1;
					cmpStrConst( t_field_s, "minpolytrim" );
					if( matched )  t.gun[t.gunid].settings.minpolytrim = t.value1;

					//  Gun basic visuals
					cmpStrConst( t_field_s, "textured" );
					if( matched )  t.gun[t.gunid].texd_s = t.value_s;
					cmpStrConst( t_field_s, "effect" );
					if( matched )  t.gun[t.gunid].effect_s = t.value_s;
					cmpStrConst( t_field_s, "transparency" );
					if( matched )  t.gun[t.gunid].transparency = t.value1;
					cmpStrConst( t_field_s, "vweaptex" );
					if( matched )  t.gun[t.gunid].vweaptex_s = t.value_s;

					cmpStrConst(t_field_s, "legacyanimation");
					if (matched)  t.gun[t.gunid].legacy_animation_s = t.value_s;

					//  control weapon shader using art flags
					cmpStrConst( t_field_s, "invertnormal" );
					if( matched )  t.gun[t.gunid].invertnormal = t.value1;
					cmpStrConst( t_field_s, "preservetangents" );
					if( matched )  t.gun[t.gunid].preservetangents = t.value1;
					cmpStrConst( t_field_s, "boostintensity" );
					if( matched )  t.gun[t.gunid].boostintensity = t.value1 / 100.0f;

					//  weapontype ; 0-grenade, 1-pistol, 2-rocket, 3-shotgun, 4-uzi, 5-assault, 51-melee(noammo)
					cmpStrConst( t_field_s, "weapontype" );
					if( matched )  t.gun[t.gunid].weapontype = t.value1;
					cmpStrConst( t_field_s, "projectile" );
					if( matched )  t.gun[t.gunid].projectile_s = t.value_s;
					cmpStrConst( t_field_s, "projectileframe" );
					if( matched )  t.gun[t.gunid].projectileframe = t.value1;
					cmpStrConst( t_field_s, "keyframe speed" );
					if( matched ) t.gun[t.gunid].keyframespeed_f = (float)t.value1 / 100.0f;

					cmpStrConst(t_field_s, "handusesnewweaponsystem");
					if (matched)  t.gun[t.gunid].handusesnewweaponsystem = t.value1;
					cmpStrConst(t_field_s, "handscale");
					if (matched)  t.gun[t.gunid].handscale_f = t.value1;
					cmpStrConst(t_field_s, "handposx");
					if (matched)  t.gun[t.gunid].handposx_f = t.value1;
					cmpStrConst( t_field_s, "handposy" );
					if( matched )  t.gun[t.gunid].handposy_f = t.value1;
					cmpStrConst( t_field_s, "handposz" );
					if( matched )  t.gun[t.gunid].handposz_f = t.value1;
					cmpStrConst( t_field_s, "handrotx" );
					if( matched )  t.gun[t.gunid].handrotx_f = t.value1;
					cmpStrConst( t_field_s, "handroty" );
					if( matched )  t.gun[t.gunid].handroty_f = t.value1;
					cmpStrConst( t_field_s, "handrotz" );
					if( matched )  t.gun[t.gunid].handrotz_f = t.value1;
					cmpStrConst( t_field_s, "alwaysshowenemyweapon" );
					if( matched )  t.gun[t.gunid].alwaysshowenemyweapon = t.value1;

					cmpStrConst(t_field_s, "weaponposx");
					if (matched)  t.gun[t.gunid].weaponposx_f = t.value1;
					cmpStrConst(t_field_s, "weaponposy");
					if (matched)  t.gun[t.gunid].weaponposy_f = t.value1;
					cmpStrConst(t_field_s, "weaponposz");
					if (matched)  t.gun[t.gunid].weaponposz_f = t.value1;
					cmpStrConst(t_field_s, "weaponrotx");
					if (matched)  t.gun[t.gunid].weaponrotx_f = t.value1;
					cmpStrConst(t_field_s, "weaponroty");
					if (matched)  t.gun[t.gunid].weaponroty_f = t.value1;
					cmpStrConst(t_field_s, "weaponrotz");
					if (matched)  t.gun[t.gunid].weaponrotz_f = t.value1;

					cmpStrConst(t_field_s, "firespotposx");
					if (matched)  t.gun[t.gunid].firespotx_f = t.value1;
					cmpStrConst(t_field_s, "firespotposy");
					if (matched)  t.gun[t.gunid].firespoty_f = t.value1;
					cmpStrConst(t_field_s, "firespotposz");
					if (matched)  t.gun[t.gunid].firespotz_f = t.value1;			

					cmpStrConst( t_field_s, "block" );
					if( matched )
					{ 
						g.firemodes[t.gunid][0].action.block.s = t.value1; 
						g.firemodes[t.gunid][0].action.block.e = t.value2; 
						g.firemodes[t.gunid][1].action.block.s = t.value1; 
						g.firemodes[t.gunid][1].action.block.e = t.value2; 
					}

					//  Load in details for two firemodes
					
					// 140616 - Whether fire mode affects projectile spot lighting
					cmpStrConst( t_field_s, "usespotlighting" );
					if( matched ) g.firemodes[t.gunid][0].settings.usespotlighting = t.value1;

					//  Classic details
					cmpStrConst( t_field_s, "muzzlesize" );
					if( matched )  g.firemodes[t.gunid][0].settings.muzzlesize_f = t.value1;
					cmpStrConst( t_field_s, "muzzlecolorr" );
					if( matched )  g.firemodes[t.gunid][0].settings.muzzlecolorr = t.value1;
					cmpStrConst( t_field_s, "muzzlecolorg" );
					if( matched )  g.firemodes[t.gunid][0].settings.muzzlecolorg = t.value1;
					cmpStrConst( t_field_s, "muzzlecolorb" );
					if( matched )  g.firemodes[t.gunid][0].settings.muzzlecolorb = t.value1;
					cmpStrConst( t_field_s, "smoke" );
					if( matched )  g.firemodes[t.gunid][0].settings.smoke = t.value1;
					cmpStrConst( t_field_s, "smokesize" );
					if( matched )  g.firemodes[t.gunid][0].settings.smokesize = t.value1;
					cmpStrConst( t_field_s, "smokespeed" );
					if( matched )  g.firemodes[t.gunid][0].settings.smokespeed = t.value1;
					cmpStrConst( t_field_s, "smokedecal" );
					if( matched )  g.firemodes[t.gunid][0].settings.smokedecal_s = t.value_s;
					cmpStrConst( t_field_s, "flak" );
					if( matched )  g.firemodes[t.gunid][0].settings.flakname_s = t.value_s;
					cmpStrConst( t_field_s, "flakrearmframe" );
					if( matched )  g.firemodes[t.gunid][0].settings.flakrearmframe = t.value1;
					cmpStrConst( t_field_s, "reloadqty" );
					if( matched )  g.firemodes[t.gunid][0].settings.reloadqty = t.value1;
					cmpStrConst( t_field_s, "reloadspeed" );
					if( matched )  g.firemodes[t.gunid][0].settings.reloadspeed = (t.value1+0.0)/100.0;
					cmpStrConst( t_field_s, "iterate" );
					if( matched )  g.firemodes[t.gunid][0].settings.iterate = t.value1;
					cmpStrConst( t_field_s, "range" );
					if( matched )  g.firemodes[t.gunid][0].settings.range = t.value1;
					cmpStrConst( t_field_s, "accuracy" );
					if( matched )  g.firemodes[t.gunid][0].settings.accuracy = t.value1;
					cmpStrConst( t_field_s, "damage" );
					if( matched )  g.firemodes[t.gunid][0].settings.damage = t.value1;
					cmpStrConst( t_field_s, "dropoff" );
					if( matched )  g.firemodes[t.gunid][0].settings.dropoff = t.value1;

					cmpStrConst(t_field_s, "clipcapacity");
					if (matched)  g.firemodes[t.gunid][0].settings.clipcapacity = t.value1;

					cmpStrConst( t_field_s, "muzzleflash" );
					if( matched )  g.firemodes[t.gunid][0].settings.muzzleflash = t.value1;
					cmpStrConst( t_field_s, "firerate" );
					if( matched )  g.firemodes[t.gunid][0].settings.firerate = t.value1;
					cmpStrConst( t_field_s, "detectcoloff" );
					if( matched )  g.firemodes[t.gunid][0].settings.detectcoloff = t.value1;
					cmpStrConst( t_field_s, "ignorematerial" );
					if( matched )  g.firemodes[t.gunid][0].settings.ignorematerial = t.value1;
					cmpStrConst( t_field_s, "zoommode" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoommode = t.value1;
					cmpStrConst( t_field_s, "zoomaccuracy" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoomaccuracy = t.value1;
					cmpStrConst( t_field_s, "zoomaccuracybreathhold" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoomaccuracybreathhold = t.value1;
					cmpStrConst( t_field_s, "zoomaccuracybreath" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoomaccuracybreath = t.value1;
					cmpStrConst( t_field_s, "zoomaccuracyheld" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoomaccuracyheld = t.value1;
					cmpStrConst( t_field_s, "zoomscope" );
					if( matched )  g.firemodes[t.gunid][0].zoomscope_s = t.value_s;
					cmpStrConst( t_field_s, "second" );
					if( matched )  g.firemodes[t.gunid][0].settings.seconduse = t.value1;
					cmpStrConst( t_field_s, "damagetype" );
					if( matched )  g.firemodes[t.gunid][0].settings.damagetype = t.value1;
					cmpStrConst( t_field_s, "scorchtype" );
					if( matched )  g.firemodes[t.gunid][0].settings.scorchtype = t.value1;
					cmpStrConst( t_field_s, "melee damage" );
					if( matched )  g.firemodes[t.gunid][0].settings.meleedamage = t.value1;
					cmpStrConst( t_field_s, "melee range" );
					if( matched )  g.firemodes[t.gunid][0].settings.meleerange = t.value1;
					cmpStrConst( t_field_s, "dofdistance" );
					if( matched )  g.firemodes[t.gunid][0].settings.dofdistance = t.value1;
					cmpStrConst( t_field_s, "dofintensity" );
					if( matched )  g.firemodes[t.gunid][0].settings.dofintensity = t.value1;
					cmpStrConst( t_field_s, "decal" );
					if( matched )  g.firemodes[t.gunid][0].decal_s = t.value_s;
					cmpStrConst( t_field_s, "decalforward" );
					if( matched )  g.firemodes[t.gunid][0].settings.decalforward = t.value1;
					cmpStrConst( t_field_s, "shotgun" );
					if( matched )  g.firemodes[t.gunid][0].settings.shotgun = t.value1;
					cmpStrConst(t_field_s, "disablerunandshoot");
					if (matched)  g.firemodes[t.gunid][0].settings.disablerunandshoot = t.value1;
					cmpStrConst(t_field_s, "meleewithrightclick");
					if (matched)  g.firemodes[t.gunid][0].settings.meleewithrightclick = t.value1;
					cmpStrConst(t_field_s, "blockwithrightclick");
					if (matched)  g.firemodes[t.gunid][0].settings.blockwithrightclick = t.value1;
					cmpStrConst(t_field_s, "meleequickrepeat");
					if (matched)  g.firemodes[t.gunid][0].settings.meleequickrepeat = t.value1;
					cmpStrConst( t_field_s, "force" );
					if( matched )  g.firemodes[t.gunid][0].settings.force = t.value1;
					cmpStrConst( t_field_s, "melee force" );
					if( matched )  g.firemodes[t.gunid][0].settings.meleeforce = t.value1;
					cmpStrConst( t_field_s, "npcignorereload" );
					if( matched )  g.firemodes[t.gunid][0].settings.npcignorereload = t.value1;
					cmpStrConst( t_field_s, "zoomhidecrosshair" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoomhidecrosshair = t.value1;
					cmpStrConst( t_field_s, "doesnotuseammo" );
					if( matched )  g.firemodes[t.gunid][0].settings.doesnotuseammo = t.value1;
					cmpStrConst( t_field_s, "usemeleedamageonly" );
					if( matched )  
						g.firemodes[t.gunid][0].settings.usemeleedamageonly = t.value1;

					//  Brass details
					cmpStrConst( t_field_s, "brass" );
					if( matched )  g.firemodes[t.gunid][0].settings.brass = t.value1;
					cmpStrConst( t_field_s, "brasslife" );
					if( matched )  g.firemodes[t.gunid][0].settings.brasslife = (t.value1+0.0);
					cmpStrConst( t_field_s, "brassangle" );
					if( matched )  g.firemodes[t.gunid][0].settings.brassangle = (t.value1+0.0);
					cmpStrConst( t_field_s, "brassanglerand" );
					if( matched )  g.firemodes[t.gunid][0].settings.brassanglerand = (t.value1+0.0);
					cmpStrConst( t_field_s, "brassspeed" );
					if( matched )  g.firemodes[t.gunid][0].settings.brassspeed = (t.value1+0.0)/100.0;
					cmpStrConst( t_field_s, "brassspeedrand" );
					if( matched )  g.firemodes[t.gunid][0].settings.brassspeedrand = (t.value1+0.0)/100.0;
					cmpStrConst( t_field_s, "brassupward" );
					if( matched )  g.firemodes[t.gunid][0].settings.brassupward = (t.value1+0.0)/100.0;
					cmpStrConst( t_field_s, "brassupwardrand" );
					if( matched )  g.firemodes[t.gunid][0].settings.brassupwardrand = (t.value1+0.0)/100.0;
					cmpStrConst( t_field_s, "brassrotx" );
					if( matched )  g.firemodes[t.gunid][0].settings.brassrotx = (t.value1+0.0);
					cmpStrConst( t_field_s, "brassrotxrand" );
					if( matched )  g.firemodes[t.gunid][0].settings.brassrotxrand = (t.value1+0.0);
					cmpStrConst( t_field_s, "brassroty" );
					if( matched )  g.firemodes[t.gunid][0].settings.brassroty = (t.value1+0.0);
					cmpStrConst( t_field_s, "brassrotyrand" );
					if( matched )  g.firemodes[t.gunid][0].settings.brassrotyrand = (t.value1+0.0);
					cmpStrConst( t_field_s, "brassrotz" );
					if( matched )  g.firemodes[t.gunid][0].settings.brassrotz = (t.value1+0.0);
					cmpStrConst( t_field_s, "brassrotzrand" );
					if( matched )  g.firemodes[t.gunid][0].settings.brassrotzrand = (t.value1+0.0);
					cmpStrConst( t_field_s, "brassdelay" );
					if( matched )  g.firemodes[t.gunid][0].settings.brassdelay = t.value1;
					cmpStrConst( t_field_s, "zoom brassdelay" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoombrassdelay = t.value1;

					//  Advanced Features
					cmpStrConst( t_field_s, "gravitygun" );
					if( matched )  g.firemodes[t.gunid][0].settings.gravitygun = t.value1;
					cmpStrConst( t_field_s, "magnet" );
					if( matched )  g.firemodes[t.gunid][0].settings.magnet = t.value1;
					cmpStrConst( t_field_s, "magnetstrength" );
					if( matched )  g.firemodes[t.gunid][0].settings.magnetstrength = t.value1;
					cmpStrConst( t_field_s, "gravitystrength" );
					if( matched )  g.firemodes[t.gunid][0].settings.gravitystrength = t.value1;
					cmpStrConst( t_field_s, "emptyreloadonly" );
					if( matched )  g.firemodes[t.gunid][0].settings.emptyreloadonly = t.value1;
					cmpStrConst( t_field_s, "overheatafter" );
					if( matched )  g.firemodes[t.gunid][0].settings.overheatafter = t.value1;
					cmpStrConst( t_field_s, "cooldown" );
					if( matched )  g.firemodes[t.gunid][0].settings.cooldown = t.value1;
					cmpStrConst( t_field_s, "jamchance" );
					if( matched )  g.firemodes[t.gunid][0].settings.jamchance = t.value1;
					cmpStrConst( t_field_s, "jamtimer" );
					if( matched )  g.firemodes[t.gunid][0].settings.mintimer = t.value1;
					cmpStrConst( t_field_s, "nosubmergedfire" );
					if( matched )  g.firemodes[t.gunid][0].settings.nosubmergedfire = t.value1;
					cmpStrConst( t_field_s, "burst" );
					if( matched )  g.firemodes[t.gunid][0].settings.burst = t.value1;
					cmpStrConst( t_field_s, "disablemovespeedmod" );
					if( matched )  g.firemodes[t.gunid][0].settings.movespeedmod = t.value1;
					cmpStrConst( t_field_s, "chamberedround" );
					if( matched )  g.firemodes[t.gunid][0].settings.chamberedround = t.value1;
					cmpStrConst( t_field_s, "noautoreload" );
					if( matched )  g.firemodes[t.gunid][0].settings.noautoreload = t.value1;
					cmpStrConst( t_field_s, "nofullreload" );
					if( matched )  g.firemodes[t.gunid][0].settings.nofullreload = t.value1;
					cmpStrConst( t_field_s, "runx" );
					if( matched )  g.firemodes[t.gunid][0].settings.runx_f = t.value1;
					cmpStrConst( t_field_s, "runy" );
					if( matched )  g.firemodes[t.gunid][0].settings.runy_f = t.value1;
					cmpStrConst( t_field_s, "runacc" );
					if( matched )  g.firemodes[t.gunid][0].settings.runaccuracy = t.value1;
					cmpStrConst( t_field_s, "runanimdelay" );
					if( matched )  g.firemodes[t.gunid][0].settings.runanimdelay = t.value1;
					cmpStrConst( t_field_s, "noscorch" );
					if( matched )  g.firemodes[t.gunid][0].settings.noscorch = t.value1;
					cmpStrConst( t_field_s, "melee noscorch" );
					if( matched )  g.firemodes[t.gunid][0].settings.meleenoscorch = t.value1;
					cmpStrConst( t_field_s, "simplezoom" );
					if( matched )  g.firemodes[t.gunid][0].settings.simplezoom = t.value1;
					cmpStrConst( t_field_s, "simplezoomx" );
					if( matched )  g.firemodes[t.gunid][0].settings.simplezoomx_f = t.value1;
					cmpStrConst( t_field_s, "simplezoomy" );
					if( matched )  g.firemodes[t.gunid][0].settings.simplezoomy_f = t.value1;
					cmpStrConst( t_field_s, "simplezoommod" );
					if( matched )  g.firemodes[t.gunid][0].settings.simplezoommod = t.value1;
					cmpStrConst( t_field_s, "simplezoomacc" );
					if( matched )  g.firemodes[t.gunid][0].settings.simplezoomacc = t.value1;
					cmpStrConst( t_field_s, "simplezoomspeed" );
					if( matched )  g.firemodes[t.gunid][0].settings.simplezoomspeed = t.value1;
					cmpStrConst( t_field_s, "simplezoomflash" );
					if( matched )  g.firemodes[t.gunid][0].settings.simplezoomflash = t.value1;
					cmpStrConst( t_field_s, "simplezoomanim" );
					if( matched )  g.firemodes[t.gunid][0].settings.simplezoomanim = t.value1;
					cmpStrConst( t_field_s, "gunlagspeed" );
					if( matched )  g.firemodes[t.gunid][0].settings.gunlagSpeed = t.value1;
					cmpStrConst( t_field_s, "gunlagxmax" );
					if( matched )  g.firemodes[t.gunid][0].settings.gunlagXmax = t.value1;
					cmpStrConst( t_field_s, "gunlagymax" );
					if( matched )  g.firemodes[t.gunid][0].settings.gunlagYmax = t.value1;
					cmpStrConst( t_field_s, "zoomgunlagspeed" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoomgunlagSpeed = t.value1;
					cmpStrConst( t_field_s, "zoomgunlagxmax" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoomgunlagXmax = t.value1;
					cmpStrConst( t_field_s, "zoomgunlagymax" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoomgunlagYmax = t.value1;
					cmpStrConst( t_field_s, "zoomwalkspeed" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoomwalkspeed = t.value1;
					cmpStrConst( t_field_s, "zoomturnspeed" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoomturnspeed = t.value1;
					cmpStrConst( t_field_s, "plrmovespeedmod" );
					if( matched )  g.firemodes[t.gunid][0].settings.plrmovespeedmod = t.value1;
					cmpStrConst( t_field_s, "plremptyspeedmod" );
					if( matched )  g.firemodes[t.gunid][0].settings.plremptyspeedmod = t.value1;
					cmpStrConst( t_field_s, "plrturnspeedmod" );
					if( matched )  g.firemodes[t.gunid][0].settings.plrturnspeedmod = t.value1;
					cmpStrConst( t_field_s, "plrjumpspeedmod" );
					if( matched )  g.firemodes[t.gunid][0].settings.plrjumpspeedmod = t.value1;
					cmpStrConst( t_field_s, "plrreloadspeedmod" );
					if( matched )  g.firemodes[t.gunid][0].settings.plrreloadspeedmod = t.value1;

					cmpStrConst( t_field_s, "soundstrength" );
					if( matched )  g.firemodes[t.gunid][0].settings.soundstrength = t.value1;
					cmpStrConst( t_field_s, "recoily" );
					if( matched )  g.firemodes[t.gunid][0].settings.recoily_f = t.value1;
					cmpStrConst( t_field_s, "recoilx" );
					if( matched )  g.firemodes[t.gunid][0].settings.recoilx_f = t.value1;
					cmpStrConst( t_field_s, "recoilyreturn" );
					if( matched )  g.firemodes[t.gunid][0].settings.recoilycorrect_f = t.value1;
					cmpStrConst( t_field_s, "recoilxreturn" );
					if( matched )  g.firemodes[t.gunid][0].settings.recoilxcorrect_f = t.value1;
					cmpStrConst( t_field_s, "zoomrecoily" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoomrecoily_f = t.value1;
					cmpStrConst( t_field_s, "zoomrecoilx" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoomrecoilx_f = t.value1;
					cmpStrConst( t_field_s, "zoomrecoilyreturn" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoomrecoilycorrect_f = t.value1;
					cmpStrConst( t_field_s, "zoomrecoilxreturn" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoomrecoilxcorrect_f = t.value1;
					cmpStrConst( t_field_s, "forcezoomout" );
					if( matched )  g.firemodes[t.gunid][0].settings.forcezoomout = t.value1;
					cmpStrConst( t_field_s, "ammomax" );
					if( matched )  g.firemodes[t.gunid][0].settings.ammomax = t.value1;
					cmpStrConst( t_field_s, "equipment" );
					if( matched )  g.firemodes[t.gunid][0].settings.equipment = t.value1;
					cmpStrConst( t_field_s, "lockcamera" );
					if( matched )  g.firemodes[t.gunid][0].settings.lockcamera = t.value1;

					// LB: for weapons like bow and arrow, fire anim starts and then arrow released some time later (in ms)
					cmpStrConst(t_field_s, "delayedshot");
					if (matched)  g.firemodes[t.gunid][0].settings.delayedshot = t.value1;
					cmpStrConst(t_field_s, "alt delayedshot");
					if (matched)  g.firemodes[t.gunid][1].settings.delayedshot = t.value1;

					// animchoicemode; 0 = default random as above, 1 = pure random allowing repeat anims to be chosen, 2 = sequential not random
					cmpStrConst(t_field_s, "animchoicemode");
					if (matched)  g.firemodes[t.gunid][0].settings.animchoicemode = t.value1;
					cmpStrConst(t_field_s, "alt animchoicemode");
					if (matched)  g.firemodes[t.gunid][1].settings.animchoicemode = t.value1;

					//  Gun model animation keyframe setting
					cmpStrConst( t_field_s, "keyframe ratio" );
					if( matched ) 
					{
						t.keyframeratio = t.value1;
					}

					//PE: This fails when called from entity_load , and t.currentgunobj == 0. We can use t.gun[t.gunid].obj instead.
					// if no comma and anim field, look up start and finish frames from object directly
					if (bFoundComma == false && iCurrentGunObj != 0)
					{
						int animfields = 0;
						bool bAnimationField = false;
						for (; animfields <= 87; animfields++)
						{
							LPSTR pFieldName = "";
							if (animfields == 0) pFieldName = "select";
							if (animfields == 1) pFieldName = "idle";
							if (animfields == 2) pFieldName = "runto";
							if (animfields == 3) pFieldName = "runfrom";
							if (animfields == 4) pFieldName = "move";
							if (animfields == 5) pFieldName = "run";
							if (animfields == 6) pFieldName = "fire";
							if (animfields == 7) pFieldName = "use";
							if (animfields == 8) pFieldName = "start fire";
							if (animfields == 9) pFieldName = "automatic fire";
							if (animfields == 10) pFieldName = "end fire";
							if (animfields == 11) pFieldName = "last start fire";
							if (animfields == 12) pFieldName = "last end fire";
							if (animfields == 13) pFieldName = "reload";
							if (animfields == 14) pFieldName = "start reload";
							if (animfields == 15) pFieldName = "reload loop";
							if (animfields == 16) pFieldName = "end reload";
							if (animfields == 17) pFieldName = "cock";
							if (animfields == 18) pFieldName = "putaway";
							if (animfields == 19) pFieldName = "start fire 2";
							if (animfields == 20) pFieldName = "end fire 2";
							if (animfields == 21) pFieldName = "start fire 3";
							if (animfields == 22) pFieldName = "end fire 3";
							if (animfields == 23) pFieldName = "fix jam";
							if (animfields == 24) pFieldName = "hand push";
							if (animfields == 25) pFieldName = "hand dead";
							if (animfields == 26) pFieldName = "jammed";
							if (animfields == 27) pFieldName = "change firemode";
							if (animfields == 28) pFieldName = "hand button";
							if (animfields == 29) pFieldName = "hand take";
							if (animfields == 30) pFieldName = "pull up";
							if (animfields == 31) pFieldName = "pull down";
							if (animfields == 32) pFieldName = "pull left";
							if (animfields == 33) pFieldName = "pull right";
							if (animfields == 34) pFieldName = "NOTAANIMFIELD"; //PE: "useempty" never use comma.
							if (animfields == 35) pFieldName = "empty shotgun";
							if (animfields == 36) pFieldName = "empty putaway";
							if (animfields == 37) pFieldName = "empty select";
							if (animfields == 38) pFieldName = "empty idle";
							if (animfields == 39) pFieldName = "empty runto";
							if (animfields == 40) pFieldName = "empty runfrom";
							if (animfields == 41) pFieldName = "empty move";
							if (animfields == 42) pFieldName = "empty run";
							if (animfields == 43) pFieldName = "empty start reload";
							if (animfields == 44) pFieldName = "empty reload loop";
							if (animfields == 45) pFieldName = "empty end reload";
							if (animfields == 46) pFieldName = "empty cock";
							if (animfields == 47) pFieldName = "empty reload";
							if (animfields == 48) pFieldName = "dryfire";
							if (animfields == 49) pFieldName = "melee start";
							if (animfields == 50) pFieldName = "melee start 2";
							if (animfields == 51) pFieldName = "melee start 3";
							if (animfields == 52) pFieldName = "melee end";
							if (animfields == 53) pFieldName = "melee end 2";
							if (animfields == 54) pFieldName = "melee end 3";
							if (animfields == 55) pFieldName = "empty melee start";
							if (animfields == 56) pFieldName = "empty melee start 2";
							if (animfields == 57) pFieldName = "empty melee start 3";
							if (animfields == 58) pFieldName = "empty melee end";
							if (animfields == 59) pFieldName = "empty melee end 2";
							if (animfields == 60) pFieldName = "empty melee end 3";
							if (animfields == 61) pFieldName = "empty zoomto";
							if (animfields == 62) pFieldName = "empty zoom idle";
							if (animfields == 63) pFieldName = "empty zoom move";
							if (animfields == 64) pFieldName = "empty zoomfrom";
							if (animfields == 65) pFieldName = "zoom dryfire";
							if (animfields == 66) pFieldName = "zoomto";
							if (animfields == 67) pFieldName = "zoomfrom";
							if (animfields == 68) pFieldName = "zoom start fire";
							if (animfields == 69) pFieldName = "zoom start fire 2";
							if (animfields == 70) pFieldName = "zoom start fire 3";
							if (animfields == 71) pFieldName = "zoom automatic fire";
							if (animfields == 72) pFieldName = "zoom end fire";
							if (animfields == 73) pFieldName = "zoom end fire 2";
							if (animfields == 74) pFieldName = "zoom end fire 3";
							if (animfields == 75) pFieldName = "zoom last start fire";
							if (animfields == 76) pFieldName = "zoom last end fire";
							if (animfields == 77) pFieldName = "zoom idle";
							if (animfields == 78) pFieldName = "zoom move";
							if (animfields == 79) pFieldName = "altto";
							if (animfields == 80) pFieldName = "altfrom";
							if (animfields == 81) pFieldName = "block start";
							if (animfields == 82) pFieldName = "block loop";
							if (animfields == 83) pFieldName = "block end";
							if (animfields == 84) pFieldName = "block fail";
							if (animfields == 85) pFieldName = "block counter";
							if (animfields == 86) pFieldName = "block counterfail";
							if (animfields == 87) pFieldName = "block fail 2";
							cmpStrConst(t_field_s, pFieldName);
							if (matched)
							{
								bAnimationField = true;
								break;
							}
							cstr pAltField = cstr("alt") + pFieldName;
							cmpStrConst(t_field_s, pAltField.Get());
							if (matched)
							{
								bAnimationField = true;
								break;
							}
						}
						if (bAnimationField == true)
						{
							if (fLastFinishFrameFromAnimName != -1)
							{				
								// works out the middle frame and sets this anim and the previous anims finish frame
								float fMarkMiddleFrameByAdvancingFromStart = t.value1;
								t.value1 = fLastStartFrameFromAnimName + fMarkMiddleFrameByAdvancingFromStart;
								t.value2 = fLastFinishFrameFromAnimName;
								if (iModifyTheLastFinishFrameWhenKnowPerc > 0 )
								{
									int iAltIndex = 0;
									if (skipAltSpace == true) iAltIndex = 1;
									if (iModifyTheLastFinishFrameWhenKnowPerc == 8) g.firemodes[t.gunid][iAltIndex].action.start.e = t.value1 - 1; // pFieldName = "start fire";
									if (iModifyTheLastFinishFrameWhenKnowPerc == 14) g.firemodes[t.gunid][iAltIndex].action.startreload.e = t.value1 - 1; // pFieldName = "start reload";
									if (iModifyTheLastFinishFrameWhenKnowPerc == 19) g.firemodes[t.gunid][iAltIndex].action.start2.e = t.value1 - 1; // pFieldName = "start fire 2";
									if (iModifyTheLastFinishFrameWhenKnowPerc == 21) g.firemodes[t.gunid][iAltIndex].action.start3.e = t.value1 - 1; // pFieldName = "start fire 3";
									if (iModifyTheLastFinishFrameWhenKnowPerc == 43) g.firemodes[t.gunid][iAltIndex].emptyaction.startreload.e = t.value1 - 1; // pFieldName = "empty start reload";
									if (iModifyTheLastFinishFrameWhenKnowPerc == 49) g.firemodes[t.gunid][iAltIndex].meleeaction.start.e = t.value1 - 1; // pFieldName = "melee start";
									if (iModifyTheLastFinishFrameWhenKnowPerc == 50) g.firemodes[t.gunid][iAltIndex].meleeaction.start2.e = t.value1 - 1; // pFieldName = "melee start 2";
									if (iModifyTheLastFinishFrameWhenKnowPerc == 51) g.firemodes[t.gunid][iAltIndex].meleeaction.start3.e = t.value1 - 1; // pFieldName = "melee start 3";
									if (iModifyTheLastFinishFrameWhenKnowPerc == 55) g.firemodes[t.gunid][iAltIndex].emptyaction.start.e = t.value1 - 1; // pFieldName = "empty melee start";
									if (iModifyTheLastFinishFrameWhenKnowPerc == 56) g.firemodes[t.gunid][iAltIndex].emptyaction.start2.e = t.value1 - 1; // pFieldName = "empty melee start 2";
									if (iModifyTheLastFinishFrameWhenKnowPerc == 57) g.firemodes[t.gunid][iAltIndex].emptyaction.start3.e = t.value1 - 1; // pFieldName = "empty melee start 3";
									if (iModifyTheLastFinishFrameWhenKnowPerc == 68) g.firemodes[t.gunid][iAltIndex].zoomaction.start.e = t.value1 - 1; // pFieldName = "zoom start fire";
									if (iModifyTheLastFinishFrameWhenKnowPerc == 69) g.firemodes[t.gunid][iAltIndex].zoomaction.start2.e = t.value1 - 1; // pFieldName = "zoom start fire 2";
									if (iModifyTheLastFinishFrameWhenKnowPerc == 70) g.firemodes[t.gunid][iAltIndex].zoomaction.start3.e = t.value1 - 1; // pFieldName = "zoom start fire 3";
									if (iModifyTheLastFinishFrameWhenKnowPerc == 75) g.firemodes[t.gunid][iAltIndex].zoomaction.laststart.e = t.value1 - 1; // pFieldName = "zoom last start fire";
									iModifyTheLastFinishFrameWhenKnowPerc = 0;
								}
								fLastFinishFrameFromAnimName = -1;
							}
							else
							{
								sObject* pObject = GetObjectData(iCurrentGunObj);
								float fFoundStart = 0, fFoundFinish = 0;
								extern int entity_lua_getanimationnamefromobject (sObject* pObject, cstr FindThisName_s, float* fFoundStart, float* fFoundFinish);
								if (entity_lua_getanimationnamefromobject(pObject, t.value_s, &fFoundStart, &fFoundFinish) > 0)
								{
									// found the frame strt and finish values from the object itself
									t.value1 = fFoundStart;
									t.value2 = fFoundFinish;

									// as this is the first of a START END set, chop end off
									bool bPartOfStartEndSet = false;
									if (animfields == 8) bPartOfStartEndSet = true; // pFieldName = "start fire";
									if (animfields == 14) bPartOfStartEndSet = true; // pFieldName = "start reload";
									if (animfields == 19) bPartOfStartEndSet = true; // pFieldName = "start fire 2";
									if (animfields == 21) bPartOfStartEndSet = true; // pFieldName = "start fire 3";
									if (animfields == 43) bPartOfStartEndSet = true; // pFieldName = "empty start reload";
									if (animfields == 49) bPartOfStartEndSet = true; // pFieldName = "melee start";
									if (animfields == 50) bPartOfStartEndSet = true; // pFieldName = "melee start 2";
									if (animfields == 51) bPartOfStartEndSet = true; // pFieldName = "melee start 3";
									if (animfields == 55) bPartOfStartEndSet = true; // pFieldName = "empty melee start";
									if (animfields == 56) bPartOfStartEndSet = true; // pFieldName = "empty melee start 2";
									if (animfields == 57) bPartOfStartEndSet = true; // pFieldName = "empty melee start 3";
									if (animfields == 68) bPartOfStartEndSet = true; // pFieldName = "zoom start fire";
									if (animfields == 69) bPartOfStartEndSet = true; // pFieldName = "zoom start fire 2";
									if (animfields == 70) bPartOfStartEndSet = true; // pFieldName = "zoom start fire 3";
									if (animfields == 75) bPartOfStartEndSet = true; // pFieldName = "zoom last start fire";
									if (bPartOfStartEndSet == true)
									{
										fLastStartFrameFromAnimName = t.value1;
										fLastFinishFrameFromAnimName = t.value2;
										fLastFinishSetSizeFromAnimName = t.value2 - t.value1;
										iModifyTheLastFinishFrameWhenKnowPerc = animfields;
									}
									else
									{
										fLastFinishFrameFromAnimName = -1;
									}
								}
							}
						}
					}

					//  Classic animations
					cmpStrConst( t_field_s, "select" );
					if( matched ) { g.firemodes[t.gunid][0].action.show.s = t.value1  ; g.firemodes[t.gunid][0].action.show.e = t.value2; }
					cmpStrConst( t_field_s, "idle" );
					if( matched ) { g.firemodes[t.gunid][0].action.idle.s = t.value1  ; g.firemodes[t.gunid][0].action.idle.e = t.value2; }
					cmpStrConst( t_field_s, "runto" );
					if( matched ) { g.firemodes[t.gunid][0].action.runto.s = t.value1  ; g.firemodes[t.gunid][0].action.runto.e = t.value2; }
					cmpStrConst( t_field_s, "runfrom" );
					if( matched ) { g.firemodes[t.gunid][0].action.runfrom.s = t.value1  ; g.firemodes[t.gunid][0].action.runfrom.e = t.value2; }
					cmpStrConst( t_field_s, "move" );
					if( matched ) { g.firemodes[t.gunid][0].action.move.s = t.value1  ; g.firemodes[t.gunid][0].action.move.e = t.value2; }
					cmpStrConst( t_field_s, "run" );
					if( matched ) { g.firemodes[t.gunid][0].action.run.s = t.value1  ; g.firemodes[t.gunid][0].action.run.e = t.value2; }
					cmpStrConst( t_field_s, "fire" );
					if( matched ) { g.firemodes[t.gunid][0].action.start.s = t.value1  ; g.firemodes[t.gunid][0].action.start.e = t.value1 ; g.firemodes[t.gunid][0].action.finish.s = t.value1 ; g.firemodes[t.gunid][0].action.finish.e = t.value2; }
					cmpStrConst( t_field_s, "use" );
					if( matched ) { g.firemodes[t.gunid][0].action.start.s = t.value1  ; g.firemodes[t.gunid][0].action.start.e = t.value1 ; g.firemodes[t.gunid][0].action.finish.s = t.value1 ; g.firemodes[t.gunid][0].action.finish.e = t.value2; }
					cmpStrConst( t_field_s, "start fire" );
					if( matched ) { g.firemodes[t.gunid][0].action.start.s = t.value1  ; g.firemodes[t.gunid][0].action.start.e = t.value2; }
					cmpStrConst( t_field_s, "automatic fire" );
					if( matched ) { g.firemodes[t.gunid][0].action.automatic.s = t.value1  ; g.firemodes[t.gunid][0].action.automatic.e = t.value2; }
					cmpStrConst( t_field_s, "end fire" );
					if( matched ) { g.firemodes[t.gunid][0].action.finish.s = t.value1  ; g.firemodes[t.gunid][0].action.finish.e = t.value2; }
					cmpStrConst( t_field_s, "last start fire" );
					if( matched ) { g.firemodes[t.gunid][0].action.laststart.s = t.value1  ; g.firemodes[t.gunid][0].action.laststart.e = t.value2; }
					cmpStrConst( t_field_s, "last end fire" );
					if( matched ) { g.firemodes[t.gunid][0].action.lastfinish.s = t.value1  ; g.firemodes[t.gunid][0].action.lastfinish.e = t.value2; }
					cmpStrConst( t_field_s, "reload" );
					if( matched ) 
					{
						g.firemodes[t.gunid][0].action.startreload.s=t.value1 ; g.firemodes[t.gunid][0].action.startreload.e=t.value2;
						g.firemodes[t.gunid][0].action.reloadloop.s=t.value2 ; g.firemodes[t.gunid][0].action.reloadloop.e=t.value2;
						g.firemodes[t.gunid][0].action.endreload.s=t.value2 ; g.firemodes[t.gunid][0].action.endreload.e=t.value2;
					}
					cmpStrConst( t_field_s, "start reload" );
					if( matched ) { g.firemodes[t.gunid][0].action.startreload.s = t.value1  ; g.firemodes[t.gunid][0].action.startreload.e = t.value2; }
					cmpStrConst( t_field_s, "reload loop" );
					if( matched ) { g.firemodes[t.gunid][0].action.reloadloop.s = t.value1  ; g.firemodes[t.gunid][0].action.reloadloop.e = t.value2; }
					cmpStrConst( t_field_s, "end reload" );
					if( matched ) { g.firemodes[t.gunid][0].action.endreload.s = t.value1  ; g.firemodes[t.gunid][0].action.endreload.e = t.value2; }
					cmpStrConst( t_field_s, "cock" );
					if( matched ) { g.firemodes[t.gunid][0].action.cock.s = t.value1  ; g.firemodes[t.gunid][0].action.cock.e = t.value2; }
					cmpStrConst( t_field_s, "putaway" );
					if( matched ) { g.firemodes[t.gunid][0].action.hide.s = t.value1  ; g.firemodes[t.gunid][0].action.hide.e = t.value2; }
					cmpStrConst( t_field_s, "start fire 2" );
					if( matched ) { g.firemodes[t.gunid][0].action.start2.s = t.value1  ; g.firemodes[t.gunid][0].action.start2.e = t.value2; }
					cmpStrConst( t_field_s, "end fire 2" );
					if( matched ) { g.firemodes[t.gunid][0].action.finish2.s = t.value1  ; g.firemodes[t.gunid][0].action.finish2.e = t.value2; }
					cmpStrConst( t_field_s, "start fire 3" );
					if( matched ) { g.firemodes[t.gunid][0].action.start3.s = t.value1  ; g.firemodes[t.gunid][0].action.start3.e = t.value2; }
					cmpStrConst( t_field_s, "end fire 3" );
					if( matched ) { g.firemodes[t.gunid][0].action.finish3.s = t.value1  ; g.firemodes[t.gunid][0].action.finish3.e = t.value2; }

					//  Advanced animations
					cmpStrConst( t_field_s, "fix jam" );
					if( matched ) { g.firemodes[t.gunid][0].action2.clearjam.s = t.value1  ; g.firemodes[t.gunid][0].action2.clearjam.e = t.value2; }
					cmpStrConst( t_field_s, "hand push" );
					if( matched ) { g.firemodes[t.gunid][0].action2.handpush.s = t.value1  ; g.firemodes[t.gunid][0].action2.handpush.e = t.value2; }
					cmpStrConst( t_field_s, "hand dead" );
					if( matched ) { g.firemodes[t.gunid][0].action2.handdead.s = t.value1  ; g.firemodes[t.gunid][0].action2.handdead.e = t.value2; }
					cmpStrConst( t_field_s, "jammed" );
					if( matched ) { g.firemodes[t.gunid][0].action2.jammed.s = t.value1  ; g.firemodes[t.gunid][0].action2.jammed.e = t.value2; }
					cmpStrConst( t_field_s, "change firemode" );
					if( matched ) { g.firemodes[t.gunid][0].action2.swaptoalt.s = t.value1  ; g.firemodes[t.gunid][0].action2.swaptoalt.e = t.value2; }
					cmpStrConst( t_field_s, "hand button" );
					if( matched ) { g.firemodes[t.gunid][0].action2.handbutton.s = t.value1  ; g.firemodes[t.gunid][0].action2.handbutton.e = t.value2; }
					cmpStrConst( t_field_s, "hand take" );
					if( matched ) { g.firemodes[t.gunid][0].action2.handtake.s = t.value1  ; g.firemodes[t.gunid][0].action2.handtake.e = t.value2; }
					cmpStrConst( t_field_s, "pull up" );
					if( matched ) { g.firemodes[t.gunid][0].action2.pullup.s = t.value1  ; g.firemodes[t.gunid][0].action2.pullup.e = t.value2; }
					cmpStrConst( t_field_s, "pull down" );
					if( matched ) { g.firemodes[t.gunid][0].action2.pulldown.s = t.value1  ; g.firemodes[t.gunid][0].action2.pulldown.e = t.value2; }
					cmpStrConst( t_field_s, "pull left" );
					if( matched ) { g.firemodes[t.gunid][0].action2.pullleft.s = t.value1  ; g.firemodes[t.gunid][0].action2.pullleft.e = t.value2; }
					cmpStrConst( t_field_s, "pull right" );
					if( matched ) { g.firemodes[t.gunid][0].action2.pullright.s = t.value1  ; g.firemodes[t.gunid][0].action2.pullright.e = t.value2; }

					//  Empty animations
					cmpStrConst( t_field_s, "useempty" );
					if( matched )  g.firemodes[t.gunid][0].settings.hasempty = t.value1;
					cmpStrConst( t_field_s, "empty shotgun" );
					if( matched )  g.firemodes[t.gunid][0].settings.emptyshotgun = t.value1;
					cmpStrConst( t_field_s, "empty putaway" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.hide.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.hide.e = t.value2; }
					cmpStrConst( t_field_s, "empty select" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.show.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.show.e = t.value2; }
					cmpStrConst( t_field_s, "empty idle" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.idle.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.idle.e = t.value2; }
					cmpStrConst( t_field_s, "empty runto" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.runto.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.runto.e = t.value2; }
					cmpStrConst( t_field_s, "empty runfrom" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.runfrom.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.runfrom.e = t.value2; }
					cmpStrConst( t_field_s, "empty move" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.move.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.move.e = t.value2; }
					cmpStrConst( t_field_s, "empty run" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.run.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.run.e = t.value2; }
					cmpStrConst( t_field_s, "empty start reload" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.startreload.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.startreload.e = t.value2; }
					cmpStrConst( t_field_s, "empty reload loop" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.reloadloop.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.reloadloop.e = t.value2; }
					cmpStrConst( t_field_s, "empty end reload" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.endreload.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.endreload.e = t.value2; }
					cmpStrConst( t_field_s, "empty cock" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.cock.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.cock.e = t.value2; }
					cmpStrConst( t_field_s, "empty reload" );
					if( matched ) 
					{
						g.firemodes[t.gunid][0].emptyaction.startreload.s=t.value1 ; g.firemodes[t.gunid][0].emptyaction.startreload.e=t.value2;
						g.firemodes[t.gunid][0].emptyaction.reloadloop.s=t.value2 ; g.firemodes[t.gunid][0].emptyaction.reloadloop.e=t.value2;
						g.firemodes[t.gunid][0].emptyaction.endreload.s=t.value2 ; g.firemodes[t.gunid][0].emptyaction.endreload.e=t.value2;
					}
					cmpStrConst( t_field_s, "dryfire" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.dryfire.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.dryfire.e = t.value2; }

					//  Melee Animations
					cmpStrConst( t_field_s, "melee start" );
					if( matched ) { g.firemodes[t.gunid][0].meleeaction.start.s = t.value1  ; g.firemodes[t.gunid][0].meleeaction.start.e = t.value2; }
					cmpStrConst( t_field_s, "melee start 2" );
					if( matched ) { g.firemodes[t.gunid][0].meleeaction.start2.s = t.value1  ; g.firemodes[t.gunid][0].meleeaction.start2.e = t.value2; }
					cmpStrConst( t_field_s, "melee start 3" );
					if( matched ) { g.firemodes[t.gunid][0].meleeaction.start3.s = t.value1  ; g.firemodes[t.gunid][0].meleeaction.start3.e = t.value2; }
					cmpStrConst( t_field_s, "melee end" );
					if( matched ) { g.firemodes[t.gunid][0].meleeaction.finish.s = t.value1  ; g.firemodes[t.gunid][0].meleeaction.finish.e = t.value2; }
					cmpStrConst( t_field_s, "melee end 2" );
					if( matched ) { g.firemodes[t.gunid][0].meleeaction.finish2.s = t.value1  ; g.firemodes[t.gunid][0].meleeaction.finish2.e = t.value2; }
					cmpStrConst( t_field_s, "melee end 3" );
					if( matched ) { g.firemodes[t.gunid][0].meleeaction.finish3.s = t.value1  ; g.firemodes[t.gunid][0].meleeaction.finish3.e = t.value2; }
					cmpStrConst( t_field_s, "empty melee start" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.start.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.start.e = t.value2; }
					cmpStrConst( t_field_s, "empty melee start 2" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.start2.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.start2.e = t.value2; }
					cmpStrConst( t_field_s, "empty melee start 3" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.start3.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.start3.e = t.value2; }
					cmpStrConst( t_field_s, "empty melee end" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.finish.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.finish.e = t.value2; }
					cmpStrConst( t_field_s, "empty melee end 2" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.finish2.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.finish2.e = t.value2; }
					cmpStrConst( t_field_s, "empty melee end 3" );
					if( matched ) { g.firemodes[t.gunid][0].emptyaction.finish3.s = t.value1  ; g.firemodes[t.gunid][0].emptyaction.finish3.e = t.value2; }
						
					//  Zoom Animations
					cmpStrConst( t_field_s, "empty zoomto" );
					if( matched ) { g.firemodes[t.gunid][0].emptyzoomactionshow.s = t.value1  ; g.firemodes[t.gunid][0].emptyzoomactionshow.e = t.value2; }
					cmpStrConst( t_field_s, "empty zoom idle" );
					if( matched ) { g.firemodes[t.gunid][0].emptyzoomactionidle.s = t.value1 ; g.firemodes[t.gunid][0].emptyzoomactionidle.e = t.value2; }
					cmpStrConst( t_field_s, "empty zoom move" );
					if( matched ) { g.firemodes[t.gunid][0].emptyzoomactionmove.s = t.value1  ; g.firemodes[t.gunid][0].emptyzoomactionmove.e = t.value2; }
					cmpStrConst( t_field_s, "empty zoomfrom" );
					if( matched ) { g.firemodes[t.gunid][0].emptyzoomactionhide.s = t.value1  ; g.firemodes[t.gunid][0].emptyzoomactionhide.e = t.value2; }
					cmpStrConst( t_field_s, "zoom dryfire" );
					if( matched ) { g.firemodes[t.gunid][0].emptyzoomactiondryfire.s = t.value1  ; g.firemodes[t.gunid][0].emptyzoomactiondryfire.e = t.value2; }
					cmpStrConst( t_field_s, "zoomto" );
					if( matched ) { g.firemodes[t.gunid][0].zoomaction.show.s = t.value1  ; g.firemodes[t.gunid][0].zoomaction.show.e = t.value2; }
					cmpStrConst( t_field_s, "zoomfrom" );
					if( matched ) { g.firemodes[t.gunid][0].zoomaction.hide.s = t.value1  ; g.firemodes[t.gunid][0].zoomaction.hide.e = t.value2; }
					cmpStrConst( t_field_s, "zoom start fire" );
					if( matched ) { g.firemodes[t.gunid][0].zoomaction.start.s = t.value1  ; g.firemodes[t.gunid][0].zoomaction.start.e = t.value2; }
					cmpStrConst( t_field_s, "zoom start fire 2" );
					if( matched ) { g.firemodes[t.gunid][0].zoomaction.start2.s = t.value1  ; g.firemodes[t.gunid][0].zoomaction.start2.e = t.value2; }
					cmpStrConst( t_field_s, "zoom start fire 3" );
					if( matched ) { g.firemodes[t.gunid][0].zoomaction.start3.s = t.value1  ; g.firemodes[t.gunid][0].zoomaction.start3.e = t.value2; }
					cmpStrConst( t_field_s, "zoom automatic fire" );
					if( matched ) { g.firemodes[t.gunid][0].zoomaction.automatic.s = t.value1  ; g.firemodes[t.gunid][0].zoomaction.automatic.e = t.value2; }
					cmpStrConst( t_field_s, "zoom end fire" );
					if( matched ) { g.firemodes[t.gunid][0].zoomaction.finish.s = t.value1  ; g.firemodes[t.gunid][0].zoomaction.finish.e = t.value2; }
					cmpStrConst( t_field_s, "zoom end fire 2" );
					if( matched ) { g.firemodes[t.gunid][0].zoomaction.finish2.s = t.value1  ; g.firemodes[t.gunid][0].zoomaction.finish2.e = t.value2; }
					cmpStrConst( t_field_s, "zoom end fire 3" );
					if( matched ) { g.firemodes[t.gunid][0].zoomaction.finish3.s = t.value1  ; g.firemodes[t.gunid][0].zoomaction.finish3.e = t.value2; }
					cmpStrConst( t_field_s, "zoom last start fire" );
					if( matched ) { g.firemodes[t.gunid][0].zoomaction.laststart.s = t.value1  ; g.firemodes[t.gunid][0].zoomaction.laststart.e = t.value2; }
					cmpStrConst( t_field_s, "zoom last end fire" );
					if( matched ) { g.firemodes[t.gunid][0].zoomaction.lastfinish.s = t.value1  ; g.firemodes[t.gunid][0].zoomaction.lastfinish.e = t.value2; }
					cmpStrConst( t_field_s, "zoom idle" );
					if( matched ) { g.firemodes[t.gunid][0].zoomaction.idle.s = t.value1  ; g.firemodes[t.gunid][0].zoomaction.idle.e = t.value2; }
					cmpStrConst( t_field_s, "zoom move" );
					if( matched ) { g.firemodes[t.gunid][0].zoomaction.move.s = t.value1  ; g.firemodes[t.gunid][0].zoomaction.move.e = t.value2; }

					// more animations supported
					cmpStrConst(t_field_s, "block start");
					if (matched) { g.firemodes[t.gunid][0].blockaction.start.s = t.value1; g.firemodes[t.gunid][0].blockaction.start.e = t.value2; }
					cmpStrConst(t_field_s, "block loop");
					if (matched) { g.firemodes[t.gunid][0].blockaction.idle.s = t.value1; g.firemodes[t.gunid][0].blockaction.idle.e = t.value2; }
					cmpStrConst(t_field_s, "block end");
					if (matched) { g.firemodes[t.gunid][0].blockaction.finish.s = t.value1; g.firemodes[t.gunid][0].blockaction.finish.e = t.value2; }
					cmpStrConst(t_field_s, "block fail");
					if (matched) { g.firemodes[t.gunid][0].blockaction.dryfire.s = t.value1; g.firemodes[t.gunid][0].blockaction.dryfire.e = t.value2; }
					cmpStrConst(t_field_s, "block counter");
					if (matched) { g.firemodes[t.gunid][0].blockaction.start2.s = t.value1; g.firemodes[t.gunid][0].blockaction.start2.e = t.value2; }
					cmpStrConst(t_field_s, "block counterfail");
					if (matched) { g.firemodes[t.gunid][0].blockaction.finish2.s = t.value1; g.firemodes[t.gunid][0].blockaction.finish2.e = t.value2; }
					cmpStrConst(t_field_s, "block fail 2");
					if (matched) { g.firemodes[t.gunid][0].blockaction.flattentochest.s = t.value1; g.firemodes[t.gunid][0].blockaction.flattentochest.e = t.value2; }

					//  Gun repeat-fire sound
					cmpStrConst( t_field_s, "fireloop" );
					if( matched )  g.firemodes[t.gunid][0].sound.fireloopend = t.value1;

					// 280618 - Special active/idle sound loop triggers
					cmpStrConst( t_field_s, "loopsound" );
					if( matched )  g.firemodes[t.gunid][0].sound.loopsound = t.value1;
					cmpStrConst( t_field_s, "empty loopsound" );
					if( matched )  g.firemodes[t.gunid][0].sound.emptyloopsound = t.value1;

					//  Gun and muzzle alignment
					cmpStrConst( t_field_s, "horiz" );
					if( matched )  g.firemodes[t.gunid][0].horiz_f = t.value1;
					cmpStrConst( t_field_s, "vert" );
					if( matched )  g.firemodes[t.gunid][0].vert_f = t.value1;
					cmpStrConst( t_field_s, "forward" );
					if( matched )  g.firemodes[t.gunid][0].forward_f = t.value1;
					cmpStrConst( t_field_s, "alignx" );
					if( matched )  g.firemodes[t.gunid][0].settings.muzzlex_f = t.value1;
					cmpStrConst( t_field_s, "aligny" );
					if( matched )  g.firemodes[t.gunid][0].settings.muzzley_f = t.value1;
					cmpStrConst( t_field_s, "alignz" );
					if( matched )  g.firemodes[t.gunid][0].settings.muzzlez_f = t.value1;
					cmpStrConst( t_field_s, "zoomalignx" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoommuzzlex_f = t.value1;
					cmpStrConst( t_field_s, "zoomaligny" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoommuzzley_f = t.value1;
					cmpStrConst( t_field_s, "zoomalignz" );
					if( matched )  g.firemodes[t.gunid][0].settings.zoommuzzlez_f = t.value1;
					cmpStrConst( t_field_s, "rotx" );
					if( matched )  g.firemodes[t.gunid][0].settings.rotx_f = t.value1;
					cmpStrConst( t_field_s, "roty" );
					if( matched )  g.firemodes[t.gunid][0].settings.roty_f = t.value1;
					cmpStrConst( t_field_s, "rotz" );
					if( matched )  g.firemodes[t.gunid][0].settings.rotz_f = t.value1;

					//  Gun particle effect
					cmpStrConst( t_field_s, "particledecal" );
					if( matched )  g.firemodes[t.gunid][0].particle.decal_s = t.value_s;
					cmpStrConst( t_field_s, "particlespeed" );
					if( matched )  g.firemodes[t.gunid][0].particle.velocity = t.value1;

					//  Advanced ammo pooling system
					cmpStrConst( t_field_s, "poolammo" );
					if( matched ) 
					{
						if (  t.value_s != "" ) 
						{
							t.poolindex=0 ; t.emptyindex=0;
							for ( t.y = 1 ; t.y<=  100; t.y++ )
							{
								if (  t.ammopool[t.y].name_s == t.value_s ) { t.poolindex = t.y  ; break; }
								if (  t.ammopool[t.y].used == 0 && t.emptyindex == 0  )  t.emptyindex = t.y;
							}
							if (  t.poolindex == 0  )  t.poolindex = t.emptyindex;
							if (  t.poolindex>0 ) 
							{
								g.firemodes[t.gunid][0].settings.poolindex=t.poolindex;
								t.ammopool[t.poolindex].used=1 ; t.ammopool[t.poolindex].name_s=t.value_s;
							}
						}
					}
					

					// Alt


					// 140616 - Whether fire mode affects projectile spot lighting
					cmpStrConst( t_field_s, "altusespotlighting" );
					if( matched ) g.firemodes[t.gunid][1].settings.usespotlighting = t.value1;

					//  Classic details
					cmpStrConst( t_field_s, "altmuzzlesize" );
					if( matched )  g.firemodes[t.gunid][1].settings.muzzlesize_f = t.value1;
					cmpStrConst( t_field_s, "altmuzzlecolorr" );
					if( matched )  g.firemodes[t.gunid][1].settings.muzzlecolorr = t.value1;
					cmpStrConst( t_field_s, "altmuzzlecolorg" );
					if( matched )  g.firemodes[t.gunid][1].settings.muzzlecolorg = t.value1;
					cmpStrConst( t_field_s, "altmuzzlecolorb" );
					if( matched )  g.firemodes[t.gunid][1].settings.muzzlecolorb = t.value1;
					cmpStrConst( t_field_s, "altsmoke" );
					if( matched )  g.firemodes[t.gunid][1].settings.smoke = t.value1;
					cmpStrConst( t_field_s, "altsmokesize" );
					if( matched )  g.firemodes[t.gunid][1].settings.smokesize = t.value1;
					cmpStrConst( t_field_s, "altsmokespeed" );
					if( matched )  g.firemodes[t.gunid][1].settings.smokespeed = t.value1;
					cmpStrConst( t_field_s, "altsmokedecal" );
					if( matched )  g.firemodes[t.gunid][1].settings.smokedecal_s = t.value_s;
					cmpStrConst( t_field_s, "altflak" );
					if( matched )  g.firemodes[t.gunid][1].settings.flakname_s = t.value_s;
					cmpStrConst( t_field_s, "altflakrearmframe" );
					if( matched )  g.firemodes[t.gunid][1].settings.flakrearmframe = t.value1;
					cmpStrConst( t_field_s, "altreloadqty" );
					if( matched )  g.firemodes[t.gunid][1].settings.reloadqty = t.value1;
					cmpStrConst( t_field_s, "altreloadspeed" );
					if( matched )  g.firemodes[t.gunid][1].settings.reloadspeed = (t.value1+0.0)/100.0;
					cmpStrConst( t_field_s, "altiterate" );
					if( matched )  g.firemodes[t.gunid][1].settings.iterate = t.value1;
					cmpStrConst( t_field_s, "altrange" );
					if( matched )  g.firemodes[t.gunid][1].settings.range = t.value1;
					cmpStrConst( t_field_s, "accuracy" );
					if( matched )  g.firemodes[t.gunid][1].settings.accuracy = t.value1;
					cmpStrConst( t_field_s, "altdamage" );
					if( matched )  g.firemodes[t.gunid][1].settings.damage = t.value1;
					cmpStrConst( t_field_s, "altdropoff" );
					if( matched )  g.firemodes[t.gunid][1].settings.dropoff = t.value1;

					cmpStrConst(t_field_s, "altclipcapacity");
					if (matched)  g.firemodes[t.gunid][1].settings.clipcapacity = t.value1;

					cmpStrConst( t_field_s, "altmuzzleflash" );
					if( matched )  g.firemodes[t.gunid][1].settings.muzzleflash = t.value1;
					cmpStrConst( t_field_s, "altfirerate" );
					if( matched )  g.firemodes[t.gunid][1].settings.firerate = t.value1;
					cmpStrConst( t_field_s, "detectcoloff" );
					if( matched )  g.firemodes[t.gunid][1].settings.detectcoloff = t.value1;
					cmpStrConst( t_field_s, "ignorematerial" );
					if( matched )  g.firemodes[t.gunid][1].settings.ignorematerial = t.value1;
					cmpStrConst( t_field_s, "altzoommode" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoommode = t.value1;
					cmpStrConst( t_field_s, "altzoomaccuracy" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoomaccuracy = t.value1;
					cmpStrConst( t_field_s, "altzoomaccuracybreathhold" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoomaccuracybreathhold = t.value1;
					cmpStrConst( t_field_s, "altzoomaccuracybreath" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoomaccuracybreath = t.value1;
					cmpStrConst( t_field_s, "altzoomaccuracyheld" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoomaccuracyheld = t.value1;
					cmpStrConst( t_field_s, "altzoomscope" );
					if( matched )  g.firemodes[t.gunid][1].zoomscope_s = t.value_s;
					cmpStrConst( t_field_s, "altsecond" );
					if( matched )  g.firemodes[t.gunid][1].settings.seconduse = t.value1;
					cmpStrConst( t_field_s, "altdamagetype" );
					if( matched )  g.firemodes[t.gunid][1].settings.damagetype = t.value1;
					cmpStrConst( t_field_s, "altscorchtype" );
					if( matched )  g.firemodes[t.gunid][1].settings.scorchtype = t.value1;
					cmpStrConst( t_field_s, "altmelee damage" );
					if( matched )  g.firemodes[t.gunid][1].settings.meleedamage = t.value1;
					cmpStrConst( t_field_s, "altmelee range" );
					if( matched )  g.firemodes[t.gunid][1].settings.meleerange = t.value1;
					cmpStrConst( t_field_s, "altdofdistance" );
					if( matched )  g.firemodes[t.gunid][1].settings.dofdistance = t.value1;
					cmpStrConst( t_field_s, "altdofintensity" );
					if( matched )  g.firemodes[t.gunid][1].settings.dofintensity = t.value1;
					cmpStrConst( t_field_s, "altdecal" );
					if( matched )  g.firemodes[t.gunid][1].decal_s = t.value_s;
					cmpStrConst( t_field_s, "altdecalforward" );
					if( matched )  g.firemodes[t.gunid][1].settings.decalforward = t.value1;
					cmpStrConst( t_field_s, "altshotgun" );
					if( matched )  g.firemodes[t.gunid][1].settings.shotgun = t.value1;
					cmpStrConst(t_field_s, "altdisablerunandshoot");
					if (matched)  g.firemodes[t.gunid][1].settings.disablerunandshoot = t.value1;
					cmpStrConst(t_field_s, "altmeleewithrightclick");
					if (matched)  g.firemodes[t.gunid][1].settings.meleewithrightclick = t.value1;
					cmpStrConst(t_field_s, "altblockwithrightclick");
					if (matched)  g.firemodes[t.gunid][1].settings.blockwithrightclick = t.value1;			
					cmpStrConst(t_field_s, "altmeleequickrepeat");
					if (matched)  g.firemodes[t.gunid][1].settings.meleequickrepeat = t.value1;
					
					cmpStrConst( t_field_s, "altforce" );
					if( matched )  g.firemodes[t.gunid][1].settings.force = t.value1;
					cmpStrConst( t_field_s, "altmelee force" );
					if( matched )  g.firemodes[t.gunid][1].settings.meleeforce = t.value1;
					cmpStrConst( t_field_s, "altnpcignorereload" );
					if( matched )  g.firemodes[t.gunid][1].settings.npcignorereload = t.value1;
					cmpStrConst( t_field_s, "altzoomhidecrosshair" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoomhidecrosshair = t.value1;
					cmpStrConst( t_field_s, "altdoesnotuseammo" );
					if( matched )  g.firemodes[t.gunid][1].settings.doesnotuseammo = t.value1;
					cmpStrConst( t_field_s, "usemeleedamageonly" );
					if( matched )  g.firemodes[t.gunid][1].settings.usemeleedamageonly = t.value1;

					//  Brass details
					cmpStrConst( t_field_s, "altbrass" );
					if( matched )  g.firemodes[t.gunid][1].settings.brass = t.value1;
					cmpStrConst( t_field_s, "altbrasslife" );
					if( matched )  g.firemodes[t.gunid][1].settings.brasslife = (t.value1+0.0);
					cmpStrConst( t_field_s, "altbrassangle" );
					if( matched )  g.firemodes[t.gunid][1].settings.brassangle = (t.value1+0.0);
					cmpStrConst( t_field_s, "altbrassanglerand" );
					if( matched )  g.firemodes[t.gunid][1].settings.brassanglerand = (t.value1+0.0);
					cmpStrConst( t_field_s, "altbrassspeed" );
					if( matched )  g.firemodes[t.gunid][1].settings.brassspeed = (t.value1+0.0)/100.0;
					cmpStrConst( t_field_s, "altbrassspeedrand" );
					if( matched )  g.firemodes[t.gunid][1].settings.brassspeedrand = (t.value1+0.0)/100.0;
					cmpStrConst( t_field_s, "altbrassupward" );
					if( matched )  g.firemodes[t.gunid][1].settings.brassupward = (t.value1+0.0)/100.0;
					cmpStrConst( t_field_s, "altbrassupwardrand" );
					if( matched )  g.firemodes[t.gunid][1].settings.brassupwardrand = (t.value1+0.0)/100.0;
					cmpStrConst( t_field_s, "altbrassrotx" );
					if( matched )  g.firemodes[t.gunid][1].settings.brassrotx = (t.value1+0.0);
					cmpStrConst( t_field_s, "altbrassrotxrand" );
					if( matched )  g.firemodes[t.gunid][1].settings.brassrotxrand = (t.value1+0.0);
					cmpStrConst( t_field_s, "altbrassroty" );
					if( matched )  g.firemodes[t.gunid][1].settings.brassroty = (t.value1+0.0);
					cmpStrConst( t_field_s, "altbrassrotyrand" );
					if( matched )  g.firemodes[t.gunid][1].settings.brassrotyrand = (t.value1+0.0);
					cmpStrConst( t_field_s, "altbrassrotz" );
					if( matched )  g.firemodes[t.gunid][1].settings.brassrotz = (t.value1+0.0);
					cmpStrConst( t_field_s, "altbrassrotzrand" );
					if( matched )  g.firemodes[t.gunid][1].settings.brassrotzrand = (t.value1+0.0);
					cmpStrConst( t_field_s, "altbrassdelay" );
					if( matched )  g.firemodes[t.gunid][1].settings.brassdelay = t.value1;
					cmpStrConst( t_field_s, "altzoom brassdelay" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoombrassdelay = t.value1;

					//  Advanced Features
					cmpStrConst( t_field_s, "altgravitygun" );
					if( matched )  g.firemodes[t.gunid][1].settings.gravitygun = t.value1;
					cmpStrConst( t_field_s, "altmagnet" );
					if( matched )  g.firemodes[t.gunid][1].settings.magnet = t.value1;
					cmpStrConst( t_field_s, "altmagnetstrength" );
					if( matched )  g.firemodes[t.gunid][1].settings.magnetstrength = t.value1;
					cmpStrConst( t_field_s, "altgravitystrength" );
					if( matched )  g.firemodes[t.gunid][1].settings.gravitystrength = t.value1;
					cmpStrConst( t_field_s, "altemptyreloadonly" );
					if( matched )  g.firemodes[t.gunid][1].settings.emptyreloadonly = t.value1;
					cmpStrConst( t_field_s, "altoverheatafter" );
					if( matched )  g.firemodes[t.gunid][1].settings.overheatafter = t.value1;
					cmpStrConst( t_field_s, "altcooldown" );
					if( matched )  g.firemodes[t.gunid][1].settings.cooldown = t.value1;
					cmpStrConst( t_field_s, "altjamchance" );
					if( matched )  g.firemodes[t.gunid][1].settings.jamchance = t.value1;
					cmpStrConst( t_field_s, "altjamtimer" );
					if( matched )  g.firemodes[t.gunid][1].settings.mintimer = t.value1;
					cmpStrConst( t_field_s, "altnosubmergedfire" );
					if( matched )  g.firemodes[t.gunid][1].settings.nosubmergedfire = t.value1;
					cmpStrConst( t_field_s, "altburst" );
					if( matched )  g.firemodes[t.gunid][1].settings.burst = t.value1;
					cmpStrConst( t_field_s, "altdisablemovespeedmod" );
					if( matched )  g.firemodes[t.gunid][1].settings.movespeedmod = t.value1;
					cmpStrConst( t_field_s, "altchamberedround" );
					if( matched )  g.firemodes[t.gunid][1].settings.chamberedround = t.value1;
					cmpStrConst( t_field_s, "altnoautoreload" );
					if( matched )  g.firemodes[t.gunid][1].settings.noautoreload = t.value1;
					cmpStrConst( t_field_s, "altnofullreload" );
					if( matched )  g.firemodes[t.gunid][1].settings.nofullreload = t.value1;
					cmpStrConst( t_field_s, "altrunx" );
					if( matched )  g.firemodes[t.gunid][1].settings.runx_f = t.value1;
					cmpStrConst( t_field_s, "altruny" );
					if( matched )  g.firemodes[t.gunid][1].settings.runy_f = t.value1;
					cmpStrConst( t_field_s, "altrunacc" );
					if( matched )  g.firemodes[t.gunid][1].settings.runaccuracy = t.value1;
					cmpStrConst( t_field_s, "altrunanimdelay" );
					if( matched )  g.firemodes[t.gunid][1].settings.runanimdelay = t.value1;
					cmpStrConst( t_field_s, "altnoscorch" );
					if( matched )  g.firemodes[t.gunid][1].settings.noscorch = t.value1;
					cmpStrConst( t_field_s, "altmelee noscorch" );
					if( matched )  g.firemodes[t.gunid][1].settings.meleenoscorch = t.value1;
					cmpStrConst( t_field_s, "altsimplezoom" );
					if( matched )  g.firemodes[t.gunid][1].settings.simplezoom = t.value1;
					cmpStrConst( t_field_s, "altsimplezoomx" );
					if( matched )  g.firemodes[t.gunid][1].settings.simplezoomx_f = t.value1;
					cmpStrConst( t_field_s, "altsimplezoomy" );
					if( matched )  g.firemodes[t.gunid][1].settings.simplezoomy_f = t.value1;
					cmpStrConst( t_field_s, "altsimplezoommod" );
					if( matched )  g.firemodes[t.gunid][1].settings.simplezoommod = t.value1;
					cmpStrConst( t_field_s, "altsimplezoomacc" );
					if( matched )  g.firemodes[t.gunid][1].settings.simplezoomacc = t.value1;
					cmpStrConst( t_field_s, "altsimplezoomspeed" );
					if( matched )  g.firemodes[t.gunid][1].settings.simplezoomspeed = t.value1;
					cmpStrConst( t_field_s, "altsimplezoomflash" );
					if( matched )  g.firemodes[t.gunid][1].settings.simplezoomflash = t.value1;
					cmpStrConst( t_field_s, "altsimplezoomanim" );
					if( matched )  g.firemodes[t.gunid][1].settings.simplezoomanim = t.value1;
					cmpStrConst( t_field_s, "altgunlagspeed" );
					if( matched )  g.firemodes[t.gunid][1].settings.gunlagSpeed = t.value1;
					cmpStrConst( t_field_s, "altgunlagxmax" );
					if( matched )  g.firemodes[t.gunid][1].settings.gunlagXmax = t.value1;
					cmpStrConst( t_field_s, "altgunlagymax" );
					if( matched )  g.firemodes[t.gunid][1].settings.gunlagYmax = t.value1;
					cmpStrConst( t_field_s, "altzoomgunlagspeed" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoomgunlagSpeed = t.value1;
					cmpStrConst( t_field_s, "altzoomgunlagxmax" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoomgunlagXmax = t.value1;
					cmpStrConst( t_field_s, "altzoomgunlagymax" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoomgunlagYmax = t.value1;
					cmpStrConst( t_field_s, "altzoomwalkspeed" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoomwalkspeed = t.value1;
					cmpStrConst( t_field_s, "altzoomturnspeed" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoomturnspeed = t.value1;
					cmpStrConst( t_field_s, "altplrmovespeedmod" );
					if( matched )  g.firemodes[t.gunid][1].settings.plrmovespeedmod = t.value1;
					cmpStrConst( t_field_s, "altplremptyspeedmod" );
					if( matched )  g.firemodes[t.gunid][1].settings.plremptyspeedmod = t.value1;
					cmpStrConst( t_field_s, "altplrturnspeedmod" );
					if( matched )  g.firemodes[t.gunid][1].settings.plrturnspeedmod = t.value1;
					cmpStrConst( t_field_s, "altplrjumpspeedmod" );
					if( matched )  g.firemodes[t.gunid][1].settings.plrjumpspeedmod = t.value1;
					cmpStrConst( t_field_s, "altplrreloadspeedmod" );
					if( matched )  g.firemodes[t.gunid][1].settings.plrreloadspeedmod = t.value1;

					cmpStrConst( t_field_s, "altsoundstrength" );
					if( matched )  g.firemodes[t.gunid][1].settings.soundstrength = t.value1;
					cmpStrConst( t_field_s, "altrecoily" );
					if( matched )  g.firemodes[t.gunid][1].settings.recoily_f = t.value1;
					cmpStrConst( t_field_s, "altrecoilx" );
					if( matched )  g.firemodes[t.gunid][1].settings.recoilx_f = t.value1;
					cmpStrConst( t_field_s, "altrecoilyreturn" );
					if( matched )  g.firemodes[t.gunid][1].settings.recoilycorrect_f = t.value1;
					cmpStrConst( t_field_s, "altrecoilxreturn" );
					if( matched )  g.firemodes[t.gunid][1].settings.recoilxcorrect_f = t.value1;
					cmpStrConst( t_field_s, "altzoomrecoily" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoomrecoily_f = t.value1;
					cmpStrConst( t_field_s, "altzoomrecoilx" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoomrecoilx_f = t.value1;
					cmpStrConst( t_field_s, "altzoomrecoilyreturn" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoomrecoilycorrect_f = t.value1;
					cmpStrConst( t_field_s, "altzoomrecoilxreturn" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoomrecoilxcorrect_f = t.value1;
					cmpStrConst( t_field_s, "altforcezoomout" );
					if( matched )  g.firemodes[t.gunid][1].settings.forcezoomout = t.value1;
					cmpStrConst( t_field_s, "altammomax" );
					if( matched )  g.firemodes[t.gunid][1].settings.ammomax = t.value1;
					cmpStrConst( t_field_s, "altequipment" );
					if( matched )  g.firemodes[t.gunid][1].settings.equipment = t.value1;
					cmpStrConst( t_field_s, "altlockcamera" );
					if( matched )  g.firemodes[t.gunid][1].settings.lockcamera = t.value1;

					//  Gun model animation keyframe setting
					cmpStrConst( t_field_s, "altkeyframe ratio" );
					if( matched ) 
					{
						t.altkeyframeratio = t.value1;
					}

					//  Classic animations
					cmpStrConst( t_field_s, "altselect" );
					if( matched ) { g.firemodes[t.gunid][1].action.show.s = t.value1  ; g.firemodes[t.gunid][1].action.show.e = t.value2; }
					cmpStrConst( t_field_s, "altidle" );
					if( matched ) { g.firemodes[t.gunid][1].action.idle.s = t.value1  ; g.firemodes[t.gunid][1].action.idle.e = t.value2; }
					cmpStrConst( t_field_s, "altrunto" );
					if( matched ) { g.firemodes[t.gunid][1].action.runto.s = t.value1  ; g.firemodes[t.gunid][1].action.runto.e = t.value2; }
					cmpStrConst( t_field_s, "altrunfrom" );
					if( matched ) { g.firemodes[t.gunid][1].action.runfrom.s = t.value1  ; g.firemodes[t.gunid][1].action.runfrom.e = t.value2; }
					cmpStrConst( t_field_s, "altmove" );
					if( matched ) { g.firemodes[t.gunid][1].action.move.s = t.value1  ; g.firemodes[t.gunid][1].action.move.e = t.value2; }
					cmpStrConst( t_field_s, "altrun" );
					if( matched ) { g.firemodes[t.gunid][1].action.run.s = t.value1  ; g.firemodes[t.gunid][1].action.run.e = t.value2; }
					cmpStrConst( t_field_s, "altfire" );
					if( matched ) { g.firemodes[t.gunid][1].action.start.s = t.value1  ; g.firemodes[t.gunid][1].action.start.e = t.value1 ; g.firemodes[t.gunid][1].action.finish.s = t.value1 ; g.firemodes[t.gunid][1].action.finish.e = t.value2; }
					cmpStrConst( t_field_s, "altuse" );
					if( matched ) { g.firemodes[t.gunid][1].action.start.s = t.value1  ; g.firemodes[t.gunid][1].action.start.e = t.value1 ; g.firemodes[t.gunid][1].action.finish.s = t.value1 ; g.firemodes[t.gunid][1].action.finish.e = t.value2; }
					cmpStrConst( t_field_s, "altstart fire" );
					if( matched ) { g.firemodes[t.gunid][1].action.start.s = t.value1  ; g.firemodes[t.gunid][1].action.start.e = t.value2; }
					cmpStrConst( t_field_s, "altautomatic fire" );
					if( matched ) { g.firemodes[t.gunid][1].action.automatic.s = t.value1  ; g.firemodes[t.gunid][1].action.automatic.e = t.value2; }
					cmpStrConst( t_field_s, "altend fire" );
					if( matched ) { g.firemodes[t.gunid][1].action.finish.s = t.value1  ; g.firemodes[t.gunid][1].action.finish.e = t.value2; }
					cmpStrConst( t_field_s, "altlast start fire" );
					if( matched ) { g.firemodes[t.gunid][1].action.laststart.s = t.value1  ; g.firemodes[t.gunid][1].action.laststart.e = t.value2; }
					cmpStrConst( t_field_s, "altlast end fire" );
					if( matched ) { g.firemodes[t.gunid][1].action.lastfinish.s = t.value1  ; g.firemodes[t.gunid][1].action.lastfinish.e = t.value2; }
					cmpStrConst( t_field_s, "altreload" );
					if( matched ) 
					{
						g.firemodes[t.gunid][1].action.startreload.s=t.value1 ; g.firemodes[t.gunid][1].action.startreload.e=t.value2;
						g.firemodes[t.gunid][1].action.reloadloop.s=t.value2 ; g.firemodes[t.gunid][1].action.reloadloop.e=t.value2;
						g.firemodes[t.gunid][1].action.endreload.s=t.value2 ; g.firemodes[t.gunid][1].action.endreload.e=t.value2;
					}
					cmpStrConst( t_field_s, "altstart reload" );
					if( matched ) { g.firemodes[t.gunid][1].action.startreload.s = t.value1  ; g.firemodes[t.gunid][1].action.startreload.e = t.value2; }
					cmpStrConst( t_field_s, "altreload loop" );
					if( matched ) { g.firemodes[t.gunid][1].action.reloadloop.s = t.value1  ; g.firemodes[t.gunid][1].action.reloadloop.e = t.value2; }
					cmpStrConst( t_field_s, "altend reload" );
					if( matched ) { g.firemodes[t.gunid][1].action.endreload.s = t.value1  ; g.firemodes[t.gunid][1].action.endreload.e = t.value2; }
					cmpStrConst( t_field_s, "altcock" );
					if( matched ) { g.firemodes[t.gunid][1].action.cock.s = t.value1  ; g.firemodes[t.gunid][1].action.cock.e = t.value2; }
					cmpStrConst( t_field_s, "altputaway" );
					if( matched ) { g.firemodes[t.gunid][1].action.hide.s = t.value1  ; g.firemodes[t.gunid][1].action.hide.e = t.value2; }
					cmpStrConst( t_field_s, "altstart fire 2" );
					if( matched ) { g.firemodes[t.gunid][1].action.start2.s = t.value1  ; g.firemodes[t.gunid][1].action.start2.e = t.value2; }
					cmpStrConst( t_field_s, "altend fire 2" );
					if( matched ) { g.firemodes[t.gunid][1].action.finish2.s = t.value1  ; g.firemodes[t.gunid][1].action.finish2.e = t.value2; }
					cmpStrConst( t_field_s, "altstart fire 3" );
					if( matched ) { g.firemodes[t.gunid][1].action.start3.s = t.value1  ; g.firemodes[t.gunid][1].action.start3.e = t.value2; }
					cmpStrConst( t_field_s, "altend fire 3" );
					if( matched ) { g.firemodes[t.gunid][1].action.finish3.s = t.value1  ; g.firemodes[t.gunid][1].action.finish3.e = t.value2; }

					//  Advanced animations
					cmpStrConst( t_field_s, "altfix jam" );
					if( matched ) { g.firemodes[t.gunid][1].action2.clearjam.s = t.value1  ; g.firemodes[t.gunid][1].action2.clearjam.e = t.value2; }
					cmpStrConst( t_field_s, "althand push" );
					if( matched ) { g.firemodes[t.gunid][1].action2.handpush.s = t.value1  ; g.firemodes[t.gunid][1].action2.handpush.e = t.value2; }
					cmpStrConst( t_field_s, "althand dead" );
					if( matched ) { g.firemodes[t.gunid][1].action2.handdead.s = t.value1  ; g.firemodes[t.gunid][1].action2.handdead.e = t.value2; }
					cmpStrConst( t_field_s, "altjammed" );
					if( matched ) { g.firemodes[t.gunid][1].action2.jammed.s = t.value1  ; g.firemodes[t.gunid][1].action2.jammed.e = t.value2; }
					cmpStrConst( t_field_s, "altchange firemode" );
					if( matched ) { g.firemodes[t.gunid][1].action2.swaptoalt.s = t.value1  ; g.firemodes[t.gunid][1].action2.swaptoalt.e = t.value2; }
					cmpStrConst( t_field_s, "althand button" );
					if( matched ) { g.firemodes[t.gunid][1].action2.handbutton.s = t.value1  ; g.firemodes[t.gunid][1].action2.handbutton.e = t.value2; }
					cmpStrConst( t_field_s, "althand take" );
					if( matched ) { g.firemodes[t.gunid][1].action2.handtake.s = t.value1  ; g.firemodes[t.gunid][1].action2.handtake.e = t.value2; }
					cmpStrConst( t_field_s, "altpull up" );
					if( matched ) { g.firemodes[t.gunid][1].action2.pullup.s = t.value1  ; g.firemodes[t.gunid][1].action2.pullup.e = t.value2; }
					cmpStrConst( t_field_s, "altpull down" );
					if( matched ) { g.firemodes[t.gunid][1].action2.pulldown.s = t.value1  ; g.firemodes[t.gunid][1].action2.pulldown.e = t.value2; }
					cmpStrConst( t_field_s, "altpull left" );
					if( matched ) { g.firemodes[t.gunid][1].action2.pullleft.s = t.value1  ; g.firemodes[t.gunid][1].action2.pullleft.e = t.value2; }
					cmpStrConst( t_field_s, "altpull right" );
					if( matched ) { g.firemodes[t.gunid][1].action2.pullright.s = t.value1  ; g.firemodes[t.gunid][1].action2.pullright.e = t.value2; }

					//  Empty animations
					cmpStrConst( t_field_s, "altuseempty" );
					if( matched )  g.firemodes[t.gunid][1].settings.hasempty = t.value1;
					cmpStrConst( t_field_s, "altempty shotgun" );
					if( matched )  g.firemodes[t.gunid][1].settings.emptyshotgun = t.value1;
					cmpStrConst( t_field_s, "altempty putaway" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.hide.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.hide.e = t.value2; }
					cmpStrConst( t_field_s, "altempty select" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.show.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.show.e = t.value2; }
					cmpStrConst( t_field_s, "altempty idle" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.idle.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.idle.e = t.value2; }
					cmpStrConst( t_field_s, "altempty runto" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.runto.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.runto.e = t.value2; }
					cmpStrConst( t_field_s, "altempty runfrom" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.runfrom.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.runfrom.e = t.value2; }
					cmpStrConst( t_field_s, "altempty move" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.move.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.move.e = t.value2; }
					cmpStrConst( t_field_s, "altempty run" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.run.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.run.e = t.value2; }
					cmpStrConst( t_field_s, "altempty start reload" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.startreload.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.startreload.e = t.value2; }
					cmpStrConst( t_field_s, "altempty reload loop" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.reloadloop.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.reloadloop.e = t.value2; }
					cmpStrConst( t_field_s, "altempty end reload" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.endreload.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.endreload.e = t.value2; }
					cmpStrConst( t_field_s, "altempty cock" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.cock.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.cock.e = t.value2; }
					cmpStrConst( t_field_s, "altempty reload" );
					if( matched ) 
					{
						g.firemodes[t.gunid][1].emptyaction.startreload.s=t.value1 ; g.firemodes[t.gunid][1].emptyaction.startreload.e=t.value2;
						g.firemodes[t.gunid][1].emptyaction.reloadloop.s=t.value2 ; g.firemodes[t.gunid][1].emptyaction.reloadloop.e=t.value2;
						g.firemodes[t.gunid][1].emptyaction.endreload.s=t.value2 ; g.firemodes[t.gunid][1].emptyaction.endreload.e=t.value2;
					}
					cmpStrConst( t_field_s, "altdryfire" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.dryfire.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.dryfire.e = t.value2; }

					//  Melee Animations
					cmpStrConst( t_field_s, "altmelee start" );
					if( matched ) { g.firemodes[t.gunid][1].meleeaction.start.s = t.value1  ; g.firemodes[t.gunid][1].meleeaction.start.e = t.value2; }
					cmpStrConst( t_field_s, "altmelee start 2" );
					if( matched ) { g.firemodes[t.gunid][1].meleeaction.start2.s = t.value1  ; g.firemodes[t.gunid][1].meleeaction.start2.e = t.value2; }
					cmpStrConst( t_field_s, "altmelee start 3" );
					if( matched ) { g.firemodes[t.gunid][1].meleeaction.start3.s = t.value1  ; g.firemodes[t.gunid][1].meleeaction.start3.e = t.value2; }
					cmpStrConst( t_field_s, "altmelee end" );
					if( matched ) { g.firemodes[t.gunid][1].meleeaction.finish.s = t.value1  ; g.firemodes[t.gunid][1].meleeaction.finish.e = t.value2; }
					cmpStrConst( t_field_s, "altmelee end 2" );
					if( matched ) { g.firemodes[t.gunid][1].meleeaction.finish2.s = t.value1  ; g.firemodes[t.gunid][1].meleeaction.finish2.e = t.value2; }
					cmpStrConst( t_field_s, "altmelee end 3" );
					if( matched ) { g.firemodes[t.gunid][1].meleeaction.finish3.s = t.value1  ; g.firemodes[t.gunid][1].meleeaction.finish3.e = t.value2; }
					cmpStrConst( t_field_s, "altempty melee start" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.start.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.start.e = t.value2; }
					cmpStrConst( t_field_s, "altempty melee start 2" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.start2.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.start2.e = t.value2; }
					cmpStrConst( t_field_s, "altempty melee start 3" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.start3.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.start3.e = t.value2; }
					cmpStrConst( t_field_s, "altempty melee end" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.finish.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.finish.e = t.value2; }
					cmpStrConst( t_field_s, "altempty melee end 2" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.finish2.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.finish2.e = t.value2; }
					cmpStrConst( t_field_s, "altempty melee end 3" );
					if( matched ) { g.firemodes[t.gunid][1].emptyaction.finish3.s = t.value1  ; g.firemodes[t.gunid][1].emptyaction.finish3.e = t.value2; }
						
					//  Zoom Animations
					cmpStrConst( t_field_s, "altempty zoomto" );
					if( matched ) { g.firemodes[t.gunid][1].emptyzoomactionshow.s = t.value1  ; g.firemodes[t.gunid][1].emptyzoomactionshow.e = t.value2; }
					cmpStrConst( t_field_s, "altempty zoom idle" );
					if( matched ) { g.firemodes[t.gunid][1].emptyzoomactionidle.s = t.value1 ; g.firemodes[t.gunid][1].emptyzoomactionidle.e = t.value2; }
					cmpStrConst( t_field_s, "altempty zoom move" );
					if( matched ) { g.firemodes[t.gunid][1].emptyzoomactionmove.s = t.value1  ; g.firemodes[t.gunid][1].emptyzoomactionmove.e = t.value2; }
					cmpStrConst( t_field_s, "altempty zoomfrom" );
					if( matched ) { g.firemodes[t.gunid][1].emptyzoomactionhide.s = t.value1  ; g.firemodes[t.gunid][1].emptyzoomactionhide.e = t.value2; }
					cmpStrConst( t_field_s, "altzoom dryfire" );
					if( matched ) { g.firemodes[t.gunid][1].emptyzoomactiondryfire.s = t.value1  ; g.firemodes[t.gunid][1].emptyzoomactiondryfire.e = t.value2; }
					cmpStrConst( t_field_s, "altzoomto" );
					if( matched ) { g.firemodes[t.gunid][1].zoomaction.show.s = t.value1  ; g.firemodes[t.gunid][1].zoomaction.show.e = t.value2; }
					cmpStrConst( t_field_s, "altzoomfrom" );
					if( matched ) { g.firemodes[t.gunid][1].zoomaction.hide.s = t.value1  ; g.firemodes[t.gunid][1].zoomaction.hide.e = t.value2; }
					cmpStrConst( t_field_s, "altzoom start fire" );
					if( matched ) { g.firemodes[t.gunid][1].zoomaction.start.s = t.value1  ; g.firemodes[t.gunid][1].zoomaction.start.e = t.value2; }
					cmpStrConst( t_field_s, "altzoom start fire 2" );
					if( matched ) { g.firemodes[t.gunid][1].zoomaction.start2.s = t.value1  ; g.firemodes[t.gunid][1].zoomaction.start2.e = t.value2; }
					cmpStrConst( t_field_s, "altzoom start fire 3" );
					if( matched ) { g.firemodes[t.gunid][1].zoomaction.start3.s = t.value1  ; g.firemodes[t.gunid][1].zoomaction.start3.e = t.value2; }
					cmpStrConst( t_field_s, "altzoom automatic fire" );
					if( matched ) { g.firemodes[t.gunid][1].zoomaction.automatic.s = t.value1  ; g.firemodes[t.gunid][1].zoomaction.automatic.e = t.value2; }
					cmpStrConst( t_field_s, "altzoom end fire" );
					if( matched ) { g.firemodes[t.gunid][1].zoomaction.finish.s = t.value1  ; g.firemodes[t.gunid][1].zoomaction.finish.e = t.value2; }
					cmpStrConst( t_field_s, "altzoom end fire 2" );
					if( matched ) { g.firemodes[t.gunid][1].zoomaction.finish2.s = t.value1  ; g.firemodes[t.gunid][1].zoomaction.finish2.e = t.value2; }
					cmpStrConst( t_field_s, "altzoom end fire 3" );
					if( matched ) { g.firemodes[t.gunid][1].zoomaction.finish3.s = t.value1  ; g.firemodes[t.gunid][1].zoomaction.finish3.e = t.value2; }
					cmpStrConst( t_field_s, "altzoom last start fire" );
					if( matched ) { g.firemodes[t.gunid][1].zoomaction.laststart.s = t.value1  ; g.firemodes[t.gunid][1].zoomaction.laststart.e = t.value2; }
					cmpStrConst( t_field_s, "altzoom last end fire" );
					if( matched ) { g.firemodes[t.gunid][1].zoomaction.lastfinish.s = t.value1  ; g.firemodes[t.gunid][1].zoomaction.lastfinish.e = t.value2; }
					cmpStrConst( t_field_s, "altzoom idle" );
					if( matched ) { g.firemodes[t.gunid][1].zoomaction.idle.s = t.value1  ; g.firemodes[t.gunid][1].zoomaction.idle.e = t.value2; }
					cmpStrConst( t_field_s, "altzoom move" );
					if( matched ) { g.firemodes[t.gunid][1].zoomaction.move.s = t.value1  ; g.firemodes[t.gunid][1].zoomaction.move.e = t.value2; }

					//  Gun repeat-fire sound
					cmpStrConst( t_field_s, "altfireloop" );
					if( matched )  g.firemodes[t.gunid][1].sound.fireloopend = t.value1;

					// 280618 - Special active/idle sound loop triggers
					cmpStrConst( t_field_s, "altloopsound" );
					if( matched )  g.firemodes[t.gunid][1].sound.loopsound = t.value1;
					cmpStrConst( t_field_s, "altempty loopsound" );
					if( matched )  g.firemodes[t.gunid][1].sound.emptyloopsound = t.value1;

					//  Gun and muzzle alignment
					cmpStrConst( t_field_s, "althoriz" );
					if( matched )  g.firemodes[t.gunid][1].horiz_f = t.value1;
					cmpStrConst( t_field_s, "altvert" );
					if( matched )  g.firemodes[t.gunid][1].vert_f = t.value1;
					cmpStrConst( t_field_s, "altforward" );
					if( matched )  g.firemodes[t.gunid][1].forward_f = t.value1;
					cmpStrConst( t_field_s, "altalignx" );
					if( matched )  g.firemodes[t.gunid][1].settings.muzzlex_f = t.value1;
					cmpStrConst( t_field_s, "altaligny" );
					if( matched )  g.firemodes[t.gunid][1].settings.muzzley_f = t.value1;
					cmpStrConst( t_field_s, "altalignz" );
					if( matched )  g.firemodes[t.gunid][1].settings.muzzlez_f = t.value1;
					cmpStrConst( t_field_s, "altzoomalignx" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoommuzzlex_f = t.value1;
					cmpStrConst( t_field_s, "altzoomaligny" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoommuzzley_f = t.value1;
					cmpStrConst( t_field_s, "altzoomalignz" );
					if( matched )  g.firemodes[t.gunid][1].settings.zoommuzzlez_f = t.value1;
					cmpStrConst( t_field_s, "altrotx" );
					if( matched )  g.firemodes[t.gunid][1].settings.rotx_f = t.value1;
					cmpStrConst( t_field_s, "altroty" );
					if( matched )  g.firemodes[t.gunid][1].settings.roty_f = t.value1;
					cmpStrConst( t_field_s, "altrotz" );
					if( matched )  g.firemodes[t.gunid][1].settings.rotz_f = t.value1;

					//  Gun particle effect
					cmpStrConst( t_field_s, "altparticledecal" );
					if( matched )  g.firemodes[t.gunid][1].particle.decal_s = t.value_s;
					cmpStrConst( t_field_s, "altparticlespeed" );
					if( matched )  g.firemodes[t.gunid][1].particle.velocity = t.value1;

					//  Advanced ammo pooling system
					cmpStrConst( t_field_s, "altpoolammo" );
					if( matched ) 
					{
						if (  t.value_s != "" ) 
						{
							t.poolindex=0 ; t.emptyindex=0;
							for ( t.y = 1 ; t.y<=  100; t.y++ )
							{
								if (  t.ammopool[t.y].name_s == t.value_s ) { t.poolindex = t.y  ; break; }
								if (  t.ammopool[t.y].used == 0 && t.emptyindex == 0  )  t.emptyindex = t.y;
							}
							if (  t.poolindex == 0  )  t.poolindex = t.emptyindex;
							if (  t.poolindex>0 ) 
							{
								g.firemodes[t.gunid][1].settings.poolindex=t.poolindex;
								t.ammopool[t.poolindex].used=1 ; t.ammopool[t.poolindex].name_s=t.value_s;
							}
						}
					}

					//  Alternate Fire settings
					cmpStrConst( t_field_s, "alternateisflak" );
					if( matched )  t.gun[t.gunid].settings.alternateisflak = t.value1;
					cmpStrConst( t_field_s, "alternateisray" );
					if( matched )  t.gun[t.gunid].settings.alternateisray = t.value1;
					cmpStrConst( t_field_s, "altto" );
					if( matched ) { t.gun[t.gunid].altaction.to.s = t.value1  ; t.gun[t.gunid].altaction.to.e = t.value2; }
					cmpStrConst( t_field_s, "altfrom" );
					if( matched ) { t.gun[t.gunid].altaction.from.s = t.value1  ; t.gun[t.gunid].altaction.from.e = t.value2; }
					cmpStrConst( t_field_s, "empty altto" );
					if( matched ) { t.gun[t.gunid].emptyaltactionto.s = t.value1  ; t.gun[t.gunid].emptyaltactionto.e = t.value2; }
					cmpStrConst( t_field_s, "empty alt to" );
					if( matched ) { t.gun[t.gunid].emptyaltactionto.s = t.value1  ; t.gun[t.gunid].emptyaltactionto.e = t.value2; }
					cmpStrConst( t_field_s, "empty altfrom" );
					if( matched ) { t.gun[t.gunid].emptyaltactionfrom.s = t.value1  ; t.gun[t.gunid].emptyaltactionfrom.e = t.value2; }
					cmpStrConst( t_field_s, "empty alt from" );
					if( matched ) { t.gun[t.gunid].emptyaltactionfrom.s = t.value1  ; t.gun[t.gunid].emptyaltactionfrom.e = t.value2; }

					cmpStrConst( t_field_s, "addtospare" );
					if( matched )  t.gun[t.gunid].settings.addtospare = t.value1;
					cmpStrConst( t_field_s, "shareloadedammo" );
					if( matched )  t.gun[t.gunid].settings.modessharemags = t.value1;

					//  Bullet control
					cmpStrConst( t_field_s, "bullethidemod" );
					if( matched )  t.gun[t.gunid].settings.bulletmod = t.value1;
					cmpStrConst( t_field_s, "bullethidereset" );
					if( matched )  t.gun[t.gunid].settings.bulletreset = t.value1;
					cmpStrConst( t_field_s, "bulletlimbtotal" );
					if( matched )  t.gun[t.gunid].settings.bulletlimbsmax = t.value1;
					cmpStrConst( t_field_s, "bullethidestart" );
					if( matched ) 
					{
						t.gun[t.gunid].settings.bulletamount=t.value1;
						t.gun[t.gunid].settings.currentbullet=t.value1+1;
					}

					//  Gun sound bank
					cmpNStrConst( t_field_s, "sound" );
					if ( matched )
					{
						for ( t.p = 1 ; t.p <= 14; t.p++ )
						{
							t.tryfield_s = "" ; 
							t.tryfield_s = t.tryfield_s + "sound" + Str(t.p);
							if (  t.field_s == t.tryfield_s  )  t.gunsound[t.gunid][t.p].name_s = t.value_s;
						}
					}

					cmpNStrConst( t_field_s, "altsound" );
					if ( matched )
					{
						for ( t.p = 1 ; t.p <= 4; t.p++ )
						{
							t.tryfield_s = ""; 
							t.tryfield_s = t.tryfield_s + "altsound" + Str(t.p) ; 
							t.tryfield2_s = "" ; 
							t.tryfield2_s = t.tryfield2_s + "alt sound" + Str(t.p);
							if ( t.p != 2 ) 
							{
								if (  t.field_s == t.tryfield_s || t.field_s == t.tryfield2_s  )  t.gunsound[t.gunid][t.p].altname_s = t.value_s;
							}
						}
					}

					//  Gun sound trigger frames
					cmpStrConst( t_field_s, "soundframes" );
					if( matched )  t.gun[t.gunid].sound.soundframes = t.value1;

					if (  t.gun[t.gunid].sound.soundframes > 0 ) 
					{
						cmpNStrConst(t_field_s, "sframeanim");
						if (matched)
						{
							for (t.p = 0; t.p <= t.gun[t.gunid].sound.soundframes; t.p++)
							{
								if (t.p < 100) // ensure cannot access sounditem items out of bounds!
								{
									lastsoundanimname_s = t.value_s;
								}
							}
						}
						cmpNStrConst( t_field_s, "sframe" );
						if ( matched )
						{
							for ( t.p = 0 ; t.p<=  t.gun[t.gunid].sound.soundframes; t.p++ )
							{
								if (t.p < 100) // ensure cannot access sounditem items out of bounds!
								{
									t.tryfield_s = ""; 
									t.tryfield_s = t.tryfield_s + "sframe" + Str(t.p);
									if (t.field_s == t.tryfield_s)
									{
										t.gunsounditem[t.gunid][t.p].keyframe = (t.value1*t.keyframeratio);
										// if soundanim name specified, add that animations start frame
										if (strlen(lastsoundanimname_s.Get()) > 0 && iCurrentGunObj != 0 )
										{
											sObject* pObject = GetObjectData(iCurrentGunObj);
											float fFoundStart = 0, fFoundFinish = 0;
											int entity_lua_getanimationnamefromobject (sObject* pObject, cstr FindThisName_s, float* fFoundStart, float* fFoundFinish);
											if (entity_lua_getanimationnamefromobject(pObject, lastsoundanimname_s, &fFoundStart, &fFoundFinish) > 0)
											{
												t.gunsounditem[t.gunid][t.p].keyframe += fFoundStart;
											}
										}
										t.gunsounditem[t.gunid][t.p].playsound = t.value2;
									}
								}
							
							}
						}
					}

					// VR specific details
					cmpStrConst(t_field_s, "vrweaponmode");
					if (matched) t.gun[t.gunid].settings.iVRWeaponMode = t.value1;
					cmpStrConst(t_field_s, "vrweaponlimbofweapon");
					if (matched) t.gun[t.gunid].settings.iVRWeaponLimbOfWeapon = t.value1;
					cmpStrConst(t_field_s, "vrweaponstaticframe");
					if (matched) t.gun[t.gunid].settings.iVRWeaponStaticFrame = t.value1;
					cmpStrConst( t_field_s, "vrweaponoffsetx" );
					if( matched ) t.gun[t.gunid].settings.fVRWeaponOffsetX = t.value1;
					cmpStrConst( t_field_s, "vrweaponoffsety" );
					if( matched ) t.gun[t.gunid].settings.fVRWeaponOffsetY = t.value1;
					cmpStrConst( t_field_s, "vrweaponoffsetz" );
					if( matched ) t.gun[t.gunid].settings.fVRWeaponOffsetZ = t.value1;
					cmpStrConst( t_field_s, "vrweaponscale" );
					if( matched ) t.gun[t.gunid].settings.fVRWeaponScale = t.value1;
					cmpStrConst( t_field_s, "vrweaponanglex" );
					if( matched ) t.gun[t.gunid].settings.fVRWeaponAngleX = t.value1;
					cmpStrConst( t_field_s, "vrweaponangley" );
					if( matched ) t.gun[t.gunid].settings.fVRWeaponAngleY = t.value1;
					cmpStrConst( t_field_s, "vrweaponanglez" );
					if( matched ) t.gun[t.gunid].settings.fVRWeaponAngleZ = t.value1;
					

					// New features.		
					cmpStrConst(t_field_s, "emissivestrength");
					if (matched) t.gun[t.gunid].settings.fEmissiveStrength = t.value1;

					cmpStrConst(t_field_s, "fakereload");
					if (matched) t.gun[t.gunid].settings.fake_reload = t.value1;

					// tracers
					cmpStrConst(t_field_s, "traceractive");
					if (matched) t.gun[t.gunid].settings.tracer_active = t.value1;
					cmpStrConst(t_field_s, "tracerlifetime");
					if (matched) t.gun[t.gunid].settings.tracer_lifetime = t.value1;
					cmpStrConst(t_field_s, "tracerglow");
					if (matched) t.gun[t.gunid].settings.tracer_glow = t.value1;
					cmpStrConst(t_field_s, "tracerscrollv");
					if (matched) t.gun[t.gunid].settings.tracer_scrollV = t.value1;
					cmpStrConst(t_field_s, "tracerscalev");
					if (matched) t.gun[t.gunid].settings.tracer_scaleV = t.value1;
					cmpStrConst(t_field_s, "tracerwidth");
					if (matched) t.gun[t.gunid].settings.tracer_width = t.value1;
					cmpStrConst(t_field_s, "tracermaxlength");
					if (matched) t.gun[t.gunid].settings.tracer_maxlength = t.value1;
					cmpStrConst(t_field_s, "tracercolorr");
					if (matched) t.gun[t.gunid].settings.tracer_colorR = t.value1 / 255.0f;
					cmpStrConst(t_field_s, "tracercolorg");
					if (matched) t.gun[t.gunid].settings.tracer_colorG = t.value1 / 255.0f;
					cmpStrConst(t_field_s, "tracercolorb");
					if (matched) t.gun[t.gunid].settings.tracer_colorB = t.value1 / 255.0f;
				}
			}
		}
		UnDim ( t.data_s );
	}

	// Correct any legacy fall-out
	if ( cstr(Lower(t.gun[t.gunid].texd_s.Get())) == "gun_d2.dds"  )  t.gun[t.gunid].texd_s = "gun_d.dds";

	// 130418 - also replace any old TGA references
	char pTexFileName[1024];
	strcpy ( pTexFileName, t.gun[t.gunid].texd_s.Get() );
	if ( stricmp ( pTexFileName+strlen(pTexFileName)-4, ".tga") == NULL )
	{
		pTexFileName[strlen(pTexFileName)-4] = 0;
		strcat ( pTexFileName, ".png" );
		t.gun[t.gunid].texd_s = pTexFileName;
	}

	// Go through gun settings and populate with defaults
	for ( t.i = 0 ; t.i<=  1; t.i++ )
	{
		//  If no run, replace with regular move action
		if (  g.firemodes[t.gunid][t.i].action.run.e == 0  )  g.firemodes[t.gunid][t.i].action.run = g.firemodes[t.gunid][t.i].action.move;
		if (  g.firemodes[t.gunid][t.i].zoomaction.run.e == 0  )  g.firemodes[t.gunid][t.i].zoomaction.run = g.firemodes[t.gunid][t.i].zoomaction.move;

		//  If no COCK animation, fill with end of reload data
		if (  g.firemodes[t.gunid][t.i].action.cock.e == 0 ) 
		{
			g.firemodes[t.gunid][t.i].action.cock.s=g.firemodes[t.gunid][t.i].action.endreload.e;
			g.firemodes[t.gunid][t.i].action.cock.e=g.firemodes[t.gunid][t.i].action.endreload.e;
		}

		//  Fill empty animations if no animation found
		if (  g.firemodes[t.gunid][t.i].emptyaction.hide.e == 0  )  g.firemodes[t.gunid][t.i].emptyaction.hide = g.firemodes[t.gunid][t.i].action.hide;
		if (  g.firemodes[t.gunid][t.i].emptyaction.show.e == 0  )  g.firemodes[t.gunid][t.i].emptyaction.show = g.firemodes[t.gunid][t.i].action.show;
		if (  g.firemodes[t.gunid][t.i].emptyaction.idle.e == 0  )  g.firemodes[t.gunid][t.i].emptyaction.idle = g.firemodes[t.gunid][t.i].action.idle;
		if (  g.firemodes[t.gunid][t.i].emptyaction.move.e == 0  )  g.firemodes[t.gunid][t.i].emptyaction.move = g.firemodes[t.gunid][t.i].action.move;
		if (  g.firemodes[t.gunid][t.i].emptyaction.cock.e == 0 && g.firemodes[t.gunid][t.i].emptyaction.endreload.e == 0  )  g.firemodes[t.gunid][t.i].emptyaction.cock = g.firemodes[t.gunid][t.i].action.cock;
		if (  g.firemodes[t.gunid][t.i].emptyaction.startreload.e == 0  )  g.firemodes[t.gunid][t.i].emptyaction.startreload = g.firemodes[t.gunid][t.i].action.startreload;
		if (  g.firemodes[t.gunid][t.i].emptyaction.reloadloop.e == 0  )  g.firemodes[t.gunid][t.i].emptyaction.reloadloop = g.firemodes[t.gunid][t.i].action.reloadloop;
		if (  g.firemodes[t.gunid][t.i].emptyaction.endreload.e == 0  )  g.firemodes[t.gunid][t.i].emptyaction.endreload = g.firemodes[t.gunid][t.i].action.endreload;
		if (  g.firemodes[t.gunid][t.i].emptyaction.run.e == 0  )  g.firemodes[t.gunid][t.i].emptyaction.run = g.firemodes[t.gunid][t.i].emptyaction.move;

		//  If no COCK animation, fill with end of reload data
		if (  g.firemodes[t.gunid][t.i].emptyaction.cock.e == 0 ) 
		{
			g.firemodes[t.gunid][t.i].emptyaction.cock.s=g.firemodes[t.gunid][t.i].emptyaction.endreload.e;
			g.firemodes[t.gunid][t.i].emptyaction.cock.e=g.firemodes[t.gunid][t.i].emptyaction.endreload.e;
		}

		//  If no muzzle colour, go with default
		if (  g.firemodes[t.gunid][t.i].settings.muzzlecolorr == 0 && g.firemodes[t.gunid][t.i].settings.muzzlecolorg == 0 && g.firemodes[t.gunid][t.i].settings.muzzlecolorb == 0 ) 
		{
			g.firemodes[t.gunid][t.i].settings.muzzlecolorr=255;
			g.firemodes[t.gunid][t.i].settings.muzzlecolorg=255;
			g.firemodes[t.gunid][t.i].settings.muzzlecolorb=0;
		}

		//  Set default smoke size
		if (  g.firemodes[t.gunid][t.i].settings.smokesize == 0 ) 
		{
			g.firemodes[t.gunid][t.i].settings.smokesize=100;
		}

		//  Set default smoke speed
		if (  g.firemodes[t.gunid][t.i].settings.smokespeed == 0 ) 
		{
			g.firemodes[t.gunid][t.i].settings.smokespeed=25;
		}

		//  Set default rate of fire
		if (  g.firemodes[t.gunid][t.i].settings.firerate == 0.0 ) 
		{
			g.firemodes[t.gunid][t.i].settings.firerate=12.0;
		}

		//  Set default force
		if (  g.firemodes[t.gunid][t.i].settings.force == 0 ) 
		{
			g.firemodes[t.gunid][t.i].settings.force=g.firemodes[t.gunid][t.i].settings.damage;
		}
		if (  g.firemodes[t.gunid][t.i].settings.meleeforce == 0 ) 
		{
			g.firemodes[t.gunid][t.i].settings.meleeforce=g.firemodes[t.gunid][t.i].settings.damage;
		}

		// no smoke key: smoke when a smoke decal is named
		if ( g.firemodes[t.gunid][t.i].settings.smoke == -1 )
		{
			if ( g.firemodes[t.gunid][t.i].settings.smokedecal_s != "" ) g.firemodes[t.gunid][t.i].settings.smoke=1; else g.firemodes[t.gunid][t.i].settings.smoke=0;
		}

		//  Default smoke decal
		if (  g.firemodes[t.gunid][t.i].settings.smokedecal_s == "" ) 
		{
			g.firemodes[t.gunid][t.i].settings.smokedecal_s="smoke1";
		}

		//  Find the decal specified
		g.firemodes[t.gunid][t.i].decalid=0;
		if (  g.firemodes[t.gunid][t.i].decal_s != "" ) 
		{
			t.decal_s=g.firemodes[t.gunid][t.i].decal_s; decal_find ( );
			if (  t.decalid<0 ) 
			{
				t.decalid=0;
			}
			else
			{
				t.decal[t.decalid].active=1;
				g.firemodes[t.gunid][t.i].decalid=t.decalid;
			}
		}

		// global setting to stop any jamming of weapons
		if ( g.globals.disableweaponjams == 1 )
		{
			g.firemodes[t.gunid][t.i].settings.jamchance = 0;
		}
	}

	// determine if legacy or new weapon system 
	t.gun[t.gunid].newweaponsystem = 0;
	cstr pathtoresourcefolder_s = "gamecore\\";
	pathtoresourcefolder_s = pathtoresourcefolder_s + g.fpgchuds_s + "\\" + t.gun_s + "\\resources\\";
	cstr pathtohudorigfile_s = "gamecore\\";
	pathtohudorigfile_s = pathtohudorigfile_s + g.fpgchuds_s + "\\" + t.gun_s + "\\hud-orig.dbo";
	if (PathExist(pathtoresourcefolder_s.Get()) == 1 || FileExist(pathtohudorigfile_s.Get()) == 1)
	{
		t.gun[t.gunid].newweaponsystem = 1;
	}
	else
	{
		//PE: Support customs hand from new project system or write folder weapons.
		char writefolder[MAX_PATH];
		strcpy(writefolder, pathtoresourcefolder_s.Get());
		GG_GetRealPath(writefolder, 0);
		if (PathExist(writefolder) == 1)
		{
			t.gun[t.gunid].newweaponsystem = 1;
		}
	}
}

void gun_createhud ( cstr customArms_s )
{
	// init
	bool bCreateThenAppend = true;

	// read HUD.dat
	Dim (t.data_s, 1000);
	t.filename_s = ""; t.filename_s = t.filename_s + "gamecore\\" + g.fpgchuds_s + "\\" + t.gun_s + "\\resources\\hud.dat";
	if (FileExist(t.filename_s.Get()) == 1)
	{
		LoadArray (t.filename_s.Get(), t.data_s);
		for (t.l = 0; t.l <= 999; t.l++)
		{
			t.line_s = t.data_s[t.l];
			if (Len(t.line_s.Get()) > 0)
			{
				LPSTR pThisFile = t.line_s.Get();
				if (pThisFile[0] != ';')
				{
					// the actual file
					cstr pActualFile_s = cstr("gamecore\\") + g.fpgchuds_s + "\\" + t.gun_s + "\\resources\\" + pThisFile + ".dbo";

					// create or append
					if (bCreateThenAppend == true)
					{
						// first one is loaded
						bool bUseCustomHands = false;
						if (stricmp(pThisFile, "base_arms") == NULL)
						{
							if (customArms_s.Len()>0)
							{
								pActualFile_s = g.fpscrootdir_s + cstr("\\Files\\gamecore\\hands\\") + customArms_s + cstr("\\arms.dbo");
								if (FileExist(pActualFile_s.Get()) == 0)
								{
									// player start marker specified hands not exist on this install, so default to legacy ones
									customArms_s = "Combat Gloves Light";
									pActualFile_s = g.fpscrootdir_s + cstr("\\Files\\gamecore\\hands\\") + customArms_s + cstr("\\arms.dbo");
								}
								if (FileExist(pActualFile_s.Get()) == 1)
								{
									bUseCustomHands = true;
								}
							}
						}

						// actual base arms load
						if (t.currentgunobj > 0)
							LoadObject(pActualFile_s.Get(), t.currentgunobj);
						else
							t.currentgunobj = loadgun(t.gunid, pActualFile_s.Get());

						// for custom hands, use custom arm textures too
						if (bUseCustomHands == true)
						{
							cstr pCustomArms_s = cstr("gamecore\\hands\\") + customArms_s + cstr("\\arms_color.dds");
							if(ImageExist(g.weaponstempimageoffset)==1 ) DeleteImage(g.weaponstempimageoffset);
							LoadImage(pCustomArms_s.Get(), g.weaponstempimageoffset);
							TextureObject(t.currentgunobj, g.weaponstempimageoffset);
						}

						// rest are animations only
						bCreateThenAppend = false;
					}
				}
			}
		}
	}

	// before append anim slots, must ensure slot list is empty before loadind in "hud.dat"
	extern std::vector<sAnimSlotStruct> g_pAnimSlotList;
	g_pAnimSlotList.clear();

	// now use the animsystem appender to add animations
	sObject* pObject = GetObjectData (t.currentgunobj);
	cstr pAbsPathToAnim = cstr("gamecore\\") + g.fpgchuds_s + "\\" + t.gun_s + "\\resources\\";
	extern void animsystem_loadanimtextfile (sObject* pObject, cstr pAbsPathToAnim, char* cFileSelected);
	animsystem_loadanimtextfile (pObject, pAbsPathToAnim, t.filename_s.Get());

	// find frame that holds the weapon
	PerformCheckListForLimbs (t.currentgunobj);
	int iFoundCorrectLimbForWeapon = -1;
	for (int iFrame = 1; iFrame <= ChecklistQuantity() - 1; iFrame++)
	{
		if (LimbExist(t.currentgunobj, iFrame) == 1)
		{
			LPSTR pLimbName = LimbName(t.currentgunobj, iFrame);
			LPSTR pFindName = "Weapon_";
			if (strnicmp (pLimbName, pFindName, strlen(pFindName)) == NULL)
			{
				iFoundCorrectLimbForWeapon = iFrame;
				break;
			}
		}
	}

	// determine if using specialised weapon model for player HUD weapon
	cstr pAbsPathToWeaponModel = cstr("gamecore\\") + g.fpgchuds_s + "\\" + t.gun_s + "\\resources\\weapon.dbo";
	if (FileExist(pAbsPathToWeaponModel.Get()) == 0)
	{
		// new system re-uses VWEAP character weapon for players HUD hands
		pAbsPathToWeaponModel = cstr("gamecore\\") + g.fpgchuds_s + "\\" + t.gun_s + "\\weapon.dbo";

		// and in this case, we use the _r_hand limb to anchor
		for (int iFrame = 1; iFrame <= ChecklistQuantity() - 1; iFrame++)
		{
			if (LimbExist(t.currentgunobj, iFrame) == 1)
			{
				LPSTR pLimbName = LimbName(t.currentgunobj, iFrame);
				LPSTR pFindName = "_r_hand";
				char pLwr[MAX_PATH];
				strcpy(pLwr, pLimbName);
				strlwr(pLwr);
				if (strstr (pLwr, pFindName) != NULL)
				{
					iFoundCorrectLimbForWeapon = iFrame;
					break;
				}
			}
		}
	}

	// now add the static weapon
	if (iFoundCorrectLimbForWeapon > -1)
	{
		if (GetMeshExist(g.meshgeneralwork2) == 1) DeleteMesh(g.meshgeneralwork2);
		int iWeapObj = g.temp2objectoffset;
		if (ObjectExist(iWeapObj) == 1) DeleteObject(iWeapObj);
		LoadObject (pAbsPathToWeaponModel.Get(), iWeapObj);
		if(LimbExist(iWeapObj,1)==1)
			OffsetLimb(iWeapObj, 1, t.gun[t.gunid].weaponposx_f, t.gun[t.gunid].weaponposy_f, t.gun[t.gunid].weaponposz_f);
		else
			OffsetLimb(iWeapObj, 0, t.gun[t.gunid].weaponposx_f, t.gun[t.gunid].weaponposy_f, t.gun[t.gunid].weaponposz_f);
		TurnObjectRight(iWeapObj, t.gun[t.gunid].weaponrotx_f);
		RollObjectRight(iWeapObj, t.gun[t.gunid].weaponroty_f);
		PitchObjectDown(iWeapObj, t.gun[t.gunid].weaponrotz_f);
		MakeMeshFromObject(g.meshgeneralwork2, iWeapObj);
		DeleteObject(iWeapObj);
		int iFrameCount = pObject->iFrameCount;
		AddLimb (t.currentgunobj, iFrameCount, g.meshgeneralwork2);
		weapon_getfreeimage ();
		if (t.tImgID != 0)
		{
			cstr pAbsPathToWeaponTexture = cstr("gamecore\\") + g.fpgchuds_s + "\\" + t.gun_s + "\\weapon_color.dds";
			LoadImage (pAbsPathToWeaponTexture.Get(), t.tImgID);
			TextureLimb(t.currentgunobj, iFrameCount, t.tImgID);
		}
		LinkLimb (t.currentgunobj, iFoundCorrectLimbForWeapon, iFrameCount);
	}
}

void gun_scaninall_dataonly ( void )
{
	for ( t.gunid = 1 ; t.gunid<=g.gunmax; t.gunid++ )
	{
		t.gun_s=t.gun[t.gunid].name_s; 
		gun_loaddata ( );
	}
}

void gun_scaninall_ref ( void )
{
	// Scan entire guns folder
	t.gunid=1;
	cstr pRootDir = GetDir();
	for (int iGunsInTwoFolders = 0; iGunsInTwoFolders < 2; iGunsInTwoFolders++)
	{
		char pathToUse[MAX_PATH];
		if (iGunsInTwoFolders == 0)
		{
			// root folder - already here
			strcpy(pathToUse, "");
		}
		if (iGunsInTwoFolders == 1)
		{
			// writable folder - switch to it
			strcpy(pathToUse, pRootDir.Get());
			GG_GetRealPath(pathToUse, 0);
			SetDir(pathToUse);
		}
		if ( PathExist("gamecore") == 1 )
		{
			SetDir ("gamecore");
			UnDim (t.filelist_s);
			buildfilelist(g.fpgchuds_s.Get(), "");
			SetDir ("..");
			if (ArrayCount(t.filelist_s) > 0)
			{
				for (t.chkfile = 0; t.chkfile <= ArrayCount(t.filelist_s); t.chkfile++)
				{
					t.file_s = t.filelist_s[t.chkfile];
					if (t.file_s != "." && t.file_s != "..")
					{
						if (cstr(Lower(Right(t.file_s.Get(), 11))) == "gunspec.txt")
						{
							t.gun[t.gunid].path_s = pathToUse;
							t.gun[t.gunid].name_s = Left(t.file_s.Get(), Len(t.file_s.Get()) - 12);
							t.strwork = ""; t.strwork = t.strwork + "gun " + Str(t.gunid) + ":" + t.file_s;
							timestampactivity(0, t.strwork.Get());
							++t.gunid;
							if (t.gunid > g.maxgunsinengine)  t.gunid = g.maxgunsinengine;
						}
					}
				}
			}
		}
	}

	// and restore root folder
	SetDir(pRootDir.Get());

	// report total number of guns
	g.gunmax = t.gunid - 1;
	t.strwork = ""; t.strwork = t.strwork + "total guns=" + Str(g.gunmax);
	timestampactivity(0, t.strwork.Get());

	//  Now sort the gun list into alphabetical order (MP needs gunid identical on each PC)
	gun_sortintoorder ( );

	// and as there is no connection back to entity that links to guns (and we need it for collection filling)
	// create a database here of known stock weapons
	for (int gunid = 1; gunid <= g.gunmax; gunid++)
	{
		LPSTR pGunName = t.gun[gunid].name_s.Get();
		if (t.gun[gunid].pathtostockentity_s.Len() == 0)
		{
			cstr MCW = "Max Collection\\Weapons\\";
			if (stricmp (pGunName, "enhanced\\AK") == NULL) t.gun[gunid].pathtostockentity_s = MCW + "Assault Rifle.fpe";
			if (stricmp (pGunName, "enhanced\\MK18") == NULL) t.gun[gunid].pathtostockentity_s = MCW + "Compact Assault Rifle.fpe";
			if (stricmp (pGunName, "enhanced\\M67") == NULL) t.gun[gunid].pathtostockentity_s = MCW + "Frag Grenade.fpe";
			if (stricmp (pGunName, "enhanced\\MK19T") == NULL) t.gun[gunid].pathtostockentity_s = MCW + "Magnum Pistol.fpe";
			if (stricmp (pGunName, "enhanced\\AR") == NULL) t.gun[gunid].pathtostockentity_s = MCW + "Patrol Rifle.fpe";
			if (stricmp (pGunName, "enhanced\\B810") == NULL) t.gun[gunid].pathtostockentity_s = MCW + "Pocket Knife.fpe";
			if (stricmp (pGunName, "enhanced\\SledgeHammer") == NULL) t.gun[gunid].pathtostockentity_s = MCW + "Sledgehammer.fpe";
			if (stricmp (pGunName, "enhanced\\M29S") == NULL) t.gun[gunid].pathtostockentity_s = MCW + "Snubnose Revolver.fpe";
			if (stricmp (pGunName, "enhanced\\R870") == NULL) t.gun[gunid].pathtostockentity_s = MCW + "Tactical Pump Shotgun.fpe";
			if (stricmp (pGunName, "max\\colt") == NULL) t.gun[gunid].pathtostockentity_s = MCW + "Colt Pistol.fpe";
			cstr AGK = "Aztec Game Kit\\Weapons\\";
			if (stricmp (pGunName, "aztec\\AztecAxe") == NULL) t.gun[gunid].pathtostockentity_s = AGK + "Aztec Axe.fpe";
			if (stricmp (pGunName, "aztec\\AztecDagger") == NULL) t.gun[gunid].pathtostockentity_s = AGK + "Aztec Dagger.fpe";
			if (stricmp (pGunName, "aztec\\AztecSpear") == NULL) t.gun[gunid].pathtostockentity_s = AGK + "Aztec Spear.fpe";

		}
	}
}

void gun_sortintoorder ( void )
{
	//  Now sort the gun list into alphabetical order
	for ( t.tgid1 = 1 ; t.tgid1<=  g.gunmax; t.tgid1++ )
	{
		for ( t.tgid2 = 1 ; t.tgid2<=  g.gunmax; t.tgid2++ )
		{
			if (  t.tgid1 != t.tgid2 ) 
			{
				t.tname1_s=Lower(t.gun[t.tgid1].name_s.Get());
				t.tname2_s=Lower(t.gun[t.tgid2].name_s.Get());
				//C++ISSUE
				if (  strlen( t.tname1_s.Get() ) > strlen( t.tname2_s.Get() ) ) 
				{
					//  smallest at top
					t.gun[t.tgid1].name_s=t.tname2_s;
					t.gun[t.tgid2].name_s=t.tname1_s;
				}
			}
		}
	}

	// once have correct order, populate weapon slots for player
	void gun_gatherslotorder_load (void);
	gun_gatherslotorder_load();
}

void gun_findweaponindexbyname_core ( void )
{
	t.foundgunid=0;
	if (  t.findgun_s != "" ) 
	{
		for ( t.tid = 1 ; t.tid<=g.gunmax; t.tid++ )
		{
			if (  cstr(Lower(t.findgun_s.Get())) == cstr(Lower(t.gun[t.tid].name_s.Get())) ) 
			{
				t.foundgunid=t.tid;
				break;
			}
		}
	}
}

void gun_scaninall_findnewlyaddedgun (void)
{
	// if find gun that is not in list, if find, flag is 'found'
	// this is expensive, so only do it IF gunlist flag has been reset (when exe starts, start of every level, etc)
	if (g_bGunListNeedsRefreshing == true)
	{
		// store original gunid
		t.storegunid = t.gunid;

		// Scan entire guns folder (again)
		cstr pRootDir = GetDir();
		for (int iGunsInTwoFolders = 0; iGunsInTwoFolders < 2; iGunsInTwoFolders++)
		{
			char pathToUse[MAX_PATH];
			if (iGunsInTwoFolders == 0)
			{
				// root folder - already here
				strcpy(pathToUse, "");
			}
			if (iGunsInTwoFolders == 1)
			{
				// writable folder - switch to it
				strcpy(pathToUse, pRootDir.Get());
				GG_GetRealPath(pathToUse, 0);
				SetDir(pathToUse);
			}
			if (PathExist("gamecore") == 1)
			{
				SetDir ("gamecore");
				UnDim (t.filelist_s);
				buildfilelist(g.fpgchuds_s.Get(), "");
				SetDir ("..");
				if (ArrayCount(t.filelist_s) > 0)
				{
					for (t.chkfile = 0; t.chkfile <= ArrayCount(t.filelist_s); t.chkfile++)
					{
						t.file_s = t.filelist_s[t.chkfile];
						if (t.file_s != "." && t.file_s != "..")
						{
							if (cstr(Lower(Right(t.file_s.Get(), 11))) == "gunspec.txt")
							{
								t.findgun_s = Left(t.file_s.Get(), Len(t.file_s.Get()) - 12);
								gun_findweaponindexbyname_core ();
								if (t.foundgunid == 0)
								{
									++g.gunmax;
									if (g.gunmax > g.maxgunsinengine)  g.gunmax = g.maxgunsinengine;
									t.gun[g.gunmax].path_s = pathToUse;
									t.gun[g.gunmax].name_s = t.findgun_s;
									t.gunid = g.gunmax;
									t.gun_s = t.findgun_s;
									gun_loaddata ();
									t.strwork = ""; t.strwork = t.strwork + "newly added gun " + Str(t.gunid) + ":" + t.file_s;
									timestampactivity(0, t.strwork.Get());
								}
							}
						}
					}
				}
			}
		}

		// and restore root folder
		SetDir(pRootDir.Get());

		// report total number of guns
		t.strwork = ""; t.strwork = t.strwork + "new total guns=" + Str(g.gunmax);
		timestampactivity(0, t.strwork.Get());

		// only refresh when necessary (performance hit)
		g_bGunListNeedsRefreshing = false;

		// restore original gunid
		t.gunid = t.storegunid;
	}
}

void gun_scaninall_findnewlyaddedgun_old ( void )
{
	//  if find gun that is not in list, if find, flag is 'found'
	t.storegunid=t.gunid;

	//  gather files
	SetDir (  "gamecore" );
	UnDim ( t.filelist_s );
	buildfilelist(g.fpgchuds_s.Get(),"");
	SetDir (  ".." );

	//  go through file list of latest guns
	if (  ArrayCount(t.filelist_s)>0 ) 
	{
		for ( t.chkfile = 0 ; t.chkfile<=  ArrayCount(t.filelist_s); t.chkfile++ )
		{
			t.file_s=t.filelist_s[t.chkfile];
			if (  t.file_s != "." && t.file_s != ".." ) 
			{
				t.findgun_s="";
				if ( cstr( Lower(Right(t.file_s.Get(),11))) == "gunspec.txt" ) 
				{
					t.findgun_s=Left(t.file_s.Get(),Len(t.file_s.Get())-12);
				}
				if (  t.findgun_s != "" ) 
				{
					gun_findweaponindexbyname_core ( );
					if (  t.foundgunid == 0 ) 
					{
						++g.gunmax;
						if (  g.gunmax>g.maxgunsinengine  )  g.gunmax = g.maxgunsinengine;
						t.gun[g.gunmax].name_s=t.findgun_s;
						t.gunid=g.gunmax;
						t.gun_s=t.findgun_s; 
						gun_loaddata ( );
					}
				}
			}
		}
	}
	t.gunid=t.storegunid;
}

void gun_findweaponindexbyname ( void )
{
	//  try to find gun
	t.storefindgun_s=t.findgun_s; 
	gun_findweaponindexbyname_core ( );

	//  if not found, try rescanning (as new guns can be added by store), and try a second time
	if (  t.foundgunid == 0 ) 
	{
		gun_scaninall_findnewlyaddedgun ( );
		t.findgun_s=t.storefindgun_s; 
		gun_findweaponindexbyname_core ( );
	}
}

cstr gun_names_tonormal(cstr thisLabel)
{
	// clean up if 2 slashes, can only have one to propery identify weapon name
	LPSTR pOldStr = thisLabel.Get();
	char pNewStr[MAX_PATH];
	strcpy(pNewStr, pOldStr);
	LPSTR pFourSlashes = strstr(pNewStr, "\\\\");
	if(pFourSlashes)
	{
		strcpy(pFourSlashes, pFourSlashes + 1);
		thisLabel = pNewStr;
	}

	if (stricmp (thisLabel.Get(), "enhanced\\Gloves_Unarmed") == NULL) thisLabel = "Melee Combat";
	if (stricmp (thisLabel.Get(), "enhanced\\AK") == NULL) thisLabel = "Assault Rifle";
	if (stricmp (thisLabel.Get(), "enhanced\\AR") == NULL) thisLabel = "Patrol Rifle";
	if (stricmp (thisLabel.Get(), "enhanced\\B810") == NULL) thisLabel = "Pocket Knife";
	if (stricmp (thisLabel.Get(), "enhanced\\M29S") == NULL) thisLabel = "Snubnose Revolver";
	if (stricmp (thisLabel.Get(), "enhanced\\Mk18") == NULL) thisLabel = "Compact Assault Rifle";
	if (stricmp (thisLabel.Get(), "enhanced\\Mk19T") == NULL) thisLabel = "Magnum Pistol";
	if (stricmp (thisLabel.Get(), "enhanced\\R870") == NULL) thisLabel = "Tactical Pump Shotgun";
	if (stricmp (thisLabel.Get(), "enhanced\\SledgeHammer") == NULL) thisLabel = "SledgeHammer";
	if (stricmp (thisLabel.Get(), "aztec\\AztecAxe") == NULL) thisLabel = "Aztec Axe";
	if (stricmp (thisLabel.Get(), "aztec\\AztecDagger") == NULL) thisLabel = "Aztec Dagger";
	if (stricmp (thisLabel.Get(), "aztec\\AztecSpear") == NULL) thisLabel = "Aztec Spear";
	return thisLabel;
}

cstr gun_names_tointernal(cstr thisLabel)
{
	if (stricmp (thisLabel.Get(), "No Weapon") == NULL) thisLabel = "";
	if (stricmp (thisLabel.Get(), "No Preference") == NULL) thisLabel = "";
	if (stricmp (thisLabel.Get(), "Melee Combat") == NULL) thisLabel = "enhanced\\Gloves_Unarmed";
	if (stricmp (thisLabel.Get(), "Grenades Only") == NULL) thisLabel = "enhanced\\M67";
	if (stricmp (thisLabel.Get(), "Assault Rifle") == NULL) thisLabel = "enhanced\\AK";
	if (stricmp (thisLabel.Get(), "Patrol Rifle") == NULL) thisLabel = "enhanced\\AR";
	if (stricmp (thisLabel.Get(), "Pocket Knife") == NULL) thisLabel = "enhanced\\B810";
	if (stricmp (thisLabel.Get(), "Snubnose Revolver") == NULL) thisLabel = "enhanced\\M29S";
	if (stricmp (thisLabel.Get(), "Compact Assault Rifle") == NULL) thisLabel = "enhanced\\Mk18";
	if (stricmp (thisLabel.Get(), "Magnum Pistol") == NULL) thisLabel = "enhanced\\Mk19T";
	if (stricmp (thisLabel.Get(), "Tactical Pump Shotgun") == NULL) thisLabel = "enhanced\\R870";
	if (stricmp (thisLabel.Get(), "SledgeHammer") == NULL) thisLabel = "enhanced\\SledgeHammer";
	if (stricmp (thisLabel.Get(), "Aztec Axe") == NULL) thisLabel = "aztec\\AztecAxe";
	if (stricmp (thisLabel.Get(), "Aztec Dagger") == NULL) thisLabel = "aztec\\AztecDagger";
	if (stricmp (thisLabel.Get(), "Aztec Spear") == NULL) thisLabel = "aztec\\AztecSpear";
	return thisLabel;
}
