//----------------------------------------------------
//--- GAMEGURU - M-LUA-Entity
//----------------------------------------------------

// Includes
#include "stdafx.h"
#include "gameguru.h"
#include <vector>
#include "GPUParticles.h"

// Externs
extern void entity_refreshelementforuse ( void );
void set_inputsys_mclick(int value);

// 
//  LUA Entity Commands
// 

// General

void entity_lua_starttimer ( void )
{
	t.entityelement[t.e].etimer=Timer();
	t.entityelement[t.e].lua.flagschanged=1;
	if (  t.game.runasmultiplayer  ==  1 && t.tLuaDontSendLua  ==  0 ) 
	{
		mp_sendlua (  MP_LUA_StartTimer,t.e,t.v );
	}
}

void entity_lua_findcharanimstate ( void )
{
	//  NOTE; This can be done as a pre-process and stored in the entityelement array
	//  NOTE; for instant retrieval (should save thousands of cycles per loop)
	//  takes e, returns tcharanimindex
	t.tcharanimindex=-1;
	for ( t.ttcharanimindex = 1 ; t.ttcharanimindex<=  g.charanimindexmax; t.ttcharanimindex++ )
	{
		if (  t.charanimstates[t.ttcharanimindex].e == t.e ) { t.tcharanimindex = t.ttcharanimindex  ; break; }
	}
	if (  t.tcharanimindex != -1  )  t.charanimstate = t.charanimstates[t.tcharanimindex];
}

void entity_lua_destroy ( void )
{
	// remove entity from level
	if ( t.entityelement[t.e].active != 0 )
	{
		bool bCanDestroy = true;
		if (t.entityelement[t.e].eleprof.iscollectable == 2)
		{
			if (t.entityelement[t.e].eleprof.quantity > 0)
			{
				bCanDestroy = false;
			}
		}
		if (bCanDestroy == true)
		{
			t.entityelement[t.e].destroyme = 1;
			if (t.game.runasmultiplayer == 1)
			{
				SteamDestroy (t.e);
			}
		}
	}
}

void entity_lua_collisionon ( void )
{
	if ( t.e > g.entityelementlist  )  return;
	t.obj=t.entityelement[t.e].obj;
	if (  t.obj>0 ) 
	{
		if ( ObjectExist(t.obj) == 1 ) 
		{
			t.entid=t.entityelement[t.e].bankindex;
			if ( t.entityelement[t.e].usingphysicsnow == 0 ) 
			{
				// create physics for entity if required
				physics_prepareentityforphysics ( );

				//  if a character, special physics setup
				int istorecharindex = t.tcharanimindex;
				t.ttte=t.e ; entity_find_charanimindex_fromttte ( );
				if ( t.tcharanimindex>0 ) 
				{
					t.tphyobj=t.entityelement[t.e].obj;
					t.tcollisionscaling = t.entityprofile[t.entid].collisionscaling;
					t.tcollisionscalingxz = t.entityprofile[t.entid].collisionscalingxz;
					physics_setupcharacter ( );
				}
				else
				{
					// 240217 - and activate physics for all, not just characters
					t.entityelement[t.e].usingphysicsnow = 1;
				}
				t.tcharanimindex = istorecharindex;
			}
			if ( t.entityelement[t.e].doorobsactive == 0 )
			{
				// if the LUA script name starts with "DOOR", the AI system will treat it as a blocking obstacle
				// so allowing path finder to ignore gaps that would have normally been used should the door be open
				if ( strnicmp ( t.entityprofile[t.entid].aimain_s.Get(), "door", 4) == NULL ) 
				{
					t.tobj=t.obj ;
					t.entityelement[t.e].doorobsactive = 1;
				}
			}
			//  ensure all transparent objects are removed from 'intersect all' consideration
			if (  t.entityprofile[t.entid].canseethrough == 1 || (t.entityprofile[t.entid].collisionmode == 11 && t.entityprofile[t.entid].ischaracter == 0) ) 
			{
				SetObjectCollisionProperty (  t.obj,1 );
			}
			else
			{
				SetObjectCollisionProperty (  t.obj,0 );
			}
		}
	}
	if (  t.game.runasmultiplayer  ==  1 && t.tLuaDontSendLua  ==  0 ) 
	{
		mp_sendlua (  MP_LUA_CollisionOn,t.e,t.v );
	}
}

void entity_lua_collisionoff ( void )
{
	t.tobj=t.entityelement[t.e].obj;
	if (  t.tobj>0 ) 
	{
		if (  ObjectExist(t.tobj) == 1 ) 
		{
			t.entid=t.entityelement[t.e].bankindex;
			if (  t.entityelement[t.e].usingphysicsnow != 0 ) 
			{
				t.tphyobj=t.tobj ; physics_disableobject ( );
				t.entityelement[t.e].usingphysicsnow=0;
			}
			if ( t.entityelement[t.e].doorobsactive == 1 )
			{
				if ( strnicmp ( t.entityprofile[t.entid].aimain_s.Get(), "door", 4) == NULL ) 
				{
					t.entityelement[t.e].doorobsactive = 0;
				}
			}
			SetObjectCollisionProperty (  t.tobj,1 );
		}
	}
	if (  t.game.runasmultiplayer  ==  1 && t.tLuaDontSendLua  ==  0 ) 
	{
		mp_sendlua (  MP_LUA_CollisionOff,t.e,t.v );
	}
}

// PROCESSING VIS FOR EVERYONE EVERY CYCLE IS SLOW, DO IT ON A LIST
std::vector<int> g_EntityPlrVisList;
#include "GGThread.h"
using namespace GGThread;
threadLock g_PlrVisibilityListLock;
void entity_lua_getentityplrvisible_clear ( void )
{
	while (!g_PlrVisibilityListLock.Acquire()) {}
	g_EntityPlrVisList.clear();
	g_PlrVisibilityListLock.Release();
}
void entity_lua_getentityplrvisible_processlist (void)
{
	if (g_EntityPlrVisList.size() > 0)
	{
		// entity to process
		while (!g_PlrVisibilityListLock.Acquire()) {}
		for ( int i = 0; i < g_EntityPlrVisList.size(); i++ )
		{
			//PE: We should never use globals in threads (t.e), it get changed everywhere,got a crash here (as it got changed while inside the function).
			int te = g_EntityPlrVisList[i];
			if (te > 0)
			{
				// calculate vis for this entity (LBOPT: can further optimize by placing this on a timer (no need to work out plr visibilty 30 times per second)
				int ttobj = t.entityelement[te].obj;
				if (ttobj > 0)
				{
					if (ObjectExist(ttobj) == 1)
					{
						charanimstatetype threadcas;
						threadcas.e = te;
						threadcas.obj = ttobj;
						threadcas.neckRightAndLeft = 0; //PE: ? we dont have it here.
						//PE: NOTE - t.plrid should also be changeable when mp :)
						darkai_calcplrvisible(threadcas);
					}
				}

				// and erase all other instances of E in list, as now up to date for plrvis
				for (int i = 0; i < g_EntityPlrVisList.size(); i++)
				{
					if (g_EntityPlrVisList[i] == te)
					{
						g_EntityPlrVisList[i] = 0;
					}
				}
			}
		}
		g_EntityPlrVisList.clear();
		g_PlrVisibilityListLock.Release();
	}
}

void entity_lua_getentityplrvisible ( void )
{
	if(t.e>0)
	{
		// only add if not already in list (solves issue of this list getting insanely massive)
		while (!g_PlrVisibilityListLock.Acquire()) {}
		int i = 0; for (; i < g_EntityPlrVisList.size(); i++) if (g_EntityPlrVisList[i] == t.e) break;
		if(i >= g_EntityPlrVisList.size()) g_EntityPlrVisList.push_back(t.e);
		g_PlrVisibilityListLock.Release();
	}
}

void entity_lua_getentityinzone ( int mode )
{
	// mode = 0 all If entity is zone, determine if ANY OTHER entity is inside it
	// mode = 1 to detect only active objects
	// mode = 2 to only detect characters that are active.
	// mode = 3 to only detect non-characters that are active.
	// mode = 4 to only detect non static objects.
	// mode = 5 to only detect static objects.

	bool bEntityIsReallyInZone = false;
	t.waypointindex=t.entityelement[t.e].eleprof.trigger.waypointzoneindex;
	if (  t.waypointindex>0 ) 
	{
		if ( t.waypoint[t.waypointindex].active == 1 ) 
		{
			if ( t.waypoint[t.waypointindex].style == 2 ) 
			{
				for ( int othere = 1; othere <= g.entityelementlist; othere++ )
				{
					if ( othere != t.e )
					{
						t.tpointx_f = t.entityelement[othere].x;
						t.tpointz_f = t.entityelement[othere].z;
						t.tokay = 0; waypoint_ispointinzone ( );
						if ( t.tokay != 0 )
						{
							bool bCheckon = false;
							if (mode == 0)
								bCheckon = true;
							else if(mode == 1 && t.entityelement[othere].active > 0)
								bCheckon = true;
							else if (mode == 2 && t.entityelement[othere].active > 0)
							{
								int masterid = t.entityelement[othere].bankindex;
								if (masterid > 0 && t.entityprofile[masterid].ischaracter > 0)
								{
									bCheckon = true;
								}
							}
							else if (mode == 3 && t.entityelement[othere].active > 0)
							{
								int masterid = t.entityelement[othere].bankindex;
								if (masterid > 0 && t.entityprofile[masterid].ischaracter == 0)
								{
									bCheckon = true;
								}
							}
							else if (mode == 4 && t.entityelement[othere].staticflag == 0)
							{
								bCheckon = true;
							}
							else if (mode == 5 && t.entityelement[othere].staticflag > 0)
							{
								bCheckon = true;
							}
							
							if (bCheckon)
							{
								if (t.entityelement[t.e].lua.entityinzone == 0)
								{
									t.entityelement[t.e].lua.entityinzone = othere;
									t.entityelement[t.e].lua.flagschanged = 1;
								}
								bEntityIsReallyInZone = true;
							}
						}
					}
				}
			}
		}
	}
	if (bEntityIsReallyInZone == false && t.entityelement[t.e].lua.entityinzone>0)
	{
		t.entityelement[t.e].lua.entityinzone = 0;
		t.entityelement[t.e].lua.flagschanged = 1;
	}
}

void entity_lua_hide ( void )
{
	if (t.entityprofile[t.entityelement[t.e].bankindex].ismarker == 10)
	{
		t.entityelement[t.e].eleprof.newparticle.bParticle_Show_At_Start = false;
		entity_updateparticleemitter(t.e);
	}
	else
	{
		t.tobj = t.entityelement[t.e].obj;
		if (t.tobj > 0)
		{
			if (ObjectExist(t.tobj) == 1)
			{
				HideObject (t.tobj);
			}
		}
		t.tattobj = t.entityelement[t.e].attachmentobj;
		if (t.tattobj > 0)
		{
			if (ObjectExist(t.tattobj) == 1)
			{
				HideObject (t.tattobj);
			}
		}
	}
}

void entity_lua_show ( void )
{
	if (t.entityprofile[t.entityelement[t.e].bankindex].ismarker == 10)
	{
		t.entityelement[t.e].eleprof.newparticle.bParticle_Show_At_Start = true;
		entity_updateparticleemitter(t.e);
	}
	else
	{
		if (t.entityelement[t.e].active != 0)
		{
			t.tobj = t.entityelement[t.e].obj;
			if (t.tobj > 0)
			{
				if (ObjectExist(t.tobj) == 1)
				{
					//PE: LuaNext() is getting "show" on marker object after second level has loaded.
					//PE: Expect this has something to do with activating object in rpg ?
					if (t.entityprofile[t.entityelement[t.e].bankindex].ismarker == 0)
						ShowObject (t.tobj);
				}
			}
			t.tattobj = t.entityelement[t.e].attachmentobj;
			if (t.tattobj > 0)
			{
				if (ObjectExist(t.tattobj) == 1)
				{
					ShowObject (t.tattobj);
				}
			}
		}
	}
}

void entity_lua_hideshowlimbs_core (bool bHide)
{
	if (t.entityprofile[t.entityelement[t.e].bankindex].ismarker == 0)
	{
		t.tobj = t.entityelement[t.e].obj;
		if (t.tobj > 0)
		{
			if (ObjectExist(t.tobj) == 1)
			{
				if (t.v > 0)
				{
					// simple hide or show specified limb
					if (bHide == true)
						HideLimb(t.tobj, t.v - 1);
					else
						ShowLimb(t.tobj, t.v - 1);
				}
				else
				{
					// hide/show all 'except specified limb'
					int iSpecifiedLimb = abs(t.v) - 1;
					sObject* pObject = g_ObjectList[t.tobj];
					for (int iLimb = 0; iLimb < pObject->iFrameCount; iLimb++)
					{
						if (iSpecifiedLimb != iLimb)
						{
							if (bHide == true)
								HideLimb(t.tobj, iLimb);
							else
								ShowLimb(t.tobj, iLimb);
						}
					}
				}
			}
		}
	}
}
void entity_lua_hidelimbs (void)
{
	entity_lua_hideshowlimbs_core(true);
}
void entity_lua_showlimbs (void)
{
	entity_lua_hideshowlimbs_core(false);
}

