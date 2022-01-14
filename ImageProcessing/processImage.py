import sys
sys.path.append(".")

import cv2
import numpy as np
from tensorflow import keras
from Solver.sudokuBoard import SudokuBoard
import time
import configparser


CELL_DIM=50


def preprocess_image(image):
    """Function preprocess image, BGR2GRAY-> GaussianBlur-> adaptiveThreshold-> Close-> Open

    Args:
        image (numpy array): Color image that needs to be preprocessed

    Returns:
        numpy array: Gray scale image that is preprocessed
    """
    new_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    new_image = cv2.GaussianBlur(new_image, (5, 5), 3)
    new_image = cv2.adaptiveThreshold(
        new_image, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY_INV, 13, 2)
    new_image = cv2.morphologyEx(new_image, cv2.MORPH_CLOSE, (3, 3))
    new_image = cv2.morphologyEx(new_image, cv2.MORPH_OPEN, (3, 3))

    return new_image


def find_board_corners(image, perimeter_threshold=0.05):
    """Find biggest conture on image that can be aproximated with four lines

    Args:
        image (numpy array): Image in which to search for conture
        perimeter_threshold (float, optional): Maximum perimetre difference between conture and conture aproximation. Defaults to 0.05.

    Returns:
        numpy array, float: 2D Array of coordinates of maximum conture, size of maximum conture
    """
    contours, _ = cv2.findContours(
        image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    max_area = 0
    max_conture = None
    
    for conture in contours:
        peri = cv2.arcLength(conture, True)
        conture_approx = cv2.approxPolyDP(conture, perimeter_threshold * peri, True)
        area = cv2.contourArea(conture_approx)
        if(area > max_area and len(conture_approx) == 4):
            max_area = area
            max_conture = conture_approx

    return max_conture, max_area

def orient_points_in_conture_clockwise(conture):
    """Orient coordinates in conture in clockwise direction starting from top left corner

    Args:
        conture (numpy array): 2D array of coordinates

    Returns:
        numpy array: 2D array of ordered coordinates 
    """
    ordered_index=np.argpartition(conture[:,1], 2)
    top2_index=ordered_index[:2]
    bottom2_index=ordered_index[2:]

    if(conture[top2_index[0]][0]>conture[top2_index[1]][0]):
        top_left=conture[top2_index[1]]
        top_right=conture[top2_index[0]]
    else:
        top_left=conture[top2_index[0]]
        top_right=conture[top2_index[1]]

    if(conture[bottom2_index[0]][0]>conture[bottom2_index[1]][0]):
        bottom_left=conture[bottom2_index[1]]
        bottom_right=conture[bottom2_index[0]]
    else:
        bottom_left=conture[bottom2_index[0]]
        bottom_right=conture[bottom2_index[1]]
    
    
    return np.array([top_left,top_right,bottom_right,bottom_left])


def find_distance(pointA, pointB):
    """Finds distance between two points A and B

    Args:
        pointA (array): Coordinates of point A
        pointB (array): Coordinates of point B

    Returns:
        {float): Distance between points
    """
    return np.sqrt((pointA[0]-pointB[0])**2+(pointA[1]-pointB[1])**2)


def get_perspective_transformation_matrix(conture):
    """Get perspective tranformation from four corners of square

    Args:
        conture (numpy array): 2D array of corner coordinates

    Returns:
        (numpy array), (integer), (integer): 3x3 perspective matrix, width and height of square that is transformed
    """
    source_points = np.reshape(conture, (4, 2)).astype(np.float32)
    source_points=orient_points_in_conture_clockwise(source_points)

    width = max(int(find_distance(source_points[0], source_points[1])), int(
        find_distance(source_points[3], source_points[2])))
    height = max(int(find_distance(source_points[0], source_points[3])), int(
        find_distance(source_points[1], source_points[2])))

    dest_points = np.array(
        [[0, 0], [width-1, 0], [width-1, height-1], [0, height-1]], dtype=np.float32)
    
    perspective_matrix = cv2.getPerspectiveTransform(
        source_points, dest_points)

    return perspective_matrix, width, height

def get_cells_from_image(image,width,height,margin_size=0.1, number_of_cells=9):
    """Cut image into 9x9 cells and return array of cells

    Args:
        image (numpy array): Image of sudoku board
        width (int): Width of image
        height (int): Height of image
        margin_size (float, optional): Percentage of each cell that is removed from edge. Defaults to 0.1.
        number_of_cells (int, optional): Number of cells in row or column of board. Defaults to 9.

    Returns:
        (list): Numpy array that contains cells cutted from image which have size CELL_DIM*CELL_DIM
    """
    size_x=width/number_of_cells
    size_y=height/number_of_cells
    margin_x=size_x*margin_size
    margin_y=size_y*margin_size
    cells=[]
    position_x=0
    position_y=0
    while(position_y<=height-size_y):
        while(position_x<=width-size_x):
            cell=image[int(position_y+margin_y):int(position_y+size_y-margin_y),int(position_x+margin_x):int(position_x+size_x-margin_y)]
            cell=cv2.resize(cell,(CELL_DIM,CELL_DIM))
            cells.append(cell)
            position_x+=int(size_x)
        position_y+=int(size_y)
        position_x=0    
    return cells

def filter_cells_for_classification(cells):
    """Filter cells that have number in them and remove noise from that cells

    Args:
        cells (numpy array): Numpy array that contains cells cutted from image which have size CELL_DIM*CELL_DIM

    Returns:
        (list),(list):Cells that have numbers, list that contains position in sudoku board of each cell that have number  
    """
    filtered_cells=[]
    values=[]
    for cell in cells:
        contours, _ = cv2.findContours(cell, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        if(len(contours)>0):
            c = max(contours, key = cv2.contourArea)
            mask = np.zeros(cell.shape, dtype="uint8")
            cv2.drawContours(mask, [c], -1, 255, -1)
            percentFilled = cv2.countNonZero(mask) / float(cell.shape[0] *cell.shape[1])

            if(percentFilled<0.1):
                values.append(0)
            else:
                digit = cv2.bitwise_and(cell, cell, mask=mask)
                if(np.count_nonzero(digit[int(CELL_DIM*0.3):-int(CELL_DIM*0.3),int(CELL_DIM*0.3):-int(CELL_DIM*0.3)])>0):
                    filtered_cells.append(np.reshape(digit,(1,CELL_DIM,CELL_DIM,1)))
                    values.append(-1)
                else:
                    values.append(0)
        else:
            values.append(0)
    
    return filtered_cells,values


def draw_solution_mask(shape,matrix,filled_cells,size_up_dim=40, x_correction=0.3, y_correction=0.78):
    """Create mask with with numbers that are missing from solution on white background

    Args:
        shape ((integer,integer)): Shape of mask to be created
        matrix (numpy array): Sudoku board matrix that is solved
        filled_cells ([numpy array]): Array that contains markers for all field that are filled in original image
        size_up_dim (int, optional): Size of cell in pixel above which font is increased. Defaults to 40.
        x_correction (float, optional): Offset on X axis in cell from which to draw number. Defaults to 0.3.
        y_correction (float, optional): Offset on Y axis in cell from which to draw number. Defaults to 0.78.

    Returns:
        (numpy array): Mask with missing number drawn on it
    """
    image=np.full(shape,255,dtype='uint8')
    color = (0, 0, 0)
    thickness = 2
    font = cv2.FONT_HERSHEY_SCRIPT_COMPLEX
    dim_x=shape[1]/9
    dim_y=shape[0]/9
    if(dim_x>size_up_dim and dim_y>size_up_dim):
        fontScale=2
    else:
        fontScale = 1

    index=0
    for row in range(0,9):
        for column in range(0,9):
            if(filled_cells[index]==0):
                position_x=int(column*dim_x+dim_x*x_correction)
                position_y=int(row*dim_y+dim_y*y_correction)
                image=cv2.putText(
                    img=image,
                    text=str(int(matrix[row,column])),
                    org=(position_x,position_y), 
                    bottomLeftOrigin=False,
                    fontScale=fontScale,
                    thickness=thickness,
                    color=color,
                    fontFace=font,
                    lineType=cv2.LINE_AA)
            index+=1

    return image


def check_border(image, threshold=0.2, border_width=5):
    """Checks if border of image are filled or not  

    Args:
        image (numpy array): Image which borders are checked
        threshold (float, optional): Minimum percentage of boarder that needs to be filled. Defaults to 0.2.
        border_width (int, optional): Width of boarder that is checked. Defaults to 5.

    Returns:
        (bool): If all boarders are filled above threshold returns True else returns False
    """
    width=image.shape[1]
    height=image.shape[0]
    
    top_border=image[0:border_width,:]
    bottom_border=image[-border_width:,:]
    left_border=image[:,0:border_width]
    right_border=image[:,-border_width:]

    top_border_pct_filed=np.count_nonzero(top_border)/(width*border_width)
    bottom_border_pct_filed=np.count_nonzero(bottom_border)/(width*border_width)
    left_border_pct_filed=np.count_nonzero(left_border)/(height*border_width)
    right_border_pct_filed=np.count_nonzero(right_border)/(height*border_width)
    if(top_border_pct_filed>threshold and bottom_border_pct_filed>threshold and left_border_pct_filed>threshold and right_border_pct_filed>threshold):
        return True
    else:
        return False


if __name__ == "__main__":
    config = configparser.ConfigParser()
    config.read('config.ini')

    model = keras.models.load_model(config['Resources']['model'])
    image = cv2.imread(config['Resources']['image'])
 

    image_area=image.shape[0]*image.shape[1]
    start_time=time.time()

    image_pre = preprocess_image(image)
    image_area=image_pre.shape[0]*image_pre.shape[1]

    conture,conture_area = find_board_corners(image_pre)
    img_contures = cv2.drawContours(image, [conture], -1, (100, 100, 255), 2)
    perspective_matrix, width, height = get_perspective_transformation_matrix(conture)
    #invers_matrix=np.linalg.inv(perspective_matrix)
    img_warped = cv2.warpPerspective(image_pre, perspective_matrix, (width, height))
    cv2.imwrite("F:\\ML Projects\\CUBIC Praksa\\SudokuSolver\\Data\\Images\\warped.jpg",img_warped)
    #img_warped_color=cv2.warpPerspective(image, perspective_matrix, (width, height))
    #cv2.imshow("warped", img_warped)
    #cv2.imshow('conture',img_contures)
    #print(time.time()-start_time)  
    #print(image_area,conture_area)
    if(check_border(img_warped) and image_area*0.2<conture_area):
    
        cells=get_cells_from_image(img_warped,width,height)

        filtered_cells,values=filter_cells_for_classification(cells)

        # for index,cell in enumerate(filtered_cells):
        #     cv2.imwrite(f"F:\\ML Projects\\CUBIC Praksa\\SudokuSolver\\Data\\TestCells\\{index}.jpg",np.reshape(cell,(50,50)))

        if(len(filtered_cells)>0):
            batch=np.concatenate(filtered_cells)

            prediction = model.predict_on_batch(batch)
            predicted_values=np.argmax(prediction,axis=1)+1

        predicted_index=0
        board_input_matrix=np.copy(values)
        for index,value in enumerate(values):
            if(value==-1):
                board_input_matrix[index]=int(predicted_values[predicted_index])
                predicted_index+=1

        game_board=SudokuBoard(np.reshape(board_input_matrix,(9,9)),input_type='array')
                
                
        if(game_board.solve()):
            print(time.time()-start_time)  
            solved=draw_solution_mask(img_warped.shape,game_board.board,values)
            solved=cv2.warpPerspective(solved,perspective_matrix,(image.shape[1],image.shape[0]),borderValue=255,flags=cv2.WARP_INVERSE_MAP)

            #cv2.imshow('solved',solved)
            #cv2.imshow('org',image)
            image[:,:,1]=np.bitwise_and(image[:,:,1],solved)
        else:
            print('NO SOLUTION FOUND')
            print(game_board.board)
    else:
        print('BAD CONTURE')

    cv2.imshow("final_image", image)
    print(time.time()-start_time)    
    cv2.waitKey(0)