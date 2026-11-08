//----------------------------------------------------
//--- GAMEGURU - M-Widget
//----------------------------------------------------

#include "stdafx.h"
#include "gameguru.h"

//PE: GameGuru IMGUI.
#include "..\Imgui\imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "..\Imgui\imgui_internal.h"
#include "..\Imgui\imgui_impl_win32.h"
#include "..\Imgui\imgui_gg_dx11.h"

#include ".\..\..\Guru-WickedMAX\wickedcalls.h"
bool widget_temp_disabled = false;
extern bool g_bRefreshRotationValuesFromObjectOnce;
#include "GGTerrain/GGTerrain.h"
using namespace GGTerrain;

#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

//PE: All selections has been changed from multiply system (PickScreenObject,PickScreen2D23D) to only use PickScreen2D23D.
//PE: This makes sure that a start selection using PickScreenObject, dont get changed to a PickScreen2D23D and make huge jumps/moves.
//PE: The PickScreenObject system has been kept if needed.
//PE: MAXMOVEPERSYNC set how far you max can move per sync.

//float debugobj = 0;
float MAXMOVEPERSYNC = 1000.0f; //When position objects, max move per sync.

// Prototypes
void gridedit_clearentityrubberbandlist ( void );
void RotateAndMoveRubberBand(int iActiveObj, float fMovedActiveObjectX, float fMovedActiveObjectY, float fMovedActiveObjectZ, GGQUATERNION quatRotationEvent);// float fMovedActiveObjectRX, float fMovedActiveObjectRY, float fMovedActiveObjectRZ);
void SetStartPositionsForRubberBand(int iActiveObj);

void SlowPositionObject(int iID, float fX, float fY, float fZ)
{
	if (t.inputsys.keyshift)
		MAXMOVEPERSYNC = 0.1f; //Fine adjust when holding SHIFT
	else
		MAXMOVEPERSYNC = 1000.0f; //PE: Disabled for now, this should be a config setting. when you need precise placement.

	float fCurX = ObjectPositionX(iID);
	float fCurY = ObjectPositionY(iID);
	float fCurZ = ObjectPositionZ(iID);
	float fNewX= fX, fNewY= fY, fNewZ = fZ;

	if (fNewX > fCurX + MAXMOVEPERSYNC) fNewX = fCurX + MAXMOVEPERSYNC;
	else if (fNewX < fCurX - MAXMOVEPERSYNC) fNewX = fCurX - MAXMOVEPERSYNC;
	else fNewX = fX;

	if (fNewY > fCurY + MAXMOVEPERSYNC) fNewY = fCurY + MAXMOVEPERSYNC;
	else if (fNewY < fCurY - MAXMOVEPERSYNC) fNewY = fCurY - MAXMOVEPERSYNC;
	else fNewY = fY;

	if (fNewZ > fCurZ + MAXMOVEPERSYNC) fNewZ = fCurZ + MAXMOVEPERSYNC;
	else if (fNewZ < fCurZ - MAXMOVEPERSYNC) fNewZ = fCurZ - MAXMOVEPERSYNC;
	else fNewZ = fZ;

	PositionObject(iID, fNewX, fNewY, fNewZ);
}

void widget_init ( void )
{
	// set widget up
	t.widget.pickedObject = 0;
	t.widget.activeObject = 0;
	t.widget.widgetXObj = g.widgetobjectoffset;
	t.widget.widgetYObj = g.widgetobjectoffset+1;
	t.widget.widgetZObj = g.widgetobjectoffset+2;
	t.widget.widgetXYObj = g.widgetobjectoffset+3;
	t.widget.widgetXZObj = g.widgetobjectoffset+4;
	t.widget.widgetYZObj = g.widgetobjectoffset+5;
	t.widget.widgetXRotObj = g.widgetobjectoffset+6;
	t.widget.widgetYRotObj = g.widgetobjectoffset+7;
	t.widget.widgetZRotObj = g.widgetobjectoffset+8;
	t.widget.widgetXScaleObj = g.widgetobjectoffset+9;
	t.widget.widgetYScaleObj = g.widgetobjectoffset+10;
	t.widget.widgetZScaleObj = g.widgetobjectoffset+11;
	t.widget.widgetXYZScaleObj = g.widgetobjectoffset+12;
	t.widget.widgetMAXObj = 12;
	t.widget.widgetPlaneObj = g.widgetobjectoffset+26;

	//  Load in media
	WickedCall_PresetObjectIgnoreTextures(true);
	cStr pWidgetPath = "editors\\gfx\\widget\\";
	LoadObject ( cStr(pWidgetPath+"widgetX.dbo").Get(),t.widget.widgetXObj );
	LoadObject ( cStr(pWidgetPath+"widgetY.dbo").Get(),t.widget.widgetYObj );
	LoadObject ( cStr(pWidgetPath+"widgetZ.dbo").Get(),t.widget.widgetZObj );
	LoadObject ( cStr(pWidgetPath+"widgetXY.dbo").Get(),t.widget.widgetXYObj );
	LoadObject ( cStr(pWidgetPath+"widgetXZ.dbo").Get(),t.widget.widgetXZObj );
	LoadObject ( cStr(pWidgetPath+"widgetYZ.dbo").Get(),t.widget.widgetYZObj );
	LoadObject ( cStr(pWidgetPath+"widgetXRot.dbo").Get(),t.widget.widgetXRotObj );
	LoadObject ( cStr(pWidgetPath+"widgetYRot.dbo").Get(),t.widget.widgetYRotObj );
	LoadObject ( cStr(pWidgetPath+"widgetZRot.dbo").Get(),t.widget.widgetZRotObj );
	LoadObject ( cStr(pWidgetPath+"widgetXScale.dbo").Get(),t.widget.widgetXScaleObj );
	LoadObject ( cStr(pWidgetPath+"widgetYScale.dbo").Get(),t.widget.widgetYScaleObj );
	LoadObject ( cStr(pWidgetPath+"widgetZScale.dbo").Get(),t.widget.widgetZScaleObj );
	
	// scale all object
	MakeObjectCube (  t.widget.widgetXYZScaleObj,2 );

	// give widgets a base texture
	TextureObject ( t.widget.widgetXObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetYObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetZObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetXYObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetXZObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetYZObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetXRotObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetYRotObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetZRotObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetXScaleObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetYScaleObj, g.editorimagesoffset+14 );
	TextureObject ( t.widget.widgetZScaleObj, g.editorimagesoffset+14 );

	// Colorize the widget gadgets
	SetObjectEmissive ( t.widget.widgetXObj, Rgb(255,0,0) );
	SetObjectEmissive ( t.widget.widgetYObj, Rgb(0,255,0) );
	SetObjectEmissive ( t.widget.widgetZObj, Rgb(0,0,255) );
	SetObjectEmissive ( t.widget.widgetXYObj, Rgb(255,0,0) );
	SetObjectEmissive ( t.widget.widgetXZObj, Rgb(0,255,0) );
	SetObjectEmissive ( t.widget.widgetYZObj, Rgb(0,0,255) );
	SetObjectEmissive ( t.widget.widgetXRotObj, Rgb(255,0,0) );
	SetObjectEmissive ( t.widget.widgetYRotObj, Rgb(0,255,0) );
	SetObjectEmissive ( t.widget.widgetZRotObj, Rgb(0,0,255) );
	SetObjectEmissive ( t.widget.widgetXScaleObj, Rgb(255,0,0) );
	SetObjectEmissive ( t.widget.widgetYScaleObj, Rgb(0,255,0) );
	SetObjectEmissive ( t.widget.widgetZScaleObj, Rgb(0,0,255) );
	SetObjectEmissive ( t.widget.widgetXYZScaleObj, Rgb(255,255,255) );

	// Widget object render settings
	SetObjectDiffuse ( t.widget.widgetXObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetYObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetZObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetXYObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetXZObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetYZObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetXRotObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetYRotObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetZRotObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetXScaleObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetYScaleObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetZScaleObj, Rgb(0,0,0) );
	SetObjectDiffuse ( t.widget.widgetXYZScaleObj, Rgb(0,0,0) );
	SetObjectTransparency ( t.widget.widgetXObj, 6 );
	SetObjectTransparency ( t.widget.widgetYObj, 6 );
	SetObjectTransparency ( t.widget.widgetZObj, 6 );
	SetObjectTransparency ( t.widget.widgetXYObj, 6 );
	SetObjectTransparency ( t.widget.widgetXZObj, 6 );
	SetObjectTransparency ( t.widget.widgetYZObj, 6 );
	SetObjectTransparency ( t.widget.widgetXRotObj, 6 );
	SetObjectTransparency ( t.widget.widgetYRotObj, 6 );
	SetObjectTransparency ( t.widget.widgetZRotObj, 6 );
	SetObjectTransparency ( t.widget.widgetXScaleObj, 6 );
	SetObjectTransparency ( t.widget.widgetYScaleObj, 6 );
	SetObjectTransparency ( t.widget.widgetZScaleObj, 6 );
	SetObjectTransparency ( t.widget.widgetXYZScaleObj, 6 );

	// hide all widget objects
	for ( t.a = 0 ; t.a <= t.widget.widgetMAXObj; t.a++ )
	{
		if (ObjectExist(g.widgetobjectoffset + t.a))
		{
			HideObject (g.widgetobjectoffset + t.a);
			sObject* pObject = GetObjectData(g.widgetobjectoffset + t.a);
			WickedCall_SetObjectRenderOrderBias(pObject, -600);
		}
	}
	extern bool bTriggerVisibleWidget;
	bTriggerVisibleWidget = false;

	// create a common widget plane we can use to detect mouse and control objects with widget gadgets
	WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_WIDGETPLANE);
	MakeObjectBox (t.widget.widgetPlaneObj, 5000000, 5000000, 1);
	WickedCall_PresetObjectRenderLayer(GGRENDERLAYERS_NORMAL);
	WickedCall_PresetObjectIgnoreTextures(false);
	RotateObject ( t.widget.widgetPlaneObj,-90,0,0 );
	HideObject ( t.widget.widgetPlaneObj );

	// Offset to rotation objects
	t.twidgetRotStartObject = g.widgetobjectoffset+30;

	// create widget helper objects
	MakeObjectSphere ( t.twidgetRotStartObject+11,60  ); HideObject (  t.twidgetRotStartObject+11 );
	MakeObjectSphere ( t.twidgetRotStartObject+12,70  ); HideObject (  t.twidgetRotStartObject+12 );
	MakeObjectSphere ( t.twidgetRotStartObject+13,65  ); HideObject (  t.twidgetRotStartObject+13 );
	MakeObjectPlane ( t.twidgetRotStartObject+14,10000,10000  ); HideObject (  t.twidgetRotStartObject+14 );

	// widget vector and matrices
	t.r=MakeVector3(g.widgetvectorindex+1);
	t.r=MakeMatrix(g.widgetStartMatrix+2);
	t.r=MakeMatrix(g.widgetStartMatrix+3);
	t.r=MakeMatrix(g.widgetStartMatrix+4);
	t.r=MakeMatrix(g.widgetStartMatrix+5);
	t.r=MakeVector3(g.widgetvectorindex+6);
	t.r=MakeVector3(g.widgetvectorindex+7);
	t.r=MakeVector3(g.widgetvectorindex+8);
	t.r=MakeVector3(g.widgetvectorindex+9);
	t.r=MakeVector3(g.widgetvectorindex+10);
	t.r=MakeVector3(g.widgetvectorindex+11);
	t.r=MakeVector3(g.widgetvectorindex+12);

	// reset var to store when widget gadget selected
	t.widget.pickedSection = 0;
}

