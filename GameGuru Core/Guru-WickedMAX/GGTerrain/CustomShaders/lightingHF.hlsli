#ifndef WI_LIGHTING_HF
#define WI_LIGHTING_HF
#include "globals.hlsli"
#include "brdf.hlsli"
#include "voxelConeTracingHF.hlsli"
#include "skyHF.hlsli"

#define DISTANCE_BASED_SHADOWS
//#define SEVEN_SHADOW_SAMPLES //PE: Test
#ifdef BRDF_CARTOON
#define DISABLE_SOFT_SHADOWMAP
#endif // BRDF_CARTOON

struct LightingPart
{
	float3 diffuse;
	float3 specular;
};
struct Lighting
{
	LightingPart direct;
	LightingPart indirect;

	inline void create(
		in float3 diffuse_direct,
		in float3 specular_direct,
		in float3 diffuse_indirect,
		in float3 specular_indirect
	)
	{
		direct.diffuse = diffuse_direct;
		direct.specular = specular_direct;
		indirect.diffuse = diffuse_indirect;
		indirect.specular = specular_indirect;
	}
};

// Combine the direct and indirect lighting into final contribution
inline LightingPart CombineLighting(in Surface surface, in Lighting lighting)
{
	LightingPart result;
	result.diffuse = lighting.direct.diffuse + lighting.indirect.diffuse * surface.occlusion;
	result.specular = lighting.direct.specular + lighting.indirect.specular * surface.occlusion;

	return result;
}

inline float shadowCmpLookup( Texture2DArray<float> shadowMap, float3 coords, float dist, float4 interp, float scaleFactor )
{
	float4 depthSamples = shadowMap.Gather( sampler_point_clamp, coords );
    float4 shadows = depthSamples > dist;
    shadows *= saturate( (depthSamples-dist)*scaleFactor );
    return dot( shadows.wzxy, interp );
}

inline float3 shadowCascade(in ShaderEntity light, in float3 shadowPos, in float2 shadowUV, in uint cascade)
{
	const float slice = light.GetTextureIndex() + cascade;
	const float realDistance = shadowPos.z; // bias was already applied when shadow map was rendered
	float scaleFactor = 65536.0 / (cascade + 1);
	float3 shadow = 0;

#ifndef DISABLE_SOFT_SHADOWMAP
	float4 interp;
    interp.xy = frac( shadowUV.xy * g_xFrame_ShadowRes2D - 0.5 );
    interp.zw = 1 - interp.xy;
    interp = interp.zxzx * interp.wwyy;
		
	shadow.x += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2(-1, -1) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
	shadow.x += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2(-1,  0) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
	shadow.x += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2(-1,  1) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
#ifndef SEVEN_SHADOW_SAMPLES
	shadow.x += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2( 0, -1) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
#endif
	shadow.x += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV, slice), realDistance, interp, scaleFactor );
#ifndef SEVEN_SHADOW_SAMPLES
	shadow.x += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2( 0,  1) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
#endif
	shadow.x += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2( 1, -1) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
	shadow.x += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2( 1,  0) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
	shadow.x += shadowCmpLookup( texture_shadowarray_2d, float3(shadowUV + float2( 1,  1) * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor );
#ifndef SEVEN_SHADOW_SAMPLES
	shadow.x = 1 - (shadow.x / 9.0);
#else
	shadow.x = 1 - (shadow.x / 7.0);
#endif
	shadow = shadow.xxx;
	
#else
	// sample smoothed depth buffer
	float depth = texture_shadowarray_2d.SampleLevel(sampler_linear_clamp, float3(shadowUV, slice), 0);
	shadow.x = depth > realDistance;
	shadow.x *= saturate( (depth-realDistance)*scaleFactor );
	shadow.x = 1 - shadow.x;
	shadow = shadow.xxx;
#endif // DISABLE_SOFT_SHADOWMAP

#ifndef DISABLE_TRANSPARENT_SHADOWMAP
	if (g_xFrame_Options & OPTION_BIT_TRANSPARENTSHADOWS_ENABLED)
	{
		float4 transparent_shadow = texture_shadowarray_transparent_2d.SampleLevel(sampler_linear_clamp, float3(shadowUV, slice), 0);
#ifdef TRANSPARENT_SHADOWMAP_SECONDARY_DEPTH_CHECK
		if (transparent_shadow.a > realDistance)
#endif // TRANSPARENT_SHADOWMAP_SECONDARY_DEPTH_CHECK
		{
			shadow *= transparent_shadow.rgb;
		}
	}
#endif //DISABLE_TRANSPARENT_SHADOWMAP

	return shadow;
}

//PE: List of best point to sample based on distance.
static const float2 bestsamplepoints[9] =
{
   float2(0.0f, 0.0f), // PCF
   float2(-1.0f, -1.0f), // PCF
   float2(1.0f, 1.0f), // PCF
   float2(1.0f, -1.0f), // PCF
   float2(-1.0f, 1.0f), // PCF
   float2(1, 0.0f), // PCF
   float2(-1.0f, 0.0f), // PCF
   float2(0.0f, -1.0f), // PCF
   float2(0.0f, 1.0f), // PCF
};

