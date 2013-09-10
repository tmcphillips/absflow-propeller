
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

  'pointers to shared variables
  long p_p_back
  long p_p_front
  long p_size
  long p_eof

  'local copies of shared constants
  long p_buffer_start
  long p_buffer_end
  long capacity  
  long sem_id
        
PUB Create(p_base, p_buffer, fifo_capacity, semaphore_id)

  ' configure local pointers to shared variables 
  InitializeLocalPointers(p_base)
                                                 
  ' initialize shared variables through the local pointers
  long[p_size]    := 0
  long[p_eof]     := FALSE
  long[p_p_back]  := p_buffer
  long[p_p_front] := p_buffer
    
  'initialize shared constants and load local variables with copies of their values
  p_buffer_start := long[p_base][BUFFER_START_OFFSET] := p_buffer
  p_buffer_end   := long[p_base][BUFFER_END_OFFSET]   := p_buffer + fifo_capacity * FIFO_WIDTH - 1  
  capacity       := long[p_base][CAPACITY_OFFSET]     := fifo_capacity
  sem_id         := long[p_base][SEM_ID_OFFSET]       := semaphore_id
    

PUB Base(p_base)

  ' configure local pointers to shared variables
  InitializeLocalPointers(p_base)

  'load local variables with copies of shared constants
  p_buffer_start := long[p_base][BUFFER_START_OFFSET]
  p_buffer_end   := long[p_base][BUFFER_END_OFFSET]
  capacity       := long[p_base][CAPACITY_OFFSET] 
  sem_id         := long[p_base][SEM_ID_OFFSET]

  
PRI InitializeLocalPointers(p_base)
  p_size    := @long[p_base][SIZE_OFFSET]
  p_eof     := @long[p_base][EOF_OFFSET]
  p_p_back  := @long[p_base][BACK_OFFSET]
  p_p_front := @long[p_base][FRONT_OFFSET]

    
PUB Push(value) | p_back
  repeat
  
    repeat until not lockset(sem_id)                    ' lock access to shared fifo data structure
                           
    if (long[p_eof])                                    ' release lock and report failure if flow has ended
      lockclr(sem_id)
      return FALSE
      
    if long[p_size] == capacity                       
      lockclr(sem_id)
      next
      
    long[p_size]++
     
    p_back := long[p_p_back]                          ' get pointer to fifo back element 
    long[p_back] := value                             ' store the value in the back element
    p_back += FIFO_WIDTH                              ' point to next element
     
    if (p_back > p_buffer_end)                        ' update shared pointer to back element
      long[p_p_back] := p_buffer_start
    else
      long[p_p_back] := p_back
        
    lockclr(sem_id) 
    return TRUE
    

PUB Pop(p_result) | p_front
  repeat
  
    repeat until not lockset(sem_id)

    if long[p_size] == 0

      if long[p_eof]
        lockclr(sem_id)
        return FALSE

      else
        lockclr(sem_id)
        next
        
    p_front := long[p_p_front] 
    long[p_result] := long[p_front]
    p_front += FIFO_WIDTH
    long[p_size]--
     
    if (p_front > p_buffer_end)
      long[p_p_front] := p_buffer_start
    else
      long[p_p_front] := p_front
      
    lockclr(sem_id)
    return TRUE

PUB EndFlow
  repeat until not lockset(sem_id)
  long[p_eof] := TRUE
  lockclr(sem_id)

PUB Size
  return long[p_size]

Pub EOF
  repeat until not lockset(sem_id)
  result := long[p_eof]
  lockclr(sem_id)