//----------------------------------------------------
//--- GAMEGURU - M-Postprocess
//----------------------------------------------------

#include "stdafx.h"
#include "gameguru.h"

#ifndef PRODUCTCLASSIC
#include "openxr.h"
#endif

#ifdef ENABLEIMGUI
//PE: GameGuru IMGUI.
#include "..\Imgui\imgui.h"
#include "..\Imgui\imgui_impl_win32.h"
#include "..\Imgui\imgui_gg_dx11.h"
#endif

#ifdef OPTICK_ENABLE
#include "optick.h"
#endif

// 
//  POST PROCESSING (0-main cam,1-reserved,2-reflect cam,3-finalrender cam)
//  4-sunlight ray camera
// 

#ifdef VRTECH
// Some convenient globals for VR controller shader and textures
int g_iCShaderID = 0;
int g_iCTextureID0 = 0;
int g_iCTextureID1 = 0;
int g_iCTextureID2 = 0;
int g_iCTextureID3 = 0;
int g_iCTextureID4 = 0;
int g_iCTextureID5 = 0;
int g_iCTextureID6 = 0;
#endif

void postprocess_init ( void )
{
	// full screen shaders
	g.gpostprocessing = 1;

	#ifdef WICKEDENGINE
	// no need for post processing, but need to init VR (copied from below)
	char pErrorStr[1024];
	sprintf ( pErrorStr, "check if VR required with codes %d and %d", g.vrglobals.GGVREnabled, g.vrglobals.GGVRUsingVRSystem );
	timestampactivity(0,pErrorStr);
	if ( g.vrglobals.GGVREnabled > 0 && g.vrglobals.GGVRUsingVRSystem == 1 )
	{
		// Set camera IDs and initialise GGVR
		t.glefteyecameraid = 6;
		t.grighteyecameraid = 7;
		g.vrglobals.GGVRInitialized = 0;
		sprintf ( pErrorStr, "initialise VR System Mode %d", g.vrglobals.GGVREnabled );
		timestampactivity(0,pErrorStr);
		if ( g_iCShaderID == 0 ) g_iCShaderID = g.controllerpbreffect;
		if ( g_iCTextureID0 == 0 ) g_iCTextureID0 = loadinternaltextureex("gamecore\\vrcontroller\\vrcontroller_color.png", 1, t.tfullorhalfdivide);
		if ( g_iCTextureID1 == 0 ) g_iCTextureID1 = loadinternaltextureex("effectbank\\reloaded\\media\\blank_O.dds", 1, t.tfullorhalfdivide);
		if ( g_iCTextureID2 == 0 ) g_iCTextureID2 = loadinternaltextureex("gamecore\\vrcontroller\\vrcontroller_normal.png", 1, t.tfullorhalfdivide);
		if ( g_iCTextureID3 == 0 ) g_iCTextureID3 = loadinternaltextureex("gamecore\\vrcontroller\\vrcontroller_metalness.png", 1, t.tfullorhalfdivide);
		if ( g_iCTextureID4 == 0 ) g_iCTextureID4 = loadinternaltextureex("gamecore\\vrcontroller\\vrcontroller_gloss.png", 1, t.tfullorhalfdivide);
		if ( g_iCTextureID5 == 0 ) g_iCTextureID5 = g.postprocessimageoffset+5;
		if ( g_iCTextureID6 == 0 ) g_iCTextureID6 = t.terrain.imagestartindex+31;//loadinternaltextureex("effectbank\\reloaded\\media\\blank_I.dds", 1, t.tfullorhalfdivide);
		int oculusTex0 = loadinternaltextureex("gamecore\\vrcontroller\\oculus\\controller_bc.png", 1, t.tfullorhalfdivide);
		sprintf ( pErrorStr, "controller asset %d %d %d %d %d %d %d %d", g_iCShaderID, g_iCTextureID0, g_iCTextureID1, g_iCTextureID2, g_iCTextureID3, g_iCTextureID4, g_iCTextureID5, g_iCTextureID6 );
		timestampactivity(0,pErrorStr);
		int iErrorCode = GGVR_Init ( g.rootdir_s.Get(), g.postprocessimageoffset + 4, g.postprocessimageoffset + 3, t.grighteyecameraid, t.glefteyecameraid, 10000, 10001, 10002, 10003, 10004, 10005, 10099, g.guishadereffectindex, g.editorimagesoffset+14, g_iCShaderID, g_iCTextureID0, g_iCTextureID1, g_iCTextureID2, g_iCTextureID3, g_iCTextureID4, g_iCTextureID5, g_iCTextureID6, oculusTex0);
		if ( iErrorCode > 0 )
		{
			sprintf ( pErrorStr, "Error starting VR : Code %d", iErrorCode );
			timestampactivity(0,pErrorStr);
			t.visuals.generalpromptstatetimer = Timer()+1000;
			t.visuals.generalprompt_s = "No OpenXR runtime found";
		}
		GGVR_SetGenericOffsetAngX( g.gvroffsetangx );
		GGVR_SetWMROffsetAngX( g.gvrwmroffsetangx );

		// used to mark VR initiated (so can free resources when done)
		t.gpostprocessmode = 1;
	}
	#else
	//  postprocesseffectoffset;
	//  0-bloom
	//  1-scatter
	//  2-nobloom

	//  if flagged, use post processing
	timestampactivity(0,"postprocessing code started");
	if (  g.gpostprocessing == 0 ) 
	{
		//  ensure camera zero set correctly
		SetCameraView (  0,0,0,GetDisplayWidth(),GetDisplayHeight() );
	}
	else
	{
		//  Initialise or Activate existing
		if ( t.gpostprocessmode == 0 ) 
		{
			// kind of post process (gpostprocessing)
			// 1 - bloom
			timestampactivity(0,"postprocessing check");
			if ( g.gpostprocessing == 1 ) 
			{
				//  new camera 3 holds post-process screen quad
				g.gfinalrendercameraid=3;
				CreateCamera ( g.gfinalrendercameraid );
				SetCameraView ( g.gfinalrendercameraid,0,0,GetDisplayWidth(),GetDisplayHeight() );
				if (  GetEffectExist(g.postprocesseffectoffset) == 1  )  DeleteEffect (  g.postprocesseffectoffset );
				t.tshaderchoice_s="effectbank\\reloaded\\post-bloom.fx";
				LoadEffect (  t.tshaderchoice_s.Get(),g.postprocesseffectoffset,0 );
				if (  GetEffectExist(g.postprocesseffectoffset+2) == 1  )  DeleteEffect (  g.postprocesseffectoffset+2 );
				t.tshaderchoice_s="effectbank\\reloaded\\post-none.fx";
				LoadEffect (  t.tshaderchoice_s.Get(),g.postprocesseffectoffset+2,0 );
				//  create and prepare resources for full screen final render
				MakeObjectPlane ( g.postprocessobjectoffset+0, 2, 2 );
				SetObjectEffect (  g.postprocessobjectoffset+0,g.postprocesseffectoffset );
				g.postprocessobjectoffset0laststate = g.postprocesseffectoffset;
				SetObjectMask (  g.postprocessobjectoffset+0,(1 << g.gfinalrendercameraid) );
				SetCameraToImage ( 0, g.postprocessimageoffset+0, GetDisplayWidth(), GetDisplayHeight(), 2 );
				TextureObject ( g.postprocessobjectoffset+0,0,g.postprocessimageoffset+0 );
				if( g.underwatermode == 1 ) {
					//PE:Underwater normal texture for wave.
					TextureObject(g.postprocessobjectoffset + 0, 1, t.terrain.imagestartindex + 7);
				}

				//  special code to instruct this post process shader to generate depth texture
				//  from the main camera zero and pass into the shader as 'DepthMapTex' texture slot
				SetVector4 (  g.terrainvectorindex,0,0,0,0 );
				SetEffectConstantV (  g.postprocesseffectoffset+0,"[hook-depth-data]",g.terrainvectorindex );

				//  post process mode on
				g.gpostprocessingnotransparency=0;
				t.gpostprocessmode=1;

				//  if Virtual Reality RIFT MODE active
				if (  g.globals.riftmode>0 ) 
				{
					//  Camera SIX is second eye camera (right eye) [left eye is camera zero]
					t.glefteyecameraid=6 ; CreateCamera (  t.glefteyecameraid );
					SetCameraToImage (  t.glefteyecameraid,g.postprocessimageoffset+3,GetDisplayWidth(),GetDisplayHeight(),2 );
					t.grighteyecameraid=7 ; CreateCamera (  t.grighteyecameraid );
					SetCameraToImage (  t.grighteyecameraid,g.postprocessimageoffset+4,GetDisplayWidth(),GetDisplayHeight(),2 );
					//  special post process shader for RIFT stereoscopics
					if (  GetEffectExist(g.postprocesseffectoffset+3) == 1  )  DeleteEffect (  g.postprocesseffectoffset+3 );
					t.tshaderchoice_s="effectbank\\reloaded\\post-rift.fx";
					LoadEffect (  t.tshaderchoice_s.Get(),g.postprocesseffectoffset+3,0 );
					//  create and prepare resources for full screen final render
					MakeObjectPlane (  g.postprocessobjectoffset+3,2,2 );
					SetObjectEffect (  g.postprocessobjectoffset+3,g.postprocesseffectoffset+3 );
					g.postprocessobjectoffset0laststate = g.postprocesseffectoffset+3;
					SetObjectMask (  g.postprocessobjectoffset+3,(1 << g.gfinalrendercameraid) );
					TextureObject (  g.postprocessobjectoffset+3,0,g.postprocessimageoffset+0 );
					//  how adjust the main post process quad and shader to create stereo screens
					SetObjectEffect (  g.postprocessobjectoffset+0,g.postprocesseffectoffset+3 );
					g.postprocessobjectoffset0laststate = g.postprocesseffectoffset+3;
					TextureObject (  g.postprocessobjectoffset+0,0,g.postprocessimageoffset+3 );
					TextureObject (  g.postprocessobjectoffset+3,0,g.postprocessimageoffset+4 );
					PositionObject (  g.postprocessobjectoffset+0,-0.5,0,0 );
					PositionObject (  g.postprocessobjectoffset+3,0.5,0,0 );

					//  Adjust left and right eye camera projections for Rift viewing
					if (  g.globals.riftmoderesult == 0 ) 
					{
						//  rift update projection matrix glefteyecameraid, grighteyecameraid, visuals.CameraNEAR#,visuals.CameraFAR#
					}
					else
					{
						// 311017 - solve Z clash issues by adjusting near depth based on far depth
						float fFinalNearDepth = 2.0f + t.visuals.CameraNEAR_f + ((t.visuals.CameraFAR_f/DEFAULT_FAR_PLANE)*8.0f);
						SetCameraRange ( t.glefteyecameraid, fFinalNearDepth, t.visuals.CameraFAR_f );
						SetCameraRange ( t.grighteyecameraid, fFinalNearDepth, t.visuals.CameraFAR_f );
					}
				}

				#ifdef VRTECH
				// VR Support - create VR cameras
				char pErrorStr[1024];
				sprintf ( pErrorStr, "check if VR required with codes %d and %d", g.vrglobals.GGVREnabled, g.vrglobals.GGVRUsingVRSystem );
				timestampactivity(0,pErrorStr);
				if ( g.vrglobals.GGVREnabled > 0 && g.vrglobals.GGVRUsingVRSystem == 1 )
				{
					// Set camera IDs and initialise GGVR
					t.glefteyecameraid = 6;
					t.grighteyecameraid = 7;
					g.vrglobals.GGVRInitialized = 0;
					sprintf ( pErrorStr, "initialise VR System Mode %d", g.vrglobals.GGVREnabled );
					timestampactivity(0,pErrorStr);
					if ( g_iCShaderID == 0 ) g_iCShaderID = g.controllerpbreffect;
					if ( g_iCTextureID0 == 0 ) g_iCTextureID0 = loadinternaltextureex("gamecore\\vrcontroller\\vrcontroller_color.png", 1, t.tfullorhalfdivide);
					if ( g_iCTextureID1 == 0 ) g_iCTextureID1 = loadinternaltextureex("effectbank\\reloaded\\media\\blank_O.dds", 1, t.tfullorhalfdivide);
					if ( g_iCTextureID2 == 0 ) g_iCTextureID2 = loadinternaltextureex("gamecore\\vrcontroller\\vrcontroller_normal.png", 1, t.tfullorhalfdivide);
					if ( g_iCTextureID3 == 0 ) g_iCTextureID3 = loadinternaltextureex("gamecore\\vrcontroller\\vrcontroller_metalness.png", 1, t.tfullorhalfdivide);
					if ( g_iCTextureID4 == 0 ) g_iCTextureID4 = loadinternaltextureex("gamecore\\vrcontroller\\vrcontroller_gloss.png", 1, t.tfullorhalfdivide);
					if ( g_iCTextureID5 == 0 ) g_iCTextureID5 = g.postprocessimageoffset+5;
					if ( g_iCTextureID6 == 0 ) g_iCTextureID6 = t.terrain.imagestartindex+31;//loadinternaltextureex("effectbank\\reloaded\\media\\blank_I.dds", 1, t.tfullorhalfdivide);
					int oculusTex0 = loadinternaltextureex("gamecore\\vrcontroller\\oculus\\controller_bc.png", 1, t.tfullorhalfdivide);
					sprintf ( pErrorStr, "controller asset %d %d %d %d %d %d %d %d", g_iCShaderID, g_iCTextureID0, g_iCTextureID1, g_iCTextureID2, g_iCTextureID3, g_iCTextureID4, g_iCTextureID5, g_iCTextureID6 );
					timestampactivity(0,pErrorStr);
					int iErrorCode = GGVR_Init ( g.rootdir_s.Get(), g.postprocessimageoffset + 4, g.postprocessimageoffset + 3, t.grighteyecameraid, t.glefteyecameraid, 10000, 10001, 10002, 10003, 10004, 10005, 10099, g.guishadereffectindex, g.editorimagesoffset+14, g_iCShaderID, g_iCTextureID0, g_iCTextureID1, g_iCTextureID2, g_iCTextureID3, g_iCTextureID4, g_iCTextureID5, g_iCTextureID6, oculusTex0);
					if ( iErrorCode > 0 )
					{
						sprintf ( pErrorStr, "Error starting VR : Code %d", iErrorCode );
						timestampactivity(0,pErrorStr);

						t.visuals.generalpromptstatetimer = Timer()+1000;
						t.visuals.generalprompt_s = "No OpenXR runtime found";
					}
					GGVR_SetGenericOffsetAngX( g.gvroffsetangx );
					GGVR_SetWMROffsetAngX( g.gvrwmroffsetangx );
				}
				#endif

				// and new SAO shader which is slower but nicer looking with SAO effect in place
				if (  GetEffectExist(g.postprocesseffectoffset+4) == 1  )  DeleteEffect ( g.postprocesseffectoffset+4 );
				t.tshaderchoice_s="effectbank\\reloaded\\post-sao.fx";
				LoadEffect ( t.tshaderchoice_s.Get(), g.postprocesseffectoffset+4,0 );
				SetVector4 ( g.terrainvectorindex,0,0,0,0 );
				SetEffectConstantV ( g.postprocesseffectoffset+4,"[hook-depth-data]", g.terrainvectorindex );
			}

			//  Light ray shade setup
			t.glightraycameraid=4;
			if (  t.glightraycameraid>0 ) 
			{
				//  load lightray shader and required texture
				t.lightrayshader_s="effectbank\\reloaded\\Scatter.fx";
				LoadEffect (  t.lightrayshader_s.Get(),g.postprocesseffectoffset+1,0 );
				LoadImage (  "effectbank\\reloaded\\media\\Sun.dds",g.postprocessimageoffset+2,0,g.gdividetexturesize );
				//  lightray camera
				CreateCamera (  t.glightraycameraid );
				SetCameraFOV (  t.glightraycameraid,75 );
				BackdropOn (  t.glightraycameraid );
				BackdropColor (  t.glightraycameraid,Rgb(0,0,0) );
				SetCameraAspect (  t.glightraycameraid,1.325f );
				//  scatter shader quad
				MakeObjectPlane (  g.postprocessobjectoffset+1,2,2,1 );
				SetObjectEffect (  g.postprocessobjectoffset+1,g.postprocesseffectoffset+1 );
				SetSphereRadius ( g.postprocessobjectoffset+1, 0 );
				g.postprocessobjectoffset0laststate = g.postprocesseffectoffset+1;
				//SetCameraToImage (  t.glightraycameraid,g.postprocessimageoffset+1,GetDisplayWidth()/2,GetDisplayHeight()/2 );
				SetCameraToImage (  t.glightraycameraid,g.postprocessimageoffset+1,GetDisplayWidth()/4,GetDisplayHeight()/4 );
				TextureObject (  g.postprocessobjectoffset+1,0,g.postprocessimageoffset+1 );
				SetObjectMask (  g.postprocessobjectoffset+1,(pow(2,0)) );
				///GhostObjectOn (  g.postprocessobjectoffset+1,0 );
				SetObjectTransparency ( g.postprocessobjectoffset+1,1 );
				DisableObjectZWrite ( g.postprocessobjectoffset+1 );
				DisableObjectZDepthEx ( g.postprocessobjectoffset+1, 0 );
				//  sun plane object
				MakeObjectPlane (  g.postprocessobjectoffset+2,1500*2,1500*2 );
				TextureObject (  g.postprocessobjectoffset+2,g.postprocessimageoffset+2 );
				SetObjectTransparency (  g.postprocessobjectoffset+2,1 );
				SetObjectLight (  g.postprocessobjectoffset+2,0 );
				SetObjectMask (  g.postprocessobjectoffset+2,pow(2,t.glightraycameraid) );
				SetObjectEffect ( g.postprocessobjectoffset+2, g.guishadereffectindex );
			}

			//  Assign all parameter handles
			if (  GetEffectExist(g.postprocesseffectoffset+1) == 1 ) 
			{
				t.effectparam.postprocess.LightDir=GetEffectParameterIndex(g.postprocesseffectoffset+1,"LightDir");
				t.effectparam.postprocess.AlphaAngle=GetEffectParameterIndex(g.postprocesseffectoffset+1,"AlphaAngle");
				t.effectparam.postprocess.LightRayLength=GetEffectParameterIndex(g.postprocesseffectoffset+1,"LightRayLength");
				t.effectparam.postprocess.LightRayQuality=GetEffectParameterIndex(g.postprocesseffectoffset+1,"LightRayQuality");
				t.effectparam.postprocess.LightRayDecay=GetEffectParameterIndex(g.postprocesseffectoffset+1,"LightRayDecay");
			}
			if (  GetEffectExist(g.postprocesseffectoffset+0) == 1 ) 
			{
				t.effectparam.postprocess.ScreenColor0=GetEffectParameterIndex(g.postprocesseffectoffset+0,"ScreenColor");
			}
			if (  GetEffectExist(g.postprocesseffectoffset+2) == 1 ) 
			{
				t.effectparam.postprocess.ScreenColor2=GetEffectParameterIndex(g.postprocesseffectoffset+2,"ScreenColor");
			}
			if (  GetEffectExist(g.postprocesseffectoffset+4) == 1 ) 
			{
				t.effectparam.postprocess.ScreenColor4=GetEffectParameterIndex(g.postprocesseffectoffset+4,"ScreenColor");
			}
		}
		else
		{
			//  Activate existing post process effect
			postprocess_on ( );
		}

		//  reset post process shader defaults
		postprocess_reset_fade();
	}
	// restore cam id when done
	SetCurrentCamera (  0 );
	#endif
}

