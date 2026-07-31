//cpp
struct Actor {
    ~Actor();
};

struct Heap;

struct Memory {
    static void Deallocate(void *, Heap *);
};

extern int _ZTV15RecRoomCupboard;
extern int _ZN25MovingCylinderClsnWithPosD1Ev;
extern int data_020a0eac;
extern "C" int __destroy_arr(char *, int, int, int);

extern "C" void *_ZN15RecRoomCupboardD0Ev(char *c) {
    *(int *)c = (int)&_ZTV15RecRoomCupboard;
    __destroy_arr(c + 0xd4, 5, 0x40, (int)&_ZN25MovingCylinderClsnWithPosD1Ev);
    ((Actor *)c)->~Actor();
    Memory::Deallocate(c, (Heap *)*(int *)&data_020a0eac);
    return c;
}