void entity_lua_spawn_core ( void )
{
	//  resurrect dead entity
	t.tte = t.e ; entity_freeragdoll ( );
	t.ttentid = t.entityelement[t.e].bankindex;

	//  restore attached object
	t.tattobj=t.entityelement[t.e].attachmentobj;
	if (  t.tattobj>0 ) { ODEDestroyObject (  t.tattobj  ) ; ShowObject (  t.tattobj ); }

	//  restore entity and it's AI
	t.entityelement[t.e].active=1;
	if (t.entityprofile[t.ttentid].ismarker == 0)
	{
		// only for regular objects, not things like zones (which also need to be respawned when reloading game position)
		t.entityelement[t.e].health = t.entityelement[t.e].eleprof.strength; //oops - t.entityprofile[t.ttentid].strength;
	}
	if ( Len(t.entityelement[t.e].eleprof.aimainname_s.Get())>1 ) 
	{
		t.entityelement[t.e].eleprof.aimain=1;
		t.entityelement[t.e].eleprof.aipreexit=-1;
	}

	//  restore object of entity
	t.obj=t.entityelement[t.e].obj;
	if (  t.obj>0 ) 
	{
		if (  ObjectExist(t.obj) == 1 ) 
		{
			t.entid=t.ttentid;
			if (  t.entityprofile[t.entid].ismarker == 0 ) 
			{
				t.tte=t.e ; entity_converttoinstance ( );
				PositionObject (  t.obj,t.entityelement[t.e].x,t.entityelement[t.e].y,t.entityelement[t.e].z );
				RotateObject (  t.obj,t.entityelement[t.e].rx,t.entityelement[t.e].ry,t.entityelement[t.e].rz );
				t.tentid=t.entid ; t.tte=t.e ; t.tobj=t.obj  ;
				//PE: Do not touch animations if object is already visible, only on new spawed objects.
				if (!GetVisible(t.tobj))
				{
					entity_resettodefaultanimation();
				}
				ShowObject (  t.obj );
			}
		}
	}

	// 051115 - only if not using limb visibility for hiding decal arrow
	if ( t.entityprofile[t.ttentid].addhandlelimb==0 )
	{
		//  restore LOD levels for object (ragdoll removed LOD feature)
		entity_calculateentityLODdistances ( t.ttentid, t.obj, t.entityelement[t.e].eleprof.lodmodifier );
	}

	// restore if character
	if ( t.entityprofile[t.ttentid].ischaracter == 1 ) 
	{
		for ( g.charanimindex = 1 ; g.charanimindex<=  g.charanimindexmax; g.charanimindex++ )
		{
			if ( t.charanimstates[g.charanimindex].originale == t.e ) 
			{
				// first destroy any part-leftovers of character
				t.charanimstate = t.charanimstates[g.charanimindex];
				t.entityelement[t.e].ragdollified = 1; // entity reload might have wiped ragdoll state
				t.charanimstates[g.charanimindex] = t.charanimstate;

				//  create new AI for this entity
				t.charanimstates[g.charanimindex].e = t.e;
				PositionObject ( t.charanimstates[g.charanimindex].obj,t.entityelement[t.e].x,t.entityelement[t.e].y,t.entityelement[t.e].z );
				darkai_setupcharacter ( );

				// finally initialise AI (see below)
				lua_initscript();

				// give resurrected characters some immunity to start with
				t.entityelement[t.e].briefimmunity = 100;

				//PE: Physics was lost after several spawn, and you cant shoot char.
				//PE: Explode state dont reset.
				//PE: https://github.com/TheGameCreators/GameGuruRepo/issues/429
				//PE: Make sure physics is setup again, and if char exploded is reset.
				entity_lua_collisionon();
				t.entityelement[t.e].collected = 0;
				t.entityelement[t.e].explodefusetime = 0;

				//PE: 21-06-2019 for Andy to test , just enable z when spawn.
				if (t.entityelement[t.e].obj > 0) 
				{
					EnableObjectZWrite(t.entityelement[t.e].obj);
					EnableObjectZDepth(t.entityelement[t.e].obj);
				}
				// found character, no need to continue
				break;
			}
		}
	}
	else
	{
		// 120916 - ensure collision restored if not character (exploding barrels could be walked through)
		// 120916 - seems collisionON sets SetObjectCollisionProperty to 0 (needed for exploding barrel)
		entity_lua_collisionon ( );
		t.entityelement[t.e].collected = 0;
		t.entityelement[t.e].explodefusetime = 0;
	}

	//  trigger LUA to update properties for this entity
	t.entityelement[t.e].lua.flagschanged=1;
}

void entity_lua_spawn ( void )
{
	t.tokay=1;
	t.ttentid=t.entityelement[t.e].bankindex;
	if ( t.game.runasmultiplayer == 1 && g.mp.coop == 0 && ( t.entityprofile[t.ttentid].ischaracter == 1 || t.entityelement[t.e].mp_isLuaChar  ==  1) ) t.tokay = 0;
	if ( t.tokay == 1 ) 
	{
		if ( t.entityelement[t.e].eleprof.spawnatstart == 0 ) 
		{
			// hidden in level, revealed by spawning
			t.entityelement[t.e].eleprof.spawnatstart = 2; // 300316 - new state means been spawned this game (use .health<=0 to know if spawned and died)
			t.entityelement[t.e].active=1;
			t.entityelement[t.e].eleprof.phyalways=0;
			entity_lua_show ( );
			entity_lua_collisionon ( );

			// 080517 - now triggers INIT AGAIN when spawning
			lua_initscript();

			// if object valid, ensure correct zdepth handling
			if (t.entityelement[t.e].obj > 0) 
			{
				EnableObjectZWrite(t.entityelement[t.e].obj);

				// ensure xorrect zdepth mode when spawn in
				//EnableObjectZDepth(t.entityelement[t.e].obj);
				entity_preparedepth(t.entityelement[t.e].bankindex, t.entityelement[t.e].obj);
			}
		}
		else
		{
			// initially visible, died, and need respawning
			if ( t.entityelement[t.e].health <= 0 ) 
			{
				//Because zones can fire off constantly, lets ensure we arent adding the same thing to the queue lots of times
				if ( t.entitiesToSpawnQueue.size() > 0 )
				{
					if ( t.entitiesToSpawnQueue[t.entitiesToSpawnQueue.size() -1] == t.e ) return;
				}
				t.entitiesToSpawnQueue.push_back ( t.e );
			}
			else
			{
				// reloading saved game can set spawnatstart==2, just need to ensure it becomes visible with correct spawnatstart=2 states
				if (t.entityelement[t.e].active == 1)
				{
					// and extra precaution of checking 'active' as objects are active==1 in this real-world scenario
					t.entityelement[t.e].eleprof.phyalways = 0;
					entity_lua_show ();
					entity_lua_collisionon ();
				}
			}
		}
	}
}

void entity_lua_setactivated ( void )
{
	t.entityelement[t.e].activated = t.v;
	t.entityelement[t.e].lua.flagschanged = 1;
}

void entity_lua_setactivatedformp ( void )
{
	t.entityelement[t.e].activated = t.v;
	t.entityelement[t.e].lua.flagschanged = 1;
	if ( t.game.runasmultiplayer == 1 && t.tLuaDontSendLua == 0 ) 
	{
		mp_sendlua ( MP_LUA_SetActivated, t.e, t.v );
	}
}

void entity_lua_resetlimbhit ( void )
{
	t.entityelement[t.e].detectedlimbhit = t.v;
}

std::vector<cstr> g_pIfUsedList;

void entity_lua_createifusedlist ( LPSTR pIfUsedString )
{
	// 170820 - can now have multiple ifused references concatonated together
	g_pIfUsedList.clear();
	char pAllIfUsed[MAX_PATH];
	strcpy(pAllIfUsed, pIfUsedString);
	for (int n = 0; n < strlen(pAllIfUsed); n++)
	{
		// if separator or end of string
		if (pAllIfUsed[n] == ':' || n == strlen(pAllIfUsed)-1)
		{
			// copy item to list
			char pIfUsedRef[MAX_PATH];
			strcpy(pIfUsedRef, pAllIfUsed);
			if ( pAllIfUsed[n] == ':' )	pIfUsedRef[n] = 0;
			g_pIfUsedList.push_back(pIfUsedRef);

			// chop off item and repeat until all items added
			strcpy(pAllIfUsed, pAllIfUsed+n+1); 
			n = 0;
		}
	}
}

bool entity_lua_manageactivationresult (int iEntityID)
{
	bool bResult = false;

	if (iEntityID > 0)
	{
		// in addition, if object inactive, spawn it (only if have health, otherwise this entity was really destroyed/collected)
		// active condition moved to caller as I need to call this even if active was 1 (loading saved games)
		if (t.entityelement[iEntityID].health > 0)
		{
			if (t.entityelement[iEntityID].activated == 1)
			{
				bool bSpawningValid = true;
				if (t.entityprofile[t.entityelement[iEntityID].bankindex].ischaracter == 1)
				{
					if (t.entityelement[iEntityID].health <= 0)
					{
						// cannot resurrect fallen characters
						bSpawningValid = false;
					}
				}
				if (bSpawningValid == true)
				{
					t.tstore = t.e;
					t.e = iEntityID;
					entity_lua_spawn_core();
					t.e = t.tstore;
					bResult = true;
				}
			}
		}

		// if item a particle, hide or show it
		if (t.entityprofile[t.entityelement[iEntityID].bankindex].ismarker == 10)
		{
			// show/hide if particle based on activation state
			if (t.entityelement[iEntityID].activated == 1)
			{
				bool bJustCreated = false;
				if (t.entityelement[iEntityID].eleprof.newparticle.bParticle_Show_At_Start == false)
				{
					bJustCreated = true;
				}
				t.entityelement[iEntityID].eleprof.newparticle.bParticle_Show_At_Start = true;
				entity_updateparticleemitter(iEntityID);

				if (bJustCreated)
				{
					int iParticleEmitter = t.entityelement[iEntityID].eleprof.newparticle.emitterid;
					if (iParticleEmitter != -1)
					{
						GPUParticles::gpup_setParticleScale(iParticleEmitter, t.entityelement[iEntityID].eleprof.newparticle.bParticle_Size);
						GPUParticles::gpup_setEffectAnimationSpeed(iParticleEmitter, t.entityelement[iEntityID].eleprof.newparticle.fParticle_Speed);
						GPUParticles::gpup_setEffectOpacity(iParticleEmitter, t.entityelement[iEntityID].eleprof.newparticle.fParticle_Opacity);
					}
				}
			}
			else
			{
				t.entityelement[iEntityID].eleprof.newparticle.bParticle_Show_At_Start = false;
				entity_updateparticleemitter(iEntityID);
			}
		}
	}

	return bResult;
}

void entity_lua_activateifused ( void )
{
	if (t.e > t.entityelement.size()) return; //PE: Got Crash Here.
	t.tstore = t.e;
	if ( t.game.runasmultiplayer == 1 && t.tLuaDontSendLua == 0 ) 
	{
		mp_sendlua ( MP_LUA_ActivateIfUsed, t.e, t.v );
	}
	entity_lua_createifusedlist(Lower(t.entityelement[t.e].eleprof.ifused_s.Get()));
	for (int ifusedindex = 0; ifusedindex < g_pIfUsedList.size(); ifusedindex++)
	{
		t.tifused_s = g_pIfUsedList[ifusedindex];
		for (t.e = 1; t.e <= g.entityelementlist; t.e++)
		{
			if (t.entityelement[t.e].obj > 0 )// && t.entityelement[t.e].active > 0) unspawned entities are zero active
			{
				if (cstr(Lower(t.entityelement[t.e].eleprof.name_s.Get())) == t.tifused_s)
				{
					// set activate flag
					t.entityelement[t.e].activated = 1;

					// also spawn if target entity not yet spawned
					if (t.entityelement[t.e].eleprof.spawnatstart == 0)
					{
						t.entitiesToActivateQueue.push_back(t.e);
					}

					// in addition, if object inactive, spawn it (only if have health, otherwise this entity was really destroyed/collected)
					bool bAllowActivationResultAction = false;
					int entid = t.entityelement[t.e].bankindex;
					if (t.entityprofile[entid].ismarker == 10) bAllowActivationResultAction = true;
					if (t.entityelement[t.e].active == 0)  bAllowActivationResultAction = true;
					if(bAllowActivationResultAction==true)
					{
						entity_lua_manageactivationresult(t.e);
					}

					// trigger LUA data to update for this element
					t.entityelement[t.e].lua.flagschanged = 1;
				}
			}
		}
	}
	t.e=t.tstore;
}

void entity_lua_performlogicconnections_core ( int iMode )
{
	// iMode : 0-normal, 1-askey , 2=only activate special Relationships
	for (int i = 0; i < 10; i++)
	{
		if (iMode == 2)
		{
			if (t.v < 0 || t.v > 9)
				return;
			i = t.v;
		}
		if (t.entityelement[t.e].eleprof.iObjectRelationships[i] > 0)
		{
			int iRelationShipObject = 0, iRelationShipEntityID = 0;
			void GetRelationshipObject (int iFindLinkID, int* piEntityID, int* piObj);
			GetRelationshipObject(t.entityelement[t.e].eleprof.iObjectRelationships[i], &iRelationShipEntityID, &iRelationShipObject);
			if (iRelationShipEntityID > 0 && iRelationShipEntityID != t.e)
			{
				// typically a value of 0,1,2 (one way) or 0,1,2,3,4,5 (two way)
				int iObjectRelationshipData = t.entityelement[t.e].eleprof.iObjectRelationshipsData[i];

				// the relationship type between item types
				// 2 = Character + Flag (supplies only data)
				// 5 = Flag + Flag (supplies only data)
				// 6 = Flag + Zone (do nothing)
				// 7 = Flag + Object (do nothing)
				int iObjectRelationshipsType = t.entityelement[t.e].eleprof.iObjectRelationshipsType[i];

				// what logic type should be performed
				if (iMode == 0 || iMode == 2)
				{
					// simple one way logic - activations
					switch (iObjectRelationshipsType)
					{
						case 1:  // Character + Character
						case 3:  // Character + Zone
						case 4:  // Character + Object
						case 8:  // Zone + Zone
						case 9:  // Zone + Object
						case 10: // Object + Object
						{
							t.entityelement[iRelationShipEntityID].eleprof.phyalways = 1; // any logic connections are always obeyed!
							t.entityelement[iRelationShipEntityID].whoactivated = t.e;
							switch (iObjectRelationshipData)
							{
								case 0: // activate
								{
									t.entityelement[iRelationShipEntityID].activated = 1;
									break;
								}
								case 1: // deactivate
								{
									t.entityelement[iRelationShipEntityID].activated = 0;
									break;
								}
								case 2: // toggle activation
								{
									// special case for particles that their active state is also their on/off state
									if (t.entityprofile[t.entityelement[iRelationShipEntityID].bankindex].ismarker == 10)
									{
										if (t.entityelement[iRelationShipEntityID].eleprof.newparticle.bParticle_Show_At_Start == true)
											t.entityelement[iRelationShipEntityID].activated = 1;
										else
											t.entityelement[iRelationShipEntityID].activated = 0;
									}

									if (t.entityelement[iRelationShipEntityID].activated == 0 || t.entityelement[iRelationShipEntityID].activated == 1)
									{
										// simple toggle
										t.entityelement[iRelationShipEntityID].activated = 1 - t.entityelement[iRelationShipEntityID].activated;
									}
									else
									{
										// activation state something else (like a door which is state 50 when locked)
										// in these cases, reset activate state which would have been the objects defaulty starting point (i.e. closed door, not open)
										t.entityelement[iRelationShipEntityID].activated = 0;
									}
									break;
								}
							}
							break;
						}
					}
				}
				if (iMode == 1)
				{
					// askey logic - add key name to usekey field string
					cstr keyName_s = t.entityelement[t.e].eleprof.name_s;

					// see if key is already named
					bool bAlreadyHaveThisKey = false;
					LPSTR pKeyString = t.entityelement[iRelationShipEntityID].eleprof.usekey_s.Get();
					LPSTR pKeyStringEnd = pKeyString + strlen(pKeyString);
					while (pKeyString && pKeyString < pKeyStringEnd)
					{
						char pThisKey[256];
						strcpy(pThisKey, pKeyString);
						for (int n = 0; n < strlen(pThisKey); n++)
						{
							if (pThisKey[n] == ';')
							{
								pThisKey[n] = 0;
								break;
							}
						}
						pKeyString += strlen(pThisKey) + 1;
						if (strlen(pThisKey) > 0)
						{
							if (stricmp(pThisKey, keyName_s.Get()) == NULL)
							{
								bAlreadyHaveThisKey = true;
								break;
							}
						}
					}
					if (bAlreadyHaveThisKey == false)
					{
						if (t.entityelement[iRelationShipEntityID].eleprof.usekey_s.Len() > 0) t.entityelement[iRelationShipEntityID].eleprof.usekey_s += cstr(";");
						t.entityelement[iRelationShipEntityID].eleprof.usekey_s += keyName_s;
						t.entityelement[iRelationShipEntityID].lua.haskey = 0;
					}
				}

				// only perform code below if target object is not visible (i.e. not spawned)
				bool bSkipResultsCode = false;
				int iObjID = t.entityelement[iRelationShipEntityID].obj;
				if (iObjID > 0 && ObjectExist(iObjID) == 1)
				{
					//PE: We must not do this if object is not visible. activated displayed cineguru camera.
					bool bVisible = GetVisible(iObjID); //PE: Hide from script.
					if (!bVisible) bSkipResultsCode = true;
					if (t.entityelement[iRelationShipEntityID].eleprof.spawnatstart == 0) bSkipResultsCode = false; //PE: Hide from ! spawn at start.
					//PE: Allow particle to be activated/deactivated.
					if (t.entityprofile[t.entityelement[iRelationShipEntityID].bankindex].ismarker == 10)
					{
						if (t.entityelement[iRelationShipEntityID].activated == 1 && !t.entityelement[iRelationShipEntityID].eleprof.newparticle.bParticle_Show_At_Start)
						{
								bSkipResultsCode = false;
						}
						else if (t.entityelement[iRelationShipEntityID].activated == 0 && t.entityelement[iRelationShipEntityID].eleprof.newparticle.bParticle_Show_At_Start)
						{
							bSkipResultsCode = false;
						}
					}
					if (bSkipResultsCode == false)
					{
						// if object inactive, spawn it (only if have health, otherwise this entity was really destroyed/collected)
						if (t.entityelement[iRelationShipEntityID].eleprof.spawnatstart != 2)
						{
							// only if not already spawned previously (otherwise it ends up with characters facing backwards!)
							if (entity_lua_manageactivationresult(iRelationShipEntityID) == true)
							{
								t.entityelement[iRelationShipEntityID].eleprof.spawnatstart = 2;
							}
						}
					}
				}
				// trigger LUA data to update for this element
				t.entityelement[iRelationShipEntityID].lua.flagschanged = 1;
			}
		}
		if (iMode == 2)
			return;
	}
}

