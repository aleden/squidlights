import array
import collections
from ola.ClientWrapper import ClientWrapper

TICK_INTERVAL = 3000  # in ms

def DmxSent(state):
    if not state.Succeeded():
        wrapper.Stop()

def SendDMXFrame():
  # schdule a function call in the future
  # we do this first in case the frame computation takes a long time.
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
    SendDMXFrame()
    wrapper.Run()
