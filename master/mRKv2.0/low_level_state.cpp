#include "states.h"
#include "function_manager.h"
#include "UtsavAPI.h"
#include "transmitProtocol.h"

#define DATA_LOCATION(ADD) ((ADD-1)*512)

LOW_LEVEL_STATE low_state = IDLE_LOW;
LOW_LEVEL_SIGNAL low_sig = NO_SIGNAL_LOW;
LOW_LEVEL_STATE next_state;

extern int node_to_send;   //the node to send the data to
extern uint8_t data[512*MAX_MOLES];      //sumukh's array to write the tx packet to.
extern struct task_function_info task_function_table[MAX_NUM_TASKS_IN_NETWORK];
extern uint16_t func_reply;

/************* private function definitions *************/
void send_task_params();
void send_function_names();
void send_task_function_code();
void send_function_code();
void send_init_state();
void send_activate();
void assign_node_task();
int any_functions_left();
int try_to_send();
/*******************************************************/


/************ private variables ***********************/
int num_functions_sent = 0;
int num_bytes = 0;
/******************************************************/

void send_low_level_signal (LOW_LEVEL_SIGNAL sig) {
    low_sig = sig;
}


void low_level_take_action () {

    switch(low_state) {
    case IDLE:
        printf("idle low level\n\r");
        switch(low_sig) {
        case SEND_TASK_LOW:
            send_task_params();
            if( try_to_send() )
                low_state = TASK_PARAMS;
            else {
                next_state = TASK_PARAMS;
                low_state = BLOCK_SEND;
            }
            low_sig = NO_SIGNAL_LOW;
            break;

        default:
            break;
        }
        break;



    case TASK_PARAMS:
        printf("Task params\n\r");
        switch(low_sig) {
        case ACK:
            send_function_names();
            if( try_to_send() )
                low_state = FUNC_NAMES;
            else {
                next_state = FUNC_NAMES;
                low_state = BLOCK_SEND;
            }

            low_sig = NO_SIGNAL_LOW;
            break;

        case NACK:
            //node did not accept... find different node.
						printf("received NACK for task params\n\r");
            send_middle_level_signal(SENDING_DONE);
            low_state = IDLE_LOW;
            low_sig = NO_SIGNAL_LOW;
            break;
        }
        break;





    case FUNC_NAMES:
        printf("Func names\n\r");
        if(low_sig == ACK) {
            num_functions_sent = 0;
					
						if(func_reply & 0x01) {
							send_task_function_code();
							if( try_to_send() )
									low_state = FUNC_CODE;
							else {
									next_state = FUNC_CODE;
									low_state = BLOCK_SEND;
							}

							low_sig = NO_SIGNAL_LOW;
						}
						else {
							low_state = FUNC_CODE;
							low_sig = NO_SIGNAL_LOW;
							num_functions_sent = 0;
						}
						
						func_reply = func_reply >> 1;
						
        }

        break;




    case FUNC_CODE:
        printf("Func code\n\r");
        if(low_sig == ACK) {
            //if all functions code sent... then
            //send init state.
            if(!any_functions_left()) {
                //send initial state.... to be done
                low_state = INIT_STATE;
                low_sig = NO_SIGNAL_LOW;
							  send_init_state();
							  if( try_to_send() )
									low_state = INIT_STATE;
								else {
									next_state = INIT_STATE;
									low_state = BLOCK_SEND;
								}
            }
            //if some functions code still left.
            //send next function.
            else {
                send_function_code();
                if( try_to_send() )
                    low_state = FUNC_CODE;
                else {
                    next_state = FUNC_CODE;
                    low_state = BLOCK_SEND;
                }

                low_sig = NO_SIGNAL_LOW;
            }
        }
        break;

    case INIT_STATE:
        printf("init state\n\r");
        if(low_sig == ACK) {
            //send deactivate signal to the current node with the task.
            //low_state = DEACTIVATE;
					  low_state = ACTIVATE;
            low_sig = NO_SIGNAL_LOW;
					  send_activate();
					  if( try_to_send() )
							low_state = ACTIVATE;
						else {
							next_state = ACTIVATE;
							low_state = BLOCK_SEND;
						}
        }
        break;




    case DEACTIVATE:
        printf("Deactivate\n\r");
        if(low_sig == ACK) {
            //send activate signal to the node.
            low_state = ACTIVATE;
            low_sig = NO_SIGNAL_LOW;
        }
        break;



    case ACTIVATE:
        printf("Activate\n\r");
        if(low_sig == ACK) {
            //done transferring the current task.
					  assign_node_task();
            send_middle_level_signal(SENDING_DONE);
            low_state = IDLE_LOW;
            low_sig = NO_SIGNAL_LOW;
        }
        break;

    case BLOCK_SEND:
        printf("Block send\n\r");
        if( try_to_send() ) {
            printf("sending succeeded\n\r");
            low_state = next_state;
        }

        else {
            printf("Not ready to send\n\r");
        }
        break;



    }
}






