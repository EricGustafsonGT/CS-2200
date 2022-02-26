/**
 * Name: Eric Gustafson
 * GTID: 903477896
 */

/*  PART 2: A CS-2200 C implementation of the arraylist data structure.
    Implement an array list.
    The methods that are required are all described in the header file. Description for the methods can be found there.

    Hint 1: Review documentation/ man page for malloc, calloc, and realloc.
    Hint 2: Review how an arraylist works.
    Hint 3: You can use GDB if your implementation causes segmentation faults.
*/

#include "arraylist.h"

//typedef unsigned int uint;
//typedef struct arraylist
//{
//    uint capacity;
//    uint size;
//    char **backing_array;
//} arraylist_t;

/**
 * Create an arraylist data structure with a backing array of type char **
 * (an array of char *). Both the backing array and the struct arraylist
 * must be freed after use.
 * Backing array must be located in the heap!
 *
 * @param capacity the initial length of the backing array
 * @return pointer to the newly created struct arraylist
 */
arraylist_t *create_arraylist(uint capacity) {
    //First let's allocate memory for the arraylist
    arraylist_t *list = calloc(1, sizeof(arraylist_t));
    if (list == NULL) {
        free(list);
        return NULL;
    }

    list->backing_array = calloc(capacity, sizeof(char *));
    if (list->backing_array == NULL) {
        destroy(list);
        return NULL;
    }

    list->capacity = capacity;
    return list;
}

/**
 * Add a char * at the specified index of the arraylist.
 * Backing array must be resized as indexing outside of the array will cause a segmentation fault.
 *
 * @param arraylist the arraylist to be modified
 * @param data a pointer to the data that will be added
 * @param index the location that data will be placed in the arraylist
 */
void add_at_index(arraylist_t *arraylist, char *data, int index) {
    if (arraylist == NULL || data == NULL) {
        return;
    }

    //Resize if needed
    if (arraylist->size == arraylist->capacity) {
        resize(arraylist); //should reallocate memory and update the capacity field of the arraylist

        //if the arraylist's size and capacity are still the same, then it means that with resize() the realloc()
        //call was not successful because we ran out of memory. Now, we deallocate arraylist and return
        if (arraylist->size == arraylist->capacity) {
            destroy(arraylist);
            return;
        }
    }

    //shift data so we can insert the new data
    for (uint i = arraylist->size; i > index; i--) {
        arraylist->backing_array[i] = arraylist->backing_array[i - 1];
    }

    arraylist->backing_array[index] = data; //add element at the specified index
    arraylist->size += 1; //increment size
}

/**
 * Append a char pointer to the end of the arraylist.
 * Backing array must be resized as indexing outside of the array will cause a segmentation fault
 *
 * @param arraylist the arraylist to be modified
 * @param data a pointer to the data that will be added
 */
void append(arraylist_t *arraylist, char *data) {
    add_at_index(arraylist, data, (int) arraylist->size); //literally just an add to the back
}

/**
 * Remove a char * from arraylist at specified index.
 * @param arraylist the arraylist to be modified
 * @param index the location that data will be removed from in the arraylist
 * @return the char * that was removed
 */
char *remove_from_index(arraylist_t *arraylist, int index) {
    if (arraylist == NULL) {
        return NULL;
    }
    char *element_to_remove = arraylist->backing_array[index];

    //shift all the elements back one
    for (uint i = index; i < arraylist->size - 1; i++) {
        arraylist->backing_array[i] = arraylist->backing_array[i + 1];
    }
    arraylist->backing_array[arraylist->size] = NULL; //set last element to NULL since we have removed an element

    arraylist->size--; //decrement the size by 1
    return element_to_remove;
}

/**
 * OPTIONAL: This method does not need to be implemented. This is a useful helper method that could be handy
 * if you need to resize your arraylist internally. However, this method is not used outside of the arraylist.c file.
 * Resize the backing array to hold arraylist->capacity * 2 elements.
 * @param arraylist the arraylist to be resized
 */
void resize(arraylist_t *arraylist) {
    if (arraylist == NULL) {
        return;
    }

    //reallocate the arraylist pointer's buffer to the new capacity
    arraylist->backing_array = realloc(arraylist->backing_array, 2 * arraylist->capacity * sizeof(char *));

    //check if realloc() was not successful
    if (arraylist == NULL) {
        return;
    }

    //update the new capacity
    arraylist->capacity *= 2;
}

/**
 * Destroys the arraylist by freeing the backing array.
 * @param arraylist the arraylist to be destroyed
 */
void destroy(arraylist_t *arraylist) {
    if (arraylist == NULL) {
        return;
    }

    //Free the allocated memory. THE BACKING ARRAY MUST BE FREED FIRST. The reason for this is because if we free
    //the list first, then we cannot free the backing array because we have lost the pointer to the list which points
    //the pointer to the backing array.
    free(arraylist->backing_array);
    free(arraylist);
}