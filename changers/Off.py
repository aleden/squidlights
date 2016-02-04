import os, sys

# Hack until I figure out how the hell to actually get this script's directory
sys.path.append("/home/squidlights/changers")

from light_geometries import UnifiedDMXController

def squid(dmxrngs):
    controller = UnifiedDMXController(dmxrngs)
    controller.SendDMXFrame()
