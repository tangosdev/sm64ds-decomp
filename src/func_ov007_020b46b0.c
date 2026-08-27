typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

typedef struct { u32 f0; u32 f4; u32 f8; } Tbl;
extern Tbl data_ov007_020d77dc[];
extern char* data_ov007_0210342c;

extern int func_ov007_020b79e4(void);
extern int func_ov007_020c1da0(int i);
extern void func_ov007_020b63e4(char** a);
extern int func_ov007_020b8f78(void);
extern int func_ov007_020b8fa0(void);
extern void func_ov007_020c1d78(int i);

void func_ov007_020b46b0(void* arg0, void* arg1)
{
    char* c = (char*)arg0;
    char* b = (char*)arg1;
    int r5 = 0;

    {
        char* p = *(char**)c;
        int r4a = 0;
        int r2a = 0;
        int r1a = 0;
        if (*(*(s16**)(p + 4)) == 0) {
            if (*(s32*)(p + 0x10) >= 0x1000
                || (data_ov007_020d77dc[**(u16**)c].f0 & 2))
                r1a = 1;
        }
        if (r1a) {
            if (!(data_ov007_020d77dc[**(u16**)c].f0 & 1))
                r2a = 1;
        }
        if (r2a) {
            int sba = 1;
            if (func_ov007_020b79e4()) {
                int r8a = 0;
                if (func_ov007_020b79e4()) {
                    u16 id2 = **(u16**)c;
                    int x = sba;
                    if (id2 != 0x16 && id2 != 0x17) x = r8a;
                    if (x) r8a = 1;
                }
                if (!r8a) sba = 0;
            }
            if (sba) r4a = 1;
        }

        if (r4a && func_ov007_020c1da0(0) == 0) {
            int r2 = 0;
            int r1 = 0;
            int r4 = *(u16*)(b + 0xc);
            if (r4 != 0) {
                if (*(u16*)(b + 0x14) == 0)
                    r1 = 1;
            }
            if (r1) {
                if ((u32)*(s32*)(b + 0x24) >= 1U)
                    r2 = 1;
            }
            if (r2) {
                char* oo = *(char**)c;
                char* pp = *(char**)(c + 4);
                char* o28 = *(char**)(oo + 0x28);
                int ip = *(s32*)(pp + 8);
                int p4 = *(s32*)(pp + 4);
                int hi_x = *(s32*)(o28 + 0x10) + (ip >> 12);
                int lo_x = *(s32*)(o28 + 8) + (ip >> 12);
                int hi_z = *(s32*)(o28 + 0xc) + (p4 >> 12);
                int lo_z = *(s32*)(o28 + 4) + (p4 >> 12);
                if (r4 != 0
                    && (s32)*(u16*)(b + 8) >= lo_z && (s32)*(u16*)(b + 8) <= hi_z
                    && (s32)*(u16*)(b + 0xa) >= lo_x && (s32)*(u16*)(b + 0xa) <= hi_x) {
                    func_ov007_020b63e4((char**)arg0);
                } else {
                    u16 t = *(u16*)oo;
                    if ((t == 0xc
                         && (*(u8**)(*(char**)(data_ov007_0210342c + 0x28)))[9] == 0)
                        || (t == 0xd
                            && (*(u8**)(*(char**)(data_ov007_0210342c + 0x28)))[9] != 0)) {
                        if (r4 != 0) {
                            s32 x = *(u16*)(b + 8);
                            if (x >= 8 && x <= 0x66) {
                                s32 y = *(u16*)(b + 0xa);
                                if (y >= 0x50 && y <= 0x60)
                                    func_ov007_020b63e4((char**)arg0);
                            }
                        }
                    } else if (t == 9) {
                        if (r4 != 0) {
                            s32 x = *(u16*)(b + 8);
                            if (x >= 8 && x <= 0x48) {
                                s32 y = *(u16*)(b + 0xa);
                                if (y >= 0x20 && y <= 0x30)
                                    func_ov007_020b63e4((char**)arg0);
                            }
                        }
                    }
                }
                goto done;
            }
        }
    }

    if (*(s16*)(*(char**)(data_ov007_0210342c + 4)) == 3) {
        u16 t0 = *(u16*)(*(char**)arg0);
        if (t0 == 3 || t0 == 6) {
            char* p2 = *(char**)c;
            int r4b = 0;
            int r2b = 0;
            int r1b = 0;
            if (*(*(s16**)(p2 + 4)) == 0) {
                if (*(s32*)(p2 + 0x10) >= 0x1000
                    || (data_ov007_020d77dc[**(u16**)c].f0 & 2))
                    r1b = 1;
            }
            if (r1b) {
                if (!(data_ov007_020d77dc[**(u16**)c].f0 & 1))
                    r2b = 1;
            }
            if (r2b) {
                int sbb = 1;
                if (func_ov007_020b79e4()) {
                    int r8b = 0;
                    if (func_ov007_020b79e4()) {
                        u16 id2 = **(u16**)c;
                        int x = sbb;
                        if (id2 != 0x16 && id2 != 0x17) x = r8b;
                        if (x) r8b = 1;
                    }
                    if (!r8b) sbb = 0;
                }
                if (sbb) r4b = 1;
            }

            if (r4b && func_ov007_020c1da0(0) == 0
                && *(u16*)(b + 0xc) != 0
                && *(s16*)func_ov007_020b8f78() == 5
                && func_ov007_020b8fa0() == 0) {
                char* oo = *(char**)c;
                char* pp = *(char**)(c + 4);
                char* o28 = *(char**)(oo + 0x28);
                int ip = *(s32*)(pp + 8);
                int p4 = *(s32*)(pp + 4);
                int hi_x = *(s32*)(o28 + 0x10) + (ip >> 12);
                int lo_x = *(s32*)(o28 + 8) + (ip >> 12);
                int hi_z = *(s32*)(o28 + 0xc) + (p4 >> 12);
                int lo_z = *(s32*)(o28 + 4) + (p4 >> 12);
                if (*(u16*)(b + 0xc) != 0
                    && (s32)*(u16*)(b + 8) >= lo_z && (s32)*(u16*)(b + 8) <= hi_z
                    && (s32)*(u16*)(b + 0xa) >= lo_x && (s32)*(u16*)(b + 0xa) <= hi_x) {
                    char* fp = *(char**)(oo + 4);
                    r5 = 1;
                    *(s32*)(fp + 4) += 1;
                    if (*(s32*)(*(char**)(*(char**)c + 4) + 4) > 0x3a)
                        func_ov007_020b63e4((char**)arg0);
                }
            }
        }
    }

done:
    if (r5 == 0)
        *(s32*)(*(char**)(*(char**)c + 4) + 4) = 0;
    {
        char* q = *(char**)(*(char**)c + 4);
        if (*(s16*)q != 2)
            return;
        if (*(s16*)(q + 2) == -1)
            return;
    }
    func_ov007_020c1d78(0);
}