inline float3 shadowCascade(in ShaderEntity light, in float3 shadowPos, in float2 shadowUV, in uint cascade,in float dist)
{
	const float slice = light.GetTextureIndex() + cascade;
	const float realDistance = shadowPos.z; // bias was already applied when shadow map was rendered
	float scaleFactor = 65536.0 / (cascade + 1);
	float3 shadow = 0;

#ifndef DISABLE_SOFT_SHADOWMAP
	float4 interp;
	interp.xy = frac(shadowUV.xy * g_xFrame_ShadowRes2D - 0.5);
	interp.zw = 1 - interp.xy;
	interp = interp.zxzx * interp.wwyy;

	int samplenum = (7 - min((int)dist * 0.003, 7)) + 1; //PE: Reduced to 7 samples, nearly no diff. Produce samplenum 1-7 based on distance.

	//if (samplenum == 8) return(0.0, 0.0, 0.0); //PE: Display range of full sampling. where they split samplenum.
	//if (samplenum == 7) return(1.0, 1.0, 1.0);
	//if (samplenum == 6) return(0.0, 0.0, 0.0);
	//if (samplenum == 5) return(1.0, 1.0, 1.0);
	//if (samplenum == 4) return(0.0, 0.0, 0.0);
	//if (samplenum == 3) return(1.0, 1.0, 1.0);
	//if (samplenum == 2) return(0.0, 0.0, 0.0);
	//if (samplenum == 1) return(1.0, 1.0, 1.0);
	//return(1.0, 1.0, 1.0);

	for (int i = 0; i < samplenum; i++)
	{
		shadow.x += shadowCmpLookup(texture_shadowarray_2d, float3(shadowUV + bestsamplepoints[i] * g_xFrame_ShadowKernel2D, slice), realDistance, interp, scaleFactor);
	}

	shadow.x = 1 - (shadow.x / samplenum);
	shadow = shadow.xxx;

#else
	// sample smoothed depth buffer
	float depth = texture_shadowarray_2d.SampleLevel(sampler_linear_clamp, float3(shadowUV, slice), 0);
	shadow.x = depth > realDistance;
	shadow.x *= saturate((depth - realDistance)*scaleFactor);
	shadow.x = 1 - shadow.x;
	shadow = shadow.xxx;
#endif // DISABLE_SOFT_SHADOWMAP

#ifndef DISABLE_TRANSPARENT_SHADOWMAP
	if (g_xFrame_Options & OPTION_BIT_TRANSPARENTSHADOWS_ENABLED)
	{
		float4 transparent_shadow = texture_shadowarray_transparent_2d.SampleLevel(sampler_linear_clamp, float3(shadowUV, slice), 0);
#ifdef TRANSPARENT_SHADOWMAP_SECONDARY_DEPTH_CHECK
		if (transparent_shadow.a > realDistance)
#endif // TRANSPARENT_SHADOWMAP_SECONDARY_DEPTH_CHECK
		{
			shadow *= transparent_shadow.rgb;
		}
	}
#endif //DISABLE_TRANSPARENT_SHADOWMAP

	return shadow;
}

inline float3 shadowCascadeSpot(in ShaderEntity light, in float3 shadowPos, in float2 shadowUV, in uint cascade)
{
	const float slice = light.GetTextureIndex() + cascade;
	const float realDistance = shadowPos.z; // bias was already applied when shadow map was rendered
	float scaleFactor = 65536.0 / (cascade + 1);
	float3 shadow = 0;

#ifndef DISABLE_SOFT_SHADOWMAP
	float4 interp;
	interp.xy = frac(shadowUV.xy * g_xFrame_ShadowResSpot2D - 0.5);
	interp.zw = 1 - interp.xy;
	interp = interp.zxzx * interp.wwyy;

	shadow.x += shadowCmpLookup(texture_shadowarray_Spot_2d, float3(shadowUV + float2(-1, -1) * g_xFrame_ShadowKernelSpot2D, slice), realDistance, interp, scaleFactor);
	shadow.x += shadowCmpLookup(texture_shadowarray_Spot_2d, float3(shadowUV + float2(-1, 0) * g_xFrame_ShadowKernelSpot2D, slice), realDistance, interp, scaleFactor);
	shadow.x += shadowCmpLookup(texture_shadowarray_Spot_2d, float3(shadowUV + float2(-1, 1) * g_xFrame_ShadowKernelSpot2D, slice), realDistance, interp, scaleFactor);
#ifndef SEVEN_SHADOW_SAMPLES
	shadow.x += shadowCmpLookup(texture_shadowarray_Spot_2d, float3(shadowUV + float2(0, -1) * g_xFrame_ShadowKernelSpot2D, slice), realDistance, interp, scaleFactor);
#endif
	shadow.x += shadowCmpLookup(texture_shadowarray_Spot_2d, float3(shadowUV, slice), realDistance, interp, scaleFactor);
#ifndef SEVEN_SHADOW_SAMPLES
	shadow.x += shadowCmpLookup(texture_shadowarray_Spot_2d, float3(shadowUV + float2(0, 1) * g_xFrame_ShadowKernelSpot2D, slice), realDistance, interp, scaleFactor);
#endif
	shadow.x += shadowCmpLookup(texture_shadowarray_Spot_2d, float3(shadowUV + float2(1, -1) * g_xFrame_ShadowKernelSpot2D, slice), realDistance, interp, scaleFactor);
	shadow.x += shadowCmpLookup(texture_shadowarray_Spot_2d, float3(shadowUV + float2(1, 0) * g_xFrame_ShadowKernelSpot2D, slice), realDistance, interp, scaleFactor);
	shadow.x += shadowCmpLookup(texture_shadowarray_Spot_2d, float3(shadowUV + float2(1, 1) * g_xFrame_ShadowKernelSpot2D, slice), realDistance, interp, scaleFactor);
#ifndef SEVEN_SHADOW_SAMPLES
	shadow.x = 1 - (shadow.x / 9.0);
#else
	shadow.x = 1 - (shadow.x / 7.0);
#endif
	shadow = shadow.xxx;

#else
	// sample smoothed depth buffer
	float depth = texture_shadowarray_Spot_2d.SampleLevel(sampler_linear_clamp, float3(shadowUV, slice), 0);
	shadow.x = depth > realDistance;
	shadow.x *= saturate((depth - realDistance)*scaleFactor);
	shadow.x = 1 - shadow.x;
	shadow = shadow.xxx;
#endif // DISABLE_SOFT_SHADOWMAP

#ifndef DISABLE_TRANSPARENT_SHADOWMAP
	if (g_xFrame_Options & OPTION_BIT_TRANSPARENTSHADOWS_ENABLED)
	{
		float4 transparent_shadow = texture_shadowarray_transparent_Spot_2d.SampleLevel(sampler_linear_clamp, float3(shadowUV, slice), 0);
#ifdef TRANSPARENT_SHADOWMAP_SECONDARY_DEPTH_CHECK
		if (transparent_shadow.a > realDistance)
#endif // TRANSPARENT_SHADOWMAP_SECONDARY_DEPTH_CHECK
		{
			shadow *= transparent_shadow.rgb;
		}
	}
#endif //DISABLE_TRANSPARENT_SHADOWMAP

	return shadow;
}

