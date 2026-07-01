//cpp
extern int ApproachLinear(int&, int, int);
extern void func_ov004_020b4e78(char* c);
namespace Sound { void PlayBank2_2D(unsigned int); }

extern int data_ov004_020bf9fc;
extern int data_ov004_020bfa04;
extern int data_ov004_020bfa00;
extern char data_ov004_020bfa34[];
extern int data_ov004_020bf9f0;
struct Obj { char pad[0xa8]; int x; };
extern Obj* data_ov004_020beb68;
extern int data_ov004_020bfa0c;
extern unsigned char data_ov004_020bfa56[];
extern void (*data_ov004_020bfa20)();
extern void func_ov004_020b586c();

extern "C" void func_ov004_020b58c4(void)
{
    if (data_ov004_020bf9fc > 0 && (data_ov004_020beb68 ? data_ov004_020beb68->x : 0) != 0) {
        if (ApproachLinear(data_ov004_020bfa04, 0, 1) == 0)
            return;
        if (data_ov004_020bf9f0 != 0) {
            int t = data_ov004_020beb68 ? data_ov004_020beb68->x : 0;
            if (t <= data_ov004_020bf9fc)
                Sound::PlayBank2_2D(0x14c);
            else
                Sound::PlayBank2_2D(0x14b);
            data_ov004_020bf9f0 = 0;
        }
        func_ov004_020b4e78(&data_ov004_020bfa34[data_ov004_020bfa00 * 0x24]);
        data_ov004_020bfa0c = data_ov004_020bfa00;
        if (ApproachLinear(data_ov004_020bfa00, 0x14, 1) != 0)
            data_ov004_020bfa00 = 0;
        if (ApproachLinear(data_ov004_020bf9fc, 0, 1) == 0)
            data_ov004_020bfa04 = 0x10;
        return;
    }
    if (data_ov004_020bfa56[data_ov004_020bfa0c * 0x24] != 0)
        return;
    data_ov004_020bfa04 = 0x3c;
    data_ov004_020bfa20 = func_ov004_020b586c;
}
