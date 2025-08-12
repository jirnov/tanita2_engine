float4x4 gWorldViewProj  : WORLDVIEWPROJ;
texture  gTexture        : TEXTURE;
float2   gTextureSize    : TEXTURESIZE;
float2   gVideoSize      : VIDEOSIZE;

sampler gSampler = 
    sampler_state
    {
        Texture = <gTexture>;
    };


technique RenderScene
{
    pass P0
    {
        VertexShaderConstant4[0] = <gWorldViewProj>;
        VertexShaderConstant1[4]  = <gTextureSize>;
        VertexShaderConstant1[5]  = <gVideoSize>;
        Texture[0] = gTexture;
        Texture[1] = gTexture;
        Texture[2] = gTexture;
        
        VertexShader = asm
        {
            vs_1_1
            
            dcl_position v0
            dcl_texcoord0 v1
            
            def c6, 0.25, 1.5, 1, 0.5
            
            dp4 oPos.x, v0, c0
            dp4 oPos.y, v0, c1
            dp4 oPos.z, v0, c2
            dp4 oPos.w, v0, c3
            
            rcp r0.x, c4.y
            mul r0.x, r0.x, c5.y
            mad oT1.y, v1.y, c6.x, r0.x
            
            mul r0.y, v1.y, c6.x
            
            mad oT2.y, r0.x, c6.y, r0.y
            mul oT0.xy, v1, c6.zwzw
            
            mul oT1.x, v1.x, c6.w
            mul oT2.x, v1.x, c6.w
        };
        
        PixelShader  = asm
        {
            ps_1_1
            def c0,  0.1455,     0,           0.19948,    0
            def c1,  0.1455,    -0.04900,    -0.10161,    0
            def c2,  0.1455,     0.25213,     0,          0
            def c3, -0.2176,     0.13238,    -0.2703,     1
            
            def c4,  0, 1, 0, 0
            def c5,  0, 0, 1, 0
            def c6,  1, 1, 1, 0
            
            tex t0
            tex t1
            tex t2
            
            lrp t0, c4, t1, t0
            
            lrp t0, c5, t2, t0
            
            dp3 r0, c0, t0
            dp3 r1, c1, t0
            lrp r0, c4, r1, r0
            
            dp3 r1, c2, t0
            lrp_x2 r0, c5, r1, r0
            
            mad_x4 r0, r0, c6, c3
        };
    }
}