void widget_movezonesandlights(int e)
{
	// handle zones if moved
	t.waypointindex = t.entityelement[e].eleprof.trigger.waypointzoneindex;
	t.thisx_f = t.entityelement[e].x;
	t.thisy_f = t.entityelement[e].y;
	t.thisz_f = t.entityelement[e].z;
	waypoint_movetothiscoordinate();

	// also update light data for this entity
	if (t.entityprofile[t.entityelement[e].bankindex].ismarker == 2)
	{
		int iLightIndex = t.entityelement[e].eleprof.light.index;
		if (iLightIndex >= 0)
		{
			t.infinilight[iLightIndex].x = t.entityelement[e].x;
			t.infinilight[iLightIndex].y = t.entityelement[e].y;
			t.infinilight[iLightIndex].z = t.entityelement[e].z;
			t.infinilight[iLightIndex].is_spot_light = t.entityelement[e].eleprof.usespotlighting;
			t.infinilight[iLightIndex].range = t.entityelement[e].eleprof.light.range;
			t.infinilight[iLightIndex].spotlightradius = t.entityelement[e].eleprof.light.offsetup;
			t.infinilight[iLightIndex].fLightHasProbe = t.entityelement[e].eleprof.light.fLightHasProbe;
			if (t.entityelement[e].eleprof.castshadow == 1)
				t.infinilight[iLightIndex].bCanShadow = false;
			else
				t.infinilight[iLightIndex].bCanShadow = true;
			t.infinilight[iLightIndex].colrgb.r = RgbR(t.entityelement[e].eleprof.light.color);
			t.infinilight[iLightIndex].colrgb.g = RgbG(t.entityelement[e].eleprof.light.color);
			t.infinilight[iLightIndex].colrgb.b = RgbB(t.entityelement[e].eleprof.light.color);
			sObject* pObject = g_ObjectList[t.entityelement[e].obj];
			if (pObject)
			{
				//PE: In wicked we use the rotation vector for spot light.
				t.infinilight[iLightIndex].f_angle_x = pObject->position.vecRotate.x;
				t.infinilight[iLightIndex].f_angle_y = pObject->position.vecRotate.y;
				t.infinilight[iLightIndex].f_angle_z = pObject->position.vecRotate.z;
			}
		}
	}

	// also update if particle emitter
	entity_updateparticleemitter(e);
	entity_updateautoflatten(e);
}

int PickObjectUsingWicked(void)
{
	//PE: Dont change anything when right mouse down.
	if (ImGui::IsMouseDown(1)) return 0;

	//PE: We was sending two very very slow WickedCall_GetPick calls per frame (GGRENDERLAYERS_NORMAL), now reuse if we already sent one. mouse coords has not changed ...
	extern int iReusePickObjectID;
	extern sObject* pReusePickObject;
	if (iReusePickObjectID != -1)
	{
		//We already sent a ray.
		if (pReusePickObject)
		{
			sObject* pHitObject = pReusePickObject;
			if (pHitObject)
			{
				int iObjectID = pHitObject->dwObjectNumber;
				if (iObjectID >= g.widgetobjectoffset + 0 && iObjectID <= g.widgetobjectoffset + t.widget.widgetMAXObj)
				{
					return iObjectID;
				}
			}
		}
		return(0);
	}
	uint64_t hitentity = 0;
	float fHitX, fHitY, fHitZ;
	WickedCall_GetPick(&fHitX, &fHitY, &fHitZ, NULL, NULL, NULL, &hitentity, GGRENDERLAYERS_NORMAL);
	if (hitentity > 0)
	{
		// found object under hovering cursor, match to entity index
		sObject* pHitObject = m_ObjectManager.FindObjectFromWickedObjectEntityID(hitentity);
		if (pHitObject)
		{
			int iObjectID = pHitObject->dwObjectNumber;
			if (iObjectID >= g.widgetobjectoffset + 0 && iObjectID <= g.widgetobjectoffset + t.widget.widgetMAXObj)
			{
				return iObjectID;
			}
		}
	}
	return 0;
}

