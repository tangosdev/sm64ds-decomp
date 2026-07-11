//cpp
extern "C" int __aeabi_idivmod(int n, int d);
struct Animation { char pad4[4]; unsigned int f; int frame; int speed; };
extern "C" void _ZN9Animation7AdvanceEv(struct Animation* a);
void _ZN9Animation7AdvanceEv(struct Animation* a) {
    unsigned int f = a->f;
    unsigned int len = f & ~0xc0000000;
    if ((f & 0xc0000000) == 0) {
        a->frame = (a->frame + a->speed + (int)len) % (int)len;
    } else {
        int *pFrame = (int *)(((int)a + 8) & 0xFFFFFFFFFFFFFFFF);
        *pFrame = *pFrame + a->speed;
        if (a->frame < 0) {
            a->frame = 0;
        } else if (a->frame >= (int)len) {
            a->frame = (int)len - 1;
        }
    }
}
