/* func_020308b4 at 0x020137fc
 * Converts a sublevel ID to a course ID, then opens the cannon for that course.
 */
typedef signed char s8;

extern s8 func_0203d854(s8 levelID);
extern void func_0203d868(s8 courseID);

void func_020308b4(s8 levelID)
{
    s8 courseID = func_0203d854(levelID);
    func_0203d868(courseID);
}
