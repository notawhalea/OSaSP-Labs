using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Task3
{
/*STORAGE WITH OUR STRINGS*/
    public static class Storage
    {
        public static List<string> UnsortedStrings { get; set; } = new List<string>();
        public static List<string> SortedStrings { get; set; } = new List<string>();
    }

    internal class Program
    {
        static void Main(string[] args)
        {
            int threadCount = 5;
            string filePath = @"D:\БГУИР\3 курс\5 семестр\ОСиСП\Прочие\OSSP-LabWork4\vadperedtoboivraiposletebya.txt";

            TaskQueue tasksQueue = new TaskQueue(threadCount);
            StringsTaker stringsTaker = new StringsTaker(filePath);

            Storage.UnsortedStrings = stringsTaker.TakeStrings(threadCount);

            Action action = SortString;

            for (int i = 0; i < threadCount; i++)
                tasksQueue.EnqueueTask(SortString);

            tasksQueue.Close();

            for (int i = 0; i < Storage.SortedStrings.Count; i++)
            {
                Console.WriteLine($"String №{i+1}: {Storage.SortedStrings[i]}\n");
            }

            StringBuilder sb = new StringBuilder(string.Empty);

            foreach (string str in Storage.SortedStrings)
                sb.AppendLine(str);


            Console.WriteLine($"{string.Concat(sb.ToString().OrderBy(c => c)).Replace(" ", "").Replace("\n", "")}");

            Console.ReadLine();

        }

        static void SortString()
        {
            string unsortedString;

            lock(Storage.UnsortedStrings)
            {
                unsortedString = Storage.UnsortedStrings.First();
                Storage.UnsortedStrings.RemoveAt(0);

                Monitor.Pulse(Storage.UnsortedStrings);
            }

            string sortedString = string.Concat(unsortedString.OrderBy(c => c));

            lock(Storage.SortedStrings)
            {
                Storage.SortedStrings.Add(sortedString);

                Monitor.Pulse(Storage.SortedStrings);
            }
        }
    }

    public class StringsTaker
    {
        private readonly string filePath;

        public StringsTaker(string filePath)
        {
            this.filePath = filePath;
        }

        public List<string> TakeStrings(int threadsCount)
        {
            List<string> result = new List<string>();

            string text = File.ReadAllText(filePath);

            int startIndex = 0;
            int length = text.Length / threadsCount;

            for (int i = 0; i < threadsCount - 1; i++)
            {
                result.Add(text.Substring(startIndex, length));
                startIndex = length * (i + 1);
            }

            if (text.Length % threadsCount == 0)
                result.Add(text.Substring(startIndex, length));
            else
                result.Add(text.Substring(startIndex, length + text.Length % threadsCount));

            return result;
        }
    }



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

        public void Close()
        {
            for (int i = 0; i < threads.Count; i++)
                EnqueueTask(null);

             foreach (Thread t in threads)
                t.Join();
        }
    }
}
