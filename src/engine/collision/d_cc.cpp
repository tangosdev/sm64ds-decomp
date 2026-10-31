//cpp
/* The cylinder-collision root (arm9 .text 0x02014aa8..0x020150e8): dCc_c, the
 * intrusive-list node that dCcPos_c, dCcAc_c and dCcAcPos_c derive from.
 * Process() walks the global list whose head is listHead, finds pairs
 * whose flags interlock, and writes per-node pushback; the two file-static
 * helpers resolve a flag-8 hit's owner to the player actor and hand it the
 * collider through Player::StartClimb.
 * mwccarm emits .text in reverse source order, so the definitions below run
 * ROM-descending.
 *
 * ~dCc_c() is the key function: mwcc emits D2, D0 and D1 from the one
 * definition and the cartridge keeps all three (0x02015058 / 0x0201507c /
 * 0x020150a8). The complete-object ctor variant C1 is the homeless copy --
 * the ROM keeps only C2 at 0x020150cc, which every derived constructor
 * enters through the base chain. The vtable/RTTI records land in .data as
 * compiler-emitted output externalized to the canonical bytes
 * (_ZTS5dCc_c 0x0208e658, _ZTI5dCc_c 0x0208e660, _ZTV5dCc_c 0x0208e6ec).
 */
#include "dCc_c.h"
#include "dActor_c.h"
#include "nitro/fx/fx.h"
#include "cstd.h"
#include "math/CalcLib.h"
#include "Player.h"

// @symbol _ZN5dCc_cC2Ev
/* C2, the base-subobject constructor. The vptr store is the compiler's; the
 * body is only what the ROM's constructor actually writes -- both intrusive
 * list links zeroed, an unlinked node. */
dCc_c::dCc_c() {
    prev = 0;
    next = 0;
}

// @symbol _ZN5dCc_cD1Ev
/* One definition emits D2, D0 and D1: the destructor body's only obligation
   is the intrusive-list unlink, and D0 follows it with the class-local
   operator delete (Memory::operator_delete2, see the header). */
dCc_c::~dCc_c() {
    Unlink();
}

// @symbol _ZN5dCc_c4InitEiijj
void dCc_c::Init(Fix12i radius_, Fix12i height_, u32 flags_, u32 vulnFlags_) {
    radius = radius_;
    height = height_;
    flags = flags_;
    vulnFlags = vulnFlags_;
}

// @symbol _ZN5dCc_c5ClearEv
void dCc_c::Clear() {
    pushback.x = 0;
    pushback.y = 0;
    pushback.z = 0;
    otherOwner = 0;
    hitFlags = 0;
}

// @symbol _ZN5dCc_c6UpdateEv
/* Link this node in at the head of the active list. The mirror of the unlink
   that the destructors run; see include/dCc_c.h. */
void dCc_c::Update() {
    if (flags & 1)
        return;
    next = listHead;
    if (listHead)
        listHead->prev = this;
    listHead = this;
}

// @symbol _ZN5dCc_c6UnlinkEv
/* Removes this collision node from the global intrusive list. The member role
 * is proven by the prev/next rewiring; the original spelling `Unlink` is an
 * evidence-bounded reconstruction. */
void dCc_c::Unlink() {
    if (prev)
        prev->next = next;
    else if (listHead == this)
        listHead = next;

    if (next)
        next->prev = prev;

    prev = 0;
    next = 0;
}

// @symbol func_02014f5c
/* Resolve a collider hit owner and, only when it is the player, offer it the
 * collider to climb. IsPlayer returns int: the ROM materialises the 0/1 before
 * testing it, and a bool predicate or a direct compare folds that away. */
#define PLAYER_ACTOR_ID 0xbf
static inline int IsPlayer(dActor_c *actor) {
    return actor->actorID == PLAYER_ACTOR_ID;
}

extern "C" void func_02014f5c(dCc_c *self, u32 id) {
    dActor_c *actor = dActor_c::FindWithID(id);
    if (actor && IsPlayer(actor))
        static_cast<Player *>(actor)->StartClimb(self);
}

// @symbol func_02014f44
extern "C" void func_02014f44(u32 id, dCc_c *self) {
    func_02014f5c(self, id);
}

