//cpp
struct Base1 {
    char pad[0x50 - 4];
    virtual ~Base1();
    virtual void f1();
};
struct Base2 {
    int x;
    virtual ~Base2();
    virtual void g1();
};
struct Animation : Base1, Base2 {
    virtual ~Animation();
};
Animation::~Animation() {}
