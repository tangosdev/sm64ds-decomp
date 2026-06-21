//cpp
struct Player;
extern "C" {

void _ZN6Player7SetAnimEji5Fix12IiEj(Player* self, unsigned int a, int b, int c, unsigned int d);

void func_ov002_020c9d1c(Player* self)
{
    char* p = (char*)self;
    *(unsigned char*)(p + 0x6e3) = 4;
    *(unsigned short*)(p + 0x69c) = 0x4000;
    _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x47, 0, 0x1000, 0);
    *(int*)(p + 0x98) = 0;
    *(int*)(p + 0xa8) = 0;
}

}
