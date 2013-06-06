#include <MemoryFree.h>

// On Arduino Duemilanove with ATmega328:
//
// Reported free memory with str commented out:
// 1824 bytes
//
// Reported free memory with str and Serial.println(str) uncommented:
// 1810
//
// Difference: 14 bytes (13 ascii chars + null terminator)

// 14-bytes string
//char str[] = "Hello, world!";
//#define malloc pvPortMalloc

void setup() {
    Serial.begin(9600);
}

char* b;
void loop() {
    //Serial.println(str);

    Serial.print("freeMemory()=");
    Serial.println(xfreeMemory());
//b = new char [8];
//malloc(8);
    delay(1000);
}

extern unsigned int __heap_start;
extern void *__brkval;

/*
 * The free list structure as maintained by the 
 * avr-libc memory allocation routines.
 */
struct __freelist {
  size_t sz;
  struct __freelist *nx;
};

/* The head of the free list structure */
extern struct __freelist *__flp;
int xfreeListSize() {
  struct __freelist* current;
  int total = 0;

  for (current = __flp; current; current = current->nx) {
    total += 2; /* Add two bytes for the memory block's header  */
    total += (int) current->sz;
  }

  return total;
}

int xfreeMemory() {
  int free_memory;
Serial.println((int)__brkval);
Serial.println((int)&__heap_start);
Serial.println((int)&free_memory);
  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__heap_start);
  } else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
    free_memory += xfreeListSize();
  }
  return free_memory;
}

