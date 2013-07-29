VAR

  byte semId
  long buffer 
  byte capacity
  byte size
  byte head
  byte tail  

PUB Create(b, c)
  buffer := b
  capacity := c
  semId := locknew
  head := 0
  tail := 0
  size := 0  
  
PUB Put(value)
  repeat
    repeat until not lockset(semId)
    if size < capacity 
      long[buffer][head++] := value
      size++
      if (head == capacity)
        head := 0
      lockclr(semId)
      return
    else
      lockclr(semId)

PUB Take
  repeat
    repeat until not lockset(semId)
    if size > 0
      result := long[buffer][tail++]
      size--
      if (tail == capacity)
        tail := 0
      lockclr(semId)
      return
    else
      lockclr(semId)

PUB Destroy
  lockret(semId)