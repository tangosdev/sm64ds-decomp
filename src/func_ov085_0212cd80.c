// @symbol func_ov085_0212cd80
// recovered name: Rabbit_Kill
/* recovered: shared common types, renamed to Class_Method, declarations from a shared header */
#include "decl_Message.h"
#include "decl_Player.h"
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method */
/* daMip_c::Kill - recovered from vtable slot identity */
#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off)))))
extern int _Z14ApproachLinearRiii(int* ref, int target, int step);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int f, unsigned char g);
extern void func_02012790(int a);
extern void StartMinigameMenu(unsigned char a);
extern void _ZN7Message7EndTalkEv(void);
extern void _ZN9ActorBase18MarkForDestructionEv(void* self);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* self, void* actor, unsigned int msg, const void* pos, unsigned int a, unsigned int b);

extern unsigned char data_0209d684;
extern unsigned char data_0209d660;

int func_ov085_0212cd80(char* self)
{
    char* other = *(char**)(self + 0x18c);
    volatile struct Vector3 v;
    unsigned char gb;
    int* s;

    if (other == 0) return 1;

    s = (int*)AT(other, 0x5c);
    v.x = s[0];
    v.y = s[1];
    v.z = s[2];
    v.y = s[1] + 0xc8000;
    _Z14ApproachLinearRiii((int*)(self + 0x60), s[1] + 0xc8000, 0xa000);

    *(int*)(self + 0x5c) = v.x;
    *(int*)(self + 0x64) = v.z;
    gb = data_0209d684;

    switch (*(int*)(self + 0x194)) {
    case 0:
        if (!_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x28, 0x12, 0x7f, 0x15ccc, 0))
            break;
        *(int*)(self + 0x194) = 1;
    case 1:
        if (data_0209d660 == 0) {
            if (gb == 1) {
                func_02012790(0x5e);
                _ZN7Message13DisplaySavingEt(0x295);
                *(int*)(self + 0x194) = 2;
            } else if (gb == 2) {
                func_02012790(0x5e);
                _ZN7Message13DisplaySavingEt(0x295);
                *(int*)(self + 0x194) = 3;
            } else if (gb == 3) {
                func_02012790(0x98);
                if (*(int*)(self + 0x198)) {
                    *(int*)(self + 0x194) = 0xa;
                } else {
                    func_ov085_0212cd0c(self);
                }
            }
        }
        break;
    case 2:
        if (data_0209d660 == 0) {
            if (*(int*)(self + 0x198)) {
                *(int*)(self + 0x194) = 0xa;
            } else {
                func_ov085_0212cd0c(self);
            }
        }
        break;
    case 3:
        if (data_0209d660 == 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x28, 0x7f, 0, 0x7444, 0);
            StartMinigameMenu(1);
            _ZN7Message7EndTalkEv();
            _ZN9ActorBase18MarkForDestructionEv(self);
        }
        break;
    case 4:
        {
            int r;
            r = _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(*(void**)(self + 0x18c), self, 0x18b, 0, 1, 0);
            if (r == 1) {
                *(int*)(self + 0x198) = 1;
                *(int*)(self + 0x194) = 0;
            }
        }
        break;
    case 6:
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x28, 0x12, 0x7f, 0x15ccc, 0)) {
            if (data_0209d660 == 0) {
                if (gb == 1) {
                    func_02012790(0x5e);
                    _ZN7Message13DisplaySavingEt(0x295);
                    *(int*)(self + 0x194) = 2;
                } else if (gb == 2) {
                    func_02012790(0x98);
                    func_ov085_0212cd0c(self);
                }
            }
        }
        break;
    case 10:
        if (_ZN6Player18HasFinishedTalkingEv(*(void**)(self + 0x18c)) == 1) {
            func_ov085_0212cd0c(self);
        }
        break;
    }
    return 1;
}
