import sys
sys.path.append(".")

import multiprocessing
import cv2
import numpy as np
import processImage
import time

from Solver.sudokuBoard import SudokuBoard


MODEL="F:\\ML Projects\\CUBIC Praksa\\SudokuSolver\\OCR\\ocr_model_v1_gen"
VIDEO="F:\\ML Projects\\CUBIC Praksa\\SudokuSolver\\Data\\Video\\video2.mp4"
NUMBER_OF_PROCESSES=3

def find_new_solution(image,model):
    cells=processImage.get_cells_from_image(image,image.shape[1],image.shape[0])
    filtered_cells,values=processImage.filter_cells_for_classification(cells)
    batch=np.concatenate(filtered_cells)
    prediction = model.predict_on_batch(batch)
    predicted_values=np.argmax(prediction,axis=1)+1               
    board_input_matrix=np.copy(values)
    predicted_index=0
    for index,value in enumerate(values):
        if(value==-1):
            board_input_matrix[index]=int(predicted_values[predicted_index])
            predicted_index+=1
    game_board=SudokuBoard(np.reshape(board_input_matrix,(9,9)),input_type='array')

    if(game_board.solve()):
        return (True,game_board.board,values)         

    return (False, None, None)

def solving_worker(data_queue,result_queue):
    from tensorflow import keras
    model = keras.models.load_model(MODEL)
    while(True):
        if(not data_queue.empty()):
            image=data_queue.get()
            result=find_new_solution(image,model)
            if(result[0]):
                result_queue.put(result)

def solving_worker_test(image,result_queue):
    from tensorflow import keras
    model = keras.models.load_model(MODEL)
    print('worker started')
    print(type(model))
    result_queue.put((False,))


if __name__ == "__main__":
    vid = cv2.VideoCapture(0)

    if (vid.isOpened()== False):
        print("Error opening video")
    else:
        #model = keras.models.load_model(MODEL)

        fps = vid.get(cv2.CAP_PROP_FPS)
        frame_time=1000/fps

        board_is_solved=False
        solution=None
        sol_values=None
        change_index=0

        manager = multiprocessing.Manager()
        data_queue=manager.Queue()
        result_queue = manager.Queue()
        pool = multiprocessing.Pool(processes=NUMBER_OF_PROCESSES, initializer=solving_worker, initargs=(data_queue,result_queue,))

        while(vid.isOpened()):
            ret, frame = vid.read()
            start_time=time.time()
            if(ret==True):
                frame_size=frame.shape[0]*frame.shape[1]
                image_pre = processImage.preprocess_image(frame)
                conture, area = processImage.find_board_corners(image_pre)
                perspective_matrix, width, height = processImage.get_perspective_transformation_matrix(conture)
                img_warped = cv2.warpPerspective(image_pre, perspective_matrix, (width, height))
                if(area>0.05*frame_size and processImage.check_border(img_warped)):
                    data_queue.put(img_warped)
                    while(not result_queue.empty()):
                        result=result_queue.get()
                        if(np.array_equal(solution, result[1]) and board_is_solved):
                            change_index+=1
                        else:
                            change_index=0
                        
                        if(board_is_solved and change_index>5):
                            change_index=0
                            board_is_solved=True
                            solution=result[1]
                            sol_values=result[2]
                        elif(not board_is_solved):
                            board_is_solved=True
                            solution=result[1]
                            sol_values=result[2]

                    if(board_is_solved):
                        solved=processImage.draw_solution_mask(img_warped.shape,width,height,solution,sol_values)
                        solved=cv2.warpPerspective(solved,perspective_matrix,(frame.shape[1],frame.shape[0]),borderValue=255,flags=cv2.WARP_INVERSE_MAP)
                        frame[:,:,1]=np.bitwise_and(frame[:,:,1],solved)
                        frame=cv2.drawContours(frame, [conture], -1, (255, 0, 255), 2)
                        cv2.putText(frame,'SOLVED',(0,100),fontScale=2,color=(255,0,255),thickness=2,fontFace=cv2.FONT_HERSHEY_PLAIN)
                    else:
                        frame=cv2.drawContours(frame, [conture], -1, (0, 255, 255), 2)
                        cv2.putText(frame,'SOLVING',(0,100),fontScale=2,color=(0,255,255),thickness=2,fontFace=cv2.FONT_HERSHEY_PLAIN)
                else:
                    cv2.putText(frame,'NO SUDOKU',(0,100),fontScale=2,color=(255,0,0),thickness=2,fontFace=cv2.FONT_HERSHEY_PLAIN)
                
                time_difference=time.time()-start_time
                cv2.putText(frame,str(int(1/time_difference)),(0,50),fontScale=2,color=(0,0,255),thickness=2,fontFace=cv2.FONT_HERSHEY_PLAIN)
                cv2.imshow('frame', frame)

                remaining_frame_time=int(frame_time-time_difference/1000)
                if cv2.waitKey(max(remaining_frame_time,1)) & 0xFF == ord('q'):
                    break

            else:
                break
        
        pool.close()
        pool.terminate()   

    vid.release()
    cv2.destroyAllWindows()
