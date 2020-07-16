#include "messageQueueList.h"

void InitializeMsgQueueList(MessageQueueList* i_MessageQueueList)
{
  i_MessageQueueList->head  = NULL;
  i_MessageQueueList->tail = NULL;
}

void AddMessageQueueToList(MessageQueueList* i_MessageQueueList, mqd_t i_MinersMessageQueues)
{
    MessageQueueNode* messageQueueNode = (MessageQueueNode*)calloc(1, sizeof(MessageQueueNode));

    messageQueueNode->minerMessageQueue = i_MinersMessageQueues;
    AddMessageQueueNodeToList(i_MessageQueueList, messageQueueNode);
}

void AddMessageQueueNodeToList( MessageQueueList* i_MessageQueueList, MessageQueueNode* i_MessageQueueNode)
{
      if (i_MessageQueueList->head != NULL)
      {
        i_MessageQueueList->tail->next = i_MessageQueueNode;
        i_MessageQueueList->tail = i_MessageQueueList->tail->next;
      }
      else
      {
        i_MessageQueueList->head = i_MessageQueueList->tail = i_MessageQueueNode;
      }
}
