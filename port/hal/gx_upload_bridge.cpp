// MSVC-method bridges for gate 4b (the cross-linkage seam, gate-3a style).
//
// The Model TUs declare GX's upload entry points as STATIC MEMBERS of a
// struct GX (mangling ?LoadTex@GX@@SAX...), while the definitions are either
// namespace-style C++ (GX::LoadTex from src/) or C-named HAL stubs. Same
// story for the SharedFilePtr and ModelComponents methods whose real
// definitions are C-named .c files. Each bridge here exists because the two
// spellings mangle differently on MSVC; none of them add behavior.
#include <cstdio>

typedef unsigned int u32;
typedef unsigned short u16;

extern "C" {
void _ZN2GX12BeginLoadTexEv(void);
void _ZN2GX10EndLoadTexEv(void);
void _ZN2GX16BeginLoadTexPlttEv(void);
void _ZN2GX14EndLoadTexPlttEv(void);
void _ZN2GX7LoadTexEPKvjj(const void *, u32, u32);
void _ZN2GX11LoadTexPlttEPKvjj(const void *, u32, u32);
void _ZN13SharedFilePtr8LoadFileEv(void *);
void _ZN13SharedFilePtr7ReleaseEv(void *);
void _ZN15ModelComponents21UpdateVertsUsingBonesEv(void *);
}

struct GX {
    static void BeginLoadTex();
    static void EndLoadTex();
    static void BeginLoadTexPltt();
    static void EndLoadTexPltt();
    static void LoadTex(const void *, u32, u32);
    static void LoadTexPltt(const void *, u32, u32);
};
void GX::BeginLoadTex() { _ZN2GX12BeginLoadTexEv(); }
void GX::EndLoadTex() { _ZN2GX10EndLoadTexEv(); }
void GX::BeginLoadTexPltt() { _ZN2GX16BeginLoadTexPlttEv(); }
void GX::EndLoadTexPltt() { _ZN2GX14EndLoadTexPlttEv(); }
void GX::LoadTex(const void *s, u32 o, u32 z) { _ZN2GX7LoadTexEPKvjj(s, o, z); }
/* SM64DS_TEX_LOG=1: the palette half of the upload ledger (hal_tex_log
   and the texel half live in hal/model_host.cpp). */
extern "C" int hal_tex_log(void);
void GX::LoadTexPltt(const void *s, u32 a, u32 z)
{
    if (hal_tex_log())
        printf("[palup]  off=%05x size=%05x -> pltt %04x\n", a, z, a >> 4);
    _ZN2GX11LoadTexPlttEPKvjj(s, a, z);
}

struct SharedFilePtr {
    void LoadFile();
    void ReallocateModelFile();
    void Release();
};
void SharedFilePtr::LoadFile() { _ZN13SharedFilePtr8LoadFileEv(this); }
/* Release is a C-form definition in src (a .c file), but the cleanup paths
   main rewrote as real methods -- ArrowSignRight's, the water's, the net's --
   reach it through include/SharedFilePtr.h as a method. Same direction as
   LoadFile above. */
/* RETIRED at ALIAS2 (wave 8, the main -> port sync): main defines
   SharedFilePtr::Release as a real member now, so src/ emits
   ?Release@SharedFilePtr@@QAEXXZ itself and this face was the second
   definition (LNK2005).
void SharedFilePtr::Release() { _ZN13SharedFilePtr7ReleaseEv(this); }        */
// Shrinks the file image to its post-parse size on the DS (a heap-space
// optimization). Skipped on host: the image simply stays at load size.
//
// RETIRED IN THE THREE TARGETS THAT CARRY THE ROM'S OWN BODY (run link100,
// lane SHADOWS2). This empty method was the SHADOW of
// src/_ZN13SharedFilePtr19ReallocateModelFileEv.cpp -- a whole matched TU kept
// out of the link by a host stand-in, while the ROM's one-statement body
// (`return func_02017060(file)`) and its callee were both available. Where the
// ROM body IS in the link, hal/method_faces.cpp aliases the void-returning
// spelling every caller uses onto it, and this definition must NOT be there to
// defeat the alias (alternatename_guard.py refuses a defined LHS).
//
// It stays for every OTHER target, and that is not a courtesy: the ROM body's
// callee func_02017060 rides port/slice_gate16.txt, which is on smoke_player,
// walk_window and walk_window_hires only. The ten narrow harnesses link the
// CALLER (src/_ZN5Model8LoadFileER13SharedFilePtr.cpp, slice_gate4b) without
// that callee, so for them the empty body is still the only definition there
// can be. Same per-target shape as SM64DS_STAGE_SLOT6_ROM in CMakeLists.txt.
#ifndef SM64DS_SFP_REALLOC_ROM
void SharedFilePtr::ReallocateModelFile() {}
#endif

struct BCA_File;
struct ModelComponents {
    void UpdateVertsUsingBones();
    void UpdateBones(BCA_File *file, int frame);
};
/* RETIRED at ALIAS2 (wave 8, the main -> port sync): main defines both of
   these as real ModelComponents members now, so src/ emits
   ?UpdateVertsUsingBones@ModelComponents@@QAEXXZ and
   ?UpdateBones@ModelComponents@@QAEXPAUBCA_File@@H@Z itself and these faces
   were the second definition (LNK2005).
void ModelComponents::UpdateVertsUsingBones()
{
    _ZN15ModelComponents21UpdateVertsUsingBonesEv(this);
}
extern "C" void _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(void *, void *, int);
void ModelComponents::UpdateBones(BCA_File *file, int frame)
{
    _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(this, file, frame);
}                                                                            */


// The compressed-texture loader keeps its C-named terminal-floor definition.
// That draft is typed void (the ARM contract returned the block offset in a
// register the C shape never names), so the bridge supplies the return the
// caller depends on: the PRE-bump block cursor is where this texture landed.
// RETIRED at SYNC6. main's #2528 matched this function and gave
// include/Model.h the u32 return the bridge existed to supply ("the
// declaration here had it right all along", Model.h's own note), so
// src/_ZN5Model27LoadCompressedTextureToVramEPcjS0_.cpp now defines
// ?LoadCompressedTextureToVram@Model@@SAIPADI0@Z itself and this was the
// second definition (one LNK2005 row in build_s2.log).
//
// What the src TU does NOT define any more is the flat ROM name: it spells the
// body as the static member, so __ZN5Model27LoadCompressedTextureToVramEPcjS0_
// went unresolved for src/func_ov075_0211aa94.c and src/func_ov080_02125630.cpp,
// which still call it flat. That one is an /alternatename in
// hal/cxx_aliases.cpp rather than a face: a STATIC member is __cdecl with no
// receiver, three arguments and a scalar return on both sides, which is the
// admissibility rule exactly.
// extern "C" void _ZN5Model27LoadCompressedTextureToVramEPcjS0_(char *, u32, char *);
// extern "C" u32 data_020a4bc8;
// struct Model {
//     static u32 LoadCompressedTextureToVram(char *src, u32 size, char *idx);
// };
// u32 Model::LoadCompressedTextureToVram(char *src, u32 size, char *idx)
// {
//     const u32 offset = data_020a4bc8;
//     _ZN5Model27LoadCompressedTextureToVramEPcjS0_(src, size, idx);
//     return offset;
// }
