
CON

  STRUCT_SIZE = 8

  FIFO_WIDTH = 4
  
  SEM_ID_OFFSET       = 0
  CAPACITY_OFFSET     = 1
  SIZE_OFFSET         = 2
  EOF_OFFSET          = 3
  BACK_OFFSET         = 4
  FRONT_OFFSET        = 5
  BUFFER_START_OFFSET = 6
  BUFFER_END_OFFSET   = 7
  
VAR

  'addresses of shared variables
  long p_p_back
  long p_p_front
  long p_size
  long p_flow_ended

  'local copies of shared constants
  long buffer_start
  long buffer_end
  long capacity  
  long sem_id
        
PUB Initialize(base_address, buffer_address, fifo_capacity, semaphore_id)

  'initialize shared constants
  long[base_address][BUFFER_START_OFFSET] := buffer_address
  long[base_address][BUFFER_END_OFFSET]   := buffer_address + fifo_capacity * FIFO_WIDTH - 1
  long[base_address][CAPACITY_OFFSET]     := fifo_capacity
  long[base_address][SEM_ID_OFFSET]       := semaphore_id
         
  SetBaseAddress(base_address)

  ' initialize shared variables
  long[p_size]       := 0
  long[p_flow_ended] := FALSE
  long[p_p_back]     := buffer_address
  long[p_p_front]    := buffer_address


PUB SetBaseAddress(base_address)

  buffer_start := long[base_address][BUFFER_START_OFFSET]
  buffer_end   := long[base_address][BUFFER_END_OFFSET]
  capacity     := long[base_address][CAPACITY_OFFSET] 
  sem_id       := long[base_address][SEM_ID_OFFSET]
      
  p_size       := @long[base_address][SIZE_OFFSET]
  p_flow_ended := @long[base_address][EOF_OFFSET]
  p_p_back     := @long[base_address][BACK_OFFSET]
  p_p_front    := @long[base_address][FRONT_OFFSET]


    
PUB Push(value) | p_back
  repeat
  
    repeat until not lockset(sem_id)                    ' lock access to shared fifo data structure
                           
    if (long[p_flow_ended])                             ' release lock and report failure if flow has ended
      lockclr(sem_id)
      return FALSE
      
    if long[p_size] == capacity                       
      lockclr(sem_id)
      next
      
    long[p_size]++
     
    p_back := long[p_p_back]                          ' get pointer to fifo back element 
    long[p_back] := value                             ' store the value in the back element
    p_back += FIFO_WIDTH                              ' point to next element
     
    if (p_back > buffer_end)                          ' update shared pointer to back element
      long[p_p_back] := buffer_start
    else
      long[p_p_back] := p_back
        
    lockclr(sem_id) 
    return TRUE
    

PUB Pop(p_result) | p_front
  repeat
  
    repeat until not lockset(sem_id)

    if long[p_size] == 0

      if long[p_flow_ended]
        lockclr(sem_id)
        return FALSE

      else
        lockclr(sem_id)
        next
        
    p_front := long[p_p_front] 
    long[p_result] := long[p_front]
    p_front += FIFO_WIDTH
    long[p_size]--
     
    if (p_front > buffer_end)
      long[p_p_front] := buffer_start
    else
      long[p_p_front] := p_front
      
    lockclr(sem_id)
    return TRUE

PUB EndFlow
  repeat until not lockset(sem_id)
  long[p_flow_ended] := TRUE
  lockclr(sem_id)

PUB Size
  return long[p_size]

Pub FlowEnded
  repeat until not lockset(sem_id)
  result := long[p_flow_ended]
  lockclr(sem_id)