#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
void *start;
int n;
void INITIALIZE(int n)
{
    start = calloc(n,sizeof(unsigned char));
}
void FINALIZE()
{
    free((void *) start);
}
void DUMP()
{
    int lines, i, j;
    lines = n / 16 + 1;
    if(n % 16 == 0)
    {
        --lines;
    }
    for(i = 0; i < lines; ++i)
    {
        printf("%8X\t", i);
        for(j = 0; j < 8; ++j)
        {
            printf("%2X ", (unsigned int)(uintptr_t)start + i * 16 + j);
        }
        printf(" ");
        for(j = 8; j < 16; ++j)
        {
            printf("%2X ", (unsigned int)(uintptr_t)start + i * 16 + j);
        }
        printf("\n");
    }
    printf("%8X\n", n);
}
void ALLOC(int size)
{
    char *newer; //auxiliary pointer
    char *now, *next; //with these pointers I go through one beginning of a memory block to another beginning of memory block
    int ok = 0; //I haven't found memory space for my block yet
    now = (char*)start;
    next = (char*)start + *((int*)now);
    while(ok == 0 && *((int*)next) != 0) //while I haven't found free memory space with sufficient size between two memory blocks				 
    {                                    //and I haven't reached the last memory block
        if(next - (now + 3 * (sizeof(int))) + *((int*)now + 2) >= size + 3 * (sizeof(int))) //if there's enough space between 
        {
            ok = 1;
            newer = now + 3 * (sizeof(int)) + *((int*)now + 2); //beginning address of the new block
            *((int*)newer) = next - (char*)start; //the offset of the next memory block after this new one
            *((int*)newer + 1) = now - (char*)start; //the offset of the previous memory block
            *((int*)newer + 2) = size; //the amount of space that I'm allocating to this block

            *((int*)now) = newer - (char*)start; //updating the neighbours' information
            *((int*)next + 1) = newer - (char*)start;
        }
        else
        {
            now = next;
            next = (char*)start + *((int*)now);
        }
    }
    if(ok == 0) //if I didn't manage to allocate between two existing blocks
    {
        now = next; //now is pointing to the last memory block
        //maybe there is enough space from the last block to the end of the available memory
        if(n - (now - (char*)start + 3 * sizeof(int) + *((int*)now + 3)) >= size) 
        {
            newer = now + 3 * sizeof(int) + *((int*)now +2);
            *((int*)newer) = 0;
            *((int*)newer + 1) = now - (char*)start;
            *((int*)newer + 2) = size;
            //'newer' now points to the last memory block in the arena
            ok = 1;
        }
    }
    if(ok == 0) //if I didn't manage to allocate anywhere
    {
        printf("0\n");
    }
    else
    {
        printf("%lu\n", newer - (char*)start + 3 * (sizeof(int)));
    }
}
void FILLL(int index, int size, int value)
{
    char* p;
    int i;
    p = start + index;
    for(i = 0; i < size; ++i)
    {
        p[i] = value;
    }
}
void FREEE(int index)
{
    char *p, *next, *last;
    int size;
    p = (char*)start + index - 3 * sizeof(int);
    size = *((int*)p + 2);
    next = (char*)start + *((int*)p); //the right neighbour block
    last = (char*)start + *((int*)p + 1);//the left neighbour block
    FILLL(index - 3 * sizeof(int), size + 3 * sizeof(int), 0);
    *((int*)last) = next - (char*)start; //updateing the neighbours's information
    *((int*)next + 1) = last - (char*)start;
}
void SHOW_FREE()
{
    int bytes, blocks;
    char *now, *next;
    bytes = 0;
    blocks = 0;
    now = (char*)start;
    next = (char*)start + *((int*)start);
    if(next == (char*)start) //if there is no other block than the starting one
    {
        blocks = 1;
        bytes = n - sizeof(int);
    }
    else
    {
	//starting block is special
        if((next - now) - sizeof(int) > 0) //checking between the first 2 blocks
        {
            ++blocks;
            bytes += (next - now) - sizeof(int);
        }
        now = next;
        next = (char*)start + *((int*)now);
        while(*((int*)next) != 0)
        {
            if((next - now) - 3 * sizeof(int) - *((int*)now + 2) > 0)
            {
                ++blocks;
                bytes += (next - now) - 3 * sizeof(int) - *((int*)now + 2);
            }
            now = next;
            next = (char*)start + *((int*)now);
        }
        if((next - now) - 3 * sizeof(int) - *((int*)now + 2) > 0)
        {
            ++blocks;
            bytes += (next - now) - 3 * sizeof(int) - *((int*)now + 2);
        }
        if(n - *((int*)now) - 3 * sizeof(int) - *((int*)next + 2) > 0) //checking for space after the last block
        {
            ++blocks;
            bytes += n - *((int*)now) - 3 * sizeof(int) - *((int*)next + 2);
        }
    }
    printf("%d blocks (%d bytes) free\n", blocks, bytes);
}
void SHOW_USAGE()
{
    char *now, *next;
    int blocks, free_blocks, eff, fragm, rezervs, used, free;
    blocks = 0;
    free_blocks = 0;
    rezervs = sizeof(int); //the int that is stored in the first block
    used = 0;
    free = 0;
    if(*((int*)start) == 0) //if there is no block allocated
    {
        blocks = 0;
        eff = 0;
        fragm = 0;
    }
    else
    {
        now = (char*)start + *((int*)start);
        ++blocks;
        rezervs += 3 * sizeof(int) + *((int*)now + 2);
        used += *((int*)now + 2);
        if(now - (char*)start - 3 * sizeof(int) > 0)
        {
            ++free_blocks;
            free += now - (char*)start - 3 * sizeof(int);
        }
        if(*((int*)now) == 0) //if I have only one block allocated I can only have a posible block of free space
        {
            if(n - *((int*)start) - 3 * sizeof(int) - *((int*)now + 2) > 0)
            {
                free += n - *((int*)start) - 3 * sizeof(int) - *((int*)now + 2);
                ++free_blocks;
            }
        }
        else
        {
            next = (char*)start + *((int*)now);
            while(*((int*)next) != 0)
            {
                ++blocks;
                rezervs += 3 * sizeof(int) + *((int*)next + 2);
                used += *((int*)next + 2);
                if(next - now - 3 * sizeof(int) - *((int*)now + 2) > 0)
                {
                    free += next - now - 3 * sizeof(int) - *((int*)now + 2);
                    ++free_blocks;
                }
                now = next;
                next = (char*)start + *((int*)now);
            }
            //'next' reached the last memory block
            ++blocks;
            rezervs += 3 * sizeof(int) + *((int*)next + 2);
            used += *((int*)next + 2);
            if(next - now - 3 * sizeof(int) - *((int*)now + 2) > 0)
            {
                free += next - now - 3 * sizeof(int) - *((int*)now + 2);
                ++free_blocks;
            }
            if(n - (*((int*)now) + 3 * sizeof(int) + *((int*)next + 2)) > 0)
            {
                free += next - now - 3 * sizeof(int) - *((int*)now + 2);
                ++free_blocks;
            }
        }
        fragm = (free_blocks - 1) * 100 / blocks;
        eff = used * 100 / rezervs;
    }
    printf("%d blocks (%d bytes) used\n", blocks, used);
    printf("%d%% efficency\n", eff);
    printf("%d%% fragmentation\n", fragm);
}
void SHOW_ALLOCATIONS()
{
    char *now, *next;
    printf("OCCUPIED %lu bytes\n", sizeof(int)); //first block
    if(*((int*)start + 2) == 0)
    {
        printf("FREE %lu bytes\n", n - sizeof(int));
    }
    else
    {
        now = (char*)start + *((int*)start);
        if(now - (char*)start - sizeof(int) > 0) //free space between the start block and the next block
        {
            printf("FREE %lu bytes\n", now - (char*)start - sizeof(int));
        }
        if(*((int*)now) == 0) //if there is only one memory block allocated
        {
            printf("OCCUPIED %lu bytes\n", *((int*)now + 2) + 3 * sizeof(int));
            if(n - *((int*)start) - 3 * sizeof(int) - *((int*)now + 2) > 0)
            {
                printf("FREE %lu bytes\n", n - *((int*)start) - 3 * sizeof(int) - *((int*)now + 2));
            }
        }
        else
        {
            next = (char*)start + *((int*)now);
            while(*((int*)next) != 0)
            {
                printf("OCCUPIED %lu bytes\n", *((int*)now + 2) + 3 * sizeof(int));
                if(next - now - 3 * sizeof(int) - *((int*)now + 2) > 0)
                {
                    printf("FREE %lu bytes\n", next - now - 3 * sizeof(int) - *((int*)now + 2));
                }
                now = next;
                next = (char*)start + *((int*)now);
            }
            printf("OCCUPIED %lu bytes\n", *((int*)now + 2) + 3 * sizeof(int));
            if(next - now - 3 * sizeof(int) - *((int*)now + 2) > 0)
            {
                printf("FREE %lu bytes\n", next - now - 3 * sizeof(int) - *((int*)now + 2));
            }
            printf("OCCUPIED %lu bytes\n", *((int*)next + 2) + 3 * sizeof(int));
            if(n - (*((int*)now) + 3 * sizeof(int) + *((int*)next + 2)) > 0)
            {
                printf("FREE %lu bytes\n", n - (*((int*)now) + 3 * sizeof(int) + *((int*)next + 2)));
            }
        }
    }

}
int s_to_int(char s[])
{
    int l, rez, i;
    l = strlen(s);
    rez = 0;
    for(i = 0; i < l; ++i)
    {
        rez = rez * 10 + (int)(s[i] - '0');
    }
    return rez;
}
int main()
{
    char cmd[20], arg1[20], arg2[20], arg3[20];
    int ok = 1;
    while(ok)
    {
        scanf("%s", cmd);
        if(strcmp(cmd, "FINALIZE") == 0)
        {
            FINALIZE();
            ok = 0;
        }
        else if(strcmp(cmd, "INITIALIZE") == 0)
        {
            scanf("%s", arg1);
            n = s_to_int(arg1);
            INITIALIZE(n);
        }
        else if(strcmp(cmd, "DUMP") == 0)
        {
            DUMP();
        }
        else if(strcmp(cmd, "ALLOC") == 0)
        {
            scanf("%s", arg1);
            ALLOC(s_to_int(arg1));
        }
        else if(strcmp(cmd, "FREE") == 0)
        {
            scanf("%s", arg1);
            FREEE(s_to_int(arg1));
        }
        else if(strcmp(cmd, "FILL") == 0)
        {
            scanf("%s", arg1);
            scanf("%s", arg2);
            scanf("%s", arg3);
            FILLL(s_to_int(arg1), s_to_int(arg2), s_to_int(arg3));
        }
        else if(strcmp(cmd, "SHOW") == 0)
        {
            scanf("%s", arg1);
            if(strcmp(arg1, "FREE") == 0)
            {
                SHOW_FREE();
            }
            else if(strcmp(arg1, "USAGE") == 0)
            {
                SHOW_USAGE();
            }
            else if(strcmp(arg1, "ALLOCATIONS") == 0)
            {
                SHOW_ALLOCATIONS();
            }
        }
    }
    return 0;
}
