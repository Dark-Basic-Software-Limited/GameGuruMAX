
// PE: changed to be used in GameGuru

#include <algorithm>
#include <string>
#include <vector>
#include "preprocessor-flags.h"
#include "preprocessor-moreflags.h"

	#define TABENTITYNAME "Level Objects##LeftPanel"
	#define TABEDITORNAME "3D Level Editor##GGRenderarget"
	//PE: Set the smallest font size to use, this was normally 0.8.
	#define SMALLFONTSIZE 0.9
	#define SMALLESTFONTSIZE 0.9

//We need to do a reset layout when a new window is added to a new version , so increase this if this is the case.
#define V3VERSION 62
//PE: Increase MAXVERSION to set preference to defaults and reset imgui windows settings (for Friday builds).
//PE: This do not reset important user features like "Write folder" ...
#define MAXVERSION 26
//PE: Increase this to ONLY reset windows, when adding a new docked window. all other setting is not changed.
#define MAXWINDOWSVERSION 3


#define ENABLEIMGUI
#define USERENDERTARGET
#define ALLOW_WEATHER_IN_EDITOR
#define ADDGGTOOLBAR

//PE: icons use tons of memory i have 12000 so...
#define DYNAMICLOADUNLOAD

//Image range reserved for UIV3
#define UIV3IMAGES 43000

#pragma once

struct ID3D11Device;
struct ID3D11DeviceContext;

IMGUI_IMPL_API bool     ImGui_ImplDX11_Init(ID3D11Device* device, ID3D11DeviceContext* device_context);
IMGUI_IMPL_API void     ImGui_ImplDX11_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplDX11_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data);

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_IMPL_API void     ImGui_ImplDX11_InvalidateDeviceObjects();
IMGUI_IMPL_API bool     ImGui_ImplDX11_CreateDeviceObjects();

#define TOOL_SHAPE UIV3IMAGES+1
#define TOOL_LEVELMODE UIV3IMAGES+2
#define TOOL_STOREDLEVEL UIV3IMAGES+3
#define TOOL_BLENDMODE UIV3IMAGES+4
#define TOOL_RAMPMODE UIV3IMAGES+5
#define TOOL_RANDOM UIV3IMAGES+216
#define TOOL_PICK UIV3IMAGES+217
#define TOOL_WRITE UIV3IMAGES+218
#define TOOL_RESTORE UIV3IMAGES+219
#define TOOL_PAINTTEXTURE UIV3IMAGES+6
#define TOOL_PAINTGRASS UIV3IMAGES+7
#define TOOL_ENTITY UIV3IMAGES+8
#define TOOL_MARKERS UIV3IMAGES+9
#define TOOL_WAYPOINTS UIV3IMAGES+10
#define TOOL_NEWWAYPOINTS UIV3IMAGES+11
#define TOOL_TESTGAME UIV3IMAGES+12
#define TOOL_VRMODE UIV3IMAGES+13
#define TOOL_SOCIALVR UIV3IMAGES+14

#define TOOL_HEADER UIV3IMAGES+15

#define TOOL_NEWLEVEL UIV3IMAGES+16
#define TOOL_LOADLEVEL UIV3IMAGES+17
#define TOOL_SAVELEVEL UIV3IMAGES+18

#define ROUNDING_OVERLAY UIV3IMAGES+19

#define EBE_BLOCK UIV3IMAGES+20
#define EBE_COLUMN UIV3IMAGES+21
#define EBE_CUBE UIV3IMAGES+22
#define EBE_FLOOR UIV3IMAGES+23
#define EBE_NEW UIV3IMAGES+24
#define EBE_ROW UIV3IMAGES+25
#define EBE_STAIRS UIV3IMAGES+26
#define EBE_WALL UIV3IMAGES+27
#define EBE_EBE UIV3IMAGES+28


#define EBE_CONTROL1 UIV3IMAGES+30
#define EBE_CONTROL2 UIV3IMAGES+31
#define EBE_CONTROL3 UIV3IMAGES+32
#define EBE_CONTROL4 UIV3IMAGES+33
#define EBE_CONTROL5 UIV3IMAGES+34
#define EBE_CONTROL6 UIV3IMAGES+35
#define EBE_THUMB UIV3IMAGES+36


#define TOOL_BUILDER UIV3IMAGES+40
#define TOOL_CCP UIV3IMAGES+41
#define TOOL_IMPORT UIV3IMAGES+42

#define MEDIA_PLAY UIV3IMAGES+43
#define MEDIA_PAUSE UIV3IMAGES+44
#define MEDIA_REFRESH UIV3IMAGES+45
#define TUTORIAL_POINTER UIV3IMAGES+46
#define MEDIA_RECORD UIV3IMAGES+47
#define MEDIA_RECORDING UIV3IMAGES+48
#define MEDIA_RECORDPROCESSING UIV3IMAGES+49

#define HELPMENU_IMAGE UIV3IMAGES+50
#define ABOUT_LOGO UIV3IMAGES+51
#define ABOUT_HB UIV3IMAGES+52
#define ABOUT_TGC UIV3IMAGES+53

#define TUTORIAL_POINTERUP UIV3IMAGES+54

#define TOOL_SHAPE_UP UIV3IMAGES+55
#define TOOL_SHAPE_DOWN UIV3IMAGES+56

#define TOOL_DRAWWAYPOINTS UIV3IMAGES+57

#define TOOL_DOTCIRCLE UIV3IMAGES+58
#define TOOL_DOTCIRCLE_S UIV3IMAGES+59
#define TOOL_DOTCIRCLE_M UIV3IMAGES+60

#define TOOL_CIRCLE UIV3IMAGES+61
#define TOOL_CIRCLE_S UIV3IMAGES+62
#define TOOL_CIRCLE_M UIV3IMAGES+63

#define TOOL_ENT_EDIT UIV3IMAGES+71
#define TOOL_ENT_EXTRACT UIV3IMAGES+72
#define TOOL_ENT_DUPLICATE UIV3IMAGES+73
#define TOOL_ENT_LOCK UIV3IMAGES+74
#define TOOL_ENT_FINDFLOOR UIV3IMAGES+75
#define TOOL_ENT_DELETE UIV3IMAGES+76
#define TOOL_ENT_SEARCH UIV3IMAGES+77

#define TOOL_VISUALS UIV3IMAGES+80
#define TOOL_CAMERA UIV3IMAGES+81
#define TOOL_CAMERALIGHT UIV3IMAGES+82
#define TOOL_GOBACK UIV3IMAGES+83
#define MEDIA_MAXIMIZE UIV3IMAGES+84
#define MEDIA_MINIMIZE UIV3IMAGES+85

#define ENV_SUN UIV3IMAGES+86
#define ENV_RAIN UIV3IMAGES+87
#define ENV_SNOW UIV3IMAGES+88
#define ENV_WEATHER UIV3IMAGES+89

#define TOOL_GAME_SETTINGS UIV3IMAGES+90

