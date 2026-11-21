//
// GPUParticles system from AGK
//

#include <string>
#include "GPUParticles.h"
#include "Utility/stb_image.h"
#include "CFileC.h"
#include "Particles/Models/gpup_part_1024.h"
#include "Particles/Models/gpup_part_4096.h"
#include "wiRenderer.h"

// redefines MAX_PATH to 1050
#include "preprocessor-moreflags.h"

#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

using namespace wiGraphics;
using namespace wiScene;

namespace GPUParticles
{
// file functions, only support one file open at a time
FILE* gpup_file = 0;
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

int OpenToRead( int fileID, const char* filename ) 
{ 
	char fullPath[ MAX_PATH ];
	strcpy_s( fullPath, MAX_PATH, filename );
	GG_GetRealPath( fullPath, 0 );
	int err = fopen_s( &gpup_file, fullPath, "rb" ); //PE: reading from docwrite failed here. (filename)
	if ( err == 0 ) return 1;
	else return 0;
}

void AGKDumpString( int fileID ) 
{  
	if ( !gpup_file ) return;

	char c;
	do
	{
		c = fgetc( gpup_file );
	} while( c != 0 && !feof(gpup_file) );
}

float AGKReadFloat( int fileID ) 
{ 
	if ( !gpup_file ) return 0;
	
	float f;
	fread( &f, 4, 1, gpup_file );
	return f;
}

int ReadInteger( int fileID ) 
{ 
	if ( !gpup_file ) return 0;

	int i = 0;
	fread( &i, 4, 1, gpup_file );
	return i;
}

void AGKCloseFile( int fileID ) 
{ 
	if ( !gpup_file ) return;
	fclose( gpup_file );
	gpup_file = 0;
}

int64_t i64StartTime = 0;
int64_t i64TimeFreq = 1;
void AGKTimerInit()
{
	QueryPerformanceCounter ( (LARGE_INTEGER*) &i64StartTime );
	QueryPerformanceFrequency ( (LARGE_INTEGER*) &i64TimeFreq );
	if ( i64TimeFreq == 0 ) i64TimeFreq = 1;
}

// Timer in seconds
float AGKTimer()
{
	int64_t i64CurrentTime;
	QueryPerformanceCounter ( (LARGE_INTEGER*) &i64CurrentTime );
	i64CurrentTime -= i64StartTime;
	return (float) (i64CurrentTime / (double) i64TimeFreq);
}

uint32_t iRandSeed = 0;
void RandomInit()
{
	int64_t i64CurrentTime;
	QueryPerformanceCounter ( (LARGE_INTEGER*) &i64CurrentTime );
	iRandSeed = (uint32_t) i64CurrentTime;
}

// produces a number between 0 and 65535
int Random() 
{ 
	// Uses integer overflow to generate pseudo random numbers.
	iRandSeed = (214013*iRandSeed + 2531011);
	// only use the top 16 bits as the lower 16 bits produce very short repeat cycles.
	return (iRandSeed >> 16) & 0xffff;
	return 0; 
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

// produces a number between -2,147,483,648 and 2,147,483,647
int Random2() 
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
	return (int) y;
} 

//PE: Changed from 32 to 64.
#define gpup_maxeffects 64

#define gpup_particles_1k    32
#define gpup_particles_4k    64
#define gpup_particles_16k   128
#define gpup_particles_64k   256
#define gpup_particles_256k  512
#define gpup_particles_1M    1024

#define gpup_particles_opaque    1
#define gpup_particles_clipped   2
#define gpup_particles_alpha     3
#define gpup_particles_additive  4

#define gpup_active    1
#define gpup_inactive  0

// MainVS constants
struct sMainVSConstantData
{
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
	float4x4 ViewProj;
	float4 globalpos[8];
	float4 tilex;
	float4 particles;
	float3 pgrow;
	float padding1;
	float3 ppos;
	float padding2;
	float3 area;
	float padding3;
	float3 rotat;
	float padding4;
	float3 rota;
	float padding5;
	float3 image_count;
	float padding6;
	float3 globalsize;
	float padding7;
	float3 globalrot;
	float padding8;
	float3 CameraPos;
	float padding9;
	float2 pcolor;
	float padding10;
	float padding11;
};

struct sMainPSConstantData
{
	float4 tile;
	float3 clr;
	float moder;
	float3 image_count;
	float agk_time;
	float opacity;
	float filler;  //PE: Must be 16 floats so, reserved for later.
	float filler2;
	float filler3;
};

struct sPosConstantData
{								// offset
	float4 gravity;				// 0
	float4 emittersize;			// 16
	float4 emitterrotation;		// 32
	float3 emitterline;			// 48
	float emittertype;			
	float3 emittersway;			// 64
	float agk_time;
	float3 localemitter;		// 80
	float warp;
	float3 pos1;				// 96
	float rnd;
	float3 pos2;				// 112
	float padding1;
	float3 emitterlinecount;	// 128
	float padding2;
	float3 moveit;				// 144
	float padding3;
	float3 particles;			// 160
	float padding4;
	float3 area;				// 176
	float padding5;
	float2 spawnpos;			// 192
	float2 lifespan;			// 200
};

struct sSpeedConstantData
{								// offset
	float4 area;				// 0
	float4 gravity;				// 16
	float4 speedvar;			// 32
	float4 field;				// 48
	float4 autorot;				// 64
	float4 reffloor;			// 80
	float4 refsphere;			// 96
	float3 particles;			// 112
	float rnd;					// 124
	float2 spawnpos;			// 128
	float warp;					// 136
	float rnd2;					// 140
};

struct sNoiseConstantData
{
	float3 off;
	float paddingsobuffermultipleofsixteen; // :)
};

sNoiseConstantData noiseConstantData;

class t_gpup_settings
{
public:
	int   simulOn = 0;
	float gtimer = 0;
	float time = 0;
	int   spawnCount = 0;
	int   pauser = 0;
	float tmr = 0;
	float sn = 0;
	float rotsn = 0;
		
	int   emitterCount = 0;
	int   lastEmitter = 0;
	int   activeEffects1 = 0;
	int   activeEffects2 = 0;
	int   activeEffects = 0;
			
	int   vrx = 0;
	int   vry = 0;
	int   split = 0;

	t_gpup_settings() {}
	~t_gpup_settings() {}
};

t_gpup_settings gpup_settings;

class t_gpup_emitter
{
public:
	std::string name;
	
	float x = 0;
	float y = 0;
	float z = 0;
	float scale = 0;
	int   particles = 0;
	float fsize = 0;
	int   frez = 0;
	int   ani_x = 0;
	int   ani_y = 0;
	int   ani_s = 0;
	
	int   effectVisible = 1;
	
	int   image_count = 0;	
	int   layer_count = 0;
	
	float area = 0;
	float varea = 0;
	float narea = 0;
	
	float r = 0;
	float g = 0;
	float b = 0;
	float opacity = 1.0;
	int   blendmode = 0;
	int   col = 0;
	int   facing = 0;
	
	float field_adhesion = 0;
	float field_x = 0;
	float field_y = 0;
	float field_z = 0;
	
	float noise_adhesion = 0;
	float offset_speed = 0;
	float auto_rot_x = 0;
	float auto_rot_y = 0;
	float auto_rot_z = 0;
	float noise_bias = 0;
	
	float gravity_x = 0;
	float gravity_y = 0;
	float gravity_z = 0;
	
	float lifespan = 0;
	float lifespan_variance = 0;
	float maxed = 0;
	
	int   emitter_type = 0;
	float emitter_local_x = 0;
	float emitter_local_y = 0;
	float emitter_local_z = 0;
	float emitter_size_x = 0;
	float emitter_size_y = 0;
	float emitter_size_z = 0;
	float emitter_radius_1 = 0;
	float emitter_radius_2 = 0;
	float emitter_height = 0;
	float emitter_x1 = 0;
	float emitter_y1 = 0;
	float emitter_z1 = 0;
	float emitter_x2 = 0;
	float emitter_y2 = 0;
	float emitter_z2 = 0;
	float emitter_rotation_x = 0;
	float emitter_rotation_y = 0;
	float emitter_rotation_z = 0;
	float emitter_speed_x = 0;
	float emitter_speed_y = 0;
	float emitter_speed_z = 0;
	float emitter_speed_v = 0;

	float emitter_speedadjustment_x = 0.5; // aded to rotate the speed vector (so can change direction of emission)
	float emitter_speedadjustment_y = 0.5; //PE: 0.5 = neutral.
	float emitter_speedadjustment_z = 0.5;

	int   emitter_wire = 0;
	int   emitter_corners = 0;
	float emitter_auto_rotation_x = 0;
	float emitter_auto_rotation_y = 0;
	float emitter_auto_rotation_z = 0;
	int   emitter_auto_movement = 0;
	float emitter_line_sway = 0;
	float emitter_line_sway_size = 0;
	float emitter_line_sway_speed = 0;
	float emitter_lineswirl = 0;
	float emitter_linebeams = 0;
	float emitter_linebeamoffset = 0;
	
	int   emitter_slow_mode = 0;
	float emitter_amount = 0;
	
	int   emitter_burst_mode = 0;
	int   emitter_burst_auto = 0;
	float emitter_burst_delay = 0;
	int   emitter_burst_frames = 0;
	int   emitter_burst_fire = 0;
	float emitter_burst_delaycount = 0;
	
	int   emitter_moving = 0;
	float emitter_moving_speed = 0;
	
	float image_distortion = 0;
	float image_distortion_speed = 0;
	
	float size1 = 0;
	float size2 = 0;
	float rotation = 0;
	float rotation_variance = 0;
	float sizer = 0;
		
	int	  currImage = 0;
	Texture texPos[2]; // point sample
	Texture texSpeed[2]; // point sample
	Texture texNoise; // linear sample
	Texture t_field; // linear sample

	RenderPass renderPassSpeed[ 2 ];
	RenderPass renderPassPos[ 2 ];
	RenderPass renderPassNoise;

	sMainVSConstantData mainVSConstantData = {};
	sMainPSConstantData mainPSConstantData = {};
	sPosConstantData posConstantData = {};
	sSpeedConstantData speedConstantData = {};
	
	float noiseOff1 = 0;
	float noiseOff2 = 0;
	int   emiton = 0;
	
	float globalx[8] = {0};
	float globaly[8] = {0};
	float globalz[8] = {0};
	
	float newX = 0;
	float newY = 0;
	float newZ = 0;
	int   pivotMoved = 0;
	
	float globalSize = 0;
	
	float globalRotX = 0;
	float globalRotY = 0;
	float globalRotZ = 0;
	
	int   testpos = 0;
	float spawnint = 0;
	int   effectLoaded = 0;
	
	float noiseZaehler = 0;
	int   noiseDir = 0;
	
	Texture image1;
	Texture imagex;
	Texture gradient_1;
	
	float bounciness = 0;
	float floorActive = 0;
	float sphereActive = 0;
	float sphereRadius = 0;
	float floorHeight = 0;
	float sphereX = 0;
	float sphereY = 0;
	float sphereZ = 0;
	
	int   spawnCount = 0;
	int   emitterActive = 0;
	int   subEmitters = 0;
	int   subEmitterUsed[8] = {0};
	float particleSize = 0;
	float activeTimer = 0;
	int   smooth_blending = 0;

	float emitter_animation_speed = 1.0f;

	float currentdistancefromcamera = 0.0f;

	t_gpup_emitter() {}
	~t_gpup_emitter() {}
};

t_gpup_emitter gpup_emitter[ gpup_maxeffects ];

// rendering variables

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

// GPUP variables
GPUBuffer mainIndexBufferObj0;
GPUBuffer mainIndexBufferObj1;
int mainIndexCountObj0 = 0;
int mainIndexCountObj1 = 0;
GPUBuffer mainVertexBufferObj0;
GPUBuffer mainVertexBufferObj1;

PipelineState psoAlpha;
PipelineState psoAdd;
PipelineState psoOpaque;

Shader shaderMainVS;
Shader shaderMainPS;

GPUBuffer mainVSConstants;
GPUBuffer mainPSConstants;

// quad variables
PipelineState psoQuadDefault;
Shader shaderQuadVS;
Shader shaderQuadDefaultPS;
GPUBuffer quadVertexBuffer;

Shader shaderNoisePS;
Shader shaderSpeedPS;
Shader shaderPosPS;

GPUBuffer speedConstants;
GPUBuffer posConstants;
GPUBuffer noiseConstants;

PipelineState psoNoise;
PipelineState psoPos;
PipelineState psoSpeed;

Sampler samplerPoint;
Sampler samplerLinear;
Sampler samplerLinearWrap;

Texture texNoiseOrig;
Texture texDist2;

int gpu_particles_initialised = 0;


void GPUP_LoadTexture( const char* filename, Texture* tex ) 
{ 
	GraphicsDevice* device = wiRenderer::GetDevice();

	int width, height, channels;
	char filePath[ MAX_PATH ];
	strcpy_s( filePath, MAX_PATH, filename );
	GG_GetRealPath( filePath, 0 );
	uint8_t* imageData = stbi_load( filePath, &width, &height, &channels, 4 );

	SubresourceData data = {};
	data.pSysMem = imageData;
	data.SysMemPitch = width * 4;

	TextureDesc texDesc = {};
	texDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;
	texDesc.clear.color[0] = 1.0f;
	texDesc.clear.color[1] = 0.0f;
	texDesc.clear.color[2] = 0.0f;
	texDesc.clear.color[3] = 0.0f;
	texDesc.SampleCount = 1;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = USAGE_DEFAULT;
	texDesc.Width = width;
	texDesc.Height = height;

	device->CreateTexture( &texDesc, &data, tex );
	device->SetName( tex, "imageTex" );

	stbi_image_free( imageData );
}

void GPUP_CreateRenderTexture( int width, int height, Texture* tex ) 
{
	GraphicsDevice* device = wiRenderer::GetDevice();

	uint32_t* imageData = new uint32_t[ width * height ];
	for( int i = 0; i < width * height; i++ ) imageData[i] = 0;

	SubresourceData data = {};
	data.pSysMem = imageData;
	data.SysMemPitch = width * 4;

	TextureDesc texDesc = {};
	texDesc.BindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;
	texDesc.clear.color[0] = 1.0f;
	texDesc.clear.color[1] = 0.0f;
	texDesc.clear.color[2] = 0.0f;
	texDesc.clear.color[3] = 0.0f;
	texDesc.SampleCount = 1;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = USAGE_DEFAULT;
	texDesc.Width = width;
	texDesc.Height = height;

	device->CreateTexture( &texDesc, &data, tex );
	device->SetName( tex, "renderTex" );

	delete [] imageData;
}

void GPUP_DeleteTexture( Texture* tex ) 
{
	GraphicsDevice* device = wiRenderer::GetDevice();
}

void GPUParticlesDrawQuad( RenderPass* renderPass, CommandList cmd )
{
	GraphicsDevice* device = wiRenderer::GetDevice();

	device->RenderPassBegin( renderPass, cmd );

	Viewport vp;
	vp.Width = (float) renderPass->GetDesc().attachments[0].texture->GetDesc().Width;
	vp.Height = (float) renderPass->GetDesc().attachments[0].texture->GetDesc().Height;
	device->BindViewports( 1, &vp, cmd );
			
	const GPUBuffer* vbs[] = { &quadVertexBuffer };
	uint32_t stride = sizeof( VertexQuad );
	device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );
	
