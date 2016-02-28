from __future__ import print_function
import array
import random
import os
import itertools
import sys
from PIL import Image
from PIL import ImageDraw
from ola.ClientWrapper import ClientWrapper
from tepilepsy import *

def perr(*objs):
    print(*objs, file=sys.stderr)

def fpf(f, *objs):
    print(*objs, file=f)

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
    seen = eval(open('/home/squidlights/build/pokeredseen','r').read())
    left = list(set(pokemon) - set(seen))
    if len(left) == 0:
        seen = []
        left = pokemon

    random.seed()
    poke = left[random.randint(0,len(left)-1)]
    seen.append(poke)
    fpf(open('/home/squidlights/build/pokeredseen', 'w'), seen)

    pals = eval(open('/home/squidlights/resources/pokered/pals','r').read())
    pokepal = eval(open('/home/squidlights/resources/pokered/pokepal','r').read())

    palnm = pokepal[poke]
    pal = pals[palnm]

    #perr(pokemon)
    #perr(pal)

    pixrows = eval(open("/home/squidlights/resources/pokered/bmon/%s" % poke,'r').read())
    w = len(pixrows[0])
    h = len(pixrows)

#   for row in pixrows:
#       for i in range(len(row)):
#           row[i] = 3 - row[i]


    perr(poke)
    #perr(palnm)
    #perr(pixrows)

    #pal[0] = (0,0,0)
    #pal[0] = (255 - pal[0][0], 255 - pal[0][1], 255 - pal[0][2])
    #pal[3] = (255 - pal[3][0], 255 - pal[3][1], 255 - pal[3][2])

    x = pal[0]
    y = pal[3]

    pal[0] = (0,0,0)
    pal[3] = x

    im = Image.new("RGB", (w, h))
    for y in range(h):
        for x in range(w):
            im.putpixel((x, y), pal[pixrows[y][x]])

    def inimage(x, y):
        return 0 <= x < im.width and 0 <= y < im.height

    imtmp = im.copy()
    def floodfill(x, y, seedclr, newclr):
        if inimage(x, y):
            tl = (x - 1, y - 1)
            l  = (x - 1, y)
            bl = (x - 1, y + 1)

            t = (x, y - 1)
            m = (x, y)
            b = (x, y + 1)

            tr = (x + 1, y - 1)
            r  = (x + 1, y)
            br = (x + 1, y + 1)
            if (
                   ((not inimage(*tl)) or imtmp.getpixel(tl) == seedclr) and
                   ((not inimage(*l))  or imtmp.getpixel(l)  == seedclr) and
                   ((not inimage(*bl)) or imtmp.getpixel(bl) == seedclr) and
                   ((not inimage(*t))  or imtmp.getpixel(t)  == seedclr) and
                   ((not inimage(*m))  or imtmp.getpixel(m)  == seedclr) and
                   ((not inimage(*b))  or imtmp.getpixel(b)  == seedclr) and
                   ((not inimage(*tr)) or imtmp.getpixel(tr) == seedclr) and
                   ((not inimage(*r))  or imtmp.getpixel(r)  == seedclr) and
                   ((not inimage(*br)) or imtmp.getpixel(br) == seedclr)
               ):
                if (inimage(*tl)):
                    im.putpixel(tl, newclr)
                if (inimage(*l)):
                    im.putpixel(l, newclr)
                if (inimage(*bl)):
                    im.putpixel(bl, newclr)
                if (inimage(*t)):
                    im.putpixel(t, newclr)
                if (inimage(*m)):
                    im.putpixel(m, newclr)
                if (inimage(*b)):
                    im.putpixel(b, newclr)
                if (inimage(*tr)):
                    im.putpixel(tr, newclr)
                if (inimage(*r)):
                    im.putpixel(r, newclr)
                if (inimage(*br)):
                    im.putpixel(br, newclr)

                floodfill(x - 1, y, seedclr, newclr)
                floodfill(x + 1, y, seedclr, newclr)
                floodfill(x, y - 1, seedclr, newclr)
                floodfill(x, y + 1, seedclr, newclr)


    seedclr = (0, 0, 0)
    ImageDraw.floodfill(im, (0,0), seedclr)
    ImageDraw.floodfill(im, (im.width-1,0), seedclr)
    ImageDraw.floodfill(im, (0,im.height-1), seedclr)
    ImageDraw.floodfill(im, (im.width-1,im.height-1), seedclr)

    ##floodfill(0, 0, seedclr, (0, 0, 0))


    # |---
    # |
    # |
    # |
    # |
    #x = 0
    #for y in range(im.height):
        #floodfill(x, y, seedclr, (0, 0, 0))

    # ---|
    #    |
    #    |
    #    |
    #    |
    #x = im.width - 1
    #for y in range(im.height):
        #floodfill(x, y, seedclr, (0, 0, 0))

    # |--------------
    # |
    # |
    #y = 0
    #for x in range(im.width):
        #floodfill(x, y, seedclr, (0, 0, 0))

    # |
    # |
    # |--------------
    #y = im.height - 1
    #for x in range(im.width):
        #floodfill(x, y, seedclr, (0, 0, 0))

    #im.thumbnail((tepilepsy.GetWidth(), tepilepsy.GetHeight()), Image.LANCZOS)

    # set background color
    #for x in range(tepilepsy.GetWidth()):
    #    for y in range(tepilepsy.GetHeight()):
    #        tepilepsy.SetPixel((x, y), pal[0])

    # center vertically?
    yoff = 0
    #if im.height - im.height / 4 >= tepilepsy.GetHeight():
    #    yoff = -im.height / 4

    # for each horizontal row of pixels from the top, add -1 to the yoff
#   allblackrows = [all(elem == pal[0] for elem in l) for l in poke]
#   for y in range(len(allblackrows)):
#       if allblackrows[y] == False:
#           break
#       yoff = -y

    # center horizontally?
    xoff = 0
    if im.width < tepilepsy.GetWidth():
        xoff = (tepilepsy.GetWidth() / 2) - (im.width / 2)

    # center vertically?
    yoff = 0
    for y in xrange(im.height):
        if all(im.getpixel((x, y)) == (0,0,0) for x in xrange(im.width)):
            yoff -= 1
        else:
            break


#   if im.height < tepilepsy.GetHeight():
#       yoff = (tepilepsy.GetHeight() / 2) - (im.height / 2)

#   for x in range(im.width):
#       for y in range(im.height):
#           tepilepsy.SetPixel((x, y), seedclr)

    # set image pixels
    for x in range(im.width):
        for y in range(im.height):
            tepilepsy.SetPixel((x + xoff, y + yoff), im.getpixel((x, y)))

    global wrapper
    wrapper = ClientWrapper()

    wrapper.AddEvent(22, SendDMXFrame)
    wrapper.Run()
