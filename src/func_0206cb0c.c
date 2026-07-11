typedef unsigned short u16;

extern int func_0206cf7c(int arg0);

#pragma optimize_for_size on
void func_0206cb0c(void *obj, int val) {
    volatile u16 *self = (volatile u16 *)obj;
    u16 idx = self[0];
    u16 next = (idx + 1) & 0x7fff;
    unsigned char *bank;
    if (func_0206cf7c(0x2000)) {
        bank = (unsigned char *)0x09fd0000;
    } else {
        bank = (unsigned char *)0x08fd0000;
    }
    if (next != self[2]) {
        u16 *p = (u16 *)(bank + (idx & ~1));
        u16 cur = *p;
        u16 out;
        if ((idx & 1) != 0) {
            out = (cur & 0x00ff) | ((unsigned int)(val << 16) >> 8);
        } else {
            out = (cur & 0xff00) | ((unsigned int)(val << 16) >> 16);
        }
        *p = out;
        while (self[0] != next) {
            self[0] = next;
        }
    }
}
