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
 * The unnamed helpers keep their address names and C linkage. Each one is a
 * two-call shape that hands `self` back: push {r4, lr} / mov r4, r0 / bl ... /
 * mov r0, r4. The construct helpers chain to func_02017e48 and the destroy
 * helpers to func_02017e34. A construct helper never touches r1, so the file
 * ID its caller leaves there reaches func_02017e0c untouched. The parameter
 * is spelled out here and forwarded, which is byte-identical, because a
 * host ABI would lose the value otherwise. Their signatures are the ones the
 * retired one-function files defined, so every caller's declaration agrees
 * or disagrees exactly as before.
 *
 * Layout: +0x0 u16 fileID, +0x2 u8 numRefs, +0x4 void *filePtr, as
 * include/SharedFilePtr.h declares it. Every member below reads it.
 *
 * mwccarm's default deferred codegen emits .text in reverse source order, so
 * the definitions run from the highest address down. Do not reorder.
 *
 * HOST BUILD. port/ compiles this file for gate 3b and every target above it.
 * The host gets the refcounting core (func_02017e0c, func_02017c24, LoadFile,
 * Release). dExtSimpleModel_c::LoadFile and dExtFrameCtrl_c::LoadFile are compiled only
 * where the gate-4b slice supplies their UpdateFileOffsets, and
 * dBgW_Kc::LoadFile only where gate 8 does: port/CMakeLists.txt defines
 * SM64DS_PORT_GATE4B and SM64DS_PORT_GATE8 on those targets. Everything else
 * is DS-only. port/hal/fs.cpp supplies Load (the card seam) and an
 * ABI-portable Construct, and port/hal/gx_upload_bridge.cpp supplies
 * ReallocateModelFile.
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
#ifndef _MSC_VER
#include "nitro/fs/file.h"
#endif

extern "C" {
/* The last file ID any handle touched: every entry point stores it first,
   so a crash mid-load names the file. */
extern unsigned int data_0209d3bc;

unsigned int func_02018a24(unsigned int);
int func_020186c0(unsigned int val);
int func_02018568(int a);
unsigned int func_02018ac4(unsigned int *p);
void DecompressLZ16(void *src, void *dst);
int func_020185c0(void *buf, unsigned int x);
void func_02018770(void);
int myFS_ReadFile(void *a, void *b, int c);
}

/* ROM symbol is _Z13func_02017e0cPvj. The host call is C: port/hal/fs.cpp
   declares it inside its extern "C" seam. */
#ifdef _MSC_VER
extern "C"
#endif
    void
    func_02017e0c(void *self, unsigned int fileID);
unsigned int TrimModelFile(void *file);

#ifndef _MSC_VER
void *func_02017e48(void *self, unsigned int fileID) {
    func_02017e0c(self, fileID);
    return self;
}

void *func_02017e34(void *self) {
    data_0209d3bc = static_cast<SharedFilePtr *>(self)->fileID;
    return self;
}
#endif

/* Stamps a fresh handle: no file, no holders, and the FAT file ID that
   func_02018a24 resolves from the overlay-0 file ID. */
void func_02017e0c(void *self, unsigned int fileID) {
    SharedFilePtr *handle = static_cast<SharedFilePtr *>(self);
    handle->filePtr = 0;
    handle->numRefs = 0;
    handle->fileID = func_02018a24(fileID);
}

#ifndef _MSC_VER
/* Resolve fileID to its bytes, either already resident in the overlay-file
   table or streamed from the card, decompress them if they are LZ-tagged,
   and stash and return the buffer. */
void *SharedFilePtr::Load() {
    data_0209d3bc = fileID;
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
        void *tmp = Memory::Allocate(csize);
        MI_CpuCopy8(cp, tmp, csize);
        Memory::Deallocate((void *)raw);
        raw = (int)Memory::Allocate(fsize);
        DecompressLZ16(tmp, (void *)raw);
        Memory::Deallocate(tmp);
    }
    DC_StoreRange((void *)raw, fsize);
    filePtr = (void *)raw;
    return filePtr;
}
#endif

/* Frees the file once the last holder lets go. */
void func_02017c24(SharedFilePtr *self) {
    data_0209d3bc = self->fileID;
    Memory::Deallocate(self->filePtr);
    self->filePtr = 0;
}

