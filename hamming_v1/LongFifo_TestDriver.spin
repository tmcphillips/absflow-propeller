
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000

  FIFO_MAX_CAPACITY = 1024
  
OBJ
   
  term  : "SerialConnection"
  fifo  : "LongFifo"

VAR

  long fifo_struct[fifo#STRUCT_SIZE]
  long fifo_buffer[FIFO_MAX_CAPACITY]

PUB Main | i, sem_id, fifo_capacity, value

  term.Start(115_200)
  sem_id := locknew
  
  repeat
                    
    case term.CharIn

      "I":  'Initialize fifo of requested size
        term.ReadLong(@fifo_capacity)
        fifo.Create(@fifo_struct, @fifo_buffer, fifo_capacity, sem_id)
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
                 