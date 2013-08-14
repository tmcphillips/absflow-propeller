
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000

  FIFO_MAX_DEPTH = 1024
  
OBJ
   
  fork_actor     : "ForkActor"
  input_fifo     : "LongFifo"
  output_1_fifo  : "LongFifo"
  output_2_fifo  : "LongFifo"
  output_3_fifo  : "LongFifo"
  output_4_fifo  : "LongFifo"
  term           : "SerialConnection"

VAR

  long input_fifo_struct[input_fifo#STRUCT_SIZE]
  
  long output_fifo_1_struct[input_fifo#STRUCT_SIZE]
  long output_fifo_2_struct[input_fifo#STRUCT_SIZE]
  long output_fifo_3_struct[input_fifo#STRUCT_SIZE]
  long output_fifo_4_struct[input_fifo#STRUCT_SIZE]

  long input_fifo_buffer[FIFO_MAX_DEPTH]

  long output_fifo_1_buffer[FIFO_MAX_DEPTH]
  long output_fifo_2_buffer[FIFO_MAX_DEPTH]
  long output_fifo_3_buffer[FIFO_MAX_DEPTH]
  long output_fifo_4_buffer[FIFO_MAX_DEPTH]
  
  long input_sem_id
  
  long output_1_sem_id
  long output_2_sem_id
  long output_3_sem_id
  long output_4_sem_id

PUB Main | i, v

  term.Start(115_200)

  input_sem_id := locknew
  output_1_sem_id := locknew
  output_2_sem_id := locknew
  output_3_sem_id := locknew
  output_4_sem_id := locknew    

  repeat
                    
    case term.CharIn
    
      "I":  'Initialize input and output fifos with requested size 
        input_fifo.Initialize(@input_fifo_struct, @input_fifo_buffer, term.ReadLong, input_sem_id)
        output_1_fifo.Initialize(@output_fifo_1_struct, @output_fifo_1_buffer, term.ReadLong, output_1_sem_id)
        output_2_fifo.Initialize(@output_fifo_2_struct, @output_fifo_2_buffer, term.ReadLong, output_2_sem_id)
        output_3_fifo.Initialize(@output_fifo_3_struct, @output_fifo_3_buffer, term.ReadLong, output_3_sem_id)
        output_4_fifo.Initialize(@output_fifo_4_struct, @output_fifo_4_buffer, term.ReadLong, output_4_sem_id)
        term.WriteLong(fork_actor.Start(@input_fifo_struct, @output_fifo_1_struct, @output_fifo_2_struct, @output_fifo_3_struct, @output_fifo_4_struct))
        
      "P":  'Put long to input fifo
        term.WriteLong(input_fifo.put(term.ReadLong))                

      "T":  'Take character from each output fifo
        term.WriteLong(output_1_fifo.Take)
        term.WriteLong(output_2_fifo.Take)
        term.WriteLong(output_3_fifo.Take)
        term.WriteLong(output_4_fifo.Take)

      "L":  'Return last character taken from each fifo
        term.WriteLong(output_1_fifo.LastTaken)
        term.WriteLong(output_2_fifo.LastTaken)
        term.WriteLong(output_3_fifo.LastTaken)
        term.WriteLong(output_4_fifo.LastTaken)
        
      "E":  'Signal end of flow
        input_fifo.EndFlow

      "F":  'Check if output flow has ended
        term.WriteLong(output_1_fifo.FlowEnded)
        term.WriteLong(output_2_fifo.FlowEnded)
        term.WriteLong(output_3_fifo.FlowEnded)
        term.WriteLong(output_4_fifo.FlowEnded)

      "D":  'Destroy actor
        input_fifo.EndFlow
        repeat while fork_actor.IsRunning
        term.WriteLong(true)

      "W":  'Wait for input queue to empty
        repeat while long[@input_fifo_struct][input_fifo#OCCUPANCY_OFFSET] > 0
       term.WriteLong(true)
  
      "Q":  'Query fifo data structure
        case term.CharIn
          0: 
            repeat i from 0 to 31
              term.Char(byte[@input_fifo_struct][i])  
          1: 
            repeat i from 0 to 31
              term.Char(byte[@output_fifo_1_struct][i])  
          2:                               
            repeat i from 0 to 31
              term.Char(byte[@output_fifo_2_struct][i])  
          3: 
            repeat i from 0 to 31
              term.Char(byte[@output_fifo_3_struct][i])  
          4: 
            repeat i from 0 to 31
              term.Char(byte[@output_fifo_4_struct][i])  
                      
                