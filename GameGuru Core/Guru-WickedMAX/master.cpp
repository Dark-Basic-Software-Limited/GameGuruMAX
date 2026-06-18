//
// Wicked MAX Master Code
//

// Includes
#include "stdafx.h"
#include "io.h"
#include "master.h"
#include "wickedcalls.h"
#include "globstruct.h"
#include ".\..\Dark Basic Public Shared\Dark basic Pro SDK\Shared\Core\DBDLLCore.h"
#include "CCameraC.h"
#include "CLightC.h"
#include "CInputC.h"
#include "CFileC.h"
#include "CSoundC.h"
#include "SimonReloaded.h"
#include "GameGuruMain.h"
#include <direct.h>
#include <shellapi.h>
#include "GGVR.h"
#include "gameguru.h"
#include <ctime>
#include <iostream>

// IMGUI Setup (first graphic to try and render)
#include "..\GameGuru\Imgui\imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "..\GameGuru\Imgui\imgui_internal.h"
#include "..\GameGuru\Imgui\imgui_impl_win32.h"
#include "..\GameGuru\Imgui\imgui_gg_dx11.h"

#include "wiECS.h"
#include "openxr.h"
#include "GPUParticles.h"
#include "GGTerrain/GGTerrain.h"
#include "GGTerrain/GGTrees.h"
#include "GGTerrain/GGGrass.h"
#include "tracers/TracerManager.h"
using namespace Tracers;

// For Steam authentication check
#include "steam/steam_api.h"

#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

#ifdef GGMAXEDU
#ifdef GGMAXEPIC
#include "eos_sdk.h"
#include "eos_auth.h"
#include "platform.h"
#include "eos_ecom.h"
cStr g_pEPICLoginStatus = "";
EOS_EpicAccountIdDetails* g_pEPICLocalUserID;
bool g_bOwnershipChecked = false;
bool g_bMAXIsOwned = false;
#endif
#endif

using namespace GPUParticles;
using namespace GGTerrain;
using namespace GGTrees;
using namespace GGGrass;

// Defines and pragmas
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4099)
#pragma warning(disable : 4996)

#define MATCHCLASSICROTATION

// Namespaces
using namespace std;
using namespace wiGraphics;
using namespace wiScene;
using namespace wiFont;
using namespace wiECS;

// Globals
GlobStruct							g_Glob;
GlobStruct*							g_pGlob = &g_Glob;

// Key Externs
extern HINSTANCE					hInst;
extern HWND							hMainWnd;

// For IMGUI
LPGGDEVICE							m_pD3D = NULL;
LPGGIMMEDIATECONTEXT				m_pImmediateContext = NULL;

// From EXEBLOCK
LPGG								m_pDX = NULL;
LPGGDEPTHSTENCILSTATE				m_pDepthStencilState = NULL;
LPGGDEPTHSTENCILSTATE				m_pDepthNoWriteStencilState = NULL;
LPGGDEPTHSTENCILSTATE				m_pDepthDisabledStencilState = NULL;
LPGGRASTERIZERSTATE					m_pRasterState = NULL;
LPGGRASTERIZERSTATE					m_pRasterStateShadow = NULL;
LPGGRASTERIZERSTATE					m_pRasterStateNoCullShadow = NULL;
LPGGRASTERIZERSTATE					m_pRasterStateTerrainShadow = NULL;
LPGGRASTERIZERSTATE					m_pRasterStateNoCull = NULL;
LPGGRASTERIZERSTATE					m_pRasterStateDepthBias = NULL;
LPGGBLENDSTATE						m_pBlendStateAlpha = NULL;
LPGGBLENDSTATE						m_pBlendStateNoAlpha = NULL;
LPGGBLENDSTATE						m_pBlendStateShadowBlend = NULL;
HWND								GlobalHwndCopy = NULL;
char								m_pWorkString[_MAX_PATH];
char								gUnpackDirectory[_MAX_PATH];
char								gpDBPDataName[_MAX_PATH];
DWORD								gEncryptionKey;
time_t								TodaysDay = 0;
extern struct _finddata_t			filedata;

bool								m_bUsingVR = false;
bool								m_bForceRender = false;

bool								bAreWeAEditor = false;
Entity g_entityCameraLight = 0;
Entity g_entityThumbLight = 0, g_entityThumbLight2 = 0;
wiECS::Entity g_weatherEntityID = 0;
Entity g_entitySunLight;
MasterRenderer * master_renderer;
wiSprite* pboundbox[4];
int g_envProbeResolution = 128;
int g_iShowSplashForFirstFewCycles = 10;

extern ImVec2 renderTargetAreaPos;
extern ImVec2 renderTargetAreaSize;
extern bool bImGuiInTestGame;
extern int g_iActivelyUsingVRNow;
extern bool bActivateStandaloneOutline;

enum EDITORSTENCILREF
{
	EDITORSTENCILREF_CLEAR = 0x00,
	EDITORSTENCILREF_HIGHLIGHT_OBJECT = 0x01,
	EDITORSTENCILREF_HIGHLIGHT_MATERIAL = 0x02,
	EDITORSTENCILREF_HIGHLIGHT_OBJECT_RED = 0x03,
	EDITORSTENCILREF_HIGHLIGHT_OBJECT_BLUE = 0x04,
	EDITORSTENCILREF_LAST = 0x0F,
};

wiGraphics::Texture rt_Outline; //PE: For object outline.
wiGraphics::Texture rt_Outline_Red; //PE: For object red outline.
wiGraphics::Texture rt_Outline_Blue; //PE: For object blue outline.
wiGraphics::Texture rt_MSAAOutline; //PE: For object outline.
wiGraphics::Texture rt_MSAAOutline_Red; //PE: For object red outline.
wiGraphics::Texture rt_MSAAOutline_Blue; //PE: For object blue outline.
wiGraphics::RenderPass renderpass_Outline;
wiGraphics::RenderPass renderpass_Outline_Red;
wiGraphics::RenderPass renderpass_Outline_Blue;

const XMFLOAT4 selectionColor = XMFLOAT4(0.9f, 0.7f, 0.2f, 1.0f); //PE: Outline color. no transparent.
const XMFLOAT4 selectionColorRed = XMFLOAT4(0.7f, 0.0f, 0.0f, 1.0f); //PE: Outline color red. no transparent.
const XMFLOAT4 selectionColorBlue = XMFLOAT4(0.3f, 0.6f, 0.8f, 1.0f); //PE: Outline color light blue. no transparent.

// can use PIX markers here
#include "../../../WICKEDREPO/packages/WinPixEventRuntime.1.0.240308001/Include/WinPixEventRuntime/pix3.h"
// g_enablePixMarkers not duplicated in main EXE

//
// Master Functions
//

struct CubeVertex
{
    float x, y, z;
	float nx, ny, nz;
	float u, v;
};

