//cpp
/* arm9/SharedFilePtr -- the shared file handle, its card loader, and the
 * per-format LoadFile wrappers built on it. .text 0x0201787c..0x02017e60.
 *
 * A SharedFilePtr is an eight-byte handle to one file that several actors
 * share: the first LoadFile reads the file in, later ones only count
 * references, and the last Release frees it. Each format's LoadFile
 * (dExtSimpleModel_c, dExtAnmTexPat_c, dExtFrameCtrl_c, dBgW_Kc) loads through the handle
 * and rebases the file's internal offsets the first time it arrives. Those
 * four are members of other classes, but the cartridge emits them in this
 * run, so they live here.
 *
 * The constructors and destructors are the handle kinds' own (coined class
 * names, see include/SharedFilePtr.h). Each derived one is the shape mwcc
 * gives an empty body over a base: push {r4, lr} / mov r4, r0 / bl <base> /
 * mov r0, r4 -- the constructor returns `this` and enters the base's C2, the
 * destructor enters its D2. The file ID rides in r1 untouched down to
 * func_02017e0c. mwcc emits all four variants of each; the ones the cartridge
 * does not carry are deadstripped compiler output in the manifest.
 *
 * Layout: +0x0 u16 fileID, +0x2 u8 numRefs, +0x4 void *filePtr, as
 * include/SharedFilePtr.h declares it. Every member below reads it.
 *
 * mwccarm's default deferred codegen emits .text in reverse source order, so
 * the definitions run from the highest address down. Do not reorder.
 */

#include "types.h"
#include "SharedFilePtr.h"
#include "dExtSimpleModel_c.h"
#include "dExtAnmTexPat_c.h"
#include "dBgW_Kc.h"

namespace Memory {
void *Allocate(u32 size);
void *Allocate(u32 size, int align);
void Deallocate(void *ptr);
} // namespace Memory

#include "nitro/os/cache.h"
#include "nitro/mi/memory.h"
#include "nitro/os/system.h"
#include "nitro/fs/file.h"

extern "C" {
/* The last file ID any handle touched: every entry point stores it first,
   so a crash mid-load names the file. */
extern unsigned int gLastUsedFileID;

unsigned int func_02018a24(unsigned int);
int func_020186c0(unsigned int val);
int func_02018568(int a);
unsigned int func_02018ac4(unsigned int *p);
void DecompressLZ16(void *src, void *dst);
int func_020185c0(void *buf, unsigned int x);
void func_02018770(void);
int myFS_ReadFile(void *a, void *b, int c);
}

unsigned int TrimModelFile(void *file);

SharedFilePtr::SharedFilePtr(unsigned int id) {
    func_02017e0c(id);
}

/* Publishes the file ID, like every entry point; freeing is Release's job. */
SharedFilePtr::~SharedFilePtr() {
    gLastUsedFileID = fileID;
}

/* Stamps a fresh handle: no file, no holders, and the FAT file ID that
   func_02018a24 resolves from the overlay-0 file ID. */
void SharedFilePtr::func_02017e0c(unsigned int id) {
    filePtr = 0;
    numRefs = 0;
    fileID = func_02018a24(id);
}

/* Resolve fileID to its bytes, either already resident in the overlay-file
   table or streamed from the card, decompress them if they are LZ-tagged,
   and stash and return the buffer. */
void *SharedFilePtr::Load() {
    gLastUsedFileID = fileID;
    int p = func_020186c0(fileID);
    if (p != 0) {
        unsigned int size;
        void *mem;
        if (func_02018568(p)) {
            unsigned int *q = (unsigned int *)(p + 4);
            size = func_02018ac4(q);
            mem = Memory::Allocate(size);
            DecompressLZ16(q, mem);
        } else {
            FSFile obj;
            func_020185c0(&obj, fileID);
            size = FS_GetLength(&obj);
            FS_CloseFile(&obj);
            mem = Memory::Allocate(size, 0x20);
            MI_CpuCopy8((void *)p, mem, size);
        }
        func_02018770();
        DC_StoreRange(mem, size);
        filePtr = mem;
        return filePtr;
    }

    FSFile obj2;
    void *cp;
    unsigned int csize;
    int raw;
    int fsize;

    func_020185c0(&obj2, fileID);
    fsize = FS_GetLength(&obj2);
    if (fsize <= 8)
        OS_Terminate();
    raw = (int)Memory::Allocate(fsize, 0x20);
    DC_FlushRange((void *)raw, fsize);
    int r = myFS_ReadFile(&obj2, (void *)raw, fsize);
    if (r != fsize)
        return 0;
    FS_CloseFile(&obj2);
    func_02018770();
    if (func_02018568(raw) != 0) {
        unsigned int t;
        cp = (void *)(raw + 4);
        t = func_02018ac4((unsigned int *)cp);
        csize = fsize - 4;
        fsize = t;
        void *compressed = Memory::Allocate(csize);
        MI_CpuCopy8(cp, compressed, csize);
        Memory::Deallocate((void *)raw);
        raw = (int)Memory::Allocate(fsize);
        DecompressLZ16(compressed, (void *)raw);
        Memory::Deallocate(compressed);
    }
    DC_StoreRange((void *)raw, fsize);
    filePtr = (void *)raw;
    return filePtr;
}

