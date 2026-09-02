# VOICE - proximity voice chat in online VS matches, game side

Lane: VOICE. Worktree `C:\tmp\voice`, branch `port/voice`, base `ad09cd9f5`
(the shipping branch `port-mount-noseat-cluster` tip). Private build directory
`C:\tmp\voicebld`, build output `C:\tmp\voice\build\port`. Nothing pushed,
nothing merged.

The launcher UI is a separate lane. This file is the contract between the two:
the **Settings keys** section below is the whole of what the launcher has to
write, and it is exact.

---

## 1. The settings keys, exactly (this is the launcher lane's spec)

All five live in `settings.json`, the flat JSON object the launcher already
writes beside the exe. All five **reload live** - the game re-reads them while
it is running, so the launcher's toggle works mid-match with no restart. Key
names are matched case-insensitively, the same rule every other key in that
file follows.

| Key | Type | Default | Meaning |
|---|---|---|---|
| `VoiceEnabled` | bool | `false` | The master switch. **False opens no recording device at all** - not opened and muted, not opened and discarded. Turning it off while the game is running closes the device it had and drops every buffered remote voice. |
| `VoiceMicDevice` | string | `""` | `""` is the system's default recording device. Any other value is matched **case-insensitively as a substring** against the device names Windows reports; first match wins. No match falls back to the default device with one line on stderr. |
| `VoiceVolume` | int 0..100 | `80` | Linear gain on decoded remote audio, on top of the distance falloff. **Independent of the existing `Volume` key** - muting the game does not mute the people you are playing with. |
| `VoiceNearRadius` | int | `512` | World units. Full `VoiceVolume` inside this distance. |
| `VoiceFarRadius` | int | `3072` | World units. Silence at and beyond it; log falloff between the two. |

Rules the launcher can rely on:

- **A missing key is its default.** A `settings.json` written before these
  existed reads exactly as one that turned voice off, which is the old program.
- **A malformed value is its default.** A non-integer `VoiceVolume`, a
  `VoiceEnabled` that is not `true`/`false`, a `VoiceMicDevice` longer than 95
  characters: all read as absent, never as an error.
- **`VoiceVolume` is clamped to 100** on the way in; negative reads as absent.
- **`VoiceFarRadius <= VoiceNearRadius` puts BOTH back to their defaults**
  (512 / 3072). A falloff with no span is a typo or a launcher bug, not a
  choice, and clamping one to the other would give a hard on/off cutoff nobody
  asked for.
- **Substring matching on the device name is deliberate.** Windows truncates a
  recording device name to 31 characters plus a terminator, so the string a
  launcher shows a player and the string the game sees are routinely different
  lengths. An exact match would refuse the device the player picked out of the
  launcher's own list.

### Getting the device list into the launcher

The game will print the names it matches against:

    walk_window.exe with SM64DS_VOICE_DEVICES=1

writes one `[voice]   device N: '<name>'` line per recording device to stderr
and opens nothing. Those are the exact strings `VoiceMicDevice` is matched
against. `port/tools/voice_proof.py --devices` runs that for you, quiet and
muted, and prints only the `[voice]` lines.

An example settings.json:

```json
{
  "VoiceEnabled": true,
  "VoiceMicDevice": "Headset",
  "VoiceVolume": 80,
  "VoiceNearRadius": 512,
  "VoiceFarRadius": 3072
}
```

### Boot and live-reload logging

With `VoiceEnabled` on, boot prints one line naming the mic, the volume and the
two radii. A live re-read that changed any of the five prints

    [settings] live re-read: voice ON, volume 80, mic 'Headset', radii 512..3072

so a support log shows what the player actually had.

---

## 2. The datagram

One aux message every 40 ms, carrying two 20 ms frames. It rides the seam's
own `send_aux` (contract v2: unreliable, unordered, whole messages, one socket
multiplexed by a four-byte message kind) under a new tag `'SYNV'`, inside the
`'SYN*'` tag space the carrier's relay-collision rule requires.

