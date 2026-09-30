//----------------------------------------------------
//--- GAMEGURU - M-Physics
//----------------------------------------------------

#include "stdafx.h"
#include "gameguru.h"

#include "..\..\GameGuru\Imgui\imgui.h"
#include "..\..\GameGuru\Imgui\imgui_impl_win32.h"
#include "..\..\GameGuru\Imgui\imgui_gg_dx11.h"

#include "GGTerrain\GGTerrain.h"
#include "GGTerrain\GGTrees.h"
using namespace GGTerrain;
using namespace GGTrees;
bool g_bSpecialPhysicsDebuggingMode = false;
GGVECTOR3 g_vPlayAreaMin = GGVECTOR3(0, 0, 0);
GGVECTOR3 g_vPlayAreaMax = GGVECTOR3(0, 0, 0);
bool g_bModifiedThisTerrainGrid[21][21];
int g_iLastProgressPercentage = -1;
bool g_bMapMatIDToMatIndexAvailable = false;
int g_iMapMatIDToMatIndex[32];
int g_iSuccessfullyBlockedAtTime = 0;
bool g_bSpawningThisOneNow = false;

#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

// 
//  Physics Subroutines and Functions
// 

// 
//  PHYSICS CODE
// 

void physics_inittweakables ( void )
{
	//  Editable in Player Start Marker
	t.playercontrol.regenrate=0;
	t.playercontrol.regenspeed=100;
	t.playercontrol.regendelay=3000;
	t.playercontrol.regentime=0;
	t.playercontrol.jumpmax_f=215.0;
	t.playercontrol.gravity_f=900.0;
	t.playercontrol.fallspeed_f=5000.0;
	t.playercontrol.climbangle_f = 66.0f; //LB: messed with ability to climbs stairs, and small pallets on floor... 50.0f; //LB: was 70.0;
	t.playercontrol.footfallpace_f=3.0;
	t.playercontrol.wobblespeed_f=460.0;
	t.playercontrol.wobbleheight_f=1.5;
	t.playercontrol.accel_f=25.0;

	//  third person defaults
	t.playercontrol.thirdperson.enabled=0;
	t.playercontrol.thirdperson.charactere=0;
	t.playercontrol.thirdperson.startmarkere=0;
	t.playercontrol.thirdperson.cameralocked=0;
	t.playercontrol.thirdperson.cameradistance=200.0;
	t.playercontrol.thirdperson.cameraheight=100.0;
	t.playercontrol.thirdperson.camerafocus=5.0;
	t.playercontrol.thirdperson.cameraspeed=50.0;
	t.playercontrol.thirdperson.camerashoulder=6.0;
	t.playercontrol.thirdperson.camerafollow=1;
	t.playercontrol.thirdperson.camerareticle=1;
}

void physics_loadmaterialsoundsintomapmat ( LPSTR pOptionalMaterialSoundsFile )
{
	if (FileExist(pOptionalMaterialSoundsFile) == 1)
	{
		OpenToRead(1, pOptionalMaterialSoundsFile);
		while (FileEnd(1) == 0)
		{
			LPSTR pLine = ReadString(1);
			if (pLine != NULL)
			{
				if (pLine[0] != ';' && strlen(pLine) < 30)
				{
					char pNums[MAX_PATH];
					memset(pNums, 0, sizeof(pNums));
					if (strlen(pLine) > 3)
					{
						strcpy_s(pNums, MAX_PATH, pLine + 3); // skip 'mat'
					}
					LPSTR pEqual = strstr(pNums, "=");
					if (pEqual)
					{
						char pMaterialIndex[32];
						strcpy(pMaterialIndex, pEqual + 1);
						*pEqual = 0;
						char pTerrainMatID[32];
						strcpy(pTerrainMatID, pNums);
						int iMaterialIndex = atoi(pMaterialIndex);
						int iMatID = atoi(pTerrainMatID);
						if (iMatID >= 1 && iMatID <= 32)
						{
							g_iMapMatIDToMatIndex[iMatID - 1] = iMaterialIndex;
						}
					}
				}
			}
		}
		CloseFile(1);
	}
}

void physics_copymatmaptocustommat (void)
{
	extern int g_iCustomTerrainMatSounds[32];
	for (int i = 0; i < 32; i++)
	{
		g_iCustomTerrainMatSounds[i] = g_iMapMatIDToMatIndex[i];
	}
}

void physics_init ( void )
{
	// create material ID to material sound mapping
	if (g_bMapMatIDToMatIndexAvailable == false)
	{
		g_bMapMatIDToMatIndexAvailable = true;
		if (t.visuals.customTexturesFolder.Len() > 0)
		{
			// Custom terrain materials are determined by user, not matsounds.txt
			extern int g_iCustomTerrainMatSounds[32];
			for (int i = 0; i < 32; i++)
			{
				g_iMapMatIDToMatIndex[i] = g_iCustomTerrainMatSounds[i];
			}
		}
		else
		{
			LPSTR pMatConvertTableFile = "terraintextures\\matsounds.txt";
			physics_loadmaterialsoundsintomapmat (pMatConvertTableFile);
		}
	}

	//  Player Control
	t.playercontrol.wobble_f=0.0;
	t.playercontrol.floory_f=0.0;
	t.playercontrol.topspeed_f=0.75;
	t.playercontrol.footfalltype=0;
	t.playercontrol.footfallcount=0;
	if (  t.playercontrol.regenrate>0 ) 
	{
		t.playercontrol.regentime=Timer();
		t.playercontrol.regentick=Timer();
	}
	else
	{
		t.playercontrol.regentime=0;
		t.playercontrol.regentick=0;
	}

	//  Reset jetpack
	t.playercontrol.jetpackmode=0;
	t.playercontrol.jetpackthrust_f=0.0;
	t.playercontrol.jetpackfuel_f=0;

	//  Player misc settings
	t.playercontrol.disablemusicreset=0;

	//  Reset gun collected count
	t.guncollectedcount=0;

	//  Init physics system
	ODEStart (   ); g.gphysicssessionactive=1;

	//  Set starting water Line (  )
	terrain_updatewaterphysics ( );

	//  Create terrain collision
	t.tgenerateterraindirtyregiononly=0;
	timestampactivity(0,"_physics_createterraincollision");
	physics_createterraincollision ( );
	t.tgenerateterraindirtyregiononly=0;

	// MAX also has virtual trees that need physics
	physics_createvirtualtreecylinders();

	//  Player Controller Object
	if (  ObjectExist(t.aisystem.objectstartindex) == 0 ) 
	{
		//  normally created by AI which precedes Physics initialisations
		MakeObjectCube (  t.aisystem.objectstartindex,10 );
	}
	HideObject (  t.aisystem.objectstartindex );

	//  set default player gravity
	t.playercontrol.gravityactive=1;
	t.playercontrol.gravityactivepress=0;
	t.playercontrol.lockatheight=0;
	t.playercontrol.lockatheightpress=0;

	//  Setup entity physics
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.entid=t.entityelement[t.e].bankindex;
		physics_prepareentityforphysics ( );
	}

	//  Introduce all characters and entities to the physics universe
	timestampactivity(0,"Introduce all characters");
	for ( g.charanimindex = 1 ; g.charanimindex <= g.charanimindexmax; g.charanimindex++ )
	{
		// get physics object for this character
		t.tphyobj=t.charanimstates[g.charanimindex].obj;

		// 111115 - but exclude any third person character
		if ( t.playercontrol.thirdperson.enabled == 1 && t.playercontrol.thirdperson.characterindex == g.charanimindex ) 
			t.tphyobj = 0;

		// if object still requires Y adjustment check
		if ( t.tphyobj>0 ) 
		{
			t.e = t.charanimstates[g.charanimindex].e;
			bool bCharacterUsesPhysics = false;
			t.entityelement[t.e].usingphysicsnow = 0;
			if (t.entityprofile[t.entityelement[t.e].bankindex].physics != 0) bCharacterUsesPhysics = true;
			if (bCharacterUsesPhysics == true)
			{
				// get entity index associated with character
				t.tcollisionscaling = t.entityprofile[t.entityelement[t.e].bankindex].collisionscaling;
				t.tcollisionscalingxz = t.entityprofile[t.entityelement[t.e].bankindex].collisionscalingxz;
				physics_setupcharacter ();
			}
		}
	}

	// Ensure the LUA mouse is always reset
	lua_deactivatemouse();

	// player physics setup closer to main loop
	t.freezeplayerposonly = 0;
	physics_setupplayer ( );
}

void physics_finalize ( void )
{
	ODEFinalizeWorld();
}

// one way flood fill to reduce terrain polys
struct sLargeQuadListItem
{
	int iX1;
	int iZ1;
	int iX2;
	int iZ2;
};
//int g_iQuadGrid[201][201];
int g_iQuadGrid[401][401];
std::vector<sLargeQuadListItem> g_LargeQuadList;
std::vector<sLargeQuadListItem> g_RefinedLargeQuadList;
void physics_processheightsusingfloodfill (int iX, int iZ, int iGridAtX, int iGridAtZ, float fOneTileSize, int iFullResolutionSize, int iResolutionSIze, int iHeightDataSize, float* pfHeightData)
{
	// stage 0 - we mark this as used right now so no overlapping behaviors occur
	int iUniqueTileID = 1 + g_LargeQuadList.size();
	g_iQuadGrid[iX][iZ] = iUniqueTileID;

	// the right/bottom edge needs full resolution (to join seamlessly with next area)
	int iX1 = iX;
	int iZ1 = iZ;
	int iX2 = iX;
	int iZ2 = iZ;
	if (1)//iX < iResolutionSIze && iZ < iResolutionSIze)
	{
		// stage 1 - create plane from this quad (iX,iZ)
		GGPLANE thisPlane;
		int iRealX = (iGridAtX*iResolutionSIze);
		int iRealZ = (iGridAtZ*iResolutionSIze);
		int iGetHeightX = (iRealX + iX);
		int iGetHeightZ = (iRealZ + iZ);
		float fHeight = 0.0f;
		int iOffset = (iGetHeightZ*iFullResolutionSize) + iGetHeightX;
		if (iOffset < iHeightDataSize) fHeight = pfHeightData[iOffset];
		GGVECTOR3 vecPos0 = GGVECTOR3(iX * fOneTileSize, fHeight, iZ * fOneTileSize);
		iGetHeightX = (iRealX + iX + 1);
		iGetHeightZ = (iRealZ + iZ);
		iOffset = (iGetHeightZ*iFullResolutionSize) + iGetHeightX;
		if (iOffset < iHeightDataSize) fHeight = pfHeightData[iOffset];
		GGVECTOR3 vecPos1 = GGVECTOR3((iX + 1) * fOneTileSize, fHeight, iZ * fOneTileSize);
		iGetHeightX = (iRealX + iX);
		iGetHeightZ = (iRealZ + iZ + 1);
		iOffset = (iGetHeightZ*iFullResolutionSize) + iGetHeightX;
		if (iOffset < iHeightDataSize) fHeight = pfHeightData[iOffset];
		GGVECTOR3 vecPos2 = GGVECTOR3(iX * fOneTileSize, fHeight, (iZ + 1) * fOneTileSize);
		GGPlaneFromPoints(&thisPlane, &vecPos0, &vecPos1, &vecPos2);

		// stage 2 - expand quad in both axis while within threshold
		bool bExpanding = true;
		float fThreshold = 2.0f;
		int iTryX = iX;
		int iTryZ = iZ;
		bool bCanExpandX = true;
		bool bCanExpandZ = true;
		while (bExpanding == true)
		{
			// expand in directions where g_iQuadGrid is free
			if (bCanExpandX == true)
			{
				iTryX++;
				for (int iAllTheZs = iZ1; iAllTheZs <= iZ2; iAllTheZs++)
				{
					if (g_iQuadGrid[iTryX][iAllTheZs] == 0)
					{
						iGetHeightX = (iRealX + iTryX);
						iGetHeightZ = (iRealZ + iAllTheZs);
						fHeight = 0.0f;
						iOffset = (iGetHeightZ*iFullResolutionSize) + iGetHeightX;
						if (iOffset < iHeightDataSize) fHeight = pfHeightData[iOffset];
						GGVECTOR3 vecTryPos = GGVECTOR3(iTryX * fOneTileSize, fHeight, iAllTheZs * fOneTileSize);
						float fDeviationFromPlane = GGPlaneDotCoord(&thisPlane, &vecTryPos);
						if (fabs(fDeviationFromPlane) > fThreshold)
						{
							// this position cannot be added to the current plane/largequad
							bCanExpandX = false;
						}
					}
					else
					{
						// hit a used tile, also cannot expand in this direction any more
						bCanExpandX = false;
					}
				}
				if (bCanExpandX == true)
				{
					// this is fine, keep going in the X direction
					iX2 = iTryX;
					for (int z = iZ1; z <= iZ2; z++)
						for (int x = iX1; x <= iX2; x++)
							g_iQuadGrid[x][z] = iUniqueTileID;
				}
				if (iTryX == iResolutionSIze) bCanExpandX = false;
			}
			if (bCanExpandZ == true)
			{
				iTryZ++;
				for (int iAllTheXs = iX1; iAllTheXs <= iX2; iAllTheXs++)
				{
					if (g_iQuadGrid[iAllTheXs][iTryZ] == 0)
					{
						iGetHeightX = (iRealX + iAllTheXs);
						iGetHeightZ = (iRealZ + iTryZ);
						fHeight = 0.0f;
						iOffset = (iGetHeightZ*iFullResolutionSize) + iGetHeightX;
						if (iOffset < iHeightDataSize) fHeight = pfHeightData[iOffset];
						GGVECTOR3 vecTryPos = GGVECTOR3(iAllTheXs * fOneTileSize, fHeight, iTryZ * fOneTileSize);
						float fDeviationFromPlane = GGPlaneDotCoord(&thisPlane, &vecTryPos);
						if (fabs(fDeviationFromPlane) > fThreshold)
						{
							// this position cannot be added to the current plane/largequad
							bCanExpandZ = false;
						}
					}
					else
					{
						// hit a used tile, also cannot expand in this direction any more
						bCanExpandZ = false;
					}
				}
				if (bCanExpandZ == true)
				{
					// this is fine, keep going in the Z direction
					iZ2 = iTryZ;
					for (int z = iZ1; z <= iZ2; z++)
						for (int x = iX1; x <= iX2; x++)
							g_iQuadGrid[x][z] = iUniqueTileID;
				}
				if (iTryZ == iResolutionSIze) bCanExpandZ = false;
			}

			// if threshold exceeded in both directions, cannot expand any more
			if (bCanExpandX == false && bCanExpandZ == false)
				bExpanding = false;
		}
	}

	// stage 3 - create a 'larger' quad for this expanded area
	sLargeQuadListItem item;
	item.iX1 = iX1;
	item.iZ1 = iZ1;
	item.iX2 = iX2;
	item.iZ2 = iZ2;
	g_LargeQuadList.push_back(item);
}

void physics_createterraincollision ( void )
{
	// we use the 'play area' to reduce 'test level' load time (concentrating only on the area you are using in the level)
	timestampactivity(0, "detect and define play area");
	// calculate bounds of static features
	static float fLastPlayerStartX = -1.0f;
	static float fLastPlayerStartZ = -1.0f;
	float fPlayerStartX = CameraPositionX(0);
	float fPlayerStartZ = CameraPositionZ(0);
	GGVECTOR3 vecMin, vecMax;
	vecMin = GGVECTOR3(fPlayerStartX, 0, fPlayerStartZ);
	vecMax = GGVECTOR3(fPlayerStartX, 0, fPlayerStartZ);
	vecMin -= GGVECTOR3(-500, 0, -500);
	vecMax += GGVECTOR3(500, 0, 500);
	int iObjectCount = 0;
	for (int e = 1; e <= g.entityelementlist; e++)
	{
		if ( 1 ) // must include start marker and all dynamic objects (everything) for full coverage of level
		{
			int iObj = t.entityelement[e].obj;
			if (iObj > 0)
			{
				if (ObjectExist(iObj) == 1)
				{
					if (vecMin.x > ObjectPositionX(iObj)) vecMin.x = ObjectPositionX(iObj);
					if (vecMin.y > ObjectPositionY(iObj)) vecMin.y = ObjectPositionY(iObj);
					if (vecMin.z > ObjectPositionZ(iObj)) vecMin.z = ObjectPositionZ(iObj);
					if (vecMax.x < ObjectPositionX(iObj)) vecMax.x = ObjectPositionX(iObj);
					if (vecMax.y < ObjectPositionY(iObj)) vecMax.y = ObjectPositionY(iObj);
					if (vecMax.z < ObjectPositionZ(iObj)) vecMax.z = ObjectPositionZ(iObj);
					iObjectCount++;
				}
			}
		}
		int entid = t.entityelement[e].bankindex;
		if (entid > 0)
		{
			if (t.entityprofile[entid].ismarker == 1)
			{
				// player start marker
				fPlayerStartX = t.entityelement[e].x;
				fPlayerStartZ = t.entityelement[e].z;
			}
		}
	}
	float fEditableSizeHalved = GGTerrain_GetEditableSize();
	t.terraineditableareasizeminx = -fEditableSizeHalved;
	t.terraineditableareasizeminz = -fEditableSizeHalved;
	t.terraineditableareasizemaxx = fEditableSizeHalved;
	t.terraineditableareasizemaxz = fEditableSizeHalved;
	bool bAnythingHasMoved = false;
	if (fLastPlayerStartX != fPlayerStartX || fLastPlayerStartZ != fPlayerStartZ ) bAnythingHasMoved = true;
	fLastPlayerStartX = fPlayerStartX;
	fLastPlayerStartZ = fPlayerStartZ;
	g_vPlayAreaMin = vecMin - GGVECTOR3(1000, 0, 1000);
	g_vPlayAreaMax = vecMax + GGVECTOR3(1000, 0, 1000);
	if (g_vPlayAreaMin.x < -50000) g_vPlayAreaMin.x = -50000;
	if (g_vPlayAreaMin.x > 50000)  g_vPlayAreaMin.x = 50000;
	if (g_vPlayAreaMin.z < -50000) g_vPlayAreaMin.z = -50000;
	if (g_vPlayAreaMin.z > 50000)  g_vPlayAreaMin.z = 50000;
	if (g_vPlayAreaMax.x < -50000) g_vPlayAreaMax.x = -50000;
	if (g_vPlayAreaMax.x > 50000)  g_vPlayAreaMax.x = 50000;
	if (g_vPlayAreaMax.z < -50000) g_vPlayAreaMax.z = -50000;
	if (g_vPlayAreaMax.z > 50000)  g_vPlayAreaMax.z = 50000;

	// above values are used by navmesh
	if (t.visuals.bEnableEmptyLevelMode == false)
	{
		// only bother creating terrain physics if we are not in 'empty level mode'
		ODECreateGGTerrain();
	}

	// and we are done
	return;
}

struct sVTreeObj
{
	int iID;
	float fX;
	float fY;
	float fZ;
	bool bActive;
};
std::vector<sVTreeObj> g_VTreeObj;

