
CON

  METADATA_SIZE = 5
                                                
  SEM_ID_OFFSET       = 0
  CAPACITY_OFFSET     = 1
  POOL_START_OFFSET   = 2  
  NEXT_OFFSET         = 3
  SIZE_OFFSET         = 4
  
VAR

  'local copies of shared constants
  long sem_id
  long capacity
  long p_pool_start

  'pointers to shared variables
  long p_p_next
  long p_size
        
PUB Create(p_base, p_pool, pool_capacity, semaphore_id)

  ' configure local pointers to shared variables 
  InitializeLocalPointers(p_base)
                                                 
  ' initialize shared variables through the local pointers
  long[p_size]   := 0
  long[p_p_next] := p_pool
  
  'initialize shared constants and load local variables with copies of their values
  p_pool_start := long[p_base][POOL_START_OFFSET] := p_pool
  capacity     := long[p_base][CAPACITY_OFFSET]   := pool_capacity
  sem_id       := long[p_base][SEM_ID_OFFSET]     := semaphore_id
    

PUB Base(p_base)

  ' configure local pointers to shared variables
  InitializeLocalPointers(p_base)

  'load local variables with copies of shared constants
  p_pool_start := long[p_base][POOL_START_OFFSET]
  capacity     := long[p_base][CAPACITY_OFFSET] 
  sem_id       := long[p_base][SEM_ID_OFFSET]

  
PRI InitializeLocalPointers(p_base)
  p_size   := @long[p_base][SIZE_OFFSET]
  p_p_next := @long[p_base][NEXT_OFFSET]
 
    
PUB Allocate(requested_bytes) | new_size, p_next, allocation_bytes, p_allocated_memory

  repeat until not lockset(sem_id)                      ' lock access to shared memory pool data structure

  allocation_bytes := (requested_bytes + 3) & (!3)
  
  new_size := long[p_size] + allocation_bytes
  
  if (new_size > capacity)
      lockclr(sem_id)
      return 0    

  long[p_size] := new_size

  p_next := long[p_p_next]
  p_allocated_memory := p_next
  long[p_p_next] := p_allocated_memory + allocation_bytes
        
  lockclr(sem_id)
  return p_allocated_memory
        

PUB Size
  return long[p_size]