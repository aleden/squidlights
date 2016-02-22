import array
import itertools
from ola.ClientWrapper import ClientWrapper

TICK_INTERVAL = 1000 # set every second

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    wrapper.AddEvent(TICK_INTERVAL, SendDMXFrame)

    for rng,data in itertools.izip(rngs, datas):
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

def squid(dmxrngs, dmx_first, dmx_last):
    global rngs
    rngs = dmxrngs

    dmx_beg = dmx_first - 1
    dmx_end = dmx_last - 1

    global datas
    datas = []
    for rng in rngs:
        data = array.array('B', [0] * 512)
        for chann in range(dmx_beg, dmx_end):
            data[chann] = 255
        datas.append(data)

    global wrapper
    wrapper = ClientWrapper()

    wrapper.AddEvent(22, SendDMXFrame)
    wrapper.Run()
