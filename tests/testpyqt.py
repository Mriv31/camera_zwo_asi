
from time import perf_counter

import numpy as np

import pyqtgraph as pg
from pyqtgraph.Qt import QtCore

from camera_zwo_asi import Camera, VideoThread, ImageType


#Camera
camera = Camera(0)
roi = camera.get_roi()
controls = camera.get_controls()
print(controls)
print(roi.type)
roi.type = ImageType.y8

del(camera,roi,controls)

camera = Camera(0)
roi = camera.get_roi()
controls = camera.get_controls()
print(controls)
print(roi.type)
roi.type = ImageType.y8

bins = 0
if (bins):
    roi.height//=2
    roi.width//=2
    roi.bins = 2

camera.configure(roi,controls)
print(controls['Exposure'].min_value)
print(controls['AutoExpMaxExpMS'].value)
print(controls['BandWidth'].is_auto)

camera.set_control("Offset",0)
camera.set_control("BandWidth",'auto')

camera.set_control("Gain",100)
camera.set_control("HighSpeedMode",1)
camera.set_control("Exposure",10000)


print(controls['Exposure'].is_auto)
print(controls['Gain'].is_auto)
print(controls['AutoExpMaxExpMS'].is_auto)
print(controls['BandWidth'].is_auto)

print(controls['AutoExpMaxExpMS'].max_value)
print(controls['Exposure'].max_value)

print(roi.height,roi.width)
#camera.set_control("OverCLK",1)



a = np.array([0],dtype=np.int64)
a.flags.writeable = False


vt = VideoThread(camera)
vt.define_image_counter(a,1)
bf = [np.zeros([roi.height,roi.width],dtype=np.ubyte) for i in range(128)]
for i in range(128):
    vt.addBuffer(bf[i],roi.height*roi.width)
    bf[i].flags.writeable = False
vt.Start()


app = pg.mkQApp("ImageItem Example")

## Create window with GraphicsView widget
win = pg.GraphicsLayoutWidget()
win.show()  ## show widget alone in its own window
win.setWindowTitle('pyqtgraph example: ImageItem')
view = win.addViewBox()

## lock the aspect ratio so pixels are always square
view.setAspectLocked(True)

## Create image item
img = pg.ImageItem(border="w")
view.addItem(img)

## Set initial view bounds
#view.setRange(QtCore.QRectF(0, 0, 600, 600))

## Create random image


timer = QtCore.QTimer()
timer.setSingleShot(True)
# not using QTimer.singleShot() because of persistence on PyQt. see PR #1605

def updateData():
    global img, data, updateTime, elapsed

    ## Display the data
    img.setImage(np.rot90(bf[a[0]%128-1]))
    img.setLevels([0, 255])


    timer.start(100)

    # print(f"{1 / elapsed:.1f} fps")

timer.timeout.connect(updateData)
updateData()

if __name__ == '__main__':
    pg.exec()
