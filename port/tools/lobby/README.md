# sm64ds-lobby

Room state and chat for the PC port's Multiplayer button. A stdlib-only
python3 HTTP service in its own container, holding a handful of rooms in
memory. No database, no disk, no credential, no outbound request.

**It never speaks to the relay and the relay never speaks to it.** A lobby
outage cannot touch a match that is already running: a match's only server
dependency is the relay. That separation is the whole reason this is its own
container instead of a handler bolted into `port_ingest`, which has write
access to the release zips every launcher downloads and runs.

**Stage A** was create, join, poll, chat, params, kick, leave, health.
**Stage B** adds `start` (host arms a match), `ready` (a launcher confirms it can
spawn it), and `failed` (any playing member aborts it back to lobby). NOTHING
spawns a game until every playing seat has readied, and the seat count is frozen
at `start` and forced into every launch plan as `SM64DS_VS_PLAYERS` -- the
join-race guarantee. `result` arrives in stage C and answers `404 unknown_verb`
today.

---

## Layout

| file | what |
|---|---|
| `app/server.py` | the whole service |
| `Dockerfile` | three lines, same shape as `port-ingest/Dockerfile` |
| `docker-compose.yml` | the deploy unit, and every limit as an env knob |
| `test_client.py` | over-the-wire `selftest`, `negatives` and `soak` |
| `test_units.py` | the grammar, the caps, the lifecycle, the limiters, in process |

---

## Running it

Locally, with nothing else needed:

```
LISTEN_PORT=8091 LISTEN_ADDR=127.0.0.1 python3 app/server.py
curl -s http://127.0.0.1:8091/port/lobby/health
```

The tests spawn their own server on a pid-derived port and need no arguments:

```
python3 test_units.py                       # no sockets at all
python3 test_client.py selftest             # spawns a server, proves the wire
python3 test_client.py negatives --out DIR  # writes one file per refusal
python3 test_client.py dial                 # the dial, on the DEPLOYED knobs
python3 test_client.py soak --seconds 60
python3 test_client.py selftest --url https://tangos.dev/port/lobby
```

---

## THE WIRE CONTRACT, v1–v2

### Rules that apply to every request

| rule | value | on violation |
|---|---|---|
| method | `POST` for all verbs; `GET` for `/health` only | 405 / 404, connection closed |
| URL | exactly `/port/lobby/<verb>`, no query string ever | 404 `unknown_verb` + one log line |
| content type | `application/json` | 415 `bad_content_type` |
| `Content-Length` | REQUIRED, and parsed **before any rejection** | missing -> 411 `length_required`, close |
| request body | <= 4096 bytes | 413 `too_large`, connection closed (body not read) |
| JSON shape | one object; no arrays at the top level | 400 `bad_shape` |
| `v` | REQUIRED, integer, `1` to `2` (see below) | 400 `bad_version` |
| unknown top-level key | rejected | 400 `bad_field` |
| missing required key | rejected | 400 `bad_field` |
| wrong type for a key | rejected | 400, the field's own code |
| any string field | length checked **in bytes** before anything else | 400 `too_long` |

Error bodies are `{"v":1,"error":"<code>"}`. Codes are written with underscores
(`bad_field`, `bad_text`, `unknown_verb`), which is the form the log lines use.

**Strictness is deliberate.** An unknown key is a 400, not an ignore. The
launcher and the server ship together and are versioned together, so a field
that appears without a version bump is a bug or an attack and there is no third
option. Adding a field is a `v` bump.

**The version is a RANGE, and the answer echoes what the request claimed.**
`v` may be anything from `contract_min` (1) to `contract_v` (2), both reported
by `/health`. A hard pin would have turned "adding a field is a `v` bump" into
"every restart refuses every launcher already in the wild", which is the
opposite of what the rule protects. The strictness stays where it belongs:
**each verb offers a v1 client exactly the v1 field set**, so a request that
says `"v":1` and then carries a v2 field is `bad_field`, exactly as it would be
for a field nobody has ever defined. A v1 request gets a v1 answer.

| version | what it adds |
|---|---|
| v1 | everything through launcher 0.3.0 |
| v2 | `params` may carry `match_players` — the host's player-count dial |

**The keep-alive drain trap, and why a refusal closes the connection.** With
`HTTP/1.1` keep-alive, returning a rejection *without reading the body the
client announced* leaves those bytes in the socket, and the next read parses
the middle of that body as a request line — so one rejected request could
appear twice, once as its real status and once as a phantom 400. `port_ingest`
handles this by draining the announced body before answering. This service does
**not** drain: a refusal closes the connection instead. Draining is a hole here
— an over-budget sender could open one slow body and the 429 meant to shed it
would block on the very bytes it was refusing — so every refusal sets
`Connection: close` and answers at once, and the unread body dies with the
socket. `Content-Length` is still parsed first, before any rejection is
written, so the refusal path always knows there is a body to walk away from.

**Every blocking socket read is bounded.** `HANDLER_TIMEOUT_S` (20 s) is set on
the connection, so a client that announces a body and then sends it one byte a
decade is dropped rather than holding its worker thread forever — the failure
the review took from 2 to 202 threads. A long poll does not ride this deadline:
it blocks on a condition variable, not on a socket read, and is capped by the
`wait` clamp and `MAX_WAITERS` instead.

### Field grammar

