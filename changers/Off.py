import array
from ola.ClientWrapper import ClientWrapper

TICK_INTERVAL = 10000  # turn off every 10 seconds just in case

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    wrapper.AddEvent(TICK_INTERVAL, SendDMXFrame)

    for rng in rngs:
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

def squid(dmxrngs):
    global rngs
    rngs = dmxrngs

    global data
    data = array.array('B', [0] * 512)

    global wrapper
    wrapper = ClientWrapper()

    wrapper.AddEvent(22, SendDMXFrame)
    wrapper.Run()