	// bind samplers
	device->BindSampler( PS, &samplerLinear, 0, cmd );
	device->BindSampler( PS, &samplerPoint, 1, cmd );
	device->BindSampler( PS, &samplerLinearWrap, 2, cmd );

	device->Draw( 6, 0, cmd );

	device->RenderPassEnd( cmd );
}

uint32_t g_emitterSorting[256][gpup_maxeffects];
uint32_t g_emitterSortingDrawCount[256][gpup_maxeffects];
uint32_t g_emitterCurrentIndex[256];

extern "C" void gpup_draw_init(const wiScene::CameraComponent & camera, wiGraphics::CommandList cmd)
{
	// calculates all needed particles and distances from camera
	for (size_t i = 0; i < gpup_maxeffects; ++i)
	{
		g_emitterSortingDrawCount[cmd][i] = 0;
		g_emitterSorting[cmd][i] = 0;
		g_emitterSorting[cmd][i] |= (uint32_t)i & 0x0000FFFF;
		if (gpup_emitter[i].effectLoaded == 0 || gpup_emitter[i].effectVisible == 0) continue;
		XMFLOAT3 emitercenter = XMFLOAT3(gpup_emitter[i].globalx[0], gpup_emitter[i].globaly[0], gpup_emitter[i].globalz[0]);
		float distance = wiMath::DistanceEstimated(XMFLOAT3(emitercenter.x, camera.Eye.y, emitercenter.z), camera.Eye);
		gpup_emitter[i].currentdistancefromcamera = distance;
		g_emitterSorting[cmd][i] |= ((uint32_t)(distance * 10) & 0x0000FFFF) << 16;
	}

	// now sort so distant particles are first in list
	std::sort(std::begin(g_emitterSorting[cmd]), std::end(g_emitterSorting[cmd]), std::greater<uint32_t>());

	// we start here, being the particle furthest away
	g_emitterCurrentIndex[cmd] = 0;
}

extern "C" void gpup_draw_bydistance(const wiScene::CameraComponent & camera, wiGraphics::CommandList cmd, float fDistanceFromCamera)
{
	// finished rendering particles
	if (g_emitterCurrentIndex[cmd] == -1 || g_emitterCurrentIndex[cmd] >= gpup_maxeffects)
		return;

	// is called just before a transparent object is rendered at the specified distance
	// allowing us to insert the particle rendering as needed (i.e. window, particle, window, window, particle, window)
	int iThisLoopStart = g_emitterCurrentIndex[cmd];
	for (size_t i = iThisLoopStart; i < gpup_maxeffects; ++i)
	{
		size_t e = g_emitterSorting[cmd][i] & 0x0000FFFF;

		// clever bit - if this particle distance is further than the current distance from the camera, 
		// we need to render it now as the transparent object from Wicked will be rendered next and particle
		// needs to be behind it!
		if (e >= 0 && e < gpup_maxeffects && gpup_emitter[e].currentdistancefromcamera > fDistanceFromCamera)
		{
			// we carry on and render this particle at least
			// and will loop around to see if the next particle is 
			// is further than the current distance
		}
		else
		{
			// we stop right now, this particle is closer than the current distance
			// and we cannot render it yet			
			return;
		}

		// next time we are ready for next particle after this one
		g_emitterCurrentIndex[cmd] = i + 1;

		if (e < 0 || e >= gpup_maxeffects || gpup_emitter[e].effectLoaded == 0 || gpup_emitter[e].effectVisible == 0) continue;

		GraphicsDevice* device = wiRenderer::GetDevice();
		device->EventBegin("GPUParticles Draw", cmd);

		gpup_emitter[e].mainVSConstantData.Proj = camera.Projection;
		gpup_emitter[e].mainVSConstantData.View = camera.View;

		XMMATRIX proj((float*)&camera.Projection);
		XMMATRIX viewProj((float*)&camera.View);
		viewProj *= proj;
		memcpy(gpup_emitter[e].mainVSConstantData.ViewProj.m, viewProj.r, sizeof(float) * 16);

		gpup_emitter[e].mainVSConstantData.CameraPos.x = camera.Eye.x;
		gpup_emitter[e].mainVSConstantData.CameraPos.y = camera.Eye.y;
		gpup_emitter[e].mainVSConstantData.CameraPos.z = camera.Eye.z;

		float scale = 1.0f;
		gpup_emitter[e].mainVSConstantData.World.m[0][0] = scale;
		gpup_emitter[e].mainVSConstantData.World.m[0][1] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[0][2] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[0][3] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[1][0] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[1][1] = scale;
		gpup_emitter[e].mainVSConstantData.World.m[1][2] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[1][3] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[2][0] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[2][1] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[2][2] = scale;
		gpup_emitter[e].mainVSConstantData.World.m[2][3] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[3][3] = 1;

		//PE: Blend state is not always set when changing from psoAlpha to psoAdd, add different blendfactor's to force change (not visible).
		//PE: TODO - psoOpaque -> psoAdd has another change problem, must be desc.dss = &depthDesc;
		//PE: TODO - As we dont use sprites currently, leave this for after EA. ( REMOVED_EARLYACCESS )
		switch (gpup_emitter[e].blendmode)
		{
			case 0: // opaque
			case 1: // opaque
			case 2: // opaque
			{
				if (gpup_emitter[e].mainPSConstantData.opacity != 1.0f)
				{
					device->BindPipelineState(&psoAlpha, cmd); //If using opacity switch to alpha blend.
					device->BindBlendFactor(1.0, 1.0, 1.0, 1.0, cmd);
				}
				else
				{
					device->BindPipelineState(&psoOpaque, cmd); //Sprites
					device->BindBlendFactor(1.0, 1.0, 0.998, 1.0, cmd);
				}

			} break;

			case 3: // alpha
			{
				device->BindPipelineState(&psoAlpha, cmd);
				device->BindBlendFactor(1.0, 1.0, 1.0, 1.0, cmd);
			} break;

			case 4: // additive
			{
				device->BindPipelineState(&psoAdd, cmd); //Particles
				device->BindBlendFactor(1.0, 1.0, 0.999, 1.0, cmd);
			} break;

			default:
			{
				device->BindPipelineState(&psoAdd, cmd);
				device->BindBlendFactor(1.0, 1.0, 0.999, 1.0, cmd);
			} break;
		}

		gpup_emitter[e].mainPSConstantData.agk_time = AGKTimer();

		device->UpdateBuffer(&mainPSConstants, &gpup_emitter[e].mainPSConstantData, cmd, sizeof(sMainPSConstantData));
		device->BindConstantBuffer(VS, &mainVSConstants, 0, cmd);
		device->BindConstantBuffer(PS, &mainPSConstants, 1, cmd);

		int numIndices = 0;
		const GPUBuffer* vbs[1];
		if (gpup_emitter[e].particles == 32)
		{
			vbs[0] = &mainVertexBufferObj1;
			device->BindIndexBuffer(&mainIndexBufferObj1, INDEXFORMAT_16BIT, 0, cmd);
			numIndices = mainIndexCountObj1;
		}
		else
		{
			vbs[0] = &mainVertexBufferObj0;
			device->BindIndexBuffer(&mainIndexBufferObj0, INDEXFORMAT_16BIT, 0, cmd);
			numIndices = mainIndexCountObj0;
		}

		uint32_t stride = sizeof(GPUP_1024_Vertex); // 3 position + 2 uv
		device->BindVertexBuffers(vbs, 0, 1, &stride, 0, cmd);

		// bind textures
		device->BindResource(VS, &gpup_emitter[e].texPos[gpup_emitter[e].currImage], 0, cmd);
		device->BindResource(VS, &gpup_emitter[e].gradient_1, 2, cmd);
		device->BindResource(VS, &gpup_emitter[e].texSpeed[gpup_emitter[e].currImage], 5, cmd);
		device->BindResource(PS, &gpup_emitter[e].imagex, 1, cmd);
		device->BindResource(PS, &gpup_emitter[e].image1, 3, cmd);
		device->BindResource(PS, &texDist2, 4, cmd);

		// bind samplers
		device->BindSampler(PS, &samplerLinear, 0, cmd);
		device->BindSampler(PS, &samplerPoint, 1, cmd);
		device->BindSampler(PS, &samplerLinearWrap, 2, cmd);
		device->BindSampler(VS, &samplerPoint, 1, cmd);

		int ii = gpup_emitter[e].particles / 64;
		if (gpup_emitter[e].particles == 32) ii = 1;

		if (g_emitterSortingDrawCount[cmd][i] == 0)
		{
			for (int i = 0; i < ii; i++)
			{
				for (int j = 0; j < ii; j++)
				{
					gpup_emitter[e].mainVSConstantData.World.m[3][0] = 64.0f * i;
					gpup_emitter[e].mainVSConstantData.World.m[3][1] = 0.0f;
					gpup_emitter[e].mainVSConstantData.World.m[3][2] = 64.0f * j;

					device->UpdateBuffer(&mainVSConstants, &gpup_emitter[e].mainVSConstantData, cmd, sizeof(sMainVSConstantData));

					device->DrawIndexed(numIndices, 0, 0, cmd);
				}
			}
		}
		device->EventEnd(cmd);

		// increment draw count for this emitter
		g_emitterSortingDrawCount[cmd][i]++;
	}
}

