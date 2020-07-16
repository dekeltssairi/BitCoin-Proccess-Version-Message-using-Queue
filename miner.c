#include "miner.h"

#define assert_if(errnum) if (errnum <= 0) {perror("Error: %m\n"); exit(EXIT_FAILURE);}

int main(int argc, char* argv[])
{
  FLEXIBLE_Connection_Request*  minerInfo;
  int numberOfBytesReceived, minerId;
  mqd_t minerQueueDescriptor, serverQueueDescriptor;
  int minerQueueNameLen = strlen(argv[1]) + 12;
  char* nameOfMinerQueue = GetMinerQueueName(argv[1]);
  struct mq_attr queueAttr = {0};

  if(argc < 2)
  {
    return -1; /// inVALID NUMBER OF ARGUMENTS
  }
  //----------------------------make_miner_queues_name----------------//

  minerInfo = (FLEXIBLE_Connection_Request*) malloc(sizeof(FLEXIBLE_Connection_Request) + minerQueueNameLen);
  minerInfo->minerID = atoi(argv[1]);
  strcpy(minerInfo->queueName, nameOfMinerQueue);
  assert_if(minerInfo->minerID);

  queueAttr.mq_msgsize = sizeof(BLOCK_T);
  queueAttr.mq_maxmsg = MAX_MSG;

  //-------------------open_miner_queue---------------//

  mq_unlink(nameOfMinerQueue);
  minerQueueDescriptor = mq_open(nameOfMinerQueue, O_CREAT | O_NONBLOCK | O_RDONLY | O_EXCL, 0666, &queueAttr);
  printf("Miner id = %d, queue name = %s\n", minerInfo->minerID, nameOfMinerQueue);
  assert_if(minerQueueDescriptor);

  //-------------------open_server_queue---------------//
  do {
      serverQueueDescriptor = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
  } while(serverQueueDescriptor <= 0);
  assert_if(serverQueueDescriptor);


  ConnectToServer(serverQueueDescriptor, minerInfo, sizeof(FLEXIBLE_Connection_Request) + minerQueueNameLen);
  printf("Miner %d sent connect request on %s\n",minerInfo->minerID, nameOfMinerQueue);
  StartMining(serverQueueDescriptor, minerQueueDescriptor,minerInfo->minerID);

  return 0;
}


void StartMining(mqd_t i_ServerQueueDescriptor, mqd_t i_MinerQueueDescriptor, int i_MinerId)
{
  static int numberOfHashedBlocks = 0;
  BLOCK_T i_CurrentBlockUnderWork;
  MSG_T* messageToSend = (MSG_T*)malloc(sizeof(MSG_T) + sizeof(BLOCK_T));

  i_CurrentBlockUnderWork.nonce = -2;

  while(true)
  {
    receiveUpdatedBlock(i_MinerQueueDescriptor, &i_CurrentBlockUnderWork, i_MinerId);
    i_CurrentBlockUnderWork.relayed_by =  i_MinerId;
    MiningBlock(&i_CurrentBlockUnderWork, i_MinerQueueDescriptor);
    numberOfHashedBlocks++;
    messageToSend->type = HASHED_BLOCKED;
    messageToSend->dataSizeInBytes = sizeof(BLOCK_T);
    memcpy(messageToSend->data,&i_CurrentBlockUnderWork, messageToSend->dataSizeInBytes);
    printf("Miner #%d: Mined a new block %d, with the hash 0x%x, difficulty %d\n", i_MinerId, numberOfHashedBlocks, i_CurrentBlockUnderWork.hash, i_CurrentBlockUnderWork.difficulty);
    mq_send(i_ServerQueueDescriptor, (char*)messageToSend, sizeof(MESSAGE_TYPE) + MAX_SERVER_MSG_SIZE_DATA, 0);
  }
}

void ConnectToServer(mqd_t i_ServerQueueDescriptor, FLEXIBLE_Connection_Request* i_ConnectionRequest, int i_FlexbileConnectionRequestSize)
{
  MSG_T* messageToSend = (MSG_T*)malloc(sizeof(MESSAGE_TYPE) + i_FlexbileConnectionRequestSize);
  messageToSend->type = CONNECTION_REQUEST;
  messageToSend->dataSizeInBytes = i_FlexbileConnectionRequestSize;
  //strcpy(messageToSend->data, i_ConnectionRequest);
  memcpy(messageToSend->data, i_ConnectionRequest, i_FlexbileConnectionRequestSize );

  mq_send(i_ServerQueueDescriptor, (char*)messageToSend, sizeof(MESSAGE_TYPE) + MAX_SERVER_MSG_SIZE_DATA, 0);
}

