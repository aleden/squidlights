import array
import colorsys
import operator
import numpy as np
import math
import threading
from ola.ClientWrapper import ClientWrapper

TICK_INTERVAL = 33  # in ms - so display at approximately 30 fps

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    global t
    global beg_hsv
    global end_hsv

    t += TICK_INTERVAL

    if t > period:
        t -= period
        beg_hsv, end_hsv = end_hsv, beg_hsv

    progress = float(t) / float(period)
    hsv_delta = tuple(map(operator.sub, end_hsv, beg_hsv))
    hsv_addend = tuple(map(operator.mul, tuple([progress]*3), hsv_delta))

    hsv = tuple(map(operator.add, beg_hsv, hsv_addend))
    clr = tuple(map(int, map(operator.mul, colorsys.hsv_to_rgb(*hsv), tuple([255.0]*3))))

    # prepare DMX512 serial data
    datas = []
    for rng in rngs:
        data = array.array('B', [0] * 512)
        for chann in range(rng[1], rng[2] - 3, 3):
            data[chann+0] = clr[0]
            data[chann+1] = clr[1]
            data[chann+2] = clr[2]
        datas.append(data)
    
    # display frame
    for rng, data in zip(rngs, datas):
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

    threading.Timer((1.0/1000.0)*float(TICK_INTERVAL), SendDMXFrame).start()

def squid(dmxrngs, begclr, endclr, delta_t):
    global rngs
    rngs = dmxrngs

    global beg_hsv
    beg_hsv = colorsys.rgb_to_hsv(*tuple(map(operator.div, map(float, begclr), [255.0]*3)))

    global end_hsv
    end_hsv = colorsys.rgb_to_hsv(*tuple(map(operator.div, map(float, endclr), [255.0]*3)))

    global period
    period = delta_t

    global t
    t = -TICK_INTERVAL

    global wrapper
    wrapper = ClientWrapper()

    SendDMXFrame()
