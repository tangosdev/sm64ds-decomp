#include "types.h"
#include "CLPS.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u8 categoryAndStar;
    u8 count;
    u8 pad[2];
    void *entries;
} ObjSub;

typedef struct {
    u16 raw;
    s16 x;
    s16 y;
    s16 z;
} SimpleEntry;

typedef struct {
    s16 x;
    s16 y;
    s16 z;
} PathNode;

typedef struct {
    void *objTable;
    void *bta;
    s8 minimapId;
    u8 raw_09[3];
} AreaEnt;

typedef struct {
    void *clps;
    void *objTable;
    u16 modelFileId;
    u16 kclFileId;
    u16 minimapTilesFile;
    u16 minimapPaletteFile;
    void *areas;
    u8 areaCount;
    u8 cameraFlags;
    s16 minimapScale;
    u32 skybox;
} LevelEnt;

typedef char ObjSub_size[sizeof(ObjSub) == 8 ? 1 : -1];
typedef char SimpleEntry_size[sizeof(SimpleEntry) == 8 ? 1 : -1];
typedef char PathNode_size[sizeof(PathNode) == 6 ? 1 : -1];
typedef char AreaEnt_size[sizeof(AreaEnt) == 0xc ? 1 : -1];
typedef char LevelEnt_size[sizeof(LevelEnt) == 0x1c ? 1 : -1];