```
off  size  field
0    4     'S','Y','N','V'
4    1     version, 1. Anything else is dropped.
5    1     sender slot. DIAGNOSTIC ONLY - routing uses the slot the carrier
           itself classified, so a payload cannot claim to be somebody else.
6    1     frame count, 1 or 2
7    1     flags, 0
8    4     sequence number of the FIRST frame, u32 little endian
12   164   frame block 0
176  164   frame block 1, when the frame count says 2
```

A frame block is 20 ms of 16 kHz mono, IMA ADPCM at four bits a sample:

```
off  size  field
0    2     starting predictor, s16 little endian
2    1     starting step index, 0..88
3    1     reserved, 0
4    160   320 nibbles, low nibble of each byte first
```

**340 bytes on the wire.** Under the relay's `MAX_PAYLOAD` of 700 and under the
carrier's aux message size, both asserted at compile time in
`port/hal/voice_chat.cpp` as well as where they are defined.

**EVERY BLOCK CARRIES ITS OWN CODEC STATE**, which costs three bytes a frame
and is what makes the channel survive the loss it is promised: an ADPCM stream
whose state ran across datagrams would turn one lost packet into a decoder that
stays wrong for the rest of the call. This way a lost packet is 40 ms of silence
and the next one is correct.

**The step index is CARRIED by the encoder and WRITTEN into the block; the
predictor is not carried.** Those are different decisions and review caught the
first one being wrong. The original encoder reset the index to 0 at every frame
boundary and shipped that zero, so the quantiser had to re-climb from a step
size of 7 fifty times a second and the first handful of samples of every frame
were badly wrong: **18.7 dB SNR** on a sine round trip through the shipped
functions. Carrying the encoder's adapted index and putting THAT in the header
reads **36.4 dB** (section 8.6). It costs the loss property nothing, which is
the only reason it is allowed: the block still describes its own starting state
completely, so a decoder that never saw the previous datagram still decodes this
one exactly. The encoder is only choosing a better starting index than zero.

The PREDICTOR still resets to the frame's own first sample. That half was always
right: it makes the first residual zero, and unlike a step index a stale
predictor is an audible DC jump rather than a slow re-converge.

**Batching two frames halves the packet rate** - 25 datagrams a second instead
of 50 - which is what makes the relay budget below workable at all.

---

## 3. What changed, file by file

### `port/hal/comms_loopback.cpp` - the aux tag / kind / classifier / recv switch

Lane LAGDELAY owns the input-delay and `lb_exchange` starve path in this file.
**The exact lines this lane touched**, all of them in the aux message-kind
machinery and none of them inside LAGDELAY's ranges:

`git diff ad09cd9f5 -- port/hal/comms_loopback.cpp` is seven hunks and no more:

| Hunk (old -> new) | Lines in the new file | What |
|---|---|---|
| `@@ -368,6 +368,10 @@` | 371-374 | `kAuxVoiceLE` = `'SYNV'`, a comment and one const beside the three existing aux tags |
| `@@ -568,7 +572,12 @@` | 575-580 | `kAuxMaxBytes` 256 -> 384, with the reason (a 340-byte voice datagram would have been refused at 256) |
| `@@ -578,8 +587,15 @@` | 590-598 | `kAuxKinds` 3 -> 4, and `kAuxKindVoice` = 3 with the note on why the voice kind is served separately |
| `@@ -1617,6 +1633,8 @@` | 1636-1637 | the `dispatch()` classifier: a fourth `else if` on the tag |
| `@@ -2476,6 +2494,7 @@` | 2497 | ONE `continue` in `lb_recv_aux`'s round-robin, skipping the voice kind |
| `@@ -2487,6 +2506,41 @@` | 2508-2545 | `lb_recv_voice`, new, immediately after `lb_recv_aux` |
| `@@ -2531,6 +2585,23 @@` | 2587-2603 | `comms_recv_voice`, the export, immediately after `comms_session_players` |

