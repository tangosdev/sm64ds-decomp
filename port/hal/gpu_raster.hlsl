// THE OPAQUE 3D PASS, ON THE CARD. Compiled offline with the Windows SDK's
// fxc.exe into hal/gpu_raster_shaders.h; d3dcompiler.dll never loads at run
// time. The exact commands are in that header's own comment.
//
// EVERY DECISION HERE IS A COPY OF ntr/gx.cpp's SOFTWARE RASTER, not an
// improvement on it. The software path is the byte gate and the default, so
// where this file and that loop disagree the software one is right and this is
// the bug. The three places that matter:
//
//  1. VERTEX COLOUR INTERPOLATES SCREEN-LINEAR, which is why it is declared
//     noperspective. gx.cpp interpolates the colour with the plain screen-space
//     barycentrics (l0 * acol + l1 * bcol + l2 * ccol, no 1/w anywhere) while it
//     interpolates u and v through 1/w. A perspective-correct colour would be
//     the more "correct" picture and the wrong one.
//
//  2. THE TEXEL ARITHMETIC IS THE SAME ARITHMETIC. gx.cpp computes
//     round(vertex_colour * texel / 255) per channel with the vertex colour
//     carried as a 0..255 float, so the colour comes in here as 0..255 floats
//     rather than a packed byte colour, and the texel arrives as an 8-bit UNORM
//     sample, whose conversion to float is exactly n/255.
//
//  3. THE ALPHA RULE IS THE SAME RULE. A texel with alpha 0 is no pixel at all:
//     no colour, no coverage and no depth (gx.cpp's "transparent texel"
//     continue). The effective alpha is then the integer (poly_alpha * texel_a +
//     127) / 255 in 0..31, and at the setting this is gated on -- point sampling
//     -- an opaque-class polygon always lands on exactly 31, which makes the
//     blend below an exact replacement.
//
// COVERAGE TRAVELS IN THE POLYGON-ID TARGET, not in the colour target's alpha,
// because the colour target's alpha has to drive the blend. Bit 7 of the R8
// target says "this engine wrote this pixel" and bits 0..5 are the polygon ID
// the shadow pass reads back; a DS polygon ID is six bits, so bit 7 is free.

struct VSIn {
    float4 pos  : POSITION;    // clip space, built on the CPU as (xn*w, yn*w, z*w, w)
    float2 uv   : TEXCOORD0;   // already divided by the bound buffer's size
    float3 col  : COLOR0;      // vertex colour, 0..255 per channel
    float2 attr : TEXCOORD1;   // x = polygon alpha 0..31, y = polygon ID 0..63
};

struct VSOut {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    noperspective float3 col : COLOR0;
    nointerpolation float2 attr : TEXCOORD1;
};

VSOut vs_main(VSIn i)
{
    VSOut o;
    o.pos = i.pos;
    o.uv = i.uv;
    o.col = i.col;
    o.attr = i.attr;
    return o;
}

Texture2D tex0 : register(t0);
SamplerState smp0 : register(s0);

struct PSOut {
    float4 col : SV_TARGET0;
    uint id : SV_TARGET1;
};

PSOut ps_main(VSOut i)
{
    PSOut o;
    float4 t = tex0.Sample(smp0, i.uv);
    // An untextured polygon binds a one-pixel opaque white texture, so this is
    // the same code path with t == 1 and no branch: gx.cpp's own untextured
    // case is literally texel = 0xFFFFFFFF.
    float ta = floor(t.a * 255.0 + 0.5);
    clip(ta - 0.5);                       // alpha-0 texel: not a pixel
    float sa = floor((i.attr.x * ta + 127.0) / 255.0);
    float3 c = clamp(floor(i.col * t.rgb + 0.5), 0.0, 255.0) * (1.0 / 255.0);
    o.col = float4(c, min(sa, 31.0) * (1.0 / 31.0));
    o.id = 0x80u | (uint)(i.attr.y + 0.5);
    return o;
}