typedef struct {
    u32 w0;
    u32 w4;
    u32 w8;
    u32 wc;
    u32 w10;
    u32 w14;
    u32 w18;
    u32 w1c;
    u32 w20;
    u32 w24;
    u32 w28;
} data_ov024_02112a10_Layout;
typedef struct {
    u32 w0;
    u32 w4;
    u32 w8;
    u32 wc;
    u32 w10;
    u32 w14;
    u32 w18;
    u32 w1c;
    u32 w20;
    u32 w24;
    u32 w28;
    u32 w2c;
    u32 w30;
    u32 w34;
    u32 w38;
    u32 w3c;
    u32 w40;
    void *p44;
    u32 w48;
    void *p4c;
    u32 w50;
    void *p54;
    u32 w58;
    void *p5c;
    u32 w60;
    void *p64;
    u32 w68;
    void *p6c;
    u32 w70;
    void *p74;
    u32 w78;
    void *p7c;
    u32 w80;
    void *p84;
    u32 w88;
    void *p8c;
    u32 w90;
    void *p94;
    u32 w98;
    void *p9c;
    u32 wa0;
    void *pa4;
    u32 wa8;
    void *pac;
    u32 wb0;
    void *pb4;
    u32 wb8;
    void *pbc;
    u32 wc0;
    void *pc4;
    u32 wc8;
    void *pcc;
    u32 wd0;
    void *pd4;
} data_ov024_02112a3c_Layout;
typedef struct {
    u32 w0;
    u32 w4;
    u32 w8;
    u32 wc;
    u32 w10;
    u32 w14;
    u32 w18;
    u32 w1c;
    u32 w20;
    u32 w24;
    u32 w28;
    u32 w2c;
    u32 w30;
    u32 w34;
    u32 w38;
    u32 w3c;
    u32 w40;
    u32 w44;
    u32 w48;
    u32 w4c;
    u32 w50;
    u32 w54;
    u32 w58;
    u32 w5c;
    u32 w60;
    u32 w64;
    u32 w68;
    u32 w6c;
    u32 w70;
    u32 w74;
    u32 w78;
    u32 w7c;
    u32 w80;
    u32 w84;
    u32 w88;
    u32 w8c;
    u32 w90;
    u32 w94;
    u32 w98;
    u32 w9c;
    u32 wa0;
    u32 wa4;
    u32 wa8;
    u32 wac;
    u32 wb0;
    u32 wb4;
    u32 wb8;
    u32 wbc;
    u32 wc0;
    u32 wc4;
    u32 wc8;
    u32 wcc;
    u32 wd0;
    u32 wd4;
    u32 wd8;
    u32 wdc;
    u32 we0;
    u32 we4;
    u32 we8;
    u32 wec;
    u32 wf0;
    u32 wf4;
    u32 wf8;
    u32 wfc;
    u32 w100;
} data_ov024_02112b14_Layout;
typedef struct {
    u32 w0;
    u32 w4;
    u32 w8;
    u32 wc;
    u32 w10;
    u32 w14;
    u32 w18;
    u32 w1c;
    u32 w20;
    u32 w24;
    u32 w28;
    u32 w2c;
    u32 w30;
    u32 w34;
    u32 w38;
    u32 w3c;
    u32 w40;
    u32 w44;
    u32 w48;
    u32 w4c;
    u32 w50;
    u32 w54;
    u32 w58;
    u32 w5c;
    u32 w60;
    u32 w64;
    u32 w68;
    u32 w6c;
    u32 w70;
    u32 w74;
    u32 w78;
    u32 w7c;
    u32 w80;
    u32 w84;
    u32 w88;
    u32 w8c;
    u32 w90;
    u32 w94;
    u32 w98;
    u32 w9c;
    u32 wa0;
    u32 wa4;
    u32 wa8;
    u32 wac;
    u32 wb0;
    u32 wb4;
    u32 wb8;
    u32 wbc;
    u32 wc0;
    u32 wc4;
    u32 wc8;
    u32 wcc;
    u32 wd0;
    u32 wd4;
    u32 wd8;
    u32 wdc;
    u32 we0;
    u32 we4;
    u32 we8;
    u32 wec;
    u32 wf0;
    u32 wf4;
    u32 wf8;
    u32 wfc;
    u32 w100;
    u32 w104;
    u32 w108;
    u32 w10c;
    u32 w110;
    u32 w114;
    u32 w118;
    u32 w11c;
    u32 w120;
    u32 w124;
    u32 w128;
    u32 w12c;
    u32 w130;
    u32 w134;
    u32 w138;
    u32 w13c;
    u32 w140;
    u32 w144;
    u32 w148;
    u32 w14c;
    u32 w150;
    u32 w154;
    u32 w158;
    u32 w15c;
    u32 w160;
    u32 w164;
    u32 w168;
    u32 w16c;
    u32 w170;
    u32 w174;
    u32 w178;
    u32 w17c;
    u32 w180;
    u32 w184;
    u32 w188;
    u32 w18c;
    u32 w190;
    u32 w194;
    u32 w198;
    u32 w19c;
    u32 w1a0;
    u32 w1a4;
    u32 w1a8;
    u32 w1ac;
    u32 w1b0;
    u32 w1b4;
    u32 w1b8;
    u32 w1bc;
    u32 w1c0;
    u32 w1c4;
    u32 w1c8;
    u32 w1cc;
    u32 w1d0;
    u32 w1d4;
    u32 w1d8;
    u32 w1dc;
    u32 w1e0;
    u32 w1e4;
    u32 w1e8;
    u32 w1ec;
    u32 w1f0;
    u32 w1f4;
    u32 w1f8;
    u32 w1fc;
    u32 w200;
    u32 w204;
    u32 w208;
    u32 w20c;
    u32 w210;
    u32 w214;
    u32 w218;
    u32 w21c;
    u32 w220;
    u32 w224;
    u32 w228;
    u32 w22c;
    u32 w230;
    u32 w234;
    u32 w238;
    u32 w23c;
    u32 w240;
} data_ov024_02112c18_Layout;
typedef struct {
    u32 w0;
    u32 w4;
    u32 w8;
    u32 wc;
    u32 w10;
    u32 w14;
    u32 w18;
    u32 w1c;
    u32 w20;
    u32 w24;
    u32 w28;
    u32 w2c;
    u32 w30;
    u32 w34;
    u32 w38;
    u32 w3c;
    u32 w40;
    u32 w44;
    u32 w48;
    u32 w4c;
    u32 w50;
    u32 w54;
    u32 w58;
    u32 w5c;
    u32 w60;
    u32 w64;
    u32 w68;
    u32 w6c;
    u32 w70;
    u32 w74;
    u32 w78;
    u32 w7c;
    u32 w80;
    u32 w84;
    u32 w88;
    u32 w8c;
    u32 w90;
    u32 w94;
    u32 w98;
    u32 w9c;
    u32 wa0;
    u32 wa4;
    u32 wa8;
    u32 wac;
    u32 wb0;
    u32 wb4;
    u32 wb8;
    u32 wbc;
    u32 wc0;
    u32 wc4;
    u32 wc8;
    u32 wcc;
    u32 wd0;
    u32 wd4;
    u32 wd8;
    u32 wdc;
    u32 we0;
    u32 we4;
    u32 we8;
    u32 wec;
    u32 wf0;
    u32 wf4;
    u32 wf8;
    u32 wfc;
    u32 w100;
    u32 w104;
    u32 w108;
    u32 w10c;
    u32 w110;
    u32 w114;
    u32 w118;
    u32 w11c;
    u32 w120;
    u32 w124;
    u32 w128;
    u32 w12c;
    u32 w130;
    u32 w134;
    u32 w138;
    u32 w13c;
    u32 w140;
    u32 w144;
    u32 w148;
    u32 w14c;
    u32 w150;
    u32 w154;
    u32 w158;
    u32 w15c;
    u32 w160;
    u32 w164;
    u32 w168;
    u32 w16c;
    u32 w170;
    u32 w174;
    u32 w178;
    u32 w17c;
    u32 w180;
    u32 w184;
    u32 w188;
    u32 w18c;
    u32 w190;
    u32 w194;
    u32 w198;
    u32 w19c;
    u32 w1a0;
    u32 w1a4;
    u32 w1a8;
    u32 w1ac;
    u32 w1b0;
    u32 w1b4;
    u32 w1b8;
    u32 w1bc;
    u32 w1c0;
    u32 w1c4;
    u32 w1c8;
    u32 w1cc;
    u32 w1d0;
    u32 w1d4;
    u32 w1d8;
    u32 w1dc;
    u32 w1e0;
    u32 w1e4;
    u32 w1e8;
    u32 w1ec;
    u32 w1f0;
    u32 w1f4;
    u32 w1f8;
    u32 w1fc;
    u32 w200;
    u32 w204;
    u32 w208;
    u32 w20c;
    u32 w210;
    u32 w214;
    u32 w218;
    u32 w21c;
    u32 w220;
    u32 w224;
    u32 w228;
    u32 w22c;
    u32 w230;
    u32 w234;
    u32 w238;
    u32 w23c;
    u32 w240;
    u32 w244;
    u32 w248;
    u32 w24c;
    u32 w250;
    u32 w254;
    u32 w258;
    u32 w25c;
    u32 w260;
    u32 w264;
    u32 w268;
    u32 w26c;
    u32 w270;
    u32 w274;
    u32 w278;
    u32 w27c;
    u32 w280;
    u32 w284;
    u32 w288;
    u32 w28c;
    u32 w290;
    u32 w294;
    u32 w298;
    u32 w29c;
    u32 w2a0;
    u32 w2a4;
    u32 w2a8;
    u32 w2ac;
    u32 w2b0;
    u32 w2b4;
    u32 w2b8;
    u32 w2bc;
    u32 w2c0;
    u32 w2c4;
    u32 w2c8;
    u32 w2cc;
    u32 w2d0;
    u32 w2d4;
    u32 w2d8;
    u32 w2dc;
    u32 w2e0;
    u32 w2e4;
    u32 w2e8;
    u32 w2ec;
    u32 w2f0;
    u32 w2f4;
    u32 w2f8;
    u32 w2fc;
    u32 w300;
    u32 w304;
    u32 w308;
    u32 w30c;
    u32 w310;
    u32 w314;
    u32 w318;
    u32 w31c;
    u32 w320;
    u32 w324;
    u32 w328;
    u32 w32c;
    u32 w330;
    u32 w334;
    u32 w338;
    u32 w33c;
    u32 w340;
    u32 w344;
    u32 w348;
    u32 w34c;
    u32 w350;
    u32 w354;
    u32 w358;
    u32 w35c;
    u32 w360;
    u32 w364;
    u32 w368;
    u32 w36c;
    u32 w370;
    u32 w374;
    u32 w378;
    u32 w37c;
    u32 w380;
    u32 w384;
    u32 w388;
    u32 w38c;
    u32 w390;
    u32 w394;
    u32 w398;
    u32 w39c;
    u32 w3a0;
    u32 w3a4;
    u32 w3a8;
    u32 w3ac;
    u32 w3b0;
    u32 w3b4;
    u32 w3b8;
    u32 w3bc;
    u32 w3c0;
    u32 w3c4;
    u32 w3c8;
    u32 w3cc;
    u32 w3d0;
    u32 w3d4;
    u32 w3d8;
    u32 w3dc;
    u32 w3e0;
    u32 w3e4;
    u32 w3e8;
    u32 w3ec;
    u32 w3f0;
    u32 w3f4;
    u32 w3f8;
    u32 w3fc;
    u32 w400;
} data_ov024_02112e5c_Layout;
typedef struct {
    u32 tag;
    u16 stride;
    u16 count;
    struct CLPS entries[168];
    u32 tail[110];
} data_ov024_02113260_Clps;

