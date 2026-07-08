//cpp
struct Actor {
    ~Actor();
};

struct Heap;

struct Memory {
    static void Deallocate(void *, Heap *);
};

extern int data_ov058_02111a70;
extern int _ZN25MovingCylinderClsnWithPosD1Ev;
extern int _ZN6Memory11gameHeapPtrE;
extern "C" int func_0207328c(char *, int, int, int);

extern "C" void *func_ov058_021111e0(char *c) {
    *(int *)c = (int)&data_ov058_02111a70;
    func_0207328c(c + 0xd4, 5, 0x40, (int)&_ZN25MovingCylinderClsnWithPosD1Ev);
    ((Actor *)c)->~Actor();
    Memory::Deallocate(c, (Heap *)*(int *)&_ZN6Memory11gameHeapPtrE);
    return c;
}