// must be extern "C" to allow /alternatename linker flag to be set correctly
// called from WickedEngine RenderPath3D::RenderTransparents()
extern "C" void gpup_draw( const CameraComponent& camera, CommandList cmd )
{
	// cannot do any quad rendering in here as we are already in a render pass by this point
	// only render final emitter objects

	//PE: Quick dist sorting from emitter center to camera.
	gpup_draw_init(camera, cmd);

	for( size_t i = 0; i < gpup_maxeffects; ++i )
	{
		size_t e = g_emitterSorting[cmd][i] & 0x0000FFFF;

		if ( e < 0 || e >= gpup_maxeffects || gpup_emitter[e].effectLoaded == 0 || gpup_emitter[e].effectVisible == 0 ) continue;

		GraphicsDevice* device = wiRenderer::GetDevice();
		device->EventBegin("GPUParticles Draw", cmd);
		
		gpup_emitter[e].mainVSConstantData.Proj = camera.Projection;
		gpup_emitter[e].mainVSConstantData.View = camera.View;

		XMMATRIX proj( (float*) &camera.Projection );
		XMMATRIX viewProj( (float*) &camera.View );
		viewProj *= proj;
		memcpy( gpup_emitter[e].mainVSConstantData.ViewProj.m, viewProj.r, sizeof(float) * 16 );

		gpup_emitter[e].mainVSConstantData.CameraPos.x = camera.Eye.x;
		gpup_emitter[e].mainVSConstantData.CameraPos.y = camera.Eye.y;
		gpup_emitter[e].mainVSConstantData.CameraPos.z = camera.Eye.z;

		float scale = 1.0f;
		gpup_emitter[e].mainVSConstantData.World.m[0][0] = scale;
		gpup_emitter[e].mainVSConstantData.World.m[0][1] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[0][2] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[0][3] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[1][0] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[1][1] = scale;
		gpup_emitter[e].mainVSConstantData.World.m[1][2] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[1][3] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[2][0] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[2][1] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[2][2] = scale;
		gpup_emitter[e].mainVSConstantData.World.m[2][3] = 0;
		gpup_emitter[e].mainVSConstantData.World.m[3][3] = 1;

		//PE: Blend state is not always set when changing from psoAlpha to psoAdd, add different blendfactor's to force change (not visible).
		//PE: TODO - psoOpaque -> psoAdd has another change problem, must be desc.dss = &depthDesc;
		//PE: TODO - As we dont use sprites currently, leave this for after EA. ( REMOVED_EARLYACCESS )
		switch( gpup_emitter[e].blendmode )
		{
			case 0: // opaque
			case 1: // opaque
			case 2: // opaque
			{
				if (gpup_emitter[e].mainPSConstantData.opacity != 1.0f)
				{
					device->BindPipelineState(&psoAlpha, cmd); //If using opacity switch to alpha blend.
					device->BindBlendFactor(1.0, 1.0, 1.0, 1.0, cmd);
				}
				else
				{
					device->BindPipelineState(&psoOpaque, cmd); //Sprites
					device->BindBlendFactor(1.0, 1.0, 0.998, 1.0, cmd);
				}

			} break;

			case 3: // alpha
			{
				device->BindPipelineState( &psoAlpha, cmd );
				device->BindBlendFactor(1.0, 1.0, 1.0, 1.0, cmd);
			} break;

			case 4: // additive
			{
				device->BindPipelineState( &psoAdd, cmd ); //Particles
				device->BindBlendFactor(1.0, 1.0, 0.999, 1.0, cmd);
			} break;

			default:
			{
				device->BindPipelineState( &psoAdd, cmd );
				device->BindBlendFactor(1.0, 1.0, 0.999, 1.0, cmd);
			} break;
		}

		gpup_emitter[e].mainPSConstantData.agk_time = AGKTimer();
		
		device->UpdateBuffer( &mainPSConstants, &gpup_emitter[e].mainPSConstantData, cmd, sizeof(sMainPSConstantData) );
		
		device->BindConstantBuffer( VS, &mainVSConstants, 0, cmd );
		device->BindConstantBuffer( PS, &mainPSConstants, 1, cmd );

		int numIndices = 0;
		const GPUBuffer* vbs[ 1 ];
		if ( gpup_emitter[e].particles == 32 ) 
		{
			vbs[ 0 ] = &mainVertexBufferObj1;
			device->BindIndexBuffer( &mainIndexBufferObj1, INDEXFORMAT_16BIT, 0, cmd );
			numIndices = mainIndexCountObj1;
		}
		else 
		{
			vbs[ 0 ] = &mainVertexBufferObj0;
			device->BindIndexBuffer( &mainIndexBufferObj0, INDEXFORMAT_16BIT, 0, cmd );
			numIndices = mainIndexCountObj0;
		}

		uint32_t stride = sizeof(GPUP_1024_Vertex); // 3 position + 2 uv
		device->BindVertexBuffers( vbs, 0, 1, &stride, 0, cmd );

		// bind textures
		device->BindResource( VS, &gpup_emitter[e].texPos[ gpup_emitter[e].currImage ], 0, cmd );
		device->BindResource( VS, &gpup_emitter[e].gradient_1, 2, cmd );
		device->BindResource( VS, &gpup_emitter[e].texSpeed[ gpup_emitter[e].currImage ], 5, cmd );
		
		device->BindResource( PS, &gpup_emitter[e].imagex, 1, cmd );
		device->BindResource( PS, &gpup_emitter[e].image1, 3, cmd );
		device->BindResource( PS, &texDist2, 4, cmd );
	
		// bind samplers
		device->BindSampler( PS, &samplerLinear, 0, cmd );
		device->BindSampler( PS, &samplerPoint, 1, cmd );
		device->BindSampler( PS, &samplerLinearWrap, 2, cmd );

		device->BindSampler( VS, &samplerPoint, 1, cmd );

		int ii = gpup_emitter[e].particles / 64;
		if ( gpup_emitter[e].particles == 32 ) ii = 1;
	
		for( int i = 0; i < ii; i++ )
		{
			for( int j = 0; j < ii; j++ )
			{
				gpup_emitter[e].mainVSConstantData.World.m[3][0] = 64.0f * i;
				gpup_emitter[e].mainVSConstantData.World.m[3][1] = 0.0f;
				gpup_emitter[e].mainVSConstantData.World.m[3][2] = 64.0f * j;

				device->UpdateBuffer( &mainVSConstants, &gpup_emitter[e].mainVSConstantData, cmd, sizeof(sMainVSConstantData) );

				device->DrawIndexed( numIndices, 0, 0, cmd );
			}
		}
		device->EventEnd(cmd);
	}
}

void gpup_updatesettings( int enr )
{
	if ( enr < 0 ) return;
	
	// only for files
	gpup_emitter[enr].posConstantData.emittertype = (float) gpup_emitter[enr].emitter_type;
	// ***
		
	float narea = 0.5f + gpup_emitter[enr].narea * 16.0f;
	switch( gpup_emitter[enr].emitter_type )
	{
		case 0:
		{
			gpup_emitter[enr].posConstantData.emittersize.x = gpup_emitter[enr].emitter_size_x * gpup_emitter[enr].varea;
			gpup_emitter[enr].posConstantData.emittersize.y = gpup_emitter[enr].emitter_size_y * gpup_emitter[enr].varea;
			gpup_emitter[enr].posConstantData.emittersize.z = gpup_emitter[enr].emitter_size_z * gpup_emitter[enr].varea;
			gpup_emitter[enr].posConstantData.emittersize.w = 0; 			
		} break;
				
		case 1:
		{
			gpup_emitter[enr].posConstantData.emittersize.x = gpup_emitter[enr].emitter_radius_1 * gpup_emitter[enr].varea;
			gpup_emitter[enr].posConstantData.emittersize.y = gpup_emitter[enr].emitter_radius_2 * gpup_emitter[enr].varea;
			gpup_emitter[enr].posConstantData.emittersize.z = gpup_emitter[enr].emitter_height * gpup_emitter[enr].varea;
			gpup_emitter[enr].posConstantData.emittersize.w = 1.0f; 
		} break;
				
		case 2:
		{
			gpup_emitter[enr].posConstantData.emittersize.x = gpup_emitter[enr].emitter_radius_1 * gpup_emitter[enr].varea;
			gpup_emitter[enr].posConstantData.emittersize.y = gpup_emitter[enr].emitter_radius_2 * gpup_emitter[enr].varea;
			gpup_emitter[enr].posConstantData.emittersize.z = 0.0f;
			gpup_emitter[enr].posConstantData.emittersize.w = 2.0f;
		} break;
				
		case 3:
		{
			gpup_emitter[enr].posConstantData.emittersize.x = (gpup_emitter[enr].emitter_x1 - 0.5f) * 2.0f * gpup_emitter[enr].varea;
			gpup_emitter[enr].posConstantData.emittersize.y = (gpup_emitter[enr].emitter_y1 - 0.5f) * 2.0f * gpup_emitter[enr].varea;
			gpup_emitter[enr].posConstantData.emittersize.z = (gpup_emitter[enr].emitter_z1 - 0.5f) * 2.0f * gpup_emitter[enr].varea;
			gpup_emitter[enr].posConstantData.emittersize.w = 3.0f;
			
			gpup_emitter[enr].posConstantData.emitterline.x = (gpup_emitter[enr].emitter_x2 - 0.5f) * 2.0f * gpup_emitter[enr].varea;
			gpup_emitter[enr].posConstantData.emitterline.y = (gpup_emitter[enr].emitter_y2 - 0.5f) * 2.0f * gpup_emitter[enr].varea;
			gpup_emitter[enr].posConstantData.emitterline.z = (gpup_emitter[enr].emitter_z2 - 0.5f) * 2.0f * gpup_emitter[enr].varea;
			
			gpup_emitter[enr].posConstantData.emittersway.x = gpup_emitter[enr].emitter_line_sway_speed;
			gpup_emitter[enr].posConstantData.emittersway.y = gpup_emitter[enr].emitter_line_sway;
			gpup_emitter[enr].posConstantData.emittersway.z = gpup_emitter[enr].emitter_line_sway_size;
			
			gpup_emitter[enr].posConstantData.emitterlinecount.x = gpup_emitter[enr].emitter_lineswirl;
			gpup_emitter[enr].posConstantData.emitterlinecount.y = gpup_emitter[enr].emitter_linebeams;
			gpup_emitter[enr].posConstantData.emitterlinecount.z = gpup_emitter[enr].emitter_linebeamoffset;
		} break;
	}
		
	gpup_emitter[enr].mainPSConstantData.clr.x = gpup_emitter[enr].r;
	gpup_emitter[enr].mainPSConstantData.clr.y = gpup_emitter[enr].g;
	gpup_emitter[enr].mainPSConstantData.clr.z = gpup_emitter[enr].b;
	gpup_emitter[enr].mainPSConstantData.opacity = 1.0f;

	float fFinalSpeedX = gpup_emitter[enr].emitter_speedadjustment_x;// gpup_emitter[enr].emitter_speed_x;
	float fFinalSpeedY = gpup_emitter[enr].emitter_speedadjustment_y;// gpup_emitter[enr].emitter_speed_y;
	float fFinalSpeedZ = gpup_emitter[enr].emitter_speedadjustment_z;// gpup_emitter[enr].emitter_speed_z;

	gpup_emitter[enr].speedConstantData.speedvar.x = (fFinalSpeedX-0.5f)*0.1f;
	gpup_emitter[enr].speedConstantData.speedvar.y = (fFinalSpeedY-0.5f)*0.1f;
	gpup_emitter[enr].speedConstantData.speedvar.z = (fFinalSpeedZ-0.5f)*0.1f;
	gpup_emitter[enr].speedConstantData.speedvar.w = gpup_emitter[enr].emitter_speed_v ; 
	
	gpup_emitter[enr].speedConstantData.field.x = gpup_emitter[enr].field_x*50.0f;
	gpup_emitter[enr].speedConstantData.field.y = gpup_emitter[enr].field_y*50.0f;
	gpup_emitter[enr].speedConstantData.field.z = gpup_emitter[enr].field_z*50.0f;
	gpup_emitter[enr].speedConstantData.field.w = gpup_emitter[enr].field_adhesion*0.1f ;
	
	gpup_emitter[enr].posConstantData.gravity.x = (gpup_emitter[enr].gravity_x-0.5f)/60.0f;
	gpup_emitter[enr].posConstantData.gravity.y = (gpup_emitter[enr].gravity_y-0.5f)/60.0f;
	gpup_emitter[enr].posConstantData.gravity.z = (gpup_emitter[enr].gravity_z-0.5f)/60.0f;
	gpup_emitter[enr].posConstantData.gravity.w = (float) gpup_emitter[enr].emitter_corners ;
	
	gpup_emitter[enr].speedConstantData.gravity.x = (gpup_emitter[enr].gravity_x-0.5f)/60.0f;
	gpup_emitter[enr].speedConstantData.gravity.y = (gpup_emitter[enr].gravity_y-0.5f)/60.0f;
	gpup_emitter[enr].speedConstantData.gravity.z = (gpup_emitter[enr].gravity_z-0.5f)/60.0f;
	gpup_emitter[enr].speedConstantData.gravity.w = gpup_emitter[enr].noise_bias*0.05f ;
	
	gpup_emitter[enr].posConstantData.area.x = gpup_emitter[enr].area;
	gpup_emitter[enr].posConstantData.area.y = gpup_emitter[enr].varea;
	gpup_emitter[enr].posConstantData.area.z = 64;
	
	gpup_emitter[enr].speedConstantData.area.x = gpup_emitter[enr].area;
	gpup_emitter[enr].speedConstantData.area.y = gpup_emitter[enr].varea;
	gpup_emitter[enr].speedConstantData.area.z = 64;
	gpup_emitter[enr].speedConstantData.area.w = narea ; 
	
	gpup_emitter[enr].mainVSConstantData.area.x = gpup_emitter[enr].area;
	gpup_emitter[enr].mainVSConstantData.area.y = gpup_emitter[enr].varea;
	gpup_emitter[enr].mainVSConstantData.area.z = 64;
	
	gpup_emitter[enr].posConstantData.lifespan.x = gpup_emitter[enr].lifespan*60.0f*10.0f;
	gpup_emitter[enr].posConstantData.lifespan.y = gpup_emitter[enr].lifespan_variance;
	
	gpup_emitter[enr].mainVSConstantData.pgrow.x = 0.001f+gpup_emitter[enr].size1*gpup_emitter[enr].sizer;
	gpup_emitter[enr].mainVSConstantData.pgrow.y = 0.001f+gpup_emitter[enr].size2*gpup_emitter[enr].sizer;
	gpup_emitter[enr].mainVSConstantData.pgrow.z = gpup_emitter[enr].particleSize;
	
	gpup_emitter[enr].speedConstantData.autorot.x = gpup_emitter[enr].auto_rot_x - 0.5f;
	gpup_emitter[enr].speedConstantData.autorot.y = gpup_emitter[enr].auto_rot_y - 0.5f;
	gpup_emitter[enr].speedConstantData.autorot.z = gpup_emitter[enr].auto_rot_z - 0.5f;
	gpup_emitter[enr].speedConstantData.autorot.w = gpup_emitter[enr].offset_speed*0.5f ;
		
	int particles = gpup_emitter[enr].particles;
	gpup_emitter[enr].mainVSConstantData.particles.x = (float) particles;
	gpup_emitter[enr].mainVSConstantData.particles.y = (float) (particles*particles);
	gpup_emitter[enr].mainVSConstantData.particles.z = (float) (particles*particles-1);
	gpup_emitter[enr].mainVSConstantData.particles.w = (float) gpup_emitter[enr].facing;
	
	gpup_emitter[enr].posConstantData.particles.x = (float) particles;
	gpup_emitter[enr].posConstantData.particles.y = (float) (particles*particles);
	gpup_emitter[enr].posConstantData.particles.z = (float) (particles*particles-1);
	
	gpup_emitter[enr].speedConstantData.particles.x = (float) particles;
	gpup_emitter[enr].speedConstantData.particles.y = (float) (particles*particles);
	gpup_emitter[enr].speedConstantData.particles.z = (float) (particles*particles-1);
	
	gpup_emitter[enr].mainVSConstantData.image_count.x = (float) (gpup_emitter[enr].image_count+1);
	gpup_emitter[enr].mainVSConstantData.image_count.y = gpup_emitter[enr].image_distortion;
	gpup_emitter[enr].mainVSConstantData.image_count.z = gpup_emitter[enr].image_distortion_speed;
	
	gpup_emitter[enr].mainPSConstantData.image_count.x = (float) (gpup_emitter[enr].image_count+1);
	gpup_emitter[enr].mainPSConstantData.image_count.y = gpup_emitter[enr].image_distortion;
	gpup_emitter[enr].mainPSConstantData.image_count.z = gpup_emitter[enr].image_distortion_speed;
	
	gpup_emitter[enr].posConstantData.pos1.x = gpup_emitter[enr].emitter_x1;
	gpup_emitter[enr].posConstantData.pos1.y = gpup_emitter[enr].emitter_y1;
	gpup_emitter[enr].posConstantData.pos1.z = gpup_emitter[enr].emitter_z1;
	
	gpup_emitter[enr].posConstantData.pos2.x = gpup_emitter[enr].emitter_x2;
	gpup_emitter[enr].posConstantData.pos2.y = gpup_emitter[enr].emitter_y2;
	gpup_emitter[enr].posConstantData.pos2.z = gpup_emitter[enr].emitter_z2;
		
	int bop = 0;
	int subs = gpup_getSubPositionCount( enr );
	for( int i = 0; i < 8; i++ )
	{
		if ( gpup_emitter[enr].subEmitterUsed[i] == 1 )
		{
			gpup_emitter[enr].mainVSConstantData.globalpos[bop].x = gpup_emitter[enr].globalx[i];
			gpup_emitter[enr].mainVSConstantData.globalpos[bop].y = gpup_emitter[enr].globaly[i];
			gpup_emitter[enr].mainVSConstantData.globalpos[bop].z = gpup_emitter[enr].globalz[i];
			gpup_emitter[enr].mainVSConstantData.globalpos[bop].w = (float) subs;

			bop++;
		}
	}
		
	gpup_emitter[enr].mainVSConstantData.globalsize.x = gpup_emitter[enr].globalSize;
	gpup_emitter[enr].mainVSConstantData.globalsize.y = gpup_emitter[enr].globalSize;
	gpup_emitter[enr].mainVSConstantData.globalsize.z = gpup_emitter[enr].globalSize;
	
	gpup_emitter[enr].mainVSConstantData.globalrot.x = 0;// gpup_emitter[enr].globalRotX; set in gpup_setGlobalRotation
	gpup_emitter[enr].mainVSConstantData.globalrot.y = 0;// gpup_emitter[enr].globalRotY;
	gpup_emitter[enr].mainVSConstantData.globalrot.z = 0;// gpup_emitter[enr].globalRotZ;

	gpup_emitter[enr].speedConstantData.reffloor.x = gpup_emitter[enr].floorActive;
	gpup_emitter[enr].speedConstantData.reffloor.y = gpup_emitter[enr].floorHeight;
	gpup_emitter[enr].speedConstantData.reffloor.z = gpup_emitter[enr].sphereRadius;
	gpup_emitter[enr].speedConstantData.reffloor.w = gpup_emitter[enr].bounciness ;
	
	gpup_emitter[enr].speedConstantData.refsphere.x = gpup_emitter[enr].sphereActive;
	gpup_emitter[enr].speedConstantData.refsphere.y = gpup_emitter[enr].sphereX;
	gpup_emitter[enr].speedConstantData.refsphere.z = gpup_emitter[enr].sphereY;
	gpup_emitter[enr].speedConstantData.refsphere.w = gpup_emitter[enr].sphereZ ;
	
	gpup_emitter[enr].posConstantData.localemitter.x = gpup_emitter[enr].emitter_local_x*4.0f-2.0f;
	gpup_emitter[enr].posConstantData.localemitter.y = gpup_emitter[enr].emitter_local_y*4.0f-2.0f;
	gpup_emitter[enr].posConstantData.localemitter.z = gpup_emitter[enr].emitter_local_z*4.0f-2.0f;
}

