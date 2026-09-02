// HOST COPY of CylinderClsn::Process (src/_ZN12CylinderClsn7ProcessEv.cpp,
// arm9 0x02014b00) -- the frame's cylinder-overlap pass over the intrusive
// list Update threads onto data_0209cee8.
//
// WHY A COPY RATHER THAN THE MATCHED SOURCE: the matched TU calls GetPos()
// and GetOwnerID() through include/CylinderClsn.h, and MSVC collapses the
// Itanium D1/D0 destructor PAIR into a single deleting-destructor slot.
// CylinderClsn declares its destructor FIRST, so under MSVC every virtual
// past it lands one slot low: GetPos indexes slot 1, which in the port's
// ROM-shaped cylinder vtables is D0. The first overlap pass Mario ran freed
// a tree cylinder out of the middle of the heap (UnlinkNode AV at frame 0).
//
// The ROM shape is load-bearing on the other side: the matched grab test
// func_ov002_020caf98 models the cylinder as three plain virtuals and calls
// slot 2 for GetPos, which is exactly where the ROM tables keep it. One
// object, one vptr, two contested layouts -- so the port keeps every
// cylinder vtable ROM-shaped ([0] D1, [1] D0, [2] GetPos, [3] GetOwnerID)
// and this copy dispatches by explicit slot index, the same reading the
// C-compiled TUs use. Same body otherwise, transcribed line for line.
//
// The x86 ABI note: the port's cylinder vtables hold __fastcall(void*,void*)
// thunks (hal/actor_classes.cpp), which a __thiscall zero-argument call
// enters correctly -- this in ecx, edx dead, nothing on the stack.
//
// WHAT ACTUALLY CRASHED THIS ON FRAME 0, and it was not the slot shift above:
// two of the four cylinder vtables were never filled. _ZTV18MovingCylinderClsn
// and _ZTV25MovingCylinderClsnWithPos were bare zeroed arrays in
// hal/actor_vtables.cpp, because until this pass existed nothing in the port
// dispatched a cylinder virtual at all. The castle grounds thread 28 nodes
// onto data_0209cee8 from the first frame: 21 CylinderClsnWithPos (the trees,
// whose table WAS filled) and then six MovingCylinderClsn and one
// MovingCylinderClsnWithPos, whose GetPos and GetOwnerID were both null. The
// loop below asks every node for both, so it walked the 21 good ones and
// called address zero on the 22nd. hal/actor_classes.cpp now fills both
// tables ROM-shaped and gates 16 and 21 carry the slot fillers.
//
// PORT_TRACE_CYL=1 prints the list with each node's class and both slots,
// which is how that was pinned and how the next null should be read.

typedef int Fix12i;

struct CylClsn {
    void **vt;             /* 0x00 */
    Fix12i radius;         /* 0x04 */
    Fix12i height;         /* 0x08 */
    Fix12i pushback[3];    /* 0x0c */
    unsigned flags;        /* 0x18 */
    unsigned vulnFlags;    /* 0x1c */
    unsigned hitFlags;     /* 0x20 */
    unsigned otherOwner;   /* 0x24 */
    CylClsn *prev;         /* 0x28 */
    CylClsn *next;         /* 0x2c */
};

typedef Fix12i *(__fastcall *ClsnGetPosFn)(void *self, void *dead);
typedef unsigned (__fastcall *ClsnGetOwnerFn)(void *self, void *dead);

static Fix12i *clsn_pos(CylClsn *c)       /* ROM slot 2 */
{ return ((ClsnGetPosFn)c->vt[2])(c, 0); }
static unsigned clsn_owner(CylClsn *c)    /* ROM slot 3 */
{ return ((ClsnGetOwnerFn)c->vt[3])(c, 0); }

extern "C" {
extern CylClsn *data_0209cee8;
extern int port_vs_king_target(void);   /* >0 == a King of the Star match */
Fix12i Vec3_HorzLen(const Fix12i *v);
void func_02014f44(unsigned id, CylClsn *clsn);
int _ZN4cstd4fdivEii(int a, int b);
extern void *_ZTV19CylinderClsnWithPos[];
extern void *_ZTV18MovingCylinderClsn[];
extern void *_ZTV25MovingCylinderClsnWithPos[];
extern void *_ZTV9LakituBro[];
}

/* PORT_TRACE_CYL=1 walks the list without dispatching anything and names each
   node by which of the four cylinder vtables it carries, with its slot 2 and
   3. It is what a frame-0 fault in here should be read with: a null in either
   column is a class whose GetPos/GetOwnerID never got compiled into the
   slice, and the pass calls both on every node. */
#include <stdio.h>
#include <stdlib.h>
static const char *cyl_vt_name(void **vt)
{
    if (vt == _ZTV19CylinderClsnWithPos)       return "CylinderClsnWithPos";
    if (vt == _ZTV18MovingCylinderClsn)        return "MovingCylinderClsn";
    if (vt == _ZTV25MovingCylinderClsnWithPos) return "MovingCylinderClsnWithPos";
    return 0;
}

