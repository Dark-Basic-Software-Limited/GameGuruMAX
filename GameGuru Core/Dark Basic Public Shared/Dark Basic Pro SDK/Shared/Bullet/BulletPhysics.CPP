// BulletPhysics PHYSICS COMMANDS

//#define FASTBULLETPHYSICS
#define PUTPHYSICSBACKINMAINTHREADSEQUENCE
#pragma warning( disable: 4312 ) // conversion from 'int' to 'void *' of greater size

// Includes
#include "BulletPhysics.h"
#include <stack>
#include <stdio.h>
#include <math.h>
#include <mutex>

// Bullet specific includes
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "../Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h"
#include "GGTerrainPhysicsShape.h"

// Ragdoll
#include "Ragdoll/DBPro/Include/DBPro.hpp"
#include "Ragdoll/DBProToBullet.h"
#include "Ragdoll/BaseItem.h"
#include "Ragdoll/BaseItemManager.h"
#include "Ragdoll/DBProRagDollBone.h"
#include "Ragdoll/DBProRagDoll.h"
#include "Ragdoll/DBProRagDollManager.h"
#include "Ragdoll/DBProJointManager.h"

//DM
#include "LinearMath/btAlignedObjectArray.h"
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btAabbUtil2.h"
#include "BulletCollision/BroadphaseCollision/btOverlappingPairCache.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

//PE
#include "BulletCollision\CollisionShapes\btConvexHullShape.h"
#include "BulletCollision\CollisionShapes\btShapeHull.h"
#include "LinearMath/btConvexHull.h"
//End PE

#ifdef WICKEDENGINE
//ZJ
#include "..\..\..\..\Guru-WickedMAX\Wicked-MAX\BulletDebugDrawer.h"
//End ZJ
#endif

//LB
#ifdef WICKEDENGINE
#include "..\Extras\VHACD\public\VHACD.h"
#endif

// access to other LIBs
#include "CObjectsC.h"
#include "CFileC.h"
#include ".\..\..\..\..\Guru-WickedMAX\wickedcalls.h"

//Dave
void BULLETAnglesFromMatrix ( GGMATRIX* pmatMatrix, GGVECTOR3* pVecAngles );

btCompoundShape*	pImporterCompoundShape;
int					pImporterMainObjectID;
char				g_ProvidedMeshFilenameForBNVHSaving[MAX_PATH];
char				g_ProvidedMeshFilenameForOBJLoading[MAX_PATH];

void DeleteallImporters(); //PE: We need to remove mem used by importers.

// externals to ragdoll system
extern DBProJointManager* jointManager;
extern DBProRagdollManager* ragdollManager;

// Constants
#define BIT(x) (1<<(x))
enum collisiontypes {
    COL_NOTHING = 0, //<Collide with nothing
    COL_TERRAIN = BIT(0), //<Collide with terrain
    COL_OBJECT = BIT(1), //<Collide with static object
	COL_CAPSULECHAR = BIT(2), //<Collide with capsule(character)
	COL_OBJECT_DYNAMIC = BIT(3), //<Collide with dynamic object
};

//PE: Debug stuff.
bool bPlayerShouldbeMoving = false;
int iPlayerStuckErrors = 0;
float fPlayerLastGoodX, fPlayerLastGoodY, fPlayerLastGoodZ;

// World Scaling (turns 1 unit = 1 inch to 1 unit = 1 meter) (75 inches high character becomes 1.8 high in Bullet)
float gSc = 40.0f;// 40.0f;

// Mass Scaling (to produce a mass value from the volume dimensions of the object)
// float gfVolumeReducer = 100000.0f; // not used
const float maxDynamicVolume = 20000.0f;
float gMs = 50.0f;

// Capsule default height
const float g_fCapsuleHeight = 45.0f;

// Global gravity values
float fCharacterGravity = 900.0f;
float fCharacterFallSpeed = 5000.0f;
float fCharacterMaxSlope = 70.0f;

// Water Line handling
#ifdef WICKEDENGINWE
float g_fWaterLineY = 0.0f;
#else
float g_fWaterLineY = 500.0f;
#endif

bool zero_gravity = false;
bool physics_playground = false;

// Globals
extern GlobStruct* g_pGlob;
char BULLETerrStr[256];
//int g_debugraycastvisual = 0;

void HandleError( char* szMessage )
{
	MessageBox( NULL, szMessage, "ODE Error", 0 );
	exit(0);
}

// Bullet globals
btAlignedObjectArray<btBulletWorldImporter*> g_importers;
btDiscreteDynamicsWorld* g_dynamicsWorld;
btAlignedObjectArray<btCollisionShape*> g_collisionShapes;
btAlignedObjectArray<btTriangleIndexVertexArray*> g_indexvertexarrays;
btDefaultCollisionConfiguration* g_collisionConfiguration;
btCollisionDispatcher* g_dispatcher;
btSequentialImpulseConstraintSolver* g_solver;
btPairCachingGhostObject* m_ghostObject = NULL;
btKinematicCharacterController* m_character = NULL;
btAxisSweep3* sweepBP = NULL;
btAxisSweep3* m_overlappingPairCache = NULL;
#ifdef WICKEDENGINE
BulletDebugDrawer g_debugDrawer;
#endif
int g_CharacterControlObject = 0;

// ray detect globals
int g_hitObjectNumber = 0;
btVector3 g_hitPointWorld;
btVector3 g_hitNormalWorld;

// gore ptr direct from exe stub
extern GlobStruct* g_pGlob;

// Data to control physics system interaction with DBP
struct sObjectList
{
	int iID;
	btRigidBody* body;
	bool bDynamicUpdate;
	btDefaultMotionState* myMotionState;
	bool bCapsuleControl;
	bool bBouyant;
	LPVOID pMem1;
	LPVOID pMem2;
	float fScaled;
	float fRaised;
	btScalar fMass;
	int iResponseMode;
};
std::vector < sObjectList > g_PhyObjectList;

// Internal functions

void BULLETReceiveCoreDataPtr ( void )
{
}

void ODEAddObject ( int iID, sObject* pObject, btRigidBody* body, int iIsDynamic, btDefaultMotionState* myMotionState, 
	                bool bCapsuleMode = false, bool bBouyant = false, LPVOID pMem1 = NULL, LPVOID pMem2 = NULL,
	                float fScaled = 1.0f, float fRaised = 1.0f, float fFriction = 90.0f, btScalar fMass = -1.0f )
{
	// add object to list
	sObjectList object;
	object.iID = iID;

	object.body = body;
	if ( iIsDynamic==1 )
		object.bDynamicUpdate = true;
	else
		object.bDynamicUpdate = false;
	object.myMotionState = myMotionState;
	object.bCapsuleControl = bCapsuleMode;
	object.bBouyant = bBouyant;
	object.pMem1 = pMem1;
	object.pMem2 = pMem2;
	object.fScaled = fScaled;
	object.fRaised = fRaised;
	object.fMass = fMass;
	object.iResponseMode = 0;
	g_PhyObjectList.push_back ( object );

	// body settings
	if ( body )
	{
		// default body settings
		float fRealFriction = 0.9f;
		if (fFriction == -1) fFriction = 90.0f;

		// 200516 - set some safety caps for friction
		if (fFriction < 0.0f) fFriction = 0.0f;
		//if (fFriction > 100.0f) fFriction = 100.0f; no upper limt, can make physics objects sticky

		if (zero_gravity) 
		{
			body->setSleepingThresholds(0.10f, 0.0025f);

			// set physics friction
			fRealFriction = fFriction / 200.0f;
			body->setFriction(fRealFriction);
			body->setDamping(0.1f, 0.05f);

			// and mass
			object.fMass = fMass * btScalar(0.25);

			// submitted by Frank Boyle (stab in the dark software)
			if (body->getInvMass() > 0.0)
			{
				btVector3 bbDimensions;
				btScalar diameter;
				bbDimensions = ((btBoxShape*)body->getCollisionShape())->getImplicitShapeDimensions();
				diameter = bbDimensions.getX();
				diameter = bbDimensions.getY() > 0.0 && bbDimensions.getY() < diameter ? bbDimensions.getY() : diameter;
				diameter = bbDimensions.getZ() > 0.0 && bbDimensions.getZ() < diameter ? bbDimensions.getZ() : diameter;
				body->setCcdMotionThreshold(diameter / 8.0f);
				body->setCcdSweptSphereRadius(diameter);
			}
		}
		else 
		{
			body->setSleepingThresholds(1.0f, 0.25f);

			// set physics friction
			fRealFriction = fFriction / 100.0f;
			body->setFriction(fRealFriction);

			// for very small things, increase damping so they do not move forever (small tins)
			if (fMass < 1.0f)
			{
				// more damping on angular velocity, and sleep threshold
				body->setDamping(0.01f, 0.75f);
				body->setSleepingThresholds(1.0f, 0.5f);
			}
			else
			{
				body->setDamping(0.01f, 0.05f);
			}

			// submitted by Frank Boyle (stab in the dark software)
			if (body->getInvMass() > 0.0)
			{
				btVector3 bbDimensions;
				btScalar diameter;
				bbDimensions = ((btBoxShape*)body->getCollisionShape())->getImplicitShapeDimensions();
				diameter = bbDimensions.getX();
				diameter = bbDimensions.getY() > 0.0 && bbDimensions.getY() < diameter ? bbDimensions.getY() : diameter;
				diameter = bbDimensions.getZ() > 0.0 && bbDimensions.getZ() < diameter ? bbDimensions.getZ() : diameter;
				body->setCcdMotionThreshold(diameter / 2.0f);
				body->setCcdSweptSphereRadius(diameter);
			}
		}
	}
}

void ODERemoveObject ( int iID )
{
	int iDeleteIndex = -1;
	for ( int i = 0; i < (int)g_PhyObjectList.size ( ); i++ )
	{
		if ( g_PhyObjectList [ i ].iID == iID )
		{
			iDeleteIndex = i;
			break;
		}
	}
	if ( iDeleteIndex > -1 )
		g_PhyObjectList.erase ( g_PhyObjectList.begin() + iDeleteIndex );
}

sObjectList* ODEFindID ( int iID )
{
	for ( int i = 0; i < (int)g_PhyObjectList.size ( ); i++ )
	{
		if ( g_PhyObjectList [ i ].iID == iID )
			return &g_PhyObjectList [ i ];
	}
	return NULL;
}

int ODEFind (int iID)
{
	if (ODEFindID(iID) != NULL)
		return 1;
	else
		return 0;
}

int ODEFindID( btRigidBody* body)
{
	for (int i = 0; i < (int)g_PhyObjectList.size(); i++)
	{
		if ( g_PhyObjectList[i].body == body )
			return g_PhyObjectList[i].iID;
	}
	return -1;
}

const int maxCollisions = 5;

struct LuaCollisionInfoDEF
{   int ObjectId;
	int numObjCollisions;
	int CollisionObjects[ maxCollisions ];
	float fX[ maxCollisions ], fY[ maxCollisions ], fZ[ maxCollisions ], fF[ maxCollisions ];
	int numTerCollisions;
	int latestTerCollision;
	float ftX[ maxCollisions ], ftY[ maxCollisions ], ftZ[ maxCollisions ];
};

std::vector< LuaCollisionInfoDEF > luaCollisionList;

void AddCollisionToList( const int A, const int B, float fX, float fY, float fZ, float fF )
{
	std::vector< LuaCollisionInfoDEF >::iterator loc = luaCollisionList.begin();
	while ( loc != luaCollisionList.end() )
	{
		if ( loc->ObjectId == A )
		{
			if ( B > 70000 )    // object on object collision  
				                // ( g.entityviewstartobj buit how do I access it? )
			{
				if ( loc->numObjCollisions >= maxCollisions ) return;
				for ( int i = 0; i < loc->numObjCollisions; i++ )
				{
					// check if we've already got an entry for this pairing
					// (we only record the first contact between the pair)
					if ( loc->CollisionObjects[i] == B ) return;
				}
				loc->CollisionObjects[ loc->numObjCollisions ] = B;
				loc->fX[ loc->numObjCollisions ] = fX;
				loc->fY[ loc->numObjCollisions ] = fY;
				loc->fZ[ loc->numObjCollisions ] = fZ;
				loc->fF[ loc->numObjCollisions ] = fF;
				loc->numObjCollisions++;
				return;
			}
			else if ( B >= 7000 ) // Assume it's a terrain collision
				                  // ( t.tphysicsterrainobjstart but how do I access it ? )
			{
				// just store cyclically and let caller sort out the order
				loc->ftX[ loc->latestTerCollision ] = fX;
				loc->ftY[ loc->latestTerCollision ] = fY;
				loc->ftZ[ loc->latestTerCollision ] = fZ;

				loc->latestTerCollision++;
				if ( loc->latestTerCollision >= maxCollisions )
				{
					loc->latestTerCollision = 0;
					loc->numTerCollisions = maxCollisions;
				}
				else if ( loc->numTerCollisions <  maxCollisions )
				{
					loc->numTerCollisions = loc->latestTerCollision;
				}
				return;
			}
		}
		loc++;
	}
}

void AddObjectCollisionCheck( const int iObjectNumber )
{
	sObjectList* pPhyObject = ODEFindID( iObjectNumber );
	if ( pPhyObject == NULL ) return;

	std::vector< LuaCollisionInfoDEF >::iterator loc = luaCollisionList.begin();
	while ( loc != luaCollisionList.end() )
	{
		if ( loc->ObjectId == iObjectNumber )
		{
			// still in the list from previous request so simply set
			// index to start recording again
			loc->numObjCollisions = 0;
			loc->numTerCollisions = 0;
			loc->latestTerCollision = 0;
			return;   // already in list
		}
		loc++;
	}
	LuaCollisionInfoDEF luaCol;
	luaCol.ObjectId = iObjectNumber;
	luaCol.numObjCollisions = 0;
	luaCol.numTerCollisions = 0; 
	luaCol.latestTerCollision = 0;
	luaCollisionList.push_back( luaCol );
}

void RemoveObjectCollisionCheck(const int iObjectNumber)
{
	sObjectList* pPhyObject = ODEFindID(iObjectNumber);
	if (pPhyObject == NULL) return;

	std::vector< LuaCollisionInfoDEF >::iterator loc = luaCollisionList.begin();
	while (loc != luaCollisionList.end())
	{
		if (loc->ObjectId == iObjectNumber)
		{
			luaCollisionList.erase(loc);
			break;
		}
		loc++;
	}
}

void GetObjectCollisionDetails( const int iObjectNumber, const int iColNumber, int &iColObj, 
	                            float &fX, float &fY, float &fZ, float &fImpulse )
{
	iColObj = 0;
	sObjectList* pPhyObject = ODEFindID( iObjectNumber );
	if (pPhyObject == NULL) return;
	std::vector< LuaCollisionInfoDEF >::iterator loc = luaCollisionList.begin();
	while ( loc != luaCollisionList.end() )
	{
		if ( loc->ObjectId == iObjectNumber )
		{
			if ( iColNumber > 0 && iColNumber <= loc->numObjCollisions )
			{
				// if last collision requested reset count for next time
				if ( iColNumber == loc->numObjCollisions ) loc->numObjCollisions = 0;
				iColObj = loc->CollisionObjects[ iColNumber - 1 ];
				fX = loc->fX[ iColNumber - 1 ];
				fY = loc->fY[ iColNumber - 1 ];
				fZ = loc->fZ[ iColNumber - 1 ];
				fImpulse = loc->fF[ iColNumber - 1 ];

				return;
			}
			else if ( loc->numObjCollisions > 0 )
			{
				// if specified collision out of range return first 
				// contact in list
				iColObj = loc->CollisionObjects[ 0 ];
				fX = loc->fX[ 0 ];
				fY = loc->fY[ 0 ];
				fZ = loc->fZ[ 0 ];
				fImpulse = loc->fF[ iColNumber - 1 ];

				return;
			}
			else
			{
				return;
			}
		}
		loc++;
	}
}

void GetTerrainCollisionDetails( const int iObjectNumber, const int iColNumber, int &iLatest,
	                             float &fX, float &fY, float &fZ)
{
	iLatest = 0;
	sObjectList* pPhyObject = ODEFindID(iObjectNumber);
	if (pPhyObject == NULL) return;
	std::vector< LuaCollisionInfoDEF >::iterator loc = luaCollisionList.begin();
	while (loc != luaCollisionList.end())
	{
		if (loc->ObjectId == iObjectNumber)
		{
			if ( iColNumber > 0 && iColNumber <= loc->numTerCollisions )
			{
				iLatest = loc->latestTerCollision;
				// if last collision requested reset count for next time
				if ( iColNumber == loc->numTerCollisions )
				{
					loc->numTerCollisions = 0;
					loc->latestTerCollision = 0;
				}
				fX = loc->ftX[iColNumber - 1];
				fY = loc->ftY[iColNumber - 1];
				fZ = loc->ftZ[iColNumber - 1];

				return;
			}
			else if (loc->numTerCollisions > 0)
			{
				// if specified collision out of range return first 
				// contact in list
				iLatest = 1;
				fX = loc->ftX[ 0 ];
				fY = loc->ftY[ 0 ];
				fZ = loc->ftZ[ 0 ];

				return;
			}
			else
			{
				return;
			}
		}
		loc++;
	}
}

int GetObjectNumCollisions( const int iObjectNumber )
{
	sObjectList* pPhyObject = ODEFindID( iObjectNumber );
	if ( pPhyObject == NULL ) return 0;
	std::vector< LuaCollisionInfoDEF >::iterator loc = luaCollisionList.begin();
	while ( loc != luaCollisionList.end() )
	{
		if ( loc->ObjectId == iObjectNumber )
		{
			return loc->numObjCollisions;
		}
		loc++;
	}
	// object not in list
	return 0;
}

int GetTerrainNumCollisions( const int iObjectNumber )
{
	sObjectList* pPhyObject = ODEFindID( iObjectNumber );
	if ( pPhyObject == NULL ) return 0;
	std::vector< LuaCollisionInfoDEF >::iterator loc = luaCollisionList.begin();
	while ( loc != luaCollisionList.end() )
	{
		if ( loc->ObjectId == iObjectNumber )
		{
			return loc->numTerCollisions;
		}
		loc++;
	}
	// object not in list
	return 0;
}


void UpdateCollisionsForLua()
{
	if ( luaCollisionList.empty()) { return; };

	const float scalefactor = 40.0;

    int numManifolds = g_dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = g_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		const int objIdA = ODEFindID( (btRigidBody*)obA );
		const int objIdB = ODEFindID( (btRigidBody*)obB );

		float fXb = 0.0; float fYb = 0.0; float fZb = 0.0; float fF = 0.0;

		int numContacts = contactManifold->getNumContacts();
		if ( numContacts > 0 )
		for ( int j = 0; j < numContacts; j++ )
		{
			btManifoldPoint& pt = contactManifold->getContactPoint( j );

			if ( pt.getDistance() < 0.f )
			{
				const btVector3& ptB = pt.getPositionWorldOnB() * scalefactor;
				fXb = ptB.getX(); fYb = ptB.getY(); fZb = ptB.getZ();
				fF = -pt.getDistance();
			}
		}

		if ( fXb != 0.0 )  // only add real contacts to list
		{

			std::vector< LuaCollisionInfoDEF >::iterator loc = luaCollisionList.begin();
			while ( loc != luaCollisionList.end() )
			{
				if ( objIdA == loc->ObjectId )
				{
					AddCollisionToList( objIdA, objIdB, fXb, fYb, fZb, fF );
				}
				else if ( objIdB == loc->ObjectId )
				{
					AddCollisionToList( objIdB, objIdA, fXb, fYb, fZb, fF );
				}
				loc++;
			}
		}
	}
}


void PostTickCallback(btDynamicsWorld *world, btScalar timeStep)
{
	UpdateCollisionsForLua();

	for (int j=((int)g_PhyObjectList.size())-1; j>=0 ;j--)
	{
		btRigidBody* body = g_PhyObjectList[j].body;
		if ( body==NULL ) continue;
		sObject* pObject = GetObjectData ( g_PhyObjectList[j].iID );
		if ( pObject==NULL ) return;
        if ( body!=NULL && body->isActive() )
        {
            btVector3 velocity = body->getLinearVelocity();
            btScalar speed = velocity.length();
            btScalar maxLinearVelocity = 999999.9f;
            if(speed > maxLinearVelocity)
            {
                velocity *= maxLinearVelocity/speed;
                body->setLinearVelocity(velocity);
            }
        }
    }
}

void ODEStart ( void )
{
	// ragdoll system
	//SAFE_DELETE_ARRAY ( jointManager );
	SAFE_DELETE ( jointManager );
	jointManager = new DBProJointManager();
	//SAFE_DELETE_ARRAY ( ragdollManager );
	SAFE_DELETE ( ragdollManager );
	ragdollManager = new DBProRagdollManager();

	// clear physics system data
	g_PhyObjectList.clear();

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	g_collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	g_dispatcher = new	btCollisionDispatcher(g_collisionConfiguration);
	btVector3 worldMin(-500,-2500,-500);
	btVector3 worldMax(1800,4000,1800);
	sweepBP = new btAxisSweep3(worldMin,worldMax);
	m_overlappingPairCache = sweepBP;

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	g_solver = new btSequentialImpulseConstraintSolver;

	// create dynamics world
	g_dynamicsWorld = new btDiscreteDynamicsWorld(g_dispatcher,m_overlappingPairCache,g_solver,g_collisionConfiguration);
#ifdef WICKEDENGINE
	g_dynamicsWorld->setDebugDrawer(&g_debugDrawer);
	g_debugDrawer.SetWorldScale(gSc);
#endif	
	//Wicked settings check.
//	g_dynamicsWorld->getSolverInfo().m_solverMode |= SOLVER_RANDMIZE_ORDER;
//	g_dynamicsWorld->getDispatchInfo().m_enableSatConvex = true;
//	g_dynamicsWorld->getSolverInfo().m_splitImpulse = true;

	// callback to do some processing after each simulation steo
	g_dynamicsWorld->setInternalTickCallback(&PostTickCallback);
	g_dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration=0.0001f;

	// set gravity
	if (zero_gravity) 
	{
		gMs = 1.0;
		g_dynamicsWorld->setGravity(btVector3(0, 0.00f, 0));
	}
	else
	{
		g_dynamicsWorld->setGravity(btVector3(0, -10.0f, 0));
	}

	// clear strings
	strcpy (g_ProvidedMeshFilenameForBNVHSaving, "");
	strcpy (g_ProvidedMeshFilenameForOBJLoading, "");
}

