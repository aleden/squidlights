import array
import numpy as np
import math
from ola.ClientWrapper import ClientWrapper

# globals for timing, script state storage
TICK_INTERVAL = 33  # in ms - so display at approximately 30 fps
TIME_ELAPSED = 0.0  # time in ms since we last displaed begclr
                    # when TIME_ELAPSED is first >= delta_t/2.0,
                    # we display endclr (and begin to cycle back)

def DmxSent(state):
    if not state.Succeeded():
        wrapper.Stop()

def SendDMXFrame():
    # schedule a function call in the future
    # we do this first in case the frame computation takes a long time.
    wrapper.AddEvent(TICK_INTERVAL, SendDMXFrame)

    # calculate values to display

    # figure out weighting for each color during this frame.
    # weighting elements should sum to 1.0 in this script 
    # (if they didn't, color intensity as well as hue would change with time)
    if TIME_ELAPSED < half_width:
        weighting = np.array([(half_width-TIME_ELAPSED)/half_width, TIME_ELAPSED/half_width])
    else:
        since_switch = TIME_ELAPSED - half_width
        weighting = np.array([since_switch/half_width, (half_width-since_switch)/half_width])

    # calculate color to display
    clr = weighting.dot(colors)

    datas = []

    # prepare DMX512 serial data
    for rng in rngs:
        data = array.array('B', [0] * 512)
        for chann in xrange(rng[1], rng[2] - 3, 3):
            data[chann+0] = int(clr[0])
            data[chann+1] = int(clr[1])
            data[chann+2] = int(clr[2])
        datas.append(data)
    
    # display frame
    for rng, data in zip(rngs, datas):
        wrapper.Client().SendDmx(rng[0], data, DmxSent)

    # store relevant state for next frame calculation
    global TIME_ELAPSED
    TIME_ELAPSED = math.fmod((TIME_ELAPSED + TICK_INTERVAL), period)

def squid(dmxrngs, begclr, endclr, delta_t):
    # arrange beginning and ending RGB colors into a matrix
    # which looks like [begclr,
    #                   endclr].
    global colors
    colors = np.vstack((begclr, endclr))

    global period
    period = delta_t

    global half_width
    half_width = period/2.0

    global rngs
    rngs = dmxrngs

    global wrapper
    wrapper = ClientWrapper()
    SendDMXFrame()
    # initiate main loop
    wrapper.Run()
