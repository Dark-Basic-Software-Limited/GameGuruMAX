#pragma once

//
// Classes to handle editor and rendering
//
#include "../../../WickedEngineDX12/WickedEngine/WickedEngine.h"

class MasterRenderer : public RenderPath3D
{
	protected:
		std::shared_ptr<wiResource> testTex;
		bool m_bRenderingVR;
	public:
		void Load() override;
		void Update(float dt) override;
		void Render( int mode ) const override;
		void Compose(wiGraphics::CommandList cmd) const override;
		void ResizeBuffers(void);
		void SetRenderingVR(bool bFlag) { m_bRenderingVR = bFlag; }
		void RenderOutlineHighlighers(wiGraphics::CommandList cmd) const;
};

class Master : public MainComponent
{
	public:
		MasterRenderer masterrenderer;

		void DeleteContentsOfDBPDATA(bool bOnlyIfOlderThan2DAYS);
		void MakeOrEnterUniqueDBPDATA(void);
		void DeleteAllOldDBPDATAFolders(void);
		void Initialize() override;
		void Update(float dt) override;
		void Finish(void);
		bool ForceRender(void * rt = NULL);
		void RunCustom();
		void GGMaxRun(void);
		void StopVR();

	public:
		void InitializeSecondaries();

		bool initializedSecondaries = false;
		wiGraphics::Texture g_pSplashTexture;
		bool b_gSplashTextureLoaded = false;
		bool bVsyncEnabled = true;
		int iAOSetting = RenderPath3D::AO_SSAO;// AO_DISABLED;
		float fAOPower = 1.0f;
		float fAORange = 40.0f;
		int iAOSamples = 16;
};

extern Master master;