| field | type | rule | reject code |
|---|---|---|---|
| `v` | int | `1..2` (the contract range `/health` reports) | `bad_version` |
| `room` | string | exactly 6 chars from `ABCDEFGHJKMNPQRSTUVWXYZ23456789` | `bad_room` |
| `token` | string | exactly 32 chars from `0-9a-f` | `bad_token` |
| `nick` | string | 1..16 **bytes**, every byte `0x20..0x7E` **except `,`**, not all spaces, no leading/trailing space | `bad_nick` |
| `text` | string | 1..200 **bytes**, every byte `0x20..0x7E` | `bad_text` |
| `cursor` | int | 0 .. 2^31-1 | `bad_cursor` |
| `wait` | int | 0..25 seconds; **clamped, not rejected** | - |
| `map` | int | 0..3 | `bad_map` |
| `win_mode` | string | exactly `"time"` or `"stars"` | `bad_win_mode` |
| `star_target` | int | 1..`STAR_TARGET_MAX`; required iff `win_mode == "stars"`, forbidden otherwise | `bad_star_target` |
| `seat` | int | 1..`MAX_SEATS`, an occupied seat that is not the host's | `bad_seat` |
| `match_players` | int | 2..`dial_max`; optional on `params`, and **v2 only** | `bad_match_players` |
| `pre_ok` | bool | `true`/`false`; optional on `create` and `join` | `bad_field` |

A string longer than its cap answers `too_long` rather than the field's own
code, because the length check runs first and never lets an oversized field
reach a grammar loop.

`nick` and `text` are **opaque**. They are stored as sent, echoed back as sent,
never parsed, never logged, never used in a file name and never interpolated
into anything. The launcher renders them as plain text into a read-only
`TextBox` and does nothing else with them.

**Why `,` is barred from a nickname and nowhere else.** The four nicknames
reach the game in one environment variable, `SM64DS_VS_NAMES`, as four
comma-separated fields (stage B). Comma is the separator, so barring one
character from a nickname is cheaper than any escaping scheme, and the launcher
filters it as the player types so nobody ever meets the rejection. Chat text
never leaves the lobby and keeps its commas.

### Verbs

| verb | who may send | effect |
|---|---|---|
| `POST /port/lobby/create` | anyone | make a room, become its host |
| `POST /port/lobby/join` | anyone with a room code | take a seat |
| `POST /port/lobby/poll` | a seated member | read the room; the only push channel |
| `POST /port/lobby/chat` | a seated member | say one line |
| `POST /port/lobby/params` | **host only** | set map / win condition |
| `POST /port/lobby/preflight` | a seated member | correct its OWN `pre_ok` in place |
| `POST /port/lobby/start` | **host only** | arm a match (stage B) |
| `POST /port/lobby/ready` | a playing member | "my launcher can spawn this match" (stage B) |
| `POST /port/lobby/failed` | a playing member | report a match that could not run (stage B) |
| `POST /port/lobby/kick` | **host only** | remove a member |
| `POST /port/lobby/leave` | a seated member | give up the seat (host: close the room) |
| `GET  /port/lobby/health` | anyone | liveness, counts, revision |

Stage C adds `result`. Any other path under `/port/lobby/` is a 404 and one log
line.

#### `create`

```
-> {"v":1, "nick":"tango", "pre_ok":true}
<- 200 {"v":1, "room":"K7QMR3", "token":"<32 hex>", "member":1,
        "cursor":1, "view":{...}}
```

Mints an unused room code (8 tries, then 503 `busy`) and seats the caller as
seat 1, the host, playing. `map=0`, `win_mode="time"`, no `star_target`,
`state="lobby"`. 429 `too_fast` over the per-address create budget; 503 `full`
at `MAX_ROOMS`.

`pre_ok` is optional and defaults to `false`. It says the launcher's own
pre-flight passed (ROM found, assets unpacked, `walk_window.exe` present), so
the host's Start button in stage B never has to wait on a first-time guest's
minute of asset extraction inside a twenty-second arming window.

#### `join`

```
-> {"v":1, "room":"K7QMR3", "nick":"opie", "pre_ok":true}
<- 200 {"v":1, "room":"K7QMR3", "token":"<32 hex>", "member":2,
        "cursor":7, "view":{...}}
```

404 `no_such_room` if unknown or closed; 403 `kicked` if this address was
kicked from this room inside the cooldown; 409 `room_full` at `MAX_SEATS`;
409 `in_match` if the room is not in the lobby state.

Seats are numbered 1..`MAX_SEATS` and a joiner takes the lowest free one. A
joiner plays while fewer than `GAME_MAX_PLAYERS` seats already do; the rest are
spectators, who chat and watch. When a playing member leaves, the
lowest-numbered spectator is promoted, and that reaches both windows in the
`view` every poll carries rather than as a new event kind.

**Nickname collision.** The stored `nick` is left exactly as sent. The server
adds a `display` string, suffixing later claimants of the same nickname with
their seat (`"opie (2)"`), capped at 20 bytes. The launcher never does this
itself, so both windows show the same words. The server also derives
`game_name` per member (`display` truncated to 16 bytes), which is what stage B
puts in `SM64DS_VS_NAMES` — derived here precisely so that no launcher ever
builds it and two launchers cannot disambiguate a duplicate differently.

**A nickname is set once and cannot be changed from inside a room.** There is
no `rename` verb. A player who mistyped leaves and rejoins.

#### The seat number is what becomes the game's slot

Stage B hands each playing member a slot in the match, and it derives that slot
from the seat number, so three properties of a seat number are load-bearing.
All three are asserted in `test_units.py`, in `test_seat_stability`.

1. **A member's seat number never changes while they hold it.** Nothing
   renumbers anybody, through other people joining, leaving, being removed and
   being promoted.
2. **The host is always seat 1.** The creator takes seat 1, and a host leaving
   closes the room rather than handing it on, so no live room has a host at any
   other number.
3. **Seat numbers are distinct and always inside 1..`MAX_SEATS`**, so the slots
   derived from them are distinct and always inside 0..3.

A freed number is reused rather than counting ever upward, because the numbers
have to stay inside the game's four slots. Property 1 is what makes that safe.

