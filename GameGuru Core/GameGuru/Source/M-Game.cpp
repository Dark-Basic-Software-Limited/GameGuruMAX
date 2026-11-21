//----------------------------------------------------
//--- GAMEGURU - M-Game
//----------------------------------------------------

#include "stdafx.h"
#include "gameguru.h"
#include "cOccluderThread.h"
#include "CGfxC.h"
#include "DarkLUA.h"
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include "master.h"

//PE: GameGuru IMGUI.
#include "..\Imgui\imgui.h"
#include "..\Imgui\imgui_impl_win32.h"
#include "..\Imgui\imgui_gg_dx11.h"

#include ".\\..\..\\Guru-WickedMAX\\GPUParticles.h"
using namespace GPUParticles;
#include "GGTerrain\GGTerrain.h"
#include "GGTerrain\GGTrees.h"
using namespace GGTerrain;
using namespace GGTrees;
#include "GGRecastDetour.h"
GGRecastDetour g_RecastDetour;
bool g_bShowRecastDetourDebugVisuals = false;
int old_render_params2 = 0;

#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

extern int g_Storyboard_First_Level_Node;
extern int g_Storyboard_Current_Level;
extern bool g_Storyboard_Starting_New_Level;
extern char g_Storyboard_First_fpm[256];
extern char g_Storyboard_Current_fpm[256];
extern char g_Storyboard_Current_lua[256];
extern char g_Storyboard_Current_Loading_Page[256];
extern StoryboardStruct Storyboard;
// Externs
extern bool g_occluderOn;
extern bool	g_occluderf9Mode;

extern bool g_bInTutorialMode;

// Globals
bool g_bInEditor = true;
int g_iMasterRootState = 0;
int g_iActivelyUsingVRNow = 0;
int g_iInGameMenuState = 0;
extern Master master;
bool g_bResetHasForLevelGeneration = false;

// 
//  Game Module to manage all game flow
// 

extern int iLaunchAfterSync;
extern bool commonexecutable_loop_for_game(void);

void gameexecutable_init(void)
{
	// start game init code
	int iEXEGameIsVR = 0;

	//PE: Load in any imgui media used in standalone, special mode tabtab...
	SetMipmapNum(1); //PE: mipmaps not needed.
	image_setlegacyimageloading(true);
	LoadImage("editors\\uiv3\\ccp-none.png", CCP_NONE);
	LoadImage("editors\\uiv3\\ccp-empty.png", CCP_EMPTY);
	image_setlegacyimageloading(false);
	SetMipmapNum(-1);

	extern bool bSpecialStandalone;
	if (bSpecialStandalone)
	{
		//PE: No VR when running demo games.
		editor_previewmap_initcode(0);
	}
	else
	{
		editor_previewmap_initcode(iEXEGameIsVR);
	}
	iLaunchAfterSync = 201;
}

void gameexecutable_loop(void)
{
	// loop with special modes used when in test game or standalone game
	commonexecutable_loop_for_game();
}

void gameexecutable_finish(void)
{
	// Free before exit app
	mp_free ( );
}

// The occluder thread
cOccluderThread*	g_pOccluderThread = NULL;
float				g_fOccluderCamVelX = 0.0f;
float				g_fOccluderCamVelZ = 0.0f;
float				g_fOccluderLastCamX = 0.0f;
float				g_fOccluderLastCamZ = 0.0f;

void game_scanfornewavatars ( bool bDynamicallyRecreateCharacters )
{
	// add any character creator player avatars in
	if ( t.bTriggerAvatarRescanAndLoad == true )
	{
		for ( t.tcustomAvatarCount = 0 ; t.tcustomAvatarCount <= MP_MAX_NUMBER_OF_PLAYERS-1; t.tcustomAvatarCount++ )
		{
			// check if there is a custom avatar (and not loaded)
			if ( t.mp_playerAvatars_s[t.tcustomAvatarCount] != "" && t.mp_playerAvatarLoaded[t.tcustomAvatarCount] == false ) 
			{
				// there is so lets built a temp fpe file from it
				t.ent_s = g.rootdir_s+"entitybank\\user\\charactercreatorplus\\customAvatar_"+Str(t.tcustomAvatarCount)+".fpe";
				t.avatarFile_s = t.ent_s;
				t.avatarString_s = t.mp_playerAvatars_s[t.tcustomAvatarCount];
				characterkitplus_makeMultiplayerCharacterCreatorAvatar ( );
				entity_addtoselection_core ( );
				characterkitplus_removeMultiplayerCharacterCreatorAvatar ( );
				t.tubindex[t.tcustomAvatarCount+2]=t.entid;
				t.entityprofile[t.tubindex[t.tcustomAvatarCount+2]].ischaracter=0;
				t.entityprofile[t.tubindex[t.tcustomAvatarCount+2]].collisionmode=12;
				// No lua script for player chars
				t.entityprofile[t.tubindex[t.tcustomAvatarCount+2]].aimain_s = "";
				// avatar is now loaded
				t.mp_playerAvatarLoaded[t.tcustomAvatarCount] = true;

				// additionally, when triggered, replace actual objects with new created ones above (for dynamic loading)
				if ( bDynamicallyRecreateCharacters == true )
				{
					t.e = t.mp_playerEntityID[t.tcustomAvatarCount];
					if ( t.e > 0 )
					{
						// update entity element with new character object (dynamically loaded during game)
						t.entityelement[t.e].bankindex = t.entid;

						// update entity object itself
						t.tupdatee = t.e; 
						entity_updateentityobj ( );
					}
				}
			}
		}
		t.bTriggerAvatarRescanAndLoad = false;

		// refreshes object masks of avatar heads
		t.visuals.refreshshaders = 1;
	}
}

#ifdef WIP_PROLOADLEVELTEXTURES
std::vector<std::string> preload_setup;
#endif


uint32_t GetObjectNavMeshVertexCount( int iID )
{
	if ( !ConfirmObject ( iID ) ) return 0;

	sObject* pObject = g_ObjectList [ iID ];

	uint32_t numTotalVertices = 0;
	
	for (int iFrameIndex = 0; iFrameIndex < pObject->iFrameCount; iFrameIndex++)
	{
		sFrame* pFrame = pObject->ppFrameList[iFrameIndex];
		if (pFrame)
		{
			sMesh* pMesh = pFrame->pMesh;
			if (pMesh)
			{
				if (pMesh->dwIndexCount == 0)
				{
					numTotalVertices += pMesh->dwVertexCount;
				}
				else
				{
					// has index data
					numTotalVertices += pMesh->dwIndexCount;
				}
			}
		}
	}

	return numTotalVertices;
}

void GetObjectNavMeshVertices( int iID, float* pVertices )
{
	if ( !ConfirmObject ( iID ) ) return;

	sObject* pObject = g_ObjectList [ iID ];

	uint32_t numTotalVertices = 0;

	// faces
	for (int iFrameIndex = 0; iFrameIndex < pObject->iFrameCount; iFrameIndex++)
	{
		sFrame* pFrame = pObject->ppFrameList[iFrameIndex];
		if (pFrame)
		{
			sMesh* pMesh = pFrame->pMesh;
			if (pMesh)
			{			
				GGMATRIX matThisFrame;
				GGMatrixTranslation	(&matThisFrame, pFrame->vecOffset.x, pFrame->vecOffset.y, pFrame->vecOffset.z);
				
				float* pVertData = (float*) pMesh->pVertexData;
				uint32_t stride = pMesh->dwFVFSize / 4;

				if (pMesh->dwIndexCount == 0)
				{
					// has no indice data
					
					for (DWORD dwV = 0; dwV < pMesh->dwVertexCount; dwV ++)
					{
						GGVECTOR3 vecXYZ = GGVECTOR3( pVertData[0], pVertData[1], pVertData[2] );
						GGVec3TransformCoord(&vecXYZ, &vecXYZ, &matThisFrame);

						uint32_t index = (numTotalVertices + dwV) * 3;

						pVertices[ index + 0 ] = vecXYZ.x;
						pVertices[ index + 1 ] = vecXYZ.y;
						pVertices[ index + 2 ] = vecXYZ.z;

						pVertData += stride;
					}

					numTotalVertices += pMesh->dwVertexCount;
				}
				else
				{
					// has indice data
					for (DWORD dwI = 0; dwI < pMesh->dwIndexCount; dwI++ )
					{
						DWORD dwV = pMesh->pIndices[ dwI ];
						
						uint32_t vIndex = dwV * stride;
						GGVECTOR3 vecXYZ = GGVECTOR3( pVertData[vIndex], pVertData[vIndex + 1], pVertData[vIndex + 2] );
						GGVec3TransformCoord(&vecXYZ, &vecXYZ, &matThisFrame);

						uint32_t index = (numTotalVertices + dwI) * 3;

						pVertices[ index + 0 ] = vecXYZ.x;
						pVertices[ index + 1 ] = vecXYZ.y;
						pVertices[ index + 2 ] = vecXYZ.z;
					}

					numTotalVertices += pMesh->dwIndexCount;
				}
			}
		}
	}
}

//PE: save raw vertices.
bool saveVertices(const std::string& filename, float* pVertices, int numVertices)
{

	std::ofstream outFile(filename, std::ios::binary);

	if (!outFile)
	{
		return false;
	}

	outFile.write(reinterpret_cast<const char*>(&numVertices), sizeof(int));

	outFile.write(reinterpret_cast<const char*>(pVertices), sizeof(float) * numVertices * 3);

	outFile.close();

	return true;
}

//PE: Load raw vertices.
float* loadVertices(const std::string& filename, int& outNumVertices)
{
	std::ifstream inFile(filename, std::ios::binary);

	if (!inFile)
	{
		return nullptr;
	}

	inFile.read(reinterpret_cast<char*>(&outNumVertices), sizeof(int));

	float* pVertices = new float[outNumVertices * 3];

	inFile.read(reinterpret_cast<char*>(pVertices), sizeof(float) * outNumVertices * 3);

	inFile.close();

	return pVertices;
}

void game_createnavmeshfromlevel ( bool bForceGeneration )
{
	bool bStandalone = bForceGeneration;

	if (bStandalone)
	{
		//PE: Check if we got the raw pVertices size numVertices,  under fpm name.
		cstr importer_getfilenameonly(LPSTR pFileAndPossiblePath);
		cstr filenameonly = importer_getfilenameonly(g.projectfilename_s.Get());
		std::string newfilename = filenameonly.Get();
		replaceAll(newfilename, ".fpm", ".raw");
		DARKSDK LPSTR GetDir(void);
		//PE: Save into standalone folder.
		std::string fullname = (char *) GetDir();
		fullname = fullname + "\\navbank\\";
		CreateDirectoryA(fullname.c_str(), NULL);
		fullname = fullname + newfilename;

		int loadedCount = 0;
		float* pLoadedVertices = loadVertices(fullname, loadedCount);

		if (pLoadedVertices && loadedCount > 0)
		{
			timestampactivity(0, "Using cached vertices to build the nav mesh");
			g_RecastDetour.buildall(pLoadedVertices, loadedCount);
			if (pLoadedVertices) delete[] pLoadedVertices;
			return;
		}
	}
	// area around any entities on map
	GGVECTOR3 vecMinArea = GGVECTOR3( 999999,  999999,  999999);
	GGVECTOR3 vecMaxArea = GGVECTOR3(-999999, -999999, -999999);
	float editableSize = GGTerrain_GetEditableSize();

	// find any NAVMESH LIMIT flags
	bool bUsingNavMeshLimitCustomArea = false;
	GGVECTOR3 vecCustomPlayAreaMin = GGVECTOR3(999999, 999999, 999999);
	GGVECTOR3 vecCustomPlayAreaMax = GGVECTOR3(-999999, -999999, -999999);
	for (int e = 1; e <= g.entityelementlist; e++)
	{
		int entid = t.entityelement[e].bankindex;
		if (t.entityprofile[entid].ismarker == 11 )
		{
			if (stricmp(t.entityelement[e].eleprof.name_s.Get(), "navmesh limit") == NULL)
			{
				if (t.entityelement[e].x > vecCustomPlayAreaMax.x) vecCustomPlayAreaMax.x = t.entityelement[e].x;
				if (t.entityelement[e].z > vecCustomPlayAreaMax.z) vecCustomPlayAreaMax.z = t.entityelement[e].z;
				if (t.entityelement[e].x < vecCustomPlayAreaMin.x) vecCustomPlayAreaMin.x = t.entityelement[e].x;
				if (t.entityelement[e].z < vecCustomPlayAreaMin.z) vecCustomPlayAreaMin.z = t.entityelement[e].z;
				bUsingNavMeshLimitCustomArea = true;
			}
		}
	}

	// work out hash for all "static" objects
	double dSuperHash = 0;
	for (int e = 1; e <= g.entityelementlist; e++)
	{
		int entid = t.entityelement[e].bankindex;

		// build the bounding box using both static objects and characters, in case there are no static objects near a character
		bool bNeedThisToHaveNavMesh = false;
		if ( t.entityelement[e].staticflag == 1) bNeedThisToHaveNavMesh = true;
		if ( t.entityprofile[entid].ischaracter == 1) bNeedThisToHaveNavMesh = true;
		if ( t.entityprofile[entid].ismarker == 11) bNeedThisToHaveNavMesh = true;
		if ( bNeedThisToHaveNavMesh == true)
		{
			int iObj = t.entityelement[e].obj;
			if (iObj > 0)
			{
				if (ObjectExist(iObj) == 1)
				{
					// establish bounds of static objects
					GGVECTOR3 vecPos = GGVECTOR3(ObjectPositionX(iObj), ObjectPositionY(iObj), ObjectPositionZ(iObj));
					if ( vecPos.x > editableSize || vecPos.x < -editableSize || vecPos.z > editableSize || vecPos.z < -editableSize ) continue;

					// extra feature called NAVMESH LIMIT (assign this name to a flag and place)
					// which enables the nav mesh area to be customized, allowing objects outside to be placed for scenery
					if (bUsingNavMeshLimitCustomArea == true)
					{
						if( vecPos.x < vecCustomPlayAreaMin.x ||
							vecPos.x > vecCustomPlayAreaMax.x ||
							vecPos.z < vecCustomPlayAreaMin.z ||
							vecPos.z > vecCustomPlayAreaMax.z )
						{
							// this object outside of custom navmesh limit area, can ignore (done again below)
							continue;
						}
					}

					if (vecPos.x > vecMaxArea.x) vecMaxArea.x = vecPos.x;
					if (vecPos.z > vecMaxArea.z) vecMaxArea.z = vecPos.z;
					if (vecPos.x < vecMinArea.x) vecMinArea.x = vecPos.x;
					if (vecPos.z < vecMinArea.z) vecMinArea.z = vecPos.z;

					// calculating superhash
					dSuperHash += iObj;
					dSuperHash += vecPos.x;
					dSuperHash += vecPos.y;
					dSuperHash += vecPos.z;
					dSuperHash += ObjectAngleX(iObj);
					dSuperHash += ObjectAngleY(iObj);
					dSuperHash += ObjectAngleZ(iObj);
					dSuperHash += ObjectScaleX(iObj);
					dSuperHash += ObjectScaleY(iObj);
					dSuperHash += ObjectScaleZ(iObj);
				}
			}
		}
	}
		
	if (dSuperHash == 0)
	{
		// failing all else, just use camera position
		vecMinArea.x = vecMaxArea.x = CameraPositionX(0);
		vecMinArea.z = vecMaxArea.z = CameraPositionZ(0);
	}

	// toggling trees means recalcing for tree obstacles
	dSuperHash += ggtrees_global_params.draw_enabled;

	// toggling trees means recalcing for tree obstacles
	dSuperHash += (int)t.terrain.waterliney_f;

	// must still reset nav mesh system for new run (blocker list)
	g_RecastDetour.ResetBlockerSystem();
	g_RecastDetour.ResetTokenDropSystem();
	g_RecastDetour.SetWaterTableY(t.terrain.waterliney_f);

	// exit early if no change detected in static arrangement
	static double dLastSuperHash = -1;
	if(g_bResetHasForLevelGeneration == true)
	{
		dLastSuperHash = -1;
		bForceGeneration = true;
		g_bResetHasForLevelGeneration = false;
	}
	if (bForceGeneration == false)
	{
		if (dLastSuperHash != -1 && dSuperHash == dLastSuperHash) return;
	}
	dLastSuperHash = dSuperHash;

	// create monster object representing level geometry
	int iBuildAllLevelMesh = g.meshgeneralwork;
	int iBuildAllLevelObj = g.tempobjectoffset + 0;
	if (ObjectExist(iBuildAllLevelObj) == 1) DeleteObject(iBuildAllLevelObj);

	// always expand bounding box in case characters move around
	vecMinArea.x -= 1000;
	vecMaxArea.x += 1000;
	vecMinArea.z -= 1000;
	vecMaxArea.z += 1000;

	// reuse navmeshlimiter to create a tiny navmesh nothing could possibly use (but keep navmesh calls working)
	bool bIgnoreStaticStuff = false;
	if (t.visuals.bEnableZeroNavMeshMode == true)
	{
		vecMinArea.x = -51050;
		vecMaxArea.x = -51000;
		vecMinArea.z = -51050;
		vecMaxArea.z = -51000;
		vecCustomPlayAreaMin.x = -51050;
		vecCustomPlayAreaMax.x = -51000;
		vecCustomPlayAreaMin.z = -51050;
		vecCustomPlayAreaMax.z = -51000;
		bIgnoreStaticStuff = true;
	}

	// terrain geometry
	int iLimbIndex = 1;
	int iTerrainObj = iBuildAllLevelObj;
	bool bCreateTerrainMesh = true;
	if (bCreateTerrainMesh == true )
	{
		// generate polygons for required area
		int iFirstLOD = 2;
		GGVECTOR3* pvecVerts = NULL;
		int iTerrainFloorVertexCount = GGTerrain_GetTriangleListHighQuality(&pvecVerts, vecMinArea.x, vecMinArea.z, vecMaxArea.x, vecMaxArea.z, iFirstLOD);
		if (iTerrainFloorVertexCount > 0 )
		{
			// divide up into 16-bit size meshes
			int iVertStart =  0;
			int iPiecesCount = iTerrainFloorVertexCount / 65535;
			for (int iPieces = 0; iPieces <= iPiecesCount; iPieces++)
			{
				// create a mesh from polygons
				int newobj = g.tempobjectoffset + 1;
				int iTerrainNavMeshObj = newobj;
				if (iPieces == 0) iTerrainNavMeshObj = iBuildAllLevelObj;
				if (ObjectExist(iTerrainNavMeshObj) == 1) DeleteObject (iTerrainNavMeshObj);
				MakeObjectPlane (iTerrainNavMeshObj, 1, 1);
				sObject* pObject = GetObjectData(iTerrainNavMeshObj);
				delete pObject->ppMeshList[0];
				sMesh* pMesh = new sMesh();

				// full piece size or remaining
				DWORD dwVertexCount = iTerrainFloorVertexCount - iVertStart;
				if (dwVertexCount > 65535) dwVertexCount = 65535;

				// resize object for this
				DWORD dwIndexCount = dwVertexCount;
				SetupMeshFVFData (pMesh, GGFVF_XYZ, dwVertexCount, dwIndexCount, false);
				pMesh->iPrimitiveType = GGPT_TRIANGLELIST;
				pMesh->iDrawVertexCount = pMesh->dwVertexCount;
				pMesh->iDrawPrimitives = dwIndexCount / 3;
				pObject->ppMeshList[0] = pMesh;
				pObject->pFrame->pMesh = pMesh;
				float* pVertPtr = (float*)pMesh->pVertexData;
				for (int v = 0; v < dwVertexCount; v++)
				{
					*(pVertPtr + 0) = pvecVerts[iVertStart + v].x;
					*(pVertPtr + 1) = pvecVerts[iVertStart + v].y;
					*(pVertPtr + 2) = pvecVerts[iVertStart + v].z;
					pVertPtr += 3;
				}
				WORD* pIndicePtr = (WORD*)pMesh->pIndices;
				for (int i = 0; i < dwIndexCount; i++)
				{
					*(pIndicePtr) = i;
					pIndicePtr++;
				}

				// for next piece (if any)
				iVertStart += 65535;

				// add this terrain piece as mesh to main OBJ
				if (iPieces > 0)
				{
					// after iniutial creation, add new meshes
					MakeMeshFromObject(iBuildAllLevelMesh, iTerrainNavMeshObj);
					AddLimb(iBuildAllLevelObj, iLimbIndex, iBuildAllLevelMesh);
					iLimbIndex++;

					// remove tempo mesh/obj
					if (ObjectExist(iTerrainNavMeshObj) == 1) DeleteObject (iTerrainNavMeshObj);
				}
			}

			// ensure main OIBJ is hidden during process
			HideObject(iBuildAllLevelObj);

			// free when complete
			if (pvecVerts)
			{
				delete pvecVerts;
				pvecVerts = NULL;
			}
		}
		else
		{
			// inexplicavble that any part of the terrain measuring 1000x1000 produces NO polys!!
			MakeObjectPlane (iBuildAllLevelObj, 1, 1);
			HideObject(iBuildAllLevelObj);
		}
	}
	else
	{
		MakeObjectPlane (iBuildAllLevelObj, 1, 1);
		HideObject(iBuildAllLevelObj);
	}

	// all static objects in level
	if (bIgnoreStaticStuff == false)
	{
		for (int e = 1; e <= g.entityelementlist; e++)
		{
			if (t.entityelement[e].staticflag == 1)
			{
				int iObj = t.entityelement[e].obj;
				int iBankindex = t.entityelement[e].bankindex;
				bool bValid = true;
				if (t.entityprofile[iBankindex].ismarker != 0) bValid = false;
				if (t.entityprofile[iBankindex].collisionmode == 11) bValid = false;
				if (t.entityprofile[iBankindex].collisionmode == 12) bValid = false;
				if (t.entityprofile[iBankindex].isammo == 1) bValid = false;
				if (t.entityprofile[iBankindex].isweapon_s.Len() > 1) bValid = false;

				if (bValid && iObj > 0 && ObjectExist(iObj) == 1)
				{
					// get position of static obstruction
					GGVECTOR3 vecPos = GGVECTOR3(ObjectPositionX(iObj), ObjectPositionY(iObj), ObjectPositionZ(iObj));
					if (vecPos.x > editableSize || vecPos.x < -editableSize || vecPos.z > editableSize || vecPos.z < -editableSize) continue;

					// also reject if object is moved to a non-visible position
					if (vecPos.y <= -50000) continue;

					// extra feature called NAVMESH LIMIT
					if (bUsingNavMeshLimitCustomArea == true)
					{
						if (vecPos.x < vecCustomPlayAreaMin.x ||
							vecPos.x > vecCustomPlayAreaMax.x ||
							vecPos.z < vecCustomPlayAreaMin.z ||
							vecPos.z > vecCustomPlayAreaMax.z)
						{
							// this object outside of custom navmesh limit area, can ignore
							continue;
						}
					}

					//PE: Add physics shapes here.
					if (GetMeshExist(iBuildAllLevelMesh) == 1) DeleteMesh(iBuildAllLevelMesh);
					if (iBankindex > 0 && t.entityprofile[iBankindex].collisionmode >= 50 && t.entityprofile[iBankindex].collisionmode < 60)
					{
						int newobj = g.tempobjectoffset + 1;
						if (ObjectExist(newobj)) DeleteObject(newobj);
						MakeObjectCylinder(newobj, 1);

						t.tSizeY_f = ObjectSizeY(iObj, 1);

						//  if have ABS position from AI OBSTACLE calc, use that instead
						if (t.entityelement[e].abscolx_f != -1)
						{
							t.tFinalX_f = t.entityelement[e].abscolx_f;
							t.tFinalZ_f = t.entityelement[e].abscolz_f;
						}
						else
						{
							t.tFinalX_f = ObjectPositionX(iObj);
							t.tFinalZ_f = ObjectPositionZ(iObj);
						}
						t.tFinalY_f = ObjectPositionY(iObj) + (t.tSizeY_f / 2.0);

						//  if have ABS radius from AI OBSTACLE calc, use that instead
						if (t.entityelement[e].abscolradius_f != -1)
						{
							t.tSizeX_f = t.entityelement[e].abscolradius_f;
							t.tSizeZ_f = t.entityelement[e].abscolradius_f;
						}
						else
						{
							t.tSizeX_f = 20;
							t.tSizeZ_f = 20;
						}

						//  increase size by 25%
						t.tSizeX_f = t.tSizeX_f * 1.25;
						t.tSizeZ_f = t.tSizeZ_f * 1.25;

						ScaleObject(newobj, t.tSizeX_f * 100, t.tSizeY_f * 100, t.tSizeZ_f * 100);
						MakeMeshFromObject(iBuildAllLevelMesh, newobj);
						AddLimb(iBuildAllLevelObj, iLimbIndex, iBuildAllLevelMesh);
						OffsetLimb(iBuildAllLevelObj, iLimbIndex, ObjectPositionX(iObj), ObjectPositionY(iObj) + (t.tSizeY_f * 0.5), ObjectPositionZ(iObj));
						iLimbIndex++;
					}
					else
					{
						// to make a cleaner NAVMESH, stairs are better as ramps, so use OBJ if present for this purpose
						// if object uses convex hull, see if there is an OBJ we can swap inplace of the objects full mesh
						int iObjToUseForNavMesh = iObj;
						bool bHeavyPOlyShapesShouldCheckForOBJ = false;
						if (t.entityprofile[iBankindex].collisionmode == 1) bHeavyPOlyShapesShouldCheckForOBJ = true; // polygon
						if (t.entityprofile[iBankindex].collisionmode == 8) bHeavyPOlyShapesShouldCheckForOBJ = true; // polygon with OBJ
						if (t.entityprofile[iBankindex].collisionmode == 9) bHeavyPOlyShapesShouldCheckForOBJ = true; // convex hull
						if (t.entityprofile[iBankindex].collisionmode == 10) bHeavyPOlyShapesShouldCheckForOBJ = true; // hull decomp
						if (bHeavyPOlyShapesShouldCheckForOBJ == true)
						{
							char pNoFPE[MAX_PATH];
							strcpy(pNoFPE, t.entitybank_s[iBankindex].Get());
							pNoFPE[strlen(pNoFPE) - 4] = 0;
							char pOBJCollisionMesh[MAX_PATH];
							sprintf(pOBJCollisionMesh, "%s\\Files\\entitybank\\%s.obj", g.fpscrootdir_s.Get(), pNoFPE);
							GG_GetRealPath(pOBJCollisionMesh, 0);
							if (FileExist(pOBJCollisionMesh) == 0)
							{
								sprintf(pOBJCollisionMesh, "%s\\Files\\entitybank\\%s_COL.obj", g.fpscrootdir_s.Get(), pNoFPE);
								GG_GetRealPath(pOBJCollisionMesh, 0);
							}
							if (FileExist(pOBJCollisionMesh) == 1)
							{
								// can optimize this by keeping the low poly OBJ, perhaps add to DBO as a LOD??
								if (ObjectExist(g.temp2objectoffset) == 1) DeleteObject(g.temp2objectoffset);
								LoadObject (pOBJCollisionMesh, g.temp2objectoffset);
								iObjToUseForNavMesh = g.temp2objectoffset;
								RotateObject(iObjToUseForNavMesh, ObjectAngleX(iObj), ObjectAngleY(iObj), ObjectAngleZ(iObj));
								ScaleObject(iObjToUseForNavMesh, ObjectScaleX(iObj), ObjectScaleY(iObj), ObjectScaleZ(iObj));
							}
						}
						// regular mesh from object
						MakeMeshFromObject(iBuildAllLevelMesh, iObjToUseForNavMesh);
						AddLimb(iBuildAllLevelObj, iLimbIndex, iBuildAllLevelMesh);
						OffsetLimb(iBuildAllLevelObj, iLimbIndex, ObjectPositionX(iObj), ObjectPositionY(iObj), ObjectPositionZ(iObj));
						iLimbIndex++;
					}
				}
			}
		}
	}

	// simple obstacle object/mesh to punch into navmesh
	if (ObjectExist(g.temp2objectoffset)) DeleteObject(g.temp2objectoffset); //PE: Got 7007 if any tree cylinder are created.
	MakeObjectBox (g.temp2objectoffset, 10, 200, 10);
	MakeMeshFromObject (g.meshgeneralwork2, g.temp2objectoffset);
	DeleteObject(g.temp2objectoffset);

	// solve issue of large delay when nothing in scene (hack for now, need to find real reason)
	if (iLimbIndex == 1)
	{
		AddLimb(iBuildAllLevelObj, iLimbIndex, g.meshgeneralwork2);
		OffsetLimb(iBuildAllLevelObj, iLimbIndex, 0 - 800, 0 - 5, 0 - 800);
		iLimbIndex++;
	}

	// add virtual trees into the navmesh (quick solution to code, very slow to execute!!)
	bool bOldMethodOfAddingTreesToNavMesh = false;
	if (bOldMethodOfAddingTreesToNavMesh == true)
	{
		if (ggtrees_global_params.draw_enabled == 1)
		{
			timestampactivity(0, "Adding all trees to temporary nav mesh super object");
			float fPlayAreaRadiusX = (vecMaxArea.x - vecMinArea.x) / 2;
			float fPlayAreaRadiusZ = (vecMaxArea.z - vecMinArea.z) / 2;
			float fPlayAreaCenterX = vecMinArea.x + fPlayAreaRadiusX;
			float fPlayAreaCenterZ = vecMinArea.z + fPlayAreaRadiusZ;
			float fPlayAreaRadius = fPlayAreaRadiusX;
			if (fPlayAreaRadiusZ > fPlayAreaRadius) fPlayAreaRadius = fPlayAreaRadiusZ;
			GGTrees::GGTreePoint* pOutPoints = NULL;
			int iTreeCount = GGTrees::GGTrees_GetClosest (fPlayAreaCenterX, fPlayAreaCenterZ, fPlayAreaRadius, &pOutPoints);
			if (pOutPoints)
			{
				for (int n = 0; n < iTreeCount; n++)
				{
					GGVECTOR3 vecTreePos = GGVECTOR3(pOutPoints[n].x, pOutPoints[n].y, pOutPoints[n].z);
					AddLimb(iBuildAllLevelObj, iLimbIndex, g.meshgeneralwork2);
					OffsetLimb(iBuildAllLevelObj, iLimbIndex, vecTreePos.x, vecTreePos.y, vecTreePos.z);
					iLimbIndex++;
				}
				delete[] pOutPoints;
			}
		}
	}

	// Generating raw nav mesh vertices for build
	timestampactivity(0, "Generating raw nav mesh vertices for build");
	float* pRawVertices = 0;
	uint32_t numRawVertices = 0;
	const bool saveObject = false;
	if ( saveObject )
	{
		// save level geometry object as OBJ
		char pProgressStr[256];
		sprintf_s(pProgressStr, 256, "SAVING NAV MESH TOPOGRAPHY - %d\\100 Complete", 11);
		void printscreenprompt(char*);
		printscreenprompt(pProgressStr);
		LPSTR pAllLevelGeometryFilename = "levelbank\\testmap\\rawlevelgeometry.obj";
		if (FileExist(pAllLevelGeometryFilename)) DeleteFileA(pAllLevelGeometryFilename);
		SaveObjectEx(pAllLevelGeometryFilename, iBuildAllLevelObj, true); //PE: Use 8% of the time optimize it.
	}
	else
	{
		// get raw vertex soup from super object
		char pProgressStr[256];
		sprintf_s(pProgressStr, 256, "GENERATING NAV MESH VERTICES - %d\\100 Complete", 11);
		void printscreenprompt(char*);
		printscreenprompt(pProgressStr);
		numRawVertices = GetObjectNavMeshVertexCount( iBuildAllLevelObj );
		pRawVertices = new float[numRawVertices *3 ];
		GetObjectNavMeshVertices( iBuildAllLevelObj, pRawVertices);
	}

	// new method of adding trees, direct to the vertex soup
	float* pVertices = pRawVertices;
	uint32_t numVertices = numRawVertices;
	if (bIgnoreStaticStuff == false)
	{
		if (bOldMethodOfAddingTreesToNavMesh == false)
		{
			if (ggtrees_global_params.draw_enabled == 1)
			{
				timestampactivity(0, "Adding all trees to post raw vertex soup (quicker)");
				float fPlayAreaRadiusX = (vecMaxArea.x - vecMinArea.x) / 2;
				float fPlayAreaRadiusZ = (vecMaxArea.z - vecMinArea.z) / 2;
				float fPlayAreaCenterX = vecMinArea.x + fPlayAreaRadiusX;
				float fPlayAreaCenterZ = vecMinArea.z + fPlayAreaRadiusZ;
				float fPlayAreaRadius = fPlayAreaRadiusX;
				if (fPlayAreaRadiusZ > fPlayAreaRadius) fPlayAreaRadius = fPlayAreaRadiusZ;
				GGTrees::GGTreePoint* pOutPoints = NULL;
				int iTreeCount = GGTrees::GGTrees_GetClosest (fPlayAreaCenterX, fPlayAreaCenterZ, fPlayAreaRadius, &pOutPoints);
				if (pOutPoints)
				{
					// reserve space for larger vertex soup
					numVertices = numRawVertices + (iTreeCount * 6 * 6);
					pVertices = new float[numVertices * 3];
					memcpy (pVertices, pRawVertices, sizeof(float) * numRawVertices * 3);

					// for each tree, add a cube to vertex data
					uint32_t numCurrentVertices = numRawVertices;
					for (int n = 0; n < iTreeCount; n++)
					{
						GGVECTOR3 vecTreePos = GGVECTOR3(pOutPoints[n].x, pOutPoints[n].y, pOutPoints[n].z);
						sMesh* pTreeCubeShape = g_RawMeshList[g.meshgeneralwork2];
						if (pTreeCubeShape)
						{
							float* pVertData = (float*)pTreeCubeShape->pVertexData;
							uint32_t stride = pTreeCubeShape->dwFVFSize / 4;
							for (DWORD dwI = 0; dwI < pTreeCubeShape->dwIndexCount; dwI++)
							{
								DWORD dwV = pTreeCubeShape->pIndices[dwI];
								uint32_t vIndex = dwV * stride;
								GGVECTOR3 vecXYZ = GGVECTOR3(pVertData[vIndex], pVertData[vIndex + 1], pVertData[vIndex + 2]);
								uint32_t index = (numCurrentVertices + dwI) * 3;
								pVertices[index + 0] = vecTreePos.x + vecXYZ.x;
								pVertices[index + 1] = vecTreePos.y + vecXYZ.y;
								pVertices[index + 2] = vecTreePos.z + vecXYZ.z;
							}
							numCurrentVertices += pTreeCubeShape->dwIndexCount;
						}
					}
					delete[] pOutPoints;

					// remove old vert soup in favor of new one
					if (pRawVertices) delete[] pRawVertices;
				}
			}
		}

		// final step to send all Y coords under waterline to depths to avoid paths under water (and character walking into drowning)
		for (DWORD dwV = 0; dwV < numVertices; dwV++)
		{
			uint32_t index = dwV * 3;
			float fY = pVertices[index + 1];
			if (fY < t.terrain.waterliney_f)
			{
				// sink this vertex to the depths if underwater, cannot make path from this!
				pVertices[index + 1] = -9999;
			}
		}
	}

	// can now delete massive object
	timestampactivity(0, "Delete temporary nav mesh super object");
	DeleteObject(iBuildAllLevelObj);

	// generate nav mesh using recast on OBJ
	timestampactivity(0, "Using recast to build the nav mesh");

	if (bStandalone)
	{
		//PE: Save raw pVertices size numVertices for standalone, save under fpm name.
		cstr importer_getfilenameonly(LPSTR pFileAndPossiblePath);
		cstr filenameonly = importer_getfilenameonly(g.projectfilename_s.Get());
		std::string newfilename = filenameonly.Get();
		replaceAll(newfilename, ".fpm", ".raw");
		DARKSDK LPSTR GetDir(void);
		//PE: Save into standalone folder.
		std::string fullname = (char*)GetDir();
		fullname = fullname + "\\navbank\\";
		CreateDirectoryA(fullname.c_str(), NULL);
		fullname = fullname + newfilename;
		if (saveVertices(fullname, pVertices, numVertices))
		{
			//PE: Success.
		}
	}

	g_RecastDetour.buildall( pVertices, numVertices );

	// remove vert soup
	if ( pVertices ) delete [] pVertices;
}