void postprocess_reset_fade ( void )
{
	#ifdef WICKEDENGINE
	// Wicked has post processing covered!
	#else
	// reset post process shader defaults
	if ( t.gpostprocessmode>0 ) 
	{
		if (  GetEffectExist(g.postprocesseffectoffset+0) == 1 ) 
		{
			SetVector4 (  g.terrainvectorindex,0,0,0,0 );
			SetEffectConstantV (  g.postprocesseffectoffset+0,"ScreenColor",g.terrainvectorindex );
			SetEffectConstantV (  g.postprocesseffectoffset+2,"ScreenColor",g.terrainvectorindex );
			SetEffectConstantV (  g.postprocesseffectoffset+4,"ScreenColor",g.terrainvectorindex );
			t.postprocessings.fadeinvalue_f=0;
			SetVector4 (  g.terrainvectorindex,t.postprocessings.fadeinvalue_f,t.postprocessings.fadeinvalue_f,t.postprocessings.fadeinvalue_f,0 );
			SetEffectConstantV (  g.postprocesseffectoffset+0,"OverallColor",g.terrainvectorindex );
			SetEffectConstantV (  g.postprocesseffectoffset+2,"OverallColor",g.terrainvectorindex );
			SetEffectConstantV (  g.postprocesseffectoffset+4,"OverallColor",g.terrainvectorindex );
		}
	}
	#endif
}

