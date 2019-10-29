#include "memory_management.h"
#include <unistd.h>


void* _malloc(size_t size)
{
	if (!size)
		return NULL;

	void* startOfBlock = NULL;
	//walk linked list finding suitable blocks to place data
	for (blockHeader* currentBlock = head; currentBlock != NULL; currentBlock = currentBlock->next)
	{
		//if there is enough space in free block
		if (currentBlock->free && currentBlock->size >= size)
		{
			size_t sizeOfCurrentBlock = currentBlock->size;

			//calculate pointer to start of data and update block state
			startOfBlock = currentBlock + sizeof(blockHeader);
			currentBlock->size = size;
			currentBlock->free = 0;	

			//if size of the currentBlock is suitable then split the block into 2
			if (sizeOfCurrentBlock + sizeof(blockHeader) > size)
			{
				//calculate where to place the new blockHeader
				void* locationOfNewBlockHeader = currentBlock + sizeof(blockHeader) + size;

				//cast void* to blockHeader* so we can dereference it and create a new blockHeader
				blockHeader* newBlockHeader = (blockHeader*) locationOfNewBlockHeader;
				newBlockHeader->size = sizeOfCurrentBlock - sizeof(blockHeader) - size;
				newBlockHeader->prev = currentBlock;
				newBlockHeader->next = currentBlock->next;
				newBlockHeader->free = 1;
				
				//Update linked list to point to new blockHeader
				currentBlock->next = locationOfNewBlockHeader;
			}
		}
	}

	if (startOfBlock != NULL)
		return startOfBlock;

	//If no free blocks of suitable size can be found then request pages from the OS
	size_t systemPageSize = sysconf(_SC_PAGE_SIZE);
	void* startOfPages = sbrk(((size + sizeof(blockHeader)) / systemPageSize) + 1);

	//Add a new blockHeader to the start of the pages
	blockHeader* newBlock = (blockHeader*) startOfPages;
	newBlock->size = size;

	if (head == NULL)
	{
		newBlock->prev = NULL;
		head = newBlock;
	}
	else
	{
		//get previous block header in linked list
		blockHeader* currentBlock = head;
		for (; currentBlock->next != NULL; currentBlock = currentBlock->next);
		newBlock->prev = currentBlock;
	}

	newBlock->next = NULL;

	//mark the new block as being used
	newBlock->free = 0;

	return newBlock + sizeof(blockHeader);
}

void _free(void* ptr)
{

}

