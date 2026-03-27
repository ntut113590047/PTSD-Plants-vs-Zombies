#!/usr/bin/env python3
"""
Sprite Generator for Plants vs Zombies
Generates simple geometric placeholder sprites for plants and zombies
"""

try:
    from PIL import Image, ImageDraw
except ImportError:
    print("ERROR: PIL/Pillow is required. Install with: pip install pillow")
    exit(1)

import os

FRAME_SIZE = 100
FRAMES_PER_ANIMATION = 8
OUTPUT_DPI = (100, 100)

def ensure_dir(path):
    """Create directory if it doesn't exist"""
    os.makedirs(path, exist_ok=True)

def create_frames(base_name, color, modifier_fn=None, num_frames=8):
    """Create 8 frames of animation with optional modifier"""
    frames = []
    for i in range(num_frames):
        # Create a new image with white background
        img = Image.new('RGBA', (FRAME_SIZE, FRAME_SIZE), (255, 255, 255, 255))
        draw = ImageDraw.Draw(img)

        # Draw base shape
        if modifier_fn:
            modifier_fn(draw, color, i, num_frames)
        else:
            draw_basic_shape(draw, color)

        frames.append(img)

    return frames

def save_frames(frames, output_path, base_name):
    """Save all frames to directory"""
    for i, frame in enumerate(frames, 1):
        frame.save(f"{output_path}/frame_{i}.png")

def draw_basic_shape(draw, color):
    """Draw a basic shape (circle or rectangle)"""
    draw.ellipse([10, 10, 90, 90], fill=color, outline='black', width=2)

# ===== PLANT SPRITES =====

def generate_walnut_frames():
    """Generate Walnut sprite (defensive plant)"""
    frames = []
    for i in range(8):
        img = Image.new('RGBA', (FRAME_SIZE, FRAME_SIZE), (255, 255, 255, 255))
        draw = ImageDraw.Draw(img)

        # Green circular body
        color = (100, 200, 100) if i < 4 else (80, 180, 80)
        draw.ellipse([15, 15, 85, 85], fill=color, outline='black', width=2)

        # Grid pattern on walnut
        for x in range(25, 85, 15):
            draw.line([(x, 15), (x, 85)], fill='black', width=1)
        for y in range(25, 85, 15):
            draw.line([(15, y), (85, y)], fill='black', width=1)

        frames.append(img)

    return frames

def generate_potato_frames():
    """Generate Potato Mine sprite"""
    frames = []
    for i in range(8):
        img = Image.new('RGBA', (FRAME_SIZE, FRAME_SIZE), (255, 255, 255, 255))
        draw = ImageDraw.Draw(img)

        # Brown potato body
        color = (139, 101, 50)
        draw.ellipse([20, 25, 80, 80], fill=color, outline='black', width=2)

        # Fuse on top (grows/flickers)
        fuse_x = 50
        fuse_top = 20 if i < 4 else 15
        draw.line([(fuse_x, 25), (fuse_x, fuse_top)], fill='red', width=3)

        # Small spark
        spark_offset = 5 if (i % 2 == 0) else 0
        draw.ellipse([fuse_x - 2 + spark_offset, fuse_top - 2, fuse_x + 2 + spark_offset, fuse_top + 2],
                    fill='yellow', outline='orange', width=1)

        frames.append(img)

    return frames

def generate_repeater_frames():
    """Generate Repeater sprite (double peashooter)"""
    frames = []
    for i in range(8):
        img = Image.new('RGBA', (FRAME_SIZE, FRAME_SIZE), (255, 255, 255, 255))
        draw = ImageDraw.Draw(img)

        # Main body (green)
        draw.ellipse([20, 40, 80, 80], fill=(100, 200, 100), outline='black', width=2)

        # Left gun
        draw.polygon([(30, 50), (40, 40), (45, 50), (40, 55)], fill=(139, 101, 50), outline='black')
        # Right gun
        draw.polygon([(60, 50), (70, 40), (75, 50), (70, 55)], fill=(139, 101, 50), outline='black')

        # Firing animation (small circles at barrel)
        if i < 4:
            draw.ellipse([38, 30, 42, 34], fill='yellow', outline='orange')
            draw.ellipse([68, 30, 72, 34], fill='yellow', outline='orange')

        frames.append(img)

    return frames

