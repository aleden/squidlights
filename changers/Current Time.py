import array
import itertools
import time
import sys
from PIL import Image
from ola.ClientWrapper import ClientWrapper
from tepilepsy import *

class Pen:
    def __init__(self, tepilepsy, font):
        self.ledwall = tepilepsy
        self.im = Image.open(font)
        self.im = self.im.convert('RGB')
        self.chroffs = dict()
        self.chrlens = dict()

        chrsordered = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':']

        chridx = 0
        off = 0
        for x in range(self.im.width):
            term = all(self.im.getpixel((x, y)) == (0, 0, 0) for y in range(self.im.height))
            if term:
                self.chroffs[chrsordered[chridx]] = off
                self.chrlens[chrsordered[chridx]] = x - off
                chridx += 1
                off = x + 1

        if chridx != len(chrsordered) - 1:
            print("error: could not parse font")
            sys.exit(1)

        self.chroffs[chrsordered[chridx]] = off
        self.chrlens[chrsordered[chridx]] = self.im.width - off

    def drawText(self, s, pt):
        x, y = pt
        for chr in s:
            if chr != ' ':
                self.pasteStrip((self.chroffs[chr], self.chrlens[chr]), (x, y))
                x += self.chrlens[chr]
            else:
                x += 1

    def pasteStrip(self, strip, to):
        off, w = strip
        x_to, y_to = to
        for x in xrange(off, off + w):
            for y in xrange(self.im.height):
                self.ledwall.SetPixel((x_to + x - off, y_to + y), self.im.getpixel((x, y)))

TICK_INTERVAL = 2000 # every two seconds

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    wrapper.AddEvent(TICK_INTERVAL, SendDMXFrame)

    # clear
    for x in range(tepilepsy.GetWidth()):
        for y in range(tepilepsy.GetHeight()):
            tepilepsy.SetPixel((x, y), (0, 0, 0))

    #pen.drawText(time.strftime("%b %d").upper(), (0,0))
    pen.drawText(time.strftime("%I %M %p").upper(), (0, tepilepsy.GetHeight() - pen.im.height))

    for rng,data in itertools.izip(rngs, tepilepsy.DMXData()):
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

def squid(dmxrngs):
    global rngs
    rngs = dmxrngs

    global tepilepsy
    tepilepsy = Tepilepsy()

    global pen
    pen = Pen(tepilepsy, '/home/squidlights/resources/tepilepsy_font.gif')

    global wrapper
    wrapper = ClientWrapper()

    wrapper.AddEvent(22, SendDMXFrame)
    wrapper.Run()
