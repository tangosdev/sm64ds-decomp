/* Proximity voice chat for online VS matches. Lane VOICE.
 *
 * WHAT IT IS. An open microphone -- no push to talk -- whose volume on every
 * other player's machine falls off with the distance between the two bodies
 * in the arena. The player next to you is loud, the player across the map is
 * not there at all, and nothing in between is a menu decision.
 *
 * WHAT IT IS NOT, and this is the load-bearing half: it is not part of the
 * game. It reads two things out of the simulation -- the per-slot Player
 * pointers at data_0209f394 and which slot this console is at data_0209f250 --
 * and it reads them to decide a VOLUME. It writes nothing back, it allocates
 * nothing inside the DS arena, it never touches the lockstep input record, and
 * it is not on the path func_0203ea5c.c blocks on. A build with voice on and a
 * build with voice off run the same simulation frame for frame, and the wide
 * session proof is what asserts that rather than this paragraph.
 *
 * -----------------------------------------------------------------------
 * THE DATAGRAM
 * -----------------------------------------------------------------------
 * One aux message per 40 ms, carrying two 20 ms frames. It rides the seam's
 * own send_aux (contract v2: unreliable, unordered, whole messages, one socket
 * multiplexed by a four-byte message kind) with the tag 'SYNV', which keeps it
 * inside the 'SYN*' tag space the carrier's relay-collision note requires.
 *
 *   off  size  field
 *   0    4     'S','Y','N','V'
 *   4    1     version, 1. A version this build does not know is dropped.
 *   5    1     the sender's slot. DIAGNOSTIC ONLY -- routing uses the slot the
 *              carrier itself classified, so a payload cannot claim to be
 *              somebody else.
 *   6    1     frame count, 1 or 2
 *   7    1     flags, 0
 *   8    4     sequence number of the FIRST frame, u32 little endian. Frames
 *              are numbered from the start of the run; the receiver uses it to
 *              notice a gap and to refuse a duplicate.
 *   12   164   frame block 0
 *   176  164   frame block 1, when the frame count says 2
 *
 * A frame block is 20 ms of 16 kHz mono, IMA ADPCM at four bits a sample:
 *
 *   off  size  field
 *   0    2     the block's starting predictor, s16 little endian
 *   2    1     the block's starting step index, 0..88
 *   3    1     reserved, 0
 *   4    160   320 nibbles, low nibble of each byte first
 *
 * EVERY BLOCK CARRIES ITS OWN CODEC STATE, which costs three bytes a frame and
 * is what makes the channel survive the loss it is promised. An ADPCM stream
 * whose state ran across frames would turn one lost datagram into a decoder
 * that stays wrong until the call ends; this way a lost datagram is 40 ms of
 * silence and the next one is correct.
 *
 * 340 bytes on the wire, against the relay's 700-byte cap and the carrier's
 * 384-byte aux message size. 25 datagrams a second per talking peer.
 *
 * -----------------------------------------------------------------------
 * THE SETTINGS
 * -----------------------------------------------------------------------
 * VoiceEnabled, VoiceMicDevice, VoiceVolume, VoiceNearRadius and
 * VoiceFarRadius, all of them in settings.json and all of them reloading live.
 * hal/host_settings.h carries the exact spec; port/status/VOICE.md carries it
 * again for the launcher lane.
 *
 * -----------------------------------------------------------------------
 * THE TEST HOOKS
 * -----------------------------------------------------------------------
 *   SM64DS_VOICE_TEST_TONE=1   a generated 440 Hz sine replaces the
 *                              microphone. NO CAPTURE DEVICE IS OPENED, which
 *                              is what lets a headless two-window proof run
 *                              with no recording hardware at all.
 *   SM64DS_VOICE_DEVICES=1     dump the machine's recording devices, one per
 *                              line, at first use.
 *   SM64DS_VOICE_REPORT=1      one line per second: frames captured, sent,
 *                              received, dropped, and the per-slot distance
 *                              and gain the mix is using.
 *
 * The mixed result reaches the existing SM64DS_WAV_DUMP file, because voice is
 * rendered into the same stereo block hal/sdat/mixer.cpp hands that dump. No
 * second dump knob was needed and none was added.
 */
#ifndef PORT_HAL_VOICE_CHAT_H
#define PORT_HAL_VOICE_CHAT_H

namespace port {

// Once per frame from the host loop, beside sync_tick and after it. Reads the
// settings, opens or closes the capture device to match, drains the
// microphone, encodes and sends, receives and decodes, and recomputes every
// remote slot's distance gain. Costs nothing measurable with VoiceEnabled
// false: one settings read and a return.
void voice_tick();

// The report line SM64DS_VOICE_REPORT asks for, on demand. Called by the same
// places that call sync_report.
void voice_report(const char *tag);

// ---- THE CAPTURE BACKEND, hal/voice_capture_win.cpp --------------------
// Split out so the winmm half is one small file with no opinions about the
// game, and so a future WASAPI backend replaces one file rather than editing
// this one. Every entry is a no-op that answers "nothing" on a platform with
// no backend.
namespace voice {

enum : int { kCapRate = 16000, kCapFrameSamples = 320 };   // 20 ms

// Open the named recording device, or the system default for "" or null.
// Returns 1 when a device is live. Safe to call when one already is: the same
// name is a no-op and a different name closes and reopens.
int cap_open(const char *device_name);
void cap_close();
int cap_is_open();

// Clear the "this name cannot be opened" latch. cap_open refuses in silence
// after a failure so that a box with no recording device does not run the
// enumerate-and-open loop sixty times a second; this is how the caller says
// the situation changed and it is worth another try. Called when the player
// edits VoiceMicDevice or turns VoiceEnabled off and on, never on a timer --
// the retry interval is the caller's, so the policy lives in one place.
void cap_rearm();

// One 20 ms frame of 320 mono s16 samples at 16 kHz into `out`. Returns 1 when
// a frame was produced and 0 when nothing is ready. Never blocks. A caller
// that wants everything pending loops until it returns 0.
int cap_read_frame(short *out);

// Write up to `max` device names into `names`, NUL terminated, and return how
// many the machine has (which can exceed `max`). Opens nothing.
// ONE SIZE FOR THE WHOLE FEATURE. This buffer holds a device name coming back
// from Windows AND the VoiceMicDevice value going in, and host_settings.cpp
// stores that value in 96 bytes. Two different sizes meant a name the settings
// layer accepted could be silently truncated on the way here and then never
// compare equal to itself, which is a reopen on every poll. Windows truncates
// its own names to 31 characters plus a terminator, so 96 is generous for the
// half that comes back and exact for the half that goes in.
enum : int { kCapNameBytes = 96 };
int cap_enumerate(char names[][kCapNameBytes], int max);

}  // namespace voice
}  // namespace port

#endif  // PORT_HAL_VOICE_CHAT_H
