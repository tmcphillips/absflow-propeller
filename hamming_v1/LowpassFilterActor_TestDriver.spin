
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000

  FIFO_MAX_DEPTH = 1024
  
OBJ
   
  actor : "LowpassFilterActor"
  input_fifo   : "LongFifo"
  output_fifo  : "LongFifo"
  term         : "SerialConnection"

VAR

  long input_fifo_struct[input_fifo#STRUCT_SIZE]
  long output_fifo_struct[input_fifo#STRUCT_SIZE]
  long input_fifo_buffer[FIFO_MAX_DEPTH]
  long output_fifo_buffer[FIFO_MAX_DEPTH]
  long input_sem_id
  long output_sem_id

PUB Main | i, value, input_fifo_depth, output_fifo_depth, cutoff_value, shutdown_on_overage

  term.Start(115_200)

  input_sem_id := locknew
  output_sem_id := locknew
  
  repeat
                    
    case term.CharIn
    
      "I":  'Initialize input and output fifos with requested size
        term.ReadLong(@input_fifo_depth)
        term.ReadLong(@output_fifo_depth)
        term.ReadLong(@cutoff_value)
        term.ReadLong(@shutdown_on_overage) 
        input_fifo.Initialize(@input_fifo_struct, @input_fifo_buffer, input_fifo_depth, input_sem_id)
        output_fifo.Initialize(@output_fifo_struct, @output_fifo_buffer, output_fifo_Depth, output_sem_id)
        term.WriteLong(actor.Start(@input_fifo_struct, @output_fifo_struct, cutoff_value, shutdown_on_overage))
        
      "P":  'Put long to input fifo
        term.ReadLong(@value)
        term.WriteLong(input_fifo.Push(value))                

      "T":  'Take character from output fifo
        term.WriteLong(output_fifo.Pop(@value))
        term.WriteLong(value)
        
      "E":  'Signal end of flow
        input_fifo.EndFlow

      "F":  'Check if output flow has ended
        term.WriteLong(output_fifo.FlowEnded)

      "D":  'Destroy actor
        input_fifo.EndFlow
        repeat while actor.IsRunning
        term.WriteLong(true)

      "W":  'Wait for input queue to empty
        repeat while long[@input_fifo_struct][input_fifo#OCCUPANCY_OFFSET] > 0
       term.WriteLong(true)
  
      "Q":  'Query fifo data structure
        case term.CharIn
          "i": 
            repeat i from 0 to 31
              term.Char(byte[@input_fifo_struct][i])  
          "o": 
            repeat i from 0 to 31
              term.Char(byte[@output_fifo_struct][i])  
                      
                