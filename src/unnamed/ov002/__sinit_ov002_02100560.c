/* Runtime asset handles resolved through the overlay 0 file table. */
enum sinit_ov002_02100560_AssetId {
    ASSET_BOMBHEI_BMD = 0x02b6,
    ASSET_SHELL_GREEN_BMD = 0x03c7,
    ASSET_SHELL_RED_BMD = 0x03c8,
    ASSET_S_NUMBER_BTP = 0x0453,
    ASSET_P_FLOWER_CLOSE_BMD = 0x0483,
    ASSET_P_FLOWER_OPEN_BMD = 0x0484,
    ASSET_SCALE_UP_KINOKO_BMD = 0x049f,
    ASSET_W_RING_BMD = 0x04aa
};

extern int func_02017acc(void*, int);
extern void func_020731dc(void*, void*, void**);
extern int SharedFilePtr_Construct_TexSeq(void*, int);
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_TexSeq(void);

extern void* data_ov002_0210da18;
extern void* data_ov002_0210da90;
extern void* data_ov002_0210da20;
extern void* data_ov002_0210dac0;
extern void* data_ov002_0210d9d8;
extern void* data_ov002_0210dacc;
extern void* data_ov002_0210da30;
extern void* data_ov002_0210dae4;
extern void* data_ov002_0210d9e0;
extern void* data_ov002_0210daf0;
extern void* data_ov002_0210da48;
extern void* data_ov002_0210db14;
extern void* data_ov002_0210d9b8;
extern void* data_ov002_0210db20;
extern void* data_ov002_0210da50;
extern void* data_ov002_0210db38;
extern void* data_ov002_0210d9f8;
extern void* data_ov002_0210db44;
extern void* data_ov002_0210da00;
extern void* data_ov002_0210da60;
extern void* data_ov002_0210d9c8;
extern void* data_ov002_0210da6c;
extern void* data_ov002_0210da58;
extern void* data_ov002_0210da84;
extern void* gPFlowerCloseModelFile;
extern void* data_ov002_0210da9c;
extern void* gPFlowerOpenModelFile;
extern void* data_ov002_0210dad8;
extern void* data_ov002_0210da40;
extern void* data_ov002_0210dafc;
extern void* data_ov002_0210d9a0;
extern void* data_ov002_0210db2c;
extern void* data_ov002_0210d9c0;
extern void* data_ov002_0210db50;
extern void* data_ov002_0210d9a8;
extern void* data_ov002_0210da78;
extern void* data_ov002_0210da28;
extern void* data_ov002_0210daa8;
extern void* data_ov002_0210d9e8;
extern void* data_ov002_0210db08;
extern void* data_ov002_0210da08;
extern void* data_ov002_0210db5c;
extern void* data_ov002_0210da38;
extern void* data_ov002_0210dab4;
extern void* data_ov002_0210da10;
extern void* data_ov002_0210db68;
extern void* data_ov002_0210d9f0;
extern void* data_ov002_0210db74;