**Why this matters outside this service.** Over the relay the parent's ACCEPT is
a broadcast with no recipient field, so two children claiming the same slot
cannot be told apart by the game. The lobby is the only place a slot can be
handed out uniquely. That is what resolves the spec's 4.7 caveat, which said
the seat-to-slot mapping was only guaranteed at two players: it is guaranteed at
any number once the lobby, rather than the wire, decides it.

**One thing stage B still has to choose,** flagged here rather than discovered
late. With `GAME_MAX_PLAYERS` below `MAX_SEATS` the playing seats need not be
contiguous: with seats 1, 2 and 3, seat 2 leaving promotes seat 3, and the
playing seats are then 1 and 3. `slot = seat - 1` gives slots 0 and 2 with a
hole at 1; `slot = rank among the playing seats` gives 0 and 1, packed. Both are
computable from this model and neither needs a change here. The packed reading
is very probably the one the game wants.

#### `poll` — the only push channel

```
-> {"v":1, "room":"K7QMR3", "token":"...", "cursor":7, "wait":20}
<- 200 {"v":1, "cursor":11, "view":{...}, "events":[...]}
```

Holds the request until there is an event newer than `cursor` or `wait` seconds
pass, and stamps the member's heartbeat. `wait` is optional and defaults to 0;
`cursor` is required.

- `cursor` is a per-room monotonic sequence. **Nothing can be missed:** a
  client that misses ten polls gets all ten events on the eleventh. That is the
  entire reliability story for chat and params.
- A room keeps the last **100 events** and the last **40 chat lines**. A cursor
  older than the oldest retained event answers `"resync":true` plus the full
  `view` and the retained chat, and the client replaces its state rather than
  appending to it.
- `wait` is clamped to 0..25. **A `wait` of 0 is a plain short poll and the
  contract is identical**, so if a middlebox turns out to cut long requests the
  fallback is a client-side constant and not a redesign.
- Concurrent holders are capped at `MAX_WAITERS`. Over that the server answers
  immediately, exactly as if `wait` were 0.

`view` is the whole room state and is small enough to send every time:

```json
{"state":"lobby", "host":1, "you":2, "slots":2, "max_players":2,
 "map":0, "win_mode":"stars", "star_target":3,
 "members":[{"seat":1,"display":"tango","playing":true,"pre_ok":true,"armed":false},
            {"seat":2,"display":"opie","playing":true,"pre_ok":true,"armed":false}],
 "match":null}
```

`state` is one of `lobby`, `arming`, `go`, `in_match`, `closed`. A poll answer
also carries a top-level `"plan"` whenever `state == "go"`, so a client that
resynced past its `go` event still has its plan and can spawn.

`events` entries carry a `seq` and a `kind`:

```json
{"seq":8,"kind":"joined","seat":2,"display":"opie"}
{"seq":9,"kind":"left","seat":3,"display":"nn","why":"quit"|"timeout"|"kicked"}
{"seq":10,"kind":"chat","seat":2,"display":"opie","text":"gg"}
{"seq":11,"kind":"params","map":2,"win_mode":"stars","star_target":3,"match_players":4}
{"seq":12,"kind":"arming","match":"<16 hex>","deadline_ms":20000}
{"seq":13,"kind":"go","match":"<16 hex>","plan":{...}}
{"seq":15,"kind":"failed","match":"<16 hex>","reason":"no_pairing"}
{"seq":16,"kind":"closed","why":"host_left"|"idle"|"shutdown"}
```

Stage C adds one more, `{"seq":14,"kind":"result","match":"...","win":"...","scores":[3,1,0,0]}`.

The `go` event's `plan` is PER MEMBER: each member's poll carries its own plan
(parent vs child, its slot, its spawn delay), injected at poll time from the
frozen match state; everything else in the plan is identical across members. See
`start` below.

`kind` is a closed set. **A launcher that meets an unknown `kind` ignores that
event and keeps its cursor moving** — the one place the client is deliberately
lenient, so that a server which gains an event kind in a patch release cannot
brick an older launcher that does not need it.

#### `chat`

```
-> {"v":1, "room":"...", "token":"...", "text":"gg"}
<- 200 {"v":1, "cursor":10}
```

1 message per 2 seconds per **member**, burst 3. Over that, 429 `too_fast`; the
launcher greys its Send button rather than showing an error.

#### `params` (host only)

```
-> {"v":1, "room":"...", "token":"...", "map":2, "win_mode":"stars", "star_target":3}
<- 200 {"v":1, "cursor":11}

v2 adds the player-count dial, which is OPTIONAL:
-> {"v":2, "room":"...", "token":"...", "map":2, "win_mode":"time", "match_players":4}
<- 200 {"v":2, "cursor":11}
```

403 `not_host`; 409 `not_in_lobby` once a match is arming or running. All three
v1 fields are sent together every time, so two in-flight edits have no ordering
hazard. The star target is the host's to pick, from 1 to `STAR_TARGET_MAX`.

**`match_players` — the dial.** How many of the room's seats play. The first
that many people in the room play; everybody past them watches. It is a room
parameter like the arena and the win condition: host-only, lobby-only, frozen
from Start until the room comes back.

- **Range `2` to `dial_max`**, where `dial_max = min(16, MAX_SEATS,
  GAME_MAX_PLAYERS)` and is advertised in every room `view` and by `/health`.
  Anything outside it is `400 bad_match_players` — **refused, never clamped**.
  A host who asks for eight and silently gets four has been lied to and would
  only find out by counting heads.
- **Optional, and v2-only.** Omitting it leaves the room's dial alone, which is
  what launcher 0.3.0 does on every settings edit it makes. Sending it at
  `"v":1` is `bad_field`.
- **A room that nobody dials** sits at `GAME_MAX_PLAYERS`, so seating is
  byte-for-byte what it was before the dial existed.
