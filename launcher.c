#include "launcher.h"

int main(int argc, char* argv[])
{
  int numberOfMinersToLauch;
  if(argc =! 2)
  {
    return -1;
  }
  
  if ( atoi(argv[1]) <= 0)
  {
	  return -1;
  }
  numberOfMinersToLauch = atoi(argv[1]);
  mq_unlink("/SERVER_QUEUE");
  LauncherServer();
  LauncherMiners(numberOfMinersToLauch);

  return 0;
}

void LauncherServer()
{
  if( fork() == 0)
  {
    char *args[]={"./server.out",NULL};
    execvp(args[0],args);
    exit(0);
  }
}

void LauncherMiners(int i_NumberOfMinersToLauch)
{

  for(int i = 0 ; i < i_NumberOfMinersToLauch; i++)
  {
    int minerID = i + 1;
    LaunchMiner(minerID);
  }
}

void LaunchMiner(int i_MinerId)
{
  if( fork() == 0)
  {
    char* minerIndex = itoa(i_MinerId);
    char *args[]={"./miner.out",minerIndex,NULL,};
    execvp(args[0],args);
    exit(0);
  }

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
