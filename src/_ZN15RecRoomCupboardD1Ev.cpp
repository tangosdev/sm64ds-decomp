//cpp
struct Actor {
    ~Actor();
};

extern int _ZTV15RecRoomCupboard;
extern int _ZN25MovingCylinderClsnWithPosD1Ev;
extern "C" int __destroy_arr(char *, int, int, int);

extern "C" int _ZN15RecRoomCupboardD1Ev(char *c) {
    *(int *)c = (int)&_ZTV15RecRoomCupboard;
    __destroy_arr(c + 0xd4, 5, 0x40, (int)&_ZN25MovingCylinderClsnWithPosD1Ev);
    ((Actor *)c)->~Actor();
    return (int)c;
}
