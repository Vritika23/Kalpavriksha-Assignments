// Smart Memory Manager
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_POOL 1000000

struct Block {
    size_t start;
    size_t size;
    int id;
    int isAllocated;
};

struct Manager {
    unsigned char *pool;
    size_t poolSize;
    struct Block *blocks;
    int count;
    int capacity;
    int nextId;
    int strategy;               // 1 = first fit, 2 = best fit, 3 = worst fit
    size_t lastFailed;
};

static const char *const strategyNames[] = { "", "First Fit", "Best Fit", "Worst Fit" };

long getNumber(const char *prompt)
{
    char line[100], *end;
    long value;

    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\nInput ended.\n");
            exit(1);
        }
        if (strchr(line, '\n') == NULL && strlen(line) == sizeof(line) - 1) {
            int c = getchar();
            while (c != '\n' && c != EOF)
                c = getchar();
            printf("Input is too long.\n");
            continue;
        }
        errno = 0;
        value = strtol(line, &end, 10);
        while (end != line && isspace((unsigned char)*end))
            end++;
        if (end == line || *end != '\0') {
            printf("Please enter a whole number.\n");
            continue;
        }
        if (errno == ERANGE) {
            printf("That number is too big.\n");
            continue;
        }
        return value;
    }
}

// inserts a block (grows the table with realloc if full)
int addBlock(struct Manager *m, int index, struct Block b)
{
    if (m->count == m->capacity) {
        struct Block *bigger = realloc(m->blocks, m->capacity * 2 * sizeof(struct Block));
        if (bigger == NULL)
            return 0;
        m->blocks = bigger;
        m->capacity = m->capacity * 2;
    }
    memmove(&m->blocks[index + 1], &m->blocks[index], (m->count - index) * sizeof(struct Block));
    m->blocks[index] = b;
    m->count++;
    return 1;
}

void removeBlock(struct Manager *m, int index)
{
    memmove(&m->blocks[index], &m->blocks[index + 1], (m->count - index - 1) * sizeof(struct Block));
    m->count--;
}

int findId(struct Manager *m, int id)
{
    int i;
    for (i = 0; i < m->count; i++) {
        if (m->blocks[i].isAllocated && m->blocks[i].id == id)
            return i;
    }
    return -1;
}

int findStart(struct Manager *m, size_t start)
{
    int i = 0;
    while (m->blocks[i].start != start)
        i++;
    return i;
}

void mergeBlocks(struct Manager *m)
{
    int i = 0;
    while (i + 1 < m->count) {
        if (!m->blocks[i].isAllocated && !m->blocks[i + 1].isAllocated) {
            m->blocks[i].size += m->blocks[i + 1].size;
            removeBlock(m, i + 1);
        } else {
            i++;
        }
    }
}

void getFreeInfo(struct Manager *m, size_t *total, size_t *largest)
{
    int i;
    *total = 0;
    *largest = 0;
    for (i = 0; i < m->count; i++) {
        if (!m->blocks[i].isAllocated) {
            *total += m->blocks[i].size;
            if (m->blocks[i].size > *largest)
                *largest = m->blocks[i].size;
        }
    }
}

int firstFit(struct Manager *m, size_t size)
{
    int i;
    for (i = 0; i < m->count; i++) {
        if (!m->blocks[i].isAllocated && m->blocks[i].size >= size)
            return i;
    }
    return -1;
}

int bestFit(struct Manager *m, size_t size)
{
    int i, best = -1;
    for (i = 0; i < m->count; i++) {
        if (!m->blocks[i].isAllocated && m->blocks[i].size >= size) {
            if (best == -1 || m->blocks[i].size < m->blocks[best].size)
                best = i;
        }
    }
    return best;
}

int worstFit(struct Manager *m, size_t size)
{
    int i, worst = -1;
    for (i = 0; i < m->count; i++) {
        if (!m->blocks[i].isAllocated && m->blocks[i].size >= size) {
            if (worst == -1 || m->blocks[i].size > m->blocks[worst].size)
                worst = i;
        }
    }
    return worst;
}

int findBlock(struct Manager *m, size_t size)
{
    if (m->strategy == 2)
        return bestFit(m, size);
    if (m->strategy == 3)
        return worstFit(m, size);
    return firstFit(m, size);
}

void explainFragmentation(struct Manager *m, size_t size)
{
    size_t total, largest;
    int i, first = 1;

    getFreeInfo(m, &total, &largest);
    if (total < size) {
        printf("Only %zu bytes are free in total, so there is not enough memory.\n", total);
        return;
    }
    printf("External fragmentation: %zu bytes asked, %zu bytes free in total, but split as ", size, total);
    for (i = 0; i < m->count; i++) {
        if (!m->blocks[i].isAllocated) {
            if (!first)
                printf(" + ");
            printf("%zu", m->blocks[i].size);
            first = 0;
        }
    }
    printf(" (largest piece is %zu).\n", largest);
}

