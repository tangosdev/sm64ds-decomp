//cpp
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

extern "C" {
    int GetOwnerLanguage(void);
    void* LoadFile(int handle);
    void DecompressLZ16(void* handle, int addr);
    void Deallocate(void* handle);
    u8 NumStars(void);
    void func_0203da4c(void);

    u8 data_0209f2d8;
    s8 data_0209f2f8;
    u8 data_0209f2fc;
    u8 data_0209f250;
    void* data_0209f394[];
    s8 data_ov002_02111178;
    u8 data_ov002_0211117c;
    s8 data_ov002_02111184;
    s16 data_ov002_02111188;
    struct CAA0 { char pad[8]; int unk8; };
    CAA0 data_0209caa0;
}

struct Player { u8 GetHealth(); };
namespace GX { void LoadOBJPltt(const void*, u32, u32); }
namespace GXS { void LoadOBJPltt(const void*, u32, u32); }

struct HUD {
    char pad0[0x60];
    s16 unk60;
    s16 unk62;
    s16 unk64;
    s16 unk66;
    s16 unk68;
    char pad6a[4];
    s16 unk6E;
    s16 unk70;
    u8 unk72;
    u8 unk73;
    char pad74[4];
    u8 unk78;
    int InitResources();
};

int HUD::InitResources()
{
    char* vram_a = (char*)0x6400000;
    char* vram_b = (char*)0x6600000;
    int cond1 = (data_0209f2d8 == 1);
    if (cond1 != 0) {
        void* h;
        if (GetOwnerLanguage() == 5) {
            h = LoadFile(0xB003);
            DecompressLZ16(h, 0x6400000);
            DecompressLZ16(h, 0x6600000);
            Deallocate(h);
            h = LoadFile(0xB007);
            DecompressLZ16(h, (int)(vram_a + 0x2000));
            Deallocate(h);
        } else if (GetOwnerLanguage() == 4) {
            h = LoadFile(0xAC03);
            DecompressLZ16(h, 0x6400000);
            DecompressLZ16(h, 0x6600000);
            Deallocate(h);
            h = LoadFile(0xAC07);
            DecompressLZ16(h, (int)(vram_a + 0x2000));
            Deallocate(h);
        } else if (GetOwnerLanguage() == 3) {
            h = LoadFile(0xA803);
            DecompressLZ16(h, 0x6400000);
            DecompressLZ16(h, 0x6600000);
            Deallocate(h);
            h = LoadFile(0xA807);
            DecompressLZ16(h, (int)(vram_a + 0x2000));
            Deallocate(h);
        } else if (GetOwnerLanguage() == 2) {
            h = LoadFile(0xA403);
            DecompressLZ16(h, 0x6400000);
            DecompressLZ16(h, 0x6600000);
            Deallocate(h);
            h = LoadFile(0xA407);
            DecompressLZ16(h, (int)(vram_a + 0x2000));
            Deallocate(h);
        } else {
            h = LoadFile(0xA003);
            DecompressLZ16(h, 0x6400000);
            DecompressLZ16(h, 0x6600000);
            Deallocate(h);
            h = LoadFile(0xA007);
            DecompressLZ16(h, (int)(vram_a + 0x2000));
            Deallocate(h);
        }
        h = LoadFile(0x8000);
        DecompressLZ16(h, (int)(vram_b + 0x2000));
        Deallocate(h);
        h = LoadFile(0x8002);
        GX::LoadOBJPltt(h, 0, 0x120);
        GXS::LoadOBJPltt(h, 0, 0x120);
        Deallocate(h);
        h = LoadFile(0x8003);
        GX::LoadOBJPltt(h, 0x120, 0xE0);
        Deallocate(h);
        h = LoadFile(0x8001);
        GXS::LoadOBJPltt(h, 0x120, 0xE0);
        Deallocate(h);
    } else {
        void* h;
        if (GetOwnerLanguage() == 5) h = LoadFile(0x270);
        else if (GetOwnerLanguage() == 4) h = LoadFile(0x26E);
        else if (GetOwnerLanguage() == 3) h = LoadFile(0x26C);
        else if (GetOwnerLanguage() == 2) h = LoadFile(0x26A);
        else h = LoadFile(0x269);
        DecompressLZ16(h, 0x6400000);
        DecompressLZ16(h, 0x6600000);
        Deallocate(h);
        if (GetOwnerLanguage() == 5) h = LoadFile(0xB00E);
        else if (GetOwnerLanguage() == 4) h = LoadFile(0xAC0E);
        else if (GetOwnerLanguage() == 3) h = LoadFile(0xA80E);
        else if (GetOwnerLanguage() == 2) h = LoadFile(0xA40E);
        else h = LoadFile(0xA00E);
        DecompressLZ16(h, (int)(vram_a + 0x2000));
        Deallocate(h);
        h = LoadFile(0x229);
        DecompressLZ16(h, (int)(vram_b + 0x2000));
        Deallocate(h);
        if (GetOwnerLanguage() == 5) h = LoadFile(0xB00A);
        else if (GetOwnerLanguage() == 4) h = LoadFile(0xAC0A);
        else if (GetOwnerLanguage() == 3) h = LoadFile(0xA80A);
        else if (GetOwnerLanguage() == 2) h = LoadFile(0xA40A);
        else h = LoadFile(0xA00A);
        GX::LoadOBJPltt(h, 0, 0x120);
        GXS::LoadOBJPltt(h, 0, 0x120);
        Deallocate(h);
        h = LoadFile(0x980F);
        GX::LoadOBJPltt(h, 0x120, 0xE0);
        Deallocate(h);
        h = LoadFile(0x22A);
        GXS::LoadOBJPltt(h, 0x120, 0xE0);
        Deallocate(h);
    }

    int var_r2 = 0;
    this->unk62 = 0;
    this->unk64 = 0xB4;
    this->unk66 = 0xA;
    if (data_0209f2d8 == 1) var_r2 = 1;
    if (var_r2 != 0) {
        if (data_0209f2f8 == 0x33) this->unk60 = 0x1E;
        else if (data_0209f2f8 == 0x2B) *(volatile s16*)&this->unk60 = 0x1E;
        else this->unk60 = 0x1E;
    } else {
        this->unk60 = 0x1E;
    }

    data_ov002_02111188 = 0xB4;
    data_ov002_02111184 = 0;
    {
        u8 idx = *(volatile u8*)&data_0209f250;
        data_ov002_0211117c = ((Player*)((idx ? (void* volatile*)data_0209f394 : (void* volatile*)data_0209f394)[idx]))->GetHealth();
    }
    if (data_ov002_0211117c == 8) {
        this->unk68 = -0x18;
        this->unk72 = 0;
        this->unk73 = 0;
    } else {
        this->unk68 = 0x18;
        this->unk73 = 1;
    }

    int var_r0_2 = 0;
    int var_r1;
    if (data_0209f2d8 == 1) var_r1 = 1; else var_r1 = 0;
    if (var_r1 == 0) {
        int n = NumStars();
        if (n > 0x63) var_r0_2 = 0x1B;
        else if (n > 9) var_r0_2 = 0x12;
        else var_r0_2 = 9;
    }

    int var_r1_2;
    if (data_0209f2d8 == 1) var_r1_2 = 1; else var_r1_2 = 0;
    if (var_r1_2 != 0) {
        data_ov002_02111178 = 0;
        this->unk6E = 0x10;
        this->unk70 = 0xF0;
        this->unk78 = 0;
    } else if (!(data_0209caa0.unk8 & 0x80)) {
        data_ov002_02111178 = 6;
        this->unk6E = -0x3A;
        this->unk70 = (s16)(var_r0_2 + 0x120);
        this->unk78 = 1;
    } else if (data_0209f2fc == 2) {
        data_ov002_02111178 = 1;
        this->unk6E = -0x3A;
        this->unk70 = 0xF0;
        this->unk78 = 0;
    } else if (data_0209f2fc == 1) {
        data_ov002_02111178 = 2;
        this->unk6E = -0x3A;
        this->unk70 = (s16)(var_r0_2 + 0x120);
        this->unk78 = 0;
    } else {
        data_ov002_02111178 = 0;
        this->unk6E = 0x10;
        this->unk70 = 0xF0;
        this->unk78 = 0;
    }

    func_0203da4c();
    return 1;
}