void postprocess_general_init ( void )
{
	#ifdef WICKEDENGINE
	// Wicked has post processing covered!
	#else
	// called at start of prepare level sequence
	if (  GetEffectExist(g.decaleffectoffset) == 0 ) 
	{
		LoadEffect ( "effectbank\\reloaded\\decal_basic.fx", g.decaleffectoffset, 0 );
	}
	#endif
}

void postprocess_free ( void )
{
	// only free if enagaged
	if ( t.gpostprocessmode > 0 )
	{
		#ifdef VRTECH
		// free GGVR if used
		if ( g.vrglobals.GGVREnabled > 0 && g.vrglobals.GGVRUsingVRSystem == 1 )
		{
			GGVR_Shutdown();
		}
		#endif

		#ifdef WICKEDENGINE
		// Wicked has post processing covered!
		#else
		//  free post processing objects
		if (  ObjectExist(g.postprocessobjectoffset+0) == 1  )  DeleteObject (  g.postprocessobjectoffset+0 );
		if (  ObjectExist(g.postprocessobjectoffset+1) == 1  )  DeleteObject (  g.postprocessobjectoffset+1 );
		if (  ObjectExist(g.postprocessobjectoffset+2) == 1  )  DeleteObject (  g.postprocessobjectoffset+2 );
		if (  ObjectExist(g.postprocessobjectoffset+3) == 1  )  DeleteObject (  g.postprocessobjectoffset+3 );
		if (  ImageExist(g.postprocessimageoffset+2) == 1  )  DeleteImage (  g.postprocessimageoffset+2 );
		if (  ImageExist(g.postprocessimageoffset+3) == 1  )  DeleteImage (  g.postprocessimageoffset+3 );

		//  free any resources created by post process technique
		if (  GetEffectExist(g.postprocesseffectoffset+0) == 1  )  DeleteEffect (  g.postprocesseffectoffset+0 );
		if (  GetEffectExist(g.postprocesseffectoffset+2) == 1  )  DeleteEffect (  g.postprocesseffectoffset+2 );
		if (  GetEffectExist(g.postprocesseffectoffset+3) == 1  )  DeleteEffect (  g.postprocesseffectoffset+3 );
		if (  GetEffectExist(g.postprocesseffectoffset+4) == 1  )  DeleteEffect (  g.postprocesseffectoffset+4 );
		if (  g.gfinalrendercameraid>0 ) 
		{
			if (  CameraExist(g.gfinalrendercameraid) == 1 ) 
			{
				DestroyCamera (  g.gfinalrendercameraid );
			}
			g.gfinalrendercameraid=0;
		}

		//  free lightray shader
		if (  GetEffectExist(g.postprocesseffectoffset+1) == 1  )  DeleteEffect (  g.postprocesseffectoffset+1 );
		if (  t.glightraycameraid>0 ) 
		{
			if (  CameraExist(t.glightraycameraid) == 1 ) 
			{
				DestroyCamera (  t.glightraycameraid );
			}
			t.glightraycameraid=0;
		}

		//  Total reset
		g.gpostprocessingnotransparency=0;

		//  Restore main camera
		SetCurrentCamera (  0 );
		SetCameraView (  0,0,0,GetDesktopWidth(),GetDesktopHeight() );
		SetCameraToImage (  0,-1,0,0,0 );
		#endif

		// and reset flag
		t.gpostprocessmode=0;
	}
}

