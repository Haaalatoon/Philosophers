#include "philo.h"

// Initialize circular buffer queue
int queue_init(t_queue *queue, int capacity)
{
    queue->buffer = malloc(sizeof(int) * capacity);
    if (!queue->buffer)
        return (1);
    
    queue->head = 0;
    queue->tail = 0;
    queue->size = 0;
    queue->capacity = capacity;
    return (0);
}

// Cleanup queue memory
void queue_destroy(t_queue *queue)
{
    if (queue->buffer)
    {
        free(queue->buffer);
        queue->buffer = NULL;
    }
}

// Add philosopher ID to end of queue
int queue_enqueue(t_queue *queue, int philosopher_id)
{
    if (queue->size >= queue->capacity)
        return (1); // Queue full
    
    queue->buffer[queue->tail] = philosopher_id;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;
    return (0);
}

// Remove philosopher ID from front of queue
int queue_dequeue(t_queue *queue)
{
    int philosopher_id;
    
    if (queue->size <= 0)
        return (-1); // Queue empty
    
    philosopher_id = queue->buffer[queue->head];
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;
    return (philosopher_id);
}

// Look at front of queue without removing
int queue_peek(t_queue *queue)
{
    if (queue->size <= 0)
        return (-1);
    
    return (queue->buffer[queue->head]);
}

// Check if queue is empty
int queue_is_empty(t_queue *queue)
{
    return (queue->size == 0);
}

// Get current queue size
int queue_size(t_queue *queue)
{
    return (queue->size);
}

// Check if philosopher_id is already in the queue
int queue_contains(t_queue *queue, int philosopher_id)
{
    int i;
    for (i = 0; i < queue->size; i++)
    {
        int idx = (queue->head + i) % queue->capacity;
        if (queue->buffer[idx] == philosopher_id)
            return 1;
    }
    return 0;
}