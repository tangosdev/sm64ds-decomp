typedef short s16;

extern int func_ov002_020efebc(int *p);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void func_ov002_020efbdc(char *c);
extern int _ZNK7PathPtr5LoopsEv(void *p);
extern int _ZNK7PathPtr8NumNodesEv(void *p);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *p, void *out, unsigned idx);
extern void Vec3_Sub(void *out, void *a, void *b);
extern int LenVec3(void *v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern int Vec3_HorzDist(void *a, void *b);
extern short Vec3_HorzAngle(void *a, void *b);
extern short Vec3_VertAngle(void *a, void *b);
extern int AngleDiff(int a, int b);
extern void _Z14ApproachLinearRsss(short *r, short a, short b);
extern int func_ov002_020efe68(char *c);
extern void Vec3_MulScalar(void *out, void *v, int s);
extern void SubVec3(void *a, void *b, void *c);
extern int func_ov002_020efe7c(int *p);
extern void func_ov002_020efa54(char *c, int i);

struct V3 { int x, y, z; };

void func_ov002_020ef670(char *c)
{
    struct V3 prev, node, diff1, diff2, scaled1, scaled2;
    int looped;
    int ad2;
    int ad1;
    int hdist;
    int dv;
    int len2;
    int delta;
    short sl;
    short sb;
    short q2;
    int len1;

    if (func_ov002_020efebc((int *)c) != 0) {
        if (*(unsigned char *)(c + 0x42a) == 0) {
            if (DecIfAbove0_Short((unsigned short *)(c + 0x428)) == 0) {
                func_ov002_020efbdc(c);
                return;
            }
        } else {
            *(short *)(c + 0x428) = 0x12c;
        }
    }

    delta = *(int *)(c + 0x438) - *(int *)(c + 0x43c);
    looped = 0;
    if (_ZNK7PathPtr5LoopsEv(c + 0x430)) {
        if (*(int *)(c + 0x43c) > 0) {
            if (delta < 0) delta = _ZNK7PathPtr8NumNodesEv(c + 0x430) - 1;
        } else {
            if (delta >= _ZNK7PathPtr8NumNodesEv(c + 0x430)) delta = 0;
        }
        _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x430, &node, delta);
    } else {
        if (delta < 0 || delta >= _ZNK7PathPtr8NumNodesEv(c + 0x430)) {
            node.x = *(int *)(c + 0x5c);
            node.y = *(int *)(c + 0x60);
            node.z = *(int *)(c + 0x64);
        } else {
            _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x430, &node, delta);
        }
    }
    _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x430, &prev, *(int *)(c + 0x438));

    Vec3_Sub(&diff1, c + 0x5c, &prev);
    len1 = LenVec3(&diff1);
    Vec3_Sub(&diff2, &node, &prev);
    len2 = LenVec3(&diff2);
    dv = _ZN4cstd4fdivEii(len2, *(int *)(c + 0x98)) / 0x1000;

    hdist = Vec3_HorzDist(&node, &prev);
    if (hdist != 0) {
        if (*(int *)(c + 0x43c) > 0) {
            sl = Vec3_HorzAngle(&node, &prev);
            sb = Vec3_VertAngle(&node, &prev);
        } else {
            sl = Vec3_HorzAngle(&prev, &node);
            sb = Vec3_VertAngle(&prev, &node);
        }
    } else {
        sl = *(short *)(c + 0x8e);
        sb = *(short *)(c + 0x8c);
    }

    ad1 = AngleDiff(*(short *)(c + 0x424), sl);
    ad2 = AngleDiff(*(short *)(c + 0x422), sb);
    q2 = (short)(ad2 / dv);
    _Z14ApproachLinearRsss((short *)(c + 0x8e), sl, (short)(ad1 / dv));
    if (func_ov002_020efe68(c) != 0) {
        _Z14ApproachLinearRsss((short *)(c + 0x8c), sb, q2);
    }

    if (len1 == 0 || len1 <= *(int *)(c + 0x98)) {
        Vec3_MulScalar(&scaled1, &diff1, _ZN4cstd4fdivEii(*(int *)(c + 0x98), len1));
        SubVec3(c + 0x5c, &scaled1, c + 0x5c);
        looped = 1;
    } else {
        Vec3_MulScalar(&scaled2, &diff1, _ZN4cstd4fdivEii(*(int *)(c + 0x98), len1));
        SubVec3(c + 0x5c, &scaled2, c + 0x5c);
    }

    if (looped == 0) return;

    *(int *)(((int)c + 0x438) & 0xFFFFFFFFFFFFFFFF) += *(int *)(c + 0x43c);
    if (*(int *)(c + 0x438) < 0) {
        if (_ZNK7PathPtr5LoopsEv(c + 0x430)) {
            *(int *)(c + 0x438) = _ZNK7PathPtr8NumNodesEv(c + 0x430) - 1;
        } else {
            *(unsigned char *)(c + 0x42b) = 0x14;
            *(int *)(c + 0x43c) = 1;
            *(int *)(((unsigned)c + 0x438) & 0xFFFFFFFFFFFFFFFF) += *(int *)(c + 0x43c) * 2;
        }
    }
    if (*(int *)(c + 0x438) >= _ZNK7PathPtr8NumNodesEv(c + 0x430)) {
        if (_ZNK7PathPtr5LoopsEv(c + 0x430)) {
            *(int *)(c + 0x438) = 0;
        } else if (func_ov002_020efe7c((int *)c) != 0) {
            func_ov002_020efa54(c, 2);
        } else {
            *(unsigned char *)(c + 0x42b) = 0x14;
            *(int *)(c + 0x43c) = -1;
            *(int *)(((long long)(int)(c + 0x438)) & 0xFFFFFFFFFFFFFFFFLL) += *(int *)(c + 0x43c) * 2;
        }
    }

    *(short *)(c + 0x422) = *(short *)(c + 0x8c);
    *(short *)(c + 0x424) = *(short *)(c + 0x8e);
    *(short *)(c + 0x426) = *(short *)(c + 0x90);
}