import tkinter as tk
import time


def refresh_clock():
    clock_label.config(
        text=time.strftime("%H:%M:%S", time.localtime())
    )
    root.after(1000, refresh_clock)  # <--


root = tk.Tk()

clock_label = tk.Label(root, font="Times 25", justify="center")
clock_label.pack()

root.after_idle(refresh_clock)  # <--
root.mainloop()