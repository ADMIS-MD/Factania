
/*
 *  This scheduler is intended for any sort of timed logic, primarially for gameplay code.
 *
 *  The scehduler is essentially an event list, with multiple tiers. The lowest tier happens
 *  every tick, and each tier above happens when the the teir below it runs out of ticks.
 *
 *  - Schedulers do their best to compress themselves when they can, as iterative memory 
 *    access can make running through the scheduler tasks faster
 */

typedef struct TTA_SchedulerNode {
  
} TTA_SchedulerNode;

typedef struct TTA_SOmething {

} TTA_SOmething;


