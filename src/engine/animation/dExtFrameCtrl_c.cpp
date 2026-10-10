//cpp
/* dExtFrameCtrl_c ("dExtFrameCtrl_c" in the tree), the animation-playback root and
 * arm9 .text 0x02015a7c..0x02015cb4: a frame cursor the model and animation
 * classes drive. Layout is 0x10 bytes -- vptr, frame count in the low 30 bits
 * of +0x4 with the loop flags in the top two, current frame at +0x8 as 20.12,
 * playback speed at +0xc -- pinned by C2 and read the same way by
 * Copy/Advance/Finished. The two-slot vtable at _ZTV15dExtFrameCtrl_c (0x0208e7e4)
 * is the destructor pair alone; notes/model-rtti-names.md recovers the
 * family from the ROM's own typeinfo records.
 *
 * The destructor/constructor ABI cluster at 0x02015cb4..0x02015d38 keeps its
 * per-function shards: ~dExtFrameCtrl_c() is the key function, so a TU that defines
 * it also emits _ZTV/_ZTI/_ZTS. Those records spell the coined "9Animation"
 * where the cartridge's records spell "15dExtFrameCtrl_c", and a manifest
 * cannot license data whose emitted name has no configured ROM home. The
 * split is the honest shape until the class is renamed.
 *
 * mwccarm emits .text in reverse source order, so the definitions below run
 * ROM-descending.
 *
 * LoadFile (0x0201794c) and UpdateFileOffsets (0x020469ac) are statics of
 * this class too, but they link in other translation units and keep their
 * own files.
 */
#include "dExtFrameCtrl_c.h"

extern "C" {
int __aeabi_idivmod(int n, int d);
/* SetAnimation's declared signature carries Fix12<int> by value and wall 6az
   homes class-typed by-value parameters, so the definition stays a mangled
   free function; scalar args keep the bytes.
   local extern: byte-required, notes/mwccarm-codegen.md 6az */
void _ZN15dExtFrameCtrl_c12SetAnimationEti5Fix12IiEt(dExtFrameCtrl_c *self, unsigned short numFrames, int flags, int speed, unsigned short startFrame);
}
// @symbol _ZN15dExtFrameCtrl_c7AdvanceEv
void dExtFrameCtrl_c::Advance()
{
    u32 f = numFramesAndFlags;
    u32 len = f & ~0xc0000000;
    if ((f & 0xc0000000) == 0) {
        currFrame = (currFrame + speed + (int)len) % (int)len;
    } else {
        /* the ROM materializes the field address for this
           read-modify-write, and reloads currFrame through the member
           afterwards; the launder keeps this store aliasing the member so
           the compiler re-reads it the way the ROM does */
        int *pFrame = (int *)((long long)((int)this + 8));
        *pFrame = *pFrame + speed;
        if (currFrame < 0) {
            currFrame = 0;
        } else if (currFrame >= (int)len) {
            currFrame = (int)len - 1;
        }
    }
}
// @symbol _ZN15dExtFrameCtrl_c12SetAnimationEti5Fix12IiEt
extern "C"
void _ZN15dExtFrameCtrl_c12SetAnimationEti5Fix12IiEt(dExtFrameCtrl_c *self, unsigned short numFrames, int flags, int speed, unsigned short startFrame) {
  *(int*)((char*)&self->numFramesAndFlags) = flags | (numFrames << 12);
  *(int*)((char*)&self->currFrame) = startFrame << 12;
  *(int*)((char*)&self->speed) = speed;
}
// @symbol _ZNK15dExtFrameCtrl_c13GetFrameCountEv
u32 dExtFrameCtrl_c::GetFrameCount() const
{
    u32 v = numFramesAndFlags;
    return ((v & 0x3fffffff) << 4) >> 16;
}
// @symbol _ZN15dExtFrameCtrl_c8SetFlagsEi
void dExtFrameCtrl_c::SetFlags(int flags)
{
    numFramesAndFlags = (numFramesAndFlags & 0x3fffffff) | flags;
}

// @symbol _ZN15dExtFrameCtrl_c8GetFlagsEv
int dExtFrameCtrl_c::GetFlags()
{
    return numFramesAndFlags & 0xC0000000;
}
// @symbol _ZN15dExtFrameCtrl_c8FinishedEv
int dExtFrameCtrl_c::Finished()
{
    u32 f = numFramesAndFlags;
    int cur = currFrame;
    return cur >= (int)((f & 0x3fffffff) - 1);
}

// @symbol _ZNK15dExtFrameCtrl_c12WillHitFrameEi
bool dExtFrameCtrl_c::WillHitFrame(int frame) const
{
    s32 f = frame << 12;
    s32 next = currFrame + speed;
    s32 num = numFramesAndFlags & ~0xc0000000;

    if ((numFramesAndFlags & 0xc0000000) == 0)
    {
        if (next < 0)
        {
            next = (next + num) % num;
            if ((f >= 0 && f < currFrame) || (next <= f && f < num))
                return true;
        }
        else if (next >= num)
        {
            next %= num;
            if ((currFrame <= f && f < num) || f < next)
                return true;
        }
        else if (currFrame <= next)
        {
            if (currFrame <= f && f < next)
                return true;
        }
        else
        {
            if (next <= f && f < currFrame)
                return true;
        }
    }
    else
    {
        if (next < 0)
            next = 0;
        if (next >= num)
            next = num - 1;

        if (currFrame <= next)
        {
            if (f >= currFrame && f < next)
                return true;
        }
        else if (f >= next && f < currFrame)
            return true;
    }

    return false;
}
// @symbol _ZN15dExtFrameCtrl_c4CopyERKS_
/* Copies the three members and leaves the vptr alone. */
void dExtFrameCtrl_c::Copy(const dExtFrameCtrl_c &anim)
{
    numFramesAndFlags = anim.numFramesAndFlags;
    currFrame         = anim.currFrame;
    speed             = anim.speed;
}
