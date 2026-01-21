# pip install pillow

from PIL import Image, ImageDraw, ImageFont

def conv_fnt(image, char):
    print(f"// '{char}' image.size: {image.size}")
    pixels = image.load()
    w, h = image.size
    print(f"glyph_{char}_width db {w}")
    print(f"glyph_{char}_height db {h}")
    print(f"glyph_{char} db")
    for y in range(h):
        for x in range(w):
            r, g, b, a = pixels[x, y]
            if r != g or g != b:
                print("Not equal!");
            print(" 0x%02X," % a, end = "")
            if r != 0:
                print("Alarm!!!")

        print("")

font_path = "RobotoMono-Regular.ttf"
characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890"
font_size = 24

font = ImageFont.truetype(font_path, font_size)

for char in characters:
    bbox = font.getbbox(char)
    print(bbox)
    image_width = bbox[2] - bbox[0] + 20  # paddings
    image_height = bbox[3] - bbox[1] + 30
    
    # Create image with transparent font
    image = Image.new("RGBA", (image_width, image_height), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Draw char
    draw.text((10, 10), char, font = font, fill = "black")
    image = image.crop(image.getbbox())
    conv_fnt(image, char)
    image.save(f"chr-{char}-{ord(char)}.png")
