#include "blockList.h"

void AddBlockNodeToList(BlockList* i_BlockList, BlockNode* i_BlockNode)
{
  if (i_BlockList->head != NULL)
  {
    i_BlockList->tail->next = i_BlockNode;
    i_BlockList->tail = i_BlockList->tail->next;
  }
  else
  {
    i_BlockList->head = i_BlockList->tail = i_BlockNode;
  }
}


void AddBlockToList(BlockList* i_BlockList, BLOCK_T* i_Block)
{
  BlockNode* blockNode = (BlockNode*)calloc(1, sizeof(BlockNode));
  blockNode->block = *i_Block;
  AddBlockNodeToList(i_BlockList, blockNode);
}

void InitializeBlockList(BlockList* i_BlockList)
{
  i_BlockList->head = NULL;
  i_BlockList->tail = NULL;
}