void gpup_addEmitter( int enr, int selcol )
{	
	GraphicsDevice* device = wiRenderer::GetDevice();

	gpup_emitter[enr].mainVSConstantData.globalsize.x = 1;
	gpup_emitter[enr].mainVSConstantData.globalsize.y = 1;
	gpup_emitter[enr].mainVSConstantData.globalsize.z = 1;
	
	gpup_emitter[enr].mainVSConstantData.globalrot.x = 0;
	gpup_emitter[enr].mainVSConstantData.globalrot.y = 0;
	gpup_emitter[enr].mainVSConstantData.globalrot.z = 0;
	
	gpup_emitter[enr].mainVSConstantData.particles.x = (float) selcol;
	gpup_emitter[enr].mainVSConstantData.particles.y = (float) (selcol*selcol);
	gpup_emitter[enr].mainVSConstantData.particles.z = (float) (selcol*selcol-1);
	gpup_emitter[enr].mainVSConstantData.particles.w = (float) gpup_emitter[enr].facing ;
	
	gpup_emitter[enr].posConstantData.particles.x = (float) selcol;
	gpup_emitter[enr].posConstantData.particles.y = (float) (selcol*selcol);
	gpup_emitter[enr].posConstantData.particles.z = (float) (selcol*selcol-1);
	
	gpup_emitter[enr].speedConstantData.particles.x = (float) selcol;
	gpup_emitter[enr].speedConstantData.particles.y = (float) (selcol*selcol);
	gpup_emitter[enr].speedConstantData.particles.z = (float) (selcol*selcol-1);
		
	gpup_emitter[enr].particles = selcol;
	int particles = selcol;	

	gpup_emitter[enr].currImage = 0;

	// Main Textures
	GPUP_CreateRenderTexture( particles*2, particles*2, &gpup_emitter[enr].texPos[0] );
	GPUP_CreateRenderTexture( particles*2, particles*2, &gpup_emitter[enr].texPos[1] );

	GPUP_CreateRenderTexture( particles*2, particles*2, &gpup_emitter[enr].texSpeed[0] );
	GPUP_CreateRenderTexture( particles*2, particles*2, &gpup_emitter[enr].texSpeed[1] );

	GPUP_CreateRenderTexture( 4096, 64, &gpup_emitter[enr].texNoise );

	// render passes for above textures
	RenderPassDesc renderDesc;
	renderDesc.attachments = {{ RenderPassAttachment::RENDERTARGET, RenderPassAttachment::LOADOP_DONTCARE, &gpup_emitter[enr].texPos[0], -1 }};
	device->CreateRenderPass( &renderDesc, &gpup_emitter[enr].renderPassPos[0] );

	renderDesc.attachments = {{ RenderPassAttachment::RENDERTARGET, RenderPassAttachment::LOADOP_DONTCARE, &gpup_emitter[enr].texPos[1], -1 }};
	device->CreateRenderPass( &renderDesc, &gpup_emitter[enr].renderPassPos[1] );
	
	renderDesc.attachments = {{ RenderPassAttachment::RENDERTARGET, RenderPassAttachment::LOADOP_DONTCARE, &gpup_emitter[enr].texSpeed[0], -1 }};
	device->CreateRenderPass( &renderDesc, &gpup_emitter[enr].renderPassSpeed[0] );

	renderDesc.attachments = {{ RenderPassAttachment::RENDERTARGET, RenderPassAttachment::LOADOP_DONTCARE, &gpup_emitter[enr].texSpeed[1], -1 }};
	device->CreateRenderPass( &renderDesc, &gpup_emitter[enr].renderPassSpeed[1] );

	renderDesc.attachments = {{ RenderPassAttachment::RENDERTARGET, RenderPassAttachment::LOADOP_DONTCARE, &gpup_emitter[enr].texNoise, -1 }};
	device->CreateRenderPass( &renderDesc, &gpup_emitter[enr].renderPassNoise );
			
	gpup_emitter[enr].testpos = 0;
	gpup_emitter[enr].emitter_burst_delaycount = 5;
		
	gpup_setBlendmode( enr, gpup_emitter[enr].blendmode );
	gpup_emitter[enr].maxed = 0;
	gpup_settings.gtimer = 0;
	gpup_settings.pauser = 3;
	gpup_updatesettings( enr );

	gpup_emitter[enr].posConstantData.spawnpos.x = -3.0f;
	gpup_emitter[enr].posConstantData.spawnpos.y = -2.0f;
	
	gpup_emitter[enr].speedConstantData.spawnpos.x = -3.0f;
	gpup_emitter[enr].speedConstantData.spawnpos.y = -2.0f;

	gpup_emitter[enr].noiseOff2 = 0.0000152587890625f * Random();
	gpup_emitter[enr].noiseOff1 = 0.0f;
	gpup_emitter[enr].sizer = 1.0f;
	
	noiseConstantData.off.x = gpup_emitter[enr].noiseOff1;
	noiseConstantData.off.y = gpup_emitter[enr].noiseOff2;
	noiseConstantData.off.z = 0;

	gpup_emitter[enr].effectVisible = 1;
}

