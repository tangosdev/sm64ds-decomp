//cpp
struct Vector3 { int x, y, z; };
struct ActorBase;

extern "C" {
int WithMeshClsn_IsOnGround(void* self);
void func_ov078_02125c48(void* c, void* p);
void func_02012694(int a, int* t);
void func_ov078_02125c24(void* c, int a);
void func_0200fa8c(int* t, int a);
int Player_StartTalk(void* self, ActorBase& a, bool b);
void Sound_ChangeMusicVolume(unsigned int a, int fix);
int Player_ShowMessage(void* self, ActorBase& a, unsigned int b, const Vector3* v, unsigned int d, unsigned int e);

extern int data_ov078_021270ac;
extern int data_ov078_021270ec;
extern int data_ov078_0212705c;
extern unsigned char data_0209f220;
}

extern "C" int func_ov078_02124cf4(unsigned char* thiz)
{
    if (WithMeshClsn_IsOnGround(thiz + 0x110) == 0) goto done;
    *(int*)(thiz + 0x98) = 0;
    if ((int)(*(int*)(thiz + 0x4d8) - 0x28000) > *(int*)(thiz + 0x60)) {
        func_02012694(0x128, (int*)(thiz + 0x74));
        func_ov078_02125c48(thiz, &data_ov078_021270ac);
        goto done;
    }
    if (thiz[0x499] == 0) {
        func_02012694(0x128, (int*)(thiz + 0x74));
        func_02012694(0x12b, (int*)(thiz + 0x74));
        func_ov078_02125c24(thiz, 0x7d0000);
        func_0200fa8c((int*)thiz, 1);
        thiz[0x499] = 1;
        int* p = (int*)((unsigned long long)((int)thiz + 0x500) & 0xFFFFFFFFFFFFFFFFULL);
        *p = *p - 1;
    }
    if (*(int*)(thiz + 0x500) > 0) {
        func_ov078_02125c48(thiz, &data_ov078_021270ec);
        goto done;
    }
    unsigned char* other = *(unsigned char**)(thiz + 0x430);
    if (other == 0) goto done;
    if ((unsigned short)(*(unsigned short*)(other + 0x6ce) & 0x800) != 0) goto done;
    if (Player_StartTalk(other, *(ActorBase*)thiz, 1) == 0) goto done;

    short msg = 0;
    Sound_ChangeMusicVolume(0x14, 0x15666);
    if (data_0209f220 == 1) {
        msg += (short)(*(int*)(other + 8) + 0x9a);
    } else {
        msg = 0x95;
    }

    if (Player_ShowMessage(other, *(ActorBase*)thiz, (unsigned)(int)msg, (Vector3*)(thiz + 0x5c), 0, 0) == 0) goto done;
    func_02012694(0x12a, (int*)(thiz + 0x74));
    func_ov078_02125c48(thiz, &data_ov078_0212705c);
done:
    return 1;
}
