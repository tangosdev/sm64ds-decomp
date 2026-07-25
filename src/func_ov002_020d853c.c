struct State;
extern int _ZN6Player7IsStateERNS_5StateE(void* self, struct State* s);
extern unsigned _ZNK6Player14GetBodyModelIDEjb(void* self, unsigned a, int b);
extern short Vec3_HorzAngle(void* v0, void* v1);
extern int AngleDiff(int a, int b);
extern struct State data_ov002_02110574;
extern unsigned short data_ov002_0210a5dc[];

int func_ov002_020d853c(char* c, char* o) {
  if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110574)) {
    int j = (*(unsigned char*)(c + 0x6e2) << 1) & 0xff;
    unsigned idx = _ZNK6Player14GetBodyModelIDEjb(c, *(int*)(c + 8) & 0xff, 0);
    char* mp = *(char**)(c + 0xdc + idx * 4);
    int v = ((int*)(((long long)(int)(mp + 0x50)) & 0xFFFFFFFFFFFFFFFFLL))[2];
    int k = (unsigned short)(v >> 12);
    if (k < data_ov002_0210a5dc[j] || k > data_ov002_0210a5dc[j + 1]) return 1;
    if (AngleDiff(Vec3_HorzAngle(c + 0x5c, o + 0x5c), *(short*)(c + 0x8e)) < 0x4000) return 0;
  }
  return 1;
}