int gpup_loadEffectFile( const char* fl, float x, float y, float z, float s, float qual, float ps )
{
	int   tmpint = 0;
	float tmpfloat = 0;	
	
	//gpup_maxeffects
	int enr = -1;
	gpup_settings.emitterCount = 0;
	for( int i = 0; i < gpup_maxeffects; i++ )
	{
		if ( gpup_emitter[i].effectLoaded == 0 && enr == -1 ) enr = i;
		if ( gpup_emitter[i].effectLoaded == 1 ) gpup_settings.emitterCount = gpup_settings.emitterCount + 1;
	}
		
	if ( enr <= -1 ) return -1;
		
	gpup_emitter[enr].globalx[0] = x;
	gpup_emitter[enr].globaly[0] = y;
	gpup_emitter[enr].globalz[0] = z;
		
	gpup_emitter[enr].globalSize = s;
	gpup_emitter[enr].particleSize = ps;
		
	gpup_emitter[enr].subEmitters = 1;
	gpup_emitter[enr].subEmitterUsed[0] = 1;
		
	gpup_emitter[enr].r = 1.0f;
	gpup_emitter[enr].g = 1.0f;
	gpup_emitter[enr].b = 1.0f;

	if ( *fl == 0 ) return -1;
		
	std::string flo = fl;
	flo += ".arx";

	if ( !GetFileExists( flo.c_str() ) ) return -1;
			
	// load the effect
	OpenToRead( 1, flo.c_str() );
				
	gpup_emitter[enr].name = fl;
	AGKDumpString( 1 );
					
	gpup_emitter[enr].x = AGKReadFloat( 1 );
	gpup_emitter[enr].y = AGKReadFloat( 1 );
	gpup_emitter[enr].z = AGKReadFloat( 1 );
	gpup_emitter[enr].scale = AGKReadFloat( 1 );
	int selcol = (int) AGKReadFloat( 1 );
	selcol = (int) floor( selcol * qual );
	if ( selcol < 32 ) selcol = 32;
	if ( selcol > 1024 ) selcol = 1024;
				
	gpup_emitter[enr].fsize = AGKReadFloat( 1 );
	gpup_emitter[enr].frez = ReadInteger( 1 );
				
	gpup_emitter[enr].emiton = ReadInteger( 1 );

	gpup_emitter[enr].area = AGKReadFloat( 1 );
	gpup_emitter[enr].varea = AGKReadFloat( 1 );
	gpup_emitter[enr].narea = AGKReadFloat( 1 );
			
	gpup_emitter[enr].blendmode = ReadInteger( 1 );
	gpup_emitter[enr].col = ReadInteger( 1 );
			
	gpup_emitter[enr].layer_count = ReadInteger( 1 );

	gpup_emitter[enr].field_adhesion = AGKReadFloat( 1 );
	gpup_emitter[enr].field_x = AGKReadFloat( 1 );
	gpup_emitter[enr].field_y = AGKReadFloat( 1 );
	gpup_emitter[enr].field_z = AGKReadFloat( 1 );
			
	gpup_emitter[enr].image_count = ReadInteger( 1 );
	AGKDumpString( 1 );
	AGKDumpString( 1 );
	AGKDumpString( 1 );
	AGKDumpString( 1 );
	AGKDumpString( 1 );
	AGKDumpString( 1 );
	AGKDumpString( 1 );
				
	gpup_emitter[enr].noise_adhesion = AGKReadFloat( 1 );
	gpup_emitter[enr].offset_speed = AGKReadFloat( 1 );
	gpup_emitter[enr].auto_rot_x = AGKReadFloat(1);
	gpup_emitter[enr].auto_rot_y = AGKReadFloat(1);
	gpup_emitter[enr].auto_rot_z = AGKReadFloat(1);
	gpup_emitter[enr].noise_bias = AGKReadFloat( 1 );
				
	gpup_emitter[enr].gravity_x = AGKReadFloat( 1 );
	gpup_emitter[enr].gravity_y = AGKReadFloat( 1 );
	gpup_emitter[enr].gravity_z = AGKReadFloat( 1 );
				
	gpup_emitter[enr].lifespan = AGKReadFloat( 1 );
	gpup_emitter[enr].lifespan_variance = AGKReadFloat( 1 );
	gpup_emitter[enr].maxed = AGKReadFloat( 1 );
				
	gpup_emitter[enr].emitter_type = ReadInteger( 1 );
	gpup_emitter[enr].emitter_local_x = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_local_y = AGKReadFloat( 1 );
	gpup_emitter[enr].emitter_local_z = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_size_x = AGKReadFloat( 1 );
	gpup_emitter[enr].emitter_size_y = AGKReadFloat( 1 );
	gpup_emitter[enr].emitter_size_z = AGKReadFloat( 1 );
	gpup_emitter[enr].emitter_radius_1 = AGKReadFloat( 1 );
	gpup_emitter[enr].emitter_radius_2 = AGKReadFloat( 1 );
	gpup_emitter[enr].emitter_height = AGKReadFloat( 1 );
	gpup_emitter[enr].emitter_x1 = AGKReadFloat( 1 );
	gpup_emitter[enr].emitter_y1 = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_z1 = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_x2 = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_y2 = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_z2 = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_rotation_x = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_rotation_y = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_rotation_z = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_speed_x = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_speed_y = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_speed_z = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_speed_v = AGKReadFloat( 1 ); 
				
	gpup_emitter[enr].emitter_slow_mode = ReadInteger( 1 ); 
	gpup_emitter[enr].emitter_amount = AGKReadFloat( 1 ); 
				
	gpup_emitter[enr].emitter_burst_mode = ReadInteger( 1 ); 
	gpup_emitter[enr].emitter_burst_auto = ReadInteger( 1 ); 
	gpup_emitter[enr].emitter_burst_delay = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_burst_frames = ReadInteger( 1 ); 
	gpup_emitter[enr].emitter_burst_fire = ReadInteger( 1 ); 
	gpup_emitter[enr].emitter_burst_delaycount = (float) ReadInteger( 1 ); 
				
	gpup_emitter[enr].size1 = AGKReadFloat( 1 ); 
	gpup_emitter[enr].size2 = AGKReadFloat( 1 ); 
	gpup_emitter[enr].rotation = AGKReadFloat( 1 ); 
	gpup_emitter[enr].rotation_variance = AGKReadFloat( 1 ); 
				
	gpup_emitter[enr].emitter_wire = ReadInteger( 1 );
	gpup_emitter[enr].facing = ReadInteger( 1 );
				
	gpup_emitter[enr].emitter_auto_rotation_x = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_auto_rotation_y = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_auto_rotation_z = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_auto_movement = ReadInteger( 1 );
				
	gpup_emitter[enr].emitter_line_sway_speed = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_line_sway = AGKReadFloat( 1 ); 
	gpup_emitter[enr].emitter_line_sway_size = AGKReadFloat( 1 ); 
				
	gpup_emitter[enr].image_distortion = AGKReadFloat( 1 );
	gpup_emitter[enr].image_distortion_speed = AGKReadFloat( 1 );
				
	tmpint = ReadInteger( 1 );
	if ( tmpint > 900 ) tmpint = 0;
	gpup_emitter[enr].emitter_corners = tmpint;
	tmpint = ReadInteger( 1 );
	if ( tmpint > 900 ) tmpint = 0;
	gpup_emitter[enr].emitter_moving = tmpint;
	tmpint = ReadInteger( 1 );
	if ( tmpint > 900 ) tmpint = 0;
	gpup_emitter[enr].ani_x = tmpint;
	tmpint = ReadInteger( 1 );
	if ( tmpint > 900 ) tmpint = 0;
	gpup_emitter[enr].ani_y = tmpint;
	tmpint = ReadInteger( 1 );
	if ( tmpint > 900 ) tmpint = 0;
	gpup_emitter[enr].ani_s = tmpint;
	tmpint = ReadInteger( 1 );
	if ( tmpint > 900 ) tmpint = 0;
	gpup_emitter[enr].smooth_blending = tmpint;
	tmpint = ReadInteger( 1 );
	tmpint = ReadInteger( 1 );
	tmpint = ReadInteger( 1 );
	tmpint = ReadInteger( 1 );
				
	tmpint = ReadInteger( 1 );
	tmpint = ReadInteger( 1 );
	tmpint = ReadInteger( 1 );
	tmpint = ReadInteger( 1 );
	tmpint = ReadInteger( 1 );
	tmpint = ReadInteger( 1 );
	tmpint = ReadInteger( 1 );
	tmpint = ReadInteger( 1 );
	tmpint = ReadInteger( 1 );
	tmpint = ReadInteger( 1 );
				
	tmpfloat = AGKReadFloat( 1 );
	if ( tmpfloat > 900 ) tmpfloat = 0.5f;
	gpup_emitter[enr].emitter_moving_speed = tmpfloat;
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
							 
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
	tmpfloat = AGKReadFloat( 1 );
				
	for( int i = 0; i <= gpup_emitter[enr].layer_count; i++ )
	{
		int reflayer = 0;
		tmpint = ReadInteger( 1 );
		if ( tmpint == 1 ) reflayer = 1;
				
		float local_x = AGKReadFloat( 1 );
		float local_y = AGKReadFloat( 1 );
		float local_z = AGKReadFloat( 1 );
					
		float rotation_x = AGKReadFloat( 1 );
		float rotation_y = AGKReadFloat( 1 );
		float rotation_z = AGKReadFloat( 1 );
					
		tmpfloat = AGKReadFloat( 1 );
		tmpfloat = AGKReadFloat( 1 );
					
		float bounciness = AGKReadFloat( 1 );
					
		for( int j = 0; j < 11; j++ ) tmpfloat = AGKReadFloat( 1 );
					
		float radius_1 = AGKReadFloat( 1 );
		float radius_2 = AGKReadFloat( 1 );
					
		tmpfloat = AGKReadFloat( 1 );
		tmpfloat = AGKReadFloat( 1 );					
					
		if ( reflayer == 1 )
		{
			gpup_emitter[enr].bounciness = bounciness;
			gpup_emitter[enr].floorActive = rotation_x;
			gpup_emitter[enr].sphereActive = rotation_y;
			gpup_emitter[enr].sphereRadius = radius_1;
			gpup_emitter[enr].floorHeight = radius_2;
			gpup_emitter[enr].sphereX = local_x;
			gpup_emitter[enr].sphereY = local_y;
			gpup_emitter[enr].sphereZ = local_z;
		}				
	}
				
	AGKCloseFile( 1 );

	// copy original speed vector into adjustable vector (can now be altered in realtime)
	gpup_emitter[enr].emitter_speedadjustment_x = gpup_emitter[enr].emitter_speed_x;
	gpup_emitter[enr].emitter_speedadjustment_y = gpup_emitter[enr].emitter_speed_y;
	gpup_emitter[enr].emitter_speedadjustment_z = gpup_emitter[enr].emitter_speed_z;
				
	gpup_addEmitter( enr, selcol );
				
	flo = fl; flo += "_g.png";
	if ( !GetFileExists(flo.c_str()) ) return -1;
	
	GPUP_LoadTexture( flo.c_str(), &gpup_emitter[enr].gradient_1 );
				
	if ( gpup_emitter[enr].ani_x > 0 )
	{
		flo = fl; flo += "_s1.png";
		if ( !GetFileExists(flo.c_str()) ) return -1;

		GPUP_LoadTexture( flo.c_str(), &gpup_emitter[enr].image1 );
	}
				
	flo = fl; flo += "_sx.png";
	if ( !GetFileExists(flo.c_str()) ) return -1;

	GPUP_LoadTexture( flo.c_str(), &gpup_emitter[enr].imagex );
				
	gpup_emitter[enr].mainPSConstantData.tile.x = (float) gpup_emitter[enr].ani_x;
	gpup_emitter[enr].mainPSConstantData.tile.y = (float) gpup_emitter[enr].ani_y;
	gpup_emitter[enr].mainPSConstantData.tile.z = (float) gpup_emitter[enr].ani_s;
	gpup_emitter[enr].mainPSConstantData.tile.w = (float) gpup_emitter[enr].smooth_blending ;
	
	gpup_emitter[enr].mainVSConstantData.tilex.x = (float) gpup_emitter[enr].ani_x;
	gpup_emitter[enr].mainVSConstantData.tilex.y = (float) gpup_emitter[enr].ani_y;
	gpup_emitter[enr].mainVSConstantData.tilex.z = (float) gpup_emitter[enr].ani_s;
	gpup_emitter[enr].mainVSConstantData.tilex.w = (float) gpup_emitter[enr].smooth_blending ;
				
	gpup_emitter[enr].effectLoaded = 1;
	gpup_emitter[enr].pivotMoved = 0;
	gpup_settings.emitterCount = gpup_settings.emitterCount + 1;
				
	gpup_emitter[enr].mainVSConstantData.image_count.x = (float) (gpup_emitter[enr].image_count+1);
	gpup_emitter[enr].mainVSConstantData.image_count.y = 0;
	gpup_emitter[enr].mainVSConstantData.image_count.z = 0;
	
	gpup_emitter[enr].mainPSConstantData.image_count.x = (float) (gpup_emitter[enr].image_count+1);
	gpup_emitter[enr].mainPSConstantData.image_count.y = 0;
	gpup_emitter[enr].mainPSConstantData.image_count.z = 0;
				
	gpup_setBlendmode( enr, gpup_emitter[enr].blendmode );
				
	gpup_emitter[enr].emitter_burst_delaycount = gpup_emitter[enr].emitter_burst_delay*550.0f + 22.0f;
				
	flo = fl; flo += "_r.png";
	if ( !GetFileExists(flo.c_str()) ) return -1;
	
	GPUP_LoadTexture( flo.c_str(), &gpup_emitter[enr].t_field );
					
	gpup_emitter[enr].testpos = 0;
	gpup_emitter[enr].spawnint = 0;
	gpup_emitter[enr].noiseZaehler = 0;
	gpup_settings.gtimer = 0;

	gpup_emitter[enr].emitter_animation_speed = 1.0f;

	gpup_updatesettings( enr );
		
	return enr;
}

void gpup_spawnit( int enr, int spawnint )
{
	int pr = gpup_emitter[enr].particles;
	pr = pr * pr;
	
	spawnint = spawnint * gpup_getSubPositionCount( enr );
	
	if ( spawnint > 0 )
	{
		gpup_emitter[enr].activeTimer = 1.12f;
		gpup_emitter[enr].testpos = gpup_emitter[enr].testpos + spawnint;
		if ( gpup_emitter[enr].testpos > pr-1 ) gpup_emitter[enr].testpos = gpup_emitter[enr].testpos - pr;
		
		gpup_emitter[enr].posConstantData.spawnpos.x = (float) gpup_emitter[enr].testpos;
		gpup_emitter[enr].posConstantData.spawnpos.y = (float) (gpup_emitter[enr].testpos+spawnint-1);
		
		gpup_emitter[enr].speedConstantData.spawnpos.x = (float) gpup_emitter[enr].testpos;
		gpup_emitter[enr].speedConstantData.spawnpos.y = (float) (gpup_emitter[enr].testpos+spawnint-1);
	}
	else
	{
		gpup_emitter[enr].posConstantData.spawnpos.x = -1000;
		gpup_emitter[enr].posConstantData.spawnpos.y = -999;
		
		gpup_emitter[enr].speedConstantData.spawnpos.x = -1000;
		gpup_emitter[enr].speedConstantData.spawnpos.y = -999;
	}
}