void physics_createvirtualtreecylinders (void)
{
	// going to use GGTrees_GetClosest to position kenetic cylinders so player cannot walk through virtual trees
	if (ObjectExist(g.virtualtreeobjectstart) == 0)
	{
		// create once for rest of software lifecycle (Wicked slows when create/delete objects)
		for (int iVT = 0; iVT < PHYSICS_VIRTUALTREE_MAX; iVT++)
		{
			// create cylinder for virtual tree
			int iPhyObjID = g.virtualtreeobjectstart + iVT;
			MakeObjectSphere(iPhyObjID, 9);
			HideObject(iPhyObjID);
			sObject* pObject = GetObjectData(iPhyObjID);
			if (pObject)
			{
				WickedCall_SetObjectCastShadows(pObject, false);
			}
		}
	}

	// start new Vtrees list
	g_VTreeObj.clear();
}

void physics_freevirtualtreecylinders (void)
{
	// just hide, we will need them next time
	for (int iVT = 0; iVT < PHYSICS_VIRTUALTREE_MAX; iVT++)
	{
		// create cylinder for virtual tree
		int iPhyObjID = g.virtualtreeobjectstart + iVT;
		if (ObjectExist(iPhyObjID) == 1)
		{
			ODEDestroyObject(iPhyObjID);
			HideObject(iPhyObjID);
		}
	}
}

void physics_managevirtualtreecylinders (void)
{
	// first start assuming all existing visible trees may not show again
	for (int vti = 0; vti < g_VTreeObj.size(); vti++)
		g_VTreeObj[vti].bActive = false;

	// scan for virtual trees
	if (ggtrees_global_params.draw_enabled == 1)
	{
		float fRadiusOfScan = 200.0f;
		GGTrees::GGTreePoint* pOutPoints = NULL;
		int iTreeCount = GGTrees::GGTrees_GetClosest (CameraPositionX(0), CameraPositionZ(0), fRadiusOfScan, &pOutPoints);
		if (pOutPoints)
		{
			for (int n = 0; n < iTreeCount; n++)
			{
				// this virtual tree pos
				GGVECTOR3 vecTreePos = GGVECTOR3(pOutPoints[n].x, pOutPoints[n].y, pOutPoints[n].z);
				float fTreeThickness = pOutPoints[n].scale;

				// see if tree in the visible list
				int vti = 0;
				for (; vti < g_VTreeObj.size(); vti++)
				{
					if (g_VTreeObj[vti].iID > 0 && g_VTreeObj[vti].fX == vecTreePos.x && g_VTreeObj[vti].fY == vecTreePos.y && g_VTreeObj[vti].fZ == vecTreePos.z)
						break;
				}
				if (vti < g_VTreeObj.size())
				{
					// yes we still know about this tree
					g_VTreeObj[vti].bActive = true;
				}
				else
				{
					// ah, new tree, add to list
					// first see if we have a free slot in exsiting list
					int vti = 0;
					for (; vti < g_VTreeObj.size(); vti++)
					{
						if (g_VTreeObj[vti].bActive == false)
							break;
					}
					if (vti < g_VTreeObj.size())
					{
						// use existing entry in list
					}
					else
					{
						// new list entry
						if (g_VTreeObj.size() < PHYSICS_VIRTUALTREE_MAX)
						{
							sVTreeObj newtree;
							g_VTreeObj.push_back(newtree);
							vti = g_VTreeObj.size() - 1;
						}
						else
						{
							// no room to add to visible list
							vti = -1;
						}
					}
					// entry details
					if (vti != -1)
					{
						int iPhyObjID = g.virtualtreeobjectstart + vti;
						g_VTreeObj[vti].iID = iPhyObjID;
						g_VTreeObj[vti].fX = vecTreePos.x;
						g_VTreeObj[vti].fY = vecTreePos.y;
						g_VTreeObj[vti].fZ = vecTreePos.z;
						g_VTreeObj[vti].bActive = true;

						// delete any old one
						ODEDestroyObject(iPhyObjID);

						// position dynamically
						PositionObject(iPhyObjID, vecTreePos.x, vecTreePos.y, vecTreePos.z);

						// ShowObject(iPhyObjID); //Debug only.
						HideObject(iPhyObjID);

						// now create our physics object afresh
						SetObjectArbitaryValue (iPhyObjID, 3);// 6); wood, not flesh!

						// must cater for largest thickest trees in the default biome set (scots pine dead)
						//ODECreateStaticCylinder (iPhyObjID, vecTreePos.x, vecTreePos.y, vecTreePos.z, 10, 500, 10, 0, 0, 0);
						ODECreateStaticCylinder (iPhyObjID, vecTreePos.x, vecTreePos.y, vecTreePos.z, fTreeThickness, 500, fTreeThickness, 0, 0, 0);
					}
				}
			}
			delete[] pOutPoints;
		}
		// finally remove any from list which have become inactive
		for (int vti = 0; vti < g_VTreeObj.size(); vti++)
		{
			if (g_VTreeObj[vti].bActive == false)
			{
				if (g_VTreeObj[vti].iID > 0)
				{
					int iPhyObjID = g.virtualtreeobjectstart + vti;
					if (ObjectExist(iPhyObjID) == 1)
					{
						ODEDestroyObject(iPhyObjID);
						HideObject(iPhyObjID);
					}
					g_VTreeObj[vti].iID = 0;
				}
			}
		}
	}
}

void physics_prepareentityforphysics ( void )
{
	//  takes E and ENTID
	t.tphyobj=t.entityelement[t.e].obj;
	if (  t.entid>0 && t.tphyobj>0 ) 
	{
		// Entity has a different collision mode to the parent object in the FPE file...
		int iStoreEntityIndex = t.entid;
		int iStoreOriginalCollisionMode = t.entityprofile[iStoreEntityIndex].collisionmode;
		if (t.e < t.entityelement.size() && t.entityelement[t.e].eleprof.iOverrideCollisionMode != -1)
		{
			t.entityprofile[t.entid].collisionmode = t.entityelement[t.e].eleprof.iOverrideCollisionMode;
		}

		// special hybrid collision mode can hide static limbs of primary object
		if (t.entityprofile[t.entid].collisionmode == 31)
		{
			// create a secondary object to show the static limbs (and hide the non static)
			if (t.entityelement[t.e].attachmentobj == 0)
			{
				int iSecondaryObjID = g.physicssecondariesoffset;
				while (ObjectExist(iSecondaryObjID) == 1 && iSecondaryObjID < g.physicssecondariesoffsetend) iSecondaryObjID++;
				if (ObjectExist(iSecondaryObjID) == 1) DeleteObject (iSecondaryObjID);
				CloneObject(iSecondaryObjID, t.tphyobj);
				PositionObject(iSecondaryObjID, ObjectPositionX(t.tphyobj), ObjectPositionY(t.tphyobj), ObjectPositionZ(t.tphyobj));
				SetObjectToObjectOrientation(iSecondaryObjID, t.tphyobj);
				sObject* pSecondaryObject = GetObjectData(iSecondaryObjID);
				WickedCall_TextureObject(pSecondaryObject, NULL);
				t.entityelement[t.e].attachmentobj = iSecondaryObjID;
				// ensure it does not attract a collision hit during ray cast
				WickedCall_SetObjectRenderLayer(pSecondaryObject, GGRENDERLAYERS_CURSOROBJECT);
				PerformCheckListForLimbs(iSecondaryObjID);
				for (int c = ChecklistQuantity(); c > 1; c += -1)
				{
					t.tname_s = Lower(ChecklistString(c));
					LPSTR pNamePtr = t.tname_s.Get();
					if (strlen(pNamePtr) > 7)
					{
						if (strnicmp(pNamePtr + strlen(pNamePtr) - 7, "_static", 7) != NULL)
						{
							HideLimb(iSecondaryObjID, c - 1);
						}
					}
				}
			}
			
			// hide the static limbs from the primary
			PerformCheckListForLimbs(t.tphyobj);
			for ( int c = ChecklistQuantity(); c >= 1; c += -1)
			{
				t.tname_s = Lower(ChecklistString(c));
				LPSTR pNamePtr = t.tname_s.Get();
				if (strlen(pNamePtr)>7)
				{
					if (strnicmp(pNamePtr+strlen(pNamePtr)-7,"_static",7)==NULL )
					{
						HideLimb(t.tphyobj, c-1);
					}
				}
			}
		}

		// now use collisionindex
		t.tnophysics=0;
		if (  t.entityprofile[t.entid].ismarker != 0  )  t.tnophysics = 1;
		if (  t.entityprofile[t.entid].collisionmode == 11  )  t.tnophysics = 1;
		if (  t.entityprofile[t.entid].collisionmode == 12  )  t.tnophysics = 1;
		if (  t.entityelement[t.e].eleprof.physics == 0  )  t.tnophysics = 1;
		if (  t.entityelement[t.e].eleprof.physics == 2  )  t.tnophysics = 1;
		if (  t.entityprofile[t.entid].isammo == 1  )  t.tnophysics = 1;
		if (t.entityelement[t.e].eleprof.iOverrideCollisionMode == -1)
		{
			// special case where weapon drops can be assigned a physics shapoe when dropped
			if (Len(t.entityprofile[t.entid].isweapon_s.Get()) > 1)  t.tnophysics = 1;
		}
		if (  t.tnophysics == 1 ) 
		{
			//  no physics
		}
		else
		{
			if (  t.entityprofile[t.entid].isebe != 0 ) 
			{
				// EBE structure from cubes
				physics_setupebestructure ( );
				t.entityelement[t.e].usingphysicsnow=1;
			}
			else
			{
				if ( t.entityprofile[t.entid].ischaracter == 1 && t.entityelement[t.e].eleprof.isimmobile == 0 )
				{
					//  physics objects belong to Ghost AI Objects (set outside of this function, i.e. LUA-Entity.cpp)
					// except when you are spawning them in game and want physics sorted
					if (g_bSpawningThisOneNow == true)
					{
						// you shall go to the ball, cinders!
						t.tcollisionscaling = t.entityprofile[t.entid].collisionscaling;
						t.tcollisionscalingxz = t.entityprofile[t.entid].collisionscalingxz;
						physics_setupcharacter ();
					}
				}
				else
				{
					// static or dynamic
					t.tstatic=t.entityelement[t.e].staticflag;
					if (  t.entityelement[t.e].eleprof.isimmobile == 1  )  t.tstatic = 1;

					// physics object is faux-character capsule (zombies, custom characters)
					if ( t.entityprofile[t.entid].collisionmode == 21 || t.entityprofile[t.entid].collisionmode == 22 )
					{
						// create capsule (to be controlled as entity-driven, i.e. MoveForward)
						t.tcollisionscaling = t.entityprofile[t.entid].collisionscaling;
						t.tcollisionscalingxz = t.entityprofile[t.entid].collisionscalingxz;
						physics_setupcharacter ( );
					}
					else
					{
						// create solid entities
						if (t.entityprofile[t.entid].collisionmode == 1)
						{
							t.tshape = 2;
						}
						else if (t.entityprofile[t.entid].collisionmode == 8)
						{
							t.tshape = 8; // new version of '2' but detects for OBJ
						}
						else if (t.entityprofile[t.entid].collisionmode == 9)
						{
							t.tshape = 9;
						}
						else if (t.entityprofile[t.entid].collisionmode == 10)
						{
							t.tshape = 10;
						}
						else if (t.entityprofile[t.entid].collisionmode == 2)
						{
							t.tshape = 6;
						}
						else if (t.entityprofile[t.entid].collisionmode == 3)
						{
							t.tshape = 7;
						}
						else
						{
							if (  t.entityprofile[t.entid].collisionmode >= 1000 ) 
							{
								t.tshape=t.entityprofile[t.entid].collisionmode;
							}
							else
							{
								if (  t.entityprofile[t.entid].collisionmode >= 50 && t.entityprofile[t.entid].collisionmode<60 ) 
								{
									t.tshape=3;
									t.tstatic=1;
								}
								else
								{
									t.tshape=1;
								}
							}
						}
						//  check if it has a list of physics objects from the importer, collisionmode 40
						if (  t.entityprofile[t.entid].physicsobjectcount > 0 && t.entityprofile[t.entid].collisionmode  ==  40 ) 
						{
							t.tshape = 4;
						}
						t.tweight=t.entityelement[t.e].eleprof.phyweight;
						t.tfriction=t.entityelement[t.e].eleprof.phyfriction;
						t.tcollisionscaling=t.entityprofile[t.entid].collisionscaling;
						t.tcollisionscalingxz = t.entityprofile[t.entid].collisionscalingxz;
						physics_setupobject ( );
						t.entityelement[t.e].usingphysicsnow=1;
					}
				}
			}
		}

		// ...and restore parent when done
		t.entityprofile[iStoreEntityIndex].collisionmode = iStoreOriginalCollisionMode;
	}
}

void physics_setupplayernoreset ( void )
{
	//  create character controller for player
	PositionObject (  t.aisystem.objectstartindex,t.terrain.playerx_f,t.terrain.playery_f,t.terrain.playerz_f );
	if ( t.freezeplayerposonly==0 ) RotateObject (  t.aisystem.objectstartindex,t.terrain.playerax_f,t.terrain.playeray_f,t.terrain.playeraz_f );
	SetObjectArbitaryValue (  t.aisystem.objectstartindex, 6 ); // 6-flesh

	// allow SetWorldGravity to decide players gravity value (and prevents slow drop when holding onto ladder and constantly calling this function)
	ODECreateDynamicCharacterController (  t.aisystem.objectstartindex,t.playercontrol.gravity_f,t.playercontrol.fallspeed_f,t.playercontrol.climbangle_f );
}

void physics_setupplayer ( void )
{
	physics_setupplayernoreset ( );
	if ( g.luacameraoverride != 2 && g.luacameraoverride != 3 )
	{
		if ( t.freezeplayerposonly==0 ) RotateCamera (  0,t.terrain.playerax_f,t.terrain.playeray_f,t.terrain.playeraz_f );
	}
}

void physics_disableplayer ( void )
{
	ODEDestroyObject (  t.aisystem.objectstartindex );
}

void physics_setupcharacter ( void )
{
	// only if physics is active for this character
	if (t.entityprofile[t.entityelement[t.e].bankindex].physics != 0)
	{
		// create physics for this character/faux-character object
		SetObjectArbitaryValue (t.tphyobj, 6); // 6-flesh
		if (t.entityelement[t.e].eleprof.isimmobile == 0)
		{
			// 190718 - remove t.terrain.adjaboveground_f from enemy terrain relative positioning
			// ensure CHARACTER do not spawn UNDER the terrain
			t.tgroundheight_f = BT_GetGroundHeight(t.terrain.TerrainID, ObjectPositionX(t.tphyobj), ObjectPositionZ(t.tphyobj));
			t.tgroundheight_f = t.tgroundheight_f + 2.5;
			// 291116 - account for object vecCenter (so characters with Y=0=Floor are not unjustly raised)
			// NOW DONE EARLIER SO NO NEED TO ADJUST FOR COL CENTER TWICE (see calling function)
			//float fAccountForVecCenter = GetObjectCollisionCenterY ( t.tphyobj );
			if (ObjectPositionY(t.tphyobj) <= t.tgroundheight_f)
			{
				PositionObject (t.tphyobj, ObjectPositionX(t.tphyobj), t.tgroundheight_f, ObjectPositionZ(t.tphyobj));
			}
			else
			{
				PositionObject (t.tphyobj, ObjectPositionX(t.tphyobj), ObjectPositionY(t.tphyobj), ObjectPositionZ(t.tphyobj));
			}
			t.tfinalscale_f = g.gcharactercapsulescale_f * ((t.entityprofile[t.entityelement[t.e].bankindex].scale + 0.0f) / 100.0f);
			float fWeight = t.entityelement[t.e].eleprof.phyweight;
			float fFriction = t.entityelement[t.e].eleprof.phyfriction;
			ODECreateDynamicCapsule (t.tphyobj, t.tfinalscale_f, 0.0, fWeight, fFriction, (float)t.tcollisionscaling / 100.0f, (float)t.tcollisionscalingxz / 100.0f);
		}
		else
		{
			// 290515 - fixes scifi DLC characters floating (ISIMMOBILE=1) PositionObject ( t.tphyobj,ObjectPositionX(t.tphyobj),ObjectPositionY(t.tphyobj)+(ObjectSizeY(t.tphyobj,1)/2),ObjectPositionZ(t.tphyobj) );
			PositionObject (t.tphyobj, ObjectPositionX(t.tphyobj), ObjectPositionY(t.tphyobj), ObjectPositionZ(t.tphyobj));
			ODECreateStaticCapsule (t.tphyobj, (float)t.tcollisionscaling / 100.0f, (float)t.tcollisionscalingxz / 100.0f);
		}
		t.entityelement[t.e].usingphysicsnow = 1;
	}
	else
	{
		t.entityelement[t.e].usingphysicsnow = 0;
	}
}

void physics_setupebestructure ( void )
{
	//  create EBE physics for this object. Takes tphyobj and entid and e
	if ( t.tphyobj>0 ) 
	{
		if ( ObjectExist(t.tphyobj) == 1 ) 
		{
			ebe_physics_setupebestructure ( t.tphyobj, t.e );
		}
	}
}

