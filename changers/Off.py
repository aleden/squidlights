import array
import itertools
from ola.ClientWrapper import ClientWrapper

TICK_INTERVAL = 1000  # set every second

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    wrapper.AddEvent(TICK_INTERVAL, SendDMXFrame)

    for rng,data in itertools.izip(rngs, datas):
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

def squid(dmxrngs):
    global rngs
    rngs = dmxrngs

    global datas
    datas = []
    for rng in rngs:
        data = array.array('B', [0] * rng[1])
        for chann in range(rng[1], rng[2]):
            data.append(0)
        datas.append(data)

    global wrapper
    wrapper = ClientWrapper()

    wrapper.AddEvent(22, SendDMXFrame)
    wrapper.Run()
