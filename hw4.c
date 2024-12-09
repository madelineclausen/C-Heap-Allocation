// Madeline Clausen (clausenm, 60633236)
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int count = 1;
unsigned char heap[127] = {0};
bool first_fit = true;

struct block
{
    int payload_size;
    int start_of_payload;
    char* allocation_status;
};

void make_struct_array(struct block block_info[count])
{
    int i=0;
    int j=0;
    while (i<127)
    {
        struct block new_block;
        int size = heap[i] >> 1;
        new_block.payload_size = size - 2;
        new_block.start_of_payload = i + 1;
        int if_allocated = heap[i] & 1;
        if (!if_allocated) 
        {
            new_block.allocation_status = "free";
        }
        else
        {
            new_block.allocation_status = "allocated";
        }
        if (j < count)
        {
            block_info[j] = new_block;
            j++;
        }
        i += size;
    }
}

void sort_struct_array(struct block block_info[count])
{
    struct block temp;
    for (int i=0; i<count-1; i++)
    {
        for (int j=0; j<(count-1-i); j++)
        {
            if (block_info[j].payload_size < block_info[j+1].payload_size)
            {
                temp = block_info[j];
                block_info[j] = block_info[j+1];
                block_info[j+1] = temp;
            } 
        }
    }
}

void print_struct_array(struct block block_info[count])
{
    for (int p=0; p<count; p++)
    {
        printf("%d-%d-%s\n", block_info[p].payload_size, block_info[p].start_of_payload, block_info[p].allocation_status);
    }
}

void write_to_memory(char* content, int write_index)
{
    while (content[0] != NULL)
    {
        if (write_index < 127)
        {
            heap[write_index] = content[0];
            write_index++;
        }
        content++;
    }
}

void print_memory(int print_index, int num_chars_to_print)
{
    printf("%d", heap[print_index]);
    for (int n=1; n<num_chars_to_print; n++)
    {
        print_index++;
        printf("-%d", heap[print_index]);
    }
    printf("\n");
}

void combine_free(int start, int end)
{  
    int size = heap[start] >> 1; // size of block
    int other_size = heap[start] >> 1; //size of prev
    int total;
    //check if before and/or after
    if(start>0)
    {
        if((heap[start-1]&1)==0)//before is free 
        {
            other_size = (heap[start-1] >> 1);
            total = other_size +size;
            heap[start-other_size]= (total) << 1;
            for(int i=start-1; i<end; i++)
            {
                heap[i]=0;
            }
            heap[end]= total << 1;
            start= start-other_size;
            size = heap[start] >> 1;
            count--;

        }
    }
    if(end<126)
    {
        if((heap[end+1]&1)==0)//after is free
        {
            other_size = (heap[end+1] >> 1);
            total = other_size +size;
            heap[start]= total << 1;
            for(int i=start+1; i<(start+total)-1; i++)
            {
                heap[i]=0;
            }
            heap[start+total-1]= total << 1;
            count--;
        }
    }
}

int main(int argc, char *argv[])
{
    char size = 127;
    heap[0] = size<<1; // 127/0
    heap[126] = size << 1; // 127/0

    if (argc == 2 && strcmp(argv[1], "BestFit") == 0)
    {
        first_fit = false;
    }

    printf(">");
    char input[80];
    fgets(input, 80, stdin);
    char* command = strtok(input, " \n\t");

    while(strcmp("quit", command) != 0)
    {
        if(strcmp("malloc", command) == 0)
        {
            count++;
            char* bytes = strtok(NULL, " \n\t");
            int num_bytes = atoi(bytes) + 2;
            int a = 0;
            if (first_fit)
            {
                while (a < 127)
                {
                    int if_allocated = heap[a] & 1;
                    int size = (heap[a] >> 1) & 127;
                    if (!if_allocated) //if it is free
                    {
                        if (size >= num_bytes )
                        {
                            heap[a] = (num_bytes<<1) | 1;
                            heap[a+num_bytes-1] = heap[a]; 
                            heap[a+num_bytes] = (size - num_bytes) << 1;
                            heap[a+num_bytes+(size-num_bytes)-1] = heap[a+num_bytes];
                            printf("%d\n", a+1);
                            break;
                        }
                    }
                    a = a + size;
                }
            }
            else
            {
                int best_a;
                int bestdiff = 127;
                int size = (heap[a] >> 1) &127;
                while (a < 127)
                {
                    if (((heap[a] & 1)==0) && (size >= num_bytes)) 
                    {
                        int diff = size - num_bytes; 
                        if (bestdiff > diff)
                        {
                            bestdiff = diff;
                            best_a = a;
                        }
                    }
                    a = a + size;
                    size = (heap[a] >> 1) &127;
                }
                //int end = best_a +
                size = (heap[best_a] >> 1) &127;
                heap[best_a] = (num_bytes<<1) | 1;
                heap[best_a+num_bytes-1] = heap[best_a]; 
                heap[best_a+num_bytes] = (size - num_bytes) << 1; 
                //printf("best_a: %d num_bytes: %d, size: %d, size-num_bytes = %d\n",best_a, num_bytes, size, size-num_bytes);
                heap[best_a+num_bytes+(size-num_bytes)-1] = heap[best_a+num_bytes];
                printf("%d\n", best_a+1); 
                /*
                for(int i=0; i<127; i++)
                {
                    printf("size: %d, bit: %d\n", heap[i]>>1, heap[i]&1);
                }*/
            }
        }
        else if(strcmp("free", command) == 0)
        {
            char* free = strtok(NULL, " \n\t");
            int index = atoi(free);
            index -=1;
            int size = heap[index] >> 1; // size of block

            heap[index] = heap[index] & -2; // set last bit to 0
            for (int f=index+1; f<((size+index)); f++) // check? sets indices for payload to 0
            {
                heap[f] = 0;
            }
            heap[index+size-1] = heap[index];
            combine_free(index, index+size-1);
            /*
            for(int i=0; i<127; i++)
            {
                printf("size: %d, bit: %d\n", heap[i]>>1, heap[i]&1);
            }*/
        }
        else if(strcmp("blocklist", command) == 0)
        {
            struct block block_info[count+1];
            make_struct_array(block_info);
            sort_struct_array(block_info);
            print_struct_array(block_info);
        }
        else if(strcmp("writemem", command) == 0)
        {
            char* mem_spot = strtok(NULL, " \n\t");
            int write_index = atoi(mem_spot);
            char* content = strtok(NULL, " \n\t");
            write_to_memory(content, write_index);
        }
        else if(strcmp("printmem", command) == 0)
        {
            char* other_mem_spot = strtok(NULL, " \n\t");
            int print_index = atoi(other_mem_spot);
            char* how_much = strtok(NULL, " \n\t");
            int num_chars_to_print = atoi(how_much);
            print_memory(print_index, num_chars_to_print);
        }
        printf(">");
        fgets(input, 80, stdin);
        char* command = strtok(input, " \n\t");
    }
}