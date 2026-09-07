//cpp
// @symbol _ZN5Stage18LoadClsnAndObjectsER11LVL_OverlayjR7dBgW_Kc
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Stage.h"
#include "dBgW.h"
struct KCL_File;
struct CLPS_Block;
struct dBgW_Kc;
struct ObjTable;
struct dActor_c;
struct fBase_c;

struct LVL_SubTbl {
    struct ObjTable* table;
    char pad[8];
};

struct LVL_Overlay_s {
    struct CLPS_Block* clps;
    struct ObjTable* objTable;
    char pad8[2];
    unsigned short kclFileId;
    char padC[4];
    struct LVL_SubTbl* subTables;
    unsigned char subCount;
};

extern "C" {

extern unsigned char data_0209f2d8;
extern int data_0209caa0[];
extern struct fBase_c* data_0209f5c0;

extern struct KCL_File* LoadFile(int handle);
extern void _ZN7dBgW_Kc17UpdateFileOffsetsER8KCL_File(struct KCL_File* f);
extern void _ZN7dBgW_Kc7SetFileEP8KCL_FileR10CLPS_Block(struct dBgW_Kc* thiz, struct KCL_File* f, struct CLPS_Block* clps);
extern int _ZNK7dBgW_Kc16GetOctreeOriginYEv(struct dBgW_Kc* thiz);
extern int _ZNK7dBgW_Kc13GetUnkOctreeYEv(struct dBgW_Kc* thiz);
extern void _Z11LoadObjectsRN11LVL_Overlay8ObjTableEij(struct ObjTable* t, int i, unsigned int p);
extern void _ZN7dBase_c5SpawnEjP7fBase_cii(unsigned int id, struct fBase_c* parent, int a, int b);

void Stage::LoadClsnAndObjects(LVL_Overlay &ovlRef, u32 p, dBgW_Kc &mcRef)
{
    struct LVL_Overlay_s *ovl = (struct LVL_Overlay_s *)&ovlRef;
    struct dBgW_Kc *mc = (struct dBgW_Kc *)&mcRef;
    struct KCL_File* f;
    struct LVL_SubTbl* e;
    signed char i;
    int kuppa;
    int intro;
    int t1;
    int t2;
    int isTwo;

    func_0203accc(0);
    func_0203aca0(0, 0);

    if (ovl->kclFileId != 0) {
        f = LoadFile(ovl->kclFileId);
        _ZN7dBgW_Kc17UpdateFileOffsetsER8KCL_File(f);
        _ZN7dBgW_Kc7SetFileEP8KCL_FileR10CLPS_Block(mc, f, ovl->clps);
        func_0202a850(_ZNK7dBgW_Kc16GetOctreeOriginYEv(mc), _ZNK7dBgW_Kc13GetUnkOctreeYEv(mc));
        ((dBgW *)(mc))->Enable((dActor_c *)(0));
    }

    data_ov002_0211118c = 0;
    _Z11LoadObjectsRN11LVL_Overlay8ObjTableEij(ovl->objTable, -1, p);

    e = ovl->subTables;
    for (i = 0; i < ovl->subCount; i++) {
        if (e->table != 0)
            _Z11LoadObjectsRN11LVL_Overlay8ObjTableEij(e->table, i, p);
        e++;
    }

    kuppa = ContinueKuppaScriptIfNecessary();

    intro = 0;
    t2 = 0;
    t1 = (data_0209f2d8 == 0);
    if (t1 != 0) {
        if ((data_0209caa0[2] & 0x80) == 0)
            t2 = 1;
    }
    if (t2 != 0) {
        if (kuppa == 0)
            intro = 1;
    }

    isTwo = (data_0209f2d8 == 2);
    if (isTwo == 0) {
        _ZN7dBase_c5SpawnEjP7fBase_cii(0x14f, data_0209f5c0, 0, 0);
        if (intro == 0)
            _ZN7dBase_c5SpawnEjP7fBase_cii(0x14e, data_0209f5c0, 0, 0);
    }

    if (intro != 0)
        StartIntroCutscene();
}

}
