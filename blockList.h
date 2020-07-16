#include "blockStruct.h"
#include <stdlib.h>

typedef struct blockNode {
  BLOCK_T block;
  struct blockNode* next;
} BlockNode;

typedef struct blockList {
  BlockNode* head;
  BlockNode* tail;
} BlockList;

void InitializeBlockList(BlockList* i_BlockList);
void AddBlockToList(BlockList* i_BlockList, BLOCK_T* i_Block);
void AddBlockNodeToList(BlockList* i_BlockList, BlockNode* i_BlockNode);
