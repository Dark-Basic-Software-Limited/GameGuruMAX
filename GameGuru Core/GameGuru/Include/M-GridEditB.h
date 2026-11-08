//
// Included by GridEdit and GridEdit B for types and protos common to both
//
#pragma once

// Includes
#include "M-CharacterCreatorPlusTTS.h"

//PE: GameGuru IMGUI.
#include "..\..\GameGuru\Imgui\imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "..\..\GameGuru\Imgui\imgui_internal.h"
#include "..\..\GameGuru\Imgui\imgui_impl_win32.h"
#include "..\..\GameGuru\Imgui\imgui_gg_dx11.h"

// structures
#define TUTORIALMAXTEXT 1024
#define TUTORIALMAXSTEPS 20
struct ActiveTutorial
{
	bool bActive = false;
	int iSteps = 0;
	char cStartText[TUTORIALMAXTEXT] = "\0";
	char cVideoPath[TUTORIALMAXTEXT] = "\0";
	bool bVideoReady = false;
	bool bVideoInit = false;
	int bVideoID = 0;
	char cStepHeader[TUTORIALMAXSTEPS][TUTORIALMAXTEXT] = { "\0" };
	char cStepText[TUTORIALMAXSTEPS][TUTORIALMAXTEXT] = { "\0" };
	char cStepAction[TUTORIALMAXSTEPS][TUTORIALMAXTEXT] = { "\0" };
	ImVec2 vOffsetPointer[TUTORIALMAXSTEPS] = { ImVec2(0, 0) };
	int iCurrent_Step = 0;
	float fScore = 0;
};

struct sLibraryList
{
	cstr cFile = "";
	cstr cPath = "PatH";
	cstr cName = "";
	cstr cDescription = "";
	time_t m_tFileModify = 0;
	int iImage = 0;
	int iType = 0;
	int iOrderBy = 0;
	cstr cProject = "";
	int timer = 0;
	bool bProjectExists = false;
};

class cFolderItem
{
public:
	struct sFolderFiles {
		sFolderFiles * m_dropptr; //Need to be the first entry for drag/drop.
		cStr m_sName;
		cStr m_sNameFinal;
		cStr m_sNameFinalCredit;
		cStr m_sPath;
		cStr m_sFolder;
		UINT iFlags;
		int iPreview; //Preview image.
		int iBigPreview; //Preview image.
		int id;
		int iAnimationFrom = 0;
		bool bPreviewProcessed;
		long last_used;
		bool bSorted;
		bool bFavorite;
		bool bAvailableInFreeTrial;
		time_t m_tFileModify;
		cstr m_Backdrop;
		sFolderFiles * m_pNext;
		sFolderFiles * m_pNextTime;
		sFolderFiles * m_pCustomSort;
		cFolderItem *pNewFolder;
		cStr m_sBetterSearch;
		int uniqueId;
		bool m_bFPELoaded;
		cstr m_sFPEModel;
		cstr m_sFPETextured;
		cstr m_sFPEKeywords;
		cstr m_sFPEDBOFile;
		cstr m_sDLuaDescription;
		float m_fDLuaHeight;
		int m_iFPEDBOFileSize;
		bool m_bIsCharacterCreator;
		bool m_bIsGroupObject;
		int iType;
	};
	cStr m_sFolder;
	cStr m_sFolderFullPath;
	int m_iEntityOffset;
	cFolderItem *m_pNext;
	cFolderItem *m_pSubFolder;
	sFolderFiles * m_pFirstFile;
	bool m_bFilesRead;
	bool visible;
	bool alwaysvisible;
	bool deletethisentry;
	bool bIsCustomFolder;
	char cfolder[256]; //PE: Only for faster sorting.
	time_t m_tFolderModify;
	float m_fLastTimeUpdate;
	UINT iFlags;
	int count;
	int iType;
	cFolderItem() { m_pNext = 0; iType = 0; iFlags = 0; m_bFilesRead = false; m_pFirstFile = NULL; m_pNext = NULL; m_pSubFolder = NULL; m_fLastTimeUpdate = 0; m_iEntityOffset = 0; }
	~cFolderItem() { }
};

int screen_editor(int nodeid, bool standalone = false, char *screen = NULL);