inline float3 shadowCascadeSpot(in ShaderEntity light, in float3 shadowPos, in float2 shadowUV, in uint cascade,in float dist)
{
	const float slice = light.GetTextureIndex() + cascade;
	const float realDistance = shadowPos.z; // bias was already applied when shadow map was rendered
	float scaleFactor = 65536.0 / (cascade + 1);
	float3 shadow = 0;

#ifndef DISABLE_SOFT_SHADOWMAP
	float4 interp;
	interp.xy = frac(shadowUV.xy * g_xFrame_ShadowResSpot2D - 0.5);
	interp.zw = 1 - interp.xy;
	interp = interp.zxzx * interp.wwyy;

	int samplenum = (7 - min((int)dist * 0.003, 7)) + 1; //PE: Produce samplenum 1-7 based on distance.

	//if (samplenum == 6) return(0.0, 0.0, 0.0); //PE: Display range of full sampling. where they split samplenum.
	//if (samplenum == 5) return(1.0, 1.0, 1.0);
	//if (samplenum == 4) return(0.0, 0.0, 0.0);
	//if (samplenum == 3) return(1.0, 1.0, 1.0);
	//if (samplenum == 2) return(0.0, 0.0, 0.0);
	//if (samplenum == 1) return(1.0, 1.0, 1.0);
	//return(1.0, 1.0, 1.0);

	for (int i = 0; i < samplenum; i++)
	{
		shadow.x += shadowCmpLookup(texture_shadowarray_Spot_2d, float3(shadowUV + bestsamplepoints[i] * g_xFrame_ShadowKernelSpot2D, slice), realDistance, interp, scaleFactor);
	}
	shadow.x = 1 - (shadow.x / samplenum);
	shadow = shadow.xxx;

#else
	// sample smoothed depth buffer
	float depth = texture_shadowarray_Spot_2d.SampleLevel(sampler_linear_clamp, float3(shadowUV, slice), 0);
	shadow.x = depth > realDistance;
	shadow.x *= saturate((depth - realDistance)*scaleFactor);
	shadow.x = 1 - shadow.x;
	shadow = shadow.xxx;
#endif // DISABLE_SOFT_SHADOWMAP

#ifndef DISABLE_TRANSPARENT_SHADOWMAP
	if (g_xFrame_Options & OPTION_BIT_TRANSPARENTSHADOWS_ENABLED)
	{
		float4 transparent_shadow = texture_shadowarray_transparent_Spot_2d.SampleLevel(sampler_linear_clamp, float3(shadowUV, slice), 0);
#ifdef TRANSPARENT_SHADOWMAP_SECONDARY_DEPTH_CHECK
		if (transparent_shadow.a > realDistance)
#endif // TRANSPARENT_SHADOWMAP_SECONDARY_DEPTH_CHECK
		{
			shadow *= transparent_shadow.rgb;
		}
	}
#endif //DISABLE_TRANSPARENT_SHADOWMAP

	return shadow;
}

inline float shadowCmpLookupCube( TextureCubeArray<float> shadowMap, float4 coords, float dist, float4 interp, float scaleFactor )
{
	float4 depthSamples = shadowMap.Gather( sampler_point_clamp, coords );
    float4 shadows = depthSamples > dist;
    shadows *= saturate( (depthSamples-dist)*scaleFactor );
    return dot( shadows.wzxy, interp );
}

