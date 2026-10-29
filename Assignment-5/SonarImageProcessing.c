#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MIN_N 2
#define MAX_N 10
#define MAX_INTENSITY 255
#define INPUT_LEN 100

typedef struct
{
    int n;
    int *data;
    int **cells;
} SonarImage;

int readSize(void)
{
    char line[INPUT_LEN];
    char extra;
    int n;

    while (1)
    {
        printf("Enter matrix size (%d-%d): ", MIN_N, MAX_N);
        if (fgets(line, sizeof(line), stdin) == NULL)
            return -1;

        if (strchr(line, '\n') == NULL)
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;
            printf("Input too long.\n");
            continue;
        }
        if (sscanf(line, "%d %c", &n, &extra) != 1)
        {
            printf("Enter only an integer.\n");
            continue;
        }
        if (n < MIN_N || n > MAX_N)
        {
            printf("Size must be between %d and %d.\n", MIN_N, MAX_N);
            continue;
        }
        return n;
    }
}

void imageDestroy(SonarImage *image)
{
    free(image->data);
    free(image->cells);
    image->data = NULL;
    image->cells = NULL;
}

int imageCreate(SonarImage *image, int n)
{
    image->n = n;
    image->data = malloc(n * n * sizeof(int));
    image->cells = malloc(n * n * sizeof(int *));

    if (image->data == NULL || image->cells == NULL)
    {
        imageDestroy(image);
        return 0;
    }
    return 1;
}

int **imageCell(const SonarImage *image, int row, int col)
{
    return image->cells + (row * image->n) + col;
}

void imageFill(SonarImage *image)
{
    int *value = image->data;
    int **cell = image->cells;
    int **end = image->cells + (image->n * image->n);

    while (cell < end)
    {
        *value = rand() % (MAX_INTENSITY + 1);
        *cell = value;
        value++;
        cell++;
    }
}

void imagePrint(const SonarImage *image)
{
    int **cell = image->cells;
    int **rowEnd;
    int **end = image->cells + (image->n * image->n);

    while (cell < end)
    {
        rowEnd = cell + image->n;
        while (cell < rowEnd)
        {
            printf("%4d", **cell);
            cell++;
        }
        printf("\n");
    }
}

static void cycleFour(int **top, int **right, int **bottom, int **left)
{
    int *saved = *top;
    *top = *left;
    *left = *bottom;
    *bottom = *right;
    *right = saved;
}

static void rotateLayer(int **topLeft, int n, int span)
{
    int **top = topLeft;
    int **right = topLeft + span;
    int **bottom = topLeft + (span * n) + span;
    int **left = topLeft + (span * n);
    int **topEnd = topLeft + span;

    while (top < topEnd)
    {
        cycleFour(top, right, bottom, left);
        top++;
        right += n;
        bottom--;
        left -= n;
    }
}

void rotateClockwise(SonarImage *image)
{
    int n = image->n;
    int **layerStart = image->cells;
    int span = n - 1;

    while (span > 0)
    {
        rotateLayer(layerStart, n, span);
        layerStart += n + 1;
        span -= 2;
    }
}

static void loadRow(const SonarImage *image, int row, int *buffer)
{
    int **cell = imageCell(image, row, 0);
    int **end = cell + image->n;

    while (cell < end)
    {
        *buffer = **cell;
        buffer++;
        cell++;
    }
}

static void addWindow(const int *rowBuffer, int col, int n, int *sum, int *count)
{
    const int *p = (col > 0) ? rowBuffer + col - 1 : rowBuffer + col;
    const int *last = (col < n - 1) ? rowBuffer + col + 1 : rowBuffer + col;

    while (p <= last)
    {
        *sum += *p;
        (*count)++;
        p++;
    }
}

static void smoothRow(const SonarImage *image, int row,
                      const int *prev, const int *curr, const int *next)
{
    int n = image->n;
    int **out = imageCell(image, row, 0);

    for (int col = 0; col < n; col++)
    {
        int sum = 0, count = 0;

        if (prev != NULL)
            addWindow(prev, col, n, &sum, &count);
        addWindow(curr, col, n, &sum, &count);
        if (next != NULL)
            addWindow(next, col, n, &sum, &count);

        **out = sum / count;
        out++;
    }
}

int smoothImage(SonarImage *image)
{
    int n = image->n;
    int *prev = malloc(n * sizeof(int));
    int *curr = malloc(n * sizeof(int));
    int *next = malloc(n * sizeof(int));
    int *tmp;

    if (prev == NULL || curr == NULL || next == NULL)
    {
        free(prev);
        free(curr);
        free(next);
        return 0;
    }

    loadRow(image, 0, curr);
    loadRow(image, 1, next);

    for (int row = 0; row < n; row++)
    {
        smoothRow(image, row,
                  (row > 0) ? prev : NULL,
                  curr,
                  (row < n - 1) ? next : NULL);

        tmp = prev;
        prev = curr;
        curr = next;
        next = tmp;

        if (row + 2 < n)
            loadRow(image, row + 2, next);
    }

    free(prev);
    free(curr);
    free(next);
    return 1;
}

int main(void)
{
    SonarImage image;
    int n = readSize();

    if (n < 0)
        return 1;

    srand((unsigned)time(NULL));

    if (!imageCreate(&image, n))
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    imageFill(&image);
    printf("\nOriginal Randomly Generated Matrix:\n");
    imagePrint(&image);

    rotateClockwise(&image);
    printf("\nMatrix after 90 degree Clockwise Rotation:\n");
    imagePrint(&image);

    if (!smoothImage(&image))
        printf("Memory allocation failed during filtering.\n");
    else
    {
        printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
        imagePrint(&image);
    }

    imageDestroy(&image);
    return 0;
}
