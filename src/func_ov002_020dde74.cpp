//cpp
typedef unsigned char u8;
typedef unsigned short u16;
struct State;
struct Player;
extern "C" int _ZN6Player11ChangeStateERNS_5StateE(Player* thiz, State* s);
extern "C" int _ZN6Player7IsStateERNS_5StateE(Player* thiz, State* s);
extern "C" int func_ov002_020bf30c(Player* c, int a);
extern State data_ov002_021105d4;
extern State data_ov002_02110034;
extern State data_ov002_021101e4;
extern State data_ov002_0211019c;
extern State data_ov002_021105bc;
extern State data_ov002_02110574;
extern u8 data_020a0e40;
extern char data_0209f49c[];

extern "C" int func_ov002_020dde74(char* thiz){
  int b = (*(int*)(thiz + 0x358) != 0);
  if (b) {
    _ZN6Player11ChangeStateERNS_5StateE((Player*)thiz, &data_ov002_021105d4);
    return 1;
  }
  if (*(int*)(thiz + 8) == 3 && *(u8*)(thiz + 0x703) == 0) {
    _ZN6Player11ChangeStateERNS_5StateE((Player*)thiz, &data_ov002_02110034);
    return 1;
  }
  if ((*(u16*)(data_0209f49c + (u8)data_020a0e40 * 0x18) & 2) ||
      *(u8*)(thiz + 0x703) != 0)
    goto change574;

  if (*(int*)(thiz + 0x98) < func_ov002_020bf30c((Player*)thiz, 0x20000)) {
    if (_ZN6Player7IsStateERNS_5StateE((Player*)thiz, &data_ov002_021101e4))
      goto change5bc;
    if (!_ZN6Player7IsStateERNS_5StateE((Player*)thiz, &data_ov002_0211019c))
      goto change574;
    if (*(u8*)(thiz + 0x6e1) != 2)
      goto change574;
  }
change5bc:
  if (*(int*)(thiz + 0x98) < func_ov002_020bf30c((Player*)thiz, 0x24000))
    *(int*)(thiz + 0x98) = func_ov002_020bf30c((Player*)thiz, 0x24000);
  return _ZN6Player11ChangeStateERNS_5StateE((Player*)thiz, &data_ov002_021105bc);

change574:
  if (*(u8*)(thiz + 0x6de) != 0)
    *(u8*)(thiz + 0x6e2) = 2;
  else
    *(u8*)(thiz + 0x6e2) = 0;
  return _ZN6Player11ChangeStateERNS_5StateE((Player*)thiz, &data_ov002_02110574);
}