void entity_lua_performlogicconnections()
{
	entity_lua_performlogicconnections_core(0);
}
void entity_lua_performlogicconnectionnumber()
{
	entity_lua_performlogicconnections_core(2);
}

void entity_lua_performlogicconnectionsaskey()
{
	entity_lua_performlogicconnections_core(1);
}

//Activate from the queue
int lastActiveTime = 0;
void entity_lua_activateifusedfromqueue ( void )
{
	if ( t.entitiesToActivateQueue.size() == 0 ) return;

	if ( Timer() - lastActiveTime < 70 ) return;

	lastActiveTime = Timer();

	t.tstore=t.e;
	t.e = t.entitiesToActivateQueue.back();
	t.entitiesToActivateQueue.pop_back();
	t.entitiesActivatedForLua.push_back ( t.e );
	t.delayOneFrameForActivatedLua = 1;
	t.tstore=t.e;
	entity_lua_spawn();
	t.e=t.tstore;
}

void entity_lua_spawnifused ( void )
{
	t.tstore=t.e;
	//t.tifused_s=Lower(t.entityelement[t.e].eleprof.ifused_s.Get());
	entity_lua_createifusedlist(Lower(t.entityelement[t.e].eleprof.ifused_s.Get()));
	for (int ifusedindex = 0; ifusedindex < g_pIfUsedList.size(); ifusedindex++)
	{
		t.tifused_s = g_pIfUsedList[ifusedindex];
		for (t.e = 1; t.e <= g.entityelementlist; t.e++)
		{
			if (cstr(Lower(t.entityelement[t.e].eleprof.name_s.Get())) == t.tifused_s)
			{
				t.entitiesToSpawnQueue.push_back(t.e);
			}
		}
	}
	t.e=t.tstore;
}

int lastSpawnedTime = 0;
void entity_lua_spawnifusedfromqueue ( void )
{
	if ( t.entitiesToSpawnQueue.size() == 0 ) return;
	if ( Timer() - lastSpawnedTime < 70 ) return;

	lastSpawnedTime = Timer();
	t.tstore=t.e;
	t.e = t.entitiesToSpawnQueue.back();
	t.entitiesToSpawnQueue.pop_back();
	entity_lua_spawn_core();
	t.e=t.tstore;
}

void entity_lua_transporttoifused ( void )
{
	//  takes e
	t.tstore=t.e;
	t.transporttoe=-1;
	entity_lua_createifusedlist(Lower(t.entityelement[t.e].eleprof.ifused_s.Get()));
	int ifusedindex = 0;
	if (g_pIfUsedList.size() > 0 && ifusedindex < g_pIfUsedList.size())
	{
		t.tifused_s = g_pIfUsedList[ifusedindex];
		for (t.e = 1; t.e <= g.entityelementlist; t.e++)
		{
			if (cstr(Lower(t.entityelement[t.e].eleprof.name_s.Get())) == t.tifused_s)
			{
				//  transport player to this location
				t.transporttoe = t.e;
			}
		}
	}
	else
	{
		// can also specify target for teleport using visual logic
		for (int i = 0; i < 10; i++)
		{
			if (t.entityelement[t.e].eleprof.iObjectRelationships[i] > 0)
			{
				int iRelationShipObject = 0, iRelationShipEntityID = 0;
				void GetRelationshipObject (int iFindLinkID, int* piEntityID, int* piObj);
				GetRelationshipObject(t.entityelement[t.e].eleprof.iObjectRelationships[i], &iRelationShipEntityID, &iRelationShipObject);
				if (iRelationShipEntityID > 0 && iRelationShipEntityID != t.e)
				{
					t.transporttoe = iRelationShipEntityID;
				}
			}
		}
	}
	t.e = t.tstore;
	if (t.transporttoe != -1)
	{
		t.huddamage.immunity = 200;
		t.freezeplayerposonly = 0;
		physics_disableplayer ();
		t.terrain.playerx_f = t.entityelement[t.transporttoe].x;
		t.terrain.playery_f = t.entityelement[t.transporttoe].y + 30;
		t.terrain.playerz_f = t.entityelement[t.transporttoe].z;
		t.terrain.playerax_f = 0;
		t.terrain.playeray_f = t.entityelement[t.transporttoe].ry;
		t.playercontrol.finalcameraangley_f = t.terrain.playeray_f;
		t.camangy_f = t.terrain.playeray_f;
		t.terrain.playeraz_f = 0;
		physics_setupplayer ();
	}
}

void entity_lua_refreshentity ( void )
{
	// all entity data updated directly, now  
	// ensure visible entity matches data again (for reloading game)
	bool bWeRestoredACloneSoRefresh = false;
	t.obj = t.entityelement[t.e].obj;
	if (t.obj == 0)
	{
		// scenario of entity cloned after level start, but when
		// game save reloaded, this entity does not exist, and needs to be created (as though previously cloned)
		if (t.e != t.v)
		{
			// if different, two params passed in, i.e: RefreshEntity(e,parente)
			t.gridentityoverwritemode = t.e;
			extern int SpawnNewEntityCore(int iEntityIndex);
			int iNewE = SpawnNewEntityCore(t.v);
			t.gridentityoverwritemode = 0;
			if (iNewE == t.e)
			{
				// success, created in same slot ;)
				// finally we can proceed as though object always existed in this level
				t.obj = t.entityelement[t.e].obj;

				// this fires up the entity!
				t.entityelement[iNewE].active = 0;
				t.entityelement[iNewE].lua.flagschanged = 123;
				bWeRestoredACloneSoRefresh = true;
			}
		}
	}
	if ( t.obj > 0 )
	{
		if ( ObjectExist ( t.obj ) == 1 )
		{
			// determine if should 1=destroy, 2=create, 3=createbutnotspawnedyet or 0=leave
			int iRefreshMode = 0;
			if (t.entityelement[t.e].collected != 0)
			{
				// entity was collected, leave alone
				iRefreshMode = 4;
			}
			else
			{
				if (t.entityelement[t.e].active == 0)
				{
					// do not destroy if entity yet to spawn
					if (t.entityelement[t.e].eleprof.spawnatstart == 0)
					{
						// not spawned yet, so needs creating for future spawn
						// iRefreshMode = 3; this caused characters to spawn twice when loading a saved game
						// so remove this to solve the reversing character issue, and keep an eye on other objects that might want this..
					}
					else
					{
						if (t.entityelement[t.e].eleprof.spawnatstart == 2 && t.entityelement[t.e].health <= 0)
						{
							// has been spawned, and killed, should be destroyed
							iRefreshMode = 1;
						}
						else
						{
							if (bWeRestoredACloneSoRefresh == true)
							{
								// entiy just restored an object that was cloned in the level, then game saved, then reloaded
								// and needs to be fully restored in its original cloned/spawned state
								iRefreshMode = 5;
							}
							else
							{
								// entity has been destroyed
								iRefreshMode = 1;
							}
						}
					}
				}
				else
				{
					// in scenario where entity has been Hidden in the _INIT part of the script, do NOT refresh
					// as cases such as CINEGURU elements, we keep them as is when they are hidden
					sObject* pObject = g_ObjectList[t.obj];
					if (pObject->bVisible == true)
					{
						// hmm, this seems to recreate entities that a fresh level already prepared and set up,
						// so when for example FISH are recreated, they go nuts due to collision changes, etc
						// but this still seems to be needed for particles, so for now only allow through for
						// particle use, and return here when disabling this for other scenarios demands a 
						// closer look as to why FISH are corrupted, but XX is/may not..
						if (t.entityprofile[t.entityelement[t.e].bankindex].ismarker == 10)
						{
							// entity needs creating?!
							iRefreshMode = 2;
						}
					}
				}
			}

			// activate flag switchies entities on and off
			if ( iRefreshMode == 1 )
			{
				// hide in any event (also done in script)
				entity_lua_hide();

				// deactivate collision if any
				entity_lua_collisionoff();

				//  possible remove character
				entity_lua_findcharanimstate ( );
				if (  t.tcharanimindex != -1 ) 
				{
					//  deactivate DarkA.I for this dead entity
					darkai_killai ( );

					//  Convert object back to instance and hide it
					t.charanimstates[t.tcharanimindex] = t.charanimstate;
				}
				else
				{
					//  can still have non-character ragdoll (zombie), so remove ragdoll if so
					t.tphyobj=t.obj ; ragdoll_destroy ( );
				}
			}
			if ( iRefreshMode == 2 || iRefreshMode == 3 )
			{
				// common resets when entity is renewed
				entity_refreshelementforuse();

				// Posiiton and rotate object prior to final physics position, along with other spawning qualities
				t.ttentid = t.entityelement[t.e].bankindex;
				int iHaveCurrentHealth = t.entityelement[t.e].health;
				entity_lua_spawn_core();
				t.entityelement[t.e].health = iHaveCurrentHealth;

				// only restore physics if had any to start with
				if (t.entityelement[t.e].usingphysicsnow != 0)
				{
					// delete old physics so collision on command can renew it
					t.entityelement[t.e].usingphysicsnow = 0;
					ODEDestroyObject (t.obj);

					// activate collision if any
					entity_lua_collisionon();
				}

				// handle particles that may have been activated
				if (iRefreshMode == 2)
				{
					// special case if particle was activated (and not spawned at start, need to show)
					if (t.entityprofile[t.entityelement[t.e].bankindex].ismarker == 10)
					{
						// show/hide if particle based on activation state
						if (t.entityelement[t.e].activated == 1)
						{
							if (t.entityelement[t.e].active == 0)
							{
								entity_lua_manageactivationresult(t.e);
							}
						}
					}
				}

				// this entity needs to be in 'unspawned state'
				if ( iRefreshMode == 3 )
				{
					// set to spawn later in game
					t.entityelement[t.e].eleprof.spawnatstart = 0;
					t.entityelement[t.e].active=0;
					entity_lua_hide ( );
					entity_lua_collisionoff ( );
				}
			}
		}	
	}
}

void entity_lua_collected ( void )
{
	t.entityelement[t.e].collected = t.v;
	extern void darklua_refreshhaskeystatefor(LPSTR);
	darklua_refreshhaskeystatefor(t.entityelement[t.e].eleprof.name_s.Get());
	t.entityelement[t.e].lua.flagschanged = 1;
}

void entity_lua_checkpoint ( void )
{
	if(t.e > 0) t.waypointindex = t.entityelement[t.e].eleprof.trigger.waypointzoneindex;

	// player inside zone
	waypoint_hide ( );

	// record player position
	t.playercheckpoint.stored=1;
	t.playercheckpoint.x=CameraPositionX(0);
	t.playercheckpoint.y=CameraPositionY(0);
	t.playercheckpoint.z=CameraPositionZ(0);
	t.playercheckpoint.a=t.entityelement[t.e].ry;

	// record all soundloops at the time
	bool bPauseAndResumeFromGameMenu = false;
	game_main_snapshotsoundloopcheckpoint(bPauseAndResumeFromGameMenu);
}

int entity_lua_sound_convertVtoTSND (int te, int tv)
{
	int ttsnd = 0;
	if (tv == 0) ttsnd = t.entityelement[te].soundset;
	if (tv == 1) ttsnd = t.entityelement[te].soundset1;
	if (tv == 2) ttsnd = t.entityelement[te].soundset2;
	if (tv == 3) ttsnd = t.entityelement[te].soundset3;
	if (tv == 4) ttsnd = t.entityelement[te].soundset4;
	if (tv == 5) ttsnd = t.entityelement[te].soundset5;
	if (tv == 6) ttsnd = t.entityelement[te].soundset6;
	return ttsnd;
}

void entity_lua_playsound ( void )
{
	t.tsnd = entity_lua_sound_convertVtoTSND(t.e, t.v);
	if ( t.tsnd > 0 )
	{
		//If we start in a zone, volume might not be set yet, so: (https://github.com/TheGameCreators/GameGuruRepo/issues/251)
		if (t.audioVolume.soundFloat == 0.0f)
		{
			t.audioVolume.soundFloat = 1.0f;
		}
		playinternal3dsound(t.tsnd,t.entityelement[t.e].x,t.entityelement[t.e].y,t.entityelement[t.e].z);
	}
	t.luaglobal.lastsoundnumber = t.tsnd;
	if ( t.game.runasmultiplayer == 1 && t.tLuaDontSendLua == 0 ) 
	{
		mp_sendlua (  MP_LUA_PlaySound,t.e,t.v );
	}
}

void entity_lua_playsoundifsilent ( void )
{
	t.tsnd = entity_lua_sound_convertVtoTSND(t.e, t.v);
	if ( t.tsnd>0 )
	{
		if ( SoundExist(t.tsnd) == 1 ) 
		{
			if ( SoundPlaying(t.tsnd) == 0 ) 
			{
				if (t.audioVolume.soundFloat == 0.0f)
				{
					t.audioVolume.soundFloat = 1.0f;
				}
				playinternal3dsound(t.tsnd,t.entityelement[t.e].x,t.entityelement[t.e].y,t.entityelement[t.e].z);
				t.luaglobal.lastsoundnumber=t.tsnd;
			}
		}
	}
}

