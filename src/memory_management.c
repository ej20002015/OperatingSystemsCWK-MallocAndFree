#include "memory_management.h"
#include <unistd.h>
#include <stdio.h>

static void* checkCurrentBlocks(size_t size);
static void* createNewBlock(size_t size);
static void* expandHeap(size_t size);

//TODO: NEED TO CONVERT TO CHAR* BEFORE DOING POINTER ARITHMETIC WITH ABSOLUTE VALUES

void* _malloc(size_t size)
{
	//If size is zero then return NULL
	if (!size)
		return NULL;

	void* startOfBlock = NULL;

	//Check if any of the existing blocks have enough space for the newly requested block
	startOfBlock = checkCurrentBlocks(size);
	if (startOfBlock != NULL)
		return startOfBlock;
	
	//Create a new block and BlockHeader
	printf("Creating new block - not enough space in the existing free blocks\n");
	return createNewBlock(size);
}

static void* checkCurrentBlocks(size_t size)
{
	void* startOfBlock = NULL;
	//walk linked list finding suitable blocks to place data
	for (BlockHeader* currentBlock = head; currentBlock != NULL; currentBlock = currentBlock->next)
	{
		//if there is enough space in free block
		if (currentBlock->free && currentBlock->size >= size)
		{
			printf("Space in block %p\n", currentBlock);
			size_t sizeOfCurrentBlock = currentBlock->size;

			//calculate pointer to start of data and update block state
			startOfBlock = (char*) currentBlock + sizeof(BlockHeader);
			currentBlock->size = size;
			currentBlock->free = 0;	

			//if size of the currentBlock is great enough then split the block into 2
			if (sizeOfCurrentBlock + sizeof(BlockHeader) > size)
			{
				//calculate where to place the new BlockHeader
				void* locationOfNewBlockHeader = (char*) currentBlock + sizeof(BlockHeader) + size;

				//cast void* to BlockHeader* so we can dereference it and create a new BlockHeader
				BlockHeader* newBlockHeader = (BlockHeader*) locationOfNewBlockHeader;
				newBlockHeader->size = sizeOfCurrentBlock - sizeof(BlockHeader) - size;
				newBlockHeader->prev = currentBlock;
				newBlockHeader->next = currentBlock->next;
				newBlockHeader->free = 1;
				
				//Update linked list to point to new BlockHeader
				currentBlock->next = locationOfNewBlockHeader;
				printf("Splitting block %p, new block at %p\n", currentBlock, newBlockHeader);
			}
		}
	}
	return startOfBlock;
}

static void* createNewBlock(size_t size)
{
	//CHECK IF CURRENTLY ALLOCATED HEAP MEMORY IS LARGE ENOUGH TO CREATE NEW BLOCKHEADER

	if (head != NULL)
	{
		//get last block in the linked list
		BlockHeader* currentBlock = head;
		for (; currentBlock->next != NULL; currentBlock = currentBlock->next);
		printf("Previous BlockHeader location: %p\n", currentBlock);

		//if there is enough space in the currently mapped menu to add a new BlockHeader
		void* endOfNewBlockHeader = (char*) currentBlock + currentBlock->size + (2 * sizeof(BlockHeader));
		if (endOfNewBlockHeader <= endOfHeap)
		{
			void* startOfBlock = endOfNewBlockHeader - sizeof(BlockHeader);
			printf("Adding new block header at %p\n", startOfBlock);
			BlockHeader* newBlockHeader = (BlockHeader*) startOfBlock;
			newBlockHeader->size = size;
			newBlockHeader->prev = currentBlock;
			newBlockHeader->next = NULL;
			newBlockHeader->free = 0;

			currentBlock->next = newBlockHeader;

			//if the mapped memory is not large enough for the block
			size_t remainingSizeOfHeap = (char*) endOfHeap - (char*) endOfNewBlockHeader;
			if (remainingSizeOfHeap < size)
			{
				printf("Not enough space for block size - expanding heap\n");
				expandHeap(size - remainingSizeOfHeap);
			}
			
			return endOfNewBlockHeader;
		}
	}

	//IF NOT ENOUGH SPACE FOR BLOCKHEADER

	//map memory for new BlockHeader and Block
	BlockHeader* newBlock =  (BlockHeader*) expandHeap(size);
	printf("Not enough space for block header - expanding heap - creating new block header at %p\n", newBlock);

	newBlock->size = size;

	if (head == NULL)
	{
		newBlock->prev = NULL;
		head = newBlock;
	}
	else
	{
		//get previous block header in linked list
		BlockHeader* currentBlock = head;
		for (; currentBlock->next != NULL; currentBlock = currentBlock->next);
		newBlock->prev = currentBlock;
		currentBlock->next = newBlock;
	}

	newBlock->next = NULL;

	//mark the new block as being used
	newBlock->free = 0;

	return (char*) newBlock + sizeof(BlockHeader);
}

static void* expandHeap(size_t size)
{
	//if we require more memory than is mapped then memory is requested from the OS
	
	//request memory using sbrk system call
	void* startOfPages = sbrk(size);

	//update end of heap pointer
	long systemPageSize = sysconf(_SC_PAGE_SIZE);
	unsigned pagesToAdd = ((size + sizeof(BlockHeader)) / systemPageSize) + 1;

	endOfHeap = (char*) startOfPages + (systemPageSize * pagesToAdd);
	printf("End of heap: %p\n", endOfHeap); //doesn't work
	return startOfPages;
}

void _free(void* ptr)
{

}

