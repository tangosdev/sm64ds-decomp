# SM64DS port UDP relay

A small server that lets two people play the SM64DS PC port together over the
internet without either of them touching their router.

## Why this exists

Two players behind ordinary home routers cannot send packets straight to each
other. Both routers block anything that arrives out of the blue. The usual fix
is port forwarding, which most people will not do and some cannot do at all.

So neither side waits for a connection. Both sides dial **out** to this relay,
which sits on a public box with one open port. Because each player opened the
conversation, their own router is happy to let the answers back in. The relay
notices that two players quoted the same session code, pairs them up, and from
then on passes their packets along.

It is deliberately dumb. It never looks inside a packet it forwards, never
stores anything on disk, and never speaks to anyone who has not spoken first.

## The wire contract

Frozen. The game side and this relay were built against exactly this, and
neither may drift from it without the other.

**HELLO** (client to relay), 16 bytes:

| offset | size | field                                        |
|--------|------|----------------------------------------------|
| 0      | 4    | magic `SMRC`                                 |
| 4      | 1    | version, currently 1                         |
| 5      | 1    | role: 0 parent, 1 child                      |
| 6      | 2    | reserved, send 0                             |
| 8      | 8    | session code, ASCII, right padded with NUL   |

**HELLO-ACK** (relay to client), 16 bytes:

| offset | size | field                                        |
|--------|------|----------------------------------------------|
| 0      | 4    | magic `SMRA`                                 |
| 4      | 1    | version, currently 1                         |
| 5      | 1    | status: 0 ok, 1 session full, 2 bad request  |
| 6      | 2    | reserved, always 0                           |
| 8      | 8    | the code, echoed back                        |

Rules both sides rely on:

- A client repeats its HELLO about once a second until it gets an ACK. That
  same repetition keeps the hole in its router open, so it never stops.
- A repeat HELLO from an endpoint the relay already knows is harmless. It is
  answered again and the pairing is left exactly as it was.
- A session is one code with one parent and up to three children.
- Once paired, every datagram that is not a HELLO is forwarded **verbatim**:
  parent to all children, child to the parent only. Never child to child.
- The relay does not read, rewrite, or reorder payloads. They are opaque.

Two details the game side has to respect, both a consequence of the contract
above rather than choices made here:

- A forwarded packet must not be exactly 16 bytes starting with `SMRC`, or the
  relay will read it as a HELLO. Any other size or any other first four bytes
  is fine.
- The reserved field is not policed. Whatever arrives in it is ignored, and
  zeros are always sent back.

## Limits, and what happens when you hit one

Every one of these is enforced, and every one has a test.

| Limit                        | Value | On breach                                  |
|------------------------------|-------|--------------------------------------------|
| Forwarded payload            | 700 B | Larger datagrams are dropped in silence     |
| Rate per endpoint            | 120/s | Excess dropped; a 120 packet burst is fine  |
| Sessions at once             | 64    | A new code gets status 1                    |
| Endpoints per session        | 4     | 1 parent + 3 children; the next gets status 1 |
| Idle before a seat is dropped| 90 s  | The player must HELLO again to get it back  |
| Malformed HELLO              |       | One status 2 reply, then that address is ignored for 10 s |

120 packets a second is generous for 30 frames a second of lockstep play plus
retries. A player sending more than that is either broken or malicious, and
either way the fix is the same.

The relay never sends more bytes to an unknown address than it just received
from it. That is what stops it being used to bounce amplified traffic at
somebody else, and it is measured in the test suite rather than assumed.

## Install

The kit is this directory. Copy it to the server and run the script there:

    scp -r relay/ youruser@your.server:/tmp/sm64ds-relay-kit
    ssh youruser@your.server
    sudo bash /tmp/sm64ds-relay-kit/deploy.sh

`deploy.sh` echoes every step as it goes. It:

1. checks python3 is present and new enough, and that relay.py compiles,
2. creates a locked down `sm64ds-relay` account if it is not already there,
3. copies relay.py into `/home/sm64ds-relay/sm64ds-relay/`,
4. writes the systemd unit, reloads, enables and starts it,
5. checks the service is up, shows the listening socket and the last log lines,
6. sends one HELLO to itself and reports the answer.

