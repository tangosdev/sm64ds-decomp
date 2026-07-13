//cpp
typedef int Fix12i;
typedef unsigned int u32;

struct Vector3 { Fix12i x, y, z; };

struct CylinderClsn
{
    virtual int v0();
    virtual int v1();
    virtual Vector3& GetPos();      // vt+0x08
    virtual u32 GetOwnerID();       // vt+0x0C

    Fix12i radius;       // 0x04
    Fix12i height;       // 0x08
    Vector3 pushback;    // 0x0C
    u32 flags;           // 0x18
    u32 vulnFlags;       // 0x1C
    u32 hitFlags;        // 0x20
    u32 otherOwner;      // 0x24
    u32 unk28;           // 0x28
    CylinderClsn* next;  // 0x2C

    static void Process();
};

extern "C" {
extern CylinderClsn* data_0209cee8;
extern Fix12i Vec3_HorzLen(const Vector3* v);
extern void func_02014f44(unsigned int id, CylinderClsn* clsn);
}

namespace cstd { int fdiv(int a, int b); }

void CylinderClsn::Process()
{
    u32 owner0;
    Fix12i vertDist;
    Vector3* pos0;
    CylinderClsn* other;
    Fix12i dist;
    Fix12i overlap;
    u32 sharedFlags;
    Fix12i minDist;
    Vector3 diff;

    if (data_0209cee8 == 0)
        return;

    minDist = 0x1000;

    do
    {
        pos0 = &data_0209cee8->GetPos();
        owner0 = data_0209cee8->GetOwnerID();

        for (other = data_0209cee8->next; other != 0; other = other->next)
        {
            sharedFlags = (data_0209cee8->flags & other->vulnFlags) | (data_0209cee8->vulnFlags & other->flags);
            if (sharedFlags == 0)
                continue;
            if (owner0 != 0 && owner0 == other->GetOwnerID())
                continue;

            Vector3& pos1 = other->GetPos();
            diff.y = pos1.y - pos0->y;
            if (diff.y < 0)
                vertDist = other->height + diff.y;
            else
                vertDist = data_0209cee8->height - diff.y;
            if (vertDist <= 0)
                continue;

            diff.x = pos1.x - pos0->x;
            diff.z = pos1.z - pos0->z;
            dist = Vec3_HorzLen(&diff);
            if (dist == 0)
            {
                dist = minDist;
                diff.x = dist;
            }

            overlap = data_0209cee8->radius + other->radius - dist;
            if (overlap <= 0)
                continue;

            data_0209cee8->hitFlags = other->flags;
            other->hitFlags = data_0209cee8->flags;
            data_0209cee8->otherOwner = other->GetOwnerID();
            other->otherOwner = data_0209cee8->GetOwnerID();

            if (sharedFlags & 0x4000000)
            {
                u32* pf1 = (u32*)((unsigned long long)(unsigned int)((char*)other + 0x20) & 0xFFFFFFFFFFFFFFFFULL);
                u32* pf0 = (u32*)((unsigned long long)(unsigned int)((char*)data_0209cee8 + 0x20) & 0xFFFFFFFFFFFFFFFFULL);
                *pf0 |= 0x8000000;
                overlap -= 0x50000;
                *pf1 |= 0x8000000;
                if (overlap < 0)
                    continue;
            }

            {
                u32 f0 = data_0209cee8->flags;
                if (f0 & 2)
                    continue;
                u32 f1 = other->flags;
                if (f1 & 2)
                    continue;

                if (f0 & 4)
                {
                    if (f1 & 4)
                    {
                        if (diff.y < 0)
                        {
                            Fix12i h = vertDist >> 1;
                            data_0209cee8->pushback.y = h;
                            other->pushback.y = -h;
                        }
                        else
                        {
                            Fix12i h = vertDist >> 1;
                            data_0209cee8->pushback.y = -h;
                            other->pushback.y = h;
                        }
                        other->pushback.x = 0;
                        other->pushback.z = 0;
                    }
                    else
                    {
                        if (diff.y < 0)
                            other->pushback.y = -vertDist;
                        else
                            other->pushback.y = vertDist;
                        if (data_0209cee8->flags & 8)
                        {
                            Fix12i r = data_0209cee8->radius;
                            Fix12i t = overlap - (r - (((r << 1) + r) >> 3));
                            if (t < 0)
                                overlap = 0;
                            else
                                overlap = cstd::fdiv(t, dist);
                            func_02014f44(data_0209cee8->otherOwner, data_0209cee8);
                        }
                        else
                            overlap = cstd::fdiv(overlap, dist);
                        other->pushback.x = (Fix12i)(((long long)diff.x * overlap + 0x800) >> 12);
                        other->pushback.z = (Fix12i)(((long long)diff.z * overlap + 0x800) >> 12);
                    }
                    data_0209cee8->pushback.x = 0;
                    data_0209cee8->pushback.z = 0;
                }
                else if (f1 & 4)
                {
                    if (diff.y < 0)
                        data_0209cee8->pushback.y = vertDist;
                    else
                        data_0209cee8->pushback.y = -vertDist;
                    if (other->flags & 8)
                    {
                        Fix12i r = data_0209cee8->radius;
                        Fix12i t = overlap - (r - (((r << 1) + r) >> 3));
                        if (t < 0)
                            overlap = 0;
                        else
                            overlap = cstd::fdiv(t, dist);
                        func_02014f44(other->otherOwner, other);
                    }
                    else
                        overlap = cstd::fdiv(overlap, dist);
                    data_0209cee8->pushback.x = -(Fix12i)(((long long)diff.x * overlap + 0x800) >> 12);
                    data_0209cee8->pushback.z = -(Fix12i)(((long long)diff.z * overlap + 0x800) >> 12);
                    other->pushback.x = 0;
                    other->pushback.z = 0;
                }
                else if (!(f0 & 0x10000000) && !(f1 & 0x10000000))
                {
                    if (diff.y < 0)
                    {
                        Fix12i h = vertDist >> 1;
                        data_0209cee8->pushback.y = h;
                        other->pushback.y = -h;
                    }
                    else
                    {
                        Fix12i h = vertDist >> 1;
                        data_0209cee8->pushback.y = -h;
                        other->pushback.y = h;
                    }
                    Fix12i ratio = cstd::fdiv(overlap, dist) >> 1;
                    data_0209cee8->pushback.x = -(Fix12i)(((long long)diff.x * ratio + 0x800) >> 12);
                    data_0209cee8->pushback.z = -(Fix12i)(((long long)diff.z * ratio + 0x800) >> 12);
                    other->pushback.x = (Fix12i)(((long long)diff.x * ratio + 0x800) >> 12);
                    other->pushback.z = (Fix12i)(((long long)diff.z * ratio + 0x800) >> 12);
                }
            }
        }

        {
            CylinderClsn* nxt = data_0209cee8->next;
            data_0209cee8->unk28 = 0;
            data_0209cee8->next = 0;
            data_0209cee8 = nxt;
        }
    } while (data_0209cee8 != 0);
}
