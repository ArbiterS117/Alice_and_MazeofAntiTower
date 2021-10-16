

//*****************************************************************************
// �萔�o�b�t�@
//*****************************************************************************

// �}�g���N�X�o�b�t�@
cbuffer WorldBuffer : register( b0 )
{
	matrix World;
}

cbuffer ViewBuffer : register( b1 )
{
	matrix View;
}

cbuffer ProjectionBuffer : register( b2 )
{
	matrix Projection;
}

// �}�e���A���o�b�t�@
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte���E�p
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ���C�g�p�o�b�t�@
struct LIGHT
{
	float4		Direction[10];
	float4		Position[10];
	float4		Diffuse[10];
	float4		Ambient[10];
	float4		Attenuation[10];
	int4		Flags[10];
	int			Enable;
	int			Dummy[3];//16byte���E�p
};

cbuffer LightBuffer : register( b4 )
{
	LIGHT		Light;
}

struct FOG
{
	float4		Distance;
	float4		FogColor;
	int			Enable;
	float		Dummy[3];//16byte���E�p
};

// �t�H�O�p�o�b�t�@
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

//=============================================================================
// ���_�V�F�[�_
//=============================================================================
void VertexShaderPolygon( in  float4 inPosition		: POSITION0,
						  in  float4 inNormal		: NORMAL0,
						  in  float4 inDiffuse		: COLOR0,
						  in  float2 inTexCoord		: TEXCOORD0,

						  out float4 outPosition	: SV_POSITION,
						  out float4 outNormal		: NORMAL0,
						  out float2 outTexCoord	: TEXCOORD0,
						  out float4 outDiffuse		: COLOR0 )
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);

	outPosition = mul(inPosition, wvp);
	outNormal = inNormal;
	outTexCoord = inTexCoord;

	if (Light.Enable == 0) 
	{
		outDiffuse = inDiffuse * Material.Diffuse;
	}
	else
	{
		float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

		for (int i = 0; i < 10; i++)
		{
			if (Light.Flags[i].x == 1)
			{
				float4 worldNormal, normal;
				normal = float4(inNormal.xyz, 0.0);
				worldNormal = mul(normal, World);
				worldNormal = normalize(worldNormal);

				float light = 0.5 - 0.5 * dot(Light.Direction[i].xyz, worldNormal.xyz);

				color = inDiffuse * Material.Diffuse * light * Light.Diffuse[i];
				color += inDiffuse * Material.Ambient * Light.Ambient[i];
			}
			else if (Light.Flags[i].x == 2)
			{
				float4 worldNormal, normal;
				normal = float4(inNormal.xyz, 0.0);
				worldNormal = mul(normal, World);
				worldNormal = normalize(worldNormal);

				float3 posWorld = mul(inPosition, World);
				float3 lightDir;
				lightDir = normalize(posWorld - Light.Position[i]);
				float light = -1.0 * dot(lightDir.xyz, worldNormal.xyz);

				color = inDiffuse * Material.Diffuse * light * Light.Diffuse[i];
				color += inDiffuse * Material.Ambient * Light.Ambient[i];
				float distance = length(posWorld - Light.Position[i]);

				float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
				color *= att;
			}
			else
			{
				color = float4(0.0f, 0.0f, 0.0f, 0.0f);
			}

			outColor += color;
		}

		outColor += Material.Emission;

		outDiffuse = saturate(outColor);
		outDiffuse.a = inDiffuse.a * Material.Diffuse.a;

		// �ł��Â��F
		if (outDiffuse.x < 0.2f)outDiffuse.x = 0.2f;
		if (outDiffuse.y < 0.2f)outDiffuse.y = 0.2f;
		if (outDiffuse.z < 0.2f)outDiffuse.z = 0.2f;
	}
}



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Texture2D		g_Texture : register( t0 );
SamplerState	g_SamplerState : register( s0 );


//=============================================================================
// �s�N�Z���V�F�[�_
//=============================================================================
void PixelShaderPolygon( in  float4 inPosition		: SV_POSITION,
						 in  float4 inNormal		: NORMAL0,
						 in  float2 inTexCoord		: TEXCOORD0,
						 in  float4 inDiffuse		: COLOR0,

						 out float4 outDiffuse		: SV_Target )
{
	float4 color;

	if (Material.noTexSampling == 0)
	{
		color = g_Texture.Sample(g_SamplerState, inTexCoord);

		color *= inDiffuse;
	}
	else
	{
		color = inDiffuse;
	}


	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
		//outDiffuse.a = 0.5f;
	}
	else
	{
		outDiffuse = color;
		//outDiffuse.a = 1.0f;
	}
}