#include <string.h>
#include "pmm.h"
#include "KernelDisplay.h"

#define PMM_BLOCKS_PER_BYTE 	8		// there are 8 blocks per byte (8bit = 1byte)
#define PMM_BLOCK_SIZE     	4096		// each block is 4kb
#define PMM_BLOCK_ALIGN		PMM_BLOCK_SIZE

static uint32_t _pmm_memory_size = 0; // Physical memory size
static uint32_t _pmm_used_blocks = 0; // currentrly used blocks
static uint32_t _pmm_max_blocks = 0;  // Total blocks number
static uint32_t* _pmm_memory_map = 0; // This is the memory map bit array. Each bit represent a memory block (0=free, 1=used)

inline void mmap_set (int bit); 		// set a bit in the _pmm_memory_map   (if bit is set on _pmm_memory_map, that block is used)
inline void mmap_unset (int bit);   		// unset a bit in the _pmm_memory_map (if bit is unset on _pmm_memory_map, that block is free)
inline uint8_t mmap_test (int bit);     	// test if bit is set or unset in the _pmm_memory_map
int mmap_find_first_free ();			// finds first free block in _pmm_memory_map
int mmap_find_first_free_s (size_t size);	// finds first "size" number of free blocks in _pmm_memory_map

inline void mmap_set (int bit){
	_pmm_memory_map[bit/32] |= (1<< (bit % 32));
}

inline void mmap_unset (int bit){
	_pmm_memory_map[bit/32] &= ~ (1 << (bit % 32));
}

inline uint8_t mmap_test (int bit){
	return _pmm_memory_map[bit/32] & (1 << (bit % 32));
}

int mmap_find_first_free(){
	uint32_t i;
	int j;
	for(i=0;i<pmm_get_block_count()/32;i++)
		if(_pmm_memory_map[i] != 0xffffffff)
			for(j=0;j<32;j++){   // 32 = bits in a block
				int bit = 1 << j;
				if(!(_pmm_memory_map[i] & bit))
					return i*4*8+j;		// Found :)	
			}
	return -1; // out of memory
}



int mmap_find_first_free_s (size_t size){
	if(size==0)
		return -1;
	
	if(size==1)
		return mmap_find_first_free();

	uint32_t i;
	int j;
	uint32_t counter;
	for(i=0;i<pmm_get_block_count()/32;i++)
		if(_pmm_memory_map[i] != 0xffffffff)
			for(j=0;j<32;j++){   // 32 = bits in a block
				int bit = 1 << j;
				if(!(_pmm_memory_map[i] & bit)){
					int startbit = i*32;
					startbit+=bit;
					uint32_t free=0;
					for(counter=0;counter<=size;counter++){
						if(!mmap_test(startbit+counter))
							free++;
						if(free==size)
							return i*4*8+j; // Found :)
					}
				}
			}
	return -1; // unable to find "size" number of free contiguous blocks
					
}

void pmm_init(size_t memSize, physical_addr bitmap){
	_pmm_memory_size = memSize;
	_pmm_memory_map = (uint32_t*) bitmap;
	_pmm_max_blocks= (pmm_get_memory_size()*1024)/ PMM_BLOCK_SIZE;
	_pmm_used_blocks = _pmm_max_blocks;

	memset(_pmm_memory_map,0xf,pmm_get_block_count()/PMM_BLOCKS_PER_BYTE); // by default, let's assume that all mem is in use
}

void pmm_init_region (physical_addr base, size_t size){
	int align = base / PMM_BLOCK_SIZE;
	int blocks = size / PMM_BLOCK_SIZE;

	for(;blocks >=0;blocks--){
		mmap_unset(align++);
		_pmm_used_blocks--;
	}

	mmap_set(0); // First block should be always set !
}

void pmm_deinit_region (physical_addr base, size_t size){
	int align = base / PMM_BLOCK_SIZE;
	int blocks = size / PMM_BLOCK_SIZE;
	
	for(;blocks>=0;blocks--){
		mmap_set(align++);
		_pmm_used_blocks++;
	}

}

void* pmm_alloc_block(){

	if(pmm_get_free_block_count()<=0)
		return 0;  // out of memory

	int frame = mmap_find_first_free();

	if(frame==-1)
		return 0; //all blocks are used - out of memory

	mmap_set(frame);

	physical_addr addr = frame * PMM_BLOCK_SIZE;
	_pmm_used_blocks++;

	return (void*)addr;

}

void pmm_free_block(void* p){
	physical_addr addr = (physical_addr)p;
	int frame = addr / PMM_BLOCK_SIZE;
	mmap_unset(frame);

	_pmm_used_blocks--;
}

void* pmm_alloc_blocks(size_t size){
	if(pmm_get_free_block_count()<=size)
		return 0; 	//not enough free blocks

	int frame = mmap_find_first_free_s(size);

	if(frame == -1)
		return 0;
	
	uint32_t i;
	for(i=0;i<size;i++)
		mmap_set(frame+i);
	
	physical_addr addr = frame * PMM_BLOCK_SIZE;
	_pmm_used_blocks+=size;

	return (void*)addr;
}

void pmm_free_blocks(void *p, size_t size){
	physical_addr addr = (physical_addr)p;
	int frame = addr / PMM_BLOCK_SIZE;

	uint32_t i;
	for(i=0;i<size;i++)
		mmap_unset(frame+i);

	_pmm_used_blocks-=size;
}

size_t pmm_get_memory_size(){
	return _pmm_memory_size;
}

uint32_t pmm_get_block_count(){
	return _pmm_max_blocks;
}

uint32_t pmm_get_used_block_count(){
	return _pmm_used_blocks;
}

uint32_t pmm_get_free_block_count(){
	return _pmm_max_blocks - _pmm_used_blocks;
}

uint32_t pmm_get_block_size(){
	return PMM_BLOCK_SIZE;
}

uint32_t pmm_is_paging(){
	uint32_t res = 0;
	res=pmm_is_paging_asm();
	return (res & 0x80000000) ? 0 : 1 ;

}


