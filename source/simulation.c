#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//#include <random.h>
//#include "linkedlist.h"


#define VERBOSE 0

#define DAY_LENGTH 60*12
#define CAR_STAY_TIME_MU 10
#define CAR_STAY_TIME_SIGMA 3
#define NO_OF_CARS_POISSON 3
#define SAFE_DISTANCE 0.5

//defaults
#define STREET_LENGTH 200.0

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

/*

1. random
2. first spot avaibale
3. take the middle spot
4. smallest 

5. must leave som space. 
concrete suggestiio: change l, x, , lamda, d security dist

*/



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




int main(int argc, char *argv[]) {


	FILE *fp;
	fp = fopen("data.out", "wb");
	fprintf(fp, "Iteration:  NumOfCars:  CarsThatDontFit:  FreeSpace:  PercentFreeSpace:\n" );

	FILE *fp_aggr;
	fp_aggr = fopen("data_aggr.out","a");

	double street_length; //Streetlength
	int sim_num; //simulation number
	switch (argc) {
		case 2:
			sscanf(argv[1], "%lf", &street_length);
			break;
		case 3: 
			sscanf(argv[1], "%lf", &street_length);
			sscanf(argv[2], "%d", &sim_num);
			break;
		default:
			street_length = STREET_LENGTH;
			sim_num = 0;
			break;
	}

	//int street_length = 5;
	//int rate_of_arrival;
	//int rate_of_departure;

	//initialization
		srand((unsigned) time(NULL) + sim_num);
	//start_input(&street_length);


	//head of the linked list containing cars.
	//struct Node *head = NULL; 



	//reformulate in terms of a pointer to car.
	//Spot *street = malloc(sizeof(Spot) * street_length);

	int n_cars = 0;
	int used_space = 1;
	int car_index = 0;
	Car * cars_array = malloc(sizeof(Car) * used_space);

	double min_free_space = street_length; // minimum free space
	int cars_at_min = 0;


	int i; //loop variable

	/*initalization, u can use calloc.
	
	for (i=0; i<street_length; i++) {
		street[i].is_occupied = 0;
	}
	*/

	

	//main iteration loop:	
	for (i = 0; i < DAY_LENGTH; i++)	{

		double total_free; //free space (written at the END of each iteration)
		double total_free_percent;
		int j;
		int cars_coming = poisson(NO_OF_CARS_POISSON);
		int didnt_fit_counter = 0;
		

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

			if ((rand() % 101) < 200) {
				new_car.strategy = 0;
			} else {
				new_car.strategy = 1;
			}
			

			car_index++;
			
			//new_car.x = 50.0;

			if (VERBOSE) {
				printf("\n     Car: %d:   \n", new_car.car_nr);
				printf("      length: %f  \n", new_car.length);
				printf("      strategy: %d  \n", new_car.strategy);
				printf("      staying 'til iteration: %d", new_car.depart_time);
			}

			double (*distances)[2] = malloc(sizeof(double)*2*(n_cars +1));
			if (distances == NULL) {
				printf("Malloc Failed\n");
			}
			distances[0][0] = 0;
			distances[n_cars][1] = street_length; //STREET_LENGTH;

			int k = 0; //variable for loops

			for (k = 0; k<n_cars; k++) {
				distances[k][1] = cars_array[k].x;
				distances[k+1][0] = cars_array[k].x + cars_array[k].length;

			}

			//some prints
			if (VERBOSE) {
				printf("\n    Available empty slots for car %d: \n", new_car.car_nr);
			
				for (k = 0; k<n_cars+1; k++) {
					printf("    %f %f (distance of %f) \n" , distances[k][0], distances[k][1], distances[k][1] - distances[k][0]);

				}

				//printf("\n    Available slots after SAFE_DISTANCE for car %d: \n", new_car.car_nr);

				//for (k = 0; k<n_cars+1; k++) {
				//	printf("    %f %f \n" , distances[k][0] + SAFE_DISTANCE, distances[k][1] - SAFE_DISTANCE);

				//}

			}

			//possible distances stores INDECES.
			//The indeces are potential slots for inserting a new car.
			int *possible_distances = malloc(sizeof(int)*(n_cars+1));

			int m = 0;
			for (k = 0; k<n_cars+1; k++) {

				if (n_cars == 0) {
					if ((distances[k][1] - distances[k][0]) >= new_car.length) {
					
					possible_distances[m] = k;
					m++;

					}

				} else if (k == 0 || k == n_cars) {
					if ((distances[k][1] - distances[k][0]) >= new_car.length + SAFE_DISTANCE) {
					
					possible_distances[m] = k;
					m++;

					}

				} else {


					if ((distances[k][1] - distances[k][0]) >= new_car.length + 2*SAFE_DISTANCE) {
					
					possible_distances[m] = k;
					m++;

					}
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

				
				double allowed_random_x;
				switch (new_car.strategy)  {
					case 0: //random avaibale, always nice
						if (distances[index][0] == 0) {
							new_car.x = distances[index][0];
						} else {
							new_car.x = distances[index][0]  + SAFE_DISTANCE;
						}
						break;
					case 1: //random x in random avaibale
						if (n_cars == 0 ) {
							allowed_random_x = (distances[index][1] - distances[index][0] - new_car.length)*((double) rand() / (double) (RAND_MAX));
							new_car.x = allowed_random_x + distances[index][0];
							//printf("Putting car in only spot\n");
						} else if (distances[index][1] == street_length /*STREET_LENGTH*/ ) {
							allowed_random_x = (distances[index][1] - distances[index][0] - new_car.length - SAFE_DISTANCE)*((double) rand() / (double) (RAND_MAX));
							new_car.x = allowed_random_x + distances[index][0] + SAFE_DISTANCE;
							//printf("Putting car in last spot\n");
						} else if (distances[index][0] == 0) {
							allowed_random_x = (distances[index][1] - distances[index][0] - new_car.length - SAFE_DISTANCE)*((double) rand() / (double) (RAND_MAX));
							new_car.x = allowed_random_x + distances[index][0];
							//printf("Putting car in first spot\n");
						} else {
							allowed_random_x = (distances[index][1] - distances[index][0] - new_car.length - 2*SAFE_DISTANCE)*((double) rand() / (double) (RAND_MAX));
							new_car.x = allowed_random_x + distances[index][0]  + SAFE_DISTANCE;
						}
						break;
					case 2: //always in middle, if you are on the edges, then shift to not break 2m rule
						new_car.x = ((distances[index][1] - distances[index][0]) - new_car.length) / 2 + distances[index][0];
						if (n_cars > 0 && distances[index][0] == 0) {
							if (cars_array[index+1].x - (new_car.x + new_car.length)  < SAFE_DISTANCE) {
								//printf("trying first, too close, dist= %f\n", cars_array[index+1].x - (new_car.x + new_car.length));
								
								//printf("moved the car by %f\n",(SAFE_DISTANCE - (cars_array[index+1].x - (new_car.x + new_car.length))) );
								new_car.x = new_car.x - (SAFE_DISTANCE - (cars_array[index+1].x - (new_car.x + new_car.length)));
							}
						} else if (n_cars > 0 && distances[index][1] == street_length /*STREET_LENGTH*/ ) {
							if (new_car.x - (cars_array[index-1].x + cars_array[index-1].length) < SAFE_DISTANCE) {
								//printf("trying last too close, dist= %f\n", new_car.x - (cars_array[index-1].x + cars_array[index-1].length) );
								
								//printf("moved the car by %f\n",(SAFE_DISTANCE - (new_car.x - (cars_array[index-1].x + cars_array[index-1].length)) ) ) ;
								new_car.x = new_car.x + (SAFE_DISTANCE - (new_car.x - (cars_array[index-1].x + cars_array[index-1].length)) );
							}
						}
							
						
						break;

				}

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
			
			/*
			if (j == cars_coming - 1) {
				total_free = 0;
				total_free_percent = 0;
				for (k = 0; k<n_cars+1; k++) {
					total_free += distances[k][1] - distances[k][0];
				}
				total_free_percent = (total_free / STREET_LENGTH) * 100;		
			}
			*/

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
		

		
		//if (i==0) { break;}
		

		total_free = street_length;
		total_free_percent = 0;
		
		for (j=0; j<n_cars; j++) {
			/*
			if (j == 0) {
				total_free += cars_array[j].x;
			} else {
				total_free += cars_array[j].x - (cars_array[j-1].x + cars_array[j-1].length);
			}
			if (j == n_cars-1) {
				total_free += street_length - (cars_array[j].x + cars_array[j].length);
			}
			*/
			total_free = total_free - (cars_array[j].length);
		}
		total_free_percent = (total_free / street_length)*100;

		if (VERBOSE) {
			printf("Writing: \nIteration:  %d, \nn_cars:  %d, \nDidn't Fit:  %d, \nTotal Free:  %f, \nTotal Free Percent:  %f \n", i, n_cars, didnt_fit_counter, total_free, total_free_percent);
		}
		fprintf(fp, "  %d  %d  %d  %f  %f \n", i, n_cars, didnt_fit_counter, total_free, total_free_percent);
		
		if (total_free < min_free_space) {
			min_free_space = total_free;
			cars_at_min = n_cars;
		}
		//END OF SIMULATION LOOP
	}

	if (sim_num == 0 || sim_num == 1) {
		fp_aggr = freopen("data_aggr.out","wb", fp_aggr);
		fprintf(fp_aggr, "Simulation nr:   Min Free Space:   Cars at min:\n");
		fprintf(fp_aggr, "%d   %f  %d\n", sim_num, min_free_space, cars_at_min);
	} else { 
		fprintf(fp_aggr, "%d   %f  %d\n", sim_num, min_free_space, cars_at_min);
	}


	if (VERBOSE) {
		printf("\n");
		printf("Number of cars at in the street after complete simulation: %d\n", n_cars );
		printf("Street Length: %f \n", street_length);
	}

	

	//free(street);
	free(cars_array);
	
	fclose(fp);
	fclose(fp_aggr);
	//getch();
	return 0;
	//END OF MAIN

}