void postprocess_off ( void )
{
	#ifdef WICKEDENGINE
	// Wicked has post processing covered!
	#else
	if ( t.gpostprocessmode>0 ) 
	{
		if (  ObjectExist(g.postprocessobjectoffset+0) == 1  )  HideObject (  g.postprocessobjectoffset+0 );
		if (  ObjectExist(g.postprocessobjectoffset+1) == 1  )  HideObject (  g.postprocessobjectoffset+1 );
		if (  ObjectExist(g.postprocessobjectoffset+2) == 1  )  HideObject (  g.postprocessobjectoffset+2 );
		SetCameraToImage (  0,-1,0,0,0 );
		if (  g.gfinalrendercameraid>0 ) 
		{
			SetCameraView (  g.gfinalrendercameraid,0,0,1,1 );
		}
		if (  t.glightraycameraid>0 ) 
		{
			SetCameraView (  t.glightraycameraid,0,0,1,1 );
		}
	}
	#endif
}

void postprocess_on ( void )
{
	#ifdef WICKEDENGINE
	// Wicked has post processing covered!
	#else
	if ( t.gpostprocessmode>0 ) 
	{
		if ( ObjectExist(g.postprocessobjectoffset+0) == 1  )  ShowObject (  g.postprocessobjectoffset+0 );
		if ( ObjectExist(g.postprocessobjectoffset+1) == 1  )  ShowObject (  g.postprocessobjectoffset+1 );
		if ( ObjectExist(g.postprocessobjectoffset+2) == 1  )  ShowObject (  g.postprocessobjectoffset+2 );
		SetCameraToImage (  0,g.postprocessimageoffset+0,GetDisplayWidth(),GetDisplayHeight(),2 );
		if ( g.gfinalrendercameraid>0 ) 
		{
			SetCameraView (  g.gfinalrendercameraid,0,0,GetDisplayWidth(),GetDisplayHeight() );
		}
		if ( t.glightraycameraid>0 ) 
		{
			SetCameraView (  t.glightraycameraid,0,0,GetDisplayWidth(),GetDisplayHeight() );
		}
	}
	#endif
}