void physics_setupobject ( void )
{
	// default is no special handling of OBJ collision meshes
	ODESetOBJLoadingFilename("");

	// create physics for this object. Takes tphyobj and entid and e
	if ( t.tphyobj>0 ) 
	{
		if ( ObjectExist(t.tphyobj) == 1 ) 
		{
			// hull decomp take time - offer ability to save and reload
			if (t.tshape == 10)
			{
				char pNoFPE[MAX_PATH];
				strcpy(pNoFPE, t.entitybank_s[t.entid].Get());
				pNoFPE[strlen(pNoFPE) - 4] = 0;
				char pObjectFilename[MAX_PATH];
				sprintf(pObjectFilename, "%s\\Files\\entitybank\\%s.bullet", g.fpscrootdir_s.Get(), pNoFPE);
				ODESetMeshFilename(pObjectFilename);
				GG_GetRealPath(pObjectFilename, 0);
				if (FileExist(pObjectFilename) == 0)
				{
					// use only the name
					char pJustName[MAX_PATH];
					strcpy(pJustName, "");
					for (int n = strlen(pNoFPE) - 1; n > 0; n--)
					{
						if (pNoFPE[n] == '/' || pNoFPE[n] == '\\')
						{
							sprintf(pJustName, " - %s", pNoFPE + n + 1);
							break;
						}
					}
					char pHullDecompWait[256];
					sprintf(pHullDecompWait, "HULL DECOMPOSITION%s", pJustName);
					t.screenprompt_s = pHullDecompWait;
					extern DWORD g_SensibleMessageTimer;
					g_SensibleMessageTimer = 1;
					printscreenprompt(t.screenprompt_s.Get());
				}
			}

			extern bool physics_playground;
			// skip if material should come direct from DBO mesh data
			if (t.entityprofile[t.entid].materialindex != 99999)
			{
				SetObjectArbitaryValue ( t.tphyobj, t.entityprofile[t.entid].materialindex );
			}
			if ( t.tstatic == 1 ) // now allow physics entities in multiplayer || t.game.runasmultiplayer == 1 ) 
			{
				// if static, need to ensure FIXNEWY pivot is respected
				if ( t.tstatic == 1 ) 
				{
					t.tstaticfixnewystore_f=ObjectAngleY(t.tphyobj);
					RotateObject (  t.tphyobj,ObjectAngleX(t.tphyobj),ObjectAngleY(t.tphyobj)+t.entityprofile[t.entid].fixnewy,ObjectAngleZ(t.tphyobj) );
				}

				// if special polygon collision mode (that uses OBJ) handle now)
				if (t.tshape == 8)
				{
					// set the FPE filename so the .OBJ or _COL.obj can be checked and used if present
					char pFullFPEFilename[MAX_PATH];
					sprintf(pFullFPEFilename, "%s\\Files\\entitybank\\%s", g.fpscrootdir_s.Get(), t.entitybank_s[t.entid].Get());
					ODESetOBJLoadingFilename(pFullFPEFilename);

					// and now treat as regular polygon collision mesh
					t.tshape = 2; 
				}

				//  create the physics now
				if (physics_playground)
				{
					t.entityelement[t.e].staticflag = 0;
					ODECreateDynamicTriangleMesh(t.tphyobj, t.tweight, t.tfriction, -1, 1); //Turn everything into convex hull.
				}
				else if (t.tshape >= 1000 && t.tshape < 2000)
				{
					ODECreateStaticBox (  t.tphyobj,t.tshape-1000 );
				}
				else if ( t.tshape >= 2000 && t.tshape < 3000 ) 
				{
					ODECreateStaticTriangleMesh (  t.tphyobj,t.tshape-2000 );
				}
				else if ( t.tshape == 1 ) 
				{
					ODECreateStaticBox ( t.tphyobj );
				}
				else if (t.tshape == 6)
				{
					ODECreateStaticSphere ( t.tphyobj );
				}
				else if (t.tshape == 7)
				{
					ODECreateStaticCylinder ( t.tphyobj );
				}
				else if ( t.tshape == 2 || t.tshape == 9 || t.tshape == 10)
				{
					if (  t.tshape == 2 ) 
					{
						if (  t.tcollisionscaling != 100 ) 
						{
							ODECreateStaticTriangleMesh (  t.tphyobj,-1,t.tcollisionscaling );
						}
						else
						{
							ODECreateStaticTriangleMesh (  t.tphyobj );
						}
					}
					else
					{
						if (t.tshape == 10)
						{
							ODECreateStaticTriangleMesh (t.tphyobj, -1, t.tcollisionscaling, 2);
						}
						else
						{
							ODECreateStaticTriangleMesh (t.tphyobj, -1, t.tcollisionscaling, 1);
						}
					}
				}
				else if ( t.tshape == 3 ) 
				{
					physics_setuptreecylinder ( );
				}
				// tshape 4 is a list of physics objects from the importer
				else if ( t.tshape == 4 ) 
				{
					physics_setupimportershapes ( );
				}
				// if static, restore object before leaving
				if ( t.tstatic == 1 ) 
				{
					RotateObject (  t.tphyobj,ObjectAngleX(t.tphyobj),t.tstaticfixnewystore_f,ObjectAngleZ(t.tphyobj) );
				}
			}
			else
			{
				// objects will fall through Floor (  if they are perfectly sitting on it )
				PositionObject ( t.tphyobj, ObjectPositionX(t.tphyobj), ObjectPositionY(t.tphyobj)+0.1, ObjectPositionZ(t.tphyobj) );

				if(physics_playground)
				{
					ODECreateDynamicTriangleMesh(t.tphyobj, t.tweight, t.tfriction, -1, 1);
				}
				else
				if ( t.tshape == 6 )     
				{
					// Sphere
					ODECreateDynamicSphere(t.tphyobj, t.tweight, t.tfriction, 0.01f);
				}
				else if ( t.tshape == 7 ) 
				{
					// Cylinder
					ODECreateDynamicCylinder(t.tphyobj, t.tweight, t.tfriction, 0.01f);
				}
				else if (t.tshape == 9)
				{
					// Dynamic convex hull
					ODECreateDynamicTriangleMesh(t.tphyobj, t.tweight, t.tfriction, -1, 1);
				}
				else if (t.tshape == 10)
				{
					// Dynamic hull decomp
					ODECreateDynamicTriangleMesh(t.tphyobj, t.tweight, t.tfriction, -1, 2);
				}
				else
				{
					// box
					ODECreateDynamicBox(t.tphyobj, -1, 0, t.tweight, t.tfriction, -1);
				}

				// apply zero gravity if ticked
				if (t.entityelement[t.e].eleprof.iAffectedByGravity == 0)
				{
					ODESetNoGravity(t.tphyobj, 0);
				}
			}
		}
	}
	ODESetMeshFilename("");
	ODESetOBJLoadingFilename("");
}

int physics_findfreegamerealtimeobj ( void )
{
	int iFreeObj = g.gamerealtimeobjoffset;
	while ( ObjectExist ( iFreeObj ) == 1 && iFreeObj < g.gamerealtimeobjoffsetmax )
	{
		iFreeObj++;
	}
	if ( iFreeObj == g.gamerealtimeobjoffsetmax )
	{
		if ( ObjectExist ( iFreeObj ) == 1 ) DeleteObject ( iFreeObj );
	}
	return iFreeObj;
}

void physics_freeallgamerealtimeobjs ( void )
{
	for ( int iObj = g.gamerealtimeobjoffset; iObj <= g.gamerealtimeobjoffsetmax; iObj++ )
		if ( ObjectExist ( iObj ) == 1 ) 
			DeleteObject ( iObj );
}

void physics_setupimportershapes ( void )
{
	// flag to control if debug collision boxes should be left
	bool bLeaveDebugCollisionBoxes = false;
	if ( g.globals.showdebugcollisonboxes == 1 ) bLeaveDebugCollisionBoxes = true;

	// get collision boxes data from entity to make importer collision shapes
	if ( t.entid > MAX_ENTITY_PHYSICS_BOXES*2  ) 
	{
		Dim2 ( t.entityphysicsbox , t.entid , MAX_ENTITY_PHYSICS_BOXES   );
	}
	ODEStartStaticObject (  t.tphyobj );
	float fMoveToObjectWorldX = ObjectPositionX ( t.tphyobj );
	float fMoveToObjectWorldY = ObjectPositionY ( t.tphyobj );
	float fMoveToObjectWorldZ = ObjectPositionZ ( t.tphyobj );
	for ( t.tcount = 0 ; t.tcount <= t.entityprofile[t.entid].physicsobjectcount-1; t.tcount++ )
	{
		if (  ObjectExist(g.tempimporterlistobject)  )  DeleteObject (  g.tempimporterlistobject );
		int iObjectToUse = g.tempimporterlistobject;
		if ( bLeaveDebugCollisionBoxes == true ) iObjectToUse = physics_findfreegamerealtimeobj();
		t.tescale=t.entityprofile[t.entid].scale;
		if (  t.tescale>0 ) 
		{
			t.tnewscalex_f=t.tescale+t.entityelement[t.e].scalex;
			t.tnewscaley_f=t.tescale+t.entityelement[t.e].scaley;
			t.tnewscalez_f=t.tescale+t.entityelement[t.e].scalez;
		}
		else
		{
			t.tnewscalex_f=100+t.entityelement[t.e].scalex;
			t.tnewscaley_f=100+t.entityelement[t.e].scaley;
			t.tnewscalez_f=100+t.entityelement[t.e].scalez;
		}
		t.tnewsizex_f=(t.tnewscalex_f/100.0)*(t.entityphysicsbox[t.entid][t.tcount].SizeX);
		t.tnewsizey_f=(t.tnewscaley_f/100.0)*(t.entityphysicsbox[t.entid][t.tcount].SizeY);
		t.tnewsizez_f=(t.tnewscalez_f/100.0)*(t.entityphysicsbox[t.entid][t.tcount].SizeZ);
		MakeObjectBox ( iObjectToUse, t.tnewsizex_f, t.tnewsizey_f, t.tnewsizez_f );
		float tNewOffX = (t.tnewscalex_f/100.0) * t.entityphysicsbox[t.entid][t.tcount].OffX;
		float tNewOffY = (t.tnewscaley_f/100.0) * t.entityphysicsbox[t.entid][t.tcount].OffY;
		float tNewOffZ = (t.tnewscalez_f/100.0) * t.entityphysicsbox[t.entid][t.tcount].OffZ;
		t.tocy_f=ObjectSizeY(t.tphyobj,1)/2.0;
		PositionObject ( iObjectToUse, tNewOffX, t.tocy_f + tNewOffY, tNewOffZ );
		RotateObject ( iObjectToUse, t.entityphysicsbox[t.entid][t.tcount].RotX , t.entityphysicsbox[t.entid][t.tcount].RotY , t.entityphysicsbox[t.entid][t.tcount].RotZ );
		ODEAddStaticObjectBox ( t.tphyobj, iObjectToUse, t.entityprofile[t.entid].materialindex );
		if ( bLeaveDebugCollisionBoxes == true ) 
		{
			FixObjectPivot ( iObjectToUse );
			RotateObject ( iObjectToUse, t.entityelement[t.e].rx, t.entityelement[t.e].ry, t.entityelement[t.e].rz );
			GGVECTOR3 vecOffset = GGVECTOR3 ( tNewOffX, t.tocy_f + tNewOffY, tNewOffZ );
			sObject* pObjectPtr = GetObjectData ( t.tphyobj );
			GGVec3TransformCoord ( &vecOffset, &vecOffset, &pObjectPtr->position.matRotation );
			PositionObject ( iObjectToUse, fMoveToObjectWorldX+vecOffset.x, fMoveToObjectWorldY+vecOffset.y, fMoveToObjectWorldZ+vecOffset.z );
		}
	}
	if ( ObjectExist(g.tempimporterlistobject)  )  DeleteObject (  g.tempimporterlistobject );
	ODEEndStaticObject (  t.tphyobj, 0 );
}

void physics_setuptreecylinder ( void )
{
	//  takes; tphyobj and entid and sets up a tree cylinder
	if (  t.tphyobj < 1  )  return;
	if (  ObjectExist(t.tphyobj)  ==  0  )  return;
	if (  t.entid < 1  )  return;

	//  Tree height (adjusted for scale)
	t.tSizeY_f = ObjectSizeY(t.tphyobj,1);

	//  if have ABS position from AI OBSTACLE calc, use that instead
	if (  t.entityelement[t.e].abscolx_f != -1 ) 
	{
		t.tFinalX_f = t.entityelement[t.e].abscolx_f;
		t.tFinalZ_f = t.entityelement[t.e].abscolz_f;
	}
	else
	{
		t.tFinalX_f = ObjectPositionX(t.tphyobj);
		t.tFinalZ_f = ObjectPositionZ(t.tphyobj);
	}
	t.tFinalY_f = ObjectPositionY(t.tphyobj) + (t.tSizeY_f/2.0);

	//  if have ABS radius from AI OBSTACLE calc, use that instead
	if (  t.entityelement[t.e].abscolradius_f != -1 ) 
	{
		t.tSizeX_f = t.entityelement[t.e].abscolradius_f;
		t.tSizeZ_f = t.entityelement[t.e].abscolradius_f;
	}
	else
	{
		t.tSizeX_f = 20;
		t.tSizeZ_f = 20;
	}

	//  increase size by 25%
	t.tSizeX_f=t.tSizeX_f*1.25;
	t.tSizeZ_f=t.tSizeZ_f*1.25;

	// now create our physics object
	SetObjectArbitaryValue (t.tphyobj, 3);// 3-wood not 6 flesh!
	ODECreateStaticCylinder ( t.tphyobj,t.tFinalX_f,t.tFinalY_f,t.tFinalZ_f,t.tSizeX_f,t.tSizeY_f,t.tSizeZ_f,0,0,0 );
}

void physics_disableobject ( void )
{
	ODEDestroyObject (  t.tphyobj );
}

void physics_beginsimulation ( void )
{
	t.machineindependentphysicsupdate = timeGetSecond();
}

void physics_pausephysics ( void )
{
	t.ptimer1 = timeGetSecond();
}

void physics_resumephysics ( void )
{
	//t.ptimer2=PerformanceTimer() ; t.ptimer2=t.ptimer2/t.pfreq1;
	t.ptimer2 = timeGetSecond();
	t.machineindependentphysicsupdate=t.machineindependentphysicsupdate+(t.ptimer2-t.ptimer1);
}

#include "..\..\..\WICKEDREPO\WickedEngine\wiProfiler.h"

void physics_loop ( void )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	// shuffle virtual trees about as the player needs
	physics_managevirtualtreecylinders();

	// Player control
	extern int iEnterGodMode;
	if (iEnterGodMode != 2)
	{
		if (g.gproducelogfiles == 2) timestampactivity(0, "calling physics_player");
		physics_player();
	}
	//  Update physics system
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling timeGetSecond");
	t.tphysicsadvance_f = timeGetSecond() - t.machineindependentphysicsupdate;
	if (  t.tphysicsadvance_f >= (1.0/120.0) ) 
	{
		//  only process physics once we reach the minimum substep constant
		if ( t.tphysicsadvance_f>0.05f ) t.tphysicsadvance_f = 0.05f;
		t.machineindependentphysicsupdate = timeGetSecond();
		if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling ODEUpdate");
		ODEUpdate ( t.tphysicsadvance_f );
	}
}

void physics_free ( void )
{
	// special hybrid collision mode can hide static limbs, so reshow them when physics done
	for (t.e = 1; t.e <= g.entityelementlist; t.e++)
	{
		t.obj = t.entityelement[t.e].obj;
		if (t.obj > 0)
		{
			if (ObjectExist(t.obj) == 1)
			{
				t.entid = t.entityelement[t.e].bankindex;
				if (t.entityprofile[t.entid].collisionmode == 31)
				{
					// reshow previously hidden static limbs
					PerformCheckListForLimbs(t.obj);
					for (int c = ChecklistQuantity(); c >= 1; c += -1)
					{
						t.tname_s = Lower(ChecklistString(c));
						LPSTR pNamePtr = t.tname_s.Get();
						if (strlen(pNamePtr) > 7)
						{
							if (strnicmp(pNamePtr + strlen(pNamePtr) - 7, "_static", 7) == NULL)
							{
								ShowLimb(t.obj, c - 1);
							}
						}
					}

					// and delete the secondary object created to show the static limbs detached from the primary
					if (t.entityelement[t.e].attachmentobj > 0)
					{
						int iSecondaryObjID = t.entityelement[t.e].attachmentobj;
						if (ObjectExist(iSecondaryObjID) == 1) DeleteObject (iSecondaryObjID);
						t.entityelement[t.e].attachmentobj = 0;
					}
				}
			}
		}
	}

	// just hide the virtual tree cylinders until we use them again
	physics_freevirtualtreecylinders();

	// remove any game realtime objects (used for debugging collision boxes, possible LUA spawned 3D objects, etc)
	physics_freeallgamerealtimeobjs();

	// free terrain physics object
	if ( t.terrain.superflat == 0 ) 
	{
		for ( t.tobj = t.terrain.TerrainLODOBJStart ; t.tobj <= t.terrain.TerrainLODOBJFinish; t.tobj++ )
		{
			if (  ObjectExist(t.tobj) == 1 ) 
			{
				ODEDestroyObject (  t.tobj );
			}
		}
	}
	else
	{
		//PE: Need to free all terrain phy objects, 7000+ , remember we keep the pMem1,pMem2 memory with data that need to be freed.
		if (ObjectExist(t.tphysicsterrainobjstart) == 1)
		{
			if (t.tphysicsterrainobjend > t.tphysicsterrainobjstart)
			{
				for (int obj = t.tphysicsterrainobjstart; obj <= t.tphysicsterrainobjend; obj++)
				{
					ODEDestroyObject(obj);
					DeleteObject(obj);
				}
			}
			else
			{
				ODEDestroyObject(t.tphysicsterrainobjstart);
				DeleteObject(t.tphysicsterrainobjstart);
			}
		}
	}

	//  detatch entity from physics
	for ( t.e = 1 ; t.e <= g.entityelementlist; t.e++ )
	{
		t.obj=t.entityelement[t.e].obj;
		if ( t.obj>0 ) 
		{
			if ( ObjectExist(t.obj) == 1 ) 
			{
				if ( t.entityelement[t.e].usingphysicsnow == 1 ) 
				{
					t.tphyobj=t.obj  ; physics_disableobject ( );
					t.entityelement[t.e].usingphysicsnow=0;
				}
			}
		}
	}

	// Clean-up physics system
	extern int g_iDevToolsOpen;
	if (g_iDevToolsOpen != 0)
	{
		physics_set_debug_draw(0);
	}
	ODEEnd(); 
	g.gphysicssessionactive=0;
}

