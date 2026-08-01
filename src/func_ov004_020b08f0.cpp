//cpp
// @symbol func_ov004_020b08f0
// recovered name: dScMgBase_c_AfterInitResources
/* recovered: renamed to Class_Method */
/* dScMgBase_c::AfterInitResources - recovered from vtable slot identity */
extern "C" {
    void LoadFont(int n);
    void func_ov004_020ae330();
}

struct Scene {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void targetMethod();
    void AfterInitResources(unsigned int flags);
};

extern "C" void func_ov004_020b08f0(Scene* self, unsigned int flags) {
    self->targetMethod();
    LoadFont(2);
    func_ov004_020ae330();
    self->AfterInitResources(flags);
}
