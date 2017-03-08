/* Concurrent Counting Algorithm */

int n = 0;
int n_mutex = 0;

bool p_complete = false;
bool q_complete = false;

proctype p() {
	int temp;
	int i;
	for (i : 1 .. 2) {
		/* Using atomic here makes the use of temp redundant */
		atomic {
			temp = n;
			temp = temp + 1;
			n = temp;
		}
	}
	p_complete = true;
}

proctype q() {
	int i;
	for (i : 1 .. 2) {
		atomic {
			n = n + 1;
		}
	}
	q_complete = true;
}

init {
	run p();
	run q();

	(p_complete == true && q_complete == true) ->
			assert(n == 4);

	/*(p_complete == true && q_complete == true) ->
			assert(n == 4);*/
}