void game_updatenavmeshsystem(void)
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	// render any debug objects (such as nav mesh)
	if (g_bShowRecastDetourDebugVisuals == true)
		g_RecastDetour.handleDebugRender();
	else
		g_RecastDetour.cleanupDebugRender();
}

void game_masterroot_gameloop_initcode(int iUseVRTest)
{
	// also hide rendering of 3D while we set up a new level
	SyncMaskOverride ( 0 );

	extern uint32_t LuaFrameCount;
	LuaFrameCount = 0;

	// Loading page
	timestampactivity(0,"_titles_loadingpageupdate");
	if ( t.game.gameisexe == 1 ) 
	{
		// Ensure no previous flatareadata exists prior to entity creating new ones
		timestampactivity(0, "Remove All Terrain Flat Areas");
		GGTerrain_RemoveAllFlatAreas();
		
		//PE: When getting here everything was faded out.
		t.postprocessings.fadeinvalue_f = 1.0f;
		HideOrShowLUASprites(false);
		EnableAllSprites(); // the disable is called in DarkLUA by ResetFade() black out command when load game position

		//titles_loadingpage ( );
		timestampactivity(0,"LUA script : loading");
		sky_hide();
		t.game.levelloadprogress = 0;
		titleslua_init ( );
		titleslua_main ( "loading" );
		sky_show();
		titleslua_main_loopcode();
		extern bool g_bNoSwapchainPresent;
		//PE: Why was we doing this, this will make a 10 sec blackscreen delay until loading screen is displayed ?????
		//PE: Removed for now TODO check why it was added.
		
		//PE: This was to prevent old loadingscreen is displayed.
		//g_Storyboard_Current_Loading_Page
		extern char g_Storyboard_Current_Loading_Page[256];
		void FindLoadingScreen(void);
		FindLoadingScreen();
		//PE: If using cuatom loading screen delay rendering, TODO: find another way to speed up the 10 sec delay before the screen. (mapfile_loadproject_fpm)
		if( stricmp(g_Storyboard_Current_Loading_Page , "loading.lua") != NULL)
			g_bNoSwapchainPresent = true;
		t.game.levelloadprogress=0  ;
		titles_loadingpageupdate ( );
		g_bNoSwapchainPresent = false;

	}

	// Extract level files from FPM
	if ( t.game.runasmultiplayer == 1 ) 
	{
		// Multiplayer FPM loading
		g.projectfilename_s=g.mysystem.editorsGrideditAbs_s+"__multiplayerlevel__.fpm";//g.fpscrootdir_s+"\\Files\\editors\\gridedit\\__multiplayerlevel__.fpm";
		t.trerfeshvisualsassets=1;
		mapfile_loadproject_fpm ( );
		t.game.jumplevel_s="";
	}
	else
	{
		// Single player
		if ( Len(t.game.jumplevel_s.Get())> 0 ) //PE: issue https://github.com/TheGameCreators/GameGuruRepo/issues/444
		{
			// can override jumplevel with 'advanced warning level filename' when LOAD level from MAIN MENU
			if ( strcmp ( t.game.pAdvanceWarningOfLevelFilename, "" ) != NULL )
			{
				t.game.jumplevel_s = t.game.pAdvanceWarningOfLevelFilename;
				strcpy ( t.game.pAdvanceWarningOfLevelFilename, "" );
			}

			// work out first level from exe name (copied to jumplevel_s)
			g.projectfilename_s = g.mysystem.mapbank_s + t.game.jumplevel_s;
			//PE: In new code ifused can now include mapbank
			if (pestrcasestr(t.game.jumplevel_s.Get(), "mapbank\\"))
				g.projectfilename_s = t.game.jumplevel_s;
			if ( cstr(Lower(Right(g.projectfilename_s.Get(),4))) != ".fpm" )
				g.projectfilename_s=g.projectfilename_s+".fpm";

			// 050316 - if not there, try all subfolders
			if ( FileExist(cstr(g.fpscrootdir_s+"\\Files\\"+g.projectfilename_s).Get()) == 0 ) 
			{
				// go into mapbank folder
				cstr tthisold_s =  "";
				tthisold_s=GetDir();
				SetDir ( g.mysystem.mapbankAbs_s.Get() );

				// scan for ALL files/folders
				ChecklistForFiles (  );
				for ( int c = 1 ; c<=  ChecklistQuantity(); c++ )
				{
					if (  ChecklistValueA(c) != 0 ) 
					{
						// only folders
						cstr tfolder_s = ChecklistString(c);
						if ( tfolder_s != "." && tfolder_s != ".." ) 
						{
							// skip . and .. folders
							cstr newlevellocation = g.mysystem.mapbank_s + tfolder_s + "\\" + t.game.jumplevel_s;
							if ( cstr(Lower(Right(newlevellocation.Get(),4))) != ".fpm" )
								newlevellocation = newlevellocation + ".fpm";

							// does this guessed file location exist
							if ( FileExist(cstr(g.fpscrootdir_s+"\\Files\\"+newlevellocation).Get()) == 1 ) 
							{
								// found the level inside a nested folder
								g.projectfilename_s = newlevellocation; 
								break;
							}
						}
					}
				}
				SetDir ( tthisold_s.Get() );
			}

			//PE: Make sure if using storyboard we set the current level correctly.
			//+t.game.jumplevel_s{ m_pString = 0x000001468321fe60 "level 3.fpm" m_size = 11 m_capacity = 26 }	cStr
			//+ g.projectfilename_s{ m_pString = 0x00000146ae96d490 "mapbank\\level 3.fpm" m_size = 19 m_capacity = 27 }	cStr
			if (strlen(Storyboard.gamename) > 0)
			{
				//PE: Find g_Storyboard_Current_Level from t.game.jumplevel_s.
				for (int i = 0; i < STORYBOARD_MAXNODES; i++)
				{
					if (Storyboard.Nodes[i].used)
					{
						if (pestrcasestr(Storyboard.Nodes[i].level_name, t.game.jumplevel_s.Get()) != 0)
						{
							g_Storyboard_Current_Level = i;
							strcpy(g_Storyboard_Current_fpm, Storyboard.Nodes[i].level_name);
						}
					}
				}
			}

			// finally load the level in
			mapfile_loadproject_fpm ( );
			t.visuals=t.gamevisuals;
			t.game.jumplevel_s = "";

		}
	}

	// reload gunspecs
	if (g.reloadWeaponGunspecs == 1)
	{
		gun_scaninall_dataonly();
	}

	// we first load extra guns into gun array EARLY (ahead of entity data load which assigns gunids to isweapon hasweapon)
	gun_tagmpgunstolist ( );

	// help keep progress bar instant and moving
	char pProgressStr[256];
	sprintf_s(pProgressStr, 256, "PREPARING TEST LEVEL - %d\\100 Complete", 1);
	void printscreenprompt(char*);
	printscreenprompt(pProgressStr);

	// just load the entity data for now (rest in _game_loadinleveldata)
	timestampactivity(0,"_game_loadinentitiesdatainlevel");
	if ( t.game.gameisexe == 1 || t.game.runasmultiplayer == 1 ) 
	{
		//  extra precaution, delete any old entities and LM objects
		if ( t.game.runasmultiplayer == 1 ) 
		{
			entity_delete ( );
		}
		#ifdef WIP_PROLOADLEVELTEXTURES
		//PE: Record preload informations here.
		preload_setup.clear();
		#endif

		game_loadinentitiesdatainlevel ( );

		#ifdef WIP_PROLOADLEVELTEXTURES
		std::string sString = g.projectfilename_s.Get();
		replaceAll(sString, "\\", "_");
		replaceAll(sString, ".", "_");
		replaceAll(sString, ":", "_");

		//PE: Save preload information.
		char szRealFilename[MAX_PATH];
		strcpy_s(szRealFilename, MAX_PATH, "preloadinfo\\");
		strcat(szRealFilename, sString.c_str());
		GG_GetRealPath(szRealFilename, 1);
		std::ofstream output_file(szRealFilename);
		std::ostream_iterator<std::string> output_iterator(output_file, "\n");
		std::copy(preload_setup.begin(), preload_setup.end(), output_iterator);
		preload_setup.clear();
		#endif
	}

	// Load any extra material sounds associated with new entities (i.e. material(m).usedinlevel=1?)
	// NOTE: Level can collect materials (and material depth) and apply here to quicken material loader (2s)
	material_loadsounds ( 0 );

	// and reset 3D listener for consistency each level
	extern void ResetListener (void);
	ResetListener();

	// if multiplayer, detect spawn positions and add extra UBER characters
	if ( t.game.runasmultiplayer == 1 ) 
	{
		// these are the multiplayer start markers
		t.tnumberofstartmarkers = 0;
		g.mp.team = 0;
		g.mp.coop = 0;
		for ( t.tc = 1 ; t.tc<=  MP_MAX_NUMBER_OF_PLAYERS; t.tc++ )
		{
			t.mpmultiplayerstart[t.tc].active=0;
		}
		t.plrindex=1;
		t.tfoundAMultiplayerScript = 0;
		for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
		{
			// reset all updates
			t.entityelement[t.e].mp_updateOn = 0;
			t.entityelement[t.e].mp_isLuaChar = 0;
			t.entityelement[t.e].mp_rotateType = 0;
			t.entid=t.entityelement[t.e].bankindex;
			if ( t.entid>0 ) 
			{
			}
		}

		// Build multiplayer start markers
		t.thaveTeamAMarkers = 0;
		t.thaveTeamBMarkers = 0;
		t.tmpstartindex = 1;
		for ( t.e = 1 ; t.e <= g.entityelementlist; t.e++ )
		{
			t.entid=t.entityelement[t.e].bankindex;
			if ( t.entid>0 ) 
			{
				if ( t.entityprofile[t.entid].ismarker == 7 && t.tmpstartindex <= MP_MAX_NUMBER_OF_PLAYERS ) 
				{
					// add start markers for free for all or team a
					// a spawn GetPoint ( for the multiplayer )
					t.mpmultiplayerstart[t.tmpstartindex].active=1;
					t.mpmultiplayerstart[t.tmpstartindex].x=t.entityelement[t.e].x;
					// added 10 onto the y otherwise the players fall through the ground
					t.mpmultiplayerstart[t.tmpstartindex].y=t.entityelement[t.e].y+50;
					t.mpmultiplayerstart[t.tmpstartindex].z=t.entityelement[t.e].z;
					t.mpmultiplayerstart[t.tmpstartindex].angle=t.entityelement[t.e].ry;
					t.thaveTeamAMarkers = 1;
					++t.tnumberofstartmarkers;
					++t.tmpstartindex;
				}
			}
		}
		// add team b markers if in team mode

		// check for coop mode
		g.mp.coop = 0;

		// perhaps it is a solo game with a start maker only
		bool bHaveRegularStartMarker = false;
		if ( g.mp.coop == 0 && t.tnumberofstartmarkers == 0 ) 
		{
			for ( t.e = 1 ; t.e <= g.entityelementlist; t.e++ )
			{
				t.entid=t.entityelement[t.e].bankindex;
				if ( t.entid>0 ) 
				{
					if ( t.entityprofile[t.entid].ismarker == 1 ) 
					{
						// a spawn GetPoint ( for the multiplayer )
						bHaveRegularStartMarker = true;
						t.mpmultiplayerstart[1].active=1;
						t.mpmultiplayerstart[1].x=t.entityelement[t.e].x;
						// added 10 onto the y otherwise the players fall through the ground
						t.mpmultiplayerstart[1].y=t.entityelement[t.e].y+50;
						t.mpmultiplayerstart[1].z=t.entityelement[t.e].z;
						t.mpmultiplayerstart[1].angle=t.entityelement[t.e].ry;
						t.entityelement[t.e].eleprof.phyalways = 1;

					}
				}
			}
		}

		//  if multiplayer and not coop, disable ai characters
			// Photon retains all characters in map

		// if multiplayer and coop, setup ai for switching who control them, depending on gameplay circumstances

		// if no multiplayer markers, put some at the default height
		if ( t.tnumberofstartmarkers == 0 && bHaveRegularStartMarker == false ) 
		{
			for ( t.tloop = 1; t.tloop <= MP_MAX_NUMBER_OF_PLAYERS; t.tloop++ )
			{
				t.mpmultiplayerstart[t.tloop].active=1;
				t.mpmultiplayerstart[t.tloop].x=GGORIGIN_X;
				//  added 10 onto the y otherwise the players fall through the ground
				t.mpmultiplayerstart[t.tloop].y=BT_GetGroundHeight(t.terrain.TerrainID,GGORIGIN_X,GGORIGIN_Z)+50;
				t.mpmultiplayerstart[t.tloop].z=GGORIGIN_Z;
				t.mpmultiplayerstart[t.tloop].angle=0;
			}
		}

		// reserve max multiplayer characters (all weapon animations included)
		Dim ( t.tubindex,2+MP_MAX_NUMBER_OF_PLAYERS  );
		t.ent_s=g.rootdir_s+"charactercreatorplus\\Uber Character.fpe";
		entity_addtoselection_core ( );
		t.tubindex[0]=t.entid;
		t.entityprofile[t.tubindex[0]].ischaracter=0;
		t.entityprofile[t.tubindex[0]].collisionmode=12;
		t.entityprofile[t.tubindex[0]].aimain_s = "";

			// No teams - no combat!

		// add any character creator player avatars in
		for ( t.tcustomAvatarCount = 0 ; t.tcustomAvatarCount <= MP_MAX_NUMBER_OF_PLAYERS-1; t.tcustomAvatarCount++ )
		{
			t.mp_playerAvatarLoaded[t.tcustomAvatarCount] = false;
		}
		t.bTriggerAvatarRescanAndLoad = true;
		game_scanfornewavatars ( false );

		// store ttiswitch for tti as multiplayer avatars can upset the 0->1 switching!
		t.ttiswitch = 1;
		for ( t.plrindex = 1 ; t.plrindex <= MP_MAX_NUMBER_OF_PLAYERS; t.plrindex++ )
		{
			// Add the max number of players into the level if there are start markers or not
			if ( g.mp.team == 1 && g.mp.coop == 0 ) 
			{
				t.ttiswitch = 1 - t.ttiswitch;
			}
			else
			{
				t.ttiswitch = 0;
			}
			t.tti = t.ttiswitch;

			// check if the player has their own avatar
			if ( t.mp_playerAvatars_s[t.plrindex-1] != "" ) 
			{
				t.tti = t.plrindex-1+2;
			}

			t.ubercharacterindex = t.tubindex[t.tti];
			t.entitymaintype=1 ; t.entitybankindex=t.ubercharacterindex;
			t.gridentityeditorfixed=0;
			t.gridentitystaticmode=0;
			t.gridentityhasparent=0;
			t.tfoundone = 0;
			if ( t.mpmultiplayerstart[t.plrindex].active == 1 ) 
			{
				t.tfoundone = 1;
				t.gridentityposx_f=t.mpmultiplayerstart[t.plrindex].x;
				t.gridentityposy_f=t.mpmultiplayerstart[t.plrindex].y;
				t.gridentityposz_f=t.mpmultiplayerstart[t.plrindex].z;
			}
			else
			{
				t.tonetotry = t.plrindex/2;
				if ( t.tonetotry > 0 ) 
				{
					t.tfoundone = 1;
					if ( t.mpmultiplayerstart[t.tonetotry].active == 1 ) 
					{
						t.gridentityposx_f=t.mpmultiplayerstart[t.tonetotry].x;
						t.gridentityposy_f=t.mpmultiplayerstart[t.tonetotry].y;
						t.gridentityposz_f=t.mpmultiplayerstart[t.tonetotry].z;
					}
				}
			}
			if ( t.tfoundone == 0 ) 
			{
				if ( t.mpmultiplayerstart[1].active == 1 ) 
				{
					t.gridentityposx_f=t.mpmultiplayerstart[1].x;
					t.gridentityposy_f=t.mpmultiplayerstart[1].y;
					t.gridentityposz_f=t.mpmultiplayerstart[1].z;
				}
			}
			t.gridentityrotatex_f=0;
			t.gridentityrotatey_f=t.mpmultiplayerstart[t.plrindex].angle;
			t.gridentityrotatez_f=0;
			t.gridentityrotatequatmode = 0;
			t.gridentityrotatequatx_f = 0;
			t.gridentityrotatequaty_f = 0;
			t.gridentityrotatequatz_f = 0;
			t.gridentityrotatequatw_f = 1;
			t.gridentityscalex_f=100;
			t.gridentityscaley_f=100;
			t.gridentityscalez_f=100;
			entity_fillgrideleproffromprofile ( );
			entity_addentitytomap_core ( );
			t.mpmultiplayerstart[t.plrindex].ghostentityindex=t.e;

			// Grab the entity number for steam to use
			t.mp_playerEntityID[t.plrindex-1] = t.e;
			t.entityprofile[t.ubercharacterindex].ismultiplayercharacter=1;
			t.entityprofile[t.ubercharacterindex].hasweapon_s="";
			t.entityprofile[t.ubercharacterindex].hasweapon=0;
			t.entityprofile[t.ubercharacterindex].aimain_s = "";
		}
	}

	// in standalone, no IDE feeding test level, so load it in
	timestampactivity(0,"_game_loadinleveldata");
	if ( t.game.gameisexe == 1 || t.game.runasmultiplayer == 1 ) 
	{
		game_loadinleveldata ( );
	}

	//  Prepare this level
	t.game.levelplrstatsetup = 1; //PE: Make sure to setup new "player start marker" settings.
	if (t.game.gameisexe == 1) loadingpageprogress(5);

	// help keep progress bar instant and moving
	sprintf_s(pProgressStr, 256, "LOADING LEVEL RESOURCES - %d\\100 Complete", 2);
	void printscreenprompt(char*);
	printscreenprompt(pProgressStr);
	game_preparelevel ( );
	game_preparelevel_forplayer ( );

	//LB: some corruption in older levels, can correct here (level editor also corrects, but not for levels loaded and ran)
	for (t.e = 1; t.e <= g.entityelementlist; t.e++)
	{
		if (t.entityelement[t.e].obj > 0 && t.e < g.entityelementlist)
		{
			int masterid = t.entityelement[t.e].bankindex;
			if (masterid > 0 && (t.entityprofile[masterid].ragdoll == 1 || t.entityprofile[masterid].ischaracter == 1))
			{
				entity_calculateeuleryfromquat(t.e);
			}
		}
	}

	sprintf_s(pProgressStr, 256, "FINALIZING LEVEL DATA");// -% d\\100 Complete", 15);
	void printscreenprompt(char*);
	printscreenprompt(pProgressStr);
	game_preparelevel_finally ( );

	//PE: Disable collision.
	for (t.e = 1; t.e <= g.entityelementlist; t.e++)
	{
		int tentid = t.entityelement[t.e].bankindex;
		int tobj = t.entityelement[t.e].obj;
		if (tentid > 0 && tobj > 0)
		{
			if (t.entityprofile[tentid].ischaracter == 0)
			{
				int iShape = t.entityprofile[tentid].collisionmode;
				if (t.entityelement[t.e].eleprof.iOverrideCollisionMode != -1)
						iShape = t.entityelement[t.e].eleprof.iOverrideCollisionMode;
				//PE: no max fpe is using canseethrough. else if (t.entityprofile[tentid].canseethrough == 1) iShape = 11;
				if (iShape == 11)
				{
					sObject* pObject = g_ObjectList[tobj];
					if (pObject)
					{
						// LB: Exception are objects that have is collectable set (need to detect these, even if no collision mode set)
						if (t.entityelement[t.e].eleprof.iscollectable==0)
						{
							WickedCall_SetDisableCollision(pObject, true);
						}
					}
				}
			}
		}
	}

	g.merged_new_objects = 0;
	if ( t.tlmloadsuccess == 0  ) 
	{ 
		//&& !g.disable_drawcall_optimizer
		//###################################################################
		//#### PE: Very simple but effectively draw call optimizer       ####
		//#### Could be made more intelligent when time allow :)         ####
		//#### On a 2000 object level it takes below 2 sec to run.       ####
		//#### setup.ini "drawcalloptimizer=1" will optimize everything  ####
		//#### setup.ini "drawcalloptimizer=0" only fpe settings counts. ####
		//#### .fpe "drawcalloptimizer=1" will optimize this object.     ####
		//#### .fpe "drawcalloptimizer=0" will NOT optimize object.      ####
		//#### .fpe "drawcalloptimizeroff=1" will NOT optimize object.   ####
		//#### (drawcalloptimizeroff is used when you optimize           ####
		//####  everything but have problems with a object )             ####
		//#### .fpe "drawcallscaleadjust" adjust scale.                  ####
		//###################################################################

		timestampactivity(0, "draw call optimizer.");
		#define DC_DISTANCE 1000
		#define MAX_DRAWPRIMITIVES 32765 // max faces.
		#define MAX_DRAWVERTEX 65520 // max vertex 65530.

		// clear old draw call optimizer objects
		for (t.e = 1; t.e <= g.entityelementlist; t.e++)
		{
			if (t.entityelement[t.e].obj > 0 && t.e < g.entityelementlist )
			{
				t.entityelement[t.e].dc_merged = false;
				if (t.entityelement[t.e].draw_call_obj > 0) 
				{
					if (t.entityelement[t.e].draw_call_obj > 0 && ObjectExist(t.entityelement[t.e].draw_call_obj) == 1) 
					{
						DeleteObject(t.entityelement[t.e].draw_call_obj);
						t.entityelement[t.e].draw_call_obj = 0;
					}
				}
			}
		}

		// go through all level objects
		for (t.e = 1; t.e <= g.entityelementlist; t.e++)
		{
			// for each object
			t.entid = t.entityelement[t.e].bankindex;
			t.obj = t.entityelement[t.e].obj;
			if (t.obj > 0 && t.e < g.entityelementlist && t.entityelement[t.e].dc_merged == false && (g.globals.drawcalloptimizer==1 || t.entityprofile[t.entid].drawcalloptimizer == 1) && t.entityprofile[t.entid].drawcalloptimizeroff == 0 && t.entityprofile[t.entid].isimmobile != 1 && t.entityelement[t.e].eleprof.isimmobile != 1 && t.entityelement[t.e].eleprof.spawnatstart == 1)
			{
				struct OrderByObjectDistance
				{
					bool operator()(int pObjectA, int pObjectB)
					{
						if (t.entityelement[pObjectA].dc_distance < t.entityelement[pObjectB].dc_distance) return true;
						if (t.entityelement[pObjectA].dc_distance == t.entityelement[pObjectB].dc_distance) return true;
						return false;
					}
				};

				// Sort a sublist by object, distance to increase hit rate
				int nextObjeid = 0;
				std::vector< int > vObjList;
				if (ObjectExist(t.obj)) 
				{
					for (int i = 1; i <= g.entityelementlist; i++) 
					{
						int testobj = t.entityelement[i].obj;
						int iEntid = t.entityelement[i].bankindex;
						if (testobj > 0 && i != t.e && ObjectExist(testobj) && t.entityelement[i].dc_merged == false && t.entityprofile[iEntid].isimmobile != 1 && t.entityelement[i].eleprof.isimmobile != 1 && t.entityelement[i].staticflag == 1 && t.entityelement[i].eleprof.spawnatstart == 1) 
						{
							sObject* pObject = g_ObjectList[t.obj];
							int instanceonly = 0;
							if (pObject && pObject->pInstanceOfObject) 
							{
								pObject = pObject->pInstanceOfObject;
							}

							sObject* pObjectTest = g_ObjectList[testobj];
							if (pObjectTest && pObjectTest->pInstanceOfObject) 
							{
								pObjectTest = pObjectTest->pInstanceOfObject;
							}

							if (pObject && pObjectTest && pObject == pObjectTest) 
							{
								t.tdx_f = t.entityelement[t.e].x - t.entityelement[i].x;
								t.tdz_f = t.entityelement[t.e].z - t.entityelement[i].z;
								t.tdd_f = Sqrt(abs(t.tdx_f*t.tdx_f) + abs(t.tdz_f*t.tdz_f));
								t.entityelement[i].dc_distance = t.tdd_f;
								vObjList.push_back(i);
							}
						}
					}

					//Sort list
					std::sort(vObjList.begin(), vObjList.end(), OrderByObjectDistance());
				}
				if (vObjList.size() > 0)
					nextObjeid = vObjList[0];

				// Merge objects
				int glueid = t.entityelement[nextObjeid].bankindex;
				int glueobj = t.entityelement[nextObjeid].obj;
				if (vObjList.size() > 0 && glueobj > 0 && ObjectExist(t.obj) && ObjectExist(glueobj))
				{
					bool validshader = false;
					if (strcmp(Lower(t.entityprofile[t.entid].effect_s.Get()), "effectbank\\reloaded\\apbr_basic.fx") == 0)
						validshader = true;
					if (strcmp(Lower(t.entityprofile[t.entid].effect_s.Get()), "effectbank\\reloaded\\apbr_tree.fx") == 0)
						validshader = true;
					if (strcmp(Lower(t.entityprofile[t.entid].effect_s.Get()), "effectbank\\reloaded\\entity_basic.fx") == 0)
						validshader = true;
					if (strcmp(Lower(t.entityprofile[t.entid].effect_s.Get()), "effectbank\\reloaded\\apbr_illum.fx") == 0)
						validshader = true;
					if (strcmp(Lower(t.entityprofile[t.entid].effect_s.Get()), "") == 0)
						validshader = true;
					if(t.entityprofile[t.entid].animmax == 0)
						validshader = true;
					if (strcmp(Lower(t.entityprofile[t.entid].effect_s.Get()), "effectbank\\reloaded\\treea_basic.fx") == 0)
						validshader = false;
					if (strcmp(Lower(t.entityprofile[t.entid].effect_s.Get()), "effectbank\\reloaded\\apbr_anim.fx") == 0)
						validshader = false;
					if (strcmp(Lower(t.entityprofile[t.entid].effect_s.Get()), "effectbank\\reloaded\\apbr_animwithtran.fx") == 0)
						validshader = false;
					if (strcmp(Lower(t.entityprofile[t.entid].effect_s.Get()), "effectbank\\reloaded\\apbr_treea.fx") == 0)
						validshader = false;
					if (strcmp(Lower(t.entityprofile[t.entid].effect_s.Get()), "effectbank\\reloaded\\apbr_anim8bone.fx") == 0)
						validshader = false;
					if (strcmp(Lower(t.entityprofile[t.entid].effect_s.Get()), "effectbank\\reloaded\\apbr_animwithtran.fx") == 0)
						validshader = false;
					if (t.entityprofile[t.entid].animmax > 0)
						validshader = false;
							
					if (validshader && t.entityprofile[t.entid].ismarker == 0 && t.entityprofile[t.entid].isebe == 0 && t.entityprofile[t.entid].transparency == 0 && t.entityelement[nextObjeid].staticflag == 1 && t.entityprofile[t.entid].isimmobile != 1 && t.entityelement[t.e].eleprof.isimmobile != 1 && t.entityelement[t.e].eleprof.spawnatstart == 1 && t.entityelement[t.e].staticflag == 1)
					{
						//Validate if same master object.
						sObject* pObject = g_ObjectList[t.obj];
						int instanceonly = 0;
						if (pObject && pObject->pInstanceOfObject) 
						{
							pObject = pObject->pInstanceOfObject;
							instanceonly++;
						}
						sObject* pObject2 = g_ObjectList[glueobj];
						if (pObject2 && pObject2->pInstanceOfObject) 
						{
							pObject2 = pObject2->pInstanceOfObject;
							instanceonly++;
						}
						int iMeshWithTexture = -1;
						int iFrameVertex = 0;
						std::vector< int > vUniqueImageIds;
						vUniqueImageIds.clear();
						if (pObject)
						{
							for (int i = 0; i < pObject->iFrameCount; i++)
							{
								if (pObject->ppFrameList[i]->pMesh)
								{
									sMesh* pMesh = pObject->ppFrameList[i]->pMesh;
									if( pMesh->pTextures )
									{
										//PE: Count how many different images is used.
										vUniqueImageIds.push_back(pMesh->pTextures[0].iImageID);
									}
									iMeshWithTexture = i;
									iFrameVertex += pObject->ppFrameList[i]->pMesh->dwVertexCount;
								}
							}
						}

						//Cant merge too many vertex.
						if (iFrameVertex > 0x8000) 
							instanceonly = 0;

						// Allow users to also drawcall optimize multimaterial objects, if set in fpe.
						if (t.entityprofile[t.entid].drawcalloptimizer != 1)
						{
							if (vUniqueImageIds.size() > 1)
							{
								instanceonly = 0;
							}
							if (iMeshWithTexture >= 0 && pObject->ppFrameList[iMeshWithTexture]->pMesh) 
							{
								//PE: Cant do multi material for now.
								if (pObject->ppFrameList[iMeshWithTexture]->pMesh->bUseMultiMaterial)
									instanceonly = 0;
							}
						}

						// Allows you to force even multi-textured object to be batched.
						int iMultiMatCount = GetMultiMaterialCount(t.obj);
						if (iMultiMatCount > 0) 
						{
							if(t.entityprofile[t.entid].drawcalloptimizer != 1)
								instanceonly = 0;
						}

						// Keep objects distance below DC_DISTANCE for best culling.
						if (pObject && pObject2 && instanceonly >= 2  && t.entityelement[nextObjeid].dc_distance < DC_DISTANCE && g.merged_new_objects < 2890 ) 
						{
							if (pObject == pObject2) 
							{
								//Same master glue it.
								if (GetMeshExist(g.meshlightmapwork) == 1)  DeleteMesh(g.meshlightmapwork);

								float gluescalex = ObjectScaleX(glueobj);
								float gluescaley = ObjectScaleY(glueobj);
								float gluescalez = ObjectScaleZ(glueobj);

								float src_scalex = ObjectScaleX(t.obj);
								float src_scaley = ObjectScaleY(t.obj);
								float src_scalez = ObjectScaleZ(t.obj);

								float scaleadjust = t.entityprofile[t.entid].drawcallscaleadjust;

								int tmpobj = (g.merged_new_objects+100) + 87000; //PE: TODO change 85000
								if (tmpobj < g_iObjectListCount && g_ObjectList[tmpobj])
								{
									if (g_ObjectList[tmpobj]->pFrame)
									{
										DeleteObject(tmpobj);
									}
								}

								CloneObject(tmpobj, t.obj);

								// The lod removal could be improved.
								int bestlod = -1;
								PerformCheckListForLimbs(tmpobj);
								for (t.c = ChecklistQuantity(); t.c >= 1; t.c += -1)
								{
									t.tname_s = Lower(ChecklistString(t.c));

									LPSTR pRightFive = "";
									if (strlen(t.tname_s.Get()) > 5)
										pRightFive = t.tname_s.Get() + strlen(t.tname_s.Get()) - 5;

									if ( (t.tname_s == "lod_0" || stricmp(pRightFive,"_lod0") == 0 ) ) bestlod = 0;
									if ( (t.tname_s == "lod_1" || stricmp(pRightFive,"_lod1") == 0 ) && (bestlod < 0 || bestlod > 1) )  bestlod = 1;
									if ( (t.tname_s == "lod_2" || stricmp(pRightFive,"_lod2") == 0 ) && (bestlod < 0) )  bestlod = 2;

									if (t.entityprofile[t.entid].resetlimbmatrix == 1)
									{
										OffsetLimb(tmpobj, t.c - 1, 0, 0, 0, 0);
									}
								}
								if (bestlod >= 0) 
								{
									for (t.c = ChecklistQuantity(); t.c >= 1; t.c += -1)
									{
										t.tname_s = Lower(ChecklistString(t.c));
										LPSTR pRightFive = "";
										if (strlen(t.tname_s.Get()) > 5)
											pRightFive = t.tname_s.Get() + strlen(t.tname_s.Get()) - 5;

										if (bestlod == 0 && ( t.tname_s == "lod_1" || t.tname_s == "lod_2" || t.tname_s == "lod_3") ) 
										{
											RemoveLimb(tmpobj, t.c - 1);
										}
										if (bestlod == 0 && (stricmp(pRightFive, "_lod1") == 0 || stricmp(pRightFive, "_lod2") == 0 || stricmp(pRightFive, "_lod3") == 0 )) 
										{
											RemoveLimb(tmpobj, t.c - 1);
										}
										if (bestlod == 1 && (t.tname_s == "lod_2" || stricmp(pRightFive, "_lod2") == 0) ) 
										{
											RemoveLimb(tmpobj, t.c - 1);
										}
										if (bestlod == 2 && (t.tname_s == "lod_3" || stricmp(pRightFive, "_lod3") == 0 ) ) 
										{
											RemoveLimb(tmpobj, t.c - 1);
										}
									}
								}

								PositionObject(tmpobj, 0, 0, 0); //PE: Need to be at 0,0,0
								ScaleObject(tmpobj, 100, 100, 100); //PE: no scale.

								int iAfterPolygonTotal = 0;
								int iAfterVertex = 0;

								sObject* pAfterObject = g_ObjectList[tmpobj];
								if (pAfterObject)
								{
									if (pAfterObject->iMeshCount>0)
									{
										for (int iM = 0; iM<pAfterObject->iMeshCount; iM++)
										{
											sMesh* pMesh = pAfterObject->ppMeshList[iM];
											if (pMesh)
											{
												iAfterPolygonTotal += pMesh->iDrawPrimitives;
												iAfterVertex += pMesh->dwVertexCount;
											}
										}
									}
								}

								if (pAfterObject && pAfterObject->iMeshCount == 0) 
								{
									if (GetMeshExist(g.meshlightmapwork) == 1)  
										DeleteMesh(g.meshlightmapwork);
								}
								else 
								{
									MakeMeshFromObject(g.meshlightmapwork, tmpobj);
								}

								if( (iAfterPolygonTotal * 2 < MAX_DRAWPRIMITIVES) && (iAfterVertex * 2 < MAX_DRAWVERTEX)  && GetMeshExist(g.meshlightmapwork) == 1)
								{
									int destobj = g.merged_new_objects + 87000; //PE: TODO change 85000 , perhaps reverse from 90000 ?
									if (g_ObjectList[destobj])
									{
										if (g_ObjectList[destobj]->pFrame)
										{
											DeleteObject(destobj);
										}
									}

									t.tmasterx_f = ObjectPositionX(t.obj);
									t.tmastery_f = ObjectPositionY(t.obj);
									t.tmasterz_f = ObjectPositionZ(t.obj);

									// Use mesh to prevent any transforms.
									MakeObject(destobj, g.meshlightmapwork, -1); 

									int testypos = 0;
									PositionObject(destobj, 0, 0, 0); //PE: Need to be at 0,0,0
									ScaleObject(destobj, 100, 100, 100); //PE: no scale.

									float src_angx = ObjectAngleX(t.obj);
									float src_angy = ObjectAngleY(t.obj);
									float src_angz = ObjectAngleZ(t.obj);

									PerformCheckListForLimbs(destobj);
									AddLimb(destobj, ChecklistQuantity(), g.meshlightmapwork);

									t.tox_f = ObjectPositionX(glueobj) - t.tmasterx_f;
									t.toy_f = ObjectPositionY(glueobj) - t.tmastery_f;
									t.toz_f = ObjectPositionZ(glueobj) - t.tmasterz_f;

									OffsetLimb(destobj, ChecklistQuantity(), t.tox_f, t.toy_f, t.toz_f);

									RotateLimb(destobj, ChecklistQuantity(), ObjectAngleX(glueobj), ObjectAngleY(glueobj), ObjectAngleZ(glueobj));
									ScaleLimb(destobj, ChecklistQuantity(), gluescalex + scaleadjust, gluescaley + scaleadjust, gluescalez + scaleadjust);
									for (int i = ChecklistQuantity()-1; i >= 0; i--) 
									{
										RotateLimb(destobj, i, src_angx, src_angy, src_angz);
										ScaleLimb(destobj, i, ObjectScaleX(t.obj) + scaleadjust, ObjectScaleY(t.obj) + scaleadjust, ObjectScaleZ(t.obj) + scaleadjust);
									}

									bool additionaladded2 = false;
									int glueid2 = 0;
									int glueobj2 = 0;
									if ((iAfterPolygonTotal * 3 < MAX_DRAWPRIMITIVES) && (iAfterVertex * 3 < MAX_DRAWVERTEX) && vObjList.size() > 1 ) 
									{
										nextObjeid = vObjList[1];
										glueid2 = t.entityelement[nextObjeid].bankindex;
										glueobj2 = t.entityelement[nextObjeid].obj;
										sObject* pObject3 = g_ObjectList[glueobj2];
										if (pObject3 && pObject3->pInstanceOfObject) 
										{
											pObject3 = pObject3->pInstanceOfObject;
											if (pObject3 == pObject2) 
											{
												t.tdx_f = t.entityelement[t.e].x - t.entityelement[nextObjeid].x;
												t.tdz_f = t.entityelement[t.e].z - t.entityelement[nextObjeid].z;
												t.tdd_f = Sqrt(abs(t.tdx_f*t.tdx_f) + abs(t.tdz_f*t.tdz_f));
												if (t.tdd_f < DC_DISTANCE) 
												{
													//Object ok add.
													float gluescalex2 = ObjectScaleX(glueobj2);
													float gluescaley2 = ObjectScaleY(glueobj2);
													float gluescalez2 = ObjectScaleZ(glueobj2);

													//Its the same master so reuse g.meshlightmapwork
													PerformCheckListForLimbs(destobj);
													AddLimb(destobj, ChecklistQuantity(), g.meshlightmapwork);

													t.tox_f = ObjectPositionX(glueobj2) - t.tmasterx_f;
													t.toy_f = ObjectPositionY(glueobj2) - t.tmastery_f;
													t.toz_f = ObjectPositionZ(glueobj2) - t.tmasterz_f;

													OffsetLimb(destobj, ChecklistQuantity(), t.tox_f, t.toy_f, t.toz_f);

													RotateLimb(destobj, ChecklistQuantity(), ObjectAngleX(glueobj2), ObjectAngleY(glueobj2), ObjectAngleZ(glueobj2));
													ScaleLimb(destobj, ChecklistQuantity(), gluescalex2 + scaleadjust, gluescaley2 + scaleadjust, gluescalez2 + scaleadjust);
													additionaladded2 = true;
												}
											}
										}
									}

									bool additionaladded3 = false;
									int glueid3 = 0;
									int glueobj3 = 0;
									if ((iAfterPolygonTotal * 4 < MAX_DRAWPRIMITIVES) && (iAfterVertex * 4 < MAX_DRAWVERTEX)  && vObjList.size() > 2) 
									{
										nextObjeid = vObjList[2];
										glueid3 = t.entityelement[nextObjeid].bankindex;
										glueobj3 = t.entityelement[nextObjeid].obj;

										sObject* pObject4 = g_ObjectList[glueobj3];
										if (pObject4 && pObject4->pInstanceOfObject) 
										{
											pObject4 = pObject4->pInstanceOfObject;
											if (pObject4 == pObject2) 
											{
												t.tdx_f = t.entityelement[t.e].x - t.entityelement[nextObjeid].x;
												t.tdz_f = t.entityelement[t.e].z - t.entityelement[nextObjeid].z;
												t.tdd_f = Sqrt(abs(t.tdx_f*t.tdx_f) + abs(t.tdz_f*t.tdz_f));
												if (t.tdd_f < DC_DISTANCE) 
												{
													//Object ok add.
													float gluescalex3 = ObjectScaleX(glueobj3);
													float gluescaley3 = ObjectScaleY(glueobj3);
													float gluescalez3 = ObjectScaleZ(glueobj3);
													//Its the same master so reuse g.meshlightmapwork
													PerformCheckListForLimbs(destobj);
													AddLimb(destobj, ChecklistQuantity(), g.meshlightmapwork);

													t.tox_f = ObjectPositionX(glueobj3) - t.tmasterx_f;
													t.toy_f = ObjectPositionY(glueobj3) - t.tmastery_f;
													t.toz_f = ObjectPositionZ(glueobj3) - t.tmasterz_f;

													OffsetLimb(destobj, ChecklistQuantity(), t.tox_f, t.toy_f, t.toz_f);

													RotateLimb(destobj, ChecklistQuantity(), ObjectAngleX(glueobj3), ObjectAngleY(glueobj3), ObjectAngleZ(glueobj3));
													ScaleLimb(destobj, ChecklistQuantity(), gluescalex3 + scaleadjust, gluescaley3 + scaleadjust, gluescalez3 + scaleadjust);
													additionaladded3 = true;
												}
											}
										}
									}

									bool additionaladded4 = false;
									int glueid4 = 0;
									int glueobj4 = 0;
									if ((iAfterPolygonTotal * 5 < MAX_DRAWPRIMITIVES) && (iAfterVertex * 5 < MAX_DRAWVERTEX)  && vObjList.size() > 3) 
									{
										nextObjeid = vObjList[3];
										glueid4 = t.entityelement[nextObjeid].bankindex;
										glueobj4 = t.entityelement[nextObjeid].obj;
										sObject* pObject5 = g_ObjectList[glueobj4];
										if (pObject5 && pObject5->pInstanceOfObject) 
										{
											pObject5 = pObject5->pInstanceOfObject;
											if (pObject5 == pObject2) 
											{
												t.tdx_f = t.entityelement[t.e].x - t.entityelement[nextObjeid].x;
												t.tdz_f = t.entityelement[t.e].z - t.entityelement[nextObjeid].z;
												t.tdd_f = Sqrt(abs(t.tdx_f*t.tdx_f) + abs(t.tdz_f*t.tdz_f));
												if (t.tdd_f < DC_DISTANCE) 
												{
													//Object ok add.
													float gluescalex4 = ObjectScaleX(glueobj4);
													float gluescaley4 = ObjectScaleY(glueobj4);
													float gluescalez4 = ObjectScaleZ(glueobj4);

													//Its the same master so reuse g.meshlightmapwork
													PerformCheckListForLimbs(destobj);
													AddLimb(destobj, ChecklistQuantity(), g.meshlightmapwork);

													t.tox_f = ObjectPositionX(glueobj4) - t.tmasterx_f;
													t.toy_f = ObjectPositionY(glueobj4) - t.tmastery_f;
													t.toz_f = ObjectPositionZ(glueobj4) - t.tmasterz_f;

													OffsetLimb(destobj, ChecklistQuantity(), t.tox_f, t.toy_f, t.toz_f);

													RotateLimb(destobj, ChecklistQuantity(), ObjectAngleX(glueobj4), ObjectAngleY(glueobj4), ObjectAngleZ(glueobj4));
													ScaleLimb(destobj, ChecklistQuantity(), gluescalex4 + scaleadjust, gluescaley4 + scaleadjust, gluescalez4 + scaleadjust);
													additionaladded4 = true;
												}
											}
										}
									}

									bool additionaladded5 = false;
									int glueid5 = 0;
									int glueobj5 = 0;
									if ((iAfterPolygonTotal * 6 < MAX_DRAWPRIMITIVES) && (iAfterVertex * 6 < MAX_DRAWVERTEX)  && vObjList.size() > 4) 
									{
										nextObjeid = vObjList[4];
										glueid5 = t.entityelement[nextObjeid].bankindex;
										glueobj5 = t.entityelement[nextObjeid].obj;

										sObject* pObject6 = g_ObjectList[glueobj5];
										if (pObject6 && pObject6->pInstanceOfObject) 
										{
											pObject6 = pObject6->pInstanceOfObject;
											if (pObject6 == pObject2) 
											{
												t.tdx_f = t.entityelement[t.e].x - t.entityelement[nextObjeid].x;
												t.tdz_f = t.entityelement[t.e].z - t.entityelement[nextObjeid].z;
												t.tdd_f = Sqrt(abs(t.tdx_f*t.tdx_f) + abs(t.tdz_f*t.tdz_f));
												if (t.tdd_f < DC_DISTANCE) 
												{
													//Object ok add.
													float gluescalex5 = ObjectScaleX(glueobj5);
													float gluescaley5 = ObjectScaleY(glueobj5);
													float gluescalez5 = ObjectScaleZ(glueobj5);

													//Its the same master so reuse g.meshlightmapwork
													PerformCheckListForLimbs(destobj);
													AddLimb(destobj, ChecklistQuantity(), g.meshlightmapwork);

													t.tox_f = ObjectPositionX(glueobj5) - t.tmasterx_f;
													t.toy_f = ObjectPositionY(glueobj5) - t.tmastery_f;
													t.toz_f = ObjectPositionZ(glueobj5) - t.tmasterz_f;

													OffsetLimb(destobj, ChecklistQuantity(), t.tox_f, t.toy_f, t.toz_f);

													RotateLimb(destobj, ChecklistQuantity(), ObjectAngleX(glueobj5), ObjectAngleY(glueobj5), ObjectAngleZ(glueobj5));
													ScaleLimb(destobj, ChecklistQuantity(), gluescalex5 + scaleadjust, gluescaley5 + scaleadjust, gluescalez5 + scaleadjust);
													additionaladded5 = true;
												}
											}
										}
									}

									bool additionaladded6 = false;
									int glueid6 = 0;
									int glueobj6 = 0;
									if ((iAfterPolygonTotal * 7 < MAX_DRAWPRIMITIVES) && (iAfterVertex * 7 < MAX_DRAWVERTEX)  && vObjList.size() > 5) 
									{
										nextObjeid = vObjList[5];
										glueid6 = t.entityelement[nextObjeid].bankindex;
										glueobj6 = t.entityelement[nextObjeid].obj;

										sObject* pObject7 = g_ObjectList[glueobj6];
										if (pObject7 && pObject7->pInstanceOfObject) 
										{
											pObject7 = pObject7->pInstanceOfObject;
											if (pObject7 == pObject2) 
											{
												t.tdx_f = t.entityelement[t.e].x - t.entityelement[nextObjeid].x;
												t.tdz_f = t.entityelement[t.e].z - t.entityelement[nextObjeid].z;
												t.tdd_f = Sqrt(abs(t.tdx_f*t.tdx_f) + abs(t.tdz_f*t.tdz_f));
												if (t.tdd_f < DC_DISTANCE) 
												{
													//Object ok add.
													float gluescalex6 = ObjectScaleX(glueobj6);
													float gluescaley6 = ObjectScaleY(glueobj6);
													float gluescalez6 = ObjectScaleZ(glueobj6);

													//Its the same master so reuse g.meshlightmapwork
													PerformCheckListForLimbs(destobj);
													AddLimb(destobj, ChecklistQuantity(), g.meshlightmapwork);

													t.tox_f = ObjectPositionX(glueobj6) - t.tmasterx_f;
													t.toy_f = ObjectPositionY(glueobj6) - t.tmastery_f;
													t.toz_f = ObjectPositionZ(glueobj6) - t.tmasterz_f;

													OffsetLimb(destobj, ChecklistQuantity(), t.tox_f, t.toy_f, t.toz_f);

													RotateLimb(destobj, ChecklistQuantity(), ObjectAngleX(glueobj6), ObjectAngleY(glueobj6), ObjectAngleZ(glueobj6));
													ScaleLimb(destobj, ChecklistQuantity(), gluescalex6 + scaleadjust, gluescaley6 + scaleadjust, gluescalez6 + scaleadjust);
													additionaladded6 = true;
												}
											}
										}
									}

									// Merge everything into a single mesh.
									DeleteMesh(g.meshlightmapwork);
									MakeMeshFromObject(g.meshlightmapwork, destobj);
									DeleteObject(destobj);
									MakeObject(destobj, g.meshlightmapwork, -1);
									PositionObject(destobj, t.tmasterx_f, t.tmastery_f + testypos, t.tmasterz_f);
									if (t.entityprofile[t.entid].canseethrough == 1)
									{
										SetObjectCollisionProperty(destobj, 1);
									}
									if (t.entityprofile[t.entid].ischaracter == 0)
									{
										if (t.entityprofile[t.entid].collisionmode == 11)
										{
											SetObjectCollisionProperty(destobj, 1);
										}
									}
									if (t.entityprofile[t.entid].cullmode >= 0)
									{
										if (t.entityprofile[t.entid].cullmode != 0)
										{
											SetObjectCull(destobj, 0);
										}
										else
										{
											SetObjectCull(destobj, 1);
										}
									}

									if (GetMeshExist(g.meshlightmapwork) == 1)  
										DeleteMesh(g.meshlightmapwork);
												
									CloneObject(destobj, t.obj, 101); //PE: Copy textures only.

									SetObjectStatic(destobj, true); //Mark as static.

									// Disable if any LOD setup from original object.
									if (bestlod >= 0) 
									{
										SetObjectLOD(destobj, 1, 50000);
										SetObjectLOD(destobj, 2, 50000);
									}

									t.entityelement[t.e].draw_call_obj = destobj;
									t.entityelement[t.e].dc_obj[0] = glueobj;
									t.entityelement[t.e].dc_entid[0] = vObjList[0];
									t.entityelement[vObjList[0]].dc_merged = true;
									if (additionaladded2) 
									{
										t.entityelement[t.e].dc_obj[1] = glueobj2;
										t.entityelement[t.e].dc_entid[1] = vObjList[1];
										t.entityelement[vObjList[1]].dc_merged = true;
										HideObject(glueobj2);
									}
									if (additionaladded3) 
									{
										t.entityelement[t.e].dc_obj[2] = glueobj3;
										t.entityelement[t.e].dc_entid[2] = vObjList[2];
										t.entityelement[vObjList[2]].dc_merged = true;
										HideObject(glueobj3);
									}
									if (additionaladded4) 
									{
										t.entityelement[t.e].dc_obj[3] = glueobj4;
										t.entityelement[t.e].dc_entid[3] = vObjList[3];
										t.entityelement[vObjList[3]].dc_merged = true;
										HideObject(glueobj4);
									}
									if (additionaladded5) 
									{
										t.entityelement[t.e].dc_obj[4] = glueobj5;
										t.entityelement[t.e].dc_entid[4] = vObjList[4];
										t.entityelement[vObjList[4]].dc_merged = true;
										HideObject(glueobj5);
									}
									if (additionaladded6) 
									{
										t.entityelement[t.e].dc_obj[5] = glueobj6;
										t.entityelement[t.e].dc_entid[5] = vObjList[5];
										t.entityelement[vObjList[5]].dc_merged = true;
										HideObject(glueobj6);
									}

									//Hide org objects.
									HideObject(t.obj);
									HideObject(glueobj);
									ShowObject(t.entityelement[t.e].draw_call_obj);
									g.merged_new_objects++;

									// NOTE: Does this mean batched objects will not benefit from these important flags (some models have OpenGL normals and the auto-generated tangents shift about)											
									// PE: Ups my bad , actually we would use pObject->draw_call_obj so it would have all the original settings,
									// PE: but not yet, so just added this again, cant remember why i commented it out ? seams to work fine :)
									DWORD dwArtFlags = 0;
									if (t.entityprofile[t.entid].invertnormal == 1) dwArtFlags = 1;
									if (t.entityprofile[t.entid].preservetangents == 1) dwArtFlags |= 1 << 1;
									SetObjectArtFlags(destobj, dwArtFlags, 0.0f);

								}
								DeleteObject(tmpobj);
							}
						}
					}
				}
			}
		}
	}

	// Create nav mesh from entire level geometry
	timestampactivity(0, "Attempt to create nav mesh");
	if (t.game.gameisexe == 1)
	{
		loadingpageprogress(5);
		game_createnavmeshfromlevel ( true );
	}
	else
	{
		game_createnavmeshfromlevel ( false );
	}

	// Setup variables for main game loop
	t.screenprompt_s = "STARTING GAME";
	if (t.game.gameisexe == 0)  printscreenprompt(t.screenprompt_s.Get()); else loadingpageprogress(5);
	timestampactivity(0, t.screenprompt_s.Get());
	game_init ( );

	// Helpful prompt for start of test game
	if ( t.game.gameisexe == 0 && t.game.runasmultiplayer == 0 ) 
	{
		if (g_bInTutorialMode == true)
		{
			t.visuals.generalprompt_s = "PRESS ESCAPE TO RETURN TO TUTORIAL";
		}
		else
		{
			t.visuals.generalprompt_s = "Press TAB to see framerate or ESCAPE to exit test";
		}
		t.visuals.generalpromptstatetimer=Timer()+123;
	}
	else
	{
		if ( t.game.runasmultiplayer == 1 ) 
		{
			t.visuals.generalpromptstatetimer=Timer()+1000;
			t.visuals.generalprompt_s="Welcome to GameGuru MAX Multiplayer";
		}
		else
		{
			t.visuals.generalpromptstatetimer=0;
		}

		//PE: start any animations that use startanimingame > 0. ( standalone ).
		for (t.tte = 1; t.tte <= g.entityelementlist; t.tte++)
		{
			//PE: issue https://github.com/TheGameCreators/GameGuruRepo/issues/341
			// hide EBE markers
			int iIndex = t.entityelement[t.tte].bankindex;
			if (t.entityprofile[iIndex].isebe != 0)
			{
				t.tobj = t.entityelement[t.tte].obj;
				if (t.tobj>0)
				{
					if (ObjectExist(t.tobj) == 1)
					{
						HideLimb(t.tobj, 0);
					}
				}
			}

			t.entid = t.entityelement[t.tte].bankindex;
			t.tttsourceobj = g.entitybankoffset + t.entityelement[t.tte].bankindex;
			t.tobj = t.entityelement[t.tte].obj;
			if (t.tobj > 0)
			{
				if (ObjectExist(t.tobj) == 1)
				{
					//PE: Possible fix for issues:
					//PE: https://github.com/TheGameCreators/GameGuruRepo/issues/206
					//PE: https://github.com/TheGameCreators/GameGuruRepo/issues/273
					//PE: need testing.
					if (t.entityprofile[t.entid].ischaracter == 1) {
						//Char should always have z depth , but somehow its removed somewhere.

						//PE: check t.entityprofile[t.tentid].zdepth == 0
						EnableObjectZDepth(t.tobj);
					}

					//PE: Make sure we reset all animations. mainly for lua controlled objects like doors
					if (t.entityprofile[t.entid].animmax > 0)
					{
						SetObjectFrame(t.tttsourceobj, 0);
						StopObject(t.tttsourceobj);
						SetObjectFrame(t.tobj, 0);
						StopObject(t.tobj);
					}

					if (t.entityprofile[t.entid].startanimingame > 0) {
						if (t.entityprofile[t.entid].animmax > 0) {
							t.q = t.entityprofile[t.entid].startanimingame - 1;
							SetObjectFrame(t.tttsourceobj, 0);
							LoopObject(t.tttsourceobj, t.entityanim[t.entid][t.q].start, t.entityanim[t.entid][t.q].finish);
							SetObjectFrame(t.tobj, 0);
							LoopObject(t.tobj, t.entityanim[t.entid][t.q].start, t.entityanim[t.entid][t.q].finish);
						}
					}
				}
			}
		}

	}

	//  setup spin values, this rotates the player 360 degrees at the start to kill initial
	//  stutter issues, during this time we don't want low FPS warnings
	t.postprocessings.spinfill=0 ; t.postprocessings.spinfillStartAng_f=CameraAngleY(0);

	//  apply any settings
	timestampactivity(0,"immediate title settings applied");
	if (  t.game.gameisexe == 1 || t.game.runasmultiplayer == 1 ) 
	{
		titles_immediateupdatesound ( );
		titles_immediateupdategraphics ( );
	}

	//  for multiplayer, check if there is a jetpack in the level and grab the model to place on players back
	if (  t.game.runasmultiplayer == 1 ) 
	{
		for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
		{
			t.entid=t.entityelement[t.e].bankindex;
			if (  t.entid>0 ) 
			{
				if (  cstr(Lower(Left(t.entityprofileheader[t.entid].desc_s.Get(),8)))  ==   "jet pack"  && ObjectExist(g.steamplayermodelsoffset+120)  ==  0 ) 
				{
					CloneObject (  g.steamplayermodelsoffset+120,t.entityelement[t.e].obj );
					YRotateObject (  g.steamplayermodelsoffset+120,180 );
					FixObjectPivot (  g.steamplayermodelsoffset+120 );
				}
			}
		}
	}

	//  Clear screen of any artifacts
	titles_loadingpagefree();
	CLS (  Rgb(0,0,0) );

	// In EXE running, override cameras with no mask until title/loading done
	SyncMaskOverride ( 0xFFFFFFFF );

	// resort texture list to ignore objects set to be ignored
	DoTextureListSort ( );

	//PE: Make sure we do not display hud used in last level.
	t.game.activeStoryboardScreen = -1;
	for (int i = 0; i < STORYBOARD_MAXNODES; i++)
	{
		if (Storyboard.Nodes[i].showAtStart)
		{
			t.game.activeStoryboardScreen = i;
			break;
		}
	}


	// if reloading standalone level, need to restore basic stats from LUA save file
	// must now reload preserved state of level when enter it (g_LevelFilename)
	if (!g_Storyboard_Starting_New_Level)
	{
		char pLUACustomLoadCall[256];
		strcpy(pLUACustomLoadCall, "GameLoopLoadStats");
		LuaSetFunction(pLUACustomLoadCall, 2, 0);
		LuaPushInt(g_Storyboard_Current_Level);
		LuaPushInt(t.game.jumplevelresetstates);
		t.game.jumplevelresetstates = 0;
		LuaCall();
	}
	g_Storyboard_Starting_New_Level = false;

	// one final command to improve static physics performance
	physics_finalize ( );

	// Wipe out mouse deltas
	t.tMousemove_f = MouseMoveX() + MouseMoveY() + MouseZ(); t.tMousemove_f  = 0;

	//  Tab mode LOW FPS Warning
	g.tabmode=0 ; g.lowfpstarttimer=Timer();

	//  Game loop will run while single level is in play
	t.huddamage.immunity=1000;
	t.game.gameloop=1;
	g.timeelapsed_f=0;

	// 260220 - for some reason, Social VR sets view 0,0,1,1, and does not set it back!
	// so we do so here to ensure we see the game
	SetCameraView(0, 0, 0, GetDisplayWidth(), GetDisplayHeight());

	// no more prompts, reset system so next time we can have a 2 seconds grace before any prompts (see printscreenprompt)
	t.screenprompt_s = "";
	printscreenprompt(t.screenprompt_s.Get());
	t.postprocessings.fadeinvalue_f = 0.0; //PE: Fade in level.

	// prompt
	if ( g.gproducelogfiles == 2 )
		timestampactivity(0,"main game loop begins in deep debug trace mode");
	else
		timestampactivity(0,"main game loop begins");

	extern int iTriggerGrassTreeUpdate;
	iTriggerGrassTreeUpdate = 5; //PE: Make sure trees and grass height is set after terrain has finish.

	//Stop any menu background music ...
	int iFreeSoundID = g.temppreviewsoundoffset + 2;
	if (SoundExist(iFreeSoundID) == 1 && SoundPlaying(iFreeSoundID) == 1)
	{
		// stop currently playing preview
		StopSound(iFreeSoundID);
	}

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
	iFreeSoundID = g.temppreviewsoundoffset + 5;
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

	//PE: Disable all terrain editor tools in testgame.
	old_render_params2 = ggterrain_global_render_params2.flags2;
	ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP;
	ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;

	//PE: Always start with weapon render on.
	extern bool bHideWeapons;
	bHideWeapons = false;
	extern bool bHideWeaponsMuzzle;
	extern bool bHideWeaponsSmoke;
	bHideWeaponsMuzzle = false;
	bHideWeaponsSmoke = false;

	// The map bounds can optionally be shown in testgame.
	extern void TestLevel_ToggleBoundary(bool, bool);
	TestLevel_ToggleBoundary(t.showtestgame2dbounds, t.showtestgame3dbounds);

	// if this was called from standalone, need to update graphics settings to match visuals just loaded
	if (t.game.gameisexe == 1)
	{
		// ensures when game restarted the last graphics settings refresh the level
		visuals_shaderlevels_update();
	}

	// at the point the level actually starts, use VR in standalone
	if (t.game.gameisexe == 1 && g.vrglobals.GGVREnabled == 2) g_iActivelyUsingVRNow = 1;
}

