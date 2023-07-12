from tkinter import *
from tkinter import *
import _thread
import time


def update():
    while True:
      t=time.strftime('%I:%M:%S',time.localtime())
      time_label['text'] = t



win = Tk()
win.geometry('200x200')

time_label = Label(win, text='0:0:0', font=('',15))
time_label.pack()


_thread.start_new_thread(update,())

win.mainloop()