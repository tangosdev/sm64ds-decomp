//cpp
extern "C" {
int func_ov014_02111fb8(char* c);
char* _ZN5Actor15FindWithActorIDEjPS_(unsigned int a, void* b);
void func_ov014_02111f08(char* c);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* cc);
void func_ov014_02112114(char* c);
void func_ov014_02111fe0(char* c);
void func_ov014_0211250c(char* c);
void func_ov014_0211236c(char* c);
void func_ov014_021122dc(char* c);
void func_ov014_02112788(char* c);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* self, void* v);
void _ZN12CylinderClsn5ClearEv(void* self);
char* _ZN5Actor13ClosestPlayerEv(char* self);
void _ZN12CylinderClsn6UpdateEv(void* self);
extern int data_ov014_02114700[];

int _ZN10ChainChomp8BehaviorEv(char* c){
    *(unsigned char*)(c + 0x61c) = 0;
    {
        int v = *(int*)(c + 0x5f0) + 0xc8000;
        if (*(int*)(c + 0x60) <= v) {
            *(int*)(c + 0x60) = v;
            if (*(unsigned char*)(c + 0x61d) == 0) {
                func_ov014_02111fb8(c);
            }
            *(unsigned char*)(c + 0x61c) = 1;
        }
    }
    *(unsigned char*)(c + 0x61d) = *(unsigned char*)(c + 0x61c);
    if (*(int*)(c + 0x60c) == 0) {
        char* r = _ZN5Actor15FindWithActorIDEjPS_(0x29, 0);
        *(int*)(c + 0x60c) = *(int*)(r + 4);
    }
    func_ov014_02111f08(c);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
    func_ov014_02112114(c);
    if (*(unsigned char*)(c + 0x605) == 0) {
        func_ov014_02111fe0(c);
    }
    func_ov014_0211250c(c);
    if (*(unsigned char*)(c + 0x605) == 0) {
        func_ov014_0211236c(c);
        func_ov014_021122dc(c);
    }
    func_ov014_02112788(c);
    {
        int v[3];
        v[0] = data_ov014_02114700[0];
        v[1] = data_ov014_02114700[1];
        v[2] = data_ov014_02114700[2];
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x110, v);
    }
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    if (*(unsigned char*)(_ZN5Actor13ClosestPlayerEv(c) + 0x6fb) == 0) {
        _ZN12CylinderClsn6UpdateEv(c + 0x110);
    }
    return 1;
}
}