inline float3 shadowCube(in ShaderEntity light, in float3 L, in float3 Lunnormalized)
{
	const float slice = light.GetTextureIndex();
	float remappedDistance = light.GetCubemapDepthRemapNear() +
		light.GetCubemapDepthRemapFar() / (max(max(abs(Lunnormalized.x), abs(Lunnormalized.y)), abs(Lunnormalized.z)) * 0.989); // little bias to avoid border sampling artifact
	float3 shadow = 0;
#ifndef DISABLE_SOFT_SHADOWMAP
	// sample along a cube pattern around center:
	L = -L;
	
	shadow.x += texture_shadowarray_cube.SampleCmpLevelZero(sampler_cmp_depth, float4(L + float3(-1, -1, -1) * g_xFrame_ShadowKernelCube, slice), remappedDistance);
	shadow.x += texture_shadowarray_cube.SampleCmpLevelZero(sampler_cmp_depth, float4(L + float3(1, -1, -1) * g_xFrame_ShadowKernelCube, slice), remappedDistance);
	shadow.x += texture_shadowarray_cube.SampleCmpLevelZero(sampler_cmp_depth, float4(L + float3(-1, 1, -1) * g_xFrame_ShadowKernelCube, slice), remappedDistance);
#ifndef SEVEN_SHADOW_SAMPLES
	shadow.x += texture_shadowarray_cube.SampleCmpLevelZero(sampler_cmp_depth, float4(L + float3(1, 1, -1) * g_xFrame_ShadowKernelCube, slice), remappedDistance);
#endif
	shadow.x += texture_shadowarray_cube.SampleCmpLevelZero(sampler_cmp_depth, float4(L, slice), remappedDistance);
#ifndef SEVEN_SHADOW_SAMPLES
	shadow.x += texture_shadowarray_cube.SampleCmpLevelZero(sampler_cmp_depth, float4(L + float3(-1, -1, 1) * g_xFrame_ShadowKernelCube, slice), remappedDistance);
#endif
	shadow.x += texture_shadowarray_cube.SampleCmpLevelZero(sampler_cmp_depth, float4(L + float3(1, -1, 1) * g_xFrame_ShadowKernelCube, slice), remappedDistance);
	shadow.x += texture_shadowarray_cube.SampleCmpLevelZero(sampler_cmp_depth, float4(L + float3(-1, 1, 1) * g_xFrame_ShadowKernelCube, slice), remappedDistance);
	shadow.x += texture_shadowarray_cube.SampleCmpLevelZero(sampler_cmp_depth, float4(L + float3(1, 1, 1) * g_xFrame_ShadowKernelCube, slice), remappedDistance);
#ifndef SEVEN_SHADOW_SAMPLES
	shadow.x /= 9.0;
#else
	shadow.x /= 7.0;
#endif
	shadow = shadow.xxx;
	
	/*
	float4 interp;
    interp.xy = frac( shadowUV.xy * g_xFrame_ShadowResCube - 0.5 );
    interp.zw = 1 - interp.xy;
    interp = interp.zxzx * interp.wwyy;

	float scaleFactor = 65536.0 / (cascade + 1.0);

	shadow.x += shadowCmpLookupCube( texture_shadowarray_cube, float4(L + float3(-1, -1, -1) * g_xFrame_ShadowKernelCube, slice), remappedDistance, interp, scaleFactor );
	shadow.x += shadowCmpLookupCube( texture_shadowarray_cube, float4(L + float3( 1, -1, -1) * g_xFrame_ShadowKernelCube, slice), remappedDistance, interp, scaleFactor );
	shadow.x += shadowCmpLookupCube( texture_shadowarray_cube, float4(L + float3(-1,  1, -1) * g_xFrame_ShadowKernelCube, slice), remappedDistance, interp, scaleFactor );
	shadow.x += shadowCmpLookupCube( texture_shadowarray_cube, float4(L + float3( 1,  1, -1) * g_xFrame_ShadowKernelCube, slice), remappedDistance, interp, scaleFactor );
	shadow.x += shadowCmpLookupCube( texture_shadowarray_cube, float4(L, slice), remappedDistance, interp, scaleFactor );
	shadow.x += shadowCmpLookupCube( texture_shadowarray_cube, float4(L + float3(-1, -1,  1) * g_xFrame_ShadowKernelCube, slice), remappedDistance, interp, scaleFactor );
	shadow.x += shadowCmpLookupCube( texture_shadowarray_cube, float4(L + float3( 1, -1,  1) * g_xFrame_ShadowKernelCube, slice), remappedDistance, interp, scaleFactor );
	shadow.x += shadowCmpLookupCube( texture_shadowarray_cube, float4(L + float3(-1,  1,  1) * g_xFrame_ShadowKernelCube, slice), remappedDistance, interp, scaleFactor );
	shadow.x += shadowCmpLookupCube( texture_shadowarray_cube, float4(L + float3( 1,  1,  1) * g_xFrame_ShadowKernelCube, slice), remappedDistance, interp, scaleFactor );
	shadow.x = 1 - (shadow.x / 9.0);
	shadow = shadow.xxx;
	*/
#else
	shadow = texture_shadowarray_cube.SampleCmpLevelZero(sampler_cmp_depth, float4(-Lunnormalized, slice), remappedDistance).r;
#endif // DISABLE_SOFT_SHADOWMAP

#ifndef DISABLE_TRANSPARENT_SHADOWMAP
	if (g_xFrame_Options & OPTION_BIT_TRANSPARENTSHADOWS_ENABLED)
	{
		float4 transparent_shadow = texture_shadowarray_transparent_cube.SampleLevel(sampler_linear_clamp, float4(-Lunnormalized, slice), 0);
#ifdef TRANSPARENT_SHADOWMAP_SECONDARY_DEPTH_CHECK
		if (transparent_shadow.a > remappedDistance)
#endif // TRANSPARENT_SHADOWMAP_SECONDARY_DEPTH_CHECK
		{
			shadow *= transparent_shadow.rgb;
		}
	}
#endif //DISABLE_TRANSPARENT_SHADOWMAP

	return shadow;
}


static const float3 bestsamplepointscube[9] =
{
   float3(0, 0, 0), // PCF
   float3(-1, -1, -1), // PCF
   float3(1, 1, 1), // PCF
   float3(1, -1, -1), // PCF
   float3(-1, 1, 1), // PCF
   float3(-1, 1, -1), // PCF
   float3(1, -1, 1), // PCF
   float3(1, 1, -1), // PCF
   float3(-1, -1, 1), // PCF
};

