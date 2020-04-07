# Port of Adafruit GFX Arduino library to MicroPython.
# Based on: https://github.com/adafruit/Adafruit-GFX-Library
# Author: Tony DiCola (original GFX author Phil Burgess)
# License: MIT License (https://opensource.org/licenses/MIT)

class GFX:

    def __init__(self, width, height, pixel, hline=None, vline=None):
        # Create an instance of the GFX drawing class.  You must pass in the
        # following parameters:
        #  - width = The width of the drawing area in pixels.
        #  - height = The height of the drawing area in pixels.
        #  - pixel = A function to call when a pixel is drawn on the display.
        #            This function should take at least an x and y position
        #            and then any number of optional color or other parameters.
        #  You can also provide the following optional keyword argument to
        #  improve the performance of drawing:
        #  - hline = A function to quickly draw a horizontal line on the display.
        #            This should take at least an x, y, and width parameter and
        #            any number of optional color or other parameters.
        #  - vline = A function to quickly draw a vertical line on the display.
        #            This should take at least an x, y, and height paraemter and
        #            any number of optional color or other parameters.
        self.width = width
        self.height = height
        self._pixel = pixel
        # Default to slow horizontal & vertical line implementations if no
        # faster versions are provided.
        if hline is None:
            self.hline = self._slow_hline
        else:
            self.hline = hline
        if vline is None:
            self.vline = self._slow_vline
        else:
            self.vline = vline

    def _slow_hline(self, x0, y0, width, *args, **kwargs):
        # Slow implementation of a horizontal line using pixel drawing.
        # This is used as the default horizontal line if no faster override
        # is provided.
        if y0 < 0 or y0 > self.height or x0 < -width or x0 > self.width:
            return
        for i in range(width):
            self._pixel(x0+i, y0, *args, **kwargs)

    def _slow_vline(self, x0, y0, height, *args, **kwargs):
        # Slow implementation of a vertical line using pixel drawing.
        # This is used as the default vertical line if no faster override
        # is provided.
        if y0 < -height or y0 > self.height or x0 < 0 or x0 > self.width:
            return
        for i in range(height):
            self._pixel(x0, y0+i, *args, **kwargs)

    def rect(self, x0, y0, width, height, *args, **kwargs):
        # Rectangle drawing function.  Will draw a single pixel wide rectangle
        # starting in the upper left x0, y0 position and width, height pixels in
        # size.
        if y0 < -height or y0 > self.height or x0 < -width or x0 > self.width:
            return
        self.hline(x0, y0, width, *args, **kwargs)
        self.hline(x0, y0+height-1, width, *args, **kwargs)
        self.vline(x0, y0, height, *args, **kwargs)
        self.vline(x0+width-1, y0, height, *args, **kwargs)

    def fill_rect(self, x0, y0, width, height, *args, **kwargs):
        # Filled rectangle drawing function.  Will draw a single pixel wide
        # rectangle starting in the upper left x0, y0 position and width, height
        # pixels in size.
        if y0 < -height or y0 > self.height or x0 < -width or x0 > self.width:
            return
        for i in range(x0, x0+width):
            self.vline(i, y0, height, *args, **kwargs)

    def line(self, x0, y0, x1, y1, *args, **kwargs):
        # Line drawing function.  Will draw a single pixel wide line starting at
        # x0, y0 and ending at x1, y1.
        steep = abs(y1 - y0) > abs(x1 - x0)
        if steep:
            x0, y0 = y0, x0
            x1, y1 = y1, x1
        if x0 > x1:
            x0, x1 = x1, x0
            y0, y1 = y1, y0
        dx = x1 - x0
        dy = abs(y1 - y0)
        err = dx // 2
        ystep = 0
        if y0 < y1:
            ystep = 1
        else:
            ystep = -1
        while x0 <= x1:
            if steep:
                self._pixel(y0, x0, *args, **kwargs)
            else:
                self._pixel(x0, y0, *args, **kwargs)
            err -= dy
            if err < 0:
                y0 += ystep
                err += dx
            x0 += 1

    def circle(self, x0, y0, radius, *args, **kwargs):
        # Circle drawing function.  Will draw a single pixel wide circle with
        # center at x0, y0 and the specified radius.
        f = 1 - radius
        ddF_x = 1
        ddF_y = -2 * radius
        x = 0
        y = radius
        self._pixel(x0, y0 + radius, *args, **kwargs)
        self._pixel(x0, y0 - radius, *args, **kwargs)
        self._pixel(x0 + radius, y0, *args, **kwargs)
        self._pixel(x0 - radius, y0, *args, **kwargs)
        while x < y:
            if f >= 0:
                y -= 1
                ddF_y += 2
                f += ddF_y
            x += 1
            ddF_x += 2
            f += ddF_x
            self._pixel(x0 + x, y0 + y, *args, **kwargs)
            self._pixel(x0 - x, y0 + y, *args, **kwargs)
            self._pixel(x0 + x, y0 - y, *args, **kwargs)
            self._pixel(x0 - x, y0 - y, *args, **kwargs)
            self._pixel(x0 + y, y0 + x, *args, **kwargs)
            self._pixel(x0 - y, y0 + x, *args, **kwargs)
            self._pixel(x0 + y, y0 - x, *args, **kwargs)
            self._pixel(x0 - y, y0 - x, *args, **kwargs)

    def fill_circle(self, x0, y0, radius, *args, **kwargs):
        # Filled circle drawing function.  Will draw a filled circule with
        # center at x0, y0 and the specified radius.
        self.vline(x0, y0 - radius, 2*radius + 1, *args, **kwargs)
        f = 1 - radius
        ddF_x = 1
        ddF_y = -2 * radius
        x = 0
        y = radius
        while x < y:
            if f >= 0:
                y -= 1
                ddF_y += 2
                f += ddF_y
            x += 1
            ddF_x += 2
            f += ddF_x
            self.vline(x0 + x, y0 - y, 2*y + 1, *args, **kwargs)
            self.vline(x0 + y, y0 - x, 2*x + 1, *args, **kwargs)
            self.vline(x0 - x, y0 - y, 2*y + 1, *args, **kwargs)
            self.vline(x0 - y, y0 - x, 2*x + 1, *args, **kwargs)

    def triangle(self, x0, y0, x1, y1, x2, y2, *args, **kwargs):
        # Triangle drawing function.  Will draw a single pixel wide triangle
        # around the points (x0, y0), (x1, y1), and (x2, y2).
        self.line(x0, y0, x1, y1, *args, **kwargs)
        self.line(x1, y1, x2, y2, *args, **kwargs)
        self.line(x2, y2, x0, y0, *args, **kwargs)

    def fill_triangle(self, x0, y0, x1, y1, x2, y2, *args, **kwargs):
        # Filled triangle drawing function.  Will draw a filled triangle around
        # the points (x0, y0), (x1, y1), and (x2, y2).
        if y0 > y1:
            y0, y1 = y1, y0
            x0, x1 = x1, x0
        if y1 > y2:
            y2, y1 = y1, y2
            x2, x1 = x1, x2
        if y0 > y1:
            y0, y1 = y1, y0
            x0, x1 = x1, x0
        a = 0
        b = 0
        y = 0
        last = 0
        if y0 == y2:
            a = x0
            b = x0
            if x1 < a:
                a = x1
            elif x1 > b:
                b = x1
            if x2 < a:
                a = x2
            elif x2 > b:
                b = x2
            self.hline(a, y0, b-a+1, *args, **kwargs)
            return
        dx01 = x1 - x0
        dy01 = y1 - y0
        dx02 = x2 - x0
        dy02 = y2 - y0
        dx12 = x2 - x1
        dy12 = y2 - y1
        if dy01 == 0:
            dy01 = 1
        if dy02 == 0:
            dy02 = 1
        if dy12 == 0:
            dy12 = 1
        sa = 0
        sb = 0
        if y1 == y2:
            last = y1
        else:
            last = y1-1
        for y in range(y0, last+1):
            a = x0 + sa // dy01
            b = x0 + sb // dy02
            sa += dx01
            sb += dx02
            if a > b:
                a, b = b, a
            self.hline(a, y, b-a+1, *args, **kwargs)
        sa = dx12 * (y - y1)
        sb = dx02 * (y - y0)
        while y <= y2:
            a = x1 + sa // dy12
            b = x0 + sb // dy02
            sa += dx12
            sb += dx02
            if a > b:
                a, b = b, a
            self.hline(a, y, b-a+1, *args, **kwargs)
            y += 1