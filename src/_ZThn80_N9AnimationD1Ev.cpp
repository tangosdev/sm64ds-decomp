//cpp
// This-adjusting virtual destructor thunk (adjustment -0x50). Compiler-generated
// as a byproduct of a class with a secondary base at offset 0x50 and its own
// out-of-line destructor; the tail-branch target is a relocation wildcard.
struct AnimBase1 {
    char pad[0x50 - 4];
    virtual ~AnimBase1();
    virtual void f1();
};
struct AnimBase2 {
    int x;
    virtual ~AnimBase2();
    virtual void g1();
};
struct Animation : AnimBase1, AnimBase2 {
    virtual ~Animation();
};
Animation::~Animation() {}
