//cpp
// _ZN5Stage21RenderVsModeCountdownEv at 0x0202a168
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
struct Base {
    virtual void v0(); virtual void v1(); virtual void v2();
    virtual void v3(); virtual void v4(); virtual int m();
};
extern "C" {
extern unsigned char* data_0209f394[4];
extern Base* data_0209f5bc;
extern unsigned char data_0209fc50;
extern unsigned char data_0209f2bc;
extern unsigned short data_0209f304;
extern int data_ov002_0210d008;
extern int data_ov002_0210d098;
extern int data_ov002_0210d130;
extern int data_ov002_0210d1c8;
extern int data_ov002_0210d210;
extern void* data_ov002_0210ca0c[];
extern int data_ov002_0210d378;
extern int data_ov002_0210d3a0;
extern int data_ov002_0210d3c0;
extern int data_ov002_0210d3e0;
extern int data_ov002_0210d420;
int GetOwnerLanguage(void);
int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void*, int, int, int, int, int, int, int, int);
}

extern "C" void _ZN5Stage21RenderVsModeCountdownEv(void) {
    int count = 0;
    int i;
    for (i = 0; i < 4; i++) {
        if (data_0209f394[i]) {
            int t = (data_0209f394[i][0x711] != 0);
            if (t == 1) count++;
        }
    }
    if (data_0209f5bc->m() == 0) return;
    if (count < data_0209fc50) return;
    if (data_0209f2bc != 0) {
        if (GetOwnerLanguage() == 5)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210d008, 0x80, 0x30, -1,-1,0x1000,0x1000,0,-1);
        else if (GetOwnerLanguage() == 4)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210d098, 0x80, 0x30, -1,-1,0x1000,0x1000,0,-1);
        else if (GetOwnerLanguage() == 3)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210d130, 0x80, 0x30, -1,-1,0x1000,0x1000,0,-1);
        else if (GetOwnerLanguage() == 2)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210d1c8, 0x80, 0x30, -1,-1,0x1000,0x1000,0,-1);
        else
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210d210, 0x80, 0x30, -1,-1,0x1000,0x1000,0,-1);
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov002_0210ca0c[3 - data_0209f2bc], 0x80, 0x50, -1,-1,0x1000,0x1000,0,-1);
        return;
    }
    if (data_0209f304 == 0) return;
    if (GetOwnerLanguage() == 5)
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210d378, 0x80, 0x50, -1,-1,0x1000,0x1000,0,-1);
    else if (GetOwnerLanguage() == 4)
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210d3a0, 0x80, 0x50, -1,-1,0x1000,0x1000,0,-1);
    else if (GetOwnerLanguage() == 3)
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210d3c0, 0x80, 0x50, -1,-1,0x1000,0x1000,0,-1);
    else if (GetOwnerLanguage() == 2)
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210d3e0, 0x80, 0x50, -1,-1,0x1000,0x1000,0,-1);
    else
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210d420, 0x80, 0x50, -1,-1,0x1000,0x1000,0,-1);
}