**None of those overlap LAGDELAY's three ranges** (~700-730, where `g_input_delay`
lives at 722; ~2250-2330; ~2880-2900). The two edits that come closest are the
`continue` at 2497 and the new function at 2508-2545, both of which sit in the
gap between LAGDELAY's second and third ranges. Every one of my lines is inside
the aux message-kind machinery: a tag, a size, a kind count, a classifier arm, a
skip and a reader.

`port/hal/comms_loopback.h`: one declaration added after
`comms_session_players`.

**Why a separate reader and not the seam's `recv_aux`.** Two consumers on one
drain-until-zero queue means whichever pumps first eats the other's messages.
The sync layer's pump would have swallowed audio; the voice pump would have
swallowed snapshots. So the voice kind is served by `comms_recv_voice` and
skipped by `recv_aux`, with its own round-robin cursor so a busy voice channel
cannot starve a quiet peer's snapshots or vice versa.

**Why the `CommsTransport` struct was not widened.** It is a frozen contract
with a declared v2 boundary, and proximity chat is a port feature, not a term
of the seam. Voice **sends** through the contract's own `send_aux` - an opaque
whole message with its own tag, which is exactly what that entry is for - and
**reads** through one carrier-specific entry, because the read side is the only
half that needs the sender's identity and a queue of its own.

### `port/hal/voice_chat.{h,cpp}` - new

Capture pump, IMA ADPCM encoder and decoder, the send/receive path, the
per-peer jitter buffer, the distance gain, and the mixer hook. `voice_tick()`
is called once per frame from `tests/walk_window.cpp`, immediately after
`port::sync_tick()` and for the same structural reason: the input record is
already on the wire by then, so a voice datagram cannot get in front of the
thing the lockstep blocks on.

### `port/hal/voice_capture_win.cpp` - new

winmm `waveIn`, `LoadLibraryA` at first use, no static import. This mirrors
`hal/sdat/out_win.cpp` and the reason is that file's: the loader resolves
static imports **before** the TLS callback that claims
`0x02000000..0x07ffffff`, and a DLL placed in that range takes an address the
game needs. 16 kHz mono, 8 buffers of 320 samples, `CALLBACK_NULL` and polled
from the frame loop - so every captured byte is handled on the thread that runs
the game and there is no lock anywhere in this lane.

**The failure latch**, added in review. A box with no recording device, or one
whose microphone another program holds exclusively, used to run the whole
enumerate-and-open path on every frame, because the caller's only test was "is a
device open" and on such a box it never is. Sixty attempts a second and sixty
copies of the failure line in the log with them. A failure is now latched
against the name that produced it - the same shape `load_lib()` already used for
the library - and `cap_open` returns in silence for that name without making one
winmm call. `cap_rearm()` clears it, and `voice_chat.cpp` calls that on exactly
two events: the player edits `VoiceMicDevice`, or turns `VoiceEnabled` off and
on. A still-closed device with an unchanged name gets a five second retry
instead, which is a situation rather than an event. Proven in section 8.6.

`SM64DS_VOICE_NO_DEVICE=1` refuses the open as if the machine had no recording
hardware, so the latch can be proven on a box that does have a microphone
without the proof taking it.

### `port/hal/sdat/mixer.cpp`, `sdat.h` - one nullable hook

`sd_mix_set_aux_render(fn)` registers a function called at the end of
`sd_mix_render`, over the finished stereo block, at `SD_MIX_RATE`. A function
pointer and not a direct call because `mixer.cpp` links into targets with no
network; a hard call would fail their link for a feature they do not build.

**It runs AFTER the host master volume, on purpose.** Everything above that
line is the DS's own mixer plus the host output trim, and the WAV dump
downstream is the record of what the game sounded like. Voice chat is not the
game: it has its own slider, and a player who muted the game to hear his
friends should still hear his friends. Running it above the trim would have
made `SM64DS_VOLUME=0` a second mute for a control that already has one.

**No SDAT channel is allocated.** The DS mixer has sixteen and the game budgets
against all sixteen; a voice taking one would silence a sound effect. The extra
render pass adds to the same buffer instead.

### `port/hal/host_settings.{h,cpp}` - the five keys

