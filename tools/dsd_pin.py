"""Check the documented dsd build pin before generating linker inputs.

This records the installed executable's identity; it does not download, replace,
or certify an executable against a published release checksum.
"""
import hashlib
import pathlib
import subprocess


VERSION = "0.11.0"
VERSION_OUTPUT = f"ds-decomp-cli {VERSION}"
VERSION_TIMEOUT_SECONDS = 10
SETUP_URL = "https://github.com/tangosdev/sm64ds-decomp/blob/main/CONTRIBUTING.md#first-time-setup"


class DsdPinError(RuntimeError):
    """The installed dsd cannot be identified as the documented build version."""


def _refusal(path, actual):
    return DsdPinError(
        f"dsd preflight: expected {VERSION_OUTPUT} at {path}; actual {actual}. "
        f"Install the pinned dsd {VERSION} binary for your platform; "
        f"see CONTRIBUTING.md#first-time-setup ({SETUP_URL}).")


def check(path):
    """Return path/version/SHA-256 for the installed pin, or raise DsdPinError."""
    executable = pathlib.Path(path).absolute()
    try:
        before = executable.read_bytes()
    except OSError as error:
        raise _refusal(executable, f"executable missing or unreadable: {error}") from error
    try:
        result = subprocess.run(
            [str(executable), "--version"], capture_output=True, text=True,
            encoding="utf-8", errors="replace", timeout=VERSION_TIMEOUT_SECONDS,
            check=False)
    except subprocess.TimeoutExpired as error:
        raise _refusal(executable, f"version query timed out after {VERSION_TIMEOUT_SECONDS}s") from error
    except OSError as error:
        raise _refusal(executable, f"version query could not run: {error}") from error
    output = result.stdout.strip()
    if result.returncode:
        detail = (output + " " + result.stderr.strip()).strip()[:400]
        raise _refusal(executable, f"version query exited {result.returncode}: {detail!r}")
    if output != VERSION_OUTPUT:
        raise _refusal(executable, f"version output {output!r}")
    try:
        after = executable.read_bytes()
    except OSError as error:
        raise _refusal(executable, f"executable became unreadable after version query: {error}") from error
    if after != before:
        raise _refusal(executable, "executable changed during the version query")
    return {"path": str(executable), "version": VERSION,
            "sha256": hashlib.sha256(after).hexdigest()}
