"""
geoip_last.py — reads /var/log/wtmp, prints GeoIP city/country per login IP.

Usage (CLI):  python geoip_last.py [wtmp_file]
Usage (lib):  from geoip_last import iter_logins, geoip_lookup
"""

import ipaddress
import socket
import struct
import sys
from functools import lru_cache
from urllib.request import urlopen
from urllib.error import URLError
import json

# ── utmp struct (Linux/macOS) ─────────────────────────────────────────────────
# matches struct utmp in <utmp.h>
_FMT  = "hi32s32s32s256sHHiI4i20s"
_SIZE = struct.calcsize(_FMT)
USER_PROCESS = 7


def iter_logins(path: str = "/var/log/wtmp"):
    """Yield IP strings for every USER_PROCESS entry that has a remote host."""
    with open(path, "rb") as f:
        while chunk := f.read(_SIZE):
            if len(chunk) < _SIZE:
                break
            fields = struct.unpack(_FMT, chunk)
            ut_type = fields[0]
            ut_host = fields[5].rstrip(b"\x00").decode(errors="ignore")
            if ut_type != USER_PROCESS or not ut_host:
                continue
            ip = _resolve(ut_host)
            if ip and not _is_private(ip):
                yield ip


def _resolve(host: str) -> str | None:
    try:
        ipaddress.ip_address(host)
        return host
    except ValueError:
        pass
    try:
        return socket.gethostbyname(host)
    except socket.gaierror:
        return None


def _is_private(ip: str) -> bool:
    try:
        return ipaddress.ip_address(ip).is_private
    except ValueError:
        return True


@lru_cache(maxsize=512)
def geoip_lookup(ip: str) -> tuple[str, str]:
    """Return (city, country) for an IP via ip-api.com (no key required)."""
    try:
        url = f"http://ip-api.com/json/{ip}?fields=status,city,country"
        with urlopen(url, timeout=5) as r:
            data = json.loads(r.read())
        if data.get("status") == "success":
            return data.get("city", "?"), data.get("country", "?")
    except (URLError, json.JSONDecodeError):
        pass
    return "?", "?"


# ── CLI ───────────────────────────────────────────────────────────────────────
def main():
    path = sys.argv[1] if len(sys.argv) > 1 else "/var/log/wtmp"
    try:
        ips = list(iter_logins(path))
    except FileNotFoundError:
        print(f"Error: {path} not found", file=sys.stderr)
        sys.exit(1)

    if not ips:
        print("No remote logins found.")
        return

    print(f"{'IP':<45} {'City':<22} Country")
    print("-" * 75)
    for ip in ips:
        city, country = geoip_lookup(ip)
        print(f"{ip:<45} {city:<22} {country}")


if __name__ == "__main__":
    main()
