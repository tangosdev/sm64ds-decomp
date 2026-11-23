#include "types.h"

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
} data_ov014_02113380_Layout;
typedef struct {
    u32 w0;
} data_ov014_02113384_Layout;
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
} data_ov014_021134c8_Layout;
typedef struct {
    void *p0;
    u32 w4;
    void *p8;
    u32 wc;
    void *p10;
    u32 w14;
    void *p18;
    u32 w1c;
    void *p20;
    u32 w24;
    void *p28;
    u32 w2c;
    void *p30;
    u32 w34;
    void *p38;
    u32 w3c;
    void *p40;
    u32 w44;
    void *p48;
    u32 w4c;
    void *p50;
    u32 w54;
    void *p58;
    u32 w5c;
    void *p60;
    u32 w64;
    void *p68;
    u32 w6c;
    void *p70;
    u32 w74;
    void *p78;
    u32 w7c;
    u32 w80;
    u32 w84;
    u32 w88;
    u32 w8c;
    u32 w90;
    u32 w94;
    u32 w98;
    u32 w9c;
} data_ov014_02113554_Layout;
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
    u32 w404;
    u32 w408;
    u32 w40c;
    u32 w410;
    u32 w414;
    u32 w418;
    u32 w41c;
    u32 w420;
    u32 w424;
    u32 w428;
    u32 w42c;
    u32 w430;
    u32 w434;
    u32 w438;
    u32 w43c;
    u32 w440;
    u32 w444;
    u32 w448;
    u32 w44c;
    u32 w450;
    u32 w454;
    u32 w458;
    u32 w45c;
    u32 w460;
    u32 w464;
    u32 w468;
    u32 w46c;
    u32 w470;
    u32 w474;
    u32 w478;
    u32 w47c;
    u32 w480;
    u32 w484;
    u32 w488;
    u32 w48c;
    u32 w490;
    u32 w494;
    u32 w498;
    u32 w49c;
    u32 w4a0;
    u32 w4a4;
    u32 w4a8;
    u32 w4ac;
    u32 w4b0;
    u32 w4b4;
    u32 w4b8;
    u32 w4bc;
    u32 w4c0;
    u32 w4c4;
    u32 w4c8;
    u32 w4cc;
    u32 w4d0;
    u32 w4d4;
    u32 w4d8;
    u32 w4dc;
    u32 w4e0;
    u32 w4e4;
    u32 w4e8;
    u32 w4ec;
    u32 w4f0;
    u32 w4f4;
    u32 w4f8;
    u32 w4fc;
    u32 w500;
    u32 w504;
    u32 w508;
    u32 w50c;
    u32 w510;
    u32 w514;
    u32 w518;
    u32 w51c;
    u32 w520;
    u32 w524;
    u32 w528;
    u32 w52c;
    u32 w530;
    u32 w534;
    u32 w538;
    u32 w53c;
    u32 w540;
    u32 w544;
    u32 w548;
    u32 w54c;
    u32 w550;
    u32 w554;
    u32 w558;
    u32 w55c;
    u32 w560;
    u32 w564;
    u32 w568;
    u32 w56c;
    u32 w570;
    u32 w574;
    u32 w578;
    u32 w57c;
    u32 w580;
    u32 w584;
    u32 w588;
    u32 w58c;
    u32 w590;
    u32 w594;
    u32 w598;
    u32 w59c;
    u32 w5a0;
    u32 w5a4;
    u32 w5a8;
    u32 w5ac;
    u32 w5b0;
    u32 w5b4;
    u32 w5b8;
    u32 w5bc;
    u32 w5c0;
    u32 w5c4;
    u32 w5c8;
    u32 w5cc;
    u32 w5d0;
    u32 w5d4;
    u32 w5d8;
    u32 w5dc;
    u32 w5e0;
    u32 w5e4;
    u32 w5e8;
    u32 w5ec;
    u32 w5f0;
    u32 w5f4;
    u32 w5f8;
    u32 w5fc;
    u32 w600;
    u32 w604;
    u32 w608;
    u32 w60c;
    u32 w610;
    u32 w614;
    u32 w618;
    u32 w61c;
    u32 w620;
    u32 w624;
    u32 w628;
    u32 w62c;
    u32 w630;
    u32 w634;
    u32 w638;
    u32 w63c;
    u32 w640;
    u32 w644;
    u32 w648;
    u32 w64c;
    u32 w650;
    u32 w654;
    u32 w658;
    u32 w65c;
    u32 w660;
    u32 w664;
    u32 w668;
    u32 w66c;
    u32 w670;
    u32 w674;
    u32 w678;
    u32 w67c;
    u32 w680;
    u32 w684;
    u32 w688;
    u32 w68c;
    u32 w690;
    u32 w694;
    u32 w698;
    u32 w69c;
    u32 w6a0;
    u32 w6a4;
    u32 w6a8;
    u32 w6ac;
    u32 w6b0;
    u32 w6b4;
    u32 w6b8;
    u32 w6bc;
    u32 w6c0;
    u32 w6c4;
    u32 w6c8;
    u32 w6cc;
    u32 w6d0;
    u32 w6d4;
    u32 w6d8;
    u32 w6dc;
    u32 w6e0;
    u32 w6e4;
    u32 w6e8;
    u32 w6ec;
    u32 w6f0;
    u32 w6f4;
    u32 w6f8;
    u32 w6fc;
    u32 w700;
    u32 w704;
    u32 w708;
    u32 w70c;
    u32 w710;
    u32 w714;
    u32 w718;
    u32 w71c;
    u32 w720;
    u32 w724;
    u32 w728;
    u32 w72c;
    u32 w730;
    u32 w734;
    u32 w738;
    u32 w73c;
    u32 w740;
    u32 w744;
    u32 w748;
    u32 w74c;
    u32 w750;
    u32 w754;
    u32 w758;
    u32 w75c;
    u32 w760;
    u32 w764;
    u32 w768;
    u32 w76c;
    u32 w770;
    u32 w774;
    u32 w778;
    u32 w77c;
    u32 w780;
    u32 w784;
    u32 w788;
    u32 w78c;
    u32 w790;
    u32 w794;
    u32 w798;
    u32 w79c;
    u32 w7a0;
    u32 w7a4;
    u32 w7a8;
    u32 w7ac;
    u32 w7b0;
    u32 w7b4;
    u32 w7b8;
    u32 w7bc;
    u32 w7c0;
    u32 w7c4;
    u32 w7c8;
    u32 w7cc;
    u32 w7d0;
    u32 w7d4;
    u32 w7d8;
    u32 w7dc;
    u32 w7e0;
    u32 w7e4;
    u32 w7e8;
    u32 w7ec;
    u32 w7f0;
    u32 w7f4;
    u32 w7f8;
    u32 w7fc;
    u32 w800;
    u32 w804;
    u32 w808;
    u32 w80c;
    u32 w810;
    u32 w814;
    u32 w818;
    u32 w81c;
    u32 w820;
    u32 w824;
    u32 w828;
    u32 w82c;
    u32 w830;
    u32 w834;
    u32 w838;
    u32 w83c;
    u32 w840;
    u32 w844;
    u32 w848;
    u32 w84c;
    u32 w850;
    u32 w854;
    u32 w858;
    u32 w85c;
    u32 w860;
    u32 w864;
    u32 w868;
    u32 w86c;
    u32 w870;
    u32 w874;
    u32 w878;
    u32 w87c;
    u32 w880;
    u32 w884;
    u32 w888;
    u32 w88c;
    u32 w890;
    u32 w894;
    u32 w898;
    u32 w89c;
    u32 w8a0;
    u32 w8a4;
    u32 w8a8;
    u32 w8ac;
    u32 w8b0;
    u32 w8b4;
    u32 w8b8;
    u32 w8bc;
    u32 w8c0;
    u32 w8c4;
    u32 w8c8;
    u32 w8cc;
    u32 w8d0;
    u32 w8d4;
    u32 w8d8;
    u32 w8dc;
    u32 w8e0;
    u32 w8e4;
    u32 w8e8;
    u32 w8ec;
    u32 w8f0;
    u32 w8f4;
    u32 w8f8;
    u32 w8fc;
    u32 w900;
    u32 w904;
    u32 w908;
    u32 w90c;
    u32 w910;
    u32 w914;
    u32 w918;
    u32 w91c;
    u32 w920;
    u32 w924;
    u32 w928;
    u32 w92c;
    u32 w930;
    u32 w934;
    u32 w938;
    u32 w93c;
    u32 w940;
    u32 w944;
    u32 w948;
    u32 w94c;
    u32 w950;
    u32 w954;
    u32 w958;
    u32 w95c;
    u32 w960;
    u32 w964;
    u32 w968;
    u32 w96c;
    u32 w970;
    u32 w974;
    u32 w978;
    u32 w97c;
    u32 w980;
    u32 w984;
    u32 w988;
    u32 w98c;
    u32 w990;
    u32 w994;
    u32 w998;
    u32 w99c;
    u32 w9a0;
    u32 w9a4;
    u32 w9a8;
    u32 w9ac;
    u32 w9b0;
    u32 w9b4;
    u32 w9b8;
    u32 w9bc;
    u32 w9c0;
    u32 w9c4;
    u32 w9c8;
    u32 w9cc;
    u32 w9d0;
    u32 w9d4;
    u32 w9d8;
    u32 w9dc;
    u32 w9e0;
    u32 w9e4;
    u32 w9e8;
    u32 w9ec;
    u32 w9f0;
    u32 w9f4;
    u32 w9f8;
    u32 w9fc;
    u32 wa00;
    u32 wa04;
    u32 wa08;
    u32 wa0c;
    u32 wa10;
    u32 wa14;
    u32 wa18;
    u32 wa1c;
    u32 wa20;
    u32 wa24;
    u32 wa28;
    u32 wa2c;
    u32 wa30;
    u32 wa34;
    u32 wa38;
    u32 wa3c;
    u32 wa40;
    u32 wa44;
    u32 wa48;
    u32 wa4c;
    u32 wa50;
    u32 wa54;
    u32 wa58;
    u32 wa5c;
    u32 wa60;
    u32 wa64;
    u32 wa68;
    u32 wa6c;
    u32 wa70;
    u32 wa74;
    u32 wa78;
    u32 wa7c;
    u32 wa80;
    u32 wa84;
    u32 wa88;
    u32 wa8c;
    u32 wa90;
    u32 wa94;
    u32 wa98;
    u32 wa9c;
    u32 waa0;
    u32 waa4;
    u32 waa8;
    u32 waac;
    u32 wab0;
    u32 wab4;
    u32 wab8;
    u32 wabc;
    u32 wac0;
    u32 wac4;
    u32 wac8;
    u32 wacc;
    u32 wad0;
    u32 wad4;
    u32 wad8;
    u32 wadc;
    u32 wae0;
    u32 wae4;
    u32 wae8;
    u32 waec;
    u32 waf0;
    u32 waf4;
    u32 waf8;
    void *pafc;
    void *pb00;
    void *pb04;
    void *pb08;
    void *pb0c;
    void *pb10;
    u32 wb14;
    u32 wb18;
    u32 wb1c;
    u32 wb20;
    u32 wb24;
    void *pb28;
    u32 wb2c;
    u32 wb30;
    u32 wb34;
    u32 wb38;
    u32 wb3c;
    u32 wb40;
    u32 wb44;
    void *pb48;
    void *pb4c;
    void *pb50;
    void *pb54;
    void *pb58;
    void *pb5c;
    void *pb60;
    void *pb64;
    void *pb68;
    void *pb6c;
    u32 wb70;
    void *pb74;
    void *pb78;
    void *pb7c;
    void *pb80;
    void *pb84;
    void *pb88;
    void *pb8c;
    void *pb90;
    void *pb94;
    void *pb98;
    void *pb9c;
    void *pba0;
    void *pba4;
    void *pba8;
    void *pbac;
    void *pbb0;
    void *pbb4;
    void *pbb8;
    void *pbbc;
    void *pbc0;
    void *pbc4;
    u32 wbc8;
} data_ov014_02113abc_Layout;

