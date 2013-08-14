
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000

  FIFO_MAX_DEPTH = 1024
  
OBJ
   
  term  : "SerialConnection"
  fifo  : "LongFifo"

VAR

  long fifo_struct[fifo#STRUCT_SIZE]
  long fifo_buffer[FIFO_MAX_DEPTH]

PUB Main | i, sem_id

  term.Start(115_200)
  sem_id := locknew
  
  repeat
                    
    case term.CharIn

      "I":  'Initialize fifo of requested size
        fifo.Initialize(@fifo_struct, @fifo_buffer, term.ReadLong, sem_id)
        
      "P":  'Put long to fifo
        term.WriteLong(fifo.put(term.ReadLong))                

      "T":  'Take character from fifo
        term.WriteLong(fifo.Take)

      "L":  'Return last character taken from fifo
        term.WriteLong(fifo.LastTaken)
        
      "E":  'Signal end of flow
        fifo.EndFlow

      "F":  'Check if flow has ended
        term.WriteLong(fifo.FlowEnded)  

      "Q":  'Query fifo data structure
        repeat i from 0 to 15
          term.Char(byte[@fifo_struct][i])  
                 