// @symbol _ZN5dCc_c7ProcessEv
void dCc_c::Process() {
    u32 ownerID;
    Fix12i verticalOverlap;
    Vector3 *position;
    dCc_c *other;
    Fix12i horizontalDistance;
    Fix12i overlap;
    u32 sharedFlags;
    Fix12i coincidentDistance;
    Vector3 separation;

    if (listHead == 0)
        return;

    coincidentDistance = 0x1000;

    do {
        position = &listHead->GetPos();
        ownerID = listHead->GetOwnerID();

        for (other = listHead->next; other != 0; other = other->next) {
            sharedFlags =
                (listHead->flags & other->vulnFlags) | (listHead->vulnFlags & other->flags);
            if (sharedFlags == 0)
                continue;
            if (ownerID != 0 && ownerID == other->GetOwnerID())
                continue;

            Vector3 &otherPosition = other->GetPos();
            separation.y = otherPosition.y - position->y;
            if (separation.y < 0)
                verticalOverlap = other->height + separation.y;
            else
                verticalOverlap = listHead->height - separation.y;
            if (verticalOverlap <= 0)
                continue;

            separation.x = otherPosition.x - position->x;
            separation.z = otherPosition.z - position->z;
            horizontalDistance = Vec3_HorzLen(&separation);
            // Coincident centers need a nonzero separation for division.
            if (horizontalDistance == 0) {
                horizontalDistance = coincidentDistance;
                separation.x = horizontalDistance;
            }

            overlap = listHead->radius + other->radius - horizontalDistance;
            if (overlap <= 0)
                continue;

            listHead->hitFlags = other->flags;
            other->hitFlags = listHead->flags;
            listHead->otherOwner = other->GetOwnerID();
            other->otherOwner = listHead->GetOwnerID();

            if (sharedFlags & 0x4000000) {
                listHead->hitFlags |= 0x8000000;
                overlap -= 0x50000;
                other->hitFlags |= 0x8000000;
                if (overlap < 0)
                    continue;
            }

            {
                u32 flags = listHead->flags;
                if (flags & 2)
                    continue;
                u32 otherFlags = other->flags;
                if (otherFlags & 2)
                    continue;

                if (flags & 4) {
                    if (otherFlags & 4) {
                        if (separation.y < 0) {
                            Fix12i halfVerticalOverlap = verticalOverlap >> 1;
                            listHead->pushback.y = halfVerticalOverlap;
                            other->pushback.y = -halfVerticalOverlap;
                        } else {
                            Fix12i halfVerticalOverlap = verticalOverlap >> 1;
                            listHead->pushback.y = -halfVerticalOverlap;
                            other->pushback.y = halfVerticalOverlap;
                        }
                        other->pushback.x = 0;
                        other->pushback.z = 0;
                    } else {
                        if (separation.y < 0)
                            other->pushback.y = -verticalOverlap;
                        else
                            other->pushback.y = verticalOverlap;
                        if (listHead->flags & 8) {
                            Fix12i radius = listHead->radius;
                            Fix12i adjustedOverlap =
                                overlap - (radius - (((radius << 1) + radius) >> 3));
                            if (adjustedOverlap < 0)
                                overlap = 0;
                            else
                                overlap = cstd::fdiv(adjustedOverlap, horizontalDistance);
                            func_02014f44(listHead->otherOwner, listHead);
                        } else
                            overlap = cstd::fdiv(overlap, horizontalDistance);
                        other->pushback.x = FX_MUL(separation.x, overlap);
                        other->pushback.z = FX_MUL(separation.z, overlap);
                    }
                    listHead->pushback.x = 0;
                    listHead->pushback.z = 0;
                } else if (otherFlags & 4) {
                    if (separation.y < 0)
                        listHead->pushback.y = verticalOverlap;
                    else
                        listHead->pushback.y = -verticalOverlap;
                    if (other->flags & 8) {
                        Fix12i radius = listHead->radius;
                        Fix12i adjustedOverlap =
                            overlap - (radius - (((radius << 1) + radius) >> 3));
                        if (adjustedOverlap < 0)
                            overlap = 0;
                        else
                            overlap = cstd::fdiv(adjustedOverlap, horizontalDistance);
                        func_02014f44(other->otherOwner, other);
                    } else
                        overlap = cstd::fdiv(overlap, horizontalDistance);
                    listHead->pushback.x = -FX_MUL(separation.x, overlap);
                    listHead->pushback.z = -FX_MUL(separation.z, overlap);
                    other->pushback.x = 0;
                    other->pushback.z = 0;
                } else if (!(flags & 0x10000000) && !(otherFlags & 0x10000000)) {
                    if (separation.y < 0) {
                        Fix12i halfVerticalOverlap = verticalOverlap >> 1;
                        listHead->pushback.y = halfVerticalOverlap;
                        other->pushback.y = -halfVerticalOverlap;
                    } else {
                        Fix12i halfVerticalOverlap = verticalOverlap >> 1;
                        listHead->pushback.y = -halfVerticalOverlap;
                        other->pushback.y = halfVerticalOverlap;
                    }
                    Fix12i ratio = cstd::fdiv(overlap, horizontalDistance) >> 1;
                    listHead->pushback.x = -FX_MUL(separation.x, ratio);
                    listHead->pushback.z = -FX_MUL(separation.z, ratio);
                    other->pushback.x = FX_MUL(separation.x, ratio);
                    other->pushback.z = FX_MUL(separation.z, ratio);
                }
            }
        }

        {
            dCc_c *nextCollider = listHead->next;
            listHead->prev = 0;
            listHead->next = 0;
            listHead = nextCollider;
        }
    } while (listHead != 0);
}
