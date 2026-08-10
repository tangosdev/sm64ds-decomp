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
/* SM64DS_TEX_LOG=1: the palette half of the upload ledger (hal_tex_log
   and the texel half live in hal/model_host.cpp). */
extern "C" int hal_tex_log(void);
#ifdef _WIN32
/* MSVC: `GX::LoadTex` etc. mangle differently from the extern-C _ZN2GX... names
   the real host uploaders (hal/model_host.cpp) define, so these method-shaped
   bridges forward the one to the other and add nothing. */
void GX::BeginLoadTex() { _ZN2GX12BeginLoadTexEv(); }
void GX::EndLoadTex() { _ZN2GX10EndLoadTexEv(); }
void GX::BeginLoadTexPltt() { _ZN2GX16BeginLoadTexPlttEv(); }
void GX::EndLoadTexPltt() { _ZN2GX14EndLoadTexPlttEv(); }
void GX::LoadTex(const void *s, u32 o, u32 z) { _ZN2GX7LoadTexEPKvjj(s, o, z); }
void GX::LoadTexPltt(const void *s, u32 a, u32 z)
{
    if (hal_tex_log())
        printf("[palup]  off=%05x size=%05x -> pltt %04x\n", a, z, a >> 4);
    _ZN2GX11LoadTexPlttEPKvjj(s, a, z);
}
#endif /* _WIN32 -- on GCC each `GX::X` IS the _ZN2GX...X symbol it would forward
   to, so defining these here calls them into themselves. On Linux the C callers
   and the src `GX::` calls both resolve straight to the real host uploaders in
   hal/model_host.cpp, which own those exact symbols. */

struct SharedFilePtr {
    void LoadFile();
    void ReallocateModelFile();
    void Release();
};
/* LINUX CRITICAL: `SharedFilePtr::LoadFile()` mangles to
   _ZN13SharedFilePtr8LoadFileEv -- the very extern-C name this body calls -- so
   on GCC the two are ONE symbol and the bridge self-recurses (a stack-overflow
   SIGSEGV the moment the first asset loads, right after Stage::Stage). Same for
   Release. On MSVC the C++ method and the extern-C name mangle differently so
   the bridge is a real converter. On Linux the real src/ definitions
   (src/_ZN13SharedFilePtr8LoadFileEv.c and the Release .c) already carry these
   exact names with the correct ABI, so omit the host copies. */
#ifdef _WIN32
void SharedFilePtr::LoadFile() { _ZN13SharedFilePtr8LoadFileEv(this); }
/* Release is a C-form definition in src (a .c file), but the cleanup paths
   main rewrote as real methods -- ArrowSignRight's, the water's, the net's --
   reach it through include/SharedFilePtr.h as a method. Same direction as
   LoadFile above. */
void SharedFilePtr::Release() { _ZN13SharedFilePtr7ReleaseEv(this); }
#endif /* _WIN32 -- Linux binds callers straight to the real src/ methods */
// Shrinks the file image to its post-parse size on the DS (a heap-space
// optimization). Skipped on host: the image simply stays at load size.
void SharedFilePtr::ReallocateModelFile() {}

struct BCA_File;
struct ModelComponents {
    void UpdateVertsUsingBones();
    void UpdateBones(BCA_File *file, int frame);
};
extern "C" void _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(void *, void *, int);
#ifdef _WIN32 /* LINUX: each `ModelComponents::X` IS the _ZN15ModelComponents...X symbol it forwards to (this=self, args match) -> self-recurse on GCC. On Linux bind to the real src/ TUs. */
void ModelComponents::UpdateVertsUsingBones()
{
    _ZN15ModelComponents21UpdateVertsUsingBonesEv(this);
}
void ModelComponents::UpdateBones(BCA_File *file, int frame)
{
    _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(this, file, frame);
}
#endif /* _WIN32 */


// The compressed-texture loader keeps its C-named terminal-floor definition.
// That draft is typed void (the ARM contract returned the block offset in a
// register the C shape never names), so the bridge supplies the return the
// caller depends on: the PRE-bump block cursor is where this texture landed.
extern "C" u32 data_020a4bc8;
struct Model {
    static u32 LoadCompressedTextureToVram(char *src, u32 size, char *idx);
};
#ifdef _WIN32
/* MSVC: the C++ method and the extern-C terminal-floor name mangle differently,
   so this wrapper is a real forwarder that supplies the return the ARM contract
   passed in r0 (the pre-bump block cursor) which the void-typed C floor drops. */
extern "C" void _ZN5Model27LoadCompressedTextureToVramEPcjS0_(char *, u32, char *);
u32 Model::LoadCompressedTextureToVram(char *src, u32 size, char *idx)
{
    const u32 offset = data_020a4bc8;
    _ZN5Model27LoadCompressedTextureToVramEPcjS0_(src, size, idx);
    return offset;
}
#else
/* LINUX: `Model::LoadCompressedTextureToVram(char*,u32,char*)` mangles to exactly
   _ZN5Model27LoadCompressedTextureToVramEPcjS0_ -- the SAME symbol the src
   terminal-floor body defines. Defining the wrapper here as that method makes its
   own body call itself -> infinite recursion. So on Linux the method IS the
   terminal-floor body: it owns the symbol and returns the pre-bump cursor the
   caller reads. Capturing data_020a4bc8 before the load and restoring the ROM's
   r0 semantics keeps LoadTexAndPal's `ret` correct without the redundant symbol. */
extern "C" void _ZN2GX12BeginLoadTexEv();
extern "C" void _ZN2GX7LoadTexEPKvjj(const void *, u32, u32);
extern "C" void _ZN2GX10EndLoadTexEv();
extern "C" u32 data_020a4be8, data_020a4be4, data_020a4be0;
extern "C" void Crash();
u32 Model::LoadCompressedTextureToVram(char *src, u32 size, char *idx)
{
    const u32 offset = data_020a4bc8;      /* pre-bump block cursor = the r0 return */
    if ((data_020a4be8 - data_020a4bc8) < size) Crash();
    _ZN2GX12BeginLoadTexEv();
    _ZN2GX7LoadTexEPKvjj(src, data_020a4bc8, size);
    data_020a4be4 += size;
    _ZN2GX7LoadTexEPKvjj(idx, data_020a4be0, size >> 1);
    data_020a4be4 += size >> 1;
    _ZN2GX10EndLoadTexEv();
    data_020a4bc8 += size;
    data_020a4be0 += size >> 1;
    return offset;
}
#endif
