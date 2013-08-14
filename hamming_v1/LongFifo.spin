
CON

  STRUCT_SIZE = 8

  FIFO_WIDTH = 4
  
  SEM_ID_OFFSET         = 0
  DEPTH_OFFSET          = 1
  OCCUPANCY_OFFSET      = 2
  EOF_OFFSET            = 3
  NEXT_WRITE_OFFSET     = 4
  NEXT_READ_OFFSET      = 5
  BUFFER_START_OFFSET   = 6
  BUFFER_END_OFFSET     = 7
  
VAR

  long sem_id
  long depth
  long occupancy
  long next_write
  long next_read
  long buffer_start
  long buffer_end
  long flow_ended

  long _last_taken
  
PUB Initialize(base_address, buffer_address, fifo_depth, semaphore_id)
  SetBaseAddress(base_address)
  long[sem_id]       := semaphore_id
  long[depth]        := fifo_depth
  long[occupancy]    := 0
  long[flow_ended]   := FALSE
  long[next_write]   := buffer_address
  long[next_read]    := buffer_address
  long[buffer_start] := buffer_address
  long[buffer_end]   := @long[buffer_address][fifo_depth] - 1

      
PUB SetBaseAddress(base_address)
  sem_id       := @long[base_address][SEM_ID_OFFSET]
  depth        := @long[base_address][DEPTH_OFFSET] 
  occupancy    := @long[base_address][OCCUPANCY_OFFSET]
  flow_ended   := @long[base_address][EOF_OFFSET]
  next_write   := @long[base_address][NEXT_WRITE_OFFSET]
  next_read    := @long[base_address][NEXT_READ_OFFSET]
  buffer_start := @long[base_address][BUFFER_START_OFFSET]
  buffer_end   := @long[base_address][BUFFER_END_OFFSET]


PUB EndFlow
  repeat until not lockset(long[sem_id])
  long[flow_ended] := TRUE
  lockclr(long[sem_id])

Pub FlowEnded
  repeat until not lockset(long[sem_id])
  result := long[flow_ended]
  lockclr(long[sem_id])
    
PUB Put(value)
  repeat
    repeat until not lockset(long[sem_id])
                           
    if (long[flow_ended])
      lockclr(long[sem_id])
      return FALSE
      
    if long[occupancy] < long[depth]
    
      long[long[next_write]] := value
      long[next_write] += FIFO_WIDTH
      long[occupancy]++
      
      if (long[next_write] > long[buffer_end])
        long[next_write] := long[buffer_start]
        
      lockclr(long[sem_id])
      return TRUE
      
    else
    
      lockclr(long[sem_id])

PUB Take
  repeat
  
    repeat until not lockset(long[sem_id])

    if long[occupancy] > 0
    
      _last_taken := long[long[next_read]]
      long[next_read] += FIFO_WIDTH
      long[occupancy]--
      
      if (long[next_read] > long[buffer_end])
        long[next_read] := long[buffer_start]
        
      lockclr(long[sem_id])
      return TRUE

    elseif long[flow_ended]

      lockclr(long[sem_id])
      return FALSE
      
    else
    
      lockclr(long[sem_id])

Pub LastTaken
  return _last_taken