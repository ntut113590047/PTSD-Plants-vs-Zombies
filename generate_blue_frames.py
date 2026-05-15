#!/usr/bin/env python3
from PIL import Image
import os

script_dir = os.path.dirname(os.path.abspath(__file__))
zombies_dir = os.path.join(script_dir, "Resources", "Image", "zombies")

zombie_dirs = [
    ("zombie", "zombie_blue"),
    ("zombieAttack", "zombieAttack_blue"),
    ("bucketZombie", "bucketZombie_blue"),
    ("bucketZombieAttack", "bucketZombieAttack_blue"),
    ("coneZombie", "coneZombie_blue"),
    ("coneZombieAttack", "coneZombieAttack_blue"),
    ("flagZombie", "flagZombie_blue"),
    ("flagZombieAttack", "flagZombieAttack_blue"),
    ("polevaulter", "polevaulter_blue"),
]


def sorted_png_files(directory):
    png_files = [f for f in os.listdir(directory) if f.lower().endswith(".png")]

    def frame_key(name):
        name_only = os.path.splitext(name)[0]
        try:
            return int(name_only.split("_")[-1])
        except ValueError:
            return name_only

    return sorted(png_files, key=frame_key)


def tint_blue(image_path, blue_boost=1.45, red_cut=0.65, green_cut=0.80):
    try:
        img = Image.open(image_path).convert("RGBA")
        pixels = img.load()
        width, height = img.size

        for y in range(height):
            for x in range(width):
                r, g, b, a = pixels[x, y]
                if a == 0:
                    continue
                nr = int(max(0, min(255, r * red_cut)))
                ng = int(max(0, min(255, g * green_cut)))
                nb = int(max(0, min(255, b * blue_boost)))
                pixels[x, y] = (nr, ng, nb, a)

        return img
    except Exception as exc:
        print(f"[ERROR] Could not tint {image_path}: {exc}")
        return None


for src_name, blue_name in zombie_dirs:
    src_dir = os.path.join(zombies_dir, src_name)
    blue_dir = os.path.join(zombies_dir, blue_name)

    if not os.path.isdir(src_dir):
        print(f"[SKIP] Source directory not found: {src_dir}")
        continue

    os.makedirs(blue_dir, exist_ok=True)
    frame_files = sorted_png_files(src_dir)

    if not frame_files:
        print(f"[SKIP] No PNG frames found in {src_dir}")
        continue

    print(f"[START] Generating blue frames for {src_name} -> {blue_name} ({len(frame_files)} frames)")
    for filename in frame_files:
        source_path = os.path.join(src_dir, filename)
        blue_img = tint_blue(source_path)
        if not blue_img:
            continue

        output_path = os.path.join(blue_dir, filename)
        blue_img.save(output_path)
        print(f"[OK] {blue_name}/{filename}")

print("\n[DONE] All blue frames generated!")
