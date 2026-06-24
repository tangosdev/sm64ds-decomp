extern void func_02017acc();
extern void func_020731dc();
extern void _ZN13SharedFilePtr9ConstructEj();
extern void func_02017ab4(void);
extern void func_02017984(void);

extern int data_ov026_02113f0c[];
extern int data_ov026_02113f14[];
extern int data_ov026_02113f04[];
extern int data_ov026_02113f20[];

typedef struct { int a, b; } S8;
extern S8 data_ov026_02113d04;
extern S8 data_ov026_02113cec;
extern S8 data_ov026_02113cf4;
extern S8 data_ov026_02113cfc;
typedef struct { S8 x, y; } S16;
extern S16 data_ov026_02113f2c;
extern S16 data_ov026_02113f3c;

void __sinit_ov026_02112c94(void)
{
    func_02017acc(data_ov026_02113f0c, 0x4a9);
    func_020731dc(data_ov026_02113f0c, &func_02017ab4, data_ov026_02113f14);
    _ZN13SharedFilePtr9ConstructEj(data_ov026_02113f04, 0x4a8);
    func_020731dc(data_ov026_02113f04, &func_02017984, data_ov026_02113f20);
    data_ov026_02113f2c.x = data_ov026_02113d04;
    data_ov026_02113f2c.y = data_ov026_02113cec;
    data_ov026_02113f3c.x = data_ov026_02113cf4;
    data_ov026_02113f3c.y = data_ov026_02113cfc;
}
