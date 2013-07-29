CON

  STRUCT_SIZE = 6
  
  SEMID    = 0
  CAPACITY = 1
  SIZE     = 2
  HEAD     = 3
  TAIL     = 4
  BUFFER   = 5

VAR

  long fifo

PUB Construct(fifoStructAddress, bufferAddress, c)
  Initialize(fifoStructAddress)
  long[fifo][SEMID] := locknew
  long[fifo][CAPACITY] := c
  long[fifo][SIZE] := 0
  long[fifo][HEAD] := 0
  long[fifo][TAIL] := 0
  long[fifo][BUFFER] := bufferAddress

PUB Initialize(fifoStructAddress)
  fifo := fifoStructAddress       
  
PUB Put(value)
  repeat
    repeat until not lockset(long[fifo][SEMID])
    if long[fifo][SIZE] < long[fifo][CAPACITY]
      long[long[fifo][BUFFER]][long[fifo][HEAD]++] := value
      long[fifo][SIZE]++
      if (long[fifo][HEAD] == long[fifo][CAPACITY])
        long[fifo][HEAD] := 0
      lockclr(long[fifo][SEMID])
      return
    else
      lockclr(long[fifo][SEMID])

PUB Take
  repeat
    repeat until not lockset(long[fifo][SEMID])
    if long[fifo][SIZE] > 0
      result := long[long[fifo][BUFFER]][long[fifo][TAIL]++]
      long[fifo][SIZE]--
      if (long[fifo][TAIL] == long[fifo][CAPACITY])
        long[fifo][TAIL] := 0
      lockclr(long[fifo][SEMID])
      return
    else
      lockclr(long[fifo][SEMID])

PUB Destroy
  lockret(long[fifo][SEMID])