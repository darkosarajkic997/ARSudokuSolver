import ctypes
import cv2
import numpy as np
import configparser


def mask_image_C(image,image_function,image_data_type=ctypes.c_uint8):
    image_function.argtypes=[np.ctypeslib.ndpointer(image_data_type),ctypes.c_int32,ctypes.c_int32]

    return image_function(image,image.shape[0],image.shape[1])




if __name__ =="__main__":    
    config = configparser.ConfigParser()
    config.read('config.ini')

    testLib=ctypes.CDLL(config['Resources']['dll'])
    image = cv2.imread(config['Resources']['image'])
    #print(image.size)

    mask_image_C(image,testLib.doPicture)

    cv2.imshow('image',image)
    cv2.waitKey(0)
