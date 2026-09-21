/* THE OPTIONAL GRAPHICS-CARD RENDERER. See hal/gpu_raster.h for what the
 * setting is and hal/gpu_device.h for the one device it shares with the
 * present path.
 *
 * ============================ THE SEAM ====================================
 *
 * ntr/gx.cpp draws a frame in two passes over the same buffers: an OPAQUE pass
 * (colour into the framebuffer, a float depth with a LESS test, a per-pixel
 * polygon ID, the 3D coverage mask) and then a TRANSLUCENT pass (blended
 * polygons with the equal-ID refusal, and mode-3 shadow volumes with their
 * stencil protocol) which READS what the opaque pass left. The opaque pass is
 * nearly all of the fill and none of the awkward state.
 *
 * So this file draws the opaque pass, and only the opaque pass, into three
 * offscreen targets and reads them straight back into the very buffers the
 * software opaque pass would have filled. gx.cpp then runs its translucent
 * pass, unchanged, over them. Everything downstream -- the edge smoothing, the
 * 2D compositor, the DS display capture, the selftest bitmap, the present path
 * -- sees an ordinary finished frame and cannot tell which rasteriser drew it.
 *
 * ========================= MATCHING THE SOFTWARE ==========================
 *
 * GEOMETRY arrives already in host screen space: x and y in pixels of the
 * active render size, z in [0,1], w kept, u and v in texel units. The clip
 * position is rebuilt on the CPU as (x_ndc * w, y_ndc * w, z * w, w) so that
 *   - u and v interpolate perspective-correct, which is what gx.cpp does
 *     (it interpolates u/w, v/w and 1/w and divides), and
 *   - SV_Position.z comes out as the plain screen-space interpolation of z,
 *     which is what gx.cpp's depth is (l0*a.z + l1*b.z + l2*c.z, no 1/w).
 * The vertex colour is declared noperspective for the same reason: gx.cpp
 * interpolates colour screen-linear. All three were read out of the raster
 * before this was written; see hal/gpu_raster.hlsl.
 *
 * DEPTH is D32_FLOAT with a LESS comparison, which keeps the FIRST triangle
 * submitted at a tie exactly as gx.cpp's `if (z >= depth) continue` does.
 * DepthClipEnable is FALSE because the software raster has no z clipping at
 * all: the near clip is done on the CPU before a triangle ever gets here and
 * there is no far clip.
 *
 * CULLING stays on the CPU, from the same signed screen area and the same
 * POLYGON_ATTR bits, and the card is told to cull nothing. A degenerate
 * triangle is dropped by the same 1e-6 test.
 *
 * THE SCISSOR is the present rectangle, which is what gx.cpp clamps its
 * bounding box to.
 *
 * COVERAGE TRAVELS IN THE POLYGON-ID TARGET. gx.cpp keeps a one-byte coverage
 * mask beside the framebuffer, and the 2D compositor needs it to know which
 * pixels the 3D engine wrote. It cannot come from the colour target's alpha
 * here because that alpha drives the blend, so the R8_UINT ID target carries
 * 0x80 | polygon_id: bit 7 is "this engine wrote this pixel" and the low six
 * bits are the DS's six-bit polygon ID.
 *
 * THE CLEAR COLOUR is read out of the framebuffer at the top of the frame
 * rather than hard-coded, and the readback COPIES ONLY COVERED PIXELS, so a
 * pixel the opaque pass did not reach keeps exactly the bytes the caller's own
 * clear put there. That is what makes this independent of whether the caller's
 * clear is one colour or many.
 *
 * ========================== WHERE IT DIFFERS ==============================
 *
 * 1. THE FILL RULE. This card uses Direct3D's top-left rule, one pixel one
 *    triangle; gx.cpp accepts a pixel on every edge, so two triangles sharing
 *    an edge both cover it and the depth test settles them. Along a shared
 *    edge both triangles interpolate to the same values, so the colour agrees
 *    to within float rounding; on a silhouette the two rasterisers disagree by
 *    whole pixels. That is why this is not a byte gate and why the in-process
 *    A/B excludes pixels within one pixel of a coverage or ID edge.
 *
 * 2. TEXTUREFILTER 1 AND 2. With filtering on, a filtered texel's alpha can
 *    land between 0 and full, and gx.cpp then takes its BLENDING branch inside
 *    the opaque pass: it blends without writing depth and records a
 *    translucent attribute. Here the blend happens in the card's blend unit
 *    and the depth is written. At TextureFilter 0 -- the arm this is gated on
 *    -- an opaque-class polygon's texel alpha is only ever 0 or full, so the
 *    effective alpha is always 31, the blend is an exact replacement and the
 *    depth write is the same depth write. The difference exists only on the
 *    filtered arms, which are reported separately and gate nothing.
 *
 * 3. MIP CHAINS. TextureFilter 2 asks the card to generate the chain, which is
 *    a plain box filter; gx.cpp builds its own chain in premultiplied alpha so
 *    a cut-out does not drag its transparent colour outward. Same arm, same
 *    reason, not gated.
 *
 * 4. THE DEGENERATE-W FALLBACK. gx.cpp falls back to an affine UV when the
 *    interpolated 1/w is under 1e-9. The card has no such case; a vertex with
 *    w at zero is left to the hardware.
 *
 * ============================== FAILURE ===================================
 *
 * Every call is checked. The first failure prints one plain line and sets a
 * flag; from then on this file answers "I did not draw" and gx.cpp runs its
 * own opaque pass for that frame and every frame after. Nothing is left half
 * written, because the CPU buffers are only touched after all three readbacks
 * have succeeded.
 */
