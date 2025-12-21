import tkinter as tk

cnt = 1
w = 480
h = 272

image_data = []
img = 0
root = 0

def read_image():
  global image_data
  with open("dump.bin", "rb") as file:
    image_data = file.read()
    if len(image_data) != 480*272*2:
      print("Image size should be 480*272*2 but it's", len(image_data))
  print("Read ok")

def rgb2str(clr):
  r = clr & 0x1F
  clr = clr >> 5
  g = clr & 0x1F
  clr = clr >> 5
  b = clr & 0x1F
  return "#%02X%02X%02X" % (r<<3, g<<3, b<<3)

def update_loop():
  read_image()
  global img
  global root
  if image_data:
    print("Out...")
    for y in range(0, 272):
      for x in range(0, 480):
        clr = image_data[y*480 + x]
        img.put(rgb2str(clr), (x, y))
  else:
    print("No image")

  root.after(10, update_loop)

def main():
  global root
  root = tk.Tk()
  root.title("74HCPU")
  root.resizable(False, False)
  root.geometry("{}x{}".format(w + 20, h + 10))

  canvas = tk.Canvas(root, width=w, height=h, bg='white', highlightthickness=0)
  canvas.pack()

  global img
  img = tk.PhotoImage(width=w, height=h)
  canvas.create_image(0, 0, image=img, anchor="nw")

  update_loop()
  root.mainloop()

main()
