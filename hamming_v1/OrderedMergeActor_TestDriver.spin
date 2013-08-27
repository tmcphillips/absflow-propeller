
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000

  MAX_DEPTH = 1024

  A = 0
  B = 1
  C = 2
  
OBJ
   
  term    : "SerialConnection"
  actor   : "OrderedMergeActor"
  fifo[3] : "LongFifo"

VAR
  
  long struct[3*fifo#STRUCT_SIZE]
  long buffer[3*MAX_DEPTH]
  long sem[3]
  

PUB Main | i, depth, f, v

  term.Start(115_200)

  repeat i from A to C
    sem[i] := locknew

  repeat
                    
    case term.CharIn
    
      "I":  'Initialize input and output fifos with requested size

        repeat i from A to C
          term.ReadLong(@depth)                                                                     
          fifo[i].Initialize(@struct[i*fifo#STRUCT_SIZE], @buffer[i*MAX_DEPTH], depth, sem[i]) 
        
        term.WriteLong(actor.Start(@struct[A*fifo#STRUCT_SIZE],@struct[B*fifo#STRUCT_SIZE],@struct[C*fifo#STRUCT_SIZE]))
        
      "P":  'Put long to input fifo
        term.ReadLong(@f)
        term.ReadLong(@v)
        term.WriteLong(fifo[f].put(v))             

      "T":  'Take character from output fifo
        term.WriteLong(fifo[C].Take) 

      "L":  'Return last character taken from each fifo
        term.WriteLong(fifo[C].LastTaken)
        
      "E":  'Signal end of flow
        term.ReadLong(@f) 
        fifo[f].EndFlow                  
    
      "F":  'Check if output flow has ended
        term.WriteLong(fifo[C].FlowEnded)

      "D":  'Destroy actor
        fifo[A].EndFlow
        fifo[B].EndFlow
        repeat while actor.IsRunning
        term.WriteLong(true)

      "W":  'Wait for input queues to empty
        repeat i from A to B
          repeat while long[@struct[i*fifo#STRUCT_SIZE]][fifo#OCCUPANCY_OFFSET] > 0
        term.WriteLong(true)
  
      "Q":  'Query fifo data structure
        term.ReadLong(@f)
        repeat i from 0 to 31
          term.Char(byte[@struct[f*fifo#STRUCT_SIZE]][i])  

                      
                