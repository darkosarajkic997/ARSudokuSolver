import cv2
import numpy as np

def check_border(image):
    width=image.shape[1]
    height=image.shape[0]
    
    top_border=image[0:5,:]
    bottom_border=image[-5:-1,:]
    left_border=image[:,0:5]
    right_border=image[:,-5:-1]

    top_border_pct_filed=np.count_nonzero(top_border)/(width*5)
    bottom_border_pct_filed=np.count_nonzero(bottom_border)/(width*5)
    left_border_pct_filed=np.count_nonzero(left_border)/(height*5)
    right_border_pct_filed=np.count_nonzero(right_border)/(height*5)

    if(top_border_pct_filed>0.3 and bottom_border_pct_filed>0.3 and left_border_pct_filed>0.3 and right_border_pct_filed>0.3):
        return True
    else:
        return False


IMAGE="F:\\ML Projects\\CUBIC Praksa\\ARSudokuSolver\\Data\warped\\img_warped2.jpg"
if __name__ == "__main__":
    image = cv2.imread(IMAGE)
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    vertical_kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (1,40))
    img_v = cv2.morphologyEx(gray, cv2.MORPH_OPEN, vertical_kernel, iterations=1)
    horizontal_kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (40,1))
    img_h = cv2.morphologyEx(gray, cv2.MORPH_OPEN, horizontal_kernel, iterations=1)

    img=np.bitwise_or(img_h,img_v)

    check_border(img)

    cv2.imshow('thresh', img)
    cv2.imshow('image', image)

    cv2.waitKey(0)