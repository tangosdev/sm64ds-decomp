typedef struct { int unk[1]; } SharedFilePtr;

extern SharedFilePtr data_ov002_0210da00;   /* 0x0210da00 */
extern SharedFilePtr data_ov002_0210d9c8;  /* 0x0210d9c8 */

extern void _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* ptr); /* 0x02017a3c */

void LoadBlueCoinModel(void)
{
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da00);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9c8);
}