void entity_lua_playnon3Dsound_core ( int iLoopMode )
{
	//  since sounds that call this will normally be 3D sounds, positioning the sound is still required. This will mean
	//  the sound playback will not be "non 3d", especially as the player moves. When process entities, if this sound
	//  is flagged as 'non 3D', we update the sound position if playing (in entity_loop). But only when the
	//  soundisnonthreedee flag is set so we don't perform unnecessary calls to SoundPlaying(x) and position
	t.tsnd = entity_lua_sound_convertVtoTSND(t.e, t.v);
	if ( t.tsnd>0 )
	{
		if ( SoundExist(t.tsnd) == 1 ) 
		{
			PositionSound (  t.tsnd,CameraPositionX(0),CameraPositionY(0),CameraPositionZ(0) );
			SetSoundVolume (  t.tsnd,soundtruevolume(100.0) );
			if (iLoopMode == 0) {
				PlaySound(t.tsnd);
			}
			else 
			{
				if (SoundLooping(t.tsnd) == 0) 
				{
					LoopSound(t.tsnd);
				}
			}
			t.entityelement[t.e].soundisnonthreedee=1;
		}
	}
	t.luaglobal.lastsoundnumber=t.tsnd;
}

void entity_lua_playnon3Dsound ( void )
{
	entity_lua_playnon3Dsound_core ( 0 );
}

void entity_lua_loopnon3Dsound ( void )
{
	entity_lua_playnon3Dsound_core ( 1 );
}

void entity_lua_loopsound ( void )
{
	t.tsnd = entity_lua_sound_convertVtoTSND(t.e, t.v);
	if ( t.tsnd>0 )
	{
		if (SoundExist(t.tsnd) == 1) {

			if (SoundLooping(t.tsnd) == 0)
			{
				loopinternal3dsound(t.tsnd, t.entityelement[t.e].x, t.entityelement[t.e].y, t.entityelement[t.e].z);
			}
			else
			{
				// keep calling LoopSound to update entity position during game
				posinternal3dsound(t.tsnd, t.entityelement[t.e].x, t.entityelement[t.e].y, t.entityelement[t.e].z);
			}
		}
	}
	t.luaglobal.lastsoundnumber=t.tsnd;
}

// New command to allow the setting of the active sound before other commands
void entity_lua_setsound ( void )
{
	t.tsnd = entity_lua_sound_convertVtoTSND(t.e, t.v);
	if ( t.tsnd > 0 )
	{
		t.luaglobal.lastsoundnumber=t.tsnd;
	}	
}

void entity_lua_stopsound ( void )
{
	t.tsnd = entity_lua_sound_convertVtoTSND(t.e, t.v);
	if ( t.tsnd>0 )
	{
		if ( SoundExist(t.tsnd) == 1 ) 
		{
			StopSound (  t.tsnd );
		}
	}
}

void entity_lua_setsoundspeed ( void )
{
	t.tsnd=t.luaglobal.lastsoundnumber;
	if ( t.tsnd>0 ) 
	{
		if ( SoundExist(t.tsnd) == 1 ) 
		{
			SetSoundSpeed (  t.tsnd,t.v );
		}
	}
}

std::unordered_map<int,float> luavolumes;
void restore_last_lua_volumes_settings( void )
{
	for (const auto& pair : luavolumes) {
		int snd = pair.first;
		float volume = pair.second;
		if (SoundExist(snd) == 1)
		{
			SetSoundVolume(snd, volume);
		}
	}
	luavolumes.clear();
}
void entity_lua_setsoundvolume ( void )
{
	t.tsnd=t.luaglobal.lastsoundnumber;
	if ( t.tsnd>0 ) 
	{
		if ( SoundExist(t.tsnd) == 1 ) 
		{
			t.tvolume_f = t.v;
			if (  t.tvolume_f<0  )  t.tvolume_f = 0;
			if (  t.tvolume_f>100  )  t.tvolume_f = 100;
			t.tvolume_f = t.tvolume_f * t.audioVolume.soundFloat;
			SetSoundVolume (  t.tsnd,t.tvolume_f );
			if (luavolumes.size() == 0)
			{
				luavolumes.reserve(100);
			}
			luavolumes.insert_or_assign(t.tsnd, t.tvolume_f);
		}
	}
}

void entity_lua_playspeech ( void )
{
	// establish ptr to mouth data list
	std::vector <sCharacterCreatorPlusMouthData> pMouthData;
	if ( t.v == 0 ) pMouthData = t.entityelement[t.e].lipset;
	if ( t.v == 1 ) pMouthData = t.entityelement[t.e].lipset1;
	if ( t.v == 2 ) pMouthData = t.entityelement[t.e].lipset2;
	if ( t.v == 3 ) pMouthData = t.entityelement[t.e].lipset3;
	if ( t.v == 4 ) pMouthData = t.entityelement[t.e].lipset4;

	// get character reference, to copy mouth data for character to playback
	entity_lua_findcharanimstate ( );
	if ( t.tcharanimindex != -1 ) 
	{
		// default is no mouth movement
		t.charanimstates[t.tcharanimindex].ccpo.speak.mouthData.clear();
		t.charanimstates[t.tcharanimindex].ccpo.speak.fMouthTimeStamp = 0.0f;
		t.charanimstates[t.tcharanimindex].ccpo.speak.iMouthDataShape = 0;
		t.charanimstates[t.tcharanimindex].ccpo.speak.iMouthDataIndex = 0;
		t.charanimstates[t.tcharanimindex].ccpo.speak.fNeedToBlink = 0.0f;

		// if character has mouthshapes animation, we can animate the mouth
		bool bObjectHasMouthShapesAnimation = false;
		int iObj = t.entityelement[t.e].obj;
		if (iObj > 0)
		{
			sObject* pObject = GetObjectData(iObj);
			if (pObject)
			{
				sAnimationSet* pAnimSet = pObject->pAnimationSet;
				while (pAnimSet)
				{
					if (stricmp(pAnimSet->szName, "mouthshapes") == NULL)
					{
						bObjectHasMouthShapesAnimation = true;
						break;
					}
					pAnimSet = pAnimSet->pNext;
				}
			}
		}
		if (bObjectHasMouthShapesAnimation == true )
		{
			// transfer mouth shape data over
			for (int n = 0; n < pMouthData.size(); n++)
				t.charanimstates[t.tcharanimindex].ccpo.speak.mouthData.push_back (pMouthData[n]);

			// and trigger mouth to start speaking
			t.charanimstates[t.tcharanimindex].ccpo.speak.fMouthTimeStamp = (float)Timer() / 1000.0f;
			t.charanimstates[t.tcharanimindex].ccpo.speak.fMouthTimeStamp -= 0.1f; // accelerate mouth by tenth of a second to arrive at mouth shape at same time as audio
			t.charanimstates[t.tcharanimindex].ccpo.speak.fSmouthDataSpeedToNextShape = 4.0f;
		}
	}
}

void entity_lua_stopspeech ( void )
{
	// stop any character mouth simulation
	entity_lua_findcharanimstate ( );
	if ( t.tcharanimindex != -1 ) 
	{
		t.charanimstates[t.tcharanimindex].ccpo.speak.fMouthTimeStamp = 0.0f;
	}
}

// video

void entity_lua_playvideonoskip ( int i3DMode, int iNoSkipFlag )
{
	if ( t.v > 1 ) return; 
	if ( t.v == 0 ) 
	{
		t.tvideoid=t.entityelement[t.e].soundset;
	}
	else
	{
		t.tvideoid=t.entityelement[t.e].soundset1;
	}
	if ( t.tvideoid<0 ) 
	{
		t.tvideoid=abs(t.tvideoid);
		if ( t.luaglobal.lastvideonumber>0 ) 
		{
			if ( AnimationExist(t.luaglobal.lastvideonumber) == 1 ) 
			{
				StopAnimation (  t.luaglobal.lastvideonumber );
			}
		}
		if ( AnimationExist(t.tvideoid) == 1 ) 
		{
			// run animation
			PlayAnimation (  t.tvideoid );
			if (i3DMode == 1)
			{
				PlaceAnimation(t.tvideoid, -10, -10, -5, -5);
				//PE: Use imgui instead , to render video as 2d ?.
				SetRenderAnimToImage(t.tvideoid, true);
				//ID3D11ShaderResourceView* lpVideoTexture = GetAnimPointerView(iVideoThumbID);
			}
			else
				PlaceAnimation ( t.tvideoid, 0, 0, GetDisplayWidth() + 1, GetDisplayHeight() + 1);

			// create object for 3D rendering
			if ( i3DMode == 1 )
			{
				if ( ObjectExist(g.video3dobjectoffset)==1 ) DeleteObject ( g.video3dobjectoffset );
				if ( ObjectExist(g.video3dobjectoffset)==0 )
				{
					float fCorrectWidth = AnimationWidth(t.luaglobal.lastvideonumber);
					float fCorrectHeight = AnimationHeight(t.luaglobal.lastvideonumber);
					MakeObjectBox(g.video3dobjectoffset, 10, fCorrectHeight/9.0f, fCorrectWidth/18);
					PositionObject ( g.video3dobjectoffset, -100000, -100000, -100000 );
					SetObjectEffect ( g.video3dobjectoffset, g.guishadereffectindex );
					DisableObjectZDepth ( g.video3dobjectoffset );
					DisableObjectZRead ( g.video3dobjectoffset );
					SetSphereRadius ( g.video3dobjectoffset, 0 );
					TextureObject ( g.video3dobjectoffset, 0, g.editorimagesoffset+14 );
					HideObject ( g.video3dobjectoffset ); // hide white rectangle issue
					if ( g.vrglobals.GGVREnabled > 0 && g.vrglobals.GGVRUsingVRSystem == 1 )
						SetObjectMask ( g.video3dobjectoffset, (1<<6) + (1<<7) + 1 );
					else
						SetObjectMask ( g.video3dobjectoffset, 1 );
				}
			}

			t.ttrackmouse=0;
			int triggerEndVideo = 0;

			while ( AnimationPlaying(t.tvideoid) == 1 ) 
			{
				// AnimationPlaying() always returns 1, so manually calculate if the video has ended.
				float fdone = GetAnimPercentDone(t.tvideoid) / 100.0f;
				if (fdone > 0.5)
					triggerEndVideo = 1;
				if (triggerEndVideo > 0 && (fdone == 0.0f || fdone >= 1.0f))
					break;

				// handle skip functionality
				if ( iNoSkipFlag == 0 )
				{
					set_inputsys_mclick(MouseClick());// t.inputsys.mclick = MouseClick();
					if (  t.inputsys.mclick == 0 && t.ttrackmouse == 0  )  t.ttrackmouse = 1;
					if (  t.inputsys.mclick != 0 && t.ttrackmouse == 1  )  t.ttrackmouse = 2;
					if (  t.inputsys.mclick == 0 && t.ttrackmouse == 2  )  break;
					if ( GGVR_RightController_Trigger() != 0 || GGVR_LeftController_Trigger() != 0 ) break;
				}
				else
				{
					// if test game, can skip with escape
					if (t.game.gameisexe == 0 && SpaceKey() == 1)
					{
						break;
					}
				}

				// handle 3d object if available 
				if ( i3DMode == 1 )
				{
					float fTrueAngY = t.playercontrol.cy_f; // better indicator of actual direction as VR tracker can invert (-180) the yaw
					RotateCamera(0, 0, fTrueAngY, 0);
					MoveCamera ( 0, 100.0f );
					float fX = CameraPositionX(0);
					float fZ = CameraPositionZ(0);
					float fY = CameraPositionY(0);
					float fA = fTrueAngY;
					MoveCamera ( 0, -100.0f );
					RotateCamera(0, 0, fTrueAngY, 0);
					PositionObject ( g.video3dobjectoffset, fX, fY-(ObjectSizeY(g.video3dobjectoffset)/4), fZ );
					PointObject ( g.video3dobjectoffset, ObjectPositionX(t.aisystem.objectstartindex), ObjectPositionY(t.aisystem.objectstartindex)+60.0f, ObjectPositionZ(t.aisystem.objectstartindex) );
					RotateObject ( g.video3dobjectoffset, 0, fA+180.0f+90, 0 ); // strangeness when playing VRQ videos, so made obj a box elongated oin Z axis and rotated 90 degrees
					OverrideTextureWithAnimation ( t.tvideoid, g.video3dobjectoffset ); //PE: This dont work with wicked.
				}

				//PE: Just a fullscreen video for now!
				//PE: Add a sprite draw call.
				//PE: TODO - Need to fit video ratio to screen ratio.
				//PE: TODO - Need i3DMode support should just render it smaller.
				//PE: TODO - Remove old plane. above.
				UpdateAllAnimation();
				ID3D11ShaderResourceView* lpVideoTexture = GetAnimPointerView(t.tvideoid);
				if (lpVideoTexture)
				{
					if (AnimationExist(t.tvideoid) && AnimationPlaying(t.tvideoid))
					{
						float animU = GetAnimU(t.tvideoid);
						float animV = GetAnimV(t.tvideoid);
						PasteImageRaw(lpVideoTexture, GetDisplayWidth() + 1, GetDisplayHeight() + 1, 0, 0, animU, animV, 0);
					}
				}
				// keep things going
				t.aisystem.processplayerlogic = 0;

				//PE: game_main_loop , will also execute lua scripts , so the script we are in can call itself again, gives some problems.
				//PE: @Lee is this needed while we play video ?
				//LB: Yes, for VR rendering, so moved needed calls here :)
				//game_main_loop ( ); 
				postprocess_preterrain ( );
				game_sync ( );

				//We need to render everything.
				void StartForceRender(void);
				StartForceRender();
			}


			//PE: Make sure Video texture lpVideoTexture , is removed from imgui drawlist, so we dont crash when its deleted below.
			//PE: dont matter what we render,it will get clipped. , TOOL_PENCIL = 43095
			//PE: g.editorimagesoffset + 14
			ID3D11ShaderResourceView* lpTexture = GetImagePointerView(43095);
			if (lpTexture)
			{
				DrawSpriteBatcher(); //Empty any batched sprites.
				PasteImageRaw(lpTexture, 2, 2, -100, -100, 0.0, 1.0, 0);
				void StartForceRender(void);
				StartForceRender();
			}

			//Make sure last frame is NOT rendered, so lpVideoTexture is valid.
			extern bool bRenderNextFrame;
			extern bool bBlockImGuiUntilNewFrame;
			bBlockImGuiUntilNewFrame = true;
			bRenderNextFrame = false;

			t.aisystem.processplayerlogic = 1;
			PlaceAnimation (  t.tvideoid,-1,-1,0,0 );
			StopAnimation (  t.tvideoid );
			t.luaglobal.lastvideonumber=t.tvideoid;
			// free 3d object
			if ( i3DMode == 1 )
			{
				if ( ObjectExist ( g.video3dobjectoffset ) == 1 ) DeleteObject ( g.video3dobjectoffset );
			}
			//  clear mouse deltas when return to game
			t.tclear=MouseMoveX();
			t.tclear=MouseMoveY();
			t.tclear=GetFileMapDWORD( 1, 0 );
			t.tclear=GetFileMapDWORD( 1, 4 );
		}
	}

	// ensure video trigger does not cause low FPS message
	g.lowfpstarttimer = Timer();
}

