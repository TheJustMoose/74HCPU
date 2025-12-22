# This code require opencv to work
# To install type:
# pip install opencv-python

import cv2
import numpy as np

width = 480
height = 272
size = width * height

image = []

def rgb(clr):
  r = clr & 0x1F
  clr = clr >> 5
  g = clr & 0x1F
  clr = clr >> 5
  b = clr & 0x1F
  return (r<<3, g<<3, b<<3)

def read_image():
  dtype_string = '<u2'
  values_array = np.fromfile("dump.bin", dtype=np.dtype(dtype_string))
  if len(values_array) != width*height:
    print("Image size should be 480*272 but it's", len(values_array))
  else:
    print("Read ok")

  image_data = bytearray(width*height*3)
  for y in range(0, height):
    for x in range(0, width):
      t = rgb(values_array[y*width + x])      # array of 16 bit values
      image_data[(y*width + x)*3 + 2] = t[2]  # red
      image_data[(y*width + x)*3 + 1] = t[1]  # green
      image_data[(y*width + x)*3] = t[0]      # blue

  return image_data

def update_loop():
  global image

  image_data = read_image()
  image = np.frombuffer(image_data, dtype=np.uint8).reshape((height, width, 3))

  #self.after(100, update_loop)

def main():
  window_name = 'TFT LCD'
  cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)

  update_loop()

  cv2.imshow(window_name, image)
  cv2.waitKey(0)
  cv2.destroyAllWindows()

main()
