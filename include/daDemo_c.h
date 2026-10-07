#ifndef DADEMO_C_H
#define DADEMO_C_H

#ifdef __cplusplus

#include "dActor_c.h"
#include "ModelAnim.h"

/* The nested classes' bases are declared here under the cartridge's RTTI
 * spellings rather than through the old project spellings. daDemo_c's
 * typeinfo and vtable records are emitted by this translation unit, and an
 * emitted record is only verifiable when its mangled name is the one
 * symbols.txt configures: "9ModelAnim" exists nowhere in the ROM,
 * "14dExtAnmModel_c" is the real record at arm9:0x0208e924. The model
 * structs below copy the field layout and vtable order of ModelBase.h and
 * Model.h. The frame-controller base is the real dExtFrameCtrl_c: that is
 * now the project class as well as the cartridge name, so a second local
 * struct would be a redefinition. notes/model-rtti-names.md maps the
 * spellings. */
struct dExtModel_c {
    BMD_File *modelFile;        /* 0x04 */

    dExtModel_c();
    virtual ~dExtModel_c();                              /* slots 0 (D1), 1 (D0) */
    virtual int DoSetFile(char *file, int a, int b) = 0; /* slot 2, null here */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
};

struct dExtSimpleModel_c : dExtModel_c {
    ModelComponents data;      /* 0x08 */
    Matrix4x3 mat4x3;          /* 0x1c */
    void *transformsBuf;       /* 0x4c */

    dExtSimpleModel_c();
    virtual ~dExtSimpleModel_c();                     /* slots 0 (D1), 1 (D0) */
    virtual int DoSetFile(char *file, int a, int b);  /* slot 2 */
    virtual void UpdateVerts();                       /* slot 3 */
    virtual void Virtual10(Matrix4x3 &mat);           /* slot 4 */
    virtual void Render(const Vector3 *scale);        /* slot 5 */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
};

struct dExtAnmModel_c : dExtSimpleModel_c, dExtFrameCtrl_c {
    BCA_File *file;            /* 0x60 */

    virtual ~dExtAnmModel_c();                          /* slots 0 (D1), 1 (D0) */
    virtual void UpdateVerts();                         /* slot 3 */
    virtual void Virtual10(Matrix4x3 &mat);             /* slot 4 */
    virtual void Render(const Vector3 *scale);          /* slot 5 */
    virtual void Virtual18(u32 mat, const Vector3 *scale); /* slot 6 */
    dExtAnmModel_c();
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
};

/* The cutscene-only actor family, vtable _ZTV8daDemo_c.
 *
 * The factory allocates 0x104 bytes, constructs dActor_c, and installs this
 * vtable. The destructor performs no class-local teardown: it changes the
 * vptr, runs dActor_c's destruction, and releases the actor allocation. That
 * is exactly the code generated for an empty destructor on this inheritance
 * graph; none of those operations belongs in the source body.
 *
 * The two owned render objects are selected by param1. InitResources writes a
 * Model pointer at 0xdc for the static variants and a ModelAnim pointer at
 * 0xe0 for the animated variants; CleanupResources destroys whichever exists.
 */
struct daDemo_c : dActor_c {
    /* The model helpers nested in daDemo_c share a scale-bearing base the
       ROM's RTTI names daDemo_c::param_c. It is declared first so it lands
       as the non-primary base at the object tail (0x64 in anmModel_c, 0x50
       in simpleModel_c): bases destroy in reverse declaration order, so the
       Model/ModelAnim base runs before the Vector3 array, which is the
       ROM's order and what its vmi typeinfo records encode. The remaining
       resource pointers live inside the model bases; fields not proven as
       members are read at raw offsets in the destructor bodies. */
    struct param_c {
        Vector3 mScale[1];

        void *func_ov002_020f6a50();
    };

    struct anmModel_c : param_c, dExtAnmModel_c {
        void func_ov002_020f64ac(char *r4);
        void func_ov002_020f6514(void *tbl, unsigned char arg);
        void func_ov002_020f65b8();
        int func_ov002_020f65ec();
        int func_ov002_020f6618(SharedFilePtr *mdl, int nAnims,
                                SharedFilePtr **anims, int arg5,
                                unsigned char texByte, SharedFilePtr **texs,
                                int tsData);
        virtual ~anmModel_c();
        void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
    };

    struct simpleModel_c : param_c, dExtSimpleModel_c {
        int func_ov002_020f6960(SharedFilePtr *fp, int n);
        virtual ~simpleModel_c();
        void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
    };

    u8 pad_0d0[4];          /* 0x0d0 */
    void *unk_0d4;          /* 0x0d4 - 0x36-byte scroll-state block (func_ov002_020f2630) */
    void *unk_0d8;          /* 0x0d8 - heap block freed by func_ov002_020f63a0 */
    Model *mModel;          /* 0x0dc */
    ModelAnim *mModelAnim;  /* 0x0e0 */
    u8 pad_0e4[0x10];       /* 0x0e4 */
    s32 unk_0f4;            /* 0x0f4 - fix12 height/amplitude the handlers scale by */
    u8 pad_0f8[8];          /* 0x0f8 */
    u8 unk_100;             /* 0x100 - scroll-mode byte the PMF3 handlers write */
    u8 pad_101;             /* 0x101 */
    u8 mOpacity;            /* 0x102 */
    u8 unk_103;             /* 0x103 */

    /* Keep the destructor first: it is the class's key function and remains
       the translation-unit owner selected by the ROM's lifecycle symbols. */
    virtual ~daDemo_c();
    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
    virtual void OnPendingDestroy();

    void func_ov002_020f1fcc();
    void func_ov002_020f20f4();
    void func_ov002_020f2210();
    void func_ov002_020f2340();
    void func_ov002_020f237c();
    int func_ov002_020f23d0();
    int func_ov002_020f23f0();
    int func_ov002_020f26c4(unsigned char *src);
    int func_ov002_020f63a0();
    int func_ov002_020f63d4();
    int func_ov002_020f63f8(unsigned char *src);
    int func_ov002_020f6424();
    int func_ov002_020f6448(unsigned char *arg1);
    int func_ov002_020f6a9c();
    int func_ov002_020f6ab8(unsigned char *p);
    int func_ov002_020f6ae4(unsigned char *p);
    int func_ov002_020f6b28(unsigned char *p);
    int func_ov002_020f6b4c(unsigned char *p);
    s32 func_ov002_020f6bc0(unsigned char *data);
    int func_ov002_020f6c24(unsigned char *src);
    int func_ov002_020f6c34(unsigned char *src);
    int func_ov002_020f6c60(void *arg1, int arg2, int arg3);
    int func_ov002_020f6e48(unsigned char *p);
    void func_ov002_020f6f48(Vector3 *v, int amt);
    int func_ov002_020f7020();
    int func_ov002_020f7038(int a, int arg);
    int func_ov002_020f71c4(int a, int arg);
    int func_ov002_020f72bc();
    int func_ov002_020f7384(unsigned char *flag, int val);
    int func_ov002_020f7410(unsigned char *in, int sel);
    int func_ov002_020f7538(unsigned char *arg1, int arg2);
    int func_ov002_020f7780(void *unused, int mode);
    int func_ov002_020f79c0(void *unused, int mode);
    int func_ov002_020f7bb8(unsigned char *p, u16 id);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daDemo_c_size_must_be_0x104[
    sizeof(daDemo_c) == 0x104 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DADEMO_C_H */
