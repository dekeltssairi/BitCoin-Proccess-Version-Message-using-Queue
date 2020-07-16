
//-------------INCLUDES-------------
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
//-------------DEFINES--------------
#define NUM_OF_MINERS 4
#define MAX_NUM_OF_MESSAGES
#define MAX_SIZE_OF_MESSAGE

//-------------DECLARTIONS----------
void LauncherServer();
void LauncherMiners(int i_NumberOfMinersToLauch);
void LaunchMiner(int i_MinerId);
char* itoa(int i_Num);
int GetLen(int i_Num);
