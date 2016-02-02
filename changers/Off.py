import array
import collections
import threading
from ola.ClientWrapper import ClientWrapper

TICK_INTERVAL = 10.0  # in s

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    for rng in rngs:
	wrapper.Client().SendDmx(rng[0], data, DmxSent)
    threading.Timer(TICK_INTERVAL, SendDMXFrame).start()

def squid(dmxrngs):
    global rngs
    rngs = dmxrngs

    global data
    data = array.array('B', [0] * 512)

    global wrapper
    wrapper = ClientWrapper()

    SendDMXFrame()
