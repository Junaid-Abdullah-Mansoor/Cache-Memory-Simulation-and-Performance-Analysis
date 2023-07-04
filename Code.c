#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_TRACE_LINE 100

int main()
{
    // Collect inputs
    int address_bits;
    int cache_size_bytes;
    int block_size_bytes;
    int associativity;
    char trace_file_name[MAX_TRACE_LINE];
    printf("Enter number of address bits: ");
    scanf("%d", &address_bits);
    printf("Enter cache size (in bytes): ");
    scanf("%d", &cache_size_bytes);
    printf("Enter block size (in bytes): ");
    scanf("%d", &block_size_bytes);
    printf("Enter associativity: ");
    scanf("%d", &associativity);
    printf("Enter trace file name: ");
    scanf("%s", trace_file_name);

    // Calculate tag/index/offset bits
    int offset_bits = log2(block_size_bytes);
    int index_bits;
    int tag_bits;
    if (associativity == 1) {
        // Direct-mapped cache
        index_bits = log2(cache_size_bytes / block_size_bytes);
        tag_bits = address_bits - offset_bits - index_bits;
    } else {
        // 2-way associative cache
        index_bits = log2(cache_size_bytes / (block_size_bytes * associativity));
        tag_bits = address_bits - offset_bits - index_bits - log2(associativity);
    }
    int num_sets = pow(2, index_bits);
    int num_lines = associativity;

    // Initialize cache
    int cache[num_sets][num_lines];
    for (int i = 0; i < num_sets; i++) {
        for (int j = 0; j < num_lines; j++) {
            cache[i][j] = 0;
        }
    }

    // Open trace file
    FILE *trace_file = fopen(trace_file_name, "r");
    if (trace_file == NULL) {
        printf("Error opening trace file.\n");
        return 1;
    }

    // Simulate cache accesses
    char line[MAX_TRACE_LINE];
    int hits = 0;
    int misses = 0;
    while (fgets(line, MAX_TRACE_LINE, trace_file) != NULL) {
        // Extract address
        unsigned int address;
        sscanf(line, "%x", &address);

        // Calculate tag, index, and offset
        unsigned int tag = address >> (offset_bits + index_bits);
        unsigned int index = (address >> offset_bits) & ((1 << index_bits) - 1);

        // Check cache for hit or miss
        int hit = 0;
        for (int i = 0; i < num_lines; i++) {
            if (cache[index][i] == tag) {
                hit = 1;
                hits++;
                printf("Hit at address %x\n", address);
                break;
            }
        }
        if (!hit) {
            misses++;
            printf("Miss at address %x\n", address);
            // Update cache with new tag
            for (int i = num_lines - 1; i >= 1; i--) {
                cache[index][i] = cache[index][i-1];
            }
            cache[index][0] = tag;
        }
    }

    // Close trace file
    fclose(trace_file);

    // Output results
    printf("Number of hits: %d\n", hits);
    printf("Number of misses: %d\n", misses);
    printf("Hit rate: %.2f%%\n", 100.0 * hits / (hits + misses));

    // Calculate cache size and overhead
int cache_overhead_bytes;
if (associativity == 1) {
    // Direct-mapped cache
    cache_overhead_bytes = num_lines * (tag_bits + 1);
} else {
    // 2-way associative cache
    cache_overhead_bytes = num_sets * num_lines * (tag_bits + 1 + 1);
}
int total_cache_size_bytes = cache_size_bytes + cache_overhead_bytes;

// Output cache parameters and statistics
printf("\nCache Parameters\n");
printf("Number of sets: %d\n", num_sets);
printf("Number of lines: %d\n", num_lines);
printf("Tag bits: %d\n", tag_bits);
printf("Index bits: %d\n", index_bits);
printf("Offset bits: %d\n", offset_bits);
printf("Cache overhead: %d bytes\n", cache_overhead_bytes);
printf("Total cache size: %d bytes\n", total_cache_size_bytes);

// Output hit and miss addresses
printf("\nHit Addresses:\n");
for (int i = 0; i < num_sets; i++) {
    for (int j = 0; j < num_lines; j++) {
        if (cache[i][j] != 0) {
            printf("Set %d, Line %d: %x\n", i, j, cache[i][j]);
        }
    }
}
printf("\nMiss Addresses:\n");
fseek(trace_file, 0, SEEK_SET); // Move file pointer to beginning of file
while (fgets(line, MAX_TRACE_LINE, trace_file) != NULL) {
    // Extract address
    unsigned int address;
    sscanf(line, "%x", &address);

    // Calculate tag, index, and offset
    unsigned int tag = address >> (offset_bits + index_bits);
    unsigned int index = (address >> offset_bits) & ((1 << index_bits) - 1);

    // Check cache for hit or miss
    int hit = 0;
    for (int i = 0; i < num_lines; i++) {
        if (cache[index][i] == tag) {
            hit = 1;
            break;
        }
    }
    if (!hit) {
        printf("%x\n", address);
    }
}

// Close trace file
fclose(trace_file);

getchar();
return 0;
}
