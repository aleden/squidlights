import array
import math
import itertools
import colorsys
import operator
from ola.ClientWrapper import ClientWrapper
from tepilepsy import *

class Shimmering:
    width = 1.01
    hue = 0.0
    shimmer_angle = 0.0
    center_x=0.0
    center_y=0.0
    dx=0.001
    dy=0.001

    def __init__(self, tepilepsy, value):
        self.ledwall = tepilepsy
        self.v = value

    def draw(self):
        self.hue+=2/255.0;
        self.shimmer_angle += 0.05
        self.center_x+=self.dx*math.cos(self.shimmer_angle/100)
        self.center_y+=self.dy*math.sin(self.shimmer_angle/100)
        if self.center_x>self.width :
            self.dx = -self.dx
            self.center_x=1.0
        elif self.center_x < -1 :
            self.dx = -self.dx
            self.center_x=-1
        if self.center_y>1 :
            self.dy = -self.dy
            self.center_y=1
        elif self.center_y < -1 :
            self.dy=-self.dy
            self.center_y=-1

        shimmer_angle = self.shimmer_angle
        alpha, beta, gamma = math.cos(shimmer_angle / 10), math.cos(shimmer_angle), math.sin(shimmer_angle)
        width, center_x, center_y = self.width, self.center_x, self.center_y
        hue = self.hue
        pheight, pwidth = self.ledwall.GetHeight(), self.ledwall.GetWidth()
        for y in range(self.ledwall.GetHeight()):
            for x in range(self.ledwall.GetWidth()):
                cur_x = (float(x)/pwidth-0.5)*(width * 2)-center_x
                cur_y = (float(y)/pheight-0.5)*(width * 2)-center_y
                dist = alpha * cur_x * cur_y \
                     + beta * cur_x**2 \
                     + gamma * cur_y**2

                #rgbfl = colorsys.hsv_to_rgb(max(0.0, min(1.0, hue + dist / 2.5)), 1.0, 1.0)
                rgbfl = colorsys.hsv_to_rgb(abs(math.fmod(hue + dist / 2.5, 1.0)), 1.0, self.v)
                rgb = tuple(map(int, map(operator.mul, rgbfl, tuple([255.0]*3))))
                self.ledwall.SetPixel((x, y), rgb)

TICK_INTERVAL = 99

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    wrapper.AddEvent(TICK_INTERVAL, SendDMXFrame)

    shimmering.draw()

    for rng,data in itertools.izip(rngs, tepilepsy.DMXData()):
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

def squid(dmxrngs, value):
    global rngs
    rngs = dmxrngs

    global tepilepsy
    tepilepsy = Tepilepsy()

    v = float(value) / 255.0
    global shimmering
    shimmering = Shimmering(tepilepsy, v)

    global wrapper
    wrapper = ClientWrapper()

    wrapper.AddEvent(22, SendDMXFrame)
    wrapper.Run()
