/* func_ov006_020fba48 at 0x020fba48
 * Draws the high score: the stored best comes back from func_ov004_020adc1c
 * as a full word and goes straight to the HUD's number drawer. Render hands
 * every drawer the scene; this one does not read it.
 */

extern int func_ov004_020adc1c(void);
extern int func_ov004_020b19f0(int score);

void func_ov006_020fba48(void *scene)
{
    func_ov004_020b19f0(func_ov004_020adc1c());
}
