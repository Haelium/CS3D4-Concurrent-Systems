// Dining Philosophers Promela Model.
// Based on code found here:
// http://dslab.konkuk.ac.kr/Class/2014/14ASE/Project/DP_Spin_%EB%B0%95%EC%84%B1%ED%9B%88_%EB%B0%95%EC%A7%84%EC%84%B1.pdf

#define NUMBER_OF_PHILOSOPHERS	4

// fork[n] is the fork to the left of philosopher n, fork[(n + 1) % m] is the fork to the right of philosopher n, where m is the number of philosophers
// Each fork can have m values, -1 (denoting the fork sitting on the table), and {0-m}, where m is the number of philosophers
// It is necessary for a fork's value to represent a distinct state depending on which philosopher is holding it to implement this model correctly
int fork[NUMBER_OF_PHILOSOPHERS];
int philosopher_eating[NUMBER_OF_PHILOSOPHERS];
int philosophers_eating;
int availiable_forks;

// Set up some verification constraints
ltl somebody_eats { eventually philosophers_eating > 0 }

// Check for deadlock (each philosopher holding one fork while nobody eats)
ltl nobody_eats { eventually philosophers_eating == 0 && availiable_forks == 0}

proctype Philosopher (byte id) {
THINK:
	if
	:: atomic {
		fork[id] == -1 -> 
			fork[id] = id

			availiable_forks--;
	};
	:: atomic { 
		fork[(id + 1) % NUMBER_OF_PHILOSOPHERS] == -1 -> 
			fork[(id + 1) % NUMBER_OF_PHILOSOPHERS] = id; 
			
			availiable_forks--;
	};
	fi;

	if
	:: atomic {
		// If holding fork in left hand and fork at right is availiable, pick up right fork and start eating
		fork[id] == id ; fork[(id + 1) % NUMBER_OF_PHILOSOPHERS] == -1 -> 
			fork[(id + 1) % NUMBER_OF_PHILOSOPHERS] = id;

			philosophers_eating++;
			philosopher_eating[id] = 1;
			availiable_forks--;
	}
	:: atomic {
		// If holding fork in right hand and fork at left is availiable, pick up left fork and start eating
		fork[id] == -1 ; fork[(id + 1) % NUMBER_OF_PHILOSOPHERS] == id -> 
			fork[id] = id;
		
			philosophers_eating++;
			philosopher_eating[id] = 1;
			availiable_forks--;
	}
	fi;

EAT:
	atomic {
		philosophers_eating--;
		philosopher_eating[id] = 0;
	}
	atomic {
		fork[(id + 1) % NUMBER_OF_PHILOSOPHERS] = -1;
		availiable_forks++;
	}
	atomic {
		fork[id] = -1;
		availiable_forks++;
	}

	goto THINK;
}

init {
	atomic {	// Philosophers must wait until everybody is there before eating, these are polite philosophers
		philosophers_eating = 0;
		availiable_forks = NUMBER_OF_PHILOSOPHERS;
		int i;
		// Start with all forks on the table (denoted -1)
		for (i : 0 .. NUMBER_OF_PHILOSOPHERS - 1) {
			fork[i] = -1
		}
		// Start each philosopher
		for (i : 0 .. NUMBER_OF_PHILOSOPHERS - 1) {
			run Philosopher(i)
		}
	}

}