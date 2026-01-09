//
// Created by willb on 1/8/26.
//

#ifndef FACTANIA_FACTORY_OBJ_H
#define FACTANIA_FACTORY_OBJ_H

#include <nds/ndstypes.h>

typedef enum TTA_FODef_InventoryBin_Flags {
    TTA_FODef_InventoryBin_ACCESS_ANYWHERE_IN = (1 << 0),
    TTA_FODef_InventoryBin_ACCESS_ANYWHERE_OUT = (1 << 1)
} TTA_FODef_InventoryBin_Flags;

// Defines a specific part of an inventory
typedef struct TTA_FODef_InventoryBin {
    u8 port_coords[2];      // Where this inventory can be externally accessed
    u8 slots;               // Number of item types
    u8 slot_max;            // Number of items per slot (generally should be u8_max)
    u16 flags;            // see TTA_FODef_InventoryBin_Flags

    const char* name;
    void* layout;     // TODO: How

    // TODO: Define item classes for filters (ex: smeltable)
} TTA_FODef_InventoryBin;

typedef struct TTA_FODef_Action {
    u8 input_bin;
    u8 output_bin;
    void* recipie; // TODO: How
    u16 timeout;
    u8 batch_size;

    void* resource_requirements; // Power, etc. TODO: How
} TTA_FODef_Action;

// Factory object definition
typedef struct TTA_FODef {
    TTA_FODef_InventoryBin* bins;
    TTA_FODef_Action* actions;

    u8 bin_count;
    u8 action_count;

    u8 obj_size[2];
    const void* display;    // TODO: define graphics display
} TTA_FODef;

typedef struct TTA_FactoryObj {
    TTA_FODef* def;


} TTA_FactoryObj;

/*
*N - Non-sequential cycle
Requests a transfer to/from an address which is NOT related to the address used in the previous cycle. (Called 1st Access in GBA language).
The execution time for 1N is 1 clock cycle (plus non-sequential access waitstates).

S - Sequential cycle
Requests a transfer to/from an address which is located directly after the address used in the previous cycle. Ie. for 16bit or 32bit accesses at incrementing addresses, the first access is Non-sequential, the following accesses are sequential. (Called 2nd Access in GBA language).
The execution time for 1S is 1 clock cycle (plus sequential access waitstates).

I - Internal Cycle
CPU is just too busy, not even requesting a memory transfer for now.
The execution time for 1I is 1 clock cycle (without any waitstates).

C - Coprocessor Cycle
The CPU uses the data bus to communicate with the coprocessor (if any), but no memory transfers are requested.
*/

#endif //FACTANIA_FACTORY_OBJ_H