#define TOOL_GOEXIT UIV3IMAGES+91
//#define TOOL_RPG UIV3IMAGES+90
//#define TOOL_PUZZLE UIV3IMAGES+91
#define TOOL_LOGIC UIV3IMAGES+92

#define TOOL_TRIGGERZONE UIV3IMAGES+93
#define TOOL_FLAG UIV3IMAGES+94

#define TOOL_PENCIL UIV3IMAGES+95

#define UI3D_DOTOBJECTS UIV3IMAGES+96
#define UI3D_DOTMIDDLEOBJECTS UIV3IMAGES+97

#define SHAPE_CIRCLE UIV3IMAGES+100
#define SHAPE_SQUARE UIV3IMAGES+101

#define KEY_ALT UIV3IMAGES+105
#define KEY_BACKSPACE UIV3IMAGES+106
#define KEY_KEYBOARD UIV3IMAGES+107
#define KEY_CONTROL UIV3IMAGES+108
#define KEY_MINUS UIV3IMAGES+109
#define KEY_PLUS UIV3IMAGES+110
#define KEY_SHIFT UIV3IMAGES+111
#define MOUSE_LMB UIV3IMAGES+112
#define MOUSE_RMB UIV3IMAGES+113
//#define KEY_CONTROL_2 UIV3IMAGES+114 // used ?
#define KEY_SEPARATOR UIV3IMAGES+114
#define KEY_SEPARATOR_SMALL UIV3IMAGES+115

#define MEDIA_FAVORITE UIV3IMAGES+116

#define TOOL_GROUPEDIT UIV3IMAGES+117
#define TOOL_GROUP UIV3IMAGES+118
#define TOOL_UNGROUP UIV3IMAGES+119

#define TOOL_TRASHCAN UIV3IMAGES+120
#define TOOL_LOCK UIV3IMAGES+121
#define TOOL_UNLOCK UIV3IMAGES+122

#define MEDIA_FAVORITE_DIS UIV3IMAGES+123
#define KEY_MAXIMIZE UIV3IMAGES+124
#define MOUSE_MMB UIV3IMAGES+125

#define KEY_R UIV3IMAGES+126
#define KEY_DELETE UIV3IMAGES+127
#define KEY_Y UIV3IMAGES+128
#define KEY_RETURN UIV3IMAGES+129
#define KEY_PGUP UIV3IMAGES+130
#define KEY_PGDN UIV3IMAGES+131
#define KEY_F UIV3IMAGES+132
#define KEY_G UIV3IMAGES+133

#define OBJECT_MOVE_XZ UIV3IMAGES+134
#define OBJECT_MOVE_Y UIV3IMAGES+135
#define KEY_TAB UIV3IMAGES+136

#define KEY_CONTROL_SHIFT UIV3IMAGES+137

#define ICON_INFO UIV3IMAGES+138
#define OBJECT_MOVE_SURFACESCAN UIV3IMAGES+139
#define OBJECT_MOVE_FINDFLOOR UIV3IMAGES+140
#define OBJECT_MOVE_LOCK UIV3IMAGES+141
#define OBJECT_MOVE_UNLOCK UIV3IMAGES+142

#define TOOL_ENT_FILTER UIV3IMAGES+143

#define MARKETPLACE_GGMAX UIV3IMAGES+144
#define MARKETPLACE_GCSTORE UIV3IMAGES+145
#define MARKETPLACE_SKETCHFAB UIV3IMAGES+146
#define MARKETPLACE_FILLER UIV3IMAGES+147
#define MARKETPLACE_HEADER UIV3IMAGES+148
#define WELCOME_HEADER UIV3IMAGES+149
#define WELCOME_FILLERROUNDED UIV3IMAGES+150

#define FILETYPE_OGG UIV3IMAGES+151
#define FILETYPE_MP3 UIV3IMAGES+152
#define FILETYPE_WAV UIV3IMAGES+153
#define MARKETPLACE_SHOCKWAVESOUND UIV3IMAGES+154

#define FILETYPE_VIDEO UIV3IMAGES+155
#define FILETYPE_SCRIPT UIV3IMAGES+156
#define PLAYER_START UIV3IMAGES+157
#define FILETYPE_PARTICLE UIV3IMAGES+158

#define ENTITY_PARTICLE UIV3IMAGES+159
#define ENTITY_LIGHT UIV3IMAGES+160
#define ENTITY_WIN UIV3IMAGES+161
#define ENTITY_IMAGE UIV3IMAGES+162
#define ENTITY_MUSIC UIV3IMAGES+163
#define ENTITY_SOUND UIV3IMAGES+164
#define ENTITY_TEXT UIV3IMAGES+165
#define ENTITY_VIDEO UIV3IMAGES+166

#define MARKETPLACE_COMMUNITY UIV3IMAGES+167

#define LIGHT_POINT UIV3IMAGES+168
#define LIGHT_SPOT UIV3IMAGES+169

#define ENTITY_START UIV3IMAGES+170
#define ENTITY_CHECKPOINT UIV3IMAGES+171

#define OBJECT_MOVE_ORIENTATION UIV3IMAGES+172

#define FILTER_CHAR_ON UIV3IMAGES+173
#define FILTER_CHAR_OFF UIV3IMAGES+174
#define FILTER_SCENARY_ON UIV3IMAGES+175
#define FILTER_SCENARY_OFF UIV3IMAGES+176
#define FILTER_FAVORITE_ON UIV3IMAGES+177
#define FILTER_FAVORITE_OFF UIV3IMAGES+178
#define FILTER_HUD_ON UIV3IMAGES+179
#define FILTER_HUD_OFF UIV3IMAGES+180
#define FILTER_ELEMENTS_ON UIV3IMAGES+181
#define FILTER_ELEMENTS_OFF UIV3IMAGES+182
#define FILTER_USER_ON UIV3IMAGES+183
#define FILTER_USER_OFF UIV3IMAGES+184
#define FILTER_DLUA_ON UIV3IMAGES+185
#define FILTER_DLUA_OFF UIV3IMAGES+186

#define MEDIA_PIN UIV3IMAGES+187
#define MEDIA_UNPIN UIV3IMAGES+188

#define TOOL_GROUPSAVE UIV3IMAGES+189

#define ENTITY_FLAG UIV3IMAGES+190
#define ENTITY_GUNS UIV3IMAGES+191
#define ENTITY_AMMO UIV3IMAGES+192
#define ENTITY_ENEMIES UIV3IMAGES+193
#define ENTITY_ALLIES UIV3IMAGES+194

#define INFOIMAGE UIV3IMAGES+195
#define TOOL_SMARTOBJECT UIV3IMAGES+196
#define TOOL_TERRAIN_TOOLBAR UIV3IMAGES+197

#define ENTITY_PROBE UIV3IMAGES+198
#define ENTITY_COVER UIV3IMAGES+199
#define ENTITY_BEHAVIOR UIV3IMAGES+211