extern u8 data_ov024_02112880[];
extern u8 data_ov024_02112958[];
extern u8 data_ov024_021128f8[];
extern u8 data_ov024_02112884[];
extern u8 data_ov024_02112998[];
extern u8 data_ov024_02112948[];
extern u8 data_ov024_02112890[];
extern u8 data_ov024_02112908[];
extern u8 data_ov024_02112918[];
extern u8 data_ov024_02112898[];
extern u8 data_ov024_02112928[];
extern u8 data_ov024_02112938[];
extern u8 data_ov024_021129d0[];
extern u8 _ZTVN3abi20__si_class_type_infoE[];
extern u8 _ZTI8dActor_c[];
extern u8 _ZTI10dBgActor_c[];
extern void _Z26daObjDlPyramid_c_classInitv(void);
extern void _Z31daObjDlPyramidDummy_c_classInitv(void);
extern void _ZN21daObjDlPyramidDummy_c13InitResourcesEv(void);
extern void _ZN8dActor_c19BeforeInitResourcesEv(void);
extern void _ZN8dActor_c18AfterInitResourcesEj(void);
extern void _ZN7fBase_c16CleanupResourcesEv(void);
extern void _ZN8dActor_c22BeforeCleanupResourcesEv(void);
extern void _ZN8dActor_c21AfterCleanupResourcesEj(void);
extern void _ZN21daObjDlPyramidDummy_c8BehaviorEv(void);
extern void _ZN8dActor_c14BeforeBehaviorEv(void);
extern void _ZN8dActor_c13AfterBehaviorEj(void);
extern void _ZN7fBase_c6RenderEv(void);
extern void _ZN8dActor_c12BeforeRenderEv(void);
extern void _ZN8dActor_c11AfterRenderEj(void);
extern void _ZN7fBase_c16OnPendingDestroyEv(void);
extern void _ZN7fBase_c9Virtual34Ejj(void);
extern void _ZN7fBase_c9Virtual38Ejj(void);
extern void _ZN7fBase_c13OnHeapCreatedEv(void);
extern void _ZN21daObjDlPyramidDummy_cD1Ev(void);
extern void _ZN21daObjDlPyramidDummy_cD0Ev(void);
extern void _ZN8dActor_c13OnYoshiTryEatEv(void);
extern void _ZN8dActor_c13OnTurnIntoEggER7daPly_c(void);
extern void _ZN8dActor_c9Virtual50Ev(void);
extern void _ZN8dActor_c15OnGroundPoundedERS_(void);
extern void _ZN8dActor_c11OnAttacked1ERS_(void);
extern void _ZN8dActor_c11OnAttacked2ERS_(void);
extern void _ZN8dActor_c8OnKickedERS_(void);
extern void _ZN8dActor_c8OnPushedERS_(void);
extern void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void);
extern void _ZN8dActor_c15OnHitByMegaCharER7daPly_c(void);
extern void _ZN8dActor_c19OnHitFromUnderneathERS_(void);
extern void _ZN8dActor_c16OnAimedAtWithEggEv(void);
extern void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(void);
extern void _ZN16daObjDlPyramid_c13InitResourcesEv(void);
extern void _ZN16daObjDlPyramid_c16CleanupResourcesEv(void);
extern void _ZN16daObjDlPyramid_c8BehaviorEv(void);
extern void _ZN16daObjDlPyramid_c6RenderEv(void);
extern void _ZN16daObjDlPyramid_cD1Ev(void);
extern void _ZN16daObjDlPyramid_cD0Ev(void);

