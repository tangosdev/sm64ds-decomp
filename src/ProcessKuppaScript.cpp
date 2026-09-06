//cpp
#include "types.h"
extern "C" {
    short ReadUnalignedShort(unsigned char* p);
    unsigned int ReadUnalignedInt(unsigned char* p);
    unsigned int func_02012790(unsigned int a);
    void func_0201f32c(int a);
    void LoadLevelNoReturn(u32 levelID, u32 arg1, u32 arg2, u32 arg3);
    int func_02008550(void* obj, unsigned char* msg, int a2, int a3);
    int func_0200e5ac(void* c, int r1, int r2);
    int func_0200e494(void* c, int r1, int r2);
    void EndKuppaScript(void);

    extern unsigned char* data_0209fc48;
    extern u16 data_0209b274;
    extern u8 data_020a0e40;
    extern u8 data_020a0e58[];
    extern u8 data_020a0de8[];
    extern u8 data_0209d66c;
    extern u32 data_0209fc4c;
    extern u8 data_0209f2d8;
    extern s8 data_0209d45c;
    extern u32 data_ov002_02110aec;
    extern void* data_0209f318;
    extern s8 data_02092110;
}
struct FaderBrightness {
    Fix12i currInterp;
    Fix12i speed;
    /* THE DESTRUCTOR PAIR, SPELLED AS TWO PLAIN VIRTUALS ON PURPOSE. mwccarm
       gives `virtual ~FaderBrightness()` TWO vtable entries -- D1 complete and
       D0 deleting, the Itanium pair -- and MSVC folds them into ONE. Spelt as a
       destructor this declaration indexed correctly on the ARM and ONE SLOT
       EARLY on the host, so every dispatch below reached its ROM neighbour:
       SetForwardTime ran the ROM's SetBackwardTime at +0x0c, SetToStart ran
       SetToEnd at +0x20, SetBackwardTime ran AdvanceFade at +0x08. Two ordinary
       virtuals occupy the SAME two entries under mwccarm, so the ROM bytes are
       untouched (2004/b56, 1.2/base, 1.2/sp2 and 1.2/sp2p3 all still match),
       and they occupy two under MSVC as well, so the host lands where the ARM
       does. Neither is ever called from here; they hold the two slots the ROM's
       table holds, and the UnkVis struct below already takes this shape for the
       same reason. port/stage_lifecycle_map.txt section 16 is the measurement. */
    virtual void Destructor1();
    virtual void Destructor0();
    virtual int Advance();
    virtual int SetBackwardTime(unsigned frames, unsigned arg2);
    virtual int SetForwardTime(unsigned frames, unsigned arg2);
    virtual int IsAtStart();
    virtual int IsAtEnd();
    virtual void Virtual1C();
    virtual void SetToEnd();
    virtual void SetToStart();
    u16 fieldC;
};
extern "C" FaderBrightness data_0209b294;
extern "C" FaderBrightness* data_0209d4b0;

struct UnkVis {
    virtual void u0();
    virtual void u1();
    virtual void u2();
    virtual void u3();
    virtual void u4();
    virtual void u5();
    virtual void u6();
    virtual int u7();
};
extern "C" UnkVis* data_0209f5bc;

class Scene {
public:
    static void StartSceneFade(unsigned int a, unsigned int b, unsigned short c);
    static void SetAndStopColorFader();
    static void SetFaders(FaderBrightness* f);
};
class Sound {
public:
    static void StopLoadedMusic_Layer1(unsigned int a);
    static void LoadAndSetMusic_Layer1(int a);
    static void Play2D(unsigned int a, unsigned int b);
};
class SaveData {
public:
    static int IsCharacterUnlocked(unsigned int a);
};

#pragma opt_common_subs off