- **Moving it moves as few people as possible.** Raising it promotes the
  lowest-numbered watchers; lowering it demotes the highest-numbered players.
  The host (seat 1) is never demoted — the host is the session's parent.
- **`view` gains `match_players` and `dial_max`.** `max_players` is unchanged
  and still means `GAME_MAX_PLAYERS`; the dial does not get to redefine a field
  a shipped launcher already reads.
- **The `go` plan gains `match_players`** beside `players`. `players` is what
  actually turned up and is what rides into `SM64DS_VS_PLAYERS`;
  `match_players` is what the host asked for. They differ when the room did not
  fill, and the launcher can then say "3 of the 4 you picked" without guessing.

**The dial reaches 16 and the game does not.** See the ceiling map at the end
of this file. Sixteen is what the control expresses; four is what the game has
been proved at, and `GAME_MAX_PLAYERS` in `docker-compose.yml` is the one place
that truth is configured.

#### `preflight`

```
-> {"v":1, "room":"...", "token":"...", "pre_ok":true}
<- 200 {"v":1, "cursor":13}
```

A seated member correcting **its own** `pre_ok`. There is no seat argument, so
this grants no authority over anybody else.

**Why it exists,** from a live match: `pre_ok` was measured once when the
Multiplayer window opened and sent once at create/join, so a player who unpacked
their ROM while that window sat open kept a stale "not ready" on their seat. The
recovery a real pair had to find was close the window, reopen it, check it went
green, make a new room and rejoin. Nobody discovers that alone.

- **Unchanged is a no-op.** A launcher re-runs its pre-flight on a timer and on
  window focus, so "still the same answer" is the common case by far: it pushes
  no event and moves no cursor. Only a real change costs a push.
- **A change pushes a `preflight` event** (`{seat, display, pre_ok}`) as well as
  landing in the `view`, so a member holding a 25-second long poll learns now
  rather than when its wait expires. The event `kind` set gained one member;
  section 3.5's rule that a launcher ignores an unknown `kind` and keeps its
  cursor moving is exactly what makes that safe for an older launcher.
- **Legal in every live state**, not only `lobby`. During a match the flag feeds
  nothing (the roster froze at `start`), so allowing it is harmless -- and it
  means a player who finishes unpacking mid-match is ready for the rematch
  instead of blocking it.

#### `start` (host only) — stage B

```
-> {"v":1, "room":"...", "token":"..."}
<- 200 {"v":1, "cursor":12, "match":"<16 hex>"}
```

Refusals, each with its own code: 403 `not_host`; 409 `not_in_lobby` if a match
is already arming or running; 409 `not_enough_players` if fewer than 2 seats are
playing; 409 `member_not_ready` if a playing seat is not `pre_ok`.

On success NOTHING SPAWNS. The room moves to `arming` and, at one frozen instant,
the server mints a 16-hex match id and a fresh 8-char comms code (from the room
alphabet, **never shown to any client** except inside a launch plan), freezes the
params, the roster, the slot map and the `SM64DS_VS_NAMES` string, and records
`agreed_players` = the playing-seat count. An `arming` event carries the match id
and `deadline_ms`. Every playing launcher must then POST `ready` within
`ARM_DEADLINE_S`, or the deadline returns the room to lobby with a `failed`
event, reason `member_not_ready`. This is the join-race guarantee's front half:
`agreed_players` rides every plan as `SM64DS_VS_PLAYERS`, so both games seat the
same number of players regardless of wire timing.

**The slot map.** Playing seats are packed into slots 0..3 in seat order, so the
host (seat 1) is always slot 0 and the plan hands each child a distinct slot. The
launcher exports a child's slot as `SM64DS_COMMS_SLOT`; the parent's slot 0 is
implicit (the game ignores `SM64DS_COMMS_SLOT` for the parent). This is what lets
the lobby, rather than the wire, decide slots -- the relay ACCEPT is a
recipientless broadcast and cannot tell two children apart.

**The go plan** (delivered inside each member's `go` event, and at the poll's
top level while `state == "go"`):

```json
{"role":"parent", "code":"M7KQ2PXR", "relay":"135.148.26.201:41234",
 "map":2, "players":2, "slot":0, "spawn_delay_ms":0, "playing":true,
 "names":"tango,opie,,", "star_target":3}
```

`names` is byte-identical in every member's plan. `star_target` is present only
in stars mode. The host gets `spawn_delay_ms: 0`; children get `1500` (the parent
goes first). A spectator gets `{"playing": false}` and spawns nothing.

#### `ready` — stage B

```
-> {"v":1, "room":"...", "token":"...", "match":"<16 hex>"}
<- 200 {"v":1, "cursor":13}
```

