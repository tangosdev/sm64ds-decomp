// MSVC-method bridges for gate 4b (the cross-linkage seam, gate-3a style).
//
// The Model TUs declare GX's upload entry points as STATIC MEMBERS of a
// struct GX (mangling ?LoadTex@GX@@SAX...), while the definitions are either
// namespace-style C++ (GX::LoadTex from src/) or C-named HAL stubs. Same
// story for the SharedFilePtr and ModelComponents methods whose real
// definitions are C-named .c files. Each bridge here exists because the two
// spellings mangle differently on MSVC; none of them add behavior.
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
void GX::LoadTexPltt(const void *s, u32 a, u32 z) { _ZN2GX11LoadTexPlttEPKvjj(s, a, z); }

struct SharedFilePtr {
    void LoadFile();
    void ReallocateModelFile();
};
void SharedFilePtr::LoadFile() { _ZN13SharedFilePtr8LoadFileEv(this); }
// Shrinks the file image to its post-parse size on the DS (a heap-space
// optimization). Skipped on host: the image simply stays at load size.
void SharedFilePtr::ReallocateModelFile() {}

struct ModelComponents {
    void UpdateVertsUsingBones();
};
void ModelComponents::UpdateVertsUsingBones()
{
    _ZN15ModelComponents21UpdateVertsUsingBonesEv(this);
}

// The compressed-texture loader keeps its C-named terminal-floor definition.
// That draft is typed void (the ARM contract returned the block offset in a
// register the C shape never names), so the bridge supplies the return the
// caller depends on: the PRE-bump block cursor is where this texture landed.
extern "C" void _ZN5Model27LoadCompressedTextureToVramEPcjS0_(char *, u32, char *);
extern "C" u32 data_020a4bc8;
struct Model {
    static u32 LoadCompressedTextureToVram(char *src, u32 size, char *idx);
};
u32 Model::LoadCompressedTextureToVram(char *src, u32 size, char *idx)
{
    const u32 offset = data_020a4bc8;
    _ZN5Model27LoadCompressedTextureToVramEPcjS0_(src, size, idx);
    return offset;
}