extern u8 _ZTVN3abi20__si_class_type_infoE[];
extern u8 _ZTI13daObjSwdoor_c[];
extern u8 gBAnaShutterModelFile[];
extern u8 gBAnaShutterCollisionFile[];
extern void _Z24daObjBSwdoor_c_classInitv(void);
extern void _ZN14daObjBSwdoor_c13InitResourcesEv(void);
extern void _ZN8dActor_c19BeforeInitResourcesEv(void);
extern void _ZN8dActor_c18AfterInitResourcesEj(void);
extern void _ZN14daObjBSwdoor_c16CleanupResourcesEv(void);
extern void _ZN8dActor_c22BeforeCleanupResourcesEv(void);
extern void _ZN8dActor_c21AfterCleanupResourcesEj(void);
extern void _ZN14daObjBSwdoor_c8BehaviorEv(void);
extern void _ZN8dActor_c14BeforeBehaviorEv(void);
extern void _ZN8dActor_c13AfterBehaviorEj(void);
extern void _ZN8dActor_c12BeforeRenderEv(void);
extern void _ZN8dActor_c11AfterRenderEj(void);
extern void _ZN7fBase_c16OnPendingDestroyEv(void);
extern void _ZN7fBase_c9Virtual34Ejj(void);
extern void _ZN7fBase_c9Virtual38Ejj(void);
extern void _ZN7fBase_c13OnHeapCreatedEv(void);
extern void _ZN14daObjBSwdoor_cD1Ev(void);
extern void _ZN14daObjBSwdoor_cD0Ev(void);
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