void entity_lua_stopvideo ( void )
{
	if ( t.v > 1 ) return; 
	if ( t.v == 0 ) 
	{
		t.tvideoid=t.entityelement[t.e].soundset;
	}
	else
	{
		t.tvideoid=t.entityelement[t.e].soundset1;
	}
	if ( t.tvideoid<0 ) 
	{
		t.tvideoid=abs(t.tvideoid);
		if ( AnimationExist(t.tvideoid) == 1 ) 
		{
			StopAnimation (  t.tvideoid );
			PlaceAnimation (  t.tvideoid,-1,-1,0,0 );
		}
	}
	t.luaglobal.lastvideonumber=0;
}

void entity_lua_moveupdate ( void )
{
	//  takes v# as indicator to entity speed
	t.te=t.e; 
	t.tv_f=t.v_f; 
	g.charanimindex = 0;
	entity_updatepos();
}

void entity_lua_rotateupdate ( void )
{
	t.obj=t.entityelement[t.e].obj;
	if (  t.obj>0 ) 
	{
		if (  t.entityelement[t.e].usingphysicsnow == 1 ) 
		{
			//  control physics object (entity-driven)
			t.tavy_f=t.entityelement[t.e].ry-ObjectAngleY(t.obj);
			t.tavy_f=t.tavy_f*10.0;
			ODESetAngularVelocity ( t.obj, 0, t.tavy_f, 0 );
		}
		RotateObject (  t.obj,t.entityelement[t.e].rx,t.entityelement[t.e].ry,t.entityelement[t.e].rz );
	}
}

void entity_lua_scaleupdate ( void )
{
	if (t.entityprofile[t.entityelement[t.e].bankindex].ismarker == 10)
	{
		t.entityelement[t.e].scalex = t.entityelement[t.e].eleprof.scale - 100.0f;
		entity_updateparticleemitter(t.e);
	}
	else
	{
		t.obj = t.entityelement[t.e].obj;
		ScaleObject (t.obj, t.entityelement[t.e].eleprof.scale, t.entityelement[t.e].eleprof.scale, t.entityelement[t.e].eleprof.scale);
	}
}

void entity_lua_moveup ( void )
{
	t.ttv_f=(t.v_f/19.775)*g.timeelapsed_f;
	t.entityelement[t.e].nogravity=1;
	t.entityelement[t.e].y=t.entityelement[t.e].y+t.ttv_f;
	entity_lua_moveupdate ( );
}

void entity_lua_sethoverfactor ( void )
{
	t.entityelement[t.e].hoverfactoroverride = t.v_f;
	entity_lua_moveupdate ( );
}

void entity_lua_moveforward_core_nooverlap ( int te, float* pNX, float* pNZ )
{
	float fPlrDistOfMovingEntity = t.entityelement[te].plrdist;
	for ( int tcharanimindex = 1; tcharanimindex <= g.charanimindexmax; tcharanimindex++ )
	{
		int ee = t.charanimstates[tcharanimindex].e;
		if ( te != ee && t.entityelement[te].eleprof.disableascharacter == 0 )
		{
			if ( t.entityelement[ee].health > 0 )
			{
				float fDX = t.entityelement[ee].x - *pNX;
				float fDZ = t.entityelement[ee].z - *pNZ;
				float fDD = sqrt ( fabs(fDX*fDX) + fabs(fDZ*fDZ) );
				if ( fDD > 0.0f && fDD < 30.0f )
				{
					fDX /= fDD;
					fDZ /= fDD;
					fDX *= 30.1f;
					fDZ *= 30.1f;
					*pNX = t.entityelement[ee].x - fDX;
					*pNZ = t.entityelement[ee].z - fDZ;
					// uses dynamicavoidance in a different way in co-op with behavior script system
					break;
				}
			}
		}
	}
}

void entity_lua_moveforward_core_ex (float fActualMoveUnitsOverride, float fActualMoveAngleOverride, float fForceWorldX, float fForceWorldZ )
{
	// resets
	float fLastX = 0;
	float fLastZ = 0;
	float fBeforeX = 0;
	float fBeforeZ = 0;

	// can move by timer MC-indie or actual move units (from animation spine v object)
	float fNewX = 0.0f;
	float fNewZ = 0.0f;
	if (fForceWorldX != 0.0f && fForceWorldZ != 0.0f)
	{
		// override entity angle with forced XZ shift but still use 'entity_lua_moveforward_core'
		fNewX = fForceWorldX;
		fNewZ = fForceWorldZ;
	}
	else
	{
		if (fActualMoveUnitsOverride != 0.0f)
			t.ttv_f = fActualMoveUnitsOverride;
		else
			t.ttv_f = (t.v_f / 19.775) * g.timeelapsed_f;

		// can also override actual angle of movement
		float fActualAngleOfMovement = t.entityelement[t.e].ry;
		if (fActualMoveAngleOverride != -1.0f) fActualAngleOfMovement = fActualMoveAngleOverride;
		fNewX = NewXValue(t.entityelement[t.e].x, fActualAngleOfMovement, t.ttv_f);
		fNewZ = NewZValue(t.entityelement[t.e].z, fActualAngleOfMovement, t.ttv_f);
	}

	int entid = t.entityelement[t.e].bankindex;
	bool bCancelAvoidFlag = false;
	if (t.entityprofile[entid].ischaracter == 1 && t.entityelement[t.e].eleprof.disableascharacter == 0)
	{
		// need a movement system that will NEVER enter another character radius
		fLastX = t.entityelement[t.e].x;
		fLastZ = t.entityelement[t.e].z;
		t.entityelement[t.e].x = fNewX;
		t.entityelement[t.e].z = fNewZ;
		fBeforeX = t.entityelement[t.e].x; // this is silly, the bCancelAvoidFlag will NEVER be true!!
		fBeforeZ = t.entityelement[t.e].z;
		entity_lua_moveforward_core_nooverlap ( t.e, &t.entityelement[t.e].x, &t.entityelement[t.e].z );
		if ( t.entityelement[t.e].x != fBeforeX || t.entityelement[t.e].z != fBeforeZ ) bCancelAvoidFlag = true;
	}
	else
	{
		t.entityelement[t.e].x = fNewX;
		t.entityelement[t.e].z = fNewZ;
	}

	// process movement of visual object (with physics) and calc avoidance state
	entity_lua_moveupdate ( );

	// uses dynamicavoidance in a different way in co-op with behavior script system
}

void entity_lua_moveforward_core (float fActualMoveUnitsOverride)
{
	entity_lua_moveforward_core_ex (fActualMoveUnitsOverride, -1.0f, 0.0f, 0.0f);
}

void entity_lua_moveforward ( void )
{
	entity_lua_moveforward_core ( 0.0f );
}

void entity_lua_movebackward ( void )
{
	t.ttv_f=(t.v_f/19.775)*g.timeelapsed_f;
	t.entityelement[t.e].x=NewXValue(t.entityelement[t.e].x,t.entityelement[t.e].ry,t.ttv_f*-1.0);
	t.entityelement[t.e].z=NewZValue(t.entityelement[t.e].z,t.entityelement[t.e].ry,t.ttv_f*-1.0);
	entity_lua_moveupdate ( );
}

void entity_lua_setpositionx ( void )
{
	t.entityelement[t.e].x=t.v_f;
	t.te=t.e ; t.tv_f=t.v_f ; g.charanimindex=0  ;entity_updatepos ( );
}

void entity_lua_setpositiony ( void )
{
	t.entityelement[t.e].y=t.v_f;
	t.te=t.e ; t.tv_f=t.v_f ; g.charanimindex=0  ; entity_updatepos ( );
}

void entity_lua_setpositionz ( void )
{
	t.entityelement[t.e].z=t.v_f;
	t.te=t.e ; t.tv_f=t.v_f ; g.charanimindex=0  ; entity_updatepos ( );
}

extern bool g_bResetPosIgnoreGravity;

void entity_lua_resetpositionx ( void )
{
	t.entityelement[t.e].x=t.v_f;
	if (  t.entityelement[t.e].usingphysicsnow == 1 ) 
	{
		t.tobj=t.entityelement[t.e].obj;
		t.entityelement[t.e].x = t.v_f;
		ODESetBodyPosition (  t.tobj,t.v_f, ObjectPositionY(t.tobj), ObjectPositionZ(t.tobj) );
		PositionObject (  t.tobj, t.v_f, ObjectPositionY(t.tobj), ObjectPositionZ(t.tobj) );
	}
	else
	{
		g_bResetPosIgnoreGravity = true;
		t.te=t.e ; t.tv_f=t.v_f ; g.charanimindex=0  ; entity_updatepos ( );
		g_bResetPosIgnoreGravity = false;
	}
	entity_lua_findcharanimstate ( );
	if ( t.tcharanimindex != -1 ) 
	{
		t.i = t.charanimstates[t.tcharanimindex].obj;
	}
}

void entity_lua_resetpositiony ( void )
{
	t.entityelement[t.e].y=t.v_f;
	if (  t.entityelement[t.e].usingphysicsnow == 1 ) 
	{
		t.tobj=t.entityelement[t.e].obj;
		t.entityelement[t.e].y = t.v_f;
		ODESetBodyPosition (  t.tobj,ObjectPositionX(t.tobj), t.v_f, ObjectPositionZ(t.tobj) );
		PositionObject (  t.tobj, ObjectPositionX(t.tobj), t.v_f, ObjectPositionZ(t.tobj) );
	}
	else
	{
		g_bResetPosIgnoreGravity = true;
		t.te=t.e ; t.tv_f=t.v_f ; g.charanimindex=0  ; entity_updatepos ( );
		g_bResetPosIgnoreGravity = false;
	}
	entity_lua_findcharanimstate ( );
	if ( t.tcharanimindex != -1 ) 
	{
		//t.i = t.charanimstates[g.charanimindex].obj;
		t.i = t.charanimstates[t.tcharanimindex].obj;
		// MAX has no AI subsystem
	}
}

void entity_lua_resetpositionz ( void )
{
	t.entityelement[t.e].z=t.v_f;
	if (  t.entityelement[t.e].usingphysicsnow == 1 ) 
	{
		t.tobj=t.entityelement[t.e].obj;
		t.entityelement[t.e].z = t.v_f;
		ODESetBodyPosition (  t.tobj,ObjectPositionX(t.tobj), ObjectPositionY(t.tobj), t.v_f );
		PositionObject (  t.tobj, ObjectPositionX(t.tobj), ObjectPositionY(t.tobj), t.v_f );
	}
	else
	{
		g_bResetPosIgnoreGravity = true;
		t.te=t.e ; t.tv_f=t.v_f ; g.charanimindex=0  ; entity_updatepos ( );
		g_bResetPosIgnoreGravity = false;
	}
	entity_lua_findcharanimstate ( );
	if ( t.tcharanimindex != -1 ) 
	{
		//t.i = t.charanimstates[g.charanimindex].obj;
		t.i = t.charanimstates[t.tcharanimindex].obj;
		// MAX has no AI subsystem
	}
}

void entity_lua_setrotationx ( void )
{
	t.entityelement[t.e].rx=t.v_f;
	entity_lua_rotateupdate ( );
}

void entity_lua_setrotationy ( void )
{
	t.entityelement[t.e].ry=t.v_f;
	entity_lua_rotateupdate ( );
}

void entity_lua_setrotationz ( void )
{
	t.entityelement[t.e].rz=t.v_f;
	entity_lua_rotateupdate ( );
}

void entity_lua_resetrotationx ( void )
{
	t.entityelement[t.e].rx = t.v_f;
	if (  t.entityelement[t.e].usingphysicsnow == 1 ) 
	{
		t.tobj=t.entityelement[t.e].obj;
		ODESetBodyAngle ( t.tobj,t.v_f, ObjectAngleY(t.tobj), ObjectAngleZ(t.tobj) );
		RotateObject ( t.tobj, t.v_f, ObjectAngleY(t.tobj), ObjectAngleZ(t.tobj) );
	}
	else
	{
		entity_lua_rotateupdate ( );
	}
}

void entity_lua_resetrotationy ( void )
{
	// 180520 - okay, some entities have rotations of >360 degrees
	// which can mess up math which expects -360 o 360, so
	// wrap when we use the reset rotation (added also to character AI init)
	t.v_f = WrapValue(t.v_f);
	t.entityelement[t.e].ry=t.v_f;
	if (  t.entityelement[t.e].usingphysicsnow == 1 ) 
	{
		t.tobj=t.entityelement[t.e].obj;
		ODESetBodyAngle ( t.tobj, ObjectAngleX(t.tobj), t.v_f, ObjectAngleZ(t.tobj) );
		RotateObject ( t.tobj, ObjectAngleX(t.tobj), t.v_f, ObjectAngleZ(t.tobj) );
	}
	else
	{
		entity_lua_rotateupdate ( );
	}

	// if character, also set the current angle as this was a hard reset of the Y angle
	entity_lua_findcharanimstate ();
	if (t.tcharanimindex != -1)
	{
		// hard change to rotation
		t.charanimstate.currentangle_f = t.entityelement[t.e].ry;
		t.charanimstates[t.tcharanimindex] = t.charanimstate;
	}
}

void entity_lua_resetrotationz ( void )
{
	t.entityelement[t.e].rz=t.v_f;
	if (  t.entityelement[t.e].usingphysicsnow == 1 ) 
	{
		t.tobj=t.entityelement[t.e].obj;
		ODESetBodyAngle ( t.tobj, ObjectAngleX(t.tobj), ObjectAngleY(t.tobj), t.v_f );
		RotateObject ( t.tobj, ObjectAngleX(t.tobj), ObjectAngleY(t.tobj), t.v_f );
	}
	else
	{
		entity_lua_rotateupdate ( );
	}
}

void entity_lua_modulatespeed ( void )
{
	t.entityelement[t.e].speedmodulator_f=t.v_f;
}

void entity_lua_rotatex ( void )
{
	t.v_f=(t.v_f/19.775)*g.timeelapsed_f;
	t.entityelement[t.e].rx=t.entityelement[t.e].rx+t.v_f;
	entity_lua_rotateupdate ( );
}
void entity_lua_rotatey ( void )
{
	t.v_f=(t.v_f/19.775)*g.timeelapsed_f;
	t.entityelement[t.e].ry=t.entityelement[t.e].ry+t.v_f;
	entity_lua_rotateupdate ( );
}
void entity_lua_rotatez ( void )
{
	t.v_f=(t.v_f/19.775)*g.timeelapsed_f;
	t.entityelement[t.e].rz=t.entityelement[t.e].rz+t.v_f;
	entity_lua_rotateupdate ( );
}

void entity_lua_setlimbindex ( void )
{
	int iObj = t.entityelement[t.e].obj;
	if ( iObj > 0 )
	{
		if ( ObjectExist(iObj)==1 )
		{
			if ( t.v < GetObjectData(iObj)->iFrameCount )
			{
				t.lualimbindex = t.v;
			}
		}
	}
}