void DestroyThisObject ( sObjectList* pPhyObject )
{
	if ( pPhyObject )
	{
		// delete allocations for this object
		if ( pPhyObject->myMotionState ) delete pPhyObject->myMotionState;

		// remove ridig body from world
		if ( pPhyObject->body )
		{
			// store shape from ragdoll initially
			btCollisionShape* shape = pPhyObject->body->getCollisionShape();

			// remove body from world then delete body
			g_dynamicsWorld->removeRigidBody(pPhyObject->body);
			delete pPhyObject->body;

			// delete stored collision shape from collision object list and memory
			for (int j=0;j<g_collisionShapes.size();j++)
			{
				if ( g_collisionShapes[j]==shape )
				{
					g_collisionShapes[j] = 0;
					if(shape) delete shape;
					g_collisionShapes.removeAtIndex(j);
					break;
				}
			}
		}

		// release any memory created to assign physics object
		if ( pPhyObject->pMem1 ) delete[] pPhyObject->pMem1;
		if ( pPhyObject->pMem2 ) delete[] pPhyObject->pMem2;
		pPhyObject->pMem1 = NULL;
		pPhyObject->pMem2 = NULL;

		// use regular DBPro matrix again
		sObject* pObject = GetObjectData ( pPhyObject->iID );
		if ( pObject ) pObject->position.bCustomWorldMatrix = false;

		// when remove item from middle of list, messes up how objects are referenced here!
		pPhyObject->body = NULL;
		pPhyObject->myMotionState = NULL;
	}
}

