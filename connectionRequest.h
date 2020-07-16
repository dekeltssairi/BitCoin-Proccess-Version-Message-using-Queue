typedef struct flexible_connection_Request{
  int minerID;
  //char* queueName;
  char queueName[];
} FLEXIBLE_Connection_Request;
