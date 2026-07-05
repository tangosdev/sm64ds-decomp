struct Vector3;
extern void func_ov002_020aefb8(void *c);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, struct Vector3 *v);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *c);
extern void func_ov002_020af3a8(void *c);
extern int func_ov002_020af218(void *c, int a);
extern void func_ov002_020aeee4(void *c);

void func_ov002_020af724(unsigned char *self)
{
    func_ov002_020aefb8(self);
    switch (*(int *)(self + 0x388)) {
    case 0:
        _ZN5Sound9PlayBank3EjRK7Vector3(0x69, (struct Vector3 *)(self + 0x74));
        *(int *)((int)self + 0x388 & 0xFFFFFFFFFFFFFFFF) += 1;
        break;
    case 1:
        if (_ZNK12WithMeshClsn10IsOnGroundEv((void *)((int)self + 0x144 & 0xFFFFFFFFFFFFFFFF)) != 0) {
            *(int *)((int)self + 0x128 & 0xFFFFFFFFFFFFFFFF) &= ~1;
            *(int *)((int)self + 0x388 & 0xFFFFFFFFFFFFFFFF) += 1;
        }
        break;
    case 2:
        func_ov002_020af3a8(self);
        break;
    }
    func_ov002_020af218(self, 0xbb8);
    func_ov002_020aeee4(self);
}