void game_masterroot_gameloop_afterexitgamemenu(void)
{
	if (t.game.jumplevel_s == "")
	{
		// can override jumplevel with 'advanced warning level filename' when LOAD level from MAIN MENU
		if (strcmp(t.game.pAdvanceWarningOfLevelFilename, "") != NULL)
		{
			t.game.jumplevel_s = t.game.pAdvanceWarningOfLevelFilename;
			if (strlen(Storyboard.gamename) > 0)
			{
				//PE: We use t.game.pAdvanceWarningOfLevelFilename later.
			}
			else
			{
				strcpy(t.game.pAdvanceWarningOfLevelFilename, "");
			}
		}
	}
	if (t.game.jumplevel_s != "")
	{
		//Load level.
		t.game.gameloop = 0;
	}
			
	timestampactivity(0,"leaving options page");
	g.titlesettings.updateshadowsaswell=0;
	//PE: Clear g_iMouseDeltaZ
	MouseMoveZ();
	LuaSetInt("g_MouseWheel", 0); //PE: Reset g_MouseWheel
}

void game_masterroot_gameloop_afterescapepressed(void)
{
	extern void gun_SetObjectSpeed(int, float);
	if ( t.currentgunobj>0 ) { if ( ObjectExist(t.currentgunobj) == 1 ) { gun_SetObjectSpeed (  t.currentgunobj,t.currentgunanimspeed_f); } }
	physics_resumephysics ( );
	entity_resumeanimations ( );
	t.aisystem.cumilativepauses=Timer()-t.tremembertimer;
	game_main_snapshotsoundresume ( );
	t.strwork = ""; t.strwork = t.strwork + "resuming game loop with flag "+Str(t.game.gameloop);
	timestampactivity(0, t.strwork.Get() );
	// Wipe out mouse deltas
	t.tMousemove_f = MouseMoveX() + MouseMoveY() + MouseZ(); t.tMousemove_f  = 0;

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
				SetSoundVolume(iFreeSoundID, t.visuals.iCombatMusicTrackVolume);
			}
		}
	}

	// at the point we leave the in-game menu, resume VR mode while if required
	if (t.game.gameisexe == 1 && g.vrglobals.GGVREnabled == 2) g_iActivelyUsingVRNow = 1;
}

