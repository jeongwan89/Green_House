import sys
import urllib.request
import json
from pprint import pprint

import ttkbootstrap as ttk
from ttkbootstrap.constants import *

sys.setrecursionlimit = 2500

GAP_SEC = 15000 # time delay in millisecond
app = ttk.Window()
app.geometry("800x420+0+0")

MY_URL = ["https://api.thingspeak.com/channels/846408/feeds.json?results=1&timezone=Asia%2FSeoul",
            "https://api.thingspeak.com/channels/908037/feeds.json?results=1&timezone=Asia%2FSeoul",
            "https://api.thingspeak.com/channels/1468404/feeds.json?results=1&timezone=Asia%2FSeoul",
            "https://api.thingspeak.com/channels/1468405/feeds.json?results=1&timezone=Asia%2FSeoul"]

GREEN_HOUSE = []

for my_url_str in MY_URL:
    TS = urllib.request.urlopen(my_url_str)
    GREEN_HOUSE.append(json.loads(TS.read()))
    pprint(GREEN_HOUSE[-1])
    a = GREEN_HOUSE[-1]['channel']['name']
    b = GREEN_HOUSE[-1]['channel']['field3']
    c = GREEN_HOUSE[-1]['feeds'][-1]['field3']
    print (a + " : " + b + " : " + c)
    TS.close()

def read_thing_speak() :
    '''
    온실에서 ThinsSpeak 서버에 게시한 data를 읽는 함수
    주의: 무한 루프에서 실행될 때, 매번 지속적으로 이 함수가 실행되면, 서버에서 블록킹 되어 버린다.
        내 단말기에서 올리는 시간은 15초마다 한번씩이니까 여기에 동조해 설정하면 많은 문제가 해결된다.
    인수:
        없다.
    리턴:
        없다.
    인수와 리턴값이 없는 이유는 global GREEN_HOUSE[]안에 바로 쓰고 읽고 하기 때문이다.
    
    '''
    global MY_URL
    global GREEN_HOUSE
    i = 0

    for my_url_str in MY_URL:
        TS = urllib.request.urlopen(my_url_str)
        GREEN_HOUSE[i] = json.loads(TS.read())
        pprint(GREEN_HOUSE[i])
        a = GREEN_HOUSE[i]['channel']['name']
        b = GREEN_HOUSE[i]['channel']['field3']
        c = GREEN_HOUSE[i]['feeds'][-1]['field3']
        print (a + " : " + b + " : " + c)
        TS.close()
        i = i + 1

def heat_change(*args):
    '''
    이 함수는 meter의 값, 곧 amountusedvar 값이 바뀔 때 호출되는 함수이다.
    함수는 if -elif -else 구조로 유지된다.
    amountusedvar의 값이 조건부 구조로 판별되면, bootstyle을 조절한다.
    meter1.amountused.trace함수에 연결되어 있다. 
    아마 이 함수에 연결된 파라메터가 heat_change(*args)에 전달 되는 것 같다.
    '''
    global HEAT_GAUGE

    for m in HEAT_GAUGE:

        if m['amountused'] <= 5:
            m['bootstyle'] = DARK
        elif m['amountused'] <= 10 :
            m['bootstyle'] = DANGER
        elif m['amountused'] <= 15 :
            m['bootstyle'] = WARNING
        elif m['amountused'] <= 25 :
            m['bootstyle'] = SUCCESS
        elif m['amountused'] <=31 :
            m['bootstyle'] = WARNING
        elif m['amountused'] <= 35 :
            m['bootstyle'] = DANGER
        else :
            m['bootstyle'] = DARK

def hum_change(*args):
    '''
    이 함수는 meter의 값, 곧 amountusedvar 값이 바뀔 때 호출되는 함수이다.
    함수는 if -elif -else 구조로 유지된다.
    amountusedvar의 값이 조건부 구조로 판별되면, bootstyle을 조절한다.
    meter1.amountused.trace함수에 연결되어 있다. 
    아마 이 함수에 연결된 파라메터가 heat_change(*args)에 전달 되는 것 같다.
    '''
    global HUM_GAUGE

    for m in HUM_GAUGE:

        if m['amountused'] <= 50:
            m['bootstyle'] = DARK
        elif m['amountused'] <= 60 :
            m['bootstyle'] = DANGER
        elif m['amountused'] <= 70 :
            m['bootstyle'] = WARNING
        elif m['amountused'] <= 80 :
            m['bootstyle'] = SUCCESS
        elif m['amountused'] <=90 :
            m['bootstyle'] = WARNING
        elif m['amountused'] <= 95 :
            m['bootstyle'] = DANGER
        else :
            m['bootstyle'] = DARK

