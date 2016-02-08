import array
import itertools
import random
from PIL import Image
from ola.ClientWrapper import ClientWrapper
from tepilepsy import *

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    wrapper.AddEvent(tick_interval, SendDMXFrame)

    for rng,data in itertools.izip(rngs, tepilepsy.DMXData()):
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

def squid(dmxrngs, t, size):
    global rngs
    rngs = dmxrngs

    global tick_interval
    tick_interval = t

    global tepilepsy
    tepilepsy = Tepilepsy()

    colors = ((255, 0, 0), (0, 255, 0), (0, 0, 255), (255, 255, 0), (0, 255, 255), (255, 255, 255))
    i = 0
    for x in range(0, tepilepsy.GetWidth() - (size - 1), size):
        for y in range(0, tepilepsy.GetHeight() - (size - 1), size):
            for j in range(size):
                for k in range(size):
                    tepilepsy.SetPixel((x+j, y+k), colors[i % len(colors)])
            i+=1

    global wrapper
    wrapper = ClientWrapper()

    wrapper.AddEvent(22, SendDMXFrame)
    wrapper.Run()
