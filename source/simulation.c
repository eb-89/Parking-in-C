/*   Parking Simulation 
*    Simulates parking along a street, given various driver behaviours
*    
*    Outputs various files filled with data
*
*
*/


#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* Flags

 VERBOSE -- gives detailed console printouts .
 ANIMOUT -- If 1, Prints animation file.

*/
#define VERBOSE 0
#define ANIMOUT 0


/* STAYTIME_COEFF: We calculate car stay time. 

**If STAYTIME_COEFF == 1, we expect that the first car leaves when the street is full. (on avg)
**If we don't preset cars, cars' stay time is given by these eqns:

  car_stay_time_mu = STAYTIME_COEFF*street_length/((car_length_mu+2*safe_dist)*NO_OF_CARS_POISSON);
  car_stay_time_sigma = car_stay_time_mu*0.1;

Otherwise, plug these in:
  #define CAR_STAY_TIME_MU 30
  #define CAR_STAY_TIME_SIGMA 1

*/
#define STAYTIME_COEFF 0.8


/* 
 -- PREINIT_PERCENT Initializes the simulation to a certain %
 -- PERMATIME - leave time for permanent cars.
*/
#define PREINIT_PERCENT 0
#define PERMATIME 100000

/* Boolean - if this is 1, data is outputted when first car leaves. */
#define START_AT_FIRST_LEAVE 1

/* NO_OF_CARS_POISSON - poisson distribution param. */
#define NO_OF_CARS_POISSON 1

/* Break the simulation when this many cars can't fit */
#define BREAK_CANNOT_FIT 30
/* Alternatively - change the main loop and specify number of iterations 
#define DAY_LENGTH 60*12
*/

/* Defined safe distance */
#define SAFE_DISTANCE 0.2


/* Function prototypes */
int poisson(int lambda);
double normal(double mu, double sigma);

/* A Car */
typedef struct {
	int car_nr;
	double x;
	double y;
	int depart_time;
	int strategy;
	int spot_strategy;
	double length;
} Car;



