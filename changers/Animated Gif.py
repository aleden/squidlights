import array
import itertools
from PIL import Image
from ola.ClientWrapper import ClientWrapper
from tepilepsy import *

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    global idx

    wrapper.AddEvent(tick_interval, SendDMXFrame)

    for rng, data in itertools.izip(rngs, dmxframes[idx]):
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

    idx = (idx + 1) % len(dmxframes)

def squid(dmxrngs, gif, period):
    global rngs
    rngs = dmxrngs

    global tick_interval
    tick_interval = period

    # open image, resize to fit
    im = Image.open(gif)

    global dmxframes
    dmxframes = []

    global idx
    idx = 0

    while True:
        try:
            tepilepsy = Tepilepsy()

            # set image pixels
            for x in range(im.width):
                for y in range(im.height):
                    i = im.getpixel((x, y))
                    clr = im.getpalette()[3*i:3*i+3]
                    tepilepsy.SetPixel((x, y), clr)

            dmxframes.append(tepilepsy.DMXData())

            im.seek(im.tell() + 1)
        except EOFError:
            break # end of sequence

    global wrapper
    wrapper = ClientWrapper()
    wrapper.AddEvent(22, SendDMXFrame)
    wrapper.Run()
