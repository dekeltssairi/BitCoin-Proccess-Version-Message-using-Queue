#include "e_MessageType.h"

typedef struct msg{
  MESSAGE_TYPE type;
  int dataSizeInBytes; //added
  char data[];
}MSG_T;
