import tkinter as tk

cnt = 1
w = 480
h = 272

image_data = []

def read_image():
  with open("dump.bin", "rb") as file:
    image_data = file.read()
    if len(image_data) != 480*272*2:
      print("Image size should be 480*272*2 but it's", len(image_data))

def update_loop():
  read_image()
  if image_data:
    for y in range(0, 272):
      for x in range(0, 480):
        img.put(clr, (x, y))

  root.after(10, update_loop)

root = tk.Tk()
root.title("74HCPU")
root.resizable(False, False)
root.geometry("{}x{}".format(w + 20, h + 10))

canvas = tk.Canvas(root, width=w, height=h, bg='white', highlightthickness=0)
canvas.pack()

img = tk.PhotoImage(width=w, height=h)
canvas.create_image(0, 0, image=img, anchor="nw")

update_loop()

root.mainloop()
