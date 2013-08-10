
CON

  STRUCT_SIZE = 8
  
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
  
PUB Initialize(baseAddress, bufferAddress, fifoDepth)
  SetBaseAddress(baseAddress)
  long[sem_id]       := 0 'locknew
  long[depth]        := fifoDepth
  long[occupancy]    := 0
  long[flow_ended]   := FALSE
  long[next_write]   := bufferAddress
  long[next_read]    := bufferAddress
  long[buffer_start] := bufferAddress
  long[buffer_end]   := @long[bufferAddress][fifoDepth] - 1

      
PUB SetBaseAddress(baseAddress)
  sem_id       := @long[baseAddress][SEM_ID_OFFSET]
  depth        := @long[baseAddress][DEPTH_OFFSET] 
  occupancy    := @long[baseAddress][OCCUPANCY_OFFSET]
  flow_ended   := @long[baseAddress][EOF_OFFSET]
  next_write   := @long[baseAddress][NEXT_WRITE_OFFSET]
  next_read    := @long[baseAddress][NEXT_READ_OFFSET]
  buffer_start := @long[baseAddress][BUFFER_START_OFFSET]
  buffer_end   := @long[baseAddress][BUFFER_END_OFFSET]


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
      long[next_write] += 4
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
      long[next_read] += 4
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

PUB Destroy
  lockret(long[sem_id])