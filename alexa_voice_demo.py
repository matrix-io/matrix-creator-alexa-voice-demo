#! /usr/bin/env python

import os
import random
import time
import random
from creds import *
import requests
import json
import re
import subprocess
from memcache import Client

# Setup
recorded = False
servers = ["127.0.0.1:11211"]
mc = Client(servers, debug=1)
path = os.path.realpath(__file__).rstrip(os.path.basename(__file__))


def internet_on():
    print "Checking Internet Connection"
    try:
        r = requests.get('https://api.amazon.com/auth/o2/token')
        print "Connection OK"
        return True
    except:
        print "Connection Failed"
        return False


def gettoken():
    token = mc.get("access_token")
    refresh = refresh_token
    if token:
        return token
    elif refresh:
        payload = {"client_id": Client_ID, "client_secret": Client_Secret,
                   "refresh_token": refresh, "grant_type": "refresh_token", }
        url = "https://api.amazon.com/auth/o2/token"
        print("payload=")
        print(payload)
        r = requests.post(url, data=payload)
        print("res=")
        print(r.text)
        resp = json.loads(r.text)
        mc.set("access_token", resp['access_token'], 3570)
        return resp['access_token']
    else:
        return False


def alexa():
    url = 'https://access-alexa-na.amazon.com/v1/avs/speechrecognizer/recognize'
    headers = {'Authorization': 'Bearer %s' % gettoken()}
    d = {  # a dict
        "messageHeader": {
            "deviceContext": [
                {
                    "name": "playbackState",
                    "namespace": "AudioPlayer",
                    "payload": {
                            "streamId": "",
                            "offsetInMilliseconds": "0",
                            "playerActivity": "IDLE"
                    }
                }
            ]
        },
        "messageBody": {
            "profile": "alexa-close-talk",
            "locale": "en-us",
            "format": "audio/L16; rate=16000; channels=1"
        }
    }
    with open(path + 'recording.wav') as inf:
        files = [  # a list
            ('file', ('request', json.dumps(d), 'application/json; charset=UTF-8')),
            ('file', ('audio', inf, 'audio/L16; rate=16000; channels=1'))
        ]
        print type(files)
        print type(d)
        r = requests.post(url, headers=headers, files=files)
    if r.status_code == 200:
        for v in r.headers['content-type'].split(";"):
            if re.match('.*boundary.*', v):
                boundary = v.split("=")[1]
        data = r.content.split(boundary)
        for d in data:
            if (len(d) >= 1024):
                audio = d.split('\r\n\r\n')[1].rstrip('--')
                print type(audio)
        with open(path + "response.mp3", 'wb') as f:
            f.write(audio)
        os.system(
            'mpg123 -q {}1sec.mp3 {}response.mp3'.format(path + "/assets/", path))
    else:
        print "requests returned r.status_code = %r" % r.status_code


def start():
    print "Touch MATRIX Creator IR Sensor"
    process = subprocess.Popen(
        ['./micarray/build/micarray_dump'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    audio, err = process.communicate()

    rf = open(path + 'recording.wav', 'w')
    rf.write(audio)
    rf.close()
    alexa()


if __name__ == "__main__":
    print "This is a MATRIX Creator demo - not ready for production"
    print "Running workaround for GPIO 16 (IR-RX) "
    subprocess.Popen(['sudo', 'rmmod', 'lirc_rpi'])

    while internet_on() == False:
        print "."
    token = gettoken()
    os.system('mpg123 -q {}1sec.mp3 {}hello.mp3'.format(path +
                                                        "/assets/", path + "/assets/"))
    while True:
        subprocess.Popen(['gpio','edge','16','both'])
        start()
