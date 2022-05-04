#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Thread
{
    int ID;
} Thread;

typedef struct Task
{
    int ID;
    Thread thread;
    int prioritate, timp_exec, timp_exec_ramas;
} Task;

typedef struct Stack
{
    Thread thread;
    struct Stack *next;
} Stack;

typedef struct Queue
{
    Task task;
    struct Queue *next;
} Queue;

Stack *init_node_s(int id)
{
    Stack *p = (Stack *)malloc(sizeof(Stack));
    p->thread.ID = id;
    p->next = NULL;
    return p;
}

void add_to_stack(Stack **top, Stack *new_node)
{
    new_node->next = *top;
    *top = new_node;
}

Queue *init_node_q(int id, int pr, int ID_th, int timp)
{
    Queue *p = (Queue *)malloc(sizeof(Queue));
    p->task.ID = id;
    p->task.prioritate = pr;
    p->task.thread.ID = ID_th;
    p->task.timp_exec = timp;
    p->task.timp_exec_ramas = timp;
    p->next = NULL;
    return p;
}

int comp_func(Queue *Node1, Queue *Node2)
{
    if (Node1->task.prioritate < Node2->task.prioritate)
        return 1;
    if (Node1->task.prioritate > Node2->task.prioritate)
        return 0;
    if (Node1->task.timp_exec > Node2->task.timp_exec)
        return 1;
    if (Node1->task.timp_exec < Node2->task.timp_exec)
        return 0;
    if (Node1->task.ID > Node2->task.ID)
        return 1;
    return 0;
}

void add_to_queue(Queue **first, Queue *new_node)
{
    if (*first == NULL)
    {
        *first = new_node;
        new_node->next = NULL;
    }
    else
    {
        Queue *p = *first;
        if (comp_func(new_node, *first) == 0)
        {
            new_node->next = *first;
            *first = new_node;
        }
        else
        {
            while (p->next != NULL && comp_func(new_node, p->next))
                p = p->next;

            new_node->next = p->next;
            p->next = new_node;
        }
    }
}

Queue *pop_from_queue(Queue **first)
{
    Queue *p = *first;
    if (*first != NULL)
    {
        *first = (*first)->next;
        p->next = NULL;
    }
    return p;
}

void print_stack(Stack *top)
{
    Stack *p = top;
    while (p != NULL)
    {
        printf("%d ", p->thread.ID);
        p = p->next;
    }
}

void print_waiting(Queue *first)
{
    Queue *p = first;

    printf("====== Waiting queue ======\n[");
    while (p != NULL)
    {
        printf("(%d: priority = %d, remaining_time = %d)", p->task.ID, p->task.prioritate, p->task.timp_exec);
        if (p->next != NULL)
            printf(",\n");
        else
            printf("]\n");
        p = p->next;
    }
}

void print_running(Queue *first)
{
    Queue *p = first;

    printf("====== Running in parallel ======\n[");
    while (p != NULL)
    {
        printf("(%d: priority = %d, remaining_time = %d, running_thread = %d)", p->task.ID, p->task.prioritate, p->task.timp_exec_ramas, p->task.thread.ID);
        if (p->next != NULL)
            printf(",\n");
        else
            printf("]\n");
        p = p->next;
    }
}
void print_finished(Queue *first)
{
    Queue *p = first;

    printf("====== Finished queue ======\n[");
    while (p != NULL)
    {
        printf("(%d: priority = %d, executed_time = %d)", p->task.ID, p->task.prioritate, p->task.timp_exec);
        if (p->next != NULL)
            printf(",\n");
        else
            printf("]\n");
        p = p->next;
    }
}

void complete_threads(Queue **waiting, Queue **running, int *nr_threads_disp, int *threads_disp, int N)
{

    int i;
    Queue *node;
    for (i = 0; i < *nr_threads_disp && *waiting != NULL; i++)
    {
        int id = N - 1;

        while (threads_disp[id] == 1)
            id--;

        node = pop_from_queue(waiting);
        node->task.thread.ID = id;
        threads_disp[id] = 1;
        add_to_queue(running, node);
    }
    *nr_threads_disp = *nr_threads_disp - i;
}

void get_thread(int thread_id, Queue *running, int *threads_disp)
{
    if (threads_disp[thread_id] == 0)
        printf("Thread <thread_id> is idle.\n");
    else
    {
        Queue *p = running;
        while (p->task.thread.ID != thread_id)
        {
            p = p->next;
        }
        printf("Thread %d is running task %d (remaining_time = %d).\n", thread_id, p->task.ID, p->task.timp_exec_ramas);
    }
}