void ODEEnd ( void )
{
	// ragdoll system termination
	SAFE_DELETE ( jointManager );
	SAFE_DELETE ( ragdollManager );

	///-----cleanup_start-----
	if ( g_dynamicsWorld )
	{
		// remove character controller if any
		if (m_character)
		{
			g_dynamicsWorld->removeCollisionObject(m_ghostObject);
			m_ghostObject = NULL;
			delete m_character;
			m_character = NULL;
		}

		// reset links to regular DBP objects in case any missed
		for (int j=((int)g_PhyObjectList.size())-1; j>=0 ;j--)
			DestroyThisObject ( &g_PhyObjectList[j] );

		// remove the collision objects from the dynamics world and delete them (any stragglers not deleted by DestroyThisObject)
		for (int i=g_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
		{
			btCollisionObject* obj = g_dynamicsWorld->getCollisionObjectArray()[i];
			g_dynamicsWorld->removeCollisionObject( obj );
			delete obj;
		}

		//delete collision shapes
		for (int j=0;j<g_collisionShapes.size();j++)
		{
			if ( g_collisionShapes[j]!=0 )
			{
				btCollisionShape* shape = g_collisionShapes[j];
				g_collisionShapes[j] = 0;
				if(shape) delete shape;
			}
		}
		g_collisionShapes.clear();

		// clear physics system data (no mallocs in structure, just references)
		g_PhyObjectList.clear();

		// delete physics system resources
		delete g_dynamicsWorld;

		// free indexvertex arrays (only after world destroyed)
		for (int j=0;j<g_indexvertexarrays.size();j++)
		{
			btTriangleIndexVertexArray* pIndexVertexArray = g_indexvertexarrays[j];
			SAFE_DELETE ( pIndexVertexArray );
		}
		g_indexvertexarrays.clear();

		delete g_solver;
		delete m_overlappingPairCache;
		delete g_dispatcher;
		delete g_collisionConfiguration;

		// free main var
		g_dynamicsWorld = NULL;
	}
	
	DeleteallImporters();

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	g_collisionShapes.clear();
}

// ensures the wave always has a fixed center per level start
float g_fStartingCenterPositionX = 0;
float g_fStartingCenterPositionY = 0;
float g_fStartingCenterPositionZ = 0;
int g_iActivationsPerCycle = 0;
bool* g_bBeenActivatedSinceStart = NULL;

void ODEFinalizeWorld ( void )
{
	// 290916 - performance of static geometry - instructs engine to skip updating bounds (they never move)
	if ( g_dynamicsWorld ) g_dynamicsWorld->setForceUpdateAllAabbs( false );

	// deactivate ALL dynamic physics
	for (int j = ((int)g_PhyObjectList.size()) - 1; j >= 0; j--)
	{
		btRigidBody* body = g_PhyObjectList[j].body;
		if (body == NULL) continue;
		sObject* pObject = GetObjectData (g_PhyObjectList[j].iID);
		if (g_PhyObjectList[j].iID == 7000) //PE: Fake terrain.
			continue;
		if (pObject == NULL) return;
		if (body != NULL)
		{
			body->forceActivationState(ISLAND_SLEEPING);
		}
	}

	// mark the starting center position
	g_fStartingCenterPositionX = CameraPositionX(0);
	g_fStartingCenterPositionY = CameraPositionY(0);
	g_fStartingCenterPositionZ = CameraPositionZ(0);
	g_iActivationsPerCycle = 0;
	if (g_bBeenActivatedSinceStart) delete g_bBeenActivatedSinceStart;
	//PE: We get a crash when spawning objects as there is no mem allocated for them.
	uint32_t iReserveForSpawnedObjects = 1000;
	g_bBeenActivatedSinceStart = new bool[(int)g_PhyObjectList.size() + iReserveForSpawnedObjects];
	for (int j = 0; j < (int)g_PhyObjectList.size() + iReserveForSpawnedObjects; j++)
		g_bBeenActivatedSinceStart[j] = false;
}

float ODEProjectActivationWave ( float fDistanceWithinToActivate, float fSlices )
{
	// to ease the initial physics burden, cast an activation wave so dynamic things can fall from sky and settle over the initial seconds
	// after calling this with the desired distance to activate
	if (!g_bBeenActivatedSinceStart)
		return fDistanceWithinToActivate;
	bool bMoveOnWhenAllActivatedInSlice = true;
	for (int j = ((int)g_PhyObjectList.size()) - 1; j >= 0; j--)
	{
		btRigidBody* body = g_PhyObjectList[j].body;
		if (body == NULL) continue;
		sObject* pObject = GetObjectData (g_PhyObjectList[j].iID);
		if (pObject == NULL) continue;
		if (body != NULL)
		{
			float fDX = g_fStartingCenterPositionX - pObject->position.vecPosition.x;
			float fDY = g_fStartingCenterPositionY - pObject->position.vecPosition.y;
			float fDZ = g_fStartingCenterPositionZ - pObject->position.vecPosition.z;
			float fDist = sqrt(fabs(fDX*fDX) + fabs(fDY*fDY) + fabs(fDZ*fDZ));
			if (fDist > fDistanceWithinToActivate && fDist <= fDistanceWithinToActivate + fSlices)
			{
				if (body->isActive() == false && g_bBeenActivatedSinceStart[j] == false)
				{
					bMoveOnWhenAllActivatedInSlice = false;
					if (g_iActivationsPerCycle > 0)
					{
						g_iActivationsPerCycle--;
					}
					else
					{
						body->activate();
						g_bBeenActivatedSinceStart[j] = true;
						g_iActivationsPerCycle = 30;
					}
					break;
				}
			}
		}
	}
	if (bMoveOnWhenAllActivatedInSlice == true)
	{
		fDistanceWithinToActivate += fSlices;
	}
	return fDistanceWithinToActivate;
}

GGQUATERNION BT2DX_QUATERNION(const btQuaternion &q)
{
 return GGQUATERNION(q.x(), q.y(), q.z(), q.w());
}

GGMATRIX BT2DX_MATRIX(const btTransform &ms)
{
 btQuaternion q = ms.getRotation();
 btVector3 p = ms.getOrigin();

 GGMATRIX pos, rot, world;
 GGMatrixTranslation(&pos, p.x(), p.y(), p.z());
 GGMatrixRotationQuaternion(&rot, &BT2DX_QUATERNION(q));
 GGMatrixMultiply(&world, &rot, &pos);

 return world;
}

//Dave
float LinearInterpolate ( float y1, float y2, float mu )
{
   return ( y1 * ( 1 - mu ) + y2 * mu );
}

float BULLETCosineInterpolate ( float y1, float y2, float mu )
{
 float mu2 = ( 1 - cos ( mu * 3.141f ) ) / 2;
 return ( y1 * ( 1 - mu2 ) + y2 * mu2 );
}

//#ifdef PUTPHYSICSBACKINMAINTHREADSEQUENCE
//std::mutex physicslock = {};
std::recursive_mutex physicslock = {};
//#else
//class physicslockclass
//{
//public:
//	void lock(void) {};
//	void unlock(void) {};
//};
//physicslockclass physicslock;
//#endif

void Update(float fTimeStep)
{
	// step physics simulation
	//PE: 1.0f/120.0f is what is killing the bullet engine (fps drops) , simulation in 1:1 is 1.0f / 60.0f, 1.0f/120.0f run twice as fast.
	//PE: We are calling it at 60 fps , so bullet is always behind and need to call the simulation many times per frame to keep up.
	//PE: Well this change physics demos and how they are coded , bad luck ...
	//PE: We should consider to use this as the default in Max.
	
	//PE: Cant touch physics object while running stepsimulation.
	physicslock.lock();
	#ifdef FASTBULLETPHYSICS
		//PE: fixed timestep with interpolation set at 1/60.
		g_dynamicsWorld->stepSimulation(fTimeStep, 7, btScalar(1.) / btScalar(60.));
	#else
		g_dynamicsWorld->stepSimulation(fTimeStep, 7, 1.0f / 120.0f);
	#endif

	physicslock.unlock();

	// update ragdoll simulations
	ragdollManager->Update();

	// character controller update
	if (g_CharacterControlObject > 0)
	{
		// position of character
		btTransform characterWorldTrans;
		characterWorldTrans = m_ghostObject->getWorldTransform();
		if (isnan(characterWorldTrans.getOrigin().getX()) == 0)
		{
			// update object position vector
			float fHorizontalSmoothing = 0.35f;
			float fVerticalSmoothing = 0.5f;

			sObject* p1 = GetObjectData(g_CharacterControlObject);
			float fMoveX = (p1->position.vecPosition.x - (characterWorldTrans.getOrigin().getX()*gSc));
			float fMoveY = (p1->position.vecPosition.y - (characterWorldTrans.getOrigin().getY()*gSc));
			float fMoveZ = (p1->position.vecPosition.z - (characterWorldTrans.getOrigin().getZ()*gSc));
			fMoveX = (fMoveX * fHorizontalSmoothing);
			fMoveY = (fMoveY * fVerticalSmoothing);
			fMoveZ = (fMoveZ * fHorizontalSmoothing);
			float newX = p1->position.vecPosition.x - fMoveX;
			float newZ = p1->position.vecPosition.z - fMoveZ;
			float newY = p1->position.vecPosition.y;// -fMoveY; // ensures when 0,0,0 gravity set, character does not slide downward (ladder/zipline)
			if (m_character->getGravity()!=0.0f ) newY -= fMoveY;
			PositionObject(g_CharacterControlObject, newX, newY, newZ);

			// update object rotation which returns status (X=whether can jump)
			float fAngleX = 0.0f;
			if (m_character->wasOnGround()) fAngleX = 1.0f;
			RotateObject(g_CharacterControlObject, fAngleX, 0.0f, 0.0f);
		}
	}

	// print positions of all objects
	physicslock.lock();
	for (int j = ((int)g_PhyObjectList.size()) - 1; j >= 0; j--)
	{
		// get this object
		btRigidBody* body = g_PhyObjectList[j].body;
		if (body == NULL) continue;
		sObject* pObject = GetObjectData(g_PhyObjectList[j].iID);
		if (pObject == NULL) continue;

		// handle response mode to freeze objects
		if (g_PhyObjectList[j].iResponseMode == 1)
		{
			body->setLinearVelocity(btVector3(0, 0, 0));
		}

		// handle physics body visual update
		if (body->getMotionState() && g_PhyObjectList[j].bDynamicUpdate == true)
		{
			// get position from physics body
			btTransform trans;
			body->getMotionState()->getWorldTransform(trans);

			// 210516 - skip NAN values if detected in BODY (massive friction etc)
			if (isnan(trans.getOrigin().getX()) == 1)
			{
				continue;
			}

			// full object control or bCapsuleControl
			if (g_PhyObjectList[j].bCapsuleControl)
			{
				// Update DBP object position so can get coordinate back in DBP
				// handled differently, capsules are secondary and controlled from movement system
				// LB: can set arbvalue here based on material 'arb' being stood on, so the footfall of the
				// capsule (character) is accurate to the material being traversed
				int materialindex = 0;
				g_PhyObjectList[j].body->setUserPointer((void*)(1+materialindex));
			}
			else
			{
				// get body angles
				btQuaternion angles;
				angles = body->getOrientation();

				// make the rotation matrix
				GGMATRIX matRotation;
				GGQUATERNION qRotation;
				qRotation.x = angles.getX();
				qRotation.y = angles.getY();
				qRotation.z = angles.getZ();
				qRotation.w = angles.getW();
				GGMatrixRotationQuaternion(&matRotation, &qRotation);

				// LEE, I think the matrix returned is a ZYX order rotation, but we need XYZ
				// so it translates back properly. I could use ZYX in FPSC but it will screw up all maps..
				if (pObject->position.bApplyPivot)
					matRotation = pObject->position.matPivot * matRotation;

				// Use physics object position
				GGMATRIX matTranslation;
				GGMatrixTranslation(&matTranslation, trans.getOrigin().getX()*gSc, trans.getOrigin().getY()*gSc, trans.getOrigin().getZ()*gSc);

				// Use physics object position				
				GGMATRIX matCenterColOffset;
				float fXSize = (pObject->collision.vecMax.x - pObject->collision.vecMin.x) * pObject->position.vecScale.x;//;
				float fYSize = (pObject->collision.vecMax.y - pObject->collision.vecMin.y) * pObject->position.vecScale.y;//;
				float fZSize = (pObject->collision.vecMax.z - pObject->collision.vecMin.z) * pObject->position.vecScale.z;//;
				float fXOffset = (pObject->collision.vecMin.x * pObject->position.vecScale.x) + (fXSize / 2.0f);
				float fYOffset = (pObject->collision.vecMin.y * pObject->position.vecScale.y) + (fYSize / 2.0f);
				float fZOffset = (pObject->collision.vecMin.z * pObject->position.vecScale.z) + (fZSize / 2.0f);

				if (pObject->pFrame)
				{
					//PE: This only work for box, dont work for dynamic convexhull shapes ?
					//PE: pFrame->vecOffset  is already in matTranslation, so only offset center of rotation.
					fXOffset += -(pObject->pFrame->vecOffset.x * pObject->position.vecScale.x);
					fYOffset += -(pObject->pFrame->vecOffset.y * pObject->position.vecScale.y);
					fZOffset += -(pObject->pFrame->vecOffset.z * pObject->position.vecScale.z);
					//PE: Add pFrame->vecRotation to matWorld.
					GGMATRIX matRotateFrameX, matRotateFrameY, matRotateFrameZ;
					GGMatrixRotationX(&matRotateFrameX, GGToRadian(pObject->pFrame->vecRotation.x));	// x rotation
					GGMatrixRotationY(&matRotateFrameY, GGToRadian(pObject->pFrame->vecRotation.y));	// y rotation
					GGMatrixRotationZ(&matRotateFrameZ, GGToRadian(pObject->pFrame->vecRotation.z));	// z rotation
					matRotation = matRotateFrameX * matRotateFrameY * matRotateFrameZ * matRotation;
				}

				// All, the trick was to put 'matCenterColOffset' before the rotation step to correct object offset issue :)
				PositionObject(g_PhyObjectList[j].iID, trans.getOrigin().getX()*gSc, trans.getOrigin().getY()*gSc, trans.getOrigin().getZ()*gSc);
				GGMatrixTranslation(&matCenterColOffset, -fXOffset, -fYOffset, -fZOffset);
				GGMATRIX matWorld = pObject->position.matScale * matCenterColOffset * matRotation * matTranslation;

				// use ODE matrix, not regular DBPro matrix
				pObject->position.bCustomWorldMatrix = true;
				pObject->position.matWorld = matWorld;

				// communicate custom world matrix to wicked
				WickedCall_UpdateObject(pObject);

				// update DBPro object position and rotation to match physics
				pObject->position.vecPosition = GGVECTOR3(matWorld._41, matWorld._42, matWorld._43);

				GGVECTOR3 vecRotate = GGVECTOR3(0, 0, 0);
				BULLETAnglesFromMatrix(&matWorld, &vecRotate);
				pObject->position.vecRotate = vecRotate;
				pObject->collision.bColCenterUpdated = true;
				body->applyDamping(0.1f);
			}
		}

		// Handle bouyancy
		if (g_PhyObjectList[j].bBouyant == true)
		{
			// water line to control strength of bouyancy
			float fWaterLine = g_fWaterLineY - pObject->position.vecPosition.y;

			// SK: Simplified boyancy for now
			if (fWaterLine > 0.0f)
			{
				float fForce = fWaterLine / g_fWaterLineY;
				if (fForce > 0.03f) fForce = 0.03f;
				btVector3 vOldVel = body->getLinearVelocity();
				float fYVel = vOldVel.getY();
				if (fYVel > 2.0f) fYVel = 2.0f; // Slow ascent
				body->setLinearVelocity(btVector3(vOldVel.getX()*0.75f, fYVel*0.75f, vOldVel.getZ()*0.75f));
				vOldVel = body->getAngularVelocity();
				body->setAngularVelocity(btVector3(vOldVel.getX()*0.99f, fYVel*0.99f, vOldVel.getZ()*0.99f));
				body->applyDamping(0.5f);
			}
		}
	}
	physicslock.unlock();

	// Draw the debug physics objects.
	if (g_debugDrawer.getDebugMode() != 0)
	{
		btRigidBody* rBody = nullptr;
		btTransform transform;

		// Draw the dynamic objects.
		for (int i = 0; i < g_PhyObjectList.size(); i++)
		{
			rBody = g_PhyObjectList[i].body;
			if (rBody == nullptr) continue;
			if (!rBody->isStaticObject())
			{
				rBody->getMotionState()->getWorldTransform(transform);
				// LB: not sure wwhy this was commented out??
				g_dynamicsWorld->debugDrawObject(transform, rBody->getCollisionShape(), btVector3(0, 1, 0));
			}
			rBody = nullptr;
		}

		// Draw the constraints.
		int iConstraintCount = jointManager->GetNumberOfJoints();
		for (int i = 0; i < iConstraintCount; i++)
		{
			btTypedConstraint* constraint = jointManager->GetJoint(i)->GetConstraint();
			if (constraint)
			{
				constraint->setDbgDrawSize(0.5f);
				g_dynamicsWorld->debugDrawConstraint(constraint);
			}
		}
	}
}

void ODEUpdate ( void )
{
	Update ( 1.0f/60.0f );
}

void ODEUpdate ( float fManualStep )
{
	Update ( fManualStep );
}

// Creation commands

//void GetSizePositionRotation( sObject* pObject, float &fXSize, float &fYSize, float &fZSize,
//	                                            float &fXPos,  float &fYPos,  float &fZPos,
//	                                            float &fXRot,  float &fYRot,  float &fZRot,
//												bool bMakeCollisionCenterAdjustment, float fScalingFactor = 1.0f )
//{
//	fXSize = ( pObject->collision.vecMax.x - pObject->collision.vecMin.x) * pObject->position.vecScale.x * fScalingFactor;
//	fYSize = ( pObject->collision.vecMax.y - pObject->collision.vecMin.y) * pObject->position.vecScale.y * fScalingFactor;
//	fZSize = ( pObject->collision.vecMax.z - pObject->collision.vecMin.z) * pObject->position.vecScale.z * fScalingFactor;
//	fXPos = pObject->position.vecPosition.x;//+ pObject->pFrame->vecOffset.x;
//	fYPos = pObject->position.vecPosition.y;//+ pObject->pFrame->vecOffset.y;
//	fZPos = pObject->position.vecPosition.z;//+ pObject->pFrame->vecOffset.z;
//	fXRot = GGToRadian( pObject->position.vecRotate.x + pObject->pFrame->vecRotation.x );
//	fYRot = GGToRadian( pObject->position.vecRotate.y + pObject->pFrame->vecRotation.y );
//	fZRot = GGToRadian( pObject->position.vecRotate.z + pObject->pFrame->vecRotation.z );
//
//	// adjust for center of object as all physics objects coords are at center
//	if ( bMakeCollisionCenterAdjustment == true )
//	{
//		float fXOffset = (pObject->collision.vecMin.x * pObject->position.vecScale.x) + (fXSize / 2.0f);
//		float fYOffset = (pObject->collision.vecMin.y * pObject->position.vecScale.y) + (fYSize / 2.0f);
//		float fZOffset = (pObject->collision.vecMin.z * pObject->position.vecScale.z) + (fZSize / 2.0f);
//		GGVECTOR3 vColOffset = GGVECTOR3( fXOffset, fYOffset, fZOffset );
//		GGMATRIX matRot;
//		GGMatrixRotationX( &matRot, fXRot );  GGVec3TransformCoord( &vColOffset, &vColOffset, &matRot );
//		GGMatrixRotationY( &matRot, fYRot );  GGVec3TransformCoord( &vColOffset, &vColOffset, &matRot );
//		GGMatrixRotationZ( &matRot, fZRot );  GGVec3TransformCoord( &vColOffset, &vColOffset, &matRot );
//		fXPos += vColOffset.x;
//		fYPos += vColOffset.y;
//		fZPos += vColOffset.z;
//	}
//
//}

void GetSizePositionRotation(sObject* pObject, float &fXSize, float &fYSize, float &fZSize,
	float &fXPos, float &fYPos, float &fZPos,
	float &fXRot, float &fYRot, float &fZRot,
	bool bMakeCollisionCenterAdjustment, float fScalingFactor = 1.0f)
{
	fXSize = (pObject->collision.vecMax.x - pObject->collision.vecMin.x) * pObject->position.vecScale.x * fScalingFactor;
	fYSize = (pObject->collision.vecMax.y - pObject->collision.vecMin.y) * pObject->position.vecScale.y * fScalingFactor;
	fZSize = (pObject->collision.vecMax.z - pObject->collision.vecMin.z) * pObject->position.vecScale.z * fScalingFactor;
	fXPos = pObject->position.vecPosition.x;//+ pObject->pFrame->vecOffset.x;
	fYPos = pObject->position.vecPosition.y;//+ pObject->pFrame->vecOffset.y;
	fZPos = pObject->position.vecPosition.z;//+ pObject->pFrame->vecOffset.z;
	fXRot = GGToRadian(pObject->position.vecRotate.x);
	fYRot = GGToRadian(pObject->position.vecRotate.y);
	fZRot = GGToRadian(pObject->position.vecRotate.z);

	// adjust for center of object as all physics objects coords are at center
	if (bMakeCollisionCenterAdjustment == true)
	{
		float fXOffset = (pObject->collision.vecMin.x * pObject->position.vecScale.x) + (fXSize / 2.0f);
		float fYOffset = (pObject->collision.vecMin.y * pObject->position.vecScale.y) + (fYSize / 2.0f);
		float fZOffset = (pObject->collision.vecMin.z * pObject->position.vecScale.z) + (fZSize / 2.0f);
		GGVECTOR3 vObjectOffset = { 0.0,0.0,0.0 };
		GGVECTOR3 vObjectOffsetCenter = { 0.0,0.0,0.0 };

		#ifdef WICKEDENGINE
		if (pObject->pFrame)
		{
			//PE: Offset center of rotation for new pFrame->vecOffset values.
			vObjectOffsetCenter.x = -(pObject->pFrame->vecOffset.x * pObject->position.vecScale.x);
			vObjectOffsetCenter.y = -(pObject->pFrame->vecOffset.y * pObject->position.vecScale.y);
			vObjectOffsetCenter.z = -(pObject->pFrame->vecOffset.z * pObject->position.vecScale.z);

			//PE: Rotation matrix
			GGMATRIX matRotation;
			GGMATRIX matRotateX, matRotateY, matRotateZ;
			GGMatrixRotationX(&matRotateX, GGToRadian(pObject->pFrame->vecRotation.x));	// x rotation
			GGMatrixRotationY(&matRotateY, GGToRadian(pObject->pFrame->vecRotation.y));	// y rotation
			GGMatrixRotationZ(&matRotateZ, GGToRadian(pObject->pFrame->vecRotation.z));	// z rotation
			matRotation = matRotateX * matRotateY * matRotateZ;

			//PE: Translation with matRotation for pFrame->vecRotation
			GGVECTOR3 vecPosOffset = GGVECTOR3(pObject->pFrame->vecOffset.x, pObject->pFrame->vecOffset.y, pObject->pFrame->vecOffset.z);
			GGVec3TransformCoord(&vecPosOffset, &vecPosOffset, &matRotation);
			vObjectOffset.x = vecPosOffset.x;
			vObjectOffset.y = vecPosOffset.y;
			vObjectOffset.z = vecPosOffset.z;
		}
		#endif

		// and now account for the collision center shift
		GGVECTOR3 vColOffset = GGVECTOR3( fXOffset + vObjectOffsetCenter.x, fYOffset + vObjectOffsetCenter.y, fZOffset + vObjectOffsetCenter.z );
		GGMATRIX matRot;
		GGMatrixRotationX( &matRot, fXRot );  GGVec3TransformCoord( &vColOffset, &vColOffset, &matRot );
		GGMatrixRotationY( &matRot, fYRot );  GGVec3TransformCoord( &vColOffset, &vColOffset, &matRot );
		GGMatrixRotationZ( &matRot, fZRot );  GGVec3TransformCoord( &vColOffset, &vColOffset, &matRot );
		fXPos += vColOffset.x + vObjectOffset.x;
		fYPos += vColOffset.y + vObjectOffset.y;
		fZPos += vColOffset.z + vObjectOffset.z;
	}
	else
	{
		#ifdef WICKEDENGINE
		//PE: Make sure only objects are checked , not terrain ...
		if (pObject->dwObjectNumber > 70000 && pObject->pFrame)
		{
			// vars
			bool ConvexHull = false;
			GGVECTOR3 vObjectOffset = { 0.0,0.0,0.0 };
			GGVECTOR3 vObjectOffsetCenter = { 0.0,0.0,0.0 };

			// rotation matrix
			GGMATRIX matRotation;
			GGMATRIX matRotateX, matRotateY, matRotateZ;
			GGMatrixRotationX(&matRotateX, GGToRadian(pObject->pFrame->vecRotation.x));	// x rotation
			GGMatrixRotationY(&matRotateY, GGToRadian(pObject->pFrame->vecRotation.y));	// y rotation
			GGMatrixRotationZ(&matRotateZ, GGToRadian(pObject->pFrame->vecRotation.z));	// z rotation
			matRotation = matRotateX * matRotateY * matRotateZ;

			// translation with matRotation for pFrame->vecRotation
			GGVECTOR3 vecPosOffset = GGVECTOR3(pObject->pFrame->vecOffset.x, pObject->pFrame->vecOffset.y, pObject->pFrame->vecOffset.z);
			GGVec3TransformCoord(&vecPosOffset, &vecPosOffset, &matRotation);
			vObjectOffset.x = vecPosOffset.x;
			vObjectOffset.y = vecPosOffset.y;
			vObjectOffset.z = vecPosOffset.z;

			//PE: if offset in matrix , ignore.
			if (!(floor(pObject->pFrame->matCombined._41) == floor(pObject->pFrame->vecOffset.x) &&
				floor(pObject->pFrame->matCombined._42) == floor(pObject->pFrame->vecOffset.y) &&
				floor(pObject->pFrame->matCombined._43) == floor(pObject->pFrame->vecOffset.z)) )
			{
				vObjectOffset.x = 0;
				vObjectOffset.y = 0;
				vObjectOffset.z = 0;
			}
			// apply offset of root frame , PE: we now check for frame indentity mastrix so also add offset.
			fXPos += vObjectOffset.x;
			fYPos += vObjectOffset.y;
			fZPos += vObjectOffset.z;
		}
		#endif
	}
}

void fillTransform( btTransform &trans, float fXPos, float fYPos, float fZPos, float fXRot, float fYRot, float fZRot )
{
	trans.setIdentity();

	trans.setOrigin( btVector3( fXPos, fYPos, fZPos ) );

	GGQUATERNION QuatAroundX( 1, 0, 0, fXRot );
	GGQUATERNION QuatAroundY( 0, 1, 0, fYRot );
	GGQUATERNION QuatAroundZ( 0, 0, 1, fZRot );

	GGQuaternionRotationAxis( &QuatAroundX, &GGVECTOR3( 1, 0, 0 ), fXRot );
	GGQuaternionRotationAxis( &QuatAroundY, &GGVECTOR3( 0, 1, 0 ), fYRot );
	GGQuaternionRotationAxis( &QuatAroundZ, &GGVECTOR3( 0, 0, 1 ), fZRot );

	GGQUATERNION finalOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
	trans.setRotation( btQuaternion( finalOrientation.x,
		                             finalOrientation.y,
		                             finalOrientation.z,
	                                 finalOrientation.w ) );
}

void CreateBox ( int iObjectNumber, int isDynamic, int iLimbNumber, int iTipOverMode, float fWeight, float fFriction, float fRestitution,
				 bool blnManual, float fXIN, float fYIN, float fZIN, float fXSizeIN, float fYSizeIN, float fZSizeIN, float fXAngIN, float fYAngIN, float fZAngIN)
{
	// defaults
	if ( fWeight==-1 ) fWeight = 100.0f;
	if ( fFriction==-1 ) fFriction = 90;
	if ( fRestitution==-1 ) fRestitution = 0.0f;

	// get reference object
	sObject* pObject = GetObjectData ( iObjectNumber );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot;

	if (blnManual)
	{
		fXPos = fXIN;
		fYPos = fYIN;
		fZPos = fZIN;
		fXSize = fXSizeIN;
		fYSize = fYSizeIN;
		fZSize = fZSizeIN;
		fXRot = GGToRadian (fXAngIN);
		fYRot = GGToRadian (fYAngIN);
		fZRot = GGToRadian (fZAngIN);
	}
	else
	{
		GetSizePositionRotation( pObject, fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot, true);
	}
	
	// real object
	sObject* pRealObject = pObject;
	if ( pObject->pInstanceOfObject ) pRealObject = pObject->pInstanceOfObject;
	
	// if limb number specified
	if ( iLimbNumber!=-1 && iLimbNumber<pRealObject->iFrameCount )
	{
		// change boundbox to that of the limb
		sFrame* pLimbFrame = pRealObject->ppFrameList[iLimbNumber];
		if ( pLimbFrame )
		{
			sMesh* pLimbMesh = pLimbFrame->pMesh;
			if ( pLimbMesh )
			{
				fXSize = ( pLimbMesh->Collision.vecMax.x - pLimbMesh->Collision.vecMin.x );
				fYSize = ( pLimbMesh->Collision.vecMax.y - pLimbMesh->Collision.vecMin.y );
				fZSize = ( pLimbMesh->Collision.vecMax.z - pLimbMesh->Collision.vecMin.z );
				GGVECTOR3 vLimbCenter = GGVECTOR3(pLimbMesh->Collision.vecMin.x+(fXSize/2),pLimbMesh->Collision.vecMin.y+(fYSize/2),pLimbMesh->Collision.vecMin.z+(fZSize/2));
				GGVec3TransformCoord ( &vLimbCenter, &vLimbCenter, &pLimbFrame->matAbsoluteWorld );
				fXPos = vLimbCenter.x;
				fYPos = vLimbCenter.y;
				fZPos = vLimbCenter.z;
				fXSize = fXSize * pObject->position.vecScale.x;
				fYSize = fYSize * pObject->position.vecScale.y;
				fZSize = fZSize * pObject->position.vecScale.z;
			}
		}
	}

	// ensure very small objects are not allowed below this size
	if ( fXSize < 3 ) fXSize = 3;
	if ( fYSize < 4 ) fYSize = 4;
	if ( fZSize < 3 ) fZSize = 3;

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// volume calculation
	float fVolume = 0;
	fVolume = fXSize * fYSize * fZSize;

	// modify with weight modifier
	fVolume *= (fWeight/100.0f);

	// can specify special collision type through iTipOverMode
	bool bDoNotMoveAnyOtherObjects = false;
	bool bLeavePlayerAlone = false;
	bool bDoNotCollideWithCapsuleCharacters = false;
	if ( iTipOverMode == 11 ) { bDoNotMoveAnyOtherObjects=true; iTipOverMode = 0; }
	if ( iTipOverMode == 12 ) { bLeavePlayerAlone = true; iTipOverMode = 0; }
	if ( iTipOverMode == 13 ) { bDoNotCollideWithCapsuleCharacters = true; iTipOverMode = 0; }

	//PE: Weapon drop , must wait for physics thread to end.
	physicslock.lock();

	// create a bullet shape
	btCollisionShape* boxShape = NULL;
	if ( iTipOverMode==0 )
	{
		// regular box
		boxShape = new btBoxShape(btVector3(btScalar(fXSize/2.0f),btScalar(fYSize/2.0f),btScalar(fZSize/2.0f)));
	}
	else
	{
		// create additional shape for a TipOver shape (balancing on gun nozzle)
		btCompoundShape* pCompound = new btCompoundShape();
		btTransform localTrans;
		localTrans.setIdentity();
		btCollisionShape* pShape1 = new btBoxShape(btVector3(btScalar(fXSize/2.0f),btScalar(fYSize/2.0f),btScalar(fZSize/2.0f)));
		float fXSize2 = fXSize * 1.5f;
		float fYSize2 = fYSize * 1.5f;
		float fZSize2 = fZSize * 1.5f;
		if ( iTipOverMode==1 ) fXSize2 = 0.005f/gSc;
		if ( iTipOverMode==2 ) fYSize2 = 0.005f/gSc;
		if ( iTipOverMode==3 ) fZSize2 = 0.005f/gSc;
		btCollisionShape* pShape2 = new btBoxShape(btVector3(btScalar(fXSize2/2.0f),btScalar(fYSize2/2.0f),btScalar(fZSize2/2.0f)));
		pCompound->addChildShape(localTrans, pShape1);
		pCompound->addChildShape(localTrans, pShape2);
		boxShape = (btCollisionShape*)pCompound;
	}

	// add shape to collision list so can release later, they can be "re-used" amongst bodies!!
	g_collisionShapes.push_back(boxShape);

	// mass calculation
	if (isDynamic == 0)
	{
		fVolume = 0;
	}
	else
	{
		// max
		if (fVolume > maxDynamicVolume) fVolume = maxDynamicVolume;

		// also handle min - ensure MASS not SO SMALL THINGS FLY MILLIONS OF MILES!
		if (fVolume < 0.1f) fVolume = 0.1f;
	}
	btScalar mass(fVolume/gMs);
	btVector3 localInertia(0,0,0);
	if ( isDynamic==1 ) boxShape->calculateLocalInertia(mass,localInertia);

	// set position transform for physics object
	btTransform startTransform;
	fillTransform( startTransform, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot );

	// motionstate provides interpolation capabilities and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,boxShape,localInertia);
	rbInfo.m_friction = 1.0f;
	rbInfo.m_restitution = fRestitution/100.0f;
	btRigidBody* body = new btRigidBody(rbInfo);

	// pass material index as usrptr
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	body->setUserPointer((void*)iArbValue);

	// also remove all contact response
	if ( bDoNotMoveAnyOtherObjects==true )
	{
		body->setCollisionFlags ( btCollisionObject::CF_NO_CONTACT_RESPONSE );
	}

	// add physics object to system
	short sColliderType = COL_OBJECT;
	if (isDynamic == 1) sColliderType = COL_OBJECT_DYNAMIC;
	short sCollidesWith = COL_OBJECT | COL_OBJECT_DYNAMIC | COL_CAPSULECHAR | COL_TERRAIN;
	if (bDoNotCollideWithCapsuleCharacters == true)
	{
		// this mode is used for the physics box to project where characters ragdoll plus falls
		sColliderType = COL_CAPSULECHAR;
		sCollidesWith = COL_OBJECT | COL_TERRAIN; // ignore dynamic objects (so box does not sit ON TOP OF THEM
	}

	if (g_dynamicsWorld) 
	{
		if (bLeavePlayerAlone) 
		{
			//PE: Brass is hitting and adding forces to gun and player.
			//PE: Most easy way to fix this is get it setup as COL_CAPSULECHAR
			//PE: So collision with object and terrain still works, but its not part of the COL_OBJECT collision group.
			g_dynamicsWorld->addRigidBody(body, COL_CAPSULECHAR, sCollidesWith);
		}
		else 
		{
			g_dynamicsWorld->addRigidBody(body, sColliderType, sCollidesWith);
		}
	}
	// boyancy mode
	bool bBouyant = false;
	if ( isDynamic==1 ) 
		bBouyant = true; 
	else
		pObject = NULL;

	// add physics object details to pbject list
	ODEAddObject ( iObjectNumber, pObject, body, isDynamic, myMotionState, false, bBouyant, NULL, NULL, 1, 1, fFriction, mass );
	
	physicslock.unlock();
}

void StartStaticObject ( int iObjectNumber )
{
	pImporterCompoundShape = new btCompoundShape;
	pImporterMainObjectID = iObjectNumber;
}

void EndStaticObject ( int iObjectNumber, int iTerrainCollider )
{
	// get reference object
	sObject* pObject = GetObjectData ( iObjectNumber );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fXSize = (pObject->collision.vecMax.x - pObject->collision.vecMin.x) * pObject->position.vecScale.x;
	float fYSize = (pObject->collision.vecMax.y - pObject->collision.vecMin.y) * pObject->position.vecScale.y;
	float fZSize = (pObject->collision.vecMax.z - pObject->collision.vecMin.z) * pObject->position.vecScale.z;
	float fXPos = pObject->position.vecPosition.x;
	float fYPos = pObject->position.vecPosition.y;
	float fZPos = pObject->position.vecPosition.z;
	float fXRot = GGToRadian(pObject->position.vecRotate.x);
	float fYRot = GGToRadian(pObject->position.vecRotate.y);
	float fZRot = GGToRadian(pObject->position.vecRotate.z);

	// adjust for center of object as all physics objects coords are at center
	GGVECTOR3 vColOffset = GGVECTOR3(0, 0, 0);
	GGMATRIX matRot;
	GGMatrixRotationX(&matRot, fXRot);	GGVec3TransformCoord(&vColOffset, &vColOffset, &matRot);
	GGMatrixRotationY(&matRot, fYRot);	GGVec3TransformCoord(&vColOffset, &vColOffset, &matRot);
	GGMatrixRotationZ(&matRot, fZRot); GGVec3TransformCoord(&vColOffset, &vColOffset, &matRot);
	fXPos += vColOffset.x;
	fYPos += vColOffset.y;
	fZPos += vColOffset.z;

	// real object
	sObject* pRealObject = pObject;
	if ( pObject->pInstanceOfObject ) pRealObject = pObject->pInstanceOfObject;
	
	// ensure very small objects are not allowed below this size
	if ( fXSize < 6 ) fXSize = 6;
	if ( fYSize < 6 ) fYSize = 6;
	if ( fZSize < 6 ) fZSize = 6;

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// add shape to collision list so can release later, they can be "re-used" amongst bodies!!
	g_collisionShapes.push_back(pImporterCompoundShape);

	// mass/volume calculation
	btScalar mass(0);
	btVector3 localInertia(0,0,0);

	// set position transform for physics object
	btTransform startTransform;
	fillTransform( startTransform, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot );

	// motionstate provides interpolation capabilities and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,pImporterCompoundShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// pass material index as usrptr
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	body->setUserPointer((void*)iArbValue);

	// add physics object to system
	short sCollidesWith = COL_OBJECT | COL_OBJECT_DYNAMIC | COL_CAPSULECHAR | COL_TERRAIN;
	if ( iTerrainCollider == 1 )
		g_dynamicsWorld->addRigidBody ( body, COL_TERRAIN, sCollidesWith);
	else
		g_dynamicsWorld->addRigidBody ( body, COL_OBJECT, sCollidesWith);

	// boyancy mode
	bool bBouyant = false;
	pObject = NULL;

	// add physics object details to pbject list
	ODEAddObject ( iObjectNumber, pObject, body, 0, myMotionState, false, bBouyant, NULL, NULL, 1, 1, 100.0f, mass );
}

int CreateHingeSingle( int iObjectNumber, float fX, float fY, float fZ, int iType, float fAngMin, float fAngMax )
{
	sObjectList* pPhyObject = ODEFindID( iObjectNumber );
	if ( pPhyObject == NULL ) return -1; 
	if ( pPhyObject->body == NULL ) return -1;
	if ( iType <= 0 || iType > 3 ) return -1;
	btVector3 vType;
	if (iType == 1) vType = btVector3(1, 0, 0);  // hinge on objects X axis
	if (iType == 2) vType = btVector3(0, 1, 0);  // hinge on objects Y axis
	if (iType == 3) vType = btVector3(0, 0, 1);  // hinge on objects Z axis

	btHingeConstraint *hingeC = new btHingeConstraint( *(pPhyObject->body),
		                                               btVector3(fX / gSc, fY / gSc, fZ / gSc),
		                                               vType,
		                                               true );
	if ( fAngMin != 0.0 || fAngMax != 0.0 )
	{
		hingeC->setLimit( btScalar( fAngMin ), btScalar( fAngMax ) );
	}
	g_dynamicsWorld->addConstraint( hingeC );

	// need to find constraint index and return it to caller (this is really naff!)
	for (int i = 0; i < g_dynamicsWorld->getNumConstraints(); i++)
	{
		btTypedConstraint *tC = g_dynamicsWorld->getConstraint(i);
		if ( tC == hingeC ) return i;
	}
	return -1;
}

void SetHingeLimits(int iConstraint, float fAngMin, float fAngMax, float fSoftness, float fBias, float fRelaxation)
{
	btHingeConstraint *hingeC = (btHingeConstraint*)g_dynamicsWorld->getConstraint(iConstraint);
	if (hingeC == NULL) return;

	hingeC->setLimit(btScalar(fAngMin), btScalar(fAngMax), btScalar(fSoftness), btScalar(fBias), btScalar(fRelaxation));
}

void SetHingeMotor( int iConstraint, float fSpeed, float fAngle, float fForce )
{
	btHingeConstraint *hingeC = (btHingeConstraint*)g_dynamicsWorld->getConstraint(iConstraint);
	if (hingeC == NULL) return;

	if ( fAngle != 0.0f )
	{
		hingeC->enableMotor( fSpeed != 0.0f  );
		hingeC->setMaxMotorImpulse( btScalar( fForce ) );
		hingeC->setMotorTarget( btScalar( fAngle ), btScalar( 0.1f ) );
	}
	else
	{
		hingeC->enableAngularMotor( fSpeed != 0.0f, btScalar( fSpeed ), btScalar( fForce ) );
	}
}

void SetSliderMotor( int iConstraint, bool OnOff, float fForce, float fVelocity )
{
	btSliderConstraint *sliderC = (btSliderConstraint*)g_dynamicsWorld->getConstraint(iConstraint);
	if (sliderC == NULL) return;

	sliderC->setPoweredLinMotor( OnOff );
	sliderC->setMaxLinMotorForce( btScalar( fForce ) );
	sliderC->setTargetLinMotorVelocity( btScalar( fVelocity ) );
}

float GetHingeAngle(int iConstraint)
{
	btHingeConstraint *hingeC = (btHingeConstraint*)g_dynamicsWorld->getConstraint(iConstraint);
	if (hingeC == NULL) return 0.0f;

	return (float)hingeC->getHingeAngle();
}

int CreateHingeDouble( int iObjectNumA, int iObjectNumB, float fXa, float fYa, float fZa,
	                                                     float fXb, float fYb, float fZb, 
	                                                     int iTypeA, int iTypeB, int noCollision )
{
	sObjectList* pPhyObjectA = ODEFindID( iObjectNumA );
	sObjectList* pPhyObjectB = ODEFindID( iObjectNumB );
	if ( pPhyObjectA == NULL || pPhyObjectB == NULL ) return -1;
	if ( pPhyObjectA->body == NULL || pPhyObjectB->body == NULL ) return -1;

	if ( iTypeA <= 0 || iTypeA > 3 ) return -1;
	if ( iTypeB <= 0 || iTypeB > 3 ) return -1;

	btVector3 vTypeA, vTypeB;
	if ( iTypeA == 1 ) vTypeA = btVector3( 1, 0, 0 );  // hinge on X axis
	if ( iTypeA == 2 ) vTypeA = btVector3( 0, 1, 0 );  // hinge on Y axis
	if ( iTypeA == 3 ) vTypeA = btVector3( 0, 0, 1 );  // hinge on Z axis
	if ( iTypeB == 1 ) vTypeB = btVector3( 1, 0, 0 );  // hinge on X axis
	if ( iTypeB == 2 ) vTypeB = btVector3( 0, 1, 0 );  // hinge on Y axis
	if ( iTypeB == 3 ) vTypeB = btVector3( 0, 0, 1 );  // hinge on Z axis

	btHingeConstraint *hingeC = new btHingeConstraint( *( pPhyObjectA->body ), 
		                                               *( pPhyObjectB->body ),
		                                               btVector3( fXa / gSc, fYa / gSc, fZa / gSc ),
		                                               btVector3( fXb / gSc, fYb / gSc, fZb / gSc ),
		                                               vTypeA, vTypeB,
		                                               true );
	g_dynamicsWorld->addConstraint( hingeC, noCollision == 1 );

	// need to find constraint index and return it to caller
	for (int i = 0; i < g_dynamicsWorld->getNumConstraints(); i++)
	{
		btTypedConstraint *tC = g_dynamicsWorld->getConstraint(i);
		if ( tC == hingeC ) return i;
	}
	return -1;
}

int CreateJointSingle( int iObjectNumber, float fX, float fY, float fZ )
{
	sObjectList* pPhyObject = ODEFindID( iObjectNumber );
	if ( pPhyObject == NULL ) return -1;
	if ( pPhyObject->body == NULL ) return -1;
	btPoint2PointConstraint *jointC = new btPoint2PointConstraint( *( pPhyObject->body ),
		                                                           btVector3( fX / gSc, fY / gSc, fZ / gSc ));
	g_dynamicsWorld->addConstraint( jointC );

	// need to find constraint index and return it to caller
	for (int i = 0; i < g_dynamicsWorld->getNumConstraints(); i++)
	{
		btTypedConstraint *tC = g_dynamicsWorld->getConstraint(i);
		if (tC == jointC) return i;
	}
	return -1;
}

int CreateJointDouble( int iObjectNumA, int iObjectNumB, float fXa, float fYa, float fZa, float fXb, float fYb, float fZb, int noCollision )
{
	sObjectList* pPhyObjectA = ODEFindID( iObjectNumA );
	sObjectList* pPhyObjectB = ODEFindID( iObjectNumB );
	if ( pPhyObjectA == NULL || pPhyObjectB == NULL ) return -1;
	if ( pPhyObjectA->body == NULL || pPhyObjectB->body == NULL ) return -1;

	btPoint2PointConstraint *jointC = new btPoint2PointConstraint( *( pPhyObjectA->body ),
		                                                           *( pPhyObjectB->body ),
		                                                           btVector3( fXa / gSc, fYa / gSc, fZa / gSc ),
		                                                           btVector3( fXb / gSc, fYb / gSc, fZb / gSc ) );
	g_dynamicsWorld->addConstraint( jointC, noCollision == 1 );

	// need to find constraint index and return it to caller
	for (int i = 0; i < g_dynamicsWorld->getNumConstraints(); i++)
	{
		btTypedConstraint *tC = g_dynamicsWorld->getConstraint(i);
		if ( tC == jointC ) return i;
	}
	return -1;
}

int CreateSliderDouble( int iObjectNumA, int iObjectNumB, float qXa, float qYa, float qZa, float qWa, float fXa, float fYa, float fZa,
	                                                      float qXb, float qYb, float qZb, float qWb, float fXb, float fYb, float fZb,
	                                                      bool useLinearReferenceFrameA )
{
	sObjectList* pPhyObjectA = ODEFindID(iObjectNumA);
	sObjectList* pPhyObjectB = ODEFindID(iObjectNumB);
	if (pPhyObjectA == NULL || pPhyObjectB == NULL) return -1;
	if (pPhyObjectA->body == NULL || pPhyObjectB->body == NULL) return -1;

	const btTransform& frameInA = btTransform( btQuaternion( qXa, qYa, qZa, qWa ), { fXa, fYa, fZa } );
	const btTransform& frameInB = btTransform( btQuaternion( qXb, qYb, qZb, qWb ), { fXb, fYb, fZb } );

	btSliderConstraint *sliderC = new btSliderConstraint( *(pPhyObjectA->body),
		                                                  *(pPhyObjectB->body), 
		                                                  frameInA, frameInB, 
		                                                  useLinearReferenceFrameA );

	g_dynamicsWorld->addConstraint( sliderC );

	// need to find constraint index and return it to caller
	for (int i = 0; i < g_dynamicsWorld->getNumConstraints(); i++)
	{
		btTypedConstraint *tC = g_dynamicsWorld->getConstraint(i);
		if ( tC == sliderC ) return i;
	}
	return -1;
}

void SetSliderLimits( int iConstraint, float fLowerLin, float fUpperLin, float fLowerAng, float fUpperAng )
{
	btSliderConstraint *sliderC = (btSliderConstraint*)g_dynamicsWorld->getConstraint(iConstraint);
	if (sliderC == NULL) return;

	sliderC->setLowerLinLimit( btScalar( fLowerLin ) );
	sliderC->setUpperLinLimit( btScalar( fUpperLin ) );
	sliderC->setLowerAngLimit( btScalar( fLowerAng ) );
	sliderC->setUpperAngLimit( btScalar( fUpperAng ) );
}

float GetSliderPosition( int iConstraint )
{
	btSliderConstraint *sliderC = (btSliderConstraint*)g_dynamicsWorld->getConstraint(iConstraint);
	if (sliderC == NULL) return 0.0f;

	return (float)sliderC->getLinearPos();
}

void AddStaticObjectBox ( int iObjectNumber , int iStaticBoxObject, int iMaterialRefIndex )
{
	// get reference object
	sObject* pObject = GetObjectData ( iStaticBoxObject );
	if ( pObject==NULL ) return;

	float fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot;
	GetSizePositionRotation(pObject, fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot, true);
	
	// real object
	sObject* pRealObject = pObject;
	if ( pObject->pInstanceOfObject ) pRealObject = pObject->pInstanceOfObject;
	
	// ensure very small objects are not allowed below this size
	if ( fXSize < 6 ) fXSize = 6;
	if ( fYSize < 6 ) fYSize = 6;
	if ( fZSize < 6 ) fZSize = 6;

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// create a bullet shape
	btCollisionShape* boxShape = new btBoxShape(btVector3(btScalar(fXSize/2.0f),btScalar(fYSize/2.0f),btScalar(fZSize/2.0f)));

	// assign material ref index to boxShape (for later use when detecting physics material)
	boxShape->setUserPointer ( (void*)iMaterialRefIndex );

	// add shape to collision list so can release later, they can be "re-used" amongst bodies!!
	g_collisionShapes.push_back(boxShape);

	// set position transform for physics object
	btTransform startTransform;
	
	fillTransform( startTransform, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot );

	pImporterCompoundShape->addChildShape( startTransform, boxShape );
}

void CreateSphere ( int iObjectNumber, int isDynamic, float fWeight, float fFriction, float fRestitution )
{
	// defaults
	if ( fWeight==-1 ) fWeight = 100.0f;
	if ( fFriction==-1 ) fFriction = 90.0f;
	if ( fRestitution==-1 ) fRestitution = 0.0f;

	// get reference object
	sObject* pObject = GetObjectData ( iObjectNumber );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fRadius = pObject->collision.fRadius * pObject->position.vecScale.x;

	float fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot;
	GetSizePositionRotation(pObject, fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot, true);
	
	// just before shape creation, reduce scale of size and position
	fRadius /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// volume calculation
	float fRadiusCubed = fRadius*fRadius*fRadius;
	float fVolume = ((4.0f/3.0f) * fRadiusCubed) * 3.14159f;

	// modify with weight modifier
	fVolume *= (fWeight/100.0f);

	// create a dynamic rigidbody
	btCollisionShape* colShape = new btSphereShape(btScalar(fRadius));
	g_collisionShapes.push_back(colShape);

	// mass/volume calculation
	if ( isDynamic==0 )
	{ 
		fVolume = 0;
	}
	else
	{
		// max
		if (fVolume > maxDynamicVolume) fVolume = maxDynamicVolume;

		// also handle min - ensure MASS not SO SMALL THINGS FLY MILLIONS OF MILES!
		if (fVolume < 0.1f) fVolume = 0.1f;
	}
	btScalar mass(fVolume/gMs);
	btVector3 localInertia(0,0,0);
	if ( isDynamic==1 ) colShape->calculateLocalInertia(mass,localInertia);

	// physics object pos and rotation
	btTransform startTransform;
	fillTransform( startTransform, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot );
	
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
	rbInfo.m_friction = 1.0f;
	rbInfo.m_restitution = fRestitution/100.0f;
	btRigidBody* body = new btRigidBody(rbInfo);

	// pass material index as usrptr
	sObject* pRealObject = pObject;
	if ( pRealObject->pInstanceOfObject ) pRealObject = pRealObject->pInstanceOfObject;
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	body->setUserPointer((void*)iArbValue);

	// add physics object to system
	short sColliderType = COL_OBJECT;
	if (isDynamic == 1) sColliderType = COL_OBJECT_DYNAMIC;
	short sCollidesWith = COL_OBJECT | COL_OBJECT_DYNAMIC | COL_CAPSULECHAR | COL_TERRAIN;
	g_dynamicsWorld->addRigidBody ( body, sColliderType, sCollidesWith);

	// boyancy mode
	bool bBouyant = false;
	if ( isDynamic==1 ) 
		bBouyant = true; 
	else
		pObject = NULL;

	// add physics object details to pbject list
	ODEAddObject ( iObjectNumber, pObject, body, isDynamic, myMotionState, false, bBouyant, NULL, NULL, 1, 1, fFriction, mass );
}

void CreateCylinder ( int iObjectNumber, int isDynamic, float fXPos, float fYPos, float fZPos, float fXSize, float fYSize, float fZSize, float fXRot, float fYRot, float fZRot, float fWeight, float fFriction, float fRestitution)
{
	// defaults
	if ( fWeight==-1 ) fWeight = 100.0f;
	if ( fFriction==-1 ) fFriction = 90.0f;
	if ( fRestitution==-1 ) fRestitution = 0.0f;

	// get reference object
	sObject* pObject = GetObjectData ( iObjectNumber );
	if ( pObject==NULL ) return;

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;	

	// volume calculation
	float fVolume = 0;
	fVolume = fXSize * fYSize * fZSize;

	// modify with weight modifier
	fVolume *= ( fWeight / 100.0f );

	// create a bullet shape
	btCollisionShape* cylinderShape = new btCylinderShape(btVector3( btScalar( fXSize / 2.0f ), 
		                                                             btScalar( fYSize / 2.0f ), 
		                                                             btScalar( fZSize / 2.0f ) ) );

	// add shape to collision list so can release later, they can be "re-used" amongst bodies!!
	g_collisionShapes.push_back( cylinderShape );

	// mass/volume calculation
	if ( isDynamic==0 ) 
	{
		fVolume = 0;
	}
	else
	{
		// max
		if (fVolume > maxDynamicVolume) fVolume = maxDynamicVolume;

		// also handle min - ensure MASS not SO SMALL THINGS FLY MILLIONS OF MILES!
		if (fVolume < 0.1f) fVolume = 0.1f;
	}
	btScalar mass(fVolume/gMs);
	btVector3 localInertia(0,0,0);
	if ( isDynamic == 1 ) cylinderShape->calculateLocalInertia(mass,localInertia);

	// set position transform for physics object
	btTransform startTransform;
	fillTransform( startTransform, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot );

	// motionstate provides interpolation capabilities and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState( startTransform );
	btRigidBody::btRigidBodyConstructionInfo rbInfo( mass, myMotionState, cylinderShape, localInertia );
	rbInfo.m_friction = 1.0f;
	rbInfo.m_restitution = fRestitution / 100.0f;
	btRigidBody* body = new btRigidBody( rbInfo );

	// pass material index as usrptr
	sObject* pRealObject = pObject;
	if ( pRealObject->pInstanceOfObject ) pRealObject = pRealObject->pInstanceOfObject;
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	body->setUserPointer((void*)iArbValue);

	// add physics object to system
	short sColliderType = COL_OBJECT;
	if (isDynamic == 1) sColliderType = COL_OBJECT_DYNAMIC;
	short sCollidesWith = COL_OBJECT | COL_OBJECT_DYNAMIC | COL_CAPSULECHAR | COL_TERRAIN;
	g_dynamicsWorld->addRigidBody ( body, sColliderType, sCollidesWith);

	// boyancy mode
	bool bBouyant = false;
	if ( isDynamic==1 ) 
		bBouyant = true; 
	else
		pObject = NULL;

	// add physics object details to pbject list
	ODEAddObject ( iObjectNumber, pObject, body, isDynamic, myMotionState, false, bBouyant, NULL, NULL, 1, 1, fFriction, mass );
}

void CreateCylinder ( int iObjectNumber, int isDynamic, float fWeight, float fFriction, float fRestitution )
{
	// get reference object
	sObject* pObject = GetObjectData ( iObjectNumber );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot;
	GetSizePositionRotation(pObject, fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot, true);

	CreateCylinder(iObjectNumber, isDynamic, fXPos, fYPos, fZPos, fXSize, fYSize, fZSize, fXRot, fYRot, fZRot, fWeight, fFriction, fRestitution);
}

void SetMeshFilename (LPSTR pFilename)
{
	strcpy (g_ProvidedMeshFilenameForBNVHSaving, pFilename);
}

void SetOBJLoadingFilename (LPSTR pFilename)
{
	strcpy (g_ProvidedMeshFilenameForOBJLoading, pFilename);
}

void CreateMesh ( int iObjectNumber, int isDynamic, int iLimbNumber, int iTerrainMesh, int iCollisionScaling, int iHullReduction, float fWeight, float fFriction, float fRestitution )
{
	// get reference object
	sObject* pObject = GetObjectData ( iObjectNumber );
	if ( pObject==NULL ) return;

	// real object for mesh data
	sObject* pRealObject = pObject;
	if ( pObject->pInstanceOfObject ) pRealObject = pObject->pInstanceOfObject;                 

	// scaling modifier
	float fScalingFactor = (float)iCollisionScaling / 100.0f;

	// obtain sizes, position and rotation
	float fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot;
	GetSizePositionRotation( pObject, fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot, false, fScalingFactor );

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// LB: Account for any offset from the model importer
	GGMATRIX matModelOffset;
	GGMatrixIdentity(&matModelOffset);
	GGMATRIX matIdentity;
	GGMatrixIdentity(&matIdentity);
	if (pObject->pFrame)
	{
		// work out the inverse of the model frame zero transform - we need to remove the offset from the raw mesh data further down
		float fDet = 0.0f;
		matModelOffset = pObject->pFrame->matCombined;
		GGMatrixInverse(&matModelOffset, &fDet, &matModelOffset);
	}

	// volume calculation
	float fVolume = 0;
	fVolume = fXSize * fYSize * fZSize;

	// create aabb boundboxes
	btVector3 aabbMin(pObject->collision.vecMin.x / gSc, pObject->collision.vecMin.y / gSc, pObject->collision.vecMin.z / gSc),
			  aabbMax(pObject->collision.vecMax.x / gSc, pObject->collision.vecMax.y / gSc,	pObject->collision.vecMax.z / gSc);

	// creation part
	bool useQuantizedAabbCompression = true;
	btIndexedMesh btMesh;
	btTriangleIndexVertexArray* m_indexVertexArrays;
	btAlignedObjectArray<btVector3> ch_vertexBuffer;
	bool bDWORDSizeIndices = false;
	float* gVertices = NULL;
	LPVOID gIndices = NULL;

	// defaults to passed in object for collision mesh data
	sObject* pObjectUsedToExtractPhysicsMesh = pRealObject;

	// determine if OBJ file carries collision mesh
	bool bObjectUsesOBJCollisionMesh = false;
	int iObjectUsedToExtractPhysicsMesh = 0;
	if (strlen(g_ProvidedMeshFilenameForOBJLoading) > 4)
	{
		char pNoFPE[MAX_PATH];
		strcpy(pNoFPE, g_ProvidedMeshFilenameForOBJLoading);
		pNoFPE[strlen(pNoFPE) - 4] = 0;
		char pOBJCollisionMesh[MAX_PATH];
		sprintf(pOBJCollisionMesh, "%s.obj", pNoFPE);
		GG_GetRealPath(pOBJCollisionMesh, 0);
		if (FileExist(pOBJCollisionMesh) == 0)
		{
			sprintf(pOBJCollisionMesh, "%s_COL.obj", pNoFPE);
			GG_GetRealPath(pOBJCollisionMesh, 0);
		}
		if (FileExist(pOBJCollisionMesh) == 1)
		{
			iObjectUsedToExtractPhysicsMesh = 69501;// g.temp2objectoffset;
			if (ObjectExist(iObjectUsedToExtractPhysicsMesh) == 1) DeleteObject(iObjectUsedToExtractPhysicsMesh);
			LoadObject (pOBJCollisionMesh, iObjectUsedToExtractPhysicsMesh);
			RotateObject(iObjectUsedToExtractPhysicsMesh, ObjectAngleX(iObjectNumber), ObjectAngleY(iObjectNumber), ObjectAngleZ(iObjectNumber));
			ScaleObject(iObjectUsedToExtractPhysicsMesh, ObjectScaleX(iObjectNumber), ObjectScaleY(iObjectNumber), ObjectScaleZ(iObjectNumber));
			pObjectUsedToExtractPhysicsMesh = GetObjectData(iObjectUsedToExtractPhysicsMesh);
			bObjectUsesOBJCollisionMesh = true;
			if(pObjectUsedToExtractPhysicsMesh)
			{
				//PE: New loaded .obj collision mesh dont match original frame zero transform, so set to MatrixIdentity
				GGMatrixIdentity(&matModelOffset);
			}
		}
	}

	// determine if using LOD, if so, choose lowest
	extern bool bPhysicsLowestLOD;
	bool bGotLowestLOD = false;

	if (bPhysicsLowestLOD && bObjectUsesOBJCollisionMesh == false)
	{
		for (int iFrame = 0; iFrame < pRealObject->iFrameCount; iFrame++)
		{
			sFrame* pFrame = pRealObject->ppFrameList[iFrame];
			if (pFrame)
			{
				sMesh* pMesh = pFrame->pMesh;
				if (pMesh)
				{
					if (pMesh->dwIndexCountLOD3 > 0 && pMesh->pIndicesLOD3)
					{
						bGotLowestLOD = true;
						break;
					}
					else if (pMesh->dwIndexCountLOD2 > 0 && pMesh->pIndicesLOD2)
					{
						bGotLowestLOD = true;
						break;
					}
					else if (pMesh->dwIndexCountLOD1 > 0 && pMesh->pIndicesLOD1)
					{
						bGotLowestLOD = true;
						break;
					}
				}
			}
		}
	}

	int iChooseLowLOD = -1;
	if (bObjectUsesOBJCollisionMesh == false && !bGotLowestLOD)
	{
		// if no OBJ, rely on internal mesh stored in object (prefer frame named 'collision_mesh')
		int iBestLowLOD = 0;
		for (int iFrame = 0; iFrame < pRealObject->iFrameCount; iFrame++)
		{
			sFrame* pFrame = pRealObject->ppFrameList[iFrame];
			if (pFrame)
			{
				if (pFrame->pMesh)
				{
					if (stricmp(pFrame->szName, "lod_0") == NULL && iBestLowLOD < 1) { iChooseLowLOD = iFrame; iBestLowLOD = 1; }
					if (stricmp(pFrame->szName, "lod_1") == NULL && iBestLowLOD < 2) { iChooseLowLOD = iFrame; iBestLowLOD = 2; }
					if (stricmp(pFrame->szName, "lod_2") == NULL && iBestLowLOD < 3) { iChooseLowLOD = iFrame; iBestLowLOD = 3; }
					if (stricmp(pFrame->szName, "collision_mesh") == NULL && iBestLowLOD < 4) { iChooseLowLOD = iFrame; iBestLowLOD = 4; }
				}
			}
		}
	}

	// do I need to create new data, or can I use mesh data directly in mesh builder? (save memory)
	int totalVerts = 0, totalIndices = 0;
	for (int iFrame = 0; iFrame < pObjectUsedToExtractPhysicsMesh->iFrameCount; iFrame++)
	{
		if (iChooseLowLOD == -1 || iChooseLowLOD == iFrame)
		{
			sFrame* pFrame = pObjectUsedToExtractPhysicsMesh->ppFrameList[iFrame];
			if (pFrame)
			{
				sMesh* pMesh = pFrame->pMesh;
				if (pMesh)
				{
					if (pMesh->Collision.dwArbitaryValue != 99999) // mesh excluded for collision purposes
					{
						if (!bObjectUsesOBJCollisionMesh && bPhysicsLowestLOD)
						{
							if (pMesh->dwIndexCountLOD3 > 0 && pMesh->pIndicesLOD3)
							{
								totalVerts = totalVerts + pMesh->dwVertexCount;
								totalIndices = totalIndices + pMesh->dwIndexCountLOD3;
							}
							else if (pMesh->dwIndexCountLOD2 > 0 && pMesh->pIndicesLOD2)
							{
								totalVerts = totalVerts + pMesh->dwVertexCount;
								totalIndices = totalIndices + pMesh->dwIndexCountLOD2;
							}
							else if (pMesh->dwIndexCountLOD1 > 0 && pMesh->pIndicesLOD1)
							{
								totalVerts = totalVerts + pMesh->dwVertexCount;
								totalIndices = totalIndices + pMesh->dwIndexCountLOD1;
							}
							else
							{
								totalVerts = totalVerts + pMesh->dwVertexCount;
								totalIndices = totalIndices + pMesh->dwIndexCount;
							}
						}
						else
						{
							totalVerts = totalVerts + pMesh->dwVertexCount;
							totalIndices = totalIndices + pMesh->dwIndexCount;
						}
					}
				}
			}
		}
	}

	// if more vertices than a WORD can index, use 32bit indices
	if (totalVerts > 0xFFFF)
	{
		bDWORDSizeIndices = true;
	}
	gVertices = new float[totalVerts * 4];
	memset(gVertices, 0, sizeof(float) * totalVerts * 4);

	//PE: We need to convert some triangle lists to indices.
	std::vector<DWORD> dwIndices;

	//LB: get root node rotation to ensure mesh is right way up
	GGMATRIX matRootRotOnly;
	matRootRotOnly = pObjectUsedToExtractPhysicsMesh->ppFrameList[0]->matCombined;
	matRootRotOnly._41 = 0;
	matRootRotOnly._42 = 0;
	matRootRotOnly._43 = 0;

	//PE: This system do not work with Mesh->dwIndexCount == 0 triangle lists they are ignored.
	//PE: use dwIndices and convert triangle lists to indices based.
	
	// for convex hull, collect for vert only initially
	int vcount = 0, icount = 0;
	for (int iFrame = 0; iFrame < pObjectUsedToExtractPhysicsMesh->iFrameCount; iFrame++)
	{
		if (iChooseLowLOD == -1 || iChooseLowLOD == iFrame)
		{
			sFrame* pFrame = pObjectUsedToExtractPhysicsMesh->ppFrameList[iFrame];
			if (pFrame)
			{
				sMesh* pMesh = pFrame->pMesh;
				if (pMesh)
				{
					if (pMesh->Collision.dwArbitaryValue != 99999) // mesh excluded for collision purposes
					{
						// transform matrices for frame/mesh
						GGMATRIX matFinal;
						matFinal = pFrame->matCombined;

						// LB: reverse ANY effect of the frame zero transform, then restore the rotation required
						// this removes the offset portion from the mesh data, and will be returned with moving the physics object directly
						//PE: Using matModelOffset * matFinal on frame 0 invert changes and return a Identity matix, so they even themself out.
						//PE: Other frames with Identity matix do not need this.
						if (matFinal != matIdentity)
						{
							GGMatrixMultiply(&matFinal, &matFinal, &matModelOffset);
						}
						GGMatrixMultiply (&matFinal, &matFinal, &matRootRotOnly);

						// Do not scale raw geometry, we use setlocalscaling on shape instead (so can save and load raw convex hulls,etc)
						GGMATRIX matScale;
						GGMatrixScaling (&matScale, (1.0f * fScalingFactor) / gSc, (1.0f * fScalingFactor) / gSc, (1.0f * fScalingFactor) / gSc);
						GGMatrixMultiply (&matFinal, &matFinal, &matScale);

						// copy vertex to physics mesh buffer
						float* pVertData = (float*)pMesh->pVertexData;
						DWORD dwSkip = pMesh->dwFVFSize / sizeof(float);
						for (int v = 0; v < (int)pMesh->dwVertexCount; v++)
						{
							GGVECTOR3 vec = GGVECTOR3(*(pVertData + 0), *(pVertData + 1), *(pVertData + 2));
							GGVec3TransformCoord (&vec, &vec, &matFinal);
							*(gVertices + ((vcount + v) * 4) + 0) = vec.x;
							*(gVertices + ((vcount + v) * 4) + 1) = vec.y;
							*(gVertices + ((vcount + v) * 4) + 2) = vec.z;
							*(gVertices + ((vcount + v) * 4) + 3) = 1.0f;
							ch_vertexBuffer.push_back(btVector3(vec.x, vec.y, vec.z));
							pVertData += dwSkip;
						}

						WORD* pIndexData = (WORD*)pMesh->pIndices;
						int iIndexCount = pMesh->dwIndexCount;
						if (!bObjectUsesOBJCollisionMesh && bPhysicsLowestLOD)
						{
							if (pMesh->dwIndexCountLOD3 > 0 && pMesh->pIndicesLOD3)
							{
								pIndexData = (WORD*)pMesh->pIndicesLOD3;
								iIndexCount = pMesh->dwIndexCountLOD3;
							}
							else if (pMesh->dwIndexCountLOD2 > 0 && pMesh->pIndicesLOD2)
							{
								pIndexData = (WORD*)pMesh->pIndicesLOD2;
								iIndexCount = pMesh->dwIndexCountLOD2;
							}
							else if (pMesh->dwIndexCountLOD1 > 0 && pMesh->pIndicesLOD1)
							{
								pIndexData = (WORD*)pMesh->pIndicesLOD1;
								iIndexCount = pMesh->dwIndexCountLOD1;
							}
						}
						if (iIndexCount == 0)
						{
							//PE: Convert triangle lists.
							for (int v = 0; v < (int)pMesh->dwVertexCount; v++)
							{
								dwIndices.push_back(vcount + v);
								iIndexCount++;
							}
						}
						else
						{
							for (int i = 0; i < (int)iIndexCount; i++)
							{
								dwIndices.push_back(vcount + (*pIndexData));
								//if (bDWORDSizeIndices == true)
								//	((DWORD*)gIndices)[icount + i] = vcount + (*pIndexData);
								//else
								//	((WORD*)gIndices)[icount + i] = vcount + (*pIndexData);
								pIndexData++;
							}
						}
						vcount += pMesh->dwVertexCount;
						icount += iIndexCount;
					}
				}
			}
		}
	}
	
	totalIndices = dwIndices.size();

	if (bDWORDSizeIndices == true)
	{
		gIndices = (LPVOID)new DWORD[totalIndices];
		memset(gIndices, 0, sizeof(DWORD) * totalIndices);
	}
	else
	{
		gIndices = (LPVOID)new WORD[totalIndices];
		memset(gIndices, 0, sizeof(WORD) * totalIndices);
	}

	//PE: Copy from dwIndices to gIndices.
	for( int i = 0; i < dwIndices.size(); i++)
	{
		if (bDWORDSizeIndices == true)
			((DWORD*)gIndices)[i] = dwIndices[i];
		else
			((WORD*)gIndices)[i] = (WORD) dwIndices[i];
	}
	dwIndices.clear();

	// if turns out this is index buffer based, ch_vertexBuffer data not enough
	if (totalIndices > 0)
	{
		ch_vertexBuffer.clear();
		for (int i = 0; i < totalIndices; i += 3)
		{
			int iFace0, iFace1, iFace2;
			if (bDWORDSizeIndices == true)
			{
				iFace0 = ((DWORD*)gIndices)[i + 0];
				iFace1 = ((DWORD*)gIndices)[i + 1];
				iFace2 = ((DWORD*)gIndices)[i + 2];
			}
			else
			{
				iFace0 = ((WORD*)gIndices)[i + 0];
				iFace1 = ((WORD*)gIndices)[i + 1];
				iFace2 = ((WORD*)gIndices)[i + 2];
			}
			GGVECTOR3 vec;
			vec = GGVECTOR3(*(gVertices + ((iFace0) * 4) + 0), *(gVertices + ((iFace0) * 4) + 1), *(gVertices + ((iFace0) * 4) + 2));
			ch_vertexBuffer.push_back(btVector3(vec.x, vec.y, vec.z));
			vec = GGVECTOR3(*(gVertices + ((iFace1) * 4) + 0), *(gVertices + ((iFace1) * 4) + 1), *(gVertices + ((iFace1) * 4) + 2));
			ch_vertexBuffer.push_back(btVector3(vec.x, vec.y, vec.z));
			vec = GGVECTOR3(*(gVertices + ((iFace2) * 4) + 0), *(gVertices + ((iFace2) * 4) + 1), *(gVertices + ((iFace2) * 4) + 2));
			ch_vertexBuffer.push_back(btVector3(vec.x, vec.y, vec.z));
		}
	}

	// index based mesh or verts only
	if (totalIndices == 0)
	{
		// verts only, so CREATE index list
		totalIndices = totalVerts;
		gIndices = (LPVOID)new DWORD[totalIndices];
		memset (gIndices, 0, sizeof(DWORD)*totalIndices);
		for (int n = 0; n < totalIndices; n++) ((DWORD*)gIndices)[n] = n;
		bDWORDSizeIndices = true;
	}

	// index based mesh
	const int totalTriangles = totalIndices / 3;
	int vertStride = sizeof(float) * 4;
	btMesh.m_numVertices = totalVerts;
	btMesh.m_vertexBase = (const unsigned char*)gVertices;
	btMesh.m_vertexStride = vertStride;
	btMesh.m_numTriangles = totalTriangles;
	btMesh.m_triangleIndexBase = (const unsigned char*)gIndices;

	// free any temp object (used to load OBJ collision data in)
	if (bObjectUsesOBJCollisionMesh==true)
	{
		if (ObjectExist(iObjectUsedToExtractPhysicsMesh) == 1) DeleteObject(iObjectUsedToExtractPhysicsMesh);
		bObjectUsesOBJCollisionMesh = false;
		iObjectUsedToExtractPhysicsMesh = 0;
	}

	// possible shapes
	int iShapeUsed = 0;
	btBvhTriangleMeshShape* trimeshShape;
	btConvexHullShape* simplifiedConvexShape;
	btCompoundShape* compoundShape;

	// convex hull optimization if flagged
	// PE: Convex hull - We keep gVertices,gIndices as mem1/2 , gVertices is used for height data directly on terrain.
	// PE: so we later can test terrain and convex hull. remove (iTerrainMesh != 1)
	int ch_size = ch_vertexBuffer.size();
	if( iHullReduction > 0 && iTerrainMesh != 1 && ch_size > 6 )
	{
		bool bLoadedAndCreatedOkay = false;
		if (strlen(g_ProvidedMeshFilenameForBNVHSaving) > 0)
		{
			// try to load in convex hull/hull decomp shapes
			btBulletWorldImporter* importer = new btBulletWorldImporter();
			char pFindWhereBULLETFIleIsLocated[MAX_PATH];
			strcpy(pFindWhereBULLETFIleIsLocated, g_ProvidedMeshFilenameForBNVHSaving);
			GG_GetRealPath(pFindWhereBULLETFIleIsLocated, 0);
			if (importer->loadFile(pFindWhereBULLETFIleIsLocated))
			{
				int numShapes = importer->getNumCollisionShapes();
				if (numShapes)
				{
					compoundShape = (btCompoundShape*)importer->getCollisionShapeByIndex(0);
					bLoadedAndCreatedOkay = true;
					iShapeUsed = 2;
				}
				g_importers.push_back(importer); //PE: Free mem when ending ODE.
			}
		}
		if (bLoadedAndCreatedOkay == false)
		{
			// iHullReduction : 1-single hull, 2-hull decomposition (multiple hulls)
			if (iHullReduction == 2)
			{
				// fill in temp points(verts) and triangles(indices)
				std::vector<float> points;
				std::vector<int> triangles;
				points.clear();
				triangles.clear();
				for (int v = 0; v < ch_vertexBuffer.size(); v += 3)
				{
					points.push_back(ch_vertexBuffer[v + 0].getX());
					points.push_back(ch_vertexBuffer[v + 0].getY());
					points.push_back(ch_vertexBuffer[v + 0].getZ());
					points.push_back(ch_vertexBuffer[v + 1].getX());
					points.push_back(ch_vertexBuffer[v + 1].getY());
					points.push_back(ch_vertexBuffer[v + 1].getZ());
					points.push_back(ch_vertexBuffer[v + 2].getX());
					points.push_back(ch_vertexBuffer[v + 2].getY());
					points.push_back(ch_vertexBuffer[v + 2].getZ());
					triangles.push_back(v + 0);
					triangles.push_back(v + 1);
					triangles.push_back(v + 2);
				}

				// process 'ch_vertexBuffer' and create multiple convex hulls
				VHACD::IVHACD::Parameters paramsVHACD;
				paramsVHACD.Init();
				paramsVHACD.m_maxNumVerticesPerCH = 1024;
				VHACD::IVHACD* interfaceVHACD = VHACD::CreateVHACD();
				bool res = interfaceVHACD->Compute(&points[0], 3, (unsigned int)points.size() / 3, &triangles[0], 3, (unsigned int)triangles.size() / 3, paramsVHACD);
				if (res)
				{
					unsigned int nConvexHulls = interfaceVHACD->GetNConvexHulls();
					VHACD::IVHACD::ConvexHull ch;
					compoundShape = new btCompoundShape();
					for (unsigned int p = 0; p < nConvexHulls; ++p)
					{
						btVector3 vert;
						interfaceVHACD->GetConvexHull(p, ch);
						ch_vertexBuffer.clear();
						for (int tri = 0; tri < (int)ch.m_nTriangles * 3; tri += 3)
						{
							int face0 = ch.m_triangles[tri + 0];
							int face1 = ch.m_triangles[tri + 1];
							int face2 = ch.m_triangles[tri + 2];
							vert = btVector3(btScalar(ch.m_points[(face0 * 3) + 0]), btScalar(ch.m_points[(face0 * 3) + 1]), btScalar(ch.m_points[(face0 * 3) + 2]));
							ch_vertexBuffer.push_back(vert);
							vert = btVector3(btScalar(ch.m_points[(face1 * 3) + 0]), btScalar(ch.m_points[(face1 * 3) + 1]), btScalar(ch.m_points[(face1 * 3) + 2]));
							ch_vertexBuffer.push_back(vert);
							vert = btVector3(btScalar(ch.m_points[(face2 * 3) + 0]), btScalar(ch.m_points[(face2 * 3) + 1]), btScalar(ch.m_points[(face2 * 3) + 2]));
							ch_vertexBuffer.push_back(vert);
						}

						simplifiedConvexShape = new btConvexHullShape(&(ch_vertexBuffer[0].getX()), ch_vertexBuffer.size());
						btTransform localTransform;
						fillTransform(localTransform, 0, 0, 0, 0, 0, 0);
						compoundShape->addChildShape(localTransform, simplifiedConvexShape);
						g_collisionShapes.push_back(simplifiedConvexShape);
					}
					g_collisionShapes.push_back(compoundShape);
					iShapeUsed = 2;

					// save out all shapes of hull decomp so can load in quickly in future
					if (strlen(g_ProvidedMeshFilenameForBNVHSaving) > 0)
					{
						char pShapeName[256];
						sprintf(pShapeName, "objectshape");
						btVector3 aabbMin(-1000, -1000, -1000), aabbMax(1000, 1000, 1000);
						int maxSerializeBufferSize = 1024 * 1024 * 5;
						btDefaultSerializer* serializer = new btDefaultSerializer(maxSerializeBufferSize);
						serializer->startSerialization();
						serializer->registerNameForPointer(compoundShape, pShapeName);
						compoundShape->serializeSingleShape(serializer);
						serializer->finishSerialization();
						char pWriteOutBULLETFile[MAX_PATH];
						strcpy(pWriteOutBULLETFile, g_ProvidedMeshFilenameForBNVHSaving);
						GG_GetRealPath(pWriteOutBULLETFile, 1);
						FILE* f2 = fopen(pWriteOutBULLETFile, "wb");
						fwrite(serializer->getBufferPointer(), serializer->getCurrentBufferSize(), 1, f2);
						fclose(f2);

						//PE: We need to free the mem used here.
						serializer->~btDefaultSerializer();
					}
				}

				// free resources
				interfaceVHACD->Clean();
				interfaceVHACD->Release();
				points.clear();
				triangles.clear();
			}
			if (iHullReduction == 1)
			{
				//PE: Add support for convex hull (collisionmode = 9) for faster physics.
				btConvexHullShape* ch_collShape = new btConvexHullShape(&(ch_vertexBuffer[0].getX()), ch_vertexBuffer.size());
				ch_collShape->setMargin(0.00);
				btShapeHull* hull = new btShapeHull(ch_collShape);
				btScalar margin = ch_collShape->getMargin();
				hull->buildHull(margin);
				simplifiedConvexShape = new btConvexHullShape(&(hull->getVertexPointer()[0].getX()), hull->numVertices());
				SAFE_DELETE(ch_collShape);

				// only use CH if large enough
				int hull_size = hull->numVertices();
				if (hull_size < 6)
				{
					m_indexVertexArrays = new btTriangleIndexVertexArray();
					if (bDWORDSizeIndices == true)
					{
						btMesh.m_indexType = PHY_INTEGER;
						btMesh.m_triangleIndexStride = 3 * sizeof(DWORD);
						m_indexVertexArrays->addIndexedMesh(btMesh, PHY_INTEGER);
					}
					else
					{
						btMesh.m_indexType = PHY_SHORT;
						btMesh.m_triangleIndexStride = 3 * sizeof(WORD);
						m_indexVertexArrays->addIndexedMesh(btMesh, PHY_SHORT);
					}
					g_indexvertexarrays.push_back(m_indexVertexArrays);

					trimeshShape = new btBvhTriangleMeshShape(m_indexVertexArrays, useQuantizedAabbCompression, aabbMin, aabbMax, false);
					trimeshShape->buildOptimizedBvh();
					g_collisionShapes.push_back(trimeshShape);
				}
				else
				{
					// we will use convex hull instead below
					g_collisionShapes.push_back(simplifiedConvexShape);
					iShapeUsed = 1;
				}
			}
		}
	}
	else 
	{
		// not HullReduction
		m_indexVertexArrays = new btTriangleIndexVertexArray();
		if (bDWORDSizeIndices == true)
		{
			btMesh.m_indexType = PHY_INTEGER;
			btMesh.m_triangleIndexStride = 3 * sizeof(DWORD);
			m_indexVertexArrays->addIndexedMesh(btMesh, PHY_INTEGER);
		}
		else
		{
			btMesh.m_indexType = PHY_SHORT;
			btMesh.m_triangleIndexStride = 3 * sizeof(WORD);
			m_indexVertexArrays->addIndexedMesh(btMesh, PHY_SHORT);
		}
		g_indexvertexarrays.push_back(m_indexVertexArrays);

		if (strlen(g_ProvidedMeshFilenameForBNVHSaving) == 0)
		{
			// generate regular trimeshshape chunk as normal
			trimeshShape = new btBvhTriangleMeshShape(m_indexVertexArrays, useQuantizedAabbCompression, aabbMin, aabbMax, false);
			trimeshShape->buildOptimizedBvh();
		}
		else
		{
			// see if terrain obj file exists for this chunk?
			//PE: Found it , we was leaking around 400-600mb on each test game.
			//PE: Problem was all the btBulletWorldImporter was never free'ed :)
			//PE: Now keep a list and free everything when physics end's.
			btBulletWorldImporter* importer = new btBulletWorldImporter();
			char pBULLETFileForReading[MAX_PATH];
			strcpy(pBULLETFileForReading, g_ProvidedMeshFilenameForBNVHSaving);
			GG_GetRealPath(pBULLETFileForReading, 0);
			if (importer->loadFile(pBULLETFileForReading))
			{
				// this just saves and loads BVH, half a second quicker than above!
				int numBvh = importer->getNumBvhs();
				if (numBvh)
				{
					btOptimizedBvh* bvh = importer->getBvhByIndex(0);
					btVector3 aabbMin(-1000, -1000, -1000), aabbMax(1000, 1000, 1000);
					trimeshShape = new btBvhTriangleMeshShape(m_indexVertexArrays, useQuantizedAabbCompression, aabbMin, aabbMax, false);
					trimeshShape->setOptimizedBvh(bvh);
				}
				g_importers.push_back(importer); //PE: Free mem when ending ODE.
			}
			else
			{
				// generate trimeshshape for BVH chunk
				trimeshShape = new btBvhTriangleMeshShape(m_indexVertexArrays, useQuantizedAabbCompression, aabbMin, aabbMax, false);
				trimeshShape->buildOptimizedBvh();

				// save out BVH shape so can load in quickly for same terrain physics
				char pTerrainBVHChunkName[256];
				sprintf(pTerrainBVHChunkName, "terrainobj%d", pObject->dwObjectNumber);
				btVector3 aabbMin(-1000, -1000, -1000), aabbMax(1000, 1000, 1000);
				int maxSerializeBufferSize = 1024 * 1024 * 5;
				btDefaultSerializer* serializer = new btDefaultSerializer(maxSerializeBufferSize);
				serializer->startSerialization();
				serializer->registerNameForPointer(trimeshShape, pTerrainBVHChunkName);
				// 3.5 seconds for terrain
				trimeshShape->serializeSingleBvh(serializer);
				serializer->finishSerialization();
				char pWriteOutBULLETFile[MAX_PATH];
				strcpy(pWriteOutBULLETFile, g_ProvidedMeshFilenameForBNVHSaving);
				GG_GetRealPath(pWriteOutBULLETFile, 1);
				FILE* f2 = fopen(pWriteOutBULLETFile, "wb");
				fwrite(serializer->getBufferPointer(), serializer->getCurrentBufferSize(), 1, f2);
				fclose(f2);
				//PE: We need to free the mem used here.
				serializer->~btDefaultSerializer();	
			}
		}
		g_collisionShapes.push_back(trimeshShape);
	}

	// clear usage now hull constructed (if any)
	ch_vertexBuffer.clear();

	// mass/volume calculation
	if ( isDynamic==0 ) fVolume = 0;
	btScalar mass(fVolume/gMs);
	btVector3 localInertia( 0, 0, 0 );
	if (isDynamic == 1)
	{
		switch (iShapeUsed)
		{
			case 0: trimeshShape->calculateLocalInertia(mass, localInertia); break;
			case 1: simplifiedConvexShape->calculateLocalInertia(mass, localInertia); break;
			case 2: compoundShape->calculateLocalInertia(mass, localInertia); break;
		}
	}

	// physics object pos and rotation
	btTransform startTransform;
	fillTransform(startTransform, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot);

	// get real scale of object
	btVector3 vLocalShapeScale = btVector3(pObject->position.vecScale.x, pObject->position.vecScale.y, pObject->position.vecScale.z);

	// create rigid body
	btRigidBody* body = NULL;
	btDefaultMotionState* myMotionState = new btDefaultMotionState( startTransform );

	switch (iShapeUsed)
	{
		case 0: 
		{
			trimeshShape->setLocalScaling(vLocalShapeScale);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, trimeshShape, localInertia);
			body = new btRigidBody(rbInfo);
			break;
		}
		case 1:
		{
			simplifiedConvexShape->setLocalScaling(vLocalShapeScale);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, simplifiedConvexShape, localInertia);
			body = new btRigidBody(rbInfo);
			break;
		}
		case 2:
		{
			compoundShape->setLocalScaling(vLocalShapeScale);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, compoundShape, localInertia);
			body = new btRigidBody(rbInfo);
			break;
		}	
	}

	// pass material index as usrptr
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	body->setUserPointer((void*)iArbValue);

	// add physics object to system
	short sColliderType = COL_OBJECT;
	if (isDynamic == 1) sColliderType = COL_OBJECT_DYNAMIC;
	short sCollidesWith = COL_OBJECT | COL_OBJECT_DYNAMIC | COL_CAPSULECHAR | COL_TERRAIN;
	if ( iTerrainMesh == 1 )
		g_dynamicsWorld->addRigidBody ( body, COL_TERRAIN, sCollidesWith);
	else
		g_dynamicsWorld->addRigidBody ( body, sColliderType, sCollidesWith);

	// boyancy mode
	bool bBouyant = false;
	if ( isDynamic==1 ) 
		bBouyant = true; 
	else
		pObject = NULL;

	// add physics object details to pbject list
	ODEAddObject ( iObjectNumber, pObject, body, isDynamic, myMotionState, false, bBouyant, gVertices, gIndices, 1, 1, fFriction, mass );
}

