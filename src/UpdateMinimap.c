// UpdateMinimap: copies a 16-byte descriptor into data_0209f3c8 and stores
// the four scalar params into data_0209f3c4 (+0x14..+0x20).
typedef unsigned int u32;

typedef struct MinimapDesc { u32 a, b, c, d; } MinimapDesc;

typedef struct MinimapState {
    char _pad[0x14];
    volatile u32 f14, f18, f1c, f20;
} MinimapState;

extern MinimapDesc data_0209f3c8;
extern MinimapState data_0209f3c4;

void UpdateMinimap(MinimapDesc *desc, u32 a, u32 b, u32 c, u32 d)
{
    *(MinimapDesc *)(((long long)(int)&data_0209f3c8) & 0xFFFFFFFFFFFFFFFFLL) = *desc;
    data_0209f3c4.f14 = a;
    data_0209f3c4.f18 = b;
    data_0209f3c4.f1c = c;
    data_0209f3c4.f20 = d;
}