inline float3 shadowCube(in ShaderEntity light, in float3 L, in float3 Lunnormalized,in float dist)
{
	const float slice = light.GetTextureIndex();
	float remappedDistance = light.GetCubemapDepthRemapNear() +
		light.GetCubemapDepthRemapFar() / (max(max(abs(Lunnormalized.x), abs(Lunnormalized.y)), abs(Lunnormalized.z)) * 0.989); // little bias to avoid border sampling artifact
	float3 shadow = 0;
#ifndef DISABLE_SOFT_SHADOWMAP
	// sample along a cube pattern around center:
	L = -L;

	int samplenum = (7 - min((int)dist * 0.003, 7)) + 1; //PE: Produce samplenum 1-7 based on distance.

	//if (samplenum == 6) return(0.0, 0.0, 0.0); //PE: Display range of full sampling. where they split samplenum.
	//if (samplenum == 5) return(1.0, 1.0, 1.0);
	//if (samplenum == 4) return(0.0, 0.0, 0.0);
	//if (samplenum == 3) return(1.0, 1.0, 1.0);
	//if (samplenum == 2) return(0.0, 0.0, 0.0);
	//if (samplenum == 1) return(1.0, 1.0, 1.0);
	//return(1.0, 1.0, 1.0);

	for (int i = 0; i < samplenum; i++)
	{
		shadow.x += texture_shadowarray_cube.SampleCmpLevelZero(sampler_cmp_depth, float4(L + bestsamplepointscube[i] * g_xFrame_ShadowKernelCube, slice), remappedDistance);
	}
	shadow.x /= samplenum;
	shadow = shadow.xxx;

#else
	shadow = texture_shadowarray_cube.SampleCmpLevelZero(sampler_cmp_depth, float4(-Lunnormalized, slice), remappedDistance).r;
#endif // DISABLE_SOFT_SHADOWMAP

#ifndef DISABLE_TRANSPARENT_SHADOWMAP
	if (g_xFrame_Options & OPTION_BIT_TRANSPARENTSHADOWS_ENABLED)
	{
		float4 transparent_shadow = texture_shadowarray_transparent_cube.SampleLevel(sampler_linear_clamp, float4(-Lunnormalized, slice), 0);
#ifdef TRANSPARENT_SHADOWMAP_SECONDARY_DEPTH_CHECK
		if (transparent_shadow.a > remappedDistance)
#endif // TRANSPARENT_SHADOWMAP_SECONDARY_DEPTH_CHECK
		{
			shadow *= transparent_shadow.rgb;
		}
	}
#endif //DISABLE_TRANSPARENT_SHADOWMAP

	return shadow;
}

// Caustic pattern from: https://www.shadertoy.com/view/XtKfRG
inline float caustic_pattern(float2 uv, float time)
{
	float3 k = float3(uv, time);
	float3x3 m = float3x3(-2, -1, 2, 3, -2, 1, 1, 2, 2);
	float3 a = mul(k, m) * 0.5;
	float3 b = mul(a, m) * 0.4;
	float3 c = mul(b, m) * 0.3;
	return pow(min(min(length(0.5 - frac(a)), length(0.5 - frac(b))), length(0.5 - frac(c))), 7) * 25.;
}

