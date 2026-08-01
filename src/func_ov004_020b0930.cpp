//cpp
// @symbol func_ov004_020b0930
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_Scene.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgBase_c.h"
// recovered name: dScMgBase_c_BeforeInitResources
/* recovered: renamed to Class_Method */
/* dScMgBase_c::BeforeInitResources - recovered from vtable slot identity */
struct Obj {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual int v26();  virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33();
};

extern "C" {
extern void Enable3dEngines(void);

extern char data_0209b308[];
extern void* data_ov004_020beb60;
extern char* data_ov004_020beb68;
extern char data_0209f61c[];
extern unsigned char data_0209d460[];
extern unsigned char data_0209d458[];

int func_ov004_020b0930(char* c)
{
    struct dScMgBase_c *self = (struct dScMgBase_c *)(void *)c;
    if (_ZN5Scene19BeforeInitResourcesEv(c) == 0) return 0;
    if (((Obj*)c)->v26() == 0)
        func_02019028();
    else
        Enable3dEngines();
    self->unk_0c8 = *(int*)(data_0209b308 + 0x28);
    if (data_ov004_020beb60 == 0)
        data_ov004_020beb60 = _ZN6Memory13operator_new2Ej(0x4000);
    if (data_ov004_020beb68 != 0)
        *(int*)(data_ov004_020beb68 + 0xb0) = 0;
    self->unk_0b4 = 0;
    self->unk_0b8 = 0;
    self->unk_465c = 0;
    func_ov004_020b8a8c(c + 0x4000);
    ((Obj*)c)->v33();
    func_ov004_020b2cb8();
    _ZN5Scene9SetFadersEP15FaderBrightness(data_0209f61c);
    func_0202ec9c(data_0209f61c, 0);
    data_0209d460[0] = 0;
    data_0209d458[0] = 0;
    ((Obj*)c)->v31();
    return 1;
}
}
