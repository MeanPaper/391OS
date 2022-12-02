#include "schedule.h"
#include "sys_call_helper.h"
#include "paging.h"

#define SQR_WAVE_MODE 0x36      // square wave mode, use by most of the OS
#define PIT_APPR_MAX  1193180   // this is approximately the highest requency for the pit
#define FREQ          100 //40
#define MAX_TERM      3

// int flag = 0;
int launch_terminal = 0;

void pit_handler(){
    // return;    
    // cli();
    int32_t next_term;
    uint32_t shell_esp, shell_ebp;

   
    send_eoi(PIT_IRQ_POS);

    if(active_terminal[0]==-1){
        execute_on_term((uint8_t*)"shell", 0);
    }
    
    if(get_process_total() < 6){
        // flag = 0;
        if(active_terminal[launch_terminal] == -1){
            map_current_video_page(launch_terminal);
            
            int availiable = get_availiable_pid();
            // if(availiable==-1){
            //     printf("No more available processes\n");
            //     pcb_t * current = (pcb_t*)(GET_PCB(current_pid_num));

            //     tss.ss0 = KERNEL_DS;
            //     tss.esp0 = EIGHT_MB - 4 - (EIGHT_KB * (current->pid -1));

            //     asm volatile(
            //         "movl   %0, %%ebp;"
            //         "movl   %1, %%esp;"
            //         : 
            //         :"r"(current->sched_ebp), "r"(current->sched_esp)
            //     );


            //     return;
            // }
        
            current_term_id = launch_terminal;

            shell_esp = EIGHT_MB - 4 - (EIGHT_KB * (availiable-1));
            shell_ebp = EIGHT_MB - 4 - (EIGHT_KB * (availiable-1));

            asm volatile(
                "movl   %0, %%ebp;"
                "movl   %1, %%esp;"
                : 
                :"r"(shell_ebp), "r"(shell_esp)
            );
            execute_on_term((uint8_t*)"shell", launch_terminal);
        }
    }
    // else{
    //     if(!flag){
    //         flag = 1;
    //         printf("process full\n");
    //     }
    // }

    pcb_t * current = (pcb_t*)(GET_PCB(current_pid_num));
    next_term = (current_term_id + 1) % MAX_TERM;
    while(active_terminal[next_term] == -1){
        next_term = (next_term + 1) % MAX_TERM;
    }
    // if(current_pid_num == active_terminal[next_term]){
    //     sti();
    //     return;
    // }

    pcb_t * next_proc = (pcb_t*)(GET_PCB(active_terminal[next_term]));
    map_program_page(next_proc->pid);
    map_current_video_page(next_term);
    map_video_page((PROG_128MB << 1) + (next_term * FOUR_KB), next_term);    // loading new video page

    // map_video_page(PROG_128MB << 1, next_proc->terminal_idx);flush_TLB();
    current_pid_num = next_proc->pid;
    current_term_id = (uint32_t)next_term;
    // set_video_mem(vram_addrs[next_term]);

    tss.ss0 = KERNEL_DS;
    tss.esp0 = EIGHT_MB - 4 - (EIGHT_KB * (next_proc->pid -1));
    

    // terminal = terms[current_term_id];
    asm volatile(
        "movl   %%ebp, %0;"
        "movl   %%esp, %1;"
        :"=r"(current->sched_ebp), "=r"(current->sched_esp)
    );
    asm volatile(
        "movl   %0, %%ebp;"
        "movl   %1, %%esp;"
        : 
        :"r"(next_proc->sched_ebp), "r"(next_proc->sched_esp)
    );
    // sti();
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