void physics_explodesphere ( void )
{
	//  takes texplodex#,texplodey#,texplodez#,texploderadius#,t.texplodesourceEntity
	t.tstrengthofexplosion_f = t.tDamage_f;

	//  detect if player within radius and apply damage
	t.tdx_f=ObjectPositionX(t.aisystem.objectstartindex)-t.texplodex_f;
	t.tdy_f=ObjectPositionY(t.aisystem.objectstartindex)-t.texplodey_f;
	t.tdz_f=ObjectPositionZ(t.aisystem.objectstartindex)-t.texplodez_f;
	t.tdd_f=Sqrt(abs(t.tdx_f*t.tdx_f)+abs(t.tdy_f*t.tdy_f)+abs(t.tdz_f*t.tdz_f));
	if (  t.tdd_f<t.texploderadius_f ) 
	{
		//  apply camera shake for nearby explosion
		t.playercontrol.camerashake_f=(((t.texploderadius_f*2)-(t.tdd_f/(t.texploderadius_f*2)))*t.tstrengthofexplosion_f)/150.0/20.0;
	}
	if (  t.tdd_f<t.texploderadius_f ) 
	{
		// apply damage
		t.te=-1;
		t.tdamage = (1.0f - (t.tdd_f / t.texploderadius_f)) * t.tstrengthofexplosion_f;
		if ( t.tdamage > t.tstrengthofexplosion_f ) t.tdamage = t.tstrengthofexplosion_f;
		if ( t.game.runasmultiplayer == 1 ) 
		{
			t.tsteamwasnetworkdamage = 0;
			if ( t.entityelement[t.texplodesourceEntity].mp_networkkill == 1 ) 
			{
				// 13032015 0XX - Team Multiplayer
				if ( g.mp.team  ==  0 || g.mp.friendlyfireoff  ==  0 || t.mp_team[t.entityelement[t.texplodesourceEntity].mp_killedby]  !=  t.mp_team[g.mp.me] ) 
				{
					t.tsteamwasnetworkdamage = 1;
				}
			}
		}
		if ( t.game.runasmultiplayer  ==  1 ) 
		{
			// 13032015 0XX - Team Multiplayer
			// Can't kill yourself if friendly fire is off
			if ( t.tsteamwasnetworkdamage  ==  1 || g.mp.friendlyfireoff  ==  0 || g.mp.damageWasFromAI  ==  1 ) 
			{
				physics_player_takedamage ( );
			}
		}
		else
		{
			physics_player_takedamage ( );
		}
		if ( t.game.runasmultiplayer == 1 ) t.tsteamwasnetworkdamage = 0;

		//  apply force to push player
		t.playercontrol.pushangle_f = atan2deg(t.tdx_f,t.tdz_f);
		float fForceRelativeToDamage = t.tstrengthofexplosion_f / 30.0f;
		t.playercontrol.pushforce_f = (1.0f-(t.tdd_f/t.texploderadius_f))*fForceRelativeToDamage;
	}
	//  if the explosion was caused by another player, we let them handle it rather than us
	if (  t.game.runasmultiplayer  ==  1 ) 
	{
		if (  t.entityelement[t.texplodesourceEntity].mp_networkkill  ==  1 && g.mp.damageWasFromAI  ==  0 ) 
		{
			return;
		}
	}
	//  create a sphere of force at this location
	for ( t.e = 1 ; t.e <= g.entityelementlist; t.e++ )
	{
		t.entid=t.entityelement[t.e].bankindex;
		if (  g.mp.damageWasFromAI  ==  0 ) 
		{
			if (  t.texplodesourceEntity > 0 && t.e == t.texplodesourceEntity  )  t.entid = 0;
		}
		if ( t.entid > 0 && t.entityelement[t.e].obj > 0 ) 
		{
			// early rejects
			if (t.entityprofile[t.entid].ismarker != 0) continue;
			if (t.entityelement[t.e].active == 0) continue;
			if (t.entityelement[t.e].staticflag == 1) continue;
			if (t.entityelement[t.e].y <= -899999) continue;

			// 220618 - use center of object, not coordinate of entity XYZ
			float fCenterOfEntityX = ObjectPositionX(t.entityelement[t.e].obj) + GetObjectCollisionCenterX(t.entityelement[t.e].obj);
			float fCenterOfEntityY = ObjectPositionY(t.entityelement[t.e].obj) + GetObjectCollisionCenterY(t.entityelement[t.e].obj);
			float fCenterOfEntityZ = ObjectPositionZ(t.entityelement[t.e].obj) + GetObjectCollisionCenterZ(t.entityelement[t.e].obj);
			t.tdx_f = fCenterOfEntityX - t.texplodex_f;
			t.tdy_f = fCenterOfEntityY - t.texplodey_f;
			t.tdz_f = fCenterOfEntityZ - t.texplodez_f;
			t.tdd_f = Sqrt(abs(t.tdx_f*t.tdx_f)+abs(t.tdy_f*t.tdy_f)+abs(t.tdz_f*t.tdz_f));
			if (t.tdd_f < t.texploderadius_f)
			{
				// 220618 - before apply actual entity damage/effect, ensure a line of sight exists (could be behind wall/door)
				float fRayDestFromExplosionX = fCenterOfEntityX - t.texplodex_f;
				float fRayDestFromExplosionY = fCenterOfEntityY - t.texplodey_f;
				float fRayDestFromExplosionZ = fCenterOfEntityZ - t.texplodez_f;
				fRayDestFromExplosionX += t.texplodex_f;
				fRayDestFromExplosionY += t.texplodey_f;
				fRayDestFromExplosionZ += t.texplodez_f;

				// refer to previously collected information on anything that explodes (performance boost and anit-freeze system)
				t.tintersectvalue = -1;
				int iExplodingE = t.texplodesourceEntity;
				if (iExplodingE > 0)
				{
					if (t.entityelement[iExplodingE].iPreScannedVisible.size() > 0)
					{
						for (int i = 0; i < t.entityelement[iExplodingE].iPreScannedVisible.size(); i++)
						{
							if (t.e == t.entityelement[iExplodingE].iPreScannedVisible[i])
							{
								t.tintersectvalue = 0;
								break;
							}
						}
					}
				}

				// if still no block, must check all (the slower way)
				if (t.tintersectvalue == -1)
				{
					// NOTE: May speed this up by setting all above prescan objects to cursor objects for this one test..
					// must perform direct test as dont have prescannedvis list to compare if we have a direct ray line
					t.tintersectvalue = IntersectAll(g.entityviewstartobj, g.entityviewendobj,
										t.texplodex_f, t.texplodey_f, t.texplodez_f, 
										fRayDestFromExplosionX, fRayDestFromExplosionY, fRayDestFromExplosionZ, 
										t.entityelement[t.e].obj );
				}

				if ( t.tintersectvalue == 0 || t.tintersectvalue == t.entityelement[ t.texplodesourceEntity ].obj )
				{
					t.tdamage = (1.0f - (t.tdd_f / t.texploderadius_f)) * t.tstrengthofexplosion_f;
					t.tdamageforce = (1.0f - (t.tdd_f / t.texploderadius_f)) * (t.tstrengthofexplosion_f);
					t.brayx1_f = t.texplodex_f;
					t.brayy1_f = t.texplodey_f;
					t.brayz1_f = t.texplodez_f;
					t.brayx2_f = fCenterOfEntityX;
					t.brayy2_f = fCenterOfEntityY;
					t.brayz2_f = fCenterOfEntityZ;
					t.braydx_f = t.brayx2_f-t.brayx1_f;
					t.braydz_f = t.brayz2_f-t.brayz1_f;
					t.braydist_f = Sqrt(abs(t.braydx_f*t.braydx_f)+abs(t.braydz_f*t.braydz_f));
					if ( t.braydist_f < 75 ) t.brayy2_f = t.texplodey_f + 100.0;
					if ( t.tdamageforce > 150 ) t.tdamageforce = 150;
					// explosion (time delayed explosion), except when a grenade/missile vs exploder, instead make it instant :)
					t.tdamagesource = 2;
					if (t.texplodesourceEntity == 0) t.tdamagesource = 3;
					t.ttte = t.e ; entity_applydamage( ); t.e = t.ttte;

					//  inform darkAI of the explosion
					t.tsx_f = t.entityelement[t.e].x; 
					t.tsz_f = t.entityelement[t.e].z;
					darkai_makeexplosionsound ( );
				}
			}
		}
	}

	//  Reset flag for ai damage
	g.mp.damageWasFromAI = 0;
}

void physics_player_init ( void )
{
	// One Player In Single Player Game 
	t.plrid=1;
	t.tnostartmarker=1;
	g.flashLightKeyEnabled = true;

	//  Initialise player settings
	if ( t.game.levelplrstatsetup == 1 )
	{
		// starting stats
		t.playercontrol.startlives=0;
		t.playercontrol.startstrength=100;
		t.playercontrol.startviolent=1;
		t.playercontrol.starthasweapon=0;
		t.playercontrol.starthasweaponqty=0;
		t.playercontrol.speedratio_f=1.0;
		t.playercontrol.hurtfall=100;
		t.playercontrol.canrun=1;
		t.player[t.plrid].lives=t.playercontrol.startlives;
		t.player[t.plrid].powers.level = 100;

		//LB: new player health intercept
		t.player[t.plrid].health=t.playercontrol.startstrength; // overwritten later when LUA init and active!

		// act on start marker
		t.playercontrol.hurtfall=0;
		t.playercontrol.speedratio_f=1.0;
	}
	for ( t.e = 1 ; t.e <= g.entityelementlist; t.e++ )
	{
		t.entid=t.entityelement[t.e].bankindex;
		if ( t.entityprofile[t.entid].ismarker == 1 ) 
		{
			//  Player Start Marker Settings
			t.terrain.playerx_f=t.entityelement[t.e].x;
			t.terrain.playery_f=t.entityelement[t.e].y;
			t.terrain.playerz_f=t.entityelement[t.e].z;

			void FixEulerZInverted(float &ax, float &ay, float &az);
			float ax, ay, az;
			ax = t.entityelement[t.e].rx;
			ay = t.entityelement[t.e].ry;
			az = t.entityelement[t.e].rz;
			FixEulerZInverted(ax,ay,az);

			t.terrain.playerax_f=0;
			t.terrain.playeray_f = ay;
			t.camangy_f=t.terrain.playeray_f;
			t.terrain.playeraz_f=0;
			t.playercontrol.finalcameraangley_f=t.terrain.playeray_f;

			//  Player Global Settings for this level
			if ( t.game.levelplrstatsetup == 1 )
			{
				t.playercontrol.startlives = t.entityelement[t.e].eleprof.lives;
				t.playercontrol.startstrength=t.entityelement[t.e].eleprof.strength;
				if (  t.playercontrol.thirdperson.enabled == 1 ) 
				{
					t.tprotagoniste=t.playercontrol.thirdperson.charactere;
					t.playercontrol.starthasweapon=t.entityelement[t.tprotagoniste].eleprof.hasweapon;
				}
				else
				{
					t.playercontrol.starthasweapon=t.entityelement[t.e].eleprof.hasweapon;
				}
				t.playercontrol.starthasweaponqty=t.entityelement[t.e].eleprof.quantity;
				t.playercontrol.startviolent=t.entityelement[t.e].eleprof.isviolent;
				t.playercontrol.speedratio_f=t.entityelement[t.e].eleprof.speed/100.0;
				t.playercontrol.hurtfall=t.entityelement[t.e].eleprof.hurtfall;

				t.playercontrol.iPlayHeartBeatSoundOff = t.entityelement[t.e].eleprof.perentityflags & 1;
				t.playercontrol.iShowScreenBloodOff = (t.entityelement[t.e].eleprof.perentityflags & (1 << 1)) >> 1;
				t.playercontrol.fWeaponDamageMultiplier = t.entityelement[t.e].eleprof.weapondamagemultiplier;
				t.playercontrol.fMeleeDamageMultiplier = t.entityelement[t.e].eleprof.meleedamagemultiplier;
				t.playercontrol.fSwimSpeed = t.entityelement[t.e].eleprof.iSwimSpeed;

				// new property of player start marker to disable flashlight
				if (t.entityelement[t.e].eleprof.usespotlighting == 1 )
					g.flashLightKeyEnabled = false;
				else
					g.flashLightKeyEnabled = true;

				// 050416 - if in parental mode, ensure no weapon at start
				if ( g.quickparentalcontrolmode == 2 )
				{
					// only ban modern day weapons, not fireball
					int iPlrGunID = t.weaponindex=t.playercontrol.starthasweapon;
					if ( iPlrGunID > 0 )
					{
						if ( strnicmp ( t.gun[iPlrGunID].name_s.Get(), "modernday", 9 ) == NULL )
						{
							t.playercontrol.starthasweapon = 0;
							t.playercontrol.starthasweaponqty = 0;
							t.playercontrol.startviolent = 0;
						}
					}
				}

				//  Populate lives and health with default player
				t.player[t.plrid].lives=t.playercontrol.startlives;

				//LB: new player health intercept
				t.player[t.plrid].health=t.playercontrol.startstrength; // overwritten later when LUA init and active!

				//  Start Marker present
				t.tnostartmarker=0;
			}
			else
			{
				// level 2 and above do not control start stats or weaponry
			}
		}
	}

	float fEditableSizeHalved = GGTerrain_GetEditableSize();
	t.terraineditableareasizeminx = -fEditableSizeHalved;
	t.terraineditableareasizeminz = -fEditableSizeHalved;
	t.terraineditableareasizemaxx = fEditableSizeHalved;
	t.terraineditableareasizemaxz = fEditableSizeHalved;
	if (t.terrain.playerx_f < t.terraineditableareasizeminx + 100.0f) t.terrain.playerx_f = t.terraineditableareasizeminx + 100.0f;
	if (t.terrain.playerx_f > t.terraineditableareasizemaxx - 100.0f) t.terrain.playerx_f = t.terraineditableareasizemaxx - 100.0f;
	if (t.terrain.playerz_f < t.terraineditableareasizeminz + 100.0f) t.terrain.playerz_f = t.terraineditableareasizeminz + 100.0f;
	if (t.terrain.playerz_f > t.terraineditableareasizemaxz - 100.0f) t.terrain.playerz_f = t.terraineditableareasizemaxz - 100.0f;

	//  If no player start marker, reset player physics tweakables
	if ( t.game.levelplrstatsetup == 1 )
	{
		if ( t.tnostartmarker == 1 ) physics_inittweakables ( );
	}

	// if multiplayer mode, change start position to the multiplayer start marker default
	if ( t.game.runasmultiplayer == 1 ) 
	{
		// store good one
		float fGoodX = t.terrain.playerx_f;
		float fGoodY = t.terrain.playery_f;
		float fGoodZ = t.terrain.playerz_f;
		float fGoodA = t.terrain.playeray_f;

		// chose a multiplayer start position at random
		int iChoose = 1;
		if ( t.tmpstartindex > 1 ) iChoose = 1 + (rand() % t.tmpstartindex);
		t.terrain.playerx_f=t.mpmultiplayerstart[iChoose].x;
		t.terrain.playery_f=t.mpmultiplayerstart[iChoose].y;
		t.terrain.playerz_f=t.mpmultiplayerstart[iChoose].z;
		t.terrain.playeray_f=t.mpmultiplayerstart[iChoose].angle;
		t.camangy_f=t.terrain.playeray_f;
		if ( t.terrain.playerx_f < 100 )
		{
			// no start position, revert to regular start marker
			t.terrain.playerx_f = fGoodX;
			t.terrain.playery_f = fGoodY;
			t.terrain.playerz_f = fGoodZ;
			t.terrain.playeray_f = fGoodA;
			t.camangy_f=t.terrain.playeray_f;
		}
		t.playercontrol.finalcameraangley_f=t.terrain.playeray_f;
	}

	//  Player start height (marker or no)
	t.tbestterrainplayery_f = BT_GetGroundHeight(t.terrain.TerrainID, t.terrain.playerx_f, t.terrain.playerz_f) + t.terrain.adjaboveground_f;

	// LB: Probably some request/rationalle to always start above water line, but new request wants to start underwater if marker is underwater :)
	// also ensure ABOVE water Line
	//if (  t.tbestterrainplayery_f<t.terrain.waterliney_f+20+t.terrain.adjaboveground_f ) 
	//{
	//	t.tbestterrainplayery_f=t.terrain.waterliney_f+20+t.terrain.adjaboveground_f;
	//}
	if (  t.terrain.playery_f == 0 ) 
	{
		t.terrain.playery_f=t.tbestterrainplayery_f;
	}
	else
	{
		t.terrain.playery_f=t.terrain.playery_f+t.terrain.adjaboveground_f;
		if (  t.terrain.playery_f<t.tbestterrainplayery_f  )  t.terrain.playery_f = t.tbestterrainplayery_f;
	}

	// Select weapon if start marker specifies it
	if ( t.game.levelplrstatsetup == 1 )
	{
		// sometimes the player would spawn with a "phantom weapon" where the weapon slot would be occupied and obstruct collecting/using weapons
		// this makes sure that the player start the level with no weapons and unoccupied weapon slots
		physics_player_resetWeaponSlots();

		// if starting with a weapon, grant it here
		if (  t.playercontrol.starthasweapon>0 ) 
		{
			// weapon added to weapon slot 1
			t.weaponindex=t.playercontrol.starthasweapon;
			t.tqty=t.playercontrol.starthasweaponqty;
			physics_player_addweapon ( );

			// as this was never a level-object weapon, we force it into the slot 
			// and ensure it cannot be removed or dropped, there is no object associated with it
			inventoryContainerType item;
			item.e = 0;
			cstr thisWeaponTitle = gun_names_tonormal(t.gun[t.weaponindex].name_s.Get());
			item.collectionID = find_rpg_collectionindex(thisWeaponTitle.Get());
			if (item.collectionID == 0)
			{
				thisWeaponTitle = t.gun[t.weaponindex].name_s.Get();
				item.collectionID = find_rpg_collectionindex(thisWeaponTitle.Get());
			}
			item.slot = 0;
			for (int n = 1; n <= 10; n++) { if (t.weaponslot[n].pref == t.weaponindex) { item.slot = n - 1; break; } }
			t.inventoryContainer[1].push_back(item);
		}
	}
	else
	{
		// if have weapon from previous level session, activate it
		if ( t.lastgunid > 0 ) 
		{
			g.autoloadgun = t.lastgunid;
			t.lastgunid = 0;
		}
	}

	// and only use this flag once per game
	t.game.levelplrstatsetup = 0;

	//  OpenFileMap (  for IDE access )
	if (  t.plrfilemapaccess == 0 && t.game.gameisexe == 0 ) 
	{
		OpenFileMap (  11, "FPSEXCHANGE" );
		SetEventAndWait (  11 );
		t.plrfilemapaccess=1;
	}
}

void physics_player_free ( void )
{
	if (  t.plrfilemapaccess == 1 ) 
	{
		t.plrfilemapaccess=0;
	}
}

//Dave Performance
int physics_player_listener_delay = 0;
void physics_player ( void )
{
	if ( t.game.runasmultiplayer == 0 || g.mp.noplayermovement == 0 ) 
	{
		if ( t.aisystem.processplayerlogic == 1 ) 
		{
			if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling physics_player_gatherkeycontrols");
			physics_player_gatherkeycontrols ( );
			gun_update_hud ( );
			if ( ++physics_player_listener_delay > 3 )
			{
				physics_player_listener_delay = 0;
				if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling physics_player_listener");
				physics_player_listener ( );
			}
		}
		else
		{
			// prevent player physics movement
			if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling ODEControlDynamicCharacterController");
			ODEControlDynamicCharacterController ( t.aisystem.objectstartindex, 0, 0, 0, 0, t.aisystem.playerducking, 0, 0, 0 );

			//PE: Make sure to rotate set camera to make shadows work in freezemode.
			if (t.freezeplayerposonly == 0)
				RotateObject(t.aisystem.objectstartindex, t.terrain.playerax_f, t.terrain.playeray_f, t.terrain.playeraz_f);
			if (g.luacameraoverride != 2 && g.luacameraoverride != 3)
			{
				// lee - 140820 - when this flag is set with Q&A script, terrain suddenly gets shadow!
				if (t.freezeplayerposonly == 0) 
					RotateCamera(0, t.terrain.playerax_f, t.terrain.playeray_f, t.terrain.playeraz_f);
				else
					RotateCamera(0, t.freezeplayerax, t.freezeplayeray, t.freezeplayeraz);
			}

		}
	}
}

