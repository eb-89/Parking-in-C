#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//#include "linkedlist.h"

#define VERBOSE 0
#define DAY_LENGTH 60*12
#define CAR_STAY_TIME_MU 10
#define CAR_STAY_TIME_SIGMA 0.50
#define NO_OF_CARS_POISSON 30
#define STREET_LENGTH 500.0

int poisson(int lambda);
void start_input(int *street_length_units);
double normal(double mu, double sigma);


typedef struct {
	int car_nr;
	double x;
	double y;
	int depart_time;
	int strategy;
	double length;
} Car;




double normal(double mu, double sigma)
//sigma here is standard deviation, s^2 would be the variance.
{
	double u1 = ((double) rand() / (double) (RAND_MAX));
	double u2 = ((double) rand() / (double) (RAND_MAX));

	double z = sqrt(-2*log(u1))*cos(2*M_PI*u2);
	return sigma*z + mu;
}


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

	FILE *fp;
	fp = fopen("data.out", "wb");
	fprintf(fp, "Iteration:  NumOfCars:  CarsThatDontFit:  FreeSpace:  PercentFreeSpace:\n" );

	//int street_length = 5;
	//int rate_of_arrival;
	//int rate_of_departure;

	//initialization
	srand((unsigned) time(NULL));
	//start_input(&street_length);


	//head of the linked list containing cars.
	//struct Node *head = NULL; 



	//reformulate in terms of a pointer to car.
	//Spot *street = malloc(sizeof(Spot) * street_length);

	int n_cars = 0;
	int used_space = 1;
	int car_index = 0;
	Car * cars_array = malloc(sizeof(Car) * used_space);
	int i; //loop variable

	/*initalization, u can use calloc.
	
	for (i=0; i<street_length; i++) {
		street[i].is_occupied = 0;
	}
	*/

	//main iteration loop:	
	for (i = 0; i < DAY_LENGTH; i++)	{

		int j;
		int cars_coming = poisson(NO_OF_CARS_POISSON);
		int didnt_fit_counter = 0;
		double total_free; //free space (written at the END of each iteration)
		double total_free_percent;

		if (n_cars + cars_coming >= used_space) {
			used_space = n_cars + cars_coming;
			cars_array = realloc(cars_array, sizeof(Car) * used_space);
			if (cars_array == NULL) {
				printf("realloc Error\n");
			}
		}

		if (VERBOSE) {
			printf("---------Iteration: %d: \n", i);
			printf("  Cars coming for this iteration: %d \n", cars_coming);
		}
		//here calculate empty spaces

		for (j = 0; j<cars_coming;j++) {

			Car new_car;
			new_car.car_nr = car_index;
			new_car.length = 5;
			new_car.depart_time = i + normal(CAR_STAY_TIME_MU,CAR_STAY_TIME_SIGMA);

			car_index++;
			
			//new_car.x = 50.0;

			if (VERBOSE) {
				printf("\n     Car: %d, staying 'til iteration %d  \n", new_car.car_nr, new_car.depart_time);
			}

			double (*distances)[2] = malloc(sizeof(double)*2*(n_cars +1));
			if (distances == NULL) {
				printf("Malloc Failed\n");
			}
			distances[0][0] = 0;
			distances[n_cars][1] = STREET_LENGTH;

			int k = 0; //variable for loops

			for (k = 0; k<n_cars; k++) {
				distances[k][1] = cars_array[k].x;
				distances[k+1][0] = cars_array[k].x + cars_array[k].length;

			}

			//some prints
			if (VERBOSE) {
				printf("\n    Available empty slots for car %d: \n", new_car.car_nr);
			
				for (k = 0; k<n_cars+1; k++) {
					printf("    %f %f \n" , distances[k][0], distances[k][1]);

				}
			}

			//possible distances stores INDECES.
			//The indeces are potential slots for inserting a new car.
			int *possible_distances = malloc(sizeof(int)*(n_cars+1));

			int m = 0;
			for (k = 0; k<n_cars+1; k++) {
				if ((distances[k][1] - distances[k][0]) >= new_car.length) {
					
					possible_distances[m] = k;
					m++;
				}
				//distances[k+1][0] = cars_array[k].x + cars_array[k].length;
			}

			if (m != 0) {
				int random_spot = rand() % m;
				int index = possible_distances[random_spot];

				

				for (k = n_cars; k != index; k = k-1) {
					
					//printf("moving slots, carrs[%d] is assigned carrs[%d] \n", k,k-1);
					cars_array[k] = cars_array[k-1];
					//printf("value of k:%d \n", k);
				}

				new_car.x = (distances[index][1] - distances[index][0] - new_car.length)*((double) rand() / (double) (RAND_MAX)) + distances[index][0];
				cars_array[index] = new_car;
				
				if (VERBOSE) {
					printf("    Putting car %d at index %d, with x-coord %f \n", new_car.car_nr, index, new_car.x);
				}

				n_cars++;
				//printf("Ive Incresed cars\n");
			} else {
				didnt_fit_counter++;
				if (VERBOSE) {
					printf("    CANNOT FIND SPACE\n");
				}
			}
			

			if (j == cars_coming -1) {
				total_free = 0;
				total_free_percent = 0;
				for (k = 0; k<n_cars+1; k++) {
					total_free += distances[k][1] - distances[k][0];
				}
				total_free_percent = (total_free / STREET_LENGTH) * 100;		
			}


			free(possible_distances);
			//printf("Ive freed pinter\n");
			free(distances);
			//free(distances);
						
			

		}
			//END OF for each car loop
		if (VERBOSE) {
			printf("\n    -- Departures --- \n");
		}

		j = 0;
		while (j < n_cars) {
				if (i > cars_array[j].depart_time) {
					if (VERBOSE) {
						printf("    Car %d departing, parked at x-coord %f\n", cars_array[j].car_nr, cars_array[j].x);
					}

					int l;
					for (l = j; l < n_cars; l++) {
						cars_array[l] = cars_array[l+1];
					}

					n_cars--;
				} else {
					j++;
				}
		}
		if (VERBOSE) {
			printf(" \n\n");
		}

		//fprintf(fp, "iteration %d" ,i);
		//fputc(fp, "iteration %d" ,i);
		//fputs(fp, "iteration %d" ,i);
		fprintf(fp, "  %d  %d  %d  %f  %f \n", i, n_cars, didnt_fit_counter, total_free, total_free_percent);

		//END OF SIMULATION LOOP

	}

	if (VERBOSE) {
		printf("Number of cars at in the street after complete simulation: %d\n", n_cars );
	}
	//printf("The street at the end of iteration in array %d: ", i);
	

	

	//free(street);
	free(cars_array);
	
	fclose(fp);
	//getch();
	return 0;
	//END OF MAIN

}
