"""
이 모듈은 ThingSpeak에서 data를 받아 Python GUI로 온도와 습도를 보기 좋게 표시하기 위한 것이다.
"""
import urllib.request
import json
from pprint import pprint

import ttkbootstrap as ttk
from ttkbootstrap.constants import *

"""
def readThingSpeak() :
    TS = urllib.request.urlopen("https://api.thingspeak.com/channels/908037/feeds.json?results=1&timezone=Asia%2FSeoul")

    response = TS.read()
    data=json.loads(response)
    pprint(data)
    b = data['channel']['field3']
    c = data['feeds'][-1]['field3']
    print (b + " : " + c)
    TS.close()
"""
class Application(ttk.Win):
    def __init__(self, master=None):
        super().__init__(self, master)
        self.pack()
        #self.createWidget()

    def createWidget(self):
        self.meter = ttk.Meter(
            metersize=280,
            padding=10,
            amoutused=25,
            metertype='semi',
            subtext='celcius',
            interactive=False
        )
        self.meter.pack(side='top')

        #self.QUIT = ttk.Button(self, text='Quit', fg='red', command=root.destroy)
        #self.QUIT.pack(side='bottom')

        #self.onUpdate()

    def onUpdate(self):
        # update thingspeak data
        TS = urllib.request.urlopen("https://api.thingspeak.com/channels/908037/feeds.json?results=1&timezone=Asia%2FSeoul")
        response = TS.read()
        TS.close()
        data=json.loads(response)
        self.meter.configure(amountused=data['feeds'][-1]['field3'])
        # schedule timer to call myself after 15 second
        self.after(15000, self.onUpdate)

app = Application()
app.mainloop()
