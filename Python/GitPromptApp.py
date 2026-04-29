#!/usr/bin/env python3
"""
Git prompt tools.

This is a Python rewrite of the original C++ prompt generator. It prints a
Bash-safe PS1 string, including wrapped ANSI escape sequences and a wrapped
terminal-title escape sequence.
"""

from __future__ import annotations

import getpass
import os
import platform
import re
import shutil
import socket
import subprocess
import sys
from dataclasses import dataclass, field
from datetime import datetime
from pathlib import Path
from typing import Iterable


VOLUME_USED_WARN_PCT = 90
LEFT_DIRS_NUM = 2
RIGHT_DIRS_NUM = 2


@dataclass(frozen=True)
class Config:
    is_date: bool = False
    is_time: bool = True
    is_host_name: bool = False
    my_host_names: tuple[str, ...] = ("skynowa-pc", "skynowa-laptop")
    is_user_name: bool = False
    my_user_names: tuple[str, ...] = ("skynowa",)
    is_dir_path_in_git_repo: bool = False
    is_volume_used_pct: bool = False
    is_power_supply: bool = True


@dataclass(frozen=True)
class Color:
    fg: str
    bold: bool = True

    def set_text(self, text: object) -> str:
        codes: list[str] = []
        if self.bold:
            codes.append("1")
        if self.fg:
            codes.append(self.fg)
        return f"\033[{';'.join(codes)}m{text}\033[0m"


CL_GREEN_BOLD = Color("32")
CL_YELLOW_BOLD = Color("33")
CL_BLUE_BOLD = Color("34")
CL_MAGENTA_BOLD = Color("35")
CL_RED_BOLD = Color("31")
CL_WHITE_BOLD = Color("37")
CL_CYAN_BOLD = Color("36")
CL_DEFAULT_BOLD = Color("39")


@dataclass
class RemoteInfo:
    host_name: str = ""
    group_name: str = ""
    repo_name: str = ""


@dataclass
class GitInfo:
    root: Path | None = None
    branch_name: str = ""
    remotes: list[str] = field(default_factory=list)
    remote_info: RemoteInfo = field(default_factory=RemoteInfo)
    local_branches_num: int = 0
    files_statuses: str = ""
    modified_files_num: int = 0
    ahead_num: int = 0
    behind_num: int = 0
    stashes_num: int = 0

    @property
    def is_git_dir(self) -> bool:
        return self.root is not None

    @property
    def repo_name(self) -> str:
        if self.remote_info.repo_name:
            return self.remote_info.repo_name
        return self.root.name if self.root else ""


def run_text(args: list[str], cwd: Path | None = None) -> str:
    try:
        result = subprocess.run(
            args,
            cwd=cwd,
            check=False,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True,
        )
    except OSError:
        return ""

    if result.returncode != 0:
        return ""

    return result.stdout.strip()


def local_branches_num(root: Path) -> int:
    branch_names = []
    for line in run_text(["git", "branch", "--format=%(refname:short)"], root).splitlines():
        branch_name = line.strip()
        if branch_name and branch_name not in {"main", "master"}:
            branch_names.append(branch_name)

    return len(branch_names)


def current_git_info() -> GitInfo:
    root_text = run_text(["git", "rev-parse", "--show-toplevel"])
    if not root_text:
        return GitInfo()

    root = Path(root_text)
    branch_name = (
        run_text(["git", "branch", "--show-current"], root)
        or run_text(["git", "rev-parse", "--short", "HEAD"], root)
        or "detached"
    )
    remotes = [line for line in run_text(["git", "remote"], root).splitlines() if line]
    remote_url = ""
    if remotes:
        remote_url = run_text(["git", "remote", "get-url", remotes[0]], root)

    status_lines = run_text(["git", "status", "--porcelain"], root).splitlines()
    ahead_num, behind_num = commits_ahead_behind(root)

    return GitInfo(
        root=root,
        branch_name=branch_name,
        remotes=remotes,
        remote_info=parse_remote_url(remote_url),
        local_branches_num=local_branches_num(root),
        files_statuses=format_file_statuses(status_lines),
        modified_files_num=len(status_lines),
        ahead_num=ahead_num,
        behind_num=behind_num,
        stashes_num=len(run_text(["git", "stash", "list"], root).splitlines()),
    )


def parse_remote_url(url: str) -> RemoteInfo:
    if not url:
        return RemoteInfo()

    cleaned = url.strip()
    match = re.match(r"^(?:https?|ssh)://(?:[^@/]+@)?(\[[^\]]+\]|[^/:]+)(?::\d+)?/(.+)$", cleaned)
    if match:
        host, path = match.groups()
    else:
        match = re.match(r"^git@([^:/]+):(.+)$", cleaned)
        if not match:
            return RemoteInfo()
        host, path = match.groups()

    path = path.removesuffix(".git").strip("/")
    parts = [part for part in path.split("/") if part]
    repo_name = parts[-1] if parts else ""
    group_name = "/".join(parts[:-1])
    host_name = host.removeprefix("[").removesuffix("]").split(":", 1)[0].split(".", 1)[0]

    return RemoteInfo(host_name=host_name, group_name=group_name, repo_name=repo_name)