void receiveUpdatedBlock( mqd_t i_MinerQueueDescriptor, BLOCK_T* io_Block, int i_MinerId)
{
  static int numOfBlocksRecieved = 0;
  int numberOfBytesReceived = -1;

  do {
    struct mq_attr attr;
    mq_getattr(i_MinerQueueDescriptor,&attr);

      numberOfBytesReceived = mq_receive(i_MinerQueueDescriptor, (char*)io_Block, sizeof(BLOCK_T), NULL);
      if (numberOfBytesReceived > 0)
      {
        numOfBlocksRecieved++;
        if(numOfBlocksRecieved == 1)
        {
          printf("Miner #%d recieved first block: relayed_by(%d), height(%d) , timestamp(%d), hash(0x%x), prev_hash(0x%x), difficulty(%d), nonce(%d) \n", i_MinerId, io_Block->relayed_by ,io_Block->height, io_Block->timestamp, io_Block->hash, io_Block->prev_hash, io_Block->difficulty, io_Block->nonce);
        }
        else
        {
          printf("Miner #%d recieved block: relayed_by(%d), height(%d) , timestamp(%d), hash(0x%x), prev_hash(0x%x), difficulty(%d), nonce(%d) \n", i_MinerId, io_Block->relayed_by, io_Block->height, io_Block->timestamp, io_Block->hash, io_Block->prev_hash, io_Block->difficulty, io_Block->nonce);
        }
        io_Block-> height =  io_Block-> height + 1;
        io_Block-> prev_hash =  io_Block-> hash;
      }

  } while(numberOfBytesReceived <= 0 && io_Block->nonce != -2 );
}

void MiningBlock(BLOCK_T* i_CurrentBlockUnderWork, mqd_t i_MinerQueueDescriptor)
{
  SetCurrentTimeStamp(i_CurrentBlockUnderWork);
  MiningHash(i_CurrentBlockUnderWork, i_MinerQueueDescriptor);
}

void MiningHash(BLOCK_T* i_CurrentBlockUnderWork, mqd_t i_MinerQueueDescriptor)
{
  int numberOfBytesReceived = -1;
  unsigned int mask = 0xffff0000;
  unsigned int crcHash;
  int minerId = i_CurrentBlockUnderWork->relayed_by;

  do {
        i_CurrentBlockUnderWork-> nonce =  i_CurrentBlockUnderWork-> nonce + 1;
        crcHash = MakeHashFromStruct(i_CurrentBlockUnderWork);
        if ((mask & crcHash) != 0)
        {
          numberOfBytesReceived = mq_receive(i_MinerQueueDescriptor, (char*)i_CurrentBlockUnderWork, sizeof(BLOCK_T), NULL);
          if (numberOfBytesReceived >= 0)
          {
            SetCurrentTimeStamp(i_CurrentBlockUnderWork);
            i_CurrentBlockUnderWork-> height = i_CurrentBlockUnderWork-> height + 1;
            i_CurrentBlockUnderWork-> prev_hash =  i_CurrentBlockUnderWork-> hash;
            i_CurrentBlockUnderWork->relayed_by = minerId;
            i_CurrentBlockUnderWork->nonce = 0;
          }
        }
   } while((mask & crcHash) != 0);

   i_CurrentBlockUnderWork-> hash = crcHash;

   return;
}

void SetCurrentTimeStamp(BLOCK_T* i_Block)
{
  time_t seconds = time(NULL);
  i_Block-> timestamp = seconds;
}

char* GetMinerQueueName(char* i_MinerId)
{
  char* nameOfMinerQueue = (char*)malloc(sizeof(char) * (12 + strlen(i_MinerId)));
  strcpy(nameOfMinerQueue, "/miner_");
  strcat(nameOfMinerQueue, i_MinerId);
  strcat(nameOfMinerQueue, "_q\0");

  return nameOfMinerQueue;
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
