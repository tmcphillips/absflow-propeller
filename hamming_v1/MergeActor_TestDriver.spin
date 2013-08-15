
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000

  FIFO_MAX_DEPTH = 1024
  
OBJ
   
  merge_actor   : "MergeActor"
  input_1_fifo  : "LongFifo"
  input_2_fifo  : "LongFifo"
  input_3_fifo  : "LongFifo"
  output_fifo   : "LongFifo"
  term          : "SerialConnection"

VAR

  
  long input_fifo_1_struct[output_fifo#STRUCT_SIZE]
  long input_fifo_2_struct[output_fifo#STRUCT_SIZE]
  long input_fifo_3_struct[output_fifo#STRUCT_SIZE]

  long output_fifo_struct[output_fifo#STRUCT_SIZE]

  long input_fifo_1_buffer[FIFO_MAX_DEPTH]
  long input_fifo_2_buffer[FIFO_MAX_DEPTH]
  long input_fifo_3_buffer[FIFO_MAX_DEPTH]

  long output_fifo_buffer[FIFO_MAX_DEPTH]
  
  long output_sem_id
  
  long input_1_sem_id
  long input_2_sem_id
  long input_3_sem_id

PUB Main | i, v

  term.Start(115_200)

  input_1_sem_id := locknew
  input_2_sem_id := locknew
  input_3_sem_id := locknew
  output_sem_id := locknew

  repeat
                    
    case term.CharIn
    
      "I":  'Initialize input and output fifos with requested size 
        input_1_fifo.Initialize(@input_fifo_1_struct, @input_fifo_1_buffer, term.ReadLong, input_1_sem_id)
        input_2_fifo.Initialize(@input_fifo_2_struct, @input_fifo_2_buffer, term.ReadLong, input_2_sem_id)
        input_3_fifo.Initialize(@input_fifo_3_struct, @input_fifo_3_buffer, term.ReadLong, input_3_sem_id)
        output_fifo.Initialize(@output_fifo_struct, @output_fifo_buffer, term.ReadLong, output_sem_id)
        term.WriteLong(merge_actor.Start(@input_fifo_1_struct, @input_fifo_2_struct, @input_fifo_3_struct, @output_fifo_struct))
        
      "P":  'Put long to input fifo
        case term.ReadLong
          1:
            term.WriteLong(input_1_fifo.put(term.ReadLong))                
          2:
            term.WriteLong(input_2_fifo.put(term.ReadLong))                
          3:
            term.WriteLong(input_3_fifo.put(term.ReadLong))                

      "T":  'Take character from  output fifo
        term.WriteLong(output_fifo.Take) 

      "L":  'Return last character taken from each fifo
        term.WriteLong(output_fifo.LastTaken)
        
      "E":  'Signal end of flow
        case term.ReadLong
          1:
            input_1_fifo.EndFlow                  
          2:
            input_2_fifo.EndFlow                
          3:
            input_3_fifo.EndFlow

      "F":  'Check if output flow has ended
        term.WriteLong(output_fifo.FlowEnded)

      "D":  'Destroy actor
        input_1_fifo.EndFlow
        input_2_fifo.EndFlow
        input_3_fifo.EndFlow
        repeat while merge_actor.IsRunning
        term.WriteLong(true)

      "W":  'Wait for input queues to empty
        repeat while long[@input_fifo_1_struct][output_fifo#OCCUPANCY_OFFSET] > 0
        repeat while long[@input_fifo_2_struct][output_fifo#OCCUPANCY_OFFSET] > 0
        repeat while long[@input_fifo_3_struct][output_fifo#OCCUPANCY_OFFSET] > 0
        term.WriteLong(true)
  
      "Q":  'Query fifo data structure
        case term.ReadLong
          0: 
            repeat i from 0 to 31
              term.Char(byte[@output_fifo_struct][i])  
          1: 
            repeat i from 0 to 31
              term.Char(byte[@input_fifo_1_struct][i])  
          2:                               
            repeat i from 0 to 31
              term.Char(byte[@input_fifo_2_struct][i])  
          3: 
            repeat i from 0 to 31
              term.Char(byte[@input_fifo_3_struct][i])  

                      
                