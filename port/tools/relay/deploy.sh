#!/usr/bin/env bash
#
# Install (or update) the SM64DS port UDP relay on a Linux box with systemd.
#
# Run this BY HAND, on the server, as root:
#
#     scp -r relay/ youruser@your.server:/tmp/sm64ds-relay-kit
#     ssh youruser@your.server
#     sudo bash /tmp/sm64ds-relay-kit/deploy.sh
#
# It is safe to run again any time: it only changes what actually differs, and
# rerunning it is the normal way to push a new relay.py.
#
# It does NOT touch the firewall. Opening a port is the operator's call, so the
# exact commands are printed at the end for you to run yourself.
#
# Options:
#   --port N     UDP port to listen on              (default 41234)
#   --user NAME  unprivileged account to run as     (default sm64ds-relay)
#   --idle N     seconds before an idle player is dropped (default 90)
#   --dry-run    print every step, change nothing
#

set -euo pipefail

SVC_NAME="sm64ds-relay"
SVC_USER="sm64ds-relay"
PORT="41234"
IDLE="90"
DRY_RUN="no"

SRC_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

while [ $# -gt 0 ]; do
    case "$1" in
        --port) PORT="$2"; shift 2 ;;
        --user) SVC_USER="$2"; shift 2 ;;
        --idle) IDLE="$2"; shift 2 ;;
        --dry-run) DRY_RUN="yes"; shift ;;
        -h|--help)
            # The header comment is the help text. Stop at the first line
            # that is not a comment, so no code leaks into the output.
            awk 'NR > 1 && /^#/ { sub(/^# ?/, ""); print; next }
                 NR > 1 { exit }' "${BASH_SOURCE[0]}"
            exit 0 ;;
        *) echo "unknown option: $1" >&2; exit 2 ;;
    esac
done

HOME_DIR="/home/${SVC_USER}"
APP_DIR="${HOME_DIR}/${SVC_NAME}"
UNIT_PATH="/etc/systemd/system/${SVC_NAME}.service"

step() { printf '\n==> %s\n' "$*"; }
info() { printf '    %s\n' "$*"; }
run() {
    # Echo the command quoted the way a human could paste it back, then run
    # it. "$*" would drop the quoting and print a multi word argument as if
    # it were several.
    local shown=""
    local arg
    for arg in "$@"; do
        shown="${shown} $(printf '%q' "$arg")"
    done
    printf '    $%s\n' "$shown"
    if [ "$DRY_RUN" = "yes" ]; then
        return 0
    fi
    "$@"
}
die() { printf '\nFAILED: %s\n' "$*" >&2; exit 1; }

printf '%s\n' "SM64DS relay deploy"
info "source kit : ${SRC_DIR}"
info "service    : ${SVC_NAME}"
info "user       : ${SVC_USER}"
info "install to : ${APP_DIR}"
info "udp port   : ${PORT}"
info "idle expiry: ${IDLE}s"
[ "$DRY_RUN" = "yes" ] && info "MODE       : dry run, nothing will change"

# ---------------------------------------------------------------- preflight

step "Checking prerequisites"
if [ "$(id -u)" -ne 0 ] && [ "$DRY_RUN" != "yes" ]; then
    die "run this as root (sudo bash $0)"
fi
info "running as uid $(id -u)"

for f in relay.py sm64ds-relay.service; do
    [ -f "${SRC_DIR}/${f}" ] || die "missing ${f} next to this script"
    info "found ${SRC_DIR}/${f}"
done

command -v systemctl >/dev/null 2>&1 || die "systemd not found on this host"
info "systemd present: $(command -v systemctl)"

PY_BIN="$(command -v python3 || true)"
[ -n "$PY_BIN" ] || die "python3 not found; install python3 and rerun"
PY_VER="$("$PY_BIN" -c 'import sys; print("%d.%d" % sys.version_info[:2])')"
info "python3: ${PY_BIN} (${PY_VER})"
"$PY_BIN" -c 'import sys; raise SystemExit(0 if sys.version_info >= (3, 7) else 1)' \
    || die "python 3.7 or newer required (found ${PY_VER})"

