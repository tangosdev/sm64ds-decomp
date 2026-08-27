//cpp
// func_ov002_020c2138 at 0x020c2138
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
typedef int Fix12i;
extern "C" int _ZN4cstd5atan2E5Fix12IiES1_(Fix12i, Fix12i);
extern "C" int AngleDiff(int a, int b);
extern "C" int func_ov002_020c200c(void* self, int ang);
extern "C" int func_ov002_020c1c84(void* self, int ang);
extern "C" int func_ov002_020c1ad8(void* self, int ang);
extern "C" int func_ov002_020c2138(char* self){
int ang;
if (*(unsigned char*)(self + 0x706) || *(unsigned char*)(self + 0x6fd)) return 0;
if (*(int*)(self + 0x564)) return 0;
if (*(int*)(self + 0x37c) || *(unsigned char*)(self + 0x708)) return 0;
ang = _ZN4cstd5atan2E5Fix12IiES1_(*(int*)(self + 0x560), *(int*)(self + 0x568));
if (AngleDiff(ang, *(short*)(self + 0x94)) < 0x6000) return 0;
if (func_ov002_020c200c(self, ang)) return 1;
if ((*(int*)(self+0x358) ? 1 : 0) || (*(int*)(self+0x354) ? 1 : 0)) return 0;
if (func_ov002_020c1c84(self, ang)) return 1;
return func_ov002_020c1ad8(self, ang);
}