inline void DirectionalLight(in ShaderEntity light, in Surface surface, inout Lighting lighting, in float shadow_mask = 1)
{
#ifdef WEAPON_SHADOW
	// experiment to pull surface positions closer to camera
	// and thus ensure geometry not STUCK INSIDE a wall that would
	// cast a shadow on it
	float3 diffxytz = surface.P.xyz - g_xCamera_CamPos;
	diffxytz /= 3.0f;
	surface.P.xyz = g_xCamera_CamPos + diffxytz;
#endif
	
	float3 L = light.GetDirection();

	SurfaceToLight surfaceToLight;
	surfaceToLight.create(surface, L);

	[branch]
	if (any(surfaceToLight.NdotL_sss))
	{
		float3 shadow = shadow_mask;

		[branch]
		//if (light.IsCastingShadow() && surface.IsReceiveShadow()) LB: fixes issue of "move way away from position 0,0,0 causes graphic issues #5174"
		if (light.IsCastingShadow() && surface.IsReceiveShadow() && g_xFrame_ShadowRes2D > 0)
		{
#ifndef RTAPI
			[branch]
			if ((g_xFrame_Options & OPTION_BIT_RAYTRACED_SHADOWS) == 0)
#endif // RTAPI
			{
				if ( L.y <= 0 ) 
				{
					shadow = float3(0,0,0);
				}
				else
				{
#ifdef DISTANCE_BASED_SHADOWS
					float dist = length(g_xCamera_CamPos - surface.P);
#endif
					// Loop through cascades from closest (smallest) to furthest (largest)
					[loop]
					for (uint cascade = 0; cascade < g_xFrame_ShadowCascadeCount; ++cascade)
					{
						// Project into shadow map space (no need to divide by .w because ortho projection!):
						float3 ShPos = mul(MatrixArray[light.GetMatrixIndex() + cascade], float4(surface.P, 1)).xyz;
						float3 ShTex = ShPos * float3(0.5, -0.5, 0.5) + 0.5;

						// Determine if pixel is inside current cascade bounds and compute shadow if it is:
						[branch]
						if (is_saturated(ShTex))
						{
#ifdef DISTANCE_BASED_SHADOWS
							const float3 shadow_main = shadowCascade(light, ShPos, ShTex.xy, cascade, dist);
#else
							const float3 shadow_main = shadowCascade(light, ShPos, ShTex.xy, cascade);
#endif
							const float3 cascade_edgefactor = saturate(saturate(abs(ShPos)) - 0.8) * 5.0; // fade will be on edge and inwards 20%
							const float cascade_fade = max(cascade_edgefactor.x, max(cascade_edgefactor.y, cascade_edgefactor.z));

							// If we are on cascade edge threshold and not the last cascade, then fallback to a larger cascade:
							[branch]
							if (cascade_fade > 0 && cascade < g_xFrame_ShadowCascadeCount - 1)
							{
								// Project into next shadow cascade (no need to divide by .w because ortho projection!):
								cascade += 1;
								ShPos = mul(MatrixArray[light.GetMatrixIndex() + cascade], float4(surface.P, 1)).xyz;
								ShTex = ShPos * float3(0.5, -0.5, 0.5) + 0.5;
#ifdef DISTANCE_BASED_SHADOWS
								const float3 shadow_fallback = shadowCascade(light, ShPos, ShTex.xy, cascade, dist);
#else
								const float3 shadow_fallback = shadowCascade(light, ShPos, ShTex.xy, cascade);
#endif

								shadow *= lerp(shadow_main, shadow_fallback, cascade_fade);
							}
							else
							{
								shadow *= shadow_main;
							}
							break;
						}
					}
				}
			}
		}

		[branch]
		if (any(shadow))
		{
			float3 atmosphereTransmittance = 1;
			if (g_xFrame_Options & OPTION_BIT_REALISTIC_SKY)
			{
				atmosphereTransmittance = GetAtmosphericLightTransmittance(g_xFrame_Atmosphere, surface.P, L, texture_transmittancelut);
			}
			
			float3 lightColor = light.GetColor().rgb * light.GetEnergy() * shadow * atmosphereTransmittance;

			lighting.direct.diffuse +=
				max(0, lightColor * surfaceToLight.NdotL_sss * BRDF_GetDiffuse(surface, surfaceToLight));

			lighting.direct.specular +=
				max(0, lightColor * surfaceToLight.NdotL * BRDF_GetSpecular(surface, surfaceToLight));

#ifndef WATER
			float2 ocean_uv = (surface.P.xz + (surface.P.yy * 0.25)) * 0.0045;
			float water_height = g_xFrame_WaterHeight;
			if ((g_xFrame_Options & OPTION_BIT_WATER_ENABLED) && surface.P.y < water_height)
			{
				float3 caustic = caustic_pattern(ocean_uv, g_xFrame_Time * 0.65);
				caustic *= sqr(saturate((water_height - surface.P.y) * 0.0025)); // fade out at shoreline
				caustic *= lightColor;
				lighting.indirect.diffuse += caustic;
			}
#endif

		}
	}
}
inline void PointLight(in ShaderEntity light, in Surface surface, inout Lighting lighting, in float shadow_mask = 1)
{
	#ifdef WEAPON_SHADOW
	//PE: Try the same for point shadows.
	// experiment to pull surface positions closer to camera
	// and thus ensure geometry not STUCK INSIDE a wall that would
	// cast a shadow on it
	float3 diffxytz = surface.P.xyz - g_xCamera_CamPos;
	diffxytz /= 3.0f;
	surface.P.xyz = g_xCamera_CamPos + diffxytz;
	#endif

	float3 L = light.position - surface.P;
	const float dist2 = dot(L, L);
	const float range2 = light.GetRange() * light.GetRange();

	[branch]
	if (dist2 < range2)
	{
		const float3 Lunnormalized = L;
		const float dist = sqrt(dist2);
		L /= dist;

		SurfaceToLight surfaceToLight;
		surfaceToLight.create(surface, L);

		[branch]
		if (any(surfaceToLight.NdotL_sss))
		{
			float3 shadow = shadow_mask;

			[branch]
			if (light.IsCastingShadow() && surface.IsReceiveShadow())
			{
#ifndef RTAPI
				[branch]
				if ((g_xFrame_Options & OPTION_BIT_RAYTRACED_SHADOWS) == 0)
#endif // RTAPI
				{
#ifdef DISTANCE_BASED_SHADOWS
					float dist = length(g_xCamera_CamPos - surface.P);
					shadow *= shadowCube(light, L, Lunnormalized, dist);
#else
					shadow *= shadowCube(light, L, Lunnormalized);
#endif
				}
			}

			[branch]
			if (any(shadow))
			{
				float3 lightColor = light.GetColor().rgb * light.GetEnergy() * shadow;

				const float att = saturate(1 - (dist2 / range2));
				const float attenuation = att * att;
				lightColor *= attenuation;

				lighting.direct.diffuse +=
					max(0, lightColor * surfaceToLight.NdotL_sss * BRDF_GetDiffuse(surface, surfaceToLight));

				lighting.direct.specular +=
					max(0, lightColor * surfaceToLight.NdotL * BRDF_GetSpecular(surface, surfaceToLight));
			}
		}
	}
}
inline void SpotLight(in ShaderEntity light, in Surface surface, inout Lighting lighting, in float shadow_mask = 1)
{
	#ifdef WEAPON_SHADOW
	//PE: Try the same for spot shadows.
	// experiment to pull surface positions closer to camera
	// and thus ensure geometry not STUCK INSIDE a wall that would
	// cast a shadow on it
	float3 diffxytz = surface.P.xyz - g_xCamera_CamPos;
	diffxytz /= 3.0f;
	surface.P.xyz = g_xCamera_CamPos + diffxytz;
	#endif

	float3 L = light.position - surface.P;
	const float dist2 = dot(L, L);
	const float range2 = light.GetRange() * light.GetRange();

	[branch]
	if (dist2 < range2)
	{
		const float dist = sqrt(dist2);
		L /= dist;

		SurfaceToLight surfaceToLight;
		surfaceToLight.create(surface, L);

		[branch]
		if (any(surfaceToLight.NdotL_sss))
		{
			const float SpotFactor = dot(L, light.GetDirection());
			const float spotCutOff = light.GetConeAngleCos();

			[branch]
			if (SpotFactor > spotCutOff)
			{
				float3 shadow = shadow_mask;

				[branch]
				if (light.IsCastingShadow() && surface.IsReceiveShadow())
				{
#ifndef RTAPI
					[branch]
					if ((g_xFrame_Options & OPTION_BIT_RAYTRACED_SHADOWS) == 0)
#endif // RTAPI
					{
						float4 ShPos = mul(MatrixArray[light.GetMatrixIndex() + 0], float4(surface.P, 1));
						ShPos.xyz /= ShPos.w;
						float2 ShTex = ShPos.xy * float2(0.5, -0.5) + 0.5;
						[branch]
						if (is_saturated(ShTex))
						{
#ifdef DISTANCE_BASED_SHADOWS
							float dist = length(g_xCamera_CamPos - surface.P);
							shadow *= shadowCascadeSpot(light, ShPos.xyz, ShTex.xy, 0, dist);
#else
							shadow *= shadowCascadeSpot(light, ShPos.xyz, ShTex.xy, 0);
#endif
						}
					}
				}

				[branch]
				if (any(shadow))
				{
					float3 lightColor = light.GetColor().rgb * light.GetEnergy() * shadow;

					const float att = saturate(1 - (dist2 / range2));
					float attenuation = att * att;
					attenuation *= saturate((1 - (1 - SpotFactor) * 1 / (1 - spotCutOff)));
					lightColor *= attenuation;

					lighting.direct.diffuse +=
						max(0, lightColor * surfaceToLight.NdotL_sss * BRDF_GetDiffuse(surface, surfaceToLight));

					lighting.direct.specular +=
						max(0, lightColor * surfaceToLight.NdotL * BRDF_GetSpecular(surface, surfaceToLight));
				}
			}
		}
	}
}