void DeleteallImporters()
{
	for (int i = 0; i < g_importers.size(); i++)
	{
		g_importers[i]->deleteAllData();
	}
	g_importers.clear();
}

void CreateTerrain ( int iObjectNumber, int isDynamic, int iWidth, int iLength, DWORD dwMemBlockPtr )
{
	// problem is that Blitzwerks uses alternating quad edge order which means polygon slope cannot be
	// predicted by Bullet height map shape (pity as it would have been low memory fast performance

	// get reference object
	sObject* pObject = GetObjectData ( iObjectNumber );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot;
	GetSizePositionRotation(pObject, fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot, false );

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// create a bullet shape
	int s_gridSize = iWidth; // 1024x1024
	float m_minHeight = 0.0f;
	float m_maxHeight = 50000.0f/gSc;
	int m_upAxis = 1;
	bool flipQuadEdges = true;
	float* m_rawHeightfieldData = new float[s_gridSize*s_gridSize];
	memset ( m_rawHeightfieldData, 0, sizeof(float)*s_gridSize*s_gridSize );
	float* pMemBlockPtr = (float*)dwMemBlockPtr;
	for ( int t=0; t<s_gridSize*s_gridSize; t++ )
	{
		m_rawHeightfieldData[t] = (*pMemBlockPtr) / gSc;
		pMemBlockPtr++;
	}
	btHeightfieldTerrainShape* terrainShape = new btHeightfieldTerrainShape(	s_gridSize, s_gridSize,
																					m_rawHeightfieldData,
																					0.0f,
																					m_minHeight, m_maxHeight,
																					m_upAxis, PHY_FLOAT, flipQuadEdges);
	btVector3 localScaling = btVector3(50.0f/gSc,1.0f,50.0f/gSc);
	terrainShape->setLocalScaling(localScaling);

	// add shape to collision list so can release later, they can be "re-used" amongst bodies!!
	g_collisionShapes.push_back(terrainShape);

	// fully static is our ground terrain!!
	btScalar mass(0/gMs);
	btVector3 localInertia(0,0,0);
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin( btVector3( fXPos, fYPos, fZPos ) );

	// motionstate provides interpolation capabilities and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,terrainShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// pass material index as usrptr
	sObject* pRealObject = pObject;
	if ( pRealObject->pInstanceOfObject ) pRealObject = pRealObject->pInstanceOfObject;
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	body->setUserPointer((void*)iArbValue);

	// add physics object to system
	short sCollidesWith = COL_OBJECT | COL_OBJECT_DYNAMIC | COL_CAPSULECHAR | COL_TERRAIN;
	g_dynamicsWorld->addRigidBody ( body, COL_TERRAIN, sCollidesWith);

	// boyancy mode
	bool bBouyant = false;
	if ( isDynamic==1 ) 
		bBouyant = true; 
	else
		pObject = NULL;

	// add physics object details to pbject list
	ODEAddObject ( iObjectNumber, pObject, body, isDynamic, myMotionState, false, bBouyant, (LPVOID)m_rawHeightfieldData, NULL, 1, 1, 100.0f, mass );
}