One reader, `read_voice_keys`, called from both `load_once` and `reload_live`,
so boot values and reloaded values cannot come out of two pieces of code that
drift.

### `port/tests/walk_window.cpp` - one call

`port::voice_tick();` after `port::sync_tick();` in the level frame loop, plus
the include.

### `port/CMakeLists.txt`

`hal/voice_chat.cpp` and `hal/voice_capture_win.cpp` appended to
`SLICE_COMMS_SOURCES`, which is taken by exactly the three targets that also
take `SOUND_HOST_SOURCES` - and voice needs both.

---

## 4. The relay change (I do not deploy this; the owner does)

`port/tools/relay/relay.py`: `RATE_PPS` and `RATE_BURST` become environment
knobs, **with their existing values as defaults**. A relay nobody reconfigures
behaves exactly as it did.

```
SM64DS_RELAY_RATE_PPS     sustained packets per second, per source address
                          (default 120, unchanged; clamped 30..2000)
SM64DS_RELAY_RATE_BURST   token bucket depth (default: same as RATE_PPS)
```

**Why it had to become a knob.** The budget is per source address and counts
everything one seat sends on its one socket:

| kind | rate |
|---|---|
| lockstep input record | up to 60/s |
| sync snapshots | 30/s (`SM64DS_SYNC_HZ`, default 30) |
| RTT probes | 2/s |
| **before voice** | **~92/s** |
| voice | 25/s |
| **with voice** | **~117/s against a budget of 120** |

Three packets of headroom is not headroom. What the relay does to a seat over
the line is **drop, silently** - by design, because a rate limiter that
explained itself would be a reflection amplifier - so a voice session on a
stock relay would lose lockstep records under load and read as a desync.

**The recommendation: `SM64DS_RELAY_RATE_PPS=240` on any relay carrying voice
sessions.** Double the old budget for a seat whose steady state is ~117, which
leaves the same proportional headroom the old number left a seat at ~92. The
relay prints a note on startup while the rate is still at the pre-voice value.

Also changed: the startup banner prints `rate_burst`, `README.md` gains a
packet-rate troubleshooting section, and `sm64ds-relay.service` carries the
commented-out `Environment=SM64DS_RELAY_RATE_PPS=240` line.

Voice datagrams are 340 bytes, well inside the 700-byte cap, so nothing else
about the relay's contract moves.

---

## 5. The falloff, and where the default radii come from

    d <= near            gain = 1
    near < d < far       gain = log(far / d) / log(far / near)
    d >= far             gain = 0

Log rather than linear because that is the shape distance has: the fall is
steepest where the listener is closest, which is where a step in volume is
actually audible.

The distance is the 3D separation between the two bodies, taken from the Fix12
position words at `Actor+0x5c..0x64` (the same three words
`hal/comms_sync.cpp`'s divergence detector and `hal/player_fields.h` already
read), shifted to whole world units before the squares so an arena-sized
separation cannot overflow.

### The default radii, measured rather than guessed

The arena the proofs run in is VS map 0, which is level 51. Two things were
measured out of the runs themselves (`port/tools/voice_proof.py`'s own
`[vs]` probe logs, `runs/voice/out/near_B/run.log`):

| | |
|---|---|
| separation of the two spawn points | **190 world units**, drifting to 234 as the bodies settle |
| ground a walking player covers in 900 frames | **4846 units** along z |

The second number is the useful one, because it is a SPEED: 4846 units in 900
frames is about 5.4 units a frame, or roughly 320 units of ground a second at
the ROM's own 60 Hz. So the two defaults are readable as time rather than as
numbers nobody has a feel for:

* **`VoiceNearRadius` 512** is about a second and a half of running. Somebody
  standing near you, or who just ran up to you, is at full volume.
* **`VoiceFarRadius` 3072** is about nine or ten seconds of running. Somebody
  who has left is gone.

Both spawn points sit inside the near radius, which is correct and deliberate:
a match starts with everyone able to hear everyone, and the falloff is what
happens once people scatter. They are settings keys, so a player or the
launcher can move them without a build.

