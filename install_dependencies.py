#!/usr/bin/env python3
"""
Install/check dependencies for GitPromptApp.py.

GitPromptApp.py only uses Python standard-library modules. This script verifies
the required commands and can install missing system packages with a supported
Linux package manager.
"""

from __future__ import annotations

import importlib
import shutil
import subprocess
import sys


REQUIRED_COMMANDS = ("python3", "git")
STDLIB_MODULES = (
    "getpass",
    "os",
    "platform",
    "re",
    "shutil",
    "socket",
    "subprocess",
    "sys",
    "dataclasses",
    "datetime",
    "pathlib",
    "typing",
)

PACKAGE_MANAGERS = (
    ("apt-get", (("sudo", "apt-get", "update"), ("sudo", "apt-get", "install", "-y", "python3", "git"))),
    ("dnf", (("sudo", "dnf", "install", "-y", "python3", "git"),)),
    ("pacman", (("sudo", "pacman", "-Sy", "--needed", "python", "git"),)),
    ("zypper", (("sudo", "zypper", "install", "-y", "python3", "git"),)),
)


def missing_commands() -> list[str]:
    return [command for command in REQUIRED_COMMANDS if shutil.which(command) is None]


def install_system_packages() -> bool:
    for manager_name, commands in PACKAGE_MANAGERS:
        if shutil.which(manager_name) is None:
            continue

        for command in commands:
            subprocess.run(command, check=True)
        return True

    return False


def missing_stdlib_modules() -> list[str]:
    missing = []
    for module_name in STDLIB_MODULES:
        try:
            importlib.import_module(module_name)
        except ImportError:
            missing.append(module_name)
    return missing


def main() -> int:
    missing = missing_commands()
    if missing:
        print(f"Missing commands: {' '.join(missing)}")
        if not install_system_packages():
            print(f"Unsupported package manager. Install these manually: {' '.join(missing)}", file=sys.stderr)
            return 1

    missing_modules = missing_stdlib_modules()
    if missing_modules:
        print(f"Missing Python modules: {', '.join(missing_modules)}", file=sys.stderr)
        return 1

    print("Python standard-library imports are available.")
    print("Dependencies are ready.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
