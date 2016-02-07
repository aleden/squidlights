import array
from ola.ClientWrapper import ClientWrapper

TICK_INTERVAL = 10000  # set every 10 seconds just in case

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    wrapper.AddEvent(TICK_INTERVAL, SendDMXFrame)

    for rng,data in zip(rngs, datas):
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

def squid(dmxrngs, clr):
    global rngs
    rngs = dmxrngs

    global datas
    datas = []
    for rng in rngs:
        data = array.array('B', [0] * rng[1])
        for chann in range(rng[1], rng[2] - 2, 3):
            data.append(clr[0])
            data.append(clr[1])
            data.append(clr[2])
        datas.append(data)

    global wrapper
    wrapper = ClientWrapper()

    wrapper.AddEvent(22, SendDMXFrame)
    wrapper.Run()
