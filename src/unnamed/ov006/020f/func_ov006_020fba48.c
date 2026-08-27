/* func_ov006_020fba48 at 0x020137fc
 * Converts a sublevel ID to a course ID, then opens the cannon for that course.
 */
typedef signed char s8;

extern s8 func_ov004_020adc1c(s8 levelID);
extern void func_ov004_020b19f0(s8 courseID);

void func_ov006_020fba48(s8 levelID)
{
    s8 courseID = func_ov004_020adc1c(levelID);
    func_ov004_020b19f0(courseID);
}
