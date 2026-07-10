/* func_ov006_0211e5cc — once no entry (stride 0x24, 16 of them) is active
 * with stage <= 2, trigger func_ov004_020b0aa0(0xc) and bump the one-shot
 * latch byte at 0x4c20 (skipped while the latch is set). */

typedef unsigned char u8;

extern void func_ov004_020b0aa0(int arg);

typedef struct {
    char _pad0[0x4677];
    u8 active;   /* +0x4677 */
    u8 stage;    /* +0x4678 */
} View;

typedef struct {
    char _pad0[0x4c20];
    u8 latch;    /* +0x4c20 */
} Work;

void func_ov006_0211e5cc(char* c)
{
    int found;
    int i;
    char* p;
    if (((Work*)c)->latch != 0)
        return;
    found = 0;
    for (i = 0, p = c; i < 0x10; i++, p += 0x24) {
        if (((View*)p)->active != 0) {
            if (((View*)p)->stage <= 2) {
                found++;
                break;
            }
        }
    }
    if (found != 0)
        return;
    func_ov004_020b0aa0(0xc);
    (*(u8*)((long long)(int)(c + 0x4c20) & 0xFFFFFFFFFFFFFFFFLL))++;
}
