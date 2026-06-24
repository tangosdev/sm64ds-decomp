//cpp
extern "C" {
int func_ov098_0213a984(void* c);
void _ZN12CylinderClsn5ClearEv(void* self);
void _ZN12CylinderClsn6UpdateEv(void* self);
}

struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov098_0213c8fc[];

struct C {
    char pad[0x180];
    unsigned int idx;   // 0x180
    unsigned char flag; // 0x184
};

extern "C" int func_ov098_0213b26c(C* c)
{
    if (c->flag != 1) {
        (c->*data_ov098_0213c8fc[c->idx].pmf)();
    }
    func_ov098_0213a984(c);
    _ZN12CylinderClsn5ClearEv((char*)c + 0x124);
    _ZN12CylinderClsn6UpdateEv((char*)c + 0x124);
    return 1;
}
