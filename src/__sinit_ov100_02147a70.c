extern int func_02017acc();
extern void func_020731dc();
extern void func_02017ab4(void);
extern int STAR_DOOR_MODEL_PTR[];
extern int data_ov100_0214893c[];

struct Pair { int a, b; };
extern struct Pair data_02086b58;
extern struct Pair data_ov100_0214836c;
extern struct Pair data_ov100_02148364;
extern struct Pair data_ov100_02148344;
extern struct Pair data_ov100_0214834c;
extern struct Pair data_ov100_0214835c;
extern struct Pair data_ov100_02148354;
extern struct Pair data_ov100_0214833c;
extern struct Pair data_ov100_02148334;

struct Dst2 {
    struct Pair p0;
    struct Pair p1;
};
extern struct Dst2 data_ov100_02148974;
extern struct Dst2 data_ov100_02148964;
extern struct Dst2 data_ov100_02148984;
extern struct Dst2 data_ov100_02148994;
extern struct Dst2 data_ov100_02148954;

void __sinit_ov100_02147a70(void)
{
    func_02017acc(STAR_DOOR_MODEL_PTR, 0x4a1);
    func_020731dc(STAR_DOOR_MODEL_PTR, &func_02017ab4, data_ov100_0214893c);
    data_ov100_02148974.p0 = data_02086b58;
    data_ov100_02148974.p1 = data_ov100_0214836c;
    data_ov100_02148984.p0 = data_ov100_02148364;
    data_ov100_02148984.p1 = data_ov100_02148344;
    data_ov100_02148994.p0 = data_ov100_0214834c;
    data_ov100_02148994.p1 = data_ov100_0214835c;
    data_ov100_02148954.p0 = data_ov100_02148354;
    data_ov100_02148954.p1 = data_ov100_0214833c;
    data_ov100_02148964.p0 = data_02086b58;
    data_ov100_02148964.p1 = data_ov100_02148334;
}