#define CCP_HAT UIV3IMAGES+200
#define CCP_FEET UIV3IMAGES+201
#define CCP_LEGS UIV3IMAGES+202
#define CCP_BODY UIV3IMAGES+203
#define CCP_GLASSES UIV3IMAGES+204
#define CCP_BEARD UIV3IMAGES+205
#define CCP_HAIR UIV3IMAGES+206
#define CCP_HEAD UIV3IMAGES+207
#define CCP_EMPTY UIV3IMAGES+208
#define CCP_NONE UIV3IMAGES+209
#define CCP_TATTOO UIV3IMAGES+210
//PE: 211 used above.
#define CCP_ACCESSORY1 UIV3IMAGES+212
#define CCP_ACCESSORY2 UIV3IMAGES+213

#define ENTITY_NEW_PARTICLE UIV3IMAGES+214

#define KEY_Z UIV3IMAGES+215


#define TERRAIN_RANDOM UIV3IMAGES+216
#define TERRAIN_PICK UIV3IMAGES+217
#define TERRAIN_WRITE UIV3IMAGES+218
#define TERRAIN_RESTORE UIV3IMAGES+219

#define STORYBOARD_HEADER UIV3IMAGES+220
#define STORYBOARD_BACKDROP UIV3IMAGES+221
#define STORYBOARD_MUSIC UIV3IMAGES+222
#define STORYBOARD_PREVIEW UIV3IMAGES+223


#define KEY_N UIV3IMAGES+225
#define KEY_L UIV3IMAGES+226
#define KEY_E UIV3IMAGES+227
#define KEY_SPACE UIV3IMAGES+228

#define STORYBOARD_TRANSPARET UIV3IMAGES+229
#define WELCOME_VIDEO UIV3IMAGES+230

#define TOOL_PAINTTREE UIV3IMAGES+231
#define TOOL_TREE_ADD UIV3IMAGES+232
#define TOOL_TREE_DELETE UIV3IMAGES+233
#define TOOL_TREE_MOVE UIV3IMAGES+234
#define TOOL_TREES_DELETE UIV3IMAGES+235
#define TOOL_TREES_ADD UIV3IMAGES+236

#define BOX_CLICK_HERE UIV3IMAGES+237

#define FREETRIAL_HEADER UIV3IMAGES+238
#define FREETRIAL_BODY UIV3IMAGES+239

#define KEY_T UIV3IMAGES+240
#define KEY_O UIV3IMAGES+241
#define KEY_Q UIV3IMAGES+242

#define FREETRIAL_NOTAVAILABLE UIV3IMAGES+243

#define ENTITY_TRIGGERZONE UIV3IMAGES+245

#define BRAIN_ICON UIV3IMAGES+250
#define QUESTION_ICON UIV3IMAGES+251

#define UI3D_TERRAINMOVER UIV3IMAGES+252


#define TOOL_PAINTBUSH UIV3IMAGES+253
#define TOOL_BUSH_ADD UIV3IMAGES+254
#define TOOL_BUSH_DELETE UIV3IMAGES+255
#define TOOL_BUSH_MOVE UIV3IMAGES+256
#define TOOL_BUSHES_DELETE UIV3IMAGES+257
#define TOOL_BUSHES_ADD UIV3IMAGES+258
#define TOOL_BUSH_SCALE UIV3IMAGES+259
#define TOOL_TREE_SCALE UIV3IMAGES+260

#define PLAYER_START2 UIV3IMAGES+261

#define KEY_I UIV3IMAGES+262

#define HUB_LIVEBROADCAST UIV3IMAGES+263
#define HUB_DISCORD UIV3IMAGES+264
#define HUB_FACEBOOK UIV3IMAGES+265
#define HUB_FORUM UIV3IMAGES+266
#define HUB_WORKSHOPITEM UIV3IMAGES+267
#define HUB_TIKTOK UIV3IMAGES+268
#define HUB_TWITTER UIV3IMAGES+269
#define HUB_USERGUIDE UIV3IMAGES+270
#define HUB_WEBSITE UIV3IMAGES+271

#define FREETRIAL_COUNTER_BASE UIV3IMAGES+272
#define FREETRIAL_COUNTER_ONEDAY UIV3IMAGES+273

#define SCREENEDITOR_IMAGE UIV3IMAGES+280
#define SCREENEDITOR_TEXT UIV3IMAGES+281
#define SCREENEDITOR_BUTTON UIV3IMAGES+282
#define SCREENEDITOR_TICKBOX UIV3IMAGES+283
#define SCREENEDITOR_SLIDER UIV3IMAGES+284
#define SCREENEDITOR_PROGRESSBAR UIV3IMAGES+285
#define SCREENEDITOR_RADIOBUTTON UIV3IMAGES+286
#define SCREENEDITOR_TEXTAREA UIV3IMAGES+287
#define SCREENEDITOR_VIDEO UIV3IMAGES+288

#define TOOLBAR_POSITION UIV3IMAGES+289
#define TOOLBAR_SCALE UIV3IMAGES+290
#define TOOLBAR_ROTATE UIV3IMAGES+291
#define TOOLBAR_GRID UIV3IMAGES+292
#define TOOLBAR_SNAP UIV3IMAGES+293
#define TOOLBAR_SURFACE UIV3IMAGES+294
#define TOOLBAR_VERT UIV3IMAGES+295
#define TOOLBAR_HORI UIV3IMAGES+296
#define TOOLBAR_GRIDSETTINGS UIV3IMAGES+297

#define ENTITY_EYE_ON UIV3IMAGES+700
#define ENTITY_EYE_OFF UIV3IMAGES+701

//PE: 02-09-2021 moved range.
//298,297 also used in backdrop.
#define BACKDROPMAGE UIV3IMAGES+299
#define SKYBOX_ICONS UIV3IMAGES+300
#define MARKETPLACE_ICONS UIV3IMAGES+400

#define HUB_COMMTUT0 UIV3IMAGES+601
#define HUB_COMMTUT1 UIV3IMAGES+602
#define HUB_COMMTUT2 UIV3IMAGES+603
#define HUB_COMMTUT3 UIV3IMAGES+604
#define HUB_COMMTUT4 UIV3IMAGES+605
#define HUB_COMMTUT5 UIV3IMAGES+606

#define ENTITY_CACHE_ICONS UIV3IMAGES+1000
#define ENTITY_CACHE_ICONS_LARGE UIV3IMAGES+2000
#define PROPERTIES_CACHE_ICONS UIV3IMAGES+3000
#define IMPORTER_ALL_MESH UIV3IMAGES + 4000
#define CCP_THUMBS UIV3IMAGES + 5000
#define FILTER_THUMBS UIV3IMAGES + 5900

#define TERRAINGENERATOR_OBJECT 17998
//#define TERRAINGENERATOR_IMAGE 63340 //PE: Not used and goes into storyboard uniqueids.

//PE: STORYBOARD_THUMBS must be the last in the list here. if you need room here, increase STORYBOARD_THUMBS so its last.
#define STORYBOARD_THUMBS UIV3IMAGES + 6000

