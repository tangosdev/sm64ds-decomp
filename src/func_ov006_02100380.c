/* func_ov006_02100380 — for each of 16 active entries (stride 0x18 array at
 * 0x5330): every 8 ticks bump a stage byte; after 4 stages clear the entry. */

typedef unsigned short u16;
typedef unsigned char u8;

typedef struct {
    char _pad0[0x5330];
    u16 timer;   /* +0x5330 */
    char _pad1[2];
    u8 f4;       /* +0x5334 */
    u8 stage;    /* +0x5335 */
    u8 active;   /* +0x5336 */
} View;

void func_ov006_02100380(char* c)
{
    int i;
    for (i = 0; i < 16; i++, c += 0x18) {
        if (((View*)c)->active == 0)
            continue;
        (*(u16*)((long long)(int)(c + 0x5330) & 0xFFFFFFFFFFFFFFFFLL))++;
        if (((View*)c)->timer < 8)
            continue;
        (*(u8*)((long long)(int)(c + 0x5335) & 0xFFFFFFFFFFFFFFFFLL))++;
        if (((View*)c)->stage >= 4) {
            ((View*)c)->active = 0;
            ((View*)c)->stage = 0;
            ((View*)c)->f4 = 0;
        }
    }
}
