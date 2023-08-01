import urllib.request
import json
from pprint import pprint

def readThingSpeak() :
    TS = urllib.request.urlopen("https://api.thingspeak.com/channels/908037/feeds.json?results=1&timezone=Asia%2FSeoul")

    response = TS.read()
    data=json.loads(response)
    pprint(data)
    b = data['channel']['field3']
    c = data['feeds'][-1]['field3']
    print (b + " : " + c)
    TS.close()

if __name__ == '__main__':
    readThingSpeak()
