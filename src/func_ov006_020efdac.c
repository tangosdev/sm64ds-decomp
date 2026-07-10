/* func_ov006_020efdac at 0x020efdac
 *
 * Copies a global index (data_0209f60c -> data_0209f608) and tail-calls
 * MultiCopy_Int to copy the 0x300-byte record at that index to 0x04000040.
 */
typedef unsigned int u32;

struct Record { char _pad[0x300]; };

extern u32 data_0209f60c;
extern volatile u32 data_0209f608;
extern struct Record data_0209f648[];

extern void MultiCopy_Int(struct Record *src, void *dst, u32 count);

void func_ov006_020efdac(void)
{
    data_0209f608 = data_0209f60c;
    MultiCopy_Int(&data_0209f648[data_0209f608], (void *)0x04000040, 4);
}
