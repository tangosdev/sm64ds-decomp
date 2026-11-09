// MSVC-method bridges for gate 4b (the cross-linkage seam, gate-3a style).
//
// The SharedFilePtr and ModelComponents methods whose real
// definitions are C-named .c files. Each bridge here exists because the two
// spellings mangle differently on MSVC; none of them add behavior.
typedef unsigned int u32;
typedef unsigned short u16;
#include "SharedFilePtr.h"
#include "dExtModel_c.h"

// Three older callers still declare LoadFile as void. MSVC encodes return
// types, unlike Itanium here; the call ABI is otherwise identical and all three
// callers discard the real pointer result.
#pragma comment(linker, "/alternatename:?LoadFile@SharedFilePtr@@QAEXXZ=?LoadFile@SharedFilePtr@@QAEPAXXZ")
#pragma comment(linker, "/alternatename:?data_020a60b0@@3IA=_data_020a60b0")

// cstd::abs's C-spelling forwarder moved to hal/heap_globals.cpp, next to the
// other Itanium-spelling bridges; every target that links this file links that
// one, and the allocator layer needs the same symbol.

// Shrinks the file image to its post-parse size on the DS (a heap-space
// optimization). Skipped on host: the image simply stays at load size.
void SharedFilePtr::ReallocateModelFile() {}

// Historical C-name callers -> methods that are now real C++ definitions.
extern "C" {
void _ZN15ModelComponents21UpdateVertsUsingBonesEv(ModelComponents *self)
{ self->ModelComponents::UpdateVertsUsingBones(); }
void _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(
    ModelComponents *self, BCA_File *file, int frame)
{ self->ModelComponents::UpdateBones(file, frame); }
}


// The compressed-texture loader keeps its C-named terminal-floor definition.
// That draft is typed void (the ARM contract returned the block offset in a
// register the C shape never names), so the bridge supplies the return the
// caller depends on: the PRE-bump block cursor is where this texture landed.
extern "C" void _ZN17dExtSimpleModel_c27LoadCompressedTextureToVramEPcjS0_(char *, u32, char *);
extern "C" u32 data_020a4bc8;
struct dExtSimpleModel_c {
    static u32 LoadCompressedTextureToVram(char *src, u32 size, char *idx);
};
u32 dExtSimpleModel_c::LoadCompressedTextureToVram(char *src, u32 size, char *idx)
{
    const u32 offset = data_020a4bc8;
    _ZN17dExtSimpleModel_c27LoadCompressedTextureToVramEPcjS0_(src, size, idx);
    return offset;
}