bool overWriteFileBox(char * file);
bool CancelQuit();
bool askBox(char * ask, char *title);
bool changedFileBox(char * file);
void BoxerInfo(char * text, const char *heading);
void DebugInfo(char * text, const char *heading);
int askBoxCancel(const char * ask, const char *title);

enum {
	NOC_FILE_DIALOG_OPEN = 1 << 0,   // Create an open file dialog.
	NOC_FILE_DIALOG_SAVE = 1 << 1,   // Create a save file dialog.
	NOC_FILE_DIALOG_DIR = 1 << 2,   // Open a directory.
	NOC_FILE_DIALOG_OVERWRITE_CONFIRMATION = 1 << 3,
};
const char *noc_file_dialog_open(int flags,
	const char *filters,
	const char *default_path,
	const char *default_name, 
	bool bUseDefaultPath = false, 
	const char* pTitle = NULL);

namespace ImGui {
	bool MenuItem2(const char* label, const char* shortcut, bool selected, bool enabled);
	bool BeginPopupContextItemAGK(const char* str_id = NULL, int mouse_button = 1);
	const char* CalcWordWrapPositionB(float scale, const char* text, const char* text_end, float wrap_width, float line_start);
	void TextCenter(const char* fmt, ...);
	bool StyleButtonEx(const char* label, const ImVec2& size_arg = ImVec2(0, 0), bool bDisabled = false);
	bool StyleButton(const char* label, const ImVec2& size_arg = ImVec2(0, 0));
	bool StyleButtonDark(const char* label, const ImVec2& size_arg = ImVec2(0, 0));
	bool HyberlinkButton(const char* label, const ImVec2& size_arg = ImVec2(0, 0));
	bool MinMaxButtonEx(const char* str_id, ImGuiDir dir);
	void RenderArrowOutLine(ImDrawList* draw_list, ImVec2 pos, ImU32 col, ImGuiDir dir, float scale);
	bool StyleCollapsingHeader(const char* label, ImGuiTreeNodeFlags flags = 0);
	bool IsLastSliderHovered(void);
	bool MaxSliderInputFloat(const char* label, float* v, float v_min, float v_max, const char* tooltip, int startval=0, float maxval = 100.0f, int numericboxwidth = 37.0f);
	bool MaxSliderInputFloat2(const char* label, float* v, float v_min, float v_max, const char* tooltip, int startval = 0, float maxval = 100.0f, int numericboxwidth = 37.0f);
	bool MaxSliderInputFloatPower(const char* label, float* v, float v_min, float v_max, const char* tooltip, int startval = 0, float maxval = 100.0f, int numericboxwidth = 37.0f, float power = 1.0f, int precision = 0);
	bool MaxSliderInputInt(const char* label, int* v, int v_min, int v_max, const char* tooltip, int boxWidth = 37);
	bool MaxSliderInputRangeFloat(const char* label, float* v, float* v2, float v_min, float v_max, const char* tooltip);
	bool MaxSliderInputRangeFloatDirect(const char* label, float* v, float* v2, float v_min, float v_max, const char* tooltip);
	bool windowTabVisible(void);
	int windowTabFlags(void);
	int windowDockNodeId(void);
	void SetBlurMode(bool blur);

	bool ImgBtn(int iImageID, const ImVec2& btn_size = ImVec2(0, 0), const ImVec4& bg_col = ImColor(255, 255, 255, 0),
		const ImVec4& drawCol_normal = ImColor(220, 220, 220, 220),
		const ImVec4& drawCol_hover = ImColor(255, 255, 255, 255),
		const ImVec4& drawCol_Down = ImColor(180, 180, 160, 255), int frame_padding = -1, int atlasindex = 0, int atlasrows = 0, int atlascolumns = 0, bool nowhite = false, bool gratiant = false, bool center_image = false, bool noalpha = false, bool useownid = false, bool boost25 = false);
	bool ImgBtnWicked(void* pMaterial, const ImVec2& btn_size = ImVec2(0, 0), const ImVec4& bg_col = ImColor(255, 255, 255, 0),
		const ImVec4& drawCol_normal = ImColor(220, 220, 220, 220),
		const ImVec4& drawCol_hover = ImColor(255, 255, 255, 255),
		const ImVec4& drawCol_Down = ImColor(180, 180, 160, 255), int frame_padding = -1, int atlasindex = 0, int atlasrows = 0, int atlascolumns = 0, bool nowhite = false, bool gratiant = false, bool center_image = false, bool noalpha = false, bool useownid = false);
	bool ImgBtnBack(int iImageID, const ImVec2& btn_size = ImVec2(0, 0), const ImVec4& bg_col = ImColor(255, 255, 255, 0),
		const ImVec4& drawCol_normal = ImColor(220, 220, 220, 220),
		const ImVec4& drawCol_hover = ImColor(255, 255, 255, 255),
		const ImVec4& drawCol_Down = ImColor(180, 180, 160, 255), int frame_padding = 0);

	void ToggleButton(const char* str_id, bool* v);
	bool RoundButton(const char* str_id,ImVec2 size, float radius);
	bool MaxIsItemFocused(void);
	void ImRotateStart(void);
	ImVec2 ImRotationCenter(void);
	void ImRotateEnd(float rad, ImVec2 center);
	void ImGui_GG_HideWindow(ImGuiViewport* viewport);
	void ImGui_GG_ShowWindow(ImGuiViewport* viewport);
	void HideAllViewPortWindows(void);
	void ShowAllViewPortWindows(void);
	bool BeginTimeline(const char* str_id, float max_value);
	bool TimelineEvent(const char* str_id, float* values);
	void EndTimeline(void);
	bool RangeSlider(const char* str_id, float & val1, float & val2, float max_value, bool bDisplayValues = true);
}

#define REMEMBERLASTFILES 5
#define MAXSEARCHHISTORY 7

#define SECTION_VISUALS 1
#define SECTION_LEVEL_ENTITIES 2
#define SECTION_ENTITY_TOOLS 3
#define SECTION_SCULPT_TERRAIN 4
#define SECTION_PAINT_TERRAIN 5
#define SECTION_ADD_VEGETATION 6
#define SECTION_WAYPOINTS 7
#define SECTION_CHARACTER_CREATOR 8
#define SECTION_STRUCTURE_EDITOR 9
#define SECTION_IMPORTER 10
#define SECTION_WEATHER 11

#define SECTION_SHOOTERGENRE 12
#define SECTION_RPGGENRE 13
#define SECTION_PUZZLEGENRE 14

#define SECTION_STORYBOARD 15
#define SECTION_TERRAIN_GENERATOR 16
#define SECTION_LEVEL_EDITOR 17
#define SECTION_OBJECT_LIBRARY 18
#define SECTION_MAX_HUB 19


