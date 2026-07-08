//cpp
typedef unsigned char u8;
typedef unsigned short u16;
struct State;
struct Player;
extern "C" int _ZN6Player11ChangeStateERNS_5StateE(Player* thiz, State* s);
extern "C" int _ZN6Player7IsStateERNS_5StateE(Player* thiz, State* s);
extern "C" int func_ov002_020bf30c(Player* c, int a);
extern State _ZN6Player8ST_THROWE;
extern State _ZN6Player14ST_YOSHI_POWERE;
extern State _ZN6Player12ST_SIDE_FLIPE;
extern State _ZN6Player7ST_JUMPE;
extern State _ZN6Player7ST_DIVEE;
extern State _ZN6Player13ST_PUNCH_KICKE;
extern u8 data_020a0e40;
extern char data_0209f49c[];

extern "C" int func_ov002_020dde74(char* thiz){
  int b = (*(int*)(thiz + 0x358) != 0);
  if (b) {
    _ZN6Player11ChangeStateERNS_5StateE((Player*)thiz, &_ZN6Player8ST_THROWE);
    return 1;
  }
  if (*(int*)(thiz + 8) == 3 && *(u8*)(thiz + 0x703) == 0) {
    _ZN6Player11ChangeStateERNS_5StateE((Player*)thiz, &_ZN6Player14ST_YOSHI_POWERE);
    return 1;
  }
  if ((*(u16*)(data_0209f49c + (u8)data_020a0e40 * 0x18) & 2) ||
      *(u8*)(thiz + 0x703) != 0)
    goto change574;

  if (*(int*)(thiz + 0x98) < func_ov002_020bf30c((Player*)thiz, 0x20000)) {
    if (_ZN6Player7IsStateERNS_5StateE((Player*)thiz, &_ZN6Player12ST_SIDE_FLIPE))
      goto change5bc;
    if (!_ZN6Player7IsStateERNS_5StateE((Player*)thiz, &_ZN6Player7ST_JUMPE))
      goto change574;
    if (*(u8*)(thiz + 0x6e1) != 2)
      goto change574;
  }
change5bc:
  if (*(int*)(thiz + 0x98) < func_ov002_020bf30c((Player*)thiz, 0x24000))
    *(int*)(thiz + 0x98) = func_ov002_020bf30c((Player*)thiz, 0x24000);
  return _ZN6Player11ChangeStateERNS_5StateE((Player*)thiz, &_ZN6Player7ST_DIVEE);

change574:
  if (*(u8*)(thiz + 0x6de) != 0)
    *(u8*)(thiz + 0x6e2) = 2;
  else
    *(u8*)(thiz + 0x6e2) = 0;
  return _ZN6Player11ChangeStateERNS_5StateE((Player*)thiz, &_ZN6Player13ST_PUNCH_KICKE);
}
