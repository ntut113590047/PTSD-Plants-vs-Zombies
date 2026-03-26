#!/usr/bin/env python3
from PIL import Image, ImageDraw
import os

# Create directories if they don't exist
plants_dir = "/c/PTSD-Plants-vs-Zombies/Resources/Image/Plants"
zombies_dir = "/c/PTSD-Plants-vs-Zombies/Resources/Image/zombies"

# New plants to generate
plants = {
    "cherry": (200, 50, 50),      # Red color for cherry bomb
    "snowpea": (100, 150, 255),   # Blue color for snow pea
    "chomper": (100, 200, 50),    # Green color for chomper
}

# New zombie to generate
zombies = {
    "flag": (150, 150, 150),      # Gray color for flag zombie
}

# Generate plant sprites (8 frames each)
for plant_name, color in plants.items():
    plant_dir = os.path.join(plants_dir, plant_name)
    os.makedirs(plant_dir, exist_ok=True)

    for frame in range(1, 9):
        img = Image.new('RGBA', (100, 100), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)

        # Draw a circle (plant body)
        offset = 5 + (frame % 4)
        draw.ellipse([10 + offset, 10 + offset, 90 - offset, 90 - offset],
                     fill=color, outline=(50, 50, 50), width=2)

        # Draw eyes
        draw.ellipse([30, 35, 40, 45], fill=(0, 0, 0))
        draw.ellipse([60, 35, 70, 45], fill=(0, 0, 0))

        img.save(os.path.join(plant_dir, f"frame_{frame}.png"))
        print(f"[OK] {plant_name}/frame_{frame}.png")

# Generate zombie sprites (8 frames each)
for zombie_name, color in zombies.items():
    zombie_dir = os.path.join(zombies_dir, zombie_name)
    os.makedirs(zombie_dir, exist_ok=True)

    for frame in range(1, 9):
        img = Image.new('RGBA', (100, 100), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)

        # Draw a rectangle (zombie body)
        offset = 5 + (frame % 3)
        draw.rectangle([10 + offset, 20 + offset, 90 - offset, 85 - offset],
                       fill=color, outline=(50, 50, 50), width=2)

        # Draw eyes
        draw.ellipse([25, 35, 35, 45], fill=(255, 0, 0))
        draw.ellipse([65, 35, 75, 45], fill=(255, 0, 0))

        # Draw mouth
        draw.line([(30, 60), (70, 60)], fill=(0, 0, 0), width=2)

        img.save(os.path.join(zombie_dir, f"frame_{frame}.png"))
        print(f"[OK] {zombie_name}/frame_{frame}.png")

print("\n[DONE] All placeholder sprites generated!")