Run it again any time. It only changes what actually differs, so rerunning it
is the normal way to push a new relay.py. `--dry-run` shows what it would do
and changes nothing. `--port`, `--user` and `--idle` override the defaults.

Requirements on the server: python3 (3.7 or newer) and systemd. No packages to
install, no dependencies, nothing written to disk at runtime.

## Firewall

**Only UDP 41234 needs to be open.** Nothing else. The relay makes no outbound
connections of its own and needs no other port.

`deploy.sh` deliberately does not change firewall rules. Run whichever of these
matches the box:

ufw:

    sudo ufw allow 41234/udp comment 'SM64DS relay'
    sudo ufw status verbose

raw iptables (add the ip6tables line too if the box has IPv6):

    sudo iptables -A INPUT -p udp --dport 41234 -j ACCEPT
    sudo ip6tables -A INPUT -p udp --dport 41234 -j ACCEPT
    sudo iptables -L INPUT -n --line-numbers | grep 41234

If the host also sits behind a cloud provider firewall or security group,
allow UDP 41234 inbound there as well or nothing will ever arrive.

## Verify

    ss -ulnp | grep 41234
    journalctl -u sm64ds-relay -n 20
    systemctl status sm64ds-relay

Watch two players connect live:

    journalctl -u sm64ds-relay -f

From any machine that has this kit, against the public address:

    python3 test_client.py probe --host YOUR.SERVER.IP --port 41234
    python3 test_client.py remote-check --host YOUR.SERVER.IP --port 41234

`probe` sends one HELLO and prints the answer and the round trip. `remote-check`
pairs a parent and a child through the relay and measures 100 round trips, so a
clean run proves the whole path end to end, not just that something is
listening.

## Two players in one VS match through this relay

The game side, one line per machine. The two must share the code and must not
share the role.

    SM64DS_VS_MAP=0 SM64DS_COMMS_RELAY=your.server:41234 \
      SM64DS_COMMS_CODE=SOMECODE SM64DS_COMMS_ROLE=0    walk_window.exe

    SM64DS_VS_MAP=0 SM64DS_COMMS_RELAY=your.server:41234 \
      SM64DS_COMMS_CODE=SOMECODE SM64DS_COMMS_ROLE=1    walk_window.exe

`SM64DS_COMMS_ROLE` takes the role numbers from the datagram table above (0
parent, 1 child) or the words `parent` and `child`; they mean the same thing.
`SM64DS_VS_MAP` is 0 to 3 over the ROM's own four VS maps. A host name works
here as well as an address.

It worked when both logs carry these, which the game writes into
`playlog/play_*.log` unless `SM64DS_NO_PLAYLOG=1` is set:

    [comms:loopback] open(mode=2) as parent, ... via RELAY ..., code 'SOMECODE'
    [comms:relay] paired as parent on code 'SOMECODE'; the relay will forward
    [comms:conductor] session up after N turns: ... players=2

Any `[comms:loopback]` line saying NOT installing is the game refusing before
it ever reached the relay, and the line names the knob it refused. The whole
path is asserted end to end by `port/tools/vs_online_proof.py`.

## Why it does not run at a round trip per frame

The match is lockstep: both machines run the same frame from the same pair of
inputs, which is what keeps two copies of the game telling the same story with
no guessing and no rewinding. Done the obvious way that means a frame cannot
finish until the other player's input for that same frame has crossed the wire
and come back, so **the frame rate is the round trip**. Through this relay that
is around 90 ms, which is about 11 frames a second against the game's 30, and
it feels exactly as bad as it sounds.

The fix is an **input delay**. Each machine runs frame R from the inputs that
were exchanged N frames ago, so the wire has N whole frames to deliver a packet
instead of a fraction of one. As long as **both machines use the same N**, they
still play the identical sequence of inputs in the identical order -- nothing
is predicted, nothing is thrown away, and the two games cannot drift apart.
That "same N" is not a suggestion; see the next section for what happens
otherwise and how the game now guarantees it. The only cost is that a button
press lands N frames later, and N frames of that is far less noticeable than
the whole game running at a third speed.