struct preferences {
	char szCheckFile[15] = "GAMEGURU-PREFS";
	int launched = 0;
	int current_style = 0;
	// int iImporterDome; //@Zak this will ruin all old saved setttings as it load directly to the structure, always add to the bottom of list.
	ImVec2 vStartResolution = { 1280,800 };
	char last_open_files[10][260] = { "\0","\0","\0","\0","\0","\0","\0","\0","\0","\0" };
	int save_layout = true;
	int dummy_filler = 0;
	int current_version = 0;
	char search_history[15][260] = { "\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0" };
	char filler[260];
	int iMaximized = 1;
	bool bHideTutorials = false;
	bool bMultiplyOpenHeaders = false;
	bool bAutoClosePropertySections = true;
	bool bDisableMultipleViewport = false;
	int dummy_filler2 = 0; //PE: Importent we align to 32bit when using bools here. thats why we have fillers.
	int iAllowUndocking = false;
	ImVec4 tint_style = ImVec4(0.0, 0.0, 0.0, 0.0);
	ImVec4 shade_style = ImVec4(0.0, 0.0, 0.0, 0.0);
	char small_search_history[15][260] = { "\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0" };
	int iTurnOffUITransparent = false;
	int iPlayedVideoSection[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int iResetAutoRunVideosOnNextStartup = 0;
	int iEnableCustomColors = 0;
	int iEnableDragDropEntityMode = 1;
	ImVec4 title_style = ImVec4(0.0, 0.0, 0.0, 0.0);
	int iEnableAdvancedSky = 0;
	int iEnableAdvancedWater = 0;
	int iEnableAdvancedPostProcessing = 0;
	int iEnableAdvancedShadows = 0;
	int iEnableArcRelationshipLines = 1;
	int iEnableRelationPopupWindow = 0;
	int iEnableAxisRotationShortcuts = 0;
	int iObjectEnableAdvanced = 0;
	int iEnableDragDropWidgetSelect = 0;
	int iEnableDragDropMousePickSystem = 0;
	int iEnableEditorOutlineSelection = 1;
	int iEnableSingleRightPanelAdvanced = 0;
	char cCustomWriteFolder[260] = { "\0" };
	int iEnableDeveloperProperties = 0;
	int iEnableIdentityProperties = 1;
	int iDragCameraMovement = 1;
	int bAutoOpenMenuItems = true;
	int iGameCreaterStore = 0;
	char cDefaultImportPath[260] = { "\0" };
	int iFullscreenPreviewAdvanced = 0;
	char last_import_files[10][260] = { "\0","\0","\0","\0","\0","\0","\0","\0","\0","\0" };
	int iDisplayWelcomeScreen = 1;
	float vSaved_Light_Palette_R[16] = { 1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f };
	float vSaved_Light_Palette_G[16] = { 1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f };
	float vSaved_Light_Palette_B[16] = { 1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f };
	int iSaved_Light_Type[16] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
	int iSaved_Light_Range[16] = { 0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	float fSaved_Light_ProbeScale[16] = { 1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f };

	//Particles.
	char Saved_Particle_Name[16][260] = { "\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0" };
	int Saved_bParticle_Preview[16] = { true,true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
	int Saved_bParticle_Show_At_Start[16] = { true,true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
	int Saved_bParticle_Looping_Animation[16] = { true,true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
	int Saved_bParticle_Full_Screen[16] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
	float Saved_fParticle_Fullscreen_Duration[16] = { 10.0f,10.0f,10.0f,10.0f,10.0f,10.0f,10.0f,10.0f,10.0f,10.0f,10.0f,10.0f,10.0f,10.0f,10.0f,10.0f };
	float Saved_fParticle_Fullscreen_Fadein[16] = { 1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f };
	float Saved_fParticle_Fullscreen_Fadeout[16] = { 1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f };
	char Saved_Particle_Fullscreen_Transition[16][260] = { "\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0" };
	float Saved_fParticle_Speed[16] = { 1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f };
	float Saved_fParticle_Opacity[16] = { 1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f };
	int iEnableAdvancedEntityList = 0;
	float fHighLightThickness = 1.0;

	int iTurnOffEditboxTooltip = false;
	int iImporterDome = 1;
	int iEnableAutoExposureInEditor = 0;
	char cRememberLastSearchObjects[260] = { "\0" };
	int iSetColumnsEntityLib = 3;
	int iCheckboxFilters[10] = { 1,1,1,1,1,1,1,1,1,1 };
	
	int iTerrainAdvanced = 0;
	int iAdvancedGridModeSettings = 0;// iTerrainDebugMode = 0;
	int iEnableAdvancedCharacterCreator = 0;

	char cDefaultStandalonePath[260] = { "\0" };
	int iStoryboardAdvanced = 0;

	char last_project_files[10][260] = { "\0","\0","\0","\0","\0","\0","\0","\0","\0","\0" };

	int iDisableObjectLibraryViewport = 1; //PE: Now used to dsable all multi-viewports for all windows, to a crash on some single monitor setups.
	int iLastInStoryboard = false;
	char cLastUsedStoryboardProject[260] = { "\0" };

	time_t changelog_ftime = 0;

	int iDisableProjectAutoSave = 0;
	int iDisableLevelAutoSave = 0;

	int iSplashStartMessage = 0;
	int iTitleStartMessage = 0;
	int iLoadingStartMessage = 0;
	int iGameWonStartMessage = 0;
	int iGameOverStartMessage = 0;

	int iHideKeyboardShortcuts = 0;

	int iDisplayIntroScreen = 1;

	int iEnableDeveloperObjectTools = 0;
	int iEnableTerrainHeightmaps = 0;

	int iEnableLevelEditorOpenAndNew = 0;
	int iDisplayTerrainGeneratorWelcome = 1;

	int iTestGameGraphicsQuality = 2;

	int iSaved_Light_Radius[16] = { 45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45 };
	float fSaved_Light_AngX[16] = { 90.0f,90.0f,90.0f,90.0f,90.0f,90.0f,90.0f,90.0f,90.0f,90.0f,90.0f,90.0f,90.0f,90.0f,90.0f,90.0f };
	float fSaved_Light_AngY[16] = { 0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	float fSaved_Light_AngZ[16] = { 0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	int iEnableAdvancedGrass = 0;
	int iEnableFpsMemMonitor = 0;
	int iEnableAutoFlattenSystem = 1;
	int iProjectSortMode = 0;
	int iEnableDragDropStopSelectFromInside = 0;
	int iGridEnabled = 0;
	int iGridMode = 0;
	float fEditorGridOffsetX = 50.0f;
	float fEditorGridOffsetY = 0.0f;
	float fEditorGridOffsetZ = 50.0f;
	float fEditorGridSizeX = 100.0f;
	float fEditorGridSizeY = 100.0f;
	float fEditorGridSizeZ = 100.0f;
	int iDevToolsOpen = 0;
	int iCheckFilesModifiedOnFocus = 1;
	int iSmallToolbar = 0;
	ImVec4 status_bar_color = ImVec4((1.0f / 255.0f) * 14, (1.0f / 255.0f) * 99, (1.0f / 255.0f) * 156, 1.0);
	ImVec4 highlight_color = status_bar_color;
	int current_version_new_windows = 0;
	int square_storybord_grid = 0;
};

// can be moved to preferences (Lee needs to know the sequence to add to prefs properly, and the consequences) :)
extern bool g_bEnableAutoFlattenSystem;

void coreResetIMGUIFunctionalityPrefs(void);

void myDarkStyle(ImGuiStyle* dst);
void myDefaultStyles(void);
void myLightStyle(ImGuiStyle* dst);
void myStyle(ImGuiStyle* dst);
void myStyle2(ImGuiStyle* dst);
void myStyle2_colors_only(void);
void myStyle3(ImGuiStyle* dst);
void TintCurrentStyle(void);
void myStyleBlue(ImGuiStyle* dst);

void ChangeGGFont(const char *cpcustomfont, int iIDEFontSize);
bool NoCaseLess(const std::string &a, const std::string &b);
void GetMainEntityList(char* folder_s, char* rel_s, void *pFolder ,char* folder_name_start="", bool bForceToTop=false, int foldertype = 0);
bool saveVectorFileContent(std::string fileName, std::vector<std::string> & vecOfStrs);
bool getVectorFileContent(std::string fileName, std::vector<std::string> & vecOfStrs,bool bAllowEmpty = false);
void RemoveStrStrFromVectorFile(char *str, std::vector<std::string> & vecOfStrs,bool bIgnoreSpaces = false);
std::string GetLineFromVectorFile(char *str, std::vector<std::string> & vecOfStrs, bool bIgnoreSpaces);
std::string GetLineParameterFromVectorFile(char *str, std::vector<std::string> & vecOfStrs, bool bIgnoreSpaces);
void fpe_thread_start(void);
bool fpe_thread_in_progress(void);

int cstring_cmp_folder(const void *a, const void *b);
void RefreshEntityFolder(char* folder_s, void* pFolder);

// done in cStr
//void replaceAll(std::string& str, const std::string& from, const std::string& to);

void FindAWriteablefolder(void);
void AddPayLoad(ImGuiPayload* payload, bool addtocursor = false);

enum eKeyboardShortcutType
{
	eKST_Sculpt,
	eKST_Paint,
	eKST_AddVeg,
	eKST_ObjectMode,
	eKST_CharacterCreator,
	eKST_ObjectLibrary,
	eKST_Storyboard,
	eKST_TerrainGenerator,
	eKST_Last
};
void UniversalKeyboardShortcut(eKeyboardShortcutType KST);

#define STORYBOARD_MAXOUTPUTS 30
#define STORYBOARD_MAXWIDGETS 100
//PE: 150 = stack overflow.
#define STORYBOARD_MAXNODES 150

#define SMALL_STORYBOARD_MAXNODES 15
#define STORYBOARDVERSION 203
#define NODE_WIDTH_PADDING -15.0
#define NODE_HEIGHT_PADDING 23.0

#define EMULATERESOLUTION


void storyboard_menubar(float area_width, float node_width, float node_height);
void storeboard_init_nodes(float area_width, float node_width, float node_height);
int storyboard_add_missing_nodex(int node, float area_width, float node_width, float node_height, bool bForce = false, bool bRestoring = false);
void mapNodeStyle(void);
void load_storyboard(char *name);
bool load_checkproject_storyboard(char *name);
void save_storyboard(char *name, bool bSaveAs);
bool FindFreeLevelNode(int &iNextLevel, int &levelname, int &iFirstNodeFree);
void FindFirstLevel(int &iFirstLevelNode, char *level_name, bool bFailIfNoLink = false);
int FindNextLevel(int &iNextLevelNode, char *level_name, int action = 0);
void FindFirstSplash(char *splash_name);
int FindLuaScreenNode(char *name);
int FindOutputScreenNode(int iNode, int index);
void GetProjectList(char *path, bool bGetThumbs = false);

enum STORYBOARD_TYPE_
{
	STORYBOARD_TYPE_NONE,
	STORYBOARD_TYPE_SPLASH,
	STORYBOARD_TYPE_SCREEN,
	STORYBOARD_TYPE_LEVEL, 
	STORYBOARD_TYPE_HUD
};

enum STORYBOARD_ACTIONS_
{
	STORYBOARD_ACTIONS_NONE,
	STORYBOARD_ACTIONS_STARTGAME, //Will make a output pin to connect to level 1
	STORYBOARD_ACTIONS_EXITGAME,
	STORYBOARD_ACTIONS_GOTOSCREEN, //Will make a output pin to connect to another screen.
	STORYBOARD_ACTIONS_GOTOLEVEL, //Will make a output pin to connect to another level.
	STORYBOARD_ACTIONS_CONTINUE, //Just close screen, or goto title.lua if win or loose.lua.
	STORYBOARD_ACTIONS_BACK, //Just close screen, or goto title.lua if win or loose.lua.
	STORYBOARD_ACTIONS_LEAVEGAME, //Lua LeaveGame()
	STORYBOARD_ACTIONS_RESUMEGAME, //Lua ResumeGame()
	STORYBOARD_ACTIONS_RETURNVALUETOLUA,
	STORYBOARD_ACTIONS_GOTOSCREENHUD2,
	STORYBOARD_ACTIONS_GOTOSCREENHUD3,
	STORYBOARD_ACTIONS_GOTOSCREENHUD4,
	STORYBOARD_ACTIONS_GOTOSCREENHUD5,
	STORYBOARD_ACTIONS_GOTOSCREENHUD6,
	STORYBOARD_ACTIONS_GOTOSCREENHUD7,
	STORYBOARD_ACTIONS_GOTOSCREENHUD8,
	STORYBOARD_ACTIONS_GOTOSCREENHUD9,
	STORYBOARD_ACTIONS_GOTOSCREENHUD10,
	STORYBOARD_ACTIONS_GOTOSCREENHUD11,
	STORYBOARD_ACTIONS_GOTOSCREENHUD12,
	STORYBOARD_ACTIONS_GOTOSCREENHUD13,
	STORYBOARD_ACTIONS_GOTOSCREENHUD14,
	STORYBOARD_ACTIONS_GOTOSCREENHUD15,
	STORYBOARD_ACTIONS_GOTOSCREENHUD16,
	STORYBOARD_ACTIONS_GOTOSCREENHUD17,
	STORYBOARD_ACTIONS_GOTOSCREENHUD18,
	STORYBOARD_ACTIONS_GOTOSCREENHUD19,
	STORYBOARD_ACTIONS_GOTOSCREENHUD20,
	STORYBOARD_ACTIONS_GOTOSCREENHUD21,
	STORYBOARD_ACTIONS_GOTOSCREENHUD22,
	STORYBOARD_ACTIONS_GOTOSCREENHUD23,
	STORYBOARD_ACTIONS_GOTOSCREENHUD24,
	STORYBOARD_ACTIONS_GOTOSCREENHUD25,
	STORYBOARD_ACTIONS_GOTOSCREENHUD26,
	STORYBOARD_ACTIONS_GOTOSCREENHUD27,
	STORYBOARD_ACTIONS_GOTOSCREENHUD28,
	STORYBOARD_ACTIONS_GOTOSCREENHUD29,
	STORYBOARD_ACTIONS_GOTOSCREENHUD30,
	STORYBOARD_ACTIONS_GOTOSCREENHUD31,
	STORYBOARD_ACTIONS_GOTOSCREENHUD32
};
enum STORYBOARD_WIDGET_
{
	STORYBOARD_WIDGET_NONE,
	STORYBOARD_WIDGET_BUTTON,
	STORYBOARD_WIDGET_TEXT,
	STORYBOARD_WIDGET_IMAGE,
	STORYBOARD_WIDGET_VIDEO,
	STORYBOARD_WIDGET_PROGRESS,
	STORYBOARD_WIDGET_TEXTAREA,
	STORYBOARD_WIDGET_RADIOTYPE,
	STORYBOARD_WIDGET_SLIDER,
	STORYBOARD_WIDGET_BAR,
	STORYBOARD_WIDGET_TICKBOX
};

enum WidgetsAvailable
{
	ALLOW_BUTTON = 1 << 0,
	ALLOW_TEXT = 1 << 1,
	ALLOW_IMAGE = 1 << 2,
	ALLOW_VIDEO = 1 << 3,
	ALLOW_PROGRESS = 1 << 4,
	ALLOW_TEXTAREA = 1 << 5,
	ALLOW_RADIOTYPE = 1 << 6,
	ALLOW_SLIDER = 1 << 7,
	ALLOW_TICKBOX = 1 << 8,
};

enum ReadoutLayers
{
	READOUT_GAMEPLAY = 1 << 0,
	READOUT_GRAPHICS = 1 << 1,
	READOUT_SOUND = 1 << 2, 
	READOUT_INPUT = 1 << 3
};

enum ReadoutTypes
{
	READOUT_INT,
	READOUT_FLOAT,
	READOUT_BOOL,
	READOUT_STRING
};

struct StoryboardNodesStruct
{
	int type;
	int id;
	ImVec2 restore_position;
	int iEditEnable;
	int used;
	int thumb_id;
	char title[256];
	char levelnumber[256];
	char thumb[256];
	char level_name[256]; //PE: For custom lua scripts.
	char lua_name[256]; //PE: For custom lua scripts.
	char scene_name[256]; //PE: Scene temp files can have anykind of info, so just make each scene it own file.
	int input_id[STORYBOARD_MAXOUTPUTS];
	int output_id[STORYBOARD_MAXOUTPUTS];
	char output_action[STORYBOARD_MAXOUTPUTS][256];
	char output_title[STORYBOARD_MAXOUTPUTS][256];
	int output_linkto[STORYBOARD_MAXOUTPUTS];
	int output_can_link_to_type[STORYBOARD_MAXOUTPUTS];
	char input_title[STORYBOARD_MAXOUTPUTS][256];
	char input_action[STORYBOARD_MAXOUTPUTS][256];

	//Screen
	char screen_title[256];
	char screen_music[256];
	char screen_backdrop[256];
	int screen_backdrop_id;
	ImVec4 screen_back_color;
	int screen_backdrop_placement; // center,stretch,zoom.
	char screen_thumb[256];
	int screen_backdrop_ratio_placement[10]; // 0=1920x1080 center,stretch,zoom. 1=1366x768 center,stretch,zoom ...
	int screen_grid_size = 0;

	//Editor.
	int widget_used[STORYBOARD_MAXWIDGETS];
	char widget_label[STORYBOARD_MAXWIDGETS][256];
	ImVec2 widget_size[STORYBOARD_MAXWIDGETS];
	ImVec2 widget_pos[STORYBOARD_MAXWIDGETS];
	char widget_normal_thumb[STORYBOARD_MAXWIDGETS][256];
	int widget_normal_thumb_id[STORYBOARD_MAXWIDGETS];
	char widget_highlight_thumb[STORYBOARD_MAXWIDGETS][256];
	int widget_highlight_thumb_id[STORYBOARD_MAXWIDGETS];
	char widget_selected_thumb[STORYBOARD_MAXWIDGETS][256]; //only for state change button, checkbox ...
	int widget_selected_thumb_id[STORYBOARD_MAXWIDGETS];
	char widget_click_sound[STORYBOARD_MAXWIDGETS][256];
	int widget_action[STORYBOARD_MAXWIDGETS];
	char widget_font[STORYBOARD_MAXWIDGETS][256];
	ImVec4 widget_font_color[STORYBOARD_MAXWIDGETS];
	float widget_font_size[STORYBOARD_MAXWIDGETS];
	int widget_type[STORYBOARD_MAXWIDGETS]; // but,text,image,video...
	int widget_read_only[STORYBOARD_MAXWIDGETS];
	int widget_layer[STORYBOARD_MAXWIDGETS];
	//int widget_output_pin[STORYBOARD_MAXWIDGETS]; //off,level,screen. 2023 - does not seem to be used
	int widget_initial_value[STORYBOARD_MAXWIDGETS]; //off,level,screen.
	char widget_name[STORYBOARD_MAXWIDGETS][256]; //for generating final images in titlebank/default/...
	//PE: Lets do some fillers, so when starting to save project we can just expend it later.
	//PE: You just add a new one, a line before the filler and cound the filler down.
	int screen_backdrop_transparent;
	int readouts_available = 0;
	int widgets_available = ALLOW_TEXT | ALLOW_IMAGE | ALLOW_VIDEO;
	int toggleKey = 0;
	int showAtStart = 0;
	int loop_music = 0;
	int iFiller20[14];
	float fFiller20[20];

	int universal_resolution[STORYBOARD_MAXOUTPUTS];
	int iFillerMaxOutputs20[19][STORYBOARD_MAXOUTPUTS];
	char FillerCharMaxOutput20[20][STORYBOARD_MAXOUTPUTS][256];
};

struct StoryboardStruct
{
	char sig[12] = "Storyboard\0";
	char gamename[256];
	int iStoryboardVersion = STORYBOARDVERSION; //PE: Just in case in the future if we need to convert this structure into a new struct.
	int iChanged;
	ImVec2 vEditorPanning = ImVec2(0.0f, 0.0f);
	StoryboardNodesStruct Nodes[STORYBOARD_MAXNODES];
	int NodeRadioButtonSelected[STORYBOARD_MAXNODES];
	float NodeSliderValues[STORYBOARD_MAXNODES][STORYBOARD_MAXWIDGETS];
	char game_icon[256];
	char game_thumb[256];
	char game_description[4096];
	char game_world_edge_text[1024]; //PE: ? should be a node ? so display screen directly ?
	int project_readonly;
	int game_thumb_id;
	int game_icon_id;
	char game_developer_desc[1024];
	// Have to add here instead of inside StoryboardNodesStruct, otherwise loaded storyboards would become corrupted
	ImVec4 widget_colors[STORYBOARD_MAXNODES][STORYBOARD_MAXWIDGETS];
	char widget_readout[STORYBOARD_MAXNODES][STORYBOARD_MAXWIDGETS][128];
	ImVec2 widget_textoffset[STORYBOARD_MAXNODES][STORYBOARD_MAXWIDGETS];
	int widget_ingamehidden[STORYBOARD_MAXNODES][STORYBOARD_MAXWIDGETS];
	int widget_drawordergroup[STORYBOARD_MAXNODES][STORYBOARD_MAXWIDGETS];

	char customprojectfolder[256];

	int project_inactive;
	//PE: To add new variables add them here, always add to botton of list.

};

#define STORYBOARD_MAXNODES202 50
#define STORYBOARD_MAXOUTPUTS202 20
#define STORYBOARD_MAXWIDGETS202 50

struct StoryboardNodesStruct202
{
	int type;
	int id;
	ImVec2 restore_position;
	int iEditEnable;
	int used;
	int thumb_id;
	char title[256];
	char levelnumber[256];
	char thumb[256];
	char level_name[256]; //PE: For custom lua scripts.
	char lua_name[256]; //PE: For custom lua scripts.
	char scene_name[256]; //PE: Scene temp files can have anykind of info, so just make each scene it own file.
	int input_id[STORYBOARD_MAXOUTPUTS202];
	int output_id[STORYBOARD_MAXOUTPUTS202];
	char output_action[STORYBOARD_MAXOUTPUTS202][256];
	char output_title[STORYBOARD_MAXOUTPUTS202][256];
	int output_linkto[STORYBOARD_MAXOUTPUTS202];
	int output_can_link_to_type[STORYBOARD_MAXOUTPUTS202];
	char input_title[STORYBOARD_MAXOUTPUTS202][256];
	char input_action[STORYBOARD_MAXOUTPUTS202][256];

	//Screen
	char screen_title[256];
	char screen_music[256];
	char screen_backdrop[256];
	int screen_backdrop_id;
	ImVec4 screen_back_color;
	int screen_backdrop_placement; // center,stretch,zoom.
	char screen_thumb[256];
	int screen_backdrop_ratio_placement[10]; // 0=1920x1080 center,stretch,zoom. 1=1366x768 center,stretch,zoom ...
	int screen_grid_size = 0;

	//Editor.
	int widget_used[STORYBOARD_MAXWIDGETS202];
	char widget_label[STORYBOARD_MAXWIDGETS202][256];
	ImVec2 widget_size[STORYBOARD_MAXWIDGETS202];
	ImVec2 widget_pos[STORYBOARD_MAXWIDGETS202];
	char widget_normal_thumb[STORYBOARD_MAXWIDGETS202][256];
	int widget_normal_thumb_id[STORYBOARD_MAXWIDGETS202];
	char widget_highlight_thumb[STORYBOARD_MAXWIDGETS202][256];
	int widget_highlight_thumb_id[STORYBOARD_MAXWIDGETS202];
	char widget_selected_thumb[STORYBOARD_MAXWIDGETS202][256]; //only for state change button, checkbox ...
	int widget_selected_thumb_id[STORYBOARD_MAXWIDGETS202];
	char widget_click_sound[STORYBOARD_MAXWIDGETS202][256];
	int widget_action[STORYBOARD_MAXWIDGETS202];
	char widget_font[STORYBOARD_MAXWIDGETS202][256];
	ImVec4 widget_font_color[STORYBOARD_MAXWIDGETS202];
	float widget_font_size[STORYBOARD_MAXWIDGETS202];
	int widget_type[STORYBOARD_MAXWIDGETS202]; // but,text,image,video...
	int widget_read_only[STORYBOARD_MAXWIDGETS202];
	int widget_layer[STORYBOARD_MAXWIDGETS202];
	//int widget_output_pin[STORYBOARD_MAXWIDGETS202]; //off,level,screen. 2023 - does not seem to be used
	int widget_initial_value[STORYBOARD_MAXWIDGETS202]; //off,level,screen.
	char widget_name[STORYBOARD_MAXWIDGETS202][256]; //for generating final images in titlebank/default/...
	//PE: Lets do some fillers, so when starting to save project we can just expend it later.
	//PE: You just add a new one, a line before the filler and cound the filler down.
	int screen_backdrop_transparent;
	int readouts_available = 0;
	int widgets_available = ALLOW_TEXT | ALLOW_IMAGE | ALLOW_VIDEO;
	int toggleKey = 0;
	int showAtStart = 0;
	int loop_music = 0;
	int iFiller20[14];
	float fFiller20[20];
	int universal_resolution[STORYBOARD_MAXOUTPUTS202];
	int iFillerMaxOutputs20[19][STORYBOARD_MAXOUTPUTS202];
	char FillerCharMaxOutput20[20][STORYBOARD_MAXOUTPUTS202][256];
};

struct StoryboardStruct202
{
	char sig[12] = "Storyboard\0";
	char gamename[256];
	int iStoryboardVersion = STORYBOARDVERSION; //PE: Just in case in the future if we need to convert this structure into a new struct.
	int iChanged;
	ImVec2 vEditorPanning = ImVec2(0.0f, 0.0f);
	StoryboardNodesStruct202 Nodes[STORYBOARD_MAXNODES202];
	int NodeRadioButtonSelected[STORYBOARD_MAXNODES202];
	float NodeSliderValues[STORYBOARD_MAXNODES202][STORYBOARD_MAXWIDGETS202];
	char game_icon[256];
	char game_thumb[256];
	char game_description[4096];
	char game_world_edge_text[1024]; //PE: ? should be a node ? so display screen directly ?
	int project_readonly;
	int game_thumb_id;
	int game_icon_id;
	char game_developer_desc[1024];
	// Have to add here instead of inside StoryboardNodesStruct, otherwise loaded storyboards would become corrupted
	ImVec4 widget_colors[STORYBOARD_MAXNODES202][STORYBOARD_MAXWIDGETS202];
	char widget_readout[STORYBOARD_MAXNODES202][STORYBOARD_MAXWIDGETS202][128];
	ImVec2 widget_textoffset[STORYBOARD_MAXNODES202][STORYBOARD_MAXWIDGETS202];
	int widget_ingamehidden[STORYBOARD_MAXNODES202][STORYBOARD_MAXWIDGETS202];
	int widget_drawordergroup[STORYBOARD_MAXNODES202][STORYBOARD_MAXWIDGETS202];

	char customprojectfolder[256];
};

//PE: SmallStoryboardStruct Not to be used, only for fast locating of thumbs.
struct SmallStoryboardStruct
{
	char sig[12] = "Storyboard\0";
	char gamename[256];
	int iStoryboardVersion = STORYBOARDVERSION; //PE: Just in case in the future if we need to convert this structure into a new struct.
	int iChanged;
	ImVec2 vEditorPanning = ImVec2(0.0f, 0.0f);
	StoryboardNodesStruct Nodes[SMALL_STORYBOARD_MAXNODES];
};


struct SliderData
{
	float fDefaultMaxValue; // fDefaultMaxValue is only used to speed up the search time for certain sliders.
	float fHighestMaxValue;
	char cID[256];
};
