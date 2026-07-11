typedef unsigned int u32;
void func_02018c00(void* dst, int a, int id);
int func_0203d7b8(void);
struct Pair;
void func_0205e088(struct Pair* dst, char* src);
void Crash(void);
void FS_LoadOverlay(int a, int id);

struct Region { u32 valid; u32 start; u32 size; };
extern struct Region data_0209d3c4[12];

struct Pair { int a, b; };

void LoadOverlay(int id)
{
    struct Pair pair;
    char buf[0x24];
    u32 start;
    struct Region* slot;
    u32 i;
    u32 total;
    struct Region* p;
    u32 end;

    func_02018c00(buf, 0, id);
    if (func_0203d7b8() != 0) return;
    func_0205e088(&pair, buf);

    slot = 0;
    total = *(u32*)(buf + 8) + *(u32*)(buf + 0xc);
    start = *(u32*)(buf + 4);
    end = start + total;
    p = data_0209d3c4;
    for (i = 0; i < 12; i++, p++) {
        if (p->valid == 0xffffffff) {
            if (slot == 0) slot = p;
        } else if (p->start + p->size > start &&
                   end > p->start) {
            Crash();
        }
    }
    if (slot != 0) {
        slot->valid = id;
        slot->start = start;
        slot->size = total;
    }
    FS_LoadOverlay(0, id);
    func_0203d7b8();
}
