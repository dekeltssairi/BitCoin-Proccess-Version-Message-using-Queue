//----------------includes---------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <zlib.h>
#include "blockStruct.h"
#include "connectionRequest.h"
#include "message.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include "commonVariables.h"
#include <stdbool.h>
#define MAX_MSG 10






void StartMining(mqd_t i_ServerQueueDescriptor, mqd_t i_MinerQueueDescriptor, int i_MinerId);
void ConnectToServer(mqd_t i_ServerQueueDescriptor, FLEXIBLE_Connection_Request* i_ConnectionRequest, int i_FlexbileConnectionRequestSize);
void receiveUpdatedBlock( mqd_t i_MinerQueueDescriptor, BLOCK_T* io_Block, int i_MinerId);
void MiningBlock(BLOCK_T* i_CurrentBlockUnderWork, mqd_t i_MinerQueueDescriptor);
void MiningHash(BLOCK_T* i_CurrentBlockUnderWork, mqd_t i_MinerQueueDescriptor);
void SetCurrentTimeStamp(BLOCK_T* i_Block);
char* GetMinerQueueName(char* i_MinerId);
unsigned int MakeHashFromStruct(BLOCK_T* i_Block);
