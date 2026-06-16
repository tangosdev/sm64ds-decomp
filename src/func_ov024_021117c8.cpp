//cpp
extern "C" {
int _ZN5Actor15FindWithActorIDEjPS_(unsigned int, void*);
void _ZN9ActorBase18MarkForDestructionEv(void*);
void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void*, void*, int, int, unsigned int, unsigned int);
int func_ov024_021117c8(char* self){
    int a = _ZN5Actor15FindWithActorIDEjPS_(0x55, 0);
    if(a == 0){ _ZN9ActorBase18MarkForDestructionEv(self); return 1; }
    *(int*)(self+0x108) = *(int*)(a+4);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(self+0xd4, self, 0x7d000, 0x28000, 2, 0x400000);
    return 1;
}
}
