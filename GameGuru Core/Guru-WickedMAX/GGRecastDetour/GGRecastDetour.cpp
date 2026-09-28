// Includes
#include "GGRecastDetour.h"
#include "CFileC.h"
#include "DetourCommon.h"

// Globals
bool g_bNavMeshChanged = false;
std::vector<sBlocker> g_BlockerList;
std::vector<sTokenDrop> g_TokenDropList;
float g_fWaterTableY = 0.0f;

// main functions
GGRecastDetour::GGRecastDetour()
{
	sample = 0;
	geom = 0;
	tool = 0;
}

void GGRecastDetour::freeall (void)
{
	if (tool)
	{
		delete tool;
		tool = 0;
	}
	if (sample)
	{
		delete sample;
		sample = 0;
	}
	if (geom)
	{
		delete geom;
		geom = 0;
	}
}

int GGRecastDetour::buildall (float* pVertices, uint32_t numVertices)
{
	// free any previous resources
	freeall();

	// create simple solo mesh sample
	sample = new Sample_TileMesh();
	if (sample)
	{
		// initialise debug objects in sample class
		sample->getDebugDraw().init();

		// set context for sample
		sample->setContext(&ctx);

		// create geom to hold level data
		geom = new InputGeom;
		sample->handleMeshChanged(geom);

		if ( pVertices )
		{
			if (!geom->loadData(&ctx, pVertices, numVertices))
			{
				// error
				delete geom;
				geom = 0;
				delete sample;
				sample = 0;
				return 0;
			}
		}
		else
		{
			// load data into geom
			char szRealFilename[2048];
			strcpy_s(szRealFilename, 2048, "levelbank\\testmap\\rawlevelgeometry.obj");
			GG_GetRealPath(szRealFilename, 1);
			std::string meshName = szRealFilename;
			if (!geom->load(&ctx, meshName))
			{
				// error
				delete geom;
				geom = 0;
				delete sample;
				sample = 0;
				return 0;
			}
		}

		// if have sample and geom, build nav mesh
		if (geom && sample)
		{
			sample->handleSettings();

			// build navmesh
			ctx.resetLog();
			if (!sample->handleBuild())
			{
				// failed
			}
			ctx.dumpLog("Build log");
		}
	}

	// create navmesh tool and init with build sample
	tool = new NavMeshTesterTool;
	if (tool) tool->init(sample);

	// success
	return 1;
}

int GGRecastDetour::findPath (float fStart[3], float fEnd[3])
{
	if (tool)
	{
		// find path through nav mesh
		tool->findAPath (fStart, fEnd);

		// success
		return 1;
	}

	// failure
	return 0;
}

int GGRecastDetour::getPath (int* piPointCount, float** ppfPointData)
{
	if (tool)
	{
		// resulting path
		tool->getPathResult(piPointCount, ppfPointData);
		if (*ppfPointData != NULL)
		{
			// success
			return 1;
		}
	}

	// failure
	return 0;
}

bool GGRecastDetour::isWithinNavMeshEx (float fX, float fY, float fZ, float* pvecNearestPt, bool bMustBeOverPoly)
{
	if (tool)
	{ 
		bool pIsOverPoly = false;
		if (tool->isWithinNavMeshEx(fX, fY, fZ, pvecNearestPt, &pIsOverPoly) == true)
		{
			if (bMustBeOverPoly == true)
			{
				if (pIsOverPoly == true)
				{
					return true;
				}
			}
			else
			{
				return true;
			}
		}
		return false;
	}
	else
		return false;
}

bool GGRecastDetour::isWithinNavMesh (float fX, float fY, float fZ)
{
	return isWithinNavMeshEx(fX, fY, fZ, 0, 0);
}

float GGRecastDetour::getYFromPos (float fX, float fY, float fZ)
{
	if (tool)
		return tool->getYFromPos(fX, fY, fZ);
	else
		return fY;
}

