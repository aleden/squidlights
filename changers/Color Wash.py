import math
import threading

import os, sys
# Hack until I figure out how the hell to actually get this script's directory
sys.path.append("/home/squidlights/changers")

from light_geometries import UnifiedDMXController

# TODO add saturation, brightness sliders or something to frontend?

class UnifiedWashController(UnifiedDMXController):
    def __init__(self, dmxrngs, delta_t):
        super(UnifiedWashController, self).__init__(dmxrngs)
        self.delta_t = delta_t
        self.time_elapsed = 0.0

    def SendDMXFrame(self):
        self.light.setHSV((self.time_elapsed/self.delta_t, 1.0, 1.0))
        #super(UnifiedWashController, self).SendDMXFrame()
        datas = self._getDatas()
        for r in self.dmxrngs:
            self.wrapper.Client().SendDmx(r[0], datas[r], self._DMXSent)

    def SendDMXStream(self, tick_interval):
        self.time_elapsed = math.fmod(self.time_elapsed + tick_interval, self.delta_t)
        #super(UnifiedWashController, self).SendDMXStream(tick_interval)
        self.SendDMXFrame()
        self.timer = threading.Timer((1.0/1000.0)*float(tick_interval), self.SendDMXStream, [tick_interval])
        self.timer.start()


def squid(dmxrngs, delta_t):
    controller = UnifiedWashController(dmxrngs, delta_t)
    controller.SendDMXStream(33.0)
    
