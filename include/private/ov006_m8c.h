#ifndef OV006_M8C_H
#define OV006_M8C_H
/* Dummy vtable stand-in: slot 0x8c is dScMgBase_c's last new virtual.
   pad + width place width at object +0x4cbc (vptr is 4 bytes). */
struct Ov006M8c {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34();
    virtual int m8c();
    char pad_4cb8[0x4cb8];
    int width;
};
#endif
