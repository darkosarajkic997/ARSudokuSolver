import time
import processImage
import numpy as np
import cv2
import multiprocessing
import sys
import configparser

sys.path.append(".")
from Solver.sudokuBoard import SudokuBoard

NUMBER_OF_PROCESSES = 1
ALPHA = 0.2
SMOOTH_QUEUE_SIZE = 5
CHANGE_SOLUTION_THRESHOLD = 5
MIN_PCT_OF_FRAME = 0.1
MIN_FRAME_TO_CLEAR_SOLUTION=25


def generate_matrix_from_image(image, model):
    """Find numbers on image and solve sudoku

    Args:
        image (numpy array): Image of sudoku board
        model (Keras model): Pretrained Keras model for OCR

    Returns:
        ((bool),(numpy array)): Tuple where first element indicates whether solution is found and second is solution if one is found
    """
    cells = processImage.get_cells_from_image(
        image, image.shape[1], image.shape[0])
    filtered_cells, values = processImage.filter_cells_for_classification(
        cells)
    if(len(filtered_cells) > 0):
        batch = np.concatenate(filtered_cells)
        prediction = model.predict_on_batch(batch)
        predicted_values = np.argmax(prediction, axis=1)+1
        predicted_index = 0
        for index, value in enumerate(values):
            if(value == -1):
                values[index] = int(predicted_values[predicted_index])
                predicted_index += 1

        return (True, np.array(values))

    return (False,)


def solving_worker(data_queue, result_queue, board_dict, model):
    """Create background worker that process images, worker check shared data_queue for data and if find one process it.
    After finding what numbers are on image if board was solved earlier writes key from shared dictionary with solutions in shared result queue, else
    if board is not solved, worker solves it, writes solution in shared dictionary and key in shared result queue 

    Args:
        data_queue (Queue): Shared queue that contains conture images that are found in main process 
        result_queue (Queue]): Shared queue that contains keys for results from shared dictionary  
        board_dict (Dictionary): Shared dictionary that contains key, value pairs where key is string representation of unsolved board and value is solved board
        model (Keras): Pretrained Keras model for OCR
    """
    from tensorflow import keras
    model = keras.models.load_model(model)
    while(True):
        if(not data_queue.empty()):
            image = data_queue.get()
            extracted_data = generate_matrix_from_image(image, model)
            if(extracted_data[0]):
                key = np.array2string(extracted_data[1])
                if(key not in board_dict):
                    init_values = np.copy(extracted_data[1])
                    game_board = SudokuBoard(np.reshape(extracted_data[1], (9, 9)), input_type='array')
                    if(game_board.validate_board() and game_board.solve()):
                        board_dict[key] = (game_board.board, init_values)
                        result_queue.put((key))
                else:
                    result_queue.put((key))


def create_background_processes():
    manager = multiprocessing.Manager()
    data_queue = manager.Queue()
    result_queue = manager.Queue()
    board_dict = manager.dict()
    pool = multiprocessing.Pool(processes=NUMBER_OF_PROCESSES, initializer=solving_worker, initargs=(data_queue, result_queue, board_dict, config['Resources']['model']))
    return data_queue, result_queue, board_dict, pool

def image_resize(image, width = None, height = None, inter = cv2.INTER_AREA):
    dim = None
    (h, w) = image.shape[:2]

    if width is None and height is None:
        return image
    if width is None:
        r = height / float(h)
        dim = (int(w * r), height)
    else:
        r = width / float(w)
        dim = (width, int(h * r))
    resized = cv2.resize(image, dim, interpolation = inter)
    return resized