**It is on by default and the recipe above needs no extra knob.** The game
picks the depth from how it is connected: 5 frames through a relay, 2 on a
direct connection, 0 on this machine's own loopback where there is no round
trip to hide. The relay number is measured rather than derived -- 4 is what the
arithmetic gives on a good hour and it is the wrong answer on an ordinary bad
one, where 5 takes the stalls from 44 in 360 frames down to 5. On the shape of
connection this relay presents, going past 5 buys nothing; on a connection that
swings much more wildly it still can, which is what the knob is for.

### The number is set on the host, and only on the host

**Both machines must run the same depth, and different depths break the game.**
Not "run out of sync in time" -- actually break it: the two copies start
playing different matches, positions drift apart after a couple of seconds, and
**both machines still report a perfectly healthy connection the whole time**.
Nothing in either log says anything is wrong. It is the nastiest failure this
thing can produce, so the game no longer lets you cause it by hand:

- **The host decides.** The host's depth travels inside the reply that lets the
  other player in, and the joining machine adopts it and says so in its log:
  `the parent runs input delay 5 and this end had 2; ADOPTING 5`.
- **Setting it on the joining machine does nothing.** The knob is read, then
  overruled by the host, and the log line says it lost. Set it on the host.
- **Old copies of the game are refused, loudly.** A build from before this
  existed has no depth to send and would default to none, so the two would
  desync. They now will not pair at all, and the log says why:
  `REFUSING a peer speaking wire version 1; this build speaks 2`. If you see
  that, one of you is on an older build -- update and try again.

  That version number is the **game's** own, carried inside the packets this
  relay forwards without looking at them. It is not the HELLO version in the
  wire contract above, which is still 1 and unchanged. **Nothing about this
  needs the relay redeployed** -- an unmodified relay carries both generations
  perfectly well, which is precisely why the two games have to refuse each
  other themselves.

`SM64DS_COMMS_INPUT_DELAY=<0..8>` **on the host** overrides the default, and
there are two reasons to reach for it:

- **Your pair is further apart than the default covers.** The signal is
  `starved` on the line the game writes when the session ends, in
  `playlog/play_*.log`:

      [comms:loopback] closed after 600 rounds; indelay=5 starved=8 sent=...

  A handful over a long session is nothing -- a starved frame waits only for
  the part of the round trip that ran past the budget, not for a whole one. A
  number that climbs steadily means frames are routinely waiting on the wire.
  Raise **the host's** number by one and look again; the other machine follows
  on its own. The rule is `N >= round trip in ms / 33.3`, plus a frame of
  headroom, and more headroom on a jumpy connection than on a merely slow one --
  a steady 120 ms is covered by a depth, a connection that swings between 80 and
  400 ms wants more. `python3 test_client.py remote-check --host YOUR.SERVER.IP`
  measures the round trip and prints the N it implies.
- **You want the old behaviour back to measure it.** `=0` on the host is
  stop-and-wait exactly as it was, on both machines.

`port/tools/vs_pace.py` measures the whole thing end to end: solo, a pair
through a relay on this machine, and a pair through the live relay, with and
without the input delay, reporting the frame-time distribution for each.

## Rollback

    sudo systemctl disable --now sm64ds-relay

That stops it and stops it coming back on boot. To remove it completely:

    sudo rm /etc/systemd/system/sm64ds-relay.service
    sudo systemctl daemon-reload
    sudo rm -rf /home/sm64ds-relay/sm64ds-relay

## Configuration

Set in the unit file, visible with `systemctl cat sm64ds-relay`:

| Variable               | Default   | Meaning                            |
|------------------------|-----------|------------------------------------|
| `SM64DS_RELAY_PORT`    | 41234     | UDP port to listen on              |
| `SM64DS_RELAY_BIND`    | 0.0.0.0   | address to bind (`::` for IPv6)    |
| `SM64DS_RELAY_IDLE_S`  | 90        | seconds before an idle seat is dropped |
| `SM64DS_RELAY_STATS_S` | 60        | seconds between counter lines in the log |

After editing the unit: `sudo systemctl daemon-reload && sudo systemctl restart
sm64ds-relay`. Changing the port means changing the firewall rule to match, and
telling the game side.

## Reading the log

Everything goes to the journal. Session lifecycle only, never payloads, and
client addresses are cut down before they are written:

    session ABCD1234 create by 203.0.x.x#51820
    session ABCD1234 join 203.0.x.x#51820 as parent (1/4)
    session ABCD1234 join 198.51.x.x#40311 as child (2/4)
    session ABCD1234 leave 198.51.x.x#40311 (idle)
    session ABCD1234 expire

