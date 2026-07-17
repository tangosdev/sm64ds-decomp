//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;

typedef struct { int x, y, z; } Vector3;
typedef struct { short x, y, z; } Vector3_16;

extern "C" {
extern int _ZN6Player6IsAnimEj(void *self, unsigned int id);
extern int _ZNK6Player14GetBodyModelIDEjb(void *self, unsigned int a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void *anim, int frame);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, Vector3 *v);
extern int _ZN6Player12FinishedAnimEv(void *self);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *self, unsigned int anim, int a, int b, unsigned int d);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, Vector3 *v, Vector3_16 *v2, int e, int f);
extern void func_ov089_0213115c(void *actor, int i);

extern u8 data_0209f20c;
extern u8 data_0209f2d8;
extern char data_0209caa0[];

void func_ov002_020c7350(char *c)
{
    int id;
    char *anim;

    if (*(u8 *)(c + 0x6de) != 0)
        return;

    if (_ZN6Player6IsAnimEj(c, 0x10)) {
        id = _ZNK6Player14GetBodyModelIDEjb(c, *(int *)(c + 8) & 0xff, 0);
        anim = (char *)((int *)(c + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x52)) {
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(c + 0x6d9), 0x27, (Vector3 *)(c + 0x74));
        }
    }

    if (_ZN6Player6IsAnimEj(c, 0x85)) {
        id = _ZNK6Player14GetBodyModelIDEjb(c, *(int *)(c + 8) & 0xff, 0);
        anim = (char *)((int *)(c + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x20)) {
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(c + 0x6d9), 0x35, (Vector3 *)(c + 0x74));
        }
    }

    if (_ZN6Player6IsAnimEj(c, 0xa5)) {
        id = _ZNK6Player14GetBodyModelIDEjb(c, *(int *)(c + 8) & 0xff, 0);
        anim = (char *)((int *)(c + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x23)) {
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(c + 0x6d9), 0x37, (Vector3 *)(c + 0x74));
        }
    }

    id = _ZNK6Player14GetBodyModelIDEjb(c, *(int *)(c + 8) & 0xff, 0);
    anim = (char *)((int *)(c + 0xdc))[id] + 0x50;
    if (_ZNK9Animation12WillHitFrameEi(anim, 0x8e)) {
        if (_ZN6Player6IsAnimEj(c, 0xa5)) {
            data_0209f20c = 1;
        }
    }

    if (!_ZN6Player12FinishedAnimEv(c))
        return;

    {
        u8 st = *(u8 *)(c + 0x6e3);
        if (st == 0x10 || st == 0x12) {
            void *actor;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x94, 0x40000000, 0x1000, 0);
            actor = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x11a, *(s8 *)(c + 0x719), (Vector3 *)(c + 0x5c), (Vector3_16 *)(c + 0x8c), *(s8 *)(c + 0xcc), -1);
            if (*(int *)(c + 8) == 2) {
                func_ov089_0213115c(actor, 4);
            } else {
                func_ov089_0213115c(actor, 1);
            }
            *(u8 *)(c + 0x6e5) = 6;
            return;
        }
    }

    if (_ZN6Player6IsAnimEj(c, 0x85)) {
        data_0209f20c = 1;
    }

    {
        u8 v = data_0209f2d8;
        int isOne = (v == 1);
        if (isOne != false)
            goto setanim2;
        if (!(*(int *)(data_0209caa0 + 8) & 0x80))
            goto skip_setanim2;
        {
            int isTwo = (v == 2);
            if (isTwo != false)
                goto skip_setanim2;
        }
    setanim2:
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
    skip_setanim2:
        ;
    }

    *(u8 *)(c + 0x6e5) = 2;
}
}
