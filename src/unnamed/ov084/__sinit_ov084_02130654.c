/* Runtime asset handles resolved through the overlay 0 file table. */
enum sinit_ov084_02130654_AssetId {
    ASSET_PAKKUN_ATTACK_BCA = 0x03cf,
    ASSET_PAKKUN_DEAD_BCA = 0x03d0,
    ASSET_PAKKUN_FIRE_BCA = 0x03d1,
    ASSET_PAKKUN_FIRE_ED_BCA = 0x03d2,
    ASSET_PAKKUN_MODEL_BMD = 0x03d3,
    ASSET_PAKKUN_SLEEP_BCA = 0x03d4,
    ASSET_PAKKUN_SLEEP_LOOP_BCA = 0x03d5
};

extern int func_02017acc();
extern void func_020731dc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);

extern char data_ov084_02130dfc[];
extern char data_ov084_02130e44[];
extern char data_ov084_02130e14[];
extern char data_ov084_02130e50[];
extern char data_ov084_02130e24[];
extern char data_ov084_02130e68[];
extern char data_ov084_02130e0c[];
extern char data_ov084_02130e74[];
extern char data_ov084_02130df4[];
extern char data_ov084_02130e5c[];
extern char data_ov084_02130e04[];
extern char data_ov084_02130e2c[];
extern char data_ov084_02130e1c[];
extern char data_ov084_02130e38[];

struct P2 { int a, b; };
extern struct P2 data_ov084_02130e80[];
extern struct P2 data_ov084_02130bb4;
extern struct P2 data_ov084_02130ba4;
extern struct P2 data_ov084_02130bbc;
extern struct P2 data_ov084_02130bac;
extern struct P2 data_ov084_02130bc4;
extern struct P2 data_ov084_02130bcc;
extern struct P2 data_ov084_02130bdc;
extern struct P2 data_ov084_02130bd4;
extern struct P2 data_ov084_02130be4;

void __sinit_ov084_02130654(void)
{
    func_02017acc(data_ov084_02130dfc, ASSET_PAKKUN_MODEL_BMD);
    func_020731dc(data_ov084_02130dfc, func_02017ab4, data_ov084_02130e44);
    _ZN13SharedFilePtr9ConstructEj(data_ov084_02130e14, ASSET_PAKKUN_ATTACK_BCA);
    func_020731dc(data_ov084_02130e14, SharedFilePtr_Destruct_Anim, data_ov084_02130e50);
    _ZN13SharedFilePtr9ConstructEj(data_ov084_02130e24, ASSET_PAKKUN_DEAD_BCA);
    func_020731dc(data_ov084_02130e24, SharedFilePtr_Destruct_Anim, data_ov084_02130e68);
    _ZN13SharedFilePtr9ConstructEj(data_ov084_02130e0c, ASSET_PAKKUN_SLEEP_BCA);
    func_020731dc(data_ov084_02130e0c, SharedFilePtr_Destruct_Anim, data_ov084_02130e74);
    _ZN13SharedFilePtr9ConstructEj(data_ov084_02130df4, ASSET_PAKKUN_SLEEP_LOOP_BCA);
    func_020731dc(data_ov084_02130df4, SharedFilePtr_Destruct_Anim, data_ov084_02130e5c);
    _ZN13SharedFilePtr9ConstructEj(data_ov084_02130e04, ASSET_PAKKUN_FIRE_BCA);
    func_020731dc(data_ov084_02130e04, SharedFilePtr_Destruct_Anim, data_ov084_02130e2c);
    _ZN13SharedFilePtr9ConstructEj(data_ov084_02130e1c, ASSET_PAKKUN_FIRE_ED_BCA);
    func_020731dc(data_ov084_02130e1c, SharedFilePtr_Destruct_Anim, data_ov084_02130e38);

    data_ov084_02130e80[0] = data_ov084_02130bb4;
    data_ov084_02130e80[1] = data_ov084_02130ba4;
    data_ov084_02130e80[2] = data_ov084_02130bbc;
    data_ov084_02130e80[3] = data_ov084_02130bac;
    data_ov084_02130e80[4] = data_ov084_02130bc4;
    data_ov084_02130e80[5] = data_ov084_02130bcc;
    data_ov084_02130e80[6] = data_ov084_02130bdc;
    data_ov084_02130e80[7] = data_ov084_02130bd4;
    data_ov084_02130e80[8] = data_ov084_02130be4;
}