extern data_ov014_02113380_Layout data_ov014_02113380;
extern data_ov014_02113384_Layout data_ov014_02113384;
extern SimpleEntry data_ov014_02113388[1];
extern SimpleEntry data_ov014_02113390[1];
extern ObjSub data_ov014_02113398;
extern ObjSub data_ov014_021133a0;
extern SimpleEntry data_ov014_021133a8[1];
extern AreaEnt data_ov014_021133b0;
extern SimpleEntry data_ov014_021133bc[2];
extern SimpleEntry data_ov014_021133cc[2];
extern SimpleEntry data_ov014_021133dc[2];
extern SimpleEntry data_ov014_021133ec[3];
extern SimpleEntry data_ov014_02113404[3];
extern SimpleEntry data_ov014_0211341c[3];
extern LevelEnt data_ov014_02113434;
extern SimpleEntry data_ov014_02113450[4];
extern SimpleEntry data_ov014_02113470[4];
extern SimpleEntry data_ov014_02113490[7];
extern data_ov014_021134c8_Layout data_ov014_021134c8;
extern data_ov014_02113554_Layout data_ov014_02113554;
extern SimpleEntry data_ov014_021135f4[153];
extern data_ov014_02113abc_Layout data_ov014_02113abc;

#pragma define_section sGap0 ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sGap0 begin
data_ov014_02113384_Layout data_ov014_02113384 = {0x00010114};

data_ov014_02113380_Layout data_ov014_02113380 = {0x000006e4};

SimpleEntry data_ov014_02113388[1] = {
    {0x423f, 0x0bb6, 0x10c4, (s16)0xee0b},
};

SimpleEntry data_ov014_021133a8[1] = {
    {0xa001, (s16)0xa0a0, 0x2710, (s16)0xc350},
};

ObjSub data_ov014_021133a0 = {0x09, 0x00, {0x00, 0x00}, (void *)((u8 *)&data_ov014_021134c8 + 64)};

SimpleEntry data_ov014_02113390[1] = {
    {0x443f, 0x0771, 0x10c4, (s16)0xee33},
};

ObjSub data_ov014_02113398 = {0x10, 0x00, {0x00, 0x00}, (void *)((u8 *)&data_ov014_021134c8 + 136)};

AreaEnt data_ov014_021133b0 = {&data_ov014_02113398, (void *)0x00000000, 0x00, {0x00, 0x40, 0x1f}};

SimpleEntry data_ov014_021133bc[2] = {
    {0xff0e, 0x1ab1, 0x07bf, (s16)0xe6f1},
    {0xff0e, (s16)0xed13, 0x03e8, (s16)0xfd12},
};

SimpleEntry data_ov014_021133cc[2] = {
    {0xff0e, 0x1ab1, 0x07bf, (s16)0xe6f1},
    {0xff0e, (s16)0xed13, 0x03e8, (s16)0xfd12},
};

SimpleEntry data_ov014_021133dc[2] = {
    {0xff0e, 0x1ab1, 0x07bf, (s16)0xe6f1},
    {0xff0e, (s16)0xed13, 0x03e8, (s16)0xfd12},
};

SimpleEntry data_ov014_021133ec[3] = {
    {0x0000, (s16)0xff08, 0x7fff, 0x0008},
    {0xff05, 0x7fff, 0x000d, (s16)0xff1a},
    {0x7fff, 0x0027, (s16)0xff0a, (s16)0xffff},
};

SimpleEntry data_ov014_02113404[3] = {
    {0x063e, (s16)0xfa3e, 0x03b6, 0x136e},
    {0x063e, (s16)0xe7fa, 0x041a, (s16)0xebe2},
    {0xc63d, (s16)0xeeec, 0x00fa, 0x1581},
};

SimpleEntry data_ov014_0211341c[3] = {
    {0xff0e, 0x1ab1, 0x07bf, (s16)0xe6f1},
    {0xff0e, (s16)0xed13, 0x03e8, (s16)0xfd12},
    {0x483f, 0x0bb6, 0x10c4, (s16)0xee0b},
};

LevelEnt data_ov014_02113434 = {(void *)((u8 *)&data_ov014_021135f4 + 988),
                                &data_ov014_021133a0,
                                0x06e5,
                                0x06e3,
                                0x06e6,
                                0x06e7,
                                &data_ov014_021133b0,
                                0x01,
                                0x00,
                                0x0640,
                                0x0000001f};

SimpleEntry data_ov014_02113450[4] = {
    {0x063f, 0x0ea1, (s16)0xeae8, 0x0090},
    {0x0207, 0x0a3c, (s16)0xeb2a, 0x0090},
    {0xe713, 0x03e8, (s16)0xf28b, 0x00c0},
    {0x0752, 0x02ee, 0x17fe, 0x0000},
};

SimpleEntry data_ov014_02113470[4] = {
    {0x0229, 0x0a3c, (s16)0xeb6e, 0x0001},
    {0x065c, 0x0ea1, (s16)0xeb1d, 0x1001},
    {0xe704, 0x03e8, (s16)0xf2a0, 0x3002},
    {0x0794, 0x02ee, 0x1820, 0x2002},
};

SimpleEntry data_ov014_02113490[7] = {
    {0xff00, 0x0b9f, 0x110d, (s16)0xee28}, {0x0000, 0x0000, 0x0000, (s16)0xff03},
    {0x0bb8, 0x1060, (s16)0xee08, 0x0000}, {0x0000, 0x0000, (s16)0xff02, (s16)0xe3ab},
    {0x0199, 0x1c51, 0x0000, 0x5e93},      {0x0000, (s16)0xff07, 0x0b9f, 0x016d},
    {0xee28, 0x0000, 0x0000, 0x0000},
};