# ===== ZOMBIE SPRITES =====

def generate_zombie_head(draw, x, y, is_attacking=False):
    """Helper to draw zombie head"""
    head_color = (100, 180, 100)
    draw.ellipse([x - 15, y - 15, x + 15, y + 15], fill=head_color, outline='black', width=2)

    # Eyes
    draw.ellipse([x - 8, y - 5, x - 3, y + 5], fill='white', outline='black')
    draw.ellipse([x + 3, y - 5, x + 8, y + 5], fill='white', outline='black')
    draw.ellipse([x - 6, y, x - 4, y + 2], fill='black')
    draw.ellipse([x + 4, y, x + 6, y + 2], fill='black')

    # Mouth/teeth
    draw.line([(x - 10, y + 8), (x + 10, y + 8)], fill='black', width=1)

def generate_cone_zombie_frames():
    """Generate Cone Zombie sprite"""
    frames = []
    for i in range(8):
        img = Image.new('RGBA', (FRAME_SIZE, FRAME_SIZE), (255, 255, 255, 255))
        draw = ImageDraw.Draw(img)

        # Body moves left slightly
        body_offset = 2 if (i % 2 == 0) else 0

        # Body
        draw.rectangle([20 + body_offset, 40, 80 + body_offset, 75],
                      fill=(100, 180, 100), outline='black', width=2)

        # Head
        generate_zombie_head(draw, 50 + body_offset, 30, i < 4)

        # Cone helmet (brown triangle)
        cone_points = [(40 + body_offset, 15), (60 + body_offset, 15), (50 + body_offset, 5)]
        draw.polygon(cone_points, fill=(139, 101, 50), outline='black')

        # Arms
        draw.rectangle([15 + body_offset, 45, 20 + body_offset, 65], fill=(100, 180, 100), outline='black')
        draw.rectangle([80 + body_offset, 45, 85 + body_offset, 65], fill=(100, 180, 100), outline='black')

        frames.append(img)

    return frames

def generate_bucket_zombie_frames():
    """Generate Bucket Zombie sprite"""
    frames = []
    for i in range(8):
        img = Image.new('RGBA', (FRAME_SIZE, FRAME_SIZE), (255, 255, 255, 255))
        draw = ImageDraw.Draw(img)

        body_offset = 2 if (i % 2 == 0) else 0

        # Body (larger)
        draw.rectangle([15 + body_offset, 40, 85 + body_offset, 80],
                      fill=(100, 180, 100), outline='black', width=2)

        # Head under bucket
        generate_zombie_head(draw, 50 + body_offset, 32, i < 4)

        # Silver bucket helmet (rectangle with shiny effect)
        draw.rectangle([35 + body_offset, 10, 65 + body_offset, 30],
                      fill=(192, 192, 192), outline='black', width=2)
        # Shiny part
        draw.rectangle([38 + body_offset, 12, 45 + body_offset, 18], fill='white', outline='gray')

        # Arms
        draw.rectangle([10 + body_offset, 50, 15 + body_offset, 70], fill=(100, 180, 100), outline='black')
        draw.rectangle([85 + body_offset, 50, 90 + body_offset, 70], fill=(100, 180, 100), outline='black')

        frames.append(img)

    return frames

