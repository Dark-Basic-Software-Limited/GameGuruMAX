#ifndef _H_GGTREES
#define _H_GGTREES

#include <stdint.h>
#include "../../../../WickedEngineDX12/WickedEngine/wiGraphicsDevice.h"
#include "../../../../WickedEngineDX12/WickedEngine/wiScene.h"

#define GGTREES_PAINT_SPRAY         0
#define GGTREES_PAINT_ADD           1
#define GGTREES_PAINT_REMOVE        2
#define GGTREES_PAINT_MOVE          3
#define GGTREES_PAINT_SPRAY_REMOVE  4
#define GGTREES_PAINT_SCALE         5

struct sUndoSysEventTreeMove;

namespace GGTrees
{
	struct GGTreesParams
	{
		int draw_enabled = 0;
		int draw_shadows = 1;
		int tree_shadow_range = 3; // higher values draw tree shadows at greater distance, max=5

		int paint_mode = GGTREES_PAINT_SPRAY;
		uint64_t paint_tree_bitfield = 0x00100000; // 1 bit per tree, default pine
		int paint_density = 65; // 0 to 100
		float water_dist = 10;

		int paint_scale_random_low = 10; // 0 to 100
		int paint_scale_random_high = 245; // 0 to 100

		float lod_dist = 3000;
		float lod_dist_shadow = 2500;

		int hide_until_update = 0;

	};

	// these values must not be modified outside the tree module
	struct GGTreesInternalParams 
	{
		int prevMouseLeft = 0;
		int mouseLeftPressed = 0;
		int mouseLeftState = 0;
		int mouseLeftReleased = 0;
		int mouseY = 0;
		float minTotalHeight = 0;
		float maxTotalHeight = 0;
		uint32_t tree_selected = 0xFFFFFFFF;
		int scaleMouseYStart = 0;
		int scaleStart = 0;
		uint32_t treeChunkUpdate = 0;
	};

	extern GGTreesParams ggtrees_global_params; // modify this anywhere

	extern int ggtrees_draw_enabled;

	struct GGTreePoint
	{
		float x;
		float y;
		float z;
		float scale;
	};

	int GGTrees_GetClosest( float x, float z, float radius, GGTreePoint** pOutPoints ); // returns the number of trees in pOutPoints, pOutPoints must be undefined it will be created
	int GGTrees_RayCast( RAY pickRay, float maxDist, float* outDist, uint32_t* treeID ); // returns 1 if hit, 0 if not. If hit then treeID will be populated
	void GGTrees_SetTreePosition( uint32_t treeID, float x, float z );
	
	uint32_t GGTrees_GetDataSize(); // number of floats required in data array
	int GGTrees_GetData( float* data ); // data must be allocated with GGTrees_GetSculptDataSize() floats, returns 1 on success
	int GGTrees_SetData( float* data ); // number of floats must be equal to GGTrees_GetSculptDataSize(), returns 1 on success
	int GGTrees_GetSnapshot(uint8_t* data);

	void GGTrees_SetPerformanceMode( uint32_t mode );
	void GGTrees_Delete_Trees(float pickX, float pickZ, float radius);

	void GGTrees_Init();
	void GGTrees_UpdateFrustumCulling( wiScene::CameraComponent* camera );
	void GGTrees_Update( float camX, float camY, float camZ, wiGraphics::CommandList cmd, bool bRenderTargetFocus);
	void GGTrees_Update_Painting( RAY ray );
	int GGTrees_UsingBrush();
	uint32_t GGTrees_GetNumTypes();
	uint32_t GGTrees_GetNumHighDetail();
	void GGTrees_ChangeDensity(int density);
	void GGTrees_RepopulateInstances();
	int GGTrees_UpdateInstances(int accurate);
	void GGTrees_HideAll();
	void GGTrees_DeselectHighlightedTree(void);
	void GGTrees_LockVisibility();

	const char* GGTrees_GetTextureName( uint32_t index );
	float GGTrees_GetImageScale( uint32_t index );

	void GGTrees_UpdateFlatArea( int mode, int type, float x, float z, float sx, float sz, float angle );
	void GGTrees_RestoreAllFlattened();

	bool GGTrees_GetDefaultDataV2(char *filename);
}

#endif // _H_GGTREES