void physics_player_gatherkeycontrols ( void )
{
	// Camera in control of player one
	t.plrid=1;

	// Key configuration
	t.plrkeyW=17;
	t.plrkeyA=30;
	t.plrkeyS=31;
	t.plrkeyD=32;
	t.plrkeyQ=16;
	t.plrkeyE=18;
	t.plrkeyF=33;
	t.plrkeyC=46;
	t.plrkeyZ=44;
	t.plrkeyR=19;
	t.plrkeySPACE=57;
	t.plrkeyRETURN=28;
	t.plrkeySHIFT=42;
	t.plrkeySHIFT2=54;
	t.plrkeyF12=88;
	t.plrkeyJ=36;

	// used to override physical key pressed
	static int g_lastplrkeyForceKeystate = -1;
	if (t.plrkeyForceKeystate != g_lastplrkeyForceKeystate)
	{
		g_lastplrkeyForceKeystate = t.plrkeyForceKeystate;
		extern int ForceKeyStateValue(int);
		ForceKeyStateValue(t.plrkeyForceKeystate);
	}

	// from SETUP.INI config keys
	if ( t.listkey[1]>0  )  t.plrkeyW = t.listkey[1];
	if ( t.listkey[2]>0  )  t.plrkeyS = t.listkey[2];
	if ( t.listkey[3]>0  )  t.plrkeyA = t.listkey[3];
	if ( t.listkey[4]>0  )  t.plrkeyD = t.listkey[4];
	if ( t.listkey[5]>0  )  t.plrkeySPACE = t.listkey[5];
	if ( t.listkey[6]>0  )  t.plrkeyC = t.listkey[6];
	if ( t.listkey[7]>0  )  t.plrkeyRETURN = t.listkey[7];
	if ( t.listkey[8]>0  )  t.plrkeyR = t.listkey[8];
	if ( t.listkey[9]>0  )  t.plrkeyQ = t.listkey[9];
	if ( t.listkey[10]>0  )  t.plrkeyE = t.listkey[10];
	if ( t.listkey[11]>0  )  t.plrkeySHIFT = t.listkey[11];

	// Read keys from config, and use in player control actions
	if ( g.walkonkeys == 1 ) 
	{
		if ( KeyState(g.keymap[t.plrkeyW]) ==1 ) { t.plrkeyW=1 ; t.plrkeySLOWMOTION=0 ;} else t.plrkeyW=0;
		if ( KeyState(g.keymap[t.plrkeyA]) ==1 ) { t.plrkeyA=1 ; t.plrkeySLOWMOTION=0 ;} else t.plrkeyA=0;
		if ( KeyState(g.keymap[t.plrkeyS]) ==1 ) { t.plrkeyS=1 ; t.plrkeySLOWMOTION=0 ;} else t.plrkeyS=0;
		if ( KeyState(g.keymap[t.plrkeyD]) ==1 ) { t.plrkeyD=1 ; t.plrkeySLOWMOTION=0 ;} else t.plrkeyD=0;
	}
	if ( g.arrowkeyson == 1 ) 
	{
		t.tplrkeySLOWMOTIONold=t.plrkeySLOWMOTION;
		if ( UpKey() == 1 ) { t.plrkeyW = 1  ; t.plrkeySLOWMOTION = 1; }
		if ( LeftKey() == 1 ) { t.plrkeyA = 1  ; t.plrkeySLOWMOTION = 1; }
		if ( DownKey() == 1 ) { t.plrkeyS = 1  ; t.plrkeySLOWMOTION = 1; }
		if ( RightKey() == 1 ) { t.plrkeyD = 1  ; t.plrkeySLOWMOTION = 1; }
		if ( t.tplrkeySLOWMOTIONold != t.plrkeySLOWMOTION ) 
		{
			t.null=MouseMoveX() ; t.null=MouseMoveY();
			t.cammousemovex_f=0 ; t.cammousemovey_f=0;
			t.tFinalCamX_f=ObjectPositionX(t.aisystem.objectstartindex);
			t.tFinalCamY_f=ObjectPositionY(t.aisystem.objectstartindex);
			t.tFinalCamZ_f=ObjectPositionZ(t.aisystem.objectstartindex);
		}
	}
	if ( KeyState(g.keymap[t.plrkeySHIFT]) == 1 && g.runkeys == 1 && t.jumpaction == 0  )  t.plrkeySHIFT = 1; else t.plrkeySHIFT = 0;
	if ( KeyState(g.keymap[t.plrkeySHIFT2]) == 1 && g.runkeys == 1 && t.jumpaction == 0  )  t.plrkeySHIFT2 = 1; else t.plrkeySHIFT2 = 0;

	// when in vr mode
	if ( g.vrglobals.GGVREnabled > 0 && g.vrglobals.GGVRUsingVRSystem == 1 )
	{
		// lee - 040619 - detect trigger and use as t.plrkeyE
		if ( GGVR_RightController_Trigger() > 0.5 || GGVR_LeftController_Trigger() > 0.5 )
			t.plrkeyE = 1;

		// lee - 040619 - detect grip button and use as run
		if ( GGVR_RightController_Grip() == 1 || GGVR_LeftController_Grip() == 1 )
			t.plrkeySHIFT = 1;

		// Using PGUP/PGDN to control gvrsitteradjust that modifies the VR sitting position for the eye of the player/user
		int iVRSitterAdjustMode = 0;
		if (KeyState(201)) iVRSitterAdjustMode = 1;
		if (KeyState(209)) iVRSitterAdjustMode = 2;
		if (iVRSitterAdjustMode > 0)
		{
			// make thr adjustment
			if (iVRSitterAdjustMode == 1) g.gvrsitteradjust += 1;
			if (iVRSitterAdjustMode == 2) g.gvrsitteradjust -= 1;

			// and save the setting for next time
			FPSC_SaveSETUPVRINI();
		}
	}

	if ( t.conkit.editmodeactive != 0 ) 
	{
		// FPS 3D Editing Mode - keys elsewhere
	}
	else
	{
		// FPS Gaming Mode
		if ( KeyState(g.keymap[t.plrkeySPACE]) == 1 && g.jumponkey == 1  )  t.plrkeySPACE = 1; else t.plrkeySPACE = 0;
		if ( KeyState(g.keymap[t.plrkeyQ]) == 1 && g.peekonkeys == 1  )  t.plrkeyQ = 1; else t.plrkeyQ = 0;
		if ( KeyState(g.keymap[t.plrkeyE]) == 1 && g.peekonkeys == 1  )  t.plrkeyE = 1; else t.plrkeyE = 0;
		if ( KeyState(g.keymap[t.plrkeyF]) == 1  )  t.plrkeyF = 1; else t.plrkeyF = 0;
		if ( KeyState(g.keymap[t.plrkeyC]) == 1 && g.crouchonkey == 1  )  t.plrkeyC = 1; else t.plrkeyC = 0;
		if ( ControlKey() == 1  )  t.plrkeyC = 1;
		if ( KeyState(g.keymap[t.plrkeyZ]) == 1  )  t.plrkeyZ = 1; else t.plrkeyZ = 0;
		if ( KeyState(g.keymap[t.plrkeyR]) == 1  )  t.plrkeyR = 1; else t.plrkeyR = 0;
		if ( KeyState(g.keymap[t.plrkeyRETURN]) == 1  )  t.plrkeyRETURN = 1; else t.plrkeyRETURN = 0;
		if ( KeyState(g.keymap[t.plrkeyJ]) == 1  )  t.plrkeyJ = 1; else t.plrkeyJ = 0;
	}

	// XBOX/Controller Keys
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"checking XBOX/Controller Keys");
	if ( g.gxbox == 1 ) 
	{
		if ( JoystickFireC() == 1 ) 
		{
			t.plrkeyR = 1;
		}
		if ( JoystickFireD() == 1 )  // also duplicated in LUA.cpp
		{
			t.plrkeyE = 1;
		}
	}
	if ( g.gxbox == 1 ) 
	{
		if ( g.walkonkeys == 1 ) 
		{
			if ( JoystickY()<-850  )  t.plrkeyW = 1;
			if ( JoystickY()>850  )  t.plrkeyS = 1;
			if ( JoystickX()<-850  )  t.plrkeyA = 1;
			if ( JoystickX()>850  )  t.plrkeyD = 1;
		}
		if ( JoystickFireA() == 1 && g.jumponkey == 1   )  t.plrkeySPACE = 1;
		if ( g.gxboxcontrollertype == 0 ) 
		{
			// XBOX360 Controller
			if ( JoystickFireXL(8) == 1 && g.crouchonkey == 1  )  t.plrkeyC = 1;
			if ( JoystickFireXL(9) == 1  )  t.plrkeyZ = 1;
		}
		if ( g.gxboxcontrollertype == 1 ) 
		{
			// Dual Action
			if ( JoystickFireXL(10) == 1 && g.crouchonkey == 1  )  t.plrkeyC = 1;
			if ( JoystickFireXL(11) == 1  )  t.plrkeyZ = 1;
			if ( JoystickFireXL(4) == 1 && g.runkeys == 1  )  t.plrkeySHIFT = 1;
			if ( JoystickFireXL(6) == 1 && g.runkeys == 1  )  t.plrkeySHIFT = 1;
		}
		if ( g.gxboxcontrollertype == 2 ) 
		{
			// Dual Action F310
			if ( JoystickFireXL(10) == 1 && g.crouchonkey == 1  )  t.plrkeyC = 1;
			if ( JoystickFireXL(11) == 1  )  t.plrkeyZ = 1;
			if ( JoystickFireXL(4) == 1 && g.runkeys == 1  )  t.plrkeySHIFT = 1;
			if ( JoystickFireXL(6) == 1 && g.runkeys == 1  )  t.plrkeySHIFT = 1;
		}
	}

	// VR Support - take extra input from VR controllers
	if ( g.vrglobals.GGVREnabled > 0 && g.vrglobals.GGVRUsingVRSystem == 1 )
	{
		// Intialize the player to the start position and rotation and setup the GGVR Player Object
		if ( g.gproducelogfiles == 2 ) timestampactivity(0,"checking VR controllers");
		if (g.vrglobals.GGVRInitialized == 0)
		{
			if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling GGVR_SetPlayerPosition");
			GGVR_SetPlayerPosition(t.tFinalCamX_f, BT_GetGroundHeight(t.terrain.TerrainID, t.tFinalCamX_f, t.tFinalCamZ_f), t.tFinalCamZ_f);
			if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling GGVR_SetPlayerRotation");

			// this sets the origin based on the current camera zero (ARG!)
			// should only set based on player angle (minus HMD influence) as HMD added later at right time for smooth headset viewing!
			GGVR_SetPlayerRotation(0, 0, 0);

			if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling GGVR_UpdatePlayer(false,terrainID)");
			if ( g.gproducelogfiles == 2 ) 
			{
				char pWhatIsPath[1024];
				GetCurrentDirectoryA ( 1024, pWhatIsPath );
				timestampactivity(0,pWhatIsPath);
			}
			try
			{
				int iBatchStart = g.batchobjectoffset;
				int iBatchEnd = g.batchobjectoffset + g.merged_new_objects + 1;
				GGVR_UpdatePlayer(false,t.terrain.TerrainID,g.lightmappedobjectoffset,g.lightmappedobjectoffsetfinish,g.entityviewstartobj,g.entityviewendobj, iBatchStart, iBatchEnd);
			}
			catch(...)
			{
				timestampactivity(0,"try catch failed when calling GGVR_UpdatePlayer");
			}
			if ( g.gproducelogfiles == 2 ) timestampactivity(0,"getting GGVR_GetHMDOffsets");
			g.vrglobals.GGVR_Old_XposOffset = GGVR_GetHMDOffsetX();
			g.vrglobals.GGVR_Old_ZposOffset = GGVR_GetHMDOffsetZ();
			g.vrglobals.GGVR_Old_Yangle = GGVR_GetPlayerAngleY();
			if ( g.gproducelogfiles == 2 ) timestampactivity(0,"g.vrglobals.GGVRInitialized");
			g.vrglobals.GGVRInitialized = 1;
		}
		if (g.walkonkeys == 1)
		{
			if (GGVR_LeftController_JoyY() > 0.5) t.plrkeyW = 1;
			if (GGVR_LeftController_JoyY() < -0.5)  t.plrkeyS = 1;
		}
		g.vrglobals.GGVR_XposOffset = GGVR_GetHMDOffsetX();
		g.vrglobals.GGVR_ZposOffset = GGVR_GetHMDOffsetZ();
		g.vrglobals.GGVR_XposOffsetChange = g.vrglobals.GGVR_XposOffset - g.vrglobals.GGVR_Old_XposOffset;
		g.vrglobals.GGVR_ZposOffsetChange = g.vrglobals.GGVR_ZposOffset - g.vrglobals.GGVR_Old_ZposOffset;
		g.vrglobals.GGVR_Old_XposOffset = g.vrglobals.GGVR_XposOffset;
		g.vrglobals.GGVR_Old_ZposOffset = g.vrglobals.GGVR_ZposOffset;
	}

	// Automated actions (script control)
	switch ( g.playeraction ) 
	{
		case 1 : t.player[1].state.firingmode = 1; break ;
		case 2 : t.gunzoommode = 1 ; break ;
		case 3 : t.player[1].state.firingmode = 2 ; break ;
		case 4 : g.forcecrouch = 1 ; break ;
		case 5 : t.plrkeySPACE = 1 ; break ;
		case 6 : t.plrkeyE = 1 ; break ;
		case 7 : t.plrkeyQ = 1 ; break ;
		case 8 : t.plrkeyRETURN = 1 ; break ;
		case 9 : t.tmouseclick = 1 ; break ;
		case 10 : t.tmouseclick = 2 ; break ;
		case 11 : 
		{
			// ensure weapon unjams affect both modes if sharing ammo
			g.firemodes[t.gunid][g.firemode].settings.jammed = 1; 
			if ( t.gun[t.gunid].settings.modessharemags == 1 ) 
			{
				g.firemodes[t.gunid][0].settings.jammed = 1;
				g.firemodes[t.gunid][1].settings.jammed = 1;
			}
		}
		break;
	}

	// Third person disables crouch/zoom/RMB
	t.playercontrol.camrightmousemode=0;
	if ( t.playercontrol.thirdperson.enabled == 1 ) 
	{
		g.forcecrouch=0 ; t.plrkeyC=0;
		t.gunzoommode = 0 ; if ( t.tmouseclick == 2 ) { t.tmouseclick = 0  ; t.playercontrol.camrightmousemode = 1; }
	}

	//  Free weapon jam if reload used (possible relocate these to gun module
	if ( t.player[1].state.firingmode == 2 ) //&& t.gunzoommode == 0 ) 
	{
		// unjam or reload animation to unjam weapon
		g.plrreloading=1;
		g.firemodes[t.gunid][g.firemode].settings.shotsfired=0;
		// play free jam animation if it exists
		if ( g.firemodes[t.gunid][g.firemode].action2.clearjam.s != 0 && g.firemodes[t.gunid][g.firemode].settings.jammed == 1 ) 
		{
			// come out of zoom if in it
			if ( t.gunzoommode >=8 ) t.gunzoommode = 11; // catches all states of a zoomed in state

			// play anim to fix jam
			g.plrreloading=2;
			g.custstart=g.firemodes[t.gunid][g.firemode].action2.clearjam.s;
			g.custend=g.firemodes[t.gunid][g.firemode].action2.clearjam.e;
			t.gunmode=9998;
		}
		g.firemodes[t.gunid][g.firemode].settings.shotsfired=0;

		// ensure weapon unjams affect both modes if sharing ammo
		g.firemodes[t.gunid][g.firemode].settings.jammed = 0;
		if ( t.gun[t.gunid].settings.modessharemags == 1 ) 
		{
			g.firemodes[t.gunid][0].settings.jammed = 0;
			g.firemodes[t.gunid][1].settings.jammed = 0;
		}
	}

	// Forced key controls (script control)
	if ( g.forcemove>0  )  t.plrkeyW = 1;
	if ( g.forcemove<0  )  t.plrkeyS = 1;
	if ( g.forcecrouch == 1 && g.playeraction != 4  )  g.forcecrouch = 0;
	if ( g.playeraction != 4  )  g.playeraction = 0;

	// interrogate IDE to see if we have input focus
	if ( t.game.gameisexe == 0 ) 
	{
		t.plrhasfocus=1;
		if ( t.plrfilemapaccess == 1 ) 
		{
			// if VR, disable this as WMR changes the focus window
			if ( g.vrglobals.GGVREnabled > 0 )
			{
				// window focus can be switched to HMD window
			}
			else
			{
				// normal behavior
				t.plrhasfocus=1;
				#if !defined(ENABLEIMGUI) || defined(USEOLDIDE)
				t.plrhasfocus=GetFileMapDWORD( 11, 148 );
				#endif
			}
		}
	}

	// If player no health (dead), cannot control anything
	if ( t.player[t.plrid].health <= 0 || t.plrhasfocus == 0 ) 
	{
		t.plrkeyW=0;
		t.plrkeyA=0;
		t.plrkeyS=0;
		t.plrkeyD=0;
		t.plrkeyQ=0;
		t.plrkeyE=0;
		t.plrkeyF=0;
		t.plrkeyC=0;
		t.plrkeyZ=0;
		t.plrkeyR=0;
		t.plrkeySPACE=0;
		t.plrkeyRETURN=0;
		t.plrkeySHIFT=0;
		t.plrkeySHIFT2=0;
		t.plrkeyJ=0;
		t.plrkeyForceKeystate = 0;
	}
}

void physics_no_gun_zoom ( void )
{
	g.realfov_f=t.visuals.CameraFOV_f;
	if ( g.realfov_f < 15 ) g.realfov_f = 15;
	SetCameraFOV ( g.realfov_f );
	SetCameraFOV ( 2, g.realfov_f );
}

void physics_getcorrectjumpframes ( int entid, float* fStartFrame, float* fHoldFrame, float* fResumeFrame, float* fFinishFrame )
{
	// use frames stored in VAULT animation
	t.q = t.entityprofile[entid].startofaianim + t.csi_stoodvault[1];
	*fStartFrame = t.entityanim[entid][t.q].start;
	*fFinishFrame = t.entityanim[entid][t.q].finish;

	// jump hold animation frames overridden in FPE
	if ( t.entityprofile[entid].jumphold > 0 )
		*fHoldFrame = t.entityprofile[entid].jumphold;
	else
		*fHoldFrame = t.entityanim[entid][t.q].finish - 10;

	// jump resume frame to indicate when can resume movement
	if ( t.entityprofile[entid].jumpresume > 0 )
		*fResumeFrame = t.entityprofile[entid].jumpresume;
	else
		*fResumeFrame = t.entityanim[entid][t.q].finish;
}

void physics_player_control_LUA ( void )
{
	// when LUA global ready, call LUA player control function
	if ( t.playercontrol.gameloopinitflag == 0 )
	{
		// F9 Edit Mode Controls internal
		if ( t.conkit.editmodeactive != 0 )
		{
		}
		else
		{
			// Feed in-game mappable keys
			LuaSetInt ( "g_PlrKeyW", t.plrkeyW );
			LuaSetInt ( "g_PlrKeyA", t.plrkeyA );
			LuaSetInt ( "g_PlrKeyS", t.plrkeyS );
			LuaSetInt ( "g_PlrKeyD", t.plrkeyD );
			LuaSetInt ( "g_PlrKeyQ", t.plrkeyQ );
			LuaSetInt ( "g_PlrKeyE", t.plrkeyE ); 
			LuaSetInt ( "g_PlrKeyF", t.plrkeyF );
			LuaSetInt ( "g_PlrKeyC", t.plrkeyC );
			LuaSetInt ( "g_PlrKeyZ", t.plrkeyZ );
			LuaSetInt ( "g_PlrKeyR", t.plrkeyR );
			LuaSetInt ( "g_PlrKeySPACE", t.plrkeySPACE );
			LuaSetInt ( "g_PlrKeyRETURN", t.plrkeyRETURN );
			LuaSetInt ( "g_PlrKeySHIFT", t.plrkeySHIFT );
			LuaSetInt ( "g_PlrKeySHIFT2", t.plrkeySHIFT2 );
			LuaSetInt ( "g_PlrKeyJ", t.plrkeyJ );

			// Call externaliszed script
			LuaSetFunction ( "PlayerControl", 0, 0 );
			LuaCall();
		}
	}
}

void physics_player_control ( void )
{
	// No player controls when in editing mode
	if ( t.plrhasfocus == 0 ) return;

	// Gather input data
	t.k_s=Lower(Inkey() );

	// Invincibe Mode - God Mode
	if ( g.ggodmodestate == 1 && t.k_s == "i" ) t.player[1].health = 99999;

	// Get MouseClick (except when in TAB TAB mode)
	// Mode 1 = ignore A, C, D buttons of controller
	if ( g.tabmode < 2 ) 
		t.tmouseclick = control_mouseclick_mode(1);
	else
		t.tmouseclick=0;

	// Set input data for LUA call
	LuaSetInt ( "g_KeyPressJ", t.plrkeyJ );
	LuaSetInt ( "g_MouseClickControl", t.tmouseclick );
	physics_player_control_LUA();

	// Apply colour to shader
	SetVector4 ( g.terrainvectorindex, t.playercontrol.redDeathFog_f, 0, 0, 0 );
	t.tColorVector = g.terrainvectorindex;
	postprocess_setscreencolor ( );
}

