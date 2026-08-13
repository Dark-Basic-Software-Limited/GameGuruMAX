
//PE: Display all imgui DX11 errors.
//#define DEBUGDISPLAY

//PE: dear imgui: Renderer for DirectX11
// PE: Changed to be used in GameGuru.
// PE: Added additional dialog here.

// Includes 
#include "stdafx.h"
#include "commdlg.h"

#include "imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"
#include "imgui_gg_dx11.h"

#include "stdio.h"
#include <direct.h>

// DirectX
#include <stdio.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#ifdef _MSC_VER
#pragma comment(lib, "d3dcompiler") // Automatically link with d3dcompiler.lib as we are using D3DCompile() below.
#endif

#include "CFileC.h"
#include <Shlobj.h>

#include <algorithm>
#include <string>

#include "..\..\Guru-WickedMAX\wickedcalls.h"

#include "M-RPG.h"
#include "M-Workshop.h"
#include <deque>

char launchLoadOnStartup[260] = "\0";

// Names and types of 'readouts' - each one represents a variable somewhere in the engine, to be displayed in HUD
std::vector<std::string> readoutTitles;
std::vector<STORYBOARD_WIDGET_> readoutWidgetTypes;
std::vector<ReadoutLayers> readoutLayers;
std::vector<ReadoutTypes> readoutTypes;
std::vector<std::function<void()>> readoutCallbacks;

// Legacy WhiteList Globals
bool g_bCreateLegacyWhiteList = true;
std::vector<LPSTR> g_pLegacyWhiteList;

bool bImGuiFrameState = false;
bool bImGuiGotFocus = false;
bool bImGuiRenderTargetFocus = false;
bool bImGuiRenderTargetWantKeyboard = false;
bool bImGuiReadyToRender = false;
bool bImGuiInTestGame = false;
bool bBlockImGuiUntilNewFrame = false;
bool bImGuiRenderWithNoCustomTextures = false;
bool bBlockImGuiUntilFurtherNotice = false;
bool bImGuiInitDone = false;
int ImGuiStatusBar_Size = 0;

bool bPreviewWPE = false;
uint32_t PreviewWPERoot = 0;
float fPreviewYOffset = 0;
float fPreviewXOffset = 0;
float fPreviewZOffset = 0;

preferences pref;
bool g_bEnableAutoFlattenSystem = true;

ImVec2 vStartResolution = { 1280 , 800 }; // { 1024, 768 }; //1280x800
ImVec2 OldrenderTargetSize = { 0,0 };
ImVec2 OldrenderTargetPos = { 0,0 };
ImVec2 renderTargetAreaPos = { 0,0 };
ImVec2 renderTargetAreaSize = { 0,0 };
bool bCenterRenderView = false;

ImVec2 fImGuiScissorTopLeft = { 0, 0 };
ImVec2 fImGuiScissorBottomRight = { 0, 0 };

// DirectX data
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGIFactory*            g_pFactory = NULL;
static ID3D11Buffer*            g_pVB = NULL;
static ID3D11Buffer*            g_pIB = NULL;
static ID3D10Blob*              g_pVertexShaderBlob = NULL;
static ID3D11VertexShader*      g_pVertexShader = NULL;
static ID3D11InputLayout*       g_pInputLayout = NULL;
static ID3D11Buffer*            g_pVertexConstantBuffer = NULL;
static ID3D10Blob*              g_pPixelShaderBlob = NULL;
static ID3D11PixelShader*       g_pPixelShader = NULL;
static ID3D10Blob*              g_pPixelShaderBlobBlur = NULL;
static ID3D11PixelShader*       g_pPixelShaderBlur = NULL;

static ID3D10Blob*              g_pPixelShaderNoWhiteBlob = NULL;
static ID3D11PixelShader*       g_pPixelShaderNoWhite = NULL;
static ID3D10Blob*              g_pPixelShaderNoAlphaBlob = NULL;
static ID3D11PixelShader*       g_pPixelShaderNoAlpha = NULL;

static ID3D10Blob*              g_ppixelShaderBoost25Blob = NULL;
static ID3D11PixelShader*       g_ppixelShaderBoost25 = NULL;

static ID3D11SamplerState*      g_pFontSampler = NULL;
static ID3D11ShaderResourceView*g_pFontTextureView = NULL;
static ID3D11RasterizerState*   g_pRasterizerState = NULL;
static ID3D11BlendState*        g_pBlendState = NULL;
static ID3D11DepthStencilState* g_pDepthStencilState = NULL;
static int                      g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;

struct VERTEX_CONSTANT_BUFFER
{
    float   mvp[4][4];
};

std::vector<SliderData> g_SliderData; // For storing max values for sliders.
std::vector<std::vector<std::string>> luadropdownlabels; // Storage for dropdown labels extracted from lua script.
std::vector<std::string> g_DLuaVariableNames;

// Forward Declarations
static void ImGui_ImplDX11_InitPlatformInterface();
static void ImGui_ImplDX11_ShutdownPlatformInterface();

static void ImGui_ImplDX11_SetupRenderState(ImDrawData* draw_data, ID3D11DeviceContext* ctx, bool nowhite = false )
{
    // Setup viewport
    D3D11_VIEWPORT vp;
    memset(&vp, 0, sizeof(D3D11_VIEWPORT));
    vp.Width = draw_data->DisplaySize.x;
    vp.Height = draw_data->DisplaySize.y;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = vp.TopLeftY = 0;
    ctx->RSSetViewports(1, &vp);

    // Setup shader and vertex buffers
    unsigned int stride = sizeof(ImDrawVert);
    unsigned int offset = 0;
    ctx->IASetInputLayout(g_pInputLayout);
    ctx->IASetVertexBuffers(0, 1, &g_pVB, &stride, &offset);
    ctx->IASetIndexBuffer(g_pIB, sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->VSSetShader(g_pVertexShader, NULL, 0);
    ctx->VSSetConstantBuffers(0, 1, &g_pVertexConstantBuffer);
	if(nowhite)
		ctx->PSSetShader(g_pPixelShaderNoWhite, NULL, 0);
	else
		ctx->PSSetShader(g_pPixelShader, NULL, 0);
	
    ctx->PSSetSamplers(0, 1, &g_pFontSampler);
    ctx->GSSetShader(NULL, NULL, 0);
    ctx->HSSetShader(NULL, NULL, 0); // In theory we should backup and restore this as well.. very infrequently used..
    ctx->DSSetShader(NULL, NULL, 0); // In theory we should backup and restore this as well.. very infrequently used..
    ctx->CSSetShader(NULL, NULL, 0); // In theory we should backup and restore this as well.. very infrequently used..

    // Setup blend state
    const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
    ctx->OMSetBlendState(g_pBlendState, blend_factor, 0xffffffff);
    ctx->OMSetDepthStencilState(g_pDepthStencilState, 0);
    ctx->RSSetState(g_pRasterizerState);
}

extern bool bRenderTabTab;
extern bool bRenderNextFrame;

bool bForceRenderEverywhere = false;

std::vector<ID3D11ShaderResourceView*> lpBadTexture;
std::deque<std::vector<ID3D11ShaderResourceView*>> badTextureFrames;
const size_t MAX_FRAMES_TO_KEEP = 3;

void ImGuiHook_RenderCall_Direct(void* ctxptr, void* d3dptr)
{
	if (bBlockImGuiUntilNewFrame || bBlockImGuiUntilFurtherNotice)
		return;
	if (bRenderNextFrame)
	{
		//bRenderNextFrame = false;
	}
	else
	{
		if (bImGuiInTestGame && !bRenderTabTab)
			return;
		//	if (bRenderTabTab)
		//		bRenderTabTab = false;
	}

	// goes through the same sequence as 'ImGui_ImplDX11_RenderDrawData' but Wicked Friendly..
	ID3D11DeviceContext* ctx = (ID3D11DeviceContext*)ctxptr;
	ID3D11Device* pd3dDevice = (ID3D11Device*) d3dptr;

	ImDrawData* draw_data = ImGui::GetDrawData();

	// Avoid rendering if no data
	if (draw_data == NULL)
		return;

	// Avoid rendering when minimized
	if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
		return;

	// Create and grow vertex/index buffers if needed
	if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
	{
		if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
		g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
		D3D11_BUFFER_DESC desc;
		memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = g_VertexBufferSize * sizeof(ImDrawVert);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		if (pd3dDevice->CreateBuffer(&desc, NULL, &g_pVB) < 0)
			return;
	}
	if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
	{
		if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
		g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
		D3D11_BUFFER_DESC desc;
		memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = g_IndexBufferSize * sizeof(ImDrawIdx);
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		if (pd3dDevice->CreateBuffer(&desc, NULL, &g_pIB) < 0)
			return;
	}

	// Upload vertex/index data into a single contiguous GPU buffer
	D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
	if (ctx->Map(g_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource) != S_OK)
		return;
	if (ctx->Map(g_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource) != S_OK)
		return;
	ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource.pData;
	ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource.pData;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtx_dst += cmd_list->VtxBuffer.Size;
		idx_dst += cmd_list->IdxBuffer.Size;
	}
	ctx->Unmap(g_pVB, 0);
	ctx->Unmap(g_pIB, 0);

	// Setup orthographic projection matrix into our constant buffer
	// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
	{
		D3D11_MAPPED_SUBRESOURCE mapped_resource;
		if (ctx->Map(g_pVertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource) != S_OK)
			return;
		VERTEX_CONSTANT_BUFFER* constant_buffer = (VERTEX_CONSTANT_BUFFER*)mapped_resource.pData;
		float L = draw_data->DisplayPos.x;
		float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
		float T = draw_data->DisplayPos.y;
		float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
		float mvp[4][4] =
		{
			{ 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
			{ 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
			{ 0.0f,         0.0f,           0.5f,       0.0f },
			{ (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
		};
		memcpy(&constant_buffer->mvp, mvp, sizeof(mvp));
		ctx->Unmap(g_pVertexConstantBuffer, 0);
	}

	// Setup desired DX state
	ImGui_ImplDX11_SetupRenderState(draw_data, ctx);

	badTextureFrames.push_back(std::move(lpBadTexture));

	// Render command lists
	// (Because we merged all buffers into a single one, we maintain our own offset into them)
	int global_idx_offset = 0;
	int global_vtx_offset = 0;
	ImVec2 clip_off = draw_data->DisplayPos;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			
			if (!pcmd->TextureId)
				continue;

			bool bBadTextureAll = false;
			for (const auto& frameTextures : badTextureFrames)
			{
				//PE: Check all frames for bad texture_srv.
				for (int i = 0; i < frameTextures.size(); i++)
				{
					ID3D11ShaderResourceView* texture_srv = (ID3D11ShaderResourceView*)pcmd->TextureId;
					if (texture_srv == frameTextures[i])
					{
						bBadTextureAll = true;
						break;
					}
				}
			}
			if (bBadTextureAll)
				continue;

			if (pcmd->UserCallback == (ImDrawCallback)10)
			{
				bForceRenderEverywhere = true;
			}
			else if (pcmd->UserCallback == (ImDrawCallback)11)
			{
				bForceRenderEverywhere = false;
			}
			else if (pcmd->UserCallback == (ImDrawCallback)1)
			{
				//PE: Change shaders.
				// for now we ignore shader changes mid-processing, put back when we see something!
				ctx->PSSetShader(g_pPixelShaderNoWhite, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)2)
			{
				// for now we ignore shader changes mid-processing, put back when we see something!
				ctx->PSSetShader(g_pPixelShader, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)3)
			{
				//NoAlpha.
				// for now we ignore shader changes mid-processing, put back when we see something!
				ctx->PSSetShader(g_pPixelShaderNoAlpha, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)4)
			{
				//Alpha.
				// for now we ignore shader changes mid-processing, put back when we see something!
				ctx->PSSetShader(g_pPixelShader, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)5)
			{
				//boost colors 25 percent.
				ctx->PSSetShader(g_ppixelShaderBoost25, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)6)
			{
				//Blur
				ctx->PSSetShader(g_pPixelShaderBlur, NULL, 0);
			}
			else if (pcmd->UserCallback != NULL)
			{
				// User callback, registered via ImDrawList::AddCallback()
				// (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
				// for now we ignore shader changes mid-processing, put back when we see something!
				//if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
				//    ImGui_ImplDX11_SetupRenderState(draw_data, ctx);
				//else
				//    pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				// Apply scissor/clipping rectangle
				const D3D11_RECT r = { (LONG)(pcmd->ClipRect.x - clip_off.x), (LONG)(pcmd->ClipRect.y - clip_off.y), (LONG)(pcmd->ClipRect.z - clip_off.x), (LONG)(pcmd->ClipRect.w - clip_off.y) };
				ctx->RSSetScissorRects(1, &r);

				// Bind texture, Draw

				//ID3D11ShaderResourceView* lpTexture = GetImagePointerView(iImageID);
				ID3D11ShaderResourceView* texture_srv = (ID3D11ShaderResourceView*)pcmd->TextureId;

				//Locate bad textures.
				bool bBadTexture = false;
				//PE: Prevent imgui from crashing when rendering using a deleted ID3D11ShaderResourceView.
				if (lpBadTexture.size() > 0)
				{
					for (int i = 0; i < lpBadTexture.size(); i++)
					{
						if (texture_srv == lpBadTexture[i])
						{
							bBadTexture = true;
							break;
						}
					}
				}
				if (!bBadTexture)
				{
					ctx->PSSetShaderResources(0, 1, &texture_srv);
					ctx->DrawIndexed(pcmd->ElemCount, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset);
					ID3D11ShaderResourceView *const pSRV[1] = { NULL };
					ctx->PSSetShaderResources(0, 1, pSRV);
				}
			}
		}
		global_idx_offset += cmd_list->IdxBuffer.Size;
		global_vtx_offset += cmd_list->VtxBuffer.Size;
	}

	lpBadTexture.clear();

	if (badTextureFrames.size() > MAX_FRAMES_TO_KEEP) {
		badTextureFrames.pop_front();
	}

}

// New Render function which is now called from Wicked Engine LIB
extern bool bDigAHoleToHWND;
D3D11_RECT rD3D11DigAHole = { 300,300,600,600 };


bool IntersectsWith(D3D11_RECT rect , D3D11_RECT compare)
{

	if (rect.left <= compare.right && rect.right >= compare.left && rect.top <= compare.bottom)
	{
		return rect.bottom >= compare.top;
	}

	return false;
}

void ImGuiHook_RenderCall(void* ctxptr)
{
	extern bool g_bNoGGUntilGameGuruMainCalled;
	if (g_bNoGGUntilGameGuruMainCalled==false)
		return;
	extern bool g_bNo2DRender;
	if (g_bNo2DRender)
		return;
	bool bSpecialNoCustomTextureRender = false;
	if (bBlockImGuiUntilNewFrame || bBlockImGuiUntilFurtherNotice)
	{
		if (bBlockImGuiUntilNewFrame && bImGuiRenderWithNoCustomTextures)
		{
			bSpecialNoCustomTextureRender = true;
		}
		else
		{
			return;
		}
	}

	if (bRenderNextFrame)
	{
		//PE: If we have zero sprites and if (bImGuiInTestGame && !bRenderTabTab) , we need to return.
		
		//bRenderNextFrame = false;
	} 
	else
	{
		if (bImGuiInTestGame && !bRenderTabTab)
			return;
		//	if (bRenderTabTab)
		//		bRenderTabTab = false;
	}

	// goes through the same sequence as 'ImGui_ImplDX11_RenderDrawData' but Wicked Friendly..
    ID3D11DeviceContext* ctx = (ID3D11DeviceContext*)ctxptr;

	ImDrawData* draw_data = ImGui::GetDrawData();

	// Avoid rendering if no data
	if (draw_data == NULL)
		return;

    // Avoid rendering when minimized
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

    // Create and grow vertex/index buffers if needed
    if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
    {
        if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
        g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = g_VertexBufferSize * sizeof(ImDrawVert);
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        if (g_pd3dDevice->CreateBuffer(&desc, NULL, &g_pVB) < 0)
            return;
    }
    if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
    {
        if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
        g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = g_IndexBufferSize * sizeof(ImDrawIdx);
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (g_pd3dDevice->CreateBuffer(&desc, NULL, &g_pIB) < 0)
            return;
    }

    // Upload vertex/index data into a single contiguous GPU buffer
    D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
    if (ctx->Map(g_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource) != S_OK)
        return;
    if (ctx->Map(g_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource) != S_OK)
        return;
    ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource.pData;
    ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource.pData;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtx_dst += cmd_list->VtxBuffer.Size;
        idx_dst += cmd_list->IdxBuffer.Size;
    }
    ctx->Unmap(g_pVB, 0);
    ctx->Unmap(g_pIB, 0);

    // Setup orthographic projection matrix into our constant buffer
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    {
        D3D11_MAPPED_SUBRESOURCE mapped_resource;
        if (ctx->Map(g_pVertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource) != S_OK)
            return;
        VERTEX_CONSTANT_BUFFER* constant_buffer = (VERTEX_CONSTANT_BUFFER*)mapped_resource.pData;
        float L = draw_data->DisplayPos.x;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        float T = draw_data->DisplayPos.y;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
        float mvp[4][4] =
        {
            { 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
            { 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
            { 0.0f,         0.0f,           0.5f,       0.0f },
            { (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f },
        };
        memcpy(&constant_buffer->mvp, mvp, sizeof(mvp));
        ctx->Unmap(g_pVertexConstantBuffer, 0);
    }

    // Setup desired DX state
    ImGui_ImplDX11_SetupRenderState(draw_data, ctx);

	/*
	if (bDigAHoleToHWND)
	{
		//PE: Cant use it without a geometry shaders.
		//PE: Need 4 viewports.
		D3D11_VIEWPORT vp[4];
		for (int i = 0; i < 4; i++)
		{
			memset(&vp[i], 0, sizeof(D3D11_VIEWPORT));
			vp[i].Width = draw_data->DisplaySize.x;
			vp[i].Height = draw_data->DisplaySize.y;
			vp[i].MinDepth = 0.0f;
			vp[i].MaxDepth = 1.0f;
			vp[i].TopLeftX = vp[i].TopLeftY = 0;
		}
		ctx->RSSetViewports(4, &vp[0]);
	}
	*/

	ImGuiContext& g = *GImGui;

	badTextureFrames.push_back(std::move(lpBadTexture));

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_idx_offset = 0;
    int global_vtx_offset = 0;
    ImVec2 clip_off = draw_data->DisplayPos;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

			if (!pcmd->TextureId)
				continue;

			bool bBadTextureAll = false;
			for (const auto& frameTextures : badTextureFrames)
			{
				//PE: Check all frames for bad texture_srv.
				for (int i = 0; i < frameTextures.size(); i++)
				{
					ID3D11ShaderResourceView* texture_srv = (ID3D11ShaderResourceView*)pcmd->TextureId;
					if (texture_srv == frameTextures[i])
					{
						bBadTextureAll = true;
						break;
					}
				}
			}
			if (bBadTextureAll)
				continue;

			if (pcmd->UserCallback == (ImDrawCallback)10)
			{
				bForceRenderEverywhere = true;
			}
			else if (pcmd->UserCallback == (ImDrawCallback)11)
			{
				bForceRenderEverywhere = false;
			}
			else if (pcmd->UserCallback == (ImDrawCallback) 1)
			{
				//PE: Change shaders.
				// for now we ignore shader changes mid-processing, put back when we see something!
				ctx->PSSetShader(g_pPixelShaderNoWhite, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)2)
			{
				// for now we ignore shader changes mid-processing, put back when we see something!
				ctx->PSSetShader(g_pPixelShader, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)3)
			{
				//NoAlpha.
				// for now we ignore shader changes mid-processing, put back when we see something!
				ctx->PSSetShader(g_pPixelShaderNoAlpha, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)4)
			{
				//Alpha.
				// for now we ignore shader changes mid-processing, put back when we see something!
				ctx->PSSetShader(g_pPixelShader, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)5)
			{
				//boost colors 25 percent.
				ctx->PSSetShader(g_ppixelShaderBoost25, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)6)
			{
				//Blur
				ctx->PSSetShader(g_pPixelShaderBlur, NULL, 0);
			}
			else if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
				// for now we ignore shader changes mid-processing, put back when we see something!
                //if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                //    ImGui_ImplDX11_SetupRenderState(draw_data, ctx);
                //else
                //    pcmd->UserCallback(cmd_list, pcmd);
            }
			else if(bSpecialNoCustomTextureRender)
			{
				//PE: Only render eveything but no special textures.
				if (pcmd->TextureId == g.Font->ContainerAtlas->TexID)
				{
					// Apply scissor/clipping rectangle
					const D3D11_RECT r = { (LONG)(pcmd->ClipRect.x - clip_off.x), (LONG)(pcmd->ClipRect.y - clip_off.y), (LONG)(pcmd->ClipRect.z - clip_off.x), (LONG)(pcmd->ClipRect.w - clip_off.y) };
					ctx->RSSetScissorRects(1, &r);

					// Bind texture, Draw
					ID3D11ShaderResourceView* texture_srv = (ID3D11ShaderResourceView*)pcmd->TextureId;

					bool bBadTexture = false;
					//PE: Prevent imgui from crashing when rendering using a deleted ID3D11ShaderResourceView.
					if (lpBadTexture.size() > 0)
					{
						for (int i = 0; i < lpBadTexture.size(); i++)
						{
							if (texture_srv == lpBadTexture[i])
							{
								bBadTexture = true;
								break;
							}
						}
					}

					if (!bBadTexture)
					{
						//PE: Strange got one today, just protect it for now.
						try
						{
							// PREBEN, this is the line that crashes when exiting a blank test game
							ctx->PSSetShaderResources(0, 1, &texture_srv);
							// PREBEN, this is the line that crashes when exiting a blank test game
							ctx->DrawIndexed(pcmd->ElemCount, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset);
							ID3D11ShaderResourceView *const pSRV[1] = { NULL };
							ctx->PSSetShaderResources(0, 1, pSRV);
						}
						catch (...)
						{
							ID3D11ShaderResourceView *const pSRV[1] = { NULL };
							ctx->PSSetShaderResources(0, 1, pSRV);
							return;
						}
					}
				}
			}
            else
            {
                // Apply scissor/clipping rectangle

				if (bDigAHoleToHWND && !bForceRenderEverywhere)
				{
					const D3D11_RECT rAll = { (LONG)draw_data->DisplayPos.x - clip_off.x, draw_data->DisplayPos.y - clip_off.y, (LONG)draw_data->DisplayPos.x + draw_data->DisplaySize.x, (LONG)draw_data->DisplayPos.y + draw_data->DisplaySize.y };
					D3D11_RECT r[4];
					for (int i = 0; i < 4; i++)
					{
						r[i] = rAll;
					}
					r[0].bottom = rD3D11DigAHole.top; //Rect 1
					r[1].top = rD3D11DigAHole.top; //Rect 2
					r[1].right = rD3D11DigAHole.left; //Rect 2
					r[2].top = rD3D11DigAHole.bottom; //Rect 3
					r[2].left = rD3D11DigAHole.left; //Rect 3
					r[3].top = rD3D11DigAHole.top; //Rect 4
					r[3].left = rD3D11DigAHole.right; //Rect 4
					r[3].bottom = rD3D11DigAHole.bottom; //Rect 4
					ID3D11ShaderResourceView* texture_srv = (ID3D11ShaderResourceView*)pcmd->TextureId;

					bool bBadTexture = false;
					//PE: Prevent imgui from crashing when rendering using a deleted ID3D11ShaderResourceView.
					if (lpBadTexture.size() > 0)
					{
						for (int i = 0; i < lpBadTexture.size(); i++)
						{
							if (texture_srv == lpBadTexture[i])
							{
								bBadTexture = true;
								break;
							}
						}
					}
					if (!bBadTexture)
					{
						for (int i = 0; i < 3; i++)
						{
							ctx->RSSetScissorRects(1, &r[i]);
							try
							{
								ctx->PSSetShaderResources(0, 1, &texture_srv);
								ctx->DrawIndexed(pcmd->ElemCount, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset);
								ID3D11ShaderResourceView *const pSRV[1] = { NULL };
								ctx->PSSetShaderResources(0, 1, pSRV);
							}
							catch (...)
							{
								ID3D11ShaderResourceView *const pSRV[1] = { NULL };
								ctx->PSSetShaderResources(0, 1, pSRV);
								return;
							}
						}
					}
					ctx->RSSetScissorRects(1, &r[3]);
				}
				else
				{
					const D3D11_RECT r = { (LONG)(pcmd->ClipRect.x - clip_off.x), (LONG)(pcmd->ClipRect.y - clip_off.y), (LONG)(pcmd->ClipRect.z - clip_off.x), (LONG)(pcmd->ClipRect.w - clip_off.y) };
					ctx->RSSetScissorRects(1, &r);
				}

                // Bind texture, Draw
	            ID3D11ShaderResourceView* texture_srv = (ID3D11ShaderResourceView*)pcmd->TextureId;

				bool bBadTexture = false;
				//PE: Prevent imgui from crashing when rendering using a deleted ID3D11ShaderResourceView.
				if (lpBadTexture.size() > 0)
				{
					for (int i = 0; i < lpBadTexture.size(); i++)
					{
						if (texture_srv == lpBadTexture[i])
						{
							bBadTexture = true;
							break;
						}
					}
				}
				if (!bBadTexture)
				{

					//PE: Strange got one today, just protect it for now.
					try
					{
						// PREBEN, this is the line that crashes when exiting a blank test game
						ctx->PSSetShaderResources(0, 1, &texture_srv);
						// PREBEN, this is the line that crashes when exiting a blank test game
						ctx->DrawIndexed(pcmd->ElemCount, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset);
						ID3D11ShaderResourceView *const pSRV[1] = { NULL };
						ctx->PSSetShaderResources(0, 1, pSRV);
					}
					catch (...)
					{
						ID3D11ShaderResourceView *const pSRV[1] = { NULL };
						ctx->PSSetShaderResources(0, 1, pSRV);
						return;
					}

				}

            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }

	lpBadTexture.clear();
	
	if (badTextureFrames.size() > MAX_FRAMES_TO_KEEP) {
		badTextureFrames.pop_front();
	}

}

bool ImGuiHook_GetScissorArea(float* pX1, float* pY1, float* pX2, float* pY2)
{
	extern bool m_bForceRender;
	if (m_bForceRender)
		return false;

	if (1)
	{
		if (bDigAHoleToHWND)
		{
			*pX1 = rD3D11DigAHole.left;
			*pY1 = rD3D11DigAHole.top;
			*pX2 = rD3D11DigAHole.right;
			*pY2 = rD3D11DigAHole.bottom;
		}
		else
		{
			*pX1 = fImGuiScissorTopLeft.x;
			*pY1 = fImGuiScissorTopLeft.y;
			*pX2 = fImGuiScissorBottomRight.x;
			*pY2 = fImGuiScissorBottomRight.y;
		}
		return true;
	}
	else
		return false;
}

// Render function
// (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call this directly from your main loop)
void ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

	HRESULT hr;

    ID3D11DeviceContext* ctx = g_pd3dDeviceContext;
	#ifdef DEBUGDISPLAY
	if (!ctx) MessageBoxA(NULL, "(RenderDrawData) ctx==NULL", "Debug", 0);
	#endif

    // Create and grow vertex/index buffers if needed
    if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
    {
        if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
        g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = g_VertexBufferSize * sizeof(ImDrawVert);
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
		if (hr=g_pd3dDevice->CreateBuffer(&desc, NULL, &g_pVB) < 0)
		{
			#ifdef DEBUGDISPLAY
			if (FAILED(hr))
			{
				char tmp[255];
				sprintf(tmp, "(RenderDrawData) g_pd3dDevice->CreateBuffer failed (%ld)", hr);
				MessageBoxA(NULL, tmp, "Debug", 0);
			}
			#endif
			return;
		}
    }
    if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
    {
        if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
        g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = g_IndexBufferSize * sizeof(ImDrawIdx);
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		if (hr=g_pd3dDevice->CreateBuffer(&desc, NULL, &g_pIB) < 0)
		{
			#ifdef DEBUGDISPLAY
			if (FAILED(hr))
			{
				char tmp[255];
				sprintf(tmp, "(RenderDrawData2) g_pd3dDevice->CreateBuffer failed (%ld)", hr);
				MessageBoxA(NULL, tmp, "Debug", 0);
			}
			#endif

			return;
		}
    }

    // Upload vertex/index data into a single contiguous GPU buffer
    D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
	if (hr=ctx->Map(g_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource) != S_OK)
	{
		#ifdef DEBUGDISPLAY
		if (FAILED(hr))
		{
			char tmp[255];
			sprintf(tmp, "(RenderDrawData) ctx->Map vtx_resource failed (%ld)", hr);
			MessageBoxA(NULL, tmp, "Debug", 0);
		}
		#endif
		return;
	}
	if (hr = ctx->Map(g_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource) != S_OK)
	{
		#ifdef DEBUGDISPLAY
		if (FAILED(hr))
		{
			char tmp[255];
			sprintf(tmp, "(RenderDrawData) ctx->Map idx_resource failed (%ld)", hr);
			MessageBoxA(NULL, tmp, "Debug", 0);
		}
		#endif
		return;
	}
    ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource.pData;
    ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource.pData;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtx_dst += cmd_list->VtxBuffer.Size;
        idx_dst += cmd_list->IdxBuffer.Size;
    }
    ctx->Unmap(g_pVB, 0);
    ctx->Unmap(g_pIB, 0);

    // Setup orthographic projection matrix into our constant buffer
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    {
        D3D11_MAPPED_SUBRESOURCE mapped_resource;
		if (hr = ctx->Map(g_pVertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource) != S_OK)
		{
			#ifdef DEBUGDISPLAY
			if (FAILED(hr))
			{
				char tmp[255];
				sprintf(tmp, "(RenderDrawData) ctx->Map mapped_resource failed (%ld)", hr);
				MessageBoxA(NULL, tmp, "Debug", 0);
			}
			#endif
			return;
		}
        VERTEX_CONSTANT_BUFFER* constant_buffer = (VERTEX_CONSTANT_BUFFER*)mapped_resource.pData;
        float L = draw_data->DisplayPos.x;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        float T = draw_data->DisplayPos.y;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
        float mvp[4][4] =
        {
            { 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
            { 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
            { 0.0f,         0.0f,           0.5f,       0.0f },
            { (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f },
        };
        memcpy(&constant_buffer->mvp, mvp, sizeof(mvp));
        ctx->Unmap(g_pVertexConstantBuffer, 0);
    }

    // Backup DX state that will be modified to restore it afterwards (unfortunately this is very ugly looking and verbose. Close your eyes!)
    struct BACKUP_DX11_STATE
    {
        UINT                        ScissorRectsCount, ViewportsCount;
        D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
        D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
        ID3D11RasterizerState*      RS;
        ID3D11BlendState*           BlendState;
        FLOAT                       BlendFactor[4];
        UINT                        SampleMask;
        UINT                        StencilRef;
        ID3D11DepthStencilState*    DepthStencilState;
        ID3D11ShaderResourceView*   PSShaderResource;
        ID3D11SamplerState*         PSSampler;
        ID3D11PixelShader*          PS;
        ID3D11VertexShader*         VS;
        ID3D11GeometryShader*       GS;
        UINT                        PSInstancesCount, VSInstancesCount, GSInstancesCount;
        ID3D11ClassInstance         *PSInstances[256], *VSInstances[256], *GSInstances[256];   // 256 is max according to PSSetShader documentation
        D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
        ID3D11Buffer*               IndexBuffer, *VertexBuffer, *VSConstantBuffer;
        UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
        DXGI_FORMAT                 IndexBufferFormat;
        ID3D11InputLayout*          InputLayout;
    };
    BACKUP_DX11_STATE old;
    old.ScissorRectsCount = old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    ctx->RSGetScissorRects(&old.ScissorRectsCount, old.ScissorRects);
    ctx->RSGetViewports(&old.ViewportsCount, old.Viewports);
    ctx->RSGetState(&old.RS);
    ctx->OMGetBlendState(&old.BlendState, old.BlendFactor, &old.SampleMask);
    ctx->OMGetDepthStencilState(&old.DepthStencilState, &old.StencilRef);
    ctx->PSGetShaderResources(0, 1, &old.PSShaderResource);
    ctx->PSGetSamplers(0, 1, &old.PSSampler);
    old.PSInstancesCount = old.VSInstancesCount = old.GSInstancesCount = 256;
    ctx->PSGetShader(&old.PS, old.PSInstances, &old.PSInstancesCount);
    ctx->VSGetShader(&old.VS, old.VSInstances, &old.VSInstancesCount);
    ctx->VSGetConstantBuffers(0, 1, &old.VSConstantBuffer);
    ctx->GSGetShader(&old.GS, old.GSInstances, &old.GSInstancesCount);

    ctx->IAGetPrimitiveTopology(&old.PrimitiveTopology);
    ctx->IAGetIndexBuffer(&old.IndexBuffer, &old.IndexBufferFormat, &old.IndexBufferOffset);
    ctx->IAGetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
    ctx->IAGetInputLayout(&old.InputLayout);

    // Setup desired DX state
    ImGui_ImplDX11_SetupRenderState(draw_data, ctx);

	badTextureFrames.push_back(std::move(lpBadTexture));

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_idx_offset = 0;
    int global_vtx_offset = 0;
    ImVec2 clip_off = draw_data->DisplayPos;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

			if (!pcmd->TextureId)
				continue;

			bool bBadTextureAll = false;
			for (const auto& frameTextures : badTextureFrames)
			{
				//PE: Check all frames for bad texture_srv.
				for (int i = 0; i < frameTextures.size(); i++)
				{
					ID3D11ShaderResourceView* texture_srv = (ID3D11ShaderResourceView*)pcmd->TextureId;
					if (texture_srv == frameTextures[i])
					{
						bBadTextureAll = true;
						break;
					}
				}
			}
			if (bBadTextureAll)
				continue;

			if (pcmd->UserCallback == (ImDrawCallback)10)
			{
				bForceRenderEverywhere = true;
			}
			else if (pcmd->UserCallback == (ImDrawCallback)11)
			{
				bForceRenderEverywhere = false;
			}
			else if (pcmd->UserCallback == (ImDrawCallback) 1)
			{
				//PE: Change shaders.
				ctx->PSSetShader(g_pPixelShaderNoWhite, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)2)
			{
				ctx->PSSetShader(g_pPixelShader, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)3)
			{
				//NoAlpha.
				ctx->PSSetShader(g_pPixelShaderNoAlpha, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)4)
			{
				//Alpha.
				ctx->PSSetShader(g_pPixelShader, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)5)
			{
				//boost colors 25 percent.
				ctx->PSSetShader(g_ppixelShaderBoost25, NULL, 0);
			}
			else if (pcmd->UserCallback == (ImDrawCallback)6)
			{
				//Blur
				ctx->PSSetShader(g_pPixelShaderBlur, NULL, 0);
			}
			else if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplDX11_SetupRenderState(draw_data, ctx);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Apply scissor/clipping rectangle
                const D3D11_RECT r = { (LONG)(pcmd->ClipRect.x - clip_off.x), (LONG)(pcmd->ClipRect.y - clip_off.y), (LONG)(pcmd->ClipRect.z - clip_off.x), (LONG)(pcmd->ClipRect.w - clip_off.y) };
                ctx->RSSetScissorRects(1, &r);

                // Bind texture, Draw
                ID3D11ShaderResourceView* texture_srv = (ID3D11ShaderResourceView*)pcmd->TextureId;

				bool bBadTexture = false;
				//PE: Prevent imgui from crashing when rendering using a deleted ID3D11ShaderResourceView.
				if (lpBadTexture.size() > 0)
				{
					for (int i = 0; i < lpBadTexture.size(); i++)
					{
						if (texture_srv == lpBadTexture[i])
						{
							bBadTexture = true;
							break;
						}
					}
				}
				if (!bBadTexture)
				{
					try
					{
						ctx->PSSetShaderResources(0, 1, &texture_srv);
						ctx->DrawIndexed(pcmd->ElemCount, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset);
						ID3D11ShaderResourceView *const pSRV[1] = { NULL };
						ctx->PSSetShaderResources(0, 1, pSRV);
					}
					catch (...)
					{
						ID3D11ShaderResourceView *const pSRV[1] = { NULL };
						ctx->PSSetShaderResources(0, 1, pSRV);
						return;
					}
				}
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }

	lpBadTexture.clear();

	if (badTextureFrames.size() > MAX_FRAMES_TO_KEEP) {
		badTextureFrames.pop_front();
	}

    // Restore modified DX state
    ctx->RSSetScissorRects(old.ScissorRectsCount, old.ScissorRects);
    ctx->RSSetViewports(old.ViewportsCount, old.Viewports);
    ctx->RSSetState(old.RS); if (old.RS) old.RS->Release();
    ctx->OMSetBlendState(old.BlendState, old.BlendFactor, old.SampleMask); if (old.BlendState) old.BlendState->Release();
    ctx->OMSetDepthStencilState(old.DepthStencilState, old.StencilRef); if (old.DepthStencilState) old.DepthStencilState->Release();
    ctx->PSSetShaderResources(0, 1, &old.PSShaderResource); if (old.PSShaderResource) old.PSShaderResource->Release();
    ctx->PSSetSamplers(0, 1, &old.PSSampler); if (old.PSSampler) old.PSSampler->Release();
    ctx->PSSetShader(old.PS, old.PSInstances, old.PSInstancesCount); if (old.PS) old.PS->Release();
    for (UINT i = 0; i < old.PSInstancesCount; i++) if (old.PSInstances[i]) old.PSInstances[i]->Release();
    ctx->VSSetShader(old.VS, old.VSInstances, old.VSInstancesCount); if (old.VS) old.VS->Release();
    ctx->VSSetConstantBuffers(0, 1, &old.VSConstantBuffer); if (old.VSConstantBuffer) old.VSConstantBuffer->Release();
    ctx->GSSetShader(old.GS, old.GSInstances, old.GSInstancesCount); if (old.GS) old.GS->Release();
    for (UINT i = 0; i < old.VSInstancesCount; i++) if (old.VSInstances[i]) old.VSInstances[i]->Release();
    ctx->IASetPrimitiveTopology(old.PrimitiveTopology);
    ctx->IASetIndexBuffer(old.IndexBuffer, old.IndexBufferFormat, old.IndexBufferOffset); if (old.IndexBuffer) old.IndexBuffer->Release();
    ctx->IASetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset); if (old.VertexBuffer) old.VertexBuffer->Release();
    ctx->IASetInputLayout(old.InputLayout); if (old.InputLayout) old.InputLayout->Release();
}

static void ImGui_ImplDX11_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Upload texture to graphics system
    {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        ID3D11Texture2D *pTexture = NULL;
        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = pixels;
        subResource.SysMemPitch = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

        // Create texture view
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
		if (g_pFontTextureView) { g_pFontTextureView->Release(); g_pFontTextureView = NULL; ImGui::GetIO().Fonts->TexID = NULL; }
        g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &g_pFontTextureView);
        pTexture->Release();
    }

    // Store our identifier
    io.Fonts->TexID = (ImTextureID)g_pFontTextureView;

    // Create texture sampler
    {
        D3D11_SAMPLER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.MipLODBias = 0.f;
        desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        desc.MinLOD = 0.f;
        desc.MaxLOD = 0.f;
		if (g_pFontSampler) { g_pFontSampler->Release(); g_pFontSampler = NULL; }
        g_pd3dDevice->CreateSamplerState(&desc, &g_pFontSampler);
    }
}

bool    ImGui_ImplDX11_CreateDeviceObjects()
{
    if (!g_pd3dDevice)
        return false;
    if (g_pFontSampler)
        ImGui_ImplDX11_InvalidateDeviceObjects();

    // By using D3DCompile() from <d3dcompiler.h> / d3dcompiler.lib, we introduce a dependency to a given version of d3dcompiler_XX.dll (see D3DCOMPILER_DLL_A)
    // If you would like to use this DX11 sample code but remove this dependency you can:
    //  1) compile once, save the compiled shader blobs into a file or source code and pass them to CreateVertexShader()/CreatePixelShader() [preferred solution]
    //  2) use code to detect any version of the DLL and grab a pointer to D3DCompile from the DLL.
    // See https://github.com/ocornut/imgui/pull/638 for sources and details.

    // Create the vertex shader
    {
        static const char* vertexShader =
            "cbuffer vertexBuffer : register(b0) \
            {\
            float4x4 ProjectionMatrix; \
            };\
            struct VS_INPUT\
            {\
            float2 pos : POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            \
            struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            \
            PS_INPUT main(VS_INPUT input)\
            {\
            PS_INPUT output;\
            output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
            output.col = input.col;\
            output.uv  = input.uv;\
            return output;\
            }";

        //D3DCompile(vertexShader, strlen(vertexShader), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &g_pVertexShaderBlob, NULL);
		D3DCompile(vertexShader, strlen(vertexShader), NULL, NULL, NULL, "main", "vs_5_0", 0, 0, &g_pVertexShaderBlob, NULL);
		if (g_pVertexShaderBlob == NULL) // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
            return false;
        if (g_pd3dDevice->CreateVertexShader((DWORD*)g_pVertexShaderBlob->GetBufferPointer(), g_pVertexShaderBlob->GetBufferSize(), NULL, &g_pVertexShader) != S_OK)
            return false;

        // Create the input layout
        D3D11_INPUT_ELEMENT_DESC local_layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        if (g_pd3dDevice->CreateInputLayout(local_layout, 3, g_pVertexShaderBlob->GetBufferPointer(), g_pVertexShaderBlob->GetBufferSize(), &g_pInputLayout) != S_OK)
            return false;

        // Create the constant buffer
        {
            D3D11_BUFFER_DESC desc;
            desc.ByteWidth = sizeof(VERTEX_CONSTANT_BUFFER);
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;
            g_pd3dDevice->CreateBuffer(&desc, NULL, &g_pVertexConstantBuffer);
        }
    }

    // Create the pixel shader
    {
        static const char* pixelShader =
            "struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            sampler sampler0;\
            Texture2D texture0;\
            \
            float4 main(PS_INPUT input) : SV_Target\
            {\
            float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
            return out_col; \
            }";
		//PE: This will normally be used to display large images as small icons, so texelsize is set higher and fixed (512).
		static const char* pixelShaderBlur =
			"struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            sampler sampler0;\
            Texture2D texture0;\
            \
            float4 main(PS_INPUT input) : SV_Target\
            {\
			float texelSize = 1.0f / 512.0f;\
            float4 out_col = texture0.Sample(sampler0, input.uv); \
            out_col += texture0.Sample(sampler0, input.uv + float2(0.0f,texelSize) ); \
            out_col += texture0.Sample(sampler0, input.uv + float2(texelSize,texelSize) ); \
            out_col += texture0.Sample(sampler0, input.uv + float2(texelSize,0.0f) ); \
            out_col += texture0.Sample(sampler0, input.uv + float2(0.0f,-texelSize) ); \
            out_col += texture0.Sample(sampler0, input.uv + float2(-texelSize,-texelSize) ); \
            out_col += texture0.Sample(sampler0, input.uv + float2(-texelSize,0.0f) ); \
            out_col += texture0.Sample(sampler0, input.uv + float2(texelSize,-texelSize) ); \
			out_col *= 0.125f;\
            out_col = input.col * out_col; \
            return out_col; \
            }";
		static const char* pixelShaderNoWhite =
			"struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            sampler sampler0;\
            Texture2D texture0;\
            \
            float4 main(PS_INPUT input) : SV_Target\
            {\
            float4 img_col = texture0.Sample(sampler0, input.uv); \
            float4 out_col = input.col * img_col; \
			if( img_col.r >= 0.9 && img_col.g >= 0.9 && img_col.b >= 0.9) { \
				out_col.a = 0.0; \
			} \
			return out_col; \
            }";

		static const char* pixelShaderBoost25 =
			"struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            sampler sampler0;\
            Texture2D texture0;\
            \
            float4 main(PS_INPUT input) : SV_Target\
            {\
            float4 img_col = texture0.Sample(sampler0, input.uv); \
			img_col.rgb = (img_col.rgb * 0.75) + 0.35; \
            float4 out_col = input.col * img_col; \
			return out_col; \
            }";

		static const char* pixelShaderNoAlpha =
			"struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            sampler sampler0;\
            Texture2D texture0;\
            \
            float4 main(PS_INPUT input) : SV_Target\
            {\
            float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
            out_col.w = 1.0; \
            return out_col; \
            }";

        //D3DCompile(pixelShader, strlen(pixelShader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &g_pPixelShaderBlob, NULL);
		D3DCompile(pixelShader, strlen(pixelShader), NULL, NULL, NULL, "main", "ps_5_0", 0, 0, &g_pPixelShaderBlob, NULL);
		if (g_pPixelShaderBlob == NULL)  // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
            return false;
        if (g_pd3dDevice->CreatePixelShader((DWORD*)g_pPixelShaderBlob->GetBufferPointer(), g_pPixelShaderBlob->GetBufferSize(), NULL, &g_pPixelShader) != S_OK)
            return false;

		//D3DCompile(pixelShaderNoWhite, strlen(pixelShaderNoWhite), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &g_pPixelShaderNoWhiteBlob, NULL);
		D3DCompile(pixelShaderNoWhite, strlen(pixelShaderNoWhite), NULL, NULL, NULL, "main", "ps_5_0", 0, 0, &g_pPixelShaderNoWhiteBlob, NULL);
		if (g_pPixelShaderNoWhiteBlob == NULL)
			return false;
		if (g_pd3dDevice->CreatePixelShader((DWORD*)g_pPixelShaderNoWhiteBlob->GetBufferPointer(), g_pPixelShaderNoWhiteBlob->GetBufferSize(), NULL, &g_pPixelShaderNoWhite) != S_OK)
			return false;

		//D3DCompile(pixelShaderNoAlpha, strlen(pixelShaderNoAlpha), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &g_pPixelShaderNoAlphaBlob, NULL);
		D3DCompile(pixelShaderNoAlpha, strlen(pixelShaderNoAlpha), NULL, NULL, NULL, "main", "ps_5_0", 0, 0, &g_pPixelShaderNoAlphaBlob, NULL);
		if (g_pPixelShaderNoAlphaBlob == NULL)
			return false;
		if (g_pd3dDevice->CreatePixelShader((DWORD*)g_pPixelShaderNoAlphaBlob->GetBufferPointer(), g_pPixelShaderNoAlphaBlob->GetBufferSize(), NULL, &g_pPixelShaderNoAlpha) != S_OK)
			return false;

		//D3DCompile(pixelShaderBoost25, strlen(pixelShaderBoost25), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &g_ppixelShaderBoost25Blob, NULL);
		D3DCompile(pixelShaderBoost25, strlen(pixelShaderBoost25), NULL, NULL, NULL, "main", "ps_5_0", 0, 0, &g_ppixelShaderBoost25Blob, NULL);
		if (g_ppixelShaderBoost25Blob == NULL)
			return false;
		if (g_pd3dDevice->CreatePixelShader((DWORD*)g_ppixelShaderBoost25Blob->GetBufferPointer(), g_ppixelShaderBoost25Blob->GetBufferSize(), NULL, &g_ppixelShaderBoost25) != S_OK)
			return false;

		//D3DCompile(pixelShaderBlur, strlen(pixelShaderBlur), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &g_pPixelShaderBlobBlur, NULL);
		D3DCompile(pixelShaderBlur, strlen(pixelShaderBlur), NULL, NULL, NULL, "main", "ps_5_0", 0, 0, &g_pPixelShaderBlobBlur, NULL);
		if (g_pPixelShaderBlobBlur == NULL)
			return false;
		if (g_pd3dDevice->CreatePixelShader((DWORD*)g_pPixelShaderBlobBlur->GetBufferPointer(), g_pPixelShaderBlobBlur->GetBufferSize(), NULL, &g_pPixelShaderBlur) != S_OK)
			return false;
		
    }

    // Create the blending setup
    {
        D3D11_BLEND_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.AlphaToCoverageEnable = false;
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        g_pd3dDevice->CreateBlendState(&desc, &g_pBlendState);
    }

    // Create the rasterizer state
    {
        D3D11_RASTERIZER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        desc.ScissorEnable = true;
        desc.DepthClipEnable = true;
        g_pd3dDevice->CreateRasterizerState(&desc, &g_pRasterizerState);
    }

    // Create depth-stencil State
    {
        D3D11_DEPTH_STENCIL_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.DepthEnable = false;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
        desc.StencilEnable = false;
        desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        desc.BackFace = desc.FrontFace;
        g_pd3dDevice->CreateDepthStencilState(&desc, &g_pDepthStencilState);
    }

    ImGui_ImplDX11_CreateFontsTexture();

    return true;
}

void    ImGui_ImplDX11_InvalidateDeviceObjects()
{
    if (!g_pd3dDevice)
        return;

    if (g_pFontSampler) { g_pFontSampler->Release(); g_pFontSampler = NULL; }
    if (g_pFontTextureView) { g_pFontTextureView->Release(); g_pFontTextureView = NULL; ImGui::GetIO().Fonts->TexID = NULL; } // We copied g_pFontTextureView to io.Fonts->TexID so let's clear that as well.
    if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
    if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }

    if (g_pBlendState) { g_pBlendState->Release(); g_pBlendState = NULL; }
    if (g_pDepthStencilState) { g_pDepthStencilState->Release(); g_pDepthStencilState = NULL; }
    if (g_pRasterizerState) { g_pRasterizerState->Release(); g_pRasterizerState = NULL; }
    if (g_pPixelShader) { g_pPixelShader->Release(); g_pPixelShader = NULL; }
    if (g_pPixelShaderBlob) { g_pPixelShaderBlob->Release(); g_pPixelShaderBlob = NULL; }

	if (g_pPixelShaderBlur) { g_pPixelShaderBlur->Release(); g_pPixelShaderBlur = NULL; }
	if (g_pPixelShaderBlobBlur) { g_pPixelShaderBlobBlur->Release(); g_pPixelShaderBlobBlur = NULL; }

    if (g_pVertexConstantBuffer) { g_pVertexConstantBuffer->Release(); g_pVertexConstantBuffer = NULL; }
    if (g_pInputLayout) { g_pInputLayout->Release(); g_pInputLayout = NULL; }
    if (g_pVertexShader) { g_pVertexShader->Release(); g_pVertexShader = NULL; }
    if (g_pVertexShaderBlob) { g_pVertexShaderBlob->Release(); g_pVertexShaderBlob = NULL; }
}

bool    ImGui_ImplDX11_Init(ID3D11Device* device, ID3D11DeviceContext* device_context)
{
    // Setup back-end capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendRendererName = "imgui_impl_dx11";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)

    // Get factory from device
    IDXGIDevice* pDXGIDevice = NULL;
    IDXGIAdapter* pDXGIAdapter = NULL;
    IDXGIFactory* pFactory = NULL;

    if (device->QueryInterface(IID_PPV_ARGS(&pDXGIDevice)) == S_OK)
        if (pDXGIDevice->GetParent(IID_PPV_ARGS(&pDXGIAdapter)) == S_OK)
            if (pDXGIAdapter->GetParent(IID_PPV_ARGS(&pFactory)) == S_OK)
            {
                g_pd3dDevice = device;
                g_pd3dDeviceContext = device_context;
                g_pFactory = pFactory;
            }
    if (pDXGIDevice) pDXGIDevice->Release();
    if (pDXGIAdapter) pDXGIAdapter->Release();
    g_pd3dDevice->AddRef();
    g_pd3dDeviceContext->AddRef();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        ImGui_ImplDX11_InitPlatformInterface();

    return true;
}

int ImGui_Get_Allow_Undocking()
{
	return pref.iAllowUndocking;
}
int ImGui_Get_Multi_Viewports_Disabled()
{
	return pref.iDisableObjectLibraryViewport;
}


void ImGui_ImplDX11_Shutdown()
{
    ImGui_ImplDX11_ShutdownPlatformInterface();
    ImGui_ImplDX11_InvalidateDeviceObjects();
    if (g_pFactory) { g_pFactory->Release(); g_pFactory = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
}

void ImGui_ImplDX11_NewFrame()
{
    if (!g_pFontSampler)
        ImGui_ImplDX11_CreateDeviceObjects();
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the back-end to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

struct ImGuiViewportDataDx11
{
    IDXGISwapChain*             SwapChain;
    ID3D11RenderTargetView*     RTView;

    ImGuiViewportDataDx11()     { SwapChain = NULL; RTView = NULL; }
    ~ImGuiViewportDataDx11()    { IM_ASSERT(SwapChain == NULL && RTView == NULL); }
};


static void ImGui_ImplDX11_CreateWindow(ImGuiViewport* viewport)
{
    ImGuiViewportDataDx11* data = IM_NEW(ImGuiViewportDataDx11)();
    viewport->RendererUserData = data;

    // PlatformHandleRaw should always be a HWND, whereas PlatformHandle might be a higher-level handle (e.g. GLFWWindow*, SDL_Window*).
    // Some back-end will leave PlatformHandleRaw NULL, in which case we assume PlatformHandle will contain the HWND.
    HWND hwnd = viewport->PlatformHandleRaw ? (HWND)viewport->PlatformHandleRaw : (HWND)viewport->PlatformHandle;
    IM_ASSERT(hwnd != 0);

    // Create swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferDesc.Width = (UINT)viewport->Size.x;
    sd.BufferDesc.Height = (UINT)viewport->Size.y;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = hwnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

	HRESULT hr;

    IM_ASSERT(data->SwapChain == NULL && data->RTView == NULL);

	#ifdef DEBUGDISPLAY
	if (!g_pFactory)
	{
		MessageBoxA(NULL, "g_pFactory==NULL", "Debug", 0);
	}
	#endif

    hr = g_pFactory->CreateSwapChain(g_pd3dDevice, &sd, &data->SwapChain);

	#ifdef DEBUGDISPLAY
	if (FAILED(hr))
	{
		if (hr == DXGI_STATUS_OCCLUDED)
		{
			MessageBoxA(NULL, "DXGI_STATUS_OCCLUDED", "Debug", 0);
		}
		if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
		{
			MessageBoxA(NULL, "DXGI_ERROR_NOT_CURRENTLY_AVAILABLE", "Debug", 0);
		}
		if (hr == E_OUTOFMEMORY)
		{
			MessageBoxA(NULL, "E_OUTOFMEMORY", "Debug", 0);
		}
		if (!data->SwapChain)
		{
			MessageBoxA(NULL, "data->SwapChain==NULL", "Debug", 0);
		}
	}
	#endif
    // Create the render target
    if (data->SwapChain)
    {
        ID3D11Texture2D* pBackBuffer;
        hr = data->SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		#ifdef DEBUGDISPLAY
		if (FAILED(hr))
		{
			char tmp[255];
			sprintf(tmp,"data->SwapChain->GetBuffer failed (%ld)", hr);
			MessageBoxA(NULL, tmp, "Debug", 0);
		}
		#endif

		hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &data->RTView);

		#ifdef DEBUGDISPLAY
		if (FAILED(hr))
		{
			char tmp[255];
			sprintf(tmp, "g_pd3dDevice->CreateRenderTargetView failed (%ld)", hr);
			MessageBoxA(NULL, tmp, "Debug", 0);
		}
		#endif
		pBackBuffer->Release();
    }
}

static void ImGui_ImplDX11_DestroyWindow(ImGuiViewport* viewport)
{
    // The main viewport (owned by the application) will always have RendererUserData == NULL since we didn't create the data for it.
    if (ImGuiViewportDataDx11* data = (ImGuiViewportDataDx11*)viewport->RendererUserData)
    {
        if (data->SwapChain)
            data->SwapChain->Release();
        data->SwapChain = NULL;
        if (data->RTView)
            data->RTView->Release();
        data->RTView = NULL;
        IM_DELETE(data);
    }
    viewport->RendererUserData = NULL;
}

static void ImGui_ImplDX11_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
    ImGuiViewportDataDx11* data = (ImGuiViewportDataDx11*)viewport->RendererUserData;
    if (data->RTView)
    {
        data->RTView->Release();
        data->RTView = NULL;
    }
	HRESULT hr;
    if (data->SwapChain)
    {
        ID3D11Texture2D* pBackBuffer = NULL;
        hr = data->SwapChain->ResizeBuffers(0, (UINT)size.x, (UINT)size.y, DXGI_FORMAT_UNKNOWN, 0);
		#ifdef DEBUGDISPLAY
		if (FAILED(hr))
		{
			char tmp[255];
			sprintf(tmp, "(SetWindowSize) data->SwapChain->ResizeBuffers failed (%ld)", hr);
			MessageBoxA(NULL, tmp, "Debug", 0);
		}
		#endif

		hr = data->SwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		#ifdef DEBUGDISPLAY
		if (FAILED(hr))
		{
			char tmp[255];
			sprintf(tmp, "(SetWindowSize) data->SwapChain->GetBuffer failed (%ld)", hr);
			MessageBoxA(NULL, tmp, "Debug", 0);
		}
		#endif

        if (pBackBuffer == NULL) { fprintf(stderr, "ImGui_ImplDX11_SetWindowSize() failed creating buffers.\n"); return; }
        hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &data->RTView);
		#ifdef DEBUGDISPLAY
		if (FAILED(hr))
		{
			char tmp[255];
			sprintf(tmp, "(SetWindowSize) g_pd3dDevice->CreateRenderTargetView failed (%ld)", hr);
			MessageBoxA(NULL, tmp, "Debug", 0);
		}
		#endif
		pBackBuffer->Release();
    }
}

static void ImGui_ImplDX11_RenderWindow(ImGuiViewport* viewport, void*)
{
    ImGuiViewportDataDx11* data = (ImGuiViewportDataDx11*)viewport->RendererUserData;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

	HRESULT hr;

	#ifdef DEBUGDISPLAY
	if(!g_pd3dDeviceContext) MessageBoxA(NULL, "(RenderWindow) g_pd3dDeviceContext==NULL", "Debug", 0);
	if(!data->RTView) MessageBoxA(NULL, "(RenderWindow) data->RTView==NULL", "Debug", 0);
	#endif

    g_pd3dDeviceContext->OMSetRenderTargets(1, &data->RTView, NULL);

    if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
        g_pd3dDeviceContext->ClearRenderTargetView(data->RTView, (float*)&clear_color);
    ImGui_ImplDX11_RenderDrawData(viewport->DrawData);
}


static void ImGui_ImplDX11_SwapBuffers(ImGuiViewport* viewport, void*)
{
    ImGuiViewportDataDx11* data = (ImGuiViewportDataDx11*)viewport->RendererUserData;
	if (data->SwapChain) //PE: need to check if still alive.
	{
		//PE: We need to update the directx include files in the repo.
		#ifndef DXGI_PRESENT_DO_NOT_WAIT
		#define DXGI_PRESENT_DO_NOT_WAIT               0x00000008UL
		#endif
		//PE: Sometimes Present never return.
		//PE: This did not help.
		//PE: This only happen in tab tab when you move a window to another monitor, so disable this for now.
		//PE: Until the reason is found. DXGI_PRESENT_DO_NOT_WAIT dont seam to fix it.
		//wiRenderer::GetDevice()->WaitForGPU(); //This did not help.
		//DXGI_SWAP_EFFECT_DISCARD 
		//PE: Think the problem is that we use a VERY old gxgi.h and we really should update this and start using a FLIP model.

		HRESULT hr = data->SwapChain->Present(0, DXGI_PRESENT_DO_NOT_WAIT); // Present without vsync

		if (FAILED(hr))
		{
			#ifdef DEBUGDISPLAY
			if (FAILED(hr))
			{
				//Graphix got this:
				//"-2005270518" Windows error 0x887A000A, -2005270518 (The GPU was busy at the moment when the call was made, and the call was neither executed nor scheduled.)
				//char tmp[255];
				//sprintf(tmp, "(SwapBuffers) data->SwapChain->Present failed (%ld)", hr);
				//MessageBoxA(NULL, tmp, "Debug", 0);
			}
			#endif

			if (hr == DXGI_ERROR_WAS_STILL_DRAWING)
			{
				//PE: Need to test this more.
				//hr = 0x887a000a : The GPU was busy at the moment when the call was made, and the call was neither executed nor scheduled. 
				//printf("tmp");
			}
		}
	}
}

static void ImGui_ImplDX11_InitPlatformInterface()
{
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Renderer_CreateWindow = ImGui_ImplDX11_CreateWindow;
    platform_io.Renderer_DestroyWindow = ImGui_ImplDX11_DestroyWindow;
    platform_io.Renderer_SetWindowSize = ImGui_ImplDX11_SetWindowSize;
    platform_io.Renderer_RenderWindow = ImGui_ImplDX11_RenderWindow;
    platform_io.Renderer_SwapBuffers = ImGui_ImplDX11_SwapBuffers;
}

static void ImGui_ImplDX11_ShutdownPlatformInterface()
{
    ImGui::DestroyPlatformWindows();
}

int ImGui_GetActiveViewPorts(void)
{
	ImGuiContext& g = *GImGui;
	if (GImGui != NULL)
	{
		if (g.Initialized)
		{
			ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
			return platform_io.Viewports.Size;
		}
	}
	return 0;
}

//#################################
//PE: Additional dialogs code here.
//#################################
#define GG_WINDOWS
#define AGK_WINDOWS

#include "globstruct.h"

extern GlobStruct*	g_pGlob;
#define g_agkhWnd g_pGlob->hWnd

#include "boxer.h"

//Noc File dialog cross platform.
//const char *noc_file_dialog_open(int flags,
//	const char *filters,
//	const char *default_path,
//	const char *default_name);

#include <stdlib.h>
#include <string.h>

static char *g_noc_file_dialog_ret = NULL;


#ifdef GG_WINDOWS

#include "windows.h"
#include "winuser.h"
#include <shlobj.h>
#include <conio.h>

static int __stdcall BrowseCallbackProcW(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
#ifdef UNICODE
		SendMessageW(hwnd, BFFM_SETSELECTIONW, TRUE, (LPARAM)pData);
#else
		SendMessageA(hwnd, BFFM_SETSELECTIONW, TRUE, (LPARAM)pData);
#endif
	}
	return 0;
}


// callback function
INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	if (uMsg == BFFM_INITIALIZED)
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, pData);
	return 0;
}

const char *noc_file_dialog_open(int flags,
	const char *filters,
	const char *default_path,
	const char *default_name,
	bool bUseDefaultPath,
	const char *pTitle)
{
	OPENFILENAMEA ofn;       // common dialog box structure
	char szFile[MAX_PATH];       // buffer for file name
	int ret;
	szFile[0] = '\0';

	if (flags & NOC_FILE_DIALOG_DIR) 
	{
		static wchar_t lBuff[MAX_PATH];
		//wchar_t aTitle[MAX_PATH];
		BROWSEINFOW bInfo;
		LPITEMIDLIST lpItem;
		HRESULT lHResult;

		CoUninitialize();
		lHResult = CoInitialize(NULL);

		ZeroMemory(&bInfo, sizeof(BROWSEINFO));

		bInfo.hwndOwner = g_agkhWnd;
		bInfo.lpszTitle = L"Select folder";
		bInfo.lpfn = BrowseCallbackProc;

		if (lHResult == S_OK || lHResult == S_FALSE)
		{
			bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE; //BIF_USENEWUI; //BIF_NEWDIALOGSTYLE

			if (bUseDefaultPath && default_path)
			{
				// ZJ: Added MultiByteToWideChar so cast to LPARAM is successful
				wchar_t szFolderPath[MAX_PATH];
				MultiByteToWideChar(CP_UTF8, 0, default_path, -1, szFolderPath, MAX_PATH);
				bInfo.lParam = (LPARAM)szFolderPath;

				//bInfo.lParam = (LPARAM)default_path;
			}
				
			else
				bInfo.lParam = (LPARAM)NULL;

			lpItem = SHBrowseForFolderW(&bInfo);
			if (lpItem)
			{
				SHGetPathFromIDListW(lpItem, lBuff);
			}

			if (lHResult == S_OK || lHResult == S_FALSE)
			{
				CoUninitialize();
				CoInitializeEx(NULL, COINIT_MULTITHREADED);

			}
		}
		sprintf(szFile, "%ws", lBuff);

		//Make sure ther blocking dialog did not skip some keys, reset.
		ImGuiIO& io = ImGui::GetIO();
		io.KeySuper = false;
		io.KeyCtrl = false;
		io.KeyAlt = false;
		io.KeyShift = false;

		io.KeysDown[13] = false; //also reset imgui keys.
		io.KeysDown[16] = false;
		io.KeysDown[17] = false;
		io.KeysDown[18] = false;
		io.KeysDown[19] = false;
		io.KeysDown[0x7B] = false; //F12
		io.KeysDown[78] = false;
		io.KeysDown[79] = false;
		io.KeysDown[83] = false;
		io.KeysDown[90] = false;
		io.KeysDown[73] = false; // I reset all system wide shortcuts used.

		io.KeysDown[69] = false; //New CTRL-Keys added. E-N-L-SPACE
		io.KeysDown[78] = false;
		io.KeysDown[76] = false;
		io.KeysDown[32] = false;

		io.MouseDown[0] = 0; //PE: Mouse (release) is also loast inside blocking dialogs. Reset!
		io.MouseDown[1] = 0;
		io.MouseDown[2] = 0;
		io.MouseDown[3] = 0;

		return strdup(szFile);
	}

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filters;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrTitle = pTitle;

	//PE: Why was default path removed ? added a flag instead bUseDefaultPath , need it :)
	if ((flags & NOC_FILE_DIALOG_OPEN || flags & NOC_FILE_DIALOG_SAVE) && default_path && bUseDefaultPath) {
		ofn.lpstrInitialDir = default_path;
	}
	else
		ofn.lpstrInitialDir = NULL;

	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.hwndOwner = g_agkhWnd;

	if (flags & NOC_FILE_DIALOG_DIR) 
	{
		ofn.Flags = OFN_CREATEPROMPT;
		ret = GetOpenFileNameA(&ofn);
	}
	else if (flags & NOC_FILE_DIALOG_OPEN) // || flags
		ret = GetOpenFileNameA(&ofn);
	else
		ret = GetSaveFileNameA(&ofn);

	//Make sure ther blocking dialog did not skip some keys, reset.
	ImGuiIO& io = ImGui::GetIO();
	io.KeySuper = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	io.KeyShift = false;

	io.KeysDown[13] = false; //also reset imgui keys.
	io.KeysDown[16] = false;
	io.KeysDown[17] = false;
	io.KeysDown[18] = false;
	io.KeysDown[19] = false;
	io.KeysDown[0x7B] = false; //F12
	io.KeysDown[78] = false;
	io.KeysDown[79] = false;
	io.KeysDown[83] = false;
	io.KeysDown[90] = false;
	io.KeysDown[73] = false;
	io.KeysDown[69] = false; //New CTRL-Keys added. E-N-L-SPACE
	io.KeysDown[78] = false;
	io.KeysDown[76] = false;
	io.KeysDown[32] = false;

	io.MouseDown[0] = 0;
	io.MouseDown[1] = 0;
	io.MouseDown[2] = 0;
	io.MouseDown[3] = 0;

	if (g_noc_file_dialog_ret != NULL)
		free(g_noc_file_dialog_ret);
	g_noc_file_dialog_ret = ret ? strdup(szFile) : NULL;

	return g_noc_file_dialog_ret;
}

#else
#ifdef AGK_MACOS

#include <AppKit/AppKit.h>

const char *noc_file_dialog_open(int flags,
	const char *filters,
	const char *default_path,
	const char *default_name)
{
	NSURL *url;
	const char *utf8_path;
	NSSavePanel *panel;
	NSOpenPanel *open_panel;
	NSMutableArray *types_array;
	NSURL *default_url;
	char buf[256], *patterns;

	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	if (flags & NOC_FILE_DIALOG_OPEN) {
		panel = open_panel = [NSOpenPanel openPanel];
	}
	else {
		panel = [NSSavePanel savePanel];
	}

	if (flags & NOC_FILE_DIALOG_DIR) {
		[open_panel setCanChooseDirectories : YES];
		[open_panel setCanChooseFiles : NO];
	}

	if (default_path) {
		default_url = [NSURL fileURLWithPath :
		[NSString stringWithUTF8String : default_path]];
		[panel setDirectoryURL : default_url];
		[panel setNameFieldStringValue : default_url.lastPathComponent];
	}

	if (filters) {
		types_array = [NSMutableArray array];
		while (*filters) {
			filters += strlen(filters) + 1; // skip the name
			strcpy(buf, filters);
			buf[strlen(buf) + 1] = '\0';
			for (patterns = buf; *patterns; patterns++)
				if (*patterns == ';') *patterns = '\0';
			patterns = buf;
			while (*patterns) {
				assert(strncmp(patterns, "*.", 2) == 0);
				patterns += 2; // Skip the "*."
				[types_array addObject : [NSString stringWithUTF8String : patterns]];
				patterns += strlen(patterns) + 1;
			}
			filters += strlen(filters) + 1;
		}
		[panel setAllowedFileTypes : types_array];
	}

	if (g_noc_file_dialog_ret != NULL)
		free(g_noc_file_dialog_ret);

	g_noc_file_dialog_ret = NULL;
	if ([panel runModal] == NSModalResponseOK) {
		url = [panel URL];
		utf8_path = [[url path] UTF8String];
		g_noc_file_dialog_ret = strdup(utf8_path);
	}

	[pool release];
	return g_noc_file_dialog_ret;
}

#else
//Linux.
#include <gtk/gtk.h>

static char selected_char[1024];


const char *noc_file_dialog_open(int flags,
	const char *filters,
	const char *default_path,
	const char *default_name)
{
	GtkWidget *dialog;
	GtkFileFilter *filter;
	GtkFileChooser *chooser;
	GtkFileChooserAction action;
	gint res;
	char buf[128], *patterns;

	if (flags & NOC_FILE_DIALOG_DIR)
	{
		strcpy(selected_char, "");
		FILE *f = popen("zenity --file-selection --directory", "r");
		fgets(selected_char, 1024, f);
		pclose(f);
		int length = strlen(selected_char);
		if (length < 2)
		{
			return NULL;
		}
		if (selected_char[length - 1] == '\n' || selected_char[length - 1] == '\r') selected_char[length - 1] = 0;

		return &selected_char[0];
	}

	if (flags & NOC_FILE_DIALOG_OPEN) {
		char cmd[1024], cmd1[1024];
		//
		if (default_path) {
			if (default_name)
				sprintf(cmd, "zenity --title \"Open File\" --file-selection --filename=\"%s/%s\"", default_path, default_name);
			else
				sprintf(cmd, "zenity --title \"Open File\" --file-selection --filename=\"%s\"", default_path);
		}
		else {
			strcpy(cmd, "zenity --title \"Open File\" --file-selection");
		}


		if (filters) {
			sprintf(cmd1, " --file-filter='(%s) | *.%s'", filters, filters);
			strcat(cmd, cmd1);
		}

		strcpy(selected_char, "");
		FILE *f = popen(cmd, "r");
		fgets(selected_char, 1024, f);
		pclose(f);
		int length = strlen(selected_char);
		if (length < 2)
		{
			return NULL;
		}
		if (selected_char[length - 1] == '\n' || selected_char[length - 1] == '\r') selected_char[length - 1] = 0;

		return &selected_char[0];
	}


	if (flags & NOC_FILE_DIALOG_SAVE) {
		char cmd[1024], cmd1[1024];
		//
		if (default_path) {
			if (default_name)
				sprintf(cmd, "zenity --save --title \"Save File\" --file-selection --filename=\"%s/%s\"", default_path, default_name);
			else {
				if (default_path[strlen(default_path) - 1] == '/')
					sprintf(cmd, "zenity --save --title \"Save File\" --file-selection --filename=\"%s\"", default_path);
				else
					sprintf(cmd, "zenity --save --title \"Save File\" --file-selection --filename=\"%s/\"", default_path);
			}
		}
		else {
			strcpy(cmd, "zenity --save --title \"Save File\" --file-selection");
		}


		if (filters) {
			sprintf(cmd1, " --file-filter='(%s) | *.%s'", filters, filters);
			strcat(cmd, cmd1);
		}

		strcpy(selected_char, "");
		FILE *f = popen(cmd, "r");
		fgets(selected_char, 1024, f);
		pclose(f);
		int length = strlen(selected_char);
		if (length < 2)
		{
			return NULL;
		}
		if (selected_char[length - 1] == '\n' || selected_char[length - 1] == '\r') selected_char[length - 1] = 0;

		return &selected_char[0];
	}

	//zenity --file-selection --file-filter='PDF files (pdf) | *.pdf'
#ifdef USEGTKDIRECTLY
	action = flags & NOC_FILE_DIALOG_SAVE ? GTK_FILE_CHOOSER_ACTION_SAVE :
		GTK_FILE_CHOOSER_ACTION_OPEN;
	if (flags & NOC_FILE_DIALOG_DIR)
		action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;

	gtk_init_check(NULL, NULL);
	dialog = gtk_file_chooser_dialog_new(
		flags & NOC_FILE_DIALOG_SAVE ? "Save File" : "Open File",
		NULL,
		action,
		"_Cancel", GTK_RESPONSE_CANCEL,
		flags & NOC_FILE_DIALOG_SAVE ? "_Save" : "_Open", GTK_RESPONSE_ACCEPT,
		NULL);
	chooser = GTK_FILE_CHOOSER(dialog);
	if (flags & NOC_FILE_DIALOG_OVERWRITE_CONFIRMATION)
		gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

	if (default_path)
		gtk_file_chooser_set_filename(chooser, default_path);
	if (default_name)
		gtk_file_chooser_set_current_name(chooser, default_name);

	while (filters && *filters) {
		filter = gtk_file_filter_new();
		gtk_file_filter_set_name(filter, filters);
		filters += strlen(filters) + 1;

		// Split the filter pattern with ';'.
		strcpy(buf, filters);
		buf[strlen(buf)] = '\0';
		for (patterns = buf; *patterns; patterns++)
			if (*patterns == ';') *patterns = '\0';
		patterns = buf;
		while (*patterns) {
			gtk_file_filter_add_pattern(filter, patterns);
			patterns += strlen(patterns) + 1;
		}

		gtk_file_chooser_add_filter(chooser, filter);
		filters += strlen(filters) + 1;
	}

	res = gtk_dialog_run(GTK_DIALOG(dialog));

	free(g_noc_file_dialog_ret);
	g_noc_file_dialog_ret = NULL;

	if (res == GTK_RESPONSE_ACCEPT)
		g_noc_file_dialog_ret = gtk_file_chooser_get_filename(chooser);
	gtk_widget_destroy(dialog);
	while (gtk_events_pending()) gtk_main_iteration();
	return g_noc_file_dialog_ret;
#endif

}

#endif
#endif


bool CancelQuit()
{
	boxer::Selection selection;
	selection = boxer::show("Are you sure you want to quit ?", " Warning!", boxer::Style::Question, boxer::Buttons::OKCancel);

	//Make sure ther blocking dialog did not skip some keys, reset.
	ImGuiIO& io = ImGui::GetIO();
	io.KeySuper = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	io.KeyShift = false;

	io.KeysDown[13] = false;
	io.KeysDown[17] = false;
	io.KeysDown[18] = false;
	io.KeysDown[19] = false;
	io.KeysDown[0x7B] = false; //F12
	io.KeysDown[78] = false;
	io.KeysDown[79] = false;
	io.KeysDown[83] = false;
	io.KeysDown[90] = false;
	io.KeysDown[73] = false;
	io.KeysDown[69] = false; //New CTRL-Keys added. E-N-L-SPACE
	io.KeysDown[78] = false;
	io.KeysDown[76] = false;
	io.KeysDown[32] = false;


	io.MouseDown[0] = 0;
	io.MouseDown[1] = 0;
	io.MouseDown[2] = 0;
	io.MouseDown[3] = 0;

	if (selection == boxer::Selection::Cancel) {
		return true;
	}

	return false;
}

bool overWriteFileBox(char * file)
{
	boxer::Selection selection;
	selection = boxer::show(" File exists, do you want to overwrite file?", " Warning!", boxer::Style::Question, boxer::Buttons::YesNo);

	//Make sure ther blocking dialog did not skip some keys, reset.
	ImGuiIO& io = ImGui::GetIO();
	io.KeySuper = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	io.KeyShift = false;
	io.KeysDown[13] = false;
	io.KeysDown[16] = false;
	io.KeysDown[17] = false;
	io.KeysDown[18] = false;
	io.KeysDown[19] = false;
	io.MouseDown[0] = 0;
	io.MouseDown[1] = 0;
	io.MouseDown[2] = 0;
	io.MouseDown[3] = 0;
	io.KeysDown[0x7B] = false; //F12
	io.KeysDown[78] = false;
	io.KeysDown[79] = false;
	io.KeysDown[83] = false;
	io.KeysDown[90] = false;
	io.KeysDown[73] = false;
	io.KeysDown[69] = false; //New CTRL-Keys added. E-N-L-SPACE
	io.KeysDown[78] = false;
	io.KeysDown[76] = false;
	io.KeysDown[32] = false;

	if (selection == boxer::Selection::Yes) return(true);

	return(false);
}


int askBoxCancel(const char * ask, const char *title)
{
	boxer::Selection selection;
	selection = boxer::show(ask, title, boxer::Style::Question, boxer::Buttons::YesNoCancel);

	//Make sure ther blocking dialog did not skip some keys, reset.
	ImGuiIO& io = ImGui::GetIO();
	io.KeySuper = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	io.KeyShift = false;

	io.KeysDown[13] = false; //also reset imgui keys.
	io.KeysDown[16] = false;
	io.KeysDown[17] = false;
	io.KeysDown[18] = false;
	io.KeysDown[19] = false;
	io.KeysDown[0x7B] = false; //F12
	io.KeysDown[78] = false;
	io.KeysDown[79] = false;
	io.KeysDown[83] = false;
	io.KeysDown[90] = false;
	io.KeysDown[73] = false;
	io.KeysDown[69] = false; //New CTRL-Keys added. E-N-L-SPACE
	io.KeysDown[78] = false;
	io.KeysDown[76] = false;
	io.KeysDown[32] = false;

	io.MouseDown[0] = 0;
	io.MouseDown[1] = 0;
	io.MouseDown[2] = 0;
	io.MouseDown[3] = 0;

	if (selection == boxer::Selection::Yes) return(1);
	if (selection == boxer::Selection::Cancel) return(2);
	return(0);
}

bool askBox(char * ask, char *title)
{
	boxer::Selection selection;
	selection = boxer::show(ask, title, boxer::Style::Question, boxer::Buttons::YesNo);

	//Make sure ther blocking dialog did not skip some keys, reset.
	ImGuiIO& io = ImGui::GetIO();
	io.KeySuper = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	io.KeyShift = false;

	io.KeysDown[13] = false; //also reset imgui keys.
	io.KeysDown[16] = false;
	io.KeysDown[17] = false;
	io.KeysDown[18] = false;
	io.KeysDown[19] = false;
	io.KeysDown[0x7B] = false; //F12
	io.KeysDown[78] = false;
	io.KeysDown[79] = false;
	io.KeysDown[83] = false;
	io.KeysDown[90] = false;
	io.KeysDown[73] = false;
	io.KeysDown[69] = false; //New CTRL-Keys added. E-N-L-SPACE
	io.KeysDown[78] = false;
	io.KeysDown[76] = false;
	io.KeysDown[32] = false;

	io.MouseDown[0] = 0;
	io.MouseDown[1] = 0;
	io.MouseDown[2] = 0;
	io.MouseDown[3] = 0;

	if (selection == boxer::Selection::Yes) return(true);

	return(false);
}

bool changedFileBox(char * file)
{
	boxer::Selection selection;
	char msg[1024];

	sprintf(msg, "The file has unsaved changes. Do you want to save it?\n%s", file);
	selection = boxer::show(msg, " Warning!", boxer::Style::Question, boxer::Buttons::YesNo);

	//Make sure ther blocking dialog did not skip some keys, reset.
	ImGuiIO& io = ImGui::GetIO();
	io.KeySuper = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	io.KeyShift = false;

	io.KeysDown[13] = false;
	io.KeysDown[16] = false;
	io.KeysDown[17] = false;
	io.KeysDown[18] = false;
	io.KeysDown[19] = false;
	io.KeysDown[0x7B] = false; //F12
	io.KeysDown[78] = false;
	io.KeysDown[79] = false;
	io.KeysDown[83] = false;
	io.KeysDown[90] = false;
	io.KeysDown[73] = false;
	io.KeysDown[69] = false; //New CTRL-Keys added. E-N-L-SPACE
	io.KeysDown[78] = false;
	io.KeysDown[76] = false;
	io.KeysDown[32] = false;

	io.MouseDown[0] = 0;
	io.MouseDown[1] = 0;
	io.MouseDown[2] = 0;
	io.MouseDown[3] = 0;

	if (selection == boxer::Selection::Yes) return(true);

	return(false);
}

void BoxerInfo(char * text, const char *heading)
{
	boxer::show(text, heading);
	//Make sure ther blocking dialog did not skip some keys, reset.
	ImGuiIO& io = ImGui::GetIO();
	io.KeySuper = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	io.KeyShift = false;

	io.KeysDown[13] = false;
	io.KeysDown[16] = false;
	io.KeysDown[17] = false;
	io.KeysDown[18] = false;
	io.KeysDown[19] = false;
	//PE: Need to reset any system wide hotkey that execute a blocking dialog.
	io.KeysDown[0x7B] = false; //F12
	io.KeysDown[78] = false;
	io.KeysDown[79] = false;
	io.KeysDown[83] = false;
	io.KeysDown[90] = false;
	io.KeysDown[73] = false;
	io.KeysDown[69] = false; //New CTRL-Keys added. E-N-L-SPACE
	io.KeysDown[78] = false;
	io.KeysDown[76] = false;
	io.KeysDown[32] = false;

	io.MouseDown[0] = 0;
	io.MouseDown[1] = 0;
	io.MouseDown[2] = 0;
	io.MouseDown[3] = 0;


}

void DebugInfo(char * text, const char *heading)
{
#ifndef DEVVERSION
	return;
#else
	boxer::show(text, heading);
	//Make sure ther blocking dialog did not skip some keys, reset.
	ImGuiIO& io = ImGui::GetIO();
	io.KeySuper = false;
	io.KeyCtrl = false;
	io.KeyAlt = false;
	io.KeyShift = false;
#endif
}



//################################################################
//#### PE: ImgBtn                                             ####
//#### Used to add image buttons directly from a GG image id. ####
//################################################################

#include "CImageC.h"

namespace ImGui {

	bool MenuItem2(const char* label, const char* shortcut, bool selected, bool enabled)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		ImGuiStyle& style = g.Style;
		ImVec2 pos = window->DC.CursorPos;
		ImVec2 label_size = CalcTextSize(label, NULL, true);

		// We've been using the equivalent of ImGuiSelectableFlags_SetNavIdOnHover on all Selectable() since early Nav system days (commit 43ee5d73),
		// but I am unsure whether this should be kept at all. For now moved it to be an opt-in feature used by menus only.
		ImGuiSelectableFlags flags = ImGuiSelectableFlags_PressedOnRelease | ImGuiSelectableFlags_SetNavIdOnHover | (enabled ? 0 : ImGuiSelectableFlags_Disabled);
		bool pressed;
		if (window->DC.LayoutType == ImGuiLayoutType_Horizontal)
		{
			// Mimic the exact layout spacing of BeginMenu() to allow MenuItem() inside a menu bar, which is a little misleading but may be useful
			// Note that in this situation we render neither the shortcut neither the selected tick mark
			float w = label_size.x;
			window->DC.CursorPos.x += (float)(int)(style.ItemSpacing.x * 0.5f);
			PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x * 2.0f, style.ItemSpacing.y));
			pressed = Selectable(label, selected, flags, ImVec2(w, 0.0f));
			PopStyleVar();
			window->DC.CursorPos.x += (float)(int)(style.ItemSpacing.x * (-1.0f + 0.5f)); // -1 spacing to compensate the spacing added when Selectable() did a SameLine(). It would also work to call SameLine() ourselves after the PopStyleVar().
		}
		else
		{
			ImVec2 shortcut_size = shortcut ? CalcTextSize(shortcut, NULL) : ImVec2(0.0f, 0.0f);
			float w = window->MenuColumns.DeclColumns(label_size.x, shortcut_size.x, (float)(int)(g.FontSize * 1.20f)); // Feedback for next frame
			float extra_w = ImMax(0.0f, GetContentRegionAvail().x - w);
			pressed = Selectable(label, selected, flags | ImGuiSelectableFlags_DrawFillAvailWidth, ImVec2(w, 0.0f));
			if (shortcut_size.x > 0.0f)
			{
				PushStyleColor(ImGuiCol_Text, g.Style.Colors[ImGuiCol_TextDisabled]);
				RenderText(pos + ImVec2(window->MenuColumns.Pos[1] + extra_w, 0.0f), shortcut, NULL, false);
				PopStyleColor();
			}
			if (selected)
				RenderCheckMark(pos + ImVec2(window->MenuColumns.Pos[2] + extra_w + g.FontSize * 0.40f, g.FontSize * 0.134f * 0.5f), GetColorU32(enabled ? ImGuiCol_Text : ImGuiCol_TextDisabled), g.FontSize * 0.866f);
		}

		IMGUI_TEST_ENGINE_ITEM_INFO(window->DC.LastItemId, label, window->DC.ItemFlags | ImGuiItemStatusFlags_Checkable | (selected ? ImGuiItemStatusFlags_Checked : 0));
		return pressed;
	}

	const char* CalcWordWrapPositionB(float scale, const char* textorig, const char* text_end, float wrap_width, float line_start)
	{
		// Simple word-wrapping for English, not full-featured. Please submit failing cases!
		// FIXME: Much possible improvements (don't cut things like "word !", "word!!!" but cut within "word,,,,", more sensible support for punctuations, support for Unicode punctuations, etc.)

		// For references, possible wrap point marked with ^
		//  "aaa bbb, ccc,ddd. eee   fff. ggg!"
		//      ^    ^    ^   ^   ^__    ^    ^

		// List of hardcoded separators: .,;!?'"

		// Skip extra blanks after a line returns (that includes not counting them in width computation)
		// e.g. "Hello    world" --> "Hello" "World"

		// Cut words that cannot possibly fit within one line.
		// e.g.: "The tropical fish" with ~5 characters worth of width --> "The tr" "opical" "fish"

		float line_width = line_start / scale;
		float word_width = 0.0f;
		float blank_width = 0.0f;
		wrap_width /= scale; // We work with unscaled widths to avoid scaling every characters

		// can mess up with text starts with spaces!
		const char* text = textorig;
		while ((*text == ' ' || *text == '/t') && text<text_end) text++;

		const char* word_end = text;
		//const char* prev_word_end = NULL;
		const char* prev_word_end = line_start > 0.0f ? word_end : NULL;
		bool inside_word = true;

		const char* s = text;
		while (s < text_end)
		{
			unsigned int c = (unsigned int)*s;
			const char* next_s;
			if (c < 0x80)
				next_s = s + 1;
			else
				next_s = s + ImTextCharFromUtf8(&c, s, text_end);
			if (c == 0)
				break;

			if (c < 32)
			{
				if (c == '\n')
				{
					line_width = word_width = blank_width = 0.0f;
					inside_word = true;
					s = next_s;
					continue;
				}
				if (c == '\r')
				{
					s = next_s;
					continue;
				}
			}

			const float char_width = ImGui::GetFont()->GetCharAdvance(c); //((int)c < IndexAdvanceX.Size ? IndexAdvanceX.Data[c] : FallbackAdvanceX);
			if (ImCharIsBlankW(c))
			{
				if (inside_word)
				{
					line_width += blank_width;
					blank_width = 0.0f;
					word_end = s;
				}
				blank_width += char_width;
				inside_word = false;
			}
			else
			{
				word_width += char_width;
				if (inside_word)
				{
					word_end = next_s;
				}
				else
				{
					prev_word_end = word_end;
					line_width += word_width + blank_width;
					word_width = blank_width = 0.0f;
				}

				// Allow wrapping after punctuation.
				inside_word = !(c == '.' || c == ',' || c == ';' || c == '!' || c == '?' || c == '\"');
			}

			// We ignore blank width at the end of the line (they can be skipped)
			if (line_width + word_width > wrap_width)
			{
				// Words that cannot possibly fit within an entire line will be cut anywhere.
				if (word_width < wrap_width)
					s = prev_word_end ? prev_word_end : word_end;
				break;
			}

			s = next_s;
		}

		return s;
	}


	void TextCenter(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;
		const char* text_end = g.TempBuffer + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);

		
		float textwidth = ImGui::CalcTextSize(g.TempBuffer).x;
		//PE: Change - Account for indent.
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2( (GetContentRegionAvail().x*0.5) - (textwidth*0.5) - (window->DC.Indent.x*0.5) , 0.0f));

		TextEx(g.TempBuffer, text_end, ImGuiTextFlags_NoWidthForLargeClippedText);
		va_end(args);
	}

	bool StyleCollapsingHeader(const char* label, ImGuiTreeNodeFlags flags)
	{
		if (pref.current_style == 25)
		{
			//ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, ImVec2(0, 0));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.43f, 0.57f, 1.00f));
			auto *style = &ImGui::GetStyle();
			style->FrameBorderSize = 0.0f;
		}

		bool bret = CollapsingHeader(label, flags);
		
		if (pref.current_style == 25)
		{
			ImGui::PopStyleColor();
			auto *style = &ImGui::GetStyle();
			style->FrameBorderSize = 1.0f;
			//ImGui::PopStyleVar();
		}

		return bret;
	}

	bool HyberlinkButton(const char* label, const ImVec2& size_arg)
	{
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.43f, 0.57f, 0.00f)); //no border.
		auto *style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;
		
		ImVec4 backImGuiCol_Button = colors[ImGuiCol_Button];
		ImVec4 backImGuiCol_ButtonHovered = colors[ImGuiCol_ButtonHovered];
		ImVec4 backImGuiCol_ButtonActive = colors[ImGuiCol_ButtonActive];

		colors[ImGuiCol_Button] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

		style->FrameBorderSize = 0.0f;

		ImVec2 cpos = ImGui::GetCursorPos();

		bool bret = ImGui::Button(label, size_arg);
		
		ImGuiWindow* window = GetCurrentWindow();

		//window->DC.CursorPosPrevLine.x
		float yoffset = 8.0;
		float xpadding = 4.0;
		ImRect image_bb(ImVec2(window->Pos.x + window->Scroll.x + cpos.x + xpadding, window->DC.CursorPos.y - yoffset), ImVec2(window->DC.CursorPosPrevLine.x- xpadding, (window->DC.CursorPos.y - yoffset) + 2.0f));

		ImVec4 col = backImGuiCol_Button;
		if (IsItemHovered())
		{
			col = backImGuiCol_ButtonHovered;
		}
		window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(col));

		ImGui::PopStyleColor();
		style->FrameBorderSize = 1.0f;

		colors[ImGuiCol_Button] = backImGuiCol_Button;
		colors[ImGuiCol_ButtonHovered] = backImGuiCol_ButtonHovered;
		colors[ImGuiCol_ButtonActive] = backImGuiCol_ButtonActive;

		return bret;
	}

	bool MinMaxButtonEx(const char* str_id, ImGuiDir dir)
	{
		float sz = GetFrameHeight();
		ImVec2 size = ImVec2(sz, sz);

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiID id = window->GetID(str_id);
		const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
		const float default_size = GetFrameHeight();
		ItemSize(size, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);
		if (!ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held, 0);

		// Render
		const ImU32 bg_col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		const ImU32 text_col = GetColorU32(ImGuiCol_Text);
		RenderNavHighlight(bb, id);
		RenderFrame(bb.Min, bb.Max, bg_col, true, g.Style.FrameRounding);
		RenderArrowOutLine(window->DrawList, bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), text_col, dir , 1.0f);

		return pressed;
	}

	void AddTriangleNotClosed(ImDrawList* draw_list,const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness)
	{
		if ((col & IM_COL32_A_MASK) == 0)
			return;

		draw_list->PathLineTo(p2);
		draw_list->PathLineTo(p1);
		draw_list->PathLineTo(p3);
		draw_list->PathStroke(col, false, thickness);
	}

	void RenderArrowOutLine(ImDrawList* draw_list, ImVec2 pos, ImU32 col, ImGuiDir dir, float scale)
	{
		const float h = draw_list->_Data->FontSize * 1.00f;
		float r = h * 0.40f * scale;
		ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f * scale);

		ImVec2 a, b, c;
		switch (dir)
		{
		case ImGuiDir_Up:
		case ImGuiDir_Down:
			if (dir == ImGuiDir_Up) r = -r;
			a = ImVec2(+0.000f, +0.750f) * r;
			b = ImVec2(-0.866f, -0.750f) * r;
			c = ImVec2(+0.866f, -0.750f) * r;
			break;
		case ImGuiDir_Left:
		case ImGuiDir_Right:
			if (dir == ImGuiDir_Left) r = -r;
			a = ImVec2(+0.750f, +0.000f) * r;
			b = ImVec2(-0.750f, +0.866f) * r;
			c = ImVec2(-0.750f, -0.866f) * r;
			break;
		case ImGuiDir_None:
		case ImGuiDir_COUNT:
			IM_ASSERT(0);
			break;
		}
		AddTriangleNotClosed(draw_list,center + a, center + b, center + c, col,1.75f);
	}

	bool StyleButtonEx(const char* label, const ImVec2& size_arg, bool bDisabled)
	{
		ImGuiButtonFlags_ flags = ImGuiButtonFlags_None;
		if (bDisabled == true) flags = ImGuiButtonFlags_Disabled;
		if (pref.current_style == 25)
		{
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.43f, 0.57f, 1.00f));
			auto *style = &ImGui::GetStyle();
			style->FrameBorderSize = 0.0f;
		}
		if (bDisabled == true) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.22f, 0.22f, 0.22f, 0.5f));
		bool bret = ImGui::ButtonEx(label, size_arg, flags);
		if (bDisabled == true) ImGui::PopStyleColor();
		if (pref.current_style == 25)
		{
			ImGui::PopStyleColor();
			auto *style = &ImGui::GetStyle();
			style->FrameBorderSize = 1.0f;
		}
		return bret;
	}

	bool StyleButton(const char* label, const ImVec2& size_arg)
	{
		if (pref.current_style == 25)
		{
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.43f, 0.57f, 1.00f));
			auto *style = &ImGui::GetStyle();
			style->FrameBorderSize = 0.0f;
		}
		bool bret = ImGui::Button(label, size_arg);
		if (pref.current_style == 25)
		{
			ImGui::PopStyleColor();
			auto *style = &ImGui::GetStyle();
			style->FrameBorderSize = 1.0f;
			//ImGui::PopStyleVar();
		}
		return bret;
	}

	bool StyleButtonDark(const char* label, const ImVec2& size_arg)
	{
		auto* style = &ImGui::GetStyle();
		style->FrameBorderSize = 0.0f;
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12f, 0.33f, 0.5f, 1.0f));
		bool bret = ImGui::Button(label, size_arg);
		ImGui::PopStyleColor();
		style->FrameBorderSize = 1.0f;
	
		return bret;
	}

	//PE: Max gadget types.
	bool bLastSliderHovered = false;
	bool IsLastSliderHovered(void)
	{
		return(bLastSliderHovered);
	}

	bool MaxSliderInputInt(const char* label, int* v, int v_min, int v_max, const char* tooltip, int boxWidth)
	{
		bLastSliderHovered = false;
		if (!label) return(false);
		char cUniqueLabel[256];
		strncpy(cUniqueLabel, label, 240);
		cUniqueLabel[240] = 0;

		int iCounter = g_SliderData.size() - 1;
		// Check to see if this slider has a non-default max value.
		for (int i = 0; i < g_SliderData.size(); i++)
		{
			if (v_max == g_SliderData[i].fDefaultMaxValue)
			{
				if (strcmp(cUniqueLabel, g_SliderData[i].cID) == 0)
				{
					// This slider has a non-default max value, so use it instead of the value passed in.
					if (*v > g_SliderData[i].fHighestMaxValue)
						g_SliderData[i].fHighestMaxValue = *v;

					v_max = g_SliderData[i].fHighestMaxValue;

					break;
				}
			}
			
			iCounter--;
		}

		if (iCounter <= 0)
		{
			// This slider does not have a custom max value set yet.
			if (*v > v_max)
			{
				SliderData data;
				data.fDefaultMaxValue = v_max;
				data.fHighestMaxValue = *v;
				strcpy(data.cID, cUniqueLabel);
				g_SliderData.push_back(data);

				v_max = *v;
			}
		}

		int iInput = *v;
		bool bRet = false;
		ImGui::PushItemWidth(-10 - boxWidth - 10);

		if (ImGui::SliderInt(cUniqueLabel, &iInput, v_min, v_max, " "))
		{
			bRet = true;
		}
		if (tooltip && ImGui::IsItemHovered()) {
			bLastSliderHovered = true;
			ImGui::SetTooltip(tooltip);
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PushItemWidth(boxWidth);
		
		if (strstr(cUniqueLabel, "##"))
			strcat(cUniqueLabel, "i");
		else
			strcat(cUniqueLabel, "##i");

		if (ImGui::InputInt(cUniqueLabel, &iInput, 0, 0))
		{

			bRet = true;
		}
		if (!pref.iTurnOffEditboxTooltip && tooltip && ImGui::IsItemHovered()) {
			bLastSliderHovered = true;
			ImGui::SetTooltip(tooltip);
		}
		ImGui::PopItemWidth();

		if (iInput < v_min)
			iInput = v_min;

		*v = iInput;
		return(bRet);
	}

	bool MaxSliderInputFloat(const char* label, float* v, float v_min, float v_max, const char* tooltip, int startval, float maxval, int numericboxwidth)
	{
		bLastSliderHovered = false;
		if (!label) return(false);
		char cUniqueLabel[256];
		strncpy(cUniqueLabel, label, 240);
		cUniqueLabel[240] = 0;

		int iCounter = g_SliderData.size() - 1;
		// Check to see if this slider has a non-default max value.
		for (int i = 0; i < g_SliderData.size(); i++)
		{
			// Compare the default values first, to save doing lots of string compares.
			if (v_max == g_SliderData[i].fDefaultMaxValue)  
			{
				if (strcmp(label, g_SliderData[i].cID) == 0)
				{
					// This slider has a non-default max value, so use it instead of the value passed in.
					if (*v > g_SliderData[i].fHighestMaxValue)
						g_SliderData[i].fHighestMaxValue = *v;

					v_max = g_SliderData[i].fHighestMaxValue;
					maxval = v_max;

					break;
				}
			}

			iCounter--;
		}

		if (iCounter <= 0)
		{
			// This slider does not have a custom max value set yet.
			if (*v > v_max)
			{
				SliderData data;
				data.fDefaultMaxValue = v_max;
				data.fHighestMaxValue = *v;
				strcpy(data.cID, label);
				g_SliderData.push_back(data);

				v_max = *v;
				maxval = v_max;
			}
		}

		float fInput = *v - v_min;
		
		float fRange = v_max - v_min;
		bool bRet = false;
		int iControlInput, iMaxInt = 100;
		ImGui::PushItemWidth(-10 - 40);
		if(v_max <= 10.0)
			iControlInput = maxval / fRange * (fInput + 0.009f); // Added +0.009 to eliminate floating point error which resulted in iControlInput dropping 1
		else
			iControlInput = maxval / fRange * ceil(fInput);

		if (iControlInput < startval)
			iControlInput = startval;

		iMaxInt = maxval;

		if (ImGui::SliderInt (cUniqueLabel, &iControlInput, startval, iMaxInt, " "))
		{
			fInput = (float)iControlInput * (fRange / maxval);
			bRet = true;
		}
		if (tooltip && ImGui::IsItemHovered())
		{
			bLastSliderHovered = true;
			ImGui::SetTooltip(tooltip);
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushItemWidth(numericboxwidth);
		strcat(cUniqueLabel, "##i");

		if (ImGui::InputInt(cUniqueLabel, &iControlInput, 0, 0))
		{
			if (v_max <= 10.0)
				fInput = (float)iControlInput * (fRange / maxval);
			else
				fInput = ceil((float)iControlInput * (fRange / maxval));
			bRet = true;
		}
		if (!pref.iTurnOffEditboxTooltip && tooltip && ImGui::IsItemHovered())
		{
			bLastSliderHovered = true;
			ImGui::SetTooltip(tooltip);
		}
		ImGui::PopItemWidth();

		*v = v_min + fInput;
		return(bRet);
	}

	bool MaxSliderInputFloat2(const char* label, float* v, float v_min, float v_max, const char* tooltip, int startval, float maxval, int numericboxwidth)
	{
		bLastSliderHovered = false;
		if (!label) return(false);
		char cUniqueLabel[256];
		strncpy(cUniqueLabel, label, 240);
		cUniqueLabel[240] = 0;

		int iCounter = g_SliderData.size() - 1;
		// Check to see if this slider has a non-default max value.
		for (int i = 0; i < g_SliderData.size(); i++)
		{
			// Compare the default values first, to save doing lots of string compares.
			if (v_max == g_SliderData[i].fDefaultMaxValue)
			{
				if (strcmp(label, g_SliderData[i].cID) == 0)
				{
					// This slider has a non-default max value, so use it instead of the value passed in.
					if (*v > g_SliderData[i].fHighestMaxValue)
						g_SliderData[i].fHighestMaxValue = *v;

					v_max = g_SliderData[i].fHighestMaxValue;
					maxval = v_max;

					break;
				}
			}

			iCounter--;
		}

		if (iCounter <= 0)
		{
			// This slider does not have a custom max value set yet.
			if (*v > v_max)
			{
				SliderData data;
				data.fDefaultMaxValue = v_max;
				data.fHighestMaxValue = *v;
				strcpy(data.cID, label);
				g_SliderData.push_back(data);

				v_max = *v;
				maxval = v_max;
			}
		}

		float fInput = *v - v_min;
		float fRange = v_max - v_min;
		bool bRet = false;
		float fControlInput, iMaxInt = 100;
		ImGui::PushItemWidth(-10 - 10 - numericboxwidth);
		fControlInput = maxval / fRange * fInput;
		if (fControlInput < startval)
			fControlInput = startval;

		iMaxInt = maxval;

		if (ImGui::SliderFloat(cUniqueLabel, &fControlInput, startval, iMaxInt, " ")) //"%.2f"
		{
			fInput = (float)fControlInput * (fRange / maxval);
			bRet = true;
		}
		if (tooltip && ImGui::IsItemHovered())
		{
			bLastSliderHovered = true;
			ImGui::SetTooltip(tooltip);
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushItemWidth(numericboxwidth);
		strcat(cUniqueLabel, "##i");
		if (ImGui::InputFloat(cUniqueLabel, &fControlInput, 0, 0, 2))
		{
			fInput = (float)fControlInput * (fRange / maxval);
			bRet = true;
		}
		if (tooltip && ImGui::IsItemHovered())
		{
			bLastSliderHovered = true;
			ImGui::SetTooltip(tooltip);
		}
		ImGui::PopItemWidth();

		*v = v_min + fInput;
		return(bRet);
	}

	// Use for sliders with large ranges, where fine control over the slider values is required.
	bool MaxSliderInputFloatPower(const char* label, float* v, float v_min, float v_max, const char* tooltip, int startval, float maxval, int numericboxwidth, float power, int precision)
	{
		bLastSliderHovered = false;
		if (!label) return(false);
		char cUniqueLabel[256];
		strncpy(cUniqueLabel, label, 240);
		cUniqueLabel[240] = 0;

		int iCounter = g_SliderData.size() - 1;
		// Check to see if this slider has a non-default max value.
		for (int i = 0; i < g_SliderData.size(); i++)
		{ 
			// Compare the default values first, to save doing lots of string compares.
			if (v_max == g_SliderData[i].fDefaultMaxValue)
			{
				if (strcmp(label, g_SliderData[i].cID) == 0)
				{
					// This slider has a non-default max value, so use it instead of the value passed in.
					if (*v > g_SliderData[i].fHighestMaxValue)
						g_SliderData[i].fHighestMaxValue = *v;

					v_max = g_SliderData[i].fHighestMaxValue;
					maxval = v_max;

					break;
				}
			}

			iCounter--;
		}

		if (iCounter <= 0)
		{
			// This slider does not have a custom max value set yet.
			if (*v > v_max)
			{
				SliderData data;
				data.fDefaultMaxValue = v_max;
				data.fHighestMaxValue = *v;
				strcpy(data.cID, label);
				g_SliderData.push_back(data);

				v_max = *v;
				maxval = v_max;
			}
		}

		float fInput = *v - v_min;
		float fRange = v_max - v_min;
		bool bRet = false;
		float fControlInput, fMax = 100;
		ImGui::PushItemWidth(-10 - 10 - numericboxwidth);
		fControlInput = maxval / fRange * fInput;
		if (fControlInput < startval)
			fControlInput = startval;

		fMax = maxval;

		if (ImGui::SliderFloat(cUniqueLabel, &fControlInput, startval, fMax, "", power))
		{
			fInput = fControlInput * (fRange / maxval);
			bRet = true;
		}
		if (tooltip && ImGui::IsItemHovered())
		{
			bLastSliderHovered = true;
			ImGui::SetTooltip(tooltip);
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushItemWidth(numericboxwidth);
		strcat(cUniqueLabel, "##i");
		//if (ImGui::InputInt(cUniqueLabel, &fControlInput, 0, 0))
		if (ImGui::InputFloat(cUniqueLabel, &fControlInput, 0, 0, precision))
		{
			fInput = fControlInput * (fRange / maxval);
			bRet = true;
		}
		if (!pref.iTurnOffEditboxTooltip && tooltip && ImGui::IsItemHovered())
		{
			bLastSliderHovered = true;
			ImGui::SetTooltip(tooltip);
		}
		ImGui::PopItemWidth();

		*v = v_min + fInput;
		return(bRet);
	}

	bool MaxSliderInputRangeFloat(const char* label, float* v, float* v2, float v_min, float v_max, const char* tooltip)
	{
		bLastSliderHovered = false;
		if (!label) return(false);

		char cUniqueLabel[256];
		strncpy(cUniqueLabel, label, 240);
		cUniqueLabel[240] = 0;

		//int iCounter = g_SliderData.size() - 1;
		//// Check to see if this slider has a non-default max value.
		//for (int i = 0; i < g_SliderData.size(); i++)
		//{
		//	// Compare the default values first, to save doing lots of string compares.
		//	if (v_max == g_SliderData[i].fDefaultMaxValue)
		//	{
		//		if (strcmp(label, g_SliderData[i].cID) == 0)
		//		{
		//			// This slider has a non-default max value, so use it instead of the value passed in.
		//			if (*v2 > g_SliderData[i].fHighestMaxValue)
		//				g_SliderData[i].fHighestMaxValue = *v2;
		//
		//			v_max = g_SliderData[i].fHighestMaxValue;
		//
		//			break;
		//		}
		//	}
		//
		//	iCounter--;
		//}
		//
		//if (iCounter <= 0)
		//{
		//	// This slider does not have a custom max value set yet.
		//	if (*v2 > v_max)
		//	{
		//		SliderData data;
		//		data.fDefaultMaxValue = v_max;
		//		data.fHighestMaxValue = *v2;
		//		strcpy(data.cID, label);
		//		g_SliderData.push_back(data);
		//
		//		v_max = *v2;
		//	}
		//}

		float fInput = *v - v_min;
		float fInput2 = *v2;

		//		if (fInput > fInput2)
		//		{
		//			float ftmp = fInput;
		//			fInput = fInput2;
		//			fInput2 = ftmp;
		//		}

		float fRange = v_max - v_min;
		bool bRet = false;
		int iControlInput, iControlInput2;
		iControlInput = 100.0 / fRange * fInput;
		iControlInput2 = 100.0 / fRange * fInput2;
		float fTmp1 = iControlInput, fTmp2 = iControlInput2;

		//ImGui::SetCursorPos( ImVec2(ImGui::GetCursorPosX(), end_post.y) );
		ImGui::PushItemWidth(30);
		strcat(cUniqueLabel, "##i");
		if (ImGui::InputInt(cUniqueLabel, &iControlInput, 0, 0))
		{
			fInput = ceil((float)iControlInput * (fRange / 100.0));
			bRet = true;
		}
		if (!pref.iTurnOffEditboxTooltip && tooltip && ImGui::IsItemHovered())
		{
			bLastSliderHovered = true;
			ImGui::SetTooltip(tooltip);
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushItemWidth(-10 - 40); //77
		ImVec2 end_post = ImGui::GetCursorPos();
		strcat(cUniqueLabel, "slider");
		if (ImGui::RangeSlider(cUniqueLabel, fTmp1, fTmp2, 100.0f, false))
		{
			fInput = fTmp1 * (fRange / 100.0);
			fInput2 = fTmp2 * (fRange / 100.0);
			bRet = true;
		}
		if (tooltip && ImGui::IsItemHovered())
		{
			bLastSliderHovered = true;
			ImGui::SetTooltip(tooltip);
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushItemWidth(30);
		strcat(cUniqueLabel, "t");
		if (ImGui::InputInt(cUniqueLabel, &iControlInput2, 0, 0))
		{
			fInput2 = ceil((float)iControlInput2 * (fRange / 100.0));
			bRet = true;
		}
		if (!pref.iTurnOffEditboxTooltip && tooltip && ImGui::IsItemHovered())
		{
			bLastSliderHovered = true;
			ImGui::SetTooltip(tooltip);
		}
		ImGui::PopItemWidth();

		//		if (fInput > fInput2)
		//		{
		//			float ftmp = fInput;
		//			fInput = fInput2;
		//			fInput2 = ftmp;
		//		}

		*v = v_min + fInput;
		*v2 = fInput2;
		return(bRet);
	}


	bool MaxSliderInputRangeFloatDirect(const char* label, float* v, float* v2, float v_min, float v_max, const char* tooltip)
	{
		bLastSliderHovered = false;
		if (!label) return(false);

		char cUniqueLabel[256];
		strncpy(cUniqueLabel, label, 240);
		cUniqueLabel[240] = 0;

		float fInput = *v - v_min;
		float fInput2 = *v2;

		float fRange = v_max - v_min;
		bool bRet = false;
		int iControlInput, iControlInput2;
		iControlInput = 100.0 / fRange * fInput;
		iControlInput2 = 100.0 / fRange * fInput2;
		//float fTmp1 = iControlInput, fTmp2 = iControlInput2;
		float fTmp1 = fInput, fTmp2 = fInput2;

		//ImGui::SetCursorPos( ImVec2(ImGui::GetCursorPosX(), end_post.y) );
		ImGui::PushItemWidth(30);
		strcat(cUniqueLabel, "##i");
		if (ImGui::InputInt(cUniqueLabel, &iControlInput, 0, 0))
		{
			fInput = ceil((float)iControlInput * (fRange / 100.0));
			bRet = true;
		}
		if (!pref.iTurnOffEditboxTooltip && tooltip && ImGui::IsItemHovered())
		{
			bLastSliderHovered = true;
			ImGui::SetTooltip(tooltip);
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushItemWidth(-10 - 40); //77
		ImVec2 end_post = ImGui::GetCursorPos();
		strcat(cUniqueLabel, "slider");
		if (ImGui::RangeSlider(cUniqueLabel, fTmp1, fTmp2, v_max, false))
		{
			fInput = fTmp1; // *(fRange / 100.0);
			fInput2 = fTmp2; // *(fRange / 100.0);
			bRet = true;
		}
		if (tooltip && ImGui::IsItemHovered())
		{
			bLastSliderHovered = true;
			ImGui::SetTooltip(tooltip);
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushItemWidth(30);
		strcat(cUniqueLabel, "t");
		if (ImGui::InputInt(cUniqueLabel, &iControlInput2, 0, 0))
		{
			fInput2 = ceil((float)iControlInput2 * (fRange / 100.0));
			bRet = true;
		}
		if (!pref.iTurnOffEditboxTooltip && tooltip && ImGui::IsItemHovered())
		{
			bLastSliderHovered = true;
			ImGui::SetTooltip(tooltip);
		}
		ImGui::PopItemWidth();

		//		if (fInput > fInput2)
		//		{
		//			float ftmp = fInput;
		//			fInput = fInput2;
		//			fInput2 = ftmp;
		//		}

		*v = v_min + fInput;
		*v2 = fInput2;
		return(bRet);
	}


	bool BeginPopupContextItemAGK(const char* str_id, int mouse_button)
	{
		ImGuiWindow* window = GImGui->CurrentWindow;
		ImGuiID id = str_id ? window->GetID(str_id) : window->DC.LastItemId; // If user hasn't passed an ID, we can use the LastItemID. Using LastItemID as a Popup ID won't conflict!
		IM_ASSERT(id != 0);                                                  // You cannot pass a NULL str_id if the last item has no identifier (e.g. a Text() item)
		if (IsMouseReleased(mouse_button) && IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
			OpenPopupEx(id);
		return BeginPopupEx(id, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking);
	}


	bool windowTabVisible(void)
	{
		if (!ImGui::IsWindowDocked()) return(true);
		ImGuiWindow* window = GetCurrentWindow();
		if (!window->DockNode) return(true);
		return window->DockTabIsVisible;
	}
	int windowTabFlags(void)
	{
		//DockTabItemStatusFlags
		ImGuiWindow* window = GetCurrentWindow();
		return (int)window->DockTabItemStatusFlags;
	}
	int windowDockNodeId(void)
	{
		//DockTabItemStatusFlags
		ImGuiWindow* window = GetCurrentWindow();
		if (!window->DockNode) return(0);
		return (int)window->DockNode->ID;
	}

	bool bBlurMode = false;
	void SetBlurMode(bool blur)
	{
		bBlurMode = blur;
	}

	bool ImgBtn(int iImageID, const ImVec2& btn_size, const ImVec4& bg_col,
		const ImVec4& drawCol_normal,
		const ImVec4& drawCol_hover,
		const ImVec4& drawCol_Down, int frame_padding, int atlasindex, int atlasrows, int atlascolumns , bool nowhite , bool gratiant ,bool center_image, bool noalpha, bool useownid, bool boost25)
	{

		ID3D11ShaderResourceView* lpTexture = GetImagePointerView(iImageID);

		if (!lpTexture) return false;
		int iTexID = iImageID;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;
		ImVec2 size = ImVec2(btn_size.x, btn_size.y);
		if (btn_size.x == 0 && btn_size.y == 0) 
		{
			size.x = (float)ImageWidth(iImageID);
			size.y = (float)ImageHeight(iImageID);
		}

		ImVec2 uv0 = ImVec2(0, 0);
		ImVec2 uv1 = ImVec2(1, 1);
		if (atlasindex > 0) {
			//atlasrows
			//atlascolumns
			float asx = (float)ImageWidth(iImageID);
			float asy = (float)ImageHeight(iImageID);

			float uvratiox = 1.0f / (asx);
			float uvratioy = 1.0f / (asy);
			float imgsizex = asx / atlasrows;
			float imgsizey = asy / atlascolumns;

			int index_x = (int)fmod(atlasindex - 1, atlasrows);
			int index_y = (atlasindex - 1) / atlasrows; //atlascolumns;

			float uvborderx = uvratiox;
			float uvbordery = uvratioy;
			uvborderx *= (imgsizex / 32);
			uvbordery *= (imgsizey / 32);

			float atlasstartx = (index_x * (imgsizex)) * uvratiox + (uvborderx*0.5f);
			float atlasstarty = (index_y * (imgsizey)) * uvratioy + (uvbordery*0.5f);
			float atlassizex = (imgsizex)* uvratiox - (uvborderx); //0.987
			float atlassizey = (imgsizey)* uvratioy - (uvbordery);

			uv0 = ImVec2(atlasstartx, atlasstarty);
			uv1 = ImVec2(atlasstartx + atlassizex, atlasstarty + atlassizey);
		}
		if (atlascolumns == 9999)
		{
			uv0 = ImVec2(0, 0);
			uv1 = ImVec2(0.5, 1.0);
		}
		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		if(!useownid) PushID(iTexID);
		const ImGuiID id = window->GetID("#image");
		if (!useownid) PopID();

		const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
		const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2);
		ImRect image_bb(window->DC.CursorPos + padding, window->DC.CursorPos + padding + size);

		image_bb.Floor(); // Fix Small graphical issue with the THUMBNAIL if non-exact-pixel cords was used.

		//PE: Add the background color. not really needed as most buttons are transparent.
		if (bg_col.w > 0.0f && !nowhite) {

			if (gratiant) {
				ImVec4 bg_fade = bg_col;
				for (int i = (int)image_bb.Min.y; i <= (int)image_bb.Max.y; i++)
				{
					window->DrawList->AddRectFilled(ImVec2(image_bb.Min.x, i), ImVec2(image_bb.Max.x, i + 1), GetColorU32(bg_fade));
					bg_fade = bg_fade + ImVec4(0.0055f, 0.0055f, 0.0055f, 0.0055f);
				}
			}
			else
				window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));
		}

		ItemSize(bb);
		if (!ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held);

		// ZJ: Moved this to behind the image so it doesn't darken it.
		////PE: Add the background color. not really needed as most buttons are transparent.
		//if (bg_col.w > 0.0f && !nowhite) {

		//	if (gratiant) {
		//		ImVec4 bg_fade = bg_col;
		//		for (int i = (int)image_bb.Min.y; i <= (int)image_bb.Max.y; i ++ ) 
		//		{
		//			window->DrawList->AddRectFilled( ImVec2(image_bb.Min.x, i), ImVec2(image_bb.Max.x, i+1), GetColorU32(bg_fade));
		//			bg_fade = bg_fade + ImVec4(0.0055f, 0.0055f, 0.0055f, 0.0055f);
		//		}
		//	} else
		//		window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));
		//}

		//if (pref.current_style != 3 && nowhite)
		if(pref.current_style > 2 && pref.current_style < 10 && nowhite)
			window->DrawList->AddCallback( (ImDrawCallback) 1 , NULL );

		if (noalpha) {
			window->DrawList->AddCallback((ImDrawCallback)3, NULL);
		}

		if (boost25)
			window->DrawList->AddCallback((ImDrawCallback)5, NULL); //new shader , boost colors.

		if(bBlurMode)
			window->DrawList->AddCallback((ImDrawCallback)6, NULL); //new shader , blur.


		window->DrawList->AddImage((ImTextureID)lpTexture, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(
			(hovered && held) ? drawCol_Down : hovered ? drawCol_hover : drawCol_normal));

		if ( pref.current_style > 2 && pref.current_style < 10 && nowhite)
			window->DrawList->AddCallback((ImDrawCallback)2, NULL);
		if (noalpha) {
			window->DrawList->AddCallback((ImDrawCallback)4, NULL);
		}
		if (boost25)
			window->DrawList->AddCallback((ImDrawCallback)2, NULL); //switch shader back to normal shader.

		if (bBlurMode)
			window->DrawList->AddCallback((ImDrawCallback)2, NULL); //switch shader back to normal shader.

		if (pref.current_style == 0 && nowhite) {
			ID3D11ShaderResourceView* lpTextureRound = GetImagePointerView(ROUNDING_OVERLAY); //ROUNDING_OVERLAY
			ImVec4 back = ImVec4(1.0, 1.0, 1.0, 1.0);
			window->DrawList->AddImage((ImTextureID)lpTextureRound, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(back));
		}
		if (pref.current_style == 1 && nowhite) {
			ID3D11ShaderResourceView* lpTextureRound = GetImagePointerView(ROUNDING_OVERLAY); //ROUNDING_OVERLAY
			ImVec4 back = ImVec4(0.0, 0.0, 0.0, 1.0);
			window->DrawList->AddImage((ImTextureID)lpTextureRound, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(back));
		}
		if (pref.current_style == 2 && nowhite) {
			ID3D11ShaderResourceView* lpTextureRound = GetImagePointerView(ROUNDING_OVERLAY); //ROUNDING_OVERLAY
			ImVec4 back = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
			window->DrawList->AddImage((ImTextureID)lpTextureRound, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(back));
		}
		//

		if (pressed) {
			return(true);
		}

		return(pressed);
	}
	bool ImgBtnWicked(void* pMaterial, const ImVec2& btn_size, const ImVec4& bg_col,
		const ImVec4& drawCol_normal,
		const ImVec4& drawCol_hover,
		const ImVec4& drawCol_Down, int frame_padding, int atlasindex, int atlasrows, int atlascolumns, bool nowhite, bool gratiant, bool center_image, bool noalpha, bool useownid)
	{

		ID3D11ShaderResourceView* lpTexture = (ID3D11ShaderResourceView*)wiRenderer::GetDevice()->MaterialGetSRV((void*)pMaterial);
		
		//(Texture*) pMaterial

//		pObjectMaterial->textures[MaterialComponent::BASECOLORMAP].resource->texture->desc.Width;

//		auto internal_state = to_internal(resource);
//		ID3D11ShaderResourceView* SRV;

		//ID3D11ShaderResourceView* lpTexture = GetImagePointerView(iImageID);

		if( (void*) lpTexture == (void*) 1)  return false;
		if (!lpTexture) return false;
		int iTexID = (int) lpTexture; //Just need to be unique for each image.

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;
		ImVec2 size = ImVec2(btn_size.x, btn_size.y);
		if (btn_size.x == 0 && btn_size.y == 0)
		{
			size.x = (float) WickedCall_GetTextureWidth(pMaterial);
			size.y = (float) WickedCall_GetTextureHeight(pMaterial);
		}

		ImVec2 uv0 = ImVec2(0, 0);
		ImVec2 uv1 = ImVec2(1, 1);
		if (atlasindex > 0) {
			//atlasrows
			//atlascolumns
			float asx = (float) WickedCall_GetTextureWidth(pMaterial);
			float asy = (float) WickedCall_GetTextureHeight(pMaterial);

			float uvratiox = 1.0f / (asx);
			float uvratioy = 1.0f / (asy);
			float imgsizex = asx / atlasrows;
			float imgsizey = asy / atlascolumns;

			int index_x = (int)fmod(atlasindex - 1, atlasrows);
			int index_y = (atlasindex - 1) / atlasrows; //atlascolumns;

			float uvborderx = uvratiox;
			float uvbordery = uvratioy;
			uvborderx *= (imgsizex / 32);
			uvbordery *= (imgsizey / 32);

			float atlasstartx = (index_x * (imgsizex)) * uvratiox + (uvborderx*0.5f);
			float atlasstarty = (index_y * (imgsizey)) * uvratioy + (uvbordery*0.5f);
			float atlassizex = (imgsizex)* uvratiox - (uvborderx); //0.987
			float atlassizey = (imgsizey)* uvratioy - (uvbordery);

			uv0 = ImVec2(atlasstartx, atlasstarty);
			uv1 = ImVec2(atlasstartx + atlassizex, atlasstarty + atlassizey);
		}
		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		if (!useownid) PushID(iTexID);
		const ImGuiID id = window->GetID("#image");
		if (!useownid) PopID();

		const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
		const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2);
		ImRect image_bb(window->DC.CursorPos + padding, window->DC.CursorPos + padding + size);

		image_bb.Floor(); // Fix Small graphical issue with the THUMBNAIL if non-exact-pixel cords was used.

		ItemSize(bb);
		if (!ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held);

		//PE: Add the background color. not really needed as most buttons are transparent.
		if (bg_col.w > 0.0f && !nowhite) {

			if (gratiant) {
				ImVec4 bg_fade = bg_col;
				for (int i = (int)image_bb.Min.y; i <= (int)image_bb.Max.y; i++)
				{
					window->DrawList->AddRectFilled(ImVec2(image_bb.Min.x, i), ImVec2(image_bb.Max.x, i + 1), GetColorU32(bg_fade));
					bg_fade = bg_fade + ImVec4(0.0055f, 0.0055f, 0.0055f, 0.0055f);
				}
			}
			else
				window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));
		}

		//if (pref.current_style != 3 && nowhite)
		if (pref.current_style > 2 && pref.current_style < 10 && nowhite)
			window->DrawList->AddCallback((ImDrawCallback)1, NULL);

		if (noalpha) {
			window->DrawList->AddCallback((ImDrawCallback)3, NULL);
		}

		window->DrawList->AddImage((ImTextureID)lpTexture, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(
			(hovered && held) ? drawCol_Down : hovered ? drawCol_hover : drawCol_normal));

		if (pref.current_style > 2 && pref.current_style < 10 && nowhite)
			window->DrawList->AddCallback((ImDrawCallback)2, NULL);
		if (noalpha) {
			window->DrawList->AddCallback((ImDrawCallback)4, NULL);
		}

		if (pref.current_style == 0 && nowhite) {
			ID3D11ShaderResourceView* lpTextureRound = GetImagePointerView(ROUNDING_OVERLAY); //ROUNDING_OVERLAY
			ImVec4 back = ImVec4(1.0, 1.0, 1.0, 1.0);
			window->DrawList->AddImage((ImTextureID)lpTextureRound, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(back));
		}
		if (pref.current_style == 1 && nowhite) {
			ID3D11ShaderResourceView* lpTextureRound = GetImagePointerView(ROUNDING_OVERLAY); //ROUNDING_OVERLAY
			ImVec4 back = ImVec4(0.0, 0.0, 0.0, 1.0);
			window->DrawList->AddImage((ImTextureID)lpTextureRound, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(back));
		}
		if (pref.current_style == 2 && nowhite) {
			ID3D11ShaderResourceView* lpTextureRound = GetImagePointerView(ROUNDING_OVERLAY); //ROUNDING_OVERLAY
			ImVec4 back = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
			window->DrawList->AddImage((ImTextureID)lpTextureRound, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(back));
		}
		//

		if (pressed) {
			return(true);
		}

		return(pressed);
	}

	struct ImGuiViewportDataWin32
	{
		HWND    Hwnd;
		bool    HwndOwned;
		DWORD   DwStyle;
		DWORD   DwExStyle;

		ImGuiViewportDataWin32() { Hwnd = NULL; HwndOwned = false;  DwStyle = DwExStyle = 0; }
		~ImGuiViewportDataWin32() { IM_ASSERT(Hwnd == NULL); }
	};

	//PE: function needed when we do test game.
	void ImGui_GG_HideWindow(ImGuiViewport* viewport)
	{
		ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData;
		if (data)
		{
			IM_ASSERT(data->Hwnd != 0);
			if (viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing)
				::ShowWindow(data->Hwnd, SW_HIDE);
			else
				::ShowWindow(data->Hwnd, SW_HIDE);
		}
	}
	void ImGui_GG_ShowWindow(ImGuiViewport* viewport)
	{
		ImGuiViewportDataWin32* data = (ImGuiViewportDataWin32*)viewport->PlatformUserData;
		if (data)
		{
			IM_ASSERT(data->Hwnd != 0);
			if (viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing)
				::ShowWindow(data->Hwnd, SW_SHOWNA);
			else
				::ShowWindow(data->Hwnd, SW_SHOW);
		}
	}

	void HideAllViewPortWindows(void)
	{
		ImGuiContext& g = *GImGui;
		ImGuiViewport* main_viewport = GetMainViewport();
		for (int i = 0; i < g.Viewports.Size; i++)
			//PE: not main viewport.
			if (main_viewport != g.Viewports[i]) {
				ImGui_GG_HideWindow(g.Viewports[i]);
			}

	}
	void ShowAllViewPortWindows(void)
	{
		ImGuiContext& g = *GImGui;
		ImGuiViewport* main_viewport = GetMainViewport();
		for (int i = 0; i < g.Viewports.Size; i++)
			//PE: not main viewport.
			if (main_viewport != g.Viewports[i]) {
				ImGui_GG_ShowWindow(g.Viewports[i]);
			}

	}

	void ToggleButton(const char* str_id, bool* v)
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec4 *style_colors = ImGui::GetStyle().Colors;

		float height = ImGui::GetFrameHeight() * 0.8f;
		float bordery = ImGui::GetFrameHeight() * 0.12f;
		float width = height * 1.7f;
		float radius = height * 0.50f;

		p.y += bordery;

		ImGui::InvisibleButton(str_id, ImVec2(width, height));
		if (ImGui::IsItemClicked())
			*v = !*v;

		float t = *v ? 1.0f : 0.0f;

		ImGuiContext& g = *GImGui;
		float ANIM_SPEED = 0.08f;
		if (g.LastActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (t_anim) : (1.0f - t_anim);
		}

		ImU32 col_bg;
		if (ImGui::IsItemHovered())
			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(style_colors[ImGuiCol_ButtonHovered]), ImVec4(style_colors[ImGuiCol_PlotHistogram]), t));
		else
			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(style_colors[ImGuiCol_FrameBg]), ImVec4(style_colors[ImGuiCol_PlotHistogram]), t));

		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
		draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
	}

	bool RoundButton(const char* str_id, ImVec2 size, float radius)
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec4 *style_colors = ImGui::GetStyle().Colors;

		float height = size.y;
		float bordery = ImGui::GetFrameHeight() * 0.12f;
		float width = size.x;
		//radius *= 0.5;
		if (radius > height)
			radius = height;

		bool ret = ImGui::InvisibleButton(str_id, ImVec2(width, height));

		//p.y += bordery;
		if (height > (radius*2.0))
			p.y += ((height - (radius*2.0)) * 0.5);
		if (width > (radius*2.0))
			p.x += ((width - (radius*2.0)) * 0.5);

		ImU32 col_bg;

		if (pref.current_style == 3) {
			col_bg = ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_Button]));
			if (!ImGui::IsItemHovered())
				col_bg = ImGui::GetColorU32(ImLerp(ImVec4(style_colors[ImGuiCol_ButtonHovered]), ImVec4(style_colors[ImGuiCol_Button]), 0.5));
		}
		else {
			col_bg = ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_PlotHistogram]));
			if (ImGui::IsItemHovered())
				col_bg = ImGui::GetColorU32(ImLerp(ImVec4(style_colors[ImGuiCol_ButtonHovered]), ImVec4(style_colors[ImGuiCol_PlotHistogram]), 0.5));
		}

		draw_list->AddCircleFilled(ImVec2(p.x + radius, p.y + radius), radius, IM_COL32(0, 0, 0, 128));
		draw_list->AddCircleFilled(ImVec2(p.x + radius, p.y + radius), radius - 1.5f, col_bg);


		return ret;

	}
	//PE: Currently only to be used for InputText , always return true if we have a blinking cursor.
	bool MaxIsItemFocused(void)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		//PE: Check if clicked outside item.
		if (g.ActiveId != g.NavId)
			return false;

		//PE: dont consider g.NavDisableHighlight.
		if (g.NavId == 0 || g.NavId != window->DC.LastItemId)
			return false;

		// Special handling for the dummy item after Begin() which represent the title bar or tab. 
		// When the window is collapsed (SkipItems==true) that last item will never be overwritten so we need to detect the case.
		if (window->DC.LastItemId == window->ID && window->WriteAccessed)
			return false;

		return true;
	}


	int rotation_start_index;
	void ImRotateStart()
	{
		rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
	}

	ImVec2 ImRotationCenter()
	{
		ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

		const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
		for (int i = rotation_start_index; i < buf.Size; i++)
			l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

		return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
	}

	void ImRotateEnd(float rad, ImVec2 center)
	{
		float s = sin(rad), c = cos(rad);
		center = ImRotate(center, s, c) - center;

		auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
		for (int i = rotation_start_index; i < buf.Size; i++)
			buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
	}



	//PE: https://github.com/nem0/LumixEngine/blob/timeline_gui/external/imgui/imgui_user.inl#L814

	static float s_max_timeline_value = 100.0f;

	bool BeginTimeline(const char* str_id, float max_value)
	{
		s_max_timeline_value = max_value;
		return BeginChild(str_id);
	}


	static const float TIMELINE_RADIUS = 6;


	bool TimelineEvent(const char* str_id, float* values)
	{
		ImGuiWindow* win = GetCurrentWindow();
		const ImU32 inactive_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Button]);
		const ImU32 active_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_ButtonHovered]);
		const ImU32 line_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_ButtonActive]); //ImGuiCol_ColumnActive
		bool changed = false;
		ImVec2 cursor_pos = win->DC.CursorPos;

		for (int i = 0; i < 2; ++i)
		{
			ImVec2 pos = cursor_pos;
			pos.x += win->Size.x * values[i] / s_max_timeline_value + TIMELINE_RADIUS;
			pos.y += TIMELINE_RADIUS;

			SetCursorScreenPos(pos - ImVec2(TIMELINE_RADIUS, TIMELINE_RADIUS));
			PushID(i);
			InvisibleButton(str_id, ImVec2(2 * TIMELINE_RADIUS, 2 * TIMELINE_RADIUS));
			if (IsItemActive() || IsItemHovered())
			{
				ImGui::SetTooltip("%f", values[i]);
				ImVec2 a(pos.x, GetWindowContentRegionMin().y + win->Pos.y);
				ImVec2 b(pos.x, GetWindowContentRegionMax().y + win->Pos.y);
				win->DrawList->AddLine(a, b, line_color);
			}
			if (IsItemActive() && IsMouseDragging(0))
			{
				values[i] += GetIO().MouseDelta.x / win->Size.x * s_max_timeline_value;
				changed = true;
			}
			PopID();
			win->DrawList->AddCircleFilled(
				pos, TIMELINE_RADIUS, IsItemActive() || IsItemHovered() ? active_color : inactive_color);
		}

		ImVec2 start = cursor_pos;
		start.x += win->Size.x * values[0] / s_max_timeline_value + 2 * TIMELINE_RADIUS;
		start.y += TIMELINE_RADIUS * 0.5f;
		ImVec2 end = start + ImVec2(win->Size.x * (values[1] - values[0]) / s_max_timeline_value - 2 * TIMELINE_RADIUS,
			TIMELINE_RADIUS);

		PushID(-1);
		SetCursorScreenPos(start);
		InvisibleButton(str_id, end - start);
		if (IsItemActive() && IsMouseDragging(0))
		{
			values[0] += GetIO().MouseDelta.x / win->Size.x * s_max_timeline_value;
			values[1] += GetIO().MouseDelta.x / win->Size.x * s_max_timeline_value;
			changed = true;
		}
		PopID();

		SetCursorScreenPos(cursor_pos + ImVec2(0, GetTextLineHeightWithSpacing()));

		win->DrawList->AddRectFilled(start, end, IsItemActive() || IsItemHovered() ? active_color : inactive_color);

		if (values[0] > values[1])
		{
			float tmp = values[0];
			values[0] = values[1];
			values[1] = tmp;
		}
		if (values[1] > s_max_timeline_value) values[1] = s_max_timeline_value;
		if (values[0] < 0) values[0] = 0;
		return changed;
	}

	static double s_time_scale = 1;
	static double s_time_offset = 0;


	bool RangeSlider(const char* str_id, float & val1, float & val2, float max_value, bool bDisplayValues)
	{
		float values[2] = { val1 , val2 };

		s_max_timeline_value = max_value;
		ImGuiWindow* win = GetCurrentWindow();
		const ImU32 inactive_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Button]);
		const ImU32 active_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_ButtonHovered]);
		const ImU32 line_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_ButtonActive]);
		bool changed = false;
		ImVec2 cursor_pos = win->DC.CursorPos;
		ImVec2 vContentSize = win->Size;
		//vContentSize.x -= 60.0f;
		float fMinimumButtonSpace = 10.0f;
		vContentSize.x = CalcItemWidth() - 10.0f; //GetContentRegionAvail();
		vContentSize.y = win->Size.y;

		const ImVec2 label_size = CalcTextSize(str_id, NULL, true);
		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = win->GetID(str_id);

		ImVec2 size = CalcItemSize(ImVec2(-10,0), label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
		vContentSize.y = size.y;
		const ImRect bb(cursor_pos, cursor_pos + ImVec2(vContentSize.x + fMinimumButtonSpace, vContentSize.y) );

		vContentSize.x -= fMinimumButtonSpace;

		if (IsMouseHoveringRect(bb.Min, bb.Max)) {
			//Avtive dont look so good.
			//if (IsMouseDown(0))
			//	win->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBgActive), style.FrameRounding, ImDrawCornerFlags_Bot);
			//else
			win->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBgHovered), style.FrameRounding, ImDrawCornerFlags_Bot);
		}
		else {
			win->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg), style.FrameRounding, ImDrawCornerFlags_Bot);
		}

		const float border_size = g.Style.FrameBorderSize;
		if (border_size > 0.0f)
		{
			win->DrawList->AddRect(bb.Min + ImVec2(1, 1), bb.Max + ImVec2(1, 1), GetColorU32(ImGuiCol_BorderShadow), style.FrameRounding, ImDrawCornerFlags_All, border_size);
			win->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_Border), style.FrameRounding, ImDrawCornerFlags_All, border_size);
		}

		bool bButInvHover = false;
		ImGuiID gad_id[2];

		//PE: Middle bar.
		int iMiddleBarY = 6,iSideBordersX = 1; //2

		iMiddleBarY = vContentSize.y - 4.0;
		ImVec2 start = cursor_pos;
		start.x += vContentSize.x * values[0] / s_max_timeline_value + iSideBordersX * TIMELINE_RADIUS;
		start.y += (vContentSize.y*0.5) - (iMiddleBarY*0.5);
		ImVec2 end = start + ImVec2((vContentSize.x * (values[1] - values[0]) / s_max_timeline_value - iSideBordersX * TIMELINE_RADIUS) + fMinimumButtonSpace - iSideBordersX, iMiddleBarY);
		win->DrawList->AddRectFilled(start, end, IsItemActive() || IsItemHovered() ? active_color : inactive_color,0.0f);

		for (int i = 0; i < 2; ++i)
		{
			ImVec2 pos = cursor_pos;
			float ysize = size.y - (style.FramePadding.y * 2.0f);
			pos.x += vContentSize.x * values[i] / s_max_timeline_value + TIMELINE_RADIUS;
			pos.y += ysize;
			
			if (i == 1) pos.x += fMinimumButtonSpace;

			SetCursorScreenPos(pos - ImVec2(TIMELINE_RADIUS, ysize));
			//PushID(i);
			PushOverrideID(id+i);
			InvisibleButton(str_id, ImVec2(10.0, size.y ));

			gad_id[i] = win->GetID(str_id);

			ImGuiWindow* window = GetCurrentWindow();

			//PE: Disabled the hover values , as we now have a input box.
			if (bDisplayValues)
			{
				if (IsItemActive() || IsItemHovered())
				{
					ImGui::SetTooltip("%.2f", values[i]);
					bButInvHover = true;
					SetHoveredID(0);
					window->DC.LastItemStatusFlags &= ~ImGuiItemStatusFlags_HoveredRect;
				}
			}

			if (IsItemActive() && IsMouseDragging(0))
			{
				//values[i] += (GetIO().MouseDelta.x / (vContentSize.x) ) * s_max_timeline_value;

				//PE: This system works way better, more easy to slide.
				ImVec2 mousepos = ImGui::GetMousePos();
				float range = (bb.Max.x - cursor_pos.x) - (bb.Min.x - cursor_pos.x);
				float val = mousepos.x - cursor_pos.x;
				float newval = (s_max_timeline_value / range) * val;
				values[i] = newval;
				if (values[i] > max_value)
					values[i] = max_value;
				if (values[i] < 0)
					values[i] = 0;
				changed = true;
			}
			if (i == 1 && !bButInvHover)
			{
				if (IsMouseHoveringRect(bb.Min, bb.Max)) {
					SetHoveredID(window->DC.LastItemId);
					window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_HoveredRect;
				}
			}
			PopID();

			pos -= ImVec2(TIMELINE_RADIUS, ysize);
			//win->DrawList->AddCircleFilled(pos, TIMELINE_RADIUS, IsItemActive() || IsItemHovered() ? active_color : inactive_color);
			if(IsItemActive())
				win->DrawList->AddRectFilled(pos, pos + ImVec2(10.0, size.y), GetColorU32(ImGuiCol_SliderGrabActive), style.GrabRounding);
			else
				win->DrawList->AddRectFilled(pos, pos + ImVec2(10.0, size.y), GetColorU32(ImGuiCol_SliderGrab), style.GrabRounding);

		}


		SetCursorScreenPos(start);
		//PushID(-1);
		if (values[1] - values[0] > 2)
		{
			PushOverrideID(id + 3);
			ImGui::SetItemAllowOverlap();
			InvisibleButton(str_id, end - start - ImVec2(2,0) );
			static float fMiddleBarMouseXDown = -1;
			if (IsItemActive())
			{
				if (ImGui::IsMouseDown(0)) // IsMouseDragging(0)
				{
					//float deltax = GetIO().MouseDelta.x / vContentSize.x * s_max_timeline_value;
					ImVec2 mousepos = ImGui::GetMousePos();
					float range = (bb.Max.x - cursor_pos.x) - (bb.Min.x - cursor_pos.x);
					float val = mousepos.x - cursor_pos.x;
					float newval = (s_max_timeline_value / range) * val;
					if (fMiddleBarMouseXDown == -1)
						fMiddleBarMouseXDown = newval;

					float deltax = newval - fMiddleBarMouseXDown;
					fMiddleBarMouseXDown = newval;
					if (values[0] + deltax >= 0 && values[1] + deltax <= (s_max_timeline_value + 1.0))
					{
						values[0] += deltax;
						values[1] += deltax;
						changed = true;
					}
				}
			}
			else {
				if (!ImGui::IsMouseDown(0))
				{
					fMiddleBarMouseXDown = -1;
				}
			}
			PopID();
		}
		//win->DrawList->AddRectFilled(start, end, IsItemActive() || IsItemHovered() ? active_color : inactive_color,2.0f);


		if (values[0] > values[1])
		{
			//Swap active id (and gadget).
			ImGuiContext& g = *GImGui;
			if (g.ActiveId)
			{
				if (g.ActiveId == gad_id[0])
				{
					g.ActiveId = gad_id[1];
					SetActiveID(g.ActiveId, win);
				}
				else if (g.ActiveId == gad_id[1])
				{
					g.ActiveId = gad_id[0];
					SetActiveID(g.ActiveId, win);
				}
			}

			float tmp = values[0];
			values[0] = values[1];
			values[1] = tmp;
		}

		if (values[1] > s_max_timeline_value) values[1] = s_max_timeline_value;
		if (values[0] < 0) values[0] = 0;

//		if(values[1]- values[0] > 2)
//			win->DrawList->AddRectFilled(start, end, IsItemActive() || IsItemHovered() ? active_color : inactive_color, 0.0f);

		char value_buf[64];
		sprintf(value_buf, "%.2f - %.2f", values[0], values[1]);
		if(bDisplayValues)
			RenderTextClipped(bb.Min, bb.Max, value_buf, value_buf+strlen(value_buf), NULL, ImVec2(0.5f, 0.5f));

		SetCursorScreenPos(cursor_pos);
		vContentSize.x += fMinimumButtonSpace;
		vContentSize.x += fMinimumButtonSpace;
		ItemSize(vContentSize, 0);
		SetCursorScreenPos(cursor_pos + ImVec2(0, g.FontSize + g.Style.ItemSpacing.y ));

		val1 = values[0];
		val2 = values[1];
		return changed;
	}



	void EndTimeline()
	{
		ImGuiWindow* win = GetCurrentWindow();

		ImU32 color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Button]);
		ImU32 line_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Border]);
		ImU32 text_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Text]);
		float rounding = GImGui->Style.ScrollbarRounding;
		ImVec2 start(GetWindowContentRegionMin().x + win->Pos.x,
			GetWindowContentRegionMax().y - GetTextLineHeightWithSpacing() + win->Pos.y);
		ImVec2 end = GetWindowContentRegionMax() + win->Pos;

		win->DrawList->AddRectFilled(start, end, color, rounding);

		const int LINE_COUNT = 5;
		const ImVec2 text_offset(0, GetTextLineHeightWithSpacing());
		for (int i = 0; i < LINE_COUNT; ++i)
		{
			ImVec2 a = GetWindowContentRegionMin() + win->Pos + ImVec2(TIMELINE_RADIUS, 0);
			a.x += i * GetWindowContentRegionWidth() / LINE_COUNT;
			ImVec2 b = a;
			b.y = start.y;
			win->DrawList->AddLine(a, b, line_color);
			char tmp[256];
			ImFormatString(tmp, sizeof(tmp), "%.2f", i * s_max_timeline_value / LINE_COUNT);
			win->DrawList->AddText(b, text_color, tmp);
		}

		EndChild();
	}

}

void myDefaultStyles(void)
{
	ImGui::GetStyle().TabRounding = 4;
	ImGui::GetStyle().ChildRounding = 3.0f;
	ImGui::GetStyle().FrameRounding = 3.0f;
	ImGui::GetStyle().ItemSpacing = ImVec2(8.0f, 4.0f);
	ImGui::GetStyle().ScrollbarRounding = 9.0f;
	ImGui::GetStyle().WindowBorderSize = 2.0f;
}

//PE: Styles from AGKS
void myDarkStyle(ImGuiStyle* dst)
{
	ImGui::StyleColorsDark();
	//Small overwrites to dark style.
	ImGuiStyle &st = ImGui::GetStyle();
	st.WindowBorderSize = 2.0f;
	st.WindowPadding = { 4.0f,4.0f };
	st.ScrollbarSize = 18.0;
	st.Colors[ImGuiCol_Separator] = { 0.16f, 0.29f, 0.48f, 0.60f };
	st.Colors[ImGuiCol_Tab] = { 0.29f, 0.29f, 0.29f, 0.86f };
	st.Colors[ImGuiCol_DockingPreview] = ImVec4(0.36f, 0.49f, 0.68f, 0.80f);
	st.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.46f, 0.59f, 0.78f, 0.90f);
	st.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.247f, 0.353f, 0.507f, 0.90f);
	st.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	st.Colors[ImGuiCol_PopupBg] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

	st.WindowRounding = 3.0f;
	st.ChildRounding = 3.0f;
	st.FrameRounding = 3.0f;

	//st.TabBorderSize = 0.0f;
	//st.TabRounding = 8.0f;

	st.FramePadding = ImVec2(4.0f, 4.0f);

	st.Colors[ImGuiCol_Tab] = { 0.161f, 0.290f, 0.478f, 1.000f };
	st.Colors[ImGuiCol_TabUnfocused] = { 0.161f, 0.290f, 0.478f, 1.000f };
	st.Colors[ImGuiCol_TabUnfocusedActive] = { 0.200f, 0.410f, 0.680f, 1.000f };
	st.Colors[ImGuiCol_TitleBg] = { 0.160f, 0.290f, 0.480f, 1.000f };
	TintCurrentStyle();
}

void myLightStyle(ImGuiStyle* dst)
{
	ImGui::StyleColorsLight();
	//Small overwrites to light style.
	ImGuiStyle &st = ImGui::GetStyle();
	ImVec4* Colors = st.Colors;
	st.WindowBorderSize = 2.0f;
	st.WindowPadding = { 4.0f,4.0f };
	st.ScrollbarSize = 18.0;

	st.WindowRounding = 3.0f;
	st.ChildRounding = 3.0f;
	st.FrameRounding = 3.0f;

	Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.20f, 0.00f, 1.00f); //Also <h1> tags in help.
	TintCurrentStyle();
}

void myStyle(ImGuiStyle* dst)
{
	ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
	ImVec4* Colors = style->Colors;

	Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	//Colors[ImGuiCol_TextHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	//Colors[ImGuiCol_TextActive] = ImVec4(1.00f, 1.00f, 0.00f, 1.00f);
	Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
	Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	//Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
	Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	Colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

//	Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
//	Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
//	Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

	Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	//Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
	//Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	//Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f); //Also <h1> tags in help.
	Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	//Colors[ImGuiCol_TooltipBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	style->Colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);
	TintCurrentStyle();
}

void myStyleBlue(ImGuiStyle* dst)
{
	myStyle2(dst);
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_TextDisabled] = ImVec4(0.39f, 0.56f, 0.68f, 1.00f);
	colors[ImGuiCol_Text] = ImVec4(0.95f, 0.98f, 1.00f, 1.00f);
	//colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.16f, 0.22f, 0.97f); //Add a little transparent.
	colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.16f, 0.22f, 0.9f); //Add a little transparent.
	colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.16f, 0.22f, 1.0f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.16f, 0.22f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.58f, 0.58f, 0.58f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.11f, 0.16f, 0.23f, 1.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.16f, 0.23f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.23f, 0.33f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.11f, 0.16f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.11f, 0.16f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.43f, 0.57f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.11f, 0.16f, 0.23f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.26f, 0.35f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.11f, 0.16f, 0.23f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.22f, 0.43f, 0.57f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.22f, 0.43f, 0.57f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.22f, 0.43f, 0.57f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.95f, 0.98f, 1.00f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.22f, 0.43f, 0.57f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.18f, 0.36f, 0.48f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.16f, 0.23f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.22f, 0.43f, 0.57f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.22f, 0.43f, 0.57f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.22f, 0.43f, 0.57f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.61f, 0.63f, 0.69f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.61f, 0.63f, 0.69f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.61f, 0.63f, 0.69f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.16f, 0.22f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.20f, 0.38f, 0.51f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.22f, 0.43f, 0.57f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.22f, 0.43f, 0.57f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.22f, 0.43f, 0.57f, 1.00f);

	// Wicked renders first, IMGUI last, let Wicked renderings through! 
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);

	colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.6f);

	colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);

	//PE: Darker tabs so they dont look like buttons.
	float transparent = 0.55;
	colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.16f, 0.22f, transparent);
	colors[ImGuiCol_TabHovered] = ImVec4(0.20f, 0.38f, 0.51f, transparent);
	colors[ImGuiCol_TabActive] = ImVec4(0.22f, 0.43f, 0.57f, transparent);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.22f, 0.43f, 0.57f, transparent);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.22f, 0.43f, 0.57f, transparent);

	//ImGui::GetStyle().TabBorderSize = 1.0f;
	//ImGui::GetStyle().TabRounding = 1.0f;


	TintCurrentStyle();

}

void DarkColorsNoTransparent(void)
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	//colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_Text] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	//colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.16f, 0.98f);
	colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	//colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.18f, 0.94f);
	//colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 0.67f);
	//colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);

	colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.44f, 0.44f, 0.44f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.47f, 0.48f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
//	colors[ImGuiCol_Header] = ImVec4(0.70f, 0.70f, 0.70f, 0.31f);
	colors[ImGuiCol_Header] = ImVec4(0.70f, 0.70f, 0.70f, 0.25f);
//	colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.80f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.40f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.50f, 0.52f, 1.00f);
	//colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	//colors[ImGuiCol_SeparatorHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.52f, 0.52f, 0.52f, 0.78f);

	colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	//PE: VS2022 style
	const float r = pref.highlight_color.x; // (1.0f / 255.0f) * 14;
	const float g = pref.highlight_color.y; // (1.0f / 255.0f) * 99;
	const float b = pref.highlight_color.z; // (1.0f / 255.0f) * 156;

	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(r, g, b, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);

	auto* style = &ImGui::GetStyle();
	style->ItemSpacing.y = 3.0f;
	style->TabRounding = 0;
	style->ChildRounding = 0;
	style->FrameRounding = 3;
	style->ScrollbarRounding = 1;
	style->WindowBorderSize = 0;

	TintCurrentStyle();
}

void myStyle2(ImGuiStyle* dst)
{
	auto *style = (dst ? dst : &ImGui::GetStyle());
	style->WindowRounding = 5.3f;
	style->GrabRounding = style->FrameRounding = 2.3f;
	style->ScrollbarRounding = 5.0f;
	style->FrameBorderSize = 1.0f;
	style->ItemSpacing.y = 6.5f;

	style->ScrollbarSize = 16.0;

	style->Colors[ImGuiCol_Text] = { 0.78f, 0.78f, 0.78f, 1.00f };
	style->Colors[ImGuiCol_TextDisabled] = { 0.55f, 0.55f, 0.55f, 1.00f };

	style->Colors[ImGuiCol_WindowBg] = { 0.23f, 0.23f, 0.23f, 0.75f }; //0.98f
	style->Colors[ImGuiCol_ChildBg] = { 0.23529413f, 0.24705884f, 0.25490198f, 0.00f };

	style->Colors[ImGuiCol_PopupBg] = { 0.31f, 0.32f, 0.34f, 1.0f }; //0.94

	style->Colors[ImGuiCol_Border] = { 0.33333334f, 0.33333334f, 0.33333334f, 0.50f };
	style->Colors[ImGuiCol_BorderShadow] = { 0.15686275f, 0.15686275f, 0.15686275f, 0.00f };

	style->Colors[ImGuiCol_FrameBg] = { 0.16862746f, 0.16862746f, 0.16862746f, 0.64f };

	style->Colors[ImGuiCol_FrameBgHovered] = { 0.453125f, 0.67578125f, 0.99609375f, 0.77f };
	style->Colors[ImGuiCol_FrameBgActive] = { 0.47058827f, 0.47058827f, 0.47058827f, 0.77f };

	style->Colors[ImGuiCol_TitleBg] = { 0.04f, 0.04f, 0.04f, 1.00f };
	style->Colors[ImGuiCol_TitleBgCollapsed] = { 0.16f, 0.29f, 0.48f, 1.00f };
	style->Colors[ImGuiCol_TitleBgActive] = { 0.00f, 0.00f, 0.00f, 1.0f };

	style->Colors[ImGuiCol_MenuBarBg] = { 0.27058825f, 0.28627452f, 0.2901961f, 0.92f };

	style->Colors[ImGuiCol_ScrollbarBg] = { 0.195f, 0.195f, 0.195f, 0.60f };
	style->Colors[ImGuiCol_ScrollbarGrab] = { 0.39f, 0.39f, 0.39f, 0.51f };

	style->Colors[ImGuiCol_ScrollbarGrabHovered] = { 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };
	style->Colors[ImGuiCol_ScrollbarGrabActive] = { 0.13725491f, 0.19215688f, 0.2627451f, 0.91f };
	// style->Colors[ImGuiCol_ComboBg]               = {0.1f, 0.1f, 0.1f, 0.99f};
	style->Colors[ImGuiCol_CheckMark] = { 0.90f, 0.90f, 0.90f, 0.83f };
	style->Colors[ImGuiCol_SliderGrab] = { 0.70f, 0.70f, 0.70f, 0.62f };
	style->Colors[ImGuiCol_SliderGrabActive] = { 0.30f, 0.30f, 0.30f, 0.84f };
	style->Colors[ImGuiCol_Button] = { 0.33333334f, 0.3529412f, 0.36078432f, 0.49f };
	style->Colors[ImGuiCol_ButtonHovered] = { 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };
	style->Colors[ImGuiCol_ButtonActive] = { 0.13725491f, 0.19215688f, 0.2627451f, 1.00f };
	style->Colors[ImGuiCol_Header] = { 0.33333334f, 0.3529412f, 0.36078432f, 0.53f };
	style->Colors[ImGuiCol_HeaderHovered] = { 0.453125f, 0.67578125f, 0.99609375f, 0.67f };
	style->Colors[ImGuiCol_HeaderActive] = { 0.47058827f, 0.47058827f, 0.47058827f, 0.67f };
	style->Colors[ImGuiCol_Separator] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
	style->Colors[ImGuiCol_SeparatorHovered] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
	style->Colors[ImGuiCol_SeparatorActive] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
	style->Colors[ImGuiCol_ResizeGrip] = { 1.00f, 1.00f, 1.00f, 0.85f };
	style->Colors[ImGuiCol_ResizeGripHovered] = { 1.00f, 1.00f, 1.00f, 0.60f };
	style->Colors[ImGuiCol_ResizeGripActive] = { 1.00f, 1.00f, 1.00f, 0.90f };
	style->Colors[ImGuiCol_PlotLines] = { 0.61f, 0.61f, 0.61f, 1.00f };
	style->Colors[ImGuiCol_PlotLinesHovered] = { 1.00f, 0.43f, 0.35f, 1.00f };
	style->Colors[ImGuiCol_PlotHistogram] = { 0.90f, 0.70f, 0.00f, 1.00f }; //Also <h1> tags in help.
	style->Colors[ImGuiCol_PlotHistogramHovered] = { 1.00f, 0.60f, 0.00f, 1.00f };
	style->Colors[ImGuiCol_TextSelectedBg] = { 0.18431373f, 0.39607847f, 0.79215693f, 0.90f };

	//Still need to be set.
	style->Colors[ImGuiCol_Tab] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	style->Colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_TabActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	style->Colors[ImGuiCol_DockingPreview] = ImVec4(0.38f, 0.48f, 0.60f, 1.00f);

	// Wicked renders first, IMGUI last, let Wicked renderings through! 
	 style->Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);

	style->Colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style->Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
	style->Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
	style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.6f);


	style->Colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);

	TintCurrentStyle();
}


void myStyle2_colors_only(void)
{
	auto *style = &ImGui::GetStyle();

	style->Colors[ImGuiCol_Text] = { 0.78f, 0.78f, 0.78f, 1.00f };
	style->Colors[ImGuiCol_TextDisabled] = { 0.55f, 0.55f, 0.55f, 1.00f };
	style->Colors[ImGuiCol_WindowBg] = { 0.23f, 0.23f, 0.23f, 0.75f }; //0.98f
	style->Colors[ImGuiCol_ChildBg] = { 0.23529413f, 0.24705884f, 0.25490198f, 0.00f };
	style->Colors[ImGuiCol_PopupBg] = { 0.31f, 0.32f, 0.34f, 1.0f }; //0.94
	style->Colors[ImGuiCol_Border] = { 0.33333334f, 0.33333334f, 0.33333334f, 0.50f };
	style->Colors[ImGuiCol_BorderShadow] = { 0.15686275f, 0.15686275f, 0.15686275f, 0.00f };
	style->Colors[ImGuiCol_FrameBg] = { 0.16862746f, 0.16862746f, 0.16862746f, 0.64f };
	style->Colors[ImGuiCol_FrameBgHovered] = { 0.453125f, 0.67578125f, 0.99609375f, 0.77f };
	style->Colors[ImGuiCol_FrameBgActive] = { 0.47058827f, 0.47058827f, 0.47058827f, 0.77f };
	style->Colors[ImGuiCol_TitleBg] = { 0.04f, 0.04f, 0.04f, 1.00f };
	style->Colors[ImGuiCol_TitleBgCollapsed] = { 0.16f, 0.29f, 0.48f, 1.00f };
	style->Colors[ImGuiCol_TitleBgActive] = { 0.00f, 0.00f, 0.00f, 1.0f };
	style->Colors[ImGuiCol_MenuBarBg] = { 0.27058825f, 0.28627452f, 0.2901961f, 0.92f };
	style->Colors[ImGuiCol_ScrollbarBg] = { 0.195f, 0.195f, 0.195f, 0.60f };
	style->Colors[ImGuiCol_ScrollbarGrab] = { 0.39f, 0.39f, 0.39f, 0.51f };
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = { 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };
	style->Colors[ImGuiCol_ScrollbarGrabActive] = { 0.13725491f, 0.19215688f, 0.2627451f, 0.91f };
	style->Colors[ImGuiCol_CheckMark] = { 0.90f, 0.90f, 0.90f, 0.83f };
	style->Colors[ImGuiCol_SliderGrab] = { 0.70f, 0.70f, 0.70f, 0.62f };
	style->Colors[ImGuiCol_SliderGrabActive] = { 0.30f, 0.30f, 0.30f, 0.84f };
	style->Colors[ImGuiCol_Button] = { 0.33333334f, 0.3529412f, 0.36078432f, 0.49f };
	style->Colors[ImGuiCol_ButtonHovered] = { 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };
	style->Colors[ImGuiCol_ButtonActive] = { 0.13725491f, 0.19215688f, 0.2627451f, 1.00f };
	style->Colors[ImGuiCol_Header] = { 0.33333334f, 0.3529412f, 0.36078432f, 0.53f };
	style->Colors[ImGuiCol_HeaderHovered] = { 0.453125f, 0.67578125f, 0.99609375f, 0.67f };
	style->Colors[ImGuiCol_HeaderActive] = { 0.47058827f, 0.47058827f, 0.47058827f, 0.67f };
	style->Colors[ImGuiCol_Separator] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
	style->Colors[ImGuiCol_SeparatorHovered] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
	style->Colors[ImGuiCol_SeparatorActive] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
	style->Colors[ImGuiCol_ResizeGrip] = { 1.00f, 1.00f, 1.00f, 0.85f };
	style->Colors[ImGuiCol_ResizeGripHovered] = { 1.00f, 1.00f, 1.00f, 0.60f };
	style->Colors[ImGuiCol_ResizeGripActive] = { 1.00f, 1.00f, 1.00f, 0.90f };
	style->Colors[ImGuiCol_PlotLines] = { 0.61f, 0.61f, 0.61f, 1.00f };
	style->Colors[ImGuiCol_PlotLinesHovered] = { 1.00f, 0.43f, 0.35f, 1.00f };
	style->Colors[ImGuiCol_PlotHistogram] = { 0.90f, 0.70f, 0.00f, 1.00f }; //Also <h1> tags in help.
	style->Colors[ImGuiCol_PlotHistogramHovered] = { 1.00f, 0.60f, 0.00f, 1.00f };
	style->Colors[ImGuiCol_TextSelectedBg] = { 0.18431373f, 0.39607847f, 0.79215693f, 0.90f };
	style->Colors[ImGuiCol_Tab] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	style->Colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_TabActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	style->Colors[ImGuiCol_DockingPreview] = ImVec4(0.38f, 0.48f, 0.60f, 1.00f);

	// Wicked renders first, IMGUI last, let Wicked renderings through! 
//	style->Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);

	style->Colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style->Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
	style->Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
	style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.6f);
	style->Colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);

	//TintCurrentStyle();
}

void myStyle3(ImGuiStyle* dst)
{
	ImGuiStyle &st = ImGui::GetStyle();
	st.FrameBorderSize = 1.0f;
	st.FramePadding = ImVec2(4.0f, 2.0f);
	st.ItemSpacing = ImVec2(8.0f, 2.0f);
	st.WindowBorderSize = 2.0f;
	//	st.TabBorderSize = 1.0f;
	st.WindowRounding = 1.0f;
	st.ChildRounding = 1.0f;
	st.FrameRounding = 1.0f;
	st.ScrollbarRounding = 1.0f;
	st.ScrollbarSize = 18.0;
	st.GrabRounding = 1.0f;
	//	st.TabRounding = 1.0f;

	//	st.TabBorderSize = 5.0f;
	//	st.TabRounding = 2.0f;


	// Setup style
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.12f, 0.12f, 0.941f);
	colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.031f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.53f, 0.53f, 0.53f, 0.25f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.0f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.22f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 0.53f);

	colors[ImGuiCol_TitleBg] = ImVec4(0.114f, 0.191f, 0.199f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.203f, 0.335f, 0.348f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.0f);

	colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.48f, 0.47f, 0.47f, 0.91f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.55f, 0.55f, 0.62f);
	colors[ImGuiCol_Button] = ImVec4(0.50f, 0.50f, 0.50f, 0.63f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.67f, 0.68f, 0.63f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.26f, 0.26f, 0.63f);
	colors[ImGuiCol_Header] = ImVec4(0.54f, 0.54f, 0.54f, 0.58f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.64f, 0.65f, 0.65f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
	colors[ImGuiCol_Separator] = ImVec4(0.58f, 0.58f, 0.58f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.5f, 0.5f, 0.5f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
	colors[ImGuiCol_Tab] = ImVec4(0.114f, 0.161f, 0.200f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.134f, 0.181f, 0.220f, 0.86f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.114f, 0.161f, 0.200f, 0.86f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.124f, 0.171f, 0.210f, 1.0f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.38f, 0.48f, 0.60f, 1.00f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.77f, 0.33f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.87f, 0.55f, 0.08f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.47f, 0.60f, 0.76f, 0.47f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	TintCurrentStyle();
}


void TintColor(ImVec4 & color, int iModeColor = 0)
{
	ImVec4 tint = pref.tint_style;
	ImVec4 shade = pref.shade_style;
	ImVec4 title = pref.title_style;

	if (pref.iEnableCustomColors || (pref.current_style >= 10 && pref.current_style != 25 ))
	{
		//PE: pref.current_style == 25 (Blue) is a little special as it is used as a new "main" color setup.
		bool bUseGrayScale = true;
		float grayscale = (color.x + color.y + color.z) / 3.0;
		if (iModeColor == 2)
		{
			bool bNoTitle = false;
			if (title.x <= 0.008 && title.y <= 0.008 && title.z <= 0.008 && pref.current_style == 25)
				bNoTitle = true;
			if (!bNoTitle)
			{
				if (grayscale > 0.5)
				{
					//Darken.
					color.x = grayscale * (title.x);
					color.y = grayscale * (title.y);
					color.z = grayscale * (title.z);
				}
				else {
					//Lighting
					color.x = grayscale + title.x + title.x / 3;
					color.y = grayscale + title.y + title.y / 3;
					color.z = grayscale + title.z + title.z / 3;
				}
			}
		}
		else if (iModeColor == 1)
		{

			bool bNoTint = false;
			if (tint.x >= 0.998 && tint.y >= 0.998 && tint.z >= 0.998 && pref.current_style == 25)
				bNoTint = true;

			if(!bNoTint)
			{
				//PE: Dont grayscale "text" we need the yellow/highlight colors.
				if (grayscale > 0.5)
				{
					//Darken.
					color.x = color.x * (tint.x);
					color.y = color.y * (tint.y);
					color.z = color.z * (tint.z);
				}
				else {
					//Lighting
					color.x = color.x + tint.x + tint.x / 3;
					color.y = color.y + tint.y + tint.y / 3;
					color.z = color.z + tint.z + tint.z / 3;
				}
			}
		}
		else
		{

			bool bNoShade = false;
			if (shade.x <= 0.008 && shade.y <= 0.008 && shade.z <= 0.008 && pref.current_style == 25)
				bNoShade = true;

			if (!bNoShade)
			{
				if (bUseGrayScale)
				{
					if (grayscale > 0.5)
					{
						//Darken.
						color.x = grayscale * (shade.x);
						color.y = grayscale * (shade.y);
						color.z = grayscale * (shade.z);
					}
					else {
						//Lighting
						color.x = grayscale + shade.x + shade.x / 3;
						color.y = grayscale + shade.y + shade.y / 3;
						color.z = grayscale + shade.z + shade.z / 3;
					}
				}
				else
				{
					if (grayscale > 0.5)
					{
						//Darken.
						color.x = color.x * (shade.x);
						color.y = color.y * (shade.y);
						color.z = color.z * (shade.z);
					}
					else {
						//Lighting
						color.x = color.x + shade.x + shade.x / 3;
						color.y = color.y + shade.y + shade.y / 3;
						color.z = color.z + shade.z + shade.z / 3;
					}
				}
			}
		}
	}

	if (pref.iTurnOffUITransparent != 0)
		color.w = 1.0f;

}

//Text
//CheckMark
//ResizeGrip
//NavWindow ?
//PlotHistogram

void TintCurrentStyle( void )
{
	auto *style = &ImGui::GetStyle();

	//Light
	TintColor(style->Colors[ImGuiCol_Text],1);
	TintColor(style->Colors[ImGuiCol_CheckMark],1);
	TintColor(style->Colors[ImGuiCol_ResizeGrip],1);
	TintColor(style->Colors[ImGuiCol_PlotHistogram],1);
	TintColor(style->Colors[ImGuiCol_SliderGrab],1);


	//Highlight , title and tabs.
	TintColor(style->Colors[ImGuiCol_TitleBg],2);
	TintColor(style->Colors[ImGuiCol_TitleBgCollapsed], 2);
	TintColor(style->Colors[ImGuiCol_TitleBgActive], 2);
	TintColor(style->Colors[ImGuiCol_Tab], 2);
	TintColor(style->Colors[ImGuiCol_TabHovered], 2);
	TintColor(style->Colors[ImGuiCol_TabActive], 2);
	TintColor(style->Colors[ImGuiCol_TabUnfocused], 2);
	TintColor(style->Colors[ImGuiCol_TabUnfocusedActive], 2);


	//Dark
	TintColor(style->Colors[ImGuiCol_TextDisabled]);
	TintColor(style->Colors[ImGuiCol_WindowBg]);
	TintColor(style->Colors[ImGuiCol_ChildBg]);
	TintColor(style->Colors[ImGuiCol_PopupBg]);
	TintColor(style->Colors[ImGuiCol_Border]);
	TintColor(style->Colors[ImGuiCol_BorderShadow]);
	TintColor(style->Colors[ImGuiCol_FrameBg]);
	TintColor(style->Colors[ImGuiCol_FrameBgHovered]);
	TintColor(style->Colors[ImGuiCol_FrameBgActive]);
	TintColor(style->Colors[ImGuiCol_MenuBarBg]);
	TintColor(style->Colors[ImGuiCol_ScrollbarBg]);
	TintColor(style->Colors[ImGuiCol_ScrollbarGrab]);
	TintColor(style->Colors[ImGuiCol_ScrollbarGrabHovered]);
	TintColor(style->Colors[ImGuiCol_ScrollbarGrabActive]);

	TintColor(style->Colors[ImGuiCol_SliderGrabActive]);
	TintColor(style->Colors[ImGuiCol_Button]);
	TintColor(style->Colors[ImGuiCol_ButtonHovered]);
	TintColor(style->Colors[ImGuiCol_ButtonActive]);
	TintColor(style->Colors[ImGuiCol_Header]);
	TintColor(style->Colors[ImGuiCol_HeaderHovered]);
	TintColor(style->Colors[ImGuiCol_HeaderActive]);
	TintColor(style->Colors[ImGuiCol_Separator]);
	TintColor(style->Colors[ImGuiCol_SeparatorHovered]);
	TintColor(style->Colors[ImGuiCol_SeparatorActive]);
	TintColor(style->Colors[ImGuiCol_ResizeGripHovered]);
	TintColor(style->Colors[ImGuiCol_ResizeGripActive]);
	TintColor(style->Colors[ImGuiCol_PlotLines]);
	TintColor(style->Colors[ImGuiCol_PlotLinesHovered]);
	TintColor(style->Colors[ImGuiCol_PlotHistogramHovered]);
	TintColor(style->Colors[ImGuiCol_TextSelectedBg]);

	//Still need to be set.
	TintColor(style->Colors[ImGuiCol_DockingPreview]);

	// Wicked renders first, IMGUI last, let Wicked renderings through! 
	//style->Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

	TintColor(style->Colors[ImGuiCol_NavHighlight]);
	TintColor(style->Colors[ImGuiCol_NavWindowingHighlight]);
	TintColor(style->Colors[ImGuiCol_NavWindowingDimBg]);
	TintColor(style->Colors[ImGuiCol_ModalWindowDimBg]);
	TintColor(style->Colors[ImGuiCol_DragDropTarget]);

	//PE: To prevent flicker when changing tint, this code has been added to the main loop.
	ImGui::GetStyle().Colors[ImGuiCol_ChildBg] = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
}



ImFont* defaultfont;
ImFont* customfont;
ImFont* customfontlarge;
ImFont* tmpfont;

static const ImWchar Generic_ranges_all[] =
{
	0x0020, 0x00FF, // Basic Latin + Latin Supplement
	0x0100, 0x017F,	//0100 — 017F  	Latin Extended-A
	0x0180, 0x024F,	//0180 — 024F  	Latin Extended-B
	0,
};
//PE: Optimize exclude Phonetic Extensions (1D00 — 1D7F) , phonetic lettering (japan,china), Greak/Latin Extended, block elements,Dingbats,...
static const ImWchar Generic_ranges_everything[] =
{
   0x0020, 0x07FF, // unicode 0x800-0x900 not defined so exclude from here.
   0,
};

void ChangeGGFont(const char *cpcustomfont, int iIDEFontSize)
{
	//MessageBoxA(NULL, "ChangeGGFont", "ChangeGGFont", 0);

	//PE: Add all lang.
//	static const ImWchar Generic_ranges_everything[] =
//	{
//	   0x0020, 0xFFFF, // Everything test.
//	   0,
//	};

	float FONTUPSCALE = 2.0; //Font upscaling.

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->Clear();

	if (FileExist((char*)cpcustomfont))
	{

		customfont = io.Fonts->AddFontFromFileTTF(cpcustomfont, iIDEFontSize*FONTUPSCALE, NULL, &Generic_ranges_everything[0]); //Set as default font.
		customfontlarge = io.Fonts->AddFontFromFileTTF(cpcustomfont, 60, NULL, &Generic_ranges_everything[0]); //Set as default font.

#ifdef IMGUIAL_FONTS_MATERIAL_DESIGN
		int ttf_size;
		const void* ttf_data = ImGuiAl::Fonts::GetCompressedData(ImGuiAl::Fonts::kMaterialDesign, &ttf_size);

		if (ttf_data) {
			static const ImWchar ranges[] = { ICON_MIN_MD, ICON_MAX_MD, 0 };
			ImFontConfig config;
			config.MergeMode = true;
			config.PixelSnapH = true;
			if (pref.bUseUpscaling)
				config.GlyphOffset.y += 7.0f;
			else
				config.GlyphOffset.y += 3.5f;
			customfont = io.Fonts->AddFontFromMemoryCompressedTTF(ttf_data, ttf_size, iIDEFontSize*FONTUPSCALE, &config, ranges);
		}

#endif
#ifdef IMGUIAL_FONTS_FONT_AWESOME
		//Not used yet.
		static const ImWchar ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig config;
		config.MergeMode = true;
		config.PixelSnapH = true;
		config.GlyphOffset.y += 0.5f;
		ttf_data = ImGuiAl::Fonts::GetCompressedData(ImGuiAl::Fonts::kFontAwesome, &ttf_size);
		if (ttf_data)
			io.Fonts->AddFontFromMemoryCompressedTTF(ttf_data, ttf_size, iIDEFontSize*FONTUPSCALE, &config, ranges);
#endif
		io.FontGlobalScale = 1.0f / FONTUPSCALE;
	}
	else
	{
		customfont = io.Fonts->AddFontDefault();
	}
	defaultfont = io.Fonts->AddFontDefault();

	//Add all fonts from:
	extern std::vector< std::pair<ImFont*, std::string>> DefaultStoryboardFonts;
	extern std::vector< std::pair<ImFont*, std::string>> StoryboardFonts;

	StoryboardFonts.clear();
	DefaultStoryboardFonts.clear();

	cstr pOldDir = GetDir();

	for (int a = 0; a < 2; a++)
	{
		char destination[MAX_PATH];
		strcpy(destination, "editors\\templates\\fonts\\");
		if (a == 1)
		{
			//PE: DocWrite folder.
			extern char szWriteDir[MAX_PATH];
			strcpy(destination, szWriteDir);
			strcat(destination, "Files\\editors\\");
			CreateDirectoryA(destination, NULL);
			strcat(destination, "templates\\");
			CreateDirectoryA(destination, NULL);
			strcat(destination, "fonts\\");
			CreateDirectoryA(destination, NULL);
		}
		if (PathExist(destination))
		{
			SetDir(destination);
			ChecklistForFiles();
			SetDir(pOldDir.Get());
			DARKSDK LPSTR ChecklistString(int iIndex);
			DARKSDK int ChecklistQuantity(void);
			for (int c = 1; c <= ChecklistQuantity(); c++)
			{
				char* file = ChecklistString(c);
				if (file)
				{
					if (strlen(file) > 4)
					{
						if (strnicmp(file + strlen(file) - 4, ".ttf", 4) == NULL || strnicmp(file + strlen(file) - 4, ".otf", 4) == NULL)
						{
							const char* pestrcasestr(const char* arg1, const char* arg2);
							bool bAlreadyThere = false;
							for (int i = 0; i < StoryboardFonts.size(); i++)
							{
								if (pestrcasestr(file, StoryboardFonts[i].second.c_str()))
								{
									bAlreadyThere = true;
									break;
								}
							}
							//Add font.
							if (!bAlreadyThere)
							{
								//Add font.
								char path[MAX_PATH];
								strcpy(path, destination);
								strcat(path, file);
								if (pestrcasestr(file, "arial"))
									tmpfont = io.Fonts->AddFontFromFileTTF(path, 60, NULL, &Generic_ranges_everything[0]); //Add font
								else
									tmpfont = io.Fonts->AddFontFromFileTTF(path, 60, NULL, &Generic_ranges_all[0]); //Add font
								StoryboardFonts.push_back(std::make_pair(tmpfont, file));
								if (a == 0)
								{
									DefaultStoryboardFonts.push_back(std::make_pair(tmpfont, file));
								}
							}
						}
					}
				}
			}
		}
	}
	SetDir(pOldDir.Get());


	ImGui_ImplDX11_CreateDeviceObjects();

}

void AddRemoteProjectFonts(void)
{
	void timestampactivity(int i, char* desc_s);
	extern StoryboardStruct Storyboard;
	extern std::vector< std::pair<ImFont*, std::string>> StoryboardFonts;
	bool bAddedFonts = false;
	//PE: Need projectfolder here.
	if (strlen(Storyboard.gamename) > 0 && strlen(Storyboard.customprojectfolder) > 0)
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		cstr pOldDir = GetDir();
		char destination[MAX_PATH];
		strcpy(destination, Storyboard.customprojectfolder);
		strcat(destination, Storyboard.gamename);
		strcat(destination, "\\files\\editors\\templates\\fonts\\");
		if (PathExist(destination))
		{
			SetDir(destination);
			ChecklistForFiles();
			SetDir(pOldDir.Get());
			DARKSDK LPSTR ChecklistString(int iIndex);
			DARKSDK int ChecklistQuantity(void);
			for (int c = 1; c <= ChecklistQuantity(); c++)
			{
				char* file = ChecklistString(c);
				if (file)
				{
					if (strlen(file) > 4)
					{
						const char* pestrcasestr(const char* arg1, const char* arg2);
						if (strnicmp(file + strlen(file) - 4, ".ttf", 4) == NULL || strnicmp(file + strlen(file) - 4, ".otf", 4) == NULL)
						{
							bool bAlreadyThere = false;
							for (int i = 0; i < StoryboardFonts.size(); i++)
							{
								if (pestrcasestr(file, StoryboardFonts[i].second.c_str()))
								{
									bAlreadyThere = true;
									break;
								}
							}
							//Add font.
							if (!bAlreadyThere)
							{
								char msg[MAX_PATH];
								sprintf(msg, "Adding Font: %s", file);
								timestampactivity(0, msg);

								char path[MAX_PATH];
								strcpy(path, destination);
								strcat(path, file);
								if (pestrcasestr(file, "arial"))
									tmpfont = io.Fonts->AddFontFromFileTTF(path, 60, NULL, &Generic_ranges_everything[0]); //Add font
								else
									tmpfont = io.Fonts->AddFontFromFileTTF(path, 60, NULL, &Generic_ranges_all[0]); //Add font
								StoryboardFonts.push_back(std::make_pair(tmpfont, file));
								bAddedFonts = true;
							}
						}
					}
				}
			}
			if (bAddedFonts)
			{
				//ImGui_ImplDX11_CreateDeviceObjects();
				timestampactivity(0, "ImGui_ImplDX11_CreateFontsTexture();");
				ImGui_ImplDX11_CreateFontsTexture();
				//PE: old frame could have our old font texture , so disable it until newframe.
				bBlockImGuiUntilNewFrame = true;
			}
		}
		SetDir(pOldDir.Get());
	}
}

//struct case_insensitive_less //: public std::binary_function< char, char, bool >
//{
//	bool operator () (char x, char y) const
//	{
//		return toupper(static_cast< unsigned char >(x)) <
//			toupper(static_cast< unsigned char >(y));
//	}
//};

bool CaseInsensitiveLess(char x, char y)
{
	return toupper(static_cast<unsigned char>(x)) <
		toupper(static_cast<unsigned char>(y));
}

bool NoCaseLess(const std::string &a, const std::string &b)
{
	/*return std::lexicographical_compare(a.begin(), a.end(),
		b.begin(), b.end(), case_insensitive_less());*/
	return std::lexicographical_compare(a.begin(), a.end(),
		b.begin(), b.end(), CaseInsensitiveLess);
}


/* done in cStr
void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}
*/

//
const char *pestrcasestr(const char *arg1, const char *arg2);

//##########################################################
//#### Recursive get all files/folders inside a folder. ####
//##########################################################

#include "cStr.h"
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


//#########################################
//#### PE: Sort for files and folders. ####
//#########################################

int cstring_cmp_folder(const void *a, const void *b)
{
	struct cFolderItem * ia = *(struct cFolderItem **) a;
	struct cFolderItem * ib = *(struct cFolderItem **) b;
	if (!ia) return(0);
	if (!ib) return(0);
	return _stricmp(ia->cfolder, ib->cfolder);
}

cFolderItem MainEntityList;
int iTotalFolders = 0;
int iTotalFiles = 0;
#include "cStr.h"

// seemingly not used!
//void Clear_MainEntityList(void)
//{
//	MainEntityList.m_pNext = NULL;
//	extern bool bExternal_Entities_Init;
//	bExternal_Entities_Init = false;
//}


//std::vector< std::pair<std::string, time_t> > files_time_stamp;
std::unordered_map<std::string, time_t> files_time_stamp;


std::vector<std::string> files_favorite;
std::vector<std::string> files_availableinfreetrial;
std::vector<std::string> files_pinned_categories;

std::string GetLineFromVectorFile(char *str, std::vector<std::string> & vecOfStrs, bool bIgnoreSpaces)
{
	for (int i = 0; i < vecOfStrs.size(); i++)
	{
		std::string entry = vecOfStrs[i];
		if (entry.length() > 0)
		{
			if (bIgnoreSpaces)
			{
				replaceAll(entry, " ", "");
				replaceAll(entry, "\t", "");
			}
			if (pestrcasestr(entry.c_str(), str))
			{
				return(vecOfStrs[i]);
			}
		}
	}
	return "";
}

std::string GetLineParameterFromVectorFile(char *str, std::vector<std::string> & vecOfStrs, bool bIgnoreSpaces)
{
	std::string Line = GetLineFromVectorFile(str, vecOfStrs, bIgnoreSpaces);
	if (Line.length() > 0)
	{
		//replaceAll(Line, " ", "");
		//replaceAll(Line, "\t", "");
		char *find = (char * ) pestrcasestr(Line.c_str(), "=");
		if (find)
		{
			find++;
			while (*find == ' ' || *find == '\t' && *find != 0)
				find++;
			int len = strlen(find);
			if (len > 1)
			{
				int i = len-1;
				while (find[i] == ' ' || find[i] == '\t' && find[i]!= 0)
				{
					find[i]= 0;
					i--;
				}
			}
			return(std::string(find));
		}
	}
	return "";
}

void RemoveStrStrFromVectorFile(char *str, std::vector<std::string> & vecOfStrs, bool bIgnoreSpaces)
{
	for (int i = 0; i < vecOfStrs.size(); i++)
	{
		std::string entry = vecOfStrs[i];
		if (entry.length() > 0)
		{
			if (bIgnoreSpaces)
			{
				replaceAll(entry, " ", "");
				replaceAll(entry, "\t", "");
			}
			if (pestrcasestr(entry.c_str(), str))
			{
				auto itr = std::find(vecOfStrs.begin(), vecOfStrs.end(), vecOfStrs[i]);
				if (itr != vecOfStrs.end())
				{
					vecOfStrs.erase(itr);
					RemoveStrStrFromVectorFile(str, vecOfStrs, bIgnoreSpaces);
					return;
				}
			}
		}
	}
}


bool getVectorFileContent(std::string fileName, std::vector<std::string> & vecOfStrs, bool bAllowEmpty)
{
	char szRealFilename[MAX_PATH];
	strcpy_s(szRealFilename, MAX_PATH, fileName.c_str());
	GG_GetRealPath(szRealFilename, 0);

	// Open the File
	std::ifstream in(szRealFilename);
	// Check if object is valid
	if (!in)
	{
		return false;
	}
	std::string str;
	while (std::getline(in, str))
	{
		if (str.size() > 0 || bAllowEmpty)
			vecOfStrs.push_back(str);
	}
	in.close();
	return true;
}

bool saveVectorFileContent(std::string fileName, std::vector<std::string> & vecOfStrs)
{
	char szRealFilename[MAX_PATH];
	strcpy_s(szRealFilename, MAX_PATH, fileName.c_str());
	GG_GetRealPath(szRealFilename, 1);

	std::ofstream output_file(szRealFilename);
	std::ostream_iterator<std::string> output_iterator(output_file, "\n");
	std::copy(vecOfStrs.begin(), vecOfStrs.end(), output_iterator);
	return true;
}


//Thread to collect fpe informations.
std::vector<cFolderItem::sFolderFiles *> g_ScanFpeFiles;
bool g_bFpeScanning = false;
int g_iScannedFiles = 0;
std::thread* g_pFPEScan = NULL;
void fpe_thread_function(void)
{
	g_bFpeScanning = true;

	for (int n = 0; n < g_ScanFpeFiles.size(); n++)
	{
		g_iScannedFiles++;
		cFolderItem::sFolderFiles * item = g_ScanFpeFiles[n];
		if (item)
		{
			char fpe_file[MAX_PATH];
			strcpy(fpe_file, item->m_sPath.Get());
			strcat(fpe_file, "\\");
			strcat(fpe_file, item->m_sName.Get());
			//HANDLE hfile = GG_CreateFile(fpe_file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			//We already have the full path so.
			//PE: getVectorFileContent already check if it exists.
			//HANDLE hfile = CreateFileA(fpe_file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			//if (hfile != INVALID_HANDLE_VALUE)
			//{
			//	CloseHandle(hfile);

			{
				std::vector<std::string> fpe_vector;
				if (getVectorFileContent(fpe_file, fpe_vector, true))
				{
					bool ischaracter = false;

					std::string s_isgroupobject = GetLineParameterFromVectorFile("isgroupobject=", fpe_vector, true);
					item->m_bIsGroupObject = 0;
					if (s_isgroupobject.length() > 0 && atoi(s_isgroupobject.c_str()) == 1)
						item->m_bIsGroupObject = 1;

					//Get info.
					#ifdef INCLUDEPOLYGONSORT
					std::string model = GetLineParameterFromVectorFile("model=", fpe_vector, true);
					if (model.length() > 0)
						item->m_sFPEModel = model.c_str();
					else
					#endif
						item->m_sFPEModel = "";

					//PE: Currently not used in advanced fpe search.
					//std::string textured = GetLineParameterFromVectorFile("textured=", fpe_vector, true); //Not used yet.
					//if (textured.length() > 0)
					//	item->m_sFPETextured = textured.c_str();
					//else
						item->m_sFPETextured = "";

					//Ignore there , and dont display thumbs, dont work anyway.
					#ifdef INCLUDEPOLYGONSORT
					std::string s_ischaractercreator = GetLineParameterFromVectorFile("charactercreator=", fpe_vector, true);
					#endif
					item->m_bIsCharacterCreator = 0;
					#ifdef INCLUDEPOLYGONSORT
					if (s_ischaractercreator.length() > 0 && atoi(s_ischaractercreator.c_str()) == 1)
						item->m_bIsCharacterCreator = 1;
					#endif

					#ifdef INCLUDEPOLYGONSORT
					std::string s_ischaracter = GetLineParameterFromVectorFile("ischaracter=", fpe_vector, true);
					ischaracter = false;
					if (s_ischaracter.length() > 0 && atoi(s_ischaracter.c_str()) == 1)
						ischaracter = true;

					bool isAnimated = false;
					std::string s_animmax = GetLineParameterFromVectorFile("animmax=", fpe_vector, true);
					if (s_animmax.length() > 0 && atoi(s_animmax.c_str()) > 0)
						isAnimated = true;
					#endif

					std::string keywords = GetLineParameterFromVectorFile("keywords=", fpe_vector, true); //Not used yet.
					if (keywords.length() > 0)
						item->m_sFPEKeywords = keywords.c_str();
					else
						item->m_sFPEKeywords = "";

					item->m_sFPEDBOFile = "";
					item->m_iFPEDBOFileSize = 0;

					#ifdef INCLUDEPOLYGONSORT
					//PE: This step is not needed when we dont have polygon sort.
					if (model.length() > 0)
					{
						//Get DBO info.
						char sEntityBank[MAX_PATH];
						strcpy(sEntityBank, "entitybank\\");

						char * find = (char *) pestrcasestr(fpe_file, sEntityBank);
						if (find)
						{
							find += strlen(sEntityBank);
							char * find2 = (char *)pestrcasestr(find, "ebebank\\");
							if (find2) strcpy(sEntityBank,"");
							
							char epath[MAX_PATH];
							strcpy(epath, find);
							bool bNotFound = true;
							for (int i = strlen(epath); i > 1; i--)
							{
								if (epath[i-1] == '\\' || epath[i-1] == '/')
								{
									bNotFound = false;
									epath[i] = 0;
									break;
								}
							}
							if(bNotFound) strcpy(epath, "");

							char sFile[MAX_PATH];
							strcpy(sFile, "");
							if (item->m_bIsCharacterCreator == 0)
							{
								strcpy(sFile, sEntityBank);
								strcat(sFile, epath);
								strcat(sFile, item->m_sFPEModel.Get());
							}
							else
							{
								strcpy(sFile, item->m_sFPEModel.Get());
							}

							//PE: Todo remove spaces at end sFile="entitybank\Purchased\Oldpman\ARTIST PACK\Scenery\block66.x                         "
							//Make sure we use full path to files.

							int iSrcFormat = 0;
							if (pestrcasestr(sFile,".x")) iSrcFormat = 1;
							if (pestrcasestr(sFile, ".fbx")) iSrcFormat = 2;
							if (iSrcFormat > 0)
							{
								char sDboFile[MAX_PATH];
								if (iSrcFormat == 1)
								{
									// X File Format
									strcpy(sDboFile, sFile);
									sDboFile[strlen(sDboFile) - 2] = 0;
									strcat(sDboFile, ".dbo");
									GG_GetRealPath(sDboFile, false);
									if (FileExist(sDboFile) == 1) strcpy(sFile,sDboFile);
								}
								if (iSrcFormat == 2)
								{
									// FBX File Format
									strcpy(sDboFile, sFile);
									sDboFile[strlen(sDboFile) - 4] = 0;
									strcat(sDboFile, ".dbo");
									GG_GetRealPath(sDboFile, false);
									if (FileExist(sDboFile) == 1) strcpy(sFile,sDboFile);
								}
							}

							GG_GetRealPath(sFile, false);
							HANDLE hfile = GG_CreateFile(sFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
							if (hfile != INVALID_HANDLE_VALUE)
							{
								item->m_sFPEDBOFile = sFile;
								item->m_iFPEDBOFileSize = GetFileSize(hfile, NULL);
								#ifdef INCLUDEPOLYGONSORT
								if (ischaracter)
								{
									if (item->m_iFPEDBOFileSize > 14000000) // zoombies are really large.
										item->m_iFPEDBOFileSize = 4500*9+ g_iScannedFiles;
									else if (item->m_iFPEDBOFileSize > 7000000) // uber soldier is large.
										item->m_iFPEDBOFileSize = 17400 * 9 + g_iScannedFiles;
									else if (item->m_iFPEDBOFileSize > 4000000) // Shotgun soldier.dbo
										item->m_iFPEDBOFileSize = 14500 * 9 + g_iScannedFiles;
									else if (item->m_iFPEDBOFileSize > 3000000) // masked soldier.dbo
										item->m_iFPEDBOFileSize = 17400 * 9 + g_iScannedFiles;
									else if (item->m_iFPEDBOFileSize > 1800000) // charactercreatorplus
										item->m_iFPEDBOFileSize = 20000 * 9 + g_iScannedFiles;
									else if (item->m_iFPEDBOFileSize > 900000) // wizard
										item->m_iFPEDBOFileSize = 10000 * 9 + g_iScannedFiles;
									else //Chars have a large amount of animations, so... This is REALLY not precise.
										item->m_iFPEDBOFileSize = item->m_iFPEDBOFileSize & 0x1ffff;

								}
								else
								{
									if (isAnimated)
									{
										//Cant trust file when animated. This is REALLY not precise.
										item->m_iFPEDBOFileSize = item->m_iFPEDBOFileSize & 0x1ffff;
									}
								}
								#endif

								CloseHandle(hfile);
							}
						}
					}
					#endif

					item->m_bFPELoaded = true; //Mark that its now safe to use all FPE settings from other threads.
				}

				fpe_vector.clear();
			}
		}
	}
	g_ScanFpeFiles.clear();
	g_bFpeScanning = false;
}

void fpe_thread_start(void)
{
//	g_pFPEScan = new std::thread(fpe_thread_function, std::ref(g_object_list));
	g_pFPEScan = new std::thread(fpe_thread_function);
}
bool fpe_thread_in_progress(void)
{
	return g_bFpeScanning;
}


//std::vector<std::string> duplicate_files_check;
//PE: Converted to unordered_set using hash for faster lookups.
std::unordered_set<std::string> duplicate_files_check;

void CustomSortFiles(int iSortBy, cFolderItem::sFolderFiles * m_pFileSortStart)
{
	if (!m_pFileSortStart) return;
	std::vector<std::string> sorted_files;

	cFolderItem::sFolderFiles * m_pFiles = m_pFileSortStart;
	//PE: Make a list here thats sorted by file time.
	m_pFiles->bSorted = false;
	if (m_pFiles) m_pFiles = m_pFiles->m_pNext; //PE: Skip "..." entry.

	cFolderItem::sFolderFiles * m_pFileSort = m_pFiles;
	cFolderItem::sFolderFiles * m_pScanSort = m_pFiles;
	while (m_pFileSort)
	{
		m_pFileSort->bSorted = false;
		if (iSortBy == 1)
		{
			cstr sort_s = m_pFileSort->m_sNameFinal;
			sorted_files.push_back(sort_s.Get());
		}
		m_pFileSort = m_pFileSort->m_pNext;
	}

	m_pFileSort = m_pFileSortStart;
	cFolderItem::sFolderFiles * m_pSortBest = NULL;
	cFolderItem::sFolderFiles * m_pPrevBest = NULL;

	while (m_pFileSort)
	{
		bool bFoundBest = false;
		time_t m_tFileModifySort = 0;
		m_pScanSort = m_pFiles;
		while (m_pScanSort)
		{
			if (!m_pScanSort->bSorted)
			{
				if (iSortBy == 0)
				{
					if (m_pScanSort->m_tFileModify >= m_tFileModifySort)
					{
						m_tFileModifySort = m_pScanSort->m_tFileModify;
						m_pSortBest = m_pScanSort;
						bFoundBest = true;
					}
				}
				else
				{
					//Default just accept all.
					m_pSortBest = m_pScanSort;
					bFoundBest = true;
				}
			}
			m_pScanSort = m_pScanSort->m_pNext;
		}

		if (bFoundBest) {
			if (iSortBy == 0)
			{
				if (m_pPrevBest)
				{
					m_pPrevBest->m_pNextTime = m_pSortBest;
				}
				else
				{
					m_pFileSort->m_pNextTime = m_pSortBest;
				}
				m_pPrevBest = m_pSortBest;
				m_pSortBest->m_pNextTime = NULL;
				//printf("%s", m_pSortBest->m_sName);
				m_pSortBest->bSorted = true;
			}
			else
			{
				if (m_pPrevBest)
				{
					m_pPrevBest->m_pCustomSort = m_pSortBest;
				}
				else
				{
					m_pFileSort->m_pCustomSort = m_pSortBest;
				}
				m_pPrevBest = m_pSortBest;
				m_pSortBest->m_pCustomSort = NULL;
				//printf("%s", m_pSortBest->m_sName);
				m_pSortBest->bSorted = true;

				//Default just add.
			}
		}
		else {
			if (iSortBy == 0)
			{
				if (m_pPrevBest)
					m_pPrevBest->m_pNextTime = NULL;
			}
			else
			{
				if (m_pPrevBest)
					m_pPrevBest->m_pCustomSort = NULL;
			}
		}

		m_pFileSort = m_pFileSort->m_pNext;
	}

	sorted_files.clear();
	//PE: DEBUG ONLY
	/*
	if (m_pSortBest->m_pNextTime != NULL)
	{
		printf("err:");
	}
	m_pFileSort = pNewFolder->m_pNext->m_pFirstFile;; //TEST
	while (m_pFileSort)
	{
		printf("%s", m_pFileSort->m_sName);
		m_pFileSort = m_pFileSort->m_pNextTime;
	}
	*/

}

void SetAvailableInFreeTrial(int foldertype, cFolderItem::sFolderFiles* pNewItem, cstr file)
{
	if (foldertype == 0)
	{
		// exclude all objects except those in files_availableinfreetrial list
		std::vector<std::string>::iterator itf = files_availableinfreetrial.begin();
		for (; itf != files_availableinfreetrial.end(); ++itf)
		{
			if (itf->size() > 0)
			{
				if (strnicmp(itf->c_str(), file.Get(), file.Len()) == 0)
				{
					pNewItem->bAvailableInFreeTrial = true;
					break;
				}
			}
		}
	}
	else
	{
		if (foldertype == 4)
		{
			// behavior exclusions
			LPSTR pExclude = "";
			bool bBlockThisOne = false;
			LPSTR pThisFile = file.Get();
			pExclude = "jumpscare.lua"; if (strnicmp(pThisFile + strlen(pThisFile) - strlen(pExclude), pExclude, strlen(pExclude)) == NULL) bBlockThisOne = true;
			pExclude = "bounce.lua"; if (strnicmp(pThisFile + strlen(pThisFile) - strlen(pExclude), pExclude, strlen(pExclude)) == NULL) bBlockThisOne = true;
			pExclude = "freezeplayer.lua"; if (strnicmp(pThisFile + strlen(pThisFile) - strlen(pExclude), pExclude, strlen(pExclude)) == NULL) bBlockThisOne = true;
			pExclude = "heal.lua"; if (strnicmp(pThisFile + strlen(pThisFile) - strlen(pExclude), pExclude, strlen(pExclude)) == NULL) bBlockThisOne = true;
			pExclude = "hurt.lua"; if (strnicmp(pThisFile + strlen(pThisFile) - strlen(pExclude), pExclude, strlen(pExclude)) == NULL) bBlockThisOne = true;
			pExclude = "slip.lua"; if (strnicmp(pThisFile + strlen(pThisFile) - strlen(pExclude), pExclude, strlen(pExclude)) == NULL) bBlockThisOne = true;
			pExclude = "teleport.lua"; if (strnicmp(pThisFile + strlen(pThisFile) - strlen(pExclude), pExclude, strlen(pExclude)) == NULL) bBlockThisOne = true;
			pExclude = "collection count.lua"; if (strnicmp(pThisFile + strlen(pThisFile) - strlen(pExclude), pExclude, strlen(pExclude)) == NULL) bBlockThisOne = true;
			pExclude = "dynamite.lua"; if (strnicmp(pThisFile + strlen(pThisFile) - strlen(pExclude), pExclude, strlen(pExclude)) == NULL) bBlockThisOne = true;
			pExclude = "seecam.lua"; if (strnicmp(pThisFile + strlen(pThisFile) - strlen(pExclude), pExclude, strlen(pExclude)) == NULL) bBlockThisOne = true;
			pExclude = "spiketrap.lua"; if (strnicmp(pThisFile + strlen(pThisFile) - strlen(pExclude), pExclude, strlen(pExclude)) == NULL) bBlockThisOne = true;
			pExclude = "trapdoor.lua"; if (strnicmp(pThisFile + strlen(pThisFile) - strlen(pExclude), pExclude, strlen(pExclude)) == NULL) bBlockThisOne = true;
			if (bBlockThisOne == true)
			{
				pNewItem->bAvailableInFreeTrial = false;
			}
			else
			{
				pNewItem->bAvailableInFreeTrial = true;
			}
		}
		else
		{
			// allow all
			pNewItem->bAvailableInFreeTrial = true;
		}
	}
}

cstr GetNameFinalCreditFromAbsPath (LPSTR pAbsFullFolderPath)
{
	cstr sNameFinalCredit = "";
	#ifndef GGMAXEDU
	LPSTR pFoundMatchPtr = strstr(pAbsFullFolderPath, "\\Community\\");
	if (pFoundMatchPtr == NULL) pFoundMatchPtr = strstr(pAbsFullFolderPath, "\\community\\");
	if (pFoundMatchPtr != NULL)
	{
		// we have a community asset here, credit it!
		char pAccountID[256];
		strcpy(pAccountID, pFoundMatchPtr + strlen("\\community\\"));
		for (int n = 0; n < strlen(pAccountID); n++)
		{
			if (pAccountID[n] == '\\' || pAccountID[n] == '/')
			{
				pAccountID[n] = 0;
				break;
			}
		}
		if (strlen(pAccountID) > 0)
		{
			extern std::vector<sWorkshopSteamUserName> g_workshopSteamUserNames;
			for (int j = 0; j < g_workshopSteamUserNames.size(); j++)
			{
				if (stricmp(g_workshopSteamUserNames[j].sSteamUserAccountID.Get(), pAccountID) == NULL)
				{
					strcpy(pAccountID, g_workshopSteamUserNames[j].sSteamUsersPersonaName.Get());
					break;
				}
			}
			sNameFinalCredit = cstr(" (by ") + cstr(pAccountID) + cstr(")");
		}
	}
	#endif
	return sNameFinalCredit;
}

extern char szBeforeChangeWriteDir[MAX_PATH];

void GetMainEntityList(char* folder_s, char* rel_s, void *pFolder, char* folder_name_start, bool bForceToTop, int foldertype)
{
	//foldertype: 0 fpe , 1 waw mp3 ogg. , 2 BMP, JPG, PNG, TIF, and GIF
	// get legacy white list once
	if (g_bCreateLegacyWhiteList == true)
	{
		getVectorFileContent("favoritelist.ini", files_favorite);
		extern bool g_bFreeTrialVersion;
		if (g_bFreeTrialVersion == true)
		{
			getVectorFileContent("freetriallist.ini", files_availableinfreetrial);
		}

		g_pLegacyWhiteList.clear();
		FILE* fp = GG_fopen(".\\..\\legacyblacklist.ini", "rt");
		if (fp)
		{
			char c;
			fread(&c, sizeof(char), 1, fp);
			while (!feof(fp))
			{
				// get string from file
				char szEntNameFromFile[MAX_PATH] = "";
				int iOffset = 0;
				while (!feof(fp) && c != 13 && c != 10)
				{
					szEntNameFromFile[iOffset++] = c;
					fread(&c, sizeof(char), 1, fp);
				}
				szEntNameFromFile[iOffset] = 0;

				// skip beyond CR
				while (!feof(fp) && (c == 13 || c == 10))
					fread(&c, sizeof(char), 1, fp);

				// write into array
				LPSTR pWhiteListItem = new char[512];
				strlwr(szEntNameFromFile);
				strcpy(pWhiteListItem, szEntNameFromFile);
				g_pLegacyWhiteList.push_back(pWhiteListItem);
			}
			fclose(fp);
		}
		g_bCreateLegacyWhiteList = false;
	}

	// get full entity list
	int tt = 0;
	cstr file_s = "";
	int fin = 0;
	cstr tempcstr;
	cFolderItem *pNewFolder = (cFolderItem *) pFolder;
	if (pNewFolder == NULL)
	{
		pNewFolder = &MainEntityList;
		iTotalFolders = 0;
		iTotalFiles = 0;
	}
	while (pNewFolder->m_pNext) 
	{
		pNewFolder = pNewFolder->m_pNext;
	}

	if (PathExist(folder_s) == 1)
	{
		SetDir(folder_s);

		//Create FolderItem.
		cFolderItem *pNewItem;
		pNewItem = new cFolderItem();
		pNewItem->m_sFolder = folder_s;

		cstr pOld = GetDir();
		pNewItem->m_sFolderFullPath = pOld;
		pNewItem->iType = foldertype;

		//PE: Added here for speed.
		pNewItem->m_iEntityOffset = pNewItem->m_sFolderFullPath.Len();
		cStr path = pNewItem->m_sFolderFullPath.Get();
		LPSTR pPathSearch = path.Get();
		LPSTR pFind = "\\entitybank";
		if(foldertype == 1)
			pFind = "\\audiobank";
		if (foldertype == 2)
			pFind = "\\imagebank";
		if (foldertype == 3)
			pFind = "\\videobank";
		if (foldertype == 4)
			pFind = "\\scriptbank";
		if (foldertype == 5)
			pFind = "\\particlesbank";
		if (foldertype == 6)
			pFind = "\\charactercreatorplus\\animations";
		
		for (int n = 0; n < strlen(pPathSearch); n++)
		{
			if (strnicmp(pPathSearch + n, pFind, strlen(pFind)) == NULL)
			{
				pNewItem->m_iEntityOffset = n + strlen(pFind);
				break;
			}

			// project folder passed in override specific detection above
		}

		strcpy(pNewItem->cfolder, pNewItem->m_sFolderFullPath.Get() );

		// ensures writable folder sorts all its folders to top
		if ( bForceToTop == true )
		{
			// ensure when sorted, these folders stay at the very top for easy discovery
			pNewItem->cfolder[0] = 'A';
		}

		pNewItem->m_pSubFolder = NULL;
		pNewItem->visible = true;
		pNewItem->deletethisentry = false;
		pNewItem->count = ++iTotalFolders;
		pNewItem->m_pFirstFile = NULL;
		pNewItem->bIsCustomFolder = false;
		pNewItem->m_pNext = NULL;

		//Update last folder modify date time.
		struct stat sb;
		if (stat(pNewItem->m_sFolderFullPath.Get(), &sb) == 0) 
		{
			if (sb.st_mtime != pNewItem->m_tFolderModify) 
			{
				pNewItem->m_tFolderModify = sb.st_mtime;
			}
		}	
		// taken care of above with bForceToTop
		pNewFolder->m_pNext = pNewItem;

		FindFirst(); fin = 0;

		std::vector<std::string> sorted_files;
		while (GetFileType()>-1)
		{
			file_s = GetFileName();
			if (file_s == "." || file_s == "..")
			{
				//  ignore . and ..
			}
			else
			{
				if (GetFileType() == 1)
				{
					//  folder
					GetMainEntityList(file_s.Get(), cstr(cstr(rel_s) + file_s + "\\").Get(), (void *)pNewFolder->m_pNext, "", bForceToTop, foldertype);
					FindFirst();
					if (fin > 0)
					{
						for (tt = 1; tt <= fin; tt++)
						{
							if (GetFileType()>-1)
							{
								FindNext();
							}
						}
					}
				}
				else
				{
					bool bValid = false;
					if (foldertype == 0 && pestrcasestr(file_s.Get(), ".fpe"))
						bValid = true;
					if (foldertype == 1 && pestrcasestr(file_s.Get(), ".wav"))
						bValid = true;
					if (foldertype == 1 && pestrcasestr(file_s.Get(), ".mp3"))
						bValid = true;
					if (foldertype == 1 && pestrcasestr(file_s.Get(), ".ogg"))
						bValid = true;

					if (foldertype == 2 && pestrcasestr(file_s.Get(), ".png"))
						bValid = true;
					if (foldertype == 2 && pestrcasestr(file_s.Get(), ".dds"))
						bValid = true;
					if (foldertype == 2 && pestrcasestr(file_s.Get(), ".bmp"))
						bValid = true;
					if (foldertype == 2 && pestrcasestr(file_s.Get(), ".tif"))
						bValid = true;
					if (foldertype == 2 && pestrcasestr(file_s.Get(), ".jpg"))
						bValid = true;
					if (foldertype == 2 && pestrcasestr(file_s.Get(), ".gif"))
						bValid = true;

					if (foldertype == 3 && pestrcasestr(file_s.Get(), ".wmv"))
						bValid = true;
					//if (foldertype == 3 && pestrcasestr(file_s.Get(), ".ogv"))
					//	bValid = true;
					if (foldertype == 3 && pestrcasestr(file_s.Get(), ".mp4"))
						bValid = true;

					if (foldertype == 4 && pestrcasestr(file_s.Get(), ".lua"))
						bValid = true;

					if (foldertype == 5 && (pestrcasestr(file_s.Get(), ".arx") || pestrcasestr(file_s.Get(), ".pe")))
						bValid = true;

					if (foldertype == 6 && pestrcasestr(file_s.Get(), ".fpe"))
						bValid = true;

					// file
					if(bValid)
					{
						// compare entity with legacy whitelist, and skip if on list (slowly hiding older models)
						cstr pComboRelandFile = cstr(rel_s) + file_s;

						//if (strnicmp(rel_s, "user\\lee", 8) == NULL)
						//{
						//	pComboRelandFile = cstr(rel_s) + file_s;
						//}

						LPSTR pFilePtr = pComboRelandFile.Get();
						int iLegacyWhiteListCount = g_pLegacyWhiteList.size();
						int n = 0;
						for (; n < iLegacyWhiteListCount; n++)
						{
							if (stricmp(pFilePtr, g_pLegacyWhiteList[n]) == NULL)
								break;
						}
						if (n == iLegacyWhiteListCount)
						{
							// not found in legacy whitelist, allow
							sorted_files.push_back(file_s.Get());
							
							//SAVE: time_create
							time_t ts = GetFileDateLong();
							cstr file = pNewFolder->m_pNext->m_sFolderFullPath;
							file = file + "\\" + file_s;
							//files_time_stamp.push_back( std::make_pair(file.Get(), ts ) );
							files_time_stamp[file.Get()] = ts;
						}
					}
				}
			}
			FindNext();
			fin = fin + 1;
		}

		//sorted_files
		cFolderItem::sFolderFiles * m_pFiles = NULL;
		if (!sorted_files.empty()) 
		{
			std::sort(sorted_files.begin(), sorted_files.end(), NoCaseLess);
			std::vector<std::string>::iterator it = sorted_files.begin();
			if (it->size() > 0) 
			{
				cFolderItem::sFolderFiles *pNewItem = new cFolderItem::sFolderFiles;
				pNewItem->m_sName = "...";
				pNewItem->m_sNameFinal = pNewItem->m_sName;
				pNewItem->m_sNameFinalCredit = "";
				pNewItem->m_tFileModify = 0;
				pNewItem->m_Backdrop = "";
				pNewItem->bFavorite = false;
				pNewItem->bAvailableInFreeTrial = false;
				pNewItem->m_sPath = "";
				pNewItem->m_sFolder = "[na]";
				pNewItem->iFlags = 0;
				pNewItem->iPreview = 0;
				pNewItem->iBigPreview = 0;
				pNewItem->id = iTotalFiles++;
				pNewItem->bPreviewProcessed = false;
				pNewItem->m_pNext = NULL;
				pNewItem->m_pNextTime = NULL;
				pNewItem->m_pCustomSort = NULL;

				pNewItem->m_bFPELoaded = false;
				pNewItem->m_sFPEModel="";
				pNewItem->m_sFPETextured = "";
				pNewItem->m_sFPEKeywords = "";
				pNewItem->m_sFPEDBOFile = "";
				pNewItem->m_sDLuaDescription = "##na##";
				pNewItem->m_fDLuaHeight = 0.0f;
				pNewItem->m_iFPEDBOFileSize = 0;
				pNewItem->m_bIsCharacterCreator = 0;
				pNewItem->iType = foldertype;

				pNewFolder->m_pNext->m_pFirstFile = pNewItem;
				m_pFiles = pNewItem;
			}
			for (; it != sorted_files.end(); ++it) 
			{
				bool bAddToList = true;
				//PE: Eliminate any duplicates here, DocWrite and Normal Path, Prefer DocWrite.
				if (it->size() > 0) {
					cStr sName = (char *) it->c_str();
					cStr m_sPath = pNewFolder->m_pNext->m_sFolderFullPath;
					cStr sCheck = m_sPath + cstr("\\") + sName;
					char *find = NULL;
					if(foldertype == 0)
						find = (char *) pestrcasestr(sCheck.Get(),"entitybank\\");
					if (foldertype == 1)
						find = (char *)pestrcasestr(sCheck.Get(), "audiobank\\");
					if (foldertype == 2)
						find = (char *)pestrcasestr(sCheck.Get(), "imagebank\\");
					if (foldertype == 3)
						find = (char *)pestrcasestr(sCheck.Get(), "videobank\\");
					if (foldertype == 4)
						find = (char *)pestrcasestr(sCheck.Get(), "scriptbank\\");
					if (foldertype == 5)
						find = (char *)pestrcasestr(sCheck.Get(), "particlesbank\\");
					if (foldertype == 6)
						find = (char *)pestrcasestr(sCheck.Get(), "charactercreatorplus\\animations\\");
					if (find)
					{
						// ZJ: Got heap corruption error here.
						//sCheck = find;
						cStr sFind(find);
						strcpy(sCheck.Get(), sFind.Get());
						//sCheck = sCheck + cstr("\\") + sName;
					}
					sCheck = sCheck.Lower();
					//auto itr = std::find(duplicate_files_check.begin(), duplicate_files_check.end(), sCheck.Get());
					//if (itr != duplicate_files_check.end() && duplicate_files_check.size() > 0 )
					//	bAddToList = false;
					//else
					//	duplicate_files_check.push_back(sCheck.Get());
					
					if (duplicate_files_check.count(sCheck.Get()) > 0)
						bAddToList = false;
					else
						duplicate_files_check.insert(sCheck.Get());
				}
				if (it->size() > 0 && bAddToList) 
				{
					cFolderItem::sFolderFiles *pNewItem;
					pNewItem = new cFolderItem::sFolderFiles;

					pNewItem->m_sName = it->c_str();
					if ((foldertype == 0 || foldertype == 6) && pNewItem->m_sName.Len() > 4)
						pNewItem->m_sNameFinal = pNewItem->m_sName.Left(pNewItem->m_sName.Len() - 5);
					else
						pNewItem->m_sNameFinal = pNewItem->m_sName;

					// credit if community asset
					pNewItem->m_sNameFinalCredit = GetNameFinalCreditFromAbsPath (pNewFolder->m_pNext->m_sFolderFullPath.Get());

					//Generate a better search string. include category at end.
					std::string sBetterSearch = pNewItem->m_sNameFinal.Get();
					sBetterSearch = sBetterSearch + " ( " + pNewFolder->m_pNext->m_sFolder.Get() + " )";
					//Remove main folder for better search.
					if (foldertype == 1)
						replaceAll(sBetterSearch, "audiobank", "");
					if (foldertype == 2)
						replaceAll(sBetterSearch, "imagebank", "");
					if (foldertype == 3)
						replaceAll(sBetterSearch, "videobank", "");
					if (foldertype == 4)
						replaceAll(sBetterSearch, "scriptbank", "");
					if (foldertype == 5)
						replaceAll(sBetterSearch, "particlesbank", "");
					if (foldertype == 6)
						replaceAll(sBetterSearch, "charactercreatorplus\\animations", "");
					replaceAll(sBetterSearch, "_", " ");
					replaceAll(sBetterSearch, "-", " ");
					pNewItem->m_sBetterSearch = sBetterSearch.c_str();

					pNewItem->m_tFileModify = 0; //PE: Need timestamp here.
					pNewItem->bFavorite = false;
					pNewItem->bAvailableInFreeTrial = false;

					//PE: This was to slow.
					struct stat sb;
					cstr file = pNewFolder->m_pNext->m_sFolderFullPath;
					file = file + "\\" + pNewItem->m_sName;
					//std::vector< std::pair<std::string, time_t> >::iterator its = files_time_stamp.begin();
					//for (; its != files_time_stamp.end(); ++its)
					//{
					//	if (its->first.size() > 0)
					//	{
					//		if( strcmp(its->first.c_str(),file.Get()) == 0 )
					//		{
					//			pNewItem->m_tFileModify = its->second;
					//			break;
					//		}
					//	}
					//}

					auto it = files_time_stamp.find(file.Get());
					if (it != files_time_stamp.end()) {
						pNewItem->m_tFileModify = it->second;
					}
					std::vector<std::string>::iterator itf = files_favorite.begin();
					for (; itf != files_favorite.end(); ++itf)
					{
						if (itf->size() > 0)
						{
							if (strnicmp(itf->c_str(), file.Get(), file.Len() ) == 0)
							{
								pNewItem->bFavorite = true;
								break;
							}
						}
					}
					extern bool g_bFreeTrialVersion;
					if (g_bFreeTrialVersion == true)
					{
						SetAvailableInFreeTrial(foldertype, pNewItem, file);
					}

					pNewItem->m_sPath = pNewFolder->m_pNext->m_sFolderFullPath;
					pNewItem->m_sFolder = pNewFolder->m_pNext->m_sFolder;
					pNewItem->m_Backdrop = "";
					pNewItem->iFlags = 0;
					pNewItem->iPreview = 0;
					pNewItem->iBigPreview = 0;
					pNewItem->id = iTotalFiles++;
					pNewItem->bPreviewProcessed = false;
					pNewItem->m_pNext = NULL;
					pNewItem->m_pNextTime = NULL;
					pNewItem->m_pCustomSort = NULL;

					pNewItem->m_bFPELoaded = false;
					pNewItem->m_sFPEModel = "";
					pNewItem->m_sFPETextured = "";
					pNewItem->m_sFPEKeywords = "";
					pNewItem->m_sFPEDBOFile = "";
					pNewItem->m_sDLuaDescription = "##na##";
					pNewItem->m_fDLuaHeight = 0.0f;
					pNewItem->m_iFPEDBOFileSize = 0;
					pNewItem->m_bIsCharacterCreator = 0;
					pNewItem->iType = foldertype;

					m_pFiles->m_pNext = pNewItem;
					m_pFiles->m_pNextTime = pNewItem;
					m_pFiles->m_pCustomSort = pNewItem;
					m_pFiles = pNewItem;
				}
			}
			sorted_files.clear();

			CustomSortFiles(0, pNewFolder->m_pNext->m_pFirstFile);
		}
		SetDir("..");
	}
}

void RefreshEntityFolder(char* folder_s, void *pFolder)
{
	int tt = 0;
	cstr file_s = "";
	int fin = 0;
	cstr tempcstr;

	//pNewFolder->m_pFirstFile
	cFolderItem *pNewFolder = (cFolderItem *)pFolder;

	cstr OldDir = GetDir();
	if(pNewFolder && PathExist(folder_s) == 1)
	{
		int foldertype = pNewFolder->iType;

		//Delete everything inside pNewFolder->m_pFirstFile
		cFolderItem::sFolderFiles * m_pnextFiles, *m_pFiles = pNewFolder->m_pFirstFile;

		while (m_pFiles) 
		{
			m_pnextFiles = m_pFiles->m_pNext;
			if (m_pFiles->iPreview > 0) 
			{
				//Delete any old thumb images.
				//PE: Bug - tool icons can be used , so never delete m_pFiles->iPreview < UIV3IMAGES
				if ( m_pFiles->iPreview >= 4000 && GetImageExistEx(m_pFiles->iPreview) == 1 && m_pFiles->iPreview < UIV3IMAGES )
				{
					DeleteImage(m_pFiles->iPreview);
					m_pFiles->iPreview = 0;
				}
			}
			delete m_pFiles;
			m_pFiles = m_pnextFiles;
		}

		SetDir(folder_s);

		FindFirst(); fin = 0;

		std::vector<std::string> sorted_files;
		while (GetFileType() > -1)
		{
			file_s = GetFileName();
			if (file_s == "." || file_s == "..")
			{
				//  ignore . and ..
			}
			else
			{
				if (GetFileType() == 1)
				{
					//We only update the folder requested.
				}
				else
				{
					bool bValid = false;
					if (foldertype == 0 && pestrcasestr(file_s.Get(), ".fpe"))
						bValid = true;
					if (foldertype == 1 && pestrcasestr(file_s.Get(), ".wav"))
						bValid = true;
					if (foldertype == 1 && pestrcasestr(file_s.Get(), ".mp3"))
						bValid = true;
					if (foldertype == 1 && pestrcasestr(file_s.Get(), ".ogg"))
						bValid = true;

					if (foldertype == 2 && pestrcasestr(file_s.Get(), ".png"))
						bValid = true;
					if (foldertype == 2 && pestrcasestr(file_s.Get(), ".dds"))
						bValid = true;
					if (foldertype == 2 && pestrcasestr(file_s.Get(), ".bmp"))
						bValid = true;
					if (foldertype == 2 && pestrcasestr(file_s.Get(), ".tif"))
						bValid = true;
					if (foldertype == 2 && pestrcasestr(file_s.Get(), ".jpg"))
						bValid = true;
					if (foldertype == 2 && pestrcasestr(file_s.Get(), ".gif"))
						bValid = true;

					if (foldertype == 3 && pestrcasestr(file_s.Get(), ".wmv"))
						bValid = true;
					//if (foldertype == 3 && pestrcasestr(file_s.Get(), ".ogv"))
					//	bValid = true;
					if (foldertype == 3 && pestrcasestr(file_s.Get(), ".mp4"))
						bValid = true;

					if (foldertype == 4 && pestrcasestr(file_s.Get(), ".lua"))
						bValid = true;

					if (foldertype == 5 && (pestrcasestr(file_s.Get(), ".arx") || pestrcasestr(file_s.Get(), ".pe")))
						bValid = true;

					// The animation library was not displaying any animations, sorted_files was not populated with the animation files
					// This ensures that they are included
					//if (foldertype == 6 && pestrcasestr(file_s.Get(), ".dbo"))
					//	bValid = true;

					//PE: Should be .fpe, that also means that it displays the thumbs and you can preview animations :)
					if (foldertype == 6 && pestrcasestr(file_s.Get(), ".fpe"))
						bValid = true;



					if (bValid) {
						sorted_files.push_back(file_s.Get());

						//SAVE: time_create
						time_t ts = GetFileDateLong();
						cstr file = pNewFolder->m_sFolderFullPath;
						file = file + "\\" + file_s;

						//std::vector< std::pair<std::string, time_t> >::iterator its = files_time_stamp.begin();
						//for (; its != files_time_stamp.end(); ++its)
						//{
						//	if (its->first.size() > 0) {
						//		if (strcmp(its->first.c_str(), file.Get()) == 0)
						//		{
						//			files_time_stamp.erase(its);
						//			break;
						//		}
						//	}
						//}

						auto it = files_time_stamp.find(file.Get());
						if (it != files_time_stamp.end()) {
							files_time_stamp.erase(it);
						}

						//files_time_stamp.push_back(std::make_pair(file.Get(), ts));
						files_time_stamp[file.Get()] = ts;

					}
				}
			}
			FindNext();
			fin = fin + 1;
		}

		//sorted_files
		if (!sorted_files.empty()) 
		{
			std::sort(sorted_files.begin(), sorted_files.end(), NoCaseLess);

			std::vector<std::string>::iterator it = sorted_files.begin();

			if (it->size() > 0) 
			{
				cFolderItem::sFolderFiles *pNewItem = new cFolderItem::sFolderFiles;
				pNewItem->m_sName = "...";
				pNewItem->m_sNameFinal = pNewItem->m_sName;
				pNewItem->m_sNameFinalCredit = "";
				pNewItem->m_tFileModify = 0;
				pNewItem->m_Backdrop = "";
				pNewItem->bFavorite = false;
				pNewItem->bAvailableInFreeTrial = false;
				pNewItem->m_sPath = "";
				pNewItem->m_sFolder = "[na]";
				pNewItem->iFlags = 0;
				pNewItem->iPreview = 0;
				pNewItem->iBigPreview = 0;
				pNewItem->id = iTotalFiles++;
				pNewItem->bPreviewProcessed = false;
				pNewItem->m_pNext = NULL;
				pNewItem->m_pNextTime = NULL;
				pNewItem->m_pCustomSort = NULL;

				pNewItem->m_bFPELoaded = false;
				pNewItem->m_sFPEModel = "";
				pNewItem->m_sFPETextured = "";
				pNewItem->m_sFPEKeywords = "";
				pNewItem->m_sFPEDBOFile = "";
				pNewItem->m_sDLuaDescription = "##na##";
				pNewItem->m_fDLuaHeight = 0.0f;
				pNewItem->m_iFPEDBOFileSize = 0;
				pNewItem->m_bIsCharacterCreator = 0;
				pNewItem->iType = foldertype;

				pNewFolder->m_pFirstFile = pNewItem;
				m_pFiles = pNewItem;
			}

			for (; it != sorted_files.end(); ++it) 
			{
				//PE: Here we update , so we only have new files. but need to update duplicate_files_check
				if (it->size() > 0) {
					cStr sName = (char *)it->c_str();
					cStr m_sPath = folder_s;
					cStr sCheck = m_sPath + cstr("\\") + sName;
					char *find = NULL;

					if(foldertype == 0)
						find = (char *)pestrcasestr(sCheck.Get(), "entitybank\\");
					if (foldertype == 1)
						find = (char *)pestrcasestr(sCheck.Get(), "audiobank\\");
					if (foldertype == 2)
						find = (char *)pestrcasestr(sCheck.Get(), "imagebank\\");
					if (foldertype == 3)
						find = (char *)pestrcasestr(sCheck.Get(), "videobank\\");
					if (foldertype == 4)
						find = (char *)pestrcasestr(sCheck.Get(), "scriptbank\\");
					if (foldertype == 5)
						find = (char *)pestrcasestr(sCheck.Get(), "particlesbank\\");
					if (foldertype == 6)
						find = (char *)pestrcasestr(sCheck.Get(), "charactercreatorplus\\animations\\");

					if (find)
					{
						// ZJ: Got a heap corruption error here.
						//sCheck = find;
						cStr sFind(find);
						strcpy(sCheck.Get(), sFind.Get());
						//sCheck = sCheck + cstr("\\") + sName;
					}
					sCheck = sCheck.Lower();
					//auto itr = std::find(duplicate_files_check.begin(), duplicate_files_check.end(), sCheck.Get());
					//if(!(itr != duplicate_files_check.end() && duplicate_files_check.size() > 0))
					//	duplicate_files_check.push_back(sCheck.Get());

					if (duplicate_files_check.count(sCheck.Get()) <= 0)
						duplicate_files_check.insert(sCheck.Get());

				}
				if (it->size() > 0)
				{
					cFolderItem::sFolderFiles *pNewItem = new cFolderItem::sFolderFiles;
					pNewItem->m_sName = it->c_str();
					if ((foldertype == 0 || foldertype == 6) && pNewItem->m_sName.Len() > 4)
						pNewItem->m_sNameFinal = pNewItem->m_sName.Left(pNewItem->m_sName.Len() - 5);
					else
						pNewItem->m_sNameFinal = pNewItem->m_sName;

					// credit if community asset
					pNewItem->m_sNameFinalCredit = GetNameFinalCreditFromAbsPath (pNewFolder->m_sFolderFullPath.Get());

					//Generate a better search string. include category at end.#
					std::string sBetterSearch = pNewItem->m_sNameFinal.Get();
					sBetterSearch = sBetterSearch + " ( " + pNewFolder->m_sFolder.Get() + " )";
					//Remove main folder for better search.
					if (foldertype == 1)
						replaceAll(sBetterSearch, "audiobank", "");
					if (foldertype == 2)
						replaceAll(sBetterSearch, "imagebank", "");
					if (foldertype == 3)
						replaceAll(sBetterSearch, "videobank", "");
					if (foldertype == 4)
						replaceAll(sBetterSearch, "scriptbank", "");
					if (foldertype == 5)
						replaceAll(sBetterSearch, "particlesbank", "");
					if (foldertype == 6)
						replaceAll(sBetterSearch, "charactercreatorplus\\animations", "");

					replaceAll(sBetterSearch, "_", " ");
					replaceAll(sBetterSearch, "-", " ");
					pNewItem->m_sBetterSearch = sBetterSearch.c_str();

					pNewItem->m_tFileModify = 0; //PE: Need timestamp here.
					pNewItem->bFavorite = false;
					pNewItem->bAvailableInFreeTrial = false;

					//PE: This was to slow.
					struct stat sb;
					cstr file = pNewFolder->m_sFolderFullPath;
					file = file + "\\" + pNewItem->m_sName;

					//std::vector< std::pair<std::string, time_t> >::iterator its = files_time_stamp.begin();
					//for (; its != files_time_stamp.end(); ++its)
					//{
					//	if (its->first.size() > 0) 
					//	{
					//		if (strcmp(its->first.c_str(), file.Get()) == 0)
					//		{
					//			pNewItem->m_tFileModify = its->second;
					//			break;
					//		}
					//	}
					//}

					auto it = files_time_stamp.find(file.Get());
					if (it != files_time_stamp.end()) {
						pNewItem->m_tFileModify = it->second;
					}

					std::vector<std::string>::iterator itf = files_favorite.begin();
					for (; itf != files_favorite.end(); ++itf)
					{
						if (itf->size() > 0) 
						{
							if (strnicmp(itf->c_str(), file.Get(), file.Len()) == 0)
							{
								pNewItem->bFavorite = true;
								break;
							}
						}
					}
					extern bool g_bFreeTrialVersion;
					if (g_bFreeTrialVersion == true)
					{
						SetAvailableInFreeTrial(foldertype, pNewItem, file);
						/*
						std::vector<std::string>::iterator itf = files_availableinfreetrial.begin();
						for (; itf != files_availableinfreetrial.end(); ++itf)
						{
							if (itf->size() > 0)
							{
								if (strnicmp(itf->c_str(), file.Get(), file.Len()) == 0)
								{
									pNewItem->bAvailableInFreeTrial = true;
									break;
								}
							}
						}
						*/
					}

					pNewItem->m_sPath = folder_s;
					pNewItem->m_sFolder = folder_s;
					pNewItem->m_Backdrop = "";
					pNewItem->iFlags = 0;
					pNewItem->iPreview = 0;
					pNewItem->iBigPreview = 0;
					pNewItem->id = iTotalFiles++;
					pNewItem->bPreviewProcessed = false;
					pNewItem->m_pNext = NULL;
					pNewItem->m_pNextTime = NULL;
					pNewItem->m_pCustomSort = NULL;

					pNewItem->m_bFPELoaded = false;
					pNewItem->m_sFPEModel = "";
					pNewItem->m_sFPETextured = "";
					pNewItem->m_sFPEKeywords = "";
					pNewItem->m_sFPEDBOFile = "";
					pNewItem->m_sDLuaDescription = "##na##";
					pNewItem->m_fDLuaHeight = 0.0f;
					pNewItem->m_iFPEDBOFileSize = 0;
					pNewItem->m_bIsCharacterCreator = 0;
					pNewItem->iType = foldertype;
					m_pFiles->m_pNext = pNewItem;
					m_pFiles->m_pNextTime = pNewItem;
					m_pFiles->m_pCustomSort = pNewItem;
					m_pFiles = pNewItem;
				}
			}
			sorted_files.clear();

			CustomSortFiles(0, pNewFolder->m_pFirstFile);

		}

		//Update last folder modify date time.
		struct stat sb;
		if (stat(pNewFolder->m_sFolderFullPath.Get(), &sb) == 0) 
		{
			if (sb.st_mtime != pNewFolder->m_tFolderModify) 
			{
				pNewFolder->m_tFolderModify = sb.st_mtime;
			}
		}
	}

	SetDir(OldDir.Get());
}

void RefreshPurchasedFolder ( void )
{
	// First update for any folders
	// for this we need to delete the curretn one
	bool bCannotDeletFirstItIsGlobal = false;
	cFolderItem* pDeleteFolder = &MainEntityList;
	while (pDeleteFolder->m_pNext)
	{
		cFolderItem* pNextOneToDelete = pDeleteFolder->m_pNext;
		pDeleteFolder->m_pNext = NULL;
		if (bCannotDeletFirstItIsGlobal == false)
		{
			// just clear the first instance as it is global
			memset(&pDeleteFolder, 0, sizeof(pDeleteFolder));
			bCannotDeletFirstItIsGlobal = true;
		}
		else
		{
			// rest are created in runtime, so can delete
			delete pDeleteFolder;
		}
		pDeleteFolder = pNextOneToDelete;
	}
	extern bool bExternal_Entities_Init;
	bExternal_Entities_Init = false;

	// collect an entire list of all relevant filders (entitybank, scriptbank, images, particles, etc)
	extern void mapeditorexecutable_full_folder_refresh (void);
	mapeditorexecutable_full_folder_refresh();

	// Second scan purchased folder
	cFolderItem *pNewFolder = &MainEntityList;
	while (pNewFolder->m_pNext)
	{
		if (strnicmp (pNewFolder->m_sFolder.Get(), "purchased", strlen("purchased")) == NULL)
		{
			// force this folder to refresh
			pNewFolder->m_tFolderModify = 0;
			LPSTR pFolderFullPath = pNewFolder->m_sFolderFullPath.Get();
			RefreshEntityFolder(pFolderFullPath, pNewFolder);
			break;
		}
		pNewFolder = pNewFolder->m_pNext;
	}

	// Third scan ALL folders (store can place files and folders most places!)
	pNewFolder = &MainEntityList;
	while (pNewFolder->m_pNext)
	{
		// force this folder to refresh
		pNewFolder->m_tFolderModify = 0;
		LPSTR pFolderFullPath = pNewFolder->m_sFolderFullPath.Get();
		RefreshEntityFolder(pFolderFullPath, pNewFolder);
		pNewFolder = pNewFolder->m_pNext;
	}

	// ensure the sorted list that is static in the main loop is reset to avoid crash if contents changes mid-loop
	extern bool bUpdateSearchSorting;
	bUpdateSearchSorting = true;
}

char defaultWriteFolder[260];

void FindAWriteablefolder( void )
{
	if ( (SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, &defaultWriteFolder[0])) >= 0 ) {

		cstr tmp = defaultWriteFolder;
		tmp += "\\AppData";
		if (PathExist(tmp.Get()) == 1) {
			tmp += "\\Local\\TGC";
			if (PathExist(tmp.Get()) != 1) {
				_mkdir(tmp.Get());
			}
		}
		tmp += "\\";
		strcpy(defaultWriteFolder, tmp.Get());
	}
	else
	{
		//
		cstr tmp = GetDir();
		tmp += "\\";
		strcpy(defaultWriteFolder, tmp.Get());
	}
}

#include "Types.h"
#include <set>

void InitParseLuaScript(entityeleproftype *tmpeleprof)
{
	for (int i = 0; i < MAXPROPERTIESVARIABLES; i++) {
		tmpeleprof->PropertiesVariable.VariableType[i] = 0;
		strcpy(tmpeleprof->PropertiesVariable.Variable[i], "");
		strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], "");
		tmpeleprof->PropertiesVariable.VariableValueFrom[i] = 0.0f;
		tmpeleprof->PropertiesVariable.VariableValueTo[i] = 0.0f;
		//strcpy(tmpeleprof->PropertiesVariable.VariableSectionDescription[i], "");
		tmpeleprof->PropertiesVariable.VariableSectionDescription[i] = "";
		//strcpy(tmpeleprof->PropertiesVariable.VariableSectionEndDescription[i], "");
		tmpeleprof->PropertiesVariable.VariableSectionEndDescription[i] = "";
	}

	luadropdownlabels.clear();
	g_DLuaVariableNames.clear();
}

void ParseLuaScriptWithElementID(entityeleproftype *tmpeleprof, char * script, int iObjID)
{
	char cScriptName[MAX_PATH];
	strcpy(cScriptName, script);
	int pos;
	for (pos = strlen(cScriptName); pos > 0; pos-- ) {
		if (cScriptName[pos] == '\\' || cScriptName[pos] == '/')
			break;
	}
	if (pos > 0) {
		strcpy(cScriptName, &cScriptName[pos+1]);
		if (strlen(cScriptName) > 4)
			if (cScriptName[strlen(cScriptName) - 4] == '.')
				cScriptName[strlen(cScriptName) - 4] = 0;
	}

	strcpy(tmpeleprof->PropertiesVariable.VariableScript, cScriptName);

	InitParseLuaScript(tmpeleprof);

	tmpeleprof->PropertiesVariableActive = 0;
	tmpeleprof->PropertiesVariable.iVariables = 0;
	tmpeleprof->PropertiesVariable.VariableDescription = "";
	//scriptbank\markers\storyinzone.lua fails.

	FILE* fScript = GG_fopen(script, "r");
	if (fScript)
	{
		char ctmp[8192];
		int include_returns = 0;
		int description_lines = 0;
		bool bFirstLine = true;
		while (!feof(fScript))
		{
			fgets(ctmp, 8190 , fScript);
			ctmp[8190] = 0;
			if (strlen(ctmp) > 0 && ctmp[strlen(ctmp) - 1] == '\n')
				ctmp[strlen(ctmp) - 1] = 0;
			int cadd = 0;
			char *find = (char *) pestrcasestr(ctmp, "-- DESCRIPTION:");
			if (!find) {
				find = (char *)pestrcasestr(ctmp, "--DESCRIPTION:");
				cadd = -1;
			}

			//PE: Make a raw description text first, then parse after all descriptions are merged.

			if (find) {
				if(find[15] == ' ')
					strcpy(ctmp, find + 16 + cadd);
				else
					strcpy(ctmp, find + 15 + cadd);

				if(bFirstLine)
					tmpeleprof->PropertiesVariable.VariableDescription = ctmp;
				else {
					tmpeleprof->PropertiesVariable.VariableDescription += " "; 
					tmpeleprof->PropertiesVariable.VariableDescription += ctmp;
				}
				description_lines++;
				//Activate Propertie Variables.
				bFirstLine = false;
			}
		}
		fclose(fScript);

		//PE: Make error for scripts with missing newline.
		if (description_lines <= 1)
		{
			//PE: Check if lua files is missing \n newlines this can really give strange results.
			strcpy(ctmp, tmpeleprof->PropertiesVariable.VariableDescription.Get());
			//PE: Scan for non newline text file.
			for (int i = 0; i < strlen(ctmp); i++)
			{
				if (ctmp[i] == '\r')
				{
					include_returns++;
					break;
				}
			}
			if (include_returns > 0)
			{
				tmpeleprof->PropertiesVariable.VariableDescription = "Lua script is missing 'newlines' and description can't be parsed!";
			}
		}

		//Activate Propertie Variables.
		bFirstLine = false;
		//while [ , collect all variables.
		char *find = tmpeleprof->PropertiesVariable.VariableDescription.Get();
		char *SectionDescription = find;
		while ((find = (char *)pestrcasestr(find, "[")) && tmpeleprof->PropertiesVariable.iVariables < MAXPROPERTIESVARIABLES)
		{
			char *find2 = (char *)pestrcasestr(find, "]");
			if (find2) 
			{
				find2[0] = 0;
				find[0] = 0;
				if (strlen(SectionDescription) >= MAXVARIABLETEXTSIZELARGE)
				{
					//To Large.
					SectionDescription = find2 + 1;
				}
				else
				{
					if (tmpeleprof->PropertiesVariable.iVariables >= 1) 
					{
						//Add space
						//strcpy(tmpeleprof->PropertiesVariable.VariableSectionDescription[tmpeleprof->PropertiesVariable.iVariables], " ");
						//strcat(tmpeleprof->PropertiesVariable.VariableSectionDescription[tmpeleprof->PropertiesVariable.iVariables], SectionDescription);
						tmpeleprof->PropertiesVariable.VariableSectionDescription[tmpeleprof->PropertiesVariable.iVariables] = cStr(" ") + cStr(SectionDescription);
					}
					else 
					{
						//strcpy(tmpeleprof->PropertiesVariable.VariableSectionDescription[tmpeleprof->PropertiesVariable.iVariables], SectionDescription);
						tmpeleprof->PropertiesVariable.VariableSectionDescription[tmpeleprof->PropertiesVariable.iVariables] = SectionDescription;
					}
					//strcpy(tmpeleprof->PropertiesVariable.VariableSectionEndDescription[tmpeleprof->PropertiesVariable.iVariables], "");
					tmpeleprof->PropertiesVariable.VariableSectionEndDescription[tmpeleprof->PropertiesVariable.iVariables] = "";
					SectionDescription = find2 + 1;
					if (!pestrcasestr(SectionDescription, "[")) 
					{
						if (SectionDescription[0] != 0) 
						{
							//strcpy(tmpeleprof->PropertiesVariable.VariableSectionEndDescription[tmpeleprof->PropertiesVariable.iVariables], SectionDescription);
							tmpeleprof->PropertiesVariable.VariableSectionEndDescription[tmpeleprof->PropertiesVariable.iVariables] = SectionDescription;
						}
					}
					tmpeleprof->PropertiesVariable.VariableType[tmpeleprof->PropertiesVariable.iVariables] = 0;

					//PE: tmpeleprof->PropertiesVariable.Variable can be larger then 100 before '=' is found.
					char tmpVariable[1024];
					strcpy(tmpVariable, find + 1);

					//PE: Scan all lua script for largest [] // [@MULTI_TRIGGER=2(1=Yes, 2=No)]
					//if (strlen(tmpeleprof->PropertiesVariable.Variable[tmpeleprof->PropertiesVariable.iVariables]) > maxvariable)
					//	maxvariable = strlen(tmpeleprof->PropertiesVariable.Variable[tmpeleprof->PropertiesVariable.iVariables]);

					if (pestrcasestr(tmpVariable, "#"))
						tmpeleprof->PropertiesVariable.VariableType[tmpeleprof->PropertiesVariable.iVariables] = 1; //float
					if (pestrcasestr(tmpVariable, "$"))
						tmpeleprof->PropertiesVariable.VariableType[tmpeleprof->PropertiesVariable.iVariables] = 2; //string
					if (pestrcasestr(tmpVariable, "!"))
						tmpeleprof->PropertiesVariable.VariableType[tmpeleprof->PropertiesVariable.iVariables] = 3; //bool
					if (pestrcasestr(tmpVariable, "@"))
						tmpeleprof->PropertiesVariable.VariableType[tmpeleprof->PropertiesVariable.iVariables] = 4; // labelled int[]
					if (pestrcasestr(tmpVariable, "@@"))
						tmpeleprof->PropertiesVariable.VariableType[tmpeleprof->PropertiesVariable.iVariables] = 7; // labelled string
					if (pestrcasestr(tmpVariable, "*"))
						tmpeleprof->PropertiesVariable.VariableType[tmpeleprof->PropertiesVariable.iVariables] = 5; // user specified in seconds
					if (pestrcasestr(tmpVariable, "&"))
						tmpeleprof->PropertiesVariable.VariableType[tmpeleprof->PropertiesVariable.iVariables] = 6; // should alter eleprof variable

					//Set default values search for =
					char *find3 = (char *)pestrcasestr(tmpVariable, "=");
					if (find3) 
					{
						strcpy(tmpeleprof->PropertiesVariable.VariableValue[tmpeleprof->PropertiesVariable.iVariables], find3 + 1);
						
						find3[0] = 0; // remove = from Variable.
						strcpy(tmpeleprof->PropertiesVariable.Variable[tmpeleprof->PropertiesVariable.iVariables], tmpVariable);

						//check if we got a range.
						char *find4 = (char *)pestrcasestr(tmpeleprof->PropertiesVariable.VariableValue[tmpeleprof->PropertiesVariable.iVariables], "(");
						if (find4) 
						{
							find4[0] = 0;
							char from[1050];
							strcpy(from, find4 + 1);

							// Need to calculate the range differently for the dropdown type.
							if (tmpeleprof->PropertiesVariable.VariableType[tmpeleprof->PropertiesVariable.iVariables] == 4 || tmpeleprof->PropertiesVariable.VariableType[tmpeleprof->PropertiesVariable.iVariables] == 7)
							{
								// Count number of = symbols
								int iFirstEqualsIndex = -1;
								int iLastEqualsIndex = -1;
								for (int i = 0; i < strlen(from); i++)
								{
									if (from[i] == '=')
									{
										iLastEqualsIndex = i;
										if (iFirstEqualsIndex < 0)
											iFirstEqualsIndex = i;
									}
								}

								if (iFirstEqualsIndex > -1)
								{
									/* e.g. (1=Slow, 2=Fast, 3=Very Fast)*/
									/* or.  (0=AnimSetList)*/
									/* or.  (0=QuestList)*/

									// Determine range.
									tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables]
										= atof(from + iFirstEqualsIndex - 1);

									tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables]
										= atof(from + iLastEqualsIndex - 1);

									// Store labels.
									int count = tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables]
										- tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables];

									std::vector<std::string> labels;
									// First element is the variable number - needed for when there are more than one dropdown variables in a single script.
									char cVariable[8];
									sprintf(cVariable, "%d", tmpeleprof->PropertiesVariable.iVariables);
									labels.push_back(std::string(cVariable));

									for (int i = iFirstEqualsIndex; i < strlen(from); i++)
									{
										if (from[i] == '=')
										{
											int end = i;
											// Now find the comma or close bracket, to determine where the label starts and ends.
											for (int j = i+1; j < strlen(from); j++)
											{
												if (from[j] == ',' || from[j] == ')')
												{
													end = j;
													break;
												}
											}

											bool bExit = false;
											if (from[end] == ')')
												bExit = true;

											// copy the label into the buffer.
											char buffer[MAX_PATH];
											strcpy(buffer, from + i+1);
											buffer[end - i - 1] = 0;

											i = end;

											labels.push_back(std::string(buffer));

											if (bExit) break;
										}
									}

									// can intercept label list here if special indicator that it is an animset or questlist list
									//PE: Allow more DLUA lists to work on zones.
									if (labels.size()==2 ) // && iObjID > 0
									{
										if(iObjID > 0 && stricmp(labels[1].c_str(),"animsetlist")==NULL)
										{
											labels.clear();
											labels.push_back(cVariable);
											extern sObject* GetObjectData (int);
											sObject* pObject = GetObjectData(iObjID);
											sAnimationSet* pAnimSet = pObject->pAnimationSet;
											int iAnimSetIndex = 0;
											if (pAnimSet)
											{
												if (stricmp(pAnimSet->szName, "all") == NULL)
												{
													pAnimSet = pAnimSet->pNext; // skip first one (base zero all character anims)
												}
											}
											while (pAnimSet)
											{
												if (stricmp(pAnimSet->szName, "mouthshapes") != NULL)
												{
													iAnimSetIndex++;
													labels.push_back(pAnimSet->szName);
												}
												pAnimSet = pAnimSet->pNext;
											}
											if (iAnimSetIndex > 0)
											{
												tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 1;
												tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = iAnimSetIndex;
											}
											else
											{
												tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 0;
												tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = 0;
											}
										}
										if (labels.size() == 2)
										{
											//InterActionWeaponList
											if (stricmp(labels[1].c_str(), "interactionweaponlist") == NULL)
											{
												labels.clear();
												labels.push_back(cVariable);
												int iWeaponListIndex = 1;
												//PE: Add filter.
												int FillWeaponList(std::vector<std::string>& labels, char* filter);
												iWeaponListIndex += FillWeaponList(labels, "Interaction");

												if (iWeaponListIndex > 0)
												{
													tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 1;
													tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = iWeaponListIndex;
												}
												else
												{
													tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 0;
													tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = 0;
												}

											}
											if (stricmp(labels[1].c_str(), "anyweaponlist") == NULL)
											{
												labels.clear();
												labels.push_back(cVariable);
												int iWeaponListIndex = 1;
												int FillWeaponList(std::vector<std::string> &labels, char* filter);
												iWeaponListIndex += FillWeaponList( labels , nullptr);

												if (iWeaponListIndex > 0)
												{
													tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 1;
													tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = iWeaponListIndex;
												}
												else
												{
													tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 0;
													tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = 0;
												}

											}

											if (stricmp(labels[1].c_str(), "questlist") == NULL)
											{
												labels.clear();
												labels.push_back(cVariable);
												int iQuestListIndex = 1;
												labels.push_back("None");
												for (int n = 0; n < g_collectionQuestList.size(); n++)
												{
													iQuestListIndex++;
													labels.push_back(g_collectionQuestList[n].collectionFields[0].Get());
												}
												if (iQuestListIndex > 0)
												{
													tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 1;
													tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = iQuestListIndex;
												}
												else
												{
													tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 0;
													tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = 0;
												}
											}
										}
									}
									//PE: Let some lists work on zones also.
									if (labels.size() == 2)
									{
										if (stricmp(labels[1].c_str(), "armanimsetlist") == NULL)
										{
											labels.clear();
											labels.push_back(cVariable);
											labels.push_back("None");

											char animlist[MAX_PATH];
											//strcpy(animlist, "gamecore\\interactive_anims.txt");
											strcpy(animlist, "gamecore\\guns\\interactive\\interactive_anims.txt");
											GG_GetRealPath(animlist, 0);
											if (FileExist(animlist) == 1)
											{
												FILE* fAnimList = GG_fopen(animlist, "r");
												if (fAnimList)
												{
													char ctmp[MAX_PATH];
													while (!feof(fAnimList))
													{
														strcpy(ctmp, "");
														fgets(ctmp, MAX_PATH - 1, fAnimList);
														if (strlen(ctmp) > 0 && ctmp[strlen(ctmp) - 1] == '\n')
															ctmp[strlen(ctmp) - 1] = 0;
														if (strlen(ctmp) > 0)
															labels.push_back(&ctmp[0]);
													}
													fclose(fAnimList);
												}
											}
											if (labels.size() > 1)
											{
												tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 1;
												tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = labels.size();
											}
											else
											{
												tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 0;
												tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = 0;
											}

										}
										

										if (stricmp(labels[1].c_str(), "effectlist") == NULL)
										{
											labels.clear();
											labels.push_back(cVariable);
											labels.push_back("None");

											char writePath[MAX_PATH];
											extern const char* GG_GetWritePath();
											strcpy(writePath, GG_GetWritePath());

											std::vector<std::string> effectFilesWrite, effectFilesDoc;
											effectFilesWrite.clear();
											effectFilesDoc.clear();
											char writableEffect[MAX_PATH];
											strcpy(writableEffect, writePath);
											strcat(writableEffect, "Files\\particlesbank\\wpe");
											CollectFilesWithExtension(".pe", writableEffect, &effectFilesWrite);

											extern char szRootDir[MAX_PATH];
											char docEffect[MAX_PATH];
											strcpy(docEffect, szRootDir);
											strcat(docEffect, "\\Files\\particlesbank\\wpe");
											CollectFilesWithExtension(".pe", docEffect, &effectFilesDoc);

											std::set<std::string> uniqueEffectFiles;
											for (const std::string& file : effectFilesWrite) {
												uniqueEffectFiles.insert(&file[strlen(writableEffect)-17]);
											}
											for (const std::string& file : effectFilesDoc) {
												uniqueEffectFiles.insert(file);
											}
											for (const std::string& file : uniqueEffectFiles) {
												labels.push_back(&file[strlen(docEffect)-17]);
											}
											uniqueEffectFiles.clear();
											effectFilesWrite.clear();
											effectFilesDoc.clear();

											if (labels.size() > 1)
											{
												tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 1;
												tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = labels.size();
											}
											else
											{
												tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 0;
												tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = 0;
											}
										}
										if (stricmp(labels[1].c_str(), "decallist") == NULL)
										{
											labels.clear();
											labels.push_back(cVariable);
											labels.push_back("None");
											std::vector <cstr> mydecals;
											int fillgloballistwithdecals(std::vector <cstr>& list_s);
											int listmax = fillgloballistwithdecals(mydecals);
											for (int n = 1; n < listmax; n++)
											{
												if (mydecals[n].Len() > 0)
												{
													std::string name = mydecals[n].Get();
													labels.push_back(name);
												}
											}
											if (labels.size() > 1)
											{
												tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 1;
												tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = labels.size();
											}
											else
											{
												tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 0;
												tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = 0;
											}
										}

										if (stricmp(labels[1].c_str(), "hudscreenlist") == NULL)
										{
											static std::vector<std::string> globallist_labels;
											extern StoryboardStruct Storyboard;
											labels.clear();
											labels.push_back(cVariable);
											labels.push_back("None");
											for (int allhudscreensnodeid = 0; allhudscreensnodeid < STORYBOARD_MAXNODES; allhudscreensnodeid++)
											{
												if (Storyboard.Nodes[allhudscreensnodeid].used)
												{
													if (Storyboard.Nodes[allhudscreensnodeid].type == STORYBOARD_TYPE_HUD)
													{
														if (strlen(Storyboard.Nodes[allhudscreensnodeid].title) > 0)
														{
															labels.push_back(Storyboard.Nodes[allhudscreensnodeid].title);

														}
													}
												}
											}
											if (labels.size() > 1)
											{
												tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 1;
												tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = labels.size();
											}
											else
											{
												tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 0;
												tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = 0;
											}

										}
										if (stricmp(labels[1].c_str(), "globallist") == NULL)
										{
											static std::vector<std::string> globallist_labels;
											extern StoryboardStruct Storyboard;
											extern std::vector<int> g_gameGlobalListNodeId;
											extern std::vector<int> g_gameGlobalListIndex;
											extern std::vector<int> g_gameGlobalListValue;

											if (1) //PE: Always update.
											{
												globallist_labels.clear();
												globallist_labels.push_back(cVariable);
												globallist_labels.push_back("None");
												//PE: Need to push these to g_UserGlobal[''].
												globallist_labels.push_back("Gun Ammo Remaining");
												globallist_labels.push_back("Health Remaining");
												globallist_labels.push_back("Lives Remaining");

												for (int allhudscreensnodeid = 0; allhudscreensnodeid < STORYBOARD_MAXNODES; allhudscreensnodeid++)
												{
													//PE: Need custom ? stricmp(readout.c_str(), "User Defined Global Statusbar") == NULL
													if (strlen(Storyboard.Nodes[allhudscreensnodeid].lua_name) > 0 && strnicmp(Storyboard.Nodes[allhudscreensnodeid].lua_name, "hud", 3) == NULL)
													{
														for (int i = STORYBOARD_MAXWIDGETS; i >= 0; i--)
														{
															if (Storyboard.Nodes[allhudscreensnodeid].widget_type[i] == STORYBOARD_WIDGET_TEXT)
															{
																std::string readout = Storyboard.widget_readout[allhudscreensnodeid][i];
																if (stricmp(readout.c_str(), "User Defined Global") == NULL
																	|| stricmp(Storyboard.widget_readout[allhudscreensnodeid][i], "User Defined Global Text") == NULL)
																{
																	//"User Defined Global Statusbar"
																	// only add unique ones to game global list
																	LPSTR pNewName = Storyboard.Nodes[allhudscreensnodeid].widget_label[i];
																	if (strlen(pNewName) > 0)
																	{
																		if (!pestrcasestr(pNewName, ":")) //PE: Do not show : rpginventorykinds.
																		{
																			for (int n = 0; n < globallist_labels.size(); n++)
																			{
																				if (strcmp(pNewName, globallist_labels[n].c_str()) == NULL)
																				{
																					// already exists
																					pNewName = "";
																					break;
																				}
																			}
																			if (strlen(pNewName) > 0)
																				globallist_labels.push_back(pNewName);
																		}
																	}
																}
															}
														}
													}
												}
											}
											labels = globallist_labels;
											if (labels.size() > 1)
											{
												tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 1;
												tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = labels.size();
											}
											else
											{
												tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = 0;
												tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = 0;
											}
										}
									}
								

									// and add to official dropdown lists
									luadropdownlabels.push_back(labels);
								}
							}
							else
							{
								// Extract range normally.
								char *find5 = (char *)pestrcasestr(from, ",");
								if (find5)
								{
									find5[0] = 0;
									tmpeleprof->PropertiesVariable.VariableValueFrom[tmpeleprof->PropertiesVariable.iVariables] = atof(find4 + 1);
									char *find6 = (char *)pestrcasestr(find5 + 1, ")");
									if (find6)
									{
										find6[0] = 0;
										tmpeleprof->PropertiesVariable.VariableValueTo[tmpeleprof->PropertiesVariable.iVariables] = atof(find5 + 1);
									}
								}
							}
						}
					}
					else
					{
						strcpy(tmpeleprof->PropertiesVariable.VariableValue[tmpeleprof->PropertiesVariable.iVariables], "");
						tmpVariable[MAXVARIABLESIZE - 1] = 0;
						strcpy(tmpeleprof->PropertiesVariable.Variable[tmpeleprof->PropertiesVariable.iVariables], tmpVariable);
					}

					//Clean variable name.
					std::string clean_string = tmpeleprof->PropertiesVariable.Variable[tmpeleprof->PropertiesVariable.iVariables];
					replaceAll(clean_string, "#", "");
					replaceAll(clean_string, "$", "");
					replaceAll(clean_string, "!", "");
					replaceAll(clean_string, "@", "");
					replaceAll(clean_string, "*", "");
					replaceAll(clean_string, "&", "");
					if (tmpeleprof->PropertiesVariable.VariableType[tmpeleprof->PropertiesVariable.iVariables] == 6)
					{
						g_DLuaVariableNames.push_back(clean_string);
					}
					strcpy(tmpeleprof->PropertiesVariable.Variable[tmpeleprof->PropertiesVariable.iVariables], clean_string.c_str());

					//Clean variablevalue.
					clean_string = tmpeleprof->PropertiesVariable.VariableValue[tmpeleprof->PropertiesVariable.iVariables];
					replaceAll(clean_string, "\"", ""); //cant use "
					strcpy(tmpeleprof->PropertiesVariable.VariableValue[tmpeleprof->PropertiesVariable.iVariables], clean_string.c_str());

					tmpeleprof->PropertiesVariable.iVariables++;
					tmpeleprof->PropertiesVariableActive = 1;
				}

				find = find2 + 1;
			}
			else find++;
		}


		//[RANGE] integer [SPEED#] float [TEXT$] string
		cstr sLuaScriptName = tmpeleprof->PropertiesVariable.VariableScript;
		sLuaScriptName += "_properties(";
		//Check if we need to update with new default values.
		if (!pestrcasestr(tmpeleprof->soundset4_s.Get(),sLuaScriptName.Get())) {
			if (tmpeleprof->PropertiesVariable.iVariables > 0) {
				tmpeleprof->soundset4_s = sLuaScriptName;
				//Add varables.
				for (int i = 0; i < tmpeleprof->PropertiesVariable.iVariables; i++) {

					char val[3];
					val[0] = tmpeleprof->PropertiesVariable.VariableType[i] + '0';
					val[1] = 0;

					tmpeleprof->soundset4_s += val;
					tmpeleprof->soundset4_s += "\"";
					std::string clean_string = tmpeleprof->PropertiesVariable.VariableValue[i];
					replaceAll(clean_string, "\"", ""); //cant use "
					tmpeleprof->soundset4_s += (char *)clean_string.c_str();
					//tmpeleprof->soundset4_s += tmpeleprof->PropertiesVariable.VariableValue[i];
					tmpeleprof->soundset4_s += "\"";
					if (i < tmpeleprof->PropertiesVariable.iVariables - 1)
						tmpeleprof->soundset4_s += ",";
				}
				tmpeleprof->soundset4_s += ")";
				//fclose(fScript);
				tmpeleprof->PropertiesVariableActive = 1;
				//return;
			}
		}

		//Get old value into arrayes.
		char tmp[4096];
		ZeroMemory(tmp, 4095);
		strcpy(tmp, tmpeleprof->soundset4_s.Get()); //dont change original.
//		char *find;
		int iNextVariable = 0;
		find = (char *)pestrcasestr(tmp, "(");
		if (find) {
			//cmd = &tmp[0];
			find[0] = 0;
			find++;
			while ((find = (char *)pestrcasestr(find, "\"")) && iNextVariable < tmpeleprof->PropertiesVariable.iVariables) {
				char *find2 = (char *)pestrcasestr(find + 1, "\"");
				if (find2) {
					int type = find[-1] - '0';
					find2[0] = 0;

					if (type >= 0 && type <= 9) {
						if (type == 1) {
							strcpy(tmpeleprof->PropertiesVariable.VariableValue[iNextVariable],find + 1);
						}
						else if (type == 2) {
							strcpy(tmpeleprof->PropertiesVariable.VariableValue[iNextVariable], find + 1);
						}
						else {
							strcpy(tmpeleprof->PropertiesVariable.VariableValue[iNextVariable], find + 1);
						}
					}
					find = find2 + 1;
				}
				else {
					find++;
				}

				iNextVariable++;
			}
		}

	}
}

void ParseLuaScript(entityeleproftype *tmpeleprof, char * script)
{
	ParseLuaScriptWithElementID(tmpeleprof, script, -1);
}

cstr DLUAFormatLabel(LPSTR pIn)
{
	// introduce a display space if the first word has a capital in the original variable label
	bool bDetectAnyLowerCase = false;
	for (int i = 0; i < strlen(pIn); i++)
	{
		unsigned char letter = pIn[i];
		if (letter >= 'a' && letter <= 'z')
		{
			bDetectAnyLowerCase = true;
			break;
		}
	}
	int iWordPos = 0;
	char pFullLabel[MAX_PATH];
	memset(pFullLabel, 0, sizeof(pFullLabel));
	if (bDetectAnyLowerCase == true)
	{
		// handle lower case word separating
		for (int i = 0; i < strlen(pIn); i++)
		{
			bool bUpper = false;
			unsigned char letter = pIn[i];
			if (letter >= 'A' && letter <= 'Z') bUpper = true;
			if (i > 0 && bUpper == true) pFullLabel[iWordPos++] = ' ';
			pFullLabel[iWordPos++] = letter;
		}
		pFullLabel[iWordPos] = 0;
	}
	else
	{
		// all or mostly upper case, treat as such
		strcpy (pFullLabel, pIn);
	}
	// format variable labels to convert _ to spaces
	for (int i = 0; i < strlen(pFullLabel); i++)
	{
		unsigned char letter = pFullLabel[i];
		if (letter == '_') pFullLabel[i] = ' ';
	}
	// finally ensure all lower case except for first letter of each word (now separated by space)
	strlwr(pFullLabel);
	bool bMakeUpperCase = true;
	for (int i = 0; i < strlen(pFullLabel); i++)
	{
		unsigned char letter = pFullLabel[i];
		if (bMakeUpperCase == true)
		{
			int iLwrToUpr = 'a' - 'A';
			pFullLabel[i] = pFullLabel[i] - iLwrToUpr;
			bMakeUpperCase = false;
		}
		if (letter == ' ') bMakeUpperCase = true;
	}
	cstr out = pFullLabel;
	return out;
}
float fDescriptionBoxSize = 0;
cstr sDescriptionBoxSize = "";
int DisplayLuaDescription(entityeleproftype *tmpeleprof)
{
	char tmp[2048];
	bool bUpdateMainString = false;
	int speech_entries = 0;
	float fPropertiesColoumWidth = ImGui::GetCursorPosX() + 110.0f;
	
	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	if (1) 
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_FrameBg]);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, style.FrameBorderSize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, style.FramePadding);

		bool ret;

		if (fDescriptionBoxSize > ImGui::GetFontSize() * 7) 
		{
			ret = ImGui::BeginChild("##DLUADescriptionbox", ImVec2(ImGui::GetContentRegionAvailWidth() - 10, (ImGui::GetFontSize()*7)), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding );
		}
		else 
		{
			if (fDescriptionBoxSize == 0)
				ret = ImGui::BeginChild("##DLUADescriptionbox", ImVec2(ImGui::GetContentRegionAvailWidth() - 10, (ImGui::GetFontSize()*5)), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
			else
				ret = ImGui::BeginChild("##DLUADescriptionbox", ImVec2(ImGui::GetContentRegionAvailWidth() - 10, fDescriptionBoxSize), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
		}
		void fDisplayDescriptionBox(entityeleproftype *tmpeleprof, bool textonly = false);
		fDisplayDescriptionBox(tmpeleprof);
		ImGui::EndChild();

		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor();
	}
	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

	extern int speech_ids[5];

	int imageindexi = 0; // can have eight images indexed this way
	bool bwpefile = false;

	for (int i = 0; i < tmpeleprof->PropertiesVariable.iVariables; i++) 
	{
		bool speech = false;

		cstr tmpvar = tmpeleprof->PropertiesVariable.Variable[i];
		tmpvar = tmpvar.Lower();

		if (speech_entries <= 3) {
			if (tmpvar == "speech1" || tmpvar == "speech 1") {
				speech_ids[speech_entries++] = i; speech = true;
			}
			else if (tmpvar == "speech2" || tmpvar == "speech 2") {
				speech_ids[speech_entries++] = i; speech = true;
			}
			else if (tmpvar == "speech3" || tmpvar == "speech 3") {
				speech_ids[speech_entries++] = i; speech = true;
			}
			else if (tmpvar == "speech4" || tmpvar == "speech 4") {
				speech_ids[speech_entries++] = i; speech = true;
			}
			else if (tmpvar == "speech0" || tmpvar == "speech 0") {
				speech_ids[speech_entries++] = i; speech = true;
			}
		}
		if (!speech)
		{
			if (tmpeleprof->PropertiesVariable.VariableType[i] == 3)
			{
				// BOOL needs to be on one line
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));
				ImGui::Text(DLUAFormatLabel(tmpeleprof->PropertiesVariable.Variable[i]).Get());
				ImGui::SameLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 3));
				ImGui::SetCursorPos(ImVec2(fPropertiesColoumWidth, ImGui::GetCursorPosY()));
			}
			else
			{
				// REST can use new gadget style
				ImGui::TextCenter(DLUAFormatLabel(tmpeleprof->PropertiesVariable.Variable[i]).Get());
			}

			ImGui::PushID(54321 + i); //unique id.
			ImGui::PushItemWidth(-10);
			cstr val = tmpeleprof->PropertiesVariable.VariableValue[i];
			if (tmpeleprof->PropertiesVariable.VariableType[i] == 1)
			{
				//Float
				float tmpfloat = atof(val.Get());
				if (tmpeleprof->PropertiesVariable.VariableValueTo[i] > 0 && tmpeleprof->PropertiesVariable.VariableValueTo[i] > tmpeleprof->PropertiesVariable.VariableValueFrom[i])
				{
					ImGui::PushItemWidth(-50);
					if (ImGui::SliderFloat("##floatslider", &tmpfloat, tmpeleprof->PropertiesVariable.VariableValueFrom[i], tmpeleprof->PropertiesVariable.VariableValueTo[i], " "))
					{
						sprintf(tmp, "%f", tmpfloat);
						strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], tmp);
						bUpdateMainString = true;
					}
					ImGui::SameLine();
					ImGui::PopItemWidth();
					ImGui::PushItemWidth(30);
					if (ImGui::InputFloat("##floatinput", &tmpfloat, 0, 0, "%.2f"))
					{
						// optionally cap within any specified range
						if (tmpfloat < tmpeleprof->PropertiesVariable.VariableValueFrom[i]) tmpfloat = tmpeleprof->PropertiesVariable.VariableValueFrom[i];
						if (tmpfloat > tmpeleprof->PropertiesVariable.VariableValueTo[i]) tmpfloat = tmpeleprof->PropertiesVariable.VariableValueTo[i];
					
						sprintf(tmp, "%f", tmpfloat);
						strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], tmp);
						bUpdateMainString = true;
					}
					ImGui::PopItemWidth();
				}
				else
				{
					cstr title = cstr("##float") + cstr(tmpeleprof->PropertiesVariable.Variable[i]);
					if (ImGui::InputFloat(title.Get(), &tmpfloat))
					{
						// ZJ: Range based floats are above.
						//// optionally cap within any specified range
						//if (tmpfloat < tmpeleprof->PropertiesVariable.VariableValueFrom[i]) tmpfloat = tmpeleprof->PropertiesVariable.VariableValueFrom[i];
						//if (tmpfloat > tmpeleprof->PropertiesVariable.VariableValueTo[i]) tmpfloat = tmpeleprof->PropertiesVariable.VariableValueTo[i];
					}
					sprintf(tmp, "%f", tmpfloat);
					strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], tmp);
					bUpdateMainString = true;
				}

			}
			else if (tmpeleprof->PropertiesVariable.VariableType[i] == 2) 
			{
				//String
				char * imgui_setpropertyfile2(int group, char* data_s, char* field_s, char* desc_s, char* within_s);
				char * imgui_setpropertyfile2_v2(int group, char* data_s, char* field_s, char* desc_s, char* within_s, bool readonly, char *startsearch = NULL);;
				//Special setups.
				//VIDEO1, FILE-IMAGE for file selector.
				if (pestrcasestr(tmpvar.Get(), "file")) 
				{
					if (pestrcasestr(tmpvar.Get(), "image")) 
					{
						cstr tmpvalue = tmpeleprof->PropertiesVariable.VariableValue[i];
						bool readonly = false;

						//Allow up to 8 images to be previewed in the properties area
						//#define IMGFILEID (PROPERTIES_CACHE_ICONS+998)
						int iImgFileIndex = imageindexi;
						if (iImgFileIndex > 8) iImgFileIndex = 8;
						int iImgFileID = PROPERTIES_CACHE_ICONS + 900 + iImgFileIndex;

						static cstr imgfile[10] = { "", "", "", "", "", "", "", "", "", "" };
						static int imgfile_preview_id[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
						if (tmpvalue != imgfile[iImgFileIndex] )
						{
							//Load new image preview.
							imgfile_preview_id[iImgFileIndex] = 0;
							if (tmpvalue != "")
							{
								image_setlegacyimageloading(true);
								LoadImage((char *)tmpvalue.Get(), iImgFileID);
								image_setlegacyimageloading(false);
								imgfile_preview_id[iImgFileIndex] = iImgFileID;
								if (!GetImageExistEx(iImgFileID))
								{
									imgfile_preview_id[iImgFileIndex] = 0;
								}
							}
							imgfile[iImgFileIndex] = tmpvalue;
						}

						tmpvalue = imgui_setpropertyfile2_v2(1, tmpvalue.Get(), "", "Select image to appear in-level", "imagebank\\", readonly);

						if (imgfile_preview_id[iImgFileIndex] > 0 && GetImageExistEx(imgfile_preview_id[iImgFileIndex]))
						{
							extern ImVec4 drawCol_back;
							extern ImVec4 drawCol_normal;
							extern ImVec4 drawCol_hover;
							extern ImVec4 drawCol_Down;

							float w = ImGui::GetContentRegionAvailWidth();
							float iwidth = w;
							float ImgW = ImageWidth(imgfile_preview_id[iImgFileIndex]);
							float ImgH = ImageHeight(imgfile_preview_id[iImgFileIndex]);
							float fHighRatio = ImgH / ImgW;
							if (ImgW < (iwidth - 18.0f))
							{
								//PE: Fit to width.
								iwidth = ImgW + 18.0f;
							}
							ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (iwidth*0.5), 0.0f));
							ImGui::ImgBtn(imgfile_preview_id[iImgFileIndex], ImVec2( iwidth-18.0f, (iwidth - 18.0f) * fHighRatio), drawCol_back, drawCol_normal, drawCol_normal, drawCol_normal, -1, 0, 0, 0, true);
						}
						if (tmpvalue != tmpeleprof->PropertiesVariable.VariableValue[i]) {
							strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], tmpvalue.Get());
							bUpdateMainString = true;
						}
						imageindexi++;
					}
					else 
					{
						if (pestrcasestr(tmpeleprof->PropertiesVariable.Variable[i], "wpefile"))
						{
							if (FileExist(tmpeleprof->PropertiesVariable.VariableValue[i]))
							{
								//PE: Possible to preview.
								bwpefile = true;
							}
						}
						cstr tmpvalue = tmpeleprof->PropertiesVariable.VariableValue[i];
						tmpvalue = imgui_setpropertyfile2(1, tmpvalue.Get(), "", "Select File", "..\\files\\");
						if (tmpvalue != tmpeleprof->PropertiesVariable.VariableValue[i]) 
						{
							LPSTR pThisString = tmpvalue.Get();
							if (pThisString[1] == ':')
							{
								// replace absolute paths with relative ones
								char pRelativePathAndFile[MAX_PATH];
								strcpy(pRelativePathAndFile, tmpvalue.Get());
								GG_GetRealPath(pRelativePathAndFile, 0);
								extern char szWriteDir[MAX_PATH];
								char pRemoveAbsPart[MAX_PATH];
								strcpy(pRemoveAbsPart, szWriteDir);
								strcat(pRemoveAbsPart, "Files\\");
								if (strnicmp(pRelativePathAndFile, pRemoveAbsPart, strlen(pRemoveAbsPart)) == NULL)
								{
									strcpy(pRelativePathAndFile, pThisString + strlen(pRemoveAbsPart));
								}
								strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], pRelativePathAndFile);
							}
							else
							{
								strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], tmpvalue.Get());
							}
							bUpdateMainString = true;
							if (PreviewWPERoot != 0 && bPreviewWPE)
							{
								//PE: Delete effect and reload.
								WickedCall_PerformEmitterAction(6, PreviewWPERoot);
								void DeleteEmitterEffects(uint32_t root);
								DeleteEmitterEffects(PreviewWPERoot);
								PreviewWPERoot = 0;
								PreviewWPERoot = WickedCall_LoadWPE(tmpeleprof->PropertiesVariable.VariableValue[i]);
								void WickedCall_PerformEmitterAction(int iAction, uint32_t emitter_root);
								WickedCall_PerformEmitterAction(1, PreviewWPERoot);
								WickedCall_PerformEmitterAction(4, PreviewWPERoot);
								WickedCall_PerformEmitterAction(5, PreviewWPERoot);
							}
						}
						if (bwpefile)
						{
							ImGui::Separator();
							if (ImGui::Checkbox("Preview", &bPreviewWPE))
							{
								if (PreviewWPERoot == 0 && bPreviewWPE)
								{
									//PE: Load effect.
									//PE: Delete this preview before test game. and set bPreviewWPE = false
									PreviewWPERoot = WickedCall_LoadWPE(tmpeleprof->PropertiesVariable.VariableValue[i]);
									//iAction = 1 Burst all. 2 = Pause. - 3 = Resume. - 4 = Restart - 5 - visible - 6 = not visible. - 7 = pause emit - 8 = resume emit
									void WickedCall_PerformEmitterAction(int iAction, uint32_t emitter_root);
									WickedCall_PerformEmitterAction(1, PreviewWPERoot);
									WickedCall_PerformEmitterAction(4, PreviewWPERoot);
									WickedCall_PerformEmitterAction(5, PreviewWPERoot);
								}
								if (PreviewWPERoot != 0 && !bPreviewWPE)
								{
									//PE: Delete effects.
									WickedCall_PerformEmitterAction(6, PreviewWPERoot);
									void DeleteEmitterEffects(uint32_t root);
									DeleteEmitterEffects(PreviewWPERoot);
									PreviewWPERoot = 0;
								}
							}
							ImGui::SameLine();
							if (ImGui::Button("Burst##Burst", ImVec2(60, 0)))
							{
								WickedCall_PerformEmitterAction(1, PreviewWPERoot);
							}
							ImGui::Separator();
						}
					}
				}
				else if (tmpvar == "video1" || tmpvar == "video 1") 
				{
					cstr tmpvalue = tmpeleprof->soundset1_s;

					#define VIDEOFILEID (PROPERTIES_CACHE_ICONS+997)
					static cstr videofile = "";
					static int videofile_preview_id = 0;
					if (tmpvalue != videofile)
					{
						//Load new image preview.
						videofile_preview_id = 0;
						if (tmpvalue != "")
						{
							videofile_preview_id = VIDEOFILEID;

							std::string stmp = tmpvalue.Get();
							replaceAll(stmp, "videobank", ""); //Video thumbs stored without videobank.
							replaceAll(stmp, "\\\\", "\\"); //Remove double backslash.

							bool CreateBackBufferCacheName(char *file, int width, int height);
							extern cstr BackBufferCacheName;
							CreateBackBufferCacheName( (char *) stmp.c_str(), 512, 288);
							extern StoryboardStruct Storyboard;
							if (!(strlen(Storyboard.gamename) > 0 && strlen(Storyboard.customprojectfolder) > 0))
							{
								GG_SetWritablesToRoot(true);
							}
							SetMipmapNum(1); //PE: mipmaps not needed.
							image_setlegacyimageloading(true);
							if (FileExist(BackBufferCacheName.Get()))
							{
								LoadImage((char *)BackBufferCacheName.Get(), videofile_preview_id);
							}
							image_setlegacyimageloading(false);
							SetMipmapNum(-1); //PE: mipmaps not needed.
							GG_SetWritablesToRoot(false);
							if (!GetImageExistEx(VIDEOFILEID))
							{
								videofile_preview_id = 0;
							}
						}
						videofile = tmpvalue;
					}


					tmpvalue = imgui_setpropertyfile2(1, tmpeleprof->soundset1_s.Get(), "", "Specify movie file you would like to play when the player enters the zone", "videobank\\");
					if (tmpvalue != tmpeleprof->soundset1_s) 
					{
						tmpeleprof->soundset1_s = tmpvalue;
						bUpdateMainString = true;
					}

					if (videofile_preview_id > 0 && GetImageExistEx(videofile_preview_id))
					{
						extern ImVec4 drawCol_back;
						extern ImVec4 drawCol_normal;
						extern ImVec4 drawCol_hover;
						extern ImVec4 drawCol_Down;

						float w = ImGui::GetContentRegionAvailWidth();
						float iwidth = w;
						float ImgW = ImageWidth(videofile_preview_id);
						float ImgH = ImageHeight(videofile_preview_id);
						float fHighRatio = ImgH / ImgW;
						ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w*0.5) - (iwidth*0.5), 0.0f));
						ImGui::ImgBtn(videofile_preview_id, ImVec2(iwidth - 18.0f, (iwidth - 18.0f) * fHighRatio), drawCol_back, drawCol_normal, drawCol_normal, drawCol_normal, -1, 0, 0, 0, true);
					}

				}
				else 
				{
					if (ImGui::InputText("##string", &tmpeleprof->PropertiesVariable.VariableValue[i][0], MAXVARIABLESIZE)) 
					{
						bUpdateMainString = true;
					}
					if (ImGui::MaxIsItemFocused()) bImGuiGotFocus = true;

				}
			}
			else if (tmpeleprof->PropertiesVariable.VariableType[i] == 3) 
			{
				//Bool
				bool tmpbool = atoi(val.Get());
				if( ImGui::Checkbox("##bool",&tmpbool) ) 
				{
					if(tmpbool)
						strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], "1");
					else
						strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], "0");
					bUpdateMainString = true;
				}
				// hack in tooltip for now, better idea is adding verbose to LUA itself!
				LPSTR pTooltipForTickbox = NULL;
				if (stricmp (tmpeleprof->PropertiesVariable.Variable[i], "FollowAPath") == NULL) pTooltipForTickbox = "Tick to all character to find and follow a nearby flag path";
				if (stricmp (tmpeleprof->PropertiesVariable.Variable[i], "CanRetreat") == NULL) pTooltipForTickbox = "Tick to allow the character to retreat when hurt";
				if (stricmp (tmpeleprof->PropertiesVariable.Variable[i], "StandGround") == NULL) pTooltipForTickbox = "Tick to ensure the character stays near their starting position";
				if (stricmp (tmpeleprof->PropertiesVariable.Variable[i], "FlankTarget") == NULL) pTooltipForTickbox = "Tick to allow the character to wide flank their target";
				if (stricmp (tmpeleprof->PropertiesVariable.Variable[i], "Alerted") == NULL) pTooltipForTickbox = "Tick to skip the initial suspicion and go direct to full alert";
				if (stricmp (tmpeleprof->PropertiesVariable.Variable[i], "Feeding") == NULL) pTooltipForTickbox = "Tick to start the zombie off in a ground feeding state";
				if (pTooltipForTickbox && ImGui::IsItemHovered()) ImGui::SetTooltip(pTooltipForTickbox);
			}
			else if (tmpeleprof->PropertiesVariable.VariableType[i] == 4 || tmpeleprof->PropertiesVariable.VariableType[i] == 7)
			{
				// Dropdown of labelled integers.
				std::vector<std::string> labels;
				bool bGotLabels = false;
				bool bAsString = false;
				if (tmpeleprof->PropertiesVariable.VariableType[i] == 7)
					bAsString = true;

				// Retrieve the labels for this dropdown.
				for (int l = 0; l < luadropdownlabels.size(); l++)
				{
					labels = luadropdownlabels[l];

					// The first element is the variable number.
					if (atol(labels[0].c_str()) == i)
					{
						bGotLabels = true;
						break;
					}
				}
				if (bGotLabels)
				{
					// type of dropdown
					bool bIsAQuestList = false;
					int iQuestIndex = 0;

					// Determine the label for the currently selected value.
					int iSelectedIndex = 0;
					const char* preview = "";
					if (bAsString)
					{
						preview = tmpeleprof->PropertiesVariable.VariableValue[i];
					}
					else
					{
						iSelectedIndex = atol(tmpeleprof->PropertiesVariable.VariableValue[i]) - tmpeleprof->PropertiesVariable.VariableValueFrom[i];
						// Since the first element of the labels is the variable number, add 1.
						iSelectedIndex++;
						// Ensure preview for combo is always valid
						if (iSelectedIndex < labels.size()) preview = labels[iSelectedIndex].c_str();
					}

					// No combi if no choices
					if (tmpeleprof->PropertiesVariable.VariableValueTo[i] == 0 && tmpeleprof->PropertiesVariable.VariableValueFrom[i] == 0)
					{
						// Indicate no choices (no drop down list or animations)
						ImGui::TextCenter("No choices available");
					}
					else
					{
						// Create combo now
						if (ImGui::BeginCombo("##DLUACOMBO", preview))
						{
							for(int j = 1; j < labels.size(); j++)
							{
								bool bSelected = false;
								char labelID[128];
								int iValue = tmpeleprof->PropertiesVariable.VariableValueFrom[i] + j;
								sprintf(labelID, "%d", iValue-1);

								char label[128];
								strcpy_s(label, 127,  labels[j].c_str());
								label[127] = 0;

								if (bAsString)
								{
									if (strcmp(label, tmpeleprof->PropertiesVariable.VariableValue[i]) == NULL)
										bSelected = true;
								}
								else
								{
									if (strcmp(labelID, tmpeleprof->PropertiesVariable.VariableValue[i]) == NULL)
										bSelected = true;
								}
							
								if (ImGui::Selectable(label, bSelected))
								{
									if (bAsString)
									{
										strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], label);
										if (strcmp(tmpeleprof->PropertiesVariable.VariableValue[i], "None") == 0)
											strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], "");									
									}
									else
										strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], labelID);

									bUpdateMainString = true;
								}
								if (bSelected) ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
					}

					// additional option to create a new quest under Quest Choice
					bIsAQuestList = false;
					for (int v = 0; v < tmpeleprof->PropertiesVariable.iVariables; v++)
					{
						if (strstr(tmpeleprof->PropertiesVariable.Variable[v], "QuestChoice") != NULL)
						{
							bIsAQuestList = true;
							break;
						}
					}
					if (bIsAQuestList == true)
					{
						if (bAsString)
						{
							//PE: Locate index.
							for (int v = 0; v < g_collectionQuestList.size(); v++)
							{
								if (stricmp(g_collectionQuestList[v].collectionFields[0].Get(), tmpeleprof->PropertiesVariable.VariableValue[i]) == 0)
								{
									iQuestIndex = v + 2;
									break;
								}
							}
							if (iQuestIndex == 0)
							{
								//PE: Default to "none"
								if (pestrcasestr(tmpeleprof->aimain_s.Get(), "quest_poster") ||
									pestrcasestr(tmpeleprof->aimain_s.Get(), "quest_giver"))
								{
									iQuestIndex = 1;
								}
							}
						}
						else
						{
							iQuestIndex = atoi(tmpeleprof->PropertiesVariable.VariableValue[i]);
						}
						if (bUpdateMainString == true)
						{
							if (iQuestIndex == 1)
							{
								tmpeleprof->name_s = "Quest Giver";
							}
							if (iQuestIndex >= 2 && iQuestIndex <= 1 + g_collectionQuestList.size())
							{
								// if quest list selection, change the object name to identify the quest chosen
								tmpeleprof->name_s = g_collectionQuestList[iQuestIndex - 2].collectionFields[0];
							}
						}
					}
					bool bBelowQuestCombo = false;
					if (strstr(tmpeleprof->PropertiesVariable.Variable[i], "QuestChoice") != NULL)
					{
						bBelowQuestCombo = true;
					}
					if (bBelowQuestCombo && bIsAQuestList == true && iQuestIndex > 0)
					{
						bool bDoARefresh = false;
						float but_gadget_size = ImGui::GetFontSize() * 12.0;
						float w = ImGui::GetWindowContentRegionWidth() - 10.0;
						ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w * 0.5) - (but_gadget_size * 0.5), 0.0f));
						if (iQuestIndex == 1)
						{
							// prescan to ensure name unique
							bool bFoundMatch = false;
							for (int q = 0; q < g_collectionQuestList.size(); q++)
							{
								if (stricmp(g_collectionQuestList[q].collectionFields[0].Get(), tmpeleprof->name_s.Get()) == NULL)
								{
									bFoundMatch = true;
								}
							}

							if (ImGui::StyleButton("Quest Editor", ImVec2(but_gadget_size, 0)))
							{
								void CloseAllOpenTools(bool bTerrainTools = true);
								CloseAllOpenTools();
								extern std::vector<collectionQuestType> g_collectionQuestList_backup;
								g_collectionQuestList_backup.clear();
								extern bool bQuestEditor_Window;
								bQuestEditor_Window = true;
							}
							if (ImGui::IsItemHovered())
							{
								ImGui::SetTooltip("Open Quest Editor");
							}
							//PE: Now use "Quest Editor"
							/*
							LPSTR pCreateButtonLabel = "Create New Quest";
							if (ImGui::StyleButton(pCreateButtonLabel, ImVec2(but_gadget_size, 0)))
							{
								collectionQuestType item;
								fill_rpg_quest_defaults(&item, tmpeleprof->name_s.Get());

								//PE: Just to stop crash if not using storyboard.
								if (g_collectionQuestLabels.size() == 0)
								{
									bFoundMatch = false;
								}
								// only add unique quest titles
								if (bFoundMatch == false)
								{
									// add unique to quest list
									g_collectionQuestList.push_back(item);
									int iQuestIndexAdded = 1 + g_collectionQuestList.size();

									// inject into behaviour choice so can reflect as existing
									for (int n = 0; n < tmpeleprof->PropertiesVariable.iVariables; n++)
									{
										if (pestrcasestr(tmpeleprof->PropertiesVariable.Variable[n], "QuestChoice"))
										{
											sprintf(tmpeleprof->PropertiesVariable.VariableValue[n], "%d", iQuestIndexAdded);
											break;
										}
									}

									// refresh behaviour and the quest dropdown
									bDoARefresh = true;
								}
							}
							if (ImGui::IsItemHovered())
							{
								if(bFoundMatch==true)
									ImGui::SetTooltip("This quest name already exists in the main quest list!");
								else
									ImGui::SetTooltip("Use the name of this object to add a new quest to the main quest list");
							}
							*/
						}
						else
						{
							if (ImGui::StyleButton("Quest Editor", ImVec2(but_gadget_size, 0)))
							{
								extern int current_quest_selection; //iQuestIndex - 1
								int iIndex = iQuestIndex - 2;
								if (iIndex >= 0 && iIndex < g_collectionQuestList.size())
									current_quest_selection = iIndex;

								void CloseAllOpenTools(bool bTerrainTools = true);
								CloseAllOpenTools();
								extern std::vector<collectionQuestType> g_collectionQuestList_backup;
								g_collectionQuestList_backup.clear();
								extern bool bQuestEditor_Window;
								bQuestEditor_Window = true;
							}
							if (ImGui::IsItemHovered())
							{
								ImGui::SetTooltip("Open Quest Editor");
							}

							ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2((w * 0.5) - (but_gadget_size * 0.5), 0.0f));

							LPSTR pCreateButtonLabel = "Delete Quest";
							if (ImGui::StyleButton(pCreateButtonLabel, ImVec2(but_gadget_size, 0)))
							{
								int iAction = askBoxCancel("This will delete the current selected quest. Are you sure ?", "Quest Confirmation"); //1==Yes 2=Cancel 0=No
								if (iAction == 1)
								{
									// create new quest list without the one deleted
									std::vector<collectionQuestType> newCollectionQuestList;
									for (int q = 0; q < g_collectionQuestList.size(); q++)
									{
										if (q != iQuestIndex - 2)
										{
											newCollectionQuestList.push_back(g_collectionQuestList[q]);
										}
									}
									g_collectionQuestList = newCollectionQuestList;
									sprintf(tmpeleprof->PropertiesVariable.VariableValue[i], "%d", 1);
									bDoARefresh = true;
								}
							}
							if (ImGui::IsItemHovered()) ImGui::SetTooltip("Delete this quest from the main quest list of the game project");
						}
						if (bDoARefresh == true)
						{
							// refresh behaviour and the quest dropdown
							extern int fpe_current_loaded_script;
							extern bool g_bChangedGameCollectionList;
							fpe_current_loaded_script = -1;
							g_bChangedGameCollectionList = true;
							bUpdateMainString = true;
						}
					}

					// User needs to select a non-default next level.
					if (preview && strcmp(preview, "Go to Specific Level") == 0)
					{
						extern StoryboardStruct Storyboard;
						char title[MAX_PATH];
						if (strlen(tmpeleprof->ifused_s.Get()) > 0)
						{
							// Remove mapbank\ and .fpm from the level name.
							int offset = 0;
							if (strstr(tmpeleprof->ifused_s.Get(), "mapbank"))
								offset = 8;
							strcpy(title, tmpeleprof->ifused_s.Get() + offset);
							title[strlen(title) - 4] = 0;
						}
						else
							strcpy(title, "Select a level from your game project...");

						if (ImGui::BeginCombo("##LuaLevelCombo", title))
						{
							for (int i = 0; i < STORYBOARD_MAXNODES; i++)
							{
								if (Storyboard.Nodes[i].used && strlen(Storyboard.Nodes[i].level_name) > 0)
								{
									title[0] = 0;
									int offset = 0;
									if (strstr(Storyboard.Nodes[i].level_name, "mapbank"))
										offset = 8;
									strcpy(title, Storyboard.Nodes[i].level_name + offset);
									title[strlen(title) - 4] = 0;
									ImGui::PushID(91679 + i);
									if (ImGui::Selectable(title))
									{
										strcpy(tmpeleprof->ifused_s.Get(), Storyboard.Nodes[i].level_name);
									}
									ImGui::PopID();
								}
							}
							ImGui::EndCombo();
						}
					}
					else if (preview && stricmp(preview, "Use Storyboard Logic") == 0)
					{
						// Ensure that ifused_s gets wiped so that the next level in storyboard logic will be used, instead of specific level
						strcpy(tmpeleprof->ifused_s.Get(), "");
					}
				}
			}
			else if (tmpeleprof->PropertiesVariable.VariableType[i] == 5)
			{
				// integer - but converts seconds (from user) to milliseconds in actual stored value
				int tmpint = atoi(val.Get()) / 1000;
				LPSTR pTooltipForIntegerSlider = "Use slider to set the desired value.";
				if (stricmp (tmpeleprof->PropertiesVariable.Variable[i], "CombatTime") == NULL) pTooltipForIntegerSlider = "Set the time in seconds for how long the character stays in combat mode before returning to base";
				cstr id = cstr("##") + tmpeleprof->PropertiesVariable.VariableScript/*tmpeleprof->name_s*/ + cstr(tmpeleprof->PropertiesVariable.Variable[i]);
				if (tmpeleprof->PropertiesVariable.VariableValueFrom[i] > 0 && tmpeleprof->PropertiesVariable.VariableValueTo[i] > 0 && tmpeleprof->PropertiesVariable.VariableValueTo[i] > tmpeleprof->PropertiesVariable.VariableValueFrom[i])
				{
					if (ImGui::MaxSliderInputInt(id.Get(), &tmpint, (int)tmpeleprof->PropertiesVariable.VariableValueFrom[i], (int)tmpeleprof->PropertiesVariable.VariableValueTo[i], pTooltipForIntegerSlider))
					{
						sprintf(tmp, "%d", tmpint * 1000);
						strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], tmp);
						bUpdateMainString = true;
					}
				}
				else
				{
					if (ImGui::MaxSliderInputInt(id.Get(), &tmpint, 0.0F, 100.0f, pTooltipForIntegerSlider))
					{
						sprintf(tmp, "%d", tmpint * 1000);
						strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], tmp);
						bUpdateMainString = true;
					}
				}
			}
			else if (tmpeleprof->PropertiesVariable.VariableType[i] == 6)
			{
				// Should alter the matching eleprof variable value e.g. in script "&QUANTITY" is eleprof->quantity
				
				// Determine the index into g_DLuaVariableNames for this properties variable.
				int variableNameIndex = -1;
				for (int j = 0; j < MAXPROPERTIESVARIABLES; j++)
				{
					if (tmpeleprof->PropertiesVariable.VariableType[j] == 6)
					{
						variableNameIndex++;
						if (j == i)
							break;
					}
				}

				if (variableNameIndex >= 0 && variableNameIndex < g_DLuaVariableNames.size())
				{
					// Find the matching eleprof variable that we want to edit.
					std::string variableName = g_DLuaVariableNames[variableNameIndex];
					void* pVariable = nullptr;

					enum VariableType
					{
						NONE,
						INT,
						FLOAT,
						STRING,
						BOOL
					};
					VariableType type = NONE;
					
					if (stricmp(variableName.c_str(), "quantity") == 0)
					{
						type = INT;
						pVariable = &tmpeleprof->quantity;
					}
					else if (stricmp(variableName.c_str(), "damage") == 0)
					{
						type = INT;
						pVariable = &tmpeleprof->damage;
					}
					else if (stricmp(variableName.c_str(), "accuracy") == 0)
					{
						type = INT;
						pVariable = &tmpeleprof->accuracy;
					}
					else if (stricmp(variableName.c_str(), "reloadqty") == 0)
					{
						type = INT;
						pVariable = &tmpeleprof->reloadqty;
					}
					else if (stricmp(variableName.c_str(), "fireiterations") == 0)
					{
						type = INT;
						pVariable = &tmpeleprof->fireiterations;
					}
					else if (stricmp(variableName.c_str(), "range") == 0)
					{
						type = INT;
						pVariable = &tmpeleprof->range;
					}
					else if (stricmp(variableName.c_str(), "dropoff") == 0)
					{
						type = INT;
						pVariable = &tmpeleprof->dropoff;
					}
					else if (stricmp(variableName.c_str(), "clipcapacity") == 0)
					{
						type = INT;
						pVariable = &tmpeleprof->clipcapacity;
					}
					else if (stricmp(variableName.c_str(), "weaponpropres1") == 0)
					{
						type = INT;
						pVariable = &tmpeleprof->weaponpropres1;
					}
					else if (stricmp(variableName.c_str(), "weaponpropres2") == 0)
					{
						type = INT;
						pVariable = &tmpeleprof->weaponpropres2;
					}

					// Display the correct ImGui gadget based on the variable type.
					switch (type)
					{
					case INT:
					{
						int* pEditVariable = (int*)pVariable;
						if (pEditVariable)
						{
							// Choose a range for the slider (1,100 if not specified in script).
							int from = 1;
							int to = 100;
							int scriptFrom = tmpeleprof->PropertiesVariable.VariableValueFrom[i];
							int scriptTo = tmpeleprof->PropertiesVariable.VariableValueTo[i];
							if (scriptFrom != scriptTo)
							{
								from = scriptFrom;
								to = scriptTo;
							}

							// Edit the variable via ImGui
							cstr sliderLabel = cstr("##") + cstr((char*)variableName.c_str());
							ImGui::MaxSliderInputInt(sliderLabel.Get(), pEditVariable, from, to, 0);
						}
						break;
					}
					case FLOAT:
						break;
					case STRING:
						break;
					case BOOL:
						break;
					}
				}
			}
			else 
			{
				//Integer
				int tmpint = atoi(val.Get());
				// hack in tooltip for now, better idea is adding verbose to LUA itself!
				LPSTR pTooltipForIntegerSlider = "Use slider to set the desired value.";
				if (stricmp (tmpeleprof->PropertiesVariable.Variable[i], "RetreatRange") == NULL) pTooltipForIntegerSlider = "Set the distance the character will retreat to before stopping";
				if (stricmp (tmpeleprof->PropertiesVariable.Variable[i], "ChaseModes") == NULL) pTooltipForIntegerSlider = "The first three modes are slow walkers and the last two are fast walkers";
				cstr id = cstr("##") + tmpeleprof->PropertiesVariable.VariableScript/*tmpeleprof->name_s*/ + cstr(tmpeleprof->PropertiesVariable.Variable[i]);

				bool bwpeyoffet = false;
				bool bwpexoffet = false;
				bool bwpezoffet = false;

				if (bwpefile)
				{
					if (pestrcasestr(tmpeleprof->PropertiesVariable.Variable[i], "offsety"))
					{
						bwpeyoffet = true;
						fPreviewYOffset = 0;
						fPreviewXOffset = 0;
						fPreviewZOffset = 0;
					}
					if (pestrcasestr(tmpeleprof->PropertiesVariable.Variable[i], "offsetx"))
					{
						bwpexoffet = true;
					}
					if (pestrcasestr(tmpeleprof->PropertiesVariable.Variable[i], "offsetz"))
					{
						bwpezoffet = true;
					}
				}

				// strange condition to enable correct integer slider - from can be zero just fine
				//if (tmpeleprof->PropertiesVariable.VariableValueFrom[i] != 0 && tmpeleprof->PropertiesVariable.VariableValueTo[i] != 0 && tmpeleprof->PropertiesVariable.VariableValueTo[i] > tmpeleprof->PropertiesVariable.VariableValueFrom[i])
				if (bwpefile && (bwpeyoffet || bwpexoffet || bwpezoffet))
				{
					if (ImGui::MaxSliderInputInt(id.Get(), &tmpint, -100.0F, 100.0f, pTooltipForIntegerSlider))
					{
						sprintf(tmp, "%d", tmpint);
						strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], tmp);
						bUpdateMainString = true;
					}
				}
				else if (tmpeleprof->PropertiesVariable.VariableValueTo[i] != 0 && tmpeleprof->PropertiesVariable.VariableValueTo[i] > tmpeleprof->PropertiesVariable.VariableValueFrom[i])
				{
					if (ImGui::MaxSliderInputInt(id.Get(), &tmpint, (int)tmpeleprof->PropertiesVariable.VariableValueFrom[i], (int)tmpeleprof->PropertiesVariable.VariableValueTo[i], pTooltipForIntegerSlider))
					{
						sprintf(tmp, "%d", tmpint);
						strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], tmp);
						bUpdateMainString = true;
					}
				}
				else
				{
					if (ImGui::MaxSliderInputInt(id.Get(), &tmpint, 0.0F, 100.0f, pTooltipForIntegerSlider))
					{
						sprintf(tmp, "%d", tmpint);
						strcpy(tmpeleprof->PropertiesVariable.VariableValue[i], tmp);
						bUpdateMainString = true;
					}
				}

				if (bwpefile)
				{
					if (bwpeyoffet)
					{
						fPreviewYOffset = tmpint;
					}
					if (bwpexoffet)
					{
						fPreviewXOffset = tmpint;
					}
					if (bwpezoffet)
					{
						fPreviewZOffset = tmpint;
					}
				}

			}
			ImGui::PopItemWidth();
			ImGui::PopID();
		}
	}
	
	//Update soundset4_s when we have changes.
	if (bUpdateMainString) 
	{
		cstr sLuaScriptName = tmpeleprof->PropertiesVariable.VariableScript;
		sLuaScriptName += "_properties(";
		//Check if we need to update with new default values.
		if (tmpeleprof->PropertiesVariable.iVariables > 0) 
		{
			tmpeleprof->soundset4_s = sLuaScriptName;
			//Add variables.
			for (int i = 0; i < tmpeleprof->PropertiesVariable.iVariables; i++) 
			{
				char val[3];
				val[0] = tmpeleprof->PropertiesVariable.VariableType[i] + '0';
				val[1] = 0;

				tmpeleprof->soundset4_s += val;
				tmpeleprof->soundset4_s += "\"";
				std::string clean_string = tmpeleprof->PropertiesVariable.VariableValue[i];
				replaceAll(clean_string, "\"", ""); //cant use "
				tmpeleprof->soundset4_s += (char *)clean_string.c_str();
				//tmpeleprof->soundset4_s += tmpeleprof->PropertiesVariable.VariableValue[i];
				tmpeleprof->soundset4_s += "\"";
				if (i < tmpeleprof->PropertiesVariable.iVariables - 1)
					tmpeleprof->soundset4_s += ",";
			}
			tmpeleprof->soundset4_s += ")";
		}
	}

	return speech_entries;
}


int DisplayLuaDescriptionOnly(entityeleproftype *tmpeleprof)
{
	char tmp[2048];
	bool bUpdateMainString = false;
	int speech_entries = 0;
	float fPropertiesColoumWidth = ImGui::GetCursorPosX() + 110.0f;

	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	if (1) {
		ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_FrameBg]);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, style.FrameBorderSize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, style.FramePadding);

		bool ret;
		if (fDescriptionBoxSize > ImGui::GetFontSize() * 7) {
			ret = ImGui::BeginChild("##DLUADescriptionbox", ImVec2(ImGui::GetContentRegionAvailWidth() - 10, ImGui::GetFontSize() * 7), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding);
		}
		else {
			if (fDescriptionBoxSize == 0)
				ret = ImGui::BeginChild("##DLUADescriptionbox", ImVec2(ImGui::GetContentRegionAvailWidth() - 10, ImGui::GetFontSize() * 5), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
			else
				ret = ImGui::BeginChild("##DLUADescriptionbox", ImVec2(ImGui::GetContentRegionAvailWidth() - 10, fDescriptionBoxSize), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar);
		}
		void fDisplayDescriptionBox(entityeleproftype *tmpeleprof, bool textonly = false);
		fDisplayDescriptionBox(tmpeleprof,true);
		ImGui::EndChild();

		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor();
	}
	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 3));

	return 0;
}


void fDisplayDescriptionBox(entityeleproftype *tmpeleprof, bool textonly = false)
{
	struct Segment
	{
		Segment(const char* text, ImU32 col = 0, bool underline = false)
			: textStart(text)
			, textEnd(text + strlen(text))
			, colour(col)
			, underline(underline)
		{}

		const char* textStart;
		const char* textEnd;
		ImU32		colour;
		bool		underline;

		Segment() { colour = 0; underline = false; }

	};
	
	Segment segs[MAXPROPERTIESVARIABLES*3] = {};
	int curseg = 0;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	ImVec4 col = style.Colors[ImGuiCol_Text] * ImVec4(255.0f, 255.0f, 255.0f, 255.0f);

	if (textonly) 
	{
		segs[curseg++] = Segment(tmpeleprof->PropertiesVariable.VariableDescription.Get());
	}
	else 
	{
		for (int i = 0; i < tmpeleprof->PropertiesVariable.iVariables; i++)
		{
			//segs[curseg++] = Segment(tmpeleprof->PropertiesVariable.VariableSectionDescription[i]);
			segs[curseg++] = Segment(tmpeleprof->PropertiesVariable.VariableSectionDescription[i].Get());
			segs[curseg++] = Segment(tmpeleprof->PropertiesVariable.Variable[i], IM_COL32(col.x, col.y, col.z, col.w), true);
			//if (strlen(tmpeleprof->PropertiesVariable.VariableSectionEndDescription[i]) > 0) 
			//{
			//	segs[curseg++] = Segment(tmpeleprof->PropertiesVariable.VariableSectionEndDescription[i]);
			//}
			if (tmpeleprof->PropertiesVariable.VariableSectionEndDescription[i].Len() > 0)
			{
				segs[curseg++] = Segment(tmpeleprof->PropertiesVariable.VariableSectionEndDescription[i].Get());
			}
			if (curseg >= (MAXPROPERTIESVARIABLES * 3) - 1)
				break;
		}
	}
	float startpos = ImGui::GetCursorPosY();

	const float wrapWidth = ImGui::GetWindowContentRegionWidth();
	for (int i = 0; i < curseg; ++i)
	{
		const char* textStart = segs[i].textStart;
		const char* textEnd = segs[i].textEnd ? segs[i].textEnd : textStart + strlen(textStart);

		ImFont* Font = ImGui::GetFont();

		do
		{
			float widthRemaining = ImGui::CalcWrapWidthForPos(ImGui::GetCursorScreenPos(), 0.0f);
			widthRemaining *= 1.65;
			const char* drawEnd = ImGui::CalcWordWrapPositionB(1.0f, textStart, textEnd, wrapWidth, wrapWidth - widthRemaining); //, wrapWidth - widthRemaining);
			if (textStart == drawEnd)
			{
				ImGui::NewLine();
				drawEnd = ImGui::CalcWordWrapPositionB(1.0f, textStart, textEnd, wrapWidth, wrapWidth - widthRemaining); //, wrapWidth - widthRemaining);
			}

			if (segs[i].colour)
				ImGui::PushStyleColor(ImGuiCol_Text, segs[i].colour);
			ImGui::TextUnformatted(textStart, textStart == drawEnd ? nullptr : drawEnd);
			if (segs[i].colour)
				ImGui::PopStyleColor();
			if (segs[i].underline)
			{
				ImVec2 lineEnd = ImGui::GetItemRectMax();
				ImVec2 lineStart = lineEnd;
				lineStart.x = ImGui::GetItemRectMin().x;
				ImGui::GetWindowDrawList()->AddLine(lineStart, lineEnd, segs[i].colour);

				//if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
				//	ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
			}

			if (textStart == drawEnd || drawEnd == textEnd)
			{
				ImGui::SameLine(0.0f, 0.0f);
				break;
			}

			textStart = drawEnd;

			while (textStart < textEnd)
			{
				const char c = *textStart;
				if (ImCharIsBlankA(c)) { textStart++; }
				else if (c == '\n') { textStart++; break; }
				else { break; }
			}
		} while (true);
	}

	fDescriptionBoxSize = (ImGui::GetCursorPosY() - startpos) + (ImGui::GetFontSize()*2.0);

}

float fDisplaySegmentText(char *text)
{
	if (!text) return(0);
	struct Segment
	{
		Segment(const char* text, ImU32 col = 0, bool underline = false)
			: textStart(text)
			, textEnd(text + strlen(text))
			, colour(col)
			, underline(underline)
		{}

		const char* textStart;
		const char* textEnd;
		ImU32		colour;
		bool		underline;

		Segment() { colour = 0; underline = false; }

	};

	Segment segs[MAXPROPERTIESVARIABLES * 3] = {};
	int curseg = 0;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	ImVec4 col = style.Colors[ImGuiCol_Text] * ImVec4(255.0f, 255.0f, 255.0f, 255.0f);

	segs[curseg++] = Segment(text);
	
	float startpos = ImGui::GetCursorPosY();

	const float wrapWidth = ImGui::GetWindowContentRegionWidth();
	for (int i = 0; i < curseg; ++i)
	{
		const char* textStart = segs[i].textStart;
		const char* textEnd = segs[i].textEnd ? segs[i].textEnd : textStart + strlen(textStart);

		ImFont* Font = ImGui::GetFont();

		do
		{
			float widthRemaining = ImGui::CalcWrapWidthForPos(ImGui::GetCursorScreenPos(), 0.0f);
			widthRemaining *= 1.65;
			const char* drawEnd = ImGui::CalcWordWrapPositionB(1.0f, textStart, textEnd, wrapWidth, wrapWidth - widthRemaining); //, wrapWidth - widthRemaining);
			if (textStart == drawEnd)
			{
				ImGui::NewLine();
				drawEnd = ImGui::CalcWordWrapPositionB(1.0f, textStart, textEnd, wrapWidth, wrapWidth - widthRemaining); //, wrapWidth - widthRemaining);
			}

			if (segs[i].colour)
				ImGui::PushStyleColor(ImGuiCol_Text, segs[i].colour);
			ImGui::TextUnformatted(textStart, textStart == drawEnd ? nullptr : drawEnd);
			if (segs[i].colour)
				ImGui::PopStyleColor();
			if (segs[i].underline)
			{
				ImVec2 lineEnd = ImGui::GetItemRectMax();
				ImVec2 lineStart = lineEnd;
				lineStart.x = ImGui::GetItemRectMin().x;
				ImGui::GetWindowDrawList()->AddLine(lineStart, lineEnd, segs[i].colour);
			}

			if (textStart == drawEnd || drawEnd == textEnd)
			{
				ImGui::SameLine(0.0f, 0.0f);
				break;
			}

			textStart = drawEnd;

			while (textStart < textEnd)
			{
				const char c = *textStart;
				if (ImCharIsBlankA(c)) { textStart++; }
				else if (c == '\n') { textStart++; break; }
				else { break; }
			}
		} while (true);
	}
	return( (ImGui::GetCursorPosY() - startpos) + (ImGui::GetFontSize()*2.0) );
}

void UniversalKeyboardShortcutAddItem(int iIconID, int iIcon2ID, LPSTR pLabel)
{
	ImGui::Indent(10);
	// standard spacing settings
	int iRightColumn = 70;
	int iKeyIconSize = 26;
	float fShortcutTextSpacing = 5.0f;
	float fShortcutVerticalSpacing = -10.0f;

	if (iIconID > 0)
	{
		float fSize = 1.0f;
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x - 4.0f, ImGui::GetCursorPos().y));
	

		ImGui::ImgBtn(iIconID, ImVec2(iKeyIconSize*fSize, iKeyIconSize*fSize), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), 0, 0, 0, 0, false, false, false, false);
		LPSTR pTooltip = "";
		if (iIcon2ID != 0)
		{
			if (iIconID == KEY_TAB) pTooltip = "Hold down the TAB key";
			if (iIconID == KEY_SHIFT) pTooltip = "Hold down the SHIFT key";
			if (iIconID == KEY_CONTROL) pTooltip = "Hold down the CONTROL key";
			if (iIconID == KEY_CONTROL_SHIFT) pTooltip = "Hold down the CONTROL and SHIFT keys";
			if (iIconID == KEY_ALT) pTooltip = "Hold down the ALT key";
		}
		else
		{
			if (iIconID == KEY_TAB) pTooltip = "Press the TAB key";
			if (iIconID == KEY_SHIFT) pTooltip = "Press the SHIFT key";
			if (iIconID == KEY_CONTROL) pTooltip = "Press the CONTROL key";
			if (iIconID == KEY_CONTROL_SHIFT) pTooltip = "Press the CONTROL and SHIFT keys";
			if (iIconID == KEY_ALT) pTooltip = "Press the ALT key";

		}
		if (iIconID == KEY_KEYBOARD) pTooltip = "Use the WASD or the arrow keys";
		if (iIconID == MOUSE_LMB) pTooltip = "Use the left mouse button";
		if (iIconID == MOUSE_MMB) pTooltip = "Use the mouse wheel";
		if (iIconID == MOUSE_RMB) pTooltip = "Use the right mouse button";
		if (iIconID == KEY_BACKSPACE) pTooltip = "Press the BACKSPACE key";
		if (iIconID == KEY_R) pTooltip = "Press the R key";
		if (iIconID == KEY_Y) pTooltip = "Press the Y key";
		if (iIconID == KEY_F) pTooltip = "Press the F key";
		if (iIconID == KEY_G) pTooltip = "Press the G key";
		if (iIconID == KEY_I) pTooltip = "Press the I key";
		if (iIconID == KEY_N) pTooltip = "Press the N key";
		if (iIconID == KEY_L) pTooltip = "Press the L key";
		if (iIconID == KEY_E) pTooltip = "Press the E key";
		if (iIconID == KEY_Q) pTooltip = "Press the Q key";
		if (iIconID == KEY_SPACE) pTooltip = "Press the SPACE key";
		if (iIconID == KEY_DELETE) pTooltip = "Press the DELETE key";
		if (iIconID == KEY_RETURN) pTooltip = "Press the ENTER key";
		if (iIconID == KEY_PGUP) pTooltip = "Press the PAGEUP key";
		if (iIconID == KEY_PGDN) pTooltip = "Press the PAGEDOWN key";
		if (iIconID == KEY_O) pTooltip = "Press the O key";
		if (iIconID == KEY_T) pTooltip = "Press the T key";
		if (iIconID == KEY_Z) pTooltip = "Press the Z key";
		LPSTR pTooltip2 = "";
		ImGui::SameLine();
		if (iIcon2ID != 0)
		{
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x - 14.0f - ((fSize - 1.0f)*4.0f), ImGui::GetCursorPos().y));
			ImGui::ImgBtn(KEY_SEPARATOR_SMALL, ImVec2((float)iKeyIconSize*0.5*fSize, iKeyIconSize*fSize), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), 0, 0, 0, 0, false, false, false, false);
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x - 14.0f - ((fSize - 1.0f)*4.0f), ImGui::GetCursorPos().y));
			ImGui::ImgBtn(iIcon2ID, ImVec2(iKeyIconSize*fSize, iKeyIconSize*fSize), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), 0, 0, 0, 0, false, false, false, false);
			if (iIcon2ID == KEY_PLUS) pTooltip2 = "Press the PLUS key";
			if (iIcon2ID == KEY_MINUS) pTooltip2 = "Press the MINUS key";
			if (iIcon2ID == KEY_SHIFT) pTooltip2 = "Press the SHIFT key";
			if (iIcon2ID == MOUSE_LMB) pTooltip2 = "Use the left mouse button";
			if (iIcon2ID == MOUSE_RMB) pTooltip2 = "Use the right mouse button";
			if (iIcon2ID == KEY_BACKSPACE) pTooltip2 = "Press the BACKSPACE key";
			if (iIcon2ID == KEY_R) pTooltip2 = "Press the R key";
			if (iIcon2ID == KEY_G) pTooltip2 = "Press the G key";
			if (iIcon2ID == KEY_I) pTooltip2 = "Press the I key";
			if (iIcon2ID == KEY_SPACE) pTooltip2 = "Press the SPACE key";
			if (iIcon2ID == KEY_N) pTooltip2 = "Press the N key";
			if (iIcon2ID == KEY_L) pTooltip2 = "Press the L key";
			if (iIcon2ID == KEY_E) pTooltip2 = "Press the E key";
			if (iIcon2ID == KEY_DELETE) pTooltip2 = "Press the DELETE key";
			if (iIcon2ID == KEY_Y) pTooltip2 = "Press the Y key";
			if (iIcon2ID == KEY_RETURN) pTooltip2 = "Press the ENTER key";
			if (iIconID == KEY_O) pTooltip = "Press the O key";
			if (iIconID == KEY_T) pTooltip = "Press the T key";
			if (iIconID == KEY_Z) pTooltip = "Press the Z key";

			ImGui::SameLine();
		}
		int iNewRightColumn = iRightColumn;
		if (fSize > 1.0 && iIcon2ID != 0) iNewRightColumn = iRightColumn + 42.0f;
		ImGui::SetCursorPos(ImVec2(iNewRightColumn, ImGui::GetCursorPos().y + fShortcutTextSpacing));
		//if (bEnlargingKeyIcon == true)
		//{
			cstr pFullLabel = pTooltip;
			if (strlen(pTooltip2) > 0)
			{
				pFullLabel += " and ";
				pFullLabel += pTooltip2;
				//fShortcutVerticalSpacing += 12.0f;
			}
			else
			{
				//fShortcutVerticalSpacing += 8.0f;
			}
			pFullLabel += " to ";
			pFullLabel += pLabel;
			//pFullLabel += ".";
			//ImGui::TextWrapped(pFullLabel.Get());
		//}
		//else
		//{
			ImGui::TextWrapped(pLabel);

			if (ImGui::IsItemHovered()) ImGui::SetTooltip(pFullLabel.Get());
		//}
	}
	else
	{
		// separator
		//ImGui::Text("");
	}
	ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, fShortcutVerticalSpacing));
	ImGui::Indent(-10);
}

//void UniversalKeyboardShortcutAddItem(int iIconID, int iIcon2ID, LPSTR pLabel)
//{
//	// standard spacing settings
//	int iRightColumn = 60;
//	int iKeyIconSize = 26;
//	float fShortcutTextSpacing = 5.0f;
//	float fShortcutVerticalSpacing = -10.0f;
//
//	if (iIconID > 0)
//	{
//
//		float fSize = 1.0f;
//		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x - 4.0f, ImGui::GetCursorPos().y));
//		ImRect bbwin(ImGui::GetWindowPos()+ImGui::GetCursorPos()+ImVec2(0,-ImGui::GetScrollY()), ImGui::GetWindowPos() + ImGui::GetCursorPos() + ImVec2(0, -ImGui::GetScrollY()) + ImVec2(ImGui::GetWindowSize().x,ImGui::GetFontSize()*2.0));
//		// enlarge key icon if hover over it
//		bool bEnlargingKeyIcon = false;
//		if (ImGui::IsMouseHoveringRect(bbwin.Min, bbwin.Max))
//		{
//			//fSize = 1.75f;
//			ImGuiWindow* window = ImGui::GetCurrentWindow();
//			ImVec4 back_col = ImGui::GetStyle().Colors[ImGuiCol_ChildBg];
//			ImVec2 offset = { -10.0f,8.0f };
//			window->DrawList->AddRectFilled(bbwin.Min + offset, bbwin.Max + offset, ImGui::GetColorU32(back_col), 0.0f, 0.0f);
//			//bEnlargingKeyIcon = true;
//		}
//
//		ImGui::ImgBtn(iIconID, ImVec2(iKeyIconSize*fSize, iKeyIconSize*fSize), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), 0, 0, 0, 0, false, false, false, false);
//		LPSTR pTooltip = "";
//		if (iIcon2ID != 0)
//		{
//			if (iIconID == KEY_TAB) pTooltip = "Hold down the TAB key";
//			if (iIconID == KEY_SHIFT) pTooltip = "Hold down the SHIFT key";
//			if (iIconID == KEY_CONTROL) pTooltip = "Hold down the CONTROL key";
//			if (iIconID == KEY_CONTROL_SHIFT) pTooltip = "Hold down the CONTROL and SHIFT keys";
//			if (iIconID == KEY_ALT) pTooltip = "Hold down the ALT key";
//		}
//		else
//		{
//			if (iIconID == KEY_TAB) pTooltip = "Press the TAB key";
//			if (iIconID == KEY_SHIFT) pTooltip = "Press the SHIFT key";
//			if (iIconID == KEY_CONTROL) pTooltip = "Press the CONTROL key";
//			if (iIconID == KEY_CONTROL_SHIFT) pTooltip = "Press the CONTROL and SHIFT keys";
//			if (iIconID == KEY_ALT) pTooltip = "Press the ALT key";
//		}
//		if (iIconID == KEY_KEYBOARD) pTooltip = "Use the W, A, S and D keys";
//		if (iIconID == MOUSE_LMB) pTooltip = "Use the left mouse button";
//		if (iIconID == MOUSE_MMB) pTooltip = "Use the mouse wheel";
//		if (iIconID == MOUSE_RMB) pTooltip = "Use the right mouse button";
//		if (iIconID == KEY_BACKSPACE) pTooltip = "Press the BACKSPACE key";
//		if (iIconID == KEY_R) pTooltip = "Press the R key";
//		if (iIconID == KEY_Y) pTooltip = "Press the Y key";
//		if (iIconID == KEY_F) pTooltip = "Press the F key";
//		if (iIconID == KEY_G) pTooltip = "Press the G key";
//		if (iIconID == KEY_N) pTooltip = "Press the N key";
//		if (iIconID == KEY_L) pTooltip = "Press the L key";
//		if (iIconID == KEY_E) pTooltip = "Press the E key";
//		if (iIconID == KEY_SPACE) pTooltip = "Press the SPACE key";
//		if (iIconID == KEY_DELETE) pTooltip = "Press the DELETE key";
//		if (iIconID == KEY_RETURN) pTooltip = "Press the ENTER key";
//		if (iIconID == KEY_PGUP) pTooltip = "Press the PAGEUP key";
//		if (iIconID == KEY_PGDN) pTooltip = "Press the PAGEDOWN key";
//		LPSTR pTooltip2 = "";
//		ImGui::SameLine();
//		if (iIcon2ID != 0)
//		{
//			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x - 14.0f - ((fSize-1.0f)*4.0f), ImGui::GetCursorPos().y));
//			ImGui::ImgBtn(KEY_SEPARATOR_SMALL, ImVec2((float)iKeyIconSize*0.5*fSize, iKeyIconSize*fSize), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), 0, 0, 0, 0, false, false, false, false);
//			ImGui::SameLine();
//			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x - 14.0f - ((fSize - 1.0f)*4.0f), ImGui::GetCursorPos().y));
//			ImGui::ImgBtn(iIcon2ID, ImVec2(iKeyIconSize*fSize, iKeyIconSize*fSize), ImVec4(0.0, 0.0, 0.0, 0.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), ImVec4(1.0, 1.0, 1.0, 1.0), 0, 0, 0, 0, false, false, false, false);
//			if (iIcon2ID == KEY_PLUS) pTooltip2 = "Press the PLUS key";
//			if (iIcon2ID == KEY_MINUS) pTooltip2 = "Press the MINUS key";
//			if (iIcon2ID == KEY_SHIFT) pTooltip2 = "Press the SHIFT key";
//			if (iIcon2ID == MOUSE_LMB) pTooltip2 = "Use the left mouse button";
//			if (iIcon2ID == MOUSE_RMB) pTooltip2 = "Use the right mouse button";
//			if (iIcon2ID == KEY_BACKSPACE) pTooltip2 = "Press the BACKSPACE key";
//			if (iIcon2ID == KEY_R) pTooltip2 = "Press the R key";
//			if (iIcon2ID == KEY_G) pTooltip2 = "Press the G key";
//			if (iIcon2ID == KEY_SPACE) pTooltip2 = "Press the SPACE key";
//			if (iIcon2ID == KEY_N) pTooltip2 = "Press the N key";
//			if (iIcon2ID == KEY_L) pTooltip2 = "Press the L key";
//			if (iIcon2ID == KEY_E) pTooltip2 = "Press the E key";
//			if (iIcon2ID == KEY_DELETE) pTooltip2 = "Press the DELETE key";
//			if (iIcon2ID == KEY_Y) pTooltip2 = "Press the Y key";
//			if (iIcon2ID == KEY_RETURN) pTooltip2 = "Press the ENTER key";
//			ImGui::SameLine();
//		}
//		int iNewRightColumn = iRightColumn;
//		if (fSize > 1.0 && iIcon2ID != 0) iNewRightColumn = iRightColumn + 42.0f;
//		ImGui::SetCursorPos(ImVec2(iNewRightColumn, ImGui::GetCursorPos().y + fShortcutTextSpacing));
//		if (bEnlargingKeyIcon == true)
//		{
//			cstr pFullLabel = pTooltip;
//			if (strlen(pTooltip2) > 0)
//			{
//				pFullLabel += " and ";
//				pFullLabel += pTooltip2;
//				fShortcutVerticalSpacing += 12.0f;
//			}
//			else
//			{
//				fShortcutVerticalSpacing += 8.0f;
//			}
//			pFullLabel += " to ";
//			pFullLabel += pLabel;
//			pFullLabel += ".";
//			ImGui::TextWrapped(pFullLabel.Get());
//		}
//		else
//		{
//			ImGui::TextWrapped(pLabel);
//		}
//	}
//	else
//	{
//		// separator
//		ImGui::Text("");
//	}
//	ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, fShortcutVerticalSpacing));
//}

int iContentHeight[eKST_Last]; //PE: Support multiply different panels open at the same time.
float fLastContentHeight[eKST_Last];

void UniversalKeyboardShortcut(eKeyboardShortcutType KST)
{

	if (pref.iHideKeyboardShortcuts)
		return;

	extern int iLastOpenHeader;
	extern bool bStoryboardWindow;
	int iHeader = 30;
	if (KST == eKST_ObjectMode)
		iHeader = 20;

	if (!bStoryboardWindow && pref.bAutoClosePropertySections && iLastOpenHeader != iHeader)
		ImGui::SetNextItemOpen(false, ImGuiCond_Always);

	// called to create Keyboard Shortcut component for all software
	if (ImGui::StyleCollapsingHeader("Keyboard Shortcuts", ImGuiTreeNodeFlags_DefaultOpen))
	{
		iLastOpenHeader = iHeader;

		ImVec2 vChildSize = { 0,0 };
		if (fLastContentHeight[KST] > 20)
			vChildSize.y = fLastContentHeight[KST];

		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
		ImGui::BeginChild("##KeyboardShortcutsChild", vChildSize, false, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_None | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
		ImGui::PopStyleColor();
		iContentHeight[KST] = 0;
		// indent to start list of shortcuts
		ImGui::Indent(10);
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 4.0f));

		// record start of keyboard shortcut content
		float fStartOfKeyboardShortcutContent = ImGui::GetCursorPos().y;

		// additional common controls for specific modes
		bool bShowCameraViewShortcuts = false;
		if (KST == eKST_Sculpt || KST == eKST_Paint || KST == eKST_AddVeg || KST == eKST_ObjectMode) bShowCameraViewShortcuts = true;
		if (bShowCameraViewShortcuts == true)
		{
			ImGui::Text("General Shortcuts"); iContentHeight[KST]++;
			//UniversalKeyboardShortcutAddItem(0, 0, ""); iContentHeight[KST]++; //PE: Removed for consistency with other sub sections.
			UniversalKeyboardShortcutAddItem(KEY_KEYBOARD, 0, "Move camera around the map"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_E, 0, "Move camera up"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_Q, 0, "Move camera down"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_SHIFT, 0, "Move camera around quickly"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_CONTROL, 0, "Move camera around slowly"); iContentHeight[KST]++;
			extern bool bEditorInFreeFlightMode;
			if (bEditorInFreeFlightMode == true)
			{
				UniversalKeyboardShortcutAddItem(MOUSE_RMB, 0, "Rotate camera view"); iContentHeight[KST]++;
			}
			UniversalKeyboardShortcutAddItem(MOUSE_MMB, 0, "Zoom in and out"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_O, 0, "Enable object tools"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_T, 0, "Enable terrain tools"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_F, 0, "Toggle camera view"); iContentHeight[KST]++;

		}

		float fSectionSpacer = 8.0f;

		// shortcuts for each type
		switch (KST)
		{
		case eKST_Sculpt:
			if( bShowCameraViewShortcuts ) ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, fSectionSpacer));
			ImGui::Text("Sculpt Shortcuts"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(MOUSE_LMB, 0, "Raise and Lower the terrain"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_TAB, 0, "Toggle between raising and lowering"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_PLUS, 0, "Increase the brush size"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_MINUS, 0, "Decrease the brush size"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_CONTROL, MOUSE_MMB, "Increase/Decrease the brush size"); iContentHeight[KST]++;
			break;
		case eKST_Paint:
			if (bShowCameraViewShortcuts) ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, fSectionSpacer));
			ImGui::Text("Paint Shortcuts"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(MOUSE_LMB, 0, "Paint and clear terrain texture"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_TAB, 0, "Toggle between painting modes"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_PLUS, 0, "Increase the brush size"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_MINUS, 0, "Decrease the brush size"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_CONTROL, MOUSE_MMB, "Increase/Decrease the brush size"); iContentHeight[KST]++;
			break;
		case eKST_AddVeg:
			if (bShowCameraViewShortcuts) ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, fSectionSpacer));
			ImGui::Text("Vegetation Shortcuts"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(MOUSE_LMB, 0, "Add and remove terrain vegetation"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_TAB, 0, "Toggle between adding and removing"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_PLUS, 0, "Increase the brush size"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_MINUS, 0, "Decrease the brush size"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_CONTROL, MOUSE_MMB, "Increase/Decrease the brush size"); iContentHeight[KST]++;
			break;
		case eKST_ObjectMode:
			if (bShowCameraViewShortcuts) ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, fSectionSpacer));
			ImGui::Text("Object Tools Shortcuts"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(MOUSE_LMB, 0, "Drag camera around"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_SHIFT, MOUSE_LMB, "Select multiple objects"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_R, 0, "Rotate selected object"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_SHIFT, KEY_R, "Rotate object quickly"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_CONTROL, KEY_R, "Rotate object slowly"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_TAB, 0, "Toggle between positioning modes"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_DELETE, 0, "Delete selected object(s)"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_CONTROL, KEY_Z, "Undo"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_CONTROL, KEY_Y, "Redo"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_Y, 0, "Toggle object static and dynamic"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_G, 0, "Toggle between grid modes"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_I, 0, "Toggle spray objects"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_RETURN, 0, "Place object on nearest surface"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_PGUP, 0, "Move object upward"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_PGDN, 0, "Move object downward"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_CONTROL, KEY_G, "Group selected objects"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_CONTROL_SHIFT, KEY_G, "UnGroup selected objects"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_CONTROL, KEY_L, "Lock/unlock selected object"); iContentHeight[KST]++;

			iContentHeight[KST]++; //Extra for wrapping.
			iContentHeight[KST]++; //Extra for wrapping.
			iContentHeight[KST]++; //Extra for wrapping.
			break;
		case eKST_CharacterCreator:
			if (bShowCameraViewShortcuts) ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, fSectionSpacer));
			ImGui::Text("Character Creator Shortcuts"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_KEYBOARD, 0, "Move the camera around"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(MOUSE_RMB, 0, "Look around with the camera"); iContentHeight[KST]++;
			break;
		case eKST_ObjectLibrary:
		case eKST_TerrainGenerator:
			if (bShowCameraViewShortcuts) ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, fSectionSpacer));
			if (KST == eKST_TerrainGenerator)
			{
				ImGui::Text("Terrain Generator Shortcuts"); iContentHeight[KST]++;
				UniversalKeyboardShortcutAddItem(KEY_KEYBOARD, 0, "Move camera around the map"); iContentHeight[KST]++;

			}
			else
			{
				ImGui::Text("Object Library Shortcuts"); iContentHeight[KST]++;
			}
			UniversalKeyboardShortcutAddItem(MOUSE_LMB, 0, "Move the terrain preview"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(MOUSE_RMB, 0, "Rotate the terrain preview"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(MOUSE_MMB, 0, "Zoom in and out of the terrain preview"); iContentHeight[KST]++;
			break;
		case eKST_Storyboard:
			if (bShowCameraViewShortcuts) ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, fSectionSpacer));
			ImGui::Text("Storyboard Shortcuts"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(MOUSE_LMB, 0, "Drag screen around"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_SHIFT, MOUSE_LMB, "Select multiple nodes"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_CONTROL, KEY_N, "Add new level"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_CONTROL, KEY_L, "Add existing level"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_CONTROL, KEY_SPACE, "Play game"); iContentHeight[KST]++;
			UniversalKeyboardShortcutAddItem(KEY_CONTROL, KEY_E, "Save standalone"); iContentHeight[KST]++;

			break;
		}

		// restore indent
		ImGui::Indent(-10);

		float fShortcutTextSpacing = 6.6f;
		//PE: TODO - We need to calcutate wrapping lines also for this to be precise.
		//PE: We need some additional height when we are using wrapping.
		if(iContentHeight[KST] <= 5)
			fLastContentHeight[KST] = (float) (iContentHeight[KST] + 2) * (ImGui::GetFontSize()+ fShortcutTextSpacing);
		else
			fLastContentHeight[KST] = (float)(iContentHeight[KST] + 5) * (ImGui::GetFontSize() + fShortcutTextSpacing);

		if (bShowCameraViewShortcuts) fLastContentHeight[KST] += fSectionSpacer;

		// finished panel
		ImGui::EndChild();
	}
}

void coreResetIMGUIFunctionalityPrefs(void)
{
	// this is called when MAXVERSION incremented
	// and restores/resets all flags so that each NEW build
	// provides users with a view of the STANDARD USER MOD£
	// not advanced or developer tools modes
	pref.current_style = 25; // 0
	pref.bHideTutorials = false;
	pref.bMultiplyOpenHeaders = false;
	pref.bAutoClosePropertySections = true;
	pref.bDisableMultipleViewport = false;
	pref.bAutoOpenMenuItems = true;
	pref.iAllowUndocking = false;
	pref.iTurnOffUITransparent = false;
	pref.iEnableDragDropEntityMode = 1;
	pref.iEnableAdvancedSky = 0;
	pref.iEnableAdvancedWater = 0;
	pref.iEnableAdvancedPostProcessing = 0;
	pref.iEnableAdvancedShadows = 0;
	pref.iEnableArcRelationshipLines = 1;
	pref.iEnableRelationPopupWindow = 1;// 0; we want you back ;)
	pref.iEnableAxisRotationShortcuts = 0;
	pref.iObjectEnableAdvanced = 0;
	pref.iEnableDragDropWidgetSelect = 0;
	pref.iEnableEditorOutlineSelection = 1;
	pref.iEnableSingleRightPanelAdvanced = 0;
	pref.iDragCameraMovement = 1;
	pref.iGameCreaterStore = 0;
	pref.iFullscreenPreviewAdvanced = 0;
	pref.iDisplayWelcomeScreen = 1;
	pref.iDisplayIntroScreen = 1;
	pref.iEnableAdvancedEntityList = 0;
	pref.fHighLightThickness = 1.0;
	pref.iTurnOffEditboxTooltip = false;
	pref.iImporterDome = 1;
	pref.iEnableAutoExposureInEditor = 0;
	pref.iSetColumnsEntityLib = 3;
	pref.iTerrainAdvanced = 0;
	pref.iStoryboardAdvanced = 0;
	pref.iAdvancedGridModeSettings = 0;// iTerrainDebugMode = 0;
	pref.iEnableAdvancedCharacterCreator = 0;
	pref.iDisableProjectAutoSave = 0;
	pref.iDisableLevelAutoSave = 0;
	pref.iEnableFpsMemMonitor = 0;
	pref.iEnableAutoFlattenSystem = 1;

	for (int i = 0; i < 10; i++)
		pref.iCheckboxFilters[i] = 1;

	//pref.iDisableObjectLibraryViewport = false;
	pref.iLastInStoryboard = false;
	strcpy(pref.cLastUsedStoryboardProject, "");
	pref.changelog_ftime = 0;

	pref.iHideKeyboardShortcuts = 0;
	pref.iEnableDeveloperObjectTools = 0;
	pref.iEnableLevelEditorOpenAndNew = 0;
	pref.iDisplayTerrainGeneratorWelcome = 1;
	pref.iTestGameGraphicsQuality = 2;
	pref.iEnableAutoFlattenSystem = 1;
	pref.iEnableAdvancedGrass = 0;

	pref.iEnableDragDropStopSelectFromInside = 0;
	pref.iGridMode = 0;
	pref.iGridEnabled = 0;
	pref.fEditorGridOffsetX = 50.0f;
	pref.fEditorGridOffsetY = 0.0f;
	pref.fEditorGridOffsetZ = 50.0f;
	pref.fEditorGridSizeX = 100.0f;
	pref.fEditorGridSizeY = 10.0f;
	pref.fEditorGridSizeZ = 100.0f;
	pref.iDevToolsOpen = 0;
	extern int g_iDevToolsOpen;
	g_iDevToolsOpen = 0;
	pref.iCheckFilesModifiedOnFocus = 1;
	pref.status_bar_color = ImVec4((1.0f / 255.0f) * 14, (1.0f / 255.0f) * 99, (1.0f / 255.0f) * 156, 1.0);
	pref.highlight_color = pref.status_bar_color;
}

void DrawRubberBand(float fx, float fy, float fx2, float fy2 )
{
	ImGuiViewport* mainviewport = ImGui::GetMainViewport();
	if (mainviewport)
	{
		ImDrawList* dl = ImGui::GetForegroundDrawList(mainviewport);
		if (dl)
		{
			ImRect bb;
			bb.Min.x = fx;
			bb.Min.y = fy;
			bb.Max.x = fx2;
			bb.Max.y = fy2;
			dl->AddRect(bb.Min,bb.Max, ImGui::GetColorU32(ImVec4(1.0,1.0,1.0,0.8)), 0.0f, ImDrawCornerFlags_None, 3.0f);
		}
	}

}
