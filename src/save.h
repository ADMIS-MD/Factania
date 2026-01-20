// Sangbeom Kim
// 01/14/2026

#pragma once

#include <stdbool.h>
#include <filesystem.h>
#include <fat.h> 
#include <unistd.h>

struct SaveData {
    float x, y, z;
    int angle_x;
    int angle_z;

    int save_count;
};

inline SaveData savedata;

bool save_write(const SaveData* sWrite);

bool save_load(SaveData* sLoad);

bool save_delete(void);