void physics_player_handledeath ( void )
{
	// handle player death
	if (  t.game.runasmultiplayer == 0 ) 
	{
		//  Handle player death - only for single player
		if (  t.playercontrol.deadtime>0 ) 
		{
			//  control sequence
			if (  t.playercontrol.thirdperson.enabled == 0 ) 
			{
				if (  CameraAngleZ(0)<45 ) 
				{
					if ( g.luacameraoverride != 2 && g.luacameraoverride != 3 )
					{
						ZRotateCamera (  0,CameraAngleZ(0)+5.0 );
					}
				}
			}
			//  when death pause over
			if (  t.aisystem.processplayerlogic == 1 ) 
			{
				if (  Timer()>t.playercontrol.deadtime ) 
				{
					if (  t.playercontrol.startlives>0 && t.player[t.plrid].lives == 0 && t.game.gameisexe == 1 ) 
					{
						//  280415 - GAME OVER flag!
						t.game.gameloop=0 ; t.game.lostthegame=1;
					}
					else
					{
						//  move player to start
						physics_player_gotolastcheckpoint ( );
						//PE: Restart any ambient music tracks.
						if (t.gamevisuals.bEndableAmbientMusicTrack)
						{
							//PE: start any ambient music tracks.
							int iFreeSoundID = g.temppreviewsoundoffset + 3;
							if (SoundExist(iFreeSoundID) == 1) DeleteSound(iFreeSoundID);
							if (FileExist(t.visuals.sAmbientMusicTrack.Get()) == 1)
							{
								LoadSound(t.visuals.sAmbientMusicTrack.Get(), iFreeSoundID, 0, 1);
								if (SoundExist(iFreeSoundID) == 1)
								{
									LoopSound(iFreeSoundID);
									SetSoundVolume(iFreeSoundID, t.visuals.iAmbientMusicTrackVolume);
								}
							}
						}
						int iFreeSoundID = g.temppreviewsoundoffset + 5;
						if (SoundExist(iFreeSoundID) == 1) DeleteSound(iFreeSoundID);
						if (FileExist(t.visuals.sCombatMusicTrack.Get()) == 1)
						{
							LoadSound(t.visuals.sCombatMusicTrack.Get(), iFreeSoundID, 0, 1);
							if (t.gamevisuals.bEnableCombatMusicTrack)
							{
								if (SoundExist(iFreeSoundID) == 1)
								{
									LoopSound(iFreeSoundID);
									SetSoundVolume(iFreeSoundID, 0); //PE: Controlled in lua t.visuals.iCombatMusicTrackVolume);
									extern std::unordered_map<int, float> luavolumes;
									luavolumes.insert_or_assign(iFreeSoundID, 0);
								}
							}
						}
					}
				}
			}
		}
	}
}

void physics_play_thump_sound (float fX, float fY, float fZ, float fStartFreq, float fFreqRange)
{
	int iThumpID = 1 + (rand() % 6);
	#define SIXTHUMPSOUNDSFORMELEE 6
	for (int iTryAll = 0; iTryAll < SIXTHUMPSOUNDSFORMELEE; iTryAll++)
	{
		if (SoundPlaying(g.meleethumpsoundoffset + iThumpID) == 0)
			break;
		iThumpID++;
		if (iThumpID > SIXTHUMPSOUNDSFORMELEE)
			iThumpID = 1;
	}
	int iThumpSound = g.meleethumpsoundoffset + iThumpID;
	if (SoundPlaying(iThumpSound) == 0)
	{
		if (iThumpSound > 0 && SoundExist(iThumpSound) == 1)
		{
			PositionSound (iThumpSound, fX, fY, fZ);
			SetSoundSpeed (iThumpSound, fStartFreq + Rnd(fFreqRange));
			PlaySound (iThumpSound);
		}
	}
}

void physics_player_reset_underwaterstate ( void )
{
	visuals_underwater_off ( );
	t.playercontrol.inwaterstate = 0;
	t.playercontrol.drowntimestamp = 0;
}

void physics_player_listener ( void )
{
	ScaleListener (  5.0  ); RotateListener (  0,CameraAngleY(),0 );
	PositionListener ( CameraPositionX(), CameraPositionY(), CameraPositionZ() );
}

void physics_player_takedamage ( void )
{
	// Receives; tdamage, te, tDrownDamageFlag
	// Uses tDrownDamageFlag to avoid blood splats and other non drowning damage effects.
	// This is set to 0 after takedamage is called, so doesn't need to be unset elsewhere
	// before calling this sub
	float fOriginalDamage = t.tdamage;

	// Apply player shake, even if immune to damage
	if ((t.tdamage > 0 && t.player[t.plrid].health > 0) || t.tdamage > 65000)
	{
		float fMoreShakeIfMeleeBased = 1.0f;
		if (t.te > 0)
		{
			int gunid = t.entityelement[t.te].eleprof.hasweapon;
			if (gunid > 0)
			{
				if (t.gun[gunid].weapontype == 51) // melee weapon type is 51
				{
					fMoreShakeIfMeleeBased = 3.0f;
				}
			}
		}
		else
		{
			// if no entity inflicting, assume external or generic damage, so more shake!
			fMoreShakeIfMeleeBased = 2.5f;
		}
		t.playercontrol.camerashake_f = (t.tdamage / 100.0f) * 25.0f * fMoreShakeIfMeleeBased;
	}

	// a successful block with deflect any further damage event
	bool bSuccessfullyBlockingNow = false;
	if (t.player[1].state.blockingaction == 2)
	{
		float protectedstart = g.firemodes[t.gunid][g.firemode].blockaction.idle.s;
		float protectedend = g.firemodes[t.gunid][g.firemode].blockaction.idle.e;
		if (protectedstart == 0 && protectedstart == 0)
		{
			// when block loop (idle) missing, use entire block sequence as more generous block
			protectedstart = g.firemodes[t.gunid][g.firemode].blockaction.start.s;
			protectedend = g.firemodes[t.gunid][g.firemode].blockaction.finish.e;
		}
		else
		{
			// be a 'little' lenient when using BLOCK LOOP phase, allowing for the block frame to rest
			protectedstart -= 2; protectedend += 5;
			if (protectedend > g.firemodes[t.gunid][g.firemode].blockaction.finish.e)
			{
				// however do not allow it to go beyond end of block finish frame
				protectedend = g.firemodes[t.gunid][g.firemode].blockaction.finish.e;
			}
		}
		float fFrameNow = GetFrame(t.currentgunobj);
		if (fFrameNow >= protectedstart && fFrameNow <= protectedend)
		{
			// block has succeeded, player hurt within block range
			bSuccessfullyBlockingNow = true;

			// to give us an extra game cycle of bullet time!
			SetObjectSpeed (t.currentgunobj, 1);

			// if this entity hitteer was blocked, then it has opened itself up to being counter attacked :)
			if (t.te > 0)
			{
				extern int g_iCounterAttackTargetForPlayer;
				if (g_iCounterAttackTargetForPlayer == 0)
				{
					g_iCounterAttackTargetForPlayer = t.te;
				}
			}
		}
		else
		{
			// block has failed, player hurt outside of block range
			// this can only trigger if BLOCK LOOP is specified, ie the window of ACTUAL block!
			t.player[1].state.blockingaction = 4;
		}
	}

	// player cannot be damaged when immune!
	if ( t.huddamage.immunity>0  )  return;

	// quite early if in F9 editing mode
	if ( t.conkit.editmodeactive != 0  )  return;

	//  Apply player health damage
	if (  (t.tdamage>0 && t.player[t.plrid].health>0) || t.tdamage>65000 ) 
	{
		// aloow all player hit code to proceed (block/etc) but just deal no damage
		bool bImmunityFromActualDamage = false;
		if (t.player[t.plrid].health >= 99999)
		{
			bImmunityFromActualDamage = true;
			t.tdamage = 0;
		}

		//  Flag player damage in health regen code
		if (  t.playercontrol.regentime>0  )  t.playercontrol.regentime = Timer();

		//  Deduct health from player
		if (t.playercontrol.startstrength > 0 && bSuccessfullyBlockingNow==false)
		{	
			// deduct player health
			LuaSetFunction ("PlayerHealthSubtract", 1, 0);
			LuaPushInt(t.tdamage);
			LuaCall();
			t.player[t.plrid].health = LuaGetInt("g_PlayerHealth");
		}

		// if NOT drowning, do usual damage stuff
		if ( t.tDrownDamageFlag == 0 ) 
		{
			// Instruct HUD about player damage
			if ( t.playercontrol.startviolent != 0 && g.quickparentalcontrolmode != 2 && bSuccessfullyBlockingNow==false )
			{
				if ( t.playercontrol.thirdperson.enabled == 1 ) 
				{
					// third person character produces blood
					if ( t.te != -1 ) 
					{
						t.ttobj1=t.entityelement[t.playercontrol.thirdperson.charactere].obj;
						t.decalx1=ObjectPositionX(t.ttobj1);
						t.decaly1=ObjectPositionY(t.ttobj1)+(ObjectSizeY(t.ttobj1)/2);
						t.decalz1=ObjectPositionZ(t.ttobj1);
						t.ttobj2=t.entityelement[t.te].obj;
						t.decalx2=ObjectPositionX(t.ttobj2);
						t.decaly2=ObjectPositionY(t.ttobj2)+(ObjectSizeY(t.ttobj2)/2);
						t.decalz2=ObjectPositionZ(t.ttobj2);
						t.ttdx_f=t.decalx1-t.decalx2;
						t.ttdy_f=t.decaly1-t.decaly2;
						t.ttdz_f=t.decalz1-t.decalz2;
						t.ttdd_f=Sqrt(abs(t.ttdx_f*t.ttdx_f)+abs(t.ttdy_f*t.ttdy_f)+abs(t.ttdz_f*t.ttdz_f));
						t.ttdx_f=t.ttdx_f/t.ttdd_f;
						t.ttdy_f=t.ttdy_f/t.ttdd_f;
						t.ttdz_f=t.ttdz_f/t.ttdd_f;
						g.decalx=t.decalx1-(t.ttdx_f*5.0);
						g.decaly=t.decaly1-(t.ttdy_f*5.0);
						g.decalz=t.decalz1-(t.ttdz_f*5.0);
						g.decalx=(g.decalx-10)+Rnd(20);
						g.decaly=(g.decaly-20)+Rnd(40);
						g.decalz=(g.decalz-10)+Rnd(20);
						for ( t.iter = 1 ; t.iter<=  1+Rnd(1); t.iter++ )
						{
							decal_triggerbloodsplat ( );
						}
					}
				}
				else
				{
					if (bImmunityFromActualDamage == false)
					{
						if (t.te != -1)
						{
							// only if entity caused damage
							new_damage_marker(t.te, ObjectPositionX(t.entityelement[t.te].obj), ObjectPositionY(t.entityelement[t.te].obj), ObjectPositionZ(t.entityelement[t.te].obj), t.tdamage);
						}
						else
						{
							// hurt from non-entity source
							for (t.iter = 0; t.iter <= 9; t.iter++)
							{
								placeblood(50, 0, 0, 0, 0);
							}
						}
					}
				}
			}

			// if melee damage, mark with a thump!
			//PE: Only if melee. it would play whenever hurtplayer was called from lua.
			if (t.te > 0)
			{
				int gunid = t.entityelement[t.te].eleprof.hasweapon;
				if (gunid > 0)
				{
					if (t.gun[gunid].weapontype == 51) // melee weapon type is 51
					{
						physics_play_thump_sound(CameraPositionX(), CameraPositionY(), CameraPositionZ(), 38000, Rnd(8000));
					}
				}
			}


			// Trigger player grunt noise or block sound
			if (bSuccessfullyBlockingNow == false)
			{
				if (bImmunityFromActualDamage == false)
				{
					if (t.playercontrol.startviolent != 0 && g.quickparentalcontrolmode != 2)
					{
						if ((DWORD)(Timer() + 250) > t.playercontrol.timesincelastgrunt)
						{
							// only ever one in three or if been a while since we grunted
							t.playercontrol.timesincelastgrunt = Timer();
							int iLastOne = t.tplrhurt;
							bool bHaveUniqueSound = false;
							while (bHaveUniqueSound == false)
							{
								int iRandomHurt = Rnd(12);
								switch (iRandomHurt)
								{
									case 0: t.tplrhurt = 1; break;
									case 1: t.tplrhurt = 2; break;
									case 2: t.tplrhurt = 3; break;
									case 3: t.tplrhurt = 4; break;
									case 4: t.tplrhurt = 8; break;
									case 5: t.tplrhurt = 9; break;
									case 6: t.tplrhurt = 10; break;
									case 7: t.tplrhurt = 16; break;
									case 8: t.tplrhurt = 21; break;
									case 9: t.tplrhurt = 22; break;
									case 10: t.tplrhurt = 23; break;
									case 11: t.tplrhurt = 24; break;
									case 12: t.tplrhurt = 25; break;
								}
								if (iLastOne != t.tplrhurt) bHaveUniqueSound = true;
							}
							t.tsnd = t.playercontrol.soundstartindex + t.tplrhurt;
							playinternalsound(t.tsnd);
						}
					}
				}
			}
			else
			{
				// if player is blocking, no damage
				
				// and play shield style sound with subtle camera shake to show we took it well!
				int iRandomBlockSnd = Rnd(3);
				switch (iRandomBlockSnd)
				{
					case 0: t.tplrhurt = 26; break;
					case 1: t.tplrhurt = 27; break;
					case 2: t.tplrhurt = 28; break;
				}
				t.tsnd = t.playercontrol.soundstartindex + t.tplrhurt;
				playinternalsound(t.tsnd);
				int iMinDamage = t.tdamage;
				if (t.tdamage < 50) iMinDamage = 50;
				t.playercontrol.camerashake_f = (iMinDamage / 100.0f) * 100.0f;

				// aloow all player hit code to proceed (block/etc) but just deal no damage
				if (t.player[t.plrid].health >= 99999) t.tdamage = fOriginalDamage;
				float fhalfdamagebacktoenemy = t.tdamage / 2.0f;

				// but repel their damage back to the attacker
				if (t.te > 0 && t.te < t.entityelement.size())
				{
					t.ttte = t.te;
					t.tdamage = fhalfdamagebacktoenemy;
					t.tdamageforce = 0.0f;
					t.brayx1_f = CameraPositionX();
					t.brayy1_f = CameraPositionY();
					t.brayz1_f = CameraPositionZ();
					t.brayx2_f = t.entityelement[t.te].x;
					t.brayy2_f = t.entityelement[t.te].y;
					t.brayz2_f = t.entityelement[t.te].z;
					t.tallowanykindofdamage = 0;
					t.twhox_f = t.brayx1_f;
					t.twhoz_f = t.brayz1_f;
					t.tdamagesource = 1;
					entity_applydamage ();
				}

				// and mark an actual player block
				if (g_iSuccessfullyBlockedAtTime == 0)
					g_iSuccessfullyBlockedAtTime = 1;

				// no further player damage code
				return;
			}
		}
		
		// Check if player health at zero
		if ( t.player[t.plrid].health <= 0 ) 
		{
			t.player[t.plrid].health = 0;
			LuaSetFunction ("PlayerHealthSet", 1, 0);
			LuaPushInt(0);
			LuaCall();
			if (  t.game.runasmultiplayer  ==  1 )
			{
				if (  t.tsteamwasnetworkdamage  ==  1 ) 
				{
					if (  t.entityelement[t.texplodesourceEntity].mp_networkkill  ==  1 ) 
					{
						//  inform of network kill
						mp_networkkill ( );
					}
				}
			}
			//  player looses a life
			if (  t.playercontrol.startlives>0 ) 
			{
				//  only reduce lives if using lives
				if (  t.game.runasmultiplayer == 0 ) 
				{
					t.player[t.plrid].lives=t.player[t.plrid].lives-1;
					if (  t.player[t.plrid].lives <= 0 ) 
					{
						t.player[t.plrid].lives=0;
					}
				}
			}
			if ( t.playercontrol.startviolent != 0 && g.quickparentalcontrolmode != 2 )
			{
				if ( t.tDrownDamageFlag == 0 ) 
				{
					// player grunts in deadness if this isn't death by drowning
					playinternalsound(t.playercontrol.soundstartindex+1);
				}
			}
			// if camera was overriden, take it back
			g.luacameraoverride = 0;
			// if was frozen, unfreeze for the restore
			t.aisystem.processplayerlogic = 1;
			// restore player zoom
			t.plrzoominchange=1 ; t.plrzoomin_f=0.0;
			gun_playerdead ( );
			// start death sequence for player
			t.playercontrol.deadtime=Timer()+2000;
			// make sure all music is stopped
			if (  t.playercontrol.disablemusicreset == 0 ) 
			{
				music_resetall ( );
			}
			//  if third person, also create ragdoll of protagonist
			if (  t.playercontrol.thirdperson.enabled == 1 ) 
			{
				t.ttte=t.playercontrol.thirdperson.charactere;
				t.tdamageforce=0;
				t.entityelement[t.ttte].health=1 ; t.tdamage=1;
				t.entityelement[t.ttte].ry=ObjectAngleY(t.entityelement[t.ttte].obj);
				t.tskiplayerautoreject=1;
				entity_applydamage ( );
				t.tskiplayerautoreject=0;
			}
		}
	}
}

void physics_player_gotolastcheckpoint ( void )
{
	//  move player to last checkpoint (or start marker if no checkpoint)
	t.terrain.playerx_f=t.playercheckpoint.x;
	t.terrain.playery_f=t.playercheckpoint.y;
	t.terrain.playerz_f=t.playercheckpoint.z;
	t.terrain.playerax_f=0;
	t.terrain.playeray_f=t.playercheckpoint.a;
	t.camangy_f=t.terrain.playeray_f;
	t.terrain.playeraz_f=0;
	t.playercontrol.finalcameraangley_f=t.terrain.playeray_f;
	physics_resetplayer_core ( );

	// resume all soundloops from when passed through checkpoint
	bool bPauseAndResumeFromGameMenu = false;
	game_main_snapshotsoundresumecheckpoint(bPauseAndResumeFromGameMenu);
}

