
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000

  MAX_POOL_SIZE_IN_BYTES = 4096
  
OBJ
   
  term  : "SerialConnection"
  pool  : "MemoryPool"

VAR

  long pool_metadata[pool#METADATA_SIZE]
  byte pool_data[MAX_POOL_SIZE_IN_BYTES]

PUB Main | p_pool, capacity_bytes, sem_id, requested_bytes

  term.Start(115_200)
  sem_id := locknew
  
  repeat
                    
    case term.CharIn

      "I":  'Initialize memory pool
        term.ReadLong(@capacity_bytes)
        pool.Create(@pool_metadata, @pool_data, capacity_bytes, sem_id)
        term.WriteLong(true)
        
      "A":  'Allocate memory block of requested size
        term.ReadLong(@requested_bytes)
        term.WriteLong(pool.Allocate(requested_bytes))                

      "L":  'Get ID of lock associated with memory pool
        term.WriteLong(pool_metadata[pool#SEM_ID_OFFSET])
        
      "C":  'Get capacity of memory pool
        term.WriteLong(pool_metadata[pool#CAPACITY_OFFSET])       

      "S":  'Get current size of memory pool (total allocated memory)
        term.WriteLong(pool_metadata[pool#SIZE_OFFSET])

      "P":  'Get pool start address
        term.WriteLong(pool_metadata[pool#POOL_START_OFFSET])

      "N":  'Get address of next memory allocation
        term.WriteLong(pool_metadata[pool#NEXT_OFFSET])


                