data_ov014_021134c8_Layout data_ov014_021134c8 = {
    0xe7af0000, 0x18d106a4,
    0x60000000, 0x05100000,
    0xe6110000, 0x162b0000,
    0x40000000, 0x00780000,
    0xe6ad0000, 0x12ff0000,
    0x40000000, 0x00780000,
    0xe7520000, 0x17fd0000,
    0x40000000, 0x00780000,
    0x00003102, (void *)((u8 *)&data_ov014_021135f4 + 1212),
    0x00000403, &data_ov014_021133ec,
    0x00000404, &data_ov014_02113490,
    0x0000010b, &data_ov014_02113380,
    0x00000401, &data_ov014_021134c8,
    0x00000406, &data_ov014_02113470,
    0x00000407, &data_ov014_02113450,
    0x00000108, &data_ov014_021133a8,
    0x0000010e, &data_ov014_02113384,
    0x00005900};

data_ov014_02113554_Layout data_ov014_02113554 = {(void *)((u8 *)&data_ov014_02113abc + 1292),
                                                  0x00005805,
                                                  (void *)((u8 *)&data_ov014_02113abc + 588),
                                                  0x00000c20,
                                                  (void *)((u8 *)&data_ov014_021135f4 + 796),
                                                  0x00000125,
                                                  &data_ov014_02113388,
                                                  0x00000b40,
                                                  (void *)((u8 *)&data_ov014_021135f4 + 444),
                                                  0x00000145,
                                                  &data_ov014_02113390,
                                                  0x00000a60,
                                                  (void *)((u8 *)&data_ov014_02113554 + 124),
                                                  0x00000365,
                                                  &data_ov014_02113404,
                                                  0x00001380,
                                                  (void *)((u8 *)&data_ov014_02113abc + 284),
                                                  0x00000385,
                                                  &data_ov014_0211341c,
                                                  0x00000aa0,
                                                  (void *)((u8 *)&data_ov014_021135f4 + 124),
                                                  0x000002a5,
                                                  &data_ov014_021133cc,
                                                  0x00000bc0,
                                                  (void *)((u8 *)&data_ov014_021135f4 + 620),
                                                  0x000002c5,
                                                  &data_ov014_021133dc,
                                                  0x00000ae0,
                                                  (void *)((u8 *)&data_ov014_021135f4 + 284),
                                                  0x000002e5,
                                                  &data_ov014_021133bc,
                                                  0xea840014,
                                                  0x16a80096,
                                                  0x00000001,
                                                  0x0b220000,
                                                  0x05530032,
                                                  0xf6e50b21,
                                                  0xf3340000,
                                                  0x00010000,
                                                  0xe8c20014};

