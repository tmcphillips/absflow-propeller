
CON

  FIFO_COUNT    = 9
  MAX_DEPTH     = 100

  FILTER_IN_FIFO        = 0
  TIMES_2_IN_FIFO       = 1
  TIMES_3_IN_FIFO       = 2
  TIMES_5_IN_FIFO       = 3
  MERGE_2_3_IN_A_FIFO   = 4
  MERGE_2_3_IN_B_FIFO   = 5
  MERGE_2_3_5_IN_A_FIFO = 6
  MERGE_2_3_5_IN_B_FIFO = 7
  WORKFLOW_OUT_FIFO     = 8
    
OBJ
   
  term          : "SerialConnection"
  fifo[9]       : "LongFifo"
  filtered_fork : "FilterThenForkActor"
  times_2       : "ConstantMultiplierActor"
  times_3       : "ConstantMultiplierActor"
  times_5       : "ConstantMultiplierActor"
  merge_2_3     : "OrderedMergeActor"
  merge_2_3_5   : "OrderedMergeActor"

VAR

  long fifo_struct[FIFO_COUNT*fifo#STRUCT_SIZE]
  long fifo_buffer[FIFO_COUNT*MAX_DEPTH]
  long filter_in
  long times_2_in
  long times_3_in
  long times_5_in
  long merge_2_3_in_a
  long merge_2_3_in_b
  long merge_2_3_5_in_a
  long merge_2_3_5_in_b
  long workflow_out 

PUB Initialize(fifo_depths) | i, f, max_output_value, value

  filter_in        := @fifo_struct[ FILTER_IN_FIFO        * fifo#STRUCT_SIZE ]
  times_2_in       := @fifo_struct[ TIMES_2_IN_FIFO       * fifo#STRUCT_SIZE ]
  times_3_in       := @fifo_struct[ TIMES_3_IN_FIFO       * fifo#STRUCT_SIZE ] 
  times_5_in       := @fifo_struct[ TIMES_5_IN_FIFO       * fifo#STRUCT_SIZE ]
  merge_2_3_in_a   := @fifo_struct[ MERGE_2_3_IN_A_FIFO   * fifo#STRUCT_SIZE ]
  merge_2_3_in_b   := @fifo_struct[ MERGE_2_3_IN_B_FIFO   * fifo#STRUCT_SIZE ]
  merge_2_3_5_in_a := @fifo_struct[ MERGE_2_3_5_IN_A_FIFO * fifo#STRUCT_SIZE ] 
  merge_2_3_5_in_b := @fifo_struct[ MERGE_2_3_5_IN_B_FIFO * fifo#STRUCT_SIZE ]
  workflow_out     := @fifo_struct[ WORKFLOW_OUT_FIFO     * fifo#STRUCT_SIZE ]

  repeat f from 0 to FIFO_COUNT - 1                                                                   
    fifo[f].Initialize(@fifo_struct[f*fifo#STRUCT_SIZE], @fifo_buffer[f*MAX_DEPTH], fifo_depths, 0) 
    

Pub Begin(max_output_value)
     
  times_2.Start(times_2_in, merge_2_3_in_a, 2)
  times_3.Start(times_3_in, merge_2_3_in_b, 3)
  times_5.Start(times_5_in, merge_2_3_5_in_a, 5)
  merge_2_3.Start(merge_2_3_in_a, merge_2_3_in_b, merge_2_3_5_in_b)
  merge_2_3_5.Start(merge_2_3_5_in_a, merge_2_3_5_in_b, filter_in)
  filtered_fork.Start(filter_in, times_2_in, times_3_in, times_5_in, workflow_out, max_output_value, true)
   
  fifo[FILTER_IN_FIFO].Push(1)
                       
Pub ReadOutput(pValue)
  return fifo[WORKFLOW_OUT_FIFO].Pop(pValue)

Pub Wait
  repeat while filtered_fork.IsRunning
  repeat while times_2.IsRunning
  repeat while times_3.IsRunning
  repeat while times_5.IsRunning
  repeat while merge_2_3.IsRunning
  repeat while merge_2_3_5.IsRunning

Pub Peek(f, i)
  return byte[@fifo_struct[f*fifo#STRUCT_SIZE]][i]
       