void entity_lua_rotatelimbx ( void )
{
	int iObj = t.entityelement[t.e].obj;
	if ( iObj > 0 )
	{
		if ( ObjectExist(iObj)==1 )
		{
			if ( t.lualimbindex >= 0 && t.lualimbindex < GetObjectData(iObj)->iFrameCount )
			{
				RotateLimb ( iObj, t.lualimbindex, t.v_f, LimbAngleY(iObj, t.lualimbindex), LimbAngleZ(iObj, t.lualimbindex) );
			}
		}
	}
}
void entity_lua_rotatelimby ( void )
{
	int iObj = t.entityelement[t.e].obj;
	if ( iObj > 0 )
	{
		if ( ObjectExist(iObj)==1 )
		{
			if ( t.lualimbindex >= 0 && t.lualimbindex < GetObjectData(iObj)->iFrameCount )
			{
				RotateLimb ( iObj, t.lualimbindex, LimbAngleX(iObj, t.lualimbindex), t.v_f, LimbAngleZ(iObj, t.lualimbindex) );
			}
		}
	}
}
void entity_lua_rotatelimbz ( void )
{
	int iObj = t.entityelement[t.e].obj;
	if ( iObj > 0 )
	{
		if ( ObjectExist(iObj)==1 )
		{
			if ( t.lualimbindex >= 0 && t.lualimbindex < GetObjectData(iObj)->iFrameCount )
			{
				RotateLimb ( iObj, t.lualimbindex, LimbAngleX(iObj, t.lualimbindex), LimbAngleY(iObj, t.lualimbindex), t.v_f );
			}
		}
	}
}

void entity_lua_scale ( void )
{
	t.entityelement[t.e].eleprof.scale=t.v_f;
	entity_lua_scaleupdate ( );
}

void entity_lua_setanimation ( void )
{
	t.luaglobal.setanim=t.e;
	if (  t.game.runasmultiplayer  ==  1 && t.tLuaDontSendLua  ==  0 ) 
	{
		mp_sendlua (  MP_LUA_SetAnimation,t.e,0 );
	}
}

int entity_lua_getanimationnamefromobject (sObject* pObject, cstr FindThisName_s, float* fFoundStart, float* fFoundFinish)
{
	int iFoundBest = 0;
	LPSTR pFindThisName = FindThisName_s.Get();
	cStr lowercase_s = FindThisName_s.Lower();
	if (pObject)
	{
		for (int iSearchPatterns = 0; iSearchPatterns < 5 && iFoundBest == 0; iSearchPatterns++)
		{
			cStr lowercase_meaning_s = "";
			if (iSearchPatterns == 4)
			{
				// if all else fails, then check if animset name contains part of the general meaning of the name supplied by the script, case insensitive
				// used in situations where an old model uses something like Attack_01 but newer scripts use improved names such as Attack_Floor, etc
				// so we extract the general meaning of the new script animation name and match against a likely anim from the animset
				int iWritePosition = 0;
				char pLowercaseMeaning[MAX_PATH];
				LPSTR pLowercase = lowercase_s.Get();
				bool bStartGrabbingNow = false;
				for (int n = 0; n < strlen(pLowercase); n++)
				{
					if (pLowercase[n] >= 'a' && pLowercase[n] <= 'z')
					{
						if (bStartGrabbingNow == false) bStartGrabbingNow = true;
						pLowercaseMeaning[iWritePosition] = pLowercase[n];
						iWritePosition++;
					}
					else
					{
						if (bStartGrabbingNow == true)
						{
							// after we started grabbingthing, and the letters ran out, stop constructing string
							break;
						}
					}
				}
				pLowercaseMeaning[iWritePosition] = 0;
				lowercase_meaning_s = pLowercaseMeaning;
			}

			int iAnimSetCount = 1;
			sAnimationSet* pAnimSet = pObject->pAnimationSet;
			while (pAnimSet)
			{
				if (iSearchPatterns == 0)
				{
					// exact match and case match
					if (strcmp(pFindThisName, pAnimSet->szName) == NULL)
					{
						*fFoundStart = pAnimSet->fAnimSetStart;
						*fFoundFinish = pAnimSet->fAnimSetFinish;
						if (pAnimSet == pObject->pAnimationSet) *fFoundFinish = pAnimSet->ulLength;
						iFoundBest = iAnimSetCount;
						break;
					}
				}
				if (iSearchPatterns == 1)
				{
					// exact match and case insensitive
					char pAnimSetNameLower[MAX_PATH];
					strcpy (pAnimSetNameLower, pAnimSet->szName);
					strlwr(pAnimSetNameLower);
					if (strcmp(lowercase_s.Get(), pAnimSetNameLower) == NULL)
					{
						*fFoundStart = pAnimSet->fAnimSetStart;
						*fFoundFinish = pAnimSet->fAnimSetFinish;
						if (pAnimSet == pObject->pAnimationSet) *fFoundFinish = pAnimSet->ulLength;
						iFoundBest = iAnimSetCount;
						break;
					}
				}
				if (iSearchPatterns == 2)
				{
					// animset name contains the name, case insensitive, and have some specific cases we can watch out for
					char pAnimSetNameLower[MAX_PATH];
					strcpy (pAnimSetNameLower, pAnimSet->szName);
					strlwr(pAnimSetNameLower);
					if (strcmp (pAnimSetNameLower, "walk loop") == NULL) strcpy(pAnimSetNameLower, "walk"); // CCP can have "walk start", "walk loop" and walk finish" (need walk loop!)
					if (strcmp (pAnimSetNameLower, lowercase_s.Get()) == NULL)
					{
						*fFoundStart = pAnimSet->fAnimSetStart;
						*fFoundFinish = pAnimSet->fAnimSetFinish;
						if (pAnimSet == pObject->pAnimationSet) *fFoundFinish = pAnimSet->ulLength;
						iFoundBest = iAnimSetCount;
						break;
					}
				}
				if (iSearchPatterns == 3)
				{
					// animset name contains the name 'somewhere', case insensitive
					char pAnimSetNameLower[MAX_PATH];
					strcpy (pAnimSetNameLower, pAnimSet->szName);
					strlwr(pAnimSetNameLower);
					if (strstr(pAnimSetNameLower, lowercase_s.Get()) != NULL)
					{
						*fFoundStart = pAnimSet->fAnimSetStart;
						*fFoundFinish = pAnimSet->fAnimSetFinish;
						if (pAnimSet == pObject->pAnimationSet) *fFoundFinish = pAnimSet->ulLength;
						iFoundBest = iAnimSetCount;
						break;
					}
				}
				if (iSearchPatterns == 4)
				{
					// animset name contains part of the general meaning of the name supplied by the script, case insensitive
					// and see if we can match again 'lowercase_meaning_s'
					if (lowercase_meaning_s.Len() > 0)
					{
						char pAnimSetNameLower[MAX_PATH];
						strcpy (pAnimSetNameLower, pAnimSet->szName);
						strlwr(pAnimSetNameLower);
						if (strstr(pAnimSetNameLower, lowercase_meaning_s.Get()) != NULL)
						{
							*fFoundStart = pAnimSet->fAnimSetStart;
							*fFoundFinish = pAnimSet->fAnimSetFinish;
							if (pAnimSet == pObject->pAnimationSet) *fFoundFinish = pAnimSet->ulLength;
							iFoundBest = iAnimSetCount;
							break;
						}
					}
				}
				pAnimSet = pAnimSet->pNext;
				iAnimSetCount++;
			}
		}
	}
	return iFoundBest;
}

cstr entity_lua_getanimationbyindexviaobject(sObject* pObject, int animsetindextofind)
{
	if (pObject)
	{
		sAnimationSet* pAnimSet = pObject->pAnimationSet;
		if (pAnimSet)
		{
			if (stricmp(pAnimSet->szName, "all") == NULL)
			{
				pAnimSet = pAnimSet->pNext; // skip first one (base zero all anims)
			}
		}
		int iAnimSetCount = 1;
		while (pAnimSet)
		{
			if (stricmp(pAnimSet->szName, "mouthshapes") != NULL)
			{
				if (iAnimSetCount == animsetindextofind)
				{
					return cstr(pAnimSet->szName);
				}
				iAnimSetCount++;
			}
			pAnimSet = pAnimSet->pNext;
		}
	}
	return "";
}

cstr entity_lua_getanimationbyindex(int e, int animsetindextofind)
{
	sObject* pObject = GetObjectData(t.entityelement[e].obj);
	return entity_lua_getanimationbyindexviaobject(pObject, animsetindextofind);
}

int entity_lua_getanimationname (int e, cstr FindThisName_s, float* fFoundStart, float* fFoundFinish)
{
	// change animation to name if passed in numeric
	cstr pUseAnimName = FindThisName_s;
	LPSTR pAnimationName = (LPSTR)pUseAnimName.Get();
	if (strlen(pAnimationName) > 1)
	{
		if (pAnimationName[0] == '=')
		{
			// passed in name as numeric, i.e '=2' which means find animation at slot 2
			int iAnimSetIndexToFind = atoi(pAnimationName + 1);
			extern cstr entity_lua_getanimationbyindex(int, int);
			pUseAnimName = entity_lua_getanimationbyindex (e, iAnimSetIndexToFind);
		}
	}

	// and get the data
	sObject* pObject = GetObjectData(t.entityelement[e].obj);
	return entity_lua_getanimationnamefromobject (pObject, pUseAnimName, fFoundStart, fFoundFinish);
}

void entity_lua_setanimationname ( void )
{
	// find the start and finish frames from object
	float fFoundStart = -1, fFoundFinish = -1;
	entity_lua_getanimationname(t.e, t.s_s, &fFoundStart, &fFoundFinish);

	// set animation ready for object
	t.luaglobal.setanim=-1;
	t.luaglobal.setanimstart=fFoundStart;
	t.luaglobal.setanimfinish=fFoundFinish;
	if ( t.game.runasmultiplayer == 1 && t.tLuaDontSendLua == 0 ) 
	{
		mp_sendlua (  MP_LUA_SetAnimationFrames, fFoundStart, fFoundFinish );
	}
}

void entity_loop_using_negative_playanimineditor(int e, int obj, cstr animname)
{
	float fStartFrame = 0;
	float fFinishFrame = 0;
	sObject* pObject = GetObjectData(obj);
	if (strlen(animname.Get()) > 0 && entity_lua_getanimationnamefromobject (pObject, animname, &fStartFrame, &fFinishFrame) > 0)
	{
		LoopObject(obj, fStartFrame, fFinishFrame);
	}
	else
	{
		int entid = t.entityelement[e].bankindex;
		if (t.entityprofile[entid].ischaracter == 1)
		{
			// makes no sense to animate ALL the anims in a character, looks like a bug (IDLE not guarenteed to be at slot 1)
			LoopObject(obj, 0, 0);
		}
		else
		{
			animname = entity_lua_getanimationbyindexviaobject (pObject, 1);
			if (entity_lua_getanimationnamefromobject (pObject, animname, &fStartFrame, &fFinishFrame) > 0)
			{
				LoopObject(obj, fStartFrame, fFinishFrame);
			}
		}
	}
}

void entity_lua_setanimationframes ( void )
{
	t.luaglobal.setanim=-1;
	t.luaglobal.setanimstart=t.e;
	t.luaglobal.setanimfinish=t.v;
	if (  t.game.runasmultiplayer  ==  1 && t.tLuaDontSendLua  ==  0 ) 
	{
		mp_sendlua (  MP_LUA_SetAnimationFrames,t.e,t.v );
	}
}

void entity_lua_playorloopanimation ( float fStartFromPercentage )
{
	// ensure entity is cloned to allow animation
	t.tte = t.e; entity_converttoclone ( );

	// then do animation
	t.obj = t.entityelement[t.e].obj;
	if ( t.obj>0 ) 
	{
		if ( ObjectExist(t.obj) == 1 ) 
		{
			t.entid = t.entityelement[t.e].bankindex ; t.q=t.luaglobal.setanim;
			if ( t.q == -1 ) 
			{
				// play specific frames
				t.ttstart=t.luaglobal.setanimstart ; t.ttfinish=t.luaglobal.setanimfinish;
			}
			else
			{
				// play from FPE animsets
				t.ttstart=t.entityanim[t.entid][t.q].start ; t.ttfinish=t.entityanim[t.entid][t.q].finish;
			}

			// trigger transition to desired animation
			t.playflag=1-t.luaglobal.loopmode ; t.smoothanim[t.obj].st=-1;
			float fCurrentlyNotUsed = 5.0f;
			smoothanimtriggerrev(t.obj, t.ttstart, t.ttfinish, fCurrentlyNotUsed, 0, t.playflag, fStartFromPercentage);

			// ensure spine tracker is reset when start new animation
			sObject* pObject = GetObjectData(t.obj);
			pObject->bSpineTrackerMoving = false;

			// set as animating
			t.entityelement[t.e].lua.animating=1;
			LuaSetFunction ("UpdateEntityAnimatingFlag", 2, 0);
			LuaPushInt (t.e);
			LuaPushInt (t.entityelement[t.e].lua.animating);
			LuaCall ();
		}
	}
	if ( t.game.runasmultiplayer == 1 ) 
	{
		if ( t.luaglobal.setanim == -1 ) t.luaglobal.setanim = 0;
	}
}

void entity_lua_playanimation ( void )
{
	t.luaglobal.loopmode=0;
	entity_lua_playorloopanimation ( 0 );
	if (  t.game.runasmultiplayer  ==  1 && t.tLuaDontSendLua  ==  0 ) 
	{
		mp_sendlua (  MP_LUA_PlayAnimation,t.e,t.v );
	}
}

void entity_lua_playanimationfrom (void)
{
	t.luaglobal.loopmode = 0;
	entity_lua_playorloopanimation (t.v);
}

void entity_lua_loopanimation ( void )
{
	t.luaglobal.loopmode=1;
	entity_lua_playorloopanimation ( 0 );
}

void entity_lua_loopanimationfrom (void)
{
	t.luaglobal.loopmode = 1;
	entity_lua_playorloopanimation ( t.v );
}

void entity_lua_stopanimation ( void )
{
	t.obj=t.entityelement[t.e].obj;
	if (  t.obj>0 ) 
	{
		if (  ObjectExist(t.obj) == 1 ) 
		{
			StopObject (  t.obj );
		}
	}
}

void entity_lua_movewithanimation ( void )
{
	// is called continually for cycles where the animation should move the character
	entity_lua_findcharanimstate ( );
	if ( t.tcharanimindex != -1 ) 
	{
		int iID = t.entityelement[t.e].obj;
		if ( iID > 0 )
		{
			sObject* pObject = g_ObjectList [ iID ];
			if ( pObject )
			{
				if ( t.v == 1 )
				{
				}
				else
				{
					// keep character still, ignore spine vs base tracking
					if (pObject->bSpineTrackerMoving == true)
					{
						if (pObject->ppFrameList)
						{
							if (pObject->dwSpineCenterLimbIndex > 0)
							{
								sFrame* pFrame = pObject->ppFrameList[pObject->dwSpineCenterLimbIndex];
								if (pFrame)
								{
									WickedCall_SetBip01Position(pObject, pFrame, 0, 0, 0);
								}
							}
						}
						pObject->bSpineTrackerMoving = false;
					}
				}
			}
		}
	}
}