// ENVIRONMENT MAPS


inline float3 GetAmbient(in float3 N)
{
	float3 ambient = 0;

#ifdef ENVMAPRENDERING
	/*
	// Set realistic_sky_stationary to true so we capture ambient at float3(0.0, 0.0, 0.0), similar to the standard sky to avoid flickering and weird behavior
	ambient = lerp(
		GetDynamicSkyColor(float3(0, -1, 0), false, false, false, true),
		GetDynamicSkyColor(float3(0, 1, 0), false, false, false, true),
		saturate(N.y * 0.5 + 0.5));
		*/
#else

	//ambient = texture_envmaparray.SampleLevel(sampler_linear_clamp, float4(N, g_xFrame_GlobalEnvProbeIndex), g_xFrame_EnvProbeMipCount).rgb;

#endif // ENVMAPRENDERING

	// This is not entirely correct if we have probes, because it shouldn't be added twice.
	//	However, it is not correct if we leave it out from probes, because if we render a scene
	//	with dark sky but ambient, we still want some visible result.
	ambient += GetAmbientColor();

	return ambient;
}

// surface:				surface descriptor
// MIP:					mip level to sample
// return:				color of the environment color (rgb)
inline float3 EnvironmentReflection_Global(in Surface surface)
{
	float3 envColor;

#ifdef ENVMAPRENDERING
	/*
	// There is no access to envmaps, so approximate sky color:
	// Set realistic_sky_stationary to true so we capture environment at float3(0.0, 0.0, 0.0), similar to the standard sky to avoid flickering and weird behavior
	float3 realSkyColor = GetDynamicSkyColor(surface.R, false, false, false, true); // false: disable sun disk and clouds
	float3 roughSkyColor = lerp(
		GetDynamicSkyColor(float3(0, -1, 0), false, false, false, true),
		GetDynamicSkyColor(float3(0, 1, 0), false, false, false, true),
		saturate(surface.R.y * 0.5 + 0.5));

	envColor = lerp(realSkyColor, roughSkyColor, saturate(surface.roughness)) * surface.F;
	*/
	return float3(0,0,0);
#else

	float MIP = surface.roughness * g_xFrame_EnvProbeMipCount;
	envColor = texture_envmaparray.SampleLevel(sampler_linear_clamp, float4(surface.R, g_xFrame_GlobalEnvProbeIndex), MIP).rgb * surface.F;

#ifdef BRDF_SHEEN
	envColor *= surface.sheen.albedoScaling;
	MIP = surface.sheen.roughness * g_xFrame_EnvProbeMipCount;
	envColor += texture_envmaparray.SampleLevel(sampler_linear_clamp, float4(surface.R, g_xFrame_GlobalEnvProbeIndex), MIP).rgb * surface.sheen.color * surface.sheen.DFG;
#endif // BRDF_SHEEN

#ifdef BRDF_CLEARCOAT
	envColor *= 1 - surface.clearcoat.F;
	MIP = surface.clearcoat.roughness * g_xFrame_EnvProbeMipCount;
	envColor += texture_envmaparray.SampleLevel(sampler_linear_clamp, float4(surface.clearcoat.R, g_xFrame_GlobalEnvProbeIndex), MIP).rgb * surface.clearcoat.F;
#endif // BRDF_CLEARCOAT

#endif // ENVMAPRENDERING

	envColor *= (0.5 * surface.metalness + 0.5);
	return envColor;
}