extern SimpleEntry data_ov024_02112a00[2];
extern data_ov024_02112a10_Layout data_ov024_02112a10;
extern data_ov024_02112a3c_Layout data_ov024_02112a3c;
extern data_ov024_02112b14_Layout data_ov024_02112b14;
extern data_ov024_02112c18_Layout data_ov024_02112c18;
extern data_ov024_02112e5c_Layout data_ov024_02112e5c;
extern data_ov024_02113260_Clps data_ov024_02113260;

#pragma define_section sGap1 ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sGap1 begin
SimpleEntry data_ov024_02112a00[2] = {
    {0xffff, (s16)0xffff, (s16)0xffff, (s16)0xffff},
    {0xffff, (s16)0xffff, (s16)0xffff, (s16)0xffff},
};

data_ov024_02112a10_Layout data_ov024_02112a10 = {0xf830ff00, 0xfc1805cc, 0x00000000, 0xff020000,
                                                  0x015e02ee, 0x00001edc, 0x00008000, 0xf830ff07,
                                                  0xfc180000, 0x00000000, 0x00000000};

data_ov024_02112a3c_Layout data_ov024_02112a3c = {
    0x02bc0000, 0x180606c9,
    0x40000000, 0x05080000,
    0x01c20000, 0x186a0025,
    0x40000000, 0x00080000,
    0x01c20000, 0x19640025,
    0x40000000, 0x00080000,
    0x02bc0000, 0x19c80025,
    0x40000000, 0x00080000,
    0x00006002, (void *)((u8 *)&data_ov024_02112c18 + 232),
    0x00002f03, (void *)((u8 *)&data_ov024_02112b14 + 208),
    0x00000304, &data_ov024_02112a10,
    0x0000010b, &data_ov024_02112880,
    0x00000401, &data_ov024_02112a3c,
    0x00000206, &data_ov024_02112958,
    0x00000207, &data_ov024_021128f8,
    0x0000010e, &data_ov024_02112884,
    0x0000020a, &data_ov024_02112998,
    0x00003200, (void *)((u8 *)&data_ov024_02112e5c + 228),
    0x00001a05, &data_ov024_02112b14,
    0x00000120, &data_ov024_02112948,
    0x00000125, &data_ov024_02112890,
    0x00000140, &data_ov024_02112908,
    0x00000160, &data_ov024_02112918,
    0x00000165, &data_ov024_02112898,
    0x00000180, &data_ov024_02112928,
    0x000001c0, &data_ov024_02112938,
    0x000002e0, &data_ov024_021129d0};

