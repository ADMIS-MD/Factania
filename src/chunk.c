/*
 *
 *  Fractal chunks!
 *
 *  Reference
 *  - https://problemkaputt.de/gbatek.htm#dsmemorytimings
 *
 *  Chunk node object must be aligned at TTA_CHUNK_NODE_SIZE * sizeof(void*) bytes
 *
 *
 *  Note/TODO: 
 *    - Nds7 might be better to do the map management? the map is less important and it has faster vram timings
 *    - Data memory can be really fast sequentially, even if not fully cached
 *    - Cache specs:
 *      - 4 way set associative
 *      - 12 kb instruction
 *      - 4 kb data (!)
 *      - 8 word lines (8 * 4 byte word = 32 bytes)
 *      - implies 128 blocks
 *
 *    - ROugh calculations:
 *      - 512 things placed in a line, 8x8 chunk, need 18 separators
 *      - entering:
 *        - worst case: 18 cache misses
 *        - more reasonable case: 9 cache misses
 *      - leaving if random:
 *        - we'll probably pick up a couple 3 cycles but really thats ok
 *
 *    - DS rom max speed is around 6 mb/s, lets say to be safe quarter that in write
 *
 */



#define TTA_CHUNK_NODE_SIZE 32

#include <stdint.h>

// 8 x 8/
// Entity can be:
//  - Entity id
//  - tile id
//  - some other unique data 
typedef struct TTA_Chunk {
   uint16_t objects[64];
} TTA_Chunk;

typedef struct TTA_ChunkNode {
  TTA_Chunk* chunk_refs[TTA_CHUNK_NODE_SIZE];
} TTA_ChunkNode;


