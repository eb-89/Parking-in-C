#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define DAY_LENGTH 20//12*12
#define CAR_STAY_TIME 3

int poisson(int lambda);
void start_input(int *street_length_units);

typedef struct {
	int is_occupied;
	int arrival_time;
	int departure_time;
} Spot;


int poisson(int lambda)
{
	double p = 1;
	double L = exp(-lambda);
	int k = 0;

	do {
		k++;
		double u = ((double) rand() / (double) (RAND_MAX));
		p = p*u;
		//printf(" %f \n", u);
	} while (p>L);

	return k-1;

}



void start_input(int *street_length_units)
{
	printf("How long is the steet in parking units? \n");
	scanf("%d", street_length_units);
}




int main(void) {


	int street_length;
	int rate_of_arrival;
	int rate_of_departure;

	//initialization
	srand((unsigned) time(NULL));
	start_input(&street_length);

	Spot *street = malloc(sizeof(Spot) * street_length);

	int i;
	for (i=0; i<street_length; i++) {
		street[i].is_occupied = 0;
	}


	//main iteration loop:	
	for (i = 0; i < DAY_LENGTH; i++)	{

		int j;
		int cars_coming = poisson(3);

		printf("Iteration: %d: \n", i);
		printf("  Cars coming for this iteration: %d \n", cars_coming);


		for (j = 0; j<cars_coming;j++) {
			
			printf("    Car: %d \n", j);

			int empty_slots = 0;
			int empty_slot_index = 0;

			int k;
			for (k = 0; k<street_length; k++) {
				if (!street[k].is_occupied) {
					empty_slots++;
				}
			}

			printf("     Empty slots to choose from: %d\n", empty_slots);


			if (empty_slots > 0) {

				empty_slot_index = (rand() % empty_slots) + 1;
				printf("     Putting in %dth empty slot \n", empty_slot_index);

				empty_slots = 0;
				for (k = 0; k<street_length; k++) {
					if (!street[k].is_occupied) {
						empty_slots++;
					
						if (empty_slots == empty_slot_index) {
							street[k].is_occupied = 1;
							street[k].arrival_time = i;
							street[k].departure_time = i + CAR_STAY_TIME;
							printf("     Filled slot at index: %d\n", k);
						}

					}

				}

			} else {
				printf("     No empty spots left!\n");
			}


		}

		printf("  Cars leaving for this iteration: \n");
		for(j=0; j<street_length; j++) {
			if (street[j].is_occupied == 1 && street[j].departure_time < i) {
				street[j].is_occupied = 0;
				street[j].arrival_time = 0;
				street[j].departure_time = 0;
				printf("    Car at spot %d left \n", j);
			} 
		}



		
		printf("The street at the end of iteration %d: ", i);
		for(j=0; j<street_length; j++) {
			printf("%d", street[j].is_occupied);
		}

		printf(" \n\n");

	}


	

	free(street);
	
	getch();
	return 0;

}