data_ov024_02112b14_Layout data_ov024_02112b14 = {
    0x16760e3f, 0x0c80028a, 0x0cb2e226, 0xed7c0000, 0xebb0e226, 0xe69cff5d, 0xe98ae225, 0xf63c04e2,
    0xe98ae225, 0x02ee04e2, 0x06d6e225, 0x02ee04e2, 0x06d6e225, 0xf63c047e, 0xe958e225, 0xfa880064,
    0xe958e225, 0xfb500064, 0xe958e225, 0xfc180064, 0xe958e225, 0xfce00064, 0xe958e225, 0xfda80064,
    0xf5bee225, 0xf78a02ee, 0xf686e225, 0xf78a02ee, 0xf74ee225, 0xf78a02ee, 0xf816e225, 0xf78a02ee,
    0xf8dee225, 0xf78a02ee, 0x01f4e226, 0xf2540640, 0xee6ce226, 0xf2540640, 0xee08e226, 0x05780640,
    0x0258e226, 0x05dc0640, 0xf830043d, 0xfe0c046a, 0x1676ee26, 0x08fc0032, 0xe9267e29, 0xed7f0000,
    0xe570e226, 0x1ce80032, 0xe5ffca3f, 0xe98900fa, 0xff040000, 0x0004ffff, 0x7f02ff02, 0xff020006,
    0x00087f02, 0x7f02ff02, 0xff02000a, 0x000c7f02, 0x7f02ff02, 0xff02000e, 0x00107f02, 0x7f02ff02,
    0xff020012};

data_ov024_02112c18_Layout data_ov024_02112c18 = {
    0x00147f02, 0x7f02ff02, 0xff020016, 0x00187f02, 0x7f02ff02, 0xff02001a, 0x001c7f02, 0x7f02ff02,
    0xff02001e, 0x00207f02, 0x7f02ff02, 0xff020022, 0x00247f02, 0x7f02ff02, 0xff020026, 0x00287f02,
    0x7f02ff02, 0xff02002a, 0x002c7f02, 0x7f02ff02, 0xff02002e, 0x00307f02, 0x7f02ff02, 0xff020032,
    0x00347f02, 0x7f02ff02, 0xff020036, 0x00387f02, 0x7f02ff02, 0xff02003a, 0x003c7f02, 0x7f02ff02,
    0xff02003e, 0x00407f02, 0x7f03ff02, 0xff020042, 0x00447f03, 0x7f02ff02, 0xff020046, 0x00487f02,
    0x7f02ff02, 0xff02004a, 0x004c7f02, 0x7f02ff02, 0xff02004e, 0x00507f02, 0x7f02ff02, 0xff020052,
    0x00542202, 0x2302ff02, 0xff020056, 0x00587f02, 0x7f02ff02, 0xff02005a, 0x005c7f02, 0x7f02ff02,
    0xff02005e, 0x00007f02, 0x050c06d5, 0xe98b02ee, 0x02ee050c, 0x050ce988, 0x06d5f643, 0xf63e050c,
    0xfff0f254, 0xf2540818, 0x0aacff57, 0xffefef99, 0xf1e109c4, 0x0af0ff57, 0xffe8efc5, 0xf1e10cb2,
    0x0b6dff56, 0xfff2f254, 0xf2540e55, 0x0bd2ff5e, 0xffedf4fe, 0xf2cc0cb2, 0x0b6dff57, 0xffebf500,
    0xf2cc09c4, 0x0af0ff58, 0xffeffe10, 0xfe10081c, 0x0ab0ff56, 0xffeefb54, 0xfd9d09c8, 0x0af4ff56,
    0xffe8fb80, 0xfd9d0cb6, 0x0b71ff56, 0xfff3fe10, 0xfe100e59, 0x0bd6ff5f, 0xffee00b8, 0xfe880cb6,
    0x0b71ff59, 0xffec00ba, 0xfe8809c8, 0x0af4ff58, 0xfff003dd, 0x03dd0b7b, 0x0e0eff57, 0xfff10121,
    0x036a0d26, 0x0e52ff59, 0xffea014d, 0x036a1014, 0x0ecfff58, 0xfff203dd, 0x03dd11b7, 0x0f34ff5e,
    0xffeb0686, 0x04551014, 0x0ecfff55, 0xffe90688, 0x04550d26, 0x0e52ff56, 0xfff00bb4, 0x0bb4fe53,
    0x00e6ff58, 0xfff008f8, 0x0b41fffe, 0x0129ff58, 0xffe80924, 0x0b4102eb, 0x01a6ff57, 0xfff20bb4,
    0x0bb4048f, 0x020cff5d, 0xffec0e5d, 0x0c2c02eb, 0x01a6ff56, 0xffeb0e5f, 0x0c2cfffe, 0x0129ff58,
    0xffef15e7, 0x15e712d7, 0x156bff57, 0xfff2132b, 0x15741483, 0x15afff5b, 0xffec1357, 0x15741771,
    0x162cff5a};