and once a minute a counter line:

    stats sessions=1 endpoints=2 rx=41233 fwd=41180 ack=53 drop_rate=0
    drop_size=0 drop_unpaired=0 drop_ignored=0 bad=0 full=0 expired=0
    rss_kb=14208

What the counters mean: `rx` datagrams received, `fwd` datagrams passed on,
`ack` HELLO answers sent, `drop_rate` dropped for exceeding the packet rate,
`drop_size` dropped for being over 700 bytes, `drop_unpaired` from an address
with no session, `drop_ignored` from an address inside its 10 second penalty,
`bad` malformed HELLOs answered, `full` refusals for a full session or a full
relay, `expired` seats dropped for going quiet, `rss_kb` the relay's own memory.

## Troubleshooting

**Service will not start.** `systemctl status sm64ds-relay` and
`journalctl -u sm64ds-relay -n 50`. The usual cause is the port already being
in use: `ss -ulnp | grep 41234` will name the other process.

**Players cannot connect but the service is up.** Almost always the firewall
or a cloud security group. Check from outside with
`python3 test_client.py probe --host YOUR.SERVER.IP`. No answer with a healthy
service means the packet is not reaching the box.

**One player connects, the other does not.** Look for two `join` lines with the
same code in the log. If only one appears, the other player never reached the
relay, or typed a different code. If you see a `refused` line, the session was
already full.

**Players connect then drop after a minute and a half.** Their client stopped
sending its keepalive HELLOs; the 90 second idle expiry then dropped the seat.
Look for `leave ... (idle)`.

**It stops relaying under fast play.** Check `drop_rate` in the counter line.
If it is climbing, something is sending more than the packet rate allows
(`rate_pps` on the startup line, 120 by default).

**Voice chat sessions need a raised packet rate.** A seat with proximity voice
chat on sends about 117 packets a second: up to 60 lockstep input records, 30
state snapshots, 2 RTT probes and 25 voice datagrams. That is three packets
under the stock 120 budget, which is not headroom, and what the relay does when
a seat goes over is drop -- silently, so it looks like a desync rather than a
limit. Set

    Environment=SM64DS_RELAY_RATE_PPS=240

in the unit file before hosting voice sessions and restart. The relay says so
itself on startup while the rate is still at the pre-voice number. Voice
datagrams are 340 bytes, well inside the 700 byte cap, so nothing else about
the relay's contract moves.

**A player who dropped out cannot get back in for a minute or so.** If their
router hands them a different outside port when they reconnect, the relay sees
a brand new player, and their old seat is still held by the address that went
quiet. They get status 1 until that seat times out, which takes up to 90
seconds. Waiting clears it; so does using a fresh session code. This is the
price of never letting a stranger take over a seat by quoting the code.

## Security posture

- Payloads are never inspected, logged, or interpreted.
- The relay speaks only to addresses that spoke to it first.
- It never sends an unknown address more bytes than it just received, so it
  cannot be used to amplify traffic at a third party.
- A malformed sender gets one short answer and is then ignored for 10 seconds.
- It runs as an unprivileged account with no login, no capabilities, a
  read-only view of the filesystem and a restricted system call set.
- No credentials, keys, or secrets exist anywhere in this kit. A session code
  is not a secret: it is a meeting point, and anyone who guesses one can join
  that session as a spare child. Codes should be random enough not to be
  guessed by accident.

## Tests

The suite runs entirely locally. It spawns relay instances on ports derived
from its own process id, so several runs can go at once without colliding.

    python3 test_client.py selftest
    python3 test_client.py soak --minutes 10

`selftest` covers pairing and a thousand datagrams each way with round trip
times and loss, star routing including that one child never sees another's
packets, a second child joining a live session, two sessions on one relay never
crossing, the 700 byte drop, the rate cap engaging and releasing, idle expiry
and recovery by re-HELLO, malformed HELLO handling with the bytes in and out
counted, re-HELLO idempotence, and the session and endpoint caps.

`soak` runs two paired endpoints trading packets and reports loss and the
relay's memory at the start and the end.
