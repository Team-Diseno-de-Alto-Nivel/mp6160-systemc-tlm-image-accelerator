#!/usr/bin/env python3
"""Extract objective simulation results from the sim log and RAW image files:
final SystemC simulated time, byte counts, and a BT.601 conversion check on
sample pixels. No narrative — just the data, for CI to publish alongside the
images in the `simulation-results` GitHub Release.

Usage:
    python scripts/generate_results.py <sim_log_path> [--output results.md]
"""

import argparse
import re
from pathlib import Path

WIDTH = 1920
HEIGHT = 1080
INPUT_RAW = Path("images/input/image.raw")
OUTPUT_RAW = Path("images/output/output.raw")

TIME_RE = re.compile(r"(\d+(?:\.\d+)?)\s*(ns|us|ms|s)\b")


def parse_final_sim_time(log_text: str) -> str:
    times = TIME_RE.findall(log_text)
    if not times:
        return "unknown"
    value, unit = times[-1]
    return f"{value} {unit}"


def verify_pixels(rgb: bytes, gray: bytes, positions: list) -> list:
    rows = []
    for p in positions:
        r, g, b = rgb[p * 3], rgb[p * 3 + 1], rgb[p * 3 + 2]
        expected = round(0.299 * r + 0.587 * g + 0.114 * b)
        actual = gray[p]
        rows.append((p, (r, g, b), expected, actual, expected == actual))
    return rows


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("sim_log", type=Path)
    parser.add_argument("--output", type=Path, default=Path("results.md"))
    args = parser.parse_args()

    log_text = args.sim_log.read_text()
    rgb = INPUT_RAW.read_bytes()
    gray = OUTPUT_RAW.read_bytes()

    pixel_count = WIDTH * HEIGHT
    expected_input_bytes = pixel_count * 3
    expected_output_bytes = pixel_count

    sample_positions = [0, pixel_count // 4, pixel_count // 2, pixel_count - 1]
    pixel_rows = verify_pixels(rgb, gray, sample_positions)
    final_time = parse_final_sim_time(log_text)

    lines = [
        "# Simulation Results",
        "",
        f"- Final SystemC simulated time (`sc_time_stamp()`): **{final_time}**",
        f"- Input bytes transferred: **{len(rgb):,}** "
        f"(expected for {WIDTH}x{HEIGHT} RGB: {expected_input_bytes:,}, match: {len(rgb) == expected_input_bytes})",
        f"- Output bytes transferred: **{len(gray):,}** "
        f"(expected for {WIDTH}x{HEIGHT} grayscale: {expected_output_bytes:,}, match: {len(gray) == expected_output_bytes})",
        "",
        "## BT.601 conversion verification (sample pixels)",
        "",
        "| Pixel # | RGB | Expected gray (BT.601) | Actual gray | Match |",
        "|---|---|---|---|---|",
    ]
    for p, rgb_val, expected, actual, match in pixel_rows:
        lines.append(f"| {p} | {rgb_val} | {expected} | {actual} | {'✅' if match else '❌'} |")
    lines += [
        "",
        "## Full simulation log",
        "",
        "```",
        log_text.strip(),
        "```",
    ]

    args.output.write_text("\n".join(lines) + "\n")
    print(f"Wrote results to {args.output}")


if __name__ == "__main__":
    main()