void send_task_params() {
    int task_num, ret_val;
    struct task_function_info *task_ptr;

    task_num = find_first_unassigned_task();

    task_ptr = &(task_function_table[task_num]);
		printf("dest node %d\n\r", node_to_send);
    printf("task name %s\n\r", task_ptr->name);
    printf("task parameters %d %d %d %d \n\r", task_ptr->periods, task_ptr->periodms, task_ptr->wcets, task_ptr->wcetms);
    set_MessageTypes(&data[DATA_LOCATION(node_to_send)], TYPE_TASK_PARAMS);

    set_TaskParams(&data[DATA_LOCATION(node_to_send)],
                   task_ptr->periods,
                   task_ptr->periodms,
                   task_ptr->wcets,
                   task_ptr->wcetms);
    num_bytes = 25;

}





void send_function_names() {
    int task_num, ret_val, i, length;
    struct task_function_info *task_ptr;
    task_num = find_first_unassigned_task();
    task_ptr = &task_function_table[task_num];

    set_MessageTypes(&data[DATA_LOCATION(node_to_send)], TYPE_FUNC_NAMES);
    set_number_of_functions(&data[DATA_LOCATION(node_to_send)], task_ptr->num_references + 1);  //one is the task code + other fun references
	  set_functionNames(&data[DATA_LOCATION(node_to_send)], task_ptr->name);
    for(i = 0; i < task_ptr->num_references; i++) {
        set_functionNames(&data[DATA_LOCATION(node_to_send)], task_ptr->fun_references[i]);
    }



    num_bytes = 2 + MAX_NAME_LENGTH * (task_ptr->num_references + 1);

}




void send_task_function_code() {
    int task_num, ret_val, i;
    struct task_function_info *task_ptr;
    const char *fun_name;
    char *fn_ptr;
	  uint16_t length;

	
    task_num = find_first_unassigned_task();
    task_ptr = &task_function_table[task_num];

    fun_name = task_ptr->name;
    printf("in sending code, name is %s\n\r", fun_name);
    length = 256;         //To be done... put correct function size.
    fn_ptr = (char *) get_function_handle(fun_name, MAX_NAME_LENGTH - 1);
    fn_ptr = (char *)((uint32_t)fn_ptr & ~(1));



    set_MessageTypes(&data[DATA_LOCATION(node_to_send)], TYPE_FUNC_CODE);
			
    set_FuncCode(&data[DATA_LOCATION(node_to_send)], fun_name, length, fn_ptr);
		
		
    num_bytes = 3 + MAX_NAME_LENGTH + 256;

		printf("sending %s funciton code\n\r", fun_name);

}





void send_function_code() {
    int task_num, length;
    struct task_function_info *task_ptr;
    const char *fun_name;
    char *fn_ptr;
    task_num = find_first_unassigned_task();
    task_ptr = &task_function_table[task_num];

    fun_name = task_ptr->fun_references[num_functions_sent];
    length = 256;         //To be done... put correct function size.
    fn_ptr = (char *) get_function_handle(fun_name, MAX_NAME_LENGTH - 1);
    fn_ptr = (char *)((uint32_t)fn_ptr & ~(1));

    set_MessageTypes(&data[DATA_LOCATION(node_to_send)], TYPE_FUNC_CODE);
    set_FuncCode(&data[DATA_LOCATION(node_to_send)], fun_name, length, fn_ptr);

    num_bytes = 3 + MAX_NAME_LENGTH + 256;


    num_functions_sent++;
	
		printf("sending %s function code\n\r", fun_name);
}


void send_init_state() {
    int task_num;
    struct task_function_info *task_ptr;

    task_num = find_first_unassigned_task();
    task_ptr = &task_function_table[task_num];


    set_MessageTypes(&data[DATA_LOCATION(node_to_send)], TYPE_INIT_STATES);
    set_States(&data[DATA_LOCATION(node_to_send)], task_ptr->states);
    
	  num_bytes = 2 + sizeof(uint32_t) * 10;

		printf("sending init states\n\r");
}		


void send_activate() {
	int task_num;
	struct task_function_info *task_ptr;

	task_num = find_first_unassigned_task();
	task_ptr = &task_function_table[task_num];


	set_MessageTypes(&data[DATA_LOCATION(node_to_send)], TASK_ACTIVATE);
	set_activate(&data[DATA_LOCATION(node_to_send)], task_ptr->name);

	num_bytes = 1 + strlen(task_ptr->name) + 2;

	printf("sending activate signal\n\r");
}

void assign_node_task() {
	int task_num;
	struct task_function_info *task_ptr;

	task_num = find_first_unassigned_task();
	task_ptr = &task_function_table[task_num];
	
	task_ptr->assigned_node_id = node_to_send;
}


int any_functions_left() {
    int task_num;
    struct task_function_info *task_ptr;

    task_num = find_first_unassigned_task();
    task_ptr = &task_function_table[task_num];
	
    while(((func_reply & 0x01) == 0) && (num_functions_sent <= task_ptr->num_references)) {
			func_reply = func_reply >> 1;
			num_functions_sent++;
		}

    if(num_functions_sent >= task_ptr->num_references) {
        printf("no more functions left\n\r");
        return 0;
    }

    else {
        printf("Functions left is %d\n\r", task_ptr->num_references - num_functions_sent);
        return 1;
    }
}




int try_to_send() {
    if(!ReadyToSendData(node_to_send)) {
        printf("Not ready to send\n\r");
        return 0;
    }

    else {
        startDataTransmission(node_to_send, num_bytes);
        return 1;
    }
}