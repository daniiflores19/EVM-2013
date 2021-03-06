#include "states.h"
#include "function_manager.h"
#include "UtsavAPI.h"

/******* Private functions defns ***************/
void find_next_node();
/***********************************************/


MIDDLE_LEVEL_SIGNAL mid_sig = NO_SIGNAL_MIDDLE;
MIDDLE_LEVEL_STATE mid_state = IDLE;
int node_to_send = 2, slave_pos = 0;
extern char slavesList[MAX_SLAVES];

void send_middle_level_signal (MIDDLE_LEVEL_SIGNAL sig) {
	mid_sig = sig;
};	
	

void middle_level_take_action () {
	int val;
	
	switch(mid_state) {
		case IDLE:
		printf("idle middle\n\r");
		switch(mid_sig) {
			case FIND_NODE_SIGNAL:
				mid_state = FIND_NEXT_NODE;
		    mid_sig = NO_SIGNAL_MIDDLE;
				break;
			
			default:
				break;
		}
		break;
			
		case FIND_NEXT_NODE:
		printf("Find next node\n\r");
		find_next_node();
		send_low_level_signal(SEND_TASK_LOW);
		mid_state = SEND_TASK_MID;
		break;


		case SEND_TASK_MID:
		printf("Send task mid\n\r");
		switch(mid_sig) {
			case SENDING_DONE:
			mid_sig = NO_SIGNAL_MIDDLE;
			val = find_unassigned_tasks();
			if(val == 0) {
				send_top_level_signal(ASSGN_DONE);
				mid_state = IDLE;
			}
			
			else {
				mid_state = FIND_NEXT_NODE;
			}
			break;
			
			default:
			break;
		}
		break;
	}
}



void find_next_node() {
	int counter = 0;
	while(1) {
		if(slavesList[slave_pos] != 0) {
			node_to_send = slave_pos + 2;   //0 is not present, 1 is the master
			return;  //found the node to transmit to.
		}
		
		slave_pos++;
		if(slave_pos > (MAX_SLAVES - 1))
			slave_pos = 0;
		
		counter++;
		
		if(counter == 10) {
			printf("No slaves present\n\r");
			counter = 0;
		}
	}
}
		
	