SimpleEntry data_ov014_021135f4[153] = {
    {0x0000, 0x1964, 0x00ff, (s16)0xc000},
    {0x0000, 0x07d2, 0x0030, 0x122a},
    {0x0352, 0x0a5a, 0x0000, 0x0000},
    {0x0000, 0x0f03, 0x000f, 0x1ac2},
    {0x0795, (s16)0xeb7e, 0x0000, 0x0000},
    {0x0000, (s16)0xff1f, 0x0033, 0x05a9},
    {0x02ee, 0x0197, 0x0000, (s16)0xe8e4},
    {0x0000, (s16)0xffff, 0x000f, (s16)0xef9d},
    {0x02ee, 0x08d0, 0x0000, 0x0000},
    {0x0000, (s16)0xff1f, 0x004c, (s16)0xee80},
    {0x0000, 0x14bc, (s16)0xffff, (s16)0xc000},
    {0x0000, 0x074a, 0x000f, (s16)0xff38},
    {0x0000, 0x107c, 0x0000, (s16)0xc000},
    {0x0000, (s16)0xf0f0, 0x000f, 0x1451},
    {0x03cf, 0x0e18, 0x0000, (s16)0xe000},
    {0x0000, (s16)0xf1f0, 0x0014, (s16)0xea84},
    {0x0096, 0x16a8, 0x0001, 0x0000},
    {0x0000, 0x0b22, 0x000a, 0x0190},
    {0x02ee, 0x076c, 0x0000, 0x0000},
    {0x0000, (s16)0xffff, 0x0033, 0x05a9},
    {0x02ee, 0x0197, 0x0000, (s16)0xe8e4},
    {0x0000, (s16)0xffff, 0x0032, 0x0553},
    {0x0b21, (s16)0xf6e5, 0x0000, (s16)0xf334},
    {0x0000, 0x0001, 0x0015, 0x0fa3},
    {0x02ed, 0x1837, 0x0000, (s16)0xc000},
    {0x0000, 0x0000, 0x0014, (s16)0xe8c2},
    {0x0000, 0x1964, 0x00ff, (s16)0xc000},
    {0x0000, 0x07d2, 0x0017, (s16)0xed13},
    {0x03e8, (s16)0xfd12, 0x0000, 0x0000},
    {0x0000, (s16)0xffff, 0x0017, (s16)0xef33},
    {0x03e8, (s16)0xfd7a, 0x0000, 0x2000},
    {0x0000, (s16)0xffff, 0x000f, (s16)0xff38},
    {0x0000, 0x107c, 0x0000, (s16)0xc000},
    {0x0000, (s16)0xf0f0, 0x000f, 0x1451},
    {0x03cf, 0x0e18, 0x0000, (s16)0xe000},
    {0x0000, (s16)0xf1f0, 0x0014, (s16)0xea84},
    {0x0096, 0x16a8, 0x0001, 0x0000},
    {0x0000, 0x0b22, 0x000a, 0x0190},
    {0x02ee, 0x076c, 0x0000, 0x0000},
    {0x0000, (s16)0xffff, 0x0033, 0x05a9},
    {0x02ee, 0x0197, 0x0000, (s16)0xe8e4},
    {0x0000, (s16)0xffff, 0x0032, 0x0553},
    {0x0b21, (s16)0xf6e5, 0x0000, (s16)0xf334},
    {0x0000, 0x0001, 0x0015, 0x0fa3},
    {0x02ed, 0x1837, 0x0000, (s16)0xc000},
    {0x0000, 0x0000, 0x0014, (s16)0xe8c2},
    {0x0000, 0x1964, 0x00ff, (s16)0xc000},
    {0x0000, 0x07d2, 0x0017, (s16)0xed13},
    {0x03e8, (s16)0xfd12, 0x0000, 0x0000},
    {0x0000, (s16)0xffff, 0x0017, (s16)0xef33},
    {0x03e8, (s16)0xfd7a, 0x0000, 0x2000},
    {0x0000, (s16)0xffff, 0x000f, (s16)0xff38},
    {0x0000, 0x107c, 0x0000, (s16)0xc000},
    {0x0000, (s16)0xf0f0, 0x000f, 0x1451},
    {0x03cf, 0x0e18, 0x0000, (s16)0xe000},
    {0x0000, (s16)0xf1f0, 0x0119, (s16)0xf18c},
    {0x0000, 0x13ec, 0x0002, 0x0000},
    {0x0000, 0x34d2, 0x011a, 0x0bb8},
    {0x1060, (s16)0xee08, 0x0000, 0x0000},
    {0x0000, (s16)0xffff, 0x000f, 0x1451},
    {0x03cf, 0x0e19, 0x0000, (s16)0xe000},
    {0x0000, (s16)0xf2f0, 0x0014, (s16)0xe8c2},
    {0x0000, 0x1964, 0x00ff, (s16)0xc000},
    {0x0000, 0x07d2, 0x0014, (s16)0xea84},
    {0x0096, 0x16a8, 0x0001, 0x0000},
    {0x0000, 0x07d3, 0x000a, 0x0190},
    {0x02ee, 0x076c, 0x0000, 0x0000},
    {0x0000, (s16)0xffff, 0x0033, 0x05a9},
    {0x02ee, 0x0197, 0x0000, (s16)0xe8e4},
    {0x0000, (s16)0xffff, 0x0032, 0x0553},
    {0x0b21, (s16)0xf6e5, 0x0000, (s16)0xf334},
    {0x0000, 0x0001, 0x0014, (s16)0xef8b},
    {0x0000, 0x1387, 0x00ff, (s16)0xc000},
    {0x0000, 0x0b26, 0x00fe, 0x10b0},
    {0x0d06, (s16)0xe6a4, 0x0000, 0x0000},
    {0x0000, 0x2011, 0x000f, (s16)0xff39},
    {0x0000, 0x107d, 0x0000, (s16)0xc000},
    {0x0000, (s16)0xf0f0, 0x0014, (s16)0xea84},
    {0x0096, 0x16a8, 0x0001, 0x0000},
    {0x0000, 0x0b22, 0x000a, 0x0190},
    {0x02ee, 0x076c, 0x0000, 0x0000},
    {0x0000, (s16)0xffff, 0x0033, 0x05a9},
    {0x02ee, 0x0197, 0x0000, (s16)0xe8e4},
    {0x0000, (s16)0xffff, 0x0032, 0x0553},
    {0x0b21, (s16)0xf6e5, 0x0000, (s16)0xf334},
    {0x0000, 0x0001, 0x0015, 0x0fa3},
    {0x02ed, 0x1837, 0x0000, (s16)0xc000},
    {0x0000, 0x0000, 0x0014, (s16)0xe8c2},
    {0x0000, 0x1964, 0x00ff, (s16)0xc000},
    {0x0000, 0x07d2, 0x0014, (s16)0xf357},
    {0x002d, 0x1619, 0x0003, (s16)0xc000},
    {0x0000, (s16)0xffff, 0x0017, (s16)0xed13},
    {0x03e8, (s16)0xfd12, 0x0000, 0x0000},
    {0x0000, (s16)0xffff, 0x0017, (s16)0xef33},
    {0x03e8, (s16)0xfd7a, 0x0000, 0x2000},
    {0x0000, (s16)0xffff, 0x000f, (s16)0xff38},
    {0x0000, 0x107c, 0x0000, (s16)0xc000},
    {0x0000, (s16)0xf0f0, 0x000f, 0x1451},
    {0x03cf, 0x0e18, 0x0000, (s16)0xe000},
    {0x0000, (s16)0xf1f0, 0x0034, (s16)0xf1a7},
    {0x0b22, (s16)0xf9e0, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0034, 0x0ea0},
    {0x0b22, 0x0d8e, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0032, 0x0553},
    {0x0b21, (s16)0xf6e5, 0x0000, (s16)0xf334},
    {0x0000, 0x0000, 0x0014, (s16)0xe8c2},
    {0x0000, 0x1964, 0x00ff, (s16)0xc000},
    {0x0000, 0x07d0, 0x0014, (s16)0xea84},
    {0x0096, 0x16a8, 0x00ff, 0x0000},
    {0x0000, 0x07d1, 0x0038, 0x05dd},
    {0x1060, (s16)0xeaf5, 0x0000, (s16)0x982e},
    {0x0000, (s16)0xff01, 0x000a, 0x0190},
    {0x02ee, 0x076c, 0x0000, 0x0000},
    {0x0000, (s16)0xffff, 0x0033, 0x05a9},
    {0x02ee, 0x0197, 0x0000, (s16)0xe8e4},
    {0x0000, (s16)0xffff, 0x0032, (s16)0xea46},
    {0x007d, 0x155c, 0x0000, 0x6000},
    {0x0000, 0x0000, 0x0013, 0x0462},
    {0x0b10, (s16)0xf692, 0x0000, (s16)0xf334},
    {0x0000, 0x0001, 0x000f, (s16)0xff38},
    {0x0000, 0x107c, 0x0000, (s16)0xc000},
    {0x0000, (s16)0xf0f0, 0x000f, 0x1451},
    {0x03cf, 0x0e18, 0x0000, (s16)0xe000},
    {0x0000, (s16)0xf1f0, 0x4c43, 0x5350},
    {0x0008, 0x0018, 0x0fc0, 0x0000},
    {0x00ff, 0x0000, 0x0fc2, 0x0004},
    {0x00ff, 0x0000, 0x0fc1, 0x0004},
    {0x00ff, 0x0000, (s16)0x8042, 0x0003},
    {0x00ff, 0x0000, (s16)0x8041, 0x0003},
    {0x00ff, 0x0000, 0x0004, 0x0104},
    {0x00ff, 0x0000, 0x0005, 0x0004},
    {0x00ff, 0x0000, 0x0004, 0x0004},
    {0x00ff, 0x0000, 0x0002, 0x0004},
    {0x00ff, 0x0000, 0x0001, 0x0004},
    {0x00ff, 0x0000, 0x2002, 0x0004},
    {0x00ff, 0x0000, 0x2001, 0x0004},
    {0x00ff, 0x0000, 0x3002, 0x0004},
    {0x00ff, 0x0000, 0x3001, 0x0004},
    {0x00ff, 0x0000, 0x0fc1, 0x0003},
    {0x00ff, 0x0000, 0x0fc5, 0x001b},
    {0x00ff, 0x0000, 0x0fc4, 0x0103},
    {0x00ff, 0x0000, 0x0fcc, 0x0103},
    {0x00ff, 0x0000, 0x0fc9, 0x0003},
    {0x00ff, 0x0000, 0x0fc5, 0x0003},
    {0x00ff, 0x0000, 0x0fc4, 0x0003},
    {0x00ff, 0x0000, 0x0fc2, 0x0003},
    {0x00ff, 0x0000, 0x3fc2, 0x0003},
    {0x00ff, 0x0000, 0x3fc4, 0x0003},
    {0x00ff, 0x0000, (s16)0xffff, (s16)0xffff},
    {0xffff, (s16)0xffff, (s16)0xffff, (s16)0xffff},
    {0xffff, (s16)0xffff, (s16)0xffff, (s16)0xffff},
    {0xffff, (s16)0xffff, 0x0582, 0x0e3c},
    {0xe92b, 0x0c58, 0x0da8, (s16)0xe5dd},
};

