
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000

  HEAP_SIZE_BYTES = 4096
    
OBJ
   
  buffer_actor  : "BufferActor"
  heap          : "MemoryPool"
  input_queue   : "FifoQueue"
  output_queue  : "FifoQueue"
  term          : "SerialConnection"

VAR

  long heap_metadata[heap#METADATA_SIZE]
  byte heap_data[HEAP_SIZE_BYTES]
  
  long heap_sem_id
  long input_sem_id
  long output_sem_id
  
  long p_input_queue_metadata
  long p_output_queue_metadata
  long p_input_queue_data
  long p_output_queue_data
  

PUB Main | i, value, input_fifo_depth, output_fifo_depth   

  term.Start(115_200)

  input_sem_id := locknew
  output_sem_id := locknew
  heap_sem_id := locknew
  
  repeat
                    
    case term.CharIn
    
      "I":  'Initialize input and output fifos with requested size

        heap.Create(@heap_metadata, @heap_data, HEAP_SIZE_BYTES, heap_sem_id)
              
        term.ReadLong(@input_fifo_depth)
        p_input_queue_data := heap.Allocate(input_fifo_depth * 4)
        p_input_queue_metadata := heap.Allocate(input_queue#STRUCT_SIZE)
        input_queue.Create(p_input_queue_metadata, p_input_queue_data, input_fifo_depth, input_sem_id)

        term.ReadLong(@output_fifo_depth)   
        p_input_queue_data := heap.Allocate(output_fifo_depth * 4)
        p_output_queue_metadata := heap.Allocate(output_queue#STRUCT_SIZE)
        output_queue.Create(p_output_queue_metadata, p_output_queue_data, output_fifo_depth, output_sem_id)
        
        term.WriteLong(buffer_actor.Start(p_input_queue_metadata, p_output_queue_metadata))
        
      "P":  'Put long to input fifo
        term.ReadLong(@i)
        term.WriteLong(input_queue.Push(i))                

      "T":  'Take character from output fifo
        term.WriteLong(output_queue.Pop(@value))                                           
        term.WriteLong(value)
        
      "E":  'Signal end of flow
        input_queue.EndFlow

      "F":  'Check if output flow has ended
        term.WriteLong(output_queue.EOF)

      "D":  'Destroy actor
        input_queue.EndFlow
        repeat while buffer_actor.IsRunning
        term.WriteLong(true)

      "W":  'Wait for input queue to empty
        repeat while input_queue.Size > 0
       term.WriteLong(true)
  
      "Q":  'Query fifo data structure
        case term.CharIn
          "i": 
            repeat i from 0 to 31
              term.Char(byte[p_input_queue_metadata][i])  
          "o": 
            repeat i from 0 to 31
              term.Char(byte[p_output_queue_metadata][i])  
                      
                