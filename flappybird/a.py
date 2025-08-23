from PIL import Image
import os

# Input files
input_files = ["assets/bird1.png", "assets/bird2.png", "assets/bird3.png"]
output_dir = "normalized_sheets"

# Target frame size
frame_size = 40   # each frame will be 40x40
grid_size = 4     # 4x4 grid
target_size = (frame_size * grid_size, frame_size * grid_size)  # (160, 160)

os.makedirs(output_dir, exist_ok=True)

for file in input_files:
    sheet = Image.open(file).convert("RGBA")

    # Resize to (160x160)
    sheet_resized = sheet.resize(target_size, Image.Resampling.LANCZOS)

    # Save
    out_path = os.path.join(output_dir, os.path.basename(file))
    sheet_resized.save(out_path)

    print(f"✅ Resized {file} -> {out_path} ({target_size[0]}x{target_size[1]})")
