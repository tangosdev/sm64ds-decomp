//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct Info {
    char pad[0x14];
    u8 count;
};

struct Inner {
    char pad[0x30];
    u16 count;
    u8 *ids;
};

struct Component {
    char pad[0x24];
    u32 flags;
};

struct ModelComponents {
    void *sub;               // offset 0x0 -> *(sub+8) is Inner*
    Component *components;   // offset 0x4
};

struct Slot {
    void *transformer;       // 0x0
    u8 flag;                  // 0x4
    char pad[7];
};

struct ModelBase {
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void Render(void *ctx);
};

extern Info *data_0209f340;
extern char data_020755d4;

extern "C" void _ZN18TextureTransformer6UpdateER15ModelComponents(void *transformer, ModelComponents &mc);

extern "C" void _ZN5Stage11RenderModelEv(char *self)
{
    ModelComponents *mc = (ModelComponents *)(((long long)(int)(self + 0x874)) & 0xFFFFFFFFFFFFFFFFLL);
    Inner *inner = *(Inner **)((char *)mc->sub + 8);
    Slot *slot = (Slot *)(self + 0x8bc);
    int i;

    for (i = 0; i < data_0209f340->count; i++) {
        u8 flag = slot->flag;
        u8 *idx = inner->ids;

        if (flag != 0) {
            u16 j;
            for (j = 0; j < inner->count; j++) {
                u8 id = *idx;
                Component *comp = (Component *)((char *)mc->components + id * 0x30);
                idx++;
                u32 flagsTest = *(volatile u32 *)&comp->flags;
                if ((flagsTest & 0x1f0000) == 0x1f0000) {
                    u32 *p = (u32 *)(((long long)(int)((char *)comp + 0x24)) & 0xFFFFFFFFFFFFFFFFLL);
                    *p &= ~0x80000000;
                } else {
                    u32 *p = (u32 *)(((long long)(int)((char *)comp + 0x24)) & 0xFFFFFFFFFFFFFFFFLL);
                    *p |= 0x80000000;
                }
            }
            if (slot->transformer != 0)
                _ZN18TextureTransformer6UpdateER15ModelComponents(slot->transformer, *mc);
        } else {
            u16 j;
            for (j = 0; j < inner->count; j++) {
                u8 id = *idx;
                Component *comp = (Component *)((char *)mc->components + id * 0x30);
                *(u32 *)(((long long)(int)((char *)comp + 0x24)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x80000000;
                idx++;
            }
        }

        slot = (Slot *)((char *)slot + 0xc);
        inner = (Inner *)((char *)inner + 0x40);
    }

    ((ModelBase *)(self + 0x86c))->Render(&data_020755d4);
}