void entity_lua_setanimationframe ( void )
{
	// only for animating entities, which are visible (to prevent ALL animatable objects to clone when reload a saved game position)
	int iID = t.entityelement[t.e].obj;
	if ( iID > 0 )
	{
		sObject* pObject = g_ObjectList [ iID ];
		if ( pObject )
		{
		}
		else
			return;
	}
	else
		return;

	// force a frame in the entity object
	SetObjectFrameEx ( t.entityelement[t.e].obj, t.v_f );
}

void entity_lua_changeanimationframe (void)
{
	// only for animating entities, which are visible (to prevent ALL animatable objects to clone when reload a saved game position)
	int iID = t.entityelement[t.e].obj; if (iID <=0 ) return;
	sObject* pObject = g_ObjectList[iID]; if (!pObject) return;

	// force a frame in the entity object WITHOUT stopping the animation
	ChangeObjectFrame (t.entityelement[t.e].obj, t.v_f);
}

void entity_lua_setanimationspeed ( void )
{
	//  animspeed is modulated by timeelapsed in entity_loopanim
	t.entityelement[t.e].eleprof.animspeed = t.v_f * 100.0f; // 101115 - character scripts all refer to 1.0 as a speed of 100

	// and characters need to know this change immediately
	entity_lua_findcharanimstate ( );
	if (  t.tcharanimindex != -1 ) 
		t.charanimstates[t.tcharanimindex].animationspeed_f=(65.0/100.0)*t.entityelement[t.e].eleprof.animspeed;
}

// g_bForceRagdoll when calling 'entity_applydamage'
bool g_bForceRagdoll = false;
bool g_bForceNoRagdollJustDestroy = false;

void entity_lua_setentityhealth_core ( int iSilentOrDamage )
{
	// iSilentOrDamage  0 : none, 1 : silent, 2 : damage
	if (t.v == -12345 || t.v == -12346)
	{
		// set health to zero
		if (t.v == -12345) g_bForceRagdoll = true; // but also force ragdoll over preferred death anim
		if (t.v == -12346) g_bForceNoRagdollJustDestroy = true; // but also DO NOT force ragdoll or any preferred death anim!
		t.v = 0;
	}
	
	// cannot set any health/damage if currently in ragdoll phase
	if (t.entityelement[t.e].ragdollplusactivate != 0)
	{
		// the death flags above are for this call only, restored as at the end (else the next death of any cause takes them)
		g_bForceRagdoll = false;
		g_bForceNoRagdollJustDestroy = false;
		return;
	}
	// if new health is zero, apply damage to entity directly
	if ( t.v <= 0 && iSilentOrDamage == 0 )
	{
		//  set an entities health
		if ( t.entityelement[t.e].briefimmunity == 0 )
		{
			t.ttte = t.e;
			t.tdamage = t.entityelement[t.e].health;
			//PE: https://github.com/TheGameCreators/GameGuruRepo/commit/1863aaa879b05925670badf777a43825b1069702
			//PE: 150619 - this also produce this: https://github.com/TheGameCreators/GameGuruRepo/issues/466
			//PE: Changes so SetEntityHealth(e,-1) is needed to produce headshot effect.
			if (t.v == -1) 
			{
				t.tdamageforce = 700.0f; // 210918 - so headshots have better ragdoll reaction
				t.brayy1_f=t.entityelement[t.e].y-500.0;
				t.v = 0;
			}
			else 
			{
				t.tdamageforce = 0.0f; //PE: 150619
				t.brayy1_f = t.entityelement[t.e].y - 20.0; //PE:
			}
			t.tdamagesource = 0;
			t.brayx1_f=t.entityelement[t.e].x;
			t.brayx2_f=t.entityelement[t.e].x;
			t.brayy2_f=t.entityelement[t.e].y;
			t.brayz1_f=t.entityelement[t.e].z;
			t.brayz2_f=t.entityelement[t.e].z;
			t.tallowanykindofdamage=1;
			entity_applydamage ( );
			t.tallowanykindofdamage=0;
		}
	}
	else
	{
		if (t.v == -99999)
		{
			t.entityelement[t.e].briefimmunity = -1;
		}
		else
		{
			if (iSilentOrDamage == 2)
			{
				t.ttte = t.e;
				// check if immunity effectively blocked this damage
				if (t.entityelement[t.e].health > 99000)
				{
					// the entity t.e blocked this damage, allow them to
					// counter attack having deflected the potential damage avoided
				}
				else
				{
					t.tdamage = t.entityelement[t.e].health - t.v;
					t.tdamageforce = 0.0f;
					t.tdamagesource = 0;
					t.brayx1_f = t.entityelement[t.e].x;
					t.brayx2_f = t.entityelement[t.e].x;
					t.brayy1_f = t.entityelement[t.e].y;
					t.brayy2_f = t.entityelement[t.e].y;
					t.brayz1_f = t.entityelement[t.e].z;
					t.brayz2_f = t.entityelement[t.e].z;
					t.tallowanykindofdamage = 1;
					entity_applydamage ();
					entity_applydecalfordamage(t.e, -1, -1, -1);
					t.tallowanykindofdamage = 0;
				}
			}
			else
			{
				if (iSilentOrDamage == 1 && t.v == -1)
				{
					// allow non-zero damage again (reverse of Prevent Zero Health)
					t.entityelement[t.e].briefimmunity = 0;
				}
				else
				{
					if (iSilentOrDamage == 1 && t.v == 0) t.entityelement[t.e].briefimmunity = 0;
					t.entityelement[t.e].health = t.v;
				}
			}
		}
	}
	// and restore before leave
	g_bForceRagdoll = false;
	g_bForceNoRagdollJustDestroy = false;
}
void entity_lua_setentityhealth ( )
{
	entity_lua_setentityhealth_core ( 0 );
}
void entity_lua_setentityhealthsilent ( )
{
	entity_lua_setentityhealth_core ( 1 );
}
void entity_lua_setentityhealthwithdamage ()
{
	entity_lua_setentityhealth_core (2);
}

void entity_lua_setforcex ( void )
{
	t.brayx2_f = t.v; t.brayx1_f = 0.0f;
}
void entity_lua_setforcey ( void )
{
	t.brayy2_f = t.v; t.brayy1_f = 0.0f;
}
void entity_lua_setforcez ( void )
{
	t.brayz2_f = t.v; t.brayz1_f = 0.0f;
}
void entity_lua_setforcelimb ( void )
{
	t.entityelement[t.e].ragdollifiedforcelimb = t.v;
}
void entity_lua_ragdollforce ( void )
{
	//  set an entities ragdoll force value (t.e to t.v)
	t.entityelement[t.e].ragdollifiedforcex_f = (t.brayx2_f-t.brayx1_f)*0.8;
	t.entityelement[t.e].ragdollifiedforcey_f = (t.brayy2_f-t.brayy1_f)*1.2;
	t.entityelement[t.e].ragdollifiedforcez_f = (t.brayz2_f-t.brayz1_f)*0.8;
	t.entityelement[t.e].ragdollifiedforcevalue_f = t.v * 8000.0;
}

void entity_lua_charactercontrolmanual ( void )
{
	entity_lua_findcharanimstate ( );
	if (  t.tcharanimindex != -1 ) 
	{
		//  check we are not already in that state before sending out lua, so we dont flood
		if (  t.charanimstates[t.tcharanimindex].playcsi != g.csi_limbo ) 
			if (  t.game.runasmultiplayer  ==  1 && g.mp.coop  ==  1 && t.tLuaDontSendLua  ==  0 && g.mp.endplay  ==  0 ) 
				mp_sendlua (  MP_LUA_CharacterControlManual,t.e,t.v );

		//  disable all character control influence 
		t.charanimstate.playcsi=g.csi_limbo;
		t.charanimstate.limbomanualmode = 1; // (AND ENTER NEW AI FULL MANUAL MODE)
		t.charanimstates[t.tcharanimindex]=t.charanimstate;
	}
	if (  t.game.runasmultiplayer  ==  1 && g.mp.coop  ==  1 && t.tLuaDontSendLua  ==  0 ) 
		mp_sendlua (  MP_LUA_CharacterControlManual,t.e,t.v );
}

void entity_lua_charactercontrollimbo ( void )
{
	entity_lua_findcharanimstate ( );
	if (  t.tcharanimindex != -1 ) 
	{
		//  check we are not already in that state before sending out lua, so we dont flood
		if (  t.charanimstates[t.tcharanimindex].playcsi  !=  g.csi_limbo ) 
		{
			if (  t.game.runasmultiplayer  ==  1 && g.mp.coop  ==  1 && t.tLuaDontSendLua  ==  0 && g.mp.endplay  ==  0 ) 
			{
				mp_sendlua (  MP_LUA_CharacterControlLimbo,t.e,t.v );
			}
		}
		//  disable all character control influence
		t.charanimstate.playcsi=g.csi_limbo;
		t.charanimstate.limbomanualmode = 0; // REGULAR LIMBO MODE WITH SOME CONTROL OVER AI OBJECT
		t.charanimstates[t.tcharanimindex]=t.charanimstate;
	}
	if (  t.game.runasmultiplayer  ==  1 && g.mp.coop  ==  1 && t.tLuaDontSendLua  ==  0 ) 
	{
		mp_sendlua (  MP_LUA_CharacterControlLimbo,t.e,t.v );
	}
}

void entity_lua_charactercontrolunarmed ( void )
{
	entity_lua_findcharanimstate ( );
	if (  t.tcharanimindex != -1 ) 
	{
		//  check we are not already in that state before sending out lua, so we dont flood
		if ( t.charanimstates[t.tcharanimindex].playcsi != g.csi_unarmed ) 
		{
			if (  t.game.runasmultiplayer  ==  1 && g.mp.coop  ==  1 && t.tLuaDontSendLua  ==  0 && g.mp.endplay  ==  0 ) 
			{
				mp_sendlua (  MP_LUA_CharacterControlUnarmed,t.e,t.v );
			}
		}
		//  restful no weapon out
		if ( t.charanimstates[t.tcharanimindex].playcsi == g.csi_limbo  )  t.charanimstates[t.tcharanimindex].playcsi = g.csi_unarmed;
		t.charanimcontrols[t.tcharanimindex].alerted=0;
	}
}

void entity_lua_charactercontrolarmed ( void )
{
	entity_lua_findcharanimstate ( );
	if (  t.tcharanimindex != -1 && t.csi_stood.size() > 0)
	{
		// 200316 - replaced UNARMED state with STOOD (as this is the armed CSI states!)
		// check we are not already in that state before sending out lua, so we dont flood
		if ( t.charanimstates[t.tcharanimindex].playcsi != t.csi_stood[t.charanimstates[t.tcharanimindex].weapstyle] ) 
		{
			if (  t.game.runasmultiplayer  ==  1 && g.mp.coop  ==  1 && t.tLuaDontSendLua  ==  0 && g.mp.endplay  ==  0 ) 
			{
				mp_sendlua (  MP_LUA_CharacterControlArmed,t.e,t.v );
			}
		}
		// get weapon out (oh er)
		if (  t.charanimstates[t.tcharanimindex].playcsi == g.csi_limbo ) 
		{
			t.charanimstates[t.tcharanimindex].playcsi = t.csi_stood[t.charanimstates[t.tcharanimindex].weapstyle];
			if ( t.charanimstates[t.tcharanimindex].playcsi == 0 )
			{
				// 110416 - no weapon on this character, defer to unarmed state (legacy behaviour)
				t.charanimstates[t.tcharanimindex].playcsi = g.csi_unarmed;
			}
		}
		t.charanimcontrols[t.tcharanimindex].alerted=1;
	}
}

void entity_lua_charactercontrolfidget ( void )
{
	entity_lua_findcharanimstate ( );
	if (  t.tcharanimindex != -1 ) 
	{
		//  cause character to fidget - do we use charactercontrol array for everything?!?
		//  want to avoid 'unnecessary' state machines hard-coded into engine
		if (  t.charanimstates[t.tcharanimindex].playcsi == g.csi_limbo  )  t.charanimstates[t.tcharanimindex].playcsi = g.csi_unarmed;
	}
}

void entity_lua_charactercontrolducked ( void )
{
	entity_lua_findcharanimstate ( );
	if (  t.tcharanimindex != -1 ) 
	{
		//  ducked mode
		if (  t.charanimstates[t.tcharanimindex].playcsi == g.csi_limbo  )  t.charanimstates[t.tcharanimindex].playcsi = g.csi_unarmed;
		t.charanimcontrols[t.tcharanimindex].ducking = 1;
	}
return;

}

void entity_lua_charactercontrolstand ( void )
{
	entity_lua_findcharanimstate ( );
	if (  t.tcharanimindex != -1 ) 
	{
		//  stand up mode
		if (  t.charanimstates[t.tcharanimindex].playcsi == g.csi_limbo ) 
		{
			t.charanimstates[t.tcharanimindex].playcsi=g.csi_unarmed;
		}
		t.charanimcontrols[t.tcharanimindex].ducking = 2;
	}
}

void entity_lua_setcharactertowalkrun ( void )
{
	entity_lua_findcharanimstate ( );
	if (  t.tcharanimindex != -1 ) 
	{
		t.charanimstate.runmode=t.v;
		t.charanimstate.strafemode=0;
		t.charanimstates[t.tcharanimindex]=t.charanimstate;
	}
	if (  t.game.runasmultiplayer  ==  1 && g.mp.coop  ==  1 && t.tLuaDontSendLua  ==  0 ) 
	{
		mp_sendlua (  MP_LUA_setcharactertowalkrun,t.e,t.v );
	}
}

void entity_lua_setlockcharacter ( void )
{
	entity_lua_findcharanimstate ( );
	if (  t.tcharanimindex != -1 ) 
	{
		t.charanimstate.runmode=0;
		t.charanimstate.strafemode=0;
		t.charanimstate.freezeallmovement=t.v;
		t.charanimstates[t.tcharanimindex]=t.charanimstate;
	}
}

void entity_lua_setcharactertostrafe ( void )
{
	entity_lua_findcharanimstate ( );
	if (  t.tcharanimindex != -1 ) 
	{
		t.charanimstate.runmode=0;
		t.charanimstate.strafemode=1+t.v;
		t.charanimstates[t.tcharanimindex]=t.charanimstate;
	}
return;

}

void entity_lua_setcharactervisiondelay ( void )
{
	entity_lua_findcharanimstate ( );
	if (  t.tcharanimindex != -1 ) 
	{
		t.charanimstate.visiondelay=t.v;
		t.charanimstates[t.tcharanimindex]=t.charanimstate;
	}
}

void entity_lua_lookatplayer ( void )
{
	entity_lua_findcharanimstate();
	if (t.tcharanimindex != -1)
	{
		// for smoothness, move smooth track so always called until reach final position (for head tracking)
		t.charanimstates[t.tcharanimindex].entityTarget = 0;
		t.charanimstates[t.tcharanimindex].neckAiming = t.v_f;
	}
}

int g_presetTargetE = 0;
float g_presetTargetYOffset_f = 0.0f;

void entity_lua_lookattargetyoffset (void)
{
	g_presetTargetYOffset_f = t.v_f;
}