bool bMainLoopRunning = false;

bool game_masterroot_gameloop_loopcode(int iUseVRTest)
{
	// within gameloop, can have a in-game menu loop, so handle this if used
	if (g_iInGameMenuState == 1)
	{
		if (titleslua_main_loopcode() == true)
		{
			game_masterroot_gameloop_afterexitgamemenu();
			game_masterroot_gameloop_afterescapepressed();
			g_iInGameMenuState = 0;
		}
		else
		{
			return false;
		}
	}

	//  Game cycle loop
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"winddown mp_closeconnection");
	if ( t.game.gameloopwinddown == 1 )
	{
		if ( mp_closeconnection() == 1 )
		{
			t.game.gameloopwinddown = 0;
			t.game.gameloop = 0;
		}
	}

	// detect if standalone is a foreground window
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"obtain plrhasfocus");
	if ( t.game.gameisexe == 1 )
	{
		if ( g.gvrmode == 3 )
		{
			t.plrhasfocus = 1;
		}
		else
		{
			HWND hForeWnd = GetForegroundWindow();
			if ( GetWindowHandle() != hForeWnd ) 
				t.plrhasfocus = 0;
			else
				t.plrhasfocus = 1;
		}
	}

	// if controller active, also detect for START button press (same as ESCAPE)
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"controller start button check");
	bool bControllerEscape = false;
	if ( g.gxbox > 0 && JoystickFireXL(9) == 1 ) bControllerEscape = true;

	// VR support can escape to in-game menu with button A
	if (g.vrglobals.GGVREnabled > 0 && g_iActivelyUsingVRNow == 1)
	{
		if (GGVR_RightController_Button1() == 1) bControllerEscape == true;
	}

	//  trigger options page or exit test level
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"escape button check");
	if ( EscapeKey() == 1 || bControllerEscape == true ) 
	{
		// at the point we enter the in-game menu, stop VR mode if required
		if (t.game.gameisexe == 1 && g.vrglobals.GGVREnabled == 2) g_iActivelyUsingVRNow = 0;

		// can perform some extra debug snapshots when enter in-game menu - useful!
		if (g.gproducelogfiles == 2) GGTerrain::GGTerrain_DebugOutputFlattenedAreas();

		t.tremembertimer=Timer();
		game_main_snapshotsound ( );
		while ( EscapeKey() != 0 ) {}
		physics_pausephysics ( );
		entity_pauseanimations ( );
		extern void gun_SetObjectSpeed(int, float);
		if ( t.currentgunobj > 0 ) { if ( ObjectExist(t.currentgunobj)==1 ) { gun_SetObjectSpeed (  t.currentgunobj,0) ; } }
		if ( t.playercontrol.jetobjtouse>0 ) 
		{
			if ( ObjectExist(t.playercontrol.jetobjtouse) == 1  )  SetObjectSpeed (  t.playercontrol.jetobjtouse,0 );
		}
		if ( t.game.gameisexe == 0 ) // no menu in multiplayer test mode && t.game.runasmultiplayer == 0 ) 
		{
			if ( t.game.runasmultiplayer == 1 )
			{
				// wait until connection closed, then exit game loop
				t.game.gameloopwinddown = 1; 
			}
			else
			{
				// leave right away
				t.game.gameloop=0; 
				t.game.levelloop=0; 
				t.game.masterloop=0;
			}
			if ( t.conkit.editmodeactive == 1 ) 
			{
				//conkitedit_switchoff ( );
			}
		}
		else
		{
			g.titlesettings.updateshadowsaswell = 1;
			timestampactivity(0, "entering options page");
			titleslua_init();
			g_iInGameMenuState = 1;
			titleslua_main("gamemenu");
			if (titleslua_main_loopcode() == true)
			{
				// title systen left right away, continue
				game_masterroot_gameloop_afterexitgamemenu();
			}
			else
			{
				// cycle
				return false;
			}
		}
		game_masterroot_gameloop_afterescapepressed();
		g_iInGameMenuState = 0;
	}

	// Fade in gamescreen (using post process shader)
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"fade game screen logic");
	if ( t.postprocessings.fadeinvalue_f<1.0 ) 
	{
		// Hide Lua Sprites
		HideOrShowLUASprites ( true );
		if ( t.postprocessings.fadeinvalue_f <= 0 )
		{
			// only if in test game mode, standalone already set volume values (in title.lua)						
			if ( t.game.gameisexe == 0 )
			{
				// set music and sound global volumes
				audio_volume_init ( );
			}
			else if (!FileExist("savegames\\sounds.dat")) 
			{
				//PE: This is not always in standalone ? , so:
				audio_volume_init();
			}

			// and update internal volume values so music update can use volumes!
			audio_volume_update ( );
			//PE: LUA first calls has already been made, so restore the settings after this reset.
			void restore_last_lua_volumes_settings(void);
			restore_last_lua_volumes_settings();
		}
		//PE: Wicked seams a littler faster then classic , strange ?
		t.postprocessings.fadeinvalue_f = t.postprocessings.fadeinvalue_f + (g.timeelapsed_f*0.08f);
		if (  t.postprocessings.fadeinvalue_f >= 1.0f ) 
		{
			//PE: Disable 3D hiding. we are all done and ready to play.
			extern int iKeepBackgroundForFrames;
			iKeepBackgroundForFrames = 0;
			t.postprocessings.fadeinvalue_f=1.0f;
			//g.globals.hidelowfpswarning = 0; // this overrides the SETUP.INI setting
			HideOrShowLUASprites ( false );
			EnableAllSprites(); // the disable is called in DarkLUA by ResetFade() black out command when load game position
		}
		extern bool bFakeStandaloneTest;
		if ( (t.game.gameisexe == 1 || bFakeStandaloneTest ) && t.postprocessings.fadeinvalue_f < 1.0) //PE: Only standalone and , not on last frame.
		{
			extern bool bImGuiInTestGame;
			extern bool bRenderTabTab;
			extern bool bBlockImGuiUntilNewFrame;
			extern bool bImGuiRenderWithNoCustomTextures;
			extern bool g_bNoGGUntilGameGuruMainCalled;
			extern bool bImGuiFrameState;

			if ((bImGuiInTestGame) && !bRenderTabTab && !bImGuiFrameState)
			{
				//We need a new frame.
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();
				bRenderTabTab = true;
				bBlockImGuiUntilNewFrame = false;
				bImGuiRenderWithNoCustomTextures = false;
				extern bool bSpriteWinVisible;
				bSpriteWinVisible = false;
			}

			ImGuiViewport* mainviewport = ImGui::GetMainViewport();
			if (mainviewport)
			{
				ImDrawList* drawlist = ImGui::GetForegroundDrawList(mainviewport);
				if (drawlist)
				{
					ImVec4 monitor_col = ImVec4(0.0, 0.0, 0.0, 1.0 - t.postprocessings.fadeinvalue_f); //Fade in.
					drawlist->AddRectFilled(ImVec2(-1, -1), ImGui::GetMainViewport()->Size, ImGui::GetColorU32(monitor_col));
				}
			}
		}
		t.postprocessings.fadeinvalueupdate=1;
	}

	// handle fading
	bMainLoopRunning = true;

	//  Immunity when respawn
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"handle player immunity");
	if (  t.huddamage.immunity>0 ) 
	{
		t.huddamage.immunity=t.huddamage.immunity-(10*g.timeelapsed_f);
		if (  t.huddamage.immunity<0  )  t.huddamage.immunity = 0;
	}

	//  Run all game subroutines		
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling game_main_loop");
	game_main_loop ( );

	//  Update screen
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling game_sync");
	game_sync ( );

	// determine if end of game loop
	if (t.game.gameloop != 1)
		return true;
	else
		return false;
}