# Parse it rather than py_compile it, so nothing is written next to the kit
# and this still works from a read-only copy.
"$PY_BIN" -c 'import ast,sys; ast.parse(open(sys.argv[1]).read())' \
    "${SRC_DIR}/relay.py" \
    || die "relay.py does not parse with this python"
info "relay.py parses clean"

for f in relay.py sm64ds-relay.service; do
    if grep -q $'\r' "${SRC_DIR}/${f}"; then
        die "${f} has CRLF line endings; re-copy the kit with LF endings"
    fi
done
info "line endings look right"

# ------------------------------------------------------------- service user

step "Ensuring the unprivileged service account exists"
if id -u "$SVC_USER" >/dev/null 2>&1; then
    info "user ${SVC_USER} already exists (uid $(id -u "$SVC_USER"))"
else
    NOLOGIN="/usr/sbin/nologin"
    [ -x "$NOLOGIN" ] || NOLOGIN="/sbin/nologin"
    [ -x "$NOLOGIN" ] || NOLOGIN="/bin/false"
    info "user ${SVC_USER} does not exist, creating a system account with no login"
    info "login shell will be ${NOLOGIN}"
    run useradd --system --create-home --home-dir "$HOME_DIR" \
        --shell "$NOLOGIN" --comment "SM64DS port UDP relay" "$SVC_USER"
fi

step "Ensuring the install directory exists"
run install -d -o "$SVC_USER" -g "$SVC_USER" -m 0755 "$HOME_DIR"
run install -d -o "$SVC_USER" -g "$SVC_USER" -m 0755 "$APP_DIR"
info "install directory: ${APP_DIR}"

# ------------------------------------------------------------------ payload

step "Installing relay.py"
if [ -f "${APP_DIR}/relay.py" ] \
   && cmp -s "${SRC_DIR}/relay.py" "${APP_DIR}/relay.py"; then
    info "relay.py is already identical, nothing to copy"
    RELAY_CHANGED="no"
else
    info "relay.py differs or is new, copying"
    run install -o "$SVC_USER" -g "$SVC_USER" -m 0644 \
        "${SRC_DIR}/relay.py" "${APP_DIR}/relay.py"
    RELAY_CHANGED="yes"
fi

for extra in README.md test_client.py; do
    if [ -f "${SRC_DIR}/${extra}" ]; then
        run install -o "$SVC_USER" -g "$SVC_USER" -m 0644 \
            "${SRC_DIR}/${extra}" "${APP_DIR}/${extra}"
    fi
done

# --------------------------------------------------------------- unit file

step "Installing the systemd unit"
TMP_UNIT="$(mktemp)"
sed -e "s|^User=.*|User=${SVC_USER}|" \
    -e "s|^Group=.*|Group=${SVC_USER}|" \
    -e "s|^WorkingDirectory=.*|WorkingDirectory=${APP_DIR}|" \
    -e "s|^ExecStart=.*|ExecStart=${PY_BIN} -u ${APP_DIR}/relay.py|" \
    -e "s|^Documentation=.*|Documentation=file://${APP_DIR}/README.md|" \
    -e "s|^Environment=SM64DS_RELAY_PORT=.*|Environment=SM64DS_RELAY_PORT=${PORT}|" \
    -e "s|^Environment=SM64DS_RELAY_IDLE_S=.*|Environment=SM64DS_RELAY_IDLE_S=${IDLE}|" \
    "${SRC_DIR}/sm64ds-relay.service" > "$TMP_UNIT"
info "unit rendered for user=${SVC_USER} port=${PORT} idle=${IDLE}s"

if [ -f "$UNIT_PATH" ] && cmp -s "$TMP_UNIT" "$UNIT_PATH"; then
    info "unit at ${UNIT_PATH} is already identical"
    UNIT_CHANGED="no"
else
    info "writing ${UNIT_PATH}"
    run install -o root -g root -m 0644 "$TMP_UNIT" "$UNIT_PATH"
    UNIT_CHANGED="yes"
fi
rm -f "$TMP_UNIT"

# ------------------------------------------------------------------ enable

