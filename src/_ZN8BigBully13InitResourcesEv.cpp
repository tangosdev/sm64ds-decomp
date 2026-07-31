//cpp
extern "C" {
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed char s8;
typedef short s16;
typedef unsigned int u32;

typedef struct { int x, y, z; } Vector3;
typedef struct { short x, y, z; } Vector3_16;
typedef struct Actor Actor;
struct RaycastGround { char buf[0x50]; };

extern int func_ov064_02116ec0(void* obj);
extern int _ZN5Actor9TrackStarEjj(Actor* self, unsigned int a, unsigned int b);
extern void _ZN13RaycastGroundC1Ev(RaycastGround* self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, const Vector3* p, Actor* a);
extern int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
extern void _ZN13RaycastGroundD1Ev(RaycastGround* self);
extern Actor* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 actorID, u32 param1, const Vector3* pos, const Vector3_16* rot, s8 areaID, s16 deathTableID);
extern int data_ov064_0211b93c;
extern s16 data_02082214[];
}

struct BigBully {
    int InitResources();
};

int BigBully::InitResources()
{
    char* c = (char*)this;
int saved;

    *(void**)(c + 0x330) = &data_ov064_0211b93c;
    saved = func_ov064_02116ec0(c);
    *(u8*)(c + 0x3fc) = *(int*)(c + 8) & 0xf;
    *(u8*)(c + 0x3fd) = (u8)_ZN5Actor9TrackStarEjj((Actor*)c, *(u8*)(c + 0x3fc), 2);
    *(short*)(c + 0x3fa) = 0;

    if ((*(int*)(c + 8) & 0xff00) == 0x100) {
        RaycastGround rg;
        Vector3 pos;
        Vector3 v;
        int i;
        int ang;

        *(u8*)(c + 0x3fe) = 0;
        _ZN13RaycastGroundC1Ev(&rg);

        {
            int tz = *(int*)(c + 0x64);
            int ty = *(int*)(c + 0x60) + 0x32000;
            int tx = *(int*)(c + 0x5c);
            v.x = tx;
            v.y = ty;
            v.z = tz;
        }
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &v, (Actor*)c);

        if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0) {
            pos.y = *(int*)(rg.buf + 0x44);
        }

        i = 0;
        ang = 0;
        do {
            int idx = (u16)(s16)ang >> 4;
            Actor* spawned;

            pos.x = *(int*)(c + 0x5c) + data_02082214[idx * 2] * 500 - 0x64000;
            pos.z = *(int*)(c + 0x64) - data_02082214[idx * 2 + 1] * 500;

            spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xd7, -1, &pos, (Vector3_16*)0, *(s8*)(c + 0xcc), -1);
            if (spawned != 0) {
                *(int*)((char*)spawned + 0x3fc) = *(int*)(c + 4);
            } else {
                _ZN13RaycastGroundD1Ev(&rg);
                return 0;
            }

            i++;
            ang += 0x5555;
        } while (i < 3);

        _ZN13RaycastGroundD1Ev(&rg);
        goto done;
    }

    *(u8*)(c + 0x3fe) = 0xff;
done:
    return saved;

}
