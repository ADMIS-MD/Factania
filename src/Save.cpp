// Sangbeom Kim
// 01/14/2026

#include "Save.h"

#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define SAVE_PATH_MAX 512

static bool BuildSavePaths(char out_path[SAVE_PATH_MAX], char out_tmp[SAVE_PATH_MAX]) 
{
    char* cwd = fatGetDefaultCwd();
    if (!cwd) {
        return false;
    }

    snprintf(out_path, SAVE_PATH_MAX, "%sFactania.sav", cwd);
    snprintf(out_tmp, SAVE_PATH_MAX, "%sFactania.tmp", cwd);

    free(cwd);
    return true;
}

bool LoadSave(SaveData* sLoad) 
{
    char savePath[SAVE_PATH_MAX];
    char tmpPath[SAVE_PATH_MAX];

    if (!BuildSavePaths(savePath, tmpPath)) {
        return false;
    }

    FILE* f = fopen(savePath, "rb");
    if (!f) {
        return false;
    }

    SaveData savedata;
    size_t r = fread(&savedata, 1, sizeof(savedata), f);
    if (fclose(f) != 0) {
        return false;
    }

    if (r != sizeof(savedata)) {
        return false;
    }

    *sLoad = savedata;
    return true;
}

bool WriteSave(const SaveData* sWrite) 
{
    char savePath[SAVE_PATH_MAX];
    char tmpPath[SAVE_PATH_MAX];

    if (!BuildSavePaths(savePath, tmpPath)) {
        return false;
    }

    FILE* f = fopen(tmpPath, "wb");
    if (!f) {
        return false;
    }

    size_t w = fwrite(sWrite, 1, sizeof(*sWrite), f);
    if (fclose(f) != 0) {
        return false;
    }

    if (w != sizeof(*sWrite)) {
        return false;
    }

    if (rename(tmpPath, savePath) == 0) {
        return true;
    }

    remove(savePath);
    if (rename(tmpPath, savePath) == 0) {
        return true;
    }

    return false;
}

bool DeleteSave(void) 
{
    char savePath[SAVE_PATH_MAX];
    char tmpPath[SAVE_PATH_MAX];

    if (!BuildSavePaths(savePath, tmpPath)) {
        return false;
    }

    bool ok = true;

    if (remove(savePath) != 0) {
        if (errno != ENOENT) ok = false;
    }

    if (remove(tmpPath) != 0) {
        if (errno != ENOENT) ok = false;
    }

    return ok;
}