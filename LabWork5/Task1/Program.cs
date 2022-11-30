using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using static System.Collections.Specialized.BitVector32;

namespace Task1
{
/*
ACTION NEEDS TO SEPERATES TASKS IN QUEUE(PLUG)
*/
    public class ExAction
    {
        public static void PrintHello()
        {
            Console.WriteLine("");
        }
    }
/*
PROGRAM DOCS 
IN CYCLE WE MAKE COUNT OF THREADS(10)
tasksQueue.EnqueueTask(action) - IN EVERY THREAD WE PUSH TASK FOR EXECUTION 
*/
    internal class Program
    {
        static void Main(string[] args)
        {
            TaskQueue tasksQueue = new TaskQueue();
            Action action = ExAction.PrintHello;
       
            for (int i = 0; i < 10; i++)
            {
                new Thread(() =>
                {
                    tasksQueue.EnqueueTask(action);
                }).Start();
            }
            Console.WriteLine();
        }
    }
/*
TASKQUEUE DOCS 
QUEUE<ACTION> IS A QUEUE OF TASK FOR EXECUTION
NEW QUEUE - IS A EMPTY LIST TO FILL 
ENQUEUETASK IS A METHOD TO ADD TASK TO A QUEUE FOR EXECUTION
FOR THIS WE NEED TO BLOCK AN OBJECT OF QUEUE
*/
    public class TaskQueue
    {
        private Queue<Action> tasks;

        public TaskQueue()
        {
            tasks = new Queue<Action>();
        }

        public void EnqueueTask(Action task)
        {
            lock (tasks)
            {
                tasks.Enqueue(task);
                Console.WriteLine("WHILE U STILL ALIVE U CAN ALWAYS TRY!");
                Thread.Sleep(1000);
            }
        }
    }
}
