/* HOST COPY of src/func_ov002_020bd480.c -- cutscene-object command 4, the one
 * that plays a character voice.
 *
 * WHY IT IS HOSTED NOW. hal/intro_kuppa_dispatch.cpp trapped this command with
 * the note "needs Sound::PlayCharVoice (unhosted)", and left the trap LOUD on
 * purpose: "The opening was not expected to reach it; that it did is the
 * finding." With the script chain running, the opening reaches it, and the
 * finding is that the premise was stale rather than that the command is exotic:
 *
 *     src/_ZN5Sound13PlayCharVoiceEjjRK7Vector3.cpp   port/slice_gate10.txt:1225
 *     port/unmatched/Player_Behavior.cpp:213/220      already calls it
 *
 * so the body IS in this binary and has been. Nothing new is dragged in.
 *
 * WHY A HOST COPY RATHER THAN THE SRC LINE. The matched TU declares its callee
 * inside a namespace:
 *
 *     namespace Sound { void PlayCharVoice(unsigned, unsigned, const Vector3 &); }
 *
 * which MSVC mangles as ?PlayCharVoice@Sound@@YAXIIABUVector3@@@Z, while the one
 * real definition carries the Itanium name with C linkage. hal/cxx_aliases.cpp
 * carries two /alternatename bridges for this symbol and neither spells that
 * form. Declaring it the way port/unmatched/Player_Behavior.cpp already does --
 * extern "C", by its real name -- needs no new alias and no edit to a shared
 * alias file, and it is the pattern this tree already uses for exactly this.
 *
 * The body is the matched source line for line: the sound id out of the
 * command's unaligned argument word, the character id from the player's +0x6d9
 * (which tests/walk_window.cpp's own note calls the voice-bank byte), and the
 * position at +0x74.
 */
extern "C" {

unsigned int ReadUnalignedInt(unsigned char *p);
/* The real symbol, spelled the way Player_Behavior.cpp spells it. A reference
   argument and a pointer argument are the same word on this ABI. */
void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int charID,
                                           unsigned int soundID,
                                           const void *pos);

int func_ov002_020bd480(unsigned char *c, unsigned char *p)
{
    unsigned int v = ReadUnalignedInt(p);
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(c[0x6d9], v, (const void *)(c + 0x74));
    return 1;
}

}