step "Reloading systemd and starting the service"
run systemctl daemon-reload
run systemctl enable "$SVC_NAME"
if [ "$DRY_RUN" = "yes" ]; then
    info "dry run: would restart ${SVC_NAME}"
else
    info "relay.py changed: ${RELAY_CHANGED}, unit changed: ${UNIT_CHANGED}"
    run systemctl restart "$SVC_NAME"
fi

# ------------------------------------------------------------------ verify

step "Verifying"
if [ "$DRY_RUN" = "yes" ]; then
    info "dry run: skipping verification"
else
    sleep 1
    if systemctl is-active --quiet "$SVC_NAME"; then
        info "service is active"
    else
        printf '\n'
        systemctl status "$SVC_NAME" --no-pager -l || true
        journalctl -u "$SVC_NAME" -n 30 --no-pager || true
        printf '\n'
        printf '    Two things to check before anything else:\n'
        printf '      1. is UDP %s already in use?   ss -ulnp | grep %s\n' \
            "$PORT" "$PORT"
        printf '      2. if the log shows a permission or memory error at\n'
        printf '         startup, this python may not like one of the\n'
        printf '         hardening lines in %s. Comment out\n' "$UNIT_PATH"
        printf '         MemoryDenyWriteExecute and SystemCallFilter, then\n'
        printf '         systemctl daemon-reload && systemctl restart %s\n' \
            "$SVC_NAME"
        die "service did not come up; the status and log above say why"
    fi

    printf '\n'
    run ss -ulnp "sport = :${PORT}" || true
    printf '\n'
    run journalctl -u "$SVC_NAME" -n 20 --no-pager || true

    if [ -f "${APP_DIR}/test_client.py" ]; then
        step "Sending one HELLO from this box to itself"
        if "$PY_BIN" "${APP_DIR}/test_client.py" probe \
                --host 127.0.0.1 --port "$PORT" --code DEPLOY01; then
            info "the relay answered; it is live and speaking the protocol"
        else
            die "no answer on 127.0.0.1:${PORT}; check journalctl above"
        fi
    fi
fi

# -------------------------------------------------------------- next steps

if [ "$DRY_RUN" = "yes" ]; then
    DONE_LINE="Dry run finished. Nothing was changed."
    STATE_LINE="It WOULD be installed at ${APP_DIR}, running as ${SVC_USER}."
else
    DONE_LINE="Done."
    STATE_LINE="The relay is installed at ${APP_DIR} and runs as ${SVC_USER}."
fi

cat <<EOF

==> ${DONE_LINE}

${STATE_LINE}
It listens on UDP ${PORT}. Nothing else on this box is touched.

FIREWALL: only UDP ${PORT} needs to be reachable from the internet. This
script does not change firewall rules; run whichever of these applies.

  ufw:
      sudo ufw allow ${PORT}/udp comment 'SM64DS relay'
      sudo ufw status verbose

  raw iptables (and ip6tables for IPv6):
      sudo iptables -A INPUT -p udp --dport ${PORT} -j ACCEPT
      sudo ip6tables -A INPUT -p udp --dport ${PORT} -j ACCEPT
      sudo iptables -L INPUT -n --line-numbers | grep ${PORT}

  If the box also has a cloud provider firewall or security group, allow
  UDP ${PORT} inbound there as well, or nothing will reach this host.

VERIFY, any time:

      ss -ulnp | grep ${PORT}
      journalctl -u ${SVC_NAME} -n 20
      systemctl status ${SVC_NAME}

END TO END, from a machine that has the kit (fill in the public address):

      python3 test_client.py probe --host YOUR.SERVER.IP --port ${PORT}
      python3 test_client.py remote-check --host YOUR.SERVER.IP --port ${PORT}

WATCH IT WORK while two players connect:

      journalctl -u ${SVC_NAME} -f

ROLLBACK:

      sudo systemctl disable --now ${SVC_NAME}

  and to remove it completely:

      sudo rm ${UNIT_PATH} && sudo systemctl daemon-reload
      sudo rm -rf ${APP_DIR}

EOF
