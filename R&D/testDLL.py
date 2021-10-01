import sys
sys.path.append(".")

from CppDllWrap import boardFinder
import cv2
import numpy as np
import configparser
import math

ROLLING_RANGE=10
PEAK_WIDTH=30

def draw_lines_on_image_and_display(lines,number_of_lines,image,color_RGB=255, display=True):
    for index in range(0,number_of_lines):
        rho=lines[2*index]
        theta=(lines[2*index+1]*math.pi/180)
        a=math.cos(theta)
        b=math.sin(theta)
        x0 = a * rho
        y0 = b * rho
        x1 = int(x0 - 1500 * b)
        y1 = int(y0 + 1500 * a)
        x2 = int(x0 + 1500 * b)
        y2 = int(y0 - 1500 * a)
        cv2.line(image,(x1,y1),(x2,y2),color_RGB,2)

    if display:
        cv2.imshow('Image',image)
        cv2.waitKey(0)
        cv2.destroyAllWindows()


if __name__ =="__main__":
    config = configparser.ConfigParser()
    config.read('config.ini')    
    image = cv2.imread(config['Resources']['image_lines'],cv2.IMREAD_GRAYSCALE)
    image=cv2.Canny(image,140,200)
    lines=np.zeros(boardFinder.MAX_LINES,dtype=np.int)
    voters_threshold=0.5
    number_of_lines=boardFinder.hough_line_detector(image,image.shape[1],image.shape[0],lines,voters_threshold,boardFinder.MAX_LINES)

    horizontal_lines=np.zeros(2*number_of_lines,dtype=np.int)
    vertical_lines=np.zeros(2*number_of_lines,dtype=np.int)

    number_of_horizontal_lines,number_of_vertical_lines=boardFinder.find_two_biggest_clusters_of_lines(lines,number_of_lines,ROLLING_RANGE,PEAK_WIDTH,boardFinder.MAX_LINES,horizontal_lines,vertical_lines)

    horizontal_clusters=np.zeros(2*number_of_horizontal_lines,dtype=np.int)
    vertical_clusters=np.zeros(2*number_of_vertical_lines,dtype=np.int)

    number_of_horizontal_clusters=boardFinder.lines_DBSCAN(horizontal_lines,number_of_horizontal_lines,horizontal_clusters)
    number_of_vertical_clusters=boardFinder.lines_DBSCAN(vertical_lines,number_of_vertical_lines,vertical_clusters)

    mean_horizontal_lines=np.zeros(2*number_of_horizontal_clusters,dtype=np.single)
    mean_vertical_lines=np.zeros(2*number_of_vertical_clusters,dtype=np.single)

    boardFinder.find_average_for_clustered_lines(horizontal_lines,number_of_horizontal_lines,horizontal_clusters,number_of_horizontal_clusters,mean_horizontal_lines)
    boardFinder.find_average_for_clustered_lines(vertical_lines,number_of_vertical_lines,vertical_clusters,number_of_vertical_clusters,mean_vertical_lines)

    draw_lines_on_image_and_display(mean_horizontal_lines,number_of_horizontal_clusters,image,150,False)
    draw_lines_on_image_and_display(mean_vertical_lines,number_of_vertical_clusters,image,150)