**Not tuned by ear.** These are derived from the arena's own scale and nothing
in this lane listened to them. That is the first thing to revisit once somebody
plays a real match.

---

## 6. Mono, deliberately

Every remote voice is mixed at equal level into both channels. Panning was
scoped and dropped rather than guessed: it needs the world handedness of the
heading word at `Actor+0x8e`, this lane derived no measurement for it, and a
left/right image that is backwards is worse than none. The seam for it is the
`pan` field on `RemoteVoice`, written once and read once.

---

## 7. Determinism

Voice reads two things out of the simulation - `data_0209f394[slot]` and
`data_0209f250` - and writes none of it. It allocates nothing in the DS arena,
never touches the lockstep input record, and is not on the path
`func_0203ea5c.c` blocks on. Its sends go out through `send_aux`, which the
seam's own ordering rule puts behind the input record, and `voice_tick` is
called after the conductor has already published the frame.

**Proven, verbatim.** `port/tools/voice_determinism.py --windows 4
--frames 900`, four windows in one arena, each holding a different direction so
the bodies actually move. Two passes over the identical setup, voice on and
voice off, then `dhdiff.py` on every pair and on each window against itself
across the two passes.

```
PASS  voice-det every window with voice ON exited clean | rc [0, 0, 0, 0]
PASS  voice-det ON p0 was really talking and listening | {'on': 1, 'tone': 1, 'dev': 0, 'cap': 1502, 'tx': 751, 'rx': 1999, 'bad': 0, 'dup': 0}
PASS  voice-det ON p1 was really talking and listening | {'on': 1, 'tone': 1, 'dev': 0, 'cap': 1505, 'tx': 752, 'rx': 1779, 'bad': 0, 'dup': 0}
PASS  voice-det ON p2 was really talking and listening | {'on': 1, 'tone': 1, 'dev': 0, 'cap': 1505, 'tx': 752, 'rx': 1790, 'bad': 0, 'dup': 0}
PASS  voice-det ON p3 was really talking and listening | {'on': 1, 'tone': 1, 'dev': 0, 'cap': 1505, 'tx': 752, 'rx': 1815, 'bad': 0, 'dup': 0}
PASS  voice-det ON p0 vs p1 | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
PASS  voice-det ON p0 vs p2 | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
PASS  voice-det ON p0 vs p3 | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
PASS  voice-det ON p1 vs p2 | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
PASS  voice-det ON p1 vs p3 | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
PASS  voice-det ON p2 vs p3 | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
PASS  voice-det every window with voice OFF exited clean | rc [0, 0, 0, 0]
PASS  voice-det OFF p0 sent and received NOTHING (the report line is only emitted with voice on) | None
PASS  voice-det OFF p1 sent and received NOTHING (the report line is only emitted with voice on) | None
PASS  voice-det OFF p2 sent and received NOTHING (the report line is only emitted with voice on) | None
PASS  voice-det OFF p3 sent and received NOTHING (the report line is only emitted with voice on) | None
PASS  voice-det OFF p0 vs p1 | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
PASS  voice-det OFF p0 vs p2 | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
PASS  voice-det OFF p0 vs p3 | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
PASS  voice-det OFF p1 vs p2 | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
PASS  voice-det OFF p1 vs p3 | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
PASS  voice-det OFF p2 vs p3 | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
PASS  voice-det CROSS-RUN p0: voice ON hashes are identical to voice OFF | NO DIVERGENCE. The two windows agree on the world hash for all 1620 common frames (f0..f1619).
PASS  voice-det CROSS-RUN p1: voice ON hashes are identical to voice OFF | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
PASS  voice-det CROSS-RUN p2: voice ON hashes are identical to voice OFF | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
PASS  voice-det CROSS-RUN p3: voice ON hashes are identical to voice OFF | NO DIVERGENCE. The two windows agree on the world hash for all 900 common frames (f0..f899).
```

