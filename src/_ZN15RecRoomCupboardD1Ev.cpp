//cpp
struct dActor_c {
    ~dActor_c();
};

extern int _ZTV15RecRoomCupboard;
extern int _ZN10dCcAcPos_cD1Ev;
extern "C" int __destroy_arr(char *, int, int, int);

extern "C" int _ZN15RecRoomCupboardD1Ev(char *c) {
    *(int *)c = (int)&_ZTV15RecRoomCupboard;
    __destroy_arr(c + 0xd4, 5, 0x40, (int)&_ZN10dCcAcPos_cD1Ev);
    ((dActor_c *)c)->~dActor_c();
    return (int)c;
}