def soil_temp_change(*args):
    '''
    이 함수는 meter의 값, 곧 amountusedvar 값이 바뀔 때 호출되는 함수이다.
    함수는 if -elif -else 구조로 유지된다.
    amountusedvar의 값이 조건부 구조로 판별되면, bootstyle을 조절한다.
    meter1.amountused.trace함수에 연결되어 있다. 
    아마 이 함수에 연결된 파라메터가 heat_change(*args)에 전달 되는 것 같다.
    
    '''
    global SOIL_TEMP_GAUGE

    for m in SOIL_TEMP_GAUGE:

        if m['amountused'] <= 5:
            m['bootstyle'] = DARK
        elif m['amountused'] <= 7 :
            m['bootstyle'] = DANGER
        elif m['amountused'] <= 13 :
            m['bootstyle'] = WARNING
        elif m['amountused'] <= 21 :
            m['bootstyle'] = SUCCESS
        elif m['amountused'] <= 24 :
            m['bootstyle'] = WARNING
        elif m['amountused'] <= 28 :
            m['bootstyle'] = DANGER
        else :
            m['bootstyle'] = DARK

def update_state():
    '''
    ttk.after함수에 바인딩된 함수. ttk.after에 정의된 시간마다 (이 모듈에서는 gap_sec에 정의된
    15,000 millisec) thingspeak의 자료를 읽고 이것을 meter widget에 반영하는 함수
    '''
    global GREEN_HOUSE
    global HEAT_GAUGE
    global SOIL_TEMP_GAUGE

    read_thing_speak()

    for i in range(0,4) :
        HEAT_GAUGE[i]['amountused'] = GREEN_HOUSE[i]['feeds'][-1]['field3']
        HUM_GAUGE[i]['amountused'] = GREEN_HOUSE[i]['feeds'][-1]['field4']
        SOIL_TEMP_GAUGE[i]['amountused'] = GREEN_HOUSE[i]['feeds'][-1]['field5']
    app.after(GAP_SEC, update_state)


HEAT_GAUGE = []
HUM_GAUGE = []
SOIL_TEMP_GAUGE = []

#meter1~4 초기화한다.
for i in range(0,4):
    label_heat = "%d동 온도" %(i+1)
    label_hum = "%d동 습도" %(i+1)
    label_soil_temp = "%d동 배지온도" %(i+1)
    HEAT_GAUGE.append(ttk.Meter(app, metersize=160, padding=0, amounttotal=90, 
                                 arcrange=315, arcoffset=-225, amountused=0, textright='°C',
                                 subtext=label_heat, wedgesize=0, bootstyle='default', interactive=True)
                      ) 
    HUM_GAUGE.append(ttk.Meter(app, metersize=160, padding=0, amounttotal=100, 
                                 arcrange=180, arcoffset=-180, amountused=0, textright='%',
                                 subtext=label_hum, wedgesize=0, bootstyle='default', interactive=True)
                      )
    SOIL_TEMP_GAUGE.append(ttk.Meter(app, metersize=160, padding=0, amountused=0, amounttotal=70, 
                                 metertype='semi', textright='°C', stripethickness=10,
                                 subtext=label_soil_temp, wedgesize=0, bootstyle='default', interactive=True)
                          )
for i in range(0, 4):
    HEAT_GAUGE[i].grid(row=0, column=i)
    HUM_GAUGE[i].grid(row=1, column=i)
    SOIL_TEMP_GAUGE[i].grid(row=2, column=i)


for m in HEAT_GAUGE:
    m.amountusedvar.trace("w", heat_change)
for n in HUM_GAUGE:
    n.amountusedvar.trace("w", hum_change)
for l in SOIL_TEMP_GAUGE:
    l.amountusedvar.trace("w", soil_temp_change)

for i in range(0,4):
    HEAT_GAUGE[i].configure(amountused = GREEN_HOUSE[i]['feeds'][-1]['field3'])
    HUM_GAUGE[i].configure(amountused = GREEN_HOUSE[i]['feeds'][-1]['field4'])
    SOIL_TEMP_GAUGE[i].configure(amountused = GREEN_HOUSE[i]['feeds'][-1]['field5'])
    
app.after(GAP_SEC, update_state)
app.mainloop()