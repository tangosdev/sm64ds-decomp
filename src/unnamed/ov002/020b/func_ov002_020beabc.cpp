//cpp
struct Model { void SetPolygonMode(int); };
struct Player {
    unsigned int GetBodyModelID(unsigned int, bool) const;
};
extern "C" int func_ov002_020becf4(Player *self, unsigned int a, int b);

extern "C" void func_ov002_020beabc(Player *self)
{
    char *p = (char *)self;
    *(short *)(p + 0x73c) = 5;
    *(short *)(p + 0x73e) = 0x2f;
    *(short *)(p + 0x740) = 0x1000;
    *(unsigned char *)(p + 0x6dc) = *(unsigned char *)(p + 0x6d9);
    *(unsigned char *)(p + 0x6dd) = *(unsigned char *)(p + 0x6dc);
    unsigned int id = self->GetBodyModelID(*(unsigned char *)(p + 0x6d9), 0);
    ((Model *)*(int *)(p + (id << 2) + 0xdc))->SetPolygonMode(2);
    unsigned int id2 = func_ov002_020becf4(self, *(int *)(p + 8) & 0xff, 0);
    ((Model *)*(int *)(p + (id2 << 2) + 0x154))->SetPolygonMode(2);
}