void UpdateTerrain ( int iObjectNumber, int isDynamic, int iWidth, int iLength, LPSTR dwMemBlockPtr, int iX1, int iZ1, int iX2, int iZ2 )
{
	// get reference object
	sObject* pObject = GetObjectData ( iObjectNumber );
	if ( pObject==NULL ) return;
	sObjectList* pPhyObject = ODEFindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return;

	// NOTE: Terrain collision replaced with sector meshes (which will need to be updated instead!!)
	float* m_rawHeightfieldData = (float*)pPhyObject->pMem1;
	if ( m_rawHeightfieldData==NULL ) return;

	// create a bullet shape
	int s_gridSize = iWidth; // 1024x1024
	float m_minHeight = 0.0f;
	float m_maxHeight = 50000.0f/gSc;
	int m_upAxis = 1;
	bool flipQuadEdges = true;
	float* pMemBlockPtr = (float*)dwMemBlockPtr;
	for ( int y=iZ1; y<=iZ2; y++ )
	{
		for ( int x=iX1; x<=iX2; x++ )
		{
			int t = (y*s_gridSize)+x;
			m_rawHeightfieldData[t] = (*pMemBlockPtr) / gSc;
			pMemBlockPtr++;
		}
	}

	// remove body from simulation
	g_dynamicsWorld->removeRigidBody(pPhyObject->body);

	// remove collision shape
	btHeightfieldTerrainShape* terrainOLDShape = (btHeightfieldTerrainShape*)pPhyObject->body->getCollisionShape();
	delete terrainOLDShape;

	// create new shape
	btHeightfieldTerrainShape* terrainNEWShape = new btHeightfieldTerrainShape(	s_gridSize, s_gridSize,
																					m_rawHeightfieldData,
																					0.0f,
																					m_minHeight, m_maxHeight,
																					m_upAxis, PHY_FLOAT, flipQuadEdges);

	// prepare new shape intertia
	btVector3 localScaling = btVector3(50.0f/gSc,1.0f,50.0f/gSc);
	terrainNEWShape->setLocalScaling(localScaling);

	// assign new shape
	pPhyObject->body->setCollisionShape(terrainNEWShape);

	// pass material index as usrptr
	sObject* pRealObject = pObject;
	if ( pRealObject->pInstanceOfObject ) pRealObject = pRealObject->pInstanceOfObject;
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	pPhyObject->body->setUserPointer((void*)iArbValue);

	// add new body back to simulation
	short sCollidesWith = COL_OBJECT | COL_OBJECT_DYNAMIC | COL_CAPSULECHAR | COL_TERRAIN;
	g_dynamicsWorld->addRigidBody ( pPhyObject->body, COL_TERRAIN, sCollidesWith);

	// replace shape in collision shapes list
	for (int j=0;j<g_collisionShapes.size();j++)
	{
		if ( g_collisionShapes[j]==terrainOLDShape )
		{
			g_collisionShapes[j] = terrainNEWShape;
			break;
		}
	}
}