void game_masterroot_gameloop_afterloopcode(int iUseVRTest)
{
	// at the point we leave the loop, stop VR mode when leaving the level if required
	if (t.game.gameisexe == 1 && g.vrglobals.GGVREnabled == 2) g_iActivelyUsingVRNow = 0;

	// must stop extra thread right away
	extern void GuruLoopStopExtraThread(void);
	GuruLoopStopExtraThread();

	// first save current level stats before reset LUA
	// must now preserve state of level when leave it
	char pLUACustomSaveCall[256];
	strcpy ( pLUACustomSaveCall, "GameLoopSaveStats" );
	LuaSetFunction ( pLUACustomSaveCall, 1, 0 ); 
	LuaPushInt(g_Storyboard_Current_Level);
	LuaCall ();

	// free any lua activity (restore FOV if ingame activity there)
	timestampactivity(0,"finalising LUA system before reset");
	lua_quitting(); 

	// Rest any internal game variables
	game_main_stop ( );

	//PE: Enable collision.
	for (t.e = 1; t.e <= g.entityelementlist; t.e++)
	{
		int tentid = t.entityelement[t.e].bankindex;
		int tobj = t.entityelement[t.e].obj;
		if (tentid > 0 && tobj > 0)
		{
			if (t.entityprofile[tentid].ischaracter == 0)
			{
				int iShape = t.entityprofile[tentid].collisionmode;
				if (t.entityelement[t.e].eleprof.iOverrideCollisionMode != -1)
					iShape = t.entityelement[t.e].eleprof.iOverrideCollisionMode;
				//PE: no max fpe is using canseethrough. else if (t.entityprofile[tentid].canseethrough == 1) iShape = 11;
				if (iShape == 11)
				{
					sObject* pObject = g_ObjectList[tobj];
					if (pObject)
					{
						WickedCall_SetDisableCollision(pObject, false);
					}
				}
			}
		}
	}


	//PE: Draw call optimizer
//	if (!g.disable_drawcall_optimizer)
	{
		//PE: restore all states. delete all batched objects.
		for (t.e = 1; t.e <= g.entityelementlist; t.e++)
		{
			t.entid = t.entityelement[t.e].bankindex;
			t.obj = t.entityelement[t.e].obj;

			if (t.obj > 0 && t.e < g.entityelementlist && ObjectExist(t.obj) == 1)
			{
				if (t.entityelement[t.e].draw_call_obj > 0) {

					if (t.entityelement[t.e].draw_call_obj > 0 && ObjectExist(t.entityelement[t.e].draw_call_obj) == 1) {
						DeleteObject(t.entityelement[t.e].draw_call_obj);
						t.entityelement[t.e].draw_call_obj = 0;
						if(ObjectExist(t.obj) == 1)
							ShowObject(t.obj);
					}

					if (t.entityelement[t.e].dc_obj[0] > 0 && ObjectExist(t.entityelement[t.e].dc_obj[0]) == 1)
						ShowObject(t.entityelement[t.e].dc_obj[0]);
					if (t.entityelement[t.e].dc_obj[1] > 0 && ObjectExist(t.entityelement[t.e].dc_obj[1]) == 1)
						ShowObject(t.entityelement[t.e].dc_obj[1]);
					if (t.entityelement[t.e].dc_obj[2] > 0 &&  ObjectExist(t.entityelement[t.e].dc_obj[2]) == 1)
						ShowObject(t.entityelement[t.e].dc_obj[2]);
					if (t.entityelement[t.e].dc_obj[3] > 0 && ObjectExist(t.entityelement[t.e].dc_obj[3]) == 1)
						ShowObject(t.entityelement[t.e].dc_obj[3]);
					if (t.entityelement[t.e].dc_obj[4] > 0 && ObjectExist(t.entityelement[t.e].dc_obj[4]) == 1)
						ShowObject(t.entityelement[t.e].dc_obj[4]);
					if (t.entityelement[t.e].dc_obj[5] > 0 && ObjectExist(t.entityelement[t.e].dc_obj[5]) == 1)
						ShowObject(t.entityelement[t.e].dc_obj[5]);

					t.entityelement[t.e].dc_obj[0] = 0;
					t.entityelement[t.e].dc_obj[1] = 0;
					t.entityelement[t.e].dc_obj[2] = 0;
					t.entityelement[t.e].dc_obj[3] = 0;
					t.entityelement[t.e].dc_obj[4] = 0;
					t.entityelement[t.e].dc_obj[5] = 0;

					t.entityelement[t.e].dc_entid[0] = 0;
					t.entityelement[t.e].dc_entid[1] = 0;
					t.entityelement[t.e].dc_entid[2] = 0;
					t.entityelement[t.e].dc_entid[3] = 0;
					t.entityelement[t.e].dc_entid[4] = 0;
					t.entityelement[t.e].dc_entid[5] = 0;
				}
			}
		}
	}

	bool bFreeLevelAfterLuaScreen = true;

	//PE: Moved here as standalone will delete all objects, so we could not free DCO objects.
	//  Free any level resources
	if (!bFreeLevelAfterLuaScreen)
	{
		timestampactivity(0, "game_freelevel"); //PE: Additional debug to remove
		game_freelevel();
	}
	else
	{
		bulletholes_free();
		// free any HUD screen objects
		if (ObjectExist(g.hudscreen3dobjectoffset) == 1) DeleteObject(g.hudscreen3dobjectoffset);
		hud_free();
		game_stopallsounds();
		lua_freeprompt3d();
		lua_freeallperentity3d();
		lighting_free();
		darkai_free();
		BPhys_ClearDebugDrawData();
	}

	// must reset LUA here for clean end-game-screens
	// ensure LUA is completely reset before loading new ones in
	// the free call is because game options menu init, but not freed back then
	timestampactivity(0,"resetting LUA system");
	titleslua_free ( );
	LuaReset (  );
	extern std::unordered_map<int, sFrame*> lastHitFrame;
	lastHitFrame.clear();

	//PE: restore waterline.
	t.terrain.waterliney_f = g.gdefaultwaterheight;

	reset_env_particles();
	void delete_notused_decal_particles(void);
	//PE: Make sure to delete not used decal particles.

	delete_notused_decal_particles();

	//PE: Clear all wicked particle effects created by lua.
	void CleanUpEmitterEffects(void);
	CleanUpEmitterEffects();

	//PE: restore sun position for editor.
	t.terrain.sundirectionx_f = t.terrain.skysundirectionx_f;
	t.terrain.sundirectiony_f = t.terrain.skysundirectiony_f;
	t.terrain.sundirectionz_f = t.terrain.skysundirectionz_f;

	// 240316 - additional cleanup
	mp_freefadesprite ( );

	// Advance level to 'next one' or 'win game'
	timestampactivity(0,"end of level stage");
	if ( t.game.gameisexe == 1 )
	{
		timestampactivity(0,"game is standalone exe");
		if ( t.game.quitflag == 0 ) 
		{
			timestampactivity(0,"game has not quit");
			bool bUseOldSystem = true;
			if ( strlen(Storyboard.gamename) > 0)
			{
				bUseOldSystem = false;
				if (t.game.lostthegame == 1)
				{
					// Get output link to lose screen from current level node.
					int actiontype = FindNextLevel(g_Storyboard_Current_Level, g_Storyboard_Current_fpm , 1);
					if (actiontype == 2)
					{
						//Found a lose output to a screen.
						std::string script = g_Storyboard_Current_fpm;
						replaceAll(script, ".lua", "");
						char tmp[MAX_PATH];
						sprintf(tmp, "Project LUA script : %s", script.c_str());
						timestampactivity(0, tmp);
						sky_hide();
						if (script != "title")
						{
							titleslua_init();
							titleslua_main((char *)script.c_str());
							//PE: We need a blocking run or screen is not displayed. t.game.levelloop = 0; will start title screen.
							titleslua_blocking_run();
							titleslua_free();
						}
						sky_show();
						t.game.levelloop = 0;
					}
					else
					{
						timestampactivity(0, "Project LUA script : lose");
						sky_hide();
						titleslua_init();
						titleslua_main("lose");
						//PE: We need a blocking run or screen is not displayed. t.game.levelloop = 0; will start title screen.
						titleslua_blocking_run();
						titleslua_free();
						sky_show();
						t.game.levelloop = 0;
					}
				}
				else
				{
					if (strcmp(t.game.pAdvanceWarningOfLevelFilename, "") != NULL)
					{
						//From load game
						t.game.jumplevel_s = t.game.pAdvanceWarningOfLevelFilename;
						strcpy(t.game.pAdvanceWarningOfLevelFilename, "");
						//PE: Find g_Storyboard_Current_Level from t.game.jumplevel_s.
						for (int i = 0; i < STORYBOARD_MAXNODES; i++)
						{
							if (Storyboard.Nodes[i].used)
							{
								if (pestrcasestr(Storyboard.Nodes[i].level_name, t.game.jumplevel_s.Get()) != 0)
								{
									g_Storyboard_Current_Level = i;
									strcpy(g_Storyboard_Current_fpm, Storyboard.Nodes[i].level_name);
								}
							}
						}
					}
					else
					{
						//Get next level.
						int actiontype = FindNextLevel(g_Storyboard_Current_Level, g_Storyboard_Current_fpm);
						if (actiontype == 2 || actiontype == 3)
						{
							//We got a screen jump to that screen.
							std::string script = g_Storyboard_Current_fpm;
							replaceAll(script, ".lua", "");
							char tmp[MAX_PATH];
							sprintf(tmp, "Project LUA script : %s", script.c_str());
							timestampactivity(0, tmp);
							if (script != "title")
							{
								//PE: Block for one frame so we do not see old screens.
								extern bool bBlockImGuiUntilNewFrame;
								bBlockImGuiUntilNewFrame = true;
								sky_hide();
								titleslua_init();
								titleslua_main((char *)script.c_str());
								//PE: We need a blocking run or screen is not displayed. t.game.levelloop = 0; will start title screen.
								titleslua_blocking_run();
								titleslua_free();
								sky_show();
							}
							if (actiontype == 3)
							{
								// is the actual game won screen, need to leave level loop afer this!
								t.game.levelloop = 0;
							}
						}
						else
						{
							//We got a new level, use it.
							std::string nextlevel = g_Storyboard_Current_fpm;
							replaceAll(nextlevel, "mapbank\\", "");
							replaceAll(nextlevel, ".fpm", "");
							char tmp[MAX_PATH];
							sprintf(tmp, "Project t.game.jumplevel_s : %s", nextlevel.c_str());
							timestampactivity(0, tmp);
							t.game.jumplevel_s = nextlevel.c_str();

							//PE: Door collision not working on second level, fixed in script.
							//PE: https://github.com/Dark-Basic-Software-Limited/GameGuruRepo/issues/6246
						}
					}
				}
			}
			if (bUseOldSystem)
			{
				//PE: issue https://github.com/TheGameCreators/GameGuruRepo/issues/444
				if (Len(t.game.jumplevel_s.Get()) > 0)
				{
					//  goes around and loads this level name
					timestampactivity(0, "game is loading non-linear level map:");
					timestampactivity(0, "t.game.jumplevel_s.Get()");
				}
				else
				{
					// win, lose or next level pages
					if (t.game.lostthegame == 1)
					{
						//titles_gamelostpage ( );
						timestampactivity(0, "LUA script : lose");
						sky_hide();
						titleslua_init();
						titleslua_main("lose");
						//PE: We need a blocking run or screen is not displayed. t.game.levelloop = 0; will start title screen.
						titleslua_blocking_run();
						titleslua_free();
						sky_show();
						t.game.levelloop = 0;
					}
					else
					{
						t.game.level = t.game.level + 1;
						if (t.game.level > t.game.levelmax)
						{
							timestampactivity(0, "LUA script : win");
							sky_hide();
							titleslua_init();
							titleslua_main("win");
							//PE: We need a blocking run or screen is not displayed. t.game.levelloop = 0; will start title screen.
							titleslua_blocking_run();
							titleslua_free();
							sky_show();
							t.game.levelloop = 0;
						}
						else
						{
							timestampactivity(0, "LUA script : nextlevel");
							sky_hide();
							titleslua_init();
							titleslua_main("nextlevel");
							//PE: We need a blocking run or screen is not displayed.
							titleslua_blocking_run();
							sky_show();
						}
					}
				}
			}
		}
	}

	if (bFreeLevelAfterLuaScreen)
	{
		game_freelevel();
		//PE: Try to avoid the screen between win/loose... and next screen (show blank map).
		extern int iBlockRenderingForFrames;
		extern bool g_bNoSwapchainPresent;
		iBlockRenderingForFrames = 5;
		g_bNoSwapchainPresent = true;
	}


	t.game.quitflag=0;

	//  If was in multiplayer session, no level loop currently
	if (  t.game.runasmultiplayer == 1 ) 
	{
		t.game.levelloop=0;
	}

	// PE: Dump image usage after level.
	if (g.memgeneratedump == 1) 
	{
		timestampactivity(0, "DumpImageList after freeing level data.");
		DumpImageList(); 
	}

	g_RecastDetour.cleanupDebugRender();
	g_bShowRecastDetourDebugVisuals = false;

	// 250619 - very large levels can fragment 32 bit memory after a few levels
	// so this mode will restart the executable, and launch the new level
	// crude solution until 64 bit allows greater memory referencing
	if ( t.game.allowfragmentation == 2 )
		t.game.levelloop = 0;

	if (t.gamevisuals.bEndableAmbientMusicTrack)
	{
		//PE: Stop any ambient music tracks.
		int iFreeSoundID = g.temppreviewsoundoffset + 3;
		if (SoundExist(iFreeSoundID) == 1 && SoundPlaying(iFreeSoundID) == 1)
		{
			StopSound(iFreeSoundID);
		}
		if (SoundExist(iFreeSoundID) == 1) DeleteSound(iFreeSoundID);
	}
	int iFreeSoundID = g.temppreviewsoundoffset + 5;
	if (SoundExist(iFreeSoundID) == 1 && SoundPlaying(iFreeSoundID) == 1)
	{
		StopSound(iFreeSoundID);
	}
	if (SoundExist(iFreeSoundID) == 1) DeleteSound(iFreeSoundID);
	//PE: restore old terrain settings.
	ggterrain_global_render_params2.flags2 = old_render_params2;

	//PE: Always turn back on weapon render.
	extern bool bHideWeapons;
	bHideWeapons = false;
	extern bool bHideWeaponsMuzzle;
	extern bool bHideWeaponsSmoke;
	bHideWeaponsMuzzle = false;
	bHideWeaponsSmoke = false;

}

bool game_masterroot_levelloop_initcode(int iUseVRTest)
{
	// first hide rendering of 3D while we set up
	SyncMaskOverride ( 0 );

	//  Optionally set resolution for game and setup for dependencies
	timestampactivity(0,"_game_setresolution");
	if (  t.game.set.resolution == 1 ) 
	{
		game_setresolution ( );
		game_postresolutionchange ( );
		t.game.set.resolution=0;
	}

	//  One-off splash screen or animation
	if (  t.game.set.initialsplashscreen == 1 ) 
	{
		t.game.set.initialsplashscreen=0;
	}

	//  Setup level progression settings
	t.game.firstlevelinitializesanygameprojectlua = 123;
	t.game.level=1;
	t.game.levelmax=1;
	t.game.levelloop=1;
	t.game.levelendingcycle=0;
	t.game.lostthegame=0;
	t.game.jumplevel_s="";
	strcpy ( t.game.pAdvanceWarningOfLevelFilename, "" );

	//  specify first level to load (same name as executable)
	if (  t.game.gameisexe == 1 ) 
	{
		t.tapp_s=Appname();
		for ( t.n = Len(t.tapp_s.Get()) ; t.n >= 1 ; t.n+= -1 )
		{
			if (  t.tapp_s.Get()[t.n-1] == '\\' || t.tapp_s.Get()[t.n-1] == '/' ) 
			{
				t.tapp_s=Right(t.tapp_s.Get(),Len(t.tapp_s.Get())-t.n);
				break;
			}
		}
		t.game.jumplevel_s=Left(t.tapp_s.Get(),Len(t.tapp_s.Get())-4);


		//PE: Check if we are using a storyboard project
		strcpy(Storyboard.gamename, "");
		load_storyboard(t.game.jumplevel_s.Get());
		if (strlen(Storyboard.gamename) > 0)
		{
			//PE: Got a project. find first level to load.
			FindFirstLevel(g_Storyboard_First_Level_Node, g_Storyboard_First_fpm);
			g_Storyboard_Current_Level = g_Storyboard_First_Level_Node;
			strcpy(g_Storyboard_Current_fpm, g_Storyboard_First_fpm);
			//Clean name.
			
			std::string sLevelTitle = g_Storyboard_First_fpm;
			replaceAll(sLevelTitle, ".fpm", "");
			replaceAll(sLevelTitle, "mapbank\\", "");
			t.game.jumplevel_s = sLevelTitle.c_str();
		}
	}

	//  Title init - If this is just a test game, we only need to set default volumes
	timestampactivity(0,"_titles_init");
	if (  t.game.gameisexe == 1 || t.game.runasmultiplayer == 1 ) 
	{
		titles_init ( );
	}
	else
	{
		t.gamesounds.sounds=100;
		t.gamesounds.music=100;
	}

	// Do title page
	timestampactivity(0,"_titles_titlepage");
	if (t.game.gameisexe == 1 && t.game.ignoretitle == 0)
	{
		sky_hide();
		titles_loadingpageinit();
		titleslua_init();

		// this is a special flag set when quit from game (avoids rogue IMGUI renders that have been deleted in the restart)
		extern bool bBlockImGuiUntilFurtherNotice;
		bBlockImGuiUntilFurtherNotice = false;

		// 250619 - for straight-through level loading, still need to handle title resources
		// (which include sound loading/playing - fixes 3D sound delay issue?!?)
		if (g.iStandaloneIsReloading == 2)
		{
			titleslua_main_inandout("title");
		}
		else
		{
			bool bValid = false;
			int FindFirstSplashNode(void);
			int nodeid = FindFirstSplashNode();
			if (nodeid >= 0)
			{
				int index = 1;

				int iLinkTo = Storyboard.Nodes[nodeid].output_linkto[0];
				int iLinkScreen = -1;
				for (int i = 0; i < STORYBOARD_MAXNODES; i++)
				{
					if (Storyboard.Nodes[i].used)
					{
						for (int l = 0; l < STORYBOARD_MAXOUTPUTS; l++)
						{
							if (iLinkTo > 0 && iLinkTo == Storyboard.Nodes[i].input_id[l])
							{
								iLinkScreen = i;
								break;
							}
						}
					}
				}
				if (iLinkScreen >= 0)
				{
					//PE: Check if we got a video outlink.
					for (int ll = 0; ll < STORYBOARD_MAXWIDGETS; ll++)
					{
						if (Storyboard.Nodes[iLinkScreen].widget_used[ll])
						{
							if (Storyboard.Nodes[iLinkScreen].widget_type[ll] == STORYBOARD_WIDGET_VIDEO)
							{
								if (Storyboard.Nodes[iLinkScreen].widget_action[ll] == STORYBOARD_ACTIONS_GOTOSCREEN)
								{
									bValid = true;
									break;
								}
							}
						}
					}
					if (bValid)
					{
						// screens can have same name (old corruption issue), so new method to identify screen by node
						std::string node_ident_name = ":node:";
						node_ident_name += std::to_string(iLinkScreen);
						t.s_s = node_ident_name.c_str();
						lua_switchpage();
						//bLuaPageClosing = true; //always stop music.
						//iRet = STORYBOARD_ACTIONS_GOTOSCREEN;
					}
				}
			}
			if (bValid)
			{
				titleslua_main(t.s_s.Get());
				strcpy(t.game.pSwitchToLastPage, "title");
			}
			else
			{
				// start title system loop
				titleslua_main("title");
			}
			return true;
		}
	}

	// no title system loop, continue
	game_masterroot_levelloop_initcode_aftertitleloop();
	return false;
}

