#!/usr/bin/env python3
from PIL import Image, ImageEnhance
import os

script_dir = os.path.dirname(os.path.abspath(__file__))
zombies_dir = os.path.join(script_dir, "Resources", "Image", "zombies")

# Source and target directories for zombie bright frames
zombie_dirs = [
    ("zombie", "zombie_bright"),
    ("zombieAttack", "zombieAttack_bright"),
    ("bucketZombie", "bucketZombie_bright"),
    ("bucketZombieAttack", "bucketZombieAttack_bright"),
    ("coneZombie", "coneZombie_bright"),
    ("coneZombieAttack", "coneZombieAttack_bright"),
    ("flagZombie", "flagZombie_bright"),
    ("flagZombieAttack", "flagZombieAttack_bright"),
    ("polevaulter", "polevaulter_bright"),
]


def brighten_image(image_path, brightness_factor=1.5):
    """Load image and make it brighter."""
    try:
        img = Image.open(image_path)
        enhancer = ImageEnhance.Brightness(img)
        bright_img = enhancer.enhance(brightness_factor)
        return bright_img
    except Exception as exc:
        print(f"[ERROR] Could not brighten {image_path}: {exc}")
        return None


def sorted_png_files(directory):
    png_files = [f for f in os.listdir(directory) if f.lower().endswith('.png')]
    def frame_key(name):
        name_only = os.path.splitext(name)[0]
        try:
            return int(name_only.split('_')[-1])
        except ValueError:
            return name_only
    return sorted(png_files, key=frame_key)


# Generate bright frames for each zombie source directory
for src_name, bright_name in zombie_dirs:
    src_dir = os.path.join(zombies_dir, src_name)
    bright_dir = os.path.join(zombies_dir, bright_name)

    if not os.path.isdir(src_dir):
        print(f"[SKIP] Source directory not found: {src_dir}")
        continue

    os.makedirs(bright_dir, exist_ok=True)
    frame_files = sorted_png_files(src_dir)

    if not frame_files:
        print(f"[SKIP] No PNG frames found in {src_dir}")
        continue

    print(f"[START] Generating bright frames for {src_name} -> {bright_name} ({len(frame_files)} frames)")
    for filename in frame_files:
        source_path = os.path.join(src_dir, filename)
        bright_img = brighten_image(source_path)
        if not bright_img:
            continue
        output_path = os.path.join(bright_dir, filename)
        bright_img.save(output_path)
        print(f"[OK] {bright_name}/{filename}")

print("\n[DONE] All bright frames generated!")
