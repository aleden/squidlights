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
    # TODO as fuck


def squid(dmxrngs, delta_t):
    # arrange beginning and ending RGB colors into a matrix
    # which looks like [begclr,
    #                   endclr].
    global period
    period = delta_t

    global rngs
    rngs = dmxrngs

    global wrapper
    wrapper = ClientWrapper()
    SendDMXFrame()
    # initiate main loop
    wrapper.Run()
