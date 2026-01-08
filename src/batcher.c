/*
 *  Batching of update sequences into a single multiplication
 *
 *  Create a series of instructions to define a math formula that describes
 *
 *  Time:
 *    - At 10 tps, a int overflow at uint32_t would be something like 13 years - we are fine. If we reach half
 *      of that, we could even just do a pass on all time values and reduce. On top of that, things would fill
 *      by then.
 *
 *  General:
 *    - This seems to require a simple CAS (computer algeba system) so this will be tough. Most things should
 *      be fairly simple but this will require attention
 *    - It's 
 */

enum SolveNodes {

}

/*
 *  Simple example
 *    
 *
 *
 *    [s1] -> [t] -> [u] -> [s2]
 *
 *    result:
 *      - s1 rseource become s2 resource
 *    limits:
 *      - s1 count must be > 0
 *      - S2 count must be < s2 max storage size
 *    values:
 *      - s1 has 500
 *      - s2 has 200 can hold 800 (can hold 600)
 *    timings:
 *      - s1 -> t takes 2 ticks
 *      - t processing takes 5 ticks
 *      - t -> u takes 2 ticks
 *      - u processing takes 5 ticks
 *      - u -> s2 takes 2 ticks
 *    
 *    formula for timing:
 *      - transfers = 2 * (500 + 2)
 *      - processing = 5 * (500 + 1)
 *      - total = (7 * 500) + (2 * 2) + (5)
 *      - = 3509 
 *
 *    pseudo:
 *      productive-ticks = ticks-powered - start-tick
 *      s2 count = max(0, 7 * productive-ticks - 9) while (productive-ticks <= 3509)
 *      s2 count = 700 while (productive-ticks > 3509)
 *
 *
 *      3, 4, 5, 6
 */

// assembly like dsl optimized for both storage and speed
// variables are provided by indices
// - 0 = productive-ticks
//
// variables >= 32 are outputs
enum TTA_BatchOperations {
  TTAOP_Result, // {count} {entity} * n - defines the results of the operations. max 32 (?) count, mapped to uint8_t + 32 indidces
                // 2b + count * 2b
  TTAOP_JumpMap, // {count} {variable} { {value} {goto index} } * n {goto index} - Jump to location based on how large of a value you have, last is else
                 // If its too high, it just means recalculate
                 // 5b + count * (6b)
  TTAOP_JumpMapOutOffset, // {count} {variable} {variable_out} { {value} {goto index} } *n - 1 { goto index } - Jumpmap but output the value variable - matched_value
                 // 6b + count * 6b
  TTAOP_End, // End computation, everything provided
                // 1b

  TTAOP_MulAdd, // {variable} {a} {b} {variable_out} -- variable * a + b -> variable_out
                // 11b
  TTAOP_MulDivAdd, // {variable} {a} {b} {c} {variable_out} -- (variable / b) * a + c -> variable_out
                   // 15b
  TTAOP_DivAdd, // {variable} {a} {b} {variable_out} -- (variable / b) + c -> variable_out
                // 11b
  TTAOP_PushResult, // {a} {variable_out} -- puts value into variable_out
                    // 6b
}

/*
 * Example but with operations:
 *
 *  s1: 
 *    - Result 1 [s1]
 *    - JumpMap 1 0 (( < 3493 )) (( >= 3493 ))
 *    -   Label( < 3493 )
 *    - MulDivAdd 0 -1 7 499 32
 *    - End
 *    -   Label( >= 3493 )
 *    - PushResult 0 32
 *    - End
 *  = 4 + 11 + 15 + 1 + 6 + 1 = 36 bytes
 *
 *  t: // Cna't be bothered to finish
 *    - JumpMap
 *    - Mod 0 7
 *    - JumpMap
 *
 *  s2:
 *    - Result 1 [s1]
 *    - JumpMapOutOFfset 2 0 33 ((<16)) ((<3509)) ((>=3509))
 *    -  Label(<16)
 *    - PushResult 0 32
 *    - End
 *    -  Label(< 3509)
 *    - DivAdd 0 7 201 32
 *    - End
 *    -  Label(>=3509)
 *    - PushResult 700
 *    - END
 *  = 4 + 18 + 6 + 1 + 11 + 1 + 6 + 1 = 48 bytes
 *
 *
 *  This can be improved w/ more intelligent integer size choices or better instructions
 *
 *  This is great! How do we create this?
 */

void basic_example() {
  
}/