/* Takes one reference and hands back the file. The first taker loads it;
   at 255 holders the count would wrap, so the call is refused instead. */
void *SharedFilePtr::LoadFile() {
    data_0209d3bc = fileID;

    if (numRefs == 0) {
        if (!Load())
            return 0;
    }

    if (numRefs >= 0xff) {
        return 0;
    }

    numRefs += 1;
    return filePtr;
}

/* Hands one reference back and frees the file when it was the last. */
void SharedFilePtr::Release() {
    data_0209d3bc = fileID;

    if (numRefs == 0)
        return;

    numRefs -= 1;

    if (numRefs != 0)
        return;

    func_02017c24(this);
}

#ifndef _MSC_VER
void *func_02017b4c(void *self, unsigned int fileID) {
    func_02017e48(self, fileID);
    return self;
}

int SharedFilePtr_Destruct_Clsn(int self) {
    func_02017e34(reinterpret_cast<void *>(self));
    return self;
}
#endif

#if !defined(_MSC_VER) || defined(SM64DS_PORT_GATE8)
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
#endif

#ifndef _MSC_VER
void *func_02017ae4(void *self, unsigned int fileID) {
    func_02017e48(self, fileID);
    return self;
}

void *func_02017acc(void *self, unsigned int fileID) {
    func_02017e48(self, fileID);
    return self;
}

int func_02017ab4(int self) {
    func_02017e34(reinterpret_cast<void *>(self));
    return self;
}

int func_02017a9c(int self) {
    func_02017e34(reinterpret_cast<void *>(self));
    return self;
}

/* Gives the model file back the heap space past its real size. */
void SharedFilePtr::ReallocateModelFile() {
    TrimModelFile(filePtr);
}
#endif

#if !defined(_MSC_VER) || defined(SM64DS_PORT_GATE4B)
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
#endif

#ifndef _MSC_VER
void *func_02017a24(void *self, unsigned int fileID) {
    func_02017ae4(self, fileID);
    return self;
}

int func_02017a0c(int self) {
    func_02017a9c(self);
    return self;
}

/* Loads a BMD through the handle and gives it to a dExtModel_c. If SetFile
   refuses it, the reference is handed back. */
int func_020179b4(SharedFilePtr *filePtr, dExtModel_c *model, int arg) {
    int result = 0;
    if (model) {
        BMD_File *file = dExtSimpleModel_c::LoadFile(*filePtr);
        if (file) {
            result = model->SetFile(file, arg, -1);
            if (!result) {
                filePtr->Release();
            }
        }
    }
    return result;
}

/* Hands the handle back so the caller can chain. */
SharedFilePtr &SharedFilePtr::Construct(unsigned int fileID) {
    func_02017e48(this, fileID);
    return *this;
}

extern "C" int SharedFilePtr_Destruct_Anim(int self) {
    func_02017e34(reinterpret_cast<void *>(self));
    return self;
}
#endif

#if !defined(_MSC_VER) || defined(SM64DS_PORT_GATE4B)
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
#endif

#ifndef _MSC_VER
extern "C" void *SharedFilePtr_Construct_TexSeq(void *self, unsigned int fileID) {
    func_02017e48(self, fileID);
    return self;
}

extern "C" int SharedFilePtr_Destruct_TexSeq(int self) {
    func_02017e34(reinterpret_cast<void *>(self));
    return self;
}

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

void *func_020178cc(void *self, unsigned int fileID) {
    func_02017ae4(self, fileID);
    return self;
}

int func_020178b4(int self) {
    func_02017a9c(self);
    return self;
}

/* A BMD LoadFile that rebases the file but, unlike dExtSimpleModel_c::LoadFile, neither
   registers it in the common model data array nor trims its allocation. */
extern "C" void *func_0201787c(SharedFilePtr *ptr) {
    BMD_File *filePtr;
    ptr->LoadFile();
    filePtr = (BMD_File *)ptr->filePtr;
    if (ptr->numRefs == 1 && filePtr != 0) {
        dExtSimpleModel_c::UpdateFileOffsets(*filePtr);
    }
    return filePtr;
}
#endif