void  gpup_doit( int enr, CommandList cmd )
{
	GraphicsDevice* device = wiRenderer::GetDevice();
	float emitter_time = gpup_settings.time * gpup_emitter[enr].emitter_animation_speed;

	gpup_emitter[enr].speedConstantData.warp = emitter_time;
	gpup_emitter[enr].posConstantData.warp = emitter_time;
		
	if ( gpup_settings.pauser > 0 ) gpup_settings.pauser = gpup_settings.pauser - 1;
	
	if ( gpup_settings.simulOn == 1 )
	{		
		if ( gpup_emitter[enr].noiseDir == 0 )
		{
			gpup_emitter[enr].noiseZaehler = gpup_emitter[enr].noiseZaehler + gpup_emitter[enr].offset_speed * 2.5f * emitter_time;
			if ( gpup_emitter[enr].noiseZaehler > 360.0f )
			{
				gpup_emitter[enr].noiseZaehler = 360.0f;
				gpup_emitter[enr].noiseDir = 1;
				gpup_emitter[enr].noiseOff1 = 0.0000152587890625f * Random();
			}
		}
		else
		{
			gpup_emitter[enr].noiseZaehler = gpup_emitter[enr].noiseZaehler - gpup_emitter[enr].offset_speed * 2.5f * emitter_time;
			if ( gpup_emitter[enr].noiseZaehler < 0.0f )
			{
				gpup_emitter[enr].noiseZaehler = 0.0f;
				gpup_emitter[enr].noiseDir = 0;
				gpup_emitter[enr].noiseOff2 = 0.0000152587890625f * Random();
			}
		}

		noiseConstantData.off.x = gpup_emitter[enr].noiseOff1;
		noiseConstantData.off.y = gpup_emitter[enr].noiseOff2;
		noiseConstantData.off.z = gpup_emitter[enr].noiseZaehler/360.0f;

		device->UpdateBuffer( &noiseConstants, &noiseConstantData.off, cmd, sizeof(sNoiseConstantData) );
		device->BindConstantBuffer( PS, &noiseConstants, 0, cmd );
		device->BindPipelineState( &psoNoise, cmd );
		device->BindResource( PS, &texNoiseOrig, 0, cmd );
		GPUParticlesDrawQuad( &gpup_emitter[enr].renderPassNoise, cmd );

		// emittance stats
		int buf = gpup_emitter[enr].particles * gpup_emitter[enr].particles;
		float lif = gpup_emitter[enr].lifespan * 60.0f * 10.0f;
		float per = buf / lif;
		gpup_emitter[enr].maxed = per;
				
		if ( gpup_emitter[enr].emitter_burst_mode == 0 )
		{
			//PE: Looping on, should respect emitter_burst_auto. Just like in the particle editor.
			//PE: https://github.com/TheGameCreators/GameGuruRepo/issues/5929#issuecomment-2740317619
			if (gpup_emitter[enr].emitter_burst_auto == 1 && gpup_emitter[enr].emitterActive == 1)
			{
				gpup_emitter[enr].spawnint = 0;
				if (gpup_emitter[enr].emitter_burst_fire > 0)
				{
					gpup_emitter[enr].spawnint = buf * 0.9f * gpup_emitter[enr].emitter_amount;
					gpup_emitter[enr].emitter_burst_fire = gpup_emitter[enr].emitter_burst_fire - 1;
					gpup_settings.pauser = 0; //PE: Fire it right now.
				}
				if (gpup_emitter[enr].emitter_burst_auto == 1 && gpup_emitter[enr].emitterActive == 1)
				{
					gpup_emitter[enr].emitter_burst_delaycount = gpup_emitter[enr].emitter_burst_delaycount + 1.0f * emitter_time;
					if (gpup_emitter[enr].emitter_burst_delaycount > (gpup_emitter[enr].emitter_burst_delay * 550.0f + 30.0f))
					{
						gpup_emitter[enr].spawnint = buf * 0.9f * gpup_emitter[enr].emitter_amount;
						gpup_emitter[enr].emitter_burst_delaycount = 0;
						gpup_emitter[enr].emitter_burst_fire = 1;
					}
				}
			}
			else
			{
				float spawnamount = gpup_emitter[enr].emitter_amount * gpup_emitter[enr].maxed * emitter_time;
				if (gpup_emitter[enr].emitter_slow_mode == 1) spawnamount = spawnamount / 16.0f;
				gpup_emitter[enr].spawnint = spawnamount;
				if (spawnamount < 1)
				{
					gpup_emitter[enr].spawnCount = gpup_emitter[enr].spawnCount + 1;
					if (gpup_emitter[enr].spawnCount > (1.0f / spawnamount))
					{
						gpup_emitter[enr].spawnint = 1;
						gpup_emitter[enr].spawnCount = 0;
					}
				}
			}
		}
		else
		{
			//PE: Looping off , so only single burst here.
			gpup_emitter[enr].spawnint = 0;
			if ( gpup_emitter[enr].emitter_burst_fire > 0 )
			{
				gpup_emitter[enr].spawnint = buf * 0.9f * gpup_emitter[enr].emitter_amount;
				gpup_emitter[enr].emitter_burst_fire = gpup_emitter[enr].emitter_burst_fire - 1;
				gpup_settings.pauser = 0; //PE: Fire it right now.
			}
			//PE: Moved so we only trigger one explosion when looping off. moved to above when looping on.
			/*
			if ( gpup_emitter[enr].emitter_burst_auto == 1 && gpup_emitter[enr].emitterActive == 1 )
			{
				gpup_emitter[enr].emitter_burst_delaycount = gpup_emitter[enr].emitter_burst_delaycount + 1.0f * emitter_time;
				if ( gpup_emitter[enr].emitter_burst_delaycount > (gpup_emitter[enr].emitter_burst_delay * 550.0f + 30.0f) )
				{
					gpup_emitter[enr].spawnint = buf * 0.9f * gpup_emitter[enr].emitter_amount;
					gpup_emitter[enr].emitter_burst_delaycount = 0;
					gpup_emitter[enr].emitter_burst_fire = 1;
				}
			}
			*/
		}
				
		if ( gpup_emitter[enr].activeTimer > 0.0f )
		{
			if ( gpup_settings.split == 0 ) gpup_settings.activeEffects1 = gpup_settings.activeEffects1 + 1;
			if ( gpup_settings.split == 1 ) gpup_settings.activeEffects2 = gpup_settings.activeEffects2 + 1;
			
			if ( gpup_emitter[enr].pivotMoved > 0 )
			{
				float s = gpup_emitter[enr].globalSize;
				float newX = (gpup_emitter[enr].globalx[0] - gpup_emitter[enr].newX) / s;
				float newY = (gpup_emitter[enr].globaly[0] - gpup_emitter[enr].newY) / s;
				float newZ = (gpup_emitter[enr].globalz[0] - gpup_emitter[enr].newZ) / s;

				gpup_emitter[enr].posConstantData.moveit.x = newX;
				gpup_emitter[enr].posConstantData.moveit.y = newY;
				gpup_emitter[enr].posConstantData.moveit.z = newZ;

				gpup_emitter[enr].pivotMoved = 0;
				gpup_emitter[enr].globalx[0] = gpup_emitter[enr].newX;
				gpup_emitter[enr].globaly[0] = gpup_emitter[enr].newY;
				gpup_emitter[enr].globalz[0] = gpup_emitter[enr].newZ;
				
				gpup_emitter[enr].mainVSConstantData.globalpos[0].x = gpup_emitter[enr].globalx[0];
				gpup_emitter[enr].mainVSConstantData.globalpos[0].y = gpup_emitter[enr].globaly[0];
				gpup_emitter[enr].mainVSConstantData.globalpos[0].z = gpup_emitter[enr].globalz[0];
				gpup_emitter[enr].mainVSConstantData.globalpos[0].w = (float) gpup_getSubPositionCount(enr) ;
			}

			int currImage = gpup_emitter[enr].currImage;
			int nextImage = 1 - currImage;

			// render speed image
			device->UpdateBuffer( &speedConstants, &gpup_emitter[enr].speedConstantData, cmd, sizeof(sSpeedConstantData) );
			device->BindConstantBuffer( PS, &speedConstants, 0, cmd );
			device->BindPipelineState( &psoSpeed, cmd );
			device->BindResource( PS, &gpup_emitter[enr].texPos[currImage], 0, cmd );
			device->BindResource( PS, &gpup_emitter[enr].texSpeed[currImage], 1, cmd );
			device->BindResource( PS, &gpup_emitter[enr].texNoise, 2, cmd );
			device->BindResource( PS, &gpup_emitter[enr].t_field, 3, cmd );
			GPUParticlesDrawQuad( &gpup_emitter[enr].renderPassSpeed[nextImage], cmd );

			gpup_emitter[enr].posConstantData.agk_time = AGKTimer();
			
			// render pos image
			device->UpdateBuffer( &posConstants, &gpup_emitter[enr].posConstantData, cmd, sizeof(sPosConstantData) );
			device->BindConstantBuffer( PS, &posConstants, 0, cmd );
			device->BindPipelineState( &psoPos, cmd );
			device->BindResource( PS, &gpup_emitter[enr].texPos[currImage], 0, cmd );
			device->BindResource( PS, &gpup_emitter[enr].texSpeed[nextImage], 1, cmd );
			GPUParticlesDrawQuad( &gpup_emitter[enr].renderPassPos[nextImage], cmd );
				
			gpup_emitter[enr].currImage = nextImage;
			
			gpup_emitter[enr].posConstantData.moveit.x = 0;
			gpup_emitter[enr].posConstantData.moveit.y = 0;
			gpup_emitter[enr].posConstantData.moveit.z = 0;
		}
			
		gpup_emitter[enr].posConstantData.spawnpos.x = -3.0f;
		gpup_emitter[enr].posConstantData.spawnpos.y = -2.0f;
		
		gpup_emitter[enr].speedConstantData.spawnpos.x = -3.0f;
		gpup_emitter[enr].speedConstantData.spawnpos.y = -2.0f;
		
		if ( gpup_emitter[enr].emiton == 1 || gpup_emitter[enr].emitter_burst_fire > 0 )
		{
			if ( gpup_settings.pauser < 1 && gpup_emitter[enr].emitterActive == 1 ) gpup_spawnit( enr, (int) gpup_emitter[enr].spawnint );
		}

		gpup_emitter[enr].posConstantData.rnd = gpup_settings.sn;
		
		gpup_emitter[enr].speedConstantData.rnd = gpup_settings.sn;
		gpup_emitter[enr].speedConstantData.rnd2 = Random2()/65536.0f;
		
		gpup_emitter[enr].mainVSConstantData.rota.x = gpup_emitter[enr].rotation-0.5f;
		gpup_emitter[enr].mainVSConstantData.rota.y = gpup_emitter[enr].rotation_variance;
		gpup_emitter[enr].mainVSConstantData.rota.z = gpup_settings.sn;

		float rotX = gpup_emitter[enr].emitter_rotation_x * 6.2831853f + gpup_settings.rotsn * (gpup_emitter[enr].emitter_auto_rotation_x - 0.5f);
		float rotY = gpup_emitter[enr].emitter_rotation_y * 6.2831853f + gpup_settings.rotsn * (gpup_emitter[enr].emitter_auto_rotation_y - 0.5f);
		float rotZ = gpup_emitter[enr].emitter_rotation_z * 6.2831853f + gpup_settings.rotsn * (gpup_emitter[enr].emitter_auto_rotation_z - 0.5f);

		gpup_emitter[enr].posConstantData.emitterrotation.x = rotX;
		gpup_emitter[enr].posConstantData.emitterrotation.y = rotY;
		gpup_emitter[enr].posConstantData.emitterrotation.z = rotZ;
		gpup_emitter[enr].posConstantData.emitterrotation.w = (float) gpup_emitter[enr].emitter_wire ; 
		
		float lifedrain = 0.998f / (gpup_emitter[enr].lifespan * 60 * 10);
		gpup_emitter[enr].activeTimer = gpup_emitter[enr].activeTimer - lifedrain;
		if ( gpup_emitter[enr].activeTimer < 0.0f ) gpup_emitter[enr].activeTimer = 0.0f;
	}
}

// initialize the system
int gpup_init()
{
	if ( gpu_particles_initialised ) return 1;
	gpu_particles_initialised = 1;

	GraphicsDevice* device = wiRenderer::GetDevice();

	AGKTimerInit();
	RandomInit();
	Random2Init();

	gpup_settings.simulOn = 1;
	gpup_settings.gtimer = 0;
	gpup_settings.spawnCount = 0;

	// shaders
	wiRenderer::LoadShader( VS, shaderQuadVS, "GPUP_QuadVS.cso" );
	wiRenderer::LoadShader( VS, shaderMainVS, "GPUP_MainVS.cso" );

	wiRenderer::LoadShader( PS, shaderQuadDefaultPS, "QuadDefaultPS.cso" );
	wiRenderer::LoadShader( PS, shaderNoisePS, "GPUP_NoisePS.cso" );
	wiRenderer::LoadShader( PS, shaderSpeedPS, "GPUP_SpeedPS.cso" );
	wiRenderer::LoadShader( PS, shaderPosPS, "GPUP_PosPS.cso" );
	wiRenderer::LoadShader( PS, shaderMainPS, "GPUP_MainPS.cso" );

	// images	
	if ( !GetFileExists("Files/effectbank/common/noise64.png") ) return -1;
	GPUP_LoadTexture( "Files/effectbank/common/noise64.png", &texNoiseOrig );

	if ( !GetFileExists("Files/effectbank/common/dist2.png") ) return -1;
	GPUP_LoadTexture( "Files/effectbank/common/dist2.png", &texDist2 );

	// pipeline state
	RasterizerState rasterDesc = {};
	rasterDesc.FillMode = FILL_SOLID;
	rasterDesc.CullMode = CULL_NONE;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0;
	rasterDesc.SlopeScaledDepthBias = 0;
	rasterDesc.DepthClipEnable = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.AntialiasedLineEnable = false;
	
	DepthStencilState depthDesc = {};
	depthDesc.DepthEnable = true;
	depthDesc.DepthWriteMask = DEPTH_WRITE_MASK_ZERO;
	depthDesc.DepthFunc = COMPARISON_GREATER_EQUAL;
	depthDesc.StencilEnable = false;
		
	BlendState blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
	blendDesc.IndependentBlendEnable = false;
	
	// input layout
	InputLayout layoutDesc;
	layoutDesc.elements =
	{
		{ "POSITION", 0, wiGraphics::FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_PER_VERTEX_DATA },
		{ "UV",       0, wiGraphics::FORMAT_R32G32_FLOAT,    0, 12, INPUT_PER_VERTEX_DATA },
	};
	
	// pipeline state objects
	PipelineStateDesc desc = {};
	desc.vs = &shaderMainVS;
	desc.ps = &shaderMainPS;

	// alpha
	desc.il = &layoutDesc;
	desc.pt = TRIANGLELIST;
	desc.rs = &rasterDesc;
	desc.dss = &depthDesc;
	desc.bs = &blendDesc;
	device->CreatePipelineState( &desc, &psoAlpha );

	// additive
	blendDesc.RenderTarget[0].DestBlend = BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = BLEND_ONE;
	device->CreatePipelineState( &desc, &psoAdd );

	// opaque
	depthDesc.DepthWriteMask = DEPTH_WRITE_MASK_ALL;
	blendDesc.RenderTarget[0].BlendEnable = false;
	device->CreatePipelineState( &desc, &psoOpaque );
	
	// Quad pipeline state
	InputLayout layoutDescQuad;
	layoutDescQuad.elements =
	{
		{ "POSITION", 0, wiGraphics::FORMAT_R32G32_FLOAT, 0, 0, INPUT_PER_VERTEX_DATA }		
	};
	
	depthDesc.DepthEnable = false;
	desc.vs = &shaderQuadVS;
	desc.ps = &shaderQuadDefaultPS;
	desc.il = &layoutDescQuad;
	device->CreatePipelineState( &desc, &psoQuadDefault );

	desc.ps = &shaderNoisePS;
	device->CreatePipelineState( &desc, &psoNoise );

	desc.ps = &shaderSpeedPS;
	device->CreatePipelineState( &desc, &psoSpeed );

	desc.ps = &shaderPosPS;
	device->CreatePipelineState( &desc, &psoPos );

	// constant buffers
	GPUBufferDesc bd = {};
	bd.Usage = USAGE_DEFAULT;
	bd.ByteWidth = sizeof(sPosConstantData);
	bd.BindFlags = BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, nullptr, &posConstants );

	bd.Usage = USAGE_DEFAULT;
	bd.ByteWidth = sizeof(sSpeedConstantData);
	bd.BindFlags = BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, nullptr, &speedConstants );

	bd.Usage = USAGE_DEFAULT;
	bd.ByteWidth = sizeof(sNoiseConstantData);
	bd.BindFlags = BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, nullptr, &noiseConstants );

	bd.Usage = USAGE_DEFAULT;
	bd.ByteWidth = sizeof(sMainVSConstantData);
	bd.BindFlags = BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, nullptr, &mainVSConstants );

	bd.Usage = USAGE_DEFAULT;
	bd.ByteWidth = sizeof(sMainPSConstantData);
	bd.BindFlags = BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, nullptr, &mainPSConstants );

	// 1024 index buffer (obj1)
	mainIndexCountObj1 = 6144;
	SubresourceData data = {};
	data.pSysMem = gpup_1024_indices;
	bd.ByteWidth = sizeof(unsigned short) * mainIndexCountObj1;
	bd.BindFlags = BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, &data, &mainIndexBufferObj1 );

	// 4096 index buffer (obj0)
	mainIndexCountObj0 = 24576;
	data.pSysMem = gpup_4096_indices;
	bd.ByteWidth = sizeof(unsigned short) * mainIndexCountObj0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, &data, &mainIndexBufferObj0 );

	// 1024 vertex buffer (obj1)
	data.pSysMem = gpup_1024_vertices;
	bd.ByteWidth = sizeof(GPUP_1024_Vertex) * 4096;
	bd.BindFlags = BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, &data, &mainVertexBufferObj1 );

	// 4096 vertex buffer (obj0)
	data.pSysMem = gpup_4096_vertices;
	bd.ByteWidth = sizeof(GPUP_4096_Vertex) * 16384;
	wiRenderer::GetDevice()->CreateBuffer( &bd, &data, &mainVertexBufferObj0 );

	// quad vertex buffer
	data.pSysMem = g_VerticesQuad;
	bd.ByteWidth = sizeof(VertexQuad) * 6;
	bd.BindFlags = BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	wiRenderer::GetDevice()->CreateBuffer( &bd, &data, &quadVertexBuffer );

	// samplers
	SamplerDesc samplerDesc = {};
	samplerDesc.AddressU = TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = TEXTURE_ADDRESS_CLAMP;
	samplerDesc.Filter = FILTER_MIN_MAG_MIP_POINT;
	device->CreateSampler( &samplerDesc, &samplerPoint );

	samplerDesc.Filter = FILTER_MIN_MAG_MIP_LINEAR;
	device->CreateSampler( &samplerDesc, &samplerLinear );

	samplerDesc.AddressU = TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = TEXTURE_ADDRESS_WRAP;
	device->CreateSampler( &samplerDesc, &samplerLinearWrap );

	//InitGPUParticlesTest();
	
	return 1;
}

