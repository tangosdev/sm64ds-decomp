//cpp
#include "Sound.h"

int ApproachLinear(int& ref, int target, int step);

extern "C" {
extern int func_ov004_020b04c0(void);
extern void func_ov004_020b5108(char* c, int* in);

extern int data_ov004_020bfa00;
extern int data_ov004_020bfa04;
extern int data_ov004_020bfa10;
extern int data_ov004_020bfa18;
extern int data_ov004_020bfa1c;
extern int data_ov004_020bfa20;
extern short data_ov004_020bf9e4;
extern char data_ov004_020bfa34[];
}

extern "C" void func_ov004_020b5f6c(void)
{
    int timedTargetPos[2];
    int targetPos[2];
    if (data_ov004_020bfa18 <= data_ov004_020bfa00)
        return;
    if (data_ov004_020bfa00 < data_ov004_020bfa1c) {
        if (ApproachLinear(data_ov004_020bfa04, 0, 1) == 0)
            return;
        {
            int targetY = (-func_ov004_020b04c0() - 0x10) << 12;
            int targetX = (data_ov004_020bfa00 * 16 + 8) << 12;
            timedTargetPos[0] = targetX;
            timedTargetPos[1] = targetY;
        }
        func_ov004_020b5108(&data_ov004_020bfa34[data_ov004_020bfa00 * 0x24], timedTargetPos);
        Sound::PlayBank2_2D(0x14d);
        data_ov004_020bfa04 = 0x18;
        if (data_ov004_020bfa00 == data_ov004_020bfa18 - 1) {
            data_ov004_020bfa10 = 0;
            data_ov004_020bf9e4 = 1;
            data_ov004_020bfa20 = 0;
            return;
        }
        data_ov004_020bfa00 = data_ov004_020bfa00 + 1;
        return;
    }
    {
        int targetY = (-func_ov004_020b04c0() - 0x10) << 12;
        int targetX = (data_ov004_020bfa00 * 16 + 8) << 12;
        targetPos[0] = targetX;
        targetPos[1] = targetY;
        func_ov004_020b5108(&data_ov004_020bfa34[data_ov004_020bfa00 * 0x24], targetPos);
    }
    data_ov004_020bfa10 = 0;
    if (data_ov004_020bfa00 == data_ov004_020bfa18 - 1) {
        data_ov004_020bf9e4 = 1;
        data_ov004_020bfa20 = 0;
        return;
    }
    data_ov004_020bfa00 = data_ov004_020bfa00 + 1;
}