int sizeIsValid(struct Manager *m, long size)
{
    if (size <= 0 || (size_t)size > m->poolSize) {
        printf("Invalid size. It must be between 1 and %zu.\n", m->poolSize);
        return 0;
    }
    return 1;
}

int useBlock(struct Manager *m, int index, size_t size, int id)
{
    size_t rest = m->blocks[index].size - size;

    if (rest > 0) {
        struct Block leftover = { m->blocks[index].start + size, rest, 0, 0 };
        if (!addBlock(m, index + 1, leftover))
            return 0;
    }
    m->blocks[index].size = size;
    m->blocks[index].id = id;
    m->blocks[index].isAllocated = 1;
    return 1;
}

void allocateMemory(struct Manager *m, long request)
{
    int index;
    size_t size;

    if (!sizeIsValid(m, request))
        return;
    size = (size_t)request;

    index = findBlock(m, size);
    if (index == -1) {
        m->lastFailed = size;
        printf("Allocation failed: no contiguous free block is large enough.\n");
        explainFragmentation(m, size);
        return;
    }
    if (!useBlock(m, index, size, m->nextId)) {
        printf("Allocation failed: could not grow the block table.\n");
        return;
    }

    memset(m->pool + m->blocks[index].start, m->nextId, size);
    m->lastFailed = 0;
    printf("Allocation successful. ID: %d, Start: %zu, Size: %zu\n", m->nextId, m->blocks[index].start, size);
    m->nextId++;
}

void releaseMemory(struct Manager *m, long id)
{
    int index;

    if (id <= 0 || id >= m->nextId) {
        printf("Release failed: unknown allocation ID.\n");
        return;
    }
    index = findId(m, (int)id);
    if (index == -1) {
        printf("Release failed: allocation #%ld is already free.\n", id);
        return;
    }
    m->blocks[index].isAllocated = 0;
    m->blocks[index].id = 0;
    mergeBlocks(m);
    printf("Allocation released.\n");
}

int shrinkBlock(struct Manager *m, int i, size_t newSize)
{
    size_t cut = m->blocks[i].size - newSize;

    if (i + 1 < m->count && !m->blocks[i + 1].isAllocated) {
        m->blocks[i + 1].start -= cut;
        m->blocks[i + 1].size += cut;
    } else {
        struct Block tail = { m->blocks[i].start + newSize, cut, 0, 0 };
        if (!addBlock(m, i + 1, tail))
            return 0;
    }
    m->blocks[i].size = newSize;
    return 1;
}

int growInPlace(struct Manager *m, int i, size_t newSize)
{
    size_t extra = newSize - m->blocks[i].size;

    if (i + 1 >= m->count || m->blocks[i + 1].isAllocated || m->blocks[i + 1].size < extra)
        return 0;

    if (m->blocks[i + 1].size == extra) {
        removeBlock(m, i + 1);
    } else {
        m->blocks[i + 1].start += extra;
        m->blocks[i + 1].size -= extra;
    }
    m->blocks[i].size = newSize;
    return 1;
}

void moveBlock(struct Manager *m, int i, size_t newSize)
{
    int id = m->blocks[i].id;
    size_t oldStart = m->blocks[i].start;
    size_t oldSize = m->blocks[i].size;
    size_t newStart;
    int target;

    target = findBlock(m, newSize);
    if (target == -1) {
        m->lastFailed = newSize;
        printf("Resize failed: it cannot grow in place and no other free block can hold %zu bytes "
               "(a move needs a separate free block). The allocation is unchanged.\n", newSize);
        explainFragmentation(m, newSize);
        return;
    }
    if (!useBlock(m, target, newSize, id)) {
        printf("Resize failed: could not grow the block table. The allocation is unchanged.\n");
        return;
    }

    newStart = m->blocks[target].start;
    memcpy(m->pool + newStart, m->pool + oldStart, oldSize);

    i = findStart(m, oldStart);         // the old block may have moved in the table
    m->blocks[i].isAllocated = 0;
    m->blocks[i].id = 0;
    mergeBlocks(m);
    printf("Allocation #%d moved from %zu to %zu and resized to %zu bytes.\n", id, oldStart, newStart, newSize);
}

// shrink, grow in place, or move (in that order)
void resizeMemory(struct Manager *m, long id, long request)
{
    int i;
    size_t newSize;

    if (id <= 0 || id >= m->nextId) {
        printf("Resize failed: unknown allocation ID.\n");
        return;
    }
    i = findId(m, (int)id);
    if (i == -1) {
        printf("Resize failed: allocation #%ld is already free.\n", id);
        return;
    }
    if (!sizeIsValid(m, request))
        return;
    newSize = (size_t)request;

    if (newSize == m->blocks[i].size) {
        printf("Allocation #%ld already has that size.\n", id);
    } else if (newSize < m->blocks[i].size) {
        if (shrinkBlock(m, i, newSize))
            printf("Allocation #%ld shrunk to %zu bytes.\n", id, newSize);
        else
            printf("Resize failed: could not grow the block table. The allocation is unchanged.\n");
    } else if (growInPlace(m, i, newSize)) {
        printf("Allocation #%ld grew in place to %zu bytes.\n", id, newSize);
    } else {
        moveBlock(m, i, newSize);
    }
}

