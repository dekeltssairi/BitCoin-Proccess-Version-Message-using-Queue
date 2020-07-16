#include "server.h"

BlockList blockList;

int main(int argc, char* argv[])
{
  struct sched_param max_priority = {sched_get_priority_max(SCHED_RR)};
  pthread_setschedparam(pthread_self(), SCHED_RR, &max_priority);
  mqd_t serverMessageQueue = CreateServerQueue();
  MessageQueueList* messageQueueList = (MessageQueueList*)calloc(1,sizeof(MessageQueueList));

  ProvideService(messageQueueList,serverMessageQueue);
  CloseQueue(serverMessageQueue);

  return 0;
}

void AcceptNewConnection(NFLEXIBLE_Connection_Request i_ConnectionRequest, MessageQueueList* i_MessageQueueList)
{
  printf("Received connection request from miner id %d, queue name  %s\n", i_ConnectionRequest.minerID, i_ConnectionRequest.queueName);
  BLOCK_T blockToSend;
  mqd_t minerMessageQueue = mq_open(i_ConnectionRequest.queueName, O_WRONLY | O_NONBLOCK);
  AddMessageQueueToList(i_MessageQueueList, minerMessageQueue);
  blockToSend = blockList.tail->block;
  mq_send(minerMessageQueue,(char*)&blockToSend,sizeof(BLOCK_T),0);
}

void ProvideService(MessageQueueList* i_MessageQueueList, mqd_t i_ServerMessageQueue)
{
  int minerId;
  ssize_t numberOfBytesRead;
  size_t messageSize = MAX_SERVER_MSG_SIZE_DATA + sizeof(MSG_T);
  MSG_T* msg = (MSG_T*)calloc(1,sizeof(MSG_T) + MAX_SERVER_MSG_SIZE_DATA);
  BLOCK_T* gensisBlock = GenerateInitialGensisBlock();
  AddBlockToList(&blockList, gensisBlock);
  printf("Listening on %s\n",SERVER_QUEUE_NAME);
  while(true)
  {
    numberOfBytesRead = mq_receive(i_ServerMessageQueue, (char*)msg, messageSize, NULL);

    // -----------------Finished---------------------------
    if (msg->type == CONNECTION_REQUEST)
    {
      NFLEXIBLE_Connection_Request nfConnectionRequest;
      FLEXIBLE_Connection_Request* fConnectionRequest = malloc(msg->dataSizeInBytes);
      memcpy(fConnectionRequest, msg->data, msg->dataSizeInBytes);

      nfConnectionRequest.queueName = (char*)malloc(msg->dataSizeInBytes - sizeof(FLEXIBLE_Connection_Request));
      memcpy(nfConnectionRequest.queueName, fConnectionRequest->queueName,  msg->dataSizeInBytes - sizeof(FLEXIBLE_Connection_Request));
      nfConnectionRequest.minerID = fConnectionRequest->minerID;
      minerId = nfConnectionRequest.minerID;

      AcceptNewConnection(nfConnectionRequest,i_MessageQueueList);
    }
    else
    {


      BLOCK_T* block = (BLOCK_T*)malloc(msg->dataSizeInBytes);
      memcpy(block, msg->data, msg->dataSizeInBytes);
      minerId = block->relayed_by;
      if (IsValidBlock(block))
      {
        UpdateBlockList(block);
        printf("Server added new block by miner #%d: height(%d) , timestamp(%d), hash(0x%x), prev_hash(0x%x), difficulty(%d), nonce(%d) \n", minerId, block->height, block->timestamp, block->hash, block->prev_hash, block->difficulty, block->nonce);
        NotifyMinersOfNewBlock(i_MessageQueueList);
      }
    }
  }
}

_Bool IsValidBlock(BLOCK_T* i_Block)
{
  BLOCK_T lastMinedBLock = blockList.tail->block;
  bool isValidBlock = false;
  unsigned int crcHash, mask = 0xffff0000;
  bool isMinerHashHaveLeadingZeros = (i_Block->hash & mask) == 0;

  if (isMinerHashHaveLeadingZeros)
  {
    crcHash = MakeHashFromStruct(i_Block);
    bool isHashIdentical = i_Block->hash == crcHash;
    bool isHeightIdentical  = i_Block->height  == lastMinedBLock.height + 1;
    bool isPrevHashIdentical  = i_Block->prev_hash == lastMinedBLock.hash;

    isValidBlock = isHashIdentical && isHeightIdentical && isPrevHashIdentical;
  }

  return isValidBlock;
}