#include ".\..\..\Guru-WickedMAX\wickedcalls.h"
void tab_tab_visuals(int iPage, int iMode);
void Wicked_Update_Visuals(void *voidvisual);
void Wicked_Update_Visibles(void* voidvisual);
void DrawLogicNodes(bool bVisible);
void CreateDotObject(int obj);
void PositionDotObject(int obj);
void MoveSelectedDotObject(void);
void DisplayRelationshipMenu(int iDotMiddleIndex, int mode = 0);
void DrawDotArcsCircle(int from, int radius);
bool CreateBackBufferCacheNameEx(char* file, int width, int height, bool bUsedForSaving);
bool CreateBackBufferCacheName(char* file, int width, int height);
bool CopyToProjectFolder(char *file);
bool CreateProjectCacheName(char *project, char *file);
void RevertBackbufferCubemap(void);
bool DeleteEntityFromLists(int e);
int isEntityInGroupList(int e, int ignoregroup = -1);
int isEntityInGroupListDirect(int e, int group);
void ReplaceEntityInGroupList(int e, int eto);
void CheckGroupListForRubberbandSelectionsForAllEntities(int iEntityInGroupList, int entityindex);
void CheckGroupListForRubberbandSelections(int entityindex);
void AddGroupListToRubberBand(int l);
int DuplicateFromListToCursor(std::vector<sRubberBandType> vEntityDuplicateList, bool bRandomShiftXZ = true, int iOriginalGroupIndexForChild = -1, bool bAttachToCursor = true);
void ListGroupContextMenu(bool bPickedOnly = false, int iEntityId = 0);
void AddEntityToCursor(int e, bool bDuplicate = true);
void BeginDragDropFPE(char *fpe, int textureid, bool bToolTipActive, ImVec2 vSize);
void CreateNewGroup(int iParentGroupID, bool bSnapshotGroupThumb = true, cstr GroupName_s = "", bool bGenerateUndo = true, int iForceIntoIndex = -1);
bool SaveGroup(int iGroupID, LPSTR pObjectSavedFilename);
bool LoadGroup(LPSTR pAbsFilename);
void UnGroupSelected(bool bRetainRubberBandList = false);
void UnGroupUndoSys(int index);
void GroupUndoSys(int index, std::vector<sRubberBandType> groupData);
void FreeTempImageList(void);
void DisplayFPEBehavior(bool readonly, int entid, entityeleproftype* edit_grideleprof = NULL, int elementID = 0, bool bHideIcon = false);
void DisplayFPEAdvanced(bool readonly, int entid, entityeleproftype *edit_grideleprof = NULL, int elementID = 0);
void DisplayFPEGeneral(bool readonly, int entid, entityeleproftype *edit_grideleprof, int elementID);
void DisplayFPEPhysics(bool readonly, int entid, entityeleproftype *edit_grideleprof = NULL);
void DisplayFPEMedia(bool readonly, int entid, entityeleproftype *edit_grideleprof = NULL);
char * imgui_setpropertystring2_v2(int group, char* data_s, char* field_s, char* desc_s, bool readonly);
char * imgui_setpropertyfile2_v2(int group, char* data_s, char* field_s, char* desc_s, char* within_s, bool readonly, char *startsearch = NULL);
char * imgui_setpropertylist2c_v2(int group, int controlindex, char* data_s, char* field_s, char* desc_s, int listtype, bool readonly, bool forcharacters, bool bForShooting, bool bForMelee, int iSpecialValue);
int imgui_setpropertylist2_v2(int group, int controlindex, char* data_s, char* field_s, char* desc_s, int listtype, bool readonly = false);
void StartDragDropFromEntityID(int iEntID, int iGroup = -1, int iCustomImage = 0);
void ApplyPivotToGridEntity(void);
void ProcessBugReporting(void);
void GetBugReport(void);
ImVec4 GetRealCenterToGridEntity(void);
ImVec4 GetRealCenterToObject(int obj);
ImVec4 GetRealSizeToGridEntity(int direction);
float GetLowestY(int obj);
void ClearAllGroupLists(void);
ImVec2 Convert3DTo2D(float x, float y, float z);
void GetRubberbandLowHighValues(void);
void DragDrop_CheckTrashcanDrop(ImRect bb);
void MouseLeftDragXZPanning(void);
void MouseWheelYPanning(void);
void DragCameraMovement(void);
void GetFilesListForLibrary(char *path, bool bCreateThumbs = false, int win = 0, int iThumbWidth = 512, int iThumbHeight = 288,int SortType = 0);
cstr ComboFilesListForLibrary(char *currentselection, int columns = 0, int iFixedWidth = -1, bool bRemoveExtension = false);
cstr ListboxFilesListForLibrary(char *currentselection, int columns = 0, int iFixedWidth = -1, bool bRemoveExtension = false, bool bIncludeNone = true, char *filter = NULL);
bool imgui_CheckMinMaxStartupState(int win);
bool imgui_AddMinMaxButton(int win, bool bRestore);
bool imgui_GetMinMaxButtonState(int win);
void Welcome_Screen(void);
void About_Screen(void);
bool DoTreeNodeEntity(int masterid, bool bMoveCameraToObjectPosition);
bool DoTreeNodeGroup(int groupindex, bool bMoveCameraToObjectPosition);
bool DoTreeNodeBehavior(LPSTR behaviorscriptname, bool bMoveCameraToObjectPosition);
void SetupDecalObject(int obj, int elementID);
void SetIconSet(bool bInstant = false);
void SetIconSetCheck(bool bInstant = false);
void process_storeboard(bool bInitOnly = false);
int save_create_storyboard_project(void);
void LockSelectedObject(bool bLock, int iLockedIndex);


