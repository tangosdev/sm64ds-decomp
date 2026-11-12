/* THE PACE LOCK (run coopfix1, lane HITCH1). See hal/pace_lock.cpp. */
#ifndef PORT_HAL_PACE_LOCK_H
#define PORT_HAL_PACE_LOCK_H

#ifdef __cplusplus
extern "C" {
#endif

/* The pacer's step for one turn, in QPC counts. `step` is the budget the
   ROM's divider asks for (16.65 ms per vblank); when the display's refresh
   period R and a trusted vblank time are known and `step` is within 0.6% of
   a whole number k of refreshes, the answer is k * R, otherwise `step` itself. */
long long pace_lock_step(long long step, long long qpf);

/* The correction to take off the next deadline so that a picture presented
   `work_ms` after the turn's start lands in the middle of a refresh; 0 when
   not locked. Small (at most a quarter of a millisecond a turn). */
long long pace_lock_slew(long long next, long long qpf, double work_ms);

/* Every picture handed to the window, right after the hand-over. `repeat` is
   1 for a picture that repeats the one before. Feeds the [pace] summary and
   the SM64DS_PACE_LOG lines. */
void pace_lock_note_present(int repeat, int tick);

/* One pacer turn: the turn's wait in ms, the tick's work in ms and whether
   the turn overran its deadline. Feeds the summary. */
void pace_lock_note_turn(double work_ms, double slept_ms, int overran, int tick);

#ifdef __cplusplus
}
#endif

#endif
