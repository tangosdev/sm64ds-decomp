typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

typedef struct Ent71c {
    int _0;      /* 0x00 */
    int pos;     /* 0x04 */
    int vel;     /* 0x08 */
    u16 timer;   /* 0x0c */
    u8 active;   /* 0x0e */
    u8 fE;       /* 0x0f */
    u8 phase;    /* 0x10 */
    u8 pad[3];
} Ent71c;

extern Ent71c data_ov006_02142044[16];

void func_ov006_020ea71c(void)
{
    int i;
    for (i = 0; i < 16; i++) {
        Ent71c *e = &data_ov006_02142044[i];
        if (e->active) {
            if (e->pos > 0xb8000)
                e->pos = 0xb8000;
            if (e->phase == 0) {
                if (e->timer != 0) {
                    *(u16 *)(((int)e + 0xc) & 0xFFFFFFFFFFFFFFFF) -= 1;
                    if ((s16)e->timer < 0)
                        e->timer = 0;
                    *(int *)(((int)e + 4) & 0xFFFFFFFFFFFFFFFF) += e->vel;
                    *(int *)(((int)e + 8) & 0xFFFFFFFFFFFFFFFF) += 0x100;
                } else {
                    e->timer = 0x40;
                    *(u8 *)(((int)e + 0x10) & 0xFFFFFFFFFFFFFFFF) += 1;
                }
            } else {
                if (e->timer != 0) {
                    *(u16 *)(((int)e + 0xc) & 0xFFFFFFFFFFFFFFFF) -= 1;
                    if ((s16)e->timer < 0)
                        e->timer = 0;
                } else {
                    e->timer = 0;
                    e->active = 0;
                    e->fE = 0;
                }
            }
        }
    }
}
