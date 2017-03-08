#define NUMBER_OF_PHILOSOPHERS	6

// fork[n] is the fork to the left of philosopher n, fork[(n + 1) % m] is the fork to the right of philosopher n, where m is the number of philosophers
byte fork[NUMBER_OF_PHILOSOPHERS];

proctype Philosopher (byte id) {
THINK:
	// Some delay
EAT:
	atomic {	// Pick up left fork
	}
	atomic {	// Pick up right fork
	}
	// Some delay
	atomic {	// Drop left fork
	}
	atomic {	// Drop right fork
	}
}

init {
	atomic {	// Philosophers must wait until everybody is there before eating
		for (i : 0 .. 6) {
			run Philosopher(i)
		}
	}
}
