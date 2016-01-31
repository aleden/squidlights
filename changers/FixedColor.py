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

  for rng,data in zip(rngs, datas):
    wrapper.Client().SendDmx(rng[0], data, DmxSent)

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
  wrapper.Run()
