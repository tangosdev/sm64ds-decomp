typedef unsigned char u8;

struct V3 { int x, y, z; };

extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN9ActorBase18MarkForDestructionEv(char *self);
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);

int func_ov002_020f0f08(char *self)
{
    struct V3 pos;

    if (*(unsigned short *)(self + 0x14c) != 0) return 1;

    *(int *)((int)(self + 0xa8) & 0xFFFFFFFFFFFFFFFFLL) += *(int *)(self + 0x9c);
    if (*(int *)(self + 0xa8) < *(int *)(self + 0xa0))
        *(int *)(self + 0xa8) = *(int *)(self + 0xa0);
    *(int *)((int)(self + 0x60) & 0xFFFFFFFFFFFFFFFFLL) += *(int *)(self + 0xa8);

    switch (*(u8 *)(self + 0x14e)) {
    case 0:
        if (*(int *)(self + 0x60) < *(int *)(self + 0x140)) {
            *(int *)(self + 0x60) = *(int *)(self + 0x140);
            *(int *)(self + 0xa8) = 0xf000;
            (*(u8 *)((int)(self + 0x14e) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
        break;
    case 1:
        if (*(int *)(self + 0x60) < *(int *)(self + 0x140)) {
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, *(int *)(self + 0x5c), *(volatile int *)(self + 0x60), *(int *)(self + 0x64));
            _ZN9ActorBase18MarkForDestructionEv(self);
        }
        break;
    }

    pos.x = *(int *)(self + 0x5c);
    pos.y = *(int *)(self + 0x60);
    pos.z = *(int *)(self + 0x64);
    if (*(int *)(self + 0x138) != 0) {
        char *other = _ZN5Actor10FindWithIDEj(*(int *)(self + 0x138));
        if (other != 0) {
            struct V3 *op = (struct V3 *)((int)(other + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
            int oy;
            pos.x = op->x;
            pos.y = oy = op->y;
            pos.z = op->z;
            pos.y = oy + (*(int *)(self + 0x148) + (*(int *)(self + 0x60) - *(int *)(self + 0x140)));
        }
    }
    Matrix4x3_FromTranslation((void *)(self + 0xf0), pos.x >> 3, pos.y >> 3, (*(volatile int *)&pos.z) >> 3);
    return 1;
}
