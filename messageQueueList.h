#include <mqueue.h>
#include <stdlib.h>
typedef struct messageQueueNode {
  mqd_t minerMessageQueue;
  struct messageQueueNode* next;
} MessageQueueNode;

typedef struct messageQueueList {
  MessageQueueNode* head;
  MessageQueueNode* tail;
} MessageQueueList;

void InitializeMsgQueueList(MessageQueueList* i_MessageQueueList);
void AddMessageQueueToList(MessageQueueList* i_MessageQueueList, mqd_t i_MinersMessageQueues);
void AddMessageQueueNodeToList(MessageQueueList* i_MessageQueueList, MessageQueueNode* i_MessageQueueNode);
