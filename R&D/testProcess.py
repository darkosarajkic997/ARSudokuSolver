import multiprocessing
import time

class TestClass:
    def __init__(self):
        self.x=2

    def prod(self,number):
        time.sleep(2)
        return self.x*number

def funk(data_queue, result_queue):
    while(True):
        if(not data_queue.empty()):
            number=data_queue.get()
            print(f"work on unmber {number}")
            result_queue.put(number)

if __name__ == '__main__':
    m = multiprocessing.Manager()
    data_q = m.Queue()
    res_q=m.Queue()
    pool = multiprocessing.Pool(processes=3, initializer=funk, initargs=(data_q,res_q))
    
    index=0
    while(index<3):
        for i in range(0,5):
            data_q.put(i)
            print(f"added {i}")
        print("RES QUEUE")
        time.sleep(2)
        while(not res_q.empty()):
            print(f"from queue {res_q.get()}")
        index+=1
        
    pool.close()
    pool.terminate()
