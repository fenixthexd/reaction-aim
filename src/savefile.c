// -- Imports --
#include "savefile.h"

#include <raylib.h>
#include <stddef.h>

// -- Savefile functions --
void SaveHighScore(const char *filename, int score)
{
    SaveFileData(filename, &score, sizeof(int));
}

int LoadHighScore(const char *filename)
{
    int loaded_score = 0;
    int size = 0;

    unsigned char *data = LoadFileData(filename, &size);

    if (data != NULL)
    {
        if (size == sizeof(int))
        {
            loaded_score = *(int *)data;
        }

        UnloadFileData(data);
    }

    return loaded_score;
}