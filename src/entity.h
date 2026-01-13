/**
 *
 * Entity storage definition
 *
 *  Entities are weird in C.
 *
 *
 *  Each Entity table is of this structure:
 *    - [ ENTITY ID,  COMPONENT A,  COMPONENT B,  COMPONENT C ]
 *    -   5           data...       data...       data...
 *    -   6           data...       .             .
 *    -   7           .             .             .
 *    -   19          .             .             .
 *    -   0           .             .             .
 *
 *  Most of the time, the data will be passed to you as an array using the columns
 *
 *  Entities are constant and defined by archetypes, meaning archetypes are predefined
 *  and entities register for specific archetypes.
 */

#include <nds/ndstypes.h>
#include <stdbool.h>

typedef uint32_t Entity;

// Required things for all gameplay components
// - Most of the functions operate on columns instead of single components.
//   Column size can be 1!
// - Due to time constraints, entities are limited in the fact that components
//   cannot be added or removed without archetype "transmutation", which should
//   be avoided.
struct ComponentDefinition {
  //// De-/Serialization
  
  // If both sizes are 0, component is a tag and serialization is not done
  u16 serialized_size;   // Size of object when serialized
  u16 deserialized_size; // Size of object deserialized - `sizeof(obj)` 

  // Place the component in the storage provided
  void (*serialize) (
      void const* component_col, 
      Entity const* id_col, 
      void* storage_col, 
      int column_size
  );
  // Place the component in the component ptr provided
  void (*deserialize) (
      void const* storage_col, 
      void* component_col, 
      Entity* id_col,
      int column_size
  );


  //// Ticking Rules
  
  // The number of frames between this entity's ticks
  u16 tick_every_n_frames;
  // The ticking function
  void (*tick) (
      void* component_col, 
      Entity* id_col,
      int column_size
  );
};


