import numpy as np
import cv2

def fillEquationMatrix(Hmat,sourcePoint,destinationPoint,rowIndex):
    Hmat[rowIndex,0]=-destinationPoint[0]
    Hmat[rowIndex,1]=-destinationPoint[1]
    Hmat[rowIndex,2]=-1
    Hmat[rowIndex,6]=destinationPoint[0]*sourcePoint[0]
    Hmat[rowIndex,7]=destinationPoint[1]*sourcePoint[0]
    Hmat[rowIndex,8]=sourcePoint[0]
    Hmat[rowIndex+1,3]=-destinationPoint[0]
    Hmat[rowIndex+1,4]=-destinationPoint[1]
    Hmat[rowIndex+1,5]=-1
    Hmat[rowIndex+1,6]=destinationPoint[0]*sourcePoint[1]
    Hmat[rowIndex+1,7]=destinationPoint[1]*sourcePoint[1]
    Hmat[rowIndex+1,8]=sourcePoint[1]


if __name__ == "__main__":
    destination=[[355.378357,112.425209],[468.996429,73.3033],[520.466370,155.036240],[402.700592,201.425400]]
    source=[[0,0],[450,0],[450,450],[0,450]] 

    sourcePoints=np.array(source)
    destinationPoints=np.array(destination)

    mat,_=cv2.findHomography(sourcePoints,destinationPoints)

    point=np.array([577.183350,245.101288,1])

    print(mat)

    new_point=np.linalg.solve(mat, point)
    print(new_point)
    print(new_point[0]/new_point[2],new_point[1]/new_point[2])

    Hmat=np.zeros((9,9))
    Hmat[8,8]=1
    solution=np.zeros((9))
    solution[8]=1
    fillEquationMatrix(Hmat,destinationPoints[0],sourcePoints[0],0)
    fillEquationMatrix(Hmat,destinationPoints[1],sourcePoints[1],2)
    fillEquationMatrix(Hmat,destinationPoints[2],sourcePoints[2],4)
    fillEquationMatrix(Hmat,destinationPoints[3],sourcePoints[3],6)
    print("\n")
    print(Hmat)
    print("\n")
    homography2=np.linalg.solve(Hmat,solution)
    print(homography2.reshape(3,3))
    point=np.array([577.183350,245.101288,1])
    new_point=np.linalg.solve(homography2.reshape(3,3), point)
    print(new_point)
    print(new_point[0]/new_point[2],new_point[1]/new_point[2])