data_ov024_02112e5c_Layout data_ov024_02112e5c = {
    0xfff215e7, 0x15e71914, 0x1691ff5e, 0xffea1890, 0x165f1771, 0x162cff54, 0xffe81892, 0x165f1483,
    0x15afff54, 0xfff00116, 0x01161d6c, 0x1f23ff15, 0xffff1d4e, 0x1f28ff69, 0xff69ff12, 0x0000f837,
    0xf8370777, 0x04570000, 0xffeb01cc, 0x004705ac, 0x043bff2d, 0xffeb0391, 0x006ffba6, 0xfba6ff22,
    0x000001b4, 0x0087f2a8, 0xf3cbff48, 0xffecf87f, 0xf87ff0b3, 0xf3d1ff24, 0xffe2ee8c, 0xf00bf2ae,
    0xf3e3ff35, 0xffeeecc3, 0xefd7fb8e, 0xfb82ff29, 0xfff2ee6c, 0xeff105a0, 0x043fff38, 0xffec170c,
    0x170cfdbc, 0xfaa7ff27, 0xffd108be, 0x0983fc33, 0xf9b7ff29, 0xfffafaec, 0xfaecf054, 0xeeecff46,
    0xffe7f094, 0xf20dea84, 0xea84ff2a, 0xfff20960, 0x0960e4c6, 0xe6eeff06, 0xffd51cf4, 0x1b42f060,
    0xf060ff06, 0x0d4800bd, 0x1644003c, 0x00000000, 0x00000000, 0xf8300067, 0xfc1805dc, 0x00000000,
    0xffff0000, 0x06d60068, 0x02ee03e8, 0x00000000, 0xffff0000, 0x06d60068, 0xf63c036b, 0x00000000,
    0xffff0000, 0xe98a0068, 0xf63c03e8, 0x00000000, 0xffff0000, 0xe98a0068, 0x02ee03e8, 0x00000000,
    0xffff0000, 0x06dd00bd, 0xfd3104b0, 0x00000000, 0x00010000, 0x044c0098, 0x189c0025, 0x00000000,
    0xffff0000, 0x07d00013, 0x18380000, 0x00000000, 0x00050000, 0x0ce40013, 0x18380000, 0x00000000,
    0x00050000, 0x15e00031, 0x15e000a2, 0x00000000, 0x00f30000, 0x157c000f, 0x0a280032, 0x00000000,
    0xf1f00000, 0x1838000f, 0x0a280032, 0x00000000, 0xf1f10000, 0x16760017, 0x10680032, 0x00000000,
    0xffff0000, 0x16760017, 0x08fc0032, 0x00000000, 0xffff0000, 0x1676001c, 0x0c800032, 0x00000000,
    0xffff0000, 0x1676001e, 0x10680352, 0x00000000, 0xff050000, 0x15e0004c, 0x0af00258, 0x0000ffff,
    0x073b0000, 0x1676001f, 0x08fc0352, 0x00000000, 0xff030000, 0x1388004c, 0xfe700012, 0x0000ffff,
    0x05460000, 0x1a5e001e, 0xebb000fa, 0x00000000, 0xff050000, 0xec1400a1, 0xf4480073, 0x00000000,
    0xffff0000, 0xf1f000bd, 0xfc18043b, 0x00000000, 0x00010000, 0xfe2900a1, 0xf7a00258, 0x00000000,
    0xffff0000, 0xf4ac001e, 0x02ee01f4, 0x00000000, 0xff050000, 0xfbb4001f, 0x02ee01f4, 0x00000000,
    0xff020000, 0xf380004c, 0x02ee00fa, 0x0000ffff, 0x073c0000, 0xff380031, 0x10040000, 0x00000000,
    0x00f30000, 0xecdc0098, 0x06a40000, 0x00000000, 0xffff0000, 0xe9260031, 0xed7f0258, 0x00000000,
    0x00170000, 0xfb1e00f2, 0xe98a0000, 0x00000000, 0xff000000, 0x04e200f2, 0xed720000, 0x00000000,
    0xff010000, 0x128e00f2, 0xf34e0000, 0x00000000, 0xff020000, 0xe98a0014, 0xf18c0000, 0x40000002,
    0x0b220000, 0x1a2c0032, 0xe5d40000, 0xc0000000, 0x00020000, 0x1a2c001d, 0xe5d40000, 0x00000000,
    0xff010000, 0x11c600fd, 0x1324000f, 0x00000000, 0xffff0000, 0x146400fd, 0x02d400ce, 0x00000000,
    0xffff0000, 0xe63800fd, 0xf31c0000, 0x00000000, 0xffff0000, 0xec4600fd, 0x0bea0064, 0x00000000,
    0xffff0000, 0x03e80140, 0x0e94ff38, 0x00000000, 0xff140000, 0x0bb80140, 0x016eff38, 0x00000000,
    0xff140000, 0xf2540140, 0x0b32ff38, 0x00000000, 0xff140000, 0xe9260031, 0xed7f0258, 0x00000000,
    0x00f80000, 0xf574000f, 0x02ee00fa, 0x00000000, 0xf1f20000, 0x1492011c, 0xe98c0000, 0x000000b6,
    0xffff0000, 0x1492002c, 0xe98c0000, 0x40000000, 0x4b050000, 0x0cac00fe, 0xe99a0000, 0x00000000,
    0x02040000, 0xe97700fe, 0x007c0017, 0x00000000, 0x00040000, 0xe7be001c, 0x00ec0000, 0x00000000,
    0xffff0000};