def generate_polevaulter_zombie_frames():
    """Generate Pole Vaulter Zombie sprite"""
    frames = []
    for i in range(8):
        img = Image.new('RGBA', (FRAME_SIZE, FRAME_SIZE), (255, 255, 255, 255))
        draw = ImageDraw.Draw(img)

        # Jumping/running animation
        jump_y = -10 if (i < 4) else 0

        # Body
        draw.rectangle([25, 45 + jump_y, 75, 80 + jump_y],
                      fill=(100, 180, 100), outline='black', width=2)

        # Head
        generate_zombie_head(draw, 50, 30 + jump_y, False)

        # Pole (diagonal when jumping)
        angle = 30 if (i < 4) else 0
        pole_start_x = 70
        pole_end_x = 85
        draw.line([(pole_start_x, 40 + jump_y), (pole_end_x, 20 + jump_y)],
                 fill=(139, 101, 50), width=4)

        # Arms
        draw.rectangle([20, 55 + jump_y, 25, 75 + jump_y], fill=(100, 180, 100), outline='black')
        draw.rectangle([75, 55 + jump_y, 80, 75 + jump_y], fill=(100, 180, 100), outline='black')

        frames.append(img)

    return frames

def generate_football_zombie_frames():
    """Generate Football Zombie sprite"""
    frames = []
    for i in range(8):
        img = Image.new('RGBA', (FRAME_SIZE, FRAME_SIZE), (255, 255, 255, 255))
        draw = ImageDraw.Draw(img)

        body_offset = 2 if (i % 2 == 0) else 0

        # Large armored body (darker)
        draw.rectangle([10 + body_offset, 35, 90 + body_offset, 85],
                      fill=(80, 160, 80), outline='black', width=3)

        # Armor plates on chest
        draw.rectangle([25 + body_offset, 50, 45 + body_offset, 70], fill='gray', outline='black', width=1)
        draw.rectangle([55 + body_offset, 50, 75 + body_offset, 70], fill='gray', outline='black', width=1)

        # Head
        generate_zombie_head(draw, 50 + body_offset, 25, i < 4)

        # Football helmet (red)
        helmet_y = 10
        draw.ellipse([40 + body_offset, helmet_y, 60 + body_offset, helmet_y + 18],
                    fill=(200, 0, 0), outline='black', width=2)
        # Face guard
        draw.rectangle([45 + body_offset, helmet_y + 5, 55 + body_offset, helmet_y + 15],
                      fill=None, outline='black', width=1)

        # Arms
        draw.rectangle([5 + body_offset, 50, 10 + body_offset, 75], fill=(80, 160, 80), outline='black', width=2)
        draw.rectangle([90 + body_offset, 50, 95 + body_offset, 75], fill=(80, 160, 80), outline='black', width=2)

        frames.append(img)

    return frames

def main():
    print("Generating sprite assets for Plants vs Zombies...")

    base_path = os.path.dirname(os.path.abspath(__file__))
    resources_path = os.path.join(base_path, "Resources", "Image")

    # Generate plant sprites
    plants_output = {
        "Plants/wallnut": generate_walnut_frames(),
        "Plants/potato": generate_potato_frames(),
        "Plants/repeater": generate_repeater_frames(),
    }

    for plant_name, frames in plants_output.items():
        output_path = os.path.join(resources_path, plant_name)
        ensure_dir(output_path)
        save_frames(frames, output_path, plant_name)
        print("[OK] Generated {} frames for {}".format(len(frames), plant_name))

    # Generate zombie sprites
    zombies_output = {
        "zombies/cone": generate_cone_zombie_frames(),
        "zombies/bucket": generate_bucket_zombie_frames(),
        "zombies/polevaulter": generate_polevaulter_zombie_frames(),
        "zombies/football": generate_football_zombie_frames(),
    }

    for zombie_name, frames in zombies_output.items():
        output_path = os.path.join(resources_path, zombie_name)
        ensure_dir(output_path)
        save_frames(frames, output_path, zombie_name)
        print("[OK] Generated {} frames for {}".format(len(frames), zombie_name))

    print("\n[OK] All sprites generated successfully!")
    print(f"Output directory: {resources_path}")

if __name__ == "__main__":
    main()