btCollisionShape* g_pTerrainShape = 0;

void ODECreateGGTerrain()
{
	g_pTerrainShape = new GGTerrainPhysicsShape( gSc );

	g_collisionShapes.push_back( g_pTerrainShape );

	btScalar mass( 0 );
	btVector3 localInertia( 0, 0, 0 );
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin( btVector3(0, 0, 0) );

	btDefaultMotionState* myMotionState = new btDefaultMotionState( startTransform );
	btRigidBody::btRigidBodyConstructionInfo rbInfo( mass, myMotionState, g_pTerrainShape, localInertia );
	btRigidBody* body = new btRigidBody( rbInfo );

	body->setUserPointer((void*)12345);// indicate this is terrain and uses material look up for footfall sounds - old was 0 );

	// add physics object to system
	short sCollidesWith = COL_OBJECT | COL_OBJECT_DYNAMIC | COL_CAPSULECHAR;
	g_dynamicsWorld->addRigidBody ( body, COL_TERRAIN, sCollidesWith);

	//PE: Add fake collision callback for new ggterrain system.
	ODEAddObject(7000, nullptr, body, false, myMotionState, false, false, NULL, NULL, 1, 1, 100.0f, mass);

}

void CreateCapsule ( int iObjectNumber, int isDynamic, float fScaleModifier, float fRaised, float fWeight, float fFriction, float fCollisionScaling, float fCollisionScalingXZ)
{
	// defaults
	if ( fWeight==-1 ) fWeight = 100.0f;
	if ( fFriction==-1 ) fFriction = 90.0f;
	if (fCollisionScaling == 0) fCollisionScaling = 1.0f;
	if (fCollisionScalingXZ == 0) fCollisionScalingXZ = 1.0f;
	float fRestitution = 0.0f;

	// get reference object
	sObject* pObject = GetObjectData ( iObjectNumber );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot;
	GetSizePositionRotation(pObject, fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot, true);

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// volume calculation
	float fVolume = 0;
	fVolume = fXSize * fYSize * fZSize;

	// modify with weight modifier
	fVolume *= (fWeight/100.0f);

	// instead of fixed capsule height, calc proportion so a character 73 units high gives capsule of 80(half=40)
	float fProportionalCapsuleHeight = (fYSize*gSc)*0.55f;

	// additional ability to scale the capsule height
	fProportionalCapsuleHeight *= fCollisionScaling;

	// apply modifier
	if ( fScaleModifier==0.0f ) fScaleModifier = 1.0f;
	float fCapsuleHeight = fProportionalCapsuleHeight/gSc;
	fCapsuleHeight = fCapsuleHeight * fScaleModifier;

	// somenow affects final height of capsule in simulation, find out metrics!!
	// also use proportional radius for capsule (fat animals; cows)
	#ifdef WICKEDENGINE
	// LB: nav mesh allows paths through thinner gaps, still so reduce a little more
	float fProportionalCapsuleRadius = (((fXSize + fZSize) / 2.0f / 2.0f)*gSc)*0.5f;
	#else
	float fProportionalCapsuleRadius = (((fXSize+fZSize)/2.0f/2.0f)*gSc)*0.8f; // so zombie can get through narrower door!
	#endif

	// will offset real object by HALF of total height of capsule (sphere halves+cylinder)
	fRaised = fRaised + ((fProportionalCapsuleRadius*2)+fProportionalCapsuleHeight * fScaleModifier)/2.0f;

	// create a bullet shape (fScaleModifier applied to support on-standard characters; creatures)
	float fCapsuleWidth = (fProportionalCapsuleRadius/gSc) * fScaleModifier * fCollisionScalingXZ;
	btCollisionShape* capsuleShape = new btCapsuleShape(fCapsuleWidth, fCapsuleHeight);

	// add shape to collision list so can release later, they can be "re-used" amongst bodies!!
	g_collisionShapes.push_back(capsuleShape);

	// mass/volume calculation
	if ( isDynamic==0 )
	{
		fVolume = 0;
	}
	else
	{
		// max
		if (fVolume > maxDynamicVolume) fVolume = maxDynamicVolume;

		// also handle min - ensure MASS not SO SMALL THINGS FLY MILLIONS OF MILES!
		if (fVolume < 0.1f) fVolume = 0.1f;
	}
	btScalar mass(fVolume/gMs);
	btVector3 localInertia(0,0,0);
	if ( isDynamic==1 ) capsuleShape->calculateLocalInertia(mass,localInertia);

	// set position transform for physics object
	btTransform startTransform;
	fillTransform( startTransform, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot );

	// motionstate provides interpolation capabilities and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,capsuleShape,localInertia);
	rbInfo.m_friction = 1.0f;
	rbInfo.m_restitution = fRestitution/100.0f;
	btRigidBody* body = new btRigidBody(rbInfo);

	// ensure capsule stands upright
	body->setAngularFactor(0.0f);

	// pass material index as usrptr
	sObject* pRealObject = pObject;
	if ( pRealObject->pInstanceOfObject ) pRealObject = pRealObject->pInstanceOfObject;
	int iArbValue = 0;
	if ( pRealObject->ppMeshList!=NULL ) iArbValue = pRealObject->ppMeshList[0]->Collision.dwArbitaryValue;
	body->setUserPointer((void*)iArbValue);

	// add physics object to system
	short sCollidesWith = COL_OBJECT | COL_OBJECT_DYNAMIC | COL_TERRAIN;
	g_dynamicsWorld->addRigidBody ( body, COL_CAPSULECHAR, sCollidesWith);

	// add physics object details to pbject list
	bool bBouyant = false;
	ODEAddObject ( iObjectNumber, pObject, body, isDynamic, myMotionState, true, bBouyant, NULL, NULL, fScaleModifier, fRaised, fFriction, mass );
}

void ODECreateDynamicCharacterController ( int iObjectNumber, float fGravity, float fFallSpeed, float fMaxSlope )
{
	// set new global defaults for character controller
	fCharacterGravity = fGravity;
	fCharacterFallSpeed = fFallSpeed;
	fCharacterMaxSlope = fMaxSlope;

	// get reference object
	sObject* pObject = GetObjectData ( iObjectNumber );
	if ( pObject==NULL ) return;

	// obtain sizes, position and rotation
	float fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot;
	// 280618 - this threw initial player position up (incorrectly correcting for object center!)
	bool bMakeCollisionCenterAdjustment = false;
	GetSizePositionRotation(pObject, fXSize, fYSize, fZSize, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot, bMakeCollisionCenterAdjustment); 

	// just before shape creation, reduce scale of size and position
	fXSize /= gSc;
	fYSize /= gSc;
	fZSize /= gSc;
	fXPos /= gSc;
	fYPos /= gSc;
	fZPos /= gSc;

	// set position transform for physics object
	btTransform startTransform;
	fillTransform( startTransform, fXPos, fYPos, fZPos, fXRot, fYRot, fZRot );

	// create a character controller shape (if not already created)
	if ( m_ghostObject==NULL )
	{
		// create a new
		m_ghostObject = new btPairCachingGhostObject();
		m_ghostObject->setWorldTransform(startTransform);
		sweepBP->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
		btScalar characterWidth  = 15.0f/gSc;
		btScalar characterHeight = 40.0f/gSc;

		btConvexShape* capsule = new btCapsuleShape(characterWidth,characterHeight);
		m_ghostObject->setCollisionShape (capsule);
		m_ghostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);
		btScalar stepHeight = btScalar(10.35 / gSc);
		m_character = new btKinematicCharacterController (m_ghostObject,capsule,stepHeight);

		// only collide with static for now (no interaction with dynamic objects)
		#ifdef WICKEDENGINE
		g_dynamicsWorld->addCollisionObject (m_ghostObject, COL_CAPSULECHAR, COL_OBJECT | COL_OBJECT_DYNAMIC | COL_TERRAIN);
		#else
		g_dynamicsWorld->addCollisionObject (m_ghostObject, COL_OBJECT, COL_OBJECT | COL_OBJECT_DYNAMIC | COL_TERRAIN);
		#endif
		g_dynamicsWorld->addAction(m_character);
	}
	else
	{
		// simply reposition existing
		m_ghostObject->setWorldTransform(startTransform);
	}

	// required for character
	g_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(m_ghostObject->getBroadphaseHandle(),g_dynamicsWorld->getDispatcher());

	// setup character settings
	m_character->reset ();
	m_character->resetFallStuff();
	m_character->setGravity(fCharacterGravity/gSc); 
	m_character->setFallSpeed(fCharacterFallSpeed/gSc);
	m_character->setMaxSlope(GGToRadian(fCharacterMaxSlope));
	m_character->warp (btVector3(fXPos,fYPos,fZPos));
	m_character->m_bCurrentlyDucked = false;
	m_character->m_bDidJump = false;

	// and pass water level to character
	m_character->setWaterLineY (g_fWaterLineY);

	// add physics object details to pbject list
	bool bBouyant = false;
	ODEAddObject ( iObjectNumber, pObject, NULL, 1, NULL, false, bBouyant, NULL, NULL, NULL, NULL, 100.0f, -1.0f );
}

void ODESetDynamicCharacterController ( int iObjectNumber, float fWaterLineY, float fRes1, float fRes2, float fRes3, float fRes4, float fRes5, float fRes6 )
{
	// get reference object
	sObject* pObject = GetObjectData ( iObjectNumber );
	if ( pObject==NULL ) return;

	// set properties for the character on the fly
	g_fWaterLineY = fWaterLineY;
	if ( m_character ) m_character->setWaterLineY ( fWaterLineY );
}

void ODESetWaterLine ( float fWaterLineY )
{
	// set properties for the character on the fly
	g_fWaterLineY = fWaterLineY;
	if ( m_character ) m_character->setWaterLineY ( fWaterLineY );
}

// Manually set character walk direction. axisX,Y,Z are local to the player. Should be called after ODEControlDynamicCharacterController.
void ODESetCharacterDirectionOverride(float angX, float angY, float angZ, float axisX, float axisY, float axisZ, float mag) 
{
	if (m_character)
	{
		GGMATRIX matRot;
		GGMatrixIdentity(&matRot);
		GGMATRIX matX, matY, matZ;

		// Create a rotation matrix using the provided angles.
		GGMatrixRotationX(&matX, GGToRadian(angX));
		GGMatrixRotationY(&matY, GGToRadian(angY));
		GGMatrixRotationZ(&matZ, GGToRadian(angZ));
		GGMatrixMultiply(&matRot, &matRot, &matX);
		GGMatrixMultiply(&matRot, &matRot, &matY);
		GGMatrixMultiply(&matRot, &matRot, &matZ);
	
		// Transform default forward vector by the rotation matrix to get the new direction vector.
		GGVECTOR3 look(axisX, axisY, axisZ);
		GGVec3TransformCoord(&look, &look, &matRot);
		GGVec3Normalize(&look, &look);

		#ifdef PRODUCTCONVERTER
		m_character->setWalkDirection((btVector3(look.x, look.y, look.z)*mag) / gSc);
		#else
		if (m_character->GetSwimmingLimitVertical())
			m_character->setWalkDirection((btVector3(look.x, 0, look.z)*mag) / gSc);
		else
			m_character->setWalkDirection((btVector3(look.x, look.y, look.z)*mag) / gSc);
		#endif
	}
}

void ODESwimmingLimitVertical(int limit)
{
#ifdef WICKEDENGINE
	m_character->SetSwimmingLimitVertical(limit);
#endif
}

void ODEGetCharacterWalkDirection(GGVECTOR3* look)
{
#ifdef WICKEDENGINE
	btVector3 direction = m_character->GetWalkDirection();
	GGVECTOR3 dir = GGVECTOR3(direction.getX(), direction.getY(), direction.getZ());
	GGVec3Normalize(look, &dir);
#endif
}

bool g_bGlobalCurrentlyDucked = false;

void ODEControlDynamicCharacterController ( int iObjectNumber, float fAngleY, float fAngleX, float fSpeed, float fJump, float fDucking, float fPushAngle, float fPushForce, float fThrustUpwards )
{
	// get reference object
	sObject* pObject = GetObjectData ( iObjectNumber );
	if ( pObject==NULL ) return;
	if ( m_character==NULL ) return;

	// if physics world present
	if ( g_dynamicsWorld!=NULL )
	{
		GGMATRIX matRotX;
		GGMatrixRotationX ( &matRotX, GGToRadian(fAngleX) );
		GGMATRIX matRotY;
		GGMatrixRotationY ( &matRotY, GGToRadian(fAngleY) );
		GGMATRIX matRot;
		GGMatrixMultiply ( &matRot, &matRotX, &matRotY );
		GGVECTOR3 vecForward = GGVECTOR3(0,0,1);
		GGVec3TransformCoord ( &vecForward, &vecForward, &matRot );
		if ( fAngleX<0 ) vecForward.y *= -1;
		btVector3 walkDirection = btVector3(vecForward.x,vecForward.y,vecForward.z);
		btScalar walkVelocity = fSpeed;
		btScalar walkSpeed = walkVelocity;

		// and pass water level to character
		m_character->setWaterLineY (g_fWaterLineY);

		// control character controller jumping
		m_character->m_bDidJump = false;
		if ( m_character->canJump() )
		{
			if ( fJump > 0.0f )
			{
				m_character->setJumpSpeed(fJump/gSc);
				m_character->jump();
			}
		}

		// new walk/push method work together
		// push character if forced in a specific direction
		// control character controller for walk direction
		btVector3 bvWalkDir = walkDirection * walkSpeed;
		btVector3 bvCombinedDir = bvWalkDir;
		if (fPushForce > 0.0f)
		{
			float fPushAngleRad = GGToRadian(fPushAngle);
			btVector3 vecPush = btVector3(sin(fPushAngleRad), 0.0, cos(fPushAngleRad));
			bvCombinedDir += (vecPush * fPushForce);
		}
		if (fabs(bvCombinedDir.getX())>0 || fabs(bvCombinedDir.getY())>0 || fabs(bvCombinedDir.getZ())>0)
		{
			bPlayerShouldbeMoving = true;
			m_character->setWalkDirection(bvCombinedDir / gSc);
		}
		else 
		{
			bPlayerShouldbeMoving = false;
			m_character->setWalkDirection(btVector3(0.0f, 0.0f, 0.0f));
		}
		/* old method can only walk OR push, not both
		// push character if forced in a specific direction
		if ( fPushForce>0.0f )
		{
			// cannot walk when being pushed
			float fPushAngleRad = GGToRadian(fPushAngle);
			btVector3 vecPush = btVector3(sin(fPushAngleRad),0.0,cos(fPushAngleRad));
			m_character->setWalkDirection((vecPush*fPushForce)/gSc);
		}
		else
		{
			// control character controller for walk direction
			btVector3 bvWalkDir = walkDirection * walkSpeed;
			if (walkSpeed > 0) {
				bPlayerShouldbeMoving = true;
				m_character->setWalkDirection(bvWalkDir / gSc);
			}
			else {
				bPlayerShouldbeMoving = false;
				m_character->setWalkDirection(btVector3(0.0f,0.0f,0.0f));
			}
		}
		*/

		// Apply thrust value to system
		m_character->m_fThrustUpwards = fThrustUpwards;

		// Ducking control
		float fXPos = pObject->position.vecPosition.x;
		float fYPos = pObject->position.vecPosition.y+1;
		float fZPos = pObject->position.vecPosition.z;
		if ( fDucking==1.0f )
		{
			if (g_bGlobalCurrentlyDucked == false )// m_character->m_bCurrentlyDucked==false )
			{
				// change to small capsule
				btCollisionShape* oldrigidbodyshape = m_ghostObject->getCollisionShape();
				btScalar characterWidth  = 15.0f/gSc;
				btScalar characterHeight = 22.0f/gSc;
				btConvexShape* capsule = new btCapsuleShape(characterWidth,characterHeight);
				m_ghostObject->setCollisionShape ( capsule );
				m_character->SetConvexShape ( capsule );
				delete oldrigidbodyshape;

				//m_character->m_bCurrentlyDucked = true;
				g_bGlobalCurrentlyDucked = true;

				// reposition character for ducking (otherwise it jitters)
				m_character->warp (btVector3(fXPos/gSc,fYPos/gSc,fZPos/gSc));
			}
		}
		else
		{
			if (g_bGlobalCurrentlyDucked==true )//m_character->m_bCurrentlyDucked==true )
			{
				// do raycast to see if we can unduck 
				bool bHitSomething = false;
				for ( int allcorners=0; allcorners<5; allcorners++ )
				{
					btVector3 Start, End, Normal;
					if ( allcorners==0 ) { Start = btVector3(fXPos,fYPos,fZPos); End = btVector3(fXPos,fYPos+40,fZPos); }
					if ( allcorners==1 ) { Start = btVector3(fXPos-15,fYPos,fZPos-15); End = btVector3(fXPos-15,fYPos+40,fZPos-15); }
					if ( allcorners==2 ) { Start = btVector3(fXPos+15,fYPos,fZPos-15); End = btVector3(fXPos+15,fYPos+40,fZPos-15); }
					if ( allcorners==3 ) { Start = btVector3(fXPos-15,fYPos,fZPos+15); End = btVector3(fXPos-15,fYPos+40,fZPos+15); }
					if ( allcorners==4 ) { Start = btVector3(fXPos+15,fYPos,fZPos+15); End = btVector3(fXPos+15,fYPos+40,fZPos+15); }
					Start.setX(Start.getX() / gSc);
					Start.setY(Start.getY() / gSc);
					Start.setZ(Start.getZ() / gSc);
					End.setX(End.getX() / gSc);
					End.setY(End.getY() / gSc);
					End.setZ(End.getZ() / gSc);
					btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
					g_dynamicsWorld->rayTest(Start, End, RayCallback);
					if(RayCallback.hasHit())
					{
						//End = RayCallback.m_hitPointWorld;
						//Normal = RayCallback.m_hitNormalWorld;
						bHitSomething = true;
					}
				}

				// if hit, don't get up yet!
				if ( bHitSomething==false )
				{
					// change back to regular capsule
					btCollisionShape* oldrigidbodyshape = m_ghostObject->getCollisionShape();
					btScalar characterWidth  = 15.0f/gSc;
					btScalar characterHeight = 40.0f/gSc;
					btConvexShape* capsule = new btCapsuleShape(characterWidth,characterHeight);
					m_ghostObject->setCollisionShape ( capsule );
					m_character->SetConvexShape ( capsule );
					delete oldrigidbodyshape;

					//m_character->m_bCurrentlyDucked = false;
					g_bGlobalCurrentlyDucked = false;
				}
			}
		}

		// ensure object associated with controller is updated in ODEUPDATE
		g_CharacterControlObject = iObjectNumber;
	}
}

