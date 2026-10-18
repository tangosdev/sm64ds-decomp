//cpp
#include "Particle__Manager.h"

extern "C" {
int func_ov007_020c3df4(int a, void *b);
char *func_0204a4c8(char *(*alloc)(int), int a, int b, int c, unsigned short arg5);
void *func_ov007_020b3f20(int a);
struct EData;
void func_0204a17c(EData *a, char *b);

extern char data_ov007_02102dd0[];
extern void *data_ov007_02103448;
extern int data_ov007_02103444;
extern char *(func_ov007_020b78d0)(int);
extern void *data_ov007_0210344c;
extern Particle::System *data_ov007_02103450[];

void func_ov007_020b782c(void) {
    int i;
    Particle::System *empty;
    data_ov007_02103448 = (void *)func_ov007_020c3df4((int)&data_ov007_02102dd0, (void *)0x3c00);
    data_ov007_02103444 = 0;
    data_ov007_0210344c = func_0204a4c8(func_ov007_020b78d0, 2, 0xc8, 0x23, 0x25);
    i = 0;
    empty = 0;
    do {
        data_ov007_02103450[i] = empty;
        i++;
    } while (i < 2);
    func_0204a17c(static_cast<EData *>(data_ov007_0210344c),
                  static_cast<char *>(func_ov007_020b3f20(0x17)));
}
}