void entity_lua_lookattargete (void)
{
	g_presetTargetE = (int)t.v_f;
	if (g_presetTargetE < 0) g_presetTargetE = 0;
}

void entity_lua_lookattarget (void)
{
	entity_lua_findcharanimstate();
	if (t.tcharanimindex != -1)
	{
		t.charanimstates[t.tcharanimindex].entityTarget = g_presetTargetE;
		t.charanimstates[t.tcharanimindex].entityTargetYOffset_f = g_presetTargetYOffset_f;
		t.charanimstates[t.tcharanimindex].neckAiming = t.v_f;
	}
}

void entity_lua_aimsmoothmode (void)
{
	entity_lua_findcharanimstate();
	if (t.tcharanimindex != -1)
	{
		// for smoothness, move smooth track so always called until reach final position (for spine)
		t.charanimstates[t.tcharanimindex].entityTarget = g_presetTargetE;
		t.charanimstates[t.tcharanimindex].entityTargetYOffset_f = g_presetTargetYOffset_f;
		t.charanimstates[t.tcharanimindex].spineAiming = t.v_f;
	}
}

void entity_lua_lookforward ( void )
{
	entity_lua_findcharanimstate();
	if ( t.tcharanimindex != -1 ) 
	{
		t.charanimstates[t.tcharanimindex].neckAiming = 0.0f;
	}
}
void entity_lua_lookatangle (void)
{
	entity_lua_findcharanimstate();
	if (t.tcharanimindex != -1)
	{
		t.charanimstates[t.tcharanimindex].neckAiming = (t.v_f - 10000); // special code to store forced angle
	}
}

void entity_lua_rotatetoanglecore ( float fDestAngle, float fAngleOffset )
{
	entity_lua_findcharanimstate ( );
	if ( t.v == 100 ) 
	{
		t.tsmooth_f=1.0;
	}
	else
	{
		t.tsmooth_f=(100.0/(t.v+0.0))/g.timeelapsed_f;
	}
	if ( t.tcharanimindex == -1 || t.entityelement[t.e].eleprof.disableascharacter == 1 )
	{
		//  regular entity
		t.tnewangley_f=CurveAngle(fDestAngle,t.entityelement[t.e].ry,t.tsmooth_f);
		t.entityelement[t.e].ry=t.tnewangley_f;
		entity_lua_rotateupdate ( );
	}
	else
	{
		// MAX has no AI subsystem - but for characters we will set the destination angle and smoothly rotate within char_loop or similar
		t.charanimstate.currentangle_f = fDestAngle;
		float fModulateRotSpeed = t.charanimstate.iRotationAlongPathMode / 100.0f;
		t.charanimstate.currentangleslowlyspeed_f = (t.v * fModulateRotSpeed);
		t.charanimstate.moveToMode = 0; // face target will override goto target
		t.charanimstates[t.tcharanimindex] = t.charanimstate;
	}
	if (  t.game.runasmultiplayer  ==  1 && g.mp.coop  ==  1 && t.tLuaDontSendLua  ==  0 ) 
	{
		if (  Timer() - t.entityelement[t.e].mp_rotateTimer > 1000 ) 
		{
			mp_sendlua (  MP_LUA_RotateToPlayer,t.e,t.tnewangley_f );
			t.entityelement[t.e].mp_rotateTimer = Timer();
		}
	}
}

void entity_lua_rotatetocore(float fAngleOffset)
{
	t.tdx_f=t.tcamerapositionx_f-t.entityelement[t.e].x;
	t.tdz_f=t.tcamerapositionz_f-t.entityelement[t.e].z;
	float fDestAngle = atan2deg(t.tdx_f,t.tdz_f);
	entity_lua_rotatetoanglecore ( fDestAngle, fAngleOffset );
}

void entity_lua_rotatetoplayer ( void )
{
	//  third person moves camera to player position
	t.tcamerapositionx_f=CameraPositionX(t.terrain.gameplaycamera);
	t.tcamerapositiony_f=CameraPositionY(t.terrain.gameplaycamera);
	t.tcamerapositionz_f=CameraPositionZ(t.terrain.gameplaycamera);
	entity_lua_rotatetocore ( 0.0f );
}

void entity_lua_rotatetocamera ( void )
{
	//  for when we want the REAL camera (for decals)
	entity_gettruecamera ( );
	entity_lua_rotatetocore ( 0.0f );
}

void entity_lua_rotatetoplayerwithoffset ( void )
{
	t.tcamerapositionx_f=CameraPositionX(t.terrain.gameplaycamera);
	t.tcamerapositiony_f=CameraPositionY(t.terrain.gameplaycamera);
	t.tcamerapositionz_f=CameraPositionZ(t.terrain.gameplaycamera);
	entity_lua_rotatetocore ( t.v_f );
}

void entity_lua_set_gravity ( void )
{
	t.entityelement[t.e].nogravity=t.v;
	ODESetNoGravity(t.entityelement[t.e].obj, 1 - t.entityelement[t.e].nogravity);
}

void entity_lua_fireweapon ( bool instant )
{
	entity_lua_findcharanimstate();
	if (t.tcharanimindex != -1)
	{
		// at this point, weapons already animating a fire so move this to canfire code
		darkai_shoottarget(t.charanimstates[t.tcharanimindex].entityTarget);
		t.charanimstates[t.tcharanimindex] = t.charanimstate;
	}
}

void entity_lua_hurtplayer ( void )
{
	t.tdamage=t.v ; t.te=t.e ; t.tDrownDamageFlag=0;
	physics_player_takedamage ( );
}

void entity_lua_drownplayer ( void )
{
	t.tdamage=t.v ; t.te=t.e ; t.tDrownDamageFlag=1;
	physics_player_takedamage ( );
	t.tDrownDamageFlag=0;
}

void entity_lua_switchscript ( void )
{
	bool bAllowScriptToSwitch = true;
	if (strlen(t.s_s.Get()) > 0)
	{
		// only record original behavior
		if (t.entityelement[t.e].lua.returningaimainstored == 0)
		{
			t.entityelement[t.e].lua.returningaimain_s = t.entityelement[t.e].eleprof.aimain_s;
			t.entityelement[t.e].lua.returningaimainstored = 1;
		}
	}
	else
	{
		// when want to return, pass in ""
		if (t.entityelement[t.e].lua.returningaimainstored == 1)
		{
			t.s_s = t.entityelement[t.e].lua.returningaimain_s;
			t.entityelement[t.e].lua.returningaimainstored = 0;
		}
		else
		{
			// if call a return, but already at master, do nothing
			bAllowScriptToSwitch = false;
		}
	}
	if (bAllowScriptToSwitch == true)
	{
		t.entityelement[t.e].eleprof.aimain_s = t.s_s;
		lua_loadscriptin();

		// first try initialising with a name string
		t.strwork = ""; t.strwork = t.strwork + t.entityelement[t.e].eleprof.aimainname_s.Get() + "_init_name";
		LuaSetFunction (t.strwork.Get(), 2, 0);
		t.tentityname_s = t.entityelement[t.e].eleprof.name_s;
		LuaPushInt (t.e); LuaPushString (t.tentityname_s.Get());
		LuaCallSilent ();

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

		// then the actual _init call
		t.strwork = cstr(cstr(Lower(t.entityelement[t.e].eleprof.aimainname_s.Get())) + "_init");
		LuaSetFunction (t.strwork.Get(), 1, 0);
		LuaPushInt (t.e); LuaCallSilent ();

		//Check if we use properties variables.
		char tmp[MAX_PATH];
		strcpy(tmp, t.entityelement[t.e].eleprof.aimainname_s.Get());
		char* pFindSlash = strrchr(tmp, '\\');
		if (pFindSlash) strcpy(tmp, pFindSlash + 1);
		strcat(tmp, "_properties(");
		if (pestrcasestr(t.entityelement[t.e].eleprof.soundset4_s.Get(), tmp))
		{
			//Found one , parse and sent variables to script.
			lua_execute_properties_variable(t.entityelement[t.e].eleprof.soundset4_s.Get());
		}
	}
}

int g_iSuggestedSlot = 0;

void entity_lua_addplayerweapon(void)
{
	if (t.e > t.entityelement.size()) return; //PE: Got Crash Here.
	// collect this weapon
	t.tentid = t.entityelement[t.e].bankindex;
	t.weaponindex = t.entityprofile[t.tentid].isweapon;
	if (t.weaponindex != 0)
	{
		t.tqty = t.entityelement[t.e].eleprof.quantity;
		g_iSuggestedSlot = t.v;
		if (physics_player_addweapon() == true)
		{
			t.entityelement[t.e].eleprof.quantity = 0;// and keep the ammo elsewhere :)
		}
		g_iSuggestedSlot = 0;
	}
}

void entity_lua_changeplayerweapon(void)
{
	// what weapon
	t.findgun_s = t.s_s;
	gun_findweaponindexbyname();

	// before force this weapon, ensure if it does NOT exist, to create it first so can have ammo too
	int iHaveThis = 0;
	for (t.ws = 1; t.ws < 12; t.ws++)
	{
		if (t.weaponslot[t.ws].got == t.foundgunid)
		{
			iHaveThis = t.ws;
			break;
		}
	}
	if (iHaveThis == 0)
	{
		t.tqty = g.firemodes[t.foundgunid][0].settings.reloadqty;
		t.weaponindex = t.foundgunid;
		if (physics_player_addweapon() == true)
		{
			// weapon was added, with some ammo
		}
	}

	// force this weapon NAME to be selected
	g.autoloadgun = t.foundgunid; 
	gun_change();
}

void entity_lua_changeplayerweaponid(void)
{
	extern bool bForceGunUnderWater;
	if (bForceGunUnderWater)
		return;
	// if no gun, this will load in gun if it is not already in memory (normally all pre-loaded but standalonelevelreload mode can load a level with missing guns)
	// so this will be called from the GameLoopLoadStats global function for all weapons currently held by player at that point in game when start the fresh level
	int iWeaponID = t.v;

	// force this weapon ID to be selected
	g.autoloadgun = iWeaponID;
	gun_change();
}

void entity_lua_replaceplayerweapon ( void )
{
	//  replace this weapon with one currently held
	//  remove weapon from slot
	t.tswapslot=0;
	if (  t.gunid>0 ) 
	{
		//  find swap slot for old weapon (gunid)
		for ( t.ws = 1 ; t.ws < 12; t.ws++ )
		{
			if (  t.weaponslot[t.ws].pref == t.gunid  )  t.tswapslot = t.ws;
		}
		//  remove old weapon
		t.weaponindex=t.gunid ; physics_player_removeweapon ( );
	}
	//  what is new weapon
	t.tentid=t.entityelement[t.e].bankindex;
	t.weaponindex=t.entityprofile[t.tentid].isweapon;
	//  assign preference for new weapon
	if (  t.tswapslot>0 ) 
	{
		t.weaponslot[t.tswapslot].pref = t.weaponindex;
		if(t.tswapslot < 11)
			t.weaponammo[t.tswapslot] = 0; // reset so new weapon can work out its new ammo
	}
	//  now collect weapon (will find freed up slot from above)
	t.tqty=t.entityelement[t.e].eleprof.quantity;
	physics_player_addweapon ( );
}

void entity_lua_addplayerammo ( void )
{
	//  Collect Ammo for player
	t.tentid=t.entityelement[t.e].bankindex;
	t.tqty=t.entityelement[t.e].eleprof.quantity;
	t.tgunid=t.entityprofile[t.tentid].hasweapon;

	t.tfiremode=0;
	t.tpool=g.firemodes[t.tgunid][t.tfiremode].settings.poolindex;
	if (  t.tpool == 0 ) 
	{
		// the ammo is for a weapon that is missing from files, check to see if the ammo can be used for any other weapons.
		if (t.entityprofile[t.tentid].ammopool_s.Len() > 0)
		{
			for (int i = 0; i < t.ammopool.size(); i++)
			{
				if (stricmp(t.ammopool[i].name_s.Get(), t.entityprofile[t.tentid].ammopool_s.Get()) == 0)
				{
					g.firemodes[t.tgunid][t.tfiremode].settings.poolindex = i;
					t.tpool = i;
					break;
				}
			}
		}
	}
	if(t.tpool != 0)
	{
		// increase ammo pool by ammo quantity
		t.ammopool[t.tpool].ammo = t.ammopool[t.tpool].ammo + t.tqty;
		int iMaxClipCapacity = g.firemodes[t.tgunid][t.tfiremode].settings.clipcapacity * g.firemodes[t.tgunid][t.tfiremode].settings.reloadqty;
		if (iMaxClipCapacity == 0) iMaxClipCapacity = 99999;
		if (t.ammopool[t.tpool].ammo > iMaxClipCapacity) t.ammopool[t.tpool].ammo = iMaxClipCapacity;
	}
}

void entity_lua_addplayerhealth ( void )
{
	// collect health
	t.tqty = t.entityelement[t.e].eleprof.strength;

	//LB: new player health intercept
	LuaSetFunction ("PlayerHealthAdd", 1, 0);
	LuaPushInt(t.tqty);
	LuaCall();
	t.player[t.plrid].health = LuaGetInt("g_PlayerHealth");
}

void entity_lua_setplayerpower ( void )
{
	//  increase power of player (levelup/magic)
	t.player[t.plrid].powers.level=t.v;
return;

}

void entity_lua_addplayerpower ( void )
{
	//  increase power of player (levelup/magic)
	t.player[t.plrid].powers.level=t.player[t.plrid].powers.level+t.v;
return;

}

void entity_lua_addplayerjetpack ( void )
{
	// collect jet pack
	t.playercontrol.jetpackcollected = 1;
	if (  t.playercontrol.jetpackmode == 0  )  t.playercontrol.jetpackmode = 1;
	t.playercontrol.jetpackfuel_f=t.playercontrol.jetpackfuel_f+t.v;
	if (  t.playercontrol.jetobjtouse>0 ) 
	{
		if (  ObjectExist(t.playercontrol.jetobjtouse) == 1  )  HideObject (  t.playercontrol.jetobjtouse );
	}
	t.thudlayeritemindex=t.entityprofile[t.entityelement[t.e].bankindex].ishudlayer;
	t.playercontrol.jetobjtouse=t.hudlayerlist[t.thudlayeritemindex].obj;
	t.playercontrol.jetpackhidden=t.hudlayerlist[t.thudlayeritemindex].hidden;
	if (  t.playercontrol.thirdperson.enabled == 1 ) 
	{
		t.playercontrol.jetpackhidden=1;
	}
	if (  t.playercontrol.jetobjtouse>0 ) 
	{
		if (  ObjectExist(t.playercontrol.jetobjtouse) == 1  )  ShowObject (  t.playercontrol.jetobjtouse );
	}
}

void entity_lua_set_light_visible ( void )
{
	//  receives e and v
	t.entityelement[t.e].eleprof.light.islit=t.v;
	for ( t.l = 1 ; t.l<=  g.infinilightmax; t.l++ )
	{
		if (  t.infinilight[t.l].used == 1 ) 
		{
			if (  t.infinilight[t.l].e == t.e ) 
			{
				t.infinilight[t.l].islit=t.v;
			}
		}
	}
}