data_ov014_02113abc_Layout data_ov014_02113abc = {
    0x0c221338,
    0x1346ea27,
    0xf2500b95,
    0x0b100c14,
    0x0a66f719,
    0xf7aa0626,
    0x05290825,
    0xf94afb10,
    0xfd820000,
    0x09d2015e,
    0x0bcae9bc,
    0xe407095d,
    0x07be152d,
    0x165de8dc,
    0xf4c30730,
    0x05cf0a20,
    0xf18cfa95,
    0x13ec0000,
    0x0000f380,
    0x0320157c,
    0x157c02ee,
    0x02ed07d0,
    0x076c12c0,
    0x0ed802ee,
    0x02eef9c0,
    0xf5100708,
    0x044c0000,
    0x0026fd44,
    0x0b54fda8,
    0xfaec0634,
    0x0779189c,
    0x170cf254,
    0xe638080c,
    0x09630bb8,
    0xfe0ce37c,
    0xebb009f8,
    0x0a4f00c8,
    0x02bcf128,
    0xf4480a86,
    0x0b2008fc,
    0x0ce4f830,
    0xf8f80b4e,
    0x0ba01388,
    0x1324f254,
    0xea200c23,
    0x0dc90bb8,
    0x03e8e5d4,
    0xea200e59,
    0x0edf03e8,
    0x0bb8f2b8,
    0xf5100f6f,
    0x0ff6117c,
    0x1193f208,
    0xeff8101b,
    0x10600bbd,
    0x0d48ef02,
    0x0cb20319,
    0x03440c1c,
    0x13881482,
    0x180602f0,
    0x03611a2c,
    0x196414b4,
    0x0d48037d,
    0x02bc0af0,
    0x0d7a079e,
    0x0064029a,
    0x02b313ec,
    0x189cff06,
    0x015e02e1,
    0x035b18ce,
    0x000006d6,
    0xed130017,
    0xfd1203e8,
    0x00000000,
    0xffff0000,
    0x05a90033,
    0x019702ee,
    0xe8e40000,
    0xffff0000,
    0x0190000a,
    0x076c02ee,
    0x00000000,
    0xffff0000,
    0xf704013c,
    0xf0f60708,
    0xd3e90000,
    0xffc003e2,
    0xf66e013b,
    0xf128035d,
    0x00020005,
    0x0022ffff,
    0x16da013b,
    0xf254076c,
    0x00020006,
    0x0023ffff,
    0x1806013c,
    0xee080bb8,
    0x40000000,
    0xffc003e3,
    0xea840014,
    0x16a80096,
    0x00000001,
    0x0b220000,
    0x05530032,
    0xf6e50b21,
    0xf3340000,
    0x00010000,
    0x0fa30015,
    0x183702ed,
    0xc0000000,
    0x00000000,
    0xe8c20014,
    0x19640000,
    0xc00000ff,
    0x07d20000,
    0x10b0000f,
    0xe6a40d06,
    0x00000000,
    0xf0f00000,
    0x0bb80038,
    0xee081060,
    0x982e0000,
    0xff040000,
    0xf4b50017,
    0x00fa02ee,
    0x20000000,
    0xffff0000,
    0xf4280017,
    0x018702ee,
    0x20000000,
    0xffff0000,
    0xf6c70017,
    0xedb003b8,
    0x13330000,
    0xffff0000,
    0xf66c0017,
    0xecfe03b8,
    0x13330000,
    0xffff0000,
    0xef330017,
    0xfd7a03e8,
    0x20000000,
    0xffff0000,
    0x0207000f,
    0xf5be0ac9,
    0x80000000,
    0xf2f00000,
    0xe89d1e29,
    0xe72d0371,
    0xe9e71e29,
    0xee3e03e7,
    0xe60b1e29,
    0xed6f03f7,
    0xe8741e29,
    0xfe5b03e8,
    0xec781e29,
    0x07d002ed,
    0xf0601e29,
    0x05db02ed,
    0xe69d1e29,
    0x0dac02ee,
    0xf0601e29,
    0x0bb802ee,
    0xf2551e29,
    0x09c302ed,
    0xefb81e29,
    0x122f0000,
    0xfa3e1e29,
    0x136e0082,
    0x0b1a1e29,
    0x169102ed,
    0x14b41e29,
    0x170c0352,
    0x100c1e29,
    0x0e630389,
    0x0fa01e29,
    0x06400bb8,
    0x19ef1e29,
    0xeaaf07a8,
    0x17031e29,
    0xe2c7085e,
    0x113be225,
    0xf6210bba,
    0x11fce225,
    0xf5c30bbe,
    0x12bee225,
    0xf5600bc1,
    0x1375e225,
    0xf5020bc3,
    0x1427e225,
    0xf49e0bc6,
    0xfd68e225,
    0x15820000,
    0xfea2e225,
    0x15820000,
    0xffd6e225,
    0x15820000,
    0x011be225,
    0x15820000,
    0x0220e225,
    0x15820000,
    0x0620e225,
    0xf7470b21,
    0x06bbe225,
    0xf7890b21,
    0x076fe225,
    0xf7c20b20,
    0x081ae225,
    0xf80b0b20,
    0x08b4e225,
    0xf84b0b20,
    0xe7644a3f,
    0x041a0352,
    0x0b86e225,
    0x04b00f3c,
    0x0b86e225,
    0x05780ed8,
    0x0b86e225,
    0x05dc0e10,
    0x0b86e225,
    0x05780d48,
    0x0b86e225,
    0x04b00ce4,
    0x0b86e225,
    0x03e80d48,
    0x0b86e225,
    0x03840e10,
    0x0b86e225,
    0x03e80ed8,
    0x0b86e225,
    0x04b00e10,
    0x04e2e225,
    0x04b00f3c,
    0x04e2e225,
    0x05780ed8,
    0x04e2e225,
    0x05dc0e10,
    0x04e2e225,
    0x05780d48,
    0x04e2e225,
    0x04b00ce4,
    0x04e2e225,
    0x03e80d48,
    0x04e2e225,
    0x03840e10,
    0x04e2e225,
    0x03e80ed8,
    0x04e2e225,
    0x04b00e10,
    0x0834e225,
    0x04b00f3c,
    0x0834e225,
    0x05780ed8,
    0x0834e225,
    0x05dc0e10,
    0x0834e225,
    0x05780d48,
    0x0834e225,
    0x04b00ce4,
    0x0834e225,
    0x03e80d48,
    0x0834e225,
    0x03840e10,
    0x0834e225,
    0x03e80ed8,
    0x0834e225,
    0x04b00e10,
    0xfe3ee225,
    0x04b01130,
    0xfe3ee225,
    0x057810cc,
    0xfe3ee225,
    0x05dc1004,
    0xfe3ee225,
    0x05780f3c,
    0xfe3ee225,
    0x04b00ed8,
    0xfe3ee225,
    0x03e80f3c,
    0xfe3ee225,
    0x03841004,
    0xfe3ee225,
    0x03e810cc,
    0xfe3ee225,
    0x04b01004,
    0xfa24e225,
    0x04b01676,
    0xfa24e225,
    0x05781612,
    0xfa24e225,
    0x05dc154a,
    0xfa24e225,
    0x05781482,
    0xfa24e225,
    0x04b0141e,
    0xfa24e225,
    0x03e81482,
    0xfa24e225,
    0x0384154a,
    0xfa24e225,
    0x03e81612,
    0xfa24e225,
    0x04b0154a,
    0x12f6c226,
    0x03290565,
    0x0642c226,
    0x10f203cb,
    0x018dc226,
    0x0768041d,
    0xf574c226,
    0xf0920000,
    0x03ecc226,
    0xe527071f,
    0xeb7ec226,
    0x08fc02ed,
    0xe6cec226,
    0x08fc02ed,
    0x0f9ec226,
    0x06410e6f,
    0xe8470c3f,
    0x0a7b0320,
    0x05f90e3d,
    0x0108041a,
    0x06590007,
    0x11940032,
    0x00000000,
    0x00020000,
    0x06590007,
    0x11940032,
    0x00000000,
    0x00070000,
    0xf73600ad,
    0x03e802bc,
    0x20000000,
    0x00000000,
    0x01a7000d,
    0xeacc0a3c,
    0x97780000,
    0x01200000,
    0x05c5000d,
    0xea6a0ea1,
    0x94a00000,
    0x00200000,
    0xff9d000d,
    0xf2ef0320,
    0x00000000,
    0x0f010000,
    0xfadf000d,
    0xf1a70316,
    0x00000000,
    0x0f010000,
    0x0126001e,
    0x190000fa,
    0x00000000,
    0xff050000,
    0xf736002b,
    0xf2220000,
    0x13000000,
    0xff000000,
    0xf70e002d,
    0xf4d40000,
    0x13000000,
    0x00000000,
    0xf943002d,
    0xf3b70000,
    0x93330000,
    0x00010000,
    0xf7d1000f,
    0x14e800cd,
    0xcaab0000,
    0xffff0000,
    0x0e07000f,
    0x15db0318,
    0x40000000,
    0xffff0000,
    0x03e6000f,
    0x151702ed,
    0xc0000000,
    0xffff0000,
    0x109f000f,
    0x065602e2,
    0xe38f0000,
    0xffff0000,
    0x1a2c000f,
    0xe9cd07ad,
    0x00000000,
    0xffff0000,
    0x08670013,
    0x0d6502e4,
    0x00000000,
    0x00050000,
    0xf9b90013,
    0xf76e0000,
    0x00000000,
    0x00050000,
    0xfc370013,
    0xfc170000,
    0x00000000,
    0x00050000,
    0xf5d80013,
    0x06400000,
    0x00000000,
    0x00050000,
    0xf8890013,
    0x0c080000,
    0x00000000,
    0x00050000,
    0xe6ce0024,
    0x06a402ee,
    0x00000000,
    0xffff0000,
    0xe6ce0024,
    0x0b5402ed,
    0x00000000,
    0xffff0000,
    0xeb7e0024,
    0x06a402ed,
    0x00000000,
    0xffff0000,
    0xeb7e0024,
    0x0b5402ed,
    0x00000000,
    0xffff0000,
    0xe700001e,
    0xf70404e2,
    0x00000000,
    0xff050000,
    0xf1dd0013,
    0xfecb03e8,
    0x00000000,
    0x00050000,
    0xf5210013,
    0xf61c0333,
    0x00000000,
    0x00050000,
    0xf0fd0013,
    0xf2f603a9,
    0x00000000,
    0x00050000,
    0x187a000f,
    0x12ce039e,
    0xc0000000,
    0xffff0000,
    0xf060004c,
    0x170c0000,
    0xc000ffff,
    0x03e90000,
    0xf060004c,
    0x18380000,
    0xc000ffff,
    0x03f00000,
    0x051f004c,
    0x0beb02ee,
    0x0000ffff,
    0x03e80000,
    0xf78e004c,
    0xef6403c7,
    0xd556ffff,
    0x03eb0000,
    0x0ce4004c,
    0x06a40bb8,
    0x2aaaffff,
    0x03ec0000,
    0x199a004c,
    0xe6f107d2,
    0x0000ffff,
    0x03ee0000,
    0xe926004c,
    0x0b5402ed,
    0x0000ffff,
    0x07470000,
    0x0f81004c,
    0xe57d0d48,
    0x2aaaffff,
    0x03ef0000,
    0x0025004c,
    0x1ab20000,
    0xc000ffff,
    0x07480000,
    0xf280004c,
    0x012402ed,
    0x2000ffff,
    0x073e0000,
    0x1324004c,
    0x08980bb8,
    0x00b6ffff,
    0x07400000,
    0xe606004c,
    0xf70403e8,
    0x0000ffff,
    0x073c0000,
    0xea3e0032,
    0x1563007d,
    0x00000000,
    0x00024000,
    0x14820032,
    0x119405dc,
    0x40000000,
    0x00028000,
    0xecdc0032,
    0xf2860514,
    0x00000000,
    0x00024000,
    0x18380032,
    0xe50c07ef,
    0xc0000000,
    0x00020000,
    0x10360032,
    0xf6a00bb8,
    0xc0000000,
    0x00020000,
    0x109a0032,
    0x04b10bb8,
    0x80000000,
    0x0002c000,
    0x0e74001e,
    0x079e0cb2,
    0x00000000,
    0xff050000,
    0x0e34004f,
    0xe5390d7f,
    0x00000000,
    0xffff0000,
    0xfa240130,
    0x04b0154a,
    0x00000000,
    0x0f050000,
    0x0b860130,
    0x04b00e10,
    0x00000000,
    0x0f050000,
    0x08340130,
    0x04b00e10,
    0x00000000,
    0x0f050000,
    0x04e20130,
    0x04b00e10,
    0x00000000,
    0x0f050000,
    0xfe3e0130,
    0x04b01004,
    0x00000000,
    0x0f050000,
    0xee9e0013,
    0xf76803e8,
    0x00000000,
    0x00050000,
    0xf6d20031,
    0xf02e0000,
    0x00000000,
    0x00f30000,
    0x1853000f,
    0x023402e7,
    0x00000000,
    0xffff0000,
    0xea46001d,
    0x155c007d,
    0x00000000,
    0xff010000,
    0x1482001d,
    0x119405dc,
    0x00000000,
    0xffff0000,
    0xecdc001d,
    0xf2860514,
    0x00000000,
    0xffff0000,
    0x1838001d,
    0xe50c07ef,
    0x00000000,
    0xffff0000,
    0x1036001d,
    0xf6a00bb8,
    0x00000000,
    0xffff0000,
    0x109a001d,
    0x04b10bb8,
    0x00000000,
    0xffff0000,
    0xfb1c000f,
    0x161e00f9,
    0xc0000000,
    0xffff0000,
    0x14b40031,
    0x170c0596,
    0x00000000,
    0x00170000,
    0x14b40031,
    0x170c0596,
    0x00000000,
    0x00f80000,
    0xe82c0031,
    0xed4003e8,
    0x00000000,
    0x00fb0000,
    0xe7fa0031,
    0xebe203e8,
    0x00000000,
    0x00850000,
    0xe6ce0031,
    0xed4003e8,
    0x00000000,
    0x00fb0000,
    0xe6380031,
    0xec7803e8,
    0x00000000,
    0x00fb0000,
    0xe66a0031,
    0xeb7e03e8,
    0x00000000,
    0x00fb0000,
    0xe7960031,
    0xeab603e8,
    0x00000000,
    0x00fb0000,
    0xe8c20031,
    0xea8403e8,
    0x00000000,
    0x00fb0000,
    0xe9ee0031,
    0xeb4c03e8,
    0x00000000,
    0x00fb0000,
    0xe9580031,
    0xec7803e8,
    0x00000000,
    0x00fb0000,
    0xf4480013,
    0xec460000,
    0x00000000,
    0x00050000,
    0xe85f000f,
    0x031b02ee,
    0x00000000,
    0xffff0000,
    0x0e85000f,
    0x0f30036f,
    0xc0000000,
    0xf0f10000,
    0x0dfc000f,
    0x062f02d0,
    0xc0000000,
    0xf0f20000,
    0xfa57000f,
    0xf9600000,
    0x15550000,
    0xf1f10000,
    0xeb93000f,
    0x040d02ee,
    0xc0000000,
    0xf1f20000,
    0x08c000bb,
    0x108102ee,
    0xc0000000,
    0xffff0000,
    0xed3900bc,
    0xf8e503e8,
    0x20000000,
    0xffff0000,
    0xf38000bc,
    0xee8b0308,
    0x0e380000,
    0xffff0000,
    0xf95d00bc,
    0xea5b03e8,
    0xd1c80000,
    0xffff0000,
    0x17fa00bb,
    0xfa4f0351,
    0x20000000,
    0xffff0000,
    0x1ab10017,
    0xe6f107bf,
    0x00000000,
    0xffff0000,
    0xf1b40017,
    0x190b0000,
    0x00000000,
    0xffff0000,
    0x53504c43,
    0x00010008,
    0x01880fc0,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0x53504c43,
    0x00010008,
    0x01880fc0,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0x53504c43,
    0x00010008,
    0x00000fc0,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    &_ZTVN3abi20__si_class_type_infoE,
    (void *)((u8 *)&data_ov014_02113abc + 2836),
    &_ZTI13daObjSwdoor_c,
    &gBAnaShutterModelFile,
    &gBAnaShutterCollisionFile,
    (void *)((u8 *)&data_ov014_02113abc + 2748),
    0x61643431,
    0x426a624f,
    0x6f647753,
    0x635f726f,
    0x00000000,
    (void *)_Z24daObjBSwdoor_c_classInitv,
    0x00890028,
    0x00000003,
    0x00120000,
    0x00400000,
    0x01000000,
    0x00000000,
    0x00000000,
    (void *)((u8 *)&data_ov014_02113abc + 2812),
    (void *)_ZN14daObjBSwdoor_c13InitResourcesEv,
    (void *)_ZN8dActor_c19BeforeInitResourcesEv,
    (void *)_ZN8dActor_c18AfterInitResourcesEj,
    (void *)_ZN14daObjBSwdoor_c16CleanupResourcesEv,
    (void *)_ZN8dActor_c22BeforeCleanupResourcesEv,
    (void *)_ZN8dActor_c21AfterCleanupResourcesEj,
    (void *)_ZN14daObjBSwdoor_c8BehaviorEv,
    (void *)_ZN8dActor_c14BeforeBehaviorEv,
    (void *)_ZN8dActor_c13AfterBehaviorEj,
    0x020babf0,
    (void *)_ZN8dActor_c12BeforeRenderEv,
    (void *)_ZN8dActor_c11AfterRenderEj,
    (void *)_ZN7fBase_c16OnPendingDestroyEv,
    (void *)_ZN7fBase_c9Virtual34Ejj,
    (void *)_ZN7fBase_c9Virtual38Ejj,
    (void *)_ZN7fBase_c13OnHeapCreatedEv,
    (void *)_ZN14daObjBSwdoor_cD1Ev,
    (void *)_ZN14daObjBSwdoor_cD0Ev,
    (void *)_ZN8dActor_c13OnYoshiTryEatEv,
    (void *)_ZN8dActor_c13OnTurnIntoEggER7daPly_c,
    (void *)_ZN8dActor_c9Virtual50Ev,
    (void *)_ZN8dActor_c15OnGroundPoundedERS_,
    (void *)_ZN8dActor_c11OnAttacked1ERS_,
    (void *)_ZN8dActor_c11OnAttacked2ERS_,
    (void *)_ZN8dActor_c8OnKickedERS_,
    (void *)_ZN8dActor_c8OnPushedERS_,
    (void *)_ZN8dActor_c24OnHitByCannonBlastedCharERS_,
    (void *)_ZN8dActor_c15OnHitByMegaCharER7daPly_c,
    (void *)_ZN8dActor_c19OnHitFromUnderneathERS_,
    (void *)_ZN8dActor_c16OnAimedAtWithEggEv,
    (void *)_ZN8dActor_c25OnAimedAtWithEggReturnVecEv,
    0x020ee55c};

#pragma section sGap0 end
