import array

# LED wall, 60x36 pixels
class Tepilepsy:
    # composed of 5x3 panels
    panelColumns = 5
    panelRows = 3

    # each panel is composed of two vertically split 'half panels' which have
    # their own DMX address spaces, each 6x36 pixels
    halfPanelWidth = 6
    halfPanelHeight = 12

    def __init__(self):
        numPanels = self.panelColumns * self.panelRows
        numHalfPanels = numPanels * 2

        # initialize DMX channel values for each half panel
        halfPanelPixels = self.halfPanelWidth * self.halfPanelHeight
        halfPanelChannels = halfPanelPixels * 3
        self.halfPanelsDmx = [array.array('B', [0] * halfPanelChannels) for i in range(numHalfPanels)]

        #
        # create lookup table to decompose a given half panel by row and column
        # into its corresponding index in the list of DMX ranges. the
        # arrangement looks like this, visually facing the LED wall:
        #
        # 0  1  2  3  4  5  6  7  8  9
        # 10 11 12 13 14 15 16 17 18 19
        # 20 21 22 23 24 25 26 27 28 29
        #
        halfPanelColumns = 2 * self.panelColumns
        halfPanelRows = self.panelRows
        halfPanelMap = [[0 for r in range(halfPanelRows)] for c in range(halfPanelColumns)]
        idx = 0
        for r in range(halfPanelRows):
            for c in range(halfPanelColumns):
                halfPanelMap[c][r] = idx
                idx += 1
        #
        # create lookup tables to decompose x, y offsets in the pixel coordinate
        # space to DMX channel offsets.
        # 
        # for left panels, it goes like this (pixels):
        #
        #  ---------------|
        #    35   23   11 |
        #                 |
        #   ...  ...  ... |
        #                 |
        #    27   15    3 |
        #    26   14    2 |
        #    25   13    1 |
        #    24   12    0 |
        #  ---------------|
        #
        # for right panels, it goes like this (pixels):
        #
        # |---------------
        # | 11   23   35
        # |
        # | ...  ...  ...
        # |
        # | 3    15   27
        # | 2    14   26
        # | 1    13   25
        # | 0    12   24
        # |---------------
        #
        leftHalfPanelMap = [[0 for y in range(self.halfPanelHeight)] for x in range(self.halfPanelWidth)]
        rightHalfPanelMap = [[0 for y in range(self.halfPanelHeight)] for x in range(self.halfPanelWidth)]

        idx = 0
        for x in reversed(range(self.halfPanelWidth)):
            for y in reversed(range(self.halfPanelHeight)):
                leftHalfPanelMap[x][y] = 3*idx
                idx += 1

        idx = 0
        for x in range(self.halfPanelWidth):
            for y in reversed(range(self.halfPanelHeight)):
                rightHalfPanelMap[x][y] = 3*idx
                idx += 1

        # create lookup table to decompose a given pixel coordinate to a half-
        # panel index and DMX channel within that half-panel's DMX address space
        self.pixelMap = [[(0,0) for y in range(self.GetHeight())] for x in range(self.GetWidth())]
        for x in range(self.GetWidth()):
            for y in range(self.GetHeight()):
                halfPanelColumn = x / self.halfPanelWidth
                halfPanelRow = y / self.halfPanelHeight
                halfPanelIdx = halfPanelMap[halfPanelColumn][halfPanelRow]
                xOff = x % self.halfPanelWidth
                yOff = y % self.halfPanelHeight
                isLeft = halfPanelIdx % 2 == 0
                if isLeft:
                    self.pixelMap[x][y] = (halfPanelIdx, leftHalfPanelMap[xOff][yOff])
                else:
                    self.pixelMap[x][y] = (halfPanelIdx, rightHalfPanelMap[xOff][yOff])

    def GetWidth(self):
        panelWidth = 2 * self.halfPanelWidth
        return panelWidth * self.panelColumns # 60 pixels

    def GetHeight(self):
        panelHeight = self.halfPanelHeight
        return self.halfPanelHeight * self.panelRows # 36 pixels

    def SetPixel(self, point, color):
        x, y = point

        if x >= self.GetWidth():
            return
        if y >= self.GetHeight():
            return

        panelIdx, chann = self.pixelMap[x][y]
        self.halfPanelsDmx[panelIdx][chann+0] = color[0]
        self.halfPanelsDmx[panelIdx][chann+1] = color[1]
        self.halfPanelsDmx[panelIdx][chann+2] = color[2]

    def GetPixel(self, point, color):
        x, y = point

        if x >= self.GetWidth():
            return
        if y >= self.GetHeight():
            return

        panelIdx, chann = self.pixelMap[x][y]
        r = self.halfPanelsDmx[panelIdx][chann+0]
        g = self.halfPanelsDmx[panelIdx][chann+1]
        b = self.halfPanelsDmx[panelIdx][chann+2]

        return (r, g, b)

    def DMXData(self):
        return self.halfPanelsDmx
