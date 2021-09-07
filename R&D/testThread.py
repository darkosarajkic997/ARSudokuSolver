import sys
sys.path.append(".")
import queue
from threading import Thread
import time

def funk(task_queue,result_queue):
    while True:
        number=task_queue.get()
        time.sleep(5-number)
        print(f"work on unmber {number}")
        task_queue.task_done()
        result_queue.put(number**2)

if __name__=='__main__':
    num_threads = 5
    task_queue=queue.Queue()
    result_queue=queue.Queue()
    
    index=0

    for i in range(num_threads):
        worker = Thread(target=funk, args=(task_queue,result_queue,))
        worker.setDaemon(True)
        worker.start()
    
    while(index<3):
        for i in range(0,5):
            task_queue.put(i)
            print(f"added {i}")
        print("RES QUEUE")
        time.sleep(2)
        while(not result_queue.empty()):
            print(result_queue.get())
            result_queue.task_done()
        index+=1

    