void game_masterroot_levelloop_initcode_aftertitleloop(void)
{
	// if game executable and not ignoring title system
	if (t.game.gameisexe == 1 && t.game.ignoretitle == 0)
	{
		titleslua_free();
		sky_show();
	}

	// 250619 - can relaunch game executable at a specific level (reduce memory fragmentation)
	if ( g.iStandaloneIsReloading == 2 )
	{
		t.game.jumplevel_s = g.sStandaloneIsReloadingLevel;
		t.luaglobal.gamestatechange = atoi(g.sStandaloneIsReloadingLevelGameStatChange.Get());
		g.sStandaloneIsReloadingLevel = "";
		g.sStandaloneIsReloadingLevelGameStatChange = "";
	}

	// Standaline Multiplayer HOST/JOIN screen
	if ( t.game.runasmultiplayer == 1 ) 
	{
		// Multiplayer init
		mp_fullinit();
		g.mp.mode = MP_MODE_MAIN_MENU;
		timestampactivity(0,"_titles_steampage");
		t.game.cancelmultiplayer=0;
		SetCameraView (  0,0,1,1 );
		titles_steampage ( );
		if ( t.game.cancelmultiplayer == 1 ) 
		{
			// user selected BACK (cancel multiplayer)
			mp_fullclose();
			t.game.levelloop=0;
		}
		else
		{
			// proceed into level loop where multiplayer spawn markers are detected and ghosts loaded
		}
	}

	// Initialise gun system (transcends per-level initialisations)
	t.game.levelplrstatsetup = 1;
	gun_restart ( );
	gun_resetactivateguns ( );
}

bool game_masterroot_levelloop_loopcode(int iUseVRTest)
{
	// Level loop will run while level progression is in progress
	if (t.game.levelloop != 1)
		return true;
	else
		return false;
}

void game_masterroot_levelloop_afterloopcode(int iUseVRTest)
{
	// Free any game resources
	game_freegame ( );

	if ( t.game.runasmultiplayer == 1 ) 
	{
		mp_free_game ( );
		mp_cleanupGame ( );
		if (  g.mp.goBackToEditor  ==  1 ) 
		{
			g.mp.goBackToEditor = 0;
			t.game.masterloop = 0;
		}
	}

	// get rid of debris and particles that may be lingering
	explosion_cleanup ( );

	ravey_particles_hide_all_particles();

	// if ignored title, exit now
	if (  t.game.ignoretitle == 1 && t.game.runasmultiplayer == 0  )  t.game.masterloop = 0;

	// Master loop end
	t.game.allowfragmentation_mainloop = t.game.masterloop;
	if (t.game.allowfragmentation == 0 || t.game.allowfragmentation == 2) t.game.masterloop = 0; //break;
}

void game_masterroot_initcode(int iUseVRTest)
{
	// prevent any VR if VRtest is off
	if (t.game.gameisexe == 1)
	{
		// in standalonme mode, GGVRUsingVRSystem set 1 elsehwere as needed
	}
	else
	{
		// in test level mode, can toggle this
		if (iUseVRTest == 0) g.vrglobals.GGVRUsingVRSystem = 0;
	}
	g_iActivelyUsingVRNow = iUseVRTest;

	//  Load all one-off non-graphics assets
	timestampactivity(0,"_game_oneoff_nongraphics");
	game_oneoff_nongraphics ( );

	// Pick the HUD screen that should be shown at the start of the level
	t.game.activeStoryboardScreen = -1;
	for (int i = 0; i < STORYBOARD_MAXNODES; i++)
	{
		if (Storyboard.Nodes[i].showAtStart)
		{
			t.game.activeStoryboardScreen = i;
			break;
		}
	}

	//  Master loop will run until whole game terminated
	t.game.masterloop=1;
}

bool game_masterroot_loopcode(int iUseVRTest)
{
	// state engine to handle nested loops (master / level / gameloop)
	if (g_iMasterRootState == 0)
	{
		if (game_masterroot_levelloop_initcode(iUseVRTest) == true)
		{
			// go to title loop first (below)
			g_iMasterRootState = 51;
		}
		else
		{
			// title system not used, continue
			g_iMasterRootState = 1;
		}
	}
	if (g_iMasterRootState == 1)
	{
		if (game_masterroot_levelloop_loopcode(iUseVRTest) == true)
		{
			g_iMasterRootState = 6;
		}
		else
		{
			g_iMasterRootState = 2;
		}
	}
	if (g_iMasterRootState == 2)
	{
		game_masterroot_gameloop_initcode(iUseVRTest);
		g_iMasterRootState = 3;
	}
	if (g_iMasterRootState == 3)
	{
		if (game_masterroot_gameloop_loopcode(iUseVRTest) == true)
		{
			g_iMasterRootState = 4;
		}
	}
	if (g_iMasterRootState == 4)
	{
		game_masterroot_gameloop_afterloopcode(iUseVRTest);
		g_iMasterRootState = 5;
	}
	if (g_iMasterRootState == 5)
	{
		if (t.game.levelloop == 1 && Len(t.game.jumplevel_s.Get()) > 0 )
		{
			g_iMasterRootState = 1;
		}
		else
		{
			g_iMasterRootState = 6;
		}
	}
	if (g_iMasterRootState == 6)
	{
		game_masterroot_levelloop_afterloopcode(iUseVRTest);
		g_iMasterRootState = 0;
	}

	// title system state control
	if (g_iMasterRootState == 51)
	{
		if ( titleslua_main_loopcode() == true )
		{
			game_masterroot_levelloop_initcode_aftertitleloop();
			g_iMasterRootState = 2;
		}
	}

	// determine if end of master loop
	if (t.game.masterloop != 1)
		return true;
	else
		return false;
}

void game_masterroot_afterloopcode(int iUseVRTest)
{
	// End splash if EXE is advertising
	if ( t.game.set.endsplash == 1 ) 
	{
		t.game.set.endsplash=0;
	}

	// restore VR activity (vrtest flag has done its job) 
	g.vrglobals.GGVRUsingVRSystem = 1;
	g_iActivelyUsingVRNow = 0;
	master.StopVR();
	// restore normal rendering activity when finish game run
	SyncMaskOverride ( 0xFFFFFFFF );
	// cannot rely on postprocess to restore, so do so here when return
	SetCameraView ( 0, 0, 0, GetDisplayWidth(), GetDisplayHeight() );
}

void game_masterroot(int iUseVRTest)
{
	bool bRunLoop = true;
	game_masterroot_initcode(iUseVRTest);
	g_iMasterRootState = 0;
	while (bRunLoop == true)
	{
		if (game_masterroot_loopcode(iUseVRTest) == true) bRunLoop = false;
	}
	game_masterroot_afterloopcode(iUseVRTest);
}

void game_setresolution ( void )
{
	//  set game resolution here
	t.multisamplingfactor=0;
	t.multimonitormode=0;
	SetDisplayMode (  GetDesktopWidth(),GetDesktopHeight(),32,g.gvsync,t.multisamplingfactor,t.multimonitormode );
	SyncOn (   ); SyncRate (  0  ); Sync (   ); SetAutoCamOff (  );
	DisableEscapeKey (  );
}

void game_postresolutionchange ( void )
{
}

void game_oneoff_nongraphics ( void )
{
	// Trigger a sound stops initial slow-down?
	if (g.silentsoundoffset > 0)
	{
		if (SoundExist(g.silentsoundoffset) == 1)
		{
			PlaySound(g.silentsoundoffset);
			PositionSound(g.silentsoundoffset, 0, 0, 0);
		}
	}

	//  Force all weapons into weapon slots (initial default start)
	gun_gatherslotorder ( );
}

void game_loadinentitiesdatainlevel ( void )
{
	// Load player settings
	timestampactivity(0,"Load player config");
	mapfile_loadplayerconfig ( );

	//PE: Free master on load.
	bool bRetainMasterObjects = false;
	if (t.game.gameisexe == 1)
	{
		static cstr old_level_name = "";
		if (g.projectfilename_s == old_level_name)
		{
			bRetainMasterObjects = true;
		}
		old_level_name = g.projectfilename_s;
	}

	if (!bRetainMasterObjects)
	{
		// Load entity bank
		t.screenprompt_s = "LOADING ENTITY BANK";
		if (t.game.gameisexe == 0) printscreenprompt(t.screenprompt_s.Get()); else loadingpageprogress(5);
		timestampactivity(0, t.screenprompt_s.Get());
		entity_loadbank();
	}

	// Load entity elements
	t.screenprompt_s="LOADING ENTITY ELEMENTS";
	if ( t.game.gameisexe == 0 ) printscreenprompt(t.screenprompt_s.Get()); else loadingpageprogress(5);
	timestampactivity(0,t.screenprompt_s.Get());
	timestampactivity(0, "s2:entity_loadelementsdata()");
	entity_loadelementsdata ( );
	timestampactivity(0, "e2:entity_loadelementsdata()");

	// LB: quick sanity check to screen out corrupt entityparent ID references
	for (t.e = 1; t.e <= g.entityelementlist; t.e++)
	{
		t.entid = t.entityelement[t.e].bankindex;
		if (t.entid > 0 && t.entid >= t.entityprofile.size())
		{
			// ensures can never reference an out of bounds entityparent ID
			t.entityelement[t.e].bankindex = 0;
		}
	}
}

void game_loadinleveldata ( void )
{
	//  Load waypoints
	t.screenprompt_s="LOADING WAYPOINTS DATA";
	if (  t.game.gameisexe == 0  )  printscreenprompt(t.screenprompt_s.Get()); else loadingpageprogress(5);
	timestampactivity(0,t.screenprompt_s.Get());
	waypoint_loaddata ( );
	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );
	waypoint_recreateobjs ( );
	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );

	//  Load terrain
	t.screenprompt_s="LOADING TERRAIN DATA";
	if (  t.game.gameisexe == 0  )  printscreenprompt(t.screenprompt_s.Get()); else loadingpageprogress(5);
	timestampactivity(0,t.screenprompt_s.Get());
	terrain_loaddata ( );
	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );

	//  Recreate all entities in level
	t.screenprompt_s="CREATE ENTITY OBJECTS";
	if (  t.game.gameisexe == 0  )  printscreenprompt(t.screenprompt_s.Get()); else loadingpageprogress(5);
	timestampactivity(0,t.screenprompt_s.Get());

	char debug[MAX_PATH];
	sprintf(debug, "Setup objects: %ld", g.entityelementlist);
	timestampactivity(0, debug);
	extern bool bNoHierarchySorting;
	bNoHierarchySorting = true;
	extern int iInstancedTotal;
	iInstancedTotal = 0;

	for ( t.tupdatee = 1 ; t.tupdatee<=  g.entityelementlist; t.tupdatee++ )
	{
		entity_updateentityobj ( );
		if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );
	}

	sprintf(debug, "Instanced objects: %ld", iInstancedTotal);
	timestampactivity(0, debug);
	bNoHierarchySorting = false;

	t.terrain.terrainpainteroneshot=0;

	//  default start position is edit-camera XZ (Y done in physics init call)
	t.terrain.playerx_f=25000;
	t.terrain.playery_f=0;
	t.terrain.playerz_f=25000;
	t.terrain.playerax_f=0.0;
	t.terrain.playeray_f=0.0;
	t.terrain.playeraz_f=0.0;
	t.camangy_f=0;

	//  hide all markers
	t.screenprompt_s="GAME OBJECT CLEANUP";
	if (  t.game.gameisexe == 0  )  printscreenprompt(t.screenprompt_s.Get()); else loadingpageprogress(5);
	timestampactivity(0,t.screenprompt_s.Get());
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.obj=t.entityelement[t.e].obj;
		if (  t.obj>0 ) 
		{
			if (  ObjectExist(t.obj) == 1 ) 
			{
				t.entid=t.entityelement[t.e].bankindex;
				if (  t.entityprofile[t.entid].ismarker != 0 ) 
				{
					//  all markers must be hidden
					HideObject (  t.obj );
				}
				if (  t.entityprofile[t.entid].addhandlelimb>0 ) 
				{
					//  hide decal handles
					HideLimb (  t.obj,t.entityprofile[t.entid].addhandlelimb );

				}
			}
		}
	}
	waypoint_hideall ( );
}

float GetWAVtoLIPProgress(void);

void game_preparelevel ( void )
{
	// need the latest refreshed gunlist for each new level
	extern bool g_bGunListNeedsRefreshing;
	g_bGunListNeedsRefreshing = true;

	//  Init music system first to make sure nothing is playing during the load sequence
	// 271115 - has to be here as LUA triggers Play Music during its INIT but if MUSIC_INIT was
	// called last it would stop the default music from playing, but setting volume to zero 
	// will achieve the same result of keeping music silent until ready
	music_init ( );
	t.audioVolume.musicFloat = 0;
	t.audioVolume.soundFloat = 0;

	//  Load all assets required to perform level
	timestampactivity(0,"_game_preparelevel:");

	//  Switch on post process if it was switched off (init called later in _finally subroutine)
	postprocess_on ( );

	//  (re)load any player sounds (player style specified in player start marker)
	material_loadplayersounds ( );

	//  init character sound
	character_sound_init ( );

	//  particles
	ravey_particles_init ( );
	reset_env_particles ( );

	// bulletholes return at last
	bulletholes_init();

	//  Allow Steam to refresh (so does not stall)
	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );

	//  HUD graphics
	t.screenprompt_s = "LOADING HUD GRAPHICS";
	timestampactivity(0,t.screenprompt_s.Get());
	hud_init ( );

	//  Load sky spec (for any shaders later that require sun-pos)
	t.screenprompt_s="LOADING SKY";
	if (  t.game.gameisexe == 0  )  printscreenprompt(t.screenprompt_s.Get()); else loadingpageprogress(5);
	timestampactivity(0,t.screenprompt_s.Get());
	t.terrainskyspecinitmode=0;

	//PE: Remember sun angle.
	float oSx = t.visuals.SunAngleX;
	float oSy = t.visuals.SunAngleY;
	float oSz = t.visuals.SunAngleZ;

	sky_skyspec_init( false );

	//PE: In wicked we want to restore the sun angle from the map and not use skyspec.ini settings. (only when loading a old level).
	if (t.visuals.skyindex == 0 || t.visuals.bDisableSkybox)
	{
		//PE: Only if we re not using a simple skybox.
		t.terrain.sunrotationx_f = t.visuals.SunAngleX = oSx;
		t.terrain.sunrotationy_f = t.visuals.SunAngleY = oSy;
		t.terrain.sunrotationz_f = t.visuals.SunAngleZ = oSz;
	}

	//  Load in HUD Layer assets
	t.screenprompt_s = "LOADING HUD LAYERS";
	timestampactivity(0,t.screenprompt_s.Get());
	hud_scanforhudlayers ( );

	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );

	//  setup terrain for in-game
	t.screenprompt_s = "LOADING WATER SYSTEM";
	timestampactivity(0,t.screenprompt_s.Get());
	terrain_start_play ( );
	terrain_water_init ( );

	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );

	//  setup terrain for in-game
	t.screenprompt_s="LOADING A.I SYSTEM";
	if (  t.game.gameisexe == 0  )  printscreenprompt(t.screenprompt_s.Get()); else loadingpageprogress(5);
	timestampactivity(0,t.screenprompt_s.Get());
	darkai_init ( );
	t.aisystem.containerpathmax=0;
	t.screenprompt_s="PREPARING A.I SYSTEM";
	if (  t.game.gameisexe == 0  )  printscreenprompt(t.screenprompt_s.Get()); else loadingpageprogress(5);
	timestampactivity(0,t.screenprompt_s.Get());

	//  Reset waypoint for game activity
	if (t.showtestgameelements == 0)
	{
		t.screenprompt_s = "RESETTING WAYPOINTS A.I";
		timestampactivity(0, t.screenprompt_s.Get());
		waypoint_reset();
	}

	//  setup entities
	t.screenprompt_s="CREATING ENTITY A.I";
	timestampactivity(0,t.screenprompt_s.Get());
	entity_init ( );
	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );

	// the editor's legacy particle effects switched off and hidden for a test game, each particle entity as the game wants it
	if ( t.game.gameisexe == 0 )
	{
		extern void newparticle_testgamebegin ( void );
		newparticle_testgamebegin ( );
	}

	// Load weapon system
	t.screenprompt_s = "LOADING NEW WEAPONS";
	if (t.game.gameisexe == 0)  printscreenprompt(t.screenprompt_s.Get()); else loadingpageprogress(5);
	timestampactivity(0, t.screenprompt_s.Get());
	gun_activategunsfromentities ();
	gun_setup ();
	gun_loadonlypresent ();
	entity_init_nowcreateattachments();
	entity_init_overwritefireratesettings();

	//  create A.I entities for all characters
	t.screenprompt_s="SETTING UP CHARACTERS";
	timestampactivity(0,t.screenprompt_s.Get());
	darkai_setup_characters ( );
	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );

	//  When all static entities added, complete obstacle map
	if (  g.gskipobstaclecreation == 0 ) 
	{
		t.screenprompt_s="CREATING A.I OBSTACLES";
		timestampactivity(0,t.screenprompt_s.Get());
	}

	//  setup infinilights
	t.screenprompt_s="PREPARING DYNAMIC LIGHTS";
	timestampactivity(0,t.screenprompt_s.Get());
	lighting_init ( );
	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );

	//  particles/flak/debris required
	decal_activatedecalsfromentities ( );
	material_activatedecals ( );
	void weapon_projectile_activatedecals(void);
	weapon_projectile_activatedecals();
	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );

	// load in decals (and new particle decals)
	t.screenprompt_s = "LOADING DECAL EFFECTS";
	timestampactivity(0, t.screenprompt_s.Get());
	decal_loadonlyactivedecals ( );
	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );

	// load required scripts
	void GunInitAnimationSettings(void);
	GunInitAnimationSettings();
	lua_init ( );
	lua_scanandloadactivescripts ( );

	// if still generating LIP file, wait here
	t.screenprompt_s="GENERATING LIP SYNC DATA";
	//if (  t.game.gameisexe == 0  )  printscreenprompt(t.screenprompt_s.Get()); else loadingpageprogress(5); looks wrong, most levels dont have any lips..
	timestampactivity(0,t.screenprompt_s.Get());
	float fProgressOfGeneration = GetWAVtoLIPProgress();
	while ( fProgressOfGeneration > 0.0f && fProgressOfGeneration < 1.0f )
	{
		fProgressOfGeneration = GetWAVtoLIPProgress();
		if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );
		Sleep(50);
	}

	// load entity sounds and video
	entity_loadactivesoundsandvideo ( );
}

void game_preparelevel_forplayer ( void )
{
	//  Player settings
	physics_player_init ( );

	//  player start position must come from level setuo info
	t.terrain.gameplaycamera=0;
	SetCurrentCamera (  t.terrain.gameplaycamera );
	PositionCamera (  t.terrain.gameplaycamera,t.terrain.playerx_f,t.terrain.playery_f,t.terrain.playerz_f );
	PointCamera (  t.terrain.gameplaycamera,0,100,0 );
	RotateCamera (  t.terrain.gameplaycamera,t.terrain.playerax_f,t.terrain.playeray_f,t.terrain.playeraz_f );

	//  should be done in visual update call now!!
	SetCameraRange (  t.terrain.gameplaycamera, DEFAULT_NEAR_PLANE, DEFAULT_FAR_PLANE );
	SetCameraAspect (  t.terrain.gameplaycamera,1.325f );
	SetCameraFOV (  t.terrain.gameplaycamera,75 );
}

//Dave Performance - setup character entities for shader switching
void game_setup_character_shader_entities ( bool bMode )
{
}

extern int howManyMarkers;

