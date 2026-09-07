from PIL import Image
import os


INPUT_DIR = "."

faces = {
    "nx": "nx.png",  # -X
    "px": "px.png",  # +X
    "ny": "ny.png",  # -Y
    "py": "py.png",  # +Y
    "nz": "nz.png",  # -Z
    "pz": "pz.png",  # +Z
}


# Load images
images = {}

for name, filename in faces.items():
    path = os.path.join(INPUT_DIR, filename)

    if not os.path.exists(path):
        raise FileNotFoundError(
            f"Missing cubemap face: {path}"
        )

    images[name] = Image.open(path).convert("RGBA")


# Check that all images have the same size
size = images["px"].size

for name, image in images.items():
    if image.size != size:
        raise ValueError(
            f"{name}.png has size {image.size}, expected {size}"
        )


w, h = size


# Cubemap layout:
#
#              PY
#
#      NX      PZ      PX      NZ
#
#              NY
#
atlas = Image.new(
    "RGBA",
    (w * 4, h * 3),
    (0, 0, 0, 0)
)


# +Y
atlas.paste(images["py"], (w, 0))

# -X, +Z, +X, -Z
atlas.paste(images["nx"], (0, h))
atlas.paste(images["pz"], (w, h))
atlas.paste(images["px"], (w * 2, h))
atlas.paste(images["nz"], (w * 3, h))

# -Y
atlas.paste(images["ny"], (w, h * 2))


# Save
output = "cubemap.png"
atlas.save(output)

print(f"Cubemap saved to: {output}")
print(f"Size: {atlas.width}x{atlas.height}")