void showMap(struct Manager *m)
{
    int i;
    printf("Memory Map (%zu bytes):\n", m->poolSize);
    for (i = 0; i < m->count; i++) {
        if (m->blocks[i].isAllocated)
            printf("[Allocated #%d: %zu bytes, Start: %zu]\n", m->blocks[i].id, m->blocks[i].size, m->blocks[i].start);
        else
            printf("[Free: %zu bytes, Start: %zu]\n", m->blocks[i].size, m->blocks[i].start);
    }
}

void showStats(struct Manager *m)
{
    size_t freeBytes, largest;
    int i, usedBlocks = 0, freeBlocks = 0;

    getFreeInfo(m, &freeBytes, &largest);
    for (i = 0; i < m->count; i++) {
        if (m->blocks[i].isAllocated)
            usedBlocks++;
        else
            freeBlocks++;
    }

    printf("Total memory: %zu bytes\n", m->poolSize);
    printf("Allocated: %zu bytes\n", m->poolSize - freeBytes);
    printf("Free: %zu bytes\n", freeBytes);
    printf("Allocated blocks: %d\n", usedBlocks);
    printf("Free blocks: %d\n", freeBlocks);
    printf("Largest free block: %zu bytes\n", largest);

    if (m->lastFailed > 0 && freeBytes >= m->lastFailed && largest < m->lastFailed)
        printf("Note: The request for %zu bytes failed even though %zu bytes are free, "
               "because the free memory is fragmented.\n", m->lastFailed, freeBytes);
}

int initManager(struct Manager *m, size_t poolSize)
{
    m->pool = malloc(poolSize);
    m->blocks = malloc(4 * sizeof(struct Block));
    if (m->pool == NULL || m->blocks == NULL) {
        free(m->pool);
        free(m->blocks);
        return 0;
    }
    memset(m->pool, 0, poolSize);
    m->poolSize = poolSize;
    m->capacity = 4;
    m->nextId = 1;
    m->strategy = 1;
    m->lastFailed = 0;

    m->blocks[0].start = 0;
    m->blocks[0].size = poolSize;
    m->blocks[0].id = 0;
    m->blocks[0].isAllocated = 0;
    m->count = 1;
    return 1;
}

void chooseStrategy(struct Manager *m)
{
    long choice;
    int valid;

    do {
        choice = getNumber("Select allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ");
        valid = (choice >= 1 && choice <= 3);
        if (!valid)
            printf("Invalid choice. Enter 1, 2 or 3.\n");
    } while (!valid);
    m->strategy = (int)choice;
    printf("Strategy set to %s.\n", strategyNames[m->strategy]);
}

void destroyManager(struct Manager *m)
{
    free(m->pool);
    free(m->blocks);
    free(m);
}

void printMenu(void)
{
    printf("\nMenu:\n");
    printf("1. Allocate memory\n");
    printf("2. Release memory\n");
    printf("3. Resize allocation\n");
    printf("4. Display memory map\n");
    printf("5. Display memory statistics\n");
    printf("6. Change allocation strategy\n");
    printf("7. Exit\n");
}

int main(void)
{
    struct Manager *m = malloc(sizeof(*m));
    long poolSize, choice, id, size;
    int valid;

    if (m == NULL) {
        printf("Error: could not allocate memory.\n");
        return 1;
    }

    do {
        poolSize = getNumber("Enter memory pool size: ");
        valid = (poolSize >= 1 && poolSize <= MAX_POOL);
        if (!valid)
            printf("Pool size must be between 1 and %d.\n", MAX_POOL);
    } while (!valid);
    if (!initManager(m, (size_t)poolSize)) {
        printf("Error: could not allocate memory.\n");
        free(m);
        return 1;
    }
    chooseStrategy(m);

    do {
        printMenu();
        choice = getNumber("Choice: ");

        switch (choice) {
        case 1:
            size = getNumber("Requested size: ");
            allocateMemory(m, size);
            break;
        case 2:
            id = getNumber("Allocation ID: ");
            releaseMemory(m, id);
            break;
        case 3:
            id = getNumber("Allocation ID: ");
            size = getNumber("New size: ");
            resizeMemory(m, id, size);
            break;
        case 4:
            showMap(m);
            break;
        case 5:
            showStats(m);
            break;
        case 6:
            chooseStrategy(m);
            break;
        case 7:
            break;
        default:
            printf("Invalid choice. Enter a number from 1 to 7.\n");
        }
    } while (choice != 7);

    destroyManager(m);
    printf("Goodbye.\n");
    return 0;
}
