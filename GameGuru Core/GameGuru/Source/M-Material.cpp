//----------------------------------------------------
//--- GAMEGURU - M-Material
//----------------------------------------------------

#include "stdafx.h"
#include "gameguru.h"

// 
//  Material Subroutines
// 

void material_init ( void )
{
	//  load material list (if not present)
	timestampactivity(0, cstr(cstr("_material_init (")+Str(g.gmaterialmax)+")").Get() );
	if (  g.gmaterialmax == 0 ) 
	{
		//PE: Had a crash in cStr, After recompiling it looks fine again , very strange ?
		t.tfile_s="audiobank\\materials\\materialdefault.txt";
		if (  FileExist(t.tfile_s.Get()) == 1 ) 
		{
			//  Load Data from file
			Dim (  t.data_s,999  );
			LoadArray (  t.tfile_s.Get() ,t.data_s );
			for ( t.l = 0 ; t.l<=  999; t.l++ )
			{
				t.line_s=t.data_s[t.l];
				if (  Len(t.line_s.Get())>0 ) 
				{
					if (  cstr(Lower(Left(t.line_s.Get(),4))) == ";end"  )  break;
					if (  cstr(Left(t.line_s.Get(),1)) != ";" )
					{
						//  take fieldname and values
						for ( t.c = 0 ; t.c < Len(t.line_s.Get()); t.c++ )
						{
							if (  t.line_s.Get()[t.c] == '=' ) { t.mid = t.c+1 ; break; }
						}
						t.field_s=Lower(removeedgespaces(Left(t.line_s.Get(),t.mid-1)));
						t.value_s=removeedgespaces(Right(t.line_s.Get(),Len(t.line_s.Get())-t.mid));

						for ( t.c = 0 ; t.c < Len(t.value_s.Get()); t.c++ )
						{
							if (  t.value_s.Get()[t.c] == ',' ) { t.mid = t.c+1 ; break; }
						}
						t.value1=ValF(removeedgespaces(Left(t.value_s.Get(),t.mid-1)));
						t.value2_s=removeedgespaces(Right(t.value_s.Get(),Len(t.value_s.Get())-t.mid));
						if (  Len(t.value2_s.Get())>0  )  t.value2 = ValF(t.value_s.Get()); else t.value2 = -1;
						//  load max materials
						t.tryfield_s="materialmax";
						if (  t.field_s == t.tryfield_s ) 
						{
							g.gmaterialmax=t.value1;
							if (  g.gmaterialmax<18  )  g.gmaterialmax = 18;
							Dim (  t.material,g.gmaterialmax );
						}
						#ifdef WICKEDENGINE
						//  load material data (MAX)
						for (t.m = 0; t.m <= g.gmaterialmax; t.m++)
						{
							t.tryfield_s = cstr("matdesc") + Str(t.m); if (t.field_s == t.tryfield_s)  t.material[t.m].name_s = t.value_s;
							t.tryfield_s = cstr("matwave") + Str(t.m); if (t.field_s == t.tryfield_s)  t.material[t.m].base_s = t.value_s;
							t.tryfield_s = cstr("matfreq") + Str(t.m); if (t.field_s == t.tryfield_s)  t.material[t.m].freq = t.value1;
							t.tryfield_s = cstr("matdecal") + Str(t.m); if (t.field_s == t.tryfield_s)  t.material[t.m].decal_s = t.value_s;
						}
						#else
						//  load material data (Classic)
						for (t.m = 0; t.m <= g.gmaterialmax; t.m++)
						{
							t.tryfield_s = cstr("matdesc") + Str(t.m); if (t.field_s == t.tryfield_s)  t.material[t.m].name_s = t.value_s;
							t.tryfield_s = cstr("matwave") + Str(t.m); if (t.field_s == t.tryfield_s)  t.material[t.m].tred0_s = t.value_s;
							t.tryfield_s = cstr("matwaves") + Str(t.m); if (t.field_s == t.tryfield_s)  t.material[t.m].scrape_s = t.value_s;
							t.tryfield_s = cstr("matwavei") + Str(t.m); if (t.field_s == t.tryfield_s)  t.material[t.m].impact_s = t.value_s;
							t.tryfield_s = cstr("matwaved") + Str(t.m); if (t.field_s == t.tryfield_s)  t.material[t.m].destroy_s = t.value_s;
							t.tryfield_s = cstr("matfreq") + Str(t.m); if (t.field_s == t.tryfield_s)  t.material[t.m].freq = t.value1;
							t.tryfield_s = cstr("matdecal") + Str(t.m); if (t.field_s == t.tryfield_s)  t.material[t.m].decal_s = t.value_s;
						}
						#endif
					}
				}
			}
			UnDim (  t.data_s );
		}

		#ifdef WICKEDENGINE
		// Fill in material defaults if info lacking (Classic)
		if (g.gmaterialmax > 0)
		{
			for (t.m = 0; t.m <= g.gmaterialmax; t.m++)
			{
				cstr sBase = t.material[t.m].base_s;
				char pFinalSoundFile[MAX_PATH];
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_footstep_walk_%s_left_1.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_WalkLeft][0] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_footstep_walk_%s_left_2.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_WalkLeft][1] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_footstep_walk_%s_left_3.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_WalkLeft][2] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_footstep_walk_%s_right_1.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_WalkRight][0] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_footstep_walk_%s_right_2.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_WalkRight][1] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_footstep_walk_%s_right_3.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_WalkRight][2] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_footstep_run_%s_left_1.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_RunLeft][0] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_footstep_run_%s_left_2.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_RunLeft][1] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_footstep_run_%s_left_3.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_RunLeft][2] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_footstep_run_%s_right_1.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_RunRight][0] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_footstep_run_%s_right_2.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_RunRight][1] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_footstep_run_%s_right_3.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_RunRight][2] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_light_land_%s_1.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_LandLight][0] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_light_land_%s_2.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_LandLight][1] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_hard_land_%s_1.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_LandHard][0] = pFinalSoundFile;
				sprintf(pFinalSoundFile, "audiobank\\materials\\%s\\SFX_hard_land_%s_2.wav", sBase.Get(), sBase.Get()); t.material[t.m].matsound_s[matSound_LandHard][1] = pFinalSoundFile;
				if (t.material[t.m].freq == 0)  t.material[t.m].freq = 22000;
			}
		}
		#else
		// Fill in material defaults if info lacking (Classic)
		if (  g.gmaterialmax>0 ) 
		{
			for ( t.m = 0 ; t.m<=  g.gmaterialmax; t.m++ )
			{
				if (  FileExist(t.material[t.m].scrape_s.Get()) == 0  )  t.material[t.m].scrape_s = t.material[t.m].tred0_s;
				if (  FileExist(t.material[t.m].impact_s.Get()) == 0  )  t.material[t.m].impact_s = t.material[t.m].tred0_s;
				if (  FileExist(t.material[t.m].destroy_s.Get()) == 0  )  t.material[t.m].destroy_s = t.material[t.m].tred0_s;
				t.material[t.m].tred1_s=cstr(Left(t.material[t.m].tred0_s.Get(),Len(t.material[t.m].tred0_s.Get())-4))+"A1.wav";
				t.material[t.m].tred2_s=cstr(Left(t.material[t.m].tred0_s.Get(),Len(t.material[t.m].tred0_s.Get())-4))+"A2.wav";
				t.material[t.m].tred3_s=cstr(Left(t.material[t.m].tred0_s.Get(),Len(t.material[t.m].tred0_s.Get())-4))+"A3.wav";
				t.material[t.m].tred0_s=cstr(Left(t.material[t.m].tred0_s.Get(),Len(t.material[t.m].tred0_s.Get())-4))+"A4.wav";
				if (  t.material[t.m].freq == 0  )  t.material[t.m].freq = 22000;
			}
		}
		#endif
	}
}