void GGRecastDetour::forceDebugUpdate(void)
{
	g_bNavMeshChanged = true;
}

void GGRecastDetour::handleDebugRender(void)
{
	if (sample)
	{
		if (g_bNavMeshChanged == true)
		{
			sample->GetDD()->hideDebugObjects();
			g_bNavMeshChanged = false;
		}
		sample->handleRender();
	}
	if (tool)
	{
		tool->handleRender();
	}
}

void GGRecastDetour::cleanupDebugRender(void)
{
	if (sample)
	{
		sample->GetDD()->hideDebugObjects();
	}
}

// radius of 15 seems about right
void GGRecastDetour::TogglePolys( float x, float y, float z, float radius, bool enable )
{
	float p[ 3 ] = { x, y, z };
	
	if ( !sample ) return;
	dtNavMesh* nav = sample->getNavMesh();
	dtNavMeshQuery* navquery = sample->getNavMeshQuery();
	if (nav && navquery)
	{
		dtQueryFilter filter;
		const float halfExtents[ 3 ] = { radius, radius, radius };
			
		int numFound = 0;
		const int findMax = 100;
		dtPolyRef ref[ findMax ];
			
		navquery->queryPolygons(p, halfExtents, &filter, ref, &numFound, findMax);
		for( int i = 0; i < numFound; i++ )
		{
			const dtPoly* pPoly = 0;
			const dtMeshTile* pTile = 0;
			nav->getTileAndPolyByRefUnsafe( ref[i], &pTile, &pPoly );

			float closest[3];
			unsigned short vert1 = pPoly->verts[0];
			unsigned short vert2 = pPoly->verts[1];
			unsigned short vert3 = pPoly->verts[2];
			float* a = &pTile->verts[ vert1*3 ];
			float* b = &pTile->verts[ vert2*3 ];
			float* c = &pTile->verts[ vert3*3 ];
			dtClosestPtPointTriangle(closest, p, a, b, c );

			float dist = dtVdistSqr( closest, p );
			if ( dist > radius*radius ) continue;

			unsigned short flags = 0;
			if (dtStatusSucceed(nav->getPolyFlags(ref[i], &flags)))
			{
				if ( enable ) flags |= SAMPLE_POLYFLAGS_DISABLED;
				else flags &= ~SAMPLE_POLYFLAGS_DISABLED;
				nav->setPolyFlags(ref[i], flags);
			}
		}
	}

	// when we toggle polys, force a debug update
	forceDebugUpdate();
}

void GGRecastDetour::ResetTokenDropSystem(void)
{
	g_TokenDropList.clear();
}
void GGRecastDetour::DoTokenDrop(float x, float y, float z, int iType, float fDuration)
{
	bool bAnyNavMeshTokenDropStateChanged = false;
	if(iType>0)
	{
		sTokenDrop item;
		item.iType = iType;
		item.X = x;
		item.Y = y;
		item.Z = z;
		item.fTimeLeft = fDuration;
		g_TokenDropList.push_back(item);
		bAnyNavMeshTokenDropStateChanged = true;
	}
	if (bAnyNavMeshTokenDropStateChanged == true)
	{
		extern bool g_bShowRecastDetourDebugVisuals;
		if (g_bShowRecastDetourDebugVisuals == true)
		{
			extern GGRecastDetour g_RecastDetour;
			g_RecastDetour.cleanupDebugRender();
			g_RecastDetour.handleDebugRender();
		}
	}
}
void GGRecastDetour::ManageTokenDropSystem(float fTimeDelta)
{
	// fTimeDelta comes in at 0.75 for 60fps, so for MS we *20
	fTimeDelta *= 20.0f; // so now we get a nice 16.6ms chomp from the duration
	int iTokenDropCount = g_TokenDropList.size();
	if (iTokenDropCount > 0)
	{
		for (int iTokenDropIndex = 0; iTokenDropIndex < iTokenDropCount; iTokenDropIndex++)
		{
			g_TokenDropList[iTokenDropIndex].fTimeLeft -= fTimeDelta;
			if (g_TokenDropList[iTokenDropIndex].fTimeLeft <= 0.0f)
			{
				g_TokenDropList.erase(g_TokenDropList.begin() + iTokenDropIndex);
				iTokenDropCount--;
				iTokenDropIndex--;
			}
		}
	}
}
int GGRecastDetour::GetTokenDropCount() { return g_TokenDropList.size(); }
float GGRecastDetour::GetTokenDropX(int iIndex) { return g_TokenDropList[iIndex].X; }
float GGRecastDetour::GetTokenDropY(int iIndex) { return g_TokenDropList[iIndex].Y; }
float GGRecastDetour::GetTokenDropZ(int iIndex) { return g_TokenDropList[iIndex].Z; }
int GGRecastDetour::GetTokenDropType(int iIndex) { return g_TokenDropList[iIndex].iType; }
float GGRecastDetour::GetTokenDropTimeLeft(int iIndex) { return g_TokenDropList[iIndex].fTimeLeft; }