/* Frees the file once the last holder lets go. */
void SharedFilePtr::func_02017c24() {
    gLastUsedFileID = fileID;
    Memory::Deallocate(filePtr);
    filePtr = 0;
}

/* Takes one reference and hands back the file. The first taker loads it;
   at 255 holders the count would wrap, so the call is refused instead. */
void *SharedFilePtr::LoadFile() {
    gLastUsedFileID = fileID;

    if (numRefs == 0) {
        if (!Load())
            return 0;
    }

    if (numRefs >= 255) {
        return 0;
    }

    numRefs += 1;
    return filePtr;
}

/* Hands one reference back and frees the file when it was the last. */
void SharedFilePtr::Release() {
    gLastUsedFileID = fileID;

    if (numRefs == 0)
        return;

    numRefs -= 1;

    if (numRefs != 0)
        return;

    func_02017c24();
}

ClsnFile::ClsnFile(unsigned int id) : SharedFilePtr(id) {}

ClsnFile::~ClsnFile() {}

/* Loads a KCL collision file; the first holder rebases its offsets. */
KCL_File *dBgW_Kc::LoadFile(SharedFilePtr &ptr) {
    u8 refs;
    KCL_File *file;

    ptr.LoadFile();
    refs = ptr.numRefs;
    file = (KCL_File *)ptr.filePtr;
    if (refs == 1 && file != 0) {
        UpdateFileOffsets(*file);
    }
    return file;
}

ModelFile::ModelFile(unsigned int id) : SharedFilePtr(id) {}

ModelFile::~ModelFile() {}

/* Gives the model file back the heap space past its real size. */
void SharedFilePtr::ReallocateModelFile() {
    TrimModelFile(filePtr);
}

/* Loads a BMD model. The first holder rebases its offsets, registers it in
   the common model data array, and trims its allocation to the real size. */
BMD_File *dExtSimpleModel_c::LoadFile(SharedFilePtr &ptr) {
    BMD_File *filePtr;
    ptr.LoadFile();
    filePtr = (BMD_File *)ptr.filePtr;
    if (ptr.numRefs == 1 && filePtr != 0) {
        UpdateFileOffsets(*filePtr);
        AddToCommonModelDataArr(*filePtr);
        ptr.ReallocateModelFile();
    }
    return filePtr;
}

ModelFileA::ModelFileA(unsigned int id) : ModelFile(id) {}

ModelFileA::~ModelFileA() {}

/* Loads a BMD through the handle and gives it to a dExtModel_c. If SetFile
   refuses it, the reference is handed back. */
int SharedFilePtr::func_020179b4(dExtModel_c *model, int arg) {
    int result = 0;
    if (model) {
        BMD_File *file = dExtSimpleModel_c::LoadFile(*this);
        if (file) {
            result = model->SetFile(file, arg, -1);
            if (!result) {
                Release();
            }
        }
    }
    return result;
}

AnimFile::AnimFile(unsigned int id) : SharedFilePtr(id) {}

AnimFile::~AnimFile() {}

/* Loads a BCA animation; the first holder rebases its offsets. */
BCA_File *dExtFrameCtrl_c::LoadFile(SharedFilePtr &ptr) {
    u8 refs;
    BCA_File *file;

    ptr.LoadFile();
    refs = ptr.numRefs;
    file = (BCA_File *)ptr.filePtr;
    if (refs == 1 && file != 0) {
        UpdateFileOffsets(*file);
    }
    return file;
}

TexSeqFile::TexSeqFile(unsigned int id) : SharedFilePtr(id) {}

TexSeqFile::~TexSeqFile() {}

/* Loads a BTP texture animation; the first holder rebases its offsets. */
BTP_File *dExtAnmTexPat_c::LoadFile(SharedFilePtr &ptr) {
    BTP_File *filePtr;
    ptr.LoadFile();
    filePtr = (BTP_File *)ptr.filePtr;
    if (ptr.numRefs == 1 && filePtr != 0) {
        UpdateFileOffsets(*filePtr);
    }
    return filePtr;
}

ModelFileB::ModelFileB(unsigned int id) : ModelFile(id) {}

ModelFileB::~ModelFileB() {}

/* A BMD LoadFile that rebases the file but, unlike dExtSimpleModel_c::LoadFile, neither
   registers it in the common model data array nor trims its allocation. */
void *SharedFilePtr::func_0201787c() {
    BMD_File *loaded;
    LoadFile();
    loaded = (BMD_File *)filePtr;
    if (numRefs == 1 && loaded != 0) {
        dExtSimpleModel_c::UpdateFileOffsets(*loaded);
    }
    return loaded;
}
