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
    global rngs
    global TICK_INTERVAL
    global wrapper

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

    threading.Timer((1.0/1000.0)*float(TICK_INTERVAL), SendDMXFrame).start()

def squid(dmxrngs, delta_t):
    global rngs
    rngs = dmxrngs

    global TICK_INTERVAL
    TICK_INTERVAL = delta_t

    global wrapper
    wrapper = ClientWrapper()

    SendDMXFrame()
