// THE PRESENT QUAD (run hd2, lane GPU1). One vertex shader and one pixel
// shader, and between them they do the whole of what the optional Direct3D 11
// present path draws: the finished picture, as a texture, stretched once into
// the same destination rectangle the GDI path blits into.
//
// COMPILED OFFLINE, never at run time. The bytecode in hal/gpu_present_shaders.h
// is what this file compiles to, produced with the Windows SDK's fxc.exe:
//
//   fxc /nologo /T vs_4_0 /E vs_main /O3 /Vn kGpuPresentVS /Fh <vs header> gpu_present.hlsl
//   fxc /nologo /T ps_4_0 /E ps_main /O3 /Vn kGpuPresentPS /Fh <ps header> gpu_present.hlsl
//
// and the two /Fh outputs concatenated into the one header. vs_4_0 / ps_4_0 is
// feature level 10_0, which is the floor the backend asks the device for.
// d3dcompiler.dll is therefore never loaded and never shipped.
//
// NO VERTEX BUFFER AND NO INPUT LAYOUT. The four corners come out of
// SV_VertexID as a triangle strip, so the draw is Draw(4, 0) against a null
// input assembler. One less resource to create, one less thing to lose on a
// device reset.

cbuffer PresentParams : register(b0)
{
    // The destination rectangle in normalised device coordinates: x0, y0 is
    // the TOP LEFT corner (y0 is the larger number, NDC y grows upwards) and
    // x1, y1 the bottom right. Everything outside it is whatever the clear
    // left there, which is the black bar.
    float4 rect;
    // src_w, src_h, 1/src_w, 1/src_h -- the size of the picture being sampled,
    // which is the framebuffer's ACTIVE rectangle and not its allocation.
    float4 texsize;
    // dst_w, dst_h, filter, unused. filter is 0 nearest, 1 linear, 2 sharp.
    float4 dstsize;
};

Texture2D<float4> src : register(t0);
SamplerState      smp : register(s0);

struct VSOut
{
    float4 pos : SV_Position;
    float2 uv  : TEXCOORD0;
};

VSOut vs_main(uint id : SV_VertexID)
{
    // id 0,1,2,3 -> (0,0) (1,0) (0,1) (1,1): a triangle strip covering the
    // rectangle. uv.y is 0 at the TOP, which is the order the framebuffer's
    // rows are in (the DIB header the GDI path uses is top-down for the same
    // reason).
    const float2 c = float2((id & 1) ? 1.0f : 0.0f, (id & 2) ? 1.0f : 0.0f);
    VSOut o;
    o.pos = float4(lerp(rect.x, rect.z, c.x), lerp(rect.y, rect.w, c.y),
                   0.0f, 1.0f);
    o.uv = c;
    return o;
}

float4 ps_main(VSOut i) : SV_Target
{
    float2 uv = i.uv;
    // FILTER 2, "sharp bilinear": the picture is blown up by a whole number
    // with hard pixel edges first and only the leftover fraction is blended,
    // so a 2.6x window keeps the DS's blocky look instead of going soft, and
    // the seam between two source pixels is the only place anything blurs.
    // Done here as a coordinate warp rather than with a second render target:
    // the sample point is pinned to the middle of its source pixel everywhere
    // except within one destination pixel of a source pixel's edge, where it
    // sweeps across, and the linear sampler then does the blend. The
    // arithmetic is the CPU reference in hal/gpu_present.cpp, byte for byte.
    if (dstsize.z >= 1.5f) {
        const float2 texel   = uv * texsize.xy;
        const float2 floored = floor(texel);
        const float2 s       = texel - floored;
        const float2 scale   = max(floor(dstsize.xy / texsize.xy), 1.0f);
        const float2 range   = 0.5f - 0.5f / scale;
        const float2 dist    = s - 0.5f;
        const float2 f = (dist - clamp(dist, -range, range)) * scale + 0.5f;
        uv = (floored + f) * texsize.zw;
    }
    // SampleLevel rather than Sample: there is one mip level, the quad is flat,
    // and asking for level 0 outright means no derivative work and no chance of
    // a driver picking a smaller level at a steep window size.
    return src.SampleLevel(smp, uv, 0.0f);
}
