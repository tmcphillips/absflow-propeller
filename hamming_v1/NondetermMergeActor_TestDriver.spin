
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000

  FIFO_MAX_DEPTH = 1024
  
OBJ
   
  merge_actor   : "NondetermMergeActor"
  input_1_fifo  : "FifoQueue"
  input_2_fifo  : "FifoQueue"
  output_fifo   : "FifoQueue"
  term          : "SerialConnection"

VAR

  
  long input_fifo_1_struct[output_fifo#STRUCT_SIZE]
  long input_fifo_2_struct[output_fifo#STRUCT_SIZE]

  long output_fifo_struct[output_fifo#STRUCT_SIZE]

  long input_fifo_1_buffer[FIFO_MAX_DEPTH]
  long input_fifo_2_buffer[FIFO_MAX_DEPTH]

  long output_fifo_buffer[FIFO_MAX_DEPTH]
  
  long output_sem_id
  
  long input_1_sem_id
  long input_2_sem_id
  long input_3_sem_id

PUB Main | i, v, input_fifo_1_depth, input_fifo_2_depth, output_fifo_depth, option, value

  term.Start(115_200)

  input_1_sem_id := locknew
  input_2_sem_id := locknew
  output_sem_id := locknew

  repeat
                    
    case term.CharIn
    
      "I":  'Initialize input and output fifos with requested size

        term.ReadLong(@input_fifo_1_depth)
        term.ReadLong(@input_fifo_2_depth)
        term.ReadLong(@output_fifo_depth)
        input_1_fifo.Create(@input_fifo_1_struct, @input_fifo_1_buffer, input_fifo_1_depth, input_1_sem_id)
        input_2_fifo.Create(@input_fifo_2_struct, @input_fifo_2_buffer, input_fifo_2_depth, input_2_sem_id)
        output_fifo.Create(@output_fifo_struct, @output_fifo_buffer, output_fifo_depth, output_sem_id)
        term.WriteLong(merge_actor.Start(@input_fifo_1_struct, @input_fifo_2_struct, @output_fifo_struct))
        
      "P":  'Put long to input fifo
        term.ReadLong(@option)
        term.ReadLong(@value)
        case option
          1:
            term.WriteLong(input_1_fifo.Push(value))                
          2:
            term.WriteLong(input_2_fifo.Push(value))             

      "T":  'Take character from  output fifo
        term.WriteLong(output_fifo.Pop(@value)) 
        term.WriteLong(value)
        
      "E":  'Signal end of flow
        term.ReadLong(@option)
        case option
          1:
            input_1_fifo.EndFlow                  
          2:
            input_2_fifo.EndFlow

      "F":  'Check if output flow has ended
        term.WriteLong(output_fifo.EOF)

      "D":  'Destroy actor
        input_1_fifo.EndFlow
        input_2_fifo.EndFlow
        repeat while merge_actor.IsRunning
        term.WriteLong(true)

      "W":  'Wait for input queues to empty
        repeat while input_1_fifo.Size > 0
        repeat while input_2_fifo.Size > 0
        term.WriteLong(true)
  
      "Q":  'Query fifo data structure
        term.ReadLong(@option)
        case option
        
          0: 
            repeat i from 0 to 31
              term.Char(byte[@output_fifo_struct][i])  
          1: 
            repeat i from 0 to 31
              term.Char(byte[@input_fifo_1_struct][i])  
          2:                               
            repeat i from 0 to 31
              term.Char(byte[@input_fifo_2_struct][i])

                      
                