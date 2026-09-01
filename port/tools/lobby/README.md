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
python3 test_client.py soak --seconds 60
python3 test_client.py selftest --url https://tangos.dev/port/lobby
```

---

## THE WIRE CONTRACT, v1

### Rules that apply to every request

| rule | value | on violation |
|---|---|---|
| method | `POST` for all verbs; `GET` for `/health` only | 405 / 404, connection closed |
| URL | exactly `/port/lobby/<verb>`, no query string ever | 404 `unknown_verb` + one log line |
| content type | `application/json` | 415 `bad_content_type` |
| `Content-Length` | REQUIRED, and parsed **before any rejection** | missing -> 411 `length_required`, close |
| request body | <= 4096 bytes | 413 `too_large`, connection closed (body not read) |
| JSON shape | one object; no arrays at the top level | 400 `bad_shape` |
| `v` | REQUIRED, integer, must be `1` | 400 `bad_version` |
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
| `v` | int | `== 1` | `bad_version` |
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
{"seq":11,"kind":"params","map":2,"win_mode":"stars","star_target":3}
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
```

403 `not_host`; 409 `not_in_lobby` once a match is arming or running. All three
fields are sent together every time, so two in-flight edits have no ordering
hazard. The star target is the host's to pick, from 1 to `STAR_TARGET_MAX`.

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
matches. When the last playing seat readies, the room moves to `go` and the `go`
event (with each member's plan) is pushed. A `ready` that arrives after `go` for
the same match is a harmless idempotent duplicate.

#### `failed` — stage B

```
-> {"v":1, "room":"...", "token":"...", "match":"<16 hex>", "reason":"no_pairing"}
<- 200 {"v":1, "cursor":15}
```

Any playing member reporting a match that could not run. `reason` is a fixed
enum: `spawn_failed`, `no_pairing`, `wrong_player_count`, `startup_error`,
`user_cancelled`, `timeout` (`member_not_ready` is server-minted only and is
refused from a client). 409 `stale_match` guards the match id. Any `failed`
returns the whole room to lobby with a `failed` event, so every other launcher
learns why. The `go`->`in_match` transition happens `GO_GRACE_S` after `go`, and
a match with no result is returned to lobby with `failed` reason `timeout` after
`MATCH_TIMEOUT_S`.

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
<- 200 {"ok":true,"ts":"...","revision":"lobby-1","rooms":3,"members":7,"waiters":5}
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
