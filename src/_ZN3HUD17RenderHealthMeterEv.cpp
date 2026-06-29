//cpp
// _ZN3HUD17RenderHealthMeterEv at 0x020fcfec
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
struct OamAttr;

extern "C" {

extern unsigned char data_0209f250[];
extern void* data_0209f394[];
extern unsigned char data_ov002_0211117c[];

extern struct OamAttr* data_ov002_0210c254[];
extern struct OamAttr* data_ov002_0210c278[];
extern struct OamAttr* data_ov002_0210c230[];
extern int data_ov002_0210c29c[];

extern struct OamAttr data_ov002_0210d5d0;
extern struct OamAttr data_ov002_0210d4f0;
extern struct OamAttr data_ov002_0210c690;

extern short data_ov002_0210c310[];

extern int _ZN6Player16IsInsideOfCannonEv(void* thisptr);
extern int GetOwnerLanguage(void);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    int a, struct OamAttr* attr, int x, int y, int e, int f, int g, int h, int i, int j);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void* p, unsigned int a, unsigned int b);

void _ZN3HUD17RenderHealthMeterEv(void* thisptr)
{
    if (_ZN6Player16IsInsideOfCannonEv(data_0209f394[data_0209f250[0]])) return;

    if (GetOwnerLanguage() == 5) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, data_ov002_0210c254[data_ov002_0211117c[0]], 0x80, *(short*)((char*)thisptr + 0x68), -1, 1, 0x1000, 0x1000, 0, -1);
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, &data_ov002_0210d5d0, 0x80, *(short*)((char*)thisptr + 0x68), -1, 1, 0x1000, 0x1000, 0, -1);
    } else if (GetOwnerLanguage() == 4 || GetOwnerLanguage() == 2) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, data_ov002_0210c278[data_ov002_0211117c[0]], 0x80, *(short*)((char*)thisptr + 0x68), -1, 1, 0x1000, 0x1000, 0, -1);
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, &data_ov002_0210d4f0, 0x80, *(short*)((char*)thisptr + 0x68), -1, 1, 0x1000, 0x1000, 0, -1);
    } else {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, data_ov002_0210c230[data_ov002_0211117c[0]], 0x80, *(short*)((char*)thisptr + 0x68), -1, 1, 0x1000, 0x1000, 0, -1);
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, &data_ov002_0210c690, 0x80, *(short*)((char*)thisptr + 0x68), -1, 1, 0x1000, 0x1000, 0, -1);
    }

    _ZN2GX11LoadOBJPlttEPKvjj(
        &data_ov002_0210c310[data_ov002_0210c29c[data_ov002_0211117c[0]]], 0x60, 0x20);
}

}
