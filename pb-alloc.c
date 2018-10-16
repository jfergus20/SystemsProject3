// =================================================================================================================================
/**
 * pb-malloc.c
 *
 * A pointer-bumping, non-reclaiming memory alligator.
 **/
// =================================================================================================================================



// =================================================================================================================================
// INCLUDES

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
// =================================================================================================================================



// =================================================================================================================================
// CONSTANTS AND MACRO FUNCTIONS.

/** The system's page size. */
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

/** Macros to easily calculate the number of bytes for larger scales (e.g., kilo, mega, gigabytes). */
#define KB(size)  ((size_t)size * 1024)
#define MB(size)  (KB(size) * 1024)
#define GB(size)  (MB(size) * 1024)

/** The virtual address space reserved for the heap. */
#define HEAP_SIZE GB(2)

// =================================================================================================================================

typedef struct header{
  size_t size;
  struct header* next;
} header_t;

// =================================================================================================================================
// GLOBALS

/** The current beginning of free heap space. */
static void*    free_ptr  = NULL;

/** The beginning of the heap. */
static intptr_t start_ptr;

/** The end of the heap. */
static intptr_t end_ptr;

//declare head of linked list  header_t
static header_t link_head;

/** Points to the head of the list, changes every time free() is called +++ */
static intptr_t head_ptr;

/** Points to what head just pointed to +++ */
static intptr_t next_ptr;
// =================================================================================================================================



// =================================================================================================================================
/**
 * The initialization method.  If this is the first use of the heap, initialize it.
 */

void init () {

  // Only do anything if the heap has not yet been used.
  if (free_ptr == NULL) {

    // Allocate virtual address space in which the heap will reside; free space will be allocated from where it starts.  Make it
    // un-shared and not backed by any file.  A failure to map this space is fatal.
    free_ptr = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (free_ptr == MAP_FAILED) {
      write(STDOUT_FILENO, "mmap failed!\n", 13);
      exit(1);
    }

    // Hold onto the boundaries of the heap as a whole.
    start_ptr = (intptr_t)free_ptr;
    end_ptr   = start_ptr + HEAP_SIZE;

    // DEBUG: Emit a message to indicate that this alligator is being called.
    write(STDOUT_FILENO, "pb! Calling Alligator\n", 22);
    fsync(STDOUT_FILENO);

  }

} // init ()
// =================================================================================================================================

//WRITE STUFF BELOW

// =================================================================================================================================
/**
 * Allocate and return `size` bytes of heap space.
 *
 * \param size The number of bytes to allocate.
 * \return A pointer to the allocated block, if successful; `NULL` if unsuccessful.
 */

/**malloc() should preferentially select the first sufficiently large block from that list when an allocation is requested; if there is no such block in the free list, then it should use pointer-bumping to allocate a new portion of the heap*/

void* malloc (size_t size) {
  init();

  // B project
  void* new_ptr = free_ptr;
  free_ptr = free_ptr + size;

  return new_ptr;
  //  Or is it vv
  //  return free_ptr;

  // Is this really are there is to the B project?

  //=-=-=-=-=-=-=-=-=-=-=-=-=-=
  // A project

  // find the first sufficiently large block from list

  // if(there is a sufficiently large block in the free list){
  // select that block
  /**
     }else{
     void* new_ptr = free_ptr;
     free_ptr = free_ptr + size;
     }
 */

  // allocate CODE STARTS HERE Write a working, pointer-bumping alligator.  Be sure that you understand what has been set up in init().
  // Also note that calloc()/realloc() are fully written and depend on malloc(), so you need not touch them.
  

} // malloc()
// =================================================================================================================================

// =================================================================================================================================
/**
 * Deallocate a given block on the heap.  This function currently does nothing, leaving freed blocks unused.
 *
 * \param ptr A pointer to the block to be deallocated.
 */

//vvvvvvv
/** blocks passed to free() are placed onto a list and again made available for allocation */
//^^^^^^^^^^
void free (void* ptr) {

  // A Project
  // Keep this vvv commented out until malloc works, then test this
 /**
  intptr_t addr = (intptr_t)ptr; //casting this pointer as int
  addr = addr - 8;
  size_t* s = (size_t*)addr; //this is a ptr*
  size_t block_size = *s; // give me the value s points to
   */
  // reclaim/add to linked list

} // free()
// =================================================================================================================================

// WRITE CODE ABOVE THIS

// =================================================================================================================================
/**
 * Allocate a block of `nmemb * size` bytes on the heap, zeroing its contents.
 *
 * \param nmemb The number of elements in the new block.
 * \param size The size, in bytes, of each of the `nmemb` elements.
 * \return A pointer to the newly allocated and zeroed block, if successful; `NULL` if unsuccessful.
 */
void* calloc (size_t nmemb, size_t size) {

  // Allocate a block of the requested size.
  size_t block_size    = nmemb * size;
  void*  new_block_ptr = malloc(block_size);

  // If the allocation succeeded, clear the entire block.
  if (new_block_ptr != NULL) {
    memset(new_block_ptr, 0, block_size);
  }

  return new_block_ptr;
  
} // calloc ()
// =================================================================================================================================



// =================================================================================================================================
/**
 * Update the given block at `ptr` to take on the given `size`.  Here, if `size` is a reduction for the block, then the block is
 * returned unchanged.  If the `size` is an increase for the block, then a new and larger block is allocated, and the data from the
 * old block is copied, the old block freed, and the new block returned.
 *
 * \param ptr The block to be assigned a new size.
 * \param size The new size that the block should assume.
 * \return A pointer to the resultant block, which may be `ptr` itself, or may be a newly allocated block.
 */
void* realloc (void* ptr, size_t size) {

  // Special case:  If there is no original block, then just allocate the new one of the given size.
  if (ptr == NULL) {
    return malloc(size);
  }

  // Special case: If the new size is 0, that's tantamount to freeing the block.
  if (size == 0) {
    free(ptr);
    return NULL;
  }

  // Get the current block size from its header.
  size_t* header_ptr = (size_t*)((intptr_t)ptr - sizeof(size_t));
  size_t  block_size = *header_ptr;

  // If the new size isn't an increase, then just return the original block as-is.
  if (size <= block_size) {
    return ptr;
  }

  // The new size is an increase.  Allocate the new, larger block, copy the contents of the old into it, and free the old.
  void* new_block_ptr = malloc(size);
  if (new_block_ptr != NULL) {
    memcpy(new_block_ptr, ptr, block_size);
    free(ptr);
  }
    
  return new_block_ptr;
  
 } // realloc()
// =================================================================================================================================



#if !defined (PB_NO_MAIN)
// =================================================================================================================================
/**
 * The entry point if this code is compiled as a standalone program for testing purposes.
 */
void main () {

  // Allocate an array of 100 ints.
  int  size = 100;
  int* x    = (int*)malloc(size * sizeof(int));
  assert(x != NULL);

  // Assign some values.
  for (int i = 0; i < size; i += 1) {
    x[i] = i * 2;
  }

  // Print one of the values from the middle, just to show that it worked.
  //  printf("%d\n", x[size / 2]);
  printf("%d\n", x[size-1]);
} // main()
// =================================================================================================================================
#endif