// Load an Effect and set Global Position and Scale. The function returns the effect ID as an integer
int gpup_loadEffect( const char* fl, float x, float y, float z, float s )
{
	return gpup_loadEffectFile( fl, x, y, z, s, 1.0f, 1.0f );
}

// Load an Effect one quality level lower than specified in the effect file and set Global Position, Effect Scale and Particle Scale. This quarters the particle count of the effect. 
// The function returns the effect ID as an integer
int gpup_loadEffectLow( const char* fl, float x, float y, float z, float s, float ps )
{
	return gpup_loadEffectFile( fl, x, y, z, s, 0.5f, ps );
}

// Load an Effect one quality level higher than specified in the effect file and set Global Position, Effect Scale and Particle Scale. This quadruples the particle count of the effect. 
// The function returns the effect ID as an integer
int gpup_loadEffectHigh( const char* fl, float x, float y, float z, float s, float ps )
{
	return gpup_loadEffectFile( fl, x, y, z, s, 2.0f, ps );
}

// whether the emitter of an effect spawns particles (1), or not (0); -1 no such effect
int gpup_getEmitterActive( int ID )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return -1;
	return gpup_emitter[ID].emitterActive;
}

// Activate or deactivate the emitter of a specific effect. 
void gpup_emitterActive( int ID, int active )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;
	int iOldActiveState = gpup_emitter[ID].emitterActive;
	gpup_emitter[ID].emitterActive = active ? 1 : 0;

	//LB: each time an emitter is made active, do a burst fire (good for non looping one time particles)
	if (iOldActiveState == 0 && gpup_emitter[ID].emitterActive != 0 )
	{
		gpup_emitterFire(ID);
	}
}

// Set a new global position for the effect. Can be used to move an effect including all emitted particles and the turbulence
void gpup_setGlobalPosition( int ID, float x, float y, float z )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;

	gpup_emitter[ID].globalx[0] = x;
	gpup_emitter[ID].globaly[0] = y;
	gpup_emitter[ID].globalz[0] = z;
	if ( gpup_emitter[ID].effectLoaded == 1 )
	{
		gpup_emitter[ID].mainVSConstantData.globalpos[0].x = gpup_emitter[ID].globalx[0];
		gpup_emitter[ID].mainVSConstantData.globalpos[0].y = gpup_emitter[ID].globaly[0];
		gpup_emitter[ID].mainVSConstantData.globalpos[0].z = gpup_emitter[ID].globalz[0];
		gpup_emitter[ID].mainVSConstantData.globalpos[0].w = (float) gpup_getSubPositionCount(ID) ;
	}
}

// Added this so I can influence emitter direction!
void gpup_getEmitterSpeedAngleAdjustment(int ID, float* x, float* y, float* z)
{
	if (ID < 0 || ID >= gpup_maxeffects) return;
	*x = gpup_emitter[ID].emitter_speed_x;
	*y = gpup_emitter[ID].emitter_speed_y;
	*z = gpup_emitter[ID].emitter_speed_z;
}

void gpup_setEmitterSpeedAngleAdjustment(int ID, float x, float y, float z)
{
	if (ID < 0 || ID >= gpup_maxeffects) return;
	if (gpup_emitter[ID].emitter_type == 0 || gpup_emitter[ID].emitter_type == 2)
	{
		// only affects:
		// 0 = box spawn emitters (newly added for fountain directional)
		// 2 = smoke style emitters
		gpup_emitter[ID].emitter_speedadjustment_x = x;
		gpup_emitter[ID].emitter_speedadjustment_y = y;
		gpup_emitter[ID].emitter_speedadjustment_z = z;
		if (gpup_emitter[ID].effectLoaded == 1)
		{
			float fFinalSpeedX = gpup_emitter[ID].emitter_speedadjustment_x;
			float fFinalSpeedY = gpup_emitter[ID].emitter_speedadjustment_y;
			float fFinalSpeedZ = gpup_emitter[ID].emitter_speedadjustment_z;
			gpup_emitter[ID].speedConstantData.speedvar.x = (fFinalSpeedX - 0.5f) * 0.1f;
			gpup_emitter[ID].speedConstantData.speedvar.y = (fFinalSpeedY - 0.5f) * 0.1f;
			gpup_emitter[ID].speedConstantData.speedvar.z = (fFinalSpeedZ - 0.5f) * 0.1f;
		}
	}
}

// Set a new Global Porition for the effect without moving already existing particles. The vector and noise fields will be moved to the new position.
void gpup_setGlobalPivot( int ID, float x, float y, float z )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;

	gpup_emitter[ID].newX = x;
	gpup_emitter[ID].newY = y;
	gpup_emitter[ID].newZ = z;
	gpup_emitter[ID].pivotMoved = 1 ;
}

// Change the global scale of an effect
void gpup_setGlobalScale( int ID, float s )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;

	gpup_emitter[ID].globalSize = s;
	if ( gpup_emitter[ID].effectLoaded == 1 )
	{
		gpup_emitter[ID].mainVSConstantData.globalsize.x = gpup_emitter[ID].globalSize;
		gpup_emitter[ID].mainVSConstantData.globalsize.y = gpup_emitter[ID].globalSize;
		gpup_emitter[ID].mainVSConstantData.globalsize.z = gpup_emitter[ID].globalSize;
	}
}

// Change the size of each particle. This will only scale each particle sprite and not have an effect on the overall size and flow of the effect.
void gpup_setParticleScale( int ID, float s )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;

	gpup_emitter[ID].particleSize = s;
	if ( gpup_emitter[ID].effectLoaded == 1 ) 
	{
		gpup_emitter[ID].mainVSConstantData.pgrow.x = 0.001f+gpup_emitter[ID].size1*gpup_emitter[ID].sizer;
		gpup_emitter[ID].mainVSConstantData.pgrow.y = 0.001f+gpup_emitter[ID].size2*gpup_emitter[ID].sizer;
		gpup_emitter[ID].mainVSConstantData.pgrow.z = gpup_emitter[ID].particleSize;
	}
}

// Change the global rotation of an effect
#define MR_PI    ( ( float ) 3.141592654f )
void gpup_setGlobalRotation( int ID, float x, float y, float z )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;	
	gpup_emitter[ID].globalRotX = (x) * (MR_PI / 180.0f);
	gpup_emitter[ID].globalRotY = (y) * (MR_PI / 180.0f);
	gpup_emitter[ID].globalRotZ = (z) * (MR_PI / 180.0f);
	if ( gpup_emitter[ID].effectLoaded == 1 )
	{
		if (gpup_emitter[ID].emitter_type != 2)
		{
			// rings should be rotatable
			gpup_emitter[ID].mainVSConstantData.globalrot.x = gpup_emitter[ID].globalRotX;
			gpup_emitter[ID].mainVSConstantData.globalrot.y = gpup_emitter[ID].globalRotY;
			gpup_emitter[ID].mainVSConstantData.globalrot.z = gpup_emitter[ID].globalRotZ;
		}
		else
		{
			// for smoke projecting outward
			gpup_emitter[ID].mainVSConstantData.globalrot.x = 0;
			gpup_emitter[ID].mainVSConstantData.globalrot.y = 0;
			gpup_emitter[ID].mainVSConstantData.globalrot.z = 0;
		}
	}
}

// Set the local position of an emitter
void gpup_setLocalPosition( int ID, float x, float y, float z )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;

	// coordinates already passed in as relative to the particle global position
	gpup_emitter[ID].emitter_local_x = (x) / (gpup_emitter[ID].varea * gpup_emitter[ID].globalSize * 2.0f) + 0.5f;
	gpup_emitter[ID].emitter_local_y = (y) / (gpup_emitter[ID].varea * gpup_emitter[ID].globalSize * 2.0f) + 0.5f;
	gpup_emitter[ID].emitter_local_z = (z) / (gpup_emitter[ID].varea * gpup_emitter[ID].globalSize * 2.0f) + 0.5f;
	if ( gpup_emitter[ID].effectLoaded == 1 )
	{
		gpup_emitter[ID].posConstantData.localemitter.x = gpup_emitter[ID].emitter_local_x * 4.0f - 2.0f;
		gpup_emitter[ID].posConstantData.localemitter.y = gpup_emitter[ID].emitter_local_y * 4.0f - 2.0f;
		gpup_emitter[ID].posConstantData.localemitter.z = gpup_emitter[ID].emitter_local_z * 4.0f - 2.0f;
	}
}
void gpup_resetLocalPosition(int ID)
{
	if (ID < 0 || ID >= gpup_maxeffects) return;

	// ignores local offset passed in so emitter spawns from exact spot of particle game element (if called)
	gpup_emitter[ID].emitter_local_x = 0.5f;
	gpup_emitter[ID].emitter_local_y = 0.5f;
	gpup_emitter[ID].emitter_local_z = 0.5f;
	if (gpup_emitter[ID].effectLoaded == 1)
	{
		gpup_emitter[ID].posConstantData.localemitter.x = gpup_emitter[ID].emitter_local_x * 4.0f - 2.0f;
		gpup_emitter[ID].posConstantData.localemitter.y = gpup_emitter[ID].emitter_local_y * 4.0f - 2.0f;
		gpup_emitter[ID].posConstantData.localemitter.z = gpup_emitter[ID].emitter_local_z * 4.0f - 2.0f;
	}
}

// Set a Subemitter's position. Subemitter 0 is the overall global position of the effect.
void gpup_setSubPosition( int ID, int pos, float x, float y, float z )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;

	gpup_emitter[ID].globalx[pos] = x;
	gpup_emitter[ID].globaly[pos] = y;
	gpup_emitter[ID].globalz[pos] = z;
	if ( gpup_emitter[ID].effectLoaded == 1 ) gpup_updatesettings( ID );
}

// Add a sub-emitter
int gpup_addSubPosition( int ID, float x, float y, float z )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return 0;

	int se = 0;
	int i = 0;
		
	for( int i = 1; i < 8; i++ )
	{
		if ( gpup_emitter[ID].subEmitterUsed[i] == 0 )
		{
			se = i;
			break;
		}
	}
	if ( se == 0 ) return 0;

	gpup_emitter[ID].globalx[se] = x; //  /(gpup_emitter[ID].varea*gpup_emitter[ID].globalSize*2.0f)+0.5f
	gpup_emitter[ID].globaly[se] = y; //  /(gpup_emitter[ID].varea*gpup_emitter[ID].globalSize*2.0f)+0.5f
	gpup_emitter[ID].globalz[se] = z; //  /(gpup_emitter[ID].varea*gpup_emitter[ID].globalSize*2.0f)+0.5f
	gpup_emitter[ID].subEmitterUsed[se] = 1;
	gpup_emitter[ID].subEmitters = gpup_emitter[ID].subEmitters + 1;
	if ( gpup_emitter[ID].effectLoaded == 1 ) gpup_updatesettings( ID );
	
	return se;
}

// Delete a sub-emitter
int gpup_deleteSubPosition( int ID, int pos )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return 0;

	int se = -1;
	if ( pos > 0 && pos < 8 && gpup_emitter[ID].subEmitterUsed[pos] == 1 )
	{
		gpup_emitter[ID].subEmitterUsed[pos] = 0;
		gpup_emitter[ID].subEmitters = gpup_emitter[ID].subEmitters - 1;
		se = 1;
		if ( gpup_emitter[ID].effectLoaded == 1 ) gpup_updatesettings( ID );
	}
	
	return se;
}

// query the number of current sub-emitters
int gpup_getSubPositionCount( int ID )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return 0;

	int se = 0;
	for( int i = 0; i < 8; i++ )
	{
		if ( gpup_emitter[ID].subEmitterUsed[i] == 1 ) se++;
	}
	
	return se;
}

// Fire a burst emitter
void gpup_emitterBurstLoopAutoMode(int ID, int iAuto)
{
	if (ID < 0 || ID >= gpup_maxeffects) return;
	gpup_emitter[ID].emitter_burst_auto = iAuto;
}
void gpup_emitterBurstMode(int ID, int iMode)
{
	if (ID < 0 || ID >= gpup_maxeffects) return;
	gpup_emitter[ID].emitter_burst_mode = iMode;
}
void gpup_emitterFire(int ID)
{
	if (ID < 0 || ID >= gpup_maxeffects) return;
	gpup_emitter[ID].emitter_burst_fire = 1;
}

// Set the Blending Mode for an Effect. 
void gpup_setBlendmode( int ID, int usel )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;
	
	gpup_emitter[ID].blendmode = usel; // 0 = opaque, 1 = opaque, 2 = opaque, 3 = alpha, 4 = additive
		
	if ( gpup_emitter[ID].effectLoaded == 1 )
	{
		if ( usel == 0 ) usel = 1;
		gpup_emitter[ID].mainPSConstantData.moder = (float) usel;
	}
}