void material_startup ( void )
{
	for ( t.m = 0; t.m <= 18; t.m++ ) t.material[t.m].usedinlevel = 0;
	t.m = 0;  t.material[t.m].usedinlevel = 1;
	t.m = 1;  t.material[t.m].usedinlevel = 1;
	t.m = 2;  t.material[t.m].usedinlevel = 1;
	t.m = 3;  t.material[t.m].usedinlevel = 1;
	t.m = 6;  t.material[t.m].usedinlevel = 1;
	t.m = 10; t.material[t.m].usedinlevel = 1;
	t.m = 11; t.material[t.m].usedinlevel = 1;
	t.m = 13; t.material[t.m].usedinlevel = 1;
	t.m = 17; t.material[t.m].usedinlevel = 1;
	t.m = 18; t.material[t.m].usedinlevel = 1;
}

#define TENCLONESOUNDSFOREXPLOSIONS 10
#define SIXTHUMPSOUNDSFORMELEE 6

void material_loadsounds ( int iInitial )
{
	// Load silent sound
	if ( SoundExist(g.silentsoundoffset) == 0 ) Load3DSound ( "audiobank\\misc\\silence.wav",g.silentsoundoffset );

	// Load Explosion sound clones
	if ( SoundExist(g.explodesoundoffset) == 0 ) 
	{
		Load3DSound (  "audiobank\\misc\\explode.wav", g.explodesoundoffset );
		for ( t.t = 1 ; t.t <= TENCLONESOUNDSFOREXPLOSIONS; t.t++ ) CloneSound ( g.explodesoundoffset + t.t, g.explodesoundoffset );
	}

	// Load Melee Thump - not clones, variants for more variantarynessness (use SIXTHUMPSOUNDSFORMELEE)
	if ( SoundExist(g.meleethumpsoundoffset + 1) == 0 )
	{
		Load3DSound ("audiobank\\misc\\melee1.wav", g.meleethumpsoundoffset + 1);
		Load3DSound ("audiobank\\misc\\melee2.wav", g.meleethumpsoundoffset + 2);
		Load3DSound ("audiobank\\misc\\melee3.wav", g.meleethumpsoundoffset + 3);
		Load3DSound ("audiobank\\misc\\melee4.wav", g.meleethumpsoundoffset + 4);
		Load3DSound ("audiobank\\misc\\melee5.wav", g.meleethumpsoundoffset + 5);
		Load3DSound ("audiobank\\misc\\melee6.wav", g.meleethumpsoundoffset + 6);
	}

	// Load material sounds into memory
	if ( iInitial == 0 )
	{
		timestampactivity(0, cstr(cstr("_material_loadsounds (")+Str(g.gmaterialmax)+")").Get() );
		t.tbase = g.materialsoundoffset;
		for ( t.m = 0 ; t.m <= g.gmaterialmax; t.m++ )
		{
			// material sound loading SO slow for some reason!
			char pPrompt[1024];
			sprintf( pPrompt, "LOADING MATERIAL SOUND %d", 1+t.m );
			t.tsplashstatusprogress_s = pPrompt;
			timestampactivity(0,t.tsplashstatusprogress_s.Get());
			version_splashtext_statusupdate ( );

			// load material sound
			if ( t.material[t.m].name_s != "" && t.material[t.m].usedinlevel == 1 ) 
			{
				for ( int msoundtypes = 0; msoundtypes < matSound_Count; msoundtypes++)
				{
					// each sound type has diffetent variants
					for (int iter = 0; iter < MATSOUNDITERMAX; iter++)
					{
						t.snd_s = t.material[t.m].matsound_s[msoundtypes][iter];
						t.msoundassign = t.material[t.m].matsound_id[msoundtypes][iter];
						if (FileExist(t.snd_s.Get()) == 1 && t.msoundassign == 0)
						{
							// find free sound
							while (SoundExist(t.tbase) == 1 && t.tbase < g.materialsoundoffsetend - 5) ++t.tbase;

							// if sound within range of available slots
							if (t.tbase < g.materialsoundoffsetend - 5)
							{
								Load3DSound (t.snd_s.Get(), t.tbase);
								SetSoundSpeed (t.tbase, t.material[t.m].freq + g.soundfrequencymodifier);
								t.msoundassign = t.tbase;

								// leave a gap of five to clone in 'instances' for same sounds that need to play in overlap
								t.tbase += 5;
							}
						}
						t.material[t.m].matsound_id[msoundtypes][iter] = t.msoundassign;
					}
				}
				// fill in missing IDs so have a sound for any event
				int iCommonSound = -1;
				for (int msoundtypes = 0; msoundtypes < matSound_Count; msoundtypes++)
				{
					for (int iter = 0; iter < MATSOUNDITERMAX; iter++)
					{
						t.msoundassign = t.material[t.m].matsound_id[msoundtypes][iter];
						if (t.msoundassign == 0)
						{
							if (msoundtypes == matSound_WalkLeft || msoundtypes == matSound_WalkRight)
							{
								if(msoundtypes == matSound_WalkLeft)
									t.material[t.m].matsound_id[msoundtypes][iter] = t.material[t.m].matsound_id[matSound_RunLeft][iter];
								else
									t.material[t.m].matsound_id[msoundtypes][iter] = t.material[t.m].matsound_id[matSound_RunRight][iter];
							}
							if (msoundtypes == matSound_RunLeft || msoundtypes == matSound_RunRight)
							{
								if (msoundtypes == matSound_RunLeft)
									t.material[t.m].matsound_id[msoundtypes][iter] = t.material[t.m].matsound_id[matSound_WalkLeft][iter];
								else
									t.material[t.m].matsound_id[msoundtypes][iter] = t.material[t.m].matsound_id[matSound_WalkRight][iter];
							}
						}
						else
						{
							if (iCommonSound == -1) iCommonSound = t.msoundassign;
						}
					}
				}
				// finally ANY empty ones get a 'iCommonSound'
				for (int msoundtypes = 0; msoundtypes < matSound_Count; msoundtypes++)
				{
					for (int iter = 0; iter < MATSOUNDITERMAX; iter++)
					{
						t.msoundassign = t.material[t.m].matsound_id[msoundtypes][iter];
						if (t.msoundassign == 0)
						{
							t.material[t.m].matsound_id[msoundtypes][iter] = iCommonSound;
						}
					}
				}
			}
		}
		if ( t.tbase > g.materialsoundoffset ) 
		{
			t.tnewmax = (t.tbase-1)-g.materialsoundoffset;
			if ( t.tnewmax>g.materialsoundmax ) 
			{
				g.materialsoundmax = t.tnewmax;
			}
		}
	}
}

