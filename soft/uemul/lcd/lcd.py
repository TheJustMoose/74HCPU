import tkinter as tk

# 1. Create the main window object
root = tk.Tk()

# 2. Add a title and set dimensions (Width x Height)
root.title("74HCPU")
root.geometry("480x272")

# 3. Add a basic label (optional)
label = tk.Label(root, text="Hello, World!")
label.pack(pady=20)

# 4. Start the application
root.mainloop()
