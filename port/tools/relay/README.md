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
`ack` HELLO answers sent, `drop_rate` dropped for exceeding 120/s,
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
If it is climbing, something is sending more than 120 packets a second.

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
