import array
import itertools
import random
from PIL import Image
from ola.ClientWrapper import ClientWrapper
from tepilepsy import *

TICK_INTERVAL = 1000 # set every second

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    wrapper.AddEvent(TICK_INTERVAL, SendDMXFrame)

    for rng,data in itertools.izip(rngs, tepilepsy.DMXData()):
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

#Assumes the image is composed of 96 width, 103 height pokemon sprites, 8 sprites across.
def get_ith_sprite(img, index):
    left_coord = (index % 8) * 96
    top_coord = (index / 8) * 103
    #23 is just cropping to tepilepsy wideformat aspect ratio
    return img.crop((left_coord, top_coord + 23, left_coord + 96, top_coord + 103 - 23))

def get_pokemon(idx):
    if idx < 0:
        print("ERROR")
    elif idx < 173:
        global gen_1
        index_within_gen = idx
        return get_ith_sprite(gen_1, index_within_gen)
    elif idx < 322:
        global gen_2
        index_within_gen = idx - 173
        return get_ith_sprite(gen_2, index_within_gen)
    elif idx < 481 :
        global gen_3
        index_within_gen = idx - 322
        return get_ith_sprite(gen_3, index_within_gen)
    elif idx < 649:
        global gen_4
        index_within_gen = idx - 481    
        return get_ith_sprite(gen_4, index_within_gen)
    elif idx < 826:
        global gen_5
        index_within_gen = idx - 649
        return get_ith_sprite(gen_5, index_within_gen)
    else:
        print("TOO HIGH")

def squid(dmxrngs):
    global rngs
    rngs = dmxrngs

    global tepilepsy
    tepilepsy = Tepilepsy()

    global gen_1
    gen_1 = Image.open('/home/squidlights/resources/gen1.png')
    global gen_2
    gen_2 = Image.open('/home/squidlights/resources/gen2.png')
    global gen_3
    gen_3 = Image.open('/home/squidlights/resources/gen3.png')
    global gen_4
    gen_4 = Image.open('/home/squidlights/resources/gen4.png')
    global gen_5
    gen_5 = Image.open('/home/squidlights/resources/gen5.png')

    # clear first
    for x in range(tepilepsy.GetWidth()):
        for y in range(tepilepsy.GetHeight()):
            tepilepsy.SetPixel((x, y), (0, 0, 0))

    # decide on pokemon index
    idx = random.randint(0, 825)
    # open image, resize to fit
    im = get_pokemon(idx)
    im = im.convert('RGB')
    im.thumbnail((tepilepsy.GetWidth(), tepilepsy.GetHeight()), Image.NEAREST)
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
