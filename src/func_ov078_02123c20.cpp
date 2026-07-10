//cpp
extern "C" {
extern int* _ZN13MontyMoleRockD0Ev(int* t);
extern void func_ov078_02123864(char* r7);
extern void func_ov002_020db54c(int a, int b, int c, int d);
extern int _ZNK9Animation12WillHitFrameEi(void* self, int f);
extern void func_02012694(int a, char* b, int c);
extern int _ZN9Animation8FinishedEv(void* self);
extern void func_ov078_02125c48(char* c, void* p);
extern int data_ov078_021270fc[];

int func_ov078_02123c20(char* c){
    if((int)_ZN13MontyMoleRockD0Ev((int*)c) == 1){
        func_ov078_02123864(c);
        int v = *(int*)(c+0x494);
        if(v != 0){
            func_ov002_020db54c(v, 0, 0x50000, *(short*)(c+0x8e));
            *(int*)(c+0x430) = *(int*)(c+0x494);
            *(int*)(c+0x494) = 0;
        }
        return 1;
    }
    if(_ZNK9Animation12WillHitFrameEi(c+0x31c, 0x14)){
        int v = *(int*)(c+0x494);
        if(v != 0){
            func_ov002_020db54c(v, 0x28000, 0x50000, *(short*)(c+0x8e));
            *(int*)(c+0x430) = *(int*)(c+0x494);
            *(int*)(c+0x494) = 0;
            func_02012694(0x131, c+0x74, 0);
        }
    }
    if(_ZN9Animation8FinishedEv(c+0x31c)){
        func_ov078_02125c48(c, data_ov078_021270fc);
    }
    return 1;
}
}
