# Assignment 2 Notes

The Sleeping TA

# Input
n - the number of student threads

# Output
events?

# Details

## Threads
- 1 TA
- n students

### TA Thread 
- Check if student is in hallway
    - if not, take a nap
- If (student), "help them" (random sleep timeout)
- Be woken up by students
    - NB: use semaphores according to lab doc
- Loop

### Student Thread
- Do "Programming" (random sleep)
- Try to ask TA for help
    - If TA is free (napping), wake them
- If TA is busy, try to join hallway
    - If hallway is full, go back to programming
    - Else, join hallway
- Loop

## Implementation
- Hallway queue - length 3
    - All operations (pop, push, peek) must be mutex locked
- Semaphore - wake TA
    - TA waits on semaphore while napping
    - Student can signal semaphore to wake TA
    - When TA is done waiting (takes), he will "help student"
    - TA gives the semaphore back and the student goes to program

## Constants
- Time spent programming - randomly distributed value between `MIN_PROG` and `MAX_PROG`
- Time spent helping - randomly distributed value between `MIN_HELP` and `MAX_HELP`
