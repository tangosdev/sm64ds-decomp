/* PORT-UNMATCHED path-follow update (func_ov002_020bfa74, 0x44c bytes).
 * Adopted from the banked near-miss draft (nearmiss/db.jsonl,
 * source log_attempt, 102 codegen divergences vs ROM). Semantics only;
 * retires when the function matches for real. */

typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;

struct State;
struct Vec16 { s16 x, y, z; };
union VecArg { struct Vec16 v; u32 w[2]; };
struct Vec32 { s32 x, y, z; };
struct PathPtrObj { int a, b; };

/* Force layout: path@4 dir@c nodes@14 pos@2c body@38 */
struct Frame {
    struct PathPtrObj path;
    s16 dirx, diry, dirz, dirpad;
    struct Vec32 nodes[2];
    struct Vec32 pos;
    volatile unsigned bodyIdx;
};

void _ZN7PathPtrC1Ev(void* self);
void _ZN7PathPtr6FromIDEj(void* self, u32 id);
s32 _ZNK7PathPtr8NumNodesEv(const void* self);
void _ZNK7PathPtr7GetNodeER7Vector3j(const void* self, void* out, u32 idx);
u32 func_0203ad54(void* c);
s16 Vec3_HorzAngle(const void* v0, const void* v1);
s16 Vec3_VertAngle(const void* v0, const void* v1);
s32 func_ov002_020e0478(void* c);
s32 _ZN6Player7IsStateERNS_5StateE(void* self, void* state);
s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 a, s32 b);
s32 Vec3_HorzLen(const void* v);
u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, const void* pos, u32 d);
void func_ov002_020bf800(void* self, union VecArg v);

extern s16 data_02082214[];
extern s32 data_ov002_020ff210[];
extern struct State data_ov002_0211013c;
extern struct State data_ov002_02110154;
extern struct State data_ov002_02110574;
extern struct State data_ov002_02110184;
extern struct State data_ov002_02110514;
extern struct State data_ov002_021104e4;
extern struct State data_ov002_02110364;
extern struct State data_ov002_0211064c;
extern struct State data_ov002_021101b4;
extern struct State data_ov002_021102d4;

int func_ov002_020bfa74(char* self)
{
    struct Frame fr;
    s32 numNodes;
    s32 i;
    struct Vec32* node;
    s16 horzAngle, vertAngle;
    s32 vertIdx, horzIdx;
    s16 vertA, horzB, horzC;
    s16* tbl;
    u16* st;

    if (*(u32*)(self + 0x664) != 0xB) return 0;
    if (*(u32*)(self + 0x670) == 0xFF) goto ret0;

    _ZN7PathPtrC1Ev(&fr.path);
    _ZN7PathPtr6FromIDEj(&fr.path, *(u32*)(self + 0x670));
    numNodes = _ZNK7PathPtr8NumNodesEv(&fr.path);

    i = 0;
    if (numNodes > 0) {
        node = fr.nodes;
        do {
            _ZNK7PathPtr7GetNodeER7Vector3j(&fr.path, node, i);
            i = i + 1;
            node = node + 1;
        } while (i < numNodes);
    }

    fr.bodyIdx = (u8)func_0203ad54(&fr.path);
    if (fr.bodyIdx >= 4) fr.bodyIdx = 1;

    horzAngle = Vec3_HorzAngle(&fr.nodes[0], &fr.nodes[1]);
    vertAngle = Vec3_VertAngle(&fr.nodes[0], &fr.nodes[1]);
    vertIdx = (u16)vertAngle >> 4;
    horzIdx = (u16)horzAngle >> 4;

    tbl = data_02082214;
    {
        s16 dy = tbl[vertIdx * 2];
        s16 va = tbl[vertIdx * 2 + 1];
        s16 hb = tbl[horzIdx * 2];
        s16 hc = tbl[horzIdx * 2 + 1];
        s16 dx = (s16)(((s32)va * (s32)hb + 0x800) >> 0xc);
        s16 dz = (s16)(((s32)va * (s32)hc + 0x800) >> 0xc);
        fr.diry = dy;
        fr.dirx = dx;
        fr.dirz = dz;
        /* pack with unsigned halfword reload */
        {
            unsigned short *p = (unsigned short *)&fr.dirx;
            unsigned short *q = (unsigned short *)((char *)&fr.path - 12);
            unsigned short x = p[0];
            unsigned short y = p[1];
            unsigned short z = p[2];
            q[0] = x;
            q[1] = y;
            q[2] = z;
            func_ov002_020bf800(self, *(union VecArg *)q);
        }
    }

    *(u32*)(self + 0x624) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(u32*)(self + 0x624), 3, 0x90, self + 0x74, 0);
    if (*(u8*)(self + 0x6f9) != 0 || *(u8*)(self + 0x709) != 0 || func_ov002_020e0478(self) != 0)
        return 0;

    if (*(u8*)(self + 0x6de) == 0) {
        if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211013c)
         || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110154)
         || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110574)
         || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110184)
         || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110514)
         || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_021104e4)
         || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110364)
         || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211064c)) {
            s32 dirScale;
            if (*(s32*)(self + 0x98) <= 0) dirScale = 0x8000;
            else dirScale = (s32)(((s64)data_ov002_020ff210[fr.bodyIdx] * *(s32*)(self + 0x98) + 0x800) >> 0xc);
            {
                s32* q = (s32*)((int)(((s64)(int)(self + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL));
                *q = *q + (s32)(((s64)dirScale * fr.dirx + 0x800) >> 0xc);
            }
            self = self + 0x64;
            *(s32*)self = *(s32*)self + (s32)(((s64)dirScale * fr.dirz + 0x800) >> 0xc);
        }
        goto ret1;
    }
    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_021101b4)
     || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_021102d4))
        return 1;

    {
        s16 cA = tbl[(*(u16*)(self + 0x94) >> 4) * 2];
        s32 term1 = (s32)(((s64)*(s32*)(self + 0x98) * cA + 0x800) >> 0xc);
        s32 scaled = (s32)((fr.dirx * 0x2800LL + 0x800) >> 0xc);
        s16 sA; s32 term2;
        fr.pos.x = term1;
        fr.pos.x = term1 + scaled;
        sA = tbl[(*(u16*)(self + 0x94) >> 4) * 2 + 1];
        term2 = (s32)(((s64)*(s32*)(self + 0x98) * sA + 0x800) >> 0xc);
        scaled = (s32)((fr.dirz * 0x2800LL + 0x800) >> 0xc);
        fr.pos.z = term2;
        fr.pos.z = term2 + scaled;
        *(u16*)(self + 0x94) = _ZN4cstd5atan2E5Fix12IiES1_(fr.pos.x, fr.pos.z);
        *(s32*)(self + 0x98) = Vec3_HorzLen(&fr.pos);
    }
ret1:
    return 1;
ret0:
    return 0;
}
