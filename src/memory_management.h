#include <sys/types.h>
#include <stdlib.h>

typedef struct BlockHeader
{
  size_t size;
  struct BlockHeader* prev;
  struct BlockHeader* next;
  u_int8_t free;
} BlockHeader;

static BlockHeader* head = NULL;

void * _malloc(size_t size);

void _free(void * ptr);
