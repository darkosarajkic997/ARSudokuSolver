# *ARSudokuSolver*
## *Augmented reality sudoku solver written in Python and C++* (USE THIS FOR SPRINT CLOSING PPT SLIDE)
- ***Modeified backtracking algorithm for solving sudoku:*** Uses extra memory space to keep track of all possible numbers in each cell. Next cell for backtracking algorithm is selected as one with minimum remaining possibilities. If there is a cell that is not filled and don't have any possible number to put in it than current solution is invalid and algorithm goes back.
- ***OCR model:*** Keras convolutional neural network trained on a set of digit images. Model classifies input binary images of size 50*50 pixels into 9 classes that represent a number from 1 to 9
- ***Sudoku board detection using Python and OpenCV:*** Finds biggest conture in an image that can be approximated with 4 lines and selects it as a possible sudoku board. From the 4 corners algorithm gets homogrpahy transformation and warps sudoku board.  
- ***Video processing:*** Process video frame by frame, on each frame finds if there is a possible sudoku board and if there is one send warped sudoku board to shared input data queue. Background workers process board and find numbers. If unsolved board is found in shared solution dictionary than key for that solution is placed in shared solution queue. Main process reads solution queue and if there is solution creates mask to be displayed over input frame and displays it

#### ***In progress***
- ***C++ DLL for board finding:*** Custom C++ DLL that can be called from Python script, which finds the homography matrix and board position. In DLL we will use Hough line detector for line detection, DBSCAN Algorithm for line clustering and RANSAC algorithm for selecting best possible homography from all intersecting points of two clusters of lines.   

![Alt Text](https://github.com/darkosarajkic997/ARSudokuSolver/blob/master/sudoku.gif)

## *Installation*
- Install required python libraries from requirements.txt using pip install -r requirements.txt
- Create config.ini file using config.ini.example and fill required fields

## *Usage*
- Run ImageProcessing\processVideoWithProc.py script, default setting is to use video from camera, if you want to use custom video change parameter in first line of main function in call of openCV function cv2.VideoCapture() from 0 to video path
