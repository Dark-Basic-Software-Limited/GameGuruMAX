#ifndef _H_GGRASS
#define _H_GGRASS

#include <stdint.h>
#include "../../../../WickedEngineDX12/WickedEngine/wiGraphicsDevice.h"
#include "../../../../WickedEngineDX12/WickedEngine/wiScene.h"

#include "Shaders/GGGrassConstants.hlsli"

#define GGGRASS_PAINT_SPRAY         0
#define GGGRASS_PAINT_SPRAY_REMOVE  1
#define GGGRASS_PAINT_SPRAY_RESTORE 2

#define GGGRASS_INITIAL_LOD_DIST 1500

struct sUndoSysEventGrass;

namespace GGGrass
{
	struct GGGrassParams
	{
		int draw_enabled = 1;
		int shadow_range = 0; // 0=no shadows, 1-5=shadow cascade cutoff

		uint64_t paint_type = 0;  // 1 bit per grass type
		int paint_material = 0; // 0=auto, otherwise force the use of the grass for that material
		int paint_mode = GGGRASS_PAINT_SPRAY;
		int paint_density = 100; // 0 to 100

		float max_height = 30000;
		float min_height = -1000;

		float max_height_underwater = 1000;
		float min_height_underwater = -7000;

		float lod_dist = GGGRASS_INITIAL_LOD_DIST;
		int simplePBR = 0;
		float grass_scale = 40;
	};

	// these values must not be modified outside the tree module
	struct GGGrassInternalParams 
	{
		int prevMouseLeft = 0;
		int mouseLeftPressed = 0;
		int mouseLeftState = 0;
		int mouseLeftReleased = 0;
		float prevLodDist = GGGRASS_INITIAL_LOD_DIST;
	};

	extern GGGrassParams gggrass_save_params;
	extern GGGrassParams gggrass_global_params; // modify this anywhere

	void GGGrass_Init_Textures(LPSTR pRemoteGrassPath);
	void GGGrass_Init();
	void GGGrass_Update( wiScene::CameraComponent* camera, wiGraphics::CommandList cmd, bool bRenderTargetFocus );
	void GGGrass_Update_Painting( RAY ray );
	int GGGrass_UsingBrush();
	void GGGrass_BindGrassArray( uint32_t slot, wiGraphics::CommandList cmd );

	const char* GGGrass_GetTextureFilename( uint32_t matIndex, uint32_t grassIndex );
	const char* GGGrass_GetTextureShortName( uint32_t matIndex, uint32_t grassIndex );

	void GGGrass_SetPerformanceMode( uint32_t mode );

	void GGGrass_AddAll();
	void GGGrass_RemoveAll();
	void GGGrass_RestoreAll();

	uint32_t GGGrass_GetDataSize();
	int GGGrass_GetData( uint8_t* data ); // data must be allocated with a size of GGGrass_GetDataSize(), returns 1 on success
	int GGGrass_SetData( uint32_t size, uint8_t* data, sUndoSysEventGrass* pEvent = nullptr); // size must be equal to GGGrass_GetDataSize(), returns 1 on success

	void GGGrass_UpdateFlatArea( int mode, int type, float x, float z, float sx, float sz, float angle );
	void GGGrass_RestoreAllFlattened();
	int GGGrass_UpdateInstances();
}

#endif // _H_GGGRAS