bool widget_getplanepos ( float fActivePosX, float fActivePosY, float fActivePosZ, float* pPlanePosX, float* pPlanePosY, float* pPlanePosZ )
{
	#ifdef OPTICK_ENABLE
		OPTICK_EVENT();
	#endif

	// when drag in object initially, use new smart placement system
	extern int iObjectMoveMode;
	bool bJustForInitialDragIn = false;
	extern bool bDraggingActiveInitial;
	if(bDraggingActiveInitial==true )
		bJustForInitialDragIn = true;

	bool bPlanePosRegistered = false;
	if (ObjectExist(t.widget.widgetPlaneObj))
	{
		PositionObject(t.widget.widgetPlaneObj, fActivePosX, fActivePosY, fActivePosZ);
		if (t.widget.pickedSection == -99)
		{
			float fUpDownAngle = WrapValue(CameraAngleX(0));
			if (iObjectMoveMode == 0 && bJustForInitialDragIn == false)
			{
				// for XZ mode, always use horizon plane (after initial drag in)
				fUpDownAngle = 45.0f;
			}
			// if facing to the side, and in smart mode, prefer floor plane
			int iForwardFacing = t.entityprofile[t.gridentity].forwardfacing;
			if (bDraggingActiveInitial == false)
			{
				if (fUpDownAngle > 10.0f && fUpDownAngle < 350.0f)
				{
					// as you were
				}
				else
				{
					if (iObjectMoveMode == 2 && iForwardFacing == 0)
					{
						// prefer floor plane for ground and wall objects
						fUpDownAngle = 45.0f;
					}
					else
					{
						// ceiling objects keep to up and down
					}
				}
			}
			if (bDraggingActiveInitial == false && fUpDownAngle > 10.0f && fUpDownAngle < 350.0f)
			{
				// a horizontal plane for up/down views
				if (iForwardFacing == 0)
				{
					RotateObject(t.widget.widgetPlaneObj, -90, 0, 0);
				}
				if (iForwardFacing == 1)
				{
					// plane takes on angle of forward facing object (which can orient to the ray detected jormal direction)
					SetObjectToObjectOrientation(t.widget.widgetPlaneObj, t.gridentityobj);
				}
				if (iForwardFacing == 2)
				{
					RotateObject(t.widget.widgetPlaneObj, -90, 0, 0);
				}
			}
			else
			{
				// a vertical plane for sidelong views
				RotateObject(t.widget.widgetPlaneObj, 0, CameraAngleY(0), 0);
			}
		}
		if (t.widget.pickedSection == -98)
		{
			// will provide good Y up and down no matter the angle!
			RotateObject(t.widget.widgetPlaneObj, 0, CameraAngleY(0), 0);
		}
		if (t.widget.pickedSection == t.widget.widgetXObj) RotateObject(t.widget.widgetPlaneObj, -90, 0, 0);
		if (t.widget.pickedSection == t.widget.widgetYObj) RotateObject(t.widget.widgetPlaneObj, 0, 0, 0);
		if (t.widget.pickedSection == t.widget.widgetZObj) RotateObject(t.widget.widgetPlaneObj, -90, 0, 0);
		if (t.widget.pickedSection == t.widget.widgetXYObj) RotateObject(t.widget.widgetPlaneObj, 0, 0, 0);
		if (t.widget.pickedSection == t.widget.widgetXZObj) RotateObject(t.widget.widgetPlaneObj, -90, 0, 0);
		if (t.widget.pickedSection == t.widget.widgetYZObj) RotateObject(t.widget.widgetPlaneObj, 0, 90, 0);
		if (t.widget.pickedSection == t.widget.widgetYRotObj) RotateObject(t.widget.widgetPlaneObj, 90, 0, 0);
		if (t.widget.pickedSection == t.widget.widgetXRotObj) RotateObject(t.widget.widgetPlaneObj, 0, 90, 0);
		if (t.widget.pickedSection == t.widget.widgetZRotObj) RotateObject(t.widget.widgetPlaneObj, 0, 0, 0);
		if (t.widget.pickedSection == t.widget.widgetXScaleObj) PointObject(t.widget.widgetPlaneObj, CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0));
		if (t.widget.pickedSection == t.widget.widgetYScaleObj) PointObject(t.widget.widgetPlaneObj, CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0));
		if (t.widget.pickedSection == t.widget.widgetZScaleObj) PointObject(t.widget.widgetPlaneObj, CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0));
		if (t.widget.pickedSection == t.widget.widgetXYZScaleObj) PointObject(t.widget.widgetPlaneObj, CameraPositionX(0), CameraPositionY(0), CameraPositionZ(0));

		WickedCall_UpdateObject(GetObjectData(t.widget.widgetPlaneObj));
		WickedCall_UpdateSceneForPick();
		ShowObject(t.widget.widgetPlaneObj); //PE: Need to be visible while pick.
		int layer = GGRENDERLAYERS_WIDGETPLANE;
		bPlanePosRegistered = WickedCall_GetPick(pPlanePosX, pPlanePosY, pPlanePosZ, NULL, NULL, NULL, NULL, layer);
		
		//
		// A good way to debug what is being clicked and coordinates returned :)
		//
		HideObject(t.widget.widgetPlaneObj);
		
		if (t.widget.pickedSection == t.widget.widgetXObj) { *pPlanePosZ = fActivePosZ; *pPlanePosY = fActivePosY; }
		if (t.widget.pickedSection == t.widget.widgetYObj) { *pPlanePosX = fActivePosX; *pPlanePosZ = fActivePosZ; }
		if (t.widget.pickedSection == t.widget.widgetZObj) { *pPlanePosX = fActivePosX; *pPlanePosY = fActivePosY; }
		if (t.widget.pickedSection == t.widget.widgetXYObj) { *pPlanePosZ = fActivePosZ; }
		if (t.widget.pickedSection == t.widget.widgetXZObj) { *pPlanePosY = fActivePosY; }
		if (t.widget.pickedSection == -99)
		{
			if ( iObjectMoveMode == 2 || bJustForInitialDragIn == true )
			{
				// allow smart mode 2 full access to plane XYZ
			}
			else
			{
				if (iObjectMoveMode == 1)
				{
					// also do NOT reset the Y result when trying to move vertically!
				}
				else
				{
					*pPlanePosY = fActivePosY;
				}
			}
		}
		if (t.widget.pickedSection == t.widget.widgetYZObj) { *pPlanePosX = fActivePosX; }
	}
	return bPlanePosRegistered;
}

void widget_cancelplanerotation( float *pPlanePosX, float *pPlanePosY, float *pPlanePosZ)
{
	float fDet;
	GGVECTOR3 vecRelPlanePos = GGVECTOR3(*pPlanePosX, *pPlanePosY, *pPlanePosZ);
	sObject* pObjPtr = GetObjectData(t.widget.widgetPlaneObj);
	GGMATRIX matInvPlaneRotation = pObjPtr->position.matRotation;
	GGMatrixInverse(&matInvPlaneRotation, &fDet, &matInvPlaneRotation);
	GGVec3TransformCoord(&vecRelPlanePos, &vecRelPlanePos, &matInvPlaneRotation);
	*pPlanePosX = vecRelPlanePos.x;
	*pPlanePosY = vecRelPlanePos.y;
	*pPlanePosZ = vecRelPlanePos.z;
}

