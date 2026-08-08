//cpp
// @symbol _ZN6Player12CanEnterDoorEh
// Player::CanEnterDoor(unsigned char door)
// The +0x48 vcall must be spelled as a real C++ virtual call (dummy-virtual
// struct, slot 18): the C function-pointer cast perturbs callee-saved homing
// rank to this=r4/door=r5, while virtual dispatch keeps the natural
// reverse-arg homing (door=r4, this=r5) the ROM has. See
// notes/mwccarm-codegen.md 6bc.
struct Door {
    virtual int v00(); virtual int v01(); virtual int v02(); virtual int v03();
    virtual int v04(); virtual int v05(); virtual int v06(); virtual int v07();
    virtual int v08(); virtual int v09(); virtual int v10(); virtual int v11();
    virtual int v12(); virtual int v13(); virtual int v14(); virtual int v15();
    virtual int v16(); virtual int v17();
    virtual int GetType();
};
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void *c, void *s);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *c, void *s);
extern int _ZN6Player17SetNoControlStateEhih(void *c, unsigned char, int, unsigned char);
extern int data_ov002_0211022c[];
extern int data_ov002_0211013c[];
extern int data_ov002_02110154[];
extern int data_ov002_0211043c[];
extern int data_ov002_0211004c[];
}
extern "C" int _ZN6Player12CanEnterDoorEh(void *c, unsigned char door)
{
  if (_ZN6Player7IsStateERNS_5StateE(c, data_ov002_0211022c)) {
    if (!(*(unsigned char *)((char *)c + 0x6e3) == 0x13 &&
          *(unsigned char *)((char *)c + 0x70b) != 0))
      return 0;
  }
  if (_ZN6Player7IsStateERNS_5StateE(c, data_ov002_0211013c) ||
      _ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110154) ||
      _ZN6Player7IsStateERNS_5StateE(c, data_ov002_0211022c) ||
      _ZN6Player7IsStateERNS_5StateE(c, data_ov002_0211043c)) {
    if (*(struct Door **)((char *)c + 0x360) != 0) {
      if ((*(struct Door **)((char *)c + 0x360))->GetType() == 6 ||
          (*(struct Door **)((char *)c + 0x360))->GetType() == 1) {
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211004c);
      }
      return 0;
    }
    *(unsigned char *)((char *)c + 0x6e3) = door;
    if (_ZN6Player17SetNoControlStateEhih(c, 7, -1, 1) != 0)
      return 1;
  }
  return 0;
}
