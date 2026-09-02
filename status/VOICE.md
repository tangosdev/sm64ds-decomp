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

**Every block carries its own codec state.** That costs three bytes a frame and
is what makes the channel survive the loss it is promised: an ADPCM stream
whose predictor ran across datagrams would turn one lost packet into a decoder
that stays wrong for the rest of the call. This way a lost packet is 40 ms of
silence and the next one is correct.

**Batching two frames halves the packet rate** - 25 datagrams a second instead
of 50 - which is what makes the relay budget below workable at all.

---

## 3. What changed, file by file

### `port/hal/comms_loopback.cpp` - the aux tag / kind / classifier / recv switch

Lane LAGDELAY owns the input-delay and `lb_exchange` starve path in this file.
**The exact lines this lane touched**, all of them in the aux message-kind
machinery and none of them inside LAGDELAY's ranges:

| Lines (post-change) | What |
|---|---|
| 372-376 | `kAuxVoiceLE` = `'SYNV'`, beside the three existing aux tags |
| 585-590 | `kAuxMaxBytes` 256 -> 384, with the reason (a 340-byte voice datagram would have been refused at 256) |
| 596-607 | `kAuxKinds` 3 -> 4 and `kAuxKindVoice` = 3, with the note on why the voice kind is served separately |
| 1653-1654 | the `dispatch()` classifier arm: a fourth `else if` on the tag |
| 2504 | one `continue` in `lb_recv_aux`'s round-robin, skipping the voice kind |
| 2518-2557 | `lb_recv_voice`, new, immediately after `lb_recv_aux` |
| 2600-2617 | `comms_recv_voice`, the export, immediately after `comms_session_players` |

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

PROOFS_RADII_PLACEHOLDER

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

PROOFS_DETERMINISM_PLACEHOLDER

---

## 8. Proofs

PROOFS_PLACEHOLDER

---

## 9. Gaps, plainly

GAPS_PLACEHOLDER
