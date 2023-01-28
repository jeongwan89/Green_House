import urllib.request
import json
from pprint import pprint

import ttkbootstrap as ttk
from ttkbootstrap.constants import *


GAP_SEC = 15000 # time delay in millisecond
app = ttk.Window()

def read_thing_speak() :
    '''
    온실에서 ThinsSpeak 서버에 게시한 data를 읽는 함수
    주의: 무한 루프에서 실행될 때, 매번 지속적으로 이 함수가 실행되면, 서버에서 블록킹 되어 버린다.
        내 단말기에서 올리는 시간은 15초마다 한번씩이니까 여기에 동조해 설정하면 많은 문제가 해결된다.
    리턴값으로 읽은 json 데이터를 돌려보핸다.
    return:
        각 온실의 json값을 list로 모아서 보낸다.
    '''
    my_url = ["https://api.thingspeak.com/channels/846408/feeds.json?results=1&timezone=Asia%2FSeoul",
              "https://api.thingspeak.com/channels/908037/feeds.json?results=1&timezone=Asia%2FSeoul",
              "https://api.thingspeak.com/channels/1468404/feeds.json?results=1&timezone=Asia%2FSeoul",
              "https://api.thingspeak.com/channels/1468405/feeds.json?results=1&timezone=Asia%2FSeoul"]
    response = []
    data = []
    i = 0

    for my_url_str in my_url:
        TS = urllib.request.urlopen(my_url_str)
        response.append(TS.read())
        data.append(json.loads(response[i]))
        pprint(data[i])
        a = data[i]['channel']['name']
        b = data[i]['channel']['field3']
        c = data[i]['feeds'][-1]['field3']
        print (a + " : " + b + " : " + c)
        TS.close()
        i = i + 1
    
    return data

def meter_change(*args):
    '''
    이 함수는 meter의 값, 곧 amountusedvar 값이 바뀔 때 호출되는 함수이다.
    함수는 if -elif -else 구조로 유지된다.
    amountusedvar의 값이 조건부 구조로 판별되면, bootstyle을 조절한다.
    meter1.amountused.trace함수에 연결되어 있다. 
    아마 이 함수에 연결된 파라메터가 meter_change(*args)에 전달 되는 것 같다.
    '''
    global METER_GAUGE

    for m in METER_GAUGE:

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

GREEN_HOUSE = []

def update_state():
    '''
    ttk.after함수에 바인딩된 함수. ttk.after에 정의된 시간마다 (이 모듈에서는 gap_sec에 정의된
    15,000 millisec) thingspeak의 자료를 읽고 이것을 meter widget에 반영하는 함수
    '''
    global GREEN_HOUSE
    global METER_GAUGE

    GREEN_HOUSE = read_thing_speak()

    for i in range(0,4) :
        METER_GAUGE[i]['amountused'] = GREEN_HOUSE[i]['feeds'][-1]['field3']


METER_GAUGE = [] 
#meter1~4 초기화한다.
for i in range(0,4):
    label = "%d동 온도" %i
    METER_GAUGE.append(
                    ttk.Meter(app, metersize=180, padding=20, amounttotal=70, 
                              arcrange=270, arcoffset=-180, amountused=0, textleft='°C',
                              subtext=label, wedgesize=10, bootstyle='default', interactive=True)
                      ) 

METER_GAUGE = [] 
#meter1~4 초기화한다.
for i in range(0,4):
    label = "%d동 온도" %i
    METER_GAUGE.append(
                    ttk.Meter(app, metersize=180, padding=20, amounttotal=70, 
                              arcrange=270, arcoffset=-180, amountused=0, textleft='°C',
                              subtext=label, wedgesize=10, bootstyle='default', interactive=True)
                      ) 

METER_GAUGE[0].grid(row=0, column=0)
METER_GAUGE[1].grid(row=0, column=1)
METER_GAUGE[2].grid(row=1, column=0)
METER_GAUGE[3].grid(row=1, column=1)


for m in METER_GAUGE:
    m.amountusedvar.trace("w", meter_change)

#meter2.amountusedvar.trace("w", meter_change)
initial_data = []
initial_data = read_thing_speak()
for i in range(0,4):
    METER_GAUGE[i].configure(amountused = initial_data[i]['feeds'][-1]['field3'])
app.after(GAP_SEC, update_state)
app.mainloop()