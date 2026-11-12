/* Host copy of src/unnamed/ov023/__sinit_ov023_02111aa8.c as it stood on the port line
 * before the 2026-10 main merge (749f5799a), which deleted that unenrolled
 * draft: the ROM range 0x02111aa8..0x02111b14 is owned by
 * src/game/actors/d_a_obj_fm_battan.cpp, whose static initializer is
 * file-local there and so cannot answer this flat ROM name.
 * port/slice_hostgen4.txt seated this name; the body is unchanged. */
extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern int data_ov023_02112088[];
extern int func_02017ab4[];
extern int data_ov023_0211209c[];
extern int data_ov023_02112080[];
extern int SharedFilePtr_Destruct_Clsn[];
extern int data_ov023_02112090[];
void __sinit_ov023_02111aa8(void)
{
    func_02017acc(data_ov023_02112088, 1558);
    func_020731dc(data_ov023_02112088, func_02017ab4, data_ov023_0211209c);
    func_02017b4c(data_ov023_02112080, 1559);
    func_020731dc(data_ov023_02112080, SharedFilePtr_Destruct_Clsn, data_ov023_02112090);
}
