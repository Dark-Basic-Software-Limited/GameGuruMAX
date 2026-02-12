//#pragma optimize("", off)

//#define DISABLETEMP

#include "stdafx.h"
#include "../../../../WickedEngineDX12/WickedEngine/WickedEngine.h"
#include "TracerManager.h"
#include "wiResourceManager.h"
#include "wiTimer.h"
#include "preprocessor-moreflags.h"
#include "gameguru.h"
#include "Utility/tinyddsloader.h"
#ifdef OPTICK_ENABLE
#include "optick.h"
#endif


template<typename T> static inline T PELerp(T a, T b, float t) { return (T)(a + (b - a) * t); }

//PE: Tracers will now follow gunid (400) Added 100 for LUA only use.
#define MAXTRACERS 400
#define MAXLUATRACERS 100

namespace Tracers
{
    std::vector<Tracer> tracers;
    GPUBuffer constantBuffer;
    Texture tracerTexture[MAXTRACERS + MAXLUATRACERS];
    PipelineState tracerPSO;
    GPUBuffer quadVB;
    GPUBuffer quadIB;


    float gameTime = 0.0f;
    wiTimer mytimer;
    Sampler samplerTrilinearWrap;
    Sampler samplerTrilinearClamp;
    BlendState blendStatesAdditive;
    Shader shaderTracerVS;
    Shader shaderTracerPS;
    RasterizerState rasterizerState;
    DepthStencilState depthStencilState;

    struct Vertex { XMFLOAT3 pos; XMFLOAT2 uv; };
    Vertex vertices[] = {
        { XMFLOAT3(0, -0.5f, 0), XMFLOAT2(0, 1) },
        { XMFLOAT3(1, -0.5f, 0), XMFLOAT2(1, 1) },
        { XMFLOAT3(0,  0.5f, 0), XMFLOAT2(0, 0) },
        { XMFLOAT3(1,  0.5f, 0), XMFLOAT2(1, 0) },
    };
    uint16_t indices[] = { 0, 1, 2, 2, 1, 3 };

    wiGraphics::FORMAT ConvertDDSFormat(tinyddsloader::DDSFile::DXGIFormat format)
    {
        switch (format)
        {
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_Float: return FORMAT_R32G32B32A32_FLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_UInt: return FORMAT_R32G32B32A32_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_SInt: return FORMAT_R32G32B32A32_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_Float: return FORMAT_R32G32B32_FLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_UInt: return FORMAT_R32G32B32_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_SInt: return FORMAT_R32G32B32_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_Float: return FORMAT_R16G16B16A16_FLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_UNorm: return FORMAT_R16G16B16A16_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_UInt: return FORMAT_R16G16B16A16_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_SNorm: return FORMAT_R16G16B16A16_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_SInt: return FORMAT_R16G16B16A16_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32_Float: return FORMAT_R32G32_FLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32_UInt: return FORMAT_R32G32_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32_SInt: return FORMAT_R32G32_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_UNorm: return FORMAT_R10G10B10A2_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_UInt: return FORMAT_R10G10B10A2_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R11G11B10_Float: return FORMAT_R11G11B10_FLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8X8_UNorm: return FORMAT_B8G8R8A8_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm: return FORMAT_B8G8R8A8_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm_SRGB: return FORMAT_B8G8R8A8_UNORM_SRGB;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm: return FORMAT_R8G8B8A8_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm_SRGB: return FORMAT_R8G8B8A8_UNORM_SRGB;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UInt: return FORMAT_R8G8B8A8_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SNorm: return FORMAT_R8G8B8A8_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SInt: return FORMAT_R8G8B8A8_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_Float: return FORMAT_R16G16_FLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_UNorm: return FORMAT_R16G16_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_UInt: return FORMAT_R16G16_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_SNorm: return FORMAT_R16G16_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_SInt: return FORMAT_R16G16_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::D32_Float: return FORMAT_D32_FLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32_Float: return FORMAT_R32_FLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32_UInt: return FORMAT_R32_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32_SInt: return FORMAT_R32_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_UNorm: return FORMAT_R8G8_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_UInt: return FORMAT_R8G8_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_SNorm: return FORMAT_R8G8_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_SInt: return FORMAT_R8G8_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16_Float: return FORMAT_R16_FLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::D16_UNorm: return FORMAT_D16_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16_UNorm: return FORMAT_R16_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16_UInt: return FORMAT_R16_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16_SNorm: return FORMAT_R16_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16_SInt: return FORMAT_R16_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R8_UNorm: return FORMAT_R8_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8_UInt: return FORMAT_R8_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R8_SNorm: return FORMAT_R8_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8_SInt: return FORMAT_R8_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::A8_UNorm: return FORMAT_R8_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm: return FORMAT_BC1_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm_SRGB: return FORMAT_BC1_UNORM_SRGB;
        case tinyddsloader::DDSFile::DXGIFormat::BC2_UNorm: return FORMAT_BC2_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::BC2_UNorm_SRGB: return FORMAT_BC2_UNORM_SRGB;
        case tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm: return FORMAT_BC3_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm_SRGB: return FORMAT_BC3_UNORM_SRGB;
        case tinyddsloader::DDSFile::DXGIFormat::BC4_UNorm: return FORMAT_BC4_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::BC4_SNorm: return FORMAT_BC4_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::BC5_UNorm: return FORMAT_BC5_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::BC5_SNorm: return FORMAT_BC5_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm: return FORMAT_BC7_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm_SRGB: return FORMAT_BC7_UNORM_SRGB;
        default:
            assert(0); // incoming format is not supported 
            return FORMAT_UNKNOWN;
        }
    }