#if defined(_WIN32)

#include <windows.h>
#include <d3d11.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include <vector>

#include "hal/gpu_raster.h"
#include "hal/gpu_device.h"
#include "hal/gpu_raster_shaders.h"
#include "hal/host_settings.h"
#include "ntr/gx.h"

namespace {

/* ---- the settings and the test knobs, read once -------------------------- */

int g_on;              /* the key said yes */
int g_started;
int g_down;            /* fallen back for the rest of the run */
int g_want_warp = -1;  /* SM64DS_RENDERER_DEVICE */
int g_perf;            /* SM64DS_RENDERER_PERF: the cost breakdown */
int g_fail_after;      /* SM64DS_RENDERER_FAIL_AFTER: the fallback drill */
int g_fail_readback;   /* SM64DS_RENDERER_FAIL_READBACK: the other drill */
int g_addrcheck;       /* SM64DS_RENDERER_ADDRCHECK: the six DS address ranges */

/* ---- what the run learned, for the one line at exit ---------------------- */

long long g_frames, g_tris, g_batches, g_verts;
double    g_ms_build, g_ms_draw, g_ms_read;
int       g_perf_n;
double    g_perf_build, g_perf_draw, g_perf_read;
long long g_perf_tris, g_perf_batches;

/* ---- the device and the pipeline ----------------------------------------- */

ID3D11Device        *g_dev;
ID3D11DeviceContext *g_ctx;

ID3D11VertexShader   *g_vs;
ID3D11PixelShader    *g_ps;
ID3D11InputLayout    *g_layout;
ID3D11RasterizerState *g_rast;
ID3D11DepthStencilState *g_dss;
ID3D11BlendState     *g_blend;
ID3D11Buffer         *g_vb;
UINT                  g_vb_verts;

/* [filter][addressU][addressV]; address 0 clamp, 1 wrap, 2 mirror */
ID3D11SamplerState   *g_smp[3][3][3];

/* the one-pixel opaque white texture an untextured polygon binds, so the
   shader has no branch and gx.cpp's own `texel = 0xFFFFFFFF` is literally
   what it samples */
ID3D11Texture2D          *g_white;
ID3D11ShaderResourceView *g_white_srv;

/* ---- the three targets and their staging copies -------------------------- */

ID3D11Texture2D        *g_col, *g_col_stage;
ID3D11RenderTargetView *g_col_rtv;
ID3D11Texture2D        *g_id, *g_id_stage;
ID3D11RenderTargetView *g_id_rtv;
ID3D11Texture2D        *g_dep, *g_dep_stage;
ID3D11DepthStencilView *g_dep_dsv;
int                     g_rt_w, g_rt_h;

/* ---- the texture cache on the card --------------------------------------- */

struct GTex {
    ID3D11Texture2D          *tex;
    ID3D11ShaderResourceView *srv;
    int w, h;
};
std::map<uint32_t, GTex> g_tex;
uint32_t g_tex_gen_seen;
size_t   g_tex_bytes;

/* ---- small helpers ------------------------------------------------------- */

long long qpc()
{
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return c.QuadPart;
}
long long g_qpf;
double ms_between(long long a, long long b)
{
    if (!g_qpf) return 0.0;
    return (double)(b - a) * 1000.0 / (double)g_qpf;
}

int env_int(const char *name, int dflt)
{
    const char *e = getenv(name);
    if (!e || !*e) return dflt;
    char *end = 0;
    const long v = strtol(e, &end, 10);
    return (end != e) ? (int)v : dflt;
}

/* ONE LINE, AND THEN THIS RUN DRAWS IN SOFTWARE FOR GOOD. Never an exit, never
   a stall: the caller returns 0 and gx.cpp's own opaque pass draws the frame
   that was about to be drawn here, so not even one picture is lost. */
void fall_back(const char *why, HRESULT hr)
{
    if (g_down) return;
    g_down = 1;
    if (hr)
        fprintf(stderr, "[renderer] the graphics card stopped drawing the 3D "
                        "picture (%s, code %08x). This run goes back to drawing "
                        "it the ordinary way; nothing else changes.\n",
                why, (unsigned)hr);
    else
        fprintf(stderr, "[renderer] the graphics card cannot draw the 3D picture "
                        "(%s). This run draws it the ordinary way; nothing else "
                        "changes.\n", why);
}

/* ---- the pipeline objects, made once ------------------------------------- */

bool make_pipeline()
{
    HRESULT hr = g_dev->CreateVertexShader(kGpuRasterVS, sizeof kGpuRasterVS, 0,
                                           &g_vs);
    if (FAILED(hr)) { fall_back("the vertex shader would not load", hr); return false; }
    hr = g_dev->CreatePixelShader(kGpuRasterPS, sizeof kGpuRasterPS, 0, &g_ps);
    if (FAILED(hr)) { fall_back("the pixel shader would not load", hr); return false; }

    const D3D11_INPUT_ELEMENT_DESC el[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16,
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24,
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 36,
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = g_dev->CreateInputLayout(el, 4, kGpuRasterVS, sizeof kGpuRasterVS,
                                  &g_layout);
    if (FAILED(hr)) { fall_back("the vertex layout was refused", hr); return false; }

    /* CULL_NONE: the cull test is done on the CPU from the same signed screen
       area and the same POLYGON_ATTR bits gx.cpp uses, so by the time a
       triangle reaches the card it has already been kept or dropped.
       DepthClipEnable FALSE: the software raster has no z clip at all.
       ScissorEnable TRUE: the present rectangle. */
    D3D11_RASTERIZER_DESC rd;
    memset(&rd, 0, sizeof rd);
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = FALSE;
    rd.ScissorEnable = TRUE;
    hr = g_dev->CreateRasterizerState(&rd, &g_rast);
    if (FAILED(hr)) { fall_back("no rasterizer state", hr); return false; }

    D3D11_DEPTH_STENCIL_DESC dd;
    memset(&dd, 0, sizeof dd);
    dd.DepthEnable = TRUE;
    dd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dd.DepthFunc = D3D11_COMPARISON_LESS;
    dd.StencilEnable = FALSE;
    hr = g_dev->CreateDepthStencilState(&dd, &g_dss);
    if (FAILED(hr)) { fall_back("no depth state", hr); return false; }

    /* The colour target blends by the shader's own alpha, which is the DS's
       effective alpha over 31. At TextureFilter 0 that alpha is always 1 and
       this is an exact replacement; see WHERE IT DIFFERS at the top. The ID
       target has blending off because an integer target cannot blend, which is
       what IndependentBlendEnable is here for. */
    D3D11_BLEND_DESC bd;
    memset(&bd, 0, sizeof bd);
    bd.IndependentBlendEnable = TRUE;
    bd.RenderTarget[0].BlendEnable = TRUE;
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    bd.RenderTarget[1].BlendEnable = FALSE;
    bd.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = g_dev->CreateBlendState(&bd, &g_blend);
    if (FAILED(hr)) { fall_back("no blend state", hr); return false; }

    const uint32_t white = 0xFFFFFFFFu;
    D3D11_TEXTURE2D_DESC wd;
    memset(&wd, 0, sizeof wd);
    wd.Width = wd.Height = 1;
    wd.MipLevels = wd.ArraySize = 1;
    wd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    wd.SampleDesc.Count = 1;
    wd.Usage = D3D11_USAGE_IMMUTABLE;
    wd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    D3D11_SUBRESOURCE_DATA wsd;
    memset(&wsd, 0, sizeof wsd);
    wsd.pSysMem = &white;
    wsd.SysMemPitch = 4;
    hr = g_dev->CreateTexture2D(&wd, &wsd, &g_white);
    if (FAILED(hr)) { fall_back("no untextured stand-in", hr); return false; }
    hr = g_dev->CreateShaderResourceView(g_white, 0, &g_white_srv);
    if (FAILED(hr)) { fall_back("no untextured stand-in view", hr); return false; }
    return true;
}

/* WRAP, CLAMP AND MIRROR ARE PER AXIS, which is ntr/gx.cpp's tex_coord_i: a
   cleared repeat bit is CLAMP, repeat without flip is WRAP and repeat with
   flip is MIRROR. Built on demand and kept, so a level pays for the handful of
   combinations it actually uses. */
ID3D11SamplerState *sampler(int filter, int au, int av)
{
    if (filter < 0) filter = 0;
    if (filter > 2) filter = 2;
    if (g_smp[filter][au][av]) return g_smp[filter][au][av];
    static const D3D11_TEXTURE_ADDRESS_MODE kMode[3] = {
        D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_MIRROR,
    };
    D3D11_SAMPLER_DESC sd;
    memset(&sd, 0, sizeof sd);
    sd.AddressU = kMode[au];
    sd.AddressV = kMode[av];
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sd.MaxLOD = D3D11_FLOAT32_MAX;
    /* 0 is the DS's own nearest sampling; 1 is bilinear with no chain, so the
       mip level is pinned at the top; 2 is the chain plus the blend between
       two of its levels, which is what trilinear is. */
    if (filter == 0) sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    else if (filter == 1) {
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.MaxLOD = 0.0f;
    } else sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    if (FAILED(g_dev->CreateSamplerState(&sd, &g_smp[filter][au][av])))
        return 0;
    return g_smp[filter][au][av];
}

/* ---- the three targets --------------------------------------------------- */

void release_targets()
{
    if (g_col_rtv) { g_col_rtv->Release(); g_col_rtv = 0; }
    if (g_col_stage) { g_col_stage->Release(); g_col_stage = 0; }
    if (g_col) { g_col->Release(); g_col = 0; }
    if (g_id_rtv) { g_id_rtv->Release(); g_id_rtv = 0; }
    if (g_id_stage) { g_id_stage->Release(); g_id_stage = 0; }
    if (g_id) { g_id->Release(); g_id = 0; }
    if (g_dep_dsv) { g_dep_dsv->Release(); g_dep_dsv = 0; }
    if (g_dep_stage) { g_dep_stage->Release(); g_dep_stage = 0; }
    if (g_dep) { g_dep->Release(); g_dep = 0; }
    g_rt_w = g_rt_h = 0;
}

bool ensure_targets(int w, int h)
{
    if (g_col && g_rt_w == w && g_rt_h == h) return true;
    release_targets();

    D3D11_TEXTURE2D_DESC d;
    memset(&d, 0, sizeof d);
    d.Width = (UINT)w;
    d.Height = (UINT)h;
    d.MipLevels = d.ArraySize = 1;
    d.SampleDesc.Count = 1;
    d.Usage = D3D11_USAGE_DEFAULT;

    /* colour: the framebuffer's own byte order */
    d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    d.BindFlags = D3D11_BIND_RENDER_TARGET;
    HRESULT hr = g_dev->CreateTexture2D(&d, 0, &g_col);
    if (FAILED(hr)) { fall_back("no colour target", hr); return false; }
    hr = g_dev->CreateRenderTargetView(g_col, 0, &g_col_rtv);
    if (FAILED(hr)) { fall_back("no colour target view", hr); return false; }

    /* polygon ID plus the coverage bit */
    d.Format = DXGI_FORMAT_R8_UINT;
    hr = g_dev->CreateTexture2D(&d, 0, &g_id);
    if (FAILED(hr)) { fall_back("no polygon-id target", hr); return false; }
    hr = g_dev->CreateRenderTargetView(g_id, 0, &g_id_rtv);
    if (FAILED(hr)) { fall_back("no polygon-id target view", hr); return false; }

    /* depth: typeless so the same texture can be a depth view for drawing and
       a plain float copy for reading back */
    d.Format = DXGI_FORMAT_R32_TYPELESS;
    d.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = g_dev->CreateTexture2D(&d, 0, &g_dep);
    if (FAILED(hr)) { fall_back("no depth target", hr); return false; }
    D3D11_DEPTH_STENCIL_VIEW_DESC vd;
    memset(&vd, 0, sizeof vd);
    vd.Format = DXGI_FORMAT_D32_FLOAT;
    vd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    hr = g_dev->CreateDepthStencilView(g_dep, &vd, &g_dep_dsv);
    if (FAILED(hr)) { fall_back("no depth target view", hr); return false; }

    /* the three staging copies the frame is read back through */
    d.BindFlags = 0;
    d.Usage = D3D11_USAGE_STAGING;
    d.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    hr = g_dev->CreateTexture2D(&d, 0, &g_col_stage);
    if (FAILED(hr)) { fall_back("no colour readback buffer", hr); return false; }
    d.Format = DXGI_FORMAT_R8_UINT;
    hr = g_dev->CreateTexture2D(&d, 0, &g_id_stage);
    if (FAILED(hr)) { fall_back("no polygon-id readback buffer", hr); return false; }
    d.Format = DXGI_FORMAT_R32_TYPELESS;
    hr = g_dev->CreateTexture2D(&d, 0, &g_dep_stage);
    if (FAILED(hr)) { fall_back("no depth readback buffer", hr); return false; }

    g_rt_w = w;
    g_rt_h = h;
    return true;
}

/* ---- the texture cache --------------------------------------------------- */

void release_textures()
{
    for (std::map<uint32_t, GTex>::iterator it = g_tex.begin();
         it != g_tex.end(); ++it) {
        if (it->second.srv) it->second.srv->Release();
        if (it->second.tex) it->second.tex->Release();
    }
    g_tex.clear();
    g_tex_bytes = 0;
}

/* KEYED ON A MONOTONIC ID, NOT ON THE PIXEL POINTER. ntr/gx.cpp's decode cache
   frees and reuses its buffers, so a pointer that named one texture a moment
   ago can name a different one now; the id is handed out at bind time and
   never reused, and gx_invalidate_textures bumps a generation that throws this
   whole cache away. */
const GTex *upload_texture(uint32_t id, const uint32_t *px, int w, int h,
                           int filter)
{
    std::map<uint32_t, GTex>::iterator it = g_tex.find(id);
    if (it != g_tex.end()) return &it->second;
    if (!px || w <= 0 || h <= 0) return 0;

    GTex t;
    memset(&t, 0, sizeof t);
    t.w = w;
    t.h = h;

    D3D11_TEXTURE2D_DESC d;
    memset(&d, 0, sizeof d);
    d.Width = (UINT)w;
    d.Height = (UINT)h;
    d.ArraySize = 1;
    d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;   /* 0xAARRGGBB in memory is BGRA */
    d.SampleDesc.Count = 1;
    d.Usage = D3D11_USAGE_DEFAULT;
    d.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    const bool mips = (filter >= 2);
    HRESULT hr;
    if (mips) {
        /* the card builds the chain; see WHERE IT DIFFERS at the top for how
           that chain differs from the one ntr/gx.cpp builds for itself */
        d.MipLevels = 0;
        d.BindFlags |= D3D11_BIND_RENDER_TARGET;
        d.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        hr = g_dev->CreateTexture2D(&d, 0, &t.tex);
    } else {
        d.MipLevels = 1;
        D3D11_SUBRESOURCE_DATA sd;
        memset(&sd, 0, sizeof sd);
        sd.pSysMem = px;
        sd.SysMemPitch = (UINT)w * 4u;
        hr = g_dev->CreateTexture2D(&d, &sd, &t.tex);
    }
    if (FAILED(hr) || !t.tex) { fall_back("a texture would not upload", hr); return 0; }
    hr = g_dev->CreateShaderResourceView(t.tex, 0, &t.srv);
    if (FAILED(hr) || !t.srv) {
        t.tex->Release();
        fall_back("a texture view was refused", hr);
        return 0;
    }
    if (mips) {
        g_ctx->UpdateSubresource(t.tex, 0, 0, px, (UINT)w * 4u, 0);
        g_ctx->GenerateMips(t.srv);
    }
    g_tex_bytes += (size_t)w * (size_t)h * 4u * (mips ? 2u : 1u);
    return &g_tex.insert(std::make_pair(id, t)).first->second;
}

/* ---- the vertex buffer --------------------------------------------------- */

struct GVert {
    float x, y, z, w;
    float u, v;
    float r, g, b;      /* 0..255, the units ntr/gx.cpp interpolates in */
    float pa, pid;      /* polygon alpha 0..31 and polygon ID 0..63 */
};
std::vector<GVert> g_scratch;

struct Batch {
    const GTex *tex;
    int au, av;
    UINT first, count;
};
std::vector<Batch> g_batch;

bool ensure_vb(size_t verts)
{
    if (g_vb && g_vb_verts >= verts) return true;
    if (g_vb) { g_vb->Release(); g_vb = 0; g_vb_verts = 0; }
    size_t want = verts + verts / 2 + 1024;
    D3D11_BUFFER_DESC bd;
    memset(&bd, 0, sizeof bd);
    bd.ByteWidth = (UINT)(want * sizeof(GVert));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    const HRESULT hr = g_dev->CreateBuffer(&bd, 0, &g_vb);
    if (FAILED(hr) || !g_vb) { fall_back("no vertex buffer", hr); return false; }
    g_vb_verts = (UINT)want;
    return true;
}

/* ---- the one-time start-up ----------------------------------------------- */

void at_exit();

bool start()
{
    if (g_down) return false;
    if (g_dev) return true;
    if (port_gpu_device_failed()) { fall_back("no Direct3D device would start", 0); return false; }
    port_gpu_device_address_rows("before", g_addrcheck);
    if (!port_gpu_device_acquire(g_want_warp)) {
        fall_back("no Direct3D device would start", 0);
        return false;
    }
    g_dev = (ID3D11Device *)port_gpu_device();
    g_ctx = (ID3D11DeviceContext *)port_gpu_device_context();
    if (!g_dev || !g_ctx) { fall_back("the Direct3D device went missing", 0); return false; }
    LARGE_INTEGER f;
    if (QueryPerformanceFrequency(&f)) g_qpf = f.QuadPart;
    fprintf(stderr, "[renderer] the 3D picture is drawn by \"%s\"%s, feature "
            "level %u_%u, ready in %.1f ms. The translucent polygons, the "
            "shadows and everything after them still run here.\n",
            port_gpu_device_adapter()[0] ? port_gpu_device_adapter() : "(unnamed)",
            port_gpu_device_is_warp() ? " (WARP, the software device Windows ships)"
                                      : "",
            (unsigned)((port_gpu_device_feature_level() >> 12) & 0xf),
            (unsigned)((port_gpu_device_feature_level() >> 8) & 0xf),
            port_gpu_device_create_ms());
    if (!make_pipeline()) return false;
    port_gpu_device_address_rows("after ", g_addrcheck);
    atexit(at_exit);
    return true;
}

void at_exit()
{
    if (g_frames)
        fprintf(stderr, "[renderer] %lld frame(s) drawn on the card: %lld "
                "triangle(s) in %lld batch(es), %.3f ms building them, %.3f ms "
                "submitting, %.3f ms reading the picture back, per frame. "
                "Textures held on the card: %u KB.\n",
                g_frames, g_tris, g_batches, g_ms_build / (double)g_frames,
                g_ms_draw / (double)g_frames, g_ms_read / (double)g_frames,
                (unsigned)(g_tex_bytes / 1024));
    /* THE ADDRESS ROWS WITH THE THREE TARGETS STILL ALIVE, which is the point
       of taking them here rather than after the release below: a 32-bit
       process that lost one of the six DS ranges to a driver's own mapping is
       a process whose game cannot run, and the render targets are the biggest
       thing this file ever asks a driver for. */
    port_gpu_device_address_rows("exit  ", g_addrcheck);
    release_textures();
    release_targets();
    if (g_vb) { g_vb->Release(); g_vb = 0; }
    if (g_white_srv) { g_white_srv->Release(); g_white_srv = 0; }
    if (g_white) { g_white->Release(); g_white = 0; }
    if (g_blend) { g_blend->Release(); g_blend = 0; }
    if (g_dss) { g_dss->Release(); g_dss = 0; }
    if (g_rast) { g_rast->Release(); g_rast = 0; }
    for (int a = 0; a < 3; ++a)
        for (int b = 0; b < 3; ++b)
            for (int c = 0; c < 3; ++c)
                if (g_smp[a][b][c]) { g_smp[a][b][c]->Release(); g_smp[a][b][c] = 0; }
    if (g_layout) { g_layout->Release(); g_layout = 0; }
    if (g_ps) { g_ps->Release(); g_ps = 0; }
    if (g_vs) { g_vs->Release(); g_vs = 0; }
    /* the device itself belongs to hal/gpu_device.cpp and is released there */
}

/* ---- the frame ----------------------------------------------------------- */

int draw_frame(const ntr::GxGpuFrame *f)
{
    if (g_down || !f) return 0;
    if (!start()) return 0;
    if (f->cw <= 0 || f->ch <= 0 || f->pw <= 0 || f->ph <= 0) return 0;

    if (g_fail_after && g_frames >= g_fail_after) {
        fprintf(stderr, "[renderer] SM64DS_RENDERER_FAIL_AFTER=%d reached after "
                "%lld frame(s); forcing the fallback on purpose\n",
                g_fail_after, g_frames);
        fall_back("a test asked for it", 0);
        return 0;
    }

    if (f->tex_generation != g_tex_gen_seen) {
        g_tex_gen_seen = f->tex_generation;
        release_textures();
    }
    if (!ensure_targets(f->cw, f->ch)) return 0;

    const long long t0 = qpc();

    /* ---- build the batches, in submission order ---------------------------
       Consecutive triangles that share a texture and a wrap mode go into one
       draw. The order within the list is the order the game submitted them,
       which is the order ntr/gx.cpp draws them, and the depth test settles the
       rest. */
    g_scratch.clear();
    g_batch.clear();
    const GTex *cur_tex = 0;
    int cur_au = -1, cur_av = -1;
    bool open = false;

    const float inv_cw = 2.0f / (float)f->cw;
    const float inv_ch = 2.0f / (float)f->ch;

    for (size_t i = 0; i < f->count; ++i) {
        const ntr::GxTriangle &t = f->tris[i];
        if (t.translucent) continue;          /* the second pass' business */
        if (t.mode == 3) continue;            /* a shadow volume, likewise */

        const ntr::GxVertex &a = t.v[0], &b = t.v[1], &c = t.v[2];
        /* THE SAME CULL TEST, IN THE SAME ORDER, as ntr/gx.cpp's band loop. */
        const float area = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
        if (area < 1e-6f && area > -1e-6f) continue;
        const bool backface = area > 0.0f;
        if (backface && !(t.cull & 1)) continue;
        if (!backface && !(t.cull & 2)) continue;

        const bool textured = t.tex && t.tw > 0 && t.th > 0;
        const GTex *gt = 0;
        int au = 0, av = 0;
        if (textured) {
            gt = upload_texture(t.tex_id, t.tex, t.tw, t.th, f->tex_filter);
            if (!gt) return 0;                /* fall_back already said why */
            const bool rep_s = (t.wrap & 1) != 0, rep_t = (t.wrap & 2) != 0;
            const bool flip_s = (t.wrap & 4) != 0, flip_t = (t.wrap & 8) != 0;
            au = rep_s ? (flip_s ? 2 : 1) : 0;
            av = rep_t ? (flip_t ? 2 : 1) : 0;
        }
        if (!open || gt != cur_tex || au != cur_au || av != cur_av) {
            if (open) g_batch.back().count =
                (UINT)g_scratch.size() - g_batch.back().first;
            Batch nb;
            nb.tex = gt;
            nb.au = au;
            nb.av = av;
            nb.first = (UINT)g_scratch.size();
            nb.count = 0;
            g_batch.push_back(nb);
            cur_tex = gt;
            cur_au = au;
            cur_av = av;
            open = true;
        }

        const float tsc = (float)(t.tex_scale ? t.tex_scale : 1);
        const float iw = textured ? 1.0f / (float)t.tw : 0.0f;
        const float ih = textured ? 1.0f / (float)t.th : 0.0f;
        const float pa = (float)((t.alpha >= 31 || t.alpha == 0) ? 31u : t.alpha);
        const float pid = (float)t.polyid;

        for (int k = 0; k < 3; ++k) {
            const ntr::GxVertex &v = t.v[k];
            GVert o;
            /* SCREEN PIXELS TO CLIP SPACE. The viewport is the whole active
               picture, so a screen x maps to 2x/cw - 1 and a screen y to
               1 - 2y/ch; multiplying through by w is what makes the card
               interpolate u and v perspective-correct and z screen-linear,
               which is the pair ntr/gx.cpp produces. */
            const float xn = (float)v.x * inv_cw - 1.0f;
            const float yn = 1.0f - (float)v.y * inv_ch;
            o.x = xn * v.w;
            o.y = yn * v.w;
            o.z = v.z * v.w;
            o.w = v.w;
            o.u = textured ? v.u * tsc * iw : 0.0f;
            o.v = textured ? v.v * tsc * ih : 0.0f;
            o.r = (float)((v.color >> 16) & 0xFF);
            o.g = (float)((v.color >> 8) & 0xFF);
            o.b = (float)(v.color & 0xFF);
            o.pa = pa;
            o.pid = pid;
            g_scratch.push_back(o);
        }
    }
    if (open)
        g_batch.back().count = (UINT)g_scratch.size() - g_batch.back().first;

    const long long t1 = qpc();

    /* ---- clear, submit ---------------------------------------------------- */

    /* THE CLEAR COLOUR IS THE FRAMEBUFFER'S OWN, read at the top of the frame
       rather than assumed, and the alpha is meaningless here because coverage
       travels in the ID target. It only matters at all on a filtered arm,
       where a partly transparent texel blends against it; the readback below
       copies covered pixels only, so an uncovered pixel keeps exactly the
       bytes the caller's clear left. */
    const float clr[4] = {
        (float)((f->clear_argb >> 16) & 0xFF) / 255.0f,
        (float)((f->clear_argb >> 8) & 0xFF) / 255.0f,
        (float)(f->clear_argb & 0xFF) / 255.0f,
        0.0f,
    };
    const float zero[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_ctx->ClearRenderTargetView(g_col_rtv, clr);
    g_ctx->ClearRenderTargetView(g_id_rtv, zero);
    g_ctx->ClearDepthStencilView(g_dep_dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

    if (!g_scratch.empty()) {
        if (!ensure_vb(g_scratch.size())) return 0;
        D3D11_MAPPED_SUBRESOURCE ms;
        memset(&ms, 0, sizeof ms);
        HRESULT hr = g_ctx->Map(g_vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
        if (FAILED(hr)) { fall_back("the vertex buffer would not open", hr); return 0; }
        memcpy(ms.pData, &g_scratch[0], g_scratch.size() * sizeof(GVert));
        g_ctx->Unmap(g_vb, 0);

        ID3D11RenderTargetView *rtvs[2] = { g_col_rtv, g_id_rtv };
        g_ctx->OMSetRenderTargets(2, rtvs, g_dep_dsv);
        D3D11_VIEWPORT vp;
        vp.TopLeftX = 0.0f;
        vp.TopLeftY = 0.0f;
        vp.Width = (float)f->cw;
        vp.Height = (float)f->ch;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        g_ctx->RSSetViewports(1, &vp);
        D3D11_RECT sc;
        sc.left = f->px0;
        sc.top = f->py0;
        sc.right = f->px0 + f->pw;
        sc.bottom = f->py0 + f->ph;
        g_ctx->RSSetScissorRects(1, &sc);
        g_ctx->RSSetState(g_rast);
        g_ctx->OMSetDepthStencilState(g_dss, 0);
        const float bf[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_ctx->OMSetBlendState(g_blend, bf, 0xffffffffu);
        g_ctx->IASetInputLayout(g_layout);
        g_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        const UINT stride = (UINT)sizeof(GVert), offset = 0;
        g_ctx->IASetVertexBuffers(0, 1, &g_vb, &stride, &offset);
        g_ctx->VSSetShader(g_vs, 0, 0);
        g_ctx->PSSetShader(g_ps, 0, 0);

        for (size_t bi = 0; bi < g_batch.size(); ++bi) {
            const Batch &bt = g_batch[bi];
            if (!bt.count) continue;
            ID3D11ShaderResourceView *srv = bt.tex ? bt.tex->srv : g_white_srv;
            ID3D11SamplerState *smp =
                bt.tex ? sampler(f->tex_filter, bt.au, bt.av) : sampler(0, 0, 0);
            if (!smp) { fall_back("no sampler state", 0); return 0; }
            g_ctx->PSSetShaderResources(0, 1, &srv);
            g_ctx->PSSetSamplers(0, 1, &smp);
            g_ctx->Draw(bt.count, bt.first);
            ++g_batches;
        }
        ID3D11ShaderResourceView *none = 0;
        g_ctx->PSSetShaderResources(0, 1, &none);
        g_ctx->OMSetRenderTargets(0, 0, 0);
    }

    const long long t2 = qpc();

    /* ---- read the picture back ------------------------------------------- */

    if (g_fail_readback && g_frames >= g_fail_readback) {
        fprintf(stderr, "[renderer] SM64DS_RENDERER_FAIL_READBACK=%d reached "
                "after %lld frame(s); forcing a readback failure on purpose\n",
                g_fail_readback, g_frames);
        fall_back("a test asked the readback to fail", 0);
        return 0;
    }

    g_ctx->CopyResource(g_col_stage, g_col);
    g_ctx->CopyResource(g_id_stage, g_id);
    if (f->want_depth) g_ctx->CopyResource(g_dep_stage, g_dep);

    D3D11_MAPPED_SUBRESOURCE mc, mi, md;
    memset(&mc, 0, sizeof mc);
    memset(&mi, 0, sizeof mi);
    memset(&md, 0, sizeof md);
    HRESULT hr = g_ctx->Map(g_col_stage, 0, D3D11_MAP_READ, 0, &mc);
    if (FAILED(hr)) { fall_back("the picture could not be read back", hr); return 0; }
    hr = g_ctx->Map(g_id_stage, 0, D3D11_MAP_READ, 0, &mi);
    if (FAILED(hr)) {
        g_ctx->Unmap(g_col_stage, 0);
        fall_back("the polygon ids could not be read back", hr);
        return 0;
    }
    if (f->want_depth) {
        hr = g_ctx->Map(g_dep_stage, 0, D3D11_MAP_READ, 0, &md);
        if (FAILED(hr)) {
            g_ctx->Unmap(g_id_stage, 0);
            g_ctx->Unmap(g_col_stage, 0);
            fall_back("the depth could not be read back", hr);
            return 0;
        }
    }

    /* NOTHING ON THE CPU SIDE HAS BEEN TOUCHED UNTIL HERE, which is what makes
       a failure above safe: the software pass then draws the frame over
       buffers that are still exactly as it cleared them. */
    const int x0 = f->px0, x1 = f->px0 + f->pw;
    const int y0 = f->py0, y1 = f->py0 + f->ph;
    for (int y = y0; y < y1; ++y) {
        const uint32_t *crow =
            (const uint32_t *)((const unsigned char *)mc.pData + (size_t)y * mc.RowPitch);
        const unsigned char *irow =
            (const unsigned char *)mi.pData + (size_t)y * mi.RowPitch;
        const float *drow = f->want_depth
            ? (const float *)((const unsigned char *)md.pData + (size_t)y * md.RowPitch)
            : 0;
        uint32_t *fbrow = f->fb + (size_t)y * f->stride;
        uint8_t *cvrow = f->cover + (size_t)y * f->stride;
        uint8_t *idrow = f->attrid + (size_t)y * f->stride;
        float *dprow = f->depth + (size_t)y * f->stride;
        for (int x = x0; x < x1; ++x) {
            const unsigned char idv = irow[x];
            if (!(idv & 0x80u)) continue;
            fbrow[x] = 0xFF000000u | (crow[x] & 0x00FFFFFFu);
            cvrow[x] = 1;
            if (f->want_attrid) idrow[x] = (uint8_t)(idv & 0x3Fu);
            if (drow) dprow[x] = drow[x];
        }
    }

    if (f->want_depth) g_ctx->Unmap(g_dep_stage, 0);
    g_ctx->Unmap(g_id_stage, 0);
    g_ctx->Unmap(g_col_stage, 0);

    const long long t3 = qpc();
    ++g_frames;
    g_tris += (long long)(g_scratch.size() / 3);
    g_verts += (long long)g_scratch.size();
    g_ms_build += ms_between(t0, t1);
    g_ms_draw += ms_between(t1, t2);
    g_ms_read += ms_between(t2, t3);

    if (g_perf) {
        g_perf_build += ms_between(t0, t1);
        g_perf_draw += ms_between(t1, t2);
        g_perf_read += ms_between(t2, t3);
        g_perf_tris += (long long)(g_scratch.size() / 3);
        g_perf_batches += (long long)g_batch.size();
        if (++g_perf_n >= 30) {
            fprintf(stderr, "[renderer] build %6.3fms submit %6.3fms readback "
                    "%6.3fms tris %6lld batches %4lld\n",
                    g_perf_build / g_perf_n, g_perf_draw / g_perf_n,
                    g_perf_read / g_perf_n, g_perf_tris / g_perf_n,
                    g_perf_batches / g_perf_n);
            g_perf_build = g_perf_draw = g_perf_read = 0.0;
            g_perf_tris = g_perf_batches = 0;
            g_perf_n = 0;
        }
    }
    return 1;
}

int backend(const ntr::GxGpuFrame *f) { return draw_frame(f); }

}  // namespace

extern "C" void port_gpu_raster_configure(void)
{
    if (g_started) return;
    g_started = 1;
    g_on = host_setting_renderer();
    if (!g_on) return;                 /* the whole default path stops here */

    const char *dev = getenv("SM64DS_RENDERER_DEVICE");
    if (dev && *dev) {
        if (!_stricmp(dev, "warp")) g_want_warp = 1;
        else if (!_stricmp(dev, "hw") || !_stricmp(dev, "hardware")) g_want_warp = 0;
    }
    g_perf = env_int("SM64DS_RENDERER_PERF", 0);
    g_fail_after = env_int("SM64DS_RENDERER_FAIL_AFTER", 0);
    g_fail_readback = env_int("SM64DS_RENDERER_FAIL_READBACK", 0);
    g_addrcheck = env_int("SM64DS_RENDERER_ADDRCHECK", 0);
    ntr::gx_set_gpu_opaque(&backend);
}

extern "C" int port_gpu_raster_active(void)
{
    return (g_on && !g_down) ? 1 : 0;
}

#else   /* not Windows: the port ships on Windows and there is no card here */

#include "hal/gpu_raster.h"

void port_gpu_raster_configure(void) {}
int port_gpu_raster_active(void) { return 0; }

#endif
