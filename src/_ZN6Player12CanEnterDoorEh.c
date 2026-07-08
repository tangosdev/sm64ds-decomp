//cpp
// NONMATCHING: register allocation (div=13). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct State {};
struct Player {
    bool IsState(State &);
    void ChangeState(State &);
    int SetNoControlState(unsigned char, int, unsigned char);
    int CanEnterDoor(unsigned char);
};

extern State _ZN6Player13ST_NO_CONTROLE;
extern State _ZN6Player7ST_WALKE;
extern State data_ov002_02110154;
extern State _ZN6Player10ST_ON_WALLE;
extern State _ZN6Player10ST_SWALLOWE;

struct DoorObj { int (**vtbl)(DoorObj *); };

int Player::CanEnterDoor(unsigned char door)
{
    if (IsState(_ZN6Player13ST_NO_CONTROLE)) {
        if (!(*(unsigned char *)((char *)this + 0x6e3) == 0x13 &&
              *(unsigned char *)((char *)this + 0x70b) != 0))
            return 0;
    }

    if (IsState(_ZN6Player7ST_WALKE) || IsState(data_ov002_02110154) ||
        IsState(_ZN6Player13ST_NO_CONTROLE) || IsState(_ZN6Player10ST_ON_WALLE)) {
        if (*(DoorObj **)((char *)this + 0x360) != 0) {
            if ((*(DoorObj **)((char *)this + 0x360))->vtbl[0x48 / 4](*(DoorObj **)((char *)this + 0x360)) == 6 ||
                (*(DoorObj **)((char *)this + 0x360))->vtbl[0x48 / 4](*(DoorObj **)((char *)this + 0x360)) == 1) {
                ChangeState(_ZN6Player10ST_SWALLOWE);
            }
            return 0;
        }
        *(unsigned char *)((char *)this + 0x6e3) = door;
        if (SetNoControlState(7, -1, 1) != 0)
            return 1;
    }
    return 0;
}
