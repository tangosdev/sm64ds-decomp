typedef unsigned long long u64;
typedef unsigned short u16;

struct ARMMathState {
    u64 div_numer;
    u64 div_denom;
    u64 sqrt;
    u16 div_mode;
    u16 sqrt_mode;
};

void ARMMathLoadState(const struct ARMMathState *s)
{
    *(volatile u64*)0x4000290 = s->div_numer;
    *(volatile u64*)0x4000298 = s->div_denom;
    *(volatile u16*)0x4000280 = s->div_mode;
    *(volatile u64*)0x40002b8 = s->sqrt;
    *(volatile u16*)0x40002b0 = s->sqrt_mode;
}