// Toggle the auto fire option for burst emitters
void gpup_setAutoFire( int ID, int act )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;
	
	gpup_emitter[ID].emitter_burst_auto = act;
}

// Sets the parameters of the floor reflection
void gpup_floorReflection( int ID, int active, float height ) 
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;
	
	gpup_emitter[ID].floorHeight = (height - gpup_emitter[ID].globaly[0]) / (gpup_emitter[ID].varea * gpup_emitter[ID].globalSize * 2.0f) + 0.5f;
	gpup_emitter[ID].floorActive = (active == 1) ? 1.0f : 0.0f;
	
	if ( gpup_emitter[ID].effectLoaded == 1 ) gpup_updatesettings( ID );
}
void gpup_restoreFloorReflection(int ID, int active, float height)
{
	if (ID < 0 || ID >= gpup_maxeffects) return;
	gpup_emitter[ID].floorHeight = height;
	gpup_emitter[ID].floorActive = (active == 1) ? 1.0f : 0.0f;
	if (gpup_emitter[ID].effectLoaded == 1) gpup_updatesettings(ID);
}

// Sets the parameters of the spherical reflection
void gpup_sphereReflection( int ID, int active, float x, float y, float z, float radius ) 
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;

	gpup_emitter[ID].sphereX = (x - gpup_emitter[ID].globalx[0]) / (gpup_emitter[ID].varea * gpup_emitter[ID].globalSize * 2.0f) + 0.5f;
	gpup_emitter[ID].sphereY = (y - gpup_emitter[ID].globaly[0]) / (gpup_emitter[ID].varea * gpup_emitter[ID].globalSize * 2.0f) + 0.5f;
	gpup_emitter[ID].sphereZ = (z - gpup_emitter[ID].globalz[0]) / (gpup_emitter[ID].varea * gpup_emitter[ID].globalSize * 2.0f) + 0.5f;
	gpup_emitter[ID].sphereRadius = radius / (gpup_emitter[ID].varea * gpup_emitter[ID].globalSize * 0.5f);
		
	gpup_emitter[ID].sphereActive = (active == 1) ? 1.0f : 0.0f;
	
	if ( gpup_emitter[ID].effectLoaded == 1 ) gpup_updatesettings( ID );
}

// Change the bounciness of the particles of an effect with reflections
void gpup_setBounciness( int ID, float value )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;
	
	if ( value < 0.0f ) value = 0.0f;
	if ( value > 1.0f ) value = 1.0f;
	gpup_emitter[ID].bounciness = value;
	if ( gpup_emitter[ID].effectLoaded == 1 ) gpup_updatesettings( ID );
}


// Sets the end points of line and beam emitters
void gpup_setLine( int ID, float x1, float y1, float z1, float x2, float y2, float z2 )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;

	gpup_emitter[ID].globalx[0] = (x1 + x2) * 0.5f;
	gpup_emitter[ID].globaly[0] = (y1 + y2) * 0.5f;
	gpup_emitter[ID].globalz[0] = (z1 + z2) * 0.5f;
	gpup_emitter[ID].emitter_x1 = x1 - gpup_emitter[ID].globalx[0];
	gpup_emitter[ID].emitter_y1 = y1 - gpup_emitter[ID].globaly[0];
	gpup_emitter[ID].emitter_z1 = z1 - gpup_emitter[ID].globalz[0];
	gpup_emitter[ID].emitter_x2 = x2 - gpup_emitter[ID].globalx[0];
	gpup_emitter[ID].emitter_y2 = y2 - gpup_emitter[ID].globaly[0];
	gpup_emitter[ID].emitter_z2 = z2 - gpup_emitter[ID].globalz[0];
	if ( gpup_emitter[ID].effectLoaded == 1 ) gpup_updatesettings( ID );
}

// Sets the end points of line and beam emitters
void gpup_setLineLocal( int ID, float x1, float y1, float z1, float x2, float y2, float z2 )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;

	gpup_emitter[ID].emitter_x1 = x1 - gpup_emitter[ID].globalx[0];
	gpup_emitter[ID].emitter_y1 = y1 - gpup_emitter[ID].globaly[0];
	gpup_emitter[ID].emitter_z1 = z1 - gpup_emitter[ID].globalz[0];
	gpup_emitter[ID].emitter_x2 = x2 - gpup_emitter[ID].globalx[0];
	gpup_emitter[ID].emitter_y2 = y2 - gpup_emitter[ID].globaly[0];
	gpup_emitter[ID].emitter_z2 = z2 - gpup_emitter[ID].globalz[0];
	if ( gpup_emitter[ID].effectLoaded == 1 ) gpup_updatesettings( ID );
}

// Adds more than one beam to a line emitter (up to 10) and sets the offset between the duplicated beams. These beams share
// the particles of the effect, so set the emit rate high enough to supportthe desired amount of beams. 
// The function also has a parameter to make the beam curl around it's main axis. 
void gpup_setLineBeams( int ID, int beamcount, float beamoffset, float beamcurl )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;

	if ( beamcount < 1 ) beamcount = 1;
	if ( beamcount > 10 ) beamcount = 10;
	gpup_emitter[ID].emitter_linebeams = (float) beamcount;
	gpup_emitter[ID].emitter_linebeamoffset = beamoffset;
	gpup_emitter[ID].emitter_lineswirl = beamcurl;
	if ( gpup_emitter[ID].effectLoaded == 1 ) gpup_updatesettings( ID );
}

// Set a global Tint for an effect. This helps to integrate alpha and clip blended effects into your scene. 
void gpup_setEffectColor( int ID, int r, int g, int b )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;
	
	gpup_emitter[ID].r = 0.00390625f * r;
	gpup_emitter[ID].g = 0.00390625f * g;
	gpup_emitter[ID].b = 0.00390625f * b;
	if ( gpup_emitter[ID].effectLoaded == 1 ) 
	{
		gpup_emitter[ID].mainPSConstantData.clr.x = gpup_emitter[ID].r;
		gpup_emitter[ID].mainPSConstantData.clr.y = gpup_emitter[ID].g;
		gpup_emitter[ID].mainPSConstantData.clr.z = gpup_emitter[ID].b;
	}
}

void gpup_setEffectOpacity(int ID, float a)
{
	if (ID < 0 || ID >= gpup_maxeffects) return;

	gpup_emitter[ID].opacity = a;
	if (gpup_emitter[ID].effectLoaded == 1)
	{
		gpup_emitter[ID].mainPSConstantData.opacity = gpup_emitter[ID].opacity;
	}
}

// Activate or deactivate smooth blending of spritesheet animations
void gpup_setSmoothBlending( int ID, int act )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;
	
	if ( gpup_emitter[ID].effectLoaded == 1  )
	{
		gpup_emitter[ID].smooth_blending = act;
		
		gpup_emitter[ID].mainPSConstantData.tile.x = (float) gpup_emitter[ID].ani_x;
		gpup_emitter[ID].mainPSConstantData.tile.y = (float) gpup_emitter[ID].ani_y;
		gpup_emitter[ID].mainPSConstantData.tile.z = (float) gpup_emitter[ID].ani_s;
		gpup_emitter[ID].mainPSConstantData.tile.w = (float) act;
		
		gpup_emitter[ID].mainVSConstantData.tilex.x = (float) gpup_emitter[ID].ani_x;
		gpup_emitter[ID].mainVSConstantData.tilex.y = (float) gpup_emitter[ID].ani_y;
		gpup_emitter[ID].mainVSConstantData.tilex.z = (float) gpup_emitter[ID].ani_s;
		gpup_emitter[ID].mainVSConstantData.tilex.w = (float) act;
	}
}

void gpup_deleteAllEffects(void)
{
	for (int i = 0; i < gpup_maxeffects; i++)
	{
		gpup_deleteEffect(i);
	}
}

// Delete a loaded effect. 
int gpup_deleteEffect( int ID )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return -1;
	if ( gpup_emitter[ID].effectLoaded == 0 ) return -1;
	
	GPUP_DeleteTexture( &gpup_emitter[ID].t_field );
	GPUP_DeleteTexture( &gpup_emitter[ID].texNoise );
	GPUP_DeleteTexture( &gpup_emitter[ID].texPos[0] );
	GPUP_DeleteTexture( &gpup_emitter[ID].texPos[1] );
	GPUP_DeleteTexture( &gpup_emitter[ID].texSpeed[0] );
	GPUP_DeleteTexture( &gpup_emitter[ID].texSpeed[1] );
	GPUP_DeleteTexture( &gpup_emitter[ID].image1 );
	GPUP_DeleteTexture( &gpup_emitter[ID].imagex );
	GPUP_DeleteTexture( &gpup_emitter[ID].gradient_1 );
			
	gpup_emitter[ID].effectLoaded = 0 ;
			
	return 1;
}

// Toggle for disabling the rendering of an effect without stopping the simulation
void gpup_setEffectVisible( int ID, int visible )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;

	gpup_emitter[ID].effectVisible = visible;
}

int gpup_getEffectVisible ( int ID )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return 0;
	return gpup_emitter[ID].effectVisible;
}

int gpup_getMaxEffects ( void )
{
	return gpup_maxeffects;
}

int gpup_getEffectLoaded ( int ID )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return 0;
	return gpup_emitter[ID].effectLoaded;
}
float gpup_getParticleSpeed(int ID)
{
	if (ID < 0 || ID >= gpup_maxeffects) return 0;
	return gpup_emitter[ID].emitter_animation_speed;
}
float gpup_getParticleOpacity(int ID)
{
	if (ID < 0 || ID >= gpup_maxeffects) return 0;
	return gpup_emitter[ID].opacity;
}
float gpup_getParticleSize(int ID)
{
	if (ID < 0 || ID >= gpup_maxeffects) return 0;
	return gpup_emitter[ID].particleSize;
}
float gpup_getBounciness(int ID)
{
	if (ID < 0 || ID >= gpup_maxeffects) return 0;
	return gpup_emitter[ID].bounciness;
}
float gpup_getFloorReflectionHeight(int ID)
{
	if (ID < 0 || ID >= gpup_maxeffects) return 0;
	return gpup_emitter[ID].floorHeight;
}
void gpup_getEffectColor(int ID, float* r, float* g, float* b)
{
	if (ID < 0 || ID >= gpup_maxeffects) return;
	*r = gpup_emitter[ID].r / 0.00390625f;
	*g = gpup_emitter[ID].g / 0.00390625f;
	*b = gpup_emitter[ID].b / 0.00390625f;
}
float gpup_getEffectLifespan(int ID)
{
	if (ID < 0 || ID >= gpup_maxeffects) return 0;
	return (gpup_emitter[ID].lifespan * 60 * 10);
}

void gpup_setEffectLifespan(int ID, float value)
{
	if (ID < 0 || ID >= gpup_maxeffects) return;
	
	gpup_emitter[ID].lifespan = value / (60 * 10);
}

void gpup_setEffectAnimationSpeed(int ID,float speed)
{
	if (ID < 0 || ID >= gpup_maxeffects) return;

	gpup_emitter[ID].emitter_animation_speed = speed;
}

// Toggle bilinear filtering on the particle sprites
void gpup_setBilinear( int ID, int active )
{
	if ( ID < 0 || ID >= gpup_maxeffects ) return;
}

float g_fSlowParticleTime = 1.0f;

// Update the Particles
void gpup_update( float frameTime, wiGraphics::CommandList cmd )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	GraphicsDevice* device = wiRenderer::GetDevice();
	device->EventBegin( "GPUParticles Update", cmd );
	
	gpup_settings.tmr = frameTime * g_fSlowParticleTime;
	
	gpup_settings.sn = gpup_settings.sn + gpup_settings.tmr*10.0f;
	gpup_settings.rotsn = gpup_settings.rotsn + gpup_settings.tmr;
	
	if ( gpup_settings.sn > 999999999.0f ) gpup_settings.sn = 0.0f;
	if ( gpup_settings.rotsn > 999999999.0f ) gpup_settings.rotsn = 0.0f;
	
	gpup_settings.gtimer = gpup_settings.gtimer + gpup_settings.tmr;


	if ( gpup_settings.gtimer > 0.015f )
	{
		gpup_settings.time = gpup_settings.gtimer / 0.015f;

		// will split particle updates across two frames if more than 2 emitters are loaded and fps is high enough
		if ( gpup_settings.tmr > 0.007143f || gpup_settings.emitterCount < 2 )
		{
			gpup_settings.activeEffects1 = 0;
			gpup_settings.activeEffects2 = 0;
			for( int i = 0; i < gpup_maxeffects; i++ )
			{
				if ( gpup_emitter[i].effectLoaded == 1 ) gpup_doit( i, cmd );
			}
			gpup_settings.gtimer = gpup_settings.gtimer - 0.015f*gpup_settings.time;
		}
		else
		{
			gpup_settings.split = 1 - gpup_settings.split;
			if ( gpup_settings.split == 0 )
			{
				gpup_settings.activeEffects1 = 0;
				int count = 0;
				for( int i = 0; i < gpup_maxeffects; i++ )
				{
					if ( gpup_emitter[i].effectLoaded == 1 ) 
					{
						gpup_doit( i, cmd );
						count++;
						if ( count >= floor(gpup_settings.emitterCount/2) ) 
						{
							gpup_settings.lastEmitter = i+1;
						}
					}
				}
			}
			else
			{
				gpup_settings.activeEffects2 = 0;
				int count = 0;
				for( int i = gpup_settings.lastEmitter; i < gpup_maxeffects; i++ )
				{
					if ( gpup_emitter[i].effectLoaded == 1 ) gpup_doit( i, cmd );
				}
			}
			gpup_settings.gtimer = gpup_settings.gtimer - 0.0075f*gpup_settings.time;
		}
		gpup_settings.activeEffects = gpup_settings.activeEffects1 + gpup_settings.activeEffects2;
	}

	device->EventEnd( cmd );
}

} // namespace GPUParticles