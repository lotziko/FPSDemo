Shader
{
	Properties
	{
		Texture2D _BaseMap = "white"
		Texture2D _NormalMap = "normal"
		Texture2D _ORMMap = "white"
	}
	Pass
	{
		Blend One Zero
		ZTest Equal
		ZWrite Off
		Cull Back

		HLSLBEGIN
		#pragma vertex MeshLitVertex
		#pragma fragment MeshLitFragment

		#pragma keyword_local_fragment TRANSPARENT
		#pragma keyword_global_vertex MULTIVIEW
		#pragma keyword_global_vertex LIGHTMAP
		#pragma keyword_global_fragment MULTIVIEW
		#pragma keyword_global_fragment SHADOWS
		#pragma keyword_global_fragment REFLECTIONS
		#pragma keyword_global_fragment LIGHTMAP PROBES

		#include "Core.hlsl"

		struct Attributes
		{
			float3 positionOS : POSITION;
			float3 normalOS	: NORMAL;
			float4 tangentOS : TANGENT;
			float2 texcoord : TEXCOORD0;
#if (LIGHTMAP)
			float3 texcoordLightmap : TEXCOORD1;
#endif
			VERTEX_INPUT_INSTANCE_ID
		};

		struct Varyings
		{
			float4 positionCS : SV_POSITION;
			float3 positionWS : TEXCOORD0;
			float3 normalWS : TEXCOORD1;
			float3 tangentWS : TEXCOORD2;
			float3 bitangentWS : TEXCOORD3;
			float2 texcoord : TEXCOORD4;
#if (LIGHTMAP)
			centroid float2 texcoordLightmap : TEXCOORD5;
#endif
			VERTEX_OUTPUT_VIEW_INDEX
		};

		Varyings MeshLitVertex(Attributes input)
		{
			Varyings output;
			SETUP_INSTANCE_ID(input);
			SETUP_OUTPUT_VIEW_INDEX(output);

			output.positionWS = TransformObjectToWorld(input.positionOS);
			output.positionCS = TransformWorldToClip(output.positionWS);
			output.normalWS = TransformObjectToWorldNormal(input.normalOS);
			output.tangentWS = TransformObjectToWorldNormal(input.tangentOS.xyz);
			output.bitangentWS = cross(output.normalWS, output.tangentWS) * input.tangentOS.w * 1;
			output.texcoord = input.texcoord;
#if (LIGHTMAP)
			float4 offsetScale = _PerLightmapInstanceData[LIGHTMAP_CHART_OFFSET.x > 0 ? LIGHTMAP_CHART_OFFSET.x + input.texcoordLightmap.z : 0];
			output.texcoordLightmap = input.texcoordLightmap.xy * offsetScale.zw + offsetScale.xy;
#endif
			return output;
		}

		TEXTURE2D(_BaseMap);		SAMPLER(_BaseMap_Sampler);
		TEXTURE2D(_NormalMap);		SAMPLER(_NormalMap_Sampler);
		TEXTURE2D(_ORMMap);			SAMPLER(_ORMMap_Sampler);

		inline void InitializeSurfaceData(float2 uv, out SurfaceData outSurfaceData)
		{
			outSurfaceData = (SurfaceData)0;
			float4 color = _BaseMap.Sample(_BaseMap_Sampler, uv);
			outSurfaceData.albedo = color.rgb;
#if (TRANSPARENT)
			outSurfaceData.alpha = 1;
			clip(color.a - 0.125);
#else
			outSurfaceData.alpha = 1;
#endif
			float3 orm = _ORMMap.Sample(_ORMMap_Sampler, uv).rgb;
			outSurfaceData.metallic = orm.b;
			outSurfaceData.roughness = orm.g;
			outSurfaceData.normalTS = _NormalMap.Sample(_NormalMap_Sampler, uv);
			outSurfaceData.emission = 0;
			outSurfaceData.occlusion = orm.r;
		}

		inline void InitializeInputData(Varyings input, float3 normalTS, out InputData outInputData)
		{
			outInputData = (InputData)0;
			outInputData.positionWS = input.positionWS;
			outInputData.positionVS = TransformWorldToView(input.positionWS);
			outInputData.normalGS = input.normalWS;
			outInputData.normalWS = NormalTSToNormalWS(ReconstructNormal(normalTS * 2.0 - 1.0), input.normalWS, input.tangentWS, input.bitangentWS);
			outInputData.viewDirectionWS = GetNormalizedViewDirectionWS(input.positionWS);
			outInputData.normalizedScreenSpaceUV = GetNormalizedScreenSpaceUV(input.positionCS);
#if (LIGHTMAP)
			outInputData.bakedGI = SampleLightmap(input.texcoordLightmap);
#elif (PROBES)
			outInputData.bakedGI = SampleProbeVolume(input.positionWS, input.normalWS);
#else
			outInputData.bakedGI = _AmbientLightColor.rgb;
#endif
		}

		float4 MeshLitFragment(Varyings input) : SV_TARGET
		{
			SETUP_INPUT_VIEW_INDEX(input);
			
			SurfaceData surfaceData;
			InitializeSurfaceData(input.texcoord, surfaceData);

			InputData inputData;
			InitializeInputData(input, surfaceData.normalTS, inputData);
			//return float4(inputData.bakedGI, 1);

			float2 positionSS = input.positionCS.xy * RENDER_TARGET_SIZE_INV_SIZE.zw;
			surfaceData.occlusion *= SAMPLE_TEXTURE2D_X(_ScreenOcclusionTexture, _ScreenOcclusionTexture_Sampler, positionSS).r;
			float4 color = float4(CalculatePBR(surfaceData, inputData), surfaceData.alpha);
			color = ApplyVolumetricFog(color, inputData.normalizedScreenSpaceUV, input.positionCS.z);
			return color;
		}
		HLSLEND
	}
	Pass
	{
		Blend One Zero
		ZWrite On
		Cull Back

		HLSLBEGIN
		#pragma vertex MeshLitVertex
		#pragma fragment MeshLitFragment
		
		#pragma keyword_local_fragment TRANSPARENT
		#pragma keyword_global_vertex MULTIVIEW

		#include "Core.hlsl"

		struct Attributes
		{
			float3 positionOS : POSITION;
			float3 normalOS	: NORMAL;
			float4 tangentOS : TANGENT;
			float2 texcoord : TEXCOORD0;
			VERTEX_INPUT_INSTANCE_ID
		};

		struct Varyings
		{
			float4 positionCS : SV_POSITION;
			float3 normalWS : TEXCOORD0;
			float3 tangentWS : TEXCOORD1;
			float3 bitangentWS : TEXCOORD2;
			float2 texcoord : TEXCOORD3;
			VERTEX_OUTPUT_VIEW_INDEX
		};

		Varyings MeshLitVertex(Attributes input)
		{
			Varyings output;
			SETUP_INSTANCE_ID(input);
			SETUP_OUTPUT_VIEW_INDEX(output);

			output.positionCS = TransformObjectToClip(input.positionOS);
			output.normalWS = TransformObjectToWorldNormal(input.normalOS);
			output.tangentWS = TransformObjectToWorldNormal(input.tangentOS.xyz);
			output.bitangentWS = cross(output.normalWS, output.tangentWS) * input.tangentOS.w * 1;
			output.texcoord = input.texcoord;

			return output;
		}

		TEXTURE2D(_BaseMap);		SAMPLER(_BaseMap_Sampler);
		TEXTURE2D(_NormalMap);		SAMPLER(_NormalMap_Sampler);

		/*float4*/void MeshLitFragment(Varyings input) //: SV_TARGET
		{
			float2 uv = input.texcoord;
#if (TRANSPARENT)
			clip(_BaseMap.Sample(_BaseMap_Sampler, uv).a - 0.125);
#endif		
			//float2 normal = _NormalMap.Sample(_NormalMap_Sampler, uv).rg * 2 - 1;
			//float3 normalWS = NormalTSToNormalWS(normalize(float3(normal.x, normal.y, sqrt(saturate(1 - dot(normal, normal))))), input.normalWS, input.tangentWS, input.bitangentWS);
			//return float4(1, 0, 0, 1);//return float4(normalWS * 0.5 + 0.5, 1.0f);
		}
		HLSLEND
	}
	Pass
	{
		Blend One Zero
		ZWrite On
		Cull Back

		HLSLBEGIN
		#pragma vertex MeshLitVertex
		#pragma fragment MeshLitFragment

		#pragma keyword_local_vertex TRANSPARENT
		#pragma keyword_local_fragment TRANSPARENT

		#include "Core.hlsl"

		struct Attributes
		{
			float3 positionOS : POSITION;
#if (TRANSPARENT)
			float2 texcoord : TEXCOORD0;
#endif
			VERTEX_INPUT_INSTANCE_ID
		};

		struct Varyings
		{
			float4 positionCS : SV_POSITION;
#if (TRANSPARENT)
			float2 texcoord : TEXCOORD0;
#endif
		};

		TEXTURE2D(_BaseMap);		SAMPLER(_BaseMap_Sampler);

		Varyings MeshLitVertex(Attributes input)
		{
			SETUP_INSTANCE_ID(input);

			Varyings output;
			output.positionCS = TransformObjectToClip(input.positionOS);
#if (TRANSPARENT)
			output.texcoord = input.texcoord;
#endif
			return output;
		}

		void MeshLitFragment(Varyings input)
		{
#if (TRANSPARENT)
			clip(_BaseMap.Sample(_BaseMap_Sampler, input.texcoord).a - 0.125);
#endif		
		}
		HLSLEND
	}
}