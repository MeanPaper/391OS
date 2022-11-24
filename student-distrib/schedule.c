#include "schedule.h"
#include "sys_call_helper.h"
#define SQR_WAVE_MODE 0x36      // square wave mode, use by most of the OS
#define PIT_APPR_MAX  1193180   // this is approximately the highest requency for the pit
#define FREQ          40
#define MAX_TERM      3
// void schedule(){
    
// }

void pit_handler(){
    // return;    

    int32_t next_term;

   
    send_eoi(PIT_IRQ_POS);

    if(active_terminal[0]==-1){
        execute_on_term((uint8_t*)"shell", 0);
    }

    cli();
    pcb_t * current = (pcb_t*)(GET_PCB(current_pid_num));
    next_term = (current_term_id + 1) % MAX_TERM;
    while(active_terminal[next_term] == -1){
        next_term = (next_term + 1) % MAX_TERM;
    }
    if(current_pid_num == active_terminal[next_term]) return;

    pcb_t * next_proc = (pcb_t*)(GET_PCB(active_terminal[next_term]));
    map_program_page(next_proc->pid);
    // map_video_page(PROG_128MB << 1, next_proc->terminal_idx);
    flush_TLB();
    
    tss.ss0 = KERNEL_DS;
    tss.esp0 = EIGHT_MB - 4 - (EIGHT_KB * (next_proc->pid -1));
    
    current_pid_num = next_proc->pid;
    current_term_id = (uint32_t)(next_proc->terminal_idx);
    // terminal = terms[current_term_id];
    sti();

    asm volatile(
        "movl   %%ebp, %0;"
        "movl   %%esp, %1;"
        :"=r"(current->save_ebp), "=r"(current->save_esp)
    );
    asm volatile(
        "movl   %0, %%ebp;"
        "movl   %1, %%esp;"
        : 
        :"r"(next_proc->save_ebp), "r"(next_proc->save_esp)
    );

    return;
}

void pit_init(){
    cli();
    uint32_t divisor = PIT_APPR_MAX / FREQ;
    outb(SQR_WAVE_MODE, PIT_CMD_PORT);
    outb( divisor & 0xFF, PIT_DATA_0); // sending low bytes
    outb( divisor >> 8, PIT_DATA_0); // sending high bytes
    enable_irq(PIT_IRQ_POS);
    sti();
}