void get_task(int task_id, Queue *waiting, Queue *running, Queue *finished)
{
    Queue *p = waiting;
    while (p != NULL)
    {
        if (p->task.ID == task_id)
        {
            printf("Task %d is waiting (remaining_time = %d).\n", task_id, p->task.timp_exec_ramas);
            return;
        }
        p = p->next;
    }
    p = running;
    while (p != NULL)
    {
        if (p->task.ID == task_id)
        {
            printf("Task %d is running (remaining_time = %d).\n", task_id, p->task.timp_exec_ramas);
            return;
        }
        p = p->next;
    }
    p = finished;
    while (p != NULL)
    {
        if (p->task.ID == task_id)
        {
            printf("Task %d is finished.\n", task_id);
            return;
        }
        p = p->next;
    }
    printf("Task %d not found.", task_id);
}

void run(Queue **waiting, Queue **running, Queue **finished, int run_time, int *nr_threads_disp, int *threads_disp, int N, int *total_time)
{
    if (*running == NULL)
        complete_threads(waiting, running, nr_threads_disp, threads_disp, N);

    Queue *p = *running;
    while (p != NULL)
    {
        p->task.timp_exec_ramas -= run_time;
        p = p->next;
    }

    p = *running;
    while (p != NULL && p->task.timp_exec_ramas <= 0)
    {
        Queue *node = pop_from_queue(running);
        threads_disp[node->task.thread.ID] = 0;
        add_to_queue(finished, node);
        p = *running;
        (*nr_threads_disp)++;
    }

    complete_threads(waiting, running, nr_threads_disp, threads_disp, N);

    *total_time += run_time;
}

void free_queue(Queue *first)
{

    Queue *p = first;
    Queue *next;

    while (p != NULL)
    {
        next = p->next;
        free(p);
        p = next;
    }

    first = NULL;
}

void free_stack(Stack *first)
{

    Stack *p = first;
    Stack *next;

    while (p != NULL)
    {
        next = p->next;
        free(p);
        p = next;
    }

    first = NULL;
}

int main()
{
    FILE *file = fopen("file.in", "r");
    int Q, C, N, i, total_time = 0;
    char input[100], command[10] = "\0";
    fscanf(file, "%d%d", &Q, &C);
    N = 2 * C;

    Stack *top = NULL;

    for (i = N - 1; i >= 0; i--)
    {
        Stack *new_node_s = init_node_s(i);
        add_to_stack(&top, new_node_s);
    }

    Queue *waiting = NULL, *running = NULL, *finished = NULL;

    int id_disp[32767] = {0}, *threads_disp = (int *)malloc(N * sizeof(int)), nr_threads_disp = N;

    for (i = 0; i < N; i++)
        threads_disp[i] = 0;

    while (fgets(input, 100, file))
    {
        sscanf(input, "%s", command);

        if (strcmp(command, "add_tasks") == 0)
        {
            int nr_tasks, time, priority;
            sscanf(input + strlen(command), "%d%d%d", &nr_tasks, &time, &priority);

            for (i = 0; i < nr_tasks; i++)
            {
                int id = 1;

                while (id_disp[id] == 1)
                    id++;

                id_disp[id] = 1;
                Queue *new_node_q = init_node_q(id, priority, N, time);
                add_to_queue(&waiting, new_node_q);
                printf("Task created successfully : ID %d.\n", id);
            }
        }
        if (strcmp(command, "print") == 0)
        {
            sscanf(input + strlen(command), "%s", command);

            if (strcmp(command, "waiting") == 0)
                print_waiting(waiting);
            if (strcmp(command, "running") == 0)
                print_running(running);
            if (strcmp(command, "finished") == 0)
                print_finished(finished);
        }
        if (strcmp(command, "run") == 0)
        {
            int run_time;
            sscanf(input + strlen(command), "%d", &run_time);

            printf("Running tasks for %d ms…\n", run_time);
            run(&waiting, &running, &finished, run_time, &nr_threads_disp, threads_disp, N, &total_time);
        }
        if (strcmp(command, "get_thread") == 0)
        {
            int thread_id;
            sscanf(input + strlen(command), "%d", &thread_id);
            get_thread(thread_id, running, threads_disp);
        }
        if (strcmp(command, "get_task") == 0)
        {
            int task_id;
            sscanf(input + strlen(command), "%d", &task_id);
            get_task(task_id, waiting, running, finished);
        }
        if (strcmp(command, "finish") == 0)
        {
            while (waiting != NULL)
            {
                run(&waiting, &running, &finished, running->task.timp_exec_ramas, &nr_threads_disp, threads_disp, N, &total_time);
            }
            Queue *last = running;
            while ((last->next != NULL))
                last = last->next;

            run(&waiting, &running, &finished, last->task.timp_exec_ramas, &nr_threads_disp, threads_disp, N, &total_time);
            printf("Total time: %d\n", total_time);
            break;
        }
    }

    free(threads_disp);
    free_queue(running);
    free_queue(waiting);
    free_queue(finished);
    free_stack(top);
    fclose(file);

    return 0;
}