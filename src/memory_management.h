#include <sys/types.h>
#include <stdlib.h>

typedef struct blockHeader
{
  size_t size;
  struct blockHeader* prev;
  struct blockHeader* next;
  u_int8_t free;
} blockHeader;

static blockHeader* head = NULL;

void * _malloc(size_t size);

void _free(void * ptr);
