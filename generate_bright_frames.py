#!/usr/bin/env python3
from PIL import Image, ImageDraw, ImageEnhance
import os

zombies_dir = "/c/PTSD-Plants-vs-Zombies/Resources/Image/zombies"

# Zombies that need bright versions
zombies_to_brighten = {
    "bucket": 8,
    "cone": 8,
    "polevaulter": 8,
    "flag": 8,
}

def brighten_image(image_path, brightness_factor=1.5):
    """Load image and make it brighter"""
    try:
        img = Image.open(image_path)
        enhancer = ImageEnhance.Brightness(img)
        bright_img = enhancer.enhance(brightness_factor)
        return bright_img
    except:
        return None

# Generate bright frames for each zombie type
for zombie_name, frame_count in zombies_to_brighten.items():
    # Create bright directory
    bright_dir = os.path.join(zombies_dir, f"{zombie_name}_bright")
    os.makedirs(bright_dir, exist_ok=True)

    zombie_dir = os.path.join(zombies_dir, zombie_name)

    for frame in range(1, frame_count + 1):
        source_path = os.path.join(zombie_dir, f"frame_{frame}.png")

        if not os.path.exists(source_path):
            print(f"[SKIP] {zombie_name}/frame_{frame}.png not found")
            continue

        bright_img = brighten_image(source_path)
        if bright_img:
            output_path = os.path.join(bright_dir, f"frame_{frame}.png")
            bright_img.save(output_path)
            print(f"[OK] {zombie_name}_bright/frame_{frame}.png")

print("\n[DONE] All bright frames generated!")
