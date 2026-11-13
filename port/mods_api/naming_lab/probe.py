"""Smallest client: send one command to the naming-lab plugin, print what comes back.
usage: python probe.py "hello" ["actors" ...]   (127.0.0.1:47621)"""
import socket, sys, time
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.settimeout(0.5)
for cmd in sys.argv[1:] or ['hello']:
    s.sendto(cmd.encode(), ('127.0.0.1', 47621))
    end = time.time() + 1.0
    while time.time() < end:
        try:
            d = s.recvfrom(65535)[0].decode(errors='replace')
        except socket.timeout:
            continue
        if d.startswith(('status', 'actors')) and cmd not in ('status', 'actors'):
            continue
        print(d[:3000])
        if not d.startswith('actors'):
            break
