/*
Vincente Valdez
900366314
3.11.26 4:07pm
*/
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

void push_back(vector_t *vector, int newItem)
{
    if (vector == NULL)
    {
        printf("Error: Vector doesn't exist");
        return;
    }

    if (vector->capacity > __SIZE_MAX__ / 2)
    {
        // Prevents integer overflow when doubling the capacity of the vector
        printf("Error: vector is at maximum capacity.\n");
        return;
    }

    if (vector->capacity == 0)
    {
        // Initializes the vector with a capacity of 2 if the vector is empty
        int *newData = malloc(sizeof(int) * 2);
        if (newData == NULL)
        {
            printf("Error: Malloc Failed\n");
            return;
        }
        vector->data = newData;

        vector->capacity = 2;
    }
    else if (vector->size == vector->capacity)
    {
        // Doubles the capacity of the vector if the size of the vector is equal to the capacity
        int newCapacity = vector->capacity * 2;

        if (newCapacity > (__SIZE_MAX__ / sizeof(int)))
        {
            printf("Error: Requested memory exceeds system limit.");
            return;
        }

        int *newData = realloc(vector->data, sizeof(int) * newCapacity);
        if (newData == NULL)
        {
            printf("Error: Malloc Failed\n");
            return;
        }

        vector->data = newData;
        vector->capacity = newCapacity;
    }

    vector->data[vector->size] = newItem;
    vector->size++;
    // Adds the new item to the end of the vector and increments the size of the vector
}

int *at(const vector_t *vector, int index)
{
    if (index < 0)
    {
        // Prevents negative index values
        printf("Error: index cannot be negative.\n");
        return NULL;
    }

    if (index > vector->size - 1)
    {
        // Prevents index values that are out of bounds
        printf("Error: index is out of bounds.\n");
        return NULL;
    }

    return &vector->data[index];
}

void pop_back(vector_t *vector)
{
    if (vector->data == NULL)
    {
        // Prevents access to an empty vector
        printf("Error: vector is empty.\n");
        return;
    }

    vector->size--;
    // Removes the last item from the vector and decrements the size of the vector

    if (vector->size < vector->capacity / 2)
    {
        // Reduces the capacity of the vector by 1/4 if the size of the vector falls below half of the capacity
        int* newData = realloc(vector->data, sizeof(int) * (vector->capacity - vector->capacity / 4));
        if (newData == NULL)
        {
            print("Error: Realloc Failed\n");
            return;
        }
        
        vector->data = newData;
        vector->capacity = vector->capacity - (vector->capacity / 4);
    }
}

void erase(vector_t *vector)
{
    free(vector->data);

    vector->data = NULL;
    vector->capacity = 0;
    vector->size = 0;
}

vector_t create_vector(void)
{
    vector_t newVector;

    newVector.data = NULL;
    newVector.capacity = 0;
    newVector.size = 0;

    return newVector;
}