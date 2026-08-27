extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *file, int b, int c, int scale, unsigned short e);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *file, int b, int scale, unsigned int d);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *self, void *file, int b, int scale, unsigned int d);
extern int data_ov006_0213ae60[];
extern int data_ov006_0213adf0[];

struct W2 { int a, b; };

void func_ov006_020c2664(char *c)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 8, *(void **)(c + 0x10c), 4, 0x40000000, 0x800, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0xc8, *(void **)(c + 0x14c), 0x40000000, 0x800, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0xdc, data_ov006_0213ae60, 0x40000000, 0x800, 0);
    *(struct W2 *)c = *(struct W2 *)data_ov006_0213adf0;
}