void game_preparelevel_finally ( void )
{
	// Tell shadow maps to restore previous vis list rather than sort it
	g_bInEditor = false;
	t.performanceCameraDrawDistance = 0;
	t.haveSetupShaderSwitching = false;

	g.isGameBeingPlayed = true;

	// This is used to record when we have switched lighting modes so we don't do it constantly
	g.inGameLightingMode = 0;
	if (  t.game.runasmultiplayer == 1 ) 
	{
		mp_load_guns ( );
	}

	//Free up spawns sent to lua
	t.entitiesActivatedForLua.clear();

	// Don't switch off guns!
	g.noPlayerGuns = false;
	g.remembergunid = 0;

	//  Generate mega texture of terrain paint for VERY LOW shaders
	if (  t.terrain.generatedsupertexture == 0 ) 
	{
		t.screenprompt_s="GENERATING TERRAIN SUPER TEXTURE";
		timestampactivity(0,t.screenprompt_s.Get());
		t.terrain.generatedsupertexture = 1;
	}

	//  Initiate post process system (or reactivate it)
	t.screenprompt_s="INITIALIZING POSTPROCESS";
	timestampactivity(0,t.screenprompt_s.Get());
	postprocess_init ( );
	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );
	timestampactivity(0,"postprocessing initialized");

	//  Ensure correct shaders in play
	visuals_shaderlevels_update ( );

	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );

	//  Initialise Construction Kit
	//conkit_init ( );

	//  Init physics
	t.screenprompt_s="INITIALIZING PHYSICS";
	if (  t.game.gameisexe == 0  )  printscreenprompt(t.screenprompt_s.Get()); else loadingpageprogress(5);
	timestampactivity(0,t.screenprompt_s.Get());
	physics_init ( );

	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );

	//  Activate Occlusion System
	timestampactivity(0,"Activate Occlusion System");
	if (  g.globals.occlusionmode == 1 ) 
	{
		//  Once all assets in place, create occlusion database
		// 110416 - commented out again, turns out when occluder in THREAD is flagged to end, it clears the occluder list
		//CPU3DClear(); // 260316 - dont know why the clear was commented out, it is ESSENTIAL to ensure levels dont mess each other up
		CPU3DSetCameraIndex (  0 );
		//  Occlusion poly list can have a variable size to help performance
		CPU3DSetPolyCount ( t.visuals.occlusionvalue );
		//  Set occludees for all entities in level
		t.toccobj=g.occlusionboxobjectoffset;
		for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
		{
			if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );
			t.entid=t.entityelement[t.e].bankindex;
			if (  t.entityprofile[t.entid].ismarker == 0 ) 
			{
				t.obj=t.entityelement[t.e].obj;
				if (  t.obj>0 ) 
				{
					if (  ObjectExist(t.obj) == 1 ) 
					{
						//Dave Performance, if the object is static, make it static for the game
						if ( t.entityelement[t.e].staticflag == 1 )
							SetObjectStatic(t.obj , true );
						else
							SetObjectStatic(t.obj , false );		

						//  reject any objects too small as they won't make good occluders
						if (  t.entityelement[t.e].staticflag == 1 && t.entityprofile[t.entid].notanoccluder == 0 && ((ObjectSizeX(t.obj,1)>MINOCCLUDERSIZE && ObjectSizeY(t.obj,1)>MINOCCLUDERSIZE ) || (ObjectSizeZ(t.obj,1)>MINOCCLUDERSIZE && ObjectSizeY(t.obj,1)>MINOCCLUDERSIZE )) ) 
						{
							//  OCLUDER AND OCLUDEE
							//  OPTIMIZE; we can make this even faster by using a simple math-QUAD in the render part
							//  instead of storing the verts of a 12 polygon box!!
							if ( t.entityelement[t.e].eleprof.isocluder == 1 )
							{
								if (  t.entityprofile[t.entid].physicsobjectcount>0 && t.entityprofile[t.entid].collisionmode == 40 ) 
								{
									t.tocy_f=ObjectSizeY(t.obj)/2.0;
									for ( t.tcount = 0 ; t.tcount<=  t.entityprofile[t.entid].physicsobjectcount-1; t.tcount++ )
									{
										if (  ObjectExist(t.toccobj) == 1  )  DeleteObject (  t.toccobj );
										MakeObjectBox (  t.toccobj, t.entityphysicsbox[t.entid][t.tcount].SizeX * (t.entityprofile[t.entid].scale * 0.01 ), t.entityphysicsbox[t.entid][t.tcount].SizeY * (t.entityprofile[t.entid].scale * 0.01 ), t.entityphysicsbox[t.entid][t.tcount].SizeZ * (t.entityprofile[t.entid].scale * 0.01 ) );
										OffsetLimb (  t.toccobj, 0, t.entityphysicsbox[t.entid][t.tcount].OffX * (t.entityprofile[t.entid].scale * 0.01 ) , t.entityphysicsbox[t.entid][t.tcount].OffY * (t.entityprofile[t.entid].scale * 0.01 ) , t.entityphysicsbox[t.entid][t.tcount].OffZ * (t.entityprofile[t.entid].scale * 0.01 ) );
										RotateLimb (  t.toccobj, 0, t.entityphysicsbox[t.entid][t.tcount].RotX , t.entityphysicsbox[t.entid][t.tcount].RotY , t.entityphysicsbox[t.entid][t.tcount].RotZ );
										MakeMeshFromObject (  g.meshgeneralwork,t.toccobj );
										DeleteObject (  t.toccobj );
										MakeObject (  t.toccobj,g.meshgeneralwork,0 );
										DeleteMesh (  g.meshgeneralwork );
										PositionObject (  t.toccobj,ObjectPositionX(t.obj),ObjectPositionY(t.obj)+t.tocy_f,ObjectPositionZ(t.obj) );
										RotateObject (  t.toccobj,ObjectAngleX(t.obj),ObjectAngleY(t.obj),ObjectAngleZ(t.obj) );
										SetObjectMask (  t.toccobj, 0 );
										SetObjectCollisionProperty (  t.toccobj,1 );
										CPU3DAddOccluder (  t.toccobj );
										++t.toccobj;
									}
								}
								else
								{
									//  polygon occluders TOO EXPENSIVE
									//  NOTE; Suggest a new set of polygons inside each model marked 'occluder'
									//  which when detected are submitted here via the OBJ
									// Dave Performance - adding everything in, even poly stuff
									// Don't add in collisionmode 50-59 (trees) as they make poor occluders
									if ( t.entityprofile[t.entid].collisionmode < 50 || t.entityprofile[t.entid].collisionmode > 59 ) 
										CPU3DAddOccluder (  t.obj );
								}
							}
						}
					}
				}

				// Add as an occludee
				if ( t.entityelement[t.e].eleprof.isocludee == 1 )
				{
					// Also let the occluder know if it is a character or not as characters are shown for longer
					// compared to other objects
					if ( t.entityprofile[t.entid].ischaracter == 1 ) 
					{
						// Also add character creator parts, if this is a cc character
						CPU3DAddOccludee ( t.obj , true );
					}
					else
					{
						CPU3DAddOccludee ( t.obj , false );
					}
				}
			}
			else
				howManyMarkers++;
		}
		while (  t.toccobj<g.occlusionboxobjectoffsetfinish ) 
		{
			if (  ObjectExist(t.toccobj) == 1  )  DeleteObject (  t.toccobj );
			++t.toccobj;
		}

		//  also occlude any weapons carried by characters
		if (  g.entityattachmentindex>0 ) 
		{
			for ( t.obj = g.entityattachmentsoffset+1 ; t.obj<=  g.entityattachmentsoffset+g.entityattachmentindex; t.obj++ )
			{
				if (  t.obj>0 ) 
				{
					if (  ObjectExist(t.obj) == 1 ) 
					{
						CPU3DAddOccludee (  t.obj , false );
					}
				}
			}
		}
	}

	//  Final states of entities and call ALL entity script INIT functions
	timestampactivity(0,"Entity Initiations");
	entity_initafterphysics ( );
	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );
	lua_launchallinitscripts ( );
	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );

	//  Once player start known, fill veg area instantly
	timestampactivity(0,"Fill Veg Areas");
	t.completelyfillvegarea=1;
	if ( t.game.runasmultiplayer == 1 ) mp_refresh ( );

	//  Force a shader update to ensure correct shadows are used at start
	t.visuals.refreshcountdown=5;

	// Let steam know we have finished loading
	if ( t.game.runasmultiplayer == 1 ) 
	{
		g.mp.finishedLoadingMap = 1;
	}

	// The start marker may have given the play an initial gun, so lets call physics_player_refreshcount just incase it has
	physics_player_refreshcount();
}

void game_stopallsounds ( void )
{
	// stop ALL sounds
	for ( t.s = 1; t.s <= 65535; t.s++ )
	{
		if ( SoundExist(t.s) == 1 ) 
		{
			if ( t.s >= g.musicsoundoffset && t.s <= g.musicsoundoffsetend ) 
			{
				// if from game menu, do not stop dynamic music sound progress
				StopSound (  t.s );
			}
			else
			{
				StopSound ( t.s );
			}
		}
	}
}

void game_freelevel ( void )
{
	// remove any bulletholes
	bulletholes_free();

	// free any HUD screen objects
	if (ObjectExist(g.hudscreen3dobjectoffset) == 1) DeleteObject (g.hudscreen3dobjectoffset);

	//Reset loading bar percentage
	extern int g_iLastProgressPercentage;
	g_iLastProgressPercentage = 0;
	g.isGameBeingPlayed = false;

	//  hide any jetpacks, etc
	hud_free ( );

	//  stop ALL sounds
	game_stopallsounds();

	// Delete occluder thread and close event handles (moved from free game which is too late for multi-level games)
	if ( g_pOccluderThread )
	{
		//Let the occluder know its time to finish
		g_occluderOn = false;

		//Dave Performance - let the occluder thread know it is okay to begin
		if ( g_hOccluderBegin ) SetEvent ( g_hOccluderBegin );

		//Dave Performance - wait for occluder to finish first
		if ( g_hOccluderEnd ) WaitForSingleObject ( g_hOccluderEnd, INFINITE );
	}
	
	// remove bits created by LUA scripts
	lua_freeprompt3d();
	lua_freeallperentity3d();

	//  close down game entities
	entity_free ( );

	//  Delete any infinilights
	lighting_free ( );

	//  AI finish
	darkai_free ( );

	//  free physics
	physics_free ( );

	// Clear the static physics data so we can get the dynamic data.
	BPhys_ClearDebugDrawData();

	//  deselect current gun and hide all gun objects
	gun_free ( );
	gun_freeguns ( );
	gun_removempgunsfromlist ( );

	//  remove water and sky effects
	if( t.game.gameisexe >= 1) //Keep sky for editor.
	sky_free ( );

	terrain_water_free ( );

	//  restore terrain from in-game
	terrain_stop_play ( );

	//  free Construction Kit
	//conkit_free ( );

	//  free any visual leftovers
	visuals_free ( );

	//  free character sound
	character_sound_free ( );

	//  close script system
	lua_free ( );

	//  free projectiles
	weapon_projectile_free ( );

	// finally delete entity element objs (only if standalone)
	if ( t.game.gameisexe == 1 )
	{
		//PE: Need to delete all particle emtters.
		gpup_deleteAllEffects();

		// only for standalone as test game needs entities for editor :)
		entity_delete();
		ClearAnyLightMapInternalTextures();

		//PE: Delete all entitybank textures used.
		if (g.standalonefreememorybetweenlevels == 1)
			ClearAnyEntitybankInternalTextures();
	}
}

void game_init ( void )
{
	//  Machine independent speed
	game_timeelapsed_init ( );

	//  Load slider menu resources
	sliders_init ( );

	//  Trigger all visuals to update
	t.visuals.refreshshaders=1;

	//  HideMouse (  and clear deltas )
	game_hidemouse ( );

	//  Last thing before main game loop
	physics_beginsimulation ( );

	//  Reset game checkpoint
	t.playercheckpoint.stored=1;
	t.playercheckpoint.x=CameraPositionX(0);
	t.playercheckpoint.y=CameraPositionY(0);
	t.playercheckpoint.z=CameraPositionZ(0);
	t.playercheckpoint.a=CameraAngleY(0);

	//  Reset hardware flags with each new level map
	t.hardwareinfoglobals.noterrain=0;
	t.hardwareinfoglobals.nowater=0;
	t.hardwareinfoglobals.noguns=0;
	t.hardwareinfoglobals.nolmos=0;
	t.hardwareinfoglobals.nosky=0;
	t.hardwareinfoglobals.nophysics=0;
	t.hardwareinfoglobals.noai=0;
	t.hardwareinfoglobals.nograss=0;
	t.hardwareinfoglobals.noentities=0;

	//  initialise panel resources
	panel_init ( );

	//  construction kit f9 mode cursor
	t.characterkitcontrol.oldF9CursorEntid = 0;
}

void game_freegame ( void )
{
	// Ensure we are switched back to full res
	t.bHiResMode = true;
	t.bOldHiResMode = true;
	SetCameraHiRes ( t.bHiResMode );

	//Free up spawns sent to lua
	t.entitiesActivatedForLua.clear();

	// Ensure Steam chat sprite has gone
	if (  SpriteExist(g.steamchatpanelsprite)  )  DeleteSprite (  g.steamchatpanelsprite );

	// Restore fonts
	loadallfonts();

	//Switch back to using an additional sort visibility list in the editor to avoid flickering when adding new entities
	g_bInEditor = true;
	//  Free file map access game uses
	physics_player_free ( );

	//  Free slider resources, not needed for title
	sliders_free ( );

	// Free LUA Sprites and Images
	FreeLUASpritesAndImages ();

	//  Free resources not specific to a single level before returning to title page
	postprocess_free ( );

	if (  t.game.runasmultiplayer == 1 ) 
	{
		mp_free_game ( );
	}

	panel_free ( );

	// free temp bitmap used to redirect 2D drawing to an image
	panel_Free2DDrawing();

	//Dave Performance - switch entities and profile object back to animating
	for ( int c = 0 ; c < (int)t.characterBasicEntityList.size() ; c++ )
	{		
		int tobj = t.entityelement[t.characterBasicEntityList[c]].obj;
		if ( tobj > 0 )
		{
			if ( ObjectExist ( tobj ) == 1 )
			{
				if ( !t.characterBasicEntityListIsSetToCharacter[c] )
				{
					SetObjectEffect( tobj , t.characterBasicShaderID );
					SetObjectEffect( g.entitybankoffset+t.entityelement[t.characterBasicEntityList[c]].bankindex , t.characterBasicShaderID );				
				}
			}
		}
	}

	t.characterBasicEntityList.clear();
	t.characterBasicEntityListIsSetToCharacter.clear();

	// switch all objects back to dynamic before heading back to the editor
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.entid=t.entityelement[t.e].bankindex;
		if (  t.entityprofile[t.entid].ismarker == 0 ) 
		{
			t.obj=t.entityelement[t.e].obj;

			if (  t.obj>0 ) 
			{
				SetObjectStatic(t.obj , false );
			}
		}
	}

	//Dave Performance - restore any ignored objects and get the texture list sortid again to include them
	ClearIgnoredObjects ( );
	DoTextureListSort ( );
}


void game_hidemouse(void)
{
	if (g.mouseishidden == 0) {
		g.mouseishidden = 1;
		HideMouse();
		t.null = MouseMoveX() + MouseMoveY();
		return;
	}
}

void game_showmouse(void)
{
	if (g.mouseishidden == 1) {
		g.mouseishidden = 0;
		ShowMouse();
		//Tab Tab bug ?
		t.null = MouseMoveX() + MouseMoveY();
		return;
	}
}


void game_timeelapsed_init ( void )
{
	//  Machine indie speed
	t.TimerFrequency_f=PerformanceFrequency();
	t.StartFrameTime=PerformanceTimer();
	t.ElapsedTime_f=0.0;
	t.LastTimeStamp_f=timeGetSecond();
	g.timeelapsed_f=0;
}

void game_timeelapsed ( void )
{
	// Calculate time between cycles
	float fThisTimeCount = timeGetSecond();
	t.ElapsedTime_f = fThisTimeCount - t.LastTimeStamp_f;
	g.timeelapsed_f = t.ElapsedTime_f * 20.0;
	t.LastTimeStamp_f = fThisTimeCount;

	//  Cap to around 25fps so that leaps in movement/speed not to severe!
	if (  g.timeelapsed_f>0.75f  )  g.timeelapsed_f = 0.75f;
	if (  g.timeelapsed_f<0.00833f  )  g.timeelapsed_f = 0.00833f;

	void update_per_frame_effects(void);
	update_per_frame_effects();
}

void game_main_snapshotsoundloopcheckpoint ( bool bPauseAndResumeFromGameMenu )
{
	// remember any looping sounds but exclude weapon and rocket sounds
	if (bPauseAndResumeFromGameMenu==true || (t.playercontrol.disablemusicreset == 0 && bPauseAndResumeFromGameMenu == false) )
	{
		for ( t.s = g.soundbankoffset ; t.s<= g.soundbankoffsetfinish; t.s++ )
		{
			if (  t.soundloopcheckpoint[t.s] != 2 ) 
			{
				t.soundloopcheckpoint[t.s] = 0;
				if (  SoundExist(t.s) == 1 )
				{
					if (SoundPlaying(t.s) == 1)
					{
						t.soundloopcheckpoint[t.s] = 1;
						if (SoundLooping(t.s) == 1)
						{
							t.soundloopcheckpoint[t.s] = 3;
						}
					}
					if (bPauseAndResumeFromGameMenu == true)
					{
						PauseSound(t.s);
					}
					else
					{
						// record state only
					}
				}
			}
		}
	}
}

void game_main_snapshotsoundresumecheckpoint (bool bPauseAndResumeFromGameMenu)
{
	if (bPauseAndResumeFromGameMenu == true || (t.playercontrol.disablemusicreset == 0 && bPauseAndResumeFromGameMenu == false))
	{
		for ( t.s = g.soundbankoffset ; t.s <= g.soundbankoffsetfinish; t.s++ )
		{
			if ( t.soundloopcheckpoint[t.s] != 2 ) 
			{
				if (SoundExist(t.s) == 1)
				{
					if ( t.soundloopcheckpoint[t.s] != 0 )
					{
						if (bPauseAndResumeFromGameMenu == true)
						{
							// simply resume for game menu (and reset state flag)
							ResumeSound(t.s);
							t.soundloopcheckpoint[t.s] = 0;
						}
						else
						{
							// must recreate loop or play sounds (do not reset state flag as may restart several times)
							if (t.soundloopcheckpoint[t.s] == 3) LoopSound(t.s);
							if (t.soundloopcheckpoint[t.s] == 1) PlaySound(t.s);
						}
					}
					else
					{
						if (bPauseAndResumeFromGameMenu == true)
						{
							// game menu pause resume does not need to stop sounds
						}
						else
						{
							// stop any rogue sounds still chiming when restart at checkpoint
							StopSound(t.s);
						}
					}
				}
			}
		}
	}
}

void game_main_snapshotsound()
{
	// preserve any checkpoint state
	for (int i = g.soundbankoffset; i <= g.soundbankoffsetfinish; i++)
	{
		t.soundloopstore[i] = t.soundloopcheckpoint[i];
	}
	// grab current state into checkpoint and pause sounds
	bool bPauseAndResumeFromGameMenu = true;
	game_main_snapshotsoundloopcheckpoint(bPauseAndResumeFromGameMenu);
	// store these into game menu for when resume later
	for (int i = g.soundbankoffset; i <= g.soundbankoffsetfinish; i++)
	{
		t.soundloopgamemenu[i] = t.soundloopcheckpoint[i];
	}
}

void game_main_snapshotsoundresume()
{
	// copy game menu states into checkpoint
	for (int i = g.soundbankoffset; i <= g.soundbankoffsetfinish; i++)
	{
		t.soundloopcheckpoint[i] = t.soundloopgamemenu[i];
	}
	// resume all that was paused
	bool bPauseAndResumeFromGameMenu = true;
	game_main_snapshotsoundresumecheckpoint(bPauseAndResumeFromGameMenu);
	// restore storec checkpoint to resume game
	for (int i = g.soundbankoffset; i <= g.soundbankoffsetfinish; i++)
	{
		t.soundloopcheckpoint[i] = t.soundloopstore[i];
	}
}

extern bool g_bEarlyExcludeMode;
int oldSpaceKey = 0;

extern int NumberOfObjects;
extern int NumberOfGroupsShown;
extern int NumberOfObjectsShown;

extern int iEnterGodMode;
float camx, camy, camz, gcamax, gcamay, gcamaz;
float godcamx, godcamy, godcamz, godcamax, godcamay, godcamaz;


