This is a simple memory allocator that stores information about the blocks right into them.
A block stores: information about the next block, the previous block, the total size of the current block and the user data
There is a special block, the first block which stores the index of the next block.

The program takes the following commands:

INITIALIZE <N>:
This command is always called first and it initializez an arena with the size of N bytes.

FINALIZE:
This command is always called last and it frees the memory allocated with INITIALIZE <N>.

DUMP:
This command displays the current memory map, byte by byte.
There are 16 bytes displayed on every line.
At the beginning of the line is the current index followed by 16 bytes.
The last line contains the size of the arena.

ALLOC <SIZE>:
This command allocates the first free space it finds from left to right, large enough to store SIZE bytes + 3 * sizeof(int) bytes worth of meta-data.

FREE <INDEX>:
This command will free the memory block whose user data begins at INDEX offset from the 'start' (beginning of the arena).

FILL <INDEX> <SIZE> <VALUE>:
This command will set SIZE consecutive bytes starting with the one at INDEX to the given VALUE.
This command may corrupt the arena.

SHOW <INFO>:
INFO can be:
	FREE: displays information about free memory in the arena;
	USAGE: displays information about usage such as the number of used bytes, utilization efficency and fragmentation;
	ALLOCATIONS: displays the free and allocated memory zones, one per line along with their size in bytes.

