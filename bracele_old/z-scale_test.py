#http://codingpy.com/article/a-quick-intro-to-matplotlib/

import matplotlib.pyplot as plt
import numpy as np

#org
x0 = np.linspace(0, 2*np.pi, 50)
y0 = np.sin(x0)
mean_x0 = np.mean(x0)
std_x0 = np.std(x0)
mean_y0 = np.mean(y0)
std_y0 = np.std(y0)
x00 = (x0-mean_x0)/std_x0
y00 = (y0-mean_y0)/std_y0

#translation
x1 = np.linspace(0.5, 2*np.pi + 0.5, 50)
y1 = np.sin(x1 - 0.5) + 2
mean_x1 = np.mean(x1)
std_x1 = np.std(x1)
mean_y1 = np.mean(y1)
std_y1 = np.std(y1)
x11 = (x1-mean_x1)/std_x1
y11 = (y1-mean_y1)/std_y1

#zoom
x2 = np.linspace(0, 4*np.pi, 50)
y2 = 2 * np.sin(x2/2)
mean_x2 = np.mean(x2)
std_x2 = np.std(x2)
mean_y2 = np.mean(y2)
std_y2 = np.std(y2)
x22 = (x2-mean_x2)/std_x2
y22 = (y2-mean_y2)/std_y2

#rotation
x3 = np.linspace(0, 2*np.pi, 50)
y3 = np.sin(x3)
x3 = x3*np.cos(np.pi/8) + y3*np.sin(np.pi/8)
y3 = -x3*np.sin(np.pi/8) + y3*np.cos(np.pi/8)
mean_x3 = np.mean(x3)
std_x3 = np.std(x3)
mean_y3 = np.mean(y3)
std_y3 = np.std(y3)
x33 = (x3-mean_x3)/std_x3
y33 = (y3-mean_y3)/std_y3

plt.subplot(2, 4, 1)
plt.title('org-zscale')
(l1, l2) = plt.plot(x0, y0, 'g', x00, y00, 'r')
plt.legend(handles=[l1, l2], labels=['org', 'zscaled'], loc='best')

plt.subplot(2, 4, 2)
plt.title('translation-zscale')
(l1, l2) = plt.plot(x1, y1, 'g', x11, y11, 'r')
plt.legend(handles=[l1, l2], labels=['translation', 'zscaled'], loc='best')

plt.subplot(2, 4, 3)
plt.title('zoom-zscale')
(l1, l2) = plt.plot(x2, y2, 'g', x22, y22, 'r')
plt.legend(handles=[l1, l2], labels=['zoom', 'zscaled'], loc='best')

plt.subplot(2, 4, 4)
plt.title('rotation-zscale')
(l1, l2) = plt.plot(x3, y3, 'g', x33, y33, 'r')
plt.legend(handles=[l1, l2], labels=['rotation', 'zscaled'], loc='best')

plt.subplot(2, 4, 5)
plt.title('org-translation(zscale)')
plt.plot(x00, y00, 'g', x11, y11, 'r')

plt.subplot(2, 4, 6)
plt.title('org-zoom(zscale)')
plt.plot(x00, y00, 'g', x22, y22, 'r')

plt.subplot(2, 4, 7)
plt.title('org-rotation(zscale)')
plt.plot(x00, y00, 'g', x33, y33, 'r')

plt.subplot(2, 4, 8)
plt.title('all')
plt.plot(x0, y0, 'g', x1, y1, 'r', x2, y2, 'b', x3, y3, 'y')
plt.show()