void postprocess_preterrain ( void )
{
#ifdef OPTICK_ENABLE
	OPTICK_EVENT();
#endif
	// Most rendering done in master for wicked, but some code from below remains for alignment of controls
	if ( g.vrglobals.GGVREnabled > 0 && g.vrglobals.GGVRUsingVRSystem == 1 )
	{
		if ( !GGVR_IsRuntimeFound() ) GGVR_ReInit();
		if ( !GGVR_IsRuntimeFound() )
		{
			t.visuals.generalpromptstatetimer = Timer()+1000;
			t.visuals.generalprompt_s = "OpenXR runtime not found";
		}
		else
		{
			// position VR player at location of main camera
			GGVR_SetPlayerPosition(t.tFinalCamX_f, t.tFinalCamY_f, t.tFinalCamZ_f);

			// this sets the origin based on the current camera zero (ARG!)
			// should only set based on player angle (minus HMD influence) as HMD added later at right time for smooth headset viewing!
			GGVR_SetPlayerAngleY(t.camangy_f);

			// update seated/standing flag
			g.vrglobals.GGVRStandingMode = GGVR_GetTrackingSpace();

			// handle teleport
			bool bAllowPlayerTeleport = false;
			if (bAllowPlayerTeleport == true)
			{
				float fTelePortDestX = 0.0f;
				float fTelePortDestY = 0.0f;
				float fTelePortDestZ = 0.0f;
				float fTelePortDestAngleY = 0.0f;
				bool VRteleport = GGVR_HandlePlayerTeleport (&fTelePortDestX, &fTelePortDestY, &fTelePortDestZ, &fTelePortDestAngleY);
				if (VRteleport)
				{
					physics_disableplayer ();
					t.terrain.playerx_f = fTelePortDestX;
					t.terrain.playery_f = fTelePortDestY + 30;
					t.terrain.playerz_f = fTelePortDestZ;
					physics_setupplayer ();
				}
			}

			// update HMD position and controller feedback
			bool bPlayerDucking = false;
			if ( t.aisystem.playerducking != 0 ) bPlayerDucking = true;
			int iBatchStart = g.batchobjectoffset;
			int iBatchEnd = g.batchobjectoffset + g.merged_new_objects + 1;
			GGVR_UpdatePlayer(bPlayerDucking,t.terrain.TerrainID,g.lightmappedobjectoffset,g.lightmappedobjectoffsetfinish,g.entityviewstartobj,g.entityviewendobj,iBatchStart,iBatchEnd);
						
			// set some values the player control script needs!
			GGVR_SetOpenXRValuesForMAX();
		}
	}
}

