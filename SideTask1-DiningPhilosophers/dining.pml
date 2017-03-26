#define NUMBER_OF_PHILOSOPHERS	6

// fork[n] is the fork to the left of philosopher n, fork[(n + 1) % m] is the fork to the right of philosopher n, where m is the number of philosophers
byte fork[NUMBER_OF_PHILOSOPHERS];
int food;

proctype Philosopher (byte id) {
THINK:
	// Some delay
	atomic {	// Pick up left fork
		(fork[id] == 1) -> fork[id] = 0
	}
	atomic {	// Pick up right fork
		(fork[(id + 1) % NUMBER_OF_PHILOSOPHERS] == 1) -> fork[id] = 0
	}
	// Some delay
	atomic {	// Drop left fork
		(fork[id] == 0) -> fork[id] = id + 1
	}
	atomic {	// Drop right fork
		(fork[(id + 1) % NUMBER_OF_PHILOSOPHERS] == 0) -> fork[id] = id + 1
	}
EAT:
	food--;
	goto THINK
}

init {
	atomic {	// Philosophers must wait until everybody is there before eating, these are polite philosophers
		food = 10000
		int i
		for (i : 0 .. 5) {
			fork[i] = 1
		}
		for (i : 0 .. 5) {
			run Philosopher(i)
		}
	}
}