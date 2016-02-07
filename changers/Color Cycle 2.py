import array
import colorsys
import operator
from ola.ClientWrapper import ClientWrapper

TICK_INTERVAL = 33  # in ms - so display at approximately 30 fps

def DmxSent(state):
    if not state.Succeeded():
        sys.exit(1)

def SendDMXFrame():
    global forward
    global t_idx
    global frames

    wrapper.AddEvent(TICK_INTERVAL, SendDMXFrame)

    for j in range(0, len(rngs)):
        wrapper.Client().SendDmx(rngs[j][0], frames[t_idx][j], DmxSent)

    if t_idx == len(frames)-1:
        forward = False
    elif t_idx == 0:
        forward = True

    if forward:
        t_idx += 1
    else:
        t_idx -= 1

def squid(dmxrngs, begclr, endclr, period):
    global frames
    frames = []

    #
    # precompute dmx packets for every frame
    #
    beg_hsv = colorsys.rgb_to_hsv(*tuple(map(operator.div, map(float, begclr), [255.0]*3)))
    end_hsv = colorsys.rgb_to_hsv(*tuple(map(operator.div, map(float, endclr), [255.0]*3)))
    for i in (range(0, period, TICK_INTERVAL) + [period]):
        progress = float(i) / float(period)

        hsv_delta = tuple(map(operator.sub, end_hsv, beg_hsv))
        hsv_addend = tuple(map(operator.mul, tuple([progress]*3), hsv_delta))

        hsv = tuple(map(operator.add, beg_hsv, hsv_addend))
        clr = tuple(map(int, map(operator.mul, colorsys.hsv_to_rgb(*hsv), tuple([255.0]*3))))

        frame = []
        for j in range(0, len(dmxrngs)):
            dmxpacket = array.array('B', [0] * dmxrngs[j][1])
            for chann in range(dmxrngs[j][1], dmxrngs[j][2] - 2, 3):
                dmxpacket.append(clr[0])
                dmxpacket.append(clr[1])
                dmxpacket.append(clr[2])
            frame.append(dmxpacket)
        frames.append(frame)

    global rngs
    rngs = dmxrngs

    global forward
    forward = True

    global t_idx
    t_idx = 0

    global wrapper
    wrapper = ClientWrapper()

    wrapper.AddEvent(22, SendDMXFrame)
    wrapper.Run()