void postprocess_setscreencolor ( void )
{
	#ifdef WICKEDENGINE
	// Wicked has post processing covered

	float fade = GetXVector4(t.tColorVector);
	if (fade > 0.0)
	{
		extern bool bImGuiInTestGame;
		extern bool bRenderTabTab;
		extern bool bBlockImGuiUntilNewFrame;
		extern bool bImGuiRenderWithNoCustomTextures;
		extern bool g_bNoGGUntilGameGuruMainCalled;
		extern bool bImGuiFrameState;

		if ((bImGuiInTestGame) && !bRenderTabTab && !bImGuiFrameState)
		{
			//We need a new frame.
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			bRenderTabTab = true;
			bBlockImGuiUntilNewFrame = false;
			bImGuiRenderWithNoCustomTextures = false;
			extern bool bSpriteWinVisible;
			bSpriteWinVisible = false;
		}

		ImGuiViewport* mainviewport = ImGui::GetMainViewport();
		if (mainviewport)
		{
			ImDrawList* drawlist = ImGui::GetForegroundDrawList(mainviewport);
			if (drawlist)
			{
				ImVec4 monitor_col = ImVec4(0.0, 0.0, 0.0, fade); //Fade in.
				drawlist->AddRectFilled(ImVec2(-1, -1), ImGui::GetMainViewport()->Size, ImGui::GetColorU32(monitor_col));
			}
		}
	}
	#else
	SetEffectConstantVEx (  g.postprocesseffectoffset+0,t.effectparam.postprocess.ScreenColor0,t.tColorVector );
	SetEffectConstantVEx (  g.postprocesseffectoffset+2,t.effectparam.postprocess.ScreenColor2,t.tColorVector );
	SetEffectConstantVEx (  g.postprocesseffectoffset+4,t.effectparam.postprocess.ScreenColor4,t.tColorVector );
	#endif
}
