/* ExitLevel at 0x02029bd4
 * Exits the current level: sets next level to 1 (hub world), then starts the exit wipe.
 */

extern void SetNextLevel(int levelID);
extern void StartExitCharacterWipe(void);

void ExitLevel(void)
{
    SetNextLevel(1);
    StartExitCharacterWipe();
}