def commits_ahead_behind(root: Path) -> tuple[int, int]:
    upstream = run_text(["git", "rev-parse", "--abbrev-ref", "--symbolic-full-name", "@{u}"], root)
    if not upstream:
        return 0, 0

    counts = run_text(["git", "rev-list", "--left-right", "--count", f"HEAD...{upstream}"], root)
    try:
        ahead, behind = counts.split()
        return int(ahead), int(behind)
    except ValueError:
        return 0, 0


def format_file_statuses(status_lines: Iterable[str]) -> str:
    staged = changed = untracked = conflicted = 0
    for line in status_lines:
        code = line[:2]
        if code == "??":
            untracked += 1
        elif "U" in code or code in {"AA", "DD"}:
            conflicted += 1
        else:
            if code[0] != " ":
                staged += 1
            if code[1] != " ":
                changed += 1

    rv = ""
    if staged:
        rv += "+"
    if changed:
        rv += "!"
    if untracked:
        rv += "?"
    if conflicted:
        rv += "x"
    return rv


def current_dir_brief() -> str:
    current = Path.cwd()
    home = Path.home()

    try:
        display = Path("~") / current.relative_to(home)
    except ValueError:
        display = current

    return brief_path(str(display))


def brief_path(path: str) -> str:
    if path in {"", "/", "~"}:
        return path

    prefix = ""
    rest = path
    if path.startswith("~/"):
        prefix = "~/"
        rest = path[2:]
    elif path.startswith("/"):
        prefix = "/"
        rest = path[1:]

    parts = [part for part in rest.split("/") if part]
    max_parts = LEFT_DIRS_NUM + RIGHT_DIRS_NUM
    if len(parts) <= max_parts:
        return prefix + "/".join(parts)

    left = parts[:LEFT_DIRS_NUM]
    right = parts[-RIGHT_DIRS_NUM:]
    return prefix + "/".join([*left, "...", *right])


def volume_used_pct() -> int:
    usage = shutil.disk_usage(Path.cwd())
    if usage.total == 0:
        return 0
    return (usage.total - usage.free) * 100 // usage.total


def is_admin() -> bool:
    return hasattr(os, "geteuid") and os.geteuid() == 0


def battery_status(config: Config) -> str:
    if not config.is_power_supply:
        return ""

    power_root = Path("/sys/class/power_supply")
    batteries = sorted(power_root.glob("BAT*"))
    if not batteries:
        return ""

    battery = batteries[0]
    capacity = read_first_line(battery / "capacity")
    status = read_first_line(battery / "status").lower()
    if not capacity:
        return ""

    icon = "?"
    if status == "discharging":
        icon = "↓"
    elif status == "charging":
        icon = "↑"
    elif status == "full":
        icon = "🔋"

    return f", Power: {capacity}%{icon}"