data_ov024_02113260_Clps data_ov024_02113260 = {
    0x53504c43,
    8,
    168,
    {{0x00750fc4, 0x000000ff}, {0x01000fc0, 0x000000ff}, {0x00740fc4, 0x000000ff},
     {0x01710fc4, 0x000000ff}, {0x00710fc4, 0x000000ff}, {0x00710fc0, 0x000000ff},
     {0x00742004, 0x000000ff}, {0x00740004, 0x000000ff}, {0x00741004, 0x000000ff},
     {0x00743004, 0x000000ff}, {0x00040020, 0x000000ff}, {0x01040004, 0x000000ff},
     {0x018c0000, 0x000000ff}, {0x003c0008, 0x000000ff}, {0x004c0008, 0x00000001},
     {0x004c0008, 0x00000006}, {0x004c0008, 0x00000004}, {0x004c0008, 0x00000005},
     {0x004c0008, 0x00000003}, {0x004c0008, 0x00000002}, {0x004c0008, 0x00000007},
     {0x004c0008, 0x0000000c}, {0x004c0008, 0x0000000a}, {0x004c0008, 0x0000000b},
     {0x004c0008, 0x00000009}, {0x004c0008, 0x00000008}, {0x004c0008, 0x0000000d},
     {0x004c0008, 0x00000012}, {0x004c0008, 0x00000010}, {0x004c0008, 0x00000011},
     {0x004c0008, 0x0000000f}, {0x004c0008, 0x0000000e}, {0x004c0008, 0x00000013},
     {0x004c0008, 0x00000018}, {0x004c0008, 0x00000016}, {0x004c0008, 0x00000017},
     {0x004c0008, 0x00000015}, {0x004c0008, 0x00000014}, {0x004c0008, 0x00000019},
     {0x004c0008, 0x0000001e}, {0x004c0008, 0x0000001c}, {0x004c0008, 0x0000001d},
     {0x004c0008, 0x0000001b}, {0x004c0008, 0x0000001a}, {0x004c0008, 0x00000021},
     {0x004c0008, 0x00000028}, {0x004c0008, 0x00000025}, {0x004c0008, 0x00000027},
     {0x004c0008, 0x00000026}, {0x004c0008, 0x00000024}, {0x004c0008, 0x00000023},
     {0x004c0008, 0x00000022}, {0x004c0008, 0x000000ff}, {0x004c0008, 0x0000001f},
     {0x004c0008, 0x00000020}, {0x004c0008, 0x00000029}, {0x004c0008, 0x0000002d},
     {0x004c0008, 0x0000002c}, {0x004c0008, 0x0000002b}, {0x004c0008, 0x0000002e},
     {0x004c0008, 0x0000002a}, {0x00440008, 0x00000001}, {0x00440008, 0x00000006},
     {0x00440008, 0x00000004}, {0x00440008, 0x00000005}, {0x00440008, 0x00000003},
     {0x00440008, 0x00000002}, {0x00440008, 0x00000007}, {0x00440008, 0x0000000c},
     {0x00440008, 0x0000000a}, {0x00440008, 0x0000000b}, {0x00440008, 0x00000009},
     {0x00440008, 0x00000008}, {0x00440008, 0x0000000d}, {0x00440008, 0x00000012},
     {0x00440008, 0x00000010}, {0x00440008, 0x00000011}, {0x00440008, 0x0000000f},
     {0x00440008, 0x0000000e}, {0x00440008, 0x00000013}, {0x00440008, 0x00000018},
     {0x00440008, 0x00000016}, {0x00440008, 0x00000017}, {0x00440008, 0x00000015},
     {0x00440008, 0x00000014}, {0x00440008, 0x00000019}, {0x00440008, 0x0000001e},
     {0x00440008, 0x0000001c}, {0x00440008, 0x0000001d}, {0x00440008, 0x0000001b},
     {0x00440008, 0x0000001a}, {0x003c0008, 0x00000001}, {0x003c0008, 0x00000006},
     {0x003c0008, 0x00000004}, {0x003c0008, 0x00000005}, {0x003c0008, 0x00000003},
     {0x003c0008, 0x00000002}, {0x003c0008, 0x00000007}, {0x003c0008, 0x0000000c},
     {0x003c0008, 0x0000000a}, {0x003c0008, 0x0000000b}, {0x003c0008, 0x00000009},
     {0x003c0008, 0x00000008}, {0x003c0008, 0x0000000d}, {0x003c0008, 0x00000012},
     {0x003c0008, 0x00000010}, {0x003c0008, 0x00000011}, {0x003c0008, 0x0000000f},
     {0x003c0008, 0x0000000e}, {0x003c0008, 0x00000013}, {0x003c0008, 0x00000018},
     {0x003c0008, 0x00000016}, {0x003c0008, 0x00000017}, {0x003c0008, 0x00000015},
     {0x003c0008, 0x00000014}, {0x003c0008, 0x00000019}, {0x003c0008, 0x0000001e},
     {0x003c0008, 0x0000001c}, {0x003c0008, 0x0000001d}, {0x003c0008, 0x0000001b},
     {0x003c0008, 0x0000001a}, {0x003c0008, 0x0000001f}, {0x003c0008, 0x00000020},
     {0x003c0008, 0x00000027}, {0x003c0008, 0x00000026}, {0x003c0008, 0x00000025},
     {0x003c0008, 0x00000024}, {0x003c0008, 0x00000021}, {0x003c0008, 0x00000028},
     {0x003c0008, 0x0000002e}, {0x003c0008, 0x0000002a}, {0x003c0008, 0x0000002c},
     {0x003c0008, 0x0000002b}, {0x003c0008, 0x00000029}, {0x003c0008, 0x0000002d},
     {0x00340008, 0x00000001}, {0x00340008, 0x00000006}, {0x00340008, 0x00000004},
     {0x00340008, 0x00000005}, {0x00340008, 0x00000003}, {0x00340008, 0x00000002},
     {0x00340008, 0x00000007}, {0x00340008, 0x0000000c}, {0x00340008, 0x0000000a},
     {0x00340008, 0x0000000b}, {0x00340008, 0x00000009}, {0x00340008, 0x00000008},
     {0x00340008, 0x0000000d}, {0x00340008, 0x00000012}, {0x00340008, 0x00000010},
     {0x00340008, 0x00000011}, {0x00340008, 0x0000000f}, {0x00340008, 0x0000000e},
     {0x00340008, 0x00000013}, {0x00340008, 0x00000018}, {0x00340008, 0x00000016},
     {0x00340008, 0x00000017}, {0x00340008, 0x00000015}, {0x00340008, 0x00000014},
     {0x00340008, 0x00000019}, {0x00340008, 0x0000001e}, {0x00340008, 0x0000001c},
     {0x00340008, 0x0000001d}, {0x00340008, 0x0000001b}, {0x00340008, 0x0000001a},
     {0x00042008, 0x000000ff}, {0x00044008, 0x000000ff}, {0x00040008, 0x000000ff}},
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x0209a764, 0x021137ec,
     0x0208e390, 0x0209a764, 0x021137d8, 0x021089ec, 0x61643631, 0x446a624f, 0x7279506c, 0x64696d61,
     0x0000635f, 0x61643132, 0x446a624f, 0x7279506c, 0x64696d61, 0x6d6d7544, 0x00635f79, 0x02111874,
     0x00f80055, 0x00000000, 0x00064000, 0x000fa000, 0x00c80000, 0x00640000, 0x0211183c, 0x00f90056,
     0x00000001, 0x00064000, 0x000fa000, 0x00c80000, 0x00640000, 0x00000000, 0x021137c0, 0x021117c8,
     0x02011268, 0x02011244, 0x02043bf0, 0x02011220, 0x02011214, 0x02111668, 0x02010fd4, 0x02010fc8,
     0x02043af0, 0x02010f78, 0x02010f6c, 0x02043ac0, 0x0204357c, 0x0204349c, 0x02043494, 0x0211124c,
     0x0211127c, 0x02010160, 0x02010154, 0x0201014c, 0x02010148, 0x02010144, 0x02010140, 0x0201013c,
     0x02010138, 0x02010134, 0x02010130, 0x0201012c, 0x02010124, 0x020100dc, 0x00000000, 0x021137cc,
     0x021116ec, 0x02011268, 0x02011244, 0x02111504, 0x02011220, 0x02011214, 0x02111568, 0x02010fd4,
     0x02010fc8, 0x0211153c, 0x02010f78, 0x02010f6c, 0x02043ac0, 0x0204357c, 0x0204349c, 0x02043494,
     0x021111a0, 0x021111ec, 0x02010160, 0x02010154, 0x0201014c, 0x02010148, 0x02010144, 0x02010140,
     0x0201013c, 0x02010138, 0x02010134, 0x02010130, 0x0201012c, 0x02010124, 0x020100dc, 0x020ee55c,
     0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000}};

#pragma section sGap1 end