unsigned int MakeHashFromStruct(BLOCK_T* i_Block)
{
    unsigned long crcHash = 0;
    int valueInStruct;

    for (size_t i = 0; i < 5; i++) {

      valueInStruct = (int)*((int*)i_Block + i);
      crcHash = crc32(crcHash, (const void*)&valueInStruct,sizeof(int));
    }

    return crcHash;
}

//--------------------finished@---------------------------------
void CloseQueue(mqd_t i_ServerMessageQueue)
{
  mq_close(i_ServerMessageQueue);
  mq_unlink(SERVER_QUEUE_NAME);
}


mqd_t CreateServerQueue()
{
  mqd_t serverMessageQueue;

  struct mq_attr queueAttr = {0};
  queueAttr.mq_msgsize =  MAX_SERVER_MSG_SIZE_DATA + sizeof(MSG_T);
  queueAttr.mq_maxmsg = MAX_MSG;
  mq_unlink(SERVER_QUEUE_NAME);
  printf("Listening on %s\n", SERVER_QUEUE_NAME);
  serverMessageQueue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY, 0666 , &queueAttr);
  return serverMessageQueue;
}


BLOCK_T* GenerateInitialGensisBlock()
{
  BLOCK_T* newBlockT = (BLOCK_T*)calloc(1,sizeof(BLOCK_T));

  newBlockT->difficulty = DIFFICULTY;
  newBlockT-> nonce = -1;
  newBlockT-> hash = 0;
  newBlockT-> prev_hash = 0;
  MinigingBlock(newBlockT);

  return newBlockT;
}

void UpdateBlockList(BLOCK_T* i_Block)
{
  AddBlockToList(&blockList, i_Block);
}

void NotifyMinersOfNewBlock(MessageQueueList* messageQueueList)
{
  MessageQueueNode* currentNode = messageQueueList->head;
  BLOCK_T messageToSend = blockList.tail->block;
  mqd_t currentNodesMessageQueueDescriptor;
  while(currentNode)
  {
    currentNodesMessageQueueDescriptor = currentNode->minerMessageQueue;
    mq_send(currentNodesMessageQueueDescriptor,(char*)&messageToSend,sizeof(messageToSend),0 );
    currentNode = currentNode->next;
  }
}

void MinigingBlock(BLOCK_T* i_Block)
{
  SetCurrentTimeStamp(i_Block);
  MiningHash(i_Block);
}

void MiningHash(BLOCK_T* i_Block)
{
  unsigned int mask = 0xffff0000;
  unsigned int crcHash;

  do {
        i_Block-> nonce =  i_Block-> nonce + 1;
        crcHash = MakeHashFromStruct(i_Block);
   } while((mask & crcHash) != 0);

   i_Block-> hash = crcHash;
}


void SetCurrentTimeStamp(BLOCK_T* i_Block)
{
  time_t seconds = time(NULL);
  i_Block-> timestamp = seconds;
}


char* GetMinerQueueName(int i_MinerId)
{
  char* minerId = itoa(i_MinerId);
  char* nameOfMinerQueue = (char*)malloc(sizeof(char) * (12 + strlen(minerId)));
  strcpy(nameOfMinerQueue, "/miner_");
  strcat(nameOfMinerQueue, minerId);
  strcat(nameOfMinerQueue, "_q\0");

  return nameOfMinerQueue;
}



char* itoa(int i_Num)
{
  int len = GetLen(i_Num);
  char* str = (char*)malloc(sizeof(char) * (len+ 1));

  sprintf(str, "%d", i_Num);
  str[len] = '\0';

  return str;
}



int GetLen(int i_Num)
{
  if ( i_Num > 0 )
  {
    return  GetLen(i_Num/10) + 1;
  }

  return 0;
}
