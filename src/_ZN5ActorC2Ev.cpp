//cpp
typedef short s16;
typedef unsigned short u16;
typedef long long s64;

extern "C" {
extern int _ZTV12ActorDerived;
extern int _ZTV5Actor;
extern int FIRST_ACTOR_LIST_NODE;
extern s16* POS_AT_SPAWN;
extern s16* ROT_AT_SPAWN;
extern signed char data_0209b44c;
extern s16 data_0208e378;
extern int* ACTOR_SPAWN_INFO_TABLE_PTR;
extern unsigned char CURRENT_GAMEMODE;
void _ZN9ActorBaseC1Ev(void* self);
int func_0203b244(void* l, void* n);
void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(void* self, int a, int b, int c, int d);
}

extern "C" void* _ZN5ActorC2Ev(char* self)
{
    int* entry;
    int b;
    int r3;
    _ZN9ActorBaseC1Ev(self);
    *(void**)self = &_ZTV12ActorDerived;
    *(void**)self = &_ZTV5Actor;
    *(int*)(self + 0x50) = 0;
    *(int*)(self + 0x54) = 0;
    *(char**)(self + 0x58) = self;
    func_0203b244((void*)&FIRST_ACTOR_LIST_NODE, self + 0x50);
    {
        s16* p = POS_AT_SPAWN;
        if (p != 0) {
            *(int*)(self + 0x5c) = ((int*)p)[0];
            *(int*)(self + 0x60) = ((int*)p)[1];
            *(int*)(self + 0x64) = ((int*)p)[2];
        }
    }
    {
        s16* q = ROT_AT_SPAWN;
        if (q != 0) {
            *(s16*)(self + 0x8c) = q[0];
            *(s16*)(self + 0x8e) = q[1];
            *(s16*)(self + 0x90) = q[2];
            {
                s16* q2 = ROT_AT_SPAWN;
                *(s16*)(self + 0x92) = q2[0];
                *(s16*)(self + 0x94) = q2[1];
                *(s16*)(self + 0x96) = q2[2];
            }
        }
    }
    *(signed char*)(self + 0xcc) = data_0209b44c;
    *(s16*)(self + 0xce) = data_0208e378;
    entry = (int*)(((int**)ACTOR_SPAWN_INFO_TABLE_PTR)[*(u16*)(self + 0xc)]);
    *(int*)(self + 0xb0) = entry[2];
    b = (CURRENT_GAMEMODE == 2);
    if (b != 0)
        r3 = entry[5] + 0x7d0000;
    else
        r3 = entry[5];
    _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(self, entry[3], entry[4], r3, entry[6]);
    return self;
}
