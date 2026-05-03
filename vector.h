/*
Vincente Valdez
900366314
3.2.26 2:30 pm
*/
//Labeling parameters with both in & out didn't work in IDE, labeled with out
#pragma once

/**
 * @brief hello
 *
 * @member
 */
typedef struct
{
    int *data;
    size_t capacity;
    size_t size;
} vector_t;

/**
 *@brief Adds a new integer to the end of the vector, doubling the size of the vector if necessary.

 *@param[out] vector: pointer to the vector to which the new integer will be added
 *@param[in] newItem: the integer to be added to the vector
 */
void push_back(vector_t *vector, int newItem);

/**
 *@brief Finds the integer at the given index in the vector and returns a pointer to it. If the index is out of bounds, returns NULL.
 *
 *@param[in] vector: the vector from which to retrieve the integer
 *@param[in] index: the index of the integer to retrieve
 *
 *@return A pointer to the integer at the given index, or NULL if the index is out of bounds.
 */
int *at(const vector_t *vector, int index);

/**
 *@brief Creates a new vector with an initial capacity and size of 0, and a null data pointer.
 *
 *@return The newly created vector.
 */
vector_t create_vector(void);

/**
 *@brief Removes the last integer from the vector, reducing the size of the vector by 1/4 if the size falls below half of the capacity.
 *
 *@param[out] vector: pointer to the vector from which the last integer will be removed
 */
void pop_back(vector_t *vector);

/**
 *@brief Frees the memory allocated for the vector's data. Resets the vector's size and capacity to 0 and data pointer to NULL.
 *
 *@param[out] vector: pointer to the vector to be erased
 */
void erase(vector_t *vector);
