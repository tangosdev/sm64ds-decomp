/* THE PLAYER PERFORMANCE LOG (run perf1, the game half).
 *
 * WHAT IT IS. One small text file per session, `playlog/perf-<id>.jsonl`,
 * written next to the flight recorder's own `playlog/play_<id>.log` and
 * carrying the SAME <id>, so a report and its prose log pair by filename with
 * nothing to cross-reference. One JSON object per line, ASCII only: a header
 * line naming the machine and the picture settings, then one sample line
 * every five seconds of wall clock, then an end line at an orderly exit.
 * The launcher attaches it to the report a player already opts into, so
 * "the game runs badly on my PC" arrives with the numbers attached.
 *
 * WHAT IT IS NOT. Not a second measurement path. Every number on a sample
 * line is arithmetic over values the F3 overlay already computes each frame
 * (the PhaseClock spans, the two rate windows, the cpu share, the graphics
 * card's own timer totals). The accumulator runs whether or not F3 is shown,
 * because the overlay is a thing a player turns on and this is a thing that
 * has to be true of the session they actually played. Nothing here touches
 * rendering, presenting, pacing or input: the per-frame cost is a handful of
 * adds, and once every five seconds one buffered line.
 *
 * NO NAMES, NO PATHS. Hardware model strings are the point of the file and
 * are written as the machine reports them; nothing else about the machine is.
 * No user name, no computer name, no directory, no serial, no address. The
 * launcher scrubs the file again on its way out; this file gives it nothing
 * to scrub.
 *
 * THE SWITCHES.
 *   SM64DS_NO_PERFLOG=1        no perf file; the flight recorder is untouched.
 *   SM64DS_PERFLOG_FORCE=1     write one even on a run that has no flight
 *                              recorder (a selftest, or SM64DS_NO_PLAYLOG).
 *                              Test-only: it exists so a gate row can read a
 *                              real file out of a headless run.
 *   SM64DS_PERFLOG_CAP_SECONDS=<n>  shorten the four-hour sampling cap, so the
 *                              cap arm is provable in twenty seconds instead
 *                              of four hours. Test-only.
 */
#ifndef PORT_PERF_LOG_H
#define PORT_PERF_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/* One frame's worth of what the overlay already knows, handed in by the frame
 * pump. The five phase figures and the three rates are the overlay's own
 * smoothed/windowed values; frame_ms is the raw span of the frame that just
 * ended, because a p95 over smoothed numbers would be a p95 of a low-pass
 * filter rather than of the frames a player felt.
 *
 * THE SNAPSHOT FIELDS cost real work to read -- a walk of the behaviour list,
 * a working-set query, a client rect, the card's totals -- so they are filled
 * ONLY on the frame that closes a five-second bucket, which the caller learns
 * from port_perf_log_due(). On every other frame `snapshot` is 0 and this
 * module keeps the last values it was given. */
struct PortPerfFrame {
    double frame_ms;        /* raw span of the frame that just ended */
    double input_ms;        /* the five PhaseClock means, in loop order */
    double camera_ms;
    double submit_ms;
    double raster_ms;
    double blit_ms;
    double fps;             /* pictures per second handed to the window */
    double tick_rate;       /* simulation ticks per second */
    double cpu_pct;         /* this process, the figure Task Manager shows */
    int    gpu_on;          /* a setting has the graphics card doing work */
    double gpu_ms_tick;     /* card busy ms per game tick */
    double gpu_pct;         /* the same as a share of the tick's budget */
    double gpu_read_ms;     /* the readback wait per game tick */

    int    snapshot;        /* 1 when the fields below were filled this frame */
    /* WHERE THE PLAYER IS. A level's own id while a level is running, and
     * -1 - <scene id> while the title, the menus or a minigame are (the
     * scene loop and the level loop are two different loops and their ids
     * are two different numbering spaces, so the negative half keeps them
     * apart in one field). The triage table that reads the report maps both
     * halves onto a class. */
    int    level;
    int    tris;            /* polygons gx accepted */
    int    actors;          /* live entries on the behaviour list */
    unsigned mem_kb;        /* working set */
    int    window_w;        /* client area, -1 when there is no window */
    int    window_h;
    unsigned long long gpu_dropped_total;  /* cumulative; differenced here */
};

/* Open the file. `playlog_path` is the flight recorder's own filename
 * (`playlog/play_<id>.log`), whose <id> becomes this file's; pass 0 when no
 * recorder opened, in which case nothing is written unless
 * SM64DS_PERFLOG_FORCE=1 asked for it and an id is made the same way.
 * Cheap and allocation-free: it claims a name and registers the end line.
 * The header line is built on the first frame instead of here, on purpose --
 * see the banner over perf_write_header in the .cpp. */
void port_perf_log_open(const char *playlog_path);

/* 1 when a file is open and still sampling. The frame pump's one cheap test. */
int port_perf_log_on(void);

/* 1 when the frame about to be handed in closes a bucket (or is the first
 * frame, which writes the header), so the caller knows to pay for the
 * snapshot fields. */
int port_perf_log_due(void);

/* Fold one frame in, and write a line if this frame closed a bucket. */
void port_perf_log_frame(const struct PortPerfFrame *f);

/* The end line. Registered with atexit by port_perf_log_open, so an orderly
 * exit writes it and a crash leaves the file ending at its last sample --
 * which is itself the useful signal. Safe to call twice. */
void port_perf_log_close(void);

#ifdef __cplusplus
}
#endif

#endif  /* PORT_PERF_LOG_H */
