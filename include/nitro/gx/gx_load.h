#ifndef NITRO_GX_GX_LOAD_H
#define NITRO_GX_GX_LOAD_H

/* NitroSDK's VRAM loads (gx_load.h). The 2D loads are defined in
 * src/engine/gx/GXS.cpp (pret pokediamond arm9/lib/NitroSDK/src/GX_load2d.c),
 * the texture and texture-palette loads in src/engine/gx/GX.cpp
 * (GX_load3d.c). Each copies size bytes from src to offset bytes into its
 * target. Only the functions this game links are declared, less
 * GX_LoadBG3Scr: its callers declare the source non-const, and a second
 * spelling here would not compile beside theirs. Parameters stay unnamed: a name in a shared header takes an
 * @NNN in every includer. */

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif
