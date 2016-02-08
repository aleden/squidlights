import array
import itertools
from PIL import Image
from ola.ClientWrapper import ClientWrapper
from tepilepsy import *

TICK_INTERVAL = 10000  # set every 10 seconds just in case

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    wrapper.AddEvent(TICK_INTERVAL, SendDMXFrame)

    for rng,data in itertools.izip(rngs, tepilepsy.DMXData()):
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

def squid(dmxrngs, pic):
    global rngs
    rngs = dmxrngs

    global tepilepsy
    tepilepsy = Tepilepsy()

    # clear first
    for x in range(tepilepsy.GetWidth()):
        for y in range(tepilepsy.GetHeight()):
            tepilepsy.SetPixel((x, y), (0, 0, 0))

    # open image, resize to fit
    im = Image.open(pic)
    im = im.convert('RGB')
    im.thumbnail((tepilepsy.GetWidth(), tepilepsy.GetHeight()), Image.ANTIALIAS)
    im = im.convert('RGB')
    im.save('/tmp/im.png', 'PNG')

    # set image pixels
    for x in range(im.width):
        for y in range(im.height):
            tepilepsy.SetPixel((x, y), im.getpixel((x, y)))

    global wrapper
    wrapper = ClientWrapper()

    wrapper.AddEvent(22, SendDMXFrame)
    wrapper.Run()
