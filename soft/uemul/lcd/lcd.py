import tkinter as tk

cnt = 1

def update_loop():
  global cnt
  cnt = cnt + 1
  if cnt > 255:
    cnt = 0
  clr = "#%02x%02x%02x" % (0, 0, cnt)
  img.put(clr, to=(10, 10, 20, 20))
  root.after(10, update_loop)

w = 480
h = 272

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
