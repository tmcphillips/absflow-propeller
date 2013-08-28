
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
  
  long fifo_struct[3*fifo#STRUCT_SIZE]
  long fifo_buffer[3*MAX_DEPTH]
  long sem[3]
  

PUB Main | i, depth, f, value

  term.Start(115_200)

  repeat f from A to C
    sem[f] := locknew

  repeat
                    
    case term.CharIn
    
      "I":  'Initialize input and output fifos with requested size

        repeat f from A to C
          term.ReadLong(@depth)                                                                     
          fifo[f].Initialize(@fifo_struct[f*fifo#STRUCT_SIZE], @fifo_buffer[f*MAX_DEPTH], depth, sem[f]) 
        
        term.WriteLong(actor.Start(@fifo_struct[A*fifo#STRUCT_SIZE],@fifo_struct[B*fifo#STRUCT_SIZE],@fifo_struct[C*fifo#STRUCT_SIZE]))
        
      "P":  'Put long v to input fifo f
        term.ReadLong(@f)
        term.ReadLong(@value)
        term.WriteLong(fifo[f].Push(value))             

      "T":  'Take character from output fifo
        term.WriteLong(fifo[C].Pop(@value)) 
        term.WriteLong(value)
        
      "E":  'Signal end of flow on input fifo f
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
        repeat f from A to B
          repeat while long[@fifo_struct[f*fifo#STRUCT_SIZE]][fifo#OCCUPANCY_OFFSET] > 0
        term.WriteLong(true)
  
      "Q":  'Query fifo data structure f
        term.ReadLong(@f)
        repeat i from 0 to 31
          term.Char(byte[@fifo_struct[f*fifo#STRUCT_SIZE]][i])  

                      
                