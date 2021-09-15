import sys
sys.path.append(".")
import cv2
import numpy as np
import matplotlib.pyplot as plt

if __name__=="__main__":
    model_img = "F:\\ML Projects\\CUBIC Praksa\\SudokuSolver\\Data\\Images\\warped_wo_numbers.jpg"
    search_img="F:\\ML Projects\\CUBIC Praksa\\SudokuSolver\\Data\\Images\\sudoku.jpg"
    input_img="F:\\ML Projects\\CUBIC Praksa\\SudokuSolver\\Data\\Images\\sudoku_lines_3.jpg"

    ori = cv2.imread(input_img)
    image = cv2.imread(input_img)
    gray = cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)
    gray = np.float32(gray)
    gray=cv2.GaussianBlur(gray, (5, 5), 3)
    dst = cv2.cornerHarris(gray,7,7,0.1)
    dst = cv2.dilate(dst,None)
    image[dst>0.01*dst.max()]=[0,0,255]
    cv2.imshow('Original',ori) 
    cv2.imshow('Harris',image)
    if cv2.waitKey(0) & 0xff == 27:
        cv2.destroyAllWindows()

    # img = cv2.imread(input_img)
    # ori = cv2.imread(input_img)
    # gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    # gray=cv2.GaussianBlur(gray, (5, 5), 3)
    # corners = cv2.goodFeaturesToTrack(gray,40,0.1,20)
    # corners = np.int0(corners) 
    # for i in corners:
    #     x,y = i.ravel()
    #     cv2.circle(img,(x,y),3,255,-1) 
    # cv2.imshow('Original', ori)
    # cv2.imshow('Shi-Tomasi', img)
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()

    # ori = cv2.imread(input_img)
    # img = cv2.imread(input_img)
    # gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    # sift = cv2.SIFT_create()
    # kp, des = sift.detectAndCompute(gray,None)
    # img=cv2.drawKeypoints(gray,kp,img,flags=cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)
    # cv2.imshow('Original',ori) 
    # cv2.imshow('SIFT',img)
    # if cv2.waitKey(0) & 0xff == 27:
    #     cv2.destroyAllWindows()

    # ori = cv2.imread(input_img)
    # im = cv2.imread(input_img, cv2.IMREAD_GRAYSCALE)
    # detector = cv2.SimpleBlobDetector_create()
    # keypoints = detector.detect(im)
    # im_with_keypoints = cv2.drawKeypoints(im, keypoints, np.array([]), (0,0,255), cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)
    # cv2.imshow('Original',ori) 
    # cv2.imshow('BLOB',im_with_keypoints)
    # if cv2.waitKey(0) & 0xff == 27:
    #     cv2.destroyAllWindows()

    # img1 = cv2.imread(model_img,cv2.IMREAD_GRAYSCALE)          # queryImage
    # img2 = cv2.imread(search_img,cv2.IMREAD_GRAYSCALE) # trainImage
    # # Initiate SIFT detector
    # sift = cv2.SIFT_create()
    # # find the keypoints and descriptors with SIFT
    # kp1, des1 = sift.detectAndCompute(img1,None)
    # kp2, des2 = sift.detectAndCompute(img2,None)
    # # BFMatcher with default params
    # bf = cv2.BFMatcher()
    # matches = bf.knnMatch(des1,des2,k=2)
    # # Apply ratio test
    # good = []
    # for m,n in matches:
    #     if m.distance < 0.75*n.distance:
    #         good.append([m])
    # # cv.drawMatchesKnn expects list of lists as matches.
    # img3 = cv2.drawMatchesKnn(img1,kp1,img2,kp2,good,None,flags=cv2.DrawMatchesFlags_NOT_DRAW_SINGLE_POINTS)
    # plt.imshow(img3),plt.show()


    # criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
    # # prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
    # objp = np.zeros((6*7,3), np.float32)
    # objp[:,:2] = np.mgrid[0:7,0:6].T.reshape(-1,2)
    # # Arrays to store object points and image points from all the images.
    # objpoints = [] # 3d point in real world space
    # imgpoints = [] # 2d points in image plane.
    # img = cv2.imread(search_img)
    # gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # # Find the chess board corners
    # ret, corners = cv2.findChessboardCorners(gray, (8,8), None)
    # print(corners)
    # # If found, add object points, image points (after refining them)
    # if ret == True:
    #     objpoints.append(objp)
    #     corners2 = cv2.cornerSubPix(gray,corners, (11,11), (-1,-1), criteria)
    #     imgpoints.append(corners)
    #     # Draw and display the corners
    #     cv2.drawChessboardCorners(img, (8,8), corners2, ret)
    #     cv2.imshow('img', img)
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()