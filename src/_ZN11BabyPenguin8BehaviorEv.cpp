//cpp
extern "C" {
int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(void* self, int d);
int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
void func_ov072_02120d04(void* c);
void* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void* prev);
void func_ov072_02121cdc(void* c);
void func_ov072_021210c4(void* c);
}

extern "C" int _ZN11BabyPenguin8BehaviorEv(char* c)
{
    if (_ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(c, 0x7d0000) && _ZNK12WithMeshClsn10IsOnGroundEv(c + 0x194)) {
        func_ov072_02120d04(c);
    } else {
        if (*(int*)(c + 0x364) == 0)
            *(void**)(c + 0x364) = _ZN5Actor15FindWithActorIDEjPS_(0x101, 0);
        *(short*)(c + 0x36c) = 0x384;
        func_ov072_02121cdc(c);
        func_ov072_021210c4(c);
    }
    return 1;
}