    void Tracer_LoadTextureDDS(const char* filename, Texture* tex)
    {
        GraphicsDevice* device = wi::graphics::GetDevice();

        char filePath[MAX_PATH];
        strcpy_s(filePath, MAX_PATH, filename);
        GG_GetRealPath(filePath, 0);

        tinyddsloader::DDSFile dds;
        auto result = dds.Load(filePath);

        if (result != tinyddsloader::Result::Success) return;

        TextureDesc desc;
        desc.ArraySize = 1;
        desc.BindFlags = BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.Width = dds.GetWidth();
        desc.Height = dds.GetHeight();
        desc.Depth = dds.GetDepth();
        desc.MipLevels = dds.GetMipCount();
        desc.ArraySize = dds.GetArraySize();
        desc.MiscFlags = 0;
        desc.Usage = USAGE_IMMUTABLE;
        desc.layout = IMAGE_LAYOUT_SHADER_RESOURCE;
        desc.Format = ConvertDDSFormat(dds.GetFormat());

        std::vector<SubresourceData> InitData;
        for (uint32_t arrayIndex = 0; arrayIndex < desc.ArraySize; ++arrayIndex)
        {
            for (uint32_t mip = 0; mip < desc.MipLevels; ++mip)
            {
                auto imageData = dds.GetImageData(mip, arrayIndex);
                SubresourceData subresourceData;
                subresourceData.pSysMem = imageData->m_mem;
                subresourceData.SysMemPitch = imageData->m_memPitch;
                subresourceData.SysMemSlicePitch = imageData->m_memSlicePitch;
                InitData.push_back(subresourceData);
            }
        }

        auto dim = dds.GetTextureDimension();
        switch (dim)
        {
        case tinyddsloader::DDSFile::TextureDimension::Texture1D: desc.type = TextureDesc::TEXTURE_1D; break;
        case tinyddsloader::DDSFile::TextureDimension::Texture2D: desc.type = TextureDesc::TEXTURE_2D; break;
        case tinyddsloader::DDSFile::TextureDimension::Texture3D: desc.type = TextureDesc::TEXTURE_3D; break;
        default: assert(0); break;
        }

        device->CreateTexture(&desc, InitData.data(), tex);
    }

    void LoadTracerImage(char * filename , uint32_t gunid)
    {
        Tracer_LoadTextureDDS(filename, &tracerTexture[gunid]);
    }
    
