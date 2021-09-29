import sys
sys.path.append(".")

import cv2
import numpy as np
import configparser
import time
import matplotlib.pyplot as plt
from math import pi

def find_sum_from_range(angle_range_low, angle_range_high, histo):
    sum=0
    for angle in range(angle_range_low,angle_range_high):
        sum+=histo[angle]

    return sum


if __name__=='__main__':
    filter = False
    config=configparser.ConfigParser()
    config.read('config.ini')

    img = cv2.imread(config['Resources']['image_lines'],cv2.IMREAD_GRAYSCALE) 
    start=time.time()
    gray = img #cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    # dst = cv2.cornerHarris(gray,7,7,0.1)
    # dst = cv2.dilate(dst,None)
    gray=cv2.GaussianBlur(gray, (7, 7), 3)
    edges = cv2.Canny(gray,90,150,apertureSize = 5)
    kernel = np.ones((3,3),np.uint8)
    edges = cv2.dilate(edges,kernel,iterations = 1)
    kernel = np.ones((3,3),np.uint8)
    edges = cv2.erode(edges,kernel,iterations = 1)
    cv2.imshow('cannyjpg',edges)

    lines = cv2.HoughLines(edges,1,np.pi/90,150)


    if not lines.any():
        print('No lines were found')
        exit()

    if filter:
        rho_threshold = 10
        theta_threshold = 0.1

        # how many lines are similar to a given one
        similar_lines = {i : [] for i in range(len(lines))}
        for i in range(len(lines)):
            for j in range(len(lines)):
                if i == j:
                    continue

                rho_i,theta_i = lines[i][0]
                rho_j,theta_j = lines[j][0]
                if abs(rho_i - rho_j) < rho_threshold and abs(theta_i - theta_j) < theta_threshold:
                    similar_lines[i].append(j)

        # ordering the INDECES of the lines by how many are similar to them
        indices = [i for i in range(len(lines))]
        indices.sort(key=lambda x : len(similar_lines[x]))

        # line flags is the base for the filtering
        line_flags = len(lines)*[True]
        for i in range(len(lines) - 1):
            if not line_flags[indices[i]]: # if we already disregarded the ith element in the ordered list then we don't care (we will not delete anything based on it and we will never reconsider using this line again)
                continue

            for j in range(i + 1, len(lines)): # we are only considering those elements that had less similar line
                if not line_flags[indices[j]]: # and only if we have not disregarded them already
                    continue

                rho_i,theta_i = lines[indices[i]][0]
                rho_j,theta_j = lines[indices[j]][0]
                if abs(rho_i - rho_j) < rho_threshold and abs(theta_i - theta_j) < theta_threshold:
                    line_flags[indices[j]] = False # if it is similar and have not been disregarded yet then drop it now

    print('number of Hough lines:', len(lines))

    filtered_lines = []

    if filter:
        for i in range(len(lines)): # filtering
            if line_flags[i]:
                filtered_lines.append(lines[i])

        print('Number of filtered lines:', len(filtered_lines))
    else:
        filtered_lines = lines

    angle=180
    theta_histo=np.zeros((angle))
    for line in filtered_lines:
        rho,theta = line[0]
        #print(rho,theta)
        a = np.cos(theta)
        b = np.sin(theta)
        x0 = a*rho
        y0 = b*rho
        x1 = int(x0 + 1500*(-b))
        y1 = int(y0 + 1500*(a))
        x2 = int(x0 - 1500*(-b))
        y2 = int(y0 - 1500*(a))

        deg_theta=int(theta*(180/pi))%angle
        # if(deg_theta>=90):
        #     deg_theta=90-(deg_theta%90)
        theta_histo[deg_theta]+=1
        print(deg_theta , theta, rho)
        if(deg_theta<75 or deg_theta>140):
            cv2.line(img,(x1,y1),(x2,y2),(0,0,255),2)
        else:
            cv2.line(img,(x1,y1),(x2,y2),(255,255,0),2)
    # for line in lines:
    #     cv2.line(img,(line[0][0],line[0][1]),(line[0][2],line[0][3]),(0,0,255),2)
    #print(time.time()-start)
    
    
    # img[dst>0.01*dst.max()]=[255,0,0]

    cv2.imshow('houghjpg',img)
    

    rolling_histo_max=np.zeros((180))
    for index in range(5,176):
        rolling_histo_max[index]=find_sum_from_range(index-5,index+5,theta_histo)

    # plt.bar(np.arange(len(theta_histo)),theta_histo)
    # plt.show()

    plt.bar(np.arange(len(rolling_histo_max)),rolling_histo_max)
    plt.show()


    cv2.waitKey(0)