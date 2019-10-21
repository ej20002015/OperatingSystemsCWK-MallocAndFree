# Operating systems coursework repository

This repository contains the template files for the memory management coursework
for Operating Systems (COMP2211). The coursework specification sheet is available
on Minerva.

## malloc
The ``malloc`` function allocates the specified number of bytes and returns a
pointer to the allocated memory. The memory is not initialized. If the requested
size is 0, then ``_malloc()`` returns either ``NULL``, or a unique pointer value that
can later be successfully passed to free.
* the function should have the prototype ``void * _malloc(size_t size)``
* the function should return a void pointer to the start of the allocated memory
* the function should allocate memory on the heap
* the pointer returned should be suitably aligned for any kind of variable (advanced)

## free
The ``free`` function frees the memory space pointed to by the parameter which 
must have been returned by a previous call to ``_malloc()``. Otherwise, or if ``free`` 
has already been called on the pointer before, undefined behavior occurs. If the
pointer is NULL, no operation is performed.
* the function should have the prototype ``void _free(void *ptr)``
* the function should "free” the memory and make it available for subsequent allocations
* the function should return the memory to the operating system when it is appropriate (advanced)

# Makefile

There is a makefile included in the repositroy which is capable of making and testing
your submission. You will need the following libraries installed for the test 
suite to run fully:
*  make 
*  libsubunit-dev
*  libsubunit0
*  check
*  python3 
*  python3-pip
*  pip3 install gcovr

If you don't want to install all of the dependencies you can check if your code 
work by checking the artefacts of the continuous integration tests in gitlab.

### Makefile targets
``make`` will make, test and produce a coverage report for your submission

``make memory_tests`` will make the executable which tests your code 

``make test`` will make and run the test suite

``make zip`` will create a zip for submission (ready for uploading to Minerva)
