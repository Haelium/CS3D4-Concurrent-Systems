/* Concurrent Counting Algorithm */

int n = 0;

bool p_complete = false;
bool q_complete = false;

proctype p() {
	int temp;
	int i;
	for (i : 1 .. 2) {
		temp = n;
		temp = temp + 1;
		n = temp;
	}
	p_complete = true;
}

proctype q() {
	int i;
	for (i : 1 .. 2) {
		n = n + 1;
	}
	q_complete = true;
}

init {
	run p();
	run q();

	(p_complete == true && q_complete == true) ->
			assert(n != 3);

	/*(p_complete == true && q_complete == true) ->
			assert(n == 4);*/
}

