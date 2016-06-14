import matplotlib.pyplot as plt
import numpy as np
import matplotlib as ml
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
from scipy.misc import imresize
import time


while (True):
    f= open("map.txt",'r')
    b = np.zeros((10,10))
    i=0
    for line in f:
        a = line.split(" ")[0:10]
        b[i] = a
        i=i+1
    print b
    #b = scipy.misc.imresize(b,(100,100),interp='nearest',mode=None)
    b = imresize(b,(100,100),interp = 'nearest')
    print b
    f.close

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    x=range(100)
    y=range(100)
    X, Y = np.meshgrid(x,y)
    Z =b
    ax.plot_surface(X, Y, Z, rstride=1, cstride=1,cmap=cm.coolwarm,
                       linewidth=0, antialiased=False)
    plt.show()
    plt.ion()
    time.sleep(2)
    plt.close(fig)
    time.sleep(2)
