import array
import random
import os
import itertools
from PIL import Image
from ola.ClientWrapper import ClientWrapper
from tepilepsy import *

TICK_INTERVAL = 1000 # every second

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    wrapper.AddEvent(TICK_INTERVAL, SendDMXFrame)

    for rng,data in itertools.izip(rngs, tepilepsy.DMXData()):
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

def squid(dmxrngs):
    global rngs
    rngs = dmxrngs

    global tepilepsy
    tepilepsy = Tepilepsy()

    pokemon = os.listdir('/home/squidlights/resources/pokered/bmon')
    pals = eval(open('/home/squidlights/resources/pokered/pals','r').read())
    pokepal = eval(open('/home/squidlights/resources/pokered/pokepal','r').read())

    random.seed()
    poke = pokemon[random.randint(0,len(pokemon)-1)]
    palnm = pokepal[poke]
    pal = pals[palnm]

    print(poke)
    print(palnm)
    print(pal)

    pixrows = eval(open("/home/squidlights/resources/pokered/bmon/%s" % poke,'r').read())
    w = len(pixrows[0])
    h = len(pixrows)

    #pal[0] = (0,0,0)

    im = Image.new("RGB", (w, h))
    for y in range(h):
        for x in range(w):
            im.putpixel((x, y), pal[pixrows[y][x]])
    #im.thumbnail((tepilepsy.GetWidth(), tepilepsy.GetHeight()), Image.LANCZOS)

    # set background color
    for x in range(tepilepsy.GetWidth()):
        for y in range(tepilepsy.GetHeight()):
            tepilepsy.SetPixel((x, y), pal[0])

    # center vertically?
    yoff = 0
    if im.height - im.height / 4 >= tepilepsy.GetHeight():
        yoff = -im.height / 4
    # set image pixels
    for x in range(im.width):
        for y in range(im.height):
            tepilepsy.SetPixel((x, y + yoff), im.getpixel((x, y)))

    global wrapper
    wrapper = ClientWrapper()

    wrapper.AddEvent(22, SendDMXFrame)
    wrapper.Run()
