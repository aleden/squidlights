import array
from ola.ClientWrapper import ClientWrapper
from tepilepsy import *

TICK_INTERVAL = 10000  # set every 10 seconds just in case

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    wrapper.AddEvent(TICK_INTERVAL, SendDMXFrame)

    for rng,data in zip(rngs, tepilepsy.DMXData()):
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

def squid(dmxrngs):
    global rngs
    rngs = dmxrngs

    global tepilepsy
    tepilepsy = Tepilepsy()

    for x in range(0, tepilepsy.GetWidth(), 2):
        for y in range(0, tepilepsy.GetHeight(), 6):
            tepilepsy.SetPixel((x, y), (255, 0, 0))

    global wrapper
    wrapper = ClientWrapper()

    wrapper.AddEvent(22, SendDMXFrame)
    wrapper.Run()