A playing member: "my launcher can spawn this match." 409 `stale_match` if the
match id is not the room's current one, which makes it un-replayable across
matches; 403 `not_playing` from a spectator. When the last playing seat readies,
the room moves to `go` and the `go` event (with each member's plan) is pushed. A
`ready` that arrives after `go` for the same match is a harmless idempotent
duplicate.

**The roster that has to be ready is the FROZEN one.** `agreed_players`, `names`
and `slot_of` all froze at `start`, so the go gate compares the live playing
seats against the seats frozen there -- as a SET, not as a count. Two shapes this
closes, both ordinary behaviour rather than exotic input:

- the only other playing seat **leaves during arming**: the live roster shrinks
  to one, and without this the host's own `ready` would satisfy "everybody is
  ready" and send the room to `go` with a plan still saying `players: 2` and
  still crediting the departed player in `names`;
- a **spectator is promoted** when a playing seat leaves during arming: the count
  still matches `agreed_players`, but the promoted seat holds no frozen slot, so
  it would take the default slot 0 and collide with the parent.

A room whose roster moved simply stalls, and the arming deadline returns it to
lobby with `failed` reason `member_not_ready`.

#### `failed` — stage B

```
-> {"v":1, "room":"...", "token":"...", "match":"<16 hex>", "reason":"no_pairing"}
<- 200 {"v":1, "cursor":15}
```

Any playing member reporting a match that could not run. `reason` is a fixed
enum: `spawn_failed`, `no_pairing`, `wrong_player_count`, `startup_error`,
`user_cancelled`, `timeout` (`member_not_ready` is server-minted only and is
refused from a client). 409 `stale_match` guards the match id, and a spectator is
refused with **403 `not_playing`** -- a room code is not a secret, so without that
gate anyone who walked in could return the room to lobby mid-match, repeatedly,
and the host could not even `kick` them (kick is refused outside lobby). Any `failed`
returns the whole room to lobby with a `failed` event, so every other launcher
learns why. The `go`->`in_match` transition happens `GO_GRACE_S` after `go`, and
a match with no result is returned to lobby with `failed` reason `timeout` after
`MATCH_TIMEOUT_S`.

#### `result` — stage C

```
-> {"v":1, "room":"...", "token":"...", "match":"<16 hex>",
    "win":"star-target", "scores":[3,1,0,0]}
<- 200 {"v":1, "cursor":14}
```

A playing member reporting a finished match. `win` is `time-up`, `star-target`
or `draw` (the launcher posts `draw` with all-zero scores for a `MATCH OVER`
marker it could not parse). `scores` is four ints, 0..99, in slot order. The
FIRST valid result for the current match appends a `result` event, discards the
comms code, clears the match id and every `armed` flag, and returns the room to
`lobby` -- while the room code, seats, nicknames, tokens, chat AND the params all
survive, so "same again" is one Start press. A later result for the same match id
is accepted and ignored (both players reporting is normal). 409 `stale_match` for
any other match id, and **403 `not_playing`** from a spectator -- the same gate as
`failed`, and it also stops a spectator writing the scoreline every member sees.

`ready`, `failed` and `result` are the three verbs spec 3.2 restricts to "a
seated **playing** member", and all three answer 403 `not_playing` the same way.

**A different comms code every match.** Because `start` mints a fresh one and
`result` discards the old, a rematch is a brand-new relay session and the
previous match's 90-second held seats cannot refuse it.

#### `kick` (host only)

```
-> {"v":1, "room":"...", "token":"...", "seat":2}
<- 200 {"v":1, "cursor":12, "ok":true}
```

403 `not_host`; 409 `not_in_lobby`; 400 `bad_seat` for the host's own seat, an
empty seat, or a number outside the room. The seat is freed exactly the way a
`leave` frees it — same `left` event, same promotion — with `why:"kicked"`.

The kicked member's own token answers 403 `kicked` from then on, so their
launcher can say what happened instead of guessing, and the address they were
last seen at is refused by that room for `KICK_COOLDOWN_S`.

**Stated plainly, because it is a real limit and not a bug.** There are no
accounts, and a member token is minted per join, so "the same person" is not
something this service can recognise. The client address is the only handle it
has. A kicked player who changes address walks straight back in, and a second
player in the same house shares the cooldown. That is exactly why it is a
cooldown and not a ban for the room's lifetime — a permanent block on a shared
address would lock out an innocent housemate for as long as the room lived. The
remedy for someone who will not leave is still the host closing the room, which
costs one button and takes the code with it.

#### `leave`

```
-> {"v":1,"room":"...","token":"..."}
<- 200 {"v":1,"ok":true}
```

Host: the room closes (`closed`, `why:"host_left"`). Member: the seat is freed,
a `left` event is pushed, and the lowest spectator is promoted if a playing
seat opened.

#### `health`

```
GET /port/lobby/health
<- 200 {"ok":true,"ts":"...","revision":"lobby-1","contract_min":1,"contract_v":2,
        "dial_max":4,"rooms":3,"members":7,"waiters":5}
```

`rooms` visible from outside is the difference between a detectable outage and
a quiet one.

---

## Failure semantics

| event | what the server does | what players see |
|---|---|---|
| host leaves, or 45 s without a poll | room -> `closed`, `why:"host_left"`; the record is kept 60 s so members get the reason rather than a bare 404, then deleted | "The host closed the room." |
| member leaves | seat freed, `left` event, spectator promoted if a playing seat opened | the name disappears; a system line says who left |
| member stops polling | 45 s heartbeat timeout, treated as a leave with `why:"timeout"` | as above |
| service restarts | **every room is gone.** State is in memory by design | every poll gets 404; the launcher says the room is gone and returns to the create/join screen |
| room idle | 30 minutes with no event -> `closed`, `why:"idle"` | "This room timed out." |

Rooms are ephemeral and that is the whole recovery story. There is no
persistence, no database, no file on disk and nothing to restore. **If the
lobby server restarts, everyone in a room has to make a new room; anyone in a
match keeps playing and does not notice.**

---

## Limits

| limit | default | scope | env knob |
|---|---|---|---|
| requests | 20/s, burst 40 | per address | `RATE_REQ_PER_S`, `RATE_BURST` |
| creates | 30/hour | per address | `RATE_CREATE_PER_HOUR` |
| joins | 60/min | per address | `RATE_JOIN_PER_MIN` |
| chat | 1 per 2 s, burst 3 | per **member** | - |
| body | 4096 bytes | per request | - |
| rooms | 64 | server | `MAX_ROOMS` |
| seats | 4 | per room | `MAX_SEATS` |
| players in a match | 2 | per room | `GAME_MAX_PLAYERS` |
| long-poll holders | 96 | server | `MAX_WAITERS` |
| bad-sender ignore | 10 refusals in 10 s -> 10 s | per address | `BAD_LIMIT`, `BAD_WINDOW_S`, `BAD_IGNORE_S` |
| read timeout | 20 s per connection | per connection | `HANDLER_TIMEOUT_S` |

Generosity is deliberate: two launchers on one desk and four players in one
house share an address, and the owner will create dozens of rooms while
testing.

**`TRUST_XFF`.** Behind Caddy the socket peer is always Caddy, so without
reading `X-Forwarded-For` every per-address limit above would be a per-SERVER
limit and thirty creates an hour would be thirty for the whole internet. **The
LAST entry of that header is used**, never the first: the first entry is the one
a client can write itself, and reading it let a client forge a fresh address
per request and walk through every limit. The last entry is the one the nearest
proxy wrote, and it is correct whether Caddy replaces the header (one entry) or
appends the real peer (last entry). **Set `TRUST_XFF=0` for any deployment
where clients can reach the container directly** — with no proxy in front there
is no trustworthy entry, and any value the client sends is read as its address.

---

## Logging

Room lifecycle only. **Never chat text, never nicknames, never tokens.**
Addresses are blunted the way `relay.py`'s `short_addr` blunts them, and the
source port is dropped entirely rather than kept, because behind Caddy it is
the proxy's ephemeral port and says nothing about the client.

```
room K7QMR3 create by 203.0.x.x                       (1/4)
room K7QMR3 join 198.51.x.x seat 2 playing            (2/4)
room K7QMR3 params map=2 win=stars target=3
room K7QMR3 kick seat 2 by host                       (1/4)
room K7QMR3 leave seat 2 (quit)                       (1/4)
room K7QMR3 promote seat 3
room K7QMR3 close (host_left)
reject 203.0.x.x POST /port/lobby/chat 400 bad_text
reject 203.0.x.x POST /port/lobby/xyz 404 unknown_verb
stats rooms=3 members=7 waiters=5 req=1204 rej_rate=2 rej_shape=0 rej_auth=1
```

---

## Abuse

Per the owner's model the only real damage is nuisance. Every nuisance this
design permits, and its cap:

| a hostile client can... | cap |
|---|---|
| create rooms to exhaust codes | `MAX_ROOMS` 64, matched to the relay's 64 sessions; 30 creates/hour per address; 30-minute idle close |
| guess room codes to find strangers' rooms | 31^6 = 8.9e8 codes; 60 joins/min per address; a found room shows the intruder in the player list, and the host can now remove them or close the room |
| **take a relay seat in a stranger's match by knowing their invite code** | **impossible.** The room code is not the comms code. Stage B mints a fresh 8-char comms code per match and it never appears in any UI |
| flood chat | 1 per 2 s per member, burst 3, 200 bytes, 40-line ring |
| set a giant nickname, control characters, or markup | 16 bytes, `0x20..0x7E` only, rendered as plain text into a read-only TextBox |
| impersonate another player or the host | needs their 128-bit token |
| make the server do unbounded work | 4 KB bodies, a fixed field grammar, no regex over user text, no recursion, no nested JSON, no outbound requests, **no disk writes at all** |
| fill the disk | there is no disk. `read_only: true`, a small tmpfs, no bind mounts |
| exhaust threads with long polls | `MAX_WAITERS` 96; over that, `wait` is treated as 0 |
| flood the box | 20 req/s per address, burst 40, the bad-sender rule, plus Caddy in front |
| **take down updates, crash intake, tangos.dev, or a match in progress** | **cannot.** Separate container, separate budget, no shared mounts |

What a hostile client cannot reach at all: the crash corpus, the release zips,
the relay's session table, any credential (there are none here), and any other
player's environment.

---

## DEPLOY (not applied by the build lane)

```
scp -r port/tools/lobby/ tangos:~/sm64ds-lobby/
ssh tangos "cd ~/sm64ds-lobby && docker compose up -d --build"
ssh tangos "docker exec caddy caddy validate --config /etc/caddy/Caddyfile"
# then the Caddy edit below, then:
ssh tangos "docker exec caddy caddy reload --config /etc/caddy/Caddyfile"
curl -s https://tangos.dev/port/lobby/health
```

Rollback is `docker compose down` plus reverting the Caddy block. There is no
state to migrate in either direction.

### THE CADDY EDIT — the riskiest step in the whole plan

**Back the Caddyfile up first.** Its own header warns that it "was silently
lost once":

```
docker exec caddy cat /etc/caddy/Caddyfile > ~/Caddyfile.bak-before-lobby-$(date -u +%Y%m%dT%H%M%SZ)
```

Add, **above** the existing `handle /port*` block:

```
	# SM64DS multiplayer LOBBY - OWNED BY ~brennen/sm64ds-lobby/ (added <date>).
	# Room state and chat for the port's Multiplayer button. Separate container
	# from port_ingest ON PURPOSE: it is the only unauthenticated write surface
	# in the port kit and must not share a process with the release zips.
	# MUST stay above the /port* line. Joins `web`, no published ports.
	handle /port/lobby* {
		reverse_proxy sm64ds_lobby:8091
	}
```

Then `caddy validate` before `caddy reload`, and **prove the ordering with
requests rather than trusting it**:

- `curl -s https://tangos.dev/port/lobby/health` must return the lobby's JSON
  (it carries `"rooms"`), **and**
- `curl -s https://tangos.dev/port/health` must still return `port_ingest`'s
  (it carries `"families"`), **and**
- `curl -sI https://tangos.dev/port/manifest.json` must still be 200.

If the ordering does not resolve as expected, fall back to a distinct top-level
prefix (`handle /lobby*`) rather than fighting the matcher, and set
`AppConfig.LobbyUrl` accordingly. The contract does not change either way.

### The long-poll question

A `wait` of 25 must survive Caddy, or the launcher's constant comes down. It
holds locally (measured; see `status/LOBBYA.md`), and the number through Caddy
has to be measured on the deployed URL:

```
python3 test_client.py selftest --url https://tangos.dev/port/lobby
```

If a 25-second hold is cut short there, lower `LobbyPollWaitSeconds` in the
launcher. **That is a client-side constant, not a protocol change** — a `wait`
of 0 is a plain short poll with an identical contract.

---

## THE CEILINGS BETWEEN 4 AND 16

The dial reaches sixteen. **The game does four.** This is the list of what
stands between them, measured at cons `339db6bb3` rather than estimated, so the
next lane starts from evidence instead of from a guess. Every address below is
from `config/arm9/relocs.txt` and `config/arm9/**/symbols.txt`; every file:line
is from this tree.

**The shape of the answer, before the detail.** Four is not one wall, it is
three different kinds of wall, and only the first kind is cheap:

| kind | examples | cost |
|---|---|---|
| **A knob** - a number with nothing structural behind it | the lobby dial, the relay's seats | done, or one line |
| **A wire format** - a field too narrow, shared by every build | the 8-bit live mask, the 4-block payload | a versioned wire break; every peer must update together |
| **The ROM itself** - byte-matched decompiled code, or a symbol with another symbol immediately after it | the 2-bit player number, the 4-byte score array, `4 - count` | MOD territory under the port's own north star |

**The one number that decides most of it: `mPlayerNo` is TWO BITS.**
`src/_ZN6Player13InitResourcesEv.cpp:72` is `*(u8*)(c + 0x6d8) = (a >> 6) & 3;`
- the ROM unpacks a spawned player's identity from bits 6..7 of the spawn-flag
word, boxed between `f1` at bits 3..5 and the sublevel at bit 8. Slots 4..15
cannot be *expressed*, never mind stored. Everything else in this list is
downstream of that.

### 1. Comms - the wire

| what | where | today | 16 needs |
|---|---|---|---|
| `kCommsMaxPlayers` | `port/hal/comms_seam.h:237` | `4` | 16 - and it sizes the packet |
| live-slot bitmask | `comms_loopback.cpp:295` `unsigned char live` | **8 bits** | 16 bits: a wire break |
| block payload | `comms_loopback.cpp:300`, `kPacketBytes = 0x90` | `0x10 + 4*0x20` = 144 B | `0x10 + 16*0x20` = **528 B** |
| slot assignment | `comms_loopback.cpp:1281-1282` `assigned < kCommsMaxPlayers` | 0..3 | a bound, not a format |
| per-slot ports | `comms_loopback.cpp:856-865` slot k at `base+k` | 4-port span | 16-port span, **loopback/direct only** |
| relay session seats | `relay.py` `MAX_CHILDREN` | 3 | **CLIMBED - now a knob** |
| relay payload cap | `relay.py` `MAX_PAYLOAD` | 700 B | **already fits 528, 172 to spare** |
| relay rate cap | `relay.py` `RATE_PPS` 120 | | **not a ceiling** - see below |

**The packet budget at 16 is fine, and that is the good news in this section.**
A sixteen-slot packet is 528 bytes against a 700-byte cap. And the rate limiter
never comes into it: `send_to_children` (`comms_loopback.cpp:918-925`) sends
**exactly one copy** in relay mode because the relay fans out, so a parent's
outbound rate is one packet per frame - 60/s against a 120/s limit - at sixteen
players exactly as at four. Both measured, in the relay's own `t_seat_knob`.
What the relay does pay is egress: 15 copies x 60/s x 528 B is about 3.8 Mbit/s
per full session.

**The real comms wall is one byte.** `Packet.live` is an `unsigned char`, and
it is on the wire, in the ACCEPT that tells a child who else is in the session.
Widening it is a version break every peer has to take at once. The port math is
the second one, and it is smaller than it looks: it only binds `base+k` in
loopback and direct mode; over the relay every peer holds **one** socket
(`comms_loopback.cpp:882`), so the launcher's four-port span
(`LobbyLaunch.cs:195-230`) is a local-play concern only.

**Estimate:** the wire change is a day, mostly in keeping old and new peers from
half-understanding each other. It is worthless on its own - nothing above it
can use sixteen slots yet.

### 2. The Ctrl input band at `0x18 * 4`

**Re-derived, and the VS4P finding holds: on the host this is sizes.** Every
one of the ten split names is the port's own array at the full four-record
extent - `hal/auto_bss.cpp:167-174`, `hal/actor_vtables.cpp:505-521`,
`hal/sub_actors.cpp:270` - and none is `__declspec(allocate(".dsstate..."))`, so
none is address-pinned and widening one shifts nothing. `CheckInput`'s own loop
runs to `data_0209f21c`, a byte, so the ROM's reader is already generic.

**But the ROM's address space is not.** The relocs say the Ctrl block starts at
`0x0209f498` and the next referenced symbol is **`0x0209f4f8`** - exactly
`0x60`, exactly `0x18 * 4`, not a byte spare. Sixteen records need `0x180`
(`f498..f617`), which in the ROM's own map would run straight through *fifteen*
separately-referenced symbols: `f4f8`, `f5b8`, `f5bc`, `f5c0`, `f5c4`, `f5d0`,
`f5dc`, `f5e8`, `f5f8`, `f5fc`, `f600`, `f604`, `f608`, `f60c`, `f610`.

**Estimate:** small on the host, and **not climbed tonight on purpose** - the
change is free only if it shifts nothing, and proving *that* costs a full build
plus the BMP baselines, for a benefit of zero until the wire moves. It is the
right first rung the day the wire does.

### 3. `SM64DS_VS_NAMES`, and the colours question

`hal/star_flow.cpp:1028` is `static char g_vs_names[4][17]`, the reader refuses
any string without **exactly three commas** (`:1059`), and it logs four fields
(`:1093`). The lobby builds the same shape (`build_names`, four fields, slot
order). At sixteen the string is fifteen commas and up to 16x17 bytes, which
brushes the 4096-byte body cap far less than it sounds (272 bytes).

**This is the cheapest ceiling in the list** and the only one that is purely a
format the two sides agree on - no ROM, no wire. A v2 names shape (a count
prefix, or "as many fields as commas + 1") is an afternoon.

**Flagged to the coordinator, NOT decided here:** lane VSCOLOR-UI is adding
`SM64DS_VS_COLORS` tonight and would freeze a field count of its own. The two
strings should answer the field-count question **the same way, once**. I have
not touched their files.

### 4. Arena entrance records, and what the ROM does when it runs out

The count is data, not code: `hal/level_boot.cpp:2101` reads
`g_entrance_count = ((const unsigned char *)tbl)[1]` from the level's own
table. VS4P measured **4** on arena 0 (level 51), and all four players spawned
from the ROM's own records with no port stand-in - the four positions were the
level's four entrance records exactly.

When players exceed the starts, `port_vs_spawn_extra_players`
(`hal/level_boot.cpp:4294-4368`) supplies a body. Its behaviour, plainly:

- It is bounded `for (int i = 1; i < n && i < 4; ++i)` - **hard 4** at `:4309`.
- It places extras `40.0` units apart on X from player 0's record, and its own
  comment says `OVERLAP` (`:4315`) - a stand-in, not a spawn point.
- It packs `((unsigned)i << 6)` into the flag word (`:4358`), which is the
  2-bit `mPlayerNo` field. Slot 4 would write bit 8 and land in the sublevel.

**Estimate:** the arenas would need real start data authored - sixteen bodies
40 units apart in a line is not a match. This is level-data work, not code, and
it is the one item on this list that no amount of C can finish.

### 5. Scoring, the HUD band, and the winner

**The score array has four bytes and the fifth belongs to something else.**
`data_0209f310`'s ROM run is `f310..f313`; the relocs' next referenced symbol is
`0x0209f314`, loaded from five separate sites (`0x0202aa28`, `0x0202aa48`,
`0x0202aa68`, `0x0202aa84`, `0x0202b038`). Sixteen scores would overwrite
`f314`, `f318` and `f31c`. The host already gives 32 bytes
(`hal/actor_classes_star.cpp:237-241`) so nothing would *crash* - the ROM's
readers simply cannot index past 3.

The readers, and how each one is bounded:

| reader | bound |
|---|---|
| `NumVsStarsObtained` | `data_0209f21c` - **already generic** |
| `func_ov075_021165b0` (the winner) | **eight** literal `< 4` loops, byte-matched ROM |
| `func_ov002_020d94cc:30` | `(&data_0209f310)[*(u8*)(self+0x6d8)]` - the 2-bit player number |
| `HUD::RenderStarCount:39` | `data_0209f310[data_0209f250]` - local player only, generic |
| `star_flow.cpp:934,1120-1140,1338` | `g_end_scores[4]`, `scores=%d,%d,%d,%d` - **the port's own, cheap** |

**The HUD star band is the sharpest one, and it is not "four columns
hard-coded".** `src/func_ov075_0211621c.c:56-64` is:

```c
int count = data_0209fc50;
int d = 4 - count;
xbase = data_ov075_0211c6e8[d];
stride = (d * 16) + 0x38;
```

The layout table is indexed by **how many fewer than four** are playing, and
`data_ov075_0211c6e8` is a **4-byte symbol** - `data_ov075_0211c6ec` is the next
one, four bytes on. At five players `d` is `-1` and the ROM reads *before* its
own table; the stride goes negative and the columns march off the left of the
screen. This is byte-matched ROM code, so the literal cannot move without the
match moving with it.

**Estimate:** the port's own half (`g_end_scores`, the marker, the banner) is an
hour. The ROM's half is the whole MOD question.

### 6. The ROM's own wireless ceiling - the honest bottom line

The DS did four, and the decompiled code says so in the places that count:

- `src/func_0203ea5c.c` - the ROM's own wireless exchange and unpack loop -
  carries **ten** hard-coded `< 4` bounds (`:310, 326, 347, 352, 367, 403, 428,
  471, 499`).
- `data_0209f394`, the per-player `Actor*` table: ROM run `f394`, next
  referenced symbol **`f3a4`** - `0x10`, four pointers.
- `data_0209fc5c`, the per-slot live flags: `fc5c`, next referenced `fc5d` then
  **`fc60`** - four bytes.
- `data_0209f310` / `data_0209f358` / `data_0209f30c`: four each, as above.
- `mPlayerNo`: two bits.

**So anything past four is a MOD, and this file says so rather than burying
it.** Under the port's own north star - *the port must BE the decomp* - every
item in section 6, and the ROM half of sections 4 and 5, changes bytes the gate
checks. That is the owner's call to make, not a lane's, and it should be made
knowing it is a fork of the ROM's behaviour rather than a port bug being fixed.

### What was climbed, and what was left alone

**Climbed, with proof:**

- **The lobby dial to 16**, server-enforced against the deployment's real
  capability, refusing 5..16 today with a reason a player can read.
- **The relay's session seats to a knob**, default unchanged at 3 children.
  Proven at 15: sixteen pair, the seventeenth is refused, and one parent
  datagram still reaches all fifteen.

**Left alone on purpose:** the Ctrl band (free only if it shifts nothing, and
proving that costs a build for no benefit yet), `kCommsMaxPlayers` and the live
mask (a wire break every peer takes at once, useless alone), and everything in
section 6 (MOD territory, the owner's call).

**If someone picks this up, the order is:** the names shape (cheapest, and
already needs coordinating with the colours string) -> the wire (live mask,
packet, port span) -> the Ctrl band -> the port's own scoring and marker -> and
only then the conversation about the ROM.
