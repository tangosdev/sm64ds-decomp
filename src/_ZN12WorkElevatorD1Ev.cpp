//cpp
extern "C" {
extern void* func_ov075_0211478c;
extern void* _ZTV17ExclamationSwitch;
void func_0207328c(void*, int, int, void(*)(void*));
void _ZN18MovingMeshColliderD1Ev(void*);
void _ZN5ModelD1Ev(void*);
void _ZN5ActorD1Ev(void*);
void* _ZN12WorkElevatorD1Ev(char* self){
    *(void**)self = &func_ov075_0211478c;
    func_0207328c(self+0x520, 4, 0x1c8, _ZN18MovingMeshColliderD1Ev);
    func_0207328c(self+0x320, 4, 0x50, _ZN5ModelD1Ev);
    *(void**)self = &_ZTV17ExclamationSwitch;
    _ZN18MovingMeshColliderD1Ev(self+0x124);
    _ZN5ModelD1Ev(self+0xd4);
    _ZN5ActorD1Ev(self);
    return self;
}
}