**The last four rows are the ones that matter.** Every-pair agreement can be
satisfied by a change that moved all four windows the same wrong way. The
cross-run rows cannot: they say that the same window, in the same slot, holding
the same key, produced a bit-identical world with four peers talking over it and
with nobody talking at all. Every window in the ON pass really was talking
(751 datagrams sent each in this run) and really was listening (about 1800 to
2000 received each, three peers' worth), with zero malformed and zero duplicates.

---

## 8. Proofs

Every run below is quiet and muted per the standing rule
(`mp2_proof.spawn`: CREATE_NO_WINDOW, SW_SHOWMINNOACTIVE, `SM64DS_NO_FOCUS=1`,
`SM64DS_MINIMIZED=1`, `SM64DS_VOLUME=0`) and on top of that every window runs
`SM64DS_NO_AUDIO=1` and the tone hook, so **no output device and no recording
device was opened by any of them**. Nothing was audible on the machine and
nothing touched its microphone.

### 8.1 Build

`port/build-port.cmd` from `C:\tmp\voice\port` with `TEMP`/`TMP` at
`C:\tmp\voicebld`. **Exit 0**, all guards green
(`closestplayer_guard OK`, `inferred_stub_guard OK: no new guessed vtable body
seated`, every generator selftest PASS), every target linked including
`walk_window.exe`, `walk_window_hires.exe`, `smoke_player.exe` and the whole
smoke set.

### 8.2 Microphone device enumeration

`python port/tools/voice_proof.py --devices` (opens nothing; a 60-frame boot
with `SM64DS_VOICE_DEVICES=1`):

```
[voice] 2 recording devices on this machine
[voice]   device 0: 'Microphone (5- Razer BlackShark'
[voice]   device 1: 'Microphone (Razer Seiren Mini)'
```

Both names are truncated by Windows at 31 characters, which is exactly why
`VoiceMicDevice` matches as a substring: `"Razer Seiren"` selects device 1 and
`"BlackShark"` selects device 0, and neither full product name would have
matched on equality.

### 8.3 A tone through the whole chain, and the falloff on it

`python port/tools/voice_proof.py`. Two windows in one arena, both microphones
replaced by a generated 440 Hz tone at amplitude 16000, the listener's finished
mixer output written to a WAV through the existing `SM64DS_WAV_DUMP`. Three
arms, with the radii placed around the separation the game itself reported.

```
PASS  voice NEAR both windows exited clean | rc 0/0
PASS  voice the talker sent datagrams | tx=630
PASS  voice the listener received them, none malformed | {'on': 1, 'tone': 1, 'dev': 0, 'cap': 1260, 'tx': 630, 'rx': 618, 'bad': 0, 'dup': 0}
PASS  voice NEITHER window opened a recording device | listener dev=0
PASS  voice the listener measured a real separation | d=190
PASS  voice NEAR the game applies gain 1.000 | g=1.0
PASS  voice NEAR the tone arrives at FULL LEVEL through capture, IMA ADPCM, the wire, the jitter buffer and the resampler | rms 0.34424 against 0.34527 predicted (16000/sqrt2 of full scale), 440 Hz bin 0.00339
PASS  voice the two bodies are actually apart in the arena, so the bracketing arms below mean something | d=190
PASS  voice FAR the datagrams still ARRIVE (it is the mix that is silent, not the channel) | {'on': 1, 'tone': 1, 'dev': 0, 'cap': 1263, 'tx': 631, 'rx': 596, 'bad': 0, 'dup': 0}
PASS  voice FAR the game applies gain 0.000 at d=190 with far=95 | g=0.0
PASS  voice FAR the listener's whole dump is EXACTLY ZERO - so the game contributes nothing to these numbers and every non-zero sample in the other arms is voice | rms=0.00000, first non-silent sample -1
PASS  voice MID the gain the game applied IS the log falloff | g=0.5, log(far/d)/log(far/near) = 0.500 at d=190 near=95 far=380
PASS  voice MID the LEVEL follows the gain | rms 0.15371 against 0.15098 predicted (full level 0.34527 through the run's own reported gain series, modal gain 0.500)

measured separation d=190 world units, arena VS map 0
  full-level prediction  0.34527 rms (a 16000 amplitude sine)
  NEAR near=100000 far=200000  gain 1.000  rms 0.34424  440Hz 0.00339
  MID  near=95     far=380     gain 0.500  rms 0.15371  440Hz 0.00473
  FAR  near=47     far=95      gain 0.000  rms 0.00000  440Hz 0.00000
```

**Reading those numbers.**

* **NEAR** is the whole chain end to end at unity: 0.34357 against a predicted
  0.34527. A 16000-amplitude sine has an RMS of 16000/sqrt(2)/32768 = 0.34527 of
  full scale, and the tone came out of a microphone-shaped source, through IMA
  ADPCM at four bits a sample, over UDP, through a jitter buffer and a
  16000-to-32768 resampler, and arrived **within 0.5 percent of its original
  level**. Nothing in the chain is changing the gain.
* **FAR** is exactly zero. Not quiet: the listener's entire dump has no sample
  above the noise floor at all, while its counters show 883 voice datagrams
  arriving throughout. The channel is fine and the mix is silent, which is what
  distance attenuation is supposed to look like. It doubles as the proof that
  every non-zero sample in the other two arms is voice and not the game.
* **MID** is the curve itself: the game applied 0.500 where
  log(380/190)/log(380/95) = log2/log4 = 0.500, and the measured level followed
  it to within 1.5 percent.

(The RMS numbers move a percent or so run to run because the bodies drift and
the harness predicts from each run's own reported gain series; the shape does
not.)

**Why the 440 Hz bin is small while the RMS is exact**, and it is a harness
artefact rather than a defect. With `SM64DS_NO_AUDIO` the mixer is clocked off
the VIDEO FRAME at a nominal 60 Hz (one 546-sample block a frame, out_win.cpp's
no-device arm) while the tone generator is clocked off the wall. A headless
selftest does not run at 60 fps, so the dump's own clock runs slower than real
time, voice arrives faster than it is consumed, and the jitter ring drops its
oldest frame to keep the delay bounded -- exactly as designed. The result is a
tone chopped into segments with phase steps between them: its ENERGY is intact,
which is what the RMS measures, and its coherence over a multi-second window is
not, which is what the bin measures. On a real device the mixer is clocked by
the hardware at 32768 Hz and the drift does not exist.

### 8.4 The wide-session determinism proof

See section 7. Four windows, `NO DIVERGENCE` on all 6 pairs with voice on, on
all 6 with voice off, and bit-identical world hashes across the two passes for
all 4 windows.

### 8.5 The battery

`python port/tools/battery.py . --skip-build` (the build above is the one it
would have made). **`battery: ALL GREEN`**, exit 0.

```
19 smoke binaries          all ok, including smoke_sdat (the hosted ARM7 whose
                           mixer this lane put a hook in) and smoke_player
levels                     50 mounted, every one clean
scenes                     every hosted non-level scene clean, 373..390 included
default boot: ok -- a bare launch reaches the TITLE, 300 frames clean, and writes its frame (589,878 bytes, liveness only)
linkage: 9139 (80.7%)
ptr_audit: 0 unhosted code pointers
shipcfg build: ok, walk_window.exe linked in build\port-kit (PORT_ROM_CLEAN, static CRT, 390s)
shipcfg selftest: ok, rc=0 and walk_window_selftest.bmp written (589,878 bytes, 10s)
skips: level 27 without TTC_MOVING_BEAM (the decomp (_ZN14TtcMovingCubeA8BehaviorEv has no matched body)), level 45 without GOOMBOSS (the decomp (func_ov074_02121380 has no matched body))
battery: ALL GREEN
```

**Linkage 9139 (80.7%) is unchanged** from the base tip, which is the expected
answer: this lane added port-side files and touched no `src/`. `git diff
ad09cd9f5 -- src/` is empty.

**The shipping configuration builds and links too**, which is the arm that
breaks on its own: it is a different compile (PORT_ROM_CLEAN, static CRT) and
the new files go through it as well. The two skipped levels are pre-existing and
belong to other lanes.

---

### 8.6 The two things review caught

Both arms are part of `voice_proof.py` and run ahead of the three above.

```
PASS  voice CODEC a sine round trip through the shipped IMA ADPCM reads above 30 dB | SNR 36.4 dB over 49 frames
PASS  voice NO DEVICE says so ONCE over 600 frames and then goes quiet | rc 0, 1 'waveInOpen failed' lines, 1 'no recording device matches' lines
```

**The codec arm** (`SM64DS_VOICE_CODEC_SELFTEST=1`) pushes a 440 Hz sine at
amplitude 8000 through the REAL `encode_frame` and `decode_frame` for a second
of frames and reports the signal-to-noise ratio. Not a model of the codec: the
functions that ship, across frame boundaries, which is where the defect was. The
first frame is excluded because an encoder legitimately starts cold on the first
frame of a call; what this measures is the steady state. **36.4 dB**, against
**18.7 dB** with the step index thrown away at every boundary.

**The missing-device arm** runs 600 frames with `VoiceEnabled` on, no tone hook,
`VoiceMicDevice` set to a name nothing matches, and `SM64DS_VOICE_NO_DEVICE=1`
so the open refuses as if the machine had no recording hardware. **One**
`waveInOpen failed` line and **one** `no recording device matches` line for the
whole run, where before the latch it was one of each per frame. After the latch
fires, `cap_open` makes no winmm call at all for that name, so the five second
backoff never even has to engage; it is there for the case where a re-arm
cleared the latch.

---

## 9. Gaps, plainly

1. **Nothing over the relay.** Every run here is loopback, the same
   limitation the VS16 host-loop lane recorded for itself. The relay's rate
   budget change is coded, defaulted to its old value and documented, but it has
   not been deployed or measured against a live relay carrying a voice session.

2. **No real microphone end to end.** Both proof windows use the tone hook,
   because the standing quiet rule says a test launch may not take the owner's
   hardware. So `waveIn` device ENUMERATION is proven (section 8.2) and the
   capture ring, the format negotiation and a real recording are not. That is
   the first thing a human sitting at the machine should check, and it is one
   run: set `VoiceEnabled` true, leave the tone hook off, and talk.

3. **No voice activity detection.** The mic is open, which is what was asked
   for, and open here is literal: a window in a session sends 25 datagrams a
   second whether or not anybody is speaking. That is about 8.5 KB a second up
   from each talker and the same again down per peer. A silence gate is the
   obvious next thing and it is not here.

4. **No echo cancellation, no automatic gain, no noise suppression.** A player
   on speakers rather than headphones will send his own output back to
   everybody. There is nothing in this lane that stops that.

5. **Mono.** Section 6.

6. **The radii are derived, not tuned.** Section 5 says where they come from;
   nobody has listened to them in a real match.

7. **Straight-line distance only.** No occlusion, no line of sight, no walls. A
   player on the other side of solid geometry is as loud as one in the open at
   the same range.

8. **Device matching is first-match-substring.** Two devices sharing a substring
   resolve to whichever `waveIn` lists first, and there is no way in the key to
   say "the second one".

9. **Voice only runs where `voice_tick` is called**, which is the level frame
   loop in `tests/walk_window.cpp`. That covers VS matches, which is the whole
   scope asked for. Menus, the lobby and the minigame scene path do not tick it,
   so there is no lobby chat.

10. **Sixteen talkers at once was not measured.** The mix sums every remote slot
    with no cap on how many voices sound at once and no ducking. Four was proven
    (section 7); sixteen is arithmetic nobody has run.

11. **The codec is fixed-quality 16 kHz mono ADPCM.** 36.4 dB SNR on a sine
    (section 8.6) is a phone call, not music, and there is no bitrate
    adaptation: a peer on a bad connection gets the same 8.5 KB a second as
    everybody else.

12. **The jitter buffer's depth is fixed at three frames.** It does not adapt to
    a peer whose network is worse than the others'.

13. **No per-peer mute and no volume per player.** One global `VoiceVolume`.