double normal(double mu, double sigma)
//sigma here is standard deviation, s^2 would be the variance.
{
	double u1 = ((double) rand() / (double) (RAND_MAX));
	double u2 = ((double) rand() / (double) (RAND_MAX));

	double z = sqrt(-2*log(u1+0.00001))*cos(2*M_PI*u2);
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







int main(int argc, char *argv[]) {

	/* Control Variables  */
	double street_length; // Streetlength
	int sim_num; // Simulation number
	double type_percentage; // Percentage of good drivers 
	int seed;
	double safe_dist;
	double car_stay_time_mu;
	double car_stay_time_sigma;
	double car_length_mu = 4.2;
	double car_length_sigma = 0.83;
	safe_dist = SAFE_DISTANCE;

	/* Booleans for start at first leave, and for preinitialization */
	int preinit_switch = 1;
	int start_afl_switch = 1;
	

	if (argc == 5) {
		sscanf(argv[1], "%d", &sim_num);
		sscanf(argv[2], "%d", &seed);

		sscanf(argv[3], "%lf", &street_length);
		sscanf(argv[4], "%lf", &type_percentage);
		
	} else if (argc != 1) {
		printf("Please provide arguments: \n  sim_num \n seed \n street_lenth \n precent gooddrivers \n ");
		exit(0);
	} 


	car_stay_time_mu = STAYTIME_COEFF*street_length/((car_length_mu+2*safe_dist)*NO_OF_CARS_POISSON);
	car_stay_time_sigma = car_stay_time_mu*0.1;


	//printf("Stay time - mean: %f, Stay time - stdDv:%f\n", car_stay_time_mu,  car_stay_time_sigma);
	
	/* Setup to rewrite data#.out file */
	FILE *fp; //pointer to data per sim file
	char output_file_name[64];
	
	sprintf(output_file_name, "sim_data/data%d.out", sim_num);

	printf(output_file_name);
	printf("\n");

	fp = fopen(output_file_name, "wb");

	fprintf(fp, "Iteration:  NumOfCars:  CarsComing:  CarsThatDontFit:  CarsLeaving:  FreeSpace:  PercentFreeSpace:\n" );


	/* Setup to write to data_aggr#.out file */
	FILE *fp_aggr;  //pointer to aggriate data
	fp_aggr = fopen("data_aggr.out","a");

	FILE *fp_anim;
	if (ANIMOUT) {
		fp_anim = fopen("anim/data/data_anim.out","wb");
		fprintf(fp_anim, "%f\n", street_length);
	}

	/* Seeding */
	srand(seed);

	/* The number of cars in the street  */
	int n_cars = 0; 

	/* The array of cars, grows dynamically  */
	int used_space = 1;
	Car * cars_array = malloc(sizeof(Car) * used_space);

	/* Variables that get printed */
	double min_free_space = street_length; // minimum free space
	double avg_free_space_percent = 0; // avg free space
	double min_free_space_percent = 0;
	int cars_at_min = 0;
	int total_didnt_fit = 0;
	double total_free; //free space (written at the END of each iteration)
	double total_free_percent = 100;
	int completed_iters;
	int car_index = 0;


	int i; //loop variable
	
	if (preinit_switch && PREINIT_PERCENT) {
		printf("Initializing street to %d percent\n", (int) PREINIT_PERCENT);
	}


	/**** MAIN OUTER LOOP ****/
	for (i = 0; 1; i++)	{
		
		int j;  //loop variable

		int cars_coming = poisson(NO_OF_CARS_POISSON);
		
		int didnt_fit_counter = 0;
		int leave_counter = 0;

		/* Initializes the simulation to certain %  */
		if (preinit_switch && PREINIT_PERCENT) {
			if (total_free_percent > 100 - PREINIT_PERCENT) {
				i = -1;	

				
			}  else {
				car_stay_time_mu = STAYTIME_COEFF*total_free/((car_length_mu+2*safe_dist)*NO_OF_CARS_POISSON);
				car_stay_time_sigma = car_stay_time_mu*0.1;
				preinit_switch = 0;
				i = 0;
				printf("Running 0th iteration now...\n");
			}
				
		} 

		if (ANIMOUT) {
			fprintf(fp_anim, "%d\n", i);
		}

		
		/* check resizing the array*/
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
		
		/* Done for each coming car */
		for (j = 0; j<cars_coming;j++) {

			Car new_car;
			
			new_car.car_nr = car_index;
			new_car.length = normal(car_length_mu, car_length_sigma);
			new_car.depart_time = i + normal(car_stay_time_mu, car_stay_time_sigma);
			

			int k = 0; //loop variable

			/* Set driver behaviour */
			if (((rand() % 100) +1) <= type_percentage) {
				//this is "goodParker"
				new_car.strategy = 2;
				new_car.spot_strategy = 0; //random avaibale, always supernice, here is the strat % passed as arg.
			} else {
				new_car.strategy = 2; //random avaibale always in middle.
				new_car.spot_strategy = 0; 
			}
			


			car_index++;
			
			if (i == -1) {
				new_car.strategy = 1;
				new_car.spot_strategy = 1;
				new_car.depart_time = PERMATIME;
			}
			

			if (VERBOSE) {
				printf("\n     Car: %d:   \n", new_car.car_nr);
				printf("      length: %f  \n", new_car.length);
				printf("      strategy: %d  \n", new_car.strategy);
				printf("      staying 'til iteration: %d", new_car.depart_time);
			}

			
			/* Possible spaces for parking */
			double (*distances)[2] = malloc(sizeof(double)*2*(n_cars +1));
			if (distances == NULL) {
				printf("Malloc Failed\n");
			}
			distances[0][0] = 0;
			distances[n_cars][1] = street_length;

			
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

			}

			//bugcheck
			for (k = 0; k<n_cars+1; k++) {
				if ((distances[k][1] - distances[k][0]) < 0) {
					printf("Error - parkable space is empty \n");
					exit(0);
				}

			}

			/* 
			*   possible_distances stores the indeces for potential slots for inserting a new car.
			*/
			int *possible_distances = malloc(sizeof(int)*(n_cars+1));


			int m = 0;  /* Number of empty spaces */
			for (k = 0; k<n_cars+1; k++) {

				if (n_cars == 0) {
					if ((distances[k][1] - distances[k][0]) >= new_car.length + 0.000) {
					
					possible_distances[m] = k;
					m++;

					}

				} else if (k == 0 || k == n_cars) {
					if ((distances[k][1] - distances[k][0]) >= new_car.length + safe_dist + 0.000) {
					
					possible_distances[m] = k;
					m++;

					}

				} else {


					if ((distances[k][1] - distances[k][0]) >= new_car.length + 2*safe_dist + 0.000) {
					
					possible_distances[m] = k;
					m++;

					}
				}
				//distances[k+1][0] = cars_array[k].x + cars_array[k].length;
			}

			if (m != 0) {
				int random_spot = rand() % m;;
				int index; 


				/* index  will determine where the car parks: 
					0 = always first available
					1 = random spot available
					2 = largest spot available
					3 = smallest spot available

				*/
				if (new_car.spot_strategy == 0) {
					index = possible_distances[0];

				} else if (new_car.spot_strategy == 1) {
					index = possible_distances[random_spot];

				} else if (new_car.spot_strategy == 2) {
					index = possible_distances[0];
					int l;
					int max_len = distances[index][1] - distances[index][0];
					for (l = 0; l<m; l++) {

						int pd = possible_distances[l];
					 	if (distances[pd][1] - distances[pd][0] > max_len) {
					 		max_len = distances[pd][1] - distances[pd][0];
					 		index = pd;
					 	}
					}

				} else if (new_car.spot_strategy == 3) {
					index = possible_distances[0];
					int l;
					int min_len = distances[index][1] - distances[index][0];
					for (l = 0; l<m; l++) {

						int pd = possible_distances[l];
					 	if (distances[pd][1] - distances[pd][0] < min_len) {
					 		min_len = distances[pd][1] - distances[pd][0];
					 		index = pd;
					 	}
					}
				}

				for (k = n_cars; k != index; k = k-1) {
					
					//printf("moving slots, carrs[%d] is assigned carrs[%d] \n", k,k-1);
					cars_array[k] = cars_array[k-1];
					//printf("value of k:%d \n", k);
				}

				
				double allowed_random_x;

				double start = distances[index][0];
				double end = distances[index][1];
				double len = new_car.length;
				switch (new_car.strategy)  {
					
					

					case 0: /* Always park farthest to the right */
						if (start == 0) {
							new_car.x = start;
						} else {
							new_car.x = start  + safe_dist;
						}
						break;
					case 1: /* Park randomly in your space */
						
						//These are edge cases at the end and beginning of the street.
						if (n_cars == 0 ) {
							allowed_random_x = (end - start - len)*((double) rand() / (double) (RAND_MAX));
							new_car.x = allowed_random_x + start;

						} else if (end == street_length) {
							allowed_random_x = (end - start - len - safe_dist)*((double) rand() / (double) (RAND_MAX));
							new_car.x = allowed_random_x + start + safe_dist;
							
						} else if (start == 0) {
							allowed_random_x = (end - start - len - safe_dist)*((double) rand() / (double) (RAND_MAX));
							new_car.x = allowed_random_x + start;
							
						//this is the common case
						} else {
							allowed_random_x = (end - start - len - 2*safe_dist)*((double) rand() / (double) (RAND_MAX));
							new_car.x = allowed_random_x + start  + safe_dist;
						}
						break;
					
					case 2: /* Always park in the middle of the space */ 
						new_car.x = ((end - start) - len) / 2 + start;
						
						//Special cases for edges of the street
						double next_car_begin = cars_array[index+1].x;
						double prev_car_end = cars_array[index-1].x + cars_array[index-1].length;

						if (n_cars > 0 && start == 0) {
							if (cars_array[index+1].x - (new_car.x + len)  < safe_dist) {
								
								/* Move the car by the difference between the safety distance and the car distance*/
								new_car.x = new_car.x - (safe_dist - (next_car_begin - (new_car.x + len)));
							}
						} else if (n_cars > 0 && end == street_length) {
							if (new_car.x - (prev_car_end) < safe_dist) {
								
								/* Move the car by the difference between the safety distance and the car distance*/
								new_car.x = new_car.x + (safe_dist - (new_car.x - prev_car_end));
							}
						}
							
						break;
				}

				cars_array[index] = new_car;
				
				if (VERBOSE) {
					printf("    Putting car %d at index %d, with x-coord %f \n", new_car.car_nr, index, new_car.x);
				}

				n_cars++;
				
			} else {
				didnt_fit_counter++;
				if (VERBOSE) {
					printf("    CANNOT FIND SPACE\n");
				}
			}
			
			/* Animation data output */
			if (ANIMOUT) {
				int permanent_car;
				if (m != 0) {
					
					int ii;
					fprintf(fp_anim, "%d %d %d %d %f \n", i, 1, n_cars, new_car.car_nr, new_car.x);
					
					for (ii = 0; ii < n_cars; ii++) {

						if (cars_array[ii].depart_time == PERMATIME) {
							permanent_car = 1;
						} else {
							permanent_car = 0;
						}

						fprintf(fp_anim, "%f %f %d \n", cars_array[ii].x, cars_array[ii].length, permanent_car);
					}
				} else {
					int ii;
					
					fprintf(fp_anim, "%d %d %d %d \n", i, 0, n_cars, new_car.car_nr);
					
					for (ii = 0; ii < n_cars; ii++) {

						if (cars_array[ii].depart_time == PERMATIME) {
							permanent_car = 1;
						} else {
							permanent_car = 0;
						}

						fprintf(fp_anim, "%f %f %d \n", cars_array[ii].x, cars_array[ii].length, permanent_car);
					}
			}

			


			free(possible_distances);
			free(distances);
			
			/* END OF CAR LOOP */
		}
		

		/* Departures */
		if (VERBOSE) {
			printf("\n    -- Departures --- \n");
		}

		j = 0;
		while (j < n_cars) {
				if (i > cars_array[j].depart_time) {
					int permanent_car;
					if (VERBOSE) {
						printf("    Car %d departing, parked at x-coord %f\n", cars_array[j].car_nr, cars_array[j].x);
					}

					if (ANIMOUT) {
						fprintf(fp_anim, "%d %d %d %d\n", i, -1, n_cars-1, cars_array[j].car_nr);
					}
					
					int l;
					for (l = j; l < n_cars-1; l++) {
						cars_array[l] = cars_array[l+1];
					}

					n_cars--;
					leave_counter++;

					if (ANIMOUT) {
						
						for (l = 0; l < n_cars; l++) {
							if (cars_array[l].depart_time == PERMATIME) {
								permanent_car = 1;
							} else {
								permanent_car = 0;
							}
							fprintf(fp_anim, "%f %f %d \n",cars_array[l].x, cars_array[l].length, permanent_car);
						}
					}

				} else {

					j++;
				}
		}
		if (VERBOSE) {
			printf(" \n\n");
		}
		

		//bugcheck
		for (j=1; j<n_cars; j++) {
			if ((cars_array[j].x - cars_array[j-1].x) < cars_array[j-1].length )
			printf("Cars overlap\n");
			exit(0)
		}

		/* Calculation of data for this iteration */
		total_free = street_length;
		total_free_percent = 0;
		
		for (j=0; j<n_cars; j++) {

			total_free = total_free - (cars_array[j].length);
		}



		total_free_percent = (total_free / street_length)*100;

		if (VERBOSE) {
			printf("Writing: \nIteration:  %d, \nn_cars:  %d, \ncars coming:  %d, \nDidn't Fit:  %d,  \nleaving: %d, \nTotal Free:  %f, \nTotal Free Percent:  %f \n", i, n_cars, cars_coming, didnt_fit_counter, leave_counter, total_free, total_free_percent);
		}

		if (total_free < min_free_space) {

			min_free_space = total_free;
			cars_at_min = n_cars;
			min_free_space_percent = total_free_percent;
		}

		avg_free_space_percent += total_free_percent; 

		total_didnt_fit += didnt_fit_counter;

		//Various printouts, skipped if counter = -1 (so initalization);
		//can also be skipped until first car leaves. (Anim stuff still print)
		if (leave_counter == 0 && start_afl_switch && START_AT_FIRST_LEAVE) {
			continue;
		} else {
			start_afl_switch = 0;
		}

		if (i == -1) {
			continue;
		}
		
		fprintf(fp, "  %d  %d  %d  %d  %d  %f  %f \n", i, n_cars, cars_coming, didnt_fit_counter, leave_counter, total_free, total_free_percent);
		
		/* BREAK THE LOOP HERE */
		if (total_didnt_fit > BREAK_CANNOT_FIT) 
			{completed_iters = i; break;}

		//END OF SIMULATION LOOP
	}


	/* Calculations and printsouts over the whole simulation */
	avg_free_space_percent = avg_free_space_percent / ((double) completed_iters) ;

	if (sim_num == 0 || sim_num == 1) {
		fp_aggr = freopen("data_aggr.out","wb", fp_aggr);
		fprintf(fp_aggr, "SimulationNr:   MinFreeSpace:   CarsAtMin:   MinFreePercent   GoodDriverPercent   CarsDidntFitTotal  AvgFreePercent  complIters  Streetlength\n");
		fprintf(fp_aggr, "%d   %f  %d   %f   %f  %d  %f  %d %f\n", sim_num, min_free_space, cars_at_min, min_free_space_percent, type_percentage, total_didnt_fit, avg_free_space_percent, completed_iters, street_length);
	} else { 
		fprintf(fp_aggr, "%d   %f  %d   %f   %f  %d  %f  %d %f\n", sim_num, min_free_space, cars_at_min, min_free_space_percent, type_percentage, total_didnt_fit, avg_free_space_percent, completed_iters, street_length);
	}


	if (VERBOSE) {
		printf("\n");
		printf("Number of cars at in the street after complete simulation: %d\n", n_cars );
		printf("Street Length: %f \n", street_length);
	}

	/* Finalize */

	free(cars_array);
	
	if (ANIMOUT) {
		fclose(fp_anim);
	}
	fclose(fp);
	fclose(fp_aggr);
	
	return 0;
	//END OF MAIN

}
