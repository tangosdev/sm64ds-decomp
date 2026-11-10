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
join-race guarantee. Stage C added `result`. **Lobby 1.0 (contract v5)** adds
the game build check, Luigi Infection and voice, the session digest, adventure
rooms with a room-lifetime hub code, and the world pool: see CONTRACT v5 below.

---

## Layout

| file | what |
|---|---|
| `app/server.py` | the whole service |
| `Dockerfile` | three lines, same shape as `port-ingest/Dockerfile` |
| `docker-compose.yml` | the deploy unit, and every limit as an env knob |
| `test_client.py` | over-the-wire `selftest`, `negatives` and `soak` |
| `test_units.py` | the grammar, the caps, the lifecycle, the limiters, in process |
| `test_security.py` | slow bodies, forged `X-Forwarded-For`, the world pool's bounds, over raw sockets |

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
python3 test_security.py                    # spawns its own servers
python3 test_client.py arena                # arena netmode over the wire
python3 test_client.py selftest --url https://tangos.dev/port/lobby
```

---

## CONTRACT v5 (lobby 1.0) AND v6 (party and coop rooms)

Multiplayer 1.0 (run mp1, NETARCH1 design section 5). **`CONTRACT_MIN = 5`,
`CONTRACT_V = 6`**: v5 is the floor, v6 is additive (see "PARTY AND COOP
ROOMS" below). Every request that says `"v":1..4` is `400 bad_version`,
which launchers 0.3.x..0.5.x already show as "This launcher and the lobby
server do not agree on the rules. Check for an update." The floor moved with
the version because the game under it changed: a 1.0 match needs a relay v2
seat, a server-minted Luigi seed and a session digest in every plan, and a
pre-1.0 launcher would spawn a game that cannot pair with a 1.0 host. The
older gates that kept a narrow v2 client out of a wide room are gone with the
clients they guarded against; the history sections below still describe them.

**The examples further down were written at v1/v2.** Since v5 every request
says `"v":5`, and `create`/`join` also carry `game`. Everything else in them
is unchanged.

What v5 is, all of it:

| where | what |
|---|---|
| `create`, `join`, `world` | `game` REQUIRED: the game build id, `^[0-9A-Za-z._-]{1,24}$`, compared byte for byte, never parsed. A build not on the server's `GAME_IDS` allow list is **409 `unknown_game`** (check for an update). A room remembers its creator's build; a join from another listed build is **409 `game_mismatch`** ("someone in this room runs a different game version"), checked before `room_full` |
| `create` | `session_type` `"vs"` (default) or `"adventure"`; `"party"` and `"coop"` are v6 (below), and a v5 caller naming either gets `400 bad_session_type` |
| `params` (VS rooms) | optional `luigi` 0/1 (Luigi Infection, default 0) and `voice` 0/1 (the match allows proximity voice; **voice chat is shelved since 10-08: the field is still checked, but the stored value is always 0 and every plan says `voice` 0**, `VOICE_SHELVED` in server.py). Anything but the integer 0 or 1 is `400 bad_field`. On an adventure room `luigi`, `voice` or `match_players` is `400 bad_field`. Both ride the `view` and the `params` event |
| `start` (VS) | mints `seed` (u32, system RNG) and freezes `luigi`/`voice`/`seed` with the rest of the match. A new seed every match. `start` on an adventure room is **409 `not_vs`** |
| VS go plan | gains `relay_v` (2), `want_seat` (= `slot`), `luigi`, `seed`, `voice`, `digest` |
| adventure rooms | a hub code minted once at create for the room's life; the plan arrives in the create/join answer and on every poll; no start/ready gate; the room outlives its creator |
| `world`, `world_ping` | the always-on world pool (below) |
| `/health` | `contract_min` 5, `contract_v` 6, plus `relay_v`, `world_open`, `game_ids` (how many builds are listed), `worlds`, `world_seats`, `world_member_s`, `world_addr_tokens` |

**The VS plan, and what the launcher exports from it** (v5 keys marked):

```json
{"role":"child", "code":"M7KQ2PXR", "relay":"135.148.26.201:41234",
 "session_type":"vs", "map":1, "players":4, "slot":2, "spawn_delay_ms":1500,
 "playing":true, "names":"tango,opie,ace,nn", "match_players":4,
 "colors":",,,", "relay_v":2, "want_seat":2, "luigi":1, "seed":3735928559,
 "voice":1, "digest":909114140, "star_target":3}
```

| plan key | env |
|---|---|
| `luigi` | `SM64DS_VS_LUIGI_INFECTION` |
| `seed` | `SM64DS_VS_LUIGI_SEED` (decimal u32) |
| `voice` | `SM64DS_VOICE_ALLOWED` |
| `digest` | `SM64DS_SESSION_DIGEST` (decimal u32) |
| `relay_v`, `want_seat` | informational: no env today. The game already speaks relay v2 with v1 fallback, VS takes its seat from `SM64DS_COMMS_SLOT` (= `want_seat`) and the hub asks for any seat (255) |

**The session digest** is the number the game's ACCEPT check computes from its
own environment (port/hal/comms_loopback.cpp, the NETCORE1 session check), so
a console launched with a plan that disagrees with the host's leaves at the
door (exit code 3) instead of desyncing. FNV-1a 32 (`h = 2166136261; h ^= b;
h *= 16777619`) over the ASCII string

```
map|players|win|target|king|luigi|seed|netmode|wire
```

`win` is `time`/`stars`/`king`; `target` is the star target in stars mode,
else 0; `king` the king target in king mode, else 0; `seed` the seed when
`luigi` is 1, else 0; `netmode` is the plan's `netmode`, `lockstep` or `arena` (see ARENA MODE below);
`wire` is 2 for four players or fewer, else 3. Decimal, no padding, no
spaces. `"1|4|stars|3|0|1|3735928559|lockstep|2"` is `909114140`. Pinned in
`test_units.py::test_session_digest`, and the selftest recomputes every plan's
digest with an implementation of its own.

**Adventure rooms.** `create` with `session_type:"adventure"` mints the room's
hub code once (8 characters of the room alphabet, never the room code). The
create and join answers carry the plan, and so does every poll:

```json
{"playing":true, "session_type":"adventure", "code":"Q4WXN8ZT",
 "relay":"135.148.26.201:41234", "relay_v":2, "want_seat":255}