bool DoesLineGoThroughBlocker (float fFromOrigX, float fFromOrigY, float fFromOrigZ, float fToOrigX, float fToOrigY, float fToOrigZ)
{
	// LB: add an extra cost if the path runs through an area marked as a door, it will
	// force the system to find another path that does NOT go through dor areas (effectively blocking them as path ways when active)
	bool bBlocked = false;
	extern std::vector<sBlocker> g_BlockerList;
	if (g_BlockerList.size() > 0)
	{
		int iDoorCount = g_BlockerList.size();
		for (int iDoorIndex = 0; iDoorIndex < iDoorCount; iDoorIndex++)
		{
			if (bBlocked==false)
			{
				if (g_BlockerList[iDoorIndex].bBlocking == true)
				{
					float fDoorMinX = g_BlockerList[iDoorIndex].minX;
					float fDoorMaxX = g_BlockerList[iDoorIndex].maxX;
					float fDoorMinY = g_BlockerList[iDoorIndex].minY;
					float fDoorMaxY = g_BlockerList[iDoorIndex].maxY;
					float fDoorMinZ = g_BlockerList[iDoorIndex].minZ;
					float fDoorMaxZ = g_BlockerList[iDoorIndex].maxZ;
					float fCenterX = fDoorMinX + ((fDoorMaxX - fDoorMinX) / 2);
					float fCenterZ = fDoorMinZ + ((fDoorMaxZ - fDoorMinZ) / 2);
					float fFromX = fFromOrigX;
					float fFromY = fFromOrigY;
					float fFromZ = fFromOrigZ;
					float fToX = fToOrigX;
					float fToY = fToOrigY;
					float fToZ = fToOrigZ;
					// rotate into the blocker's own space (z is flipped, so add fAngle)
					float tofromradian = 0.0174533f;
					float fDX = fFromX - fCenterX;
					float fDZ = fCenterZ - fFromZ;
					float fDD = sqrtf(fabs(fDX * fDX) + fabs(fDZ * fDZ));
					float fFromA = atan2(fDX, fDZ) / tofromradian;
					fFromA = (fFromA + g_BlockerList[iDoorIndex].fAngle) * tofromradian;
					fFromX = fCenterX + (sinf(fFromA) * fDD);
					fFromZ = fCenterZ - (cosf(fFromA) * fDD);
					fDX = fToX - fCenterX;
					fDZ = fCenterZ - fToZ;
					fDD = sqrtf(fabs(fDX * fDX) + fabs(fDZ * fDZ));
					float fToA = atan2(fDX, fDZ) / tofromradian;
					fToA = (fToA + g_BlockerList[iDoorIndex].fAngle) * tofromradian;
					fToX = fCenterX + (sinf(fToA) * fDD);
					fToZ = fCenterZ - (cosf(fToA) * fDD);
					float fX = fFromX;
					float fY = fFromY;
					float fZ = fFromZ;
					float fIX = fToX - fFromX;
					float fIY = fToY - fFromY;
					float fIZ = fToZ - fFromZ;
					int iStepCount = sqrt(fabs(fIX * fIX) + fabs(fIY * fIY) + fabs(fIZ * fIZ));
					fIX /= iStepCount;
					fIY /= iStepCount;
					fIZ /= iStepCount;
					for (int iStep = 0; iStep < iStepCount; iStep += 2)
					{
						if (fX >= fDoorMinX && fX <= fDoorMaxX)
						{
							if (fY >= fDoorMinY && fY <= fDoorMaxY)
							{
								if (fZ >= fDoorMinZ && fZ <= fDoorMaxZ)
								{
									// the path goes through a door
									bBlocked = true;
									break;
								}
							}
						}
						fX += (fIX * 2);
						fY += (fIY * 2);
						fZ += (fIZ * 2);
					}
				}
			}
			else
			{
				// the path goes through a door
				break;
			}
		}
	}
	return bBlocked;
}

