//cpp
struct Player;
struct State;
extern "C" {

void _ZN6Player11ChangeStateERNS_5StateE(Player *self, State *st);
void _ZN6Player7SetAnimEji5Fix12IiEj(Player *self, unsigned int a, int b, int c, unsigned int d);
extern State data_ov002_02110154;

void func_ov002_020d228c(Player *self)
{
    char *p = (char *)self;
    _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_02110154);
    _ZN6Player7SetAnimEji5Fix12IiEj(self, 9, 0, 0x1000, 0);
    *(char *)(p + 0x6e3) = 4;
    *(int *)(p + 0x56c) = 0;
    *(int *)(p + 0x570) = 0;
    *(int *)(p + 0x574) = 0;
    *(char *)(p + 0x721) = 2;
    *(char *)(p + 0x70c) = 0;
}

}