void material_loadplayersounds ( void )
{
	// determine if player start marker specified an alternate
	t.tfromtheplayerentityelementid = 0;
	t.tplayerstyle_s="player";
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.entid=t.entityelement[t.e].bankindex;
		if ( t.entid>0 ) 
		{
			if ( t.entityprofile[t.entid].ismarker == 1 ) 
			{
				t.ttryplayerstyle_s = t.entityelement[t.e].eleprof.soundset_s;
				if ( FileExist( cstr(cstr("audiobank\\voices\\")+t.ttryplayerstyle_s+"\\silent.wav").Get() ) == 1 ) 
				{
					// find the voice we should use
					t.tplayerstyle_s = t.ttryplayerstyle_s;

					// also a great time to find the player start marker to extract more data from later
					t.tfromtheplayerentityelementid = t.e;
				}
			}
		}
	}

	// (re)load player sounds
	if ( SoundExist(t.playercontrol.soundstartindex+1) == 0 || g.gplayerstyle_s != t.tplayerstyle_s ) 
	{
		g.gplayerstyle_s=t.tplayerstyle_s;
		for ( t.s = 1 ; t.s <= 99; t.s++ )
			if (  SoundExist(t.playercontrol.soundstartindex+t.s) == 1 ) 
				DeleteSound (  t.playercontrol.soundstartindex+t.s );

		// legacy or new style
		if (stricmp(t.tplayerstyle_s.Get(), "player") == 0)
		{
			// legacy file names (and the old pre-2025 default)
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_voice_hit_1.wav").Get(), t.playercontrol.soundstartindex + 1);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_voice_hit_2.wav").Get(), t.playercontrol.soundstartindex + 2);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_voice_hit_3.wav").Get(), t.playercontrol.soundstartindex + 3);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_voice_hit_4.wav").Get(), t.playercontrol.soundstartindex + 4);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_hard_land.wav").Get(), t.playercontrol.soundstartindex + 5);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_leap.wav").Get(), t.playercontrol.soundstartindex + 6);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_spawn.wav").Get(), t.playercontrol.soundstartindex + 7);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_voice_hit_5.wav").Get(), t.playercontrol.soundstartindex + 8);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_voice_hit_6.wav").Get(), t.playercontrol.soundstartindex + 9);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_voice_hit_7.wav").Get(), t.playercontrol.soundstartindex + 10);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_drown.wav").Get(), t.playercontrol.soundstartindex + 11);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_gasp_for_air.wav").Get(), t.playercontrol.soundstartindex + 12);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_water_in.wav").Get(), t.playercontrol.soundstartindex + 13);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_water_out.wav").Get(), t.playercontrol.soundstartindex + 14);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim.wav").Get(), t.playercontrol.soundstartindex + 15);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_voice_hit_8.wav").Get(), t.playercontrol.soundstartindex + 16);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_heartbeat.wav").Get(), t.playercontrol.soundstartindex + 17);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_jetpack.wav").Get(), t.playercontrol.soundstartindex + 18);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_voice_hit_9.wav").Get(), t.playercontrol.soundstartindex + 21);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_voice_hit_10.wav").Get(), t.playercontrol.soundstartindex + 22);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_voice_hit_11.wav").Get(), t.playercontrol.soundstartindex + 23);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_reactions_slap_1.wav").Get(), t.playercontrol.soundstartindex + 24);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_reactions_slap_2.wav").Get(), t.playercontrol.soundstartindex + 25);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_reactions_slap_3.wav").Get(), t.playercontrol.soundstartindex + 26);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_reactions_slap_4.wav").Get(), t.playercontrol.soundstartindex + 27);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_reactions_slap_5.wav").Get(), t.playercontrol.soundstartindex + 28);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_breath_hold.wav").Get(), t.playercontrol.soundstartindex + 31);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_breath_out.wav").Get(), t.playercontrol.soundstartindex + 32);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_breath_out_fast.wav").Get(), t.playercontrol.soundstartindex + 33);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_underwater.wav").Get(), t.playercontrol.soundstartindex + 34);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_1.wav").Get(), t.playercontrol.soundstartindex + 35);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_2.wav").Get(), t.playercontrol.soundstartindex + 36);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_3.wav").Get(), t.playercontrol.soundstartindex + 37);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_4.wav").Get(), t.playercontrol.soundstartindex + 38);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_underwater_1.wav").Get(), t.playercontrol.soundstartindex + 39);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_underwater_2.wav").Get(), t.playercontrol.soundstartindex + 40);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_underwater_3.wav").Get(), t.playercontrol.soundstartindex + 41);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_underwater_4.wav").Get(), t.playercontrol.soundstartindex + 42);
		}
		else
		{
			// new naming convention to allow male/female/type sound files to be found and customised
			// these sounds are generic and genderless
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_hard_land.wav").Get(), t.playercontrol.soundstartindex + 5);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_leap.wav").Get(), t.playercontrol.soundstartindex + 6);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_spawn.wav").Get(), t.playercontrol.soundstartindex + 7);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_drown.wav").Get(), t.playercontrol.soundstartindex + 11);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_gasp_for_air.wav").Get(), t.playercontrol.soundstartindex + 12);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_water_in.wav").Get(), t.playercontrol.soundstartindex + 13);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_water_out.wav").Get(), t.playercontrol.soundstartindex + 14);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim.wav").Get(), t.playercontrol.soundstartindex + 15);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_heartbeat.wav").Get(), t.playercontrol.soundstartindex + 17);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_jetpack.wav").Get(), t.playercontrol.soundstartindex + 18);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_reactions_metalblock_1.wav").Get(), t.playercontrol.soundstartindex + 26);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_reactions_metalblock_2.wav").Get(), t.playercontrol.soundstartindex + 27);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_reactions_metalblock_3.wav").Get(), t.playercontrol.soundstartindex + 28);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_underwater.wav").Get(), t.playercontrol.soundstartindex + 34);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_1.wav").Get(), t.playercontrol.soundstartindex + 35);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_2.wav").Get(), t.playercontrol.soundstartindex + 36);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_3.wav").Get(), t.playercontrol.soundstartindex + 37);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_4.wav").Get(), t.playercontrol.soundstartindex + 38);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_underwater_1.wav").Get(), t.playercontrol.soundstartindex + 39);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_underwater_2.wav").Get(), t.playercontrol.soundstartindex + 40);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_underwater_3.wav").Get(), t.playercontrol.soundstartindex + 41);
			LoadSound(cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\player_swim_underwater_4.wav").Get(), t.playercontrol.soundstartindex + 42);

			// these sounds can be gendered or type specific, so use the tplayerstyle_s to specify
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_1.wav").Get(), t.playercontrol.soundstartindex + 1);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_2.wav").Get(), t.playercontrol.soundstartindex + 2);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_3.wav").Get(), t.playercontrol.soundstartindex + 3);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_4.wav").Get(), t.playercontrol.soundstartindex + 4);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_5.wav").Get(), t.playercontrol.soundstartindex + 8);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_6.wav").Get(), t.playercontrol.soundstartindex + 9);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_7.wav").Get(), t.playercontrol.soundstartindex + 10);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_8.wav").Get(), t.playercontrol.soundstartindex + 16);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_9.wav").Get(), t.playercontrol.soundstartindex + 21);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_10.wav").Get(), t.playercontrol.soundstartindex + 22);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_11.wav").Get(), t.playercontrol.soundstartindex + 23);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_12.wav").Get(), t.playercontrol.soundstartindex + 24);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_13.wav").Get(), t.playercontrol.soundstartindex + 25);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\breath_hold.wav").Get(), t.playercontrol.soundstartindex + 31);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\breath_out.wav").Get(), t.playercontrol.soundstartindex + 32);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\breath_out_fast.wav").Get(), t.playercontrol.soundstartindex + 33);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_14.wav").Get(), t.playercontrol.soundstartindex + 44);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_15.wav").Get(), t.playercontrol.soundstartindex + 45);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_16.wav").Get(), t.playercontrol.soundstartindex + 46);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_17.wav").Get(), t.playercontrol.soundstartindex + 47);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_18.wav").Get(), t.playercontrol.soundstartindex + 48);
			LoadSound (cstr(cstr("audiobank\\voices\\") + t.tplayerstyle_s + "\\voice_hit_19.wav").Get(), t.playercontrol.soundstartindex + 49);
		}
		// 100 through to 995 - other sounds as required
	}
}