void GGRecastDetour::ResetBlockerSystem(void)
{
	g_BlockerList.clear();
}

void GGRecastDetour::ToggleBlocker(float x, float y, float z, float radius, bool enable, float fRadius2, float fAngle, float fAdjMinY, float fAdjMaxY)
{
	bool bAnyNavMeshBlockerStateChanged = false;
	float minX, maxX, minY, maxY, minZ, maxZ;
	bool bFoundBlocker = false;
	if (radius != fRadius2)
	{
		// new fully cunstomizable and accurate
		if (radius < 5) radius = 5;
		if (fRadius2 < 5) fRadius2 = 5;
		radius /= 2.0f;
		fRadius2 /= 2.0f;
		radius += 5.0f;
		fRadius2 += 5.0f;
		minX = x - radius;
		maxX = x + radius;
		minY = y + fAdjMinY;
		maxY = y + fAdjMaxY;
		minZ = z - fRadius2;
		maxZ = z + fRadius2;
	}
	else
	{
		// old default
		if (radius < 5) radius = 5;
		radius /= 2.0f;
		radius += 5.0f;
		minX = x - radius;
		maxX = x + radius;
		minY = y - 5;
		maxY = y + 95;
		minZ = z - radius;
		maxZ = z + radius;
	}
	for (int b = 0; b < g_BlockerList.size(); b++)
	{
		sBlocker* blocker = &g_BlockerList[b];
		if (blocker->minX == minX && blocker->maxX == maxX)
		{
			if (blocker->minY == minY && blocker->maxY == maxY)
			{
				if (blocker->minZ == minZ && blocker->maxZ == maxZ)
				{
					bFoundBlocker = true;
					if (blocker->bBlocking != enable)
					{
						bAnyNavMeshBlockerStateChanged = true;
					}
					blocker->bBlocking = enable;
					break;
				}
			}
		}
	}
	if (bFoundBlocker == false)
	{
		sBlocker item;
		item.minX = minX;
		item.maxX = maxX;
		item.minY = minY;
		item.maxY = maxY;
		item.minZ = minZ;
		item.maxZ = maxZ;
		item.bBlocking = enable;
		item.fAngle = fAngle;
		g_BlockerList.push_back(item);
		bAnyNavMeshBlockerStateChanged = true;
	}
	if (bAnyNavMeshBlockerStateChanged == true)
	{
		extern bool g_bShowRecastDetourDebugVisuals;
		if (g_bShowRecastDetourDebugVisuals == true)
		{
			extern GGRecastDetour g_RecastDetour;
			g_RecastDetour.cleanupDebugRender();
			g_RecastDetour.handleDebugRender();
		}
	}
}


void GGRecastDetour::SetWaterTableY(float y)
{
	g_fWaterTableY = y;
}