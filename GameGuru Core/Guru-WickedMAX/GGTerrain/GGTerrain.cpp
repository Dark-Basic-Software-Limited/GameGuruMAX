
#define PEOPTIMIZING
#define TERRAINTHREADSAFE
#define ONLYLOADWHENUSED

#include <string>
#include "Utility/stb_image.h"
#include "CFileC.h"
#include "CStr.h"
#include "wiRenderer.h"
#include "wiProfiler.h"
#include "SimplexNoise.h"
#include "wiECS.h"
#include "Utility/tinyddsloader.h"

#include "master.h"

#include "..\GameGuru\Imgui\imgui.h"

// redefines MAX_PATH to 1050
#include "preprocessor-moreflags.h"

#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

// Externl for debugging
extern void timestampactivity (int i, char* desc_s);

#include "GGThread.h"
using namespace GGThread;
#include "GGTerrain.h"
#include "GGTerrainPageSettings.h"
#include "JSONElement.h"

#include "DirectXTex.h"
#include "..\..\..\..\GameGuru\Imgui\imgui_gg_dx11.h"
#include "M-UndoSys-Terrain.h"

//#define GGTERRAIN_ENABLE_PBR_SPHERES

using namespace wiGraphics;
using namespace wiScene;

void WickedCall_UpdateProbes(void);

#include "GGTerrainDataStructures.h"

#include "GGTrees.h"
using namespace GGTrees;

#include "GGGrass.h"
using namespace GGGrass;

#include "LinearMath/btVector3.h"
#include "BulletCollision/CollisionShapes/btTriangleCallback.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

// includes MACRO for crash logging
#include "wickedcalls.h"

#define PI 3.14159265358979f

#define GGKEY_BACK		0x08
#define GGKEY_TAB		0x09
#define GGKEY_ENTER		0x0D
#define GGKEY_SHIFT		0x10
#define GGKEY_CONTROL	0x11
#define GGKEY_ALT		0x12
#define GGKEY_ESCAPE	0x1B
#define GGKEY_SPACE		0x20
#define GGKEY_PAGEUP	0x21
#define GGKEY_PAGEDOWN	0x22
#define GGKEY_END		0x23
#define GGKEY_HOME		0x24
#define GGKEY_LEFT		0x25
#define GGKEY_UP		0x26
#define GGKEY_RIGHT		0x27
#define GGKEY_DOWN		0x28
#define GGKEY_INSERT	0x2D
#define GGKEY_DELETE	0x2E

#define GGKEY_0			0x30
#define GGKEY_1			0x31
#define GGKEY_2			0x32
#define GGKEY_3			0x33
#define GGKEY_4			0x34
#define GGKEY_5			0x35
#define GGKEY_6			0x36
#define GGKEY_7			0x37
#define GGKEY_8			0x38
#define GGKEY_9			0x39

#define GGKEY_A			0x41
#define GGKEY_B			0x42
#define GGKEY_C			0x43
#define GGKEY_D			0x44
#define GGKEY_E			0x45
#define GGKEY_F			0x46
#define GGKEY_G			0x47
#define GGKEY_H			0x48
#define GGKEY_I			0x49
#define GGKEY_J			0x4A
#define GGKEY_K			0x4B
#define GGKEY_L			0x4C
#define GGKEY_M			0x4D
#define GGKEY_N			0x4E
#define GGKEY_O			0x4F
#define GGKEY_P			0x50
#define GGKEY_Q			0x51
#define GGKEY_R			0x52
#define GGKEY_S			0x53
#define GGKEY_T			0x54
#define GGKEY_U			0x55
#define GGKEY_V			0x56
#define GGKEY_W			0x57
#define GGKEY_X			0x58
#define GGKEY_Y			0x59
#define GGKEY_Z			0x5A

#define GGKEY_F1		0x70
#define GGKEY_F2		0x71
#define GGKEY_F3		0x72
#define GGKEY_F4		0x73
#define GGKEY_F5		0x74
#define GGKEY_F6		0x75
#define GGKEY_F7		0x76
#define GGKEY_F8		0x77

//extern std::string FinalGameGuruMaxFolder;
#ifdef ONLYLOADWHENUSED
bool bCheckForNewTerrainTextures = false;
#endif

extern wiECS::Entity g_entitySunLight;
extern bool bImGuiGotFocus;
extern bool bImGuiInTestGame;
extern preferences pref;
extern ImVec4 vLastTerrainPickPosition;
extern ImVec4 vLastRampTerrainPickPosition;
//extern bool g_bSculptingRequiresNewPhysics;
extern bool g_bModifiedThisTerrainGrid[21][21];
extern int g_iCalculatingChangeBounds;
extern uint8_t* g_pTerrainSnapshot;
extern TerrainEditsBB g_EditBounds;
extern UndoRedoMemory g_TerrainUndoMem;
extern UndoRedoMemory g_TerrainRedoMem;
extern bool bEnableTerrainChunkCulling;
void GGTerrain_CreateUndoRedoAction(int type, int eList, bool bUserAction = true, void* pEventData = nullptr);
int Get_Spray_Mode_On(void);
void DrawDot(char* text, float x, float y, float z);
const char* pestrcasestr(const char* arg1, const char* arg2);
void GetConvertSettings(int* maxwidth, int* active);

#ifdef CUSTOMTEXTURES
extern int ConvertDDSCompressedFormat(ID3D11Device* device, char* sourceFile, DXGI_FORMAT newFormat, int newWidth, int newHeight, char* outputFile);
#endif
bool g_bOneTimeMessage = false;
uint32_t iOccludedTerrainChunks = 0;
int OCCLODSTART = 6;
int ggterrain_update_enabled = 1;
int ggterrain_safe_gpu_singlethread = 0; // 1 = build terrain GPU buffers on main thread only (fixes intermittent DEVICE LOST)

extern bool bTriggerMessage;
extern char cTriggerMessage[MAX_PATH];

#define GGTERRAIN_HEIGHTMAP_EDIT_SIZE 4096
#define GGTERRAIN_MATERIALMAP_SIZE 4096

#define GGTERRAIN_ENV_PROBE_RANGE_METERS 100
#define GGTERRAIN_ENV_PROBE_UPDATE_METERS 3

//externs
extern int g_iDeferTextureUpdateToNow;
extern cstr g_DeferTextureUpdateMAXRootFolder_s;
extern cstr g_DeferTextureUpdateCurrentFolder_s;
extern std::vector<std::string> g_DeferTextureUpdate;
extern std::vector<int> g_DeferTextureUpdateIncompatibleTextures;

namespace GGTerrain
{

#define LOCALENVPROBECOUNT 8
std::vector<envProbeItem> g_envProbeList;
int g_iEnvProbeTracking[LOCALENVPROBECOUNT] = { 0 };
bool g_bEnvProbeTrackingUpdate[LOCALENVPROBECOUNT] = { false };

float* pHeightMapEdit = 0;
uint8_t* pHeightMapEditType = 0;

uint16_t* pHeightMapFlatAreas = 0;
uint8_t* pHeightMapFlatAreaWeight = 0;

wiECS::Entity globalEnvProbe;
XMFLOAT3 globalEnvProbePos = { 0, 0, 0 };
wiECS::Entity localEnvProbe[LOCALENVPROBECOUNT];
XMFLOAT3 localEnvProbePos[LOCALENVPROBECOUNT];
uint32_t currLocalEnvProbe = 0;
uint32_t localEnvProbeTransition = 0;

bool ggterrain_key_state[ 256 ] = { false };
bool ggterrain_key_pressed[ 256 ] = { false };
bool ggterrain_key_released[ 256 ] = { false };

bool GGTerrain_GetKeyPressed( uint8_t key ) { return ggterrain_key_pressed[key]; }
bool GGTerrain_GetKeyReleased( uint8_t key ) { return ggterrain_key_released[key]; }
bool GGTerrain_GetKeyState( uint8_t key ) { return ggterrain_key_state[key]; }

struct DDSRequirements
{
	int width = 2048;
	int height = 2048;
	int mip = 0;
	DXGI_FORMAT format = DXGI_FORMAT_BC7_UNORM;
};
DDSRequirements colorDDS;
DDSRequirements normalDDS;
DDSRequirements surfaceDDS;


void GGTerrain_CheckKeys()
{
	ImGuiIO& io = ImGui::GetIO(); 

	for( int i = 0; i < 256; i++ )
	{
		ggterrain_key_pressed[i] = (io.KeysDown[i] && !ggterrain_key_state[i]);
		ggterrain_key_released[i] = (!io.KeysDown[i] && ggterrain_key_state[i]);
		ggterrain_key_state[i] = io.KeysDown[i];
	}
}

int GetFileExists( const char* filename ) 
{ 
	char fullPath[ MAX_PATH ];
	strcpy_s( fullPath, MAX_PATH, filename );
	GG_GetRealPath( fullPath, 0 );
	FILE *test = 0;
	int err = fopen_s( &test, fullPath, "rb" );
	if ( err == 0 )
	{
		fclose( test );
		return 1;
	}

	return 0; 
}

uint32_t iRandSeed = 0;
void RandomInit()
{
	int64_t i64CurrentTime;
	QueryPerformanceCounter ( (LARGE_INTEGER*) &i64CurrentTime );
	iRandSeed = (uint32_t) i64CurrentTime;
}

void RandomSeed( uint32_t seed )
{
	iRandSeed = seed;
}

// produces a number between 0 and 65535
uint32_t Random() 
{ 
	// Uses integer overflow to generate pseudo random numbers.
	iRandSeed = (214013*iRandSeed + 2531011);
	// only use the top 16 bits as the lower 16 bits produce very short repeat cycles.
	return (iRandSeed >> 16) & 0xffff;
	return 0; 
}

uint32_t Random( uint32_t from, uint32_t to ) 
{ 
	if ( to > 65535 ) to = 65535;
	if ( from > to ) from = 0;
	int diff = (to-from)+1;
	return (Random() % diff) + from;
}

int iRandMTIndex = 0;
uint32_t iRandMTArray[ 624 ];
void Random2Init()
{
	int64_t i64CurrentTime;
	QueryPerformanceCounter ( (LARGE_INTEGER*) &i64CurrentTime );

	iRandMTIndex = 0;
	iRandMTArray[0] = (uint32_t) i64CurrentTime;
	for ( int i = 1; i < 623; i++ )
	{
		iRandMTArray[i] = (1812433253 * (iRandMTArray[i-1] ^ (iRandMTArray[i-1] >> 30)) + i);
	}
}

void Random2Seed( uint32_t seed )
{
	iRandMTIndex = 0;
	iRandMTArray[0] = seed;
	for ( int i = 1; i < 623; i++ )
	{
		iRandMTArray[i] = (1812433253 * (iRandMTArray[i-1] ^ (iRandMTArray[i-1] >> 30)) + i);
	}
}

// produces a number between 0 and 2^32 - 1
uint32_t Random2() 
{ 
	if ( iRandMTIndex == 0 ) 
	{
		for ( int i = 0; i < 624; i++ ) 
		{
			uint32_t y = (iRandMTArray[i] & 0x80000000) + (iRandMTArray[(i+1) % 624] & 0x7fffffff);
			iRandMTArray[i] = iRandMTArray[(i + 397) % 624] ^ (y>>1);
			if ( (y % 2) != 0 ) iRandMTArray[i] = iRandMTArray[i] ^ 0x9908b0df;
		}
	}

	uint32_t y = iRandMTArray[iRandMTIndex];
	y = y ^ (y >> 11);
	y = y ^ ((y << 7) & 0x9d2c5680);
	y = y ^ ((y << 15) & 0xefc60000);
	y = y ^ (y >> 18);

	iRandMTIndex = (iRandMTIndex + 1) % 624;
	return y;
} 

uint32_t Random2( uint32_t from, uint32_t to ) 
{
	if ( from == to ) return from;

	if ( from > to ) 
	{
		int temp = from;
		from = to;
		to = temp;
	}

	uint32_t diff = (to-from)+1;
	return (Random2() % diff) + from;
}

float RandomFloat() 
{
	return Random2() / 4294967296.0f;
}

// page table structures
class PageNeeded
{
public:
	uint32_t identifier; // [0-7]=virtLocationX, [8-15]=virtLocationY, [16-20]=detailLevel
	uint32_t requestedCount; // [0-30]=requestedCount, [31]=isResident	
	
	uint32_t GetDetailLevel() { return (identifier >> 16) - 1; }
	uint32_t GetVirtOffsetX() { return identifier & 0xFF; }
	uint32_t GetVirtOffsetY() { return (identifier >> 8) & 0xFF; }

	void Setup( uint32_t value )
	{
		requestedCount = 1; // start requested count at 1 and not resident
		identifier = value; 
	}

	uint32_t GetRequestedCount() { return requestedCount & 0x7FFFFFFF; }
	bool isResident() { return (requestedCount >> 31) != 0; }
	void SetResident() { requestedCount |= 0x80000000; }

	PageNeeded() {}
	~PageNeeded() {}
};

#define GGTERRAIN_PAGE_QUEUE1_REGENERATE  0x01
#define GGTERRAIN_PAGE_QUEUE2_REGENERATE  0x02
#define GGTERRAIN_PAGE_WILL_REGENERATE    0x04
#define GGTERRAIN_PAGE_QUEUE3_REGENERATE  0x08
#define GGTERRAIN_PAGE_QUEUE_ALL  (GGTERRAIN_PAGE_QUEUE1_REGENERATE | GGTERRAIN_PAGE_QUEUE2_REGENERATE | GGTERRAIN_PAGE_QUEUE3_REGENERATE | GGTERRAIN_PAGE_WILL_REGENERATE)

class PageEntry;
OrderedArray<PageEntry*> pageRefreshList;

class PageEntry
{
public:
	uint32_t identifier; // [0-7]=virtLocationX, [8-15]=virtLocationY, [16-20]=detailLevel
	union
	{
		struct
		{
			uint16_t invRequestedCount; // more highly requested pages should have a lower value to make them sort lower in the eviction check
			uint16_t unusedCount; // high 16-bits
		};
		uint32_t evictionCheck;
	};
	uint16_t physLocation; // [0-7]=physX, [8-15]=physY
	uint16_t flags = 0;
		
	uint32_t GetDetailLevel() { return (identifier >> 16) - 1; } // 0 is the highest detail level
	uint32_t GetVirtOffsetX() { return identifier & 0xFF; }
	uint32_t GetVirtOffsetY() { return (identifier >> 8) & 0xFF; }
	void SetVirtLocation( uint32_t x, uint32_t y ) 
	{
		assert( x < 256 );
		assert( y < 256 );
		identifier &= 0xFFFF0000; // clear old virtual location but keep mip level
		identifier |= (y << 8);
		identifier |= x;
	}

	void SetRequestedCount( uint32_t count ) 
	{ 
		if ( count > 65535 ) count = 65535;
		invRequestedCount = 65535 - count;
	}
	uint32_t GetRequestedCount() { return 65535 - invRequestedCount; }

	uint32_t GetPhysOffsetX() { return physLocation & 0xFF; }
	uint32_t GetPhysOffsetY() { return (physLocation >> 8) & 0xFF; }
	void SetPhysLocation( uint32_t x, uint32_t y ) { physLocation = (y << 8) | x; }
	
	void Setup( uint32_t value )
	{
		if ( flags & GGTERRAIN_PAGE_QUEUE_ALL )
		{
			for( int i = 0; i < (int)pageRefreshList.NumItems(); i++ )
			{
				if ( pageRefreshList[ i ] == this )
				{
					pageRefreshList.RemoveIndex( i );
					break;
				}
			}
		}

		// clear everything except the physical location
		evictionCheck = 0;
		identifier = value;
		flags = 0;
	}
	bool ShouldRegenerate() 
	{
		return (flags & GGTERRAIN_PAGE_WILL_REGENERATE) != 0;
	}

	void Regenerated()
	{
		flags &= ~(GGTERRAIN_PAGE_QUEUE_ALL);
	}

	void AdvanceRegenerateQueue()
	{
		if ( flags & GGTERRAIN_PAGE_QUEUE1_REGENERATE )
		{
			flags = (flags & ~GGTERRAIN_PAGE_QUEUE1_REGENERATE) | GGTERRAIN_PAGE_QUEUE2_REGENERATE;
		}
		else if ( flags & GGTERRAIN_PAGE_QUEUE2_REGENERATE )
		{
			flags = (flags & ~GGTERRAIN_PAGE_QUEUE2_REGENERATE) | GGTERRAIN_PAGE_QUEUE3_REGENERATE;
		}
		else if ( flags & GGTERRAIN_PAGE_QUEUE3_REGENERATE )
		{
			flags = (flags & ~GGTERRAIN_PAGE_QUEUE3_REGENERATE) | GGTERRAIN_PAGE_WILL_REGENERATE;
		}
	}

	void SetToRegenerate()
	{
		if ( (flags & GGTERRAIN_PAGE_QUEUE_ALL) == 0 ) 
		{
			pageRefreshList.AddItem( this );
			flags |= GGTERRAIN_PAGE_QUEUE1_REGENERATE;
		}
	}

	void SetToRegenerateImmediate()
	{
		if ( (flags & GGTERRAIN_PAGE_QUEUE_ALL) == 0 ) 
		{
			pageRefreshList.AddItem( this );
			flags |= GGTERRAIN_PAGE_WILL_REGENERATE;
		}
		else
		{
			flags = (flags & ~GGTERRAIN_PAGE_QUEUE_ALL) | GGTERRAIN_PAGE_WILL_REGENERATE;
		}
	}

	PageEntry() {}
	~PageEntry() {}
};

class PageNeededArray
{
	public:
		PageNeededArray() {}

		~PageNeededArray() 
		{
			if ( m_pPages ) delete [] m_pPages;
			if ( m_pPageIndex ) delete [] m_pPageIndex;
			if ( m_pDataBuffer ) delete [] m_pDataBuffer;
		}
		
		void Setup( uint32_t width, uint32_t height ) 
		{ 
			m_iDataSize = 0;
			memset( m_pBucketCounts1, 0, sizeof(m_pBucketCounts1) );
			memset( m_pBucketCounts2, 0, sizeof(m_pBucketCounts2) );

			if ( width == m_currWidth && height == m_currHeight ) return;

			if ( m_pDataBuffer ) delete [] m_pDataBuffer;
			if ( m_pDataBuffer2 ) delete [] m_pDataBuffer2;

			uint32_t newSize = width * height * sizeof(uint32_t);
			m_pDataBuffer = new uint32_t[ newSize ];
			m_pDataBuffer2 = new uint32_t[ newSize ];

			m_currWidth = width;
			m_currHeight = height;
		}
		
		void AddNeededPage( uint32_t identifier )
		{
			assert( identifier <= 0x3FFFFF ); // 22 bits
			m_pDataBuffer[ m_iDataSize++ ] = identifier;
			uint32_t bucket1 = identifier & 0x7FF; // 11 bits
			uint32_t bucket2 = (identifier >> 11) & 0x7FF;
			m_pBucketCounts1[ bucket1 + 1 ]++;
			m_pBucketCounts2[ bucket2 + 1 ]++;
		}

		void Sort()
		{
			// first sort by identifier, then create the page array, then the page index 
			
			// acculmulate individual totals into cumulative totals
			for( uint32_t i = 1; i < 2048; i++ )
			{
				m_pBucketCounts1[ i ] += m_pBucketCounts1[ i - 1 ];
				m_pBucketCounts2[ i ] += m_pBucketCounts2[ i - 1 ];
			}

			// pass 1
			for ( uint32_t i = 0; i < m_iDataSize; i++ )
			{
				uint32_t value = m_pDataBuffer[ i ] & 0x7FF;
				uint32_t index = m_pBucketCounts1[ value ];
				m_pDataBuffer2[ index ] = m_pDataBuffer[ i ];
				++m_pBucketCounts1[ value ];
			}

			// pass 2
			for ( uint32_t i = 0; i < m_iDataSize; i++ )
			{
				uint32_t value = (m_pDataBuffer2[ i ] >> 11) & 0x7FF;
				uint32_t index = m_pBucketCounts2[ value ];
				m_pDataBuffer[ index ] = m_pDataBuffer2[ i ];
				++m_pBucketCounts2[ value ];
			}

			memset( m_pBucketCounts1, 0, sizeof(m_pBucketCounts1) );
			memset( m_pBucketCounts2, 0, sizeof(m_pBucketCounts2) );

			// count unique pages
			m_numPages = 0;
			uint32_t prevIdentifier = 0xFFFFFFFF;
			for ( uint32_t i = 0; i < m_iDataSize; i++ )
			{
				if ( m_pDataBuffer[ i ] != prevIdentifier ) m_numPages++;
				prevIdentifier = m_pDataBuffer[ i ];
			}

			if ( m_numPages == 0 ) return;
			if ( m_numPages > 0x10000 ) m_numPages = 0x10000; // should never have more than 65536 pages, in reality around 2000, but just in case

			if ( m_numPages > m_pageArraySize )
			{
				if ( m_pPages ) delete [] m_pPages;
				if ( m_pPageIndex ) delete [] m_pPageIndex;

				m_pPages = new PageNeeded[ m_numPages ];
				m_pPageIndex = new uint16_t[ m_numPages ];
				m_pageArraySize = m_numPages;
			}

			// populate page array
			m_numPages = 0;
			prevIdentifier = 0xFFFFFFFF;
			for ( uint32_t i = 0; i < m_iDataSize; i++ )
			{
				uint32_t identifier = m_pDataBuffer[ i ];
				if ( identifier != prevIdentifier ) 
				{
					if ( m_numPages > 0 )
					{
						uint32_t requestedCount = m_pPages[ m_numPages-1 ].GetRequestedCount();
						uint32_t bucket1 = requestedCount & 0x3FF; // 10 bits
						uint32_t bucket2 = (requestedCount >> 10) & 0x3FF;
						m_pBucketCounts1[ bucket1 + 1 ]++;
						m_pBucketCounts2[ bucket2 + 1 ]++;
					}

					m_pPages[ m_numPages ].Setup( identifier );
					m_numPages++;
					if ( m_numPages >= 0x10000 ) break;
				}
				else
				{
					m_pPages[ m_numPages-1 ].requestedCount += (m_pPages[ m_numPages-1 ].GetDetailLevel() + 1);
				}
				prevIdentifier = identifier;
			}

			uint32_t requestedCount = m_pPages[ m_numPages-1 ].GetRequestedCount();
			uint32_t bucket1 = requestedCount & 0x3FF; // 10 bits
			uint32_t bucket2 = (requestedCount >> 10) & 0x3FF;
			m_pBucketCounts1[ bucket1 + 1 ]++;
			m_pBucketCounts2[ bucket2 + 1 ]++;

			// acculmulate individual totals into cumulative totals
			for( uint32_t i = 1; i < 1024; i++ )
			{
				m_pBucketCounts1[ i ] += m_pBucketCounts1[ i - 1 ];
				m_pBucketCounts2[ i ] += m_pBucketCounts2[ i - 1 ];
			}

			// create page index that is sorted by requested count
			uint16_t* pTempBuffer = (uint16_t*) m_pDataBuffer2; // guaranteed to be big enough for what we need

			// pass 1
			for ( uint32_t i = 0; i < m_numPages; i++ )
			{
				uint32_t requestedCount = m_pPages[ i ].GetRequestedCount();
				uint32_t value = requestedCount & 0x3FF;
				uint32_t index = m_pBucketCounts1[ value ];
				pTempBuffer[ index ] = i;
				++m_pBucketCounts1[ value ];
			}

			// pass 2
			for ( uint32_t i = 0; i < m_numPages; i++ )
			{
				uint16_t index = pTempBuffer[ i ];
				uint32_t requestedCount = m_pPages[ index ].GetRequestedCount();
				uint32_t value = (requestedCount >> 10) & 0x3FF;
				uint32_t newIndex = m_pBucketCounts2[ value ];
				m_pPageIndex[ newIndex ] = index;
				++m_pBucketCounts2[ value ];
			}
		}
		
		uint32_t NumItems() { return m_numPages; }

		PageNeeded* GetItem( uint32_t identifier ) 
		{ 
			if ( m_numPages == 0 ) return 0;

			int high = m_numPages - 1;
			int low = 0;
			int mid;

			// binary search
			while( high >= low )
			{
				mid = (high + low) / 2;
				PageNeeded* pPage = &m_pPages[ mid ];

				if( pPage->identifier > identifier ) high = mid-1;
				else if ( pPage->identifier < identifier ) low = mid+1;
				else return pPage;
			}

			return 0;
		}

		int GetNewPages( PageNeeded** pageList, uint32_t maxPages )
		{
			assert( maxPages > 0 );
			if ( m_numPages == 0 ) return 0;

			// m_pPageIndex is in ascending order and we want the highest values
			uint32_t count = 0;
			for( int i = (int)m_numPages-1; i >= 0; i-- )
			{
				uint16_t index = m_pPageIndex[ i ];
				PageNeeded* pPage = &m_pPages[ index ];

				if ( pPage->isResident() ) continue;

				pageList[ count++ ] = pPage;
				if ( count >= maxPages ) break;
			}

			return count;
		}

		int GetNumNewPages()
		{
			uint32_t count = 0;
			for( uint32_t i = 0; i < m_numPages; i++ )
			{
				if ( !m_pPages[ i ].isResident() ) count++;
			}	
			return count;
		}
				
	protected:
		PageNeeded* m_pPages = 0; // sorted by identifier
		uint16_t* m_pPageIndex = 0; // sorted by requested count
		uint32_t m_numPages = 0;
		uint32_t m_pageArraySize = 0;
		
		// variables used for sorting
		uint32_t m_currWidth = 0;
		uint32_t m_currHeight = 0;
		uint32_t m_iDataSize = 0;
		uint32_t* m_pDataBuffer = 0;
		uint32_t* m_pDataBuffer2 = 0;
		uint32_t m_pBucketCounts1[ 2049 ];
		uint32_t m_pBucketCounts2[ 2049 ];
};

// vertex format
struct TerrainVertex
{
    float x, y, z;
	uint32_t inormal;
	uint32_t ID;
};

#define GGTERRAIN_CHUNK_STITCH_XNEG    0x01
#define GGTERRAIN_CHUNK_STITCH_XPOS    0x02
#define GGTERRAIN_CHUNK_STITCH_ZNEG    0x04
#define GGTERRAIN_CHUNK_STITCH_ZPOS    0x08

#define GGTERRAIN_CHUNK_VISIBLE          0x01
#define GGTERRAIN_CHUNK_REGENERATE       0x02
#define GGTERRAIN_CHUNK_COPIED           0x04
#define GGTERRAIN_CHUNK_FUTURE_VISIBLE   0x08

GGTerrainExtraParams ggterrain_extra_params; // Carefully modify this one anywhere, used in back and fourth communication
GGTerrainParams ggterrain_global_params; // Modify this one anywhere
GGTerrainRenderParams ggterrain_global_render_params; // Modify this one anywhere
GGTerrainRenderParams2 ggterrain_global_render_params2; // Modify this one anywhere

GGTerrainParams ggterrain_local_params; // Do not modify these values outside this file
GGTerrainRenderParams ggterrain_local_render_params; // Do not modify these values outside this file
GGTerrainRenderParams2 ggterrain_local_render_params2; // Do not modify these values outside this file
GGTerrainInternalParams ggterrain_internal_params; // Do not modify these values outside this file

#define GGTERRAIN_MAX_FLAT_AREAS   0xFFFF

uint32_t ggterrain_flat_areas_array_size = 0;
GGTerrainFlatArea* ggterrain_flat_areas = 0;
StackArray<uint32_t> ggterrain_flat_areas_free;

SimplexNoise noise( 1, 1, 2, 0.5 );

// rendering variables
RasterizerState rastState;
DepthStencilState depthStateOpaque;
BlendState blendStateOpaque;
InputLayout inputLayout;
PipelineState psoMain;
PipelineState psoMainWire;
PipelineState psoMainPrepass;
PipelineState psoMainShadow;
PipelineState psoReflectionPrepass;
PipelineState psoEditCube;
PipelineState psoEnvProbe;
PipelineState psoRamp;

GPUBuffer instanceBuffer;

// shader constants
#define GGTERRAIN_CONSTANTS_FULL_DECL
#include "Shaders/GGTerrainConstants.hlsli"

TerrainCB terrainConstantData = {};
GPUBuffer terrainConstantBuffer;

Shader shaderMainVS;
Shader shaderMainPS;
Shader shaderMainVirtualPS;
Shader shaderMainPrepassVS;
Shader shaderMainPrepassPS;
Shader shaderReflectionPrepassVS;
Shader shaderReadBackMSCS;
Shader shaderReadBackCS;
Shader shaderMainShadowVS;
Shader shaderEditCubeVS;
Shader shaderEditCubePS;
Shader shaderEnvProbeVS;
Shader shaderEnvProbePS;
Shader shaderRampVS;
Shader shaderRampPS;

Texture texLODHeightMapArray;
Texture texLODNormalMapArray;

// texture arrays for terrain source textures
Texture texColorArray;
Texture texNormalsArray;
#ifdef GGTERRAIN_USE_SURFACE_TEXTURE
	Texture texSurfaceArray;
#else
	Texture texRoughnessMetalnessArray;
	Texture texAOArray;
#endif

// virtual texture variables
// assuming 32768 x 32768 virtual texture per LOD (4k x 4k per chunk) with 128 x 128 pixel pages
// page table texture = 256 x 256 pages per LOD
Texture texPageTableArray; // LOD-1 levels with 1 mip per level in R16_UINT format, [0-3]=miplevel(0-15), [4-9]=offsetU(0-63), [10-15]=offsetV(0-63)
Texture texPageTableFinal; // single level with 7 mip levels, not a full mip stack, stops at 4x4 since the lowest levels aren't very useful

Texture texMask;

uint8_t* pMaterialMap = 0;
Texture texMaterialMap;

// CPU copy of page table data, useful when shifting the page table
//#define GGTERRAIN_PAGE_TABLE_DEPTH 32 // must be greater than max numLODLevels + max mip levels (currently 15 + 7)
#define GGTERRAIN_PAGE_TABLE_DEPTH 22 //PE: We use 9+7 on lowest so reduce it to save VRAM, system only support max 15+7 anyway.
uint16_t pageTableData[ GGTERRAIN_PAGE_TABLE_DEPTH ][ pagesX * pagesY ];

Texture texPagesColorAndMetal; // R8G8B8A8
Texture texPagesNormalsRoughnessAO; // R8G8 = normals, B8 = roughness, A8 = ambient occlusion
Texture texReadBackCompute;
#define NUM_READ_BACK_TEXTURES 3 // must be at least 2 to avoid GPU stalls, 3 seems to be safest
Texture texReadBackStaging[ NUM_READ_BACK_TEXTURES ];
//uint32_t currReadBackTex = 0;
static std::atomic<uint32_t> g_currReadBackTex{ 0 };
static constexpr uint32_t READBACK_INVALID = 0xFFFFFFFFu;
std::atomic<uint32_t> g_lastReadBackWrittenIndex{ READBACK_INVALID };
static std::atomic<uint32_t> g_lastQueuedReadbackIndex{ READBACK_INVALID }; // slot we most recently queued a copy+query for
static ID3D11Query* g_readbackTimestampQuery[NUM_READ_BACK_TEXTURES] = {};
static ID3D11Query* g_readbackDisjointQuery = nullptr;

// page generator variables
RenderPass renderPassPhysicalTex;
RenderPass renderPassPhysicalTexMip;
UnorderedArray<PageEntry*> pageGenerationList;

struct VertexPageGen
{
    float x, y;
	float u, v; // UV for texture tiling
	float u2, v2; // UV for height and normal map data
	float worldX, worldY;
	uint32_t id;
};

//PE: GGTERRAIN_REPLACEMENT_PAGE_MAX 256 changed to 128 for lower VRAM and faster processing.
//PE: A normal rotation 180 degress normally only generates around 64 pages so should be fine.
#define GGTERRAIN_REPLACEMENT_PAGE_MAX 128
#define GGTERRAIN_EVICTION_PAGE_MAX 64
#if GGTERRAIN_EVICTION_PAGE_MAX > GGTERRAIN_REPLACEMENT_PAGE_MAX
	#error GGTERRAIN_EVICTION_PAGE_MAX must be less than or equal to GGTERRAIN_REPLACEMENT_PAGE_MAX
#endif
VertexPageGen g_VerticesPageGen[ GGTERRAIN_REPLACEMENT_PAGE_MAX ][ 6 ] = { 0 };
GPUBuffer pageGenVertexBuffer;
InputLayout pageGenInputLayout;
Shader shaderPageGenVS;
Shader shaderPageGenPS;
PipelineState psoPageGen;

// list of pages needed from the last read back frame
PageNeededArray pagesNeeded;
int GGTerrain_GetPagesNeeded() { return pagesNeeded.GetNumNewPages(); }
int GGTerrain_GetPagesActive() { return pagesNeeded.NumItems(); }
int GGTerrain_GetPagesRefreshNeeded() { return pageRefreshList.NumItems(); }

int GGTerrain_GetDebugValue() { return 0; }

int GGTerrain_GetEditSizeVisible() { return (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE) ? 1 : 0; }
void GGTerrain_SetEditSizeVisible( int visible )
{
	if ( visible ) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
	else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
}

int GGTerrain_GetEditSizeVisible3D() { return (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D) ? 1 : 0; }
void GGTerrain_SetEditSizeVisible3D( int visible )
{
	if ( visible ) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
	else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
}

int GGTerrain_GetMiniMapVisible() { return (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP) ? 1 : 0; }
void GGTerrain_SetMiniMapVisible( int visible )
{
	if ( visible ) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP;
	else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP;
}

int GGTerrain_GetGenerateTerrainOutsideHeightMap() { return (ggterrain_global_params.fractal_flags & GGTERRAIN_FRACTAL_OUTSIDE_HEIGHTMAP) ? 1 : 0; }
void GGTerrain_SetGenerateTerrainOutsideHeightMap( int generate )
{
	if ( generate ) ggterrain_global_params.fractal_flags |= GGTERRAIN_FRACTAL_OUTSIDE_HEIGHTMAP;
	else ggterrain_global_params.fractal_flags &= ~GGTERRAIN_FRACTAL_OUTSIDE_HEIGHTMAP;
}

float GGTerrain_UnitsToMeters( float units )
{
	return units * 0.0254f; // 1 unit = 1 inch
}

float GGTerrain_MetersToUnits( float meters )
{
	return meters * 39.37f; // 1 unit = 1 inch
}

float GGTerrain_OffsetToMeters( float units )
{
	return units * 1270; // 1 unit = 1 inch, multiplied by 50000
}

float GGTerrain_MetersToOffset( float meters )
{
	return meters * 0.0007874f; // 1 unit = 1 inch, divided by 50000
}

// physical texture map
PageEntry pageMemory[ physPagesX * physPagesY ]; // page 0 must never be used, used as invalid page
StackArray<PageEntry*> pagesFree;

struct BoxVertex
{
    float x,y,z;
};

BoxVertex g_VerticesBox[8] = 
{
    { -1.0f, -1.0f, -1.0f },
    { -1.0f, -1.0f,  1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{  1.0f, -1.0f, -1.0f },
    {  1.0f, -1.0f,  1.0f },
	{  1.0f,  1.0f, -1.0f },
	{  1.0f,  1.0f,  1.0f },
};

uint16_t g_IndicesBox[ 24 ] =
{
	0, 4, 2, 2, 4, 6,  // Z- face
	5, 1, 7, 7, 1, 3,  // Z+ face
	1, 0, 3, 3, 0, 2,  // X- face
	4, 5, 6, 6, 5, 7,  // X+ face
	//1, 5, 0, 0, 5, 4,  // Y- face
	//2, 6, 3, 3, 6, 7  // Y+ face
};

GPUBuffer boxVertexBuffer;
GPUBuffer boxIndexBuffer;

// overlay
Shader shaderOverlayVS;
Shader shaderOverlayPS;
PipelineState psoOverlay;

// PBR spheres
struct VertexSphere
{
    float x, y, z;
	float nx, ny, nz;
	float u, v;
};

VertexSphere* g_VerticesSphere;
uint16_t *g_IndicesSphere;
uint32_t g_numIndicesSphere = 0;
uint32_t g_numVerticesSphere = 0;

GPUBuffer sphereVertexBuffer;
GPUBuffer sphereIndexBuffer;
InputLayout sphereInputLayout;
Shader sphereVS;
Shader spherePS;
Shader spherePrepassPS;
PipelineState psoSphere;
PipelineState psoSpherePrepass;

struct VertexQuad
{
    float x,y;
};

VertexQuad g_VerticesQuad[6] = 
{
    { -1.0f,  1.0f, },
    { -1.0f, -1.0f, }, 
    {  1.0f,  1.0f, }, 
    {  1.0f,  1.0f, }, 
    { -1.0f, -1.0f, }, 
    {  1.0f, -1.0f, }  
};

// quad variables
InputLayout quadInputLayout;
Shader shaderQuadVS;
Shader shaderQuadPS;
GPUBuffer quadVertexBuffer;

struct sQuadVSConstantData
{
	float4 reserved;
};

sQuadVSConstantData quadVSConstantData;
GPUBuffer quadVSConstantBuffer;

struct sQuadPSConstantData
{
	float4 reserved;
};

sQuadPSConstantData quadPSConstantData;
GPUBuffer quadPSConstantBuffer;

PipelineState psoQuad;

Sampler samplerPoint;
Sampler samplerPointWrap;
Sampler samplerBilinear;
Sampler samplerBilinearWrap;
Sampler samplerTrilinear;
Sampler samplerTrilinearWrap;
Sampler samplerAnisotropicWrap;

Texture texQuad;

void GGTerrain_CreateEmptyTexture( int width, int height, int mipLevels, int levels, FORMAT format, Texture* tex );

class GGTerrainChunk
{
public:
		
	// variables ordered for better packing

	GGTerrainChunk* pNextChunk = 0; // used for the generator queue
	float* pHeightMap = 0;
	float* pHeightMapNoFlat = 0; // height without flat areas
	uint32_t* pNormalMap = 0; // 0x00ZZYYXX
	GPUBuffer indexBuffer;
	GPUBuffer vertexBuffer;
	TerrainVertex* pVertices = 0; // temporary storage for chunk generation
	uint16_t* pIndices = 0; // temporary storage for threading
	uint32_t numIndices = 0; 
	uint32_t numVertices = 0;
	float offsetX = 0; // chunk world X offset
	float offsetZ = 0; // chunk world Z offset
	AABB aabbBounds;
	float segSize = 2.0f; // in world units
	volatile uint8_t iFlags = 0;
	volatile uint8_t iIsGenerating = 0; // modified by threads so separate from flags
	uint8_t genFlags = 0; // [0]=Xneg stitch, [1]=XPos stitch, [2]=Zneg stitch, [3]=ZPos stitch, [4-7]=lod level

	bool IsGenerating() { return iIsGenerating != 0; }
	void SetGenerating( int generating ) { iIsGenerating = generating; }

	bool IsVisible() { return (iFlags & GGTERRAIN_CHUNK_VISIBLE) != 0; }
	void SetVisible( int visible ) 
	{
		if ( visible ) iFlags |= GGTERRAIN_CHUNK_VISIBLE;
		else iFlags &= ~GGTERRAIN_CHUNK_VISIBLE;
	}

	bool IsCopied() { return (iFlags & GGTERRAIN_CHUNK_COPIED) != 0; }
	void SetCopied( int copied ) 
	{
		if ( copied ) iFlags |= GGTERRAIN_CHUNK_COPIED;
		else iFlags &= ~GGTERRAIN_CHUNK_COPIED;
	}

	bool IsFutureVisible() { return (iFlags & GGTERRAIN_CHUNK_FUTURE_VISIBLE) != 0; }
	void SetFutureVisible( int visible ) 
	{
		if ( visible ) iFlags |= GGTERRAIN_CHUNK_FUTURE_VISIBLE;
		else iFlags &= ~GGTERRAIN_CHUNK_FUTURE_VISIBLE;
	}

	bool ShouldRegenerate() { return (iFlags & GGTERRAIN_CHUNK_REGENERATE) != 0; }
	void SetToRegenerate( int regenerate ) 
	{
		if ( regenerate ) iFlags |= GGTERRAIN_CHUNK_REGENERATE;
		else iFlags &= ~GGTERRAIN_CHUNK_REGENERATE;
	}
		
	void Reset()
	{
		GraphicsDevice* device = wiRenderer::GetDevice();
		
		// clear vertex and index buffers by replacing them with smaller versions
		GPUBufferDesc bd = {};
		bd.ByteWidth = 4;
		bd.BindFlags = BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		device->CreateBuffer( &bd, nullptr, &vertexBuffer );

		bd.ByteWidth = 4;
		bd.BindFlags = BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		device->CreateBuffer( &bd, nullptr, &indexBuffer );
		
		pNextChunk = 0;
		
		offsetX = 0;
		offsetZ = 0;
		segSize = 0.5f;
		iFlags = 0;
		genFlags = 0;
		iIsGenerating = 0;
	}

	static float CalculateHeight( float fX, float fZ, int enableUserHeightmap=1, float *origHeight=0 )
	{
		float fX2 = fX / 50000;
		float fZ2 = fZ / 50000;
		fX2 -= ggterrain_local_params.offset_x;
		fZ2 -= ggterrain_local_params.offset_z;

		float frequency = ggterrain_local_params.fractal_initial_freq;
		float amplitude = 1;
		float noiseValue = 0;
		float total = 0;

		/*
		if ( ggterrain_local_params.iHeightMapEditMode == 1 )
		{
			float fX3 = fX / ggterrain_local_render_params2.editable_size;
			fX3 = fX3 * 0.5f + 0.5f;
			fX3 *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
			float fZ3 = fZ / ggterrain_local_render_params2.editable_size;
			fZ3 = fZ3 * 0.5f + 0.5f;
			fZ3 *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE;

			if ( enableUserHeightmap && pHeightMapEdit && fX3 >= 0 && fZ3 >= 0 && fX3 < GGTERRAIN_HEIGHTMAP_EDIT_SIZE && fZ3 < GGTERRAIN_HEIGHTMAP_EDIT_SIZE )
			{
				int iX0 = (int) fX3;
				int iZ0 = (int) fZ3;

				if ( iX0 < 0 ) iX0 = 0;
				if ( iZ0 < 0 ) iZ0 = 0;
				if ( iX0 > GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 ) iX0 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1;
				if ( iZ0 > GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 ) iZ0 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1;

				int iX1 = iX0 + 1;
				int iZ1 = iZ0 + 1;

				if ( iX1 > GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 ) iX1 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1;
				if ( iZ1 > GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 ) iZ1 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1;

				float lerpX = fX3 - iX0;
				float lerpZ = fZ3 - iZ0;

				if ( lerpX < 0 ) lerpX = 0;
				if ( lerpX > 1 ) lerpX = 1;
				if ( lerpZ < 0 ) lerpZ = 0;
				if ( lerpZ > 1 ) lerpZ = 1;
		
				int index00 = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - iZ0) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + iX0;
				int index01 = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - iZ0) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + iX1;
				int index10 = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - iZ1) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + iX0;
				int index11 = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - iZ1) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + iX1;

				float fHeights[ 4 ];
				fHeights[0] = pHeightMapEdit[ index00 ];
				fHeights[1] = pHeightMapEdit[ index01 ];
				fHeights[2] = pHeightMapEdit[ index10 ];
				fHeights[3] = pHeightMapEdit[ index11 ];

				fHeights[0] = fHeights[0] + (fHeights[1] - fHeights[0]) * lerpX;
				fHeights[1] = fHeights[2] + (fHeights[3] - fHeights[2]) * lerpX;

				fHeights[0] = fHeights[0] + (fHeights[1] - fHeights[0]) * lerpZ;
						
				frequency = ggterrain_local_params.fractal_initial_freq * ggterrain_local_params.heightmap_scale * 106.7f;
				//amplitude = ggterrain_local_params.fractal_initial_amplitude * 0.05f;
				noiseValue = fHeights[0];
				total = 1;
			}
		}
		*/
		
		for ( size_t i = 0; i < ggterrain_local_params.fractal_levels; i++ ) 
		{
			float offset = noise.noise( frequency * fX2, frequency * fZ2 );

			if ( ggterrain_local_params.iHeightMapEditMode == 0 )
			{			
				switch( i )
				{
					case 0:
					{
						if ( ggterrain_local_params.fractal_flags & GGTERRAIN_FRACTAL_VALLEYS0 ) offset = ( abs(offset) * 1.4f ) - 0.4f;
						else if ( ggterrain_local_params.fractal_flags & GGTERRAIN_FRACTAL_RIDGES0 ) offset = 1 - (abs(offset) * 1.4f);
					} break;

					case 1:
					{
						if ( ggterrain_local_params.fractal_flags & GGTERRAIN_FRACTAL_VALLEYS1 ) offset = ( abs(offset) * 1.4f ) - 0.4f;
						else if ( ggterrain_local_params.fractal_flags & GGTERRAIN_FRACTAL_RIDGES1 ) offset = 1 - (abs(offset) * 1.4f);
					} break;

					case 2:
					{
						if ( ggterrain_local_params.fractal_flags & GGTERRAIN_FRACTAL_VALLEYS2 ) offset = ( abs(offset) * 1.4f ) - 0.4f;
						else if ( ggterrain_local_params.fractal_flags & GGTERRAIN_FRACTAL_RIDGES2 ) offset = 1 - (abs(offset) * 1.4f);
					} break;

					case 3:
					{
						if ( ggterrain_local_params.fractal_flags & GGTERRAIN_FRACTAL_VALLEYS3 ) offset = ( abs(offset) * 1.4f ) - 0.4f;
						else if ( ggterrain_local_params.fractal_flags & GGTERRAIN_FRACTAL_RIDGES3 ) offset = 1 - (abs(offset) * 1.4f);
					} break;
				}
							
				offset *= amplitude;
				if ( i > 0 ) 
				{
					float base = noiseValue;
					if ( base < 0.01f ) base = 0.01f;
					offset *= pow( base, ggterrain_local_params.noise_fallof_power );
				}
			}
			else
			{
				offset *= amplitude;
			}

			noiseValue += offset;
			total += amplitude;

			frequency *= ggterrain_local_params.fractal_freq_increase;
			amplitude *= ggterrain_local_params.fractal_freq_weight;
		}
		noiseValue /= total;
		noiseValue *= ggterrain_local_params.fractal_initial_amplitude;
		bool noiseNeg = noiseValue < 0;
		noiseValue = pow( abs(noiseValue), ggterrain_local_params.noise_power );
		if ( noiseNeg ) noiseValue = -noiseValue;
		
		float height = noiseValue;

		if ( origHeight ) *origHeight = height;
			
		if ( ggterrain_local_params.iHeightMapEditMode == 0 )
		{
			float fX3 = fX / ggterrain_local_render_params2.editable_size;
			fX3 = fX3 * 0.5f + 0.5f;
			fX3 *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
			float fZ3 = fZ / ggterrain_local_render_params2.editable_size;
			fZ3 = fZ3 * 0.5f + 0.5f;
			fZ3 *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE;

			if ( enableUserHeightmap && fX3 >= 0 && fZ3 >= 0 && fX3 < GGTERRAIN_HEIGHTMAP_EDIT_SIZE && fZ3 < GGTERRAIN_HEIGHTMAP_EDIT_SIZE )
			{
				int iX0 = (int) fX3;
				int iZ0 = (int) fZ3;

				if ( iX0 < 0 ) iX0 = 0;
				if ( iZ0 < 0 ) iZ0 = 0;
				if ( iX0 > GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 ) iX0 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1;
				if ( iZ0 > GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 ) iZ0 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1;

				int iX1 = iX0 + 1;
				int iZ1 = iZ0 + 1;

				if ( iX1 > GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 ) iX1 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1;
				if ( iZ1 > GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 ) iZ1 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1;

				float lerpX = fX3 - iX0;
				float lerpZ = fZ3 - iZ0;

				if ( lerpX < 0 ) lerpX = 0;
				if ( lerpX > 1 ) lerpX = 1;
				if ( lerpZ < 0 ) lerpZ = 0;
				if ( lerpZ > 1 ) lerpZ = 1;
		
				int index00 = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - iZ0) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + iX0;
				int index01 = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - iZ0) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + iX1;
				int index10 = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - iZ1) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + iX0;
				int index11 = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - iZ1) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + iX1;

				if ( pHeightMapEdit )
				{
					float fHeights[ 4 ];
					fHeights[0] = pHeightMapEdit[ index00 ];
					fHeights[1] = pHeightMapEdit[ index01 ];
					fHeights[2] = pHeightMapEdit[ index10 ];
					fHeights[3] = pHeightMapEdit[ index11 ];

					uint8_t heightTypes[ 4 ];
					heightTypes[0] = pHeightMapEditType[ index00 ];
					heightTypes[1] = pHeightMapEditType[ index01 ];
					heightTypes[2] = pHeightMapEditType[ index10 ];
					heightTypes[3] = pHeightMapEditType[ index11 ];

					for( int i = 0; i < 4; i++ )
					{
						switch( heightTypes[i] )
						{
							case 0: fHeights[i] += height; break; // add
							case 1: break; // replace
							case 2: fHeights[i] = height; break; // do nothing
							case 3: fHeights[i] = height; break; // do nothing
						}
					}

					fHeights[0] = fHeights[0] + (fHeights[1] - fHeights[0]) * lerpX;
					fHeights[1] = fHeights[2] + (fHeights[3] - fHeights[2]) * lerpX;

					fHeights[0] = fHeights[0] + (fHeights[1] - fHeights[0]) * lerpZ;
						
					height = fHeights[0];
				}

				if ( origHeight ) *origHeight = height;

				if ( pHeightMapFlatAreas )
				{
					int indices[ 4 ] = { index00, index01, index10, index11 };
					float fHeights[ 4 ];
					for( uint32_t i = 0; i < 4; i++ )
					{
						uint32_t id = pHeightMapFlatAreas[ indices[i] ];
						if ( id == 0 || id >= ggterrain_flat_areas_array_size ) 
						{
							fHeights[ i ] = height;
						}
						else
						{
							//uint8_t weight = pHeightMapFlatAreaWeight[ indices[i] ]; // not used?!?
							float flatHeight = ggterrain_flat_areas[ id ].y;
							flatHeight -= ggterrain_local_params.offset_y;

							// some levels have "ggterrain_local_params.minHeight" as zero! Clearly the following will fail if so (some terrain gen levels)
							if (ggterrain_local_params.minHeight != 0.0f)
							{
								if ( flatHeight < 0 ) 
									flatHeight /= ggterrain_local_params.minHeight;
								else
									flatHeight /= ggterrain_local_params.height;
							}

							fHeights[ i ] = flatHeight;
						}
					}

					fHeights[0] = fHeights[0] + (fHeights[1] - fHeights[0]) * lerpX;
					fHeights[1] = fHeights[2] + (fHeights[3] - fHeights[2]) * lerpX;

					fHeights[0] = fHeights[0] + (fHeights[1] - fHeights[0]) * lerpZ;
						
					height = fHeights[0];
				}
			}
		}

		if ( origHeight ) 
		{
			if ( *origHeight < 0 ) *origHeight *= ggterrain_local_params.minHeight;
			else *origHeight *= ggterrain_local_params.height;
		
			*origHeight += ggterrain_local_params.offset_y;
		}

		if ( height < 0 ) height *= ggterrain_local_params.minHeight;
		else height *= ggterrain_local_params.height;
		
		return height + ggterrain_local_params.offset_y;
	}
	
	static float CalculateHeightWithHeightmap( float fX, float fZ, int enableUserHeightmap=1, float* origHeight=0 )
	{
		uint16_t* pMainHeightmap = ggterrain_local_params.pHeightmapMain;
		if ( !pMainHeightmap ) return CalculateHeight( fX, fZ, enableUserHeightmap, origHeight );

		int map_width = ggterrain_local_params.heightmap_width;
		int map_height = ggterrain_local_params.heightmap_height;
		float fX2 = GGTerrain_UnitsToMeters( fX ) * ggterrain_local_params.heightmap_scale + map_width/2.0f;
		float fZ2 = GGTerrain_UnitsToMeters( fZ ) * ggterrain_local_params.heightmap_scale + map_height/2.0f;
		
		int iX0 = (int) fX2;
		int iZ0 = (int) fZ2;

		if ( iX0 < 0 ) iX0 = 0;
		if ( iZ0 < 0 ) iZ0 = 0;
		if ( iX0 > map_width-1 ) iX0 = map_width-1;
		if ( iZ0 > map_height-1 ) iZ0 = map_height-1;

		int iX1 = iX0 + 1;
		int iZ1 = iZ0 + 1;

		if ( iX1 > map_width-1 ) iX1 = map_width-1;
		if ( iZ1 > map_height-1 ) iZ1 = map_height-1;

		float lerpX = fX2 - iX0;
		float lerpZ = fZ2 - iZ0;

		if ( lerpX < 0 ) lerpX = 0;
		if ( lerpX > 1 ) lerpX = 1;
		if ( lerpZ < 0 ) lerpZ = 0;
		if ( lerpZ > 1 ) lerpZ = 1;
		
		int index00 = (map_height-1 - iZ0) * map_width + iX0;
		int index01 = (map_height-1 - iZ0) * map_width + iX1;
		int index10 = (map_height-1 - iZ1) * map_width + iX0;
		int index11 = (map_height-1 - iZ1) * map_width + iX1;

		float height00 = pMainHeightmap[ index00 ] / 65535.0f;
		float height01 = pMainHeightmap[ index01 ] / 65535.0f;
		float height10 = pMainHeightmap[ index10 ] / 65535.0f;
		float height11 = pMainHeightmap[ index11 ] / 65535.0f;

		float height0 = height00 + (height01 - height00) * lerpX;
		float height1 = height10 + (height11 - height10) * lerpX;

		float height = height0 + (height1 - height0) * lerpZ;


		float fX3 = fX / 50000;
		float fZ3 = fZ / 50000;
		fX3 -= ggterrain_local_params.offset_x;
		fZ3 -= ggterrain_local_params.offset_z;
		
		float frequency = ggterrain_local_params.fractal_initial_freq * ggterrain_local_params.heightmap_scale * 1067.0f;
		float amplitude = ggterrain_local_params.heightmap_roughness * 0.0005f;
		float noiseValue = height;
		float total = 1;

		for ( size_t i = 0; i < ggterrain_local_params.fractal_levels; i++ ) 
		{
			float offset = noise.noise( frequency * fX3, frequency * fZ3 );
			offset *= amplitude;
			if ( i > 0 ) 
			{
				float base = noiseValue;
				if ( base < 0.01f ) base = 0.01f;
				offset *= pow( base, ggterrain_local_params.noise_fallof_power );
			}
			noiseValue += offset;
			total += amplitude;

			frequency *= ggterrain_local_params.fractal_freq_increase;
			amplitude *= ggterrain_local_params.fractal_freq_weight;
		}

		noiseValue /= total;
		bool noiseNeg = noiseValue < 0;
		noiseValue = pow( abs(noiseValue), ggterrain_local_params.noise_power );
		if ( noiseNeg ) noiseValue = -noiseValue;
		
		height = noiseValue;

		if ( origHeight ) *origHeight = height;
			
		if ( ggterrain_local_params.iHeightMapEditMode == 0 )
		{
			float fX3 = fX / ggterrain_local_render_params2.editable_size;
			fX3 = fX3 * 0.5f + 0.5f;
			fX3 *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
			float fZ3 = fZ / ggterrain_local_render_params2.editable_size;
			fZ3 = fZ3 * 0.5f + 0.5f;
			fZ3 *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE;

			if ( enableUserHeightmap && fX3 >= 0 && fZ3 >= 0 && fX3 < GGTERRAIN_HEIGHTMAP_EDIT_SIZE && fZ3 < GGTERRAIN_HEIGHTMAP_EDIT_SIZE )
			{
				int iX0 = (int) fX3;
				int iZ0 = (int) fZ3;

				if ( iX0 < 0 ) iX0 = 0;
				if ( iZ0 < 0 ) iZ0 = 0;
				if ( iX0 > GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 ) iX0 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1;
				if ( iZ0 > GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 ) iZ0 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1;

				int iX1 = iX0 + 1;
				int iZ1 = iZ0 + 1;

				if ( iX1 > GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 ) iX1 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1;
				if ( iZ1 > GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 ) iZ1 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1;

				float lerpX = fX3 - iX0;
				float lerpZ = fZ3 - iZ0;

				if ( lerpX < 0 ) lerpX = 0;
				if ( lerpX > 1 ) lerpX = 1;
				if ( lerpZ < 0 ) lerpZ = 0;
				if ( lerpZ > 1 ) lerpZ = 1;
		
				int index00 = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - iZ0) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + iX0;
				int index01 = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - iZ0) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + iX1;
				int index10 = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - iZ1) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + iX0;
				int index11 = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - iZ1) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + iX1;

				if ( pHeightMapEdit )
				{
					float fHeights[ 4 ];
					fHeights[0] = pHeightMapEdit[ index00 ];
					fHeights[1] = pHeightMapEdit[ index01 ];
					fHeights[2] = pHeightMapEdit[ index10 ];
					fHeights[3] = pHeightMapEdit[ index11 ];

					uint8_t heightTypes[ 4 ];
					heightTypes[0] = pHeightMapEditType[ index00 ];
					heightTypes[1] = pHeightMapEditType[ index01 ];
					heightTypes[2] = pHeightMapEditType[ index10 ];
					heightTypes[3] = pHeightMapEditType[ index11 ];

					for( int i = 0; i < 4; i++ )
					{
						switch( heightTypes[i] )
						{
							case 0: fHeights[i] += height; break; // add
							case 1: break; // replace
							case 2: fHeights[i] = height; break; // do nothing
							case 3: fHeights[i] = height; break; // do nothing
						}
					}

					fHeights[0] = fHeights[0] + (fHeights[1] - fHeights[0]) * lerpX;
					fHeights[1] = fHeights[2] + (fHeights[3] - fHeights[2]) * lerpX;

					fHeights[0] = fHeights[0] + (fHeights[1] - fHeights[0]) * lerpZ;
						
					height = fHeights[0];
				}

				if ( origHeight ) *origHeight = height;

				if ( pHeightMapFlatAreas )
				{
					int indices[ 4 ] = { index00, index01, index10, index11 };
					float fHeights[ 4 ];
					for( uint32_t i = 0; i < 4; i++ )
					{
						uint32_t id = pHeightMapFlatAreas[ indices[i] ];
						if ( id == 0 || id >= ggterrain_flat_areas_array_size ) 
						{
							fHeights[ i ] = height;
						}
						else
						{
							float flatHeight = ggterrain_flat_areas[ id ].y;
							flatHeight -= ggterrain_local_params.offset_y;
							if ( flatHeight < 0 ) flatHeight /= ggterrain_local_params.minHeight;
							else flatHeight /= ggterrain_local_params.height;

							fHeights[ i ] = flatHeight;
						}
					}

					fHeights[0] = fHeights[0] + (fHeights[1] - fHeights[0]) * lerpX;
					fHeights[1] = fHeights[2] + (fHeights[3] - fHeights[2]) * lerpX;

					fHeights[0] = fHeights[0] + (fHeights[1] - fHeights[0]) * lerpZ;
						
					height = fHeights[0];
				}
			}
		}

		if ( origHeight ) 
		{
			if ( *origHeight < 0 ) *origHeight *= ggterrain_local_params.minHeight;
			else *origHeight *= ggterrain_local_params.height;
		
			*origHeight += ggterrain_local_params.offset_y;
		}

		if ( height < 0 ) height *= ggterrain_local_params.minHeight;
		else height *= ggterrain_local_params.height;

		height += ggterrain_local_params.offset_y;

		if ( fX2 >= 0 && fZ2 >= 0 && fX2 <= map_width-1 && fZ2 <= map_height-1 ) 
		{
			return height;
		}
		else
		{
			float outsideHeight = ggterrain_local_params.height_outside_heightmap;

			if ( (ggterrain_local_params.fractal_flags & GGTERRAIN_FRACTAL_OUTSIDE_HEIGHTMAP) != 0 )
			{
				outsideHeight = CalculateHeight( fX, fZ, enableUserHeightmap );
			}

			float diffX = 0;
			if ( fX2 < 0 ) diffX = fX2;
			if ( fX2 > map_width-1 ) diffX = fX2 - (map_width-1);
			float diffZ = 0;
			if ( fZ2 < 0 ) diffZ = fZ2;
			if ( fZ2 > map_height-1 ) diffZ = fZ2 - (map_height-1);
			float dist = diffX*diffX + diffZ*diffZ;
			dist = sqrt( dist );
			dist /= ggterrain_local_params.fade_outside_heightmap;
			if ( dist > 1 ) dist = 1;

			return (outsideHeight-height)*dist + height;
		}
	}
	
	int GetHeight( float x, float z, float* height, int includeFlatAreas=1 )
	{
		if ( !height ) return 0;

		float hX = (x - offsetX) / segSize; //PE:!!!! GOT Exception thrown: read access violation. **this** was 0xFFFFFFFFFFFFFF4F.
		float hZ = (z - offsetZ) / segSize;
		uint32_t numSegments = ggterrain_local_params.segments_per_chunk;

		// height map is numSegments+1 in size so numSegments is a valid value
		//if ( hX < 0 || hZ < 0 || hX > numSegments || hZ > numSegments ) return 0; 
		if ( hX < 0 ) hX = 0;
		if ( hZ < 0 ) hZ = 0; 
		if ( hX > (float) numSegments ) hX = (float) numSegments;
		if ( hZ > (float) numSegments ) hZ = (float) numSegments;

		uint32_t iX0 = (uint32_t) hX;
		uint32_t iX1 = iX0 + 1;
		uint32_t iZ0 = (uint32_t) hZ;
		uint32_t iZ1 = iZ0 + 1;

		if ( iX1 > numSegments ) iX1 = numSegments;
		if ( iZ1 > numSegments ) iZ1 = numSegments;

		float lerpX = hX - iX0;
		float lerpZ = hZ - iZ0;
		
		uint32_t index00 = (numSegments - iZ0) * (numSegments+1) + iX0;
		uint32_t index01 = (numSegments - iZ0) * (numSegments+1) + iX1;
		uint32_t index10 = (numSegments - iZ1) * (numSegments+1) + iX0;
		uint32_t index11 = (numSegments - iZ1) * (numSegments+1) + iX1;

		float height00, height01, height10, height11;
		if ( includeFlatAreas )
		{
			height00 = pHeightMap[ index00 ];
			height01 = pHeightMap[ index01 ];
			height10 = pHeightMap[ index10 ];
			height11 = pHeightMap[ index11 ];
		}
		else
		{
			height00 = pHeightMapNoFlat[ index00 ];
			height01 = pHeightMapNoFlat[ index01 ];
			height10 = pHeightMapNoFlat[ index10 ];
			height11 = pHeightMapNoFlat[ index11 ];
		}

		float height0 = height00 + (height01 - height00) * lerpX;
		float height1 = height10 + (height11 - height10) * lerpX;

		*height = height0 + (height1 - height0) * lerpZ;

		return 1;
	}

	int GetNormal( float x, float z, float* normalX, float* normalY, float* normalZ )
	{
		float hX = (x - offsetX) / segSize;
		float hZ = (z - offsetZ) / segSize;
		uint32_t numSegments = ggterrain_local_params.segments_per_chunk;

		// normal map is numSegments+1 in size so numSegments is a valid value
		//if ( hX < 0 || hZ < 0 || hX > numSegments || hZ > numSegments ) return 0; 
		if ( hX < 0 ) hX = 0;
		if ( hZ < 0 ) hZ = 0; 
		if ( hX > (float) numSegments ) hX = (float) numSegments;
		if ( hZ > (float) numSegments ) hZ = (float) numSegments;

		uint32_t iX0 = (uint32_t) hX;
		uint32_t iX1 = iX0 + 1;
		uint32_t iZ0 = (uint32_t) hZ;
		uint32_t iZ1 = iZ0 + 1;

		if ( iX1 > numSegments ) iX1 = numSegments;
		if ( iZ1 > numSegments ) iZ1 = numSegments;

		float lerpX = hX - iX0;
		float lerpZ = hZ - iZ0;
		
		uint32_t index00 = (numSegments - iZ0) * (numSegments+1) + iX0;
		uint32_t index01 = (numSegments - iZ0) * (numSegments+1) + iX1;
		uint32_t index10 = (numSegments - iZ1) * (numSegments+1) + iX0;
		uint32_t index11 = (numSegments - iZ1) * (numSegments+1) + iX1;

		uint32_t normal00 = pNormalMap[ index00 ];
		uint32_t normal01 = pNormalMap[ index01 ];
		uint32_t normal10 = pNormalMap[ index10 ];
		uint32_t normal11 = pNormalMap[ index11 ];

		float3 fNormal00, fNormal01, fNormal10, fNormal11;

		fNormal00.x = (normal00 & 0xFF) / 127.5f - 1.0f;
		fNormal00.y = ((normal00 >> 8) & 0xFF) / 127.5f - 1.0f;
		fNormal00.z = ((normal00 >> 16) & 0xFF) / 127.5f - 1.0f;

		fNormal01.x = (normal01 & 0xFF) / 127.5f - 1.0f;
		fNormal01.y = ((normal01 >> 8) & 0xFF) / 127.5f - 1.0f;
		fNormal01.z = ((normal01 >> 16) & 0xFF) / 127.5f - 1.0f;

		fNormal10.x = (normal10 & 0xFF) / 127.5f - 1.0f;
		fNormal10.y = ((normal10 >> 8) & 0xFF) / 127.5f - 1.0f;
		fNormal10.z = ((normal10 >> 16) & 0xFF) / 127.5f - 1.0f;

		fNormal11.x = (normal11 & 0xFF) / 127.5f - 1.0f;
		fNormal11.y = ((normal11 >> 8) & 0xFF) / 127.5f - 1.0f;
		fNormal11.z = ((normal11 >> 16) & 0xFF) / 127.5f - 1.0f;

		float3 fNormal0, fNormal1;

		fNormal0.x = fNormal00.x + (fNormal01.x - fNormal00.x) * lerpX;
		fNormal0.y = fNormal00.y + (fNormal01.y - fNormal00.y) * lerpX;
		fNormal0.z = fNormal00.z + (fNormal01.z - fNormal00.z) * lerpX;

		fNormal1.x = fNormal10.x + (fNormal11.x - fNormal10.x) * lerpX;
		fNormal1.y = fNormal10.y + (fNormal11.y - fNormal10.y) * lerpX;
		fNormal1.z = fNormal10.z + (fNormal11.z - fNormal10.z) * lerpX;

		if ( normalX ) *normalX = fNormal0.x + (fNormal1.x - fNormal0.x) * lerpZ;
		if ( normalY ) *normalY = fNormal0.y + (fNormal1.y - fNormal0.y) * lerpZ;
		if ( normalZ ) *normalZ = fNormal0.z + (fNormal1.z - fNormal0.z) * lerpZ;

		return 1;
	}

	float GetRayExitDist( RAY ray, int* outSide )
	{   
		float minx = offsetX;
		float minz = offsetZ;
		
		float maxx = offsetX + (segSize * ggterrain_local_params.segments_per_chunk);
		float maxz = offsetZ + (segSize * ggterrain_local_params.segments_per_chunk);
		
		float tminx, tmaxx, tminz, tmaxz;
		int side = 0;

		if ( ray.direction.x == 0 ) 
		{
			tminx = (minx - ray.origin.x) * 1e20f;
			tmaxx = (maxx - ray.origin.x) * 1e20f;
		}
		else if ( ray.direction.x > 0 ) 
		{
			tminx = (minx - ray.origin.x) * ray.direction_inverse.x;
			tmaxx = (maxx - ray.origin.x) * ray.direction_inverse.x;
			side = 1;
		}
		else 
		{
			tmaxx = (minx - ray.origin.x) * ray.direction_inverse.x;
			tminx = (maxx - ray.origin.x) * ray.direction_inverse.x;
			side = 2;
		}

		if ( ray.direction.z == 0 ) 
		{
			tminz = (minz - ray.origin.z) * 1e20f;
			tmaxz = (maxz - ray.origin.z) * 1e20f;
		}
		else if ( ray.direction.z > 0 ) 
		{
			tminz = (minz - ray.origin.z) * ray.direction_inverse.z;
			tmaxz = (maxz - ray.origin.z) * ray.direction_inverse.z;
			if ( tmaxz < tmaxx ) side = 3;
		}
		else 
		{
			tmaxz = (minz - ray.origin.z) * ray.direction_inverse.z;
			tminz = (maxz - ray.origin.z) * ray.direction_inverse.z;
			if ( tmaxz < tmaxx ) side = 4;
		}
    
		if ( tmaxz < tmaxx ) tmaxx = tmaxz;
		if ( outSide ) *outSide = side;
		return tmaxx;
	}

	int CheckRayHeight( RAY ray, float* entryDist )
	{   
		if ( ray.direction.y == 0 ) 
		{
			if ( ray.origin.y > aabbBounds._max.y || ray.origin.y < aabbBounds._min.y ) return 0;

			*entryDist = 0;
		}
		else if ( ray.direction.y >= 0 ) 
		{
			if ( ray.origin.y > aabbBounds._max.y ) return 0;
			
			*entryDist = (aabbBounds._min.y - ray.origin.y) * ray.direction_inverse.y;
		} 
		else 
		{
			if ( ray.origin.y < aabbBounds._min.y ) return 0;

			*entryDist = (aabbBounds._max.y - ray.origin.y) * ray.direction_inverse.y;
		}
        
		return 1;
	}

	int RayCast( RAY ray, float startDist, float endDist, float* pOutX, float* pOutY, float* pOutZ, float* pNormX, float* pNormY, float* pNormZ, int includeFlatAreas=1 )
	{
		float largestSide = abs( ray.direction.x );
		if ( abs(ray.direction.z) > largestSide ) largestSide = abs( ray.direction.z );
		if ( largestSide <= 0.001f ) largestSide = 0.001f;

		float stepSize = segSize / largestSide;
		float prevDist = startDist;
		float currDist = prevDist + stepSize;

		float prevSampleX = ray.origin.x + ray.direction.x * prevDist;
		float prevSampleY = ray.origin.y + ray.direction.y * prevDist;
		float prevSampleZ = ray.origin.z + ray.direction.z * prevDist;

		float prevHeight;
		GetHeight( prevSampleX, prevSampleZ, &prevHeight, includeFlatAreas );
		prevHeight -= prevSampleY;
		
		while( 1 )
		{
			float sampleX = ray.origin.x + ray.direction.x * currDist;
			float sampleY = ray.origin.y + ray.direction.y * currDist;
			float sampleZ = ray.origin.z + ray.direction.z * currDist;

			float height;
			GetHeight( sampleX, sampleZ, &height, includeFlatAreas );
			height -= sampleY;

			if ( prevHeight <= 0 && height >= 0 )
			{
				// got a hit, find exactly where
				float limit = 0.2f; //segSize / 512.0f;
				if (currDist - prevDist > limit)
				{
					float midDist, midX, midY, midZ;
					float prevMid = 1e20f;
					while (currDist - prevDist > limit)
					{
						midDist = (currDist + prevDist) / 2.0f;
						midX = ray.origin.x + ray.direction.x * midDist;
						midY = ray.origin.y + ray.direction.y * midDist;
						midZ = ray.origin.z + ray.direction.z * midDist;
						if (midDist == prevMid) break;
						prevMid = midDist;

						float midHeight;
						GetHeight(midX, midZ, &midHeight, includeFlatAreas);
						midHeight -= midY;

						if (midHeight < 0) prevDist = midDist;
						else if (midHeight > 0) currDist = midDist;
						else break;
					}

					// finalise intersection details
					if (pOutX) *pOutX = midX;
					if (pOutY) *pOutY = midY;
					if (pOutZ) *pOutZ = midZ;
					GetNormal(midX, midZ, pNormX, pNormY, pNormZ);
					return 1;
				}
				else
				{
					// LB: had to add this as 'while (currDist - prevDist > limit)' could be false, 
					// causing midXYZ to be uninitialised, so gracefully fail if so. Possible crash fix.
					return 0;
				}
			}

			if ( currDist == endDist ) return 0;
			if ( sampleY > aabbBounds._max.y && ray.direction.y >= 0 ) return 0;
			if ( sampleY < aabbBounds._min.y && ray.direction.y <= 0 ) return 0;

			prevSampleX = sampleX;
			prevSampleY = sampleY;
			prevSampleZ = sampleZ;
			prevDist = currDist;
			currDist += stepSize;
			if ( currDist > endDist ) currDist = endDist;
		}

		return 0;
	}

	void PhysicsProcessVertices( void* callback, float worldToPhysScale, float minY, float maxY, int startX, int startZ, int endX, int endZ )
	{
		/*
		btVector3 vertices[3];
			
		getVertex(x, j, vertices[0]);
		getVertex(x, j + 1, vertices[1]);
		getVertex(x + 1, j, vertices[2]);

		// Skip triangle processing if the triangle is out-of-AABB.
		Range upRange = minmaxRange(vertices[0][1], vertices[1][1], vertices[2][1]);

		if (upRange.overlaps(aabbUpRange)) callback->processTriangle(vertices, 2 * x, j);

		// equivalent to: getVertex(x + 1, j, vertices[0]);
		vertices[0] = vertices[2];

		getVertex(x + 1, j + 1, vertices[2]);
		upRange.min = btMin(upRange.min, vertices[2][1]);
		upRange.max = btMax(upRange.max, vertices[2][1]);

		if (upRange.overlaps(aabbUpRange)) callback->processTriangle(vertices, 2 * x + 1, j);
		*/

		if ( minY >= aabbBounds._max.y || maxY < aabbBounds._min.y ) return;

		float fMinPhysY = minY * worldToPhysScale;
		float fMaxPhysY = maxY * worldToPhysScale;

		btTriangleCallback* physCallback = (btTriangleCallback*) callback;
		int numSegments = (int) ggterrain_local_params.segments_per_chunk;
		int numVerts = numSegments + 1;

		if ( startX < 0 ) startX = 0;
		if ( startZ < 0 ) startZ = 0;
		if ( endX >= numSegments ) endX = numSegments - 1;
		if ( endZ >= numSegments ) endZ = numSegments - 1;

		btVector3 vertices[ 3 ];

		float physSegSize = segSize * worldToPhysScale;

		for( int z = startZ; z <= endZ; z++ )
		{
			float fZ = (z * segSize) + offsetZ;
			fZ *= worldToPhysScale;
			for( int x = startX; x <= endX; x++ )
			{
				float fX = (x * segSize) + offsetX;
				fX *= worldToPhysScale;

				int indexMap = (numSegments - z) * numVerts + x;
				
				vertices[ 0 ].setValue( fX, pHeightMap[ indexMap ] * worldToPhysScale, fZ );
				vertices[ 1 ].setValue( fX, pHeightMap[ indexMap - numVerts ] * worldToPhysScale, fZ + physSegSize );
				vertices[ 2 ].setValue( fX + physSegSize, pHeightMap[ indexMap + 1 ] * worldToPhysScale, fZ );

				if ( (vertices[0][1] < fMaxPhysY || vertices[1][1] < fMaxPhysY || vertices[2][1] < fMaxPhysY) 
				  && (vertices[0][1] > fMinPhysY || vertices[1][1] > fMinPhysY || vertices[2][1] > fMinPhysY) )
				{
					physCallback->processTriangle( vertices, 2 * x, z );
				}

				vertices[ 0 ] = vertices[ 2 ];

				vertices[ 2 ].setValue( fX + physSegSize, pHeightMap[ indexMap - numVerts + 1 ] * worldToPhysScale, fZ + physSegSize );

				if ( (vertices[0][1] < fMaxPhysY || vertices[1][1] < fMaxPhysY || vertices[2][1] < fMaxPhysY) 
				  && (vertices[0][1] > fMinPhysY || vertices[1][1] > fMinPhysY || vertices[2][1] > fMinPhysY) )
				{
					physCallback->processTriangle( vertices, 2 * x + 1, z );
				}
			}
		}
	}

	void GetTriangleList( UnorderedArray<KMaths::Vector3>* vertices, float minX, float minZ, float maxX, float maxZ )
	{
		if ( aabbBounds._min.x > maxX || aabbBounds._min.z > maxZ ) return;
		if ( aabbBounds._max.x < minX || aabbBounds._max.z < minZ ) return;

		int numSegments = (int) ggterrain_local_params.segments_per_chunk;
		int numVerts = numSegments + 1;

		float fBeginX = minX - offsetX;
		float fBeginZ = minZ - offsetZ;
		float fEndX = maxX - offsetX;
		float fEndZ = maxZ - offsetZ;

		float chunkSize = segSize * numSegments;

		// check bounds in float domain in case values exceed integer range
		if ( fBeginX < 0 ) fBeginX = 0;
		if ( fBeginZ < 0 ) fBeginZ = 0;
		if ( fEndX > chunkSize ) fEndX = chunkSize;
		if ( fEndZ > chunkSize ) fEndZ = chunkSize;

		int iBeginX = (int) (fBeginX / segSize);
		int iBeginZ = (int) (fBeginZ / segSize);
		int iEndX = (int) (fEndX / segSize);
		int iEndZ = (int) (fEndZ / segSize);

		if ( iEndX >= numSegments ) iEndX = numSegments - 1;
		if ( iEndZ >= numSegments ) iEndZ = numSegments - 1;

		for( int z = iBeginZ; z <= iEndZ; z++ )
		{
			float fZ = (z * segSize) + offsetZ;
			for( int x = iBeginX; x <= iEndX; x++ )
			{
				float fX = (x * segSize) + offsetX;
				int indexMap = (numSegments - z) * numVerts + x;

				// triangle 1
				vertices->AddItem( KMaths::Vector3(fX, pHeightMap[ indexMap ], fZ) );
				vertices->AddItem( KMaths::Vector3(fX, pHeightMap[ indexMap - numVerts ], fZ + segSize) );
				vertices->AddItem( KMaths::Vector3(fX + segSize, pHeightMap[ indexMap + 1 ], fZ) );
								  
				 // triangle 2	  
				vertices->AddItem( KMaths::Vector3(fX + segSize, pHeightMap[ indexMap + 1 ], fZ) );
				vertices->AddItem( KMaths::Vector3(fX, pHeightMap[ indexMap - numVerts ], fZ + segSize) );
				vertices->AddItem( KMaths::Vector3(fX + segSize, pHeightMap[ indexMap - numVerts + 1 ], fZ + segSize) );
			}
		}
	}

	// check the two triangles in this segment against the ray
	int PhysicsRayCastSegment( void* callback, float worldToPhysScale, int x0, int z0, float startHeight, float endHeight, float fEdgeThreshold)
	{
		btTriangleRaycastCallback* rayCastCallback = (btTriangleRaycastCallback*) callback;

		//assert( x0 >= 0 );
		//assert( z0 >= 0 );
		if ( x0 < 0 || z0 < 0 ) return 0;

		int numSegments = (int) ggterrain_local_params.segments_per_chunk;
		int numVerts = numSegments + 1;
		float physSegmentSize = segSize * worldToPhysScale;

		if ( x0 >= numSegments || z0 >= numSegments ) return 0;

		int index00 = (numSegments - z0) * numVerts + x0;
		int index01 = index00 + 1;
		int index10 = index00 - numVerts;
		int index11 = index10 + 1;

		float height00 = pHeightMap[ index00 ];
		float height01 = pHeightMap[ index01 ];
		float height10 = pHeightMap[ index10 ];
		float height11 = pHeightMap[ index11 ];

		float minHeight = height00;
		float maxHeight = height00;

		if ( height01 < minHeight ) minHeight = height01;
		else maxHeight = height01;

		if ( height10 < minHeight ) minHeight = height10;
		else if ( height10 > maxHeight ) maxHeight = height10;

		if ( height11 < minHeight ) minHeight = height11;
		else if ( height11 > maxHeight ) maxHeight = height11;

		float rayMaxHeight = startHeight;
		float rayMinHeight = startHeight;

		if ( endHeight < startHeight ) rayMinHeight = endHeight;
		else rayMaxHeight = endHeight;

		// put back once better raycast against triangles solved
		//if ( rayMaxHeight < minHeight || rayMinHeight > maxHeight ) return 0;

		height00 *= worldToPhysScale;
		height01 *= worldToPhysScale;
		height10 *= worldToPhysScale;
		height11 *= worldToPhysScale;

		float oldDist = rayCastCallback->m_hitFraction;
		btVector3 vertices[ 3 ];

		float fX0 = (x0 * physSegmentSize) + (offsetX * worldToPhysScale);
		float fZ0 = (z0 * physSegmentSize) + (offsetZ * worldToPhysScale);
		float fX1 = fX0 + physSegmentSize;
		float fZ1 = fZ0 + physSegmentSize;

		// special edge tolerance to handle very glancing rays against the terrain (not ideal!)
		rayCastCallback->m_fEdgeTolerance = fEdgeThreshold;

		vertices[ 0 ].setValue( fX0, height00, fZ0 );
		vertices[ 1 ].setValue( fX0, height01, fZ1 );
		vertices[ 2 ].setValue( fX1, height10, fZ0 );

		rayCastCallback->processTriangle( vertices, 2*x0, z0 );

		vertices[ 0 ] = vertices[ 2 ];
		vertices[ 2 ].setValue( fX1, height11, fZ1 );

		rayCastCallback->processTriangle( vertices, 2*x0 + 1, z0 );

		// and restore just in case it messes future raycasts
		rayCastCallback->m_fEdgeTolerance = 0;

		// return result
		if ( rayCastCallback->m_hitFraction < oldDist ) 
			return 1;
		else 
			return 0;
	}

	int PhysicsRayCast( void* callback, float worldToPhysScale, RAY& ray, float startDist, float endDist )
	{
		float beginX = ray.origin.x + ray.direction.x * startDist;
		float beginY = ray.origin.y + ray.direction.y * startDist;
		float beginZ = ray.origin.z + ray.direction.z * startDist;

		float endX = ray.origin.x + ray.direction.x * endDist;
		float endY = ray.origin.y + ray.direction.y * endDist;
		float endZ = ray.origin.z + ray.direction.z * endDist;

		beginX -= offsetX;
		beginZ -= offsetZ;
		endX -= offsetX;
		endZ -= offsetZ;

		if ( beginX < 0 ) beginX = 0;
		if ( beginZ < 0 ) beginZ = 0;

		int iBeginX = (int) (beginX / segSize);
		int iBeginZ = (int) (beginZ / segSize);

		float dirX = ray.direction.x;
		float dirY = ray.direction.y;
		float dirZ = ray.direction.z;
		float absDirX = abs(ray.direction.x);
		float absDirY = abs(ray.direction.y);
		float absDirZ = abs(ray.direction.z);

		// edge detect threshold - expand edge detect when ray glancing across face of terrain polygons (not ideal, some issue somewhere!)
		float fEdgeThreshold = 0.0f;
		if (absDirY < 0.2f) fEdgeThreshold = 0.05f;

		if ( absDirX < 0.001f && absDirZ < 0.001f )
		{
			// single segment check
			return PhysicsRayCastSegment( callback, worldToPhysScale, iBeginX, iBeginZ, beginY, endY, fEdgeThreshold);
		}

		if ( absDirZ > absDirX )
		{
			float stepX = (absDirX / absDirZ) * segSize;
			float stepY = (absDirY / absDirZ) * segSize;
			float stepZ = segSize;
			int iStepX = 1;
			int iStepZ = 1;
			float limitX;
			float currY = beginY + stepY;
			float prevY = beginY;

			if ( dirX < 0 ) 
			{ 
				iStepX = -1; 
				limitX = (iBeginX * segSize) - beginX;
			}
			else
			{
				limitX = beginX - (iBeginX * segSize) - segSize;
			}

			if ( dirY < 0 ) stepY = -stepY;

			float edgeDistZ = ((iBeginZ + 1) * segSize) - beginZ;
			if ( dirZ < 0 ) 
			{ 
				stepZ = -stepZ; 
				iStepZ = -1; 
				edgeDistZ = segSize - edgeDistZ;
			}	
			float edgeDistX = edgeDistZ * absDirX / absDirZ;

			while ((iStepZ == 1 && beginZ < endZ) || (iStepZ == -1 && beginZ > endZ))
			{
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX - 1, iBeginZ - 1, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX, iBeginZ - 1, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX + 1, iBeginZ - 1, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX - 1, iBeginZ, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX, iBeginZ, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX + 1, iBeginZ, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX - 1, iBeginZ + 1, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX, iBeginZ + 1, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX + 1, iBeginZ + 1, currY, prevY, fEdgeThreshold)) return 1;

				limitX += stepX;
				if ( limitX >= segSize)
				{
					limitX -= segSize;
					iBeginX += iStepX;
				}
				prevY = currY;
				currY += stepY;
				iBeginZ += iStepZ;
				beginZ += stepZ;
			}
		}
		else
		{
			float stepX = segSize;
			float stepY = (absDirY / absDirX) * segSize;
			float stepZ = fabs((absDirZ / absDirX) * segSize);
			int iStepX = 1;
			int iStepZ = 1;
			float limitZ;
			float currY = beginY + stepY;
			float prevY = beginY;

			if ( dirZ < 0 ) 
			{ 
				iStepZ = -1; 
				limitZ = (iBeginZ * segSize) - beginZ;
			}
			else
			{
				limitZ = beginZ - (iBeginZ * segSize) - segSize;
			}

			if ( dirY < 0 ) stepY = -stepY;

			float edgeDistX = ((iBeginX + 1) * segSize) - beginX;
			if ( dirX < 0 ) 
			{ 
				stepX = -stepX; 
				iStepX = -1; 
				edgeDistX = segSize - edgeDistX;
			}	
			float edgeDistZ = edgeDistX * absDirZ / absDirX;

			while( (iStepX == 1 && beginX < endX) || (iStepX == -1 && beginX > endX) )
			{
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX - 1, iBeginZ - 1, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX, iBeginZ - 1, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX + 1, iBeginZ - 1, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX - 1, iBeginZ, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX, iBeginZ, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX + 1, iBeginZ, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX - 1, iBeginZ + 1, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX, iBeginZ + 1, currY, prevY, fEdgeThreshold)) return 1;
				if (PhysicsRayCastSegment(callback, worldToPhysScale, iBeginX + 1, iBeginZ + 1, currY, prevY, fEdgeThreshold)) return 1;

				limitZ += stepZ;
				if ( limitZ >= segSize)
				{
					limitZ -= segSize;
					iBeginZ += iStepZ;
				}
				prevY = currY;
				currY += stepY;
				iBeginX += iStepX;
				beginX += stepX;
			}
		}

		return 0;
	}
	
	void SetGenerateData( float x, float z, float chunkSegSize, uint8_t flags )
	{
		offsetX = x;
		offsetZ = z;
		segSize = chunkSegSize;
		genFlags = flags;

		uint32_t numSegments = ggterrain_local_params.segments_per_chunk;

		numVertices = (numSegments+1) * (numSegments+1);
		numIndices = (numSegments*2 + 3) * numSegments - 1;

		if ( pVertices ) delete [] pVertices;
		if ( pIndices ) delete [] pIndices;

		pVertices = new TerrainVertex[ numVertices ];
		pIndices = new uint16_t[ numIndices ];

		SetGenerating( 1 );
		//uint8_t lodLevel = (flags >> 4) & 0xF;
	}

	const AABB* GetBounds()
	{
		return &aabbBounds;
	}

	static uint32_t lerpNormal( uint32_t iN1, uint32_t iN2, float t )
	{
		float n1x = (iN1 & 0xFF) / 127.5f - 1.0f;
		float n1y = ((iN1 >> 8) & 0xFF) / 127.5f - 1.0f;
		float n1z = ((iN1 >> 16) & 0xFF) / 127.5f - 1.0f;

		float n2x = (iN2 & 0xFF) / 127.5f - 1.0f;
		float n2y = ((iN2 >> 8) & 0xFF) / 127.5f - 1.0f;
		float n2z = ((iN2 >> 16) & 0xFF) / 127.5f - 1.0f;

		float nx = n1x + (n2x - n1x)*t;
		float ny = n1y + (n2y - n1y)*t;
		float nz = n1z + (n2z - n1z)*t;

		float length = nx*nx + ny*ny + nz*nz;
		if ( length > 0 ) length = 1.0f / sqrt(length);

		nx *= length;
		ny *= length;
		nz *= length;

		int iNx = (int) (nx * 127.5f + 127.5f);
		int iNy = (int) (ny * 127.5f + 127.5f);
		int iNz = (int) (nz * 127.5f + 127.5f);

		if ( iNx > 255 ) iNx = 255;
		if ( iNy > 255 ) iNy = 255;
		if ( iNz > 255 ) iNz = 255;

		if ( iNx < 0 ) iNx = 0;
		if ( iNy < 0 ) iNy = 0;
		if ( iNz < 0 ) iNz = 0;

		uint32_t normal = (iNz << 16) | (iNy << 8) | iNx;
		return normal;
	}

	// may be called from a thread
	void Generate()
	{
		unsigned char xNegStitch = (genFlags & 0x01);
		unsigned char xPosStitch = (genFlags >> 1) & 0x01;
		unsigned char zNegStitch = (genFlags >> 2) & 0x01;
		unsigned char zPosStitch = (genFlags >> 3) & 0x01;
		unsigned char lodLevel = genFlags >> 4;

		float minHeight = 10000000000;
		float maxHeight = -10000000000;

		//uint32_t palette[ 8 ] = { 0xFF28B380, 0xFF0C491D, 0xFF6CF1AB, 0xFF2B6035, 0xFF006440, 0xFF147232, 0xFF1FDA48, 0xFF61B487 };
		//uint32_t palette[ 4 ] = { 0xFF0C491D, 0xFF2B6035, 0xFF147232, 0xFF61B487 };
		//uint32_t palette[ 8 ] = { 0xFF10201F, 0xFF334347, 0xFF44595F, 0xFF71969A, 0xFF89B5B9, 0xFF225552, 0xFF37716C, 0xFF588787 };

		uint32_t numSegments = ggterrain_local_params.segments_per_chunk;
		uint32_t numVerts = numSegments + 1;

		// vertices
		for ( uint32_t z = 0; z <= numSegments; z++ )
		{
			for ( uint32_t x = 0; x <= numSegments; x++ )
			{
				uint32_t index = (z * numVerts) + x;

				pVertices[ index ].ID = (lodLevel << 16) | (z << 8) | x;

				float fX = (x * segSize) + offsetX;
				float fZ = (z * segSize) + offsetZ;

				pVertices[ index ].x = fX;
				pVertices[ index ].z = fZ;
				float heightNoFlat = 0;
				float height = CalculateHeightWithHeightmap( fX, fZ, 1, &heightNoFlat );
				pVertices[ index ].y = height;
				if ( height < minHeight ) minHeight = height;
				if ( height > maxHeight ) maxHeight = height;

				uint32_t indexMap = (numSegments - z) * numVerts + x;
				pHeightMap[ indexMap ] = height;
				pHeightMapNoFlat[ indexMap ] = heightNoFlat;
			}
		}

		// normals
		for ( uint32_t z = 0; z <= numSegments; z++ )
		{
			uint32_t z1 = z; if ( z > 0 ) z1--;
			uint32_t z2 = z; if ( z < numSegments ) z2++;

			for ( uint32_t x = 0; x <= numSegments; x++ )
			{
				float fX = (x * segSize) + offsetX;
				float fZ = (z * segSize) + offsetZ;

				uint32_t x1 = x; if ( x > 0 ) x1--;
				uint32_t x2 = x; if ( x < numSegments ) x2++;
			
				uint32_t index1, index2;
				float diffX, diffZ;
				float nx, ny, nz;

				index1 = (z * numVerts) + x1;
				index2 = (z * numVerts) + x2;
				float height1 = pVertices[index1].y;
				float height2 = pVertices[index2].y;
				if ( x == 0 ) height1 = CalculateHeightWithHeightmap( fX - segSize, fZ );
				if ( x == numSegments ) height2 = CalculateHeightWithHeightmap( fX + segSize, fZ );
				diffX = height2 - height1;
				nx = -diffX / segSize;

				index1 = (z1 * numVerts) + x;
				index2 = (z2 * numVerts) + x;
				height1 = pVertices[index1].y;
				height2 = pVertices[index2].y;
				if ( z == 0 ) height1 = CalculateHeightWithHeightmap( fX, fZ - segSize );
				if ( z == numSegments ) height2 = CalculateHeightWithHeightmap( fX, fZ + segSize );
				diffZ = height2 - height1;
				nz = -diffZ / segSize;

				ny = 2;
						
				float length = nx*nx + ny*ny + nz*nz;
				if ( length > 0 ) length = 1.0f / sqrt(length);

				nx *= length;
				ny *= length;
				nz *= length;

				int iNx = (int) (nx * 127.5f + 127.5f);
				int iNy = (int) (ny * 127.5f + 127.5f);
				int iNz = (int) (nz * 127.5f + 127.5f);

				if ( iNx > 255 ) iNx = 255;
				if ( iNy > 255 ) iNy = 255;
				if ( iNz > 255 ) iNz = 255;

				if ( iNx < 0 ) iNx = 0;
				if ( iNy < 0 ) iNy = 0;
				if ( iNz < 0 ) iNz = 0;

				uint32_t normal = (iNz << 16) | (iNy << 8) | iNx;
				
				uint32_t subIndex = (z*numVerts) + x;
				pVertices[ subIndex ].inormal = normal;

				uint32_t indexMap = (numSegments - z) * numVerts + x;
				pNormalMap[ indexMap ] = normal;
			}
		}

		if ( xPosStitch )
		{
			for ( uint32_t z = 1; z < numSegments; z += 2 )
			{
				uint32_t x = numSegments;
				uint32_t index = (z * numVerts) + x;
				uint32_t index1 = index - numVerts;
				uint32_t index2 = index + numVerts;

				uint32_t indexMap = (numSegments - z) * numVerts + x;
				uint32_t indexMap1 = indexMap + numVerts; // opposite of above
				uint32_t indexMap2 = indexMap - numVerts;

				pVertices[ index ].y = (pVertices[ index1 ].y + pVertices[ index2 ].y) / 2.0f;
				pHeightMap[ indexMap ] = pVertices[ index ].y;
				
				pNormalMap[ indexMap ] = lerpNormal( pNormalMap[ indexMap1 ], pNormalMap[ indexMap2 ], 0.5f );
				pVertices[ index ].inormal = pNormalMap[ indexMap ];
			}
		}

		if ( xNegStitch )
		{
			for ( uint32_t z = 1; z < numSegments; z += 2 )
			{
				uint32_t x = 0;
				uint32_t index = (z * numVerts) + x;
				uint32_t index1 = index + numVerts;
				uint32_t index2 = index - numVerts;

				uint32_t indexMap = (numSegments - z) * numVerts + x;
				uint32_t indexMap1 = indexMap - numVerts; // opposite of above
				uint32_t indexMap2 = indexMap + numVerts;

				pVertices[ index ].y = (pVertices[ index1 ].y + pVertices[ index2 ].y) / 2.0f;
				pHeightMap[ indexMap ] = pVertices[ index ].y;
				
				pNormalMap[ indexMap ] = lerpNormal( pNormalMap[ indexMap1 ], pNormalMap[ indexMap2 ], 0.5f );
				pVertices[ index ].inormal = pNormalMap[ indexMap ];
			}
		}

		if ( zPosStitch )
		{
			for ( uint32_t x = 1; x < numSegments; x += 2 )
			{
				uint32_t z = numSegments;
				uint32_t index = (z * numVerts) + x;
				uint32_t index1 = index - 1;
				uint32_t index2 = index + 1;

				uint32_t indexMap = (numSegments - z) * numVerts + x;
				uint32_t indexMap1 = indexMap - 1; // same as above
				uint32_t indexMap2 = indexMap + 1;

				pVertices[ index ].y = (pVertices[ index1 ].y + pVertices[ index2 ].y) / 2.0f;
				pHeightMap[ indexMap ] = pVertices[ index ].y;

				pNormalMap[ indexMap ] = lerpNormal( pNormalMap[ indexMap1 ], pNormalMap[ indexMap2 ], 0.5f );
				pVertices[ index ].inormal = pNormalMap[ indexMap ];
			}
		}

		if ( zNegStitch )
		{
			for ( uint32_t x = 1; x < numSegments; x += 2 )
			{
				uint32_t z = 0;
				uint32_t index = (z * numVerts) + x;
				uint32_t index1 = index + 1;
				uint32_t index2 = index - 1;

				uint32_t indexMap = (numSegments - z) * numVerts + x;
				uint32_t indexMap1 = indexMap + 1; // same as above
				uint32_t indexMap2 = indexMap - 1;

				pVertices[ index ].y = (pVertices[ index1 ].y + pVertices[ index2 ].y) / 2.0f;
				pHeightMap[ indexMap ] = pVertices[ index ].y;

				pNormalMap[ indexMap ] = lerpNormal( pNormalMap[ indexMap1 ], pNormalMap[ indexMap2 ], 0.5f );
				pVertices[ index ].inormal = pNormalMap[ indexMap ];
			}
		}

		// indices
		uint32_t countI = 0;
		for ( uint32_t z = 0; z < numSegments; z++ )
		{
			pIndices[ countI++ ] = z * (numSegments+1);
			pIndices[ countI++ ] = (z+1) * (numSegments+1);

			for ( uint32_t x = 0; x < numSegments; x++ )
			{
				pIndices[ countI++ ] = z*(numSegments+1) + x+1;
				pIndices[ countI++ ] = (z+1)*(numSegments+1) + x+1;
			}

			if ( z < numSegments-1 ) 
			{
				pIndices[ countI++ ] = 0xFFFF; // primitive restart
			}
		}

		// update bounds
		aabbBounds._min.x = offsetX;
		aabbBounds._min.y = minHeight;
		aabbBounds._min.z = offsetZ;

		aabbBounds._max.x = offsetX + (segSize * ggterrain_local_params.segments_per_chunk);
		aabbBounds._max.y = maxHeight;
		aabbBounds._max.z = offsetZ + (segSize * ggterrain_local_params.segments_per_chunk);

		/*
		uint32_t* pX = (uint32_t*) &offsetX;
		uint32_t* pZ = (uint32_t*) &offsetZ;
		uint32_t iX = (*pX) << 16;
		uint32_t iZ = (*pZ) & 0xFFFF;
		uint32_t iRandSeed = iX | iZ;

		// chunk texture
		for( uint32_t y = 0; y < texSize; y++ )
		{
			for( uint32_t x = 0; x < texSize; x++ )
			{
				//uint32_t index = (y * texSize) + x;
				
				float fX = x + offsetX;
				float fZ = fZ / 10000.0f;

				uint8_t paletteIndex[8];
				
				frequency = 1;
				amplitude = 1;
				float red = 0;
				float green = 0;
				float blue = 0;
				for ( size_t i = 0; i < 8; i++ ) 
				{
					float bit0 = noise.noise( frequency * fX2, frequency * fZ2 );
					float bit1 = noise.noise( frequency * fX2 + 401.563, frequency * fZ2 + 379.798 );
					float bit2 = noise.noise( frequency * fX2 + 743.213, frequency * fZ2 + 951.294 );

					uint8_t iBit0 = (bit0 >= 0) ? 1 : 0;
					uint8_t iBit1 = (bit1 >= 0) ? 1 : 0;
					uint8_t iBit2 = (bit2 >= 0) ? 1 : 0;
					paletteIndex[ i ] = (iBit2 << 2) | (iBit1 << 1) | iBit0;
										
					frequency *= 2.0f;
					amplitude *= 0.5f;
				}
				
				iRandSeed = (214013*iRandSeed + 2531011);
				uint32_t randValue = (iRandSeed >> 16) & 0xffff;
				float randIndex = randValue / 65536.0f;
				randIndex = randIndex * randIndex;
				uint32_t iRandIndex = (int) (randIndex * 8);

				pVertices[ index ].color = palette[ paletteIndex[iRandIndex] ];
				

				//uint8_t iRed = (uint8_t) (red * 256);
				//uint8_t iGreen = (uint8_t) (green * 256);
				//uint8_t iBlue = (uint8_t) (blue * 256);
				//pVertices[ index ].color = 0xFF000000 | (iBlue << 16) | (iGreen << 8) | iRed;
				
				
				//uint8_t iBit0 = (bit0 >= 0) ? 1 : 0;
				//uint8_t iBit1 = (bit1 >= 0) ? 1 : 0;
				//uint8_t iBit2 = (bit2 >= 0) ? 1 : 0;
				//uint8_t colorIndex = (iBit2 << 2) | (iBit1 << 1) | iBit0;
				//pVertices[ index ].color = palette[ colorIndex ];
			}
		}
		*/
	}

	// must not be called on a thread
	void GenerateGPUBuffers()
	{
		if ( !pVertices || !pIndices ) return;

		GraphicsDevice* device = wiRenderer::GetDevice();

		// index buffer
		SubresourceData data = {};
		data.pSysMem = pIndices;

		GPUBufferDesc bd = {};
		bd.ByteWidth = sizeof(unsigned short) * numIndices;
		bd.BindFlags = BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		device->CreateBuffer( &bd, &data, &indexBuffer );

		delete [] pIndices;
		pIndices = 0;

		// vertex buffer
		data.pSysMem = pVertices;
		bd.ByteWidth = sizeof(TerrainVertex) * numVertices;
		bd.BindFlags = BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		device->CreateBuffer( &bd, &data, &vertexBuffer );

		delete [] pVertices;
		pVertices = 0;
	}
			
	GGTerrainChunk() 
	{
		uint32_t numSegments = ggterrain_local_params.segments_per_chunk;
		uint32_t numVertices = (numSegments+1) * (numSegments+1);
		pHeightMap = new float[ numVertices ];
		pHeightMapNoFlat = new float[ numVertices ];
		pNormalMap = new uint32_t[ numVertices ];
	}

	~GGTerrainChunk() 
	{
		if ( pHeightMap ) delete [] pHeightMap;
		if ( pHeightMapNoFlat ) delete [] pHeightMapNoFlat;
		if ( pNormalMap ) delete [] pNormalMap;

		if ( pVertices ) delete [] pVertices;
		if ( pIndices ) delete [] pIndices;

		pVertices = 0;
		pIndices = 0;
	}
};

class ChunkGenerator : public GGThread
{
protected:
	static GGTerrainChunk* pWaitingList;
	static threadLock waitingLock;
	static threadCondition workReady;
	
	static ChunkGenerator** pThreads;
	static uint32_t iNumThreads;

	
	uint32_t numSegmentsCached = 0; // record of how many segments were used when the pVertices and pIndices arrays were created

public:

	static void AddChunk( GGTerrainChunk* pChunk )
	{
		extern int ggterrain_safe_gpu_singlethread;
		if (ggterrain_safe_gpu_singlethread)
		{
			// Safe mode: generate on the calling (main) thread. No worker thread
			// ever calls a D3D device method, so the cross-thread race is gone.
			pChunk->pNextChunk = 0;
			pChunk->Generate();            // pure CPU
			pChunk->GenerateGPUBuffers();  // D3D CreateBuffer, now on main thread (as its comment requires)
			MemoryBarrier();
			pChunk->SetGenerating(0);
			return;
		}

		waitingLock.Acquire();

#ifdef _DEBUG
		// check we aren't adding a chunk more than once to the waiting list, this should never happen
		GGTerrainChunk* pCheck = pWaitingList;
		while( pCheck )
		{
			assert( pCheck != pChunk );
			pCheck = pCheck->pNextChunk;
		}
#endif
		// add to beginning
		pChunk->pNextChunk = pWaitingList;
		pWaitingList = pChunk;

		waitingLock.Release();		

		// tell any sleeping threads that there is work
		workReady.Signal();
	}

	static void StopAndFinishAll()
	{
		// stop all threads and wait for them to finish
		for( uint32_t i = 0; i < iNumThreads; i++ ) pThreads[i]->Stop();
		workReady.Broadcast();
		for( uint32_t i = 0; i < iNumThreads; i++ ) pThreads[i]->Join();

		// clear the waiting list, don't need locks here as everything is stopped
		pWaitingList = 0;
	}

	static void StartThreads()
	{
		for( uint32_t i = 0; i < iNumThreads; i++ ) pThreads[i]->Start();
	}

	static void SetThreads( uint32_t numThreads )
	{
		if ( numThreads == iNumThreads ) return;

		// delete any surplus threads
		for( uint32_t i = numThreads; i < iNumThreads; i++ ) pThreads[i]->Stop();
		for( uint32_t i = numThreads; i < iNumThreads; i++ ) 
		{
			pThreads[i]->Join();
			delete pThreads[i];
		}

		// copy old threads to new array
		ChunkGenerator** pNewThreads = new ChunkGenerator*[ numThreads ];
		uint32_t minThreads = numThreads;
		if ( iNumThreads < minThreads ) minThreads = iNumThreads;
		for( uint32_t i = 0; i < minThreads; i++ ) pNewThreads[ i ] = pThreads[ i ];

		// generate any new threads
		for( uint32_t i = iNumThreads; i < numThreads; i++ )
		{
			pNewThreads[ i ] = new ChunkGenerator();
			pNewThreads[ i ]->Start();
		}

		iNumThreads = numThreads;
		if ( pThreads ) delete [] pThreads;
		pThreads = pNewThreads;
	}

	uint32_t Run( ) 
	{
		// profiling
		#ifdef OPTICK_ENABLE
		OPTICK_THREAD("GGTerrain"); 
		#endif

		while( !bTerminate )
		{
			waitingLock.Acquire();
			while ( !bTerminate && !pWaitingList )
			{
				workReady.WaitCustom( waitingLock );
			}

			if ( bTerminate ) 
			{
				waitingLock.Release();
				return 0;
			}

			GGTerrainChunk* pChunk = 0;
			if ( pWaitingList ) 
			{
				pChunk = pWaitingList;
				pWaitingList = pWaitingList->pNextChunk;
			}
			
			waitingLock.Release();

			if ( pChunk ) 
			{
				pChunk->Generate();
				pChunk->GenerateGPUBuffers();
				MemoryBarrier(); // prevent pChunk->status being set before all previous data has been written, can happen with CPU instruction reordering
				pChunk->SetGenerating( 0 );
			}
		}

		return 0;
	}

	ChunkGenerator() : GGThread() 
	{
		
	}

	~ChunkGenerator() 
	{
		
	}
};

GGTerrainChunk* ChunkGenerator::pWaitingList = 0;
threadLock ChunkGenerator::waitingLock;
threadCondition ChunkGenerator::workReady;
uint32_t ChunkGenerator::iNumThreads = 0;
ChunkGenerator** ChunkGenerator::pThreads = 0;

class FractalGenerator : public GGThread
{
protected:
	static uint8_t* pData;
	static volatile int iNextRow;
	static uint32_t iHeight;
	static uint32_t iWidth;
	static threadLock indexLock;
	
	static FractalGenerator* pThreads;
	static uint32_t iNumThreads;

public:

	static void SetWork( uint8_t *data, uint32_t width, uint32_t height )
	{
		pData = data;
		iNextRow = 0;
		iHeight = height;
		iWidth = width;
	}

	static void WaitForAll()
	{
		for( uint32_t i = 0; i < iNumThreads; i++ ) pThreads[i].Join();
	}

	static void StartThreads()
	{
		for( uint32_t i = 0; i < iNumThreads; i++ ) pThreads[i].Start();
	}

	static void SetThreads( uint32_t numThreads )
	{
		if ( numThreads == iNumThreads ) return;
		if ( pThreads ) delete [] pThreads;
		
		pThreads = new FractalGenerator[ numThreads ];
		iNumThreads = numThreads;
	}

	uint32_t Run( ) 
	{
		// profiling
#ifdef OPTICK_ENABLE
		OPTICK_THREAD("GGTerrain");
#endif

		while( 1 )
		{
			if ( bTerminate ) return 0;
			
			uint32_t localIndex;
			while( !indexLock.Acquire() );
			localIndex = iNextRow;
			iNextRow++;
			indexLock.Release();

			if ( localIndex >= iHeight ) return 0;

			float fY = localIndex / 15.0f;
			for( uint32_t x = 0; x < iWidth; x++ )
			{
				float fX = x / 15.0f;

				int bit0 = noise.fractal( 12, fX, fY ) >= 0 ? 1 : 0;
				int bit1 = noise.fractal( 12, fX+567, fY+833 ) >= 0 ? 2 : 0;
				int bit2 = noise.fractal( 12, fX+1231, fY+967 ) >= 0 ? 4 : 0;
				int bit3 = noise.fractal( 12, fX-96, fY-3687 ) >= 0 ? 8 : 0;
				int bit4 = noise.fractal( 12, fX-1218, fY-321 ) >= 0 ? 16 : 0;

				uint8_t value = bit4 | bit3 | bit2 | bit1 | bit0;
						
				uint32_t index = (localIndex * iWidth) + x;
				pData[ index ] = value;
			}
		}

		return 0;
	}

	FractalGenerator() : GGThread() 
	{
		
	}

	~FractalGenerator() 
	{
		if ( pThreads ) delete [] pThreads;
	}
};

uint8_t* FractalGenerator::pData = 0;
volatile int FractalGenerator::iNextRow = 0;
uint32_t FractalGenerator::iHeight = 0;
uint32_t FractalGenerator::iWidth = 0;
threadLock FractalGenerator::indexLock;
FractalGenerator* FractalGenerator::pThreads = 0;
uint32_t FractalGenerator::iNumThreads = 0;

#define GGTERRAIN_LOD_LEVEL_GENERATING       0x01
#define GGTERRAIN_LOD_LEVEL_REGENERATE       0x02
#define GGTERRAIN_LOD_LEVEL_HAS_REGENERATED  0x04

class GGTerrainLODLevel
{
public:

	GGTerrainChunk* chunkGrid[ 64 ] = { 0 };
	float centerX = 0;
	float centerZ = 0;
	int shiftedX = 0;
	int shiftedZ = 0;
	float segSize = 0; // varies by LOD level
	uint32_t flags = 0;
	AABB aabbBounds;
	
	int GetChunk( float x, float z, GGTerrainChunk** pOutChunk )
	{
		uint32_t numSegments = ggterrain_local_params.segments_per_chunk;
		float chunkSize = segSize * numSegments;
		float LODSize = chunkSize * 8;
		float LODHalfSize = LODSize / 2.0f;

		x -= centerX;
		z -= centerZ;

		if ( x >= LODHalfSize || x < -LODHalfSize || z >= LODHalfSize || z < -LODHalfSize ) return -1;

		int iX = (int) ((x + LODHalfSize) / chunkSize);
		int iZ = (int) ((z + LODHalfSize) / chunkSize);

		assert( iX >= 0 );
		assert( iZ >= 0 );
		assert( iX < 8 );
		assert( iZ < 8 );
		if ( iX >= 8 || iZ >= 8 ) return -1;

		uint8_t index = iZ * 8 + iX;

		if ( pOutChunk ) *pOutChunk = chunkGrid[ index ];
		return index;
	}

	GGTerrainChunk* RayCastBox( RAY ray, float* dist )
	{
		float minx = aabbBounds._min.x;
		float miny = aabbBounds._min.y;
		float minz = aabbBounds._min.z;
		float maxx = aabbBounds._max.x;
		float maxy = aabbBounds._max.y;
		float maxz = aabbBounds._max.z;

		float tmin1, tmax1, tmin2, tmax2;

		if ( ray.direction.x == 0 )
		{
			tmin1 = (minx - ray.origin.x) * 1e20f;
			tmax1 = (maxx - ray.origin.x) * 1e20f;
		}
		else if ( ray.direction.x > 0 )
		{
			tmin1 = (minx - ray.origin.x) * ray.direction_inverse.x;
			tmax1 = (maxx - ray.origin.x) * ray.direction_inverse.x;
		}
		else 
		{
			tmax1 = (minx - ray.origin.x) * ray.direction_inverse.x;
			tmin1 = (maxx - ray.origin.x) * ray.direction_inverse.x;
		}
    
		if ( ray.direction.y == 0 ) 
		{
			tmin2 = (miny - ray.origin.y) * 1e20f;
			tmax2 = (maxy - ray.origin.y) * 1e20f;
		}
		else if ( ray.direction.y > 0 ) 
		{
			tmin2 = (miny - ray.origin.y) * ray.direction_inverse.y;
			tmax2 = (maxy - ray.origin.y) * ray.direction_inverse.y;
		}
		else 
		{
			tmax2 = (miny - ray.origin.y) * ray.direction_inverse.y;
			tmin2 = (maxy - ray.origin.y) * ray.direction_inverse.y;
		}
               
		if ( (tmin1 > tmax2) || (tmin2 > tmax1) ) return 0;
    
		if ( tmin2 > tmin1 ) tmin1 = tmin2;
		if ( tmax2 < tmax1 ) tmax1 = tmax2;
    
		if ( ray.direction.z == 0 )
		{
			tmin2 = (minz - ray.origin.z) * 1e20f;
			tmax2 = (maxz - ray.origin.z) * 1e20f;
		}
		else if ( ray.direction.z > 0 ) 
		{
			tmin2 = (minz - ray.origin.z) * ray.direction_inverse.z;
			tmax2 = (maxz - ray.origin.z) * ray.direction_inverse.z;
		}
		else 
		{
			tmax2 = (minz - ray.origin.z) * ray.direction_inverse.z;
			tmin2 = (maxz - ray.origin.z) * ray.direction_inverse.z;
		}
    
		if ( (tmin1 > tmax2) || (tmin2 > tmax1) ) return 0;
                                      
		if ( tmin2 > tmin1 ) tmin1 = tmin2;
		if ( tmax2 < tmax1 ) tmax1 = tmax2;
    
		if ( tmax1 < 0.0 ) return 0;
    
		if ( dist ) *dist = tmin1;

		tmin1 += segSize * 0.1f;
		float x = ray.origin.x + ray.direction.x * tmin1;
		float z = ray.origin.z + ray.direction.z * tmin1;

		GGTerrainChunk* pChunk = 0;
		int result = GetChunk( x, z, &pChunk );
		assert( result >= 0 );

		return pChunk;
	}

	int PhysicsProcessVertices( void* callback, float worldToPhysScale, const AABB* aabbCheck, bool first )
	{
		int result = 0; // 0 if aabbCheck is completely inside this LOD's XZ bounds, otherwise 1

		int numSegments = (int) ggterrain_local_params.segments_per_chunk;
		int totalSegments = numSegments * 8;

		AABB aabbCheckFitted;
		aabbCheckFitted._min = aabbCheck->_min;
		aabbCheckFitted._max = aabbCheck->_max;

		int iStartX, iStartZ, iEndX, iEndZ;

		// check X min
		if ( aabbCheckFitted._min.x <= aabbBounds._min.x )
		{
			result = 1;
			aabbCheckFitted._min.x = aabbBounds._min.x;
			iStartX = 0;
		}
		else
		{
			iStartX = (int) ((aabbCheckFitted._min.x - aabbBounds._min.x) / segSize);
			if ( iStartX > totalSegments ) iStartX = totalSegments;
		}

		// check X max
		if ( aabbCheckFitted._max.x >= aabbBounds._max.x )
		{
			result = 1;
			aabbCheckFitted._max.x = aabbBounds._max.x;
			iEndX = totalSegments;
		}
		else
		{
			iEndX = (int) ((aabbCheckFitted._max.x - aabbBounds._min.x) / segSize);
			if ( iEndX > totalSegments ) iEndX = totalSegments;
		}

		if ( aabbCheckFitted._max.x <= aabbCheckFitted._min.x ) return result;

		// check Z min
		if ( aabbCheckFitted._min.z < aabbBounds._min.z )
		{
			result = 1;
			aabbCheckFitted._min.z = aabbBounds._min.z;
			iStartZ = 0;
		}
		else
		{
			iStartZ = (int) ((aabbCheckFitted._min.z - aabbBounds._min.z) / segSize);
			if ( iStartZ > totalSegments ) iStartZ = totalSegments;
		}

		// check Z max
		if ( aabbCheckFitted._max.z > aabbBounds._max.z )
		{
			result = 1;
			aabbCheckFitted._max.z = aabbBounds._max.z;
			iEndZ = totalSegments;
		}
		else
		{
			iEndZ = (int) ((aabbCheckFitted._max.z - aabbBounds._min.z) / segSize);
			if ( iEndZ > totalSegments ) iEndZ = totalSegments;
		}

		if ( aabbCheckFitted._max.z <= aabbCheckFitted._min.z ) return result;

		// check Y
		if ( aabbCheckFitted._min.y >= aabbBounds._max.y ) return result;
		if ( aabbCheckFitted._max.y < aabbBounds._min.y ) return result;

		assert(iStartX >= 0);
		assert(iStartZ >= 0);
		assert(iEndX >= 0);
		assert(iEndZ >= 0);

		int iStartChunkX = iStartX / numSegments;
		int iStartChunkZ = iStartZ / numSegments;
		int iEndChunkX = iEndX / numSegments;
		int iEndChunkZ = iEndZ / numSegments;

		if ( iEndChunkX > 7 ) iEndChunkX = 7;
		if ( iEndChunkZ > 7 ) iEndChunkZ = 7;

		for( int chunkZ = iStartChunkZ; chunkZ <= iEndChunkZ; chunkZ++ )
		{
			for( int chunkX = iStartChunkX; chunkX <= iEndChunkX; chunkX++ )
			{
				uint8_t index = chunkZ * 8 + chunkX;
				if (!chunkGrid[index])
					continue;
				if ( !first && !chunkGrid[ index ]->IsVisible() ) continue;

				int iSX = iStartX - (chunkX * numSegments);
				int iSZ = iStartZ - (chunkZ * numSegments);
				int iEX = iEndX - (chunkX * numSegments);
				int iEZ = iEndZ - (chunkZ * numSegments);
				chunkGrid[ index ]->PhysicsProcessVertices( callback, worldToPhysScale, aabbCheckFitted._min.y, aabbCheckFitted._max.y, iSX, iSZ, iEX, iEZ );
			}
		}

		return result;
	}

	void GetTriangleList( UnorderedArray<KMaths::Vector3>* vertices, float minX, float minZ, float maxX, float maxZ, bool first )
	{
		for( int chunkZ = 0; chunkZ < 8; chunkZ++ )
		{
			for( int chunkX = 0; chunkX < 8; chunkX++ )
			{
				uint8_t index = chunkZ * 8 + chunkX;
				GGTerrainChunk* pChunk = chunkGrid[ index ];
				if ( !first && !pChunk->IsVisible() ) continue;
				
				pChunk->GetTriangleList( vertices, minX, minZ, maxX, maxZ );
			}
		}
	}

	bool IsGenerating() { return (flags & GGTERRAIN_LOD_LEVEL_GENERATING) != 0; }
	void SetGenerating() { flags |= GGTERRAIN_LOD_LEVEL_GENERATING; }

	bool ShouldRegenerate() { return (flags & GGTERRAIN_LOD_LEVEL_REGENERATE) != 0; }
	void SetToRegenerate( int regenerate ) 
	{ 
		if ( regenerate ) flags |= GGTERRAIN_LOD_LEVEL_REGENERATE; 
		else flags &= ~GGTERRAIN_LOD_LEVEL_REGENERATE; 
	}

	bool HasRegenerated() { return (flags & GGTERRAIN_LOD_LEVEL_HAS_REGENERATED) != 0; }
	void SetRegenerated( int regenerate ) 
	{ 
		if ( regenerate ) flags |= GGTERRAIN_LOD_LEVEL_HAS_REGENERATED; 
		else flags &= ~GGTERRAIN_LOD_LEVEL_HAS_REGENERATED; 
	}
	
	bool IsReady()
	{
		for( uint32_t i = 0; i < 64; i++ )
		{
			if ( chunkGrid[ i ] )
			{
				if ( chunkGrid[ i ]->IsGenerating() ) return false;
			}			
		}

		flags &= ~GGTERRAIN_LOD_LEVEL_GENERATING;
		return true;
	}

	void UpdateBounds()
	{
		float minHeight = 1000000000.0f;
		float maxHeight = -1000000000.0f;

		for( uint32_t z = 0; z < 8; z++ )
		{
			for( uint32_t x = 0; x < 8; x++ )
			{
				uint32_t index = z * 8 + x;
				GGTerrainChunk* pChunk = chunkGrid[ index ];
				if ( pChunk ) 
				{
					if ( pChunk->GetBounds()->_min.y < minHeight ) minHeight = pChunk->GetBounds()->_min.y;
					if ( pChunk->GetBounds()->_max.y > maxHeight ) maxHeight = pChunk->GetBounds()->_max.y;
				}
			}
		}

		float size = segSize * ggterrain_local_params.segments_per_chunk * 8;
		float halfSize = size / 2.0f;

		aabbBounds._min.x = centerX - halfSize;
		aabbBounds._min.y = minHeight;
		aabbBounds._min.z = centerZ - halfSize;
		aabbBounds._max.x = centerX + halfSize;
		aabbBounds._max.y = maxHeight;
		aabbBounds._max.z = centerZ + halfSize;
	}

	const AABB* GetBounds()
	{
		return &aabbBounds;
	}

	void SetActive()
	{
		for( uint32_t z = 0; z < 8; z++ )
		{
			for( uint32_t x = 0; x < 8; x++ )
			{
				uint32_t index = z * 8 + x;
				GGTerrainChunk* pChunk = chunkGrid[ index ];
				if ( pChunk ) 
				{
					pChunk->SetCopied( 0 );
					bool visible = pChunk->IsFutureVisible();
					pChunk->SetVisible( visible ? 1 : 0 );
				}
			}
		}
	}

	GGTerrainLODLevel() 
	{
		uint32_t segsPerChunk = ggterrain_local_params.segments_per_chunk;
		uint32_t lodSize = segsPerChunk * segsPerChunk * 64;
	}

	~GGTerrainLODLevel() 
	{
		for( uint32_t i = 0; i < 64; i++ )
		{
			if ( chunkGrid[ i ] && !chunkGrid[i]->IsCopied() ) delete chunkGrid[ i ];
			chunkGrid[ i ] = 0;
		}
	}
};

#define GGTERRAIN_LOD_GENERATING   0x01
#define GGTERRAIN_LOD_VALID        0x02
#define GGTERRAIN_LOD_REGENERATE   0x04

class GGTerrainLODSet
{
public:
	GGTerrainLODLevel* pLevels = 0;
	uint32_t numLODLevels = 0;
	uint32_t iFlags = 0;
	AABB aabbBounds;

	uint32_t GetNumLevels() { return numLODLevels; }
	
	bool IsGenerating() { return (iFlags & GGTERRAIN_LOD_GENERATING) != 0; }
	void SetGenerating() 
	{ 
		iFlags |= GGTERRAIN_LOD_GENERATING; 
		for( uint32_t i = 0; i < numLODLevels; i++ )
		{
			pLevels[ i ].SetGenerating();
		}
	}

	bool ShouldRegenerate() { return (iFlags & GGTERRAIN_LOD_REGENERATE) != 0; }
	void SetToRegenerate( int regenerate ) 
	{ 
		if ( regenerate ) iFlags |= GGTERRAIN_LOD_REGENERATE; 
		else iFlags &= ~GGTERRAIN_LOD_REGENERATE; 
	}

	bool IsValid() { return (iFlags & GGTERRAIN_LOD_VALID) != 0; }

	bool IsReady()
	{
		if ( !IsGenerating() ) return true;

		bool bReady = true;
		for( uint32_t i = 0; i < numLODLevels; i++ )
		{
			if ( !pLevels[ i ].IsReady() ) bReady = false; // continue with the loop so every level can update its state
		}

		if ( bReady )
		{
			iFlags &= ~GGTERRAIN_LOD_GENERATING;
			iFlags |= GGTERRAIN_LOD_VALID;
		}
		return bReady;
	}	

	void Create( uint32_t numLevels )
	{
		numLODLevels = ggterrain_local_params.lod_levels;
		pLevels = new GGTerrainLODLevel[ numLODLevels ];
		float lodSegSize = ggterrain_local_params.segment_size;
		for( uint32_t i = 0; i < numLODLevels; i++ ) 
		{
			pLevels[ i ].segSize = lodSegSize;
			lodSegSize *= 2;
		}
	}

	void Destroy()
	{
		if ( pLevels ) delete [] pLevels;
		pLevels = 0;
		numLODLevels = 0;
		iFlags = 0;
	}

	void UpdateBounds()
	{
		float minHeight = 1000000000.0f;
		float maxHeight = -1000000000.0f;

		for( uint32_t i = 0; i < numLODLevels; i++ ) 
		{
			pLevels[ i ].UpdateBounds();
			if ( pLevels[ i ].GetBounds()->_min.y < minHeight ) minHeight = pLevels[ i ].GetBounds()->_min.y;
			if ( pLevels[ i ].GetBounds()->_max.y > maxHeight ) maxHeight = pLevels[ i ].GetBounds()->_max.y;
		}

		const AABB* pLargestBounds = pLevels[ numLODLevels-1 ].GetBounds();

		aabbBounds._min.x = pLargestBounds->_min.x;
		aabbBounds._min.y = minHeight;
		aabbBounds._min.z = pLargestBounds->_min.z;

		aabbBounds._max.x = pLargestBounds->_max.x;
		aabbBounds._max.y = maxHeight;
		aabbBounds._max.z = pLargestBounds->_max.z;
	}

	const AABB* GetBounds() 
	{ 
		return &aabbBounds; 
	}

	int GetChunk( float x, float z, GGTerrainChunk** pOutChunk, uint32_t* outLevel )
	{
		for( uint32_t i = 0; i < numLODLevels; i++ )
		{
			int index = pLevels[ i ].GetChunk( x, z, pOutChunk );
			if ( index >= 0 )
			{
				if ( outLevel ) *outLevel = i;
				return 1;
			}
		}

		return 0;
	}

	void SetActive()
	{
		for( uint32_t i = 0; i < numLODLevels; i++ ) pLevels[ i ].SetActive();
	}

	GGTerrainLODSet() {}
	~GGTerrainLODSet() { Destroy(); }
};

#define GGTERRAIN_FLAG_VALID              0x01
#define GGTERRAIN_FLAG_REGENERATE_CHUNKS  0x02
#define GGTERRAIN_FLAG_SHIFT_PAGE_TABLE   0x04
#define GGTERRAIN_FLAG_REGENERATE_PAGES   0x08

class GGTerrain
{

protected:
	uint32_t iFlags = GGTERRAIN_FLAG_REGENERATE_CHUNKS;
	float lodBoundary = 1.25f; // as a percentage of chunk size. 1 = lod will change immediately when crossing chunk boundaries, higher values allow more movement before switching

	StackArray<GGTerrainChunk*> pChunksUnused; // invisible chunks that can be recycled

	uint32_t currLODArray = 0;
	uint32_t numLODLevels = 0; // higher values make a larger terrain
	GGTerrainLODSet pLODSet[2]; // flip between one and the other like double buffering
	
public:
	
	bool IsValid() { return (iFlags & GGTERRAIN_FLAG_VALID) != 0; }
	bool ShouldShiftPages() { return (iFlags & GGTERRAIN_FLAG_SHIFT_PAGE_TABLE) != 0; }
	bool ShouldRegeneratePages() { return (iFlags & GGTERRAIN_FLAG_REGENERATE_PAGES) != 0; }

	void PagesUpdated() { iFlags &= ~(GGTERRAIN_FLAG_REGENERATE_PAGES | GGTERRAIN_FLAG_SHIFT_PAGE_TABLE); }

	uint32_t GetNumLODLevels() { return numLODLevels; }
	GGTerrainLODSet* GetCurrentLODs() { return &pLODSet[currLODArray]; }
	GGTerrainLODSet* GetNewLODs() { return &pLODSet[ 1-currLODArray ]; }
	bool IsHighDetail( uint32_t level ) { return ((level + 1) >= numLODLevels) ? 0 : 1; }
	uint32_t ConvertToMipLevel( uint32_t detailLevel ) 
	{ 
		if ( numLODLevels == 0 ) return detailLevel;
		int level = detailLevel;
		level -= (numLODLevels - 1);
		if ( level < 0 ) level = 0;
		return (uint32_t) level;
	}
	
	void ResetChunks()
	{
		ChunkGenerator::StopAndFinishAll();

		pLODSet[ 0 ].Destroy();
		pLODSet[ 1 ].Destroy();
		numLODLevels = 0;
		currLODArray = 0;

		for( uint32_t i = 0; i < pChunksUnused.NumItems(); i++ ) delete pChunksUnused[ i ];
		pChunksUnused.Clear();
		pChunksUnused.Resize( 16 );
		
		ChunkGenerator::StartThreads();

		iFlags = GGTERRAIN_FLAG_REGENERATE_CHUNKS; // also clear all other flags
	}

	void CheckParams()
	{
		bool settingsUpdated = false;

		if ( !ggterrain_local_params.IsEqual( &ggterrain_global_params ) ) 
		{
			if ( ggterrain_global_params.lod_levels < 1 ) ggterrain_global_params.lod_levels = 1;
			if ( ggterrain_global_params.lod_levels > 15 ) ggterrain_global_params.lod_levels = 15; // hard limit, design currently assumes 15 will not be exceeded
			if ( ggterrain_global_params.segments_per_chunk & (ggterrain_global_params.segments_per_chunk-1) )
			{
				float power = (float) log2( ggterrain_global_params.segments_per_chunk );
				int nearestPower = (int) (power + 0.5f);
				if ( power > nearestPower ) nearestPower++;
				else nearestPower--;

				ggterrain_global_params.segments_per_chunk = 1;
				while( nearestPower > 0 ) 
				{
					ggterrain_global_params.segments_per_chunk *= 2;
					nearestPower--;
				}
			}

			if ( ggterrain_global_params.segment_size < 0.01f ) ggterrain_global_params.segment_size = 0.01f; // arbitrary limit
			if ( ggterrain_global_params.fractal_levels < 1 ) ggterrain_global_params.fractal_levels = 1;
			if ( ggterrain_global_params.fractal_levels > 14 ) ggterrain_global_params.fractal_levels = 14; // arbitrary limit
			if ( ggterrain_global_params.fractal_freq_increase < 0.001f ) ggterrain_global_params.fractal_freq_increase = 0.001f; // arbitrary limit
			if ( ggterrain_global_params.fractal_freq_weight < 0.001f ) ggterrain_global_params.fractal_freq_weight = 0.001f; // arbitrary limit
			if ( ggterrain_global_params.noise_power < 0.01f ) ggterrain_global_params.noise_power = 0.01f;
			if ( ggterrain_global_params.height < 0.001f ) ggterrain_global_params.height = 0.001f;
		
			if ( !ggterrain_local_params.IsEqual( &ggterrain_global_params ) )
			{
				settingsUpdated = true;

				ResetChunks();

				if ( ggterrain_local_params.pHeightmapMain != ggterrain_global_params.pHeightmapMain && ggterrain_local_params.pHeightmapMain ) delete [] ggterrain_local_params.pHeightmapMain;
				ggterrain_local_params.Copy( &ggterrain_global_params );

				noise.reshuffle( ggterrain_local_params.seed );
		
				// numSegments must be a power of 2
				assert( (ggterrain_local_params.segments_per_chunk & (ggterrain_local_params.segments_per_chunk-1)) == 0 );
		
				numLODLevels = ggterrain_local_params.lod_levels;
				pLODSet[ 0 ].Create( numLODLevels );
				pLODSet[ 1 ].Create( numLODLevels );
		
				if ( numLODLevels > 1 )
				{
					GraphicsDevice* device = wiRenderer::GetDevice();
	
					// height map and normal map per LOD level
					// technically LODSize should be increased by 1, but it's currently a nice power of two, and we shouldn't need the final pixel anyway
					uint32_t LODSize = ggterrain_local_params.segments_per_chunk * 8;  
					GGTerrain_CreateEmptyTexture( LODSize, LODSize, 1, numLODLevels, FORMAT_R32_FLOAT, &texLODHeightMapArray );
					GGTerrain_CreateEmptyTexture( LODSize, LODSize, 1, numLODLevels, FORMAT_R8G8B8A8_UNORM, &texLODNormalMapArray );
				}

				iFlags |= GGTERRAIN_FLAG_VALID;
			}
		}

		if ( !ggterrain_local_render_params.IsEqual( &ggterrain_global_render_params ) ) 
		{
			settingsUpdated = true;
			ggterrain_extra_params.iUpdateGrass = 2;

			ggterrain_local_render_params.Copy( &ggterrain_global_render_params );
			iFlags |= GGTERRAIN_FLAG_REGENERATE_PAGES;
		}

		if ( !ggterrain_local_render_params2.IsEqual( &ggterrain_global_render_params2 ) ) 
		{
			if ( ggterrain_global_render_params2.brushSize < 10 ) ggterrain_global_render_params2.brushSize = 10;
			if ( ggterrain_global_render_params2.brushSize > 7000 ) ggterrain_global_render_params2.brushSize = 7000;

			// no longer cap as we can now have completely empty level for space games
			//allow 0.5-5km.
			//float meterValue = GGTerrain_UnitsToMeters( ggterrain_global_render_params2.editable_size * 2);
			//if ( meterValue < 500 ) ggterrain_global_render_params2.editable_size = GGTerrain_MetersToUnits( 500.0f / 2);
			//if ( meterValue > 5000 ) ggterrain_global_render_params2.editable_size = GGTerrain_MetersToUnits( 5000.0f / 2);

			bool resized = false;
			if ( ggterrain_local_render_params2.readBackTextureReduction != ggterrain_global_render_params2.readBackTextureReduction ) 
			{
				if ( ggterrain_global_render_params2.readBackTextureReduction < 4 ) ggterrain_global_render_params2.readBackTextureReduction = 4;
				resized = true;
			}

			if ( ggterrain_local_render_params2.editable_size != ggterrain_global_render_params2.editable_size ) 
			{
				ggterrain_extra_params.bTerrainChanged = true;
				ggterrain_extra_params.iUpdateTrees = 2;
				ggterrain_extra_params.iUpdateGrass = 2;
			}
			settingsUpdated = true;
			ggterrain_local_render_params2.Copy( &ggterrain_global_render_params2 );

			if ( resized ) GGTerrain_WindowResized();
		}
	}
	
	void UpdateLODTextures( GGTerrainLODSet* pLODs, int fullUpdate )
	{
		GraphicsDevice* device = wiRenderer::GetDevice();

		uint32_t segsPerChunk = ggterrain_local_params.segments_per_chunk;
		uint32_t stride = (segsPerChunk + 1) * sizeof(float);

		for( uint32_t lod = 0; lod < pLODs->GetNumLevels(); lod++ )
		{
			GGTerrainLODLevel* pLevel = &pLODs->pLevels[ lod ];
			if ( !fullUpdate && pLevel->shiftedX == 0 && pLevel->shiftedZ == 0 && !pLevel->HasRegenerated() ) continue;
			if ( pLevel->HasRegenerated() )
			{
				ggterrain_extra_params.bTerrainChanged = true;
				ggterrain_extra_params.iUpdateTrees = 20;
				ggterrain_extra_params.iUpdateGrass = 20;
			}
			pLevel->SetRegenerated( 0 );

			CopyBox dstBox;
			dstBox.front = 0; dstBox.back = 1;
			for( uint32_t y = 0; y < 8; y++ )
			{
				for( uint32_t x = 0; x < 8; x++ )
				{
					uint32_t index = y * 8 + x;

					dstBox.left = x * segsPerChunk; 
					dstBox.right = (x + 1) * segsPerChunk;
					dstBox.top = (7-y) * segsPerChunk; 
					dstBox.bottom = (8-y) * segsPerChunk;

					// technically the edges should be increased by 1, but it's currently a nice power of two, and we shouldn't need the final pixel anyway
					//if ( x == 7 ) dstBox.right++;
					//if ( y == 0 ) dstBox.bottom++;
					
					device->UpdateTexture( &texLODHeightMapArray, 0, lod, &dstBox, pLevel->chunkGrid[ index ]->pHeightMap, stride, -1 ); 
					device->UpdateTexture( &texLODNormalMapArray, 0, lod, &dstBox, pLevel->chunkGrid[ index ]->pNormalMap, stride, -1 ); 
				}
			}
		}
	}

	// returns 0 if no intersection point found or if the terrain is currently regenerating, otherwise returns 1
	int RayCast( RAY ray, float* pOutX, float* pOutY, float* pOutZ, float* pNormX, float* pNormY, float* pNormZ, uint32_t* lodLevel, int includeFlatAreas=1 )
	{
		GGTerrainLODSet* pCurrLODs = GetCurrentLODs();
		if ( !pCurrLODs->IsValid() || pCurrLODs->IsGenerating() ) return 0;
		if ( isnan(ray.direction.x) || isnan(ray.direction.y) || isnan(ray.direction.z) ) return 0;

		// find chunk containing ray origin
		GGTerrainChunk* pChunk = 0;
		int result = pCurrLODs->GetChunk( ray.origin.x, ray.origin.z, &pChunk, lodLevel );

		float currDist = 0;

		// if not found then ray starts outside terrain, ray cast to the first chunk
		if ( !result || !pChunk )
		{
			uint maxLevel = pCurrLODs->GetNumLevels() - 1;
			pChunk = pCurrLODs->pLevels[ maxLevel ].RayCastBox( ray, &currDist );
			if ( lodLevel ) *lodLevel = maxLevel;
			result = 1;
		}

		if ( !pChunk ) return 0;
		
		int count = 0;
		while( result && pChunk && count < 10000 )
		{
			float endDist = pChunk->GetRayExitDist( ray, 0 );
			float entryDist;
			int inHeightRange = pChunk->CheckRayHeight( ray, &entryDist );

			if ( inHeightRange && entryDist <= endDist )
			{
				if ( pChunk->RayCast( ray, currDist, endDist, pOutX, pOutY, pOutZ, pNormX, pNormY, pNormZ, includeFlatAreas ) ) return 1;
			}

			currDist = endDist;

			// find next chunk
			endDist += pChunk->segSize * 0.1f;
			float nextX = ray.origin.x + ray.direction.x * endDist;
			float nextY = ray.origin.y + ray.direction.y * endDist;
			float nextZ = ray.origin.z + ray.direction.z * endDist;

			if ( nextY > pCurrLODs->GetBounds()->_max.y && ray.direction.y >= 0 ) return 0;
			if ( nextY < pCurrLODs->GetBounds()->_min.y && ray.direction.y <= 0 ) return 0;

			result = pCurrLODs->GetChunk( nextX, nextZ, &pChunk, lodLevel );
			count++;
		}
		
		return 0;
	}

	int GetHeightLOD( float x, float z, uint32_t lod, float* height, int includeFlatAreas=1 )
	{
		if ( !height ) return 0;

		GGTerrainLODSet* pCurrLODs = GetCurrentLODs();
		if (!pCurrLODs->IsValid() || pCurrLODs->IsGenerating())
			return 0;

		GGTerrainChunk* pChunk = 0;
		uint32_t maxLod = pCurrLODs->GetNumLevels() - 1;
		if ( lod > maxLod ) lod = maxLod;
		int index = pCurrLODs->pLevels[ lod ].GetChunk( x, z, &pChunk );
		if ( index >= 0 )
		{
			int result = pChunk->GetHeight( x, z, height, includeFlatAreas );
			if ( result ) return 1;
		}
		
		return 0;
	}

	int GetHeight( float x, float z, float* height, int includeFlatAreas=1 )
	{
		if ( !height ) return 0;

		GGTerrainLODSet* pCurrLODs = GetCurrentLODs();
		if (!pCurrLODs->IsValid() || pCurrLODs->IsGenerating())
			return 0;

		GGTerrainChunk* pChunk = 0;
		for( uint32_t i = 0; i < pCurrLODs->GetNumLevels(); i++ )
		{
			int index = pCurrLODs->pLevels[ i ].GetChunk( x, z, &pChunk );
			if ( index >= 0 )
			{
				int result = pChunk->GetHeight( x, z, height, includeFlatAreas );
				if ( result ) return 1;
			}
		}
		return 0;
	}

	int GetNormal( float x, float z, float* pNx, float* pNy, float* pNz )
	{
		GGTerrainLODSet* pCurrLODs = GetCurrentLODs();
		if ( !pCurrLODs->IsValid() || pCurrLODs->IsGenerating() ) return 0;

		GGTerrainChunk* pChunk = 0;
		for( uint32_t i = 0; i < pCurrLODs->GetNumLevels(); i++ )
		{
			int index = pCurrLODs->pLevels[ i ].GetChunk( x, z, &pChunk );
			if ( index >= 0 )
			{
				int result = pChunk->GetNormal( x, z, pNx, pNy, pNz );
				if ( result ) return 1;
			}
		}
		return 0;
	}

	void UpdateChunks( float playerX, float playerZ )
	{
		if ( numLODLevels == 0 ) return;

		GGTerrainLODSet* pCurrLODs = &pLODSet[ currLODArray ];
		GGTerrainLODSet* pNewLODs = &pLODSet[ 1-currLODArray ];

		// if shadow lod set is generating then check if it should be made active
		if ( pNewLODs->IsGenerating() )
		{
			if ( !pNewLODs->IsReady() ) return;
			
			// clean up old LOD chunks
			for ( uint32_t i = 0; i < numLODLevels; i++ )
			{
				for( uint32_t j = 0; j < 64; j++ )
				{
					GGTerrainChunk* pChunk = pCurrLODs->pLevels[ i ].chunkGrid[ j ];
					if ( pChunk && !pChunk->IsCopied() ) 
					{
						pChunk->Reset();
						pChunksUnused.PushItem( pChunk );
					}
					pCurrLODs->pLevels[ i ].chunkGrid[ j ] = 0;
				}
			}

			pNewLODs->UpdateBounds();
			pNewLODs->SetActive();
			if ( pCurrLODs->IsValid() ) 
			{
				iFlags |= GGTERRAIN_FLAG_SHIFT_PAGE_TABLE;
				UpdateLODTextures( pNewLODs, 0 );
			}
			else 
			{
				iFlags |= GGTERRAIN_FLAG_REGENERATE_PAGES;
				UpdateLODTextures( pNewLODs, 1 );
				WickedCall_UpdateProbes();
				ggterrain_extra_params.bTerrainChanged = true;
				ggterrain_extra_params.iUpdateTrees = 20;
				ggterrain_extra_params.iUpdateGrass = 20;
			}
			
			// swap LOD arrays
			currLODArray = 1 - currLODArray;
			return;
		}

		// these may have changed
		pCurrLODs = &pLODSet[ currLODArray ];
		pNewLODs = &pLODSet[ 1-currLODArray ];

		if ( pCurrLODs->IsGenerating() )
		{
			if ( !pCurrLODs->IsReady() ) return;
			else 
			{
				pCurrLODs->UpdateBounds();
				iFlags |= GGTERRAIN_FLAG_REGENERATE_PAGES;
				UpdateLODTextures( pCurrLODs, 1 );
				WickedCall_UpdateProbes();
				ggterrain_extra_params.bTerrainChanged = true;
				ggterrain_extra_params.iUpdateTrees = 20;
				ggterrain_extra_params.iUpdateGrass = 20;
			}
		}

		for( int i = 0; i < (int)pageRefreshList.NumItems(); i++ )
		{
			pageRefreshList[ i ]->AdvanceRegenerateQueue();
		}
				
		// guaranteed to not be generating any chunks
		// check if we need to update

		if ( ggterrain_internal_params.update_flat_areas != 0 )
		{
			ggterrain_internal_params.update_flat_areas = 0;

			// get any heights we couldn't get before
			for( uint32_t i = 0; i < ggterrain_flat_areas_array_size; i++ )
			{
				GGTerrainFlatArea* pArea = &ggterrain_flat_areas[ i ];
				if ( !pArea->IsValid() ) continue;

				if ( isnan( pArea->y ) )
				{
					float height = 0;
					if ( GGTerrain_GetHeight( pArea->x, pArea->z, &height, 0, 0 ) ) 
					{
						pArea->y = height;
					}
				}
			}

			if ( ggterrain_internal_params.update_flat_areas == 0 )
			{
				float origMinX = ggterrain_internal_params.update_flat_areas_minX;
				float origMinZ = ggterrain_internal_params.update_flat_areas_minZ;
				float origMaxX = ggterrain_internal_params.update_flat_areas_maxX;
				float origMaxZ = ggterrain_internal_params.update_flat_areas_maxZ;

				ggterrain_internal_params.update_flat_areas_minX = 1e20f;
				ggterrain_internal_params.update_flat_areas_minZ = 1e20f;
				ggterrain_internal_params.update_flat_areas_maxX = -1e20f;
				ggterrain_internal_params.update_flat_areas_maxZ = -1e20f;

				float fMinX = origMinX / ggterrain_local_render_params2.editable_size;
				fMinX = fMinX * 0.5f + 0.5f;
				fMinX *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
				float fMinZ = origMinZ / ggterrain_local_render_params2.editable_size;
				fMinZ = fMinZ * 0.5f + 0.5f;
				fMinZ *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
				float fMaxX = origMaxX / ggterrain_local_render_params2.editable_size;
				fMaxX = fMaxX * 0.5f + 0.5f;
				fMaxX *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
				float fMaxZ = origMaxZ / ggterrain_local_render_params2.editable_size;
				fMaxZ = fMaxZ * 0.5f + 0.5f;
				fMaxZ *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE;

				int minX = 0;
				int minY = 0;
				int maxX = GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
				int maxY = GGTERRAIN_HEIGHTMAP_EDIT_SIZE;

				if ( fMinX > 0 ) minX = (int) fMinX;
				if ( fMinZ > 0 ) minY = (int) fMinZ;
				if ( fMaxX < (float) GGTERRAIN_HEIGHTMAP_EDIT_SIZE ) maxX = (int) fMaxX;
				if ( fMaxZ < (float) GGTERRAIN_HEIGHTMAP_EDIT_SIZE ) maxY = (int) fMaxZ;

				if ( maxX >= 0 && maxY >= 0 && minX < GGTERRAIN_HEIGHTMAP_EDIT_SIZE && minY < GGTERRAIN_HEIGHTMAP_EDIT_SIZE )
				{
					if ( minX < 0 ) minX = 0;
					if ( minY < 0 ) minY = 0;
					if ( maxX > GGTERRAIN_HEIGHTMAP_EDIT_SIZE ) maxX = GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
					if ( maxY > GGTERRAIN_HEIGHTMAP_EDIT_SIZE ) maxY = GGTERRAIN_HEIGHTMAP_EDIT_SIZE;

					// clear out and refill invalidated area
					for( int y = minY; y < maxY; y++ )
					{
						for( int x = minX; x < maxX; x++ )
						{
							uint32_t index = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-y-1) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + x;
							pHeightMapFlatAreas[ index ] = 0;

							float fX = (float) x;
							fX = fX / GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
							fX = fX * 2 - 1;
							fX = fX * ggterrain_local_render_params2.editable_size;

							float fZ = (float) y;
							fZ = fZ / GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
							fZ = fZ * 2 - 1;
							fZ = fZ * ggterrain_local_render_params2.editable_size;

							for( uint32_t i = 0; i < ggterrain_flat_areas_array_size; i++ )
							{
								GGTerrainFlatArea* pArea = &ggterrain_flat_areas[ i ];
								if ( !pArea->IsValid() ) continue;
								if ( isnan( pArea->y ) ) continue;

								if ( pArea->GetType() == GGTERRAIN_FLAT_AREA_TYPE_CIRCLE )
								{
									float diffX = pArea->x - fX;
									float diffZ = pArea->z - fZ;
									float dist = diffX * diffX + diffZ * diffZ;
									float radius = pArea->sizeX / 2.0f;
									radius *= radius;

									if ( dist < radius )
									{
										pHeightMapFlatAreas[ index ] = i;
									}
								}
								else if ( pArea->GetType() == GGTERRAIN_FLAT_AREA_TYPE_RECT )
								{
									float ca = cos( pArea->angle * PI / 180.0f );
									float sa = sin( pArea->angle * PI / 180.0f );

									float fpX = fX - pArea->x;
									float fpZ = fZ - pArea->z;

									float faX = fpX * ca - fpZ * sa;
									float faZ = fpX * sa + fpZ * ca;

									float halfX = pArea->sizeX / 2.0f;
									float halfZ = pArea->sizeZ / 2.0f;
									if ( faX < -halfX ) continue;
									if ( faX >  halfX ) continue;
									if ( faZ < -halfZ ) continue;
									if ( faZ >  halfZ ) continue;

									pHeightMapFlatAreas[ index ] = i;
								}
							}
						}
					}				

					GGTerrain_InvalidateRegion( origMinX, origMinZ, origMaxX, origMaxZ, GGTERRAIN_INVALIDATE_ALL );
				}
			}
		}

		pCurrLODs = &pLODSet[ currLODArray ];
		pNewLODs = &pLODSet[ 1-currLODArray ];
		
		float currX = playerX;
		float currZ = playerZ;

		bool bUpdated = false;
		for ( uint32_t i = 0; i < numLODLevels; i++ )
		{
			pNewLODs->pLevels[ i ].centerX = pCurrLODs->pLevels[ i ].centerX;
			pNewLODs->pLevels[ i ].centerZ = pCurrLODs->pLevels[ i ].centerZ;
			pNewLODs->pLevels[ i ].shiftedX = 0;
			pNewLODs->pLevels[ i ].shiftedZ = 0;

			// calculate new LOD center for this LOD level
			float distX = currX - pNewLODs->pLevels[ i ].centerX;
			float distZ = currZ - pNewLODs->pLevels[ i ].centerZ;

			float chunkSize = pNewLODs->pLevels[ i ].segSize * ggterrain_local_params.segments_per_chunk;
			float distCheck = chunkSize * lodBoundary;

			if ( abs(distX) > distCheck ) 
			{
				// move LOD center by multiple of the chunk size, must always be on a chunk boundary
				bUpdated = true;
				int iX = (int) round( distX / (chunkSize * 2) );
				pNewLODs->pLevels[ i ].centerX += iX * chunkSize * 2;
				pNewLODs->pLevels[ i ].shiftedX = iX * 2;
			}

			if ( abs(distZ) > distCheck )
			{
				// move LOD center by multiple of the chunk size, must always be on a chunk boundary
				bUpdated = true;
				int iZ = (int) round( distZ / (chunkSize * 2) );
				pNewLODs->pLevels[ i ].centerZ += iZ * chunkSize * 2;
				pNewLODs->pLevels[ i ].shiftedZ = iZ * 2;
			}

			currX = pNewLODs->pLevels[ i ].centerX;
			currZ = pNewLODs->pLevels[ i ].centerZ;
		}

		if ( !bUpdated && (iFlags & GGTERRAIN_FLAG_REGENERATE_CHUNKS) == 0 && !pCurrLODs->ShouldRegenerate() ) return;

		// new chunks need to be generated
		
		iFlags &= ~GGTERRAIN_FLAG_REGENERATE_CHUNKS;
		pNewLODs->SetGenerating();
		pCurrLODs->SetToRegenerate( 0 );

		currX = playerX;
		currZ = playerZ;
		
		// populate new LOD chunks from old LOD chunks, generating new ones where required
		for ( uint32_t i = 0; i < numLODLevels; i++ )
		{
			if ( pCurrLODs->pLevels[ i ].ShouldRegenerate() ) pNewLODs->pLevels[ i ].SetRegenerated( 1 );
			pCurrLODs->pLevels[ i ].SetToRegenerate( 0 );
			
			float segSize = pNewLODs->pLevels[ i ].segSize;
			float chunkSize = segSize * ggterrain_local_params.segments_per_chunk;
			float halfChunkSize = chunkSize / 2.0f;

			// offset from old LOD position
			int iShiftX = pNewLODs->pLevels[ i ].shiftedX;
			int iShiftZ = pNewLODs->pLevels[ i ].shiftedZ;
			
			// offset from lower LOD level
			float distX = currX - pNewLODs->pLevels[ i ].centerX;
			float distZ = currZ - pNewLODs->pLevels[ i ].centerZ;
			currX = pNewLODs->pLevels[ i ].centerX;
			currZ = pNewLODs->pLevels[ i ].centerZ;

			// check necessary chunks exist
			int lowX, lowZ, highX, highZ;
				
			if ( distX > halfChunkSize ) 
			{
				lowX = 3; highX = 6;
			}
			else if ( distX < -halfChunkSize ) 
			{
				lowX = 1; highX = 4;
			}
			else 
			{
				lowX = 2; highX = 5;
			}

			if ( distZ > halfChunkSize ) 
			{
				lowZ = 3; highZ = 6; 
			}
			else if ( distZ < -halfChunkSize )
			{
				lowZ = 1; highZ = 4;
			}
			else 
			{
				lowZ = 2; highZ = 5;
			}

			// check new LOD chunks
			for( int z = 0; z < 8; z++ )
			{
				for( int x = 0; x < 8; x++ )
				{
					int visible = 1;
					if ( i > 0 )
					{
						// tiles covered by the previous lod level
						if ( x >= lowX && x <= highX && z >= lowZ && z <= highZ ) visible = 0;
					}

					uint32_t index = (z * 8) + x;

					int oldX = x + iShiftX;
					int oldZ = z + iShiftZ;

					uint8_t genFlags = 0;
					if ( x == 0 ) genFlags |= GGTERRAIN_CHUNK_STITCH_XNEG;
					if ( x == 7 ) genFlags |= GGTERRAIN_CHUNK_STITCH_XPOS;
					if ( z == 0 ) genFlags |= GGTERRAIN_CHUNK_STITCH_ZNEG;
					if ( z == 7 ) genFlags |= GGTERRAIN_CHUNK_STITCH_ZPOS;
					genFlags |= (i << 4);

					if ( oldX >= 0 && oldX < 8 && oldZ >= 0 && oldZ < 8 ) 
					{
						// if new chunk overlaps with one from the old LOD, and has the correct genFlags, then copy it across
						uint32_t oldIndex = (oldZ * 8) + oldX;
						GGTerrainChunk* pOldChunk = pCurrLODs->pLevels[ i ].chunkGrid[ oldIndex ];
						if ( pOldChunk && pOldChunk->genFlags == genFlags && !pOldChunk->ShouldRegenerate() )
						{
							pOldChunk->SetCopied( 1 );
							pNewLODs->pLevels[ i ].chunkGrid[ index ] = pOldChunk;
							pOldChunk->SetFutureVisible( visible );
						}
						else
						{
							// chunk needs to be regenerated because it doesn't exist or has incorrect stitching. 
							// If we stored the vertices then we can just fix the stitching, but that would require more memory
							GGTerrainChunk* pNewChunk = pChunksUnused.PopItem();
							if ( !pNewChunk ) pNewChunk = new GGTerrainChunk();
							pNewLODs->pLevels[ i ].chunkGrid[ index ] = pNewChunk;
							pNewChunk->SetFutureVisible( visible );
							float chunkX = pNewLODs->pLevels[ i ].centerX + ((x - 4) * chunkSize);
							float chunkZ = pNewLODs->pLevels[ i ].centerZ + ((z - 4) * chunkSize);
							pNewChunk->SetGenerateData( chunkX, chunkZ, pNewLODs->pLevels[ i ].segSize, genFlags );
							MemoryBarrier();
							ChunkGenerator::AddChunk( pNewChunk );
						}
					}
					else
					{					
						// this chunk needs a new one generating
						GGTerrainChunk* pNewChunk = pChunksUnused.PopItem();
						if ( !pNewChunk ) pNewChunk = new GGTerrainChunk();
						pNewLODs->pLevels[ i ].chunkGrid[ index ] = pNewChunk;
						pNewChunk->SetFutureVisible( visible );
						float chunkX = pNewLODs->pLevels[ i ].centerX + ((x - 4) * chunkSize);
						float chunkZ = pNewLODs->pLevels[ i ].centerZ + ((z - 4) * chunkSize);
						pNewChunk->SetGenerateData( chunkX, chunkZ, pNewLODs->pLevels[ i ].segSize, genFlags );
						MemoryBarrier();
						ChunkGenerator::AddChunk( pNewChunk );
					}
				}
			}
		}

		// if we currently don't have a valid set of LOD chunks then show the currently generating ones in real time
		if ( !pCurrLODs->IsValid() ) 
		{
			pNewLODs->SetActive();
			currLODArray = 1 - currLODArray;
		}
	}
	
	GGTerrain() 
	{

	}

	~GGTerrain() 
	{
		ResetChunks(); 
	}
};

GGTerrain ggterrain;

int ggterrain_initialised = 0;
int ggterrain_draw_enabled = 1;
int ggprobe_initialised = 0;

int ggterrain_render_wireframe = 0;
int ggterrain_render_debug = 0;

void GGTerrain_CreateFractalTexture( Texture* tex, uint32_t size ) 
{ 
	GraphicsDevice* device = wiRenderer::GetDevice();

	/*
	//uint32_t palette[ 8 ] = { 0xFF28B380, 0xFF0C491D, 0xFF6CF1AB, 0xFF2B6035, 0xFF006440, 0xFF147232, 0xFF1FDA48, 0xFF61B487 };
	uint32_t palette[ 8 ] = { 0xFF10201F, 0xFF334347, 0xFF44595F, 0xFF71969A, 0xFF89B5B9, 0xFF225552, 0xFF37716C, 0xFF588787 };
	
	uint8_t* imageData = new uint8_t[ size * size * 4 ];
	uint32_t* imageDataInt = (uint32_t*) imageData;
	for( uint32_t y = 0; y < size; y++ )
	{
		for( uint32_t x = 0; x < size; x++ )
		{
			float fX = x / 10000.0f;
			float fY = y / 10000.0f;

			float red = noise.fractal( 12, fX, fY );
			float green = noise.fractal( 12, fX*1.31f+567, fY*1.5f+833 );
			float blue = noise.fractal( 12, fX*1.77f+1231, fY*1.63f+967 );

			uint8_t iRed = (red >= 0) ? 1 : 0;
			uint8_t iGreen = (green >= 0) ? 1 : 0;
			uint8_t iBlue = (blue >= 0) ? 1 : 0;
			uint8_t colorIndex = (iRed << 2) | (iGreen << 1) | iBlue;
			
			//uint8_t iRed = red * 127 + 127;
			//uint8_t iGreen = green * 127 + 127;
			//uint8_t iBlue = blue * 127 + 127;
			
			//uint8_t iRed = (red >= 0) ? 255 : 0;
			//uint8_t iGreen = (green >= 0) ? 255 : 0;
			//uint8_t iBlue = (blue >= 0) ? 255 : 0;
			//uint32_t color = 0xFF000000 | (iBlue << 16) | (iGreen << 8) | iRed;
						
			uint32_t index = (y * size) + x;
			//imageDataInt[ index ] = color;
			imageDataInt[ index ] = palette[ colorIndex ];
		}
	}
	*/

	uint8_t* imageData = new uint8_t[size * size];
#ifdef ONLYLOADWHENUSED
	FILE* loadfile = GG_fopen("files/treebank/FractalGenerator.dat", "rb");
	if (loadfile)
	{
		size_t readsize = fread(imageData, 1, size * size, loadfile);
		fclose(loadfile);
	}
	else
	{
#endif
		//PE: This is slow , so cache it , it generate the same data anyway.
		FractalGenerator::SetWork(imageData, size, size);
		FractalGenerator::StartThreads();
		FractalGenerator::WaitForAll();
#ifdef ONLYLOADWHENUSED
		FILE* savefile = GG_fopen("files/treebank/FractalGenerator.dat", "wb+");
		if (savefile) {
			size_t length = fwrite(imageData, 1, (size * size), savefile);
			fclose(savefile);
		}
	}
#endif

	TextureDesc texDesc = {};
	texDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_RENDER_TARGET; // BIND_RENDER_TARGET must be set for generating mipmaps
	texDesc.clear.color[0] = 1.0f;
	texDesc.clear.color[1] = 0.0f;
	texDesc.clear.color[2] = 0.0f;
	texDesc.clear.color[3] = 0.0f;
	texDesc.SampleCount = 1;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = FORMAT_R8_UNORM;
	texDesc.Usage = USAGE_DEFAULT;
	texDesc.Width = size;
	texDesc.Height = size;
	texDesc.MiscFlags = RESOURCE_MISC_GEN_MIPMAPS;

	// to generate mipmaps the texture must be created with MipLevels set to 0 (all) or the desired number of levels
	// so CreateTexture data can't set a single mip level, use UpdateTexture afterwards instead
	device->CreateTexture( &texDesc, nullptr, tex );
	device->SetName( tex, "imageTex" );

	// upload mip level 0 data here
	uint32_t rowStride = size;

	device->UpdateTexture( tex, 0, 0, 0, imageData, rowStride, -1 );
	delete [] imageData;

	// generate all mip levels from mip 0
	//device->GenerateMipmaps( tex, -1 );
}

void GGTerrain_LoadTexturePNG( const char* filename, Texture* tex ) 
{ 
	GraphicsDevice* device = wiRenderer::GetDevice();
	
	int width, height, channels;
	char filePath[ MAX_PATH ];
	strcpy_s( filePath, MAX_PATH, filename );
	GG_GetRealPath( filePath, 0 );
	uint8_t* imageData = stbi_load( filePath, &width, &height, &channels, 4 );
		
	TextureDesc texDesc = {};
	texDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_RENDER_TARGET; // BIND_RENDER_TARGET must be set for generating mipmaps
	texDesc.clear.color[0] = 1.0f;
	texDesc.clear.color[1] = 0.0f;
	texDesc.clear.color[2] = 0.0f;
	texDesc.clear.color[3] = 0.0f;
	texDesc.SampleCount = 1;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 1;
	switch( channels )
	{
		case 1: texDesc.Format = FORMAT_R8_UNORM; break;
		case 2: texDesc.Format = FORMAT_R8G8_UNORM; break;
		case 3: // fall through
		case 4: texDesc.Format = FORMAT_R8G8B8A8_UNORM; break;
		default: return; // error
	}
	texDesc.Usage = USAGE_DEFAULT;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MiscFlags = RESOURCE_MISC_GEN_MIPMAPS;

	// to generate mipmaps the texture must be created with MipLevels set to 0 (all) or the desired number of levels
	// so CreateTexture data can't set a single mip level, use UpdateTexture afterwards instead
	device->CreateTexture( &texDesc, nullptr, tex );
	device->SetName( tex, "imageTex" );

	// upload mip level 0 data here
	uint32_t rowStride = width * ((channels == 3) ? 4 : channels);
	device->UpdateTexture( tex, 0, 0, 0, imageData, rowStride, -1 );  //for when new Wicked Terrain Shader is created
	stbi_image_free( imageData );

	// generate all mip levels from mip 0
	device->GenerateMipmaps( tex, -1 );
}

wiGraphics::FORMAT ConvertDDSFormat( tinyddsloader::DDSFile::DXGIFormat format )
{
	switch (format)
	{
		case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_Float: return FORMAT_R32G32B32A32_FLOAT; 
		case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_UInt: return FORMAT_R32G32B32A32_UINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_SInt: return FORMAT_R32G32B32A32_SINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_Float: return FORMAT_R32G32B32_FLOAT; 
		case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_UInt: return FORMAT_R32G32B32_UINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_SInt: return FORMAT_R32G32B32_SINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_Float: return FORMAT_R16G16B16A16_FLOAT; 
		case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_UNorm: return FORMAT_R16G16B16A16_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_UInt: return FORMAT_R16G16B16A16_UINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_SNorm: return FORMAT_R16G16B16A16_SNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_SInt: return FORMAT_R16G16B16A16_SINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R32G32_Float: return FORMAT_R32G32_FLOAT; 
		case tinyddsloader::DDSFile::DXGIFormat::R32G32_UInt: return FORMAT_R32G32_UINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R32G32_SInt: return FORMAT_R32G32_SINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_UNorm: return FORMAT_R10G10B10A2_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_UInt: return FORMAT_R10G10B10A2_UINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R11G11B10_Float: return FORMAT_R11G11B10_FLOAT; 
#ifdef GGREDUCED
		case tinyddsloader::DDSFile::DXGIFormat::B8G8R8X8_UNorm: return FORMAT_B8G8R8A8_UNORM; 
#endif
		case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm: return FORMAT_B8G8R8A8_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm_SRGB: return FORMAT_B8G8R8A8_UNORM_SRGB; 
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm: return FORMAT_R8G8B8A8_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm_SRGB: return FORMAT_R8G8B8A8_UNORM_SRGB; 
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UInt: return FORMAT_R8G8B8A8_UINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SNorm: return FORMAT_R8G8B8A8_SNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SInt: return FORMAT_R8G8B8A8_SINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R16G16_Float: return FORMAT_R16G16_FLOAT; 
		case tinyddsloader::DDSFile::DXGIFormat::R16G16_UNorm: return FORMAT_R16G16_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::R16G16_UInt: return FORMAT_R16G16_UINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R16G16_SNorm: return FORMAT_R16G16_SNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::R16G16_SInt: return FORMAT_R16G16_SINT; 
		case tinyddsloader::DDSFile::DXGIFormat::D32_Float: return FORMAT_D32_FLOAT; 
		case tinyddsloader::DDSFile::DXGIFormat::R32_Float: return FORMAT_R32_FLOAT; 
		case tinyddsloader::DDSFile::DXGIFormat::R32_UInt: return FORMAT_R32_UINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R32_SInt: return FORMAT_R32_SINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R8G8_UNorm: return FORMAT_R8G8_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::R8G8_UInt: return FORMAT_R8G8_UINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R8G8_SNorm: return FORMAT_R8G8_SNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::R8G8_SInt: return FORMAT_R8G8_SINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R16_Float: return FORMAT_R16_FLOAT; 
		case tinyddsloader::DDSFile::DXGIFormat::D16_UNorm: return FORMAT_D16_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::R16_UNorm: return FORMAT_R16_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::R16_UInt: return FORMAT_R16_UINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R16_SNorm: return FORMAT_R16_SNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::R16_SInt: return FORMAT_R16_SINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R8_UNorm: return FORMAT_R8_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::R8_UInt: return FORMAT_R8_UINT; 
		case tinyddsloader::DDSFile::DXGIFormat::R8_SNorm: return FORMAT_R8_SNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::R8_SInt: return FORMAT_R8_SINT; 
		case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm: return FORMAT_BC1_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm_SRGB: return FORMAT_BC1_UNORM_SRGB; 
		case tinyddsloader::DDSFile::DXGIFormat::BC2_UNorm: return FORMAT_BC2_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::BC2_UNorm_SRGB: return FORMAT_BC2_UNORM_SRGB; 
		case tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm: return FORMAT_BC3_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm_SRGB: return FORMAT_BC3_UNORM_SRGB; 
		case tinyddsloader::DDSFile::DXGIFormat::BC4_UNorm: return FORMAT_BC4_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::BC4_SNorm: return FORMAT_BC4_SNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::BC5_UNorm: return FORMAT_BC5_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::BC5_SNorm: return FORMAT_BC5_SNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm: return FORMAT_BC7_UNORM; 
		case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm_SRGB: return FORMAT_BC7_UNORM_SRGB; 
		default:
			assert(0); // incoming format is not supported 
			return FORMAT_UNKNOWN;
	}
}

void GGTerrain_LoadTextureDDS( const char* filename, Texture* tex ) 
{ 
	GraphicsDevice* device = wiRenderer::GetDevice();
	
	char filePath[ MAX_PATH ];
	strcpy_s( filePath, MAX_PATH, filename );
	GG_GetRealPath( filePath, 0 );
	
	tinyddsloader::DDSFile dds;
	auto result = dds.Load( filePath );

	if (result != tinyddsloader::Result::Success) return;
	
	TextureDesc desc;
	desc.ArraySize = 1;
	desc.BindFlags = BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Width = dds.GetWidth();
	desc.Height = dds.GetHeight();
	desc.Depth = dds.GetDepth();
	desc.MipLevels = dds.GetMipCount();
	desc.ArraySize = dds.GetArraySize();
	desc.MiscFlags = 0;
	desc.Usage = USAGE_IMMUTABLE;
	desc.layout = IMAGE_LAYOUT_SHADER_RESOURCE;
	desc.Format = ConvertDDSFormat( dds.GetFormat() );

	std::vector<SubresourceData> InitData;
	for (uint32_t arrayIndex = 0; arrayIndex < desc.ArraySize; ++arrayIndex)
	{
		for (uint32_t mip = 0; mip < desc.MipLevels; ++mip)
		{
			auto imageData = dds.GetImageData(mip, arrayIndex);
			SubresourceData subresourceData;
			subresourceData.pSysMem = imageData->m_mem;
			subresourceData.SysMemPitch = imageData->m_memPitch;
			subresourceData.SysMemSlicePitch = imageData->m_memSlicePitch;
			InitData.push_back(subresourceData);
		}
	}

	auto dim = dds.GetTextureDimension();
	switch (dim)
	{
		case tinyddsloader::DDSFile::TextureDimension::Texture1D: desc.type = TextureDesc::TEXTURE_1D; break;
		case tinyddsloader::DDSFile::TextureDimension::Texture2D: desc.type = TextureDesc::TEXTURE_2D; break;
		case tinyddsloader::DDSFile::TextureDimension::Texture3D: desc.type = TextureDesc::TEXTURE_3D;break;
		default: assert(0); break;
	}

	device->CreateTexture( &desc, InitData.data(), tex );
	device->SetName( tex, "imageTex" );
}

bool GGTerrain_LoadTextureDDSIntoSlice(const char* filename, Texture* tex, uint32_t arraySlice, DDSRequirements* requirements, wiGraphics::CommandList cmd, bool bFillAllSlots = false)
{ 
	GraphicsDevice* device = wiRenderer::GetDevice();
	char filePath[MAX_PATH];
	strcpy_s(filePath, MAX_PATH, filename);
	GG_GetRealPath(filePath, 0);
	tinyddsloader::DDSFile dds;
	auto result = dds.Load(filePath);
	if (result != tinyddsloader::Result::Success) return false;

	#ifdef CUSTOMTEXTURES
	// If the texture we want to load into the slice does not match the tex array format, convert it
	if (dds.GetWidth() != requirements->width || dds.GetHeight() != requirements->height || (int)dds.GetFormat() != requirements->format || dds.GetMipCount() <= 1)
	{
		// Only convert textures if already in writable folder, don't want to change any max install or other files
		if(strstr(filePath, GG_GetWritePath()))
		{
			//LB: no such GetDeviceForIMGUI access in DX12
			//LB: Moreover, the DirectXTex is too old DX11 (we need the latest with DX12 support, which also needs VS2022 - so defer until Wicked Merge complete)
			//ConvertDDSCompressedFormat((ID3D11Device*)device->GetDeviceForIMGUI(), filePath, requirements->format, requirements->width, requirements->height, filePath);
			//result = dds.Load(filePath);
			//if (g_bOneTimeMessage == false)
			//{
				bTriggerMessage = true;
				strcpy(cTriggerMessage, "Conversion to the required BC7 (color), BC5 (normal) and BC1 (surface) will be added soon!");
				//MessageBoxA(NULL, "", "PNG to DDS Texture Converter", MB_OK);
			//	g_bOneTimeMessage = true;
			//}
			return false;
		}
		else
		{
			// If we can't convert the file, it cannot be loaded into the tex array
			return false;
		}
	}

	#endif

	if (bFillAllSlots)
	{
		for (uint32_t i = 0; i < GGTERRAIN_MAX_SOURCE_TEXTURES; i++)
		{
			uint32_t maxMip = dds.GetMipCount();
			if (maxMip > tex->desc.MipLevels) maxMip = tex->desc.MipLevels;
			std::vector<SubresourceData> InitData;
			for (uint32_t mip = 0; mip < maxMip; ++mip)
			{
				auto imageData = dds.GetImageData(mip, 0);
				device->UpdateTexture(tex, mip, i, 0, imageData->m_mem, imageData->m_memPitch, cmd);
			}
			break;
		}
	}
	else
	{
		uint32_t maxMip = dds.GetMipCount();
		if (maxMip > tex->desc.MipLevels) maxMip = tex->desc.MipLevels;
		std::vector<SubresourceData> InitData;
		for (uint32_t mip = 0; mip < maxMip; ++mip)
		{
			auto imageData = dds.GetImageData(mip, 0);
			device->UpdateTexture(tex, mip, arraySlice, 0, imageData->m_mem, imageData->m_memPitch, cmd);
		}
	}
	return true;
}

// compress normals with BC5 (2 channel greyscale)
void GGTerrain_CompressNormals( LPSTR srcFilename, LPSTR dstFilename )
{
	HRESULT hr;

	int width, height, channels;
	uint8_t* imageData = stbi_load( srcFilename, &width, &height, &channels, 4 );
	if ( !imageData )
	{
		char newSrcFilename[ MAX_PATH ];
		strcpy_s( newSrcFilename, MAX_PATH, srcFilename );
		char* szExt = strrchr( newSrcFilename, '.' );
		if ( szExt ) strcpy_s( szExt, 5, ".jpg" );
		else strcat_s( newSrcFilename, MAX_PATH, ".jpg" );
		imageData = stbi_load( newSrcFilename, &width, &height, &channels, 4 );
		if ( !imageData )
		{
			assert(0);
			return;
		}
	}

	uint32_t finalWidth = width;
	uint32_t finalHeight = height;
	uint32_t mipLevels = 1;
	uint32_t testWidth = 1;
	while( testWidth < finalWidth )
	{
		testWidth <<= 1;
		mipLevels++;
	}

	// create final image
	DirectX::ScratchImage mipChain;
	hr = mipChain.Initialize2D( DXGI_FORMAT_R8G8_UNORM, finalWidth, finalHeight, 1, mipLevels, DirectX::DDS_FLAGS_NONE );
		
	struct vec3
	{
		float x, y ,z;
	};

	// convert image data into vectors
	vec3** pNormalMaps = new vec3*[ mipLevels ];
	pNormalMaps[ 0 ] = new vec3[ finalWidth * finalHeight ];

	uint8_t* pDstPtr = mipChain.GetImages()[0].pixels;
	for( uint32_t y = 0; y < finalHeight; y++ )
	{
		for( uint32_t x = 0; x < finalWidth; x++ )
		{
			uint32_t index = y * finalWidth + x;

			// only copy red and green channels into the texture
			*(pDstPtr + 0) = imageData[ 4*index + 0 ];
			*(pDstPtr + 1) = imageData[ 4*index + 1 ];
			pDstPtr += 2;

			pNormalMaps[ 0 ][ index ].x = (imageData[ 4*index + 0 ] / 255.0f) * 2.0f - 1.0f;
			pNormalMaps[ 0 ][ index ].y = (imageData[ 4*index + 1 ] / 255.0f) * 2.0f - 1.0f;
			pNormalMaps[ 0 ][ index ].z = (imageData[ 4*index + 2 ] / 255.0f) * 2.0f - 1.0f;
		}
	}

	delete [] imageData;

	// create mip levels
	for( uint32_t m = 1; m < mipLevels; m++ )
	{
		uint32_t mipWidth = finalWidth >> m;
		uint32_t mipHeight = finalHeight >> m;
		pNormalMaps[ m ] = new vec3[ mipWidth * mipHeight ];

		uint32_t prevMipWidth = finalWidth >> (m-1);
		uint32_t prevMipHeight = finalHeight >> (m-1);

		uint8_t* pDstPtr = mipChain.GetImages()[m].pixels;

		for( uint32_t y = 0; y < mipHeight; y++ )
		{
			for( uint32_t x = 0; x < mipWidth; x++ )
			{
				uint32_t index = y * mipWidth + x;
				uint32_t index2 = 2 * (y * prevMipWidth + x);

				float nx,ny,nz;

				nx = pNormalMaps[ m-1 ][ index2 ].x;
				ny = pNormalMaps[ m-1 ][ index2 ].y;
				nz = pNormalMaps[ m-1 ][ index2 ].z;
				
				nx += pNormalMaps[ m-1 ][ index2+1 ].x;
				ny += pNormalMaps[ m-1 ][ index2+1 ].y;
				nz += pNormalMaps[ m-1 ][ index2+1 ].z;
				
				nx += pNormalMaps[ m-1 ][ index2+prevMipWidth ].x;
				ny += pNormalMaps[ m-1 ][ index2+prevMipWidth ].y;
				nz += pNormalMaps[ m-1 ][ index2+prevMipWidth ].z;
				
				nx += pNormalMaps[ m-1 ][ index2+prevMipWidth+1 ].x;
				ny += pNormalMaps[ m-1 ][ index2+prevMipWidth+1 ].y;
				nz += pNormalMaps[ m-1 ][ index2+prevMipWidth+1 ].z;

				float length = nx*nx + ny*ny + nz*nz;
				if ( length == 0 )
				{
					nx = 0;
					ny = 0;
					nz = 0;
				}
				else
				{
					length = sqrt( length );
					nx = nx / length;
					ny = ny / length;
					nz = nz / length;
				}

				pNormalMaps[ m ][ index ].x = nx;
				pNormalMaps[ m ][ index ].y = ny;
				pNormalMaps[ m ][ index ].z = nz;

				// only copy red and green channels into the texture
				*(pDstPtr + 0) = (int) ((nx + 1.0f) / 2.0f * 255.0f);
				*(pDstPtr + 1) = (int) ((ny + 1.0f) / 2.0f * 255.0f);
				pDstPtr += 2;
			}
		}
	}

	for( uint32_t m = 0; m < mipLevels; m++ )
	{
		delete [] pNormalMaps[ m ];
	}
	delete [] pNormalMaps;

	// compress resulting image using BC5 (2 channel greyscale)
	DirectX::ScratchImage compressedSurface;
	hr = DirectX::Compress( mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DXGI_FORMAT_BC5_UNORM, TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, compressedSurface );
	if ( hr != S_OK )
	{
		assert(0);
		return;
	}

	mipChain.Release();

	// save the image as a DDS
	wchar_t wDstFilename[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, dstFilename, -1, wDstFilename, sizeof(wDstFilename) );
    hr = DirectX::SaveToDDSFile( compressedSurface.GetImages(), compressedSurface.GetImageCount(), compressedSurface.GetMetadata(), DirectX::DDS_FLAGS_NONE, wDstFilename );
	if ( hr != S_OK )
	{
		assert(0);
		return;
	}

	compressedSurface.Release();
}

// compress roughness and metalness into single BC5 texture (2 channel greyscale)
void GGTerrain_CompressRoughnessMetalness( LPSTR roughnessFilename, LPSTR metalnessFilename, LPSTR dstFilename )
{
	HRESULT hr;

	int width, height, channels;
	uint8_t* roughnessImageData = stbi_load( roughnessFilename, &width, &height, &channels, 4 );
	if ( !roughnessImageData )
	{
		char newSrcFilename[ MAX_PATH ];
		strcpy_s( newSrcFilename, MAX_PATH, roughnessFilename );
		char* szExt = strrchr( newSrcFilename, '.' );
		if ( szExt ) strcpy_s( szExt, 5, ".jpg" );
		else strcat_s( newSrcFilename, MAX_PATH, ".jpg" );
		roughnessImageData = stbi_load( newSrcFilename, &width, &height, &channels, 4 );
		if ( !roughnessImageData )
		{
			assert(0);
			return;
		}
	}

	int width2, height2;
	uint8_t* metalnessImageData = stbi_load( metalnessFilename, &width2, &height2, &channels, 4 );
	if ( !metalnessImageData )
	{
		char newSrcFilename[ MAX_PATH ];
		strcpy_s( newSrcFilename, MAX_PATH, metalnessFilename );
		char* szExt = strrchr( newSrcFilename, '.' );
		if ( szExt ) strcpy_s( szExt, 5, ".jpg" );
		else strcat_s( newSrcFilename, MAX_PATH, ".jpg" );
		metalnessImageData = stbi_load( newSrcFilename, &width2, &height2, &channels, 4 );
		if ( !metalnessImageData )
		{
			assert(0);
			return;
		}
	}

	if ( width != width2 || height != height2 )
	{
		assert(0);
		return;
	}

	// create final image
	DirectX::ScratchImage srcImage;
	hr = srcImage.Initialize2D( DXGI_FORMAT_R8G8_UNORM, width, height, 1, 1, DirectX::DDS_FLAGS_NONE );

	// copy source data into texture channels
	uint8_t* pDstPtr = srcImage.GetPixels();
	for( int y = 0; y < height; y++ )
	{
		for( int x = 0; x < width; x++ )
		{
			int index = y * width + x;

			// copy into red and green channels
			*(pDstPtr + 0) = roughnessImageData[ 4*index + 0 ];
			*(pDstPtr + 1) = metalnessImageData[ 4*index + 0 ];
			pDstPtr += 2;
		}
	}

	delete [] roughnessImageData;
	delete [] metalnessImageData;
	
	// create mipmaps
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps( srcImage.GetImages()[0], TEX_FILTER_WRAP | TEX_FILTER_SEPARATE_ALPHA | TEX_FILTER_FORCE_NON_WIC, 0, mipChain );
	if ( hr != S_OK )
	{
		assert(0);
		return;
	}

	srcImage.Release();

	// compress resulting image
	DirectX::ScratchImage compressedSurface;
	hr = DirectX::Compress( mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DXGI_FORMAT_BC5_UNORM, TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, compressedSurface );
	if ( hr != S_OK )
	{
		assert(0);
		return;
	}

	mipChain.Release();
			
	// save the image as a DDS
	wchar_t wDstFilename[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, dstFilename, -1, wDstFilename, sizeof(wDstFilename) );
    hr = DirectX::SaveToDDSFile( compressedSurface.GetImages(), compressedSurface.GetImageCount(), compressedSurface.GetMetadata(), DirectX::DDS_FLAGS_NONE, wDstFilename );
	if ( hr != S_OK )
	{
		assert(0);
		return;
	}

	compressedSurface.Release();
}

// compress occlusion, roughness, and metalness into single BC1 texture (RGB)
void GGTerrain_CompressSurface( LPSTR aoFilename, LPSTR roughnessFilename, LPSTR metalnessFilename, LPSTR dstFilename )
{
	HRESULT hr;

	int width, height, channels;
	uint8_t* roughnessImageData = stbi_load( roughnessFilename, &width, &height, &channels, 4 );
	if ( !roughnessImageData )
	{
		char newSrcFilename[ MAX_PATH ];
		strcpy_s( newSrcFilename, MAX_PATH, roughnessFilename );
		char* szExt = strrchr( newSrcFilename, '.' );
		if ( szExt ) strcpy_s( szExt, 5, ".jpg" );
		else strcat_s( newSrcFilename, MAX_PATH, ".jpg" );
		roughnessImageData = stbi_load( newSrcFilename, &width, &height, &channels, 4 );
		if ( !roughnessImageData )
		{
			assert(0);
			return;
		}
	}

	int width2, height2;
	uint8_t* aoImageData = stbi_load( aoFilename, &width2, &height2, &channels, 4 );
	if ( !aoImageData )
	{
		char newSrcFilename[ MAX_PATH ];
		strcpy_s( newSrcFilename, MAX_PATH, aoFilename );
		char* szExt = strrchr( newSrcFilename, '.' );
		if ( szExt ) strcpy_s( szExt, 5, ".jpg" );
		else strcat_s( newSrcFilename, MAX_PATH, ".jpg" );
		aoImageData = stbi_load( newSrcFilename, &width2, &height2, &channels, 4 );
		if ( !aoImageData )
		{
			assert(0);
			return;
		}
	}

	if ( width != width2 || height != height2 )
	{
		assert(0);
		return;
	}

	uint8_t* metalnessImageData = stbi_load( metalnessFilename, &width2, &height2, &channels, 4 );
	if ( !metalnessImageData )
	{
		char newSrcFilename[ MAX_PATH ];
		strcpy_s( newSrcFilename, MAX_PATH, metalnessFilename );
		char* szExt = strrchr( newSrcFilename, '.' );
		if ( szExt ) strcpy_s( szExt, 5, ".jpg" );
		else strcat_s( newSrcFilename, MAX_PATH, ".jpg" );
		metalnessImageData = stbi_load( newSrcFilename, &width2, &height2, &channels, 4 );
		if ( !metalnessImageData )
		{
			assert(0);
			return;
		}
	}

	if ( width != width2 || height != height2 )
	{
		assert(0);
		return;
	}

	// create final image
	DirectX::ScratchImage srcImage;
	hr = srcImage.Initialize2D( DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1, DirectX::DDS_FLAGS_NONE );

	// copy source data into texture channels
	uint8_t* pDstPtr = srcImage.GetPixels();
	for( int y = 0; y < height; y++ )
	{
		for( int x = 0; x < width; x++ )
		{
			int index = y * width + x;

			// copy into RGBA channels
			*(pDstPtr + 0) = aoImageData[ 4*index + 0 ];
			*(pDstPtr + 1) = roughnessImageData[ 4*index + 0 ];
			*(pDstPtr + 2) = metalnessImageData[ 4*index + 0 ];
			*(pDstPtr + 3) = 0;
			pDstPtr += 4;
		}
	}

	delete [] aoImageData;
	delete [] roughnessImageData;
	delete [] metalnessImageData;
	
	// create mipmaps
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps( srcImage.GetImages()[0], TEX_FILTER_WRAP | TEX_FILTER_SEPARATE_ALPHA | TEX_FILTER_FORCE_NON_WIC, 0, mipChain );
	if ( hr != S_OK )
	{
		assert(0);
		return;
	}

	srcImage.Release();

	// compress resulting image
	DirectX::ScratchImage compressedSurface;
	hr = DirectX::Compress( mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DXGI_FORMAT_BC1_UNORM, TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, compressedSurface );
	if ( hr != S_OK )
	{
		assert(0);
		return;
	}

	mipChain.Release();
			
	// save the image as a DDS
	wchar_t wDstFilename[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, dstFilename, -1, wDstFilename, sizeof(wDstFilename) );
    hr = DirectX::SaveToDDSFile( compressedSurface.GetImages(), compressedSurface.GetImageCount(), compressedSurface.GetMetadata(), DirectX::DDS_FLAGS_NONE, wDstFilename );
	if ( hr != S_OK )
	{
		assert(0);
		return;
	}

	compressedSurface.Release();
}

// compress ambient occulusion as single BC4 texture (1 channel greyscale)
void GGTerrain_CompressAO( LPSTR aoFilename, LPSTR dstFilename )
{
	HRESULT hr;

	int width, height, channels;
	uint8_t* aoImageData = stbi_load( aoFilename, &width, &height, &channels, 4 );
	if ( !aoImageData )
	{
		char newSrcFilename[ MAX_PATH ];
		strcpy_s( newSrcFilename, MAX_PATH, aoFilename );
		char* szExt = strrchr( newSrcFilename, '.' );
		if ( szExt ) strcpy_s( szExt, 5, ".jpg" );
		else strcat_s( newSrcFilename, MAX_PATH, ".jpg" );
		aoImageData = stbi_load( newSrcFilename, &width, &height, &channels, 4 );
		if ( !aoImageData )
		{
			assert(0);
			return;
		}
	}

	// create final image
	DirectX::ScratchImage srcImage;
	hr = srcImage.Initialize2D( DXGI_FORMAT_R8_UNORM, width, height, 1, 1, DirectX::DDS_FLAGS_NONE );

	// copy source data into texture channels
	uint8_t* pDstPtr = srcImage.GetPixels();
	for( int y = 0; y < height; y++ )
	{
		for( int x = 0; x < width; x++ )
		{
			int index = y * width + x;

			// copy into red and green channels
			*(pDstPtr + 0) = aoImageData[ 4*index + 0 ];
			pDstPtr += 1;
		}
	}

	delete [] aoImageData;
	
	// create mipmaps
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps( srcImage.GetImages()[0], TEX_FILTER_WRAP | TEX_FILTER_SEPARATE_ALPHA | TEX_FILTER_FORCE_NON_WIC, 0, mipChain );
	if ( hr != S_OK )
	{
		assert(0);
		return;
	}

	srcImage.Release();

	// compress resulting image
	DirectX::ScratchImage compressedSurface;
	hr = DirectX::Compress( mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DXGI_FORMAT_BC4_UNORM, TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, compressedSurface );
	if ( hr != S_OK )
	{
		assert(0);
		return;
	}

	mipChain.Release();
			
	// save the image as a DDS
	wchar_t wDstFilename[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, dstFilename, -1, wDstFilename, sizeof(wDstFilename) );
    hr = DirectX::SaveToDDSFile( compressedSurface.GetImages(), compressedSurface.GetImageCount(), compressedSurface.GetMetadata(), DirectX::DDS_FLAGS_NONE, wDstFilename );
	if ( hr != S_OK )
	{
		assert(0);
		return;
	}

	compressedSurface.Release();
}


extern "C" bool GGTerrain_ConvertToDDS(uint8_t* ImageData, int width, int height, int channels, Texture* pTexture, const std::string& name)
{
	int maxwidth, active;
	GetConvertSettings( &maxwidth, &active);

	if (active == 0)
	{
		return false;
	}
	if (maxwidth > 4096)
		maxwidth = 4096;
	if (maxwidth < 16)
		maxwidth = 16;

	//PE: Only entitybank for now.
	if (!pestrcasestr(name.c_str(), "entitybank"))
	{
		return false;
	}
	//PE: And no decals.
	if (pestrcasestr(name.c_str(), "decal"))
	{
		return false;
	}

	HRESULT hr;
	GraphicsDevice* device = wiRenderer::GetDevice();
	DirectX::ScratchImage srcImage;
	hr = srcImage.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1, DirectX::DDS_FLAGS_NONE);

	bool bPureAlpha = true;
	//PE: copy to texture
	uint8_t* pDstPtr = srcImage.GetPixels();
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int index = y * width + x;

			//PE: Always RGBA
			*(pDstPtr + 0) = ImageData[4 * index + 0];
			*(pDstPtr + 1) = ImageData[4 * index + 1];
			*(pDstPtr + 2) = ImageData[4 * index + 2];
			*(pDstPtr + 3) = ImageData[4 * index + 3];
			if(ImageData[4 * index + 3] < 252)
				bPureAlpha = false;
			pDstPtr += 4;
		}
	}

	DirectX::ScratchImage *finalimage = &srcImage;
	DirectX::ScratchImage resizedImage;
	bool bResized = false;
	if(width > maxwidth)
	{
		hr = DirectX::Resize(srcImage.GetImages(), srcImage.GetImageCount(), srcImage.GetMetadata(), maxwidth, maxwidth, DirectX::TEX_FILTER_DEFAULT, resizedImage);
		if(!FAILED(hr))
		{
			bResized = true;
			finalimage = &resizedImage;
			width = maxwidth;
			height = maxwidth;
		}
	}

	//PE: Mipmaps
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps(finalimage->GetImages()[0], TEX_FILTER_WRAP | TEX_FILTER_SEPARATE_ALPHA | TEX_FILTER_FORCE_NON_WIC, 0, mipChain);
	if (hr != S_OK)
	{
		if (bResized)
			resizedImage.Release();
		srcImage.Release();
		assert(0);
		return false;
	}

	if (bResized)
		resizedImage.Release();
	srcImage.Release();

	//PE: Compress - TODO add BC5 normal map support in setup.ini
	DirectX::ScratchImage compressedSurface;
	if (bPureAlpha)
		hr = DirectX::Compress(mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DXGI_FORMAT_BC1_UNORM, TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, compressedSurface);
	else
		hr = DirectX::Compress(mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DXGI_FORMAT_BC3_UNORM, TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, compressedSurface);
	if (hr != S_OK)
	{
		mipChain.Release();
		assert(0);
		return false;
	}

	mipChain.Release();

	//PE: Later save a .dds version so we can use that later, and not have this "slow" setup.
	//wchar_t wDstFilename[MAX_PATH];
	//MultiByteToWideChar(CP_ACP, 0, dstFilename, -1, wDstFilename, sizeof(wDstFilename));
	//hr = DirectX::SaveToDDSFile(compressedSurface.GetImages(), compressedSurface.GetImageCount(), compressedSurface.GetMetadata(), DirectX::DDS_FLAGS_NONE, wDstFilename);
	//if (hr != S_OK)
	//{
	//	assert(0);
	//	return;
	//}

	TextureDesc desc;
	desc.CPUAccessFlags = 0;
	desc.ArraySize = 1;
	desc.Depth = 1;

	desc.Height = uint32_t(height);
	desc.Width = uint32_t(width);
	desc.layout = IMAGE_LAYOUT_SHADER_RESOURCE;
	desc.BindFlags = BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	if (bPureAlpha)
		desc.Format = FORMAT_BC1_UNORM;
	else
		desc.Format = FORMAT_BC3_UNORM;
	size_t mipLevelCount = compressedSurface.GetImageCount();
	desc.MipLevels = mipLevelCount;
	desc.MiscFlags = 0;
	desc.Usage = USAGE_IMMUTABLE;
	desc.layout = IMAGE_LAYOUT_SHADER_RESOURCE;

	desc.type = TextureDesc::TEXTURE_2D;

	//PE: Lowest size if block compression. already set above in maxwidth.
	if (desc.Width < 4)
		desc.Width = 4;
	if (desc.Height < 4)
		desc.Height = 4;


	std::vector<SubresourceData> initialData;
	initialData.reserve(compressedSurface.GetImageCount());

	for (size_t i = 0; i < compressedSurface.GetImageCount(); ++i)
	{
		const DirectX::Image* img = compressedSurface.GetImage(i, 0, 0);
		if (img)
		{
			SubresourceData subData;
			subData.pSysMem = img->pixels;
			subData.SysMemPitch = (uint32_t)img->rowPitch;
			subData.SysMemSlicePitch = (uint32_t)img->slicePitch;
			initialData.push_back(subData);
		}
		else
		{
			//PE: This should not happen if GenerateMipMaps and Compress is ok.
			compressedSurface.Release();
			return false;
		}
	}

	bool success = device->CreateTexture(&desc, initialData.data(), pTexture);

	compressedSurface.Release();
	return success;
}

void GGTerrain_DecompressImage( LPSTR srcFilename, LPSTR dstFilename )
{
	HRESULT hr;
	TexMetadata metaData;
	ScratchImage scratchImage;

	wchar_t wSrcFilename[512];
	MultiByteToWideChar( CP_ACP, 0, srcFilename, -1, wSrcFilename, sizeof(wSrcFilename) );
	DirectX::LoadFromDDSFile( wSrcFilename, 0, &metaData, scratchImage );

	DirectX::ScratchImage decompressedImage;
	hr = DirectX::Decompress( scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DXGI_FORMAT_R8G8B8A8_UNORM, decompressedImage );
	if ( hr != S_OK )
	{
		assert(0);
		return;
	}

	scratchImage.Release();
		
	// save the image as a DDS
	wchar_t wDstFilename[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, dstFilename, -1, wDstFilename, sizeof(wDstFilename) );
    hr = DirectX::SaveToDDSFile( decompressedImage.GetImages(), decompressedImage.GetImageCount(), decompressedImage.GetMetadata(), DirectX::DDS_FLAGS_NONE, wDstFilename );
	if ( hr != S_OK )
	{
		assert(0);
		return;
	}

	decompressedImage.Release();
}

void GGTerrain_DiscardMipLevels( LPSTR filename, uint32_t firstMip )
{
	HRESULT hr;
	TexMetadata metaData;
	ScratchImage scratchImage;

	wchar_t wSrcFilename[512];
	MultiByteToWideChar( CP_ACP, 0, filename, -1, wSrcFilename, sizeof(wSrcFilename) );
	DirectX::LoadFromDDSFile( wSrcFilename, 0, &metaData, scratchImage );

	uint32_t newWidth = (uint32_t) metaData.width >> firstMip;
	uint32_t newHeight = (uint32_t) metaData.height >> firstMip;
	uint32_t newMips = (uint32_t) metaData.mipLevels - firstMip;

	ScratchImage finalImage;
	finalImage.Initialize2D( metaData.format, newWidth, newHeight, 1, newMips );

	for( uint32_t m = 0; m < newMips; m++ )
	{
		uint32_t imageSize = (newWidth >> m) * (newHeight >> m);
		if ( metaData.format == DXGI_FORMAT_BC4_UNORM ) imageSize >>= 1;
		uint8_t* dstPtr = finalImage.GetImages()[m].pixels;
		uint8_t* srcPtr = scratchImage.GetImages()[m+firstMip].pixels;
		memcpy( dstPtr, srcPtr, imageSize );
	}

	scratchImage.Release();
		
	// save the image as a DDS
	wchar_t wDstFilename[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, filename, -1, wDstFilename, sizeof(wDstFilename) );
    hr = DirectX::SaveToDDSFile( finalImage.GetImages(), finalImage.GetImageCount(), finalImage.GetMetadata(), DirectX::DDS_FLAGS_NONE, wDstFilename );
	if ( hr != S_OK )
	{
		assert(0);
		return;
	}

	finalImage.Release();
}

void GGTerrain_CreateRenderTexture( int width, int height, int mipLevels, FORMAT format, Texture* tex ) 
{
	GraphicsDevice* device = wiRenderer::GetDevice();
	
	TextureDesc texDesc = {};
	texDesc.BindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE;
	texDesc.clear.color[0] = 0.0f;
	texDesc.clear.color[1] = 0.0f;
	texDesc.clear.color[2] = 0.0f;
	texDesc.clear.color[3] = 0.0f;
	texDesc.SampleCount = 1;
	texDesc.MipLevels = mipLevels;
	texDesc.ArraySize = 1;
	texDesc.Format = format;
	texDesc.Usage = USAGE_DEFAULT;
	texDesc.Width = width;
	texDesc.Height = height;

	device->CreateTexture( &texDesc, nullptr, tex );
	device->SetName( tex, "renderTex" );
}

void GGTerrain_CreateComputeTexture( int width, int height, FORMAT format, Texture* tex ) 
{
	GraphicsDevice* device = wiRenderer::GetDevice();
	
	TextureDesc texDesc = {};
	texDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
	texDesc.clear.color[0] = 0.0f;
	texDesc.clear.color[1] = 0.0f;
	texDesc.clear.color[2] = 0.0f;
	texDesc.clear.color[3] = 0.0f;
	texDesc.SampleCount = 1;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = format;
	texDesc.Usage = USAGE_DEFAULT;
	texDesc.Width = width;
	texDesc.Height = height;

	device->CreateTexture( &texDesc, nullptr, tex );
	device->SetName( tex, "computeTex" );
}

void GGTerrain_CreateEmptyTexture( int width, int height, int mipLevels, int levels, FORMAT format, Texture* tex ) 
{
	GraphicsDevice* device = wiRenderer::GetDevice();
	
	TextureDesc texDesc = {};
	texDesc.BindFlags = BIND_SHADER_RESOURCE;
	texDesc.SampleCount = 1;
	texDesc.MipLevels = mipLevels;
	texDesc.ArraySize = levels;
	texDesc.Format = format;
	texDesc.Usage = USAGE_DEFAULT;
	texDesc.Width = width;
	texDesc.Height = height;

	device->CreateTexture( &texDesc, nullptr, tex );
	device->SetName( tex, "tex" );
}

void GGTerrain_CreateCPUReadTexture( int width, int height, FORMAT format, Texture* tex ) 
{
	GraphicsDevice* device = wiRenderer::GetDevice();
	
	TextureDesc texDesc = {};
	texDesc.SampleCount = 1;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = format;
	texDesc.Usage = USAGE_STAGING;
	texDesc.CPUAccessFlags = CPU_ACCESS_READ;
	texDesc.Width = width;
	texDesc.Height = height;

	device->CreateTexture( &texDesc, nullptr, tex );
	device->SetName( tex, "stagingTex" );
}

char* ToBase64( const uint8_t* input, uint32_t length )
{
	const char *b64values = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
		
	int extra = length % 3;

	if ( length == 0 ) 
	{
		char *str = new char[1]; *str = 0;
		return str;
	}

	char* sOutput = new char[(length * 8) / 6 + 4]; 
	*sOutput = 0;
	char* ptrOut = sOutput;
	
	int pos = 0;
	for ( uint32_t i = 0; i < length/3; i++ )
	{
		// first byte
		int value1 = *(input+pos);
		pos++;
		
		// second byte
		int value2 = *(input+pos);
		pos++;
		
		// third byte
		int value3 = *(input+pos);
		pos++;
		
		int b64value1 = value1 >> 2;
		int b64value2 = ((value1 & 3) << 4) | (value2 >> 4);
		int b64value3 = ((value2 & 15) << 2) | (value3 >> 6);
		int b64value4 = value3 & 63;
		
		*ptrOut = b64values[b64value1]; ptrOut++;
		*ptrOut = b64values[b64value2]; ptrOut++;
		*ptrOut = b64values[b64value3]; ptrOut++;
		*ptrOut = b64values[b64value4]; ptrOut++;
	}
	
	if ( extra > 0 )
	{
		int value1 = *(input+pos);
		pos++;

		int value2 = 0;
		if ( extra > 1 )
		{
			value2 = *(input+pos);
			pos++;
		}
		
		int b64value1 = value1 >> 2;
		int b64value2 = ((value1 & 3) << 4) | (value2 >> 4);
		int b64value3 = ((value2 & 15) << 2);
		
		*ptrOut = b64values[b64value1]; ptrOut++;
		*ptrOut = b64values[b64value2]; ptrOut++;
		
		if ( extra == 2 ) 
		{
			*ptrOut = b64values[b64value3]; ptrOut++;
		}
		else
		{
			*ptrOut = '='; ptrOut++;
		}

		*ptrOut = '='; ptrOut++;
	}
	*ptrOut = 0;

	return sOutput;
}

uint8_t GetBase64Index( uint8_t letter )
{
	switch( letter )
	{
		case 'A': return 0;
		case 'B': return 1;
		case 'C': return 2;
		case 'D': return 3;
		case 'E': return 4;
		case 'F': return 5;
		case 'G': return 6;
		case 'H': return 7;
		case 'I': return 8;
		case 'J': return 9;
		case 'K': return 10;
		case 'L': return 11;
		case 'M': return 12;
		case 'N': return 13;
		case 'O': return 14;
		case 'P': return 15;
		case 'Q': return 16;
		case 'R': return 17;
		case 'S': return 18;
		case 'T': return 19;
		case 'U': return 20;
		case 'V': return 21;
		case 'W': return 22;
		case 'X': return 23;
		case 'Y': return 24;
		case 'Z': return 25;
		case 'a': return 26;
		case 'b': return 27;
		case 'c': return 28;
		case 'd': return 29;
		case 'e': return 30;
		case 'f': return 31;
		case 'g': return 32;
		case 'h': return 33;
		case 'i': return 34;
		case 'j': return 35;
		case 'k': return 36;
		case 'l': return 37;
		case 'm': return 38;
		case 'n': return 39;
		case 'o': return 40;
		case 'p': return 41;
		case 'q': return 42;
		case 'r': return 43;
		case 's': return 44;
		case 't': return 45;
		case 'u': return 46;
		case 'v': return 47;
		case 'w': return 48;
		case 'x': return 49;
		case 'y': return 50;
		case 'z': return 51;
		case '0': return 52;
		case '1': return 53;
		case '2': return 54;
		case '3': return 55;
		case '4': return 56;
		case '5': return 57;
		case '6': return 58;
		case '7': return 59;
		case '8': return 60;
		case '9': return 61;
		case '+': return 62;
		case '-': return 63;
		default: return 255;
	}
}

uint8_t* FromBase64( const char* input, uint32_t* outLength )
{
	const char *b64values = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	
	uint32_t inputLength = (uint32_t) strlen( input );
	if ( inputLength % 4 != 0 )
	{
		// invalid base 64
		return 0;
	}

	uint32_t length = (inputLength * 6) / 8 + 1;
	uint8_t* pOutput = new uint8_t[ length ]; 
	uint32_t outIndex = 0;
		
	int pos = 0;
	for ( uint32_t i = 0; i < inputLength/4; i++ )
	{
		uint8_t b64value1 = GetBase64Index( *(input+pos) ); pos++;
		uint8_t b64value2 = GetBase64Index( *(input+pos) ); pos++;
		uint8_t b64value3 = GetBase64Index( *(input+pos) ); pos++;
		uint8_t b64value4 = GetBase64Index( *(input+pos) ); pos++;

		if ( b64value1 == 255 || b64value2 == 255 ) 
		{
			// invalid base64
			delete [] pOutput;
			return 0;
		}

		pOutput[ outIndex++ ] = (b64value1 << 2) | (b64value2 >> 4);

		if ( b64value3 != 255 )
		{
			pOutput[ outIndex++ ] = (b64value2 << 4) | (b64value3 >> 2);
			
			if ( b64value4 != 255 )
			{
				pOutput[ outIndex++ ] = (b64value3 << 6) | b64value4;
			}
		}
	}
	
	if ( outLength ) *outLength = outIndex;

	return pOutput;
}

char* GGTerrain_GetSettings()
{
	__declspec(align(4)) uint8_t settings[ sizeof(GGTerrainParams) + sizeof(GGTerrainRenderParams) + sizeof(GGTerrainRenderParams2) + 12 ];
	
	uint32_t size = sizeof(GGTerrainParams);
	*((uint32_t*)settings) = size;
	uint32_t offset = 4;
	memcpy( settings+offset, &ggterrain_local_params, size );
	offset += size;

	size = sizeof(GGTerrainRenderParams);
	*((uint32_t*)(settings+offset)) = size;
	offset += 4;
	memcpy( settings+offset, &ggterrain_local_render_params, size );
	offset += size;

	size = sizeof(GGTerrainRenderParams2);
	*((uint32_t*)(settings+offset)) = size;
	offset += 4;
	memcpy( settings+offset, &ggterrain_local_render_params2, size );

	char* szBase64 = ToBase64( (uint8_t*)&settings, sizeof(settings) );
	return szBase64;
}

int GGTerrain_SetSettings( const char* settingsString )
{
	uint32_t length;
	uint8_t* settings = FromBase64( settingsString, &length );
	
	uint32_t size1 = *((uint32_t*)settings);
	uint32_t size2 = *((uint32_t*)(settings+4+size1));
	uint32_t size3 = *((uint32_t*)(settings+4+size1+4+size2));

	if ( size1 != sizeof(GGTerrainParams) || size2 != sizeof(GGTerrainRenderParams) || size3 != sizeof(GGTerrainRenderParams2) )
	{
		// invalid
		BoxerInfo( "That settings string is not compatible with this version of GameGuru MAX", "Invalid" );
		return -1;
	}

	uint32_t size = sizeof(GGTerrainParams);
	uint32_t offset = 4;
	memcpy( &ggterrain_global_params, settings+offset, size );
	offset += size;

	size = sizeof(GGTerrainRenderParams);
	offset += 4;
	memcpy( &ggterrain_global_render_params, settings+offset, size );
	offset += size;

	size = sizeof(GGTerrainRenderParams2);
	offset += 4;
	memcpy( &ggterrain_global_render_params2, settings+offset, size );

	delete [] settings;
	return 0;
}

char* GGTerrain_SaveSettings(int water_height)
{
	std::string output;
	output = "{\n";
	
	output += "\n  \"iProceduralTerrainType\": ";   output += std::to_string(ggterrain_extra_params.iProceduralTerrainType);

	output += ",\n  \"fractal_freq_increase\": ";     output += std::to_string( ggterrain_local_params.fractal_freq_increase );
	output += ",\n  \"fractal_freq_weight\": ";       output += std::to_string( ggterrain_local_params.fractal_freq_weight );
	output += ",\n  \"fractal_initial_freq\": ";      output += std::to_string( ggterrain_local_params.fractal_initial_freq );
	output += ",\n  \"fractal_initial_amplitute\": "; output += std::to_string( ggterrain_local_params.fractal_initial_amplitude );
	output += ",\n  \"fractal_levels\": ";            output += std::to_string( ggterrain_local_params.fractal_levels );
	output += ",\n  \"fractal_flags\": ";             output += std::to_string( ggterrain_local_params.fractal_flags );
	output += ",\n  \"height\": ";                    output += std::to_string( ggterrain_local_params.height );
	output += ",\n  \"minHeight\": ";                 output += std::to_string( ggterrain_local_params.minHeight );
	output += ",\n  \"lod_levels\": ";                output += std::to_string( ggterrain_local_params.lod_levels );
	output += ",\n  \"noise_fallof_power\": ";        output += std::to_string( ggterrain_local_params.noise_fallof_power );
	output += ",\n  \"noise_power\": ";               output += std::to_string( ggterrain_local_params.noise_power );
	output += ",\n  \"offset_x\": ";                  output += std::to_string( ggterrain_local_params.offset_x );
	output += ",\n  \"offset_y\": ";                  output += std::to_string( ggterrain_local_params.offset_y );
	output += ",\n  \"offset_z\": ";                  output += std::to_string( ggterrain_local_params.offset_z );
	output += ",\n  \"seed\": ";                      output += std::to_string( ggterrain_local_params.seed );
	output += ",\n  \"segments_per_chunk\": ";        output += std::to_string( ggterrain_local_params.segments_per_chunk );
	output += ",\n  \"segment_size\": ";              output += std::to_string( ggterrain_local_params.segment_size );
	output += ",\n  \"heightmap_scale\": ";           output += std::to_string( ggterrain_local_params.heightmap_scale );
	output += ",\n  \"iHeightMapEditMode\": ";        output += std::to_string( ggterrain_local_params.iHeightMapEditMode );
	output += ",\n  \"heightmap_roughness\": ";       output += std::to_string( ggterrain_local_params.heightmap_roughness );
	output += ",\n  \"height_outside_heightmap\": ";  output += std::to_string( ggterrain_local_params.height_outside_heightmap );
	output += ",\n  \"fade_outside_heightmap\": ";    output += std::to_string( ggterrain_local_params.fade_outside_heightmap );

	output += ",\n  \"baseLayerMaterial\": ";   output += std::to_string( ggterrain_local_render_params.baseLayerMaterial );
	output += ",\n  \"bumpiness\": ";           output += std::to_string( ggterrain_local_render_params.bumpiness );
	output += ",\n  \"flags\": ";               output += std::to_string( ggterrain_local_render_params.flags );
	output += ",\n  \"layerEndHeight0\": ";     output += std::to_string( ggterrain_local_render_params.layerEndHeight[0] );
	output += ",\n  \"layerEndHeight1\": ";     output += std::to_string( ggterrain_local_render_params.layerEndHeight[1] );
	output += ",\n  \"layerEndHeight2\": ";     output += std::to_string( ggterrain_local_render_params.layerEndHeight[2] );
	output += ",\n  \"layerEndHeight3\": ";     output += std::to_string( ggterrain_local_render_params.layerEndHeight[3] );
	output += ",\n  \"layerEndHeight4\": ";     output += std::to_string( ggterrain_local_render_params.layerEndHeight[4] );
	output += ",\n  \"layerMatIndex0\": ";      output += std::to_string( ggterrain_local_render_params.layerMatIndex[0] );
	output += ",\n  \"layerMatIndex1\": ";      output += std::to_string( ggterrain_local_render_params.layerMatIndex[1] );
	output += ",\n  \"layerMatIndex2\": ";      output += std::to_string( ggterrain_local_render_params.layerMatIndex[2] );
	output += ",\n  \"layerMatIndex3\": ";      output += std::to_string( ggterrain_local_render_params.layerMatIndex[3] );
	output += ",\n  \"layerMatIndex4\": ";      output += std::to_string( ggterrain_local_render_params.layerMatIndex[4] );
	output += ",\n  \"layerStartHeight0\": ";   output += std::to_string( ggterrain_local_render_params.layerStartHeight[0] );
	output += ",\n  \"layerStartHeight1\": ";   output += std::to_string( ggterrain_local_render_params.layerStartHeight[1] );
	output += ",\n  \"layerStartHeight2\": ";   output += std::to_string( ggterrain_local_render_params.layerStartHeight[2] );
	output += ",\n  \"layerStartHeight3\": ";   output += std::to_string( ggterrain_local_render_params.layerStartHeight[3] );
	output += ",\n  \"layerStartHeight4\": ";   output += std::to_string( ggterrain_local_render_params.layerStartHeight[4] );
	output += ",\n  \"maskScale\": ";           output += std::to_string( ggterrain_local_render_params.maskScale );
	output += ",\n  \"slopeEnd0\": ";           output += std::to_string( ggterrain_local_render_params.slopeEnd[0] );
	output += ",\n  \"slopeEnd1\": ";           output += std::to_string( ggterrain_local_render_params.slopeEnd[1] );
	output += ",\n  \"slopeMatIndex0\": ";      output += std::to_string( ggterrain_local_render_params.slopeMatIndex[0] );
	output += ",\n  \"slopeMatIndex1\": ";      output += std::to_string( ggterrain_local_render_params.slopeMatIndex[1] );
	output += ",\n  \"slopeStart0\": ";         output += std::to_string( ggterrain_local_render_params.slopeStart[0] );
	output += ",\n  \"slopeStart1\": ";         output += std::to_string( ggterrain_local_render_params.slopeStart[1] );
	output += ",\n  \"tilingPower\": ";         output += std::to_string( ggterrain_local_render_params.tilingPower );

	output += ",\n  \"detailLimit\": ";         output += std::to_string( ggterrain_local_render_params2.detailLimit );
	output += ",\n  \"detailScale\": ";         output += std::to_string( ggterrain_local_render_params2.detailScale );
	output += ",\n  \"editable_size\": ";       output += std::to_string( ggterrain_local_render_params2.editable_size );
	output += ",\n  \"flags2\": ";              output += std::to_string( ggterrain_local_render_params2.flags2 );
	output += ",\n  \"reflectance\": ";         output += std::to_string( ggterrain_local_render_params2.reflectance );
	output += ",\n  \"textureGamma\": ";        output += std::to_string( ggterrain_local_render_params2.textureGamma );


	//Other settings related to terrain.
	output += ",\n  \"water_height\": ";        output += std::to_string(water_height);
	output += ",\n  \"water_dist\": ";			output += std::to_string(ggtrees_global_params.water_dist);
	output += ",\n  \"grass_draw_dist\": ";		output += std::to_string(gggrass_global_params.lod_dist);
	output += ",\n  \"paint_density\": ";		output += std::to_string(ggtrees_global_params.paint_density);
	output += ",\n  \"paint_scale_random_low\": ";		output += std::to_string(ggtrees_global_params.paint_scale_random_low);
	output += ",\n  \"paint_scale_random_high\": ";		output += std::to_string(ggtrees_global_params.paint_scale_random_high);
	output += ",\n  \"paint_tree_bitfield\": ";		output += std::to_string(ggtrees_global_params.paint_tree_bitfield);
	output += ",\n  \"grass_paint_density\": ";		output += std::to_string(gggrass_global_params.paint_density);
	output += ",\n  \"paint_material\": ";		output += std::to_string(gggrass_global_params.paint_material);
	output += ",\n  \"paint_type\": ";		output += std::to_string(gggrass_global_params.paint_type);

	output += ",\n  \"grass_scale\": ";		output += std::to_string(gggrass_global_params.grass_scale);
	
	output += "\n}";

	size_t length = output.length() + 1;
	char* strOut = new char[ length ];
	strcpy_s( strOut, length, output.c_str() );

	return strOut;
}

// returns 1 if successful, 0 if not
int GGTerrain_LoadSettings( const char* settingsJSON, bool bRestoreWater)
{
	JSONElement* pElement = JSONElement::LoadJSONFromData( settingsJSON );
	if ( pElement->GetType() != JSON_OBJECT ) return 0;

	//PE: Parameters that need to be reset.
	gggrass_global_params.grass_scale = GGGRASS_SCALE;

	JSONObject* pObject = (JSONObject*) pElement;

	pElement = pObject->GetElement("iProceduralTerrainType");
	if (pElement) { ggterrain_extra_params.iProceduralTerrainType = ((JSONNumber*)pElement)->m_iValue; }

	pElement = pObject->GetElement( "fractal_freq_increase" );   
	if ( pElement ) { ggterrain_global_params.fractal_freq_increase = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "fractal_freq_weight" );     
	if ( pElement ) { ggterrain_global_params.fractal_freq_weight = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "fractal_initial_freq" );    
	if ( pElement ) { ggterrain_global_params.fractal_initial_freq = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "fractal_initial_amplitute" );    
	if ( pElement ) { ggterrain_global_params.fractal_initial_amplitude = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "fractal_levels" );          
	if ( pElement ) { ggterrain_global_params.fractal_levels = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "fractal_flags" );          
	if ( pElement ) { ggterrain_global_params.fractal_flags = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "height" );                  
	if ( pElement ) { ggterrain_global_params.height = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement("minHeight");
	if (pElement) { ggterrain_global_params.minHeight = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "lod_levels" );
	if ( pElement ) { ggterrain_global_params.lod_levels = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "noise_fallof_power" );      
	if ( pElement ) { ggterrain_global_params.noise_fallof_power = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "noise_power" );             
	if ( pElement ) { ggterrain_global_params.noise_power = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "offset_x" );                
	if ( pElement ) { ggterrain_global_params.offset_x = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "offset_y" );                
	if ( pElement ) { ggterrain_global_params.offset_y = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "offset_z" );                
	if ( pElement ) { ggterrain_global_params.offset_z = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "seed" );                    
	if ( pElement ) { ggterrain_global_params.seed = (uint32_t) ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "segments_per_chunk" );      
	if ( pElement ) { ggterrain_global_params.segments_per_chunk = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "segment_size" );            
	if ( pElement ) { ggterrain_global_params.segment_size = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "heightmap_scale" );            
	if ( pElement ) { ggterrain_global_params.heightmap_scale = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "iHeightMapEditMode" );          
	if ( pElement ) { ggterrain_global_params.iHeightMapEditMode = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "heightmap_roughness" );    
	if ( pElement ) { ggterrain_global_params.heightmap_roughness = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "height_outside_heightmap" );    
	if ( pElement ) { ggterrain_global_params.height_outside_heightmap = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "fade_outside_heightmap" );    
	if ( pElement ) { ggterrain_global_params.fade_outside_heightmap = ((JSONNumber*)pElement)->m_fValue; }
	
	pElement = pObject->GetElement( "baseLayerMaterial" );   
	if ( pElement ) { ggterrain_global_render_params.baseLayerMaterial = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "bumpiness" );           
	if ( pElement ) { ggterrain_global_render_params.bumpiness = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "flags" );               
	if ( pElement ) { ggterrain_global_render_params.flags = (uint32_t) ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "layerEndHeight0" );     
	if ( pElement ) { ggterrain_global_render_params.layerEndHeight[0] = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "layerEndHeight1" );     
	if ( pElement ) { ggterrain_global_render_params.layerEndHeight[1] = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "layerEndHeight2" );     
	if ( pElement ) { ggterrain_global_render_params.layerEndHeight[2] = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "layerEndHeight3" );     
	if ( pElement ) { ggterrain_global_render_params.layerEndHeight[3] = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "layerEndHeight4" );     
	if ( pElement ) { ggterrain_global_render_params.layerEndHeight[4] = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "layerMatIndex0" );      
	if ( pElement ) { ggterrain_global_render_params.layerMatIndex[0] = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "layerMatIndex1" );      
	if ( pElement ) { ggterrain_global_render_params.layerMatIndex[1] = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "layerMatIndex2" );      
	if ( pElement ) { ggterrain_global_render_params.layerMatIndex[2] = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "layerMatIndex3" );      
	if ( pElement ) { ggterrain_global_render_params.layerMatIndex[3] = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "layerMatIndex4" );      
	if ( pElement ) { ggterrain_global_render_params.layerMatIndex[4] = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "layerStartHeight0" );   
	if ( pElement ) { ggterrain_global_render_params.layerStartHeight[0] = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "layerStartHeight1" );   
	if ( pElement ) { ggterrain_global_render_params.layerStartHeight[1] = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "layerStartHeight2" );   
	if ( pElement ) { ggterrain_global_render_params.layerStartHeight[2] = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "layerStartHeight3" );   
	if ( pElement ) { ggterrain_global_render_params.layerStartHeight[3] = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "layerStartHeight4" );   
	if ( pElement ) { ggterrain_global_render_params.layerStartHeight[4] = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "maskScale" );           
	if ( pElement ) { ggterrain_global_render_params.maskScale = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "slopeEnd0" );           
	if ( pElement ) { ggterrain_global_render_params.slopeEnd[0] = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "slopeEnd1" );           
	if ( pElement ) { ggterrain_global_render_params.slopeEnd[1] = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "slopeMatIndex0" );      
	if ( pElement ) { ggterrain_global_render_params.slopeMatIndex[0] = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "slopeMatIndex1" );      
	if ( pElement ) { ggterrain_global_render_params.slopeMatIndex[1] = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "slopeStart0" );         
	if ( pElement ) { ggterrain_global_render_params.slopeStart[0] = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "slopeStart1" );         
	if ( pElement ) { ggterrain_global_render_params.slopeStart[1] = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "tilingPower" );         
	if ( pElement ) { ggterrain_global_render_params.tilingPower = ((JSONNumber*)pElement)->m_fValue; }
	
	pElement = pObject->GetElement( "detailLimit" );   
	if ( pElement ) { ggterrain_global_render_params2.detailLimit = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "detailScale" );   
	if ( pElement ) { ggterrain_global_render_params2.detailScale = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "editable_size" );   
	if ( pElement ) { ggterrain_global_render_params2.editable_size = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "flags2" );   
	if ( pElement ) { ggterrain_global_render_params2.flags2 = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement( "reflectance" );   
	if ( pElement ) { ggterrain_global_render_params2.reflectance = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement( "textureGamma" );   
	if ( pElement ) { ggterrain_global_render_params2.textureGamma = ((JSONNumber*)pElement)->m_fValue; }

	//Other settings related to terrain.
	pElement = pObject->GetElement("water_dist");
	if (pElement) { ggtrees_global_params.water_dist = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement("grass_draw_dist");
	if (pElement) { gggrass_global_params.lod_dist = ((JSONNumber*)pElement)->m_fValue; }
	pElement = pObject->GetElement("paint_density");
	if (pElement) { ggtrees_global_params.paint_density = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement("paint_scale_random_low");
	if (pElement) { ggtrees_global_params.paint_scale_random_low = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement("paint_scale_random_high");
	if (pElement) { ggtrees_global_params.paint_scale_random_high = ((JSONNumber*)pElement)->m_iValue; }

	pElement = pObject->GetElement("paint_tree_bitfield");
	if (pElement)
	{
		if(((JSONNumber*)pElement)->m_iValue == 0xffffffff)
			ggtrees_global_params.paint_tree_bitfield = 1;
			else
			ggtrees_global_params.paint_tree_bitfield = ((JSONNumber*)pElement)->m_iValue;
	}
	pElement = pObject->GetElement("grass_paint_density");
	if (pElement) { gggrass_global_params.paint_density = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement("paint_material");
	if (pElement) { gggrass_global_params.paint_material = ((JSONNumber*)pElement)->m_iValue; }
	pElement = pObject->GetElement("paint_type");
	if (pElement) { gggrass_global_params.paint_type = ((JSONNumber*)pElement)->m_iValue; }

	pElement = pObject->GetElement("grass_scale");
	if (pElement) { gggrass_global_params.grass_scale = ((JSONNumber*)pElement)->m_fValue; }


	if (bRestoreWater)
	{
		pElement = pObject->GetElement("water_height");
		if (pElement)
		{
			int  water_height = ((JSONNumber*)pElement)->m_iValue;
			void wicked_set_water_level(int water_height);
			wicked_set_water_level(water_height);
		}
	}

	delete pObject;
#ifdef ONLYLOADWHENUSED
	bCheckForNewTerrainTextures = true;
#endif
	return 1;
}

int GGTerrain_LoadHeightMap( const char* szFilename, uint32_t width, uint32_t height,bool bBigEndian)
{
	// Do not touch ggterrain_local_params.pHeightmapMain (local) or it may crash

	char filePath[ MAX_PATH ];
	strcpy_s( filePath, MAX_PATH, szFilename );
	GG_GetRealPath( filePath, 0 );

	FILE* pFile;
	fopen_s( &pFile, filePath, "rb" );
	fseek( pFile, 0, SEEK_END );
	uint32_t size = (uint32_t) ftell( pFile );
	fseek( pFile, 0, SEEK_SET );
	if ( size != width*height*sizeof(uint16_t) ) 
	{
		fclose( pFile );
		return 0; // incorrect file size for given width and height
	}

	if ( ggterrain_global_params.pHeightmapMain != ggterrain_local_params.pHeightmapMain ) 
	{
		// if the heightmap has already been updated this frame then we must delete our previous update.
		// once the heightmap has had a chance to move to ggterrain_local_params then it will get cleaned up there instead
		if ( ggterrain_global_params.pHeightmapMain ) delete [] ggterrain_global_params.pHeightmapMain;
		ggterrain_global_params.pHeightmapMain = 0;
	}

	ggterrain_global_params.heightmap_width = width;
	ggterrain_global_params.heightmap_height = height;
	ggterrain_global_params.pHeightmapMain = new uint16_t[ width * height ];

	fread( ggterrain_global_params.pHeightmapMain, 1, size, pFile );
	fclose( pFile );

	float* pHeightmap = new float[ width * height ];
	float minHeight = 100000;
	float maxHeight = 0;
	for( uint32_t y = 0; y < height; y++ )
	{
		for( uint32_t x = 0; x < width; x++ )
		{
			uint32_t index = y * width + x;
			uint16_t value = ggterrain_global_params.pHeightmapMain[ index ];
			if(bBigEndian) value = (value >> 8) | (value << 8);
			float height = value / 65535.0f;
			pHeightmap[ index ] = height;
			if ( height > maxHeight ) maxHeight = height;
			if ( height < minHeight ) minHeight = height;
		}
	}
	
	float heightRange = maxHeight - minHeight;
	for( uint32_t y = 0; y < height; y++ )
	{
		for( uint32_t x = 0; x < width; x++ )
		{
			uint32_t index = y * width + x;
			float height = (pHeightmap[ index ] - minHeight) / heightRange;
			height *= 65535.0f;
			if ( height < 0 ) height = 0;
			if ( height > 65535 ) height = 65535;
			ggterrain_global_params.pHeightmapMain[ index ] = (uint16_t) height;
		}
	}

	delete [] pHeightmap;

	return 1;
}

int GGTerrain_SetHeightMap(uint16_t* srcdata, uint32_t width, uint32_t height, bool bBigEndian)
{
	// Do not touch ggterrain_local_params.pHeightmapMain (local) or it may crash

	if (ggterrain_global_params.pHeightmapMain != ggterrain_local_params.pHeightmapMain)
	{
		// if the heightmap has already been updated this frame then we must delete our previous update.
		// once the heightmap has had a chance to move to ggterrain_local_params then it will get cleaned up there instead
		if (ggterrain_global_params.pHeightmapMain) delete[] ggterrain_global_params.pHeightmapMain;
		ggterrain_global_params.pHeightmapMain = 0;
	}

	ggterrain_global_params.heightmap_width = width;
	ggterrain_global_params.heightmap_height = height;
	ggterrain_global_params.pHeightmapMain = new uint16_t[width * height];

	memcpy(ggterrain_global_params.pHeightmapMain, srcdata, width*height * sizeof(uint16_t));

	float* pHeightmap = new float[width * height];
	float minHeight = 100000;
	float maxHeight = 0;
	for (uint32_t y = 0; y < height; y++)
	{
		for (uint32_t x = 0; x < width; x++)
		{
			uint32_t index = y * width + x;
			uint16_t value = ggterrain_global_params.pHeightmapMain[index];
			if (bBigEndian) value = (value >> 8) | (value << 8);
			float height = value / 65535.0f;
			pHeightmap[index] = height;
			if (height > maxHeight) maxHeight = height;
			if (height < minHeight) minHeight = height;
		}
	}

	float heightRange = maxHeight - minHeight;
	for (uint32_t y = 0; y < height; y++)
	{
		for (uint32_t x = 0; x < width; x++)
		{
			uint32_t index = y * width + x;
			float height = (pHeightmap[index] - minHeight) / heightRange;
			height *= 65535.0f;
			if (height < 0) height = 0;
			if (height > 65535) height = 65535;
			ggterrain_global_params.pHeightmapMain[index] = (uint16_t)height;
		}
	}

	delete[] pHeightmap;

	return 1;
}

void GGTerrain_RemoveHeightMap()
{
	// Do not touch ggterrain_local_params.pHeightmapMain (local) or it may crash

	if ( ggterrain_global_params.pHeightmapMain != ggterrain_local_params.pHeightmapMain ) 
	{
		// if the heightmap has already been updated this frame then we must delete our previous update.
		// once the heightmap has had a chance to move to ggterrain_local_params then it will get cleaned up there instead
		if ( ggterrain_global_params.pHeightmapMain ) delete [] ggterrain_global_params.pHeightmapMain;
		ggterrain_global_params.pHeightmapMain = 0;
	}

	ggterrain_global_params.heightmap_width = 0;
	ggterrain_global_params.heightmap_height = 0;
	ggterrain_global_params.pHeightmapMain = 0;
}


uint32_t GGTerrain_GetHeightmapDataSize(uint32_t &width, uint32_t &height)
{
	if (!ggterrain_global_params.pHeightmapMain) return(0);
	width = ggterrain_global_params.heightmap_width;
	height = ggterrain_global_params.heightmap_height;
	return ggterrain_global_params.heightmap_width * ggterrain_global_params.heightmap_height * sizeof(uint16_t);
}

int GGTerrain_GetHeightmapData(uint16_t* data)
{
	if (!ggterrain_global_params.pHeightmapMain) return(0);
	uint32_t size = ggterrain_global_params.heightmap_width * ggterrain_global_params.heightmap_height * sizeof(uint16_t);
	memcpy(data,ggterrain_global_params.pHeightmapMain, size);
	return 1;
}

int GGTerrain_SetHeightmapData(uint16_t* data, uint32_t width, uint32_t height)
{
	// Do not touch ggterrain_local_params.pHeightmapMain (local) or it may crash

	if (ggterrain_global_params.pHeightmapMain != ggterrain_local_params.pHeightmapMain)
	{
		// if the heightmap has already been updated this frame then we must delete our previous update.
		// once the heightmap has had a chance to move to ggterrain_local_params then it will get cleaned up there instead
		if (ggterrain_global_params.pHeightmapMain) delete[] ggterrain_global_params.pHeightmapMain;
		ggterrain_global_params.pHeightmapMain = 0;
	}

	ggterrain_global_params.heightmap_width = width;
	ggterrain_global_params.heightmap_height = height;
	ggterrain_global_params.pHeightmapMain = new uint16_t[width * height];

	memcpy(ggterrain_global_params.pHeightmapMain, data, width*height * sizeof(uint16_t));

	GGTerrain_InvalidateEverything(GGTERRAIN_INVALIDATE_TEXTURES);

	return 1;
}


uint16_t GGTerrain_CreateFlatArea()
{
	uint16_t id = 0;
	if ( ggterrain_flat_areas_free.NumItems() > 0 ) id = ggterrain_flat_areas_free.PopItem();
	else
	{
		// need to expand element array
		if ( ggterrain_flat_areas_array_size >= GGTERRAIN_MAX_FLAT_AREAS ) return 0; // full

		uint32_t newSize = ggterrain_flat_areas_array_size + (ggterrain_flat_areas_array_size / 2);
		if ( newSize < 2 ) newSize = 2;
		if ( newSize > GGTERRAIN_MAX_FLAT_AREAS ) newSize = GGTERRAIN_MAX_FLAT_AREAS;

		GGTerrainFlatArea* newArray = new GGTerrainFlatArea[ newSize ];
		if ( ggterrain_flat_areas ) memcpy( newArray, ggterrain_flat_areas, sizeof(GGTerrainFlatArea) * ggterrain_flat_areas_array_size );

		for( int i = (int)newSize-1; i >= (int)ggterrain_flat_areas_array_size; i-- )
		{
			if ( i == 0 ) continue;
			ggterrain_flat_areas_free.PushItem( i );
		}

		if ( ggterrain_flat_areas ) delete [] ggterrain_flat_areas;
		ggterrain_flat_areas = newArray;
		ggterrain_flat_areas_array_size = newSize;

		id = ggterrain_flat_areas_free.PopItem();
	}

	return id;
}

uint32_t GGTerrain_AddFlatRect( float posX, float posZ, float sizeX, float sizeZ, float angle, float height )
{
	float terrainHeight;
	if ( isnan(height) && GGTerrain_GetHeight( posX, posZ, &terrainHeight, 0, 0 ) ) 
	{
		height = terrainHeight;
	}

	uint16_t id = GGTerrain_CreateFlatArea();
	if ( id == 0 ) return 0;
	
	GGTerrainFlatArea* pArea = &ggterrain_flat_areas[ id ];
	pArea->SetTypeRect();
	pArea->x = posX;
	pArea->z = posZ;
	pArea->y = height;
	pArea->sizeX = sizeX;
	pArea->sizeZ = sizeZ;
	pArea->angle = angle;
	pArea->SetValid( 1 );

	float ca = cos( angle * PI / 180.0f );
	float sa = sin( angle * PI / 180.0f );

	for( int x = -1; x < 2; x += 2 )
	{
		for( int z = -1; z < 2; z += 2 )
		{
			float x1 = sizeX*x/2.0f;
			float z1 = sizeZ*z/2.0f;

			// clockwise rotation
			float xr = x1 * ca + z1 * sa;
			float zr = z1 * ca - x1 * sa;

			xr += posX;
			zr += posZ;

			if ( xr < ggterrain_internal_params.update_flat_areas_minX ) ggterrain_internal_params.update_flat_areas_minX = xr;
			if ( xr > ggterrain_internal_params.update_flat_areas_maxX ) ggterrain_internal_params.update_flat_areas_maxX = xr;
			if ( zr < ggterrain_internal_params.update_flat_areas_minZ ) ggterrain_internal_params.update_flat_areas_minZ = zr;
			if ( zr > ggterrain_internal_params.update_flat_areas_maxZ ) ggterrain_internal_params.update_flat_areas_maxZ = zr;
		}
	}

	ggterrain_internal_params.update_flat_areas = 1;

	GGTrees_UpdateFlatArea( 0, pArea->GetType(), posX, posZ, sizeX, sizeZ, angle );
	GGGrass_UpdateFlatArea( 0, pArea->GetType(), posX, posZ, sizeX, sizeZ, angle );
	return id;
}

uint32_t GGTerrain_AddFlatCircle( float posX, float posZ, float diameter, float height )
{
	float terrainHeight;
	if ( isnan(height) && GGTerrain_GetHeight( posX, posZ, &terrainHeight, 0, 0 ) ) 
	{
		height = terrainHeight;
	}

	uint16_t id = GGTerrain_CreateFlatArea();
	if ( id == 0 ) return 0;

	GGTerrainFlatArea* pArea = &ggterrain_flat_areas[ id ];
	pArea->SetTypeCircle();
	pArea->x = posX;
	pArea->z = posZ;
	pArea->y = height;
	pArea->sizeX = diameter;
	pArea->SetValid( 1 );

	float minX = posX - (diameter / 2.0f);
	float minZ = posZ - (diameter / 2.0f);
	float maxX = posX + (diameter / 2.0f);
	float maxZ = posZ + (diameter / 2.0f);

	if ( minX < ggterrain_internal_params.update_flat_areas_minX ) ggterrain_internal_params.update_flat_areas_minX = minX;
	if ( minZ < ggterrain_internal_params.update_flat_areas_minZ ) ggterrain_internal_params.update_flat_areas_minZ = minZ;
	if ( maxX > ggterrain_internal_params.update_flat_areas_maxX ) ggterrain_internal_params.update_flat_areas_maxX = maxX;
	if ( maxZ > ggterrain_internal_params.update_flat_areas_maxZ ) ggterrain_internal_params.update_flat_areas_maxZ = maxZ;
	
	ggterrain_internal_params.update_flat_areas = 1;

	GGTrees_UpdateFlatArea( 0, pArea->GetType(), posX, posZ, diameter, 0, 0 );
	GGGrass_UpdateFlatArea( 0, pArea->GetType(), posX, posZ, diameter, 0, 0 );
	return id;
}

// angle is ignored for circle areas
void GGTerrain_UpdateFlatArea( uint32_t id, float posX, float posZ, float angle, float sizeX, float sizeZ, float height )
{
	if ( id >= ggterrain_flat_areas_array_size ) return;

	GGTerrainFlatArea* pArea = &ggterrain_flat_areas[ id ];
	if ( !pArea->IsValid() ) return;

	GGTrees_UpdateFlatArea( 1, pArea->GetType(), pArea->x, pArea->z, pArea->sizeX, pArea->sizeZ, pArea->angle );
	GGGrass_UpdateFlatArea( 1, pArea->GetType(), pArea->x, pArea->z, pArea->sizeX, pArea->sizeZ, pArea->angle );

	if ( pArea->GetType() == GGTERRAIN_FLAT_AREA_TYPE_RECT )
	{
		float ca = cos( pArea->angle * PI / 180.0f );
		float sa = sin( pArea->angle * PI / 180.0f );

		for( int x = -1; x < 2; x += 2 )
		{
			for( int z = -1; z < 2; z += 2 )
			{
				float x1 = pArea->sizeX *x;
				float z1 = pArea->sizeZ *z;

				// clockwise rotation
				float xr = x1 * ca + z1 * sa;
				float zr = z1 * ca - x1 * sa;

				xr += pArea->x;
				zr += pArea->z;

				if ( xr < ggterrain_internal_params.update_flat_areas_minX ) ggterrain_internal_params.update_flat_areas_minX = xr;
				if ( xr > ggterrain_internal_params.update_flat_areas_maxX ) ggterrain_internal_params.update_flat_areas_maxX = xr;
				if ( zr < ggterrain_internal_params.update_flat_areas_minZ ) ggterrain_internal_params.update_flat_areas_minZ = zr;
				if ( zr > ggterrain_internal_params.update_flat_areas_maxZ ) ggterrain_internal_params.update_flat_areas_maxZ = zr;
			}
		}
	}
	else if ( pArea->GetType() == GGTERRAIN_FLAT_AREA_TYPE_CIRCLE )
	{
		float minX = pArea->x - (pArea->sizeX / 2.0f);
		float minZ = pArea->z - (pArea->sizeX / 2.0f);
		float maxX = pArea->x + (pArea->sizeX / 2.0f);
		float maxZ = pArea->z + (pArea->sizeX / 2.0f);

		if ( minX < ggterrain_internal_params.update_flat_areas_minX ) ggterrain_internal_params.update_flat_areas_minX = minX;
		if ( minZ < ggterrain_internal_params.update_flat_areas_minZ ) ggterrain_internal_params.update_flat_areas_minZ = minZ;
		if ( maxX > ggterrain_internal_params.update_flat_areas_maxX ) ggterrain_internal_params.update_flat_areas_maxX = maxX;
		if ( maxZ > ggterrain_internal_params.update_flat_areas_maxZ ) ggterrain_internal_params.update_flat_areas_maxZ = maxZ;
	}

	pArea->x = posX;
	pArea->z = posZ;
	pArea->angle = angle;
	pArea->sizeX = sizeX;
	pArea->sizeZ = sizeZ;

	if ( pArea->GetType() == GGTERRAIN_FLAT_AREA_TYPE_RECT )
	{
		float ca = cos( pArea->angle * PI / 180.0f );
		float sa = sin( pArea->angle * PI / 180.0f );

		for( int x = -1; x < 2; x += 2 )
		{
			for( int z = -1; z < 2; z += 2 )
			{
				float x1 = pArea->sizeX*x;
				float z1 = pArea->sizeZ*z;

				// clockwise rotation
				float xr = x1 * ca + z1 * sa;
				float zr = z1 * ca - x1 * sa;

				xr += pArea->x;
				zr += pArea->z;

				if ( xr < ggterrain_internal_params.update_flat_areas_minX ) ggterrain_internal_params.update_flat_areas_minX = xr;
				if ( xr > ggterrain_internal_params.update_flat_areas_maxX ) ggterrain_internal_params.update_flat_areas_maxX = xr;
				if ( zr < ggterrain_internal_params.update_flat_areas_minZ ) ggterrain_internal_params.update_flat_areas_minZ = zr;
				if ( zr > ggterrain_internal_params.update_flat_areas_maxZ ) ggterrain_internal_params.update_flat_areas_maxZ = zr;
			}
		}
	}
	else if ( pArea->GetType() == GGTERRAIN_FLAT_AREA_TYPE_CIRCLE )
	{
		float minX = pArea->x - (pArea->sizeX / 2.0f);
		float minZ = pArea->z - (pArea->sizeX / 2.0f);
		float maxX = pArea->x + (pArea->sizeX / 2.0f);
		float maxZ = pArea->z + (pArea->sizeX / 2.0f);

		if ( minX < ggterrain_internal_params.update_flat_areas_minX ) ggterrain_internal_params.update_flat_areas_minX = minX;
		if ( minZ < ggterrain_internal_params.update_flat_areas_minZ ) ggterrain_internal_params.update_flat_areas_minZ = minZ;
		if ( maxX > ggterrain_internal_params.update_flat_areas_maxX ) ggterrain_internal_params.update_flat_areas_maxX = maxX;
		if ( maxZ > ggterrain_internal_params.update_flat_areas_maxZ ) ggterrain_internal_params.update_flat_areas_maxZ = maxZ;
	}

	float terrainHeight;
	if ( isnan(height) && GGTerrain_GetHeight( posX, posZ, &terrainHeight, 0, 0 ) ) 
	{
		height = terrainHeight;
	}

	pArea->y = height;

	GGTrees_UpdateFlatArea( 0, pArea->GetType(), pArea->x, pArea->z, pArea->sizeX, pArea->sizeZ, pArea->angle );
	GGGrass_UpdateFlatArea( 0, pArea->GetType(), pArea->x, pArea->z, pArea->sizeX, pArea->sizeZ, pArea->angle );

	ggterrain_internal_params.update_flat_areas = 1;
}

void GGTerrain_RemoveFlatArea( uint32_t id )
{
	if ( id >= ggterrain_flat_areas_array_size ) return;
	if ( id == 0 ) return;

	GGTerrainFlatArea* pArea = &ggterrain_flat_areas[ id ];
	pArea->SetValid( 0 );
	ggterrain_flat_areas_free.PushItem( id );

	GGTrees_UpdateFlatArea( 1, pArea->GetType(), pArea->x, pArea->z, pArea->sizeX, pArea->sizeZ, pArea->angle );
	GGGrass_UpdateFlatArea( 1, pArea->GetType(), pArea->x, pArea->z, pArea->sizeX, pArea->sizeZ, pArea->angle );

	if ( pArea->GetType() == GGTERRAIN_FLAT_AREA_TYPE_RECT )
	{
		float ca = cos( pArea->angle * PI / 180.0f );
		float sa = sin( pArea->angle * PI / 180.0f );

		for( int x = -1; x < 2; x += 2 )
		{
			for( int z = -1; z < 2; z += 2 )
			{
				float x1 = pArea->sizeX*x;
				float z1 = pArea->sizeZ*z;

				// clockwise rotation
				float xr = x1 * ca + z1 * sa;
				float zr = z1 * ca - x1 * sa;

				xr += pArea->x;
				zr += pArea->z;

				if ( xr < ggterrain_internal_params.update_flat_areas_minX ) ggterrain_internal_params.update_flat_areas_minX = xr;
				if ( xr > ggterrain_internal_params.update_flat_areas_maxX ) ggterrain_internal_params.update_flat_areas_maxX = xr;
				if ( zr < ggterrain_internal_params.update_flat_areas_minZ ) ggterrain_internal_params.update_flat_areas_minZ = zr;
				if ( zr > ggterrain_internal_params.update_flat_areas_maxZ ) ggterrain_internal_params.update_flat_areas_maxZ = zr;
			}
		}
	}
	else if ( pArea->GetType() == GGTERRAIN_FLAT_AREA_TYPE_CIRCLE )
	{
		float minX = pArea->x - (pArea->sizeX / 2.0f);
		float minZ = pArea->z - (pArea->sizeX / 2.0f);
		float maxX = pArea->x + (pArea->sizeX / 2.0f);
		float maxZ = pArea->z + (pArea->sizeX / 2.0f);

		if ( minX < ggterrain_internal_params.update_flat_areas_minX ) ggterrain_internal_params.update_flat_areas_minX = minX;
		if ( minZ < ggterrain_internal_params.update_flat_areas_minZ ) ggterrain_internal_params.update_flat_areas_minZ = minZ;
		if ( maxX > ggterrain_internal_params.update_flat_areas_maxX ) ggterrain_internal_params.update_flat_areas_maxX = maxX;
		if ( maxZ > ggterrain_internal_params.update_flat_areas_maxZ ) ggterrain_internal_params.update_flat_areas_maxZ = maxZ;
	}

	ggterrain_internal_params.update_flat_areas = 1;
}

void GGTerrain_RemoveAllFlatAreas()
{
	if ( ggterrain_flat_areas ) delete [] ggterrain_flat_areas;
	ggterrain_flat_areas_array_size = 4;
	ggterrain_flat_areas = new GGTerrainFlatArea[ ggterrain_flat_areas_array_size ];
	memset(ggterrain_flat_areas, 0, ggterrain_flat_areas_array_size * sizeof(GGTerrainFlatArea));

	timestampactivity(0, "GGTerrain_RemoveAllFlatAreas:4");
	ggterrain_flat_areas_free.Clear();
	ggterrain_flat_areas_free.Resize( ggterrain_flat_areas_array_size );
	
	timestampactivity(0, "GGTerrain_RemoveAllFlatAreas:5");
	for( uint32_t i = ggterrain_flat_areas_array_size-1; i > 0; i-- ) // don't add index 0 as that will be the error id
	{
		ggterrain_flat_areas_free.PushItem( i );
	}

	timestampactivity(0, "GGTerrain_RemoveAllFlatAreas:6");
	GGTrees_RestoreAllFlattened();
	timestampactivity(0, "GGTerrain_RemoveAllFlatAreas:7");
	GGGrass_RestoreAllFlattened();

	timestampactivity(0, "GGTerrain_RemoveAllFlatAreas:8");
	ggterrain_internal_params.update_flat_areas_minX = -1e20f;
	ggterrain_internal_params.update_flat_areas_minZ = -1e20f;
	ggterrain_internal_params.update_flat_areas_maxX = 1e20f;
	ggterrain_internal_params.update_flat_areas_maxZ = 1e20f;
	ggterrain_internal_params.update_flat_areas = 1;
}

// 0 = editor, 1 = test game and standalone
void GGTerrain_SetGameMode( int mode )
{

}

void GGTerrain_SetPerformanceMode( uint32_t mode )
{
	switch( mode )
	{
		case GGTERRAIN_PERFORMANCE_LOW:
		{
			ggterrain_global_params.lod_levels = 9; // min = 1, max = 16
			ggterrain_global_params.segments_per_chunk = 16; // must be a power of 2, max = 128
			ggterrain_global_params.segment_size = 16.0f;
			ggterrain_global_render_params2.detailScale = 0.5f;
			ggterrain_global_render_params2.detailLimit = 2;
			ggterrain_global_render_params.tilingPower = 0.68f;
			ggterrain_global_render_params2.readBackTextureReduction = 8;
		} break;

		case GGTERRAIN_PERFORMANCE_MED:
		{
			ggterrain_global_params.lod_levels = 9; // min = 1, max = 16
			ggterrain_global_params.segments_per_chunk = 32; // must be a power of 2, max = 128
			ggterrain_global_params.segment_size = 16.0f;
			ggterrain_global_render_params2.detailScale = 0.75f;
			ggterrain_global_render_params2.detailLimit = 1;
			ggterrain_global_render_params.tilingPower = 0.62f;
			ggterrain_global_render_params2.readBackTextureReduction = 6;
		} break;

		case GGTERRAIN_PERFORMANCE_HIGH:
		{
			ggterrain_global_params.lod_levels = 9; // min = 1, max = 16
			ggterrain_global_params.segments_per_chunk = 64; // must be a power of 2, max = 128
			ggterrain_global_params.segment_size = 8.0f;
			ggterrain_global_render_params2.detailScale = 1.0f;
			ggterrain_global_render_params2.detailLimit = 0;
			ggterrain_global_render_params.tilingPower = 0.56f;
			ggterrain_global_render_params2.readBackTextureReduction = 4;
		} break;

		case GGTERRAIN_PERFORMANCE_ULTRA:
		{
			ggterrain_global_params.lod_levels = 8; // min = 1, max = 16
			ggterrain_global_params.segments_per_chunk = 128; // must be a power of 2, max = 128
			ggterrain_global_params.segment_size = 8.0f;
			ggterrain_global_render_params2.detailScale = 1.0f;
			ggterrain_global_render_params2.detailLimit = 0;
			ggterrain_global_render_params.tilingPower = 0.45f;
			ggterrain_global_render_params2.readBackTextureReduction = 4;
		} break;
	}
}

void GGTerrain_CreateSphere( float diameter, int rows, int columns )
{
	if ( rows < 2 ) rows = 2;
	if ( rows > 254 ) rows = 254;
	if ( columns < 3 ) columns = 3;
	if ( columns > 254 ) columns = 254;

	float radius = diameter / 2.0f;

	int faces = (rows-1) * columns * 2;
	g_numVerticesSphere = (rows+1) * (columns+1); 
	g_numIndicesSphere = faces*3;
	
	g_IndicesSphere = new uint16_t[ g_numIndicesSphere ];

	g_VerticesSphere = new VertexSphere[ g_numVerticesSphere ];
	
	float fSegY = PI / rows;
	float fSegX = 2*PI / columns;

	float fSegU = 1.0f / columns;
	float fSegV = 1.0f / rows;

	// vertices
	uint32_t count = 0;
	for ( int j = 0; j <= rows; j++ )
	{
		float fSY = sin( fSegY*j );
		float fCY = cos( fSegY*j );

		for ( int i = 0; i <= columns; i++ )
		{
			g_VerticesSphere[ count ].x = sin( -fSegX*i ) * fSY * radius;
			g_VerticesSphere[ count ].y = fCY * radius;
			g_VerticesSphere[ count ].z = cos( -fSegX*i ) * fSY * radius;
			if ( j == 0 || j == rows ) g_VerticesSphere[ count ].u = fSegU*i + fSegU/2.0f;
			else g_VerticesSphere[ count ].u = fSegU*i;
			g_VerticesSphere[ count ].v = fSegV*j;
			count++;
		}
	}

	// normals
	for ( uint32_t i = 0 ; i < g_numVerticesSphere; i++ )
	{
		g_VerticesSphere[ i ].nx = g_VerticesSphere[ i ].x / radius;
		g_VerticesSphere[ i ].ny = g_VerticesSphere[ i ].y / radius;
		g_VerticesSphere[ i ].nz = g_VerticesSphere[ i ].z / radius;
	}

	// indices
	uint32_t countI = 0;
	// top row
	for ( int i = 0; i < columns; i++ )
	{
		int next = i+1;

		g_IndicesSphere[ countI*3 + 0 ] = i;
		g_IndicesSphere[ countI*3 + 1 ] = columns+1 + i;
		g_IndicesSphere[ countI*3 + 2 ] = columns+1 + next;
		countI++;
	}

	// middle rows
	for ( int j = 1; j < rows-1; j++ )
	{
		for ( int i = 0; i < columns; i++ )
		{
			int next = i+1;

			g_IndicesSphere[ countI*3 + 0 ] = (columns+1)*j + i;
			g_IndicesSphere[ countI*3 + 1 ] = (columns+1)*(j+1) + i;
			g_IndicesSphere[ countI*3 + 2 ] = (columns+1)*j + next;
			countI++;

			g_IndicesSphere[ countI*3 + 0 ] = (columns+1)*j + next;
			g_IndicesSphere[ countI*3 + 1 ] = (columns+1)*(j+1) + i;
			g_IndicesSphere[ countI*3 + 2 ] = (columns+1)*(j+1) + next;
			countI++;
		}
	}

	// bottom row
	for ( int i = 0; i < columns; i++ )
	{
		int next = i+1;

		g_IndicesSphere[ countI*3 + 0 ] = (columns+1)*(rows-1) + i;
		g_IndicesSphere[ countI*3 + 1 ] = (columns+1)*rows + i;
		g_IndicesSphere[ countI*3 + 2 ] = (columns+1)*(rows-1) + next;
		countI++;
	}
	
	
}

float pHeightMapEditMemMovedOutOfHeap[GGTERRAIN_HEIGHTMAP_EDIT_SIZE * GGTERRAIN_HEIGHTMAP_EDIT_SIZE];
uint8_t pMaterialMapMemMoveOutOfHeap[GGTERRAIN_MATERIALMAP_SIZE * GGTERRAIN_MATERIALMAP_SIZE];

#ifdef ONLYLOADWHENUSED
bool bTextureUploaded[GGTERRAIN_MAX_SOURCE_TEXTURES+1];
#endif

// initialize the system
int GGTerrain_Init( wiGraphics::CommandList cmd )
{
	if ( ggterrain_initialised ) return 1;
	ggterrain_initialised = 1;

#ifdef ONLYLOADWHENUSED
	for (int i = 0; i < GGTERRAIN_MAX_SOURCE_TEXTURES; i++)
	{
		bTextureUploaded[i] = false;
	}
#endif

	char pCurrDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, pCurrDir);
	g_DeferTextureUpdateMAXRootFolder_s = pCurrDir;

	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	uint32_t numThreads = sysinfo.dwNumberOfProcessors;
	if (numThreads > 1) numThreads--; // leave one thread for other work
	if (numThreads > 8) numThreads=8;

	ChunkGenerator::SetThreads( numThreads );
	FractalGenerator::SetThreads( numThreads );

	GraphicsDevice* device = wiRenderer::GetDevice();

	RandomInit();
	Random2Init();

	noise.reshuffle( ggterrain_local_params.seed );

	// shaders
	wiRenderer::LoadShader( VS, shaderQuadVS, "GGTerrainQuadVS.cso" );
	wiRenderer::LoadShader( PS, shaderQuadPS, "GGTerrainQuadPS.cso" );

	wiRenderer::LoadShader( VS, shaderPageGenVS, "GGTerrainPageGenVS.cso" );
	wiRenderer::LoadShader( PS, shaderPageGenPS, "GGTerrainPageGenPS.cso" );

	wiRenderer::LoadShader( VS, shaderMainVS, "GGTerrainVS.cso" );
	wiRenderer::LoadShader( PS, shaderMainVirtualPS, "GGTerrainVirtualPBR_PS.cso" );

	wiRenderer::LoadShader( VS, shaderMainPrepassVS, "GGTerrainPrepassVS.cso" );
	wiRenderer::LoadShader( PS, shaderMainPrepassPS, "GGTerrainPrepassPS.cso" );

	wiRenderer::LoadShader( VS, shaderReflectionPrepassVS, "GGTerrainPrepassRefVS.cso" );

	wiRenderer::LoadShader( CS, shaderReadBackMSCS, "GGTerrainReadBackMSCS.cso" );
	wiRenderer::LoadShader( CS, shaderReadBackCS, "GGTerrainReadBackCS.cso" );

	wiRenderer::LoadShader( VS, shaderMainShadowVS, "GGTerrainShadowMapVS.cso" );

	wiRenderer::LoadShader( VS, shaderEditCubeVS, "GGTerrainEditBoxVS.cso" );
	wiRenderer::LoadShader( PS, shaderEditCubePS, "GGTerrainEditBoxPS.cso" );

	wiRenderer::LoadShader( VS, shaderOverlayVS, "GGTerrainOverlayVS.cso" );
	wiRenderer::LoadShader( PS, shaderOverlayPS, "GGTerrainOverlayPS.cso" );

	wiRenderer::LoadShader( VS, shaderEnvProbeVS, "GGTerrainEnvProbeVS.cso" );
	wiRenderer::LoadShader( PS, shaderEnvProbePS, "GGTerrainEnvProbePS.cso" );

	wiRenderer::LoadShader( VS, shaderRampVS, "GGTerrainRampVS.cso" );
	wiRenderer::LoadShader( PS, shaderRampPS, "GGTerrainRampPS.cso" );

	wiRenderer::LoadShader( VS, sphereVS, "GGTerrainSphereVS.cso" );
	wiRenderer::LoadShader( PS, spherePS, "GGTerrainSpherePS.cso" );
	wiRenderer::LoadShader( PS, spherePrepassPS, "GGTerrainSpherePrepassPS.cso" );

	// images	

	/*
	const char* terrainTextureNames[] = {
		"01 - Grass, mud and stones 1",
		"02 - Grass, mud and stones 2",
		"03 - Grass, tree roots and stones",
		"04 - Mud and rocks",
		"05 - Mud, rocks and weeds 1",
		"06 - Mud, rocks and weeds 2",
		"07 - Muddy",
		"08 - Muddy rocks",
		"09 - Sandy and rocks 1",
		"10 - Sandy and rocks 2",
		"11 - Sandy and rocks 3",
		"12 - Sand, rocks and weeds",
		"13 - Cracking ice",
		"14 - Snow and ice",
		"15 - Ice and rocks 1",
		"16 - Ice and rocks 2",
		"17 - Dark rock 1",
		"18 - Dark rock 2",
		"19 - Mossy rock",
		"20 - Dark rock 3",
	};
	*/
	
	//if ( GetFileExists("Files/mipmap_debug.dds") ) GGTerrain_LoadTextureDDS( "Files/mipmap_debug.dds", &texMipmapDebug );
	GGTerrain_CreateFractalTexture( &texMask, 1024 );
	GGTerrain_CreateEmptyTexture( 2048, 2048, 10, GGTERRAIN_MAX_SOURCE_TEXTURES, FORMAT_BC7_UNORM_SRGB, &texColorArray );
	GGTerrain_CreateEmptyTexture( 2048, 2048, 10, GGTERRAIN_MAX_SOURCE_TEXTURES, FORMAT_BC5_UNORM, &texNormalsArray );
#ifdef GGTERRAIN_USE_SURFACE_TEXTURE
	GGTerrain_CreateEmptyTexture( 2048, 2048, 10, GGTERRAIN_MAX_SOURCE_TEXTURES, FORMAT_BC1_UNORM, &texSurfaceArray ); // R: occlusion, G: roughness, B: metalness
#else
	GGTerrain_CreateEmptyTexture( 2048, 2048, 10, GGTERRAIN_MAX_SOURCE_TEXTURES, FORMAT_BC5_UNORM, &texRoughnessMetalnessArray );
	GGTerrain_CreateEmptyTexture( 2048, 2048, 10, GGTERRAIN_MAX_SOURCE_TEXTURES, FORMAT_BC4_UNORM, &texAOArray );
#endif

	colorDDS.format = DXGI_FORMAT_BC7_UNORM;
	normalDDS.format = DXGI_FORMAT_BC5_UNORM;
	surfaceDDS.format = DXGI_FORMAT_BC1_UNORM;
	GGTerrain_ReloadTextures(cmd);
	
	//if ( GetFileExists("Files/Grass, mud and stones 1_color.dds") )        GGTerrain_LoadTextureDDS( "Files/Grass, mud and stones 1_color.dds",    &texColor );
	//if ( GetFileExists("Files/Grass, mud and stones 1_normal.dds") )       GGTerrain_LoadTextureDDS( "Files/Grass, mud and stones 1_normal.dds",   &texNormals );
	
	/*
	const char* origNames[] = {
		"rrRFpeBol0yyENN8gdrw7g_4K",
		"TWfeorkCv0yHYqJWgteq0A_4K",
		"7gir7IuixU6sdfkf_88ZNw_4K",
		"BTcqk6tO5EOog-wvmZpYvQ_4K",
		"4VtFp_v6-EOPiUMWilDGIA_4K",
		"YMKpuREwMUultXc196xPtw_4K",
		"F4FEiA96a02If3JnSCa_1Q_4K",
		"mSUxKvgtOkqoooYbmSJ6kA_4K",
		"1oKsiZAKp0G7vzzZFw-MNw_4K",
		"5jYkmOUSi0-yCum8lV2YdA_4K",
		"5e46JhgUGk2oVHTKKoSb_g_4K",
		"Ground 56_4K",
		"snow_26",
		"snow_29",
		"snow_25",
		"snow_04",
		"Rock 778_4K",
		"Rock 777_4K",
		"Rock 784_4K",
		"Rock 782_4K",
	};

	*/
	/*
	for( uint32_t i = 0; i < 20; i++ )
	{
		char szDstRoot[ MAX_PATH ];

		sprintf_s( szDstRoot, MAX_PATH, "E:\\Temp\\GGMax\\Terrain Textures 1K Compressed\\%s\\%s", folderNames[ i ], folderNames[ i ] + 5 );
		
		char szDstColorPath[ MAX_PATH ];
		char szDstNormalPath[ MAX_PATH ];
		char szDstRoughnessMetalnessPath[ MAX_PATH ];
		char szDstAOPath[ MAX_PATH ];

		sprintf_s( szDstColorPath, MAX_PATH, "%s_Color.dds", szDstRoot );
		sprintf_s( szDstNormalPath, MAX_PATH, "%s_Normal.dds", szDstRoot );
		sprintf_s( szDstRoughnessMetalnessPath, MAX_PATH, "%s_RoughnessMetalness.dds", szDstRoot );
		sprintf_s( szDstAOPath, MAX_PATH, "%s_AO.dds", szDstRoot );

		GGTerrain_DiscardMipLevels( szDstColorPath, 1 );
		GGTerrain_DiscardMipLevels( szDstNormalPath, 1 );
		GGTerrain_DiscardMipLevels( szDstRoughnessMetalnessPath,1 );
		GGTerrain_DiscardMipLevels( szDstAOPath, 1 );
	}
	*/
	/*
	for( uint32_t i = 0; i < 20; i++ )
	{
		char szSrcRoot[ MAX_PATH ];
		char szDstRoot[ MAX_PATH ];

		sprintf_s( szSrcRoot, MAX_PATH, "E:\\Downloads\\Terrain Textures Original\\textures\\maps_%02d\\maps\\%s", i+1, origNames[ i ] );
		sprintf_s( szDstRoot, MAX_PATH, "E:\\Temp\\GGMax\\Final\\%s\\%s", folderNames[ i ], folderNames[ i ] + 5 );
		
		char szSrcNormalPath[ MAX_PATH ];
		char szSrcRoughnessPath[ MAX_PATH ];
		char szSrcMetalnessPath[ MAX_PATH ];
		char szSrcAOPath[ MAX_PATH ];

		sprintf_s( szSrcNormalPath, MAX_PATH, "%s_Normal.png", szSrcRoot );
		sprintf_s( szSrcRoughnessPath, MAX_PATH, "%s_Roughness.png", szSrcRoot );
		sprintf_s( szSrcMetalnessPath, MAX_PATH, "%s_Metalness.png", szSrcRoot );
		sprintf_s( szSrcAOPath, MAX_PATH, "%s_AO.png", szSrcRoot );

		char szDstNormalPath[ MAX_PATH ];
		char szDstRoughnessMetalnessPath[ MAX_PATH ];
		char szDstAOPath[ MAX_PATH ];

		sprintf_s( szDstNormalPath, MAX_PATH, "%s_Normal.dds", szDstRoot );
		sprintf_s( szDstRoughnessMetalnessPath, MAX_PATH, "%s_RoughnessMetalness.dds", szDstRoot );
		sprintf_s( szDstAOPath, MAX_PATH, "%s_AO.dds", szDstRoot );

		GGTerrain_CompressNormals( szSrcNormalPath, szDstNormalPath );
		GGTerrain_CompressRoughnessMetalness( szSrcRoughnessPath, szSrcMetalnessPath, szDstRoughnessMetalnessPath );
		GGTerrain_CompressAO( szSrcAOPath, szDstAOPath );
	}
	*/

	//GGTerrain_CompressNormals( "E:/Downloads/grass2/grass_green_02_normal.png", "E:/Downloads/grass2/grass2_Normal.dds" );
	//GGTerrain_CompressRoughnessMetalness( "E:/Downloads/grass2/grass_green_02_roughness.png", "E:/Downloads/grass2/grass_green_02_metallic.png", "E:/Downloads/grass2/grass2_RoughnessMetalness.dds" );
	//GGTerrain_CompressAO( "E:/Downloads/grass2/grass_green_02_ambient_occlusion.png", "E:/Downloads/grass2/grass2_AO.dds" );
	
	//GGTerrain_DecompressImage( "E:/Programs/GameGuruMAX/Max/Files/Grass, mud and stones 1_color.dds", "E:\\Temp\\GGMax\\color_RGB.dds" );
	//GGTerrain_DecompressImage( "E:/Downloads/grass1-bl/grass1_color.dds", "E:/Downloads/grass1-bl/grass1_color_RGB.dds" );

	// final page table texture
	GGTerrain_CreateEmptyTexture( pagesX, pagesY, GGTERRAIN_MAX_PAGE_TABLE_MIP, 1, FORMAT_R16_UNORM, &texPageTableFinal );

	/*
	GGTerrain_LoadHeightMap( "Files/heightmaps/Valley.raw", 4096, 4096 );
	ggterrain_global_params.noise_power = 1.0f;
	ggterrain_global_params.noise_fallof_power = 0.0f;
	ggterrain_global_params.fractal_initial_freq = 0.317f;
	ggterrain_global_params.fractal_levels = 7;
	ggterrain_global_params.fractal_freq_increase = 2.5f;
	ggterrain_global_params.fractal_freq_weight = 0.4f;
	ggterrain_global_params.heightmap_roughness = 0.4f;
	ggterrain_global_params.heightmap_scale = 0.1f;
	ggterrain_global_params.height = GGTerrain_MetersToUnits( 2700.0f );
	ggterrain_global_params.offset_y = GGTerrain_MetersToUnits( -1000.0f );
	ggterrain_global_render_params.layerMatIndex[ 2 ] = 0x100 | 24;	
	ggterrain_global_render_params.slopeStart[ 0 ] = 0.15f;
	ggterrain_global_render_params.slopeEnd[ 0 ] = 0.3f;
	*/

	/*
	GGTerrain_LoadHeightMap( "Files/heightmaps/Mountains.raw", 4096, 4096 );
	ggterrain_global_params.noise_power = 1.0f;
	ggterrain_global_params.noise_fallof_power = 0.0f;
	ggterrain_global_params.fractal_initial_freq = 0.317f;
	ggterrain_global_params.fractal_levels = 7;
	ggterrain_global_params.fractal_freq_increase = 2.5f;
	ggterrain_global_params.fractal_freq_weight = 0.4f;
	ggterrain_global_params.heightmap_roughness = 0.4f;
	ggterrain_global_params.heightmap_scale = 0.1f;
	ggterrain_global_params.height = GGTerrain_MetersToUnits( 2700.0f );
	ggterrain_global_params.offset_y = GGTerrain_MetersToUnits( -1000.0f );
	ggterrain_global_render_params.layerMatIndex[ 2 ] = 0x100 | 24;	
	ggterrain_global_render_params.slopeStart[ 0 ] = 0.15f;
	ggterrain_global_render_params.slopeEnd[ 0 ] = 0.3f;
	*/

	/*
	GGTerrain_LoadHeightMap( "Files/heightmaps/River.raw", 4096, 4096 );
	ggterrain_global_params.noise_power = 1.0f;
	ggterrain_global_params.noise_fallof_power = 0.0f;
	ggterrain_global_params.fractal_initial_freq = 0.317f;
	ggterrain_global_params.fractal_levels = 7;
	ggterrain_global_params.fractal_freq_increase = 2.5f;
	ggterrain_global_params.fractal_freq_weight = 0.4f;
	ggterrain_global_params.heightmap_roughness = 0.2f;
	ggterrain_global_params.heightmap_scale = 0.2f;
	ggterrain_global_params.height = GGTerrain_MetersToUnits( 2000.0f );
	ggterrain_global_params.offset_y = GGTerrain_MetersToUnits( -132.0f );
	ggterrain_global_render_params.layerMatIndex[ 2 ] = 0x100 | 24;	
	ggterrain_global_render_params.slopeStart[ 0 ] = 0.15f;
	ggterrain_global_render_params.slopeEnd[ 0 ] = 0.3f;
	*/
	
	/*
	GGTerrain_LoadHeightMap( "Files/heightmaps/Hills.raw", 4096, 4096 );
	ggterrain_global_params.noise_power = 1.0f;
	ggterrain_global_params.noise_fallof_power = 0.0f;
	ggterrain_global_params.fractal_initial_freq = 0.274f;
	ggterrain_global_params.fractal_levels = 7;
	ggterrain_global_params.fractal_freq_increase = 2.5f;
	ggterrain_global_params.fractal_freq_weight = 0.4f;
	ggterrain_global_params.heightmap_roughness = 1.0f;
	ggterrain_global_params.heightmap_scale = 0.5f;
	ggterrain_global_params.height = GGTerrain_MetersToUnits( 370.0f );
	ggterrain_global_params.offset_y = GGTerrain_MetersToUnits( -67.0f );
	*/

	/*
	GGTerrain_LoadHeightMap( "Files/heightmaps/GrandCanyon.raw", 4096, 4096 );
	ggterrain_global_params.noise_power = 1.0f;
	ggterrain_global_params.noise_fallof_power = 0.0f;
	ggterrain_global_params.fractal_initial_freq = 0.362f;
	ggterrain_global_params.fractal_levels = 7;
	ggterrain_global_params.fractal_freq_increase = 2.5f;
	ggterrain_global_params.fractal_freq_weight = 0.4f;
	ggterrain_global_params.heightmap_roughness = 0.5f;
	ggterrain_global_params.height = GGTerrain_MetersToUnits( 2290.0f );
	ggterrain_global_params.offset_y = GGTerrain_MetersToUnits( -55.0f );
	ggterrain_global_params.heightmap_scale = 0.125f;
	ggterrain_global_render_params.baseLayerMaterial = 0x100 | 30;
	ggterrain_global_render_params.layerMatIndex[ 0 ] = 0x100 | 30;
	ggterrain_global_render_params.layerMatIndex[ 1 ] = 0x100 | 14;
	ggterrain_global_render_params.layerMatIndex[ 2 ] = 0x100 | 15;	
	ggterrain_global_render_params.slopeMatIndex[ 0 ] = 0x100 | 16;
	ggterrain_global_render_params.layerStartHeight[ 0 ] = GGTerrain_MetersToUnits( 0 );
	ggterrain_global_render_params.layerStartHeight[ 1 ] = GGTerrain_MetersToUnits( 7.6f );
	ggterrain_global_render_params.layerStartHeight[ 2 ] = GGTerrain_MetersToUnits( 1500 );
	ggterrain_global_render_params.layerStartHeight[ 3 ] = GGTerrain_MetersToUnits( 10000 );
	ggterrain_global_render_params.layerStartHeight[ 4 ] = GGTerrain_MetersToUnits( 10000 );
	ggterrain_global_render_params.layerEndHeight[ 0 ] = GGTerrain_MetersToUnits( 2.5f );
	ggterrain_global_render_params.layerEndHeight[ 1 ] = GGTerrain_MetersToUnits( 68.24f );
	ggterrain_global_render_params.layerEndHeight[ 2 ] = GGTerrain_MetersToUnits( 1800 );
	ggterrain_global_render_params.layerEndHeight[ 3 ] = GGTerrain_MetersToUnits( 10000 );
	ggterrain_global_render_params.layerEndHeight[ 4 ] = GGTerrain_MetersToUnits( 10000 );
	ggterrain_global_render_params.slopeStart[ 0 ] = 0.2f;
	ggterrain_global_render_params.slopeStart[ 1 ] = 1.0f;
	ggterrain_global_render_params.slopeEnd[ 0 ] = 0.4f;
	ggterrain_global_render_params.slopeEnd[ 1 ] = 1.0f;
	*/

	/*
	GGTerrain_LoadHeightMap( "Files/heightmaps/Snowden.raw", 1081, 1081 );
	ggterrain_global_params.noise_power = 1.0f;
	ggterrain_global_params.noise_fallof_power = 0.0f;
	ggterrain_global_params.fractal_initial_freq = 0.274f;
	ggterrain_global_params.fractal_levels = 7;
	ggterrain_global_params.fractal_freq_increase = 2.5f;
	ggterrain_global_params.fractal_freq_weight = 0.4f;
	ggterrain_global_params.heightmap_roughness = 1.0f;
	ggterrain_global_params.heightmap_scale = 0.06f;
	ggterrain_global_params.height = GGTerrain_MetersToUnits( 1000.0f );
	ggterrain_global_params.offset_y = GGTerrain_MetersToUnits( -60.0f );
	*/

	/*
	GGTerrain_LoadHeightMap( "Files/heightmaps/Volcano.raw", 4096, 4096 );
	ggterrain_global_params.noise_power = 1.0f;
	ggterrain_global_params.noise_fallof_power = 0.0f;
	ggterrain_global_params.fractal_initial_freq = 0.274f;
	ggterrain_global_params.fractal_levels = 7;
	ggterrain_global_params.fractal_freq_increase = 2.5f;
	ggterrain_global_params.fractal_freq_weight = 0.4f;
	ggterrain_global_params.heightmap_roughness = 1.0f;
	ggterrain_global_params.heightmap_scale = 0.2f;
	ggterrain_global_params.height = GGTerrain_MetersToUnits( 1000.0f );
	ggterrain_global_params.offset_y = GGTerrain_MetersToUnits( -60.0f );
	ggterrain_global_render_params.baseLayerMaterial = 0x100 | 17;
	ggterrain_global_render_params.layerMatIndex[ 0 ] = 0x100 | 2;
	ggterrain_global_render_params.layerMatIndex[ 1 ] = 0x100 | 19;
	ggterrain_global_render_params.layerMatIndex[ 2 ] = 0x100 | 0;	
	ggterrain_global_render_params.slopeMatIndex[ 0 ] = 0x100 | 4;
	ggterrain_global_render_params.layerStartHeight[ 0 ] = GGTerrain_MetersToUnits( 0 );
	ggterrain_global_render_params.layerStartHeight[ 1 ] = GGTerrain_MetersToUnits( 28.5f );
	ggterrain_global_render_params.layerStartHeight[ 2 ] = GGTerrain_MetersToUnits( 10000 );
	ggterrain_global_render_params.layerStartHeight[ 3 ] = GGTerrain_MetersToUnits( 10000 );
	ggterrain_global_render_params.layerStartHeight[ 4 ] = GGTerrain_MetersToUnits( 10000 );
	ggterrain_global_render_params.layerEndHeight[ 0 ] = GGTerrain_MetersToUnits( 2.5f );
	ggterrain_global_render_params.layerEndHeight[ 1 ] = GGTerrain_MetersToUnits( 89.9f );
	ggterrain_global_render_params.layerEndHeight[ 2 ] = GGTerrain_MetersToUnits( 10000 );
	ggterrain_global_render_params.layerEndHeight[ 3 ] = GGTerrain_MetersToUnits( 10000 );
	ggterrain_global_render_params.layerEndHeight[ 4 ] = GGTerrain_MetersToUnits( 10000 );
	ggterrain_global_render_params.slopeStart[ 0 ] = 0.2f;
	ggterrain_global_render_params.slopeStart[ 1 ] = 1.0f;
	ggterrain_global_render_params.slopeEnd[ 0 ] = 0.4f;
	ggterrain_global_render_params.slopeEnd[ 1 ] = 1.0f;
	ggterrain_global_params.height_outside_heightmap = -2000;
	GGTerrain_SetGenerateTerrainOutsideHeightMap( 0 );
	*/

	// edtiable heightmap
	pHeightMapEdit = &pHeightMapEditMemMovedOutOfHeap[0]; //new float[ GGTERRAIN_HEIGHTMAP_EDIT_SIZE * GGTERRAIN_HEIGHTMAP_EDIT_SIZE ];
	pHeightMapEditType = new uint8_t[ GGTERRAIN_HEIGHTMAP_EDIT_SIZE * GGTERRAIN_HEIGHTMAP_EDIT_SIZE ];
	pHeightMapFlatAreas = new uint16_t[ GGTERRAIN_HEIGHTMAP_EDIT_SIZE * GGTERRAIN_HEIGHTMAP_EDIT_SIZE ];
	for( int i = 0; i < GGTERRAIN_HEIGHTMAP_EDIT_SIZE * GGTERRAIN_HEIGHTMAP_EDIT_SIZE; i++ )
	{
		pHeightMapEdit[ i ] = 0;
		pHeightMapEditType[ i ] = 0;

		pHeightMapFlatAreas[ i ] = 0;
	}
	
	// editable material map
	GGTerrain_CreateEmptyTexture( 4096, 4096, 1, 1, FORMAT_R8_UNORM, &texMaterialMap );
	//GGTerrain_CreateFractalTexture( &texMaterialMap, 4096 );

	pMaterialMap = &pMaterialMapMemMoveOutOfHeap[0]; // new uint8_t[GGTERRAIN_MATERIALMAP_SIZE * GGTERRAIN_MATERIALMAP_SIZE];
	memset( pMaterialMap, 0, GGTERRAIN_MATERIALMAP_SIZE * GGTERRAIN_MATERIALMAP_SIZE );
	/*
	for( uint32_t y = 0; y < GGTERRAIN_MATERIALMAP_SIZE; y++ )
	{
		for( uint32_t x = 0; x < GGTERRAIN_MATERIALMAP_SIZE; x++ )
		{
			uint32_t index = (y * GGTERRAIN_MATERIALMAP_SIZE) + x;

			if ( x == 0 || y == 0 || x == GGTERRAIN_MATERIALMAP_SIZE-1 || y == GGTERRAIN_MATERIALMAP_SIZE-1 )
			{
				pMaterialMap[ index ] = 0;
			}
			else
			{
				float fX = x / 150.0f;
				float fY = y / 150.0f;

				int bit0 = noise.fractal( 12, fX, fY ) >= 0 ? 1 : 0;
				int bit1 = noise.fractal( 12, fX+567, fY+833 ) >= 0 ? 1 : 0;
				int bit2 = noise.fractal( 12, fX+1231, fY+967 ) >= 0 ? 1 : 0;
				int bit3 = noise.fractal( 12, fX-96, fY-3687 ) >= 0 ? 1 : 0;
				int bit4 = noise.fractal( 12, fX-1218, fY-321 ) >= 0 ? 1 : 0;

				uint32_t value = (bit4 << 4) | (bit3 << 3) | (bit2 << 2) | (bit1 << 1) | bit0;
			
				pMaterialMap[ index ] = value;
			}
		}
	}
	*/
	device->UpdateTexture( &texMaterialMap, 0, 0, NULL, pMaterialMap, GGTERRAIN_MATERIALMAP_SIZE, -1 );

	ggterrain_flat_areas_array_size = 4;
	ggterrain_flat_areas = new GGTerrainFlatArea[ ggterrain_flat_areas_array_size ];
	memset(ggterrain_flat_areas, 0, ggterrain_flat_areas_array_size * sizeof(GGTerrainFlatArea));
	ggterrain_flat_areas_free.Resize( ggterrain_flat_areas_array_size );
	for( uint32_t i = ggterrain_flat_areas_array_size-1; i > 0; i-- ) // don't add index 0 as that will be the error id
	{
		ggterrain_flat_areas_free.PushItem( i );
	}

	// physical texture for virtual texture
	// trilinear filtering requires 2 pixel border all around on mip 0 and 1 pixel border all around on mip 1
	// anisotropic x4 filtering requires 4 pixel border all around on mip 0 and 2 pixel border all around on mip 1
	// anisotropic x8 filtering seems to be fine with 6 pixel border all around on mip 0 and 3 pixel border all around on mip 1, may require more
	GGTerrain_CreateRenderTexture( physTexSizeX, physTexSizeY, 2, FORMAT_R8G8B8A8_UNORM_SRGB, &texPagesColorAndMetal );
	GGTerrain_CreateRenderTexture( physTexSizeX, physTexSizeY, 2, FORMAT_R8G8B8A8_UNORM, &texPagesNormalsRoughnessAO );

	// have to create subresource views individually
	device->CreateSubresource( &texPagesColorAndMetal, RTV, 0, -1, 0, 1 );
	device->CreateSubresource( &texPagesColorAndMetal, RTV, 0, -1, 1, 1 );
	device->CreateSubresource( &texPagesNormalsRoughnessAO, RTV, 0, -1, 0, 1 );
	device->CreateSubresource( &texPagesNormalsRoughnessAO, RTV, 0, -1, 1, 1 );

	// create render passes for physical texture
	RenderPassDesc renderDesc = {};
	renderDesc.attachments.push_back( { RenderPassAttachment::RENDERTARGET, RenderPassAttachment::LOADOP_LOAD, &texPagesColorAndMetal, 0 } );
	renderDesc.attachments.push_back( { RenderPassAttachment::RENDERTARGET, RenderPassAttachment::LOADOP_LOAD, &texPagesNormalsRoughnessAO, 0 } );
	device->CreateRenderPass( &renderDesc, &renderPassPhysicalTex );

	// and for the mip level
	renderDesc.attachments[0].subresource = 1;
	renderDesc.attachments[1].subresource = 1;
	device->CreateRenderPass( &renderDesc, &renderPassPhysicalTexMip );
	
	// fill final page table with everything pointing to page 0
	memset( pageTableData, 0, GGTERRAIN_PAGE_TABLE_DEPTH*pagesX*pagesY*sizeof(uint16_t) ); // default everything to 0 (invalid page)
	uint32_t stride = pagesX * sizeof(uint16_t); 
	for( uint32_t mipLevel = 0; mipLevel < GGTERRAIN_MAX_PAGE_TABLE_MIP; mipLevel++ ) // not a full mip stack
	{
		device->UpdateTexture( &texPageTableFinal, mipLevel, 0, 0, pageTableData, stride, -1 );
		stride /= 2;
	}

	/*
	// fill physical texture with demo pages
	uint32_t* phyData = new uint32_t[ physTexSizeX * physTexSizeY ];
	for( uint32_t i = 0; i < physTexSizeX * physTexSizeY; i++ ) phyData[ i ] = 0xFFFFFFFF;
	for( uint32_t y = 0; y < paddedPageSize; y++ )
	{
		for( uint32_t x = 0; x < paddedPageSize; x++ )
		{
			uint32_t index = y * physTexSizeX + x;
			
			if ( y == 128 + pagePaddingLeft-1 ) phyData[ index ] = 0xFF0000FF; // ABGR
			else if ( y == 128 + pagePaddingLeft ) phyData[ index ] = 0xFF00FF00; // ABGR
			else if ( x == 128 + pagePaddingLeft-1 ) phyData[ index ] = 0xFFFFFF00; // ABGR
			else if ( x == 128 + pagePaddingLeft ) phyData[ index ] = 0xFF00FFFF; // ABGR
			else 
			phyData[ index ] = 0xFFFF0000; // ABGR
			//if ( x < 4 || x > 506 || y < 4 || y > 506 ) phyData[ index ] = 0xFF000000; // ABGR
			//else phyData[ index ] = 0xFF00FF00; // ABGR
		}
	}
	*/
/*
	for( int y = 520; y < 1014; y++ )
	{
		for( int x = 0; x < 500; x++ )
		{
			uint32_t index = y * physicalX + x;
			phyData[ index ] = 0xFF00FF00; // green
		}
	}

	for( int y = 0; y < 500; y++ )
	{
		for( int x = 520; x < 1014; x++ )
		{
			uint32_t index = y * physicalX + x;
			phyData[ index ] = 0xFFFF0000; // blue
		}
	}
	*/
	//device->UpdateTexture( &texPagesColorAndMetal, 0, 0, 0, phyData, physTexSizeX*sizeof(uint32_t), -1 );
	
	//for( uint32_t i = 0; i < physTexSizeX * physTexSizeY; i++ ) phyData[ i ] = 0xFF00FF00;
	/*
	for( uint32_t y = 0; y < paddedPageSize/2; y++ )
	{
		for( uint32_t x = 0; x < paddedPageSize/2; x++ )
		{
			uint32_t index = y * (physTexSizeX/2) + x;
			if ( y == 126 ) phyData[ index ] = 0xFF0000FF; // ABGR
			else if ( y == 127 ) phyData[ index ] = 0xFF00FFFF; // ABGR
			else if ( x == 126 ) phyData[ index ] = 0xFF00FF00; // ABGR
			else if ( x == 127 ) phyData[ index ] = 0xFFFFFF00; // ABGR
			else 
			phyData[ index ] = 0xFF00FF00; // ABGR
			//if ( x < 4 || x > 506 || y < 4 || y > 506 ) phyData[ index ] = 0xFF000000; // ABGR
			//else phyData[ index ] = 0xFF00FF00; // ABGR
		}
	}*/
	//device->UpdateTexture( &texPagesColorAndMetal, 1, 0, 0, phyData, physTexSizeX*sizeof(uint32_t)/2, -1 );
	//delete [] phyData;

	pageGenerationList.Resize( GGTERRAIN_REPLACEMENT_PAGE_MAX );
	
	// raster state
	rastState = {};
	rastState.FillMode = FILL_SOLID;
	rastState.CullMode = CULL_BACK;
	rastState.FrontCounterClockwise = false;
	rastState.DepthBias = 0;
	rastState.DepthBiasClamp = 0;
	rastState.SlopeScaledDepthBias = 0;
	rastState.DepthClipEnable = true;
	rastState.MultisampleEnable = false;
	rastState.AntialiasedLineEnable = false;
	
	// depth stencil state
	depthStateOpaque = {};
	depthStateOpaque.DepthEnable = true;
	depthStateOpaque.DepthFunc = COMPARISON_GREATER_EQUAL;
	depthStateOpaque.StencilEnable = false;
	depthStateOpaque.DepthWriteMask = DEPTH_WRITE_MASK_ALL;
	
	// blend state
	blendStateOpaque = {};
	blendStateOpaque.RenderTarget[0].BlendEnable = false;
	blendStateOpaque.RenderTarget[0].SrcBlend = BLEND_ONE;
	blendStateOpaque.RenderTarget[0].DestBlend = BLEND_ZERO;
	blendStateOpaque.RenderTarget[0].BlendOp = BLEND_OP_ADD;
	blendStateOpaque.RenderTarget[0].SrcBlendAlpha = BLEND_ONE;
	blendStateOpaque.RenderTarget[0].DestBlendAlpha = BLEND_ZERO;
	blendStateOpaque.RenderTarget[0].BlendOpAlpha = BLEND_OP_ADD;
	blendStateOpaque.RenderTarget[0].RenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
	blendStateOpaque.IndependentBlendEnable = false;
	
	// input layout
	inputLayout.elements = {
		{ "POSITION", 0, wiGraphics::FORMAT_R32G32B32_FLOAT, 0, 0,  INPUT_PER_VERTEX_DATA },
		{ "INORMAL",  0, wiGraphics::FORMAT_R8G8B8A8_UNORM,  0, 12, INPUT_PER_VERTEX_DATA },
		{ "ID",       0, wiGraphics::FORMAT_R32_UINT,        0, 16, INPUT_PER_VERTEX_DATA },
	};

	// pipeline state object
	PipelineStateDesc desc = {};
	desc.vs = &shaderMainVS;
	desc.ps = &shaderMainVirtualPS;

	desc.il = &inputLayout;
	desc.pt = TRIANGLESTRIP;
	desc.rs = &rastState;
	desc.dss = &depthStateOpaque;
	desc.bs = &blendStateOpaque;
	depthStateOpaque.DepthWriteMask = DEPTH_WRITE_MASK_ZERO;
	device->CreatePipelineState( &desc, &psoMain );

	rastState.FillMode = FILL_WIREFRAME;
	depthStateOpaque.DepthWriteMask = DEPTH_WRITE_MASK_ALL;
	device->CreatePipelineState( &desc, &psoMainWire );
	rastState.FillMode = FILL_SOLID;

	desc.vs = &shaderMainPrepassVS;
	desc.ps = &shaderMainPrepassPS;
	depthStateOpaque.DepthWriteMask = DEPTH_WRITE_MASK_ALL;
	device->CreatePipelineState( &desc, &psoMainPrepass );

	desc.vs = &shaderReflectionPrepassVS;
	desc.ps = nullptr;
	depthStateOpaque.DepthWriteMask = DEPTH_WRITE_MASK_ALL;
	device->CreatePipelineState( &desc, &psoReflectionPrepass );
	
	rastState.DepthBias = -1;
	rastState.SlopeScaledDepthBias = -4.0f;
	desc.vs = &shaderMainShadowVS;
	desc.ps = nullptr;
	rastState.DepthClipEnable = false;
	depthStateOpaque.DepthWriteMask = DEPTH_WRITE_MASK_ALL;
	device->CreatePipelineState( &desc, &psoMainShadow );
	rastState.DepthBias = 0;
	rastState.DepthClipEnable = true;
	rastState.SlopeScaledDepthBias = 0;

	// env probe layout with instances
	InputLayout envLayout;
	envLayout.elements = {
		{ "POSITION",     0, wiGraphics::FORMAT_R32G32B32_FLOAT, 0, 0,  INPUT_PER_VERTEX_DATA },
		{ "INORMAL",      0, wiGraphics::FORMAT_R8G8B8A8_UNORM,  0, 12, INPUT_PER_VERTEX_DATA },
		{ "ID",           0, wiGraphics::FORMAT_R32_UINT,        0, 16, INPUT_PER_VERTEX_DATA },
		{ "INSTANCEDATA", 0, wiGraphics::FORMAT_R32_UINT,        1, 0,  INPUT_PER_INSTANCE_DATA },
	};

	desc.il = &envLayout;
	desc.vs = &shaderEnvProbeVS;
	desc.ps = &shaderEnvProbePS;
	device->CreatePipelineState( &desc, &psoEnvProbe );

	// instance buffer
	GPUBufferDesc bd = {};
	bd.ByteWidth = sizeof(uint32_t) * 6;
	bd.BindFlags = BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	device->CreateBuffer( &bd, nullptr, &instanceBuffer );

	{
		// PBR spheres
		InputLayout sphereLayout;
		sphereLayout.elements = {
			{ "POSITION",     0, wiGraphics::FORMAT_R32G32B32_FLOAT, 0, 0,  INPUT_PER_VERTEX_DATA },
			{ "NORMAL",       0, wiGraphics::FORMAT_R32G32B32_FLOAT, 0, 12, INPUT_PER_VERTEX_DATA },
			{ "UV",           0, wiGraphics::FORMAT_R32G32_FLOAT,    0, 24, INPUT_PER_VERTEX_DATA },
		};

		RasterizerState rastState2 = {};
		rastState2.FillMode = FILL_SOLID;
		rastState2.CullMode = CULL_BACK;
		rastState2.FrontCounterClockwise = true;
		rastState2.DepthBias = 0;
		rastState2.DepthBiasClamp = 0;
		rastState2.SlopeScaledDepthBias = 0;
		rastState2.DepthClipEnable = true;
		rastState2.MultisampleEnable = false;
		rastState2.AntialiasedLineEnable = false;

		desc.rs = &rastState2;
		desc.vs = &sphereVS;
		desc.ps = &spherePS;
		desc.pt = TRIANGLELIST;
		desc.il = &sphereLayout;
		depthStateOpaque.DepthWriteMask = DEPTH_WRITE_MASK_ZERO;
		device->CreatePipelineState( &desc, &psoSphere );

		desc.ps = &spherePrepassPS;
		depthStateOpaque.DepthWriteMask = DEPTH_WRITE_MASK_ALL;
		device->CreatePipelineState( &desc, &psoSpherePrepass );
		
		desc.rs = &rastState;

		GGTerrain_CreateSphere( 2, 100, 100 );

		SubresourceData data = {};
		data.pSysMem = g_VerticesSphere;
		bd.ByteWidth = sizeof(VertexSphere) * g_numVerticesSphere;
		bd.BindFlags = BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		wiRenderer::GetDevice()->CreateBuffer( &bd, &data, &sphereVertexBuffer );

		// index buffer
		data.pSysMem = g_IndicesSphere;
		bd.ByteWidth = sizeof(uint16_t) * g_numIndicesSphere;
		bd.BindFlags = BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		wiRenderer::GetDevice()->CreateBuffer( &bd, &data, &sphereIndexBuffer );
	}

	// edit box layout
	InputLayout boxLayout;
	boxLayout.elements = {
		{ "POSITION", 0, wiGraphics::FORMAT_R32G32B32_FLOAT, 0, 0,  INPUT_PER_VERTEX_DATA },
	};

	// transparent edit box
	depthStateOpaque.DepthWriteMask = DEPTH_WRITE_MASK_ZERO;
	blendStateOpaque.RenderTarget[0].BlendEnable = true;
	blendStateOpaque.RenderTarget[0].SrcBlend = BLEND_SRC_ALPHA;
	blendStateOpaque.RenderTarget[0].DestBlend = BLEND_INV_SRC_ALPHA;
	blendStateOpaque.RenderTarget[0].BlendOp = BLEND_OP_ADD;
	rastState.CullMode = CULL_NONE;
	desc.vs = &shaderEditCubeVS;
	desc.ps = &shaderEditCubePS;
	desc.pt = TRIANGLELIST;
	desc.il = &boxLayout;
	rastState.FrontCounterClockwise = true;
	device->CreatePipelineState( &desc, &psoEditCube );
	blendStateOpaque.RenderTarget[0].BlendEnable = false;
	depthStateOpaque.DepthWriteMask = DEPTH_WRITE_MASK_ALL;
	rastState.CullMode = CULL_BACK;

	// transparent ramp
	depthStateOpaque.DepthWriteMask = DEPTH_WRITE_MASK_ZERO;
	blendStateOpaque.RenderTarget[0].BlendEnable = true;
	blendStateOpaque.RenderTarget[0].SrcBlend = BLEND_SRC_ALPHA;
	blendStateOpaque.RenderTarget[0].DestBlend = BLEND_INV_SRC_ALPHA;
	blendStateOpaque.RenderTarget[0].BlendOp = BLEND_OP_ADD;
	rastState.CullMode = CULL_NONE;
	desc.vs = &shaderRampVS;
	desc.ps = &shaderRampPS;
	desc.pt = TRIANGLELIST;
	rastState.FrontCounterClockwise = true;
	device->CreatePipelineState( &desc, &psoRamp );
	blendStateOpaque.RenderTarget[0].BlendEnable = false;
	depthStateOpaque.DepthWriteMask = DEPTH_WRITE_MASK_ALL;
	rastState.CullMode = CULL_BACK;
	
	// Quad pipeline state
	quadInputLayout.elements = {
		{ "POSITION", 0, wiGraphics::FORMAT_R32G32_FLOAT, 0, 0, INPUT_PER_VERTEX_DATA }		
	};
	
	desc.vs = &shaderQuadVS;
	desc.ps = &shaderQuadPS;
	desc.il = &quadInputLayout;
	desc.pt = TRIANGLELIST;
	depthStateOpaque.DepthEnable = false;
	depthStateOpaque.DepthWriteMask = DEPTH_WRITE_MASK_ZERO;
	rastState.FrontCounterClockwise = true;

	device->CreatePipelineState( &desc, &psoQuad );

	// overlay
	desc.vs = &shaderOverlayVS;
	desc.ps = &shaderOverlayPS;

	device->CreatePipelineState( &desc, &psoOverlay );

	// page gen pipeline state
	pageGenInputLayout.elements = {
		{ "POSITION", 0, wiGraphics::FORMAT_R32G32_FLOAT, 0, 0,  INPUT_PER_VERTEX_DATA },
		{ "UV",       0, wiGraphics::FORMAT_R32G32_FLOAT, 0, 8,  INPUT_PER_VERTEX_DATA },
		{ "HEIGHTUV", 0, wiGraphics::FORMAT_R32G32_FLOAT, 0, 16, INPUT_PER_VERTEX_DATA },
		{ "WORLDPOS", 0, wiGraphics::FORMAT_R32G32_FLOAT, 0, 24, INPUT_PER_VERTEX_DATA },
		{ "CHUNKID",  0, wiGraphics::FORMAT_R32_UINT,     0, 32, INPUT_PER_VERTEX_DATA },
	};
	
	desc.vs = &shaderPageGenVS;
	desc.ps = &shaderPageGenPS;
	desc.il = &pageGenInputLayout;
	desc.pt = TRIANGLELIST;
	depthStateOpaque.DepthEnable = false;
	depthStateOpaque.DepthWriteMask = DEPTH_WRITE_MASK_ZERO;
	rastState.FrontCounterClockwise = true;

	device->CreatePipelineState( &desc, &psoPageGen );

	// constant buffers
	bd.Usage = USAGE_DEFAULT;
	bd.ByteWidth = sizeof(TerrainCB);
	bd.BindFlags = BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, nullptr, &terrainConstantBuffer );

	// quad constant buffers
	bd.Usage = USAGE_DEFAULT;
	bd.ByteWidth = sizeof(sQuadVSConstantData);
	bd.BindFlags = BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, nullptr, &quadVSConstantBuffer );

	bd.Usage = USAGE_DEFAULT;
	bd.ByteWidth = sizeof(sQuadPSConstantData);
	bd.BindFlags = BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, nullptr, &quadPSConstantBuffer );
		
	// samplers
	SamplerDesc samplerDesc = {};
	samplerDesc.AddressU = TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = TEXTURE_ADDRESS_CLAMP;
	samplerDesc.Filter = FILTER_MIN_MAG_MIP_POINT;
	device->CreateSampler( &samplerDesc, &samplerPoint );

	samplerDesc.AddressU = TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = FILTER_MIN_MAG_MIP_POINT;
	device->CreateSampler( &samplerDesc, &samplerPointWrap );

	samplerDesc.AddressU = TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = TEXTURE_ADDRESS_CLAMP;
	samplerDesc.Filter = FILTER_MIN_MAG_LINEAR_MIP_POINT;
	device->CreateSampler( &samplerDesc, &samplerBilinear );

	samplerDesc.AddressU = TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = FILTER_MIN_MAG_LINEAR_MIP_POINT;
	device->CreateSampler( &samplerDesc, &samplerBilinearWrap );

	samplerDesc.AddressU = TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = TEXTURE_ADDRESS_CLAMP;
	samplerDesc.Filter = FILTER_MIN_MAG_MIP_LINEAR;
	device->CreateSampler( &samplerDesc, &samplerTrilinear );

	samplerDesc.AddressU = TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = FILTER_MIN_MAG_MIP_LINEAR;
	device->CreateSampler( &samplerDesc, &samplerTrilinearWrap );

	samplerDesc.AddressU = TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = FILTER_ANISOTROPIC;
#if (GGTERRAIN_TEXTURE_FILTERING == GGTERRAIN_TEXTURE_FILTERING_ANISO_X8)
	samplerDesc.MaxAnisotropy = 8;
#else
	samplerDesc.MaxAnisotropy = 4;
#endif
	device->CreateSampler( &samplerDesc, &samplerAnisotropicWrap );
	
	// box vertex buffer
	SubresourceData data = {};
	data.pSysMem = g_VerticesBox;
	bd.ByteWidth = sizeof(BoxVertex) * 8;
	bd.BindFlags = BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, &data, &boxVertexBuffer );

	// box index buffer
	data.pSysMem = g_IndicesBox;
	bd.ByteWidth = sizeof(uint16_t) * 36;
	bd.BindFlags = BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, &data, &boxIndexBuffer );

	// quad vertex buffer (no index buffer)
	data = {};
	data.pSysMem = g_VerticesQuad;
	bd.ByteWidth = sizeof(VertexQuad) * 6;
	bd.BindFlags = BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, &data, &quadVertexBuffer );

	// page gen vertex buffer (no index buffer)
	data = {};
	data.pSysMem = g_VerticesPageGen;
	bd.ByteWidth = sizeof(VertexPageGen) * 6 * GGTERRAIN_REPLACEMENT_PAGE_MAX;
	bd.BindFlags = BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, &data, &pageGenVertexBuffer );
	
	//GGTerrainInitTest();

	ggterrain_local_params.lod_levels = 0; // force parameter update on next cycle

	GGTerrain_WindowResized();

	for( int y = ((int)physPagesY)-1; y >= 0; y-- )
	{
		for( int x = ((int)physPagesX)-1; x >= 0; x-- )
		{
			int index = y * physPagesX + x;
			PageEntry* pPage = &pageMemory[ index ];

			pPage->SetPhysLocation( x, y );
			pPage->Setup( 0 );
			if ( index != 0 ) pagesFree.PushItem( pPage ); // page 0 is used as invalid page
		}
	}

	#ifdef GGTERRAIN_UNDOREDO
	undosys_terrain_init(GGTerrain_GetSculptDataSize(),  GGTERRAIN_HEIGHTMAP_EDIT_SIZE);
	#endif

	ggprobe_initialised = 1;

	// global probe used to have at least some kind of correct env map for places where local probes not extending
	float globalrange = 50000;
	globalEnvProbePos = XMFLOAT3( 0, ggterrain_local_params.height, 0 );
	globalEnvProbe = wiScene::GetScene().Entity_CreateEnvironmentProbe("globalEnvProbe", globalEnvProbePos);
	EnvironmentProbeComponent* probe = wiScene::GetScene().probes.GetComponent(globalEnvProbe);
	probe->range = globalrange;
	probe->userdata = 255;
	probe->SetDirty();
	wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(globalEnvProbe);
	pTransform->ClearTransform();
	pTransform->Translate(globalEnvProbePos);
	pTransform->Scale(XMFLOAT3(globalrange, globalrange, globalrange));
	pTransform->UpdateTransform();
	pTransform->SetDirty();

	// local env probe creation
	float range = 1;
	for( int i = 0; i < LOCALENVPROBECOUNT; i++ )
	{
		range = 1;
		localEnvProbePos[ i ] = XMFLOAT3( 0, 0, 0 );
		char strName[ 64 ];
		sprintf_s( strName, "localEnvProbe%d", i ); 
		localEnvProbe[ i ] = wiScene::GetScene().Entity_CreateEnvironmentProbe(strName, localEnvProbePos[i]);
		probe = wiScene::GetScene().probes.GetComponent(localEnvProbe[i]);
		probe->range = range; // env cube range
		probe->userdata = 0; // 0-255 alpha
		probe->SetDirty();
		pTransform = wiScene::GetScene().transforms.GetComponent(localEnvProbe[i]);
		pTransform->ClearTransform();
		pTransform->Translate(localEnvProbePos[i]);
		pTransform->Scale(XMFLOAT3(range, range, range));
		pTransform->UpdateTransform();
		pTransform->SetDirty();
	}

	// current probe for player to use
	currLocalEnvProbe = 0;
	
	// complete
	return 1;
}

void GGTerrain_RecreateReadbackResources(ID3D11Device* dev)
{
	assert(dev != nullptr);

	// --- invalidate state FIRST ---
	g_lastQueuedReadbackIndex.store(READBACK_INVALID, std::memory_order_release);

	// --- destroy queries ---
	for (uint32_t i = 0; i < NUM_READ_BACK_TEXTURES; ++i)
	{
		if (g_readbackTimestampQuery[i])
		{
			g_readbackTimestampQuery[i]->Release();
			g_readbackTimestampQuery[i] = nullptr;
		}
	}

	if (g_readbackDisjointQuery)
	{
		g_readbackDisjointQuery->Release();
		g_readbackDisjointQuery = nullptr;
	}

	// --- recreate timestamp queries ---
	for (uint32_t i = 0; i < NUM_READ_BACK_TEXTURES; ++i)
	{
		D3D11_QUERY_DESC qd = {};
		qd.Query = D3D11_QUERY_TIMESTAMP;

		HRESULT hr = dev->CreateQuery(&qd, &g_readbackTimestampQuery[i]);
		assert(SUCCEEDED(hr) && g_readbackTimestampQuery[i] != nullptr);
	}

	// --- recreate disjoint query ---
	D3D11_QUERY_DESC dj = {};
	dj.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;

	HRESULT hr = dev->CreateQuery(&dj, &g_readbackDisjointQuery);
	assert(SUCCEEDED(hr) && g_readbackDisjointQuery != nullptr);
}

void GGTerrain_WindowResized()
{
	uint32_t screenWidth = master.masterrenderer.GetWidth3D();
	uint32_t screenHeight = master.masterrenderer.GetHeight3D();

	uint32_t renderWidth = screenWidth / ggterrain_local_render_params2.readBackTextureReduction;
	uint32_t renderHeight = screenHeight / ggterrain_local_render_params2.readBackTextureReduction;

	GGTerrain_CreateComputeTexture( renderWidth, renderHeight, FORMAT_R32_UINT, &texReadBackCompute );

	for( uint32_t i = 0; i < NUM_READ_BACK_TEXTURES; i++ )
	{
		GGTerrain_CreateCPUReadTexture( renderWidth, renderHeight, FORMAT_R32_UINT, texReadBackStaging + i );
	}

	pageGenerationList.Clear();
	g_currReadBackTex = 0;
	g_lastReadBackWrittenIndex.store(READBACK_INVALID, std::memory_order_release);

	// After recreating texReadBackCompute and texReadBackStaging[]
	ID3D11Device* d3dDevice = (ID3D11Device*)wiRenderer::GetDevice()->GetDeviceForIMGUI();
	GGTerrain_RecreateReadbackResources(d3dDevice);
}

void GGTerrain_DrawPages( CommandList cmd )
{
	if ( !ggterrain_initialised ) return;

	GraphicsDevice* device = wiRenderer::GetDevice();
	auto range = wiProfiler::BeginRangeGPU( "Terrain Page Generation", cmd );
	device->EventBegin("Terrain Page Generation", cmd);

	uint32_t numPages = pageGenerationList.NumItems();
	if ( numPages == 0 ) 
	{
		device->EventEnd( cmd );
		wiProfiler::EndRange( range );
		return;
	}
	assert( numPages <= GGTERRAIN_REPLACEMENT_PAGE_MAX );

	float LODSegments = ggterrain_local_params.segments_per_chunk * 8.0f;

	GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
	uint32_t numLODLevels = pCurrLODs->GetNumLevels();

	// for 2K textures with detailLimit=3 tilingPower around 0.63 works well
	// for 2K textures with detailLimit=5 tilingPower around 0.45 works well
	uint32_t detailLimit = 4;
	float tilingPower = ggterrain_local_render_params.tilingPower;
	float textureTiling = 16.0f / (float) pow( tilingPower, detailLimit );

	// render mip level 0
	for( uint32_t i = 0; i < numPages; i++ )
	{
		PageEntry* pPage = pageGenerationList[ i ];

		uint32_t detailLevel = pPage->GetDetailLevel();
		uint32_t tilingDetail = detailLevel;
		if ( tilingDetail < detailLimit ) tilingDetail = detailLimit;
		float tilingScale = (float) pow( tilingPower, tilingDetail );
		float pageTiling = textureTiling * tilingScale / 2048.0f;

		// physcial position
		uint32_t offsetX = pPage->GetPhysOffsetX();
		uint32_t offsetY = pPage->GetPhysOffsetY();

		float scaleX = 1.0f / physPagesX;
		float scaleY = 1.0f / physPagesY;

		float left = offsetX * scaleX;
		float right = (offsetX + 1) * scaleX;
		float top = offsetY * scaleY;
		float bottom = (offsetY + 1) * scaleY;

		// virtual position
		uint32_t virtX = pPage->GetVirtOffsetX();
		uint32_t virtY = pPage->GetVirtOffsetY();

		uint32_t mipLevel = ggterrain.ConvertToMipLevel( detailLevel );
		float mipSize = (float) (256 >> mipLevel);

		if ( detailLevel >= numLODLevels ) detailLevel = numLODLevels - 1;

		float LODSize = LODSegments * pCurrLODs->pLevels[ detailLevel ].segSize;
		float realSize = LODSize / mipSize;
		float padding2 = realSize * paddingScale;

		float realX = (virtX / mipSize) * LODSize + pCurrLODs->pLevels[ detailLevel ].centerX - (LODSize / 2);
		float realY = (1 - (virtY / mipSize)) * LODSize + pCurrLODs->pLevels[ detailLevel ].centerZ - (LODSize / 2);
		float realX2 = realX + realSize + padding2;
		float realY2 = realY - realSize - padding2;
		realX -= padding2;
		realY += padding2;
						
		float u1 = realX * pageTiling;
		float u2 = realX2 * pageTiling;
		float v1 = -realY * pageTiling;
		float v2 = -realY2 * pageTiling;

		int heightLevel = detailLevel;
		if ( heightLevel > 5 ) heightLevel = 5;
		if ( heightLevel > (int)numLODLevels-1 ) heightLevel = numLODLevels - 1;
		
		float heightU1, heightU2, heightV1, heightV2;
		float heightScale = 513.0f / 512.0f;

		heightLevel--;
		do
		{
			heightLevel++;
			float size = LODSegments * pCurrLODs->pLevels[ heightLevel ].segSize;
			float centerX = pCurrLODs->pLevels[ heightLevel ].centerX;
			float centerZ = pCurrLODs->pLevels[ heightLevel ].centerZ;
			heightU1 = (realX - centerX) / size + 0.5f;
			heightU2 = (realX2 - centerX) / size + 0.5f;
			heightV1 = 0.5f - (realY - centerZ) / size;
			heightV2 = 0.5f - (realY2 - centerZ) / size;

			heightU1 *= heightScale;
			heightU2 *= heightScale;
			heightV1 *= heightScale;
			heightV2 *= heightScale;
		} while( (heightU1 < 0.01 || heightU2 > 0.99 || heightV1 < 0.01 || heightV2 > 0.99) && heightLevel < (int)numLODLevels-1 );

		g_VerticesPageGen[ i ][ 0 ].x = left;
		g_VerticesPageGen[ i ][ 0 ].y = top;
		g_VerticesPageGen[ i ][ 0 ].u = u1;
		g_VerticesPageGen[ i ][ 0 ].v = v1;
		g_VerticesPageGen[ i ][ 0 ].u2 = heightU1;
		g_VerticesPageGen[ i ][ 0 ].v2 = heightV1;
		g_VerticesPageGen[ i ][ 0 ].worldX = realX;
		g_VerticesPageGen[ i ][ 0 ].worldY = realY;
		g_VerticesPageGen[ i ][ 0 ].id = heightLevel | (detailLevel << 4);

		g_VerticesPageGen[ i ][ 1 ].x = left;
		g_VerticesPageGen[ i ][ 1 ].y = bottom;
		g_VerticesPageGen[ i ][ 1 ].u = u1;
		g_VerticesPageGen[ i ][ 1 ].v = v2;
		g_VerticesPageGen[ i ][ 1 ].u2 = heightU1;
		g_VerticesPageGen[ i ][ 1 ].v2 = heightV2;
		g_VerticesPageGen[ i ][ 1 ].worldX = realX;
		g_VerticesPageGen[ i ][ 1 ].worldY = realY2;
		g_VerticesPageGen[ i ][ 1 ].id = heightLevel | (detailLevel << 4);

		g_VerticesPageGen[ i ][ 2 ].x = right;
		g_VerticesPageGen[ i ][ 2 ].y = top;
		g_VerticesPageGen[ i ][ 2 ].u = u2;
		g_VerticesPageGen[ i ][ 2 ].v = v1;
		g_VerticesPageGen[ i ][ 2 ].u2 = heightU2;
		g_VerticesPageGen[ i ][ 2 ].v2 = heightV1;
		g_VerticesPageGen[ i ][ 2 ].worldX = realX2;
		g_VerticesPageGen[ i ][ 2 ].worldY = realY;
		g_VerticesPageGen[ i ][ 2 ].id = heightLevel | (detailLevel << 4);

		g_VerticesPageGen[ i ][ 3 ].x = right;
		g_VerticesPageGen[ i ][ 3 ].y = top;
		g_VerticesPageGen[ i ][ 3 ].u = u2;
		g_VerticesPageGen[ i ][ 3 ].v = v1;
		g_VerticesPageGen[ i ][ 3 ].u2 = heightU2;
		g_VerticesPageGen[ i ][ 3 ].v2 = heightV1;
		g_VerticesPageGen[ i ][ 3 ].worldX = realX2;
		g_VerticesPageGen[ i ][ 3 ].worldY = realY;
		g_VerticesPageGen[ i ][ 3 ].id = heightLevel | (detailLevel << 4);

		g_VerticesPageGen[ i ][ 4 ].x = left;
		g_VerticesPageGen[ i ][ 4 ].y = bottom;
		g_VerticesPageGen[ i ][ 4 ].u = u1;
		g_VerticesPageGen[ i ][ 4 ].v = v2;
		g_VerticesPageGen[ i ][ 4 ].u2 = heightU1;
		g_VerticesPageGen[ i ][ 4 ].v2 = heightV2;
		g_VerticesPageGen[ i ][ 4 ].worldX = realX;
		g_VerticesPageGen[ i ][ 4 ].worldY = realY2;
		g_VerticesPageGen[ i ][ 4 ].id = heightLevel | (detailLevel << 4);

		g_VerticesPageGen[ i ][ 5 ].x = right;
		g_VerticesPageGen[ i ][ 5 ].y = bottom;
		g_VerticesPageGen[ i ][ 5 ].u = u2;
		g_VerticesPageGen[ i ][ 5 ].v = v2;
		g_VerticesPageGen[ i ][ 5 ].u2 = heightU2;
		g_VerticesPageGen[ i ][ 5 ].v2 = heightV2;
		g_VerticesPageGen[ i ][ 5 ].worldX = realX2;
		g_VerticesPageGen[ i ][ 5 ].worldY = realY2;
		g_VerticesPageGen[ i ][ 5 ].id = heightLevel | (detailLevel << 4);
	}

	device->UpdateBuffer( &pageGenVertexBuffer, g_VerticesPageGen, cmd, numPages*sizeof(VertexPageGen)*6 );

	device->RenderPassBegin( &renderPassPhysicalTex, cmd );

	Viewport vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (float) renderPassPhysicalTex.GetDesc().attachments[0].texture->GetDesc().Width;
	vp.Height = (float) renderPassPhysicalTex.GetDesc().attachments[0].texture->GetDesc().Height;
	device->BindViewports( 1, &vp, cmd );

	device->BindPipelineState( &psoPageGen, cmd );
	
	uint32_t bindSlot = 2;
	device->BindConstantBuffer( VS, &terrainConstantBuffer, bindSlot, cmd );
	device->BindConstantBuffer( PS, &terrainConstantBuffer, bindSlot, cmd );
	
	const GPUBuffer* vbs[] = { &pageGenVertexBuffer };
	uint32_t stride = sizeof( VertexPageGen );
	device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );
	
	device->BindResource( PS, &texColorArray, 50, cmd );
	device->BindResource( PS, &texNormalsArray, 51, cmd );
#ifdef GGTERRAIN_USE_SURFACE_TEXTURE
	device->BindResource( PS, &texSurfaceArray, 52, cmd );
#else
	device->BindResource( PS, &texRoughnessMetalnessArray, 52, cmd );
	device->BindResource( PS, &texAOArray, 53, cmd );
#endif
	device->BindResource( PS, &texLODHeightMapArray, 54, cmd );
	device->BindResource( PS, &texLODNormalMapArray, 55, cmd );
	device->BindResource( PS, &texMask, 56, cmd );
	device->BindResource( PS, &texMaterialMap, 57, cmd );
	GGGrass_BindGrassArray( 58, cmd );

	device->BindSampler( PS, &samplerTrilinearWrap, 0, cmd );
	device->BindSampler( PS, &samplerBilinear, 1, cmd );
	device->BindSampler( PS, &samplerBilinearWrap, 2, cmd );
	device->BindSampler( PS, &samplerPoint, 3, cmd );

	device->Draw( 6*numPages, 0, cmd );

	device->RenderPassEnd( cmd );

	// render mip level 1
	for( uint32_t i = 0; i < numPages; i++ )
	{
		PageEntry* pPage = pageGenerationList[ i ];

		uint32_t detailLevel = pPage->GetDetailLevel();
		uint32_t tilingDetail = detailLevel + 1;
		if ( detailLevel < detailLimit ) tilingDetail = detailLimit;
		float tilingScale = (float) pow( tilingPower, tilingDetail );
		float pageTiling = textureTiling * tilingScale / 2048.0f;
		
		// virtual position
		uint32_t virtX = pPage->GetVirtOffsetX();
		uint32_t virtY = pPage->GetVirtOffsetY();

		uint32_t mipLevel = ggterrain.ConvertToMipLevel( detailLevel );
		uint32_t mipLevel2 = ggterrain.ConvertToMipLevel( detailLevel + 1 );
		float mipSize = (float) (256 >> mipLevel);
		float mipSize2 = (float) (256 >> mipLevel2);

		if ( detailLevel >= pCurrLODs->GetNumLevels() ) detailLevel = pCurrLODs->GetNumLevels() - 1;

		float LODSize = LODSegments * pCurrLODs->pLevels[ detailLevel ].segSize;
		float realSize = (LODSize / mipSize);
		float padding2 = realSize * paddingScale;
		float realX = (virtX / mipSize) * LODSize + pCurrLODs->pLevels[ detailLevel ].centerX - (LODSize / 2);
		float realY = ((mipSize-virtY) / mipSize) * LODSize + pCurrLODs->pLevels[ detailLevel ].centerZ - (LODSize / 2);
		float realX2 = realX + realSize + padding2;
		float realY2 = realY - realSize - padding2;
		realX -= padding2;
		realY += padding2;
								
		float u1 = realX * pageTiling;
		float u2 = realX2 * pageTiling;
		float v1 = -realY * pageTiling;
		float v2 = -realY2 * pageTiling;

		int heightLevel = detailLevel + 1;
		if ( heightLevel > 5 ) heightLevel = 5;
		if ( heightLevel > (int)numLODLevels-1 ) heightLevel = numLODLevels - 1;

		float heightU1, heightU2, heightV1, heightV2;
		float heightScale = 513.0f / 512.0f;

		heightLevel--;
		do
		{
			heightLevel++;
			float size = LODSegments * pCurrLODs->pLevels[ heightLevel ].segSize;
			float centerX = pCurrLODs->pLevels[ heightLevel ].centerX;
			float centerZ = pCurrLODs->pLevels[ heightLevel ].centerZ;
			heightU1 = (realX - centerX) / size + 0.5f;
			heightU2 = (realX2 - centerX) / size + 0.5f;
			heightV1 = 0.5f - (realY - centerZ) / size;
			heightV2 = 0.5f - (realY2 - centerZ) / size;

			heightU1 *= heightScale;
			heightU2 *= heightScale;
			heightV1 *= heightScale;
			heightV2 *= heightScale;
		} while( (heightU1 < 0.01 || heightU2 > 0.99 || heightV1 < 0.01 || heightV2 > 0.99) && heightLevel < (int)numLODLevels-1 );

		uint32_t mipDetailLevel = detailLevel + 1;
		g_VerticesPageGen[ i ][ 0 ].u = u1;
		g_VerticesPageGen[ i ][ 0 ].v = v1;
		g_VerticesPageGen[ i ][ 0 ].u2 = heightU1;
		g_VerticesPageGen[ i ][ 0 ].v2 = heightV1;
		g_VerticesPageGen[ i ][ 0 ].worldX = realX;
		g_VerticesPageGen[ i ][ 0 ].worldY = realY;
		g_VerticesPageGen[ i ][ 0 ].id = heightLevel | (mipDetailLevel << 4);

		g_VerticesPageGen[ i ][ 1 ].u = u1;
		g_VerticesPageGen[ i ][ 1 ].v = v2;
		g_VerticesPageGen[ i ][ 1 ].u2 = heightU1;
		g_VerticesPageGen[ i ][ 1 ].v2 = heightV2;
		g_VerticesPageGen[ i ][ 1 ].worldX = realX;
		g_VerticesPageGen[ i ][ 1 ].worldY = realY2;
		g_VerticesPageGen[ i ][ 1 ].id = heightLevel | (mipDetailLevel << 4);

		g_VerticesPageGen[ i ][ 2 ].u = u2;
		g_VerticesPageGen[ i ][ 2 ].v = v1;
		g_VerticesPageGen[ i ][ 2 ].u2 = heightU2;
		g_VerticesPageGen[ i ][ 2 ].v2 = heightV1;
		g_VerticesPageGen[ i ][ 2 ].worldX = realX2;
		g_VerticesPageGen[ i ][ 2 ].worldY = realY;
		g_VerticesPageGen[ i ][ 2 ].id = heightLevel | (mipDetailLevel << 4);

		g_VerticesPageGen[ i ][ 3 ].u = u2;
		g_VerticesPageGen[ i ][ 3 ].v = v1;
		g_VerticesPageGen[ i ][ 3 ].u2 = heightU2;
		g_VerticesPageGen[ i ][ 3 ].v2 = heightV1;
		g_VerticesPageGen[ i ][ 3 ].worldX = realX2;
		g_VerticesPageGen[ i ][ 3 ].worldY = realY;
		g_VerticesPageGen[ i ][ 3 ].id = heightLevel | (mipDetailLevel << 4);

		g_VerticesPageGen[ i ][ 4 ].u = u1;
		g_VerticesPageGen[ i ][ 4 ].v = v2;
		g_VerticesPageGen[ i ][ 4 ].u2 = heightU1;
		g_VerticesPageGen[ i ][ 4 ].v2 = heightV2;
		g_VerticesPageGen[ i ][ 4 ].worldX = realX;
		g_VerticesPageGen[ i ][ 4 ].worldY = realY2;
		g_VerticesPageGen[ i ][ 4 ].id = heightLevel | (mipDetailLevel << 4);

		g_VerticesPageGen[ i ][ 5 ].u = u2;
		g_VerticesPageGen[ i ][ 5 ].v = v2;
		g_VerticesPageGen[ i ][ 5 ].u2 = heightU2;
		g_VerticesPageGen[ i ][ 5 ].v2 = heightV2;
		g_VerticesPageGen[ i ][ 5 ].worldX = realX2;
		g_VerticesPageGen[ i ][ 5 ].worldY = realY2;
		g_VerticesPageGen[ i ][ 5 ].id = heightLevel | (mipDetailLevel << 4);
	}

	device->UpdateBuffer( &pageGenVertexBuffer, g_VerticesPageGen, cmd, numPages*sizeof(VertexPageGen)*6 );

	device->RenderPassBegin( &renderPassPhysicalTexMip, cmd );

	vp.Width /= 2;
	vp.Height /= 2;
	device->BindViewports( 1, &vp, cmd );

	device->BindPipelineState( &psoPageGen, cmd );
			
	device->Draw( 6*numPages, 0, cmd );

	device->RenderPassEnd( cmd );

	device->EventEnd( cmd );
	wiProfiler::EndRange( range );

	pageGenerationList.Clear();
}

int GGTerrain_GeneratePage( PageEntry* pPage )
{
	if ( pageGenerationList.NumItems() >= GGTERRAIN_REPLACEMENT_PAGE_MAX ) return 0;

	GraphicsDevice* device = wiRenderer::GetDevice();

	// update page table texture
	uint32_t detailLevel = pPage->GetDetailLevel(); // detailLevel 0 is the highest detail level
			
	uint32_t virtOffsetX = pPage->GetVirtOffsetX();
	uint32_t virtOffsetY = pPage->GetVirtOffsetY();
			
	uint32_t physOffsetX = pPage->GetPhysOffsetX();
	uint32_t physOffsetY = pPage->GetPhysOffsetY();
			
	uint16_t pixel = (physOffsetY << 8) | physOffsetX;
			
	CopyBox dstBox;
	dstBox.left = virtOffsetX; dstBox.right = virtOffsetX + 1;
	dstBox.top = virtOffsetY; dstBox.bottom = virtOffsetY + 1;
	dstBox.front = 0; dstBox.back = 1;
			
	if ( ggterrain.IsHighDetail( detailLevel ) )
	{
		pageTableData[ detailLevel ][ virtOffsetY * pagesX + virtOffsetX ] = pixel;
		device->UpdateTexture( &texPageTableArray, 0, detailLevel, &dstBox, &pixel, 0, -1 ); 
	}
	else
	{
		uint32_t mipLevel = ggterrain.ConvertToMipLevel( detailLevel );
		pageTableData[ detailLevel ][ virtOffsetY * (pagesX >> mipLevel) + virtOffsetX ] = pixel;
		device->UpdateTexture( &texPageTableFinal, mipLevel, 0, &dstBox, &pixel, 0, -1 ); 
	}
			
	pageGenerationList.AddItem( pPage );
	return 1;
}

void GGTerrain_CheckPageShift()
{
	GraphicsDevice* device = wiRenderer::GetDevice();

	GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
	uint32_t numLODLevels = pCurrLODs->GetNumLevels();

	if ( ggterrain.ShouldRegeneratePages() )
	{
		pageGenerationList.Clear();
		g_currReadBackTex = 0;
		g_lastReadBackWrittenIndex.store(READBACK_INVALID, std::memory_order_release);
		ID3D11Device* d3dDevice = (ID3D11Device*)wiRenderer::GetDevice()->GetDeviceForIMGUI();
		GGTerrain_RecreateReadbackResources(d3dDevice);

		// wipe out page memory and start again
		pagesFree.Clear();
		pageRefreshList.Clear();
		for( int y = ((int)physPagesY)-1; y >= 0; y-- )
		{
			for( int x = ((int)physPagesX)-1; x >= 0; x-- )
			{
				int index = y * physPagesX + x;
				PageEntry* pPage = &pageMemory[ index ];
				pPage->Setup( 0 );
				if ( index != 0 ) pagesFree.PushItem( pPage ); // page 0 is used as invalid page
			}
		}

		// page table array texture, one level per LOD, except the last LOD level which is mipmapped
		GGTerrain_CreateEmptyTexture( pagesX, pagesY, 1, numLODLevels-1, FORMAT_R16_UNORM, &texPageTableArray );

		// clear page tables
		memset( pageTableData, 0, GGTERRAIN_PAGE_TABLE_DEPTH*pagesX*pagesY*sizeof(uint16_t) ); // default everything to 0 (invalid page)
		uint32_t stride = pagesX * sizeof(uint16_t); 
		for( uint32_t level = 0; level < numLODLevels-1; level++ )
		{
			device->UpdateTexture( &texPageTableArray, 0, level, 0, pageTableData, stride, -1 );
		}

		for( uint32_t mipLevel = 0; mipLevel < GGTERRAIN_MAX_PAGE_TABLE_MIP; mipLevel++ ) // not a full mip stack
		{
			device->UpdateTexture( &texPageTableFinal, mipLevel, 0, 0, pageTableData, stride, -1 );
			stride /= 2;
		}
		
		// generate commonly needed physical texture pages, at least the 16 4x4 level pages must be generated
		uint32_t detailLevel = (numLODLevels - 1) + GGTERRAIN_MAX_PAGE_TABLE_MIP - 1; // lowest detail level
		for( int y = 0; y < 4; y++ )
		{
			for( int x = 0; x < 4; x++ )
			{
				PageEntry* pPage = pagesFree.PopItem();
				assert( pPage );

				uint32_t identifier = ((detailLevel + 1) << 16) | (y << 8) | x;
				pPage->Setup( identifier );
				GGTerrain_GeneratePage( pPage );
			}
		}
	}
	else if ( ggterrain.ShouldShiftPages() )
	{
		pageGenerationList.Clear();
		g_currReadBackTex = 0;
		g_lastReadBackWrittenIndex.store(READBACK_INVALID, std::memory_order_release);
		ID3D11Device* d3dDevice = (ID3D11Device*)wiRenderer::GetDevice()->GetDeviceForIMGUI();
		GGTerrain_RecreateReadbackResources(d3dDevice);

		// shift page memory
		for( uint32_t y = 0; y < physPagesY; y++ )
		{
			uint32_t index = y * physPagesX;
			for( uint32_t x = 0; x < physPagesX; x++ )
			{
				PageEntry* pPage = &pageMemory[ index++ ];
				if ( pPage->identifier == 0 ) continue; // empty page

				int detailLevel = pPage->GetDetailLevel(); // level 0 is highest detail
				int virtX = pPage->GetVirtOffsetX();
				int virtY = pPage->GetVirtOffsetY();
				uint32_t mipSize = 256;

				if ( ggterrain.IsHighDetail( detailLevel ) )
				{
					int shiftX = pCurrLODs->pLevels[ detailLevel ].shiftedX;
					int shiftY = pCurrLODs->pLevels[ detailLevel ].shiftedZ;
					int modifier = 256 / 8; // page table size divided by number chunks per LOD
					shiftX *= modifier;
					shiftY *= modifier;

					virtX -= shiftX;
					virtY += shiftY;
				}
				else
				{
					uint32_t mipLevel = ggterrain.ConvertToMipLevel( detailLevel );
					uint32_t maxLODLevel = numLODLevels - 1;

					int shiftX = pCurrLODs->pLevels[ maxLODLevel ].shiftedX;
					int shiftY = pCurrLODs->pLevels[ maxLODLevel ].shiftedZ;
					mipSize >>= mipLevel;
					assert(mipSize >= 4);

					if ( mipSize < 8 )
					{
						// assumes shift is a multiple of 2
						assert((shiftX % 2) == 0);
						assert((shiftY % 2) == 0);
						shiftX /= 2;
						shiftY /= 2;
					}
					else
					{
						int modifier = mipSize / 8; // page table size divided by number chunks per LOD
						shiftX *= modifier;
						shiftY *= modifier;
					}

					virtX -= shiftX;
					virtY += shiftY;
				}

				if ( virtX < 0 || virtY < 0 || virtX >= (int)mipSize || virtY >= (int)mipSize )
				{
					// clear page
					pPage->Setup( 0 );
					pagesFree.PushItem( pPage );
				}
				else
				{
					// shift page
					pPage->SetVirtLocation( virtX, virtY );
				}
			}
		}

		// shift page tables
		uint16_t* newPageData = new uint16_t[ pagesX * pagesY ];

		for( uint32_t level = 0; level < numLODLevels-1; level++ )
		{
			int shiftX = pCurrLODs->pLevels[ level ].shiftedX;
			int shiftY = pCurrLODs->pLevels[ level ].shiftedZ;
			if ( shiftX == 0 && shiftY == 0 ) continue;

			int modifier = 256 / 8; // page table size divided by number chunks per LOD
			shiftX *= modifier;
			shiftY *= modifier;

			for( uint32_t y = 0; y < pagesY; y++ )
			{
				uint32_t newIndex = y * pagesX;
				int oldY = y;
				oldY -= shiftY;
				if ( oldY < 0 || oldY >= pagesY )
				{
					memset( newPageData + newIndex, 0, pagesX*sizeof(uint16_t) );
				}
				else
				{
					int oldIndex = oldY * pagesX + shiftX;
					for( uint32_t x = 0; x < pagesX; x++ )
					{
						int oldX = x; oldX += shiftX;

						if ( oldX < 0 || oldX >= pagesX ) newPageData[ newIndex ] = 0;
						else newPageData[ newIndex ] = pageTableData[ level ][ oldIndex ];

						oldIndex++;
						newIndex++;
					}
				}
			}

			memcpy( &pageTableData[ level ], newPageData, pagesX*pagesY*sizeof(uint16_t) );
			uint32_t stride = pagesX * sizeof(uint16_t);
			device->UpdateTexture( &texPageTableArray, 0, level, 0, &pageTableData[ level ], stride, -1 );
		}

		// shift final mipmapped page table texture
		int shiftX = pCurrLODs->pLevels[ numLODLevels-1 ].shiftedX;
		int shiftY = pCurrLODs->pLevels[ numLODLevels-1 ].shiftedZ;
		if ( shiftX != 0 || shiftY != 0 ) 
		{
			assert((shiftX % 2) == 0);
			assert((shiftY % 2) == 0);
			
			for( uint32_t level = 0; level < GGTERRAIN_MAX_PAGE_TABLE_MIP; level++ )
			{
				int mippedShiftX = shiftX;
				int mippedShiftY = shiftY;
				uint32_t mipSize = 256 >> level;

				if ( mipSize < 8 )
				{
					// assumes shift is a multiple of 2
					mippedShiftX /= 2;
					mippedShiftY /= 2;
				}
				else
				{
					int modifier = mipSize / 8; // page table size divided by number chunks per LOD
					mippedShiftX *= modifier;
					mippedShiftY *= modifier;
				}

				uint32_t pageLevel = level + numLODLevels - 1;
				int mippedPagesX = pagesX >> level;
				int mippedPagesY = pagesY >> level;

				for( uint32_t y = 0; y < (uint32_t)mippedPagesY; y++ )
				{
					uint32_t newIndex = y * mippedPagesX;
					int oldY = y;
					oldY -= mippedShiftY;
					if ( oldY < 0 || oldY >= mippedPagesY )
					{
						memset( newPageData + newIndex, 0, mippedPagesX*sizeof(uint16_t) );
					}
					else
					{
						int oldIndex = oldY * mippedPagesX + mippedShiftX;
						for( uint32_t x = 0; x < (uint32_t)mippedPagesX; x++ )
						{
							int oldX = x; 
							oldX += mippedShiftX;

							if ( oldX < 0 || oldX >= mippedPagesX ) newPageData[ newIndex ] = 0;
							else newPageData[ newIndex ] = pageTableData[ pageLevel ][ oldIndex ];

							oldIndex++;
							newIndex++;
						}
					}
				}

				memcpy( &pageTableData[ pageLevel ], newPageData, mippedPagesX*mippedPagesY*sizeof(uint16_t) );
				uint32_t stride = mippedPagesX * sizeof(uint16_t);
				device->UpdateTexture( &texPageTableFinal, level, 0, 0, &pageTableData[ pageLevel ], stride, -1 );
			}
						
			// generate any new 4x4 level pages
			uint32_t detailLevel = (numLODLevels - 1) + GGTERRAIN_MAX_PAGE_TABLE_MIP - 1; // lowest detail level
			for( int y = 0; y < 4; y++ )
			{
				for( int x = 0; x < 4; x++ )
				{
					uint32_t index = y * 4 + x;
					if ( pageTableData[ detailLevel ][ index ] ) continue;

					PageEntry* pPage = pagesFree.PopItem();
					assert( pPage );

					uint32_t identifier = ((detailLevel + 1) << 16) | (y << 8) | x;
					pPage->Setup( identifier );
					int result = GGTerrain_GeneratePage( pPage );
					assert( result );
				}
			}
		}

		delete [] newPageData;
	}

	ggterrain.PagesUpdated();
}

void GGTerrain_CheckReadBack(wiGraphics::CommandList cmd)
{
	GraphicsDevice* device = wiRenderer::GetDevice();

	GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
	uint32_t numLODLevels = pCurrLODs->GetNumLevels();
		
	if ( true )
	{
		uint32_t texWidth = texReadBackCompute.GetDesc().Width;
		uint32_t texHeight = texReadBackCompute.GetDesc().Height;

		pagesNeeded.Setup( texWidth, texHeight );
		
		Mapping mapping;
		mapping._flags = Mapping::FLAG_READ;
		mapping.size = texWidth * texHeight * sizeof(uint32_t);

		ID3D11DeviceContext* g_d3dImmediateCtx = (ID3D11DeviceContext*)wiRenderer::GetDevice()->GetImmediateForIMGUI();
		if (!g_d3dImmediateCtx) return;

		uint32_t lastQueued = g_lastQueuedReadbackIndex.load(std::memory_order_acquire);
		if (lastQueued == READBACK_INVALID)
			return;
		lastQueued %= NUM_READ_BACK_TEXTURES;

		// 1) Ensure the disjoint bracket for this readback is complete:
		if (g_readbackDisjointQuery)
		{
			D3D11_QUERY_DATA_TIMESTAMP_DISJOINT dj = {};
			HRESULT hdr = g_d3dImmediateCtx->GetData(
				g_readbackDisjointQuery, &dj, sizeof(dj), D3D11_ASYNC_GETDATA_DONOTFLUSH
			);

			if (hdr != S_OK) return;       // not finished yet
			if (dj.Disjoint) return;       // GPU frequency changed; be strict and skip
		}

		// 2) Find most recent slot with a completed timestamp:
		int chosen = -1;
		for (uint32_t step = 0; step < NUM_READ_BACK_TEXTURES; ++step)
		{
			uint32_t idx = (lastQueued + NUM_READ_BACK_TEXTURES - step) % NUM_READ_BACK_TEXTURES;
			ID3D11Query* q = g_readbackTimestampQuery[idx];
			if (!q) continue;

			UINT64 ts = 0;
			HRESULT hr = g_d3dImmediateCtx->GetData(q, &ts, sizeof(ts), D3D11_ASYNC_GETDATA_DONOTFLUSH);
			if (hr == S_OK)
			{
				chosen = (int)idx;
				break;
			}
		}
		if (chosen < 0) return;

		auto rangeTotal = wiProfiler::BeginRangeCPU("Max - Terrain Read Back (All)");
		auto range = wiProfiler::BeginRangeCPU("Max - Terrain Read Back Collect");

		GG_CRASH_CONTEXT_NO_FLAG("GGTerrain_CheckReadBack Check", "frame=%u chosen=%u", wiRenderer::GetDevice()->GetFrameCount(), chosen);
		device->Map(&texReadBackStaging[chosen], &mapping);

		if ( !mapping.data )
		{
			wiProfiler::EndRange( range );
			wiProfiler::EndRange( rangeTotal );
			return;
		}

		uint32_t pitch = mapping.rowpitch / sizeof(uint32_t);
		
		#ifdef PEOPTIMIZING
		//PE: OPT1 No need to update all pages each frame. Moving camera from Lod to Lod takes many frames.
		//PE: This reduce the sort that are really the most slow part of the terrain system and done each frame.

		static uint32_t iFrameCount = 0;
		for (uint32_t y = 0; y < texHeight; y++)
		{
			uint32_t index = y * pitch;
			uint32_t* dataPtr = ((uint32_t*)mapping.data) + index;

			if ((y + iFrameCount) % 7 == 0)
			{
				for (uint32_t x = 0; x < texWidth; x++)
				{
					uint32_t value = *dataPtr;
					dataPtr++;

					// value format: [0-7]=virtLocationX, [8-15]=virtLocationY, [16-20]=mipLevel (mipLevel 1 is higest detail, 0 is invalid)
					if (value == 0) continue; // undrawn areas of texture
					pagesNeeded.AddNeededPage(value);
				}
			}
		}
		iFrameCount++;

#else
		for( uint32_t y = 0; y < texHeight; y++ )
		{
			uint32_t index = y * pitch;
			uint32_t* dataPtr = ((uint32_t*)mapping.data) + index;

			for( uint32_t x = 0; x < texWidth; x++ )
			{
				uint32_t value = *dataPtr;
				dataPtr++;

				// value format: [0-7]=virtLocationX, [8-15]=virtLocationY, [16-20]=mipLevel (mipLevel 1 is higest detail, 0 is invalid)
				if ( value == 0 ) continue; // undrawn areas of texture

				pagesNeeded.AddNeededPage( value );
			}
		}
#endif
		device->Unmap( &texReadBackStaging[chosen] );

		wiProfiler::EndRange( range );
		range = wiProfiler::BeginRangeCPU( "Max - Terrain Read Back Sort" );
		
		pagesNeeded.Sort();

		wiProfiler::EndRange( range );
		range = wiProfiler::BeginRangeCPU( "Max - Terrain Read Back Check" );

		// check loaded pages against the needed pages
		PageEntry* pEvictionCandiates[ GGTERRAIN_EVICTION_PAGE_MAX ] = {0};
		uint32_t evictionMin = 0;
		for( uint32_t y = 0; y < physPagesY; y++ )
		{
			uint32_t index = y * physPagesX;
			for( uint32_t x = 0; x < physPagesX; x++ )
			{
				PageEntry* pPage = &pageMemory[ index++ ];
				if ( pPage->identifier == 0 ) continue; // page is empty

				PageNeeded* pFoundPage = pagesNeeded.GetItem( pPage->identifier );
				if ( pFoundPage )
				{
					// active page is still needed
					pPage->unusedCount = 0;
					pPage->SetRequestedCount( pFoundPage->GetRequestedCount() );
					pFoundPage->SetResident();
				}
				else
				{
					// active page is not currently needed
					if ( pPage->unusedCount < 0xFFFF ) pPage->unusedCount++;
				}

				uint32_t maxLevel = numLODLevels - 1;
				maxLevel += (GGTERRAIN_MAX_PAGE_TABLE_MIP - 1);
				if ( pPage->GetDetailLevel() >= maxLevel ) continue; // never evict lowest LOD level pages
				
				// keep a list of the most unused pages
				if ( pagesFree.NumItems() < GGTERRAIN_EVICTION_PAGE_MAX && pPage->evictionCheck > evictionMin )
				{
					PageEntry* pReplacement = pPage;
					for( int i = 0; i < GGTERRAIN_EVICTION_PAGE_MAX; i++ )
					{
						if ( !pEvictionCandiates[ i ] )
						{
							pEvictionCandiates[ i ] = pReplacement;
							break;
						}

						if ( pEvictionCandiates[ i ]->evictionCheck < pReplacement->evictionCheck )
						{
							PageEntry* pTemp = pReplacement;
							pReplacement = pEvictionCandiates[ i ];
							pEvictionCandiates[ i ] = pTemp;
						}
					}

					if ( pEvictionCandiates[ GGTERRAIN_EVICTION_PAGE_MAX-1 ] != 0 )
					{
						evictionMin = pEvictionCandiates[ GGTERRAIN_EVICTION_PAGE_MAX-1 ]->evictionCheck;
					}
				}
			}
		}

		// lots of new pages will take time to generate, tell read back step to use lower detail next time (result will be delayed by a few frames)
		uint32_t numNewPages = pagesNeeded.GetNumNewPages();
		/*
		if ( numNewPages > 1000 ) terrainConstantData.detailLimit = 4;
		else if ( numNewPages > 650 ) terrainConstantData.detailLimit = 3;
		else if ( numNewPages > 400 ) terrainConstantData.detailLimit = 2;
		else if ( numNewPages > 200 ) terrainConstantData.detailLimit = 1;
		else terrainConstantData.detailLimit = 0;*/

		wiProfiler::EndRange( range );
		range = wiProfiler::BeginRangeCPU( "Max - Terrain Read Back Replace" );
		
		// select a number of new pages to load this frame
		PageNeeded* pNewPages[ GGTERRAIN_EVICTION_PAGE_MAX ] = { 0 };
		numNewPages = pagesNeeded.GetNewPages( pNewPages, GGTERRAIN_EVICTION_PAGE_MAX );
		
		uint32_t evictionIndex = 0;
		for( uint32_t i = 0; i < numNewPages; i++ )
		{
			if ( pageGenerationList.NumItems() >= GGTERRAIN_REPLACEMENT_PAGE_MAX ) break;

			PageNeeded* pNewPage = pNewPages[i];
			PageEntry* pPageEntry = pagesFree.PopItem();
			if ( !pPageEntry ) 
			{
				if ( evictionIndex >= GGTERRAIN_EVICTION_PAGE_MAX ) break;
				pPageEntry = pEvictionCandiates[ evictionIndex ];
				evictionIndex++;
				assert( pPageEntry );

				// if the replacement page is less requested than the evicted page then keep the evicted pages instead
				if ( pPageEntry->unusedCount == 0 && pPageEntry->GetRequestedCount() > pNewPage->GetRequestedCount() ) break;

				// delete old page table entry
				uint32_t detailLevel = pPageEntry->GetDetailLevel(); // detailLevel 0 is the highest detail level
				uint32_t virtOffsetX = pPageEntry->GetVirtOffsetX();
				uint32_t virtOffsetY = pPageEntry->GetVirtOffsetY();
				
				uint16_t pixel = 0; // 0 will cause the shader to loop down the mip levels until it finds a suitable page
				
				CopyBox dstBox;
				dstBox.left = virtOffsetX; dstBox.right = virtOffsetX + 1;
				dstBox.top = virtOffsetY; dstBox.bottom = virtOffsetY + 1;
				dstBox.front = 0; dstBox.back = 1;

				if ( ggterrain.IsHighDetail( detailLevel ) )
				{
					pageTableData[ detailLevel ][ virtOffsetY * pagesX + virtOffsetX ] = pixel;
					device->UpdateTexture( &texPageTableArray, 0, detailLevel, &dstBox, &pixel, 0, -1 ); 
				}
				else
				{
					uint32_t mipLevel = ggterrain.ConvertToMipLevel( detailLevel );
					pageTableData[ detailLevel ][ virtOffsetY * (pagesX >> mipLevel) + virtOffsetX ] = pixel;
					device->UpdateTexture( &texPageTableFinal, mipLevel, 0, &dstBox, &pixel, 0, -1 ); 
				}
			}

			// copy needed page into page table entry
			pPageEntry->Setup( pNewPage->identifier );
			pPageEntry->SetRequestedCount( pNewPage->GetRequestedCount() );

			int result = GGTerrain_GeneratePage( pPageEntry );
			assert( result );
		}

		wiProfiler::EndRange( range );
		wiProfiler::EndRange( rangeTotal );
	}
}

void GGTerrain_InvalidateRegion( float minX, float minZ, float maxX, float maxZ, uint32_t flags )
{
	if (!ggterrain_initialised) return;
	GGTerrainLODSet* pLODs = ggterrain.GetNewLODs();
	if ( !pLODs->IsGenerating() ) pLODs = ggterrain.GetCurrentLODs();

	float LODSegments = ggterrain_local_params.segments_per_chunk * 8.0f;
	uint32_t numLODLevels = pLODs->GetNumLevels();
	bool regenerate = false;
	for( uint32_t lodLevel = 0; lodLevel < numLODLevels; lodLevel++ )
	{
		GGTerrainLODLevel* pLevel = &pLODs->pLevels[ lodLevel ];
		const AABB* aabb = pLevel->GetBounds();
		if ( aabb->_min.x > maxX || aabb->_max.x < minX || aabb->_min.z > maxZ || aabb->_max.z < minZ ) continue;

		if ( flags & GGTERRAIN_INVALIDATE_CHUNKS )
		{
			// invalidate chunks
			for( int i = 0; i < 64; i++ )
			{
				GGTerrainChunk* pChunk = pLevel->chunkGrid[ i ];
				if ( !pChunk ) continue;
				aabb = pChunk->GetBounds();
				if ( aabb->_min.x > maxX || aabb->_max.x < minX || aabb->_min.z > maxZ || aabb->_max.z < minZ ) continue;

				// chunk needs to be regenerated
				pChunk->SetToRegenerate( 1 );
				pLevel->SetToRegenerate( 1 );
				regenerate = true;
			}
		}

		if ( flags & GGTERRAIN_INVALIDATE_TEXTURES )
		{
			// invalidate texture pages
			if ( lodLevel < numLODLevels-1 )
			{
				float LODSize = LODSegments * pLevel->segSize;
				float realSize = LODSize / 256.0f;
		
				for( uint32_t y = 0; y < 256; y++ )
				{
					for( uint32_t x = 0; x < 256; x++ )
					{
						float pageMinX = (x / 256.0f) * LODSize + pLevel->centerX - (LODSize / 2);
						float pageMaxZ = (1 - (y / 256.0f)) * LODSize + pLevel->centerZ - (LODSize / 2);
						float pageMaxX = pageMinX + realSize;
						float pageMinZ = pageMaxZ - realSize;
						if ( pageMinX > maxX || pageMaxX < minX || pageMinZ > maxZ || pageMaxZ < minZ ) continue;

						uint32_t pageIndex = y * 256 + x;
						uint16_t pageEntry = pageTableData[ lodLevel ][ pageIndex ];
						if ( pageEntry == 0 ) continue;

						uint16_t physOffsetX = pageEntry & 0xFF;
						uint16_t physOffsetY = pageEntry >> 8;

						int pageCacheIndex = physOffsetY * physPagesX + physOffsetX;
						PageEntry* pPage = &pageMemory[ pageCacheIndex ];
						if ( flags & GGTERRAIN_INVALIDATE_CHUNKS ) pPage->SetToRegenerate();
						else pPage->SetToRegenerateImmediate();
					}
				}
			}
			else
			{
				// final lod level
				float LODSize = LODSegments * pLevel->segSize;
			
				for( uint32_t mipLevel = 0; mipLevel < GGTERRAIN_MAX_PAGE_TABLE_MIP; mipLevel++ )
				{		
					uint32_t mipSize = 256 >> mipLevel;
					float fMipSize = (float) mipSize;
					float realSize = LODSize / fMipSize;
					for( uint32_t y = 0; y < mipSize; y++ )
					{
						for( uint32_t x = 0; x < mipSize; x++ )
						{
							float pageMinX = (x / fMipSize) * LODSize + pLevel->centerX - (LODSize / 2);
							float pageMaxZ = (1 - (y / fMipSize)) * LODSize + pLevel->centerZ - (LODSize / 2);
							float pageMaxX = pageMinX + realSize;
							float pageMinZ = pageMaxZ - realSize;
							if ( pageMinX > maxX || pageMaxX < minX || pageMinZ > maxZ || pageMaxZ < minZ ) continue;

							uint32_t pageIndex = y * mipSize + x;
							uint16_t pageEntry = pageTableData[ lodLevel + mipLevel ][ pageIndex ];
							if ( pageEntry == 0 ) continue;

							uint16_t physOffsetX = pageEntry & 0xFF;
							uint16_t physOffsetY = pageEntry >> 8;

							int pageCacheIndex = physOffsetY * physPagesX + physOffsetX;
							PageEntry* pPage = &pageMemory[ pageCacheIndex ];
							if ( flags & GGTERRAIN_INVALIDATE_CHUNKS ) pPage->SetToRegenerate();
							else pPage->SetToRegenerateImmediate();
						}
					}
				}
			}
		}
	}
	if (regenerate)
	{
		pLODs->SetToRegenerate(1);
		float fEditableSizeHalved = GGTerrain_GetEditableSize();
		float fScaleItDown = (fEditableSizeHalved*2.0f) / 20.0f; // 10000.0f / 20
		int iGridMinX = (int) ((fEditableSizeHalved + minX) / fScaleItDown); if (iGridMinX < 0) iGridMinX = 0;
		int iGridMaxX = (int) ((fEditableSizeHalved + maxX) / fScaleItDown); if (iGridMaxX > 20) iGridMaxX = 20;
		int iGridMinZ = (int) ((fEditableSizeHalved + minZ) / fScaleItDown); if (iGridMinZ < 0) iGridMinZ = 0;
		int iGridMaxZ = (int) ((fEditableSizeHalved + maxZ) / fScaleItDown); if (iGridMaxZ > 20) iGridMaxZ = 20;
		for (int z = iGridMinZ; z <= iGridMaxZ; z++)
		{
			for (int x = iGridMinX; x <= iGridMaxX; x++)
			{
				g_bModifiedThisTerrainGrid[x][z] = true;
			}
		}
	}
}

void GGTerrain_InvalidateEverything( uint32_t flags )
{
	GGTerrain_InvalidateRegion( -1e20f, -1e20f, 1e20f, 1e20f, flags );
}

void GGTerrain_Update_Sculpting( float pickX, float pickY, float pickZ )
{
#ifdef GGTERRAIN_ENABLE_SCULPTING
	if ( ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_NONE ) return;

	float fX = pickX / ggterrain_local_render_params2.editable_size;
	fX = fX * 0.5f + 0.5f;
	fX *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE;

	float fZ = pickZ / ggterrain_local_render_params2.editable_size;
	fZ = fZ * 0.5f + 0.5f;
	fZ *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE;

	if ( ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_RAMP )
	{
		if ( ggterrain_internal_params.mouseLeftPressed )
		{
			if ( !ggterrain_internal_params.sculpt_prev_ramp_valid )
			{
				ggterrain_internal_params.sculpt_prev_ramp_valid = true;
				ggterrain_internal_params.sculpt_prev_ramp_x = pickX;
				ggterrain_internal_params.sculpt_prev_ramp_z = pickZ;
				vLastRampTerrainPickPosition.x = pickX;
				vLastRampTerrainPickPosition.y = pickZ;
				vLastRampTerrainPickPosition.z = 0;
				vLastRampTerrainPickPosition.w = 0;
			}
			else 
			{
				#ifdef GGTERRAIN_UNDOREDO
				if (g_iCalculatingChangeBounds == 0)
				{
					g_EditBounds.minX = 50000;
					g_EditBounds.minY = 50000;
					g_EditBounds.maxX = 0;
					g_EditBounds.maxY = 0;
					// Take a snapshot of how the terrain is before any edits take place.
					GGTerrain_GetSculptData(g_pTerrainSnapshot);
					g_iCalculatingChangeBounds = 1;
				}
				#endif

				ggterrain_internal_params.sculpt_prev_ramp_valid = false;
				vLastRampTerrainPickPosition.z = pickX;
				vLastRampTerrainPickPosition.w = pickZ;

				float fStartX = ggterrain_internal_params.sculpt_prev_ramp_x / ggterrain_local_render_params2.editable_size;
				fStartX = fStartX * 0.5f + 0.5f;
				fStartX *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE;

				float fStartZ = ggterrain_internal_params.sculpt_prev_ramp_z / ggterrain_local_render_params2.editable_size;
				fStartZ = fStartZ * 0.5f + 0.5f;
				fStartZ *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE;

				float fStartY = 0;
				GGTerrain_GetHeight( ggterrain_internal_params.sculpt_prev_ramp_x, ggterrain_internal_params.sculpt_prev_ramp_z, &fStartY );

				float radius = ggterrain_local_render_params2.brushSize / ggterrain_local_render_params2.editable_size;
				radius *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE * 0.5f;

				float fEndX = fX;
				float fEndZ = fZ;

				float fEndY = 0;
				GGTerrain_GetHeight( pickX, pickZ, &fEndY );

				float rampVX = fEndX - fStartX;
				float rampVZ = fEndZ - fStartZ;

				float rampLength = sqrt( rampVX*rampVX + rampVZ*rampVZ );
				if ( rampLength > 0 )
				{
					rampVX /= rampLength;
					rampVZ /= rampLength;
				}

				float fMinX = fStartX;
				float fMaxX = fEndX;
				float fMinZ = fStartZ;
				float fMaxZ = fEndZ;

				if ( fMinX > fMaxX )
				{
					float temp = fMinX;
					fMinX = fMaxX;
					fMaxX = temp;
				}

				if ( fMinZ > fMaxZ )
				{
					float temp = fMinZ;
					fMinZ = fMaxZ;
					fMaxZ = temp;
				}

				fMinX -= radius;
				fMinZ -= radius;
				fMaxX += radius;
				fMaxZ += radius;

				int minX = (int) fMinX;
				int minY = (int) fMinZ;
				int maxX = (int) fMaxX;
				int maxY = (int) fMaxZ;

				if ( rampLength > 0 && maxX >= 0 && maxY >= 0 && minX < GGTERRAIN_HEIGHTMAP_EDIT_SIZE && minY < GGTERRAIN_HEIGHTMAP_EDIT_SIZE )
				{
					if ( minX < 0 ) minX = 0;
					if ( minY < 0 ) minY = 0;
					if ( maxX > GGTERRAIN_HEIGHTMAP_EDIT_SIZE ) maxX = GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
					if ( maxY > GGTERRAIN_HEIGHTMAP_EDIT_SIZE ) maxY = GGTERRAIN_HEIGHTMAP_EDIT_SIZE;

					#ifdef GGTERRAIN_UNDOREDO
					if (g_iCalculatingChangeBounds)
					{
						undosys_terrain_editsize(minX, maxX, minY, maxY);
					}
					#endif

					for( int y = minY; y < maxY; y++ )
					{
						for( int x = minX; x < maxX; x++ )
						{
							float diffX = x - fStartX;
							float diffZ = y - fStartZ;
							float dotp = diffX*rampVX + diffZ*rampVZ;
							if ( dotp < 0 || dotp > rampLength ) continue;

							float rampVX2 = rampVZ;
							float rampVZ2 = -rampVX;
							float perpDist = diffX*rampVX2 + diffZ*rampVZ2;
							if ( perpDist < -radius || perpDist > radius ) continue;

							float fract = dotp / rampLength;
							float targetY = fStartY + fract*(fEndY - fStartY);

							targetY = targetY - ggterrain_local_params.offset_y;
							if ( targetY < 0 ) targetY /= ggterrain_local_params.minHeight;
							else targetY /= ggterrain_local_params.height;

							float fade = perpDist / radius;
							fade = pow(fade, 6);
							fade = 1 - fade;

							uint32_t index = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - y) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + x;
							uint8_t heightType = pHeightMapEditType[ index ];
							
							float heightMapHeight = pHeightMapEdit[ index ];

							if ( heightType == 0 )
							{
								float realX = x / (float) GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
								realX = realX * 2 - 1;
								realX *= ggterrain_local_render_params2.editable_size;
								float realZ = y / (float) GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
								realZ = realZ * 2 - 1;
								realZ *= ggterrain_local_render_params2.editable_size;

								float realHeight = 0;
								GGTerrain_GetHeight( realX, realZ, &realHeight );
								realHeight -= ggterrain_local_params.offset_y;
								if ( realHeight < 0 ) realHeight /= ggterrain_local_params.minHeight;
								else realHeight /= ggterrain_local_params.height;

								heightMapHeight = realHeight;
							}
							
							float newHeightMapHeight = heightMapHeight + fade*(targetY - heightMapHeight);
								
							pHeightMapEditType[ index ] = 1; // replace mode
							pHeightMapEdit[ index ] = newHeightMapHeight;
						}
					}

					float realMinX = fMinX / (float) GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
					realMinX = realMinX * 2 - 1;
					realMinX *= ggterrain_local_render_params2.editable_size;
					float realMinZ = fMinZ / (float) GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
					realMinZ = realMinZ * 2 - 1;
					realMinZ *= ggterrain_local_render_params2.editable_size;
					float realMaxX = fMaxX / (float) GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
					realMaxX = realMaxX * 2 - 1;
					realMaxX *= ggterrain_local_render_params2.editable_size;
					float realMaxZ = fMaxZ / (float) GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
					realMaxZ = realMaxZ * 2 - 1;
					realMaxZ *= ggterrain_local_render_params2.editable_size;
					GGTerrain_InvalidateRegion( realMinX, realMinZ, realMaxX, realMaxZ, GGTERRAIN_INVALIDATE_ALL );
				}
			}
		}
	} 
	else if ( ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_PICK )
	{
		if ( ggterrain_internal_params.mouseLeftReleased )
		{
			float realHeight = 0;
			if ( GGTerrain_GetHeight( pickX, pickZ, &realHeight ) ) 
			{
				ggterrain_extra_params.sculpt_chosen_height = realHeight;
				ggterrain_extra_params.sculpt_mode = GGTERRAIN_SCULPT_WRITE;
			}
		}
	}
	else if ( ggterrain_internal_params.mouseLeftState )
	{		
		#ifdef GGTERRAIN_UNDOREDO
		if (g_iCalculatingChangeBounds == 0)
		{
			// Reset the calculated bounding box, ready for the next undo action.
		// minX and minY set higher than they can be, so the < comparison is always correct.
			g_EditBounds.minX = 50000;
			g_EditBounds.minY = 50000;
			g_EditBounds.maxX = 0;
			g_EditBounds.maxY = 0;
			// Take a snapshot of how the terrain is before any edits take place.
			GGTerrain_GetSculptData(g_pTerrainSnapshot);
			g_iCalculatingChangeBounds = 1;
		}
		#endif

		float radius = ggterrain_local_render_params2.brushSize / ggterrain_local_render_params2.editable_size;
		radius *= GGTERRAIN_HEIGHTMAP_EDIT_SIZE * 0.5f;

		int startX = (int) (fX - radius);
		int startY = (int) (fZ - radius);
		int endX = (int) (fX + radius);
		int endY = (int) (fZ + radius);

		if ( endX >= 0 && endY >= 0 && startX < GGTERRAIN_HEIGHTMAP_EDIT_SIZE && startY < GGTERRAIN_HEIGHTMAP_EDIT_SIZE )
		{
			if ( startX < 0 ) startX = 0;
			if ( startY < 0 ) startY = 0;
			if ( endX > GGTERRAIN_HEIGHTMAP_EDIT_SIZE ) endX = GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
			if ( endY > GGTERRAIN_HEIGHTMAP_EDIT_SIZE ) endY = GGTERRAIN_HEIGHTMAP_EDIT_SIZE;

			#ifdef GGTERRAIN_UNDOREDO
			if (g_iCalculatingChangeBounds == 1)
			{
				undosys_terrain_editsize(startX, endX, startY, endY);
			}
			#endif

			if ( ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_BLEND )
			{
				if ( startX > 1 ) startX-=2;
				if ( startY > 1 ) startY-=2;
				if ( endX < GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 ) endX+=2;
				if ( endY < GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 ) endY+=2;

				int diffX = endX - startX;
				int diffY = endY - startY;
				int dataSize = diffX * diffY;
				float* newHeights = new float[ dataSize ];
				float* newHeights2 = new float[ dataSize ];

				for( int y = 0; y < diffY; y++ )
				{
					for( int x = 0; x < diffX; x++ )
					{
						int origY = y + startY;
						int origX = x + startX;
						int indexOrig = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - origY) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + origX;
						int index = y * diffX + x;

						float realHeight = 0;
						switch( pHeightMapEditType[ indexOrig ] )
						{
							case 0: // add
							{
								float realX2 = origX / (float) GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
								realX2 = realX2 * 2 - 1;
								realX2 *= ggterrain_local_render_params2.editable_size;
								float realZ2 = origY / (float) GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
								realZ2 = realZ2 * 2 - 1;
								realZ2 *= ggterrain_local_render_params2.editable_size;

								GGTerrain_GetHeight( realX2, realZ2, &realHeight );
								realHeight -= ggterrain_local_params.offset_y;
								if ( realHeight < 0 ) realHeight /= ggterrain_local_params.minHeight;
								else realHeight /= ggterrain_local_params.height;
							} break;

							case 1: // replace
							{
								realHeight = pHeightMapEdit[ indexOrig ];
							} break;
						}

						newHeights[ index ] = realHeight;
					}
				}

				float halfDiffX = diffX / 2.0f;
				float halfDiffY = diffY / 2.0f;

				int filterSize = (int) sqrt(ggterrain_extra_params.sculpt_speed * 2);
				filterSize |= 1; // make sure it is odd
				if ( filterSize > 21 ) filterSize = 21;
				if ( filterSize < 3 ) filterSize = 3;
				int filterSizeHalf = filterSize / 2;
				float weight = 1.0f / (float) filterSize;

				// blur newHeights

				// horizontlal blur
				for( int y = 0; y < diffY; y++ )
				{
					for( int x = 0; x < diffX; x++ )
					{
						int index2 = y * diffX + x;

						float distX = x - halfDiffX;
						float distY = y - halfDiffY;
						float dist = distX*distX + distY*distY;
						if ( dist > radius*radius ) 
						{
							newHeights2[ index2 ] = newHeights[ index2 ];
							continue;
						}

						float avg = 0;
						for( int i = 0; i < filterSize; i++ )
						{
							int sampleX = x + i - filterSizeHalf;
							if ( sampleX < 0 ) sampleX = 0;
							if ( sampleX >= diffX ) sampleX = diffX-1;

							int index = y * diffX + sampleX;
							avg += newHeights[ index ] * weight;
						}

						newHeights2[ index2 ] = avg;
					}
				}

				// vertical blur
				for( int y = 0; y < diffY; y++ )
				{
					for( int x = 0; x < diffX; x++ )
					{
						int index = y * diffX + x;

						float distX = x - halfDiffX;
						float distY = y - halfDiffY;
						float dist = distX*distX + distY*distY;
						if ( dist > radius*radius ) 
						{
							newHeights[ index ] = newHeights2[ index ];
							continue;
						}

						float avg = 0;
						for( int i = 0; i < filterSize; i++ )
						{
							int sampleY = y + i - filterSizeHalf;
							if ( sampleY < 0 ) sampleY = 0;
							if ( sampleY >= diffY ) sampleY = diffY-1;

							int index2 = sampleY * diffX + x;
							avg += newHeights2[ index2 ] * weight;
						}
						
						newHeights[ index ] = avg;
					}
				}

				delete [] newHeights2;

				// update heightmap
				for( int y = 0; y < diffY; y++ )
				{
					for( int x = 0; x < diffX; x++ )
					{
						float distX = x - halfDiffX;
						float distY = y - halfDiffY;
						float dist = distX*distX + distY*distY;
						if ( dist > radius*radius ) continue;

						int origY = y + startY;
						int origX = x + startX;
						int indexOrig = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - origY) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + origX;
						int index = y * diffX + x;

						pHeightMapEditType[ indexOrig ] = 1; // replace
						pHeightMapEdit[ indexOrig ] = newHeights[ index ];
					}
				}

				delete [] newHeights;
			}
			else
			{
				for( int y = startY; y < endY; y++ )
				{
					for( int x = startX; x < endX; x++ )
					{
						float diffX = x - fX;
						float diffY = y - fZ;
						float dist = diffX*diffX + diffY*diffY;
						if ( dist > radius*radius ) continue;
				
						uint32_t index = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE-1 - y) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + x;
						uint8_t heightType = pHeightMapEditType[ index ];
						float newHeightMapHeight;

						float realX = x / (float) GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
						realX = realX * 2 - 1;
						realX *= ggterrain_local_render_params2.editable_size;
						float realZ = y / (float) GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
						realZ = realZ * 2 - 1;
						realZ *= ggterrain_local_render_params2.editable_size;

						float heightMapHeight = pHeightMapEdit[ index ];

						newHeightMapHeight = heightMapHeight;
										
						switch( ggterrain_extra_params.sculpt_mode )
						{
							case GGTERRAIN_SCULPT_RAISE:
							{
								float adjust = dist / (radius*radius);
								adjust = 1 - adjust;
								adjust *= ggterrain_extra_params.sculpt_speed * 0.00004f;

								switch( heightType )
								{
									case 0: // add
									{
										newHeightMapHeight = heightMapHeight + adjust;
									} break;

									case 1: // replace
									{
										float realHeight = 0;
										GGTerrain_GetHeight( realX, realZ, &realHeight );
										realHeight -= ggterrain_local_params.offset_y;
										if ( realHeight < 0 ) realHeight /= ggterrain_local_params.minHeight;
										else realHeight /= ggterrain_local_params.height;

										float fractalHeight = GGTerrainChunk::CalculateHeightWithHeightmap( realX, realZ, 0 );
										fractalHeight -= ggterrain_local_params.offset_y;
										if ( fractalHeight < 0 ) fractalHeight /= ggterrain_local_params.minHeight;
										else fractalHeight /= ggterrain_local_params.height;

										newHeightMapHeight = realHeight - fractalHeight + adjust;
									} break;
								}

								heightType = 0; // add
							} break;

							case GGTERRAIN_SCULPT_LOWER:
							{
								float adjust = dist / (radius*radius);
								adjust = adjust - 1;
								adjust *= ggterrain_extra_params.sculpt_speed * 0.00004f;

								switch( heightType )
								{
									case 0: // add
									{
										newHeightMapHeight = heightMapHeight + adjust;
									} break;

									case 1: // replace
									{
										float realHeight = 0;
										GGTerrain_GetHeight( realX, realZ, &realHeight );
										realHeight -= ggterrain_local_params.offset_y;
										if ( realHeight < 0 ) realHeight /= ggterrain_local_params.minHeight;
										else realHeight /= ggterrain_local_params.height;

										float fractalHeight = GGTerrainChunk::CalculateHeightWithHeightmap( realX, realZ, 0 );
										fractalHeight -= ggterrain_local_params.offset_y;
										if ( fractalHeight < 0 ) fractalHeight /= ggterrain_local_params.minHeight;
										else fractalHeight /= ggterrain_local_params.height;

										newHeightMapHeight = realHeight - fractalHeight + adjust;
									} break;
								}

								heightType = 0; // add
							} break;

							case GGTERRAIN_SCULPT_LEVEL:
							{
								float targetY = ggterrain_internal_params.sculpt_pick_y - ggterrain_local_params.offset_y;
								//float targetY = pickY - ggterrain_local_params.offset_y;
								if ( targetY < 0 ) targetY /= ggterrain_local_params.minHeight;
								else targetY /= ggterrain_local_params.height;

								float fade = dist / (radius*radius);
								fade = 1 - fade;
							
								switch( heightType )
								{
									case 0: // add
									{
										float realHeight = 0;
										GGTerrain_GetHeight( realX, realZ, &realHeight );
										realHeight -= ggterrain_local_params.offset_y;
										if ( realHeight < 0 ) realHeight /= ggterrain_local_params.minHeight;
										else realHeight /= ggterrain_local_params.height;

										newHeightMapHeight = realHeight;
									} break;

									case 1: // replace
									{
										float diffY = targetY - heightMapHeight;
										float speed = ggterrain_extra_params.sculpt_speed * 0.0002f;
										if ( diffY > speed ) diffY = speed;
										else if ( diffY < -speed ) diffY = -speed;
										newHeightMapHeight = heightMapHeight + diffY * fade;
									} break;
								}

								heightType = 1; // replace
							} break;

							case GGTERRAIN_SCULPT_RANDOM:
							{
								float fX2 = realX / 50000;
								float fZ2 = realZ / 50000;

								float frequency = ggterrain_extra_params.sculpt_randomness_frequency;
								float amplitude = 0.01f;
								float noiseValue = 0;
								float total = 0;

								for ( size_t i = 0; i < 10; i++ ) 
								{
									float offset = noise.noise( frequency * fX2, frequency * fZ2 );

									offset *= amplitude;
									noiseValue += offset;
									total += amplitude;

									frequency *= 2.5f;
									amplitude *= 0.4f;
								}
								noiseValue /= total;
								noiseValue = noiseValue * 0.5f + 0.5f;
							
								float fade = dist / (radius*radius);
								fade = 1 - fade;
							
								switch( heightType )
								{
									case 0: // add
									{
										float realHeight = 0;
										GGTerrain_GetHeight( realX, realZ, &realHeight );
										realHeight -= ggterrain_local_params.offset_y;
										if ( realHeight < 0 ) realHeight /= ggterrain_local_params.minHeight;
										else realHeight /= ggterrain_local_params.height;

										newHeightMapHeight = realHeight;
									} break;

									case 1: // replace
									{
										float speed = ggterrain_extra_params.sculpt_speed * 0.00002f;
										noiseValue *= speed;
										newHeightMapHeight = heightMapHeight + noiseValue * fade;
									} break;
								}

								heightType = 1; // replace
							} break;

							case GGTERRAIN_SCULPT_WRITE:
							{
								float targetY = ggterrain_extra_params.sculpt_chosen_height - ggterrain_local_params.offset_y;
								if ( targetY < 0 ) targetY /= ggterrain_local_params.minHeight;
								else targetY /= ggterrain_local_params.height;

								float fade = dist / (radius*radius);
								fade = 1 - fade;
							
								switch( heightType )
								{
									case 0: // add
									{
										float realHeight = 0;
										GGTerrain_GetHeight( realX, realZ, &realHeight );
										realHeight -= ggterrain_local_params.offset_y;
										if ( realHeight < 0 ) realHeight /= ggterrain_local_params.minHeight;
										else realHeight /= ggterrain_local_params.height;

										newHeightMapHeight = realHeight;
									} break;

									case 1: // replace
									{
										float diffY = targetY - heightMapHeight;
										float speed = ggterrain_extra_params.sculpt_speed * 0.0002f;
										if ( diffY > speed ) diffY = speed;
										else if ( diffY < -speed ) diffY = -speed;
										newHeightMapHeight = heightMapHeight + diffY * fade;
									} break;
								}

								heightType = 1; // replace
							} break;

							case GGTERRAIN_SCULPT_RESTORE:
							{
								newHeightMapHeight = 0;
								heightType = 0; // add
							} break;
						}

						pHeightMapEditType[ index ] = heightType;
						pHeightMapEdit[ index ] = newHeightMapHeight;
					}
				}
			}

			float realRadius = ggterrain_local_render_params2.brushSize + 50;
			GGTerrain_InvalidateRegion( pickX - realRadius, pickZ - realRadius, pickX + realRadius, pickZ + realRadius, GGTERRAIN_INVALIDATE_ALL );
		}
	}
#endif
}

void GGTerrain_Update_Painting( float pickX, float pickY, float pickZ )
{
#ifdef GGTERRAIN_ENABLE_PAINTING
	float fX = pickX / ggterrain_local_render_params2.editable_size;
	fX = fX * 0.5f + 0.5f;
	fX *= GGTERRAIN_MATERIALMAP_SIZE;

	float fZ = pickZ / ggterrain_local_render_params2.editable_size;
	fZ = fZ * 0.5f + 0.5f;
	fZ *= GGTERRAIN_MATERIALMAP_SIZE;

	if ( ggterrain_internal_params.mouseLeftState )
	{		
		#ifdef GGTERRAIN_UNDOREDO
		if (g_iCalculatingChangeBounds == 0)
		{
			// Take a snapshot of how the terrain is before any edits take place.
			GGTerrain_GetPaintData(g_pTerrainSnapshot);
			g_iCalculatingChangeBounds = 1;
		}
		#endif

		float radius = ggterrain_local_render_params2.brushSize / ggterrain_local_render_params2.editable_size;
		radius *= GGTERRAIN_MATERIALMAP_SIZE * 0.5f;

		int startX = (int) (fX - radius);
		int startY = (int) (fZ - radius);
		int endX = (int) (fX + radius);
		int endY = (int) (fZ + radius);

		// don't allow drawing of the border pixels so clamp mode will always read 0
		if ( endX >= 1 && endY >= 1 && startX < GGTERRAIN_MATERIALMAP_SIZE-2 && startY < GGTERRAIN_MATERIALMAP_SIZE-2 )
		{
			if ( startX < 1 ) startX = 1;
			if ( startY < 1 ) startY = 1;
			if ( endX > GGTERRAIN_MATERIALMAP_SIZE-1 ) endX = GGTERRAIN_MATERIALMAP_SIZE-1;
			if ( endY > GGTERRAIN_MATERIALMAP_SIZE-1 ) endY = GGTERRAIN_MATERIALMAP_SIZE-1;

			// Calculate bounding box of all changed values (for the undo system).
			#ifdef GGTERRAIN_UNDOREDO
			if (g_iCalculatingChangeBounds == 1)
			{
				undosys_terrain_editsize(startX, endX, startY, endY);
			}
			#endif

			for( int y = startY; y < endY; y++ )
			{
				for( int x = startX; x < endX; x++ )
				{
					float diffX = x - fX;
					float diffY = y - fZ;
					float dist = diffX*diffX + diffY*diffY;
					if ( dist > radius*radius ) continue;
				
					uint32_t index = y * GGTERRAIN_MATERIALMAP_SIZE + x;
					pMaterialMap[ index ] = ggterrain_extra_params.paint_material;
				}
			}

			wiRenderer::GetDevice()->UpdateTexture( &texMaterialMap, 0, 0, NULL, pMaterialMap, GGTERRAIN_MATERIALMAP_SIZE, -1 );

			float realRadius = ggterrain_local_render_params2.brushSize + 50;
			GGTerrain_InvalidateRegion( pickX - realRadius, pickZ - realRadius, pickX + realRadius, pickZ + realRadius, GGTERRAIN_INVALIDATE_TEXTURES );
		}
	}

	if ( ggterrain_internal_params.mouseLeftReleased )
	{
		ggterrain_extra_params.iUpdateGrass = 2;
	}
#endif
}

// contorl env probe list
void GGTerrain_ClearEnvProbeList(void)
{
	// create new env probe local list
	g_envProbeList.clear();

	// also clear all real probes so can be updated
	for (int scan = 0; scan < LOCALENVPROBECOUNT; scan++)
	{
		g_bEnvProbeTrackingUpdate[scan] = true;
		g_iEnvProbeTracking[scan] = 0;
	}

	// a good time to refresh global in case of env changes!
	globalEnvProbePos.y = 0;
}
void GGTerrain_InstantEnvProbeRefresh(int iCoolDownIndex)
{
	// now update all live probes
	for (int iRealProbeIndex = 0; iRealProbeIndex < LOCALENVPROBECOUNT; iRealProbeIndex++)
	{
		EnvironmentProbeComponent* probe = wiScene::GetScene().probes.GetComponent(localEnvProbe[iRealProbeIndex]);
		wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(localEnvProbe[iRealProbeIndex]);
		if (probe && pTransform)
		{
			if (g_iEnvProbeTracking[iRealProbeIndex] > 0)
			{
				if (iCoolDownIndex == 0 || iCoolDownIndex == iRealProbeIndex)
				{
					probe->SetDirty();
					if (iCoolDownIndex != 0)
						break;
				}
			}
		}
	}
}

void GGTerrain_AddEnvProbeList(float x, float y, float z, float range, float quatx, float quaty, float quatz, float quatw, float sx, float sy, float sz, float brightness)
{
	envProbeItem item;
	item.position = XMFLOAT3(x, y, z);
	item.range = range;
	item.rotation = XMFLOAT4(quatx, quaty, quatz, quatw);
	item.size = XMFLOAT3(sx, sy, sz);
	item.distance = 0;
	item.brightness = brightness;
	g_envProbeList.push_back(item);
}

#ifdef TERRAINTHREADSAFE
//std::mutex terrainlock = {};
std::recursive_mutex terrainlock = {};
#else
class terrainlockclass
{
public:
	void lock(void) {};
	void unlock(void) {};
};
terrainlockclass terrainlock;
#endif

void GGTerrain_EnvProbeWork (float playerX, float playerY, float playerZ)
{
	if (!ggprobe_initialised)
	{
		ggprobe_initialised = 1;
		float globalrange = 50000;
		globalEnvProbePos = XMFLOAT3(0, ggterrain_local_params.height, 0);
		globalEnvProbe = wiScene::GetScene().Entity_CreateEnvironmentProbe("globalEnvProbe", globalEnvProbePos);
		EnvironmentProbeComponent* probe = wiScene::GetScene().probes.GetComponent(globalEnvProbe);
		probe->range = globalrange;
		probe->userdata = 255;
		probe->SetDirty();
		wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(globalEnvProbe);
		pTransform->ClearTransform();
		pTransform->Translate(globalEnvProbePos);
		pTransform->Scale(XMFLOAT3(globalrange, globalrange, globalrange));
		pTransform->UpdateTransform();
		pTransform->SetDirty();

		// local env probe creation
		float range = 1;
		for (int i = 0; i < LOCALENVPROBECOUNT; i++)
		{
			range = 1;
			localEnvProbePos[i] = XMFLOAT3(0, 0, 0);
			char strName[64];
			sprintf_s(strName, "localEnvProbe%d", i);
			localEnvProbe[i] = wiScene::GetScene().Entity_CreateEnvironmentProbe(strName, localEnvProbePos[i]);
			probe = wiScene::GetScene().probes.GetComponent(localEnvProbe[i]);
			probe->range = range; // env cube range
			probe->userdata = 0; // 0-255 alpha
			probe->SetDirty();
			pTransform = wiScene::GetScene().transforms.GetComponent(localEnvProbe[i]);
			pTransform->ClearTransform();
			pTransform->Translate(localEnvProbePos[i]);
			pTransform->Scale(XMFLOAT3(range, range, range));
			pTransform->UpdateTransform();
			pTransform->SetDirty();
		}

		// current probe for player to use
		currLocalEnvProbe = 0;
	}
	// find the eight closest env light probes to the player
	bool bUseOld2SwapSystem = true;
	if (g_envProbeList.size() > 0 || bImGuiInTestGame == true)
	{
		// using env probe placement approach (best), and always use when in editor mode (so 2-way trick not seen as not useful in editor)
		bUseOld2SwapSystem = false;
	}

	// New placed probe system or fallback 2-probe swap system
	if (bUseOld2SwapSystem == false)
	{
		// special system to createw a delta from movement, and use this to transition the alpha influence of env probes
		// this solves the issue of camera leaving zones of calc for the env causing a pop in the visual
		bool bMovementOfPlayer = false;
		static XMFLOAT3 g_vLastPos;
		float fMovementDelta = 1.0;
		if (bImGuiInTestGame == true)
		{
			fMovementDelta = (fabs(g_vLastPos.x - playerX) + fabs(g_vLastPos.z - playerZ)) / 20.0f;
			if (fMovementDelta > 0.0f && fMovementDelta < 1.0f)
			{
				bMovementOfPlayer = true;
				fMovementDelta = 1.0f;
			}
			if (globalEnvProbePos.y == 0)
			{
				// indicates a forced env map refresh (light on/off) so nudge the update!
				bMovementOfPlayer = true;
				fMovementDelta = 1.0f;
			}
		}
		g_vLastPos.x = playerX;
		g_vLastPos.z = playerZ;

		// Use g_envProbeList to coordinate best use of the LOCALENVPROBECOUNT (8) probes reserved for this
		for (int p = 0; p < g_envProbeList.size(); p++)
		{
			float diffX = g_envProbeList[p].position.x - playerX;
			float diffY = g_envProbeList[p].position.y - playerY;
			float diffZ = g_envProbeList[p].position.z - playerZ;
			float sqrDist = diffX * diffX + diffY * diffY + diffZ * diffZ;
			g_envProbeList[p].distance = sqrDist;
			g_envProbeList[p].used = 0;
		}
		std::vector<int> envBestProbes;
		envBestProbes.clear();
		while (envBestProbes.size() < LOCALENVPROBECOUNT)
		{
			int iBestP = -1;
			float fBestDist = 99999999.9;
			for (int p = 0; p < g_envProbeList.size(); p++)
			{
				if (g_envProbeList[p].distance < fBestDist && g_envProbeList[p].used == 0)
				{
					fBestDist = g_envProbeList[p].distance;
					iBestP = p;
				}
			}
			if (iBestP != -1)
			{
				// next best one is added
				g_envProbeList[iBestP].used = 1;
				envBestProbes.push_back(iBestP);
			}
			else
			{
				// can find no more, leave
				break;
			}
		}

		// now maintain and assign real probes
		if (envBestProbes.size() > 0)
		{
			// first clear out any live slots no longer used
			for (int scan = 0; scan < LOCALENVPROBECOUNT; scan++)
			{
				if (g_iEnvProbeTracking[scan] > 0)
				{
					int p = g_iEnvProbeTracking[scan] - 1;
					if ((p < g_envProbeList.size() && g_envProbeList[p].used == 0) || p >= g_envProbeList.size())
					{
						g_bEnvProbeTrackingUpdate[scan] = true;
						g_iEnvProbeTracking[scan] = 0; // cause to fade out
					}
				}
			}

			// then maintain live ones from envBestProbes that are already being used, and assign new ones if needed
			for (int best = 0; best < envBestProbes.size(); best++)
			{
				bool bFoundInTrackingList = false;
				for (int scan = 0; scan < LOCALENVPROBECOUNT; scan++)
				{
					if (g_iEnvProbeTracking[scan] > 0)
					{
						int p = g_iEnvProbeTracking[scan] - 1;
						if (p == envBestProbes[best])
						{
							bFoundInTrackingList = true;
						}
					}
				}
				if (bFoundInTrackingList == false)
				{
					// assign this to a spare slot
					for (int scan = 0; scan < LOCALENVPROBECOUNT; scan++)
					{
						if (g_iEnvProbeTracking[scan] == 0 && g_bEnvProbeTrackingUpdate[scan] == false) // only when not in fade out mode, can reuse this
						{
							int p = envBestProbes[best];
							g_iEnvProbeTracking[scan] = 1 + p;
							g_bEnvProbeTrackingUpdate[scan] = true;
							break;
						}
					}
				}
			}
		}

		// now update all live probes
		for (int iRealProbeIndex = 0; iRealProbeIndex < LOCALENVPROBECOUNT; iRealProbeIndex++)
		{
			// the real probe ptr
			EnvironmentProbeComponent* probe = wiScene::GetScene().probes.GetComponent(localEnvProbe[iRealProbeIndex]);
			wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(localEnvProbe[iRealProbeIndex]);
			if (probe && pTransform)
			{
				// the virtual probe index
				bool bRefreshProbeFromUpdatesAndPlayerMovement = false;
				if (g_bEnvProbeTrackingUpdate[iRealProbeIndex] == true)
				{
					if (g_iEnvProbeTracking[iRealProbeIndex] > 0)
					{
						// locate correct virtual local probe
						int p = g_iEnvProbeTracking[iRealProbeIndex] - 1;

						// new location of this probe based on pos
						localEnvProbePos[iRealProbeIndex] = g_envProbeList[p].position;
						probe->position = localEnvProbePos[iRealProbeIndex];

						// move probe and set range
						int range = g_envProbeList[p].range;
						probe->range = range;
						if (probe->userdata < 255)
						{
							int iNewValue = probe->userdata + (fabs(fMovementDelta) * 20.0f);
							probe->userdata = iNewValue;
							if (probe->userdata >= 255)
							{
								g_bEnvProbeTrackingUpdate[iRealProbeIndex] = false;
								probe->userdata = 255;
							}
						}
						else
						{
							g_bEnvProbeTrackingUpdate[iRealProbeIndex] = false;
						}

						probe->SetBrightness(g_envProbeList[p].brightness);

						// update probe with correct scaling
						pTransform->ClearTransform();
						pTransform->Translate(localEnvProbePos[iRealProbeIndex]);
						float fSX = g_envProbeList[p].size.x;// / 100.0f;
						float fSY = g_envProbeList[p].size.y;// / 100.0f;
						float fSZ = g_envProbeList[p].size.z;// / 100.0f;
						pTransform->Scale(XMFLOAT3(fSX, fSY, fSZ));
						pTransform->Rotate(g_envProbeList[p].rotation);
						pTransform->UpdateTransform();
						pTransform->SetDirty();
					}
					else
					{
						// when no longer used, can fade it out of usage
						if (probe->range > 1)
						{
							probe->range -= (fMovementDelta * 20.0f);
							if (probe->range < 1) probe->range = 1;
						}
						if (probe->userdata > 0)
						{
							int iNewValue = probe->userdata - (fabs(fMovementDelta) * 20.0f);
							if (iNewValue < 0) iNewValue = 0;
							probe->userdata = iNewValue;
							if (probe->userdata <= 0)
							{
								g_bEnvProbeTrackingUpdate[iRealProbeIndex] = false;
								probe->userdata = 0;
							}
						}
						else
						{
							g_bEnvProbeTrackingUpdate[iRealProbeIndex] = false;
						}
						if (g_bEnvProbeTrackingUpdate[iRealProbeIndex] == false)
						{
							// update probe with correct scaling
							pTransform->ClearTransform();
							localEnvProbePos[iRealProbeIndex] = XMFLOAT3(-999999, -999999, -999999);
							pTransform->Translate(localEnvProbePos[iRealProbeIndex]);
							pTransform->UpdateTransform();
							pTransform->SetDirty();
						}
					}
					bRefreshProbeFromUpdatesAndPlayerMovement = true;
				}
				if (bRefreshProbeFromUpdatesAndPlayerMovement == true && fMovementDelta > 0.0f)
				{
					probe->SetDirty();
					bRefreshProbeFromUpdatesAndPlayerMovement = false;
				}
			}
		}
	}
	else
	{
#ifdef PEOPTIMIZING
		static bool bInEditor = false;
#endif
		// Dynamic 2-probe swapping system (performant and stable but causes artifacts when moving from one extreme to another)
		if (bImGuiInTestGame == true)
		{
#ifdef PEOPTIMIZING
			bInEditor = false;
#endif
#define GGTERRAIN_ENV_TRANSITION_FRAMES 60
			float diffX = playerX - localEnvProbePos[currLocalEnvProbe].x;
			float diffY = playerY - localEnvProbePos[currLocalEnvProbe].y;
			float diffZ = playerZ - localEnvProbePos[currLocalEnvProbe].z;
			float sqrDist = diffX * diffX + diffY * diffY + diffZ * diffZ;
			float checkDist = GGTerrain_MetersToUnits(GGTERRAIN_ENV_PROBE_UPDATE_METERS);
			if (localEnvProbeTransition == 0 && sqrDist > checkDist * checkDist)
			{
				// make old probe active and place it at player position
				currLocalEnvProbe = 1 - currLocalEnvProbe;

				// find position above floor of player location to position the probe
				float range = GGTerrain_MetersToUnits(GGTERRAIN_ENV_PROBE_RANGE_METERS);
				XMFLOAT3 playerPos = XMFLOAT3(playerX, playerY, playerZ);
				float terrainHeight;
				if (ggterrain.GetHeightLOD(playerX, playerZ, 4, &terrainHeight))
				{
					terrainHeight += 25;
					if (playerPos.y < terrainHeight) playerPos.y = terrainHeight;
				}

				// move probe and set range
				EnvironmentProbeComponent* probe = wiScene::GetScene().probes.GetComponent(localEnvProbe[currLocalEnvProbe]);
				probe->position = playerPos;
				probe->range = range;
				probe->userdata = 0;
				probe->SetDirty();

				// update probe with correct scaling
				wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(localEnvProbe[currLocalEnvProbe]);
				pTransform->ClearTransform();
				pTransform->Translate(playerPos);
				pTransform->Scale(XMFLOAT3(range, range, range));
				pTransform->UpdateTransform();
				pTransform->SetDirty();

				// new location of this probe based on player pos
				localEnvProbePos[currLocalEnvProbe] = playerPos;

				// start transition between old and new probes
				localEnvProbeTransition = GGTERRAIN_ENV_TRANSITION_FRAMES;
			}

			// transition counter will slerp from old to new position
			if (localEnvProbeTransition > 0)
			{
				// count down until finished transition
				localEnvProbeTransition--;

				// fade between probes
				int transition1 = (localEnvProbeTransition * 511) / GGTERRAIN_ENV_TRANSITION_FRAMES;
				if (transition1 > 511) transition1 = 511;
				int transition2 = transition1 - 256;
				if (transition2 < 0) transition2 = 0;
				if (transition1 > 255) transition1 = 255;

				// new probe expands
				EnvironmentProbeComponent* probe = wiScene::GetScene().probes.GetComponent(localEnvProbe[currLocalEnvProbe]);
				probe->userdata = 255 - transition2;

				// old probe shrinks
				probe = wiScene::GetScene().probes.GetComponent(localEnvProbe[1 - currLocalEnvProbe]);
				probe->userdata = transition1;

				// when finished transition
				if (localEnvProbeTransition == 0)
				{
					// make old probe small enough to be excluded
					probe->range = 1;
					wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(localEnvProbe[1 - currLocalEnvProbe]);
					pTransform->ClearTransform();
					pTransform->Translate(localEnvProbePos[1 - currLocalEnvProbe]);
					pTransform->Scale(XMFLOAT3(1, 1, 1));
					pTransform->UpdateTransform();
					pTransform->SetDirty();
				}

				// ensure rest of probes cleared
				for (int iRest = 2; iRest < LOCALENVPROBECOUNT; iRest++)
				{
					EnvironmentProbeComponent* restprobe = wiScene::GetScene().probes.GetComponent(localEnvProbe[iRest]);
					if (restprobe)
					{
						restprobe->range = 1;
						restprobe->userdata = 0;
						wiScene::TransformComponent* pRestTransform = wiScene::GetScene().transforms.GetComponent(localEnvProbe[iRest]);
						if (pRestTransform)
						{
							pRestTransform->ClearTransform();
							pRestTransform->Translate(localEnvProbePos[iRest]);
							pRestTransform->Scale(XMFLOAT3(1, 1, 1));
							pRestTransform->UpdateTransform();
							pRestTransform->SetDirty();
						}
					}
				}
			}
		}
		else
		{
#ifdef PEOPTIMIZING
			//PE: OPT1 Only run this once, each time its dirty both probes updates even if range=1. (objects,terrain,trees...).
			if (!bInEditor)
			{
				bInEditor = true;
				// when in editor, and no probes
				for (int twoway = 0; twoway < 2; twoway++)
				{
					EnvironmentProbeComponent* probe = wiScene::GetScene().probes.GetComponent(localEnvProbe[twoway]);
					probe->SetRealTime(false);
					probe->range = 1;
					probe->userdata = 0;
					probe->SetDirty();
					wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(localEnvProbe[twoway]);
					pTransform->ClearTransform();
					pTransform->Scale(XMFLOAT3(1, 1, 1));
					pTransform->UpdateTransform();
					pTransform->SetDirty();
				}
			}
#else
			// when in editor, and no probes
			for (int twoway = 0; twoway < 2; twoway++)
			{
				EnvironmentProbeComponent* probe = wiScene::GetScene().probes.GetComponent(localEnvProbe[twoway]);
				probe->range = 1;
				probe->userdata = 0;
				probe->SetDirty();
				wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(localEnvProbe[twoway]);
				pTransform->ClearTransform();
				pTransform->Scale(XMFLOAT3(1, 1, 1));
				pTransform->UpdateTransform();
				pTransform->SetDirty();
			}
#endif
		}
	}

	// update global probe and local env probe when flagged
	if (ggterrain_extra_params.bUpdateProbes)
	{
		// also clear all real probes so can be updated
		for (int scan = 0; scan < LOCALENVPROBECOUNT; scan++)
		{
			g_bEnvProbeTrackingUpdate[scan] = true;
		}
		ggterrain_extra_params.bUpdateProbes = false;
	}

	// handle global probe positioned by globalEnvProbePos
	float globalrange = 50000;
	//float height; base on terrain biome (stock default), not local terrain ground height
	//GGTerrain_GetHeight( 0, 0, &height );
	//height += GGTerrain_MetersToUnits(30);
	float height = ggterrain_local_params.height;
	float heightDiff = fabs(height - globalEnvProbePos.y); // can refresh global probe by setting globalEnvProbePos.y to 0
	if (heightDiff > 50)
	{
		globalEnvProbePos = XMFLOAT3(0, height, 0);
		EnvironmentProbeComponent* probe = wiScene::GetScene().probes.GetComponent(globalEnvProbe);
		probe->position = globalEnvProbePos;
		probe->range = globalrange;
		probe->userdata = 255;
		float GetEnvProbeBrightness(void);
		probe->SetBrightness(GetEnvProbeBrightness());
		probe->SetDirty();
		
		wiScene::TransformComponent* pTransform = wiScene::GetScene().transforms.GetComponent(globalEnvProbe);
		pTransform->ClearTransform();
		pTransform->Translate(globalEnvProbePos);
		pTransform->Scale(XMFLOAT3(probe->range, probe->range, probe->range));
		pTransform->UpdateTransform();
		pTransform->SetDirty();
	}
}

// update the terrain, generates new chunks if necessary, and updates the virtual texture and page tables
void GGTerrain_Update( float playerX, float playerY, float playerZ, wiGraphics::CommandList cmd, bool bRenderTargetFocus )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	if (g_iDeferTextureUpdateToNow > 0 && ggterrain_initialised)
	{
		if (g_iDeferTextureUpdateToNow == 1)
		{
			cstr oldDir = GetDir();
#ifdef ONLYLOADWHENUSED
			//PE: Reload textures here.
			for (int i = 0; i < GGTERRAIN_MAX_SOURCE_TEXTURES; i++)
			{
				bTextureUploaded[i] = false;
			}
#endif

			SetDir(g_DeferTextureUpdateMAXRootFolder_s.Get());// g.fpscrootdir_s.Get());
			GGTerrain_ReloadTextures(cmd);
			SetDir(oldDir.Get());
			g_iDeferTextureUpdateToNow = 0;

		}
		if (g_iDeferTextureUpdateToNow == 2)
		{
			cstr oldDir = GetDir();
			// Update the textures for the terrain
#ifdef ONLYLOADWHENUSED
			for (int i = 0; i < GGTERRAIN_MAX_SOURCE_TEXTURES; i++)
			{
				bTextureUploaded[i] = false;
			}
#endif

			g_DeferTextureUpdateIncompatibleTextures.clear();
			if(strlen(g_DeferTextureUpdateCurrentFolder_s.Get())>0) SetDir(g_DeferTextureUpdateCurrentFolder_s.Get());
			GGTerrain_ReloadTextures(cmd, &g_DeferTextureUpdate, &g_DeferTextureUpdateIncompatibleTextures, g_DeferTextureUpdateMAXRootFolder_s.Get());// g.fpscrootdir_s.Get());
			SetDir(oldDir.Get());
			g_iDeferTextureUpdateToNow = 3;

#ifdef ONLYLOADWHENUSED
			//PE: All will be loaded correct here.
			for (int i = 0; i < GGTERRAIN_MAX_SOURCE_TEXTURES; i++)
			{
				bTextureUploaded[i] = true;
			}
#endif

		}
	}
#ifdef ONLYLOADWHENUSED
	if (bCheckForNewTerrainTextures && ggterrain_initialised)
	{
		GGTerrain_CheckMaterialUsed(cmd);
	}
#endif
	if ( !bImGuiGotFocus && ggterrain_initialised)
	{
		GGTerrain_CheckKeys();
		if (GGTerrain_GetKeyPressed(GGKEY_ESCAPE)) GGTerrain_CancelRamp();
	}

	// Environmental Light Probe System
	GGTerrain_EnvProbeWork(playerX, playerY, playerZ);

	if ( !ggterrain_initialised ) return;

	// terrain generation and chunk updating
	GGTerrainLODSet* pCurrLODs = nullptr;
	if (true)
	{
		// suspect this was the cause of GPU instability due to large stalls!
		/*
		terrainlock.lock();
		if (ggterrain_update_enabled)
		{
			ggterrain.CheckParams();
			ggterrain.UpdateChunks(playerX, playerZ);

			// wait for the lowest level to complete
			GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
			uint32_t timeout = 0;
			while (pCurrLODs->IsGenerating() && !pCurrLODs->pLevels[pCurrLODs->GetNumLevels() - 1].IsReady() && timeout++ < 300) Sleep(1);
			if (timeout >= 300)
			{
				pCurrLODs->iFlags &= ~GGTERRAIN_LOD_GENERATING; // terrain is not looking correct after this.
				ggterrain_global_params.bForceUpdate = 1 - ggterrain_global_params.bForceUpdate;
			}
		}
		terrainlock.unlock();
		*/
		if (ggterrain_update_enabled)
		{
			terrainlock.lock();
			ggterrain.CheckParams();
			ggterrain.UpdateChunks(playerX, playerZ);
			terrainlock.unlock();

			// wait for the lowest level to complete
			GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
			uint32_t timeout = 0;
			while (pCurrLODs->IsGenerating() && !pCurrLODs->pLevels[pCurrLODs->GetNumLevels() - 1].IsReady() && timeout++ < 300) Sleep(1);
			if (timeout >= 300)
			{
				pCurrLODs->iFlags &= ~GGTERRAIN_LOD_GENERATING; // terrain is not looking correct after this.
				ggterrain_global_params.bForceUpdate = 1 - ggterrain_global_params.bForceUpdate;
			}
		}
		pCurrLODs = ggterrain.GetCurrentLODs();
	}
	else
	{
		// replace with approach that does not wait inside a lock:
		// HOWEVER THIS MESSES UP TERRAIN FOR NOW!!!
		// --- only hold the lock for the minimum time ---
		terrainlock.lock();
		if (ggterrain_update_enabled)
		{
			ggterrain.CheckParams();
			ggterrain.UpdateChunks(playerX, playerZ);

			// Grab pointer/state while protected:
			pCurrLODs = ggterrain.GetCurrentLODs();
		}
		terrainlock.unlock();
		// --- any waiting happens OUTSIDE the lock ---
		if (pCurrLODs)
		{
			uint32_t timeout = 0;
			while (pCurrLODs->IsGenerating() &&
				!pCurrLODs->pLevels[pCurrLODs->GetNumLevels() - 1].IsReady() &&
				timeout++ < 300)
			{
				Sleep(0); // yield, don't force a 1ms sleep
				// If you want to keep exactly the old behavior, use Sleep(1) here instead.
			}
			if (timeout >= 300)
			{
				// If other threads can touch iFlags, protect this write too:
				terrainlock.lock();
				pCurrLODs->iFlags &= ~GGTERRAIN_LOD_GENERATING;
				terrainlock.unlock();
				ggterrain_global_params.bForceUpdate = 1 - ggterrain_global_params.bForceUpdate;
			}
		}
		if (pCurrLODs == nullptr) pCurrLODs = ggterrain.GetCurrentLODs();
	}

	// and continue
	GGTerrainLODSet* pNewLODs = ggterrain.GetNewLODs();

	// update terrain constants
	terrainConstantData.terrain_numLODLevels = pCurrLODs->GetNumLevels();
	for( uint32_t i = 0; i < pCurrLODs->GetNumLevels(); i++ )
	{
		float LODHalfSize = ggterrain_local_params.segments_per_chunk * pCurrLODs->pLevels[ i ].segSize * 4;
		terrainConstantData.terrain_LOD[ i ].x = pCurrLODs->pLevels[ i ].centerX - LODHalfSize;
		terrainConstantData.terrain_LOD[ i ].z = pCurrLODs->pLevels[ i ].centerZ - LODHalfSize;
		terrainConstantData.terrain_LOD[ i ].size = 0.5f / LODHalfSize;
	}

	terrainConstantData.terrain_baseLayerMaterial = ggterrain_local_render_params.baseLayerMaterial;
 	for( int i = 0; i < 5; i++ )
	{
		terrainConstantData.terrain_layers[ i ].start = ggterrain_local_render_params.layerStartHeight[ i ];
		terrainConstantData.terrain_layers[ i ].transition = 1.0f / (ggterrain_local_render_params.layerEndHeight[i] - ggterrain_local_render_params.layerStartHeight[i]);
		terrainConstantData.terrain_layers[ i ].material = ggterrain_local_render_params.layerMatIndex[ i ];
	}

	for( int i = 0; i < 2; i++ )
	{
		terrainConstantData.terrain_slopes[ i ].start = ggterrain_local_render_params.slopeStart[ i ];
		terrainConstantData.terrain_slopes[ i ].transition = 1.0f / (ggterrain_local_render_params.slopeEnd[i] - ggterrain_local_render_params.slopeStart[i]);
		terrainConstantData.terrain_slopes[ i ].material = ggterrain_local_render_params.slopeMatIndex[ i ];
	}
	terrainConstantData.terrain_bumpiness = ggterrain_local_render_params.bumpiness;
	terrainConstantData.terrain_reflectance = ggterrain_global_render_params2.reflectance;
	terrainConstantData.terrain_textureGamma = ggterrain_global_render_params2.textureGamma;
	terrainConstantData.terrain_detailLimit = (float) ggterrain_global_render_params2.detailLimit;

	float detailScale = 1.0f;
	uint32_t screenWidth = master.masterrenderer.GetWidth3D();
	if ( screenWidth > 2560 )
	{
		ggterrain_global_render_params2.detailScale = 1.42f - screenWidth * 0.0001628f; // carefully chosen to equal 1.0 at 2560
	}
	
	terrainConstantData.terrain_detailScale = 1.0f / (detailScale * ggterrain_global_render_params2.detailScale);
	terrainConstantData.terrain_flags = ggterrain_local_render_params.flags | ggterrain_local_render_params2.flags2;

	bool hideBrush = false;
	if (!Get_Spray_Mode_On())
	{
		switch (ggterrain_extra_params.edit_mode)
		{
		case GGTERRAIN_EDIT_NONE: hideBrush = true; break;
		case GGTERRAIN_EDIT_TREES:
		{
			if (!GGTrees_UsingBrush()) hideBrush = true;
		} break;
		}
	}
	if ( hideBrush ) terrainConstantData.terrain_flags &= ~GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;

	terrainConstantData.terrain_maskScale = ggterrain_local_render_params.maskScale / 50000.0f;
	terrainConstantData.terrain_mapEditSize = ggterrain_global_render_params2.editable_size;

	float maskAng = 0.1963495f; // (2.0 * pi) / 32.0
	for( int i = 0; i < 32; i++ )
	{
		float c = cos( maskAng * i );
		float s = sin( maskAng * i );
		terrainConstantData.terrain_maskRotMat[ i + 32 ].x = c;
		terrainConstantData.terrain_maskRotMat[ i + 32 ].y = -s;
		terrainConstantData.terrain_maskRotMat[ i + 32 ].z = s;
		terrainConstantData.terrain_maskRotMat[ i + 32 ].w = c;
	}

	float maskAngSmall = maskAng * 0.05f;
	for( int i = 0; i < 32; i++ )
	{
		float c = cos( maskAngSmall * (i - 16) );
		float s = sin( maskAngSmall * (i - 16) );
		terrainConstantData.terrain_maskRotMat[ i ].x = c;
		terrainConstantData.terrain_maskRotMat[ i ].y = -s;
		terrainConstantData.terrain_maskRotMat[ i ].z = s;
		terrainConstantData.terrain_maskRotMat[ i ].w = c;
	}

	terrainConstantData.terrain_readBackReduction = ggterrain_local_render_params2.readBackTextureReduction;

	wiInput::MouseState mouseState = wiInput::GetMouseState();
	ggterrain_internal_params.mouseLeftState = mouseState.left_button_press;
	ggterrain_internal_params.mouseLeftPressed = (mouseState.left_button_press && !ggterrain_internal_params.prevMouseLeft) ? 1 : 0;
	ggterrain_internal_params.mouseLeftReleased = (!mouseState.left_button_press && ggterrain_internal_params.prevMouseLeft) ? 1 : 0;
	ggterrain_internal_params.prevMouseLeft = mouseState.left_button_press ? 1 : 0;

	float pickX = 0, pickY = 0, pickZ = 0;
	RAY pickRay = wiRenderer::GetPickRay( (long)mouseState.position.x, (long)mouseState.position.y, master.masterrenderer );

	int includeFlatAreas = 1;
	if ( ggterrain_extra_params.edit_mode == 5 || ggterrain_extra_params.edit_mode == 6 ) includeFlatAreas = 0;

	int pickHit = 0;
	if ( !ggterrain_internal_params.mouseLeftState || ggterrain_extra_params.edit_mode == GGTERRAIN_EDIT_TREES || ggterrain_extra_params.edit_mode == GGTERRAIN_EDIT_GRASS )
	{
		pickHit = GGTerrain_RayCast( pickRay, &pickX, &pickY, &pickZ, 0, 0, 0, 0, includeFlatAreas );
	}
	else
	{
		if ( ggterrain_internal_params.mouseLeftPressed )
		{
			// initial mouse press, cast ray with terrain
			pickHit = GGTerrain_RayCast( pickRay, &pickX, &pickY, &pickZ, 0, 0, 0, 0, includeFlatAreas );

			ggterrain_internal_params.sculpt_pick_y = pickY;
		}
		else
		{
			float vy = ggterrain_internal_params.sculpt_pick_y - pickRay.origin.y;

			if ( vy >= -0.1 || ggterrain_extra_params.edit_pick_mode == 0 )
			{
				pickHit = GGTerrain_RayCast( pickRay, &pickX, &pickY, &pickZ, 0, 0, 0, 0, includeFlatAreas );
			}
			else
			{
				// subsequent mouse button down, cast ray with plane
				float dotp = vy * pickRay.direction.y;
				if ( dotp > 0.000001f )
				{
					float dist = (vy*vy) / dotp;

					pickX = pickRay.origin.x + pickRay.direction.x * dist;
					pickY = ggterrain_internal_params.sculpt_pick_y;
					pickZ = pickRay.origin.z + pickRay.direction.z * dist;
			
					pickHit = 1;
				}
			}
		}
	}	
		
	terrainConstantData.terrain_brushSize = ggterrain_local_render_params2.brushSize;

	/*if ( pickHit && !bImGuiGotFocus )*/
	if (pickHit && bRenderTargetFocus && !bImGuiGotFocus)
	{
		float sizeX = 1000.0f;
		float sizeZ = 2000.0f;

		switch( ggterrain_extra_params.edit_mode )
		{
			case GGTERRAIN_EDIT_SCULPT:
			{
				if ( ggterrain_extra_params.sculpt_mode != GGTERRAIN_SCULPT_NONE ) 
				{
					if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_PICK) terrainConstantData.terrain_brushSize = 100;
			
					if ( !pCurrLODs->IsGenerating() && !pNewLODs->IsGenerating() )
					{
						GGTerrain_Update_Sculpting( pickX, pickY, pickZ );
						vLastTerrainPickPosition.x = pickX;
						vLastTerrainPickPosition.y = pickY;
						vLastTerrainPickPosition.z = pickZ;
					}
				}
			} break;

			case GGTERRAIN_EDIT_PAINT: 
			{
#ifdef ONLYLOADWHENUSED
				int mat = ggterrain_extra_params.paint_material & 0xff;
				if (mat > 0 && mat <= GGTERRAIN_MAX_SOURCE_TEXTURES)
					mat--;
				if (mat >= 0 && !bTextureUploaded[mat])
				{
					bCheckForNewTerrainTextures = true; //PE: Load texture.
				}
#endif
				GGTerrain_Update_Painting( pickX, pickY, pickZ );
			} break;

			case 5:
			{
				if ( ggterrain_internal_params.mouseLeftPressed )
				{
					//GGTerrain_AddFlatCircle( pickX, pickZ, terrainConstantData.terrain_brushSize*2 );
					GGTerrain_AddFlatRect( pickX, pickZ, sizeX, sizeZ, ggterrain_extra_params.flat_area_angle );
				}
			} break;

			case 6:
			{
				if ( ggterrain_internal_params.mouseLeftState )
				{
					GGTerrain_UpdateFlatArea( 1, pickX, pickZ, ggterrain_extra_params.flat_area_angle, sizeX, sizeZ );
				}
			} break;
		}
	}

	if ( pickHit )
	{
		terrainConstantData.terrain_mouseHit.x = pickX;
		terrainConstantData.terrain_mouseHit.y = pickZ;

		#ifdef FULLTERRAINEDITING
		if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_PICK) terrainConstantData.terrain_brushSize = 100;
		else terrainConstantData.terrain_brushSize = ggterrain_local_render_params2.brushSize;
		if (ggterrain_extra_params.edit_mode == GGTERRAIN_EDIT_TREES && ggtrees_global_params.paint_mode == GGTREES_PAINT_ADD) terrainConstantData.terrain_brushSize = 25.0f;
		#else
		terrainConstantData.terrain_brushSize = 0;
		#endif
		
		// ramp world matrix
		if ( ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_RAMP )
		{
			float startX = ggterrain_internal_params.sculpt_prev_ramp_x;
			float startZ = ggterrain_internal_params.sculpt_prev_ramp_z;
			float endX = pickX;
			float endZ = pickZ;

			float startY = 0;
			GGTerrain_GetHeight( startX, startZ, &startY );
			
			float endY = 0;
			GGTerrain_GetHeight( endX, endZ, &endY );

			float diffX = endX - startX;
			float diffY = endY - startY;
			float diffZ = endZ - startZ;
			float rampLength = sqrt( diffX*diffX + diffY*diffY + diffZ*diffZ );

			XMFLOAT3 temp;
			temp.x = ggterrain_local_render_params2.brushSize * 0.85f;
			temp.y = rampLength / 2.0f;
			temp.z = 15.0f;
			XMVECTOR vScale = XMLoadFloat3( &temp );
			temp.x = startX + diffX / 2.0f;
			temp.y = startY + diffY / 2.0f;
			temp.z = startZ + diffZ / 2.0f;
			XMVECTOR vPos = XMLoadFloat3( &temp );
			float angY = atan2( diffX, diffZ );// + 3.14159265f;
			float angX = acos( diffY / rampLength );
			XMMATRIX matWorld = XMMatrixScalingFromVector(vScale) 
				              * XMMatrixRotationX( angX ) 
							  * XMMatrixRotationY( angY )
							  *	XMMatrixTranslationFromVector(vPos);

			XMStoreFloat4x4( &terrainConstantData.terrain_rampWorldMat, matWorld );
		}
	}

	wiRenderer::GetDevice()->UpdateBuffer( &terrainConstantBuffer, &terrainConstantData, cmd, sizeof(TerrainCB) );

	if ( !pCurrLODs->IsGenerating() )
	{
		// CPU only side of the read back, GPU side is done in prepass render
		GGTerrain_CheckPageShift();
		GGTerrain_CheckReadBack(cmd); 
	}

	for( int i = 0; i < (int)pageRefreshList.NumItems(); i++ )
	{
		if ( pageGenerationList.NumItems() >= GGTERRAIN_REPLACEMENT_PAGE_MAX ) break;

		PageEntry* pPage = pageRefreshList[ i ];
		if ( pPage->ShouldRegenerate() )
		{
			pPage->Regenerated();
			pageGenerationList.AddItem( pPage );
			pageRefreshList[ i ] = 0;
		}
	}
	pageRefreshList.RemoveGaps();

	GGTerrain_DrawPages( cmd );

	#ifdef GGTERRAIN_UNDOREDO
	if (ggterrain_internal_params.mouseLeftReleased && g_iCalculatingChangeBounds)
	{
		// Create an undo item here.
		if(ggterrain_extra_params.edit_mode != GGTERRAIN_EDIT_GRASS)
			g_iCalculatingChangeBounds = 0;
		
		int type = 0;
		if (ggterrain_extra_params.edit_mode == GGTERRAIN_EDIT_SCULPT)
			type = eUndoSys_Terrain_Sculpt;
		else if (ggterrain_extra_params.edit_mode == GGTERRAIN_EDIT_PAINT)
			type = eUndoSys_Terrain_Paint;
		GGTerrain_CreateUndoRedoAction(type, eUndoSys_UndoList);
	}
	#endif
}

void GGTerrain_Update_EmptyLevel(float playerX, float playerY, float playerZ, wiGraphics::CommandList cmd)
{
	// most basic transfers of values
	terrainlock.lock();
	if (ggterrain_update_enabled)
	{
		ggterrain.CheckParams();
	}
	terrainlock.unlock();

	// also need to mighrate settings related to the editbox for seeing extent of mapsize
	float detailScale = 1.0f;
	uint32_t screenWidth = master.masterrenderer.GetWidth3D();
	if (screenWidth > 2560) ggterrain_global_render_params2.detailScale = 1.42f - screenWidth * 0.0001628f; // carefully chosen to equal 1.0 at 2560
	terrainConstantData.terrain_detailScale = 1.0f / (detailScale * ggterrain_global_render_params2.detailScale);
	terrainConstantData.terrain_flags = ggterrain_local_render_params.flags | ggterrain_local_render_params2.flags2;
	terrainConstantData.terrain_maskScale = ggterrain_local_render_params.maskScale / 50000.0f;
	terrainConstantData.terrain_mapEditSize = ggterrain_global_render_params2.editable_size;
	wiRenderer::GetDevice()->UpdateBuffer(&terrainConstantBuffer, &terrainConstantData, cmd, sizeof(TerrainCB));

	// also need to handle env probe refreshes - Environmental Light Probe System
	GGTerrain_EnvProbeWork(playerX, playerY, playerZ);
}

int GGTerrain_IsReady()
{
	GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
	if (!pCurrLODs->IsValid() || pCurrLODs->IsGenerating())
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int GGTerrain_RayCast( RAY pickRay, float* pOutX, float* pOutY, float* pOutZ, float* pNormX, float* pNormY, float* pNormZ, uint32_t* lodLevel, int includeFlatAreas )
{
	return ggterrain.RayCast( pickRay, pOutX, pOutY, pOutZ, pNormX, pNormY, pNormZ, lodLevel, includeFlatAreas );
}

int GGTerrain_GetHeight( float x, float z, float* outHeight, int accurateButSlow, int includeFlatAreas )
{
	if (!ggterrain_update_enabled) return(0);
	if ( !accurateButSlow ) return ggterrain.GetHeight( x, z, outHeight, includeFlatAreas );
	else
	{
		if ( !outHeight ) return 0;
		float height;
		float heightNoFlat = 0;
		height = GGTerrainChunk::CalculateHeightWithHeightmap( x, z, 1, &heightNoFlat );
		if ( includeFlatAreas ) *outHeight = height;
		else *outHeight = heightNoFlat;
		return 1;
	}
}

int GGTerrain_GetNormal( float x, float z, float* outNx, float* outNy, float* outNz )
{
	return ggterrain.GetNormal( x, z, outNx, outNy, outNz );
}

void GGTerrain_CancelRamp()
{
	ggterrain_internal_params.sculpt_prev_ramp_valid = false;
}

void GGTerrain_ResetSculpting()
{
	for( int i = 0; i < GGTERRAIN_HEIGHTMAP_EDIT_SIZE * GGTERRAIN_HEIGHTMAP_EDIT_SIZE; i++ )
	{
		pHeightMapEdit[ i ] = 0;
		pHeightMapEditType[ i ] = 0;
	}

	GGTerrain_InvalidateEverything( GGTERRAIN_INVALIDATE_ALL );
}

bool GGTerrain_GetTerrainChanged()
{
	bool bResult = ggterrain_extra_params.bTerrainChanged;
	ggterrain_extra_params.bTerrainChanged = false;
	return bResult;
}

void GGTerrain_DebugOutputFlattenedAreas(void)
{
	timestampactivity(0, "List of Auto Flattened Areas");
	for (uint32_t i = 0; i < ggterrain_flat_areas_array_size; i++)
	{
		char areaitem[1024];
		GGTerrainFlatArea* pArea = &ggterrain_flat_areas[i];
		if (!pArea->IsValid())
		{
			sprintf(areaitem, "Area %d : Invalid", i);
		}
		else
		{
			if (isnan(pArea->y))
			{
				sprintf(areaitem, "Area %d : isNAN", i);
			}
			else
			{
				// full area item view
				sprintf(areaitem, "Area %d : DATA=%d Y=%d", i, (int)pArea->data, (int)pArea->y);
			}
		}
		timestampactivity(0, areaitem);
	}
}

// returns the editable size in world units, use this with GGTerrain_GetHeightMap to work out the scale of the heightmap to world size
// the edtiable area is always square, so both X and Z use editableSize
float GGTerrain_GetEditableSize() 
{
	return ggterrain_local_render_params2.editable_size;
}

// Will fill the given pHeightMap array with height values in the editable area
// The height map will be filled with all the X values for Z = 0, then all the X values for Z = 1, and so on
// You can choose the amount of detail you want using the sizeX and sizeZ parameters
// pHeightMap must be allocated with at least sizeX * sizeZ * sizeof(float) bytes
// If this returns 1 then the height map was successfully populated, otherwise you should ignore it and try again later
int GGTerrain_GetHeightMapEx( uint32_t sizeX, uint32_t sizeZ, float* pHeightMap, bool bUseOriginalLOD )
{
	if ( !GGTerrain_IsReady() ) return 0;

	for( uint32_t z = 0; z < sizeZ; z++ )
	{
		float fZ = (float) z; 
		fZ /= (sizeZ - 1);
		fZ = fZ * 2 - 1;
		fZ *= ggterrain_local_render_params2.editable_size;

		for( uint32_t x = 0; x < sizeX; x++ )
		{
			uint32_t index = z * sizeX + x;

			float fX = (float) x;
			fX /= (sizeX - 1);
			fX = fX * 2 - 1;
			fX *= ggterrain_local_render_params2.editable_size;

			if (bUseOriginalLOD == true)
			{
				// slower but need the actual height data!
				GGTerrain_GetHeight (fX, fZ, &(pHeightMap[index]), 1, 1);
			}
			else
			{
				ggterrain.GetHeight(fX, fZ, &(pHeightMap[index]), 1);
			}
		}
	}
	
	return 1;
}
int GGTerrain_GetHeightMap(uint32_t sizeX, uint32_t sizeZ, float* pHeightMap)
{
	return GGTerrain_GetHeightMapEx(sizeX, sizeZ, pHeightMap, false);
}

uint32_t GGTerrain_GetSculptDataSize()
{
	return GGTERRAIN_HEIGHTMAP_EDIT_SIZE * GGTERRAIN_HEIGHTMAP_EDIT_SIZE * (sizeof(float) + sizeof(uint8_t));
}

int GGTerrain_GetSculptData( uint8_t* data )
{
	uint32_t size1 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE * GGTERRAIN_HEIGHTMAP_EDIT_SIZE * sizeof(uint8_t);
	memcpy( data, pHeightMapEditType, size1 );

	uint32_t size2 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE * GGTERRAIN_HEIGHTMAP_EDIT_SIZE * sizeof(float);
	memcpy( data+size1, pHeightMapEdit, size2 );

	return 1;
}

int GGTerrain_SetSculptData( uint32_t size, uint8_t* data, sUndoSysEventTerrainSculpt* sculptEvent )
{
	if (!ggterrain_initialised) return 0;

	if (!sculptEvent)
	{
		uint32_t size1 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE * GGTERRAIN_HEIGHTMAP_EDIT_SIZE * sizeof(uint8_t);
		uint32_t size2 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE * GGTERRAIN_HEIGHTMAP_EDIT_SIZE * sizeof(float);

		if (size1 + size2 != size) return 0;
		memcpy(pHeightMapEditType, data, size1);
		memcpy(pHeightMapEdit, data + size1, size2);

		GGTerrain_InvalidateEverything( GGTERRAIN_INVALIDATE_ALL );

		return 1;
	}
	
	#ifdef GGTERRAIN_UNDOREDO
	for (int y = sculptEvent->minY; y < sculptEvent->maxY; y++)
	{
		for (int x = sculptEvent->minX; x < sculptEvent->maxX; x++)
		{
			//int indexHeightMap = GGTERRAIN_HEIGHTMAP_EDIT_SIZE * y + x;
			int indexHeightMap = (GGTERRAIN_HEIGHTMAP_EDIT_SIZE - 1 - y) * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + x;
			int indexSavedData = (sculptEvent->maxX - sculptEvent->minX) * (y - sculptEvent->minY) + (x - sculptEvent->minX);
			pHeightMapEdit[indexHeightMap] = sculptEvent->editData[indexSavedData];
			pHeightMapEditType[indexHeightMap] = sculptEvent->typeData[indexSavedData];
		}
	}

	float realMinX = sculptEvent->minX / (float)GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
	realMinX = realMinX * 2 - 1;
	realMinX *= ggterrain_local_render_params2.editable_size;
	float realMinZ = sculptEvent->minY / (float)GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
	realMinZ = realMinZ * 2 - 1;
	realMinZ *= ggterrain_local_render_params2.editable_size;
	float realMaxX = sculptEvent->maxX / (float)GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
	realMaxX = realMaxX * 2 - 1;
	realMaxX *= ggterrain_local_render_params2.editable_size;
	float realMaxZ = sculptEvent->maxY / (float)GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
	realMaxZ = realMaxZ * 2 - 1;
	realMaxZ *= ggterrain_local_render_params2.editable_size;

	GGTerrain_InvalidateRegion(realMinX, realMinZ, realMaxX, realMaxZ, GGTERRAIN_INVALIDATE_ALL);

	return 1;
	#endif
}

uint32_t GGTerrain_GetPaintDataSize()
{
	return GGTERRAIN_MATERIALMAP_SIZE * GGTERRAIN_MATERIALMAP_SIZE * sizeof(uint8_t);
}

int GGTerrain_GetPaintData( uint8_t* data )
{
	uint32_t size = GGTERRAIN_HEIGHTMAP_EDIT_SIZE * GGTERRAIN_HEIGHTMAP_EDIT_SIZE * sizeof(uint8_t);
	memcpy( data, pMaterialMap, size );

	return 1;
}

int GGTerrain_SetPaintData( uint32_t size, uint8_t* data, sUndoSysEventTerrainPaint* pPaintEvent)
{
	if (!ggterrain_initialised) return 0;
	if (!pPaintEvent)
	{
		uint32_t size1 = GGTERRAIN_HEIGHTMAP_EDIT_SIZE * GGTERRAIN_HEIGHTMAP_EDIT_SIZE * sizeof(uint8_t);

		if (size1 != size) return 0;
		memcpy(pMaterialMap, data, size1);

		//PE: Was needed so we can invalidate region and clear old textures.
		wiRenderer::GetDevice()->UpdateTexture(&texMaterialMap, 0, 0, NULL, pMaterialMap, GGTERRAIN_MATERIALMAP_SIZE, -1);

#ifdef ONLYLOADWHENUSED
		bCheckForNewTerrainTextures = true;
#endif

		GGTerrain_InvalidateEverything(GGTERRAIN_INVALIDATE_TEXTURES);

		return 1;
	}

#ifdef GGTERRAIN_UNDOREDO
	for (int y = pPaintEvent->minY; y < pPaintEvent->maxY; y++)
	{
		for (int x = pPaintEvent->minX; x < pPaintEvent->maxX; x++)
		{
			int indexHeightMap = y * GGTERRAIN_HEIGHTMAP_EDIT_SIZE + x;
			int indexSavedData = (pPaintEvent->maxX - pPaintEvent->minX) * (y - pPaintEvent->minY) + (x - pPaintEvent->minX);
			pMaterialMap[indexHeightMap] = pPaintEvent->materialData[indexSavedData];
		}
	}

	float realMinX = pPaintEvent->minX / (float)GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
	realMinX = realMinX * 2 - 1;
	realMinX *= ggterrain_local_render_params2.editable_size;
	float realMinZ = pPaintEvent->minY / (float)GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
	realMinZ = realMinZ * 2 - 1;
	realMinZ *= ggterrain_local_render_params2.editable_size;
	float realMaxX = pPaintEvent->maxX / (float)GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
	realMaxX = realMaxX * 2 - 1;
	realMaxX *= ggterrain_local_render_params2.editable_size;
	float realMaxZ = pPaintEvent->maxY / (float)GGTERRAIN_HEIGHTMAP_EDIT_SIZE;
	realMaxZ = realMaxZ * 2 - 1;
	realMaxZ *= ggterrain_local_render_params2.editable_size;

	GGTerrain_InvalidateRegion(realMinX, realMinZ, realMaxX, realMaxZ, GGTERRAIN_INVALIDATE_ALL);
	wiRenderer::GetDevice()->UpdateTexture(&texMaterialMap, 0, 0, NULL, pMaterialMap, GGTERRAIN_MATERIALMAP_SIZE, -1);

	return 1;
#endif
	
}

void GGTerrain_ResetPaintData()
{
	memset( pMaterialMap, 0, GGTERRAIN_MATERIALMAP_SIZE * GGTERRAIN_MATERIALMAP_SIZE );
	wiRenderer::GetDevice()->UpdateTexture(&texMaterialMap, 0, 0, NULL, pMaterialMap, GGTERRAIN_MATERIALMAP_SIZE, -1);

	GGTerrain_InvalidateEverything( GGTERRAIN_INVALIDATE_TEXTURES );
}


int GGTerrain_GetMaterialIndex( float x, float z )
{
	terrainlock.lock();
	float fX = x / ggterrain_local_render_params2.editable_size;
	fX = fX * 0.5f + 0.5f;
	fX *= GGTERRAIN_MATERIALMAP_SIZE;

	float fZ = z / ggterrain_local_render_params2.editable_size;
	fZ = fZ * 0.5f + 0.5f;
	fZ *= GGTERRAIN_MATERIALMAP_SIZE;

	int iX = (int) fX;
	int iZ = (int) fZ;

	if ( iX >= 0 && iZ >= 0 && iX < GGTERRAIN_MATERIALMAP_SIZE && iZ < GGTERRAIN_MATERIALMAP_SIZE )
	{
		uint32_t index = iZ * GGTERRAIN_MATERIALMAP_SIZE + iX;
		if (pMaterialMap[index] > 0)
		{
			terrainlock.unlock();
			return pMaterialMap[index] - 1;
		}
	}

	// calculate material from height and slope
	float normalY;
	if (!GGTerrain_GetNormal(x, z, 0, &normalY, 0))
	{
		terrainlock.unlock();
		return 31;
	}
	normalY = 1 - abs(normalY);

	for( int i = 0; i < 2; i++ )
	{
		float slopeStart = ggterrain_local_render_params.slopeStart[ i ];
		float transition = 1.0f / (ggterrain_local_render_params.slopeEnd[ i ] - ggterrain_local_render_params.slopeStart[ i ]);
		float t = (normalY - slopeStart) * transition;
		if (t > 0.5)
		{
			terrainlock.unlock();
			return ggterrain_local_render_params.slopeMatIndex[i] & 0xFF;
		}
	}

	float height = 0;
	GGTerrain_GetHeight( x, z, &height );
	for( int i = 4; i >= 0; i-- )
	{
		float start = ggterrain_local_render_params.layerStartHeight[ i ];
		float transition = 1.0f / (ggterrain_local_render_params.layerEndHeight[ i ] - ggterrain_local_render_params.layerStartHeight[ i ]);
		float t = (height - start) * transition;
		if (t > 0.45)
		{
			terrainlock.unlock();
			return ggterrain_local_render_params.layerMatIndex[i] & 0xFF;
		}
	}
	terrainlock.unlock();
	return ggterrain_local_render_params.baseLayerMaterial & 0xFF;
}

extern "C" void GGTerrain_VirtualTexReadBack(Texture texReadBack, uint32_t sampleCount, wiGraphics::CommandList cmd)
{
	if (!ggterrain_update_enabled) return;
	if (!ggterrain_initialised) return;
	if (!ggterrain_draw_enabled) return;

	GraphicsDevice* device = wiRenderer::GetDevice();

	device->EventBegin("VirtTexReadBackDownSample", cmd);
	auto range = wiProfiler::BeginRangeGPU("Terrain Read Back Downsample", cmd);

	device->BindResource(CS, &texReadBack, 50, cmd);
	device->BindUAV(CS, &texReadBackCompute, 0, cmd);

	const TextureDesc& desc = texReadBackCompute.GetDesc();

	if (sampleCount > 1) device->BindComputeShader(&shaderReadBackMSCS, cmd);
	else device->BindComputeShader(&shaderReadBackCS, cmd);

	device->BindConstantBuffer(CS, &terrainConstantBuffer, 2, cmd);

	device->Dispatch((desc.Width + 7) / 8, (desc.Height + 7) / 8, 1, cmd);

	device->UnbindResources(50, 1, cmd);
	device->UnbindUAVs(0, 1, cmd);

	// --- pick slot index ONCE, without mutating atomic weirdly ---
	const uint32_t slot = g_currReadBackTex.fetch_add(1, std::memory_order_relaxed) % NUM_READ_BACK_TEXTURES;

	device->CopyResource(&texReadBackStaging[slot], &texReadBackCompute, cmd);

	// --- timestamp gating (safe for deferred contexts) ---
	ID3D11DeviceContext* ctx = (ID3D11DeviceContext*)wiRenderer::GetDevice()->GetDeviceContext(cmd);

	if (g_readbackDisjointQuery)
		ctx->Begin(g_readbackDisjointQuery);

	ID3D11Query* tsq = g_readbackTimestampQuery[slot];
	if (tsq)
		ctx->End(tsq); // TIMESTAMP query is written by End()

	if (g_readbackDisjointQuery)
		ctx->End(g_readbackDisjointQuery);

	g_lastQueuedReadbackIndex.store(slot, std::memory_order_release);

	wiProfiler::EndRange(range);
	device->EventEnd(cmd);
}

void GGTerrain_DrawQuad( RenderPass* renderPass, CommandList cmd )
{
	if (!ggterrain_update_enabled) return;
	if ( !ggterrain_initialised ) return;

	GraphicsDevice* device = wiRenderer::GetDevice();

	device->RenderPassBegin( renderPass, cmd );

	Viewport vp;
	vp.Width = (float) renderPass->GetDesc().attachments[0].texture->GetDesc().Width;
	vp.Height = (float) renderPass->GetDesc().attachments[0].texture->GetDesc().Height;
	device->BindViewports( 1, &vp, cmd );

	device->BindPipelineState( &psoQuad, cmd );
			
	const GPUBuffer* vbs[] = { &quadVertexBuffer };
	uint32_t stride = sizeof( VertexQuad );
	device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );
	
	// bind samplers
	device->BindSampler( PS, &samplerBilinear, 0, cmd );

	device->Draw( 6, 0, cmd );

	device->RenderPassEnd( cmd );
}

// must be extern "C" to allow /alternatename linker flag to be set correctly
// called from WickedEngine RenderPath3D::Render()
extern "C" void GGTerrain_Draw_Prepass( const Frustum* frustum, CommandList cmd )
{
	if (!ggterrain_update_enabled) return;
	if (!ggterrain_initialised) return;

	// must not do any quad rendering in here
	if ( !ggterrain_initialised ) return;
	if ( !ggterrain_draw_enabled ) return;
	if ( !ggterrain.IsValid() ) return;
	if ( ggterrain_render_wireframe ) return;

	GraphicsDevice* device = wiRenderer::GetDevice();
	device->EventBegin("GGTerrain Prepass Draw", cmd);
		
	device->BindPipelineState( &psoMainPrepass, cmd );

	int bindSlot = 2;
	device->BindConstantBuffer( VS, &terrainConstantBuffer, bindSlot, cmd );
	device->BindConstantBuffer( PS, &terrainConstantBuffer, bindSlot, cmd );

	device->BindResource( PS, &texPageTableArray, 53, cmd );
	device->BindResource( PS, &texPageTableFinal, 54, cmd );

#if (GGTERRAIN_TEXTURE_FILTERING == GGTERRAIN_TEXTURE_FILTERING_TRILINEAR)
	device->BindSampler( PS, &samplerTrilinearWrap, 0, cmd );
#else
	device->BindSampler( PS, &samplerAnisotropicWrap, 0, cmd );
#endif

	uint32_t numSegments = ggterrain_local_params.segments_per_chunk;
	uint32_t numIndices = (numSegments*2 + 3) * numSegments - 1;
	
	GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
	for( uint32_t lod = 0; lod < pCurrLODs->GetNumLevels(); lod++ )
	{
		for( uint32_t i = 0; i < 64; i++ )
		{
			GGTerrainChunk* pChunk = pCurrLODs->pLevels[ lod ].chunkGrid[ i ];
			if ( !pChunk || pChunk->IsGenerating() ) continue;
			if ( !pChunk->IsVisible() )
			{
				// if the next lod level down is still generating then show it anyway
				if ( lod == 0 || !pCurrLODs->pLevels[ lod-1 ].IsGenerating() ) continue;
			}

			const AABB* aabb = pChunk->GetBounds();
			if ( !frustum->CheckBoxFast( *aabb ) ) continue;

			const GPUBuffer* vbs[] = { &pChunk->vertexBuffer };
			uint32_t stride = sizeof( TerrainVertex );
			device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );
			device->BindIndexBuffer( &pChunk->indexBuffer, INDEXFORMAT_16BIT, 0, cmd );
			device->DrawIndexed( numIndices, 0, 0, cmd );
		}
	}

	device->EventEnd(cmd);

#ifdef GGTERRAIN_ENABLE_PBR_SPHERES
	device->EventBegin("Sphere Prepass Draw", cmd);
	device->BindPipelineState( &psoSpherePrepass, cmd );
	const GPUBuffer* vbs[] = { &sphereVertexBuffer };
	uint32_t stride = sizeof( VertexSphere );
	device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );
	device->BindIndexBuffer( &sphereIndexBuffer, INDEXFORMAT_16BIT, 0, cmd );
	device->DrawIndexedInstanced( g_numIndicesSphere, 23, 0, 0, 0, cmd );		
	device->EventEnd(cmd);
#endif
}

#define GGTERRAIN_REFLECTION_LOWEST_LOD 2

// must be extern "C" to allow /alternatename linker flag to be set correctly
// called from WickedEngine RenderPath3D::Render()
extern "C" void GGTerrain_Draw_Prepass_Reflections( const Frustum* frustum, CommandList cmd )
{
	if (!ggterrain_update_enabled) return;
	if (!ggterrain_initialised) return;

	// must not do any quad rendering in here
	if ( !ggterrain_initialised ) return;
	if ( !ggterrain_draw_enabled ) return;
	if ( !ggterrain.IsValid() ) return;
	if ( ggterrain_render_wireframe ) return;

	GraphicsDevice* device = wiRenderer::GetDevice();
	device->EventBegin("GGTerrain Prepass Reflections Draw", cmd);
		
	device->BindPipelineState( &psoReflectionPrepass, cmd );

	GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();

	uint32_t numSegments = ggterrain_local_params.segments_per_chunk;
	uint32_t numIndices = (numSegments*2 + 3) * numSegments - 1;
	
	for( uint32_t lod = GGTERRAIN_REFLECTION_LOWEST_LOD; lod < pCurrLODs->GetNumLevels(); lod++ )
	{
		for( uint32_t i = 0; i < 64; i++ )
		{
			GGTerrainChunk* pChunk = pCurrLODs->pLevels[ lod ].chunkGrid[ i ];
			if ( !pChunk || pChunk->IsGenerating() ) continue;
			if ( !pChunk->IsVisible() )
			{
				// if the next lod level down is still generating then show it anyway
				if ( lod > GGTERRAIN_REFLECTION_LOWEST_LOD && !pCurrLODs->pLevels[ lod-1 ].IsGenerating() ) continue;
			}

			const AABB* aabb = pChunk->GetBounds();
			if ( !frustum->CheckBoxFast( *aabb ) ) continue;

			const GPUBuffer* vbs[] = { &pChunk->vertexBuffer };
			uint32_t stride = sizeof( TerrainVertex );
			device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );
			device->BindIndexBuffer( &pChunk->indexBuffer, INDEXFORMAT_16BIT, 0, cmd );
			device->DrawIndexed( numIndices, 0, 0, cmd );
		}
	}
	
	device->EventEnd(cmd);
}

// must be extern "C" to allow /alternatename linker flag to be set correctly
// called from WickedEngine wiRenderer::DrawShadowmaps()
extern "C" void GGTerrain_Draw_ShadowMap( const Frustum* frustum, int cascade, CommandList cmd )
{
	if (!ggterrain_update_enabled) return;
	if ( !ggterrain_initialised ) return;
	if ( !ggterrain_draw_enabled ) return;
	if ( !ggterrain.IsValid() ) return;
	if ( ggterrain_render_wireframe ) return;

	GraphicsDevice* device = wiRenderer::GetDevice();
	device->EventBegin("GGTerrain Shadow Draw", cmd);

	device->BindPipelineState( &psoMainShadow, cmd );
	
	GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
	uint32_t numSegments = ggterrain_local_params.segments_per_chunk;
	uint32_t numIndices = (numSegments*2 + 3) * numSegments - 1;

	uint32_t numLODLevels = pCurrLODs->GetNumLevels();
	uint32_t lowestLevel = 0;
	
	switch ( cascade )
	{
		case 0: lowestLevel = 0; break;
		case 1: lowestLevel = 1; break;
		case 2: lowestLevel = 3; break;
		case 3: lowestLevel = 5; break;
		case 4: lowestLevel = 7; break;
		case 5: lowestLevel = 9; break;
	}
	if ( lowestLevel >= numLODLevels ) lowestLevel = numLODLevels - 1;
	
	for( uint32_t lod = lowestLevel; lod < numLODLevels; lod++ )
	{
		for( uint32_t i = 0; i < 64; i++ )
		{
			GGTerrainChunk* pChunk = pCurrLODs->pLevels[ lod ].chunkGrid[ i ];
			if ( !pChunk || pChunk->IsGenerating() || (!pChunk->IsVisible() && lod != lowestLevel) ) continue;

			const AABB* aabb = pChunk->GetBounds();
			if ( !frustum->CheckBoxFast( *aabb ) ) continue;

			const GPUBuffer* vbs[] = { &pChunk->vertexBuffer };
			uint32_t stride = sizeof( TerrainVertex );
			device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );
			device->BindIndexBuffer( &pChunk->indexBuffer, INDEXFORMAT_16BIT, 0, cmd );
			device->DrawIndexed( numIndices, 0, 0, cmd );
		}
	}
	
	device->EventEnd(cmd);
}

// must be extern "C" to allow /alternatename linker flag to be set correctly
// called from WickedEngine wiRenderer::RefreshEnvProbes()
extern "C" void GGTerrain_Draw_EnvProbe( const SPHERE* culler, const Frustum* frusta, uint32_t frustum_count, CommandList cmd )
{
	if (!ggterrain_update_enabled) return;
	if (!ggterrain_initialised) return;

	// must not do any quad rendering in here
	if ( !ggterrain_initialised ) return;
	if ( !ggterrain_draw_enabled ) return;
	if ( !ggterrain.IsValid() ) return;
	if ( ggterrain_render_wireframe ) return;

	GraphicsDevice* device = wiRenderer::GetDevice();
	device->EventBegin("GGTerrain Draw Env Probe", cmd);

	// environment probe doesn't happen every frame, but it can be expensive when it does
	auto range = wiProfiler::BeginRangeGPU( "Environment Probe - Terrain", cmd );
		
	device->BindPipelineState( &psoEnvProbe, cmd );

	int bindSlot = 2;
	device->BindConstantBuffer( VS, &terrainConstantBuffer, bindSlot, cmd );
	device->BindConstantBuffer( PS, &terrainConstantBuffer, bindSlot, cmd );

	// bind texture and sampler
	device->BindResource( PS, &texPagesColorAndMetal, 50, cmd );
	device->BindResource( PS, &texPagesNormalsRoughnessAO, 51, cmd );
	
	device->BindResource( PS, &texPageTableArray, 53, cmd );
	device->BindResource( PS, &texPageTableFinal, 54, cmd );
	
#if (GGTERRAIN_TEXTURE_FILTERING == GGTERRAIN_TEXTURE_FILTERING_TRILINEAR)
	device->BindSampler( PS, &samplerTrilinearWrap, 1, cmd );
#else
	device->BindSampler( PS, &samplerAnisotropicWrap, 1, cmd );
#endif

	uint32_t numSegments = ggterrain_local_params.segments_per_chunk;
	uint32_t numIndices = (numSegments*2 + 3) * numSegments - 1;
	
	GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
	uint32_t numLODLevels = pCurrLODs->GetNumLevels();

	uint32_t lowestLevel = 4;

	uint32_t instanceData[ 6 ];
	for( int i = 0; i < 6; i++ ) instanceData[ i ] = i;
	device->UpdateBuffer( &instanceBuffer, &instanceData, cmd );
	const GPUBuffer* vbs[] = { &instanceBuffer };
	uint32_t stride = sizeof( uint32_t );
	device->BindVertexBuffers( vbs, 1, 1, &stride, 0, cmd );
		
	for( uint32_t lod = lowestLevel; lod < numLODLevels; lod++ )
	{
		for( uint32_t i = 0; i < 64; i++ )
		{
			GGTerrainChunk* pChunk = pCurrLODs->pLevels[ lod ].chunkGrid[ i ];
			if ( !pChunk || pChunk->IsGenerating() ) continue;
			if ( !pChunk->IsVisible() )
			{
				// if the next lod level down is still generating then show it anyway
				if ( lod > lowestLevel && !pCurrLODs->pLevels[ lod-1 ].IsGenerating() ) continue;
			}

			const AABB* aabb = pChunk->GetBounds();
			if ( !culler->intersects( *aabb ) ) continue;

			// find nearby lights for forward renderer
			ForwardEntityMaskCB cb;
			cb.xForwardLightMask.x = 0;
			cb.xForwardLightMask.y = 0;
			cb.xForwardDecalMask = 0;
			cb.xForwardEnvProbeMask = 0;

			uint32_t buckets[2] = { 0,0 };
			wiRenderer::Visibility *vis = &master.masterrenderer.visibility_main;
			for (size_t i = 0; i < min(size_t(64), vis->visibleLights.size()); ++i) // only support indexing 64 lights at max for now
			{
				const uint16_t lightIndex = vis->visibleLights[i].index;
				const AABB& light_aabb = vis->scene->aabb_lights[lightIndex];
				if (light_aabb.intersects(*aabb))
				{
					const uint8_t bucket_index = uint8_t(i / 32);
					const uint8_t bucket_place = uint8_t(i % 32);
					buckets[bucket_index] |= 1 << bucket_place;
				}
			}
			cb.xForwardLightMask.x = buckets[0];
			cb.xForwardLightMask.y = buckets[1];

			// update light buffer
			device->UpdateBuffer(&wiRenderer::constantBuffers[CBTYPE_FORWARDENTITYMASK], &cb, cmd);
			device->BindConstantBuffer(PS, &wiRenderer::constantBuffers[CBTYPE_FORWARDENTITYMASK], CB_GETBINDSLOT(ForwardEntityMaskCB), cmd);

			const GPUBuffer* vbs[] = { &pChunk->vertexBuffer };
			uint32_t stride = sizeof( TerrainVertex );
			device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );
			device->BindIndexBuffer( &pChunk->indexBuffer, INDEXFORMAT_16BIT, 0, cmd );
			device->DrawIndexedInstanced( numIndices, 6, 0, 0, 0, cmd );
		}
	}
	
	wiProfiler::EndRange( range );
	device->EventEnd(cmd);
}

struct TerrainChunkOcclusion
{
	AABB aabb = {};
	bool bChunkVisible = false;
	uint32_t history = 1;
	uint32_t writeQuery = 0;
} sCO[32][64];

extern "C" TerrainChunkOcclusion * GetChunkVisibleMem(int lod, int idx)
{
	return &sCO[lod][idx];
}
extern "C" uint32_t GetChunkLodStart(void)
{
	return OCCLODSTART;
}

// must be extern "C" to allow /alternatename linker flag to be set correctly
// called from WickedEngine RenderPath3D::Render()
extern "C" void GGTerrain_Draw( const Frustum* frustum, int mode, CommandList cmd )
{
	if (!ggterrain_update_enabled) return;
	if (!ggterrain_initialised) return;

	// must not do any quad rendering in here
	if ( !ggterrain_initialised ) return;
	if ( !ggterrain_draw_enabled ) return;
	if ( !ggterrain.IsValid() ) return;
	if ( mode == 1 && ggterrain_render_wireframe ) return;

	iOccludedTerrainChunks = 0;
	GraphicsDevice* device = wiRenderer::GetDevice();
	device->EventBegin("GGTerrain Draw", cmd);
		
	if ( ggterrain_render_wireframe ) device->BindPipelineState( &psoMainWire, cmd );
	else device->BindPipelineState( &psoMain, cmd );

	int bindSlot = 2;
	device->BindConstantBuffer( VS, &terrainConstantBuffer, bindSlot, cmd );
	device->BindConstantBuffer( PS, &terrainConstantBuffer, bindSlot, cmd );

	// bind texture and sampler
	device->BindResource( PS, &texPagesColorAndMetal, 50, cmd );
	device->BindResource( PS, &texPagesNormalsRoughnessAO, 51, cmd );
	
	device->BindResource( PS, &texPageTableArray, 53, cmd );
	device->BindResource( PS, &texPageTableFinal, 54, cmd );
	
#if (GGTERRAIN_TEXTURE_FILTERING == GGTERRAIN_TEXTURE_FILTERING_TRILINEAR)
	device->BindSampler( PS, &samplerTrilinearWrap, 1, cmd );
#else
	device->BindSampler( PS, &samplerAnisotropicWrap, 1, cmd );
#endif

	uint32_t numSegments = ggterrain_local_params.segments_per_chunk;
	uint32_t numIndices = (numSegments*2 + 3) * numSegments - 1;
	
	GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
	uint32_t numLODLevels = pCurrLODs->GetNumLevels();

	uint32_t lowestLevel = 0;
	if ( mode == 1 ) lowestLevel = GGTERRAIN_REFLECTION_LOWEST_LOD;
	if ( lowestLevel >= numLODLevels ) lowestLevel = numLODLevels - 1;
	
	//PE: Chunks should do a quick bounding box occlusion check.
	for( uint32_t lod = lowestLevel; lod < numLODLevels; lod++ )
	{
		for( uint32_t i = 0; i < 64; i++ )
		{
			sCO[lod][i].bChunkVisible = false;
			GGTerrainChunk* pChunk = pCurrLODs->pLevels[ lod ].chunkGrid[ i ];
			if ( !pChunk || pChunk->IsGenerating() ) continue;
			if ( !pChunk->IsVisible() )
			{
				// if the next lod level down is still generating then show it anyway
				if ( lod > lowestLevel && !pCurrLODs->pLevels[ lod-1 ].IsGenerating() ) continue;
			}
			const AABB* aabb = pChunk->GetBounds();
			if ( !frustum->CheckBoxFast( *aabb ) ) continue;
			
			if (wiRenderer::GetOcclusionCullingEnabled() && bEnableTerrainChunkCulling)
			{
				sCO[lod][i].bChunkVisible = true;
				sCO[lod][i].aabb = *aabb;
				if (lod >= OCCLODSTART && lod < 9)
				{
					if (sCO[lod][i].history == 0)
					{
						if (mode == 0)
						{
							if (!bImGuiInTestGame)
							{
								//XMFLOAT3 center = aabb->getCenter();
								//char tmp[80] = "*\0";
								//DrawDot(tmp, center.x, center.y, center.z);
							}
							iOccludedTerrainChunks++;
						}
						continue;
					}
				}
			}

			const GPUBuffer* vbs[] = { &pChunk->vertexBuffer };
			uint32_t stride = sizeof( TerrainVertex );
			device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );
			device->BindIndexBuffer( &pChunk->indexBuffer, INDEXFORMAT_16BIT, 0, cmd );
			device->DrawIndexed( numIndices, 0, 0, cmd );
		}
	}

	device->EventEnd(cmd);

#ifdef GGTERRAIN_ENABLE_PBR_SPHERES
	device->EventBegin("Sphere Draw", cmd);

	device->BindPipelineState( &psoSphere, cmd );

	device->BindConstantBuffer( PS, &terrainConstantBuffer, bindSlot, cmd );

	device->BindResource( PS, &texColorArray, 50, cmd );
	device->BindResource( PS, &texNormalsArray, 51, cmd );
	device->BindResource( PS, &texSurfaceArray, 52, cmd );

	device->BindSampler( PS, &samplerTrilinearWrap, 0, cmd );

	const GPUBuffer* vbs[] = { &sphereVertexBuffer };
	uint32_t stride = sizeof( VertexSphere );
	device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );
	device->BindIndexBuffer( &sphereIndexBuffer, INDEXFORMAT_16BIT, 0, cmd );
	device->DrawIndexedInstanced( g_numIndicesSphere, 23, 0, 0, 0, cmd );
	
	device->EventEnd(cmd);
#endif
}

// must be extern "C" to allow /alternatename linker flag to be set correctly
// called from WickedEngine RenderPath3D::Render()
extern "C" void GGTerrain_Draw_Transparent( const Frustum* frustum, CommandList cmd )
{
	if (!ggterrain_update_enabled) return;
	if (!ggterrain_initialised) return;

	//we are going to allow editbox map size 3D to show, even for completely empty levels :)
	//if ( !ggterrain_initialised ) return;
	//if ( !ggterrain_draw_enabled ) return;
	//if ( !ggterrain.IsValid() ) return;

	GraphicsDevice* device = wiRenderer::GetDevice();
	device->EventBegin("GGTerrain Transparents", cmd);

	if ( GGTerrain_GetEditSizeVisible3D() )
	{
		device->BindPipelineState( &psoEditCube, cmd );

		int bindSlot = 2;
		device->BindConstantBuffer( VS, &terrainConstantBuffer, bindSlot, cmd );
		device->BindConstantBuffer( PS, &terrainConstantBuffer, bindSlot, cmd );
	
		const GPUBuffer* vbs[] = { &boxVertexBuffer };
		uint32_t stride = sizeof( BoxVertex );
		device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );
		device->BindIndexBuffer( &boxIndexBuffer, INDEXFORMAT_16BIT, 0, cmd );
		device->DrawIndexed( 24, 0, 0, cmd );
	}

	if ( ggterrain_internal_params.sculpt_prev_ramp_valid )
	{
		device->BindPipelineState( &psoRamp, cmd );

		int bindSlot = 2;
		device->BindConstantBuffer( VS, &terrainConstantBuffer, bindSlot, cmd );
		device->BindConstantBuffer( PS, &terrainConstantBuffer, bindSlot, cmd );
	
		const GPUBuffer* vbs[] = { &boxVertexBuffer };
		uint32_t stride = sizeof( BoxVertex );
		device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );
		device->BindIndexBuffer( &boxIndexBuffer, INDEXFORMAT_16BIT, 0, cmd );
		device->DrawIndexed( 24, 0, 0, cmd );
	}
	
	device->EventEnd(cmd);
}

// must be extern "C" to allow /alternatename linker flag to be set correctly
// called from WickedEngine RenderPath2D::Compose()
extern "C" void GGTerrain_Draw_Debug( CommandList cmd )
{
	if ( !ggterrain_update_enabled ) return;
	if ( !ggterrain_render_debug ) return;

	auto range = wiProfiler::BeginRangeGPU( "Terrain - Debug", cmd );

	GraphicsDevice* device = wiRenderer::GetDevice();
	XMFLOAT4 area = { 0, 0, (float)master.masterrenderer.GetWidth3D(), (float)master.masterrenderer.GetHeight3D() };
	device->SetScissorArea(cmd, area);

	device->BindPipelineState( &psoQuad, cmd );

	int bindSlot = 2;
	device->BindConstantBuffer( PS, &terrainConstantBuffer, bindSlot, cmd );
	
	const GPUBuffer* vbs[] = { &quadVertexBuffer };
	uint32_t stride = sizeof( VertexQuad );
	device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );
	
	device->BindResource( PS, &texReadBackCompute, 0, cmd );
	device->BindResource( PS, &texPageTableArray, 1, cmd );
	device->BindResource( PS, &texPageTableFinal, 2, cmd );
	device->BindResource( PS, &texPagesColorAndMetal, 3, cmd );
	device->BindResource( PS, &texPagesNormalsRoughnessAO, 4, cmd );
	device->BindResource( PS, &texMask, 5, cmd );
	device->BindResource( PS, &texLODNormalMapArray, 6, cmd );
	device->BindResource( PS, &texMaterialMap, 7, cmd );
	device->BindResource( PS, wiRenderer::GetTexture(TEXTYPE_2D_SKYATMOSPHERE_SKYLUMINANCELUT), 13, cmd );
	device->BindSampler( PS, &samplerBilinear, 0, cmd );

	device->Draw( 6, 0, cmd );

	wiProfiler::EndRange( range );
}

// must be extern "C" to allow /alternatename linker flag to be set correctly
// called from WickedEngine RenderPath2D::Compose()
extern "C" void GGTerrain_Draw_Overlay( CommandList cmd )
{
	if (!ggterrain_update_enabled) return;

	if ( (ggterrain_local_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MINI_MAP) == 0 ) return;

	auto range = wiProfiler::BeginRangeGPU( "Terrain - Overlay", cmd );

	GraphicsDevice* device = wiRenderer::GetDevice();
	XMFLOAT4 area = { 0, 0, (float)master.masterrenderer.GetWidth3D(), (float)master.masterrenderer.GetHeight3D() };
	device->SetScissorArea(cmd, area);

	device->BindPipelineState( &psoOverlay, cmd );

	int bindSlot = 2;
	device->BindConstantBuffer( VS, &terrainConstantBuffer, bindSlot, cmd );
	device->BindConstantBuffer( PS, &terrainConstantBuffer, bindSlot, cmd );
	
	const GPUBuffer* vbs[] = { &quadVertexBuffer };
	uint32_t stride = sizeof( VertexQuad );
	device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );
	
	device->BindResource( PS, &texLODHeightMapArray, 0, cmd );
	device->BindResource( PS, &texLODNormalMapArray, 1, cmd );
	device->BindResource( PS, &texColorArray, 2, cmd );

	device->BindSampler( PS, &samplerBilinear, 0, cmd );

	device->Draw( 6, 0, cmd );

	wiProfiler::EndRange( range );
}


// Physics shape

const int lastPhysicsLODLevel = -1;

int GGTerrain_Physics_GetBounds( float* minX, float* minY, float* minZ, float* maxX, float* maxY, float* maxZ )
{
	if ( !ggterrain_initialised ) return 0;

	GGTerrainLODSet* pLODSet = ggterrain.GetCurrentLODs();
	if ( !pLODSet->IsReady() ) return 0;

	const AABB* aabb;
	if ( lastPhysicsLODLevel < 0 ) aabb = pLODSet->GetBounds();
	else
	{
		int level = lastPhysicsLODLevel;
		if ( level >= (int) pLODSet->GetNumLevels() ) level = pLODSet->GetNumLevels() - 1;
		aabb = pLODSet->pLevels[ level ].GetBounds();
	}

	*minX = aabb->_min.x;
	*minY = aabb->_min.y;
	*minZ = aabb->_min.z;
				
	*maxX = aabb->_max.x;
	*maxY = aabb->_max.y;
	*maxZ = aabb->_max.z;

	return 1;
}

// min max are in world coordinates, any triangle callbacks must be in physics coordinates
void GGTerrain_Physics_ProcessVertices( void* callback, float worldToPhysScale, float minX, float minY, float minZ, float maxX, float maxY, float maxZ )
{
	if ( !ggterrain_initialised ) return;

	GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
	if ( !pCurrLODs->IsReady() || pCurrLODs->IsGenerating() ) return;

	AABB aabbCheck;
	aabbCheck._min.x = minX;
	aabbCheck._min.y = minY;
	aabbCheck._min.z = minZ;
	aabbCheck._max.x = maxX;
	aabbCheck._max.y = maxY;
	aabbCheck._max.z = maxZ;

	int firstLevel = 1; // LB: was 0, try next LOD down for acceptable accuracy
	int maxLevels = pCurrLODs->GetNumLevels();
	int lastLevel = lastPhysicsLODLevel;
	if ( lastLevel < 0 || lastLevel >= maxLevels ) lastLevel = maxLevels - 1;

	for( int level = firstLevel; level <= lastLevel; level++ )
	{
		GGTerrainLODLevel* pLevel = &(pCurrLODs->pLevels[ level ]);
		if ( pLevel->PhysicsProcessVertices( callback, worldToPhysScale, &aabbCheck, (level==firstLevel) ) == 0 ) return;
	}
}

// min max are in world coordinates, any triangle callbacks must be in physics coordinates
void GGTerrain_Physics_RayCast( void* callback, float worldToPhysScale, float srcX, float srcY, float srcZ, float dstX, float dstY, float dstZ )
{
	if ( !ggterrain_initialised ) return;

	GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
	if ( !pCurrLODs->IsValid() || pCurrLODs->IsGenerating() ) return;
	
	RAY ray;
	ray.origin.x = srcX;
	ray.origin.y = srcY;
	ray.origin.z = srcZ;
	ray.direction.x = dstX - srcX;
	ray.direction.y = dstY - srcY;
	ray.direction.z = dstZ - srcZ;

	float sqrDist = ray.direction.x*ray.direction.x + ray.direction.y*ray.direction.y + ray.direction.z*ray.direction.z;
	if ( sqrDist < 0.001f ) return;
	else
	{
		float dist = sqrt( sqrDist );
		ray.direction.x /= dist;
		ray.direction.y /= dist;
		ray.direction.z /= dist;
	}

	if ( ray.direction.x != 0 ) ray.direction_inverse.x = 1.0f / ray.direction.x;
	else ray.direction_inverse.x = 1e20f;
	if ( ray.direction.y != 0 ) ray.direction_inverse.y = 1.0f / ray.direction.y;
	else ray.direction_inverse.y = 1e20f;
	if ( ray.direction.z != 0 ) ray.direction_inverse.z = 1.0f / ray.direction.z;
	else ray.direction_inverse.z = 1e20f;

	// find chunk containing ray origin
	GGTerrainChunk* pChunk = 0;
	int result = pCurrLODs->GetChunk( ray.origin.x, ray.origin.z, &pChunk, 0 );

	float currDist = 0;

	// if not found then ray starts outside terrain, ray cast to the first chunk
	if ( !result || !pChunk )
	{
		uint maxLevel = pCurrLODs->GetNumLevels() - 1;
		pChunk = pCurrLODs->pLevels[ maxLevel ].RayCastBox( ray, &currDist );
		result = 1;
	}

	if ( !pChunk ) return;
		
	int count = 0;
	while( result && pChunk && count < 10000 )
	{
		float endDist = pChunk->GetRayExitDist( ray, 0 );
		if (endDist < 0)
		{
			endDist = 0;
		}
		float entryDist = 0.0f;
		int inHeightRange = pChunk->CheckRayHeight( ray, &entryDist );

		if ( inHeightRange && entryDist <= endDist )
		{
			if ( pChunk->PhysicsRayCast( callback, worldToPhysScale, ray, currDist, endDist ) ) return;
		}

		currDist = endDist;

		// find next chunk
		endDist += pChunk->segSize * 0.1f;
		float nextX = ray.origin.x + ray.direction.x * endDist;
		float nextY = ray.origin.y + ray.direction.y * endDist;
		float nextZ = ray.origin.z + ray.direction.z * endDist;

		if ( nextY > pCurrLODs->GetBounds()->_max.y && ray.direction.y >= 0 ) return;
		if ( nextY < pCurrLODs->GetBounds()->_min.y && ray.direction.y <= 0 ) return;

		result = pCurrLODs->GetChunk( nextX, nextZ, &pChunk, 0 );
		count++;
	}
}

int GGTerrain_GetTriangleList( KMaths::Vector3** vertices, float minX, float minZ, float maxX, float maxZ, int firstLOD )
{
	// extracts the triangle list from the 'current' terrain polygons, including the lower resolutionm ones due to LOD
	if ( !ggterrain_initialised ) return 0;

	GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
	if ( !pCurrLODs->IsValid() || pCurrLODs->IsGenerating() ) return 0;

	UnorderedArray<KMaths::Vector3> vertexArray;
	int lastLevel = pCurrLODs->GetNumLevels() - 1;

	for( int level = firstLOD; level <= lastLevel; level++ )
	{
		GGTerrainLODLevel* pLevel = &pCurrLODs->pLevels[ level ];
		pLevel->GetTriangleList( &vertexArray, minX, minZ, maxX, maxZ, level==firstLOD );
	}

	uint32_t numVertices = vertexArray.NumItems();
	*vertices = new KMaths::Vector3[ numVertices ];
	for( uint32_t i = 0; i < numVertices; i++ )
	{
		(*vertices)[ i ] = vertexArray[ i ];
	}

	return numVertices;
}

int GGTerrain_GetTriangleListHighQuality(KMaths::Vector3** vertices, float minXoverall, float minZoverall, float maxXoverall, float maxZoverall, int firstLOD)
{
	// extracts the triangle list at specific LOD quality throughout terrain, by shifting cameera position and updating chunks to get best polygons at that location
	if (!ggterrain_initialised) return 0;

	// collect all vertices for our high quality trianle list
	UnorderedArray<KMaths::Vector3> vertexArray;

	// subdivide terrain area
	float fSliceSize = 4000.0f;
	for (float minX = minXoverall; minX < maxXoverall; minX += fSliceSize)
	{
		for (float minZ = minZoverall; minZ < maxZoverall; minZ += fSliceSize)
		{
			float maxX = minX + fSliceSize;
			float maxZ = minZ + fSliceSize;
			float centerX = minX + (fSliceSize / 2);
			float centerZ = minZ + (fSliceSize / 2);

			// move camera and update chunks at that location
			for ( int iChunkIsMarchingCubes = 0; iChunkIsMarchingCubes < 15; iChunkIsMarchingCubes++ )
			{
				terrainlock.lock();
				if (ggterrain_update_enabled)
				{
					ggterrain.CheckParams();
					ggterrain.UpdateChunks(centerX, centerZ);
					GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
					uint32_t timeout = 0;
					while (pCurrLODs->IsGenerating() && !pCurrLODs->pLevels[pCurrLODs->GetNumLevels() - 1].IsReady() && timeout++ < 300) Sleep(1);
					if (timeout >= 300)
					{
						// investigate why this stalled, maybe more than 300ms?
					}
				}
				terrainlock.unlock();
				Sleep(1);
			}

			// add relevant trianles to list
			GGTerrainLODSet* pCurrLODs = ggterrain.GetCurrentLODs();
			if (!pCurrLODs->IsValid() || pCurrLODs->IsGenerating()) return 0;
			int lastLevel = pCurrLODs->GetNumLevels() - 1; // may not need this as we have updated the chunk data at this location!
			for (int level = firstLOD; level <= lastLevel; level++)
			{
				GGTerrainLODLevel* pLevel = &pCurrLODs->pLevels[level];
				pLevel->GetTriangleList(&vertexArray, minX, minZ, maxX, maxZ, level == firstLOD);
			}
		}
	}

	// when all triangles collected in list, prepare some memory to store it and pass out
	uint32_t numVertices = vertexArray.NumItems();
	*vertices = new KMaths::Vector3[numVertices];
	for (uint32_t i = 0; i < numVertices; i++)
	{
		(*vertices)[i] = vertexArray[i];
	}

	// success
	return numVertices;
}

// End physics shape

//PE: These functions are missing in imgui_terrain_loop_v3() to control new terrain system.
//PE: Missing function.
//void cTerrain::SetBrush ( unsigned int iBrush )
void GGTerrain_SetBrush(int i)
{
	//0 = Circle brush , 1 = Square
	return;
}
//void cTerrain::SetPaintHardness(unsigned int iHardness)
void GGTerrain_SetPaintHardness(unsigned int iHardness)
{
	//Goes From Transparent to Opaque 0-255.
	return;
}
//void cTerrain::SetScaleSource(float fScale)
void GGTerrain_SetScaleSource(float fScale)
{
	//When drawing a texture to terrain scale it down.
	//Range 1.0 - 10.0
	//1:1 = 1.0
	//Highter = smaller , need to be fitted to POW2 so tiling is still seamless.
	return;
}

#ifdef CUSTOMTEXTURES
void GGTerrain_ReloadTextures(wiGraphics::CommandList cmd, std::vector<std::string>* files, std::vector<int>* failures, char* rootDir)
{
	if (files)
	{
		for (uint32_t i = 0; i < GGTERRAIN_MAX_SOURCE_TEXTURES; i++)
		{
			bool bFailed = false;

			if (files->size() > i && files->at(i).length() > 0)
			{
				bFailed = !GGTerrain_LoadTextureDDSIntoSlice(files->at(i).c_str(), &texColorArray, i, &colorDDS, cmd);
				
				if (!bFailed)
				{
					char normal[MAX_PATH];
					strcpy(normal, files->at(i).c_str());
					normal[strlen(normal) - strlen("color.dds")] = 0;
					strcat(normal, "normal.dds");
					if (FileExist(normal))
					{
						bFailed = !GGTerrain_LoadTextureDDSIntoSlice(normal, &texNormalsArray, i, &normalDDS, cmd);
					}

					if (!bFailed)
					{
						char surface[MAX_PATH];
						strcpy(surface, files->at(i).c_str());
						surface[strlen(surface) - strlen("color.dds")] = 0;
						strcat(surface, "surface.dds");
						if (FileExist(surface))
						{
							bFailed = !GGTerrain_LoadTextureDDSIntoSlice(surface, &texSurfaceArray, i, &surfaceDDS, cmd);
						}
					}
				}
			}
			else
			{
				bFailed = true;
			}

			if (bFailed)
			{
				if (rootDir)
				{
					GGTerrain_LoadDefaultTextureIntoSlot(i, rootDir, cmd);
				}
				else
				{
					GGTerrain_LoadDefaultTextureIntoSlot(i, "", cmd);
				}
				if (failures)
				{
					failures->push_back(i);
				}
			}
		}
	}
	else
	{
		// Reload default selection of textures
		for (uint32_t i = 0; i < GGTERRAIN_MAX_SOURCE_TEXTURES; i++)
		{
			char szDstRoot[MAX_PATH];
			sprintf_s(szDstRoot, MAX_PATH, "Files/terraintextures/mat%d", i + 1);

			char szDstColorPath[MAX_PATH];
			char szDstNormalPath[MAX_PATH];

			sprintf_s(szDstColorPath, MAX_PATH, "%s/Color.dds", szDstRoot);
			sprintf_s(szDstNormalPath, MAX_PATH, "%s/Normal.dds", szDstRoot);
#ifdef ONLYLOADWHENUSED
			//PE: Load actual texture later when they are used.
			if (bTextureUploaded[0] == false && i == 0)
			{
				bTextureUploaded[0] = true;
				//PE: Only upload textures that are actually in use by terrain.
				GGTerrain_LoadTextureDDSIntoSlice(szDstColorPath, &texColorArray, i, &colorDDS, cmd, true);
				GGTerrain_LoadTextureDDSIntoSlice(szDstNormalPath, &texNormalsArray, i, &normalDDS, cmd, true);

				char szDstSurfacePath[MAX_PATH];
				sprintf_s(szDstSurfacePath, MAX_PATH, "%s/Surface.dds", szDstRoot);
				GGTerrain_LoadTextureDDSIntoSlice(szDstSurfacePath, &texSurfaceArray, i, &surfaceDDS, cmd, true);
			}
			else if(bTextureUploaded[i] == false && (i == 2 || i == 4 || i == 17 || i == 20 || i == 31)) //PE: Default set.
			{
				bTextureUploaded[i] = true;
				//PE: Only upload textures that are actually in use by terrain.
				GGTerrain_LoadTextureDDSIntoSlice(szDstColorPath, &texColorArray, i, &colorDDS, cmd, false);
				GGTerrain_LoadTextureDDSIntoSlice(szDstNormalPath, &texNormalsArray, i, &normalDDS, cmd, false);

				char szDstSurfacePath[MAX_PATH];
				sprintf_s(szDstSurfacePath, MAX_PATH, "%s/Surface.dds", szDstRoot);
				GGTerrain_LoadTextureDDSIntoSlice(szDstSurfacePath, &texSurfaceArray, i, &surfaceDDS, cmd, false);
			}
#else
			GGTerrain_LoadTextureDDSIntoSlice(szDstColorPath, &texColorArray, i, &colorDDS, cmd);
			GGTerrain_LoadTextureDDSIntoSlice(szDstNormalPath, &texNormalsArray, i, &normalDDS, cmd);
			char szDstSurfacePath[MAX_PATH];
			sprintf_s(szDstSurfacePath, MAX_PATH, "%s/Surface.dds", szDstRoot);
			GGTerrain_LoadTextureDDSIntoSlice(szDstSurfacePath, &texSurfaceArray, i, &surfaceDDS, cmd);
#endif
		}
	}

	// Can't update the texture here without a command list, so flag it for update later
	////ggterrain_internal_params.update_material_texture = 1;
	GGTerrain_InvalidateEverything(GGTERRAIN_INVALIDATE_TEXTURES);
}

#ifdef ONLYLOADWHENUSED
void GGTerrain_CheckMaterialUsed(wiGraphics::CommandList cmd)
{

	if (bCheckForNewTerrainTextures)
	{
		bool bMaterialUsed[256];
		for (uint32_t i = 0; i < GGTERRAIN_MAX_SOURCE_TEXTURES; i++)
		{
			bMaterialUsed[i] = false;
		}

		//PE: Add material settings.
		for (int i = 0; i < 5; i++)
		{
			int mat = ggterrain_global_render_params.layerMatIndex[i] & 0xff;
			bMaterialUsed[mat] = true;
		}
		int mat = ggterrain_global_render_params.baseLayerMaterial & 0xff;
		bMaterialUsed[mat] = true;
		mat = ggterrain_global_render_params.slopeMatIndex[0] & 0xff;
		bMaterialUsed[mat] = true;
		mat = ggterrain_global_render_params.slopeMatIndex[1] & 0xff;
		bMaterialUsed[mat] = true;

		if (ggterrain_extra_params.paint_material > 0)
		{
			mat = ggterrain_extra_params.paint_material & 0xff;
			if(mat > 0 && mat <= GGTERRAIN_MAX_SOURCE_TEXTURES)
				bMaterialUsed[mat-1] = true;
		}

		for (int x = 0; x < GGTERRAIN_MATERIALMAP_SIZE; x++)
		{
			for (int y = 0; y < GGTERRAIN_MATERIALMAP_SIZE; y++)
			{
				//uint8_t mat = GGTerrain_GetMaterialIndex(x, y);
				uint32_t index = y * GGTERRAIN_MATERIALMAP_SIZE + x;
				if (x >= 0 && y >= 0 && x < GGTERRAIN_MATERIALMAP_SIZE && y < GGTERRAIN_MATERIALMAP_SIZE)
				{
					uint8_t mat = pMaterialMap[index];
					if (mat > 0)
					{
						if (mat <= GGTERRAIN_MAX_SOURCE_TEXTURES)
							bMaterialUsed[mat-1] = true;
					}
				}

			}
		}
		bool bTextChange = false;
		for (uint32_t i = 0; i < GGTERRAIN_MAX_SOURCE_TEXTURES; i++)
		{
			if(bMaterialUsed[i] == true && bTextureUploaded[i] == false)
			{
				//PE: Check all material IDs if we need to update any.
				char szDstRoot[MAX_PATH];
				sprintf_s(szDstRoot, MAX_PATH, "terraintextures/mat%d", i + 1);


				char szDstColorPath[MAX_PATH];
				char szDstNormalPath[MAX_PATH];

				sprintf_s(szDstColorPath, MAX_PATH, "%s/Color.dds", szDstRoot);
				sprintf_s(szDstNormalPath, MAX_PATH, "%s/Normal.dds", szDstRoot);
				bTextureUploaded[i] = true;
				bMaterialUsed[i] = false;
				//PE: Only upload textures that are actually in use by terrain.
				GGTerrain_LoadTextureDDSIntoSlice(szDstColorPath, &texColorArray, i, &colorDDS, cmd, false);
				GGTerrain_LoadTextureDDSIntoSlice(szDstNormalPath, &texNormalsArray, i, &normalDDS, cmd, false);

				char szDstSurfacePath[MAX_PATH];
				sprintf_s(szDstSurfacePath, MAX_PATH, "%s/Surface.dds", szDstRoot);
				GGTerrain_LoadTextureDDSIntoSlice(szDstSurfacePath, &texSurfaceArray, i, &surfaceDDS, cmd, false);
				bTextChange = true;
			}
		}
		bCheckForNewTerrainTextures = false;
		if (bTextChange)
		{
			//PE: update textures.
			GGTerrain_InvalidateEverything(GGTERRAIN_INVALIDATE_TEXTURES);
		}
	}
}
#endif


void GGTerrain_LoadDefaultTextureIntoSlot(int i, char* rootDir, wiGraphics::CommandList cmd)
{
	// Load default texture into any unused slots
	char szDstRoot[MAX_PATH];
	strcpy(szDstRoot, rootDir);
	strcat(szDstRoot, "\\Files\\");
	strcat(szDstRoot, "terraintextures/mat32");

	char szDstColorPath[MAX_PATH];
	char szDstNormalPath[MAX_PATH];

	sprintf_s(szDstColorPath, MAX_PATH, "%s/Color.dds", szDstRoot);
	sprintf_s(szDstNormalPath, MAX_PATH, "%s/Normal.dds", szDstRoot);

	GGTerrain_LoadTextureDDSIntoSlice(szDstColorPath, &texColorArray, i, &colorDDS, cmd);
	GGTerrain_LoadTextureDDSIntoSlice(szDstNormalPath, &texNormalsArray, i, &normalDDS, cmd);

	char szDstSurfacePath[MAX_PATH];
	sprintf_s(szDstSurfacePath, MAX_PATH, "%s/Surface.dds", szDstRoot);
	GGTerrain_LoadTextureDDSIntoSlice(szDstSurfacePath, &texSurfaceArray, i, &surfaceDDS, cmd);
}
#endif

} // namespace GGTerrain

#ifdef GGTERRAIN_UNDOREDO
void GGTerrain_PerformUndoRedoAction(int type, void* pEventData, int eList)
{
	if (!pEventData)
		return;

	uint8_t* pAddressOfEventData = nullptr;

	switch (type)
	{
		case eUndoSys_Terrain_Sculpt:
		{
			sUndoSysEventTerrainSculpt* pEvent = (sUndoSysEventTerrainSculpt*)pEventData;

			GGTerrain::GGTerrain_SetSculptData(0, 0, pEvent);
			
			pAddressOfEventData = pEvent->typeData;

			delete pEvent;
			pEvent = 0;

			break;
		}
		case eUndoSys_Terrain_Paint:
		{
			sUndoSysEventTerrainPaint* pEvent = (sUndoSysEventTerrainPaint*)pEventData;

			GGTerrain::GGTerrain_SetPaintData(0, 0, pEvent);
		
			pAddressOfEventData = pEvent->materialData;

			delete pEvent;
			pEvent = 0;

			break;
		}
	}

	//Mark the sculpt data memory for this event as unused (0 = undo, 1 = redo)
	if (eList == eUndoSys_UndoList)
		undosys_terrain_setmemoryunusedfrom(pAddressOfEventData, g_TerrainUndoMem);
	else
		undosys_terrain_setmemoryunusedfrom(pAddressOfEventData, g_TerrainRedoMem);
}

void GGTerrain_CreateUndoRedoAction(int type, int eList, bool bUserAction, void* pEventData)
{
	// count any sculpts as will need to reverse them all when undo-ing a delayed object repositioning
	extern int g_iDelayActualObjectAdjustmentSculptCount;
	if (type == eUndoSys_Terrain_Sculpt && bUserAction == true )
	{
		g_iDelayActualObjectAdjustmentSculptCount++;
	}
	
	// User performed this undo action, so clear the redo stack since it now contains outdated events.
	if (bUserAction == true)
	{
		undosys_clearredostack();
		undosys_terrain_cleardata(g_TerrainRedoMem);
	}

	if (type == eUndoSys_Terrain_Sculpt || type == eUndoSys_Terrain_Paint)
	{
		// If we are undoing a redo or vice versa then the terrain snapshot will be out of date.
		if (!bUserAction)
		{
			if (type == eUndoSys_Terrain_Sculpt)
				GGTerrain::GGTerrain_GetSculptData(g_pTerrainSnapshot);
			else if(type == eUndoSys_Terrain_Paint)
				GGTerrain::GGTerrain_GetPaintData(g_pTerrainSnapshot);
		}

		TerrainEditsBB bb;
		if (bUserAction)
		{	// Use the bounding box from the sculpt/paint that the user performed.
			bb = g_EditBounds;
		}
		else
		{
			// Use the bounding box from the event that we are storing the undo/redo data for.
			if (type == eUndoSys_Terrain_Sculpt)
			{
				sUndoSysEventTerrainSculpt* pImpendingUndoRedoEvent = (sUndoSysEventTerrainSculpt*)pEventData;
				bb.maxX = pImpendingUndoRedoEvent->maxX;
				bb.maxY = pImpendingUndoRedoEvent->maxY;
				bb.minX = pImpendingUndoRedoEvent->minX;
				bb.minY = pImpendingUndoRedoEvent->minY;
			}
			else if (type == eUndoSys_Terrain_Paint)
			{
				sUndoSysEventTerrainPaint* pImpendingUndoRedoEvent = (sUndoSysEventTerrainPaint*)pEventData;
				bb.maxX = pImpendingUndoRedoEvent->maxX;
				bb.maxY = pImpendingUndoRedoEvent->maxY;
				bb.minX = pImpendingUndoRedoEvent->minX;
				bb.minY = pImpendingUndoRedoEvent->minY;
			}
		}

		if (type == eUndoSys_Terrain_Sculpt)
		{
			// After sculpting, any objects that were on the terrain are moved up in line with the sculpt, so need to collect multiple events so both actions can be undone in one press.
			// though cannot use undosys_multiplevents_start(); we will use glue system instead (added to repos obj code)
			undosys_terrain_sculpt(bb, g_pTerrainSnapshot, eList);
		}
		else if (type == eUndoSys_Terrain_Paint)
		{
			undosys_terrain_paint(bb, g_pTerrainSnapshot, eList);
		}			
	}
}


#endif // GGTERRAIN_UNDOREDO
