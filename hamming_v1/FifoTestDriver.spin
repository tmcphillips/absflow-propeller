
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000

  FIFO_MAX_DEPTH = 1024
  
OBJ
   
  term  : "Parallax Serial Terminal"
  f     : "Fifo"

VAR

  long fifo_struct[f#STRUCT_SIZE]
  long fifo_buffer[FIFO_MAX_DEPTH]

PUB Main | i

  term.Start(115_200)
  
  repeat
                    
    case term.CharIn

      "I":  'Initialize fifo of requested size 
        f.Initialize(@fifo_struct, @fifo_buffer, term.CharIn)
        
      "P":  'Put character to fifo
        term.Char(f.put(term.CharIn))                

      "T":  'Take character from fifo
        term.Char(f.Take)

      "L":  'Return last character taken from fifo
        term.Char(f.LastTaken)
        
      "E":  'Signal end of flow
        f.EndFlow

      "F":  'Check if flow has ended
        term.Char(f.FlowEnded)

      "D":  'Destroy fifo
        f.Destroy

      "Q":  'Query fifo data structure
        repeat i from 0 to 3
          term.Char(long[@fifo_struct][i])  
                      
                           