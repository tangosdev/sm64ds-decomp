typedef struct UnkA
{
    char pad[0x94];
    short unk94;
} UnkA;

typedef struct UnkB
{
    char pad[0x8e];
    short unk8e;
} UnkB;

extern void func_ov002_020ae968(UnkA *, UnkB *);

void func_ov002_020ae954(UnkA *a, UnkB *b)
{
    a->unk94 = b->unk8e;
    func_ov002_020ae968(a, b);
}