CubeVertex g_VerticesCube[ 24 ] = 
{
	// Z- face
    { -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f },
    {  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f },
	{ -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f },
	{  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f },

	// Z+ face
	{  1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
	{ -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
	{  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	{ -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f },

	// X- face
	{ -1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
	{ -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
	{ -1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
	{ -1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f },

	// X+ face
	{  1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
    {  1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
    {  1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    {  1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f },

	// Y- face
	{ -1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f },
	{  1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f },
	{ -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f },
	{  1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f },

	// Y+ face
	{ -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
	{  1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
	{ -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
	{  1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
};

uint16_t g_IndicesCube[ 36 ] =
{
	 0,  1,  2,  2,  1,  3, // Z- face
	 4,  5,  6,  6,  5,  7, // Z+ face
	 8,  9, 10, 10,  9, 11, // X- face
	12, 13, 14, 14, 13, 15, // X+ face
	16, 17, 18, 18, 17, 19, // Y- face
	20, 21, 22, 22, 21, 23, // Y+ face
};

Entity CreateCubeMesh( std::string name, Scene& scene )
{
	Entity materialEntity = scene.Entity_CreateMaterial( name + "_material" );
	MaterialComponent& material = *scene.materials.GetComponent(materialEntity);

	material.baseColor = XMFLOAT4(1,1,1, 1);
	material.metalness = 0;
	material.roughness = 1;

	std::string folder = "";

	//load textures
	if (!material.textures[MaterialComponent::SURFACEMAP].name.empty())
	{
		material.textures[MaterialComponent::SURFACEMAP].resource = wiResourceManager::Load(folder + material.textures[MaterialComponent::SURFACEMAP].name);
	}
	if (!material.textures[MaterialComponent::BASECOLORMAP].name.empty())
	{
		material.textures[MaterialComponent::BASECOLORMAP].resource = wiResourceManager::Load(folder + material.textures[MaterialComponent::BASECOLORMAP].name);
	}
	if (!material.textures[MaterialComponent::NORMALMAP].name.empty())
	{
		material.textures[MaterialComponent::NORMALMAP].resource = wiResourceManager::Load(folder + material.textures[MaterialComponent::NORMALMAP].name);
	}
	if (!material.textures[MaterialComponent::DISPLACEMENTMAP].name.empty())
	{
		material.textures[MaterialComponent::DISPLACEMENTMAP].resource = wiResourceManager::Load(folder + material.textures[MaterialComponent::DISPLACEMENTMAP].name);
	}


	Entity objectEntity = scene.Entity_CreateObject( name );
	Entity meshEntity = scene.Entity_CreateMesh( name + "_mesh" );
	ObjectComponent& object = *scene.objects.GetComponent(objectEntity);
	MeshComponent& mesh = *scene.meshes.GetComponent(meshEntity);

	object.meshID = meshEntity;

	mesh.subsets.push_back( MeshComponent::MeshSubset() );
	mesh.subsets.back().materialID = materialEntity;
	mesh.subsets.back().indexOffset = 0;

	for( int i = 0; i < 24; i++ )
	{
		CubeVertex& vert = g_VerticesCube[i];
		mesh.vertex_positions.push_back( XMFLOAT3(vert.x, vert.y, vert.z) );
		mesh.vertex_normals.push_back( XMFLOAT3(vert.nx, vert.ny, vert.nz) );
		mesh.vertex_uvset_0.push_back( XMFLOAT2(vert.u, vert.v) );
	}

	for ( int i = 0; i < 36; i++ )
	{
		mesh.indices.push_back( g_IndicesCube[i] );
		mesh.subsets.back().indexCount++;
	}

	mesh.CreateRenderData();

	scene.Update(0);

	return objectEntity;
}

void CreateVRControllerObjects( void )
{
	Scene localScene;

	Entity leftEntity = CreateCubeMesh( "VR_Controller_Left", localScene );
	TransformComponent* pLeftHandTransform = localScene.transforms.GetComponent ( leftEntity );
	pLeftHandTransform->Translate ( XMFLOAT3 ( -0.25f, -0.2f, 0.5f ) );
	pLeftHandTransform->Scale( XMFLOAT3(10,10,10) );
	ObjectComponent* leftObject = localScene.objects.GetComponent( leftEntity );
	leftObject->SetRenderable( false ); // hide by default

	Entity rightEntity = CreateCubeMesh( "VR_Controller_Right", localScene );
	TransformComponent* pRightHandTransform = localScene.transforms.GetComponent ( rightEntity );
	pRightHandTransform->Translate ( XMFLOAT3 ( 0.25f, -0.2f, 0.5f ) );
	pRightHandTransform->Scale( XMFLOAT3(10,10,10) );
	ObjectComponent* rightObject = localScene.objects.GetComponent( rightEntity );
	rightObject->SetRenderable( false ); // hide by default
	
	wiScene::GetScene().Merge( localScene );
}

__declspec(noinline) void ForceCrash_AccessViolation()
{
	volatile int* p = nullptr;
	*p = 123; // writes to null -> 0xC0000005
}

void Master::Initialize()
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	__super::Initialize();
}

void Master::InitializeSecondaries()
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	initializedSecondaries = true;
	infoDisplay.active = false;
	infoDisplay.watermark = false;// true;
	infoDisplay.fpsinfo = false;// true;
	infoDisplay.resolution = false;// true;
	master_renderer = &masterrenderer;
	masterrenderer.Load();
	masterrenderer.setMSAASampleCount(1);
	ActivatePath(&masterrenderer);
	masterrenderer.Set3DResolution( masterrenderer.GetPhysicalWidth(), masterrenderer.GetPhysicalHeight() );

	// smooth out experience, avoid jerkiness
	wiEvent::SetVSync(bVsyncEnabled);
	masterrenderer.setAO(RenderPath3D::AO_MSAO);// AO_DISABLED );

	// occlusion query box has been expanded in an attempt to avoid flickering
	wiRenderer::SetOcclusionCullingEnabled(false); // the lag is clearly visible, causing obvious gaps for some level designs (performance hit now disabled)!

	// populate D3D ptrs (for IMGUI)
	m_pD3D = (ID3D11Device*)wiRenderer::GetDevice()->GetDeviceForIMGUI();
	m_pImmediateContext = (ID3D11DeviceContext*)wiRenderer::GetDevice()->GetImmediateForIMGUI();

	// Get current working directory (for temp folder compare)
	char CurrentDirectory[_MAX_PATH];
	_getcwd(CurrentDirectory, _MAX_PATH);

	// Find temporary directory (C:\WINDOWS\Temp)
	char WindowsTempDirectory[_MAX_PATH];
	GetTempPathA(_MAX_PATH, WindowsTempDirectory);
	if (stricmp(WindowsTempDirectory, CurrentDirectory) != NULL)
	{
		// XP Temp Folder
		_chdir(WindowsTempDirectory);
		MakeOrEnterUniqueDBPDATA();
		strcat(WindowsTempDirectory, "\\");
		strcat(WindowsTempDirectory, gpDBPDataName);
		strcpy(gUnpackDirectory, WindowsTempDirectory);
		_chdir(gpDBPDataName);
		DeleteContentsOfDBPDATA(false);
		_chdir(CurrentDirectory);
	}

	//Setup default write folder and load preferences.
	extern char defaultWriteFolder[260];
	extern preferences pref;
	FindAWriteablefolder();

	bool bResetPreferences = true;
	char prefile[MAX_PATH];
	strcpy(prefile, defaultWriteFolder);
	strcat(prefile, "gamegurumax.pref");

	//Load setup.
	FILE* preffile = fopen(prefile, "rb");
	if (preffile)
	{
		size_t size = fread(&pref, 1, sizeof(pref), preffile);
		//Valid pref:
		if (pref.szCheckFile[0] == 'G' && pref.szCheckFile[9] == 'P')
		{
			bResetPreferences = false;
		}
		pref.launched++;
		fclose(preffile);
	}

	if (bResetPreferences)
	{
		//Init prefereences defaults.
		strcpy(pref.szCheckFile, "GAMEGURU-PREFS");
		pref.launched = 0;
		pref.current_style = 25; // now "blue" was 0 , Max default to style "Dark Style"
		pref.vStartResolution = { 1280,800 };
		for (int i = 0; i < 10; i++)
		{
			strcpy(pref.last_open_files[i], "");
			strcpy(pref.last_project_files[i], "");
		}

		for (int i = 0; i < 15; i++)
			strcpy(pref.search_history[i], "");

		for (int i = 0; i < 15; i++)
			strcpy(pref.small_search_history[i], "");

		for (int i = 0; i < 10; i++)
			strcpy(pref.last_import_files[i], "");

		for (int il = 0; il < 16; il++)
		{
			pref.vSaved_Light_Palette_R[il] = 1.0f;
			pref.vSaved_Light_Palette_G[il] = 1.0f;
			pref.vSaved_Light_Palette_B[il] = 1.0f;
			pref.iSaved_Light_Type[il] = -1;
			pref.iSaved_Light_Range[il] = 0;
			pref.fSaved_Light_ProbeScale[il] = 1.0f;
			pref.iSaved_Light_Radius[il] = 45;
			pref.fSaved_Light_AngX[il] = 90;
			pref.fSaved_Light_AngY[il] = 0;
			pref.fSaved_Light_AngZ[il] = 0;
		}

		for (int il = 0; il < 16; il++)
		{
			strcpy(pref.Saved_Particle_Name[il], "");
			pref.Saved_bParticle_Preview[il] = true;
			pref.Saved_bParticle_Show_At_Start[il] = true;
			pref.Saved_bParticle_Looping_Animation[il] = true;
			pref.Saved_bParticle_Full_Screen[il] = false;
			pref.Saved_fParticle_Fullscreen_Duration[il] = 10.0f;
			pref.Saved_fParticle_Fullscreen_Fadein[il] = 1.0f;
			pref.Saved_fParticle_Fullscreen_Fadeout[il] = 1.0f;
			strcpy(pref.Saved_Particle_Fullscreen_Transition[il], "");
			pref.Saved_fParticle_Speed[il] = 1.0f;
			pref.Saved_fParticle_Opacity[il] = 1.0f;
		}

		pref.save_layout = true;
		pref.iMaximized = 1;

		//PE: Make sure autoflatten is set if not preference found (standalone), autoflatten are now in entityelements.
		pref.iEnableAutoFlattenSystem = true;
	}

	if (pref.current_version != MAXVERSION)
	{
		// Only reset layout.
		pref.vStartResolution = { 1280,800 };
		pref.iMaximized = 1;

		// also reset values that may have already been set in previous prefs (but NEED to be set for newer functionality)
		coreResetIMGUIFunctionalityPrefs();
		if(MAXVERSION == 26) pref.iDisableObjectLibraryViewport = 1;
	}

	if (pref.iResetAutoRunVideosOnNextStartup == 1)
	{
		for (int i = 0; i < 20; i++)
			pref.iPlayedVideoSection[i] = 0;
		pref.iResetAutoRunVideosOnNextStartup = 0;
	}
	if (pref.iAdvancedGridModeSettings == 0)
	{
		//PE: Set new fEditorGridSize settings. 
		if (pref.fEditorGridSizeX <= 0.1f) pref.fEditorGridSizeX = 0.1f;
		pref.fEditorGridOffsetX = 0;
		pref.fEditorGridOffsetY = 0;
		pref.fEditorGridOffsetZ = 0;
		pref.fEditorGridSizeY = pref.fEditorGridSizeX;
		pref.fEditorGridSizeZ = pref.fEditorGridSizeX;
	}

	g_bEnableAutoFlattenSystem = pref.iEnableAutoFlattenSystem;

	// Force some prefs as ALWAYS SET 
	// we want the popup drop down back, it was coooool ;)
	pref.iEnableRelationPopupWindow = 1;

	// Initialises g_pGlob data used throughout engine
	int iDisplayWidth = masterrenderer.GetPhysicalWidth();
	int iDisplayHeight = masterrenderer.GetPhysicalHeight();
	InitDisplayAndGlob(hInst, hMainWnd, gUnpackDirectory, iDisplayWidth, iDisplayHeight);

	// Some constructor calls still required
	CameraConstructor();
	LightConstructor();
	SoundConstructor();
	InputConstructor(false);
	InfiniteVegetationConstructor();
	void AnimationConstructor(void);
	AnimationConstructor();

	CreateVRControllerObjects();

	// setup OpenXR, will fail if VR not available
	// OpenXRInit now as vrmodefordevelopers=1 can activate VR for experiments!
	m_bUsingVR = false;
	if ( OpenXRInit( (ID3D11Device*) wiRenderer::GetDevice()->GetDeviceForIMGUI() ) >= 0 )
	{
		// VR available
		m_bUsingVR = true;
	}

	// Before launch MAX, run the Classic to MAX converter to see if any assets require porting from X files to DBO
	// continues in background, does not wait until finished before launching MAX (small chance of trying to use an old X file first time!)
	// better down the load if this popped up as an IMGUI prompt showing this process happening on startup
	// with an option in the dialog and in the editor settings to switch off this activity by default
}

void camerahook_domydemostuff(float fX, float fY, float fZ, float fAX, float fAY, float fAZ)
{
	// handle camera positioning in wicked
	double dMyDegToRad = 3.141592654f/180.0f;
	fAX *= dMyDegToRad;
	fAY *= dMyDegToRad;
	fAZ *= dMyDegToRad;
	wiScene::TransformComponent camera_transform;
	camera_transform.ClearTransform();
	camera_transform.Translate(XMFLOAT3(fX, fY, fZ));
	DirectX::XMFLOAT3 rot;
	rot.x = fAX;
	rot.y = fAY;
	rot.z = fAZ;

	// when in VR mode the camera must be prevented from rotating on X and Z to avoid disorientating the player
	if (m_bUsingVR == true && g_iActivelyUsingVRNow == 1)
	{
		rot.x = 0;
		rot.z = 0;
	}

	// this will match how classic rotations work
	#ifndef MATCHCLASSICROTATION
	camera_transform.RotateRollPitchYaw(rot);
	#else
	XMMATRIX rotm;
	rotm = XMMatrixRotationX(rot.x);
	rotm = rotm * XMMatrixRotationY(rot.y);
	rotm = rotm * XMMatrixRotationZ(rot.z);
	XMVECTOR S,R,T;
	XMMatrixDecompose(&S, &R, &T, rotm);
	camera_transform.Rotate(R);
	#endif
	camera_transform.UpdateTransform();
	wiScene::GetCamera().TransformCamera(camera_transform);

	// thix sets the player camera position and rotation (later built upon by the RunCustom() function if VR used to position hands and head)
	wiScene::GetCamera().UpdateCamera();
	
	// handle camera light used by character creator
	if (g_entityCameraLight) {
		TransformComponent* transformLightCamera = wiScene::GetScene().transforms.GetComponent(g_entityCameraLight);
		transformLightCamera->ClearTransform();
		transformLightCamera->Translate(XMFLOAT3( CameraPositionX(0), CameraPositionY(0) + 20.0f, CameraPositionZ(0)));
		transformLightCamera->SetDirty();
	}
}

void camerahook_domydemostuff2(float* fX, float* fY, float* fWidth, float* fHeight)
{
	// update 3D viewport so 3Dto2D mouse coords can be calculated
	*fX = renderTargetAreaPos.x;
	*fY = renderTargetAreaPos.y;
	*fWidth = renderTargetAreaSize.x;
	*fHeight = renderTargetAreaSize.y;
}

#ifdef GGMAXEPIC
void LoginCompleteCallbackFn(const EOS_Auth_LoginCallbackInfo* Data)
{
	assert(Data != NULL);
	EOS_HAuth AuthHandle = EOS_Platform_GetAuthInterface(FPlatform::GetPlatformHandle());
	assert(AuthHandle != nullptr);
	if (Data->ResultCode == EOS_EResult::EOS_Success)
	{
		const int32_t AccountsCount = EOS_Auth_GetLoggedInAccountsCount(AuthHandle);
		for (int32_t AccountIdx = 0; AccountIdx < AccountsCount; ++AccountIdx)
		{
			EOS_ELoginStatus LoginStatus;
			LoginStatus = EOS_Auth_GetLoginStatus(AuthHandle, Data->LocalUserId);
			if (LoginStatus == EOS_ELoginStatus::EOS_LS_LoggedIn)
			{
				g_pEPICLoginStatus = "Logged In";
				g_pEPICLocalUserID = Data->LocalUserId;
			}
		}
	}
	else
	{
		g_pEPICLoginStatus = "Login Failed";
	}
}
void EOS_Ecom_OnQueryOwnershipCallbackFn (const EOS_Ecom_QueryOwnershipCallbackInfo* Data)
{
	assert(Data != NULL);
	if ( Data->ItemOwnershipCount > 0 )
	{
		for (int n = 0; n < Data->ItemOwnershipCount; n++)
		{
			if (Data->ItemOwnership[n].OwnershipStatus == EOS_EOwnershipStatus::EOS_OS_Owned)
			{
				g_bMAXIsOwned = true;
			}
		}
	}
	g_bOwnershipChecked = true;
}
#endif

void Master::Update(float dt)
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	// leave right away if not ready for update
	if ( !initializedSecondaries ) return;

	// super update to keep things ticking along during setup and regular loop
	__super::Update(dt);

	// push splash render to end of function FLICKER - MAKE THIS WORK!!!!
	static bool bCustomSplash = false;
	wiImageParams fx;
	fx.enableFullScreen();
	fx.blendFlag = BLENDMODE_OPAQUE;

	// while init phase, show splash
	if (GuruUpdate() == false)
	{
		// to avoid screen goping white and black while splashscreening at launch
		g_iShowSplashForFirstFewCycles = 10;

		// output graphcs card log created earlier
		static bool bOnlyReportOnce = true;
		if (bOnlyReportOnce == true)
		{
			//extern char g_pGraphicsCardLog[10240];
			//timestampactivity(0, g_pGraphicsCardLog);
			GraphicsDevice* device = wiRenderer::GetDevice();
			if (device->GetGraphicsCardName())
			{
				char pActualGraphicsCardUsed[MAX_PATH];
				sprintf(pActualGraphicsCardUsed, "Graphics Card Used: %s", device->GetGraphicsCardName());
				timestampactivity(0, pActualGraphicsCardUsed);
			}
			bOnlyReportOnce = false;
		}

		// until we are loaded and ready, present splash screen
		timestampactivity(0, "initial setup");

		// load and show MAX splash
		timestampactivity(0, "splash screen");
		if (b_gSplashTextureLoaded == false)
		{
			// determine if in editor or game/standalone
			bAreWeAEditor = true;
			std::string appname = Appname();
			const char* pestrcasestr(const char* arg1, const char* arg2);
			if (!(pestrcasestr(appname.c_str(), "guru-mapeditor.exe") || pestrcasestr(appname.c_str(), "vr quest app.exe") || pestrcasestr(appname.c_str(), "gamegurumax.exe")))
				bAreWeAEditor = false;

			// do auth check VERY early
			extern bool g_bParticleEditorPresent;
			extern bool g_bBuildingEditorPresent;
			extern bool g_bUpdateAppAvailable;
			extern bool g_bFreeTrialVersion;
			g_bParticleEditorPresent = false;
			g_bBuildingEditorPresent = false;
			g_bUpdateAppAvailable = false;
			g_bFreeTrialVersion = false;
			if (bAreWeAEditor == true)
			{
				// check if we are non-Steam and have the auto-updater
				timestampactivity(0, "authentication check");
				cstr pOldDir = GetDir();
				timestampactivity(0, pOldDir.Get());

				// check if have the Particle Editor installed
				timestampactivity(0, "particle editor exist check");
				if (FileExist("Tools\\Particle Editor\\particle_editor.exe") == 1)
				{
					timestampactivity(0, "particle editor exists!");
					g_bParticleEditorPresent = true;
				}

				// check if have the Building Editor installed
				timestampactivity(0, "building editor exist check");
				if (FileExist("Tools\\Building Editor\\GameGuru Building Editor.exe") == 1)
				{
					// confirm and flag
					timestampactivity(0, "building editor exists!");
					g_bBuildingEditorPresent = true;

					// also create writeable folder to place exports (so object library can be pick them up right away on first use)
					char pRealWritableArea[MAX_PATH];
					strcpy(pRealWritableArea, pOldDir.Get());
					strcat(pRealWritableArea, "\\Files\\entitybank\\User\\");
					GG_GetRealPath(pRealWritableArea, 1);
					SetDir(pRealWritableArea);
					MakeDirectory("buildingeditor");
					SetDir(pOldDir.Get());
				}

				// are we non-steam
				SetDir("..");
				cstr pCurrentDirChecking = GetDir();

				// restore current in Max folder
				SetDir(pOldDir.Get());

				//force non-Steam version (some installations are NOT detecting 'GameGuru MAX Updater.exe')
				//#define FORCENONSTEAMAUTHENTICATION
#ifdef FORCENONSTEAMAUTHENTICATION
				timestampactivity(0, "forcing non-Steam authentication");
				timestampactivity(0, pCurrentDirChecking.Get());
				g_bUpdateAppAvailable = true;
#endif

				// test for Steam functionality 
				///#define TESTSTEAMFREETRIAL

				// if steam, do auth check
				if (g_bUpdateAppAvailable == false)
				{
#ifndef GGMAXEDU
					// Steam Purchased (Owned)
					char pInstallSteamTrialFile[MAX_PATH];
					strcpy(pInstallSteamTrialFile, pOldDir.Get());
					strcat(pInstallSteamTrialFile, "\\installsteamtrial.dat");
					char pInstallSteamFile[MAX_PATH];
					strcpy(pInstallSteamFile, pOldDir.Get());
					strcat(pInstallSteamFile, "\\installsteam.dat");
					GG_GetRealPath(pInstallSteamFile, 0);
					timestampactivity(0, "GameGuru Steam version check");
					bool bGameGuruMAXOwned = false;
					int iSteamErrorCode = 0;
					bool bLoggedIntoSteamForWorkshop = false;
					if (SteamAPI_Init())
					{
						// must be logged into steam, which will be the case if run from within steam. 
						if (SteamUser()->BLoggedOn())
						{
							SteamInitClient();

#ifdef TESTSTEAMFREETRIAL
							if (SteamApps()->BIsSubscribedApp(1247289) == true) // dummy app, forces the free trial mode
#else
							if (SteamApps()->BIsSubscribedApp(1247290) == true) //1247290) == true) // APPID = 1247290 = GameGuru MAX
#endif
							{
								// yes, user owns Steam version :)
								timestampactivity(0, "GameGuru MAX Steam version owned!");
								bGameGuruMAXOwned = true;
							}
							else
							{
								// this is a free trial version
								timestampactivity(0, "GameGuru MAX Steam version not owned - switching to free trial mode");
								g_bFreeTrialVersion = true;
								extern bool bFreeTrial_Window;
								bFreeTrial_Window = true;

								// install file is different for free trial
								strcpy(pInstallSteamFile, pInstallSteamTrialFile);

								// also stop intro video as gets in way of upgrade scree
								extern bool bEnsureIntroVideoIsNotRun;
								bEnsureIntroVideoIsNotRun = true;

								// construct a list of all objects the free trial makes available (all rest are greyed out)
								cstr pCurrentDir = GetDir();
								char pAbsolutePathAndFile[MAX_PATH];
								extern std::vector<std::string> files_availableinfreetrial;
								files_availableinfreetrial.clear();
								for (int demos = 0; demos < 4; demos++)
								{
									std::vector<std::string> files_fromLST;
									files_fromLST.clear();
									LPSTR pLSTFile = "";
									if (demos == 0) pLSTFile = "Files\\mapbank\\canyon offensive.lst";
									if (demos == 1) pLSTFile = "Files\\mapbank\\island showdown.lst";
									if (demos == 2) pLSTFile = "Files\\mapbank\\zombie cellar demo - level1.lst";
									if (demos == 3) pLSTFile = "Files\\mapbank\\RPG Template.lst";
									getVectorFileContent(pLSTFile, files_fromLST);
									for (int fileindex = 0; fileindex < files_fromLST.size(); fileindex++)
									{
										LPSTR pThisFile = (LPSTR)files_fromLST[fileindex].c_str();
										if (strlen(pThisFile) > 4)
										{
											LPSTR pExt = pThisFile + strlen(pThisFile) - 4;
											if (stricmp (pExt, ".fpe") == NULL)
											{
												if (strnicmp (pThisFile, "entitybank\\_markers\\", 20) != NULL)
												{
													strcpy(pAbsolutePathAndFile, pCurrentDir.Get());
													strcat(pAbsolutePathAndFile, "\\Files\\");
													strcat(pAbsolutePathAndFile, pThisFile);
													files_availableinfreetrial.push_back(pAbsolutePathAndFile);
												}
											}
										}
									}
								}
								saveVectorFileContent("freetriallist.ini", files_availableinfreetrial);
							}

							// update install steam date stamp (so can run afterwards offline for 30 days - or 7 days if free trial)
							// do not create the "pInstallSteamFile" file when running free trial
							if (g_bFreeTrialVersion == false || (g_bFreeTrialVersion == true && FileExist(pInstallSteamFile) == 0))
							{
								if (FileExist(pInstallSteamFile) == 1) DeleteFileA(pInstallSteamFile);
								OpenToWrite(1, pInstallSteamFile);
								std::time_t t = std::time(0);
								std::tm* now = std::localtime(&t);
								int iDateAsValueNow = (now->tm_year * 380) + (now->tm_mon * 31) + now->tm_mday;
								char pDateString[MAX_PATH];
								sprintf(pDateString, "%d", iDateAsValueNow);
								WriteString(1, pDateString);
								char pDateChecksum[MAX_PATH];
								int iDayTrick1 = (now->tm_mday - now->tm_mon);
								int iDayTrick2 = (iDayTrick1 * iDayTrick1) % 31;
								int iChecksumValue = (now->tm_year * 1165) + (now->tm_mon * 412) + (iDayTrick2 * 9);
								sprintf(pDateChecksum, "%d", iChecksumValue);
								WriteString(1, pDateChecksum);
								CloseFile(1);
							}

							// when logged in can use workshop
							bLoggedIntoSteamForWorkshop = true;
						}
						else
						{
							iSteamErrorCode = 2;
						}

						// a marker to know if this user used the demo first :)
						if (g_bFreeTrialVersion == true)
						{
							char pTestForDemo[MAX_PATH];
							strcpy(pTestForDemo, pOldDir.Get());
							strcat(pTestForDemo, "\\demowasnice.dat");
							GG_GetRealPath(pTestForDemo, 0);
							if (FileExist(pTestForDemo) == 1) DeleteFileA(pTestForDemo);
							OpenToWrite(1, pTestForDemo);
							char pTextTest[MAX_PATH];
							sprintf(pTextTest, "demowasnice");
							WriteString(1, pTextTest);
							extern std::vector<std::string> early_debug_list;
							if (early_debug_list.size() > 0)
							{
								for (int l = 0; l < early_debug_list.size(); l++)
								{
									WriteString(1, (LPSTR)early_debug_list[l].c_str());
								}
							}
							CloseFile(1);
						}
					}
					else
					{
						iSteamErrorCode = 1;
					}

					// always init workshop for testing purposes if not logged in
					extern void workshop_init(bool);
					workshop_init(bLoggedIntoSteamForWorkshop);

					extern int g_iFreeTrialDaysLeft;
					g_iFreeTrialDaysLeft = 0;
					bool bReadSteamDateFile = false;
					if (iSteamErrorCode > 0 || g_bFreeTrialVersion == true)
					{
						// before declare auth error, check 30 day offline system, allow if have previously logged into Steam during 30 days
						// or if a free trial and need to work out days left
						bReadSteamDateFile = true;
					}
					if (bReadSteamDateFile == true)
					{
						if (FileExist(pInstallSteamFile) == 1)
						{
							OpenToRead(1, pInstallSteamFile);
							LPSTR pInstallDateOnSteam = ReadString(1);
							LPSTR pDateChecksum = ReadString(1);
							CloseFile(1);
							if (pInstallDateOnSteam)
							{
								if (strlen(pInstallDateOnSteam) > 0)
								{
									int iDateAsValueLast = atoi(pInstallDateOnSteam);
									int iWasY = iDateAsValueLast / 380;
									int iWasM = (iDateAsValueLast - (iWasY * 380)) / 31;
									int iWasD = iDateAsValueLast - (iWasY * 380) - (iWasM * 31);
									int iDayTrick1 = (iWasD - iWasM);
									int iDayTrick2 = (iDayTrick1 * iDayTrick1) % 31;
									int iChecksumValueShouldBe = (iWasY * 1165) + (iWasM * 412) + (iDayTrick2 * 9);
									int iChecksumValueIs = atoi(pDateChecksum);
									if (iChecksumValueIs == iChecksumValueShouldBe)
									{
										std::time_t t = std::time(0);
										std::tm* now = std::localtime(&t);
										int iDateAsValueNow = (now->tm_year * 380) + (now->tm_mon * 31) + now->tm_mday;
										if (g_bFreeTrialVersion == true)
										{
											// MAX free trial
											g_iFreeTrialDaysLeft = 7 - (iDateAsValueNow - iDateAsValueLast);
											if (g_iFreeTrialDaysLeft < 0) g_iFreeTrialDaysLeft = 0;
											iSteamErrorCode = 0;
										}
										else
										{
											// MAX owned
											if (iDateAsValueNow <= iDateAsValueLast + 30)
											{
												// steam was run online less that 30 days ago
												timestampactivity(0, "GameGuru MAX Steam version activated in offline mode");
												iSteamErrorCode = 0;
											}
										}
									}
									else
									{
										// checksum mismatch!
										iSteamErrorCode = 3;
									}
								}
							}
						}
						if (g_bFreeTrialVersion == true && g_iFreeTrialDaysLeft <= 0)
						{
							iSteamErrorCode = 4;
						}
					}
					if (iSteamErrorCode > 0)
					{
						LPSTR pErrorStr = "";
						switch (iSteamErrorCode)
						{
							case 1: pErrorStr = "The Steam client must be running to run GameGuru MAX, or you must have run MAX through the client in the last month"; break;
							case 2: pErrorStr = "You must be logged into your Steam account to run GameGuru MAX, or you must have run MAX through the client in the last month"; break;
							case 3: pErrorStr = "GameGuru MAX failed to run in offline mode. The Steam client must be running to run GameGuru MAX"; break;
							case 4: pErrorStr = "GAMEGURU MAX FREE TRIAL VERSION HAS EXPIRED"; break;
						}
						MessageBoxA (NULL, pErrorStr, "Steam Authentication", MB_ICONEXCLAMATION | MB_OK);
						timestampactivity(0, pErrorStr);
						PostQuitMessage(0);
					}
#else
					// can use an Educational Authenticator here (i.e. license key) if needed in the future..
#endif
				}

				// Init EOS SDK
#ifdef GGMAXEDU
#ifdef GGMAXEPIC
				timestampactivity(0, "Initialize Epic Online Services API");
				EOS_InitializeOptions SDKOptions = {};
				SDKOptions.ApiVersion = EOS_INITIALIZE_API_LATEST;
				SDKOptions.AllocateMemoryFunction = nullptr;
				SDKOptions.ReallocateMemoryFunction = nullptr;
				SDKOptions.ReleaseMemoryFunction = nullptr;
				SDKOptions.ProductName = "GameGuru MAX";
				SDKOptions.ProductVersion = "1.0";
				SDKOptions.SystemInitializeOptions = nullptr;
				SDKOptions.OverrideThreadAffinity = nullptr;
				EOS_EResult InitResult = EOS_Initialize(&SDKOptions);
				if (InitResult != EOS_EResult::EOS_Success)
				{
					timestampactivity(0, "[EOS SDK] Init Failed!");
				}
				else
				{
					timestampactivity(0, "[EOS SDK] Platform Creation...");
					EOS_HPlatform PlatformHandle;
					const bool bCreateSuccess = FPlatform::Create();
					if (!bCreateSuccess)
					{
						timestampactivity(0, "[EOS SDK] Platform Create failed!");
					}
					else
					{
						if (!FPlatform::IsInitialized())
						{
							timestampactivity(0, "[EOS SDK] Can't Log In - EOS SDK Not Initialized!");
						}
						else
						{
							extern bool bSpecialStandalone;
							extern bool bSpecialEditorFromStandalone;
							if (bSpecialStandalone == true || bSpecialEditorFromStandalone == true || g.iStandaloneIsReloading > 0)
							{
								// editor calling itself
								timestampactivity(0, "GameGuru MAX Calling Itself.");
								g_bMAXIsOwned = true;
							}
							else
							{
								timestampactivity(0, "[EOS SDK] EOS_Auth_Login...");
								EOS_HAuth AuthHandle = EOS_Platform_GetAuthInterface(FPlatform::GetPlatformHandle());
								assert(AuthHandle != nullptr);
								EOS_Auth_Credentials Credentials = {};
								Credentials.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;
								extern LPSTR gRefCommandLineString;
								char* findexchangecode = (char*)pestrcasestr(gRefCommandLineString, "-AUTH_PASSWORD=");
								if (findexchangecode)
								{
									// run via Epic Launcher, so use exchange code
									timestampactivity(0, "[EOS SDK] run via Epic Launcher, so use exchange code...");
									char pExchangeCode[MAX_PATH];
									strcpy(pExchangeCode, findexchangecode + strlen("-AUTH_PASSWORD="));
									for (int n = 0; n < strlen(pExchangeCode); n++)
									{
										if (pExchangeCode[n] == ' ')
										{
											pExchangeCode[n] = 0;
											break;
										}
									}
									Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_ExchangeCode;
									Credentials.Id = NULL;
									Credentials.Token = pExchangeCode;
									//char pExCode[MAX_PATH];
									//sprintf(pExCode, "Exchange Code: %s (%s)", gRefCommandLineString, pExchangeCode);
									//timestampactivity(0, pExCode);
								}
								else
								{
									// not run via Epic Launcher, so use account portal
									timestampactivity(0, "[EOS SDK] not run via Epic Launcher, so use account portal...");
									Credentials.Type = EOS_ELoginCredentialType::EOS_LCT_AccountPortal;
									Credentials.Id = NULL;
									Credentials.Token = NULL;
								}
								EOS_Auth_LoginOptions LoginOptions = {};
								memset(&LoginOptions, 0, sizeof(LoginOptions));
								LoginOptions.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
								LoginOptions.Credentials = &Credentials;
								LoginOptions.LoginFlags = 0;
								EOS_Auth_Login(AuthHandle, &LoginOptions, this, LoginCompleteCallbackFn);
								while (strlen(g_pEPICLoginStatus.Get()) == 0)
								{
									FPlatform::Update();
									Sleep(1);
								}
								//char pLoginResult[MAX_PATH];
								//sprintf(pLoginResult, "Login Result: %s (%d)", g_pEPICLoginStatus.Get(), (int)g_pEPICLocalUserID);
								//timestampactivity(0, pLoginResult);
								if (g_pEPICLocalUserID)
								{
									timestampactivity(0, "[EOS SDK] EOS_Platform_GetEcomInterface...");
									EOS_HEcom EcomHandle = EOS_Platform_GetEcomInterface(FPlatform::GetPlatformHandle());
									EOS_Ecom_QueryOwnershipOptions OwnershipOptions;
									memset(&OwnershipOptions, 0, sizeof(OwnershipOptions));
									OwnershipOptions.ApiVersion = EOS_ECOM_QUERYOWNERSHIP_API_LATEST;
									OwnershipOptions.LocalUserId = g_pEPICLocalUserID;
									EOS_Ecom_CatalogItemId CatalogItemArray[2];
									CatalogItemArray[0] = "defe1f286d294267ae1d2d84c7bde024"; // from Full SKU spreadsheet
									CatalogItemArray[1] = "ec6a6e2f5ac54daea88a18b9d7e58835"; // staging and live
									OwnershipOptions.CatalogItemIds = CatalogItemArray;
									OwnershipOptions.CatalogItemIdCount = 2;
									OwnershipOptions.CatalogNamespace = NULL;
									void* pMyVoidPtr = new int[10];
									EOS_Ecom_QueryOwnership(EcomHandle, &OwnershipOptions, pMyVoidPtr, EOS_Ecom_OnQueryOwnershipCallbackFn);
									while (g_bOwnershipChecked == false)
									{
										FPlatform::Update();
										Sleep(1);
									}
									if (g_bMAXIsOwned == true)
									{
										// YAY! This MAX is owned, continue as normal!
										timestampactivity(0, "GameGuru MAX Epic Version Owned!");
									}
								}
							}
						}
					}
				}
				if (g_bMAXIsOwned == false)
				{
					// still run, but in free trial mode
					timestampactivity(0, "GameGuru MAX Epic version not owned - switching to free trial mode!");
					g_bFreeTrialVersion = true;
				}
#endif
#endif
			}

			// and then get the correct splash
			bool bDecryptedFile = false;
			char VirtualFilename[_MAX_PATH];
			strcpy(VirtualFilename, "");
			char fileName[MAX_PATH];
			extern bool bSpecialEditorFromStandalone;
			LPSTR pFolderToUse = "";
			if (g_bFreeTrialVersion == true)
				pFolderToUse = "freetrial";
			else
				pFolderToUse = "uiv3";
			if (bSpecialEditorFromStandalone == true)
			{
				sprintf(fileName, "Files\\editors\\%s\\loadingsplash-fromstandalone.jpg", pFolderToUse);
			}
			else
			{
				if (bAreWeAEditor == true)
				{
					// use higher quality PNG when loading MAX!
					sprintf(fileName, "Files\\editors\\%s\\loadingsplash.png", pFolderToUse);
				}
				else
				{
					sprintf(fileName, "Files\\editors\\%s\\loadingsplash.jpg", pFolderToUse);
				}
			}

			// Do we have a storyboard ?
			if ( bAreWeAEditor == false )
			{
				// only for standalones while showing splash screen (moved from later in init sequence for elegance)
				// Must show the window (editor EXE hides by default)
				#ifdef _DEBUG
				// crashes when in debug
				#else
				SetWindowSettings (0, 0, 0);
				WindowToFront ("Game Guru Standalone Executable");
				SetWindowSize (GetDesktopWidth(), GetDesktopHeight());
				ShowWindow(); MaximiseWindow();
				DisableEscapeKey();
				#endif

				extern bool bSpecialStandalone;
				extern char cSpecialStandaloneProject[MAX_PATH];
				if(bSpecialStandalone)
					SetWriteSameAsRoot(false); //Allow to read from docwrite.
				else
					SetWriteSameAsRoot(true); //Standalone settings.
				void replaceAll(std::string& str, const std::string& from, const std::string& to);
				replaceAll(appname, ".exe", "");
				char tmp[MAX_PATH];
				strcpy(tmp, appname.c_str());
				int iPos = strlen(tmp) - 1;
				while (iPos > 0 && tmp[iPos] != '\\' && tmp[iPos] != '/' && tmp[iPos] != 0) iPos--;
				if (iPos > 0) strcpy(tmp, &tmp[iPos+1]);
				SetDir("Files\\");
				extern StoryboardStruct Storyboard;
				strcpy(Storyboard.gamename, "");

				//PE: Moved here load_storyboard now load media.
				extern void SetCanUse_e_(int);
				SetCanUse_e_(1);

				load_storyboard(tmp);
				if (strlen(Storyboard.gamename) > 0)
				{
					void FindFirstSplash(char *splash_name);
					FindFirstSplash(fileName);

					// Uses actual or virtual file..
					strcpy(VirtualFilename, fileName);
					extern bool CheckForWorkshopFile(LPSTR);
					CheckForWorkshopFile(VirtualFilename);
					if (GG_FileExists(VirtualFilename))
					{
						// Decrypt and use media
						g_pGlob->Decrypt(VirtualFilename);
						strcpy(fileName, VirtualFilename);
						bDecryptedFile = true;
					}
					if (!pestrcasestr(fileName, "Files\\"))
					{
						int GG_GetRealPath(char* fullPath, int create, bool bIgnoreAdditional = false);
						GG_GetRealPath(fileName, 0);
					}
				}
				SetDir("..");
				bCustomSplash = true;
			}
			std::shared_ptr<wiResource> tex = wiResourceManager::Load(fileName);
			if (tex) g_pSplashTexture = tex->texture;
			if (g_pSplashTexture.desc.Width == 0)
			{
				// could be a DDS renamed as JPG
				char pTryDDS[MAX_PATH];
				strcpy(pTryDDS, fileName);
				pTryDDS[strlen(pTryDDS) - 4] = 0;
				strcat(pTryDDS, ".dds");
				DeleteFileA(pTryDDS);
				CopyFileA(fileName, pTryDDS, FALSE);
				std::shared_ptr<wiResource> tex2 = wiResourceManager::Load(pTryDDS);
				if (tex2)
				{
					g_pSplashTexture = tex2->texture;
				}
			}
			if (bDecryptedFile == true)
			{
				// re-encrypt if applicable
				g_pGlob->Encrypt(VirtualFilename);
			}
			b_gSplashTextureLoaded = true;
			extern bool bSpecialStandalone;
			if (bAreWeAEditor == true && bSpecialStandalone==false && bSpecialEditorFromStandalone==false)
			{
				if (g_bFreeTrialVersion == true)
				{
					// play sound over splash (only for the free trial users)
					LPSTR pIntroSoundFile = "Files\\editors\\uiv3\\MAX.wav";
					LoadSound(pIntroSoundFile, 8809); // g.introsoundsoundoffset = 8809;
					if (SoundExist(8809) == 1) PlaySound(8809);
				}
			}
		}
	}
	else
	{
		// when GG completely initialised, can proceed normally
	}

	// handle visual splash until not needed
	if (g_iShowSplashForFirstFewCycles > 0)
	{
		// render splash screen here
		CommandList cmd = wiRenderer::GetDevice()->BeginCommandList(QUEUE_GRAPHICS, "Splash");
		wiRenderer::GetDevice()->RenderPassBegin(&swapChain, cmd);
		wiImage::SetCanvas(canvas, cmd);
		wiFont::SetCanvas(canvas, cmd);
		Viewport viewport;
		viewport.Width = (float)swapChain.desc.width;
		viewport.Height = (float)swapChain.desc.height;
		wiRenderer::GetDevice()->BindViewports(1, &viewport, cmd);
		wiFontParams params;
		params.posX = 5.f;
		params.posY = 5.f;
		if (g_pSplashTexture.IsValid())
		{
			if (bCustomSplash)
			{
				float screenheight = canvas.GetLogicalHeight();
				float screenwidth = canvas.GetLogicalWidth();
				float img_w = g_pSplashTexture.desc.Width;
				float img_h = g_pSplashTexture.desc.Height;
				float fRatio = 1.0f / (img_h / img_w);
				img_h = screenheight;
				img_w = screenheight * fRatio;
				if (img_w < screenwidth)
				{
					img_w = screenwidth;
					img_h = screenwidth * (1.0f / ((float)g_pSplashTexture.desc.Width / (float)g_pSplashTexture.desc.Height));
				}
				fx.disableFullScreen();
				fx.pos.x = screenwidth * 0.5;
				fx.pos.y = screenheight * 0.5;
				fx.pivot.x = 0.5;
				fx.pivot.y = 0.5;
				fx.siz.x = img_w;
				fx.siz.y = img_h;
			}
			wiImage::Draw(&g_pSplashTexture, fx, cmd);
		}

		// and overlay with a ratio corrected 'universal' logo for UltraWide goodness
		if (bAreWeAEditor == true)
		{ 
			bool bUniversalLogo = true;
			if (bUniversalLogo == true)
			{
				char logoFile[MAX_PATH];
				sprintf(logoFile, "Files\\editors\\uiv3\\MAX-Logo-Square.png");
				std::shared_ptr<wiResource> tex = wiResourceManager::Load(logoFile);
				wiGraphics::Texture pMAXLogoTexture;
				if (tex) pMAXLogoTexture = tex->texture;
				if (pMAXLogoTexture.IsValid())
				{
					float screenheight = canvas.GetLogicalHeight();
					float screenwidth = canvas.GetLogicalWidth();
					float img_w = pMAXLogoTexture.desc.Width;
					float img_h = pMAXLogoTexture.desc.Height;
					fx.disableFullScreen();
					fx.pos.x = screenwidth * 0.5;
					fx.pos.y = screenheight * 0.5;
					fx.pivot.x = 0.5;
					fx.pivot.y = 0.5;
					fx.siz.x = img_w;
					fx.siz.y = img_h;
					fx.blendFlag = BLENDMODE_ALPHA;
					wiImage::Draw(&pMAXLogoTexture, fx, cmd);
				}
			}
		}

		wiRenderer::GetDevice()->RenderPassEnd(cmd);
		wiRenderer::GetDevice()->SubmitCommandLists();
		g_iShowSplashForFirstFewCycles--;
		if (g_iShowSplashForFirstFewCycles <= 0)
		{
			if (SoundExist(g.temppreviewsoundoffset) == 1)
			{
				if (SoundPlaying(g.temppreviewsoundoffset) == 1)
				{
					// hold off one ending splash sequence until intro sound has finished!
					g_iShowSplashForFirstFewCycles = 1;
				}
				else
				{
					// and remove intro sound
					DeleteSound(8809);
				}
			}
		}
		if (g_iShowSplashForFirstFewCycles <= 0 )
		{
			// only hand over rendering submissions when splash is no longer needed
			g_iShowSplashForFirstFewCycles = 0;
			setCompletelyLoaded(true);
		}
	}
}

void Master::Finish(void)
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	GuruFinish();
}

bool Master::ForceRender(void* rt)
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	ID3D11RenderTargetView* nrt = (ID3D11RenderTargetView*) rt;
	if (!initialized)
	{
		return false;
	}
	if (!wiInitializer::IsInitializeFinished())
	{
		return false;
	}
	if ( !initializedSecondaries ) return false;

	m_bForceRender = true;

	masterrenderer.SetRenderingVR(true);
	Update(0);
	masterrenderer.SetRenderingVR(false);

	Render( 0 );

	CommandList cmd = wiRenderer::GetDevice()->BeginCommandList(QUEUE_GRAPHICS, "Compose");
	{
		if (nrt)
		{
			Viewport vp;
			#ifdef USEFIXEDBACKBUFFERSIZE
			//PE: This is the resolution all current thumbs has used.
			extern bool bProceduralLevel;
			extern bool bSnapShotModeUse2D;
			extern bool bFullScreenBackbuffer;
			if (bProceduralLevel || bSnapShotModeUse2D || bFullScreenBackbuffer)
			{
				Viewport vp;
				vp.Width = (float)masterrenderer.GetPhysicalWidth();
				vp.Height = (float)masterrenderer.GetPhysicalHeight();
				wiRenderer::GetDevice()->BindViewports(1, &vp, cmd);
			}
			else
			{
				vp.Width = (float)1920.0f;
				vp.Height = (float)1017.0f;
				wiRenderer::GetDevice()->BindViewports(1, &vp, cmd);
			}
			#endif
			wiRenderer::GetDevice()->SetRenderTarget(cmd, nrt);

		}
		else
		{
			wiRenderer::GetDevice()->RenderPassBegin( &swapChain, cmd );
		}
		extern bool bPopModalOpenProcedural;
		extern bool bSnapShotModeUse2D;
		extern bool BackBufferGrabGameScreen;
		if (!bPopModalOpenProcedural && !bSnapShotModeUse2D && !BackBufferGrabGameScreen)
		{
			masterrenderer.ComposeSimple(cmd);
		}
		else
		{
			if (BackBufferGrabGameScreen)
			{
				masterrenderer.ComposeSimple2D(cmd); //only 2D
			}
			else
			{
				masterrenderer.Compose(cmd); //Also do 2d compose
			}
		}

	}
	wiProfiler::EndFrame(cmd);

	//wiFont::Draw("TESTING FONT HERE", wiFontParams(148, 148, 30, WIFALIGN_LEFT, WIFALIGN_TOP, wiColor(255, 255, 255, 255)), cmd);
	wiRenderer::GetDevice()->SubmitCommandLists();
	m_bForceRender = false;
	return true;
}

void Master::RunCustom()
{
	// profiling
#ifdef OPTICK_ENABLE
	OPTICK_FRAME("MainThread");
#endif

	if (!initialized)
	{
		// Initialize in a lazy way, so the user application doesn't have to call this explicitly
#ifdef OPTICK_ENABLE
		OPTICK_EVENT("Initialize Primaries");
#endif
		Initialize();
		initialized = true;
	}
	if (!wiInitializer::IsInitializeFinished())
	{
		return;
	}
	if ( !initializedSecondaries )
	{
		#ifdef CUSTOMSHADERS
		void AddCustomShaders(void);
		AddCustomShaders();
		#endif

		// Good time to check if we have all necessarily files for the editor, otherwise we call auto updater to repair us 
		char pAppName[MAX_PATH];
		GetModuleFileNameA(g_pGlob->hInstance, pAppName, MAX_PATH);
		if ( strnicmp ( pAppName + strlen(pAppName) - 15, "gamegurumax.exe", 15 ) == 0 )
		{
			char pOldDir[MAX_PATH];
			GetCurrentDirectoryA(MAX_PATH, pOldDir);
			SetCurrentDirectoryA("Files");
			// MAP EDITOR MODE - check we have enough valid folders (initial install may have been aborted)
			// (is there a better eay to catch any start-up exceptions, and then offer to run the auto updater?)
			bool bFoundMissingFolder = false;
			for (int iCheckKeyFolders = 1; iCheckKeyFolders <= 23; iCheckKeyFolders++)
			{
				LPSTR pKeyFolder = "";
				if (iCheckKeyFolders == 1) pKeyFolder = "..\\shaders";
				if (iCheckKeyFolders == 2) pKeyFolder = "audiobank";
				if (iCheckKeyFolders == 3) pKeyFolder = "charactercreatorplus";
				if (iCheckKeyFolders == 4) pKeyFolder = "ebebank";
				if (iCheckKeyFolders == 5) pKeyFolder = "editors";
				if (iCheckKeyFolders == 6) pKeyFolder = "effectbank";
				if (iCheckKeyFolders == 7) pKeyFolder = "entitybank";
				if (iCheckKeyFolders == 8) pKeyFolder = "fontbank";
				if (iCheckKeyFolders == 9) pKeyFolder = "gamecore";
				if (iCheckKeyFolders == 10) pKeyFolder = "grassbank";
				if (iCheckKeyFolders == 11) pKeyFolder = "imagebank";
				if (iCheckKeyFolders == 12) pKeyFolder = "languagebank";
				if (iCheckKeyFolders == 13) pKeyFolder = "levelbank";
				if (iCheckKeyFolders == 14) pKeyFolder = "mapbank";
				if (iCheckKeyFolders == 15) pKeyFolder = "particlesbank";
				if (iCheckKeyFolders == 16) pKeyFolder = "scriptbank";
				if (iCheckKeyFolders == 17) pKeyFolder = "skybank";
				if (iCheckKeyFolders == 18) pKeyFolder = "terraintextures";
				if (iCheckKeyFolders == 19) pKeyFolder = "texturebank";
				if (iCheckKeyFolders == 20) pKeyFolder = "titlesbank";
				if (iCheckKeyFolders == 21) pKeyFolder = "tutorialbank";
				if (iCheckKeyFolders == 22) pKeyFolder = "vegbank";
				if (iCheckKeyFolders == 23) pKeyFolder = "videobank";
				if (PathExist(pKeyFolder) == 0)
				{
					bFoundMissingFolder = true;
					break;
				}
			}
			SetCurrentDirectoryA(pOldDir);
			if (bFoundMissingFolder == true)
			{
				MessageBoxA(NULL, "GameGuru MAX seems to have files missing, run VERIFY FILES on Steam to restore missing files", "GameGuru MAX Missing Files", MB_OK);
				extern bool g_bCascadeQuitFlag;
				g_bCascadeQuitFlag = true;
				PostQuitMessage(0);
				return;
			}
		}
#ifdef OPTICK_ENABLE
		OPTICK_EVENT("Initialize Secondaries");
#endif
		InitializeSecondaries(); // synchronous so no need to return and come back later
	}

	// we need to customize the Run() process when rendering in VR, 
	// so control both approaches here
	bool bRequireVRRendering = false;
	if ( m_bUsingVR == true && g_iActivelyUsingVRNow == 1 ) bRequireVRRendering = true;

	if ( bRequireVRRendering )
	{
		if ( !OpenXRIsInitialised() )
		{
#ifdef OPTICK_ENABLE
			OPTICK_EVENT("Initialize OpenXR");
#endif
			if ( OpenXRInit( (ID3D11Device*) wiRenderer::GetDevice()->GetDeviceForIMGUI() ) < 0 )
			{
				bRequireVRRendering = false;
				g_iActivelyUsingVRNow = 0;
			}
		}
	}

	// VR or not to VR
	if ( bRequireVRRendering == false )
	{
		// regular run calls update() and render() (above)
		masterrenderer.SetRenderingVR(false);
		Run();
	}
	else
	{
		// VR run calls
		masterrenderer.SetRenderingVR(true);
		
		RenderPath* activePath = GetActivePath();
		if ( !activePath ) return;

		// for an active VR session, set resolution
		//PE: resolutionScale need to be changed when we use FSR , this must be controlled by internalresolution. both FSR upscaling and internalresolution down scaling.
		float resolutionScale = 1.0f;
		if (!OpenXRIsSessionSetup())
		{
			OpenXRStartSession();
			int width, height;
			OpenXRGetResolution(OPENXR_RENDER_LEFT, &width, &height);
			if ( width == 0 || height == 0 )
			{
				Run();
				return;
			}
			bVsyncEnabled = false;
			wiEvent::SetVSync(bVsyncEnabled);
			masterrenderer.Set3DResolution( width/resolutionScale, height/resolutionScale );
		}

		// regular logic loop (stripped out render aspects)
		auto range = wiProfiler::BeginRangeCPU("Update - Logic");
		bool bFullyInitialised = GuruLoopLogic();
		wiProfiler::EndRange(range);

		deltaTime = float(std::max(0.0, timer.elapsed() / 1000.0));
		//LB: if leave app, timer.elapsed() is going to be huge on returning, causing weird animation issues
		// so cap this to a maximum so it reduces the glitch the user sees (without causing anything over 30fps to misbehave)
		double dThirtyFPSinMS = (1.0f / 30.0f);
		if (deltaTime > dThirtyFPSinMS) deltaTime = dThirtyFPSinMS;
		timer.record();

		if (bFullyInitialised == true)
		{
			// normal update
			wiScene::CameraComponent &camera = wiScene::GetCamera();

			// must be outside a render pass and only called once, even if VR renders twice
			CommandList cmd = wiRenderer::GetDevice()->BeginCommandList(QUEUE_GRAPHICS, "Particles");
			auto range = wiProfiler::BeginRangeCPU("Update - Particles");
			gpup_update(deltaTime, cmd);
			wiProfiler::EndRange(range);

			// terrain processing
			#ifdef GGTERRAIN_USE_NEW_TERRAIN
			extern bool bImGuiRenderTargetFocus;
			extern int g_iDisableTerrainSystem;
			PIXScopedEvent(PIX_COLOR_DEFAULT, "GGTerrain_Update");
			GGTerrain_Update(camera.Eye.x, camera.Eye.y, camera.Eye.z, cmd, bImGuiRenderTargetFocus);
			if (g_iDisableTerrainSystem == 0)
			{
				PIXScopedEvent(PIX_COLOR_DEFAULT, "GGTrees_Update");
				GGTrees_Update(camera.Eye.x, camera.Eye.y, camera.Eye.z, cmd, bImGuiRenderTargetFocus);
				PIXScopedEvent(PIX_COLOR_DEFAULT, "GGGrass_Update");
				GGGrass_Update(&camera, cmd, bImGuiRenderTargetFocus);
			}
			#endif

			// now just prepared IMGUI, but actual render called from Wicked hook
			GuruLoopRender();
		}
		
		// Wake up the events that need to be executed on the main thread, in thread safe manner:
		wiEvent::FireEvent(SYSTEM_EVENT_THREAD_SAFE_POINT, 0);

		// get ready with OpenXR
		OpenXRHandleEvents();

		// get player body
		XMMATRIX cameraOrig = wiScene::GetCamera().GetInvView();

		// PGUP/PGDN elsewhere is used to control this value
		XMMATRIX mOffset = DirectX::XMMatrixTranslation(0, g.gvrsitteradjust, 0);
		cameraOrig = DirectX::XMMatrixMultiply(cameraOrig, mOffset);

		// get left controller matrix
		XMFLOAT4X4 matLeft;
		OpenXRGetLeftHandMatrix((float*)&matLeft, 1.56f);
		matLeft._41 *= 39.37f; // convert from meters to inches
		matLeft._42 *= 39.37f;
		matLeft._43 *= 39.37f;

		// transform it by player body
		XMMATRIX mLeft = XMMATRIX((float*)&matLeft);
		mLeft = DirectX::XMMatrixMultiply(mLeft, cameraOrig);

		// apply it to hand object transform
		XMVECTOR pos, quat, scale;
		XMMatrixDecompose(&scale, &quat, &pos, mLeft);

		// set position and rotation to hand object as we need to retrieve the position and rotation in other places
		XMFLOAT3 objPos;
		XMStoreFloat3(&objPos, pos);
		XMMATRIX matXMRot = XMMatrixRotationQuaternion(quat);
		XMFLOAT4X4 matXMFloats;
		XMStoreFloat4x4(&matXMFloats, matXMRot);
		GGMATRIX matRot;
		matRot._11 = matXMFloats._11;
		matRot._12 = matXMFloats._12;
		matRot._13 = matXMFloats._13;
		matRot._14 = matXMFloats._14;
		matRot._21 = matXMFloats._21;
		matRot._22 = matXMFloats._22;
		matRot._23 = matXMFloats._23;
		matRot._24 = matXMFloats._24;
		matRot._31 = matXMFloats._31;
		matRot._32 = matXMFloats._32;
		matRot._33 = matXMFloats._33;
		matRot._34 = matXMFloats._34;
		matRot._41 = matXMFloats._41;
		matRot._42 = matXMFloats._42;
		matRot._43 = matXMFloats._43;
		matRot._44 = matXMFloats._44;
		GGVECTOR3 vecRot;
		AnglesFromMatrix(&matRot, &vecRot);
		GGVR_SetHandObjectForMAX(0, objPos.x, objPos.y, objPos.z, vecRot.x, vecRot.y, vecRot.z);
				
		// get right controller matrix
		XMFLOAT4X4 matRight;
		OpenXRGetRightHandMatrix((float*)&matRight, 1.56f);
		matRight._41 *= 39.37f; // convert from meters to inches
		matRight._42 *= 39.37f;
		matRight._43 *= 39.37f;

		// transform it by player body
		XMMATRIX mRight = XMMATRIX((float*)&matRight);
		mRight = DirectX::XMMatrixMultiply(mRight, cameraOrig);

		// apply it to hand object transform
		XMMatrixDecompose(&scale, &quat, &pos, mRight);

		// set position and rotation to hand object as we need to retrieve the position and rotation in other places
		XMStoreFloat3(&objPos, pos);
		matXMRot = XMMatrixRotationQuaternion(quat);
		XMStoreFloat4x4(&matXMFloats, matXMRot);
		matRot._11 = matXMFloats._11;
		matRot._12 = matXMFloats._12;
		matRot._13 = matXMFloats._13;
		matRot._14 = matXMFloats._14;
		matRot._21 = matXMFloats._21;
		matRot._22 = matXMFloats._22;
		matRot._23 = matXMFloats._23;
		matRot._24 = matXMFloats._24;
		matRot._31 = matXMFloats._31;
		matRot._32 = matXMFloats._32;
		matRot._33 = matXMFloats._33;
		matRot._34 = matXMFloats._34;
		matRot._41 = matXMFloats._41;
		matRot._42 = matXMFloats._42;
		matRot._43 = matXMFloats._43;
		matRot._44 = matXMFloats._44;
		AnglesFromMatrix(&matRot, &vecRot);
		GGVR_SetHandObjectForMAX(1, objPos.x, objPos.y, objPos.z, vecRot.x, vecRot.y, vecRot.z);

		// render + composite the left eye and right eye
		bool rendered = false;
		if (OpenXRIsSessionActive())
		{
			bool shouldRender = OpenXRStartFrame();
			if (shouldRender)
			{
				rendered = true;

				activePath->init(canvas);	
				activePath->PreUpdate();

				// begin profiling frame
				wiProfiler::BeginFrame();

				// uses proj matrix from OpenXR
				wiScene::GetCamera().SetCustomProjectionEnabled(true);

				// render left eye
				wiScene::TransformComponent camera_transform;
				OpenXRGetViewMat(OPENXR_RENDER_LEFT, (float*) &(camera_transform.world));

				camera_transform.world._41 *= 39.37f; // convert from meters to inches
				camera_transform.world._42 *= 39.37f;
				camera_transform.world._43 *= 39.37f;
				XMMATRIX m = XMMATRIX((float*) &(camera_transform.world));
				m = DirectX::XMMatrixMultiply(m, cameraOrig);
				memcpy(camera_transform.world.m, m.r, sizeof(float) * 16);
				wiScene::GetCamera().TransformCamera(camera_transform);
				OpenXRGetProjMat(OPENXR_RENDER_LEFT, t.visuals.CameraNEAR_f, t.visuals.CameraFAR_f, (float*) &(wiScene::GetCamera().Projection));
				wiScene::GetCamera().UpdateCamera();
				deltaTime = float(std::max(0.0, timer.elapsed() / 1000.0));
				timer.record();
				const float dt = framerate_lock ? (1.0f / targetFrameRate) : deltaTime;
				fadeManager.Update(dt);

				// Get rotation from left eye view and calculate euler angles for HMD local values (used by player global script)
				XMMATRIX matXMRot = XMMatrixRotationQuaternion(camera_transform.GetRotationV());
				XMFLOAT4X4 matXMFloats;
				XMStoreFloat4x4(&matXMFloats, matXMRot);
				GGMATRIX matRot;
				matRot._11 = matXMFloats._11;
				matRot._12 = matXMFloats._12;
				matRot._13 = matXMFloats._13;
				matRot._14 = matXMFloats._14;
				matRot._21 = matXMFloats._21;
				matRot._22 = matXMFloats._22;
				matRot._23 = matXMFloats._23;
				matRot._24 = matXMFloats._24;
				matRot._31 = matXMFloats._31;
				matRot._32 = matXMFloats._32;
				matRot._33 = matXMFloats._33;
				matRot._34 = matXMFloats._34;
				matRot._41 = matXMFloats._41;
				matRot._42 = matXMFloats._42;
				matRot._43 = matXMFloats._43;
				matRot._44 = matXMFloats._44;
				GGVECTOR3 vecAngles;
				AnglesFromMatrix(&matRot, &vecAngles);
				GGVECTOR3 vecNormal = GGVECTOR3(0, 0, 1);
				GGVec3TransformNormal(&vecNormal, &vecNormal, &matRot);
				GGVR_SetHMDDirectly(vecAngles.x, vecAngles.y, vecAngles.z, vecNormal.x, vecNormal.y, vecNormal.z);

				// Fixed time update
				{
					if (frameskip)
					{
						deltaTimeAccumulator += dt;
						if (deltaTimeAccumulator > 10)
						{
							// application probably lost control, fixed update would take too long
							deltaTimeAccumulator = 0;
						}
						const float targetFrameRateInv = 1.0f / targetFrameRate;
						while (deltaTimeAccumulator >= targetFrameRateInv)
						{
							FixedUpdate();
							deltaTimeAccumulator -= targetFrameRateInv;
						}
					}
					else
					{
						FixedUpdate();
					}
				}

				GGTrees_UpdateFrustumCulling( &wiScene::GetCamera() );

				// Variable-timed update:
				Update(dt);
				wiInput::Update( window );

				// render everything to left eye now
				Render( EYE_LEFT );

				activePath->StorePreviousLeft();

				// compose left eye
				ID3D11RenderTargetView* leftView = OpenXRStartRender(OPENXR_RENDER_LEFT);
				CommandList cmd = wiRenderer::GetDevice()->BeginCommandList(QUEUE_GRAPHICS, "LeftEye");
				wiRenderer::GetDevice()->SetRenderTarget(cmd, leftView);
				Viewport vp;
				vp.Width = (float)masterrenderer.GetWidth3D() * resolutionScale;
				vp.Height = (float)masterrenderer.GetHeight3D() * resolutionScale;
				wiRenderer::GetDevice()->BindViewports(1, &vp, cmd);
				masterrenderer.ComposeSimple(cmd);
				OpenXREndRender();

				// render right eye
				OpenXRGetViewMat(OPENXR_RENDER_RIGHT, (float*) &(camera_transform.world));
				camera_transform.world._41 *= 39.37f; // convert from meters to inches
				camera_transform.world._42 *= 39.37f;
				camera_transform.world._43 *= 39.37f;
				m = XMMATRIX((float*) &(camera_transform.world));
				m = DirectX::XMMatrixMultiply(m, cameraOrig);
				memcpy(camera_transform.world.m, m.r, sizeof(float) * 16);
				wiScene::GetCamera().TransformCamera(camera_transform);
				OpenXRGetProjMat(OPENXR_RENDER_RIGHT, t.visuals.CameraNEAR_f, t.visuals.CameraFAR_f, (float*) &(wiScene::GetCamera().Projection));
				wiScene::GetCamera().UpdateCamera();

				GGTrees_UpdateFrustumCulling( &wiScene::GetCamera() );

				Update(0);
				Render( EYE_RIGHT );

				activePath->StorePreviousRight();

				// compose right eye
				ID3D11RenderTargetView* rightView = OpenXRStartRender(OPENXR_RENDER_RIGHT);
				cmd = wiRenderer::GetDevice()->BeginCommandList(QUEUE_GRAPHICS, "RightEye");
				wiRenderer::GetDevice()->SetRenderTarget(cmd, rightView);
				vp.Width = (float)masterrenderer.GetWidth3D() * resolutionScale;
				vp.Height = (float)masterrenderer.GetHeight3D() * resolutionScale;
				wiRenderer::GetDevice()->BindViewports(1, &vp, cmd);
				masterrenderer.ComposeSimple(cmd);
				OpenXREndRender();

				// present final to device
				wiScene::GetCamera().SetCustomProjectionEnabled(false);
				cmd = wiRenderer::GetDevice()->BeginCommandList(QUEUE_GRAPHICS, "PresentFinal");
				wiRenderer::GetDevice()->RenderPassBegin( &swapChain, cmd );
				{
					wiImage::SetCanvas(canvas, cmd);
					wiFont::SetCanvas(canvas, cmd);
					Viewport viewport;
					viewport.Width = (float)swapChain.desc.width;
					viewport.Height = (float)swapChain.desc.height;
					wiRenderer::GetDevice()->BindViewports(1, &viewport, cmd);
					masterrenderer.ComposeSimple(cmd); // no 2D stuff, done through forcerender to quad plane
					wiRenderer::GetDevice()->RenderPassEnd(cmd);
					wiProfiler::EndFrame(cmd); // End before Present() so that GPU queries are properly recorded
				}
				wiRenderer::GetDevice()->SubmitCommandLists();
			}
			OpenXREndFrame();
		}

		// if not rendered at all, backtrack to regular
		if (!rendered)
		{
			Run();
		}
	}
	void GrabBackBufferCopy(void);
	GrabBackBufferCopy();

	// as GrabBackBufferCopy can save files, and those files need to exist before an entity refresh, use a delayed flag
	extern int g_iCheckExistingFilesModifiedDelayed;
	if (g_iCheckExistingFilesModifiedDelayed>0)
	{
		g_iCheckExistingFilesModifiedDelayed--;
		if (g_iCheckExistingFilesModifiedDelayed <= 0)
		{
			extern void CheckExistingFilesModified(bool);
			CheckExistingFilesModified(false);
			g_iCheckExistingFilesModifiedDelayed = 0;
		}
	}
}

void Master::StopVR()
{
	OpenXRDestroy();
}

void Master::DeleteContentsOfDBPDATA ( bool bOnlyIfOlderThan2DAYS )
{
	// Delete all files in current folder
	FFindFirstFile();
	int iAttempts=20;
	while(FGetFileReturnValue()!=-1L && iAttempts>0)
	{
		// only if older than 2 days
		bool bGo = false;
		if ( bOnlyIfOlderThan2DAYS==false ) bGo = true;
		if ( bOnlyIfOlderThan2DAYS==true )
		{
			time_t ThisFileDays = filedata.time_write / 60 / 60 / 24;
			time_t Difference = TodaysDay - ThisFileDays; 
			if ( Difference >= 2 )
			{
				// this file is at least 2 days old
				bGo = true;
			}
		}

		// go
		if ( bGo )
		{
			if(FGetActualTypeValue(filedata.attrib)==1)
			{
				if(stricmp(filedata.name, ".")!=NULL
				&& stricmp(filedata.name, "..")!=NULL)
				{
					char file[_MAX_PATH];
					strcpy(file, filedata.name);
					char old[_MAX_PATH];
					getcwd(old, _MAX_PATH);
					BOOL bResult = RemoveDirectoryA(file);
					if(bResult==FALSE)
					{
						_chdir(file);
						FFindCloseFile();
						DeleteContentsOfDBPDATA ( bOnlyIfOlderThan2DAYS );
						_chdir(old);
						BOOL bResult = RemoveDirectoryA(file);
						FFindFirstFile();
					}
					iAttempts--;
				}
			}
			else
			{
				DeleteFileA(filedata.name);
			}
		}
		FFindNextFile();
	}
	FFindCloseFile();
}

void Master::MakeOrEnterUniqueDBPDATA(void)
{
	// Default DBPDATA string
	strcpy(gpDBPDataName,"dbpdata");

	// Prepare DBPDATA string builder
	DWORD dwBuildID=2;
	char pBuildStart[_MAX_PATH];
	strcpy(pBuildStart, gpDBPDataName);

	// Make it as unique
	mkdir(gpDBPDataName);

	// get todays day when made directory
	FFindFirstFile();
	TodaysDay = filedata.time_write / 60 / 60 / 24;
	FFindCloseFile();
}

void Master::DeleteAllOldDBPDATAFolders(void)
{
	// Default DBPDATA string
	strcpy(gpDBPDataName,"dbpdata");

	// Prepare DBPDATA string builder
	DWORD dwBuildID=2;
	char pBuildStart[_MAX_PATH];
	strcpy(pBuildStart, gpDBPDataName);

	// Go for a million attempts
	while(dwBuildID<1000000)
	{
		// Check if DBPDATA folder exists
		if(_chdir(gpDBPDataName)!=-1 || dwBuildID==2)
		{
			// Delete contents of directory
			DeleteContentsOfDBPDATA(true);

			// Delete directory (if empty)
			_chdir("..");
			rmdir(gpDBPDataName);

			// Create new folder name from build data
			wsprintfA(gpDBPDataName, "%s%d", pBuildStart, dwBuildID);
		}
		else
		{
			// Done scanning
			break;
		}

		// Will change foldername slightly
		dwBuildID++;
	}
}

//
// MasterRenderer Functions
//

void MasterRenderer::Load()
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	__super::Load();

	// remove VSYNC cap
	wiEvent::SetVSync( false );

	// clear image management system early
	char pCurrentDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, pCurrentDir);
	WickedCall_InitImageManagement(pCurrentDir);

	// default wicked settings
	setSSREnabled ( false ); //PE: Not on by default.
	setReflectionsEnabled ( true );
	setFXAAEnabled ( false ); //PE: We already have MSAA 8 so.
	#ifdef POSTPROCESSRAIN
	setRainEnabled(false); //PE: test post process shader.
	setRainTextures("Files\\effectbank\\common\\rain_color.png", "Files\\effectbank\\common\\rain_normal.png");
	setRainOpacity(0.0);
	setRainScaleX(1.0);
	setRainScaleY(1.0);
	setRainRefreactionScale(0.01);
	#endif
	#ifdef POSTPROCESSSNOW
	setSnowLayers(15.0);
	setSnowDepth(0.5);
	setSnowWindiness(0.5);
	setSnowSpeed(0.15);
	setSnowOpacity(1.0);
	setSnowOffset(0.0);
	#endif
	setBloomEnabled ( true );
	setShadowsEnabled ( true );
	wiRenderer::SetTessellationEnabled(false); //PE: Tessellation dont work like this it has to be set per mesh, so have never worked.
	setLightShaftsEnabled ( true );
	setBloomThreshold ( 2.0f );
	setBloomStrength( 1.0f );

	// only activated when in TEST LEVEL
	setEyeAdaptionEnabled( false );

	// disable wicked backlog, can draw behind imgui , can be seen sometimes.
	if (wiBackLog::isActive()) 
	{
		wiBackLog::Toggle();
	}

	// for best terrain rendering
	wiGraphics::SamplerDesc desc = wiRenderer::GetSampler ( SSLOT_OBJECTSHADER )->GetDesc ( );
	desc.Filter = wiGraphics::FILTER_ANISOTROPIC;

	// create cloudy sky by default
	Scene weatherscene;
	g_weatherEntityID = CreateEntity();
	auto& weather = weatherscene.weathers.Create ( g_weatherEntityID );
	weather.ambient = XMFLOAT3(0.5f, 0.5f, 0.5f);
	weather.horizon = XMFLOAT3 ( 0.38f, 0.38f, 0.38f );
	weather.zenith = XMFLOAT3 ( 0.42f, 0.42f, 0.42f );
	
	//PE: We dont want any lightshaft from the sun. before we activate it.
	weather.cloudiness = 0.0f;
	weather.cloudSpeed = 0.0f;
	weather.fogStart = 0;
	weather.fogEnd = 5000;
	weather.SetRealisticSky( true );

	weather.SetVolumetricClouds( true );

	wiScene::GetScene ( ).Merge ( weatherscene );

	// create directional light from sun
	Entity entitySunLight = wiScene::GetScene ( ).Entity_CreateLight ( "sunLight", XMFLOAT3 ( 0, 3, 0 ), XMFLOAT3 ( 1, 1, 1 ), 4, 600 );
	g_entitySunLight = entitySunLight;
	LightComponent* lightSun = wiScene::GetScene ( ).lights.GetComponent ( entitySunLight );
	lightSun->SetCastShadow ( true );
	lightSun->SetVisualizerEnabled ( false );
	lightSun->direction = XMFLOAT3 ( 0.25, -0.5, 0.25 );
	lightSun->color = XMFLOAT3 ( 1.0, 1.0, 1.0 );
	lightSun->SetType ( wiScene::LightComponent::DIRECTIONAL );
	lightSun->SetVolumetricsEnabled( true );

	// LB: sun needs lens flare texture
	int iFlareCount = 3;
	lightSun->lensFlareRimTextures.resize(iFlareCount);
	lightSun->lensFlareNames.resize(iFlareCount);
	for (int iFlareChain = 0; iFlareChain < iFlareCount; iFlareChain++)
	{
		std::string fileName;
		if (iFlareChain == 0) fileName = "Files\\lensflares\\flare1.jpg";
		if (iFlareChain == 1) fileName = "Files\\lensflares\\flare2.jpg";
		if (iFlareChain == 2) fileName = "Files\\lensflares\\flare3.jpg";
		lightSun->lensFlareRimTextures[iFlareChain] = wiResourceManager::Load(fileName);
		lightSun->lensFlareNames[iFlareChain] = fileName;
	}

	// force window to maximised view
	HWND hWnd = GetActiveWindow();
	ShowWindow(hWnd, SW_MAXIMIZE);
}

void MasterRenderer::Update(float dt)
{
	// otherwise continue
	if (m_bRenderingVR == false)
	{
#ifdef OPTICK_ENABLE
		OPTICK_EVENT("GuruLoopLogic");
#endif
		// regular update mode
		auto range = wiProfiler::BeginRangeCPU("Update - Logic");
		bool bFullyInitialised = GuruLoopLogic();
		wiProfiler::EndRange(range);

		// no further than logic while in splash show mode
		if (g_iShowSplashForFirstFewCycles > 0)
		{
			return;
		}
		if (bFullyInitialised == true )
		{
			// normal update
			wiScene::CameraComponent &camera = wiScene::GetCamera();

			// must be outside a render pass and only called once, even if VR renders twice
			CommandList cmd = wiRenderer::GetDevice()->BeginCommandList(QUEUE_GRAPHICS, "Particles");
			auto range = wiProfiler::BeginRangeCPU("Update - Particles");
			gpup_update(dt, cmd);
			wiProfiler::EndRange(range);

			// we can skip all terrain activity when building a standalone
			extern bool bExport_Standalone_Window;
			if (bExport_Standalone_Window == false)
			{
				// terrain processing (if used)
				if (t.visuals.bEnableEmptyLevelMode == false)
				{
					extern int g_iDisableTerrainSystem;
					auto range3 = wiProfiler::BeginRangeCPU("Update - Terrain");
					extern bool bImGuiRenderTargetFocus;
					PIXScopedEvent(PIX_COLOR_DEFAULT, "GGTerrain_Update");
					GGTerrain_Update(camera.Eye.x, camera.Eye.y, camera.Eye.z, cmd, bImGuiRenderTargetFocus);
					if (g_iDisableTerrainSystem == 0)
					{
						PIXScopedEvent(PIX_COLOR_DEFAULT, "GGTrees_Update");
						GGTrees_Update(camera.Eye.x, camera.Eye.y, camera.Eye.z, cmd, bImGuiRenderTargetFocus);
						GGTrees_UpdateFrustumCulling(&camera);
						PIXScopedEvent(PIX_COLOR_DEFAULT, "GGGrass_Update");
						GGGrass_Update(&camera, cmd, bImGuiRenderTargetFocus);
					}
					wiProfiler::EndRange(range3);
				}
				else
				{
					// still need for terrain globals to update local params (for editable_size reading)
					PIXScopedEvent(PIX_COLOR_DEFAULT, "GGTerrain_Update_EmptyLevel");
					GGTerrain_Update_EmptyLevel(camera.Eye.x, camera.Eye.y, camera.Eye.z, cmd);
				}
			}
			
			#ifdef WICKEDPARTICLESYSTEM
			auto range4 = wiProfiler::BeginRangeCPU("Update - Emitters");
			WickedCall_UpdateEmitters();
			wiProfiler::EndRange(range4);
			#endif

			// now just prepared IMGUI, but actual render called from Wicked hook
			auto range2 = wiProfiler::BeginRangeCPU("Update - Render");
			GuruLoopRender();
			wiProfiler::EndRange(range2);
		}
	}

	//Disable wicked backlog, can draw behind imgui , can be seen sometimes. Make sure it is never activated.
	if (wiBackLog::isActive()) wiBackLog::Toggle();

	// super update
	auto range2 = wiProfiler::BeginRangeCPU("Update - Wicked");
	__super::Update(dt);
	wiProfiler::EndRange(range2);
}

void MasterRenderer::ResizeBuffers(void)
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	if ( GetInternalResolution().x == 0 || GetInternalResolution().y == 0 ) return;


	//PE: Must be the same, currently its out of synch with internal resolution.
	//PE: Cant change if in VR that have another resolution.
	if (!m_bUsingVR)
	{
		master.masterrenderer.Set3DResolution(master.masterrenderer.GetPhysicalWidth(), master.masterrenderer.GetPhysicalHeight(), false); //GGREDUCED
	}

	//PE: Resizebuffers change FOV.

	__super::ResizeBuffers();

	GraphicsDevice* device = wiRenderer::GetDevice();
	HRESULT hr;

	if (GetDepthStencil() != nullptr)
	{
		TextureDesc desc;
		desc.Width = GetWidth3D();
		desc.Height = GetHeight3D();
		desc.SampleCount = 1;
		desc.Format = FORMAT_R8_UNORM;
		desc.BindFlags = BIND_RENDER_TARGET | BIND_SHADER_RESOURCE;

		//PE: Get the below error when using dx11 debug layer, so switched to resolving MSAA instead.
		//D3D11 ERROR: ID3D11DeviceContext::Draw: The Shader Resource View dimension declared in the shader code (TEXTURE2D)
		//             does not match the view type bound to slot 23 of the Pixel Shader unit (TEXTURE2DMS).
		//             This mismatch is invalid if the shader actually uses the view (e.g. it is not skipped due to shader code branching).
		//             [ EXECUTION ERROR #354: DEVICE_DRAW_VIEW_DIMENSION_MISMATCH]

		hr = device->CreateTexture(&desc, nullptr, &rt_Outline);
		hr = device->CreateTexture(&desc, nullptr, &rt_Outline_Red);
		hr = device->CreateTexture(&desc, nullptr, &rt_Outline_Blue);
		if (getMSAASampleCount() > 1)
		{
			desc.SampleCount = getMSAASampleCount();
			hr = device->CreateTexture(&desc, nullptr, &rt_MSAAOutline);
			hr = device->CreateTexture(&desc, nullptr, &rt_MSAAOutline_Red);
			hr = device->CreateTexture(&desc, nullptr, &rt_MSAAOutline_Blue);
		}
		assert(SUCCEEDED(hr));
	}

	{
		RenderPassDesc desc;

		//PE: New wickedrepo.
		desc.attachments.push_back(RenderPassAttachment::RenderTarget(&rt_Outline, RenderPassAttachment::LOADOP_CLEAR));

		//PE: We now use the MSAA desc.SampleCount so dont need to resolve it.
		if (getMSAASampleCount() > 1)
		{
			desc.attachments[0].texture = &rt_MSAAOutline;
			desc.attachments.push_back(RenderPassAttachment::Resolve(&rt_Outline));
		}
		
		//wiTextureHelper::getBlack(),
		
		desc.attachments.push_back(
			RenderPassAttachment::DepthStencil(
				GetDepthStencil(),
				RenderPassAttachment::LOADOP_LOAD,
				RenderPassAttachment::STOREOP_STORE,
				IMAGE_LAYOUT_DEPTHSTENCIL_READONLY,
				IMAGE_LAYOUT_DEPTHSTENCIL_READONLY,
				IMAGE_LAYOUT_DEPTHSTENCIL_READONLY
			)
		);
		
		hr = device->CreateRenderPass(&desc, &renderpass_Outline);
		assert(hr);

		desc.attachments[0].texture = &rt_Outline_Red;
		if (getMSAASampleCount() > 1)
		{
			desc.attachments[0].texture = &rt_MSAAOutline_Red;
			desc.attachments[1].texture = &rt_Outline_Red;
		}
		hr = device->CreateRenderPass(&desc, &renderpass_Outline_Red);
		assert(hr);

		desc.attachments[0].texture = &rt_Outline_Blue;
		if (getMSAASampleCount() > 1)
		{
			desc.attachments[0].texture = &rt_MSAAOutline_Blue;
			desc.attachments[1].texture = &rt_Outline_Blue;
	}
		hr = device->CreateRenderPass(&desc, &renderpass_Outline_Blue);
		assert(hr);

	}

	GGTerrain_WindowResized();
	extern bool bTriggerFovUpdate;
	bTriggerFovUpdate = true; //PE: restore FOV.
}


float fGetHighlightThickness(void);

void Wicked_Render_Opaque_Scene(CommandList cmd)
{
	extern bool g_bNo2DRender;
	extern bool BackBufferSnapShotMode;
	if (bImGuiInTestGame)
	{
		if (bActivateStandaloneOutline && master_renderer->GetDepthStencil() != nullptr) //&& !translator.selected.empty())
		{
			GraphicsDevice* device = wiRenderer::GetDevice();

			XMFLOAT4 area;
			bool ImGuiHook_GetScissorArea(float* pX1, float* pY1, float* pX2, float* pY2);
			if (ImGuiHook_GetScissorArea(&area.x, &area.y, &area.z, &area.w) == true)
				device->SetScissorArea(cmd, area);

			wiRenderer::BindCommonResources(cmd);
			XMFLOAT4 col = XMFLOAT4(0.8f, 0.8f, 0.8f, 0.8f);;
			wiRenderer::Postprocess_Outline(rt_Outline, cmd, 0.1f, 0.8f, col);
			device->EventEnd(cmd);
			area = { 0, 0, (float)master.masterrenderer.GetWidth3D(), (float)master.masterrenderer.GetHeight3D() };
			device->SetScissorArea(cmd, area);
		}
		return;
	}

	if (!bImGuiInTestGame && (!g_bNo2DRender || BackBufferSnapShotMode) )
	{
		if (master_renderer->GetDepthStencil() != nullptr) //&& !translator.selected.empty())
		{
			GraphicsDevice* device = wiRenderer::GetDevice();

			XMFLOAT4 area;
			float thickness = fGetHighlightThickness();
			bool ImGuiHook_GetScissorArea(float* pX1, float* pY1, float* pX2, float* pY2);
			if (ImGuiHook_GetScissorArea(&area.x, &area.y, &area.z, &area.w) == true)
				device->SetScissorArea(cmd, area);

			wiRenderer::BindCommonResources(cmd);
			XMFLOAT4 col = selectionColor;
			col.w *= 0.65; //opacity;
			wiRenderer::Postprocess_Outline(rt_Outline, cmd, 0.1f, thickness, col);
			device->EventEnd(cmd);

			col = selectionColorRed;
			col.w *= 0.65; //opacity;
			wiRenderer::Postprocess_Outline(rt_Outline_Red, cmd, 0.1f, thickness, col);
			device->EventEnd(cmd);

			col = selectionColorBlue;
			col.w *= 0.65; //opacity;
			wiRenderer::Postprocess_Outline(rt_Outline_Blue, cmd, 0.1f, thickness, col);
			device->EventEnd(cmd);

			area = { 0, 0, (float)master.masterrenderer.GetWidth3D(), (float)master.masterrenderer.GetHeight3D() };
			device->SetScissorArea(cmd, area);

		}
	}
}

void MasterRenderer::Compose(CommandList cmd) const
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	__super::Compose(cmd);
}

void MasterRenderer::Render( int mode ) const
{
	__super::Render( mode );
}

// moved into function so we can call it at the right time from within renderpath3D, just before 2D is rendered
std::vector<int> g_StandaloneObjectHighlightList;
void MasterRenderer::RenderOutlineHighlighers(CommandList cmd) const
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	if (m_bRenderingVR == false)
	{
		// regular update
		// PE: Make sure highlight and other editor functions is not redered in test game.
		void WickedCall_RenderEditorFunctions(void);
		//PE: Moved to function so we can delete any already setup highlights.
		//if (!bImGuiInTestGame) WickedCall_RenderEditorFunctions();
		extern bool bImGuiRenderTargetFocus;
		WickedCall_SetRenderTargetMouseFocus(bImGuiRenderTargetFocus);
		WickedCall_RenderEditorFunctions();

		if (!bImGuiInTestGame)
		{
			// Selection outline:
			if (GetDepthStencil() != nullptr)
			{
				GraphicsDevice* device = wiRenderer::GetDevice();
				CommandList cmd = device->BeginCommandList(QUEUE_GRAPHICS, "OutlineMask1");

				device->EventBegin("GGMax - Selection Outline Mask", cmd);

				Viewport vp;
				vp.Width = (float)rt_Outline.GetDesc().Width;
				vp.Height = (float)rt_Outline.GetDesc().Height;
				device->BindViewports(1, &vp, cmd);

				wiImageParams fx;
				fx.enableFullScreen();
				fx.stencilComp = STENCILMODE::STENCILMODE_EQUAL;
				fx.stencilRefMode = STENCILREFMODE_USER;

				// Objects outline:
				{
					device->UnbindResources(TEXSLOT_ONDEMAND0, 1, cmd);
					device->RenderPassBegin(&renderpass_Outline, cmd);
					// Draw solid blocks of selected objects
					fx.stencilRef = EDITORSTENCILREF_HIGHLIGHT_OBJECT;
					wiImage::Draw(wiTextureHelper::getWhite(), fx, cmd);
					device->RenderPassEnd(cmd);
				}

				// Objects outline Red:
				{
					device->UnbindResources(TEXSLOT_ONDEMAND0, 1, cmd);
					device->RenderPassBegin(&renderpass_Outline_Red, cmd);
					// Draw solid blocks of selected objects
					fx.stencilRef = EDITORSTENCILREF_HIGHLIGHT_OBJECT_RED;
					wiImage::Draw(wiTextureHelper::getWhite(), fx, cmd);
					device->RenderPassEnd(cmd);
				}

				// Objects outline Blue:
				{
					device->UnbindResources(TEXSLOT_ONDEMAND0, 1, cmd);
					device->RenderPassBegin(&renderpass_Outline_Blue, cmd);
					// Draw solid blocks of selected objects
					fx.stencilRef = EDITORSTENCILREF_HIGHLIGHT_OBJECT_BLUE;
					wiImage::Draw(wiTextureHelper::getWhite(), fx, cmd);
					device->RenderPassEnd(cmd);
				}
				device->EventEnd(cmd);
			}
		}
		else
		{
			if (!bActivateStandaloneOutline) return;

			//PE: Add objects here for standalone highlights/Outline.
			if (g_StandaloneObjectHighlightList.size() > 0)
			{
				for (int i = 0; i < (int)g_StandaloneObjectHighlightList.size(); i++)
				{
					int obj = g_StandaloneObjectHighlightList[i];
					if (obj > 0)
					{
						if (ObjectExist(obj))
						{
							void* GetObjectsInternalData(int iID);
							sObject* pObject = (sObject*)GetObjectsInternalData(obj);
							if (pObject)
							{
								//WickedCall_SetObjectHighlight(pObject, false);
								void WickedCall_DrawObjctBox(sObject * pObject, XMFLOAT4 color, bool bThickLine, bool ForceBox);
								WickedCall_DrawObjctBox(pObject, XMFLOAT4(0.8f, 0.8f, 0.8f, 0.8f), false, false);
							}
						}
					}
				}
			}


			if (GetDepthStencil() != nullptr)
			{
				GraphicsDevice* device = wiRenderer::GetDevice();
				CommandList cmd = device->BeginCommandList(QUEUE_GRAPHICS, "OutlineMask2");

				device->EventBegin("GGMax - Selection Outline Mask", cmd);

				Viewport vp;
				vp.Width = (float)rt_Outline.GetDesc().Width;
				vp.Height = (float)rt_Outline.GetDesc().Height;
				device->BindViewports(1, &vp, cmd);

				wiImageParams fx;
				fx.enableFullScreen();
				fx.stencilComp = STENCILMODE::STENCILMODE_EQUAL;
				fx.stencilRefMode = STENCILREFMODE_USER;

				// Objects outline:
				{
					device->UnbindResources(TEXSLOT_ONDEMAND0, 1, cmd);
					device->RenderPassBegin(&renderpass_Outline, cmd);
					// Draw solid blocks of selected objects
					fx.stencilRef = EDITORSTENCILREF_HIGHLIGHT_OBJECT;
					wiImage::Draw(wiTextureHelper::getWhite(), fx, cmd);
					device->RenderPassEnd(cmd);
				}
				device->EventEnd(cmd);
			}
		}
	}
}