/* Walk what is left of the list and flag any node whose vt is no longer one
   of the cylinder tables. `when` names the moment, so the first [cylbad] line
   in the log is the step that corrupted it. The LakituBro table is printed as
   a landmark because the frame-0 fault dispatches out of its slot 17. */
static int cyl_scan(const char *when)
{
    int n = 0, bad = 0;
    for (CylClsn *p = data_0209cee8; p && n <= 200; p = p->next, ++n) {
        if (!cyl_vt_name(p->vt)) {
            fprintf(stderr,
                    "[cylbad] %s: node %d %p vt=%p (lakitu vt=%p delta=%ld) "
                    "vt[2]=%p vt[3]=%p\n",
                    when, n, (void *)p, (void *)p->vt,
                    (void *)_ZTV9LakituBro,
                    (long)((char *)p->vt - (char *)_ZTV9LakituBro),
                    p->vt[2], p->vt[3]);
            bad = 1;
        }
    }
    return bad;
}

static void cyl_trace_list(void)
{
    int n = 0;
    for (CylClsn *p = data_0209cee8; p; p = p->next, ++n) {
        const char *nm = "?";
        if (p->vt == _ZTV19CylinderClsnWithPos)         nm = "CylinderClsnWithPos";
        else if (p->vt == _ZTV18MovingCylinderClsn)     nm = "MovingCylinderClsn";
        else if (p->vt == _ZTV25MovingCylinderClsnWithPos)
                                                        nm = "MovingCylinderClsnWithPos";
        fprintf(stderr, "[cyl] %2d %p vt=%p %-26s GetPos=%p GetOwnerID=%p%s\n",
                n, (void *)p, (void *)p->vt, nm, p->vt[2], p->vt[3],
                (!p->vt[2] || !p->vt[3]) ? "   <-- NULL SLOT" : "");
        if (n > 200) { fprintf(stderr, "[cyl] list runs away\n"); break; }
    }
    fprintf(stderr, "[cyl] %d nodes\n", n);
}

