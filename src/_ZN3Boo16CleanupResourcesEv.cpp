//cpp
struct Actor;
extern "C" Actor *_ZN5Actor10FindWithIDEj(unsigned int id);
extern "C" void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern "C" void UnloadBlueCoinModel(void *o);
struct SharedFilePtr;
extern "C" void _ZN13SharedFilePtr7ReleaseEv(SharedFilePtr *self);
extern "C" void UnloadKeyModels(int i);
extern "C" void _ZN8CapEnemy14UnloadCapModelEv(void *self);

extern SharedFilePtr data_ov063_0211edec;
extern SharedFilePtr data_ov063_0211edc4;
extern SharedFilePtr data_ov063_0211eddc;
extern SharedFilePtr data_ov063_0211edf4;
extern SharedFilePtr data_ov063_0211ede4;
extern SharedFilePtr data_ov063_0211edd4;
extern SharedFilePtr data_ov063_0211edcc;

struct O {
    char pad0[0xc];
    unsigned short f0c;       // +0xc
    char pad1[0x48c - 0xe];
    void *f48c;               // +0x48c
    char pad2[0x494 - 0x490];
    unsigned int f494;        // +0x494
    char pad3[0x49c - 0x498];
    unsigned int f49c;        // +0x49c
    unsigned short f4a0;      // +0x4a0
    char pad5[0x5cf - 0x4a2];
    unsigned char f5cf;       // +0x5cf
};

extern "C" int _ZN3Boo16CleanupResourcesEv(O *self)
{
    int b;
    int *cnt;

    if (self->f49c != 0) {
        self->f48c = _ZN5Actor10FindWithIDEj(self->f49c);
        if (self->f48c != 0)
            _ZN9ActorBase18MarkForDestructionEv(self->f48c);
        self->f48c = 0;
    }
    if (self->f494 != 0) {
        self->f48c = _ZN5Actor10FindWithIDEj(self->f494);
        if (self->f48c != 0) {
            cnt = (int *)(((int)self->f48c + 0x5a0) & 0xFFFFFFFFFFFFFFFF);
            (*cnt)++;
        }
        self->f48c = 0;
    }
    if (self->f4a0 == 0x122)
        UnloadBlueCoinModel(self);
    else if (self->f4a0 == 0xd4)
        _ZN13SharedFilePtr7ReleaseEv(&data_ov063_0211edec);

    b = (self->f0c == 0xd1);
    if (b != 0) {
        _ZN13SharedFilePtr7ReleaseEv(&data_ov063_0211edc4);
        _ZN13SharedFilePtr7ReleaseEv(&data_ov063_0211eddc);
    } else {
        _ZN13SharedFilePtr7ReleaseEv(&data_ov063_0211edf4);
        _ZN13SharedFilePtr7ReleaseEv(&data_ov063_0211ede4);
        if (self->f5cf == 0xf) {
            UnloadKeyModels(3);
            _ZN13SharedFilePtr7ReleaseEv(&data_ov063_0211edd4);
            _ZN13SharedFilePtr7ReleaseEv(&data_ov063_0211edcc);
        }
    }
    _ZN8CapEnemy14UnloadCapModelEv(self);
    return 1;
}