// surface:				surface descriptor
// probe :				the shader entity holding properties
// probeProjection:		the inverse OBB transform matrix
// clipSpacePos:		world space pixel position transformed into OBB space by probeProjection matrix
// MIP:					mip level to sample
// return:				color of the environment map (rgb), blend factor of the environment map (a)
inline float4 EnvironmentReflection_Local(in Surface surface, in ShaderEntity probe, in float4x4 probeProjection, in float3 clipSpacePos)
{
	// Perform parallax correction of reflection ray (R) into OBB:
	//float3 RayLS = mul(surface.R, (float3x3)probeProjection);
	//PE: https://github.com/turanszkij/WickedEngine/commit/814e6a07bd727f65fe88a11b9caca5311fd63aa9
	float3 RayLS = mul((float3x3)probeProjection, surface.R);

	float3 FirstPlaneIntersect = (float3(1, 1, 1) - clipSpacePos) / RayLS;
	float3 SecondPlaneIntersect = (-float3(1, 1, 1) - clipSpacePos) / RayLS;
	float3 FurthestPlane = max(FirstPlaneIntersect, SecondPlaneIntersect);
	float Distance = min(FurthestPlane.x, min(FurthestPlane.y, FurthestPlane.z));
	float3 IntersectPositionWS = surface.P + surface.R * Distance;
	float3 R_parallaxCorrected = IntersectPositionWS - probe.position;

	// Sample cubemap texture:
	float MIP = surface.roughness * g_xFrame_EnvProbeMipCount;
	float3 envColor = texture_envmaparray.SampleLevel(sampler_linear_clamp, float4(R_parallaxCorrected, probe.GetTextureIndex()), MIP).rgb * surface.F;

#ifdef BRDF_SHEEN
	envColor *= surface.sheen.albedoScaling;
	MIP = surface.sheen.roughness * g_xFrame_EnvProbeMipCount;
	envColor += texture_envmaparray.SampleLevel(sampler_linear_clamp, float4(R_parallaxCorrected, probe.GetTextureIndex()), MIP).rgb * surface.sheen.color * surface.sheen.DFG;
#endif // BRDF_SHEEN

#ifdef BRDF_CLEARCOAT
	//RayLS = mul(surface.clearcoat.R, (float3x3)probeProjection);
	//PE: https://github.com/turanszkij/WickedEngine/commit/814e6a07bd727f65fe88a11b9caca5311fd63aa9
	RayLS = mul((float3x3)probeProjection, surface.clearcoat.R);
	FirstPlaneIntersect = (float3(1, 1, 1) - clipSpacePos) / RayLS;
	SecondPlaneIntersect = (-float3(1, 1, 1) - clipSpacePos) / RayLS;
	FurthestPlane = max(FirstPlaneIntersect, SecondPlaneIntersect);
	Distance = min(FurthestPlane.x, min(FurthestPlane.y, FurthestPlane.z));
	IntersectPositionWS = surface.P + surface.clearcoat.R * Distance;
	R_parallaxCorrected = IntersectPositionWS - probe.position;

	envColor *= 1 - surface.clearcoat.F;
	MIP = surface.clearcoat.roughness * g_xFrame_EnvProbeMipCount;
	envColor += texture_envmaparray.SampleLevel(sampler_linear_clamp, float4(R_parallaxCorrected, probe.GetTextureIndex()), MIP).rgb * surface.clearcoat.F;
#endif // BRDF_CLEARCOAT

	// blend out if close to any cube edge:
	float edgeBlend = 1 - pow(saturate(max(abs(clipSpacePos.x), max(abs(clipSpacePos.y), abs(clipSpacePos.z)))), 8);

	envColor *= (0.5 * surface.metalness + 0.5);
	edgeBlend *= ((probe.userdata & 0xFF) / 255.0);
	return float4(envColor, edgeBlend);
}



// VOXEL RADIANCE

inline void VoxelGI(in Surface surface, inout Lighting lighting)
{
	[branch] if (g_xFrame_VoxelRadianceDataRes != 0)
	{
		// determine blending factor (we will blend out voxel GI on grid edges):
		float3 voxelSpacePos = surface.P - g_xFrame_VoxelRadianceDataCenter;
		voxelSpacePos *= g_xFrame_VoxelRadianceDataSize_rcp;
		voxelSpacePos *= g_xFrame_VoxelRadianceDataRes_rcp;
		voxelSpacePos = saturate(abs(voxelSpacePos));
		float blend = 1 - pow(max(voxelSpacePos.x, max(voxelSpacePos.y, voxelSpacePos.z)), 4);

		// diffuse:
		{
			float4 trace = ConeTraceDiffuse(texture_voxelradiance, surface.P, surface.N);
			lighting.indirect.diffuse = lerp(lighting.indirect.diffuse, trace.rgb, trace.a * blend);
		}

		// specular:
		[branch]
		if (g_xFrame_Options & OPTION_BIT_VOXELGI_REFLECTIONS_ENABLED)
		{
			float4 trace = ConeTraceSpecular(texture_voxelradiance, surface.P, surface.N, surface.V, surface.roughness);
			lighting.indirect.specular = lerp(lighting.indirect.specular, trace.rgb * surface.F, trace.a * blend);
		}
	}
}

#endif // WI_LIGHTING_HF
