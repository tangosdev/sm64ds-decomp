//cpp
namespace Sound { void PlayBank2_2D(unsigned int id); }
extern "C" void func_02012790(int a);
extern "C" void _Z14ApproachLinearRiii(int &x, int target, int step);

struct G { int v; };
extern G *data_ov004_020beb68;
extern int data_ov004_020bf9ec;
extern int data_ov004_020bc7d0;
extern int data_ov004_020bfa18;
extern int data_ov004_020bfa10;
extern int data_ov004_020bfa00;
extern void *data_ov004_020bfa20;
extern "C" {
extern void func_ov004_020b5f6c();
}

extern "C" void func_ov004_020b6234(void) {
    int x;
    if (data_ov004_020beb68 != 0) {
        x = *(int *)((char *)data_ov004_020beb68 + 0xa8);
    } else {
        x = 0;
    }
    if (x == 0) {
        func_02012790(0xe);
        return;
    }
    if (data_ov004_020bf9ec != 0 || data_ov004_020bfa18 >= data_ov004_020bc7d0) {
        func_02012790(0xe);
        return;
    }
    if (data_ov004_020bfa10 != 0) {
        return;
    }
    data_ov004_020bfa10 = 1;
    _Z14ApproachLinearRiii(data_ov004_020bfa18, data_ov004_020bc7d0, 1);
    data_ov004_020bfa00 = data_ov004_020bfa18 - 1;
    Sound::PlayBank2_2D(0x14e);
    data_ov004_020bfa20 = (void *)func_ov004_020b5f6c;
}