void material_triggersound_core ( int iSoundID )
{
	if (iSoundID > 0 && SoundExist(iSoundID) == 1)
	{
		PlaySound(iSoundID);
		PositionSound(iSoundID, t.tsx_f, t.tsy_f, t.tsz_f);
		SetSoundVolume(iSoundID, (80.0 + (t.tvol_f*0.2)) * t.audioVolume.soundFloat);
		t.tspd_f = t.tspd_f + g.soundfrequencymodifier;
		if (t.tspd_f > 66000) t.tspd_f = 66000;
		if (t.tspd_f > 2000) SetSoundSpeed(iSoundID, t.tspd_f);
	}
}

void material_triggersound ( int iPlayEvenIfStillPlaying )
{
	// when trigger, play a material sound
	if ( t.tsoundtrigger>0 ) 
	{
		bool bFoundOneToPlay = false;
		t.sbase=t.tsoundtrigger;
		for ( t.tchannels = 0 ; t.tchannels <= 4; t.tchannels++ )
		{
			t.ts=t.sbase+t.tchannels;
			//PE: create sound clones here if needed.
			if (t.tchannels > 0 && SoundExist(t.ts) == 0)
			{
				CloneSound(t.ts, t.sbase); //Clone original sound.
			}

			if ( SoundExist(t.ts) == 1 ) 
			{
				if ( SoundPlaying(t.ts) == 0 )
				{
					material_triggersound_core ( t.ts );
					bFoundOneToPlay = true;
					break;
				}
			}
		}
		if ( iPlayEvenIfStillPlaying == 1 && bFoundOneToPlay == false )
		{
			// find furthest sound, and use that one
			float fFurthestDD = 999999.0f;;
			int iFurthestSoundID = 0;
			for ( t.tchannels = 0 ; t.tchannels <= 4; t.tchannels++ )
			{
				t.ts=t.sbase+t.tchannels;
				if ( SoundExist(t.ts) == 1 ) 
				{
					float fDX = SoundPositionX(t.ts) - CameraPositionX(0);
					float fDZ = SoundPositionZ(t.ts) - CameraPositionZ(0);
					float fDD = sqrt(fabs(fDX*fDX)+fabs(fDZ*fDZ));
					if ( fDD < fFurthestDD )
					{
						fFurthestDD = fDD;
						iFurthestSoundID = t.ts;
					}
				}
			}
			if ( iFurthestSoundID > 0 )
			{
				material_triggersound_core ( iFurthestSoundID );
			}
		}
	}
}

void material_activatedecals ( void )
{
	//  Activate decals for materials
	for ( t.m = 0 ; t.m<=  g.gmaterialmax; t.m++ )
	{
		t.material[t.m].decalid=0;
		if (  t.material[t.m].name_s != "" ) 
		{
			t.decal_s=t.material[t.m].decal_s;
			decal_find ( );
			if (  t.decalid<0  )  t.decalid = 0;
			if (  t.decalid>0 ) 
			{
				t.material[t.m].decalid=t.decalid;
				t.decal[t.decalid].active=1;
			}
		}
	}
}