    void Initialize()
    {
#ifdef DISABLETEMP
        return;
#endif
        GraphicsDevice* device = wi::graphics::GetDevice();
        //PE: Load tracer texture
        //PE: Moved to gunfolder/tracer.dds
        //Tracer_LoadTextureDDS("files/gamecore/tracers/tracer1.dds", &tracerTexture[0]);
        //Tracer_LoadTextureDDS("files/gamecore/tracers/tracer2.dds", &tracerTexture[1]);

        //PE: Create constant buffer
        GPUBufferDesc cbd;
        cbd.Usage = USAGE_DYNAMIC;
        cbd.ByteWidth = sizeof(TracerCB);
        cbd.BindFlags = BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = CPU_ACCESS_WRITE;
        device->CreateBuffer(&cbd, nullptr, &constantBuffer);

        SamplerDesc samplerDesc;
        samplerDesc.AddressU = TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = TEXTURE_ADDRESS_WRAP;
        samplerDesc.Filter = FILTER_MIN_MAG_MIP_LINEAR;
        device->CreateSampler(&samplerDesc, &samplerTrilinearWrap);

        samplerDesc.AddressU = TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = TEXTURE_ADDRESS_CLAMP;
        samplerDesc.Filter = FILTER_MIN_MAG_MIP_LINEAR;
        device->CreateSampler(&samplerDesc, &samplerTrilinearClamp);

        BlendState bd;
        //PE: Additive for now.
        bd.RenderTarget[0].BlendEnable = true;
        bd.RenderTarget[0].SrcBlend = BLEND_SRC_ALPHA;
        bd.RenderTarget[0].DestBlend = BLEND_ONE;
        bd.RenderTarget[0].BlendOp = BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlendAlpha = BLEND_ZERO;
        bd.RenderTarget[0].DestBlendAlpha = BLEND_ONE;
        bd.RenderTarget[0].BlendOpAlpha = BLEND_OP_ADD;
        bd.RenderTarget[0].RenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
        bd.IndependentBlendEnable = false;
        blendStatesAdditive = bd;

        /*
        //Alpha
        bd.RenderTarget[0].BlendEnable = true;
        bd.RenderTarget[0].SrcBlend = BLEND_SRC_ALPHA;
        bd.RenderTarget[0].DestBlend = BLEND_INV_SRC_ALPHA;
        bd.RenderTarget[0].BlendOp = BLEND_OP_ADD;
        bd.RenderTarget[0].SrcBlendAlpha = BLEND_ONE;
        bd.RenderTarget[0].DestBlendAlpha = BLEND_INV_SRC_ALPHA;
        bd.RenderTarget[0].BlendOpAlpha = BLEND_OP_ADD;
        bd.RenderTarget[0].RenderTargetWriteMask = COLOR_WRITE_ENABLE_ALL;
        bd.IndependentBlendEnable = false;
        blendStatesAdditive = bd;
        */

        RasterizerState rs;
        rs.FillMode = FILL_SOLID;
        rs.CullMode = CULL_NONE;
        rs.FrontCounterClockwise = true;
        rs.DepthBias = 0;
        rs.DepthBiasClamp = 0;
        rs.SlopeScaledDepthBias = 0;
        rs.DepthClipEnable = false;
        rs.MultisampleEnable = false;
        rs.AntialiasedLineEnable = false;
        rasterizerState = rs;

        DepthStencilState dsd;
        dsd.DepthEnable = true;
        dsd.DepthWriteMask = DEPTH_WRITE_MASK_ZERO;
        dsd.DepthFunc = COMPARISON_GREATER_EQUAL;
        dsd.StencilEnable = false;
        depthStencilState = dsd;

        //PE: Create quad mesh and pipeline state
        CreateQuad();
        CreatePipelineState();
    }

    void AddTracer(const XMFLOAT3& start, const XMFLOAT3& end, float lifeTime, XMFLOAT4 color, float glow, float scroll, float scaleV, float width, float max_length, uint32_t tracerID)
    {
#ifdef DISABLETEMP
        return;
#endif
        tracers.push_back({ start, end, gameTime, lifeTime, color, glow, scroll ,scaleV, width, max_length, tracerID });
    }


    void Update()
    {
#ifdef DISABLETEMP
        return;
#endif
        float currentTime = gameTime;

        tracers.erase(std::remove_if(tracers.begin(), tracers.end(), [currentTime](const Tracer& tracer)
            {
                return (currentTime - tracer.spawnTime) > tracer.lifeTime;
            }), tracers.end());

        float deltaTime = float(std::max(0.0, mytimer.elapsed() / 1000.0));
        if (deltaTime > (1.0f / 20.0f)) deltaTime = (1.0f / 20.0f);
        mytimer.record();

        gameTime += deltaTime;
    }


    extern "C" void tracer_draw(const wiScene::CameraComponent& camera, wiGraphics::CommandList cmd)
    {
#ifdef OPTICK_ENABLE
        OPTICK_EVENT();
#endif

#ifdef DISABLETEMP
        return;
#endif
        Draw(cmd, camera.GetViewProjection());
        Tracers::Update();
    }

