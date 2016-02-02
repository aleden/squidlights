import array
import collections
import threading
from ola.ClientWrapper import ClientWrapper

TICK_INTERVAL = 10.0  # in s

def DmxSent(state):
    if not state.Succeeded():
	sys.exit(1)

def SendDMXFrame():
    for rng,data in zip(rngs, datas):
	wrapper.Client().SendDmx(rng[0], data, DmxSent)
    threading.Timer(TICK_INTERVAL, SendDMXFrame).start()

def squid(dmxrngs, clr):
    global rngs
    rngs = dmxrngs

    global datas
    datas = []
    for rng in rngs:
	data = array.array('B', [0] * 512)
	for chann in range(rng[1], rng[2] - 3, 3):
	    data[chann+0] = clr[0]
	    data[chann+1] = clr[1]
	    data[chann+2] = clr[2]
	datas.append(data)

    global wrapper
    wrapper = ClientWrapper()

    SendDMXFrame()