void game_main_loop ( void )
{	
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	// this trigger informs the in-game extra thread to begin for one frame (extra stuff done at end (when GPU/Wicked doing its thing)
	extern bool g_bInGameCPUFrameComplete;
	g_bInGameCPUFrameComplete = true;

	//  Timer (  based movement )
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling game_timeelapsed");
	game_timeelapsed ( );

	//  Music processing
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling music_loop");
	music_loop ( );

	//  Character sound update
	//  110315 - 019 - If spawning in, no sound for the player
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling character_sound_update");
	if (  t.game.runasmultiplayer  ==  0 || g.mp.noplayermovement  ==  0 ) 
	{
		character_sound_update ( );
	}

	//  Force a shader update to ensure correct shadows are used at start
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling visuals_shaderlevels_update");
	if (  t.visuals.refreshcountdown>0 ) 
	{
		visuals_shaderlevels_update();
		--t.visuals.refreshcountdown;
	}

	// debug reason - why is my SocialVR completely black!!
	bool bSocialVRDebugTABTAB = true;

	// Testgame or Standalone
	// 250316 - when level ends, suspend all logic (including more calls to JumpTolevel or in-game last minute AI stuff)
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"checking levelendingcycle");
	if ( t.game.levelendingcycle == 0 )
	{
		if ( (t.game.gameisexe == 0 || g.gprofileinstandalone == 1) && (t.game.runasmultiplayer == 0 || bSocialVRDebugTABTAB == true)  ) 
		{
			// Test Game Mode
			// Tab Mode (only when not mid-fpswarning)
			if ( g.gproducelogfiles == 2 ) timestampactivity(0,"checking tab key handler");
			if ( t.plrkeySHIFT == 0 && t.plrkeySHIFT2 == 0  )  t.tkeystate15 = KeyState(g.keymap[15]); else t.tkeystate15 = 0;
			if ( t.game.runasmultiplayer == 1 && bSocialVRDebugTABTAB == false ) g.tabmode = 0;
			if ( t.conkit.editmodeactive == 1 )  g.tabmode = 0;
			if ( g.lowfpswarning == 0 || g.lowfpswarning == 3 ) 
			{
				if ( t.tkeystate15 == 0 ) t.tabpress = 0;
				if ( g.globals.riftmode>0 ) 
				{
					if (  t.tkeystate15 == 1 && t.tabpress == 0 ) 
					{
						// 101115 - reset hardware menu if press TAB
						g.tabmodeshowfps = 0;

						if (  g.tabmode == 0 ) 
						{
							game_showmouse ( );
							g.tabmode=2;
						}
						else
						{
							game_hidemouse ( );
							g.tabmode=0;
						}
						t.tabpress=1;
					}
				}
				else
				{
					if (  t.tkeystate15 == 1 && t.tabpress == 0 ) 
					{
						// 101115 - reset hardware menu if press TAB
						g.tabmodeshowfps = 0;

						g.tabmode=g.tabmode+1;
						if (  g.tabmode>2 ) 
						{
							g.tabmode=0;
						}
						if (  g.tabmode<2 ) 
						{
							game_hidemouse ( );
						}
						if (  g.tabmode == 2 ) 
						{
							game_showmouse ( );
						}
						t.tabpress=1;
					}
				}
			}
		}
		else
		{
			//  Standalone Mode
			#ifdef FREETRIALVERSION
			 if ( t.game.gameisexe != 0 )
			 {
				// No lightmapping in free trial version
				t.visuals.generalpromptstatetimer=Timer()+123;
				t.visuals.generalprompt_s="Game Created With Free Trial Version Of GameGuru";
			 }
			#endif
		}

		//  Measure Sync (  to loop start )
		t.game.perf.resttosync += PerformanceTimer()-g.gameperftimestamp ; g.gameperftimestamp=PerformanceTimer();

		//  Control slider menus (based on tab page)
		if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling sliders_loop");
		sliders_loop ( );

		// CTRL+H to hide the hud when testing levels. For reason some the key results don't match what they should be from keymap?
		if (t.game.gameisexe == 0)
		{
			static int iKeyRepeatCounter = 0;
			iKeyRepeatCounter++;
			if (iKeyRepeatCounter > 60)
				iKeyRepeatCounter = 60;
			if (KeyState(g.keymap[35]) && KeyState(g.keymap[29]) && iKeyRepeatCounter >= 60)
			{
				g.tabmodehidehuds = !g.tabmodehidehuds;
				iKeyRepeatCounter = 0;
			}
		}

		//  update all projectiles
		if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling weapon_projectile_loop");
		weapon_projectile_loop ( );

		//  Prompt
		if ( g.gproducelogfiles == 2 ) timestampactivity(0,"checking prompts");
		if (  t.sky.currenthour_f<1.0 || t.sky.currenthour_f >= 13.0 ) 
		{
			t.pm=int(t.sky.currenthour_f);
			if (  t.pm == 0  )  t.pm = 12; else t.pm = t.pm-12;
			t.pm_s = ""; t.pm_s = t.pm_s + Str(t.pm)+"PM";
		}
		else
		{
			t.pm_s = "";t.pm_s = t.pm_s + Str(int(t.sky.currenthour_f))+"AM";
		}
		t.promptextra_s = ""; t.promptextra_s=t.promptextra_s + "FPS:"+Str(GetDisplayFPS())+" TIME:"+t.pm_s;
		t.game.perf.misc += PerformanceTimer()-g.gameperftimestamp ; g.gameperftimestamp=PerformanceTimer();

		if (t.playercontrol.thirdperson.enabled != 1)
		{
			if (iEnterGodMode == 1)
			{
				godcamx = camx = CameraPositionX(t.terrain.gameplaycamera);
				godcamy = camy = CameraPositionY(t.terrain.gameplaycamera);
				godcamz = camz = CameraPositionZ(t.terrain.gameplaycamera);
				godcamax = gcamax = CameraAngleX(t.terrain.gameplaycamera);
				godcamay = gcamay = CameraAngleY(t.terrain.gameplaycamera);
				godcamaz = gcamaz = CameraAngleZ(t.terrain.gameplaycamera);
				iEnterGodMode++;
			}
			if (iEnterGodMode == 2)
			{
				if (g.luacameraoverride != 1 && g.luacameraoverride != 3)
				{
					PositionCamera(t.terrain.gameplaycamera, camx, camy, camz);
					RotateCamera(t.terrain.gameplaycamera, gcamax, gcamay, gcamaz);
					t.tobj = t.aisystem.objectstartindex;
					if (ObjectExist(t.tobj))
						PositionObject(t.tobj, camx, camy, camz);
				}
			}
		}


		//PE: Moved of of thread, none of the object functions are 100% thread safe.
		if (BPhys_GetDebugDrawerMode() != 0)
		{
			physics_render_debug_meshes();
		}

		//  loop physics
		if (  t.hardwareinfoglobals.nophysics == 0 )
		{
			// Handle physics
			if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling physics_loop");
			auto range2 = wiProfiler::BeginRangeCPU("Update - Logic - Physics");

			// reinstated physics into main CPU thread for stability over performance
			physics_loop ( );

			// special mode for testing
			if (iEnterGodMode != 2)
			{
				physics_player_control (); // has LUA calls inside it
			}

			physics_player_handledeath (); // handles sound, so keep in main thread
			wiProfiler::EndRange(range2);

			// read all slider values for player
			if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling sliders readall");
			t.slidersmenuindex=t.slidersmenunames.player; sliders_readall ( );

			//  Do weapon attachments AFTER physics moved objects (and if char killed off)
			for ( g.charanimindex = 1 ; g.charanimindex <= g.charanimindexmax; g.charanimindex++ )
			{
				// detect collection of dropped guns
				t.e = t.charanimstates[g.charanimindex].originale;
				if ( t.e > 0 ) entity_monitorattachments ( );
			}

			//  Construction Kit control
			if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling conkit_loop");
			//conkit_loop ( );
		}
		t.game.perf.physics += PerformanceTimer()-g.gameperftimestamp ; g.gameperftimestamp=PerformanceTimer();

		// In-Game Mode (moved from above so LUA is AFTER physics)
		if ( g.gproducelogfiles == 2 ) timestampactivity(0,"checking in-game edit mode");

		if ( t.conkit.editmodeactive == 0 )
		{
			// if third person, trick AI by moving camera to protagonist location
			if ( t.playercontrol.thirdperson.enabled == 1 ) 
			{
				t.playercontrol.thirdperson.storecamposx = CameraPositionX(t.terrain.gameplaycamera);
				t.playercontrol.thirdperson.storecamposy = CameraPositionY(t.terrain.gameplaycamera);
				t.playercontrol.thirdperson.storecamposz = CameraPositionZ(t.terrain.gameplaycamera);
				t.tobj = t.aisystem.objectstartindex;
				if ( g.luacameraoverride != 1 && g.luacameraoverride != 3 )
				{
					PositionCamera ( t.terrain.gameplaycamera, ObjectPositionX(t.tobj), ObjectPositionY(t.tobj), ObjectPositionZ(t.tobj) );
				}
			}

			// All Entity logic
			t.ttempoverallaiperftimerstamp=PerformanceTimer();
			if ( t.hardwareinfoglobals.noai == 0 ) 
			{
				// LUA Logic
				auto range1 = wiProfiler::BeginRangeCPU("Update - Logic - LUA");
				lua_loop ( );
				wiProfiler::EndRange(range1);

				// Entity Logic
				auto range2 = wiProfiler::BeginRangeCPU("Update - Logic - Objects");
				t.game.perf.ai1 += PerformanceTimer()-g.gameperftimestamp ; g.gameperftimestamp=PerformanceTimer();
				entity_loop ( );
				entity_loopanim ( );
				t.game.perf.ai2 += PerformanceTimer()-g.gameperftimestamp ; g.gameperftimestamp=PerformanceTimer();
				wiProfiler::EndRange(range2);

				// Update all AI and Characters and VWeaps
				auto range3 = wiProfiler::BeginRangeCPU("Update - Logic - AI");
				if ( t.aisystem.processlogic == 1 )
				{
					if ( t.visuals.debugvisualsmode<100 ) 
					{
						darkai_loop ( );
					}

					// handle anmy token drops once per cycle
					g_RecastDetour.ManageTokenDropSystem(g.timeelapsed_f);
				}
				wiProfiler::EndRange(range3);

				// handle any AI stuff related to recastretour
				game_updatenavmeshsystem();
			}
			t.game.perf.ai += PerformanceTimer()-t.ttempoverallaiperftimerstamp;
		}

		// if third person, restore camera from protag-cam trick
		if ( t.playercontrol.thirdperson.enabled == 1 ) 
		{
			if ( g.luacameraoverride != 1 && g.luacameraoverride != 3 )
			{
				PositionCamera (  t.terrain.gameplaycamera,t.playercontrol.thirdperson.storecamposx,t.playercontrol.thirdperson.storecamposy,t.playercontrol.thirdperson.storecamposz );
			}
		}
		else
		{
			if (iEnterGodMode == 2)
			{
				if (g.luacameraoverride != 1 && g.luacameraoverride != 3)
				{
					//PE: Move godcam here.

					PositionCamera(t.terrain.gameplaycamera, godcamx, godcamy, godcamz);
					RotateCamera(t.terrain.gameplaycamera, godcamax, godcamay, godcamaz);
					void GodCameraControl(float& x, float& y, float& z, float& ax, float& ay, float& az);
					GodCameraControl(godcamx, godcamy, godcamz, godcamax, godcamay, godcamaz);
					t.tobj = t.aisystem.objectstartindex;
					if (ObjectExist(t.tobj))
						PositionObject(t.tobj, camx, camy, camz);

				}
			}

		}

		//  Gun control
		if ( t.hardwareinfoglobals.noguns == 0 ) 
		{
			if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling gun_manager");
			gun_manager ( );
			t.slidersmenuindex=t.slidersmenunames.weapon ; sliders_readall ( );
		}
		t.game.perf.gun += PerformanceTimer()-g.gameperftimestamp ; g.gameperftimestamp=PerformanceTimer();
	}

	//  update all particles and emitters
	update_env_particles();
	ravey_particles_update();

	if (t.visuals.bPPSnow && t.visuals.bpp_disable_indoor)
	{
		//Disable weather indoor.
		float xPos = CameraPositionX();
		float yPos = CameraPositionY();
		float zPos = CameraPositionZ();
		static int iDelayedRayCast;
		if (iDelayedRayCast++ % 15 == 0)
		{
			extern wiECS::Entity g_weatherEntityID;
			wiScene::WeatherComponent* weather = wiScene::GetScene().weathers.GetComponent(g_weatherEntityID);
			int iHitObj = IntersectAllEx(g.entityviewstartobj, g.entityviewendobj, xPos, yPos, zPos, xPos, yPos + 2000.0f, zPos, 0, 0, 0, 0, 1, true);
			if (iHitObj > 0)
			{
				weather->SetPPSnowEnabled(false);
			}
			else
			{
				weather->SetPPSnowEnabled(t.visuals.bPPSnow);
			}
		}
	}

	//  Decal control
	decalelement_control();

	// bullethole manegement
	bulletholes_update();

	//  Steam call moved here as camera changes need to be BEFORE the shadow update
	if (  t.game.runasmultiplayer == 1 ) 
	{
		// debug tracing
		if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling mp_gameLoop");

		// run multiplayer logic
		mp_gameLoop ( );

		// mp logic can trigger a new avatar to be loaded and created dynamically
		game_scanfornewavatars ( true );
	}

	// If the camera is spun round quick, redraw shadows immediately
	// 281116 - int tMouseMove = MouseMoveX(); - yep, this killed fluid mousemoveX, thanks for that Lee!
	int tMouseMove = t.cammousemovex_f;
	if (  t.hardwareinfoglobals.noterrain == 0 ) 
	{
		//Dave Performance, calling update on veg and terrain shadow every 4 frames rather than every frame
		//Grass every other frame
		//Gets me 10fps increase on my machine		
		static bool terrainvegdelay = true;
		t.game.perf.terrain1 += PerformanceTimer()-g.gameperftimestamp ; g.gameperftimestamp=PerformanceTimer();
		t.game.perf.terrain2 += PerformanceTimer()-g.gameperftimestamp ; g.gameperftimestamp=PerformanceTimer();
	}
	if (  t.hardwareinfoglobals.nosky == 0 ) 
	{
		if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling sky_loop");
		sky_loop ( );
	}
	t.game.perf.terrain3 += PerformanceTimer()-g.gameperftimestamp ; g.gameperftimestamp=PerformanceTimer();

	//  Game Debug Prompts
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"assembling debug prompts");
	if (  t.aisystem.showprompts == 1 ) 
	{
		pastebitmapfont("DEBUG PROMPTS",8,20,1,255) ; t.i=1;
		t.strwork = ""; t.strwork = t.strwork + "NUMBER OF CHARACTERS:"+Str(g.charanimindexmax);
		pastebitmapfont( t.strwork.Get() ,8,20+(t.i*25),1,255) ; ++t.i;
		for ( g.charanimindex = 1 ; g.charanimindex<=  g.charanimindexmax; g.charanimindex++ )
		{
			t.tdesc_s = ""; t.tdesc_s = t.tdesc_s + "CHAR:"+Str(g.charanimindex);
			t.tdesc_s=t.tdesc_s+"  AIstate_s="+AIGetEntityState(t.charanimstates[g.charanimindex].obj);
			t.tdesc_s=t.tdesc_s+"  t.moving="+Str(t.charanimcontrols[g.charanimindex].moving);
			t.tdesc_s=t.tdesc_s+"  t.ducking="+Str(t.charanimcontrols[g.charanimindex].ducking);
			pastebitmapfont(t.tdesc_s.Get(),8,20+(t.i*25),1,255) ; ++t.i;
			t.tdesc_s="";
			t.tdesc_s=t.tdesc_s+"  plrvisible="+Str(t.entityelement[t.charanimstates[g.charanimindex].e].plrvisible);
			t.tdesc_s=t.tdesc_s+"  health="+Str(t.entityelement[t.charanimstates[g.charanimindex].e].health);
			t.tdesc_s=t.tdesc_s+"  playcsi="+Str(t.charanimstates[g.charanimindex].playcsi);
			t.tdesc_s=t.tdesc_s+"  t.charseq.mode="+Str(t.charseq[t.charanimstates[g.charanimindex].playcsi].mode);
			pastebitmapfont(t.tdesc_s.Get(),8,20+(t.i*25),1,255) ; ++t.i;
			++t.i;
		}
	}

	// Handle occlusion if active
	if ( g.globals.occlusionmode == 1 ) 
	{
		// VR software cannot use occlusion at the moment
		if ( g.vrqcontrolmode == 0 ) //g.vrglobals.GGVREnabled == 0 )
		{
			// detect velocity of XZ motion of player and advance 'virtual camera' ahead of real camera
			// in order to give occluder time to reveal visible objects in advance of getting there
			if ( g.gproducelogfiles == 2 ) timestampactivity(0,"checking occlusionp");
			float plrx = CameraPositionX(0);
			float plrz = CameraPositionZ(0);
			g_fOccluderCamVelX = plrx - g_fOccluderLastCamX;
			g_fOccluderCamVelZ = plrz - g_fOccluderLastCamZ;
			g_fOccluderLastCamX = plrx;
			g_fOccluderLastCamZ = plrz;
			if ( fabs(g_fOccluderCamVelX)>0.01f || fabs(g_fOccluderCamVelZ)>0.01f )
			{
				float fDDMultiplier = 20.0f / sqrt(fabs(g_fOccluderCamVelX*g_fOccluderCamVelX)+fabs(g_fOccluderCamVelZ*g_fOccluderCamVelZ));
				g_fOccluderCamVelX *= fDDMultiplier;
				g_fOccluderCamVelZ *= fDDMultiplier;
				if ( g_fOccluderCamVelX < -20.0f ) g_fOccluderCamVelX = -20.0f;
				if ( g_fOccluderCamVelZ < -20.0f ) g_fOccluderCamVelZ = -20.0f;
				if ( g_fOccluderCamVelX > 20.0f ) g_fOccluderCamVelX = 20.0f;
				if ( g_fOccluderCamVelZ > 20.0f ) g_fOccluderCamVelZ = 20.0f;
			}
			else
			{
				g_fOccluderCamVelX = 0.0f;
				g_fOccluderCamVelZ = 0.0f;
			}
			// show me this
			CPUShiftXZ ( g_fOccluderCamVelX, g_fOccluderCamVelZ );

			CPU3DSetCameraFar ( t.visuals.CameraFAR_f );
			if ( g_pOccluderThread == NULL )
			{
				CPU3DOcclude (  );
				g_hOccluderBegin = CreateEvent ( NULL, FALSE, FALSE, NULL );
				g_hOccluderEnd   = CreateEvent ( NULL, FALSE, FALSE, NULL );
				g_pOccluderThread = new cOccluderThread;
				g_pOccluderThread->Start ( );
			}
			g_occluderOn = true;
		}
	}
	t.game.perf.occlusion += PerformanceTimer()-g.gameperftimestamp ; g.gameperftimestamp=PerformanceTimer();

	// Final post processing step

	// Render pre-terrain post process cameras (includes lightray rendering)
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling postprocess_preterrain");
	postprocess_preterrain ( );

	//  explosions and fire
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling draw_particles");
	draw_particles();

	//  handle fade out for level progression
	if (  t.game.levelendingcycle > 0 ) 
	{
		if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling game_end_of_level_check");
		game_end_of_level_check ( );
	}

	//  Post process and visual settings system
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling visuals_loop");
	visuals_loop ( );
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling lighting_loop");
	lighting_loop ( );
	t.game.perf.postprocessing += PerformanceTimer()-g.gameperftimestamp ; g.gameperftimestamp=PerformanceTimer();

	// Check for player guns switched off
	if ( g.noPlayerGuns )
	{
		if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling physics_no_gun_zoom");
		physics_no_gun_zoom ( );
		if ( g.autoloadgun != 0 ) { g.autoloadgun=0 ; gun_change ( ); }
	}

	//  Update HUD Layer objects (jetpack)
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling hud_updatehudlayerobjects");
	hud_updatehudlayerobjects ( );

	// trigger screen to be grabbed for a HUD image
	bool bGrabSceneToStoreInImage = true;
	if (bGrabSceneToStoreInImage == true)
	{
		extern void GrabBackBufferForAnImage (void);
		GrabBackBufferForAnImage();
	}
}

extern int howManyOccluders;
extern int howManyOccludersDrawn;
extern int howManyOccludees;
extern int howManyOccludeesHidden;
extern int howManyMarkers;
extern float trackingSize;

void game_dynamicRes()
{
	// If dynamic res is disabled via setup ini, return out
	if ( t.DisableDynamicRes ) return;

	if ( g_pGlob->dwNumberOfPrimCalls > 500 && GetDisplayFPS() < 60 )
		t.bHiResMode = false;
	else if ( g_pGlob->dwNumberOfPrimCalls < 200 && GetDisplayFPS() > 60 )
		t.bHiResMode = true;

	if ( t.visuals.debugvisualsmode == 20 ) t.bHiResMode = false;
	if ( t.visuals.debugvisualsmode == 21 ) t.bHiResMode = true;

	if ( t.bHiResMode != t.bOldHiResMode )
	{
		SetCameraHiRes ( t.bHiResMode );
		t.bOldHiResMode = t.bHiResMode;
	}
}

extern float smallDistanceMulti;

void game_sync ( void )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	//  Work out overall time spent per cycle
	t.game.perf.overall += PerformanceTimer()-g.gameperfoveralltimestamp ; g.gameperfoveralltimestamp=PerformanceTimer();

	//  HUD Damage Display
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling controlblood");
	controlblood();
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling controldamagemarker");
	controldamagemarker();

	//  Slider menus rendered last
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling sliders_draw");
	sliders_draw ( );

	//  Detect if FPS drops (only for single player - never for MP games)
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"checking show hide mouse");
	if (  t.game.runasmultiplayer == 0 && g.globals.hidelowfpswarning == 0 && g.tabmode == 0 && g.ghardwareinfomode == 0 && t.visuals.generalpromptstatetimer == 0 ) 
	{
		if ( t.conkit.cooldown>0 )  
		{
			// cooldown ensures this FPS warning only happens when cooled off
			--t.conkit.cooldown;
			g.lowfpstarttimer = Timer();
		}
		if (  t.conkit.editmodeactive == 0 && t.conkit.cooldown == 0 ) 
		{
			if (  g.lowfpswarning == 0 ) 
			{
				if ( (unsigned long)Timer()>g.lowfpstarttimer+2000 ) 
				{
					if ( GetDisplayFPS()<20 ) 
					{
						g.lowfpswarning=1;
						game_showmouse ( );
					}
				}
			}
			else
			{
				if (  g.lowfpswarning == 2 ) 
				{
					game_hidemouse ( );
					while ( MouseClick() != 0 ) { }
					g.lowfpswarning=3;
				}
			}
		}
	}

	//  Only render main and postprocess camera (not paint camera, reflection or lightray cameras)
	//  for globals.riftmode, left and right eyes are rendered in the _postprocess_preterrain step
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"applying sync mask");
	t.tmastersyncmask=0;
	SyncMask (  t.tmastersyncmask+(1<<3)+(1) );

	//  Update RealSense if any
	///realsense_loop ( );

	//  Update screen
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling sync");
	Sync (  );
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling game_dynamicRes");
	game_dynamicRes();

	//Dave Performance - let the occluder thread know it is okay to begin
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling CPU3DOcclude");
	CPU3DOcclude (  );
	if ( g_hOccluderBegin ) SetEvent ( g_hOccluderBegin );

	t.game.perf.synctime += (PerformanceTimer()-g.gameperftimestamp) ; g.gameperftimestamp=PerformanceTimer();

	//  collect main Sync (  statistics )
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"checking statistics");
	t.mainstatistic1=GetStatistic(1);
	t.mainstatistic5=GetStatistic(5);

	//  detect and slow down action
	if (  t.visuals.debugvisualsmode == 4 ) 
	{
		if (  ReturnKey() == 1 ) 
		{
			t.player[1].health=50000;
			physics_pausephysics ( );
			SleepNow (  200 );
			physics_resumephysics ( );
		}
	}

	//  Work out performance metrics
	if ( g.gproducelogfiles == 2 ) timestampactivity(0,"calling sliders readall");
	t.slidersmenuindex=t.slidersmenunames.performance  ; sliders_readall ( );
}

void game_main_stop ( void )
{
	// Rest any ingame variables
	if ( t.conkit.entityeditmode != 0 || t.conkit.editmodeactive == 1 ) 
	{
		//conkitedit_switchoff ( );
	}
}

void game_jump_to_level_from_lua (int iResetStates)
{
	if (  t.game.gameisexe == 1 ) 
	{
		if (  t.game.gameloop == 1 && t.game.levelendingcycle  ==  0 ) 
		{
			t.game.jumplevelresetstates = iResetStates;
			t.game.jumplevel_s = t.tleveltojump_s;
			t.game.levelendingcycle = 4000;
		}
	}
	else
	{
		t.s_s = "" ; t.s_s = t.s_s+"Jump To Level : "+t.tleveltojump_s ; lua_prompt ( );
	}
}

void game_finish_level_from_lua ( void )
{
	if (  t.game.gameisexe == 1 ) 
	{
		if (  t.game.gameloop == 1 && t.game.levelendingcycle  ==  0 ) 
		{
			t.game.levelendingcycle = 4000;
		}
	}
	else
	{
		t.s_s="Game Completed"  ; lua_prompt ( );
	}
}

void game_end_of_level_check ( void )
{
	// end of level fade out
	t.game.levelendingcycle = t.game.levelendingcycle - (g.timeelapsed_f * 400.0);// 200.0); slicker!
	t.huddamage.immunity=1000;
	if (  t.game.levelendingcycle  <=  0 ) 
	{
		t.game.gameloop=0;
		t.game.levelendingcycle = 0;
		t.postprocessings.fadeinenabled = 1;
		//PE: Delay screen update , some frames so we only see the "Game Over Screen".
		extern int iBlockRenderingForFrames;
		iBlockRenderingForFrames = 10;
		extern bool g_bNoSwapchainPresent;
		g_bNoSwapchainPresent = true; 
	}

	// control fade out of screen
	if (t.postprocessings.fadeinenabled)
	{
		t.postprocessings.fadeinvalue_f = t.game.levelendingcycle / 4500.0; //PE: fadeinvalue_f is a bit delayed , so just count down faster.
		t.postprocessings.fadeinvalueupdate = 1;
	}

	// fade audio (can be slow, so only do at end)
	if (t.game.levelendingcycle <= 100)
	{
		if (t.audioVolume.music > t.postprocessings.fadeinvalue_f * 100.0)  t.audioVolume.music = t.postprocessings.fadeinvalue_f * 100.0;
		if (t.audioVolume.sound > t.postprocessings.fadeinvalue_f * 100.0)  t.audioVolume.sound = t.postprocessings.fadeinvalue_f * 100.0;
		audio_volume_update ();
	}
}

void GodCameraControl(float &x, float &y, float &z, float& ax, float& ay, float& az)
{
	ImGuiIO& io = ImGui::GetIO();
	
	if (1)
	{
		if (ImGui::IsMouseDown(1))
		{
			float speed = 6.0f;
			float xdiff = ImGui::GetIO().MouseDelta.x / speed;
			float ydiff = ImGui::GetIO().MouseDelta.y / speed;
			ax += ydiff;
			ay += xdiff;
			if (ax > 180.0f)  ax = ax - 360.0f;
			if (ax < -89.999f)  ax = -89.999f;
			if (ax > 89.999f)  ax = 89.999f;
			RotateCamera(ax, ay, 0);
		}

	}

	if(1)
	{
		static float fAccelerationTimer = 0.0f;
		if (t.inputsys.keyup == 1)  t.plrkeyW = 1; else t.plrkeyW = 0;
		if (t.inputsys.keyleft == 1)  t.plrkeyA = 1; else t.plrkeyA = 0;
		if (t.inputsys.keydown == 1)  t.plrkeyS = 1; else t.plrkeyS = 0;
		if (t.inputsys.keyright == 1)  t.plrkeyD = 1; else t.plrkeyD = 0;

		//  mouse wheel mimmics W and S when no CONTROL key pressed (170616 - but not when in EBE mode as its used for grid layer control)
		int usingWheel = 0;
		t.traise_f = 0.0;
		if (t.inputsys.keyshift == 1 || io.KeyShift)
		{

			fAccelerationTimer += g.timeelapsed_f * 0.005f;
			if (fAccelerationTimer > 1.0f) fAccelerationTimer = 1.0f;
			t.tffcspeed_f = 10.0 * g.timeelapsed_f;
		}
		else
		{
			fAccelerationTimer = 0.0f;
			if (t.inputsys.keycontrol == 1 || io.KeyCtrl)
			{
				// reduce this until we sort out scale!
				t.tffcspeed_f = 1.0 * g.timeelapsed_f;
			}
			else
			{
				// reduce this until we sort out scale!
				t.tffcspeed_f = 5.0 * g.timeelapsed_f;
			}
		}

		
		if(1)
		{
			float fHeightAtThisPartOfTerrain = BT_GetGroundHeight(t.terrain.TerrainID, x, z);
			float height = y - fHeightAtThisPartOfTerrain;
			if (height < 0) height = 0;
			float modifier = height * height * 0.00001f + 2 + 50 * fAccelerationTimer;
			if (modifier > 50) modifier = 50;
			if (modifier < 2) modifier = 2;
			t.tffcspeed_f *= modifier;
		}

		if (t.inputsys.k_s == "e" || ImGui::IsKeyDown(69))  t.traise_f = -90;
		if (t.inputsys.k_s == "q" || ImGui::IsKeyDown(81))  t.traise_f = 90;

		PositionCamera(x, y, z);

		if (t.plrkeyW == 1 || ImGui::IsKeyDown(87) || ImGui::IsKeyDown(38))
			MoveCamera(t.tffcspeed_f);
		if (t.plrkeyS == 1 || ImGui::IsKeyDown(83) || ImGui::IsKeyDown(40))
			MoveCamera(t.tffcspeed_f * -1);

		if (t.plrkeyA == 1 || ImGui::IsKeyDown(65) || ImGui::IsKeyDown(37)) { RotateCamera(0, ay - 90, 0); MoveCamera(t.tffcspeed_f); }
		if (t.plrkeyD == 1 || ImGui::IsKeyDown(68) || ImGui::IsKeyDown(39)) { RotateCamera(0, ay + 90, 0); MoveCamera(t.tffcspeed_f); }

		if (t.traise_f != 0) { RotateCamera(t.traise_f, 0, 0); MoveCamera(t.tffcspeed_f); }
		if (MouseClick() == 4)
		{
			//  new middle mouse panning
			RotateCamera(0, ay, 0);
			MoveCamera(t.cammousemovey_f * -2);
			if (t.cammousemovex_f < 0) { RotateCamera(0, ay - 90, 0); MoveCamera(abs(t.cammousemovex_f * 2)); }
			if (t.cammousemovex_f > 0) { RotateCamera(0, ay + 90, 0); MoveCamera(t.cammousemovex_f * 2); }
		}
		x = CameraPositionX();
		y = CameraPositionY();
		z = CameraPositionZ();
	}

	//  ensure camera NEVER goes into Floor (  )
	if (0)
	{
		t.tcurrenth_f = BT_GetGroundHeight(t.terrain.TerrainID, x, z) + 10.0;
		if (y < t.tcurrenth_f)
		{
			y = t.tcurrenth_f;
		}

		if (t.editorfreeflight.s.y_f < t.tcurrenth_f)
		{
			t.editorfreeflight.s.y_f = t.tcurrenth_f;
		}
	}

	PositionCamera(x, y, z);
	RotateCamera(ax, ay, 0);

}