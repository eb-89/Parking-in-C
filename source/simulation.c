#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>


void init_input(int *street_length_units, int *iterations)
{
	printf("How long is the steet in parking units? \n");
	scanf("%d", street_length_units);
	printf("How many units of time? \n");
	scanf("%d", iterations);
}


int main(void) {


	int street_length, iterations;
	int rate_of_arrival;
	int rate_of_departure;

	srand((unsigned) time(NULL));

	init_input(&street_length, &iterations);
	
	int *street_ptr = malloc(sizeof(int) * street_length);

	int i;
	for (i=0; i<street_length; i++)
		street_ptr[i] = 0;

	
	for (i = 0; i<iterations;i++)	{

		
		if (i<4) {
			rate_of_arrival += 1;

		} else if (i> iterations - 4) {
			rate_of_arrival -= 1;

		}

		if (i>4) {
			rate_of_departure += 1;

		} else if (i>= iterations - 4) {
			rate_of_departure -= 1;

		}

		int j;
		for(j=0; j<rate_of_arrival; j++) {

			int random = (rand() % street_length);

			if (street_ptr[random] == 0) 
				street_ptr[random] = 1;
			
		}

		for(j=0; j<rate_of_departure; j++) {

			int random = (rand() % street_length);

			if (street_ptr[random] == 1) 
				street_ptr[random] = 0;
			
		}


	}

	printf("The street looks like this: \n");
	for(i =0; i<street_length; i++) {
		printf("%d", street_ptr[i]);
	}

	free(street_ptr);
	
	getch();
	return 0;

}