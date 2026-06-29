/* func_ov006_0210e120 at 0x0210e120
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
typedef struct { int x, y; } Vec2;
extern void func_ov006_0211470c(int *a, int *b);
extern int func_0203d614(Vec2 *v);
int func_ov006_0210e120(int *self)
{
    int i;
    Vec2 delta, pos;
    for (i = 0; i < *(int *)((char *)self[1] + 0x4668); i++) {
        int *o  = (i >= 13) ? 0 : ((int **)((char *)self[1] + 0x4688))[i];
        if (*(unsigned char *)((char *)o + 0x30) != 0) {
            int *o2 = (i >= 13) ? 0 : ((int **)((char *)self[1] + 0x4688))[i];
            func_ov006_0211470c((int *)&pos, o2);
            delta = pos;
            delta.x -= self[2];
            delta.y -= self[3];
            if (func_0203d614(&delta) < 0x12000)
                return 0;
        }
    }
    return 1;
}
