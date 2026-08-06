#ifndef OV004_OBJ_VTBL_H
#define OV004_OBJ_VTBL_H
struct Obj {
    virtual int v00();
    virtual int v04();
    virtual int v08();
    virtual int v0c();
    virtual int v10();
    virtual int v14();
    virtual int v18();
    virtual int v1c();
    virtual int v20();
    virtual int v24();
    virtual int v28();
    virtual int v2c();
    virtual int v30();
    virtual int v34();
    virtual int v38();
    virtual int v3c();
    virtual int v40();
    virtual int v44();
    virtual int v48();
    virtual int v4c();
    virtual int v50();
    virtual int v54();
    virtual int v58();
    virtual int v5c();
    virtual int v60();
    virtual int v64();
    virtual int f68();
    char pad[76];
    int unk50;
    int unk54;
    int unk58;
};
#endif