void widget_loop ( void )
{
	// fixed camera projection
	SetCurrentCamera (  0 );
	SetCameraRange (  DEFAULT_NEAR_PLANE, DEFAULT_FAR_PLANE );

	// real widget mouse X Y
	if ( t.widget.protoineffect == 1 ) 
	{
		t.widgetinputsysxmouse_f=t.inputsys.xmouse;
		t.widgetinputsysymouse_f=t.inputsys.ymouse;
	}
	else
	{
		// only update if mouse within 3D view
		if ( t.inputsys.xmouse != 500000 )
		{
			#if defined(ENABLEIMGUI) && !defined(USEOLDIDE) 
			//PE: imgui Need testing.
			t.widgetinputsysxmouse_f = ((float)t.inputsys.xmouse / (float)GetDisplayWidth()) / ((float)GetDisplayWidth() / (float)GetChildWindowWidth(-1));
			t.widgetinputsysymouse_f = ((float)t.inputsys.ymouse / (float)GetDisplayHeight()) / ((float)GetDisplayHeight() / (float)GetChildWindowHeight(-1));
			#else
			//  work out visible part of full backbuffer (i.e. 1212 of 1360)
			t.widgetinputsysxmouse_f=(GetDisplayWidth()+0.0)/(GetChildWindowWidth()+0.0);
			t.widgetinputsysymouse_f=(GetDisplayHeight()+0.0)/(GetChildWindowHeight()+0.0);
			//  scale full mouse to fit in visible area
			t.widgetinputsysxmouse_f=((t.inputsys.xmouse+0.0)/800.0)/t.widgetinputsysxmouse_f;
			t.widgetinputsysymouse_f=((t.inputsys.ymouse+0.0)/600.0)/t.widgetinputsysymouse_f;
			#endif
			//  then provide in a format for the pick-from-screen command
			#ifdef DX11
			t.widgetinputsysxmouse_f=t.widgetinputsysxmouse_f*(GetDisplayWidth()+0.0);
			t.widgetinputsysymouse_f=t.widgetinputsysymouse_f*(GetDisplayHeight()+0.0);
			#else
			t.widgetinputsysxmouse_f=t.widgetinputsysxmouse_f*(GetChildWindowWidth()+0.0);
			t.widgetinputsysymouse_f=t.widgetinputsysymouse_f*(GetChildWindowHeight()+0.0);
			#endif
		}
	}

	// highlighter
	if ( t.widget.activeObject>0 ) 
	{
		if ( t.widget.pickedSection != 0  ) t.tdim_f = 15; else t.tdim_f = 50;
		for ( t.a = 0 ; t.a <= t.widget.widgetMAXObj; t.a++ )
		{
			if ( ObjectExist(g.widgetobjectoffset+t.a) ) SetAlphaMappingOn ( g.widgetobjectoffset+t.a, t.tdim_f );
		}
		if ( t.widget.pickedSection != 0 ) 
		{
			t.thighlighterobj = t.widget.pickedSection;
		}
		else
		{
			t.thighlighterobj = PickObjectUsingWicked();
		}
		if (t.thighlighterobj > 0)
		{
			SetAlphaMappingOn(t.thighlighterobj, 75.0);
		}
	}
	else
	{
		//  ensure no widget button detection if no active object
		t.thighlighterobj=0;
		t.widget.pickedSection=0;
	}

	//  If no picked object and the widget has been in use, switch it off
	widget_updatewidgetobject ( );

	// Ensure any old widget object ptr is updated
	if ( t.widget.activeObject>0 ) 
	{
		if ( ObjectExist(t.widget.activeObject) == 0 ) 
		{
			t.widget.activeObject=0;
		}
	}

	extern bool Shooter_Tools_Window_Active;
	extern preferences pref;
	extern bool bTriggerVisibleWidget;
	if (Shooter_Tools_Window_Active || (pref.iEnableDragDropEntityMode && !bTriggerVisibleWidget))
	{
		//Hide for now.
		if (!widget_temp_disabled)
		{
			for (t.a = 0; t.a <= t.widget.widgetMAXObj; t.a++)
			{
				if (ObjectExist(g.widgetobjectoffset + t.a)) HideObject(g.widgetobjectoffset + t.a);
			}
			widget_temp_disabled = true;
			extern bool bTriggerVisibleWidget;
			bTriggerVisibleWidget = false;
		}
	}

	if (t.widget.pickedEntityIndex > 0 && t.entityelement[t.widget.pickedEntityIndex].editorlock == 1)
	{
		//PE: hide. but keep t.widget.pickedEntityIndex for properties.
		for (t.a = 0; t.a <= t.widget.widgetMAXObj; t.a++)
		{
			if (ObjectExist(g.widgetobjectoffset + t.a)) HideObject(g.widgetobjectoffset + t.a);
		}
		extern bool bTriggerVisibleWidget;
		bTriggerVisibleWidget = false;
	}

	// check if the widget is needed
	if ( t.widget.activeObject == 0 )
	{
		widget_check_for_new_object_selection ( );
	}
	else
	{
		if ( (!Shooter_Tools_Window_Active && (!pref.iEnableDragDropEntityMode || bTriggerVisibleWidget ) )  && widget_temp_disabled)
		{
			widget_temp_disabled = false;
			widget_show_widget();
		}
		// Setup positions for widget objects
		if ( ObjectExist(t.widget.activeObject) == 1 ) 
		{
			for ( t.a = 0 ; t.a <= t.widget.widgetMAXObj; t.a++ )
			{
				if ( ObjectExist (g.widgetobjectoffset+t.a) )  
				{
					if ( GetVisible (g.widgetobjectoffset+t.a) )
					{
						PositionObject(g.widgetobjectoffset + t.a, CameraPositionX(), CameraPositionY(), CameraPositionZ());
						if ( t.widget.mode == 0 )
							PointObject(g.widgetobjectoffset + t.a, ObjectPositionX(t.widget.activeObject) + t.widget.offsetx, ObjectPositionY(t.widget.activeObject) + t.widget.offsety, ObjectPositionZ(t.widget.activeObject) + t.widget.offsetz);
						else
							PointObject(g.widgetobjectoffset + t.a, ObjectPositionX(t.widget.activeObject) + 0, ObjectPositionY(t.widget.activeObject) + 0, ObjectPositionZ(t.widget.activeObject) + 0);
						MoveObject(g.widgetobjectoffset + t.a, 40);
						RotateObject(g.widgetobjectoffset + t.a, 0, 0, 0);
					}
					else
					{
						PositionObject(g.widgetobjectoffset + t.a, -100000, -100000, -100000);
					}
				}
			}

			//  detect if widget panel off screen, and shift back in
			widget_correctwidgetpanel ( );

			// orient widget rot gadgets to active object
			RotateObject(g.widgetobjectoffset + 6, 0, 0, 0);
			RotateObject(g.widgetobjectoffset + 7, 0, 0, 0);
			RotateObject(g.widgetobjectoffset + 8, 0, 0, 0);

			// orient widget scale gadgets to active object
			RotateObject(g.widgetobjectoffset + 9, ObjectAngleX(t.widget.activeObject), ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject));
			RotateObject(g.widgetobjectoffset + 10, ObjectAngleX(t.widget.activeObject), ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject));
			RotateObject(g.widgetobjectoffset + 11, ObjectAngleX(t.widget.activeObject), ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject));
			RotateObject(g.widgetobjectoffset + 12, ObjectAngleX(t.widget.activeObject), ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject));
		}

		//  If we havent picked a section of the widget, lets test for it
		if ( t.widget.pickedSection == 0 )//|| g.fForceYRotationOfRubberBandFromKeyPress > 0.0f )
		{
			//  Test for the widget selection keys X,C and V
			t.tscancode = t.inputsys.kscancode;
			if (  t.widget.oldScanCode !=  t.tscancode ) 
			{
				t.toldmode = t.widget.mode;
				if (  t.tscancode  ==  WIDGET_KEY_TRANSLATE  )  t.widget.mode  =  0;
				if (  t.tscancode  ==  WIDGET_KEY_ROTATE  )  t.widget.mode  =  1;
				if (  t.tscancode  ==  WIDGET_KEY_SCALE  )  t.widget.mode  =  2;
				if ( t.toldmode != t.widget.mode ) widget_show_widget ( );
			}
			t.widget.oldScanCode = t.tscancode;

			// just click
			if ( t.inputsys.mclick == 1 && t.widget.oldMouseClick == 0 ) t.widget.mclickpress = 2;
				
			if (t.widget.mclickpress == 2)
			{
				// See if a section has been chosen
				t.widget.pickedSection = PickObjectUsingWicked();
				if (t.widget.pickedSection > 0)
				{
					//Make sure to highlight all objects the object belong to.
					extern void CheckGroupListForRubberbandSelections(int entityindex);
					if (t.widget.pickedEntityIndex > 0)
						CheckGroupListForRubberbandSelections(t.widget.pickedEntityIndex);
				}

				//  as soon as click down, record for possible undo
				if ( t.widget.pickedSection > 0 )
				{
					// but only if we select a gadget (if click nothing we keep older position)
					t.sttentitytoselect=t.tentitytoselect;
					t.tentitytoselect=t.widget.pickedEntityIndex;
					if ( g.entityrubberbandlist.size() > 0 )
						gridedit_moveentityrubberband();
					else
					{
						entity_createundoaction(eUndoSys_Object_ChangePosRotScl, t.tentitytoselect);
					}
					t.tentitytoselect=t.sttentitytoselect;
				}

				// Translation, rotation and scale
				t.widget.mclickpress=0;
				t.toriginalTranslateClickDeferred = 3;
				t.toriginalTranslateClickX_f = 0;
				t.toriginalTranslateClickY_f = 0;
				t.toriginalTranslateClickZ_f = 0;
				t.toriginalRotationClickX_f = 0;
				t.toriginalRotationClickY_f = 0;
				t.toriginalRotationClickZ_f = 0;
				float fPlanePosX, fPlanePosY, fPlanePosZ;
				float fActivePosX = ObjectPositionX(t.widget.activeObject) + 0;
				float fActivePosY = ObjectPositionY(t.widget.activeObject) + 0;
				float fActivePosZ = ObjectPositionZ(t.widget.activeObject) + 0;
				if (t.widget.mode == 0)
				{
					fActivePosX = ObjectPositionX(t.widget.activeObject) + t.widget.offsetx;
					fActivePosY = ObjectPositionY(t.widget.activeObject) + t.widget.offsety;
					fActivePosZ = ObjectPositionZ(t.widget.activeObject) + t.widget.offsetz;
				}
				bool bPlanePosRegistered = widget_getplanepos(fActivePosX, fActivePosY, fActivePosZ, &fPlanePosX, &fPlanePosY, &fPlanePosZ );
				if (bPlanePosRegistered == true)
				{
					//LB: also use t.toriginalTranslateClickDeferred as the REAL plane pos is not good until NEXT cycle (for some reason)
					// record position where clicked
					t.toriginalTranslateClickX_f = fPlanePosX - fActivePosX;
					t.toriginalTranslateClickY_f = fPlanePosY - fActivePosY;
					t.toriginalTranslateClickZ_f = fPlanePosZ - fActivePosZ;
				}

				// record entity RY for ragdoll/character rotation code lower down
				t.tlastgoody_f = ObjectAngleY(t.widget.activeObject);

				// leelee, move below once it works so this code is not duplicated!! (also duplicated in gridedit for right panel -argzz)
				// record all current offsets from primary widget object
				bool bDisableRubberBandMoving = false;
				extern int current_selected_group;
				extern bool group_editing_on;
				if (current_selected_group >= 0 && group_editing_on)
				{
					bDisableRubberBandMoving = true;
				}
				if (!bDisableRubberBandMoving)
				{
					// storing offsets in g.entityrubberbandlist[i].x/y/z/quat
					SetStartPositionsForRubberBand(t.widget.activeObject);
				}
				// No widget property popup

				// No section selected, so switch the widget off
				if ( t.widget.pickedSection == 0 )//&& g.fForceYRotationOfRubberBandFromKeyPress == 0.0f ) 
				{
					t.widget.activeObject = 0;
					t.widget.pickedObject = 0;
					if ( t.widget.activeObject == 0 ) 
					{
						for ( t.a = 0 ; t.a <= t.widget.widgetMAXObj; t.a++ )
						{
							if (  ObjectExist(g.widgetobjectoffset+t.a) ) 
							{
								PositionObject (  g.widgetobjectoffset+t.a,-100000,-100000,-100000 );
								HideObject (  g.widgetobjectoffset+t.a );
							}
						}
						extern bool bTriggerVisibleWidget;
						bTriggerVisibleWidget = false;
					}
				}
			}
		}
		if ( t.widget.pickedSection == 0 )//&& g.fForceYRotationOfRubberBandFromKeyPress == 0 )
		{
			// code done above, needed here to handle g.fForceYRotationOfRubberBandFromKeyPress logic
		}
		else
		{
			// store old active object position (for later if we need to also move rubber band highlighted objects)
			float fOldActiveObjectX = ObjectPositionX ( t.widget.activeObject );
			float fOldActiveObjectY = ObjectPositionY ( t.widget.activeObject );
			float fOldActiveObjectZ = ObjectPositionZ ( t.widget.activeObject );
			{
				// defer detect system to fix the issue of the plane not being ready to return correct XYZ until frame AFTER plane is positioned
				if (t.toriginalTranslateClickDeferred > 0) t.toriginalTranslateClickDeferred--;
				// position
				if ( t.widget.mode == 0 && ObjectExist(t.widget.activeObject) == 1 && t.toriginalTranslateClickDeferred <= 1) //LB: mclickpress ensures this is not called in same cycle as the click!
				{
					float fPlanePosX, fPlanePosY, fPlanePosZ;
					float fActivePosX = ObjectPositionX(t.widget.activeObject) + t.widget.offsetx;
					float fActivePosY = ObjectPositionY(t.widget.activeObject) + t.widget.offsety;
					float fActivePosZ = ObjectPositionZ(t.widget.activeObject) + t.widget.offsetz;
					bool bPlanePosRegistered = widget_getplanepos(fActivePosX, fActivePosY, fActivePosZ, &fPlanePosX, &fPlanePosY, &fPlanePosZ );
					if (bPlanePosRegistered == true)
					{
						//LB: for some reason the plane 'shifted' on the second cycle, so update toriginalTranslateClickXYZ to account!
						if (t.toriginalTranslateClickDeferred == 1)
						{
							t.toriginalTranslateClickX_f = fPlanePosX - fActivePosX;
							t.toriginalTranslateClickY_f = fPlanePosY - fActivePosY;
							t.toriginalTranslateClickZ_f = fPlanePosZ - fActivePosZ;
							t.toriginalTranslateClickDeferred = 0;
						}

						// offset pos on plane with difference between widget click pos and active object
						fPlanePosX -= (t.toriginalTranslateClickX_f + t.widget.offsetx);
						fPlanePosY -= (t.toriginalTranslateClickY_f + t.widget.offsety);
						fPlanePosZ -= (t.toriginalTranslateClickZ_f + t.widget.offsetz);
						if (t.widget.pickedEntityIndex > 0 ) // allow all pos adjustments to be grid/snappable  && (t.widget.pickedSection == t.widget.widgetXZObj || t.widget.pickedSection == t.widget.widgetXObj || t.widget.pickedSection == t.widget.widgetZObj ) )
						{
							int oldte = t.e;
							int oldtgridentity = t.gridentity;
							int oldtgridentityobj = t.gridentityobj;
							float oldtgridentityposx_f = t.gridentityposx_f;
							float oldtgridentityposy_f = t.gridentityposy_f;
							float oldtgridentityposz_f = t.gridentityposz_f;

							t.e = t.widget.pickedEntityIndex;
							t.gridentity = t.entityelement[t.e].bankindex;
							t.gridentityobj = t.widget.activeObject;
							t.gridentityposx_f = fPlanePosX;
							t.gridentityposy_f = fPlanePosY;
							t.gridentityposz_f = fPlanePosZ;


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

							if (bObjectOutSideEditArea)
							{
								//Trigger warning.
								extern char cSmallTriggerMessage[MAX_PATH];
								extern int iTriggerMessageFrames;
								extern bool bTriggerSmallMessage;
								sprintf(cSmallTriggerMessage, "Object is Outside Editable Area");
								iTriggerMessageFrames = 60;
								bTriggerSmallMessage = true;
							}

							void Add_Grid_Snap_To_Position(bool bFromWidgetMode);
							Add_Grid_Snap_To_Position(true);

							fPlanePosX = t.gridentityposx_f;
							fPlanePosY = t.gridentityposy_f;
							fPlanePosZ = t.gridentityposz_f;

							t.e = oldte;
							t.gridentity = oldtgridentity;
							t.gridentityobj = oldtgridentityobj;
							t.gridentityposx_f = oldtgridentityposx_f;
							t.gridentityposy_f = oldtgridentityposy_f;
							t.gridentityposz_f = oldtgridentityposz_f;
						}

						// update active object position from planeposition detected 
						PositionObject(t.widget.activeObject, fPlanePosX, fPlanePosY, fPlanePosZ);

						// store in entity element
						t.te = t.widget.pickedEntityIndex;
						t.entityelement[t.te].x = ObjectPositionX(t.widget.activeObject);
						t.entityelement[t.te].y = ObjectPositionY(t.widget.activeObject);
						t.entityelement[t.te].z = ObjectPositionZ(t.widget.activeObject);

						// move zones and lights if in group
						widget_movezonesandlights ( t.te );

						bool bDisableRubberBandMoving = false;
						extern int current_selected_group;
						extern bool group_editing_on;
						if (current_selected_group >= 0 && group_editing_on)
						{
							bDisableRubberBandMoving = true;
						}
						if (!bDisableRubberBandMoving)
						{
							// if we need to also move rubber band highlighted objects, do so now
							if (g.entityrubberbandlist.size() > 0)
							{
								float fMovedActiveObjectX = ObjectPositionX(t.widget.activeObject) - fOldActiveObjectX;
								float fMovedActiveObjectY = ObjectPositionY(t.widget.activeObject) - fOldActiveObjectY;
								float fMovedActiveObjectZ = ObjectPositionZ(t.widget.activeObject) - fOldActiveObjectZ;
								for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
								{
									int e = g.entityrubberbandlist[i].e;
									int tobj = t.entityelement[e].obj;
									if (tobj > 0 && t.entityelement[e].editorlock == 0 )
									{
										if (ObjectExist(tobj) == 1)
										{
											if (tobj != t.widget.activeObject)
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
					}
				}
			}

			// only allow rot/scl for non markers
			t.te = t.widget.pickedEntityIndex;
			t.tttentid = t.entityelement[t.te].bankindex;

			// some markers and spotlight have Y rotation
			t.thaveyrot = 0; 
			if ( t.entityprofile[t.tttentid].ismarker == 1 || t.entityprofile[t.tttentid].ismarker == 6 || t.entityprofile[t.tttentid].ismarker == 7  )  t.thaveyrot = 1;
			if ( t.entityelement[t.te].eleprof.usespotlighting ) t.thaveyrot = 1;
			if (t.entityprofile[t.tttentid].ismarker == 2)  t.thaveyrot = 1;

			// allow rotation/scale
			bool bHaveFullRotation = false;
			if (t.entityprofile[t.tttentid].ismarker == 0) bHaveFullRotation = true;
			if (t.entityprofile[t.tttentid].ismarker == 10) bHaveFullRotation = true;
			if ( (bHaveFullRotation==true || t.thaveyrot == 1) && ObjectExist(t.widget.activeObject) == 1 )
			{
				// rotation
				if ( t.widget.mode == 1 && t.toriginalTranslateClickDeferred <= 1 )//|| g.fForceYRotationOfRubberBandFromKeyPress != 0.0f )
				{
					// calculate movement from start position to now
					float fMoveAngleX = 0.0f;
					float fMoveAngleY = 0.0f;
					float fMoveAngleZ = 0.0f;
					float fPlanePosX, fPlanePosY, fPlanePosZ;
					float fActivePosX = ObjectPositionX(t.widget.activeObject);
					float fActivePosY = ObjectPositionY(t.widget.activeObject);
					float fActivePosZ = ObjectPositionZ(t.widget.activeObject);
					bool bPlanePosRegistered = widget_getplanepos(fActivePosX, fActivePosY, fActivePosZ, &fPlanePosX, &fPlanePosY, &fPlanePosZ);
					if (bPlanePosRegistered == true)
					{
						// offset pos on plane with difference between widget click pos and active object
						fPlanePosX -= fActivePosX;
						fPlanePosY -= fActivePosY;
						fPlanePosZ -= fActivePosZ;

						// based on axis chosen, work out the absolute angle the user has indicated
						if (t.widget.pickedSection == t.widget.widgetXRotObj) fMoveAngleX = -GGToDegree(atan2(fPlanePosY, fPlanePosZ));
						if (t.widget.pickedSection == t.widget.widgetYRotObj) fMoveAngleY = -GGToDegree(atan2(fPlanePosZ, fPlanePosX));
						if (t.widget.pickedSection == t.widget.widgetZRotObj) fMoveAngleZ = GGToDegree(atan2(fPlanePosY, fPlanePosX));

						// initial click or actual adjustment
						if (t.toriginalTranslateClickDeferred == 1)
						{
							// for the initial click, detect the start of the change
							if (t.widget.pickedSection == t.widget.widgetXRotObj) t.toriginalRotationClickX_f = fMoveAngleX;
							if (t.widget.pickedSection == t.widget.widgetYRotObj) t.toriginalRotationClickY_f = fMoveAngleY;
							if (t.widget.pickedSection == t.widget.widgetZRotObj) t.toriginalRotationClickZ_f = fMoveAngleZ;
							fMoveAngleX = 0; fMoveAngleY = 0; fMoveAngleZ = 0;

							// if entity has no quat, create one (leelee, move to own function, may be using this a lot)
							if (t.entityelement[t.te].quatmode == 0)
							{
								// generate quat for first time
								GGQUATERNION QuatAroundX, QuatAroundY, QuatAroundZ;
								GGQuaternionRotationAxis(&QuatAroundX, &GGVECTOR3(1, 0, 0), GGToRadian(ObjectAngleX(t.widget.activeObject)));
								GGQuaternionRotationAxis(&QuatAroundY, &GGVECTOR3(0, 1, 0), GGToRadian(ObjectAngleY(t.widget.activeObject)));
								GGQuaternionRotationAxis(&QuatAroundZ, &GGVECTOR3(0, 0, 1), GGToRadian(ObjectAngleZ(t.widget.activeObject)));
								t.toriginalAngle = QuatAroundX * QuatAroundY * QuatAroundZ;
							}
							else
							{
								// use entities current quat
								t.toriginalAngle = GGQUATERNION(t.entityelement[t.te].quatx, t.entityelement[t.te].quaty, t.entityelement[t.te].quatz, t.entityelement[t.te].quatw);
							}

							// defer detect complete
							t.toriginalTranslateClickDeferred = 0;
						}
						else
						{
							// actual adjustment
							if (t.widget.pickedSection == t.widget.widgetXRotObj) fMoveAngleX -= t.toriginalRotationClickX_f;
							if (t.widget.pickedSection == t.widget.widgetYRotObj) fMoveAngleY -= t.toriginalRotationClickY_f;
							if (t.widget.pickedSection == t.widget.widgetZRotObj) fMoveAngleZ -= t.toriginalRotationClickZ_f;
						}
					}

					// quat rotation event
					GGQUATERNION quatRotationEvent = { 0,0,0,0 };
					if (t.entityprofile[t.tttentid].ragdoll == 1)
					{
						GGQuaternionRotationAxis(&quatRotationEvent, &GGVECTOR3(0, 1, 0), GGToRadian(fMoveAngleY));
					}
					else
					{
						GGQUATERNION QuatAroundX, QuatAroundY, QuatAroundZ;
						GGQuaternionRotationAxis(&QuatAroundX, &GGVECTOR3(1, 0, 0), GGToRadian(fMoveAngleX));
						GGQuaternionRotationAxis(&QuatAroundY, &GGVECTOR3(0, 1, 0), GGToRadian(fMoveAngleY));
						GGQuaternionRotationAxis(&QuatAroundZ, &GGVECTOR3(0, 0, 1), GGToRadian(fMoveAngleZ));
						quatRotationEvent = QuatAroundX * QuatAroundY * QuatAroundZ;
					}

					// apply the rotation event to the angle of the object
					GGQUATERNION quatNewOrientation;
					GGQuaternionMultiply(&quatNewOrientation, &t.toriginalAngle, &quatRotationEvent);

					// rotate this object with final quat and get new entity rotation eulers
					RotateObjectQuat(t.widget.activeObject, quatNewOrientation.x, quatNewOrientation.y, quatNewOrientation.z, quatNewOrientation.w);
					t.entityelement[t.te].rx = ObjectAngleX(t.widget.activeObject);
					t.entityelement[t.te].ry = ObjectAngleY(t.widget.activeObject);
					t.entityelement[t.te].rz = ObjectAngleZ(t.widget.activeObject);

					// update entity quat as the preferred source rotation
					t.entityelement[t.te].quatmode = 1;
					t.entityelement[t.te].quatx = quatNewOrientation.x;
					t.entityelement[t.te].quaty = quatNewOrientation.y;
					t.entityelement[t.te].quatz = quatNewOrientation.z;
					t.entityelement[t.te].quatw = quatNewOrientation.w;

					// mark as static if it was
					if ( t.entityelement[t.te].staticflag == 1 ) g.projectmodifiedstatic = 1;

					// move zones and lights if in group
					widget_movezonesandlights ( t.te );

					// if we need to also rotate rubber band highlighted objects, do so now
					bool bDisableRubberBandMoving = false;
					extern int current_selected_group;
					extern bool group_editing_on;
					if (current_selected_group >= 0 && group_editing_on)
					{
						bDisableRubberBandMoving = true;
					}
					if (!bDisableRubberBandMoving)
					{
						if (g.entityrubberbandlist.size() > 0)
						{
							// rotate all the grouped entities and move around Y axis of widget as pivot (from their starting positions and angles)
							RotateAndMoveRubberBand(t.widget.activeObject, 0, 0, 0, quatRotationEvent);
						}
					}
				}

				// scale
				if ( t.widget.mode == 2 && t.toriginalTranslateClickDeferred <= 1)
				{
					// store old active object scales for rubber band
					float fOldActiveObjectSX = ObjectScaleX ( t.widget.activeObject );
					float fOldActiveObjectSY = ObjectScaleY ( t.widget.activeObject );
					float fOldActiveObjectSZ = ObjectScaleZ ( t.widget.activeObject );
					float fScaleX = ObjectScaleX(t.widget.activeObject);
					float fScaleY = ObjectScaleY(t.widget.activeObject);
					float fScaleZ = ObjectScaleZ(t.widget.activeObject);
					float fPlanePosX, fPlanePosY, fPlanePosZ;
					float fActivePosX = ObjectPositionX(t.widget.activeObject);
					float fActivePosY = ObjectPositionY(t.widget.activeObject);
					float fActivePosZ = ObjectPositionZ(t.widget.activeObject);
					bool bPlanePosRegistered = widget_getplanepos(fActivePosX, fActivePosY, fActivePosZ, &fPlanePosX, &fPlanePosY, &fPlanePosZ );
					if (bPlanePosRegistered == true)
					{
						// offset pos on plane with difference between widget click pos and active object
						fPlanePosX -= fActivePosX;
						fPlanePosY -= fActivePosY;
						fPlanePosZ -= fActivePosZ;

						// if scale mode, additional init stuff when first click
						if (t.toriginalTranslateClickDeferred == 1)
						{
							// record initial click plane pos
							t.toriginalTranslateClickX_f = fPlanePosX;
							t.toriginalTranslateClickY_f = fPlanePosY;
							t.toriginalTranslateClickZ_f = fPlanePosZ;
							
							// record original scale of active object
							t.toriginalScalingX_f = ObjectScaleX(t.widget.activeObject);
							t.toriginalScalingY_f = ObjectScaleY(t.widget.activeObject);
							t.toriginalScalingZ_f = ObjectScaleZ(t.widget.activeObject);

							// deferred detect complete for rotation
							t.toriginalTranslateClickDeferred = 0;
						}
						
						// ensure the scaling modified latest current scale from moment click the scale widget gadget
						fPlanePosX -= t.toriginalTranslateClickX_f;
						fPlanePosY -= t.toriginalTranslateClickY_f;
						fPlanePosZ -= t.toriginalTranslateClickZ_f;

						GGQUATERNION quatRot = GGQUATERNION(t.entityelement[t.te].quatx, t.entityelement[t.te].quaty, t.entityelement[t.te].quatz, t.entityelement[t.te].quatw);
						GGMATRIX matRot;
						GGMatrixRotationQuaternion(&matRot, &quatRot);
						GGVECTOR3 vecDir;
						if (t.widget.pickedSection == t.widget.widgetXScaleObj) vecDir = GGVECTOR3(1, 0, 0);
						if (t.widget.pickedSection == t.widget.widgetYScaleObj) vecDir = GGVECTOR3(0, 1, 0);
						if (t.widget.pickedSection == t.widget.widgetZScaleObj) vecDir = GGVECTOR3(0, 0, 1);
						if (t.widget.pickedSection == t.widget.widgetXYZScaleObj) vecDir = GGVECTOR3(0, 1, 0);
						GGVec3TransformCoord(&vecDir, &vecDir, &matRot);
						GGVECTOR3 vecPlanePos = GGVECTOR3(fPlanePosX, fPlanePosY, fPlanePosZ);
						float fShiftDelta = GGVec4Dot(&vecDir, &vecPlanePos);
						if (t.widget.pickedSection == t.widget.widgetXScaleObj) fScaleX = t.toriginalScalingX_f + fShiftDelta;
						if (t.widget.pickedSection == t.widget.widgetYScaleObj) fScaleY = t.toriginalScalingY_f + fShiftDelta;
						if (t.widget.pickedSection == t.widget.widgetZScaleObj) fScaleZ = t.toriginalScalingZ_f + fShiftDelta;
						if (t.widget.pickedSection == t.widget.widgetXYZScaleObj)
						{
							fScaleX = t.toriginalScalingX_f + fShiftDelta;
							fScaleY = t.toriginalScalingY_f + fShiftDelta;
							fScaleZ = t.toriginalScalingZ_f + fShiftDelta;
						}
					}

					// scale object and store in entity element item
					ScaleObject ( t.widget.activeObject, fScaleX, fScaleY, fScaleZ );
					t.entityelement[t.te].scalex = ObjectScaleX(t.widget.activeObject)-100.0;
					t.entityelement[t.te].scaley = ObjectScaleY(t.widget.activeObject)-100.0;
					t.entityelement[t.te].scalez = ObjectScaleZ(t.widget.activeObject)-100.0;

					// mark as static if it was
					if ( t.entityelement[t.te].staticflag == 1 ) g.projectmodifiedstatic = 1;

					// if we need to also scale rubber band highlighted objects, do so now
					bool bDisableRubberBandMoving = false;
					extern int current_selected_group;
					extern bool group_editing_on;
					if (current_selected_group >= 0 && group_editing_on)
					{
						bDisableRubberBandMoving = true;
					}
					if (!bDisableRubberBandMoving)
					{

						if (g.entityrubberbandlist.size() > 0)
						{
							float fMovedActiveObjectSX = ObjectScaleX(t.widget.activeObject) - fOldActiveObjectSX;
							float fMovedActiveObjectSY = ObjectScaleY(t.widget.activeObject) - fOldActiveObjectSY;
							float fMovedActiveObjectSZ = ObjectScaleZ(t.widget.activeObject) - fOldActiveObjectSZ;
							for (int i = 0; i < (int)g.entityrubberbandlist.size(); i++)
							{
								int e = g.entityrubberbandlist[i].e;
								int tobj = t.entityelement[e].obj;
								if (tobj > 0 && t.entityelement[e].editorlock == 0)
								{
									if (ObjectExist(tobj) == 1)
									{
										if (tobj != t.widget.activeObject)
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

			//  detect if widget panel off screen, and shift back in
			widget_correctwidgetpanel ( );

			//PE: If we moved a light, also update probes.
			if (t.widget.pickedEntityIndex > 0)
			{
				int entid = t.entityelement[t.widget.pickedEntityIndex].bankindex;
				if (entid > 0)
				{
					if (t.entityprofile[entid].ismarker == 2)
					{
						extern bool g_bLightProbeScaleChanged;
						g_bLightProbeScaleChanged = true;
					}
				}
			}

			// user has let go of the mouse, reset section chosen
			if (t.inputsys.mclick == 0)
			{
				t.widget.pickedSection = 0;
				t.widget.grabbed = 0;
				// and finally trigger UI to update its euler values to sync with widget rotation!
				g_bRefreshRotationValuesFromObjectOnce = true;
			}
		}

		// update waypoint object when widget moves entity
		t.ttte=t.widget.pickedEntityIndex;
		if ( t.ttte>0 ) 
		{
			t.waypointindex=t.entityelement[t.ttte].eleprof.trigger.waypointzoneindex;
			if ( t.waypointindex>0 ) 
			{
				// seems when zones are cloned, they copy the waypointzoneindex index, causing corruption
				waypoint_fixcorruptduplicate(t.ttte); // acts on t.waypointindex
				t.thisx_f=t.entityelement[t.ttte].x;
				t.thisy_f=t.entityelement[t.ttte].y;
				t.thisz_f=t.entityelement[t.ttte].z;
				waypoint_movetothiscoordinate ( );
			}
		}
	}
	t.widget.oldMouseClick = t.inputsys.mclick;

	//  restore camera range
	editor_refreshcamerarange ( );
}

void widget_correctwidgetpanel ( void )
{
	// reverse widget when facing away from user
	if ( WrapValue(CameraAngleY())>180 || WrapValue(CameraAngleY())<1  ) t.txflip = 0; else t.txflip = 1;
	if ( t.txflip == 1 ) 
	{
		YRotateObject ( t.widget.widgetXObj, 180 );
		YRotateObject ( t.widget.widgetXYObj, 180 );
	}
	else
	{
		YRotateObject ( t.widget.widgetXObj, 0 );
		YRotateObject ( t.widget.widgetXYObj, 0 );
	}
	if ( WrapValue(CameraAngleY())>270 || WrapValue(CameraAngleY())<90 ) 
	{
		YRotateObject ( t.widget.widgetZObj, 180 );
		if ( t.txflip == 1 ) 
			YRotateObject ( t.widget.widgetXZObj, 90 );
		else
			YRotateObject ( t.widget.widgetXZObj, 0 );
		YRotateObject ( t.widget.widgetYZObj, 0 );
	}
	else
	{
		YRotateObject ( t.widget.widgetZObj, 0 );
		if ( t.txflip == 1 ) 
		{
			YRotateObject ( t.widget.widgetXZObj, 180 );
		}
		else
		{
			YRotateObject ( t.widget.widgetXZObj, 270 );
		}
		YRotateObject ( t.widget.widgetYZObj, 180 );
	}
}

void widget_updatewidgetobject ( void )
{
	//  If no picked object and the widget has been in use, switch it off
	if (  t.widget.pickedObject == 0 ) 
	{
		if (  t.widget.activeObject>0 ) 
		{
			t.widget.pickedEntityIndex=0;
			t.widget.grabbed=0;
			t.widget.pickedSection=0;
			t.widget.mode=0;
			for ( t.a = 0 ; t.a <= t.widget.widgetMAXObj; t.a++ )
			{
				if (  ObjectExist(g.widgetobjectoffset+t.a) ) HideObject ( g.widgetobjectoffset+t.a );
			}
			t.widget.activeObject=0;
			extern bool bTriggerVisibleWidget;
			bTriggerVisibleWidget = false;
		}
	}
	else
	{
		if (  t.widget.pickedObject != t.widget.activeObject  )  t.widget.activeObject = 0;
	}
}

void widget_check_for_new_object_selection ( void )
{
	// check if an object has been chosen
	if (  t.widget.activeObject == 0 ) 
	{
		if (  t.widget.pickedObject != 0 ) 
		{
			t.widget.activeObject=t.widget.pickedObject;
			t.widget.pickedSection=0;
			t.widget.grabbed=0;
			widget_show_widget ( );
		}
	}
	t.widget.oldActiveObject=t.widget.activeObject;
}

void widget_show_widget ( void )
{
	//  Show the widget in all its glory
	if ( t.widget.activeObject == 0  )  return;
	if ( ObjectExist(t.widget.activeObject) == 0  )  return;
	if (widget_temp_disabled) return;
	#ifdef ALLOWSELECTINGLOCKEDOBJECTS
	//PE: Never enable if locked.
	if (t.widget.pickedEntityIndex > 0 && t.entityelement[t.widget.pickedEntityIndex].editorlock == 1)
		return;
	#endif
	// what is shown varies on which mode is picked (translate, rotate or scale)
	t.oldry_f = ObjectAngleY(t.widget.activeObject);
	for (t.a = 0; t.a <= t.widget.widgetMAXObj; t.a++)
	{
		if (ObjectExist(g.widgetobjectoffset + t.a))
		{
			HideObject(g.widgetobjectoffset + t.a);
		}
		sObject* pObject = g_ObjectList[g.widgetobjectoffset + t.a];
		if (pObject)
		{
			WickedCall_SetObjectCastShadows(pObject, false);
		}
		extern bool bTriggerVisibleWidget;
		bTriggerVisibleWidget = false;
	}
	// detect if selected in an EBE entity
	bool bIsEBEWidget = false;
	int iEntID = -1;
	t.ttte=t.widget.pickedEntityIndex;
	if (  t.ttte>0 ) 
	{
		iEntID = t.entityelement[t.ttte].bankindex;
		if ( iEntID > 0 ) 
			if ( t.entityprofile[iEntID].isebe != 0 )
				bIsEBEWidget = true;
	}

	// normally scale and property (but EBE can change this texture)

	// rubber band or selected parent
	bool bRealRubberBand = false;
	if ( g.entityrubberbandlist.size() > 0 )
	{
		bRealRubberBand = true;
		if ( t.widget.pickedEntityIndex > 0 )
		{
			bool bPartOfParentChildGroup = false;
			editor_rec_checkifindexinparentchain ( t.widget.pickedEntityIndex, &bPartOfParentChildGroup );
			if ( bPartOfParentChildGroup == true )
				bRealRubberBand = false;
		}
	}

	// show all or just POS in widget popup

	if ( t.widget.mode == 0 ) 
	{
		for ( t.a = 0 ; t.a <= 5; t.a++ )
		{
			if ( ObjectExist(g.widgetobjectoffset+t.a) == 1 ) 
			{
				ShowObject ( g.widgetobjectoffset+t.a );
				PositionObject ( g.widgetobjectoffset+t.a, CameraPositionX(), CameraPositionY(), CameraPositionZ() );
				if (t.widget.mode == 0)
					PointObject ( g.widgetobjectoffset+t.a,ObjectPositionX(t.widget.activeObject)+t.widget.offsetx,ObjectPositionY(t.widget.activeObject)+t.widget.offsety,ObjectPositionZ(t.widget.activeObject)+t.widget.offsetz );
				else
					PointObject (g.widgetobjectoffset + t.a, ObjectPositionX(t.widget.activeObject) + 0, ObjectPositionY(t.widget.activeObject) + 0, ObjectPositionZ(t.widget.activeObject) + 0);
				MoveObject ( g.widgetobjectoffset+t.a,40 );
				RotateObject ( g.widgetobjectoffset+t.a,0,0,0 );
			}
		}
	}
	if ( t.widget.mode == 1 ) 
	{
		for ( t.a = 6 ; t.a <= 8; t.a++ )
		{
			if ( ObjectExist(g.widgetobjectoffset+t.a) == 1 ) 
			{
				if (iEntID > 0)
				{
					// Player start marker and characters can be rotated about the y-axis, so show the partial widget object
					if (t.entityprofile[iEntID].ismarker == 1 || t.entityprofile[iEntID].ischaracter)
					{
						if (t.a == 6 || t.a == 8)
						{
							HideObject(g.widgetobjectoffset + t.a);
							continue;
						}
					}
				}
				ShowObject ( g.widgetobjectoffset+t.a );
				PositionObject ( g.widgetobjectoffset+t.a, CameraPositionX(), CameraPositionY(), CameraPositionZ() );
				if (t.widget.mode == 0)
					PointObject (g.widgetobjectoffset + t.a, ObjectPositionX(t.widget.activeObject) + t.widget.offsetx, ObjectPositionY(t.widget.activeObject) + t.widget.offsety, ObjectPositionZ(t.widget.activeObject) + t.widget.offsetz);
				else
					PointObject ( g.widgetobjectoffset+t.a,ObjectPositionX(t.widget.activeObject)+0,ObjectPositionY(t.widget.activeObject)+0,ObjectPositionZ(t.widget.activeObject)+0 );
				MoveObject ( g.widgetobjectoffset+t.a,40 );
			}
		}
		if ( ObjectExist( g.widgetobjectoffset+6) == 1 ) 
		{
			ScaleObject ( g.widgetobjectoffset+6,50,100,100 );
			ScaleObject ( g.widgetobjectoffset+7,100,50,100 );
			ScaleObject ( g.widgetobjectoffset+8,100,100,50 );
			RotateObject(g.widgetobjectoffset + 6, 0, 0, 0);
			RotateObject(g.widgetobjectoffset + 7, 0, 0, 0);
			RotateObject(g.widgetobjectoffset + 8, 0, 0, 0);
		}
		if ( g.entityrubberbandlist.size() > 0 || bIsEBEWidget == true )
		{
			// hide X and Z adjustment (for now)
			HideObject ( g.widgetobjectoffset + 6 );
			RotateObject ( g.widgetobjectoffset+7, 0, ObjectAngleY(t.widget.activeObject), 0 );
			HideObject ( g.widgetobjectoffset + 8 );
		}	
	}
	if ( t.widget.mode == 2 ) 
	{
		bool bShowOnlyXScale = false;
		if (t.entityprofile[iEntID].ismarker == 10) bShowOnlyXScale = true;
		for ( t.a = 9 ; t.a <= 12; t.a++ )
		{
			if ( ObjectExist(g.widgetobjectoffset+t.a) == 1 ) 
			{
				if (bShowOnlyXScale == false || t.a == 12)
				{
					ShowObject (g.widgetobjectoffset + t.a);
				}
				else
				{
					HideObject (g.widgetobjectoffset + t.a);
				}
				PositionObject ( g.widgetobjectoffset+t.a, CameraPositionX(), CameraPositionY(), CameraPositionZ() );
				PointObject ( g.widgetobjectoffset+t.a,ObjectPositionX(t.widget.activeObject)+0,ObjectPositionY(t.widget.activeObject)+0,ObjectPositionZ(t.widget.activeObject)+0 );
				MoveObject ( g.widgetobjectoffset+t.a,40 );
				RotateObject ( g.widgetobjectoffset+t.a,0,0,0 );
			}
		}
		if ( ObjectExist(g.widgetobjectoffset+9) == 1 ) 
		{
			RotateObject ( g.widgetobjectoffset+9, ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject) );
			RotateObject ( g.widgetobjectoffset+10, ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject) );
			RotateObject ( g.widgetobjectoffset+11, ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject) );
			RotateObject ( g.widgetobjectoffset+12, ObjectAngleX(t.widget.activeObject),ObjectAngleY(t.widget.activeObject), ObjectAngleZ(t.widget.activeObject) );
		}
	}

	// hide any buttons and widgets if entity is a 'waypoint zone type'
}

void widget_switchoff ( void )
{
	t.widget.pickedSection=0;
	t.widget.activeObject=0;
	t.widget.pickedObject=0;
	//t.widget.mode=0; // do not reset widget mode, retain current choice for convenience
	widget_hide ( );
}

void widget_hide ( void )
{
	for ( t.a = 0 ; t.a<=  40; t.a++ )
	{
		if (  ObjectExist (g.widgetobjectoffset+t.a)  )  HideObject (  g.widgetobjectoffset+t.a );
	}
	extern bool bTriggerVisibleWidget;
	bTriggerVisibleWidget = false;
}

void widget_free ( void )
{
	//  clean up widget files (will need to call widget_init again before using the widget)
	for ( t.a = 0 ; t.a<=  40; t.a++ )
	{
		if (  ObjectExist (g.widgetobjectoffset+t.a)  )  DeleteObject (  g.widgetobjectoffset+t.a ) ;
	}
	for ( t.a = 1 ; t.a<=  12; t.a++ )
	{
		t.r=DeleteVector3(g.widgetvectorindex+t.a);
	}
	for ( t.a = 1 ; t.a<=  5; t.a++ )
	{
		if (  MatrixExist(g.widgetStartMatrix+t.a)  )  DeleteMatrix (  g.widgetStartMatrix+t.a );
	}
	t.widget.mode=0;
	return;
}