extern "C" void ProcessKuppaScript(void)
{
    unsigned char* s = data_0209fc48;
    int next;
    int fin;
    if (s == 0)
        return;
    next = 0;
    fin = 1;
    while (*s != 0) {
        int beg = ReadUnalignedShort(s + 2);
        int end = ReadUnalignedShort(s + 4);
        u16 t = data_0209b274;
        if (beg > t) {
            fin = 0;
        } else if (end < 0 || end >= t) {
            u8 cmd = s[1];
            if (cmd == 5) {
                if (data_0209f5bc->u7() == 0) {
                    u32 i = data_020a0e40;
                    u32 off = i * 4;
                    if (*(u16*)(data_020a0e58 + i * 4 + 2) != 0 ||
                        ((data_020a0de8[off] != 0 && *(data_020a0de8 + off + 1) != 0) ? 1 : 0)) {
                        Scene::StartSceneFade(1, 0, 0);
                        Sound::StopLoadedMusic_Layer1(0x3c);
                    }
                }
                fin = 0;
            } else if (cmd == 6) {
                next = ReadUnalignedInt(s + 6);
            } else if (cmd == 7) {
                Sound::LoadAndSetMusic_Layer1(ReadUnalignedInt(s + 6));
            } else if (cmd == 8) {
                Sound::Play2D(1, ReadUnalignedInt(s + 6));
            } else if (cmd == 9) {
                func_02012790(ReadUnalignedInt(s + 6));
            } else if (cmd == 0xa) {
                if (beg == t) {
                    beg = ReadUnalignedShort(s + 6);
                    if (beg == 0xd && SaveData::IsCharacterUnlocked(1) == 0 && SaveData::IsCharacterUnlocked(2) == 0)
                        beg = 0x16;
                    func_0201f32c(beg);
                } else if (end == t) {
                    data_0209d66c = 1;
                }
            } else if (cmd == 0xb) {
                LoadLevelNoReturn((s8)s[6], s[7], (s8)s[8], 0);
                data_0209fc4c = ReadUnalignedInt(s + 9);
                if (((data_0209f2d8 == 2) ? 1 : 0) == 0)
                    Sound::StopLoadedMusic_Layer1(0x3c);
                Scene::SetAndStopColorFader();
                data_0209d4b0 = 0;
            } else if (cmd == 0xc) {
                FaderBrightness* f = &data_0209b294;
                f->SetToStart();
                f->fieldC = 0x7fff;
                f->SetForwardTime(0xe, 0);
                data_0209d4b0 = f;
            } else if (cmd == 0xd) {
                FaderBrightness* f = &data_0209b294;
                f->SetToEnd();
                f->fieldC = 0x7fff;
                f->SetBackwardTime(0x50, 0);
                data_0209d4b0 = f;
            } else if (cmd == 0xe) {
                FaderBrightness* f = &data_0209b294;
                f->SetToStart();
                f->fieldC = 0;
                f->SetForwardTime(0x1e, 0);
                data_0209d4b0 = f;
            } else if (cmd == 0xf) {
                FaderBrightness* f = &data_0209b294;
                data_0209d45c = 0x10;
                f->SetToEnd();
                f->fieldC = 0;
                f->SetBackwardTime(0x50, 0);
                data_0209d4b0 = f;
            } else if (cmd == 0x10) {
                FaderBrightness* f = &data_0209b294;
                f->SetToEnd();
                f->fieldC = 0;
                f->SetForwardTime(0x1e, 0);
                Scene::SetFaders(f);
            } else if (cmd == 0x11) {
                data_ov002_02110aec = 0;
            } else if (cmd == 4) {
                func_02008550(data_0209f318, s, beg, end);
            } else if (cmd <= 3) {
                func_0200e5ac(s, beg, end);
            } else if (cmd >= 0x12) {
                func_0200e494(s, beg, end);
            }
            if (end >= 0)
                fin = 0;
        }
        s += *s;
    }
    if (next != 0) {
        data_0209fc48 = (unsigned char*)next;
        data_0209b274 = 0;
        return;
    }
    if (fin != 0 && data_02092110 < 0) {
        EndKuppaScript();
        return;
    }
    data_0209b274++;
    if (data_0209b274 == 0)
        data_0209b274--;
}
