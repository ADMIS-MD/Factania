// save.h
#pragma once

#include <stdbool.h>
#include <filesystem.h>
#include <fat.h> 
#include <unistd.h>

typedef struct SaveData {
    float x, y, z;
    int angle_x;
    int angle_z;

    int save_count;
} SaveData;

bool save_write(const SaveData* sWrite);

bool save_load(SaveData* sLoad);

bool save_delete(void);
