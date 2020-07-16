#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>


typedef struct block{
    int         height;        // Incrementeal ID of the block in the chain
    int         timestamp;    // Time of the mine in seconds since epoch
    unsigned int     prev_hash;    // Hash value of the previous block
    int         nonce;        // Incremental integer to change the hash value
    int         relayed_by;    // Miner ID
    unsigned int     hash;        // Current block hash value
    int        difficulty;    // Amount of preceding zeros in the hash
} BLOCK_T;
