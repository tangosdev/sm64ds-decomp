#ifndef NITRO_GX_GX_LOAD_H
#define NITRO_GX_GX_LOAD_H
#ifndef __cplusplus
#define GX_EndLoadTexPltt _Z17GX_EndLoadTexPlttv
#define GX_LoadTexPltt _Z14GX_LoadTexPlttPKvjj
#define GX_BeginLoadTexPltt _Z19GX_BeginLoadTexPlttv
#define GX_EndLoadTex _Z13GX_EndLoadTexv
#define GX_LoadTex _Z10GX_LoadTexPKvjj
#define GX_BeginLoadTex _Z15GX_BeginLoadTexv
#define GXS_EndLoadOBJExtPltt _Z21GXS_EndLoadOBJExtPlttv
#define GXS_LoadOBJExtPltt _Z18GXS_LoadOBJExtPlttPKvjj
#define GXS_BeginLoadOBJExtPltt _Z23GXS_BeginLoadOBJExtPlttv
#define GXS_EndLoadBGExtPltt _Z20GXS_EndLoadBGExtPlttv
#define GXS_LoadBGExtPltt _Z17GXS_LoadBGExtPlttPKvjj
#define GXS_BeginLoadBGExtPltt _Z22GXS_BeginLoadBGExtPlttv
#define GXS_LoadBG3Char _Z15GXS_LoadBG3CharPKvjj
#define GX_LoadBG3Char _Z14GX_LoadBG3CharPKvjj
#define GXS_LoadBG2Char _Z15GXS_LoadBG2CharPKvjj
#define GX_LoadBG2Char _Z14GX_LoadBG2CharPKvjj
#define GXS_LoadBG1Char _Z15GXS_LoadBG1CharPKvjj
#define GX_LoadBG1Char _Z14GX_LoadBG1CharPKvjj
#define GXS_LoadBG0Char _Z15GXS_LoadBG0CharPKvjj
#define GX_LoadBG0Char _Z14GX_LoadBG0CharPKvjj
#define GXS_LoadBG3Scr _Z14GXS_LoadBG3ScrPKvjj
#define GX_LoadBG3Scr _Z13GX_LoadBG3ScrPKvjj
#define GXS_LoadBG2Scr _Z14GXS_LoadBG2ScrPKvjj
#define GX_LoadBG2Scr _Z13GX_LoadBG2ScrPKvjj
#define GXS_LoadBG1Scr _Z14GXS_LoadBG1ScrPKvii
#define GX_LoadBG1Scr _Z13GX_LoadBG1ScrPKvii
#define GXS_LoadBG0Scr _Z14GXS_LoadBG0ScrPKvii
#define GX_LoadBG0Scr _Z13GX_LoadBG0ScrPKvii
#define GXS_LoadOBJ _Z11GXS_LoadOBJPKvjj
#define GX_LoadOBJ _Z10GX_LoadOBJPKvjj
#define GXS_LoadOAM _Z11GXS_LoadOAMPKvjj
#define GX_LoadOAM _Z10GX_LoadOAMPKvjj
#define GXS_LoadOBJPltt _Z15GXS_LoadOBJPlttPKvjj
#define GX_LoadOBJPltt _Z14GX_LoadOBJPlttPKvjj
#define GXS_LoadBGPltt _Z14GXS_LoadBGPlttPKvjj
#define GX_LoadBGPltt _Z13GX_LoadBGPlttPKvjj
#endif

/* NitroSDK's VRAM loads (gx_load.h). The 2D loads are defined in
 * src/engine/gx/GXS.cpp (pret pokediamond arm9/lib/NitroSDK/src/GX_load2d.c),
 * the texture and texture-palette loads in src/engine/gx/GX.cpp
 * (GX_load3d.c). Each copies size bytes from src to offset bytes into its
 * target. Only the functions this game links are declared, less
 * GX_LoadBG3Scr: its callers declare the source non-const, and a second
 * spelling here would not compile beside theirs. Parameters stay unnamed: a name in a shared header takes an
 * @NNN in every includer. */


void GX_LoadBGPltt(const void *, unsigned int, unsigned int);
void GXS_LoadBGPltt(const void *, unsigned int, unsigned int);
void GX_LoadOBJPltt(const void *, unsigned int, unsigned int);
void GXS_LoadOBJPltt(const void *, unsigned int, unsigned int);
void GX_LoadOBJ(const void *, unsigned int, unsigned int);
void GXS_LoadOBJ(const void *, unsigned int, unsigned int);
void GX_LoadOAM(const void *, unsigned int, unsigned int);
void GXS_LoadOAM(const void *, unsigned int, unsigned int);
void GX_LoadBG0Scr(const void *, int, int);
void GXS_LoadBG0Scr(const void *, int, int);
void GX_LoadBG1Scr(const void *, int, int);
void GXS_LoadBG1Scr(const void *, int, int);
void GX_LoadBG2Scr(const void *, unsigned int, unsigned int);
void GXS_LoadBG2Scr(const void *, unsigned int, unsigned int);
void GXS_LoadBG3Scr(const void *, unsigned int, unsigned int);
void GX_LoadBG0Char(const void *, unsigned int, unsigned int);
void GXS_LoadBG0Char(const void *, unsigned int, unsigned int);
void GX_LoadBG1Char(const void *, unsigned int, unsigned int);
void GXS_LoadBG1Char(const void *, unsigned int, unsigned int);
void GX_LoadBG2Char(const void *, unsigned int, unsigned int);
void GXS_LoadBG2Char(const void *, unsigned int, unsigned int);
void GX_LoadBG3Char(const void *, unsigned int, unsigned int);
void GXS_LoadBG3Char(const void *, unsigned int, unsigned int);

void GXS_BeginLoadBGExtPltt(void);
void GXS_LoadBGExtPltt(const void *, unsigned int, unsigned int);
void GXS_EndLoadBGExtPltt(void);
void GXS_BeginLoadOBJExtPltt(void);
void GXS_LoadOBJExtPltt(const void *, unsigned int, unsigned int);
void GXS_EndLoadOBJExtPltt(void);

void GX_BeginLoadTex(void);
void GX_LoadTex(const void *, unsigned int, unsigned int);
void GX_EndLoadTex(void);
void GX_BeginLoadTexPltt(void);
void GX_LoadTexPltt(const void *, unsigned int, unsigned int);
void GX_EndLoadTexPltt(void);


#endif
