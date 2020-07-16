//----------------includes---------------------
#define _GNU_SOURCE
#include <sched.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "blockList.h"        /* For mode constants */
#include "messageQueueList.h"
#include "commonVariables.h"
#include "connectionRequest.h"
#include "nonFlexibleConnectionRequest.h"
#include "message.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <zlib.h>
#include <time.h>
#include <string.h>
#define MAX_MSG 10

unsigned int  MakeHashFromStruct(BLOCK_T* i_Block);
void MinigingBlock(BLOCK_T* i_Block);
void MiningHash(BLOCK_T* i_Block);
void SetCurrentTimeStamp(BLOCK_T* i_Block);
unsigned int MakeHashFromStruct(BLOCK_T* i_Block);
void NotifyMinersOfNewBlock(MessageQueueList* messageQueueList);
void UpdateBlockList(BLOCK_T* i_Block);
void CloseQueue(mqd_t i_ServerMessageQueue);
void ProvideService(MessageQueueList* i_MessageQueueList, mqd_t i_ServerMessageQueue);
void AcceptNewConnection(NFLEXIBLE_Connection_Request i_ConnectionRequest, MessageQueueList* i_MessageQueueList);
BLOCK_T* GenerateInitialGensisBlock();
mqd_t CreateServerQueue();
_Bool IsValidBlock(BLOCK_T* i_Block);
char* GetMinerQueueName(int i_MinerId);
char* itoa(int i_Num);
int GetLen(int i_Num);
