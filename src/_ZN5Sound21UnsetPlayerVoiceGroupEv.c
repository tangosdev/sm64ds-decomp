/* Sound::UnsetPlayerVoiceGroup() at 0x02011fe8
 * Free function in namespace Sound (no `this`). Only clears a variable to 0;
 * the actual group unload is done by Sound::ResetPlayerVoiceGroup. See Sound.h.
 *
 * Writes 0 to the loaded-voice-group byte at 0x0209b478. The friendly symbol
 * is not yet in symbols.txt (the reloc is a wildcard pooled-global reloc, so
 * this extern name is not byte-verified); kept as a named extern over the
 * raw data_0209b478 label.
 */

extern unsigned char data_0209b478; /* 0x0209b478 */

void _ZN5Sound21UnsetPlayerVoiceGroupEv(void)
{
    data_0209b478 = 0;
}
