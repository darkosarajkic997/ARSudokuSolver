import ctypes
import cv2
import numpy as np

IMAGE="F:\\ML Projects\\CUBIC Praksa\\SudokuSolver\\Data\\Images\\sudoku_from_video.jpg"
testLib=ctypes.CDLL("F:\\ML Projects\\CUBIC Praksa\\SudokuSolver\\DLLs\\testcdll\\x64\\Debug\\testcdll.dll")

def mask_image_C(image,image_data_type=ctypes.c_uint8):
    image_function=testLib.doPicture
    image_function.argtypes=[np.ctypeslib.ndpointer(image_data_type),ctypes.c_int32,ctypes.c_int32]

    return image_function(image,image.shape[0],image.shape[1])




if __name__ =="__main__":    
    image = cv2.imread(IMAGE)
    #print(image.size)

    mask_image_C(image)

    cv2.imshow('image',image)
    cv2.waitKey(0)