```

The launcher boots the TITLE with `SM64DS_ADVENTURE=1`, `SM64DS_COMMS_CODE`
and `SM64DS_COMMS_RELAY`, and exports no `SM64DS_LEVEL` and no `SM64DS_VS_*`:
each player plays their own save. `want_seat` 255 asks the relay for any free
hub seat. Everybody in an adventure room is "playing" (there is no dial). When
the host leaves, the host role (kick only) passes to the lowest seat still in,
with a `host` event; the room stays open. An adventure room is exempt from
`ROOM_IDLE_S` and closes only after `ADV_EMPTY_S` (600 s) with nobody in it.
The heartbeat still applies, so a launcher keeps polling while its game runs.

**PARTY AND COOP ROOMS (contract v6; run mp1 FIELD5, lane ROOMS1).** Two more
hub rooms next to the adventure room, made for the launcher's "Create party
room" and "Create coop room" buttons. A party room shares stars and bodies
between friends; a coop room is a private world with the host's save and a
fully synced world. The lobby carries neither idea: it seats up to four
friends under a six-letter code, hands each one the room's hub code, and lets
the host release the room.

| where | what |
|---|---|
| `create` | `session_type:"party"` or `"coop"`, **v6 only** (a v5 caller gets `400 bad_session_type`, which every launcher words as "Check for an update"). A coop create also carries **`save_name`**, required there and `400 bad_field` anywhere else: 1 to 24 bytes of printable ASCII, no leading or trailing space, and none of `\ / : * ? " < > \|` (`400 bad_save_name`, or `too_long`). It is opaque display text; the save's **path never reaches this service** (a `save_path` or any other unknown key is `400 bad_field`), is never stored and is never in an answer |
| `create` settings (ROOMSET1) | optional `settings`: an object of toggles, party and coop only. Each is `0` or `1`. Party takes `collision`, `interact`, `damage`, `starshare`; coop takes `collision`, `interact`, `damage`, `tp`. Defaults: collision 1, interact 1, damage 0, tp 1, starshare 1. Absent or `{}` means the defaults. A name of the other kind or an unknown name is `400 bad_setting_key`; a value that is not the int 0 or 1 (a bool, a float, a string) is `400 bad_setting_value`; a non-object is `400 bad_settings`. On a VS or adventure create it is `400 bad_field`. Refused creates make no room |
| `settings` (host only, ROOMSET1) | `{"v":6,"room":"…","token":"…","settings":{"damage":1}}`. A partial update: the names sent change, the rest keep their values. Answers `{"cursor":n,"settings":{…}}` and pushes `{"kind":"settings","settings":{…}}`. A guest is `403 not_host`; a VS or adventure room is `409 no_room_settings`; after Start it is `409 already_started`; a bad value or name is refused and changes nothing. A v5 caller gets `400 bad_field` (the verb is v6) |
| the settings in every answer | party and coop views carry `settings`, and so does every party and coop plan (create, join and poll): the same toggle map for every seat. The launcher turns each into `SM64DS_ROOM_<NAME>=<0\|1>` (`COLLISION`, `INTERACT`, `DAMAGE`, `TP`, `STARSHARE`) |
| seats | `PARTY_SEATS` and `COOP_SEATS` (both 4, 2..16), apart from `MAX_SEATS`. The fifth friend is `409 room_full`. The view carries `room_seats` |
| `join` | a party or coop room needs v6 (`400 bad_session_type` for a v5 launcher, which would boot it as a plain adventure room); everything else is the VS and adventure join (`game_mismatch`, `unknown_game`, kicks, budgets) |
| the plan | the create and join answers and every poll carry the hub plan, like an adventure room's, plus `host` (this seat is the host), `started`, `launch` and `room_seats`; coop adds `save_name`: `{"playing":true,"session_type":"coop","code":"Q4WXN8ZT","relay":"...","relay_v":2,"want_seat":255,"host":true,"started":false,"launch":0,"room_seats":4,"save_name":"Tango file"}` |
| `start` (host only) | the host's **Play**. No arming, `ready`, `go` or match id. It sets `started`, adds one to `launch`, and pushes a `started` event `{"kind":"started","launch":n}`. A launcher launches its game once per `launch` number; a friend who **joins later** is handed a plan that is already `started` and launches on joining. The host may start alone. Pressed again it is launch n+1: everybody whose game is closed relaunches. A guest is `403 not_host`; extra fields are `400 bad_field` |
| the other VS verbs | `params` is `409 not_vs`; `ready`, `result` and `failed` have no match to act on and are `409 stale_match` for anyone (a forged one changes nothing) |
| the host leaves | **coop: the room ends**, `closed` with `why:"host_left"` (the host's PC holds the save; a guest keeps no copy). **Party: the room goes on** and the host role (Play, kick) passes to the lowest seat in, with a `host` event. Both also apply when the host's launcher goes silent for `MEMBER_TIMEOUT_S` |
| idle | exempt from `ROOM_IDLE_S` while anybody polls; an empty party room closes after `ADV_EMPTY_S` |
| the view | `started`, `launch`, `room_seats`, `settings` (and coop `save_name`) are added to party and coop views only; VS and adventure views are byte-identical to v5 |
| what the launcher exports | party: `SM64DS_ROOM_KIND=party`, `SM64DS_PARTY=1`; coop: `SM64DS_ROOM_KIND=coop`, `SM64DS_COOP=1`, `SM64DS_COOP_SAVE_NAME`, `SM64DS_SKIP_MENU=1`, `SM64DS_BOOT_FILE=0`, and on the host only `SM64DS_COOP_HOST=1` and `SM64DS_COOP_SAVE=<path the host chose, which only the host's launcher knows>`; on top of the adventure hub set (`SM64DS_ADVENTURE=1`, `SM64DS_COMMS_RELAY`, `SM64DS_COMMS_CODE`, identity). A world launch carries none of the party and coop names. The game reads them; the lobby never sees them |

**VS joining, leaving and results (run mp1, L-J1).** Tango's rules for 1.0:
a joiner waits in the room and is in from the next match; no mid-match drop-in;
the host leaving mid-match ends the match for everybody and the next player is
host; one player's result does not end the match for the room. No field name
changed and no version moved: only states and reasons widened.

| what | before L-J1 | now |
|---|---|---|
| `join` while the room is `arming`/`go`/`in_match` | 409 `in_match` | **200, a WAITING seat**: `playing:false` for the match that is on, in no plan, slot map or names string; it sees the view, the roster and the chat. Every lobby cap still holds (join budget, `MAX_SEATS` -> `room_full`, kick cooldown, `game_mismatch`, `unknown_game`) |
| a playing seat leaves during a match | the dial promoted a watcher into the running match; its go plan said child, slot 0 (the host's) | **nobody is promoted while a match is frozen**; the vacancy is filled when the match ends |
| a plan for a seat not in the frozen slot map | slot 0 by default | `{"playing":false}`; `ready`/`failed`/`result` for the current match need a frozen, playing seat (else 403 `not_playing`) |
| the match ends (`result`, `failed`, a deadline, a timeout) | watchers and dropped players stayed out until the host touched the dial | **the dial runs**: waiting and dropped seats play the next match, lowest seats first, up to the host's dial |
| the VS host leaves (`leave`, or 45 s without a poll while in lobby) | the room closed, `closed` `why:"host_left"` | a match that is on ends: `failed` with **`reason:"host_left"`** (server-minted; 400 `bad_reason` from a client). Then the host passes to the **lowest seat that was playing the match that ended** (with no match on, or none of its players left: the lowest playing seat; never a joiner who waited through the match) with a `host` event (`{"kind":"host","seat":n,"display":...}`, the adventure rooms' event). The room stays open. Only the LAST member out closes it (`closed`, `why:"host_left"`) |
| slot 0 | the lowest playing seat, which was always the host (seat 1) | **the host, by name**, then everybody else in seat order (the host is no longer always seat 1) |
| `result` for the current match | the FIRST valid result from any playing seat ended the match | **the host's result ends it.** A non-host's result is a vote: accepted 200, and it ends the match only when strictly more than half of the match's live seats (frozen, still playing, still in the room) have posted the same `win` and `scores`. A drop or a leave re-counts. A seat's first vote stands. A late result for an ended match is still 200. **If every live non-host seat posted the same result and the host has sent no request since the first of those votes for `HOST_SILENT_S` (60 s; the env accepts 45 to 3600), the match ends with that result** (a pair whose host launcher died at MATCH OVER; with three or more live seats the guests agreeing is already a majority). Silence while the host's address is being refused does not count, whatever refused it (the rate limiter, and any other refusal such as a 403 or 409 answer): the clock waits, and it runs from the later of the first vote and that address's last refusal (a guest on the host's own public address cannot flood the host quiet). If the table of refused addresses is full, a refusal it cannot record holds every room's clock the same way |

A waiting seat reads as `playing:false` in the view while `state` is not
`lobby`; after the match it is `playing:true` (or still `false` if the host's
dial is below the head count). A 200 from `result` no longer means the match
ended: the `result` event and the view's `state` say that.

**World rooms.** Nobody creates a world; a launcher asks for one:

```
-> {"v":5, "nick":"tango", "game":"1.0.0"}               (+ color/shoes, + "skip":"<code>")
<- 200 {"v":5, "code":"H3PRK7VA", "relay":"...", "relay_v":2, "want_seat":255,
        "world_no":1, "token":"<32 hex>", "members":7}
-> world_ping {"v":5, "code":"H3PRK7VA", "token":"<32 hex>"}   every 10 s while the game runs
<- 200 {"v":5, "ok":true, "world_no":1, "members":7}
```

The pool is per game build. `world` answers the lowest-numbered world with
fewer than `WORLD_SEATS` (16) members seen in the last `WORLD_MEMBER_S` (30 s),
and opens the next number when all are full. `skip` names a world the relay
just refused as full (the game exits with code 4 and the launcher asks again):
that world is passed over. A member unheard for 30 s is forgotten, and its
`world_ping` answers `403 not_a_member` (ask `world` again); a code the pool no
longer has is `404 no_such_world`. A world with nobody in it for
`WORLD_EMPTY_S` (30 s) with no live member is dropped and its number is
reused. The nick and colours are grammar-checked and then dropped: a world
holds a number, a hub code and per token `[last seen, issuing address]`, and
the address is used only for the per-address cap, never logged or sent.

**What bounds the pool** (review REVLOBBY5, S2: before these, one home
connection filled all 64 worlds in about a minute with made-up build ids):

- **`GAME_IDS`, the build allow list.** Only listed builds get a room or a
  world (`409 unknown_game` otherwise). The release step writes the list into
  `docker-compose.yml`. **Empty (the default) turns the world pool off** (`503
  world_off`) while code rooms take any build, so forgetting the line fails
  safe.
- **`WORLD_ADDR_TOKENS` (4) live members per client address.** A `world` call
  from an address already holding four retires that address's least recently
  heard token (its next `world_ping` is 403 and the launcher asks again), so a
  household of four and a relaunch both work, and one address can never stand
  for more than four members however many calls it makes.
- The per-address join budget (60/min), `MAX_WORLDS` (64) across every build,
  `RELAY_V=1` (pool off), and the 30 s empty drop. The lobby never talks to the
relay, so it cannot see the relay's own seat count; `skip` is the answer to
that race.

New knobs (all in `docker-compose.yml` with these values):

| knob | default | what |
|---|---|---|
| `RELAY_V` | 2 | the relay protocol plans name; 1 if the relay v2 deploy is rolled back (the world pool is then off) |
| `GAME_IDS` | empty | the game builds served, comma-separated; empty = any build for rooms, world pool OFF. The release step sets it |
| `WORLD_ADDR_TOKENS` | 4 | live world members one client address may hold |
| `ADV_EMPTY_S` | 600 | an empty adventure or party room closes after this |
| `PARTY_SEATS`, `COOP_SEATS` | 4, 4 | seats in a party room and in a coop room (2..16; the game's hub holds four bodies today) |
| `WORLD_SEATS` | 16 | members per world |
| `WORLD_MEMBER_S` | 30 | a world member unheard this long is not counted |
| `WORLD_EMPTY_S` | 30 | a world with no live member is dropped after this |
| `MAX_WORLDS` | 64 | worlds in the pool, every build together |

**ARENA MODE: the `netmode` of a VS match (run mp1, lane A4).** The new VS
netcode (every console runs its own Yoshi with no input delay, the others are
drawn slightly behind, and a referee on the relay settles stars, hits, the
clock and the winner) is chosen per match by the lobby. No field was renamed
and no version moved. Lockstep stays as the fallback for one release.

| where | what |
|---|---|
| VS go plan | gains `netmode`, `"arena"` or `"lockstep"`, on every playing VS plan. A plan without it (an older server) means lockstep. Spectator and adventure plans never carry it. Every other key keeps its meaning: in an arena match `slot` is the player's body and relay seat (`want_seat` = `slot`), `role` is only the lobby's name for slot 0 (there is no host inside an arena match), and `code` is still fresh per match |
| digest | the canonical string's `netmode` term is the plan's `netmode`: `"0\|2\|time\|0\|0\|0\|0\|arena\|2"` is `1565304087`, `"1\|4\|stars\|3\|0\|0\|0\|arena\|2"` is `1476757693`. A console that runs lockstep computes `lockstep` and can never pair with an arena console |
| launcher env | `netmode:"arena"` adds `SM64DS_VS_NETMODE=arena` (the game's arena switch, port/hal/vs_arena.h) to exactly the VS set it exports today; the game seats itself from `SM64DS_COMMS_ROLE`/`SM64DS_COMMS_SLOT` as before. `netmode:"lockstep"` sets no `SM64DS_VS_NETMODE` (an inherited one is cleared like every `SM64DS_VS_*` the plan does not set), so a lockstep console's environment is unchanged. Any other value, or an arena plan with Luigi on or a king target, is refused before anything spawns |
| relay session | the lobby still never talks to the relay. The game picks the session from the netmode: lockstep opens the v2 star (roles parent/child, seat = slot) as before; arena opens the relay's arena session (relay lane A2) with seat = slot. Every console of one arena match gets the same map, players, target, names, colours and digest, and the digest commits to all of them, so the referee can take the settings from the seats and refuse a seat whose digest differs |
| `params` (VS rooms) | optional `netmode` `"arena"`/`"lockstep"`, the host's pick; absent keeps the room's; anything else `400 bad_netmode`; on an adventure room `400 bad_field`. A new room starts at `NETMODE_DEFAULT`. The `params` event carries the pick |
| `start` | resolves the match's netmode and freezes it with the rest: **arena when** the pick is arena, the room's build is on `ARENA_GAME_IDS`, `RELAY_V` is 2, the match has 2 to `ARENA_MAX_PLAYERS` players, the win mode is `time` or `stars` and, with Luigi on, the build is also on `ARENA_LUIGI_GAME_IDS`; **lockstep otherwise** (King of the Star always, and Luigi Infection for every build not on `ARENA_LUIGI_GAME_IDS`). Picking arena is never an error. The `arming` event carries the resolved `netmode` |
| `view` | `netmode` (the pick, VS rooms), `arena_ok` (this room's build can play arena on this server), `arena_max`, `arena_now` (what Start would resolve to on the current roster and settings), `match_netmode` (the running match's, `null` in lobby) |
| an arena match has no host in it | lockstep matches keep every L-J1 rule. In an arena match (`go` or `in_match`) the host's `failed` drops only the host's seat (`dropped`; the host keeps the room's host role), and the host leaving the room does not end the match: `left`, then `host` (the L-J1B pick: the lowest seat still playing it, never a waiting seat), and no `failed host_left`. If the new host had already posted a result, it ends the match at once. The match ends as before: a `failed` that leaves fewer than 2 players, the (current) host's result, a majority, the host-silent rule, or `MATCH_TIMEOUT_S`; with nobody left playing it, the host leaving ends it with `failed host_left`. During `arming` (nothing has spawned) the host leaving ends it as before |
| `/health` | `arena_game_ids` (how many builds), `arena_max`, `netmode_default` |

| knob | default | what |
|---|---|---|
| `ARENA_GAME_IDS` | empty | the builds that play arena, comma-separated. **Empty = arena off for every build** (this server then behaves exactly as before). `*` = every build, for dev and test boxes only. Written at the relay redeploy that brings the referee; removing it is the rollback |
| `ARENA_LUIGI_GAME_IDS` | empty | the builds whose Luigi Infection matches play arena (the referee's Luigi rules, relay contract 2), comma-separated; a build must be on `ARENA_GAME_IDS` too. **Empty = every Luigi match plays lockstep** (as before). `*` = every arena build, dev and test boxes only. Exact build ids, never a minimum version. Written when the game build that sends READY mode 2 ships, after the relay; removing it is the rollback. The view's `arena_luigi` and /health's `arena_luigi_game_ids` report it |
| `ARENA_MAX_PLAYERS` | 4 | the largest arena match (2..16); bigger matches play lockstep |
| `NETMODE_DEFAULT` | arena | a new room's pick (`arena` or `lockstep`) |

---

## THE WIRE CONTRACT, v1 and v2 (history; v5 above is what runs)

### Rules that apply to every request

| rule | value | on violation |
|---|---|---|
| method | `POST` for all verbs; `GET` for `/health` only | 405 / 404, connection closed |
| URL | exactly `/port/lobby/<verb>`, no query string ever | 404 `unknown_verb` + one log line |
| content type | `application/json` | 415 `bad_content_type` |
| `Content-Length` | REQUIRED, and parsed **before any rejection** | missing -> 411 `length_required`, close |
| request body | <= 4096 bytes | 413 `too_large`, connection closed (body not read) |
| JSON shape | one object; no arrays at the top level | 400 `bad_shape` |
| `v` | REQUIRED, integer, `5` (the contract range `/health` reports) | 400 `bad_version` |
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

### v2 IS THE UNION, and there is no half of it

Two lanes grew the contract on the same night over fields that did not
overlap — the host's player-count dial and the colour picker — and each
called its own addition "v2". Merged unchanged, that would have shipped a
number meaning two different things: a launcher built against either lane
would announce `"v":2` and support half of what v2 is. **A version is a
promise about a whole field set**, so it has exactly one definition, and it
is `VERB_FIELDS` in `app/server.py`.

| version | what it adds |
|---|---|
| v1 | everything through launcher 0.3.0 |
| v2 | `params` may carry `match_players` — the host's player-count dial |
| v2 | `create` and `join` may carry `color` + `shoes` |
| v2 | the **`color` verb exists at all** — a v2-only verb |
| v2 | (answer side, unversioned) `color`/`shoes` on every roster row, a `color` event kind, and `colors` in every playing go plan |

A client claiming v2 is claiming all of it.

**Both versions are answered, and the rule is asymmetric on purpose.** What a
client may SEND is versioned strictly: a `v:1` request carrying `color` or
`match_players` is refused `bad_field`, because a bump a server honours anyway
is decorative. What a client RECEIVES is not: the new keys appear in every
`view` and every plan whatever version asked, because an answer is JSON and the
launcher's reader ignores a property it has no field for — the same leniency
the event `kind` set already relies on, and now pinned by the launcher's own
unknown-field test rather than left to a library default. Versioning the send
side is what protects the server; versioning the receive side would only mean
two code paths that have to be kept in step forever.

So the shipped 0.3.0 launcher, which speaks v1: creates and joins as it always
did, reads a roster with extra strings per row and ignores them, gets a plan
with extra keys and ignores them, never sets `SM64DS_VS_COLORS`, plays with the
ROM's built-in Yoshi in a room where everybody else is wearing a colour, and
seats people by the room's dial without ever knowing there is one. Proved over
real HTTP in `test_client.py`'s `selftest_colors` and `dial`.

**Where a version lives, and why you cannot forget to gate a field.**
`VERB_FIELDS` is the one place a verb's field set is written down:
`(required, {optional_field: the version it arrived in})`, and optionally a
third element — **the version the VERB ITSELF arrived in**, for a verb like
`color` that did not exist at v1 at all. `shape_for` is the only thing that
reads it, so a handler cannot accept a field the table does not list, and
cannot list a field it does not gate. `test_units.py`'s
`test_every_versioned_field_is_gated` **walks the table** and proves every
field above the floor is refused below its own version, and every late verb is
refused to an earlier caller — it names no field and no verb, so anything added
tomorrow is covered by it the day it is added. Adding a versioned field is
therefore two edits and no new test: the table row, and a sample value in the
walk's `_FIELD_SAMPLES` (omit the sample and the walk fails and says so, rather
than quietly skipping the field).

A verb out of the caller's version is refused `bad_field`, not `bad_version`,
and deliberately: a v1 caller cannot tell a v2 verb apart from a misspelled
one, so the refusal gives away nothing about what the server grew. The player
sees the same sentence either way.

Required fields are v1 by construction. A later version cannot make a field
mandatory without breaking every earlier client, so a new required field is a
new verb, not a new field.

**SUNSETTING `CONTRACT_MIN`.** The floor exists to keep launchers already in
the wild working, not forever. Raise it only when all three are true, and say
so in the commit that raises it:

1. The launcher that needed the old floor is **no longer in circulation** — the
   auto-updater has carried everyone past it, and the release it shipped in is
   at least two releases back.
2. The old floor's fields are **not the only way to express something**. A
   version is retired, never a capability.
3. Raising it is **its own commit**, with nothing else in it, because it is the
   one change here that can lock a real player out of a room.

The cost of leaving the floor low is a few lines in a table and one extra
column in the walk above — deliberately cheap, so nobody is ever tempted to
raise it early. `/health` reports `contract_min` so an operator can see what a
box still accepts without reading the source.

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
| `v` | int | `5..5` (the contract range `/health` reports) | `bad_version` |
| `room` | string | exactly 6 chars from `ABCDEFGHJKMNPQRSTUVWXYZ23456789` | `bad_room` |
| `token` | string | exactly 32 chars from `0-9a-f` | `bad_token` |
| `nick` | string | 1..16 **bytes**, every byte `0x20..0x7E` **except `,`**, not all spaces, no leading/trailing space | `bad_nick` |
| `text` | string | 1..200 **bytes**, every byte `0x20..0x7E` | `bad_text` |
| `cursor` | int | 0 .. 2^31-1 | `bad_cursor` |
| `wait` | int | 0..25 seconds; **clamped, not rejected** | - |
| `map` | int | 0..3 | `bad_map` |
| `win_mode` | string | exactly `"time"`, `"stars"`, or `"king"` | `bad_win_mode` |
| `star_target` | int | 1..`STAR_TARGET_MAX`; required iff `win_mode == "stars"`, forbidden otherwise | `bad_star_target` |
| `king_target` | int | 1..`KING_TARGET_MAX`; required iff `win_mode == "king"`, forbidden otherwise. Launcher default `KING_TARGET_DEFAULT` (30) | `bad_king_target` |
| `seat` | int | 1..`MAX_SEATS`, an occupied seat that is not the host's | `bad_seat` |
| `match_players` | int | 2..`dial_max`; optional on `params`, and **v2 only** | `bad_match_players` |
| `pre_ok` | bool | `true`/`false`; optional on `create` and `join` | `bad_field` |
| `color` | string | **v2 only.** exactly 6 hex digits, either case in, stored lower case. The BODY colour, and the swatch beside the name in the roster | `bad_color` |
| `shoes` | string | **v2 only.** the same grammar. Travels with `color`: both or neither | `bad_color` |
| `game` | string | **v5, required on `create`/`join`/`world`.** 1..24 of `0-9A-Za-z._-`, compared byte for byte | `bad_game` |
| `luigi` | int | **v5.** exactly `0` or `1`; optional on `params`, VS rooms only | `bad_field` |
| `voice` | int | **v5.** exactly `0` or `1`; optional on `params`, VS rooms only | `bad_field` |
| `code` | string | **v5** (`world_ping`). exactly 8 chars of the room alphabet | `bad_code` |
| `skip` | string | **v5** (`world`, optional). the same grammar as `code` | `bad_code` |

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
| `POST /port/lobby/color` | a seated member | set its OWN two colours (**v2**, lobby state only) |
| `POST /port/lobby/start` | **host only** | arm a match (stage B); VS rooms only (`409 not_vs`) |
| `POST /port/lobby/ready` | a playing member | "my launcher can spawn this match" (stage B) |
| `POST /port/lobby/failed` | a playing member | report a match that could not run (stage B) |
| `POST /port/lobby/kick` | **host only** | remove a member |
| `POST /port/lobby/leave` | a seated member | give up the seat (VS host: close the room; adventure host: hand it on) |
| `POST /port/lobby/world` | anyone | **v5.** a seat in the world pool: a hub plan and a token |
| `POST /port/lobby/world_ping` | a world member | **v5.** "still here", every 10 s while the game runs |
| `GET  /port/lobby/health` | anyone | liveness, counts, revision |

Stage C adds `result`. Any other path under `/port/lobby/` is a 404 and one log
line.

#### `create`

```
-> {"v":1, "nick":"tango", "pre_ok":true}
-> {"v":2, "nick":"tango", "pre_ok":true, "color":"8a2be2", "shoes":"ffd700"}
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
409 `in_match` if the room is not in the lobby state (until run mp1 L-J1:
since then a join during a match is a waiting seat, see the v5 section).

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
2. **The host starts as seat 1.** The creator takes seat 1. Until run mp1
   (L-J1) a host leaving closed the room, so no live room had a host at any
   other number; since then a VS host leaving passes the role to the lowest
   seat that played the match it ended (else the lowest playing seat), and
   slot 0 goes to the host by name (`assign_slots`), not to the lowest seat.
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
in stars mode; `king_target` is present only in king mode, and the launcher
exports it as `SM64DS_VS_KING_TARGET=<n>` so the host enforces the one-star,
first-to-N-points rules. The host gets `spawn_delay_ms: 0`; children get `1500`
(the parent goes first). A spectator gets `{"playing": false}` and spawns nothing.

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
`MATCH_TIMEOUT_S` (a pair whose guest posted a result while the host went
silent ends `HOST_SILENT_S` after that result instead; see the v5 section).

#### `result` — stage C

```
-> {"v":1, "room":"...", "token":"...", "match":"<16 hex>",
    "win":"star-target", "scores":[3,1,0,0]}
<- 200 {"v":1, "cursor":14}
```

A playing member reporting a finished match. `win` is `time-up`, `star-target`
or `draw` (the launcher posts `draw` with all-zero scores for a `MATCH OVER`
marker it could not parse), or since run mp1 one of the game marker's other
ends, `king-target`, `luigi-all-infected` or `survivor-timeout` (a launcher
posts those only to a lobby whose go plans carry `netmode`). `scores` is four ints, 0..99, in slot order. The
match-ending result (since run mp1 L-J1 the HOST's, or one a majority of the
live match seats agree on, or every guest's after `HOST_SILENT_S` of host
silence; see the v5 section) appends a `result` event, discards the
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

Host (VS, since run mp1 L-J1): a match that is on ends with `failed`
`reason:"host_left"`, the host passes to the lowest seat that played that
match (else the lowest playing seat) with a `host` event, and the room stays
open; the last member out closes it (`closed`,
`why:"host_left"`). Member: the seat is freed, a `left` event is pushed, and
the lowest spectator is promoted if a playing seat opened -- in lobby only; a
seat that opens during a match is filled when the match ends.

#### `health`

```
GET /port/lobby/health
<- 200 {"ok":true,"ts":"...","revision":"lobby-6","contract_min":5,"contract_v":6,
        "dial_max":16,"relay_v":2,"world_open":true,"game_ids":1,"rooms":3,
        "members":7,"worlds":2,"world_seats":16,"world_member_s":30,
        "world_addr_tokens":4,"waiters":5}
```

`rooms` visible from outside is the difference between a detectable outage and
a quiet one.

---

#### `color` (v2)

```
-> {"v":2, "room":"K7QMR3", "token":"<32 hex>",
    "color":"8a2be2", "shoes":"ffd700"}
<- 200 {"v":2, "cursor":19}
```

Sets the caller's own two colours. `"color":"", "shoes":""` puts that player
back on the ROM's built-in Yoshi.

* **Own seat only.** There is no seat argument, and a `seat` key is refused
  `bad_field`, so the verb adds no authority anywhere.
* **Unchanged is a no-op** — 200, no event, no cursor movement. A launcher can
  re-send on every keystroke or on window focus and it costs the room nothing.
* **A real change pushes a `color` event**, so a member holding a 25-second long
  poll sees the swatch move now rather than when its wait expires.
* **`lobby` state only.** Refused `not_in_lobby` (409) once a match is armed.
  This is where it differs from `preflight`: a colour is SPENT at the arming
  freeze, and a swatch that no running game is wearing is worse than a swatch
  that cannot be changed for ninety seconds. Change it before the rematch.

#### `SM64DS_VS_COLORS`, the string the games actually read

Built once, by the server, at the arming freeze, and copied byte-identical into
every playing member's plan — the same discipline `SM64DS_VS_NAMES` gets, for
the same reason: no launcher assembles it, so two launchers cannot disagree.

```
colors := field "," field "," field "," field      exactly three commas
field  := "" | 6*HEXDIG ":" 6*HEXDIG               slot order, 0..3
```

`"8a2be2:ffd700,ff0000:00ff00,,"` is a two-player room where both picked;
`",,,"` is a room where nobody did. Longest legal string is 4x13 + 3 = 55 bytes.

**Four fields today, sixteen later, and never on its own.** Coordinator's
cross-lane ruling of 2026-09-01 (raised by lane SEAT16, answered once for both
variables): `SM64DS_VS_COLORS` mirrors `SM64DS_VS_NAMES` exactly at four fields
and three commas. When the wire supports more than four slots, BOTH move
together in one coordinated version change to sixteen comma-separated fields
(exactly fifteen commas, same per-field grammar). Neither variable ever changes
shape independently of the other.

**No palette ever goes on the wire.** Every copy of the game is told the same
four hex pairs and generates all four sixteen-colour palette rows for itself,
through the arithmetic in `port/hal/vs_palette_gen.h` — which is the tangOS
SM64DS Edition palette editor's own shading maths, transcribed and diffed
bit-for-bit against it. Identical inputs through identical arithmetic give
identical bytes, so there is nothing to relay, nothing to trust, and nothing a
hostile client can inject past six hex digits per colour. A colour also cannot
desync a match: it is decided before the level loads and the simulation never
reads it.

The game refuses the whole variable on any grammar violation, so it is worth
saying what this server can emit: `test_units.py::test_colors` runs 361 hostile
colour pairs through `create`/`join` and every one is refused before it can
reach the frozen string, which is therefore always hex, colons and exactly
three commas.


## Failure semantics

| event | what the server does | what players see |
|---|---|---|
| VS host leaves, or 45 s without a poll in lobby | a match that is on ends (`failed`, `reason:"host_left"`); the host passes to the lowest seat that played that match, else the lowest playing seat (`host` event); the room stays (run mp1 L-J1) | the match ends with a host-left message, everybody is back in the room with a new host |
| the last member leaves | room -> `closed`, `why:"host_left"`; the record is kept 60 s so members get the reason rather than a bare 404, then deleted | "The host closed the room." |
| member leaves | seat freed, `left` event, spectator promoted if a playing seat opened | the name disappears; a system line says who left |
| member stops polling | 45 s heartbeat timeout, treated as a leave with `why:"timeout"` | as above |
| service restarts | **every room is gone.** State is in memory by design | every poll gets 404; the launcher says the room is gone and returns to the create/join screen |
| VS host launcher dies at MATCH OVER (no `leave`) | 3+ live seats: the guests' same result is a majority and ends it at once. A pair: the guest's result ends it `HOST_SILENT_S` (60 s) after it came in, if the host sent nothing since and its address was not being refused; otherwise `MATCH_TIMEOUT_S` | the result shows up a minute late in a pair |
| room idle | 30 minutes with no event -> `closed`, `why:"idle"` | "This room timed out." |
| adventure host leaves | the room stays; host (kick) passes to the lowest seat with a `host` event | nothing changes for anybody playing |
| adventure room empty | closed `why:"idle"` after `ADV_EMPTY_S` (600 s) empty; never on `ROOM_IDLE_S` | a code shared yesterday is gone |
| world member silent 30 s | forgotten; its `world_ping` is 403 `not_a_member` | the launcher asks `world` again |

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
| world calls | charged as joins (60/min) | per address | `RATE_JOIN_PER_MIN` |
| worlds | 64, every build together | server | `MAX_WORLDS` |
| world seats | 16 | per world | `WORLD_SEATS` |

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

## Host events: the admin endpoint (EVENTS1, EVENTS2)

Off by default and indistinguishable from an unknown verb: both endpoints answer `404 unknown_verb` unless ALL of these are in the lobby's environment:

| Variable | Meaning |
|---|---|
| `EVENT_ADMIN_TOKEN` | the host's secret, 24+ characters, sent as `Authorization: Bearer <token>`. Never in the game zip, never in a log line. |
| `EVENT_KEY` | 16+ characters, the same string as the relay's `SM64DS_EVENT_KEY`. The lobby signs each command with it. |
| `EVENT_CMD_PORT` | the relay's command port (`SM64DS_EVENT_CMD_PORT`). |
| `EVENT_CMD_HOST` | default `127.0.0.1`. The relay's command socket is loopback only, so a relay on another machine needs a tunnel (or use `relay/event_send.py` on that machine). |

A wrong or missing token is `403 forbidden` and counts toward the lobby's normal "refused too often" rule. The lobby never forwards anything a player sent:
the body is checked field by field and re-encoded.

`POST /port/lobby/admin/event`, JSON, always `"v": 1`:

| field | meaning |
|---|---|
| `event` | `message`, `music`, `speed` (triple speed), `onehit`, `chaos`, `race`, `castle` (every star and key door open, nothing saved), `clear` |
| `text` | the message, 1..70 plain ASCII characters (message only) |
| `sublevel` | the course whose song plays (music only), 0..255 |
| `secs` | 0..3600; 0 = 8 s for a message, 60 s for the rest |
| `world` | optional: a world NUMBER from `GET admin/worlds` or its 8 character hub code; absent = every world. A number that two builds share is `ambiguous_world` |
| `stars` | optional race target, 1..150 (race only; 0 or absent = 50) |

Answers: `200 {"ok": true}`; `400` with `bad_shape`, `bad_event`, `bad_sublevel`, `bad_secs`, `bad_text`, `bad_stars`, `bad_world`, `unknown_world`,
`ambiguous_world`; `403 forbidden`; `404 unknown_verb` (feature off); `502 relay_unreachable`.

`GET /port/lobby/admin/worlds` (same token, same 404 / 403 rules) lists the live worlds: `{"worlds": [{"no": 1, "code": "ABCD2345", "members": 7}]}`.

Examples (the token and key are yours to make; keep them out of shell history):

    curl -s -X POST https://HOST/port/lobby/admin/event -H "Authorization: Bearer $TOKEN" -d '{"v":1,"event":"castle","secs":600}'
    curl -s -X POST https://HOST/port/lobby/admin/event -H "Authorization: Bearer $TOKEN" -d '{"v":1,"event":"race","secs":900,"stars":50,"world":1}'
    curl -s -X POST https://HOST/port/lobby/admin/event -H "Authorization: Bearer $TOKEN" -d '{"v":1,"event":"message","text":"Welcome to the event"}'
    curl -s -X POST https://HOST/port/lobby/admin/event -H "Authorization: Bearer $TOKEN" -d '{"v":1,"event":"clear"}'
    curl -s https://HOST/port/lobby/admin/worlds -H "Authorization: Bearer $TOKEN"

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
	# SM64DS multiplayer LOBBY - OWNED BY ~tango/sm64ds-lobby/ (added <date>).
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

It is the only ceiling here that is purely a format the two sides agree on -
no ROM, no wire. That does **not** make it independently movable, and the
coordinator has ruled on exactly that, because lane VSCOLOR-UI's
`SM64DS_VS_COLORS` would otherwise have frozen a field count of its own.

**THE COORDINATOR'S FIELD-COUNT RULING, verbatim:**

> NAMES + COLORS exactly-4 today, with the same per-field grammar (exactly 15
> commas); future = 16 fields together in one coordinated version change when
> the wire moves; never independently.

So both strings stay at exactly four fields **and share one per-field
grammar** — the sixteen-field form is exactly 15 commas, the same shape read
the same way by both readers, which is the half of the ruling that stops the
two strings from being parsed by two slightly different splitters. When
the wire moves (section 1), the two go to sixteen **together**, in one version
change. The reason is the failure this avoids: a sixteen-field names string
against a four-field colours string is two readers disagreeing about who slot 5
is, and the symptom would be a wrong name under a wrong colour on a results
screen, which is the hardest kind of bug to trace back to a format.

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

**If someone picks this up, the order is:** the wire first (live mask, packet,
port span) -> then the Ctrl band -> then `SM64DS_VS_NAMES` and
`SM64DS_VS_COLORS` to sixteen fields **together**, per the ruling in section 3
-> then the port's own scoring and marker -> and only then the conversation
about the ROM.

The names string is the smallest piece of work on that list and it is
deliberately **not** first: the ruling gates it on the wire moving, because a
format that grows before anything can use it is two readers waiting to
disagree.

---

## WHAT RUN vs16 DID TO THAT LIST, AND THE ONE WALL LEFT

The section above is SEAT16's map, measured at cons `339db6bb3`, and it is kept
as written rather than edited in place -- it is the reasoning the next lane
started from, and a map quietly rewritten to match the territory teaches
nobody anything. This section says what changed.

**Three of its conclusions were wrong, all in the same direction.** Run vs16
re-derived every item before moving it:

1. **`data_0209f310` was never a four-byte ceiling in the port.** The host
   already gives that band 32 bytes as a guarded contiguous pair
   (`hal/actor_classes_star.cpp`), and the ROM's own summing reader
   (`src/NumVsStarsObtained.c`) already walks `data_0209f21c` rather than a
   literal four. Sixteen scores needed no change at all.
2. **The four-column star band is the RESULTS SCREEN and it does not bind.**
   `4 - count` / `data_ov075_0211c6e8[d]` is `src/func_ov075_0211621c.c`,
   reached only from scene 7 -- which `hal/scene_boot.cpp` does not register
   and `hal/scene_vs_menu.cpp` explicitly leaves out. The port's match end is
   its own winner banner. The `d == -1` read is real and is a standing reason
   not to register scene 7; it is not on any path the port runs.
3. **Most of the ROM's per-player loops were already generic.**
   `NumVsStarsObtained`, `NumRedCoins`, `NumCoins`, `ResetInput`,
   `SetTouchScreenDelay`, `Actor::ClosestPlayer`,
   `Actor::ClosestNonVanishPlayer`, `LoadEntranceObjects` and
   `Stage::CheckInput`'s main loop all walk `data_0209f21c`. Even
   `func_020308d0` (SetNumPlayers) is generic upward.

**And the wall it named as the deepest was narrower than it looked.**
`mPlayerNo` is two bits *where the spawn flags pack it*. The destination at
`Player + 0x6d8` is a whole byte, and every ROM reader that identifies a player
reads that byte. So the port packs the flag word with a value the ROM's
arithmetic can carry, lets `Player::InitResources` run exactly as it always
has, and writes the true slot over the truncated one the instant `Actor::Spawn`
returns. No `src/` edit, and 2P/3P/4P pack the identical flag word they always
did.

### What is built and proved

- **A second wire.** Four players or fewer is version 2 at 0x90 bytes, byte for
  byte what every shipped build sends. Five or more is version 3 at 0x214
  bytes, sixteen blocks and a 32-bit live mask. A peer on the wrong wire is
  refused, and the refusal is SENT rather than only logged -- as a
  narrow-length packet carrying the new version, so the old build's own version
  check prints the diagnosis in the log of the person holding the stale build.
- **Sixteen-wide hosting** for every per-slot global the port hosts, with the
  two placed grouped-section bands grown by their TAIL member only, so every
  offset `gxband_guard.py` checks is untouched.
- **Deterministic spawns** for slots past the level's own records: concentric
  rings of five at 96, 192 and 288 units, half-step rotated, identical on every
  peer with nothing on the wire.
- **Names and colours at sixteen fields**, moved together with the wire exactly
  as the ruling in section 3 requires, across the game's two readers, the
  service's two builders and the launcher's two validators -- one commit set,
  none of the five alone. The grammar is exactly 3 commas or exactly 15,
  nothing between, so one shape cannot be mis-parsed as the other.
- **Contract v3** on the service, gating the dial's RANGE rather than the
  field: a v2 caller is capped at four, a room is never wider than its host can
  drive, and a v2 client joining a wide room is refused `needs_newer_client` at
  the door rather than at spawn time.

### THE ONE WALL LEFT, and it is section 6 after all

**`src/func_0203ea5c.c` -- the ROM's own wireless exchange and unpack loop --
is the thing that stops a fifth player actually playing.** It carries nine
hard-coded `< 4` bounds (`:310, 326, 347, 352, 367, 403, 428, 471, 499`), and
the effect is precise and reproducible: a peer at slot 4 or above pairs, is
accepted into the session, exchanges rounds correctly, spawns its own body and
every other player's body in the right place with the right identity -- and
then its WORLD never enters lockstep. It sits in the ROM's own wait until that
wait's bound expires and then runs on alone.

Measured, not inferred, at five and at eight windows: slots 0..3 are in exact
lockstep with zero divergence over every pairing; slots 4 and above report
their first world frame at the session's LAST round, with the transport having
completed the entire run's worth of exchanges underneath them.

That function is byte-matched ROM code, so it cannot be edited under the port's
own north star. Moving it means HOSTING it -- replacing that one function in
the port's host layer the way the port already hosts many others -- which is a
deliberate deviation of exactly the kind the owner has already approved for
online play, and is the next lane's whole job rather than a line at the end of
this one.

**So the honest sentence: sixteen players pair, spawn, and are correctly named,
coloured, scored and identified. Four of them play.**
