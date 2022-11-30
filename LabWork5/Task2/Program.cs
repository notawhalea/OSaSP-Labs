using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Task2
{
/*ACTION WE NEED TO MAKE SURE THAT OUR PROGRAM WORK*/
    public class ExThread
    {
        public static void PrintPID()
        {
            for (int i = 0; i < 10; i++)
            {
                Console.WriteLine($"Message from Thread with PID: {Thread.CurrentThread.ManagedThreadId}");
                Thread.Sleep(1000);
            }
        }
    }

    internal class Program
    {
/*MAIN DOCS
TaskQueue tasksQueue = new TaskQueue(2) MAKE QUEUE AND ADD DELEGATE WITH LINK ON METHOD
2 - IS A COUNT OF THREADS
EVERY THREAD ADD IN QUEUE ATOMICALLY
TO EXIT WE WAIT WHILE ALL TASK IN QUEUE WILL FINISH
*/
        static void Main(string[] args)
        {
            TaskQueue tasksQueue = new TaskQueue(2);
            Action action = ExThread.PrintPID;

            for (int i = 0; i < 2; i++)
            {
                Thread.Sleep(1000);
                new Thread(() =>
                {
                    Console.WriteLine($"Task {i} from Thread with PID : {Thread.CurrentThread.ManagedThreadId} has been pulled into the queue");
                    tasksQueue.EnqueueTask(action);
                }).Start();
            }
            tasksQueue.Close();
        }
    }
/*
TASKQUEUE DOCS
TaskQueue(int threadCount) - CONSTRUCTOR FOR OBJECT ACTIVATING 
var t = new Thread(DoThreadWork) - TAKE DEFAULT TASK FOR EXECUTION AND ADD IT FOR THREAD
DoThreadWork() - DEFAULT METHOD FOR IMPLEMENTATION LIKE METHOD IN THREAD
*/
    public class TaskQueue
    {
        private List<Thread> threads;
        private Queue<Action> tasks;

        public TaskQueue(int threadCount)
        {
            tasks = new Queue<Action>();
            threads = new List<Thread>();

            for (int i = 0; i < threadCount; i++)
            {
               var t = new Thread(DoThreadWork);
                threads.Add(t);
                t.IsBackground = true;
                t.Start();
            }
        }
/*
DOTHREADWORK DOCS
Action task = DequeueTask() - TRY TO TAKE TASK FOR EXECUTION FROM QUEUE
Thread.ResetAbort() - IF THREAD CLOSE IN METHOD - WE ABORT CLOSING
CYCLE CONTINUE WHILE TASK FOR EXECUTION IS NOT NULL
*/
        private void DoThreadWork()
        {
            while (true)
            {
                Action task = DequeueTask();

                if (task != null)
                {
                    try
                    {
                        task();
                    }
                    catch (ThreadAbortException)
                    {
                        Thread.ResetAbort();
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(ex);
                    }
                }
                else 
                    break;
            }
        }
/*DEQUEUE DOCS
THIS METHOD NEED FOR EXTRACT ACTION FROM QUEUE
WHILE COUNT OF TASK FOR EXECUTION IS EQUAL TO ZERO - STAY FOR WAITING
WHEN COUNT MORE THAN ONE THAN ASK FOR METHOD
*/
        private Action DequeueTask()
        {
            lock (tasks)
            {
                while (tasks.Count == 0)
                    Monitor.Wait(tasks);
                return tasks.Dequeue();
            }
        }

        public void EnqueueTask(Action task)
        {
            lock (tasks)
            {
                tasks.Enqueue(task);
                Monitor.Pulse(tasks);
            }
        }
/*CLOSE DOCS 
THIS METHOD FOR CLOSE ALL ACTIVE THREADS
EnqueueTask(null) - SYGNAL FOR CLOSING THAN WAIT WHEN ALL THREADS CLOSE
*/
        public void Close()
        {
            for (int i = 0; i < threads.Count; i++)
                EnqueueTask(null); 

            foreach (Thread t in threads)
                t.Join();
        }
    }
}
