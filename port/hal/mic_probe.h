/* Microphone-capture probe. Host-layer only, env-gated, ships inert.
 *
 * WHY THIS EXISTS. The shipped voice panel is hidden in this release because
 * voice is not fixed, and the fix is stuck behind a circular dependency: to
 * fix voice we need to know which capture formats the tester's microphone
 * actually supports, but the only way to exercise the microphone today is
 * through the voice panel, which is the thing being hidden. This probe breaks
 * the circle. It opens the default recording device across a LADDER of common
 * formats and writes down, for each, whether the open succeeded and, when it
 * failed, the exact winmm error -- so an UNSUPPORTED-FORMAT failure reads
 * differently from a device-in-use or a permission/privacy failure.
 *
 * IT MEASURES, IT DOES NOT FIX. It does not change the live capture format in
 * hal/voice_capture_win.cpp and it does not add a fallback ladder to the real
 * path. It only produces the data from which the correct ladder can be built.
 *
 * IT IS INERT UNLESS ASKED. mic_probe_run_if_enabled() reads SM64DS_MIC_PROBE
 * once. Unset (or not "1") it returns 0 immediately, having touched no audio
 * library and changed nothing -- a normal play session and a normal battery
 * run are byte-for-byte what they were without this file. Set to "1" it runs
 * the ladder, writes mic_probe.log next to the exe (and mirrors every line to
 * stderr), and returns 1 so the caller can end the run: the flag means "test
 * my microphone", not "play the game".
 */
#ifndef PORT_HAL_MIC_PROBE_H
#define PORT_HAL_MIC_PROBE_H

namespace port {
namespace voice {

// If SM64DS_MIC_PROBE=1, run the capture-format ladder against the default
// recording device, log the result of every attempt, and return 1. Otherwise
// touch nothing and return 0. Meant to be the first call in main(): a caller
// that gets 1 back should exit cleanly rather than start the game.
int mic_probe_run_if_enabled();

}  // namespace voice
}  // namespace port

#endif  // PORT_HAL_MIC_PROBE_H
