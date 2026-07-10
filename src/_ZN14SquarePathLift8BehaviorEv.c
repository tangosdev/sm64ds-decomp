typedef int Fix12;
extern int _ZNK7PathPtr5LoopsEv(void *p);
extern int _ZNK7PathPtr8NumNodesEv(void *p);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *p, void *out, unsigned idx);
extern void Vec3_Sub(void *out, void *a, void *b);
extern int LenVec3(void *v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalar(void *out, void *v, int s);
extern void SubVec3(void *a, void *b, void *c);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *p);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *p);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *p, Fix12 a, int b);
struct V3 { int x, y, z; };
int _ZN14SquarePathLift8BehaviorEv(char *c)
{
    struct V3 prev, node, diff, scaled1, scaled2;
    int looped, delta, len;
    *(int *)(c + 0x98) = 0xa000;
    delta = *(int *)(c + 0x328) - *(int *)(c + 0x32c);
    looped = 0;
    if (_ZNK7PathPtr5LoopsEv(c + 0x320)) {
        if (*(int *)(c + 0x32c) > 0) { if (delta < 0) delta = _ZNK7PathPtr8NumNodesEv(c + 0x320) - 1; }
        else { if (delta >= _ZNK7PathPtr8NumNodesEv(c + 0x320)) delta = 0; }
        _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x320, &node, delta);
    } else {
        if (delta < 0 || delta >= _ZNK7PathPtr8NumNodesEv(c + 0x320)) {
            node.x = *(int *)(c + 0x5c); node.y = *(int *)(c + 0x60); node.z = *(int *)(c + 0x64);
        } else _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x320, &node, delta);
    }
    _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x320, &prev, *(int *)(c + 0x328));
    Vec3_Sub(&diff, c + 0x5c, &prev);
    len = LenVec3(&diff);
    if (len == 0 || len <= *(int *)(c + 0x98)) {
        Vec3_MulScalar(&scaled1, &diff, _ZN4cstd4fdivEii(*(int *)(c + 0x98), len));
        SubVec3(c + 0x5c, &scaled1, c + 0x5c);
        looped = 1;
    } else {
        Vec3_MulScalar(&scaled2, &diff, _ZN4cstd4fdivEii(*(int *)(c + 0x98), len));
        SubVec3(c + 0x5c, &scaled2, c + 0x5c);
    }
    if (looped) {
        *(int *)(((int)c + 0x328) & 0xFFFFFFFFFFFFFFFF) += *(int *)(c + 0x32c);
        if (*(int *)(c + 0x328) < 0) {
            if (_ZNK7PathPtr5LoopsEv(c + 0x320)) {
                *(int *)(c + 0x328) = _ZNK7PathPtr8NumNodesEv(c + 0x320) - 1;
            } else {
                *(int *)(c + 0x32c) = 1;
                *(int *)(((unsigned)c + 0x328) & 0xFFFFFFFFFFFFFFFF) += *(int *)(c + 0x32c) * 2;
            }
        }
        if (*(int *)(c + 0x328) >= _ZNK7PathPtr8NumNodesEv(c + 0x320)) {
            if (_ZNK7PathPtr5LoopsEv(c + 0x320)) {
                *(int *)(c + 0x328) = 0;
            } else {
                *(int *)(c + 0x32c) = -1;
                *(int *)(((long long)(int)(c + 0x328)) & 0xFFFFFFFFFFFFFFFFLL) += *(int *)(c + 0x32c) * 2;
            }
        }
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x320000, 0);
    return 1;
}
