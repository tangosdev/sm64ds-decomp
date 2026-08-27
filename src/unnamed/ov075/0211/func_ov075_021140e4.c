extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
    void* self, void* f, int b, int d, int fix, unsigned short e);
extern int _ZN4cstd4fdivEii(int a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void* self, int f);
extern void _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
    void* m, void* v, void* a, int fix, void* b);
extern void func_0201251c(int a, int b, void* c, int d);
extern int data_ov075_0211d42c[];
extern int data_0209f43c[];
extern int data_0209b3ec[];

void func_ov075_021140e4(char* c){
    if(*(int*)(c+0x13c) >= *(int*)(c+0x140) / 2){
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            c, (void*)data_ov075_0211d42c[1], 4, 0, 0x1000, 0);
        *(int*)(c+0x114) = 5;
    }
    *(int*)(c+0x5c) = _ZN4cstd4fdivEii(*(int*)(c+0x13c), *(int*)(c+0x140)) + 0x1000;
    if(!_ZNK9Animation12WillHitFrameEi(c+0x50, 4)){
        if(_ZNK9Animation12WillHitFrameEi(c+0x50, 0x22) == 0) return;
    }
    int sp8[3];
    _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
        data_0209f43c, data_0209b3ec, c+0x118, 0, sp8);
    func_0201251c(0, 0x20, sp8, *(int*)(c+0x13c));
}
