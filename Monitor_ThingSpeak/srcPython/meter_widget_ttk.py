import ttkbootstrap as ttk
from ttkbootstrap.constants import *

my_w = ttk.Window()
my_w.geometry("220x260")  # width and height

m1 = ttk.Meter(
    my_w,
    amounttotal=59,
    amountused=0,
    meterthickness=20,
    bootstyle=INFO,
    metersize=200,
    stripethickness=6
)
m1.grid(row=0, column=0, padx=5, pady=10)

count=0 
gap_sec=10 # Time delay in Millseconds 
def my_second():
    global count
    if count <=59:
        m1['amountused']=count #  update meter value
        count=count+1
        my_w.after(gap_sec,my_second) # time delay in Milli seconds 
        
    else:
        count=0
    
b1=ttk.Button(my_w,text='Start',command=my_second)
b1.grid(row=1,column=0)
my_w.mainloop()