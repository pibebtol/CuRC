import cv2
import numpy as np
import sys

img = cv2.imread(sys.argv[1])
m = (0,0,0) 
s = (0.5,0.5,0.5)
noise = img.copy()
cv2.randn(noise,m,s);
cv2.imwrite(sys.argv[2],img+noise)