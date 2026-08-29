/* HOST COPY of src/func_ov002_020f6618.cpp -- the cutscene cast's LOAD SPINE,
 * with a per-object trace of the three LoadFile paths.
 *
 * WHY IT EXISTS. The render floor is an UNREBASED BMD file offset: the fault
 * address 0x00000009 is ODD, and the only odd-addressable read in
 * src/func_0204488c.c is the byte read `*cmd`, so part->cmd == 9 -- a small
 * integer where a pointer belongs. Model::LoadFile only rebases for the FIRST
 * holder:
 *     ptr.LoadFile();
 *     if (ptr.numRefs == 1 && filePtr != 0) { UpdateFileOffsets(...); ... }
 * and the watchpoint measured numRefs 2, so that branch never ran.
 *
 * On the ROM that guard is CORRECT -- the first holder rebases and everyone
 * after shares the rebased buffer. A second holder is only a bug if the FIRST
 * holder took its reference WITHOUT rebasing. This function takes references
 * through THREE different classes in one call, and the cast is eight objects
 * that plainly share assets (three pipes, several Lakitu-family files), so the
 * first holder of any given file may well be an EARLIER cast member arriving
 * through a DIFFERENT class's LoadFile:
 *
 *     Model::LoadFile            rebases when numRefs hits 1
 *     Animation::LoadFile        its own rules
 *     TextureSequence::LoadFile  its own rules
 *
 * The trace records, per object and per handle: which class touched it, the
 * handle's address, its fileID, and numRefs BEFORE and AFTER. That names the
 * first holder and says whether its path's rebase gate ever ran -- which is the
 * attribution the fix depends on. Inert unless SM64DS_INTRO_WATCH is set.
 *
 * The body is the matched source line for line otherwise. Its src line is
 * commented out in port/slice_intro.txt.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {
struct SharedFilePtr { int a, file; };
void* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr& f);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr& f);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* f, int a, int fx, unsigned int e);
void* _Znwj(unsigned int sz);
void* _ZN15TextureSequenceC1Ev(void* self);
void* _ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr& f);
void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void* bmd, void* btp);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* self, void* btp, int a, int fx, unsigned int e);
}

/* the real record shape, for reading only */
struct SfpView { unsigned short fileID; unsigned char numRefs; unsigned char pad;
                 void *filePtr; };

static int watch_on(void)
{
    static int on = -1;
    if (on < 0)
        on = std::getenv("SM64DS_INTRO_WATCH") ? 1 : 0;
    return on;
}

static unsigned g_obj;   /* which cast member this call is for */

static void trace(const char *via, const void *self, const void *fp,
                  unsigned before_refs, unsigned before_id, const void *before_ptr)
{
    if (!watch_on())
        return;
    const SfpView *v = (const SfpView *)fp;
    std::fprintf(stderr,
                 "  [load] obj#%u %-18s handle %p id %-5u refs %u->%u  ptr %p->%p%s\n",
                 g_obj, via, fp, before_id, before_refs, (unsigned)v->numRefs,
                 before_ptr, v->filePtr,
                 before_refs == 0 ? "   <- FIRST HOLDER" : "");
}

extern "C" int func_ov002_020f6618(char* self, SharedFilePtr* mdl, int nAnims,
                                   SharedFilePtr** anims, int arg5,
                                   unsigned char texByte, SharedFilePtr** texs,
                                   int tsData)
{
    int i;
    void* ts;

    ++g_obj;
    if (watch_on())
        std::fprintf(stderr, "  [load] ---- object #%u: model %p, %d anim(s), "
                     "%u tex\n", g_obj, (void *)mdl, nAnims, (unsigned)texByte);

    *(SharedFilePtr**)(self + 0x70) = mdl;
    {
        SharedFilePtr *fp = *(SharedFilePtr**)(self + 0x70);
        const SfpView *v = (const SfpView *)fp;
        const unsigned r = v->numRefs, id = v->fileID;
        void *const p = v->filePtr;
        void *bmd = _ZN5Model8LoadFileER13SharedFilePtr(*fp);
        trace("Model::LoadFile", self, fp, r, id, p);
        /* THE RETURN-VALUE PROBE. ModelBase::SetFile is declared and DEFINED
           void in src, but this caller tests its result. On ARM that works by
           accident -- SetFile tail-calls DoSetFile and DoSetFile's r0 is still
           live -- and on x86 the value is whatever the last call left in eax.
           An early bail here leaves the object HALF-BUILT: no crash now, an
           unpopulated ModelComponents later, and a renderer walking garbage.
           `data` is Model+0x08 (include/Model.h:44), so info is *(void**)(self+8)
           and the Entry table func_0204488c walks is *(void**)(info + 0x10). */
        const int sf = _ZN9ModelBase7SetFileEP8BMD_Fileii(self, bmd, 1, tsData);
        if (watch_on()) {
            void *info = *(void **)(self + 8);
            void *entries = info ? *(void **)((char *)info + 0x10) : 0;
            std::fprintf(stderr,
                         "  [load] obj#%u SetFile returned %d | info %p | "
                         "entries(info+0x10) %p%s%s\n",
                         g_obj, sf, info, entries,
                         sf == 0 ? "   <- BAILS, object left HALF-BUILT" : "",
                         (sf != 0 && !entries) ? "   <- VIEW NOT POPULATED" : "");
        }
        if (sf == 0)
            return 0;
    }

    self[0x80] = (char)nAnims;
    *(SharedFilePtr***)(self + 0x74) = anims;
    for (i = 0; i < *(unsigned char*)(self + 0x80); i++) {
        SharedFilePtr *fp = (*(SharedFilePtr***)(self + 0x74))[i];
        const SfpView *v = (const SfpView *)fp;
        const unsigned r = v->numRefs, id = v->fileID;
        void *const p = v->filePtr;
        _ZN9Animation8LoadFileER13SharedFilePtr(*fp);
        trace("Animation::LoadFile", self, fp, r, id, p);
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self,
        (void*)(*(*(SharedFilePtr***)(self + 0x74))[0]).file, 0, 0x1000, 0);

    if (texByte) {
        ts = _Znwj(0x14);
        if (ts) ts = _ZN15TextureSequenceC1Ev(ts);
        *(void**)(self + 0x7c) = ts;
        if (*(void**)(self + 0x7c) == 0)
            return 0;
        self[0x81] = texByte;
        *(SharedFilePtr***)(self + 0x78) = texs;
        for (i = 0; i < *(unsigned char*)(self + 0x81); i++) {
            void* bmd = (void*)(*(SharedFilePtr**)(self + 0x70))->file;
            SharedFilePtr *fp = (*(SharedFilePtr***)(self + 0x78))[i];
            const SfpView *v = (const SfpView *)fp;
            const unsigned r = v->numRefs, id = v->fileID;
            void *const p = v->filePtr;
            void* btp = _ZN15TextureSequence8LoadFileER13SharedFilePtr(*fp);
            trace("TexSeq::LoadFile", self, fp, r, id, p);
            _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(bmd, btp);
        }
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(*(void**)(self + 0x7c),
            (void*)(*(*(SharedFilePtr***)(self + 0x78))[0]).file, 0, 0x1000, 0);
    }
    self[0x83] = (char)arg5;
    return 1;
}
