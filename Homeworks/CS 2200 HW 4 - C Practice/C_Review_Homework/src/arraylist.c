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

}

/**
 * Append a char pointer to the end of the arraylist.
 * Backing array must be resized as indexing outside of the array will cause a segmentation fault
 *
 * @param arraylist the arraylist to be modified
 * @param data a pointer to the data that will be added
 */
void append(arraylist_t *arraylist, char *data) {

}

/**
 * Remove a char * from arraylist at specified index.
 * @param arraylist the arraylist to be modified
 * @param index the location that data will be removed from in the arraylist
 * @return the char * that was removed
 */
char *remove_from_index(arraylist_t *arraylist, int index) {

}

/**
 * OPTIONAL: This method does not need to be implemented. This is a useful helper method that could be handy
 * if you need to resize your arraylist internally. However, this method is not used outside of the arraylist.c file.
 * Resize the backing array to hold arraylist->capacity * 2 elements.
 * @param arraylist the arraylist to be resized
 */
void resize(arraylist_t *arraylist) {

}

/**
 * Destroys the arraylist by freeing the backing array.
 * @param arraylist the arraylist to be destroyed
 */
void destroy(arraylist_t *arraylist) {

}