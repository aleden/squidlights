import array
import colorsys
import operator
import threading
import random
from ola.ClientWrapper import ClientWrapper

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    wrapper.AddEvent(tick_interval, SendDMXFrame)

    datas = []
    for rng in rngs:
        data = array.array('B', [0] * 512)
        for chann in range(rng[1], rng[2] - 3, 3):
            data[chann+0] = random.randint(0, 255)
            data[chann+1] = random.randint(0, 255)
            data[chann+2] = random.randint(0, 255)
        datas.append(data)
    
    # display frame
    for rng, data in zip(rngs, datas):
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

def squid(dmxrngs, delta_t):
    global rngs
    rngs = dmxrngs

    global tick_interval
    tick_interval = delta_t

    global wrapper
    wrapper = ClientWrapper()

    wrapper.AddEvent(22, SendDMXFrame)
    wrapper.Run()
