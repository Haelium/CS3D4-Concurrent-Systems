/* Concurrent Counting Algorithm */

int n = 0;

proctype p() {
	int temp;
	int i;
	for (i : 1 .. 2) {
		temp = n;
		temp = temp + 1;
		n = temp;
	}
}

proctype q() {
	int i;
	for (i : 1 .. 2) {
		n = n + 1;
	}
}

init {
	run p();
	run q();
}