def read_first_line(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8").splitlines()[0].strip()
    except (OSError, IndexError, UnicodeDecodeError):
        return ""


def distro_name() -> str:
    os_release = Path("/etc/os-release")
    try:
        values = {}
        for line in os_release.read_text(encoding="utf-8").splitlines():
            if "=" not in line:
                continue
            key, value = line.split("=", 1)
            values[key] = value.strip().strip('"')
        return values.get("PRETTY_NAME") or values.get("NAME") or platform.system()
    except OSError:
        return platform.system()


def desktop_name() -> str:
    return os.environ.get("XDG_CURRENT_DESKTOP") or os.environ.get("DESKTOP_SESSION") or "unknown"


def is_vpn_active() -> bool:
    net_root = Path("/sys/class/net")
    try:
        interfaces = list(net_root.iterdir())
    except OSError:
        return False

    return any(is_vpn_interface(path) for path in interfaces)


def is_vpn_interface(path: Path) -> bool:
    if not is_network_interface_up(path):
        return False

    name = path.name.lower()
    vpn_prefixes = ("tun", "tap", "wg", "ppp", "vpn", "zt", "tailscale", "nordlynx", "proton", "mullvad")
    vpn_markers = ("warp",)
    if name.startswith(vpn_prefixes) or any(marker in name for marker in vpn_markers):
        return True

    return read_first_line(path / "type") == "65534"


def is_network_interface_up(path: Path) -> bool:
    flags = read_first_line(path / "flags")
    try:
        return bool(int(flags, 16) & 0x1)
    except ValueError:
        return False


def wrap_ansi_for_ps1(ps1: str) -> str:
    ansi_regex = re.compile(r"\033\[[0-9;?]*[A-Za-z]")
    return ansi_regex.sub(lambda match: rf"\[{match.group(0)}\]", ps1)


def wrap_title_for_ps1(title: str) -> str:
    return f"\\[\033]0;{title}\a\\]"


def build_prompt(shell_last_exit_code: int, config: Config) -> str:
    git = current_git_info()
    host_name = socket.gethostname()
    login_name = getpass.getuser()
    admin = is_admin()
    current_dir_path_brief = current_dir_brief()
    used_pct = volume_used_pct()
    is_mc = "MC_SID" in os.environ

    ps1 = ""

    ps1 += CL_WHITE_BOLD.set_text("[")
    now = datetime.now()
    if config.is_date:
        ps1 += CL_MAGENTA_BOLD.set_text(now.strftime("%d-%b"))
    if config.is_time:
        space = " " if config.is_date else ""
        ps1 += CL_MAGENTA_BOLD.set_text(space + now.strftime("%H:%M"))
    ps1 += CL_WHITE_BOLD.set_text("]")

    ps1 += CL_GREEN_BOLD.set_text("✔") if shell_last_exit_code == 0 else CL_RED_BOLD.set_text("✖")

    if config.is_user_name or login_name not in config.my_user_names:
        ps1 += (CL_RED_BOLD if admin else CL_MAGENTA_BOLD).set_text(login_name)

    if config.is_host_name or host_name not in config.my_host_names:
        ps1 += "@"
        ps1 += CL_CYAN_BOLD.set_text(host_name)

    ps1 += " "

    if git.is_git_dir:
        ps1 += CL_WHITE_BOLD.set_text("[")
        ps1 += CL_BLUE_BOLD.set_text(f"{git.remote_info.host_name}*{len(git.remotes)}")
        ps1 += CL_DEFAULT_BOLD.set_text("/")

        if git.remote_info.group_name:
            ps1 += CL_CYAN_BOLD.set_text(git.remote_info.group_name)
            ps1 += CL_DEFAULT_BOLD.set_text("/")

        ps1 += CL_YELLOW_BOLD.set_text(git.repo_name)
        ps1 += CL_WHITE_BOLD.set_text("]")

        if git.repo_name:
            ps1 += " "

    if git.is_git_dir:
        if config.is_dir_path_in_git_repo:
            ps1 += CL_GREEN_BOLD.set_text(current_dir_path_brief)
    else:
        ps1 += CL_GREEN_BOLD.set_text(current_dir_path_brief)

    if config.is_volume_used_pct or used_pct > VOLUME_USED_WARN_PCT:
        if ps1 and ps1[-1] != " ":
            ps1 += " "
        ps1 += CL_WHITE_BOLD.set_text(f"{used_pct}%")
        ps1 += " "

    if git.is_git_dir:
        ps1 += CL_GREEN_BOLD.set_text("(")
        ps1 += CL_RED_BOLD.set_text(git.branch_name)
        ps1 += CL_GREEN_BOLD.set_text(")")

        if git.local_branches_num > 0:
            ps1 += CL_WHITE_BOLD.set_text(f"⎇{git.local_branches_num}")

        ps1 += CL_YELLOW_BOLD.set_text(git.files_statuses)

        if git.modified_files_num > 0:
            ps1 += CL_YELLOW_BOLD.set_text(str(git.modified_files_num))

        ahead_behind = ""
        if git.ahead_num:
            ahead_behind += f"↑{git.ahead_num}"
        if git.behind_num:
            ahead_behind += f"↓{git.behind_num}"
        ps1 += CL_MAGENTA_BOLD.set_text(ahead_behind)

        if git.stashes_num > 0:
            ps1 += CL_BLUE_BOLD.set_text(f"⚑{git.stashes_num}")

    ps1 += " "
    ps1 += (CL_RED_BOLD if admin else CL_DEFAULT_BOLD).set_text("#" if admin else "$")
    ps1 += " "

    if is_mc:
        ps1 += CL_CYAN_BOLD.set_text("mc")
        ps1 += " "

    ps1 += CL_YELLOW_BOLD.set_text("❱ ")

    title = (
        f"{host_name}@{login_name} - {distro_name()}, {desktop_name()}, "
        f"CPUs: {os.cpu_count() or 0}, VPN: {'on' if is_vpn_active() else 'off'}"
        f"{battery_status(config)}                Build: Python {platform.python_version()}"
    )

    return wrap_title_for_ps1(title) + wrap_ansi_for_ps1(ps1)


def parse_shell_last_exit_code(argv: list[str]) -> int:
    if not argv:
        return 0
    try:
        return int(argv[0])
    except ValueError:
        return 0


def main(argv: list[str]) -> int:
    try:
        print(build_prompt(parse_shell_last_exit_code(argv), Config()))
        return 0
    except Exception as exc:
        print(f"GitPrompt error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
