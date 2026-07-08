typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

struct State;
extern int _ZN6Player7IsStateERNS_5StateE(void* thiz, struct State* s);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* thiz, struct State* s);

extern struct State _ZN6Player11ST_DEAD_HITE;
extern struct State _ZN6Player11ST_DEAD_PITE;
extern struct State _ZN6Player13ST_NO_CONTROLE;

int _ZN6Player17SetNoControlStateEhih(char* thiz, int a, int b, u8 c)
{
    u8 flag;

    if (_ZN6Player7IsStateERNS_5StateE(thiz, &_ZN6Player11ST_DEAD_HITE) ||
        _ZN6Player7IsStateERNS_5StateE(thiz, &_ZN6Player11ST_DEAD_PITE)) return 0;

    flag = 0;
    if (a <= 3) {
        if (*(u8*)(thiz + 0x709) != 0) return 0;
        flag = c;
    } else {
        if (*(u8*)(thiz + 0x709) != 0) {
            if (c == 0) return 0;
        }
    }

    *(u8*)(thiz + 0x70a) = a;
    *(int*)(thiz + 0x688) = b;
    _ZN6Player11ChangeStateERNS_5StateE(thiz, &_ZN6Player13ST_NO_CONTROLE);
    *(u8*)(thiz + 0x6e6) = flag;
    return 1;
}