void process_entity_library(void);
void process_entity_library_v2(void);
bool TutorialNextAction(void);
bool CheckTutorialPlaceit(void);
bool CheckTutorialAction(const char * action, float x_adder = 0.0f);
void RenderToPreview(int displayobj);
void CheckTooltipObjectDelete(void);
void get_tutorials(void);
void SmallTutorialVideo(char *tutorial, char* combo_items[] = NULL, int combo_entries = 0, int iVideoSection = 0, bool bAutoStart = false);
void SetVideoVolume(float volume);
void ProcessPreferences(void);
void CloseAllOpenTools(bool bTerrainTools = true);
void CloseAllOpenToolsThatNeedSave(void);
void imgui_shooter_tools(void);
float ApplyPivot(sObject* pObject, int iMode, GGVECTOR3 vecValue, float fValue);
void imgui_terrain_loop(void);
void imgui_terrain_loop_v2(void);
void imgui_terrain_loop_v3(void);

char * imgui_setpropertystring2(int group, char* data_s, char* field_s, char* desc_s);
int imgui_setpropertylist2(int group, int controlindex, char* data_s, char* field_s, char* desc_s, int listtype);
char * imgui_setpropertylist2c(int group, int controlindex, char* data_s, char* field_s, char* desc_s, int listtype);
char * imgui_setpropertyfile2(int group, char* data_s, char* field_s, char* desc_s, char* within_s);
char * imgui_setpropertyfile2_dlua(int group, char* data_s, char* field_s, char* desc_s, char* within_s);

void ParseLuaScriptWithElementID(entityeleproftype *tmpeleprof, char * script, int elementID);
void ParseLuaScript(entityeleproftype *tmpeleprof, char * script);
int DisplayLuaDescription(entityeleproftype *tmpeleprof);
int DisplayLuaDescriptionOnly(entityeleproftype *tmpeleprof);
float fDisplaySegmentText(char *text);
void SpeechControls(int speech_entries, bool bUpdateMainString, entityeleproftype *edit_grideleprof = NULL);
void RedockWindow(char *name);
void CheckMinimumDockSpaceSize(float minsize);
void generic_preloadfiles(void);
void CloseDownEditorProperties(void);


// moved here so Classic would compile
void ControlAdvancedSetting(int& setting, const char* tooltip, bool* bLargePreview);
void DeleteWaypointsAddedToCurrentCursor(void);
void Add_Grid_Snap_To_Position(bool bFromWidgetMode);
float ImGuiGetMouseX(void);
float ImGuiGetMouseY(void);
void RotateAndMoveRubberBand(int iActiveObj, float fMovedActiveObjectX, float fMovedActiveObjectY, float fMovedActiveObjectZ, GGQUATERNION quatRotationEvent); //float fMovedActiveObjectRX, float fMovedActiveObjectRY, float fMovedActiveObjectRZ);
void SetStartPositionsForRubberBand(int iActiveObj);
void DeleteWaypointsAddedToCurrentCursor(void);

wchar_t *CharToWchar(const char *str);
long FileSize(const char *file_name);
HRESULT EnumerateVoices();
void ReleaseVoices();
HRESULT FileToWav(CComPtr<ISpVoice> spVoice, ISpObjectToken *voice, int iSpeakRate, const char *textFile, const char *wavFile);
