typedef short s16;

struct AnimRec { void* f0; void* file; };

extern struct AnimRec data_ov002_0210de30;
extern struct AnimRec data_ov002_0210de58;
extern struct AnimRec data_ov002_0210de18;
extern struct AnimRec* data_ov002_020ff0a0[];
extern struct AnimRec* data_ov002_020ff0b8[];

extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* file, int a, int b, unsigned c);

int func_ov002_020b7330(char* self)
{
    int state;

    *(int*)(self + 0x3ec) = 0;
    *(unsigned char*)(self + 0x3fe) = 0;

    state = *(int*)(self + 0x3f0);
    if (state == 0xa || state == 0xf) {
        char* closest = (char*)_ZN5Actor13ClosestPlayerEv(self);
        if (closest != 0) {
            s16* src = (s16*)((long long)(int)(closest + 0x8c));
            *(s16*)(self + 0x92) = src[0];
            *(s16*)(self + 0x94) = src[1];
            *(s16*)(self + 0x96) = src[2];
            *(s16*)(self + 0x8c) = *(s16*)(self + 0x92);
            *(s16*)(self + 0x8e) = *(s16*)(self + 0x94);
            *(s16*)(self + 0x90) = *(s16*)(self + 0x96);
            *(int*)(self + 0x3c0) = (int)closest;
        }
    }

    switch (*(int*)(self + 0x3f0)) {
    case 0xa:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            self + 0x300, data_ov002_0210de30.file, 0x40000000, 0x1000, 0);
        break;
    case 0xf:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            self + 0x300, data_ov002_020ff0a0[*(int*)(self + 0x3f4)]->file, 0x40000000, 0x1000, 0);
        break;
    case 0x14:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            self + 0x300, data_ov002_020ff0b8[*(int*)(self + 0x3f4)]->file, 0x40000000, 0x1000, 0);
        break;
    case 0x15:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            self + 0x300, data_ov002_0210de58.file, 0x40000000, 0x1000, 0);
        break;
    case 0x16:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            self + 0x300, data_ov002_0210de18.file, 0x40000000, 0x1000, 0);
        break;
    }

    return 1;
}