    void Draw(CommandList cmd, const XMMATRIX& viewProj)
    {
#ifdef DISABLETEMP
        return;
#endif
        if (tracers.empty()) return;

        wiRenderer::BindCommonResources(cmd);

        wiScene::CameraComponent& camera = wiScene::GetCamera();
        const XMMATRIX myViewProj = camera.GetViewProjection();
        
        GraphicsDevice* device = wi::graphics::GetDevice();
        device->EventBegin("tracer Draw", cmd);
        device->BindPipelineState(&tracerPSO, cmd);

        for (const auto& tracer : tracers)
        {
            XMVECTOR start = XMLoadFloat3(&tracer.startPos);
            XMVECTOR end = XMLoadFloat3(&tracer.endPos);
            XMVECTOR dir = XMVectorSubtract(end, start);
            float length = XMVectorGetX(XMVector3Length(dir)); //Hit weapon ? *0.97;
            dir = XMVector3Normalize(dir);

            //PE: Rotate quad to face camera.
            const XMFLOAT3 cameraPos = camera.Eye;
            XMVECTOR mid = XMVectorLerp(start, end, 0.5f);
            XMVECTOR toCamera = XMVectorSubtract(XMLoadFloat3(&cameraPos), mid);
            toCamera = XMVector3Normalize(toCamera);

            //PE: Rotation matrix
            XMVECTOR right = XMVector3Cross(dir, toCamera);
            right = XMVector3Normalize(right);
            XMVECTOR up = XMVector3Cross(right, dir);

            const float width = tracer.width;

            float lifeFraction = (gameTime - (tracer.spawnTime + t.ElapsedTime_f)) / tracer.lifeTime;
            float alpha = std::clamp(1.0f - lifeFraction,0.0f, 1.0f);

            float max_width = tracer.max_length; // 50
            if (max_width > 0)
            {
                length = max_width;
            }

            XMMATRIX rotation = XMMATRIX(
                right * width,
                dir * length,
                up * width,
                XMVectorSet(0, 0, 0, 1)
            );

            //PE: Move middle point to start position using length.
            XMMATRIX translation;
            if (max_width > 0)
            {
                XMVECTOR newstart = XMVectorLerp(start, end - (dir * (max_width)), 1.0 - alpha);
                translation = XMMatrixTranslationFromVector(newstart + (dir * (length * 0.5f)));
            }
            else
                translation = XMMatrixTranslationFromVector(start + (dir * (length * 0.5f)));

            XMMATRIX world = rotation * translation;

            //PE: Final World-View-Projection matrix
            XMMATRIX wvp = world * myViewProj;

            alpha = PELerp(alpha, 1.0f, alpha); //PE: Make it fade faster at the end.

            TracerCB cb;
            cb.g_mWorldViewProj = XMMatrixTranspose(wvp);
            cb.g_TintColor = XMFLOAT4(tracer.color.x, tracer.color.y, tracer.color.z, tracer.color.w * alpha);
            cb.g_GlowIntensity = tracer.glowIntensity;
            cb.g_ScrollSpeed = tracer.scrollSpeed;
            cb.g_Time = gameTime;
            cb.g_ScaleV = tracer.scaleV;

            uint32_t bindSlot = 2;
            device->UpdateBuffer(&constantBuffer, &cb, cmd);
            device->BindConstantBuffer(VS, &constantBuffer, bindSlot, cmd);
            device->BindConstantBuffer(PS, &constantBuffer, bindSlot, cmd);
            uint32_t tID = tracer.tracerID;
            if (tID > MAXTRACERS + MAXLUATRACERS) tID = 0;
            if (tracerTexture[tID].IsValid())
                device->BindResource(PS, &tracerTexture[tID], 0, cmd);
            else
                device->BindResource(PS, wiTextureHelper::getWhite() , 0, cmd);

            device->BindResource(PS, &tracerTexture[tID], 0, cmd);
            device->BindSampler(PS, &samplerTrilinearWrap, 0, cmd);

            const GPUBuffer* vbs[] = { &quadVB };
            const UINT strides[] = { sizeof(XMFLOAT3) + sizeof(XMFLOAT2) };
            const UINT offsets[] = { 0 };
            device->BindVertexBuffers(vbs, 0, 1, strides, 0, cmd);
            device->BindIndexBuffer(&quadIB, INDEXFORMAT_16BIT, 0, cmd);
            device->DrawIndexed(6, 0, 0, cmd);
        }

        wiRenderer::BindCommonResources(cmd);

        device->EventEnd(cmd);

    }

    void CreateQuad()
    {
        GPUBufferDesc bd = {};
        SubresourceData data = {};
        data.pSysMem = vertices;
        bd.Usage = USAGE_IMMUTABLE;
        bd.ByteWidth = sizeof(vertices);
        bd.BindFlags = BIND_VERTEX_BUFFER;
        wi::graphics::GetDevice()->CreateBuffer(&bd, &data, &quadVB);

        data.pSysMem = indices;
        bd.ByteWidth = sizeof(indices);
        bd.BindFlags = BIND_INDEX_BUFFER;
        wi::graphics::GetDevice()->CreateBuffer(&bd, &data, &quadIB);
    }

    void CreatePipelineState()
    {
        PipelineStateDesc desc;
        wiRenderer::LoadShader(VS, shaderTracerVS, "BulletTracerVS.cso");
        wiRenderer::LoadShader(PS, shaderTracerPS, "BulletTracerPS.cso");
        desc.ps = &shaderTracerPS;
        desc.vs = &shaderTracerVS;
        desc.bs = &blendStatesAdditive;
        desc.rs = &rasterizerState;
        desc.dss = &depthStencilState;

        InputLayout layout;
        layout.elements = {
            { "POSITION", 0, FORMAT_R32G32B32_FLOAT, 0, 0, INPUT_PER_VERTEX_DATA },
            { "TEXCOORD", 0, FORMAT_R32G32_FLOAT, 0, 12, INPUT_PER_VERTEX_DATA }
        };
        desc.il = &layout;

        wi::graphics::GetDevice()->CreatePipelineState(&desc, &tracerPSO);
    }

}