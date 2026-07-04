//cpp
typedef short s16;
typedef unsigned short u16;
typedef signed char s8;

extern "C" {
void _ZN9Animation7AdvanceEv(void* self);
int _ZNK9Animation13GetFrameCountEv(void* self);
int _ZN9Animation8FinishedEv(void* self);
int _ZNK9Animation12WillHitFrameEi(void* self, int f);
void func_ov100_02145080(void* self, void* arg1);
void func_ov100_021453d8(void* self, void* pmf, void* arg1);
void ChangeArea(int areaID);
void func_02012694(int a, void* b);
void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void* pos);
void _ZN6Player17PlayMammaMiaSoundEv(void* self);
void _ZN6Camera14GoBehindPlayerEj(void* self, unsigned int a);
}

extern unsigned char data_ov100_02148704;
extern char data_ov100_021488a4;
extern char data_ov100_021488b4;
extern char data_0209f318;
extern char data_0209caa0;
extern char data_02092110;
extern unsigned char data_0209f250;

extern "C" int func_ov100_02144730(char* self, char* arg1)
{
    _ZN9Animation7AdvanceEv(self + 0x124);
    if (data_ov100_02148704 != 0) {
        int f = *(int*)(self + 0x12c);
        if ((unsigned)(f << 4) >> 16 > (u16)(_ZNK9Animation13GetFrameCountEv(self + 0x124) - 0x28))
            func_ov100_02145080(self, arg1);
    }

    if (_ZN9Animation8FinishedEv(self + 0x124) != 0) {
        int t;
        if (data_ov100_02148704 != 0)
            func_ov100_021453d8(self, &data_ov100_021488a4, arg1);
        else
            func_ov100_021453d8(self, &data_ov100_021488b4, arg1);
        if (*(int*)(self + 0x88) < 0)
            t = *(s16*)(self + 0x8c);
        else
            t = *(s16*)(self + 0x90);
        t = (s8)t;
        *(s8*)(arg1 + 0xcc) = t;
        ChangeArea(t);
        {
            int* p = (int*)(((long long)(int)(*(char**)&data_0209f318 + 0x154)) & 0xFFFFFFFFFFFFFFFFLL);
            *p &= ~0xc00;
        }
        func_02012694(*(int*)(self + 8) == 0x10 ? 7 : 5, self + 0x74);
    } else if (*(s8*)(self + 0x144) != 0) {
        if (_ZNK9Animation12WillHitFrameEi(self + 0x124, (u16)(_ZNK9Animation13GetFrameCountEv(self + 0x124) - 0x1c)) != 0) {
            int* p = (int*)(((long long)(int)(*(char**)&data_0209f318 + 0x154)) & 0xFFFFFFFFFFFFFFFFLL);
            *p &= ~0xc00;
            if (*(int*)(self + 8) != 0xd) {
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(&data_0209caa0 + 0x41), 0x21, self + 0x74);
            } else if (data_ov100_02148704 == 0) {
                _ZN6Player17PlayMammaMiaSoundEv(arg1);
            }
        }
    } else {
        if (_ZNK9Animation12WillHitFrameEi(self + 0x124, (u16)(_ZNK9Animation13GetFrameCountEv(self + 0x124) - 0x18)) != 0) {
            if (*(s8*)&data_02092110 >= 0)
                *(int*)(self + 0x130) = 0;
            _ZN6Camera14GoBehindPlayerEj(*(char**)&data_0209f318, data_0209f250);
        }
    }
    return 1;
}