void physics_resetplayer_core ( void )
{
	//  Cannot restart under water!
	if (  t.hardwareinfoglobals.nowater == 0 ) 
	{
		if (  t.terrain.playery_f<t.terrain.waterliney_f ) 
		{
			t.terrain.playery_f=t.terrain.waterliney_f+t.terrain.adjaboveground_f;
		}
	}

	//  if the player was previous underwater, set them above water and switch off underwater effects
	physics_player_reset_underwaterstate ( );

	//  disable and setup player
	physics_disableplayer ( );
	physics_setupplayer ( );

	//  restore health
	t.player[t.plrid].health=t.playercontrol.startstrength;

	//  ressurection cease fire allows player to escape shooters when respawn
	t.playercontrol.ressurectionceasefire=Timer()+3000;

	//  reset vegetation
	t.completelyfillvegarea=1;

	//  fade in game screen again
	t.postprocessings.fadeinvalue_f=0.0;

	//  player is immune for a while
	t.huddamage.immunity=1000;

	//  reset death state
	t.playercontrol.deadtime=0;
	if ( g.luacameraoverride != 2 && g.luacameraoverride != 3 )
	{
		ZRotateCamera (  0,0 );
	}

	//  red screen effect finish
	t.playercontrol.redDeathFog_f = 0;
	SetVector4 (  g.terrainvectorindex,0,0,0,0 );
	t.tColorVector = g.terrainvectorindex ; postprocess_setscreencolor ( );

	//  Stop any blood HUD
	resetblood();
	resetdamagemarker();

	//  Deal with sounds if not disabled via script
	if (  t.playercontrol.disablemusicreset == 0 ) 
	{
		//  Stop any incidental music
		game_stopallsounds();
		//  Stop any looping projectile sounds
		weapon_projectile_reset ( );
		//  play default music
		music_playdefault ( );

		//  Restore any sounds from last checkpoint/start marker
		for ( t.s = g.soundbankoffset ; t.s<=  g.soundbankoffsetfinish; t.s++ )
		{
			if (  t.soundloopcheckpoint[t.s] == 1 ) 
			{
				if (  SoundExist(t.s) == 1 ) 
				{
					LoopSound (  t.s );
				}
			}
		}
	}

	//  ensure all markers and waypoints remain hidden
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.entid=t.entityelement[t.e].bankindex;
		if (  t.entityprofile[t.entid].ismarker != 0 ) 
		{
			t.obj=t.entityelement[t.e].obj;
			if (  t.obj>0 ) 
			{
				if (  ObjectExist(t.obj) == 1 ) 
				{
					HideObject (  t.obj );
				}
			}
		}
	}
	waypoint_hideall ( );

	// if third person, resurrect ragdoll protagonist
	if ( t.playercontrol.thirdperson.enabled == 1 ) 
	{
		// ensure character protagonist is respawned
		t.e=t.playercontrol.thirdperson.charactere;
		entity_lua_spawn_core ( );
		physics_player_thirdpersonreset ( );
		//PE: Something is disable Z depth buffer when 3rd die ?, just reenable.
		//PE: https://github.com/TheGameCreators/GameGuruRepo/issues/330
		EnableObjectZWrite(t.entityelement[t.e].obj);
		EnableObjectZDepth(t.entityelement[t.e].obj);
	}
}

void physics_player_thirdpersonreset ( void )
{
	//  convert to clone so can animate it (ragdoll makes it an instance)
	t.tentityconverttoclonenotshared=1;
	t.tte=t.e ; entity_converttoclone ( );
	t.tentityconverttoclonenotshared=0;
	t.charanimstate.e=t.e ; t.obj=t.entityelement[t.e].obj;
	entity_setupcharobjsettings ( );
	//  and reset any transitional states in animation system (avoid freezing it)
	t.smoothanim[t.charanimstate.obj].transition=0;
	t.smoothanim[t.charanimstate.obj].playstarted=0;
	//  camera has smoothing in third person, so reset this
	if ( g.luacameraoverride != 1 && g.luacameraoverride != 3 )
	{
		PositionCamera (  t.terrain.gameplaycamera,t.terrain.playerx_f,t.terrain.playery_f,t.terrain.playerz_f );
	}

	// ensure depth is not written so no motion blur
	// apply special character shader so can uniquely change shader constants
	// without affecting other NPCs and trees, etc
	if ( t.obj>0 ) 
	{
		if ( ObjectExist(t.obj) == 1 ) 
		{
			int tttentid = t.entityelement[t.e].bankindex;
			int ttsourceobj = g.entitybankoffset + tttentid;
			if ( ttsourceobj > 0 )
			{
				if ( ObjectExist ( ttsourceobj ) == 1 )
				{
					if ( GetNumberOfFrames ( ttsourceobj ) > 0 )
					{
						// third person is animating
						SetObjectEffect ( t.obj, g.thirdpersoncharactereffect );
						SetEffectConstantF (  g.thirdpersoncharactereffect,"DepthWriteMode",0.0f );			
					}
					else
					{
						// third person is non-animating
						SetObjectEffect ( t.obj, g.thirdpersonentityeffect );
						SetEffectConstantF ( g.thirdpersonentityeffect, "DepthWriteMode", 0.0f );
					}
				}
			}
		}
	}
	//  and also treat the vweap attachment too
	t.tattachmentobj=t.entityelement[t.e].attachmentobj;
	if (  t.tattachmentobj>0 ) 
	{
		if (  ObjectExist(t.tattachmentobj) == 1 ) 
		{
			SetObjectEffect (  t.tattachmentobj,g.thirdpersonentityeffect );
			SetEffectConstantF (  g.thirdpersonentityeffect,"DepthWriteMode",0.0f );
		}
	}
}

bool physics_player_addweapon ( void )
{
	extern int g_iSuggestedSlot;
	//  takes weaponindex
	t.tweaponisnew=0;
	//  check all weapon slots
	t.gotweapon=0;
	for ( t.ws = 1 ; t.ws < 10; t.ws++ )
	{
		if (  t.weaponslot[t.ws].got == t.weaponindex  )  t.gotweapon = t.ws;
	}
	if ( t.gotweapon == 0 ) 
	{
		// check if we have a slot preference
		t.tweaponisnew=1;
		t.gotweaponpref=0;
		for ( t.ws = 1 ; t.ws < 12; t.ws++ )
		{
			if (  t.weaponslot[t.ws].pref == t.weaponindex )  
				t.gotweaponpref = t.ws;
		}
		// check if we are forcing a suggested slot: g_iSuggestedSlot
		if (t.gotweaponpref == 0)
		{
			// is there a suggested slot?
			if (g_iSuggestedSlot > 0)
			{
				if (t.weaponslot[g_iSuggestedSlot].got == 0)
				{
					// check if suggested slot is available (not blocked)
					bool bThisSlotBlocked = false;
					int prefGunID = t.weaponslot[g_iSuggestedSlot].pref;
					if (prefGunID > 0)
					{
						if (stricmp(t.gun[prefGunID].name_s.Get(), "Slot Not Used") == NULL)
						{
							bThisSlotBlocked = true;
						}
					}
					if (bThisSlotBlocked == false )
					{
						// suggested slot is available, use it
						t.gotweaponpref = g_iSuggestedSlot;
					}
				}
			}
		}

		// add weapon
		if (t.gotweaponpref == 0)
		{
			// find free slot
			for ( t.ws = 1 ; t.ws < 10; t.ws++ )
			{
				if (t.weaponslot[t.ws].got == 0 && t.weaponslot[t.ws].pref == 0)
					break;
			}

			// force a slot
			if ( g.forcedslot != 0 )
			{ 
				t.ws = g.forcedslot; 
				t.gotweaponpref = t.ws; 
				g.forcedslot = 0; 
			}

			// LB: superceded with more pref control, just need 't.ws <= 10' to know we have a slot available at this point!
			// count weapons for maximum slots. If exceeded, prevent pick up.
			if ( t.ws < 10 ) 
			{
				// add weapon into free slot and create pref for it
				t.weaponslot[t.ws].pref=t.weaponindex;
				t.weaponhud[t.ws]=t.gun[t.weaponindex].hudimage;

				// mark weapon with 'possible' entity that held this weapon (for equipment activation)
				g.firemodes[t.weaponindex][0].settings.equipmententityelementindex=t.autoentityusedtoholdweapon;
			}
			else
			{
				// no room for weapon in available slots
				t.ws = 0;

				// and can leave right now
				return false;
			}
		}
		else
		{
			t.ws = t.gotweaponpref;
		}

		// switch to collected weapon
		if ( g.autoswap == 1 && t.ws>0 ) 
		{
			// insert as slot weapon
			t.weaponslot[t.ws].got=t.weaponindex;
			t.weaponslot[t.ws].invpos=t.weaponinvposition;
			g.autoloadgun=t.weaponindex;
			t.weaponkeyselection=t.ws;
			t.gotweapon=t.ws;
		}

		//  place details of weapon in slot
		if (  t.ws>0 ) 
		{
			//  insert as slot weapon
			t.weaponslot[t.ws].got=t.weaponindex;
			t.weaponslot[t.ws].invpos=t.weaponinvposition;
			t.gotweapon=t.ws;
			if (  t.gunid == 0 ) 
			{
				//  if no gun held, auto select collected
				g.autoloadgun=t.weaponindex;
				t.weaponkeyselection=t.ws;
			}

			// when slot suggested (editing hot keys, no need for pref init position)
			if (g_iSuggestedSlot > 0 ) t.weaponslot[t.ws].pref = 0; 
		}
	}

	// weapons start with some ammo
	if ( t.gotweapon>0 ) 
	{
		// ammo for weapon
		t.tgunid = t.weaponslot[t.gotweapon].got;
		if ( t.gun[t.tgunid].settings.weaponisammo == 0 )
		{
			// no ammo should be possible
			if (t.tqty < 0) t.tqty = 0; 

			// when new weapon starts, get ammo from store (in case was moved to container and is brought back)
			if (t.tweaponisnew == 1)
			{
				if (t.gotweapon < 10)
				{
					t.weaponammo[t.gotweapon] = t.gun[t.tgunid].storeammo;
					t.weaponclipammo[t.gotweapon] = t.gun[t.tgunid].storeclipammo;
				}
			}

			t.taltqty = 0;
			if (t.gotweapon < 10 && t.weaponammo[t.gotweapon] == 0 && t.tweaponisnew == 1)
			{
				// provide some alternative ammo (weaponammo+10)
				if ( t.gun[t.tgunid].settings.modessharemags == 0 ) 
				{
					//  080415 - only if not sharing ammo
					t.taltqty = t.tqty;
					if (  t.taltqty>g.firemodes[t.tgunid][1].settings.reloadqty )
					{
						t.altpool=g.firemodes[t.tgunid][1].settings.poolindex;
						t.weaponammo[t.gotweapon+10]=g.firemodes[t.tgunid][1].settings.reloadqty;
						if (t.altpool > 0)
						{
							t.ammopool[t.altpool].ammo = t.ammopool[t.altpool].ammo + (t.taltqty - g.firemodes[t.tgunid][1].settings.reloadqty);
							int iMaxClipCapacity = g.firemodes[t.tgunid][1].settings.clipcapacity * g.firemodes[t.tgunid][1].settings.reloadqty;
							if (iMaxClipCapacity == 0) iMaxClipCapacity = 99999;
							if (t.ammopool[t.altpool].ammo > iMaxClipCapacity) t.ammopool[t.altpool].ammo = iMaxClipCapacity;
						}
						else
						{
							t.weaponclipammo[t.gotweapon + 10] = t.taltqty - g.firemodes[t.tgunid][1].settings.reloadqty;
						}
					}
					else
					{
						if (  t.gun[t.tgunid].settings.addtospare == 0 ) 
						{
							t.weaponammo[t.gotweapon+10]=t.taltaty;
						}
						else
						{
							if (  t.gun[t.tgunid].settings.canaddtospare == 1 ) 
							{
								t.altpool=g.firemodes[t.tgunid][1].settings.poolindex;
								int iMaxClipCapacity = g.firemodes[t.tgunid][1].settings.clipcapacity * g.firemodes[t.tgunid][1].settings.reloadqty;
								if (iMaxClipCapacity == 0) iMaxClipCapacity = 99999;
								if (t.altpool == 0)
								{
									t.weaponclipammo[t.gotweapon + 10] = t.weaponclipammo[t.gotweapon + 10] + t.taltqty;
									if (t.weaponclipammo[t.gotweapon + 10] > iMaxClipCapacity) t.weaponclipammo[t.gotweapon + 10] = iMaxClipCapacity;
								}
								else
								{
									t.ammopool[t.altpool].ammo = t.ammopool[t.altpool].ammo + t.taltqty;
									if (t.ammopool[t.altpool].ammo > iMaxClipCapacity) t.ammopool[t.altpool].ammo = iMaxClipCapacity;
								}
							}
							if (  t.gun[t.tgunid].settings.canaddtospare == 0  )  t.weaponammo[t.gotweapon+10] = t.taltqty;
						}
					}
				}
				//  provide some primary ammo
				if (  t.tqty>g.firemodes[t.tgunid][0].settings.reloadqty ) 
				{
					//  gun has MAX slots of ammo, cannot exceed this!
					t.tpool=g.firemodes[t.tgunid][0].settings.poolindex;
					t.weaponammo[t.gotweapon]=g.firemodes[t.tgunid][0].settings.reloadqty;
					if (  t.tpool>0 ) 
					{
						t.ammopool[t.tpool].ammo=t.ammopool[t.tpool].ammo+(t.tqty-g.firemodes[t.tgunid][0].settings.reloadqty);
					}
					else
					{
						t.weaponclipammo[t.gotweapon]=t.tqty-g.firemodes[t.tgunid][0].settings.reloadqty;
					}
				}
				else
				{
					if (  t.gun[t.tgunid].settings.addtospare == 0 ) 
					{
						t.weaponammo[t.gotweapon]=t.tqty;
					}
					else
					{
						//  new gunspec addition "addtospare" this will allow it so picking up ammo
						//  with an empty weapon won't add the ammo directly into the clip
						if (  t.gun[t.tgunid].settings.canaddtospare == 1 ) 
						{
							t.tpool=g.firemodes[t.tgunid][0].settings.poolindex;
							int iMaxClipCapacity = g.firemodes[t.tgunid][0].settings.clipcapacity * g.firemodes[t.tgunid][0].settings.reloadqty;
							if (iMaxClipCapacity == 0) iMaxClipCapacity = 99999;
							if (  t.tpool == 0 )
							{
								t.weaponclipammo[t.gotweapon] = t.weaponclipammo[t.gotweapon] + t.tqty;
								if (t.weaponclipammo[t.gotweapon] > iMaxClipCapacity) t.weaponclipammo[t.gotweapon] = iMaxClipCapacity;
							}
							else
							{
								t.ammopool[t.tpool].ammo=t.ammopool[t.tpool].ammo+t.tqty;
								if (t.ammopool[t.tpool].ammo > iMaxClipCapacity) t.ammopool[t.tpool].ammo = iMaxClipCapacity;
							}
						}
						if (  t.gun[t.tgunid].settings.canaddtospare == 0 ) 
						{
							t.gun[t.tgunid].settings.canaddtospare=1;
							t.weaponammo[t.gotweapon]=t.tqty;
						}
					}
				}
			}
			else
			{
				if (t.gotweapon < 10)
				{
					t.tpool = g.firemodes[t.tgunid][0].settings.poolindex;
					t.altpool = g.firemodes[t.tgunid][1].settings.poolindex;
					int iMaxClipCapacity = g.firemodes[t.tgunid][0].settings.clipcapacity * g.firemodes[t.tgunid][0].settings.reloadqty;
					if (iMaxClipCapacity == 0) iMaxClipCapacity = 99999;
					if (t.tpool == 0)
					{
						t.weaponclipammo[t.gotweapon] = t.weaponclipammo[t.gotweapon] + t.tqty;
						if (t.weaponclipammo[t.gotweapon] > iMaxClipCapacity) t.weaponclipammo[t.gotweapon] = iMaxClipCapacity;
					}
					else
					{
						t.ammopool[t.tpool].ammo = t.ammopool[t.tpool].ammo + t.tqty;
						if (t.ammopool[t.tpool].ammo > iMaxClipCapacity) t.ammopool[t.tpool].ammo = iMaxClipCapacity;
					}
					iMaxClipCapacity = g.firemodes[t.tgunid][1].settings.clipcapacity * g.firemodes[t.tgunid][1].settings.reloadqty;
					if (iMaxClipCapacity == 0) iMaxClipCapacity = 99999;
					if (t.altpool == 0)
					{
						t.weaponclipammo[t.gotweapon + 10] = t.weaponclipammo[t.gotweapon + 10] + t.taltqty;
						if (t.weaponclipammo[t.gotweapon + 10] > iMaxClipCapacity) t.weaponclipammo[t.gotweapon + 10] = iMaxClipCapacity;
					}
					else
					{
						t.ammopool[t.altpool].ammo = t.ammopool[t.altpool].ammo + t.taltqty;
						if (t.ammopool[t.altpool].ammo > iMaxClipCapacity) t.ammopool[t.altpool].ammo = iMaxClipCapacity;
					}
				}
			}
		}
	}

	//  refresh gun count
	physics_player_refreshcount ( );

	//  if collected weapon, and is empty, trigger reload if gun anim able
	if (  t.gotweapon>0 && t.gotweapon < 10)
	{
		t.tgunid=t.weaponslot[t.gotweapon].pref;
		if (  t.weaponammo[t.gotweapon] == 0 ) 
		{
			if (  t.gunmode >= 5 && t.gunmode<31 ) 
			{
				t.gunmode=121;
			}
		}
	}

	// success
	return true;
}

void physics_player_removeweapon ( void )
{
	// check all weapon slots
	for ( t.ws = 1 ; t.ws < 12; t.ws++ )
	{
		if ( t.weaponslot[t.ws].got == t.weaponindex  )  break;
	}
	if (  t.ws < 12 ) 
	{
		// Ensure gun is removed (if applicable)
		if ( t.gunid>0 && t.weaponslot[t.ws].got == t.gunid ) 
		{
			g.autoloadgun=0;
		}
		// drop weapon from slot
		t.weaponslot[t.ws].got=0;
		t.weaponslot[t.ws].invpos=0;
	}

	//  refresh gun count
	physics_player_refreshcount ( );
}

void physics_player_resetWeaponSlots( void )
{
	for (t.ws = 1; t.ws < 12; t.ws++)
	{
		t.weaponslot[t.ws].got = 0;
		t.weaponslot[t.ws].invpos = 0;
	}
}

void physics_player_refreshcount ( void )
{
	//  refresh gun count
	t.guncollectedcount=0;
	for ( t.ws = 1 ; t.ws < 10; t.ws++ )
	{
		if (  t.weaponslot[t.ws].got>0  )  ++t.guncollectedcount;
	}

	// Trigger zoom out so that the player doesn't stay zoomed in when picking up a new weapon
	if (t.gunzoommode == 9 || t.gunzoommode == 10)
	{
		t.gunzoommode = 11;
	}

	return;
}

void physics_clear_debug_draw(void)
{
	for (int i = 0; i < t.iPhysicsDebugObjects.size(); i++)
	{
		if (ObjectExist(t.iPhysicsDebugObjects[i])) DeleteObject(t.iPhysicsDebugObjects[i]);
	}
	t.iPhysicsDebugObjects.clear();
	t.iPhysicsDebugObjectsToUpdate.clear();
	t.iPhysicsCreatedDynamicMesh = 0;
	t.iPhysicsCreatedStaticMesh = 0;
	t.iPhysicsDebugMaxOffset = 0;
	t.physicsDebugDrawData = nullptr;
	t.iPhysicsDebugDynamicOffsets.clear();
	BPhys_ClearDebugDrawData();
}

// Set the drawing mode of the physics debug drawer.
void physics_set_debug_draw(int iDraw)
{
	// can force a cleanup if statics flag toggled
	if (iDraw == 1) physics_clear_debug_draw();

	// set debug draw flags
	BPhys_SetDebugDrawerMode(iDraw, t.visuals.iPhysicsDebugDrawStatics, t.visuals.iPhysicsDebugDrawConstraints);

	// Clean up.
	if (!iDraw) physics_clear_debug_draw();
}

