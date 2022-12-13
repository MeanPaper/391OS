#include "schedule.h"
#include "sys_call_helper.h"
#include "paging.h"

#define SQR_WAVE_MODE 0x36      // square wave mode, use by most of the OS
#define PIT_APPR_MAX  1193180   // this is approximately the highest requency for the pit
#define FREQ          100 //40
#define MAX_TERM      3

/*
 * pit_handler
 * Descritption: schedule processes in multi terminals
 * 
 * Input: none
 * Output: none
 * Return value: none
 * Side effect: video memory, vidmap, current running program will constantly change
 *  in the background
 */
void pit_handler(){
    int32_t next_term;

    send_eoi(PIT_IRQ_POS);
    if(active_terminal[0]==-1){
        execute_on_term((uint8_t*)"shell", 0);
    }

    if(get_process_total() < 6){
        if(active_terminal[display_terminal] == -1){
            map_sched_video_page(display_terminal);
            int availiable = get_availiable_pid();
            register uint32_t ebp_tmp asm("ebp");
            register uint32_t esp_tmp asm("esp");
            current_term_id = display_terminal;
            pcb_t * current = (pcb_t*)(GET_PCB(availiable));
            current->sched_ebp = ebp_tmp;
            current->sched_esp = esp_tmp;
            execute_on_term((uint8_t*)"shell", display_terminal);
        }
    }

    pcb_t * current = (pcb_t*)(GET_PCB(current_pid_num));
    asm volatile(
        "movl   %%ebp, %0;"
        "movl   %%esp, %1;"
        :"=r"(current->sched_ebp), "=r"(current->sched_esp)
    );

    next_term = (current_term_id + 1) % MAX_TERM;
    while(active_terminal[next_term] == -1){
        next_term = (next_term + 1) % MAX_TERM;
    }
    current_term_id = (uint32_t)next_term;
    pcb_t * next_proc = (pcb_t*)(GET_PCB(active_terminal[next_term]));
    map_program_page(next_proc->pid);
    map_sched_video_page(next_term);
    map_vidmap_page((PROG_128MB << 1) + (next_term * FOUR_KB), next_term);    // loading new video page

    current_pid_num = next_proc->pid;

    tss.ss0 = KERNEL_DS;
    tss.esp0 = EIGHT_MB - 4 - (EIGHT_KB * (next_proc->pid -1));


    asm volatile(
        "movl   %0, %%ebp;"
        "movl   %1, %%esp;"
        : 
        :"r"(next_proc->sched_ebp), "r"(next_proc->sched_esp)
    );

    return;
}


/* void pit_init();
 * Description: Initialize our pit 
 * 
 * Inputs: none
 * Output: none
 * Return Value: none
 * Side Effects: set the corresponding global flag variable, i.e. caps/shift/alt/ctrl 
*/
void pit_init(){
    cli();
    uint32_t divisor = PIT_APPR_MAX / FREQ;
    outb(SQR_WAVE_MODE, PIT_CMD_PORT);
    outb( divisor & 0xFF, PIT_DATA_0); // sending low bytes
    outb( divisor >> 8, PIT_DATA_0); // sending high bytes
    enable_irq(PIT_IRQ_POS);
    sti();
}