extern "C" void port_cylinder_clsn_process(void)
{
    unsigned owner0;
    Fix12i vertDist;
    Fix12i *pos0;
    CylClsn *other;
    Fix12i dist;
    Fix12i overlap;
    unsigned sharedFlags;
    Fix12i minDist;
    Fix12i diff[3];

    if (data_0209cee8 == 0)
        return;

    static int trace = -1;
    if (trace < 0) trace = getenv("PORT_TRACE_CYL") != 0;
    if (trace) cyl_trace_list();

    minDist = 0x1000;
    int headidx = 0;

    do
    {
        if (trace) {
            const char *nm = cyl_vt_name(data_0209cee8->vt);
            fprintf(stderr, "[cylhead] %2d %p %s\n", headidx,
                    (void *)data_0209cee8, nm ? nm : "FOREIGN");
            cyl_scan("head-top");
        }
        pos0 = clsn_pos(data_0209cee8);
        owner0 = clsn_owner(data_0209cee8);

        /* KING OF THE STAR dupe hardening: an EXPLICIT deterministic tiebreak
           for a same-frame multi-touch. The ROM records the LAST colliding
           partner in otherOwner (last-writer-wins over the list). That is
           already deterministic because the list order is, but here we make the
           winner provable: this head keeps the LOWEST owner id (lowest player
           slot) of everyone it overlaps this frame, so every console resolves a
           tie to the same slot regardless of list order. Off (king_target==0)
           leaves the ROM's write byte-for-byte unchanged. */
        const int king = port_vs_king_target() > 0;
        unsigned head_low = 0;
        int head_set = 0;

        for (other = data_0209cee8->next; other != 0; other = other->next)
        {
            sharedFlags = (data_0209cee8->flags & other->vulnFlags) |
                          (data_0209cee8->vulnFlags & other->flags);
            if (sharedFlags == 0)
                continue;
            if (owner0 != 0 && owner0 == clsn_owner(other))
                continue;

            Fix12i *pos1 = clsn_pos(other);
            diff[1] = pos1[1] - pos0[1];
            if (diff[1] < 0)
                vertDist = other->height + diff[1];
            else
                vertDist = data_0209cee8->height - diff[1];
            if (vertDist <= 0)
                continue;

            diff[0] = pos1[0] - pos0[0];
            diff[2] = pos1[2] - pos0[2];
            dist = Vec3_HorzLen(diff);
            if (dist == 0)
            {
                dist = minDist;
                diff[0] = dist;
            }

            overlap = data_0209cee8->radius + other->radius - dist;
            if (overlap <= 0)
                continue;

            data_0209cee8->hitFlags = other->flags;
            other->hitFlags = data_0209cee8->flags;
            {
                const unsigned cand = clsn_owner(other);
                if (king) {
                    if (!head_set || cand < head_low) {
                        head_low = cand;
                        head_set = 1;
                    }
                    data_0209cee8->otherOwner = head_low;
                } else {
                    data_0209cee8->otherOwner = cand;
                }
            }
            other->otherOwner = clsn_owner(data_0209cee8);

            if (sharedFlags & 0x4000000)
            {
                data_0209cee8->hitFlags |= 0x8000000;
                overlap -= 0x50000;
                other->hitFlags |= 0x8000000;
                if (overlap < 0)
                    continue;
            }

            {
                unsigned f0 = data_0209cee8->flags;
                if (f0 & 2)
                    continue;
                unsigned f1 = other->flags;
                if (f1 & 2)
                    continue;

                if (f0 & 4)
                {
                    if (f1 & 4)
                    {
                        if (diff[1] < 0)
                        {
                            Fix12i h = vertDist >> 1;
                            data_0209cee8->pushback[1] = h;
                            other->pushback[1] = -h;
                        }
                        else
                        {
                            Fix12i h = vertDist >> 1;
                            data_0209cee8->pushback[1] = -h;
                            other->pushback[1] = h;
                        }
                        other->pushback[0] = 0;
                        other->pushback[2] = 0;
                    }
                    else
                    {
                        if (diff[1] < 0)
                            other->pushback[1] = -vertDist;
                        else
                            other->pushback[1] = vertDist;
                        if (data_0209cee8->flags & 8)
                        {
                            Fix12i r = data_0209cee8->radius;
                            Fix12i t = overlap - (r - (((r << 1) + r) >> 3));
                            if (t < 0)
                                overlap = 0;
                            else
                                overlap = _ZN4cstd4fdivEii(t, dist);
                            if (trace)
                                fprintf(stderr, "[cylnotify] head=%p id=%08x\n",
                                        (void *)data_0209cee8,
                                        data_0209cee8->otherOwner);
                            func_02014f44(data_0209cee8->otherOwner,
                                          data_0209cee8);
                            if (trace) cyl_scan("after-notify-head");
                        }
                        else
                            overlap = _ZN4cstd4fdivEii(overlap, dist);
                        other->pushback[0] =
                            (Fix12i)(((long long)diff[0] * overlap + 0x800) >> 12);
                        other->pushback[2] =
                            (Fix12i)(((long long)diff[2] * overlap + 0x800) >> 12);
                    }
                    data_0209cee8->pushback[0] = 0;
                    data_0209cee8->pushback[2] = 0;
                }
                else if (f1 & 4)
                {
                    if (diff[1] < 0)
                        data_0209cee8->pushback[1] = vertDist;
                    else
                        data_0209cee8->pushback[1] = -vertDist;
                    if (other->flags & 8)
                    {
                        Fix12i r = data_0209cee8->radius;
                        Fix12i t = overlap - (r - (((r << 1) + r) >> 3));
                        if (t < 0)
                            overlap = 0;
                        else
                            overlap = _ZN4cstd4fdivEii(t, dist);
                        if (trace)
                            fprintf(stderr, "[cylnotify] other=%p id=%08x\n",
                                    (void *)other, other->otherOwner);
                        func_02014f44(other->otherOwner, other);
                        if (trace) cyl_scan("after-notify-other");
                    }
                    else
                        overlap = _ZN4cstd4fdivEii(overlap, dist);
                    data_0209cee8->pushback[0] =
                        -(Fix12i)(((long long)diff[0] * overlap + 0x800) >> 12);
                    data_0209cee8->pushback[2] =
                        -(Fix12i)(((long long)diff[2] * overlap + 0x800) >> 12);
                    other->pushback[0] = 0;
                    other->pushback[2] = 0;
                }
                else if (!(f0 & 0x10000000) && !(f1 & 0x10000000))
                {
                    if (diff[1] < 0)
                    {
                        Fix12i h = vertDist >> 1;
                        data_0209cee8->pushback[1] = h;
                        other->pushback[1] = -h;
                    }
                    else
                    {
                        Fix12i h = vertDist >> 1;
                        data_0209cee8->pushback[1] = -h;
                        other->pushback[1] = h;
                    }
                    Fix12i ratio = _ZN4cstd4fdivEii(overlap, dist) >> 1;
                    data_0209cee8->pushback[0] =
                        -(Fix12i)(((long long)diff[0] * ratio + 0x800) >> 12);
                    data_0209cee8->pushback[2] =
                        -(Fix12i)(((long long)diff[2] * ratio + 0x800) >> 12);
                    other->pushback[0] =
                        (Fix12i)(((long long)diff[0] * ratio + 0x800) >> 12);
                    other->pushback[2] =
                        (Fix12i)(((long long)diff[2] * ratio + 0x800) >> 12);
                }
            }
        }

        {
            CylClsn *nxt = data_0209cee8->next;
            data_0209cee8->prev = 0;
            data_0209cee8->next = 0;
            data_0209cee8 = nxt;
            ++headidx;
        }
    } while (data_0209cee8 != 0);
}