void physics_create_debug_mesh(float* data, int count, bool bStatic, int offset)
{
	if (!data) return;

	// Find a free memblock.
	int iFound = 0;
	for (int i = 1; i <= 257; i++)
	{
		if (MemblockExist(i) == 0)
		{
			iFound = i;
			break;
		}
	}
	if (iFound == 0) return;

	// Find a free object slot.
	int obj = -1;
	for (int i = g.physicsdebugdraweroffset; i < g.physicsdebugdraweroffset + 200; i++)
	{
		if (ObjectExist(i) == 0)
		{
			obj = i;
			break;
		}
	}
	if (obj < 0) return;

	int vertsize = 32;
	int iSizeBytes = 0;
	int vertexCount = count / 3;
	vertexCount *= 9;
	iSizeBytes = vertsize * vertexCount;
	iSizeBytes += 12; // Add header bytes.

	// if memblock creation fails, try smaller size so we see something!
	if (iSizeBytes > 0)
	{
		for (int iTries = 0; iTries < 6; iTries++)
		{
			MakeMemblock(iFound, iSizeBytes);
			if (MemblockExist(iFound) == 0)
			{
				// try half that
				count /= 2;
				vertexCount = count / 3;
				vertexCount *= 9;
				iSizeBytes = vertsize * vertexCount;
				iSizeBytes += 12;
			}
			else
			{
				// memblock creation successful
				break;
			}
		}
	}

	// Write the memblock header.
	if (MemblockExist(iFound) == 1)
	{
		// FVF format.
		WriteMemblockDWord(iFound, 0, GGFVF_XYZ | GGFVF_NORMAL | GGFVF_TEX1);
		// Size of single vertex - 3 x float: position, 3 x float: normal,1 x DWORD: diffuse, 2 x float: tex coords = 36 bytes.
		WriteMemblockDWord(iFound, 4, 12 + 12 + 8);
		// Number of vertices in the mesh.
		WriteMemblockDWord(iFound, 8, vertexCount);

		float x0, x1, x2, x3, x4, x5;
		float y0, y1, y2, y3, y4, y5;
		float z0, z1, z2, z3, z4, z5;
		int v = 0;
		float p0[3];
		float p1[3];
		float points[18];

		// Every 6 elements of data contain two points on the physics object.
		// data can contain the vertices of a previously created mesh, so start at offset (count at the last mesh creation).
		for (int i = offset; i <= count - 6; i += 6)
		{
			p0[0] = data[i]; p0[1] = data[i + 1]; p0[2] = data[i + 2];
			p1[0] = data[i + 3]; p1[1] = data[i + 4]; p1[2] = data[i + 5];

			physics_debug_make_prism_between_points(p0, p1, points);

			// Corners of the prism.
			x0 = points[0]; y0 = points[1]; z0 = points[2];
			x1 = points[3]; y1 = points[4]; z1 = points[5];
			x2 = points[6]; y2 = points[7]; z2 = points[8];
			x3 = points[9]; y3 = points[10]; z3 = points[11];
			x4 = points[12]; y4 = points[13]; z4 = points[14];
			x5 = points[15]; y5 = points[16]; z5 = points[17];

			// Form the faces of the prism from the corners.
			physics_add_vert_to_debug_mesh(x0, y0, z0, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x2, y2, z2, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x3, y3, z3, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x2, y2, z2, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x4, y4, z4, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x3, y3, z3, v, iFound);
			v++;

			physics_add_vert_to_debug_mesh(x1, y1, z1, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x2, y2, z2, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x4, y4, z4, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x2, y2, z2, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x5, y5, z5, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x4, y4, z4, v, iFound);
			v++;

			physics_add_vert_to_debug_mesh(x0, y0, z0, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x1, y1, z1, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x5, y5, z5, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x0, y0, z0, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x5, y5, z5, v, iFound);
			v++;
			physics_add_vert_to_debug_mesh(x3, y3, z3, v, iFound);
			v++;
		}

		// Keep track of the newly created object so we can delete it later.
		t.iPhysicsDebugObjects.push_back(obj);

		// Dynamic objects will need their vertices updated to prevent recreating the mesh each frame.
		if (!bStatic)
		{
			t.iPhysicsDebugObjectsToUpdate.push_back(obj);
			t.iPhysicsDebugDynamicOffsets.push_back(offset);
		}

		WickedCall_PresetObjectCreateOnDemand(true);
		CreateMeshFromMemblock(obj, iFound);
		MakeObject(obj, obj, 0);
		WickedCall_PresetObjectCreateOnDemand(false);

		sObject* pObject = GetObjectData(obj);
		int verts = pObject->ppMeshList[0]->dwVertexCount;

		SetObject(obj, 0, 0, 0, 0, 0, 0, 0);
		SetObjectCollisionOff(obj);
		DisableObjectZWrite(obj);
		SetObjectLight(obj, 0);
		SetObjectMask(obj, 1);

		// 150817 - GUI shader with DIFFUSE element included
		SetObjectEffect(obj, g.guidiffuseshadereffectindex);

		WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_CURSOROBJECT);
		WickedCall_RemoveObject(pObject);
		WickedCall_AddObject(pObject);
		WickedCall_TextureObject(pObject, NULL);
		WickedCall_SetObjectCastShadows(pObject, false);
		WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_NORMAL);
		// wicked object does not use per-vertex diffuse color, so find first color and apply to whole object
		DWORD diffuse = Rgb(0, 255, 0);
		SetObjectDiffuse(obj, Rgb(0, 255, 0));

		// set alpha and transparency of this object
		SetObjectTransparency(obj, 2);
		SetAlphaMappingOn(obj, 75);// 25 );

		DeleteMemblock(iFound);
	}
}

void physics_add_vert_to_debug_mesh(float x, float y, float z, int v, int memblock)
{
	//  Position of vertex in memblock
	int pos = 12 + (v * 32);// 12);

	//  Set vertex position
	WriteMemblockFloat(memblock, pos + 0, x);
	WriteMemblockFloat(memblock, pos + 4, y);
	WriteMemblockFloat(memblock, pos + 8, z);
}

void physics_update_debug_mesh(float* data, int count, int objectID, int offsetLower, int offsetUpper)
{
	float x0, x1, x2, x3, x4, x5;
	float y0, y1, y2, y3, y4, y5;
	float z0, z1, z2, z3, z4, z5;
	int v = 0;
	float p0[3];
	float p1[3];
	float points[18];

	if (!data) return;

	sObject* pObject = GetObjectData(objectID);
	if (!pObject)
	{
		return;
	}
	
	sMesh* pMesh = pObject->ppMeshList[0];
	if (!pMesh)
	{
		return;
	}
	
	LockVertexDataForLimbCore(objectID, 0, 1);
	for (int i = offsetLower; i <= offsetUpper - 6; i += 6)
	{
		// Every 6 elements of data contain two points on the physics object.
		p0[0] = data[i]; p0[1] = data[i + 1]; p0[2] = data[i + 2];
		p1[0] = data[i + 3]; p1[1] = data[i + 4]; p1[2] = data[i + 5];

		physics_debug_make_prism_between_points(p0, p1, points);

		// Corners of the prism.
		x0 = points[0]; y0 = points[1]; z0 = points[2];
		x1 = points[3]; y1 = points[4]; z1 = points[5];
		x2 = points[6]; y2 = points[7]; z2 = points[8];
		x3 = points[9]; y3 = points[10]; z3 = points[11];
		x4 = points[12]; y4 = points[13]; z4 = points[14];
		x5 = points[15]; y5 = points[16]; z5 = points[17];

		// Make each face of the prism (two triangles per face).
		SetVertexDataPosition(v++, x0, y0, z0);
		SetVertexDataPosition(v++, x2, y2, z2);
		SetVertexDataPosition(v++, x3, y3, z3);
		SetVertexDataPosition(v++, x2, y2, z2);
		SetVertexDataPosition(v++, x4, y4, z4);
		SetVertexDataPosition(v++, x3, y3, z3);
							 
		SetVertexDataPosition(v++, x1, y1, z1);
		SetVertexDataPosition(v++, x2, y2, z2);
		SetVertexDataPosition(v++, x4, y4, z4);
		SetVertexDataPosition(v++, x2, y2, z2);
		SetVertexDataPosition(v++, x5, y5, z5);
		SetVertexDataPosition(v++, x4, y4, z4);
							 
		SetVertexDataPosition(v++, x0, y0, z0);
		SetVertexDataPosition(v++, x1, y1, z1);
		SetVertexDataPosition(v++, x5, y5, z5);
		SetVertexDataPosition(v++, x0, y0, z0);
		SetVertexDataPosition(v++, x5, y5, z5);
		SetVertexDataPosition(v++, x3, y3, z3);
	}

	UnlockVertexData();

	WickedCall_UpdateMeshVertexData(pMesh);
}

void physics_debug_make_prism_between_points(float* p0, float* p1, float* results, float thickness)
{
	XMFLOAT3 a;
	XMFLOAT3 b;
	XMVECTOR positionA;
	XMVECTOR direction;
	XMVECTOR normal;
	XMFLOAT3 worldUp;
	XMVECTOR temp;
	XMVECTOR dot;
	XMVECTOR right;
	XMVECTOR up;

	// for terrain physics debug!
	XMVECTOR points[6];

	// Calculate direction between the two points.
	a = XMFLOAT3(p0[0], p0[1], p0[2]);
	b = XMFLOAT3(p1[0], p1[1], p1[2]);
	direction = XMLoadFloat3(&b) - XMLoadFloat3(&a);
	normal = XMVector3Normalize(direction);

	// Calculate a vector that allows us to get another vector perpendicular to the direction vector.
	worldUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	temp = XMLoadFloat3(&worldUp);
	dot = XMVector3Dot(normal, temp);
	positionA = XMLoadFloat3(&a);
	if (XMVectorGetX(dot) >= 0.98f || XMVectorGetX(dot) <= -0.98f)
	{
		// Normal and temp have the same direction, so change temp.
		worldUp = XMFLOAT3(1.0f, 0.0f, 0.0f);
		temp = XMLoadFloat3(&worldUp);
	}

	// Calculate the other two basis vectors, giving 3 orthogonal vectors.
	right = XMVector3Cross(normal, temp);
	right = XMVector3Normalize(right);

	up = XMVector3Cross(right, normal);
	up = XMVector3Normalize(up);

	// Calculate 3 points about the normal vector (that pass through the right and up vectors)
	for (int j = 0; j < 3; j++)
	{
		// 2.0944 radians = 120 degrees for 3 points around the positionA.
		points[j] = positionA + (thickness * cosf(2.0944f * j) * right) + (thickness * sinf(2.0944f * j) * up);

		// Calculate the corresponding point on the other triangle by moving the first point towards the second.
		points[j + 3] = points[j] + direction;
	}

	for (int j = 0; j < 6; j++)
	{
		results[3 * j + 0] = XMVectorGetX(points[j]);
		results[3 * j + 1] = XMVectorGetY(points[j]);
		results[3 * j + 2] = XMVectorGetZ(points[j]);
	}
}

// For editor only.
void physics_debug_add_object(int objectID)
{
	if(BPhys_GetDebugObjectCount() == 0) ODEStart();
	BPhys_AddDebugSingleObject(objectID);
	t.tphyobj = objectID;
	t.entid = objectID;
	physics_importer_create_temp();
}

// For editor only.
void physics_debug_remove_object(int objectID)
{
	BPhys_RemoveDebugSingleObject(objectID);
	if (BPhys_GetDebugObjectCount() == 0) ODEEnd();
}

// For editor only.
void physics_debug_draw()
{
	BPhys_DrawDebugObjects();
	physics_render_debug_meshes();
}

// For test game and editor.
void physics_render_debug_meshes()
{
	if (BPhys_GetDebugDrawerMode() != 0)
	{
		int elementCount = 0;

		if (t.iPhysicsCreatedStaticMesh == 0)
		{
			// Get all of the points in the static physics geometry.
			float* data = BPhys_GetStaticDebugDrawData(elementCount);
			if (elementCount > 0)
			{
				// Create the static physics mesh (if any)
				physics_create_debug_mesh(data, elementCount, true, 0);
			}

			// Flag set to ensure we only get the static geometry once.
			t.iPhysicsCreatedStaticMesh = 1;

			// Clear the static physics data so we can get the dynamic data.
			BPhys_ClearDebugDrawData();
		}
		else
		{
			// Get the updated debug data.
			t.physicsDebugDrawData = BPhys_GetDynamicDebugDrawData(elementCount);

			if (t.iPhysicsCreatedDynamicMesh == 0)
			{
				// Create the debug mesh if it hasn't already been created.
				physics_create_debug_mesh(t.physicsDebugDrawData, elementCount, false, t.iPhysicsDebugMaxOffset);
				t.iPhysicsDebugMaxOffset = elementCount;
				t.iPhysicsCreatedDynamicMesh = 1;
			}
			else
			{
				// Update all of the debug meshes.
				for (int i = 0; i < t.iPhysicsDebugObjectsToUpdate.size(); i++)
				{
					int iOffsetUpper = elementCount;
					if (i < t.iPhysicsDebugObjectsToUpdate.size() - 1) iOffsetUpper = t.iPhysicsDebugDynamicOffsets[i + 1];

					physics_update_debug_mesh(t.physicsDebugDrawData, elementCount, t.iPhysicsDebugObjectsToUpdate[i],
						t.iPhysicsDebugDynamicOffsets[i], iOffsetUpper);
				}

				if (elementCount > t.iPhysicsDebugMaxOffset)
				{
					// Object has been added to the physics world, so a new physics mesh must be created.
					t.iPhysicsCreatedDynamicMesh = 0;
				}
			}
		}
	}
}

void physics_importer_create_temp()
{
	if (t.importer.collisionshape == 0) t.tshape = 0;   // box
	if (t.importer.collisionshape == 1) t.tshape = 1;   // polygon
	if (t.importer.collisionshape == 2) t.tshape = 2;   // sphere
	if (t.importer.collisionshape == 3) t.tshape = 3;   // cylinder
	if (t.importer.collisionshape == 4) t.tshape = 9;   // hull
	if (t.importer.collisionshape == 5) t.tshape = 21;  // character collission
	if (t.importer.collisionshape == 6) t.tshape = 50;  // tree collision
	if (t.importer.collisionshape == 7) t.tshape = 11;  // no collision

	if (t.tstatic == 1) // now allow physics entities in multiplayer || t.game.runasmultiplayer == 1 ) 
	{
		//  create the physics now
		if (t.tshape >= 1000 && t.tshape < 2000)
		{
			ODECreateStaticBox(t.tphyobj, t.tshape - 1000);
		}
		else if (t.tshape >= 2000 && t.tshape < 3000)
		{
			ODECreateStaticTriangleMesh(t.tphyobj, t.tshape - 2000);
		}
		else if (t.tshape == 1)
		{
			ODECreateStaticBox(t.tphyobj);
		}
		else if (t.tshape == 6)
		{
			ODECreateStaticSphere(t.tphyobj);
		}
		else if (t.tshape == 7)
		{
			ODECreateStaticCylinder(t.tphyobj);
		}
		else if (t.tshape == 2 || t.tshape == 9 || t.tshape == 10)
		{
			if (t.tshape == 2)
			{
				if (t.tcollisionscaling != 100)
				{
					ODECreateStaticTriangleMesh(t.tphyobj, -1, t.tcollisionscaling);
				}
				else
				{
					ODECreateStaticTriangleMesh(t.tphyobj);
				}
			}
			else
			{
				if (t.tshape == 10)
				{
					ODECreateStaticTriangleMesh(t.tphyobj, -1, t.tcollisionscaling, 2);
				}
				else
				{
					ODECreateStaticTriangleMesh(t.tphyobj, -1, t.tcollisionscaling, 1);
				}
			}
		}
		else if (t.tshape == 3)
		{
			physics_setuptreecylinder();
		}
		// tshape 4 is a list of physics objects from the importer
		else if (t.tshape == 4)
		{
			physics_setupimportershapes();
		}
		// if static, restore object before leaving
		if (t.tstatic == 1)
		{
			RotateObject(t.tphyobj, ObjectAngleX(t.tphyobj), t.tstaticfixnewystore_f, ObjectAngleZ(t.tphyobj));
		}
	}
	else
	{
		// objects will fall through Floor (  if they are perfectly sitting on it )
		PositionObject(t.tphyobj, ObjectPositionX(t.tphyobj), ObjectPositionY(t.tphyobj) + 0.1, ObjectPositionZ(t.tphyobj));

		if (t.tshape == 6)
		{
			// Sphere
			ODECreateDynamicSphere(t.tphyobj, t.tweight, t.tfriction, 0.01f);
		}
		else if (t.tshape == 7)
		{
			// Cylinder
			ODECreateDynamicCylinder(t.tphyobj, t.tweight, t.tfriction, 0.01f);
		}
		else if (t.tshape == 9)
		{
			// Dynamic convex hull
			ODECreateDynamicTriangleMesh(t.tphyobj, t.tweight, t.tfriction, -1, 1);
		}
		else if (t.tshape == 10)
		{
			// Dynamic hull composition
			ODECreateDynamicTriangleMesh(t.tphyobj, t.tweight, t.tfriction, -1, 2);
		}
		else
		{
			// box
			ODECreateDynamicBox(t.tphyobj, -1, 0, t.tweight, t.tfriction, -1);
		}
	}
}

int physics_getmaterialindex (float fX, float fZ)
{
	int iMatID = GGTerrain_GetMaterialIndex(fX, fZ) & 0xFF;
	int iMaterialIndex = 0;
	if (iMatID >= 0 && iMatID < 32) iMaterialIndex = g_iMapMatIDToMatIndex[iMatID];
	return iMaterialIndex;
}

int physics_rayintersecttree (float fX, float fY, float fZ, float fToX, float fToY, float fToZ)
{
	float fHeightOfTreeDetect = 200.0f; //LB: Can be improved with geometry awareness (slower)
	for (int vti = 0; vti < g_VTreeObj.size(); vti++)
	{
		bool bRayTooLowOrHigh = false;
		if (fY < g_VTreeObj[vti].fY && fToY < g_VTreeObj[vti].fY) bRayTooLowOrHigh = true;
		if (fY > g_VTreeObj[vti].fY + fHeightOfTreeDetect && fToY > g_VTreeObj[vti].fY + fHeightOfTreeDetect) bRayTooLowOrHigh = true;
		if (bRayTooLowOrHigh==false)
		{
			// ray crosses Y area presence of tree
			float fCX = g_VTreeObj[vti].fX;
			float fCZ = g_VTreeObj[vti].fZ;
			float r = 15.0f;
			double x0 = fCX, y0 = fCZ;
			double x1 = fX, y1 = fZ;
			double x2 = fToX, y2 = fToZ;
			double A = y2 - y1;
			double B = x1 - x2;
			double C = x2 * y1 - x1 * y2;
			double a = (A*A) + (B*B);
			double b, c, d;
			const double eps = 1e-14;
			if (fabs(B) >= eps) 
			{
				b = 2 * (A * C + A * B * y0 - (B*B) * x0);
				c = (C*C) + 2 * B * C * y0 - (B*B) * ((r*r) - (x0*x0) - (y0*y0));
			}
			else 
			{
				b = 2 * (B * C + A * B * x0 - (A*A) * y0);
				c = (C*C) + 2 * A * C * x0 - (A*A) * ((r*r) - (x0*x0) - (y0*y0));
			}
			d = (b*b) - 4 * a * c;
			if (d > 0 )
			{
				return 1; // hit a tree
			}
		}
	}
	return 0;
}
