#include "memory_management.h"
#include <unistd.h>
#include <stdio.h>

static void* checkCurrentBlocks(size_t size);
static void* createNewBlock(size_t size);

void* _malloc(size_t size)
{
	//If size is zero then return NULL
	if (!size)
		return NULL;

	//align memory to 8 bytes
	if (size % 8 != 0)
		size = size + (8 - (size % 8));

	//Check if any of the existing blocks have enough space for the newly requested block
	void* startOfBlock;
	if (startOfBlock = checkCurrentBlocks(size))
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

	BlockHeader* newBlockHeader = sbrk(size + sizeof(BlockHeader));

	BlockHeader* previousBlockHeader = NULL;
	if (head)
	{
		BlockHeader* currentBlock = head;
		for (; currentBlock->next != NULL; currentBlock = currentBlock->next);
		previousBlockHeader = currentBlock;
	}
	else
		head = newBlockHeader;
	

	newBlockHeader->size = size;
	newBlockHeader->prev = previousBlockHeader;
	newBlockHeader->next = NULL;
	newBlockHeader->free = 0;

	if (previousBlockHeader)
		previousBlockHeader->next = newBlockHeader;
	
	return (char*) newBlockHeader + sizeof(BlockHeader);
}

void _free(void* ptr)
{
	if(!ptr)
		return;
	
	//apply offset to ptr to get the associated BlockHeader
	BlockHeader* blockHeader = (BlockHeader*) ((char*) ptr - sizeof(BlockHeader));
	printf("\n\nBlockHeader: %p\nsize: %lu\nprev: %p\nnext: %p\nfree: %d\n", blockHeader, blockHeader->size, blockHeader->prev, blockHeader->next, blockHeader->free);

	//mark the block as free
	blockHeader->free = 1;

	//Move forward through list, merging blocks

	BlockHeader* currentBlock = blockHeader;
	BlockHeader* nextBlock = blockHeader->next;
	for (; nextBlock != NULL; nextBlock = nextBlock->next)
	{
		if (!(currentBlock->free && nextBlock->free))
			break;

		currentBlock->size += nextBlock->size + sizeof(BlockHeader);
		currentBlock->next = nextBlock->next;
		if (nextBlock->next)
			nextBlock->next->prev = currentBlock;
		currentBlock = nextBlock;
	}

	//Move backward through list, merging blocks

	currentBlock = blockHeader;
	BlockHeader* previousBlock = currentBlock->prev;
	for(; previousBlock != NULL; previousBlock = previousBlock->prev)
	{
		if (!(currentBlock->free && previousBlock->free))
			break;
		
		previousBlock->size += currentBlock->size + sizeof(BlockHeader);
		previousBlock->next = currentBlock->next;
		if (currentBlock->next)
			currentBlock->next->prev = previousBlock;
		currentBlock = previousBlock;
	}

	//TODO: RETURN BLOCKS TO THE OS

	printf("\nAfter merging list looks like:\n");
	for (BlockHeader* currentBlock = head; currentBlock != NULL; currentBlock = currentBlock->next)
		printf("\n\nBlockHeader: %p\nsize: %lu\nprev: %p\nnext: %p\nfree: %d\n", currentBlock, currentBlock->size, currentBlock->prev, currentBlock->next, currentBlock->free);
	
	//return free blocks at the end of the BlockHead list to the OS
	
	//move backwards from the end of the list, finding contigious free blocks to give back
	currentBlock = head;
	for (; currentBlock->next != NULL; currentBlock = currentBlock->next);
	BlockHeader* endBlock = currentBlock;
	size_t bytesToGiveBack = 0;
	for (currentBlock = endBlock; currentBlock != NULL; currentBlock = currentBlock->prev)
	{
		if (!(currentBlock->free))
			break;
		
		//if deallocating memory of whole list then reset head pointer
		if (!(currentBlock->prev))
			head = NULL;

		printf("bytesToGiveBack: %lu\n", bytesToGiveBack);
		bytesToGiveBack += currentBlock->size + sizeof(BlockHeader);
	}
	printf("Bytes being given back %lu\n", bytesToGiveBack);
	sbrk(-bytesToGiveBack);
}

