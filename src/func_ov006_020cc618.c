typedef unsigned int u32;

typedef struct { u32 a, b; } Pair;

extern Pair data_ov006_0213b1f4;
extern void func_ov006_020cc408(void *this);

void func_ov006_020cc618(void *this) {
    *(Pair *)((char *)this + 0x64) = data_ov006_0213b1f4;
    func_ov006_020cc408(this);
}
