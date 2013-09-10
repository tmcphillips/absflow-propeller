
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000

  MEMORY_POOL_BYTES = 4096
    
OBJ
   
  term  : "SerialConnection"
  fifo  : "FifoQueue"
  pool  : "MemoryPool"

VAR

  long pool_metadata[pool#METADATA_SIZE]
  byte pool_data[MEMORY_POOL_BYTES]
  
  long fifo_struct[fifo#STRUCT_SIZE]

PUB Main | i, sem_id, fifo_capacity, value, p_fifo_buffer

  term.Start(115_200)
  sem_id := locknew  

  repeat
                    
    case term.CharIn

      "I":  'Initialize fifo of requested size
        pool.Create(@pool_metadata, @pool_data, MEMORY_POOL_BYTES, sem_id)  
        term.ReadLong(@fifo_capacity)
        p_fifo_buffer := pool.Allocate(fifo_capacity * 4)
        if (p_fifo_buffer == 0)
          term.WriteLong(false)
        fifo.Create(@fifo_struct, p_fifo_buffer, fifo_capacity, sem_id)
        term.WriteLong(true) 
         
      "P":  'Put long to fifo
        term.ReadLong(@value)
        term.WriteLong(fifo.Push(value))                

      "T":  'Take character from fifo 
        term.WriteLong(fifo.Pop(@value))
        term.WriteLong(value)
        
      "E":  'Signal end of flow
        fifo.EndFlow

      "F":  'Check if flow has ended
        term.WriteLong(fifo.EOF)  

      "Q":  'Query fifo data structure
        repeat i from 0 to 15
          term.Char(byte[@fifo_struct][i])  
                 