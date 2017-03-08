/* Concurrent Counting Algorithm */

/* Critical path handled using Peterson-Fischer Mutual Exclusion */

#define	true	1
#define false	0
#define	turn_p	1
#define	turn_q	0

int n = 0;
byte mutex_n = 0;

bool flag_p, flag_q, turn;

bool p_complete = false;
bool q_complete = false;

proctype p() {
	int temp;
	int i;
	for (i : 1 .. 2) {
		flag_p = true;	// Proc p wants to enter critical path
		turn = turn_q;
		(flag_q == false || turn == turn_p) ->
		// Start of critical path
		temp = n;
		temp = temp + 1;
		n = temp;
		// End of critical path
		
		flag_p = false;	// Proc p wants to enter critical path
	}
	p_complete = true;
}

proctype q() {
	int i;
	for (i : 1 .. 2) {
		flag_q = true;	// Proc q wants to enter critical path
		turn = turn_p;
		(flag_p == false || turn == turn_q) ->
		// Start of critical path
		n = n + 1;
		// End of critical path

		flag_q = false;	// Proc q no longer wants to enter critical path
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

