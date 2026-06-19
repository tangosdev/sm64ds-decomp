//cpp
struct Base {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(void*);
};
extern "C" void _ZN4Heap9SetNodeIDEj(Base *c, void *a) { c->v13(a); }
