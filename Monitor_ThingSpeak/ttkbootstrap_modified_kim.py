import urllib.request
import json
from pprint import pprint

import ttkbootstrap as ttk
from ttkbootstrap.constants import *


GAP_SEC = 15000 # time delay in millisecond
app = ttk.Window()

'''
def test(meter):
      meter.step()
      meter.after(10, test, meter)
'''
def read_thing_speak() :
    '''
    온실에서 ThinsSpeak 서버에 게시한 data를 읽는 함수
    주의: 무한 루프에서 실행될 때, 매번 지속적으로 이 함수가 실행되면, 서버에서 블록킹 되어 버린다.
        내 단말기에서 올리는 시간은 15초마다 한번씩이니까 여기에 동조해 설정하면 많은 문제가 해결된다.
    리턴값으로 읽은 json 데이터를 돌려보핸다.
    '''
    TS = urllib.request.urlopen("https://api.thingspeak.com/channels/908037/feeds.json?results=1&timezone=Asia%2FSeoul")

    response = TS.read()
    data=json.loads(response)
    pprint(data)
    b = data['channel']['field3']
    c = data['feeds'][-1]['field3']
    print (b + " : " + c)
    TS.close()
    
    return data

def meter_change(*args):
    '''
    이 함수는 meter의 값, 곧 amountusedvar 값이 바뀔 때 호출되는 함수이다.
    함수는 if -elif -else 구조로 유지된다.
    amountusedvar의 값이 조건부 구조로 판별되면, bootstyle을 조절한다.
    meter1.amountused.trace함수에 연결되어 있다. 
    아마 이 함수에 연결된 파라메터가 meter_change(*args)에 전달 되는 것 같다.
    '''
    if meter1['amountused'] <= 5:
        meter1['bootstyle'] = DARK
    elif meter1['amountused'] <= 10 :
        meter1['bootstyle'] = DANGER
    elif meter1['amountused'] <= 15 :
        meter1['bootstyle'] = WARNING
    elif meter1['amountused'] <= 25 :
        meter1['bootstyle'] = SUCCESS
    elif meter1['amountused'] <=31 :
        meter1['bootstyle'] = WARNING
    elif meter1['amountused'] <= 35 :
        meter1['bootstyle'] = DANGER
    else :
        meter1['bootstyle'] = DARK

def update_state():
    '''
    ttk.after함수에 바인딩된 함수. ttk.after에 정의된 시간마다 (이 모듈에서는 gap_sec에 정의된
    15,000 millisec) thingspeak의 자료를 읽고 이것을 meter widget에 반영하는 함수
    '''
    green_house_1 = read1_thing_speak()
    
    meter1['amountused'] = green_house_1['feeds'][-1]['field3']

"""
meter1 = ttk.Meter(app, metersize=180, padding=20, amountused=25, metertype='semi', subtext='miles per hour',
      interactive=True)
meter2 = ttk.Meter(app, metersize=180, padding=20, amountused=1800, amounttotal=2600, subtext='storage used', textleft='gb',
      bootstyle='info', stripethickness=10, interactive=True)
meter3 = ttk.Meter(app, metersize=180, padding=20, stripethickness=2, amountused=40, subtext='project capacity', textleft='%',
      bootstyle='success', interactive=True)
meter4 = ttk.Meter(app, metersize=180, padding=10, amounttotal=280, arcrange=180, arcoffset=-180, amountused=75, textleft='°',
      subtext='heat temperature', wedgesize=5, bootstyle='danger', interactive=True)
# 위의 meter1~4위젯은 ttkbootstrap홈페이지에서 카피하였다. 다음은 내 모니터에 actualization을 하기 위해 채택한다.
"""
meter1 = ttk.Meter(app, metersize=180, padding=20, amounttotal=70, 
                    arcrange=270, arcoffset=-180, amountused=0, textleft='°C',
                    subtext='1동 온도', wedgesize=0, bootstyle='default', interactive=True)
meter2 = ttk.Meter(app, metersize=180, padding=20, amounttotal=70, 
                    arcrange=270, arcoffset=-180, amountused=0, textleft='°C',
                    subtext='2동 온도', wedgesize=10, bootstyle='default', interactive=True)
meter3 = ttk.Meter(app, metersize=180, padding=20, amounttotal=70, arcrange=270, 
                    arcoffset=-180, amountused=0, textleft='°C',
                    subtext='3동 온도', wedgesize=10, bootstyle='default', interactive=True)
meter4 = ttk.Meter(app, metersize=180, padding=20, amounttotal=70, arcrange=270, 
                    arcoffset=-180, amountused=0, textleft='°C',
                    subtext='4동 온도', wedgesize=10, bootstyle='default', interactive=True)

meter1.grid(row=0, column=0)
meter2.grid(row=0, column=1)
meter3.grid(row=1, column=0)
meter4.grid(row=1, column=1)

meter2.amountusedvar.trace("w", meter_change)
initial_data_2 = read_thing_speak()
meter2.configure(amountused = initial_data_2['feeds'][-1]['field3'])
app.after(GAP_SEC, update_state)
app.mainloop()