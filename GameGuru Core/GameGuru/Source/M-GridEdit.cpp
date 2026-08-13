//----------------------------------------------------
//--- GAMEGURU - M-GridEdit
//----------------------------------------------------

// moved all the defines into the WICKED DEFINE area () 

// Includes 
#include "stdafx.h"
#include "gameguru.h"
#include "M-WelcomeSystem.h"
#include "M-Widget.h"
#include "GGVR.h"
#include "M-GridEditB.h"
#include "M-RPG.h"
#include "M-Workshop.h"

// OPTICK Performance
#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

#include "..\..\GameGuru\Imgui\imnodes.h"
int grideleprof_uniqui_id = 35000;
#define MAXTEXTINPUT 1024
float g_Storyboard_header_height = 150.0f;
char cTmpInput[MAXTEXTINPUT + 1];
int g_Storyboard_First_Level_Node = -1;
int g_Storyboard_Current_Level = -1;
bool g_Storyboard_Starting_New_Level = false;
cstr g_Storyboard_LoaderScreen_Name = "loading";
char g_Storyboard_First_fpm[256];
char g_Storyboard_Current_fpm[256];
char g_Storyboard_Current_lua[256];
char g_Storyboard_Current_Loading_Page[256];
std::vector<std::string> projectbank_list;
std::vector<std::string> projectbank_image;
std::vector<int> projectbank_imageid;
std::vector<int> projectbank_active;
StoryboardStruct Storyboard;
StoryboardStruct checkproject;
StoryboardStruct202 updateproject202;
std::vector< std::pair<ImFont*, std::string>> StoryboardFonts;
std::vector< std::pair<ImFont*, std::string>> DefaultStoryboardFonts;
bool bScreen_Editor_Window = false;
int iScreen_Editor_Node = -1;
int iStoryboardExecuteKey = 0;
bool bTriggerSaveAs = false;
bool bTriggerOpenProject = false;
int iDelayTriggerOpenProject = 0;
bool bTriggerSaveAsAfterNewLevel = false;
char SaveProjectAsName[256] = "\0";
char SaveProjectAsError[256] = "\0";
bool bTriggerWhatsNewInStoryboard = false;
bool bAddWhatNewToMenu = false;
bool bOpenProjectsFromWelcome = false;
cstr TriggerLoadGameProject = "";
bool bStoryboardFirstRunSetInitPos = false;
bool bStoryboardInitNodes = false;
bool bJustRederedScreenEditor = false;
int g_iRefreshLibraryFolders = 0;
int g_iRefreshLibraryFoldersAfterDelay = 0;
bool g_bCommonAssetsLoadOnce = true;
char statusbar[512];
#ifdef GGMAXEPIC
// No discounts mentioned in Epic Store listing for now
#else
#define FREETRIALONDISCOUNT
#endif

bool g_bUpdateAppAvailable = false;
bool g_bFreeTrialVersion = false;
int g_iFreeTrialDaysLeft = 0;
bool g_bFreeTrialNowExitsApp = false;

bool g_bAdjustPlaneXZUsingSurfaceXZ = false;
bool g_bResetPlaneAfterXZAdjust = false;
bool g_bHoldGridEntityPosWhenManaged = true;
float g_fHoldGridEntityPosX = 0;
float g_fHoldGridEntityPosY = 0;
float g_fHoldGridEntityPosZ = 0;
float g_fLocalTurnRotationForSmartMode = 0.0f;
int g_iStackToSurfaceMode = 0;
int g_iOrientToSurfaceMode = 0;
bool g_bParticleEditorPresent = false;
bool g_bBuildingEditorPresent = false;
DWORD g_dwParticleEditorProcessHandle = NULL;

int g_iIconImageInProperties = 0;
int g_iIconImageInPropertiesLastEntIndex = 0;
cstr g_iconImageInPropertiesLastName_s = "";
bool g_bChangedGameCollectionList = false;

bool g_bUpdateCollectionList = false;
bool g_bSelectedNewObjectToAddToLevel = false;

int g_iSuperTriggerFullGrassReveal = 0;

#include <algorithm>
#include <string>
#include "miniz.h"

int iGenralWindowsFlags = ImGuiWindowFlags_None | ImGuiWindowFlags_NoMove;
bool bBoostIconColors = false;
int iDisplayCircleFrames = 0;

#include "shellapi.h"
#include ".\\..\..\\Guru-WickedMAX\\GPUParticles.h"
using namespace GPUParticles;
#include "GGTerrain/GGTerrain.h"
using namespace GGTerrain;
#include "GGTerrain/GGTrees.h"
using namespace GGTrees;
#include "GGTerrain/GGGrass.h"
using namespace GGGrass;

#include ".\..\..\Guru-WickedMAX\wickedcalls.h"
#include "..\..\Guru-WickedMAX\master.h"
#define USE_ENTITY_TOOL_WINDOW
#ifdef DISPLAYBOUNDINGBOXIN_PROPERTIES
#define XMSTATICCOLOR XMFLOAT4(1.0f, 0.1f, 0.0f, 0.4f)
#define XMDYNAMICCOLOR XMFLOAT4(0.25f, 1.0f, 0.25f, 0.4f)
#else
#define XMSTATICCOLOR XMFLOAT4(1.0f, 0.25f, 0.0f, 0.0f)
#define XMDYNAMICCOLOR XMFLOAT4(0.25f, 1.0f, 0.25f, 0.0f)
#endif

#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

extern sObject* g_selected_editor_object;
extern int g_selected_editor_objectID;
extern XMFLOAT4 g_selected_editor_color;
extern float g_fSpecialDragInYAdjustment;

int iEditorGridSizeX = 100;
int iEditorGridSizeZ = 100;
bool bRenderTabTab = false;
bool bRenderNextFrame = false;
bool bNeedImGuiInput = false;
bool bProfilerEnable = false;
int iExtractMode = 0; //0 = find floor, 1 = extracted y value. , 3 = fixed y value.
float fExtractYValue = 0, fExtractFixedYValue = GGORIGIN_Y;
bool bExtractFixPivot = true;
bool g_bStandaloneSinglePlayer = true;
bool g_bStandaloneMultiPlayer = false;
bool g_bStandaloneVRMode = false;
bool g_bPreviewLighting = false;
int iLastOpenHeader = 1;
int iExecuteCTRLkey = 0;
int iExecuteALTkey = 0;
int iIncludeLeftIconSet = 0;
extern uint64_t g_hovered_dot_entity;
extern sObject* g_hovered_dot_pobject;
sObject* g_destination_dot_pobject = NULL;
sObject* g_source_dot_pobject = NULL;
sObject* g_selected_middle_dot_pobject = NULL;
bool bDotMiddleWindow = false;
ImVec2 vDotMiddleWindowPos;
#define MAXDOTMIDDLE 1000
#define DOTOBJECTIDADD 40000
#define DOTCURSOROBJECTID (70001+40000+20000)
#define DOTMIDDLEOBJECTID (70001+40000+20001)
#define MAXDOTARCSOBJECTS 20000
#define DOTARCSOBJECTID (70001+40000+20001+MAXDOTMIDDLE+1000000)
#define RELATIONOBJECTID (70001+40000+20001+MAXDOTMIDDLE)
#define RELATIONOBJECTMAX 1000
int iLargestDotObjectID = 70001;
int iLargestDotCount = 999999;
bool bDotObjectDragging = false;
int iDotMiddleInfoSource[MAXDOTMIDDLE];
int iDotMiddleInfoDestination[MAXDOTMIDDLE];
int iDotMiddleColor[MAXDOTMIDDLE];
int iDotArceColor[MAXDOTARCSOBJECTS];

int iDotMiddleInfoSourceType[MAXDOTMIDDLE];
int iDotMiddleInfoDestinationType[MAXDOTMIDDLE];
extern float fLastHitPosition[4];
int iCursorDotObject = 0;
bool bFactionWindow[16];
float fDrawDotCircleTimer = 0;
bool bDrawDotCircle = false;
float fDrawDotCircleRadius = 0.0;
int fDrawDotCircleFrom = 0;
bool g_bDotsAreVisible = false;

#define BACKBUFFERIMAGE (g.perentitypromptimageoffset+9000)
int BackBufferObjectID = 0;
bool BackBufferSnapShotMode = false;
bool BackBufferGrabGameScreen = false;
bool BackBufferParticlesMode = false;
int iBackBufferParticlesTrigger = 0;
int BackBufferParticleEmitter = -1;
bool bFullScreenBackbuffer = false;
bool bSnapShotModeUseCamera = false;
bool bSnapShotModeUse2D = false;
float fSnapShotModeCameraX = 0.0f, fSnapShotModeCameraY = 0.0f, fSnapShotModeCameraZ = 0.0f;
float fSnapShotModeCameraAngX = 0.0f, fSnapShotModeCameraAngY = 0.0f, fSnapShotModeCameraAngZ = 0.0f;
cstr g_LastGroupSaved_s;
bool g_bBehaviorEditorActive = false;
int bStopBackbufferGrab = 0;

bool library_createbehavior = false;
char library_newbehaviorname[256];

bool BackBufferIsGroup = false;
int BackBufferEntityID = 0;
int BackBufferImageID = 0;
int BackBufferSizeX = 0;
int BackBufferSizeY = 0;
float BackBufferRotateY = 0.0f, RestoreBackBufferRotateY = -1.0f;
float BackBufferRotateX = 0.0f, RestoreBackBufferRotateX = -1.0f;
float BackBufferRotateZ = 0.0f, RestoreBackBufferRotateZ = -1.0f;
float BackBufferZoom = 0.0f, RestoreBackBufferZoom = -1.0f;
float BackBufferCamMove = 0.0f;
float BackBufferCamLeft = 0.0f, RestoreBackBufferCamLeft = -1.0f;
float BackBufferCamUp = 0.0f, RestoreBackBufferCamUp = -1.0f;
bool bBackBufferAnimated = false;
bool bBackBufferRestoreCamera = false;
bool bEditorInFreeFlightMode = false;

bool bLoopBackBuffer = false;
bool bLoopFullFPS = false;
bool bRotateBackBuffer = false;
cstr BackBufferCacheName = "";
cstr ProjectCacheName = "";
cstr BackBufferSaveCacheName = "";
extern std::vector<sImageList> g_imageList;
static std::vector<sImageList> g_TempimageList;
int iRestoreEntidMaster = -1;
int fpe_current_loaded_script = -1;
int fpe_current_loaded_script_image = 0;
int fpe_current_loaded_script_image_count = 0;

bool bReadyToDropEntity = false;
bool bWaitOnMouseRelease = false;
bool bDraggingActive = false;
bool bDraggingActiveInitial = false;
int iDragDropActive = 0;
#define HITPOINTYSTARTPOS GGORIGIN_Y
float fHitPointX = 0.0f, fHitPointY = 0.0f, fHitPointZ = 0.0f;
float fHitOffsetX = 0.0f, fHitOffsetY = 0.0f, fHitOffsetZ = 0.0f;
float fHitRayFrom = 0.0f ,fLastHitY = 0.0f;

extern sObject* g_hovered_pobject;
bool bTriggerVisibleWidget = false;
bool bMouseInputSystemUsed = false;
int iLastHitObjectID = 0;
int iStartMouseX, iStartMouseY;
int iObjectMoveMode = 2; // default to move and find floor mode (smartest = Lees Sneaky Solution)
int iObjectMoveModeDropSystem = 0;
int iObjectMoveModeDropSystemUsing = 0;
bool bObjectAllowOverlapping = 1;

float fDebug = 0.0f, fDebug1 = 0.0f, fDebug2 = 0.0f, fDebug3 = 0.0f;

int i_switch_group_tab = 0;
int current_selected_group = -1;
int thumb_selected_group = -1;
bool group_editing_on = false;
bool bCreateNewGroupOnNextDrop = false;
int iLastEntityOnCursor = 0;
float fLastRubberBandX1 = 0.0f;
float fLastRubberBandX2 = 0.0f;
float fLastRubberBandY1 = 0.0f;
float fLastRubberBandY2 = 0.0f;
bool bDetectTerrainOnly = false;
bool bRubberBandCreated = false;
bool bDragCameraActive = false;
bool g_bThumbBankCopyMode = true;
bool g_bRefreshRotationValuesFromObjectOnce = false;
bool g_bRefreshScaleValuesFromObjectOnce = false;
bool g_bLightProbeScaleChanged = false;
bool g_bLightProbeInstantChange = false;
int g_iLightProbeInstantChangeCoolDown = 0;
int iReusePickObjectID = -1;
int iReusePickEntityID = -1;
float fReusePickHitX = 0, fReusePickHitY = 0, fReusePickHitZ = 0;
sObject* pReusePickObject = 0;
std::vector<sLibraryList> g_LibraryFileList;
cStr cLastProjectList = "";


cstr cCurrentBackDropImageFile = "None";
bool bUseBackDropImage = true;
cstr cUseBackbufferCubemap = "";
bool bBackbufferCubemapActive = false;
int iLastSelectedEntityGroup = -1;
int iLastSelectedEntity = 0;
int iSetSettingsFocusTab = 0;
bool bStoryboardWindow = false;
bool bStoryboardWindowOpenLoad = false;
bool bMarketplace_Window = false;
bool bTriggerCloseEntityWindow = false;
bool bMarketplace_Init = false;
bool bFreeTrial_Window = false;
bool bFreeTrial_Init = false;
cstr sDefaultImportPath = "";
bool bResetObjectLibrarySize = false;
bool bWelcomeScreen_Window = false;
bool bWelcomeNoBackButton = false;
bool bWelcomeScreen_Init = false;
std::map<std::string, int> selected_library_fpe;
bool bProceduralLevel = false;
bool bProceduralLevelFromStoryboard = false;
int iBlackoutForFrames = 0;
int iBlockRenderingForFrames = 0;
int iQuitProceduralLevel = false;
bool bProceduralLevelStartup = false;
int g_iUniqueGroupID = 1000;
cstr sGotoPreviewWithFile = "";
int iGotoPreviewType = 0;
int init_Left_Categories_Column_Width = 3;
int g_iDevToolsOpen = 0;
bool bInvulnerableMode = false;
bool bStartInvulnerableMode = false;
bool bNoSecondAsk = false;
int iWelcomeHeaderType = 0;
int iAboutLogoType = 0;
int active_tools_obj = 0;
int active_tools_entity_index = 0;
int g_iUseLODObjects = 1;
bool bDisableLODLoad = false;
int g_iDisableTerrainSystem = 0;
int g_iDisableJustGrassSystem = 0;
int g_iDisableWParticleSystem = 0;
int g_iDisableCrashLogSystem = 0;
int g_iEnablePIXMarkers = 0;
bool bSprayMoveWithMouse = false;

bool bTrashcanIconActive = false, bTrashcanIconActive2 = false;
int current_sort_order = 0;
int iWidgetSelection = 0;
bool bRotScaleAlreadyUpdated = false;
int old_iMSAASampleCount = -1;
int old_iFSRMode = -1;
int old_iMSAO = -1;
float old_fMSAOPower = -1.0;
int old_iShadowSpotCascadeResolution = -1;
int old_iShadowSpotResolution = -1;
int old_iShadowPointResolution = -1;
bool bForceRefreshLightCount = false;
int iUpdateOcean = 0;
bool bEditorLight = false;
cStr sNextLevelToLoad;

float fMouseWheelZoomFactor = 3.0;
bool g_bResetCameraToFreeFlightOnNewLevel = false;
float fLocalMax = 1000.0f;

// Defines
#define ENABLETUTORIALVIDEOS

// 
//  GAMEGURU MAP EDITOR EXECUTABLE CODE
// 

//Check if we are in f9 mode
extern bool g_occluderf9Mode;

// extern to global that toggles when load map removed from entities
extern bool g_bBlackListRemovedSomeEntities;
extern bool gbWelcomeSystemActive;
extern int g_iWelcomeLoopPage;
extern int g_trialStampDaysLeft;
int g_tstoreprojectmodifiedstatic = 0;

extern bool g_bCharacterCreatorPlusActivated;
// can prevent app from quitting out while in test game
extern bool g_bDisableQuitFlag;
extern bool bEnableWeather;
char cImGuiDebug[2048] = "\0";
bool bForceKey = false;
int iForceScancode = -1;
cstr csForceKey = "";
bool bForceKey2 = false;
cstr csForceKey2 = "";
bool bForceUndo = false;
bool bForceRedo = false;
int iLaunchAfterSync = 0;
bool bTriggerFovUpdate = false;
bool bKeepWindowsResponding = false;
int iLaunchAfterSyncAction = 0;
bool bLaunchTestGameAfterLoad = false;
bool bLaunchSaveStandalonefterLoad = false;
bool bCloseStoryboardAfterLoad = false;
int iLevelEditorFromStoryboardID = -1;
char pLaunchAfterSyncPreSelectModel[MAX_PATH] = "\0";
char pLaunchAfterSyncLastImportedModel[MAX_PATH] = "\0";
int iOldLaunchAfterSync = 0;
int iSkibFramesBeforeLaunch = 0;
DWORD gWindowSizeXOld = 0;
DWORD gWindowSizeYOld = 0;
DWORD gWindowSizeAddY = 0;
DWORD gWindowSizeAddX = 0;
DWORD gWindowVisibleOld = 0;
DWORD gWindowPosXOld = 0;
DWORD gWindowPosYOld = 0;
DWORD gWindowMaximized = 0;
int xmouseold = 0, ymouseold = 0;

extern bool bImGuiInTestGame;
extern bool bBlockImGuiUntilNewFrame;
extern bool bImGuiRenderWithNoCustomTextures;
extern bool bImGuiFrameState;
extern bool bImGuiReadyToRender;
extern bool bImGuiInitDone;
extern ImVec2 OldrenderTargetSize;
extern ImVec2 OldrenderTargetPos;
extern ImVec2 renderTargetAreaPos;
extern ImVec2 renderTargetAreaSize;
extern bool bImGuiRenderTargetFocus;
extern bool bImGuiGotFocus;
extern bool g_bCascadeQuitFlag;
extern int ImGuiStatusBar_Size;
extern char defaultWriteFolder[260];
bool bEntityGotFocus = false;
char cDirectOpen[260];
bool imgui_is_running = false;
int refresh_gui_docking = 0;
ImGuiID dock_main_tabs, dock_tools_windows;
cstr RedockNextWindow;
ImGuiViewport* viewport;
int toolbar_size;
bool g_bInTutorialMode = false;
int g_iCountdownToAlphaBetaMessage = 0;
ImVec4 drawCol_toogle;
int g_EntityClipboardAnchorEntityIndex = -1;
std::vector<int> g_EntityClipboard;

extern preferences pref;
extern cFolderItem MainEntityList;

bool bExport_Standalone_Window = false;
bool bExport_SaveToGameCloud_Window = false;
bool bExternal_Entities_Window = false;
int iDisplayLibraryType = 0;
int iDisplayLibrarySubType = 0;
int iLastDisplayLibraryType = -1;
cstr sStartLibrarySearchString = "";
cstr sTriggerCategorySelect = "";
int iLibraryStingReturnToID = 0;
int iSelectedLibraryStingReturnID = -1;
cstr sMakeDefaultSelecting = "";
cstr sSelectedLibrarySting = "";
bool bSelectLibraryViewAll = false;
bool bExternal_Entities_Init = false;
bool bEntity_Properties_Window = false;
bool bProperties_Window_Block_Mouse = false;
bool bCheckForClosing = false;
bool bCheckForClosingForce = false;
bool bBuilder_Properties_Window = false;
bool bBuilder_Left_Window = false;
bool bTerrain_Tools_Window = false;
bool bWaypoint_Window = false;
bool bDownloadStore_Window = false;
bool bImporter_Window = false;
bool bHelpVideo_Window = false;
bool bHelp_Window = false;
extern char cForceTutorialName[1024];
bool bHelp_Menu_Image_Window = false;
bool bAbout_Window = false;
bool bCredits_Window = false;
bool bBug_Reporting_Window = false;
bool bBug_RefreshBugList = false;
bool bAbout_Window_First_Run = false;
bool bCredits_Window_First_Run = true;
bool bAbout_Init = false;
bool Entity_Tools_Window = true;
bool bInfo_Window = false;
bool bInfo_Reload = false;
bool bInfo_Window_First_Run = true;
cstr cInfoMessage = "";
cstr cInfoImage = "", cInfoImageLast = "";
int iInfoUniqueId = 0;
extern int g_iActiveMonitors;

bool Visuals_Tools_Window = false;
bool Weather_Tools_Window = false;
bool Game_Settings_Window = false;
bool Logic_Settings_Window = false;
int iRestoreLastWindow = 0;
std::vector<sRubberBandType> vEntityLockedList;
#define MAXGROUPSLISTS 100
cstr sEntityGroupListName[MAXGROUPSLISTS];
std::vector<sRubberBandType> vEntityGroupList[MAXGROUPSLISTS];
int iEntityGroupListImage[MAXGROUPSLISTS];
bool bPreferences_Window = false;
char cPreferencesMessage[MAX_PATH] = { "\x0" };
bool Shooter_Tools_Window = false; // Shooter_Tools_Window not really a window now, just a filter mode for Object Tools
bool Puzzle_Tools_Window = false; //Not yet active. only for toggle state.
bool RPG_Tools_Window = false; //Not yet active. only for toggle state.
char cNextWindowFocus[256];
bool bEditGameSettings = false;
int media_icon_size_leftpanel = 64;
int iColumnsWidth_leftpanel = 110;
int iColumns_leftpanel = 0;
bool bDisplayText_leftpanel = true;
float fFontSize_leftpanel = 1.0;

cFolderItem::sFolderFiles *pDragDropFile = NULL;
int iOldgridentity = -1;
float fPropertiesColoumWidth = 100.0f;
bool bTriggerMessage = false;
bool bTriggerSmallMessage = false;
int iTriggerMessageDelay = 0;
int iTriggerMessageFrames = 0;
int iTriggerMessageY = 0;
char cTriggerMessage[MAX_PATH] = "\0";
char cSmallTriggerMessage[MAX_PATH] = "\0";
int iMessageTimer = 0;
ImVec4 drawCol_back;
ImVec4 drawCol_normal;
ImVec4 drawCol_hover;
ImVec4 drawCol_Down;
ImVec4 drawCol_black = { 0,0,0,0 };

extern ISpObjectToken * CCP_SelectedToken;
extern LPSTR pCCPVoiceSet;
extern char CCP_SpeakText[1024];
extern wchar_t CCP_SpeakText_w[1024];
extern int CCP_Speak_Rate;

std::vector<cstr> tutorial_list; //unsorted.
std::map<std::string, std::string> tutorial_files;
std::map<std::string, std::string> tutorial_videos;
std::map<std::string, std::string> tutorial_description;
std::vector<cstr> about_text; //unsorted.

bool bTutorial_Init = false;
int current_tutorial = -1;
int selected_tutorial = 0;
bool bVideoPlayerMaximized = false;
bool bSmallVideoPlayerMaximized = false;
bool bLastSmallVideoPlayerMaximized = false;

bool bVideoResumePossible = false;
bool bVideoPerccentStart = false;
int iVideoFindFirstFrame = 0;
int iVideoDelayExecute = 0;
bool bTutorialCheckAction = false;
int bDelayedTutorialCheckAction = -1;
int iDelayedCameraRestore = 0;
char cForceTutorialName[1024] = "\0";
char cTutorialName[TUTORIALMAXTEXT] = "\0";
cstr cVideoDescription = "";
ActiveTutorial tut;
bool bTutorialRendered = false;
bool bSmallVideoFrameStart = true;
bool bSetTutorialSectionLeft = false;

//Tooltip object code.
int iLastTooltipSelection = -1;
int iTooltipTimer = 0;
int iTooltipHoveredTimer = 0;
int iTooltipLastObjectId = 0;
bool iTooltipAlreadyLoaded = true;
bool iTooltipObjectReady = false;
float lastKeyTime = 0;
char cHelpMenuImage[MAX_PATH];
bool bLostFocus = false;
bool bRenderTargetModalMode = false;
int iStartupTime = 0;
cstr CurrentWinTitle = "";
int speech_ids[5];

extern bool bWaypointDrawmode;
extern float custom_back_color[4];
extern bool bUpdateVeg;
extern int iLastUpdateVeg;


float fEmptyLevelFloorY = 0;
bool bEmptyLevelGrid = false;

// moved here so Classic would compile
bool Shooter_Tools_Window_Active = false;
void DeleteWaypointsAddedToCurrentCursor(void);
void Add_Grid_Snap_To_Position(bool bFromWidgetMode);
float ImGuiGetMouseX(void);
float ImGuiGetMouseY(void);
void RotateAndMoveRubberBand(int iActiveObj, float fMovedActiveObjectX, float fMovedActiveObjectY, float fMovedActiveObjectZ, GGQUATERNION quatRotationEvent); //float fMovedActiveObjectRX, float fMovedActiveObjectRY, float fMovedActiveObjectRZ);
void SetStartPositionsForRubberBand(int iActiveObj);
void EmptyMessages(void);

	void HandleObjectDeletion();
	void ControlAdvancedSetting(int&, const char*, bool* = nullptr);
	void TestLevel_ToggleBoundary(bool _2d, bool _3d);
	void TestLevel_ToggleTreeVegWater(bool tree, bool veg, bool water);

void set_inputsys_mclick(int value)
{
	t.inputsys.mclick = value;
}

// GLOBAL to know when in welcome area
int iTriggerWelcomeSystemStuff = 0;
int iCountDownToShowQuickStartDialog = 0;

void gridedit_triggermessagehandler (bool bForceMessageNoFade)
{
	if (!bTriggerMessage && bTriggerSmallMessage && iTriggerMessageFrames > 0)
	{
		ImGuiViewport* mainviewport = ImGui::GetMainViewport();
		if (mainviewport)
		{
			ImDrawList* dl = ImGui::GetForegroundDrawList(mainviewport);
			if (dl)
			{
				ImGuiContext& g = *GImGui;
				float fontscale = 1.25;
				ImVec2 textsize = ImGui::CalcTextSize(cSmallTriggerMessage)  * fontscale;
				float vCenterTextX = (OldrenderTargetSize.x * 0.5) - (textsize.x * 0.5);
				ImVec4 background = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
				background.w = 0.7;

				dl->AddRectFilled(ImVec2(OldrenderTargetPos.x + vCenterTextX - 2.0, OldrenderTargetPos.y + 50.0 - 2.0), ImVec2(OldrenderTargetPos.x + vCenterTextX + textsize.x + 2.0, OldrenderTargetPos.y + 50.0 + textsize.y + 2.0), ImGui::GetColorU32(background), 2.0, ImDrawCornerFlags_All);
				dl->AddText(g.Font, g.FontSize * fontscale, ImVec2(OldrenderTargetPos.x + vCenterTextX, OldrenderTargetPos.y + 50.0 ), ImGui::GetColorU32(ImGuiCol_Text), cSmallTriggerMessage);
			}
		}
		iTriggerMessageFrames--;
		if (iTriggerMessageFrames == 0)
		{
			cSmallTriggerMessage[0] = 0;
			bTriggerSmallMessage = false;
		}
	}
	if (bTriggerMessage)
	{
		if (iTriggerMessageDelay > 0)
		{
			iTriggerMessageDelay--;
			return;
		}

		if (iTriggerMessageY == 1)
		{
			if (iMessageTimer == 0 || Timer() - iMessageTimer > 8100)
				iMessageTimer = Timer();
		}
		else
		{
			if (iMessageTimer == 0 || Timer() - iMessageTimer > 4100)
				iMessageTimer = Timer();
		}
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowViewport(viewport->ID);
		if (iTriggerMessageY > 0)
		{
			ImVec2 viewPortPos = ImGui::GetMainViewport()->Pos;
			ImVec2 viewPortSize = ImGui::GetMainViewport()->Size;
			ImGui::SetNextWindowPos(viewPortPos + ImVec2(50, 24+iTriggerMessageY), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(viewPortSize.x - 100, 0), ImGuiCond_Always);
		}
		else
		{
			//PE: Now always center on viewport instead of rendertarget. as we now are inside storyboard.
			ImVec2 viewPortPos = ImGui::GetMainViewport()->Pos;
			ImVec2 viewPortSize = ImGui::GetMainViewport()->Size;
			ImGui::SetNextWindowPos(viewPortPos + ImVec2(350, 130), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(viewPortSize.x - 700, 0), ImGuiCond_Always);
		}
		bool winopen = true;

		ImVec4* style_colors = ImGui::GetStyle().Colors;
		ImVec4 oldBgColor = style_colors[ImGuiCol_WindowBg];
		ImVec4 oldTextColor = style_colors[ImGuiCol_Text];

		float fader = 1.0f;
		if (bForceMessageNoFade == false)
		{
			if (iTriggerMessageY == 1)
				fader = ((float)Timer() - (float)iMessageTimer) / 1500.0f;
			else
				fader = ((float)Timer() - (float)iMessageTimer) / 1000.0f;

			fader -= 1.0;
			if (fader < 0) {
				fader = 0.0001;
			}
			fader /= 3.0;
			fader = 1.0 - fader;
			if (fader < 0.1)
			{
				bTriggerMessage = false;
				bTriggerSmallMessage = false;
				iTriggerMessageY = 0;
				iMessageTimer = 0;
			}
		}

		style_colors[ImGuiCol_WindowBg].x = 0.0;
		style_colors[ImGuiCol_WindowBg].y = 0.0;
		style_colors[ImGuiCol_WindowBg].z = 0.0;
		if (iTriggerMessageY == 1)
			style_colors[ImGuiCol_WindowBg].w *= (fader*0.85);
		else
			style_colors[ImGuiCol_WindowBg].w *= (fader*0.5);

		style_colors[ImGuiCol_Text].x = 1.0;
		style_colors[ImGuiCol_Text].y = 1.0;
		style_colors[ImGuiCol_Text].z = 1.0;
		style_colors[ImGuiCol_Text].w *= fader;

		ImGui::Begin("##Messageinfo", &winopen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs);
		ImGui::SetWindowFontScale(2.0);
		ImGui::Text(" ");
		float fTextSize = ImGui::CalcTextSize(cTriggerMessage).x;
		ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x*0.5) - (fTextSize*0.5), ImGui::GetCursorPos().y));

		ImGui::Text(cTriggerMessage);
		ImGui::Text(" ");
		ImGui::SetWindowFontScale(1.0);
		ImGui::End();
		style_colors[ImGuiCol_WindowBg] = oldBgColor;
		style_colors[ImGuiCol_Text] = oldTextColor;
	}
}

void vr_init (void)
{
	// VR Handling
	bool bSkipVRForNow = false;
	if (g.gvrmodefordevelopers == 1)
		bSkipVRForNow = false; // for users who wish to play with half-baked VR 
	else
		bSkipVRForNow = true; // saves 1750ms from init below!!!

	// No VR or RIFTMODE in Editor Mode
	g.globals.riftmode = 0;
	g.vrglobals.GGVREnabled = 0;
	if (bSkipVRForNow == true)
	{
		timestampactivity(0, "VR System disabled to improve launch speed (temporary)");
		g.vrglobals.GGVRUsingVRSystem = 1;
	}
	else
	{
		g.vrglobals.GGVRUsingVRSystem = 1;
		if (g.gvrmode == 2) g.vrglobals.GGVREnabled = 1; // OpenVR (Steam)
		if (g.gvrmode == 3) g.vrglobals.GGVREnabled = 2; // Windows Mixed Reality (Microsoft)
		char pVRSystemString[1024];
		sprintf(pVRSystemString, "choose VR system with mode %d", g.vrglobals.GGVREnabled);
		timestampactivity(0, pVRSystemString);
		int iErrorCode = GGVR_ChooseVRSystem(g.vrglobals.GGVREnabled, g.gproducelogfiles, "");// cstr(g.fpscrootdir_s + "\\GGWMR.dll").Get() );
		if (iErrorCode > 0)
		{
			// if VR headset is not present, switch VR off to speed up non-VR rendering (especially for debug)
			char pErrorStr[1024];
			sprintf(pErrorStr, "Error Choosing VR System : Code %d", iErrorCode);
			timestampactivity(0, pErrorStr);
			timestampactivity(0, "switching VR off, headset not detected");
			g.vrglobals.GGVREnabled = 0;
		}
		else
		{
			//PE: Only if we use vr.
			if (g.gvrmode > 0)
			{
				// Give portal enough time to start its launch, then get rid of GameWindow until we need it!
				Sleep(10);
				CloseWindow(g_pGlob->hOriginalhWnd);
				Sleep(10);
				g_pGlob->hOriginalhWnd = NULL;
				SetWindowPos(g_pGlob->hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
		}
	}
}

// mapeditor inits and loop call
int gguishadereffectindex = 0;
void mapeditorexecutable_init ( void )
{
	//  Means we are in the editor (1) or in standalone game (0)
	timestampactivity(0,"ide input mode");
	g.globals.ideinputmode = 1;

	// VR Support
	vr_init();

	//  Set device to get multisampling AA active in editor
	t.multisamplingfactor=0;
	t.multimonitormode=0;

	//  Init app
	timestampactivity(0,"sync states");
	SyncOn (   ); SyncRate (  0 );
	t.strwork = "" ; t.strwork = t.strwork + t.strarr_s[475]+" - [Editor]";
	timestampactivity(0,"window states");
	ShowWindow ( ); WindowToFront (  t.strwork.Get() );
	AlwaysActiveOff ( );

	// start thread loader for for generic (startup) files (multi-threaded loading)
	generic_preloadfiles();

	// moved auth check to void Master::Update(float dt) - as early as possible!

	// So entirely replace fixed function rendering, use this shader effect
	g.guishadereffectindex = loadinternaleffect("effectbank\\reloaded\\gui_basic.fx");
	gguishadereffectindex = g.guishadereffectindex;
	g.guidiffuseshadereffectindex = loadinternaleffect("effectbank\\reloaded\\gui_diffuse.fx");
	g.guiwireframeshadereffectindex = loadinternaleffect("effectbank\\reloaded\\gui_wireframe.fx");
	g.guidepthshadereffectindex = loadinternaleffect("effectbank\\reloaded\\gui_showdepth.fx");

	//  Camera aspect ratio adjustment for desktop resolution
	timestampactivity(0,"camera states");
	t.aspect_f=GetDesktopWidth() ; t.aspect_f=t.aspect_f/GetDesktopHeight();
	SetCameraAspect ( t.aspect_f );

	// 111115 - base start memory for GameGuru (overwritten if g.grestoreeditorsettings==0)
	timestampactivity(0,"memory states");
	g.gamememactuallyusedstart=SMEMAvailable(1);

	// Reset texture/profile in EBE folder
	ebe_restoreebedefaulttextures();

	//  Early editor only inits
	timestampactivity(0,"pre widget init state");
	t.tsplashstatusprogress_s="WIDGET INIT";
	timestampactivity(0,t.tsplashstatusprogress_s.Get());
	timestampactivity(0,"widget status update");
	version_splashtext_statusupdate ( );
	widget_init ( );

	t.tsplashstatusprogress_s="SLIDERS INIT";
	timestampactivity(0,t.tsplashstatusprogress_s.Get());
	version_splashtext_statusupdate ( );
	sliders_init ( );

	// Generic asset loading common to editor and game
	t.tresetforstartofeditor=1;
	t.tsplashstatusprogress_s="LOAD COMMON ASSETS";
	timestampactivity(0,t.tsplashstatusprogress_s.Get());
	version_splashtext_statusupdate ( );
	common_loadcommonassets ( 0 );

	//  Initialise meshes and editor resources
	t.tsplashstatusprogress_s="INIT EDITOR RESOURCES";
	timestampactivity(0,t.tsplashstatusprogress_s.Get());
	version_splashtext_statusupdate ( );
	t.lastgrideditselect=-1;
	g.gmapeditmode = 1;
	editor_init ( );

	//  Load resource file which has test game memory usage data contained
	t.tsplashstatusprogress_s="LOAD MAIN RESOURCES";
	timestampactivity(0,t.tsplashstatusprogress_s.Get());
	version_splashtext_statusupdate ( );
	loadresource();

	//  Call visuals loop once to set shader constants
	t.tsplashstatusprogress_s="UPDATE VISUAL SETTINGS";
	timestampactivity(0,t.tsplashstatusprogress_s.Get());
	version_splashtext_statusupdate ( );
	t.visuals=t.editorvisuals;
	t.visuals.refreshshaders=1;
	visuals_loop ( );
	bool bUpdateEngineToo = false;
	extern void visuals_shaderlevels_update_core (bool);
	visuals_shaderlevels_update_core (bUpdateEngineToo);
	//visuals_shaderlevels_update ( );

	//PE: FOV has changed here if on widescreen that adjust fov depending on aspect ratio.
	//PE: We must refesh the windows to account for the new fov.
	float gpw = master.masterrenderer.GetWidth3D();
	float gph = master.masterrenderer.GetHeight3D();
	if (((float)gpw / (float)gph) > 2.1 && gpw > 1920)
	{
		bTriggerFovUpdate = true; //PE: Set FOV.
	}

	// Load map editior settings
	t.bTriggerNewMapAtStart = true;
	t.bIgnoreFirstCallToNewLevel = false;
	if ( g.grestoreeditorsettings == 1 ) 
	{
		t.tsplashstatusprogress_s="RESTORE LAST PROJECT";
		timestampactivity(0,t.tsplashstatusprogress_s.Get());
		version_splashtext_statusupdate ( );
		//popup_text_close();
		t.tfile_s = g.mysystem.editorsGridedit_s+"cfg.cfg";//"editors\\gridedit\\cfg.cfg";
		if ( FileExist(t.tfile_s.Get()) == 1 ) 
		{
			//  Load last Editor CFG Settings
			t.skipfpmloading=1;
			editor_loadcfg ( );

			//  load project specified in CFG (worklevel.fpm?)
			mapfile_loadproject_fpm ( );

			//  Now wipe config (in case we fail to load in restart, we avoid an infinite loop)
			if (  FileExist(t.tfile_s.Get()) == 1  ) DeleteAFile ( t.tfile_s.Get() );

			//  load in current files in LEVELBANK\TESTMAP (not from FPM)
			gridedit_load_map ( );
			t.terrain.grassregionx1 = t.terrain.grassregionx2;
			extern int g_iSuperTriggerFullGrassReveal; // hmm, shoved in to get the damn grass showing on initial load!
			g_iSuperTriggerFullGrassReveal = 10;
			t.skipfpmloading=0;
		}
	}
	else
	{
		// Start Splash (only one which does not wait for Sync ( -as interface not avail. in debug) )
		t.tsplashstatusprogress_s="";
		timestampactivity(0,t.tsplashstatusprogress_s.Get());
		version_splashtext_statusupdate ( );
	}

	//  trigger zoom to aquire camera range for editor
	t.updatezoom=1;

	//  Start resource bar must accurately reflect ALL data loaded by editor
	if ( g.grestoreeditorsettings==0 )
	{
		t.gamememactuallyusedstarttriggercount = 5; // 111115 - one trigger to get STARTMEM at beginning of GameGuru session execution (honest value)
	}

	//  Var to control machine independent speed
	game_timeelapsed_init ( );
	t.tsl_f=Timer();

	// IDE announcement system (note VR Quest has this option)
	// (note VR Quest has this option)
	iTriggerWelcomeSystemStuff = 1;
	if (g.gshowannouncements == 1)
	{
		bTriggerWhatsNewInStoryboard = true;
	}
	// only show front dialogs if not resuming from previous session
	int iCountDownToShowQuickStartDialog = 0;
	if ( g.grestoreeditorsettings == 0 ) 
	{
		// Welcome quick start page
		g.quickstartmenumode = 0;
		if ( g.iFreeVersionModeActive != 0 )
		{
			editor_showquickstart ( 0 );
			welcome_free();
		}
		else
		{
			if (g.gshowonstartup == 1 || g.iTriggerSoftwareToQuit != 0) 
			{
				editor_showquickstart(0);
				welcome_free(); //PE: We must always close it for level auto load to work.
			}
			else
			{
				welcome_free();
			}
		}
	}
	else
	{
		// always need to close down loading splash
		welcome_free();
	}

	// After 5 minutes, trigger another trial reminder
	DWORD dwStartOfEditingSession = timeGetTime() + (1000*60*5);
	DWORD dwSecondReminder = 0;

	//Load needed images.
	image_preload_files_reset(); //PE: At this point we have no more thread loaded images to use.
	SetMipmapNum(1); //PE: mipmaps not needed.
	image_setlegacyimageloading(true);

	LoadImage("editors\\uiv3\\shape.png", TOOL_SHAPE);
	LoadImage("editors\\uiv3\\level.png", TOOL_LEVELMODE);
	LoadImage("editors\\uiv3\\storedlevel.png", TOOL_STOREDLEVEL);
	LoadImage("editors\\uiv3\\blendmode.png", TOOL_BLENDMODE);
	LoadImage("editors\\uiv3\\rampmode.png", TOOL_RAMPMODE);
	LoadImage("editors\\uiv3\\painttexture.png", TOOL_PAINTTEXTURE);
	LoadImage("editors\\uiv3\\paintgrass.png", TOOL_PAINTGRASS);
	LoadImage("editors\\uiv3\\entity.png", TOOL_ENTITY);
	LoadImage("editors\\uiv3\\markers.png", TOOL_MARKERS);
	LoadImage("editors\\uiv3\\waypoints.png", TOOL_WAYPOINTS);
	LoadImage("editors\\uiv3\\newwaypoints.png", TOOL_NEWWAYPOINTS);
	LoadImage("editors\\uiv3\\toolbar\\position.png", TOOLBAR_POSITION);
	LoadImage("editors\\uiv3\\toolbar\\scale.png", TOOLBAR_SCALE);
	LoadImage("editors\\uiv3\\toolbar\\rotate.png", TOOLBAR_ROTATE);
	LoadImage("editors\\uiv3\\toolbar\\grid.png", TOOLBAR_GRID);
	LoadImage("editors\\uiv3\\toolbar\\gridsettings.png", TOOLBAR_GRIDSETTINGS);	
	LoadImage("editors\\uiv3\\toolbar\\snap.png", TOOLBAR_SNAP);

	LoadImage("editors\\uiv3\\toolbar\\surface.png", TOOLBAR_SURFACE);
	LoadImage("editors\\uiv3\\toolbar\\vert.png", TOOLBAR_VERT);
	LoadImage("editors\\uiv3\\toolbar\\horizontal.png", TOOLBAR_HORI);

	if (FileExist("editors\\uiv3\\playbut-icon.png"))
	{
		LoadImage("editors\\uiv3\\playbut-icon.png", TOOL_TESTGAME);
	}
	else if (FileExist("editors\\uiv3\\play-icon.png"))
	{
		LoadImage("editors\\uiv3\\play-icon.png", TOOL_TESTGAME);
	}
	else
		LoadImage("editors\\uiv3\\testgame.png", TOOL_TESTGAME);
	LoadImage("editors\\uiv3\\vrmode.png", TOOL_VRMODE);
	LoadImage("editors\\uiv3\\savestandalone.png", TOOL_SOCIALVR);
	LoadImage("editors\\uiv3\\newlevel.png", TOOL_NEWLEVEL);
	LoadImage("editors\\uiv3\\loadlevel.png", TOOL_LOADLEVEL);
	LoadImage("editors\\uiv3\\savelevel.png", TOOL_SAVELEVEL);
	LoadImage("editors\\uiv3\\rounding_overlay_style0-h.png", ROUNDING_OVERLAY);
	LoadImage("editors\\uiv3\\ebe-block.png", EBE_BLOCK);
	LoadImage("editors\\uiv3\\ebe-column.png", EBE_COLUMN);
	LoadImage("editors\\uiv3\\ebe-cube.png", EBE_CUBE);
	LoadImage("editors\\uiv3\\ebe-floor.png", EBE_FLOOR);
	LoadImage("editors\\uiv3\\ebe-new.png", EBE_NEW);
	LoadImage("editors\\uiv3\\ebe-row.png", EBE_ROW);
	LoadImage("editors\\uiv3\\ebe-stairs.png", EBE_STAIRS);
	LoadImage("editors\\uiv3\\ebe-wall.png", EBE_WALL);
	LoadImage("editors\\uiv3\\builder.png", TOOL_BUILDER);
	LoadImage("editors\\uiv3\\ccp.png", TOOL_CCP);
	LoadImage("editors\\uiv3\\import.png", TOOL_IMPORT);
	LoadImage("editors\\uiv3\\media-play.png", MEDIA_PLAY);
	LoadImage("editors\\uiv3\\media-pause.png", MEDIA_PAUSE);
	LoadImage("editors\\uiv3\\media-refresh.png", MEDIA_REFRESH);
	LoadImage("editors\\uiv3\\media-record.png", MEDIA_RECORD);
	LoadImage("editors\\uiv3\\media-recording.png", MEDIA_RECORDING);
	LoadImage("editors\\uiv3\\media-recordprocessing.png", MEDIA_RECORDPROCESSING);
	LoadImage("editors\\uiv3\\pointer2.png", TUTORIAL_POINTER);
	LoadImage("editors\\uiv3\\pointer3.png", TUTORIAL_POINTERUP);	

	LoadImage("editors\\uiv3\\gameguru-max-logo.png", ABOUT_LOGO);
	iAboutLogoType = 0;
	LoadImage("editors\\uiv3\\ABOUT-TGC.png", ABOUT_TGC);
	LoadImage("editors\\uiv3\\ABOUT-Country.png", ABOUT_HB);
	LoadImage("editors\\uiv3\\ebe-control1.png", EBE_CONTROL1);
	LoadImage("editors\\uiv3\\ebe-control2.png", EBE_CONTROL2);
	LoadImage("editors\\uiv3\\shape-up.png", TOOL_SHAPE_UP);
	LoadImage("editors\\uiv3\\shape-down.png", TOOL_SHAPE_DOWN);
	LoadImage("editors\\uiv3\\drawwaypoints.png", TOOL_DRAWWAYPOINTS);
	LoadImage("editors\\uiv3\\dotcircle.png", TOOL_DOTCIRCLE);
	LoadImage("editors\\uiv3\\dotcircles.png", TOOL_DOTCIRCLE_S);
	LoadImage("editors\\uiv3\\dotcirclem.png", TOOL_DOTCIRCLE_M);

	LoadImage("editors\\uiv3\\ent-properties.png", TOOL_ENT_EDIT);
	LoadImage("editors\\uiv3\\ent-extract.png", TOOL_ENT_EXTRACT);
	LoadImage("editors\\uiv3\\ent-duplicate.png", TOOL_ENT_DUPLICATE);
	LoadImage("editors\\uiv3\\ent-lock.png", TOOL_ENT_LOCK);
	LoadImage("editors\\uiv3\\ent-findfloor.png", TOOL_ENT_FINDFLOOR);
	LoadImage("editors\\uiv3\\ent-delete.png", TOOL_ENT_DELETE);
	LoadImage("editors\\uiv3\\ent-search.png", TOOL_ENT_SEARCH);

	LoadImage("editors\\uiv3\\circle.png", TOOL_CIRCLE);
	LoadImage("editors\\uiv3\\circles.png", TOOL_CIRCLE_S);
	LoadImage("editors\\uiv3\\circlem.png", TOOL_CIRCLE_M);

	LoadImage("editors\\uiv3\\environment.png", TOOL_VISUALS);
	LoadImage("editors\\uiv3\\camera.png", TOOL_CAMERA);
	LoadImage("editors\\uiv3\\light.png", TOOL_CAMERALIGHT);
	LoadImage("editors\\uiv3\\goback.png", TOOL_GOBACK);
	LoadImage("editors\\uiv3\\goexit.png", TOOL_GOEXIT);
	LoadImage("editors\\uiv3\\media-maximize.png", MEDIA_MAXIMIZE);
	LoadImage("editors\\uiv3\\media-minimize.png", MEDIA_MINIMIZE);

	LoadImage("editors\\uiv3\\weather-sun.png", ENV_SUN);
	LoadImage("editors\\uiv3\\weather-rain.png", ENV_RAIN);
	LoadImage("editors\\uiv3\\weather-snow.png", ENV_SNOW);
	LoadImage("editors\\uiv3\\weather.png", ENV_WEATHER);
	LoadImage("editors\\uiv3\\tool-gamesettings.png", TOOL_GAME_SETTINGS);

	LoadImage("editors\\uiv3\\logic.png", TOOL_LOGIC);// shooter.png", TOOL_SHOOTER);
	
	LoadImage("entitybank\\_markers\\Trigger Zone.bmp", TOOL_TRIGGERZONE);
	LoadImage("entitybank\\_markers\\flag.bmp", TOOL_FLAG);

	LoadImage("editors\\uiv3\\pencil-small.png", TOOL_PENCIL);

	LoadImage("editors\\uiv3\\logichighlight.png", UI3D_DOTOBJECTS);	
	LoadImage("editors\\uiv3\\brain_logic_marker.dds", UI3D_DOTMIDDLEOBJECTS);

	LoadImage("editors\\uiv3\\shape-circle.png", SHAPE_CIRCLE);
	LoadImage("editors\\uiv3\\shape-square.png", SHAPE_SQUARE);


	LoadImage("editors\\uiv3\\key-alt.png", KEY_ALT);
	LoadImage("editors\\uiv3\\key-backspace.png", KEY_BACKSPACE);
	LoadImage("editors\\uiv3\\keyboard.png", KEY_KEYBOARD);
	LoadImage("editors\\uiv3\\key-control.png", KEY_CONTROL);
	LoadImage("editors\\uiv3\\key-minus.png", KEY_MINUS);
	LoadImage("editors\\uiv3\\key-plus.png", KEY_PLUS);
	LoadImage("editors\\uiv3\\key-shift.png", KEY_SHIFT);
	LoadImage("editors\\uiv3\\key-tab.png", KEY_TAB);
	LoadImage("editors\\uiv3\\left-mouse-button.png", MOUSE_LMB);
	LoadImage("editors\\uiv3\\right-mouse-button.png", MOUSE_RMB);

	LoadImage("editors\\uiv3\\key-r.png", KEY_R);
	LoadImage("editors\\uiv3\\key-delete.png", KEY_DELETE);
	LoadImage("editors\\uiv3\\key-y.png", KEY_Y);
	LoadImage("editors\\uiv3\\key-return.png", KEY_RETURN);
	LoadImage("editors\\uiv3\\key-pgup.png", KEY_PGUP);
	LoadImage("editors\\uiv3\\key-pgdn.png", KEY_PGDN);
	LoadImage("editors\\uiv3\\key-f.png", KEY_F);
	LoadImage("editors\\uiv3\\key-g.png", KEY_G);
	LoadImage("editors\\uiv3\\key-z.png", KEY_Z);
	LoadImage("editors\\uiv3\\key-i.png", KEY_I);

	LoadImage("editors\\uiv3\\key-n.png", KEY_N);
	LoadImage("editors\\uiv3\\key-l.png", KEY_L);
	LoadImage("editors\\uiv3\\key-e.png", KEY_E);
	LoadImage("editors\\uiv3\\key-space.png", KEY_SPACE);
	LoadImage("editors\\uiv3\\key-t.png", KEY_T);
	LoadImage("editors\\uiv3\\key-o.png", KEY_O);
	LoadImage("editors\\uiv3\\key-q.png", KEY_Q);


	LoadImage("editors\\uiv3\\key-separator.png", KEY_SEPARATOR);
	LoadImage("editors\\uiv3\\key-separator-small.png", KEY_SEPARATOR_SMALL);

	LoadImage("editors\\uiv3\\favoritesmall.png", MEDIA_FAVORITE);

	LoadImage("editors\\uiv3\\group-edit.png", TOOL_GROUPEDIT);
	LoadImage("editors\\uiv3\\ungroup.png", TOOL_UNGROUP);
	LoadImage("editors\\uiv3\\group.png", TOOL_GROUP);
	LoadImage("editors\\uiv3\\group-save.png", TOOL_GROUPSAVE);

	LoadImage("editors\\uiv3\\trashcan.png", TOOL_TRASHCAN);
	LoadImage("editors\\uiv3\\unlock-tools.png", TOOL_UNLOCK);
	LoadImage("editors\\uiv3\\lock-tools.png", TOOL_LOCK);
	LoadImage("editors\\uiv3\\smart-object.png", TOOL_SMARTOBJECT);
	
	LoadImage("editors\\uiv3\\favoritesmall-dis.png", MEDIA_FAVORITE_DIS);
	LoadImage("editors\\uiv3\\key-maximize.png", KEY_MAXIMIZE);

	LoadImage("editors\\uiv3\\middle-mouse-button.png", MOUSE_MMB);

	LoadImage("editors\\uiv3\\object-horizontal.png", OBJECT_MOVE_XZ);
	LoadImage("editors\\uiv3\\object-vert.png", OBJECT_MOVE_Y);
	LoadImage("editors\\uiv3\\object-surface.png", OBJECT_MOVE_SURFACESCAN);
	LoadImage("editors\\uiv3\\object-findfloor.png", OBJECT_MOVE_FINDFLOOR);
	LoadImage("editors\\uiv3\\object-orientation.png", OBJECT_MOVE_ORIENTATION);
	LoadImage("editors\\uiv3\\object-lock.png", OBJECT_MOVE_LOCK);
	LoadImage("editors\\uiv3\\object-unlock.png", OBJECT_MOVE_UNLOCK);

	LoadImage("editors\\uiv3\\key-control-shift.png", KEY_CONTROL_SHIFT);

	LoadImage("editors\\uiv3\\i-info.png", ICON_INFO);
	LoadImage("editors\\uiv3\\temp_infinity.png", IMPORTER_ALL_MESH);

	LoadImage("editors\\uiv3\\ent-filter.png", TOOL_ENT_FILTER);

	LoadImage("editors\\marketplace\\ggmax.png", MARKETPLACE_GGMAX);
	LoadImage("editors\\marketplace\\gc-store.png", MARKETPLACE_GCSTORE);
	LoadImage("editors\\marketplace\\sketchfab.png", MARKETPLACE_SKETCHFAB);
	LoadImage("editors\\marketplace\\filler.png", MARKETPLACE_FILLER);
	LoadImage("editors\\marketplace\\marketplace.png", MARKETPLACE_HEADER);
	LoadImage("editors\\marketplace\\shockwave-sound.png", MARKETPLACE_SHOCKWAVESOUND);
	LoadImage("editors\\marketplace\\community.png", MARKETPLACE_COMMUNITY);
	
	#ifdef FREETRIALONDISCOUNT
	LoadImage("editors\\freetrial\\header-sale.png", FREETRIAL_HEADER);
	LoadImage("editors\\freetrial\\body-sale.png", FREETRIAL_BODY);
	#else
	LoadImage("editors\\freetrial\\header.png", FREETRIAL_HEADER);
	LoadImage("editors\\freetrial\\body.png", FREETRIAL_BODY);
	#endif
	LoadImage("editors\\freetrial\\notavailable.png", FREETRIAL_NOTAVAILABLE);
	LoadImage("editors\\freetrial\\oldschooldigit-base-50px.png", FREETRIAL_COUNTER_BASE);
	LoadImage("editors\\freetrial\\header-sale-oneday.png", FREETRIAL_COUNTER_ONEDAY);			
	LoadImage("editors\\uiv3\\filler-rounded.png", WELCOME_FILLERROUNDED);

	LPSTR pWelcomeHeaderHUB = "editors\\uiv3\\welcome-header.png";
	if (g_bFreeTrialVersion == true)
	{
		pWelcomeHeaderHUB = "editors\\freetrial\\welcome-header.png";
	}
	if (FileExist(pWelcomeHeaderHUB))
	{
		LoadImage(pWelcomeHeaderHUB, WELCOME_HEADER);
		iWelcomeHeaderType = 3;
	}

	LoadImage("editors\\uiv3\\filetype-ogg.png", FILETYPE_OGG);
	LoadImage("editors\\uiv3\\filetype-wav.png", FILETYPE_WAV);
	LoadImage("editors\\uiv3\\filetype-mp3.png", FILETYPE_MP3);

	LoadImage("editors\\uiv3\\filetype-video.png", FILETYPE_VIDEO);

	LoadImage("editors\\uiv3\\player-start.png", PLAYER_START);
	LoadImage("editors\\uiv3\\player-start2.png", PLAYER_START2);
	LoadImage("editors\\uiv3\\filetype-particle.png", FILETYPE_PARTICLE);

	SetIconSet(true);
	SetMipmapNum(1); //PE: mipmaps not needed.
	image_setlegacyimageloading(true);

	LoadImage("editors\\uiv3\\light-point.png", LIGHT_POINT);
	LoadImage("editors\\uiv3\\light-spot.png", LIGHT_SPOT);

	LoadImage("editors\\uiv3\\checkbox-character-off.png", FILTER_CHAR_OFF);
	LoadImage("editors\\uiv3\\checkbox-character-on.png", FILTER_CHAR_ON);
	LoadImage("editors\\uiv3\\checkbox-scenary-off.png", FILTER_SCENARY_OFF);
	LoadImage("editors\\uiv3\\checkbox-scenary-on.png", FILTER_SCENARY_ON);
	LoadImage("editors\\uiv3\\checkbox-favorite-off.png", FILTER_FAVORITE_OFF);
	LoadImage("editors\\uiv3\\checkbox-favorite-on.png", FILTER_FAVORITE_ON);
	LoadImage("editors\\uiv3\\checkbox-hud-off.png", FILTER_HUD_OFF);
	LoadImage("editors\\uiv3\\checkbox-hud-on.png", FILTER_HUD_ON);
	LoadImage("editors\\uiv3\\checkbox-elements-off.png", FILTER_ELEMENTS_OFF);
	LoadImage("editors\\uiv3\\checkbox-elements-on.png", FILTER_ELEMENTS_ON);
	LoadImage("editors\\uiv3\\checkbox-user-off.png", FILTER_USER_OFF);
	LoadImage("editors\\uiv3\\checkbox-user-on.png", FILTER_USER_ON);
	LoadImage("editors\\uiv3\\checkbox-dlua-off.png", FILTER_DLUA_OFF);
	LoadImage("editors\\uiv3\\checkbox-dlua-on.png", FILTER_DLUA_ON);

	LoadImage("editors\\uiv3\\pin.png", MEDIA_PIN);
	LoadImage("editors\\uiv3\\unpin.png", MEDIA_UNPIN);
	LoadImage("editors\\uiv3\\tool-mountain.png", TOOL_TERRAIN_TOOLBAR);

	LoadImage("editors\\uiv3\\ccp-none.png", CCP_NONE);
	LoadImage("editors\\uiv3\\ccp-empty.png", CCP_EMPTY);

	LoadImage("editors\\uiv3\\terrain-random.png", TERRAIN_RANDOM);
	LoadImage("editors\\uiv3\\terrain-pick.png", TERRAIN_PICK);
	LoadImage("editors\\uiv3\\terrain-write.png", TERRAIN_WRITE);
	LoadImage("editors\\uiv3\\terrain-restore.png", TERRAIN_RESTORE);
	if (FileExist("editors\\uiv3\\storyboard-header6.png"))
	{
		LoadImage("editors\\uiv3\\storyboard-header6.png", STORYBOARD_HEADER);
		g_Storyboard_header_height = 94.0f; //PE: More storyboard area. and same size as hud header.
	}
	else
		if (FileExist("editors\\uiv3\\storyboard-header5.png"))
	{
		LoadImage("editors\\uiv3\\storyboard-header5.png", STORYBOARD_HEADER);
		g_Storyboard_header_height = 114.0f; //PE: Way better on ultra wide monitors.
	}

	LoadImage("editors\\uiv3\\entity_image2.png", STORYBOARD_BACKDROP);
	LoadImage("editors\\uiv3\\entity_music2.png", STORYBOARD_MUSIC);
	LoadImage("editors\\uiv3\\entity_checkpoint2.png", STORYBOARD_PREVIEW);
	LoadImage("editors\\templates\\backdrops\\transparent-backdrop.png", STORYBOARD_TRANSPARET);

	LoadImage("tutorialbank\\welcome-video.jpg", WELCOME_VIDEO);

	LoadImage("editors\\uiv3\\tree_tool.png", TOOL_PAINTTREE); //PE: Need another one for this, no +
	LoadImage("editors\\uiv3\\tree_add.png", TOOL_TREE_ADD);
	LoadImage("editors\\uiv3\\tree_delete.png", TOOL_TREE_DELETE);
	LoadImage("editors\\uiv3\\tree_move.png", TOOL_TREE_MOVE);
	LoadImage("editors\\uiv3\\trees_add.png", TOOL_TREES_ADD);
	LoadImage("editors\\uiv3\\trees_delete_2.png", TOOL_TREES_DELETE);

	LoadImage("editors\\uiv3\\click-here-box.png", BOX_CLICK_HERE);

	LoadImage("editors\\uiv3\\brain-icon.png", BRAIN_ICON);
	LoadImage("editors\\uiv3\\icon-question.png", QUESTION_ICON);

	LoadImage("editors\\uiv3\\terrain mover.dds", UI3D_TERRAINMOVER);//dotmiddleobject.png", UI3D_DOTMIDDLEOBJECTS);

	LoadImage("editors\\uiv3\\icon_bush.png", TOOL_PAINTBUSH); //PE: Need another one for this, no +
	LoadImage("editors\\uiv3\\add_bush.png", TOOL_BUSH_ADD);
	LoadImage("editors\\uiv3\\delete_bush.png", TOOL_BUSH_DELETE);
	LoadImage("editors\\uiv3\\move_bush.png", TOOL_BUSH_MOVE);
	LoadImage("editors\\uiv3\\paint_bushes.png", TOOL_BUSHES_ADD);
	LoadImage("editors\\uiv3\\delete_bushes.png", TOOL_BUSHES_DELETE);
	LoadImage("editors\\uiv3\\scale_bush.png", TOOL_BUSH_SCALE);
	LoadImage("editors\\uiv3\\scale_tree.png", TOOL_TREE_SCALE);

	LoadImage("editors\\uiv3\\hub-livebroadcasts.png", HUB_LIVEBROADCAST);
	LoadImage("editors\\uiv3\\hub-discordwide.png", HUB_DISCORD);
	LoadImage("editors\\uiv3\\hub-facebook.png", HUB_FACEBOOK);
	LoadImage("editors\\uiv3\\hub-forum.png", HUB_FORUM);
	LoadImage("editors\\uiv3\\hub-workshopitem.png", HUB_WORKSHOPITEM);
	LoadImage("editors\\uiv3\\hub-tiktok.png", HUB_TIKTOK);
	LoadImage("editors\\uiv3\\hub-twitter.png", HUB_TWITTER);
	LoadImage("editors\\uiv3\\hub-userguide.png", HUB_USERGUIDE);
	LoadImage("editors\\uiv3\\hub-website.png", HUB_WEBSITE);

	LoadImage("editors\\uiv3\\hub-commtut-0-placeholder.png", HUB_COMMTUT0);
	LoadImage("editors\\uiv3\\hub-commtut-1-bmi.png", HUB_COMMTUT1);
	LoadImage("editors\\uiv3\\hub-commtut-2-plemsoft.png", HUB_COMMTUT2);
	LoadImage("editors\\uiv3\\hub-commtut-3-extreme.png", HUB_COMMTUT3);

	LoadImage("editors\\uiv3\\image-icon.png", SCREENEDITOR_IMAGE);
	LoadImage("editors\\uiv3\\text-icon.png", SCREENEDITOR_TEXT);
	LoadImage("editors\\uiv3\\button-icon.png", SCREENEDITOR_BUTTON);
	LoadImage("editors\\uiv3\\radiobutton-icon.png", SCREENEDITOR_RADIOBUTTON);
	LoadImage("editors\\uiv3\\tick-box-icon.png", SCREENEDITOR_TICKBOX);
	LoadImage("editors\\uiv3\\slider-icon.png", SCREENEDITOR_SLIDER);
	LoadImage("editors\\uiv3\\progressbar-icon.png", SCREENEDITOR_PROGRESSBAR);
	LoadImage("editors\\uiv3\\textarea-icon.png", SCREENEDITOR_TEXTAREA);
	LoadImage("editors\\uiv3\\video-icon.png", SCREENEDITOR_VIDEO);

	ImNodes::CreateContext();

	gridedit_makelighthybrid();

	image_setlegacyimageloading(false);
	SetMipmapNum(-1);

	ChangeGGFont("editors\\uiv3\\Roboto-Medium.ttf",15);

	extern char launchLoadOnStartup[260];
	if (strlen(launchLoadOnStartup) > 0 ) 
	{
		strcpy(cDirectOpen, launchLoadOnStartup);
		if (strlen(cDirectOpen) > 0) {

			t.returnstring_s = cDirectOpen;
			if (t.returnstring_s != "")
			{
				if (cstr(Lower(Right(t.returnstring_s.Get(), 4))) == ".fpm")
				{
					t.gridentity = 0;
					t.inputsys.constructselection = 0;
					t.inputsys.domodeentity = 1;
					t.grideditselect = 5;
					editor_refresheditmarkers();

					g.projectfilename_s = t.returnstring_s;
					gridedit_load_map();
					g_EntityClipboard.clear(); //PE: Clear any old copy/paste.
					undosys_clearall(); //PE: Clear undo redo system.
					t.terrain.grassregionx1 = t.terrain.grassregionx2;
					bUpdateVeg = true;

					//Locate player start marker.
					for (t.e = 1; t.e <= g.entityelementlist; t.e++)
					{
						if (t.entityelement[t.e].bankindex > 0)
						{
							if (t.entityprofile[t.entityelement[t.e].bankindex].ismarker == 1 && t.entityprofile[t.entityelement[t.e].bankindex].lives != -1)
							{
								//Point camera.
								t.obj = t.entityelement[t.e].obj;
								if (t.obj > 0) {
									float offsetx = ((float)GetDesktopWidth() - renderTargetAreaSize.x) * 0.25f;
									t.cx_f = ObjectPositionX(t.obj) + offsetx; //t.editorfreeflight.c.x_f;
									t.cy_f = ObjectPositionZ(t.obj); //t.editorfreeflight.c.z_f;
								}
								break;
							}
						}
					}

				}
			}
		}

		iLastUpdateVeg = 0;
		bUpdateVeg = true;
	}
	else 
	{
	}

	//PE: redirect all to image.
	SetCameraToImage(0, g.postprocessimageoffset, GetDisplayWidth(), GetDisplayHeight(), 2);
	imgui_is_running = true;

	//Make sure we have envmap.
	visuals_justshaderupdate();
	t.visuals.refreshskysettingsfromlua = true;
	cubemap_generateglobalenvmap();
	t.visuals.refreshskysettingsfromlua = false;

	t.visuals.VegQuantity_f = t.gamevisuals.VegQuantity_f;
	t.visuals.VegWidth_f = t.gamevisuals.VegWidth_f;
	t.visuals.VegHeight_f = t.gamevisuals.VegHeight_f;

	t.terrain.grassupdateafterterrain = 1;
	t.terrain.grassupdateafterterrain = 0;
	ShowVegetationGrid();
	visuals_justshaderupdate();

	// Moved last so we can load levels before main loop.
	// start thread loader for Character Creator texture files (multi-threaded loading) (saves 2s if started CCP)
	timestampactivity(0, "preload CCP textures early");
	charactercreatorplus_preloadinitialcharacter();

	// build character types list early as needed for FPE parsing
	extern void charactercreatorplus_populatechartypes(void);
	timestampactivity(0, "preload FPE character types");
	charactercreatorplus_populatechartypes();

	//  Main loop
	iStartupTime = Timer();
	timestampactivity(0, "Guru Map Editor Loop Starts");

	//Default to OBJECT TOOL panel (so can view tutorials right away)
	bForceKey = true;
	csForceKey = "o";
	t.gridentitymarkersmodeonly = 0; 
	t.grideditselect = 0;
	
	// trigger an alha/beta prompt
	g_iCountdownToAlphaBetaMessage = 20;

	//Trigger welcome screen.
	bWelcomeScreen_Window = false;

	extern bool bSpecialEditorFromStandalone;
	extern bool bEnsureIntroVideoIsNotRun;
	extern bool bReturnToWelcome;
	if (pref.iDisplayWelcomeScreen == 1)
	{
		if (bSpecialEditorFromStandalone && bReturnToWelcome)
		{
			bWelcomeNoBackButton = true;
			bWelcomeScreen_Window = true;
		}
		else if (!bSpecialEditorFromStandalone)
		{
			bWelcomeNoBackButton = true;
			bWelcomeScreen_Window = true;
		}
	}

	//LB: ensure special return from standalone flag reset if final destination was welcome HUB (to prevent storyboard being forced to switch)
	if (bReturnToWelcome == true)
	{
		bSpecialEditorFromStandalone = false;
		bEnsureIntroVideoIsNotRun = true;
		bReturnToWelcome = false;
	}

	t.gridentitygridlock = pref.iGridMode;
}

void mapeditorexecutable_loop_leavetestgame(void)
{
	bBlockImGuiUntilNewFrame = true;
	bRenderNextFrame = false;
	SetCameraToImage(0, g.postprocessimageoffset, GetDisplayWidth(), GetDisplayHeight(), 2); //switch back to render target.
	iLaunchAfterSync = 0;
	bImGuiInTestGame = false;
	fpe_current_loaded_script = -1; //Refresh dlua after testgame.
	sky_show(); //Restore skybox.
	iLastUpdateVeg = 0; //Veg: update any changes from F9
	bUpdateVeg = true;

	// set vsync back on when we return to the editor so we don't 100% the GPU
	if (g.iEditorVSync == 0)
		wiEvent::SetVSync(false); // see if this improves performance in the level editor, was ( true );
}

ImVec2 back_renderTargetAreaPos;
ImVec2 back_renderTargetAreaSize;
int backup_pickedObject = -1;
int backup_gridentity = -1;
int backup_gridentityobj = -1;
int back_iLastResolutionWidth = 0;
int back_iLastResolutionHeight = 0;
bool bFakeStandaloneTest = false;
int iTriggerGrassTreeUpdate = 0;
int iMaxZeroHeight = 0;
bool commonexecutable_loop_for_game(void)
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	//PE: Support delayed terrain update in standalone.
	extern int iTriggerInvalidateAfterFrames;

	if (iBlockRenderingForFrames > 0)
	{
		extern bool g_bNoSwapchainPresent;
		iBlockRenderingForFrames--;
		if (iBlockRenderingForFrames <= 0)
			g_bNoSwapchainPresent = false;
		else
			g_bNoSwapchainPresent = true;
	}

	if (iTriggerGrassTreeUpdate > 0)
	{
		if (iTriggerGrassTreeUpdate == 5)
		{
			float height = 0;
			if (t.visuals.bEnableEmptyLevelMode)
			{
				//PE: Just give it 5 frame on empty levels.
				if (iMaxZeroHeight++ < 6)
				{
					iTriggerGrassTreeUpdate++;
				}
				else
				{
					iMaxZeroHeight = 0;
				}
			}
			else
			{
				if (!GGTerrain_GetHeight(CameraPositionX(), CameraPositionZ(), &height, 0, 0))
				{
					//Height not available in this are ?.
					iTriggerGrassTreeUpdate++;
				}
			}
		}
		if (iTriggerGrassTreeUpdate == 1)
		{
			ggterrain_extra_params.iUpdateGrass = 1;
			ggterrain_extra_params.iUpdateTrees = 1;
			//PE: Also update probe after terrain is done.
			extern bool g_bLightProbeScaleChanged;
			g_bLightProbeScaleChanged = true;
		}
		iTriggerGrassTreeUpdate--;
	}
	if (iTriggerInvalidateAfterFrames > 0)
	{
		if (iTriggerInvalidateAfterFrames == 1)
		{
			//PE: Height should be availble now.
			//PE: Also update grass and tree, i have some levels where grass is floating in air.
			iTriggerGrassTreeUpdate = 20;
		}
		if (iTriggerInvalidateAfterFrames == 10)
		{
			//PE: Need to do this delayed so terrain update have been updated with new data.
			//PE: Invalidate everything so custom sculpt data is updated with textures ...
			//PE: @Paul not sure if there is a better way to do this, but this works :)
			GGTerrain::GGTerrain_InvalidateRegion(-1000000.0, -1000000.0, 1000000.0, 1000000.0, GGTERRAIN_INVALIDATE_ALL);
		}
		iTriggerInvalidateAfterFrames--;
	}

	// called from both mapeditor(test game) and standalone game
	if (bTriggerFovUpdate)
	{
		float fUsedFOV = t.visuals.CameraFOV_f;
		if (bImGuiInTestGame==false) fUsedFOV = 45;
		bTriggerFovUpdate = false;
		float fCameraFov = XM_PI / ((fUsedFOV) / 15.0f); //Fit GG settings.
		if (bImGuiInTestGame == true)
		{
			fCameraFov = GGToRadian(fUsedFOV); // Oops - backwards logic, lower FOV needs lower angle passed in
		}
		wiScene::GetCamera().CreatePerspective((float)master.masterrenderer.GetLogicalWidth(), (float)master.masterrenderer.GetLogicalHeight(), t.visuals.CameraNEAR_f, t.visuals.CameraFAR_f, fCameraFov);
		wiScene::GetCamera().SetDirty(true);
	}

	void RenderPreviewEmitter(void);
	RenderPreviewEmitter();

	if (iLaunchAfterSync == 201)
	{
		//As we set 201 launch testgame after we have a imgui frame, we need to set Scissors here.
		bImGuiInTestGame = true;
		g_bDisableQuitFlag = true;

		//PE: Use the actual screen resolution, not the windows size.
		extern ImVec2 fImGuiScissorTopLeft;
		extern ImVec2 fImGuiScissorBottomRight;
		fImGuiScissorTopLeft = { 0, 0 };
		fImGuiScissorBottomRight = { (float)GetSystemMetrics(SM_CXSCREEN),  (float)GetSystemMetrics(SM_CYSCREEN) };

		//PE: Used by LUA.
		extern DWORD g_dwScreenWidth;
		extern DWORD g_dwScreenHeight;
		g_dwScreenWidth = fImGuiScissorBottomRight.x;
		g_dwScreenHeight = fImGuiScissorBottomRight.y;
		g_pGlob->iScreenWidth = fImGuiScissorBottomRight.x;
		g_pGlob->iScreenHeight = fImGuiScissorBottomRight.y;

		//PE: Change resolution in wicked.
		if (wiRenderer::GetDevice() != nullptr)
		{
			int width = fImGuiScissorBottomRight.x;
			int height = fImGuiScissorBottomRight.y;
			float fNearDistance = DEFAULT_NEAR_PLANE;
			float fFarDistance = DEFAULT_FAR_PLANE;
		}

		g_bDisableQuitFlag = false;
		t.postprocessings.fadeinvalue_f = 0.0f; //PE: Make sure we trigger default settings like music / volume ...

		WickedCall_DisplayCubes(false); //PE: Hide terrain tool cubes.
		wiProfiler::SetEnabled(false); //PE: Clear stat for a fresh testgame or standalone.

		//LB: need to hide shooter genre debug here as UI still shows them even AFTER the preview_init has been called!
		// hide visual logic dots and arcs
		extern bool g_bDotsAreVisible;
		if (g_bDotsAreVisible)
		{
			//DrawCharacterDots(false);
			DrawLogicNodes(false);
			g_bDotsAreVisible = false;
		}

		if (bStartInvulnerableMode)
		{
			bStartInvulnerableMode = false;
			bInvulnerableMode = true;
		}

		iLaunchAfterSync = 202;
		return true;
	}
	extern bool bSpecialStandalone;
	if (bSpecialStandalone || t.game.gameisexe == 1)
	{
		if (iLaunchAfterSync == 799 || iLaunchAfterSync == 699)
		{
			iLaunchAfterSync = 202;
			void AddRemoteProjectFonts(void);
			AddRemoteProjectFonts();
		}
	}
	if (iLaunchAfterSync == 202)
	{
		int iGridObj = g.ebeobjectbankoffset + 1000;
		if (ObjectExist(iGridObj))
			DeleteObject(iGridObj);
		extern uint32_t PreviewWPERoot;
		if (PreviewWPERoot != 0)
		{
			//PE: Delete effects.
			void DeleteEmitterEffects(uint32_t root);
			DeleteEmitterEffects(PreviewWPERoot);
			PreviewWPERoot = 0;
		}

		bImGuiInTestGame = true;
		g_bDisableQuitFlag = true;
		extern bool	g_bDrawSpritesFirst;
		extern bool bMainLoopRunning;
		if (g_bDrawSpritesFirst)
		{
			UpdateSprites();
		}
		bMainLoopRunning = false;
		if (editor_previewmap_loopcode(0) == true)
		{
			// when loop ends, run code after loop
			bImGuiInTestGame = false;
			bBlockImGuiUntilNewFrame = true;
			bFakeStandaloneTest = false;
			bRenderNextFrame = false;

			wiProfiler::SetEnabled(false); //PE: Clear stat.
			if (bProfilerEnable)
			{
				wiProfiler::SetEnabled(true);
			}
			editor_previewmap_afterloopcode(0);
			mapeditorexecutable_loop_leavetestgame();

			// mapeditor or standalone game
			if (t.game.gameisexe == 1)
			{
				// trigger exit from game
				iLaunchAfterSync = 0;
				PostQuitMessage(0);
			}
			else
			{
				// map editor restore
				//Restore resolution and scissor
				float fNearDistance = DEFAULT_NEAR_PLANE;
				float fFarDistance = DEFAULT_FAR_PLANE; //PE: Default editor camera range.
				extern ImVec2 fImGuiScissorTopLeft;
				extern ImVec2 fImGuiScissorBottomRight;
				fImGuiScissorTopLeft = back_renderTargetAreaPos;
				fImGuiScissorBottomRight = back_renderTargetAreaPos + back_renderTargetAreaSize;
				g_pGlob->iScreenWidth = back_iLastResolutionWidth;
				g_pGlob->iScreenHeight = back_iLastResolutionHeight;
				if(backup_pickedObject != -1)
					t.widget.pickedObject = backup_pickedObject;
				if(backup_gridentity != -1)
					backup_gridentity = t.gridentity;
				if (backup_gridentityobj != -1)
					backup_gridentityobj = t.gridentityobj;

				if(!t.showeditorelements) editor_toggle_element_vis((bool)t.showeditorelements);

				// continue
				iSkibFramesBeforeLaunch = 2;
				iLaunchAfterSync = 203;
			}

			//PE: Reset if we have any hanging keys from test game.
			ImGuiIO& io = ImGui::GetIO();
			io.KeySuper = false;
			io.KeyCtrl = false;
			io.KeyAlt = false;
			io.KeyShift = false;
			for (int iTemp = 0; iTemp < 256; iTemp++)
			{
				io.KeysDown[iTemp] = 0;
			}
			io.MouseDown[0] = 0; //PE: Mouse (release) is also loast inside blocking dialogs. Reset!
			io.MouseDown[1] = 0;
			io.MouseDown[2] = 0;
			io.MouseDown[3] = 0;

		}
		g_bDisableQuitFlag = false;
		return true;
	}

	// allow continue on to editor if appropriate
	return false;
}

void gridedit_setsmartobjectvisibilityinrubberband(bool bVisible)
{
	if (g.entityrubberbandlist.size() > 0)
	{
		for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
		{
			int e = g.entityrubberbandlist[i].e;
			if (t.entityprofile[t.entityelement[e].bankindex].ischildofgroup != 0)
			{
				if (t.entityprofile[t.entityelement[e].bankindex].ismarker != 0)
				{
					if (bVisible)
						ShowObject(t.entityelement[e].obj);
					else
						HideObject(t.entityelement[e].obj);
				}
			}
		}
	}
}

void SetGlobalGraphicsSettings( int level ) // 0=lowest, 1=medium, 2=high, 3=ultra, default to 2 (high)
{
	GGTerrain::GGTerrain_SetPerformanceMode( level );
	GGTrees::GGTrees_SetPerformanceMode( level );
	GGGrass::GGGrass_SetPerformanceMode( level );

	switch( level )
	{
		case 0: // low
		{
			t.visuals.bSSREnabled = false;
			t.visuals.bFXAAEnabled = false;
			t.visuals.bLightShafts = false;
			t.visuals.bLensFlare = false;
			t.visuals.bReflectionsEnabled = false;
			t.visuals.iShadowSpotCascadeResolution = 512;
			t.visuals.iShadowPointMax = 2;
			t.visuals.iShadowPointResolution = 256;
			t.visuals.iShadowSpotMax = 1;
			t.visuals.iShadowSpotResolution = 256;
		} break;

		case 1: // medium
		{
			t.visuals.bSSREnabled = false;
			t.visuals.bFXAAEnabled = true;
			t.visuals.bLightShafts = true;
			t.visuals.bLensFlare = true;
			t.visuals.bReflectionsEnabled = true;
			t.visuals.iShadowSpotCascadeResolution = 1024;
			t.visuals.iShadowPointMax = 4;
			t.visuals.iShadowPointResolution = 512;
			t.visuals.iShadowSpotMax = 4;
			t.visuals.iShadowSpotResolution = 512;
		} break;

		case 2: // high
		{
			t.visuals.bSSREnabled = false;
			t.visuals.bFXAAEnabled = true;
			t.visuals.bLightShafts = true;
			t.visuals.bLensFlare = true;
			t.visuals.bReflectionsEnabled = true;
			t.visuals.iShadowSpotCascadeResolution = 1024;
			t.visuals.iShadowPointMax = 12;
			t.visuals.iShadowPointResolution = 512;
			t.visuals.iShadowSpotMax = 8;
			t.visuals.iShadowSpotResolution = 512;
		} break;

		case 3: // ultra
		{
			t.visuals.bSSREnabled = false;
			t.visuals.bFXAAEnabled = true;
			t.visuals.bLightShafts = true;
			t.visuals.bLensFlare = true;
			t.visuals.bReflectionsEnabled = true;
			t.visuals.iShadowSpotCascadeResolution = 2048;
			t.visuals.iShadowPointMax = 16;
			t.visuals.iShadowPointResolution = 512;
			t.visuals.iShadowSpotMax = 8;
			t.visuals.iShadowSpotResolution = 512;
		} break;
	}
	Wicked_Update_Visuals( &t.visuals );
}

void mapeditorexecutable_full_folder_refresh(void)
{
	// work out the project folder path for third location of assets
	static char cFullProjectWritePath[MAX_PATH];
	static char cFullWritePath[MAX_PATH];

	// only update folders and files if flagged
	if (!bExternal_Entities_Init)
	{
		// do entities init once when flagged
		bExternal_Entities_Init = true;

		// eventually ensure that the tree view in process_entity_library_v2 will be updated so the user can see any new folders they created.
		extern bool bTreeViewInitInNextFrame;
		bTreeViewInitInNextFrame = true;

		// Go through all media folders
		cstr pOld = GetDir();
		for (int iMediaFolderType = 0; iMediaFolderType <= 6; iMediaFolderType++)
		{
			// folders to check
			LPSTR pMediaFolderPattern = "";
			if (iMediaFolderType == 0) pMediaFolderPattern = "entitybank";
			if (iMediaFolderType == 1) pMediaFolderPattern = "audiobank";
			if (iMediaFolderType == 2) pMediaFolderPattern = "imagebank";
			if (iMediaFolderType == 3) pMediaFolderPattern = "videobank";
			if (iMediaFolderType == 4) pMediaFolderPattern = "scriptbank";
			if (iMediaFolderType == 5) pMediaFolderPattern = "particlesbank";
			if (iMediaFolderType == 6) pMediaFolderPattern = "charactercreatorplus\\animations";
			
			// use GetMainEntityList to add root, writables and project folder files
			strcpy(cFullWritePath, pMediaFolderPattern);
			GG_GetRealPath(cFullWritePath, 1);
			cStr CurrentPath = pOld + cStr("\\") + cStr(pMediaFolderPattern);
			if (strnicmp(CurrentPath.Get(), cFullWritePath, CurrentPath.Len()) == 0)
			{
				// same folder means no separate writables area, i.e. GG_GetRealPath create mode failed
				// only root folder for non-writable systems
				GetMainEntityList(pMediaFolderPattern, "", NULL, "", true, iMediaFolderType);
			}
			else
			{
				// tracks folder creations
				cFolderItem* pLastFolder = &MainEntityList;
				cFolderItem* pFirstOfTheLastFolder = NULL;

				// writables folder
				GetMainEntityList(cFullWritePath, "", pFirstOfTheLastFolder, "w:", true, iMediaFolderType);
				pLastFolder = &MainEntityList;
				while (pLastFolder->m_pNext)
				{
					pLastFolder = pLastFolder->m_pNext;
				}

				// root folder
				SetDir(pOld.Get());
				GetMainEntityList(pMediaFolderPattern, "", pLastFolder, "", false, iMediaFolderType);

				extern char szBeforeChangeWriteDir[MAX_PATH];
				extern bool bIncludeDocumentFolderInRemoteProject;
				//PE: Normal writefolder if remote project.
				if(bIncludeDocumentFolderInRemoteProject && strlen(szBeforeChangeWriteDir) > 0)
				{
					char newpath[MAX_PATH];
					strcpy(newpath, szBeforeChangeWriteDir);
					strcat(newpath, "Files");
					SetDir(newpath);
					GetMainEntityList(pMediaFolderPattern, "", pLastFolder, "", false, iMediaFolderType);
				}

				SetDir(pOld.Get());

			}
		}

		//Sort folder entrys.
		SetDir(pOld.Get());
		cFolderItem *pNewFolder = (cFolderItem *)&MainEntityList;
		cFolderItem *m_pfirstFolder = NULL;
		int mc = 0;
		while (pNewFolder->m_pNext)
		{
			if (!m_pfirstFolder) m_pfirstFolder = pNewFolder->m_pNext;
			pNewFolder = pNewFolder->m_pNext;
			mc++;
		}
		if (mc > 1)
		{
			//#### SORT ####
			char ** cptr = new char *[mc + 1];
			cFolderItem *m_pSortFolder = m_pfirstFolder->m_pNext;
			int mc2 = 0;
			for (int a = 0; a < mc; a++)
			{
				if (m_pSortFolder)
				{
					cptr[a] = (char *)m_pSortFolder;
					m_pSortFolder = m_pSortFolder->m_pNext;
					mc2++;
				}
			}
			qsort(cptr, mc2, sizeof(cptr[0]), cstring_cmp_folder);
			m_pSortFolder = m_pfirstFolder->m_pNext;
			m_pfirstFolder->m_pNext = (cFolderItem *)cptr[0];
			for (int a = 0; a < mc2; a++)
			{
				m_pSortFolder = (cFolderItem *)cptr[a];
				if (m_pSortFolder)
				{
					if (a + 1 < mc2) m_pSortFolder->m_pNext = (cFolderItem *)cptr[a + 1];
				}
			}
			delete[] cptr;
			if (m_pSortFolder) m_pSortFolder->m_pNext = NULL;
		}
	}

	static bool bScan_Files_List = true;
	static int bScan_Files_Start = 50; //Wait some frames before we start , so terrain ... can get a head start.

	extern int g_iScannedFiles;
	extern std::vector<cFolderItem::sFolderFiles *> g_ScanFpeFiles;
	extern bool g_bFpeScanning;

	if (bExternal_Entities_Init && bScan_Files_List && bScan_Files_Start-- <= 0)
	{
		cFolderItem *pSearchFolder = &MainEntityList;
		pSearchFolder = pSearchFolder->m_pNext;
		while (pSearchFolder) {
			if (pSearchFolder->m_pFirstFile) {
				cFolderItem::sFolderFiles * searchfiles = pSearchFolder->m_pFirstFile->m_pNext;
				while (searchfiles) {
					//PE: For now only scan fpe, we might need additional info about other file types but ...
					if (searchfiles->iType == 0)
					{
						g_ScanFpeFiles.push_back(searchfiles);
					}
					searchfiles = searchfiles->m_pNext;
				}
			}
			pSearchFolder = pSearchFolder->m_pNext;
		}
		fpe_thread_start();
		bScan_Files_List = false;
	}
}

void launchOrShowEditorCore ( char* pFolderName, char* pWindowNameA, char* pWindowNameB, char* pAppName )
{
	LPSTR pEditorWindowTitle = pWindowNameA;
	if (WindowExist(pEditorWindowTitle) == 0)
	{
		pEditorWindowTitle = pWindowNameB;
		if (WindowExist(pEditorWindowTitle) == 0)
		{
			// not here, launch it!
			char pOldDir[MAX_PATH];
			strcpy(pOldDir, GetDir());
			SetDir("..");
			SetDir("Tools\\");
			SetDir(pFolderName);
			ExecuteFile(pAppName, "", "");
			SetDir(pOldDir);
		}
		else
		{
			// found you!
			WindowToFront(pEditorWindowTitle);
		}
	}
	else
	{
		// found you!
		WindowToFront(pEditorWindowTitle);
	}
}

void launchOrShowParticleEditor(void)
{
	launchOrShowEditorCore("Particle Editor","Particle Editor 0.7b","Particle Editor","particle_editor.exe");
}

void launchOrShowBuildingEditor(void)
{
	launchOrShowEditorCore("Building Editor", "GameGuru Building Editor", "GameGuru Building Editor*", "GameGuru Building Editor.exe");
}

void mapeditorexecutable_loop(void)
{
	// the moment storyboard is used, we can load the rest of the common assets needed for editor and game
	if (bStoryboardWindow == true || pref.iDisplayWelcomeScreen == 0)
	{
		// Generic asset loading common to editor and game that can be deferred until user opens a project for first time
		if (g_bCommonAssetsLoadOnce == true)
		{
			t.tsplashstatusprogress_s = "LOAD DELAYED COMMON ASSETS";
			timestampactivity(0, t.tsplashstatusprogress_s.Get());
			version_splashtext_statusupdate ();
			::SetCursor(::LoadCursor(NULL, IDC_WAIT));
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
			bKeepWindowsResponding = true;

			common_loadcommonassets_delayed (0);

			bKeepWindowsResponding = false;
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
			::SetCursor(::LoadCursor(NULL, IDC_ARROW));

			g_bCommonAssetsLoadOnce = false;
		}
	}

	if (g_iCountdownToAlphaBetaMessage > 0)
	{
		g_iCountdownToAlphaBetaMessage--;
		if (g_iCountdownToAlphaBetaMessage == 0)
		{
			// great time to check for any new updates!
			common_autoupdatecheck();
		}
	}

	if (iUpdateOcean > 0)
	{
		iUpdateOcean--;
		if ( iUpdateOcean == 0 )
		{
			extern wiECS::Entity g_weatherEntityID;
			wiScene::WeatherComponent* weather = wiScene::GetScene().weathers.GetComponent(g_weatherEntityID);
			wiScene::GetScene().OceanRegenerate();
		}
	}

	static int iCheckForMinimized = 0;
	static bool bInMinimizeMode = false;
	//WS_MINIMIZE
	if (bInMinimizeMode)
	{
		Sleep(250); //Set Speed to 4 fps, so we have some reactions in other windows outside the main window.
		LONG lWinStyle = GetWindowLong(g_pGlob->hWnd, GWL_STYLE);
		if (!(lWinStyle & WS_MINIMIZE))
			bInMinimizeMode = false;
	}
	if (iCheckForMinimized++ >= 180) //PE: Evry 3 sec.
	{
		LONG lWinStyle = GetWindowLong(g_pGlob->hWnd,GWL_STYLE);
		if (lWinStyle & WS_MINIMIZE)
			bInMinimizeMode = true;
		iCheckForMinimized = 0;
	}

	//PE: Change icon set here where we have no imgui images on screen.
	SetIconSetCheck(false);

	bSmallVideoFrameStart = true;
	// special modes used when in test game or standalone game
	if (commonexecutable_loop_for_game() == true) return;

	//PE: Some function require we have a empty imgui , so launch here.
	extern bool g_bNoSwapchainPresent;

	bLastSmallVideoPlayerMaximized = bSmallVideoPlayerMaximized; //We need to status one frame behind.
	bSmallVideoPlayerMaximized = false;

	// can update collection list with flag
	if (g_bUpdateCollectionList == true)
	{
		//PE: Problem , open a level outside this project , select a weapon. and it get saved in the current project.
		//PE: https://youtu.be/LXyva4_786Y
		//PE: Must validate that this level belong to the active project.

		bool bPartOfStoreBoard = true; //PE: Running without a project.
		if (strlen(Storyboard.gamename) > 0)
		{
			bPartOfStoreBoard = false; //PE: Running using a project.
			char currentlevel[MAX_PATH];
			strcpy(currentlevel, g.projectfilename_s.Get());
			char* find = (char *) pestrcasestr(&currentlevel[0], "mapbank\\");
			if (find)
			{
				strcpy(currentlevel, find + 8);
			}
			for (int i = 0; i < STORYBOARD_MAXNODES; i++)
			{
				if (Storyboard.Nodes[i].used && Storyboard.Nodes[i].type == STORYBOARD_TYPE_LEVEL)
				{
					// get level name
					if (strlen(Storyboard.Nodes[i].level_name) > 0)
					{
						char levelname[MAX_PATH];
						strcpy(levelname, Storyboard.Nodes[i].level_name);
						char* find = (char*)pestrcasestr(&levelname[0], "mapbank\\");
						if (find)
						{
							strcpy(levelname, find + 8);
						}
						if (pestrcasestr(currentlevel, levelname))
						{
							bPartOfStoreBoard = true;
							break;
						}
					}
				}
			}
		}
		if (bPartOfStoreBoard)
		{
			bool bLoadingLevel = false;
			refresh_collection_from_entities(bLoadingLevel);
		}
		g_bUpdateCollectionList = false;
	}

	//PE: Moved load here. so no imgui objects on screen.
	//PE: Some imgui used ShaderResourceViews seams to change while loading a new level in wicked.
	if (iSkibFramesBeforeLaunch == 0) 
	{
		switch (iLaunchAfterSync)
		{
			case 699:
			{
				iLaunchAfterSync = 0;
				void AddRemoteProjectFonts(void);
				AddRemoteProjectFonts();
				break;
			}
			case 698:
			{
				//PE: Reload all fonts.
				iLaunchAfterSync = 0;
				void AddRemoteProjectFonts(void);
				ChangeGGFont("editors\\uiv3\\Roboto-Medium.ttf", 15);
				AddRemoteProjectFonts();
				break;
			}
			case 203: //PE: trigger a WM_SIZE so resolution,scissor,targetarea all match.
			{
				iLaunchAfterSync = 0;
				if (gWindowMaximized == 1) 
				{
					ShowWindow(g_pGlob->hWnd, SW_MAXIMIZE);
				}
				else 
				{
					SetWindowPos(g_pGlob->hWnd, HWND_TOP, gWindowPosXOld, gWindowPosYOld, gWindowSizeXOld + gWindowSizeAddX, gWindowSizeYOld + gWindowSizeAddY, SWP_SHOWWINDOW);
					ShowWindow(g_pGlob->hWnd, SW_SHOWNORMAL);
				}
				bUpdateVeg = true;
				bTriggerFovUpdate = true;
				break;
			}
			case 502: //Do the actual level load.
			{

				::SetCursor(::LoadCursor(NULL, IDC_WAIT));
				ImGuiIO& io = ImGui::GetIO();
				io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

				bKeepWindowsResponding = true;

				g.projectfilename_s = sNextLevelToLoad;
			
				extern bool g_bAllowBackwardCompatibleConversion;
				g_bAllowBackwardCompatibleConversion = true;
				gridedit_load_map();
				g_bAllowBackwardCompatibleConversion = false;

				g_EntityClipboard.clear(); //PE: Clear any old copy/paste.
				undosys_clearall(); //PE: Clear undo redo system.


				if(!bCloseStoryboardAfterLoad)
					iLevelEditorFromStoryboardID = -1; //If loaded from here, we cant update storyboard.

				t.terrain.grassregionx1 = t.terrain.grassregionx2;
				bUpdateVeg = true;

				iLaunchAfterSync = 80; //Update env
				iSkibFramesBeforeLaunch = 5;

				int firstempty = -1;
				int i = 0;
				for (; i < REMEMBERLASTFILES; i++) {
					if (firstempty == -1 && strlen(pref.last_open_files[i]) <= 0)
						firstempty = i;

					if (strlen(pref.last_open_files[i]) > 0 && pestrcasestr(g.projectfilename_s.Get(), pref.last_open_files[i])) { //already there
						break;
					}
				}
				if (i >= REMEMBERLASTFILES) {
					if (firstempty == -1) {
						//No empty slots , rotate.
						for (int ii = 0; ii < REMEMBERLASTFILES - 1; ii++) {
							strcpy(pref.last_open_files[ii], pref.last_open_files[ii + 1]);
						}
						strcpy(pref.last_open_files[REMEMBERLASTFILES - 1], g.projectfilename_s.Get());
					}
					else
						strcpy(pref.last_open_files[firstempty], g.projectfilename_s.Get());
				}

				//Locate player start marker.
				for (t.e = 1; t.e <= g.entityelementlist; t.e++)
				{
					if (t.entityelement[t.e].bankindex > 0)
					{
						if (t.entityprofile[t.entityelement[t.e].bankindex].ismarker == 1 && t.entityprofile[t.entityelement[t.e].bankindex].lives != -1)
						{
							//Point camera.
							t.obj = t.entityelement[t.e].obj;
							if (t.obj > 0) {
								float offsetx = ((float)GetDesktopWidth() - renderTargetAreaSize.x) * 0.25f;
								t.cx_f = ObjectPositionX(t.obj) + offsetx; //t.editorfreeflight.c.x_f;
								t.cy_f = ObjectPositionZ(t.obj); //t.editorfreeflight.c.z_f;
							}
							break;
						}
					}
				}

				iLastUpdateVeg = 0;
				bUpdateVeg = true;
				extern int g_iSuperTriggerFullGrassReveal; // hmm, shoved in to get the damn grass showing on initial load!
				g_iSuperTriggerFullGrassReveal = 10;

				//PE: Always start in object mode.
				bForceKey = true;
				csForceKey = "o";
				bTerrain_Tools_Window = false;
				Entity_Tools_Window = true;

				bKeepWindowsResponding = false;
				io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
				::SetCursor(::LoadCursor(NULL, IDC_ARROW));

				break;
			}
			case 2: //Open
				GGTerrain_CancelRamp();
				iLaunchAfterSync = 0;
				int iRet;
				iRet = AskSaveBeforeNewAction();
				if (iRet != 2)
				{
					//PE: filedialogs change dir so.
					cStr tOldDir = GetDir();

					// we know we need to focus on the mapbank associated with the current storyboard
					cstr correctFPMLocation_s = Storyboard.customprojectfolder;
					if (correctFPMLocation_s.Len() > 0)
					{
						correctFPMLocation_s += Storyboard.gamename;
						correctFPMLocation_s += "\\Files\\mapbank";
					}
					else
					{
						correctFPMLocation_s = g.mysystem.mapbankAbs_s.Get();
					}

					//cFileSelected = (char*)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "fpm\0*.fpm\0", g.mysystem.mapbankAbs_s.Get(), NULL, true);
					char* cFileSelected = (char*)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "fpm\0*.fpm\0", correctFPMLocation_s.Get(), NULL, true);
					SetDir(tOldDir.Get());
					if (cFileSelected && strlen(cFileSelected) > 0)
					{
						t.returnstring_s = cFileSelected;
						if (t.returnstring_s != "")
						{
							if (cstr(Lower(Right(t.returnstring_s.Get(), 4))) == ".fpm")
							{
								t.gridentity = 0;
								t.inputsys.constructselection = 0;
								editor_refresheditmarkers();
								g.projectfilename_s = t.returnstring_s;
							
								//Load actual level.
								sNextLevelToLoad = t.returnstring_s;
								iLaunchAfterSync = 502; 
								iSkibFramesBeforeLaunch = 3;
								strcpy(cTriggerMessage, "Loading Level ...");
								bTriggerMessage = true;
							}
						}
					}
				}
				// hmm, shoved in to get the damn grass showing on initial load!
				extern int g_iSuperTriggerFullGrassReveal; 
				g_iSuperTriggerFullGrassReveal = 10;
				iLastUpdateVeg = 0;
				break;

			case 7: // Direct Open
			{
				GGTerrain_CancelRamp();
				iLaunchAfterSync = 0;
				int iRet;
				iRet = AskSaveBeforeNewAction();
				if (iRet != 2)
				{
					if (strlen(cDirectOpen) > 0) {

						t.returnstring_s = cDirectOpen;
						if (t.returnstring_s != "")
						{
							if (cstr(Lower(Right(t.returnstring_s.Get(), 4))) == ".fpm")
							{
								t.gridentity = 0;
								t.inputsys.constructselection = 0;
								editor_refresheditmarkers();
								g.projectfilename_s = t.returnstring_s;

								//Load actual level.
								sNextLevelToLoad = t.returnstring_s;
								iLaunchAfterSync = 502; 
								iSkibFramesBeforeLaunch = 3;
								strcpy(cTriggerMessage, "Loading Level ...");
								bTriggerMessage = true;
							}
						}
					}
				}
				// hmm, shoved in to get the damn grass showing on initial load!
				extern int g_iSuperTriggerFullGrassReveal; 
				g_iSuperTriggerFullGrassReveal = 10;
				iLastUpdateVeg = 0;
				break;
			}
		}
	}

	switch(iLaunchAfterSync)
	{
		case 1:  // Test Game
		case 20: // VR Test Game

			if (t.game.gameisexe == 0)
			{
				//PE: Backup so we can restore last selected object after testgame.
				backup_pickedObject = t.widget.pickedObject;
				backup_gridentity = t.gridentity;
				backup_gridentityobj = t.gridentityobj;
			}
			else
			{
				backup_pickedObject = -1;
				backup_gridentity = -1;
				backup_gridentityobj = -1;
			}

			GGTerrain_CancelRamp();
			bInvulnerableMode = false;
			if (bStartInvulnerableMode)
			{
				bStartInvulnerableMode = false;
				bInvulnerableMode = true;
			}

			if (iLaunchAfterSync == 20 && g.gvrmode == 0)
			{
			}
			else
			{
				int iGridObj = g.ebeobjectbankoffset + 1000;
				if (ObjectExist(iGridObj))
					DeleteObject(iGridObj);
				extern uint32_t PreviewWPERoot;
				if (PreviewWPERoot != 0)
				{
					//PE: Delete effects.
					void DeleteEmitterEffects(uint32_t root);
					DeleteEmitterEffects(PreviewWPERoot);
					PreviewWPERoot = 0;
				}

				bImGuiInTestGame = true;
				bool bTestInVRMode = false;
				if (iLaunchAfterSync == 20) bTestInVRMode = true;
				extern int iLastResolutionWidth;
				extern int iLastResolutionHeight;
				back_iLastResolutionWidth = iLastResolutionWidth;
				back_iLastResolutionHeight = iLastResolutionHeight;
				back_renderTargetAreaPos = renderTargetAreaPos;
				back_renderTargetAreaSize = renderTargetAreaSize;
				RunCode(0); //switch to backbuffer 
				// g.projectmodified = 1; if just testing, do not assume a modification!
				// ensure threads loading resources are silent before test game
				image_preload_files_wait();
				object_preload_files_wait();
				image_preload_files_reset();
				object_preload_files_reset();
				if ( bTestInVRMode == false )
				{
					editor_previewmap_initcode(0);
					iLaunchAfterSync = 201;
				}
				else
				{
					editor_previewmap_initcode(1);
					iLaunchAfterSync = 201;
				}
				break;
			}
			break;

		case 30: //Create thumbnail.
		{
			if (iTooltipLastObjectId > 0) 
			{
				int drawobj = g.entitybankoffset + iTooltipLastObjectId;
				g_bNoSwapchainPresent = true; //dont present backbuffer to HWND.
			}
			iLaunchAfterSync = 31;
			break;
		}
		case 31: //Switch back to presenting the swapchain.
		{
			g_bNoSwapchainPresent = false;
			iLaunchAfterSync = 0;
			break;
		}

		case 21: //Social VR.
		{
			extern uint32_t PreviewWPERoot;
			if (PreviewWPERoot != 0)
			{
				//PE: Delete effects.
				void DeleteEmitterEffects(uint32_t root);
				DeleteEmitterEffects(PreviewWPERoot);
				PreviewWPERoot = 0;
			}
			bImGuiInTestGame = true;
			RunCode(0); //switch to backbuffer
			editor_multiplayermode();
			bBlockImGuiUntilNewFrame = true;
			bRenderNextFrame = false;
			SetCameraToImage(0, g.postprocessimageoffset, GetDisplayWidth(), GetDisplayHeight(), 2); //switch back to render target.
			bImGuiInTestGame = false;
			iLaunchAfterSync = 0;
			sky_show(); //Restore skybox.
			iLastUpdateVeg = 0; //Veg: update any changes from F9
			bUpdateVeg = true;
			break;
		}

		default:
			break;
	}

	//Display Weather.
	extern bool bEnableWeather;
	if (bEnableWeather) 
	{
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

	}

	//PE: Imgui variables.
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//PE: To solve problem with ALT-TAB away and key "release" are sent to another app.
	HWND tmpHwnd = GetFocus();
	if (bLostFocus == false && tmpHwnd != g_pGlob->hWnd) 
	{
		bLostFocus = true;
		io.KeySuper = false;
		io.KeyCtrl = false;
		io.KeyAlt = false;
		io.KeyShift = false;

		t.inputsys.keyreturn = 0;
		t.inputsys.keyshift = 0;
		t.inputsys.keytab = 0;
		t.inputsys.keyleft = 0;
		t.inputsys.keyright = 0;
		t.inputsys.keyup = 0;
		t.inputsys.keydown = 0;
		t.inputsys.keycontrol = 0;
		t.inputsys.keyspace = 0;
		t.inputsys.kscancode = 0;

		for (int iTemp = 0; iTemp < 273; iTemp++)
			io.KeysDown[iTemp] = 0;
	}
	if (tmpHwnd == g_pGlob->hWnd) 
	{
		if (bLostFocus) 
		{
			//We got focus again.
			//Looks like its fine to reset keys on lost only.
			//If there is a problem, they can also be reset here.
		}
		bLostFocus = false;
	}

	// set when showonstartup.ini does not exist and is created (first run sorts out UI panels)
	// and it seems, fixes the issue of 'bImGuiGotFocus' being true on some laptops on first run!?
	if (g.gfirsttimerun == 1)
	{
		g.gfirsttimerun = 0;
		refresh_gui_docking = 0;
		pref.vStartResolution = { 1280,800 };
		pref.iMaximized = 1;
	}

	//PE: ImGuiWindowFlags_NoNav added to prevent cursor keys to navigate imgui.
	if(pref.iAllowUndocking)
		iGenralWindowsFlags = ImGuiWindowFlags_None | ImGuiWindowFlags_NoNav;
	else
		iGenralWindowsFlags = ImGuiWindowFlags_None | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav;

	ImVec4 style_back = ImGui::GetStyle().Colors[ImGuiCol_Text];
	ImVec4 style_winback = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
	style_back = ImVec4(1.0, 1.0, 1.0, 1.0);

	bBoostIconColors = false;

	if (pref.current_style == 3) 
	{
		drawCol_back = ImColor(255, 255, 255, 128)*style_back;
		drawCol_normal = ImColor(255, 255, 255, 255);
		drawCol_hover = ImColor(180, 180, 180, 230);
	}
	else if (pref.current_style == 25) 
	{
		drawCol_back = ImColor(255, 255, 255, 128)*style_back;
		drawCol_normal = ImColor(255, 255, 255, 255);
		drawCol_hover = ImColor(180, 180, 180, 230);
		bBoostIconColors = true;
	}
	else 
	{
		drawCol_back = ImColor(255, 255, 255, 128)*style_back;
		drawCol_normal = ImColor(220, 220, 220, 230)*style_back;
		drawCol_hover = ImColor(255, 255, 255, 255)*style_back;
	}
	drawCol_Down = ImColor(255, 255, 255, 255)*style_back;
	ImVec4 drawCol_active = ImColor(120, 220, 120, 220)*style_back;
	ImVec4 drawCol_tmp = ImColor(220, 220, 220, 220)*style_back;
	ImVec4 drawCol_header = ImColor(255, 255, 255, 255)*style_back;

	bool toolbar_gradiant = false;
	#ifdef USETOOLBARGRADIENT
	toolbar_gradiant = true;
	#endif

	int adder = 6;
	ImVec4 drawCol_back_test = ImColor(255, 255, 255, adder)*style_back; adder += 6;
	ImVec4 drawCol_back_tools = ImColor(255, 255, 255, adder)*style_back; adder += 6;
	ImVec4 drawCol_back_waypoint = ImColor(255, 255, 255, adder)*style_back; adder += 6;
	ImVec4 drawCol_back_entities = ImColor(255, 255, 255, adder)*style_back; adder += 6;
	ImVec4 drawCol_back_terrain_tools = ImColor(255, 255, 255, adder)*style_back; adder += 6;
	ImVec4 drawCol_back_terrain = ImColor(255, 255, 255, adder)*style_back; adder += 6;
	ImVec4 drawCol_back_gg = ImColor(255, 255, 255, adder)*style_back;
	drawCol_toogle = drawCol_back_gg;
	if (pref.current_style == 25) {
		drawCol_back_test = ImColor(255, 255, 255,0);
		drawCol_back_tools = ImColor(255, 255, 255, 0);
		drawCol_back_waypoint = ImColor(255, 255, 255, 0);
		drawCol_back_entities = ImColor(255, 255, 255, 0);
		drawCol_back_terrain_tools = ImColor(255, 255, 255, 0);
		drawCol_back_terrain = ImColor(255, 255, 255, 0);
		drawCol_back_gg = ImColor(255, 255, 255, 0);
		drawCol_toogle = ImColor(255, 255, 255, 50);
	}

	ImVec4 drawCol_back_active = ImColor(255, 255, 255, 160); //*style_back;
	if (pref.current_style == 25) {
		drawCol_back_active = ImColor(128, 128, 128, 128); //*style_back;
	}


	static bool bLastImGuiGotFocus;
	bLastImGuiGotFocus = bImGuiGotFocus;

	bImGuiGotFocus = false; //PE: Set this if any of the imgui windows got focus.
	bImGuiReadyToRender = false;

	// Start the Dear ImGui frame
	if (!bImGuiFrameState && !bRenderTabTab) // lee added !bRenderTabTab to prevent double newframe
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		extern bool bSpriteWinVisible;
		bSpriteWinVisible = false;
		bImGuiFrameState = true;
		bTutorialRendered = false;
		bBlockImGuiUntilNewFrame = false;
		bImGuiRenderWithNoCustomTextures = false;
	}

	float fontSize = 0.0f;
	ImVec2 mCharAdvance = ImVec2(0, 0);
	int iOldRounding = 0;
	if (bImGuiFrameState) 
	{
		fontSize = ImGui::CalcTextSize("#").x;
		mCharAdvance = ImVec2(fontSize, ImGui::GetTextLineHeightWithSpacing());
	}

	//PE: Additional resets, not done in direct input.
	t.inputsys.doartresize = 0;
	t.inputsys.dosave = 0; t.inputsys.doopen = 0; t.inputsys.donew = 0; t.inputsys.donewflat = 0; t.inputsys.dosaveas = 0;
	if (bImGuiFrameState) 
	{
		// LEELEE disable interaction with main editor if Welcome system is active!
		if (iTriggerWelcomeSystemStuff != 0)
		{
			// LEELEE disable input to IMGUI menu/toolbar/panels while Welcome System is active
			//PE: Displaying it as Modal will disable everything but the welcome system.
			bRenderTargetModalMode = true;
		}
		else
		{
			bRenderTargetModalMode = false;
		}
		int icon_size = 50;
		ImVec2 iToolbarIconSize = { (float)icon_size, (float)icon_size };
		static bool dockingopen = true;
		float fsy = ImGui::CalcTextSize("#").y;
		toolbar_size = icon_size + (fsy*2.0) + 2;
		ImVec2 viewPortPos = ImGui::GetMainViewport()->Pos;
		ImVec2 viewPortSize = ImGui::GetMainViewport()->Size;
		ImGuiStatusBar_Size = fsy * 1.4;
		//PE: Render toolbar.

		iOldRounding = ImGui::GetStyle().WindowRounding;
		ImGui::GetStyle().WindowRounding = 0.0f;
		
		if (bStopBackbufferGrab > 0)
		{
			bStopBackbufferGrab--;
			if (bStopBackbufferGrab == 0)
			{
				if (BitmapExist(99))
				{
					DeleteBitmapEx(99);
				}
				bFullScreenBackbuffer = false;
			}
		}

		//#################
		//#### Toolbar ####
		//#################

		static float toolbar_offset_center = 0;
		int current_mode = 0;

		if (t.grideditselect == 6) 
		{
			extern bool bWaypointDrawmode;
			if(bWaypointDrawmode)
				current_mode = TOOL_DRAWWAYPOINTS;
			else
				current_mode = TOOL_WAYPOINTS;
		}
		else if (t.grideditselect == 5) 
		{
			if (t.gridentitymarkersmodeonly == 0)
				current_mode = TOOL_ENTITY;
			else
				current_mode = TOOL_MARKERS;
		}
		else 
		{
			if (t.terrain.terrainpaintermode >= 1 && t.terrain.terrainpaintermode <= 5)
			{
				if (t.terrain.terrainpaintermode == 1)  current_mode = TOOL_SHAPE;
				if (t.terrain.terrainpaintermode == 2)  current_mode = TOOL_LEVELMODE;
				if (t.terrain.terrainpaintermode == 3)  current_mode = TOOL_STOREDLEVEL;
				if (t.terrain.terrainpaintermode == 4)  current_mode = TOOL_BLENDMODE;
				if (t.terrain.terrainpaintermode == 5)  current_mode = TOOL_RAMPMODE;
			}
			else
			{
				if (t.terrain.terrainpaintermode == 6)  current_mode = TOOL_PAINTTEXTURE;
				if (t.terrain.terrainpaintermode == 7)  current_mode = TOOL_PAINTTEXTURE;
				if (t.terrain.terrainpaintermode == 8)  current_mode = TOOL_PAINTTEXTURE;
				if (t.terrain.terrainpaintermode == 9)  current_mode = TOOL_PAINTTEXTURE;
				if (t.terrain.terrainpaintermode == 10) current_mode = TOOL_PAINTGRASS;
			}
		}

		if(g_bCharacterCreatorPlusActivated)
			current_mode = TOOL_CCP;
		if(bBuilder_Properties_Window || t.ebe.on == 1)
			current_mode = TOOL_BUILDER;
		if (bImporter_Window && t.importer.importerActive == 1)
			current_mode = TOOL_IMPORT;

		if ( t.gridentity > 0 && t.entityprofile[t.gridentity].isebe != 0) 
		{
			current_mode = TOOL_BUILDER;
		}

		bool bOldWelcomeScreen_Window = bWelcomeScreen_Window;

		//PE: Make sure we dont place the toolbar in its own viewport.
		ImGui::SetNextWindowPos(ImVec2(0, 0) + viewPortPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, toolbar_size));

		if (pref.current_style == 25)
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.26f, 0.35f, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.11f, 0.16f, 0.22f, 1.00f)); //org ImVec4(0.58f, 0.58f, 0.58f, 1.00f); // ImGui::PopStyleColor();
		}

		int toolbar_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
		if (bOldWelcomeScreen_Window)
		{
			toolbar_flags |= ImGuiWindowFlags_NoChangeZOrder;
		}
		toolbar_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		ImGui::Begin("Toolbar", NULL , toolbar_flags);

		if (bOldWelcomeScreen_Window)
		{
			//Disable
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		}

		ImVec4 drawCol_Selection = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		float fDiv = 1.0f / 255.0f;
		ImVec4 drawCol_Divider_Selected = ImVec4(fDiv * 177.0f, fDiv * 206.0f, fDiv * 225.0f, 1.0f);
		if (pref.current_style == 25) 
		{
			drawCol_hover = ImVec4(fDiv * 142.0f, fDiv * 184.0f, fDiv * 212.0f, 1.0f);
		}

		ImGui::GetStyle().WindowRounding = iOldRounding;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		float cursorpos = ImGui::GetCursorPos().x;

		//PE: New "Back to Game Project" ? 
		drawCol_tmp = drawCol_back_test; //LB: same background as toogle buttons for consistency
		//if (ImGui::ImgBtn(TOOL_GOBACK, iToolbarIconSize, drawCol_back_gg, drawCol_normal/**drawCol_Selection*/, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, toolbar_gradiant, false, false, false, bBoostIconColors))
		if (ImGui::ImgBtn(TOOL_GOBACK, iToolbarIconSize, drawCol_tmp, drawCol_normal/**drawCol_Selection*/, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, toolbar_gradiant, false, false, false, bBoostIconColors))
		{
			CloseAllOpenToolsThatNeedSave();
	
			if (g.projectmodified == 1 && g.projectfilename_s != "")
			{
				int iRet = askBoxCancel("Do you wish to save first?", "Confirmation"); //1==Yes 2=Cancel 0=No

				if (iRet == 1)
				{
					//Yes
					iLaunchAfterSync = 504; //Do the actualy save here.
					iSkibFramesBeforeLaunch = 3;
					strcpy(cTriggerMessage, "Saving Level ...");
					bTriggerMessage = true;
					iLaunchAfterSyncAction = 1; //Go to storyboard.
				}
				if (iRet == 0)
				{
					//PE: NO = Restore original fpm. on next load in storyboard.
					g.projectfilename_s = "";
					g.projectmodified = 0; gridedit_changemodifiedflag();
					g.projectmodifiedstatic = 0;
					GGTerrain_CancelRamp();
					bStoryboardWindow = true;
				}
			}
			else
			{
				int iRet = AskSaveBeforeNewAction();
				if (iRet != 2)
				{
					GGTerrain_CancelRamp();
					bStoryboardWindow = true;
				}
			}

			// Object library does not disappear if open when going to storyboard.
			if (bExternal_Entities_Window)
				bTriggerCloseEntityWindow = true;
		}
		if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0) ImGui::SetTooltip("%s", "Back to Game Project Storyboard");
	
		ImGui::SameLine();
		drawCol_tmp = drawCol_back_test; //LB: same background as toogle buttons for consistency
		//if (ImGui::ImgBtn(TOOL_SAVELEVEL, iToolbarIconSize, drawCol_back_gg, drawCol_normal/**drawCol_Selection*/, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, toolbar_gradiant, false, false, false, bBoostIconColors))
		if (ImGui::ImgBtn(TOOL_SAVELEVEL, iToolbarIconSize, drawCol_tmp, drawCol_normal/**drawCol_Selection*/, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, toolbar_gradiant, false, false, false, bBoostIconColors))
		{
			CloseAllOpenToolsThatNeedSave();
			if (bTutorialCheckAction) TutorialNextAction();
			iLaunchAfterSync = 3; //Save
			iSkibFramesBeforeLaunch = 2;
		}
		if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0) ImGui::SetTooltip("%s", "Save Level");
		ImGui::SameLine();

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 2.0f, ImGui::GetCursorPos().y));
	
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		ImVec4 tool_selected_col = ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram];
		if(pref.current_style == 3 )
			tool_selected_col = ImGui::GetStyle().Colors[ImGuiCol_Button];

		ImVec2 tool_selected_padding = { 1.0, 1.0 };

		ImGui::SameLine();
		drawCol_Selection = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 2.0f, ImGui::GetCursorPos().y));
		float precise_icon_width = ImGui::GetCursorPos().x;

		//---------------------------------------------------------------------------------
		drawCol_tmp = drawCol_back_test; //LB: same background as toogle buttons for consistency
		//drawCol_tmp = drawCol_back_entities;
		//if (current_mode == TOOL_ENTITY || current_mode == TOOL_LOGIC) drawCol_tmp = drawCol_back_entities * drawCol_back_active; else drawCol_tmp = drawCol_back_entities;

		CheckTutorialAction("TOOL_TESTGAME", -10.0f); //Tutorial: check if we are waiting for this action
		if (ImGui::ImgBtn(TOOL_TESTGAME, iToolbarIconSize, drawCol_tmp, drawCol_normal/**drawCol_Selection*/, drawCol_hover, drawCol_Down,0, 0, 0, 0, false, toolbar_gradiant,false,false,false, bBoostIconColors))
		{
			CloseAllOpenTools(false);
			if (bTutorialCheckAction) TutorialNextAction();
			iLaunchAfterSync = 1;
		}

		if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0) ImGui::SetTooltip("%s", "Test Level");

		ImGui::SameLine();

		precise_icon_width = ImGui::GetCursorPos().x - precise_icon_width;
		
		// VR Mode
		if (g.gvrmode > 0 && g.gvrmodefordevelopers == 1)
		{
			if (ImGui::ImgBtn(TOOL_VRMODE, iToolbarIconSize, drawCol_tmp, drawCol_normal*drawCol_Selection, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, toolbar_gradiant, false, false, false, bBoostIconColors))
			{
				CloseAllOpenTools();
				if (bTutorialCheckAction) TutorialNextAction();
				iLaunchAfterSync = 20; //Test game VR.
			}
			if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0)
			{
				ImGui::SetTooltip("%s", "Test Level in VR (Developer Mode Feature)");
			}
			ImGui::SameLine();
		}


		toolbar_offset_center = ImGui::GetCursorPos().x - cursorpos;


		float rightx = ImGui::GetContentRegionMax().x;
		float right_border = 2.0f;

		//########################
		//#### Toggle Buttons ####
		//########################

		ImVec4 toggle_color;

		//------------------------------------------------------------------

		CheckTutorialAction("TOOL_SHAPE", -10.0f); //Tutorial: check if we are waiting for this action
		
		ImGui::SetCursorPos(ImVec2(rightx - right_border - (precise_icon_width * 7), ImGui::GetCursorPos().y));
		if (t.grideditselect == 0 && t.terrain.terrainpaintermode >= 1 && t.terrain.terrainpaintermode <= 12)
		{
			//PE: Keep selection in all sculpt modes.
			drawCol_tmp = drawCol_back_terrain * drawCol_back_active;
			if (pref.current_style == 25) drawCol_Selection = drawCol_Divider_Selected;
			window->DrawList->AddRect((window->DC.CursorPos - tool_selected_padding), window->DC.CursorPos + tool_selected_padding + iToolbarIconSize, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
		}
		else
		{
			drawCol_tmp = drawCol_back_terrain;
		}
		drawCol_tmp = drawCol_back_test; //PE: Test. same background as toogle buttons.
		if (t.visuals.bEnableEmptyLevelMode == false)
		{
			if (ImGui::ImgBtn(TOOL_TERRAIN_TOOLBAR, iToolbarIconSize, drawCol_tmp, drawCol_normal, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, toolbar_gradiant, false, false, false, bBoostIconColors))
			{
				CloseAllOpenTools();
				if (bTutorialCheckAction) TutorialNextAction();
				if (!pref.iEnableSingleRightPanelAdvanced)
				{
					Logic_Settings_Window = false;
					Game_Settings_Window = false;
					Weather_Tools_Window = false;
					Visuals_Tools_Window = false;
					//LB: Shooter now a filter mode Shooter_Tools_Window = false;
					iRestoreLastWindow = 0;
				}
				bForceKey = true;
				csForceKey = "t";
				bForceKey2 = true;
				csForceKey2 = "1";
			}
			if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0) ImGui::SetTooltip("%s", "Terrain, Painting, Trees and Vegetation (T)"); //"Terrain Tools"
		}
		ImGui::SameLine();

		//------------------------------------------------------------------

		ImGui::SetCursorPos(ImVec2(rightx - right_border - (precise_icon_width * 6), ImGui::GetCursorPos().y));

		drawCol_Selection = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		CheckTutorialAction("TOOL_ENTITY", -10.0f); //Tutorial: check if we are waiting for this action
		if (current_mode == TOOL_ENTITY || current_mode == TOOL_LOGIC) drawCol_tmp = drawCol_back_entities * drawCol_back_active; else drawCol_tmp = drawCol_back_entities;
		if ((current_mode == TOOL_ENTITY || current_mode == TOOL_LOGIC) && pref.current_style == 25) drawCol_Selection = drawCol_Divider_Selected;
		if ((current_mode == TOOL_ENTITY || current_mode == TOOL_LOGIC) && pref.current_style >= 0) window->DrawList->AddRect((window->DC.CursorPos - tool_selected_padding), window->DC.CursorPos + tool_selected_padding + iToolbarIconSize, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
		drawCol_tmp = drawCol_back_test; //PE: Test. same background as toogle buttons.
		if (ImGui::ImgBtn(TOOL_ENTITY, iToolbarIconSize, drawCol_tmp, drawCol_normal/**drawCol_Selection*/, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, toolbar_gradiant, false, false, false, bBoostIconColors)) {

			CloseAllOpenTools();
			if (bTutorialCheckAction) TutorialNextAction();
			if (!pref.iEnableSingleRightPanelAdvanced)
			{
				Logic_Settings_Window = false;
				Game_Settings_Window = false;
				Weather_Tools_Window = false;
				Visuals_Tools_Window = false;
				//LB: Shooter now a filter mode Shooter_Tools_Window = false;
				iRestoreLastWindow = 0;
			}

			bForceKey = true;
			csForceKey = "o";
			Entity_Tools_Window = true;
			GGTerrain::GGTerrain_CancelRamp();
		}
		if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0) ImGui::SetTooltip("%s", "Object Tools (O)"); //Entity Mode

		ImGui::SameLine();
		
		//------------------------------------------------------------------

		// Logic Toolbar - was Shooter Start
		ImGui::SetCursorPos(ImVec2(rightx - right_border - (precise_icon_width * 5), ImGui::GetCursorPos().y));

		toggle_color = drawCol_toogle;
		if (pref.current_style == 25) drawCol_Selection = drawCol_Divider_Selected;
		if (!Shooter_Tools_Window) {
			drawCol_Selection = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			toggle_color = drawCol_back_test;
		}

		if (ImGui::ImgBtn(TOOL_LOGIC, iToolbarIconSize, toggle_color, drawCol_normal/**drawCol_Selection*/, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, toolbar_gradiant, false, false, false, bBoostIconColors))
		{
			if (iRestoreLastWindow == 0 && !pref.iEnableSingleRightPanelAdvanced)
			{
				if (bTerrain_Tools_Window)
					iRestoreLastWindow = 1;
				else
					iRestoreLastWindow = 2;
			}
			if (Shooter_Tools_Window)
			{
				Shooter_Tools_Window = false;
				if (iRestoreLastWindow >= 0 && !pref.iEnableSingleRightPanelAdvanced)
				{
					if (iRestoreLastWindow == 1)
					{
						bTerrain_Tools_Window = true;
						t.grideditselect = 0;
					}
					else
					{
						bForceKey = true;
						csForceKey = "o";
						Entity_Tools_Window = true;
					}
					iRestoreLastWindow = 0;
				}
			}
			else
			{
				if (bTerrain_Tools_Window)
				{
					//PE: Switch to object mode, so we dont make terrain changes when in logic.
					bTerrain_Tools_Window = false;
					t.grideditselect = 5;
				}

				if (!pref.iEnableSingleRightPanelAdvanced)
					CloseAllOpenTools();

				Shooter_Tools_Window = true;

				if (!pref.iEnableSingleRightPanelAdvanced)
				{
					Visuals_Tools_Window = false;
					Game_Settings_Window = false;
					if (Weather_Tools_Window)
						Weather_Tools_Window = false;
					Entity_Tools_Window = false;
				}
			}
			Logic_Settings_Window = Shooter_Tools_Window;
		}
		if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0) ImGui::SetTooltip("%s", "Visual Logic Connections");
		ImGui::SameLine();
		//Shooter End


		ImGui::SetCursorPos(ImVec2(rightx - right_border - (precise_icon_width * 4), ImGui::GetCursorPos().y));
		toggle_color = drawCol_toogle;
		if (pref.current_style == 25) drawCol_Selection = drawCol_Divider_Selected;
		if (!Visuals_Tools_Window) {
			drawCol_Selection = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			toggle_color = drawCol_back_test;
		}
		
		if (ImGui::ImgBtn(TOOL_VISUALS, iToolbarIconSize, toggle_color, drawCol_normal, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, toolbar_gradiant, false, false, false, bBoostIconColors)) {
			//Display weather window.
			if (iRestoreLastWindow == 0 && !pref.iEnableSingleRightPanelAdvanced)
			{
				if (bTerrain_Tools_Window)
					iRestoreLastWindow = 1;
				else
					iRestoreLastWindow = 2;
			}
			if (!pref.iEnableSingleRightPanelAdvanced)
				CloseAllOpenTools();

			if (Visuals_Tools_Window) {

				Visuals_Tools_Window = false;
				if (iRestoreLastWindow >= 0 && !pref.iEnableSingleRightPanelAdvanced)
				{
					if (iRestoreLastWindow == 1)
					{
						bTerrain_Tools_Window = true;
						t.grideditselect = 0;
					}
					else
					{
						bForceKey = true;
						csForceKey = "o";
						Entity_Tools_Window = true;
					}
					iRestoreLastWindow = 0;
				}
			}
			else {
				Visuals_Tools_Window = true;
				if (!pref.iEnableSingleRightPanelAdvanced)
				{
					Logic_Settings_Window = false;
					Game_Settings_Window = false;
					if (Weather_Tools_Window)
						Weather_Tools_Window = false;
					//LB: Shooter now a filter mode if (Shooter_Tools_Window)
					//	Shooter_Tools_Window = false;
					Entity_Tools_Window = false;
				}
			}
		}

		if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0) ImGui::SetTooltip("%s", "Environment Effects");
		ImGui::SameLine();

		ImGui::SetCursorPos(ImVec2(rightx - right_border -(precise_icon_width*3) , ImGui::GetCursorPos().y));
		toggle_color = drawCol_toogle;
		if (pref.current_style == 25) drawCol_Selection = drawCol_Divider_Selected;


		//PE: New Game Settings icon.
		if (!Game_Settings_Window)
		{
			drawCol_Selection = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			toggle_color = drawCol_back_test;
		}

		if (ImGui::ImgBtn(TOOL_GAME_SETTINGS, iToolbarIconSize, toggle_color, drawCol_normal/**drawCol_Selection*/, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, toolbar_gradiant, false, false, false, bBoostIconColors))
		{
			//Display game seetings window.
			if (iRestoreLastWindow == 0 && !pref.iEnableSingleRightPanelAdvanced)
			{
				if (bTerrain_Tools_Window)
					iRestoreLastWindow = 1;
				else
					iRestoreLastWindow = 2;
			}
			if (!pref.iEnableSingleRightPanelAdvanced)
				CloseAllOpenTools();
			if (Game_Settings_Window)
			{
				Game_Settings_Window = false;
				if (iRestoreLastWindow >= 0 && !pref.iEnableSingleRightPanelAdvanced)
				{
					if (iRestoreLastWindow == 1)
					{
						bTerrain_Tools_Window = true;
						t.grideditselect = 0;
					}
					else
					{
						bForceKey = true;
						csForceKey = "o";
						Entity_Tools_Window = true;
					}
					iRestoreLastWindow = 0;
				}
			}
			else
			{
				Game_Settings_Window = true;
				if (!pref.iEnableSingleRightPanelAdvanced)
				{
					if (Visuals_Tools_Window)
						Visuals_Tools_Window = false;
					Entity_Tools_Window = false;
				}
			}
		}
		if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0) ImGui::SetTooltip("%s", "Game Settings");
		ImGui::SameLine();

		ImGui::SetCursorPos(ImVec2(rightx - right_border - (precise_icon_width * 2), ImGui::GetCursorPos().y));
		toggle_color = drawCol_toogle;
		if (pref.current_style == 25) drawCol_Selection = drawCol_Divider_Selected;
		if (!bEditorLight)
		{
			drawCol_Selection = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			toggle_color = drawCol_back_test;
		}

		if (ImGui::ImgBtn(TOOL_CAMERALIGHT, iToolbarIconSize, toggle_color, drawCol_normal, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, toolbar_gradiant,false,false,false, bBoostIconColors)) 
		{
			//Toggle camera mode.
			if (bEditorLight) bEditorLight = false;
			else bEditorLight = true;
			WickedCall_EnableCameraLight(bEditorLight);
		}
		if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0) ImGui::SetTooltip("%s", "Editor Light");
		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(rightx - right_border - (precise_icon_width * 1), ImGui::GetCursorPos().y));

		bool bIsTopDownStatus = !(bool)t.editorfreeflight.mode;
		toggle_color = drawCol_toogle;
		if (pref.current_style == 25) drawCol_Selection = drawCol_Divider_Selected;
		drawCol_Selection = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		toggle_color = drawCol_back_test;

		if (ImGui::ImgBtn(TOOL_CAMERA, iToolbarIconSize, toggle_color, drawCol_normal, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, toolbar_gradiant,false,false,false, bBoostIconColors))
		{
			//Toggle camera mode.
			bForceKey = true;
			if (bIsTopDownStatus)
				csForceKey = "f";
			else
			{
				csForceKey = "f";
			}
		}
		if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0) ImGui::SetTooltip("%s", "Camera View");

		ImGui::PopStyleVar();
		ImGui::PopStyleVar();


		if (bOldWelcomeScreen_Window)
		{
			ImGui::PopItemFlag(); //PE: Enable this tab.
		}

		if (pref.current_style == 25) {
			ImGui::PopStyleColor(2);
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.43f, 0.57f, 1.00f)); //org ImVec4(0.58f, 0.58f, 0.58f, 1.00f); // ImGui::PopStyleColor();
		}

		#define HIDE_MOVED_MENU_TO_STORYBOARD

		if (bOldWelcomeScreen_Window)
		{
			//Disable
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		}

		if (ImGui::BeginMenuBar() && bExternal_Entities_Window==false && bPreferences_Window==false) // cannot have MENU when viewing XX Library
		{
			if (ImGui::BeginMenu("File") )
			{
				//PE: YES new/load/save as is back :)

				// recommend an actual dialog to control how terrain/sky/veg/trees/stuff are generated
				// to start the user off with something more interesting than bumpy terrain!
				if (pref.iEnableLevelEditorOpenAndNew)
				{
					if (ImGui::MenuItem("New Level", "CTRL+N"))
					{
						int iRet = AskSaveBeforeNewAction();
						if (iRet == 0) //No save
						{
							g.projectmodified = 0; gridedit_changemodifiedflag();
							g.projectmodifiedstatic = 0;
						}
						if (iRet != 2)
						{
							bNoSecondAsk = true;
							CloseAllOpenTools();
						}
						if (iRet != 2)
						{
							//PE: Default to terrain tools , like when we launch Max.
							bForceKey = true;
							csForceKey = "t";
							bForceKey2 = true;
							csForceKey2 = "6";
							t.inputsys.domodeterrain = 1; t.inputsys.dowaypointview = 0;
							t.gridentitymarkersmodeonly = 0; t.grideditselect = 0;
							t.terrain.terrainpaintermode = 6;
							bTerrain_Tools_Window = true;
							// must reset any manual editing
							GGTerrain_ResetSculpting();
							void reset_terrain_paint_date(void);
							reset_terrain_paint_date();
							bProceduralLevelFromStoryboard = false;
							iLaunchAfterSync = 5;
							iSkibFramesBeforeLaunch = 5;
						}
					}
				}

				if (pref.iEnableLevelEditorOpenAndNew)
				{
					if (ImGui::MenuItem("Open Level", "CTRL+O"))
					{
						CloseAllOpenToolsThatNeedSave();
						iLaunchAfterSync = 2;
						iSkibFramesBeforeLaunch = 5;
					}
				}

				if (ImGui::MenuItem("Save Level", "CTRL+B"))
				{
					//Save
					CloseAllOpenToolsThatNeedSave();
					iLaunchAfterSync = 3; 
					iSkibFramesBeforeLaunch = 5;
				}

				if (ImGui::MenuItem("Save Level As...", ""))//CTRL+R" ) )//F12") )
				{
					//Save As
					CloseAllOpenToolsThatNeedSave();
					iLaunchAfterSync = 4; 
					iSkibFramesBeforeLaunch = 5;
				}

				ImGui::Separator();

				if (pref.iEnableLevelEditorOpenAndNew)
				{
					//for (int ii = 0; ii < REMEMBERLASTFILES; ii++) { //reverse
					for (int ii = REMEMBERLASTFILES - 1; ii >= 0; ii--) 
					{
						if (strlen(pref.last_open_files[ii]) > 0) 
						{
							char tmp[260];
							strcpy(tmp, pref.last_open_files[ii]);
							int pos = strlen(tmp);
							while (pos > 0 && tmp[pos] != '\\') pos--;

							//std::string s_tmp = std::to_string(1+ii); //Reverse
							std::string s_tmp = std::to_string(REMEMBERLASTFILES - ii);
							s_tmp += ": ";
							s_tmp += &tmp[pos + 1];

							if (ImGui::MenuItem(s_tmp.c_str())) 
							{
								if (bWaypointDrawmode || bWaypoint_Window) { bWaypointDrawmode = false; bWaypoint_Window = false; }
								if (bImporter_Window) { importer_quit(); bImporter_Window = false; }
								if (g_bCharacterCreatorPlusActivated) g_bCharacterCreatorPlusActivated = false;
								if (bEntity_Properties_Window) bEntity_Properties_Window = false;
								if (t.ebe.on == 1) ebe_hide();
								strcpy(cDirectOpen, pref.last_open_files[ii]);
								iLaunchAfterSync = 7; //Direct open.
								iSkibFramesBeforeLaunch = 5;
							}
						}
					}
					ImGui::Separator();
				}


				if (ImGui::MenuItem("Back to Storyboard Editor"))
				{
					CloseAllOpenToolsThatNeedSave();
					//Storyboard can change levels ... so make sure we ask to save first.
					int iRet = AskSaveBeforeNewAction();
					if (iRet != 2)
					{
						bStoryboardWindow = true;
					}
					bStoryboardWindow = true;
					GGTerrain_CancelRamp();
				}

				ImGui::EndMenu();
			}
			else
			{
				if(pref.bAutoOpenMenuItems)
					if (ImGui::IsItemHovered()) 
						ImGui::OpenPopup("File");
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo","CTRL+Z")) 
				{
					//t.inputsys.doundo = 1;
					bForceUndo = true;
				}
				if (ImGui::MenuItem("Redo", "CTRL+Y")) 
				{
					t.inputsys.doredo = 1;
					bForceRedo = true;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {
					iExecuteCTRLkey = 'X';
				}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {
					iExecuteCTRLkey = ImGuiKey_C;
				}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {
					iExecuteCTRLkey = ImGuiKey_V;
				}
				if (ImGui::MenuItem("Import", "CTRL+I")) {
					iExecuteCTRLkey = 'I';
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Delete", "DEL")) {
					iExecuteCTRLkey = ImGuiKey_Delete;
				}



				ImGui::Separator();




				if (ImGui::MenuItem("Settings", "")) 
				{
					strcpy(cPreferencesMessage,"");
					bPreferences_Window = true;
				}

				ImGui::EndMenu();
			}
			else
			{
				if (pref.bAutoOpenMenuItems)
					if (ImGui::IsItemHovered())
						ImGui::OpenPopup("Edit");
			}

			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::MenuItem("Quest Editor"))
				{
					CloseAllOpenTools();
					extern bool bQuestEditor_Window;
					bQuestEditor_Window = true;
				}

				if (ImGui::MenuItem("Character Creator"))
				{
					CloseAllOpenTools();
					iLaunchAfterSync = 82; //Start Character Creator
					iSkibFramesBeforeLaunch = 2;
					strcpy(cTriggerMessage, "Loading Character Creator");
					bTriggerMessage = true;
				}

				// Tooling
				if (g_bParticleEditorPresent == true)
				{
					ImGui::Separator();
					if (ImGui::MenuItem("Particle Editor"))
					{
						extern void launchOrShowParticleEditor(void);
						launchOrShowParticleEditor();
					}
				}
				if (g_bBuildingEditorPresent == true)
				{
					if(!g_bParticleEditorPresent == true)
						ImGui::Separator();

					if (ImGui::MenuItem("Building Editor"))
					{
						extern void launchOrShowBuildingEditor(void);
						launchOrShowBuildingEditor();
					}
				}

				ImGui::EndMenu();
			}
			else
			{
				if (pref.bAutoOpenMenuItems)
					if (ImGui::IsItemHovered())
						ImGui::OpenPopup("Tools");
			}


#ifndef GGMAXEDU
			static bool bMarketHovered = false;
			window = ImGui::GetCurrentWindow();
			ImRect text_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(100, ImGui::GetFontSize()) );
			bool bEnabled = true;
			if (ImGui::MenuItem2("Marketplace",nullptr, bMarketHovered, bEnabled))
			{
				CloseAllOpenTools();
				DeleteWaypointsAddedToCurrentCursor();
				CloseDownEditorProperties();
				bMarketplace_Window = true;
			}
			ImVec2 startend = ImGui::GetCursorPos();
			text_bb.Max.x = window->DC.CursorPos.x;
			if (ImGui::IsMouseHoveringRect(text_bb.Min, text_bb.Max))
			{
				bMarketHovered = true;
			}
			else
				bMarketHovered = false;

#endif



			if (ImGui::BeginMenu("Help"))
			{

				image_setlegacyimageloading(true);


				if (ImGui::MenuItem("Test Level Controls")) {

					strcpy(cHelpMenuImage, "languagebank\\english\\artwork\\testgamelayout.png");
					LoadImage(cHelpMenuImage, HELPMENU_IMAGE);
					bHelp_Menu_Image_Window = true;
				}

				// no VR for now
				if (ImGui::MenuItem("Read User Manual")) 
				{
					//ExecuteFile("https://gameguru-max.document360.io/docs", "", "", 0);
					// User guide has been moved to offline only
					ExecuteFile("..\\Guides\\User Manual\\GameGuru MAX - User Guide.pdf", "", "", 0);
				}
				if (ImGui::MenuItem("Guides Folder"))
				{
					ExecuteFile("..\\Guides\\", "", "", 0);
				}
				char pAbsEXE[MAX_PATH];
				sprintf(pAbsEXE, "%s\\Guides\\MaxLua\\MaxLua.exe", g.fpscrootdir_s.Get());
				if (FileExist(pAbsEXE))
				{
					if (ImGui::MenuItem("Scripting Command List"))
					{
						char pAbsPATH[MAX_PATH];
						sprintf(pAbsPATH, "%s\\Guides\\MaxLua\\", g.fpscrootdir_s.Get());
						ExecuteFile(pAbsEXE, "", pAbsPATH, 0);
					}
				}
				if (g_bParticleEditorPresent == true)
				{
					if (ImGui::MenuItem("Particle Editor User Guide"))
					{
						char pOldDir[MAX_PATH];
						strcpy(pOldDir, GetDir());
						SetDir("..");
						SetDir("Tools\\Particle Editor\\media\\docs\\");
						ExecuteFile("Particle Editor User Guide.pdf", "", "", 0);
						SetDir(pOldDir);
					}
				}
				if (g_bBuildingEditorPresent == true)
				{
					if (ImGui::MenuItem("Building Editor Getting Started"))
					{
						ExecuteFile("https://www.youtube.com/watch?v=5o865epIhqY", "", "", 0);
					}
				}
				#ifndef DISABLETUTORIALS
				if (ImGui::MenuItem("Getting Started Tutorial"))
				{
					bHelpVideo_Window = true;
					bHelp_Window = true;
					bSetTutorialSectionLeft = false;
					strcpy(cForceTutorialName, "01 - Getting started");
				}
				#endif
				if (ImGui::MenuItem("Report an Issue (GitHub)"))
				{
					ExecuteFile("https://github.com/TheGameCreators/GameGuruRepo/issues/new", "", "", 0);

				}
				if (g_bFreeTrialVersion == true)
				{
					if (ImGui::MenuItem("Buy GameGuru MAX"))
					{
						CloseAllOpenTools();
						DeleteWaypointsAddedToCurrentCursor();
						CloseDownEditorProperties();
						bFreeTrial_Window = true;
					}
				}

				if (ImGui::MenuItem("About")) {
					bAbout_Window = true;
					bAbout_Window_First_Run = true;
				}
				image_setlegacyimageloading(false);

				ImGui::EndMenu();
			}
			else
			{
				if (pref.bAutoOpenMenuItems)
					if (ImGui::IsItemHovered()) 
						ImGui::OpenPopup("Help");
			}
			ImGui::EndMenuBar();
		}

		if (bOldWelcomeScreen_Window)
		{
			ImGui::PopItemFlag(); //PE: Enable this tab.
		}

		if (pref.current_style == 25)
			ImGui::PopStyleColor(); 

		//Process systemwide shortcut keys.

		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::GetTime() - lastKeyTime >= 0.125) 
		{ 
			//small delay between key input.
			auto ctrl = io.KeyCtrl;
			auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;
			auto shift = io.KeyShift;

			//PE: No repeat on these keys.
			static bool bWaitOnGRelease = false;
			if (bWaitOnGRelease && ImGui::IsKeyReleased(71))
				bWaitOnGRelease = false;

			//LB: ensure CTRL+Z can release Z and repress so all does not happen at once
			static bool bWaitOnZRelease = false;
			if (bWaitOnZRelease && !ImGui::IsKeyDown(90))
			{
				t.inputsys.undokeypress = 0;
				bWaitOnZRelease = false;
			}

			if (ctrl && !shift && !alt && ImGui::IsKeyPressed(89) || iExecuteCTRLkey == 'Y' ) //Y
			{ 
				//CTRL Y - redo
				lastKeyTime = (float)ImGui::GetTime();
				iExecuteCTRLkey = 0;
				bForceRedo = true;
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(78) || iExecuteCTRLkey == 'N') //N
			{
				lastKeyTime = (float)ImGui::GetTime();
				iExecuteCTRLkey = 0;
				if (bStoryboardWindow && iExecuteCTRLkey != 'N' && !bProceduralLevel) iStoryboardExecuteKey = 'N';
				if (!bStoryboardWindow)
				{
					if (pref.iEnableLevelEditorOpenAndNew)
					{
						CloseAllOpenTools();
						iLaunchAfterSync = 5;
						bProceduralLevelFromStoryboard = false;
						iSkibFramesBeforeLaunch = 5;
					}
				}
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(76)) //CTRL+L, Add level in storyboard, lock/unlock selected object(s) in editor
			{
				lastKeyTime = (float)ImGui::GetTime();
				iExecuteCTRLkey = 0;
				if (bStoryboardWindow && !bProceduralLevel)
					iStoryboardExecuteKey = 'L';
				else
				{
					bool bLock = true;
					// Determine if the selected object should be locked/unlocked.
					int iObjectLockedIndex = -1;
					if (vEntityLockedList.size() > 0)
					{
						for (int i = 0; i < vEntityLockedList.size(); i++)
						{
							int e = vEntityLockedList[i].e;
							if (e < 0 || e >= t.entityelement.size()) continue;

							if (e == t.widget.pickedEntityIndex)
							{
								iObjectLockedIndex = i;
								bLock = false;
								break;
							}
						}
					}

					LockSelectedObject(bLock, iObjectLockedIndex);
				}
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(69)) //E
			{
				lastKeyTime = (float)ImGui::GetTime();
				iExecuteCTRLkey = 0;
				if (bStoryboardWindow && !bProceduralLevel) iStoryboardExecuteKey = 'E';
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(32)) //SPACE
			{
				lastKeyTime = (float)ImGui::GetTime();
				iExecuteCTRLkey = 0;
				if (bStoryboardWindow && !bProceduralLevel) iStoryboardExecuteKey = ' ';
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(73) || iExecuteCTRLkey == 'I') //I - Importer
			{
				lastKeyTime = (float)ImGui::GetTime();
				iExecuteCTRLkey = 0;
				DeleteWaypointsAddedToCurrentCursor();
				CloseDownEditorProperties();
				CloseAllOpenTools();
				iLaunchAfterSync = 8; //Import model
				iSkibFramesBeforeLaunch = 5;
				bMarketplace_Window = false;
				if (bExternal_Entities_Window)
				{
					bTriggerCloseEntityWindow = true;
					bCheckForClosingForce = true; //Force window to close.
				}
				bEnableWeather = false;
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(79) || iExecuteCTRLkey == 'O' ) //O
			{
				if (pref.iEnableLevelEditorOpenAndNew)
				{
					lastKeyTime = (float)ImGui::GetTime();
					iExecuteCTRLkey = 0;
					CloseAllOpenTools();
					iLaunchAfterSync = 2;
					iSkibFramesBeforeLaunch = 5;
				}
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(66) || iExecuteCTRLkey == 'B') //B
			{
				lastKeyTime = (float)ImGui::GetTime();
				iExecuteCTRLkey = 0;
				CloseAllOpenToolsThatNeedSave();
				iLaunchAfterSync = 3; //Save
				iSkibFramesBeforeLaunch = 5;
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(88) || iExecuteCTRLkey == 'X') //X , "CUT"
			{
				if (!bLastImGuiGotFocus || iExecuteCTRLkey == 'X')
				{
					lastKeyTime = (float)ImGui::GetTime();
					iExecuteCTRLkey = 0;
					t.widget.deletebuttonselected = 1;
					widget_show_widget();
				}
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(90) && bWaitOnZRelease==false) 
			{ 
				//CTRL Z - undo
				lastKeyTime = (float)ImGui::GetTime();
				iExecuteCTRLkey = 0;
				bForceUndo = true;
				bWaitOnZRelease = true;
				t.inputsys.undokeypress = 1;
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(71)) //CTRL+G
			{
				//CTRL G - Group.
				if (!bWaitOnGRelease)
					CreateNewGroup(-1);
				bWaitOnGRelease = true;
			}
			else if (ctrl && shift && !alt && ImGui::IsKeyPressed(71)) //CTRL+SHIFT+G
			{
				//CTRL+SHIFT G - UnGroup.
				if(!bWaitOnGRelease)
					UnGroupSelected();
				bWaitOnGRelease = true;
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C)) || iExecuteCTRLkey == ImGuiKey_C)
			{
				//PE: Mouse need to be in Level Editor for copy paste objects to work.
				if (!bLastImGuiGotFocus || iExecuteCTRLkey == ImGuiKey_C)
				{
					lastKeyTime = (float)ImGui::GetTime();
					iExecuteCTRLkey = 0;

					//CTRL C - copy rubber band or single to clipboard
					if (t.widget.pickedEntityIndex > 0)
					{
						g_EntityClipboard.clear();
						if (g.entityrubberbandlist.size() > 0)
						{
							for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
							{
								int e = g.entityrubberbandlist[i].e;
								int entid = t.entityelement[e].bankindex;
								if (entid > 0 && t.entityprofile[entid].ismarker != 1)
								{
									g_EntityClipboard.push_back(e);
								}
							}
						}
						else
						{
							int entid = t.entityelement[t.widget.pickedEntityIndex].bankindex;
							if (entid > 0 && t.entityprofile[entid].ismarker != 1 )
							{
								g_EntityClipboard.push_back(t.widget.pickedEntityIndex);
							}
						}
						g_EntityClipboardAnchorEntityIndex = t.widget.pickedEntityIndex;
					}
				}
			}
			else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V)) || iExecuteCTRLkey == ImGuiKey_V )
			{
				//PE: Mouse need to be in Level Editor for copy paste objects to work.
				if (!bLastImGuiGotFocus || iExecuteCTRLkey == ImGuiKey_V)
				{

					lastKeyTime = (float)ImGui::GetTime();
					iExecuteCTRLkey = 0;

					//CTRL V - duplicate from clipboard
					if (g_EntityClipboard.size() > 0)
					{
						// batch the paste into a single event
						undosys_multiplevents_start();

						// a small offset so user can see new pasted entity
						float fShiftOffsetForPasteX = 25.0f + rand() % 50;
						float fShiftOffsetForPasteZ = 25.0f + rand() % 50;
						if (rand() % 2 == 0) fShiftOffsetForPasteX = -fShiftOffsetForPasteX;
						if (rand() % 2 == 0) fShiftOffsetForPasteZ = -fShiftOffsetForPasteZ;

						// determine anchor entity from original entity group
						int iAnchorEntityIndex = -1;

						// we are also going to move rubber band selection to new pasted group
						g.entityrubberbandlist.clear();

						// for each entity, create a duplicate and offset slightly so we can see it
						for (int i = 0; i < (int)g_EntityClipboard.size(); i++)
						{
							// duplicate new entity as clone of relevant original clipboard entity
							int e = g_EntityClipboard[i];
							//PE: Crash from paste from another level.
							if (e < t.entityelement.size())
							{
								t.gridentity = t.entityelement[e].bankindex;
								//PE: all t.gridentity... need to be set for this to work correctly.
								t.entid = t.gridentity;
								entity_fillgrideleproffromprofile();  // t.entid
								t.gridentityposx_f = t.entityelement[e].x;
								t.gridentityposy_f = t.entityelement[e].y;
								t.gridentityposz_f = t.entityelement[e].z;
								t.gridentityrotatex_f = t.entityelement[e].rx;
								t.gridentityrotatey_f = t.entityelement[e].ry;
								t.gridentityrotatez_f = t.entityelement[e].rz;
								t.gridentityrotatequatmode = t.entityelement[e].quatmode;
								t.gridentityrotatequatx_f = t.entityelement[e].quatx;
								t.gridentityrotatequaty_f = t.entityelement[e].quaty;
								t.gridentityrotatequatz_f = t.entityelement[e].quatz;
								t.gridentityrotatequatw_f = t.entityelement[e].quatw;
								if (t.entityprofile[t.gridentity].ismarker == 10)
								{
									t.gridentityscalex_f = 100.0f + t.entityelement[e].scalex;
									t.gridentityscaley_f = 100.0f + t.entityelement[e].scaley;
									t.gridentityscalez_f = 100.0f + t.entityelement[e].scalez;
								}
								else
								{
									t.gridentityscalex_f = ObjectScaleX(t.entityelement[e].obj);
									t.gridentityscaley_f = ObjectScaleY(t.entityelement[e].obj);
									t.gridentityscalez_f = ObjectScaleZ(t.entityelement[e].obj);
								}
								t.grideleprof = t.entityelement[e].eleprof;
								entity_cleargrideleprofrelationshipdata();
								t.grideleprof.newparticle.emitterid = -1; //PE: Must always get a new emitter ID.

								//PE: InstanceObject - Cursor,Object Tools - objects must always be real clones.
								extern bool bNextObjectMustBeClone;
								bNextObjectMustBeClone = true;

								gridedit_addentitytomap();

								bNextObjectMustBeClone = false;

								if (e == g_EntityClipboardAnchorEntityIndex) iAnchorEntityIndex = t.e;
								t.entityelement[t.e].x = t.entityelement[e].x + fShiftOffsetForPasteX;
								t.entityelement[t.e].y = t.entityelement[e].y;
								t.entityelement[t.e].z = t.entityelement[e].z + fShiftOffsetForPasteZ;
								t.entityelement[t.e].rx = t.entityelement[e].rx;
								t.entityelement[t.e].ry = t.entityelement[e].ry;
								t.entityelement[t.e].rz = t.entityelement[e].rz;
								t.entityelement[t.e].quatmode = t.entityelement[e].quatmode;
								t.entityelement[t.e].quatx = t.entityelement[e].quatx;
								t.entityelement[t.e].quaty = t.entityelement[e].quaty;
								t.entityelement[t.e].quatz = t.entityelement[e].quatz;
								t.entityelement[t.e].quatw = t.entityelement[e].quatw;
								t.entityelement[t.e].editorfixed = t.entityelement[e].editorfixed;
								t.entityelement[t.e].staticflag = t.entityelement[e].staticflag;
								t.entityelement[t.e].scalex = t.entityelement[e].scalex;
								t.entityelement[t.e].scaley = t.entityelement[e].scaley;
								t.entityelement[t.e].scalez = t.entityelement[e].scalez;
								t.entityelement[t.e].soundset = t.entityelement[e].soundset;
								t.entityelement[t.e].soundset1 = t.entityelement[e].soundset1;
								t.entityelement[t.e].soundset2 = t.entityelement[e].soundset2;
								t.entityelement[t.e].soundset3 = t.entityelement[e].soundset3;
								t.entityelement[t.e].soundset4 = t.entityelement[e].soundset4;
								t.entityelement[t.e].soundset5 = t.entityelement[e].soundset5;
								t.entityelement[t.e].soundset6 = t.entityelement[e].soundset6;
								//PE: We have a new particle id here, so cant just copy.
								newparticletype backup_newparticle = t.entityelement[t.e].eleprof.newparticle;
								t.entityelement[t.e].eleprof = t.entityelement[e].eleprof;
								t.entityelement[t.e].eleprof.newparticle = backup_newparticle;
								PositionObject(t.entityelement[t.e].obj, t.entityelement[t.e].x, t.entityelement[t.e].y, t.entityelement[t.e].z);
								RotateObject(t.entityelement[t.e].obj, t.entityelement[t.e].rx, t.entityelement[t.e].ry, t.entityelement[t.e].rz);

								// Can't copy object relations so ensure previous are cleared
								t.entityelement[t.e].eleprof.iObjectLinkID = 0;
								//PE: This caused a crash iObjectRelationshipsData[j] > 10 (should have been j) made memory overwrite inside eleprof
								for (int j = 0; j < 10; j++)
								{
									t.entityelement[t.e].eleprof.iObjectRelationships[j] = 0;
									t.entityelement[t.e].eleprof.iObjectRelationshipsType[j] = 0;
									t.entityelement[t.e].eleprof.iObjectRelationshipsData[j] = 0;
								}

								// and add to new rubber band group
								sRubberBandType rubberbandItem;
								rubberbandItem.e = t.e;
								rubberbandItem.x = t.entityelement[t.e].x;
								rubberbandItem.y = t.entityelement[t.e].y;
								rubberbandItem.z = t.entityelement[t.e].z;
								rubberbandItem.px = t.entityelement[t.e].x;
								rubberbandItem.py = t.entityelement[t.e].y;
								rubberbandItem.pz = t.entityelement[t.e].z;
								rubberbandItem.rx = t.entityelement[t.e].rx;
								rubberbandItem.ry = t.entityelement[t.e].ry;
								rubberbandItem.rz = t.entityelement[t.e].rz;
								rubberbandItem.quatmode = t.entityelement[t.e].quatmode;
								rubberbandItem.quatx = t.entityelement[t.e].quatx;
								rubberbandItem.quaty = t.entityelement[t.e].quaty;
								rubberbandItem.quatz = t.entityelement[t.e].quatz;
								rubberbandItem.quatw = t.entityelement[t.e].quatw;
								rubberbandItem.scalex = t.entityelement[t.e].scalex;
								rubberbandItem.scaley = t.entityelement[t.e].scaley;
								rubberbandItem.scalez = t.entityelement[t.e].scalez;
								g.entityrubberbandlist.push_back(rubberbandItem);
							}
						}

						// switch widget to newly pasted entity so can instantly widget it about
						if (iAnchorEntityIndex != -1)
						{
							t.widget.pickedEntityIndex = iAnchorEntityIndex;
							t.widget.pickedObject = t.entityelement[iAnchorEntityIndex].obj;
						}

						// batch the paste into a single event finish here
						undosys_multiplevents_finish();

						// ensure gridentity cleared after duplication
						t.gridentity = 0;

						bDraggingActive = false;
						t.onetimeentitypickup = 0;
						iLastSelectedEntityGroup = -1;
						iLastSelectedEntity = -1;
					}
				}
			}
			else if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)) || iExecuteCTRLkey == ImGuiKey_Delete)
			{
				HandleObjectDeletion();
				lastKeyTime = (float)ImGui::GetTime();
				iExecuteCTRLkey = 0;
			}

			

		}

		ImGui::End();

		//####################
		//#### Status bar ####
		//####################

		bool bStatusbarActive = true;
		if (gbWelcomeSystemActive == true || bWelcomeScreen_Window) bStatusbarActive = false;
		if (bStatusbarActive)
		{
			int iOldWindowBorderSize = ImGui::GetStyle().WindowBorderSize;
			ImGui::GetStyle().WindowRounding = 0.0f;
			ImGui::GetStyle().WindowBorderSize = 1.0f;

			float paddingy = ImGui::GetStyle().WindowPadding.y;
			float startposy = viewPortSize.y - (ImGuiStatusBar_Size + 2);
			ImGui::SetNextWindowPos(viewPortPos + ImVec2(0.0f, startposy), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, ImGuiStatusBar_Size));

			if (pref.current_style == 25)
			{
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.26f, 0.35f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.11f, 0.16f, 0.22f, 1.00f)); //org ImVec4(0.58f, 0.58f, 0.58f, 1.00f); // ImGui::PopStyleColor();
			}
			if (pref.current_style == 1)
			{
				//PE: VS2022 style
				const float r = pref.status_bar_color.x; // (1.0f / 255.0f) * 14;
				const float g = pref.status_bar_color.y; // (1.0f / 255.0f) * 99;
				const float b = pref.status_bar_color.z; // (1.0f / 255.0f) * 156;
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(r, g, b, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(r, g, b, 1.00f));
			}

			ImGui::Begin("Statusbar", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 10.0f, ImGui::GetCursorPos().y ));
			ImGui::Text("%s", t.laststatusbar_s.Get());
			ImGui::SameLine();
			//Align right.
			int align_checkbox = 96;
			int align_light_checkbox = 106;
			int align_combo_size = 120;
			#ifndef ADDCONTROLSTOSTAUSBAR
			align_light_checkbox = 0;
			align_checkbox = 0;
			#endif
			if (fpe_thread_in_progress())
			{
				extern int g_iScannedFiles;
				cstr title = cStr("Scanning FPE Files: ") + cStr(g_iScannedFiles) + cStr("  ");
				float fTextSize = ImGui::CalcTextSize(title.Get()).x * 1.05;
				ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - fTextSize, ImGui::GetCursorPos().y ));
				ImGui::Text(title.Get());
			}
			else
			{
				//PE: Display status, grid mode ...
				//statusbar
				float fTextSize = ImGui::CalcTextSize(statusbar).x * 1.05; // t.statusbar_s.Get()).x * 1.05;
				ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - fTextSize - 10.0f, ImGui::GetCursorPos().y ));
				ImGui::Text(statusbar); // t.statusbar_s.Get());
			}
			#ifdef ADDCONTROLSTOSTAUSBAR

			float fTextSize = ImGui::CalcTextSize(t.statusbar_s.Get()).x * 1.05;
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - fTextSize - align_combo_size - align_checkbox - align_light_checkbox, ImGui::GetCursorPos().y));
			ImGui::Text(t.statusbar_s.Get());

			ImGui::SameLine();
			ImVec2 vPos = ImGui::GetCursorPos();

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - align_combo_size - align_checkbox - align_light_checkbox, ImGui::GetCursorPos().y - 5));

			if (ImGui::Checkbox(" Editor Light", &bEditorLight))
			{
				WickedCall_EnableCameraLight(bEditorLight);
			}
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 4.0f));


			ImGui::SameLine();
			//		ImVec2 vPos = ImGui::GetCursorPos();
			ImGui::SetCursorPos(vPos);
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - align_combo_size - align_checkbox, ImGui::GetCursorPos().y - 5));
			bool bTopDownStatus = !(bool)t.editorfreeflight.mode;
			if (ImGui::Checkbox(" Top Down", &bTopDownStatus)) {
				bForceKey = true;
				if (!bTopDownStatus)
					csForceKey = "f";
				else
					csForceKey = "g";
			}
			ImGui::SameLine();

			ImGui::SetCursorPos(vPos);

			const char* items_align[] = { "NORMAL", "SNAP", "GRID" };
			int item_current_type_selection = 0;
			item_current_type_selection = t.gridentitygridlock;

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x - align_combo_size, ImGui::GetCursorPos().y - 5.0));
			ImGui::PushItemWidth(align_combo_size - 10);
			if (ImGui::Combo("##BehavioursSimpleInput", &item_current_type_selection, items_align, IM_ARRAYSIZE(items_align))) {
				t.gridentitygridlock = item_current_type_selection;
			}
			ImGui::PopItemWidth();
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Alignment");
			//			if (t.gridentitygridlock == 0)  t.statusbar_s = t.statusbar_s + "NORMAL";
			//			if (t.gridentitygridlock == 1)  t.statusbar_s = t.statusbar_s + "SNAP";
			//			if (t.gridentitygridlock == 2)  t.statusbar_s = t.statusbar_s + "GRID";

			#endif

			ImGui::End();
			ImGui::GetStyle().WindowRounding = iOldRounding;
			ImGui::GetStyle().WindowBorderSize = iOldWindowBorderSize;

			if (pref.current_style == 25 || pref.current_style == 1)
			{
				ImGui::PopStyleColor(2);
			}

		}
		else
		{
			if (bWelcomeScreen_Window)
			{
				//PE: We need to display it empty, so we can fill it out with background color on welcome screen.
				float startposy = viewPortSize.y - 32 - 2.0;
				ImGui::SetNextWindowPos(viewPortPos + ImVec2(0.0f, startposy), ImGuiCond_Always);
				ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->Size.x, ImGuiStatusBar_Size));
				ImGui::Begin("Statusbar", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
				ImGui::End();
			}
			else
			{
				//PE: We need to display it empty, or the z order will be wrong later.
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::SetNextWindowPos(viewPortPos + ImVec2(0.0f, viewPortSize.y + 40), ImGuiCond_Always); //Out of screen.
				ImGui::Begin("Statusbar", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
				ImGui::End();
			}
		}


		//Docking.
		ImVec4 OldImGuiColWindowBg = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
		ImVec4 OldImGuiColChildBg = ImGui::GetStyle().Colors[ImGuiCol_ChildBg];
		ImGui::GetStyle().Colors[ImGuiCol_ChildBg] = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];

		//######################################################################
		//#### Default dockspace setup, how is our windows split on screen. ####
		//######################################################################

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking; //ImGuiWindowFlags_MenuBar
		viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos + ImVec2(0, toolbar_size));
		ImGui::SetNextWindowSize(viewport->Size - ImVec2(0, toolbar_size + ImGuiStatusBar_Size));
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGuiWindowFlags oldwindow_flags = window_flags;
		window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpaceAGK", &dockingopen, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		window_flags = oldwindow_flags;

		static ImGuiID dock_id_bottom;
		
		//We cant make all windows dock if all windows is NOT undocked first (.ini setup problem ), so refresh_gui_docking == 2
		if (ImGui::DockBuilderGetNode(ImGui::GetID("MyDockspace")) == NULL || refresh_gui_docking == 2)
		{
			//Default docking setup.
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
			ImGui::DockBuilderSetNodePos(dockspace_id, viewport->Pos + ImVec2(0, toolbar_size));
			ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size - ImVec2(0, toolbar_size + ImGuiStatusBar_Size));

			ImGuiID dock_main_id = dockspace_id;
			ImGuiID dock_id_top = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.12f, NULL, &dock_main_id); //Toolbar


			ImGuiID dock_id_right;
			if (viewport->Size.x > 1300)
				dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.14f, NULL, &dock_main_id); //0.20f
			else if (viewport->Size.x < 1100)
				dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.23f, NULL, &dock_main_id); //0.20f
			else
				dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.21f, NULL, &dock_main_id); //0.20f


			ImGuiID dock_id_right2out;
			ImGuiID dock_id_right2 = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, &dock_id_right2out, &dock_main_id); //0.20f

			// create dock ID for above Tutorial Help Window (for Tutorial Video area)
			ImGuiID dock_id_right2below = ImGui::DockBuilderSplitNode(dock_id_right2out, ImGuiDir_Down, 0.26f, NULL, NULL); //0.18

			// leelee, technically not allowed by IMGUI but I fudged the IMGUI code to allow it - seems to work fine!
			ImGuiID dock_id_right3below = ImGui::DockBuilderSplitNode(dock_id_right2out, ImGuiDir_Down, 0.60f, NULL, NULL); //(0.65) PE: Video area must have room for a normal mp4.


			ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.15f, NULL, &dock_main_id); //0.15f
			ImGuiID dock_id_left_down = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.15f, NULL, &dock_id_left); //0.15f
			ImGuiID dock_id_left_down_large = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.30f, NULL, &dock_id_left); //0.15f

			ImGuiID dock_id_left2out;
			ImGuiID dock_id_left2 = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.30f, &dock_id_left2out, &dock_main_id); //0.20f
			ImGuiID dock_id_left2below = ImGui::DockBuilderSplitNode(dock_id_left2out, ImGuiDir_Down, 0.26f, NULL, NULL); //0.18
			ImGuiID dock_id_left3below = ImGui::DockBuilderSplitNode(dock_id_left2out, ImGuiDir_Down, 0.60f, NULL, NULL); //(0.65) PE: Video area must have room for a normal mp4.

			// Disable tab bar for custom toolbar and statusbar
			ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id_top);
			node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

			ImGui::DockBuilderDockWindow(TABEDITORNAME, dock_main_id);
			ImGui::DockBuilderDockWindow(TABENTITYNAME, dock_id_left);

			ImGui::DockBuilderDockWindow("Tutorial Video##HelpVideoWindow", dock_id_right2below);
			ImGui::DockBuilderDockWindow("Tutorial Steps##HelpWindow", dock_id_right3below);

			ImGui::DockBuilderDockWindow("Tutorial Video##LeftHelpVideoWindow", dock_id_left2below);
			ImGui::DockBuilderDockWindow("Tutorial Steps##LeftHelpWindow", dock_id_left3below);

			ImGui::DockBuilderDockWindow("Entity Properties##PropertiesWindow", dock_id_right);
			ImGui::DockBuilderDockWindow("Character Creator##PropertiesWindow", dock_id_right);
			ImGui::DockBuilderDockWindow("Structure Properties##BuilderPropertiesWindow", dock_id_right);
			ImGui::DockBuilderDockWindow("Importer##ImporterWindow", dock_id_right);

			ImGui::DockBuilderDockWindow("Terrain Tools##TerrainToolsWindow", dock_id_right);
			ImGui::DockBuilderDockWindow("Sculpt Terrain##TerrainToolsWindow", dock_id_right);
			ImGui::DockBuilderDockWindow("Paint Terrain##TerrainToolsWindow", dock_id_right);
			ImGui::DockBuilderDockWindow("Add Vegetation##TerrainToolsWindow", dock_id_right);
			ImGui::DockBuilderDockWindow("Terrain Tools##Sculpt Terrain##TerrainToolsWindow", dock_id_right);
			ImGui::DockBuilderDockWindow("Terrain Tools##Paint Terrain##TerrainToolsWindow", dock_id_right);
			ImGui::DockBuilderDockWindow("Terrain Tools##Add Vegetation##TerrainToolsWindow", dock_id_right);
			ImGui::DockBuilderDockWindow("Terrain Tools##Add Trees##TerrainToolsWindow", dock_id_right);
			ImGui::DockBuilderDockWindow("Terrain Tools##Add Bushes##TerrainToolsWindow", dock_id_right);		

			ImGui::DockBuilderDockWindow("Waypoints##WaypointsToolsWindow", dock_id_right);

			ImGui::DockBuilderDockWindow("Object Tools##EntityToolsWindow", dock_id_right);
			ImGui::DockBuilderDockWindow("Environment Effects##VisualsToolsWindow", dock_id_right);
			ImGui::DockBuilderDockWindow("Game Settings##GameSettings", dock_id_right);
			ImGui::DockBuilderDockWindow("Logic Settings##LogicSettings", dock_id_right);
			ImGui::DockBuilderDockWindow("Shooter Genre##GameLogicTools", dock_id_right);
			ImGui::DockBuilderDockWindow("Current Objects##AdditionalIconsWindow", dock_id_left_down_large);
			ImGui::DockBuilderDockWindow("Bug Reporting System##BugReportingWindow", dock_id_right);

			// Disable tab bar.
			//PE: This will not work if you are able to undock windows.
			if (!pref.iAllowUndocking)
			{
				ImGuiDockNode* newnode = ImGui::DockBuilderGetNode(dock_id_left_down_large);
				if (newnode)
					newnode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
			}
			//ImGuiDockNodeFlags_AutoHideTabBar
			dock_main_tabs = dock_main_id;
			dock_tools_windows = dock_id_right;
			ImGui::DockBuilderFinish(dockspace_id);
		}
		
		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 vOldWindowMinSize = style.WindowMinSize;
		style.WindowMinSize.x = 150.0f;

		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

		style.WindowMinSize = vOldWindowMinSize;

		ImGui::End();
		
		// Restore normal window backdrop color, but leave child backdrop color alone (as copied from window backdrop color)
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = OldImGuiColWindowBg;

		if (dock_main_tabs == 0)
			dock_main_tabs = dockspace_id;
		
		grideleprof_uniqui_id = 55000; //If using the new properties widgets outside properties window.

		//#######################
		//#### Tutorial Help ####
		//#######################
		if (bHelp_Window && bHelpVideo_Window == false) bHelp_Window = false;
		//PE: Always read in all tutorials.
		if (!bTutorial_Init) 
		{
			//Reset everything.
			tut.bActive = false;
			tut.iSteps = 0;
			strcpy(tut.cStartText, "");
			strcpy(tut.cVideoPath, "");
			tut.bVideoReady = false;
			tut.bVideoInit = false;
			bVideoResumePossible = false;
			for (int il = 0; il < TUTORIALMAXSTEPS; il++) { //Reset
				strcpy(tut.cStepHeader[il], "");
				strcpy(tut.cStepText[il], "");
				strcpy(tut.cStepAction[il], "");
				tut.vOffsetPointer[il] = ImVec2(0, 0);
			}

			get_tutorials();

			//use first entry.
			std::map<std::string, std::string>::iterator it = tutorial_files.begin();
				
			if (it->first.length() > 0) {
				strcpy(cTutorialName, it->first.c_str());
			}

			bTutorial_Init = true;
		}
			
		if (bHelp_Window && current_tutorial != selected_tutorial) 
		{
			current_tutorial = selected_tutorial;

			//Reset everything.
			tut.bActive = false;
			tut.iCurrent_Step = 0;
			tut.iSteps = 0;
			strcpy(tut.cStartText, "");
			strcpy(tut.cVideoPath, "");
			tut.bVideoReady = false;
			tut.bVideoInit = false;
			bVideoResumePossible = false;

			for (int il = 0; il < TUTORIALMAXSTEPS; il++) { //Reset
				strcpy(tut.cStepHeader[il], "");
				strcpy(tut.cStepText[il], "");
				strcpy(tut.cStepAction[il], "");
				tut.vOffsetPointer[il] = ImVec2(0, 0);
			}

			//Read in selected tutorial.
			int count_tut = 0;

			//Find filename to use:
			cstr tut_filename = "";// editors\\uiv3\\tutorial.txt";
			if (tutorial_files.size() > 0) 
			{
				for (std::map<std::string, std::string>::iterator it = tutorial_files.begin(); it != tutorial_files.end(); ++it) 
				{
					if (it->first.length() > 0) 
					{
						if (count_tut++ >= selected_tutorial) 
						{
							tut_filename = it->second.c_str();
							break;
						}
					}
				}
			}

			//Reset active tutorial.
			FILE* fTut = GG_fopen(tut_filename.Get(), "r");
			if (fTut)
			{
				char ctmp[TUTORIALMAXTEXT];
				bool bStart = false;
				while (!feof(fTut))
				{
					fgets(ctmp, TUTORIALMAXTEXT-1, fTut);
					if (strlen(ctmp) > 0 && ctmp[strlen(ctmp) - 1] == '\n')
						ctmp[strlen(ctmp) - 1] = 0;

					if (strncmp(ctmp, "TUT:", 4) == 0)
					{
						if( bStart )
							break; // new section exit.
						bStart = true;
					}
					if (bStart) {
						//Add to active tutorial.
						bool bFound = false;

						if (strncmp(ctmp, "VIDEO:", 6) == 0)
						{
							strcpy(tut.cVideoPath, &ctmp[7]);

							char resolved[MAX_PATH];
							int retval = GetFullPathNameA(tut.cVideoPath, MAX_PATH, resolved, NULL);
							if (retval > 0) {
								strcpy(tut.cVideoPath, resolved);
							}
							bFound = true;
						}
						if (strncmp(ctmp, "START:", 6) == 0)
						{
							strcpy(tut.cStartText, &ctmp[7]);
							bFound = true;
						}

						if (!bFound) {
							for (int il = 1; il < TUTORIALMAXSTEPS; il++) {

								cstr cmp = "STEP"; cmp = cmp + Str(il); cmp = cmp + "-HEADER:";
								if (strncmp(ctmp, cmp.Get(), cmp.Len()) == 0) {
									strcpy(tut.cStepHeader[il - 1], &ctmp[cmp.Len() + 1]);
									bFound = true;
								}
								cmp = "STEP"; cmp = cmp + Str(il); cmp = cmp + "-TEXT:";
								if (strncmp(ctmp, cmp.Get(), cmp.Len()) == 0) {
									strcpy(tut.cStepText[il - 1], &ctmp[cmp.Len() + 1]);
									bFound = true;
								}
								cmp = "STEP"; cmp = cmp + Str(il); cmp = cmp + "-ACTION:";
								if (strncmp(ctmp, cmp.Get(), cmp.Len()) == 0) {
									strcpy(tut.cStepAction[il - 1], &ctmp[cmp.Len() + 1]);
									bFound = true;
								}
								cmp = "STEP"; cmp = cmp + Str(il); cmp = cmp + "-OFFSETX:";
								if (strncmp(ctmp, cmp.Get(), cmp.Len()) == 0) {
									tut.vOffsetPointer[il - 1].x = atof(&ctmp[cmp.Len() + 1]);
									bFound = true;
								}
								cmp = "STEP"; cmp = cmp + Str(il); cmp = cmp + "-OFFSETY:";
								if (strncmp(ctmp, cmp.Get(), cmp.Len()) == 0) {
									tut.vOffsetPointer[il - 1].y = atof(&ctmp[cmp.Len() + 1]);
									bFound = true;
								}
								if (bFound)
								{
									if (tut.iSteps < il)
										tut.iSteps = il;
									break;
								}
							}
						}
					}
				}
				fclose(fTut);
				if (tut.iSteps > 0) 
				{
					//Add Tutorial Complete
					strcpy(tut.cStepHeader[tut.iSteps], "Final Tutorial Step");
					strcpy(tut.cStepText[tut.iSteps], "");
					strcpy(tut.cStepAction[tut.iSteps], "-=DONE=-");
					tut.iSteps++;
				}
			}
		}

		if (refresh_gui_docking == 0)
		{
			//Make sure window is setup in docking space.
			ImGui::Begin("Tutorial Video##HelpVideoWindow", &bHelpVideo_Window, iGenralWindowsFlags);
			ImGui::End();
			ImGui::Begin("Tutorial Steps##HelpWindow", &bHelp_Window, iGenralWindowsFlags);
			ImGui::End();
			ImGui::Begin("Tutorial Video##LeftHelpVideoWindow", &bHelpVideo_Window, iGenralWindowsFlags);
			ImGui::End();
			ImGui::Begin("Tutorial Steps##LeftHelpWindow", &bHelp_Window, iGenralWindowsFlags);
			ImGui::End();
		}
		else if (bHelp_Window && tutorial_files.size() > 0)
		{
			char cTutWindowVideoName[256];
			char cTutWindowStepsName[256];

			if (bSetTutorialSectionLeft)
			{
				strcpy(cTutWindowVideoName, "Tutorial Video##LeftHelpVideoWindow");
				strcpy(cTutWindowStepsName, "Tutorial Steps##LeftHelpWindow");
			}
			else
			{
				strcpy(cTutWindowVideoName, "Tutorial Video##HelpVideoWindow");
				strcpy(cTutWindowStepsName, "Tutorial Steps##HelpWindow");
			}

			if( iVideoFindFirstFrame > 0) {
				if (iVideoFindFirstFrame == 1) {
					PauseAnim(tut.bVideoID);
					bVideoResumePossible = false;
				}
				iVideoFindFirstFrame--;
			}

			switch (iVideoDelayExecute) {
				case 1: //Play restart
				{
					iVideoDelayExecute = 0;
					StopAnimation(tut.bVideoID);
					PlayAnimation(tut.bVideoID);
					SetRenderAnimToImage(tut.bVideoID, true);
					UpdateAllAnimation();
					Sleep(50); //Sleep so we get a video texture in the next call.
					UpdateAllAnimation();
					SetVideoVolume(100.0);
					bVideoResumePossible = false;
					break;
				}
				case 2: //Resume
				{
					iVideoDelayExecute = 0;
					ResumeAnim(tut.bVideoID);
					break;
				}
				case 3: //Pause
				{
					iVideoDelayExecute = 0;
					PauseAnim(tut.bVideoID);
					bVideoResumePossible = true;
					break;
				}
				default:
					break;
			}

			if (bVideoPlayerMaximized) 
			{
				ImGui::SetNextWindowSize(ImVec2(48 * ImGui::GetFontSize(), 46 * ImGui::GetFontSize()), ImGuiCond_Once);
				ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
				ImGui::Begin("Tutorial Video##Videos2MaxSize", &bVideoPlayerMaximized, 0);
			}
			else 
			{
				ImGui::Begin(cTutWindowVideoName, &bHelpVideo_Window, iGenralWindowsFlags);
			}
			ImGui::Indent(10);
			ImGui::PushItemWidth(-10);

			if (ImGui::BeginCombo("##SelectYourTutorial", cTutorialName) ) // The second parameter is the label previewed before opening the combo.
			{
				int vloop = 0;
				for (std::map<std::string, std::string>::iterator it = tutorial_files.begin(); it != tutorial_files.end(); ++it)
				{
					if (it->first.length() > 0)
					{
						bool is_selected = false;
						if (strcmp(it->first.c_str(), cTutorialName) == 0)
							is_selected = true;
						if (ImGui::Selectable(it->first.c_str(), is_selected))
						{
							//Change Tutorial.
							strcpy(cTutorialName, it->first.c_str());
							selected_tutorial = vloop;
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
						vloop++;
					}
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

			// and a force tutorial mode
			bool bForceASelection = false;
			if (strlen(cForceTutorialName) > 0)
			{
				strcpy(cTutorialName, cForceTutorialName);
				strcpy(cForceTutorialName, "");
				int vloop = 0;
				for (std::map<std::string, std::string>::iterator it = tutorial_files.begin(); it != tutorial_files.end(); ++it)
				{
					if (it->first.length() > 0)
					{
						if (strcmp(it->first.c_str(), cTutorialName) == 0)
						{
							strcpy(cTutorialName, it->first.c_str());
							selected_tutorial = vloop;
						}
						vloop++;
					}
				}
				bForceASelection = true;
			}

			// use video panel (wait until videos GOOD and other Rick-Requests)
			ImVec4 oldImGuiCol_ChildWindowBg = ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg];
			#ifdef ENABLETUTORIALVIDEOS
			{
				if (!tut.bVideoInit)
				{
					if (tut.bVideoID > 0) {
						if (AnimationExist(tut.bVideoID)) {
							if (AnimationPlaying(tut.bVideoID))
								StopAnimation(tut.bVideoID);

							DeleteAnimation(tut.bVideoID);
							tut.bVideoID = 0;
						}
					}

					t.tvideofile_s = tut.cVideoPath;
					tut.bVideoID = 0;
					t.text_s = Lower(Right(t.tvideofile_s.Get(), 4));
					if (t.text_s == ".ogv" || t.text_s == ".mp4")
					{
						tut.bVideoID = 32;
						for (int itl = 1; itl <= 32; itl++)
						{
							if (AnimationExist(itl) == 0) { tut.bVideoID = itl; break; }
						}
						char pFinalVideoFilePath[MAX_PATH];
						strcpy(pFinalVideoFilePath, t.tvideofile_s.Get());
						GG_GetRealPath(pFinalVideoFilePath, 0);
						if (LoadAnimation(pFinalVideoFilePath, tut.bVideoID, g.videoprecacheframes, g.videodelayedload, 1) == false)
						{
							tut.bVideoID = -999;
						}
					}
					if (tut.bVideoID > 0) 
					{
						PlaceAnimation(tut.bVideoID, -1, -1, -1, -1);
						SetRenderAnimToImage(tut.bVideoID, true);
						//Try to get first frame.
						StopAnimation(tut.bVideoID);
						PlayAnimation(tut.bVideoID);
						SetRenderAnimToImage(tut.bVideoID, true);
						iVideoFindFirstFrame = 4;
						UpdateAllAnimation();
						bVideoResumePossible = false;
						bVideoPerccentStart = false;
					}
					tut.bVideoInit = true;
				}

				float fRatio = 1.0f / ((float)GetDesktopWidth() / (float)GetDesktopHeight());

				ID3D11ShaderResourceView* lpVideoTexture = GetAnimPointerView(tut.bVideoID);
				float fVideoW = GetAnimWidth(tut.bVideoID);
				float fVideoH = GetAnimHeight(tut.bVideoID);
				if (tut.bVideoInit && tut.bVideoID > 0 && lpVideoTexture) {
					fRatio = 1.0f / (fVideoW / fVideoH);
				}

				float videoboxheight = (ImGui::GetContentRegionAvail().x - 10.0) * fRatio;

				oldImGuiCol_ChildWindowBg = ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg];
				ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
				ImGui::BeginChild("Video##TutorialVideo", ImVec2(ImGui::GetContentRegionAvail().x - 10.0, videoboxheight), true, iGenralWindowsFlags);
				window = ImGui::GetCurrentWindow();
				ImRect image_bb(window->DC.CursorPos, window->DC.CursorPos + ImGui::GetContentRegionAvail());
				if (lpVideoTexture) {
					SetRenderAnimToImage(tut.bVideoID, true);
					float animU = GetAnimU(tut.bVideoID);
					float animV = GetAnimV(tut.bVideoID);
					ImVec2 uv0 = ImVec2(0, 0);
					ImVec2 uv1 = ImVec2(animU, animV);
					window->DrawList->AddImage((ImTextureID)lpVideoTexture, image_bb.Min, image_bb.Max, uv0, uv1, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
				}

				if( !(tut.bVideoID > 0 && AnimationExist(tut.bVideoID) && AnimationPlaying(tut.bVideoID) ))
				{
					//Display a play button.
					ImVec2 vOldPos = ImGui::GetCursorPos();
					float fPlayButSize = ImGui::GetContentRegionAvail().x * 0.15;
					float fCenterX = (ImGui::GetContentRegionAvail().x*0.5) - (fPlayButSize*0.5);
					float fCenterY = (videoboxheight*0.5) - (fPlayButSize*0.5);
					ImGui::SetCursorPos(ImVec2(fCenterX, fCenterY));
					ImVec4 vColorFade = { 1.0,1.0,1.0,0.5 };
					if (ImGui::ImgBtn(MEDIA_PLAY, ImVec2(fPlayButSize, fPlayButSize), ImColor(255, 255, 255, 0), drawCol_normal*vColorFade, drawCol_hover*vColorFade, drawCol_Down*vColorFade, -1, 0, 0, 0, false,false,false,false,false, bBoostIconColors))
					{
						bVideoPerccentStart = true;
						if (bVideoResumePossible) {
							iVideoDelayExecute = 2; //resume
						}
						else {
							iVideoDelayExecute = 1; //play - restart.
						}
					}
					if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Play");

					ImGui::SetCursorPos(vOldPos);
				}

				if (ImGui::IsMouseHoveringRect(image_bb.Min, image_bb.Max)) 
				{
					if (!bStoryboardWindow && !bProceduralLevel)
					{
						if (ImGui::IsMouseDoubleClicked(0))
						{
							bVideoPlayerMaximized = 1 - bVideoPlayerMaximized;
						}
					}
				}

				ImGui::EndChild();
				ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg] = oldImGuiCol_ChildWindowBg;

				if (tut.bVideoID > 0) {
					if (AnimationExist(tut.bVideoID)) {

						//ImGui::SameLine();
						float fdone = GetAnimPercentDone(tut.bVideoID) / 100.0f;
						if (!bVideoPerccentStart) fdone = 0.0f;

						ImGui::ProgressBar(fdone, ImVec2(ImGui::GetContentRegionAvail().x - 10, 6), "");

#define MEDIAICONSIZE 20

						if (ImGui::ImgBtn(MEDIA_PLAY, ImVec2(MEDIAICONSIZE, MEDIAICONSIZE), ImColor(255, 255, 255, 0), drawCol_normal, drawCol_hover, drawCol_Down, -1, 0, 0, 0, false,false,false,false,false, bBoostIconColors))
						{
							bVideoPerccentStart = true;
							if (bVideoResumePossible) {
								iVideoDelayExecute = 2; //resume
							}
							else {
								iVideoDelayExecute = 1; //play - restart.
							}
						}
						if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Play");
						ImGui::SameLine();
						if (ImGui::ImgBtn(MEDIA_PAUSE, ImVec2(MEDIAICONSIZE, MEDIAICONSIZE), ImColor(255, 255, 255, 0), drawCol_normal, drawCol_hover, drawCol_Down, -1, 0, 0, 0, false,false,false,false,false, bBoostIconColors))
						{
							iVideoDelayExecute = 3; // pause
						}
						if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Pause");
						ImGui::SameLine();
						if (ImGui::ImgBtn(MEDIA_REFRESH, ImVec2(MEDIAICONSIZE, MEDIAICONSIZE), ImColor(255, 255, 255, 0), drawCol_normal, drawCol_hover, drawCol_Down, -1, 0, 0, 0, false,false,false,false,false, bBoostIconColors))
						{
							bVideoPerccentStart = true;
							iVideoDelayExecute = 1; //play - restart.
						}
						if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Restart");



						if (!bVideoPlayerMaximized)
						{
							ImGui::SameLine();
							if (ImGui::ImgBtn(MEDIA_MAXIMIZE, ImVec2(MEDIAICONSIZE, MEDIAICONSIZE), ImColor(255, 255, 255, 0), drawCol_normal, drawCol_hover, drawCol_Down, -1, 0, 0, 0, true,false,false,false,false, bBoostIconColors))
							{
								bVideoPlayerMaximized = true;
							}
							if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Maximize");
						}
						else
						{
							ImGui::SameLine();
							if (ImGui::ImgBtn(MEDIA_MINIMIZE, ImVec2(MEDIAICONSIZE, MEDIAICONSIZE), ImColor(255, 255, 255, 0), drawCol_normal, drawCol_hover, drawCol_Down, -1, 0, 0, 0, true,false,false,false,false, bBoostIconColors))
							{
								bVideoPlayerMaximized = false;
							}
							if (ImGui::windowTabVisible() && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Minimize");
						}



					}
				}
			}

			if (bVideoPlayerMaximized) {
				std::map<std::string, std::string>::iterator it = tutorial_description.find(cTutorialName);
				if (it != tutorial_description.end()) {
					cVideoDescription = it->second.c_str();
					ImGui::Separator();
					ImGui::Text("Description");
					ImGui::TextWrapped(cVideoDescription.Get());
				}
				bImGuiGotFocus = true;
			}

			if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
				//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
				ImGui::Text("");
				ImGui::Text("");
			}

			#endif

			ImGui::End();

			///

			ImGui::Begin(cTutWindowStepsName, &bHelp_Window, iGenralWindowsFlags);

			ImGui::PushItemWidth(-10);
			if (ImGui::StyleButton(tut.cStartText, ImVec2(ImGui::GetContentRegionAvail().x - 10.0f, 0.0f))) {
				tut.bActive = true;
				tut.iCurrent_Step = 0;
				//PE: Minimize video player.
				bVideoPlayerMaximized = false;
			}
			ImGui::PopItemWidth();

			for (int il = 0; il < tut.iSteps; il++) {

				int additional_lines = 0;
				char line_split[TUTORIALMAXTEXT], *line_found = NULL, *line_start = NULL;
				strcpy(line_split, tut.cStepText[il]);
				line_start = line_found = &line_split[0];
				while ((line_found = (char *)pestrcasestr(line_found, "\\n"))) {
					line_found++;
					line_found++;
					additional_lines++;
				}
				float stepboxheight = mCharAdvance.y * (3 + additional_lines);

				cstr uniqueid = "##STEP";
				uniqueid += Str(il);

				float fOldChildRounding = ImGui::GetStyle().ChildRounding;
				ImGui::GetStyle().ChildRounding = 10.0f;

				if (tut.bActive && tut.iCurrent_Step == il) { //Set current step color.
					ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg] = ImGui::GetStyle().Colors[ImGuiCol_Button];
					static int last_scroll_set = -1;
					if (tut.iCurrent_Step != last_scroll_set) {
						last_scroll_set = tut.iCurrent_Step;
						ImGui::SetScrollHereY();
					}
				}

				ImGui::BeginChild(uniqueid.Get(), ImVec2(ImGui::GetContentRegionAvail().x-10.0, stepboxheight) , true, iGenralWindowsFlags);
					
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, mCharAdvance.y*0.5 ));

				ImGui::SetWindowFontScale(1.15);
				ImGui::TextCenter(tut.cStepHeader[il]);
				ImGui::SetWindowFontScale(1.0);

				//Update Score:
				tut.fScore = (float) tut.iCurrent_Step / (float) (tut.iSteps-1) * 100.0f;
				if (tut.fScore <= 0.99f) tut.fScore = 0.0f;
				if (tut.fScore >= 99.9f) {
					tut.fScore = 100.0f;
					strcpy(tut.cStepText[tut.iSteps - 1], "COMPLETE - Well Done!");
				}
				else {
					strcpy(tut.cStepText[tut.iSteps - 1], "INCOMPLETE");
				}
									
				strcpy(line_split, tut.cStepText[il]);
				line_start = line_found = &line_split[0];

				while ((line_found = (char *)pestrcasestr(line_found, "\\n"))) {
					*line_found = 0;
					ImGui::TextCenter(line_start);
					line_found++;
					line_found++;
					line_start = line_found;
				}
				ImGui::TextCenter(line_start);

				ImGui::EndChild();

				ImGui::GetStyle().ChildRounding = fOldChildRounding;
				ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg] = oldImGuiCol_ChildWindowBg;

				ImGui::Spacing();
			}

			ImGui::Indent(-10);

			if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) 
			{
				//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
				ImGui::Text("");
				ImGui::Text("");
			}

			ImRect bbwin(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize());
			if (ImGui::IsMouseHoveringRect(bbwin.Min, bbwin.Max))
			{
				bImGuiGotFocus = true;
			}
			if (ImGui::IsAnyItemFocused()) {
				bImGuiGotFocus = true;
			}
			//Tutorial really small min, as we have multiply dock to the same side.
			CheckMinimumDockSpaceSize(20.0f);

			ImGui::End();

		}
		else 
		{
			//Help window closed , check if we ned to free any videos.
			if (tut.bVideoID > 0) {
				current_tutorial = -1; //make sure to reopen when window visible again.
				if (AnimationExist(tut.bVideoID)) {
					if (AnimationPlaying(tut.bVideoID))
						StopAnimation(tut.bVideoID);
					DeleteAnimation(tut.bVideoID);
					tut.bVideoID = 0;
					bVideoResumePossible = false;
				}
			}
		}

		//###############################
		//#### Welcome Screen Window ####
		//###############################
		if( gbWelcomeSystemActive == false )
		{
			// only show Welcome Screen if the 'old' welcome/announcement is not in effect
			Welcome_Screen();
		}
		else
		{
			Welcome_Screen(); //Also dislay welcome screen behind in new design. to hide 3D editor.
		}

		//#############################
		//#### Market place Window ####
		//#############################


		#define STOREPROMOICONS 8

		float fMarketplacePanelHeight = 48.0f;// 55.0f; fits 4:3 aspect!

			if (g_bFreeTrialVersion == true)
			{
				if (bMarketplace_Window == true)
				{
					bFreeTrial_Window = true;
					bMarketplace_Window = false;
				}
			}
			if (refresh_gui_docking == 0)
			{
				ImGui::SetNextWindowSize(ImVec2(68 * ImGui::GetFontSize(), fMarketplacePanelHeight * ImGui::GetFontSize()), ImGuiCond_Always);
				ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
				bool bTmp = true;
				ImGui::Begin("Marketplace##MarketplaceWindow", &bTmp, 0);
				ImGui::End();
			}
			else if (bMarketplace_Window)
			{
				int StorePromoItems = STOREPROMOICONS;
				static int gg_max_dlc[STOREPROMOICONS];
				static cstr gg_max_link[STOREPROMOICONS];
				static int sketchfab_dlc[STOREPROMOICONS];
				static cstr sketchfab_link[STOREPROMOICONS];
				static int shockwavesound_dlc[STOREPROMOICONS];
				static cstr shockwavesound_link[STOREPROMOICONS];
				static int community_dlc[STOREPROMOICONS];
				static cstr community_link[STOREPROMOICONS];
				static int gcstore_dlc[STOREPROMOICONS];
				static cstr gcstore_imageurl[STOREPROMOICONS];
				static cstr gcstore_link[STOREPROMOICONS];
				static int import_image[STOREPROMOICONS];
				static cstr import_fpe[STOREPROMOICONS];
				if (!bMarketplace_Init)
				{
					for (int i = 0; i < STOREPROMOICONS; i++)
					{
						image_setlegacyimageloading(true);
						if(gg_max_dlc[i] > 0 && ImageExist(gg_max_dlc[i])) DeleteImage(gg_max_dlc[i]);
						if (sketchfab_dlc[i] > 0 && ImageExist(sketchfab_dlc[i])) DeleteImage(sketchfab_dlc[i]);
						if (shockwavesound_dlc[i] > 0 && ImageExist(shockwavesound_dlc[i])) DeleteImage(shockwavesound_dlc[i]);
						if (community_dlc[i] > 0 && ImageExist(community_dlc[i])) DeleteImage(community_dlc[i]);
						if (gcstore_dlc[i] > 0 && ImageExist(gcstore_dlc[i])) DeleteImage(gcstore_dlc[i]);
						if (import_image[i] > 0 && ImageExist(import_image[i])) DeleteImage(import_image[i]);
						image_setlegacyimageloading(false);
						gg_max_dlc[i] = 0;
						gg_max_link[i] = "";
						sketchfab_dlc[i] = 0;
						sketchfab_link[i] = "";
						shockwavesound_dlc[i] = 0;
						shockwavesound_link[i] = "";
						community_dlc[i] = 0;
						community_link[i] = "";
						gcstore_dlc[i] = 0;
						gcstore_imageurl[i] = "";
						gcstore_link[i] = "";	
						import_image[i] = 0;
						import_fpe[i] = "";
					}
					SetMipmapNum(1);
					image_setlegacyimageloading(true);
					
					loadMarketplaceData(gg_max_dlc, gg_max_link, sketchfab_dlc, sketchfab_link, shockwavesound_dlc, shockwavesound_link, 
										community_dlc, community_link, gcstore_dlc, gcstore_imageurl, gcstore_link);

					image_setlegacyimageloading(false);
					SetMipmapNum(-1);
					bMarketplace_Init = true;

					//##################################
					//### Load last imported models. ###
					//##################################

					int iImportPromoIcon = 0;
					for (int i = 0; i < 10; i++)
					{
						if (strlen(pref.last_import_files[i]) > 0)
						{
							//PE: Check if we got a cached thumb in correct format.
							char cTmp[MAX_PATH];
							strcpy(cTmp, "entitybank\\");
							strcat(cTmp, pref.last_import_files[i]);
							CreateBackBufferCacheName(pref.last_import_files[i], 512, 288);
							extern StoryboardStruct Storyboard;
							if (!(strlen(Storyboard.gamename) > 0 && strlen(Storyboard.customprojectfolder) > 0))
							{
								GG_SetWritablesToRoot(true);
							}
							if (FileExist(BackBufferCacheName.Get()) && FileExist(cTmp) )
							{
								SetMipmapNum(1);
								image_setlegacyimageloading(true);
								import_image[iImportPromoIcon] = MARKETPLACE_ICONS + (STOREPROMOICONS * 3) + iImportPromoIcon;

								LoadImage((char *)BackBufferCacheName.Get(), import_image[iImportPromoIcon]);
								if (ImageExist(import_image[iImportPromoIcon]))
								{
									import_fpe[iImportPromoIcon] = pref.last_import_files[i];
									iImportPromoIcon++;
								}
								image_setlegacyimageloading(false);
								SetMipmapNum(-1);
							}
							GG_SetWritablesToRoot(false);
						}
						if (iImportPromoIcon >= STOREPROMOICONS)
							break;
					}
				}

				// TGC only sell objects right now!
				bool bHideGGMaxMarketplace = false;
				if (iDisplayLibraryType != 0 || iDisplayLibrarySubType !=0)
					bHideGGMaxMarketplace = true;

				if (bHideGGMaxMarketplace)
					ImGui::SetNextWindowSize(ImVec2(((90.0f / 4.0f) * 2) * ImGui::GetFontSize(), 33 * ImGui::GetFontSize()), ImGuiCond_Always);// ImGuiCond_Once);
				else
					ImGui::SetNextWindowSize(ImVec2(((90.0f / 4.0f) * 3) * ImGui::GetFontSize(), fMarketplacePanelHeight * ImGui::GetFontSize()), ImGuiCond_Always);//ImGuiCond_Once);

				ImGui::SetNextWindowPosCenter(ImGuiCond_Always);// ImGuiCond_Once);
				
				ImGui::Begin("Marketplace##MarketplaceWindow", &bMarketplace_Window, ImGuiWindowFlags_None | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings);
				ImGui::Indent(10);

				ImGui::Text("");
				ImVec2 vCurPos = ImGui::GetCursorPos();
				float fFontSize = ImGui::GetFontSize();
				int icon_size = ImGui::GetFontSize()*3.0;
				ImVec2 VIconSize = { (float)icon_size, (float)icon_size };
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, -15));
				if (ImGui::ImgBtn(TOOL_GOBACK, VIconSize, ImVec4(0, 0, 0, 0), drawCol_normal, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
				{
					bMarketplace_Window = false;
				}
				if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0) ImGui::SetTooltip("%s", "Exit Marketplace");
				ImGui::SameLine();

				ImVec2 VHeaderSize;
				if (bHideGGMaxMarketplace)
				{
					ImGui::SetWindowFontScale(1.0);
					ImGui::Text("");
					VHeaderSize = { 365, 39 };
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(154, -58));
					ImGui::ImgBtn(MARKETPLACE_HEADER, VHeaderSize, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), 0, 0, 0, 0, false, false, false, false, false, false);
					ImGui::Text("");
				}
				else
				{
					VHeaderSize = { 730, 78 };
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(78, -10));
					ImGui::ImgBtn(MARKETPLACE_HEADER, VHeaderSize, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), 0, 0, 0, 0, false, false, false, false, false, false);
				}

				float fButWidth = 150.0f;
				
				if (bHideGGMaxMarketplace)
					ImGui::Columns(1, "Marketplacecolumns4", false);  //false no border
				else
					ImGui::Columns(2, "Marketplacecolumns4", false);  //false no border

				float fContentWidth = ImGui::GetContentRegionAvailWidth();
				float fLogoWidth = fContentWidth;
				float fImageWidth = 460;
				float fImageHeight = 215;

				//Use same size on all logos.
				if (ImageExist(MARKETPLACE_GGMAX))
				{
					fImageWidth = ImageWidth(MARKETPLACE_GGMAX);
					fImageHeight = ImageHeight(MARKETPLACE_GGMAX);
				}
				float fScale = fLogoWidth / fImageWidth;
				float fRatio = fImageHeight / fImageWidth;
				ImVec2 vLogoSize = { fLogoWidth , fImageHeight * fScale };
				ImVec2 vPromoSize = { fLogoWidth , fImageHeight * fScale };

				ImVec2 vSizeOfScrollablePanels = ImVec2(vPromoSize.x, ((vPromoSize.y-15.0f) * 3));

				vPromoSize *= 0.5;
				vPromoSize = vPromoSize - ImVec2(15.0f, 15.0f); //ImVec2(5.0f, 5.0f);

				ImVec2 vYOffsetToButtons = ImVec2(0, (((475 / 68.6f) * vPromoSize.y)));
				if (bHideGGMaxMarketplace) vYOffsetToButtons = ImVec2(0, (((150 / 68.6f)*vPromoSize.y)));

				//MAX x=154.25 y=69.432

				int TextureID = MARKETPLACE_FILLER;
				float fPromoHeight = ImGui::GetCursorPosY();
				if (!bHideGGMaxMarketplace)
				{
					if (ImageExist(MARKETPLACE_GGMAX)) TextureID = MARKETPLACE_GGMAX;
					if (ImGui::ImgBtn(TextureID, vLogoSize, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, false, false, false, false, false))
					{
						ExecuteFile("https://store.steampowered.com/app/1247290/GameGuru_MAX/", "", "", 0);
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip("%s", "Click to view the GameGuru MAX Steam Store Page");
					}

					//#### Promo icon section. ####
					vCurPos = ImGui::GetCursorPos();
					fPromoHeight = ImGui::GetCursorPosY();
					for (int i = 0; i < 2; i++)
					{
						TextureID = MARKETPLACE_FILLER;
						ImVec4 vFadeIcons = { 1.0,1.0,1.0,0.2 };
						if (ImageExist(gg_max_dlc[i]))
						{
							TextureID = gg_max_dlc[i];
							vFadeIcons = { 1.0,1.0,1.0,1.0 };
						}
						ImGui::SetCursorPos(ImVec2(vCurPos.x+10.0f, ImGui::GetCursorPos().y));
						if (ImGui::ImgBtn(TextureID, vPromoSize*2, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)*vFadeIcons, drawCol_hover*vFadeIcons, drawCol_Down*vFadeIcons, -1, 0, 0, 0, false, false, false, false, false, false))
						{
							// already aware of steam and non-steam, assigned directly from "MarketplaceData.json"
							if (gg_max_link[i].Len() > 0)
							{
								ExecuteFile(gg_max_link[i].Get(), "", "", 0);
							}
						}
					}
					fPromoHeight = ImGui::GetCursorPosY() - fPromoHeight;

					ImGui::SetWindowFontScale(1.4f);		
					ImGui::SetCursorPos(vCurPos + vYOffsetToButtons);
					{
						if (ImGui::StyleButton("Get More DLC", ImVec2(vLogoSize.x, fFontSize*2.0)))
						{
							DeleteWaypointsAddedToCurrentCursor();
							CloseDownEditorProperties();
							ExecuteFile("https://store.steampowered.com/dlc/1247290/GameGuru_MAX/", "", "", 0);
							bTriggerCloseEntityWindow = true;
						}
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click to view the GameGuru MAX Steam DLC Page");
					}
					ImGui::SetWindowFontScale(1.0);

					ImGui::NextColumn();
				}

				TextureID = MARKETPLACE_FILLER;
				if (ImageExist(MARKETPLACE_GCSTORE))
					TextureID = MARKETPLACE_GCSTORE;

				if (ImGui::ImgBtn(TextureID, vLogoSize, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, false, false, false, false, false))
				{
					ExecuteFile("https://gamecreator.store/max?r=tgc", "", "", 0);
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click to view the Game Creator Store Website");

				//#### Promo icon section. ####
				vCurPos = ImGui::GetCursorPos();
				if ( iDisplayLibraryType == 0 )
				{
					fPromoHeight = ImGui::GetCursorPosY();
					ImGui::BeginChild("##gamecreatorstorescrollable", vSizeOfScrollablePanels, false, ImGuiWindowFlags_None | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings);
					for (int i = 0; i < 7; i++)
					{
						TextureID = MARKETPLACE_FILLER;
						ImVec4 vFadeIcons = { 1.0,1.0,1.0,0.2 };
						if (ImageExist(gcstore_dlc[i]))
						{
							TextureID = gcstore_dlc[i];
							vFadeIcons = { 1.0,1.0,1.0,1.0 };
						}
						else
						{
							// slowly load in preview thumbs over time so as not to stall the UI
							if (strlen(gcstore_imageurl[i].Get()) > 0)
							{
								static DWORD g_dwLoadGameCreatorStorePreviewsTimer = 0;
								if (timeGetTime() > g_dwLoadGameCreatorStorePreviewsTimer + 100)
								{
									// attempt a download
									char cUrl[256];
									char pDataReturned[132000];
									memset(pDataReturned, 0, sizeof(pDataReturned));
									DWORD dwDataReturnedSize = 0;
									char pImageFile[256];
									sprintf(pImageFile, "downloads\\gcStore%d.png", 1 + i);
									if (FileExist(pImageFile) == 1) DeleteAFile(pImageFile);
									std::string url = gcstore_imageurl[i].Get();
									replaceAll(url, "\\/", "/");
									replaceAll(url, "https://gcs-product-media.fra1.cdn.digitaloceanspaces.com", "");
									replaceAll(url, "http://gcs-product-media.fra1.cdn.digitaloceanspaces.com", "");
									strcpy(cUrl, url.c_str());
									LPSTR pPassInURL = cUrl;
									int iError = StoreOpenURLForDataOrFile("gcs-product-media.fra1.cdn.digitaloceanspaces.com", pDataReturned, &dwDataReturnedSize, "", "GET", pPassInURL, pImageFile);
									if (iError > 0)
									{
										// error - no image today!
										strcpy (pImageFile, "editors\\marketplace\\gcStore0.png");
									}

									// and load the preview
									image_setlegacyimageloading(true);
									LoadImage(pImageFile, gcstore_dlc[i]);
									image_setlegacyimageloading(false);

									// delay next attempt for a time
									g_dwLoadGameCreatorStorePreviewsTimer = timeGetTime();
								}
							}
						}
						if (ImGui::ImgBtn(TextureID, vPromoSize * 2, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)*vFadeIcons, drawCol_hover*vFadeIcons, drawCol_Down*vFadeIcons, -1, 0, 0, 0, false, false, false, false, false, false))
						{
							if (gcstore_link[i].Len() > 0)
							{
								ExecuteFile(gcstore_link[i].Get(), "", "", 0);
							}
						}
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click to view this object on the Game Creator Store Website");
					}
					ImGui::EndChild();
					ImGui::Text("");
					fPromoHeight = ImGui::GetCursorPosY() - fPromoHeight;
				}
				else
				{
					ImGui::Text("");
					if (iDisplayLibraryType == 1)
					{
						float fStoreAudioTextHeight = fPromoHeight * 0.5;
						ImGui::SetCursorPos(vCurPos + ImVec2(0, fStoreAudioTextHeight - (fFontSize*2.5)));
						ImGui::TextCenter("GameGuru MAX supports");
						ImGui::TextCenter("WAV,MP3,OGG files.");
					}
				}

				ImGui::SetWindowFontScale(1.4);
				ImGui::SetCursorPos(vCurPos + vYOffsetToButtons);
				if (ImGui::StyleButton("Access Your Store Items", ImVec2(vLogoSize.x, fFontSize*2.0)))
				{
					CloseAllOpenTools();
					extern int iDownloadStoreProgress;
					extern bool bDownloadStoreError;
					extern char cDownloadStoreError[4096];
					iDownloadStoreProgress = 0;
					bDownloadStoreError = false;
					strcpy(cDownloadStoreError, "");
					bDownloadStore_Window = true;
					bPreferences_Window = false;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Log-in and download items purchased from the store");
				ImGui::SetWindowFontScale(1.0f);
				ImGui::NextColumn();

				if (iDisplayLibraryType == 2 || iDisplayLibraryType == 3 || iDisplayLibraryType == 4 || iDisplayLibraryType == 5)
				{
					TextureID = MARKETPLACE_FILLER;
					if (ImageExist(MARKETPLACE_COMMUNITY))
						TextureID = MARKETPLACE_COMMUNITY;

					if (ImGui::ImgBtn(TextureID, vLogoSize, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, false, false, false, false, false))
					{
						ExecuteFile("https://forum.game-guru.com/board/1", "", "", 0);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "GameGuru MAX Community");

					//#### Promo icon section. ####
					vCurPos = ImGui::GetCursorPos();
					fPromoHeight = ImGui::GetCursorPosY();
					for (int i = 0; i < StorePromoItems; i++)
					{
						TextureID = MARKETPLACE_FILLER;
						ImVec4 vFadeIcons = { 1.0,1.0,1.0,0.2 };

						if (ImageExist(community_dlc[i]))
						{
							TextureID = community_dlc[i];
							vFadeIcons = { 1.0,1.0,1.0,1.0 };
						}
						if (ImGui::ImgBtn(TextureID, vPromoSize, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)*vFadeIcons, drawCol_hover*vFadeIcons, drawCol_Down*vFadeIcons, 0, 0, 0, 0, false, false, false, false, false, false))
						{
							if (community_link[i].Len() > 0)
							{
								ExecuteFile(community_link[i].Get(), "", "", 0);
							}
						}
						if (i % 2 == 0) ImGui::SameLine();
					}
					ImGui::Text("");
					fPromoHeight = ImGui::GetCursorPosY() - fPromoHeight;

					ImGui::SetWindowFontScale(1.4);
					ImGui::SetCursorPos(vCurPos + vYOffsetToButtons);
					if (ImGui::StyleButton("Visit GameGuru MAX Community", ImVec2(vLogoSize.x, fFontSize*2.0)))
					{
						DeleteWaypointsAddedToCurrentCursor();
						CloseDownEditorProperties();
						bTriggerCloseEntityWindow = true;
						//bMarketplace_Window = false;
						ExecuteFile("https://forum.game-guru.com/board/1", "", "", 0);
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Visit GameGuru MAX Community");

					ImGui::SetWindowFontScale(1.0);
				}
				else
				{
					if (iDisplayLibraryType == 1)
					{
						TextureID = MARKETPLACE_FILLER;
						if (ImageExist(MARKETPLACE_SHOCKWAVESOUND))
							TextureID = MARKETPLACE_SHOCKWAVESOUND;

						if (ImGui::ImgBtn(TextureID, vLogoSize, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, false, false, false, false, false))
						{
							ExecuteFile("https://www.shockwave-sound.com/a/e43bd272af", "", "", 0);
						}
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click to view the Shockwave Sound Website");

						//#### Promo icon section. ####
						vCurPos = ImGui::GetCursorPos();

						ImGui::Text("");
						float fShockwaveTextHeight = fPromoHeight * 0.5;
						ImGui::SetCursorPos(vCurPos + ImVec2(0, fShockwaveTextHeight - (fFontSize*2.5)));
						ImGui::TextCenter("GameGuru MAX supports");
						ImGui::TextCenter("WAV,MP3,OGG files.");

						ImGui::SetWindowFontScale(1.4);
						ImGui::SetCursorPos(vCurPos + vYOffsetToButtons);
						if (ImGui::StyleButton("Visit Shockwave Sound Store", ImVec2(vLogoSize.x, fFontSize*2.0)))
						{
							DeleteWaypointsAddedToCurrentCursor();
							CloseDownEditorProperties();
							bTriggerCloseEntityWindow = true;
							ExecuteFile("https://www.shockwave-sound.com/a/e43bd272af", "", "", 0);
						}
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click to view the Shockwave Sound Website");

						ImGui::SetWindowFontScale(1.0);
					}
					else
					{
						TextureID = MARKETPLACE_FILLER;
						ImGui::SetWindowFontScale(1.0);
					}
				}

				ImGui::SetWindowFontScale(1.0);

				// completed marketplace
				ImGui::Columns(1);
				ImGui::Indent(-10);
				bImGuiGotFocus = true;
				ImGui::End();
			}


		//###########################
		//#### Free Trial Window ####
		//###########################

		static int iCountingFreeDialogClicks = 0;
		if (refresh_gui_docking == 0)
		{
			ImGui::SetNextWindowSize(ImVec2(68 * ImGui::GetFontSize(), (fMarketplacePanelHeight+2.0f) * ImGui::GetFontSize()), ImGuiCond_Always);
			ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
			bool bTmp = true;
			ImGui::Begin("FreeTrial##FreeTrialWindow", &bTmp, 0);
			ImGui::End();
		}
		else if (bFreeTrial_Window)
		{
			if (!bFreeTrial_Init)
			{
				bFreeTrial_Init = true;
			}
			ImGui::SetNextWindowSize(ImVec2(((90.0f / 4.0f) * 3) * ImGui::GetFontSize(), (fMarketplacePanelHeight + 2.0f) * ImGui::GetFontSize()), ImGuiCond_Always);
			ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
			ImGui::Begin("FreeTrial##FreeTrialWindow", &bFreeTrial_Window, ImGuiWindowFlags_None | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings);
			ImGui::Indent(10);
			ImGui::Text("");
			float fFontSize = ImGui::GetFontSize();
			int icon_size = ImGui::GetFontSize()*3.0;
			ImVec2 VIconSize = { (float)icon_size, (float)icon_size };
			bool bClickAlreadyHandled = false;
			if (ImGui::ImgBtn(TOOL_GOBACK, VIconSize, ImVec4(0, 0, 0, 0), drawCol_normal, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
			{
				bClickAlreadyHandled = true;
				if (g_bFreeTrialNowExitsApp == true)
					PostQuitMessage(0);
				else
					bFreeTrial_Window = false;
			}
			if (ImGui::IsItemHovered() && iSkibFramesBeforeLaunch == 0)
			{
				if (g_bFreeTrialNowExitsApp == true)
					ImGui::SetTooltip("%s", "Exit GameGuru MAX Free Trial Version");
				else
					ImGui::SetTooltip("%s", "Exit Free Trial Window");
			}
			ImGui::SameLine();

			ImVec2 VHeaderSize;
			VHeaderSize = { 730, 128 };
			ImVec2 pOldPos = ImGui::GetCursorPos();
			ImGui::SetCursorPos(pOldPos + ImVec2(78 - 18, -35));
			int iHeaderImgID = FREETRIAL_HEADER;
			if (g_iFreeTrialDaysLeft == 1) iHeaderImgID = FREETRIAL_COUNTER_ONEDAY;
			ImGui::ImgBtn(iHeaderImgID, VHeaderSize, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), 0, 0, 0, 0, false, false, false, false, false, false);
			ImVec2 pAfterPos = ImGui::GetCursorPos();

			// demo countdown numeric 
			ImGui::SetCursorPos(pOldPos + ImVec2(78+568, -4));
			ImGui::ImgBtn(FREETRIAL_COUNTER_BASE, ImVec2(50,64), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), 0, 0, 0, 0, false, false, false, false, false, false);
			ImGui::SetCursorPos(pOldPos + ImVec2(78+338, -9));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
			ImGui::SetWindowFontScale(5.0f);
			// days left in free trial
			char pDays[32];
			sprintf(pDays, "%d", g_iFreeTrialDaysLeft);
			ImGui::TextCenter(pDays);
			ImGui::PopStyleColor();
			ImGui::SetCursorPos(pAfterPos);

			float fContentWidth = ImGui::GetContentRegionAvailWidth();
			float fLogoWidth = fContentWidth - 10;
			float fImageWidth = 460;
			float fImageHeight = 215;
			if (ImageExist(FREETRIAL_BODY))
			{
				fImageWidth = ImageWidth(FREETRIAL_BODY);
				fImageHeight = ImageHeight(FREETRIAL_BODY);
			}
			float fScale = fLogoWidth / fImageWidth;
			ImVec2 vLogoSize = { fLogoWidth , fImageHeight * fScale };
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, -20));
			#ifdef GGMAXEPIC
			if (ImGui::ImgBtn(FREETRIAL_BODY, vLogoSize, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), 0, 0, 0, 0, false, false, false, false, false, false))
			{
				bClickAlreadyHandled = true;
				ExecuteFile("https://store.epicgames.com/", "", "", 0);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click here to go to the Epic Store to buy GameGuru MAX");
			#else
			if (ImGui::ImgBtn(FREETRIAL_BODY, vLogoSize, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1), 0, 0, 0, 0, false, false, false, false, false, false))
			{
				bClickAlreadyHandled = true;
				ExecuteFile("https://store.steampowered.com/app/1247290/GameGuru_MAX/", "", "", 0);
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click here to go to the Steam page to buy GameGuru MAX");
			#endif

			ImGui::SetWindowFontScale(0.5f);
			ImGui::Text("");
			ImGui::SetWindowFontScale(2.8f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.0f, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.6f, 0.1f, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.5f, 0.0f, 1.00f));
			if (ImGui::StyleButton("Buy GameGuru MAX", ImVec2(vLogoSize.x, fFontSize*4.0)))
			{
				bClickAlreadyHandled = true;
				#ifdef GGMAXEPIC
				ExecuteFile("https://store.epicgames.com/", "", "", 0);
				#else
				#ifdef FREETRIALONDISCOUNT
				ExecuteFile("https://store.steampowered.com/bundle/25504/GameGuru_Twin_Pack/", "", "", 0);
				#else
				ExecuteFile("https://store.steampowered.com/app/1247290/GameGuru_MAX/", "", "", 0);
				#endif
				#endif
			}
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			#ifdef GGMAXEPIC
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click here to go to the Epic Store to buy GameGuru MAX");
			#else
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Click here to go to the Steam page to buy GameGuru MAX");
			#endif
			ImGui::SetWindowFontScale(1.0);

			// completed free trial window
			ImGui::Indent(-10);
			bImGuiGotFocus = true;
			ImGui::End();

			// if any click not uised above, close dialog on general principal (clicke doutside of dialog most likely)
			ImGuiIO& io = ImGui::GetIO();
			if (io.MouseReleased[0] > 0 && bClickAlreadyHandled == false)
			{
				iCountingFreeDialogClicks++;
				if (iCountingFreeDialogClicks >= 2)
				{				
					bFreeTrial_Window = false;
				}
			}
		}
		if (bFreeTrial_Window == false)
		{
			iCountingFreeDialogClicks = 0; 
		}

		//#####################
		//#### Info Window ####
		//#####################
		
		iInfoUniqueId = 500001;
		if (refresh_gui_docking == 0) 
		{
			ImGui::SetNextWindowSize(ImVec2(46 * ImGui::GetFontSize(), 32 * ImGui::GetFontSize()), ImGuiCond_Once); //ImGuiCond_FirstUseEver
			ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
			bool bTmp = true;
			ImGui::Begin("Information##InformationWindow", &bTmp, ImGuiWindowFlags_NoDocking);
			ImGui::End();
		}
		else if (bInfo_Window) {
			if (bInfo_Window_First_Run)
			{
				ImGui::SetNextWindowSize(ImVec2(46 * ImGui::GetFontSize(), 32 * ImGui::GetFontSize()), ImGuiCond_Once); //ImGuiCond_FirstUseEver
				ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
				bInfo_Window_First_Run = false;
			}
			if (bInfo_Reload || cInfoImageLast != cInfoImage )
			{
				//Load new image.
				//cInfoImage
				image_setlegacyimageloading(true);
				//  Load editor images
				SetMipmapNum(1); //PE: mipmaps not needed.
				if (GetImageExistEx(INFOIMAGE))
					DeleteImage(INFOIMAGE);
				LoadImage(cInfoImage.Get(), INFOIMAGE);
				if (!GetImageExistEx(INFOIMAGE))
				{
					//Get default information image.
					LoadImage("tutorialbank\\information-default.jpg", INFOIMAGE);
				}
				SetMipmapNum(-1);
				image_setlegacyimageloading(false);
				cInfoImageLast = cInfoImage;
			}
			ImGui::Begin("Information##InformationWindow", &bInfo_Window, ImGuiWindowFlags_NoDocking);

			if (GetImageExistEx(INFOIMAGE))
			{
				float fRegionWidth = ImGui::GetContentRegionAvailWidth();
				float img_w = ImageWidth(INFOIMAGE);
				float img_h = ImageHeight(INFOIMAGE);
				float fRatio = img_h / img_w;
#ifndef REMOVED_EARLYACCESS
				ImGui::ImgBtn(INFOIMAGE, ImVec2(fRegionWidth, fRegionWidth*fRatio), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), 0, 0, 0, 0, false);
			}
			ImGui::TextWrapped(cInfoMessage.Get());
#else
			}

			char newLine[MAX_PATH];
			char textToDisplay[MAX_PATH];
			strcpy(textToDisplay, cInfoMessage.Get());

			ImGui::SetWindowFontScale(1.75f);
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 textSize = ImGui::CalcTextSize(cInfoMessage.Get());

			int iTotalLinesAllowed = windowSize.y / textSize.y;
			iTotalLinesAllowed++;

			// Reduce window size in calculations for margins.
			windowSize.x *= 0.8f;
			
			int iNumberOfLines = textSize.x / windowSize.x;
			// Add another line to accomodate for anything after the decimal place.
			iNumberOfLines++;

			// Work out where cursor should be placed to have the text block centered.
 			ImGui::SetCursorPosY((iTotalLinesAllowed - iNumberOfLines) * 0.5f * textSize.y);

			int iOffset = 0;
			int iLength = cInfoMessage.Len();

			// Work out the target number of characters per line.
			int iIncrement = iLength / iNumberOfLines;
			iIncrement++;

			int iPreviousOffset = 0;
			
			// Split cInfoMessage to display centered text over multiple lines.
			for (int i = 0; i < iNumberOfLines; i++)
			{
				strcpy(newLine, cInfoMessage.Get() + iOffset);
				iPreviousOffset = iOffset;
				iOffset += iIncrement;

				// Find a suitable place to end the line.
				for (int j = iOffset; j < iLength; j++)
				{
					if (textToDisplay[j] == ' ')
					{
						if (textToDisplay[j + 1] == '.' || textToDisplay[j + 1] == ',')
							iOffset++;
						break;
					}
					else if (textToDisplay[j] == '.' || textToDisplay[j] == ',')
						break;
					else
						iOffset++;
				}
				newLine[iOffset - iPreviousOffset ] = 0;
				ImGui::TextCenter(newLine);
				
			}
		
			ImGui::SetWindowFontScale(1.0f);
#endif
			ImGui::End();
		}
		else
		{
			bInfo_Reload = true; //Reload on new run.
		}

		//###############
		//#### About ####
		//###############

			About_Screen();

		//##################
		//#### Importer ####
		//##################

		if (refresh_gui_docking == 0 && !bImporter_Window) 
		{
			//Make sure window is setup in docking space.
			ImGui::Begin("Importer##ImporterWindow", &bImporter_Window, iGenralWindowsFlags);
			ImGui::End();
		}

		imgui_importer_loop();

		//#########################
		//#### Help Menu Image ####
		//#########################
		static bool bReadyToProcessMouse = false;
		if(bHelp_Menu_Image_Window) {
				
			if (GetImageExistEx(HELPMENU_IMAGE)) {
				ImGui::OpenPopup("Help##HelpMenuImage");

				float img_w = ImageWidth(HELPMENU_IMAGE);
				float img_h = ImageHeight(HELPMENU_IMAGE);

				ImGui::SetNextWindowPosCenter(ImGuiCond_Always);

				if (ImGui::BeginPopupModal("Help##HelpMenuImage", &bHelp_Menu_Image_Window, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings )) { //ImGuiWindowFlags_AlwaysAutoResize
					//@Lee if you only want 1:1 pixel remove the below (but it could go outside windows).
					//@Lee if 1:1 is possible it will do it.
					if (img_w > viewPortSize.x || img_h > viewPortSize.y) {
						float fRatio = 1.0f / (img_w / img_h);
						img_w = viewPortSize.x;
						img_h = viewPortSize.x * fRatio;
						if (img_h > viewPortSize.y) {
							float fRatio = 1.0f / (img_h / img_w);
							img_h = viewPortSize.y;
							img_w = viewPortSize.y * fRatio;
						}
					}
					ImGui::ImgBtn(HELPMENU_IMAGE, ImVec2(img_w, img_h), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), 0, 0, 0, 0, false);
					bImGuiGotFocus = true;
					ImGui::EndPopup();
				}

			}
			else {
				bHelp_Menu_Image_Window = false;
			}

			//Close no matter where is clicked.
			ImGuiIO& io = ImGui::GetIO();
			if (ImGui::IsKeyPressed(27)) {
				bHelp_Menu_Image_Window = false;
			}
			if (bReadyToProcessMouse && ImGui::IsMouseReleased(0) ) {
				bHelp_Menu_Image_Window = false;
			}
			if (io.MouseClicked[0] > 0) {
				bReadyToProcessMouse = true; //next frame
			}
		}
		else {
			bReadyToProcessMouse = false;
		}

		//###########################
		//#### Export Standalone ####
		//###########################
		static bool bModalInformation = false;
		if (g_bFreeTrialVersion == true)
		{
			if (bExport_Standalone_Window == true)
			{
				bFreeTrial_Window = true;
				bExport_Standalone_Window = false;
			}
		}
		if (bExport_Standalone_Window) 
		{
			static char cStandalonePath[MAX_PATH] = "\0";
			static int iStandaloneCycle = 0;
			if (cStandalonePath[0] == 0) 
			{
				g.exedir_s = g.myownrootdir_s;
				if(cstr(Right(g.myownrootdir_s.Get(), 1)) == "\\" )
					g.exedir_s += "My Games\\";
				else
					g.exedir_s += "\\My Games\\";
				strcpy(cStandalonePath, g.exedir_s.Get());

				if (strlen(pref.cDefaultStandalonePath) > 0)
				{
					strcpy(cStandalonePath,pref.cDefaultStandalonePath);
				}
			}

			ImGui::OpenPopup("Export: Save Standalone Game##SaveStandaloneWindow");
			ImGui::SetNextWindowSize(ImVec2(43 * ImGui::GetFontSize(), 32 * ImGui::GetFontSize()), ImGuiCond_Once);
			ImGui::SetNextWindowPosCenter(ImGuiCond_Appearing);// ImGuiCond_Once);
			if (ImGui::BeginPopupModal("Export: Save Standalone Game##SaveStandaloneWindow", &bExport_Standalone_Window, 0)) 
			{
				ImGui::Indent(10);
				float col_start = 80.0f;
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

				ImGui::SetWindowFontScale(1.2);
				ImGui::TextWrapped("You can export your game project as a self-contained standalone game that can then be shared and played without the need for GameGuru MAX.");

				ImGui::TextWrapped("When your game has been saved as a standalone game, Windows File Explorer can be used to open your standalone game folder and you can then run your game from that location. You can also zip up the whole folder if you wish to share or sell your game.");
				ImGui::Text("");

				// VR mode
				if (g.gvrmode != 0)
				{
					if (ImGui::Checkbox("Save with Experimental Virtual Reality Mode##setVRModeEnabled", &g_bStandaloneVRMode))
					{
						// VR Mode changed
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip("%s", "Setting your standalone to VR mode will attempt to launch the game on any supported OpenXR device");
					}
				}

				// Option to open folder automatically after saving standalone
				static bool bOpenFolder = false;
				ImGui::Checkbox("Open folder after game has been saved and close GameGuru MAX", &bOpenFolder);
				ImGui::Text("");
				ImGui::SetWindowFontScale(1.0);

				// Save or Cancel button
				ImGui::Indent(-10);
				float save_gadget_size = ImGui::GetFontSize()*16.0;
				float w = ImGui::GetWindowContentRegionWidth();
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (save_gadget_size*0.5), 0.0f));
				ImGui::SetWindowFontScale(1.4);
				if (iStandaloneCycle == 0) 
				{
					extern float g_mapfile_fProgress;
					g_mapfile_fProgress = 0.0f;
					if (ImGui::StyleButton("SAVE STANDALONE", ImVec2(save_gadget_size, 0))) {
						g.exedir_s = cStandalonePath;
						iStandaloneCycle = 1;
					}
				}
				else 
				{
					if (ImGui::StyleButton("CANCEL", ImVec2(save_gadget_size, 0))) {
						iStandaloneCycle = 5;
					}
				}
				ImGui::SetWindowFontScale(1.0);

				ImGui::Text("");

				ImGui::PushID(iInfoUniqueId++);
				if (ImGui::ImgBtn(ICON_INFO, ImVec2(20, 20), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255), ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false)) //, bBoostIconColors
				{
					//PE: We are modal here, so need the special modal information window.
					cInfoMessage = "Games made with GameGuru MAX are designed for PCs with this minimum specification:\n\nWindows 10\nIntel Dual-Core 2GHz or AMD Dual-Core 2GHz CPU\n8 GB Ram\nNVIDIA GeForce GTX960 or similar\nDirectX 11\n30 GB device storage\nDirectX Compatabible Sound Card";
					bModalInformation = true;
					cInfoImage = "";
				}
				ImGui::PopID();
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("%s", "Click For More Information");
				}
				ImGui::SameLine();
				ImGui::TextWrapped("Ensure that anyone you share your standalone game with has a PC system that meets the minimum requirements of GameGuru MAX.");
				ImGui::Text("");

				ImGui::Indent(10);

				//New settings here!
				if (iStandaloneCycle == 0)
				{
					ImGui::SetWindowFontScale(1.2);
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
					ImGui::Text("Choose where you would like your standalone to be saved:");
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
					ImGui::Text("Path");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
					ImGui::SetCursorPos(ImVec2(col_start, ImGui::GetCursorPosY()));

					float path_gadget_size = ImGui::GetFontSize()*2.0;

					ImGui::PushItemWidth(-10 - path_gadget_size);
					ImGui::InputText("##InputPathCCP", &cStandalonePath[0], 250, ImGuiInputTextFlags_ReadOnly);
					if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;
					ImGui::PopItemWidth();

					ImGui::SameLine();
					ImGui::PushItemWidth(path_gadget_size);
					if (ImGui::StyleButton("...##ccppath")) 
					{
						cStr tOldDir = GetDir();
						char * cFileSelected;
						cstr fulldir = cStandalonePath;

						cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_DIR, "All\0*.*\0", fulldir.Get(), NULL);

						SetDir(tOldDir.Get());

						if (cFileSelected && strlen(cFileSelected) > 0) 
						{
							strcpy(cStandalonePath, cFileSelected);
							if (cStandalonePath[strlen(cStandalonePath) - 1] != '\\')
								strcat(cStandalonePath, "\\");
							strcpy(pref.cDefaultStandalonePath, cStandalonePath);
						}
					}
					ImGui::PopItemWidth();
					ImGui::SetWindowFontScale(1.0);
				}

				float fdone = (float)mapfile_savestandalone_getprogress() / 100.0f;
				if (iStandaloneCycle == 1)
				{
					// check if destination export path exists
					if (PathExist(cStandalonePath) == false)
					{
						iStandaloneCycle = 0;
						strcpy(cTriggerMessage, "Save Standalone Aborted - Destination Path Not Exist");
						bTriggerMessage = true;
						bExport_Standalone_Window = false; //Close window.
					}
					else
					{
						// can proceed
						fdone = 0.01f;
					}
				}

				if (fdone > 0.0f) 
				{
					ImGui::SetWindowFontScale(1.2);
					char tmp[32];
					sprintf(tmp, "Progress: %.0f%%", fdone*100.0f);
					ImGui::ProgressBar(fdone, ImVec2(ImGui::GetContentRegionAvail().x - 10, 28), tmp);
					ImGui::SetWindowFontScale(1.0);

					// log save standalone progress
					char pSaveStandaloneLog[256];
					sprintf(pSaveStandaloneLog, "Save Standalone %d : %s", iStandaloneCycle, tmp);
					timestampactivity(0, pSaveStandaloneLog);
				}

				if (iStandaloneCycle == 2)
				{
					// start save standalone creation
					mapfile_savestandalone_start();
					iStandaloneCycle = 3;
				}
				if (iStandaloneCycle == 3)
				{
					// run standalone creation calls
					if (mapfile_savestandalone_continue() == 1)
					{
						// complete standalone creation
						iStandaloneCycle = 4;
					}
				}
				if (iStandaloneCycle == 4)
				{
					// complete standalone creation
					mapfile_savestandalone_finish();
					iStandaloneCycle = 0;
					strcpy(cTriggerMessage, "Save Standalone Done");
					bTriggerMessage = true;
					bExport_Standalone_Window = false; //Close window.				

					void InjectIconToExe(char *icon, char *exe, int intresourcenumber);
					char projectico[MAX_PATH];
					char projectfinal_ico[MAX_PATH];
					strcpy(projectico, "projectbank\\");
					strcat(projectico, Storyboard.gamename);

					strcpy(projectico, projectico);
					strcat(projectico, "\\project256.ico");
					GG_GetRealPath(projectico, 1);
					if (FileExist(projectico))
					{
						t.dest_s = t.exepath_s + t.exename_s + "\\" + t.exename_s + ".exe";
						InjectIconToExe(projectico, t.dest_s.Get(), 1);
						HINSTANCE hinstance = ShellExecuteA(NULL, "open", "ie4uinit.exe", "-show", "", SW_SHOWDEFAULT);
						Sleep(100); //PE: Let it update
					}
					if (bOpenFolder)
					{
						cstr open_folder = cStandalonePath;
						if (g.bUseStoryBoardSetup)
						{
							//Use project name as exename
							if (strlen(Storyboard.gamename) > 0)
							{
								open_folder = open_folder + cstr(Storyboard.gamename) + "\\";
							}
						}
						HINSTANCE hinstance = ShellExecuteA(NULL, "open", open_folder.Get(), "", "", SW_SHOWDEFAULT);
						g_bCascadeQuitFlag = true;
					}
				}
				if (iStandaloneCycle == 5)
				{
					// cancel standalone creation
					mapfile_savestandalone_restoreandclose();
					iStandaloneCycle = 0;
					strcpy(cTriggerMessage, "Save Standalone Cancelled");
					bTriggerMessage = true;
					bExport_Standalone_Window = false; //Close window.
				}
				if (iStandaloneCycle == 1) iStandaloneCycle = 2;

				ImGui::Indent(-10);

				bImGuiGotFocus = true;

				//#######################################################
				//#### Modal Popup Information , must do it this way ####
				//#######################################################

				if (bModalInformation)
				{
					ImGui::OpenPopup("Information##modalinformationwindow");

					ImGui::SetNextWindowSize(ImVec2(46 * ImGui::GetFontSize(), 28 * ImGui::GetFontSize()), ImGuiCond_Once);
					ImGui::SetNextWindowPosCenter(ImGuiCond_Appearing);// ImGuiCond_Once);
					if (ImGui::BeginPopupModal("Information##modalinformationwindow", &bModalInformation, 0))
					{
						ImGui::Indent(10);
						ImGui::SetWindowFontScale(1.75f);
						ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, 4));
						ImGui::TextWrapped(cInfoMessage.Get());
						ImGui::SetWindowFontScale(1.0f);
						ImGui::Indent(-10);
						ImGui::EndPopup();
					}
				}

				ImGui::EndPopup();
			}
		}

		//######################
		//#### Object Tools ####
		//######################
		bool bIsLightProbe = false;
		g_selected_editor_object = NULL;
		g_selected_editor_objectID = 0;
		if (t.widget.pickedObject > 0) 
		{
			if (t.widget.pickedObject < g_iObjectListCount)
			{
				if (g_ObjectList[t.widget.pickedObject])
				{
					if (t.widget.pickedEntityIndex > 0) {

						if (t.entityelement[t.widget.pickedEntityIndex].staticflag)
							g_selected_editor_color = XMSTATICCOLOR;
						else
							g_selected_editor_color = XMDYNAMICCOLOR;
					}
					g_selected_editor_object = g_ObjectList[t.widget.pickedObject];
					g_selected_editor_objectID = t.widget.pickedObject;
				}
			}
		}

		static sObject* g_last_selected_editor_object = NULL;
		if (g_selected_editor_object && g_last_selected_editor_object != g_selected_editor_object)
		{
			g_last_selected_editor_object = g_selected_editor_object;
			ImGui::SetWindowFocus("Object Tools##EntityToolsWindow");

			// Check if we no longer have a group selected and then switch back to the " Current Objects" tab if so
			if (current_selected_group >= 0)
			{
				bool bIsNewObjectInGroup = false;

				for (int j = 0; j < MAXGROUPSLISTS; j++)
				{
					for (int i = 0; i < vEntityGroupList[j].size(); i++)
					{
						if (t.widget.pickedObject == vEntityGroupList[j].at(i).e)
						{
							bIsNewObjectInGroup = true;
							break;
						}
					}
				}
				if (!bIsNewObjectInGroup)
					i_switch_group_tab = 1;
				else
					i_switch_group_tab = 2;
			}
			else
			{
				i_switch_group_tab = 1;
			}
		}

		if (refresh_gui_docking == 0 && !Entity_Tools_Window) 
		{
			//Make sure window is setup in docking space.
			ImGui::Begin("Object Tools##EntityToolsWindow", &Entity_Tools_Window, iGenralWindowsFlags);
			ImGui::End();
		}
		else 
		{
			int iEntityIndex = t.widget.pickedEntityIndex;
			int iActiveObj = t.widget.activeObject;
			bool bUpdateGrideleprof = false;
			if (t.gridentityextractedindex > 0)
			{
				iEntityIndex = t.gridentityextractedindex;
				bUpdateGrideleprof = true;
				if (t.gridentityobj > 0)
					iActiveObj = t.gridentityobj;
			}
			else
			{
				iEntityIndex = t.widget.pickedEntityIndex;
				if (t.widget.activeObject == 0 && t.widget.pickedEntityIndex < t.entityelement.size() )
				{
					if(t.widget.pickedEntityIndex > 0)
						iActiveObj = t.tentityobj = t.entityelement[t.widget.pickedEntityIndex].obj;
					else
					{
						if (1==2 && t.tentitytoselect > 0)
						{
							//Temp enable widget , reset is set in next run of gridedit_mapediting();
							iEntityIndex = t.tentitytoselect;
							t.widget.activeObject = t.entityelement[t.tentitytoselect].obj;
							if (!ObjectExist(t.widget.activeObject))
							{
								t.widget.activeObject = 0;
							}
						}
						iActiveObj = t.widget.activeObject;
					}
				}
			}
			active_tools_obj = iActiveObj;
			active_tools_entity_index = iEntityIndex;
			//PE: Make sure the object we edit are a clone , fix many of the problems where material changes was also going to the master object.
			if (iEntityIndex > 0 && iEntityIndex == t.widget.pickedEntityIndex && iActiveObj > 0)
			{
				sObject* pObject = g_ObjectList[iActiveObj];
				if (pObject)
				{
					for (int iMesh = 0; iMesh < (int)pObject->iMeshCount; iMesh++)
					{
						sMesh* pMesh = pObject->ppMeshList[iMesh];
						if (pMesh)
						{
							if (pMesh->master_wickedmeshindex > 0)
							{
								//PE: Make sure selected object is a clone.
								t.tupdatee = iEntityIndex;
								entity_updateentityobj();
							}
							//PE: only checking first mesh needed.
							break;
						}
					}
				}
			}

			static int iLastActiveEntityIndex = -1, iLastActiveObj = -1;

			if (Entity_Tools_Window && ( current_mode == TOOL_ENTITY || current_mode == TOOL_MARKERS || (t.gridentity > 0 && t.entityprofile[t.gridentity].isebe != 0)  )) 
			{
				if (!g_selected_editor_object && iActiveObj > 0)
				{
					if (g_ObjectList[iActiveObj])
					{
						if (iEntityIndex > 0) {

							if (t.entityelement[iEntityIndex].staticflag)
								g_selected_editor_color = XMSTATICCOLOR;
							else
								g_selected_editor_color = XMDYNAMICCOLOR;
						}
						g_selected_editor_object = g_ObjectList[iActiveObj];
						g_selected_editor_objectID = iActiveObj;
					}
				}

				int iMasterID = t.entityelement[iEntityIndex].bankindex;
				if (bDraggingActive && t.widget.pickedEntityIndex > 0 && t.gridentity > 0)
				{
					//PE: Keep displaying old info, while dragging a gridentity around.
					iMasterID = t.gridentity;
				}

				// determine if a provbe or not
				if(iEntityIndex>0)
					if (t.entityelement[iEntityIndex].eleprof.light.fLightHasProbe >= 50.0f)
						bIsLightProbe = true;

				// detect ANY change inside entityelement (inc eleprof) so can trigger instance cloing of collectables
				bool bSnappedEntityElementCopy = false;
				static entityeleproftype snapshotentityelement;
				if (iEntityIndex > 0)
				{
					memcpy(&snapshotentityelement, &t.entityelement[iEntityIndex].eleprof, sizeof(entityeleproftype));
					bSnappedEntityElementCopy = true;
				}

				bool bWithNoScrollbar = false;
				if (iEntityIndex > 0 && iMasterID > 0 && iActiveObj > 0 && ObjectExist(iActiveObj) && pref.iEnableIdentityProperties )
				{
					ImGui::Begin("Object Tools##EntityToolsWindow", &Entity_Tools_Window, iGenralWindowsFlags | ImGuiWindowFlags_NoScrollbar);
					bWithNoScrollbar = true;
				}
				else
				{
					ImGui::Begin("Object Tools##EntityToolsWindow", &Entity_Tools_Window, iGenralWindowsFlags);
				}

				bool bRunExtractDuplicate = false;
				bool bDuplicate = false;
				bool bChildWindowOpen = false;
				bool bClickedTheLockUnlockButton = false;

				if (iActiveObj > 0)
				{
					bool bIsEBEWidget = false;
					int iEntID = 0;
					if (iEntityIndex > 0)
					{
						iEntID = t.entityelement[iEntityIndex].bankindex;
						if (iEntID > 0)
							if (t.entityprofile[iEntID].isebe != 0)
								bIsEBEWidget = true;
					}
					// rubber band or selected parent
					bool bRealRubberBand = false;
					int iEntityInGroupList = -1;
					if (g.entityrubberbandlist.size() > 0)
					{
						bRealRubberBand = true;
						if (iEntityIndex > 0)
						{
							bool bPartOfParentChildGroup = false;
							editor_rec_checkifindexinparentchain(iEntityIndex, &bPartOfParentChildGroup);
							if (bPartOfParentChildGroup == true)
								bRealRubberBand = false;
						}
					}
					else
					{
						iEntityInGroupList = isEntityInGroupList(iEntityIndex);
					}
					
					bool bToolPosition = false; // widgetPOSObj;
					bool bToolRotation = false; // widgetROTObj;
					bool bToolScale = false; // widgetSCLObj;
					bool bToolProperties = false; // widgetPRPObj;
					bool bToolExtract = false; // widgetDUPObj;
					bool bToolDelete = false; // widgetDELObj;
					bool bToolLock = false; // widgetLCKObj;
					bool bToolEdit = false;
					bool bToolSave = false;
					bool bToolDublicate = false; // 
					bool bToolFindFloor = true; //always on.,

					// show all or just POS
					if (bRealRubberBand == true)
					{
						// Rubber band select POS, DELETE and LOCK only
						bToolPosition = true;
						bToolRotation = true;
						bToolScale = true;
						bToolDelete = true;
						bToolLock = true;

						bToolFindFloor = false;
					}
					else
					{
						// POS, ROT, SCALE, etc
						bToolPosition = true;
						bToolRotation = true;
						bToolScale = true;
						bToolProperties = true;
						bToolExtract = true;
						bToolDelete = true;
						bToolLock = true;

						bToolDublicate = true;

						// hide if EBE widget
						if (bIsEBEWidget == true)
						{
							bToolProperties = false;
							bToolScale = false;
							bToolEdit = true;
						}
					}

					//  hide any buttons and widgets if entity is a 'waypoint zone type'
					t.ttte = iEntityIndex;
					if (t.ttte > 0)
					{
						t.tttwi = t.entityelement[t.ttte].eleprof.trigger.waypointzoneindex;
						if (t.tttwi > 0)
						{
							bToolRotation = false;
							bToolScale = false;
							bToolDublicate = false;
						}
						else if (iEntID > 0)
						{
							if (t.entityprofile[iEntID].islightmarker == 1 || t.entityprofile[iEntID].ischaracter == 1)
							{
								// regular light or character
								bToolScale = false;

								// allow probes to have rotation
								if (bIsLightProbe==true)//t.entityprofile[iEntID].ismarker == 2 && t.entityelement[t.ttte].eleprof.light.fLightHasProbe >= 50.0f)
								{
									bToolRotation = true;
								}
							}
							else if (t.entityprofile[iEntID].ismarker > 0)
							{
								// but allow Particles to have rotation and scale control
								if (t.entityprofile[iEntID].ismarker != 10 )
								{
									bToolRotation = false;
									bToolScale = false;

									// allow the player start marker to be rotated.
									if (t.entityprofile[iEntID].ismarker == 1)
										bToolRotation = true;
								}
							}
						}
					}

					//#############################
					//#### PE: New properties. ####
					//#############################

					int iObject = t.entityelement[iEntityIndex].obj;
					int media_icon_size = 64;
					sObject* pObject = g_ObjectList[iActiveObj];
					int iCollectionQuestIndex = -1;

					if (iEntityIndex > 0 && iMasterID > 0 && ObjectExist(iActiveObj))
					{
						grideleprof_uniqui_id = 35000;

						if (iLastActiveEntityIndex != iEntityIndex || iLastActiveObj != iActiveObj)
						{
							if (iLastActiveObj != 70000 && iLastActiveEntityIndex > 0)
							{
								if (iLastActiveEntityIndex < t.entityelement.size())
								{
									if (t.entityelement[iLastActiveEntityIndex].obj == iLastActiveObj)
									{
										//PE: InstanceObject - convert old one that we edited to a instance id possible.
										t.tupdatee = iLastActiveEntityIndex;
										entity_updateentityobj();
									}
								}
							}
							fpe_current_loaded_script = -1; //Make sure dlua is loaded in next call to DisplayFPEBehavior.
							iLastActiveEntityIndex = iEntityIndex;
							iLastActiveObj = iActiveObj;
							// Ensure that the rotation values are updated when switching objects
							g_bRefreshRotationValuesFromObjectOnce = true;
							g_bRefreshScaleValuesFromObjectOnce = true;
						}
						imgui_set_openproperty_flags(iMasterID);
						if (pref.iEnableIdentityProperties)
						{
							bChildWindowOpen = true;

							if (ImGui::StyleCollapsingHeader("Identity##2", ImGuiTreeNodeFlags_DefaultOpen)) //ImGuiTreeNodeFlags_None
							{
								//Display icon.
								if (pref.iObjectEnableAdvanced == 2)
								{
									// no icon when in compact mode
								}
								else
								{
									if (t.entityprofile[iMasterID].iThumbnailSmall > 0)
									{
										float w = ImGui::GetContentRegionAvailWidth();
										float fRatio = (float)ImageWidth(t.entityprofile[iMasterID].iThumbnailLarge) / (float)ImageHeight(t.entityprofile[iMasterID].iThumbnailLarge);
										if (ImageExist(t.entityprofile[iMasterID].iThumbnailLarge) && fRatio > 1.0)
										{
											float fwidth = media_icon_size * fRatio;
											ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (fwidth*0.5), 0.0f));
											ImGui::ImgBtn(t.entityprofile[iMasterID].iThumbnailLarge, ImVec2(fwidth, media_icon_size), drawCol_back, drawCol_normal, drawCol_normal, drawCol_normal, -1, 0, 0, 0, false);
										}
										else
										{
											ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (media_icon_size*0.5), 0.0f));
											ImGui::ImgBtn(t.entityprofile[iMasterID].iThumbnailSmall, ImVec2(media_icon_size, media_icon_size), drawCol_back, drawCol_normal, drawCol_normal, drawCol_normal, -1, 0, 0, 0, true);
										}
									}
								}

								// special color change when object is a collectable
								bool bObjectIsACollectableAndReadOnlyName = false;
								LPSTR pDescTooltip = t.strarr_s[204].Get();
								bool isProjectGlobal = t.entityelement[iEntityIndex].eleprof.isProjectGlobal;
								if (t.entityelement[iEntityIndex].eleprof.iscollectable != 0 || iCollectionQuestIndex > 0 || isProjectGlobal)
								{
									bObjectIsACollectableAndReadOnlyName = true;
									ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
									if (isProjectGlobal)
										pDescTooltip = "This object has been set as a project global object";
									else if(iCollectionQuestIndex>0)
										pDescTooltip = "This object has been set as a quest giver";
									else
										pDescTooltip = "This object has been set as an item collectable";
								}
								else
								{
									ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
								}

								if (bIsLightProbe == false)
								{
									ImGui::Indent(10);
									t.entityelement[iEntityIndex].eleprof.name_s = imgui_setpropertystring2_v2(t.group, t.entityelement[iEntityIndex].eleprof.name_s.Get(), "", pDescTooltip, bObjectIsACollectableAndReadOnlyName);
									ImGui::Indent(-10);
								}
								ImGui::PopStyleColor();
								ImGui::Separator();
							}
						}
					}

					//PE: Add a child here!
					if (bWithNoScrollbar)
					{
						ImGui::BeginChild("##objectpropertieswithnoscrollbar", ImVec2(0, 0), false, iGenralWindowsFlags);
					}

					static int iLastEntityIndex = -1;
					if (iEntityIndex != iLastEntityIndex)
					{
						iLastEntityIndex = iEntityIndex;
						ImGui::SetScrollY(0);
					}


					//##########################
					//#### Entity Transform ####
					//##########################

					bool bReadOnlyMode = false;
					if (t.entityelement[iEntityIndex].editorlock == 1)
						bReadOnlyMode = true;

					if (bToolPosition || bToolRotation || bToolScale)
					{
						// Object tools headers range from 14-20.
						// Default to the positioning header when selecting an object for the first time.
						if (g_selected_editor_object && g_last_selected_editor_object != g_selected_editor_object && (iLastOpenHeader < 13 || (iLastOpenHeader > 18 && iLastOpenHeader != 28)))
							iLastOpenHeader = 14;

						if (pref.bAutoClosePropertySections && iLastOpenHeader != 14)
							ImGui::SetNextItemOpen(false, ImGuiCond_Always);

						if (pref.iObjectEnableAdvanced == 2)
							ImGui::SetNextItemOpen(true, ImGuiCond_Always);

						if (ImGui::StyleCollapsingHeader("Positioning, Rotating and Scaling", ImGuiTreeNodeFlags_DefaultOpen)) //ImGuiTreeNodeFlags_DefaultOpen
						{
							// header prep
							if (pref.iObjectEnableAdvanced == 2)
							{
								// compact mode allows another component to be regarded as last opened header - keeps posrotscl open
							}
							else
							{
								iLastOpenHeader = 14;
							}
							float w = ImGui::GetWindowContentRegionWidth() - 30.0f;

							// if change these, update the object
							bool bUpdatePosition = false;
							bool bUpdateRoataion = false;
							bool bUpdateScale = false;
							float fPos[3], fScale[3];
							static float fAngle[3];
							static float fOldAngle[3];
							static float fScaleOrg[3];
							static int iLastPickedEntID = -1;
							static float fScaleMul = 100.0f;
							float fOldActiveObjectSX,fOldActiveObjectSY,fOldActiveObjectSZ;

							// Need these buttons for switching the widget type (when its on).
							ImGui::Indent(10);
							if (!pref.iEnableDragDropEntityMode || pref.iEnableDragDropWidgetSelect)
							{
								// widget mode 
								ImGui::TextCenter("Widget Mode");
								float fFontSize = ImGui::GetFontSize();
								int iLockButton = 0; if (pref.iObjectEnableAdvanced == 2) iLockButton = 1;
								float fButtonSize = (w-10) / ((int)bToolPosition + (int)bToolRotation + (int)bToolScale + iLockButton);
								if (bToolPosition)
								{
									ImGui::PushItemWidth(fButtonSize);
									bool bSelected = (t.widget.mode == 0);
									if (bSelected) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);											
									if (ImGui::StyleButton("Position", ImVec2(fButtonSize, 0)))
									{
										t.widget.mode = 0;
										widget_show_widget();
									}
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Position (F2)");							
									ImGui::PopItemWidth();
									if (bSelected) ImGui::PopStyleColor();
								}
								if (bToolRotation)
								{
									ImGui::SameLine();
									ImGui::PushItemWidth(fButtonSize);							
									bool bSelected = (t.widget.mode == 1);
									if (bSelected) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);							
									if (ImGui::StyleButton("Rotation", ImVec2(fButtonSize, 0)))
									{
										t.widget.mode = 1;
										widget_show_widget();
										g_bRefreshRotationValuesFromObjectOnce = true;
									}
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Rotation (F3)");
									ImGui::PopItemWidth();							
									if (bSelected) ImGui::PopStyleColor();
								}
								if (bToolScale)
								{
									ImGui::SameLine();
									ImGui::PushItemWidth(fButtonSize);							
									bool bSelected = (t.widget.mode == 2);
									if (bSelected) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);						
									if (ImGui::StyleButton("Scale", ImVec2(fButtonSize, 0)))
									{
										t.widget.mode = 2;
										widget_show_widget();
									}
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Scale (F4)");						
									ImGui::PopItemWidth();
									if (bSelected) ImGui::PopStyleColor();
								}
								if (iLockButton)
								{
									ImGui::SameLine();
									ImGui::PushItemWidth(fButtonSize);
									bool bSelected = false;
									if (bSelected) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
									LPSTR pLockButtonText = "Lock";
									if (g.entityrubberbandlist.size() == 0 && iEntityIndex > 0 && t.entityelement[iEntityIndex].editorlock == 1) pLockButtonText = "Unlock";
									if (ImGui::StyleButton(pLockButtonText, ImVec2(fButtonSize, 0)))
									{
										bClickedTheLockUnlockButton = true;
									}
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Lock/Unlock Object");
									ImGui::PopItemWidth();
									if (bSelected) ImGui::PopStyleColor();
								}
							}

							if (iEntityIndex != iLastPickedEntID)
							{
								iLastPickedEntID = iEntityIndex;
								fScaleMul = 100.0f;
								fScaleOrg[0] = ObjectScaleX(iActiveObj);
								fScaleOrg[1] = ObjectScaleY(iActiveObj);
								fScaleOrg[2] = ObjectScaleZ(iActiveObj);
							}

							fPos[0] = ObjectPositionX(iActiveObj);
							fPos[1] = ObjectPositionY(iActiveObj);
							fPos[2] = ObjectPositionZ(iActiveObj);
							if (g_bRefreshRotationValuesFromObjectOnce == true)
							{
								// should only snapshot eulers once as otherwise they
								// mess up when pulling from newer quaternion method
								g_bRefreshRotationValuesFromObjectOnce = false;
								if (t.entityprofile[iMasterID].ischaracter == 1)
								{
									// quats are the true rotations of objects, but refresh euler for characters to ONLY use the Y axis
									entity_calculateeuleryfromquat(iEntityIndex);

									// characters are simpler, just the Y angle from the entity properties
									fOldAngle[0] = 0;
									fOldAngle[1] = t.entityelement[iEntityIndex].ry;
									fOldAngle[2] = 0;
									fAngle[0] = 0;
									fAngle[1] = t.entityelement[iEntityIndex].ry;
									fAngle[2] = 0;
								}
								else
								{
									// non characters
									fOldAngle[0] = ObjectAngleX(iActiveObj);
									fOldAngle[1] = ObjectAngleY(iActiveObj);
									fOldAngle[2] = ObjectAngleZ(iActiveObj);
									fAngle[0] = ObjectAngleX(iActiveObj);
									fAngle[1] = ObjectAngleY(iActiveObj);
									fAngle[2] = ObjectAngleZ(iActiveObj);

									// rotations (such as seccam) can be widget rotated JUST on the Y, and we can detect this and use Y-only euler!
									// quats are the true rotations of objects, but refresh euler for characters to ONLY use the Y axis
									if ((fAngle[0] == 180 && fAngle[2] == 180) || (fAngle[0] == -180 && fAngle[2] == -180))
									{
										// so, 100 degrees is 80,180,180 can be converted to 100,0,0
										entity_calculateeuleryfromquat(iEntityIndex);
										fOldAngle[0] = 0;
										fOldAngle[1] = t.entityelement[iEntityIndex].ry;
										fOldAngle[2] = 0;
										fAngle[0] = 0;
										fAngle[1] = t.entityelement[iEntityIndex].ry;
										fAngle[2] = 0;
									}
								}
							}
							ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(0, 0, 0, 0));
							float fPushItemWidth = -2.0;
							ImGui::Indent(-10);

							// store for rubber band position
							float fOldActiveObjectRX = ObjectAngleX(iActiveObj);
							float fOldActiveObjectRY = ObjectAngleY(iActiveObj);
							float fOldActiveObjectRZ = ObjectAngleZ(iActiveObj);

							// LB: only need to show smart buttons if not showing widget
							if (pref.iEnableDragDropWidgetSelect == 0)
							{
								// title for three position modes 
								if (pref.iEnableDragDropEntityMode && pref.iObjectEnableAdvanced != 2)
								{
									LPSTR pEditPositionTitle = "Smart Mode";
									if (iObjectMoveMode == 0) pEditPositionTitle = "Smart Mode - Horizontal Only";
									if (iObjectMoveMode == 1) pEditPositionTitle = "Smart Mode - Vertical Only";
									if (iObjectMoveMode == 2) pEditPositionTitle = "Smart Mode";
									ImGui::TextCenter(pEditPositionTitle);
								}

								// LB: Is not a HOLD action, it must be a single press toggle
								static bool bReadyToChange = true;
								bool bPressTAB = t.inputsys.keytab == 1;
								if (!bPressTAB) bReadyToChange = true;
								if (bReadyToChange && bPressTAB)
								{
									if (t.inputsys.keyshift == 1)
									{
										iObjectMoveMode--;
										if (iObjectMoveMode < 0 || iObjectMoveMode > 2)
											iObjectMoveMode = 2;
									}
									else
									{
										iObjectMoveMode++;
										if (iObjectMoveMode > 2 || iObjectMoveMode < 0)
											iObjectMoveMode = 0;
									}
									bReadyToChange = false; //toggle, wait until tab is released again.
								}

								// center buttons
								ImVec2 padding = { 3.0, 3.0 };
								w = ImGui::GetContentRegionAvail().x - 10.0f;
								float icon_spacer = 10.0f;
								int max_icon_size = 56;
								int control_image_size = 26; //lowest possible icon size.
								float center_icons_numbers = 3.0f;
								if (pref.iObjectEnableAdvanced == 2)
								{
									// compact mode for positioning buttons
									center_icons_numbers = 6.0f;
									icon_spacer = 5.0f;
									max_icon_size = 28.0f;
									control_image_size = 13.0f;
								}
								float control_width = (control_image_size + 3.0) * center_icons_numbers + 6.0;
								int indent = 10;
								if (w > control_width)
								{
									//PE: fit perfectly with window width.
									control_image_size = (w - 20.0) / center_icons_numbers;
									control_image_size -= 4.0; //Padding.
									if (control_image_size > max_icon_size) control_image_size = max_icon_size;
									control_width = (control_image_size + 3.0) * center_icons_numbers + 6.0;
									if (control_image_size == max_icon_size)
									{
										indent = (w*0.5) - (control_width*0.5);
										if (indent < 10)
											indent = 10;
									}
								}
								else
								{
									indent = (w*0.5) - (control_width*0.5);
									if (indent < 10)
										indent = 10;
								}
								if (pref.iObjectEnableAdvanced == 2)
								{
									// smaller buttons dont need too much indent
									indent -= 10;
								}

								ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 4.0f));

								if (pref.iEnableDragDropEntityMode)
								{
									ImGui::Indent(indent);

									if (iObjectMoveMode == 2)
									{
										const ImRect image_bb((ImGui::GetCurrentWindow()->DC.CursorPos - padding), ImGui::GetCurrentWindow()->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
										ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
									}
									if (ImGui::ImgBtn(OBJECT_MOVE_SURFACESCAN, ImVec2(control_image_size, control_image_size), ImVec4(1.0, 1.0, 1.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) {
										iObjectMoveMode = 2;
									}
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("Move object and optionally find surface and orientation");

									ImGui::SameLine();
									ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

									if (iObjectMoveMode == 0)
									{
										const ImRect image_bb((ImGui::GetCurrentWindow()->DC.CursorPos - padding), ImGui::GetCurrentWindow()->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
										ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
									}
									if (ImGui::ImgBtn(OBJECT_MOVE_XZ, ImVec2(control_image_size, control_image_size), ImVec4(1.0, 1.0, 1.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
									{
										iObjectMoveMode = 0;
									}
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("Move object horizontally, keeping current Y coordinate");

									ImGui::SameLine();
									ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

									if (iObjectMoveMode == 1)
									{
										const ImRect image_bb((ImGui::GetCurrentWindow()->DC.CursorPos - padding), ImGui::GetCurrentWindow()->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
										ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
									}
									if (ImGui::ImgBtn(OBJECT_MOVE_Y, ImVec2(control_image_size, control_image_size), ImVec4(1.0, 1.0, 1.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors)) {
										iObjectMoveMode = 1;
									}
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("Move object vertically, keeping current X and Z coordinates");

									// title for stack and lock buttons
									LPSTR pEditPositionTitle = "Stack, Orientation, Lock";
									if(pref.iObjectEnableAdvanced != 2) ImGui::TextCenter(pEditPositionTitle);

									// find center of buttons for second row (repeat code from above - urg)
									if (pref.iObjectEnableAdvanced == 2)
									{
										// tag second row ofr icons to end of first for compact choices
										ImGui::SameLine();
									}
									else
									{
										ImGui::Indent(-(indent));
										center_icons_numbers = 3.0f;
										control_width = (control_image_size + 3.0) * center_icons_numbers + 6.0;
										if (w > control_width)
										{
											control_image_size = (w - 20.0) / center_icons_numbers;
											control_image_size -= 4.0;
											if (control_image_size > max_icon_size) control_image_size = max_icon_size;
											control_width = (control_image_size + 3.0) * center_icons_numbers + 6.0;
											if (control_image_size == max_icon_size)
											{
												indent = (w*0.5) - (control_width*0.5);
												if (indent < 10)
													indent = 10;
											}
										}
										else
										{
											indent = (w*0.5) - (control_width*0.5);
											if (indent < 10)
												indent = 10;
										}
										ImGui::Indent(indent);
									}

									// decide button colors
									ImVec4 buttonColor = ImVec4(1.0, 1.0, 1.0, 1.0);
									ImVec4 hoverColor = ImVec4(0.8, 0.8, 0.8, 0.8);
									if (iObjectMoveMode == 2)
									{
										if (g_iStackToSurfaceMode == 1)
										{
											const ImRect image_bb((ImGui::GetCurrentWindow()->DC.CursorPos - padding), ImGui::GetCurrentWindow()->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
											ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
										}
									}
									else
									{
										buttonColor = ImVec4(0.5, 0.5, 0.5, 0.5);
										hoverColor = ImVec4(0.5, 0.5, 0.5, 0.5);
									}

									if (ImGui::ImgBtn(OBJECT_MOVE_FINDFLOOR, ImVec2(control_image_size, control_image_size), ImVec4(1.0, 1.0, 1.0, 0.0), buttonColor, hoverColor, ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
									{
										if (iObjectMoveMode == 2)
										{
											g_iStackToSurfaceMode = 1 - g_iStackToSurfaceMode;
										}
									}
									if (ImGui::IsItemHovered())
									{
										ImGui::SetTooltip("Find a position beneath or above the object to stack it");
									}

									ImGui::SameLine();
									ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

									buttonColor = ImVec4(1.0, 1.0, 1.0, 1.0);
									hoverColor = ImVec4(0.8, 0.8, 0.8, 0.8);
									if (iObjectMoveMode == 2 && g_iStackToSurfaceMode == 1)
									{
										if (g_iOrientToSurfaceMode == 1)
										{
											const ImRect image_bb((ImGui::GetCurrentWindow()->DC.CursorPos - padding), ImGui::GetCurrentWindow()->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
											ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
										}
									}
									else
									{
										buttonColor = ImVec4(0.5, 0.5, 0.5, 0.5);
										hoverColor = ImVec4(0.5, 0.5, 0.5, 0.5);
									}
									if (ImGui::ImgBtn(OBJECT_MOVE_ORIENTATION, ImVec2(control_image_size, control_image_size), ImVec4(1.0, 1.0, 1.0, 0.0), buttonColor, hoverColor, ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
									{
										if (iObjectMoveMode == 2 && g_iStackToSurfaceMode == 1)
										{
											// toggle orientation mode
											g_iOrientToSurfaceMode = 1 - g_iOrientToSurfaceMode;
										}
									}
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("Orient object inline with angle of any surface we stack onto");

									ImGui::SameLine();
									ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0.0f));

									//OBJECT_MOVE_LOCK
									int iIcon = OBJECT_MOVE_LOCK;
									if (g.entityrubberbandlist.size() == 0 && iEntityIndex > 0 && t.entityelement[iEntityIndex].editorlock == 1) iIcon = OBJECT_MOVE_UNLOCK;
									if (iIcon == OBJECT_MOVE_UNLOCK)
									{
										const ImRect image_bb((ImGui::GetCurrentWindow()->DC.CursorPos - padding), ImGui::GetCurrentWindow()->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
										ImGui::GetCurrentWindow()->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
									}
									if (g.entityrubberbandlist.size() > 0)
									{
										bool bAllLocked = true;
										for (int i = 0; i < g.entityrubberbandlist.size(); i++)
										{
											int e = g.entityrubberbandlist[i].e;
											if (e > 0)
											{
												if (t.entityelement[e].editorlock == 0)
												{
													bAllLocked = false;
													break;
												}
											}
										}
										if (iEntityIndex > 0 && t.entityelement[iEntityIndex].editorlock == 0)
											bAllLocked = false;
										if (bAllLocked)
											iIcon = OBJECT_MOVE_UNLOCK;
									}
									if (ImGui::ImgBtn(iIcon, ImVec2(control_image_size, control_image_size), ImVec4(1.0, 1.0, 1.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
									{
										// entity lock/unlock
										bClickedTheLockUnlockButton = true;
									}
									if (ImGui::IsItemHovered())
									{
										if (iIcon == OBJECT_MOVE_UNLOCK)
										{
											if (g.entityrubberbandlist.size() == 0)
												ImGui::SetTooltip("Unlock object to allow it to be moved again");
											else
												ImGui::SetTooltip("Unlock all objects to allow them to be moved again");
										}
										else
										{
											if (g.entityrubberbandlist.size() == 0)
												ImGui::SetTooltip("Lock object to prevent it from being moved");
											else
												ImGui::SetTooltip("Lock all objects to prevent them from being moved");
										}
									}

									ImGui::Indent(-(indent));
								}
								bObjectAllowOverlapping = 1;
							}
							else
							{
								// lock/unlock code repeated above too for non-widget version
								if (pref.iEnableDragDropEntityMode && pref.iObjectEnableAdvanced != 2)
								{
									// is locked or unlocked
									int iIcon = OBJECT_MOVE_LOCK;
									if (g.entityrubberbandlist.size() == 0 && iEntityIndex > 0 && t.entityelement[iEntityIndex].editorlock == 1) iIcon = OBJECT_MOVE_UNLOCK;
									if (g.entityrubberbandlist.size() > 0)
									{
										bool bAllLocked = true;
										for (int i = 0; i < g.entityrubberbandlist.size(); i++)
										{
											int e = g.entityrubberbandlist[i].e;
											if (e > 0)
											{
												if (t.entityelement[e].editorlock == 0)
												{
													bAllLocked = false;
													break;
												}
											}
										}
										if (iEntityIndex > 0 && t.entityelement[iEntityIndex].editorlock == 0) bAllLocked = false;
										if (bAllLocked) iIcon = OBJECT_MOVE_UNLOCK;
									}

									// show lock and unlock button
									ImGui::Indent(10);
									float fFontSize = ImGui::GetFontSize();
									float fButtonSize = w / 3;
									ImGui::PushItemWidth(fButtonSize);
									LPSTR pLockUnlockTitle = "Lock Object";
									if (g.entityrubberbandlist.size() > 0)
									{
										pLockUnlockTitle = "Lock Objects";
										if (iIcon == OBJECT_MOVE_UNLOCK) pLockUnlockTitle = "Unlock Objects";
									}
									else
									{
										if (iIcon == OBJECT_MOVE_UNLOCK) pLockUnlockTitle = "Unlock Object";
									}

									int control_image_size = 56;
									ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w * 0.5) - (control_image_size * 0.5) + 7.0f , 4.0f));
									if (ImGui::ImgBtn(iIcon, ImVec2(control_image_size, control_image_size), ImVec4(1.0, 1.0, 1.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
									{
										// entity lock/unlock
										bClickedTheLockUnlockButton = true;
									}
									if (ImGui::IsItemHovered())
									{
										if (iIcon == OBJECT_MOVE_UNLOCK)
										{
											if (g.entityrubberbandlist.size() == 0)
												ImGui::SetTooltip("Unlock object to allow it to be moved again");
											else
												ImGui::SetTooltip("Unlock all objects to allow them to be moved again");
										}
										else
										{
											if (g.entityrubberbandlist.size() == 0)
												ImGui::SetTooltip("Lock object to prevent it from being moved");
											else
												ImGui::SetTooltip("Lock all objects to prevent them from being moved");
										}
									}
									ImGui::PopItemWidth();
									ImGui::Indent(-10);
								}
							}

							if (bReadOnlyMode)
							{
								//PE: Disable ALL gadgets and moving/rotation/scaling.
								ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
								ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
							}

							// input field size
							float inputsize = w / 3.0f;
							inputsize -= 10.0f; //For text.
							inputsize -= 5.0f; //For padding.

							ImVec2 vStorePos = ImGui::GetCursorPos();

							if (pref.iObjectEnableAdvanced || !pref.iEnableDragDropEntityMode)
							{
								if(pref.iObjectEnableAdvanced != 2) ImGui::TextCenter("Position");

								// X Y Z layout
								ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(8.0f, 3.0f));
								ImGui::Text("PX");
								ImGui::SameLine();
								ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, -3.0f));
								ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
								if (ImGui::InputFloat("##XYZpositionX", &fPos[0], 0.0f, 0.0f, "%.1f")) 	bUpdatePosition = true;
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Object Position X");
								ImGui::PopItemWidth();
								ImGui::SameLine();
								ImGui::Text("PY");
								ImGui::SameLine();
								ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
								if (ImGui::InputFloat("##XYZpositionY", &fPos[1], 0.0f, 0.0f, "%.1f")) 	bUpdatePosition = true;
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Object Position Y");
								ImGui::PopItemWidth();
								ImGui::SameLine();
								ImGui::Text("PZ");
								ImGui::SameLine();
								ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
								if (ImGui::InputFloat("##XYZpositionZ", &fPos[2], 0.0f, 0.0f, "%.1f")) 	bUpdatePosition = true;
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Object Position Z");
								ImGui::PopItemWidth();
							}

							if (bReadOnlyMode)
							{
								ImGui::PopItemFlag();
								ImGui::PopStyleVar();
							}

							if (bReadOnlyMode)
							{
								//PE: Disable ALL gadgets and moving/rotation/scaling.
								ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
								ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
							}

							// rotation stuff
							if (fAngle[0] > 360.0) fAngle[0] -= 360.0f;
							if (fAngle[0] < 0.0) fAngle[0] += 360.0f;
							if (fAngle[1] > 360.0) fAngle[1] -= 360.0f;
							if (fAngle[1] < 0.0) fAngle[1] += 360.0f;
							if (fAngle[2] > 360.0) fAngle[2] -= 360.0f;
							if (fAngle[2] < 0.0) fAngle[2] += 360.0f;

							bool bIsStartMarker = false;
							// Player start marker should still allow Y-axis rotations
							if (t.entityprofile[t.entityelement[t.ttte].bankindex].ismarker == 1)
								bIsStartMarker = true;

							if (!bToolRotation && !bIsStartMarker)
							{
								ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
								ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);		
							}
							if (pref.iObjectEnableAdvanced == 2)
							{
								// compact rotation layout
								bool bChangeYAngleOnly = false;
								if (g.entityrubberbandlist.size() > 0 || iEntityInGroupList >= 0) bChangeYAngleOnly = true;
								if (t.entityprofile[t.entityelement[t.ttte].bankindex].ischaracter || bIsStartMarker) bChangeYAngleOnly = true;

								// X Y Z layout
								if (bChangeYAngleOnly == true)
								{
									ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
									ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
								}
								ImGui::SetCursorPos(ImVec2(vStorePos.x, ImGui::GetCursorPos().y) + ImVec2(8.0f, 3.0f));
								ImGui::Text("RX");
								ImGui::SameLine();
								ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, -3.0f));
								ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
								if (bChangeYAngleOnly == true)
								{
									float fZero = 0.0f;
									ImGui::InputFloat("##Xrotation", &fZero, 0.0f, 0.0f, "%.1f");
								}
								else
								{
									if (ImGui::InputFloat("##Xrotation", &fAngle[0], 0.0f, 0.0f, "%.1f")) 	bUpdateRoataion = true;
								}
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Object Rotation X");
								ImGui::PopItemWidth();
								ImGui::SameLine();
								if (bChangeYAngleOnly == true)
								{
									ImGui::PopItemFlag();
									ImGui::PopStyleVar();
								}
								ImGui::Text("RY");
								ImGui::SameLine();
								ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
								if (ImGui::InputFloat("##Yrotation", &fAngle[1], 0.0f, 0.0f, "%.1f")) 	bUpdateRoataion = true;
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Object Rotation Y");
								ImGui::PopItemWidth();
								ImGui::SameLine();
								if (bChangeYAngleOnly == true)
								{
									ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
									ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
								}
								ImGui::Text("RZ");
								ImGui::SameLine();
								ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
								if (bChangeYAngleOnly == true)
								{
									float fZero = 0.0f;
									ImGui::InputFloat("##Zrotation", &fZero, 0.0f, 0.0f, "%.1f");
								}
								else
								{
									if (ImGui::InputFloat("##Zrotation", &fAngle[2], 0.0f, 0.0f, "%.1f")) 	bUpdateRoataion = true;
								}
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Object Rotation Z");
								ImGui::PopItemWidth();
								if (bChangeYAngleOnly == true)
								{
									ImGui::PopItemFlag();
									ImGui::PopStyleVar();
								}
							}
							else
							{
								ImGui::Indent(10);
								if (g.entityrubberbandlist.size() > 0 || iEntityInGroupList >= 0)
								{
									ImGui::TextCenter("Rotation Angle Y");
									if (ImGui::MaxSliderInputFloat("##Yrotation", &fAngle[1], 0.0f, 359.0f, "Adjust Object Rotation Y", 0, 359))
									{
										if (fAngle[1] > 359.0f) fAngle[1] -= 360.0f;
										bUpdateRoataion = true;
									}
								}
								else
								{
									ImGui::TextCenter("Rotation Angle Y");
									if (ImGui::MaxSliderInputFloat("##Yrotation", &fAngle[1], 0.0f, 359.0f, "Adjust Object Rotation Y", 0, 359))
									{
										if (fAngle[1] > 359.0f) fAngle[1] -= 360.0f;
										bUpdateRoataion = true;
									}
									if (t.entityprofile[t.entityelement[t.ttte].bankindex].ischaracter || bIsStartMarker)
									{
										ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
										ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
									}
									if (pref.iObjectEnableAdvanced)
									{
										ImGui::TextCenter("Rotation Angle X");
										if (ImGui::MaxSliderInputFloat("##Xrotation", &fAngle[0], 0.0f, 359.0f, "Adjust Object Rotation X", 0, 359))
										{
											if (fAngle[0] > 359.0f) fAngle[0] -= 360.0f;
											bUpdateRoataion = true;
										}
										bool bIsLight = false;
										if (!bIsLight)
										{
											ImGui::TextCenter("Rotation Angle Z");
											if (ImGui::MaxSliderInputFloat("##Zrotation", &fAngle[2], 0.0f, 359.0f, "Adjust Object Rotation Z", 0, 359))
											{
												if (fAngle[2] > 359.0f) fAngle[2] -= 360.0f;
												bUpdateRoataion = true;
											}
										}
									}
									if (t.entityprofile[t.entityelement[t.ttte].bankindex].ischaracter || bIsStartMarker)
									{
										ImGui::PopItemFlag();
										ImGui::PopStyleVar();
									}
								}
							}
							if (!bToolRotation && !bIsStartMarker)
							{
								ImGui::PopItemFlag();
								ImGui::PopStyleVar();
							}

							static std::vector<std::array<float, 3>> startRotations;
							static std::vector<std::array<int, 1>> startQuatRotationMode;
							static std::vector<std::array<float, 4>> startQuatRotations;
							static std::vector <std::array<float, 3>> startPositions;
							static bool bStartedRotationUpdate = false;

							if (bUpdateRoataion && ImGui::GetIO().MouseClicked[0])
							{
								// Store initial rotations before any have been applied.
								bStartedRotationUpdate = true;
								if (g.entityrubberbandlist.size() == 0)
								{
									std::array<float, 3> prevRotation = { t.entityelement[t.ttte].rx, t.entityelement[t.ttte].ry, t.entityelement[t.ttte].rz };
									startRotations.push_back(prevRotation);
									std::array<float, 4> prevQuatRotation = { t.entityelement[t.ttte].quatx, t.entityelement[t.ttte].quaty, t.entityelement[t.ttte].quatz, t.entityelement[t.ttte].quatw };
									startQuatRotations.push_back(prevQuatRotation);
									std::array<int, 1> prevQuatRotationMode = { t.entityelement[t.ttte].quatmode };
									startQuatRotationMode.push_back(prevQuatRotationMode);
								}
								else
								{
									for (int i = 0; i < g.entityrubberbandlist.size(); i++)
									{
										int e = g.entityrubberbandlist[i].e;
										std::array<float, 3> prevRotation = { t.entityelement[e].rx, t.entityelement[e].ry, t.entityelement[e].rz };
										startRotations.push_back(prevRotation);
										std::array<float, 4> prevQuatRotation = { t.entityelement[e].quatx, t.entityelement[e].quaty, t.entityelement[e].quatz, t.entityelement[e].quatw };
										startQuatRotations.push_back(prevQuatRotation);
										std::array<int, 1> prevQuatRotationMode = { t.entityelement[e].quatmode };
										startQuatRotationMode.push_back(prevQuatRotationMode);
										// Need to store positions for rubberband, since they rotate about a point. 
										std::array<float, 3> prevPosition = { t.entityelement[e].x, t.entityelement[e].y, t.entityelement[e].z };
										startPositions.push_back(prevPosition);
									}
								}				
							}

							if (bStartedRotationUpdate && ImGui::GetIO().MouseReleased[0])
							{
								// Pass the initial rotations to the undo system.
								if (g.entityrubberbandlist.size() == 0)
								{
									undosys_object_changeposrotscl(t.ttte, t.entityelement[t.ttte].x, t.entityelement[t.ttte].y, t.entityelement[t.ttte].z, 
										startRotations[0][0], startRotations[0][1], startRotations[0][2], 
										startQuatRotationMode[0][0], startQuatRotations[0][0], startQuatRotations[0][1], startQuatRotations[0][2], startQuatRotations[0][3],
										t.entityelement[t.ttte].scalex,	t.entityelement[t.ttte].scaley, t.entityelement[t.ttte].scalez);
								}
								else
								{
									undosys_multiplevents_start();
									for (int i = 0; i < startPositions.size(); i++)
									{
										int e = g.entityrubberbandlist[i].e;
										undosys_object_changeposrotscl(e, startPositions[i][0], startPositions[i][1], startPositions[i][2], 
											startRotations[i][0], startRotations[i][1], startRotations[i][2],
											startQuatRotationMode[i][0], startQuatRotations[i][0], startQuatRotations[i][1], startQuatRotations[i][2], startQuatRotations[i][3],
											t.entityelement[e].scalex, t.entityelement[e].scaley, t.entityelement[e].scalez);
									}
									undosys_multiplevents_finish();
								}

								bStartedRotationUpdate = false;

								startRotations.clear();
								startPositions.clear();
							}

							if (bReadOnlyMode)
							{
								ImGui::PopItemFlag();
								ImGui::PopStyleVar();
							}

							if (bReadOnlyMode)
							{
								//PE: Disable ALL gadgets and moving/rotation/scaling.
								ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
								ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
							}

							ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 4.0f));

							// scale stuff
							float fImporterScaleMultiply = 1.0f;
							fScale[0] = ObjectScaleX(iActiveObj);
							fScale[1] = ObjectScaleY(iActiveObj);
							fScale[2] = ObjectScaleZ(iActiveObj);

							// store old active object scales for rubber band
							if (g_bRefreshScaleValuesFromObjectOnce)
							{
								bUpdateScale = true;
								g_bRefreshScaleValuesFromObjectOnce = false;
							}
							fOldActiveObjectSX = ObjectScaleX(iActiveObj);
							fOldActiveObjectSY = ObjectScaleY(iActiveObj);
							fOldActiveObjectSZ = ObjectScaleZ(iActiveObj);

							if (!bToolScale)
							{
								ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
								ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
							}

							// Disable advanced scaling options for particles
							bool enableSettings = true;
							if (t.ttte > 0)
							{
								int entid = t.entityelement[t.ttte].bankindex;
								if (entid > 0)
								{
									if (t.entityprofile[entid].ismarker == 10)
									{
										enableSettings = false; 
									}
								}
							}

							if (pref.iObjectEnableAdvanced == 2)
							{
								// compact scale layout
								// X Y Z layout
								ImGui::SetCursorPos(ImVec2(vStorePos.x, ImGui::GetCursorPos().y) + ImVec2(8.0f, 3.0f));
								ImGui::Text("SX");
								ImGui::SameLine();
								ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, -3.0f));
								ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
								bool bChangedAScaleValue = false;
								if (ImGui::InputFloat("##Xscale", &fScale[0], 0.0f, 0.0f, "%.1f")) 	bChangedAScaleValue = true;
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Object Scale X");
								ImGui::PopItemWidth();
								ImGui::SameLine();
								ImGui::Text("SY");
								ImGui::SameLine();
								ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
								if (ImGui::InputFloat("##Yscale", &fScale[1], 0.0f, 0.0f, "%.1f")) 	bChangedAScaleValue = true;
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Object Scale Y");
								ImGui::PopItemWidth();
								ImGui::SameLine();
								ImGui::Text("SZ");
								ImGui::SameLine();
								ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
								if (ImGui::InputFloat("##Zscale", &fScale[2], 0.0f, 0.0f, "%.1f")) 	bChangedAScaleValue = true;
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Object Scale Z");
								ImGui::PopItemWidth();
								if (bChangedAScaleValue == true)
								{
									ScaleObject(iActiveObj, fScale[0], fScale[1], fScale[2]);
									fScaleOrg[0] = ObjectScaleX(iActiveObj);
									fScaleOrg[1] = ObjectScaleY(iActiveObj);
									fScaleOrg[2] = ObjectScaleZ(iActiveObj);
									fScaleMul = 0.0f;
									if (t.ttte > 0)
									{
										bUpdateScale = true;
									}
								}

								ImGui::Indent(10);
							}
							else
							{
								ImGui::PushItemWidth(fPushItemWidth);
								ImGui::TextCenter("Scale Multiplier Percentage");
								if (ImGui::MaxSliderInputFloat("##EntityScaleAll", &fScaleMul, 0.0f, 1000.0f, "Use this slider to multiply or divide the scale on all three axis", 0, 1000))
								{
									float fSm = fScaleMul / 100.0f;
									// In certain cases, original scale values can be wiped out to 0, causing object to 'disappear' when this slider is used
									if (fScaleOrg[0] == 0 && fScaleOrg[1] == 0 && fScaleOrg[2] == 0)
									{
										fScaleOrg[0] = ObjectScaleX(iActiveObj);
										fScaleOrg[1] = ObjectScaleY(iActiveObj);
										fScaleOrg[2] = ObjectScaleZ(iActiveObj);
									}
									ScaleObject(iActiveObj, fScaleOrg[0] * fSm, fScaleOrg[1] * fSm, fScaleOrg[2] * fSm);
									if (t.ttte > 0)
									{
										bUpdateScale = true;
									}
								}
								if (pref.iObjectEnableAdvanced)
								{
									if (enableSettings)
									{
										ImGui::TextCenter("Scale X");
										if (ImGui::MaxSliderInputFloat("##XScaleOnly", &fScale[0], 0.0f, 1000.0f, "Scale X axis only", 0, 1000))
										{
											ScaleObject(iActiveObj, fScale[0], fScale[1], fScale[2]);
											//Need to update org.
											fScaleOrg[0] = ObjectScaleX(iActiveObj);
											fScaleOrg[1] = ObjectScaleY(iActiveObj);
											fScaleOrg[2] = ObjectScaleZ(iActiveObj);
											fScaleMul = 0.0f;
											if (t.ttte > 0)
											{
												bUpdateScale = true;
											}
										}
										ImGui::TextCenter("Scale Y");
										if (ImGui::MaxSliderInputFloat("##YScaleOnly", &fScale[1], 0.0f, 1000.0f, "Scale Y axis only", 0, 1000))
										{
											ScaleObject(iActiveObj, fScale[0], fScale[1], fScale[2]);
											//Need to update org.
											fScaleOrg[0] = ObjectScaleX(iActiveObj);
											fScaleOrg[1] = ObjectScaleY(iActiveObj);
											fScaleOrg[2] = ObjectScaleZ(iActiveObj);
											fScaleMul = 0.0f;
											if (t.ttte > 0)
											{
												bUpdateScale = true;
											}
										}
										ImGui::TextCenter("Scale Z");
										if (ImGui::MaxSliderInputFloat("##ZScaleOnly", &fScale[2], 0.0f, 1000.0f, "Scale Z axis only", 0, 1000))
										{
											ScaleObject(iActiveObj, fScale[0], fScale[1], fScale[2]);
											//Need to update org.
											fScaleOrg[0] = ObjectScaleX(iActiveObj);
											fScaleOrg[1] = ObjectScaleY(iActiveObj);
											fScaleOrg[2] = ObjectScaleZ(iActiveObj);
											fScaleMul = 0.0f;
											if (t.ttte > 0)
											{
												bUpdateScale = true;
											}
										}
									}
								}
								ImGui::PopItemWidth();
							}
							if (!bToolScale)
							{
								ImGui::PopItemFlag();
								ImGui::PopStyleVar();
							}

							if (bReadOnlyMode)
							{
								ImGui::PopItemFlag();
								ImGui::PopStyleVar();
							}

							if (bIsLightProbe == false)
							{
								// random spray object mode
								ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 4.0f));
								ImGui::PushItemWidth(fPushItemWidth);
								bool bSpray = t.gridedit.entityspraymode;
								if (ImGui::Checkbox("Randomly Spray Objects", &bSpray))
								{
								}
								t.gridedit.entityspraymode = bSpray;
								if (t.gridedit.entityspraymode)
								{
									ImGui::TextCenter("Spray Radius");
									if (ImGui::MaxSliderInputInt("##SetSpray Radius", &t.gridedit.entitysprayrange, 30, 1000, "Set Spray Radius"))
									{
										iDisplayCircleFrames = 20;
									}
									if (ImGui::Checkbox("Move Spray Center With Mouse", &bSprayMoveWithMouse))
									{
									}

								}
								ImGui::PopItemWidth();
								static bool bOldSprayMode = t.gridedit.entityspraymode;
								static uint32_t oldflag = 0;
								static float oldbrushSize = 0;
								if (t.gridedit.entityspraymode)
								{
									if (bOldSprayMode != t.gridedit.entityspraymode)
									{
										oldflag = ggterrain_global_render_params2.flags2;
										oldbrushSize = ggterrain_global_render_params2.brushSize;
										bOldSprayMode = t.gridedit.entityspraymode;
									}
									void set_terrain_sculpt_mode(int mode);
									void set_terrain_edit_mode(int mode);
									set_terrain_sculpt_mode(0);
									set_terrain_edit_mode(0);
									ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_BRUSH_SIZE;
									ggterrain_global_render_params2.brushSize = (float)t.gridedit.entitysprayrange * 1.5f; //PE: A bit larger.
								}
								else
								{
									if (bOldSprayMode != t.gridedit.entityspraymode)
									{
										if (oldflag != 0) ggterrain_global_render_params2.flags2 = oldflag;
										if (oldbrushSize != 0) ggterrain_global_render_params2.brushSize = oldbrushSize;
										bOldSprayMode = t.gridedit.entityspraymode;
									}
								}
							}

							// advanced toggle
							if (pref.iObjectEnableAdvanced != 2)
							{
								ControlAdvancedSetting(pref.iObjectEnableAdvanced, "Advanced Object Tools");
							}

							ImGui::PopStyleColor();
							ImGui::Indent(-10);

							// update probe itself if changed
							if (bIsLightProbe == true)
							{
								if (!bReadOnlyMode && bUpdatePosition == true) g_bLightProbeScaleChanged = true;
								if (!bReadOnlyMode && bUpdateRoataion == true) g_bLightProbeScaleChanged = true;
							}

							// update any position, rotation or scale changes
							if (!bReadOnlyMode && bUpdatePosition == true)
							{
								// work out difference for rubber band positioning
								float fOldActiveObjectX = ObjectPositionX(iActiveObj);
								float fOldActiveObjectY = ObjectPositionY(iActiveObj);
								float fOldActiveObjectZ = ObjectPositionZ(iActiveObj);
								if (iEntityInGroupList >= 0)
								{
									//Add all groups with entity to rubberband.
									CheckGroupListForRubberbandSelections(t.ttte);
								}
								else if (g.entityrubberbandlist.size() > 0)
								{
									//Make sure all groups is selected from within rubberband selecting.
									for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
									{
										int e = g.entityrubberbandlist[i].e;
										CheckGroupListForRubberbandSelections(e);
									}
								}

								// move the target entity
								PositionObject(iActiveObj, fPos[0], fPos[1], fPos[2]);
								t.entityelement[t.ttte].x = ObjectPositionX(iActiveObj);
								t.entityelement[t.ttte].y = ObjectPositionY(iActiveObj);
								t.entityelement[t.ttte].z = ObjectPositionZ(iActiveObj);

								// if we need to also move rubber band highlighted objects, do so now
								if (g.entityrubberbandlist.size() > 0)
								{
									float fMovedActiveObjectX = ObjectPositionX(iActiveObj) - fOldActiveObjectX;
									float fMovedActiveObjectY = ObjectPositionY(iActiveObj) - fOldActiveObjectY;
									float fMovedActiveObjectZ = ObjectPositionZ(iActiveObj) - fOldActiveObjectZ;
									for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
									{
										int e = g.entityrubberbandlist[i].e;
										int tobj = t.entityelement[e].obj;
										if (tobj > 0 && t.entityelement[e].editorlock == 0)
										{
											if (ObjectExist(tobj) == 1)
											{
												if (tobj != iActiveObj)
												{
													// reposition this entity
													PositionObject(tobj, ObjectPositionX(tobj) + fMovedActiveObjectX, ObjectPositionY(tobj) + fMovedActiveObjectY, ObjectPositionZ(tobj) + fMovedActiveObjectZ);
													t.entityelement[e].x = ObjectPositionX(tobj);
													t.entityelement[e].y = ObjectPositionY(tobj);
													t.entityelement[e].z = ObjectPositionZ(tobj);
													if (t.entityelement[e].staticflag == 1) g.projectmodifiedstatic = 1;
													widget_movezonesandlights(e);
												}
											}
										}
									}
								}
							}
							if (!bReadOnlyMode && bUpdateRoataion)
							{
								if (iEntityInGroupList >= 0)
								{
									//Add all groups with entity to rubberband.
									CheckGroupListForRubberbandSelections(t.ttte);
								}
								else if (g.entityrubberbandlist.size() > 0)
								{
									//Make sure all groups is selected from within rubberband selecting.
									for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
									{
										int e = g.entityrubberbandlist[i].e;
										CheckGroupListForRubberbandSelections(e);
									}
								}

								// rotate the target entity now (one way euler values)
								int iObj = iActiveObj;
								if (ObjectExist(iObj) == 1)
								{
									// rotation event
									float fMoveAngleX = fAngle[0] - fOldAngle[0];
									float fMoveAngleY = fAngle[1] - fOldAngle[1];
									float fMoveAngleZ = fAngle[2] - fOldAngle[2];
									fOldAngle[0] = fAngle[0];
									fOldAngle[1] = fAngle[1];
									fOldAngle[2] = fAngle[2];
									GGQUATERNION quatRotationEvent, QuatAroundX, QuatAroundY, QuatAroundZ;
									GGQuaternionRotationAxis(&QuatAroundX, &GGVECTOR3(1, 0, 0), GGToRadian(fMoveAngleX));
									GGQuaternionRotationAxis(&QuatAroundY, &GGVECTOR3(0, 1, 0), GGToRadian(fMoveAngleY));
									GGQuaternionRotationAxis(&QuatAroundZ, &GGVECTOR3(0, 0, 1), GGToRadian(fMoveAngleZ));
									quatRotationEvent = QuatAroundX * QuatAroundY * QuatAroundZ;

									// current orientation from eulers
									// fundamentally, sliders and values for X Y Z and EULER, so can only set them this way!
									if (g.entityrubberbandlist.size() > 0)
									{
										// when object part of selected, use quat and only use "Y" value as a movement delta via rotation event
										GGQUATERNION QuatAroundX, QuatAroundY, QuatAroundZ;
										GGQuaternionRotationAxis(&QuatAroundX, &GGVECTOR3(1, 0, 0), GGToRadian(ObjectAngleX(iObj)));
										GGQuaternionRotationAxis(&QuatAroundY, &GGVECTOR3(0, 1, 0), GGToRadian(ObjectAngleY(iObj)));
										GGQuaternionRotationAxis(&QuatAroundZ, &GGVECTOR3(0, 0, 1), GGToRadian(ObjectAngleZ(iObj)));
										GGQUATERNION quatCurrentOrientation;
										quatCurrentOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
										GGQUATERNION quatNewOrientation;
										GGQuaternionMultiply(&quatNewOrientation, &quatCurrentOrientation, &quatRotationEvent);
										RotateObjectQuat(iObj, quatNewOrientation.x, quatNewOrientation.y, quatNewOrientation.z, quatNewOrientation.w);
										// and also rotate the selected!
										SetStartPositionsForRubberBand(iObj);
										RotateAndMoveRubberBand(iObj, 0, 0, 0, quatRotationEvent);

										// ensure the root object (active object) also gets its rotation!
										if (t.ttte > 0)
										{
											t.entityelement[t.ttte].rx = ObjectAngleX(iActiveObj);
											t.entityelement[t.ttte].ry = ObjectAngleY(iActiveObj);
											t.entityelement[t.ttte].rz = ObjectAngleZ(iActiveObj);
										}
									}
									else
									{
										// when single object, treat as euler X Y Z 
										RotateObject(iObj, fAngle[0], fAngle[1], fAngle[2]);
										if (t.ttte > 0)
										{
											t.entityelement[t.ttte].rx = ObjectAngleX(iActiveObj);
											t.entityelement[t.ttte].ry = ObjectAngleY(iActiveObj);
											t.entityelement[t.ttte].rz = ObjectAngleZ(iActiveObj);
										}
									}

									// update entity quat as the preferred source rotation
									entity_updatequatfromeuler(t.ttte);
								}
							}
							if (!bReadOnlyMode && bUpdateScale == true)
							{
								// scale the target entity
								t.entityelement[t.ttte].scalex = ObjectScaleX(iActiveObj) - 100.0;
								t.entityelement[t.ttte].scaley = ObjectScaleY(iActiveObj) - 100.0;
								t.entityelement[t.ttte].scalez = ObjectScaleZ(iActiveObj) - 100.0;

								if (iEntityInGroupList >= 0)
								{
									//Add all groups with entity to rubberband.
									CheckGroupListForRubberbandSelections(t.ttte);
								}
								else if (g.entityrubberbandlist.size() > 0)
								{
									//Make sure all groups is selected from within rubberband selecting.
									for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
									{
										int e = g.entityrubberbandlist[i].e;
										CheckGroupListForRubberbandSelections(e);
									}
								}

								// if we need to also scale rubber band highlighted objects, do so now
								if (g.entityrubberbandlist.size() > 0)
								{
									float fMovedActiveObjectSX = ObjectScaleX(iActiveObj) - fOldActiveObjectSX;
									float fMovedActiveObjectSY = ObjectScaleY(iActiveObj) - fOldActiveObjectSY;
									float fMovedActiveObjectSZ = ObjectScaleZ(iActiveObj) - fOldActiveObjectSZ;
									for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
									{
										int e = g.entityrubberbandlist[i].e;
										int tobj = t.entityelement[e].obj;
										if (tobj > 0 && t.entityelement[e].editorlock == 0)
										{
											if (ObjectExist(tobj) == 1)
											{
												if (tobj != iActiveObj)
												{
													//LB: oops! if (t.entityprofile[e].ischaracter == 0 && t.entityprofile[e].ismarker == 0)
													int entid = t.entityelement[e].bankindex;
													if (entid > 0)
													{
														if (t.entityprofile[entid].ischaracter == 0 && t.entityprofile[entid].ismarker == 0)
														{
															ScaleObject(tobj, ObjectScaleX(tobj) + fMovedActiveObjectSX, ObjectScaleY(tobj) + fMovedActiveObjectSY, ObjectScaleZ(tobj) + fMovedActiveObjectSZ);
															t.entityelement[e].scalex = ObjectScaleX(tobj) - 100;
															t.entityelement[e].scaley = ObjectScaleY(tobj) - 100;
															t.entityelement[e].scalez = ObjectScaleZ(tobj) - 100;
															if (t.entityelement[e].staticflag == 1) g.projectmodifiedstatic = 1;
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}

					//#############################
					//#### PE: New properties. ####
					//#############################

					if (iEntityIndex > 0 && iMasterID > 0 && ObjectExist(iActiveObj))
					{
						bool bGeneralActive = false;
						if ((t.entityprofile[iMasterID].ismarker == 0 /*|| t.entityprofile[iMasterID].islightmarker == 1 */ || t.tflagspawn == 1))
							bGeneralActive = true;
						if (t.tflagchar == 0 && t.tflagvis == 1 && t.tflagsimpler == 0)
							bGeneralActive = true;

						if (bGeneralActive)
						{
							char title[24] = "General##2";
							if (t.entityprofile[iMasterID].ischaracter == 1)
								strcpy(title, "Character Settings##2");

							if (pref.bAutoClosePropertySections && iLastOpenHeader != 17)
								ImGui::SetNextItemOpen(false, ImGuiCond_Always);

							if (ImGui::StyleCollapsingHeader(title, ImGuiTreeNodeFlags_None)) //ImGuiTreeNodeFlags_DefaultOpen
							{
								iLastOpenHeader = 17;
								DisplayFPEGeneral(false, iMasterID, &t.entityelement[iEntityIndex].eleprof, iEntityIndex);
							}
						}

						bool bMaterialsUsed = true;
						cStr HeaderName = "Behavior##2";
						if (t.entityprofile[iMasterID].ismarker == 1)
						{
							HeaderName = "Customize##2";
						}
						if (t.tflaglight == 1 || t.entityprofile[iMasterID].ismarker == 2)
						{
							if(bIsLightProbe==true)
							{
								HeaderName = "Probe Settings##2";
							}
							else
							{
								HeaderName = "Color Palette##2";
							}
						}
						if (t.entityprofile[iMasterID].ismarker == 10)
						{
							HeaderName = "Particles##2";
							bMaterialsUsed = false;
						}
						if (t.entityprofile[iMasterID].bIsDecal)
						{
							HeaderName = "Decal##99";
						}

						bool bAllowBehaviorChange = true;
						bool bIsThisAnEBE = false;
						if (t.entityprofile[iMasterID].isebe != 0 && t.widget.pickedEntityIndex > 0)
						{
							HeaderName = "Structure Editor Object##99";
							bIsThisAnEBE = true;
						}
						else
						{
							if (t.entityprofile[iMasterID].ismarker == 0)
							{
								if (t.entityelement[iEntityIndex].staticflag != 0)
								{
									// cannot change behavior of a static object!
									bAllowBehaviorChange = false;
								}
							}
						}

						// rubber band awareness
						bool bRubberbandActive = false;
						if (g.entityrubberbandlist.size() > 0) bRubberbandActive = true;
						if (g.entityrubberbandlist.size() == 1 && g.entityrubberbandlist[0].e == iEntityIndex) bRubberbandActive = false;
						if (!bRubberbandActive )
						{
							if (pref.bAutoClosePropertySections && iLastOpenHeader != 16)
								ImGui::SetNextItemOpen(false, ImGuiCond_Always);

							if (ImGui::StyleCollapsingHeader(HeaderName.Get(), ImGuiTreeNodeFlags_DefaultOpen))//ImGuiTreeNodeFlags_None))
							{
								iLastOpenHeader = 16;

								ImGui::Indent(10);
								if (bIsThisAnEBE == true)
								{
									// Edit Structure Editor Object
									float edit_gadget_size = ImGui::GetFontSize()*10.0;
									float w = ImGui::GetWindowContentRegionWidth();
									ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (edit_gadget_size*0.5), 0.0f));
									if (ImGui::StyleButton("Edit Structure Object", ImVec2(edit_gadget_size, 0)))
									{
										t.widget.propertybuttonselected = 1;
										t.ebe.bReleaseMouseFirst = true;
									}
								}
								else
								{
									if (bAllowBehaviorChange == true)
									{
										DisplayFPEBehavior(false, iMasterID, &t.entityelement[iEntityIndex].eleprof, iEntityIndex, false);
									}
									else
									{
										// inform user cannot use behaviors for static objects
										ImGui::TextCenter("No Behavior For Static Objects");
									}
								}
								ImGui::Indent(-10);
							}
						}

						// Collectbale/Quest Settings if required
						int iCollectableSettingsMode = 0;
						LPSTR pCollectySettingsLabel = "Additional Settings##1";
						if (t.entityelement[iEntityIndex].eleprof.iscollectable != 0) { iCollectableSettingsMode = 1; pCollectySettingsLabel = "Collectable Settings##1"; }
						if (iCollectionQuestIndex > 0)
						{
							pCollectySettingsLabel = "Quest Settings##1";
							iCollectableSettingsMode = 2;
						}
						if (iCollectableSettingsMode > 0)
						{
							if (pref.bAutoClosePropertySections && iLastOpenHeader != 28) ImGui::SetNextItemOpen(false, ImGuiCond_Always);
							if (ImGui::StyleCollapsingHeader(pCollectySettingsLabel, ImGuiTreeNodeFlags_None))
							{
								iLastOpenHeader = 28;
								int iCollectionItemIndex = -1;
								if (iCollectableSettingsMode == 1)
								{
									if (t.entityprofile[t.entityelement[iEntityIndex].bankindex].isweapon_s.Len() > 0)
									{
										// weapon style
										cstr pSearchForWeapon = cstr("weapon=") + cstr(t.entityprofile[t.entityelement[iEntityIndex].bankindex].isweapon_s);
										for (int ci = 0; ci < g_collectionList.size(); ci++)
										{
											if (stricmp (g_collectionList[ci].collectionFields[8].Get(), pSearchForWeapon.Get()) == NULL)
											{
												iCollectionItemIndex = ci;
												break;
											}
										}
										if (iCollectionItemIndex == -1)
										{
											// okay, no modern reference, fall back on title to match name of weapon!
											for (int ci = 0; ci < g_collectionList.size(); ci++)
											{
												if (stricmp (g_collectionList[ci].collectionFields[0].Get(), t.entityelement[iEntityIndex].eleprof.name_s.Get()) == NULL)
												{
													iCollectionItemIndex = ci;
													break;
												}
											}
										}
									}
									else
									{
										// regular item
										for (int ci = 0; ci < g_collectionList.size(); ci++)
										{
											if (stricmp (g_collectionList[ci].collectionFields[0].Get(), t.entityelement[iEntityIndex].eleprof.name_s.Get()) == NULL)
											{
												iCollectionItemIndex = ci;
												break;
											}
										}
									}
								}
								if (iCollectableSettingsMode == 2)
								{
									if (iCollectionQuestIndex > 0)
									{
										iCollectionItemIndex = iCollectionQuestIndex - 1;
									}
								}
								if (iCollectionItemIndex == -1)
								{
									// No Master Collection List entry, enable user to create an entry ( will be saved below ) 
									float w = ImGui::GetWindowContentRegionWidth();
									float but_gadget_size = ImGui::GetFontSize() * 15.0;
									ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w * 0.5) - (but_gadget_size * 0.5), 0.0f));
									LPSTR pCreateButtonLabel = "Create Collection Item";
									if (ImGui::StyleButton(pCreateButtonLabel, ImVec2(but_gadget_size, 0)))
									{
										if (iCollectableSettingsMode == 1)
										{
											// create an item entry
											collectionItemType item;
											fill_rpg_item_defaults(&item, iMasterID, iEntityIndex);
											// first, if a weapon, check DESC for matching 'raw weapon path'
											// and if found, overwrite with better item entry created here
											if ( t.entityprofile[iMasterID].isweapon_s.Len()>0)
											{
												for (int n = 0; n < g_collectionList.size(); n++)
												{
													if (item.collectionFields.size() > 3)
													{
														if (g_collectionList[n].collectionFields.size() > 3)
														{
															if (stricmp (g_collectionList[n].collectionFields[3].Get(), t.entityprofile[iMasterID].isweapon_s.Get()) == NULL)
															{
																// overwrite, save and leave
																g_collectionList[n].collectionFields = item.collectionFields;
																g_bChangedGameCollectionList = true;
																break;
															}
														}
													}
												}
											}
											// check if unique
											bool bNewItemIsUnqiue = true;
											for (int n = 0; n < g_collectionList.size(); n++)
											{
												if (item.collectionFields.size() > 0)
												{
													if (g_collectionList[n].collectionFields.size() > 0)
													{
														if (g_collectionList[n].collectionFields[0] == item.collectionFields[0])
														{
															bNewItemIsUnqiue = false;
															break;
														}
													}
												}
											}
											if (bNewItemIsUnqiue == true)
											{
												g_collectionList.push_back(item);
												g_bChangedGameCollectionList = true;
											}
										}
									}
								}
								else
								{
									// show collectable details
									bool bQuestTypeIsCollect = false;
									ImGui::Indent(10);
									int iCount = g_collectionLabels.size();
									if (iCollectableSettingsMode == 2) iCount = g_collectionQuestLabels.size();
									for (int l = 0; l < iCount; l++)
									{
										int iKnownLabel = -1;
										LPSTR pLabel = "";
										if (iCollectableSettingsMode == 1)
										{
											pLabel = g_collectionLabels[l].Get();
										}
										if (iCollectableSettingsMode == 2)
										{
											pLabel = g_collectionQuestLabels[l].Get();
										}
										if (stricmp(pLabel, "title") == NULL) iKnownLabel = 0;
										if (stricmp(pLabel, "image") == NULL) iKnownLabel = 2;
										if (iCollectableSettingsMode == 1)
										{
											if (stricmp(pLabel, "description") == NULL) iKnownLabel = 3;
											if (stricmp(pLabel, "cost") == NULL) iKnownLabel = 4;
											if (stricmp(pLabel, "value") == NULL) iKnownLabel = 5;
											if (stricmp(pLabel, "container") == NULL) iKnownLabel = 6;
											if (stricmp(pLabel, "ingredients") == NULL) iKnownLabel = 7;
											if (stricmp(pLabel, "style") == NULL) iKnownLabel = 8;
										}
										if (iCollectableSettingsMode == 2)
										{
											if (stricmp(pLabel, "type") == NULL) iKnownLabel = 51;
											if (stricmp(pLabel, "desc1") == NULL) iKnownLabel = 52;
											if (stricmp(pLabel, "desc2") == NULL) iKnownLabel = 53;
											if (stricmp(pLabel, "desc3") == NULL) iKnownLabel = 54;
											if (stricmp(pLabel, "object") == NULL) iKnownLabel = 55;
											if (stricmp(pLabel, "receiver") == NULL) iKnownLabel = 56;
											if (stricmp(pLabel, "level") == NULL) iKnownLabel = 57;
											if (stricmp(pLabel, "points") == NULL) iKnownLabel = 58;
											if (stricmp(pLabel, "value") == NULL) iKnownLabel = 59;
											if (stricmp(pLabel, "status") == NULL) iKnownLabel = 60;
											if (stricmp(pLabel, "activate") == NULL) iKnownLabel = 61;
											if (stricmp(pLabel, "quantity") == NULL) iKnownLabel = 62;
											if (stricmp(pLabel, "endmap") == NULL) iKnownLabel = 63;
										}
										// show the EXTRA columns entered into the collection TSV file so UI can edit them
										//if (iKnownLabel >= 0)
										if (iKnownLabel >= 0 || iCollectableSettingsMode == 1) // but only for COLLECTION items, not quests (iKnownLabel can be -1)
										{
											// Any tip
											LPSTR pShowTop = "";
											if (iCollectableSettingsMode == 1)
											{
												pShowTop = "Enter a value for this item";
												if (iKnownLabel == 2) pShowTop = "Select an image that will be used to represent this object in your HUD screens";
												if (iKnownLabel == 3) pShowTop = "Enter a description for this item that may appear in your HUD screens";
												if (iKnownLabel == 4) pShowTop = "Enter a cost for this item that may appear in your HUD screens";
												if (iKnownLabel == 5) pShowTop = "Enter a value for this item that may appear in your HUD screens";
												if (iKnownLabel == 6) pShowTop = "Enter a container for this item that determines where the item will start in the game";
												if (iKnownLabel == 7) pShowTop = "Enter ingredients separated my comma to be used by a recipe item";
												if (iKnownLabel == 8) pShowTop = "Enter a style for this item such as recipe or spell";
											}
											if (iCollectableSettingsMode == 2)
											{
												pShowTop = "Enter a value for this quest";
												if (iKnownLabel == 2) pShowTop = "Select an image that will be used to represent this quest in your HUD screens";
												if (iKnownLabel == 51) pShowTop = "Enter a quest type for the quest task";
												if (iKnownLabel == 52) pShowTop = "Enter a description for the quest task";
												if (iKnownLabel == 53) pShowTop = "Enter a description for the quest task";
												if (iKnownLabel == 54) pShowTop = "Enter a description for the quest task";
												if (iKnownLabel == 55) pShowTop = "Enter the name of an object that will represent the quest object";
												if (iKnownLabel == 56) pShowTop = "Enter the name of an object that will represent the quest receiver";
												if (iKnownLabel == 57) pShowTop = "Enter the player level required to activate this quest";
												if (iKnownLabel == 58) pShowTop = "Enter the number of XP points awarded when this quest is completed";
												if (iKnownLabel == 59) pShowTop = "Enter the money earned by completing this quest";
												if (iKnownLabel == 60) pShowTop = "Enter the initial status of this quest when the game starts";
												if (iKnownLabel == 61) pShowTop = "Enter the object to activate when this quest is completed";
												if (iKnownLabel == 62) pShowTop = "Enter a quantity associated with this quest";
												if (iKnownLabel == 63) pShowTop = "Enter the level name that this quest is active on";
											}

											// Attrib Label
											if (iKnownLabel == 2)
											{
												// can change image
												LPSTR pImageLabel = "";
												if (iCollectableSettingsMode == 1)
												{
													pImageLabel = "Item Icon Image";
												}
												if (iCollectableSettingsMode == 2)
												{
													pImageLabel = "Quest Icon Image";
												}
												ImGui::TextCenter(pImageLabel);
												float w = ImGui::GetContentRegionAvailWidth();
												cstr UniqueCollectionItemImage = "##UniqueCollectionItemImage";
												if (iSelectedLibraryStingReturnID == window->GetID(UniqueCollectionItemImage.Get()))
												{
													if (iCollectableSettingsMode == 1)
													{
														g_collectionList[iCollectionItemIndex].collectionFields[l] = sSelectedLibrarySting.Get();
														g_collectionList[iCollectionItemIndex].iEntityID = iMasterID;
														g_collectionList[iCollectionItemIndex].iEntityElementE = iEntityIndex;
													}
													if (iCollectableSettingsMode == 2)
													{
														g_collectionQuestList[iCollectionItemIndex].collectionFields[l] = sSelectedLibrarySting.Get();
													}
													g_bChangedGameCollectionList = true;
													sSelectedLibrarySting = "";
													iSelectedLibraryStingReturnID = -1; //disable.
													g_iIconImageInPropertiesLastEntIndex = 0;// trigger reload
												}
												int entid = 0;
												if (iEntityIndex > 0) entid = t.entityelement[iEntityIndex].bankindex;
												if (entid > 0)
												{
													if (g_iIconImageInPropertiesLastEntIndex != iEntityIndex)
													{
														g_iIconImageInPropertiesLastEntIndex = iEntityIndex;
														g_iconImageInPropertiesLastName_s = "";
														if (entid > 0) g_iconImageInPropertiesLastName_s = t.entitybank_s[entid];
														g_iIconImageInProperties = 0;
													}
													else
													{
														// even if sale element index, can delete and quickly create another collectable in same index slot, need to be aware of this
														if (strcmp (g_iconImageInPropertiesLastName_s.Get(), t.entitybank_s[entid].Get()) != NULL)
														{
															g_iconImageInPropertiesLastName_s = t.entitybank_s[entid];
															g_iIconImageInProperties = 0;
														}
													}
												}
												LPSTR pIconImageInProperties = "";
												if (iCollectableSettingsMode == 1)
												{
													pIconImageInProperties = g_collectionList[iCollectionItemIndex].collectionFields[l].Get();
												}
												if (iCollectableSettingsMode == 2)
												{
													pIconImageInProperties = g_collectionQuestList[iCollectionItemIndex].collectionFields[l].Get();
												}
												if (g_iIconImageInProperties == 0)
												{
													if (FileExist(pIconImageInProperties) == 0)
													{
														// image specified does not exist, original file could have moved/deleted, so revert to default
														pIconImageInProperties = "default";
													}
													cstr actualImgFile_s = "";
													if (stricmp(pIconImageInProperties, "default") == NULL)
													{
														// replace with actual img file if viewing property
														cstr entityfile = t.entitybank_s[iMasterID];
														actualImgFile_s = get_rpg_imagefinalfile(entityfile);
														pIconImageInProperties = actualImgFile_s.Get();
														if (iCollectableSettingsMode == 1)
														{
															g_collectionList[iCollectionItemIndex].collectionFields[l] = pIconImageInProperties;
															g_collectionList[iCollectionItemIndex].iEntityID = iMasterID;
															g_collectionList[iCollectionItemIndex].iEntityElementE = iEntityIndex;
														}
														if (iCollectableSettingsMode == 2)
														{
															g_collectionQuestList[iCollectionItemIndex].collectionFields[l] = pIconImageInProperties;
														}
														g_bChangedGameCollectionList = true;
													}
													g_iIconImageInProperties = g.iconimagebankoffset;
													if (GetImageExistEx(g_iIconImageInProperties) == 1) DeleteImage(g_iIconImageInProperties);
													image_setlegacyimageloading(true);
													if (FileExist(pIconImageInProperties) == 1)
													{
														// actual icon image
														LoadImage(pIconImageInProperties, g_iIconImageInProperties);
													}
													else
													{
														// specified image not found, use placeholder
														pIconImageInProperties = "imagebank\\HUD Library\\MAX\\object.png";
														LoadImage(pIconImageInProperties, g_iIconImageInProperties);
													}
													image_setlegacyimageloading(false);
												}
												int iTextureID = g_iIconImageInProperties;
												ImVec2 ImageSize = ImVec2(w - ImGui::GetCurrentWindow()->ScrollbarSizes.x, ImGui::GetFontSize());
												ID3D11ShaderResourceView* lpTexture = GetImagePointerView(iTextureID);
												if (lpTexture)
												{
													float img_w = ImageWidth(iTextureID);
													float img_h = ImageHeight(iTextureID);
													ImageSize.y = img_h * (ImageSize.x / img_w);
												}
												ImVec2 vImagePos = ImGui::GetCursorPos();
												ImGui::Dummy(ImageSize);
												ImVec4 color = ImVec4(1.0, 1.0, 1.0, 1.0);
												ImVec4 back_color = ImVec4(0.2, 0.2, 0.2, 0.75);
												if (ImGui::IsItemHovered())
												{
													color.w = 0.75;
													if (ImGui::IsMouseReleased(0))
													{
														sStartLibrarySearchString = "Icon";
														bExternal_Entities_Window = true;
														iDisplayLibraryType = 2; //Image
														iLibraryStingReturnToID = window->GetID(UniqueCollectionItemImage.Get());
													}
												}
												ImVec2 img_pos = ImGui::GetWindowPos() + vImagePos;
												img_pos.y -= ImGui::GetScrollY();
												window->DrawList->AddRectFilled(img_pos, img_pos + ImageSize, ImGui::GetColorU32(back_color));
												if (lpTexture)
												{
													window->DrawList->AddImage((ImTextureID)lpTexture, img_pos, img_pos + ImageSize, ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(color));
												}
												else
												{
													window->DrawList->AddRectFilled(img_pos, img_pos + ImageSize, ImGui::GetColorU32(color));
												}
												lpTexture = GetImagePointerView(TOOL_PENCIL); //Add pencil
												if (lpTexture)
												{
													ImVec2 vDrawPos = { ImGui::GetCursorScreenPos().x + (ImGui::GetContentRegionAvail().x - 30.0f) ,ImGui::GetCursorScreenPos().y - ImageSize.y - 3.0f };
													window->DrawList->AddImage((ImTextureID)lpTexture, vDrawPos, vDrawPos + ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));
												}
												if (ImGui::IsItemHovered() && pShowTop) ImGui::SetTooltip(pShowTop);
											}
											else
											{
												if (iCollectableSettingsMode == 2 && iKnownLabel == 51)
												{
													// drop down to make life easier
													char cTmpInput[MAX_PATH];
													strcpy(cTmpInput, g_collectionQuestList[iCollectionItemIndex].collectionFields[l].Get());
													const char* items[] = { "Collect", "Destroy", "Deliver", "Activate" };
													int item_current = 0;
													if (stricmp(cTmpInput, "collect") == NULL) item_current = 0;
													if (stricmp(cTmpInput, "destroy") == NULL) item_current = 1;
													if (stricmp(cTmpInput, "deliver") == NULL) item_current = 2;
													if (stricmp(cTmpInput, "activate") == NULL) item_current = 3;
													ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
													ImGui::Text("Quest Type");
													ImGui::SameLine();
													ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
													ImGui::SetCursorPos(ImVec2(fPropertiesColoumWidth, ImGui::GetCursorPosY()));
													ImGui::PushItemWidth(-10);
													if (ImGui::Combo("##combostaticQuestType2", &item_current, items, IM_ARRAYSIZE(items)))
													{
														if (item_current == 0) g_collectionQuestList[iCollectionItemIndex].collectionFields[l] = "collect";
														if (item_current == 1) g_collectionQuestList[iCollectionItemIndex].collectionFields[l] = "destroy";
														if (item_current == 2) g_collectionQuestList[iCollectionItemIndex].collectionFields[l] = "deliver";
														if (item_current == 3) g_collectionQuestList[iCollectionItemIndex].collectionFields[l] = "activate";
													}
													ImGui::PopItemWidth();

													// optional quantity property only applicable to some quest types
													if (item_current == 0)
													{
														// later in loop we use this flag to allow quantity to show
														bQuestTypeIsCollect = true;
													}
												}
												else
												{
													// good old typing out your entry
													bool bAllowEditing = true;
													if (iKnownLabel == 0 || iKnownLabel == 1) bAllowEditing = false;
													if (t.entityprofile[iMasterID].isweapon > 0 && iKnownLabel >= 7) bAllowEditing = false;
													if (iCollectableSettingsMode == 2 && iKnownLabel == 62 && bQuestTypeIsCollect == false) bAllowEditing = false;
													if (bAllowEditing == true)
													{
														char pNameOfAttrib[MAX_PATH];
														if (iCollectableSettingsMode == 1)
														{
															strcpy(pNameOfAttrib, "Item ");
														}
														if (iCollectableSettingsMode == 2)
														{
															strcpy(pNameOfAttrib, "Quest ");
														}
														char pCap[2];
														pCap[0] = pLabel[0];
														pCap[1] = 0;
														strupr(pCap);
														strcat(pNameOfAttrib, pCap);
														strcat(pNameOfAttrib, pLabel + 1);
														ImGui::TextCenter(pNameOfAttrib);
														ImGui::PushItemWidth(-10);
														char cTmpInput[MAX_PATH];
														if (iCollectableSettingsMode == 1)
														{
															strcpy(cTmpInput, g_collectionList[iCollectionItemIndex].collectionFields[l].Get());
														}
														if (iCollectableSettingsMode == 2)
														{
															strcpy(cTmpInput, g_collectionQuestList[iCollectionItemIndex].collectionFields[l].Get());
														}
														int inputFlags = 0;
														char pNameOfAttribUnique[MAX_PATH];
														strcpy(pNameOfAttribUnique, "##CollectableItem");
														strcat(pNameOfAttribUnique, pLabel);
														if (ImGui::InputText(pNameOfAttribUnique, &cTmpInput[0], MAXTEXTINPUT, inputFlags))
														{
															if (iCollectableSettingsMode == 1)
															{
																g_collectionList[iCollectionItemIndex].collectionFields[l] = cTmpInput;
																g_collectionList[iCollectionItemIndex].iEntityID = iMasterID;
																g_collectionList[iCollectionItemIndex].iEntityElementE = iEntityIndex;
															}
															if (iCollectableSettingsMode == 2)
															{
																g_collectionQuestList[iCollectionItemIndex].collectionFields[l] = cTmpInput;
															}
															bImGuiGotFocus = true;
															g_bChangedGameCollectionList = true;
														}
														if (ImGui::IsItemHovered() && pShowTop) ImGui::SetTooltip(pShowTop);
														if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;
														ImGui::PopItemWidth();
													}
												}
											}
										}
									}
									ImGui::Indent(-10);

									// option to delete a collectable from the global list
									if (iCollectableSettingsMode == 1)
									{
										float but_gadget_size = ImGui::GetFontSize() * 12.0;
										float w = ImGui::GetWindowContentRegionWidth() - 10.0;
										ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w * 0.5) - (but_gadget_size * 0.5), 0.0f));
										LPSTR pCreateButtonLabel = "Delete From Collection List";
										if (ImGui::StyleButton(pCreateButtonLabel, ImVec2(but_gadget_size, 0)))
										{
											// create new quest list without the one deleted
											std::vector<collectionItemType> newCollectionList;
											for (int ci = 0; ci < g_collectionList.size(); ci++)
											{
												if (ci != iCollectionItemIndex)
												{
													newCollectionList.push_back(g_collectionList[ci]);
												}
											}
											g_collectionList = newCollectionList;
											g_bChangedGameCollectionList = true;
										}
										if (ImGui::IsItemHovered()) ImGui::SetTooltip("Delete this collection item from the main collection list of the game project");
									}
								}
							}
						}

						// save any changes to game collection list 
						if (g_bChangedGameCollectionList == true)
						{
							// go through all object parents to ensure
							refresh_rpg_parents_of_items();
							
							// save collection item list out
							save_rpg_system(pref.cLastUsedStoryboardProject, true);
							g_bChangedGameCollectionList = false;
						}

						// Custom Materials now an advanced feature
						if ( bMaterialsUsed == true && pref.iObjectEnableAdvanced && t.entityprofile[iMasterID].ismarker == 0 && bIsThisAnEBE == false)
						{
							// multi-selection allows SOME material changes
							if (!bRubberbandActive)
							{
								// single object editing
								bool bNeedMaterialUpdate = false;

								// detect if object changed while showing materials, ensure the switch happens in the UI
								static void* pLastObjectPtr;
								if ((void*)pObject != pLastObjectPtr)
								{
									//PE: reset mesh names, as we got a new object.
									t.importer.bModelMeshNamesSet = false;
									t.importer.cModelMeshNames.clear();
									bNeedMaterialUpdate = true;
									pLastObjectPtr = (void*)pObject;
								}

								if (pref.bAutoClosePropertySections && iLastOpenHeader != 13)
									ImGui::SetNextItemOpen(false, ImGuiCond_Always);

								if (ImGui::StyleCollapsingHeader("Materials##2", ImGuiTreeNodeFlags_None))
								{
									iLastOpenHeader = 13;

									ImGui::Indent(10);

									{
										if(!t.entityelement[iEntityIndex].eleprof.bUseFPESettings)
										{
											if (t.entityelement[iEntityIndex].eleprof.iMaterialSoundIndex >= 5)
												t.entityelement[iEntityIndex].eleprof.iMaterialSoundIndex = 0;

											char material_sound_selection[256] = "\0";
											if (t.entityelement[iEntityIndex].eleprof.iMaterialSoundIndex <= 0)
											{
												strcpy(material_sound_selection, "None");
											}
											if (t.entityelement[iEntityIndex].eleprof.iMaterialSoundIndex == 1)
											{
												strcpy(material_sound_selection, "Silent");
											}
											else if (t.entityelement[iEntityIndex].eleprof.iMaterialSoundIndex == 2)
											{
												strcpy(material_sound_selection, "Stone");
											}
											else if (t.entityelement[iEntityIndex].eleprof.iMaterialSoundIndex == 3)
											{
												strcpy(material_sound_selection, "Metal");
											}
											else if (t.entityelement[iEntityIndex].eleprof.iMaterialSoundIndex == 4)
											{
												strcpy(material_sound_selection, "Wood");
											}
											char* cMaterialTypes[4] = { "Silent", "Stone", "Metal", "Wood" };
											ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 15));
											ImGui::Text("Material Type");
											ImGui::SameLine();
											ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
											ImGui::PushItemWidth(-10);
											if (ImGui::BeginCombo("##ImporterMaterialType", &material_sound_selection[0], ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_HeightLarge))
											{
												for (int i = 0; i < 4; i++)
												{
													bool is_selected = false;
													if (strcmp(material_sound_selection, cMaterialTypes[i]) == NULL)
													{
														is_selected = true;
													}
													if (ImGui::Selectable(cMaterialTypes[i], is_selected))
													{
														strcpy(material_sound_selection, cMaterialTypes[i]);
														t.entityelement[iEntityIndex].eleprof.iMaterialSoundIndex = i + 1;
														//t.slidersmenuvalue[t.importer.properties1Index][10].value = i + 1;
													}
													if (is_selected) ImGui::SetItemDefaultFocus();
												}
												ImGui::EndCombo();
											}
											if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select the material index for this object");
											ImGui::PopItemWidth();
											ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 15));
										}

										//PE: display material settings
										WickedSetEntityId(iMasterID);
										WickedSetElementId(iEntityIndex);
										Wicked_Change_Object_Material((void*)pObject, 0, &t.entityelement[iEntityIndex].eleprof,true, t.entityelement[iEntityIndex].eleprof.bUseFPESettings);

										if (ImGui::Checkbox("Always use Original Object Settings##2", &t.entityelement[iEntityIndex].eleprof.bUseFPESettings))
										{
											bNeedMaterialUpdate = true;
											t.importer.bModelMeshNamesSet = false;
											t.importer.cModelMeshNames.clear();
										}
										WickedSetEntityId(-1);
										WickedSetElementId(0);
									}

									ImGui::Indent(-10);
								}
								if (bNeedMaterialUpdate)
								{
									if (t.entityelement[iEntityIndex].eleprof.bUseFPESettings || !t.entityelement[iEntityIndex].eleprof.bCustomWickedMaterialActive)
									{
										// Set material settings from master object.
										sObject* pMasterObject = g_ObjectList[g.entitybankoffset + iMasterID];
										if (pMasterObject && iMasterID > 0 && iMasterID < t.entityprofile.size())
										{
											Wicked_Copy_Material_To_Grideleprof((void*)pMasterObject, 0, &t.entityelement[iEntityIndex].eleprof);
											Wicked_Set_Material_From_grideleprof((void*)pObject, 0, &t.entityelement[iEntityIndex].eleprof);
										}
									}
									else
									{
										// Set custom material settings.
										Wicked_Copy_Material_To_Grideleprof((void*)pObject, 0, &t.entityelement[iEntityIndex].eleprof);
										Wicked_Set_Material_From_grideleprof((void*)pObject, 0, &t.entityelement[iEntityIndex].eleprof);
										t.entityelement[iEntityIndex].eleprof.WEMaterial.MaterialActive = true;
										t.grideleprof.WEMaterial.MaterialActive = true;
									}
								}
							}
							else
							{
								// multi selection editing
								bool bNeedMaterialUpdate = false;
								if (pref.bAutoClosePropertySections && iLastOpenHeader != 13) ImGui::SetNextItemOpen(false, ImGuiCond_Always);
								if (ImGui::StyleCollapsingHeader("All Materials##3", ImGuiTreeNodeFlags_None))
								{
									// UI for all materials
									iLastOpenHeader = 13;
									ImGui::Indent(10);

									// any custom flags sets ALL of them (or none of them)
									bool bAnyCustomMaterials = false;
									bool bAnyUseFPESettings = false;
									for (int ii = 0; ii < g.entityrubberbandlist.size(); ii++)
									{
										int ee = g.entityrubberbandlist[ii].e;
										if (t.entityelement[ee].eleprof.bCustomWickedMaterialActive == true)
										{
											bAnyCustomMaterials = true;
											//break;
										}
										if (t.entityelement[ee].eleprof.bUseFPESettings)
										{
											bAnyUseFPESettings = true;
										}
									}
									{
										// display custom material settings for ALL objects
										if (g.entityrubberbandlist.size() > 0)
										{
											int iEntityIndex = g.entityrubberbandlist[0].e;
											WickedSetEntityId(t.entityelement[iEntityIndex].bankindex);
											WickedSetElementId(iEntityIndex);
											Wicked_Change_Object_Material((void*)pObject, 6, &t.entityelement[iEntityIndex].eleprof,true, bAnyUseFPESettings);

											if (ImGui::Checkbox("Always use Original Object Settings##3", &bAnyUseFPESettings))
											{
												for (int ii = 0; ii < g.entityrubberbandlist.size(); ii++)
												{
													int ee = g.entityrubberbandlist[ii].e;
													t.entityelement[ee].eleprof.bUseFPESettings = bAnyUseFPESettings;
													if (bAnyUseFPESettings)
													{
														//PE: Copy material from master object.
														int iMasterID = t.entityelement[ee].bankindex;
														if (iMasterID > 0 && iMasterID < t.entityprofile.size())
														{
															sObject* pMasterObject = g_ObjectList[g.entitybankoffset + iMasterID];
															if (pMasterObject)
															{
																Wicked_Copy_Material_To_Grideleprof((void*)pMasterObject, 0, &t.entityelement[ee].eleprof);
																if (t.entityprofile[iMasterID].WEMaterial.dwBaseColor[0] == -1)
																	SetObjectDiffuse(iActiveObj, Rgb(255, 255, 255));
																Wicked_Set_Material_From_grideleprof((void*)pObject, 0, &t.entityelement[ee].eleprof);
																t.entityelement[ee].eleprof.WEMaterial.MaterialActive = false;
															}
														}
													}
												}
											}

											WickedSetEntityId(-1);
											WickedSetElementId(0);
										}
									}
									ImGui::Indent(-10);
								}
							}
						}

						// Moved into its own settings.
						if (pref.iEnableDeveloperObjectTools)
						{
							char title[24] = "Developer Settings##2";

							if (pref.bAutoClosePropertySections && iLastOpenHeader != 18)
								ImGui::SetNextItemOpen(false, ImGuiCond_Always);

							//Need pref. default closed.
							if (ImGui::StyleCollapsingHeader(title, ImGuiTreeNodeFlags_None)) //ImGuiTreeNodeFlags_DefaultOpen
							{
								iLastOpenHeader = 18;
								DisplayFPEAdvanced(false, iMasterID, &t.entityelement[iEntityIndex].eleprof, iEntityIndex);
							}
						}
					}
				}

				// entity lock/unlock
				if ( bClickedTheLockUnlockButton == true )
				{
					if (iEntityIndex > 0)
					{
						int iLoopMax = 1;
						if (g.entityrubberbandlist.size() > 0) iLoopMax = g.entityrubberbandlist.size();
						for (int i = 0; i < iLoopMax; i++)
						{
							// get entity index
							int e = iEntityIndex;
							if (g.entityrubberbandlist.size() > 0)
								e = g.entityrubberbandlist[i].e;

							// toggle lock flag
							t.entityelement[e].editorlock = 1 - t.entityelement[e].editorlock;

							sObject* pObject;
							if (t.entityelement[e].obj > 0)
							{
								pObject = g_ObjectList[t.entityelement[e].obj];
								if (pObject)
								{
									if (t.entityelement[e].editorlock)
									{
										#ifndef ALLOWSELECTINGLOCKEDOBJECTS
										WickedCall_SetObjectRenderLayer(pObject, GGRENDERLAYERS_CURSOROBJECT);
										#endif
										sRubberBandType vEntityLockedItem;
										vEntityLockedItem.e = e;
										vEntityLockedList.push_back(vEntityLockedItem);
									}
									else 
									{
										//Delete from list.
										for (int i = 0; i < vEntityLockedList.size(); i++)
										{
											if (vEntityLockedList[i].e == e) 
											{
												vEntityLockedList.erase(vEntityLockedList.begin() + i);
												break;
											}
										}
										WickedCall_SetObjectRenderLayer(pObject, GGRENDERLAYERS_NORMAL);
									}
								}
							}
						}
						iLastActiveEntityIndex = -1;
						iLastActiveObj = -1;

						// any lock/unlock operations resets, avoids issue of duplcating a static object and unable to 'move' it
						t.widget.pickedObject = 0;
					}
				}
				//##############################
				//#### Grid/Editor Settings ####
				//##############################
				// grid and alignment moved here from above (no longer need a host object)
				if (pref.iSmallToolbar == 0)
				{
					// older system (never had Y axis), so force this scenario to restore legacy behavior if using old grid settings method!
					pref.fEditorGridOffsetY = 0;
					pref.fEditorGridSizeY = 0;

					if (pref.bAutoClosePropertySections && iLastOpenHeader != 15)
						ImGui::SetNextItemOpen(false, ImGuiCond_Always);

					if (ImGui::StyleCollapsingHeader("Grid and Alignment Settings", ImGuiTreeNodeFlags_None))
					{
						iLastOpenHeader = 15;
						ImGui::Indent(10);

						//static bool bEditorGridFitObjectSize = false;
						static int iGridOffsetMode = 0;
						if (t.gridentitygridlock > 0)
						{
							if (t.gridentitygridlock == 2)
								iGridOffsetMode = 0;
							else
								iGridOffsetMode = 1;
						}
						else
						{
							iGridOffsetMode = 0;
						}
						bool bGridEnabled = pref.iGridEnabled;
						if (ImGui::Checkbox("Enable Grid Mode", &bGridEnabled))
						{
							pref.iGridEnabled = bGridEnabled;
						}
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Select to snap the object to an aligned position");
						if (pref.iGridEnabled == false)
						{
							t.gridentitygridlock = 0;
							pref.iGridMode = t.gridentitygridlock;
						}
						if (pref.iGridEnabled == true)
						{
							ImGui::RadioButton("Use Grid Positions", &iGridOffsetMode, 0);
							if (ImGui::IsItemHovered()) ImGui::SetTooltip("Snaps the selected object to the chosen grid positions");
							ImGui::RadioButton("Snap Mode", &iGridOffsetMode, 1);
							if (ImGui::IsItemHovered()) ImGui::SetTooltip("Snap the object to the nearest object bound box");
							t.gridentitygridlock = 2 - iGridOffsetMode;
							pref.iGridMode = t.gridentitygridlock;
						}

						// grid size only avaiulable in advanced mode
						ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
						if (pref.iObjectEnableAdvanced)
						{
							if (t.gridentitygridlock == 2)
							{
								ImGui::TextCenter("Grid Offset");
								float w = ImGui::GetContentRegionAvail().x - 10.0f;
								float inputsize = w / 2.0f;
								inputsize -= 10.0f; //For text.
								inputsize -= 5.0f; //For padding.
								ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(18.0f, 3.0f));
								ImGui::Text("X");
								ImGui::SameLine();
								ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, -3.0f));
								ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
								ImGui::InputFloat("##XYZgridoffsetX", &pref.fEditorGridOffsetX, 0.0f, 0.0f, "%.1f");
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Offset X");
								ImGui::PopItemWidth();
								ImGui::SameLine();
								ImGui::Text("Z");
								ImGui::SameLine();
								ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
								ImGui::InputFloat("##XYZgridoffsetZ", &pref.fEditorGridOffsetZ, 0.0f, 0.0f, "%.1f");
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Offset Z");
								ImGui::PopItemWidth();
								ImGui::TextCenter("Grid Size");
								ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(18.0f, 3.0f));
								ImGui::Text("X");
								ImGui::SameLine();
								ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, -3.0f));
								ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
								ImGui::InputFloat("##XYZgridsizeX", &pref.fEditorGridSizeX, 0.0f, 0.0f, "%.1f");
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Size X");
								ImGui::PopItemWidth();
								ImGui::SameLine();
								ImGui::Text("Z");
								ImGui::SameLine();
								ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
								ImGui::InputFloat("##XYZgridsizeZ", &pref.fEditorGridSizeZ, 0.0f, 0.0f, "%.1f");
								if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Size Z");
								ImGui::PopItemWidth();

								// clever button to align grid to object (for older levels with arbitary alignments mixed together)
								if (iEntityIndex > 0 && g.entityrubberbandlist.size() == 0)
								{
									float w = ImGui::GetWindowContentRegionWidth();
									float but_gadget_size = ImGui::GetFontSize()*15.0;
									ImGui::Text("");
									ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
									if (ImGui::StyleButton("Align Grid Offset To Object", ImVec2(but_gadget_size, 0)))
									{
										float x = t.entityelement[iEntityIndex].x;
										float z = t.entityelement[iEntityIndex].z;
										int iSizeRoundedX = int(x / pref.fEditorGridSizeX)*pref.fEditorGridSizeX;
										pref.fEditorGridOffsetX = x - iSizeRoundedX;
										int iSizeRoundedZ = int(z / pref.fEditorGridSizeZ)*pref.fEditorGridSizeZ;
										pref.fEditorGridOffsetZ = z - iSizeRoundedZ;
									}
									ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
									if (ImGui::StyleButton("Align Grid Size To Object", ImVec2(but_gadget_size, 0)))
									{
										float sx = ObjectSizeX(t.entityelement[iEntityIndex].obj, 1);
										float sz = ObjectSizeZ(t.entityelement[iEntityIndex].obj, 1);
										pref.fEditorGridSizeX = sx;
										pref.fEditorGridSizeZ = sz;
									}
								}

								// can never have a grid size below one
								if (pref.fEditorGridSizeX <= 0.1f) pref.fEditorGridSizeX = 0.1f;
								if (pref.fEditorGridSizeZ <= 0.1f) pref.fEditorGridSizeZ = 0.1f;
							}
						}

						// button to unlock any objects in locked list
						if (vEntityLockedList.size() > 0)
						{
							float w = ImGui::GetWindowContentRegionWidth();
							float but_gadget_size = ImGui::GetFontSize()*10.0;
							ImGui::Text("");
							ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
							cStr unlockstr = cStr("Unlock ") + cStr((int)vEntityLockedList.size()) + cStr(" Objects");
							if (ImGui::StyleButton(unlockstr.Get(), ImVec2(but_gadget_size, 0))) 
							{
								for (int i = 0; i < vEntityLockedList.size(); i++)
								{
									int e = vEntityLockedList[i].e;
									if (e < 0 || e >= t.entityelement.size()) continue;

									t.entityelement[e].editorlock = 0;
									sObject* pObject;
									if (t.entityelement[e].obj > 0) 
									{
										pObject = g_ObjectList[t.entityelement[e].obj];
										if (pObject) 
										{
											WickedCall_SetObjectRenderLayer(pObject, GGRENDERLAYERS_NORMAL);
										}
									}
								}
								vEntityLockedList.clear();

								// any lock/unlock operations resets, avoids issue of duplcating a static object and unable to 'move' it
								t.widget.pickedObject = 0;
							}
						}
						ImGui::Indent(-10);
						ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
					}
				}

				// detect ANY change inside entityelement (inc eleprof) so can trigger instance cloing of collectables
				if (iEntityIndex > 0 && bSnappedEntityElementCopy == true)
				{
					bool bEntityElementChanged = false;
					char* pMemDataOld = (char*)&snapshotentityelement;
					char* pMemDataNew = (char*)&t.entityelement[iEntityIndex].eleprof;
					int iWhereWeDetectedChange = 0;
					for (int n = 0; n < sizeof(entityeleproftype); n++)
					{
						if (*pMemDataOld != *pMemDataNew)
						{
							// change in entity element or eleprof detected
							iWhereWeDetectedChange = n;
							bEntityElementChanged = true;
							break;
						}
						pMemDataOld++;
						pMemDataNew++;
					}
					if (bEntityElementChanged == true)
					{
						// now scan all entities in common with this entity, and clone all details
						// to them (there should only be one collectale entity element/eleprof identity)
						if (t.entityelement[iEntityIndex].eleprof.iscollectable != 0)
						{
							int thismasterid = t.entityelement[iEntityIndex].bankindex;
							LPSTR pMasterEntityName = t.entityelement[iEntityIndex].eleprof.name_s.Get();
							for (int ee = 1; ee <= g.entityelementmax; ee++)
							{
								if (ee != iEntityIndex)
								{
									int masterid = t.entityelement[ee].bankindex;
									if (masterid > 0)
									{
										// and essentially, ONLY copy into the SAME PARENT ID object (not ANYTHING named so!)
										if (thismasterid == masterid)
										{
											if (stricmp (t.entityelement[ee].eleprof.name_s.Get(), pMasterEntityName) == NULL)
											{
												// clone currently edited entity
												t.entityelement[ee].eleprof = t.entityelement[iEntityIndex].eleprof;
												sObject* pObject = GetObjectData(t.entityelement[ee].obj);
												if (pObject)
												{
													WickedSetEntityId(masterid);
													WickedSetElementId(ee);
													for (int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++)
													{
														Wicked_Set_Material_From_grideleprof_ThisMesh (pObject, 0, &t.entityelement[ee].eleprof, iMesh);
													}
													WickedSetEntityId(-1);
													WickedSetElementId(0);
												}
											}
										}
									}
								}
							}
						}

						// any entity change should invalidate project change flag so can save even small changes
						g.projectmodified = 1;
					}
				}

				// tutorial component
				if (!pref.bHideTutorials)
				{
					// Default to tutorial panel if no object is selected.
					// if (Entity_Tools_Window && !g_selected_editor_object && !Visuals_Tools_Window && iLastOpenHeader != 20)
					//LB: can keep tutorial closed now even if no object selected 
					if (Entity_Tools_Window && !g_selected_editor_object && !Visuals_Tools_Window && iLastOpenHeader != 15 && iLastOpenHeader != 16 && iLastOpenHeader != 8 && iLastOpenHeader != 9 && iLastOpenHeader != 20 && sGotoPreviewWithFile.Len() == 0) // 20 is keyboard shortcxuts, 15 is grid component
						iLastOpenHeader = 19;

					if (pref.bAutoClosePropertySections && iLastOpenHeader != 19)
						ImGui::SetNextItemOpen(false, ImGuiCond_Always);
					
					if (ImGui::StyleCollapsingHeader("Tutorial", ImGuiTreeNodeFlags_DefaultOpen))
					{
						iLastOpenHeader = 19;

						ImGui::Indent(10);
						cstr cShowTutorial = "0201 - Level Editing";
						char* tutorial_combo_items[] = { "0201 - Level Editing", "0301 - Object Library", "0401 - Object Grouping", "0601 - Terrain Editing", "0202 - Particle Editor", "0203 - The Animation Library", "0801 - Character Creator", "0901 - Behaviour AI" };
						SmallTutorialVideo(cShowTutorial.Get(), tutorial_combo_items, ARRAYSIZE(tutorial_combo_items), SECTION_ENTITY_TOOLS, true );
						float but_gadget_size = ImGui::GetFontSize()*12.0;
						float w = ImGui::GetWindowContentRegionWidth() - 10.0;
						ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (but_gadget_size*0.5), 0.0f));
						#ifdef INCLUDESTEPBYSTEP
						if (ImGui::StyleButton("View Step by Step Tutorial", ImVec2(but_gadget_size, 0)))
						{
							// pre-select tutorial 03
							bHelp_Window = true;
							bHelpVideo_Window = true;
							bSetTutorialSectionLeft = false;
							strcpy(cForceTutorialName, cShowTutorial.Get());
						}
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Start Step by Step Tutorial");
						#endif
						ImGui::Indent(-10);
					}
				}

				// insert a keyboard shortcut component into panel
				UniversalKeyboardShortcut(eKST_ObjectMode);

				if (bRunExtractDuplicate) 
				{
					t.tentitytoselect = iEntityIndex;
					t.widget.duplicatebuttonselected = 0;
					t.gridentityautofind = 7;

					t.widget.pickedObject = 0;
					widget_updatewidgetobject();

					t.onetimeentitypickup = 1;

					//  extract entity from the map
					if (t.tentitytoselect > 0)
					{
						if (t.entityelement[t.tentitytoselect].editorfixed == 0)
						{
							fExtractYValue = t.entityelement[t.tentitytoselect].y;

							t.gridentityeditorfixed = t.entityelement[t.tentitytoselect].editorfixed;
							t.gridentity = t.entityelement[t.tentitytoselect].bankindex;
							t.ttrygridentitystaticmode = t.entityelement[t.tentitytoselect].staticflag;
							t.ttrygridentity = t.gridentity; editor_validatestaticmode();
							t.gridedit.autoflatten = t.entityprofile[t.gridentity].autoflatten;
							t.gridedit.entityspraymode = 0;
							if (t.gridentityautofind == 7)
							{
								//  widget extracts without forcing entity to Floor
								t.gridentityautofind = 0;
								t.gridentityposoffground = 1;
								t.gridentityusingsoftauto = 0;
							}
							else
							{
								t.gridentityposoffground = 0;
								t.gridentityusingsoftauto = 1;
								// MAX handles its own positioning system
								{
									t.gridentityautofind = 0;
								}
							}
							t.gridentitysurfacesnap = 0; // surfacesnap off as messes up extract offset for entity
							t.gridentityextractedindex = t.tentitytoselect;
							t.gridentityhasparent = 0;//t.entityelement[t.tentitytoselect].iHasParentIndex; 210317 - break association when extract so can place free of parent
							t.gridentityposx_f = t.entityelement[t.tentitytoselect].x;
							t.gridentityposy_f = t.entityelement[t.tentitytoselect].y;
							t.gridentityposz_f = t.entityelement[t.tentitytoselect].z;
							t.gridentityrotatex_f = t.entityelement[t.tentitytoselect].rx;
							t.gridentityrotatey_f = t.entityelement[t.tentitytoselect].ry;
							t.gridentityrotatez_f = t.entityelement[t.tentitytoselect].rz;
							t.gridentityrotatequatmode = t.entityelement[t.tentitytoselect].quatmode;
							t.gridentityrotatequatx_f = t.entityelement[t.tentitytoselect].quatx;
							t.gridentityrotatequaty_f = t.entityelement[t.tentitytoselect].quaty;
							t.gridentityrotatequatz_f = t.entityelement[t.tentitytoselect].quatz;
							t.gridentityrotatequatw_f = t.entityelement[t.tentitytoselect].quatw;
							if (t.entityprofile[t.gridentity].ismarker == 10)
							{
								t.gridentityscalex_f = 100.0f + t.entityelement[t.tentitytoselect].scalex;
								t.gridentityscaley_f = 100.0f + t.entityelement[t.tentitytoselect].scaley;
								t.gridentityscalez_f = 100.0f + t.entityelement[t.tentitytoselect].scalez;
							}
							else
							{
								t.gridentityscalex_f = ObjectScaleX(t.entityelement[t.tentitytoselect].obj);
								t.gridentityscaley_f = ObjectScaleY(t.entityelement[t.tentitytoselect].obj);
								t.gridentityscalez_f = ObjectScaleZ(t.entityelement[t.tentitytoselect].obj);
							}
							t.grideleprof = t.entityelement[t.tentitytoselect].eleprof;
							t.grideleproflastname_s = t.grideleprof.name_s;

							//  Transfer any waypoint association
							t.waypointindex = t.entityelement[t.tentitytoselect].eleprof.trigger.waypointzoneindex;
							t.grideleprof.trigger.waypointzoneindex = t.waypointindex;
							t.waypoint[t.waypointindex].linkedtoentityindex = 0;
							
							if (!bDuplicate) 
							{
								iLastEntityOnCursor = t.tentitytoselect;
								gridedit_deleteentityfrommap();
							}
							else
							{
								iLastEntityOnCursor = 0;
							}

							t.refreshgrideditcursor = 1;

							// remove entity index from rubber band selection
							for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
								if (g.entityrubberbandlist[i].e == t.tentitytoselect)
									g.entityrubberbandlist[i].e = 0;

							//Just place under cursor.
							t.inputsys.dragoffsetx_f = 0;
							t.inputsys.dragoffsety_f = 0;
						}
					}
				}

				CheckMinimumDockSpaceSize(250.0f);

				//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
				if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0)
				{
					ImGui::Text("");
					ImGui::Text("");
				}

				//PE: End child here if active
				if (bWithNoScrollbar)
				{
					ImGui::EndChild();
				}

				ImGui::End();
			}
		}

		//############################
		//#### Save To Level Cloud ###
		//############################

		//########################
		//#### Download Store ####
		//########################

		//Waypoints##WaypointsToolsWindow
		if (refresh_gui_docking == 0 && !bDownloadStore_Window) 
		{
			//Make sure window is setup in docking space.
			ImGui::SetNextWindowSize(ImVec2(40 * ImGui::GetFontSize(), 30 * ImGui::GetFontSize()), ImGuiCond_Once);
			ImGui::SetNextWindowPosCenter(ImGuiCond_Once);

			ImGui::Begin("Game Creator Store Downloader##DownloadStoreWindow", &bDownloadStore_Window, 0);
			ImGui::End();
		}
		imgui_download_store();

		//#######################
		//#### Shooter Tools ####
		//#######################
		if (Shooter_Tools_Window )
		{
			//PE: Need own docking window.
			//LB: Shooter now a filter mode 
			//ImGui::Begin("Shooter Genre##GameLogicTools", &Shooter_Tools_Window, 0);
			//imgui_shooter_tools();
			//ImGui::End();
			iIncludeLeftIconSet = 1;
			g_bDotsAreVisible = true;
			if (t.showeditorelements == 1)
			{
				g_bDotsAreVisible = true;
				DrawLogicNodes(true);
			}
		}
		else 
		{
			if(iIncludeLeftIconSet == 1) iIncludeLeftIconSet = 0;
			Shooter_Tools_Window_Active = false;
			if (g_bDotsAreVisible)
			{
				DrawLogicNodes(false);
				g_bDotsAreVisible = false;
			}
		}

		//########################
		//#### Weather Window ####
		//########################

		//PE: Game Settings window.
		if (refresh_gui_docking == 0 && !Game_Settings_Window)
		{
			//Make sure window is setup in docking space.
			bool bTrue = true;
			ImGui::Begin("Game Settings##GameSettings", &bTrue, iGenralWindowsFlags);
			ImGui::End();
		}
		else
		{
			if (Game_Settings_Window)
			{
				ImGui::Begin("Game Settings##GameSettings", &Game_Settings_Window, 0);
				imgui_Customize_Game_Settings(3);
				ImGui::End();

			}
		}

		if (refresh_gui_docking == 0 && !Logic_Settings_Window)
		{
			//Make sure window is setup in docking space.
			bool bTrue = true;
			ImGui::Begin("Logic Settings##LogicSettings", &bTrue, iGenralWindowsFlags);
			ImGui::End();
		}
		else
		{
			if (Logic_Settings_Window)
			{
				ImGui::Begin("Logic Settings##LogicSettings", &Logic_Settings_Window, 0);

				// LB: inserted shooter properties at top of Object Tools if filter mode active
				if (Shooter_Tools_Window)
				{
					//PE: Looks like pref.iEnableRelationPopupWindow is not available anymore , but just in case :)
					imgui_shooter_tools();
				}

				imgui_Customize_Logic_Settings(3);
				ImGui::End();

			}
		}

		void ProcessQuestEditor(void);
		ProcessQuestEditor();


		//#####################
		//#### Preferences ####
		//#####################

		ProcessPreferences();

		//########################
		//#### Waypoint Tools ####
		//########################

		//Waypoints##WaypointsToolsWindow
		if (refresh_gui_docking == 0 && !bWaypoint_Window) 
		{
			//Make sure window is setup in docking space.
			ImGui::Begin("Waypoints##WaypointsToolsWindow", &bWaypoint_Window, iGenralWindowsFlags);
			ImGui::End();
		}
		waypoint_imgui_loop();

		//#######################
		//#### Terrain Tools ####
		//#######################
		static bool bTerrainToolsDocked = false;

		if (!bTerrainToolsDocked || (refresh_gui_docking == 0 && !bTerrain_Tools_Window ))
		{
			//Make sure window is setup in docking space.
			bool bTrue = true;
			ImGui::Begin("Terrain Tools##TerrainToolsWindow", &bTerrain_Tools_Window, iGenralWindowsFlags);
			ImGui::End();
			ImGui::Begin("Sculpt Terrain##TerrainToolsWindow", &bTrue, iGenralWindowsFlags);
			ImGui::End();
			ImGui::Begin("Paint Terrain##TerrainToolsWindow", &bTrue, iGenralWindowsFlags);
			ImGui::End();
			ImGui::Begin("Add Vegetation##TerrainToolsWindow", &bTrue, iGenralWindowsFlags);
			ImGui::End();
			ImGui::Begin("Terrain Tools##Sculpt Terrain##TerrainToolsWindow", &bTrue, iGenralWindowsFlags);
			ImGui::End();
			ImGui::Begin("Terrain Tools##Paint Terrain##TerrainToolsWindow", &bTrue, iGenralWindowsFlags);
			ImGui::End();
			ImGui::Begin("Terrain Tools##Add Vegetation##TerrainToolsWindow", &bTrue, iGenralWindowsFlags);
			ImGui::End();
			ImGui::Begin("Terrain Tools##Add Trees##TerrainToolsWindow", &bTrue, iGenralWindowsFlags);
			ImGui::End();
			ImGui::Begin("Terrain Tools##Add Bushes##TerrainToolsWindow", &bTrue, iGenralWindowsFlags);
			ImGui::End();
		
			bTerrainToolsDocked = true;
		}
		else
		{
			imgui_terrain_loop_v3(); //PE: New design for Paul's new terrain system :)
		}

		//############################
		//#### Builder Properties ####
		//############################

		if (refresh_gui_docking == 0 && !bBuilder_Properties_Window) 
		{
			//Make sure window is setup in docking space.
			ImGui::Begin("Structure Properties##BuilderPropertiesWindow", &bBuilder_Properties_Window, iGenralWindowsFlags);
			ImGui::End();
		}
		else {
			if(!bBuilder_Properties_Window)
				if (t.ebe.on == 1) ebe_hide();
		}
		imgui_ebe_loop();


		//###########################
		//#### Character Creator ####
		//###########################

		if (refresh_gui_docking == 0 && !g_bCharacterCreatorPlusActivated) 
		{
			//Make sure window is setup in docking space.
			ImGui::Begin("Character Creator##PropertiesWindow", &g_bCharacterCreatorPlusActivated, iGenralWindowsFlags);
			ImGui::End();
		}
		charactercreatorplus_imgui_v3();

		//################################
		//#### Building Editor - 2022 ####
		//################################


		//###########################
		//#### Entity Properties ####
		//###########################
			
		static int iOldPickedEntityIndex = -1;

		if (refresh_gui_docking == 0) ImGui::SetNextWindowPos(viewPortPos + ImVec2(400, 140), ImGuiCond_Always); //ImGuiCond_FirstUseEver,ImGuiCond_Once
		if (refresh_gui_docking == 0) ImGui::SetNextWindowSize(ImVec2(30 * ImGui::GetFontSize(), 40 * ImGui::GetFontSize()), ImGuiCond_Always); //ImGuiCond_FirstUseEver

		if (refresh_gui_docking == 0) 
		{
			//Need to be here while first time docking.
			ImGui::Begin("Entity Properties##PropertiesWindow", &bEntity_Properties_Window, iGenralWindowsFlags);
			ImGui::End();
		}
		else if (bEntity_Properties_Window) {
				

			if (t.widget.pickedEntityIndex > 0 && t.cameraviewmode == 2) {

				//We are in properties mode.
				grideleprof_uniqui_id = 35000;

				static int current_loaded_script = -1;
				static int current_selected_script = 0;
				static bool current_loaded_script_has_dlua = false;

				int iParentEntid = t.ttrygridentity;
					
				if (iOldPickedEntityIndex != t.widget.pickedEntityIndex) 
				{
					//PE: Setup "Materials" defaults.
					//PE: NOTE: we also need a way to get this info from t.entityelement[t.e].eleprof.WEMaterial
					//PE: should also be placed inside t.grideleprof
					int picked_object = t.gridentityobj;
					if (t.gridentityobj > 0 && t.gridentityobj < g_iObjectListCount ) 
					{
						if (g_ObjectList[t.gridentityobj])
						{
							if(t.grideleprof.WEMaterial.MaterialActive)
								Wicked_Set_Material_From_grideleprof(g_ObjectList[t.gridentityobj], 0);
							else
								Wicked_Set_Material_Defaults(g_ObjectList[t.gridentityobj], 0);
						}
					}

					iOldPickedEntityIndex = t.widget.pickedEntityIndex;

					//t.gridentity can be changed when keys like "r" is used , so make a backup.
					iOldgridentity = t.gridentity;

					// get voices sets
					if (g_voiceList_s.size() == 0) 
					{
						if (CreateListOfVoices() > 0) 
						{
							pCCPVoiceSet = g_voiceList_s[0].Get();
							CCP_SelectedToken = g_voicetoken[0];
						}
					}

					// entity may have voice preferences set to check that
					pCCPVoiceSet = t.grideleprof.voiceset_s.Get();
					CCP_Speak_Rate = t.grideleprof.voicerate;
					if (strlen(pCCPVoiceSet) > 0)
					{
						// find token for this voiceset
						for (int n = 0; n < g_voiceList_s.size(); n++)
						{
							if (stricmp(g_voiceList_s[n].Get(), pCCPVoiceSet) == NULL)
							{
								CCP_SelectedToken = g_voicetoken[n];
								break;
							}
						}
					}
					else
					{
						// default if blank
						if ( g_voiceList_s.size() > 0 ) 
							pCCPVoiceSet = g_voiceList_s[0].Get();
						else
							pCCPVoiceSet = "";
						if (g_voicetoken.size() > 0)
							CCP_SelectedToken = g_voicetoken[0];
						else
							CCP_SelectedToken = NULL;
						CCP_Speak_Rate = 0;
					}

					//Make sure to read DLUA.
					current_loaded_script = -1;
				}

				g_selected_editor_object = NULL;
				g_selected_editor_objectID = 0;
				if (iOldgridentity > 0) 
				{
					int picked_object = t.gridentityobj;
					if (picked_object > 0) 
					{
						if (picked_object < g_iObjectListCount)
						{
							if (g_ObjectList[picked_object])
							{
								if(t.gridentitystaticmode)
									g_selected_editor_color = XMSTATICCOLOR;
								else
									g_selected_editor_color = XMDYNAMICCOLOR;
								g_selected_editor_object = g_ObjectList[picked_object];
								g_selected_editor_objectID = picked_object;
							}
						}
					}
				}

				ImGui::Begin("Entity Properties##PropertiesWindow", &bEntity_Properties_Window, iGenralWindowsFlags);

				int media_icon_size = 64;
				ImGui::BeginChild("##cEntitiesPropertiesHeader", ImVec2(0, 0),false, iGenralWindowsFlags);
					
				ImGui::SetWindowFontScale(0.90);
				ImGui::PushItemWidth(ImGui::GetFontSize()*10.0);

				if (bEntity_Properties_Window)
				{
					t.gridentity = iOldgridentity;

					//Collect the flags to use.
					imgui_set_openproperty_flags(t.gridentity);
					int tflagtext = 0, tflagimage=0;

					LPSTR pAIRoot = "scriptbank\\";
					if (t.tflagai == 1)
					{
						if (g.quickparentalcontrolmode == 2)
						{
							if (t.entityprofile[t.gridentity].ismarker == 0)
							{
								if (t.tflagchar == 1)
									pAIRoot = "scriptbank\\people\\";
								else
									pAIRoot = "scriptbank\\objects\\";
							}
							else
							{
								pAIRoot = "scriptbank\\markers\\";
							}
						}
					}
						

					if (t.tsimplecharview == 1)
					{
						//  Wizard (simplified) property editing
						t.group = 0;
						if (ImGui::StyleCollapsingHeader("Character Info", ImGuiTreeNodeFlags_DefaultOpen)) {

							t.grideleprof.name_s = imgui_setpropertystring2(t.group, t.grideleprof.name_s.Get(), t.strarr_s[413].Get(), "Choose a unique name for this character");
							t.grideleprof.aimain_s = imgui_setpropertylist2(t.group, t.controlindex, t.grideleprof.aimain_s.Get(), "Behaviour", "Select a behaviour for this character", 11);
							t.grideleprof.soundset1_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset1_s.Get(), "Voiceover", "Select t.a WAV or OGG file this character will use during their behavior", "audiobank\\");
							t.grideleprof.ifused_s = imgui_setpropertystring2(t.group, t.grideleprof.ifused_s.Get(), "If Used", "Sometimes used to specify the name of an entity to be activated");
						}
					}
					else
					{

						bool bUnfoldAdvanced = false;
						ImGui::SetWindowFontScale(1.0);
						float fRegionWidth = ImGui::GetWindowContentRegionWidth();
						float textwidth;
						fPropertiesColoumWidth = 90.0f;
						int adv_flasgs = ImGuiTreeNodeFlags_DefaultOpen;
						if (g.vrqcontrolmode > 0) 
						{
							//Simple version.
							adv_flasgs = ImGuiTreeNodeFlags_None;
								
							//##################################################################
							//#### Simple , perhaps based on current .lua script.           ####
							//#### We need unique id here so add ##SimpleInput to all items ####
							//##################################################################
								
							if (t.entityprofile[t.gridentity].ischaracter > 0) {
								//Chars.

								if (ImGui::StyleCollapsingHeader("Character Properties", ImGuiTreeNodeFlags_DefaultOpen)) {

									//Display icon.
									if (t.entityprofile[iParentEntid].iThumbnailSmall > 0) {
										float w = ImGui::GetWindowContentRegionWidth();
										ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (media_icon_size*0.5), 0.0f));
										ImGui::ImgBtn(t.entityprofile[iParentEntid].iThumbnailSmall, ImVec2(media_icon_size, media_icon_size), drawCol_back, drawCol_normal, drawCol_normal, drawCol_normal, -1, 0, 0, 0, true);
									}

									ImGui::Indent(10);

									t.grideleprof.name_s = imgui_setpropertystring2(t.group, t.grideleprof.name_s.Get(), "Name", t.strarr_s[204].Get());
									//bSoundSet , Male/Female
									//PE: Type removed.
									//t.grideleprof.soundset_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset_s.Get(), "Type", t.strarr_s[255].Get(), "audiobank\\voices\\");

									ImGui::Indent(-10);
								}

								int speech_entries = 0;
								bool bUpdateMainString = false;

								for (int speech_loop = 0; speech_loop < 5; speech_loop++)
									speech_ids[speech_loop] = -1;
								//behavior
								if (ImGui::StyleCollapsingHeader("Character Behavior", ImGuiTreeNodeFlags_DefaultOpen)) {

									ImGui::Indent(10);

									ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
									ImGui::Text("Behaviors");
									ImGui::SameLine();
									ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
									ImGui::SetCursorPos(ImVec2(fPropertiesColoumWidth, ImGui::GetCursorPosY()));

									ImGui::PushItemWidth(-10);

									// scan PEOPLE folder for complete list of script
									std::vector<cstr> scriptList_s; scriptList_s.clear();
									std::vector<cstr> scriptListTitle_s; scriptListTitle_s.clear();
									cstr oldDir_s = GetDir();
									SetDir(g.fpscrootdir_s.Get());
									SetDir("Files\\scriptbank\\people");
									ChecklistForFiles();
									for ( int f = 1; f <= ChecklistQuantity(); f++)
									{
										cstr tfile_s = ChecklistString(f);
										LPSTR pFilename = tfile_s.Get();
										if (tfile_s != "." && tfile_s != "..")
										{
											if (strnicmp(pFilename + strlen(pFilename) - 4, ".lua", 4) == NULL)
											{
												// create a readable title from file
												char pTitleName[256];
												strcpy(pTitleName, pFilename);
												pTitleName[strlen(pTitleName) - 4] = 0;
												for (int n = 0; n < strlen(pTitleName); n++)
												{
													if (n == 0)
													{
														if (pTitleName[n] >= 'a' && pTitleName[n] <= 'z' )
															pTitleName[n] -= ('a' - 'A');
													}
													else
													{
														if (pTitleName[n] >= 'A' && pTitleName[n] <= 'Z' )
															pTitleName[n] += ('a' - 'A');
													}
													if (pTitleName[n] == '_') pTitleName[n] = ' ';
												}

												// add script and title to list
												scriptList_s.push_back(cstr("people\\")+tfile_s);
												scriptListTitle_s.push_back(cstr(pTitleName));
											}
										}
									}
									scriptList_s.push_back(cstr(""));
									scriptListTitle_s.push_back(cstr("Custom"));
									SetDir(oldDir_s.Get());

									// and create items list
									static int g_scriptpeople_item_count = 0;
									static char** g_scriptpeople_items = NULL;
									if (g_scriptpeople_item_count != scriptList_s.size())
									{
										if (g_scriptpeople_items)
										{
											for (int i = 0; i < g_scriptpeople_item_count; i++) SAFE_DELETE(g_scriptpeople_items[i]);
											SAFE_DELETE(g_scriptpeople_items);
										}
										g_scriptpeople_item_count = scriptList_s.size();
										g_scriptpeople_items = new char*[g_scriptpeople_item_count];
										for (int i = 0; i < g_scriptpeople_item_count; i++)
										{
											g_scriptpeople_items[i] = new char[256];
											strcpy(g_scriptpeople_items[i], scriptListTitle_s[i].Get());
										}
									}

									int item_current_type_selection = g_scriptpeople_item_count - 1; //Default Custom.
									for (int i = 0; i < g_scriptpeople_item_count - 1; i++) 
									{
										if (pestrcasestr(t.grideleprof.aimain_s.Get(), scriptList_s[i].Get())) 
										{
											item_current_type_selection = i;
											break;
										}
									}

									if (current_loaded_script != item_current_type_selection) 
									{
										//Load in lua and check for custom properties.
										cstr script_name_append = "";
										if (item_current_type_selection < g_scriptpeople_item_count - 1)
											script_name_append += (char *) scriptList_s[item_current_type_selection].Get();
										else
											script_name_append += t.grideleprof.aimain_s;

										cstr script_name = "";
										script_name = "scriptbank\\";
										script_name += script_name_append;

										//Try to parse script.
										ParseLuaScript(&t.grideleprof,script_name.Get());
										current_loaded_script = item_current_type_selection;

										if (t.grideleprof.PropertiesVariableActive == 1) 
										{
											bUpdateMainString = true;
											current_loaded_script_has_dlua = true;
										}
										else 
										{
											if (current_loaded_script_has_dlua) 
											{
												//Reset t.grideleprof.soundset4_s that contain the dlua calls.
												t.grideleprof.soundset4_s = "";
												current_loaded_script_has_dlua = false;
											}
										}
									}

									if (ImGui::Combo("##BehavioursSimpleInput", &item_current_type_selection, g_scriptpeople_items, g_scriptpeople_item_count, 20)) 
									{
										if (item_current_type_selection < g_scriptpeople_item_count - 1) 
										{
											t.grideleprof.aimain_s = scriptList_s[item_current_type_selection].Get();
										}
										else 
										{
											t.grideleprof.aimain_s = "";
										}
									}
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Select Character Behavior");

									ImGui::PopItemWidth();

									if (item_current_type_selection == g_scriptpeople_item_count - 1) 
									{
										//Custom script , display directly.
										std::string ms = t.strarr_s[417].Get();
										ms = "Script";
										cstr aim = t.grideleprof.aimain_s;
										t.grideleprof.aimain_s = imgui_setpropertyfile2(t.group, t.grideleprof.aimain_s.Get(), (char *)ms.c_str(), t.strarr_s[207].Get(), pAIRoot);
										if (aim != t.grideleprof.aimain_s)
											current_loaded_script = -1;
									}

									if (t.grideleprof.PropertiesVariableActive == 1) {
										speech_entries = DisplayLuaDescription(&t.grideleprof);
									}
									else {
										if (t.grideleprof.PropertiesVariable.VariableDescription.Len() > 0) {
											DisplayLuaDescriptionOnly(&t.grideleprof);
										}
									}

									ImGui::Indent(-10);
								}

								if(speech_entries > 0)
								{
									//@Lee all SPEECH control is moved to this function.
									SpeechControls(speech_entries, bUpdateMainString);
								}

								if (ImGui::StyleCollapsingHeader("Customize", ImGuiTreeNodeFlags_DefaultOpen)) {

									ImGui::Indent(10);

									ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
									ImGui::Text("Move Speed");
									ImGui::SameLine();
									ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
									ImGui::SetCursorPos(ImVec2(fPropertiesColoumWidth, ImGui::GetCursorPosY()));
									ImGui::PushItemWidth(-10);
									ImGui::MaxSliderInputInt("##Movement SpeedSimpleInput", &t.grideleprof.speed, 1, 500, "Set Movement Speed");

									if (t.playercontrol.thirdperson.enabled == 1) t.tanimspeed_f = t.entityelement[t.playercontrol.thirdperson.charactere].eleprof.animspeed;
									else t.tanimspeed_f = t.grideleprof.animspeed;

									ImGui::PopItemWidth();
									ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
									ImGui::Text("Anim Speed");
									ImGui::SameLine();
									ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
									ImGui::SetCursorPos(ImVec2(fPropertiesColoumWidth, ImGui::GetCursorPosY()));
									ImGui::PushItemWidth(-10);
									int tmpint = t.tanimspeed_f;
									ImGui::MaxSliderInputInt("##Animation Speed Simple", &tmpint, 1, 500, "Set Animation Speed");

									t.tanimspeed_f = tmpint;
									ImGui::PopItemWidth();
									if (t.playercontrol.thirdperson.enabled == 1) t.entityelement[t.playercontrol.thirdperson.charactere].eleprof.animspeed = t.tanimspeed_f;
									else t.grideleprof.animspeed = t.tanimspeed_f;

									ImGui::Indent(-10);
								}
							}
							else if (t.tflaglight == 1)
							{
								if (ImGui::StyleCollapsingHeader("Name", ImGuiTreeNodeFlags_DefaultOpen)) 
								{
									//Display icon.
									if (t.entityprofile[iParentEntid].iThumbnailSmall > 0) 
									{
										float w = ImGui::GetWindowContentRegionWidth();
										ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (media_icon_size*0.5), 0.0f));
										ImGui::ImgBtn(t.entityprofile[iParentEntid].iThumbnailSmall, ImVec2(media_icon_size, media_icon_size), drawCol_back, drawCol_normal, drawCol_normal, drawCol_normal, -1, 0, 0, 0, true);
									}
									ImGui::Indent(10);
									//Name and color setup only.
									ImGui::Text("");
									t.grideleprof.name_s = imgui_setpropertystring2(t.group, t.grideleprof.name_s.Get(), "Name", t.strarr_s[204].Get());
									ImGui::Indent(-10);
								}

								if (ImGui::StyleCollapsingHeader("Customize", ImGuiTreeNodeFlags_DefaultOpen)) {

									ImGui::Indent(10);

									ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
									ImGui::Text("Light Distance");
									ImGui::SameLine();
									ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
									ImGui::SetCursorPos(ImVec2(fPropertiesColoumWidth, ImGui::GetCursorPosY()));
									ImGui::PushItemWidth(-10);

									ImGui::SliderInt("##Light RangeSimpleInput", &t.grideleprof.light.range, 1, 3000);
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", t.strarr_s[250].Get() );
									ImGui::PopItemWidth();

									ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
									ImGui::Text("Light Color");
									ImGui::SameLine();
									ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
									ImGui::SetCursorPos(ImVec2(fPropertiesColoumWidth, ImGui::GetCursorPosY()));
									ImGui::PushItemWidth(-10);
									ImGui::Text(""); //place it below text, so it get larger.

									float colors[5];
									colors[3] = ((t.grideleprof.light.color & 0xff000000) >> 24) / 255.0f;
									colors[0] = ((t.grideleprof.light.color & 0x00ff0000) >> 16) / 255.0f;
									colors[1] = ((t.grideleprof.light.color & 0x0000ff00) >> 8) / 255.0f;
									colors[2] = (t.grideleprof.light.color & 0x000000ff) / 255.0f;
									ImGui::ColorPicker3("##Light ColorSimpleInput", colors, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoLabel);
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", t.strarr_s[251].Get() );

									colors[0] *= 255.0f;
									colors[1] *= 255.0f;
									colors[2] *= 255.0f;
									colors[3] *= 255.0f;
									t.grideleprof.light.color = 0xff000000 + ((unsigned int)colors[0] << 16) + ((unsigned int)colors[1] << 8) + +((unsigned int)colors[2]);
									ImGui::PopItemWidth();

									// update the light live
									if (t.gridentitywickedlightindex == 0)
									{
										int iLightType = 1;
										if (t.grideleprof.usespotlighting) iLightType = 2;
										t.gridentitywickedlightindex = WickedCall_AddLight(iLightType);
									}
									if (t.gridentitywickedlightindex > 0)
									{
										float lightx = t.gridentityposx_f;
										float lighty = t.gridentityposy_f;
										float lightz = t.gridentityposz_f;
										float lightax = t.gridentityrotatex_f;
										float lightay = t.gridentityrotatey_f;
										float lightaz = t.gridentityrotatez_f;
										float lightrange = t.grideleprof.light.range;
										float lightspotradius = t.grideleprof.light.offsetup;
										int colr = colors[0];
										int colg = colors[1];
										int colb = colors[2];
										bool bCastShadow = true;
										if (t.grideleprof.castshadow == 1) bCastShadow = false;
										WickedCall_UpdateLight(t.gridentitywickedlightindex, lightx, lighty, lightz, lightax, lightay, lightaz, lightrange, lightspotradius, colr, colg, colb, bCastShadow);
									}

									ImGui::Indent(-10);
								}
							}
							else if (t.entityprofile[t.gridentity].ismarker == 1) 
{
								//Start Marker.
								if (ImGui::StyleCollapsingHeader("Name", ImGuiTreeNodeFlags_DefaultOpen))
								{
									//Display icon.
									if (t.entityprofile[iParentEntid].iThumbnailSmall > 0) {
										float w = ImGui::GetWindowContentRegionWidth();
										ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (media_icon_size*0.5), 0.0f));
										ImGui::ImgBtn(t.entityprofile[iParentEntid].iThumbnailSmall, ImVec2(media_icon_size, media_icon_size), drawCol_back, drawCol_normal, drawCol_normal, drawCol_normal, -1, 0, 0, 0, true);
									}

									ImGui::Indent(10);

									//Name only.
									ImGui::Text("");
									t.grideleprof.name_s = imgui_setpropertystring2(t.group, t.grideleprof.name_s.Get(), "Name", t.strarr_s[204].Get());

									ImGui::Indent(-10);

								}

								// DLUA support added here.
								int speech_entries = 0;
								bool bUpdateMainString = false;
								for (int speech_loop = 0; speech_loop < 5; speech_loop++)
									speech_ids[speech_loop] = -1;
								if (current_loaded_script != current_selected_script) 
								{
									//Load in lua and check for custom properties.
									cstr script_name = "scriptbank\\";
									script_name += t.grideleprof.aimain_s;
									//Try to parse script.
									ParseLuaScript(&t.grideleprof, script_name.Get());
									current_loaded_script = current_selected_script;

									if (t.grideleprof.PropertiesVariableActive == 1) {
										bUpdateMainString = true;
										current_loaded_script_has_dlua = true;
									}
									else {
										if (current_loaded_script_has_dlua) {
											//Reset t.grideleprof.soundset4_s that contain the dlua calls.
											t.grideleprof.soundset4_s = "";
											current_loaded_script_has_dlua = false;
										}
									}

								}

								// Markers behaviours
								if (t.grideleprof.PropertiesVariableActive == 1 || t.grideleprof.PropertiesVariable.VariableDescription.Len() > 0)
								{
									if (ImGui::StyleCollapsingHeader("Behaviors", ImGuiTreeNodeFlags_DefaultOpen)) {

										ImGui::Indent(10);

										if (t.grideleprof.PropertiesVariableActive == 1) {
											speech_entries = DisplayLuaDescription(&t.grideleprof);
										}
										else {
											if (t.grideleprof.PropertiesVariable.VariableDescription.Len() > 0) {
												DisplayLuaDescriptionOnly(&t.grideleprof);
											}
										}

										ImGui::Indent(-10);
									}
								}

								if (speech_entries > 0)
								{
									// all SPEECH control is moved to this function.
									SpeechControls(speech_entries, bUpdateMainString);
								}


								if (ImGui::StyleCollapsingHeader("Customize", ImGuiTreeNodeFlags_DefaultOpen)) {

									ImGui::Indent(10);

									ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
									ImGui::Text("Player Speed");
									ImGui::SameLine();
									ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
									ImGui::SetCursorPos(ImVec2(fPropertiesColoumWidth, ImGui::GetCursorPosY()));

									ImGui::PushItemWidth(-10);

									//ImGui::SliderInt("##Movement SpeedSimpleInput", &t.grideleprof.speed, 1, 500);
									ImGui::MaxSliderInputInt("##Movement SpeedSimpleInput", &t.grideleprof.speed, 1, 500, "Set Player Speed");
									//if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Set Player Speed");

									if (t.playercontrol.thirdperson.enabled == 1) t.tanimspeed_f = t.entityelement[t.playercontrol.thirdperson.charactere].eleprof.animspeed;
									else t.tanimspeed_f = t.grideleprof.animspeed;

									ImGui::PopItemWidth();

									// Object Has Weapon
									if (t.tflaghasweapon == 1 && t.playercontrol.thirdperson.enabled == 0 && g.quickparentalcontrolmode != 2)
									{
										t.grideleprof.hasweapon_s = imgui_setpropertylist2c(t.group, t.controlindex, t.grideleprof.hasweapon_s.Get(), "Attachment", t.strarr_s[209].Get(), 1);
									}

									ImGui::Indent(-10);
								}
							}
							else 
							{
								//#################
								//#### Objects ####
								//################# 

								t.tokay = 1;
								if (ObjectExist(g.entitybankoffset + t.gridentity) == 1)
								{
									if (GetNumberOfFrames(g.entitybankoffset + t.gridentity) > 0)
									{
										t.tokay = 0;
									}
								}

								int speech_entries = 0;
								bool bUpdateMainString = false;

								for (int speech_loop = 0; speech_loop < 5; speech_loop++)
									speech_ids[speech_loop] = -1;

								//health.lua
								cstr aimain = t.grideleprof.aimain_s.Lower();
								//new: trigger anyting not a marker.
								if (t.entityprofile[t.gridentity].ismarker == 0 )// || aimain == "key.lua" || aimain == "objects\\key.lua" || aimain == "door.lua" || aimain == "default.lua" || aimain == "health.lua" || aimain == "pickuppable.lua" ) ) 
								{
									if (ImGui::StyleCollapsingHeader("Name", ImGuiTreeNodeFlags_DefaultOpen))
									{
										//Display icon.
										if (t.entityprofile[iParentEntid].iThumbnailSmall > 0) {
											float w = ImGui::GetWindowContentRegionWidth();
											ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (media_icon_size*0.5), 0.0f));
											ImGui::ImgBtn(t.entityprofile[iParentEntid].iThumbnailSmall, ImVec2(media_icon_size, media_icon_size), drawCol_back, drawCol_normal, drawCol_normal, drawCol_normal, -1, 0, 0, 0, true);
										}

										ImGui::Indent(10);

										t.grideleprof.name_s = imgui_setpropertystring2(t.group, t.grideleprof.name_s.Get(), "Name", t.strarr_s[204].Get());

										ImGui::Indent(-10);
									}
										
									// Object behaviours
									if (ImGui::StyleCollapsingHeader("Behaviors", ImGuiTreeNodeFlags_DefaultOpen))
									{
										ImGui::Indent(10);

										// scan OBJECTS folder for complete list of script
										std::vector<cstr> scriptList_s; scriptList_s.clear();
										std::vector<cstr> scriptListTitle_s; scriptListTitle_s.clear();
										cstr oldDir_s = GetDir();
										SetDir(g.fpscrootdir_s.Get());
										SetDir("Files\\scriptbank\\objects");
										ChecklistForFiles();
										for ( int f = 1; f <= ChecklistQuantity(); f++)
										{
											cstr tfile_s = ChecklistString(f);
											LPSTR pFilename = tfile_s.Get();
											if (tfile_s != "." && tfile_s != "..")
											{
												if (strnicmp(pFilename + strlen(pFilename) - 4, ".lua", 4) == NULL)
												{
													// create a readable title from file
													char pTitleName[256];
													strcpy(pTitleName, pFilename);
													pTitleName[strlen(pTitleName) - 4] = 0;
													for (int n = 0; n < strlen(pTitleName); n++)
													{
														if (n == 0)
														{
															if (pTitleName[n] >= 'a' && pTitleName[n] <= 'z' )
																pTitleName[n] -= ('a' - 'A');
														}
														else
														{
															if (pTitleName[n] >= 'A' && pTitleName[n] <= 'Z' )
																pTitleName[n] += ('a' - 'A');
														}
														if (pTitleName[n] == '_') pTitleName[n] = ' ';
													}

													// add script and title to list
													scriptList_s.push_back(cstr("objects\\")+tfile_s);
													scriptListTitle_s.push_back(cstr(pTitleName));
												}
											}
										}
										scriptList_s.push_back(cstr(""));
										scriptListTitle_s.push_back(cstr("Custom"));
										SetDir(oldDir_s.Get());

										// and create items list
										static int g_scriptobjects_item_count = 0;
										static char** g_scriptobjects_items = NULL;
										if (g_scriptobjects_item_count != scriptList_s.size())
										{
											if (g_scriptobjects_items)
											{
												for (int i = 0; i < g_scriptobjects_item_count; i++) SAFE_DELETE(g_scriptobjects_items[i]);
												SAFE_DELETE(g_scriptobjects_items);
											}
											g_scriptobjects_item_count = scriptList_s.size();
											g_scriptobjects_items = new char*[g_scriptobjects_item_count];
											for (int i = 0; i < g_scriptobjects_item_count; i++)
											{
												g_scriptobjects_items[i] = new char[256];
												strcpy(g_scriptobjects_items[i], scriptListTitle_s[i].Get());
											}
										}

										// find selection
										int item_current_type_selection = g_scriptobjects_item_count - 1; //Default Custom.
										for (int i = 0; i < g_scriptobjects_item_count - 1 ; i++) 
										{
											if (pestrcasestr(t.grideleprof.aimain_s.Get(), scriptList_s[i].Get())) 
											{
												item_current_type_selection = i;
												break;
											}
										}
												
										ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
										ImGui::Text("Behaviors");
										ImGui::SameLine();
										ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
										ImGui::SetCursorPos(ImVec2(fPropertiesColoumWidth, ImGui::GetCursorPosY()));
										ImGui::PushItemWidth(-10);

										if (ImGui::Combo("##Behaviours2SimpleInput", &item_current_type_selection, g_scriptobjects_items, g_scriptobjects_item_count, 20 ))
										{
											if (item_current_type_selection >= 0) 
												t.grideleprof.aimain_s = scriptList_s[item_current_type_selection];
											else 
												t.grideleprof.aimain_s = "default.lua";
											aimain = t.grideleprof.aimain_s.Lower();
										}
										if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Select Object Behavior");
										ImGui::PopItemWidth();

										if (current_loaded_script != item_current_type_selection) 
										{
											//Load in lua and check for custom properties.
											cstr script_name = "scriptbank\\";
											if (item_current_type_selection < g_scriptobjects_item_count - 1) //PE: Need to check for custom
												script_name += scriptList_s[item_current_type_selection];
											else
												script_name += t.grideleprof.aimain_s;
											//Try to parse script.
											ParseLuaScript(&t.grideleprof, script_name.Get());
											current_loaded_script = item_current_type_selection;

											if (t.grideleprof.PropertiesVariableActive == 1) 
											{
												bUpdateMainString = true;
												current_loaded_script_has_dlua = true;
											}
											else 
											{
												if (current_loaded_script_has_dlua) 
												{
													//Reset t.grideleprof.soundset4_s that contain the dlua calls.
													t.grideleprof.soundset4_s = "";
													current_loaded_script_has_dlua = false;
												}
											}
										}

										if (item_current_type_selection == g_scriptobjects_item_count - 1)
										{
											//Custom Behaviours , display directly.
											std::string ms = t.strarr_s[417].Get();
											ms = "Script";
											cstr aim = t.grideleprof.aimain_s;
											t.grideleprof.aimain_s = imgui_setpropertyfile2(t.group, t.grideleprof.aimain_s.Get(), (char *)ms.c_str(), t.strarr_s[207].Get(), pAIRoot);
											aimain = t.grideleprof.aimain_s.Lower();
											if (aim != t.grideleprof.aimain_s)
												current_loaded_script = -1;
										}

										if (t.grideleprof.PropertiesVariableActive == 1) 
										{
											speech_entries = DisplayLuaDescription(&t.grideleprof);
										}
										else 
										{
											if (t.grideleprof.PropertiesVariable.VariableDescription.Len() > 0) 
											{
												DisplayLuaDescriptionOnly(&t.grideleprof);
											}
										}

										ImGui::Indent(-10);
									}

									if (speech_entries > 0)
									{
										// all SPEECH control is moved to this function.
										SpeechControls(speech_entries, bUpdateMainString);
									}
								}
								else
									bUnfoldAdvanced = true;
							}
						}

						ImGui::SetWindowFontScale(1.0); //0.90

						fPropertiesColoumWidth = 120.0f; //Advanced coloum need to be larger as we have large fields.

						// All objects need 'certain fields' as pretty commmon  to have it for the script
						if (t.entityprofile[t.gridentity].ismarker == 0) // so as not to interfere with markers
						{
							bool bSound0Mentioned = false;
							bool bSound1Mentioned = false;
							bool bSound2Mentioned = false;
							bool bSound3Mentioned = false;
							bool bSound4Mentioned = false;
							bool bSound5Mentioned = false;
							bool bSound6Mentioned = false;
							bool bVideoSlotMentioned = false;
							bool bIfUsedMentioned = false;
							bool bUseKeyMentioned = false;
							bool bShootingWeaponMentioned = false;
							bool bMeleeWeaponMentioned = false;
							bool bUnarmedMentioned = false;
							int iAnimationSetMentioned = 0;
							char pCaptureAnyScriptDesc[10240 + (80 * 300) + (80 * 300)];
							strcpy(pCaptureAnyScriptDesc, t.grideleprof.PropertiesVariable.VariableDescription.Get());
							for (int i = 0; i < t.grideleprof.PropertiesVariable.iVariables; i++)
							{
								//strcat(pCaptureAnyScriptDesc, t.grideleprof.PropertiesVariable.VariableSectionDescription[i]);
								strcat(pCaptureAnyScriptDesc, t.grideleprof.PropertiesVariable.VariableSectionDescription[i].Get());
							}
							for (int i = 0; i < t.grideleprof.PropertiesVariable.iVariables; i++)
							{
								//strcat(pCaptureAnyScriptDesc, t.grideleprof.PropertiesVariable.VariableSectionEndDescription[i]);
								strcat(pCaptureAnyScriptDesc, t.grideleprof.PropertiesVariable.VariableSectionEndDescription[i].Get());
							}
							if (strstr(pCaptureAnyScriptDesc, "<Sound0>") != 0) bSound0Mentioned = true;
							if (strstr(pCaptureAnyScriptDesc, "<Sound1>") != 0) bSound1Mentioned = true;
							if (strstr(pCaptureAnyScriptDesc, "<Sound2>") != 0) bSound2Mentioned = true;
							if (strstr(pCaptureAnyScriptDesc, "<Sound3>") != 0) bSound3Mentioned = true;
							if (strstr(pCaptureAnyScriptDesc, "<Sound4>") != 0) bSound4Mentioned = true;
							if (strstr(pCaptureAnyScriptDesc, "<Sound5>") != 0) bSound5Mentioned = true;
							if (strstr(pCaptureAnyScriptDesc, "<Sound6>") != 0) bSound6Mentioned = true;
							if (strstr(pCaptureAnyScriptDesc, "<Video Slot>") != 0) bVideoSlotMentioned = true;
							if (strstr(pCaptureAnyScriptDesc, "<If Used>") != 0) bIfUsedMentioned = true;
							if (strstr(pCaptureAnyScriptDesc, "<Shooting Weapon>") != 0) bShootingWeaponMentioned = true;
							if (strstr(pCaptureAnyScriptDesc, "<Melee Weapon>") != 0) bMeleeWeaponMentioned = true;
							if (strstr(pCaptureAnyScriptDesc, "<Any Weapon>") != 0) { bShootingWeaponMentioned = true; bMeleeWeaponMentioned = true; }
							if (strstr(pCaptureAnyScriptDesc, "<Unarmed>") != 0) bUnarmedMentioned = true;
							if (strstr(pCaptureAnyScriptDesc, "<Soldier Animations>") != 0) iAnimationSetMentioned = 1;
							if (strstr(pCaptureAnyScriptDesc, "<Melee Animations>") != 0) iAnimationSetMentioned = 2;
							if (strstr(pCaptureAnyScriptDesc, "<Zombie Animations>") != 0) iAnimationSetMentioned = 3;
							if (strstr(pCaptureAnyScriptDesc, "<Default Animations>") != 0) iAnimationSetMentioned = 4;

							if (bVideoSlotMentioned == true) t.grideleprof.soundset1_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset1_s.Get(), "Video Slot", t.strarr_s[601].Get(), "videobank\\");
							if (bSound0Mentioned == true) t.grideleprof.soundset_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset_s.Get(), "Sound0", t.strarr_s[254].Get(), "audiobank\\");
							if (bSound1Mentioned == true) t.grideleprof.soundset1_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset1_s.Get(), "Sound1", t.strarr_s[254].Get(), "audiobank\\");
							if (bSound2Mentioned == true) t.grideleprof.soundset2_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset2_s.Get(), "Sound2", t.strarr_s[254].Get(), "audiobank\\");
							if (bSound3Mentioned == true) t.grideleprof.soundset3_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset3_s.Get(), "Sound3", t.strarr_s[254].Get(), "audiobank\\");
							// LB: corrected bad decision to assign sound4 to 4, 5 to 6, etc (now 4,5,6 align with the introduction of a new 4a string)
							if (bSound4Mentioned == true) t.grideleprof.soundset4a_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset4a_s.Get(), "Sound4", t.strarr_s[254].Get(), "audiobank\\");
							if (bSound5Mentioned == true) t.grideleprof.soundset5_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset5_s.Get(), "Sound5", t.strarr_s[254].Get(), "audiobank\\");
							if (bSound6Mentioned == true) t.grideleprof.soundset6_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset6_s.Get(), "Sound6", t.strarr_s[254].Get(), "audiobank\\");
							if (bIfUsedMentioned == true) t.grideleprof.ifused_s = imgui_setpropertystring2(t.group, t.grideleprof.ifused_s.Get(), t.strarr_s[437].Get(), t.strarr_s[226].Get());
							if (bUseKeyMentioned == true) t.grideleprof.usekey_s = imgui_setpropertystring2(t.group, t.grideleprof.usekey_s.Get(), t.strarr_s[436].Get(), t.strarr_s[225].Get());
							bool readonly = false;
							bool bFromCharacterCreator = false;
							if (bShootingWeaponMentioned == true || bMeleeWeaponMentioned == true)
							{
								extern void animsystem_weaponproperty (int, bool, entityeleproftype*, bool, bool);
								animsystem_weaponproperty(t.entityprofile[t.gridentity].characterbasetype, bFromCharacterCreator, &t.grideleprof, bShootingWeaponMentioned, bMeleeWeaponMentioned);
							}
							else if (bUnarmedMentioned)
							{
								extern void animsystem_weaponproperty(int, bool, entityeleproftype*, bool, bool);
								animsystem_weaponproperty(t.entityprofile[t.gridentity].characterbasetype, bFromCharacterCreator, &t.grideleprof, false, false);
							}
							if (iAnimationSetMentioned > 0)
							{
								extern void animsystem_animationsetproperty (int, bool, entityeleproftype*, int, int);
								animsystem_animationsetproperty(t.entityprofile[t.gridentity].characterbasetype, bFromCharacterCreator, &t.grideleprof, iAnimationSetMentioned, -1);
							}
						}

						sObject* pObject = NULL;
						if( t.gridentityobj > 0) pObject = g_ObjectList[t.gridentityobj];
						if (ObjectExist(g.entitybankoffset + t.gridentity) == 1 && pObject )
						{
							if (ImGui::StyleCollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen))
							{
								ImGui::Indent(10);
								{
									Wicked_Change_Object_Material((void*)pObject, 0, NULL , true , t.grideleprof.bUseFPESettings);

									if (ImGui::Checkbox("Always use Original Object Settings##2", &t.grideleprof.bUseFPESettings))
									{
										if (t.grideleprof.bUseFPESettings)
										{
											t.grideleprof.WEMaterial.MaterialActive = false;
										}
										else
										{
											Wicked_Copy_Material_To_Grideleprof((void*)pObject, 0);
											t.grideleprof.WEMaterial.MaterialActive = true;
										}
									}

								}
								ImGui::Indent(-10);
							}
						}

						// And finally the ADVANCED section
						bool bAdvencedOpen = false;
						if (g.vrqcontrolmode > 0) 
						{
							if(bUnfoldAdvanced)
								bAdvencedOpen = true;
							else if (ImGui::StyleCollapsingHeader("Advanced", adv_flasgs)) { //ImGuiTreeNodeFlags_None //ImGuiTreeNodeFlags_DefaultOpen
								bAdvencedOpen = true;
							}
						}
						else 
						{
							bAdvencedOpen = true;
						}
						if (bAdvencedOpen) 
						{
							t.group = 0;
							if (ImGui::StyleCollapsingHeader(t.strarr_s[412].Get(), ImGuiTreeNodeFlags_DefaultOpen)) 
							{
								if (bUnfoldAdvanced) 
								{
									//Display icon.
									if (t.entityprofile[iParentEntid].iThumbnailSmall > 0) 
									{
										float w = ImGui::GetWindowContentRegionWidth();
										ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (media_icon_size*0.5), 0.0f));
										ImGui::ImgBtn(t.entityprofile[iParentEntid].iThumbnailSmall, ImVec2(media_icon_size, media_icon_size), drawCol_back, drawCol_normal, drawCol_normal, drawCol_normal, -1, 0, 0, 0, true);
									}
								}
								if (bUnfoldAdvanced)
								{
									if (t.entityprofile[t.gridentity].ischaracter > 0)
									{
										t.grideleprof.name_s = imgui_setpropertystring2(t.group, t.grideleprof.name_s.Get(), "Name", t.strarr_s[204].Get());
									}
									else
									{
										if (t.entityprofile[t.gridentity].ismarker > 0)
										{
											if (t.entityprofile[t.gridentity].islightmarker > 0) {
												t.grideleprof.name_s = imgui_setpropertystring2(t.group, t.grideleprof.name_s.Get(), "Name", t.strarr_s[204].Get());
											}
											else {
												t.grideleprof.name_s = imgui_setpropertystring2(t.group, t.grideleprof.name_s.Get(), "Name", t.strarr_s[204].Get());
											}
										}
										else {
											t.grideleprof.name_s = imgui_setpropertystring2(t.group, t.grideleprof.name_s.Get(), "Name", t.strarr_s[204].Get());
										}
									}
								}

								if (t.entityprofile[t.gridentity].ismarker == 0 || t.entityprofile[t.gridentity].islightmarker == 1)
								{
									if (g.gentitytogglingoff == 0)
									{
										t.tokay = 1;
										if (ObjectExist(g.entitybankoffset + t.gridentity) == 1)
										{
											if (GetNumberOfFrames(g.entitybankoffset + t.gridentity) > 0)
											{
												t.tokay = 0;
											}
										}
										if (t.tokay == 1)
										{
											t.gridentitystaticmode = imgui_setpropertylist2(t.group, t.controlindex, Str(t.gridentitystaticmode), t.strarr_s[414].Get(), t.strarr_s[205].Get(), 0);
										}
									}
								}

								// 101016 - Additional General Parameters
								if (t.tflagchar == 0 && t.tflagvis == 1)
								{
									if (t.tflagsimpler == 0)
									{
										t.grideleprof.isocluder = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.isocluder), "Occluder", "Set to YES makes this object an occluder", 0); ++t.controlindex;
										t.grideleprof.isocludee = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.isocludee), "Occludee", "Set to YES makes this object an occludee", 0); ++t.controlindex;
									}
								}

								if (ImGui::IsAnyItemFocused()) 
								{
									bImGuiGotFocus = true;
								}
							}

							int speech_entries = 0;
							//Add DLUA here id
							if (bUnfoldAdvanced && t.entityprofile[t.gridentity].ismarker > 1) {
								//ismarker = 1 has its own function.
								//DLUA support added here.
								bool bUpdateMainString = false;
								for (int speech_loop = 0; speech_loop < 5; speech_loop++)
									speech_ids[speech_loop] = -1;
								if (current_loaded_script != current_selected_script) {
									//Load in lua and check for custom properties.
									cstr script_name = "scriptbank\\";
									script_name += t.grideleprof.aimain_s;
									//Try to parse script.
									ParseLuaScript(&t.grideleprof, script_name.Get());
									current_loaded_script = current_selected_script;

									if (t.grideleprof.PropertiesVariableActive == 1) {
										bUpdateMainString = true;
										current_loaded_script_has_dlua = true;
									}
									else {
										if (current_loaded_script_has_dlua) {
											//Reset t.grideleprof.soundset4_s that contain the dlua calls.
											t.grideleprof.soundset4_s = "";
											current_loaded_script_has_dlua = false;
										}
									}

								}

								if (t.grideleprof.PropertiesVariableActive == 1 || t.grideleprof.PropertiesVariable.VariableDescription.Len() > 0)
								{
									if (ImGui::StyleCollapsingHeader("Behavior", ImGuiTreeNodeFlags_DefaultOpen)) {

										ImGui::Indent(10);

										if (t.grideleprof.PropertiesVariableActive == 1) {
											speech_entries = DisplayLuaDescription(&t.grideleprof);
										}
										else {
											if (t.grideleprof.PropertiesVariable.VariableDescription.Len() > 0) {
												DisplayLuaDescriptionOnly(&t.grideleprof);
											}
										}

										ImGui::Indent(-10);
									}
								}

								if (speech_entries > 0)
								{
									// all SPEECH control is moved to this function.
									SpeechControls(speech_entries, bUpdateMainString);
								}

							}

							t.group = 1;
							if (ImGui::StyleCollapsingHeader(t.strarr_s[415].Get(), ImGuiTreeNodeFlags_DefaultOpen))
							{
								//  Basic AI
								if (t.tflagai == 1)
								{
									// can redirect to better folders if in g.quickparentalcontrolmode
									LPSTR pAIRoot = "scriptbank\\";
									if (g.quickparentalcontrolmode == 2)
									{
										if (t.entityprofile[t.gridentity].ismarker == 0)
										{
											if (t.tflagchar == 1)
												pAIRoot = "scriptbank\\people\\";
											else
												pAIRoot = "scriptbank\\objects\\";
										}
										else
										{
											pAIRoot = "scriptbank\\markers\\";
										}
									}
									cstr tmpvalue;
									tmpvalue = imgui_setpropertyfile2(t.group, t.grideleprof.aimain_s.Get(), t.strarr_s[417].Get(), t.strarr_s[207].Get(), pAIRoot);
									if (t.grideleprof.aimain_s != tmpvalue) 
									{
										t.grideleprof.aimain_s = tmpvalue;
										current_loaded_script = -1;
									}
								}


								//  Has Weapon
								if (t.tflaghasweapon == 1 && t.playercontrol.thirdperson.enabled == 0 && g.quickparentalcontrolmode != 2)
								{
									t.grideleprof.hasweapon_s = imgui_setpropertylist2c(t.group, t.controlindex, t.grideleprof.hasweapon_s.Get(), t.strarr_s[419].Get(), t.strarr_s[209].Get(), 1);
								}

								//  Is Weapon (FPGC - 280809 - filtered fpgcgenre=1 is shooter genre)
								if (t.tflagweap == 1 && g.fpgcgenre == 1)
								{
									t.grideleprof.damage = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.damage), t.strarr_s[420].Get(), t.strarr_s[210].Get()));
									t.grideleprof.accuracy = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.accuracy), t.strarr_s[421].Get(), "Increases the inaccuracy of conical distribution by 1/100th of t.a degree"));
									if (t.grideleprof.weaponisammo == 0)
									{
										t.grideleprof.reloadqty = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.reloadqty), t.strarr_s[422].Get(), t.strarr_s[212].Get()));
										t.grideleprof.fireiterations = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.fireiterations), t.strarr_s[423].Get(), t.strarr_s[213].Get()));
										t.grideleprof.range = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.range), "Range", "Maximum range of bullet travel"));
										t.grideleprof.dropoff = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.dropoff), "Dropoff", "Amount in inches of vertical dropoff per 100 feet of bullet travel"));
										t.grideleprof.clipcapacity = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.clipcapacity), "Clip Capacity", "The total maximum number of clips the player can carry for this weapon"));
									}
									else
									{
										t.grideleprof.lifespan = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.lifespan), t.strarr_s[424].Get(), t.strarr_s[214].Get()));
										t.grideleprof.throwspeed = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.throwspeed), t.strarr_s[425].Get(), t.strarr_s[215].Get()));
										t.grideleprof.throwangle = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.throwangle), t.strarr_s[426].Get(), t.strarr_s[216].Get()));
										t.grideleprof.bounceqty = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.bounceqty), t.strarr_s[427].Get(), t.strarr_s[217].Get()));
										t.grideleprof.explodeonhit = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.explodeonhit), t.strarr_s[428].Get(), t.strarr_s[218].Get(), 0);
									}
									if (t.tflagsimpler == 0)
									{
										t.grideleprof.usespotlighting = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.usespotlighting), "Spot Lighting", "Set whether emits dynamic spot lighting", 0);
									}
								}

								//  Is Character
								if (t.tflagchar == 1)
								{
									if (t.tflagsimpler == 0)
									{

										// 020316 - special check to avoid offering can take weapon if no HUD.X
										t.tfile_s = cstr("gamecore\\guns\\") + t.grideleprof.hasweapon_s + cstr("\\HUD.X");
										if (FileExist(t.tfile_s.Get()) == 1)
										{
											t.grideleprof.cantakeweapon = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.cantakeweapon), t.strarr_s[429].Get(), t.strarr_s[219].Get(), 0);
											//Take Weapon's Ammo
											cstr fieldname = t.strarr_s[430];
											if (fieldname == "Take Weapon's Ammo") fieldname = "Take Weapon Ammo"; //Need to be shorter.
											t.grideleprof.quantity = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.quantity), fieldname.Get() , t.strarr_s[220].Get()));
										}
										t.grideleprof.rateoffire = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.rateoffire), t.strarr_s[431].Get(), t.strarr_s[221].Get()));
									}
								}
								if (t.tflagquantity == 1 && g.quickparentalcontrolmode != 2)
								{
									t.grideleprof.quantity = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.quantity), t.strarr_s[432].Get(), t.strarr_s[222].Get()));
								}

								//  AI Extra
								if (t.tflagvis == 1 && t.tflagai == 1)
								{
									if (t.tflagchar == 1)
									{
										t.grideleprof.coneangle = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.coneangle), t.strarr_s[434].Get(), t.strarr_s[224].Get()));
										t.grideleprof.conerange = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.conerange), "View Range", "The range within which the AI may see the player. Zero triggers the characters default range."));
										t.grideleprof.ifused_s = imgui_setpropertystring2(t.group, t.grideleprof.ifused_s.Get(), t.strarr_s[437].Get(), t.strarr_s[226].Get());
										if (g.quickparentalcontrolmode != 2)
										{
											t.grideleprof.isviolent = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.isviolent), "Blood Effects", "Sets whether blood and screams should be used", 0);
										}
										if (t.tflagsimpler == 0)
										{
											t.grideleprof.colondeath = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.colondeath), "End Collision", "Set to NO switches off collision when die", 0);
										}
									}
									else
									{
										if (t.tflagweap == 0 && t.tflagammo == 0)
										{
											t.grideleprof.usekey_s = imgui_setpropertystring2(t.group, t.grideleprof.usekey_s.Get(), t.strarr_s[436].Get(), t.strarr_s[225].Get());
											if (t.tflagsimpler != 0 & t.entityprofile[t.gridentity].ismarker == 3 && t.entityprofile[t.gridentity].trigger.stylecolor == 1)
											{
												// only one level - no winzone chain option
											}
											else
											{
												t.grideleprof.ifused_s = imgui_setpropertystring2(t.group, t.grideleprof.ifused_s.Get(), t.strarr_s[437].Get(), t.strarr_s[226].Get());
											}
										}
									}
								}
								if (t.tflagifused == 1)
								{
									if (t.tflagusekey == 1)
									{
										t.grideleprof.usekey_s = imgui_setpropertystring2(t.group, t.grideleprof.usekey_s.Get(), t.strarr_s[436].Get(), t.strarr_s[225].Get());
									}
									if (t.tflagsimpler != 0 & t.entityprofile[t.gridentity].ismarker == 3 && t.entityprofile[t.gridentity].trigger.stylecolor == 1)
									{
										// only one level - no winzone chain option
									}
									else
									{
										t.grideleprof.ifused_s = imgui_setpropertystring2(t.group, t.grideleprof.ifused_s.Get(), t.strarr_s[438].Get(), t.strarr_s[227].Get());
									}
								}

							}

							if (t.tflagspawn == 1)
							{
								t.group = 1;
								if (ImGui::StyleCollapsingHeader(t.strarr_s[439].Get(), ImGuiTreeNodeFlags_DefaultOpen)) {

									t.grideleprof.spawnatstart = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.spawnatstart), t.strarr_s[562].Get(), t.strarr_s[563].Get(), 0);
								}
							}


							//  Statistics
							if ((t.tflagvis == 1 || t.tflagobjective == 1 || t.tflaglives == 1 || t.tflagstats == 1) && t.tflagweap == 0 && t.tflagammo == 0)
							{
								t.group = 1;
								if (ImGui::StyleCollapsingHeader(t.strarr_s[451].Get(), ImGuiTreeNodeFlags_DefaultOpen)) 
								{
									if (t.tflagplayersettings == 1)
									{
										t.playercontrol.jumpmax_f = atof(imgui_setpropertystring2(t.group, Str(t.playercontrol.jumpmax_f), "Jump Speed", "Sets the jump speed of the player which controls overall jump height"));
										t.playercontrol.gravity_f = atof(imgui_setpropertystring2(t.group, Str(t.playercontrol.gravity_f), "Gravity", "Sets the modified force percentage of the players own gravity"));
										t.playercontrol.fallspeed_f = atof(imgui_setpropertystring2(t.group, Str(t.playercontrol.fallspeed_f), "Fall Speed", "Sets the maximum speed percentage at which the player will fall"));
										t.playercontrol.climbangle_f = atof(imgui_setpropertystring2(t.group, Str(t.playercontrol.climbangle_f), "Climb Angle", "Sets the maximum angle permitted for the player to ascend a slope"));
										if (t.playercontrol.thirdperson.enabled == 0)
										{
											t.playercontrol.wobblespeed_f = atof(imgui_setpropertystring2(t.group, Str(t.playercontrol.wobblespeed_f), "Wobble Speed", "Sets the rate of motion applied to the camera when moving"));
											t.playercontrol.wobbleheight_f = atof(imgui_setpropertystring2(t.group, Str(t.playercontrol.wobbleheight_f * 100), "Wobble Height", "Sets the degree of motion applied to the camera when moving")) / 100.0f;
											t.playercontrol.footfallpace_f = atof(imgui_setpropertystring2(t.group, Str(t.playercontrol.footfallpace_f * 100), "Footfall Pace", "Sets the rate at which the footfall sound is played when moving")) / 100.0f;
										}
										t.playercontrol.accel_f = atof(imgui_setpropertystring2(t.group, Str(t.playercontrol.accel_f * 100), "Acceleration", "Sets the acceleration curve used when t.moving from t.a stood position")) / 100.0f;
									}
									if (t.tflagmobile == 1) 
									{
										t.grideleprof.isimmobile = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.isimmobile), t.strarr_s[457].Get(), t.strarr_s[247].Get(), 0);
									}
									if (t.tflagmobile == 1)
									{
										if (t.tflagsimpler == 0)
										{
											t.grideleprof.lodmodifier = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.lodmodifier), "LOD Modifier", "Modify when the LOD transition takes effect. The default value is 0, increase this to a percentage reduce the LOD effect."));
										}
									}
								}
							}

							//  Physics Data (non-multiplayer)
							if (t.entityprofile[t.gridentity].ismarker == 0 && t.entityprofile[t.gridentity].islightmarker == 0)
							{
								t.group = 1;
								if (ImGui::StyleCollapsingHeader(t.strarr_s[596].Get(), ImGuiTreeNodeFlags_DefaultOpen)) {

									if (t.grideleprof.physics != 1)  t.grideleprof.physics = 0;
									t.grideleprof.physics = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.physics), t.strarr_s[580].Get(), t.strarr_s[581].Get(), 0);
									t.grideleprof.phyalways = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.phyalways), t.strarr_s[582].Get(), t.strarr_s[583].Get(), 0);
									t.grideleprof.phyweight = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.phyweight), t.strarr_s[584].Get(), t.strarr_s[585].Get()));
									t.grideleprof.phyfriction = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.phyfriction), t.strarr_s[586].Get(), t.strarr_s[587].Get()));
									if (t.tflagsimpler == 0)
									{
										t.grideleprof.explodable = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.explodable), t.strarr_s[592].Get(), t.strarr_s[593].Get(), 0);
										t.grideleprof.explodedamage = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.explodedamage), t.strarr_s[594].Get(), t.strarr_s[595].Get()));
									}
								}
							}

							//  Ammo data (FPGC - 280809 - filtered fpgcgenre=1 is shooter genre
							if (g.fpgcgenre == 1)
							{
								if (t.tflagammo == 1 || t.tflagammoclip == 1)
								{
									if (ImGui::StyleCollapsingHeader(t.strarr_s[459].Get(), ImGuiTreeNodeFlags_DefaultOpen)) {
										t.grideleprof.quantity = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.quantity), t.strarr_s[460].Get(), t.strarr_s[249].Get()));
									}

								}
							}

							//  Light data
							if (t.tflaglight == 1)
							{
								if (ImGui::StyleCollapsingHeader(t.strarr_s[461].Get(), ImGuiTreeNodeFlags_DefaultOpen)) {
									t.grideleprof.light.range = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.light.range), t.strarr_s[462].Get(), t.strarr_s[250].Get())); //PE: 462=Light Range
										
									ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
									ImGui::Text(t.strarr_s[463].Get());
									ImGui::SameLine();
									ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
									ImGui::SetCursorPos(ImVec2(fPropertiesColoumWidth, ImGui::GetCursorPosY()));
									ImGui::PushItemWidth(-10);

									float colors[5];
									colors[3] = ((t.grideleprof.light.color & 0xff000000) >> 24) / 255.0f;
									colors[0] = ((t.grideleprof.light.color & 0x00ff0000) >> 16) / 255.0f;
									colors[1] = ((t.grideleprof.light.color & 0x0000ff00) >> 8) / 255.0f;
									colors[2] = (t.grideleprof.light.color & 0x000000ff) / 255.0f;
									ImGui::ColorEdit3("##LightColorSetupField", colors, 0);
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", t.strarr_s[251].Get());
									colors[0] *= 255.0f;
									colors[1] *= 255.0f;
									colors[2] *= 255.0f;
									colors[3] *= 255.0f;
									t.grideleprof.light.color = 0xff000000 + ((unsigned int)colors[0] << 16) + ((unsigned int)colors[1] << 8) + +((unsigned int)colors[2]);

									ImGui::PopItemWidth();

									if (t.tflagsimpler == 0)
									{
										t.grideleprof.usespotlighting = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.usespotlighting), "Spot Lighting", "Change dynamic light to spot lighting", 0);
									}
								}
							}

							//  Decal data
							if (t.tflagtdecal == 1)
							{
								t.propfield[t.group] = t.controlindex;
								//  Decal Particle data
								if (t.tflagdecalparticle == 1)
								{
									//++t.group; startgroup("Decal Particle"); t.controlindex = 0;
									if (ImGui::StyleCollapsingHeader("Decal Particle", ImGuiTreeNodeFlags_DefaultOpen)) {

										t.grideleprof.particleoverride = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.particleoverride), "Custom Settings", "Whether you wish to override default settings", 0);
										t.grideleprof.particle.offsety = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.offsety), "OffsetY", "Vertical adjustment of start position"));
										t.grideleprof.particle.scale = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.scale), "Scale", "A value from 0 to 100, denoting size of particle"));
										t.grideleprof.particle.randomstartx = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.randomstartx), "Random Start X", "Random start area"));
										t.grideleprof.particle.randomstarty = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.randomstarty), "Random Start Y", "Random start area"));
										t.grideleprof.particle.randomstartz = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.randomstartz), "Random Start Z", "Random start area"));
										t.grideleprof.particle.linearmotionx = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.linearmotionx), "Linear Motion X", "Constant motion direction"));
										t.grideleprof.particle.linearmotiony = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.linearmotiony), "Linear Motion Y", "Constant motion direction"));
										t.grideleprof.particle.linearmotionz = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.linearmotionz), "Linear Motion Z", "Constant motion direction"));
										t.grideleprof.particle.randommotionx = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.randommotionx), "Random Motion X", "Random motion direction"));
										t.grideleprof.particle.randommotiony = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.randommotiony), "Random Motion Y", "Random motion direction"));
										t.grideleprof.particle.randommotionz = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.randommotionz), "Random Motion Z", "Random motion direction"));
										t.grideleprof.particle.mirrormode = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.mirrormode), "Mirror Mode", "Set to one to reverse the particle"));
										t.grideleprof.particle.camerazshift = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.camerazshift), "Camera Z Shift", "Shift t.particle towards camera"));
										t.grideleprof.particle.scaleonlyx = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.scaleonlyx), "Scale Only X", "Percentage X over Y scale"));
										t.grideleprof.particle.lifeincrement = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.lifeincrement), "Life Increment", "Control lifespan of particle"));
										t.grideleprof.particle.alphaintensity = atol(imgui_setpropertystring2(t.group, Str(t.grideleprof.particle.alphaintensity), "Alpha Intensity", "Control alpha percentage of particle"));
										//  V118 - 060810 - knxrb - Decal animation setting (Added animation choice setting).
										t.grideleprof.particle.animated = imgui_setpropertylist2(t.group, t.controlindex, Str(t.grideleprof.particle.animated), "Animated Particle", "Sets whether the t.particle t.decal Texture is animated or static.", 0);
									}
								}
							}

							// moved Particle to main

							// Sound
							if (t.tflagsound == 1 || t.tflagsoundset == 1 || tflagtext == 1 || tflagimage == 1)
							{
								cstr group_text;
								if (tflagtext == 1 || tflagimage == 1)
								{
									if (tflagtext == 1) group_text = "Text";
									if (tflagimage == 1) group_text = "Image";
								}
								else
								{
									group_text = "Media";
								}
									
								if (speech_entries > 0)
								{
								}

								if (ImGui::StyleCollapsingHeader(group_text.Get(), ImGuiTreeNodeFlags_DefaultOpen))
								{
									if (g.fpgcgenre == 1)
									{
										if (t.entityprofile[t.gridentity].ischaracter > 0) 
										{
											t.grideleprof.soundset_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset_s.Get(), "Sound0", t.strarr_s[254].Get(), "audiobank\\");
										}
										else 
										{
											if (g.vrqcontrolmode != 0)
											{
												if (t.tflagsound == 1 && t.tflagsoundset != 1) 
												{
													//PE: changed from 469 to 467 , should be sound0
													t.grideleprof.soundset_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset_s.Get(), t.strarr_s[467].Get(), t.strarr_s[253].Get(), "audiobank\\");
												}
											}
											else
											{
												if (t.tflagsound == 1 && t.tflagsoundset != 1) 
												{
													t.grideleprof.soundset_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset_s.Get(), t.strarr_s[467].Get(), t.strarr_s[253].Get(), "audiobank\\");
												}
											}
											if (t.tflagsoundset == 1) 
											{
												t.grideleprof.soundset_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset_s.Get(), t.strarr_s[469].Get(), t.strarr_s[255].Get(), "audiobank\\voices\\");
											}
											if (tflagtext == 1) 
											{
												t.grideleprof.soundset_s = imgui_setpropertystring2(t.group, t.grideleprof.soundset_s.Get(), "Text to Appear", "Enter text to appear in-game");
											}
											if (tflagimage == 1) 
											{
												t.grideleprof.soundset_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset_s.Get(), "Image File", "Select image to appear in-game", "scriptbank\\images\\imagesinzone\\");
											}
										}

										if (t.tflagnosecond == 0)
										{
											if (t.tflagsound == 1 || t.tflagsoundset == 1)
											{
												//We got some missing translations.
												if (t.strarr_s[468] == "") t.strarr_s[468] = "Sound1";
												if (t.strarr_s[480] == "") t.strarr_s[480] = "Sound2";
												if (t.strarr_s[481] == "") t.strarr_s[481] = "Sound3";
												t.grideleprof.soundset1_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset1_s.Get(), t.strarr_s[468].Get(), t.strarr_s[254].Get(), "audiobank\\");
												t.grideleprof.soundset2_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset2_s.Get(), t.strarr_s[480].Get(), t.strarr_s[254].Get(), "audiobank\\");
												t.grideleprof.soundset3_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset3_s.Get(), t.strarr_s[481].Get(), t.strarr_s[254].Get(), "audiobank\\");
												//ImGui::TextCenter("Sound4");
												//ImGui::TextCenter("(repurposed)");
												t.grideleprof.soundset4a_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset4a_s.Get(), "Sound4", t.strarr_s[254].Get(), "audiobank\\");
												t.grideleprof.soundset5_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset5_s.Get(), "Sound5", t.strarr_s[254].Get(), "audiobank\\");
												t.grideleprof.soundset6_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset6_s.Get(), "Sound6", t.strarr_s[254].Get(), "audiobank\\");
											}
										}
									}
									else
									{
										if (t.tflagsoundset == 1)
										{
											t.grideleprof.soundset_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset_s.Get(), t.strarr_s[469].Get(), t.strarr_s[255].Get(), "audiobank\\voices\\");
										}
										else
										{
											t.grideleprof.soundset_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset_s.Get(), t.strarr_s[467].Get(), t.strarr_s[253].Get(), "audiobank\\"); ++t.controlindex;
										}
										t.grideleprof.soundset1_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset1_s.Get(), t.strarr_s[468].Get(), t.strarr_s[254].Get(), "audiobank\\"); ++t.controlindex;
									}
								}
							}

							// Video
							if (t.tflagvideo == 1)
							{
								if (ImGui::StyleCollapsingHeader(t.strarr_s[597].Get(), ImGuiTreeNodeFlags_DefaultOpen)) {

									t.grideleprof.soundset1_s = imgui_setpropertyfile2(t.group, t.grideleprof.soundset1_s.Get(), "Video Slot", t.strarr_s[601].Get(), "videobank\\");
								}
							}

							//  Third person settings
							if (t.tflagplayersettings == 1 && t.playercontrol.thirdperson.enabled == 1)
							{
								if (ImGui::StyleCollapsingHeader("Third Person", ImGuiTreeNodeFlags_DefaultOpen)) {

									t.livegroupforthirdperson = t.group;
									t.playercontrol.thirdperson.cameralocked = imgui_setpropertylist2(t.group, t.controlindex, Str(t.playercontrol.thirdperson.cameralocked), "Camera Locked", "Fixes camera height and angle for third person view", 0);
									t.playercontrol.thirdperson.cameradistance = atol(imgui_setpropertystring2(t.group, Str(t.playercontrol.thirdperson.cameradistance), "Camera Distance", "Sets the distance of the third person camera"));
									t.playercontrol.thirdperson.camerashoulder = atol(imgui_setpropertystring2(t.group, Str(t.playercontrol.thirdperson.camerashoulder), "Camera X Offset", "Sets the distance to shift the camera over shoulder"));
									t.playercontrol.thirdperson.cameraheight = atol(imgui_setpropertystring2(t.group, Str(t.playercontrol.thirdperson.cameraheight), "Camera Y Offset", "Sets the vertical height of the third person camera. If more than twice the camera distance, camera collision disables"));
									t.playercontrol.thirdperson.camerafocus = atol(imgui_setpropertystring2(t.group, Str(t.playercontrol.thirdperson.camerafocus), "Camera Focus", "Sets the camera X angle offset to align focus of the third person camera"));
									t.playercontrol.thirdperson.cameraspeed = atol(imgui_setpropertystring2(t.group, Str(t.playercontrol.thirdperson.cameraspeed), "Camera Speed", "Sets the retraction speed percentage of the third person camera"));
									t.playercontrol.thirdperson.camerafollow = imgui_setpropertylist2(t.group, t.controlindex, Str(t.playercontrol.thirdperson.camerafollow), "Run Mode", "If set to yes, protagonist uses WASD t.movement mode", 0);
									t.playercontrol.thirdperson.camerareticle = imgui_setpropertylist2(t.group, t.controlindex, Str(t.playercontrol.thirdperson.camerareticle), "Show Reticle", "Show the third person 'crosshair' reticle Dot ( ", 0);
								}
							}

						}
					} //Advenced open


					if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) 
					{
						//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
						ImGui::Text("");
						ImGui::Text("");
					}


					ImRect bbwin(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize());
					if (ImGui::IsMouseHoveringRect(bbwin.Min, bbwin.Max))
					{
						bImGuiGotFocus = true;
					}
					if (ImGui::IsAnyItemFocused()) {
						bImGuiGotFocus = true;
					}
				}
				ImGui::PopItemWidth();
				ImGui::SetWindowFontScale(1.0);

				ImGui::EndChild();

				CheckMinimumDockSpaceSize(250.0f);

				ImGui::End();

				if(!bEntity_Properties_Window) //Window closed.
					iOldPickedEntityIndex = -1;

				if (t.inputsys.mclick == 1 && ImGui::IsWindowHovered()) 
				{
					//Click start , block until mouse is release.
					bProperties_Window_Block_Mouse = true;
				}

			}
			else {
				iOldPickedEntityIndex = -1;
			}
		}
		else 
		{
			//PRoperties closed , check if we need to exit zoommode.
			if (t.gridentityinzoomview > 0) 
			{
				t.tpressedtoleavezoommode = 2; //Exit zoom and save.
				if (t.grideditselect < 3 || t.grideditselect > 4) 
				{
					//Make sure to exit fast.
					int igridentity = t.gridentity;

					if( iOldgridentity != t.gridentity && iOldgridentity > -1)
						t.gridentity = iOldgridentity;

					int olges = t.grideditselect;
					t.grideditselect = 4;
					editor_viewfunctionality();
					t.grideditselect = olges;
					t.gridentity = igridentity;
				}
			}

			//PE: Bug if open properties, close and delete object, then add object and properties, failed and use old id.
			iOldPickedEntityIndex = -1;
		}


		//####################################
		//#### Procedural Level Generator ####
		//####################################

		//No resetting needed fixed.
		void procedural_new_level(void);
		procedural_new_level();

		//###########################
		//#### External Entities ####
		//###########################

		static std::map<std::string, std::int32_t> entity_folders;

		//PE: Exactly fit for 9 normal object, and 6 that include dlua description.
		if (refresh_gui_docking == 0 || bResetObjectLibrarySize) ImGui::SetNextWindowSize(ImVec2(66 * ImGui::GetFontSize(), (43 * ImGui::GetFontSize()) + 19.0), ImGuiCond_Always); //ImGuiCond_FirstUseEver
		if (refresh_gui_docking == 0 || bResetObjectLibrarySize) ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
		if (refresh_gui_docking == 0) init_Left_Categories_Column_Width = 3;
		ImGuiWindowFlags ex_window_flags = 0;
		if (refresh_gui_docking == 0 && !bExternal_Entities_Window)
		{
			ImGui::Begin("##Object Library ExternalWindow", &bExternal_Entities_Window, ex_window_flags);
			ImGui::End();
		}
		if (refresh_gui_docking <= 2 )
			pref.iSetColumnsEntityLib = 3;

		#ifndef GGMAXEDU
		// ensure workshop system always called to handle callbacks and ensure latest items are available when editing
		extern void workshop_update(bool);
		workshop_update(false);
		#endif

		// librray system
		process_entity_library_v2();

		// collect an entire list of all relevant filders (entitybank, scriptbank, images, particles, etc)
		mapeditorexecutable_full_folder_refresh();

		//########################
		//#### Level Entities ####
		//########################

		if (iDragDropActive > 0)
			iDragDropActive--;

		//PE: V2SEARCHBAR will display the new search layout.
		#define V2SEARCHBAR

		if (refresh_gui_docking == 0) 
		{
			//ImGuiWindowFlags_NoDocking
			bool bOpen = true;
			ImGui::Begin(TABENTITYNAME, &bOpen, iGenralWindowsFlags);
			ImGui::End();
			ImGui::Begin("Current Objects##AdditionalIconsWindow", &bOpen, iGenralWindowsFlags);
			ImGui::End();
		}
		if (refresh_gui_docking > 0) 
		{
			//PE: Not sure if these should be there so ...
			#define REMOVE_NEWEST_AND_OLDEST
			#define SORT_LEVELITEMS 0 
			#define SORT_PROJECTSITEMS 1
			#define SORT_DETAILEDITEMS 2
			#define SORT_OLDESTITEMS 7
			#define SORT_NEWESTITEMS 6
			#define SORT_GROUPITEMS 3
			#define SORT_INSTANCEITEMS 4
			#define SORT_BEHAVIORITEMS 5
			static bool bProjectItems = false;

			bool bToolTipActive = true;
			if (pref.iEnableDragDropEntityMode && bDraggingActive)
			{
				if (t.gridentity != 0 || t.gridentityobj != 0)
				{
					bToolTipActive = false;
				}
			}
			if (bTrashcanIconActive || bTrashcanIconActive2)
				bToolTipActive = false;

			int iWinFlags = 0;
			bool bAlwaysOpen = true;
			ImGui::Begin(TABENTITYNAME, &bAlwaysOpen, iGenralWindowsFlags | ImGuiWindowFlags_NoScrollbar); //, &bAlwaysOpen, iWinFlags);
			static char cSearchEntities[1024] = "\0";

			ImGui::BeginChild("##ChirlEntitiesLeftPanel", ImVec2(ImGui::GetWindowSize().x - 2.0f, fsy*2.0), false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavInputs); //ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar

			ImGui::PushItemWidth(-1);

			CheckTutorialAction("+##+", 8.0f); //Tutorial: check if we are waiting for this action

			if (ImGui::StyleButton("Add##+", ImVec2(ImGui::GetWindowSize().x *0.19, fsy*1.5)))
			{
				if (bTutorialCheckAction) TutorialNextAction(); //Clicked get next tutorial action.
				//Open Add item page.
				cFolderItem *pSearchFolder = &MainEntityList;
				pSearchFolder = pSearchFolder->m_pNext;
				while (pSearchFolder)
				{
					if (pSearchFolder->m_pFirstFile) 
					{
						cFolderItem::sFolderFiles * searchfiles = pSearchFolder->m_pFirstFile->m_pNext;
						while (searchfiles) 
						{
							searchfiles->iFlags = 0;
							searchfiles = searchfiles->m_pNext;
						}
					}
					pSearchFolder = pSearchFolder->m_pNext;
				}

				bExternal_Entities_Window = true;
				iDisplayLibraryType = 0;
				iDisplayLibrarySubType = 0;
			}
			if (bToolTipActive && ImGui::IsItemHovered()) ImGui::SetTooltip("Click here to add a new object to the game level");

			// when click ADD, have the option to refresh library (activated in developer mode for users who import often as it is a performance hit)
			if (g_iRefreshLibraryFoldersAfterDelay > 0)
			{
				g_iRefreshLibraryFoldersAfterDelay--;
				if (g_iRefreshLibraryFoldersAfterDelay == 2)
				{
					strcpy(cTriggerMessage, "Refreshing Library Lists ...");
					bTriggerMessage = true;
				}
				if (g_iRefreshLibraryFoldersAfterDelay == 0)
				{
					g_iRefreshLibraryFolders = 2;
				}
			}
			if (g_iRefreshLibraryFolders != 0 )
			{
				timestampactivity(0, "REFRESHING LIBRARY FOLDERS");
				extern void RefreshPurchasedFolder (void);
				RefreshPurchasedFolder();
				// force the purchased cateogry to show up (and also cause needed refresh)
				extern void process_gotopurchaedandrefreshtopurchases (bool bForceSearch);
				process_gotopurchaedandrefreshtopurchases(false);
				// trigger folder tree on left of library to recalculate in case of new folders (audiobank\xx)
				extern bool bTreeViewInitInNextFrame;
				bTreeViewInitInNextFrame = true;
				// also update the gun list, we might have new weapons
				timestampactivity(0, "RESCANNING G-LIST");
				extern bool g_bGunListNeedsRefreshing;
				g_bGunListNeedsRefreshing = true;
				gun_scaninall_findnewlyaddedgun();
				decal_scaninall_findnewlyaddedgun();
				if (g_iRefreshLibraryFolders == 1)
				{
					// View All (instead of Purchased View)
					bSelectLibraryViewAll = true;
				}
				g_iRefreshLibraryFolders = 0;
			}
		
			ImGui::SameLine();
			ImGui::PopItemWidth();

			static bool bToggleThumbViews = true;
			int iToggleIcon;
			if (bToggleThumbViews)
				iToggleIcon = ENTITY_EYE_ON;
			else
				iToggleIcon = ENTITY_EYE_OFF;

			float iconoffsetx = -7.0f;
			float iconoffsety = -4.0f;
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(iconoffsetx, iconoffsety));
			if (ImGui::ImgBtn(iToggleIcon, ImVec2(23, 23), ImColor(0, 0, 0, 0), ImColor(220, 220, 220, 220), ImColor(255, 255, 255, 255),
				ImColor(180, 180, 160, 255), -1, 0, 0, 0, false, false, false, false, false))
			{
				bToggleThumbViews = 1 - bToggleThumbViews;
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle on off thumbnails view.");


			ImGui::SameLine();
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(iconoffsetx-1,abs(iconoffsety)));

			float fDropDownWidth = 0.0f;
			fDropDownWidth = 34.0f;
			ImGui::PushItemWidth(-6 - fDropDownWidth);
			window = ImGui::GetCurrentWindow();

			ImVec2 current_pos = ImGui::GetCursorPos();
			float current_item_width = ImGui::GetItemRectSize().x;

			#ifdef V2SEARCHBAR
			float fOldSpacing = style.FramePadding.x;
			style.FramePadding.x = 22.0; //Make room for search icon.
			ImVec2 vSearchPos = ImGui::GetCursorPos();
			#endif

			ImVec2 search_icon_pos = window->DC.CursorPos + ImVec2(ImGui::GetContentRegionAvailWidth() - 10.0 - 16.0 - fDropDownWidth,2.0f);
			ImGui::SetItemAllowOverlap();
			static bool bSetKeyboardFocus = false;
			// Force the keyboard focus to the input text field when the user presses the clear search button (set below).
			if (bSetKeyboardFocus)
			{
				ImGui::SetKeyboardFocusHere(0);
				bSetKeyboardFocus = false;
			}
			// only for object based lists, not groups/behaviors/etc
			LPSTR pToolTipForSearch = "Cannot search this list!";

			bool bIsSearchAble = false;
			if (current_sort_order == SORT_LEVELITEMS || current_sort_order == SORT_NEWESTITEMS || current_sort_order == SORT_OLDESTITEMS || current_sort_order == SORT_DETAILEDITEMS)
				bIsSearchAble = true;

			if (bIsSearchAble)
			{
				pToolTipForSearch = "Type here to search for an object in your level";
			}
			if (ImGui::InputText(" ##cSearchEntities", &cSearchEntities[0], MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (bIsSearchAble)
				{
					if (strlen(cSearchEntities) > 1)
					{
						bool already_there = false;
						for (int l = 0; l < MAXSEARCHHISTORY; l++) {
							if (strcmp(cSearchEntities, pref.small_search_history[l]) == 0) {
								already_there = true;
								break;
							}
						}
						if (!already_there) {
							bool foundspot = false;
							for (int l = 0; l < MAXSEARCHHISTORY; l++) {
								if (strlen(pref.small_search_history[l]) <= 0) {
									strcpy(pref.small_search_history[l], cSearchEntities);
									foundspot = true;
									break;
								}
							}
							if (!foundspot) {
								//Move entry list.
								for (int l = 0; l < MAXSEARCHHISTORY; l++) {
									strcpy(pref.small_search_history[l], pref.small_search_history[l + 1]);
								}
								strcpy(pref.small_search_history[MAXSEARCHHISTORY - 1], cSearchEntities);
							}
						}
					}
				}
			}
			if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered() && bToolTipActive) ImGui::SetTooltip("%s", pToolTipForSearch);
			if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImVec2 restore_pos = ImGui::GetCursorPos();
			current_pos.y = ImGui::GetCursorPosY();

			#ifdef V2SEARCHBAR
			style.FramePadding.x = fOldSpacing;
			//Only display closebut if we have room.
			if (restore_pos.x > 110 ) // ? not sure if we only activate after search begin ? && strlen(cSearchEntities) > 0
			{
				ImGui::SetItemAllowOverlap();
				if (ImGui::CloseButton(ImGui::GetCurrentWindow()->GetID("#ClearSearchv2"), ImGui::GetWindowPos() + ImGui::GetCursorPos() + ImVec2(-38, 0)))
				{
					strcpy(cSearchEntities, "");
					bSetKeyboardFocus = true;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Clear search");
				ImGui::SameLine();
			}
			#endif

			static bool bSearchWinToggle = false;
			bool search_img_hovered = false, search_img_held = false;
			ID3D11ShaderResourceView* lpTexture = GetImagePointerView(TOOL_ENT_SEARCH);
			if (lpTexture)
			{
				ImGui::SetItemAllowOverlap();
				#ifdef V2SEARCHBAR
				ImVec2 search_icon_pos = ImGui::GetWindowPos() + vSearchPos + ImVec2(3.0, 3.0);
				#endif
				ImRect bb(search_icon_pos, search_icon_pos + ImVec2(16, 16));
				ImGui::PushID(TOOL_ENT_SEARCH);
				const ImGuiID id = window->GetID("#image");
				ImGui::PopID();
				ImGui::ItemSize(bb);
				if (ImGui::ItemAdd(bb, id)) {
					bool pressed = ImGui::ButtonBehavior(bb, id, &search_img_hovered, &search_img_held);
					if (pressed) {
						bSearchWinToggle = 1 - bSearchWinToggle;
					}
					if (ImGui::IsItemHovered()) {
						ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
					}
					ImGuiWindow* window = ImGui::GetCurrentWindow();
					window->DrawList->AddImage((ImTextureID)lpTexture, search_icon_pos, search_icon_pos + ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1.0, 1.0, 1.0, 1.0)));
				}
			}

			if (bSearchWinToggle)
			{
				//Do we have a search history.
				bool display_history = true; //false;
				if (display_history)
				{
					ImGui::SameLine();
					
					ImGui::SetNextWindowPos(ImGui::GetWindowPos() + current_pos);
					if (ImGui::Begin("##searchselectpopup", &bSearchWinToggle, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_Tooltip )) //| ImGuiWindowFlags_Tooltip
					{
						#ifndef V2SEARCHBAR
						if( ImGui::StyleButton("Clear Search Field",ImVec2(ImGui::GetContentRegionAvailWidth(),0.0f) ) )
						{
							strcpy(cSearchEntities, "");
							bSearchWinToggle = false;
						}
						ImGui::Separator();
						#endif

						//ImGui::Indent(10);
						for (int l = 0; l < MAXSEARCHHISTORY; l++) {
							if (strlen(pref.small_search_history[l]) > 0) {
								bool is_selected = false;
								if (ImGui::Selectable(pref.small_search_history[l], is_selected)) {
									strcpy(cSearchEntities, pref.small_search_history[l]);
									bSearchWinToggle = false;
								}
							}
						}
					}
					ImGui::End();
				}
			}

			ImGui::SetCursorPos(current_pos);

			if (!search_img_hovered)
			{
				if (ImGui::IsMouseReleased(0)) //ImGui::IsAnyMouseDown())
				{
					bSearchWinToggle = false;
				}
			}

			ImGui::SetCursorPos(ImVec2(restore_pos.x-8.0f, restore_pos.y));

			#ifdef REMOVE_NEWEST_AND_OLDEST
			const char* sortby_modes[] = { "Level Items", "Project Items", "Detailed Object List", "Group List", "Instance List", "Behavior List"};
			#else
			const char* sortby_modes[] = { "Level Items", "Project Items", "Detailed Object List", "Group List", "Instance List", "Behavior List", "Newest" , "Oldest" };
			#endif
			int isortbySize = IM_ARRAYSIZE(sortby_modes);
			if (!pref.iEnableAdvancedEntityList)
				isortbySize--;

			int comboflags = ImGuiComboFlags_NoPreview | ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_HeightLarge;
			ImGui::PushItemWidth(-24);
			bool bUpdateProjectFiles = false;

			if (bProjectItems && current_sort_order == SORT_LEVELITEMS)
			{
				current_sort_order = SORT_PROJECTSITEMS;
			}

			if (ImGui::BeginCombo("##combosortbymodesentotyleft", sortby_modes[current_sort_order], comboflags))
			{
				for (int n = 0; n < isortbySize; n++)
				{
					bool is_selected = (current_sort_order == n);
					if (ImGui::Selectable(sortby_modes[n], is_selected)) 
					{
						current_sort_order = n;
						if (n == SORT_PROJECTSITEMS)
						{
							//PE: Use same list , just add all.
							bProjectItems = true;
							bUpdateProjectFiles = true;
							current_sort_order = SORT_LEVELITEMS;
						}
						else if( n == SORT_LEVELITEMS)
						{
							bProjectItems = false;
							bUpdateProjectFiles = true;
						}
						
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::EndChild();

			if (bProjectItems && current_sort_order == SORT_PROJECTSITEMS)
			{
				current_sort_order = SORT_LEVELITEMS;
			}

			ImVec2 content_avail = ImVec2(0.0, 0.0);
			content_avail = ImGui::GetContentRegionAvail();
			content_avail.y -= fsy * 3.0;
			if (content_avail.y < fsy) content_avail.y = fsy;
			
			// Size of icons.
			int entity_icons = 12; if (pref.iObjectEnableAdvanced)	entity_icons = 13;
			int entity_icons_columns = 6;
			float entity_w = ImGui::GetContentRegionAvailWidth() - 10.0f;
			float fSpacer = 0.0f;
			float entity_image_size = entity_w / (float)entity_icons_columns;
			entity_image_size -= ((1.125f) * entity_icons_columns);
			if (entity_w > 680)
			{
				//Switch to 15 per row.
				entity_icons = 15;
				entity_icons_columns = entity_icons;// 12;
				entity_image_size = entity_w / (float)entity_icons_columns;
				entity_image_size -= 7.5f;
			}
			else if (entity_w > 360)
			{
				//Switch to 12 per row.
				entity_icons_columns = entity_icons;// 12;
				entity_image_size = entity_w / (float)entity_icons_columns;
				entity_image_size -= 7.5f;
			}		

			int iIconRows = (entity_icons + (entity_icons_columns - 1)) / entity_icons_columns;
			if (content_avail.x <= 1)
			{
				content_avail = ImGui::GetContentRegionAvail();
				content_avail.y -= (fsy*0.5);
			}
			content_avail.y -= 3.0f;
			content_avail.y -= ((entity_w / entity_icons_columns) * iIconRows);
			content_avail.y -= 10.0f;
			if (entity_icons_columns > 10)
				content_avail.y -= entity_image_size;

			static bool bViewOptionsOpen = false;
			if(bViewOptionsOpen)
				ImGui::BeginChild("##MainEntitiesLeftPanel", content_avail - ImVec2(0.0f, 205.0f), false, iGenralWindowsFlags); //, false, ImGuiWindowFlags_HorizontalScrollbar);
			else
				ImGui::BeginChild("##MainEntitiesLeftPanel", content_avail, false, iGenralWindowsFlags); //, false, ImGuiWindowFlags_HorizontalScrollbar);
			
			if(bProjectItems && current_sort_order == SORT_LEVELITEMS)
				ImGui::TextCenter("Project Items");
			else
				ImGui::TextCenter(sortby_modes[current_sort_order]);


			static std::vector<std::pair<std::string,int>> sorted_entity_files;

			static int last_entidmaster = 0;
			static int last_include_icon_set = -1;

			int iMasterEntid = g.entidmaster;
			if (iRestoreEntidMaster >= 0 && bExternal_Entities_Window)
			{
				iMasterEntid = iRestoreEntidMaster;
			}

			if (current_sort_order == SORT_LEVELITEMS && !bProjectItems)
			{
				static int last_entityelementlist = 0;
				if (last_entityelementlist != g.entityelementlist)
				{
					last_entityelementlist = g.entityelementlist;
					bUpdateProjectFiles = true;
				}
				extern bool bUpdateObjectList;
				if (bUpdateObjectList)
				{
					bUpdateObjectList = false;
					bUpdateProjectFiles = true;
				}
			}
			

			static int last_sortby = -1;
			if (bUpdateProjectFiles || last_entidmaster != iMasterEntid || last_include_icon_set != iIncludeLeftIconSet || current_sort_order != last_sortby )
			{
				//Sort new list.
				sorted_entity_files.clear();
				if (iMasterEntid >= 1)
				{
					if (current_sort_order == SORT_GROUPITEMS)
					{
						// Sort group list
						for (int gi = 0; gi < MAXGROUPSLISTS; gi++)
						{
							if (vEntityGroupList[gi].size() > 0)
							{
								std::string stmp = "Group " + std::to_string(gi);
								sorted_entity_files.push_back(std::make_pair(stmp, gi));
							}
						}
					}
					else
					{
						if (current_sort_order == SORT_INSTANCEITEMS || current_sort_order == SORT_BEHAVIORITEMS)
						{
							if (current_sort_order == SORT_INSTANCEITEMS)
							{
								// Sort by instance ID order
								for (t.e = 1; t.e <= g.entityelementlist; t.e++)
								{
									cstr scriptname = t.entityelement[t.e].eleprof.name_s;
									std::string stmp = Lower(scriptname.Get());
									if (t.entityelement[t.e].bankindex > 0)
									{
										sorted_entity_files.push_back(std::make_pair(stmp, t.e));
									}
								}
							}
							if (current_sort_order == SORT_BEHAVIORITEMS)
							{
								// Sort by behavior list
								for (t.e = 1; t.e <= g.entityelementlist; t.e++)
								{
									cstr scriptname = t.entityelement[t.e].eleprof.aimain_s;
									std::string stmp = Lower(scriptname.Get());
									bool bFoundThisScriptInList = false;
									for (int i = 0; i < sorted_entity_files.size(); i++)
									{
										if (sorted_entity_files[i].first == stmp)
										{
											// only add a unique one (ie list of behaviours)
											bFoundThisScriptInList = true;
											break;
										}
									}
									if (bFoundThisScriptInList == false)
									{
										sorted_entity_files.push_back(std::make_pair(stmp, t.e));
									}
								}
							}
						}
						else
						{
							// Sort entity parent list.
							for (t.entid = 1; t.entid <= iMasterEntid; t.entid++)
							{
								std::string stmp = Lower(t.entityprofileheader[t.entid].desc_s.Get());
								if (current_sort_order == SORT_NEWESTITEMS || current_sort_order == SORT_OLDESTITEMS)
								{
									//Convert to sortable by string.
									if (t.entid < 10)
										stmp = "000" + std::to_string(t.entid);
									else if (t.entid < 100)
										stmp = "00" + std::to_string(t.entid);
									else if (t.entid < 1000)
										stmp = "0" + std::to_string(t.entid);
									else
										stmp = std::to_string(t.entid);
								}
								stmp += "###"; //We need it to be unique so add this.
								stmp += t.entityprofile[t.entid].model_s.Get();
								stmp += "###";
								stmp += std::to_string(t.entid);
								int itmp = t.entid;

								bool bValid = true;
								if (current_sort_order == SORT_LEVELITEMS && !bProjectItems)
								{
									//static int lastentityelementlist = -1;
									bValid = false;
									//PE: Exclude hidden and auto-gen objects.
									for (int i = 1; i <= g.entityelementlist; i++)
									{
										if (t.entityelement[i].bankindex == t.entid)
										{
											if (!(t.entityelement[i].y == -99999 || t.entityelement[i].y == -999999))
											{
												bValid = true;
												break;
											}
										}
										if (t.gridentity == t.entid ) //t.widget.pickedEntityIndex == t.entid)
										{
											bValid = true;
											break;
										}
									}
								}
								if(bValid)
									sorted_entity_files.push_back(std::make_pair(stmp, itmp));
							}
							std::sort(sorted_entity_files.begin(), sorted_entity_files.end());
							if (current_sort_order == SORT_NEWESTITEMS)
									std::reverse(sorted_entity_files.begin(), sorted_entity_files.end());
						}
					}
				}
				last_entidmaster = iMasterEntid;
				last_include_icon_set = iIncludeLeftIconSet;
				last_sortby = current_sort_order;
			}

			int uniqueId = 15000;
			int preview_count = 0;
			media_icon_size_leftpanel = 64;
			iColumnsWidth_leftpanel = 110;
			iColumns_leftpanel = 0;
			bDisplayText_leftpanel = true;
			fFontSize_leftpanel = SMALLFONTSIZE;
			ImGui::SetWindowFontScale(fFontSize_leftpanel);
			float fWinWidth = ImGui::GetWindowSize().x - 10.0; // Flicker - ImGui::GetCurrentWindow()->ScrollbarSizes.x;
			if (iColumnsWidth_leftpanel >= fWinWidth && fWinWidth > media_icon_size_leftpanel) 
			{
				iColumnsWidth_leftpanel = fWinWidth;
				fFontSize_leftpanel = SMALLESTFONTSIZE;
				ImGui::SetWindowFontScale(fFontSize_leftpanel);
			}
			if (fWinWidth <= media_icon_size_leftpanel + 10) 
			{
				iColumnsWidth_leftpanel = media_icon_size_leftpanel;
				fFontSize_leftpanel = SMALLESTFONTSIZE;
				ImGui::SetWindowFontScale(fFontSize_leftpanel);
			}
			if (fWinWidth <= 42) 
			{
				media_icon_size_leftpanel = 32;
				iColumnsWidth_leftpanel = media_icon_size_leftpanel + 16;
				bDisplayText_leftpanel = false;
			}
			iColumns_leftpanel = (int)(ImGui::GetWindowSize().x / (iColumnsWidth_leftpanel));
			if (iColumns_leftpanel <= 1)
				iColumns_leftpanel = 1;

			#ifdef ADD_DETAIL_LEFT_PANEL_ENTITY_LIST
			if (current_sort_order == SORT_BEHAVIORITEMS || current_sort_order == SORT_INSTANCEITEMS || current_sort_order == SORT_DETAILEDITEMS || current_sort_order == SORT_PROJECTSITEMS || current_sort_order == SORT_GROUPITEMS) //PE: Detailed display in one column.
			{
				iColumns_leftpanel = 1;
			}
			if (!bToggleThumbViews && (current_sort_order == SORT_LEVELITEMS || current_sort_order == SORT_OLDESTITEMS || current_sort_order == SORT_NEWESTITEMS))
			{
				iColumns_leftpanel = 1;
			}
			#endif

			if (!sorted_entity_files.empty())
			{
				ImGui::Columns(iColumns_leftpanel, "mycolumns4entities", false);  //false no border

				bool bHoveredUsed = false;
				for (int iloop = 0; iloop < 2; iloop++)
				{
					for (std::vector< std::pair<std::string, std::int32_t>>::iterator it = sorted_entity_files.begin(); it != sorted_entity_files.end(); ++it)
					{
						if (it->second == 999999)
						{
							if (iloop == 1)
							{
								//Seperator.
								if (iColumns_leftpanel == 1)
								{
									ImGui::Separator();
									preview_count++;
									ImGui::NextColumn();
								}
								else
								{
									if (iColumns_leftpanel == 1)
									{
										ImGui::Separator();
										preview_count++;
										ImGui::NextColumn();
									}
									else
									{
										for (int i = preview_count % iColumns_leftpanel; i < iColumns_leftpanel; i++)
										{
											preview_count++;
											ImGui::NextColumn();
										}
										ImGui::Separator();
									}
								}
							}
						}
						else if (it->second == 999998)
						{
							if (iloop == 1)
							{
								std::string sString = it->first;
								replaceAll(sString, "ZZZZ-", "");
								ImGui::Text(sString.c_str());
								preview_count++;
								ImGui::NextColumn();
							}
						}
						else if (it->second > 0)
						{
							if (iloop == 0 && current_sort_order == SORT_PROJECTSITEMS)
							{
								// Collection Items List (5)
								ImGui::SetWindowFontScale(1.0);

								// should we show this item
								int entid = it->second;
								char cName[512];
								strcpy(cName, t.entityprofileheader[entid].desc_s.Get());
								bool DisplayEntry = false;
								int iCollectionItemIndex = -1;
								for (int c = 0; c < g_collectionList.size(); c++)
								{
									if (g_collectionList[c].iEntityID == entid)
									{
										iCollectionItemIndex = c;
										break;
									}
								}
								if ( iCollectionItemIndex != -1 )
								{
									DisplayEntry = true;
									if (strlen(cSearchEntities) > 0)
									{
										if (!pestrcasestr(cName, cSearchEntities))
											DisplayEntry = false;
									}
									if (t.entityprofile[entid].groupreference != -1)
									{
										DisplayEntry = false;
									}
								}
								if (DisplayEntry == true)
								{
									ImGui::PushID(uniqueId++);
									float fFramePadding = (iColumnsWidth_leftpanel - media_icon_size_leftpanel) * 0.5;
									float fCenterX = ImGui::GetContentRegionAvail().x * 0.5;
									ImVec2 vIconSize = { (float)media_icon_size_leftpanel , (float)media_icon_size_leftpanel };
									float fRatio = 288.0f / 512.0f;
									float fImageWidth = ImGui::GetContentRegionAvail().x - 4.0f;
									vIconSize = { fImageWidth ,fImageWidth * fRatio };
									char* cFind = strstr(cName, "###");
									if (cFind) cFind[0] = '\0';
									ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow; //Got sub selections.
									bool bSelected = false;
									ImGui::PushItemWidth(-20.0);
									std::string treename = cName;
									bool TreeNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)(entid + 99000), node_flags, treename.c_str());
									bool bHovered = ImGui::IsItemHovered();
									ImGui::PopItemWidth();
									if (TreeNodeOpen)
									{
										ImGui::Indent(-5);
										bool bMoveCameraToObjectPosition = true;// false;
										DoTreeNodeEntity(entid, bMoveCameraToObjectPosition);
										ImGui::Indent(5);
										ImGui::TreePop();
									}
									ImGui::PopID();
									preview_count++;
									ImGui::NextColumn();
								}
							}
							else if (iloop == 0 && current_sort_order == SORT_GROUPITEMS)
							{
								// Group List (6)
								ImGui::SetWindowFontScale(1.0);

								// should we show this item
								int groupindex = it->second;
								char cName[512];
								strcpy(cName, sEntityGroupListName[groupindex].Get());
								if(strlen(cName)>4) cName[strlen(cName) - 4] = 0;
								if (strlen(cName) == 0) sprintf(cName, "Group %d", 1+groupindex);
								bool DisplayEntry = false;
								if (vEntityGroupList[groupindex].size() > 0)
								{
									DisplayEntry = true;
								}
								if (DisplayEntry == true)
								{
									ImGui::PushID(uniqueId++);
									float fFramePadding = (iColumnsWidth_leftpanel - media_icon_size_leftpanel) * 0.5;
									float fCenterX = ImGui::GetContentRegionAvail().x * 0.5;
									ImVec2 vIconSize = { (float)media_icon_size_leftpanel , (float)media_icon_size_leftpanel };
									float fRatio = 288.0f / 512.0f;
									float fImageWidth = ImGui::GetContentRegionAvail().x - 4.0f;
									vIconSize = { fImageWidth ,fImageWidth * fRatio };
									char* cFind = strstr(cName, "###");
									if (cFind) cFind[0] = '\0';
									ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow; //Got sub selections.
									bool bSelected = false;
									ImGui::PushItemWidth(-20.0);
									std::string treename = cName;
									bool TreeNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)(groupindex + 99000), node_flags, treename.c_str());
									bool bHovered = ImGui::IsItemHovered();
									ImGui::PopItemWidth();
									if (TreeNodeOpen)
									{
										ImGui::Indent(-5);
										bool bMoveCameraToObjectPosition = true;
										DoTreeNodeGroup(groupindex, bMoveCameraToObjectPosition);
										ImGui::Indent(5);
										ImGui::TreePop();
									}
									ImGui::PopID();
									preview_count++;
									ImGui::NextColumn();
								}
							}
							else if (iloop == 0 && current_sort_order == SORT_INSTANCEITEMS)
							{
								// Instance List (7)
								ImGui::SetWindowFontScale(1.0);
								int e = it->second;
								bool DisplayEntry = true;
								if (DisplayEntry == true)
								{
									ImGui::PushID(uniqueId++);
									float fFramePadding = (iColumnsWidth_leftpanel - media_icon_size_leftpanel) * 0.5;
									float fCenterX = ImGui::GetContentRegionAvail().x * 0.5;
									ImVec2 vIconSize = { (float)media_icon_size_leftpanel , (float)media_icon_size_leftpanel };
									float fRatio = 288.0f / 512.0f;
									float fImageWidth = ImGui::GetContentRegionAvail().x - 4.0f;
									vIconSize = { fImageWidth ,fImageWidth * fRatio };
									char cName[512];
									strcpy(cName, t.entityelement[e].eleprof.name_s.Get());
									ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf;
									std::string treename = "#" + std::to_string(e);
									treename = treename + " " + cName;
									bool bAutoGenObject = false;
									if (t.entityelement[e].x == -99999 && t.entityelement[e].y == -99999 && t.entityelement[e].z == -99999)
									{
										treename = treename + " (Auto-Gen) ";
										bAutoGenObject = true;
									}
									if (t.entityelement[e].y == -999999)
									{
										treename = treename + " (Hidden) ";
										bAutoGenObject = true;
									}
									bool TreeNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)(e + 99000), node_flags, treename.c_str());
									if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0))
									{
										// DUPLICATED ESEWHERE (LOTS) SO EVENTUALLY MAKE ONE FUNCTION FOR DIS.
										if (t.entityelement[e].obj > 0)
										{
											t.widget.pickedEntityIndex = e;
											t.widget.pickedObject = t.entityelement[t.widget.pickedEntityIndex].obj;
											g.entityrubberbandlist.clear();
											bEditorInFreeFlightMode = true;
											t.editorfreeflight.mode = 1;
											int group = isEntityInGroupList(t.widget.pickedEntityIndex);
											if (group >= 0)
											{
												//PE: Add all groups with entity to rubberband.
												CheckGroupListForRubberbandSelections(t.widget.pickedEntityIndex);
											}
											if (bAutoGenObject == false)
											{
												float zoom = ObjectSize(t.entityelement[e].obj, 1) * 2.0;
												if (zoom < 30.0f) zoom = 30.0f;
												float realcamy = ObjectSizeY(t.entityelement[e].obj, 1) * 0.75;
												float camy = realcamy;
												if (camy < 30.0f) camy = 30.0f;
												if (t.entityprofile[t.entityelement[e].bankindex].ismarker > 0)
												{
													zoom = 100.0;
													camy = 50.0;
												}
												PositionCamera(t.entityelement[e].x, t.entityelement[e].y, t.entityelement[e].z);
												PointCamera(t.entityelement[e].x, t.entityelement[e].y, t.entityelement[e].z);
												MoveCamera(0, -zoom);
												PositionCamera(CameraPositionX(0), t.entityelement[e].y + camy, CameraPositionZ(0));
												PointCamera(t.entityelement[e].x, t.entityelement[e].y + (realcamy * 0.5), t.entityelement[e].z);
												t.editorfreeflight.c.x_f = CameraPositionX();
												t.editorfreeflight.c.y_f = CameraPositionY();
												t.editorfreeflight.c.z_f = CameraPositionZ();
												t.editorfreeflight.c.angx_f = CameraAngleX();
												t.editorfreeflight.c.angy_f = CameraAngleY();
												t.cx_f = t.editorfreeflight.c.x_f;
												t.cy_f = t.editorfreeflight.c.z_f;
											}
										}
									}
									if (TreeNodeOpen) ImGui::TreePop();
									ImGui::PopID();
									preview_count++;
									ImGui::NextColumn();
								}
							}
							else if (iloop == 0 && current_sort_order == SORT_BEHAVIORITEMS)
							{
								// Behavior List (8)
								ImGui::SetWindowFontScale(1.0);
								int e = it->second;
								bool DisplayEntry = true;
								if (DisplayEntry == true)
								{
									ImGui::PushID(uniqueId++);
									float fFramePadding = (iColumnsWidth_leftpanel - media_icon_size_leftpanel) * 0.5;
									float fCenterX = ImGui::GetContentRegionAvail().x * 0.5;
									ImVec2 vIconSize = { (float)media_icon_size_leftpanel , (float)media_icon_size_leftpanel };
									float fRatio = 288.0f / 512.0f;
									float fImageWidth = ImGui::GetContentRegionAvail().x - 4.0f;
									vIconSize = { fImageWidth ,fImageWidth * fRatio };
									char cName[512];
									strcpy(cName, it->first.c_str());
									ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
									bool bSelected = false;
									ImGui::PushItemWidth(-20.0);
									std::string treename = cName;
									bool TreeNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)(e + 99000), node_flags, treename.c_str());
									ImGui::PopItemWidth();
									if (TreeNodeOpen)
									{
										ImGui::Indent(-5);
										DoTreeNodeBehavior(cName, true);
										ImGui::Indent(5);
										ImGui::TreePop();
									}
									ImGui::PopID();
									preview_count++;
									ImGui::NextColumn();
								}
							}
							else if (iloop == 0 && (current_sort_order == SORT_DETAILEDITEMS || !bToggleThumbViews ))
							{
								// Detailed Object List (4)
								ImGui::SetWindowFontScale(1.0);

								bool DisplayEntry = true;
								char cName[512];
								strcpy(cName, t.entityprofileheader[it->second].desc_s.Get());

								if (strlen(cSearchEntities) > 0)
								{
									//PE: This will search the desc. and the object name.
									if (!pestrcasestr(cName, cSearchEntities))
										DisplayEntry = false;
								}
								if (current_sort_order != SORT_DETAILEDITEMS)
								{
									if (!bToggleThumbViews && (t.entityprofile[it->second].ismarker != 0 || t.entityprofile[it->second].ischildofgroup != 0))
									{
										DisplayEntry = false;
									}
								}

								bool bUseWideThumb = false;
								int iTextureID = t.entityprofile[it->second].iThumbnailSmall;
								if (t.entityprofile[it->second].iThumbnailLarge > 0)
								{
									bUseWideThumb = true;
									iTextureID = t.entityprofile[it->second].iThumbnailLarge;
								}

								if (DisplayEntry && iTextureID > 0)
								{
									ImGui::PushID(uniqueId++);
									float fFramePadding = (iColumnsWidth_leftpanel - media_icon_size_leftpanel)*0.5;
									float fCenterX = ImGui::GetContentRegionAvail().x * 0.5;
									ImVec2 vIconSize = { (float)media_icon_size_leftpanel , (float)media_icon_size_leftpanel };

									if (bUseWideThumb)
									{
										//512x288
										float fRatio = 288.0f / 512.0f;
										float fImageWidth = ImGui::GetContentRegionAvail().x - 4.0f;
										vIconSize = { fImageWidth ,fImageWidth*fRatio };
									}

									char *cFind = strstr(cName, "###");
									if (cFind)
										cFind[0] = '\0';

									if (t.entityprofile[it->second].groupreference != -1)
									{
										strcat(cName, " (Smart Object)");
									}
									ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow; //Got sub selections.

									bool bSelected = false;

									ImGui::PushItemWidth(-20.0); //PE: Room for a icon.
									std::string treename = cName;
									bool TreeNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)(it->second + 99000), node_flags, treename.c_str());
									bool bHovered = ImGui::IsItemHovered();
									if (bHovered == true)
									{
										if (ImGui::IsMouseDoubleClicked(0))
										{
											LPSTR pParentName = t.entityprofileheader[it->second].desc_s.Get();
											int iAnchorEntityIndex = -1;
											g.entityrubberbandlist.clear();
											for (int e = 1; e <= g.entityelementlist; e++)
											{
												int entid = t.entityelement[e].bankindex;
												if (entid > 0)
												{
													LPSTR pThisName = t.entityprofileheader[entid].desc_s.Get();
													if (stricmp(pParentName, pThisName) == NULL)
													{
														iAnchorEntityIndex = e;
														sRubberBandType rubberbandItem;
														rubberbandItem.e = e;
														rubberbandItem.x = t.entityelement[e].x;
														rubberbandItem.y = t.entityelement[e].y;
														rubberbandItem.z = t.entityelement[e].z;
														rubberbandItem.px = t.entityelement[e].x;
														rubberbandItem.py = t.entityelement[e].y;
														rubberbandItem.pz = t.entityelement[e].z;
														rubberbandItem.rx = t.entityelement[e].rx;
														rubberbandItem.ry = t.entityelement[e].ry;
														rubberbandItem.rz = t.entityelement[e].rz;
														rubberbandItem.quatmode = t.entityelement[e].quatmode;
														rubberbandItem.quatx = t.entityelement[e].quatx;
														rubberbandItem.quaty = t.entityelement[e].quaty;
														rubberbandItem.quatz = t.entityelement[e].quatz;
														rubberbandItem.quatw = t.entityelement[e].quatw;
														rubberbandItem.scalex = t.entityelement[e].scalex;
														rubberbandItem.scaley = t.entityelement[e].scaley;
														rubberbandItem.scalez = t.entityelement[e].scalez;
														g.entityrubberbandlist.push_back (rubberbandItem);
													}
												}
											}
											if (iAnchorEntityIndex != -1)
											{
												t.widget.pickedEntityIndex = iAnchorEntityIndex;
												t.widget.pickedObject = t.entityelement[iAnchorEntityIndex].obj;
												t.gridentity = 0;
											}
										}
									}
									ImGui::PopItemWidth();

									cstr find = t.entitybank_s[it->second];
									BeginDragDropFPE(find.Get(), iTextureID, bToolTipActive, vIconSize);
									if (!bHoveredUsed && bHovered && bToolTipActive && !bDraggingActive)
									{
										bHoveredUsed = true;
										ImGui::BeginTooltip();
										ImGui::ImgBtn(iTextureID, vIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down, -1, 0, 0, 0, true, false, false, false, true, false);
										ImGui::EndTooltip();
									}

									bool bTreeNodeSelected = false;

									if (TreeNodeOpen) 
									{
										//Display any sub nodes
										ImGui::Indent(-5);
										DoTreeNodeEntity(it->second,true);
										ImGui::Indent(5);
										ImGui::TreePop();
									}

									if (bTreeNodeSelected)
									{
										//Only if we are not dragging in a trashcan.
										if (bToolTipActive)
										{
											if (bWaypointDrawmode || bWaypoint_Window) { bWaypointDrawmode = false; bWaypoint_Window = false; }
											if (g_bCharacterCreatorPlusActivated) g_bCharacterCreatorPlusActivated = false;
											if (bImporter_Window) { importer_quit(); bImporter_Window = false; }

											FreeTempImageList();
											DeleteWaypointsAddedToCurrentCursor();
											CloseDownEditorProperties();
											//Make sure we are in entity mode.
											bForceKey = true;
											csForceKey = "o";
											iExtractMode = 0; //PE: Always start in find floor mode.
											t.gridentity = it->second;
											t.inputsys.constructselection = it->second;
											t.inputsys.domodeentity = 1;
											t.grideditselect = 5;
											//Make sure we use a fresh t.grideleprof
											t.entid = t.gridentity;
											entity_fillgrideleproffromprofile();
											t.grideleprof.bUseFPESettings = true; //PE: New added always use bUseFPESettings.
											t.grideleprof.isProjectGlobal = false;

											t.inputsys.dragoffsetx_f = 0;
											t.inputsys.dragoffsety_f = 0;
											fHitPointX = 0;
											fHitPointY = HITPOINTYSTARTPOS;
											fHitPointZ = 0;
											fHitOffsetX = 0;
											fHitOffsetY = 0;
											fHitOffsetZ = 0;
											g_fSpecialDragInYAdjustment = 0.0f;
											bDraggingActive = false;
											g_bHoldGridEntityPosWhenManaged = true;
											g_fHoldGridEntityPosX = t.gridentityposx_f;
											g_fHoldGridEntityPosY = t.gridentityposy_f;
											g_fHoldGridEntityPosZ = t.gridentityposz_f;
											editor_refresheditmarkers();
											g_bSelectedNewObjectToAddToLevel = true;
										}
									}

									ImGui::PopID();
									preview_count++;
									ImGui::NextColumn();
								}
							}
							else
							{
								// no longer list markers in left entity panel, we have the game elements buttons now
								if ((iloop == 0 && t.entityprofile[it->second].ismarker == 0 && t.entityprofile[it->second].ischildofgroup == 0))
								{
									bool DisplayEntry = true;
									char cName[512];
									strcpy(cName, t.entityprofileheader[it->second].desc_s.Get());

									if (strlen(cSearchEntities) > 0)
									{
										//PE: This will search the desc. and the object name.
										if (!pestrcasestr(cName, cSearchEntities))
											DisplayEntry = false;
									}

									bool bUseWideThumb = false;
									int iTextureID = t.entityprofile[it->second].iThumbnailSmall;
									if (t.entityprofile[it->second].iThumbnailLarge > 0)
									{
										bUseWideThumb = true;
										iTextureID = t.entityprofile[it->second].iThumbnailLarge;
									}
									if (DisplayEntry && iTextureID > 0)
									{
										// get ready to overlay a smart object icon
										ImVec2 vSmartObjectIconPos = ImGui::GetCursorPos();

										ImGui::PushID(uniqueId++);
										float fFramePadding = (iColumnsWidth_leftpanel - media_icon_size_leftpanel)*0.5;
										float fCenterX = ImGui::GetContentRegionAvail().x * 0.5;
										ImVec2 vIconSize = { (float)media_icon_size_leftpanel , (float)media_icon_size_leftpanel };

										if (!bUseWideThumb)
										{
											ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + (fCenterX - (media_icon_size_leftpanel*0.5)), ImGui::GetCursorPosY()));
										}
										else
										{
											//512x288
											float fRatio = 288.0f / 512.0f;
											float fImageWidth = ImGui::GetContentRegionAvail().x - 4.0f;
											vIconSize = { fImageWidth ,fImageWidth*fRatio };
										}

										// Entity Left Panel.
										if (ImGui::ImgBtn(iTextureID, vIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down, -1, 0, 0, 0, false, false, false, false, true, false))
										{
											//Only if we are not dragging in a trashcan.
											if (bToolTipActive)
											{
												if (bWaypointDrawmode || bWaypoint_Window) { bWaypointDrawmode = false; bWaypoint_Window = false; }
												if (g_bCharacterCreatorPlusActivated) g_bCharacterCreatorPlusActivated = false;
												if (bImporter_Window) { importer_quit(); bImporter_Window = false; }

												FreeTempImageList();
												DeleteWaypointsAddedToCurrentCursor();
												CloseDownEditorProperties();
												//Make sure we are in entity mode.
												bForceKey = true;
												iExtractMode = 0; //PE: Always start in find floor mode.
												csForceKey = "o";
												t.gridentity = it->second;
												t.inputsys.constructselection = it->second;
												t.inputsys.domodeentity = 1;
												t.grideditselect = 5;
												//Make sure we use a fresh t.grideleprof
												t.entid = t.gridentity;
												entity_fillgrideleproffromprofile();
												t.grideleprof.bUseFPESettings = true; //PE: New added always use bUseFPESettings.
												t.grideleprof.isProjectGlobal = false;

												t.inputsys.dragoffsetx_f = 0;
												t.inputsys.dragoffsety_f = 0;
												fHitPointX = 0;
												fHitPointY = HITPOINTYSTARTPOS;
												fHitPointZ = 0;
												fHitOffsetX = 0;
												fHitOffsetY = 0;
												fHitOffsetZ = 0;
												g_fSpecialDragInYAdjustment = 0.0f;
												bDraggingActive = false;
												g_bHoldGridEntityPosWhenManaged = true;
												g_fHoldGridEntityPosX = t.gridentityposx_f;
												g_fHoldGridEntityPosY = t.gridentityposy_f;
												g_fHoldGridEntityPosZ = t.gridentityposz_f;
												editor_refresheditmarkers();
												g_bSelectedNewObjectToAddToLevel = true;
											}
										}

										cstr find = t.entitybank_s[it->second];
										BeginDragDropFPE(find.Get(), iTextureID, bToolTipActive, vIconSize);

										char *cFind = strstr(cName, "###");
										if (cFind)
											cFind[0] = '\0';
										if (ImGui::IsItemHovered() && bToolTipActive) ImGui::SetTooltip("%s", cName);

										if (bDisplayText_leftpanel)
										{
											ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, -5.0f));
											ImGui::TextCenter("%s", cName); //no wrap.
										}

										// show when object is a smart object
										if (t.entityprofile[it->second].groupreference != -1)
										{
											int iImageSize = 20;
											ImVec2 opos = ImGui::GetCursorPos();
											ImGui::SetCursorPos(ImVec2(vSmartObjectIconPos.x + vIconSize.x - 17.0f, vSmartObjectIconPos.y - 19.0f + vIconSize.y));
											ImGui::SetItemAllowOverlap();
											if (ImGui::ImgBtn(TOOL_SMARTOBJECT, ImVec2(iImageSize, iImageSize), ImColor(0, 0, 0, 0), ImColor(255, 255, 255, 180), ImColor(255, 255, 255, 180), ImColor(255, 255, 255, 180), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
											{
												// clicking does nothing
											}
											ImGui::SetCursorPos(opos);
											if (ImGui::IsItemHovered()) ImGui::SetTooltip("This is a Smart Object");
										}

										ImGui::PopID();
										preview_count++;
										ImGui::NextColumn();
									}
								}
							}
						}
					}
				}
				ImGui::Columns(1);
			}

			ImGui::SetWindowFontScale(1.00);
			if (ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered())
				bImGuiGotFocus = true;

			//PE: Doubble click to move closer to terrain. disabled for objects as we might end up into walls ...
			extern float fLastTerrainHitX, fLastTerrainHitY, fLastTerrainHitZ;
			if (!bImGuiGotFocus && bImGuiRenderTargetFocus && g_hovered_pobject == 0 && t.thighlighterobj == 0)
			{
				if (fLastTerrainHitY > g.gdefaultwaterheight && t.grideditselect == 5)
				{
					static float traveltox = 0, traveltoy = 0, traveltoz = 0;
					if(ImGui::IsMouseDoubleClicked(0))
					{
						//PE: Get a fresh.
						wiInput::MouseState mouseState = wiInput::GetMouseState();
						RAY pickRay = wiRenderer::GetPickRay((long)mouseState.position.x, (long)mouseState.position.y, master.masterrenderer);
						if (!GGTerrain::GGTerrain_RayCast(pickRay, &fLastTerrainHitX, &fLastTerrainHitY, &fLastTerrainHitZ, 0, 0, 0, 0))
						{
							fLastTerrainHitX = 0, fLastTerrainHitY = 0, fLastTerrainHitZ = 0;
						}
						else
						{
							if (fLastTerrainHitY > g.gdefaultwaterheight)
							{
								float composx, composy, composz, comangx, comangy, comangz;

								composx = CameraPositionX(0);
								composy = CameraPositionY(0);
								composz = CameraPositionZ(0);
								comangx = CameraAngleX(0);
								comangy = CameraAngleY(0);
								comangz = CameraAngleZ(0);

								float step = 10; //PE: This will move us 10% from the total distance to the target.
								float dx = (composx-fLastTerrainHitX) / step;
								float dy = (composy-fLastTerrainHitY) / step;
								float dz = (composz-fLastTerrainHitZ) / step;

								traveltox = fLastTerrainHitX + dx;
								traveltoy = fLastTerrainHitY + dy;
								traveltoz = fLastTerrainHitZ + dz;

								t.editorfreeflight.mode = 3;
								t.editorfreeflight.s.x_f = traveltox;
								t.editorfreeflight.s.y_f = traveltoy;
								t.editorfreeflight.s.z_f = traveltoz;
								PositionCamera(traveltox, traveltoy, traveltoz);
								PointCamera(fLastTerrainHitX, fLastTerrainHitY, fLastTerrainHitZ);
								t.editorfreeflight.s.angx_f = CameraAngleX(0);
								t.editorfreeflight.s.angy_f = CameraAngleY(0);
								t.editorfreeflight.c = t.editorfreeflight.s;
								PositionCamera(composx, composy, composz);
								RotateCamera(comangx, comangy, comangz);
							}
						}
					}
				}
			}

			if (bProfilerEnable)
			{
				ImGui::Separator();
				wiScene::Scene* pScene = &wiScene::GetScene();
				int iMeshes = pScene->meshes.GetCount();
				int iMaterials = pScene->materials.GetCount();

				int dc = wiProfiler::GetDrawCalls();
				int dcs = wiProfiler::GetDrawCallsShadows();
				int dct = wiProfiler::GetDrawCallsTransparent();

				int tris = wiProfiler::GetPolygons();
				int trisShadow = wiProfiler::GetPolygonsShadows();
				int trisTransparent = wiProfiler::GetPolygonsTransparent();

				ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
				ImGui::Text("DrawCalls: %d", dc);
				ImGui::Text("DrawCallsShadows: %d", dcs);
				ImGui::Text("DrawCallsTransparent: %d", dct);
				ImGui::Text("Triangles: %d", tris);
				ImGui::Text("TrianglesShadows: %d", trisShadow);
				ImGui::Text("TrianglesTransparent: %d", trisTransparent);
				ImGui::Text("Scene Meshes: %d", iMeshes);
				ImGui::Text("Scene Materials: %d", iMaterials);
				ImGui::Text("Scene Transforms: %d", (int)pScene->transforms.GetCount());
				ImGui::Text("Scene Hierarchy: %d", (int)pScene->hierarchy.GetCount());

				ImGui::Separator();
				std::string profiler_data = wiProfiler::GetProfilerData();
				ImGui::Text(profiler_data.c_str());

			}

			if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) 
			{
				//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
				ImGui::Text("");
				ImGui::Text("");
				ImGui::Text("");
			}

			ImGui::EndChild();

			// number of game element buttson shown
			entity_icons = 12;
			if (pref.iObjectEnableAdvanced)
				entity_icons = 16;

			int entity_images[] = { ENTITY_START, ENTITY_CHECKPOINT, ENTITY_FLAG, ENTITY_TRIGGERZONE, ENTITY_WIN, ENTITY_LIGHT,ENTITY_VIDEO,ENTITY_MUSIC,ENTITY_SOUND,ENTITY_PARTICLE,ENTITY_IMAGE, ENTITY_TEXT, ENTITY_PROBE, ENTITY_COVER, ENTITY_BEHAVIOR, ENTITY_NEW_PARTICLE };
			static cstr entity_scripts[] = {
				"_markers\\Player Start.fpe",
				"_markers\\Player Checkpoint.fpe",
				"_markers\\flag.fpe" ,
				"_markers\\Trigger Zone.fpe",
				"_markers\\Win Zone.fpe",
				"_markers\\White Light.fpe",
				"_markers\\Video Zone.fpe",
				"_markers\\Ambience Zone.fpe",
				"_markers\\Audio Zone.fpe",
				"_markers\\Particles.fpe",
				"_markers\\Image Zone.fpe",
				"_markers\\Text Zone.fpe",
				"_markers\\Probe.fpe",
				"_markers\\Cover Zone.fpe",
				"_markers\\Behavior.fpe", //global Behaviors
				"_markers\\NewParticles.fpe"
			};
			static cstr entity_tooltip[] = {
				"Add Player Start Position",
				"Add Player Checkpoint",
				"Add Flag",
				"Add Trigger Zone",
				"Add Win Zone",
				"Add Light",
				"Add Video Zone",
				"Add Music Zone",
				"Add Audio Zone",
				"Add New Particle",
				"Add Image Zone",
				"Add Text Zone",
				"Add Environment Probe",
				"Add Cover Zone",
				"Add Global Behavior",
				"Add WPE Zone"
			};

			int offset = 0;
			if (bViewOptionsOpen)
				offset = 225;// 205;// 115;
			if (entity_icons_columns > 10 && entity_icons_columns < 15)
				offset += entity_image_size;

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, ImGui::GetContentRegionAvail().y - offset
				- ((entity_w / entity_icons_columns) * iIconRows) - ImGui::GetFontSize() * 4.0f + 10.0f));
			ImGui::TextCenter("Game Elements");

			ImVec4 IconColor = ImVec4(1.0, 1.0, 1.0, 1.0);
			ImGui::Indent(4);
			for (int i = 0; i < entity_icons; i++)
			{
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(fSpacer, 0.0f));

				if (ImGui::ImgBtn(entity_images[i], ImVec2(entity_image_size, entity_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), IconColor, ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false, false, false))
				{
					FreeTempImageList(); //PE: Whenever g.entidmaster can change we must make sure to free any "temp" objects loaded.

					t.addentityfile_s = entity_scripts[i];
					if (t.addentityfile_s != "")
					{
						entity_adduniqueentity(false);
						t.tasset = t.entid;
						if (t.talreadyloaded == 0)
						{
							editor_filllibrary();
						}
					}

					iExtractMode = 0; //PE: Always start in find floor mode.
					t.inputsys.constructselection = t.tasset;
					t.gridentity = t.entid;
					t.inputsys.constructselection = t.entid;
					t.inputsys.domodeentity = 1;
					t.grideditselect = 5;
					Entity_Tools_Window = true;
					//Make sure we use a fresh t.grideleprof
					entity_fillgrideleproffromprofile();
					t.grideleprof.bUseFPESettings = true; //PE: New added always use bUseFPESettings.
					t.grideleprof.isProjectGlobal = false;

					editor_refresheditmarkers();

					// Show elements when placing a new one down, prevents half being hidden and half not.
					t.showeditorelements = 1;
					editor_toggle_element_vis(t.showeditorelements);

				}
				if (ImGui::IsItemHovered() && bToolTipActive) ImGui::SetTooltip(entity_tooltip[i].Get());
				BeginDragDropFPE(entity_scripts[i].Get(), entity_images[i], bToolTipActive, ImVec2(entity_image_size, entity_image_size));

				ImVec2 restore_cursorpos = ImGui::GetCursorPos();
				if ((i + 1) % entity_icons_columns != 0 && i != entity_icons - 1)
					ImGui::SameLine();
			}

			content_avail = ImGui::GetContentRegionAvail();
			content_avail.y -= 6.0;

			bViewOptionsOpen = false;

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 3.0f));
			ImGui::Indent(-4);

			if (ImGui::StyleCollapsingHeader("View Options##viewoptions"))
			{
				bViewOptionsOpen = true;
				ImGui::Columns(3);

				ImGuiWindow* win = ImGui::GetCurrentWindow();
				win->DC.CurrentColumns->Flags |= ImGuiColumnsFlags_NoResize;

				ImGui::SetColumnWidth(0, content_avail.x * 0.625f);
				ImGui::TextCenter("");

				float fFontSize = ImGui::GetFontSize();

				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, fFontSize*0.25f));
				ImGui::Text("Game Elements");
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, fFontSize * 0.5f));
				ImGui::Text("Editable Area 3D Edge");
				if (t.visuals.bEnableEmptyLevelMode == false)
				{
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, fFontSize * 0.5f));
					ImGui::Text("Editable Area 2D Edge");
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, fFontSize * 0.5f));
					ImGui::Text("Trees");
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, fFontSize * 0.5f));
					ImGui::Text("Vegetation");
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, fFontSize * 0.5f));
					ImGui::Text("Water");
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, fFontSize * 0.5f));
					ImGui::Text("Terrain");
				}
				ImGui::NextColumn();
				ImGui::SetColumnWidth(1, content_avail.x * 0.2f);
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(-1.0f, 0.0f));
				ImGui::Text("Editor");
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Changes to the Editor View settings are temporary and will only effect visuals whilst editing your levels");
				
				// Editor game elements.
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(4.0f, 0.0f));
				bool bShow = t.showeditorelements;
				if (ImGui::Checkbox("##EditorElements", &bShow))
				{
					t.showeditorelements = bShow;
					editor_toggle_element_vis(bShow);
				}
				// Editor 3D boundary.
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(4.0f, 0.0f));
				bShow = (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D) != 0;
				if (ImGui::Checkbox("##Editor3DBounds", &bShow))
				{
					if (bShow) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
					else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE_3D;
				}
				// Regular mode
				if (t.visuals.bEnableEmptyLevelMode == false)
				{
					// Editor 2D boundary.
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(4.0f, 0.0f));
					bShow = (ggterrain_global_render_params2.flags2 & GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE) ? 1 : 0;
					if (ImGui::Checkbox("##Editor2DBounds", &bShow))
					{
						if (bShow) ggterrain_global_render_params2.flags2 |= GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
						else ggterrain_global_render_params2.flags2 &= ~GGTERRAIN_SHADER_FLAG2_SHOW_MAP_SIZE;
					}
					// Editor Trees.
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(4.0f, 0.0f));
					if (t.showeditortrees < 0)
						t.showeditortrees = t.visuals.bEndableTreeDrawing;
					bShow = t.showeditortrees;
					if (ImGui::Checkbox("##EditorTrees", &bShow))
					{
						ggtrees_global_params.draw_enabled = bShow;
						t.showeditortrees = bShow;
					}

					// Editor Vegetation.
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(4.0f, 0.0f));
					if (t.showeditorveg < 0)
						t.showeditorveg = t.visuals.bEndableGrassDrawing;
					bShow = t.showeditorveg;
					if (ImGui::Checkbox("##EditorVeg", &bShow))
					{
						gggrass_global_params.draw_enabled = bShow;
						t.showeditorveg = bShow;
					}

					// Editor Water.
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(4.0f, 0.0f));
					if (t.showeditorwater < 0)
						t.showeditorwater = t.visuals.bWaterEnable;
					bShow = t.showeditorwater;
					if (ImGui::Checkbox("##EditorWater", &bShow))
					{
						t.showeditorwater = bShow;
						Wicked_Update_Visuals((void*)&t.visuals);
					}

					// Editor Terrain.
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(4.0f, 0.0f));
					if (t.showeditorterrain < 0)
						t.showeditorterrain = t.visuals.bEndableTerrainDrawing;
					bShow = t.showeditorterrain;
					if (ImGui::Checkbox("##EditorTerrain", &bShow))
					{
						t.showeditorterrain = bShow;
						Wicked_Update_Visuals((void*)&t.visuals);
					}
				}
				ImGui::NextColumn();
								
				ImGui::SetColumnWidth(2, content_avail.x * 0.18f);
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(1.0f, 0.0f));
				ImGui::Text("Level");
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Changes to the Level View settings will effect the visuals seen in-game");
				
				// Test level game elements.
				bShow = t.showtestgameelements;
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(5.0f, 0.0f));
				if (ImGui::Checkbox("##LevelElements", &bShow))	t.showtestgameelements = bShow;
				// Test level 3D boundary.
				bShow = t.showtestgame3dbounds;
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(5.0f, 0.0f));
				if (ImGui::Checkbox("##Level3DBounds", &bShow))	t.showtestgame3dbounds = bShow;
				// Regular mode
				if (t.visuals.bEnableEmptyLevelMode == false)
				{
					// Test level 2D boundary.
					bShow = t.showtestgame2dbounds;
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(5.0f, 0.0f));
					if (ImGui::Checkbox("##Level2DBounds", &bShow))	t.showtestgame2dbounds = bShow;
					// Test level Trees.
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(5.0f, 0.0f));
					if (ImGui::Checkbox("##LevelTrees", &t.visuals.bEndableTreeDrawing))
						t.gamevisuals.bEndableTreeDrawing = t.visuals.bEndableTreeDrawing;

					// Test level Vegetation.
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(5.0f, 0.0f));
					if (ImGui::Checkbox("##LevelVeg", &t.visuals.bEndableGrassDrawing))
						t.gamevisuals.bEndableGrassDrawing = t.visuals.bEndableGrassDrawing;

					// Test level Water.
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(5.0f, 0.0f));
					if (ImGui::Checkbox("##LevelWater", &t.visuals.bWaterEnable))
						t.gamevisuals.bWaterEnable = t.visuals.bWaterEnable;

					// Test level Terrain.
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(5.0f, 0.0f));
					if (ImGui::Checkbox("##LevelTerrain", &t.visuals.bEndableTerrainDrawing))
						t.gamevisuals.bEndableTerrainDrawing = t.visuals.bEndableTerrainDrawing;
				}
				ImGui::Columns(1);
			}

			int iGridObj = g.ebeobjectbankoffset + 1000;
			if (!bImGuiInTestGame && bEmptyLevelGrid)
			{
				if (!ObjectExist(iGridObj))
				{
					//PE: TODO need to exclude this mesh from select outline.
					WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_CURSOROBJECT);
					MakeObjectPlane(iGridObj, 6000, 6000);
					WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_NORMAL);
					XRotateObject(iGridObj, 90);
					DisableObjectZDepth(iGridObj);
					sObject* pObject = GetObjectData(iGridObj);
					if (pObject)
					{
						WickedCall_SetObjectCastShadows(pObject, false);
						WickedCall_SetObjectLightToUnlit(pObject, (int)wiScene::MaterialComponent::SHADERTYPE_UNLIT);
						WickedCall_SetObjectDisableDepth(pObject, true);
					}

					float ShaderParam1 = 2.0; //THICKNESS_FACTOR
					float ShaderParam2 = 3000; //FADE_DISTANCE
					float ShaderParam3 = 0.4f; //POWER_EXPONENT
					float ShaderParam4 = 0.4f; //BASE ALPHA
					void importer_set_all_material_shader_id(int obj, int shaderID, float p1, float p2, float p3, float p4, float p5, float p6, float p7);
					importer_set_all_material_shader_id(iGridObj, 4, ShaderParam1, ShaderParam2, ShaderParam3, ShaderParam4, 0, 0, 0);
				}
				float camx = CameraPositionX();
				float camy = CameraPositionY();
				float camz = CameraPositionZ();
				PositionObject(iGridObj, camx, fEmptyLevelFloorY, camz);

			}
			else if (!bEmptyLevelGrid)
			{
				if (ObjectExist(iGridObj))
				{
					DeleteObject(iGridObj);
				}
			}

			//Drag/Drop to remove objects.
			ImRect bb = { ImGui::GetWindowContentRegionMin()+ImGui::GetWindowPos(),ImGui::GetWindowContentRegionMax() + ImGui::GetWindowPos() };

			if (bTrashcanIconActive)
				bTrashcanIconActive = false;
			DragDrop_CheckTrashcanDrop(bb);

			if (ImGui::BeginDragDropTargetCustom(bb, 12345))
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_MODEL_DROP_TARGET", 0)) // ImGuiDragDropFlags_AcceptNoDrawDefaultRect
				{
					AddPayLoad((ImGuiPayload*)payload, false);
				}
			}

			// note to show we are in completely empty level mode
			if (t.visuals.bEnableEmptyLevelMode == true)
			{
				// this mode is activated when the terrain is first generated
				ImGui::TextCenter("");
				ImGui::TextCenter("Completely Empty Level Mode");
				ImGui::TextCenter("");
				ImGui::TextCenter("Terrain, water and other default");
				ImGui::TextCenter("elements removed from this level");
			}

			ImGui::End();

			//#########################
			//#### Current objects ####
			//#########################
			
			ImGui::Begin("Current Objects##AdditionalIconsWindow", &bAlwaysOpen, iGenralWindowsFlags | ImGuiWindowFlags_NoTitleBar);

			static ImVec2 vBelowContentSize = { 0.0,40.0 };
			bool bSelectionAvail = false;
			int control_image_size = 42; //32;

			//PE: Make room for tool icons.
			content_avail = ImGui::GetContentRegionAvail();
			if (vBelowContentSize.y != 40.0f) {
				if (vBelowContentSize.y > 84) //2 lines of icons max. otherwise scrollbar.
					vBelowContentSize.y = 84;
				content_avail.y -= (vBelowContentSize.y + 8.0f);
			}
			else
				content_avail.y -= fsy * 3.0;
			if (content_avail.y < fsy) content_avail.y = fsy;

			ImGui::BeginChild("##CurrentObjectsLeftPanel", content_avail, false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavInputs);

			extern sObject* g_highlight_pobject;
			extern std::vector<sRubberBandType> entityselectionlist;
			g_highlight_pobject = NULL;
			entityselectionlist.clear();
			int iUniqueTreeId = 67671;

			#define GROUPV2


			int iInsideTab = 0;
			if (ImGui::BeginTabBar("currentandgrouptabbar"))
			{
				int tabflags = 0;
				if (i_switch_group_tab == 1)
				{
					i_switch_group_tab = 0;
					tabflags = ImGuiTabItemFlags_SetSelected;
				}
				if (ImGui::BeginTabItem(" Current Objects ", NULL, tabflags))
				{
					iInsideTab = 1;
					tabflags = 0;

					ImGui::SetWindowFontScale(fFontSize_leftpanel);
					//PE: To stop flicker of icons in list. it can take some frames before t.widget.pickedObject is set.
					static int iLastWidgetPickedObject[4] = { -1,-1,-1,-1 };
					if (t.gridentity <= 0)
					{
						iLastWidgetPickedObject[0] = iLastWidgetPickedObject[1];
						iLastWidgetPickedObject[1] = iLastWidgetPickedObject[2];
						iLastWidgetPickedObject[2] = t.widget.pickedObject;
					}

					int iFirstIcon = -1;
					int iFirstEntityId = -1;
					if (t.gridentity > 0)
					{
						iFirstIcon = t.gridentity;
						iLastWidgetPickedObject[2] = -1;
					}
					else if (t.widget.pickedEntityIndex > 0) // t.widget.pickedObject > 0 && 
					{
						//Ignore picked if rubberband.
						if (!g.entityrubberbandlist.size() > 0)
						{
							int bankindex = t.entityelement[t.widget.pickedEntityIndex].bankindex;
							iFirstIcon = bankindex;
							iFirstEntityId = t.widget.pickedEntityIndex;
						}
					}

					ImGui::Columns(iColumns_leftpanel, "CurrentObjectsAdditional", false);  //false no border

					if (iFirstIcon > 0 && t.gridentityinzoomview == 0)
					{

						bool bUseWideThumb = false;
						int iTextureID = t.entityprofile[iFirstIcon].iThumbnailSmall;
						#ifdef USEWIDEICONSEVERYWHERE
						if (t.entityprofile[iFirstIcon].iThumbnailLarge > 0)
						{
							bUseWideThumb = true;
							iTextureID = t.entityprofile[iFirstIcon].iThumbnailLarge;
						}
						#endif

						if (iTextureID > 0)
						{

							bSelectionAvail = true;
							bool isThumbHovered = false;
							ImGui::PushID(uniqueId++);
							float fCenterX = ImGui::GetContentRegionAvail().x * 0.5;
							ImVec2 vIconSize = { (float)media_icon_size_leftpanel , (float)media_icon_size_leftpanel };

							if (!bUseWideThumb)
							{
								ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + (fCenterX - (media_icon_size_leftpanel*0.5)), ImGui::GetCursorPosY()));
							}
							else
							{
								//512x288
								float fRatio = 288.0f / 512.0f;
								float fImageWidth = ImGui::GetContentRegionAvail().x - 4.0f;
								vIconSize = { fImageWidth ,fImageWidth*fRatio };
							}

							ImVec2 vToolsPos = ImGui::GetCursorPos();
							
							if (ImGui::ImgBtn(iTextureID, vIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down, -1, 0, 0, 0, true))
							{
								//Add to cursor.
								if (t.widget.pickedObject > 0 && t.widget.pickedEntityIndex > 0)
								{
									AddEntityToCursor(t.widget.pickedEntityIndex);
									// AddEntityToCursor used to both pick up an object, but in this case
									// we are duplicating, so we do need to wipe out certain real-time per-instance data
									t.grideleprof.newparticle.emitterid = -1; //LB: Must always get a new emitter ID.
									entity_cleargrideleprofrelationshipdata();
									bDraggingActive = false;
								}
							}
							if (ImGui::IsItemHovered())
								isThumbHovered = true;

							if (bWaitOnMouseRelease)
							{
								if (!ImGui::IsMouseDown(0))
									bWaitOnMouseRelease = false;
							}

							if (pref.iEnableDragDropEntityMode && !bWaitOnMouseRelease && t.gridentity == 0 && t.gridentityobj == 0 && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
							{
								if (t.widget.pickedObject > 0 && t.widget.pickedEntityIndex > 0)
								{
									StartDragDropFromEntityID(t.widget.pickedEntityIndex);
								}
							}

							//#### Locked Objects ####
							bool isObjectInLocedList = false;
							int iObjectLockedIndix = -1;
							if (vEntityLockedList.size() > 0)
							{
								for (int i = 0; i < vEntityLockedList.size(); i++)
								{
									int e = vEntityLockedList[i].e;
									if (e < 0 || e >= t.entityelement.size()) continue;

									if (e == t.widget.pickedEntityIndex)
									{
										isObjectInLocedList = true;
										iObjectLockedIndix = i;
										break;
									}
								}
							}
							if (isObjectInLocedList) 
							{
								int iImageSize = 20;
								ImVec2 opos = ImGui::GetCursorPos();
								ImGui::SetCursorPos(ImVec2(vToolsPos.x + vIconSize.x - 17.0f, vToolsPos.y - 19.0f + vIconSize.y));
								ImGui::SetItemAllowOverlap();
								if (ImGui::ImgBtn(TOOL_LOCK, ImVec2(iImageSize, iImageSize), ImColor(0, 0, 0, 0), ImColor(255, 255, 255, 180), ImColor(255, 255, 255, 180), ImColor(255, 255, 255, 180), 0, 0, 0, 0, false,false,false,false,false, bBoostIconColors))
								{
									LockSelectedObject(false, iObjectLockedIndix);
								}
								ImGui::SetCursorPos(opos);
								if (ImGui::IsItemHovered() && bToolTipActive) 
								{
									isThumbHovered = false;
									ImGui::SetTooltip("UnLock Object");
								}
							}
							else
							{
								int iImageSize = 20;
								ImVec2 opos = ImGui::GetCursorPos();
								ImGui::SetCursorPos(ImVec2(vToolsPos.x + vIconSize.x - 17.0f, vToolsPos.y - 19.0f + vIconSize.y));
								ImGui::SetItemAllowOverlap();
								if (ImGui::ImgBtn(TOOL_UNLOCK, ImVec2(iImageSize, iImageSize), ImColor(0, 0, 0, 0), ImColor(255, 255, 255, 180), ImColor(255, 255, 255, 180), ImColor(255, 255, 255, 180), 0, 0, 0, 0, false,false,false,false,false, bBoostIconColors))
								{
									LockSelectedObject(true, iObjectLockedIndix);
								}
								ImGui::SetCursorPos(opos);
								if (ImGui::IsItemHovered() && bToolTipActive) 
								{
									isThumbHovered = false;
									ImGui::SetTooltip("Lock Object");
								}
							}

							if (!bToolTipActive)
								isThumbHovered = false;

							static int ContextSel = -1;
							if (isThumbHovered || ContextSel == iFirstIcon)
							{
								if (ImGui::BeginPopupContextWindow())
								{
									ContextSel = iFirstIcon;
									ListGroupContextMenu(true);
									ImGui::EndPopup();
								}
							}

							if (isThumbHovered) {
								ImGui::SetTooltip("%s", t.entityprofileheader[iFirstIcon].desc_s.Get());
								if (iFirstEntityId > 0 && t.entityelement[iFirstEntityId].obj > 0) {
									if (g_ObjectList[t.entityelement[iFirstEntityId].obj]) {
										g_highlight_pobject = g_ObjectList[t.entityelement[iFirstEntityId].obj];
									}
								}

							}

							if (bDisplayText_leftpanel) {
								#ifdef USEWIDEICONSEVERYWHERE
								ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, -5.0f));
								#endif
								if (iFirstEntityId > 0)
									ImGui::TextCenter("%s", t.entityelement[iFirstEntityId].eleprof.name_s.Get()); //no wrap.
								else
									ImGui::TextCenter("%s", t.entityprofileheader[iFirstIcon].desc_s.Get()); //no wrap.
							}
							ImGui::PopID();
							preview_count++;
							ImGui::NextColumn();
						}
					}

					if (g.entityrubberbandlist.size() > 0 && t.gridentityinzoomview == 0)
					{
						bSelectionAvail = true;
						for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
						{
							bool bValid = true;
							int e = g.entityrubberbandlist[i].e;
							int tobj = t.entityelement[e].obj;
							int bankindex = t.entityelement[e].bankindex;

							//PE: Make sure we dont display the one attached to the cursor, already displayed and is not valid.
							if (bankindex == 0) bValid = false;

							if (bValid)
							{
								bool bUseWideThumb = false;
								int iTextureID = t.entityprofile[bankindex].iThumbnailSmall;

#ifdef USEWIDEICONSEVERYWHERE
								if (t.entityprofile[bankindex].iThumbnailLarge > 0)
								{
									bUseWideThumb = true;
									iTextureID = t.entityprofile[bankindex].iThumbnailLarge;
								}
#endif

								if (iTextureID > 0)
								{
									bool isThumbHovered = false;
									ImGui::PushID(uniqueId++);
									float fCenterX = ImGui::GetContentRegionAvail().x * 0.5;
									ImVec2 vIconSize = { (float)media_icon_size_leftpanel , (float)media_icon_size_leftpanel };

									if (!bUseWideThumb)
									{
										ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + (fCenterX - (media_icon_size_leftpanel*0.5)), ImGui::GetCursorPosY()));
									}
									else
									{
										//512x288
										float fRatio = 288.0f / 512.0f;
										float fImageWidth = ImGui::GetContentRegionAvail().x - 4.0f;
										vIconSize = { fImageWidth ,fImageWidth*fRatio };
									}

									ImVec2 vToolsPos = ImGui::GetCursorPos();
									if (ImGui::ImgBtn(iTextureID, vIconSize, drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down, -1, 0, 0, 0, true))
									{
										//Copy to cursor.
										AddEntityToCursor(e);
										bDraggingActive = false;
									}
									if (ImGui::IsItemHovered())
										isThumbHovered = true;

									if (bWaitOnMouseRelease)
									{
										if (!ImGui::IsMouseDown(0))
											bWaitOnMouseRelease = false;
									}

									if (pref.iEnableDragDropEntityMode && !bWaitOnMouseRelease && t.gridentity == 0 && t.gridentityobj == 0 && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
									{
										StartDragDropFromEntityID(e);
									}

									//#### Locked Objects ####
									bool isObjectInLocedList = false;
									int iObjectLockedIndix = -1;
									if (vEntityLockedList.size() > 0)
									{
										for (int i = 0; i < vEntityLockedList.size(); i++)
										{
											if (e == vEntityLockedList[i].e)
											{
												isObjectInLocedList = true;
												iObjectLockedIndix = i;
												break;
											}
										}
									}
									if (isObjectInLocedList) 
									{
										int iImageSize = 20;
										ImVec2 opos = ImGui::GetCursorPos();
										ImGui::SetCursorPos(ImVec2(vToolsPos.x + vIconSize.x - 17.0f, vToolsPos.y - 19.0f + vIconSize.y));
										ImGui::SetItemAllowOverlap();
										if (ImGui::ImgBtn(TOOL_LOCK, ImVec2(iImageSize, iImageSize), ImColor(0, 0, 0, 0), ImColor(255, 255, 255, 180), ImColor(255, 255, 255, 180), ImColor(255, 255, 255, 180), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
										{
											if (iObjectLockedIndix >= 0) 
											{
												t.entityelement[e].editorlock = 0;
												sObject* pObject;
												if (t.entityelement[e].obj > 0) 
												{
													pObject = g_ObjectList[t.entityelement[e].obj];
													if (pObject) 
													{
														WickedCall_SetObjectRenderLayer(pObject, GGRENDERLAYERS_NORMAL);
													}
												}
												vEntityLockedList.erase(vEntityLockedList.begin() + iObjectLockedIndix);
											}
											// any lock/unlock operations resets, avoids issue of duplcating a static object and unable to 'move' it
											t.widget.pickedObject = 0;
										}
										ImGui::SetCursorPos(opos);
										if (ImGui::IsItemHovered() && bToolTipActive) 
										{
											isThumbHovered = false;
											ImGui::SetTooltip("UnLock Object");
										}
									}
									else
									{
										int iImageSize = 20;
										ImVec2 opos = ImGui::GetCursorPos();
										ImGui::SetCursorPos(ImVec2(vToolsPos.x + vIconSize.x - 17.0f, vToolsPos.y - 19.0f + vIconSize.y));
										ImGui::SetItemAllowOverlap();
										if (ImGui::ImgBtn(TOOL_UNLOCK, ImVec2(iImageSize, iImageSize), ImColor(0, 0, 0, 0), ImColor(255, 255, 255, 180), ImColor(255, 255, 255, 180), ImColor(255, 255, 255, 180), 0, 0, 0, 0, false, false, false, false, false, bBoostIconColors))
										{
											t.entityelement[e].editorlock = 1 - t.entityelement[e].editorlock;
											sObject* pObject;
											if (t.entityelement[e].obj > 0) 
											{
												pObject = g_ObjectList[t.entityelement[e].obj];
												if (pObject) {
													if (t.entityelement[e].editorlock)
													{
														#ifndef ALLOWSELECTINGLOCKEDOBJECTS
														WickedCall_SetObjectRenderLayer(pObject, GGRENDERLAYERS_CURSOROBJECT);
														#endif
														sRubberBandType vEntityLockedItem;
														vEntityLockedItem.e = e;
														vEntityLockedList.push_back(vEntityLockedItem);
													}
													else 
													{
														//Delete from list.
														for (int i = 0; i < vEntityLockedList.size(); i++)
														{
															if (vEntityLockedList[i].e == e) 
															{
																vEntityLockedList.erase(vEntityLockedList.begin() + i);
																break;
															}
														}
														WickedCall_SetObjectRenderLayer(pObject, GGRENDERLAYERS_NORMAL);
													}
												}
											}
											// any lock/unlock operations resets, avoids issue of duplcating a static object and unable to 'move' it
											t.widget.pickedObject = 0;
										}
										ImGui::SetCursorPos(opos);
										if (ImGui::IsItemHovered() && bToolTipActive) 
										{
											isThumbHovered = false;
											ImGui::SetTooltip("Lock Object");
										}
									}

									if (!bToolTipActive)
										isThumbHovered = false;

									static int ContextSel = -1;
									if (isThumbHovered || ContextSel == e)
									{
										if (ImGui::BeginPopupContextWindow())
										{
											ContextSel = e;
											ListGroupContextMenu(true, e);
											ImGui::EndPopup();
										}
									}

									if (isThumbHovered) {
										ImGui::SetTooltip("%s", t.entityprofileheader[bankindex].desc_s.Get());
										if (e > 0 && t.entityelement[e].obj > 0) {
											if (g_ObjectList[t.entityelement[e].obj]) {
												g_highlight_pobject = g_ObjectList[t.entityelement[e].obj];
											}
										}
									}

									if (bDisplayText_leftpanel) {
#ifdef USEWIDEICONSEVERYWHERE
										ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, -5.0f));
#endif
										if(e > 0)
											ImGui::TextCenter("%s", t.entityelement[e].eleprof.name_s.Get()); //no wrap.
										else
											ImGui::TextCenter("%s", t.entityprofileheader[bankindex].desc_s.Get()); //no wrap.
									}
									ImGui::PopID();
									preview_count++;
									ImGui::NextColumn();
								}
							} //BValid
						}
					}

					ImGui::Columns(1);
					ImGui::SetWindowFontScale(1.00);

					if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) {
						//Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
						ImGui::Text("");
						ImGui::Text("");
						ImGui::Text("");
					}

					ImGui::EndTabItem();
				}

				tabflags = 0;
				if (i_switch_group_tab == 2)
				{
					i_switch_group_tab = 0;
					tabflags = ImGuiTabItemFlags_SetSelected;
				}
				if (ImGui::BeginTabItem(" Groups ", NULL, tabflags))
				{
					if (ImGui::IsItemHovered() && bToolTipActive) ImGui::SetTooltip("Object Groups");

					//Use Columns and fixed size.
					ImGui::Columns(iColumns_leftpanel, "CurrentObjectsAdditional", false);  //false no border
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
					iInsideTab = 2;
					
					//#### Group Lists ####
					for (int l = 0; l < MAXGROUPSLISTS; l++)
					{
						if (vEntityGroupList[l].size() > 0)
						{
							//LB: quickly reject groups that have no image (i.e are child groups)
							if (iEntityGroupListImage[l] == 0)
								continue;

							//LB: reject groups that have names (which means they are groups for Smart Objects, not user created)
							if (sEntityGroupListName[l].Len() > 0)
								continue;

							cstr sGroupString = cstr("Group") + cstr(l + 1) + cstr(":") + cstr("Objects (") + cstr((int)vEntityGroupList[l].size()) + cstr(")");
							#ifdef GROUPV2
							float w = ImGui::GetContentRegionAvailWidth();
							float fPreviewImgSize = w - 10.0f;
							if (fPreviewImgSize > 200.0f) fPreviewImgSize = 200.0f;
							float ImgY = ImGui::GetFontSize();
							bool isClicked = false;
							ImVec2 vToolsPos = ImGui::GetCursorPos();
							if (iEntityGroupListImage[l] > 0 && ImageExist(iEntityGroupListImage[l]))
							{
								float ImgX = ImageWidth(iEntityGroupListImage[l]);
								ImgY = ImageHeight(iEntityGroupListImage[l]);
								float Ratio = fPreviewImgSize / ImgX;
								ImgY *= Ratio;

								if (current_selected_group == l)
								{
									window = ImGui::GetCurrentWindow();
									ImVec2 padding = { 3.0, 3.0 };
									const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(fPreviewImgSize, ImgY));
									window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
								}
								isClicked = ImGui::ImgBtn(iEntityGroupListImage[l], ImVec2(fPreviewImgSize, ImgY), drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false, false,false,false,false);
							}
							else
								isClicked = ImGui::Selectable(sGroupString.Get()); //PE: Missing ?

							if (ImGui::IsItemHovered())
							{
								//Highlight group.
								entityselectionlist = vEntityGroupList[l];
								if (iEntityGroupListImage[l] > 0 && ImageExist(iEntityGroupListImage[l]))
								{
									ImGui::BeginTooltip();
									float fRatio = (float)ImageHeight(iEntityGroupListImage[l]) / (float)ImageWidth(iEntityGroupListImage[l]);
									float imagew = 400.0f;
									float imageh = imagew * fRatio;
									ImGui::ImgBtn(iEntityGroupListImage[l], ImVec2(imagew, imageh), drawCol_back, drawCol_normal, drawCol_hover, drawCol_Down, 0, 0, 0, 0, false);
									ImGui::EndTooltip();
								}
							}

							if(isClicked)
							{
								current_selected_group = l;
								g.entityrubberbandlist = vEntityGroupList[l];
								//Set widget on first item in list.
								int e = g.entityrubberbandlist[0].e;
								if (e > 0)
								{
									if (t.entityelement[e].editorlock == 0)
									{
										t.widget.pickedEntityIndex = e;
										t.widget.pickedObject = t.entityelement[e].obj;
									}
								}
								//PE: Make sure next selection , delete rubberband if ctrl not used.
								iLastSelectedEntityGroup = -1;
								iLastSelectedEntity = -1;
							}

							if (bWaitOnMouseRelease)
							{
								if (!ImGui::IsMouseDown(0))
									bWaitOnMouseRelease = false;
							}

							if (pref.iEnableDragDropEntityMode && !bWaitOnMouseRelease && t.gridentity == 0 && t.gridentityobj == 0 && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
							{
								//Just use first entry.
								int e = vEntityGroupList[l][0].e;
								if (iEntityGroupListImage[l] > 0 && ImageExist(iEntityGroupListImage[l]))
									StartDragDropFromEntityID(e, l , iEntityGroupListImage[l]);
								else
									StartDragDropFromEntityID(e,l);
							}

							//Check if any item in group is locked.
							bool isObjectInLocedList = false;
							int iObjectLockedIndix = -1;
							if (vEntityGroupList[l].size() > 0)
							{
								for (int i = 0; i < vEntityGroupList[l].size(); i++)
								{
									int e = vEntityGroupList[l][i].e;
									if (e > 0 && t.entityelement[e].editorlock)
									{
										isObjectInLocedList = true;
										iObjectLockedIndix = i;
										break;
									}
								}
							}

							int iImageSize = 20;
							ImVec2 opos = ImGui::GetCursorPos();
							ImGui::SetCursorPos(ImVec2(vToolsPos.x + fPreviewImgSize - 22.0f, vToolsPos.y - 22.0f + ImgY));
							ImGui::SetItemAllowOverlap();
							ImGui::PushID( 223344 + l); //Need unique ids.
							if (isObjectInLocedList)
							{
								if (ImGui::ImgBtn(TOOL_LOCK, ImVec2(iImageSize, iImageSize), ImColor(0, 0, 0, 0), ImColor(255, 255, 255, 180), ImColor(255, 255, 255, 180), ImColor(255, 255, 255, 180), 0, 0, 0, 0, false,false,false,false,false, bBoostIconColors))
								{
									if (vEntityGroupList[l].size() > 0)
									{
										for (int i = 0; i < vEntityGroupList[l].size(); i++)
										{
											int e = vEntityGroupList[l][i].e;
											if (e > 0) {
												t.entityelement[e].editorlock = 0;
												sObject* pObject;
												if (t.entityelement[e].obj > 0) {
													pObject = g_ObjectList[t.entityelement[e].obj];
													if (pObject) {
														WickedCall_SetObjectRenderLayer(pObject, GGRENDERLAYERS_NORMAL);
													}
												}
												//Delete from list.
												for (int il = 0; il < vEntityLockedList.size(); il++)
												{
													if (vEntityLockedList[il].e == e) 
													{
														vEntityLockedList.erase(vEntityLockedList.begin() + il);
														break;
													}
												}
											}
										}

										// any lock/unlock operations resets, avoids issue of duplcating a static object and unable to 'move' it
										t.widget.pickedObject = 0;
									}
								}
								if (ImGui::IsItemHovered() && bToolTipActive) 
								{
									ImGui::SetTooltip("UnLock Group");
								}
							}
							else
							{
								if (ImGui::ImgBtn(TOOL_UNLOCK, ImVec2(iImageSize, iImageSize), ImColor(0, 0, 0, 0), ImColor(255, 255, 255, 180), ImColor(255, 255, 255, 180), ImColor(255, 255, 255, 180), 0, 0, 0, 0, false,false,false,false,false, bBoostIconColors))
								{
									if (vEntityGroupList[l].size() > 0)
									{
										for (int i = 0; i < vEntityGroupList[l].size(); i++)
										{
											int e = vEntityGroupList[l][i].e;
											if (e > 0)
											{
												t.entityelement[e].editorlock = 1;
												sObject* pObject;
												if (t.entityelement[e].obj > 0)
												{
													pObject = g_ObjectList[t.entityelement[e].obj];
													if (pObject) 
													{
														#ifndef ALLOWSELECTINGLOCKEDOBJECTS
														WickedCall_SetObjectRenderLayer(pObject, GGRENDERLAYERS_CURSOROBJECT);
														#endif
													}
												}
												sRubberBandType vEntityLockedItem;
												vEntityLockedItem.e = e;
												vEntityLockedList.push_back(vEntityLockedItem);
											}
										}

										// any lock/unlock operations resets, avoids issue of duplcating a static object and unable to 'move' it
										t.widget.pickedObject = 0;
									}
								}
								if (ImGui::IsItemHovered() && bToolTipActive) {
									ImGui::SetTooltip("Lock Group");
								}
							}
							ImGui::SetCursorPos(opos);
							ImGui::PopID();
							#endif
							ImGui::NextColumn();
						}
					}
					ImGui::PopStyleVar();
					ImGui::Columns(1);
					if (ImGui::GetCurrentWindow()->ScrollbarSizes.x > 0) 
					{
						// Hitting exactly at the botton could cause flicker, so add some additional lines when scrollbar on.
						ImGui::Text("");
						ImGui::Text("");
					}
					ImGui::EndTabItem();
				}
				
				tabflags = 0;
				if (i_switch_group_tab == 3)
				{
					i_switch_group_tab = 0;
					tabflags = ImGuiTabItemFlags_SetSelected;
				}
				ImGui::EndTabBar(); //PE:Fix Assert error , stacksize.
			}

			ImGui::EndChild();

			vBelowContentSize = ImGui::GetCursorPos();

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));


			float w = ImGui::GetWindowContentRegionWidth();
			int iTotalIcons = 3;
			if (pref.iObjectEnableAdvanced) iTotalIcons = 4;
			int icon_spacer = 10;
			float control_width = (control_image_size + 4.0) * iTotalIcons + 12.0;
			control_width += (icon_spacer*(iTotalIcons - 1));
			int indent = (w*0.5) - (control_width*0.5);
			if (indent < 2) indent = 2;
			ImGui::Indent(indent);

			ImVec2 restore_cursorpos = ImGui::GetCursorPos();

			window = ImGui::GetCurrentWindow();

			// Can create smart object from a group, so ask here
			static bool bGetNewSmartObjectName = false;
			static char pSmartObjectName[256];
			if (bGetNewSmartObjectName == true)
			{
				ImGui::SetNextWindowSize(ImVec2(26 * ImGui::GetFontSize(), 11 * ImGui::GetFontSize()), ImGuiCond_Once);
				ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
				cstr sUniqueWinName = cstr("Enter A Name for your Smart Object##Smart Object Name Window");
				bool bSmartObjectNameWindow = true; //PE: The window should always be open here.
				ImGui::Begin(sUniqueWinName.Get(), &bSmartObjectNameWindow, 0);
				ImGui::Indent(10);
				cstr sUniqueInputName = cstr("##Smart Object Name") + cstr(1);
				ImGui::PushItemWidth(-10);
				ImGui::Text("");
				ImGui::Text("Type a name for your Smart Object and press ENTER:");
				if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)) ImGui::SetKeyboardFocusHere(0);
				if (ImGui::InputText(sUniqueInputName.Get(), pSmartObjectName, 250, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					// save the group
					char pObjectSavedFilename[MAX_PATH];
					strcpy(pObjectSavedFilename, pSmartObjectName);
					if (SaveGroup(current_selected_group, pObjectSavedFilename) == true)
					{
						// success, go to preview to set thumbnail
						g_LastGroupSaved_s = pObjectSavedFilename;

						//PE: If we already have the original image use that no need for large preview. just goto the folder where it was created.
						CreateBackBufferCacheNameEx(g_LastGroupSaved_s.Get(), 512, 288, true);
						if (!FileExist(BackBufferCacheName.Get()))
						{
							extern cstr sGotoPreviewWithFile;
							extern int iGotoPreviewType;
							sGotoPreviewWithFile = pObjectSavedFilename;
							iGotoPreviewType = 2;
						}
						else
						{
							sStartLibrarySearchString = "user";
							iLastDisplayLibraryType = -1;
							bExternal_Entities_Window = true;
							iDisplayLibraryType = 0;
							iDisplayLibrarySubType = 0;
						}
					}

					// finished here
					bGetNewSmartObjectName = false;
				}
				if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;

				ImGui::PopItemWidth();
				ImGui::Text("");
				int iCancelSize = 100;
				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((ImGui::GetWindowContentRegionWidth()*0.5) - (iCancelSize*0.5), 0.0f));
				if ( !bSmartObjectNameWindow || ImGui::StyleButton("Cancel", ImVec2(iCancelSize, 0)))
				{
					//Window closed or cancel selected, just exit.
					bGetNewSmartObjectName = false;
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cancel");
				ImGui::Indent(-10);
				bImGuiGotFocus = true;
				ImGui::End();
			}
			for (int b = 0; b < iTotalIcons; b++)
			{
				//TOOL_UNLOCK
				ImVec4 IconActive = ImVec4(1.0, 1.0, 1.0, 1.0);
				ImVec4 IconInActive = ImVec4(0.7, 0.7, 0.7, 0.7);
				ImVec4 IconColor;
				bool bIconActive = false;
				int iIconID = 0;
				LPSTR pLabelToolTip = "";
				if (b == 0 ) { iIconID = TOOL_GROUP; pLabelToolTip = "Group Objects"; }
				if (b == 1 ) { iIconID = TOOL_UNGROUP; pLabelToolTip = "Ungroup Objects"; }
				if (b == 2) { iIconID = TOOL_GROUPEDIT; pLabelToolTip = "Edit Group"; }
				if (b == 3) { iIconID = TOOL_GROUPSAVE; pLabelToolTip = "Save Group"; }
				//PE: && bSelectionAvail Always display looks better.
				bool bValidSelectionForGroup = false;
				// if selection a smart object
				bool bIsASmartObject = false;
				if (current_selected_group != -1)
				{
					if (vEntityGroupList[current_selected_group].size() > 0)
					{
						int iParentGroupID = vEntityGroupList[current_selected_group][0].iParentGroupID;
						for (int i = 0; i < MAXGROUPSLISTS; i++)
						{
							if (vEntityGroupList[i].size() > 0)
							{
								if (vEntityGroupList[i][0].iGroupID == iParentGroupID)
								{
									if (sEntityGroupListName[i].Len() > 0)
									{
										// parent is a smart object
										bIsASmartObject = true;
									}
								}
							}
						}
					}
				}
				if (bIsASmartObject == true )
				{
					// yes, so change labels to reflect Smart Object editing
					if (iInsideTab == 1)
					{
						// but only when in current object view
						if (b == 0) { pLabelToolTip = "This selection is a Smart Object"; }
						if (b == 1) { pLabelToolTip = "Ungroup Smart Object"; }
						if (b == 2) { pLabelToolTip = "Edit Smart Object"; }
						if (b == 3) { pLabelToolTip = "Save As Smart Object"; }
						if (iIconID == TOOL_UNGROUP || iIconID == TOOL_GROUPEDIT || iIconID == TOOL_GROUPSAVE)
						{
							IconColor = IconActive;
							bIconActive = true;
						}
						else
						{
							IconColor = IconInActive;
						}
					}
					else
					{
						// smart objects have no controls in the group tab
						IconColor = IconInActive;
					}
				}
				else
				{
					if (bSelectionAvail)
					{
						// regular group must not be a smart object - need at least two objects to make a NEW group
						if (g.entityrubberbandlist.size() > 1)
						{
							bValidSelectionForGroup = true;
						}
					}
					if (bValidSelectionForGroup == true && !(iIconID == TOOL_UNGROUP || iIconID == TOOL_GROUPEDIT || iIconID == TOOL_GROUPSAVE))
					{
						IconColor = IconActive;
						bIconActive = true;
					}
					else
					{
						IconColor = IconInActive;
					}
					if ((iIconID == TOOL_UNGROUP || iIconID == TOOL_GROUPEDIT || iIconID == TOOL_GROUPSAVE) && current_selected_group >= 0 && iInsideTab == 2)
					{
						IconColor = IconActive;
						bIconActive = true;
					}
				}

				if (iIconID > 0)
				{
					//Highlight icon.
					if (iIconID == TOOL_GROUPEDIT && bIconActive)
					{
						if (current_selected_group >= 0 && group_editing_on)
						{
							ImVec2 padding = { 3.0, 3.0 };
							const ImRect image_bb((window->DC.CursorPos - padding), window->DC.CursorPos + padding + ImVec2(control_image_size, control_image_size));
							window->DrawList->AddRect(image_bb.Min, image_bb.Max, ImGui::GetColorU32(tool_selected_col), 0.0f, 15, 2.0f);
						}
					}

					if (ImGui::ImgBtn(iIconID, ImVec2(control_image_size, control_image_size), ImVec4(0.0, 0.0, 0.0, 0.0), IconColor, ImVec4(0.8, 0.8, 0.8, 0.8), ImVec4(0.8, 0.8, 0.8, 0.8), 0, 0, 0, 0, false, false, false, false,false, bBoostIconColors))
					{
						if ( 1 )
						{
							// clicked mode button
							if (iIconID == TOOL_GROUP)
							{
								CreateNewGroup(-1);
							}
							if (iIconID == TOOL_UNGROUP && bIconActive)
							{
								// show all game elements when editing a group
								gridedit_setsmartobjectvisibilityinrubberband(true);

								// then ungroup them so visible and ready to manage
								UnGroupSelected();
							}
							if (iIconID == TOOL_GROUPEDIT && bIconActive)
							{
								if (group_editing_on && current_selected_group >= 0)
								{
									//Turning off , refresh group thumb.
									g.entityrubberbandlist = vEntityGroupList[current_selected_group];
									GetRubberbandLowHighValues();
									//PE: Generate new thumbnail of group.
									BackBufferIsGroup = false;
									BackBufferEntityID = 0;
									BackBufferObjectID = 0;
									BackBufferImageID = iEntityGroupListImage[current_selected_group];
									BackBufferSizeX = 512 * 2.0f;
									BackBufferSizeY = 288 * 2.0f;
									BackBufferZoom = 1.0f;
									BackBufferCamLeft = 0.0f;
									BackBufferCamUp = 0.0f;
									bRotateBackBuffer = false;
									bBackBufferAnimated = false;
									bLoopBackBuffer = false;
									RevertBackbufferCubemap();
									BackBufferSnapShotMode = true;

									//PE: Fullscreen
									if (BitmapExist(99))
									{
										DeleteBitmapEx(99);
									}
									bFullScreenBackbuffer = true;
									bStopBackbufferGrab = 1;

									//if BackBufferSnapShotMode
									if (t.widget.pickedEntityIndex > 0 && t.widget.activeObject > 0)
									{
										widget_hide();
									}

									// smart object game elements hide their game elements when not editing them
									gridedit_setsmartobjectvisibilityinrubberband(false);
								}
								else
								{
									// show all game elements when editing a group
									gridedit_setsmartobjectvisibilityinrubberband(true);
								}
								group_editing_on = 1 - group_editing_on; //toggle

							}
							if (iIconID == TOOL_GROUPSAVE)
							{
								// save group as object
								if (current_selected_group >= 0)
								{
									bGetNewSmartObjectName = true;
									strcpy(pSmartObjectName, "");
								}
							}
						}
					}
					if (ImGui::IsItemHovered() && bToolTipActive) ImGui::SetTooltip(pLabelToolTip);

					if (iIconID == TOOL_GROUP)
					{
						if (ImGui::BeginPopupContextWindow())
						{
							ListGroupContextMenu();
							ImGui::EndPopup();
						}
					}


					restore_cursorpos = ImGui::GetCursorPos();
					if (b < iTotalIcons)
					{
						ImGui::SameLine();
						ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(icon_spacer, 0));
					}
					//PE: Support wrapping of icons.
					float caw = (ImGui::GetContentRegionAvailWidth() + (control_image_size*0.5));
					if (caw < control_image_size) {
						ImGui::SetCursorPos(restore_cursorpos);
					}
				}
			}
			ImGui::Indent(-indent);
			ImGui::SetCursorPos(restore_cursorpos); //Restore cursor.

			vBelowContentSize = ImGui::GetCursorPos() - vBelowContentSize;

			//Support DragDrop to remove objects.
			bb = { ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos(),ImGui::GetWindowContentRegionMax() + ImGui::GetWindowPos() };

			if (bTrashcanIconActive2)
				bTrashcanIconActive2 = false;
			bool bTmp = bTrashcanIconActive;
			DragDrop_CheckTrashcanDrop(bb);
			bTrashcanIconActive2 = bTrashcanIconActive;
			bTrashcanIconActive = bTmp;

			ImGui::End();

		}

		//################################
		//#### EBE BUILDER LEFT PANEL ####
		//################################


		//##############################
		//#### Bug Reporting system ####
		//##############################

		ProcessBugReporting();


		//####################
		//#### Storyboard ####
		//####################

		//PE: Delayed startup moved to process_storeboard so we can hide 3D editor.
		static bool bInitStoryboardStartup = true;
		if (bInitStoryboardStartup)
		{
			extern bool bSpecialEditorFromStandalone;
			extern bool bReturnToWelcome;
			if (bSpecialEditorFromStandalone && !bReturnToWelcome)
			{
				bStoryboardWindow = true;
				GGTerrain_CancelRamp();
			}
			else if (!pref.iDisplayWelcomeScreen)
			{
				// LB: If we are not displaying the welcome screen, we should display the storyboard window.
				// as the iLastInStoryboard flag to take the user direct to the last level edited does not work
				// but taking the user to the last used storyboard allows the user to select the level they 
				// wish to work on and avoids ever being in a level that has no associated FPM level.
				//if (pref.iLastInStoryboard)
				bool bAlwaysTakeUserWhoSkipsHubToLastStoryboard = true;
				if(bAlwaysTakeUserWhoSkipsHubToLastStoryboard==true)
				{
					bStoryboardWindow = true;
					GGTerrain_CancelRamp();
				}
			}
			bInitStoryboardStartup = false;
		}

		process_storeboard();

		//###################
		//#### RPG GAMES ####
		//###################
		#ifdef RPG_GAMES
		ProcessRPGSetupWindow();
		#endif

		//###########################
		//#### VISULS LEFT PANEL ####
		//###########################

		if (refresh_gui_docking == 0 && !Visuals_Tools_Window)
		{
			//Make sure window is setup in docking space.
			ImGui::Begin("Environment Effects##VisualsToolsWindow", &Visuals_Tools_Window, iGenralWindowsFlags);
			ImGui::End();
		}
		else 
		{
			//PE: Make sure we switch to the correct name , this can change in test game.
			extern cStr sWindowName;
			sWindowName = "Environment Effects##VisualsToolsWindow";
			tab_tab_visuals(1, 0);
		}

		bImGuiReadyToRender = true;

		if (refresh_gui_docking < 4) 
		{
			refresh_gui_docking++;
		}
		else 
		{
			if (!bImGuiInitDone)
			{
				WickedCall_EnableCameraLight(bEditorLight);
			}
			bImGuiInitDone = true;
			static bool bLeftPanelSelectedAsDefault = false;
			if (!bLeftPanelSelectedAsDefault) {
				ImGui::SetWindowFocus("Terrain Tools##Paint Terrain##TerrainToolsWindow");
				bLeftPanelSelectedAsDefault = true;
			}

		}

		//Some need launch after we have bImGuiReadyToRender , so prompt will work.
		if (iSkibFramesBeforeLaunch == 0) 
		{
			switch (iLaunchAfterSync)
			{
				case 503: //Save the actual map here!
				{
					::SetCursor(::LoadCursor(NULL, IDC_WAIT));
					ImGuiIO& io = ImGui::GetIO();
					io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
					bKeepWindowsResponding = true;

					iLaunchAfterSync = 0;
					if (iLaunchAfterSyncAction == 11)
					{
						strcpy(cTriggerMessage, "A start marker has been added to your new level. This is where you will start in this level when you press Test Level.");
						bTriggerMessage = true;
						iTriggerMessageDelay = 30;
						iTriggerMessageY = 1;
						iMessageTimer = 0;
						iLaunchAfterSyncAction = 0;

						//MD: Ensures the saved camera position is close to the player start marker. Otherwise, terrain generator camera position would be saved.
						t.editorfreeflight.c.x_f = t.gridentityposx_f + 10.0f;
						t.editorfreeflight.c.y_f = t.gridentityposy_f + 200.0f;
						t.editorfreeflight.c.z_f = t.gridentityposz_f + 10.0f;
						t.editorfreeflight.c.angx_f = 70.0f;
						t.editorfreeflight.c.angy_f = -70.0f;
					}
					gridedit_save_map();
					g.projectmodified = 0; gridedit_changemodifiedflag();
					g.projectmodifiedstatic = 0;

					bKeepWindowsResponding = false;
					io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
					::SetCursor(::LoadCursor(NULL, IDC_ARROW));

					break;
				}
				case 504: //Save the actual map here!
				{

					::SetCursor(::LoadCursor(NULL, IDC_WAIT));
					ImGuiIO& io = ImGui::GetIO();
					io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
					bKeepWindowsResponding = true;

					iLaunchAfterSync = 0;
					gridedit_save_map();
					g.projectmodified = 0; gridedit_changemodifiedflag();
					g.projectmodifiedstatic = 0;
					if (iLaunchAfterSyncAction == 1)
					{
						bStoryboardWindow = true;
						GGTerrain_CancelRamp();
					}
					if (iLaunchAfterSyncAction == 2)
					{
						iLaunchAfterSync = 2;
						iSkibFramesBeforeLaunch = 5;
					}

					if (iLaunchAfterSyncAction == 3)
					{
						//PE: Default to terrain tools , like when we launch Max.
						bForceKey = true;
						csForceKey = "t";
						bForceKey2 = true;
						csForceKey2 = "6";
						t.inputsys.domodeterrain = 1; t.inputsys.dowaypointview = 0;
						t.gridentitymarkersmodeonly = 0; t.grideditselect = 0;
						t.terrain.terrainpaintermode = 6;
						bTerrain_Tools_Window = true;
						// must reset any manual editing
						GGTerrain_ResetSculpting();
						void reset_terrain_paint_date(void);
						reset_terrain_paint_date();
						iLaunchAfterSync = 5;
						iSkibFramesBeforeLaunch = 5;
					}

					iLaunchAfterSyncAction = 0;

					bKeepWindowsResponding = false;
					io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
					::SetCursor(::LoadCursor(NULL, IDC_ARROW));

					break;
				}

				case 3: //Save

					GGTerrain_CancelRamp();

					iLaunchAfterSync = 0;
					if (g.projectmodified == 1)
					{
						// Make sure we have last ebe changes.
						if (t.ebe.on == 1)
						{
							ebe_hide();
						}

						// yes save first
						if (g.projectfilename_s == "")
						{
							t.returnstring_s = "";
							cStr tOldDir = GetDir();
							char * cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "fpm\0*.fpm\0", g.mysystem.mapbankAbs_s.Get(), NULL,true);
							SetDir(tOldDir.Get());
							if (cFileSelected && strlen(cFileSelected) > 0) 
							{
								t.returnstring_s = cFileSelected;
							}
							if (t.returnstring_s != "")
							{
								if (cstr(Lower(Right(t.returnstring_s.Get(), 4))) != ".fpm")  t.returnstring_s = t.returnstring_s + ".fpm";
								g.projectfilename_s = t.returnstring_s;
								bool oksave = true;
								if (FileExist(g.projectfilename_s.Get())) 
								{
									oksave = overWriteFileBox(g.projectfilename_s.Get());
								}
								if (oksave) 
								{
									//Do the actualy save here.
									iLaunchAfterSync = 503; 
									iSkibFramesBeforeLaunch = 3;

									//Add newly saved fpm level to recent list.
									int firstempty = -1;
									int i = 0;
									for (; i < REMEMBERLASTFILES; i++) 
									{
										if (firstempty == -1 && strlen(pref.last_open_files[i]) <= 0)
											firstempty = i;

										if (strlen(pref.last_open_files[i]) > 0 && pestrcasestr(g.projectfilename_s.Get(), pref.last_open_files[i])) 
										{ 
											//already there
											break;
										}
									}
									if (i >= REMEMBERLASTFILES) 
									{
										if (firstempty == -1) 
										{
											//No empty slots , rotate.
											for (int ii = 0; ii < REMEMBERLASTFILES - 1; ii++) 
											{
												strcpy(pref.last_open_files[ii], pref.last_open_files[ii + 1]);
											}
											strcpy(pref.last_open_files[REMEMBERLASTFILES - 1], g.projectfilename_s.Get());
										}
										else
											strcpy(pref.last_open_files[firstempty], g.projectfilename_s.Get());
									}
									strcpy(cTriggerMessage, "Saving Level ...");
									bTriggerMessage = true;
								}
							}
						}
						else
						{
							//Do the actualy save here.
							iLaunchAfterSync = 503; 
							iSkibFramesBeforeLaunch = 3;
							strcpy(cTriggerMessage, "Saving Level ...");
							bTriggerMessage = true;
						}
					}
					break;

				case 4: //Save As
				{
					GGTerrain_CancelRamp();

					cstr oldprojectfilename_s = g.projectfilename_s;
					iLaunchAfterSync = 0;
					if (t.ebe.on == 1)
						ebe_hide(); //Make sure we have last ebe changes.

					t.returnstring_s = "";
					cStr tOldDir = GetDir();

					// we know we need to focus on the mapbank associated with the current storyboard
					cstr correctFPMLocation_s = Storyboard.customprojectfolder;
					if (correctFPMLocation_s.Len() > 0)
					{
						correctFPMLocation_s += Storyboard.gamename;
						correctFPMLocation_s += "\\Files\\mapbank";
					}
					else
					{
						correctFPMLocation_s = g.mysystem.mapbankAbs_s.Get();
					}

					char* cFileSelected = (char*)noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "fpm\0*.fpm\0", correctFPMLocation_s.Get(), NULL, true);

					SetDir(tOldDir.Get());
					if (cFileSelected && strlen(cFileSelected) > 0) 
					{
						t.returnstring_s = cFileSelected;
					}
					if (t.returnstring_s != "")
					{
						if (cstr(Lower(Right(t.returnstring_s.Get(), 4))) != ".fpm")  t.returnstring_s = t.returnstring_s + ".fpm";
						g.projectfilename_s = t.returnstring_s;

						bool oksave = true;
						if (FileExist(g.projectfilename_s.Get())) 
						{
							oksave = overWriteFileBox(g.projectfilename_s.Get());
						}
						if (oksave) 
						{
							//Add newly saved fpm level to recent list.
							int firstempty = -1;
							int i = 0;
							for (; i < REMEMBERLASTFILES; i++) 
							{
								if (firstempty == -1 && strlen(pref.last_open_files[i]) <= 0)
									firstempty = i;
								if (strlen(pref.last_open_files[i]) > 0 && pestrcasestr(g.projectfilename_s.Get(), pref.last_open_files[i])) 
								{
									//already there
									break;
								}
							}
							if (i >= REMEMBERLASTFILES) 
							{
								if (firstempty == -1) 
								{
									//No empty slots , rotate.
									for (int ii = 0; ii < REMEMBERLASTFILES - 1; ii++) 
									{
										strcpy(pref.last_open_files[ii], pref.last_open_files[ii + 1]);
									}
									strcpy(pref.last_open_files[REMEMBERLASTFILES - 1], g.projectfilename_s.Get());
								}
								else
									strcpy(pref.last_open_files[firstempty], g.projectfilename_s.Get());
							}

							iLaunchAfterSync = 503; //Do the actualy save here.
							iSkibFramesBeforeLaunch = 3;

							//PE: Change storyboard node to new save as project name.
							if (iLevelEditorFromStoryboardID >= 0)
							{
								//Validate we have the correct one.
								if (iLevelEditorFromStoryboardID < STORYBOARD_MAXNODES)
								{
									if (stricmp(Storyboard.Nodes[iLevelEditorFromStoryboardID].level_name, oldprojectfilename_s.Get()) == NULL)
									{
										//Valid , Update node.

										//Only relative.
										char tmp[MAX_PATH];
										strcpy(tmp, t.returnstring_s.Get());
										char *find = (char *)pestrcasestr(tmp, "mapbank\\");
										if (find && find != &tmp[0]) strcpy(&tmp[0], find);

										std::string sLevelPath = &tmp[0];

										int iPos;
										for (iPos = strlen(tmp); iPos >= 0; iPos--)
											if (tmp[iPos] == '\\') break;
										if (iPos > 0) iPos++;
										std::string sLevelTitle = &tmp[iPos];
										replaceAll(sLevelTitle, ".fpm", "");

										strcpy(Storyboard.Nodes[iLevelEditorFromStoryboardID].level_name, sLevelPath.c_str());
										strcpy(Storyboard.Nodes[iLevelEditorFromStoryboardID].title, sLevelTitle.c_str());
									}
								}
							}
							strcpy(cTriggerMessage, "Saving Level ...");
							bTriggerMessage = true;
						}
					}
					break;
				}

				case 5: // New flatten level
				{
					GGTerrain_CancelRamp();

					iLaunchAfterSync = 0;
					int iRet = 0;
					if (!bNoSecondAsk)
					{
						iRet = AskSaveBeforeNewAction();
					}
					else
					{
						bNoSecondAsk = false;
						iRet = 0;
					}
					if (iRet != 2)
					{
						// need to trigger the new level code
						t.inputsys.donewflat = 1;
						t.inputsys.donew == 1;//?
						gridedit_new_map();
						t.inputsys.donewflat = 0;
						t.inputsys.donew = 0;
						iLaunchAfterSync = 80;
						iSkibFramesBeforeLaunch = 5;
						// go to procedural terrain generator
						bProceduralLevel = true;
						bProceduralLevelStartup = true; // will trigger a random theme to be selected (overriding EMPTY init above)
						GGTerrain_RemoveHeightMap();
						iLevelEditorFromStoryboardID = -1; //We cant update storyboard.

					}

					break;
				}

				case 6: // New level
				{
					GGTerrain_CancelRamp();

					iLaunchAfterSync = 0;
					int iRet = AskSaveBeforeNewAction();
					if (iRet != 2)
					{
						t.inputsys.donewflat = 0;
						t.inputsys.donew == 1;
						gridedit_new_map();
						t.inputsys.donewflat = 0;
						t.inputsys.donew = 0;

						iLaunchAfterSync = 80; //Update env
						iSkibFramesBeforeLaunch = 5;

						strcpy(cTriggerMessage, "New level created");
						bTriggerMessage = true;
					}
					iLastUpdateVeg = 0;
					bUpdateVeg = true;
					break;
				}


				case 8: //Import model.
				{
					iLaunchAfterSync = 0;

					// if free trial, no import
					if (g_bFreeTrialVersion == true)
					{
						bFreeTrial_Window = true;
						break;
					}

					GGTerrain_CancelRamp();

					if (sDefaultImportPath == "")
						sDefaultImportPath = g.fpscrootdir_s;
					cStr tOldDir = GetDir();
					char * cFileSelected;

					// if batch converting, keep going around until no files left in batch list
					extern bool bBatchConverting;
					if (bBatchConverting == true)
					{
						extern std::vector<cstr> batchFileList;
						int iBatchFileCount = batchFileList.size();
						if (iBatchFileCount > 0)
						{
							extern char cImportPath[MAX_PATH];
							strcpy(pLaunchAfterSyncPreSelectModel, cImportPath);
							strcat(pLaunchAfterSyncPreSelectModel, "\\");
							strcat(pLaunchAfterSyncPreSelectModel, batchFileList[iBatchFileCount-1].Get());
							batchFileList.pop_back();
						}
					}

					if (strlen(pLaunchAfterSyncPreSelectModel) > 0)
					{
						// can trigger the importer with a preselected model filename (used by scaling mode changes)
						cFileSelected = pLaunchAfterSyncPreSelectModel;
					}
					else
					{
						// otherwise by default it requests a model file
						cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, "All\0*.*\0X\0*.x\0DBO\0*.dbo\0OBJ\0*.obj\0FBX\0*.fbx\0GLTF\0*.gltf\0GLB\0*.glb\0\0\0", sDefaultImportPath.Get(), NULL, true, "Import Model");

						// store last manually selected model file (for reload for scaling mode)
						if ( cFileSelected ) 
							strcpy (pLaunchAfterSyncLastImportedModel, cFileSelected);
						else
							strcpy (pLaunchAfterSyncLastImportedModel, "");
					}
					SetDir(tOldDir.Get());
					if (cFileSelected && strlen(cFileSelected) > 0) 
					{
						char szModelPath[ MAX_PATH ];
						strcpy( szModelPath, cFileSelected );
						const char* pExtension = strrchr( szModelPath, '.' );
						if ( !pExtension )
						{
							strcpy(cTriggerMessage, "File extension not found");
							bTriggerMessage = true;
						}
						else
						{
							bool bFoundModel = true;
							// extract zip file and set szModelPath to first model found
							if ( _stricmp(pExtension, ".zip") == 0 )
							{
								char szSubZipPath[ MAX_PATH ];
								bool bFoundSubZip = false;

								bFoundModel = false;
								char rootFolder[ MAX_PATH ];
								strcpy( rootFolder, GG_GetWritePath() );
								strcat( rootFolder, "imported_models\\" );

								// add zip file name as a folder to extract to
								char* pSlash = strrchr( szModelPath, '/' );
								char* pBSlash = strrchr( szModelPath, '\\' );
								if ( pBSlash > pSlash ) pSlash = pBSlash;
								if ( pSlash ) strcat( rootFolder, pSlash+1 );
								
								// remove extension
								pSlash = strrchr( rootFolder, '.' );
								if ( pSlash ) *pSlash = 0;
								strcat( rootFolder, "\\" );

								char finalPath[ MAX_PATH ];
								
								// extract zip file
								mz_zip_archive zip_archive;
								memset(&zip_archive, 0, sizeof(zip_archive));
								if ( !mz_zip_reader_init_file( &zip_archive, szModelPath, 0 ) )
								{
									mz_zip_reader_end( &zip_archive );
									strcpy(cTriggerMessage, "Failed to open zip file for reading");
									bTriggerMessage = true;
								}
								int numFiles = mz_zip_reader_get_num_files( &zip_archive );
								char filename[ 1024 ];
								for( int i = 0; i < numFiles; i++ )
								{
									mz_zip_reader_get_filename( &zip_archive, i, filename, 1024 );

									if ( mz_zip_reader_is_file_a_directory( &zip_archive, i ) ) 
									{
										// skip empty folders
										continue;
									}
            
									if ( strlen(rootFolder) + strlen(filename) + 3 > MAX_PATH ) continue;

									strcpy( finalPath, rootFolder );
									strcat( finalPath, filename );
                        
									// look for model file
									if ( !bFoundModel )
									{
										pExtension = strrchr( filename, '.' );
										if ( pExtension )
										{
											if ( !stricmp(pExtension, ".x") 
											  || !stricmp(pExtension, ".dbo")
											  || !stricmp(pExtension, ".obj")
											  || !stricmp(pExtension, ".fbx")
											  || !stricmp(pExtension, ".gltf")
											  || !stricmp(pExtension, ".glb") )
											{
												bFoundModel = true;
												strcpy( szModelPath, finalPath );
											}
											if ( !stricmp(pExtension, ".zip") )
											{
												bFoundSubZip = true;
												strcpy( szSubZipPath, finalPath );
											}
										}
									}
            
									GG_CreatePath( finalPath );
            
									mz_zip_reader_extract_to_file( &zip_archive, i, finalPath, 0 );
								}
        
								mz_zip_reader_end( &zip_archive );

								if ( !bFoundModel && bFoundSubZip )
								{
									char* pSlash = strrchr( szSubZipPath, '/' );
									char* pBSlash = strrchr( szSubZipPath, '\\' );
									if ( pBSlash > pSlash ) pSlash = pBSlash;
									if ( pSlash ) strcat( rootFolder, pSlash+1 );
								
									// remove extension
									pSlash = strrchr( rootFolder, '.' );
									if ( pSlash ) *pSlash = 0;
									strcat( rootFolder, "\\" );

									char finalPath[ MAX_PATH ];
								
									// extract zip file
									mz_zip_archive zip_archive;
									memset(&zip_archive, 0, sizeof(zip_archive));
									if ( !mz_zip_reader_init_file( &zip_archive, szSubZipPath, 0 ) )
									{
										mz_zip_reader_end( &zip_archive );
										strcpy(cTriggerMessage, "Failed to open zip file for reading");
										bTriggerMessage = true;
									}
									int numFiles = mz_zip_reader_get_num_files( &zip_archive );
									char filename[ 1024 ];
									for( int i = 0; i < numFiles; i++ )
									{
										mz_zip_reader_get_filename( &zip_archive, i, filename, 1024 );

										if ( mz_zip_reader_is_file_a_directory( &zip_archive, i ) ) 
										{
											// skip empty folders
											continue;
										}
            
										if ( strlen(rootFolder) + strlen(filename) + 3 > MAX_PATH ) continue;

										strcpy( finalPath, rootFolder );
										strcat( finalPath, filename );
                        
										// look for model file
										if ( !bFoundModel )
										{
											pExtension = strrchr( filename, '.' );
											if ( pExtension )
											{
												if ( !stricmp(pExtension, ".x") 
												  || !stricmp(pExtension, ".dbo")
												  || !stricmp(pExtension, ".obj")
												  || !stricmp(pExtension, ".fbx")
												  || !stricmp(pExtension, ".gltf")
												  || !stricmp(pExtension, ".glb") )
												{
													bFoundModel = true;
													strcpy( szModelPath, finalPath );
												}
											}
										}
            
										GG_CreatePath( finalPath );
            
										mz_zip_reader_extract_to_file( &zip_archive, i, finalPath, 0 );
									}
        
									mz_zip_reader_end( &zip_archive );
								}
							}

							if ( !bFoundModel )
							{
								strcpy(cTriggerMessage, "Zip file does not contain a recognised model format");
								bTriggerMessage = true;
							}
							else
							{
								t.returnstring_s = szModelPath;
								const char* pExtension = strrchr( szModelPath, '.' );
								bool bPermittedFormat = false;
								if (stricmp(pExtension, ".x") == NULL) bPermittedFormat = true;
								if (stricmp(pExtension, ".dbo") == NULL) bPermittedFormat = true;
								if (stricmp(pExtension, ".obj") == NULL) bPermittedFormat = true;
								if (stricmp(pExtension, ".fbx") == NULL) bPermittedFormat = true;
								if (stricmp(pExtension, ".gltf") == NULL) bPermittedFormat = true;
								if (stricmp(pExtension, ".glb") == NULL) bPermittedFormat = true;
								if ( bPermittedFormat == true )
								{
									// load the model
									sDefaultImportPath = t.returnstring_s; //Remember last import path.
									t.timporterfile_s = t.returnstring_s;
									importer_loadmodel();
								}
								else 
								{
									strcpy(cTriggerMessage, "This is not a supported model file.");
									bTriggerMessage = true;
								}
							}
						}
					}
					if (bDelayedTutorialCheckAction == TOOL_IMPORT) 
					{
						bDelayedTutorialCheckAction = -1;
						TutorialNextAction();
					}
					// clear 'pLaunchAfterSyncPreSelectModel' as this is a one time use until set again
					if (strlen(pLaunchAfterSyncPreSelectModel) > 0)
					{
						strcpy (pLaunchAfterSyncPreSelectModel, "");
					}
					break;
				}

				case 80: //Update envmap
				{
					iLaunchAfterSync = 0;
					//Make sure we have envmap.
					visuals_justshaderupdate();
					t.visuals.refreshskysettingsfromlua = true;
					cubemap_generateglobalenvmap();
					t.visuals.refreshskysettingsfromlua = false;
					//extern bool bFullVegUpdate;
					//bFullVegUpdate = true;
					bUpdateVeg = true;

					//PE: We need to recreate probes after all objects is placed, so we can find the probe boundingbox.
					for (int te = 1; te <= g.entityelementlist; te++)
					{
						int entid = t.entityelement[te].bankindex;
						if (entid > 0)
						{
							entity_autoFlattenWhenAdded(te);
						}
					}
					if (bLaunchTestGameAfterLoad)
					{
						bLaunchTestGameAfterLoad = false;
						iLaunchAfterSync = 1;
					}
					if (bLaunchSaveStandalonefterLoad)
					{
						bExport_Standalone_Window = true;
						bLaunchSaveStandalonefterLoad = false;
					}
					if (bCloseStoryboardAfterLoad)
					{
						bStoryboardWindow = false;
						bCloseStoryboardAfterLoad = false;
					}

					break;
				}

				case 81: //Delayed window focus.
				{
					iLaunchAfterSync = 0;
					//Make sure we have envmap.
					ImGui::SetWindowFocus(cNextWindowFocus);
					strcpy(cNextWindowFocus, "");
					break;
				}

				case 82: //Delayed window focus.
				{
					iLaunchAfterSync = 0;
					g_bCharacterCreatorPlusActivated = true;
					break;
				}

				default:
					break;
			}
		}
		else
		{
			iSkibFramesBeforeLaunch--;
		}

		//###########################
		//#### Trigger A Message ####
		//###########################
		bool bForceMessageNoFade = false;
		gridedit_triggermessagehandler(bForceMessageNoFade);

		//PE: Hide transition from storyboard to terrain generator.
		if (iBlackoutForFrames > 0)
		{
			if (iBlackoutForFrames != 5)
			{
				//PE: newlevel: bBlockImGuiUntilNewFrame = true; Triggered , what to do ?
				if (bBlockImGuiUntilNewFrame)
					g_bNoSwapchainPresent = true;

				//PE: Hide everything in the background (wicked 3D stuff).
				ImGuiViewport* mainviewport = ImGui::GetMainViewport();
				if (0) //PE: Switched to normal message.
				{
					if (mainviewport)
					{
						ImDrawList* drawlist = ImGui::GetForegroundDrawList(mainviewport);
						if (drawlist)
						{
							ImVec4 monitor_col = ImVec4(0, 0, 0, 1.0); //Black for now.
							drawlist->AddRectFilled(ImVec2(-1, -1), ImGui::GetMainViewport()->Size + ImVec2(40, 40), ImGui::GetColorU32(monitor_col));

							ImGuiContext& g = *GImGui;
							ImGui::SetWindowFontScale(2.0);
							ImVec2 tsize = ImGui::CalcTextSize("Preparing the Terrain Generator.Please wait...");
							ImVec2 viewPortSize = ImGui::GetMainViewport()->Size;
							ImVec2 tpos = (viewPortSize * 0.5) - (tsize * 0.5);
							drawlist->AddText(g.Font, g.FontSize, tpos, ImGui::GetColorU32(ImGuiCol_Text), "Preparing the Terrain Generator.Please wait...");
							ImGui::SetWindowFontScale(1.0);

						}
					}
				}
			}
			iBlackoutForFrames--;
			if(iBlackoutForFrames == 0)
				g_bNoSwapchainPresent = false;
				
		}
	}

	// 191015 - Trigger quick start dialog when editor flowing
	if ( iCountDownToShowQuickStartDialog > 0 )
	{
		iCountDownToShowQuickStartDialog--;
		if ( iCountDownToShowQuickStartDialog == 0 )
		{
			// insert parental control prompt if not used before
			if ( g.quickparentalcontrolmode == 0 )
			{
				// 050416 - ensure once time entrance to parental control from boot-up
				g.quickparentalcontrolmode = 1;
				editor_showparentalcontrolpage ();
			}
		}
	}

	//  Do not get start memory until processed one loop (below)
	if (  t.gamememactuallyusedstarttriggercount>0 ) 
	{
		--t.gamememactuallyusedstarttriggercount;
		if (  t.gamememactuallyusedstarttriggercount == 0 ) 
		{
			g.gamememactuallyusedstart=SMEMAvailable(1);
		}
	}

	//  Machine Independent Speed
	game_timeelapsed ( );
	t.ts_f=(Timer()-t.tsl_f)/50.0 ; t.tsl_f=Timer();

	//  Send SteamID to the editor if needed
	mp_sendSteamIDToEditor ( );

	//  User input calls
	static bool imgui_onetime_init = false;
	if (g.globals.ideinputmode == 1) 
	{
		imgui_input_getcontrols();
	}
	else 
	{
		input_getcontrols();
	}
	if (!imgui_onetime_init) 
	{
		imgui_onetime_init = true;
	}

	bool bTitleSystemActive = true;
	if ((bTriggerWhatsNewInStoryboard && g_iWelcomeLoopPage == WELCOME_ANNOUNCEMENTS)) bTitleSystemActive = false;
	if (bTitleSystemActive)
	{
		// could not launch Welcome system before IMGUI inits, so flagged to happen here
		if (iTriggerWelcomeSystemStuff > 0 && iTriggerWelcomeSystemStuff < 6) iTriggerWelcomeSystemStuff++;
		if (iTriggerWelcomeSystemStuff > 5)
		{
			// Init or Cycle
			if (iTriggerWelcomeSystemStuff == 6)
			{
				// only show front dialogs if not resuming from previous session
				if (g.grestoreeditorsettings == 0)
				{
					// Welcome quick start page
					g.quickstartmenumode = 0;
					if (g.iFreeVersionModeActive != 0)
					{
						editor_showquickstart(0);
						iTriggerWelcomeSystemStuff = 99;
					}
					else
					{
						if (g.gshowonstartup == 1 || g.iTriggerSoftwareToQuit != 0)
						{
							if(!bAddWhatNewToMenu) //Already in menu.
								editor_showquickstart(0);
							iTriggerWelcomeSystemStuff = 99;
						}
						else
						{
							welcome_free();
							iTriggerWelcomeSystemStuff = 7;
						}
					}
				}
				else
				{
					// always need to close down loading splash
					welcome_free();
					iTriggerWelcomeSystemStuff = 7;
				}
			}
			else if (iTriggerWelcomeSystemStuff == 7)
			{
				//Exit wait for mouse release before closing welcome screen.
				if (t.inputsys.mclick == 0)
					iTriggerWelcomeSystemStuff = 0;
			}
			else
			{
				// Cycle - handle welcome loop
				if (welcome_cycle() == true)
				{
					welcome_free();
					iTriggerWelcomeSystemStuff = 7;
				}
			}

			//Make sure we dont sent input to rendertarget when in welcome.
			t.inputsys.xmouse = 500000;
			t.inputsys.ymouse = 0;
			t.inputsys.xmousemove = 0;
			t.inputsys.ymousemove = 0;
			set_inputsys_mclick(0);// t.inputsys.mclick = 0;
			t.inputsys.zmouse = 0;
			t.inputsys.wheelmousemove = 0;
			t.inputsys.activemouse = 0;
			t.syncthreetimes = 1;
			t.inputsys.k_s = "";
			t.inputsys.keyreturn = 0;
			t.inputsys.keyshift = 0;
			t.inputsys.keytab = 0;
			t.inputsys.keyleft = 0;
			t.inputsys.keyright = 0;
			t.inputsys.keyup = 0;
			t.inputsys.keydown = 0;
			t.inputsys.keycontrol = 0;
			t.inputsys.keyspace = 0;
			t.inputsys.kscancode = 0;
		}
	}

	// calc local cursor
	bool bPickActive = true;
	iReusePickObjectID = -1;
	pReusePickObject = 0;
	if (pref.iDragCameraMovement && t.ebe.on == 0 && bDragCameraActive) bPickActive = false;
	if (bPickActive)
	{
		input_calculatelocalcursor ();
	}

	// Character Creator Plus
	if ( g_bCharacterCreatorPlusActivated == true )
	{
		// character creator plus character edited in situ
		charactercreatorplus_loop();
	}

	//  Importer or Main Editor
	if ( t.importer.loaded != 0 || (t.interactive.active == 1 && (t.interactive.pageindex<21 || t.interactive.pageindex>90)) )
	{
		//  Importer control or Interactive Mode
		if (  t.importer.loaded != 0 ) 
		{
			importer_loop ( );
			importer_draw ( );
		}
	}
	else
	{
		// Editor Controls
		editor_constructionselection();

		if (t.grideditselect != 0)
		{
			//PE: Make sure to hide "cubes" when not editing.
			WickedCall_DisplayCubes(false);
		}

		if ( t.grideditselect == 3 || t.grideditselect == 4 ) 
		{
			// Entity controls
			editor_viewfunctionality ( );
		}
		else
		{
			editor_mainfunctionality ( );
			if ( t.grideditselect == 0 ) 
			{
				// Terrain controls
				t.terrain.camx_f=t.cx_f ; t.terrain.camz_f=t.cy_f;
				t.terrain.zoom_f=t.gridzoom_f*0.12;
				terrain_editcontrol ( );
				if (BackBufferImageID <= 0)
				{
					WickedCall_DisplayCubes(true);
				}

				//PE: Make sure clicks inside terrain tools also record a change, so level is saved.
				if (bImGuiRenderTargetFocus)
				{
					//  Any click inside 3D area constitues some sort of edit
					if (t.inputsys.mclick != 0)
					{
						g.projectmodified = 1;
						gridedit_changemodifiedflag();
						// effect on g.projectmodifiedstatic
					}
				}
			}
			else
			{
				if ( t.ebe.on == 1 )
				{
					// Easy Building Editor
					ebe_loop();
				}
				else
				{
					//  Non-terrain controls
					gridedit_mapediting ( );
				}
			}
		}
		editor_overallfunctionality ( );

		//  Handle visual components
		editor_detect_invalid_screen ( );
		editor_visuals ( );
		
		//  Ensure entity animations speeds are controlled
		if(!bExport_Standalone_Window)
			entity_loopanim ( );

		//  Widget control
		widget_loop ( );

		//  Ensure lighting is updated as lighting is edited and moved
		lighting_loop ( );

		//  Only show terrain cursor if in terrain edit mode
		if (  t.grideditselect == 0 && t.inputsys.mclick != 2 && t.inputsys.mclick != 4 && t.interactive.insidepanel == 0 ) 
		{
			terrain_cursor ( );
		}
		else
		{
			terrain_cursor_off ( );
		}

		//  Render terrain elements (shadowupdatepacer as shadow calc is expensive, time slice it)
		t.terrain.gameplaycamera=0;
		terrain_waterineditor ( );

		// 111115 - keep track of memory between sessions with simpler SYSMEM minus STARTMEM calculation
		g.gamememactuallyused = SMEMAvailable(1) - g.gamememactuallyusedstart;

		// 111115 - and introduce sliding effect to hide flicker due to reading direct system memory value
		t.tmempercdest_f = (g.gamememactuallyused+0.0f) / (g.gamememactualmaxrightnow+0.0f);
		if ( t.tmemperc_f < t.tmempercdest_f-0.01f )
		{
			t.tmemperc_f = t.tmemperc_f + 0.01f;
		}
		else
		{
			if ( t.tmemperc_f > t.tmempercdest_f+0.01f )
			{
				t.tmemperc_f = t.tmemperc_f - 0.01f;
			}
		}

		if (  t.tmemperc_f > 1.0f  )  t.tmemperc_f = 1.0;
		if (  g.ghidememorygauge == 0 ) 
		{
			// (Dave) - check the image exists
			if ( ImageExist ( g.editorimagesoffset+2 ) == 1 )
			{
				Ink (  Rgb(0,0,0),0  ); Box (  2,2,102,18 );
				Ink (  Rgb(0,255,0),0  ); Box (  2,2,3+(99*t.tmemperc_f),18 );
				PasteImage (  g.editorimagesoffset+2,2,2,1 );
			}
		}
		//  End of Character Creator branch
	}

	//  Constantly checking if VIDMEM invalidated
	editor_detect_invalid_screen ( );

	// 191015 - test level click prompt
	if ( g.showtestlevelclickprompt > 0 )
	{
		if ( timeGetTime() > g.showtestlevelclickprompt )
		{
			g.showtestlevelclickprompt = 0;
		}
		int iXPos = 630;
		int iYPos = abs ( cos( timeGetTime()/500.0f )*35.0f );
		PasteImage ( g.editorimagesoffset+61, iXPos - (ImageWidth(g.editorimagesoffset+61)/2), 50+iYPos );
	}

	//  Update screen (if mouse in 3D are)
	if (  t.recoverdonotuseany3dreferences == 0 ) 
	{
		//  editor super chuggy
		if (  t.inputsys.activemouse == 1 ) 
		{
			//  constant update
			SyncRate ( 0 ); SyncMask ( 1 ); Sync ( ); SleepNow ( 5 );
		}
		else
		{
			//  check for PAINT message
			OpenFileMap (  3, "FPSEXCHANGE" );
			SetEventAndWait (  3 );
			if (  GetFileMapDWORD( 3, 60 ) == 1 ) 
			{
				SetFileMapDWORD (  3,60,0  ); t.syncthreetimes=3;
				SetEventAndWait (  3 );
			}
			SyncRate (  0 );
			if (  t.syncthreetimes>0 ) {  --t.syncthreetimes; Sync ( ); }
			SleepNow ( 10 );
		}

		//  Detect if resolution changed (windows)
		editor_detect_invalid_screen ( );
	}

	if (g_bCascadeQuitFlag) 
	{
		int iRet = AskSaveBeforeNewAction();
		if (iRet == 2)
		{
			g_bCascadeQuitFlag = false;
		}
		else 
		{
			PostQuitMessage(0);
		}
	}
}

void mapeditorexecutable_finish(void)
{
	// End map editor program (moved above chdir and pref writes)
	common_justbeforeend();

	// Come out of Files folder
	SetCurrentDirectoryA(g.fpscrootdir_s.Get());

	if (t.game.set.ismapeditormode == 1) {

		cstr prefile = defaultWriteFolder;
		prefile += "gamegurumax.pref";
		FILE* preffile = GG_fopen(prefile.Get(), "wb+");
		if (preffile) 
		{
			fwrite(&pref, 1, sizeof(pref), preffile);
			fclose(preffile);
		}

		if (pref.save_layout) {
			char cmLayoutFile[MAX_PATH];
			sprintf(cmLayoutFile, "%suimax.layout", defaultWriteFolder);
			ImGui::SaveIniSettingsToDisk(cmLayoutFile);
		}
	}
}

void mapeditorexecutable(void)
{
	mapeditorexecutable_init();

	// main loop
	while (!g_bCascadeQuitFlag)
	{
		mapeditorexecutable_loop();
	}
	mapeditorexecutable_finish();
}

int AskSaveBeforeNewAction(void)
{
	int iAction = 0;
	if (g.projectmodified == 1)
	{
		iAction = askBoxCancel("Do you wish to save first?", "Confirmation"); //1==Yes 2=Cancel 0=No

		if (iAction == 1)
		{
			//  yes save first
			if (g.projectfilename_s == "")
			{
				t.returnstring_s = "";
				cStr tOldDir = GetDir();
				char * cFileSelected = (char *)noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, "fpm\0*.fpm\0", g.mysystem.mapbankAbs_s.Get(), NULL, true);
				SetDir(tOldDir.Get());
				if (cFileSelected && strlen(cFileSelected) > 0) {
					t.returnstring_s = cFileSelected;
				}
				if (t.returnstring_s != "")
				{
					if (cstr(Lower(Right(t.returnstring_s.Get(), 4))) != ".fpm")  t.returnstring_s = t.returnstring_s + ".fpm";
					g.projectfilename_s = t.returnstring_s;
					bool oksave = true;
					if (FileExist(g.projectfilename_s.Get())) {
						oksave = overWriteFileBox(g.projectfilename_s.Get());
					}
					if (oksave) {
						gridedit_save_map();
					}
				}
			}
			else
			{
				gridedit_save_map();
			}

			g.projectmodified = 0; gridedit_changemodifiedflag();
			g.projectmodifiedstatic = 0;
		}
	}
	return iAction;

}

void editor_detect_invalid_screen ( void )
{
}

void editor_showhelppage ( int iHelpType )
{
	return;
}

void editor_showparentalcontrolpage ( void )
{
	// allow parental control to be activated and deactivated
	 OpenFileMap (  1, "FPSEXCHANGE" );
	 SetEventAndWait (  1 );
	do
	{
		set_inputsys_mclick(MouseClick());// t.inputsys.mclick = MouseClick();
		FastSync (  );
	} 
	while ( !(  t.inputsys.mclick == 0 ) );
	t.inputsys.kscancode=0;
	t.asx_f=1.0;
	t.asy_f=1.0;
	t.imgx_f=ImageWidth(g.editorimagesoffset+8)*t.asx_f;
	t.imgy_f=ImageHeight(g.editorimagesoffset+8)*t.asy_f;
	Sprite (  123,-10000,-10000,g.editorimagesoffset+8 );
	SizeSprite (  123,t.imgx_f,t.imgy_f );
	t.lastmousex=MouseX() ; t.lastmousey=MouseY();
	t.tpressf1toleave=0;
	int iStayInParentalControlDialog = 1;
	cstr ParentFourDigitCode = "";
	int digitcode[5];
	digitcode[0] = 0;
	digitcode[1] = 0;
	digitcode[2] = 0;
	digitcode[3] = 0;
	int digitcodeindex = 0;
	int tpressedbefore = 0;
	bool bParentalToggleForcesQuit = false;
	while ( iStayInParentalControlDialog == 1 && t.inputsys.kscancode != 27 ) 
	{
		 t.inputsys.kscancode=GetFileMapDWORD( 1, 100 );
		 if (  GetFileMapDWORD( 1, 908 ) == 1  )  break;
		 if (  GetFileMapDWORD( 1, 516 )>0  )  break;
		int tnewkeycode = 0; // numpad detection
		if ( t.inputsys.kscancode == 45 ) tnewkeycode = 48;
		if ( t.inputsys.kscancode == 35 ) tnewkeycode = 49;
		if ( t.inputsys.kscancode == 40 ) tnewkeycode = 50;
		if ( t.inputsys.kscancode == 34 ) tnewkeycode = 51;
		if ( t.inputsys.kscancode == 37 ) tnewkeycode = 52;
		if ( t.inputsys.kscancode == 12 ) tnewkeycode = 53;
		if ( t.inputsys.kscancode == 39 ) tnewkeycode = 54;
		if ( t.inputsys.kscancode == 36 ) tnewkeycode = 55;
		if ( t.inputsys.kscancode == 38 ) tnewkeycode = 56;
		if ( t.inputsys.kscancode == 33 ) tnewkeycode = 57;
		if ( tnewkeycode > 0 )
		{
			t.inputsys.kscancode = tnewkeycode;
		}
		if ( t.inputsys.kscancode >= 48 && t.inputsys.kscancode <= 57 )
		{
			if ( tpressedbefore == 0 && digitcodeindex < 4 )
			{
				if ( g.quickparentalcontrolmode == 1 )
				{
					// enable lock - entering password
					digitcode[digitcodeindex] = t.inputsys.kscancode;
					digitcodeindex++;
					ParentFourDigitCode = ParentFourDigitCode + "*";
					tpressedbefore = 1;
					if ( digitcodeindex == 4 )
					{
						g.quickparentalcontrolmode = 2;
						g.quickparentalcontrolmodepassword[0] = digitcode[0];
						g.quickparentalcontrolmodepassword[1] = digitcode[1];
						g.quickparentalcontrolmodepassword[2] = digitcode[2];
						g.quickparentalcontrolmodepassword[3] = digitcode[3];
						iStayInParentalControlDialog = 0;
						bParentalToggleForcesQuit = true;
						break;
					}
				}
				if ( g.quickparentalcontrolmode == 2 )
				{
					// disable lock - confirming password
					digitcode[digitcodeindex] = t.inputsys.kscancode;
					digitcodeindex++;
					ParentFourDigitCode = ParentFourDigitCode + "*";
					tpressedbefore = 1;
					if ( digitcodeindex == 4 )
					{
						// real password or secret backdoor
						bool bPasswordOkay = false;
						if (digitcode[0]==g.quickparentalcontrolmodepassword[0]
						&&	digitcode[1]==g.quickparentalcontrolmodepassword[1]
						&&	digitcode[2]==g.quickparentalcontrolmodepassword[2]
						&&	digitcode[3]==g.quickparentalcontrolmodepassword[3] ) bPasswordOkay = true;
						if (digitcode[0]==57 // 9119
						&&	digitcode[1]==49
						&&	digitcode[2]==49
						&&	digitcode[3]==57 ) bPasswordOkay = true;
						if ( bPasswordOkay == true )
						{
							g.quickparentalcontrolmode = 1;
							iStayInParentalControlDialog = 0;
							bParentalToggleForcesQuit = true;
							break;
						}
						else
						{
							// try again
							ParentFourDigitCode = "";
							digitcodeindex = 0;
						}
					}
				}

			}
		}
		else
		{
			tpressedbefore = 0;
		}

		t.terrain.gameplaycamera=0;
		int iDialogTop = (GetChildWindowHeight(0)-t.imgy_f)/2;
		PasteSprite ( 123, (GetChildWindowWidth(0)-t.imgx_f)/2, iDialogTop );
		LPSTR pRCMTitle = "RESTRICTED CONTENT MODE : OFF";
		if ( g.quickparentalcontrolmode == 2 ) pRCMTitle = "RESTRICTED CONTENT MODE : ON";
		pastebitmapfontcenter ( pRCMTitle, GetChildWindowWidth(0)/2, iDialogTop + 20, 3, 255 );
		pastebitmapfontcenter ( "This feature will control visibility of restricted content such as", GetChildWindowWidth(0)/2, iDialogTop + 70, 1, 255 );
		pastebitmapfontcenter ( "blood, violence and gore which may be offensive to some users.", GetChildWindowWidth(0)/2, iDialogTop + 95, 1, 255 );
		pastebitmapfontcenter ( "If you do not want this, press escape now.", GetChildWindowWidth(0)/2, iDialogTop + 120, 1, 255 );
		pastebitmapfontcenter ( ParentFourDigitCode.Get(), GetChildWindowWidth(0)/2, iDialogTop + (t.imgy_f/2), 4, 255 );
		if ( g.quickparentalcontrolmode == 2 )
		{
			pastebitmapfontcenter ( "ENTER YOUR FOUR DIGIT PASSWORD TO DEACTIVATE CONTENT LOCK", GetChildWindowWidth(0)/2, iDialogTop + t.imgy_f - 70, 2, 255 );
			pastebitmapfontcenter ( "OR PRESS [ESCAPE] TO CANCEL", GetChildWindowWidth(0)/2, iDialogTop + t.imgy_f - 50, 2, 255 );
		}
		else
		{
			pastebitmapfontcenter ( "ENTER FOUR DIGIT PASSWORD TO ACTIVATE CONTENT LOCK", GetChildWindowWidth(0)/2, iDialogTop + t.imgy_f - 70, 2, 255 );
			pastebitmapfontcenter ( "OR PRESS [ESCAPE] TO ENTER REGULAR MODE", GetChildWindowWidth(0)/2, iDialogTop + t.imgy_f - 50, 2, 255 );
		}
		pastebitmapfontcenter ( "YOU CAN ACCESS THIS OPTION AGAIN FROM THE HELP MENU", GetChildWindowWidth(0)/2, iDialogTop + t.imgy_f - 30, 2, 255 );
		Sync ( );
	}
	do
	{
		t.inputsys.kscancode=GetFileMapDWORD( 1, 100 );
		FastSync (  );
	} 
	while ( (  t.inputsys.kscancode > 3 ) ); //PE: We can keep getting virtual keys <= 3.
	//PE: Make sure we dont sent mouse input to whatever is below page.
	do
	{
		set_inputsys_mclick(MouseClick());// t.inputsys.mclick = MouseClick();
		FastSync();
	} while (!(t.inputsys.mclick == 0));

	// only a mode of 2 carries the digit code for activated
	if ( g.quickparentalcontrolmode != 2 )
	{
		digitcode[0] = 0;
		digitcode[1] = 0;
		digitcode[2] = 0;
		digitcode[3] = 0;
	}

	// 050416 - flag file to control parental control mode
	t.tfile_s=g.fpscrootdir_s+"\\parentalcontrolmode.ini";
	DeleteAFile (  t.tfile_s.Get() );
	if (  FileOpen(1)  ==  1  )  CloseFile (  1 );
	OpenToWrite (  1,t.tfile_s.Get() );
	WriteString (  1, cstr(g.quickparentalcontrolmode).Get() );
	WriteByte (  1, digitcode[0] );
	WriteByte (  1, digitcode[1] );
	WriteByte (  1, digitcode[2] );
	WriteByte (  1, digitcode[3] );
	CloseFile (  1 );
	t.tfile_s = g.fpscrootdir_s+"\\parentalcontrolactive.ini";
	if ( g.quickparentalcontrolmode == 2 )
	{
		// ensure file exists for IDE benefit
		OpenToWrite ( 1,t.tfile_s.Get() );
		WriteString ( 1, "123" );
		CloseFile ( 1 );
	}
	else
	{
		// delete this file to show IDE no parental control in effect
		DeleteAFile (  t.tfile_s.Get() );
	}

	// force the product to quit if change parental control setting
	if ( bParentalToggleForcesQuit == true )
	{
		MessageBoxA ( GetForegroundWindow(), "In order for the restricted content mode chosen to take effect, you must exit GameGuru and restart", "GameGuru Restart", MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST );
	}
}

void editor_freezeanimations ( void )
{
	// go through all objects and freeze their animations
	if ( t.fStoreObjAnimSpeeds==NULL )
	{
		t.fStoreObjAnimSpeeds = new float[210000];
		for ( int iObj = 1; iObj < 210000; iObj++ )
		{
			if ( ObjectExist ( iObj )==1 )
			{
				sObject* pObject = GetObjectData ( iObj );
				t.fStoreObjAnimSpeeds [ iObj ] = pObject->fAnimSpeed;
				pObject->fAnimSpeed = 0.0f;
			}
		}
	}
}

void editor_unfreezeanimations ( void )
{
	// go through all objects and restore all animation speeds from freeze step above
	if ( t.fStoreObjAnimSpeeds )
	{
		for ( int iObj = 1; iObj < 210000; iObj++ )
		{
			if ( ObjectExist ( iObj )==1 )
			{
				sObject* pObject = GetObjectData ( iObj );
				pObject->fAnimSpeed = t.fStoreObjAnimSpeeds [ iObj ];
			}
		}
		delete t.fStoreObjAnimSpeeds;
		t.fStoreObjAnimSpeeds = NULL;
	}
}

void editor_showquickstart ( int iForceMainOpen )
{
	// open welcome system
	editor_freezeanimations();
	if ( gbWelcomeSystemActive == false )
	{
		welcome_init(1);
		welcome_staticbackdrop();
		welcome_init(2);
	}
	welcome_init(0);

	// if first time run for VRQ
	if ( g.vrqTriggerSerialCodeEntrySystem == 1 )
	{
		 PostQuitMessage(0);
	}
	else
	{
		// Welcome system not syncronous any more
		if (g.iFreeVersionModeActive == 1)
		{
			welcome_show(WELCOME_FREEINTROAPP);
		}
		if (g.iFreeVersionModeActive == 2)
		{
			welcome_show(WELCOME_FREETRIALINTROAPP);
		}

		// if welcome not deactivated
		if (g.gshowonstartup != 0 || iForceMainOpen == 1)
		{
			// if no announcement wanting to share news
			if (g_iWelcomeLoopPage != WELCOME_ANNOUNCEMENTS)
			{
				// MAX only uses old welcome system for announcements right now
				welcome_show(WELCOME_ANNOUNCEMENTS);
				if (g_iWelcomeLoopPage != WELCOME_ANNOUNCEMENTS)
					bTriggerWhatsNewInStoryboard = false; //PE: No need to trigger nothing to show.
			}
			if (strlen(t.tlevelautoload_s.Get()) > 0)
			{
				//Trigger load level.
				welcome_free();
				t.tlevelautoload_s = "";
			}
		}
	}

	//  reset before leave
	t.inputsys.kscancode=0;
	set_inputsys_mclick(0);// t.inputsys.mclick = 0;
	t.inputsys.xmouse=0;
	t.inputsys.ymouse=0;
}

void editor_preparewindow (int iUseVRTest)
{
	//PE: Test game mode.
	extern DWORD gWindowVisible;

	RECT rect = { NULL };
	GetWindowRect(g_pGlob->hWnd, &rect);

	// correct size to restore (usingt "sizer" tool)
	gWindowSizeXOld = rect.right - rect.left;
	gWindowSizeYOld = rect.bottom - rect.top;

	gWindowPosXOld = rect.left;
	gWindowPosYOld = rect.top;

	gWindowVisibleOld = gWindowVisible; //SW_MAXIMIZE
	if (IsZoomed(g_pGlob->hWnd))
		gWindowMaximized = 1;
	else
		gWindowMaximized = 0;

	HMONITOR monitor = MonitorFromWindow(g_pGlob->hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &info);
	int monitor_width = info.rcMonitor.right - info.rcMonitor.left;
	int monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;
	g_pGlob->dwWindowX = info.rcMonitor.left;
	g_pGlob->dwWindowY = info.rcMonitor.top;

	//  First call will toggle keyboard/mouse back to BACKGROUND (to capture all direct data)
	SetWindowModeOn ();

	//PE: Test game mode.
	SetWindowSettings(0, 0, 0);
	SetWindowPos(g_pGlob->hWnd, HWND_TOP, g_pGlob->dwWindowX, g_pGlob->dwWindowY, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetForegroundWindow(g_pGlob->hWnd);
	SetWindowSize(monitor_width, monitor_height);
	ShowWindow(); MaximiseWindow();
}

void editor_previewmapormultiplayer_initcode ( int iUseVRTest )
{
	//  store if project modified
	t.storeprojectmodified=g.projectmodified;
	g_tstoreprojectmodifiedstatic = g.projectmodifiedstatic; 

	//  flag that we clicked TEST GAME
	t.interactive.testgameused=1;

	g.tabmodehidehuds = 0; //Enable HUD if lua disabled it in prev session.

	//  Before launch test game, check if enough contiguous
	checkmemoryforgracefulexit();

	// called here for non-standalone
	if (t.game.gameisexe == 0)
	{
		editor_preparewindow(iUseVRTest);
	}
	else
	{
		// and called earlier before splash screen so no flicker!
	}
	 
	//Hide any windows outside main viewport.
	ImGui::HideAllViewPortWindows();
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//  center mouse pointer in editor (and hide it)
	game_hidemouse ( );

	//  hide widget if was highlighted when tested game
	widget_hide ( );

	// hide EBE if active when click test game button
	ebe_hide();

	// hide texture terrain painter panels
	terrain_paintselector_hide();

	//  switch off any IDE entity highlighting
	t.geditorhighlightingtentityobj = 0;
	t.geditorhighlightingtentityID = 0;
	editor_restoreentityhighlightobj ( );

	// switch off any rubber band entity highlighting
	gridedit_clearentityrubberbandlist();

	// 210917 - refresh HLSL shaders (flagged as doing shader work)
	if ( g.gforceloadtestgameshaders == 1 )
	{
		// go through all loaded effects and reload them
		for ( t.t = -5 ; t.t <= g.effectbankmax; t.t++ )
		{
			char pEffectFilename[1024];
			strcpy ( pEffectFilename, "effectbank\\reloaded\\" );
			if ( t.t == -5 ) 
			{ 
				t.tteffectid = t.terrain.effectstartindex+0; 
				if ( g.gpbroverride == 1 )
					strcat ( pEffectFilename, "apbr_terrain.fx"); 
				else
					strcat ( pEffectFilename, "terrain_basic.fx"); 
			}
			if ( t.t == -4 ) 
			{ 
				t.tteffectid = t.terrain.effectstartindex+2; 
				if ( g.gpbroverride == 1 )
					strcat ( pEffectFilename, "apbr_veg.fx"); 
				else
					strcat ( pEffectFilename, "vegetation_basic.fx"); 
			}
			if ( t.t == -3 ) 
			{ 
				t.tteffectid = g.thirdpersonentityeffect; 
				if ( g.gpbroverride == 1 )
					strcat ( pEffectFilename, "apbr_basic.fx"); 
				else
					strcat ( pEffectFilename, "entity_basic.fx"); 
			}
			if ( t.t == -2 ) 
			{ 
				t.tteffectid = g.thirdpersoncharactereffect; 
				if ( g.gpbroverride == 1 )
					strcat ( pEffectFilename, "apbr_animwithtran.fx"); 
				else
					strcat ( pEffectFilename, "character_basic.fx"); 
			}
			if ( t.t == -1 ) { t.tteffectid = g.staticlightmapeffectoffset; strcat ( pEffectFilename, "static_basic.fx"); }
			if ( t.t == 0 ) { t.tteffectid = g.staticshadowlightmapeffectoffset; strcat ( pEffectFilename, "shadow_basic.fx"); }
			if ( t.t > 0 ) { t.tteffectid = g.effectbankoffset+t.t; strcpy ( pEffectFilename, t.effectbank_s[t.t].Get()); }
			if ( GetEffectExist ( t.tteffectid ) == 1 ) 
			{
				// gather all objects that use this effect
				int iObjListMax = 0;
				DWORD** pObjList = new DWORD* [ g_iObjectListCount ];
				memset ( pObjList, 0, sizeof(DWORD*)*g_iObjectListCount );
				for ( DWORD dwObject = 0; dwObject < (DWORD)g_iObjectListCount; dwObject++ )
				{
					sObject* pObject = g_ObjectList [ dwObject ];
					if ( pObject )
					{ 
						bool bAnyMeshUsingEffect = false;
						for ( DWORD dwMesh = 0; dwMesh < (DWORD)pObject->iMeshCount; dwMesh++ )
						{
							if ( pObject->ppMeshList [ dwMesh ]->pVertexShaderEffect == m_EffectList [ t.tteffectid ]->pEffectObj )
							{
								bAnyMeshUsingEffect = true;
							}
						}
						if ( bAnyMeshUsingEffect == true )
						{
							DWORD* pPerObjData = new DWORD[1+pObject->iMeshCount];
							memset ( pPerObjData, 0, sizeof(DWORD)*(1+pObject->iMeshCount) );
							pObjList[iObjListMax] = pPerObjData;
							*(pPerObjData+0) = dwObject;
							DWORD dwObjDataIndex = 1;
							for ( DWORD dwFrameIndex = 0; dwFrameIndex < (DWORD)pObject->iFrameCount; dwFrameIndex++ )
							{
								if ( pObject->ppFrameList [ dwFrameIndex ]->pMesh )
								{
									if ( pObject->ppFrameList [ dwFrameIndex ]->pMesh->pVertexShaderEffect == m_EffectList [ t.tteffectid ]->pEffectObj )
									{
										*(pPerObjData+dwObjDataIndex) = 1+dwFrameIndex;
										dwObjDataIndex++;
									}
								}
							}
							iObjListMax++;
						}
					}
				}

				// delete the old effect and load a new one
				DeleteEffect ( t.tteffectid );
				LoadEffect ( pEffectFilename, t.tteffectid, 0 );
				filleffectparamarray ( t.tteffectid );

				// set the new effects to each object in the list
				if ( iObjListMax > 0 )
				{
					for ( int iObjListIndex = 0; iObjListIndex < iObjListMax; iObjListIndex++ )
					{
						DWORD* pPerObjData = pObjList[iObjListIndex];
						DWORD dwObject = *(pPerObjData+0);
						sObject* pObject = g_ObjectList [ dwObject ];
						if ( pObject )
						{
							for ( DWORD dwObjDataIndex = 0; dwObjDataIndex < (DWORD)pObject->iMeshCount; dwObjDataIndex++ )
							{
								DWORD dwFrameIndex = *(pPerObjData+1+dwObjDataIndex);
								if ( dwFrameIndex > 0 )
								{
									dwFrameIndex--;
									SetLimbEffect ( dwObject, dwFrameIndex, t.tteffectid );
								}
							}
						}
						SAFE_DELETE(pPerObjData);
					}
				}
				SAFE_DELETE(pObjList);

				// by default, set to first technique
				SetEffectTechnique ( t.tteffectid, NULL );
			}
		}

		// re-assign params for reloaded terrain and veg
		terrain_applyshader();
	}
	
	//  set-up test game screen prompt assets
	if ( t.game.runasmultiplayer == 1 ) 
	{
		loadscreenpromptassets(2);
		 printscreenprompt("ENTERING MULTIPLAYER MODE");
	}
	else
	{
		loadscreenpromptassets(iUseVRTest);
		printscreenprompt("LAUNCHING TEST LEVEL");
	}

	//  Save editor configuration
	timestampactivity(0,"PREVIEWMAP: Save config");
	editor_savecfg ( );

	// level saving takes 25% of overall 'click test level on large level'
	// removed for now in favour of user choosing when they should save/backup their creations
	// can restore this if we can get save to sub-3 seconds.
	g.gpretestsavemode = 0;

	// Now saves all part-files into temp FPM file (which multiplayer can pick up later)
	if ( t.game.runasmultiplayer == 1 ) 
	{
		//  save temp copy of current level
		g.projectfilename_s=g.mysystem.editorsGrideditAbs_s+"worklevel.fpm";//g.fpscrootdir_s+"\\Files\\editors\\gridedit\\worklevel.fpm";
		editor_savecfg ( );
		mapfile_saveproject_fpm ( );
	}

	// GCStore could have assed assets since the last 'test game' so refresh internal lists
	sky_init ( );
	terrain_initstyles ( );

	// Re-acquire indices now the lists have changed
	// takes visuals.sky$ visuals.terrain$ visuals.vegetation$
	visuals_updateskyterrainvegindex ( );

	// Ensure game visuals settings used
	t.gamevisuals.skyindex=t.visuals.skyindex;
	t.gamevisuals.sky_s=t.visuals.sky_s;
	t.gamevisuals.terrainindex=t.visuals.terrainindex;
	t.gamevisuals.terrain_s=t.visuals.terrain_s;
	t.gamevisuals.vegetationindex=t.visuals.vegetationindex;
	t.gamevisuals.vegetation_s=t.visuals.vegetation_s;
	t.gamevisuals.iEnvironmentWeather = t.visuals.iEnvironmentWeather;

	// the visuals vs gamevisuals could do with some work, I noticed our ambience is being overwritten
	// when it really needed to be transferred to the gamevisuals (is this done elsewhere?)
	t.gamevisuals.AmbienceRed_f = t.visuals.AmbienceRed_f;
	t.gamevisuals.AmbienceGreen_f = t.visuals.AmbienceGreen_f;
	t.gamevisuals.AmbienceBlue_f = t.visuals.AmbienceBlue_f;
	t.gamevisuals.SunAngleX = t.visuals.SunAngleX;
	t.gamevisuals.SunAngleY = t.visuals.SunAngleY;
	t.gamevisuals.SunAngleZ = t.visuals.SunAngleZ;
	t.gamevisuals.bSSREnabled = t.visuals.bSSREnabled;
	t.gamevisuals.bFXAAEnabled = t.visuals.bFXAAEnabled;

	t.gamevisuals.bDOF = t.visuals.bDOF;
	t.gamevisuals.fDOFStrength = t.visuals.fDOFStrength;
	t.gamevisuals.fDOFApertureSize = t.visuals.fDOFApertureSize;
	t.gamevisuals.fDOFFocalLength = t.visuals.fDOFFocalLength;

	t.gamevisuals.bLightShafts = t.visuals.bLightShafts;
	t.gamevisuals.bLensFlare = t.visuals.bLensFlare;
	t.gamevisuals.bReflectionsEnabled = t.visuals.bReflectionsEnabled;
	t.gamevisuals.iShadowSpotCascadeResolution = t.visuals.iShadowSpotCascadeResolution;
	t.gamevisuals.iShadowPointMax = t.visuals.iShadowPointMax;
	t.gamevisuals.iShadowPointResolution = t.visuals.iShadowPointResolution;
	t.gamevisuals.iShadowSpotMax = t.visuals.iShadowSpotMax;
	t.gamevisuals.iShadowSpotResolution = t.visuals.iShadowSpotResolution;
	t.gamevisuals.iEnvProbeResolution = t.visuals.iEnvProbeResolution;
	t.gamevisuals.newperformancepresets = t.visuals.newperformancepresets;

	// ensure all optimization states are transferred to the game
	t.gamevisuals.shaderlevels.entities = t.visuals.shaderlevels.entities;
	t.gamevisuals.shaderlevels.lighting = t.visuals.shaderlevels.lighting;
	t.gamevisuals.shaderlevels.terrain = t.visuals.shaderlevels.terrain;
	t.gamevisuals.shaderlevels.vegetation = t.visuals.shaderlevels.vegetation;
	t.gamevisuals.bOcclusionCulling = t.visuals.bOcclusionCulling;
	t.gamevisuals.bEnableObjectCulling = t.visuals.bEnableObjectCulling;
	t.gamevisuals.bEnableTerrainChunkCulling = t.visuals.bEnableTerrainChunkCulling;
	t.gamevisuals.bEnablePointShadowCulling = t.visuals.bEnablePointShadowCulling;
	t.gamevisuals.bEnableSpotShadowCulling = t.visuals.bEnableSpotShadowCulling;
	t.gamevisuals.bEnableAnimationCulling = t.visuals.bEnableAnimationCulling;
	t.gamevisuals.fLODMultiplier = t.visuals.fLODMultiplier;
	t.gamevisuals.bDisableSkybox = t.visuals.bDisableSkybox;
	t.gamevisuals.bEnable30FpsAnimations = t.visuals.bEnable30FpsAnimations;

	t.gamevisuals.bShadowsLowestLOD = t.visuals.bShadowsLowestLOD;
	t.gamevisuals.bProbesLowestLOD = t.visuals.bProbesLowestLOD;
	t.gamevisuals.bRaycastLowestLOD = t.visuals.bRaycastLowestLOD;
	t.gamevisuals.bPhysicsLowestLOD = t.visuals.bPhysicsLowestLOD;
	t.gamevisuals.bThreadedPhysics = t.visuals.bThreadedPhysics;
	t.gamevisuals.bReflectionsLowestLOD = t.visuals.bReflectionsLowestLOD;

	t.gamevisuals.g_bDelayedShadows = t.visuals.g_bDelayedShadows;
	t.gamevisuals.g_bDelayedShadowsLaptop = t.visuals.g_bDelayedShadowsLaptop;
	t.gamevisuals.ApparentSize = t.visuals.ApparentSize;
	t.gamevisuals.bReflectionsEnabled = t.visuals.bReflectionsEnabled;
	t.gamevisuals.bLevelVSyncEnabled = t.visuals.bLevelVSyncEnabled;

	t.gamevisuals.ColorGradingLUT = t.visuals.ColorGradingLUT;
	t.gamevisuals.bColorGrading = t.visuals.bColorGrading;

	// copy game visuals to visuals for use in level play
	t.visuals = t.gamevisuals;

	gggrass_save_params = gggrass_global_params;

	if(pref.iTestGameGraphicsQuality != 2)
		SetGlobalGraphicsSettings( pref.iTestGameGraphicsQuality );

	t.visuals.refreshshaders=1;
	t.visuals.refreshvegtexture=1;

	// Hide camera while prepare test map
	t.storecx_f=CameraPositionX();
	t.storecy_f=CameraPositionY();
	t.storecz_f=CameraPositionZ();

	// default start position is edit-camera XZ
	t.terrain.playerx_f = CameraPositionX(0);
	t.terrain.playerz_f = CameraPositionZ(0);
	t.terrain.playery_f = BT_GetGroundHeight(t.terrain.TerrainID, t.terrain.playerx_f, t.terrain.playerz_f) + 150.0;
	t.terrain.playerax_f = 0.0;
	t.terrain.playeray_f = 0.0;
	t.terrain.playeraz_f = 0.0;

	// store all editor entity positions and rotations
	t.storedentityelementlist=g.entityelementlist;
	t.storedentityviewcurrentobj=g.entityviewcurrentobj;
	Dim (  t.storedentityelement,g.entityelementlist );
	for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
	{
		t.storedentityelement[t.e]=t.entityelement[t.e];
	}

	// hide all markers
	for (t.e = 1; t.e <= g.entityelementlist; t.e++)
	{
		t.entid = t.entityelement[t.e].bankindex;
		t.obj = t.entityelement[t.e].obj;
		if (t.obj > 0)
		{
			if (ObjectExist(t.obj) == 1)
			{
				if (t.entityprofile[t.entid].ismarker != 0)
				{
					//  all markers must be hidden
					HideObject(t.obj);
				}
				if (t.entityprofile[t.entid].addhandlelimb > 0)
				{
					//  hide decal handles
					HideLimb(t.obj, t.entityprofile[t.entid].addhandlelimb);
				}
			}
		}
	}
	
	// ensure all locked entity transparency resolves
	for ( t.tte = 1 ; t.tte<=  g.entityelementlist; t.tte++ )
	{
		if ( t.entityelement[t.tte].editorlock == 1 || t.entityelement[t.tte].underground == 1 ) 
		{
			t.tobj=t.entityelement[t.tte].obj;
			if ( t.tobj>0 ) 
			{
				if ( ObjectExist(t.tobj) == 1 ) 
				{
					if ( t.entityelement[t.tte].underground == 1  )  t.entityelement[t.tte].isclone = 1;
					entity_converttoinstance ( );
				}
			}
		}
	}

	//PE: start any animations that are not in editor mode.
	for (t.tte = 1; t.tte <= g.entityelementlist; t.tte++)
	{
		// hide EBE markers
		int iIndex = t.entityelement[t.tte].bankindex;
		if (t.entityprofile[iIndex].isebe != 0)
		{
			t.tobj = t.entityelement[t.tte].obj;
			if (t.tobj > 0)
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

				//FULLBOUNDS
				if (t.entityprofile[t.entid].startanimingame > 0) 
				{
					if (t.entityprofile[t.entid].animmax > 0) 
					{
						t.q = t.entityprofile[t.entid].startanimingame - 1;
						SetObjectFrame(t.tttsourceobj, 0);
						LoopObject(t.tttsourceobj, t.entityanim[t.entid][t.q].start, t.entityanim[t.entid][t.q].finish);
						SetObjectFrame(t.tobj, 0);
						LoopObject(t.tobj, t.entityanim[t.entid][t.q].start, t.entityanim[t.entid][t.q].finish);
					}
				}
				else 
				{
					//PE: Restore any non char animations.
					if (t.tobj > 0 && t.entityprofile[t.entid].ischaracter == 0 && GetNumberOfFrames(t.tobj) > 0)
					{
						SetObjectFrame(t.tobj, 0);
						if (t.entityprofile[t.entid].animmax > 0 && t.entityprofile[t.entid].playanimineditor > 0 && t.entityprofile[t.entid].ischaractercreator == 0)
						{
							t.q = t.entityprofile[t.entid].playanimineditor - 1;
							LoopObject(t.tobj, t.entityanim[t.entid][t.q].start, t.entityanim[t.entid][t.q].finish);
						}
						else if (t.entityprofile[t.entid].playanimineditor < 0)
						{
							// uses name instead of index, the negative is the ordinal into the animset
							extern void entity_loop_using_negative_playanimineditor(int e, int obj, cstr animname);
							entity_loop_using_negative_playanimineditor(t.tte, t.tobj, t.entityprofile[t.entid].playanimineditor_name);
						}
						else
						{
							LoopObject(t.tobj); StopObject(t.tobj);
						}
					}

				}
			}
		}
	}

	editor_toggle_element_vis(t.showtestgameelements);

	// hide editor objects too
	for ( t.obj = t.editor.objectstartindex+1; t.obj <= t.editor.objectstartindex+1+10 ;  t.obj++ ) //?//t.editor.objectstartindex+10;
	{
		if ( ObjectExist(t.obj) == 1 ) 
		{
			HideObject (  t.obj );
		}
	}

	// hide any EBE site markers (limb zeros)
	for ( t.tte = 1; t.tte <= g.entityelementlist; t.tte++ )
	{
		int iIndex = t.entityelement[t.tte].bankindex;
		if ( t.entityprofile[iIndex].isebe != 0 ) 
		{
			t.tobj = t.entityelement[t.tte].obj;
			if ( t.tobj>0 ) 
			{
				if ( ObjectExist(t.tobj) == 1 ) 
				{
					// when EBE entity is loaded first time, no editing, the handle object at limb zero is not used
					sObject* pObject = GetObjectData(t.tobj);
					if ( pObject->iFrameCount > 1 )
						HideLimb ( t.tobj, 0 );
				}
			}
		}
	}

	// ensure no collision from legacy engine
	AutomaticCameraCollision (  0,0,0 );
	SetGlobalCollisionOff (  );

	// Setup game view camera
	SetCameraFOV ( 75 );
	g.grav_f=-5.0;

	// store original terrain heights
	if ( t.terrain.TerrainID>0 ) 
	{
		for ( t.z = 0 ; t.z<=  1024; t.z++ )
		{
			for ( t.x = 0 ; t.x<=  1024; t.x++ )
			{
				t.h_f=BT_GetGroundHeight(t.terrain.TerrainID,t.x*50.0,t.z*50.0,1);
				t.terrainmatrix[t.x][t.z]=t.h_f;
			}
		}
	}

	// Create heightmap from this terrain (for quad reduction)
	if ( t.terrain.TerrainID>0 ) 
	{
		t.terrain.terrainregionupdate=0;
		terrain_refreshterrainmatrix ( );
		t.theightfile_s=g.mysystem.levelBankTestMap_s+"heightmap.dds"; //"levelbank\\testmap\\heightmap.dds";
		terrain_createheightmapfromheightdata ( );
	}

	// full speed
	SyncRate ( 0 );

	// Work out the amount of memory used for the TEST GAME session
	t.tmemorybeforetestgame=SMEMAvailable(1);

	//
	// launch game root with IDE 'test at cursor position' settings
	//
	t.game.set.resolution=0;
	t.game.set.initialsplashscreen=0;
	t.game.set.ismapeditormode=0;
	extern int tgamesetismapeditormode;
	tgamesetismapeditormode = 0;
	WickedCall_SetEditorCameraLight(false);

	// can now edit behavior logic 'live'
	extern void gridedit_restartanybehaviorediting(void);
	gridedit_restartanybehaviorediting();

	//PE: Clear any test mode / standalone highlights.
	extern std::vector<int> g_StandaloneObjectHighlightList;
	g_StandaloneObjectHighlightList.clear(); //PE: They clear on all frames so this is the only thing needed.
	extern bool bActivateStandaloneOutline;
	bActivateStandaloneOutline = false;
	// game loop init code
	game_masterroot_initcode ( iUseVRTest );
}

bool editor_previewmapormultiplayer_loopcode ( int iUseVRTest )
{
	bool bEndThisLoop = false;
	g_bDisableQuitFlag = true;
	bEndThisLoop = game_masterroot_loopcode ( iUseVRTest );
	g_bDisableQuitFlag = false;
	return bEndThisLoop;
}

void editor_previewmapormultiplayer_afterloopcode ( int iUseVRTest )
{
	// game after loop code
	game_masterroot_afterloopcode ( iUseVRTest );

	t.terrain.skysundirectionx_f = t.terrain.sundirectionx_f;
	t.terrain.skysundirectiony_f = t.terrain.sundirectiony_f;
	t.terrain.skysundirectionz_f = t.terrain.sundirectionz_f;

	t.game.set.ismapeditormode=1;
	extern int tgamesetismapeditormode;
	tgamesetismapeditormode = 1;

	//PE: This will make sure everything spawed is deleted , like physics / sound / waypoints / attachments ...
	void CleanUpSpawedObject(void);
	CleanUpSpawedObject();

	//PE: Clear all wicked particle effects created by lua.
	void CleanUpEmitterEffects(void);
	CleanUpEmitterEffects();

	WickedCall_SetEditorCameraLight(true);

	//PE: Hide any hit decals.
	decal_hide();

	// restore any EBE site markers (limb zeros)
	for ( t.tte = 1; t.tte <= g.entityelementlist; t.tte++ )
	{
		int iIndex = t.entityelement[t.tte].bankindex;
		if ( t.entityprofile[iIndex].isebe != 0 ) 
		{
			t.tobj = t.entityelement[t.tte].obj;
			if ( t.tobj>0 ) 
			{
				if ( ObjectExist(t.tobj) == 1 ) 
				{
					ShowLimb ( t.tobj, 0 );
				}
			}
		}
	}

	// Revert mode to only render NEAR technique
	visuals_restoreterrainshaderforeditor ( );

	// editor speed max
	SyncMask ( 1 );
	SyncRate ( 0 );

	//PE: release mouse so all monitors can be used.
	ClipCursor(NULL);

	// restore mouse pos and visbility
	game_showmouse ( );

	// prompt informing user we are saving the level changes
	if ( t.conkit.modified == 1 ) 
	{
		popup_text("Saving level changes");
	}

	//  show all waypoints and zones
	waypoint_restore ( );

	// 101115 - restore all characters to use regular character shader
	game_setup_character_shader_entities ( false );

	// if additional entities added, remove and restore orig count
	if ( g.entityelementlist>t.storedentityelementlist ) 
	{
		for ( t.e = t.storedentityelementlist+1 ; t.e<= g.entityelementlist ; t.e++ )
		{
			t.obj=t.entityelement[t.e].obj;
			if ( t.obj>0 ) 
			{
				if ( ObjectExist(t.obj) == 1 ) 
				{
					DeleteObject ( t.obj );
				}
			}
			t.entityelement[t.e].obj=0;
			t.entityelement[t.e].bankindex=0;
		}
		g.entityelementlist=t.storedentityelementlist;
		g.entityviewcurrentobj=t.storedentityviewcurrentobj;
	}

	// in addition, remove any that where spawned inside the original list
	bool bClearAllInGameSpawns = true;
	if ( bClearAllInGameSpawns == true )
	{
		for (t.e = 1; t.e <= g.entityelementlist; t.e++)
		{
			if (t.entityelement[t.e].iWasSpawnedInGame > 0)
			{
				t.obj = t.entityelement[t.e].obj;
				if (t.obj > 0)
				{
					if (ObjectExist(t.obj) == 1)
					{
						DeleteObject (t.obj);
					}
				}
				t.entityelement[t.e].obj = 0;
				t.entityelement[t.e].bankindex = 0;
				t.entityelement[t.e].iWasSpawnedInGame = 0;
			}
		}
	}

	// restore all editor entity positions and rotations
	for ( t.e = 1 ; t.e <= g.entityelementlist; t.e++ )
	{
		t.obj=t.entityelement[t.e].obj;
		if ( t.obj>0 ) 
		{
			if ( ObjectExist(t.obj) == 1 ) 
			{
				// ensure particles can be restored (realtime changes)
				t.storedentityelement[t.e].eleprof.newparticle.fParticle_Speed_Original = t.entityelement[t.e].eleprof.newparticle.fParticle_Speed_Original;
				t.storedentityelement[t.e].eleprof.newparticle.fParticle_Opacity_Original = t.entityelement[t.e].eleprof.newparticle.fParticle_Opacity_Original;
				t.storedentityelement[t.e].eleprof.newparticle.bParticle_Size_Original = t.entityelement[t.e].eleprof.newparticle.bParticle_Size_Original;
				t.storedentityelement[t.e].eleprof.newparticle.fParticle_Floor_Height_Original = t.entityelement[t.e].eleprof.newparticle.fParticle_Floor_Height_Original;
				t.storedentityelement[t.e].eleprof.newparticle.fParticle_Bounciness_Original = t.entityelement[t.e].eleprof.newparticle.fParticle_Bounciness_Original;
				t.storedentityelement[t.e].eleprof.newparticle.fParticle_R_Original = t.entityelement[t.e].eleprof.newparticle.fParticle_R_Original;
				t.storedentityelement[t.e].eleprof.newparticle.fParticle_G_Original = t.entityelement[t.e].eleprof.newparticle.fParticle_G_Original;
				t.storedentityelement[t.e].eleprof.newparticle.fParticle_B_Original = t.entityelement[t.e].eleprof.newparticle.fParticle_B_Original;

				// only if still exists - could have been deleted
				t.entityelement[t.e] = t.storedentityelement[t.e];

				// and if particle, restore the real-time changes (emitter rot and pos)
				entity_updateparticleemitter(t.e);

				// and then wipe those stored settings
				t.entityelement[t.e].eleprof.newparticle.fParticle_Speed_Original = -123.0f;
				t.entityelement[t.e].eleprof.newparticle.fParticle_Opacity_Original = -123.0f;
				t.entityelement[t.e].eleprof.newparticle.bParticle_Size_Original = -123.0f;
				t.entityelement[t.e].eleprof.newparticle.fParticle_Floor_Height_Original = -123.0f;
				t.entityelement[t.e].eleprof.newparticle.fParticle_Bounciness_Original = -123.0f;
				t.entityelement[t.e].eleprof.newparticle.fParticle_R_Original = -123.0f;
				t.entityelement[t.e].eleprof.newparticle.fParticle_G_Original = -123.0f;
				t.entityelement[t.e].eleprof.newparticle.fParticle_B_Original = -123.0f;
			}
		}
	}
	UnDim ( t.storedentityelement );

	// restore entity positions and rotations
	for ( t.e = 1 ; t.e <= g.entityelementlist; t.e++ )
	{
		t.entid=t.entityelement[t.e].bankindex;
		t.obj=t.entityelement[t.e].obj;
		if ( t.obj>0 ) 
		{
			if ( ObjectExist(t.obj) == 1 ) 
			{
				//PE: Also restore moved particles.
				if ( t.entityprofile[t.entid].ismarker == 0  || t.entityprofile[t.entid].ismarker == 10 )
				{
					// reset entity
					PositionObject (  t.obj,t.entityelement[t.e].x,t.entityelement[t.e].y,t.entityelement[t.e].z );
					RotateObject (  t.obj,t.entityelement[t.e].rx,t.entityelement[t.e].ry,t.entityelement[t.e].rz );
					ScaleObject(t.obj, 100 + t.entityelement[t.e].scalex, 100 + t.entityelement[t.e].scaley, 100 + t.entityelement[t.e].scalez);
					ShowObject (  t.obj );

					//PE: Sometimes NPC's face the wrong way after they have been ragdoll.
					if (t.entityprofile[t.entid].ischaracter == 1)
					{
						sObject* pObject = GetObjectData(t.obj);
						void entity_resetlimbtwists(sObject * pObject, int e);
						entity_resetlimbtwists(pObject, t.e);
						RotateLimb(t.obj, 0, 0, 0, 0);

						entity_calculateeuleryfromquat(t.e); //PE: Always get correct rotation.
						RotateObject(t.obj, t.entityelement[t.e].rx, t.entityelement[t.e].ry, t.entityelement[t.e].rz);

						//PE: Ragdoll can remove pivot , so reset if not set. (NPCs would face the wrong way)
						if (!pObject->position.bApplyPivot)
						{
							RotateObject(t.obj, 0, 180, 0);
							FixObjectPivot(t.obj);
							//PE: Need to rotate again for pivot to kick in.
							RotateObject(t.obj, t.entityelement[t.e].rx, t.entityelement[t.e].ry, t.entityelement[t.e].rz);
						}
					}
					
					// restore zdepth mode of this entity
					entity_preparedepth(t.entid, t.obj);
				}
				if ( t.entityprofile[t.entid].addhandlelimb>0 ) 
				{
					ShowLimb ( t.obj,t.entityprofile[t.entid].addhandlelimb );
				}
			}
		}
	}

	// restore new particles that may have been deleted
	for (t.e = 1; t.e <= g.entityelementlist; t.e++)
	{
		int iParticleEmitter = t.entityelement[t.e].eleprof.newparticle.emitterid;
		if (iParticleEmitter != -1)
		{
			if (t.entityelement[t.e].eleprof.newparticle.bParticle_Preview == true)
				gpup_emitterActive(iParticleEmitter, 1);
			else
				gpup_emitterActive(iParticleEmitter, 0);
		}
	}

	// show all markers
	t.gridentityhidemarkers=0;
	editor_updatemarkervisibility ( );

	// ensure all locked entity transparency resolves
	for ( t.tte = 1 ; t.tte<=  g.entityelementlist; t.tte++ )
	{
		if ( t.entityelement[t.tte].editorlock == 1 || t.entityelement[t.tte].underground == 1 ) 
		{
			t.tobj=t.entityelement[t.tte].obj;
			if ( t.tobj>0 ) 
			{
				if ( ObjectExist(t.tobj) == 1 ) 
				{
					//PE: Re-enable transparent on locked entities.
					if(t.entityelement[t.tte].editorlock == 1)
					{
						t.entityelement[t.tte].isclone = 0;
						entity_converttoclonetransparent();
					}
					else 
					{
						if (t.entityelement[t.tte].underground == 1) t.entityelement[t.tte].isclone = 1;
						entity_converttoinstance();
					}
				}
			}
		}
	}

	//PE: disable any animations that should not be in editor.
	for (t.tte = 1; t.tte <= g.entityelementlist; t.tte++)
	{
		// hide EBE markers
		int iIndex = t.entityelement[t.tte].bankindex;
		if (t.entityprofile[iIndex].isebe != 0)
		{
			t.tobj = t.entityelement[t.tte].obj;
			if (t.tobj > 0)
			{
				if (ObjectExist(t.tobj) == 1)
				{
					ShowLimb(t.tobj, 0);
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
				//PE: Make sure we reset all animations. mainly for lua controlled objects like doors
				if (t.entityprofile[t.entid].animmax > 0)
				{
					SetObjectFrame(t.tttsourceobj, 0);
					StopObject(t.tttsourceobj);
					SetObjectFrame(t.tobj, 0);
					StopObject(t.tobj);

					//LB: also need to trigger Wicked to update the frame at least once
					sObject* pParentObj = GetObjectData(t.tttsourceobj);
					sObject* pInstanceObj = GetObjectData(t.tobj);
					WickedCall_SetObjectFrame(pParentObj, 0);
					WickedCall_SetObjectFrame(pInstanceObj, 0);
				}
				if (t.entityprofile[t.entid].startanimingame > 0) 
				{
					if (t.entityprofile[t.entid].animmax > 0) 
					{
						t.q = 0;
						SetObjectFrame(t.tttsourceobj, 0);
						StopObject(t.tttsourceobj);
						SetObjectFrame(t.tobj, 0);
						StopObject(t.tobj);
					}
				}
			}
		}

		//PE: pframe is lost on clone objects, recreate.
		if (t.entityprofile[t.entid].ismarker == 0 && t.entityprofile[t.entid].isebe == 0)
		{
			if (t.entityelement[t.tte].isclone == 1 && t.entityelement[t.tte].underground == 0)
			{
				if (t.entityelement[t.tte].editorlock == 0)
				{
					entity_converttoinstance();
				}
			}
		}

		if (t.tobj > 0 && t.entityprofile[t.entid].ischaracter == 1) 
		{
			//Restore any character animations for editor.
			if (GetNumberOfFrames(t.tobj) > 0)
			{
				SetObjectFrame(t.tobj, 0);
				if (t.entityprofile[t.entid].animmax > 0 && t.entityprofile[t.entid].playanimineditor > 0 && t.entityprofile[t.entid].ischaractercreator == 0)
				{
					t.q = t.entityprofile[t.entid].playanimineditor - 1;
					LoopObject(t.tobj, t.entityanim[t.entid][t.q].start, t.entityanim[t.entid][t.q].finish);
				}
				else if (t.entityprofile[t.entid].playanimineditor < 0)
				{
					// uses name instead of index, the negative is the ordinal into the animset
					extern void entity_loop_using_negative_playanimineditor(int e, int obj, cstr animname);
					entity_loop_using_negative_playanimineditor(t.tte, t.tobj, t.entityprofile[t.entid].playanimineditor_name);
				}
				else
				{
					LoopObject(t.tobj); StopObject(t.tobj);
				}
			}
		}
		else 
		{
			//PE: Restore any non char animations.
			if (t.tobj > 0 && GetNumberOfFrames(t.tobj) > 0)
			{
				SetObjectFrame(t.tobj, 0);
				if (t.entityprofile[t.entid].animmax > 0 && t.entityprofile[t.entid].playanimineditor > 0 && t.entityprofile[t.entid].ischaractercreator == 0)
				{
					t.q = t.entityprofile[t.entid].playanimineditor - 1;
					LoopObject(t.tobj, t.entityanim[t.entid][t.q].start, t.entityanim[t.entid][t.q].finish);
				}
				else if (t.entityprofile[t.entid].playanimineditor < 0)
				{
					// uses name instead of index, the negative is the ordinal into the animset
					extern void entity_loop_using_negative_playanimineditor(int e, int obj, cstr animname);
					entity_loop_using_negative_playanimineditor(t.tte, t.tobj, t.entityprofile[t.entid].playanimineditor_name);
				}
				else
				{
					LoopObject(t.tobj); StopObject(t.tobj);
				}
			}
		}

		// ensure any pivot influences are also restored
		if (t.tobj > 0)
		{
			sObject* pObject = g_ObjectList[t.tobj];
			if (pObject)
			{
				if (t.entityprofile[t.entid].fixnewy != 0)
				{
					RotateObject (t.tobj, 0, t.entityprofile[t.entid].fixnewy, 0);
					FixObjectPivot (t.tobj);
					CalculateObjectWorld (pObject, NULL);
					WickedCall_UpdateObject(pObject);
				}
			}
		}
	}

	// signal that we have finished Test Level, restore mapeditor windows
	 OpenFileMap (  1, "FPSEXCHANGE" );
	 SetFileMapDWORD (  1, 970, 1 );
	 SetEventAndWait (  1 );

	// Restore camera
	editor_restoreeditcamera ( );
	t.updatezoom=1;

	// restore object visibilities
	editor_refresheditmarkers ( );

	// 130320 - ensure water height change is not saved out to root (messes up when reload software; underwater)
	float fStoreWaterLevel = g.gdefaultwaterheight;
	g.gdefaultwaterheight = -500.0f; //GGORIGIN_Y;

	// LUA may have changed fog, restore it
	t.visuals.FogNearest_f = t.gamevisuals.FogNearest_f;
	t.visuals.FogDistance_f = t.gamevisuals.FogDistance_f;
	t.visuals.FogR_f = t.gamevisuals.FogR_f;
	t.visuals.FogG_f = t.gamevisuals.FogG_f;
	t.visuals.FogB_f = t.gamevisuals.FogB_f;
	t.visuals.FogA_f = t.gamevisuals.FogA_f;

	// remember game states for next time
	visuals_save ( );

	t.gamevisuals=t.visuals;

	// and restore as would otherwise interfere with ?
	g.gdefaultwaterheight = fStoreWaterLevel;

	// restore shader constants with editor visuals (and bring back some settings we want to retain)
	t.visuals=t.editorvisuals;
	t.visuals.skyindex=t.gamevisuals.skyindex;
	t.visuals.sky_s=t.gamevisuals.sky_s;
	t.visuals.terrainindex=t.gamevisuals.terrainindex;
	t.visuals.terrain_s=t.gamevisuals.terrain_s;
	t.visuals.vegetationindex=t.gamevisuals.vegetationindex;
	t.visuals.vegetation_s=t.gamevisuals.vegetation_s;
	t.visuals.iEnvironmentWeather = t.gamevisuals.iEnvironmentWeather;

	t.visuals.AmbienceRed_f = t.gamevisuals.AmbienceRed_f;
	t.visuals.AmbienceGreen_f = t.gamevisuals.AmbienceGreen_f;
	t.visuals.AmbienceBlue_f = t.gamevisuals.AmbienceBlue_f;

	t.visuals.FogR_f = t.gamevisuals.FogR_f;
	t.visuals.FogG_f = t.gamevisuals.FogG_f;
	t.visuals.FogB_f = t.gamevisuals.FogB_f;
	t.visuals.FogA_f = t.gamevisuals.FogA_f;
	t.visuals.FogNearest_f = t.gamevisuals.FogNearest_f;
	t.visuals.FogDistance_f = t.gamevisuals.FogDistance_f;

	t.visuals.SkyIntensity_f = t.gamevisuals.SkyIntensity_f;
	t.visuals.SunIntensity_f = t.gamevisuals.SunIntensity_f;
	t.visuals.SunRed_f = t.gamevisuals.SunRed_f;
	t.visuals.SunGreen_f = t.gamevisuals.SunGreen_f;
	t.visuals.SunBlue_f = t.gamevisuals.SunBlue_f;
	t.visuals.ZenithRed_f = t.gamevisuals.ZenithRed_f;
	t.visuals.ZenithGreen_f = t.gamevisuals.ZenithGreen_f;
	t.visuals.ZenithBlue_f = t.gamevisuals.ZenithBlue_f;
	t.visuals.fSunShadowBias = t.gamevisuals.fSunShadowBias;
	t.visuals.bColorGrading = t.gamevisuals.bColorGrading;
	t.visuals.ColorGradingLUT = t.gamevisuals.ColorGradingLUT;
	t.visuals.bBloomEnabled = t.gamevisuals.bBloomEnabled;
	t.visuals.bLevelVSyncEnabled = t.gamevisuals.bLevelVSyncEnabled;
	t.visuals.bOcclusionCulling = t.gamevisuals.bOcclusionCulling;

	t.visuals.fEnvProbeBrightness = t.gamevisuals.fEnvProbeBrightness;

	t.visuals.bEnableTerrainChunkCulling = t.gamevisuals.bEnableTerrainChunkCulling;
	t.visuals.bEnablePointShadowCulling = t.gamevisuals.bEnablePointShadowCulling;
	t.visuals.bEnableSpotShadowCulling = t.gamevisuals.bEnableSpotShadowCulling;
	t.visuals.bEnableObjectCulling = t.gamevisuals.bEnableObjectCulling;
	t.visuals.bEnableAnimationCulling = t.gamevisuals.bEnableAnimationCulling;
	t.visuals.fLODMultiplier = t.gamevisuals.fLODMultiplier;

	t.visuals.bEnable30FpsAnimations = t.gamevisuals.bEnable30FpsAnimations;

	t.visuals.bShadowsLowestLOD = t.gamevisuals.bShadowsLowestLOD;
	t.visuals.bProbesLowestLOD = t.gamevisuals.bProbesLowestLOD;
	t.visuals.bRaycastLowestLOD = t.gamevisuals.bRaycastLowestLOD;
	t.visuals.bPhysicsLowestLOD = t.gamevisuals.bPhysicsLowestLOD;
	t.visuals.bThreadedPhysics = t.gamevisuals.bThreadedPhysics;
	t.visuals.bReflectionsLowestLOD = t.gamevisuals.bReflectionsLowestLOD;

	
	t.visuals.g_bDelayedShadows = t.gamevisuals.g_bDelayedShadows;
	t.visuals.g_bDelayedShadowsLaptop = t.gamevisuals.g_bDelayedShadowsLaptop;


	t.visuals.fsetBloomThreshold = t.gamevisuals.fsetBloomThreshold;
	t.visuals.ApparentSize = t.gamevisuals.ApparentSize;
	t.visuals.bSSREnabled = t.gamevisuals.bSSREnabled;
	t.visuals.bReflectionsEnabled = t.gamevisuals.bReflectionsEnabled;
	t.visuals.bFXAAEnabled = t.gamevisuals.bFXAAEnabled;

	t.visuals.bDOF = t.gamevisuals.bDOF;
	t.visuals.fDOFStrength = t.gamevisuals.fDOFStrength;
	t.visuals.fDOFApertureSize = t.gamevisuals.fDOFApertureSize;
	t.visuals.fDOFFocalLength = t.gamevisuals.fDOFFocalLength;


	t.visuals.bTessellation = t.gamevisuals.bTessellation;
	t.visuals.bLightShafts = t.gamevisuals.bLightShafts;
	t.visuals.bLensFlare = t.gamevisuals.bLensFlare;
	t.visuals.bAutoExposure = t.gamevisuals.bAutoExposure;
	t.visuals.fAutoExposureRate = t.gamevisuals.fAutoExposureRate;
	t.visuals.fAutoExposureKey = t.gamevisuals.fAutoExposureKey;
	t.visuals.fExposure = t.gamevisuals.fExposure;
	t.visuals.fGamma = t.gamevisuals.fGamma;
	t.visuals.fDeSaturate = t.gamevisuals.fDeSaturate;

	t.visuals.SkyCloudiness = t.gamevisuals.SkyCloudiness;
	t.visuals.SkyCloudCoverage = t.gamevisuals.SkyCloudCoverage;
	t.visuals.SkyCloudHeight = t.gamevisuals.SkyCloudHeight;
	t.visuals.SkyCloudThickness = t.gamevisuals.SkyCloudThickness;
	t.visuals.SkyCloudSpeed = t.gamevisuals.SkyCloudSpeed;

	t.visuals.iMSAASampleCount = t.gamevisuals.iMSAASampleCount;
	t.visuals.iFSRMode = t.gamevisuals.iFSRMode;
	t.visuals.fFSRSharpness = t.gamevisuals.fFSRSharpness;

	t.visuals.iMSAO = t.gamevisuals.iMSAO;
	t.visuals.fMSAOPower = t.gamevisuals.fMSAOPower;

	//PE: restore SetGlobalGraphicsSettings here. unless changed with tab tab, then they are changed in gamevisuals
	//PE: Postprocess already restored.
	t.visuals.iShadowSpotCascadeResolution = t.gamevisuals.iShadowSpotCascadeResolution;
	t.visuals.iShadowSpotResolution = t.gamevisuals.iShadowSpotResolution;
	t.visuals.iShadowPointResolution = t.gamevisuals.iShadowPointResolution;
	t.visuals.iShadowPointMax = t.gamevisuals.iShadowPointMax;
	t.visuals.iShadowSpotMax = t.gamevisuals.iShadowSpotMax;
	t.visuals.bTransparentShadows = t.gamevisuals.bTransparentShadows;

	t.visuals.iEnvProbeResolution = t.gamevisuals.iEnvProbeResolution;
	t.visuals.newperformancepresets = t.gamevisuals.newperformancepresets;

	t.visuals.fShadowFarPlane = t.gamevisuals.fShadowFarPlane;

	t.visuals.bWaterEnable = t.gamevisuals.bWaterEnable;
	t.visuals.fWaterWaveAmplitude = t.gamevisuals.fWaterWaveAmplitude;
	t.visuals.fWaterPatchLength = t.gamevisuals.fWaterPatchLength;
	t.visuals.fWaterChoppyScale = t.gamevisuals.fWaterChoppyScale;
	t.visuals.fWaterWindDependency = t.gamevisuals.fWaterWindDependency;

	t.visuals.WaterFogMinDist = t.gamevisuals.WaterFogMinDist;
	t.visuals.WaterFogMaxDist = t.gamevisuals.WaterFogMaxDist;
	t.visuals.WaterFogMinAmount = t.gamevisuals.WaterFogMinAmount;
	
	//PE: Water color was missing.
	t.visuals.WaterRed_f = t.gamevisuals.WaterRed_f;
	t.visuals.WaterGreen_f = t.gamevisuals.WaterGreen_f;
	t.visuals.WaterBlue_f = t.gamevisuals.WaterBlue_f;
	t.visuals.WaterAlpha_f = t.gamevisuals.WaterAlpha_f;

	t.visuals.iTimeOfday = t.gamevisuals.iTimeOfday;
	t.visuals.SunAngleX = t.gamevisuals.SunAngleX;
	t.visuals.SunAngleY = t.gamevisuals.SunAngleY;
	t.visuals.SunAngleZ = t.gamevisuals.SunAngleZ;

	t.visuals.bSimulate24Hours = t.gamevisuals.bSimulate24Hours;
	t.visuals.fTimeSpeed = t.gamevisuals.fTimeSpeed;

	t.visuals.fWeatherIntensity = t.gamevisuals.fWeatherIntensity;
	t.visuals.fWeatherLighting = t.gamevisuals.fWeatherLighting;
	t.visuals.fWeatherThunder = t.gamevisuals.fWeatherThunder;
	t.visuals.fWeatherWind = t.gamevisuals.fWeatherWind;
	
	t.visuals.bPPSnow = t.gamevisuals.bPPSnow;
	t.visuals.voxel_steps = t.gamevisuals.voxel_steps;
	t.visuals.pp_size = t.gamevisuals.pp_size;
	t.visuals.pp_alpha = t.gamevisuals.pp_alpha;
	t.visuals.wind_direction_x = t.gamevisuals.wind_direction_x;
	t.visuals.wind_direction_y = t.gamevisuals.wind_direction_y;
	t.visuals.wind_direction_z = t.gamevisuals.wind_direction_z;
	t.visuals.wind_speed = t.gamevisuals.wind_speed;
	t.visuals.wind_randomness = t.gamevisuals.wind_randomness;
	t.visuals.tree_wind = t.gamevisuals.tree_wind;
	t.visuals.tree_sss = t.gamevisuals.tree_sss;


	t.visuals.fLevelDifficulty = t.gamevisuals.fLevelDifficulty;
	
	gggrass_global_params = gggrass_save_params;

	for (int iL = 0; iL < 32; iL++) 
	{
		t.visuals.sTerrainTextures[iL] = t.gamevisuals.sTerrainTextures[iL];
		t.visuals.sTerrainTexturesName[iL] = t.gamevisuals.sTerrainTexturesName[iL];
	}
	
	for (int iL = 0; iL < 128; iL++) 
	{
		t.visuals.sGrassTextures[iL] = t.gamevisuals.sGrassTextures[iL];
		t.visuals.sGrassTexturesName[iL] = t.gamevisuals.sGrassTexturesName[iL];
		t.visuals.sFactionName[iL] = t.gamevisuals.sFactionName[iL];
	}

	t.visuals.bEndableAmbientMusicTrack = t.gamevisuals.bEndableAmbientMusicTrack;
	t.visuals.sAmbientMusicTrack = t.gamevisuals.sAmbientMusicTrack;
	t.visuals.iAmbientMusicTrackVolume = t.gamevisuals.iAmbientMusicTrackVolume;
	t.visuals.bEnableCombatMusicTrack = t.gamevisuals.bEnableCombatMusicTrack;
	t.visuals.sCombatMusicTrack = t.gamevisuals.sCombatMusicTrack;
	t.visuals.iCombatMusicTrackVolume = t.gamevisuals.iCombatMusicTrackVolume;

	t.visuals.bEndableTreeDrawing = t.gamevisuals.bEndableTreeDrawing;
	t.visuals.bEndableGrassDrawing = t.gamevisuals.bEndableGrassDrawing;
	t.visuals.bEndableTerrainDrawing = t.gamevisuals.bEndableTerrainDrawing;
	t.visuals.bEnableEmptyLevelMode = t.gamevisuals.bEnableEmptyLevelMode;
	t.visuals.bEnableZeroNavMeshMode = t.gamevisuals.bEnableZeroNavMeshMode;

	t.visuals.iHeightmapWidth = t.gamevisuals.iHeightmapWidth;
	t.visuals.iHeightmapHeight = t.gamevisuals.iHeightmapHeight;

	t.visuals.bDisableSkybox = t.gamevisuals.bDisableSkybox;

	t.visuals.bRainEnabled = t.gamevisuals.bRainEnabled;
	t.visuals.fRainSpeedX = t.gamevisuals.fRainSpeedX;
	t.visuals.fRainSpeedY = t.gamevisuals.fRainSpeedY;
	t.visuals.fRainOpacity = t.gamevisuals.fRainOpacity;
	t.visuals.fRainScaleX = t.gamevisuals.fRainScaleX;
	t.visuals.fRainScaleY = t.gamevisuals.fRainScaleY;
	t.visuals.fRainRefreactionScale = t.gamevisuals.fRainRefreactionScale;


	t.visuals.bSnowEnabled = t.gamevisuals.bSnowEnabled;
	t.visuals.fSnowLayers = t.gamevisuals.fSnowLayers;
	t.visuals.fSnowDepth = t.gamevisuals.fSnowDepth;
	t.visuals.fSnowWind = t.gamevisuals.fSnowWind;
	t.visuals.fSnowSpeed = t.gamevisuals.fSnowSpeed;
	t.visuals.fSnowOpacity = t.gamevisuals.fSnowOpacity;
	t.visuals.fSnowOffset = t.gamevisuals.fSnowOffset;

	// and refresh assets based on restore
	t.visuals.refreshshaders=1;
	visuals_loop ( );
	visuals_shaderlevels_update ( );

	// use infinilights to show dynamic lighting in editor
	lighting_init ( );

	// Second call will toggle keyboard/mouse back to FOREGROUND
	SetWindowModeOn ( );

	//PE: Need to restore original settings.
	//PE: Setup the window here. pos size. Docking ?
	SetWindowSettings(5, 1, 1);
	SetForegroundWindow(g_pGlob->hWnd);
	SetWindowSize(gWindowSizeXOld+ gWindowSizeAddX, gWindowSizeYOld+ gWindowSizeAddY); //PE: test
	SetWindowPosition(gWindowPosXOld, gWindowPosYOld);
	ShowWindow();
	if (gWindowMaximized == 1 )
		MaximiseWindow();
	else
		RestoreWindow();

	//PE: enable outside windows again.
	ImGui::ShowAllViewPortWindows();

	// Close popup message
	if ( t.conkit.modified == 1 ) 
	{
		SleepNow ( 1000 );
		popup_text_close();
		t.conkit.modified=0;
	}

	// Ensure no terrain/entity editing carried back
	t.terrain.terrainpainteroneshot=0;

	// Set editor to use a true 1;1 pixel mapping for Text ( , Steam GUI and other overlay images )
	SetChildWindowTruePixel ( 1 );

	// restore if project modified
	t.tignoreinvalidateobstacles=1;
	g.projectmodified = t.storeprojectmodified;
	t.tignoreinvalidateobstacles=0;

	// Something is clipping objects when returning to editor
	editor_loadcfg();
	editor_refreshcamerarange();
}

void editor_previewmapormultiplayer(int iUseVRTest)
{
	// for non-MAX scenarios (single function call)
	bool bRunLoop = true;
	editor_previewmapormultiplayer_initcode(iUseVRTest);
	while ( bRunLoop == true)
	{
		if (editor_previewmapormultiplayer_loopcode(iUseVRTest) == true) bRunLoop = false;
	}
	editor_previewmapormultiplayer_afterloopcode(iUseVRTest);
	t.postprocessings.fadeinvalue_f = 1.0f;
}

void editor_multiplayermode ( void )
{
	// check we are not in the importer or character creator
	editor_checkIfInSubApp ( );

	//  Record last edited project
	t.storeprojectfilename_s=g.projectfilename_s;

	//  Set multiplayer flags here
	t.game.runasmultiplayer=1;
	editor_previewmapormultiplayer ( 1 );

	// PE: I cant restore editor after multiplayer mode ? SO:
	// call a new map editor
	OpenFileMap(2, "FPSEXCHANGE");
	SetFileMapString(2, 1000, "Guru-MapEditor.exe");
	SetFileMapString(2, 1256, "-r");
	SetFileMapDWORD(2, 994, 0);
	SetFileMapDWORD(2, 924, 1);
	SetEventAndWait(2);
	// Terminate fragmented EXE
	common_justbeforeend();
	ExitProcess(0);

	// As multiplayer can load OTHER things, restore level to state before we clicked MM button
	t.tfile_s=g.mysystem.editorsGridedit_s+"cfg.cfg";//"editors\\gridedit\\cfg.cfg";
	if ( FileExist(t.tfile_s.Get()) == 1 ) 
	{
		timestampactivity(0,"reloading your level after MM button");
		t.skipfpmloading=0;
		g.projectfilename_s=g.mysystem.editorsGrideditAbs_s+"worklevel.fpm";//g.fpscrootdir_s+"\\Files\\editors\\gridedit\\worklevel.fpm";
		editor_loadcfg ( );
		gridedit_load_map ( );
		bUpdateVeg = true;

		//  added to solve fog issue when go in and out of MP menu
		visuals_editordefaults ( );
		t.visuals.refreshshaders=1;
	}

	//  restore last edited project
	g.projectfilename_s=t.storeprojectfilename_s;
	gridedit_updateprojectname ( );

	//editor_restoreeditcamera();
	//SyncMaskOverride(0xFFFFFFFF);

}

void editor_previewmap ( int iUseVRTest )
{
	//  check if we are in the importer or character creator, if we are, don't test ma
	editor_checkIfInSubApp ( );
	//  Set single player test game flags here
	t.game.runasmultiplayer=0;
	editor_previewmapormultiplayer ( iUseVRTest );
}

void editor_previewmap_initcode(int iUseVRTest)
{
	editor_previewmapormultiplayer_initcode ( iUseVRTest );
}

bool editor_previewmap_loopcode(int iUseVRTest)
{
	// loop ended
	return editor_previewmapormultiplayer_loopcode ( iUseVRTest );
}

void editor_previewmap_afterloopcode(int iUseVRTest)
{
	editor_previewmapormultiplayer_afterloopcode ( iUseVRTest );
}

void editor_handlepguppgdn ( void )
{
	// changes and returns 't.tupdownstepvalue_f'
	bool bWidgetMove = false;
	if (t.inputsys.kscancode == 201 || t.inputsys.kscancode == 209)
	{
		float fEntityStepSize = 5.0f;
		if (t.gridentity > 0 && t.gridentityobj > 0)
		{
			if (ObjectExist(t.gridentityobj) == 1)
				fEntityStepSize = ObjectSizeY(t.gridentityobj, 1);
		}
		else
		{
			if (t.widget.pickedEntityIndex > 0)
			{
				int wobj = t.entityelement[t.widget.pickedEntityIndex].obj;
				if (t.widget.activeObject > 0)
					wobj = t.widget.activeObject;

				if (ObjectExist(wobj) == 1)
					fEntityStepSize = ObjectSizeY(wobj, 1);

				//Make sure to highlight all objects the object belong to.
				if (t.widget.pickedEntityIndex > 0)
					CheckGroupListForRubberbandSelections(t.widget.pickedEntityIndex);
				bWidgetMove = true;
			}
		}
		if (t.gridentitygridlock >= 1)
			t.tupdownstepvalue_f = 0.0;
		else
			t.tupdownstepvalue_f = 1.0;
		if (t.gridentitygridlock > 0)
		{
			if (t.inputsys.keypressallowshift == 0 && (t.inputsys.kscancode == 201 || t.inputsys.kscancode == 209))
			{
				if (t.gridentitygridlock == 1)
					t.tupdownstepvalue_f = fEntityStepSize;
				if (t.gridentitygridlock == 2)
					t.tupdownstepvalue_f = pref.fEditorGridSizeY;
				t.inputsys.keypressallowshift = 1;
			}
			else
			{
				t.tupdownstepvalue_f = 0;
			}
		}
		if (bWidgetMove)
		{
			if (t.tupdownstepvalue_f != 0.0f && t.widget.pickedEntityIndex > 0 && t.entityelement[t.widget.pickedEntityIndex].editorlock == 0)
			{
				bool bDisableRubberBandMoving = false;
				if (current_selected_group >= 0 && group_editing_on)
				{
					bDisableRubberBandMoving = true;
				}
				if (!bDisableRubberBandMoving)
				{
					if (t.inputsys.kscancode == 201)  t.entityelement[t.widget.pickedEntityIndex].y += t.tupdownstepvalue_f;
					if (t.inputsys.kscancode == 209)  t.entityelement[t.widget.pickedEntityIndex].y -= t.tupdownstepvalue_f;
					int wobj = t.entityelement[t.widget.pickedEntityIndex].obj;
					if (t.widget.activeObject > 0)
						wobj = t.widget.activeObject;

					PositionObject(wobj, t.entityelement[t.widget.pickedEntityIndex].x, t.entityelement[t.widget.pickedEntityIndex].y, t.entityelement[t.widget.pickedEntityIndex].z);

					// if we need to also move rubber band highlighted objects, do so now
					if (g.entityrubberbandlist.size() > 0)
					{
						for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
						{
							int e = g.entityrubberbandlist[i].e;
							int tobj = t.entityelement[e].obj;
							if (e != t.widget.pickedEntityIndex && tobj > 0 && t.entityelement[e].editorlock == 0)
							{
								if (ObjectExist(tobj) == 1)
								{
									if (tobj != wobj)
									{
										if (t.inputsys.kscancode == 201)  t.entityelement[e].y += t.tupdownstepvalue_f;
										if (t.inputsys.kscancode == 209)  t.entityelement[e].y -= t.tupdownstepvalue_f;
										if (t.inputsys.kscancode == 201) g.entityrubberbandlist[i].x += t.tupdownstepvalue_f;
										if (t.inputsys.kscancode == 209) g.entityrubberbandlist[i].x -= t.tupdownstepvalue_f;

										PositionObject(tobj, t.entityelement[e].x, t.entityelement[e].y, t.entityelement[e].z);
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
		}
	}
}

int last_xmousemove = 0, last_ymousemove = 0;
void imgui_input_getcontrols(void)
{
	// when setup.ini sets fulldebugview to 1, record all key states pressed
	if (g.globals.fulldebugviewofkeymap == 1)
	{
		int iRawKeyState = ScanCode();
		if (iRawKeyState > 0)
		{
			static int lastinputsyskscancode = 0;
			if (iRawKeyState != lastinputsyskscancode)
			{
				char pKeyMapDebugLog[256];
				sprintf(pKeyMapDebugLog, "Raw Key State: %d", iRawKeyState);
				timestampactivity(0, pKeyMapDebugLog);
			}
			lastinputsyskscancode = iRawKeyState;
		}
	}

	//  Some actions are directly triggered by input subroutine
	t.inputsys.doload = 0;
	t.inputsys.domodeterrain = 0;
	t.inputsys.domodeentity = 0;
	t.inputsys.domodemarker = 0;
	t.inputsys.domodewaypoint = 0;
	t.inputsys.doundo = 0;
	t.inputsys.doredo = 0;
	t.inputsys.tselcontrol = 0;
	t.inputsys.tselcut = 0;
	t.inputsys.tselcopy = 0;
	t.inputsys.tseldelete = 0;

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//PE: Take everything from imgui.
	float itmpmousex = ImGui::GetMousePos().x;
	float itmpmousey = ImGui::GetMousePos().y;
	int iSecureZone = 4;
	RECT winpos = { 0,0,0,0 };

	//PE: Must be relative to windows pos, or nothing work if you have a window placed at the rigth of the screen.
	GetWindowRect(g_pGlob->hWnd, &winpos);

	bool bCanGetInput = bImGuiRenderTargetFocus;
	if (pref.iEnableDragDropEntityMode && bDraggingActive)
	{
		bCanGetInput = true;
	}

	if (bCanGetInput && (itmpmousex + winpos.left) >= (renderTargetAreaPos.x+iSecureZone) && (itmpmousey+winpos.top) >= (renderTargetAreaPos.y + iSecureZone) && (itmpmousex - winpos.left) <= renderTargetAreaPos.x + (renderTargetAreaSize.x - iSecureZone) && (itmpmousey-winpos.top) <= renderTargetAreaPos.y + 30 + (renderTargetAreaSize.y - iSecureZone ))
	{
		t.inputsys.activemouse = 1;
		t.inputsys.xmouse = (int)itmpmousex;
		t.inputsys.ymouse = (int)itmpmousey;
		t.inputsys.zmouse = io.MouseWheel; // MouseZ();
		t.inputsys.xmousemove = t.inputsys.xmouse - xmouseold;
		t.inputsys.ymousemove = t.inputsys.ymouse - ymouseold;

		last_xmousemove = t.inputsys.xmousemove;
		last_ymousemove = t.inputsys.ymousemove;

		xmouseold = t.inputsys.xmouse;
		ymouseold = t.inputsys.ymouse;

		t.inputsys.wheelmousemove = io.MouseWheel; //MouseMoveZ();
		if (ImGui::IsMouseDown(2)) t.inputsys.wheelmousemove = 0;
		set_inputsys_mclick(io.MouseDown[0] + (io.MouseDown[1] * 2.0) + (io.MouseDown[2] * 3.0) + (io.MouseDown[3] * 4.0));// t.inputsys.mclick = io.MouseDown[0] + (io.MouseDown[1] * 2.0) + (io.MouseDown[2] * 3.0) + (io.MouseDown[3] * 4.0); //  MouseClick();
		t.inputsys.k_s = Lower(Inkey());

		//  Control keys direct from keyboard
		t.inputsys.keyreturn = io.KeysDown[13]; // ReturnKey();
		t.inputsys.keyshift = io.KeyShift;
		t.inputsys.keytab = io.KeysDown[0x09]; //TAB!
		t.inputsys.keyleft = io.KeysDown[37]; // LeftKey();
		t.inputsys.keyright = io.KeysDown[39]; //RightKey();
		t.inputsys.keyup = io.KeysDown[38]; //UpKey();
		t.inputsys.keydown = io.KeysDown[40]; // DownKey();
		t.inputsys.keycontrol = io.KeyCtrl; //ControlKey();
		t.inputsys.keyspace = io.KeysDown[32]; //SpaceKey();

		//PE: We need raw scancodes. just take it from imgui.
		t.inputsys.kscancode = 0;
		for (int iTemp = 0; iTemp < 256; iTemp++)
		{
			if (iTemp != 16 && iTemp != 17 && iTemp != 18) 
			{ 
				//shift,control (added 18, not sure what it is)
				if (io.KeysDown[iTemp] > 0)
				{
					t.inputsys.kscancode = iTemp;
					break;
				}
			}
		}
	}
	else 
	{
		//No input to DX11.
		t.inputsys.activemouse = 1;
		t.inputsys.zmouse = 0;

		float fMouseDeltaX, fMouseDeltaZ;
		WickedCall_GetMouseDeltas ( &fMouseDeltaX, &fMouseDeltaZ);
		t.inputsys.xmousemove = fMouseDeltaX;
		t.inputsys.ymousemove = fMouseDeltaZ;

		xmouseold = t.inputsys.xmouse;
		ymouseold = t.inputsys.ymouse;

		t.inputsys.wheelmousemove = 0;

		set_inputsys_mclick(0);// t.inputsys.mclick = 0;

		t.inputsys.k_s = "";

		//  Control keys direct from keyboard
		t.inputsys.keyreturn = 0;
		t.inputsys.keyshift = 0;
		t.inputsys.keytab = 0;
		t.inputsys.keyleft = 0;
		t.inputsys.keyright = 0;
		t.inputsys.keyup = 0;
		t.inputsys.keydown = 0;
		t.inputsys.keycontrol = 0;
		t.inputsys.keyspace = 0;
		t.inputsys.kscancode = 0;
	}

	int mcursor = ImGui::GetMouseCursor();

	if (bBuilder_Properties_Window) 
	{
		//Disable some keys.
	}

	if (g_bCharacterCreatorPlusActivated) 
	{
		//Disable some keys.
		if( t.inputsys.kscancode == Asc("t") || t.inputsys.kscancode == Asc("T") )
			t.inputsys.kscancode = 0;
		if( t.inputsys.k_s == "t" || t.inputsys.k_s == "T" ) 
			t.inputsys.k_s = "";
		if (t.inputsys.kscancode == Asc("p") || t.inputsys.kscancode == Asc("P"))
			t.inputsys.kscancode = 0;
		if (t.inputsys.k_s == "p" || t.inputsys.k_s == "P")
			t.inputsys.k_s = "";

	}
	if (bEntity_Properties_Window) 
	{
		//Disable all key input when in Properties.
		t.inputsys.keyleft = 0;
		t.inputsys.keyright = 0;
		t.inputsys.keyup = 0;
		t.inputsys.keydown = 0;
		t.inputsys.kscancode = 0;
		t.inputsys.k_s = "";
		t.inputsys.keyshift = 0;
		t.inputsys.keytab = 0;
	}

	if (ImGui::IsAnyItemActive() && t.inputsys.mclick != 1 ) 
	{
		//A widget got focus, like textinput , disable all keys.
		t.inputsys.keyleft = 0;
		t.inputsys.keyright = 0;
		t.inputsys.keyup = 0;
		t.inputsys.keydown = 0;
		t.inputsys.kscancode = 0;
		t.inputsys.k_s = "";
		t.inputsys.keyshift = 0;
		t.inputsys.keytab = 0;
	}

	// 060320 - somehow, some laptops set 'bImGuiGotFocus' to true (or mcursor>0), 
	// wiping out click detection in Welcome screen, so added condition to prevent this erasure!
	if ( bImGuiGotFocus || ( mcursor > 0 && mcursor != ImGuiMouseCursor_Hand) )
	{
		//No GG input when using imgui.
		t.inputsys.xmouse = 500000;
		t.inputsys.ymouse = 0;
		t.inputsys.xmousemove = 0;
		t.inputsys.ymousemove = 0;
		set_inputsys_mclick(0);// t.inputsys.mclick = 0;
		t.inputsys.zmouse = 0;
		t.inputsys.wheelmousemove = 0;
		t.inputsys.activemouse = 0;
		t.syncthreetimes = 1;
		t.inputsys.k_s = "";
		//  Control keys direct from keyboard
		t.inputsys.keyreturn = 0;
		t.inputsys.keyshift = 0;
		t.inputsys.keytab = 0;
		t.inputsys.keyleft = 0;
		t.inputsys.keyright = 0;
		t.inputsys.keyup = 0;
		t.inputsys.keydown = 0;
		t.inputsys.keycontrol = 0;
		t.inputsys.keyspace = 0;
		t.inputsys.kscancode = 0;

	}

	input_extramappings();

	//  Flag reset
	t.inputsys.dorotation = 0;
	t.inputsys.domirror = 0;
	t.inputsys.doflip = 0;
	t.inputsys.doentityrotate = 0;
	t.inputsys.dozoomin = 0;
	t.inputsys.dozoomout = 0;
	t.inputsys.doscrollleft = 0;
	t.inputsys.doscrollright = 0;
	t.inputsys.doscrollup = 0;
	t.inputsys.doscrolldown = 0;
	t.inputsys.domapresize = 0;
	t.inputsys.dogroundmode = -1;
	t.inputsys.dozoomview = 0;
	t.inputsys.dozoomviewmovex = 0;
	t.inputsys.dozoomviewmovey = 0;
	t.inputsys.dozoomviewmovez = 0;
	t.inputsys.dozoomviewrotatex = 0;
	t.inputsys.dozoomviewrotatey = 0;
	t.inputsys.dozoomviewrotatez = 0;
	t.inputsys.dosinglelayer = 0;
	t.inputsys.tselfloor = 0;
	t.inputsys.tselpaste = 0;
	t.inputsys.tselwipe = 0;
	t.inputsys.dosaveandrun = 0;

	//PE: Map additional keys.
	if (t.inputsys.kscancode == 32)  t.inputsys.keyspace = 1; else t.inputsys.keyspace = 0;
	if (t.inputsys.kscancode == 0 && t.inputsys.keyshift == 0) t.inputsys.keypressallowshift = 0;

	//  W,A,S,D in editor for scrolling about (easier for user)
	if (t.inputsys.kscancode == 87)
		t.inputsys.keyup = 1;
	if (t.inputsys.kscancode == 65)  t.inputsys.keyleft = 1;
	if (t.inputsys.kscancode == 83)  t.inputsys.keydown = 1;
	if (t.inputsys.kscancode == 68)  t.inputsys.keyright = 1;

	if (t.inputsys.keycontrol == 1)
	{
		if (t.inputsys.k_s == "z" && t.inputsys.undokeypress == 0) 
		{
			bForceUndo = true;
			//t.inputsys.doundo = 1; 
			//t.inputsys.undokeypress = 1;
		}
		else if (t.inputsys.k_s == "y" && t.inputsys.undokeypress == 0) 
		{
			t.inputsys.doredo = 1; 
			t.inputsys.undokeypress = 1;
		}
	}

	//  Convert to DX INPUT CODES
	t.t_s = ""; t.tt = 0;
	switch (t.inputsys.kscancode)
	{
		case 9: t.tt = 15; break;
		case 32: t.tt = 57; break;
		case 33: t.tt = 201; break;
		case 34: t.tt = 209; break;
		case 37: t.tt = 203; break;
		case 38: t.tt = 200; break;
		case 39: t.tt = 205; break;
		case 40: t.tt = 208; break;
		case 42: t.tt = 16; break;
		case 46: t.tt = 211; break;
		case 54: t.tt = 16; break;
		case 112: t.tt = 59; break;
		case 113: t.tt = 60; break;
		case 114: t.tt = 61; break;
		case 115: t.tt = 62; break;
		case 123: t.tt = 88; break;
		case 187: t.tt = 13; break;
		case 188: t.tt = 51; break;
		case 189: t.tt = 12; break;
		case 190: t.tt = 52; break;
		case 192: t.tt = 40; break;
		case 219: t.tt = 26; break;
		case 220: t.tt = 86; break;
		case 221: t.tt = 27; break;
		case 222: t.tt = 43; break;
		case 1001: t.tt = 13; break;
		case 1002: t.tt = 12; break;
	}
	// 031215 - then remap to new scancodes (from keymap)
	t.tt = g.keymap[t.tt];
	// and temp back into IDE key values (for last bit)
	int ttt = 0;
	switch (t.tt)
	{
		case 15: ttt = 9; break;
		case 57: ttt = 32; break;
		case 201: ttt = 33; break;
		case 209: ttt = 34; break;
		case 203: ttt = 37; break;
		case 200: ttt = 38; break;
		case 205: ttt = 39; break;
		case 208: ttt = 40; break;
		case 16: ttt = 42; break;
		case 211: ttt = 46; break;
		case 59: ttt = 112; break;
		case 60: ttt = 113; break;
		case 61: ttt = 114; break;
		case 62: ttt = 115; break;
		case 88: ttt = 123; break;
		case 13: ttt = 187; break;
		case 51: ttt = 188; break;
		case 12: ttt = 189; break;
		case 52: ttt = 190; break;
		case 40: ttt = 192; break;
		case 26: ttt = 219; break;
		case 86: ttt = 220; break;
		case 27: ttt = 221; break;
		case 43: ttt = 222; break;
	}
	// then create proper inkey chars from revised (if any) scancodes
	switch (ttt)
	{
		case 16: t.t_s = "q"; break;
		case 57: t.t_s = " "; break;
		case 107: t.t_s = "="; break;
		case 109: t.t_s = "-"; break;
		case 187: t.t_s = "="; break;
		case 188: t.t_s = ","; break;
		case 189: t.t_s = "-"; break;
		case 190: t.t_s = "."; break;
		case 192: t.t_s = "'"; break;
		case 219: t.t_s = "["; break;
		case 220: t.t_s = "\\"; break;
		case 221: t.t_s = "]"; break;
		case 222: t.t_s = "#"; break;
	}

	if (t.inputsys.kscancode >= Asc("A") && t.inputsys.kscancode <= Asc("Z"))  t.t_s = Lower(Chr(t.inputsys.kscancode));
	if (t.inputsys.kscancode >= Asc("0") && t.inputsys.kscancode <= Asc("9"))  t.t_s = Lower(Chr(t.inputsys.kscancode));
	if (t.t_s != "")  t.tt = 1;

	t.inputsys.k_s = t.t_s; t.inputsys.kscancode = t.tt;

	//  Input conditional flags
	if (t.inputsys.kscancode == 0) 
	{
		t.inputsys.keypress = 0;
		if (iForceScancode > 0) 
		{
			if (iForceScancode == 13)
				t.inputsys.keyreturn = 1;
			t.inputsys.kscancode = iForceScancode;
			iForceScancode = -1;
		}
		else if (bForceKey) 
		{
			bForceKey = false;
			t.inputsys.k_s = csForceKey;
			t.inputsys.keycontrol = 0;
			t.inputsys.keyshift = 0;
			t.inputsys.keytab = 0;
			t.inputsys.kscancode = Asc(csForceKey.Get());
		}
		else if (bForceKey2) 
		{
			bForceKey2 = false;
			t.inputsys.k_s = csForceKey2;
			t.inputsys.keycontrol = 0;
			t.inputsys.keyshift = 0;
			t.inputsys.keytab = 0;
			t.inputsys.kscancode = Asc(csForceKey2.Get());
		}
	}
	if (bForceUndo) 
	{
		t.inputsys.doundo = 1;
		t.inputsys.undokeypress = 1;
		bForceUndo = false;
	}
	if (bForceRedo) 
	{
		t.inputsys.doredo = 1;
		t.inputsys.undokeypress = 1;
		bForceRedo = false;
	}

	//  Construction Keys
	if (t.inputsys.keycontrol == 0)
	{
		// can get marker mode from anywhere
		if ((t.inputsys.kscancode == Asc("M") || t.inputsys.k_s == "m") && t.inputsys.keypress == 0)
		{
			t.inputsys.domodemarker = 1;
			t.inputsys.keypress = 1;
		}

		if ((t.grideditselect == 4 && t.gridentityinzoomview>0) || t.grideditselect == 5)
		{
			if (t.inputsys.k_s == "g" && t.inputsys.keypress == 0)
			{
				t.inputsys.keypress = 1; 
				t.gridentitygridlock = t.gridentitygridlock + 1;
				if (t.gridentitygridlock > 2)
				{
					t.gridentitygridlock = 0;
				}
				pref.iGridMode = t.gridentitygridlock;
			}
			if (t.inputsys.k_s == "y" && t.inputsys.keypress == 0 && g.gentitytogglingoff == 0)
			{
				// only if not EBE
				if (t.entityprofile[t.gridentity].isebe == 0)
				{
					//PE: This dont work, t.gridentity = 0 ?
					t.ttrygridentitystaticmode = 1 - t.gridentitystaticmode;
					t.ttrygridentity = t.gridentity; editor_validatestaticmode();
				}
				t.inputsys.keypress = 1;
			}
			if (t.inputsys.k_s == "u" && t.inputsys.keypress == 0)
			{
				//  control auto-flatten
				t.inputsys.keypress = 1;
				t.gridedit.autoflatten = 1 - t.gridedit.autoflatten;
			}
			if (t.inputsys.k_s == "i" && t.inputsys.keypress == 0)
			{
				//  control entity spray mode
				t.inputsys.keypress = 1;
				t.gridedit.entityspraymode = 1 - t.gridedit.entityspraymode;
			}
			// except when in EBE mode which handles - and + keys for material changing
			if (t.ebe.on == 0)
			{
				if (t.inputsys.k_s == "-" && t.inputsys.keypress == 0) { t.gridentitymodifyelement = 1; t.inputsys.keypress = 1; }
				if (t.inputsys.k_s == "=" && t.inputsys.keypress == 0) { t.gridentitymodifyelement = 2; t.inputsys.keypress = 1; }
			}
		}

		//  editing mode
		if (t.inputsys.k_s == "t")
		{
			if (!pref.iEnableSingleRightPanelAdvanced)
			{
				Logic_Settings_Window = false;
				Game_Settings_Window = false;
				Weather_Tools_Window = false;
				Visuals_Tools_Window = false;
				//LB: shooter now a filter mode Shooter_Tools_Window = false;
				Entity_Tools_Window = false;
				bWaypoint_Window = false;
				iRestoreLastWindow = 0;
			}
			bTerrain_Tools_Window = true;
			t.inputsys.domodeterrain = 1; t.inputsys.dowaypointview = 0;
			bTerrain_Tools_Window = true;
			t.terrain.terrainpaintermode = 1;
		}
		if (t.inputsys.k_s == "v")
		{
		}
		if (t.inputsys.k_s == "o")
		{
			if (!pref.iEnableSingleRightPanelAdvanced)
			{
				Game_Settings_Window = false;
				Weather_Tools_Window = false;
				Visuals_Tools_Window = false;
				//LB: shooter now a filter mode Shooter_Tools_Window = false;
				bTerrain_Tools_Window = false;
				bWaypoint_Window = false;
				iRestoreLastWindow = 0;
			}
			Entity_Tools_Window = true;
			t.inputsys.domodeentity = 1; t.inputsys.dowaypointview = 0;
		}
		if (t.inputsys.keyspace == 1 && t.inputsys.keypress == 0) { t.inputsys.dowaypointview = 1 - t.inputsys.dowaypointview; t.inputsys.keypress = 1; t.lastgrideditselect = -1; editor_refresheditmarkers(); }

		//  NUM-ROTATE CONTROLS
		if (t.inputsys.k_s == "r" && t.inputsys.keypress == 0 && t.ebe.on == 0 )
		{
			t.inputsys.dorotation = 1; t.inputsys.keypress = 1;
		}
		if (t.grideditselect != 4 && t.grideditselect != 0)
		{
			if (pref.iEnableAxisRotationShortcuts == 1)
			{
				if (t.inputsys.k_s == "1" && t.inputsys.keypress == 0) { t.inputsys.doentityrotate = 1; t.inputsys.keypress = 1; }
				if (t.inputsys.k_s == "2" && t.inputsys.keypress == 0) { t.inputsys.doentityrotate = 2; t.inputsys.keypress = 1; }
				if (t.inputsys.k_s == "3" && t.inputsys.keypress == 0) { t.inputsys.doentityrotate = 3; t.inputsys.keypress = 1; }
				if (t.inputsys.k_s == "4" && t.inputsys.keypress == 0) { t.inputsys.doentityrotate = 4; t.inputsys.keypress = 1; }
				if (t.inputsys.k_s == "5" && t.inputsys.keypress == 0) { t.inputsys.doentityrotate = 5; t.inputsys.keypress = 1; }
				if (t.inputsys.k_s == "6" && t.inputsys.keypress == 0) { t.inputsys.doentityrotate = 6; t.inputsys.keypress = 1; }
				if (t.inputsys.keyshift == 0)
				{
					if (t.inputsys.k_s == "0" && t.inputsys.keypress == 0) { t.inputsys.doentityrotate = 98; t.inputsys.keypress = 1; }
				}
				else
				{
					if (t.inputsys.k_s == "0" && t.inputsys.keypress == 0) { t.inputsys.doentityrotate = 99; t.inputsys.keypress = 1; }
				}
			}
		}

		//  Editing of Map
		if (t.inputsys.k_s == ",")  t.inputsys.dozoomin = 1;
		if (t.inputsys.k_s == ".")  t.inputsys.dozoomout = 1;

		//  TAB Key causes layer edit view control
		if (t.inputsys.kscancode == 15 && t.inputsys.keypress == 0) { t.inputsys.dosinglelayer = 1; t.inputsys.keypress = 1; }

		static bool bF1Released = false;
		if (t.inputsys.kscancode != 59)
			bF1Released = true;

		// F1 to toggle widget mode or smart positioning mode.
		if (t.inputsys.kscancode == 59 && bF1Released)
		{
			bF1Released = false;
			pref.iEnableDragDropWidgetSelect = !pref.iEnableDragDropWidgetSelect;
			if (pref.iEnableDragDropWidgetSelect)
				widget_show_widget();
			else
				widget_hide();
		}

		// F2-F4 in editor to control widget mode or smart positioning mode.
		if (t.inputsys.kscancode >= 60 && t.inputsys.kscancode <= 62)
		{
			t.toldmode = t.widget.mode;
			bool bWidgetEnabled = pref.iEnableDragDropWidgetSelect;
			if (t.inputsys.kscancode == 60) 
			{
				if (bWidgetEnabled)
					t.widget.mode = 0;
				else
					iObjectMoveMode = 2;

			}
			if (t.inputsys.kscancode == 61) 
			{
				if (bWidgetEnabled)
					t.widget.mode = 1;
				else
					iObjectMoveMode = 0;
			}
			if (t.inputsys.kscancode == 62) 
			{
				if (bWidgetEnabled)
				{
					// Don't allow characters and markers to be scaled with the widget
					int entid = t.entityelement[t.widget.pickedEntityIndex].bankindex;
					if (entid > 0)
					{
						bool bAllowObjectsAndParticlesToScale = false;
						if (t.entityprofile[entid].ismarker == 0) bAllowObjectsAndParticlesToScale = true;
						if (t.entityprofile[entid].ismarker == 10) bAllowObjectsAndParticlesToScale = true;
						if (t.entityprofile[entid].ischaracter == 0 && bAllowObjectsAndParticlesToScale==true)
						{
							t.widget.mode = 2;
						}
					}
				}
				else
				{
					iObjectMoveMode = 1;
				}
			}
			
			if ( t.toldmode != t.widget.mode ) widget_show_widget ( );
		}
	}
	else
	{
		if (t.inputsys.k_s == "r" && t.ebe.on == 0)  t.inputsys.dorotation = 1;
	}

	//  Key Map Scroll and Resize
	if (t.inputsys.keyshift == 0)
	{
		if (t.inputsys.keyleft == 1)  t.inputsys.doscrollleft = 3;
		if (t.inputsys.keyright == 1)  t.inputsys.doscrollright = 3;
		if (t.inputsys.keyup == 1) 
			t.inputsys.doscrollup = 3;
		if (t.inputsys.keydown == 1)  t.inputsys.doscrolldown = 3;
	}
	else
	{
		if (t.inputsys.keyleft == 1)  t.inputsys.doscrollleft = 20;
		if (t.inputsys.keyright == 1)  t.inputsys.doscrollright = 20;
		if (t.inputsys.keyup == 1)
			t.inputsys.doscrollup = 20;
		if (t.inputsys.keydown == 1)  t.inputsys.doscrolldown = 20;
	}

	//  Mouse Wheel control (170616 - but not when in EBE mode as its used for grid layer control)
	if (t.ebe.on == 0)
	{
		if (t.grideditselect == 4)
		{
			//  Zoomed in View
			t.zoomviewcamerarange_f -= (t.inputsys.wheelmousemove / 10.0);
		}
		else
		{
			//  Non-Zoomed in View
			if (t.inputsys.keycontrol == 0)
			{
				if (t.inputsys.wheelmousemove<0)
					t.inputsys.dozoomout = 1;
				if (t.inputsys.wheelmousemove>0)
					t.inputsys.dozoomin = 1;
			}
		}
	}

	//  UndoRedo Keys
	if (t.inputsys.keycontrol == 1)
	{
		if (t.inputsys.k_s == "z" && t.inputsys.undokeypress == 0) 
		{ 
			t.inputsys.doundo = 1; t.inputsys.undokeypress = 1; 
		}
		if (t.inputsys.k_s == "y" && t.inputsys.undokeypress == 0) { t.inputsys.doredo = 1; t.inputsys.undokeypress = 1; }
	}

	//  Controls only when in zoomview
	if (t.grideditselect == 4)
	{
		//  orient arrowkey movement to camera angle
		t.tca_f = WrapValue(CameraAngleY());
		if (t.tca_f >= 360 - 45 || t.tca_f <= 45)
		{
			t.txa = 1; t.txb = 2; t.txc = 0; t.txd = 0;
			t.tza = 0; t.tzb = 0; t.tzc = 2; t.tzd = 1;
		}
		else
		{
			if (t.tca_f >= 180 - 45 && t.tca_f <= 180 + 45)
			{
				t.txa = 2; t.txb = 1; t.txc = 0; t.txd = 0;
				t.tza = 0; t.tzb = 0; t.tzc = 1; t.tzd = 2;
			}
			else
			{
				if (t.tca_f <= 180)
				{
					t.txa = 0; t.txb = 0; t.txc = 2; t.txd = 1;
					t.tza = 2; t.tzb = 1; t.tzc = 0; t.tzd = 0;
				}
				else
				{
					t.txa = 0; t.txb = 0; t.txc = 1; t.txd = 2;
					t.tza = 1; t.tzb = 2; t.tzc = 0; t.tzd = 0;
				}
			}
		}
		t.inputsys.dozoomviewmovex = 0; t.inputsys.dozoomviewmovez = 0;
		if (t.inputsys.keyleft == 1) { t.inputsys.dozoomviewmovex += t.txa; t.inputsys.dozoomviewmovez += t.tza; }
		if (t.inputsys.keyright == 1) { t.inputsys.dozoomviewmovex += t.txb; t.inputsys.dozoomviewmovez += t.tzb; }
		if (t.inputsys.keyup == 1)
		{
			t.inputsys.dozoomviewmovex += t.txc;
			t.inputsys.dozoomviewmovez += t.tzc;
		}
		if (t.inputsys.keydown == 1) { t.inputsys.dozoomviewmovex += t.txd; t.inputsys.dozoomviewmovez += t.tzd; }
		//  control rotation
		if (t.inputsys.k_s == "1" && t.inputsys.keypress == 0) { t.inputsys.dozoomviewrotatex = 1; t.inputsys.keypress = 1; }
		if (t.inputsys.k_s == "2" && t.inputsys.keypress == 0) { t.inputsys.dozoomviewrotatex = 2; t.inputsys.keypress = 1; }
		if (t.inputsys.k_s == "3" && t.inputsys.keypress == 0) { t.inputsys.dozoomviewrotatey = 1; t.inputsys.keypress = 1; }
		if (t.inputsys.k_s == "4" && t.inputsys.keypress == 0) { t.inputsys.dozoomviewrotatey = 2; t.inputsys.keypress = 1; }
		if (t.inputsys.k_s == "5" && t.inputsys.keypress == 0) { t.inputsys.dozoomviewrotatez = 1; t.inputsys.keypress = 1; }
		if (t.inputsys.k_s == "6" && t.inputsys.keypress == 0) { t.inputsys.dozoomviewrotatez = 2; t.inputsys.keypress = 1; }
		if (t.inputsys.keyshift == 0)
		{
			if (t.inputsys.k_s == "0" && t.inputsys.keypress == 0) { t.inputsys.dozoomviewrotatex = 98; t.inputsys.keypress = 1; }
		}
		else
		{
			if (t.inputsys.k_s == "0" && t.inputsys.keypress == 0) { t.inputsys.dozoomviewrotatex = 99; t.inputsys.keypress = 1; }
		}
	}
	if (t.grideditselect == 4 || t.grideditselect == 5)
	{
		//  control finder (toggled using gridentityautofind value)
		// Simpler RETURN system
		t.gridentitydroptoground = 0;
		if (t.inputsys.keyreturn == 1)
		{
			t.gridentityautofind = 0;
			t.gridentityusingsoftauto = 0;
			t.gridentitysurfacesnap = 0;
			if (iObjectMoveModeDropSystemUsing == 1 && g_bHoldGridEntityPosWhenManaged == false)
			{
				if (t.gridentity > 0)
				{
					t.gridentitydroptoground = 1 + t.entityprofile[t.gridentity].forwardfacing;
				}
			}
		}
		//  control height
		if (t.grideditselect == 4)
		{
			//  move entity through zoomview system
			if (t.inputsys.kscancode == 201) { t.inputsys.dozoomviewmovey = 2; t.gridentityposoffground = 1; t.gridentityautofind = 0; t.gridentityusingsoftauto = 0; }
			if (t.inputsys.kscancode == 209) { t.inputsys.dozoomviewmovey = 1; t.gridentityposoffground = 1; t.gridentityautofind = 0; t.gridentityusingsoftauto = 0; }
		}
		else
		{
			//  directly move entity (and detatch from terrain) PGUP and PGDN
			if (t.inputsys.kscancode == 201 || t.inputsys.kscancode == 209)
			{
				editor_handlepguppgdn();
				t.gridentityposoffground = 1; t.gridentityautofind = 0; t.gridentityusingsoftauto = 0; t.gridentitysurfacesnap = 0;
			}
		}
	}

	//  Create a waypoint when instructed to
	if (t.inputsys.domodewaypointcreate == 1 && t.inputsys.keypress == 0)
	{
		//In freeflight mode t.cx_f,t.cy_f is NOT the same as CameraPositionX() , CameraPositionZ().
		//Search for "Debug c_xy" and enable those lines to see the difference.

		//Changed to this:

		float placeatx_f, placeatz_f;
		placeatx_f = CameraPositionX();
		placeatz_f = CameraPositionZ();

		t.inputsys.domodewaypointcreate = 0;
		t.inputsys.keypress = 1; t.inputsys.domodewaypoint = 1; t.grideditselect = 6;
		if (t.terrain.TerrainID>0)
		{
			g.waypointeditheight_f = BT_GetGroundHeight(t.terrain.TerrainID, placeatx_f, placeatz_f); //
		}
		else
		{
			g.waypointeditheight_f = g.gdefaultterrainheight;
		}
		t.waypointeditstyle = 1; t.waypointeditstylecolor = 0; t.waypointeditentity = 0;
		//t.mx_f = t.cx_f; t.mz_f = t.cy_f;
		t.mx_f = placeatx_f;
		t.mz_f = placeatz_f;
		waypoint_createnew();

		PointCamera(t.mx_f, g.waypointeditheight_f, t.mz_f);
		t.editorfreeflight.c.angx_f = CameraAngleX();
		t.editorfreeflight.c.angy_f = CameraAngleY();
	}

	//  fake mousemove values for low-response systems (when in zoomed in mode)
	if (t.grideditselect == 4)
	{
		if (t.inputsys.keyshift == 1)
		{
			if (t.inputsys.keyleft == 1)  t.inputsys.xmousemove = -10;
			if (t.inputsys.keyright == 1)  t.inputsys.xmousemove = 10;
			if (t.inputsys.keyup == 1)
				t.inputsys.ymousemove = -10;
			if (t.inputsys.keydown == 1)  t.inputsys.ymousemove = 10;
			set_inputsys_mclick(2);// t.inputsys.mclick = 2;
			t.inputsys.keyleft = 0;
			t.inputsys.keyright = 0;
			t.inputsys.keyup = 0;
			t.inputsys.keydown = 0;
		}
	}

	//Update statusbar
	++t.interfacestatusbarupdate;
	if (t.interfacestatusbarupdate > 30)
	{
		strcpy(statusbar, "");

		if (t.inputsys.xmouse == 500000)
		{
			//t.strwork = ""; t.statusbar_s = t.statusbar_s + "X: 0 Z: 0";
			strcpy(statusbar, "X: 0 Z: 0 | ");
		}
		else {
			//t.strwork = ""; t.statusbar_s = t.statusbar_s + "X:" + Str(t.inputsys.mmx) + " " + "Z:" + Str(t.inputsys.mmy);
			sprintf(statusbar, "X:%d Z:%d | ", t.inputsys.mmx, t.inputsys.mmy);
		}

		//PE: 17/08/21 reactivated.
		//t.statusbar_s = t.statusbar_s + " | ";
		if (t.gridentitygridlock == 0)  strcat(statusbar, "NORMAL"); // t.statusbar_s = t.statusbar_s + "NORMAL";
		if (t.gridentitygridlock == 1)  strcat(statusbar, "SNAP"); //t.statusbar_s = t.statusbar_s + "SNAP";
		if (t.gridentitygridlock == 2)  strcat(statusbar, "GRID"); //t.statusbar_s = t.statusbar_s + "GRID";

		//  editing mode

		//336 = Clipboard Selection Mode (CTRL+C=Copy DELETE=Clear)
		//332 = Terrain Painting Mode:
		//343 = Zoomed In Mode (Right click and drag to view, Left to Exit)
		//344 = Entity Editing Mode (R=Rotate Entity  ENTER=Find Floor/Wall)

		if (t.grideditselect == 0)
		{
			if (t.terrain.terrainpaintermode >= 6) {
				if (t.terrain.terrainpaintermode == 11) {
					t.laststatusbar_s = "Terrain Tree Editing Mode";
				}
				else if (t.terrain.terrainpaintermode == 12) {
					t.laststatusbar_s = "Terrain Bush Editing Mode";
				}
				else if (t.terrain.terrainpaintermode == 10) {
					t.laststatusbar_s = "Terrain Vegetation Editing Mode";
				}
				else {
					t.laststatusbar_s = "Terrain Painting Mode";
				}
			}
			else {

				t.laststatusbar_s = "Terrain Sculpt Mode ";

				if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_RAISE || ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_LOWER)
					t.laststatusbar_s = t.laststatusbar_s + "- Shape Mode";
				else if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_LEVEL)
					t.laststatusbar_s = t.laststatusbar_s + "- Level Mode";
				else if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_BLEND)
					t.laststatusbar_s = t.laststatusbar_s + "- Blend Mode";
				else if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_RAMP)
					t.laststatusbar_s = t.laststatusbar_s + "- Ramp Mode";
				else if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_PICK)
					t.laststatusbar_s = t.laststatusbar_s + "- Pick Height Mode";
				else if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_WRITE)
					t.laststatusbar_s = t.laststatusbar_s + "- Use Picked Height Mode";
				else if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_RANDOM)
					t.laststatusbar_s = t.laststatusbar_s + "- Random Mode";
				else if (ggterrain_extra_params.sculpt_mode == GGTERRAIN_SCULPT_RESTORE)
					t.laststatusbar_s = t.laststatusbar_s + "- Restore Mode";

			}
		}

		if (t.grideditselect == 5)
		{
			t.laststatusbar_s = "Object Editing Mode - ";
			t.laststatusbar_s = t.laststatusbar_s + " Object: " + t.relaytostatusbar_s;
		}
		if (t.grideditselect == 6)
		{
			//  add waypoint sta2tus
			t.laststatusbar_s = "Waypoint Editing Mode";
		}

		if (g_bCharacterCreatorPlusActivated)
			t.laststatusbar_s = "Character Creator Mode";
		if ( (bBuilder_Properties_Window || t.ebe.on == 1) || (t.gridentity > 0 && t.entityprofile[t.gridentity].isebe != 0) )
			t.laststatusbar_s = "Structure Editor Mode";
		if (bImporter_Window && t.importer.importerActive == 1)
			t.laststatusbar_s = "Importer Mode";

		//  only update infrequently
		t.interfacestatusbarupdate = 0;

	}

	static cstr WinTitle = "";

	if (strcmp(Lower(Left(g.projectfilename_s.Get(), Len(g.rootdir_s.Get()))), Lower(g.rootdir_s.Get())) == 0)
	{
		WinTitle = Right(g.projectfilename_s.Get(), Len(g.projectfilename_s.Get()) - Len(g.rootdir_s.Get()));
	}
	else
	{
		WinTitle = g.projectfilename_s;
	}
	if (g.projectmodified != 0)  WinTitle = WinTitle + "*";

	if (bStoryboardWindow)
	{
		WinTitle = Storyboard.gamename;
		if (Storyboard.project_readonly == 1)
		{
			WinTitle = WinTitle + " (read only)";
		}
		else
		{
			if (Storyboard.iChanged) WinTitle = WinTitle + "*";
		}
	}

	if (WinTitle != CurrentWinTitle) 
	{
		//Change windows title
		CurrentWinTitle = WinTitle;
		cstr NewTitle = "GameGuru MAX - ";
		if (strnicmp(WinTitle.Get(), "mapbank\\", 8) == 0)
			WinTitle = WinTitle.Get() + 8;
		NewTitle = NewTitle + WinTitle;
		SetWindowTitle(NewTitle.Get());
	}

}

void input_getcontrols ( void )
{
	//  Some actions are directly triggered by input subroutine
	t.inputsys.doload=0;
	t.inputsys.domodeterrain=0;
	t.inputsys.domodeentity=0;
	t.inputsys.domodemarker=0;
	t.inputsys.domodewaypoint=0;
	t.inputsys.doundo=0;
	t.inputsys.doredo=0;
	t.inputsys.tselcontrol=0;
	t.inputsys.tselcut=0;
	t.inputsys.tselcopy=0;
	t.inputsys.tseldelete=0;

	input_getdirectcontrols ( );

	//  Flag reset
	t.inputsys.dorotation=0;
	t.inputsys.domirror=0;
	t.inputsys.doflip=0;
	t.inputsys.doentityrotate=0;
	t.inputsys.dozoomin=0;
	t.inputsys.dozoomout=0;
	t.inputsys.doscrollleft=0;
	t.inputsys.doscrollright=0;
	t.inputsys.doscrollup=0;
	t.inputsys.doscrolldown=0;
	t.inputsys.domapresize=0;
	t.inputsys.dogroundmode=-1;
	t.inputsys.dozoomview=0;
	t.inputsys.dozoomviewmovex=0;
	t.inputsys.dozoomviewmovey=0;
	t.inputsys.dozoomviewmovez=0;
	t.inputsys.dozoomviewrotatex=0;
	t.inputsys.dozoomviewrotatey=0;
	t.inputsys.dozoomviewrotatez=0;
	t.inputsys.dosinglelayer=0;
	t.inputsys.tselfloor=0;
	t.inputsys.tselpaste=0;
	t.inputsys.tselwipe=0;
	t.inputsys.dosaveandrun=0;

	//  Input conditional flags
	if (t.inputsys.kscancode == 0) 
	{
		t.inputsys.keypress = 0;
		if (iForceScancode > 0 ) 
		{
			t.inputsys.kscancode = iForceScancode;
			iForceScancode = -1;
		}
		else if (bForceKey) 
		{
			bForceKey = false;
			t.inputsys.k_s = csForceKey;
		}
	}


	//  Construction Keys
	if (  t.inputsys.keycontrol == 0 ) 
	{
		// can get marker mode from anywhere
		if ( (t.inputsys.kscancode == Asc("M") || t.inputsys.k_s == "m") && t.inputsys.keypress == 0 ) 
		{
			t.inputsys.domodemarker = 1;
			t.inputsys.keypress = 1; 
		}

		if ( (t.grideditselect == 4 && t.gridentityinzoomview>0) || t.grideditselect == 5 ) 
		{
			if (  t.inputsys.k_s == "g" && t.inputsys.keypress == 0 )
			{
				t.inputsys.keypress=1; 
				t.gridentitygridlock=t.gridentitygridlock+1;
				if ( t.gridentitygridlock>2 )  
					t.gridentitygridlock = 0;
				pref.iGridMode = t.gridentitygridlock;
			}
			if (  t.inputsys.k_s == "y" && t.inputsys.keypress == 0 && g.gentitytogglingoff == 0 ) 
			{
				// only if not EBE
				if ( t.entityprofile[t.gridentity].isebe == 0 )
				{
					t.ttrygridentitystaticmode=1-t.gridentitystaticmode;
					t.ttrygridentity=t.gridentity ; editor_validatestaticmode ( );
				}
				t.inputsys.keypress=1; 
			}
			if (  t.inputsys.k_s == "u" && t.inputsys.keypress == 0 ) 
			{
				//  control auto-flatten
				t.inputsys.keypress=1;
				t.gridedit.autoflatten=1-t.gridedit.autoflatten;
			}
			if (  t.inputsys.k_s == "i" && t.inputsys.keypress == 0 ) 
			{
				//  control entity spray mode
				t.inputsys.keypress=1;
				t.gridedit.entityspraymode=1-t.gridedit.entityspraymode;
			}
			// except when in EBE mode which handles - and + keys for material changing
			if ( t.ebe.on == 0 )
			{
				if (  t.inputsys.k_s == "-" && t.inputsys.keypress == 0 ) { t.gridentitymodifyelement = 1  ; t.inputsys.keypress = 1; }
				if (  t.inputsys.k_s == "=" && t.inputsys.keypress == 0 ) { t.gridentitymodifyelement = 2 ; t.inputsys.keypress = 1; }
			}
		}

		//  editing mode
		if (  t.inputsys.k_s == "t" ) { t.inputsys.domodeterrain = 1  ; t.inputsys.dowaypointview = 0; }
		if (  t.inputsys.k_s == "o" ) { t.inputsys.domodeentity = 1  ; t.inputsys.dowaypointview = 0; }
		if (  t.inputsys.k_s == "p" ) { t.inputsys.domodewaypoint = 1  ; t.inputsys.dowaypointview = 0; }
		if ( t.inputsys.keyspace == 1 && t.inputsys.keypress == 0 ) { t.inputsys.dowaypointview=1-t.inputsys.dowaypointview ; t.inputsys.keypress=1 ; t.lastgrideditselect=-1  ; editor_refresheditmarkers ( ); }

		//  NUM-ROTATE CONTROLS
		if (  t.inputsys.k_s == "r" && t.inputsys.keypress == 0 && t.ebe.on == 0) { t.inputsys.dorotation = 1 ; t.inputsys.keypress = 1; }
		if (  t.grideditselect != 4 && t.grideditselect != 0 ) 
		{
			if (  t.inputsys.k_s == "1" && t.inputsys.keypress == 0 ) { t.inputsys.doentityrotate = 1  ; t.inputsys.keypress = 1; }
			if (  t.inputsys.k_s == "2" && t.inputsys.keypress == 0 ) { t.inputsys.doentityrotate = 2  ; t.inputsys.keypress = 1; }
			if (  t.inputsys.k_s == "3" && t.inputsys.keypress == 0 ) { t.inputsys.doentityrotate = 3  ; t.inputsys.keypress = 1; }
			if (  t.inputsys.k_s == "4" && t.inputsys.keypress == 0 ) { t.inputsys.doentityrotate = 4  ; t.inputsys.keypress = 1; }
			if (  t.inputsys.k_s == "5" && t.inputsys.keypress == 0 ) { t.inputsys.doentityrotate = 5  ; t.inputsys.keypress = 1; }
			if (  t.inputsys.k_s == "6" && t.inputsys.keypress == 0 ) { t.inputsys.doentityrotate = 6  ; t.inputsys.keypress = 1; }
			if (  t.inputsys.keyshift == 0 ) 
			{
				if (  t.inputsys.k_s == "0" && t.inputsys.keypress == 0 ) { t.inputsys.doentityrotate = 98  ; t.inputsys.keypress = 1; }
			}
			else
			{
				if (  t.inputsys.k_s == "0" && t.inputsys.keypress == 0 ) { t.inputsys.doentityrotate = 99 ; t.inputsys.keypress = 1; }
			}
		}

		// Editing of Map
		if ( t.inputsys.k_s == ","  )  t.inputsys.dozoomin = 1;
		if ( t.inputsys.k_s == "."  )  t.inputsys.dozoomout = 1;

		// TAB Key causes layer edit view control
		if ( t.inputsys.kscancode == 15 && t.inputsys.keypress == 0 ) { t.inputsys.dosinglelayer = 1  ; t.inputsys.keypress = 1; }

		// F1 for help page

		// this is the non-IDE input function (need to consolidate at some point - yucky repeat code!)
	}
	else
	{
		if (  t.inputsys.k_s == "r" && t.ebe.on == 0)  t.inputsys.dorotation = 1;
	}

	//  Key Map Scroll and Resize
	if (  t.inputsys.keyshift == 0 ) 
	{
		if (  t.inputsys.keyleft == 1  )  t.inputsys.doscrollleft = 3;
		if (  t.inputsys.keyright == 1  )  t.inputsys.doscrollright = 3;
		if (  t.inputsys.keyup == 1  )  t.inputsys.doscrollup = 3;
		if (  t.inputsys.keydown == 1  )  t.inputsys.doscrolldown = 3;
	}
	else
	{
		if (  t.inputsys.keyleft == 1  )  t.inputsys.doscrollleft = 20;
		if (  t.inputsys.keyright == 1  )  t.inputsys.doscrollright = 20;
		if (  t.inputsys.keyup == 1  )  t.inputsys.doscrollup = 20;
		if (  t.inputsys.keydown == 1  )  t.inputsys.doscrolldown = 20;
	}

	//  Mouse Wheel control (170616 - but not when in EBE mode as its used for grid layer control)
	if ( t.ebe.on == 0 )
	{
		if (  t.grideditselect == 4 ) 
		{
			//  Zoomed in View
			t.zoomviewcamerarange_f -= (t.inputsys.wheelmousemove / 10.0);
		}
		else
		{
			//  Non-Zoomed in View
			if (  t.inputsys.keycontrol == 0 ) 
			{
				if (  t.inputsys.wheelmousemove<0  )
					t.inputsys.dozoomout = 1;
				if (  t.inputsys.wheelmousemove>0  )
					t.inputsys.dozoomin = 1;
			}
		}
	}

	//  UndoRedo Keys
	if (  t.inputsys.keycontrol == 1 ) 
	{
		if (  t.inputsys.k_s == "z" && t.inputsys.undokeypress == 0 ) { t.inputsys.doundo = 1  ; t.inputsys.undokeypress = 1; }
		if (  t.inputsys.k_s == "y" && t.inputsys.undokeypress == 0 ) { t.inputsys.doredo = 1  ; t.inputsys.undokeypress = 1; }
	}

	//  Controls only when in zoomview
	if (  t.grideditselect == 4 ) 
	{
		//  orient arrowkey movement to camera angle
		t.tca_f=WrapValue(CameraAngleY());
		if (  t.tca_f >= 360-45 || t.tca_f <= 45 ) 
		{
			t.txa=1 ; t.txb=2 ; t.txc=0 ; t.txd=0;
			t.tza=0 ; t.tzb=0 ; t.tzc=2 ; t.tzd=1;
		}
		else
		{
			if (  t.tca_f >= 180-45 && t.tca_f <= 180+45 ) 
			{
				t.txa=2 ; t.txb=1 ; t.txc=0 ; t.txd=0;
				t.tza=0 ; t.tzb=0 ; t.tzc=1 ; t.tzd=2;
			}
			else
			{
				if (  t.tca_f <= 180 ) 
				{
					t.txa=0 ; t.txb=0 ; t.txc=2 ; t.txd=1;
					t.tza=2 ; t.tzb=1 ; t.tzc=0 ; t.tzd=0;
				}
				else
				{
					t.txa=0 ; t.txb=0 ; t.txc=1 ; t.txd=2;
					t.tza=1 ; t.tzb=2 ; t.tzc=0 ; t.tzd=0;
				}
			}
		}
		t.inputsys.dozoomviewmovex=0 ; t.inputsys.dozoomviewmovez=0;
		if (  t.inputsys.keyleft == 1 ) { t.inputsys.dozoomviewmovex += t.txa  ; t.inputsys.dozoomviewmovez +=t.tza; }
		if (  t.inputsys.keyright == 1 ) { t.inputsys.dozoomviewmovex += t.txb  ; t.inputsys.dozoomviewmovez += t.tzb; }
		if (  t.inputsys.keyup == 1 ) { t.inputsys.dozoomviewmovex += t.txc  ; t.inputsys.dozoomviewmovez += t.tzc; }
		if (  t.inputsys.keydown == 1 ) { t.inputsys.dozoomviewmovex +=t.txd  ; t.inputsys.dozoomviewmovez += t.tzd; }
		//  control rotation
		if (  t.inputsys.k_s == "1" && t.inputsys.keypress == 0 ) { t.inputsys.dozoomviewrotatex = 1  ; t.inputsys.keypress = 1; }
		if (  t.inputsys.k_s == "2" && t.inputsys.keypress == 0 ) { t.inputsys.dozoomviewrotatex = 2  ; t.inputsys.keypress = 1; }
		if (  t.inputsys.k_s == "3" && t.inputsys.keypress == 0 ) { t.inputsys.dozoomviewrotatey = 1  ; t.inputsys.keypress = 1; }
		if (  t.inputsys.k_s == "4" && t.inputsys.keypress == 0 ) { t.inputsys.dozoomviewrotatey = 2  ; t.inputsys.keypress = 1; }
		if (  t.inputsys.k_s == "5" && t.inputsys.keypress == 0 ) { t.inputsys.dozoomviewrotatez = 1  ; t.inputsys.keypress = 1; }
		if (  t.inputsys.k_s == "6" && t.inputsys.keypress == 0 ) { t.inputsys.dozoomviewrotatez = 2  ; t.inputsys.keypress = 1; }
		if (  t.inputsys.keyshift == 0 ) 
		{
			if (  t.inputsys.k_s == "0" && t.inputsys.keypress == 0 ) { t.inputsys.dozoomviewrotatex = 98  ; t.inputsys.keypress = 1; }
		}
		else
		{
			if (  t.inputsys.k_s == "0" && t.inputsys.keypress == 0 ) { t.inputsys.dozoomviewrotatex = 99  ; t.inputsys.keypress = 1; }
		}
	}
	if (  t.grideditselect == 4 || t.grideditselect == 5 ) 
	{
		//  control finder (toggled using gridentityautofind value)
		if (  t.inputsys.keyreturn == 1 ) 
		{
			if (  t.gridentityautofind == 0  ) { t.gridentityautofind = 3; }
			if (  t.gridentityautofind == 1  ) { t.gridentityautofind = 2; }
		}
		else
		{
			if (  t.gridentityautofind == 3 ) { t.gridentityautofind = 1  ; t.gridentityusingsoftauto = 0; t.gridentitysurfacesnap = 0; }
			if (  t.gridentityautofind == 2 ) { t.gridentityautofind = 0  ; t.gridentityposoffground = 0 ; t.gridentityusingsoftauto = 1; t.gridentitysurfacesnap = 0; }
		}
		if ( t.gridentityautofind == 1 && t.gridentity>0 ) 
		{
			t.gridentitydroptoground = 1 + t.entityprofile[t.gridentity].forwardfacing;
		}
		else
		{
			t.gridentitydroptoground=0;
		}
		//  control height
		if (  t.grideditselect == 4 ) 
		{
			//  move entity through zoomview system
			if (  t.inputsys.kscancode == 201 ) { t.inputsys.dozoomviewmovey = 2  ; t.gridentityposoffground = 1 ; t.gridentityautofind = 0 ; t.gridentityusingsoftauto = 0; }
			if (  t.inputsys.kscancode == 209 ) { t.inputsys.dozoomviewmovey = 1  ; t.gridentityposoffground = 1 ; t.gridentityautofind = 0 ; t.gridentityusingsoftauto = 0; }
		}
		else
		{
			//  directly move entity (and detatch from terrain) PGUP and PGDN
			if ( t.inputsys.kscancode == 201 || t.inputsys.kscancode == 209 ) 
			{
				if ( t.widget.activeObject == 0 ) 
				{
					editor_handlepguppgdn();
					t.gridentityposoffground=1 ; t.gridentityautofind=0 ; t.gridentityusingsoftauto=0; t.gridentitysurfacesnap=0;
				}
			}
		}
	}

	//  Create a waypoint when instructed to
	if (  t.inputsys.domodewaypointcreate == 1 && t.inputsys.keypress == 0 ) 
	{
		t.inputsys.domodewaypointcreate=0;
		t.inputsys.keypress=1 ; t.inputsys.domodewaypoint=1 ; t.grideditselect=6;
		if (  t.terrain.TerrainID>0 ) 
		{
			g.waypointeditheight_f=BT_GetGroundHeight(t.terrain.TerrainID,t.cx_f,t.cy_f);
		}
		else
		{
			g.waypointeditheight_f=g.gdefaultterrainheight;
		}
		t.waypointeditstyle=1 ; t.waypointeditstylecolor=0 ; t.waypointeditentity=0;
		t.mx_f=t.cx_f ; t.mz_f=t.cy_f  ; waypoint_createnew ( );
	}
}

bool CameraInsideObject (sObject* pObject)
{
	GGVECTOR3 vecLocalCamPos = GGVECTOR3(CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0));
	vecLocalCamPos -= pObject->position.vecPosition;
	GGMATRIX inverseMatrix = pObject->position.matObjectNoTran;
	float fDet;
	GGMatrixInverse (&inverseMatrix, &fDet, &inverseMatrix);
	GGVec3TransformCoord(&vecLocalCamPos, &vecLocalCamPos, &inverseMatrix);
	float fMinX = pObject->collision.vecMin.x;
	float fMinY = pObject->collision.vecMin.y;
	float fMinZ = pObject->collision.vecMin.z;
	float fMaxX = pObject->collision.vecMax.x;
	float fMaxY = pObject->collision.vecMax.y;
	float fMaxZ = pObject->collision.vecMax.z;
	if (vecLocalCamPos.x >= fMinX && vecLocalCamPos.x <= fMaxX && vecLocalCamPos.y >= fMinY && vecLocalCamPos.y <= fMaxY && vecLocalCamPos.z >= fMinZ && vecLocalCamPos.z <= fMaxZ)
		return true;
	else
		return false;
}

uint32_t g_iGridEntityFlattener = -1;

void input_calculatelocalcursor ( void )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	//PE: Dont change anything when right mouse down.
	if (ImGui::IsMouseDown(1)) return;

	// once object management begun, only allow ray tests once user has clicked and 'moved' the cursor
	static int iRecordedMouseAtStartOfManagement = 0;
	if (t.gridentityobj>0)
	{
		static XMFLOAT4 lastManagedMouse;
		XMFLOAT4 currentMouse = wiInput::GetPointer();
		if (iRecordedMouseAtStartOfManagement == 0)
		{
			iRecordedMouseAtStartOfManagement = 1;
			lastManagedMouse = currentMouse;
		}
		else
		{
			if (currentMouse.x != lastManagedMouse.x || currentMouse.y != lastManagedMouse.y)
			{
				iRecordedMouseAtStartOfManagement = 2;
				g_bHoldGridEntityPosWhenManaged = false;
			}
		}
		if (iRecordedMouseAtStartOfManagement != 2)
		{
			// until we move the mouse after starting an object management, just leave
			return;
		}
	}
	else
	{
		// when no more object to manage, can reset this system
		iRecordedMouseAtStartOfManagement = 0;
		g_bHoldGridEntityPosWhenManaged = false;
	}

	// use Wicked Pick System
	t.tx_f=0; t.tz_f=0;
	float fPickedYAxis = 0.0f;
	t.inputsys.localselectedrayhit = false;

	//PE: e <= g.entityelementmax so need one additional int, if last object in level was a marker we got a heap error.
	int* piEntityVisible = new int[g.entityelementmax+1];
	memset(piEntityVisible, 0, sizeof(int) * (g.entityelementmax+1));

	bool bDisableRubberBandMoving = false;
	if (current_selected_group >= 0 && group_editing_on)
	{
		bDisableRubberBandMoving = true;
	}
	bool bHideObjectsWeWantToIgnore = false;
	if (!bDisableRubberBandMoving && pref.iEnableDragDropEntityMode) bHideObjectsWeWantToIgnore = true;
	if ( bHideObjectsWeWantToIgnore == true )
	{
		//LB: When they are being dragged about, but allow when scanning to select an object in the rubberband (otherwise vertical move mode messes up)
		if (bDraggingActive)
		{
			//PE: MUST disable collision on ALL rubberband objects.
			if (g.entityrubberbandlist.size() > 0)
			{
				for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
				{
					int e = g.entityrubberbandlist[i].e;
					if (e <= g.entityelementmax)
					{
						int obj = t.entityelement[e].obj;
						if (obj > 0 && GetVisible(obj))
						{
							piEntityVisible[e] = 1;
							HideObject(obj);
						}
						else
						{
							piEntityVisible[e] = 0;
						}
					}
				}
			}
		}

		// also disable any gameelements (such as start marker) as they can get in the way
		if (bDraggingActive)
		{
			for (int e = 1; e <= g.entityelementmax; e++)
			{
				int entid = t.entityelement[e].bankindex;
				if (entid > 0)
				{
					if (t.entityprofile[entid].ismarker != 0)
					{
						int obj = t.entityelement[e].obj;
						if (obj > 0 && GetVisible(obj))
						{
							piEntityVisible[e] = 1;
							HideObject(obj);
						}
					}
				}
			}
		}
	}

	if (!pref.iEnableDragDropEntityMode)
	{
		int iLayerMaskForPick = GGRENDERLAYERS_NORMAL;
		if (bDraggingActive || t.gridentityobj > 0) iLayerMaskForPick = GGRENDERLAYERS_TERRAIN;
		if (WickedCall_GetPick(&t.tx_f, &fPickedYAxis, &t.tz_f, NULL, NULL, NULL, NULL, iLayerMaskForPick) == true)
			t.inputsys.localselectedrayhit = true;

		// special treatment of point lights
		if (t.gridentity > 0 && t.gridentityobj > 0 && t.entityprofile[t.gridentity].ismarker == 2)
		{
			if (t.grideleprof.usespotlighting == 0)
			{
				if (t.grideleprof.light.fLightHasProbe >= 50.0f )
					t.gridentityrotatex_f = 0;
				else
					t.gridentityrotatex_f = 90;

				t.gridentityrotatey_f = 0;
				t.gridentityrotatez_f = 0;
				t.gridentityrotatequatmode = 0;
				t.gridentityrotatequatx_f = 0;
				t.gridentityrotatequaty_f = 0;
				t.gridentityrotatequatz_f = 0;
				t.gridentityrotatequatw_f = 1;
			}
			fPickedYAxis += 10.0f;
		}
	}
	else
	{
		// LB: allow finding of other object surfaces when not dragging (initial placement of object)
		int iLayerMaskForPick = GGRENDERLAYERS_NORMAL | GGRENDERLAYERS_TERRAIN;

		uint64_t hitentity = 0;
		if (pref.iEnableDragDropEntityMode && bDraggingActive && t.gridentityobj > 0) HideObject(t.gridentityobj);

		// orient to surface mode keyboard shurtcut
		if (t.inputsys.keyspace == 1 && g_iOrientToSurfaceMode == 0) g_iOrientToSurfaceMode = 2;
		if (t.inputsys.keyspace == 0 && g_iOrientToSurfaceMode == 2) g_iOrientToSurfaceMode = 1;
		if (t.inputsys.keyspace == 1 && g_iOrientToSurfaceMode == 1) g_iOrientToSurfaceMode = 3;
		if (t.inputsys.keyspace == 0 && g_iOrientToSurfaceMode == 3) g_iOrientToSurfaceMode = 0;

		bool bApplyHitOffset = false;
		bool bRayResult = false;
		float fPickX, fPickZ;
		bool bMustFaceUpOrDown = false;
		float fUpDownAngle = WrapValue(CameraAngleX(0));
		if (fUpDownAngle > 10.0f && fUpDownAngle < 350.0f)
		{
			// a horizontal plane for down views
			bMustFaceUpOrDown = true;
		}
		// special method of detecting when should eliminate hitoffsets to helo with accurate positioning
		static float fLastPickedY = 0.0f;
		static float fLastDiff = 0.0f;
		if (!ImGui::IsMouseDown(0))
		{
			fLastPickedY = 0.0f;
			fLastDiff = 0.0f;
		}
		// pick
		float fNormalX = 0.0f;
		float fNormalY = 1.0f;
		float fNormalZ = 0.0f;
		int iForwardFacing = 0;
		if (t.gridentity > 0) iForwardFacing = t.entityprofile[t.gridentity].forwardfacing;
		if (iObjectMoveMode == 2 && t.gridentityobj > 0 && bMustFaceUpOrDown == true && t.gridentity > 0 && t.entityprofile[t.gridentity].ismarker != 2 && iForwardFacing != 2)
		{
			// work out difference to move virtual mouse pointer to base of object no matter the orientation
			GGVECTOR2 vecVirtMouseOffset = GGVECTOR2(0, 0); //vecBase - vecClickPos; too clever by half!
			XMFLOAT4 currentMouse = wiInput::GetPointer();
			bRayResult = WickedCall_GetPick2(currentMouse.x + vecVirtMouseOffset.x, currentMouse.y + vecVirtMouseOffset.y, &fPickX, &fPickedYAxis, &fPickZ, &fNormalX, &fNormalY, &fNormalZ, &hitentity, iLayerMaskForPick);
			// and finally put hitoffset back to restore object relative position
			bApplyHitOffset = true;
			// simpler system easier to use - shift mouse to object true base coord, no need for mouse 3D->2D adjustment and accurate for placement!
			if (fLastPickedY == 0.0f) fLastPickedY = fPickedYAxis;
			float fDiff = fabs(fPickedYAxis - fLastPickedY) - 5.0f;
			if (fDiff < 0.0f) fDiff = 0.0f;
			if (fDiff > fLastDiff)
			{
				fLastDiff = fDiff;
				fHitOffsetX *= 0.95f;
				fHitOffsetY *= 0.95f;
				fHitOffsetZ *= 0.95f;
			}
		}
		else
		{
			bRayResult = WickedCall_GetPick(&fPickX, &fPickedYAxis, &fPickZ, &fNormalX, &fNormalY, &fNormalZ, &hitentity, iLayerMaskForPick);
		}
		//LB: to help perfect plane positioning of EMPTY LEVEL scenes, clamp to zero Y if within threshold
		if (fPickedYAxis > -1.0f && fPickedYAxis < 1.0f) 
		{
			fPickedYAxis = 0.0f;
		}
		if (bApplyHitOffset == true)
		{
			fPickX += fHitOffsetX;
			fPickedYAxis += fHitOffsetY;
			fPickZ += fHitOffsetZ;
		}
		if (bRayResult == true )
		{
			// if initial selection or in ghost mode
			bool bJustForInitialDragIn = false;
			if (bDraggingActive == false && fHitOffsetX == 0 && fHitOffsetY == 0 && fHitOffsetZ == 0) bJustForInitialDragIn = true;
			if (bDraggingActive == true && t.gridentityposx_f == 0 && t.gridentityposz_f == 0) bJustForInitialDragIn = true;
			if (bDraggingActiveInitial == true)	bJustForInitialDragIn = true;
			if (bJustForInitialDragIn==true || (iObjectMoveModeDropSystemUsing == 1 && g_bHoldGridEntityPosWhenManaged == false) || iForwardFacing == 1 || (t.entityprofile[t.gridentity].ismarker == 2))
			{
				// modify the rotation of the object in smart mode when object is forward facing
				if (t.gridentityobj > 0 && t.gridentity > 0 && t.entityprofile[t.gridentity].ischaracter == 0)
				{
					if (iObjectMoveMode == 2)
					{
						bool bAdjustAndTilt = false;
						if (t.entityprofile[t.gridentity].ismarker == 0)
						{
							if (iForwardFacing == 0 && g_iStackToSurfaceMode == 1 && g_iOrientToSurfaceMode == 1)
							{
								// ground objects can find orientation if mode selected
								bAdjustAndTilt = true;
							}
							if (iForwardFacing == 1)
							{
								// wall objects can find ANY surface
								bAdjustAndTilt = true;
							}
						}
						else
						{
							// lights ALWAYS orient to surface (but not probes)
							if (t.entityprofile[t.gridentity].ismarker == 2 && t.grideleprof.light.fLightHasProbe < 50.0f)
							{
								bAdjustAndTilt = true;
							}
						}
						if (bAdjustAndTilt == true)
						{
							fNormalX *= 100.0f;
							fNormalY *= 100.0f;
							fNormalZ *= 100.0f;
							int iObj = t.gridentityobj;
							float fStoreAngX = ObjectAngleX(iObj);
							float fStoreAngY = ObjectAngleY(iObj);
							float fStoreAngZ = ObjectAngleZ(iObj);
							PointObject(t.gridentityobj, ObjectPositionX(iObj) - fNormalX, ObjectPositionY(iObj) - fNormalY, ObjectPositionZ(iObj) - fNormalZ);
							if (iForwardFacing == 0)
							{
								PitchObjectUp(iObj, 90);
								TurnObjectRight(iObj, g_fLocalTurnRotationForSmartMode);
							}
							float fStoreEntAngX = t.gridentityrotatex_f;
							float fStoreEntAngY = t.gridentityrotatey_f;
							float fStoreEntAngZ = t.gridentityrotatez_f;
							if (t.entityprofile[t.gridentity].ismarker == 2 && t.grideleprof.usespotlighting == 1)
							{
								// spotlights need to retain their rotation!
							}
							else
							{
								t.gridentityrotatex_f = ObjectAngleX(iObj);
								t.gridentityrotatey_f = ObjectAngleY(iObj);
								t.gridentityrotatez_f = ObjectAngleZ(iObj);
								t.gridentityrotatequatmode = 0;
								t.gridentityrotatequatx_f = 0;
								t.gridentityrotatequaty_f = 0;
								t.gridentityrotatequatz_f = 0;
								t.gridentityrotatequatw_f = 1;
							}
							if (fStoreEntAngX != t.gridentityrotatex_f || fStoreEntAngY != t.gridentityrotatey_f || fStoreEntAngZ != t.gridentityrotatez_f)
							{
								// rotation resets hitoffset as it does not translate when orientation changes
								if (bJustForInitialDragIn == false)
								{
									// but only when dragging, not initial placement
									fHitOffsetX = 0.001f;
									fHitOffsetY = 0.001f;
									fHitOffsetZ = 0.001f;
								}
							}
							if (t.entityprofile[t.gridentity].ismarker == 2)
							{
								fHitOffsetX = -fNormalX / 10.0f;
								fHitOffsetY = -fNormalY / 10.0f;
								fHitOffsetZ = -fNormalZ / 10.0f;
							}
							RotateObject(iObj, fStoreAngX, fStoreAngY, fStoreAngZ);
						}
					}
					else
					{
						// all non-smart modes should keep lights hanging down
						if (t.entityprofile[t.gridentity].ismarker == 2 && t.grideleprof.usespotlighting == 0)
						{
							if (t.grideleprof.light.fLightHasProbe >= 50.0f)
								t.gridentityrotatex_f = 0;
							else
								t.gridentityrotatex_f = 270;

							t.gridentityrotatey_f = 0;
							t.gridentityrotatez_f = 0;
							t.gridentityrotatequatmode = 0;
							t.gridentityrotatequatx_f = 0;
							t.gridentityrotatequaty_f = 0;
							t.gridentityrotatequatz_f = 0;
							t.gridentityrotatequatw_f = 1;
							fPickedYAxis += 10.0f;
						}
					}
				}
			}

			// for ceiling objects, hang down from top of object
			if (iForwardFacing == 2 && t.entityprofile[t.gridentity].ismarker == 0)
			{
				fHitOffsetX = 0.001f;
				fHitOffsetY = ObjectSizeY(t.gridentityobj, 1) * 0.95f; // allow a small margin so can see if try to place on ground
				fHitOffsetZ = 0.001f;
			}

			// register the ray hit
			t.inputsys.localselectedrayhit = true;

			// confirm pick
			t.tx_f = fPickX;
			t.tz_f = fPickZ;
		}
		g_bAdjustPlaneXZUsingSurfaceXZ = true;
		if (hitentity > 0)
		{
			// if ray test on object, need to adjust plane to follow surface
			iLastHitObjectID = 0;
			iReusePickObjectID = -1;
			pReusePickObject = 0;
			iReusePickEntityID = 0;
			// found object under hovering cursor, match to entity index
			sObject* pHitObject = m_ObjectManager.FindObjectFromWickedObjectEntityID(hitentity);
			int iHitObjectEntityElementE = -1;
			if (pHitObject)
			{
				for (int e = 1; e <= g.entityelementlist; e++)
				{
					if (t.entityelement[e].obj == pHitObject->dwObjectNumber)
					{
						iHitObjectEntityElementE = e;
						break;
					}
				}
			}
			if (pref.iEnableDragDropStopSelectFromInside == 1)
			{
				// control whether can select an object from the inside
				if (iHitObjectEntityElementE != -1)
				{
					int entid = t.entityelement[iHitObjectEntityElementE].bankindex;
					if (entid > 0 && t.entityprofile[entid].ismarker == 0)
					{
						// but only if regular object (like a building, etc, not a particle marker or light)
						if (pHitObject && CameraInsideObject(pHitObject) == true) pHitObject = NULL;
					}
				}
			}
			if (pHitObject)
			{
				pReusePickObject = pHitObject;
				int e = iHitObjectEntityElementE;
				if (e > 0)
				{
					iLastHitObjectID = pHitObject->dwObjectNumber;
					iReusePickObjectID = iLastHitObjectID;
					iReusePickEntityID = e;
					fReusePickHitX = t.tx_f;
					fReusePickHitY = fPickedYAxis;
					fReusePickHitZ = t.tz_f;
				}
			}
		}
		else
		{
			// if not touching object surface, must be terrain, so restore plane in case plane was moved with 'g_bAdjustPlaneXZUsingSurfaceXZ'
			if (iObjectMoveMode == 2 && t.inputsys.mclick == 0 )
			{
				g_bResetPlaneAfterXZAdjust = true;
			}
		}
		if (pref.iEnableDragDropEntityMode && bDraggingActive && t.gridentityobj > 0) ShowObject(t.gridentityobj);
	}
	t.inputsys.picksystemused=2;

	if (bHideObjectsWeWantToIgnore == true)
	{
		// PE: Enable rubberband collision again.
		// also renable any gameelements (such as start marker) as they can get in the way
		for (int e = 1; e <= g.entityelementmax; e++)
		{
			int obj = t.entityelement[e].obj;
			if (obj > 0)
			{
				if (piEntityVisible[e] == 1)
				{
					ShowObject(obj);
				}
			}
		}
	}
	t.tilex_f=t.tx_f;
	t.tiley_f=t.tz_f;

	// free temp vis array
	if (piEntityVisible)
	{
		//PE: stille an array so [].
		delete[] piEntityVisible;
		piEntityVisible = NULL;
	}

	//  World cursor position
	t.inputsys.localx_f=t.tx_f;
	t.inputsys.localy_f=t.tz_f;
	t.tx=t.inputsys.localx_f/100.0;
	t.ty=t.inputsys.localy_f/100.0;
	if (  t.tx<0  )  t.tx = 0;
	if (  t.ty<0  )  t.ty = 0;
	if (  t.tx>t.maxx-1  )  t.tx = t.maxx-1;
	if (  t.ty>t.maxy-1  )  t.ty = t.maxy-1;
	t.inputsys.mmx=t.tx ; t.inputsys.mmy=t.ty;

	//  layer height is terrain Floor height
	t.inputsys.localcurrentterrainheight_f = BT_GetGroundHeight(t.terrain.TerrainID,t.tx_f,t.tz_f);

	// when placing waypoints, include entities as 'ground' to check
	t.inputsys.originallocalx_f = t.inputsys.localx_f;
	t.inputsys.originallocaly_f = t.inputsys.localy_f;

	extern bool bWaypointDrawmode;
	if ( t.gridentitysurfacesnap == 1 || t.onedrag > 0 || bWaypointDrawmode)
	{
		// only when finding place to place entity
		if ( t.gridentity > 0 || t.onedrag > 0 || bWaypointDrawmode)
		{
			// get distance of current terrain hit
			float fTDX = t.inputsys.localx_f - CameraPositionX();
			float fTDY = t.inputsys.localcurrentterrainheight_f - CameraPositionY();
			float fTDZ = t.inputsys.localy_f - CameraPositionZ();
			float fTerrDist = sqrt ( fabs(fTDX*fTDX) + fabs(fTDY*fTDY) * fabs(fTDZ*fTDZ) );

			// scan for surface point
			int iEntityOver = findentitycursorobj ( -1 );
			if ( iEntityOver != 0 )
			{
				// get distance of new surface point
				if (t.gridnearcameraclip > 0) 
				{
					fTDX = g.glastpickedx_f - GetFromVectorX();
					fTDY = g.glastpickedy_f - GetFromVectorY();
					fTDZ = g.glastpickedz_f - GetFromVectorZ();
				}
				else 
				{
					fTDX = g.glastpickedx_f - CameraPositionX();
					fTDY = g.glastpickedy_f - CameraPositionY();
					fTDZ = g.glastpickedz_f - CameraPositionZ();
				}
				float fSurfaceDist = sqrt ( fabs(fTDX*fTDX) + fabs(fTDY*fTDY) * fabs(fTDZ*fTDZ) );

				// if surface closer, use that
				if ( fSurfaceDist < fTerrDist )
				{
					t.inputsys.localx_f = g.glastpickedx_f;
					t.inputsys.localcurrentterrainheight_f = g.glastpickedy_f;
					t.inputsys.localy_f = g.glastpickedz_f;
				}
			}
		}
	}

	if (t.inputsys.picksystemused == 2)
	{
		// wicked pick system provides Y coordinate via terrainheight value
		t.inputsys.localcurrentterrainheight_f = fPickedYAxis;
		
		// LB: when clicking and dragging, we need to know ray hit Y so we can position
		// the widget plane at that height for correct movement of the object (otherwise it is at terrain height and goes wrong!)
		if (pref.iEnableDragDropEntityMode)
		{
			// monitor when not dragging, and use last height just before the click as the plane height
			if (bDraggingActive == false)
			{
				t.inputsys.localselectedplaneheight_f = fPickedYAxis;
			}
		}
	}

	//  height at which zoom editing happens
	t.layerheight_f=t.zoomviewtargety_f+100.0;
}

void editor_updatemarkervisibility ( void )
{
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
					if (  t.gridentityhidemarkers == 0 ) 
					{
						ShowObject (  t.obj );
					}
					else
					{
						HideObject (  t.obj );
					}
				}
			}
		}
	}
	if (  t.gridentityhidemarkers == 0 ) 
	{
		waypoint_showall ( );
	}
	else
	{
		waypoint_hideall ( );
	}
}

void editor_disableforzoom ( void )
{
	OpenFileMap (  2, "FPSEXCHANGE" );
	SetFileMapDWORD (  2, 850, 1 );
	SetEventAndWait (  2 );
}

void editor_enableafterzoom ( void )
{
	OpenFileMap (  2, "FPSEXCHANGE" );
	SetFileMapDWORD (  2, 850, 0 );
	SetEventAndWait (  2 );
	editor_cutcopyclearstate ( );
}

void editor_init ( void )
{
	image_setlegacyimageloading(true);
	//  Load editor images
	SetMipmapNum(1); //PE: mipmaps not needed.
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\quick-help.png";
	LoadImage (  t.strwork.Get(), g.editorimagesoffset+1 );
	LoadImage (  "editors\\gfx\\memorymeter.png",g.editorimagesoffset+2 );
	LoadImage (  "editors\\gfx\\4.png",g.editorimagesoffset+3 );
	LoadImage (  "editors\\gfx\\5.png",g.editorimagesoffset+4 );
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\gurumeditation.png";
	LoadImage (  t.strwork.Get() ,g.editorimagesoffset+5 );
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\gurumeditationoff.png";
	LoadImage ( t.strwork.Get() ,g.editorimagesoffset+6 );

	// +7 reserved (below)

	// Test Game prompt
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\quick-start-testlevel-prompt.png";
	LoadImage ( t.strwork.Get(), g.editorimagesoffset+61 );

	// Cursor for entity highlighting
	LoadImage ( "editors\\gfx\\9.png",g.editorimagesoffset+7 );
	LoadImage ( "editors\\gfx\\13.png",g.editorimagesoffset+13 );

	LoadImage ( "editors\\gfx\\14-white.png",g.editorimagesoffset+14 );
	if (!GetImageExistEx(g.editorimagesoffset + 14))
		LoadImage("editors\\gfx\\14.png", g.editorimagesoffset + 14);
	LoadImage ( "editors\\gfx\\14-red.png",g.editorimagesoffset+16 );
	LoadImage ( "editors\\gfx\\14-green.png",g.editorimagesoffset+17 );

	LoadImage (  "editors\\gfx\\26.png",g.editorimagesoffset+26 );
	LoadImage ( "editors\\gfx\\cursor.dds",g.editorimagesoffset+10 );

	//  F9 Edit Mode Graphical Prompts
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\f9-help-terrain.png";
	LoadImage ( t.strwork.Get() ,g.editorimagesoffset+21 );
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\f9-help-entity.png";
	LoadImage ( t.strwork.Get() ,g.editorimagesoffset+22 );
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\f9-help-conkit.png";
	LoadImage ( t.strwork.Get() ,g.editorimagesoffset+23 );

	// new images for editor extra help
	image_setlegacyimageloading(true);
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\testgamelayout-noweapons.png";
 	LoadImage (  t.strwork.Get(), g.editorimagesoffset+27 );
	t.strwork = ""; t.strwork = t.strwork + "languagebank\\"+g.language_s+"\\artwork\\testgamelayout-vr.png";
 	LoadImage (  t.strwork.Get(), g.editorimagesoffset+28 );
	image_setlegacyimageloading(false);

	//  Also loaded by interactive mode when active
	///LoadImage (  "languagebank\\neutral\\gamecore\\huds\\interactive\\close-highlight.png",g.interactiveimageoffset+15 );

	//  for overlays on map editor view
	if (  FileExist("editors\\gfx\\resources.png") == 1 ) 
	{
		LoadImage (  "editors\\gfx\\resources.png",g.editordrawlastimagesoffset+1 );
	}
	if (  FileExist("editors\\gfx\\resourceslow.png") == 1 ) 
	{
		LoadImage (  "editors\\gfx\\resourceslow.png",g.editordrawlastimagesoffset+2 );
	}
	if (  FileExist("editors\\gfx\\resourcesgone.png") == 1 ) 
	{
		LoadImage (  "editors\\gfx\\resourcesgone.png",g.editordrawlastimagesoffset+3 );
	}
	if (  FileExist("editors\\gfx\\resourcesworking.png") == 1 ) 
	{
		LoadImage (  "editors\\gfx\\resourcesworking.png",g.editordrawlastimagesoffset+4 );
	}
	SetMipmapNum(-1);
	image_setlegacyimageloading(false);

	//  Work area entity cursor (placeholder for instance of target expanded by 1.05 to make shell highligher)
	WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_CURSOROBJECT);

	MakeObjectPlane (  t.editor.objectstartindex+5,150,150  ); 
	XRotateObject (  t.editor.objectstartindex+5,90 );
	TextureObject (  t.editor.objectstartindex+5,g.editorimagesoffset+7 );
	SetObjectMask (  t.editor.objectstartindex+5, 1 );
	SetObjectTransparency (  t.editor.objectstartindex+5,2 );
	modifyplaneimagestrip(5,8,1);
	SetObjectCollisionOff (  t.editor.objectstartindex+5 );
	SetObjectLight (  t.editor.objectstartindex+5,0 );
	HideObject (  t.editor.objectstartindex+5 );
	OffsetLimb (  t.editor.objectstartindex+5,0,0,0,-1 );
	SetObjectEffect ( t.editor.objectstartindex+5, g.guishadereffectindex );

	//  cylinder to indicate resources in editor used (and warning)
	MakeObjectCylinder (  t.editor.objectstartindex+7,50 );
	SetObjectCollisionOff (  t.editor.objectstartindex+7 );
	SetObjectLight (  t.editor.objectstartindex+7,0 );
	DisableObjectZDepth (  t.editor.objectstartindex+7 );
	DisableObjectZRead (  t.editor.objectstartindex+7 );
	TextureObject (  t.editor.objectstartindex+7,g.editordrawlastimagesoffset+1 );
	LockObjectOn (  t.editor.objectstartindex+7 );
	ScaleObject (  t.editor.objectstartindex+7,2,0,2 );
	RotateObject (  t.editor.objectstartindex+7,0,0,90 );
	PositionObject (  t.editor.objectstartindex+7,0,-117.5,200 );
	SetObjectMask (  t.editor.objectstartindex+7, 1 );

	WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_NORMAL);

	//  Setup camera
	BackdropColor (  Rgb(0,0,0) );
	// `set camera range 10,10000 now set in _editor_overallfunctionality

	SetLightRange (  0,10000 );
	SetAmbientLight (  75 );
	SetCameraFOV(45); //PE: default 45 in wicked.
	SetAutoCamOff (  );

	//  PositionCamera (  )
	t.gridscale_f=((800/2)/8)/t.gridzoom_f;
	t.workareax=800 ; t.workareay=600;
	t.borderx_f=1024.0*50.0 ; t.cx_f=GGORIGIN_X;
	t.bordery_f=1024.0*50.0 ; t.cy_f=GGORIGIN_Z;
	editor_restoreeditcamera ( );
}

void editor_makeundergroundobj ( void )
{
	//  takes tobj,tobjx#,tobjy#,tobjz#
	t.tobjoffx_f=GetObjectCollisionCenterX(t.tobj);
	t.tobjoffy_f=GetObjectCollisionCenterY(t.tobj);
	t.tobjoffz_f=GetObjectCollisionCenterZ(t.tobj);
	t.tobjsizex_f=ObjectSizeX(t.tobj,1);
	t.tobjsizey_f=ObjectSizeY(t.tobj,1);
	t.tobjsizez_f=ObjectSizeZ(t.tobj,1);
	DeleteObject ( t.tobj );
	MakeObjectBox ( t.tobj,t.tobjsizex_f,t.tobjsizey_f,t.tobjsizez_f );
	//ColorObject ( t.tobj,Rgb(255,255,0) );
	PositionObject ( t.tobj,t.tobjx_f,t.tobjy_f,t.tobjz_f );
	OffsetLimb ( t.tobj,0,t.tobjoffx_f,t.tobjoffy_f,t.tobjoffz_f );
	DisableObjectZRead ( t.tobj );
	SetObjectWireframe ( t.tobj,1 );
	SetObjectMask ( t.tobj, 1 );
	SetObjectEffect ( t.tobj, g.guiwireframeshadereffectindex );
	SetObjectEmissive ( t.tobj, Rgb(255,255,0) );
}

void editor_restoreobjhighlightifnotrubberbanded ( int highlightingtentityobj )
{
	if ( highlightingtentityobj>0 ) 
	{
		if ( ObjectExist(highlightingtentityobj) == 1 ) 
		{
			bool bHighlightedFromRubberBandSelection = false;
			if ( g.entityrubberbandlist.size() > 0 )
			{
				for ( int i = 0; i < (int)g.entityrubberbandlist.size(); i++ )
				{
					int e = g.entityrubberbandlist[i].e;
					int tobj = t.entityelement[e].obj;
					if ( highlightingtentityobj == tobj )
						bHighlightedFromRubberBandSelection = true;
				}
			}
			if ( bHighlightedFromRubberBandSelection == false )
			{
				// dehighlight primary highlighted entity
				bool bValid = true;
				if (t.geditorhighlightingtentityID > 0)
				{
					int mi = t.entityelement[t.geditorhighlightingtentityID].bankindex;
					if (mi > 0 && t.entityprofile[mi].bIsDecal) bValid = false;
				}
				if (bValid)
				{
					SetAlphaMappingOn (highlightingtentityobj, 100);
				}
				// and also dehighlight any children that may have been highlighted as well
				if ( t.tstoreentityindexofprimaryhightlighted > 0 )
				{
					gridedit_clearentityrubberbandlist();
					t.tstoreentityindexofprimaryhightlighted = 0;
				}
				//PE: SetAlphaMappingOn will overwrite basecolor.w
				//PE: Restore org colors.
				sObject* pObject = g_ObjectList[highlightingtentityobj];
				if (pObject)
				{
					for (int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++)
					{
						WickedSetMeshNumber(iMesh);
						WickedCall_SetMeshMaterial(pObject->ppMeshList[iMesh], false);
					}
				}
				if (!bValid)
				{
					//Setup decal.
					SetupDecalObject(highlightingtentityobj, t.geditorhighlightingtentityID);
				}
			}
		}
	}
}

void editor_restoreentityhighlightobj ( void )
{
	if ( t.geditorhighlightingtentityobj>0 ) 
	{
		if (t.geditorhighlightingtentityID > 0)
		{
			WickedSetEntityId(t.entityelement[t.geditorhighlightingtentityID].bankindex);
			WickedSetElementId(t.geditorhighlightingtentityID);
		}
		editor_restoreobjhighlightifnotrubberbanded ( t.geditorhighlightingtentityobj );
		if (t.geditorhighlightingtentityID > 0)
		{
			WickedSetEntityId(-1);
			WickedSetElementId(0);
		}
		t.geditorhighlightingtentityobj=0;
	}
}

void editor_rec_checkifindexinparentchain ( int entityindex, bool* pbPartOfParentChildGroup )
{
	for ( int te = 1; te <= g.entityelementlist; te++ )
	{
		if ( t.entityelement[te].iHasParentIndex == entityindex && t.entityelement[te].obj > 0 )
		{
			*pbPartOfParentChildGroup = true;
			editor_rec_checkifindexinparentchain ( te, pbPartOfParentChildGroup );
		}
	}
}

void editor_rec_addchildrentorubberband ( int entityindex )
{
	for ( int te = 1; te <= g.entityelementlist; te++ )
	{
		if ( t.entityelement[te].iHasParentIndex == entityindex && t.entityelement[te].obj > 0 )
		{
			gridedit_addEntityToRubberBandHighlights ( te );
			editor_rec_addchildrentorubberband ( te );
		}
	}
}

void editor_refreshentitycursor ( void )
{
	//  new highligher for selected entities
	t.tentityobj=0 ; t.tentstaticmode=0;
	if ( t.gridentityobj>0 ) 
	{
		t.tentityobj=t.gridentityobj;
		t.tentstaticmode=t.gridentitystaticmode;
	}
	else
	{
		if ( t.tentitytoselect>0 ) 
		{
			if ( t.tentitytoselect <= ArrayCount(t.entityelement) ) 
			{
				t.tentityobj=t.entityelement[t.tentitytoselect].obj;
				t.tentstaticmode=t.entityelement[t.tentitytoselect].staticflag;
				t.ttentid=t.entityelement[t.tentitytoselect].bankindex;
				if ( t.entityprofile[t.ttentid].ismarker == 0 && t.entityprofile[t.ttentid].isebe == 0 ) 
				{
					if ( t.entityelement[t.tentitytoselect].isclone == 1 && t.entityelement[t.tentitytoselect].underground == 0 ) 
					{
						if ( t.entityelement[t.tentitytoselect].editorlock == 0 ) 
						{
							//  restore clone back to instance if no more entity lock
							t.tobj=t.tentityobj ; t.tte=t.tentitytoselect;
							entity_converttoinstance ( );
						}
					}
				}
			}
		}
	}
	if ( t.tentityobj>0 ) 
	{
		if ( ObjectExist(t.tentityobj) == 1 ) 
		{
			// do not reset if extracted and draggging parent/children around
			if ( t.gridentityextractedindex == 0 )
			{
				t.geditorhighlightingtentityID = t.tentitytoselect;
				t.geditorhighlightingtentityobj = t.tentityobj;
				editor_restoreentityhighlightobj();
			}

			// if obj is instance, and using entity_basic shader, this sets GlowIntensity constant
			int iAlphaHighlightCode = 100;
			if ( t.tentstaticmode == 0 ) 
			{
				if ( t.gridedit.autoflatten == 1 && t.gridentityobj>0 ) 
					iAlphaHighlightCode = 104;
				else
					iAlphaHighlightCode = 103;
			}
			else
			{
				if ( t.gridedit.autoflatten == 1 && t.gridentityobj>0 ) 
					iAlphaHighlightCode = 102;
				else
					iAlphaHighlightCode = 101;
			}
			SetAlphaMappingOn ( t.tentityobj, iAlphaHighlightCode );

			//PE: This overwrite out wicked basecolor.w

			// check if this entity is a parent to children, and highlight them too
			if ( t.tentitytoselect > 0 ) 
			{
				t.tstoreentityindexofprimaryhightlighted = t.tentitytoselect;
				editor_rec_addchildrentorubberband ( t.tentitytoselect );
			}

			// record primary entity object being highlighted
			t.geditorhighlightingtentityobj = t.tentityobj;
		}
	}
	else
	{
		if (t.geditorhighlightingtentityobj > 0)
		{
			int foundit = 0;
			for (int i = 1; i < t.entityelement.size(); i++)
			{
				if (t.entityelement[i].obj == t.geditorhighlightingtentityobj)
				{
					t.geditorhighlightingtentityID = i;
					editor_restoreentityhighlightobj();
					break;
				}
			}
		}
	}
}

void editor_hideall3d ( void )
{
	SetCurrentCamera (  0 );
	PositionCamera (  199999,99999,99999 );
	PointCamera (  199999,100999,99999 );
	if ( gbWelcomeSystemActive == false ) 
	{
		Sync ( ); Sync ( );
	}
}

void editor_restoreeditcamera ( void )
{
	// editor starting camera position - reset camera
	SetCurrentCamera (  0 );
	PositionCamera ( t.cx_f, 600*t.gridzoom_f, t.cy_f );
	PointCamera ( t.cx_f, 0, t.cy_f );
	SetCameraFOV(45); //PE: default 45 in wicked.
}

void editor_clearlibrary ( void )
{
	// Delete all libraries
	for ( t.tabs = 1; t.tabs <= 3; t.tabs++ )
	{
		SetFileMapDWORD ( 1, 534, t.tabs );
		SetFileMapDWORD ( 1, 542, 1 );
		SetEventAndWait ( 1 );
		while ( GetFileMapDWORD ( 1, 542 ) == 1 ) 
		{
			SetEventAndWait ( 1 );
		}
	}

	// ENTITY TAB
	t.tadd=1;

	// And create default NEW icons
	t.t1_s=t.strarr_s[347] ; t.t2_s="files\\editors\\gfx\\missing.bmp";
	SetFileMapDWORD ( 1, 508, t.tadd );
	SetFileMapString ( 1, 1000, cstr(g.mysystem.root_s+t.t2_s).Get() );
	SetFileMapString ( 1, 1256, t.t1_s.Get() );
	SetFileMapDWORD ( 1, 500, 1 );
	SetEventAndWait ( 1 );
	while ( GetFileMapDWORD(1, 500) == 1 ) 
	{
		SetEventAndWait ( 1 );
	}

	// MARKERS TAB
	t.tadd=2;

	//  Determine if extra ZONES included
	t.tstoryzoneincluded=25;
	if ( g.vrqcontrolmode != 0 )
		t.tstoryzoneincluded=23;
	// Default markers
	for ( t.tt = 0 ; t.tt <= t.tstoryzoneincluded; t.tt++ )
	{
		if ( g.vrqcontrolmode != 0 )
		{
			if (  t.tt == 0 ) { t.t1_s = t.strarr_s[349]  ; t.t2_s = "files\\entitybank\\_markers\\player start.bmp"; }
			if (  t.tt == 1 ) { t.t1_s = t.strarr_s[659]  ; t.t2_s = "files\\entitybank\\_markers\\multiplayer start.bmp"; }
			if (  t.tt == 2 ) { t.t1_s = t.strarr_s[351]  ; t.t2_s = "files\\entitybank\\_markers\\white light.bmp"; }
			if (  t.tt == 3 ) { t.t1_s = t.strarr_s[352]  ; t.t2_s = "files\\entitybank\\_markers\\red light.bmp"; }
			if (  t.tt == 4 ) { t.t1_s = t.strarr_s[353]  ; t.t2_s = "files\\entitybank\\_markers\\green light.bmp"; }
			if (  t.tt == 5 ) { t.t1_s = t.strarr_s[354]  ; t.t2_s = "files\\entitybank\\_markers\\blue light.bmp"; }
			if (  t.tt == 6 ) { t.t1_s = t.strarr_s[355]  ; t.t2_s = "files\\entitybank\\_markers\\yellow light.bmp"; }
			if (  t.tt == 7 ) { t.t1_s = t.strarr_s[356]  ; t.t2_s = "files\\entitybank\\_markers\\purple light.bmp"; }
			if (  t.tt == 8 ) { t.t1_s = t.strarr_s[357]  ; t.t2_s = "files\\entitybank\\_markers\\cyan light.bmp"; }
			if (  t.tt == 9 ) { t.t1_s = t.strarr_s[360]  ; t.t2_s = "files\\entitybank\\_markers\\win zone.bmp"; }
			if (  t.tt == 10 ) { t.t1_s = t.strarr_s[361]  ; t.t2_s = "files\\entitybank\\_markers\\trigger zone.bmp"; }
			if (  t.tt == 11 ) { t.t1_s = "Audio Zone"  ; t.t2_s = "files\\entitybank\\_markers\\audio zone.bmp"; }
			if (  t.tt == 12 ) { t.t1_s = "Video Zone"  ; t.t2_s = "files\\entitybank\\_markers\\video zone.bmp"; }
			if (  t.tt == 13 ) { t.t1_s = "Floor Zone"; t.t2_s = "files\\entitybank\\_markers\\floor zone.bmp"; }
			if (  t.tt == 14 ) { t.t1_s = "Image Zone"; t.t2_s = "files\\entitybank\\_markers\\image zone.bmp"; }
			if (  t.tt == 15 ) { t.t1_s = "Text Zone"; t.t2_s = "files\\entitybank\\_markers\\text zone.bmp"; }
			if (  t.tt == 16 ) { t.t1_s = "Ambience Zone"; t.t2_s = "files\\entitybank\\_markers\\ambience zone.bmp"; }
			if (  t.tt == 17 ) { t.t1_s = "White Spotlight"; t.t2_s = "files\\entitybank\\_markers\\white light spot.bmp"; }
			if (  t.tt == 18 ) { t.t1_s = "Red Spotlight"; t.t2_s = "files\\entitybank\\_markers\\red light spot.bmp"; }
			if (  t.tt == 19 ) { t.t1_s = "Green Spotlight"; t.t2_s = "files\\entitybank\\_markers\\green light spot.bmp"; }
			if (  t.tt == 20 ) { t.t1_s = "Blue Spotlight"; t.t2_s = "files\\entitybank\\_markers\\blue light spot.bmp"; }
			if (  t.tt == 21 ) { t.t1_s = "Yellow Spotlight"; t.t2_s = "files\\entitybank\\_markers\\yellow light spot.bmp"; }
			if (  t.tt == 22 ) { t.t1_s = "Purple Spotlight"; t.t2_s = "files\\entitybank\\_markers\\purple light spot.bmp"; }
			if (  t.tt == 23 ) { t.t1_s = "Cyan Spotlight"; t.t2_s = "files\\entitybank\\_markers\\cyan light spot.bmp"; }
		}
		else
		{
			if (  t.tt == 0 ) { t.t1_s = t.strarr_s[349]  ; t.t2_s = "files\\entitybank\\_markers\\player start.bmp"; }
			if (  t.tt == 1 ) { t.t1_s = t.strarr_s[350]  ; t.t2_s = "files\\entitybank\\_markers\\player checkpoint.bmp"; }
			if (  t.tt == 2 ) { t.t1_s = t.strarr_s[658]  ; t.t2_s = "files\\entitybank\\_markers\\cover zone.bmp"; }
			if (  t.tt == 3 ) { t.t1_s = t.strarr_s[659]  ; t.t2_s = "files\\entitybank\\_markers\\multiplayer start.bmp"; }
			if (  t.tt == 4 ) { t.t1_s = t.strarr_s[351]  ; t.t2_s = "files\\entitybank\\_markers\\white light.bmp"; }
			if (  t.tt == 5 ) { t.t1_s = t.strarr_s[352]  ; t.t2_s = "files\\entitybank\\_markers\\red light.bmp"; }
			if (  t.tt == 6 ) { t.t1_s = t.strarr_s[353]  ; t.t2_s = "files\\entitybank\\_markers\\green light.bmp"; }
			if (  t.tt == 7 ) { t.t1_s = t.strarr_s[354]  ; t.t2_s = "files\\entitybank\\_markers\\blue light.bmp"; }
			if (  t.tt == 8 ) { t.t1_s = t.strarr_s[355]  ; t.t2_s = "files\\entitybank\\_markers\\yellow light.bmp"; }
			if (  t.tt == 9 ) { t.t1_s = t.strarr_s[356]  ; t.t2_s = "files\\entitybank\\_markers\\purple light.bmp"; }
			if (  t.tt == 10 ) { t.t1_s = t.strarr_s[357]  ; t.t2_s = "files\\entitybank\\_markers\\cyan light.bmp"; }
			if (  t.tt == 11 ) { t.t1_s = t.strarr_s[360]  ; t.t2_s = "files\\entitybank\\_markers\\win zone.bmp"; }
			if (  t.tt == 12 ) { t.t1_s = t.strarr_s[361]  ; t.t2_s = "files\\entitybank\\_markers\\trigger zone.bmp"; }
			if (  t.tt == 13 ) { t.t1_s = t.strarr_s[362]  ; t.t2_s = "files\\entitybank\\_markers\\sound zone.bmp"; }
			if (  t.tt == 14 ) { t.t1_s = t.strarr_s[607]  ; t.t2_s = "files\\entitybank\\_markers\\story zone.bmp"; }
			if (  t.tt == 15 ) { t.t1_s = "Floor Zone"; t.t2_s = "files\\entitybank\\_markers\\floor zone.bmp"; }
			if (  t.tt == 16 ) { t.t1_s = "Image Zone"; t.t2_s = "files\\entitybank\\_markers\\image zone.bmp"; }
			if (  t.tt == 17 ) { t.t1_s = "Text Zone"; t.t2_s = "files\\entitybank\\_markers\\text zone.bmp"; }
			if (  t.tt == 18 ) { t.t1_s = "Ambience Zone"; t.t2_s = "files\\entitybank\\_markers\\ambience zone.bmp"; }
			if (  t.tt == 19 ) { t.t1_s = "White Spotlight"; t.t2_s = "files\\entitybank\\_markers\\white light spot.bmp"; }
			if (  t.tt == 20 ) { t.t1_s = "Red Spotlight"; t.t2_s = "files\\entitybank\\_markers\\red light spot.bmp"; }
			if (  t.tt == 21 ) { t.t1_s = "Green Spotlight"; t.t2_s = "files\\entitybank\\_markers\\green light spot.bmp"; }
			if (  t.tt == 22 ) { t.t1_s = "Blue Spotlight"; t.t2_s = "files\\entitybank\\_markers\\blue light spot.bmp"; }
			if (  t.tt == 23 ) { t.t1_s = "Yellow Spotlight"; t.t2_s = "files\\entitybank\\_markers\\yellow light spot.bmp"; }
			if (  t.tt == 24 ) { t.t1_s = "Purple Spotlight"; t.t2_s = "files\\entitybank\\_markers\\purple light spot.bmp"; }
			if (  t.tt == 25 ) { t.t1_s = "Cyan Spotlight"; t.t2_s = "files\\entitybank\\_markers\\cyan light spot.bmp"; }
		}
		SetFileMapDWORD (  1, 508, t.tadd );
		SetFileMapString (  1, 1000, cstr(g.mysystem.root_s+t.t2_s).Get() );
		SetFileMapString (  1, 1256, t.t1_s.Get() );
		SetFileMapDWORD (  1, 500, 1 );
		SetEventAndWait (  1 );
		while (  GetFileMapDWORD(1, 500) == 1 ) 
		{
			SetEventAndWait (  1 );
		}
	}

	//  actual entity names of the markers
	Dim ( t.markerentitybank_s, 30 );
	if ( g.vrqcontrolmode != 0 )
	{
		t.markerentitybank_s[1]="_markers\\player start.fpe";
		t.markerentitybank_s[2]="_markers\\multiplayer start.fpe";
		t.markerentitybank_s[3]="_markers\\white light.fpe";
		t.markerentitybank_s[4]="_markers\\red light.fpe";
		t.markerentitybank_s[5]="_markers\\green light.fpe";
		t.markerentitybank_s[6]="_markers\\blue light.fpe";
		t.markerentitybank_s[7]="_markers\\yellow light.fpe";
		t.markerentitybank_s[8]="_markers\\purple light.fpe";
		t.markerentitybank_s[9]="_markers\\cyan light.fpe";
		t.markerentitybank_s[10]="_markers\\win zone.fpe";
		t.markerentitybank_s[11]="_markers\\trigger zone.fpe";
		t.markerentitybank_s[12] = "_markers\\audio zone.fpe";
		t.markerentitybank_s[13] = "_markers\\video zone.fpe";
		t.markerentitybank_s[14] = "_markers\\floor zone.fpe";
		t.markerentitybank_s[15] = "_markers\\image zone.fpe";
		t.markerentitybank_s[16] = "_markers\\text zone.fpe";
		t.markerentitybank_s[17] = "_markers\\ambience zone.fpe";
		t.markerentitybank_s[18] = "_markers\\white light spot.fpe";
		t.markerentitybank_s[19] = "_markers\\red light spot.fpe";
		t.markerentitybank_s[20] = "_markers\\green light spot.fpe";
		t.markerentitybank_s[21] = "_markers\\blue light spot.fpe";
		t.markerentitybank_s[22] = "_markers\\yellow light spot.fpe";
		t.markerentitybank_s[23] = "_markers\\purple light spot.fpe";
		t.markerentitybank_s[24] = "_markers\\cyan light spot.fpe";
	}
	else
	{
		t.markerentitybank_s[1]="_markers\\player start.fpe";
		t.markerentitybank_s[2]="_markers\\player checkpoint.fpe";
		t.markerentitybank_s[3]="_markers\\cover zone.fpe";
		t.markerentitybank_s[4]="_markers\\multiplayer start.fpe";
		t.markerentitybank_s[5]="_markers\\white light.fpe";
		t.markerentitybank_s[6]="_markers\\red light.fpe";
		t.markerentitybank_s[7]="_markers\\green light.fpe";
		t.markerentitybank_s[8]="_markers\\blue light.fpe";
		t.markerentitybank_s[9]="_markers\\yellow light.fpe";
		t.markerentitybank_s[10]="_markers\\purple light.fpe";
		t.markerentitybank_s[11]="_markers\\cyan light.fpe";
		t.markerentitybank_s[12]="_markers\\win zone.fpe";
		t.markerentitybank_s[13]="_markers\\trigger zone.fpe";
		t.markerentitybank_s[14] = "_markers\\sound zone.fpe";
		t.markerentitybank_s[15] = "_markers\\story zone.fpe";
		t.markerentitybank_s[16] = "_markers\\floor zone.fpe";
		t.markerentitybank_s[17] = "_markers\\image zone.fpe";
		t.markerentitybank_s[18] = "_markers\\text zone.fpe";
		t.markerentitybank_s[19] = "_markers\\ambience zone.fpe";
		t.markerentitybank_s[20] = "_markers\\white light spot.fpe";
		t.markerentitybank_s[21] = "_markers\\red light spot.fpe";
		t.markerentitybank_s[22] = "_markers\\green light spot.fpe";
		t.markerentitybank_s[23] = "_markers\\blue light spot.fpe";
		t.markerentitybank_s[24] = "_markers\\yellow light spot.fpe";
		t.markerentitybank_s[25] = "_markers\\purple light spot.fpe";
		t.markerentitybank_s[26] = "_markers\\cyan light spot.fpe";
	}

	// only if EBE enabled
	if ( g.globals.hideebe == 0 )
	{
		// BUILDER TAB
		t.tadd=3;

		// set maximum to 999
		Dim ( t.ebebank_s, 999 );
		t.ebebankmax = 0;

		// Default builder tool icons
		for ( t.tt = 0; t.tt <= 6; t.tt++ )
		{
			if ( t.tt == 0 ) { t.t1_s = "Add New Site";		t.t2_s = "files\\ebebank\\_builder\\New Site.bmp"; }
			if ( t.tt == 1 ) { t.t1_s = "Cube";				t.t2_s = "files\\ebebank\\_builder\\Cube.bmp"; }
			if ( t.tt == 2 ) { t.t1_s = "Floor";			t.t2_s = "files\\ebebank\\_builder\\Floor.bmp"; }
			if ( t.tt == 3 ) { t.t1_s = "Wall";				t.t2_s = "files\\ebebank\\_builder\\Wall.bmp"; }
			if ( t.tt == 4 ) { t.t1_s = "Column";			t.t2_s = "files\\ebebank\\_builder\\Column.bmp"; }
			if ( t.tt == 5 ) { t.t1_s = "Row";				t.t2_s = "files\\ebebank\\_builder\\Row.bmp"; }
			if ( t.tt == 6 ) { t.t1_s = "Stairs";			t.t2_s = "files\\ebebank\\_builder\\Stairs.bmp"; }
			SetFileMapDWORD ( 1, 508, t.tadd );
			SetFileMapString ( 1, 1000, cstr(g.mysystem.root_s+t.t2_s).Get() );
			SetFileMapString ( 1, 1256, t.t1_s.Get() );
			SetFileMapDWORD ( 1, 500, 1 );
			SetEventAndWait ( 1 );
			while (  GetFileMapDWORD(1, 500) == 1 ) 
			{
				SetEventAndWait (  1 );
			}
		}
		t.ebebank_s[1]="..\\ebebank\\_builder\\New Site.fpe";
		t.ebebank_s[2]="ebebank\\_builder\\Cube.pfb";
		t.ebebank_s[3]="ebebank\\_builder\\Floor.pfb";
		t.ebebank_s[4]="ebebank\\_builder\\Wall.pfb";
		t.ebebank_s[5]="ebebank\\_builder\\Column.pfb";
		t.ebebank_s[6]="ebebank\\_builder\\Row.pfb";
		t.ebebank_s[7]="ebebank\\_builder\\Stairs.pfb";

		// Now scan for extra PFB files not part of default set
		int iFirstFreeSlot = 8;
		cstr pOld = GetDir();
		SetDir("ebebank");
		UnDim(t.filelist_s);
		buildfilelist("_builder", "");
		SetDir(pOld.Get());
		int iExtraPFBCount = 0;
		if (ArrayCount(t.filelist_s) > 0)
		{
			for (t.chkfile = 0; t.chkfile <= ArrayCount(t.filelist_s); t.chkfile++)
			{
				t.file_s = t.filelist_s[t.chkfile];
				if (t.file_s != "." && t.file_s != "..")
				{
					if (cstr(Lower(Right(t.file_s.Get(), 4))) == ".pfb")
					{
						// ignore items in default list
						bool bIgnore = false;
						for (int dl = 1; dl < iFirstFreeSlot; dl++)
						{
							LPSTR pThisOne = t.ebebank_s[dl].Get();
							char pNameOnly[256];
							strcpy(pNameOnly, "");
							for (int n = strlen(pThisOne) - 1; n > 0; n--)
							{
								if (pThisOne[n] == '\\' || pThisOne[n] == '/')
								{
									strcpy(pNameOnly, pThisOne + n + 1);
									break;
								}
							}
							if (stricmp(pNameOnly, t.file_s.Get()) == NULL)
								bIgnore = true;
						}
						if (bIgnore == false)
						{
							// add to list
							t.ebebank_s[iFirstFreeSlot + iExtraPFBCount] = cstr("ebebank\\_builder\\") + Left(t.file_s.Get(), Len(t.file_s.Get()));

							// next slot
							iExtraPFBCount++;
							if (iExtraPFBCount > 100) iExtraPFBCount = 100;
						}
					}
				}
			}
			t.strwork = ""; t.strwork = t.strwork + "total extra PFBs=" + Str(iExtraPFBCount);
			timestampactivity(0, t.strwork.Get());
		}
		//  Now sort list into alphabetical order
		for ( t.tgid1 = 0; t.tgid1 < iExtraPFBCount; t.tgid1++ )
		{
			for ( t.tgid2 = 0; t.tgid2 < iExtraPFBCount; t.tgid2++ )
			{
				if (  t.tgid1 != t.tgid2 ) 
				{
					t.tname1_s=Lower(t.ebebank_s[iFirstFreeSlot+t.tgid1].Get());
					t.tname2_s=Lower(t.ebebank_s[iFirstFreeSlot+t.tgid2].Get());
					if ( strlen( t.tname1_s.Get() ) > strlen( t.tname2_s.Get() ) ) 
					{
						//  smallest at top
						t.ebebank_s[iFirstFreeSlot+t.tgid1]=t.tname2_s;
						t.ebebank_s[iFirstFreeSlot+t.tgid2]=t.tname1_s;
					}
				}
			}
		}
		// add to library list
		for ( int n = 0; n < iExtraPFBCount; n++ )
		{
			// create BMP thumbnail
			t.file_s = t.ebebank_s[iFirstFreeSlot+n];
			LPSTR pThisOne = t.file_s.Get();
			char pNameOnly[256];
			strcpy ( pNameOnly, "" );
			for ( int n = strlen(pThisOne)-1; n > 0; n-- )
			{
				if ( pThisOne[n] == '\\' ||  pThisOne[n] == '/' )
				{
					strcpy ( pNameOnly, pThisOne + n + 1 );
					break;
				}
			}
			t.t1_s = Left(pNameOnly,Len(pNameOnly)-4);
			t.t2_s = cstr("files\\") + cstr(Left(t.file_s.Get(),Len(t.file_s.Get())-4)) + cstr(".bmp");
			SetFileMapDWORD ( 1, 508, t.tadd );
			SetFileMapString ( 1, 1000, cstr(g.mysystem.root_s+t.t2_s).Get() );
			SetFileMapString ( 1, 1256, t.t1_s.Get() );
			SetFileMapDWORD ( 1, 500, 1 );
			SetEventAndWait ( 1 );
			while (  GetFileMapDWORD(1, 500) == 1 ) 
			{
				SetEventAndWait (  1 );
			}
		}
		t.ebebankmax = 8 + iExtraPFBCount;
	}

	//  clear counters
	t.locallibraryentidmaster=0;
	t.locallibraryentindex=0;

	//  Ensure start with entity tab
	editor_leftpanelreset ( );
}

void editor_filllibrary ( void )
{
	//  Store place before adds
	SetEventAndWait (  1 );
	t.tstoredtabindex=GetFileMapDWORD( 1, 520 );

	//  Ensure entity list is up to date in library
	while ( t.locallibraryentidmaster<g.entidmaster ) 
	{
		//  only if not marker
		++t.locallibraryentidmaster;
		t.t2_s = t.entityprofileheader[t.locallibraryentidmaster].desc_s;

		// named EBE entities can be shown
		bool bShowEntityInLocalLibrary = true;
		if (t.entityprofile[t.locallibraryentidmaster].isebe != 0)
		{
			if ( stricmp ( t.t2_s.Get(), "new site" ) == NULL ) bShowEntityInLocalLibrary = false;
			if ( strnicmp ( t.t2_s.Get(), "ebe", 3 ) == NULL ) 
			{
				// are the characters after 'ebe' numbers?
				bool bIsNumber = false;
				LPSTR pEntName = t.t2_s.Get();
				for ( int n = 3; n < (int)strlen(pEntName); n++ )
				{
					if ( pEntName[n] >= '0' && pEntName[n] <= '9' )
						bIsNumber = true;
					else
					{
						bIsNumber = false;
						break;
					}
				}
				if ( bIsNumber == true )
				{
					bShowEntityInLocalLibrary = false;
				}
			}
			LPSTR pEntityBankFilename = t.entitybank_s[t.locallibraryentidmaster].Get();
			pEntityBankFilename += strlen(pEntityBankFilename)-4;
			if ( stricmp ( pEntityBankFilename, ".fpe" ) != NULL )
			{
				bShowEntityInLocalLibrary = false;
			}
		}
		if ( bShowEntityInLocalLibrary == true )
		{
			if ( t.entityprofile[t.locallibraryentidmaster].ismarker == 0 || t.entityprofile[t.locallibraryentidmaster].ismarker == 4 ) 
			{
				//  add to actual list
				t.ttext_s=t.entitybank_s[t.locallibraryentidmaster];
				t.tbitmap_s=cstr("files\\entitybank\\")+t.ttext_s;
				t.t1_s = ""; t.t1_s=t.t1_s + Left(t.tbitmap_s.Get(),Len(t.tbitmap_s.Get())-4)+".bmp";
				if (  FileExist( cstr(cstr("..\\")+t.t1_s).Get() ) == 0  )  t.t1_s = "files\\editors\\gfx\\missing.bmp";
				SetFileMapDWORD (  1, 508, 1 );
				SetFileMapString (  1, 1000, Left(cstr(g.mysystem.root_s+t.t1_s).Get(),254) );
				SetFileMapString (  1, 1256, Left(t.t2_s.Get(),254) );
				SetFileMapDWORD (  1, 500, 1 );
				SetEventAndWait (  1 );
				while (  GetFileMapDWORD(1, 500) == 1 ) 
				{
					SetEventAndWait (  1 );
				}

				//  add to internal list array
				++t.locallibraryentindex;
				Dim (  t.locallibraryent,t.locallibraryentindex  );
				t.locallibraryent[t.locallibraryentindex] = t.locallibraryentidmaster;
			}
		}
	}

	//  Restore place after adds
	SetFileMapDWORD (  1, 534, 1+t.tstoredtabindex );
	SetEventAndWait (  1 );
}

void editor_leftpanelreset ( void )
{
	// Reset to GetPoint ( to entity tab )
	SetFileMapDWORD (  1, 534, 1 );
	SetEventAndWait (  1 );
}

void editor_filemapdefaultinitfornew ( void )
{
	//  Open for some Defaults for Editor
	OpenFileMap (  1, "FPSEXCHANGE" );

	//  Marker Defaults
	g.entidmaster=0;

	//  filllibrary with segments and entities from default prefabs (temp as is above)
	editor_filllibrary ( );
	editor_leftpanelreset ( );
}

void editor_filemapinit ( void )
{
	// Open for some Defaults for Editor
	OpenFileMap (  1, "FPSEXCHANGE" );
	// Set default mouse position and visibility
	SetFileMapDWORD (  1, 0, 400 );
	SetFileMapDWORD (  1, 4, 300 );
	SetEventAndWait (  1 );

	//  Each selection tab needs a NEW icon
	editor_clearlibrary ( );
	editor_filllibrary ( );
	editor_leftpanelreset ( );
}

void editor_loadcfg (bool bFromFPM)
{
	// Load existing config file
	cstr cfgfile_s = g.mysystem.editorsGridedit_s + "cfg.cfg";
	if (bFromFPM)
	{
		cfgfile_s = g.mysystem.levelBankTestMap_s + "cfg.cfg";
		if (FileExist(cfgfile_s.Get()) == 0)
		{
			cfgfile_s = g.mysystem.editorsGridedit_s + "cfg.cfg";
		}
	}
	if ( FileExist(cfgfile_s.Get()) == 1 ) 
	{
		OpenToRead (  1,cfgfile_s.Get() );
		t.cx_f = ReadFloat ( 1 );
		t.cy_f = ReadFloat ( 1 );
		t.gridzoom_f = ReadFloat ( 1 );
		t.gridlayer = ReadLong ( 1 );
		t.nogridsmart = ReadLong ( 1 );
		t.grideditartmode = ReadLong ( 1 );

		// LB: modes 3 and 4 are view-modes (should not restore into these, no way out!!)
		int iTestGridEditSelect = ReadLong ( 1 );
		if (iTestGridEditSelect == 3 || iTestGridEditSelect == 4)
		{
			t.grideditselect = 0;
		}
		else
		{
			t.grideditselect = iTestGridEditSelect;
		}

		//  Project (only need project name if skipping FPM=using temp.fpm)
		t.temp_s = ReadString ( 1 ); if (  t.skipfpmloading == 1  )  g.projectfilename_s = t.temp_s;
		g.currentFPG_s = ReadString ( 1 );

		//  Shroud Settings
		t.a = ReadLong ( 1 );
		g.gridlayershowsingle = ReadLong ( 1 );

		//PE: Restore all camera settings.
		if (bFromFPM)
		{
			char *tmp;
			tmp = ReadString(1);
			if (tmp && tmp[0] == 'V' && tmp[1] == '2')
			{
				t.editorfreeflight.mode = ReadLong(1);
				float fTmp1 = ReadFloat(1);
				float fTmp2 = ReadFloat(1);
				float fTmp3 = ReadFloat(1);
				//PE: Double check if we have some default values and something is wrong.
				if (fTmp1 != 0.0 && fTmp2 != 0.0 && fTmp3 != 0.0)
				{
					t.editorfreeflight.c.x_f = fTmp1;
					t.editorfreeflight.c.y_f = fTmp2;
					t.editorfreeflight.c.z_f = fTmp3;
					t.cx_f = ReadFloat(1);
					t.cy_f = ReadFloat(1);
					t.editorfreeflight.c.angx_f = ReadFloat(1);
					t.editorfreeflight.c.angy_f = ReadFloat(1);
					//PE: Also update t.editorfreeflight.s if any camera animation is starting.
					t.editorfreeflight.s = t.editorfreeflight.c;
				}
			}
			//PE: In wicked after loading a new fpm. we need some frames before terrain height is ready.
			iDelayedCameraRestore = 240; //It can take some time before full terrain is regenerated, so 4 sec.
		}
		CloseFile (  1 );
	}

	//  Reset editor slicing for now
	g.gridlayershowsingle=0;

	//  Update editor settings
	editor_refresheditmarkers ( );
	t.refreshgrideditcursor=1;
	t.updatezoom=1;

	//  Current project name stored for next time
	t.currentprojectfilename_s=g.projectfilename_s;
	return;
}

void editor_savecfg (char *filename)
{
	// Delete config file
	t.strwork = g.mysystem.editorsGridedit_s + "cfg.cfg";
	if (filename)
	{
		t.strwork = filename;
	}

	if ( FileOpen(1) == 1) CloseFile(1); //PE: To make sure, was missing a cfg.cfg file in fpm so...
	if ( FileExist( t.strwork.Get() ) == 1  )  DeleteAFile ( t.strwork.Get() );

	//  Save config file
	OpenToWrite (  1, t.strwork.Get() );

	//  Current Camera Position
	if (  t.editorfreeflight.mode == 1 ) 
	{
		//  when save while in free flight mode, use present location
		t.a_f=t.editorfreeflight.c.x_f ; WriteFloat (  1,t.a_f );
		t.a_f=t.editorfreeflight.c.z_f ; WriteFloat (  1,t.a_f );
	}
	else
	{
		WriteFloat (  1,t.cx_f );
		WriteFloat (  1,t.cy_f );
	}
	WriteFloat (  1,t.gridzoom_f );
	WriteLong (  1,t.gridlayer );

	//  Edit Vars
	WriteLong (  1,t.nogridsmart );
	WriteLong (  1,t.grideditartmode );
	WriteLong (  1,t.grideditselect );

	//  Project
	WriteString (  1,g.projectfilename_s.Get() );
	WriteString (  1,g.currentFPG_s.Get() );

	//  Shroud Settings
	WriteLong (  1,g.shroudsize );
	WriteLong (  1,g.gridlayershowsingle );
	WriteString(1, "V2");

	//PE: Write out all camera settings.
	WriteLong(1, t.editorfreeflight.mode);
	WriteFloat(1, t.editorfreeflight.c.x_f);
	WriteFloat(1, t.editorfreeflight.c.y_f);
	WriteFloat(1, t.editorfreeflight.c.z_f);
	WriteFloat(1, t.cx_f);
	WriteFloat(1, t.cy_f);
	WriteFloat(1, t.editorfreeflight.c.angx_f);
	WriteFloat(1, t.editorfreeflight.c.angy_f);

	// finish
	CloseFile (  1 );
}

void editor_constructionselection ( void )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	if ( t.inputsys.constructselection>0 )
	{
		//  SINGLE ENTITY
		if ( t.grideditselect == 5 ) 
		{
			if ( t.inputsys.constructselection <= g.entidmaster ) 
			{
				CloseDownEditorProperties();

				//  first cancel any widget that might be opened
				widget_switchoff ( );

				//PE: Somebody removed this line in Classic ???? Nothing worked ????
				t.gridentity = t.inputsys.constructselection;

				// use custom grideleprof if from smart object
				bool bEleProfFromSmartObject = false;

				// remove any entity group rubber band highlighting
				t.gridentity = t.inputsys.constructselection;
				int iFromGroupEntityID = 0;
				if (t.entityprofile[t.gridentity].groupreference != -1)
				{
					// look for group that matches the group object entity FPE name
					extern int GetGroupIndexFromName (cstr sLookFor);
					cstr sLookFor = cstr("entitybank\\") + t.entitybank_s[t.gridentity];
					int iParentGroupID = GetGroupIndexFromName(sLookFor);
					if (iParentGroupID != -1)
					{
						// and duplicate from that parent group
						iFromGroupEntityID = DuplicateFromListToCursor(vEntityGroupList[iParentGroupID], false, -1);

						//PE: Keep scale rot when setup from a group.
						if (iFromGroupEntityID > 0)
						{
							bRotScaleAlreadyUpdated = true;
							if (g.entityrubberbandlist.size() > 0)
							{
								int e = g.entityrubberbandlist[0].e;
								if (e > 0)
								{
									t.grideleprof = t.entityelement[e].eleprof;
									bEleProfFromSmartObject = true;
								}
							}
						}

						// smart object game elements are always hidden at first
						gridedit_setsmartobjectvisibilityinrubberband(false);
					}
					else
					{
						// strangely cannot find group for this parent smart object!!
						gridedit_clearentityrubberbandlist();
					}
				}
				else
				{
					// if not a group smart object
					gridedit_clearentityrubberbandlist();
				}

				iLastEntityOnCursor = 0;

				// the entity ID we are adding
				if ( t.entityprofile[t.gridentity].isebe > 0 )
				{
					// create unique entid and go to entity placement mode
					char pEBEFile[512];
					strcpy ( pEBEFile, t.entitybank_s[t.gridentity].Get());
					t.addentityfile_s = cstr(Left(pEBEFile,strlen(pEBEFile)-4)) + cstr(".fpe");

					CloseDownEditorProperties();

					// Work out EBE file and check if it exists
					char pFinalPathAndFile[1024];
					sprintf ( pFinalPathAndFile, "entitybank%s.ebe", Left(pEBEFile,strlen(pEBEFile)-4) );
					if ( FileExist ( pFinalPathAndFile ) )
					{
						// by creating one unique to the level, we can save our temp changes to it
						entity_adduniqueentity ( true );
						t.gridentity = t.entid;

						// name only
						char pNameOnly[256];
						strcpy ( pNameOnly, "" );
						for ( int n = strlen(pEBEFile)-1; n > 0; n-- )
						{
							if ( pEBEFile[n] == '\\' ||  pEBEFile[n] == '/' )
							{
								strcpy ( pNameOnly, pEBEFile + n + 1 );
								break;
							}
						}
						t.t1_s = Left(pNameOnly,Len(pNameOnly)-4);

						// give it a unique name
						t.entitybank_s[t.entid] = t.t1_s;
						t.entityprofileheader[t.entid].desc_s = t.t1_s;

						// load EBE data into entityID
						ebe_load_ebefile ( pFinalPathAndFile, t.entid );

						// get path only
						char pFinalPathOnly[1024];
						strcpy ( pFinalPathOnly, pFinalPathAndFile );
						for ( int n = strlen(pFinalPathAndFile); n > 0; n-- )
						{
							if ( pFinalPathAndFile[n] == '\\' || pFinalPathAndFile[n] == '/' )
							{
								pFinalPathOnly[n+1] = 0;
								break;
							}
						}

						// copy unique texture into levelbank\testmap so EDIT can copy over to ebebank
						cstr sUniqueFilename = t.entityprofile[t.entid].texd_s;
						sUniqueFilename = cstr(Left(sUniqueFilename.Get(),strlen(sUniqueFilename.Get())-6));
						cstr sDDSSourceFile = cstr(pFinalPathOnly) + sUniqueFilename + cstr("_D.dds");
						cstr sDDSFile = g.mysystem.levelBankTestMap_s + sUniqueFilename + cstr("_D.dds");
						if ( FileExist(sDDSFile.Get()) == 1 ) DeleteAFile ( sDDSFile.Get() );
						CopyFileA ( sDDSSourceFile.Get(), sDDSFile.Get(), FALSE );
						sDDSSourceFile = cstr(pFinalPathOnly) + sUniqueFilename + cstr("_N.dds");
						sDDSFile = g.mysystem.levelBankTestMap_s + sUniqueFilename + cstr("_N.dds");
						if ( FileExist(sDDSFile.Get()) == 1 ) DeleteAFile ( sDDSFile.Get() );
						CopyFileA ( sDDSSourceFile.Get(), sDDSFile.Get(), FALSE );
						sDDSSourceFile = cstr(pFinalPathOnly) + sUniqueFilename + cstr("_S.dds");
						sDDSFile = g.mysystem.levelBankTestMap_s + sUniqueFilename + cstr("_S.dds");
						if ( FileExist(sDDSFile.Get()) == 1 ) DeleteAFile ( sDDSFile.Get() );
						CopyFileA ( sDDSSourceFile.Get(), sDDSFile.Get(), FALSE );
					}
					else
					{
						// EBE not present, which means user protected it (not an editable EBE any more)
						if ( stricmp ( pEBEFile, "..\\ebebank\\_builder\\New Site.fpe" ) != NULL )
						{
							//New site is called EBE? 
							//if (!(pEBEFile[0] == 'E' && pEBEFile[1] == 'B' && pEBEFile[2] == 'E'))
							if(t.entityprofile[t.gridentity].model_s != "New Site.dbo" )  //Better way.
							{
								// except New Site of course
								t.entityprofile[t.gridentity].isebe = 0;
							}
						}
					}
				}
				//  select entity profile and start orientation
				t.gridedit.autoflatten=t.entityprofile[t.gridentity].autoflatten;
				t.inputsys.dragoffsetx_f=0;
				t.inputsys.dragoffsety_f=0;
				fHitPointX = 0;
				fHitPointY = HITPOINTYSTARTPOS;
				fHitPointZ = 0;
				fHitOffsetX = 0;
				fHitOffsetY = 0;
				fHitOffsetZ = 0;
				g_fSpecialDragInYAdjustment = 0.0f;
				// LB: these can be uninitialised, but we need these filled so the plane can be under the cursor initially
				t.gridentityposx_f = t.inputsys.localx_f;
				t.gridentityposy_f = t.inputsys.localcurrentterrainheight_f;
				t.gridentityposz_f = t.inputsys.localy_f;
				g_bHoldGridEntityPosWhenManaged = true;
				g_fHoldGridEntityPosX = t.gridentityposx_f;
				g_fHoldGridEntityPosY = t.gridentityposy_f;
				g_fHoldGridEntityPosZ = t.gridentityposz_f;
				t.gridentityposoffground=0;

				if ( t.entityprofile[t.gridentity].dontfindfloor != 0 )
				{
					// can set entity to initially ignore floor finder
					t.gridentityusingsoftauto = 0;
				}
				else
				{
					t.gridentityusingsoftauto = 1;
				}
				// MAX handles its own positioning system
				{
					t.gridentityautofind=0;
				}

				//PE: We get some flicker when moving objects around, when over object sometimes it use terrainheight.
				//PE: @Lee Think terrain is inside Wicked Pick so should not be needed. if not just remove this :)
				t.gridentityusingsoftauto = 0;

				t.gridentityeditorfixed=0;
				if (!bRotScaleAlreadyUpdated)
				{
					if (t.entityprofile[t.gridentity].ischaracter != 0)
					{
						// if character, always face the camera
						float fAngleToFaceCamera = CameraAngleY(0) + 180.0f;
						t.entityprofile[t.gridentity].roty = fAngleToFaceCamera;
					}
					t.gridentityrotatex_f = t.entityprofile[t.gridentity].rotx;
					t.gridentityrotatey_f = t.entityprofile[t.gridentity].roty;
					t.gridentityrotatez_f = t.entityprofile[t.gridentity].rotz;
					t.gridentityrotatequatmode = 0;
					t.gridentityrotatequatx_f = 0;
					t.gridentityrotatequaty_f = 0;
					t.gridentityrotatequatz_f = 0;
					t.gridentityrotatequatw_f = 1;
					t.gridentityscalex_f = t.entityprofile[t.gridentity].scale;
					t.gridentityscaley_f = t.entityprofile[t.gridentity].scale;
					t.gridentityscalez_f = t.entityprofile[t.gridentity].scale;
				}
				bRotScaleAlreadyUpdated = false;
				t.ttrygridentitystaticmode=t.entityprofile[t.gridentity].defaultstatic;
				t.ttrygridentity=t.gridentity ; editor_validatestaticmode ( );
				//  Ensure editor zoom refreshes
				t.updatezoom=1;
				//  fill new selection with defaults
				if (bEleProfFromSmartObject == false)
				{
					// only if not already populated from smart object element above
					t.sentid = t.entid; t.entid = t.gridentity;
					entity_fillgrideleproffromprofile();
					t.grideleprof.bUseFPESettings = true; //PE: New added always use bUseFPESettings.
					t.grideleprof.isProjectGlobal = false;

					t.entid = t.sentid;
				}
				t.grideleproflastname_s=t.grideleprof.name_s;
				//  marker types?
				if ( t.entityprofile[t.gridentity].ismarker == 1 && t.entityprofile[t.gridentity].lives != -1 ) 
				{
					//  selecting new player start marker resets tweakables
					physics_inittweakables ( );
				}
				if ( t.entityprofile[t.gridentity].ismarker == 3 || t.entityprofile[t.gridentity].ismarker == 6 || t.entityprofile[t.gridentity].ismarker == 8 ) 
				{
					//  trigger zone marker(3) or checkpoint marker(6) or floor zone marker(8)
					//  trigger zone has a waypoint zone companion
					if ( t.entityprofile[t.gridentity].ismarker == 8 ) 
						t.waypointeditstyle = 3; // navmeshzone
					else
						t.waypointeditstyle = 2; // normalzone
					t.waypointeditstylecolor=t.entityprofile[t.gridentity].trigger.stylecolor;
					t.waypointeditentity=0;
					t.mx_f=t.cx_f ; t.mz_f=t.cy_f;
					if (  t.terrain.TerrainID>0 ) 
					{
						g.waypointeditheight_f=BT_GetGroundHeight(t.terrain.TerrainID,t.mx_f,t.mz_f);
					}
					else
					{
						g.waypointeditheight_f=g.gdefaultterrainheight;
					}
					waypoint_createnew ( );
					t.grideleprof.trigger.waypointzoneindex=t.waypointindex;
				}
			}
		}

		//  In case new 'shader' associated with new entity, refresh just in case (i.e. first entity)
		visuals_justshaderupdate ( );

		//  Construction complete
		t.inputsys.constructselection = 0;
	}
}

void editor_validatestaticmode ( void )
{
	// receives ttrygridentitystaticmode,ttrygridentity
	if ( t.ttrygridentity>0 ) 
	{
		t.gridentitystaticmode=t.ttrygridentitystaticmode;
		bool bSomeShadersForceDynamicMode = false;
		if ( strcmp ( Lower(Right(t.entityprofile[t.ttrygridentity].effect_s.Get(),18)) , "character_basic.fx" ) == 0 ) bSomeShadersForceDynamicMode = true;
		if ( strcmp ( Lower(Right(t.entityprofile[t.ttrygridentity].effect_s.Get(),14)) , "treea_basic.fx" ) == 0 ) bSomeShadersForceDynamicMode = true;
		if ( bSomeShadersForceDynamicMode == true )
		{
			if ( ObjectExist(g.entitybankoffset+t.ttrygridentity) == 1 ) 
			{
				if ( GetNumberOfFrames(g.entitybankoffset+t.ttrygridentity)>0 ) 
				{
					t.gridentitystaticmode=0;
				}
			}
		}

		if (t.entityprofile[t.ttrygridentity].ischaracter)
		{
			// Possible fix for characters appearing static.
			t.gridentitystaticmode = 0;
		}
	}
}

void editor_overallfunctionality ( void )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	//  Restore current grid view
	if (  t.inputsys.doautozoomview == 1 ) { t.inputsys.doautozoomview = 0  ; t.inputsys.dozoomview = 1; }
	if (  t.inputsys.dozoomview == 1 ) 
	{
		if (  t.cameraviewmode == 2 ) 
		{
			//  mouselook mode off
			OpenFileMap (  1, "FPSEXCHANGE" );
			SetFileMapDWORD (  1, 48, 0 );
			SetEventAndWait (  1 );
			//  re-enable icons
			editor_enableafterzoom ( );
			//  end zoom mode
			t.grideditselect=t.stgrideditselect  ; editor_refresheditmarkers ( );
			t.inputsys.dozoomview=0;
			t.cameraviewmode=0;
		}
	}

	//  Switch to zoom view
	if (  t.inputsys.dozoomview == 1 ) 
	{
		if (  t.cameraviewmode == 0 ) 
		{
			//  Set camera to track with close-up
			t.stgrideditselect=t.grideditselect;
			t.cameraviewmode = 2;

			//  Mode - Zoom In View
			t.grideditselect=4 ; editor_refresheditmarkers ( );
			t.updatezoom=1;
		}
	}

	//  Get terrain height reading at cursor
	t.ttterrheighthere_f=BT_GetGroundHeight(t.terrain.TerrainID,t.cx_f,t.cy_f);
	
	//  ensure zoom never penetrates terrain
	if (  t.updatezoom == 1 || t.inputsys.mclick != 0 ) 
	{
		if (  (600.0*t.gridzoom_f)<(t.ttterrheighthere_f+100) ) 
		{
			t.gridzoom_f=(t.ttterrheighthere_f+100)/600.0;
		}
	}

	//  Recalculate zoom scale for editing
	if (  t.updatezoom == 1 ) 
	{

		//  grid scale for camera cursor location and zoom
		t.gridscale_f=((800/2)/8)/t.gridzoom_f;
		t.inputsys.keypress=1;
		t.updatezoom=0;

		//  gridlayershowsingle creates an alpha slice in entity shaders
		t.gridnearcameraclip=-1;
		if (  t.grideditselect != 4 ) 
		{
			if (  g.gridlayershowsingle == 1 ) 
			{
				t.gridnearcameraclip=t.clipheight_f;
			}
		}

		//  modulate shadow strength based on distance
		t.tcamrange_f=((600.0*t.gridzoom_f)+1000);
		t.toldvisualsshadowmode=t.visuals.shadowmode;
		if (  t.tcamrange_f<4000 ) 
		{
			t.visuals.shadowmode=100;
		}
		else
		{
			if (  t.tcamrange_f<6000 ) 
			{
				t.visuals.shadowmode=(6000-t.tcamrange_f)/20.0;
			}
			else
			{
				t.visuals.shadowmode=0;
			}
		}
		if (  t.toldvisualsshadowmode != t.visuals.shadowmode ) 
		{
			visuals_justshaderupdate ( );
		}

		//  adjust clipping range of camera to match
		editor_refreshcamerarange ( );

		//  Ensure the slicing clip does not go
		if (  t.gridnearcameraclip == -1 ) 
		{
			t.gridtrueslicey_f=CameraPositionY(0);
		}
		else
		{
			t.gridtrueslicey_f=t.gridnearcameraclip;
		}

		//  feed alpha slicing height into all entity shaders
		if (  g.effectbankmax>0 ) 
		{
			for ( t.t = 1 ; t.t<=  g.effectbankmax; t.t++ )
			{
				t.effectid=g.effectbankoffset+t.t;
				if (  GetEffectExist(t.effectid) == 1 ) 
				{
					if (  t.gridnearcameraclip == -1 ) 
					{
						SetVector4 ( g.terrainvectorindex, 500000, 1, 0, 0 );
					}
					else
					{
						SetVector4 ( g.terrainvectorindex, t.gridtrueslicey_f, 1, 0, 0 );
					}
					SetEffectConstantV (  t.effectid,"EntityEffectControl",g.terrainvectorindex );
				}
			}
		}

	}

	//  use intersect test to find ground/wall and drop entity onto it
	if ( t.inputsys.k_s != "l" ) 
	{
		// but not if holding L key to link entity to a new parent
		if ( t.gridentitysurfacesnap == 1 )
		{
			// no need to find entity, surfacesnap already found best 3D coordinate
		}
		else
		{
			if (t.gridentitydroptoground >= 1 && t.gridentitydroptoground <= 2)
			{
				//PE: Need rubberband support here.
				float fdiff = t.gridentityposy_f;
				t.thardauto = 1; editor_findentityground();
				if (t.gridentityposoffground == 0)
				{
					float ftmp = BT_GetGroundHeight(t.terrain.TerrainID, t.gridentityposx_f, t.gridentityposz_f);
					t.gridentityposy_f = ftmp;
					ApplyPivotToGridEntity(); //PE: Apply pivot here.
				}
				//Only drop one time if in vertical move mode.
				if (iObjectMoveMode == 1)
				{
					if (fHitOffsetY != 0)
					{
						fdiff = fdiff -t.gridentityposy_f;
						fHitOffsetY += fdiff;
					}
					t.gridentityautofind = 0;
					t.gridentityposoffground = 1; //Dont proceed updating Y.
				}
				t.gridentitydroptoground=0;
			}
			else
			{
				t.thardauto=0 ; editor_findentityground ( );
			}
		}
	}

	//  Change layer show mode
	if (  t.inputsys.dosinglelayer == 1 ) 
	{
		g.gridlayershowsingle=g.gridlayershowsingle+1;
		if (  g.gridlayershowsingle>1  )  g.gridlayershowsingle = 0;
		t.updatezoom=1;
	}

	//  ensure assigned third person char object stays with start marker
	if (  t.playercontrol.thirdperson.enabled == 1 ) 
	{
		t.tobj=t.entityelement[t.playercontrol.thirdperson.charactere].obj;
		if (  t.tobj>0 ) 
		{
			if (  ObjectExist(t.tobj) == 1 ) 
			{
				if (  t.gridentity>0 && t.entityprofile[t.gridentity].ismarker == 1 ) 
				{
					//  moving start marker
					t.tstmrkobj=t.gridentityobj;
				}
				else
				{
					//  update char on start marker entity
					t.tstmrke=t.playercontrol.thirdperson.startmarkere;
					t.tstmrkobj=t.entityelement[t.tstmrke].obj;
				}
				if (  t.tstmrkobj>0 ) 
				{
					if (  ObjectExist(t.tstmrkobj) == 1 ) 
					{
						PositionObject (  t.tobj,ObjectPositionX(t.tstmrkobj),ObjectPositionY(t.tstmrkobj),ObjectPositionZ(t.tstmrkobj) );
						RotateObject (  t.tobj,ObjectAngleX(t.tstmrkobj),ObjectAngleY(t.tstmrkobj),ObjectAngleZ(t.tstmrkobj) );
					}
				}
				MoveObject (  t.tobj,-35 );
				if (  t.tstmrkobj>0 ) 
				{
					if (  ObjectExist(t.tstmrkobj) == 1 ) 
					{
						EnableObjectZDepth (  t.tstmrkobj );
						EnableObjectZWrite (  t.tstmrkobj );
						EnableObjectZRead (  t.tstmrkobj );
					}
				}
			}
		}
	}
}

void editor_refreshcamerarange ( void )
{
	t.tcamneardistance_f=CameraPositionY(0)/500.0;
	if ( t.tcamneardistance_f < 10.0  ) t.tcamneardistance_f = 10.0;
	if ( t.widget.activeObject > 0 )
	{
		// 011215 - except when widget shown, we need min distance to avoid clipping widget
		if ( t.tcamneardistance_f > 30.0f ) 
		{
			// to avoid water plane clipping, move water plane away from terrain plate incrementally
			t.terrain.waterlineyadjustforclip_f = (t.tcamneardistance_f-30.0f) * 5;
			t.tcamneardistance_f = 30.0f;
		}
	}
	else
	{
		t.terrain.waterlineyadjustforclip_f = 0.0f;
	}
	if (  t.editorfreeflight.mode == 1 ) 
	{
		// free flight FULL camera distance
		SetCameraRange ( t.tcamneardistance_f, DEFAULT_FAR_PLANE );
	}
	else
	{
		//  top down camera distance
		SetCameraRange( t.tcamneardistance_f, DEFAULT_FAR_PLANE );
	}
}

void editor_mainfunctionality ( void )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	//  Rotation of entity
	if (  t.grideditselect == 5 ) 
	{
		bool bAllowRotate = true;
		if (t.widget.pickedEntityIndex > 0 && t.entityelement[t.widget.pickedEntityIndex].editorlock == 1)
			bAllowRotate = false;

		//  do not rotate light or trigger entity
		//PE: Allow light rotation rem: t.entityprofile[t.gridentity].ismarker != 2 &&  t.entityprofile[t.gridentity].ismarker != 3
		if ( bAllowRotate && t.entityprofile[t.gridentity].ismarker != 3 )
		{
			if (  t.inputsys.keyshift == 1 ) 
			{
				t.tspeedofrot_f=10.0 ; t.inputsys.keypress=0;
			}
			else
			{
				if (  t.inputsys.keycontrol == 1 ) 
				{
					t.tspeedofrot_f=1.0;
				}
				else
				{
					t.tspeedofrot_f=45.0;
				}
			}
			//PE: Prefer gridentity rotation. as we can now have both active at the same time.
			//PE: We dont need t.widget.pickedObject != 0 && to control widget.
			if(t.widget.pickedEntityIndex > 0 && t.gridentity == 0)
			{
				// Rotation control of widget controlled entity
				if ( t.inputsys.domodeterrain == 0 && t.inputsys.domodeentity == 0 ) 
				{
					if ( t.inputsys.dorotation == 1 || (t.inputsys.doentityrotate >= 1 && t.inputsys.doentityrotate <= 6) ||
						 t.inputsys.keyreturn == 1 || t.inputsys.kscancode == 201 || t.inputsys.kscancode == 209 )
					{
						//PE: Make sure all groups are selected.
						int group = isEntityInGroupList(t.widget.pickedEntityIndex);
						if (group >= 0)
						{
							//Add all groups with entity to rubberband.
							CheckGroupListForRubberbandSelections(t.widget.pickedEntityIndex);
						}
						else if (g.entityrubberbandlist.size() > 0)
						{
							//Make sure all groups is selected from within rubberband selecting.
							for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
							{
								int e = g.entityrubberbandlist[i].e;
								CheckGroupListForRubberbandSelections(e);
							}
						}
					}
					//  avoid interference from terrain/entity mode change
					GGQUATERNION quatRotationEvent = { 0,0,0,0 };
					float fMoveAngX = 0.0f;
					float fMoveAngY = 0.0f;
					float fMoveAngZ = 0.0f;
					float fStoreOrigYAngle = t.entityelement[t.widget.pickedEntityIndex].ry;
					bool bRotateObjectFromKeyPress = false;
					if (t.inputsys.dorotation == 1) 
					{ 
						fMoveAngY = fMoveAngY + t.tspeedofrot_f; 
						bRotateObjectFromKeyPress = true; 
					}
					if (t.inputsys.doentityrotate == 1) { fMoveAngX = fMoveAngX - t.tspeedofrot_f; bRotateObjectFromKeyPress = true; }
					if (t.inputsys.doentityrotate == 2) { fMoveAngX = fMoveAngX + t.tspeedofrot_f; bRotateObjectFromKeyPress = true; }
					if (t.inputsys.doentityrotate == 3) 
					{ 
						fMoveAngY = fMoveAngY - t.tspeedofrot_f; 
						bRotateObjectFromKeyPress = true; 
					}
					if (t.inputsys.doentityrotate == 4) 
					{ 
						fMoveAngY = fMoveAngY + t.tspeedofrot_f; 
						bRotateObjectFromKeyPress = true; 
					}
					if (t.inputsys.doentityrotate == 5) { fMoveAngZ = fMoveAngZ - t.tspeedofrot_f; bRotateObjectFromKeyPress = true; }
					if (t.inputsys.doentityrotate == 6) { fMoveAngZ = fMoveAngZ + t.tspeedofrot_f; bRotateObjectFromKeyPress = true; }

					// special case for characters, only rotate Y angle
					if (t.widget.pickedEntityIndex > 0)
					{
						int entidcheck = t.entityelement[t.widget.pickedEntityIndex].bankindex;
						if (t.entityprofile[entidcheck].ischaracter == 1)
						{
							fMoveAngX = 0.0f;
							fMoveAngZ = 0.0f;
						}
					}
					// ready for quat rot
					static bool bStartedKeyboardRotation = false;
					static std::vector<std::array<float, 3>> prevRotations;
					static std::vector<std::array<float, 4>> prevQuatRotations;
					static std::vector<std::array<int, 1>> prevQuatRotationsMode;
					static std::vector <std::array<float, 3>> prevPositions;			
					int index = t.widget.pickedEntityIndex;
					
					// Store initial rotations before any changes have been applied.
					if (bRotateObjectFromKeyPress && !bStartedKeyboardRotation)
					{
						if (g.entityrubberbandlist.size() == 0)
						{
							std::array<float, 3> prevRotation = { t.entityelement[index].rx, t.entityelement[index].ry, t.entityelement[index].rz };
							prevRotations.push_back(prevRotation);
							std::array<float, 4> prevQuatRotation = { t.entityelement[index].quatx, t.entityelement[index].quaty, t.entityelement[index].quatz, t.entityelement[index].quatw };
							prevQuatRotations.push_back(prevQuatRotation);
							std::array<int, 1> prevQuatRotationMode = { t.entityelement[index].quatmode };
							prevQuatRotationsMode.push_back(prevQuatRotationMode);
						}
						else
						{
							for (int i = 0; i < g.entityrubberbandlist.size(); i++)
							{
								int e = g.entityrubberbandlist[i].e;
								std::array<float, 3> prevRotation = { t.entityelement[e].rx, t.entityelement[e].ry, t.entityelement[e].rz };
								prevRotations.push_back(prevRotation);
								std::array<float, 4> prevQuatRotation = { t.entityelement[e].quatx, t.entityelement[e].quaty, t.entityelement[e].quatz, t.entityelement[e].quatw };
								prevQuatRotations.push_back(prevQuatRotation);
								std::array<int, 1> prevQuatRotationMode = { t.entityelement[e].quatmode };
								prevQuatRotationsMode.push_back(prevQuatRotationMode);
								// Need to store positions for rubberband, since they rotate about a point. 
								std::array<float,3> prevPosition = { t.entityelement[e].x, t.entityelement[e].y, t.entityelement[e].z };
								prevPositions.push_back(prevPosition);
							}
						}
						bStartedKeyboardRotation = true;
					}

					// Now that the rotation has finished, pass the event(s) to the undo system.
					if (bStartedKeyboardRotation && !bRotateObjectFromKeyPress)
					{
						bStartedKeyboardRotation = false;

						if (g.entityrubberbandlist.size() == 0)
						{
							undosys_object_changeposrotscl(index, t.entityelement[index].x, t.entityelement[index].y, t.entityelement[index].z, 
								prevRotations[0][0], prevRotations[0][1], prevRotations[0][2],
								prevQuatRotationsMode[0][0], prevQuatRotations[0][0], prevQuatRotations[0][1], prevQuatRotations[0][2], prevQuatRotations[0][3],
								t.entityelement[index].scalex, t.entityelement[index].scaley, t.entityelement[index].scalez);
						}
						else
						{
							undosys_multiplevents_start();
							for (int i = 0; i < prevPositions.size(); i++)
							{
								int e = g.entityrubberbandlist[i].e;
								undosys_object_changeposrotscl(e, prevPositions[i][0], prevPositions[i][1],	prevPositions[i][2], 
									prevRotations[i][0], prevRotations[i][1], prevRotations[i][2],
									prevQuatRotationsMode[i][0], prevQuatRotations[i][0], prevQuatRotations[i][1], prevQuatRotations[i][2], prevQuatRotations[i][3],
									t.entityelement[e].scalex, t.entityelement[e].scaley, t.entityelement[e].scalez);
							}
							undosys_multiplevents_finish();
						}

						prevRotations.clear();
						prevPositions.clear();
					}
					
					// trigger a rotation when detect rotation key pressed
					if (bRotateObjectFromKeyPress == true)
					{
						// the object to rotate
						int te = t.widget.pickedEntityIndex;
						int iObj = t.entityelement[t.widget.pickedEntityIndex].obj;
						int entid = t.entityelement[t.widget.pickedEntityIndex].bankindex;
						if (t.entityprofile[entid].ragdoll == 1)
						{
							fMoveAngX = 0.0f;
							fMoveAngZ = 0.0f;
						}

						// quat rotation event
						GGQUATERNION QuatAroundX, QuatAroundY, QuatAroundZ;
						GGQuaternionRotationAxis(&QuatAroundX, &GGVECTOR3(1, 0, 0), GGToRadian(fMoveAngX));
						GGQuaternionRotationAxis(&QuatAroundY, &GGVECTOR3(0, 1, 0), GGToRadian(fMoveAngY));
						GGQuaternionRotationAxis(&QuatAroundZ, &GGVECTOR3(0, 0, 1), GGToRadian(fMoveAngZ));
						quatRotationEvent = QuatAroundX * QuatAroundY * QuatAroundZ;

						// get quat from entity directly
						if (t.entityelement[te].quatmode == 0)
						{
							// if no orig quaty, calc it now
							entity_updatequatfromeuler(te);
						}
						GGQUATERNION toriginalAngle = GGQUATERNION(t.entityelement[te].quatx, t.entityelement[te].quaty, t.entityelement[te].quatz, t.entityelement[te].quatw);

						// apply the rotation event to the angle of the object
						GGQUATERNION quatNewOrientation;
						GGQuaternionMultiply(&quatNewOrientation, &toriginalAngle, &quatRotationEvent);

						// rotate this object with final quat and get new entity rotation eulers
						RotateObjectQuat(iObj, quatNewOrientation.x, quatNewOrientation.y, quatNewOrientation.z, quatNewOrientation.w);
						t.entityelement[te].rx = ObjectAngleX(iObj);
						t.entityelement[te].ry = ObjectAngleY(iObj);
						t.entityelement[te].rz = ObjectAngleZ(iObj);

						// update entity quat as the preferred source rotation
						t.entityelement[te].quatmode = 1;
						t.entityelement[te].quatx = quatNewOrientation.x;
						t.entityelement[te].quaty = quatNewOrientation.y;
						t.entityelement[te].quatz = quatNewOrientation.z;
						t.entityelement[te].quatw = quatNewOrientation.w;

						// mark as static if it was
						if (t.entityelement[te].staticflag == 1) g.projectmodifiedstatic = 1;

						// special case for characters, only want the Y angle
						if (t.entityprofile[entid].ischaracter == 1)
						{
							// quats are the true rotations of objects, but refresh euler for characters to ONLY use the Y axis
							entity_calculateeuleryfromquat(t.widget.pickedEntityIndex);
						}

						// when rotate with keys, see new value in slider/value right panel
						g_bRefreshRotationValuesFromObjectOnce = true;
					}

					//PE: Update light data for spot.
					//PE: Updating probes is slow, this is called on each frame.
					if (t.entityelement[t.widget.pickedEntityIndex].eleprof.usespotlighting)
					{
						static bool bReadyToUpdateSpot = false;
						bool bUpdate = false;
						if (!bReadyToUpdateSpot && ImGui::IsMouseClicked(0)) bReadyToUpdateSpot = true;
						if (bReadyToUpdateSpot && !ImGui::IsMouseClicked(0))
						{
							bReadyToUpdateSpot = false;
							bUpdate = true;
						}
						if(bUpdate)
						{
							lighting_refresh();
						}
					}

					// also update particle emitter
					entity_updateparticleemitter(t.widget.pickedEntityIndex);
					entity_updateautoflatten(t.widget.pickedEntityIndex);

					if ( t.entityelement[t.widget.pickedEntityIndex].obj>0 ) 
					{
						int iTargetCenterObject = t.entityelement[t.widget.pickedEntityIndex].obj;
						if ( ObjectExist ( iTargetCenterObject ) == 1 ) 
						{
							if ( g.entityrubberbandlist.size() > 0 )
							{
								// rotate all the grouped entities and move around Y axis of widget as pivot
								if (bRotateObjectFromKeyPress == true)
								{
									SetStartPositionsForRubberBand(iTargetCenterObject);
									RotateAndMoveRubberBand(iTargetCenterObject, 0, 0, 0, quatRotationEvent);
								}
							}
						}
					}
				}
				//  Find Floor (  control of widget controlled entity or Raise/lower with PGUP and PGDN )
				if (t.inputsys.keyreturn == 1 || t.inputsys.kscancode == 201 || t.inputsys.kscancode == 209)
				{
					t.tforceentityfindfloor = t.widget.pickedEntityIndex;
					t.tforcepguppgdnkeys = 1;
					editor_forceentityfindfloor (false);
				}
			}
			else
			{
				if (  t.inputsys.domodeterrain == 0 && t.inputsys.domodeentity == 0 ) 
				{
					if (t.widget.pickedEntityIndex > 0)
					{
						if (iObjectMoveModeDropSystem < 0)
						{
							//PE: Support new y placement when also using shift. t.gridentity is set.
							if (t.inputsys.keyreturn == 1 || t.inputsys.kscancode == 201 || t.inputsys.kscancode == 209)
							{
								t.tforceentityfindfloor = t.widget.pickedEntityIndex;
								t.tforcepguppgdnkeys = 1;
								editor_forceentityfindfloor(false);
							}
						}

					}
					//  avoid interference from terrain/entity mode change
					if (  t.inputsys.dorotation == 1 ) 
					{
						if (iObjectMoveMode == 2 && g_iOrientToSurfaceMode == 1)
						{
							g_fLocalTurnRotationForSmartMode += t.tspeedofrot_f;
							g_fLocalTurnRotationForSmartMode = WrapValue(g_fLocalTurnRotationForSmartMode);
						}
						else
						{
							t.gridentityrotatey_f += t.tspeedofrot_f;
							t.gridentityrotateaxis = 1;
						}
					}
					if (  t.inputsys.doentityrotate == 1 )
					{ 
						t.gridentityrotatex_f -= t.tspeedofrot_f  ; t.gridentityrotateaxis = 0; 
					}
					if (  t.inputsys.doentityrotate == 2 ) { t.gridentityrotatex_f += t.tspeedofrot_f  ; t.gridentityrotateaxis = 0; }
					if (  t.inputsys.doentityrotate == 3 ) { t.gridentityrotatey_f -= t.tspeedofrot_f  ; t.gridentityrotateaxis = 1; }
					if (  t.inputsys.doentityrotate == 4 ) { t.gridentityrotatey_f += t.tspeedofrot_f  ; t.gridentityrotateaxis = 1; }
					if (  t.inputsys.doentityrotate == 5 ) { t.gridentityrotatez_f -= t.tspeedofrot_f  ; t.gridentityrotateaxis = 2; }
					if (  t.inputsys.doentityrotate == 6 ) { t.gridentityrotatez_f += t.tspeedofrot_f  ; t.gridentityrotateaxis = 2; }
					if (  t.inputsys.doentityrotate >= 98 ) 
					{
						if (  t.inputsys.doentityrotate == 98 ) 
						{
							if (  t.gridentityrotateaxis == 0  )  t.gridentityrotatex_f = 0;
							if (  t.gridentityrotateaxis == 1  )  t.gridentityrotatey_f = 0;
							if (  t.gridentityrotateaxis == 2  )  t.gridentityrotatez_f = 0;
						}
						if (  t.inputsys.doentityrotate == 99 ) 
						{
							t.gridentityrotatex_f=0;
							t.gridentityrotatey_f=0;
							t.gridentityrotatez_f=0;
						}
					}
				}
				t.gridentityrotatex_f=WrapValue(t.gridentityrotatex_f);
				t.gridentityrotatey_f=WrapValue(t.gridentityrotatey_f);
				t.gridentityrotatez_f=WrapValue(t.gridentityrotatez_f);
			}
		}
	}

	//  Load and Save
	if ( t.inputsys.doload == 1 ) gridedit_load_map ( );
	if ( t.inputsys.dosave == 1 ) 
	{
		if (  g.galwaysconfirmsave == 1 ) 
		{
			gridedit_saveas_map ( );
		}
		else
		{
			gridedit_save_map_ask ( );
		}
	}
	if ( t.inputsys.doopen == 1 ) gridedit_open_map_ask ( );
	if ( t.inputsys.donew == 1 || t.inputsys.donewflat == 1 ) gridedit_new_map_ask ( );
	if ( t.inputsys.dosaveas == 1 ) gridedit_saveas_map ( );
	if ( t. inputsys.dosaveandrun==1 ) { t.inputsys.dosaveandrun = 0 ; editor_previewmap ( 0 ); }

	//  Undo \ Redo
	if (t.inputsys.doundo == 1)
	{
		editor_undo ();
		t.inputsys.doundo = 0;
	}
	if (t.inputsys.doredo == 1)
	{
		editor_redo ();
		t.inputsys.doredo = 0;
	}

	//  Paint Select or Art Mode
	if ( t.inputsys.domodeterrain == 1 || t.inputsys.domodemarker == 1 || t.inputsys.domodeentity == 1  || t.inputsys.domodewaypoint == 1 )
	{
		// select editing mode and refresh
		if ( t.inputsys.domodeterrain == 1 ) { t.inputsys.domodeterrain=0; t.gridentitymarkersmodeonly=0; t.grideditselect=0; }
		if ( t.inputsys.domodemarker == 1 ) { t.inputsys.domodemarker=0; t.gridentitymarkersmodeonly=1; t.grideditselect=5; }
		if ( t.inputsys.domodeentity == 1 ) { t.inputsys.domodeentity=0; t.gridentitymarkersmodeonly=0; t.grideditselect=5; }
		if ( t.inputsys.domodewaypoint == 1 ) { t.inputsys.domodewaypoint=0; t.gridentitymarkersmodeonly=0; t.grideditselect=6; }
		editor_refresheditmarkers ( );
		gridedit_updateprojectname();

		// also deactivate EBE if enter a regular editing mode
		ebe_hide();
	}

	//  Manage waypoints on map
	t.tokay=0;
	if (  t.grideditselect == 5 ) 
	{
		//  entity mode can manipulate waypoint zone style
		if (  t.widget.pickedObject == 0 && t.widget.pickedSection == 0 && t.gridentity == 0 ) 
		{
			//  ensure low interference if editing, etc
			t.tokay=1;
		}
	}
	if (  t.grideditselect == 6 ) 
	{
		//  waypoint mode has access to waypoint editing
		t.tokay=1;
	}
	if (  t.tokay == 1 ) 
	{
		t.mx_f=t.inputsys.localx_f ; t.mz_f=t.inputsys.localy_f ; t.mclick=t.inputsys.mclick;
		g.waypointeditheight_f=t.inputsys.localcurrentterrainheight_f;

		// only detect waypoints when NOT using rubber band
		if (t.inputsys.rubberbandmode == 0 && t.ebe.on == 0 && t.showeditorelements)
			waypoint_mousemanage ( );
	}

	//  New clip height control
	if (  t.inputsys.keycontrol == 1 ) 
	{
		if (  t.inputsys.wheelmousemove>0 ) 
		{
			t.clipheight_f -= 2.0f ; if (  t.clipheight_f<0.0f  )  t.clipheight_f = 0.0f;
			t.updatezoom=1;
		}
		if (  t.inputsys.wheelmousemove<0 ) 
		{
			t.clipheight_f += 2.0f ; if (  t.clipheight_f>50000.0f )  t.clipheight_f = 50000.0f;
			t.updatezoom=1;
		}
	}

	//  Zoom factor (for top down or freeflight+ControlKey ( ) )
	t.tspecialgridzoomadjustment=0;
	if (  t.editorfreeflight.mode == 0 || t.tspecialgridzoomadjustment != 0 ) 
	{
		if (  ((t.inputsys.dozoomin == 1 && t.inputsys.keypress == 0) || t.tspecialgridzoomadjustment == 1) && t.gridzoom_f>0.3 ) 
		{
			t.updatezoom=1;
			if (  t.inputsys.keyshift == 1 ) 
			{
				t.gridzoom_f -= 0.6f*fMouseWheelZoomFactor;
			}
			else
			{
				t.gridzoom_f -= 0.1f*fMouseWheelZoomFactor;
			}
		}
		if (  ((t.inputsys.dozoomout == 1 && t.inputsys.keypress == 0) || t.tspecialgridzoomadjustment == 2) && t.gridzoom_f<40.0 ) 
		{
			t.updatezoom=1;
			if (  t.inputsys.keyshift == 1 ) 
			{
				t.gridzoom_f += 0.6f*fMouseWheelZoomFactor;
			}
			else
			{
				t.gridzoom_f += 0.1f*fMouseWheelZoomFactor;
			}
		}
	}

	//  Scroll Map
	t.borderx_f=1024.0*50.0;
	t.bordery_f=1024.0*50.0;
	float camSpeedMod = 0.5f;
	if (t.inputsys.keyshift)
	{
		camSpeedMod = 1.0f;
	}
	else if (t.inputsys.keycontrol)
	{
		camSpeedMod = 0.1f;
	}
	if (  t.inputsys.doscrollleft != 0 ) 
	{
		t.cx_f -= t.gridzoom_f*3*t.inputsys.doscrollleft*camSpeedMod;
		t.updatezoom=1;
	}
	if (  t.inputsys.doscrollright != 0 ) 
	{
		t.cx_f += t.gridzoom_f * 3 * t.inputsys.doscrollright*camSpeedMod;
		t.updatezoom=1;
	}
	if (  t.inputsys.doscrollup != 0 ) 
	{
		t.cy_f += t.gridzoom_f*3*t.inputsys.doscrollup*camSpeedMod;
		t.updatezoom=1;
	}
	if (  t.inputsys.doscrolldown != 0 ) 
	{
		t.cy_f -= t.gridzoom_f*3*t.inputsys.doscrolldown*camSpeedMod;
		t.updatezoom=1;
	}

	//  Scroll boundaries
	// no such limits in the MAX world!

	//PE: Only active in object mode. t.grideditselect == 5
	if (!g_bCharacterCreatorPlusActivated && !bStoryboardWindow && t.grideditselect == 5 )
	{
		//PE: @Paul we need this in object mode :)
		MouseLeftDragXZPanning();
		MouseWheelYPanning();
	}
}

float editor_forceentityfindfloor (bool bPredictMode)
{
	// earl out if t.gridentityinzoomview out of bounds
	if (t.gridentityinzoomview >= g.entityelementmax) return 0.0f;
	//LB: And some extra protection (backs up PE addition below)
	if (t.gridentityinzoomview >= t.entityelement.size()) return 0.0f;

	// receives; tforceentityfindfloor
	// bPredictMode = set to true when we want to work out where the object will go when forced to floor
	// but without affecting any globals or states
	float fPredictedYPosition = 0.0f;
	int ssgridentityinzoomview, ssgridentitydroptoground;
	float ssgridentityposx_f, ssgridentityposy_f, ssgridentityposz_f;
	int ssgridentityobj, ssthardauto, ssgridentityposoffground, ssgridentityusingsoftauto, ssgridentitysurfacesnap;
	if (bPredictMode == true)
	{
		ssgridentityinzoomview = t.gridentityinzoomview;
		ssgridentitydroptoground = t.gridentitydroptoground;
		ssgridentityposx_f = t.gridentityposx_f;
		ssgridentityposy_f = t.gridentityposy_f;
		ssgridentityposz_f = t.gridentityposz_f;
		ssgridentityobj = t.gridentityobj;
		ssthardauto = t.thardauto;
		ssgridentityposoffground = t.gridentityposoffground;
		ssgridentityusingsoftauto = t.gridentityusingsoftauto;
		ssgridentitysurfacesnap = t.gridentitysurfacesnap;
	}
	t.storegridentityinzoomview = t.gridentityinzoomview;
	t.gridentityinzoomview = t.tforceentityfindfloor;
	t.storegridentityposy_f = t.gridentityposy_f;
	t.gridentityposy_f = t.entityelement[t.gridentityinzoomview].y; //PE: Crash , if loading level with less entitys and selecting a object, now reset in new level.
	int iEntPassMax = 1;
	if (g.entityrubberbandlist.size() > 0) iEntPassMax = g.entityrubberbandlist.size();
	for (int iEntPass = 0; iEntPass < iEntPassMax; iEntPass++)
	{
		// which entity are we dealing with
		int e = t.gridentityinzoomview;
		if (!pref.iEnableDragDropEntityMode)
		{
			if (g.entityrubberbandlist.size() > 0)
			{
				e = g.entityrubberbandlist[iEntPass].e;
			}
		}
		else
		{
			if (g.entityrubberbandlist.size() > 0)
			{
				// Ignore if entity is part of group. It would split up the group.
				e = g.entityrubberbandlist[iEntPass].e;

				// Dont allow find floor if entity is part of group.
				int grouplist = isEntityInGroupList(e);
				if (grouplist >= 0) e = 0;
			}
		}
		if ((e > 0 && t.entityelement[e].editorlock == 0) || bPredictMode == true)
		{
			// if RETURN key pressed
			if (bPredictMode == true)
				t.gridentityposy_f = ObjectPositionY(t.gridentityobj);
			else
				t.gridentityposy_f = t.entityelement[e].y;
			if (t.inputsys.keyreturn == 1 || bPredictMode == true)
			{
				// store globs in store
				t.storegridentitydroptoground = t.gridentitydroptoground;
				t.storegridentityposx_f = t.gridentityposx_f;
				t.storegridentityposz_f = t.gridentityposz_f;
				t.storegridentityobj = t.gridentityobj;
				t.storegridentityposoffground = t.gridentityposoffground;
				t.gridentitydroptoground = 1;
				if (bPredictMode == true)
				{
					t.gridentityposx_f = ObjectPositionX(t.gridentityobj);
					t.gridentityposz_f = ObjectPositionZ(t.gridentityobj);
				}
				else
				{
					t.gridentityposx_f = t.entityelement[e].x;
					t.gridentityposz_f = t.entityelement[e].z;
					t.gridentityobj = t.entityelement[e].obj;
				}
				t.thardauto = 1; editor_findentityground();
				if (t.gridentityposoffground == 0)
				{
					float ftmp = BT_GetGroundHeight(t.terrain.TerrainID, t.gridentityposx_f, t.gridentityposz_f);
					t.gridentityposy_f = ftmp;
					ApplyPivotToGridEntity();
					if (t.entityprofile[t.gridentity].ismarker != 0)  t.gridentityposy_f = t.gridentityposy_f + t.entityprofile[t.gridentity].offy;
					if (t.entityprofile[t.gridentity].defaultheight != 0)  t.gridentityposy_f = t.gridentityposy_f + t.entityprofile[t.gridentity].defaultheight;
				}
				if (bPredictMode == false)
				{
					t.entityelement[e].x = t.gridentityposx_f;
					t.entityelement[e].z = t.gridentityposz_f;
				}

				// restore globs from store
				t.gridentitydroptoground = t.storegridentitydroptoground;
				t.gridentityposx_f = t.storegridentityposx_f;
				t.gridentityposz_f = t.storegridentityposz_f;
				t.gridentityobj = t.storegridentityobj;
				t.gridentityposoffground = t.storegridentityposoffground;
				t.gridentityusingsoftauto = 1;
				t.gridentitysurfacesnap = 0;
			}
			if (bPredictMode == false)
			{
				if (t.tforcepguppgdnkeys == 1)
				{
					editor_handlepguppgdn();
				}
				if (t.entityelement[e].y != t.gridentityposy_f)
				{
					t.entityelement[e].beenmoved = 1;
				}
				t.entityelement[e].y = t.gridentityposy_f;
				if (t.entityelement[e].obj > 0)
				{
					if (ObjectExist(t.entityelement[e].obj) == 1)
					{
						PositionObject(t.entityelement[e].obj, t.entityelement[e].x, t.entityelement[e].y, t.entityelement[e].z);
					}
				}
			}
			else
			{
				// have the prediction for where the object would be placed
				fPredictedYPosition = t.gridentityposy_f;
			}
		}
	}
	t.gridentityposy_f = t.storegridentityposy_f;
	t.gridentityinzoomview = t.storegridentityinzoomview;
	if (bPredictMode == true)
	{
		// restore any changes and return prediction
		t.gridentityinzoomview = ssgridentityinzoomview;
		t.gridentitydroptoground = ssgridentitydroptoground;
		t.gridentityposx_f = ssgridentityposx_f;
		t.gridentityposy_f = ssgridentityposy_f;
		t.gridentityposz_f = ssgridentityposz_f;
		t.gridentityobj = ssgridentityobj;
		t.thardauto = ssthardauto;
		t.gridentityposoffground = ssgridentityposoffground;
		t.gridentityusingsoftauto = ssgridentityusingsoftauto;
		t.gridentitysurfacesnap = ssgridentitysurfacesnap;
		return fPredictedYPosition;
	}
	else
	{
		// regular usage
		return 0.0f;
	}
}

void editor_viewfunctionality ( void )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	// map view controls
	if ( t.grideditselect == 3 ) 
	{
		if ( t.inputsys.mclick == 1 ) 
		{
			t.stcx_f=t.inputsys.mmx*100.0;
			t.stcy_f=t.inputsys.mmy*100.0;
			t.cx_f=t.stcx_f ; t.cy_f=t.stcy_f ; t.gridzoom_f=t.stgridzoom_f;
			t.grideditselect=t.stgrideditselect ; editor_refresheditmarkers ( );
			while ( t.inputsys.mclick==1 ) { input_getcontrols() ; Sync() ; }
			t.cameraviewmode=0;
			t.updatezoom=1;
		}
	}

	// zoom view controls
	if ( t.grideditselect == 4 ) 
	{
		//  exit zoom view
		#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
		if (!bImGuiGotFocus && bImGuiRenderTargetFocus && t.inputsys.mclick == 1)  t.tpressedtoleavezoommode = 1;

		//PE: We cant leave before bEntity_Properties_Window = false;
		//PE: If we have a wicked "texture" rendered in imgui , changing texture ("clone" after properties) will crash.
		if (bEntity_Properties_Window && !bImGuiGotFocus && bImGuiRenderTargetFocus && t.inputsys.mclick == 0 && t.tpressedtoleavezoommode == 1) bEntity_Properties_Window = false;
		else if (!bImGuiGotFocus && bImGuiRenderTargetFocus && t.inputsys.mclick == 0 && t.tpressedtoleavezoommode == 1)  t.tpressedtoleavezoommode = 2;

		//When properties window open , they should click "apply","cancel".
		if(bProperties_Window_Block_Mouse)
			t.tpressedtoleavezoommode = 0;

		if (bProperties_Window_Block_Mouse) 
		{
			//Must have a release before block is released.
			if (t.inputsys.mclick == 0) 
			{
				bProperties_Window_Block_Mouse = false;
				t.tpressedtoleavezoommode = 0;
			}
		}
		#else
		if ( t.inputsys.mclick == 1  )  t.tpressedtoleavezoommode = 1;
		if ( t.inputsys.mclick == 0 && t.tpressedtoleavezoommode == 1 )  t.tpressedtoleavezoommode = 2;
		#endif

		if ( (t.tpressedtoleavezoommode == 2 || t.inputsys.kscancode == 211) || t.editorinterfaceleave == 1 ) 
		{
			// leave zoomview
			t.inputsys.doautozoomview=1;

			// reset mouse click (must release LMB before zoom mode ends)
			t.tpressedtoleavezoommode=0;

			// close any property window
			interface_closepropertywindow ( );
			t.editorinterfaceleave=0;

			// 310315 - Ensure clipping is restored when return
			t.updatezoom=1;
			#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
			#endif

			// place entity on the map
			if ( t.gridentityinzoomview>0 ) 
			{
				// DELETE key deletes entity no matter what (for fixed entities too)
				if ( t.gridentity != 0 && t.inputsys.kscancode == 211 ) 
				{
					// Delete any associated waypoint/trigger zone
					t.waypointindex=t.grideleprof.trigger.waypointzoneindex;
					if (  t.waypointindex>0 ) 
					{
						t.w=t.waypoint[t.waypointindex].start;
						waypoint_delete ( );
					}
					t.grideleprof.trigger.waypointzoneindex=0;

					// And now delete entity from cursor
					if (  t.gridentityobj == 0 ) 
					{
						DeleteObject (  t.gridentityobj );
						t.gridentityobj=0;
					}
					t.gridentityinzoomview = 0;
				}
				else
				{
					// Add entity back into map
					if (iOldgridentity == t.gridentity) 
					{
						//PE: InstanceObject - Cursor,Object Tools - objects must always be real clones.
						extern bool bNextObjectMustBeClone;
						bNextObjectMustBeClone = true;

						gridedit_addentitytomap();
						bNextObjectMustBeClone = false;

						t.gridentityinzoomview = 0;
					}
					else 
					{
						timestampactivity(0, "t.gridentity!=lastpropertiesid ?");
					}
					//  Hide widget to make clean return to editor
					t.widget.pickedObject=0  ; widget_updatewidgetobject ( );
				}

				// Reset cursor object settings
				t.refreshgrideditcursor=1;
				t.gridentity=0;
				t.gridedit.autoflatten=0;
				t.gridedit.entityspraymode=0;
				t.gridentityposoffground=0;
				t.gridentityusingsoftauto=1;
				t.gridentitysurfacesnap=1-g.gdisablesurfacesnap;
				// MAX handles its own positioning system
				t.gridentityautofind = 0;
				t.inputsys.dragoffsetx_f=0;
				t.inputsys.dragoffsety_f=0;
				editor_refreshentitycursor ( );

				if (bWaypointDrawmode || bWaypoint_Window) { bWaypointDrawmode = false; bWaypoint_Window = false; }
				if (bImporter_Window) { importer_quit(); bImporter_Window = false; }
				bEntity_Properties_Window = false; //Close Properties window.
			}
		}
	}
}

void editor_findentityground ( void )
{
	//  for entities that can be moved
	if (  t.entityelement[t.gridentityinzoomview].editorfixed == 0 ) 
	{
		//  finds ground
		if ( t.gridentitydroptoground == 1 || (t.thardauto == 0 && t.gridentityusingsoftauto == 1) ) 
		{
			bDetectTerrainOnly = false;

			//PE: MUST disable collision on ALL rubberband objects.
			std::vector<sRubberBandType> entityvisible = g.entityrubberbandlist;
			if (g.entityrubberbandlist.size() > 0)
			{
				for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
				{
					int e = g.entityrubberbandlist[i].e;
					int obj = t.entityelement[e].obj;
					if (obj > 0 && GetVisible(obj))
					{
						entityvisible[i].x = 1;
						HideObject(obj);
					}
					else
					{
						entityvisible[i].x = 0;
					}
				}
			}


			t.tbestdist_f=99999 ; t.tbesty_f=0;
			t.tto_f = t.gridentityposy_f - 9000.0; //Make sure we hit.
			for ( t.e = 1 ; t.e <= g.entityelementlist; t.e++ )
			{
				if ( t.thardauto == 1 ) 
				{
					// regular
					if ( t.entityelement[t.e].editorlock == 0 ) 
					{
						if (ObjectExist(t.gridentityobj) == 1)
						{
							// only allow a stack slightly higher than the height of the object we are stacking (so dont end up on a roof)
							// Rick complainted, so use a fixed high height so we can stack anything, even if on the roof
							t.tfrom_f = t.gridentityposy_f + 200.0f;// fObjectSizeY + fMargin;

							// and make sure never higher than the camera Y (what we can really 'see' generally)
							if (t.tfrom_f > t.gridtrueslicey_f)
							{
								t.tfrom_f = t.gridtrueslicey_f;
							}
						}
					}
					else
					{
						t.tfrom_f=t.gridentityposy_f+75.0;
					}
				}
				else
				{
					//  very subtle surface scan (to defeat small floors)
					t.tfrom_f=t.gridentityposy_f+11.0;
				}
				t.tokay=1;
				if ( t.entityprofile[t.entid].addhandlelimb>0  ) t.tokay = 0;
				if ( t.playercontrol.thirdperson.enabled == 1 ) 
				{
					// if third person char, ignore when finding surface
					if ( t.e == t.playercontrol.thirdperson.charactere  )  t.tokay = 0;
					if ( t.e == t.playercontrol.thirdperson.startmarkere  )  t.tokay = 0;
				}
				if ( t.tokay == 1 ) 
				{
					t.obj=t.entityelement[t.e].obj;
					if ( t.obj>0 && t.obj != t.gridentityobj ) 
					{
						if ( ObjectExist(t.obj) == 1 ) 
						{
							if ( GetVisible(t.obj) == 1 ) 
							{
								// 210415 - added distance check to speed up ground scan
								t.tdiffx_f=ObjectPositionX(t.obj)-t.gridentityposx_f;
								t.tdiffz_f=ObjectPositionZ(t.obj)-t.gridentityposz_f;
								t.tdiff_f=Sqrt(abs(t.tdiffx_f*t.tdiffx_f)+abs(t.tdiffz_f*t.tdiffz_f));
								if ( t.tdiff_f<ObjectSize(t.obj)*2 ) 
								{
									if ( IntersectObject(t.obj,t.gridentityposx_f,t.tfrom_f,t.gridentityposz_f,t.gridentityposx_f,t.tto_f,t.gridentityposz_f) != 0 ) 
									{
										t.tdist_f=abs(ChecklistFValueB(6)-t.tfrom_f);
										if ( t.tdist_f<t.tbestdist_f ) 
										{
											t.tbesty_f=ChecklistFValueB(6);
											t.tbestdist_f=t.tdist_f;
										}
									}
								}
							}
						}
					}
				}
			}
			if ( t.tbestdist_f < 99999 ) 
			{
				// found GetPoint ( where our entity will rest vertically )
				t.gridentityposy_f=t.tbesty_f; t.zoomviewtargety_f=t.tbesty_f;

				// now need entities own thickness from object 0,0,0 to base
				// grid of ray casts for good base detect resolution
				if ( t.gridentityobj>0 && t.thardauto == 1 ) 
				{
					if ( ObjectExist(t.gridentityobj) == 1 ) 
					{
						t.ttentsizex_f=ObjectSizeX(t.gridentityobj)/2.0;
						t.ttentsizez_f=ObjectSizeZ(t.gridentityobj)/2.0;
						if (  t.ttentsizex_f<1.0 && t.ttentsizex_f<t.ttentsizez_f  )  t.ttentsizex_f = t.ttentsizez_f;
						if (  t.ttentsizez_f<1.0 && t.ttentsizez_f<t.ttentsizex_f  )  t.ttentsizez_f = t.ttentsizex_f;
						t.stepvaluex_f=ObjectSizeX(t.gridentityobj)/10.0;
						t.stepvaluez_f=ObjectSizeZ(t.gridentityobj)/10.0;
						if (  t.stepvaluex_f<1  )  t.stepvaluex_f = 1.0;
						if (  t.stepvaluez_f<1  )  t.stepvaluez_f = 1.0;
						if (  ObjectExist(g.entityworkobjectoffset) == 1  )  DeleteObject (  g.entityworkobjectoffset );
						MakeObjectBox (  g.entityworkobjectoffset,ObjectSizeX(t.gridentityobj),ObjectSizeY(t.gridentityobj),ObjectSizeZ(t.gridentityobj) );
						PositionObject (  g.entityworkobjectoffset,ObjectPositionX(t.gridentityobj)+GetObjectCollisionCenterZ(t.gridentityobj),ObjectPositionY(t.gridentityobj)+GetObjectCollisionCenterY(t.gridentityobj),ObjectPositionZ(t.gridentityobj)+GetObjectCollisionCenterZ(t.gridentityobj) );
						RotateObject (  g.entityworkobjectoffset,ObjectAngleX(t.gridentityobj),ObjectAngleY(t.gridentityobj),ObjectAngleZ(t.gridentityobj) );
						HideObject (  g.entityworkobjectoffset );
						t.tsmallest_f=99999;
						t.tscbase_f=ObjectPositionY(g.entityworkobjectoffset)-(ObjectSizeY(g.entityworkobjectoffset)*2);
						if (  t.tsmallest_f<99999 ) 
						{
							t.tthickness_f=ObjectPositionY(t.gridentityobj)-(t.tscbase_f+t.tsmallest_f);
						}
						else
						{
							t.tthickness_f=0;
						}
						if (  ObjectExist(g.entityworkobjectoffset) == 1  )  DeleteObject (  g.entityworkobjectoffset );
						t.gridentityposy_f=t.tbesty_f+t.tthickness_f ; t.zoomviewtargety_f=t.tbesty_f+t.tthickness_f;
					}
				}
				// ensure a 'autofoundYpos' never drops BELOW Floor ( ( (i.e. skull under Floor) ) )
				float ftmp = BT_GetGroundHeight(t.terrain.TerrainID, t.gridentityposx_f, t.gridentityposz_f);
				t.trygridentityposy_f = ftmp;
				ApplyPivotToGridEntity();
				if (  t.gridentityposy_f<t.trygridentityposy_f ) 
				{
					t.gridentityposy_f = t.trygridentityposy_f;
					ApplyPivotToGridEntity();
					if (  t.entityprofile[t.gridentity].ismarker != 0  )  t.gridentityposy_f = t.gridentityposy_f + t.entityprofile[t.gridentity].offy;
					if (  t.entityprofile[t.gridentity].defaultheight != 0  )  t.gridentityposy_f = t.gridentityposy_f + t.entityprofile[t.gridentity].defaultheight;
				}
				//  we are sitting on an entity, no need for ground terrain resting
				t.gridentityposoffground=1;
			}
			else
			{
				//  if not find any entities, use terrain ground base
				t.gridentityposoffground=0;
			}

			//PE: Reenable rubberband collision.
			if (entityvisible.size() > 0)
			{
				for (int i = 0; i < (int)entityvisible.size(); i++)
				{
					int e = entityvisible[i].e;
					int obj = t.entityelement[e].obj;
					if (entityvisible[i].x == 1)
					{
						ShowObject(obj);
					}
				}
			}
		}
		else
		{
			if (!(t.gridentitydroptoground == 2 && t.thardauto == 1))
			{
				if (bDetectTerrainOnly && t.gridentity > 0 && t.gridentityobj > 0)
				{
					float newy = 0.0f;
					newy = BT_GetGroundHeight(t.terrain.TerrainID, t.gridentityposx_f, t.gridentityposz_f);
					if (newy != 0.0f)
					{
						t.gridentityposy_f = newy;
						//PE: Apply pivot here.
						bool bTmp = bExtractFixPivot;
						bExtractFixPivot = true;
						ApplyPivotToGridEntity();
						bExtractFixPivot = bTmp;
					}
				}
			}
		}

		// finds wall
		if ( t.gridentitydroptoground == 2 && t.thardauto == 1 ) 
		{
			bDetectTerrainOnly = false;

			t.tbestdist_f=99999 ; t.tbestx_f=0 ; t.tbestz_f=0;
			t.tbesty_f=t.gridentityposy_f+GetObjectCollisionCenterY(t.gridentityobj);
			t.a=t.gridentityrotatey_f;
			t.tfromx=NewXValue(t.gridentityposx_f,t.a,-5.0) ; t.ttox=NewXValue(t.gridentityposx_f,t.a,75.0);
			t.tfromz=NewZValue(t.gridentityposz_f,t.a,-5.0) ; t.ttoz=NewZValue(t.gridentityposz_f,t.a,75.0);
			for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
			{
				t.obj=t.entityelement[t.e].obj;
				if (  t.obj>0 && t.obj != t.gridentityobj ) 
				{
					if (  ObjectExist(t.obj) == 1 ) 
					{
						if (  GetVisible(t.obj) == 1 ) 
						{
							t.tdist_f=IntersectObject(t.obj,t.tfromx,t.tbesty_f,t.tfromz,t.ttox,t.tbesty_f,t.ttoz);
							if (  t.tdist_f != 0 ) 
							{
								if (  t.tdist_f<t.tbestdist_f ) 
								{
									t.tbestx_f=ChecklistFValueA(6);
									t.tbestz_f=ChecklistFValueC(6);
									t.tbestdist_f=t.tdist_f;
								}
							}
						}
					}
				}
			}
			if (  t.tbestdist_f<99999 ) 
			{
				//  found GetPoint (  where our entity will rest on wall )
				//  now need entities own thickness from object 0,0,0 to wall-contact
				t.tbestx_f=NewXValue(t.tbestx_f,t.a+180,-5.0);
				t.tbestz_f=NewZValue(t.tbestz_f,t.a+180,-5.0);
				t.ttox=NewXValue(t.tbestx_f,t.a+180,100.0);
				t.ttoz=NewZValue(t.tbestz_f,t.a+180,100.0);
				if (  ObjectExist(g.entityworkobjectoffset) == 1  )  DeleteObject (  g.entityworkobjectoffset );
				MakeObjectBox (  g.entityworkobjectoffset,ObjectSizeX(t.gridentityobj),ObjectSizeY(t.gridentityobj),ObjectSizeZ(t.gridentityobj) );
				PositionObject (  g.entityworkobjectoffset,ObjectPositionX(t.gridentityobj)+GetObjectCollisionCenterZ(t.gridentityobj),ObjectPositionY(t.gridentityobj)+GetObjectCollisionCenterY(t.gridentityobj),ObjectPositionZ(t.gridentityobj)+GetObjectCollisionCenterZ(t.gridentityobj) );
				RotateObject (  g.entityworkobjectoffset,ObjectAngleX(t.gridentityobj),ObjectAngleY(t.gridentityobj),ObjectAngleZ(t.gridentityobj) );
				HideObject (  g.entityworkobjectoffset );
				t.tgap_f=IntersectObject(g.entityworkobjectoffset,t.tbestx_f,t.tbesty_f,t.tbestz_f,t.ttox,t.tbesty_f,t.ttoz);
				if (  t.tgap_f >= 4.9 ) 
				{
					t.tgapx_f=ChecklistFValueA(6);
					t.tgapz_f=ChecklistFValueC(6);
					t.ttddx_f=t.tgapx_f-ObjectPositionX(t.gridentityobj);
					t.ttddz_f=t.tgapz_f-ObjectPositionZ(t.gridentityobj);
					t.tthickness_f=5.0+Sqrt(abs(t.ttddx_f*t.ttddx_f)+abs(t.ttddz_f*t.ttddz_f));
				}
				else
				{
					t.tthickness_f=5.0;
				}
				t.tbestx_f=NewXValue(t.tbestx_f,t.a+180,t.tthickness_f+0.5);
				t.tbestz_f=NewZValue(t.tbestz_f,t.a+180,t.tthickness_f+0.5);
				t.gridentityposx_f=t.tbestx_f ; t.zoomviewtargetx_f=t.tbestx_f;
				t.gridentityposz_f=t.tbestz_f ; t.zoomviewtargetz_f=t.tbestz_f;
				if (  ObjectExist(g.entityworkobjectoffset) == 1  )  DeleteObject (  g.entityworkobjectoffset );
			}
		}
	}
}

void editor_refresheditmarkers ( void )
{
	//  Deactivate widget if still in effect
	widget_switchoff ( );

	//  Deactivate floating selection of entity
	if ( t.grideditselect != 5 && t.grideditselect != 4 ) 
	{
		if ( t.grideditselect != 5 ) HideObject ( t.editor.objectstartindex+5 );
		t.gridentity=0 ; t.gridentityposoffground=0;
		t.gridentityusingsoftauto=0;
		t.gridentitysurfacesnap=1-g.gdisablesurfacesnap;
		// MAX handles its own positioning system
		t.gridentityautofind = 0;
		t.inputsys.dragoffsetx_f=0;
		t.inputsys.dragoffsety_f=0;
	}

	//  Update entity cursor? (delete many of these as it WAS old shroud updater!)
	t.refreshgrideditcursor=1;

	//  Update clipboard items based on mode
	editor_cutcopyclearstate ( );

	//  Waypoint visibility
	if (  t.grideditselect != t.lastgrideditselect ) 
	{
		t.lastgrideditselect=t.grideditselect;
		if (  t.grideditselect == 6 ) 
		{
			waypoint_showallpaths ( );
		}
		else
		{
			if (  t.inputsys.dowaypointview == 0 ) 
			{
				waypoint_showallpaths ( );
			}
			else
			{
				waypoint_hideallpaths ( );
			}
		}
	}

	// clear any gridentity light if gridentity no longer used
	if (t.gridentity == 0)
	{
		if (t.gridentitywickedlightindex > 0)
		{
			WickedCall_DeleteLight(t.gridentitywickedlightindex);
			t.gridentitywickedlightindex = 0;
		}
	}
}

void editor_visuals ( void )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	//  Control entity selection and alpha of layers
	if (  t.refreshgrideditcursor == 1 ) 
	{
		gridedit_recreateentitycursor ( );
		t.refreshgrideditcursor=0;
	}
	gridedit_displayentitycursor ( );

	//  Update Camera
	editor_camera ( );
}

void editor_camera(void)
{
	// Camera Mode
	float fFlatFloorY = BT_GetGroundHeight(t.terrain.TerrainID, t.editorfreeflight.c.x_f, t.editorfreeflight.c.z_f) + 100.0f;
	if (!bProceduralLevel && !bStoryboardWindow && !bWelcomeScreen_Window && t.game.gameisexe == 0 && !bImGuiInTestGame )
	{
		bool bCameraOutSideEditArea = false;
		float fEditableSizeHalved = GGTerrain_GetEditableSize();
		t.terraineditableareasizeminx = -fEditableSizeHalved;
		t.terraineditableareasizeminz = -fEditableSizeHalved;
		t.terraineditableareasizemaxx = fEditableSizeHalved;
		t.terraineditableareasizemaxz = fEditableSizeHalved;
		if (CameraPositionX() < t.terraineditableareasizeminx) { bCameraOutSideEditArea = true; }
		if (CameraPositionX() > t.terraineditableareasizemaxx) { bCameraOutSideEditArea = true; }
		if (CameraPositionZ() < t.terraineditableareasizeminz) { bCameraOutSideEditArea = true; }
		if (CameraPositionZ() > t.terraineditableareasizemaxz) { bCameraOutSideEditArea = true; }
		if (bCameraOutSideEditArea)
		{
			//Trigger warning.
			sprintf(cSmallTriggerMessage, "Outside of editable area, you cannot add objects or change the terrain here. Press spacebar to recenter.");

			if(t.inputsys.keyspace == 1)
			{
				// Recentre camera.
				t.inputsys.keyspace = 0;
				
				// Get terrain height at centre.
				float yHit = 0.0f;
				GGTerrain::GGTerrain_GetHeight(0, 0, &yHit);
				yHit += 100.0f;

				// Ensure the camera will be placed above the water.
				if (yHit < (g.gdefaultwaterheight + 100.0f))
					yHit = g.gdefaultwaterheight + 100.0f;

				t.editorfreeflight.mode = 3;
				t.editorfreeflight.s.x_f = 0;
				t.editorfreeflight.s.y_f = yHit;
				t.editorfreeflight.s.z_f = 0;
				t.editorfreeflight.s.angx_f = 0.0f;
				t.editorfreeflight.s.angy_f = 0.0f;
				t.editorfreeflight.c = t.editorfreeflight.s;
			
				
			}
			iTriggerMessageFrames = 15;
			bTriggerSmallMessage = true;

		}
	}

	static bool bPressedFKey = false;
	switch ( t.cameraviewmode )
	{
		//PE: Disable zoom view. in wicked , i got some wierd results with lensflare from wicked so you could not see the object.
		//PE: Also the zoom it is a distraction.
		case 2:
		case 0:
		{
			//  Control free flight camera viewing angle (mouselook)
			#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
			//PE: Delta already reset , so use t.inputsys.xmousemove,y
			if (g.gminvert == 1)  t.ttmousemovey = t.inputsys.xmousemove*-1; else t.ttmousemovey = t.inputsys.ymousemove;
			t.cammousemovex_f = t.inputsys.xmousemove;
			#else
			if (g.gminvert == 1)  t.ttmousemovey = MouseMoveY()*-1; else t.ttmousemovey = MouseMoveY();
			t.cammousemovex_f = MouseMoveX();
			#endif

			//PE: If outside 3D area.
			static bool bBlockRightMouseButton = false;
			if (ImGui::IsMouseDown(1) && !bImGuiRenderTargetFocus) bBlockRightMouseButton = true;
			if (!ImGui::IsMouseDown(1))  bBlockRightMouseButton = false;
			if(bImGuiRenderTargetFocus && !bBlockRightMouseButton)

			{
				t.cammousemovey_f = t.ttmousemovey;
				if (t.inputsys.mclick == 0)  t.inputsys.mclickreleasestate = 0;
				t.trmb = 0;
				if (t.inputsys.mclick == 2 && t.inputsys.mclickreleasestate == 0)
				{
					#if defined(ENABLEIMGUI) && !defined(USEOLDGUI)
					if (g.mouseishidden == 1)
					{
						ImVec2 setPos;
						//PE: Always center relative to window position , or you cant have a small window at the right of screen.
						RECT rect;
						GetWindowRect(g_pGlob->hWnd, &rect);
						setPos = { rect.left + (OldrenderTargetSize.x*0.5f) + OldrenderTargetPos.x , rect.top + (OldrenderTargetSize.y*0.5f) + OldrenderTargetPos.y };
						setPos.x = (int)setPos.x;
						setPos.y = (int)setPos.y;
						SetCursorPos(setPos.x, setPos.y);
						xmouseold = setPos.x;
						ymouseold = setPos.y;
					}
					#endif
					t.trmb = 1;
				}
				if (t.inputsys.mclick == 4 && t.inputsys.mclickreleasestate == 0)  t.trmb = 2;
				if (t.trmblock == 0)
				{
					if (t.cammousemovex_f != 0 || t.cammousemovex_f != 0 || t.inputsys.kscancode != 0)  t.trmblock = 1;
				}
				else
				{
					if (t.inputsys.mclick == 0)  t.trmblock = 0;
				}
				if (t.trmblock == 0)  t.trmb = 0;
				if (g.globals.disablefreeflight == 1)  t.trmb = 0;
				if (t.trmb != 0)
				{
					if (g.mouseishidden == 0)
					{
						game_hidemouse();
						#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
						POINT tmp;
						GetCursorPos(&tmp);
						t.editorfreeflight.storemousex = tmp.x;
						t.editorfreeflight.storemousey = tmp.y;
						#else
						t.editorfreeflight.storemousex = t.inputsys.xmouse;
						t.editorfreeflight.storemousey = t.inputsys.ymouse;
						#endif
					}
					if (t.editorfreeflight.mode == 0)
					{
						t.editorfreeflight.mode = 1; t.updatezoom = 1;
						t.editorfreeflight.c.x_f = t.cx_f;
						t.editorfreeflight.c.y_f = fFlatFloorY + (50.0f*t.gridzoom_f);
						t.editorfreeflight.c.z_f = t.cy_f;
						t.editorfreeflight.c.angx_f = CameraAngleX();
						t.editorfreeflight.c.angy_f = CameraAngleY();
					}
					else
					{
						if (t.trmb == 1)
						{
							// rotate with RMB
							#if defined(ENABLEIMGUI) && !defined(USEOLDIDE)
							//PE: a bit more smooth.
							t.tRotationDivider_f = 6.0;
							#else
							t.tRotationDivider_f = 5.0;
							#endif
							t.editorfreeflight.c.angx_f = CameraAngleX() + (t.cammousemovey_f / t.tRotationDivider_f);
							t.editorfreeflight.c.angy_f = CameraAngleY() + (t.cammousemovex_f / t.tRotationDivider_f);
							if (t.editorfreeflight.c.angx_f > 180.0f)  t.editorfreeflight.c.angx_f = t.editorfreeflight.c.angx_f - 360.0f;
							if (t.editorfreeflight.c.angx_f < -89.999f)  t.editorfreeflight.c.angx_f = -89.999f;
							if (t.editorfreeflight.c.angx_f > 89.999f)  t.editorfreeflight.c.angx_f = 89.999f;
						}
					}
					//Always display skybox.
					sky_loop();
				}
				else
				{
					if (g.mouseishidden == 1)
					{
						t.tideframestartx = 70; t.tideframestarty = 15;
						#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
						//PE: Restore mouse pos.
						SetCursorPos(t.editorfreeflight.storemousex, t.editorfreeflight.storemousey);
						xmouseold = t.editorfreeflight.storemousex;
						ymouseold = t.editorfreeflight.storemousey;
						t.inputsys.xmouse = xmouseold;
						t.inputsys.xmouse = ymouseold;
						game_showmouse();
						g.mouseishidden = 0;
						#else
						t.inputsys.xmouse = ((t.tideframestartx + t.editorfreeflight.storemousex + 0.0) / 800.0)*(GetDisplayWidth() + 0.0);
						t.inputsys.ymouse = ((t.tideframestarty + t.editorfreeflight.storemousey + 0.0) / 600.0)*(GetDisplayHeight() + 0.0);
						game_showmouse_restore_mouse(); //PE: Will use exact mouse positon stored by editor when hiding mouse. (if available, else t.tideframestartx...)
						#endif
						t.terrain.X_f = 999999; t.terrain.Y_f = 999999;
					}
				}
			}
			static int delayedNewLevelCamera = 0;
			if (delayedNewLevelCamera > 0)
			{
				delayedNewLevelCamera--;
				//PE: It can take up to 60 frames before we have a terrain height here. so just keep checking.
				t.tcurrenth_f = BT_GetGroundHeight(t.terrain.TerrainID, t.editorfreeflight.c.x_f, t.editorfreeflight.c.z_f);
				if (t.tcurrenth_f != GGORIGIN_Y) delayedNewLevelCamera = 0; //We got a height.
				if (delayedNewLevelCamera == 0)
				{
					t.editorfreeflight.mode = 3;
					t.editorfreeflight.s = t.editorfreeflight.c;
					t.editorfreeflight.s.y_f = t.tcurrenth_f + 325.0; //125.0;
					t.editorfreeflight.s.angy_f = -45.0f;
					t.editorfreeflight.s.angx_f = 16.0f;
				}
			}
			// Handle free flight camea movement
			if (t.inputsys.k_s != "f") bPressedFKey = false;
			if (t.editorfreeflight.mode == 0)
			{
				t.editorfreeflight.c.x_f = t.cx_f;
				t.editorfreeflight.c.y_f = fFlatFloorY + (50.0f*t.gridzoom_f);
				t.editorfreeflight.c.z_f = t.cy_f;
				bool bSwitchToFFView = false;
				if (g_bResetCameraToFreeFlightOnNewLevel == true) 
				{ 
					// extra condition to only 'zoom in' after Welcome screen exits
					// as it looks cooler and hides the launch load stutters
					if (iTriggerWelcomeSystemStuff == 0)
					{
						bSwitchToFFView = true;
						g_bResetCameraToFreeFlightOnNewLevel = false;
					}
				}
				if (t.inputsys.k_s == "f" && bPressedFKey == false && g.globals.disablefreeflight == 0) { bPressedFKey = true;  bSwitchToFFView = true; }
				if (bSwitchToFFView == true )
				{
					// top down back to last free flight
					t.editorfreeflight.mode = 3;

					t.editorfreeflight.s = t.editorfreeflight.c;

					t.tcurrenth_f = BT_GetGroundHeight(t.terrain.TerrainID, t.editorfreeflight.c.x_f, t.editorfreeflight.c.z_f);
					if (t.tcurrenth_f != GGORIGIN_Y)
					{
						t.editorfreeflight.s.y_f = t.tcurrenth_f + 325.0; //125.0;
					}
					else
					{
						//Start up high. as we dont have the real height at this point.
						t.editorfreeflight.s.y_f = 2600.0;
						delayedNewLevelCamera = 100; //PE: Terrain need a few frames before we can set the camera.
					}

					t.editorfreeflight.s.angy_f = -45.0f;
					t.editorfreeflight.s.angx_f = 16.0f;

				}
			}
			if ( t.editorfreeflight.mode == 1 ) 
			{
				static float fAccelerationTimer = 0.0f;
				if (t.inputsys.k_s == "f"  && bPressedFKey == false && t.inputsys.keycontrol == 0 && t.importer.importerActive == 0)
				{
					// free flight to top down
					bPressedFKey = true;
					t.editorfreeflight.s=t.editorfreeflight.c;
					t.cx_f=t.editorfreeflight.c.x_f;
					t.cy_f=t.editorfreeflight.c.z_f;
					t.editorfreeflight.mode=2;
				}
				if (  t.inputsys.keyup == 1  )  t.plrkeyW = 1; else t.plrkeyW = 0;
				if (  t.inputsys.keyleft == 1  )  t.plrkeyA = 1; else t.plrkeyA = 0;
				if (  t.inputsys.keydown == 1  )  t.plrkeyS = 1; else t.plrkeyS = 0;
				if (  t.inputsys.keyright == 1  )  t.plrkeyD = 1; else t.plrkeyD = 0;

				//  mouse wheel mimmics W and S when no CONTROL key pressed (170616 - but not when in EBE mode as its used for grid layer control)
				int usingWheel = 0;
				if ( t.ebe.on == 0 )
				{
					if (  t.inputsys.keycontrol == 0 ) 
					{
						if (  t.inputsys.wheelmousemove<0 || t.inputsys.dozoomout == 1) { t.plrkeyS = 1; usingWheel = 1; }
						if (  t.inputsys.wheelmousemove>0 || t.inputsys.dozoomin == 1 ) { t.plrkeyW = 1; usingWheel = 1; }
					}
				}
				t.traise_f=0.0;
				if (  t.inputsys.keyshift == 1 ) 
				{
					fAccelerationTimer += g.timeelapsed_f * 0.005f;
					if (fAccelerationTimer > 1.0f) fAccelerationTimer = 1.0f;
					t.tffcspeed_f=10.0*g.timeelapsed_f;
				}
				else
				{
					fAccelerationTimer = 0.0f;
					if (  t.inputsys.keycontrol == 1 ) 
					{
						t.tffcspeed_f=1.0*g.timeelapsed_f;
					}
					else
					{
						t.tffcspeed_f=5.0*g.timeelapsed_f;
					}
				}
				if (g_bCharacterCreatorPlusActivated) 
				{
					//Slow down movement when i CCP.
					t.tffcspeed_f *= 0.25;
				}

				// Only increase movement speed when not in the importer or CCP.
				if (t.importer.importerActive == 0 && !g_bCharacterCreatorPlusActivated)
				{
					// Feedback is that changing camera speed based on distance to ground is jerky and not good
					// so change to a method where the longer you hold down the shift key, the faster you go
					bool bBetterCameraSpeedOverTime = true;
					if (bBetterCameraSpeedOverTime==true)//t.visuals.bEnableEmptyLevelMode == true)
					{
						// modify movement speed based on time holding down shift
						float modifier =  100 * fAccelerationTimer;
						if (modifier > 150) modifier = 150;
						if (modifier < 2) modifier = 2;
						t.tffcspeed_f *= modifier;
					}
				}

				// speed up wheel movement
				if ( usingWheel ) t.tffcspeed_f *= 4;

				if (t.inputsys.k_s == "e")  t.traise_f = -90;
				if (t.inputsys.k_s == "q")  t.traise_f = 90;
				PositionCamera (  t.editorfreeflight.c.x_f,t.editorfreeflight.c.y_f,t.editorfreeflight.c.z_f );
				
				if (  t.plrkeyW == 1  )
					MoveCamera (  t.tffcspeed_f );

				if (  t.plrkeyS == 1  )  MoveCamera (  t.tffcspeed_f*-1 );
				if (  t.plrkeyA == 1 ) { RotateCamera (  0,t.editorfreeflight.c.angy_f-90,0  ) ; MoveCamera (  t.tffcspeed_f ); }
				if (  t.plrkeyD == 1 ) { RotateCamera (  0,t.editorfreeflight.c.angy_f+90,0  ) ; MoveCamera (  t.tffcspeed_f ); }
				if (  t.traise_f != 0 ) { RotateCamera (  t.traise_f,0,0  ) ; MoveCamera (  t.tffcspeed_f ); }
				if (  t.inputsys.mclick == 4 ) 
				{
					//  new middle mouse panning
					RotateCamera (  0,t.editorfreeflight.c.angy_f,0 );
					MoveCamera (  t.cammousemovey_f*-2 );
					if (  t.cammousemovex_f<0 ) { RotateCamera (  0,t.editorfreeflight.c.angy_f-90,0  ) ; MoveCamera (  abs(t.cammousemovex_f*2) ); }
					if (  t.cammousemovex_f>0 ) { RotateCamera (  0,t.editorfreeflight.c.angy_f+90,0  ) ; MoveCamera (  t.cammousemovex_f*2 ); }
				}
				t.editorfreeflight.c.x_f=CameraPositionX();
				t.editorfreeflight.c.y_f=CameraPositionY();
				t.editorfreeflight.c.z_f=CameraPositionZ();

				//Always display skybox.
				sky_loop();
			}

			//  view mode transitions
			if (  t.editorfreeflight.mode == 2 ) 
			{
				//  from free flight to top down
				t.tcamheight_f= fFlatFloorY+(50.0f*t.gridzoom_f);
				t.editorfreeflight.c.x_f=CurveValue(t.cx_f,CameraPositionX(),10.0);
				t.editorfreeflight.c.y_f=CurveValue(t.tcamheight_f,CameraPositionY(),10.0);
				t.editorfreeflight.c.z_f=CurveValue(t.cy_f,CameraPositionZ(),10.0);
				if (  abs(t.editorfreeflight.c.y_f-t.tcamheight_f)<20.0 ) 
				{
					t.editorfreeflight.mode=0 ; t.updatezoom=1;
				}
			}
			if (  t.editorfreeflight.mode == 3 ) 
			{
				//  from top down to free flight storage
				t.editorfreeflight.c.x_f=CurveValue(t.editorfreeflight.s.x_f,CameraPositionX(),10.0);
				t.editorfreeflight.c.y_f=CurveValue(t.editorfreeflight.s.y_f,CameraPositionY(),10.0);
				t.editorfreeflight.c.z_f=CurveValue(t.editorfreeflight.s.z_f,CameraPositionZ(),10.0);
				if (  abs(t.editorfreeflight.c.x_f-t.editorfreeflight.s.x_f)<20.0 && abs(t.editorfreeflight.c.y_f-t.editorfreeflight.s.y_f)<20.0 && abs(t.editorfreeflight.c.z_f-t.editorfreeflight.s.z_f)<20.0 ) 
				{
					t.editorfreeflight.c.x_f=t.editorfreeflight.s.x_f;
					t.editorfreeflight.c.y_f=t.editorfreeflight.s.y_f;
					t.editorfreeflight.c.z_f=t.editorfreeflight.s.z_f;
					t.editorfreeflight.mode=1 ; t.updatezoom=1;
				}
			}

			//  ensure camera NEVER goes into Floor (  )
			//PE: In wicked after loading a new fpm. we need some frames before terrain height is ready.
			if(iDelayedCameraRestore > 0)
			{
				iDelayedCameraRestore--;
			}
			else
			{
				t.tcurrenth_f = BT_GetGroundHeight(t.terrain.TerrainID, t.editorfreeflight.c.x_f, t.editorfreeflight.c.z_f) + 10.0;
					if (t.editorfreeflight.c.y_f < t.tcurrenth_f)
					{
						t.editorfreeflight.c.y_f = t.tcurrenth_f;
					}

				if (t.editorfreeflight.s.y_f < t.tcurrenth_f)
				{
					t.editorfreeflight.s.y_f = t.tcurrenth_f;
				}

			}
			//  update camera for free flight or top down modes
			PositionCamera (t.editorfreeflight.c.x_f, t.editorfreeflight.c.y_f, t.editorfreeflight.c.z_f);
			if (t.editorfreeflight.mode == 0)
			{
				PointCamera (t.cx_f, -99999, t.cy_f);
			}
			if (t.editorfreeflight.mode == 1)
			{
				RotateCamera (t.editorfreeflight.c.angx_f, t.editorfreeflight.c.angy_f, 0);
			}
			if (t.editorfreeflight.mode == 2)
			{
				t.editorfreeflight.c.angx_f = CurveAngle(90, CameraAngleX(), 10.0);
				t.editorfreeflight.c.angy_f = CurveAngle(0, CameraAngleY(), 10.0);
				RotateCamera (t.editorfreeflight.c.angx_f, t.editorfreeflight.c.angy_f, 0);
			}
			if (t.editorfreeflight.mode == 3)
			{
				t.editorfreeflight.c.angx_f = CurveAngle(t.editorfreeflight.s.angx_f, CameraAngleX(), 10.0);
				t.editorfreeflight.c.angy_f = CurveAngle(t.editorfreeflight.s.angy_f, CameraAngleY(), 10.0);
				RotateCamera (t.editorfreeflight.c.angx_f, t.editorfreeflight.c.angy_f, 0);
			}

			//  view mode prompt (top right status Text ( ) )
			if (  t.editorfreeflight.mode == 0 || t.editorfreeflight.mode == 2 ) 
			{
				t.t_s="TOP DOWN VIEW ('F' to toggle)";
				bEditorInFreeFlightMode = false;
			}
			else
			{
				t.t_s="FREE FLIGHT VIEW ('G' to toggle)";
				bEditorInFreeFlightMode = true;
			}
			#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
			#else
			t.ttxtwid=getbitmapfontwidth(t.t_s.Get(),2);
			pastebitmapfont(t.t_s.Get(),GetChildWindowWidth(0)-8-t.ttxtwid,4,2,228);
			#endif
		}
		break;

		case 999:
			//  process live updates from
			interface_live_updates ( );

			//  update camera XZ with entity if editing position
			if (  t.gridentityinzoomview>0 ) 
			{
				t.cx_f=t.zoomviewtargetx_f ; t.cy_f=t.zoomviewtargetz_f;
			}

			//  if third person start marker mode, override range and angle
			t.tlayerheight_f=t.layerheight_f;
			if (  t.playercontrol.thirdperson.enabled == 1 ) 
			{
				t.zoomviewcamerarange_f=t.playercontrol.thirdperson.livecameradistance;
				t.zoomviewcameraheight_f=t.playercontrol.thirdperson.livecameraheight;
				t.zoomviewcamerafocus_f=t.playercontrol.thirdperson.livecamerafocus;
				t.zoomviewcamerashoulder_f=t.playercontrol.thirdperson.livecamerashoulder;
				if (  t.gridentityobj>0 ) 
				{
					if (  ObjectExist(t.gridentityobj) == 1 ) 
					{
						t.zoomviewcameraangle_f=(0-ObjectAngleY(t.gridentityobj));
						t.tlayerheight_f=ObjectPositionY(t.gridentityobj);
					}
				}
			}
			else
			{
				t.zoomviewcamerafocus_f=0;
				t.zoomviewcamerashoulder_f=0;
			}

			//  calculate view from position
			t.daa_f=WrapValue(180-t.zoomviewcameraangle_f);
			t.dcx_f=t.cx_f+(Sin(t.daa_f)*t.zoomviewcamerarange_f);
			t.dcy_f=t.tlayerheight_f+t.zoomviewcameraheight_f;
			t.dcz_f=t.cy_f+(Cos(t.daa_f)*t.zoomviewcamerarange_f);
			t.tcx_f=CurveValue(t.dcx_f,CameraPositionX(),4.0);
			t.tcy_f=CurveValue(t.dcy_f,CameraPositionY(),2.0);
			t.tcz_f=CurveValue(t.dcz_f,CameraPositionZ(),4.0);

			//  if target was entity, view center of it
			if (  t.gridentityinzoomview>0 ) 
			{
				t.tobj=t.entityelement[t.gridentityinzoomview].profileobj;
				if (  t.tobj>0 ) 
				{
					t.viewatx_f=t.cx_f ; t.viewaty_f=t.zoomviewtargety_f+ObjectSizeY(t.tobj)/2.0 ; t.viewatz_f=t.cy_f;
				}
				else
				{
					t.viewatx_f=t.cx_f ; t.viewaty_f=t.zoomviewtargety_f+5 ; t.viewatz_f=t.cy_f;
				}
			}
			else
			{
				t.viewatx_f=t.cx_f ; t.viewaty_f=t.zoomviewtargety_f+5 ; t.viewatz_f=t.cy_f;
			}

			//  set smoothed camera view
			PositionCamera (  t.tcx_f,t.tcy_f,t.tcz_f );
			PointCamera (  t.viewatx_f,t.viewaty_f,t.viewatz_f );
			t.tcamax_f=CameraAngleX() ; t.tcamay_f=CameraAngleY() ; t.tcamaz_f=CameraAngleZ();
			RotateCamera (  0,t.tcamay_f+90,0 );
			MoveCamera (  t.zoomviewcamerashoulder_f );
			RotateCamera (  t.tcamax_f-t.zoomviewcamerafocus_f,t.tcamay_f,t.tcamaz_f );

			//Always display skybox.
			sky_loop();
			
		break;
	}
}

void editor_undoredoprojectstate ( void )
{
	// set as modified
	g.projectmodified=1 ; gridedit_changemodifiedflag ( );
	g.projectmodifiedstatic = 1;
}

void editor_cutcopyclearstate ( void )
{
	//  control enabling of UNDO REDO menu items
	OpenFileMap (  1, "FPSEXCHANGE" );
	SetFileMapDWORD (  1, 474, 0 );
	SetFileMapDWORD (  1, 478, 0 );
	SetFileMapDWORD (  1, 482, 0 );
	SetEventAndWait (  1 );
}

void editor_undo ( void )
{
	// undo last stage
	if ( t.ebe.on == 1 )
	{
		ebe_undo();
	}
	else
	{
		// all handled inside new undo/redo system
		entity_undo ( );
	}
}

void editor_redo ( void )
{
	if ( t.ebe.on == 1 )
	{
		ebe_redo();
	}
	else
	{
		// all handled inside new undo/redo system
		entity_redo ( );
	}
}

void gridedit_showtobjlegend ( void )
{
	t.relaytostatusbar_s="";
	if (  t.tobj>0 ) 
	{
		if (  ObjectExist(t.tobj)>0 ) 
		{
			if (  t.taddstaticlegend == 1 ) 
			{
				//  static
				t.tname_s=t.tname_s+" "+t.strarr_s[608];
			}
			else
			{
				//  dynamic
				t.tname_s=t.tname_s+" "+t.strarr_s[609];
			}
			if (  t.gridedit.autoflatten == 1  )  t.tname_s = t.tname_s+"(autoflatten)";
			if (  t.gridedit.entityspraymode == 1  )  t.tname_s = t.tname_s+"(spray mode)";
			t.relaytostatusbar_s=t.tname_s;
		}
	}
	return;
}

void editor_checkIfInSubApp ( void )
{
	t.result = 0;
}

int findentitycursorobj ( int currentlyover )
{
	#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
	#endif

	if (pref.iDragCameraMovement && t.ebe.on == 0 && bDragCameraActive)
		return 0;

	// Uses simpler system to detect what is under cursor
	int result = 0;
	uint64_t hitentity = 0;

	//PE: We already sent a ray reuse data.
	if (iReusePickObjectID != -1)
	{
		g.glastpickedx_f = fReusePickHitX;
		g.glastpickedy_f = fReusePickHitY;
		g.glastpickedz_f = fReusePickHitZ;
		t.lastfindentitycursorobj = iReusePickEntityID;
		return(iReusePickEntityID);
	}

	float fHitX, fHitY, fHitZ;
	WickedCall_GetPick(&fHitX, &fHitY, &fHitZ, NULL, NULL, NULL, &hitentity, GGRENDERLAYERS_NORMAL | GGRENDERLAYERS_TERRAIN); // LB: Added GGRENDERLAYERS_TERRAIN so cannot select objects UNDER the terrain!
	if (hitentity>0)
	{
		iLastHitObjectID = 0;

		// found object under hovering cursor, match to entity index
		sObject* pHitObject = m_ObjectManager.FindObjectFromWickedObjectEntityID(hitentity);
		int iHitObjectEntityElementE = -1;
		if (pHitObject)
		{
			for (int e = 1; e <= g.entityelementlist; e++)
			{
				if (t.entityelement[e].obj == pHitObject->dwObjectNumber)
				{
					iHitObjectEntityElementE = e;
					break;
				}
			}
		}
		if (pref.iEnableDragDropStopSelectFromInside == 1)
		{
			// control whether can select an object from the inside
			if (iHitObjectEntityElementE != -1)
			{
				int entid = t.entityelement[iHitObjectEntityElementE].bankindex;
				if (entid > 0 && t.entityprofile[entid].ismarker == 0)
				{
					// but only if regular object (like a building, etc, not a particle marker or light)
					if (pHitObject && CameraInsideObject(pHitObject) == true) pHitObject = NULL;
				}
			}
		}
		if (pHitObject)
		{
			int e = iHitObjectEntityElementE;
			if(e > 0)
			{
				iLastHitObjectID = pHitObject->dwObjectNumber;
				g.glastpickedx_f = fHitX;
				g.glastpickedy_f = fHitY;
				g.glastpickedz_f = fHitZ;
				result = e;
			}
		}
	}
	t.lastfindentitycursorobj = result;
	return result;
}

void gridedit_clearentityrubberbandlist ( void )
{
	if ( g.entityrubberbandlist.size() > 0 )
	{
		for ( int i = 0; i < (int)g.entityrubberbandlist.size(); i++ )
		{
			int e = g.entityrubberbandlist[i].e;
			//PE: Got exception here: e was to large.
			if (e <= t.entityelement.size()) {
				int tobj = t.entityelement[e].obj;
				if (tobj > 0)
				{
					int mi = t.entityelement[e].bankindex;
					if(mi > 0 && t.entityprofile[mi].bIsDecal) //PE: Got crash here, should be bankindex.
						SetupDecalObject(tobj, e);
					else
					{
						SetAlphaMappingOn(tobj, 100);
						//PE: Restore original material.
						sObject* pObject = g_ObjectList[tobj];
						if (pObject)
						{
							WickedSetEntityId(mi);
							WickedSetElementId(e);
							for (int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++)
							{
								WickedSetMeshNumber(iMesh);
								WickedCall_SetMeshMaterial(pObject->ppMeshList[iMesh], false);
							}
							WickedSetEntityId(-1);
							WickedSetElementId(0);
						}

					}
				}
			}
		}
	}
	g.entityrubberbandlist.clear();
}

void gridedit_addEntityToRubberBandHighlights ( int e )
{
	// 011215 - skip if in marker mode and not a marker
	if ( t.gridentitymarkersmodeonly == 1 && t.entityprofile[t.entityelement[e].bankindex].ismarker==0 ) 
		return;

	#ifdef ALLOWSELECTINGLOCKEDOBJECTS
	if (t.entityelement[e].editorlock) return;
	#endif
	// add entity to rubber band
	int tobj = t.entityelement[e].obj;
	bool bEntityIsHighlighted = false;
	if ( g.entityrubberbandlist.size() > 0 )
	{
		for ( int i = 0; i < (int)g.entityrubberbandlist.size(); i++ )
		{
			int thise = g.entityrubberbandlist[i].e;
			if ( e == thise ) bEntityIsHighlighted = true;
		}
	}
	if ( bEntityIsHighlighted == false )
	{
		sRubberBandType rubberbandItem;
		rubberbandItem.e = e;
		rubberbandItem.x = t.entityelement[e].x;
		rubberbandItem.y = t.entityelement[e].y;
		rubberbandItem.z = t.entityelement[e].z;
		rubberbandItem.px = t.entityelement[e].x;
		rubberbandItem.py = t.entityelement[e].y;
		rubberbandItem.pz = t.entityelement[e].z;
		rubberbandItem.rx = t.entityelement[e].rx;
		rubberbandItem.ry = t.entityelement[e].ry;
		rubberbandItem.rz = t.entityelement[e].rz;	
		rubberbandItem.quatmode = t.entityelement[e].quatmode;
		rubberbandItem.quatx = t.entityelement[e].quatx;
		rubberbandItem.quaty = t.entityelement[e].quaty;
		rubberbandItem.quatz = t.entityelement[e].quatz;
		rubberbandItem.quatw = t.entityelement[e].quatw;
		rubberbandItem.scalex = t.entityelement[e].scalex;
		rubberbandItem.scaley = t.entityelement[e].scaley;
		rubberbandItem.scalez = t.entityelement[e].scalez;
		g.entityrubberbandlist.push_back ( rubberbandItem );

		if ( t.entityelement[e].staticflag == 0 ) 
			SetAlphaMappingOn ( tobj, 103 );
		else
			SetAlphaMappingOn ( tobj, 101 );
	}
}

void gridedit_mapediting ( void )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif

	//  Determine if cursor is at rest, allows performance boost to skip
	//  expensive ray casts to find entities in map
	if (  t.inputsys.atrest != 2 ) 
	{
		t.inputsys.atrest=0;
		if (  t.inputsys.localx_f == t.inputsys.atrestx && t.inputsys.localy_f == t.inputsys.atresty ) 
		{
			t.inputsys.atrest=1;
		}
		//  never at rest when widget menu active
		if (  t.widget.pickedSection != 0  )  t.inputsys.atrest = 0;
	}
	else
	{
		if (  t.inputsys.localx_f != t.inputsys.atrestx || t.inputsys.localy_f != t.inputsys.atresty ) 
		{
			t.inputsys.atrest=0;
		}
	}

	t.inputsys.atrestx=t.inputsys.localx_f;
	t.inputsys.atresty=t.inputsys.localy_f;

	//  flag to determine if character will attach to start marker
	t.inputsys.willmakethirdperson=0;

	//  Only if within map
	if (  t.inputsys.mmx >= 0 && t.inputsys.mmy >= 0 && t.inputsys.mmx<t.maxx && t.inputsys.mmy<t.maxy ) 
	{
		//  Any click inside 3D area constitues some sort of edit
		if ( t.inputsys.mclick != 0 ) 
		{ 
			g.projectmodified = 1; 
			gridedit_changemodifiedflag ( ); 
			// effect on g.projectmodifiedstatic
		}

		//  ENTITY EDIT Handling (onedrag=0 means no waypoint dragging)
		if (  t.grideditselect !=  5  )  t.tentitytoselect  =  0;

		if (  t.grideditselect == 5 && t.onedrag == 0 ) 
		{
			//  Regular entity editing
			t.layer=t.gridlayer ; t.mx=t.inputsys.mmx ; t.my=t.inputsys.mmy;

			if (  t.selstage == 0 ) 
			{
				//  single entity highlight
				t.tshowasstatic=0 ; t.showentityid=0 ; t.tforcedynamic=0 ; t.tentitytoselect=0;

				if ( t.gridentity == 0 ) 
				{
					//  no entity attached to cursor (if RMB, deactivate entity detection for smoother moving)
					if ( t.widget.activeObject == 0 && t.inputsys.xmouse != 500000 && t.inputsys.mclick != 2 && t.inputsys.rubberbandmode == 0 ) 
					{
						if ( t.inputsys.atrest == 1 || t.inputsys.keyspace == 1 )
						{
							if(t.inputsys.mclick != 0)
								iReusePickObjectID = -1; //PE: Do a fresh raycast.
							t.tentitytoselect = findentitycursorobj(-1);
							t.tlasttentitytoselect = t.tentitytoselect;
							t.inputsys.atrest = 2;
						}
						else
						{
							// wicked fast enough to do this test each frame
							if (t.inputsys.mclick != 0)
								iReusePickObjectID = -1; //PE: Do a fresh raycast.
							t.tentitytoselect = findentitycursorobj(-1);
						}
						//Group edit mode ?
						if (t.tentitytoselect > 0 && current_selected_group >= 0 && group_editing_on)
						{
							//Only allow selection within group.
							int grouplist = isEntityInGroupList(t.tentitytoselect);
							if (grouplist < 0 || current_selected_group != grouplist) //Dont allow selecting objects from another group.
								t.tentitytoselect = 0;
						}
					}
					else
					{
						t.tentitytoselect=0;
					}

					bool bActivateRubberBand = true;
					if (bDotObjectDragging || (g_hovered_dot_pobject && t.inputsys.rubberbandmode == 0) ) {
						bActivateRubberBand = false;
					}
					if (pref.iEnableDragDropEntityMode) {
						if (bWaitOnMouseRelease || iDragDropActive > 0 ) {
							t.inputsys.rubberbandmode = 0;
							bActivateRubberBand = false;
						}
					}
					if (current_selected_group >= 0 && group_editing_on)
					{
						t.inputsys.rubberbandmode = 0;
						bActivateRubberBand = false;
					}

					// when holding SHIFT, we may want to rubber band, even if hovering over an object
					bool bSHIFTForRubberBand = false;
					if (t.inputsys.keyshift == 1)
					{
						if (t.tentitytoselect > 0)
						{
							// ensure no object already in selection when start rubber banding
							t.tentitytoselect = 0;
						}
						bSHIFTForRubberBand = true;
					}

					//PE: Added ctrl allow selecting multiply objects.
					if (pref.iDragCameraMovement && t.ebe.on == 0 && t.inputsys.keyshift == 0 && t.inputsys.keycontrol == 0)
					{
						bActivateRubberBand = false;
						//PE: Still support unselecting rubberband.
						if (t.inputsys.mclick == 1 && t.inputsys.rubberbandmode == 0 && t.widget.activeObject == 0)
						{
							if (t.inputsys.keycontrol == 0)
							{
								if (pref.iEnableDragDropEntityMode && g_hovered_pobject) 
								{
									//hover over object and click on object, this should trigger a move in the new system.
								}
								else
								{
									gridedit_clearentityrubberbandlist();
								}
							}
						}
						if (t.inputsys.rubberbandmode == 1)
						{
							bool bCancelRubberBand = false;
							if (pref.iEnableDragDropEntityMode) 
							{
								if (bWaitOnMouseRelease)
									bCancelRubberBand = true;
							}
							if (t.inputsys.xmouse == 500000 || bCancelRubberBand)
							{
								// mouse left area, cancel rubber band
								t.inputsys.rubberbandmode = 0;
							}
						}
					}
					if (pref.iDragCameraMovement && t.ebe.on == 0 && t.inputsys.keycontrol == 1 && bActivateRubberBand)
					{
						bActivateRubberBand = false;
						// if clicked a single entity WHILE holding control, can add to list (need to click to select)
						if (t.inputsys.mclick == 1 && t.inputsys.keycontrol == 1 && t.tentitytoselect > 0)
						{
							if (g.entityrubberbandlist.size() > 0)
							{
								//PE: When using ctrl to add to rubberband. make sure no locked items is inside rubberband.
								for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
								{
									int e = g.entityrubberbandlist[i].e;
									if (e > 0 && t.entityelement[e].editorlock)
									{
										//Locked obects in list, start new list.
										g.entityrubberbandlist.clear();
									}
								}
							}
							if (t.widget.pickedEntityIndex > 0)
							{
								// add initial selected object if not in list already
								bool bAlreadyInList = false;
								for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
								{
									int e = g.entityrubberbandlist[i].e;
									if (e == t.widget.pickedEntityIndex)
									{
										bAlreadyInList = true;
										break;
									}
								}
								if (bAlreadyInList == false)
								{
									gridedit_addEntityToRubberBandHighlights(t.widget.pickedEntityIndex);
								}
							}
							gridedit_addEntityToRubberBandHighlights(t.tentitytoselect);
						}
						// when select entity (widget called up), if parent to children, add them to rubberband so they can all be modified at the same time
						if (t.tentitytoselect > 0)
						{
							bool bHasChildren = false;
							t.tstoreentityindexofprimaryhightlighted = 0;
							for (int te = 1; te <= g.entityelementlist; te++)
							{
								if (t.entityelement[te].iHasParentIndex == t.tentitytoselect && t.entityelement[te].obj > 0)
								{
									gridedit_addEntityToRubberBandHighlights(te);
									editor_rec_addchildrentorubberband(te);
									bHasChildren = true;
								}
							}
							if (bHasChildren == true)
							{
								if (t.inputsys.k_s != "l")
								{
									gridedit_addEntityToRubberBandHighlights(t.tentitytoselect);
								}
							}
						}
					}

					if (bActivateRubberBand && t.inputsys.mclick == 1 && t.inputsys.rubberbandmode == 0 && t.widget.activeObject == 0 )
					{
						// clear any previous highlights
						if ( t.inputsys.keycontrol == 0 )
						{
							if (pref.iEnableDragDropEntityMode && g_hovered_pobject && bSHIFTForRubberBand == false)
							{
								// hover over object and click on object, this should trigger a move in the new system.
								// except when SHIFT held down which means we want to start a rubber band from the click!
							}
							else
							{
								gridedit_clearentityrubberbandlist();
							}
						}
						else
						{
							// if clicked a single entity WHILE holding control, can add to list
							if ( t.tentitytoselect > 0 )
							{
								gridedit_addEntityToRubberBandHighlights ( t.tentitytoselect );
							}
						}

						// when select entity (widget called up), if parent to children, add them to rubberband so they can all be modified at the same time
						if ( t.tentitytoselect > 0 && bSHIFTForRubberBand==false )
						{
							bool bHasChildren = false;
							t.tstoreentityindexofprimaryhightlighted = 0;
							for ( int te = 1; te <= g.entityelementlist; te++ )
							{
								if ( t.entityelement[te].iHasParentIndex == t.tentitytoselect && t.entityelement[te].obj > 0 )
								{
									gridedit_addEntityToRubberBandHighlights ( te );
									editor_rec_addchildrentorubberband ( te );
									bHasChildren = true;
								}
							}
							if ( bHasChildren == true )
							{
								if ( t.inputsys.k_s != "l" ) 
								{
									gridedit_addEntityToRubberBandHighlights ( t.tentitytoselect );
								}
							}
						}
					}

					if (bActivateRubberBand && t.tentitytoselect > 0 && t.entityelement[t.tentitytoselect].obj > 0 && bSHIFTForRubberBand == false)
					{
						// specific entity highlighted 
						PositionObject (  t.editor.objectstartindex+5,t.entityelement[t.tentitytoselect].x,t.entityelement[t.tentitytoselect].y,t.entityelement[t.tentitytoselect].z );
						t.showentityid=t.entityelement[t.tentitytoselect].bankindex;
						if (  t.entityprofile[t.showentityid].ismarker == 3 || t.entityprofile[t.showentityid].ismarker == 6 || t.entityprofile[t.showentityid].ismarker == 8 ) 
						{
							//  trigger zone or checkpoint
							t.tscale_f=100;
						}
						else
						{
							if (  t.entityprofile[t.showentityid].islightmarker == 1 ) 
							{
								t.tscale_f=(100/3.0)*2*(t.entityelement[t.tentitytoselect].eleprof.light.range/50.0);
							}
							else
							{
								t.tscale_f = get_cursor_scale_for_obj(t.entityelement[t.tentitytoselect].obj);
							}
						}
						ScaleObject (  t.editor.objectstartindex+5,t.tscale_f,t.tscale_f,t.tscale_f );
						t.tshowasstatic=1+t.entityelement[t.tentitytoselect].staticflag;
					}
					else
					{
						// 201015 - if not highlighting an entity, click to start dragging a rubber band box
						if (bActivateRubberBand && t.widget.activeObject == 0 && t.inputsys.xmouse != 500000 )
						{
							if ( t.inputsys.mclick == 1 )
							{
								// start rubber band box
								if ( t.inputsys.rubberbandmode == 0 )
								{
									t.inputsys.rubberbandmode = 1;
									t.inputsys.spacekeynotreleased = 1;
									//PE: Make mouse relative to window pos.
									t.inputsys.rubberbandx = ImGuiGetMouseX();
									t.inputsys.rubberbandy = ImGuiGetMouseY();
								}
							}
						}
					}

					if (pref.iDragCameraMovement && t.ebe.on == 0 && t.inputsys.keyshift == 0 && t.inputsys.rubberbandmode == 1)
					{
						//PE: Shift released , end rubberband on mouse release.
						if (t.inputsys.mclick == 0)
						{
							float fCurrentRubberBandX1 = t.inputsys.rubberbandx;
							float fCurrentRubberBandX2 = ImGuiGetMouseX();
							float fCurrentRubberBandY1 = t.inputsys.rubberbandy;
							float fCurrentRubberBandY2 = ImGuiGetMouseY();
							if (fCurrentRubberBandX1 > fCurrentRubberBandX2) { float fStore = fCurrentRubberBandX1; fCurrentRubberBandX1 = fCurrentRubberBandX2; fCurrentRubberBandX2 = fStore; }
							if (fCurrentRubberBandY1 > fCurrentRubberBandY2) { float fStore = fCurrentRubberBandY1; fCurrentRubberBandY1 = fCurrentRubberBandY2; fCurrentRubberBandY2 = fStore; }

							// finish rubber banding
							t.inputsys.rubberbandmode = 0;
							bRubberBandCreated = true;
							fLastRubberBandX1 = fCurrentRubberBandX1;
							fLastRubberBandX2 = fCurrentRubberBandX2;
							fLastRubberBandY1 = fCurrentRubberBandY1;
							fLastRubberBandY2 = fCurrentRubberBandY2;
							// auto choose an entity to act as the widget achor object
							if (g.entityrubberbandlist.size() > 0)
							{
								if (g.entityrubberbandlist.size() == 1)
								{
									// if only range selected on, make it a regular entity selection
									t.widget.pickedEntityIndex = g.entityrubberbandlist[0].e;
									gridedit_clearentityrubberbandlist();
								}
								else
								{
									t.widget.pickedEntityIndex = g.entityrubberbandlist[0].e;
								}
								t.widget.pickedObject = t.entityelement[t.widget.pickedEntityIndex].obj;
								t.widget.offsetx = 0;
								t.widget.offsety = 0;
								t.widget.offsetz = 0;

								if (pref.iEnableDragDropEntityMode)
								{
									fHitOffsetZ = 0.0001; //So we trigger a widget.
								}

								i_switch_group_tab = 1; //Display "current objects" tab.
							}
							t.inputsys.rubberbandmode = 0;
						}
					}

					// 201015 - rubber band effect and control
					if (bActivateRubberBand && t.inputsys.rubberbandmode == 1 )
					{
						//bWaitOnMouseRelease
						bool bCancelRubberBand = false;
						if (pref.iEnableDragDropEntityMode) {
							if (bWaitOnMouseRelease)
								bCancelRubberBand = true;
						}
						if ( t.inputsys.xmouse == 500000 || bCancelRubberBand )
						{
							// mouse left area, cancel rubber band
							t.inputsys.rubberbandmode = 0;
						}
						else
						{
							float fMX = 1.0f;
							float fMY = 1.0f;
						
							// reverse bound box if inside out
							float fCurrentRubberBandX1 = t.inputsys.rubberbandx;
							float fCurrentRubberBandX2 = ImGuiGetMouseX();
							float fCurrentRubberBandY1 = t.inputsys.rubberbandy;
							float fCurrentRubberBandY2 = ImGuiGetMouseY();
							//PE: Y a bit off
							fCurrentRubberBandY1 -= 20.0f;
							if ( fCurrentRubberBandX1 > fCurrentRubberBandX2 ) { float fStore = fCurrentRubberBandX1; fCurrentRubberBandX1 = fCurrentRubberBandX2; fCurrentRubberBandX2 = fStore; }
							if ( fCurrentRubberBandY1 > fCurrentRubberBandY2 ) { float fStore = fCurrentRubberBandY1; fCurrentRubberBandY1 = fCurrentRubberBandY2; fCurrentRubberBandY2 = fStore; }

							// detect all entities within box and highlight
							for ( int e = 1; e <= g.entityelementlist; e++ )
							{
								// 060116 - if locked and holding space, unlock it now
								if (t.inputsys.keyspace != 0) 
								{
									t.entityelement[e].editorlock = false;
									sObject* pObject;
									if (t.entityelement[e].obj > 0) 
									{
										pObject = g_ObjectList[t.entityelement[e].obj];
										if (pObject) 
										{
											if (t.entityelement[e].editorlock) 
											{
												#ifndef ALLOWSELECTINGLOCKEDOBJECTS
												WickedCall_SetObjectRenderLayer(pObject, GGRENDERLAYERS_CURSOROBJECT);
												#endif
											}
											else
												WickedCall_SetObjectRenderLayer(pObject, GGRENDERLAYERS_NORMAL);
										}
									}
								}

								// only if not locked
								if ( t.entityelement[e].editorlock==false )
								{
									int tobj = t.entityelement[e].obj;
									if ( tobj > 0 )
									{
										ImVec2 v2DPos = Convert3DTo2D(ObjectPositionX(tobj), ObjectPositionY(tobj), ObjectPositionZ(tobj));
										int iEntityScreenX = v2DPos.x;
										int iEntityScreenY = v2DPos.y;
										if ( iEntityScreenX > fCurrentRubberBandX1*fMX && iEntityScreenX < fCurrentRubberBandX2*fMX )
										{
											if ( iEntityScreenY > fCurrentRubberBandY1*fMY && iEntityScreenY < fCurrentRubberBandY2*fMY )
											{
												// only if not already highlighted
												gridedit_addEntityToRubberBandHighlights ( e );
											}
										}
									}
								}
							}

							// now de-highlight any in the list NOT covered by the boundbox
							if ( g.entityrubberbandlist.size() > 0 )
							{
								int i = 0;
								while ( i < (int)g.entityrubberbandlist.size() )
								{
									bool bThisOneInBox = false;
									int e = g.entityrubberbandlist[i].e;
									int tobj = t.entityelement[e].obj;
									if ( tobj > 0 )
									{
										//PE: This one work better. The old have problems selecting objects at the bottom of screen.
										ImVec2 v2DPos = Convert3DTo2D(ObjectPositionX(tobj), ObjectPositionY(tobj), ObjectPositionZ(tobj));
										int iEntityScreenX = v2DPos.x;
										int iEntityScreenY = v2DPos.y;
										if ( iEntityScreenX > fCurrentRubberBandX1*fMX && iEntityScreenX < fCurrentRubberBandX2*fMX )
										{
											if ( iEntityScreenY > fCurrentRubberBandY1*fMY && iEntityScreenY < fCurrentRubberBandY2*fMY )
											{
												bThisOneInBox = true;
											}
										}
									}
									if ( bThisOneInBox == false )
									{
										SetAlphaMappingOn ( tobj, 100 );
										//PE: Restore original colors.
										sObject* pObject = g_ObjectList[tobj];
										if (pObject)
										{
											WickedSetEntityId(t.entityelement[e].bankindex);
											WickedSetElementId(e);
											for (int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++)
											{
												WickedSetMeshNumber(iMesh);
												WickedCall_SetMeshMaterial(pObject->ppMeshList[iMesh], false);
											}
											WickedSetEntityId(-1);
											WickedSetElementId(0);
										}
										g.entityrubberbandlist.erase(g.entityrubberbandlist.begin() + i);
										i = 0;
									}
									else
									{
										i++;
									}
								}
							}

							// when release mouse while rubber banding
							WickedCall_SetSpriteBoundBox(false,0,0,0,0);
							if ( t.inputsys.mclick == 0 )
							{
								// finish rubber banding
								t.inputsys.rubberbandmode = 0;
								bRubberBandCreated = true;
								fLastRubberBandX1 = fCurrentRubberBandX1;
								fLastRubberBandX2 = fCurrentRubberBandX2;
								fLastRubberBandY1 = fCurrentRubberBandY1;
								fLastRubberBandY2 = fCurrentRubberBandY2;
								// auto choose an entity to act as the widget achor object
								if (g.entityrubberbandlist.size() > 0)
								{
									if (g.entityrubberbandlist.size() == 1)
									{
										// if only range selected on, make it a regular entity selection
										t.widget.pickedEntityIndex = g.entityrubberbandlist[0].e;
										gridedit_clearentityrubberbandlist();
									}
									else
									{
										t.widget.pickedEntityIndex = g.entityrubberbandlist[0].e;
									}
									t.widget.pickedObject = t.entityelement[t.widget.pickedEntityIndex].obj;
									t.widget.offsetx = 0;
									t.widget.offsety = 0;
									t.widget.offsetz = 0;

									if (pref.iEnableDragDropEntityMode)
									{
										fHitOffsetZ = 0.0001; //So we trigger a widget.
									}

									i_switch_group_tab = 1; //Display "current objects" tab.
								}
							}
							else
							{
								//PE: Not relative to win rect, only when selecting the actual objects, 2D is different.
								RECT rect = { NULL };
								GetWindowRect(g_pGlob->hWnd, &rect);
								float fX1 = (t.inputsys.rubberbandx + rect.left)*fMX;
								float fX2 = t.inputsys.xmouse*fMX;
								float fY1 = (t.inputsys.rubberbandy + rect.top)*fMY;
								float fY2 = t.inputsys.ymouse*fMY;
								//PE: A bit off.
								fX1 += 6.0;
								fX2 += 6.0;
								fY1 += 6.0;
								fY2 += 6.0;
								if ( fX2 < fX1 )
								{
									float fSt = fX1;
									fX1 = fX2;
									fX2 = fSt;
								}
								if ( fY2 < fY1 )
								{
									float fSt = fY1;
									fY1 = fY2;
									fY2 = fSt;
								}
								if(!g_bCharacterCreatorPlusActivated)
									WickedCall_SetSpriteBoundBox(true, fX1, fY1, fX2, fY2);
							}
						}
					}
				}
				else
				{
					//  entity attached to cursor
					PositionObject (  t.editor.objectstartindex+5,t.gridentityposx_f,t.gridentityposy_f,t.gridentityposz_f );
					if (  ObjectExist(g.entitybankoffset+t.gridentity) == 1 ) 
					{
						if (  t.entityprofile[t.gridentity].ismarker == 3 || t.entityprofile[t.gridentity].ismarker == 6 || t.entityprofile[t.gridentity].ismarker == 8 ) 
						{
							//  trigger zone or checkpoint
							t.tscale_f=100;
						}
						else
						{
							if (  t.entityprofile[t.gridentity].islightmarker == 1 || t.gridedit.entityspraymode == 1) 
							{
								if ( t.gridedit.entityspraymode == 1 )
								{
									t.tscale_f=(100/3.0)*2*(t.gridedit.entitysprayrange/50.0);
								}
								else
								{
									t.tscale_f=(100/3.0)*2*(t.grideleprof.light.range/50.0);
								}
							}
							else
							{
								t.tscale_f = get_cursor_scale_for_obj(g.entitybankoffset+t.gridentity);
							}
						}
						ScaleObject (  t.editor.objectstartindex+5,t.tscale_f,t.tscale_f,t.tscale_f );
					}
					//if (  t.entityprofile[t.gridentity].ischaracter == 1 && t.entityprofile[t.gridentity].isthirdperson == 1 ) 
					if (  t.entityprofile[t.gridentity].ischaracter == 1 ) // 220217 - now for all characters
					{
						// third person char+marker detection (will not work in VR edit mode)
						if ( t.playercontrol.thirdperson.enabled == 0 && g.vrqcontrolmode == 0 ) 
						{
							t.tattachtothis=findentitycursorobj(-1);
							if (  t.tattachtothis>0 ) 
							{
								if (  t.entityprofile[t.entityelement[t.tattachtothis].bankindex].ismarker == 1 ) 
								{
									t.tobj=t.entityelement[t.tattachtothis].obj;
									if (  t.tobj>0 ) 
									{
										if (  ObjectExist(t.tobj) == 1 ) 
										{
											t.tmousemodifierx_f=(GetDisplayWidth()+0.0)/(GetChildWindowWidth()+0.0);
											t.tmousemodifiery_f=(GetDisplayHeight()+0.0)/(GetChildWindowHeight()+0.0);
											pastebitmapfontcenter("ATTACH FOR THIRD PERSON CONTROL",GetScreenX(t.tobj)*t.tmousemodifierx_f,(GetScreenY(t.tobj)*t.tmousemodifiery_f)+50,2,255);
											t.inputsys.willmakethirdperson=t.tattachtothis;
										}
									}
								}
							}
						}
					}
					t.tshowasstatic=1+t.gridentitystaticmode;
					t.showentityid=t.gridentity;

					// no linking in MAX
					t.tentityoverdraggingcursor = 0;
				}

				if (  t.tshowasstatic>0 && t.inputsys.activemouse == 1 ) 
				{
					t.tentityworkobjectchoice=0;
					if (  t.entityprofile[t.showentityid].ismarker == 1  )  t.tforcedynamic = 1;
					if (  t.entityprofile[t.showentityid].ismarker == 4  )  t.tforcedynamic = 1;
					if (  t.entityprofile[t.showentityid].ismarker == 10  )  t.tforcedynamic = 1;
					if (  t.entityprofile[t.showentityid].ismarker == 3 || t.entityprofile[t.showentityid].ismarker == 6 || t.entityprofile[t.showentityid].ismarker == 8 ) 
					{
						//  trigger area or checkpoint
						HideObject (  t.editor.objectstartindex+5 );
						t.tforcedynamic=1;
					}
					else
					{
						// No flat range decal in wicked!
						HideObject (  t.editor.objectstartindex+5 );
					}
					//  show legend of entity hovering over (and static legend)
					t.taddstaticlegend=0;
					if (  t.tentitytoselect>0 ) 
					{
						t.tstatic=t.entityelement[t.tentitytoselect].staticflag;
						t.tentid=t.entityelement[t.tentitytoselect].bankindex;
					}
					else
					{
						t.tstatic=t.gridentitystaticmode;
						t.tentid=t.gridentity;
					}
					if (  t.tstatic == 1 ) 
					{
						t.taddstaticlegend=1;
					}
					else
					{
						t.taddstaticlegend=0;
					}
					t.editor.entityworkobjectchoice=t.tentityworkobjectchoice;
					t.editor.entitytoselect=t.tentitytoselect;
					if ( t.tentitytoselect>0 ) 
					{
						t.tobj=t.entityelement[t.tentitytoselect].obj;
						t.tname_s = "" ; t.tname_s=t.tname_s + "["+Str(t.tentitytoselect)+" {"+Str(t.tobj)+"}] "+t.entityelement[t.tentitytoselect].eleprof.name_s;
						t.ttentid=t.entityelement[t.tentitytoselect].bankindex;
						gridedit_showtobjlegend ( );
					}
					else
					{
						t.tobj=t.gridentityobj ; t.tname_s=t.grideleprof.name_s;
						t.ttentid=t.gridentity;
						gridedit_showtobjlegend ( );
					}
				}
				else
				{
					HideObject (  t.editor.objectstartindex+5 );
				}

				editor_refreshentitycursor ( );

				//  prompt when over locked entity
				if (  g.gentityundercursorlocked>0 ) 
				{
					t.relaytostatusbar_s="LOCKED - Hold SPACEBAR and click to unlock";
				}
				else
				{
					if (  t.tentitytoselect == 0 && t.gridentity == 0 ) 
					{
						t.relaytostatusbar_s="None Selected";
					}
				}

				// Entity Edit Mode
				if ( 1 ) 
				{
					// only place plane on each new placement
					static int iInitialPlacementOfPlane = 0;
					if (iInitialPlacementOfPlane == 0 && t.gridentity != 0) { iInitialPlacementOfPlane = 1; g_bResetPlaneAfterXZAdjust = false; }
					if (iInitialPlacementOfPlane == 2 && t.gridentity == 0) iInitialPlacementOfPlane = 0;

					bool bPlaceEntity = false;
					if ((t.widget.duplicatebuttonselected == 2 && t.gridentity == 0))
						bPlaceEntity = true;

					if (!bPlaceEntity)
					{
						//PE: We should only do this if we have a drag/drop process active.
						if (pref.iEnableDragDropEntityMode && bDraggingActive) 
						{
							//PE: In this mode we need a mouse release to drop
							if (ImGui::IsMouseClicked(0))
								bReadyToDropEntity = true;
							if (t.inputsys.mclick == 1)
								bReadyToDropEntity = true;
							if (t.gridentity != 0 && bReadyToDropEntity && t.inputsys.mclick == 0 && !ImGui::IsMouseDown(0))
							{
								bPlaceEntity = true;
								bReadyToDropEntity = false;
								bDraggingActive = true;
								if (iObjectMoveMode == 2 && (iObjectMoveModeDropSystemUsing == 1 && g_bHoldGridEntityPosWhenManaged == false))
								{
									// trigger force find surface events
									iObjectMoveModeDropSystem = -3;
								}
								iObjectMoveModeDropSystemUsing = 0;
							}
						}
						else
						{
							if ((t.inputsys.mclick == 1 && t.gridentity != 0))
								bPlaceEntity = true;
						}
					}

					// so can get the plane position info later
					bool bPlanePosRegistered = false;
					float fPlanePosX, fPlanePosY, fPlanePosZ;
					fPlanePosX = 0;
					fPlanePosY = -999999.9f;
					fPlanePosZ = 0;

					// special horiz mode to match terrain height when about to go under it
					static bool bDepartedFromChosenY = false;
					static float fDepartedFromThisY = 0.0f;
					if (t.inputsys.mclick != 1) bDepartedFromChosenY = false;
					int newpicksystem = -99;

					//LB: widget control handled elsewhere, so no smart placement if widget active!
					bool bWidgetHasControlHere = false;
					if (t.widget.pickedSection > 0 && t.widget.pickedSection != -98 && t.widget.pickedSection != -99) bWidgetHasControlHere = true;
					if (bTriggerVisibleWidget == true) bWidgetHasControlHere = true;

					//PE: Allow object to go 80% below terrain.
					int GetActiveEditorObject(void);
					int iActiveObj = GetActiveEditorObject();

					//Dont change anyhthing when we are ready to place entity.
					if (!bPlaceEntity && bWidgetHasControlHere == false )
					{
						//PE: Prevent user for placing objects outside playable area.
						bool bObjectOutSideEditArea = false;
						float fEditableSizeHalved = GGTerrain_GetEditableSize();
						t.terraineditableareasizeminx = -fEditableSizeHalved;
						t.terraineditableareasizeminz = -fEditableSizeHalved;
						t.terraineditableareasizemaxx = fEditableSizeHalved;
						t.terraineditableareasizemaxz = fEditableSizeHalved;
						if (t.gridentityposx_f < t.terraineditableareasizeminx) { t.gridentityposx_f = t.terraineditableareasizeminx; bObjectOutSideEditArea = true; }
						if (t.gridentityposx_f > t.terraineditableareasizemaxx) { t.gridentityposx_f = t.terraineditableareasizemaxx; bObjectOutSideEditArea = true; }
						if (t.gridentityposz_f < t.terraineditableareasizeminz) { t.gridentityposz_f = t.terraineditableareasizeminz; bObjectOutSideEditArea = true; }
						if (t.gridentityposz_f > t.terraineditableareasizemaxz) { t.gridentityposz_f = t.terraineditableareasizemaxz; bObjectOutSideEditArea = true; }
						float fOldgridentityposx_f = t.gridentityposx_f;
						float fOldgridentityposy_f = t.gridentityposy_f;
						float fOldgridentityposz_f = t.gridentityposz_f;

						//  entity placement update
						if (t.inputsys.mclick != 2)
						{
							static float fActivePosX = t.gridentityposx_f;
							static float fActivePosZ = t.gridentityposz_f;
							static float fActivePosY = fHitPointY;

							bMouseInputSystemUsed = false;
							if (pref.iEnableDragDropEntityMode)
							{
								int picksystem = t.widget.pickedSection;
								{
									if (bDraggingActive == false) bDraggingActiveInitial = false;
									if (ImGui::GetIO().MouseReleased[1])
									{
										// Wait for mouse position to update before performing pick
										// When we hold RMB to look around with an entity on the cursor, the mouse position is reset to the centre of the screen temporarily
										// When the RMB is released, the initial mouse pick will always be from the center of the screen, rather than where the mouse was before it was hidden
										// This causes the entity to flicker. Skipping this function after a RMB release will prevent the flicker.
										return;
									}
									if (t.inputsys.localselectedrayhit == true && (bDraggingActive == false || bDraggingActiveInitial == true || iObjectMoveMode == 2))
									{
										fHitPointY = t.inputsys.localcurrentterrainheight_f;

										// and move plane to discovered surface position (for specific modes)
										if (iInitialPlacementOfPlane == 2)
										{
											if (iObjectMoveMode == 2)
											{
												if (g.entityrubberbandlist.size() > 1)
												{
													// groups need to find floor fast!
													fActivePosY = fHitPointY;
												}
												else
												{
													if (g_bAdjustPlaneXZUsingSurfaceXZ == true)
													{
														float fUpDownAngle = WrapValue(CameraAngleX(0));
														int iForwardFacing = t.entityprofile[t.gridentity].forwardfacing;
														if (t.gridentitygridlock == 0 || iForwardFacing != 0)
														{
															if (iForwardFacing == 0 && fUpDownAngle > 10.0f && fUpDownAngle < 91.0f)
															{
																if (iObjectMoveMode == 2 && iObjectMoveModeDropSystemUsing == 1)
																{
																	// smart mode keeps the initial XZ plane height for predictable positioning of object
																	// then uses the iObjectMoveModeDropSystem to plop the object on the ground
																}
																else
																{
																	// normally go to surface
																	fActivePosY = fHitPointY;
																}
															}
															if (iForwardFacing == 1)
															{
																fActivePosX = t.inputsys.localx_f;
																fActivePosZ = t.inputsys.localy_f;
																fActivePosY = fHitPointY;
															}
															if (iForwardFacing == 2)
															{
																fActivePosY = fHitPointY;
															}
														}
														g_bAdjustPlaneXZUsingSurfaceXZ = false;
													}
												}

												// small shifts cause glitch grabs, so round them off
												fActivePosX = floor(fActivePosX);
												fActivePosY = floor(fActivePosY);
												fActivePosZ = floor(fActivePosZ);
											}
										}
									}
								}

								// LB: moved from below, needed this mode whether in grid mode or not
								if (iObjectMoveMode == 1)
								{
									newpicksystem = -98;
								}

								// LB: mousepick functionality disabled for now, see how new smart find ground system works out...
								if (!(fHitOffsetX == 0 && fHitOffsetY == 0 && fHitOffsetZ == 0))
								{
									int iRealObjectMoveMode = iObjectMoveMode;
									if (iObjectMoveModeDropSystem > 0) iRealObjectMoveMode = 0;
								}

								// only move plane on initial object selection, this avoids drift when placing objects
								bool bIgnoreFirstPlaneDetectItIsWrong = false;
								if (iInitialPlacementOfPlane == 1)
								{
									bool bJustForInitialDragIn = false;
									if (bDraggingActive == false && fHitOffsetX == 0 && fHitOffsetY == 0 && fHitOffsetZ == 0) bJustForInitialDragIn = true;
									if (bDraggingActive == true && t.gridentityposx_f == 0 && t.gridentityposz_f == 0) bJustForInitialDragIn = true;
									if (bDraggingActiveInitial == true)	bJustForInitialDragIn = true;
									if (bJustForInitialDragIn == true && t.inputsys.localselectedrayhit == false)
									{
										// new selection from left panel
										g_bResetPlaneAfterXZAdjust = false;
										float fDistCap = 1000.0f;
										MoveCamera(0, fDistCap);
										fActivePosX = CameraPositionX(0);
										fActivePosY = CameraPositionY(0);
										fActivePosZ = CameraPositionZ(0);
										MoveCamera(0, -fDistCap);
									}
									else
									{
										// selected object from level
										fActivePosX = t.gridentityposx_f;
										fActivePosZ = t.gridentityposz_f;

										// only put plane on surface if object connected with a surface
										if (iObjectMoveMode == 2)
										{
											bool bMustFaceDown = false;
											int iForwardFacing = t.entityprofile[t.gridentity].forwardfacing;
											float fUpDownAngle = WrapValue(CameraAngleX(0));
											if (fUpDownAngle > -30.0f && fUpDownAngle < 91.0f) //LB: was 10.0f - Rick wanted to find floor when looking camera directly forward
											{
												if (iObjectMoveModeDropSystemUsing == 1)
												{
													// drop system keeps object on initial plane
													fActivePosY = t.gridentityposy_f + fHitOffsetY;
												}
												else
												{
													if (t.gridentitygridlock != 0 && iForwardFacing == 0 && bJustForInitialDragIn == false)
													{
														// if in grid/snap mode, do not use surface Y
														fActivePosY = t.gridentityposy_f + fHitOffsetY;
													}
													else
													{
														// find nearby surface
														fActivePosY = fHitPointY;
													}
												}
											}
											else
											{
												// side/up
												fActivePosY = t.gridentityposy_f + fHitOffsetY;
											}
										}
										else
										{
											if (iObjectMoveMode == 1)
											{
												// LB: directly on site of initial click
												fActivePosX = t.gridentityposx_f + fHitOffsetX;
												fActivePosY = t.gridentityposy_f + fHitOffsetY;
												fActivePosZ = t.gridentityposz_f + fHitOffsetZ;
											}
											else
											{
												fActivePosY = t.gridentityposy_f + fHitOffsetY;
											}
										}
									}

									// small shifts cause glitch grabs, so round them off
									fActivePosX = floor(fActivePosX);
									fActivePosY = floor(fActivePosY);
									fActivePosZ = floor(fActivePosZ);
									iInitialPlacementOfPlane = 2;
									bIgnoreFirstPlaneDetectItIsWrong = true; // because it only seems to work after wicked has synced all of a sudden, ah well.
								}

								// handed to smart system for plane position and orientation
								t.widget.pickedSection = newpicksystem;

								// find where mouse is on the plane we have positioned
								bool widget_getplanepos(float fActivePosX, float fActivePosY, float fActivePosZ, float* pPlanePosX, float* pPlanePosY, float* pPlanePosZ);
								bPlanePosRegistered = widget_getplanepos(fActivePosX, fActivePosY, fActivePosZ, &fPlanePosX, &fPlanePosY, &fPlanePosZ);
								if (bPlanePosRegistered == true && bIgnoreFirstPlaneDetectItIsWrong == false)
								{
									fPlanePosX -= fHitOffsetX;
									fPlanePosY -= fHitOffsetY;
									fPlanePosZ -= fHitOffsetZ;
									if (newpicksystem == -98)
									{
										// adjusting only Y
										t.gridentityposy_f = fPlanePosY;

										//leelee, maybe some grid/snap here so vertical make sense?
										//LB: sure thing Lee, here you go, see below
										if (t.gridentitygridlock == 2 && iObjectMoveMode == 1)
										{
											if (pref.fEditorGridSizeY > 0)
											{
												// snap to grid - dding grid Y mode in 2025!
												t.gridentityposy_f = fPlanePosY;
												float fGripY = t.gridentityposy_f + (pref.fEditorGridSizeY / 2);
												fGripY -= pref.fEditorGridOffsetY;
												if (fGripY < 0)
													fGripY = ((int(fGripY / pref.fEditorGridSizeY) - 1) * pref.fEditorGridSizeY);
												else
													fGripY = (int(fGripY / pref.fEditorGridSizeY) * pref.fEditorGridSizeY);

												// only if above or on terrain
												fGripY += pref.fEditorGridOffsetY;
												float fTerrainAtThisPoint = BT_GetGroundHeight (0, t.gridentityposx_f, t.gridentityposz_f);

												//PE: Allow object to go 80% below terrain.
												if (iActiveObj > 0)
												{
													//PE: Object can go under terrain by 80%.
													float fAllowBelowTerrainMax = (ObjectSizeY(iActiveObj, 1) * 0.80f);
													fTerrainAtThisPoint -= fAllowBelowTerrainMax;
												}
												if (fGripY < fTerrainAtThisPoint)
												{
													fGripY = fTerrainAtThisPoint;
												}
												t.gridentityposy_f = fGripY;
											}
										}
									}
									else
									{
										// ensure verticle only means just that!
										if (t.gridentitygridlock == 2 && iObjectMoveMode == 1)
										{
											// snap to grid - dding grid Y mode in 2025!
										}
										else
										{
											// adjusting X and Z
											t.gridentityposx_f = fPlanePosX;
											t.gridentityposz_f = fPlanePosZ;
										}
									}
								}

								//DEBUG: if (ObjectExist(t.widget.widgetPlaneObj)) ShowObject(t.widget.widgetPlaneObj);
								t.widget.pickedSection = picksystem;
							}
							else
							{
								t.gridentityposx_f = t.inputsys.localx_f + t.inputsys.dragoffsetx_f;
								t.gridentityposz_f = t.inputsys.localy_f + t.inputsys.dragoffsety_f;
							}
						}

						bool bUseOldYSystem = true;

						bool bDontUsePivot = false;
						if (iExtractMode == 1) 
						{ 
							//0 = find floor, 1 = extracted y value. , 3 = fixed y value.
							t.gridentityposy_f = fExtractYValue;
							bUseOldYSystem = false;
							bDontUsePivot = true;
						}
						if (iExtractMode == 2) 
						{
							t.gridentityposy_f = fExtractFixedYValue;
							bUseOldYSystem = false;
						}


						if (t.tforcedynamic == 1)
						{
							t.gridentitystaticmode = 0;
						}

						//PE: Display red/green box of cursor object. to display static/dynamic.
						if (!Shooter_Tools_Window_Active)
						{
							//PE: Allow t.widget.pickedObject to be selected in this mode.
							if (!pref.iEnableDragDropEntityMode)
							{
								g_selected_editor_object = NULL;
								g_selected_editor_objectID = 0;
							}
							if (t.gridentityobj > 0) 
							{
								if (t.gridentityobj < g_iObjectListCount)
								{
									if (g_ObjectList[t.gridentityobj])
									{
										if (t.gridentitystaticmode)
											g_selected_editor_color = XMSTATICCOLOR;
										else
											g_selected_editor_color = XMDYNAMICCOLOR;
										g_selected_editor_object = g_ObjectList[t.gridentityobj];
										g_selected_editor_objectID = t.gridentityobj;
									}
								}
							}
						}

						// Find ground while placing entity on terrain
						bool bApplyEntityOffsets = false;
						bool bCanUpdateY = true;
						int iForwardFacing = 0;
						if (t.gridentity > 0) iForwardFacing = t.entityprofile[t.gridentity].forwardfacing;
						if (t.gridentitysurfacesnap == 1 || t.inputsys.picksystemused == 2)
						//if (t.gridentity > 0 && (t.gridentitysurfacesnap == 1 || t.inputsys.picksystemused == 2)) // code only needed when picked cursor object - bit not using this for now as currently stable and tested
						{
							if (pref.iEnableDragDropEntityMode)
							{
								//PE: Only newly created obects can also adjust Y
								//PE: If we grab one from the level, it should use the settings. Y or XZ.
								if (!(fHitOffsetX == 0 && fHitOffsetY == 0 && fHitOffsetZ == 0))
								{
									// object being dragged about and already in the level
									if (pref.iEnableDragDropWidgetSelect)
									{
										// but only if not dragging in objects that need fHitOffsetXYZ offset at very start
										bool bCanActivateWidgetNow = true;
										if (t.entityprofile[t.gridentity].ismarker == 2) bCanActivateWidgetNow = false;
										if (iForwardFacing == 2 && t.entityprofile[t.gridentity].ismarker == 0) bCanActivateWidgetNow = false;
										if (bCanActivateWidgetNow == true)
										{
											bTriggerVisibleWidget = true;
										}
									}
									else
										bTriggerVisibleWidget = false;

									int iRealObjectMoveMode = iObjectMoveMode;
									if (iObjectMoveModeDropSystem > 0) iRealObjectMoveMode = 0;
									if (iRealObjectMoveMode == 2)
									{
										bCanUpdateY = true;
									}
									else
									{
										// cannot update Y if horiz or vert positioning modes
										bCanUpdateY = false;
									}
								}
								else
								{
									// dragged direct from 'Level objects'
									if (bWidgetHasControlHere == false)
									{
										//LB: keep widget visible when dragging it about
										bTriggerVisibleWidget = false;
									}
								}

								//LB: requested ease of use feature, if in horiz mode, and object falls below terrain, raise it up
								if (iObjectMoveMode == 0)
								{
									// horiz position XZ mode
									if (bDepartedFromChosenY == true) t.gridentityposy_f = fDepartedFromThisY;
									float fActualHeightUnderObject = BT_GetGroundHeight(0, t.gridentityposx_f, t.gridentityposz_f);
									//PE: Allow object to go 80% below terrain.
									if (iActiveObj > 0 )
									{
										// and ensure we CAN place objects that are submerged, just make sure they do not go entirely under
										// the floor, so keep 20% of them anove the ground height
										float fAllowBelowTerrainMax = ((float)ObjectSizeY(iActiveObj, 1) * 0.8f);
										fActualHeightUnderObject -= fAllowBelowTerrainMax;
									}
									if (t.gridentityposy_f < fActualHeightUnderObject )
									{
										if (bDepartedFromChosenY == false)
										{
											bDepartedFromChosenY = true;
											fDepartedFromThisY = t.gridentityposy_f;
										}
										t.gridentityposy_f = fActualHeightUnderObject;
										bApplyEntityOffsets = true;
									}
								}
							}

							// when drag in object initially, use new smart placement system
							bool bJustForInitialDragIn = false;
							if (bDraggingActive == false && fHitOffsetX == 0 && fHitOffsetY == 0 && fHitOffsetZ == 0) bJustForInitialDragIn = true;
							if (bDraggingActive == true && t.gridentityposx_f == 0 && t.gridentityposz_f == 0) bJustForInitialDragIn = true;
							if (bDraggingActiveInitial == true)	bJustForInitialDragIn = true;

							// new system to locate grid ent pos at point where mouse touches terrain/entity surface
							// LB: Note, "t.inputsys.localcurrentterrainheight_f" will not account for 'entity surface' if iObjectMoveMode != 2!!
							if ((bUseOldYSystem && bCanUpdateY) || bJustForInitialDragIn == true)
							{
								if ((iObjectMoveMode == 2 && (t.gridentitygridlock == 0 || iForwardFacing != 0)) || bJustForInitialDragIn == true)
								{
									if (bJustForInitialDragIn == true)
									{
										if (t.inputsys.localselectedrayhit == true)
										{
											t.gridentityposx_f = t.inputsys.localx_f - fHitOffsetX;
											t.gridentityposz_f = t.inputsys.localy_f - fHitOffsetZ;
											t.gridentityposy_f = (t.inputsys.localcurrentterrainheight_f - fHitOffsetY) + g_fSpecialDragInYAdjustment;
										}
										else
										{
											if (bPlanePosRegistered == true)
											{
												t.gridentityposy_f = fPlanePosY;
											}
										}
									}
									else
									{
										if (bPlanePosRegistered == true)
										{
											if (iForwardFacing == 2)
											{
												float fUpDownAngle = WrapValue(CameraAngleX(0));
												if ((fUpDownAngle > 10.0f && fUpDownAngle < 350.0f) || (t.gridentity>0 && t.entityprofile[t.gridentity].ismarker == 2))
												{
													t.gridentityposx_f = t.inputsys.localx_f - fHitOffsetX;
													t.gridentityposz_f = t.inputsys.localy_f - fHitOffsetZ;
													t.gridentityposy_f = t.inputsys.localcurrentterrainheight_f - fHitOffsetY;
												}
												else
												{
													t.gridentityposx_f = t.inputsys.localx_f;
													t.gridentityposz_f = t.inputsys.localy_f;
													t.gridentityposy_f = t.inputsys.localcurrentterrainheight_f;
												}
											}
											else
											{
												t.gridentityposy_f = fPlanePosY;
											}
										}
										float fUpDownAngle = WrapValue(CameraAngleX(0));
										if (fUpDownAngle > 10.0f && fUpDownAngle < 350.0f)
										{
											// horizontal plane handles this - stops shake that happens when use ray Y instead of plane Y
										}
										else
										{
											// plane is vertical, so need to determine if ray hit closer than vertical plane
											if (t.inputsys.localselectedrayhit == true && iForwardFacing == 0)
											{
												float fDX = t.inputsys.localx_f - CameraPositionX(0);
												float fDY = t.inputsys.localcurrentterrainheight_f - CameraPositionY(0);
												float fDZ = t.inputsys.localy_f - CameraPositionZ(0);
												float fDistToPick = sqrt(fabs(fDX*fDX) + fabs(fDY*fDY) + fabs(fDZ*fDZ));
												fDX = fPlanePosX - CameraPositionX(0);
												fDY = fPlanePosY - CameraPositionY(0);
												fDZ = fPlanePosZ - CameraPositionZ(0);
												float fDistToPlane = sqrt(fabs(fDX*fDX) + fabs(fDY*fDY) + fabs(fDZ*fDZ));
												if (fDistToPlane > fDistToPick)
												{
													t.gridentityposx_f = t.inputsys.localx_f - fHitOffsetX;
													t.gridentityposz_f = t.inputsys.localy_f - fHitOffsetZ;
													t.gridentityposy_f = t.inputsys.localcurrentterrainheight_f - fHitOffsetY;
												}
											}
										}
									}
								}
								else
								{
									if (t.gridentitygridlock == 0 || iForwardFacing != 0)
									{
										t.gridentityposy_f = t.inputsys.localcurrentterrainheight_f;
									}
								}
								bApplyEntityOffsets = true;
							}

							if (fHitPointY == 0.0f)
							{
								fHitPointY = t.gridentityposy_f;
							}

							//LB: prevent any t.gridentitypos movement until user has moved the mouse pointer
							if (g_bHoldGridEntityPosWhenManaged == true)
							{
								t.gridentityposx_f = g_fHoldGridEntityPosX;
								t.gridentityposy_f = g_fHoldGridEntityPosY;
								t.gridentityposz_f = g_fHoldGridEntityPosZ;
							}
						}
						else
						{
							if (t.gridentityposoffground == 0)
							{
								if (bUseOldYSystem)
								{
									t.gridentityposy_f = BT_GetGroundHeight(t.terrain.TerrainID, t.gridentityposx_f, t.gridentityposz_f);
									bApplyEntityOffsets = true;
								}
							}
						}

						//PE: Prevent user for placing objects outside playable area.
						if (t.gridentityobj > 0 && t.gridentity > 0)
						{
							if (t.gridentityposx_f < t.terraineditableareasizeminx) { t.gridentityposx_f = fOldgridentityposx_f; t.gridentityposy_f = fOldgridentityposy_f; bObjectOutSideEditArea = true; }
							if (t.gridentityposx_f > t.terraineditableareasizemaxx) { t.gridentityposx_f = fOldgridentityposx_f; t.gridentityposy_f = fOldgridentityposy_f; bObjectOutSideEditArea = true; }
							if (t.gridentityposz_f < t.terraineditableareasizeminz) { t.gridentityposz_f = fOldgridentityposz_f; t.gridentityposy_f = fOldgridentityposy_f; bObjectOutSideEditArea = true; }
							if (t.gridentityposz_f > t.terraineditableareasizemaxz) { t.gridentityposz_f = fOldgridentityposz_f; t.gridentityposy_f = fOldgridentityposy_f; bObjectOutSideEditArea = true; }
							if (bObjectOutSideEditArea)
							{
								//Trigger warning.
								t.gridentityposy_f = BT_GetGroundHeight(0, t.gridentityposx_f, t.gridentityposz_f);
								sprintf(cSmallTriggerMessage, "Object is Outside Editable Area");
								iTriggerMessageFrames = 60;
								bTriggerSmallMessage = true;
							}
						}

						//LB: now apply grid/snap when user finished positioning XYZ
						if (iForwardFacing != 1)
						{
							// do not apply grid/snap for things like switches, they NEED the XZ from the surface to be exact
							Add_Grid_Snap_To_Position(false);
						}

						// handle any pivots that are object based
						if (!bDontUsePivot ) ApplyPivotToGridEntity();

						// Create and manage a ghost object for when selecting objects to move
						// so can be used by special smart positioning mode
						int iGhostObj = g.ghostcursorobjectoffset;
						if (iGhostObj > 0)
						{
							// remove if not needed
							if (ObjectExist(iGhostObj) == 1 && t.gridentityobj == 0) DeleteObject (iGhostObj);

							// create placement line of needed
							if (iObjectMoveMode == 0 || iObjectMoveMode == 1)
							{
								// create a thin line to indicate where the surface below the object is
								if (ObjectExist(iGhostObj) == 0 && t.gridentityobj > 0)
								{
									WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_CURSOROBJECT);
									MakeObjectBox(iGhostObj, 1, 1, 1);
									WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_NORMAL);
									sObject* pObject = GetObjectData(iGhostObj);
									WickedCall_TextureObject(pObject, NULL);
									if (pObject)
									{
										for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
										{
											sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
											if (pMesh)
											{
												pMesh->mMaterial.Diffuse.r = 1.0f;
												pMesh->mMaterial.Diffuse.g = 1.0f;
												pMesh->mMaterial.Diffuse.b = 1.0f;
												pMesh->mMaterial.Diffuse.a = 0.2f;
												WickedCall_SetMeshMaterial (pMesh, false);
												pMesh->bTransparency = true;
												WickedCall_SetMeshTransparent(pMesh);
											}
										}
										WickedCall_SetObjectCastShadows(pObject, false);
									}
								}
								if (ObjectExist(iGhostObj) == 1)
								{
									// position line so it fills the gap directly below the object
									sObject* pObject = GetObjectData(t.gridentityobj);
									float fRealMeshCenterY = pObject->collision.vecMin.y + ((pObject->collision.vecMax.y - pObject->collision.vecMin.y) / 2.0f);
									fRealMeshCenterY *= pObject->position.vecScale.y;
									float fY = ObjectPositionY(t.gridentityobj) + fRealMeshCenterY;
									float fPredictedY = editor_forceentityfindfloor(true);
									if (fPredictedY > fY) fPredictedY = 0.0f;
									float fX = ObjectPositionX(t.gridentityobj);
									float fZ = ObjectPositionZ(t.gridentityobj);
									float fDefaultHeight = t.entityprofile[t.gridentity].defaultheight;
									fY += fDefaultHeight;
									float fGapDistance = fY - fPredictedY;
									float fHalfWay = fPredictedY + (fGapDistance / 2.0f);
									PositionObject (iGhostObj, fX, fHalfWay-fDefaultHeight, fZ);
									ScaleObject(iGhostObj, 25, fGapDistance*100.0f, 25);
								}
							}

							//PE: When first dragged in we dont have a entityelement and cant check usespotlighting so...
							//PE: Dont make ghost on light objects.
							bool bIsLightObject = false;
							if (t.gridentity > 0 && t.entityprofile[t.gridentity].ismarker == 2)
							{
								bIsLightObject = true;
								//PE: Perhaps do this later, if needed stacking light is not really used :) , and only if we have a extracted entityid.
								//int spotlighting = t.entityelement[e].eleprof.usespotlighting;
								//entity_updatelightobjtype(obj, spotlighting);
							}

							// create ghost if needed
							bool bShowGhostIfSingleObject = false;
							if (iObjectMoveModeDropSystemUsing == 1 && g.entityrubberbandlist.size() <= 1 && g_bHoldGridEntityPosWhenManaged == false) bShowGhostIfSingleObject = true;
							if (iObjectMoveMode == 2 && bShowGhostIfSingleObject==true && !bIsLightObject && bDraggingActive)
							{
								if (ObjectExist(iGhostObj) == 0 && t.gridentityobj > 0)
								{
									WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_CURSOROBJECT);
									CloneObject (iGhostObj, t.gridentityobj);
									WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_NORMAL);
									sObject* pObject = GetObjectData(iGhostObj);
									WickedCall_TextureObject(pObject, NULL);
									if (pObject)
									{
										for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
										{
											sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
											if (pMesh)
											{
												pMesh->mMaterial.Diffuse.r = 1.0f;
												pMesh->mMaterial.Diffuse.g = 1.0f;
												pMesh->mMaterial.Diffuse.b = 1.0f;
												pMesh->mMaterial.Diffuse.a = 0.3f;
												WickedCall_SetMeshMaterial (pMesh, false);
												pMesh->bTransparency = true;
												WickedCall_SetMeshTransparent(pMesh);
											}
										}
										WickedCall_SetObjectCastShadows(pObject, false);
									}
								}
								if (ObjectExist(iGhostObj) == 1)
								{
									// position ghost version to the intended destination of a smart positioned object
									float fPredictedY = editor_forceentityfindfloor(true);
									float fX = ObjectPositionX(t.gridentityobj);
									float fY = ObjectPositionY(t.gridentityobj);
									float fZ = ObjectPositionZ(t.gridentityobj);
									PositionObject (iGhostObj, fX, fPredictedY, fZ);
									SetObjectToObjectOrientation(iGhostObj, t.gridentityobj);
									float fCurrentObjFrame = WickedCall_GetObjectFrame(GetObjectData(t.gridentityobj));
									SetObjectFrame(iGhostObj, fCurrentObjFrame);
								}
							}
						}

						//PE: Check if object is overlapping another object.
						//PE: MISSING - Lock position if object is not visible to camera (goes to the other side of another object).
						int iRealObjectMoveMode = iObjectMoveMode;
						if (iObjectMoveModeDropSystem > 0) iRealObjectMoveMode = 0;
						if (bObjectAllowOverlapping == 0 && (iRealObjectMoveMode == 0 || iRealObjectMoveMode == 2)) //iObjectMoveMode NO Y rays yet.
						{
							//#define CENTERSYSTEM
							#define NOOVERLABDEBUG
							sObject* pObject = g_ObjectList[t.gridentityobj];
							if (pObject)
							{
								PositionObject(t.obj, t.gridentityposx_f, t.gridentityposy_f, t.gridentityposz_f);
								RotateObject(t.obj, t.gridentityrotatex_f, t.gridentityrotatey_f, t.gridentityrotatez_f);
								
								//only set t.gridentityposx_f , t.gridentityposz_f
								ImVec4 ObjectCenter = GetRealCenterToGridEntity();
								ImVec4 ObjectSize;
								if (ObjectCenter.w) //Only if valid object.
								{
									XMFLOAT3 fObjCenter = { pObject->position.vecPosition.x + ObjectCenter.x,pObject->position.vecPosition.y + ObjectCenter.y,pObject->position.vecPosition.z + ObjectCenter.z };

									void WickedCall_DrawPoint(float fx, float fy, float fz, float size, XMFLOAT4 color, bool bThickLine);
									XMFLOAT4 color = { 1.0,1.0,0.0,1.0 }; //real center of object.
									#ifndef NOOVERLABDEBUG
									WickedCall_DrawPoint(fObjCenter.x, fObjCenter.y, fObjCenter.z, 4.0f, color, true);
									#endif

									static float fLastRayDist[6];
									ImVec4 vBestOffsetLeft = { 0,0,0,0 };
									ImVec4 vBestOffsetUp = { 0,0,0,0 };
									for (int i = 0; i < 4; i++)
									{
										if (i == 0) color = { 1.0,1.0,1.0,1.0 };
										if (i == 1) color = { 1.0,0.0,0.0,1.0 };
										if (i == 2) color = { 0.0,1.0,0.0,1.0 };
										if (i == 3) color = { 0.0,0.0,1.0,1.0 };
										ObjectSize = GetRealSizeToGridEntity(i);
										XMFLOAT3 fObjPos = { pObject->position.vecPosition.x + ObjectCenter.x + (ObjectSize.x) , pObject->position.vecPosition.y + ObjectCenter.y ,pObject->position.vecPosition.z + ObjectCenter.z + (ObjectSize.z) };
										#ifndef CENTERSYSTEM
										XMFLOAT3 fObjRayPos = { pObject->position.vecPosition.x + ObjectCenter.x - (ObjectSize.x) , pObject->position.vecPosition.y + ObjectCenter.y ,pObject->position.vecPosition.z + ObjectCenter.z - (ObjectSize.z) };
										#ifndef NOOVERLABDEBUG
										WickedCall_DrawPoint(fObjRayPos.x, fObjRayPos.y, fObjRayPos.z, 3.0f, color, true);
										#endif
										#else
										#ifndef NOOVERLABDEBUG
										WickedCall_DrawPoint(fObjPos.x, fObjPos.y, fObjPos.z, 4.0f, color, true);
										#endif
										#endif
										XMVECTOR vectorSub = XMVectorSubtract(XMLoadFloat3(&fObjPos), XMLoadFloat3(&fObjCenter));
										XMVECTOR rayDirection = XMVector3Normalize(vectorSub);
										XMFLOAT3 f3Dir;
										XMStoreFloat3(&f3Dir, rayDirection);
										float fHitX, fHitY, fHitZ, fdist = 99999.0;
										#ifndef CENTERSYSTEM
										if (WickedCall_SentRay(fObjRayPos.x, fObjRayPos.y, fObjRayPos.z, f3Dir.x, 0.0f, f3Dir.z, &fHitX, &fHitY, &fHitZ, NULL, NULL, NULL, NULL, GGRENDERLAYERS_NORMAL))
										#else
										if (WickedCall_SentRay(fObjCenter.x, fObjCenter.y, fObjCenter.z, f3Dir.x, 0.0f, f3Dir.z, &fHitX, &fHitY, &fHitZ, NULL, NULL, NULL, NULL, GGRENDERLAYERS_NORMAL))
										#endif
										{
											XMFLOAT3 fObjHit = { fHitX, fHitY, fHitZ };

											#ifndef CENTERSYSTEM
											float fdist = sqrt((fObjPos.x - fObjHit.x) * (fObjPos.x - fObjHit.x) +
												(fObjPos.z - fObjHit.z) * (fObjPos.z - fObjHit.z));
											fLastRayDist[i] = fdist;
											float fdist2 = sqrt((fObjRayPos.x - fObjPos.x) * (fObjRayPos.x - fObjPos.x) +
												(fObjRayPos.z - fObjPos.z) * (fObjRayPos.z - fObjPos.z));

											float fdist3 = sqrt((fObjRayPos.x - fObjHit.x) * (fObjRayPos.x - fObjHit.x) +
												(fObjRayPos.z - fObjHit.z) * (fObjRayPos.z - fObjHit.z));

											#else
											float fdist = sqrt((fObjPos.x - fObjHit.x) * (fObjPos.x - fObjHit.x) +
												(fObjPos.z - fObjHit.z) * (fObjPos.z - fObjHit.z));

											float fdist2 = sqrt((fObjCenter.x - fObjPos.x) * (fObjCenter.x - fObjPos.x) +
												(fObjCenter.z - fObjPos.z) * (fObjCenter.z - fObjPos.z));

											float fdist3 = sqrt((fObjCenter.x - fObjHit.x) * (fObjCenter.x - fObjHit.x) +
												(fObjCenter.z - fObjHit.z) * (fObjCenter.z - fObjHit.z));
											#endif

											float fDistFinal = fdist3 - fdist2;
											fDebug = fdist; //debug
											fDebug2 = fdist2; //debug
											fDebug3 = fdist3; //debug

											if (fdist3 < fdist2)
											{
												//Glue.
												float fDistFromPivot = -fdist;
												ImVec4 vOffset = { f3Dir.x*fDistFromPivot, f3Dir.y*fDistFromPivot , f3Dir.z*fDistFromPivot ,1.0f };

												if (i == 0)
													vBestOffsetLeft = vOffset;
												if (i == 2 && fLastRayDist[2] < fLastRayDist[0])
													vBestOffsetLeft = vOffset;
												if (i == 1)
													vBestOffsetUp = vOffset;
												if (i == 3 && fLastRayDist[3] < fLastRayDist[1])
													vBestOffsetUp = vOffset;
											}
											#ifndef NOOVERLABDEBUG
											WickedCall_DrawPoint(fHitX, fHitY, fHitZ, 6.0f, color, true);
											#endif
										}
									}
									if (vBestOffsetLeft.w > 0)
									{
										t.gridentityposx_f += vBestOffsetLeft.x;
										t.gridentityposz_f += vBestOffsetLeft.z;
									}
									if (vBestOffsetUp.w > 0)
									{
										t.gridentityposx_f += vBestOffsetUp.x;
										t.gridentityposz_f += vBestOffsetUp.z;
									}
								}
							}
						}

						//  move waypoint zone when move trigger entity
						if (t.grideleprof.trigger.waypointzoneindex > 0)
						{
							if (t.gridentity > 0)
							{
								waypoint_movetogrideleprof();
							}
							else
							{
								t.grideleprof.trigger.waypointzoneindex = 0;
							}
						}

						//  control modification of entity element details
						if (t.gridentitymodifyelement == 1)
						{
							if (t.gridedit.entityspraymode == 1)
							{
								if (t.gridedit.entitysprayrange > 0) t.gridedit.entitysprayrange -= 50;
							}
							else
							{
								if (t.grideleprof.light.range > 50) t.grideleprof.light.range -= 50;
							}
							t.gridentitymodifyelement = 0;
						}
						if (t.gridentitymodifyelement == 2)
						{
							if (t.gridedit.entityspraymode == 1)
							{
								if (t.gridedit.entitysprayrange < 1000) t.gridedit.entitysprayrange += 50;
							}
							else
							{
								if (t.grideleprof.light.range < 1000) t.grideleprof.light.range += 50;
							}
							t.gridentitymodifyelement = 0;
						}

					} //WICKED (!bPlaceEntity)

					//  extract entity (RMB) or place entity (LMB)
					if (bPlaceEntity)
					{
						bDetectTerrainOnly = false;
						// widget closure
						if (  t.widget.duplicatebuttonselected == 2 ) 
						{
							t.tentitytoselect=t.widget.pickedEntityIndex;
							t.widget.duplicatebuttonselected=0;
							t.gridentityautofind=7;
						}
						t.widget.pickedObject=0; 
						widget_updatewidgetobject ( );

						// either add entity to map OR extract one specified by 't.tentitytoselect'
						if ( t.gridentity != 0 ) 
						{
							// ADD ENTITY TO MAP
							// Determine if we will be adding (or moving an entity if spray mode)
							t.tentitybeingsprayed=0;
							t.tentitytomodifyindex=0;
							if ( t.gridedit.entityspraymode == 1 && t.entityprofile[t.gridentity].ismarker == 0 ) 
							{
								//  Scan area of spray and determine if density reached
								t.tpickrandoment=Rnd(t.tcountentinrange);
								t.tcountentinrange=0;
								for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
								{
									if (  t.entityelement[t.e].bankindex == t.gridentity ) 
									{
										//  this entity is same as that sprayed
										t.tdx_f=t.entityelement[t.e].x-t.gridentityposx_f;
										t.tdz_f=t.entityelement[t.e].z-t.gridentityposz_f;
										t.tdd_f=Sqrt(abs(t.tdx_f*t.tdx_f)+abs(t.tdz_f*t.tdz_f));
										if (  t.tdd_f <= t.gridedit.entitysprayrange ) 
										{
											//  this entity in range
											if (  t.tcountentinrange == 0  )  t.tentitytomodifyindex = t.e;
											if (  t.tcountentinrange == t.tpickrandoment  )  t.tentitytomodifyindex = t.e;
											++t.tcountentinrange;
										}
									}
								}
								if (  t.tcountentinrange <= 5  )  t.tentitytomodifyindex = 0;
								t.tentitybeingsprayed=1;
							}

							//  Store original entity cursor settings (for later random feature)
							t.storegridentityposx_f=t.gridentityposx_f;
							t.storegridentityposz_f=t.gridentityposz_f;
							t.storegridentityrotatex_f=t.gridentityrotatex_f;
							t.storegridentityrotatey_f=t.gridentityrotatey_f;
							t.storegridentityrotatez_f=t.gridentityrotatez_f;
							t.storegridentityrotatequatmode = t.gridentityrotatequatmode;
							t.storegridentityrotatequatx_f = t.gridentityrotatequatx_f;
							t.storegridentityrotatequaty_f = t.gridentityrotatequaty_f;
							t.storegridentityrotatequatz_f = t.gridentityrotatequatz_f;
							t.storegridentityrotatequatw_f = t.gridentityrotatequatw_f;
							t.storegridentityscalex_f=t.gridentityscalex_f;
							t.storegridentityscaley_f=t.gridentityscaley_f;
							t.storegridentityscalez_f=t.gridentityscalez_f;

							//  Spray allows entity placement to be randomised
							if (  t.tentitybeingsprayed == 1 ) 
							{
								//  rotation and scale back in (also uses native entity ROT values)
								t.gridentityrotatex_f=t.entityprofile[t.gridentity].rotx;
								t.gridentityrotatey_f=t.entityprofile[t.gridentity].roty;
								t.gridentityrotatez_f=t.entityprofile[t.gridentity].rotz;
								t.gridentityrotatequatmode = 0;
								t.gridentityscalex_f=t.gridentityscalex_f;
								t.gridentityscaley_f=t.gridentityscaley_f;
								t.gridentityscalez_f=t.gridentityscalez_f;
								if ( t.entityprofile[t.gridentity].ischaracter==0 && t.entityprofile[t.gridentity].noXZrotation==0 )
								{
									// ignore X and Z rotation for characters
									t.gridentityrotatex_f=t.gridentityrotatex_f+10.0-Rnd(20);
									t.gridentityrotatez_f=t.gridentityrotatez_f+10.0-Rnd(20);
									t.gridentityscaley_f=t.gridentityscaley_f+(Rnd(20)-10);
								}
								t.gridentityrotatey_f=t.gridentityrotatey_f+Rnd(360);
								t.gridentityrotatequatmode = 0;
								t.gridentityrotatequatx_f = 0;
								t.gridentityrotatequaty_f = 0;
								t.gridentityrotatequatz_f = 0;
								t.gridentityrotatequatw_f = 1;
								t.ttrandomposx_f=NewXValue(0,Rnd(360),Rnd(t.gridedit.entitysprayrange));
								t.ttrandomposz_f=NewZValue(0,Rnd(360),Rnd(t.gridedit.entitysprayrange));
								t.gridentityposx_f=t.gridentityposx_f+t.ttrandomposx_f;
								t.gridentityposz_f=t.gridentityposz_f+t.ttrandomposz_f;
								t.gridentityposy_f=BT_GetGroundHeight(t.terrain.TerrainID,t.gridentityposx_f,t.gridentityposz_f);
								//PE: Apply pivot here.
								ApplyPivotToGridEntity();

							}

							//  Version Control - stop high resource use
							t.resourceused=2; //version_resourcewarning ( );

							//  Either modify existing entity or place a new one (default behaviour)
							if (  t.tentitytomodifyindex>0 ) 
							{
								//  MODIFY EXISTING ENTITY
								t.entityelement[t.tentitytomodifyindex].x=t.gridentityposx_f;
								t.entityelement[t.tentitytomodifyindex].y=t.gridentityposy_f;
								t.entityelement[t.tentitytomodifyindex].z=t.gridentityposz_f;
								t.entityelement[t.tentitytomodifyindex].rx=t.gridentityrotatex_f;
								t.entityelement[t.tentitytomodifyindex].ry=t.gridentityrotatey_f;
								t.entityelement[t.tentitytomodifyindex].rz=t.gridentityrotatez_f;
								t.entityelement[t.tentitytomodifyindex].quatmode = t.gridentityrotatequatmode;
								t.entityelement[t.tentitytomodifyindex].quatx = t.gridentityrotatequatx_f;
								t.entityelement[t.tentitytomodifyindex].quaty = t.gridentityrotatequaty_f;
								t.entityelement[t.tentitytomodifyindex].quatz = t.gridentityrotatequatz_f;
								t.entityelement[t.tentitytomodifyindex].quatw = t.gridentityrotatequatw_f;
								t.tobj=t.entityelement[t.tentitytomodifyindex].obj;
								if (  t.tobj>0 ) 
								{
									if (  ObjectExist(t.tobj) == 1 ) 
									{
										PositionObject (  t.tobj,t.gridentityposx_f,t.gridentityposy_f,t.gridentityposz_f );
										RotateObject (  t.tobj,t.gridentityrotatex_f,t.gridentityrotatey_f,t.gridentityrotatez_f );
									}
								}
								if (t.gridedit.entityspraymode == 1)
								{
									//PE: Set selection to the one being changed.
									t.widget.pickedObject = t.tobj;
									t.widget.pickedEntityIndex = t.tentitytomodifyindex;

								}
							}
							else
							{
								//  PLACE NEW ENTITY
								//  after add, adjust so it auto-finds a Floor (  or wall (convenience) )
								t.gridentitydroptoground=1+t.entityprofile[t.gridentity].forwardfacing;
								if (  t.gridentitydroptoground == 2 ) 
								{
									 // already dealt with, positioned perfectly
								}
								t.gridentitydroptoground=0;

								//  find unique name for this selection (if flagged)
								if (  g.guseuniquelynamedentities == 0 ) 
								{
									//  use same name as original entity
									t.tbase_s=t.grideleprof.name_s;
								}
								else
								{
									t.tokay=0 ; t.tindex=1;
									if (  cstr(Lower(Left(t.grideleprof.name_s.Get(),Len(t.grideleproflastname_s.Get())))) == Lower(t.grideleproflastname_s.Get()) ) 
									{
										t.tbase_s=t.grideleproflastname_s;
									}
									else
									{
										t.tbase_s=t.grideleprof.name_s;
									}
									while (  t.tokay == 0 ) 
									{
										t.tokay=1 ; t.grideleprof.name_s=t.tbase_s ; t.grideleproflastname_s=t.tbase_s;
										if (  t.tindex>1  )  t.grideleprof.name_s = t.grideleprof.name_s+" "+Str(t.tindex);
										for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
										{
											if (  t.entityelement[t.e].bankindex>0 ) 
											{
												if (  cstr(Lower(t.entityelement[t.e].eleprof.name_s.Get())) == cstr(Lower(t.grideleprof.name_s.Get())) ) 
												{
													//  this name exists already, try another
													t.tokay=0 ; break;
												}
											}
										}
										++t.tindex;
									}
								}
								//  player start markers have exclusivity
								if (  t.entityprofile[t.gridentity].ismarker == 1 && t.entityprofile[t.gridentity].lives != -1 ) 
								{
									for ( t.e = 1 ; t.e<=  g.entityelementlist; t.e++ )
									{
										if (  t.entityelement[t.e].bankindex>0 ) 
										{
											if (  t.entityprofile[t.entityelement[t.e].bankindex].ismarker == 1 && t.entityprofile[t.entityelement[t.e].bankindex].lives != -1 ) 
											{
												t.tentitytoselect=t.e;
												gridedit_deleteentityfrommap ( );
											}
										}
									}
								}

								//  copy entity to map (keep selection for repeat process)
								if ( g.entityrubberbandlist.size() > 0 )
								{
									//PE: This normally makes a duplicate, what we dont need in this system.
									if ( 1 ) // when adding, need same functionality (smart object groups) pref.iEnableDragDropEntityMode )
									{
										//We might have to do some kind of support.
										int iStoreGridEntity = t.gridentity;

										// add parent entity
										//PE: InstanceObject - Cursor,Object Tools - objects must always be real clones.
										extern bool bNextObjectMustBeClone;
										bNextObjectMustBeClone = true;

										gridedit_addentitytomap();

										bNextObjectMustBeClone = false;

										if (iLastEntityOnCursor != 0 && iLastEntityOnCursor != t.e)
										{
											ReplaceEntityInGroupList(iLastEntityOnCursor, t.e);
											iLastEntityOnCursor = 0;
										}

										int iNewParentEntityIndex = t.e;

										if (pref.iEnableDragDropEntityMode && t.e > 0) {
											//PE: After placing it, sent it to widget.
											iWidgetSelection = t.e;
										}
										bDraggingActive = false;
										t.gridentity = iStoreGridEntity;
										t.e = iNewParentEntityIndex;

										if (bCreateNewGroupOnNextDrop)
										{
											//Create a new group
											fLastRubberBandX2 = 0; //We dont have a rubberband so...
											fLastRubberBandX1 = 0;
											fLastRubberBandY2 = 0;
											fLastRubberBandY1 = 0;

											CreateNewGroup(-1);
											bCreateNewGroupOnNextDrop = false;
										}
									}
									else
									{
										// store original ent ID pased down
										int iStoreGridEntity = t.gridentity;

										// add parent entity

										//PE: InstanceObject - Cursor,Object Tools - objects must always be real clones.
										extern bool bNextObjectMustBeClone;
										bNextObjectMustBeClone = true;

										gridedit_addentitytomap();

										bNextObjectMustBeClone = false;

										int iNewParentEntityIndex = t.e;

										t.entityelement[t.e].iHasParentIndex = t.gridentityhasparent;

										// add children for the parent
										int* piNewEntIndex = new int[g.entityrubberbandlist.size()];
										for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
										{
											int e = g.entityrubberbandlist[i].e;
											if (e == 0)
											{
												t.e = iNewParentEntityIndex;
											}
											else
											{
												// duplicate this child and add to map
												t.gridentity = t.entityelement[e].bankindex;

												//PE: InstanceObject - Cursor,Object Tools - objects must always be real clones.
												extern bool bNextObjectMustBeClone;
												bNextObjectMustBeClone = true;

												gridedit_addentitytomap(); //use entityelement[t.e].

												bNextObjectMustBeClone = false;

												// update child with pos/rot from source
												t.entityelement[t.e].x = t.entityelement[e].x;
												t.entityelement[t.e].y = t.entityelement[e].y;
												t.entityelement[t.e].z = t.entityelement[e].z;
												t.entityelement[t.e].rx = t.entityelement[e].rx;
												t.entityelement[t.e].ry = t.entityelement[e].ry;
												t.entityelement[t.e].rz = t.entityelement[e].rz;			
												t.entityelement[t.e].quatmode = t.entityelement[e].quatmode;
												t.entityelement[t.e].quatx = t.entityelement[e].quatx;
												t.entityelement[t.e].quaty = t.entityelement[e].quaty;
												t.entityelement[t.e].quatz = t.entityelement[e].quatz;
												t.entityelement[t.e].quatw = t.entityelement[e].quatw;
												t.entityelement[t.e].editorfixed = t.entityelement[e].editorfixed;
												t.entityelement[t.e].staticflag = t.entityelement[e].staticflag;
												t.entityelement[t.e].scalex = t.entityelement[e].scalex;
												t.entityelement[t.e].scaley = t.entityelement[e].scaley;
												t.entityelement[t.e].scalez = t.entityelement[e].scalez;
												t.entityelement[t.e].soundset = t.entityelement[e].soundset;
												t.entityelement[t.e].soundset1 = t.entityelement[e].soundset1;
												t.entityelement[t.e].soundset2 = t.entityelement[e].soundset2;
												t.entityelement[t.e].soundset3 = t.entityelement[e].soundset3;
												t.entityelement[t.e].soundset4 = t.entityelement[e].soundset4;
												t.entityelement[t.e].soundset5 = t.entityelement[e].soundset5;
												t.entityelement[t.e].soundset6 = t.entityelement[e].soundset6;
												t.entityelement[t.e].eleprof = t.entityelement[e].eleprof;
												PositionObject(t.entityelement[t.e].obj, t.entityelement[t.e].x, t.entityelement[t.e].y, t.entityelement[t.e].z);
												RotateObject(t.entityelement[t.e].obj, t.entityelement[t.e].rx, t.entityelement[t.e].ry, t.entityelement[t.e].rz);
											}
											piNewEntIndex[i] = t.e;
										}

										// and once all new entities created, link new parents to them
										for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
										{
											t.e = piNewEntIndex[i];
											int e = g.entityrubberbandlist[i].e;
											if (e != 0)
											{
												// find source parent of this child, if any
												if (t.entityelement[e].iHasParentIndex > 0)
												{
													if (t.entityelement[e].iHasParentIndex == t.gridentityextractedindex)
													{
														// entity was child of parent entity extacted
														t.entityelement[t.e].iHasParentIndex = iNewParentEntityIndex;
													}
													else
													{
														// entity was child of another entity (a child in here)
														for (int ii = 0; ii < (int)g.entityrubberbandlist.size(); ii++)
														{
															int ee = g.entityrubberbandlist[ii].e;
															if (ee > 0)
															{
																if (t.entityelement[e].iHasParentIndex == ee)
																{
																	// entity was child of parent entity extacted
																	t.entityelement[t.e].iHasParentIndex = piNewEntIndex[ii];
																}
															}
														}
													}
												}
											}
										}
										SAFE_DELETE(piNewEntIndex);

										// restore original ent ID
										t.gridentity = iStoreGridEntity;
										t.e = iNewParentEntityIndex;
									}
								}
								else
								{
									//TUT: Add here.
									//TUT: PLACEIT
									CheckTutorialAction("PLACEIT"); //Tutorial: check if we are waiting for this action
									if (bTutorialCheckAction) TutorialNextAction(); //If we are waiting for PLACEIT its done.

									//PE: InstanceObject - Cursor,Object Tools - objects must always be real clones.
									extern bool bNextObjectMustBeClone;
									bNextObjectMustBeClone = true;

									if (t.gridedit.entityspraymode == 1 || t.inputsys.keyshift)
									{
										bNextObjectMustBeClone = false;
									}

									gridedit_addentitytomap ( );

									if (!bNextObjectMustBeClone)
									{
										// Any particle that was duplicated should have a new emitter id.
										if (t.entityprofile[t.entityelement[t.e].bankindex].ismarker == 10)
										{
											t.entityelement[t.e].eleprof.newparticle.emitterid = -1;
											entity_updateparticleemitter(t.e);
											//PE: Not from lua so must do these.
											int iParticleEmitter = t.entityelement[t.e].eleprof.newparticle.emitterid;
											if (iParticleEmitter != -1)
											{
												gpup_setParticleScale(iParticleEmitter, t.entityelement[t.e].eleprof.newparticle.bParticle_Size);
												gpup_setEffectAnimationSpeed(iParticleEmitter, t.entityelement[t.e].eleprof.newparticle.fParticle_Speed);
												gpup_setEffectOpacity(iParticleEmitter, t.entityelement[t.e].eleprof.newparticle.fParticle_Opacity);
											}

										}
									}

									bNextObjectMustBeClone = false;

									if (iLastEntityOnCursor != 0 && iLastEntityOnCursor != t.e)
									{
										ReplaceEntityInGroupList(iLastEntityOnCursor, t.e);
										iLastEntityOnCursor = 0;
									}

									if (pref.iEnableDragDropEntityMode && t.e > 0) 
									{
										//PE: After placing it, sent it to widget.
										iWidgetSelection = t.e;
										//PE: Instant activete it.
										t.widget.pickedEntityIndex = iWidgetSelection;
										t.entityelement[t.widget.pickedEntityIndex].editorlock = 0;
										t.widget.pickedObject = t.entityelement[iWidgetSelection].obj;
									}
									bDraggingActive = false;
								}

								//  if drag char to start marker, assign here
								if (  t.inputsys.willmakethirdperson>0 ) 
								{
									// set this characte as third person and game as TPP 
									t.playercontrol.thirdperson.enabled=1;
									t.playercontrol.thirdperson.charactere=t.tupdatee;
									t.playercontrol.thirdperson.startmarkere=t.inputsys.willmakethirdperson;

									// also change the script of the character to a third person script (by default)
									t.entityelement[t.tupdatee].eleprof.aimain_s = "tpp\\thirdperson.lua";
									t.entityelement[t.tupdatee].eleprof.aimain = 0;
								}

								//  if trigger zone, remove from entity cursor as well
								if (  t.entityprofile[t.gridentity].ismarker == 3 || t.entityprofile[t.gridentity].ismarker == 6 || t.entityprofile[t.gridentity].ismarker == 8 ) 
								{
									//  detatch trigger zone / checkpoint here
									t.grideleprof.trigger.waypointzoneindex=0;
									t.gridentitydelete=1;
								}
								else
								{
									//  update for refresh
									t.refreshgrideditcursor=1;
								}

								// if was targetting a parent for link/associate connection (CTRL down)
								// then make this entity a child of the entity targetted
								if ( t.tentityoverdraggingcursor > 0 )
								{
									// parents influence children when they move, and shift children relatively
									t.entityelement[t.e].iHasParentIndex = t.tentityoverdraggingcursor;
								}
								else
								{
									// if still holding CTRL put place entity down on NON-entity, remove parent link/associated status
									if ( t.inputsys.k_s == "l" ) 
									{
										t.entityelement[t.e].iHasParentIndex = 0;
									}
								}

								// if NOT holding SHIFT, delete after one placement
								bool bShiftBeingHeldDown = false;
								if (t.inputsys.keyshift != 0)
								{
									if (pref.iEnableDragDropWidgetSelect == 1)
									{
										bool bJustForInitialDragIn = false;
										if (bDraggingActive == false && fHitOffsetX == 0 && fHitOffsetY == 0 && fHitOffsetZ == 0) bJustForInitialDragIn = true;
										if (bDraggingActive == true && t.gridentityposx_f == 0 && t.gridentityposz_f == 0) bJustForInitialDragIn = true;
										if (bDraggingActiveInitial == true)	bJustForInitialDragIn = true;
										if (bJustForInitialDragIn == true)
										{
											// BUT only apply SHIFT DOWN (duplicate) when carrying/dragging an object, 
											// and NOT when click an object, then hold down SHIFT, then release as this would create a mistaken duplicate
											// and mess up multi selection in the same session!
											bShiftBeingHeldDown = true;
										}
									}
									else
									{
										// a request that if NOT in widget mode, can allow the SHIFT+CLICK to duplicate in the viewport - works nice :)
										bShiftBeingHeldDown = true;
									}
								}
								else
								{
									// can ONLY persist for the one-off current object being dragged in (as its a direct copy of that Y offset value)
									// and in cases where SHIFT keeps the selection for secondary additions of this dragged in object
									g_fSpecialDragInYAdjustment = 0.0f;
								}

								if (bShiftBeingHeldDown == false && t.gridedit.entityspraymode == 0 )
								{
									t.inputsys.kscancode = 211;
								}
								//PE: Dont allow spraying with markers.
								if (bShiftBeingHeldDown == false && t.gridedit.entityspraymode == 1 && t.entityprofile[t.gridentity].ismarker != 0 )
								{
									t.inputsys.kscancode = 211;
								}

								//PE: If this system rubberband is not dublicated, so we cant do shift. also it react on mouse release not mouse down.
								//PE: So mouse release we have no rubberband selection, if still works fine with single objects.
								if (g.entityrubberbandlist.size() > 0)
								{
									t.inputsys.kscancode = 211;
								}
								t.selstage=1;
							}

							// restore original entity cursor position (after random spray feature)
							t.gridentityposx_f=t.storegridentityposx_f;
							t.gridentityposz_f=t.storegridentityposz_f;
							t.gridentityrotatex_f=t.storegridentityrotatex_f;
							t.gridentityrotatey_f=t.storegridentityrotatey_f;
							t.gridentityrotatez_f=t.storegridentityrotatez_f;
							t.gridentityrotatequatmode = t.storegridentityrotatequatmode;
							t.gridentityrotatequatx_f = t.storegridentityrotatequatx_f;
							t.gridentityrotatequaty_f = t.storegridentityrotatequaty_f;
							t.gridentityrotatequatz_f = t.storegridentityrotatequatz_f;
							t.gridentityrotatequatw_f = t.storegridentityrotatequatw_f;
							t.gridentityscalex_f=t.storegridentityscalex_f;
							t.gridentityscaley_f=t.storegridentityscaley_f;
							t.gridentityscalez_f=t.storegridentityscalez_f;
							
							if (t.gridedit.entityspraymode == 1 && bSprayMoveWithMouse)
							{
								t.gridentityposx_f = t.inputsys.localx_f;
								t.gridentityposy_f = t.inputsys.localcurrentterrainheight_f;
								t.gridentityposz_f = t.inputsys.localy_f;
							}
						}
						else
						{
							// EXTRACT ENTITY FROM MAP
							// Set flag so do not instantly delete entity (below)
							t.onetimeentitypickup=1;

							//  extract entity from the map
							if ( t.tentitytoselect>0 ) 
							{
								if ( t.entityelement[t.tentitytoselect].editorfixed == 0 ) 
								{
									t.gridentityeditorfixed=t.entityelement[t.tentitytoselect].editorfixed;
									t.gridentity=t.entityelement[t.tentitytoselect].bankindex;
									t.ttrygridentitystaticmode=t.entityelement[t.tentitytoselect].staticflag;
									t.ttrygridentity=t.gridentity; editor_validatestaticmode ( );
									t.gridedit.autoflatten=t.entityprofile[t.gridentity].autoflatten;
									t.gridedit.entityspraymode=0;
									if ( t.gridentityautofind == 7 ) 
									{
										//  widget extracts without forcing entity to Floor
										t.gridentityautofind=0;
										t.gridentityposoffground=1;
										t.gridentityusingsoftauto=0;
									}
									else
									{
										t.gridentityposoffground=0;
										t.gridentityusingsoftauto=1;
										// MAX handles its own positioning system
										{
											t.gridentityautofind=0;
										}
									}
									t.gridentitysurfacesnap=0; // surfacesnap off as messes up extract offset for entity
									t.gridentityextractedindex = t.tentitytoselect;
									t.gridentityhasparent = 0;//t.entityelement[t.tentitytoselect].iHasParentIndex; 210317 - break association when extract so can place free of parent
									t.gridentityposx_f=t.entityelement[t.tentitytoselect].x;
									t.gridentityposy_f=t.entityelement[t.tentitytoselect].y;
									t.gridentityposz_f=t.entityelement[t.tentitytoselect].z;
									t.gridentityrotatex_f=t.entityelement[t.tentitytoselect].rx;
									t.gridentityrotatey_f=t.entityelement[t.tentitytoselect].ry;
									t.gridentityrotatez_f=t.entityelement[t.tentitytoselect].rz;
									t.gridentityrotatequatmode = t.entityelement[t.tentitytoselect].quatmode;
									t.gridentityrotatequatx_f = t.entityelement[t.tentitytoselect].quatx;
									t.gridentityrotatequaty_f = t.entityelement[t.tentitytoselect].quaty;
									t.gridentityrotatequatz_f = t.entityelement[t.tentitytoselect].quatz;
									t.gridentityrotatequatw_f = t.entityelement[t.tentitytoselect].quatw;
									if (t.entityprofile[t.gridentity].ismarker == 10)
									{
										t.gridentityscalex_f = 100.0f + t.entityelement[t.tentitytoselect].scalex;
										t.gridentityscaley_f = 100.0f + t.entityelement[t.tentitytoselect].scaley;
										t.gridentityscalez_f = 100.0f + t.entityelement[t.tentitytoselect].scalez;
									}
									else
									{
										t.gridentityscalex_f = ObjectScaleX(t.entityelement[t.tentitytoselect].obj);
										t.gridentityscaley_f = ObjectScaleY(t.entityelement[t.tentitytoselect].obj);
										t.gridentityscalez_f = ObjectScaleZ(t.entityelement[t.tentitytoselect].obj);
									}
									t.grideleprof = t.entityelement[t.tentitytoselect].eleprof;
									t.grideleproflastname_s=t.grideleprof.name_s;

									//  Transfer any waypoint association
									t.waypointindex=t.entityelement[t.tentitytoselect].eleprof.trigger.waypointzoneindex;
									t.grideleprof.trigger.waypointzoneindex=t.waypointindex;
									t.waypoint[t.waypointindex].linkedtoentityindex=0;

									//  delete from map (checks grideleprof.trigger.waypointzoneindex too)
									gridedit_deleteentityfrommap ( );
									t.refreshgrideditcursor=1;

									// remove entity index from rubber band selection
									for ( int i = 0; i < (int)g.entityrubberbandlist.size(); i++ )
										if ( g.entityrubberbandlist[i].e == t.tentitytoselect )
											g.entityrubberbandlist[i].e = 0;

									//  Ensure grab GetPoint (  does not move entity! )
									t.inputsys.dragoffsetx_f=t.entityelement[t.tentitytoselect].x-t.inputsys.originallocalx_f;
									t.inputsys.dragoffsety_f=t.entityelement[t.tentitytoselect].z-t.inputsys.originallocaly_f;
								}
							}
						}
						t.selstage=1;
					}

					//  If EXTRACT button clicked initially, first reposition mouse before operation and to get mouse coord correct
					if ( t.widget.duplicatebuttonselected == 1 && t.gridentity == 0 ) 
					{
						//  work out screen/mouse position from real-world XZ coordinate
						if (  t.inputsys.picksystemused == 0 ) 
						{
							//  only if not using pick stsrem (no Floor (  to target) )
							SetCurrentCamera ( 0 );

							//  fix camera range for correct projection matrix
							SetCameraRange ( DEFAULT_NEAR_PLANE, DEFAULT_FAR_PLANE );
							t.screenwidth_f=800.0;
							t.screenheight_f=600.0;
							GetProjectionMatrix (  g.m4_projection );
							GetViewMatrix (  g.m4_view );

							// works in DX9 (D3DXVec4Transform) but not DX11 (KMATRIX)
							SetVector3 ( g.v3_far, ObjectPositionX(t.widget.widgetXYObj), ObjectPositionY(t.widget.widgetXYObj), ObjectPositionZ(t.widget.widgetXYObj) );
							TransformVectorCoordinates3 ( g.v3_far, g.v3_far, g.m4_view );
							t.tx_f=GetXVector3(g.v3_far);
							t.ty_f=GetYVector3(g.v3_far);

							SetVector4 ( g.v4_far, GetXVector3(g.v3_far), GetYVector3(g.v3_far), GetZVector3(g.v3_far), 1 );
							TransformVector4 ( g.v4_far,g.v4_far,g.m4_projection );
							t.tx_f=GetXVector4(g.v4_far);
							t.ty_f=GetYVector4(g.v4_far);
							t.tx_f=t.tx_f/GetWVector4(g.v4_far);
							t.ty_f=t.ty_f/GetWVector4(g.v4_far);

							t.tadjustedtoareax_f=(((t.tx_f+1.0)/2.0)*(GetDisplayWidth()+0.0));
							t.tadjustedtoareay_f=((((t.ty_f*-1)+1.0)/2.0)*(GetDisplayHeight()+0.0));
							t.inputsys.xmouse=t.tadjustedtoareax_f;
							t.inputsys.ymouse=t.tadjustedtoareay_f;
							t.tideframestartx=148 ; t.tideframestarty=96;


							editor_refreshcamerarange ( );
						}

						// trigger actual extraction on next cycle
						t.widget.duplicatebuttonselected = 2;
					}
				}

				HandleObjectDeletion();

				bool bDisableWidgetSelection = false;
				if (bDotObjectDragging)
					bDisableWidgetSelection = true;

				//PE: Disable selections when imgui is in drag mode.
				ImGuiContext& gui = *GImGui;
				if (gui.DragDropActive)
				{
					bDisableWidgetSelection = true;
				}

				// Select widget controlled object
				if (!bDisableWidgetSelection && (t.inputsys.mclick == 1|| iWidgetSelection > 0 ) && t.gridentity == 0)
				{
					if (iWidgetSelection > 0)
					{
						t.tentitytoselect = iWidgetSelection;
					}
					if ( t.tentitytoselect>0 ) 
					{
						//PE: Dont allow changing group when in edit mode.
						if (!group_editing_on && !bRubberBandCreated && t.tentitytoselect != iLastSelectedEntity)
						{
							//Clear rubberband. Allow CTRL to select multiply groups.
							if (t.inputsys.keycontrol == 0) {
								//Only if not in same group.
								int grouplist = isEntityInGroupList(t.tentitytoselect);
								
								if (iLastSelectedEntityGroup != grouplist)
								{
									iLastSelectedEntityGroup = grouplist;
									current_selected_group = grouplist;
									g.entityrubberbandlist.clear();
								}
								else
								{
									if (t.tentitytoselect != iLastSelectedEntity)
									{
										//PE: If we have made a selection not in rubberband, clear rubberband.
										if (g.entityrubberbandlist.size() > 0)
										{
											bool bInRubberBand = false;
											for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
											{
												int ent = g.entityrubberbandlist[i].e;
												if (ent == t.tentitytoselect)
												{
													bInRubberBand = true;
													break;
												}
											}
											if (!bInRubberBand)
											{
												//New selection not in rubberband, clear.
												g.entityrubberbandlist.clear();
											}
										}
									}
								}
							}

							iLastSelectedEntity = t.tentitytoselect;
						}
						if (bRubberBandCreated)
						{
							int grouplist = isEntityInGroupList(t.tentitytoselect);
							iLastSelectedEntityGroup = grouplist;

							if (t.tentitytoselect != iLastSelectedEntity)
							{
								//PE: If we have made a selection not in rubberband, clear rubberband.
								if (g.entityrubberbandlist.size() > 0)
								{
									bool bInRubberBand = false;
									for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
									{
										int ent = g.entityrubberbandlist[i].e;
										if (ent == t.tentitytoselect)
										{
											bInRubberBand = true;
											break;
										}
									}
									if (!bInRubberBand)
									{
										//New selection not in rubberband, clear.
										g.entityrubberbandlist.clear();
									}
								}
								iLastSelectedEntity = t.tentitytoselect;
							}

						}
							
						bRubberBandCreated = false;
						if (pref.iEnableDragDropEntityMode && iWidgetSelection == 0)
						{
							//PE: Disable single click, widget selection, and replace with entity cursor.
							CheckGroupListForRubberbandSelections(t.tentitytoselect);
							AddEntityToCursor(t.tentitytoselect,false);

							//PE: Instant activate it.
							t.widget.pickedEntityIndex = t.tentitytoselect;

							#ifdef ALLOWSELECTINGLOCKEDOBJECTS
							if (!t.entityelement[t.widget.pickedEntityIndex].editorlock)
							{
							#endif
								t.entityelement[t.widget.pickedEntityIndex].editorlock = 0;
								t.widget.pickedObject = 0; //t.entityelement[t.tentitytoselect].obj;

								t.tentitytoselect = 0;
								bReadyToDropEntity = true;
								bWaitOnMouseRelease = true;
								iDragDropActive = 0;
								bDraggingActive = true;

							#ifdef ALLOWSELECTINGLOCKEDOBJECTS
							}
							#endif
						}
						else
						{
							bDraggingActive = false;
							iWidgetSelection = 0;
							if (t.widget.pickedObject == 0)
							{
								//PE: respect markers mode only.
								if (t.gridentitymarkersmodeonly == 0 || (t.gridentitymarkersmodeonly == 1 && t.entityprofile[t.entityelement[t.tentitytoselect].bankindex].ismarker != 0))
								{
									t.widget.pickedEntityIndex = t.tentitytoselect;
									t.widget.pickedObject = t.entityelement[t.tentitytoselect].obj;
									#ifndef ALLOWSELECTINGLOCKEDOBJECTS
									t.entityelement[t.widget.pickedEntityIndex].editorlock = 0;
									#endif
									// makes widget gadgets easier to manipulate
									t.widget.offsetx = g.glastpickedx_f - ObjectPositionX(t.widget.pickedObject);
									t.widget.offsety = g.glastpickedy_f - ObjectPositionY(t.widget.pickedObject);
									t.widget.offsetz = g.glastpickedz_f - ObjectPositionZ(t.widget.pickedObject);

									CheckGroupListForRubberbandSelections(t.tentitytoselect);

									// 271015 - this may not be required as it is duplicated later on..
									if (g.entityrubberbandlist.size() > 0)
										gridedit_moveentityrubberband();
									else
									{
										//MessageBoxA(NULL, "move", "", MB_OK);
									}
								}
							}
						}
					}
				}

				// zoom into entity properties (or EBE EDIT)
				if ( t.widget.propertybuttonselected == 1 ) 
				{
					t.widget.propertybuttonselected = 0;
					if ( t.widget.pickedEntityIndex > 0 ) 
					{
						int entid = t.entityelement[t.widget.pickedEntityIndex].bankindex;
						if ( t.entityprofile[entid].isebe != 0 )
						{
							// EBE entity - begin editing this site
							ebe_newsite ( t.widget.pickedEntityIndex );
						}

						//  End widget control of this object
						t.widget.pickedObject=0;
					}
				}
				if (  t.widget.propertybuttonselected == 2 ) 
				{
					// Entity properties or EBE Save
					t.widget.propertybuttonselected = 0;
					if ( t.widget.pickedEntityIndex > 0 ) 
					{
						int entid = t.entityelement[t.widget.pickedEntityIndex].bankindex;
						if ( t.entityprofile[entid].isebe != 0 )
						{
							// EBE entity - begin editing this site
							if ( ebe_save ( t.widget.pickedEntityIndex ) == 1 )
							{
							}

							// and close widget as Save bit big deal
							widget_switchoff();
						}
						else
						{
							// regular entity
							// prepare zoom-in adjustment vars
							t.tentitytoselect=t.widget.pickedEntityIndex;
							t.e=t.tentitytoselect;

							t.gridentityinzoomview=t.e;
							t.zoomviewtargetx_f=t.entityelement[t.e].x;
							t.zoomviewtargety_f=t.entityelement[t.e].y;
							t.zoomviewtargetz_f=t.entityelement[t.e].z;
							t.zoomviewtargetrx_f=t.entityelement[t.e].rx;
							t.zoomviewtargetry_f=t.entityelement[t.e].ry;
							t.zoomviewtargetrz_f=t.entityelement[t.e].rz;
							gridedit_updatezoomviewvalues ( );

							//  extract entity from the map
							t.gridentityeditorfixed=t.entityelement[t.e].editorfixed;
							t.gridentity=t.entityelement[t.e].bankindex;
							t.ttrygridentitystaticmode=t.entityelement[t.e].staticflag;
							t.ttrygridentity=t.gridentity ; editor_validatestaticmode ( );
							t.gridentityautofind=0;
							t.gridentityposoffground=1;
							t.gridentityusingsoftauto=0;
							t.gridentitysurfacesnap=1-g.gdisablesurfacesnap;
							t.gridentityhasparent=t.entityelement[t.e].iHasParentIndex;
							t.gridentityposx_f=t.entityelement[t.e].x;
							t.gridentityposy_f=t.entityelement[t.e].y;
							t.gridentityposz_f=t.entityelement[t.e].z;
							t.gridentityrotatex_f=t.entityelement[t.e].rx;
							t.gridentityrotatey_f=t.entityelement[t.e].ry;
							t.gridentityrotatez_f=t.entityelement[t.e].rz;
							t.gridentityrotatequatmode = t.entityelement[t.e].quatmode;
							t.gridentityrotatequatx_f = t.entityelement[t.e].quatx;
							t.gridentityrotatequaty_f = t.entityelement[t.e].quaty;
							t.gridentityrotatequatz_f = t.entityelement[t.e].quatz;
							t.gridentityrotatequatw_f = t.entityelement[t.e].quatw;
							if (t.entityprofile[t.gridentity].ismarker == 10)
							{
								t.gridentityscalex_f = 100.0f + t.entityelement[t.e].scalex;
								t.gridentityscaley_f = 100.0f + t.entityelement[t.e].scaley;
								t.gridentityscalez_f = 100.0f + t.entityelement[t.e].scalez;
							}
							else
							{
								t.gridentityscalex_f = ObjectScaleX(t.entityelement[t.e].obj);
								t.gridentityscaley_f = ObjectScaleY(t.entityelement[t.e].obj);
								t.gridentityscalez_f = ObjectScaleZ(t.entityelement[t.e].obj);
							}
							t.grideleprof = t.entityelement[t.e].eleprof;

							//  Transfer any waypoint association
							t.waypointindex=t.entityelement[t.e].eleprof.trigger.waypointzoneindex;
							t.grideleprof.trigger.waypointzoneindex=t.waypointindex;
							t.waypoint[t.waypointindex].linkedtoentityindex=0;

							//  Delete entity from map
							gridedit_deleteentityfrommap ( );
							t.refreshgrideditcursor=1;

							//  simply use its current position (no offset)
							t.inputsys.dragoffsetx_f=0;
							t.inputsys.dragoffsety_f=0;

							//  zoom in to entity for fine detail
							//PE: In wicked we dont actually zoom in so keep cx,cy
							t.inputsys.doautozoomview=1;
							if (t.zoomviewcamerarange_f > 2000.0f)
							{
								//This can fail after test game. if really large set defaults.
								t.zoomviewcamerarange_f = 175.0f;
								t.zoomviewcameraheight_f = 150.0f;
							}
							//  disable icons that interfere with zoom mode
							editor_disableforzoom ( );
	
							HideObject ( t.editor.objectstartindex+5 );
							t.selstage=1;

							//  prepare entity property handler
							#if defined(ENABLEIMGUI) && !defined(USEOLDIDE)
							//PE: Just open the window..
							bEntity_Properties_Window = true;
							#else
							interface_openpropertywindow ( );
							#endif

							//  End widget control of this object
							t.widget.pickedObject=0;
						}
					}
				}

				bool bDisableRubberBandMoving = false;
				if (current_selected_group >= 0 && group_editing_on)
				{
					bDisableRubberBandMoving = true;
				}
				// update rubberband selection connected to primary cursor entity
				if ( !bDisableRubberBandMoving && t.gridentity > 0 && g.entityrubberbandlist.size() > 1 && t.fOldGridEntityX > -99999.0f )
				{
					float fMovedActiveObjectX = t.gridentityposx_f - t.fOldGridEntityX;
					float fMovedActiveObjectY = t.gridentityposy_f - t.fOldGridEntityY;
					float fMovedActiveObjectZ = t.gridentityposz_f - t.fOldGridEntityZ;
					t.gridentityrotatex_f = t.fOldGridEntityRX;
					t.gridentityrotatey_f = t.fOldGridEntityRY;
					t.gridentityrotatez_f = t.fOldGridEntityRZ;
					t.gridentityrotatequatmode = t.fOldGridEntityQuatMode;
					t.gridentityrotatequatx_f = t.fOldGridEntityQuatX;
					t.gridentityrotatequaty_f = t.fOldGridEntityQuatY;
					t.gridentityrotatequatz_f = t.fOldGridEntityQuatZ;
					t.gridentityrotatequatw_f = t.fOldGridEntityQuatW;
					t.tobj = t.gridentityobj;
					if ( t.tobj>0 ) 
					{
						// rotate all selected around t.tobj, the active object
						GGQUATERNION QuatAroundX, QuatAroundY, QuatAroundZ;
						GGQuaternionRotationAxis(&QuatAroundX, &GGVECTOR3(1, 0, 0), GGToRadian(0));
						GGQuaternionRotationAxis(&QuatAroundY, &GGVECTOR3(0, 1, 0), GGToRadian(0));
						GGQuaternionRotationAxis(&QuatAroundZ, &GGVECTOR3(0, 0, 1), GGToRadian(0));
						GGQUATERNION quatRotationEvent = QuatAroundX * QuatAroundY * QuatAroundZ;
						SetStartPositionsForRubberBand(t.tobj);
						RotateAndMoveRubberBand(t.tobj, fMovedActiveObjectX, fMovedActiveObjectY, fMovedActiveObjectZ, quatRotationEvent);
					}
				}
				// record all current offsets from primary cursor entity and rubberband selection
				if (t.gridentity > 0)
				{
					//PE: Dont reset t.fOldGridEntityX when we got a t.gridentity
				}
				else
				{
					//PE: t.gridentityposx_f can go below -1, if you drag it way out there , and you could loose moving of rubberband (out of sync).
					if (!bDisableRubberBandMoving)
					{
						t.fOldGridEntityX = -99999.0f;
						t.fOldGridEntityY = -99999.0f;
						t.fOldGridEntityZ = -99999.0f;
						t.fOldGridEntityRX = -99999.0f;
						t.fOldGridEntityRY = -99999.0f;
						t.fOldGridEntityRZ = -99999.0f;
						t.fOldGridEntityQuatMode = 0;
						t.fOldGridEntityQuatX = 0;
						t.fOldGridEntityQuatY = 0;
						t.fOldGridEntityQuatZ = 0;
						t.fOldGridEntityQuatW = 1;
					}
				}
				if (!bDisableRubberBandMoving && t.gridentity > 0 && t.gridentityobj > 0 && g.entityrubberbandlist.size() > 1 )
				{
					t.fOldGridEntityX = t.gridentityposx_f;
					t.fOldGridEntityY = t.gridentityposy_f;
					t.fOldGridEntityZ = t.gridentityposz_f;
					t.fOldGridEntityRX = t.gridentityrotatex_f;
					t.fOldGridEntityRY = t.gridentityrotatey_f;
					t.fOldGridEntityRZ = t.gridentityrotatez_f;
					t.fOldGridEntityQuatMode = t.gridentityrotatequatmode;
					t.fOldGridEntityQuatX = t.gridentityrotatequatx_f;
					t.fOldGridEntityQuatY = t.gridentityrotatequaty_f;
					t.fOldGridEntityQuatZ = t.gridentityrotatequatz_f;
					t.fOldGridEntityQuatW = t.gridentityrotatequatw_f;
					for ( int i = 0; i < (int)g.entityrubberbandlist.size(); i++ )
					{
						int e = g.entityrubberbandlist[i].e;
						GGVECTOR3 VecPos;
						VecPos.x = t.entityelement[e].x - t.gridentityposx_f;
						VecPos.y = t.entityelement[e].y - t.gridentityposy_f;
						VecPos.z = t.entityelement[e].z - t.gridentityposz_f;
						int tobj = t.entityelement[e].obj;
						if ( tobj > 0 )
						{
							float fDet = 0.0f;
							sObject* pObject = GetObjectData(tobj);
							GGMATRIX inverseMatrix = pObject->position.matObjectNoTran;
							GGMatrixInverse ( &inverseMatrix, &fDet, &inverseMatrix );
							GGVec3TransformCoord ( &VecPos, &VecPos, &inverseMatrix );
							g.entityrubberbandlist[i].x = VecPos.x;
							g.entityrubberbandlist[i].y = VecPos.y;
							g.entityrubberbandlist[i].z = VecPos.z;
						}
					}
				}

				//  gridentity delete
				if (  t.gridentitydelete == 1 ) 
				{
					//  Delete any associated waypoint/trigger zone
					t.waypointindex=t.grideleprof.trigger.waypointzoneindex;
					if (  t.waypointindex>0 ) 
					{
						t.w=t.waypoint[t.waypointindex].start;
						waypoint_delete ( );
					}
					t.grideleprof.trigger.waypointzoneindex=0;
					//  delete grid entity object and reset
					t.gridentitydelete=0;
					if (  t.gridentityobj>0 ) 
					{
						DeleteObject (  t.gridentityobj );
						t.gridentityobj=0;
					}
					t.refreshgrideditcursor=1;
					t.gridentity=0;
					t.gridentityposoffground=0;
					t.gridentityusingsoftauto=0;
					t.gridentitysurfacesnap=1-g.gdisablesurfacesnap;
					// MAX handles its own positioning system
					t.gridentityautofind = 0;
					t.inputsys.dragoffsetx_f=0;
					t.inputsys.dragoffsety_f=0;
					editor_refreshentitycursor ( );
					t.widget.pickedObject=0;

					//PE: We dont actualle make new duplicates so can skib this.
					if (1)// same functionality pref.iEnableDragDropEntityMode)
					{
						//We might have to do some kind of support.
					}
					else
					{
						bool bDisableRubberBandMoving = false;
						if (current_selected_group >= 0 && group_editing_on)
						{
							bDisableRubberBandMoving = true;
						}
						if (!bDisableRubberBandMoving)
						{
							// if rubberband selection, delete all in selection
							gridedit_deleteentityrubberbandfrommap();
						}
					}

					// flag also used to restore highlighting behavior
					t.gridentityextractedindex = 0;

					// when place down, ensure waypoint not affected until release mouse button
					t.mclickpressed = 1;
				}
			}

			if (  t.inputsys.mclick == 0 && t.selstage == 1 ) 
			{
				t.selstage=0;
			}
			if (  t.gridedit.entityspraymode == 1 && t.selstage == 1 ) 
			{
				//  entity spray keeps going while button pressed
				t.selstage=0;
			}
		}

		// this is triggered when set to a negative, and continues to force find surface until zero
		if (iObjectMoveModeDropSystem < 0)
		{
			iObjectMoveModeDropSystem++;
			iForceScancode = 13;
		}
	}
}

void gridedit_updatezoomviewvalues ( void )
{
	//  accepts gridentityinzoomview
	if (  t.gridentityinzoomview>0 ) 
	{
		t.zoomviewcameraangle_f=0.0;
		t.zoomviewcameraheight_f=50.0;
		t.zoomviewcamerarange_f=75.0;
		if (  t.entityelement[t.gridentityinzoomview].obj>0 ) 
		{
			if (  ObjectExist(t.entityelement[t.gridentityinzoomview].obj) == 1 ) 
			{
				t.zoomviewcamerarange_f=ObjectSize(t.entityelement[t.gridentityinzoomview].obj,1)*2.0;
				t.zoomviewcameraheight_f=(ObjectSize(t.entityelement[t.gridentityinzoomview].obj,1)/2.0)-100.0;
				if (  t.zoomviewcameraheight_f<5  )  t.zoomviewcameraheight_f = 5;

				// ensure camera always faces the front of an entity
				t.zoomviewcameraangle_f = (0-ObjectAngleY(t.entityelement[t.gridentityinzoomview].obj))+180.0f;
			}
		}
	}
}

void gridedit_save_test_map ( void )
{
	//  Save map data locally only (not to FPM)
	timestampactivity(0,"SAVETESTMAP: Save map");
	mapfile_savemap ( );

	if (bKeepWindowsResponding)
		EmptyMessages();

	//  Settings specific to the player
	timestampactivity(0,"SAVETESTMAP: Save player config");
	mapfile_saveplayerconfig ( );

	if (bKeepWindowsResponding)
		EmptyMessages();

	//  Save entity elements
	timestampactivity(0,"SAVETESTMAP: Save elements");
	entity_savebank ( );
	entity_savebank_ebe ( );

	if (bKeepWindowsResponding)
		EmptyMessages();

	//PE: Must remove , t.entityprofile[t.entid].ismarker == 12 && systemwide.
	extern StoryboardStruct Storyboard;
	std::vector <entitytype> StoreEntEle(20);
	int storeindex = 1;
	for (int i = 1; i <= g.entityelementlist; i++)
	{
		int tentid = t.entityelement[i].bankindex;
		if (tentid > 0 && t.entityprofile[tentid].ismarker == 12)
		{
			if (t.entityelement[i].eleprof.aimain_s.Len() > 0)
			{
				if (t.entityelement[i].eleprof.systemwide_lua)
				{
					if (strlen(Storyboard.gamename) > 0)
					{
						if (StoreEntEle.capacity() < storeindex + 1)
							StoreEntEle.reserve(storeindex + 10);
						StoreEntEle[storeindex++] = t.entityelement[i];
						t.entityelement[i].old_bankindex = t.entityelement[i].bankindex;
						//PE: These will be added when loading so mark them for reuse in map.ele
						t.entityelement[i].bankindex = 0; //PE: Save to be reused.
					}
					else
					{
						//PE: If we do not have a storyboard just convert systemwide to normal and save as normal map.ele
						t.entityelement[i].eleprof.systemwide_lua = false;
					}
				}
			}
		}
	}

	entity_saveelementsdata ( false );

	if (bKeepWindowsResponding)
		EmptyMessages();


	//PE: Restore systemwidelua.
	for (int i = 1; i <= g.entityelementlist; i++)
	{
		int tentid = t.entityelement[i].old_bankindex;
		if (tentid > 0 && t.entityprofile[tentid].ismarker == 12)
		{
			if (t.entityelement[i].eleprof.aimain_s.Len() > 0)
			{
				if (t.entityelement[i].eleprof.systemwide_lua)
				{
					t.entityelement[i].bankindex = t.entityelement[i].old_bankindex;
				}
			}
		}
	}

	//PE: Save systemwidelua.ele
	if (strlen(Storyboard.gamename) > 0)
	{
		if (bKeepWindowsResponding)
			EmptyMessages();

		timestampactivity(0, "saving systemwidelua.ele");
		cstr storeoldELEfile = t.elementsfilename_s;
		char collectionELEfilename[MAX_PATH];
		strcpy(collectionELEfilename, "projectbank\\");
		strcat(collectionELEfilename, Storyboard.gamename);
		strcat(collectionELEfilename, "\\systemwidelua.ele");
		GG_GetRealPath(collectionELEfilename, 1);
		if (FileExist(collectionELEfilename) == 1) DeleteFileA(collectionELEfilename);

		if (storeindex > 1)
		{
			t.elementsfilename_s = collectionELEfilename;

			std::vector <entitytype> storeentityelement;
			storeentityelement = t.entityelement;

			int iStoreEntEleCount = g.entityelementlist;
			g.entityelementlist = storeindex;
			t.entityelement = StoreEntEle;
			bool bForCollectionELE = true;
			entity_saveelementsdata(bForCollectionELE);

			if (bKeepWindowsResponding)
				EmptyMessages();

			g.entityelementlist = iStoreEntEleCount;
			t.entityelement = storeentityelement;
			storeentityelement.clear();
			t.elementsfilename_s = storeoldELEfile;
		}
	}

	//  Save waypoints
	timestampactivity(0,"SAVETESTMAP: Save waypoints");
	waypoint_savedata ( );

	if (bKeepWindowsResponding)
		EmptyMessages();

	//  Save editor configuration
	timestampactivity(0,"SAVETESTMAP: Save config");
	editor_savecfg ( );

	if (bKeepWindowsResponding)
		EmptyMessages();

	//  Save terrain
	timestampactivity(0,"SAVETESTMAP: Save terrain textures");
	t.tfileveg_s = g.mysystem.levelBankTestMap_s + "vegmask.png";// dds";
	t.tfilewater_s = g.mysystem.levelBankTestMap_s + "watermask.png";// dds"; 
	terrain_savetextures ( );

	if (bKeepWindowsResponding)
		EmptyMessages();

	t.tfileveggrass_s=g.mysystem.levelBankTestMap_s+"TTR0XR0\\vegmaskgrass.dat";

	if (bKeepWindowsResponding)
		EmptyMessages();

	timestampactivity(0,"SAVETESTMAP: Save terrain height data");
	t.tfile_s=g.mysystem.levelBankTestMap_s+"m.dat";
	terrain_save ( t.tfile_s.Get() );

	if (bKeepWindowsResponding)
		EmptyMessages();


	//  this ensures change flag does not use filemap port 1 (avoid freeze in build game)
	t.lastprojectmodified=0;

	//  Set modification flag
	timestampactivity(0,"SAVETESTMAP: Change modified flag of level");
	g.projectmodified = 0; gridedit_changemodifiedflag ( );
	g.projectmodifiedstatic = 0; 
	timestampactivity(0,"SAVETESTMAP: Complete");
}

void gridedit_save_map ( void )
{
	// seems save can cause IMGUI to crash out when rendering a texture that no longer exists
	extern bool bBlockImGuiUntilNewFrame;
	bBlockImGuiUntilNewFrame = true;
	
	extern bool g_bNoSwapchainPresent;
	iBlockRenderingForFrames = 5;
	g_bNoSwapchainPresent = true;

	// Proper saving message to user
	if (  t.recoverdonotuseany3dreferences == 0 ) 
	{
		editor_hideall3d ( );
	}

	// Save only to TESTMAP area (for map testing)
	gridedit_save_test_map ( );

	if (bKeepWindowsResponding)
		EmptyMessages();

	// Now store all part-files into main FPM project
	mapfile_saveproject_fpm ( );

	if (bKeepWindowsResponding)
		EmptyMessages();

	// Add Latest project To Recent List
	gridedit_updateprojectname ( );

	if (bKeepWindowsResponding)
		EmptyMessages();

	// Clear status Text (  )
	//t.statusbar_s="" ; popup_text_close();

	extern std::vector<int> g_smartObjectDummyEntities;
	for (int i = 0; i < t.entityelement.size(); i++)
	{
		if (t.entityelement[i].iIsSmarkobjectDummyObj == 1)
		{
			g_smartObjectDummyEntities.push_back(i);
		}
	}

	// refresh as SAVE can remove entities and segments
	if ( t.entityorsegmententrieschanged == 1 ) 
	{
		// 111115 - and if not exiting GG
		if ( g.savenoreloadflag == 0 )
		{
			gridedit_load_map ( );
		}
		t.entityorsegmententrieschanged=0;
	}
}

void gridedit_updatemapbeforeedit ( void )
{
	//  Completely reset filemap (and interface parts ie library)
	editor_filemapinit ( );

	//  Newly loaded map starts at layer X
	t.gridlayer=5 ; t.refreshgrideditcursor=1;
}

void gridedit_clear_settings ( void )
{
	//  Default settings
	gridedit_clear_configsettings ( );
	t.gridscale_f=((800/2)/8)/t.gridzoom_f;
	t.currentprojectfilename_s="";
	t.gridground=0;
	t.gridselection=1;
	t.bufferlayer=-1;
	g.gridlayershowsingle=0;
	t.grideditartwidth=1;
	t.grideditartwidthx=1;
	t.grideditartwidthy=1;
	t.locallibrarysegidmaster=0;
	t.locallibraryentidmaster=0;
}

void gridedit_clear_configsettings ( void )
{
	// defaults
	t.borderx_f = 1024.0*50.0; t.cx_f = GGORIGIN_X;
	t.bordery_f = 1024.0*50.0; t.cy_f = GGORIGIN_Z;

	//  Default zoom
	t.gridzoom_f=3.0 ; t.clipheight_f=655;

	//  default grideditselect
	//  0=terrain mode
	//  4=zoom mode
	//  5=entity editing
	//  6=waypoint mode
	t.grideditselect=0;
}

void gridedit_clear_map ( void )
{
	//Stop delete any particle effects.
	gpup_deleteAllEffects();

	//  Delete any old entity objects
	gridedit_deletelevelobjects ( );

	//  Delete any old weapon objects (and reload original data in case last level edited them)
	gun_releaseresources ( );
	gun_scaninall_dataonly ( );

	//  Remove any shader lighting
	lighting_free ( );

	//  delete any conkit objects
	///conkit_saveload_clear ( );

	//  Ensure whether New or Load, physics tweakables for player are reset
	physics_inittweakables ( );

	// new and loaded levels need a refreshed gunlist
	extern bool g_bGunListNeedsRefreshing;
	g_bGunListNeedsRefreshing = true;

	//  Set modification flag
	g.projectmodified = 0 ; gridedit_changemodifiedflag ( );
	g.projectmodifiedstatic = 0;

	//  ensure no leftovers from last edit session
	t.tlasttentitytoselect=-1;

	//  Must generate super texture when do test level for this new level map
	t.terrain.generatedsupertexture=0;

	//  reset free flight mode
	t.editorfreeflight.mode=0 ; t.updatezoom=1;
	t.editorfreeflight.sused=0;
	t.gridentityhidemarkers=0;
	t.cameraviewmode=0;
}

void gridedit_resetmemortracker ( void )
{
	// 121115 - good place to reset memory tracking
	int iMemoryLostFromActivitySoFar = g.gamememactuallyusedstart - SMEMAvailable(1);
	g.gamememactualmaxrightnow = g.gamememactualmaxrightnow - iMemoryLostFromActivitySoFar;
	g.gamememactuallyusedstart = SMEMAvailable(1);
}

void gridedit_emptyallcustomfiles ( void )
{
	ChecklistForFiles();
	for ( t.c = 1 ; t.c <= ChecklistQuantity(); t.c++ )
	{
		t.tfile_s = ChecklistString(t.c);
		if ( t.tfile_s != "." && t.tfile_s != ".." ) 
		{
			// only if a CUSTOM file - needs clearing when new level created
			if ( strnicmp ( t.tfile_s.Get(), "CUSTOM_", 7 ) == NULL )
			{
				DeleteAFile ( t.tfile_s.Get() );
			}
		}
	}
}
void gridedit_emptyrogueoldfiles(void)
{
	ChecklistForFiles();
	for (t.c = 1; t.c <= ChecklistQuantity(); t.c++)
	{
		t.tfile_s = ChecklistString(t.c);
		if (t.tfile_s != "." && t.tfile_s != "..")
		{
			if (strlen(t.tfile_s.Get()) > 4)
			{
				if (strnicmp (t.tfile_s.Get() + strlen(t.tfile_s.Get()) - 4, ".ele", 4) == NULL)
				{
					timestampactivity(0, t.tfile_s.Get());
					DeleteAFile (t.tfile_s.Get());
				}
			}
		}
	}
}

void gridedit_emptyallterrainobjfiles (void)
{
	// Delete all terrainobj files so fresh caches can be created
	ChecklistForFiles();
	for (t.c = 1; t.c <= ChecklistQuantity(); t.c++)
	{
		t.tfile_s = ChecklistString(t.c);
		if (t.tfile_s != "." && t.tfile_s != "..")
		{
			// only if a CUSTOM file - needs clearing when new level created
			if (strnicmp (t.tfile_s.Get(), "terrainobj", 10) == NULL)
			{
				DeleteAFile (t.tfile_s.Get());
			}
		}
	}
}

// force new level camera into free flight mode
void gridedit_resetcameraanddynamicsky(void)
{
	// set camera to free flight on specific angle and starting defaults
	g_bResetCameraToFreeFlightOnNewLevel = true;
	t.editorfreeflight.sused = 0;
}

void gridedit_new_map(void)
{
	// seems new 'may' cause IMGUI to crash out when rendering a texture that no longer exists
	extern bool bBlockImGuiUntilNewFrame;
	bBlockImGuiUntilNewFrame = true;
	ClearAllGroupLists();
	t.widget.pickedEntityIndex = 0;
	t.gridentity = 0;
	
	//PE: These need to be reset.
	t.gridentityinzoomview = 0;
	t.tforceentityfindfloor = 0;

	// ensure tab mode vars reset (no carry from previous session)
	g.tabmode = 0; //TABTAB mode
	g.tabmodeshowfps = 0; //F11 mode
	g.tabmodehidehuds = 0;
	g.mouseishidden = 0;
	t.terrain.terrainpaintermode = 1;

	// reset weather display flag
	bEnableWeather = false;

	//  Start time profiling
	timestampactivity(0, "NEWMAP: Starting new map");

	//  No project - new map
	g.projectfilename_s = "";
	g.projectmodified = 0; t.lastprojectmodified = 0;
	g.projectmodifiedstatic = 0;
	gridedit_updateprojectname();

	// hide EBE if starting new map
	ebe_hide();
	ebe_hardreset();

	// hide terrain texture panel
	terrain_paintselector_hide();

	//if ( gbWelcomeSystemActive == false ) 
	Sync();

	//  Reset visual settings for new map
	visuals_newlevel();

	// Reset all water settings.
	visuals_water_reset();

	//  Ensure default terrain and veg graphics
	terrain_changestyle();
	g.vegstyleindex = t.visuals.vegetationindex;

	//  Load map data
	editor_hideall3d();

	//  Clear all settings
	timestampactivity(0, "NEWMAP: _gridedit_clear_settings");
	gridedit_clear_settings();

	//  Empty the lightmap folder
	timestampactivity(0, "NEWMAP: mapfile_emptylightmapandttsfilesfolder_wicked");
	void mapfile_emptylightmapandttsfilesfolder_wicked(void);
	mapfile_emptylightmapandttsfilesfolder_wicked();

	// Reset texture/profile in EBE folder
	ebe_restoreebedefaulttextures();

	// Empty EBEs from testmap folder
	cstr pStoreOld = GetDir(); 
	if ( PathExist ( g.mysystem.levelBankTestMap_s.Get() ) == 0 )
	{
		// somehow levelbank\testmap folder gone (can be deleted sometimes)
		SetDir ( cstr(g.fpscrootdir_s + "\\Files\\").Get() );
		if ( PathExist ( "levelbank" ) == 0 )
		{
			MakeDirectory ( "levelbank" );
			SetDir ( "levelbank" );
		}
		if ( PathExist ( "testmap" ) == 0 )
		{
			MakeDirectory ( "testmap" );
			SetDir ( "testmap" );
		}
	}
	else
		SetDir ( g.mysystem.levelBankTestMap_s.Get() );

	// Delete any EBE files for new levels
	timestampactivity(0,"NEWMAP: mapfile_emptyebesfromtestmapfolder");
	mapfile_emptyebesfromtestmapfolder(false);

	// Delete any CUSTOM files for new levels, otherwise messes up new asset addition work
	timestampactivity(0,"NEWMAP: gridedit_emptyallcustomfiles");
	gridedit_emptyallcustomfiles ( );

	// Delete any MAP.ELE old file as this might introduce old entities such as weapons!
	timestampactivity(0, "NEWMAP: gridedit_emptyrogueoldfiles");
	gridedit_emptyrogueoldfiles ();

	// empty all terrain obj files if any
	gridedit_emptyallterrainobjfiles();

	// restore folder to default 
	SetDir ( pStoreOld.Get() );

	// Empty terraintexture files from testmap folder
	SetDir ( g.mysystem.levelBankTestMap_s.Get() );
	if ( FileExist ( "superpalette.ter" ) == 1 ) DeleteFileA ( "superpalette.ter" );
	if ( FileExist ( "Texture_D.dds" ) == 1 ) DeleteFileA ( "Texture_D.dds" );
	if ( FileExist ( "Texture_D.jpg" ) == 1 ) DeleteFileA ( "Texture_D.jpg" );
	if ( FileExist ( "Texture_N.dds" ) == 1 ) DeleteFileA ( "Texture_N.dds" );
	if ( FileExist ( "Texture_N.jpg" ) == 1 ) DeleteFileA ( "Texture_N.jpg" );
	if ( FileExist ( "globalenvmap.dds" ) == 1 ) DeleteFileA ( "globalenvmap.dds" );
	SetDir ( pStoreOld.Get() );

	// ensures new terrain in new map is loaded into terrain texture panel when shown
	terrain_resetfornewlevel();

	//  Clear map first
	timestampactivity(0,"NEWMAP: _gridedit_clear_map");
	gridedit_clear_map ( );

	// 121115 - Reset memory tracker
	gridedit_resetmemortracker ( );

	//  Delete all assets of map work
	timestampactivity(0,"NEWMAP: _waypoint_deleteall");
	waypoint_deleteall ( );
	mapfile_newmap ( );
	// Cleanup any visual logic connection objects.
	void deleterelationobjects();
	deleterelationobjects();

	//  Update remaining map data before editing
	timestampactivity(0,"NEWMAP: _gridedit_updatemapbeforeedit");
	gridedit_updatemapbeforeedit ( );

	//  Some default setup for new scene (load markers)
	timestampactivity(0,"NEWMAP: _editor_filemapdefaultinitfornew");
	editor_filemapdefaultinitfornew ( );

	//  Recreate terrain to remove links to old LOD1 objects
	timestampactivity(0,"NEWMAP: _terrain_createactualterrain");
	terrain_createactualterrain ( );

	//  Randomise/Flatten terrain when NEW level created
	if (  t.inputsys.donewflat == 1 ) 
	{
		timestampactivity(0,"NEWMAP: Save newly flattened terrain");
		terrain_flattenterrain ( );
	}
	else
	{
		timestampactivity(0,"NEWMAP: Save newly randomised terrain");
		terrain_randomiseterrain ( );
	}
	t.tfile_s=g.mysystem.levelBankTestMap_s+"m.dat";
	terrain_save ( t.tfile_s.Get() );

	timestampactivity(0,"NEWMAP: Save terrain data");
	t.tfileveg_s = g.mysystem.levelBankTestMap_s + "vegmask.png";// dds";
	t.tfilewater_s = g.mysystem.levelBankTestMap_s + "watermask.png";// dds";
	t.tgeneratefreshwatermaskflag=1;
	terrain_generatevegandmaskfromterrain ( );
	timestampactivity(0,"NEWMAP: Save terrain mask data");
	t.tfileveggrass_s=g.mysystem.levelBankTestMap_s+"TTR0XR0\\vegmaskgrass.dat";

	timestampactivity(0,"NEWMAP: Finish t.terrain generation");
	
	//  Set standard start height for camera
	t.gridzoom_f=3.0 ; t.clipheight_f=655 ; t.updatezoom=1;

	//  Reset cursor
	t. grideditselect = 0 ; editor_refresheditmarkers ( );

	//  Clear widget status
	t.widget.pickedObject=0 ; widget_updatewidgetobject ( );

	//  Reset UNDO/REDO buffer
	t.entityundo.action=0;
	t.entityundo.entityindex=0;
	t.entityundo.bankindex=0;
	t.entityundo.undoperformed=0;
	t.terrainundo.bufferfilled=0;
	t.terrainundo.mode=0;

	gridedit_resetcameraanddynamicsky();
	bForceRefreshLightCount = true;

	//  Finished new map
	timestampactivity(0,"NEWMAP: Finish creating new map");
}

void gridedit_new_map_quick(void)
{
	ClearAllGroupLists();
	t.widget.pickedEntityIndex = 0;
	t.gridentity = 0;
	// ensure tab mode vars reset (no carry from previous session)
	g.tabmode = 0; //TABTAB mode
	g.tabmodeshowfps = 0; //F11 mode
	g.tabmodehidehuds = 0;
	g.mouseishidden = 0;
	t.terrain.terrainpaintermode = 1;

	//LB: These need to be reset also
	t.gridentityinzoomview = 0;
	t.tforceentityfindfloor = 0;

	//  Start time profiling
	timestampactivity(0, "NEWMAP: Starting new map");

	//  No project - new map
	g.projectfilename_s = "";
	g.projectmodified = 0; t.lastprojectmodified = 0;
	g.projectmodifiedstatic = 0;
	gridedit_updateprojectname();

	// hide EBE if starting new map
	ebe_hide();
	ebe_hardreset();

	// hide terrain texture panel
	terrain_paintselector_hide();

	//  Reset visual settings for new map
	visuals_newlevel();

	// Reset all water settings.
	visuals_water_reset();

	//  Reset visual settings for new map
	t.visuals.refreshshaders = 1;
	
	//  Load map data
	editor_hideall3d();

	//  Clear all settings
	timestampactivity(0, "NEWMAP: _gridedit_clear_settings");
	gridedit_clear_settings();

	// Empty EBEs from testmap folder
	cstr pStoreOld = GetDir();
	if (PathExist(g.mysystem.levelBankTestMap_s.Get()) == 0)
	{
		// somehow levelbank\testmap folder gone (can be deleted sometimes)
		SetDir(cstr(g.fpscrootdir_s + "\\Files\\").Get());
		if (PathExist("levelbank") == 0)
		{
			MakeDirectory("levelbank");
			SetDir("levelbank");
		}
		if (PathExist("testmap") == 0)
		{
			MakeDirectory("testmap");
			SetDir("testmap");
		}
	}
	else {
		SetDir(g.mysystem.levelBankTestMap_s.Get());
		//  Empty the lightmap folder
		timestampactivity(0, "NEWMAP: cleantestmapfolder");
		if (FileExist("superpalette.ter") == 1) DeleteFileA("superpalette.ter");
		if (FileExist("Texture_D.dds") == 1) DeleteFileA("Texture_D.dds");
		if (FileExist("Texture_D.jpg") == 1) DeleteFileA("Texture_D.jpg");
		if (FileExist("Texture_N.dds") == 1) DeleteFileA("Texture_N.dds");
		if (FileExist("Texture_N.jpg") == 1) DeleteFileA("Texture_N.jpg");
		if (FileExist("globalenvmap.dds") == 1) DeleteFileA("globalenvmap.dds");
		//  Ensure no old OBS file and OBS triggers to generate
		if (t.tignoreinvalidateobstacles == 0) {
			if (FileExist("map.obs") == 1) DeleteFileA("map.obs");
			t.aisystem.generateobs = 1;
		}

		void mapfile_emptylightmapandttsfilesfolder_wicked(void);
		mapfile_emptylightmapandttsfilesfolder_wicked();

		// Delete any EBE files for new levels
		mapfile_emptyebesfromtestmapfolder(false);
		gridedit_emptyallcustomfiles();
		gridedit_emptyallterrainobjfiles();
	}

	// restore folder to default 
	SetDir(pStoreOld.Get());

	// ensures new terrain in new map is loaded into terrain texture panel when shown
	terrain_resetfornewlevel();

	//  Clear map first
	t.tlasttentitytoselect = -1;
	g.projectmodified = 0; gridedit_changemodifiedflag();
	g.projectmodifiedstatic = 0;

	//Stop delete any particle effects.
	gpup_deleteAllEffects();

	lighting_free();
	gridedit_deletelevelobjects();

	// 121115 - Reset memory tracker
	gridedit_resetmemortracker();

	//  Delete all assets of map work
	mapfile_newmap();

	//  Update remaining map data before editing
	if (t.game.gameisexe == 0)
	{
		// for now, it seems the standalone can call this function!!
		editor_clearlibrary();
		g.entidmaster = 0;
		editor_filllibrary();
		editor_leftpanelreset();
		t.gridlayer = 5; t.refreshgrideditcursor = 1;
	}

	//  Recreate terrain to remove links to old LOD1 objects
	timestampactivity(0, "NEWMAP: _terrain_createactualterrain");
	terrain_createactualterrain();

	//  Randomise/Flatten terrain when NEW level created
	if (t.inputsys.donewflat == 1)
	{
		timestampactivity(0, "NEWMAP: Save newly flattened terrain");
		terrain_flattenterrain();
	}
	else
	{
		timestampactivity(0, "NEWMAP: Save newly randomised terrain");
		terrain_randomiseterrain();
	}
	t.tfile_s = g.mysystem.levelBankTestMap_s + "m.dat";
	terrain_save ( t.tfile_s.Get() );
	timestampactivity(0, "NEWMAP: Save terrain data");
	t.tfileveg_s = g.mysystem.levelBankTestMap_s + "vegmask.png";// dds";
	t.tfilewater_s = g.mysystem.levelBankTestMap_s + "watermask.png";// dds";
	t.tgeneratefreshwatermaskflag = 1;
	terrain_generatevegandmaskfromterrain();
	t.tfileveggrass_s = g.mysystem.levelBankTestMap_s + "TTR0XR0\\vegmaskgrass.dat";

	//  Set standard start height for camera
	t.gridzoom_f = 3.0; t.clipheight_f = 655; t.updatezoom = 1;

	//  Reset cursor
	t.grideditselect = 0; editor_refresheditmarkers();

	//  Clear widget status
	t.widget.pickedObject = 0; widget_updatewidgetobject();

	//  Reset UNDO/REDO buffer
	t.entityundo.action = 0;
	t.entityundo.entityindex = 0;
	t.entityundo.bankindex = 0;
	t.entityundo.undoperformed = 0;
	t.terrainundo.bufferfilled = 0;
	t.terrainundo.mode = 0;

	gridedit_resetcameraanddynamicsky();
	bForceRefreshLightCount = true;

	//  Finished new map
	timestampactivity(0, "NEWMAP: Finish creating new map");
}

void gridedit_updatestatusbar ( void )
{
}

void gridedit_load_map ( void )
{
	ClearAllGroupLists();
	t.widget.pickedEntityIndex = 0;
	t.gridentity = 0;

	//Stop delete any particle effects.
	gpup_deleteAllEffects();

	//  Load map data
	editor_hideall3d ( );

	//LB: These need to be reset (probably can put these in a common 'new something' area
	t.gridentityinzoomview = 0;
	t.tforceentityfindfloor = 0;

	// hide terrain texture panel
	terrain_paintselector_hide(); Sync();

	// ensure NO old flat area items in list
	timestampactivity(0, "GGTerrain_RemoveAllFlatAreas:1");
	GGTerrain_RemoveAllFlatAreas();

	if (bKeepWindowsResponding)
		EmptyMessages();

	//  Reset visual settings for new map
	if (  t.skipfpmloading == 0 ) 
	{
		// 131115 - prevent visual settings for game get wiped out if restart session
		// where project loaded from levelbank\testlevel and visuals already filled
		visuals_newlevel ( );
	}

	//  Force the zoom to be updated to prevent black screen bug, due to old camera range
	t.updatezoom=1;

	//  Load FPM project into testmap files area
	t.tloadsuccessfully=1;
	if (  t.skipfpmloading == 1 ) 
	{
		//  replace NEW with RELOAD
		OpenFileMap (  1,"FPSEXCHANGE" );
		SetFileMapDWORD (  1, 408, 0 );
		SetEventAndWait (  1 );
	}
	else
	{
		//  this setstloadsuccessfully to zero if failed to load FPM (corrupt zipfile)
		mapfile_loadproject_fpm ( );
	}

	//  Loaded successfully
	if ( t.tloadsuccessfully == 1 ) 
	{
		//  Clear map first
		gridedit_clear_map ( );

		if (bKeepWindowsResponding)
			EmptyMessages();

		// 121115 - Reset memory tracker
		gridedit_resetmemortracker ( );

		//  Determine if FPM is accompanied by .REPLACE file
		t.treplacefilename_s = "" ; t.treplacefilename_s = t.treplacefilename_s + Left(g.projectfilename_s.Get(),Len(g.projectfilename_s.Get())-4)+".replace";
		if (  FileExist(t.treplacefilename_s.Get()) == 1 ) 
		{
			t.editor.replacefilepresent_s=t.treplacefilename_s;
		}
		else
		{
			t.editor.replacefilepresent_s="";
		}

		// as load from level to level, cannot carry over E or ENTID refs from any previous level stoerd in collection item list
		for (int n = 0; n < g_collectionList.size(); n++)
		{
			g_collectionList[n].iEntityID = 0;
			g_collectionList[n].iEntityElementE = 0;
		}

		//  Load entity bank and elements
		popup_text_change(t.strarr_s[611].Get());
		entity_loadbank ( );
		if (bKeepWindowsResponding)
			EmptyMessages();

		timestampactivity(0, "s:entity_loadelementsdata()");
		entity_loadelementsdata ( );

		if (bKeepWindowsResponding)
			EmptyMessages();

		timestampactivity(0, "e:entity_loadelementsdata()");
		t.editor.replacefilepresent_s="";

		//  Load waypoints
		popup_text_change(t.strarr_s[612].Get());
		waypoint_loaddata ( );

		if (bKeepWindowsResponding)
			EmptyMessages();

		waypoint_recreateobjs ( );

		if (bKeepWindowsResponding)
			EmptyMessages();

		//  Load data
		popup_text_change(t.strarr_s[613].Get());
		mapfile_loadmap ( );

		if (bKeepWindowsResponding)
			EmptyMessages();

		//  Load player settings
		timestampactivity(0,"Load player config");
		mapfile_loadplayerconfig ( );

		if (bKeepWindowsResponding)
			EmptyMessages();

		//  Load terrain
		popup_text_change(t.strarr_s[610].Get());
		timestampactivity(0, "Create Terrain");
		terrain_createactualterrain ( );
		terrain_loaddata ( );

		if (bKeepWindowsResponding)
			EmptyMessages();

		// ensure firerate settings updated with any overrides set by developer mode changes
		entity_init_overwritefireratesettings();

		if (bKeepWindowsResponding)
			EmptyMessages();

		//  Update remaining map data before editing
		timestampactivity(0, "Reset Editor.");
		gridedit_updatemapbeforeedit ( );

		if (bKeepWindowsResponding)
			EmptyMessages();

		//  Load editor configuration
		int iOldGE = t.grideditselect;
		editor_loadcfg ( true );

		extern bool g_bNeedToConvertClassicPositionsToMAX;
		if (g_bNeedToConvertClassicPositionsToMAX == true)
		{
			// new terrain system is located at 0,0,0 (not 25600x600x25600), so shift to new location
			GGVECTOR3 vToMAXShift = GGVECTOR3(25600, 600, 25600);
			if (g.entityelementlist > 0)
			{
				// shift all entity elements to new positions
				for (int e = 1; e <= g.entityelementlist; e++)
				{
					t.entityelement[e].x -= vToMAXShift.x;
					t.entityelement[e].y -= vToMAXShift.y;
					t.entityelement[e].z -= vToMAXShift.z;
				}
			}

			// LB: also shift camera position to match (if seemingly the old coordinate system) [need a better way to detect 'classic' levels and 'old MAX' levels!!
			if (fabs(t.cx_f-25600)<1000.0f && fabs(t.cy_f - 25600) < 1000.0f)
			{
				t.cx_f -= vToMAXShift.x;
				t.cy_f -= vToMAXShift.z;
			}
		}

		//In wicked keep current window open, terrain , entity...
		t.grideditselect = iOldGE;
		//  Load segments/prefab/entities into window
		OpenFileMap (  1,"FPSEXCHANGE" );
		editor_filllibrary ( );

		if (bKeepWindowsResponding)
			EmptyMessages();

		//  Add Latest project To Recent List
		gridedit_updateprojectname ( );
	}
	else
	{
		//  FPM could not be extracted (likely a corrupt zipfile)
		if (  t.tloadsuccessfully == 0 ) 
		{
			t.strwork = ""; t.strwork = t.strwork + t.strarr_s[614]+" : "+Right(g.projectfilename_s.Get(),Len(g.projectfilename_s.Get())-Len(g.fpscrootdir_s.Get()));
			popup_text_change( t.strwork.Get() );
		}
		if (  t.tloadsuccessfully == 2 ) 
		{
			popup_text_change("The FPM was not created with Game Guru");
		}
		SleepNow (  2000 );

		//  Create blank in this case
		t.inputsys.donewflat=1;
		gridedit_new_map ( );
	}

	if (bKeepWindowsResponding)
		EmptyMessages();

	//  Popup warning if load found some missing files
	if ( g.timestampactivityflagged == 1 ) 
	{
		//  message prompt
		g.timestampactivityflagged=0;

		//  copy time stamp log to map bank log
		if (  ArrayCount(t.missingmedia_s) >= 0 ) 
		{
			t.tmblogfile_s = "" ; t.tmblogfile_s=t.tmblogfile_s + Left(g.projectfilename_s.Get(),Len(g.projectfilename_s.Get())-4)+".log";
			if (  FileExist(t.tmblogfile_s.Get()) == 1  )  DeleteAFile (  t.tmblogfile_s.Get() );
			if (  Len(t.tmblogfile_s.Get())>4 ) 
			{
				t.missingmedia_s[0]="MISSING MEDIA:";
				for ( t.m = 1 ; t.m <= ArrayCount(t.missingmedia_s); t.m++ )
				{
					if (  Len(t.missingmedia_s[t.m].Get())>2 ) 
					{
						t.missingmedia_s[t.m]=t.missingmedia_s[t.m]+"=replace"+t.missingmedia_s[t.m];
					}
				}
				SaveArray (  t.tmblogfile_s.Get() ,t.missingmedia_s );
			}
		}
	}

	// a new global folder has been introduced, and some script files moved there
	// so need to ensure older levels using the old location are redirected on load
	bool bFindAnyMissingScriptsThatMayHaveMovedToGlobalFolder = true;
	if (bFindAnyMissingScriptsThatMayHaveMovedToGlobalFolder == true)
	{
		bool bReplacedScript = false;
		if (g.entityelementlist > 0)
		{
			for (int e = 1; e <= g.entityelementlist; e++)
			{
				LPSTR pOriginal = t.entityelement[e].eleprof.aimain_s.Get();
				LPSTR pFileOnly = NULL;
				for (int n = strlen(pOriginal); n > 0; n--)
				{
					if (pOriginal[n] == '\\' || pOriginal[n] == '/')
					{
						pFileOnly = pOriginal + n + 1;
						break;
					}
				}
				if (pFileOnly)
				{
					char pTryInGlobal[MAX_PATH];
					strcpy(pTryInGlobal, "global\\");
					strcat(pTryInGlobal, pFileOnly);
					char pTryInGlobalAbs[MAX_PATH];
					strcpy(pTryInGlobalAbs, "scriptbank\\");
					strcat(pTryInGlobalAbs, pTryInGlobal);
					GG_GetRealPath(pTryInGlobalAbs, false);
					if (FileExist(pTryInGlobalAbs) == 1)
					{
						// we found a script reference that exists in the global folder
						// so we use the global folder version!
						t.entityelement[e].eleprof.aimain_s = pTryInGlobal;
						bReplacedScript = true;
					}
				}
			}
		}
		if (bReplacedScript == true)
		{
			strcpy(cTriggerMessage, "Some behavior(s) have been moved to the new global category");
			iTriggerMessageDelay = 10;
			bTriggerMessage = true;
			iMessageTimer = 0;
		}
	}
	
	bool bAutoCleanUpOldCommunityCoreReferencesBackToStockLatest = true;
	if (bAutoCleanUpOldCommunityCoreReferencesBackToStockLatest == true)
	{
		bool bReplacedAnyScript = false;
		if (g.entityelementlist > 0)
		{
			for (int e = 1; e <= g.entityelementlist; e++)
			{
				char pCurrentScript[MAX_PATH];
				strcpy(pCurrentScript, t.entityelement[e].eleprof.aimain_s.Get());
				strlwr(pCurrentScript);
				LPSTR pPatternToMatch = "community\\6704278\\core\\";
				if ( strstr(pCurrentScript, pPatternToMatch) != NULL )
				{
					strcpy(pCurrentScript, t.entityelement[e].eleprof.aimain_s.Get() + strlen(pPatternToMatch));
					t.entityelement[e].eleprof.aimain_s = pCurrentScript;
					bReplacedAnyScript = true;
				}
			}
		}
		if (bReplacedAnyScript == true)
		{
			strcpy(cTriggerMessage, "Some core behaviors have been updated to the latest version");
			iTriggerMessageDelay = 10;
			bTriggerMessage = true;
			iMessageTimer = 0;
		}
	}

	// free usages
	if ( ArrayCount(t.missingmedia_s) >= 0 ) 
	{
		UnDim (  t.missingmedia_s );
	}
	g.missingmediacounter=0;

	//  Quick update of cursor
	t.lastgrideditselect=-1 ; editor_refresheditmarkers ( );

	//  Recreate all entities in level
	char debug[MAX_PATH];
	sprintf(debug, "Setup objects: %ld", g.entityelementlist);
	timestampactivity(0, debug);
	extern bool bNoHierarchySorting;
	bNoHierarchySorting = true;
	extern int iInstancedTotal;
	iInstancedTotal = 0;

	if (bKeepWindowsResponding)
		EmptyMessages();

	for ( t.e = 1 ; t.e <=  g.entityelementlist; t.e++ )
	{
		t.tupdatee=t.e ; gridedit_updateentityobj ( );
		if (t.e % 20 == 0)
		{
			if (bKeepWindowsResponding)
				EmptyMessages();
		}
	}
	timestampactivity(0, "End Setup objects:");
	
	sprintf(debug, "Instanced objects: %ld", iInstancedTotal);
	timestampactivity(0, debug);
	bNoHierarchySorting = false;
	lighting_refresh ( );

	//  Ensure newly updated entity does not trigger a terrain update!
	t.terrain.terrainpainteroneshot=0;

	//  Ensure visual indices for sky, terrain and veg up to date (for when we use test game)
	visuals_updateskyterrainvegindex ( );

	//  Refresh any 'shaders' that associat with new entities loaded in
	visuals_justshaderupdate ( );

	//  Ensure editor zoom refreshes
	t.updatezoom=1;

	// 161115 - in any event, ensure we generate super texture for 'distant' terrain texture 
	t.visuals.refreshterrainsupertexture = 2;

	//LB: clean any corrupt references out of editor locked list
	for (int i = 0; i < vEntityLockedList.size(); i++)
	{
		int e = vEntityLockedList[i].e;
		if (e < 0 || e >= t.entityelement.size())
		{
			// remove this entry
			vEntityLockedList.erase(vEntityLockedList.begin() + i);
			i--; // adjust index after removal
			continue;
		}
	}

	//PE: Restore locked state. from locked.cfg
	for (int i = 0; i < vEntityLockedList.size(); i++)
	{
		int e = vEntityLockedList[i].e;
		if(e < t.entityelement.size())
			t.entityelement[e].editorlock = 1;
	}
	bForceRefreshLightCount = true;

	// Level has finished loading, so no longer need to store the smart object dummy OBJs
	extern std::vector<int> g_smartObjectDummyEntities;
	g_smartObjectDummyEntities.clear();

	if (bKeepWindowsResponding)
		EmptyMessages();

	// call files modify check function and reset file timestamp map
	extern void CheckExistingFilesModified(bool);
	CheckExistingFilesModified(true);
}

void gridedit_changemodifiedflag ( void )
{
	// project flag changed, update window Text (  )
	if ( t.game.gameisexe == 0 ) 
	{
		if ( t.lastprojectmodified != g.projectmodified ) 
		{
			t.lastprojectmodified=g.projectmodified;
			gridedit_updateprojectname ( );
		}
		if ( g.projectmodified == 1 && g.projectmodifiedstatic == 1 ) 
		{
			// trigger actions if any modification made
			g.projectmodifiedstatic = 0;
		}
	}
}

void gridedit_updateprojectname ( void )
{
	OpenFileMap (  1,"FPSEXCHANGE" );

	//  add to project title
	if ( strcmp ( Lower(Left(g.projectfilename_s.Get(),Len(g.rootdir_s.Get()))) , Lower(g.rootdir_s.Get()) ) == 0 ) 
	{
		t.tprojname_s=Right(g.projectfilename_s.Get(),Len(g.projectfilename_s.Get())-Len(g.rootdir_s.Get()));
	}
	else
	{
		t.tprojname_s=g.projectfilename_s;
	}
	if (  g.projectmodified != 0  )  t.tprojname_s = t.tprojname_s+"*";

	// 011215 - Add which mode you are in
	int iEditingMode = 0;
	if ( t.grideditselect==0 ) iEditingMode = 1; // terrain
	if ( t.grideditselect==5 && t.gridentitymarkersmodeonly==0 ) iEditingMode = 2; // entity
	if ( t.grideditselect==5 && t.gridentitymarkersmodeonly==1 ) iEditingMode = 3; // markers
	if ( t.grideditselect==6 ) iEditingMode = 4; // waypoints
	switch ( iEditingMode )
	{
		case 1 : t.tprojname_s = t.tprojname_s + cstr("] - [Terrain Editing Mode"); break;
		case 2 : t.tprojname_s = t.tprojname_s + cstr("] - [Entity Editing Mode"); break;
		case 3 : t.tprojname_s = t.tprojname_s + cstr("] - [Marker Only Editing Mode"); break;
		case 4 : t.tprojname_s = t.tprojname_s + cstr("] - [Waypoint Editing Mode"); break;
	}

	// send window title text to IDE
	OpenFileMap(1, "FPSEXCHANGE");
	SetFileMapString (  1, 1000, t.tprojname_s.Get() );
	SetFileMapDWORD (  1, 416, 1 );
	SetEventAndWait (  1 );
	while (  GetFileMapDWORD(1, 416) == 1 ) 
	{
		SetEventAndWait (  1 );
	}
	//  add to recent files list
	if (  g.projectfilename_s != "" ) 
	{
		// 091215 - if folder exists
		if ( PathExist(g.projectfilename_s.Get()) == 1 )
		{
			SetFileMapString(1, 1000, g.projectfilename_s.Get());
			SetFileMapDWORD(1, 438, 1);
			SetEventAndWait(1);
			while (GetFileMapDWORD(1, 438) == 1)
			{
				SetEventAndWait(1);
			}
		}
	}
}

void gridedit_import_ask ( void )
{
	OpenFileMap (  1, "FPSEXCHANGE" );
	SetEventAndWait (  1 );
	do
	{
		t.inputsys.kscancode=GetFileMapDWORD( 1, 100 );

		//PE: Virtual keys should not be included , as if you press a cancel (in IDE) afer that import , import will not open.
	} while ( (  t.inputsys.kscancode > 3 ) );

	// if not already loaded
	if (  t.importer.loaded == 0 ) 
	{
		OpenFileMap (  1,"FPSEXCHANGE" );
		if ( strlen ( t.timporterpath_s.Get() ) == 0 )
		{
			t.strwork = ""; t.strwork = t.strwork + g.rootdir_s+"entitybank\\";
		}
		else
		{
			t.strwork = t.timporterpath_s + "\\";
		}
		SetFileMapString (  1, 1000, t.strwork.Get() );
		t.tdone = 0;
		while (  t.tdone  !=  2 ) 
		{
			if (  t.tdone  ==  0 ) 
			{
				SetFileMapString ( 1 , 1256 , "Choose an X or FBX file for a new object or an .fpe file for existing (*.*)" );
				SetFileMapString (  1, 1512, "Import New Entity" );
			}
			else
			{
				SetFileMapString (  1, 1256, "Please try again ) You must choose either an X or FBX file or an .fpe file! (*.*)" );
				SetFileMapString (  1, 1512, "Invalid File, Please t.try again" );
			}
			SetFileMapDWORD (  1, 424, 1 );
			SetEventAndWait (  1 );
			while (  GetFileMapDWORD(1, 424) == 1 ) 
			{
				SetEventAndWait (  1 );
			}
			t.returnstring_s=GetFileMapString(1, 1000);
			t.tdone = 1;
			if (	strcmp ( Lower(Right(t.returnstring_s.Get(),2)) , ".x" ) == 0 
			||		strcmp ( Lower(Right(t.returnstring_s.Get(),4)) ,".fbx" ) == 0 
			||		strcmp ( Lower(Right(t.returnstring_s.Get(),4)) , ".dbo" ) == 0 
			||		strcmp ( Lower(Right(t.returnstring_s.Get(),4)) , ".fpe" ) == 0 
			||		t.returnstring_s  ==  ""  )  
			{
				t.tdone  =  2;
			}
		}

		// refresh 3d view so dialog Box (  (  not left black Box ) )
		for ( t.tsync = 1 ; t.tsync <=  5 ; t.tsync++ ) { Sync ( ); SleepNow ( 10 );  }

		// if successfully selected a good file extension
		if (  t.returnstring_s != "" ) 
		{
			// load the model
			t.timporterfile_s = t.returnstring_s;
			importer_loadmodel ( );

			// and remember folder we arrived at, so can restore next time we use importer
			LPSTR pReturnedFile = t.timporterfile_s.Get();
			for ( int n = strlen(pReturnedFile); n>0; n-- )
			{
				if ( pReturnedFile[n] == '\\' || pReturnedFile[n] == '/' )
				{
					t.timporterpath_s = t.timporterfile_s;
					LPSTR pImporterPath = t.timporterpath_s.Get();
					pImporterPath[n] = 0;
					break;
				}
			}
		}
	}
}

void gridedit_intercept_savefirst ( void )
{
	t.editorcanceltask=0;
	if (  g.projectmodified == 1 ) 
	{
		OpenFileMap (  1,"FPSEXCHANGE" );
		SetFileMapString (  1, 1000, t.strarr_s[369].Get() );
		SetFileMapString (  1, 1256, t.strarr_s[370].Get() );
		SetFileMapDWORD (  1, 900, 2 );
		SetEventAndWait (  1 );
		while (  GetFileMapDWORD(1, 900) != 0 ) 
		{
			SetEventAndWait (  1 );
		}
		t.tokay=GetFileMapDWORD(1, 904);

		//  refresh 3d view so dialog Box (  (  not left black Box ) )
		for ( t.tsync = 1 ; t.tsync <= 5 ; t.tsync++ ) {  Sync (   ); SleepNow (  10  ); }

		if (  t.tokay == 1 ) 
		{
			//  yes save first
			gridedit_save_map_ask ( );
			g.projectmodified=0  ; gridedit_changemodifiedflag ( );
			g.projectmodifiedstatic = 0;
		}
		if (  t.tokay == 2 ) 
		{
			//  task cancelled
			t.editorcanceltask=1;
		}
	}
}

void gridedit_intercept_savefirst_noreload ( void )
{
	g.savenoreloadflag = 1;
	gridedit_intercept_savefirst();
	g.savenoreloadflag = 0;
}

void gridedit_open_map_ask ( void )
{
	//  SAVE CURRENT (IF ANY)
	t.editorcanceltask=0;
	if (  g.projectmodified == 1 ) 
	{
		//  If project modified, ask if want to save first
		gridedit_intercept_savefirst ( );
	}
	if (  t.editorcanceltask == 0 ) 
	{
		//  OPEN FPM
		OpenFileMap (  1,"FPSEXCHANGE" );
		t.strwork = g.mysystem.mapbankAbs_s;		
		SetFileMapString (  1, 1000, t.strwork.Get() );
		SetFileMapString (  1, 1256, t.strarr_s[371].Get() );
		SetFileMapString (  1, 1512, t.strarr_s[372].Get() );
		SetFileMapDWORD (  1, 424, 1 );
		SetEventAndWait (  1 );
		while (  GetFileMapDWORD(1, 424) == 1 ) 
		{
			SetEventAndWait (  1 );
		}
		t.returnstring_s=GetFileMapString(1, 1000);

		//  refresh 3d view so dialog Box (  (  not left black Box ) )
		for ( t.tsync = 1 ; t.tsync <=  5 ; t.tsync++ ) { Sync ( ); SleepNow ( 10 ); }

		if (  t.returnstring_s != "" ) 
		{
			if (  cstr(Lower(Right(t.returnstring_s.Get(),4))) == ".fpm" ) 
			{
				g.projectfilename_s=t.returnstring_s;
				gridedit_load_map ( );
			}
		}
	}
}

void gridedit_new_map_ask ( void )
{
	//  SAVE CURRENT (IF ANY)
	t.editorcanceltask=0;
	if (  g.projectmodified == 1 ) 
	{
		//  If project modified, ask if want to save first
		gridedit_intercept_savefirst ( );
	}

	if (  t.editorcanceltask == 0 ) 
	{
		//  NEW MAP
		gridedit_new_map ( );
	}
}

void gridedit_save_map_ask ( void )
{
	if (  g.projectfilename_s == "" ) 
	{
		gridedit_saveas_map ( );
	}
	else
	{
		gridedit_save_map ( );
	}
}

void gridedit_saveas_map ( void )
{
	//  SAVE AS DIALOG
	OpenFileMap (  1,"FPSEXCHANGE" );
	t.strwork = g.mysystem.mapbankAbs_s;
	SetFileMapString (  1, 1000, t.strwork.Get() );
	SetFileMapString (  1, 1256, t.strarr_s[373].Get() );
	SetFileMapString (  1, 1512, t.strarr_s[374].Get() );
	SetFileMapDWORD (  1, 428, 1 );
	SetEventAndWait (  1 );
	while (  GetFileMapDWORD(1, 428) == 1 ) 
	{
		SetEventAndWait (  1 );
	}
	t.returnstring_s=GetFileMapString(1, 1000);

	//  refresh 3d view so dialog Box (  (  not left black Box ) )
	for ( t.tsync = 1 ; t.tsync <=  5 ; t.tsync++ ) { Sync ( ); SleepNow ( 10 ); }

	if (  t.returnstring_s != "" ) 
	{
		if (  cstr(Lower(Right(t.returnstring_s.Get(),4))) != ".fpm"  )  t.returnstring_s = t.returnstring_s+".fpm";
		g.projectfilename_s=t.returnstring_s;
		gridedit_save_map ( );
	}
}

void gridedit_addentitytomap(void)
{
	extern bool bUpdateObjectList;
	bUpdateObjectList = true;
	// mark as static if it was
	if (t.gridentitystaticmode == 1) g.projectmodifiedstatic = 1;
	entity_addentitytomap();

	//PE: we loose status somewhere, so force it off after adding a entity to map.
	extern bool bCubesVisible;
	if (bCubesVisible == false) bCubesVisible = true; //Force.

	if (g_UndoSysObjectRememberBeforeMove == true)
	{
		// this happens when object deleted from level (addtocursor) but it could be a move event
		// if the object has not moved/rotated/scaled, we can skip adding a move event
		if (g_UndoSysObjectRememberBeforeMovePX == t.entityelement[t.e].x
			&&  g_UndoSysObjectRememberBeforeMovePY == t.entityelement[t.e].y
			&&  g_UndoSysObjectRememberBeforeMovePZ == t.entityelement[t.e].z
			&&  g_UndoSysObjectRememberBeforeMoveRX == t.entityelement[t.e].rx
			&&  g_UndoSysObjectRememberBeforeMoveRY == t.entityelement[t.e].ry
			&&  g_UndoSysObjectRememberBeforeMoveRZ == t.entityelement[t.e].rz
			&&  g_UndoSysObjectRememberBeforeMoveQuatMode == t.entityelement[t.e].quatmode
			&&  g_UndoSysObjectRememberBeforeMoveQuatX == t.entityelement[t.e].quatx
			&&  g_UndoSysObjectRememberBeforeMoveQuatY == t.entityelement[t.e].quaty
			&&  g_UndoSysObjectRememberBeforeMoveQuatZ == t.entityelement[t.e].quatz
			&&  g_UndoSysObjectRememberBeforeMoveQuatW == t.entityelement[t.e].quatw
			&&  g_UndoSysObjectRememberBeforeMoveSX == t.entityelement[t.e].scalex
			&&  g_UndoSysObjectRememberBeforeMoveSY == t.entityelement[t.e].scaley
			&&  g_UndoSysObjectRememberBeforeMoveSZ == t.entityelement[t.e].scalez )
		{
			// object has not moved, rotated or scaled
		}
		else
		{
			// object has moved, create a move event
			if (g.entityrubberbandlist.size() == 0)
			{
				// but only if single object move, as rubberband has its own multi object move events created when move a rubberband group
				undosys_object_changeposrotscl (g_UndoSysObjectRememberBeforeMoveE,
					g_UndoSysObjectRememberBeforeMovePX,
					g_UndoSysObjectRememberBeforeMovePY,
					g_UndoSysObjectRememberBeforeMovePZ,
					g_UndoSysObjectRememberBeforeMoveRX,
					g_UndoSysObjectRememberBeforeMoveRY,
					g_UndoSysObjectRememberBeforeMoveRZ,
					g_UndoSysObjectRememberBeforeMoveQuatMode,
					g_UndoSysObjectRememberBeforeMoveQuatX,
					g_UndoSysObjectRememberBeforeMoveQuatY,
					g_UndoSysObjectRememberBeforeMoveQuatX,
					g_UndoSysObjectRememberBeforeMoveQuatW,
					g_UndoSysObjectRememberBeforeMoveSX,
					g_UndoSysObjectRememberBeforeMoveSY,
					g_UndoSysObjectRememberBeforeMoveSZ);
			}
		}

		// and return undo sys to normal after this special case
		g_UndoSysObjectRememberBeforeMove = false;
	}
	else
	{
		// regular object addition
		entity_createundoaction(eUndoSys_Object_Add, t.e);
	}

	// if entity is a light, has a probe
	int entid = t.entityelement[t.e].bankindex;
	if (entid > 0)
	{
		if (t.entityprofile[entid].ismarker == 2)
		{
			if (t.entityelement[t.e].eleprof.light.fLightHasProbe >= 50.0f)
			{
				g_bLightProbeScaleChanged = true;
			}
		}

		if (t.entityprofile[entid].ismarker == 12)
		{
			t.entityelement[t.e].eleprof.thumb_aimain_s = "";
		}
	}
	// clear any gridentity light if gridentity no longer used
	if (t.gridentitywickedlightindex > 0)
	{
		WickedCall_DeleteLight(t.gridentitywickedlightindex);
		t.gridentitywickedlightindex = 0;
	}
}

void gridedit_deleteentityfrommap ( void )
{
	// can intercept delete if char+start marker
	t.tstoretentitytoselect=t.tentitytoselect;
	if (  t.playercontrol.thirdperson.enabled == 1 ) 
	{
		if (  t.gridentity == 0 || (t.gridentity>0 && t.entityprofile[t.gridentity].ismarker != 1) ) 
		{
			t.tstmrke=t.playercontrol.thirdperson.startmarkere;
			if (  t.tentitytoselect == t.tstmrke ) 
			{
				//  first delete char on start marker, and restore marker
				t.tentitytoselect=t.playercontrol.thirdperson.charactere;
				t.tstmrkobj=t.entityelement[t.tstmrke].obj;
				if (  t.tstmrkobj>0 ) 
				{
					if (  ObjectExist(t.tstmrkobj) == 1 ) 
					{
						DisableObjectZDepth (  t.tstmrkobj );
						DisableObjectZWrite (  t.tstmrkobj );
						DisableObjectZRead (  t.tstmrkobj );
					}
				}
				//  and reset third person settings
				t.playercontrol.thirdperson.enabled=0;
				t.playercontrol.thirdperson.charactere=0;
				t.playercontrol.thirdperson.startmarkere=0;
			}
		}
	}

	// if entity is a light, has a probe
	int entid = t.entityelement[t.tentitytoselect].bankindex;
	if (entid > 0)
	{
		if (t.entityprofile[entid].ismarker == 2)
		{
			if (t.entityelement[t.tentitytoselect].eleprof.light.fLightHasProbe >= 50.0f)
			{
				g_bLightProbeScaleChanged = true;
			}
		}
	}

	//If particle delete the effect.
	if (g_UndoSysObjectIsBeingMoved != true)
	{
		if (entid > 0 && t.entityprofile[entid].ismarker == 10)
		{
			int iParticleEmitter = t.entityelement[t.tentitytoselect].eleprof.newparticle.emitterid;
			if (iParticleEmitter != -1)
			{
				gpup_deleteEffect(iParticleEmitter);
				t.entityelement[t.tentitytoselect].eleprof.newparticle.emitterid = -1;
			}
		}
	}

	// mark as static if it was
	if ( t.entityelement[t.tentitytoselect].staticflag == 1 ) g.projectmodifiedstatic = 1;
	int te = t.tentitytoselect;
	if (g_UndoSysObjectIsBeingMoved == true)
	{
		// its move - so we store the change posrotscl event so we know where the entity came from
		g_UndoSysObjectRememberBeforeMove = true;
		g_UndoSysObjectRememberBeforeMoveE = te;
		g_UndoSysObjectRememberBeforeMovePX = t.entityelement[te].x;
		g_UndoSysObjectRememberBeforeMovePY = t.entityelement[te].y;
		g_UndoSysObjectRememberBeforeMovePZ = t.entityelement[te].z;
		g_UndoSysObjectRememberBeforeMoveRX = t.entityelement[te].rx;
		g_UndoSysObjectRememberBeforeMoveRY = t.entityelement[te].ry;
		g_UndoSysObjectRememberBeforeMoveRZ = t.entityelement[te].rz;
		g_UndoSysObjectRememberBeforeMoveQuatMode = t.entityelement[te].quatmode;
		g_UndoSysObjectRememberBeforeMoveQuatX = t.entityelement[te].quatx;
		g_UndoSysObjectRememberBeforeMoveQuatY = t.entityelement[te].quaty;
		g_UndoSysObjectRememberBeforeMoveQuatZ = t.entityelement[te].quatz;
		g_UndoSysObjectRememberBeforeMoveQuatW = t.entityelement[te].quatw;
		g_UndoSysObjectRememberBeforeMoveSX = t.entityelement[te].scalex;
		g_UndoSysObjectRememberBeforeMoveSY = t.entityelement[te].scaley;
		g_UndoSysObjectRememberBeforeMoveSZ = t.entityelement[te].scalez;
	}
	else
	{
		if (t.entityelement[te].eleprof.trigger.waypointzoneindex > 0)
		{
			// if its a zone, create multiple events - one for middle object and one for the waypoint zone data so it can be restored later.
			undosys_multiplevents_start();
			entity_createundoaction(eUndoSys_Object_Delete, te);
			entity_createundoaction(eUndoSys_Object_DeleteWaypoint, te);
			undosys_multiplevents_finish();
		}
		else
		{
			entity_createundoaction(eUndoSys_Object_Delete, te);
		}
			
		
	}
	entity_deleteentityfrommap ( );

	//  restore tentitytoselect in case switched it
	t.tentitytoselect=t.tstoretentitytoselect;
}

void gridedit_deleteentityrubberbandfrommap ( void )
{
	undosys_multiplevents_start();

	// will delete all entities in rubber band list, and preserve them into undo buffer
	for ( int i = 0; i < (int)g.entityrubberbandlist.size(); i++ )
	{
		t.tentitytoselect = g.entityrubberbandlist[i].e;
		if ( t.tentitytoselect > 0 && t.entityelement[t.tentitytoselect].editorlock == 0)
		{
			DeleteEntityFromLists(t.tentitytoselect);

			if ( t.entityelement[t.tentitytoselect].staticflag == 1 ) g.projectmodifiedstatic = 1;
			gridedit_deleteentityfrommap ( );
			g.entityrubberbandlistundo.push_back ( t.entityundo );
		}
	}
	undosys_multiplevents_finish();
}

void gridedit_moveentityrubberband ( void )
{
	// will move all entities in rubber band list, and preserve them into undo buffer
	undosys_multiplevents_start();
	for ( int i = 0; i < (int)g.entityrubberbandlist.size(); i++ )
	{
		int te = g.entityrubberbandlist[i].e;
		undosys_object_changeposrotscl (te, g.entityrubberbandlist[i].px,
			g.entityrubberbandlist[i].py,
			g.entityrubberbandlist[i].pz,
			g.entityrubberbandlist[i].rx,
			g.entityrubberbandlist[i].ry,
			g.entityrubberbandlist[i].rz,
			g.entityrubberbandlist[i].quatmode,
			g.entityrubberbandlist[i].quatx,
			g.entityrubberbandlist[i].quaty,
			g.entityrubberbandlist[i].quatz,
			g.entityrubberbandlist[i].quatw,
			g.entityrubberbandlist[i].scalex,
			g.entityrubberbandlist[i].scaley,
			g.entityrubberbandlist[i].scalez);
	}
	undosys_multiplevents_finish();

	// also, update rubberband to new entity states, so can move multiple times and undo them
	for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
	{
		int te = g.entityrubberbandlist[i].e;
		if (te > 0 && te < t.entityelement.size())
		{
			g.entityrubberbandlist[i].px = t.entityelement[te].x;
			g.entityrubberbandlist[i].py = t.entityelement[te].y;
			g.entityrubberbandlist[i].pz = t.entityelement[te].z;
			g.entityrubberbandlist[i].rx = t.entityelement[te].rx;
			g.entityrubberbandlist[i].ry = t.entityelement[te].ry;
			g.entityrubberbandlist[i].rz = t.entityelement[te].rz;
			g.entityrubberbandlist[i].quatmode = t.entityelement[te].quatmode;
			g.entityrubberbandlist[i].quatx = t.entityelement[te].quatx;
			g.entityrubberbandlist[i].quaty = t.entityelement[te].quaty;
			g.entityrubberbandlist[i].quatz = t.entityelement[te].quatz;
			g.entityrubberbandlist[i].quatw = t.entityelement[te].quatw;
			g.entityrubberbandlist[i].scalex = t.entityelement[te].scalex;
			g.entityrubberbandlist[i].scaley = t.entityelement[te].scaley;
			g.entityrubberbandlist[i].scalez = t.entityelement[te].scalez;
		}
		else
		{
			// bug somewhere, e was assigned to rubberband but this entity does not exist!!
			g.entityrubberbandlist[i].e = 0;
		}
	}
}

void gridedit_updateentityobj ( void )
{
	//  moved to m-entity
	entity_updateentityobj ( );
}

void gridedit_recreateentitycursor ( void )
{
	int ele_id = 0;
	//  Entity floating selection
	if ( t.gridentityobj>0 ) 
	{
		//  character creator remove glued objects
		if (  t.toldCursorEntidForCharacterCreator > 0 ) 
		{
			if (  t.entityprofile[t.toldCursorEntidForCharacterCreator].ischaractercreator  ==  1 ) 
			{
				t.tccobj = g.charactercreatorrmodelsoffset+((t.toldCursorEntidForCharacterCreator*3)-t.characterkitcontrol.bankOffset);
				if (  ObjectExist(t.tccobj) == 1 ) 
				{
					UnGlueObject (  g.charactercreatorrmodelsoffset+((t.toldCursorEntidForCharacterCreator*3)-t.characterkitcontrol.bankOffset)+1 );
					UnGlueObject (  g.charactercreatorrmodelsoffset+((t.toldCursorEntidForCharacterCreator*3)-t.characterkitcontrol.bankOffset)+2 );
					UnGlueObject (  t.tccobj );
				}
			}
		}
		if ( ObjectExist(t.gridentityobj) == 1  ) DeleteObject (  t.gridentityobj );
		t.gridentityobj=0;
		t.toldCursorEntidForCharacterCreator = 0;
	}
	if (  t.gridentity>0 ) 
	{
		t.obj=g.entityviewcursorobj;
		t.sourceobj=g.entitybankoffset+t.gridentity;
		if (ObjectExist(t.sourceobj) == 1)
		{
			WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_CURSOROBJECT);
			if (t.gridentityextractedindex > 0) 
			{
				if (t.gridentityextractedindex > 0 && t.gridentityextractedindex <= ArrayCount(t.entityelement))
				{
					ele_id = t.gridentityextractedindex;
				}
			}
			if (ele_id == 0 && t.tentitytoselect > 0 && t.tentitytoselect <= ArrayCount(t.entityelement) )
			{
				ele_id = t.tentitytoselect;
			}
			WickedSetEntityId(t.gridentity);
			WickedSetElementId(ele_id);
			t.entid=t.gridentity ; t.entobj=t.obj;
			if ( t.entityprofile[t.entid].ischaracter == 1 || t.entityprofile[t.entid].ismarker != 0 || t.entityprofile[t.entid].animmax>0 ) 
			{
				//  Close allows animation independence
				CloneObject ( t.obj, t.sourceobj );

				//  Character creator head
				if ( t.entityprofile[t.entid].ischaractercreator == 1 ) 
				{
					t.toldCursorEntidForCharacterCreator = t.entid;
					t.tSourcebip01_head=getlimbbyname(t.obj, "Bip01_Head");
					if ( t.tSourcebip01_head > 0 ) 
					{
						t.tccobj = g.charactercreatorrmodelsoffset+((t.entid*3)-t.characterkitcontrol.bankOffset);
						if ( ObjectExist(t.tccobj) == 1 ) 
						{
							t.tBip01_FacialHair=getlimbbyname(t.tccobj, "Bip01_FacialHair");
							if ( t.tBip01_FacialHair > 0  )  GlueObjectToLimbEx (  g.charactercreatorrmodelsoffset+((t.entid*3)-t.characterkitcontrol.bankOffset)+1,t.tccobj,t.tBip01_FacialHair,2 );
							t.Bip01_Headgear=getlimbbyname(t.tccobj, "Bip01_Headgear");
							if ( t.Bip01_Headgear > 0  )  GlueObjectToLimbEx (  g.charactercreatorrmodelsoffset+((t.entid*3)-t.characterkitcontrol.bankOffset)+2,t.tccobj,t.Bip01_Headgear,2 );
							GlueObjectToLimbEx (  t.tccobj,t.obj,t.tSourcebip01_head,2 );
						}
					}
				}
			}
			else
			{
				//  Instance creation cheaper
				InstanceObject (  t.obj,t.sourceobj );
			}
			WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_NORMAL);
			WickedSetEntityId(-1);
			WickedSetElementId(0);
			t.gridentityunderground=0;

			//LB: incorporate overrideanimset into object creation step (during editing/loading/etc)
			if (t.obj > 0)
			{
				LPSTR pOverrideAnimSet = t.entityelement[ele_id].eleprof.overrideanimset_s.Get();
				if (strlen(pOverrideAnimSet) > 1) // "" = default to weapon type, "-" = default to object anim
				{
					// replace actual object animations
					if (FileExist(pOverrideAnimSet) == 1)
					{
						sObject* pObject = GetObjectData(t.obj);
						AppendObject(pOverrideAnimSet, t.obj, 0);
						WickedCall_RefreshObjectAnimations(pObject, pObject->wickedloaderstateptr);
					}
				}
			}

			// other entity attributes
			if ( t.entityprofile[t.entid].ismarker != 0 && t.entityprofile[t.entid].ismarker != 11 ) //Allow cullmode on 11
			{
				// special setup for marker objects
				SetObjectTransparency ( t.obj, 2 );
				SetObjectCull ( t.obj, 1 );
				sObject* pObject = g_ObjectList[t.obj];
				if (pObject)
				{
					WickedCall_TextureObject(pObject, NULL);
				}
			}
			else
			{
				// For Wicked, cull mode controlled per-mesh with parent default as normal
				//PE: Prefer WEMaterial over old cullmode
				bool bUseWEMaterial = false;
				if (t.entityprofile[t.entid].WEMaterial.MaterialActive)
				{
					WickedSetEntityId(t.entid);
					if(ele_id  > 0)
						WickedSetElementId(ele_id);
					else
						WickedSetElementId(0);
					sObject* pObject = g_ObjectList[t.obj];
					if (pObject)
					{
						bUseWEMaterial = true;
						for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
						{
							sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
							if (pMesh)
							{
								// set properties of mesh
								WickedSetMeshNumber(iMeshIndex);
								bool bDoubleSided = WickedDoubleSided();
								if (bDoubleSided)
								{
									pMesh->bCull = false;
									pMesh->iCullMode = 0;
									WickedCall_SetMeshCullmode(pMesh);
								}
								else
								{
									pMesh->iCullMode = 1;
									pMesh->bCull = true;
									WickedCall_SetMeshCullmode(pMesh);
								}
							}
						}
					}
					WickedSetEntityId(-1);
				}

				if (!bUseWEMaterial)
					SetObjectCull(t.obj, 1);
				//  set transparency mode
				if (t.entityprofile[t.entid].islightmarker == 1)
				{
					sObject* pObject = g_ObjectList[t.obj];
					if (pObject)
						WickedCall_SetObjectCastShadows(pObject, false);
					t.entityprofile[t.entid].castshadow = -1;
				}
				if ((ele_id > 0) || (ele_id==0 && t.gridentity>0))
				{
					//PE: Wicked material can overwrite objects settings.
					WickedSetEntityId(t.gridentity);
					WickedSetElementId(ele_id);
					// LB: apply WEMaterial to all meshes of this object, not just the first one
					// LB: Setting object transparency defaults here (so not everything is transparent), but the TextureMesh can then set per-mesh transparency :)
					SetObjectTransparency(t.obj, t.entityelement[ele_id].eleprof.WEMaterial.bTransparency[0]);
					sObject* pObject = g_ObjectList[t.obj];
					for (int iMeshIndex = 0; iMeshIndex < pObject->iMeshCount; iMeshIndex++)
					{
						sMesh* pMesh = pObject->ppMeshList[iMeshIndex];
						if (pMesh)
						{
							// set properties of mesh
							WickedSetMeshNumber(iMeshIndex);

							// sets ALL properties of each mesh from WEMaterial
							WickedCall_TextureMesh(pMesh);

							// and must restore mesh transparency flag
							bool bTransparent = WickedGetTransparent();
							pMesh->bTransparency = bTransparent;
						}
					}
					if (t.obj == 70000) 
					{
						//Update mesh materials.
						sObject* pObject = g_ObjectList[t.obj];
						if (pObject)
						{
							for (int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++)
							{
								WickedSetMeshNumber(iMesh);
								WickedCall_SetMeshMaterial(pObject->ppMeshList[iMesh], false);
							}
						}
					}		
					WickedSetEntityId(-1);
					WickedSetElementId(0);
				}
				else
				{
					if (t.entityprofile[t.entid].transparency >= 0)
					{
						WickedSetEntityId(t.gridentity);
						WickedSetElementId(ele_id);
						SetObjectTransparency(t.obj, t.entityprofile[t.entid].transparency);
						WickedSetEntityId(-1);
						WickedSetElementId(0);
					}
				}
				// 051115 - only if not using limb visibility for hiding decal arrow
				if ( t.entityprofile[t.entid].addhandlelimb==0 )
				{
					//  set LOD attributes for entities
					entity_calculateentityLODdistances ( t.entid, t.obj, 0 );
				}
			}
			if (t.obj > 0 && GetNumberOfFrames(t.obj)>0 )
			{
				SetObjectFrame (  t.obj,0 );
				if (  t.entityprofile[t.entid].animmax>0 && t.entityprofile[t.entid].playanimineditor>0 && t.entityprofile[t.entid].ischaractercreator == 0 ) 
				{
					t.q=t.entityprofile[t.entid].playanimineditor-1;
					LoopObject (  t.obj,t.entityanim[t.entid][t.q].start,t.entityanim[t.entid][t.q].finish );
				}
				else if (t.entityprofile[t.entid].playanimineditor < 0)
				{
					// uses name instead of index, the negative is the ordinal into the animset
					extern void entity_loop_using_negative_playanimineditor(int e, int obj, cstr animname);
					entity_loop_using_negative_playanimineditor(ele_id, t.obj, t.entityprofile[t.entid].playanimineditor_name);
				}
				else
				{
					LoopObject (  t.obj  ); StopObject (  t.obj );
				}
			}
		}
		else
		{
			MakeObjectCube (  t.obj,25 );
		}
		//  ensure new object ONLY interacts with main camera and shadow camera
		//PE: 130217 added t.entityprofile[t.gridentity].zdepth == 0 to prevent decals from calling technique11 DepthMap
		if (  t.entityprofile[t.gridentity].ismarker != 0 || t.entityprofile[t.gridentity].zdepth == 0 )
		{
			SetObjectMask (  t.obj, 1 );
		}
		else
		{
			SetObjectMask (  t.obj, 1+(1<<31) );
		}
		//  pivot alignment
		if (  t.entityprofile[t.gridentity].fixnewy != 0 ) 
		{
			RotateObject (  t.obj,0,t.entityprofile[t.gridentity].fixnewy,0 );
			FixObjectPivot (  t.obj );
		}
		// scale
		t.tescale=t.entityprofile[t.gridentity].scale;
		if (t.tescale > 0)
		{
			ScaleObject (t.obj, t.tescale, t.tescale, t.tescale);
		}

		SetObjectCollisionOff (  t.obj );
		if ( g.entityrubberbandlist.size() == 1 )
		{
			if (!pref.iEnableDragDropEntityMode)
			{
				// only dehighlight if single extract, not if a rubber band / linked entities extraction
				t.geditorhighlightingtentityID = ele_id;
				t.geditorhighlightingtentityobj = t.obj;
				WickedSetEntityId(t.gridentity);
				WickedSetElementId(ele_id);
				editor_restoreobjhighlightifnotrubberbanded(t.obj);
				WickedSetEntityId(-1);
				WickedSetElementId(0);
			}
		}
		t.gridentityobj=t.obj;

		if (t.entityprofile[t.gridentity].ismarker == 2)
		{
			if (ele_id > 0)
			{
				// full light update inc. color
				entity_updatelightobj(ele_id, t.gridentityobj);
			}
			else
			{
				// do not know light settings, so just white point light
				entity_updatelightobjtype(t.gridentityobj, 0);
			}
		}

		if (t.entityprofile[t.gridentity].islightmarker == 1)
		{
			sObject* pObject = g_ObjectList[t.obj];
			if (pObject)
				WickedCall_SetObjectCastShadows(pObject, false);
			t.entityprofile[t.gridentity].castshadow = -1;
		}
		
		cstr sEffectLower = Lower(t.entityprofile[t.gridentity].effect_s.Get());
		if (sEffectLower == "effectbank\\reloaded\\decal_animate1_additive.fx")
		{
			//PE: AvengingEagle's Light Effects.
			DisableObjectZWrite(t.obj); //Additive blending.
			void WickedCall_SetObjectBlendMode(sObject * pObject, int iBlendmode);
			sObject* pObject = g_ObjectList[t.obj];
			if (pObject)
				WickedCall_SetObjectBlendMode(pObject, BLENDMODE_ADDITIVE);
			t.entityprofile[t.gridentity].blendmode = BLENDMODE_ADDITIVE;
			if (ele_id > 0)
				t.entityelement[ele_id].eleprof.blendmode = BLENDMODE_ADDITIVE;
			for (int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++)
			{
				if (pObject->ppMeshList[iMesh]) pObject->ppMeshList[iMesh]->iCullMode = 0;
			}
			WickedCall_SetObjectCullmode(pObject);

		}

		//PE: Old decal support.
		if (t.entityprofile[t.gridentity].bIsDecal)
		{
			SetupDecalObject(t.obj, ele_id);
		}
	}

	//HighLight instantly.
	if (t.gridentityobj > 0) 
	{
		if (t.gridentityobj < g_iObjectListCount)
		{
			if (g_ObjectList[t.gridentityobj])
			{
				if (t.gridentitystaticmode)
					g_selected_editor_color = XMSTATICCOLOR;
				else
					g_selected_editor_color = XMDYNAMICCOLOR;
				g_selected_editor_object = g_ObjectList[t.gridentityobj];
				g_selected_editor_objectID = t.gridentityobj;
			}
		}
	}

	editor_refreshentitycursor ( );
}

void gridedit_displayentitycursor ( void )
{
	//  create entity foating selection
	if (  t.gridentity>0 ) 
	{
		t.obj=t.gridentityobj;
		if (  ObjectExist(t.obj) == 1 ) 
		{
			PositionObject (  t.obj,t.gridentityposx_f,t.gridentityposy_f,t.gridentityposz_f );
			RotateObject (  t.obj,t.gridentityrotatex_f,t.gridentityrotatey_f,t.gridentityrotatez_f );
			t.tfinalscalex_f=t.gridentityscalex_f;
			t.tfinalscaley_f=t.gridentityscaley_f;
			t.tfinalscalez_f=t.gridentityscalez_f;
			ScaleObject ( t.obj, t.tfinalscalex_f, t.tfinalscaley_f, t.tfinalscalez_f );
			if (  t.gridentity>0 )
			{
				if (  t.entityprofile[t.gridentity].ischaracter == 0 ) 
				{
					t.tanimspeed_f=t.entityprofile[t.gridentity].animspeed;
					SetObjectSpeed (  t.obj,g.timeelapsed_f*t.tanimspeed_f );
				}
			}
		}
	}

	//  if entity cursor light, instantly feed into shader OVERRIDING LIGHT ZERO
	lighting_override ( );
}

void gridedit_deletelevelobjects ( void )
{
	// clear OBJ values in entityelements (as all objects are being removed)
	if ( g.entityelementlist>0 ) 
	{
		for ( t.e = 1 ; t.e <= g.entityelementlist; t.e++ )
		{
			// delete any env probes 
			int entid = t.entityelement[t.e].bankindex;
			if (entid > 0)
			{
				if (t.entityprofile[entid].ismarker == 2)
				{
					entity_deleteprobe(t.entityelement[t.e].obj);
				}
			}

			t.obj = t.entityelement[t.e].obj;
			if ( t.obj > 0 ) 
			{
				if ( ObjectExist(t.obj) == 1 ) DeleteObject (  t.obj );
			}

			t.entityelement[t.e].obj=0;
			t.entityelement[t.e].bankindex=0;
			deleteinternalsound(t.entityelement[t.e].soundset) ; t.entityelement[t.e].soundset = 0;
			deleteinternalsound(t.entityelement[t.e].soundset1) ; t.entityelement[t.e].soundset1 = 0;
			deleteinternalsound(t.entityelement[t.e].soundset2) ; t.entityelement[t.e].soundset2 = 0;
			deleteinternalsound(t.entityelement[t.e].soundset3) ; t.entityelement[t.e].soundset3 = 0;
			deleteinternalsound(t.entityelement[t.e].soundset4) ; t.entityelement[t.e].soundset4 = 0;
			deleteinternalsound(t.entityelement[t.e].soundset5); t.entityelement[t.e].soundset5 = 0;
			deleteinternalsound(t.entityelement[t.e].soundset6); t.entityelement[t.e].soundset6 = 0;
		}
	}
	UnDim (  t.entityelement );
	g.entityelementmax=100;
	Dim (  t.entityelement,g.entityelementmax  );
	g.entityelementlist=0;

	//  delete all objects used for level edit
	for ( t.obj = g.entityviewstartobj ; t.obj <= g.entityviewendobj; t.obj++ )
	{
		if (  ObjectExist(t.obj) == 1  )  DeleteObject (  t.obj );
	}

	//  also delete all entitybank references
	entity_deletebank ( );

	//  Indicate no level objects
	g.entityviewendobj=0;

	//  270215 - 011 - Create new entities from the beginning
	g.entityviewcurrentobj=g.entityviewstartobj;
}

float GetCurveDistanceScaler(void)
{
	return g.globals.CurveDistanceScaler;
}

void modifyplaneimagestrip ( int objno, int texmax, int texindex )
{
	float s_f = 0;
	float u_f = 0;

	//  Lock the vertex data of the object
	LockVertexDataForLimbCore (  objno,0,1 );

	//  adjust UV data
	s_f=1.0/texmax ; u_f=texindex*s_f;
	SetVertexDataUV (  0,u_f+s_f,0.0 );
	SetVertexDataUV (  1,u_f,0.0 );
	SetVertexDataUV (  2,u_f+s_f,1.0 );
	SetVertexDataUV (  3,u_f,0.0 );
	SetVertexDataUV (  4,u_f,1.0 );
	SetVertexDataUV (  5,u_f+s_f,1.0 );

	//  Unlock the vertex data of the object
	UnlockVertexData (  );
}

int Get_Spray_Mode_On(void)
{
	if (iDisplayCircleFrames > 0)
	{
		iDisplayCircleFrames--;
		return(true);
	}
	if (t.gridentity == 0) return false;
	return(t.gridedit.entityspraymode);
}

void init_readouts()
{
	extern std::vector<std::string> readoutTitles;
	extern std::vector<STORYBOARD_WIDGET_> readoutWidgetTypes;
	extern std::vector<ReadoutLayers> readoutLayers;
	extern std::vector<ReadoutTypes> readoutTypes;
	extern std::vector<std::function<void()>> readoutCallbacks;

	readoutTitles.push_back("User Defined Global");
	readoutWidgetTypes.push_back(STORYBOARD_WIDGET_TEXT);
	readoutLayers.push_back(READOUT_GAMEPLAY);
	readoutTypes.push_back(READOUT_INT);
	readoutCallbacks.push_back(nullptr);

	readoutTitles.push_back("User Defined Global Text");
	readoutWidgetTypes.push_back(STORYBOARD_WIDGET_TEXT);
	readoutLayers.push_back(READOUT_GAMEPLAY);
	readoutTypes.push_back(READOUT_STRING);
	readoutCallbacks.push_back(nullptr);

	readoutTitles.push_back("User Defined Global Statusbar");
	readoutWidgetTypes.push_back(STORYBOARD_WIDGET_BAR);
	readoutLayers.push_back(READOUT_GAMEPLAY);
	readoutTypes.push_back(READOUT_INT);
	readoutCallbacks.push_back(nullptr);

	readoutTitles.push_back("User Defined Global Image");
	readoutWidgetTypes.push_back(STORYBOARD_WIDGET_IMAGE);
	readoutLayers.push_back(READOUT_GAMEPLAY);
	readoutTypes.push_back(READOUT_INT);
	readoutCallbacks.push_back(nullptr);

	readoutTitles.push_back("User Defined Global Panel");
	readoutWidgetTypes.push_back(STORYBOARD_WIDGET_IMAGE);
	readoutLayers.push_back(READOUT_GAMEPLAY);
	readoutTypes.push_back(READOUT_INT);
	readoutCallbacks.push_back(nullptr);

	readoutTitles.push_back("Health Remaining");
	readoutWidgetTypes.push_back(STORYBOARD_WIDGET_TEXT);
	readoutLayers.push_back(READOUT_GAMEPLAY);
	readoutTypes.push_back(READOUT_INT);
	readoutCallbacks.push_back(nullptr);

	readoutTitles.push_back("Maximum Health");
	readoutWidgetTypes.push_back(STORYBOARD_WIDGET_TEXT);
	readoutLayers.push_back(READOUT_GAMEPLAY);
	readoutTypes.push_back(READOUT_INT);
	readoutCallbacks.push_back(nullptr);

	readoutTitles.push_back("Health Panel");
	readoutWidgetTypes.push_back(STORYBOARD_WIDGET_IMAGE);
	readoutLayers.push_back(READOUT_GAMEPLAY);
	readoutTypes.push_back(READOUT_INT);
	readoutCallbacks.push_back(nullptr);

	readoutTitles.push_back("Ammo Remaining");
	readoutWidgetTypes.push_back(STORYBOARD_WIDGET_TEXT);
	readoutLayers.push_back(READOUT_GAMEPLAY);
	readoutTypes.push_back(READOUT_INT);
	readoutCallbacks.push_back(nullptr);

	readoutTitles.push_back("Maximum Ammo");
	readoutWidgetTypes.push_back(STORYBOARD_WIDGET_TEXT);
	readoutLayers.push_back(READOUT_GAMEPLAY);
	readoutTypes.push_back(READOUT_INT);
	readoutCallbacks.push_back(nullptr);

	readoutTitles.push_back("Ammo Panel");
	readoutWidgetTypes.push_back(STORYBOARD_WIDGET_IMAGE);
	readoutLayers.push_back(READOUT_GAMEPLAY);
	readoutTypes.push_back(READOUT_INT);
	readoutCallbacks.push_back(nullptr);

	readoutTitles.push_back("Weapon Held");
	readoutWidgetTypes.push_back(STORYBOARD_WIDGET_IMAGE);
	readoutLayers.push_back(READOUT_GAMEPLAY);
	readoutTypes.push_back(READOUT_INT);
	readoutCallbacks.push_back(nullptr);

	readoutTitles.push_back("Weapon Firemode");
	readoutWidgetTypes.push_back(STORYBOARD_WIDGET_IMAGE);
	readoutLayers.push_back(READOUT_GAMEPLAY);
	readoutTypes.push_back(READOUT_INT);
	readoutCallbacks.push_back(nullptr);

	readoutTitles.push_back("Lives Remaining");
	readoutWidgetTypes.push_back(STORYBOARD_WIDGET_TEXT);
	readoutLayers.push_back(READOUT_GAMEPLAY);
	readoutTypes.push_back(READOUT_INT);
	readoutCallbacks.push_back(nullptr);
}


void display_profiler_data(ImDrawList* draw, char* filter,int startline)
{
	extern ImFont* customfont;
	if (!customfont) return;
	float wide = 300.0f;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 window_pos = ImVec2((viewport->Pos.x + viewport->Size.x - 10.0f), (viewport->Pos.y + 10.0f));

	bool bProfile = true;
	bProfilerEnable = true;
	wiProfiler::SetEnabled(true);
	std::string profiler_data = wiProfiler::GetProfilerDataFilter(filter);
	float line = startline;
	char* find = (char*)pestrcasestr(profiler_data.c_str(), "\n");
	while (find)
	{
		char * find2 = (char*) pestrcasestr(find + 1, "\n");
		char old = '\n';
		if (find2)
		{
			old = find2[0];
			find2[0] = 0;
		}
		draw->AddText(customfont, 15, ImVec2(window_pos.x - wide - 2, viewport->Pos.y + 24.0 + (14.0f * line) - 2), IM_COL32(0, 0, 0, 255), find + 1);
		draw->AddText(customfont, 15, ImVec2(window_pos.x - wide, viewport->Pos.y + 24.0 + (14.0f * line)), IM_COL32(255, 255, 255, 255), find + 1);
		line++;
		if (find2)
		{
			find2[0] = old;
		}
		find = (char*)pestrcasestr(find + 1, "\n");
	}
}

int GetDrawCallsShadowsCube2(void)
{
	return(wiProfiler::GetDrawCallsShadowsCube());
}

int GetWidgetMode(void)
{
	return t.widget.mode;
}
int GetEntityGridMode(void)
{
	return t.gridentitygridlock;
}
int GetEntitySelected(void)
{
	return t.widget.pickedEntityIndex;
}
int GetEntityObject(int iEntityIndex)
{
	return(t.entityelement[iEntityIndex].obj);
}
void GetEntityPosition(int iEntityIndex, float & x, float& y, float& z)
{
	x = t.entityelement[iEntityIndex].x;
	y = t.entityelement[iEntityIndex].y;
	z = t.entityelement[iEntityIndex].z;
}
int GetRubberbandSize(void)
{
	return(g.entityrubberbandlist.size());
}
void SetWidgetMode(int mode)
{
	bool bWidgetEnabled = pref.iEnableDragDropWidgetSelect;
	switch (mode)
	{
		case 0:
		{
			//pref.iEnableDragDropWidgetSelect = true
			if (bWidgetEnabled)
			{
				t.widget.mode = 0;
				widget_show_widget();
			}
		} break;
		case 1:
		{
			if (bWidgetEnabled)
			{
				t.widget.mode = 1;
				widget_show_widget();
			}
		} break;
		case 2:
		{
			if (bWidgetEnabled)
			{
				// Don't allow characters and markers to be scaled with the widget
				if (t.widget.pickedEntityIndex > 0 && t.widget.pickedEntityIndex < t.entityelement.size())
				{
					int entid = t.entityelement[t.widget.pickedEntityIndex].bankindex;
					if (entid > 0)
					{
						bool bAllowObjectsAndParticlesToScale = false;
						if (t.entityprofile[entid].ismarker == 0) bAllowObjectsAndParticlesToScale = true;
						if (t.entityprofile[entid].ismarker == 10) bAllowObjectsAndParticlesToScale = true;
						if (t.entityprofile[entid].ischaracter == 0 && bAllowObjectsAndParticlesToScale == true)
						{
							t.widget.mode = 2;
							widget_show_widget();
						}
					}
				}
			}
		} break;
		case 3:
		{
			pref.iEnableDragDropWidgetSelect = !pref.iEnableDragDropWidgetSelect;
			if (pref.iEnableDragDropWidgetSelect)
				widget_show_widget();
			else
				widget_hide();
		} break;
		case 4:
		{
			//PE: Toggle snap.
			if (pref.iGridMode == 1)
				pref.iGridMode = 0;
			else
				pref.iGridMode = 1;
			t.gridentitygridlock = pref.iGridMode;
		} break;
		case 5:
		{
			if (pref.iGridMode!=2)
			{
				// grid on
				pref.iGridEnabled = true;
				pref.iGridMode = 2; //PE: Set grid mode to 2.
			}
			else
			{
				// grid off
				pref.iGridEnabled = false;
				pref.iGridMode = 0; //PE: Set grid mode to 0.
			}
			t.gridentitygridlock = pref.iGridMode;
		} break;
	}
}
void GridPopup(ImVec2 wpos)
{
	static bool bPopupOpen = false;
	if(bPopupOpen && wpos.x != 0)
		ImGui::SetNextWindowPos(wpos);
	ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 2.0f);
	if (ImGui::BeginPopup("Grid##GridSettings", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		bPopupOpen = true;
		bool bButSpacer = true;
		const float button_width_fix = 5.0f;
		ImGui::Indent(10);
		int iEntityIndex = t.widget.pickedEntityIndex;
		int iActiveObj = t.widget.activeObject;
		if (t.gridentityextractedindex > 0)
		{
			iEntityIndex = t.gridentityextractedindex;
			if (t.gridentityobj > 0)
				iActiveObj = t.gridentityobj;
		}
		else
		{
			if (t.widget.activeObject == 0 && t.widget.pickedEntityIndex < t.entityelement.size())
			{
				if (t.widget.pickedEntityIndex > 0)
					iActiveObj = t.tentityobj = t.entityelement[t.widget.pickedEntityIndex].obj;
			}
		}

		float but_gadget_size = ImGui::GetFontSize() * 14.0;
		ImGui::ItemSize(ImVec2(ImGui::GetFontSize() * 15.0, 0));
		ImGui::SetWindowFontScale(1.1);
		ImGui::TextCenter("Grid and Alignment Settings");
		ImGui::SetWindowFontScale(1.0);

		// grid size only available in advanced mode
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
		if (1)//pref.iObjectEnableAdvanced)
		{
			if (1)//t.gridentitygridlock == 2)
			{
				if (pref.iAdvancedGridModeSettings == 0)
				{
					// Simple Grid Mode
					ImGui::TextCenter("Grid Size");
					float w = ImGui::GetContentRegionAvail().x;
					float inputsize = w / 4.0f;
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w/2)-(inputsize/2), 0.0f));
					ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
					ImGui::InputFloat("##XYZgridsizeXYZ", &pref.fEditorGridSizeX, 0.0f, 0.0f, "%.1f");
					if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Size");
					ImGui::PopItemWidth();

					// can never have a grid size below one
					if (pref.fEditorGridSizeX <= 0.1f) pref.fEditorGridSizeX = 0.1f;

					// and all grid dimensions the same!
					pref.fEditorGridOffsetX = 0;
					pref.fEditorGridOffsetY = 0;
					pref.fEditorGridOffsetZ = 0;
					pref.fEditorGridSizeY = pref.fEditorGridSizeX;
					pref.fEditorGridSizeZ = pref.fEditorGridSizeX;
				}
				else
				{
					// Advanced Grid Mode functions and settings
					ImGui::TextCenter("Grid Offset");
					float w = ImGui::GetContentRegionAvail().x;
					float inputsize = w / 3.0f;
					inputsize -= 10.0f; //For text.
					inputsize -= 5.0f; //For padding.
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 3.0f));
					ImGui::Text("X");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, -3.0f));
					ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
					ImGui::InputFloat("##XYZgridoffsetX", &pref.fEditorGridOffsetX, 0.0f, 0.0f, "%.1f");
					if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Offset X");
					ImGui::PopItemWidth();
					ImGui::SameLine();
					ImGui::Text("Y");
					ImGui::SameLine();
					ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
					ImGui::InputFloat("##XYZgridoffsetY", &pref.fEditorGridOffsetY, 0.0f, 0.0f, "%.1f");
					if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Offset Y");
					ImGui::PopItemWidth();
					ImGui::SameLine();
					ImGui::Text("Z");
					ImGui::SameLine();
					ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
					ImGui::InputFloat("##XYZgridoffsetZ", &pref.fEditorGridOffsetZ, 0.0f, 0.0f, "%.1f");
					if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Offset Z");
					ImGui::PopItemWidth();

					ImGui::TextCenter("Grid Size");
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 3.0f));
					ImGui::Text("X");
					ImGui::SameLine();
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, -3.0f));
					ImGui::SameLine();
					ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
					ImGui::InputFloat("##XYZgridsizeX", &pref.fEditorGridSizeX, 0.0f, 0.0f, "%.1f");
					if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Size X");
					ImGui::PopItemWidth();
					ImGui::SameLine();
					ImGui::Text("Y");
					ImGui::SameLine();
					ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
					ImGui::InputFloat("##XYZgridsizeY", &pref.fEditorGridSizeY, 0.0f, 0.0f, "%.1f");
					if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Size Y");
					ImGui::PopItemWidth();
					ImGui::SameLine();
					ImGui::Text("Z");
					ImGui::SameLine();
					ImGui::PushItemWidth(inputsize - ImGui::GetFontSize());
					ImGui::InputFloat("##XYZgridsizeZ", &pref.fEditorGridSizeZ, 0.0f, 0.0f, "%.1f");
					if (!pref.iTurnOffEditboxTooltip && ImGui::IsItemHovered()) ImGui::SetTooltip("Change Grid Size Z");
					ImGui::PopItemWidth();

					bButSpacer = false;
					ImGui::Text("");
					ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w * 0.5) - ((but_gadget_size * 0.5) + button_width_fix), 0.0f));
					if (ImGui::StyleButton("Default Grid Settings", ImVec2(but_gadget_size, 0)))
					{
						pref.fEditorGridOffsetX = 50;
						pref.fEditorGridOffsetY = 0;
						pref.fEditorGridOffsetZ = 50;
						pref.fEditorGridSizeX = 100;
						pref.fEditorGridSizeY = 10;
						pref.fEditorGridSizeZ = 100;
					}

					// clever button to align grid to object (for older levels with arbitary alignments mixed together)
					if (iEntityIndex > 0 && g.entityrubberbandlist.size() == 0)
					{
						ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w * 0.5) - ((but_gadget_size * 0.5) + button_width_fix), 0.0f));
						if (ImGui::StyleButton("Align Grid Offset To Object", ImVec2(but_gadget_size, 0)))
						{
							float x = t.entityelement[iEntityIndex].x;
							float y = t.entityelement[iEntityIndex].y;
							float z = t.entityelement[iEntityIndex].z;
							int iSizeRoundedX = int(x / pref.fEditorGridSizeX) * pref.fEditorGridSizeX;
							pref.fEditorGridOffsetX = x - iSizeRoundedX;
							int iSizeRoundedY = int(y / pref.fEditorGridSizeY) * pref.fEditorGridSizeY;
							pref.fEditorGridOffsetY = y - iSizeRoundedY;
							int iSizeRoundedZ = int(z / pref.fEditorGridSizeZ) * pref.fEditorGridSizeZ;
							pref.fEditorGridOffsetZ = z - iSizeRoundedZ;
						}
						ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w * 0.5) - ((but_gadget_size * 0.5) + button_width_fix), 0.0f));
						if (ImGui::StyleButton("Align Grid Size To Object", ImVec2(but_gadget_size, 0)))
						{
							float sx = ObjectSizeX(t.entityelement[iEntityIndex].obj, 1);
							float sy = ObjectSizeY(t.entityelement[iEntityIndex].obj, 1);
							float sz = ObjectSizeZ(t.entityelement[iEntityIndex].obj, 1);
							pref.fEditorGridSizeX = sx;
							pref.fEditorGridSizeY = sy;
							pref.fEditorGridSizeZ = sz;
						}
					}

					// can never have a grid size below one
					if (pref.fEditorGridSizeX <= 0.1f) pref.fEditorGridSizeX = 0.1f;
					if (pref.fEditorGridSizeY <= 0.1f) pref.fEditorGridSizeY = 0.1f;
					if (pref.fEditorGridSizeZ <= 0.1f) pref.fEditorGridSizeZ = 0.1f;
				}
			}
		}

		if (vEntityLockedList.size() > 0)
		{
			if(bButSpacer)
				ImGui::Text("");
			float w = ImGui::GetContentRegionAvail().x;
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w * 0.5) - ((but_gadget_size * 0.5) + button_width_fix), 0.0f));
			cStr unlockstr = cStr("Unlock ") + cStr((int)vEntityLockedList.size()) + cStr(" Objects");
			if (ImGui::StyleButton(unlockstr.Get(), ImVec2(but_gadget_size, 0)))
			{
				for (int i = 0; i < vEntityLockedList.size(); i++)
				{
					int e = vEntityLockedList[i].e;
					if (e > 0 && e < t.entityelement.size())
					{
						t.entityelement[e].editorlock = 0;
						sObject* pObject;
						if (t.entityelement[e].obj > 0)
						{
							if (t.entityelement[e].obj < g_iObjectListCount)
							{
								pObject = g_ObjectList[t.entityelement[e].obj];
								if (pObject)
								{
									WickedCall_SetObjectRenderLayer(pObject, GGRENDERLAYERS_NORMAL);
								}
							}
						}
					}
				}
				vEntityLockedList.clear();

				// any lock/unlock operations resets, avoids issue of duplcating a static object and unable to 'move' it
				t.widget.pickedObject = 0;
			}
		}

		ImGui::Text("");
		ImGui::Indent(-10);
		ImGui::EndPopup();
	}
	else
		bPopupOpen = false;
	ImGui::PopStyleVar(1);

}

bool GetEnableEmptyLevelMode(void)
{
	return t.visuals.bEnableEmptyLevelMode;
}

float GetEnvProbeBrightness(void)
{
	return t.visuals.fEnvProbeBrightness;
}

void GetConvertSettings(int *maxwidth,int *active)
{
	*active = g.globals.ConvertToDDS;
	*maxwidth = g.globals.ConvertToDDSMaxSize;
}

int GetActiveEditorObject( void )
{
	int iActiveObj = t.widget.activeObject;
	if (t.gridentityextractedindex > 0)
	{
		if (t.gridentityobj > 0)
			iActiveObj = t.gridentityobj;
	}
	else
	{
		int iEntityIndex = t.widget.pickedEntityIndex;
		if (t.widget.activeObject == 0 && t.widget.pickedEntityIndex < t.entityelement.size())
		{
			if (t.widget.pickedEntityIndex > 0)
				iActiveObj = t.entityelement[t.widget.pickedEntityIndex].obj;
			else
			{
				iActiveObj = t.widget.activeObject;
			}
		}
	}
	return(iActiveObj);
}
int GetActiveEditorEntityPos(float *x, float *y, float *z, float* xa, float* ya, float* za)
{
	int iActiveObj = t.widget.activeObject;
	int iEntityIndex = 0;
	if (t.gridentityextractedindex > 0)
	{
		iEntityIndex = t.gridentityextractedindex;
	}
	else
	{
		iEntityIndex = t.widget.pickedEntityIndex;
	}
	if (iEntityIndex > 0 && iEntityIndex < t.entityelement.size())
	{
		if (x)
		{
			*x = t.entityelement[iEntityIndex].x;
			*y = t.entityelement[iEntityIndex].y;
			*z = t.entityelement[iEntityIndex].z;
		}
		if (xa)
		{
			*xa = t.entityelement[iEntityIndex].rx;
			*ya = t.entityelement[iEntityIndex].ry;
			*za = t.entityelement[iEntityIndex].rz;
		}
	}
	return(iEntityIndex);
}

int GetActiveEditorEntity(void)
{
	int iActiveObj = t.widget.activeObject;
	int iEntityIndex = 0;
	if (t.gridentityextractedindex > 0)
	{
		iEntityIndex = t.gridentityextractedindex;
	}
	else
	{
		iEntityIndex = t.widget.pickedEntityIndex;
	}
	return(iEntityIndex);
}

void EmptyMessages(void)
{
	if (g.globals.DisableMessagePump > 0)
		return;

	//PE: Empty messages , so windows dont think we are dead. ( perhaps remember QUIT ? )
	MSG msg = { 0 };
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		//PE: No WM_QUIT while saving/loading levels.
		if (msg.message != WM_QUIT)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}
