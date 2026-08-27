/* func_0201fc88 at 0x0201fc88 */

typedef signed short s16;
typedef signed int s32;

extern s32 ObjectMessageIDToActualMessageID(void);
extern void func_0201f32c(s16 msgID);

void func_0201fc88(void)
{
    s16 id = (s16)ObjectMessageIDToActualMessageID();
    func_0201f32c(id);
}
