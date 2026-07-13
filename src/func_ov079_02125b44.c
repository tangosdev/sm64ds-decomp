typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;

extern int _ZN9Animation8FinishedEv(void* self);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int frame, int rate, unsigned int flags);
extern void _ZN9Animation7AdvanceEv(void* self);
extern int _ZN6Player9StartTalkER9ActorBaseb(void* self, void* actor, int b);
extern int _Z14ApproachLinearRsss(s16* p, s16 a, s16 b);
extern int _ZN6Player12GetTalkStateEv(void* self);
extern s16 Vec3_HorzAngle(const void* a, const void* b);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* self, void* actor, unsigned int msg, const void* v, unsigned int d, unsigned int e);
extern void _ZN7Message11PrepareTalkEv(void);
extern void func_0201267c(unsigned int id, void* p);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned int a);
extern void func_02011d38(void);
extern void _ZN7Message7EndTalkEv(void);
extern void func_ov079_02125504(void* c);

extern void* data_ov079_021275ec[];
extern s16 data_02082214[];
extern u8 data_0209d660;

void func_ov079_02125b44(char* c)
{
    int pos[3];

    if (_ZN9Animation8FinishedEv(c + 0x31c) != 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            c + 0x2cc,
            *(void**)((char*)data_ov079_021275ec[*(u8*)(c + 0x414) * 5 + 3] + 4),
            0, 0x1000, 0);
    }

    if (*(u8*)(c + 0x414) == 0)
        goto variant0;

    _ZN9Animation7AdvanceEv(c + 0x31c);
    *(int*)(c + 0x328) = 0x1000;

    switch (*(u8*)(c + 0x40c)) {
    case 0:
        if (*(int*)(c + 0x3f4) < 0x190000) {
            void* p = *(void**)(c + (*(int*)(c + 0x3ec) << 2) + 0x39c);
            if (*(int*)((char*)p + 0x60) >= *(int*)(c + 0x60) - 0xa000) {
                if (_ZN6Player9StartTalkER9ActorBaseb(p, c, 1) != 0) {
                    u8 *st = (u8 *)(((int)c + 0x40c) & 0xFFFFFFFFFFFFFFFFLL);
                    (*st)++;
                }
                break;
            }
        }
        *(u8*)(c + 0x401) = 3;
        break;
    case 1:
        if (_Z14ApproachLinearRsss((s16*)(c + 0x94), *(s16*)(c + 0x300 + 0xb8), 0x800) != 0) {
            u8 *st = (u8 *)(((int)c + 0x40c) & 0xFFFFFFFFFFFFFFFFLL);
            (*st)++;
        }
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        break;
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(void**)(c + (*(int*)(c + 0x3ec) << 2) + 0x39c)) == 0) {
            int *src;
            u16 ang;
            int i;
            int y;
            {
                void *p = *(void**)(c + (*(int*)(c + 0x3ec) << 2) + 0x39c);
                int *src = (int*)(((int)p + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
                pos[0] = src[0];
                pos[1] = src[1];
                pos[2] = src[2];
            }
            ang = Vec3_HorzAngle(pos, c + 0x5c);
            i = (u16)ang >> 4;
            y = pos[1] + 0x96000;
            pos[0] = data_02082214[i * 2] * 0xd7 + pos[0];
            pos[2] = data_02082214[i * 2 + 1] * 0xd7 + pos[2];
            pos[1] = y;
            if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(
                    *(void**)(c + (*(int*)(c + 0x3ec) << 2) + 0x39c),
                    c, 0xa4, pos, 0, 0) != 0) {
                u8 *st = (u8 *)(((int)c + 0x40c) & 0xFFFFFFFFFFFFFFFFLL);
                (*st)++;
                _ZN7Message11PrepareTalkEv();
                func_0201267c(0x133, c + 0x74);
            }
        }
        break;
    case 3:
        if (data_0209d660 != 0) {
            u8 *st = (u8 *)(((int)c + 0x40c) & 0xFFFFFFFFFFFFFFFFLL);
            (*st)++;
        }
        break;
    case 4:
        if (data_0209d660 == 0) {
            _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
            func_02011d38();
            _ZN7Message7EndTalkEv();
            {
                u8 *st = (u8 *)(((int)c + 0x40c) & 0xFFFFFFFFFFFFFFFFLL);
                (*st)++;
            }
        }
        break;
    case 5:
        if (_ZN6Player12GetTalkStateEv(*(void**)(c + (*(int*)(c + 0x3ec) << 2) + 0x39c)) == -1) {
            u8 *st = (u8 *)(((int)c + 0x40c) & 0xFFFFFFFFFFFFFFFFLL);
            (*st)++;
        }
        break;
    case 6:
        *(int*)(c + 0x3b0) = 2;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            c + 0x2cc,
            *(void**)((char*)data_ov079_021275ec[*(u8*)(c + 0x414) * 5 + 3] + 4),
            0, 0x1000, 0);
        break;
    }
    func_ov079_02125504(c);
    return;

variant0:
    if (*(int*)(c + 0x3f4) < 0x3e8000) {
        *(int*)(c + 0x3b0) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            c + 0x2cc,
            *(void**)((char*)data_ov079_021275ec[*(u8*)(c + 0x414) * 5 + 3] + 4),
            0, 0x1000, 0);
    }
    _ZN9Animation7AdvanceEv(c + 0x31c);
}