void __sinit_ov002_02100560(void)
{
    func_02017acc(&data_ov002_0210da18, ASSET_SHELL_GREEN_BMD);
    func_020731dc(&data_ov002_0210da18, (void*)&func_02017ab4, (void**)&data_ov002_0210da90);
    func_02017acc(&data_ov002_0210da20, ASSET_SHELL_RED_BMD);
    func_020731dc(&data_ov002_0210da20, (void*)&func_02017ab4, (void**)&data_ov002_0210dac0);
    func_02017acc(&data_ov002_0210d9d8, 0x9c0f);
    func_020731dc(&data_ov002_0210d9d8, (void*)&func_02017ab4, (void**)&data_ov002_0210dacc);
    func_02017acc(&data_ov002_0210da30, ASSET_SCALE_UP_KINOKO_BMD);
    func_020731dc(&data_ov002_0210da30, (void*)&func_02017ab4, (void**)&data_ov002_0210dae4);
    func_02017acc(&data_ov002_0210d9e0, ASSET_BOMBHEI_BMD);
    func_020731dc(&data_ov002_0210d9e0, (void*)&func_02017ab4, (void**)&data_ov002_0210daf0);
    func_02017acc(&data_ov002_0210da48, 0x8006);
    func_020731dc(&data_ov002_0210da48, (void*)&func_02017ab4, (void**)&data_ov002_0210db14);
    func_02017acc(&data_ov002_0210d9b8, 0x8005);
    func_020731dc(&data_ov002_0210d9b8, (void*)&func_02017ab4, (void**)&data_ov002_0210db20);
    func_02017acc(&data_ov002_0210da50, 0x8008);
    func_020731dc(&data_ov002_0210da50, (void*)&func_02017ab4, (void**)&data_ov002_0210db38);
    func_02017acc(&data_ov002_0210d9f8, 0x8007);
    func_020731dc(&data_ov002_0210d9f8, (void*)&func_02017ab4, (void**)&data_ov002_0210db44);
    func_02017acc(&data_ov002_0210da00, 0x9c07);
    func_020731dc(&data_ov002_0210da00, (void*)&func_02017ab4, (void**)&data_ov002_0210da60);
    func_02017acc(&data_ov002_0210d9c8, 0x9c06);
    func_020731dc(&data_ov002_0210d9c8, (void*)&func_02017ab4, (void**)&data_ov002_0210da6c);
    func_02017acc(&data_ov002_0210da58, 0x800c);
    func_020731dc(&data_ov002_0210da58, (void*)&func_02017ab4, (void**)&data_ov002_0210da84);
    func_02017acc(&gPFlowerCloseModelFile, ASSET_P_FLOWER_CLOSE_BMD);
    func_020731dc(&gPFlowerCloseModelFile, (void*)&func_02017ab4, (void**)&data_ov002_0210da9c);
    func_02017acc(&gPFlowerOpenModelFile, ASSET_P_FLOWER_OPEN_BMD);
    func_020731dc(&gPFlowerOpenModelFile, (void*)&func_02017ab4, (void**)&data_ov002_0210dad8);
    func_02017acc(&data_ov002_0210da40, 0x8011);
    func_020731dc(&data_ov002_0210da40, (void*)&func_02017ab4, (void**)&data_ov002_0210dafc);
    func_02017acc(&data_ov002_0210d9a0, 0x8010);
    func_020731dc(&data_ov002_0210d9a0, (void*)&func_02017ab4, (void**)&data_ov002_0210db2c);
    func_02017acc(&data_ov002_0210d9c0, 0x8014);
    func_020731dc(&data_ov002_0210d9c0, (void*)&func_02017ab4, (void**)&data_ov002_0210db50);
    func_02017acc(&data_ov002_0210d9a8, 0x8009);
    func_020731dc(&data_ov002_0210d9a8, (void*)&func_02017ab4, (void**)&data_ov002_0210da78);
    func_02017acc(&data_ov002_0210da28, 0x800b);
    func_020731dc(&data_ov002_0210da28, (void*)&func_02017ab4, (void**)&data_ov002_0210daa8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210d9e8, 0x800a);
    func_020731dc(&data_ov002_0210d9e8, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_0210db08);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210da08, ASSET_S_NUMBER_BTP);
    func_020731dc(&data_ov002_0210da08, (void*)&SharedFilePtr_Destruct_TexSeq, (void**)&data_ov002_0210db5c);
    func_02017acc(&data_ov002_0210da38, 0x8004);
    func_020731dc(&data_ov002_0210da38, (void*)&func_02017ab4, (void**)&data_ov002_0210dab4);
    func_02017acc(&data_ov002_0210da10, ASSET_W_RING_BMD);
    func_020731dc(&data_ov002_0210da10, (void*)&func_02017ab4, (void**)&data_ov002_0210db68);
    func_02017acc(&data_ov002_0210d9f0, 0x9c0c);
    func_020731dc(&data_ov002_0210d9f0, (void*)&func_02017ab4, (void**)&data_ov002_0210db74);
}