if __name__ == "__main__":
    config = configparser.ConfigParser()
    config.read('config.ini')
    vid = cv2.VideoCapture("F:\\ML Projects\\CUBIC Praksa\\SudokuSolver\\Data\\Video\\20211011_203347.mp4")
    if (vid.isOpened() == False):
        print("Error opening video")
    else:
        board_is_solved = False
        solution_key = None
        new_solution_key = None
        change_index = 0
        solution = None
        data_queue, result_queue, board_dict, pool = create_background_processes()
        transformation_matrix_queue = np.zeros((SMOOTH_QUEUE_SIZE, 3, 3))
        matrix_index = 0
        clear_solution_frame_count=0
        index=0
        while(vid.isOpened()):
            ret, frame = vid.read()
            if(ret == True):
                index+=1
                #frame = image_resize(frame, width=800)
                frame_size = frame.shape[0]*frame.shape[1]
                image_pre = processImage.preprocess_image(frame)
                conture, area = processImage.find_board_corners(image_pre)
                if(area > MIN_PCT_OF_FRAME*frame_size):
                    perspective_matrix, width, height = processImage.get_perspective_transformation_matrix(conture)
                    img_warped = cv2.warpPerspective(image_pre, perspective_matrix, (width, height))
                    if(processImage.check_border(img_warped)):
                        clear_solution_frame_count=0
                        data_queue.put(img_warped)
                        while(not result_queue.empty()):
                            key = result_queue.get()
                            if(not board_is_solved):
                                board_is_solved = True
                                solution = board_dict.get(key)
                                solution_key = key
                            else:
                                if(solution_key != key):
                                    if(new_solution_key is None):
                                        new_solution_key = key
                                        change_index = 1
                                    elif(new_solution_key == key):
                                        change_index += 1
                                    else:
                                        new_solution_key = key
                                else:
                                    change_index = 0

                        transformation_matrix_queue[matrix_index % SMOOTH_QUEUE_SIZE, :, :] = np.copy(perspective_matrix)
                        matrix_index += 1
                        if(matrix_index > SMOOTH_QUEUE_SIZE):
                            smooth_matrix = (np.mean(transformation_matrix_queue, axis=0))*ALPHA+perspective_matrix*(1-ALPHA)
                        else:
                            smooth_matrix = perspective_matrix

                        if(change_index > CHANGE_SOLUTION_THRESHOLD):
                            solution = board_dict.get(new_solution_key)
                            solution_key = new_solution_key
                            new_solution_key = None
                        if(board_is_solved and solution is not None):
                            solved = processImage.draw_solution_mask(img_warped.shape, solution[0], solution[1])
                            solved = cv2.warpPerspective(solved, smooth_matrix, (frame.shape[1], frame.shape[0]), borderValue=255, flags=cv2.WARP_INVERSE_MAP)
                            frame[:, :, 1] = np.bitwise_and(frame[:, :, 1], solved)
                            frame = cv2.drawContours(frame, [conture], -1, (255, 0, 255), 2)
                            cv2.putText(frame, 'SOLVED', (0, 50), fontScale=2, color=(255, 0, 255), thickness=2, fontFace=cv2.FONT_HERSHEY_PLAIN)
                        else:
                            frame = cv2.drawContours(frame, [conture], -1, (0, 255, 255), 2)
                            cv2.putText(frame, 'SOLVING', (0, 50), fontScale=2, color=(0, 255, 255), thickness=2, fontFace=cv2.FONT_HERSHEY_PLAIN)
                    else:
                        clear_solution_frame_count+=1
                else:
                    cv2.putText(frame, 'NO SUDOKU', (0, 50), fontScale=2, color=(255, 0, 0), thickness=2, fontFace=cv2.FONT_HERSHEY_PLAIN)
                    clear_solution_frame_count+=1
                if(clear_solution_frame_count>MIN_FRAME_TO_CLEAR_SOLUTION):
                    board_is_solved = False
                    solution_key = None
                    solution = None
                #cv2.imwrite(f"F:\ML Projects\CUBIC Praksa\SudokuSolver\Data\gif\{index}.jpg",frame)
                cv2.imshow('frame', frame)
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
            else:
                break

        pool.close()
        pool.terminate()

    vid.release()
    cv2.destroyAllWindows()
