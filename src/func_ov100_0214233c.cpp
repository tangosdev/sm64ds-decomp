//cpp
/* func_ov100_0214233c @ 0x0214233c (ov100, size 0x184)
 * Path-node advance: atan2 heading to current node, then a 3-axis sign-dot of
 * (node-prev) vs (node-pos). If the actor is not still approaching the node
 * (dot <= 0), copy node->prev, advance path index (wrap), fetch next node.
 * Returns -1 when the index wrapped to 0, else 1; 0 if still approaching.
 *
 * Codegen notes (mwccarm 1.2/sp2p3):
 *  - Long-lived values use 6q variable-identity mapping so birth-order colors
 *    match the ROM (A holds C8, B holds C7, C8 holds C6, ...).
 *  - Sign result s8/s6 names are swapped vs the values they hold so free-reg
 *    dest colors land on r1/r3 like the ROM; mul uses those names accordingly.
 *  - u64-launder materializes pidx between the first path-node load and store.
 */
typedef short s16;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };

extern "C" s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern "C" int _ZNK7PathPtr7GetNodeER7Vector3j(void *thiz, Vector3 *out, u32 idx);

extern "C" int func_ov100_0214233c(char *c)
{
    int px = *(int *)(c + 0x3e8);
    int ax = *(int *)(c + 0x5c);
    int prevx = *(int *)(c + 0x3dc);
    /* name A  = node.x - prev.x  (C8)  — high-priority web → r8 */
    int A = px - prevx;
    int pz = *(int *)(c + 0x3f0);
    int az = *(int *)(c + 0x64);
    int prevz = *(int *)(c + 0x3e4);
    int py = *(int *)(c + 0x3ec);
    int prevy = *(int *)(c + 0x3e0);
    /* name B  = node.y - prev.y  (C7) */
    int B = py - prevy;
    /* name C8 = node.z - prev.z  (C6) */
    int C8 = pz - prevz;
    int ay = *(int *)(c + 0x60);
    /* name C7 = node.y - pos.y   (Csl) */
    int C7 = py - ay;
    /* name C6 = node.x - pos.x   (true A) */
    int C6 = px - ax;
    /* name Csl= node.z - pos.z   (true B) */
    int Csl = pz - az;
    int sA, sB, s8, s7, s6, ssl;
    int dot;

    *(s16 *)(c + 0x3ba) = _ZN4cstd5atan2E5Fix12IiES1_(C6, Csl);

    /* s6/s8 dest names swapped vs the values they store (free-reg coloring). */
    if (A != 0) {
        if (A < 0)
            s6 = -1;
        else
            s6 = 1;
    } else {
        s6 = 0;
    }
    if (B != 0) {
        if (B < 0)
            s7 = -1;
        else
            s7 = 1;
    } else {
        s7 = 0;
    }
    if (C8 != 0) {
        if (C8 < 0)
            s8 = -1;
        else
            s8 = 1;
    } else {
        s8 = 0;
    }
    if (C6 != 0) {
        if (C6 < 0)
            sA = -1;
        else
            sA = 1;
    } else {
        sA = 0;
    }
    if (C7 != 0) {
        if (C7 < 0)
            ssl = -1;
        else
            ssl = 1;
    } else {
        ssl = 0;
    }
    if (Csl != 0) {
        if (Csl < 0)
            sB = -1;
        else
            sB = 1;
    } else {
        sB = 0;
    }

    /* s6=sign(C8), s8=sign(C6) after the dest-name swap above */
    dot = s6 * sA + s7 * ssl + s8 * sB;
    if (dot <= 0) {
        int v = *(int *)(c + 0x3e8);
        int *pidx = (int *)(int)(((long long)(int)(c + 0x3d8)) & 0xFFFFFFFFFFFFFFFFLL);
        *(int *)(c + 0x3dc) = v;
        v = *(int *)(c + 0x3ec);
        *(int *)(c + 0x3e0) = v;
        v = *(int *)(c + 0x3f0);
        *(int *)(c + 0x3e4) = v;
        *pidx = *pidx + 1;
        if (*(int *)(c + 0x3d8) >= *(int *)(c + 0x3d4)) {
            *(int *)(c + 0x3d8) = 0;
        }
        _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x3f4, (Vector3 *)(c + 0x3e8),
                                       *(int *)(c + 0x3d8));
        return (*(int *)(c + 0x3d8) == 0) ? -1 : 1;
    }
    return 0;
}