DWORD ODEGetDynamicCharacterControllerDidJump ( void )
{
	if ( m_character )
	{
		if ( m_character->m_bDidJump==true )
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

DWORD ODEGetCharacterControllerDucking ( int iObjectNumber )
{
	if ( m_character )
	{
		if (g_bGlobalCurrentlyDucked==true) //m_character->m_bCurrentlyDucked==true )
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

// handle occasional skipping over materials (occasionally zero and then touch surface again
int iTouchFloorMaterialCount = 0;
int iTouchFloorMaterialID = 0;

int ODEGetCharacterHitFloor ( void )
{
	// returns value passed in for this collider (material type)
	if ( m_character )
	{
		if (m_character->iHitFloor > 0)
		{
			iTouchFloorMaterialCount = 50;
			iTouchFloorMaterialID = m_character->iHitFloor;
			m_character->iHitFloor = 0;
		}
		else
		{
			if (iTouchFloorMaterialCount > 0)
			{
				iTouchFloorMaterialCount--;
				if (iTouchFloorMaterialCount <= 0)
				{
					iTouchFloorMaterialCount = 0;
					iTouchFloorMaterialID = 0;
				}
			}
		}
		return iTouchFloorMaterialID;
	}
	else
		return 0;
}
float ODEGetCharacterFallDistance ( void )
{
	if ( m_character )
	{
		float fReturnValue = m_character->fFallDistance * gSc;
		fReturnValue = fReturnValue / 2.0f; //seems too sensitive, check with debug later!
		m_character->fFallDistance = 0.0f;
		return fReturnValue;
	}
	else
		return 0;
}

void ODECreateStaticSphere	( int iObjectNumber )
{
	CreateSphere ( iObjectNumber, 0, -1, -1, -1 );
}
void ODECreateStaticBox ( int iObjectNumber, int iLimbNumber )
{
	CreateBox ( iObjectNumber, 0, iLimbNumber, 0, -1, -1, -1, false, 0,0,0,0,0,0,0,0,0 );
}
void ODECreateStaticBox ( int iObjectNumber )
{
	CreateBox ( iObjectNumber, 0, -1, 0, -1, -1, -1, false, 0,0,0,0,0,0,0,0,0);	            
}
void ODECreateStaticCylinder ( int iObjectNumber )
{
	CreateCylinder ( iObjectNumber, 0, -1, -1, -1 );
}
void ODECreateStaticCylinder ( int iObjectNumber, float fXPos, float fYPos, float fZPos, float fXSize, float fYSize, float fZSize, float fXRot, float fYRot, float fZRot )
{
	CreateCylinder ( iObjectNumber, 0, fXPos, fYPos, fZPos, fXSize, fYSize, fZSize, fXRot, fYRot, fZRot, -1, -1, -1 );
}
void ODESetMeshFilename (LPSTR pName)
{
	SetMeshFilename (pName);
}
void ODESetOBJLoadingFilename (LPSTR pName)
{
	SetOBJLoadingFilename (pName);
}
void ODECreateStaticTerrainMesh ( int iObjectNumber )
{
	CreateMesh ( iObjectNumber, 0, -1, 1, 100, 0, -1, -1, -1 );
}
void ODECreateStaticTriangleMesh ( int iObjectNumber )
{
	CreateMesh ( iObjectNumber, 0, -1, 0, 100, 0, -1, -1, -1 );
}
void ODECreateStaticTriangleMesh ( int iObjectNumber, int iLimbNumber )
{
	CreateMesh ( iObjectNumber, 0, iLimbNumber, 0, 100, 0, -1, -1, -1 );
}
void ODECreateStaticTriangleMesh ( int iObjectNumber, int iLimbNumber, int iCollisionScaling )
{
	CreateMesh ( iObjectNumber, 0, iLimbNumber, 0, iCollisionScaling, 0, -1, -1, -1 );
}
void ODECreateStaticTriangleMesh ( int iObjectNumber, int iLimbNumber, int iCollisionScaling, int iHullReductionMode )
{
	CreateMesh ( iObjectNumber, 0, iLimbNumber, 0, iCollisionScaling, iHullReductionMode, -1, -1, -1 );
}
void ODECreateStaticTerrain ( int iObjectNumber, int iWidth, int iLength, DWORD dwMemBlockPtr )
{
	CreateTerrain ( iObjectNumber, 0, iWidth, iLength, dwMemBlockPtr );
}
void ODEUpdateStaticTerrain ( int iObjectNumber, int iWidth, int iLength, LPSTR dwMemBlockPtr, int iX1, int iZ1, int iX2, int iZ2 )
{
	UpdateTerrain ( iObjectNumber, 0, iWidth, iLength, dwMemBlockPtr, iX1, iZ1, iX2, iZ2 );
}
void ODECreateStaticCapsule ( int iObjectNumber, float fCollisionScaling, float fCollisionScalingXZ)
{
	CreateCapsule ( iObjectNumber, 0, 1.0f, 0.0f, -1, -1, fCollisionScaling, fCollisionScalingXZ);
}
void ODECreateDynamicSphere ( int iObjectNumber, float fWeight, float fFriction, float fRestitution )
{
	CreateSphere ( iObjectNumber, 1, fWeight, fFriction, fRestitution );
}
void ODECreateDynamicBox ( int iObjectNumber )
{
	CreateBox ( iObjectNumber, 1, -1, 0, -1, -1, -1, false,0,0,0,0,0,0,0,0,0 );
}
void ODECreateDynamicBox ( int iObjectNumber, int iLimbNumber, int iTipOverMode )
{
	CreateBox ( iObjectNumber, 1, iLimbNumber, iTipOverMode, -1, -1, -1 , false, 0,0,0,0,0,0,0,0,0);
}
void ODECreateDynamicBox ( int iObjectNumber, int iLimbNumber, int iTipOverMode, float fWeight, float fFriction, float fRestitution )
{
	CreateBox ( iObjectNumber, 1, iLimbNumber, iTipOverMode, fWeight, fFriction, fRestitution, false, 0,0,0,0,0,0,0,0,0 );
}
void ODECreateDynamicBoxManual( int iObjectNumber, float fX, float fY, float fZ, float fXSize, float fYSize, float fZSize, float fXAng, float fYAng, float fZAng)
{
	CreateBox(iObjectNumber, 1, -1, 0, -1, -1, -1, true, fX,fY,fZ,fXSize,fYSize,fZSize,fXAng,fYAng,fZAng);
}
void ODECreateDynamicCylinder ( int iObjectNumber, float fWeight, float fFriction, float fRestitution )
{
	CreateCylinder ( iObjectNumber, 1, fWeight, fFriction, fRestitution );
}

void ODECreateDynamicTriangleMesh(int iObjectNumber, int iLimbNumber, int iCollisionScaling, int iHullReductionMode)
{
	CreateMesh(iObjectNumber, 1, iLimbNumber, 0, iCollisionScaling, iHullReductionMode, -1, -1, -1);
}

void ODECreateDynamicTriangleMesh ( int iObjectNumber, float fWeight, float fFriction, float fRestitution, int iHullReductionMode )
{
	CreateMesh ( iObjectNumber, 1, -1, 0, 100, iHullReductionMode, fWeight, fFriction, fRestitution );
}
void ODECreateDynamicCapsule ( int iObjectNumber, float fScaleModifier, float fRaised, float fWeight, float fFriction, float fCollisionScaling, float fCollisionScalingXZ )
{
	CreateCapsule ( iObjectNumber, 1, fScaleModifier, fRaised, fWeight, fFriction, fCollisionScaling, fCollisionScalingXZ);
}
void ODEStartStaticObject ( int iObjectNumber )
{
	StartStaticObject ( iObjectNumber );
}
void ODEEndStaticObject ( int iObjectNumber, int iTerrainCollider  )
{
	EndStaticObject ( iObjectNumber, iTerrainCollider );
}
void ODEAddStaticObjectBox ( int iObjectNumber , int iStaticBoxObject, int iMaterialRefIndex )
{
	AddStaticObjectBox ( iObjectNumber , iStaticBoxObject, iMaterialRefIndex );
}
int ODECreateHingeSingle( int iObjectNumber, float fX, float fY, float fZ, int iType, float fAngMin, float fAngMax )
{
	return CreateHingeSingle( iObjectNumber, fX, fY, fZ, iType, fAngMin, fAngMax );
}
void ODESetHingeLimits( int iConstraint, float fAngMin, float fAngMax, float fSoftness, float fBias, float fRelaxation )
{
	SetHingeLimits(iConstraint, fAngMin, fAngMax, fSoftness, fBias, fRelaxation );
}
void ODESetSliderLimits( int iConstraint, float fLowerLin, float fUpperLin, float fLowerAng, float fUpperAng )
{
	SetSliderLimits( iConstraint, fLowerLin, fUpperLin, fLowerAng, fUpperAng );
}
void ODESetSliderMotor( int iConstraint, bool OnOff, float fForce, float fVelocity )
{
	SetSliderMotor( iConstraint, OnOff, fForce, fVelocity );
}
void ODESetHingeMotor( int iConstraint, float fSpeed, float fAngle, float fForce )
{
	SetHingeMotor( iConstraint, fSpeed, fAngle, fForce );
}
float ODEGetHingeAngle( int iConstraint )
{
	return GetHingeAngle( iConstraint );
}
float ODEGetSliderPosition( int iConstraint )
{
	return GetSliderPosition( iConstraint );
}
int ODECreateHingeDouble( int iObjectNumA, int iObjectNumB, float fXa, float fYa, float fZa, 
	                                                        float fXb, float fYb, float fZb, 
	                                                        int iTypeA, int iTypeB, int noCollision )
{
    return CreateHingeDouble( iObjectNumA, iObjectNumB, fXa, fYa, fZa, fXb, fYb, fZb, iTypeA, iTypeB, noCollision );
}
int ODECreateJointSingle( int iObjectNumber, float fX, float fY, float fZ )
{
	return CreateJointSingle( iObjectNumber, fX, fY, fZ );
}
int ODECreateJointDouble( int iObjectNumA, int iObjectNumB, float fXa, float fYa, float fZa, float fXb, float fYb, float fZb, int noCollision)
{
	return CreateJointDouble( iObjectNumA, iObjectNumB, fXa, fYa, fZa, fXb, fYb, fZb, noCollision );
}
int ODECreateSliderDouble( int iObjectNumA, int iObjectNumB,
	                       float qXa, float qYa, float qZa, float qWa,
	                       float fXa, float fYa, float fZa, 
	                       float qXb, float qYb, float qZb, float qWb,
	                       float fXb, float fYb, float fZb, 
	                       bool useLinearReferenceFrameA )
{
	return CreateSliderDouble( iObjectNumA, iObjectNumB, qXa, qYa, qZa, qWa, fXa, fYa, fZa, qXb, qYb, qZb, qWb, fXb, fYb, fZb, useLinearReferenceFrameA );
}


void ODEDestroyObject ( int iObjectNumber )
{
	//PE: dont touch while simulation is processed.
	physicslock.lock();
	// get reference object
	sObjectList* pPhyObject = ODEFindID ( iObjectNumber );
	if (pPhyObject == NULL)
	{
		physicslock.unlock();
		return;
	}
	DestroyThisObject ( pPhyObject );
	ODERemoveObject ( iObjectNumber );
	physicslock.unlock();
}

// Does not work at the moment
// The idea is to have a dynamic entity that does not response to gravity or hitting the terrain
// The no gravity works, but without setting collision flags to no response it bounces off terrain
// Not always consistant. No contact response does seem to work, but then nothing collides with the object either 
// (so it gives no response and has no respone either)
void ODESetNoGravity( int iObjectNumber, int iGravity )
{
	#ifdef WICKEDENGINE
	// all objects start with gravity, force off if user wishes
	sObjectList* pPhyObject = ODEFindID (iObjectNumber);
	if (pPhyObject)
	{
		if (iGravity == 0)
		{
			// zero gravity
			btVector3 vecGravReset = btVector3(0, 0, 0);
			pPhyObject->body->setGravity (vecGravReset);
			pPhyObject->body->setFlags (!BT_DISABLE_WORLD_GRAVITY);
		}
		else
		{
			// default gravity
			pPhyObject->body->setGravity (btVector3(0, -10.0f, 0));
			pPhyObject->body->setFlags (pPhyObject->body->getFlags() | BT_DISABLE_WORLD_GRAVITY);
		}
	}
	#else
	/*sObjectList* pPhyObject = ODEFindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return;
	if ( iGravity )
	{
		btVector3 acceleration(0,0,0);
		pPhyObject->body->setGravity( acceleration);
		//pPhyObject->body->setLinearVelocity(acceleration);
		//pPhyObject->body->setAngularVelocity(acceleration);
		//pPhyObject->body->setMassProps(0,acceleration);
		pPhyObject->body->setFlags ( BT_DISABLE_WORLD_GRAVITY );
		//COL_OBJECT | COL_OBJECT_DYNAMIC | COL_CAPSULECHAR | 
		pPhyObject->body->setCollisionFlags( btCollisionObject::CF_NO_CONTACT_RESPONSE) ;
		pPhyObject->body->forceActivationState(1);
	}
	else
	{
		pPhyObject->body->setFlags ( !BT_DISABLE_WORLD_GRAVITY );
	}
	*/
	#endif
}

// Ragdoll commands

void DBProReportError ( LPSTR pErr1, LPSTR pErr2 )
{
	MessageBox ( NULL, pErr1, pErr2, MB_OK );
}

void BPhys_RagDollBegin(int objectID, float objTotalWeight)
{
#ifdef WICKEDENGINE
	// Ragdoll rigid bodies are not added to g_PhyObjectList..
	// ...so store their object IDs for the debug drawer to find them later.
	g_debugDrawer.iRagdollID.push_back(objectID);
#endif

	currentDBProRagDoll = new DBProRagDoll(objectID);
	currentDBProRagDoll->SetTotalWeight(objTotalWeight);	
}

int BPhys_RagDollGetIDFromBoneObject(int objectID)
{
	return ragdollManager->GetIDFromBoneObject(objectID);
}

int BPhys_RagDollAddBone(int startLimbID, int endLimbID, float diameter, int collisionGroup, int collisionMask)
{
	if(currentDBProRagDoll != NULL)
	{
		if ( startLimbID != -1 && endLimbID != -1 )
		{
			return currentDBProRagDoll->AddBone(currentDBProRagDoll->GetID(), startLimbID, endLimbID, diameter, collisionGroup, collisionMask);
		}
	}
	else
	{
		DBProReportError("You Must Call BPhys_RagDollBegin() before BPhys_RagDollAddBone","Bullet Physics Wrapper");
	}
	return -1;
} 

int BPhys_RagDollAddBone2(int startLimbID, int endLimbID, float diameter, float lengthmod, int collisionGroup, int collisionMask)
{
	if (startLimbID == -1 || endLimbID == -1) return -1;
	if(currentDBProRagDoll != NULL)
	{
		return currentDBProRagDoll->AddBone(currentDBProRagDoll->GetID(), startLimbID, endLimbID, diameter, lengthmod, collisionGroup, collisionMask);
	}
	else
	{
		DBProReportError("You Must Call BPhys_RagDollBegin() before BPhys_RagDollAddBone","Bullet Physics Wrapper");
	}
	return -1;
} 

void BPhys_RagDollBoneAddLimbID(int boneID, int limbID) 
{
	if (limbID == -1) return;
	if(currentDBProRagDoll != NULL)
	{
		currentDBProRagDoll->AssignLimbIDToBone( boneID, limbID);
	}
	else
	{
		DBProReportError("You Must Call BPhys_RagDollBegin() before BPhys_RagDollAddBone","Bullet Physics Wrapper");
	}
}

void BPhys_RagDollAddHingeJoint(int boneAID, int boneBID, int limbID, int jointRotationVec3, int limitsVec2)
{
	if (boneAID == -1 || boneBID == -1 || limbID == -1) return;
	if(currentDBProRagDoll!=NULL)
	{
		DBProToBullet::AssertValidVector(jointRotationVec3, "Parameter Passed To BPhys_RagDollAddHingeJoint Invalid");
		DBProToBullet::AssertValidVector(limitsVec2, "Parameter Passed To BPhys_RagDollAddHingeJoint Invalid");
		currentDBProRagDoll->AddHingeJoint(boneAID, boneBID, limbID, DBProToBullet::GetVector3(jointRotationVec3), 
																		 btScalar(GetXVector2(limitsVec2)), 
																			btScalar(GetYVector2(limitsVec2)));
	}
	else
	{
		DBProReportError("Can not call BPhys_RagDollAddHingeJoint before BPhys_RagDollBegin", "Bullet Physics Wrapper");
	}
}

void BPhys_RagDollAddTwistJoint(int boneAID, int boneBID, int limbID, int jointRotationVec3, int limitsVec3)
{
	if (limbID == -1 || boneBID == -1 || boneAID == -1) return;
	if(currentDBProRagDoll!=NULL)
	{
		DBProToBullet::AssertValidVector(jointRotationVec3, "Parameter Passed To BPhys_RagDollAddTwistJoint Invalid");
		DBProToBullet::AssertValidVector(limitsVec3, "Parameter Passed To BPhys_RagDollAddTwistJoint Invalid");
		currentDBProRagDoll->AddTwistConstraint(boneAID, boneBID, limbID, DBProToBullet::GetVector3(jointRotationVec3), DBProToBullet::GetVector3(limitsVec3));
	}
	else
	{
		DBProReportError("Can not call BPhys_RagDollAddTwistJoint before BPhys_RagDollBegin", "Bullet Physics Wrapper");
	}
}

int BPhys_RagdollGetBoneObjID(int ragdollID, int boneIndex)
{
	DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(ragdollID);
	return ragdoll ? ragdoll->GetBoneObjID(boneIndex) : -1;
}

void BPhys_RagDollEnd()
{
	ragdollManager->AddRagdoll(currentDBProRagDoll);
	currentDBProRagDoll->Finalize();
	currentDBProRagDoll->Activate();
	currentDBProRagDoll = NULL;
}

bool BPhys_RagDollApplyForce(int ragdollID,int iLimbNumber,float fFX,float fFY,float fFZ,float fTX,float fTY,float fTZ,float fForce)
{
	if ( ragdollManager )
	{
		DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(ragdollID);
		if ( ragdoll )
		{
			#ifdef WICKEDENGINE
			// halve the force for MAX - more subtle
			fForce /= 2;
			#endif
			ragdoll->Activate();
			ragdoll->ApplyForce(iLimbNumber,btVector3(fFX,fFY,fFZ),btVector3(fTX,fTY,fTZ),fForce);
			return true;
		}
	}
	return false;
}

int BPhys_RagdollExist(int ragdollID)
{
	if ( ragdollManager )
	{
		return ragdollManager->GetRagdoll(ragdollID) != NULL;
	}
	else
		return 0;
}

int BPhys_DeleteRagdoll(int ragdollID)
{
	if ( ragdollManager )
	{
		DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(ragdollID);
		if(ragdoll)
		{
			ragdollManager->DeleteRagdoll(ragdollID);
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

int BPhys_RagdollIsStatic(int ragdollID)
{
	if ( ragdollManager )
	{
		DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(ragdollID);
		if(ragdoll)
		{
			return ragdoll->IsStatic();
		}
	}
	return 0;
}

int BPhys_RagdollSetStatic(int ragdollID, int isStatic)
{
	if ( ragdollManager )
	{
		DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(ragdollID);
		if(ragdoll)
		{
			if ( isStatic==0 )
				ragdoll->SetStatic(false);
			else
				ragdoll->SetStatic(true);
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

int BPhys_RagDollHideBones(int ragdollID)
{
	if ( ragdollManager )
	{
		DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(ragdollID);
		if(ragdoll)
		{
			ragdoll->HideBones();
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

int BPhys_RagDollShowBones(int ragdollID)
{
	if ( ragdollManager )
	{
		DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(ragdollID);
		if(ragdoll)
		{
			ragdoll->ShowBones();
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

//FHB:Can only call these function before Ragdoll END.
void BPhys_RagDollSetDamping(float linear, float angular)
{
	if(currentDBProRagDoll)
	{
		currentDBProRagDoll->SetDamping(btScalar(linear), btScalar(angular));
	}
}

void BPhys_RagDollSetSleepingThresholds(float linear, float angular)
{
	if(currentDBProRagDoll)
	{
		currentDBProRagDoll->SetSleepingThresholds(btScalar(linear), btScalar(angular));
	}
}

void BPhys_RagDollSetDeactivationTime(float time)
{
	if(currentDBProRagDoll)
	{
		currentDBProRagDoll->SetDeactivationTime(btScalar(time));
	}
}

// Force commands

void ODESetLinearVelocity ( int iObject, float fX, float fY, float fZ )
{
	// get reference object
	sObjectList* pPhyObject = ODEFindID ( iObject );
	if ( pPhyObject==NULL ) return;
	if ( pPhyObject->body==NULL ) return;

	// apply linear velocity
	physicslock.lock();
	pPhyObject->body->activate();
	pPhyObject->body->setLinearVelocity(btVector3(fX/gSc,fY/gSc,fZ/gSc));
	physicslock.unlock();
}

void ODESetLinearVelocityXZWithGravity ( int iObject, float fX, float fZ, float fGravityForce )
{
	// get reference object
	sObjectList* pPhyObject = ODEFindID ( iObject );
	if ( pPhyObject==NULL ) return;
	if ( pPhyObject->body==NULL ) return;

	// apply linear velocity for XZ (kinetic but force for gravity)
	pPhyObject->body->activate();
	btVector3 currentVelocity = pPhyObject->body->getLinearVelocity();
	pPhyObject->body->setLinearVelocity(btVector3(fX/gSc,currentVelocity.getY(),fZ/gSc));
	pPhyObject->body->applyForce(btVector3(0,fGravityForce/gSc,0), btVector3(0,0,0));
}

void ODESetLinearVelocityUsingWorldPosTarget (int iObject, float fWorldX, float fWorldY, float fWorldZ)
{
	// NOTE: changed to direct position setting, velocity did not work at all!

	// get reference object
	sObjectList* pPhyObject = ODEFindID (iObject);
	if (pPhyObject == NULL) return;
	if (pPhyObject->body == NULL) return;

	// work out force from current physics position and world xz
	btTransform trans = pPhyObject->body->getWorldTransform();
	fWorldY += pPhyObject->fRaised;
	fWorldX /= gSc; fWorldY /= gSc; fWorldZ /= gSc;
	trans.setOrigin(btVector3(fWorldX, fWorldY, fWorldZ));
	pPhyObject->body->setWorldTransform(trans);
	pPhyObject->body->activate();
}

void ODEAddBodyForce ( int iObject, float fX, float fY, float fZ, float fPX, float fPY, float fPZ )
{
	// get reference object
	sObjectList* pPhyObject = ODEFindID ( iObject );
	if ( pPhyObject==NULL ) return;
	if ( pPhyObject->body==NULL ) return;

	// apply force
	pPhyObject->body->activate();
	pPhyObject->body->applyForce(btVector3(fX/gSc,fY/gSc,fZ/gSc), btVector3(fPX/gSc,fPY/gSc,fPZ/gSc));
}

void ODESetAngularVelocity (int iObject, float fX, float fY, float fZ)
{
	// get reference object
	sObjectList* pPhyObject = ODEFindID (iObject);
	if (pPhyObject == NULL) return;
	if (pPhyObject->body == NULL) return;

	// apply linear velocity
	pPhyObject->body->activate();
	pPhyObject->body->setAngularVelocity(btVector3(fX / gSc, fY / gSc, fZ / gSc));
}

void ODESetAngularFactor (int iObject)
{
	sObjectList* pPhyObject = ODEFindID (iObject);
	if (pPhyObject == NULL) return;
	if (pPhyObject->body == NULL) return;
	pPhyObject->body->activate();
	pPhyObject->body->setAngularFactor(btVector3(0, 1, 0));
}

void ODEConstrainBodyMotion( int iObjectNumber, float fX, float fY, float fZ )
{
	// get reference object
	sObjectList* pPhyObject = ODEFindID( iObjectNumber );
	if (pPhyObject == NULL) return;
	if (pPhyObject->body == NULL) return;

	pPhyObject->body->activate();
	pPhyObject->body->setLinearFactor( btVector3( fX, fY, fZ ) );
}

void ODEConstrainBodyRotation( int iObjectNumber, float fX, float fY, float fZ )
{
	// get reference object
	sObjectList* pPhyObject = ODEFindID( iObjectNumber );
	if (pPhyObject == NULL) return;
	if (pPhyObject->body == NULL) return;

	pPhyObject->body->activate();
	pPhyObject->body->setAngularFactor( btVector3( fX, fY, fZ ) );
}

void ODERemoveBodyConstraints(int iObjectNumber)
{
	// get reference object
	sObjectList* pPhyObject = ODEFindID( iObjectNumber );
	if (pPhyObject == NULL) return;
	if (pPhyObject->body == NULL) return;

	for ( int i = 0; i < g_dynamicsWorld->getNumConstraints(); i++ )
	{ 
		btTypedConstraint *tC = g_dynamicsWorld->getConstraint( i );
		if ( pPhyObject->body == &( tC->getRigidBodyA() ) ||
			 pPhyObject->body == &( tC->getRigidBodyB() ) )
		{
			g_dynamicsWorld->removeConstraint( tC );
		}
	}
}

void ODERemoveConstraint(int iConstraintId)
{
	btTypedConstraint *tC = g_dynamicsWorld->getConstraint( iConstraintId );
	if ( tC != NULL ) g_dynamicsWorld->removeConstraint( tC );
}

int ODERayTerrainEx (float fX, float fY, float fZ, float fToX, float fToY, float fToZ, int iCollisionType, bool bUpdateDebugObj)
{
	// Cast a ray against the terrain object and see if hit
	int iResult = 0;
	if (g_dynamicsWorld)
	{
		/*
		// show debug if active
		if (g_debugraycastvisual > 0 && bUpdateDebugObj == true)
		{
			if (ObjectExist(g_debugraycastvisual) == 1)
			{
				float fSourceYTweak = fY - 5.0f;
				float fCameraYTweak = fToY - 5.0f;
				float fDX = fToX - fX;
				float fDY = fCameraYTweak - fSourceYTweak;
				float fDZ = fToZ - fZ;
				float fMiddleX = fX + ((fDX) / 2);
				float fMiddleY = fSourceYTweak + ((fDY) / 2);
				float fMiddleZ = fZ + ((fDZ) / 2);
				float fDistance = sqrt(fabs(fDX*fDX) + fabs(fDY*fDY) + fabs(fDZ*fDZ));
				PositionObject(g_debugraycastvisual, fMiddleX, fMiddleY, fMiddleZ);
				PointObject (g_debugraycastvisual, fToX, fCameraYTweak, fToZ);
				ScaleObject (g_debugraycastvisual, 10, 10, fDistance);
			}
		}
		*/

		// do the cast
		btVector3 Start, End, Normal;
		Start = btVector3(fX / gSc, fY / gSc, fZ / gSc);
		End = btVector3(fToX / gSc, fToY / gSc, fToZ / gSc);
		btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
		RayCallback.m_collisionFilterGroup = 1 << 2;
		RayCallback.m_collisionFilterMask = iCollisionType;
		RayCallback.m_fEdgeDetectThreshold = 0; // normal
		g_dynamicsWorld->rayTest(Start, End, RayCallback);
		if (RayCallback.hasHit())
		{
			End = RayCallback.m_hitPointWorld;
			Normal = RayCallback.m_hitNormalWorld;
			g_hitObjectNumber = 0;
			g_hitPointWorld = End * gSc;
			g_hitNormalWorld = Normal;
			iResult = 1;
		}
		else
		{
			iResult = 0;
		}
	}
	else
		iResult = 0;

	// return result
	return iResult;
}

void ODESetBodyScaling(int iObjectNumber, float fX, float fY, float fZ)
{
	// get reference object
	sObjectList* pPhyObject = ODEFindID(iObjectNumber);
	if (pPhyObject == NULL) return;
	if (pPhyObject->body == NULL) return;
	btCollisionShape* shape = pPhyObject->body->getCollisionShape();
	shape->setLocalScaling( btVector3( fX, fY, fZ ) );
	//g_dynamicsWorld->updateSingleAabb((btCollisionObject*)pPhyObject->body); resizing live is UNSTABLE
}

// 1 when the last ODERayTerrain hit a tree rather than the ground
int g_iODERayTerrainHitTree = 0;

int ODERayTerrain ( float fX, float fY, float fZ, float fToX, float fToY, float fToZ, bool bUpdateDebugObj)
{
	// inject tree detection, so can exit early if ray tried to go 'THROUGH' a tree
	extern int physics_rayintersecttree (float fX, float fY, float fZ, float fToX, float fToY, float fToZ);
	g_iODERayTerrainHitTree = 0;
	if (physics_rayintersecttree (fX, fY, fZ, fToX, fToY, fToZ) == 1)
	{
		// report the tree hit point (not the last ray's)
		extern float g_fTreeRayHitX, g_fTreeRayHitY, g_fTreeRayHitZ, g_fTreeRayHitNX, g_fTreeRayHitNY, g_fTreeRayHitNZ;
		g_hitObjectNumber = 0;
		g_hitPointWorld = btVector3(g_fTreeRayHitX, g_fTreeRayHitY, g_fTreeRayHitZ);
		g_hitNormalWorld = btVector3(g_fTreeRayHitNX, g_fTreeRayHitNY, g_fTreeRayHitNZ);
		g_iODERayTerrainHitTree = 1;
		return 1;
	}
	return ODERayTerrainEx ( fX, fY, fZ, fToX, fToY, fToZ, COL_TERRAIN, bUpdateDebugObj);
}

int ODERayForce ( float fX, float fY, float fZ, float fToX, float fToY, float fToZ, float fForceValue )
{
	// Cast a ray, and apply force to what it hits first
	if ( g_dynamicsWorld )
	{
		btVector3 Start, End, Normal;
		Start = btVector3(fX/gSc,fY/gSc,fZ/gSc);
		End = btVector3(fToX/gSc,fToY/gSc,fToZ/gSc);
		btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
		g_dynamicsWorld->rayTest(Start, End, RayCallback);
		if(RayCallback.hasHit())
		{
			End = RayCallback.m_hitPointWorld;
			Normal = RayCallback.m_hitNormalWorld;
			g_hitObjectNumber = 0;
			g_hitPointWorld = End * gSc;
			g_hitNormalWorld = Normal;
			if ( RayCallback.m_collisionObject->getInternalType()==btCollisionObject::CO_RIGID_BODY )
			{
				// apply force to rigid body
				RayCallback.m_collisionObject->activate();
				btRigidBody* pBody = (btRigidBody*)RayCallback.m_collisionObject;
				btVector3 thisPos = pBody->getWorldTransform().getOrigin();
				btVector3 relPos = End - thisPos;
				btVector3 force = (End-Start);
				force = force / force.length() / gSc;
				force = force * (fForceValue/gSc);
				pBody->applyForce(force, relPos);

				// find which object we hit
				sObject* pFound = NULL;
				for (int j=((int)g_PhyObjectList.size())-1; j>=0 ;j--)
				{
					btRigidBody* findbody = g_PhyObjectList[j].body;
					if ( findbody==pBody )
					{
						pFound = GetObjectData ( g_PhyObjectList[j].iID );
						break;
					}
				}
				if ( pFound )
				{
					g_hitObjectNumber = pFound->dwObjectNumber;
				}
			}
			return 1;
		}
		else
			return 0;
	}
	else
		return 0;
}

int ODERayEx (float fX, float fY, float fZ, float fToX, float fToY, float fToZ, int iCollisionType, int iIgnoreObj1, int iIgnoreObj2)
{
	// cast a physics ray, get nearest hit
	if (g_dynamicsWorld)
	{
		btVector3 Start, End, Normal;
		Start = btVector3(fX / gSc, fY / gSc, fZ / gSc);
		End = btVector3(fToX / gSc, fToY / gSc, fToZ / gSc);
		btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
		RayCallback.m_collisionFilterGroup = iCollisionType;
		RayCallback.m_collisionFilterMask = iCollisionType;
		g_dynamicsWorld->rayTest(Start, End, RayCallback); // nice and fast!
		if (RayCallback.hasHit())
		{
			int iHitRegistered = 1;
			g_hitObjectNumber = 0;
			const btRigidBody* hitbody = btRigidBody::upcast(RayCallback.m_collisionObject);
			if (hitbody)
			{
				for (int j = ((int)g_PhyObjectList.size()) - 1; j >= 0; j--)
				{
					btRigidBody* body = g_PhyObjectList[j].body;
					if (body == NULL) continue;
					if (hitbody == body)
					{
						if ((iIgnoreObj1 > 0 && iIgnoreObj1 == g_PhyObjectList[j].iID) || (iIgnoreObj2 > 0 && iIgnoreObj2 == g_PhyObjectList[j].iID))
						{
							// we can ignore this one (added iHitRegistered to ensure ignored hit results in NO detection)
							iHitRegistered = 0;
						}
						else
						{
							// we hit this object
							g_hitObjectNumber = g_PhyObjectList[j].iID;
						}
						break;
					}
				}
			}
			else
			{
				// if ray hits player capsule (ghost) this is also detected!
				g_hitObjectNumber = -1;
			}
			End = RayCallback.m_hitPointWorld;
			Normal = RayCallback.m_hitNormalWorld;
			if (iHitRegistered == 1)
			{
				g_hitPointWorld = End * gSc;
				g_hitNormalWorld = Normal;
			}
			return iHitRegistered;
		}
	}
	return 0;
}

int ODERay (float fX, float fY, float fZ, float fToX, float fToY, float fToZ, int iCollisionType)
{
	return ODERayEx (fX, fY, fZ, fToX, fToY, fToZ, iCollisionType, 0, 0);
}

void ODEGetPosAndNorm(float* pOutX, float* pOutY, float* pOutZ, float* pNormX, float* pNormY, float* pNormZ)
{
	// return last ray hit position and normal
	*pOutX = g_hitPointWorld.getX();
	*pOutY = g_hitPointWorld.getY();
	*pOutZ = g_hitPointWorld.getZ();
	*pNormX = g_hitNormalWorld.getX();
	*pNormY = g_hitNormalWorld.getY();
	*pNormZ = g_hitNormalWorld.getZ();
}

float ODEGetRayCollisionX()
{
	float value = g_hitPointWorld.getX();
	return value;
}

float ODEGetRayCollisionY ( void )
{
	float value = g_hitPointWorld.getY();
	return value;
}

float ODEGetRayCollisionZ ( void )
{
	float value = g_hitPointWorld.getZ();
	return value;
}

float ODEGetRayNormalX	( void )
{
	float value = g_hitNormalWorld.getX();
	return value;
}

float ODEGetRayNormalY	( void )
{
	float value = g_hitNormalWorld.getY();
	return value;
}

float ODEGetRayNormalZ	( void )
{
	float value = g_hitNormalWorld.getZ();
	return value;
}

int ODEGetRayObjectHit ( void )
{
	return g_hitObjectNumber;
}

//
// Set commands
//

void ODESetActive ( int iObjectNumber, int iMode )
{
	sObjectList* pPhyObject = ODEFindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return;
}

void ODESetBodyPosition ( int iObjectNumber, float fX, float fY, float fZ )
{
	sObjectList* pPhyObject = ODEFindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return;

	// change position of physics object
	if ( pPhyObject->body )
	{
		//LB: retain current worldtrans, just adjust origin!
		//btTransform worldTrans;
		//worldTrans.setOrigin(btVector3(fX/gSc,fY/gSc,fZ/gSc));
		//pPhyObject->body->setWorldTransform(worldTrans);
		btTransform trans = pPhyObject->body->getWorldTransform();
		trans.setOrigin(btVector3(fX / gSc, fY / gSc, fZ / gSc));
		pPhyObject->body->setWorldTransform(trans);
		pPhyObject->body->activate();
	}
	else
	{
		// or character if no body (need further qualification here)
		if ( m_character )
		{
			m_character->warp(btVector3(fX/gSc,fY/gSc,fZ/gSc));
		}
	}
}

void ODESetBodyResponse ( int iObjectNumber, int iResponseMode )
{
	sObjectList* pPhyObject = ODEFindID ( iObjectNumber );
	if ( pPhyObject )
	{
		// simply calls setlinearvelocity in physics update each cycle
		pPhyObject->iResponseMode = iResponseMode;
	}
}

void ODESetBodyAngle ( int iObjectNumber, float fXRot, float fYRot, float fZRot )
{
	sObjectList* pPhyObject = ODEFindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return;

	// change position of physics object
	if ( pPhyObject->body )
	{
		btTransform worldTrans;
		worldTrans.setIdentity();
		btVector3 vecPos = pPhyObject->body->getWorldTransform().getOrigin();
		worldTrans.setOrigin(vecPos);
		GGQUATERNION QuatAroundX(1,0,0,fXRot);
		GGQUATERNION QuatAroundY(0,1,0,fYRot);
		GGQUATERNION QuatAroundZ(0,0,1,fZRot);
		GGQuaternionRotationAxis(&QuatAroundX,&GGVECTOR3(1,0,0),fXRot);
		GGQuaternionRotationAxis(&QuatAroundY,&GGVECTOR3(0,1,0),fYRot);
		GGQuaternionRotationAxis(&QuatAroundZ,&GGVECTOR3(0,0,1),fZRot);
		GGQUATERNION finalOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
		worldTrans.setRotation(btQuaternion(finalOrientation.x,finalOrientation.y,finalOrientation.z,finalOrientation.w));
		pPhyObject->body->setWorldTransform(worldTrans);
	}
	else
	{
		// or character if no body (need further qualification here)
		if ( m_character )
		{
			m_character->setWalkDirection(btVector3(0,fYRot,0));
		}
	}
}

void ODESetBodyMass ( int iObject, float fPercentage )
{
	sObjectList* pPhyObject = ODEFindID ( iObject );
	if ( pPhyObject==NULL ) return;
	if ( pPhyObject->body )
	{
		fPercentage=fPercentage/100.0f;
		float fOld = pPhyObject->body->getInvMass() * fPercentage;
		btScalar mass(fOld);
		btVector3 localInertia(0,0,0);
		pPhyObject->body->setMassProps(mass,localInertia);
	}
}

void ODESetBodyFriction ( int iObject, float fFriction )
{
	if ( fFriction == -1 ) fFriction = 90.0f;
	btScalar friction(fFriction);
	friction = friction / 100.0f;
	DBProRagDoll* ragdoll = ragdollManager->GetRagdoll(iObject);
	if ( ragdoll )
	{
		// ragdoll object
		ragdoll->SetFriction ( friction );
	}
	else
	{
		// regular object
		sObjectList* pPhyObject = ODEFindID ( iObject );
		if ( pPhyObject==NULL ) return;
		if ( pPhyObject->body )
		{
			pPhyObject->body->setFriction(friction);
		}
	}
}

void ODESetBodyDamping ( int iObject, float fDamp, float fAngleDamp )
{
	if ( fDamp == - 1 ) fDamp = 0.001f;
	if ( fAngleDamp == - 1 ) fAngleDamp = 0.005f;
	sObjectList* pPhyObject = ODEFindID ( iObject );
	if ( pPhyObject==NULL ) return;
	if ( pPhyObject->body )
	{
		pPhyObject->body->setDamping ( fDamp, fAngleDamp );
	}
}

DWORD ODEGetBodyLinearVelocityX ( int iObjectNumber )
{
	sObjectList* pPhyObject = ODEFindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return 0;
	const float fValue = pPhyObject->body->getLinearVelocity().getX() * gSc;
	return *(DWORD*)&fValue;
}

DWORD ODEGetBodyLinearVelocityY ( int iObjectNumber )
{
	sObjectList* pPhyObject = ODEFindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return 0;
	const float fValue = pPhyObject->body->getLinearVelocity().getY() * gSc;
	return *(DWORD*)&fValue;
}

DWORD ODEGetBodyLinearVelocityZ ( int iObjectNumber )
{
	sObjectList* pPhyObject = ODEFindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return 0;
	const float fValue = pPhyObject->body->getLinearVelocity().getZ() * gSc;
	return *(DWORD*)&fValue;
}

int ODEGetBodyAttribValue ( int iObjectNumber )
{
	sObjectList* pPhyObject = ODEFindID ( iObjectNumber );
	if ( pPhyObject==NULL ) return 0;
	int iValue = (int)pPhyObject->body->getUserPointer();
	return iValue;
}

bool ODEGetBodyIsDynamic(int iObjectNumber)
{
	for (int i = 0; i < (int)g_PhyObjectList.size(); i++)
	{
		if ( g_PhyObjectList[i].iID == iObjectNumber )
		{
			return g_PhyObjectList[i].bDynamicUpdate;
		}
	}
	return false;
}

int	ODEGetBodyNumCollisions( int iObjectNumber )
{
	return GetObjectNumCollisions( iObjectNumber );
}

void ODEGetBodyCollisionDetails( int iObjectNumber, int iColNumber, int &iColObj,
	                             float &fX, float &fY, float &fZ, float &fImpulse )
{
	GetObjectCollisionDetails( iObjectNumber, iColNumber, iColObj, fX, fY, fZ, fImpulse );
}

int	ODEGetTerrainNumCollisions( int iObjectNumber )
{
	return GetTerrainNumCollisions( iObjectNumber );
}

void ODEGetTerrainCollisionDetails( int iObjectNumber, int iColNumber, int &iLatest,
	                               float &fX, float &fY, float &fZ )
{
	GetTerrainCollisionDetails( iObjectNumber, iColNumber, iLatest, fX, fY, fZ );
}

void ODEAddBodyCollisionCheck(int iObjectNumber)
{
	AddObjectCollisionCheck( iObjectNumber );
}

void ODERemoveBodyCollisionCheck(int iObjectNumber)
{
	RemoveObjectCollisionCheck(iObjectNumber);
}

void ODESetWorldGravity ( float fX, float fY, float fZ, float fallspeed)
{
	// set player gravity (assuming 20 is default)
	if ( m_character )
	{
		float fRelative = fabs(fY/20.0f);
		m_character->setGravity((fCharacterGravity*fRelative)/gSc);
		if(fallspeed != 0)
			m_character->setFallSpeed((fallspeed*fRelative) / gSc);
		else
			m_character->setFallSpeed((fCharacterFallSpeed*fRelative)/gSc);
	}
}

void BULLETAnglesFromMatrix ( GGMATRIX* pmatMatrix, GGVECTOR3* pVecAngles )
{
 // from http://www.martinb.com/maths/geometry/rotations/conversions/matrixToEuler/index.htm
 float m00 = pmatMatrix->_11;
 float m01 = pmatMatrix->_12;
 float m02 = pmatMatrix->_13;
 float m12 = pmatMatrix->_23;
 float m22 = pmatMatrix->_33;
 float heading = (float)atan2(m01,m00);
 float attitude = (float)atan2(m12,m22);
 float bank = (float)asin(-m02);

 // check for gimbal lock
 if ( fabs ( m02 ) > 1.0f )
 {
  // looking straight up or down
  float PI = GG_PI / 2.0f;
  pVecAngles->x = 0.0f;
  pVecAngles->y = GGToDegree ( PI * m02 );
  pVecAngles->z = 0.0f;
 }
 else
 {
  pVecAngles->x = GGToDegree ( attitude );
  pVecAngles->y = GGToDegree ( bank );
  pVecAngles->z = GGToDegree ( heading );
 }
}

void ODECreateTerrain ( unsigned int iVertexCount, unsigned int iIndexCount, float* pVertices, int* piIndices )
{
	// MAX does not use this
}

int g_iDebugDrawStatics = 0;

void BPhys_SetDebugDrawerMode(int iMode, int iStatics, int iConstraints)
{
	if (g_iDebugDrawStatics != iStatics)
	{
		g_iDebugDrawStatics = iStatics;
		g_debugDrawer.setDebugMode(0);
	}
	g_debugDrawer.SetDrawTransforms(0);
	if (iMode > 0)
	{
		int mode = btIDebugDraw::DBG_DrawWireframe;
		if (iConstraints)
		{
			mode = btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawConstraints | btIDebugDraw::DBG_DrawConstraintLimits;
		}
		g_debugDrawer.setDebugMode(mode);
	}
	else
	{
		g_debugDrawer.setDebugMode(iMode);
	}
}

int	BPhys_GetDebugDrawerMode()
{
	return g_debugDrawer.getDebugMode();
}

// Called every frame.
float* BPhys_GetDynamicDebugDrawData(int& size)
{
	// Reset index tracker now that the data has been passed on.
	g_debugDrawer.elementCount = 0;
	size = (int) g_debugDrawer.lineData.size();
	if (size != 0)
	{
		return &g_debugDrawer.lineData[0];
	}
	return nullptr;
}

// Should only be called once.
float* BPhys_GetStaticDebugDrawData(int& size)
{
	btRigidBody* rBody = nullptr;
	btTransform transform;

	// Draw the static rigid bodies in the dynamic world.
	if (g_iDebugDrawStatics & 1)
	{
		for (int i = 0; i < g_PhyObjectList.size(); i++)
		{
			//PE: Dont include terrain, will run out of memory
			bool bDrawThisPhysicsObject = false;
			if (g_PhyObjectList[i].iID > 70000)
			{
				if (g_PhyObjectList[i].bDynamicUpdate == false)
				{
					bDrawThisPhysicsObject = true;
				}
			}
			if (g_PhyObjectList[i].iID <= 70000)
			{
				// if in PROXIMITY to an object with physics (terrain), show that too
				int iObj = g_PhyObjectList[i].iID;
				float fDX = ObjectPositionX(iObj) - CameraPositionX(0);
				float fDZ = ObjectPositionZ(iObj) - CameraPositionZ(0);
				float fDiff = sqrt(fabs(fDX * fDX) + fabs(fDZ * fDZ));
				if (g_iDebugDrawStatics & 2)
				{
					if (fDiff < 10000.0f) bDrawThisPhysicsObject = true;
				}
			}
			if (bDrawThisPhysicsObject == true)
			{
				rBody = g_PhyObjectList[i].body;
				if (rBody && rBody->getInvMass() == 0.0f)
				{
					rBody->getMotionState()->getWorldTransform(transform);
					g_dynamicsWorld->debugDrawObject(transform, rBody->getCollisionShape(), btVector3(0, 1, 0));
				}
				rBody = nullptr;
			}
		}

		// Reset index tracker now that the data has been passed on.
		size = (int)g_debugDrawer.lineData.size();
		if (size > 0) return &g_debugDrawer.lineData[0];
	}
	else
	{
		// no static to  create
		size = 0;
		return nullptr;
	}
}

void BPhys_ClearDebugDrawData()
{
	#ifdef WICKEDENGINE
	g_debugDrawer.lineData.clear();
	g_debugDrawer.elementCount = 0;
	g_debugDrawer.iRagdollID.clear();
	g_debugDrawer.iDrawObjects.clear();
	#endif
}

void BPhys_AddDebugSingleObject(int objectID)
{
	#ifdef WICKEDENGINE
	g_debugDrawer.iDrawObjects.push_back(objectID);
	#endif
}

void BPhys_RemoveDebugSingleObject(int objectID)
{
	#ifdef WICKEDENGINE
	for (int i = 0; i < g_debugDrawer.iDrawObjects.size(); i++)
	{
		if (g_debugDrawer.iDrawObjects[i] == objectID) 
		{
			g_debugDrawer.iDrawObjects.erase(g_debugDrawer.iDrawObjects.begin() + i);
			break;
		}
	}
	#endif
}

void BPhys_DrawDebugObjects()
{
	btRigidBody* rBody;
	btTransform transform;
	for (int i = 0; i < g_PhyObjectList.size(); i++)
	{
		for (int j = 0; j < g_debugDrawer.iDrawObjects.size(); j++)
		{
			if (g_debugDrawer.iDrawObjects[j] == g_PhyObjectList[i].iID)
			{
				rBody = nullptr;
				rBody = g_PhyObjectList[i].body;
				if (rBody == nullptr) continue;

				rBody->getMotionState()->getWorldTransform(transform);
				g_dynamicsWorld->debugDrawObject(transform, rBody->getCollisionShape(), btVector3(0, 1, 0));
				
				rBody = nullptr;
			}
		}
	}
}

int BPhys_GetDebugObjectCount()
{
	#ifdef WICKEDENGINE
	return (int) g_debugDrawer.iDrawObjects.size();
	#else
	return 0;
	#endif
}