#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//#include <random.h>
//#include "linkedlist.h"


#define VERBOSE 0
#define ANIMOUT 1

#define DAY_LENGTH 60*12

//these dont matter, defined in code
#define CAR_STAY_TIME_MU 30
#define CAR_STAY_TIME_SIGMA 1

//this coefficient measures something like competition
#define STAYTIME_COEFF 1
/////


#define PREINIT_PERCENT 0
#define PERMATIME 100000

#define START_AT_FIRST_LEAVE 1

#define NO_OF_CARS_POISSON 1
#define BREAK_CANNOT_FIT 30




//defaults
#define SAFE_DISTANCE 0.2
#define STREET_LENGTH 100.0
#define SEED 5

int poisson(int lambda);
void start_input(int *street_length_units);
double normal(double mu, double sigma);


typedef struct {
	int car_nr;
	double x;
	double y;
	int depart_time;
	int strategy;
	int spot_strategy;
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




void start_input(int *street_length_units)
{
	printf("How long is the steet in parking units? \n");
	scanf("%d", street_length_units);
}




int main(int argc, char *argv[]) {


	double street_length; //Streetlength
	int sim_num; //simulation number
	double type_percentage; //percentage of good drivers 
	int seed;
	double safe_dist;
	double car_stay_time_mu;
	double car_stay_time_sigma;
	double car_length_mu = 4.2;
	double car_length_sigma = 0.83;
	
	int preinit_switch = 1;
	int start_afl_switch = 1;
	
	//NOTE defaults -- these get overwritten
	safe_dist = SAFE_DISTANCE;
	street_length = STREET_LENGTH;
	sim_num = 0; 
	type_percentage = 100;
	seed = SEED;

	if (argc == 5) {
		sscanf(argv[1], "%d", &sim_num);
		sscanf(argv[2], "%d", &seed);

		sscanf(argv[3], "%lf", &street_length);
		sscanf(argv[4], "%lf", &type_percentage);
		
	} else if (argc != 1) {
		printf("Not enuff args, it goes: \n  sim num \n seed \n street lenth \n precent gooddrivers \n   Using defaults \n");
	} 


	car_stay_time_mu = STAYTIME_COEFF*street_length/((car_length_mu+2*safe_dist)*NO_OF_CARS_POISSON);
	car_stay_time_sigma = car_stay_time_mu*0.1;


	printf("%f %f\n", car_stay_time_mu,  car_stay_time_sigma);
	//safe_dist = safe_dist + ((double) sim_num) * 0.2;

	FILE *fp; //pointer to data per sim file
	char output_file_name[64];
	
	sprintf(output_file_name, "sim_data/data%d.out", sim_num);
	//sprintf(output_file_name, "sim_data/data.out");

	printf(output_file_name);
	printf("\n");

	fp = fopen(output_file_name, "wb");

	fprintf(fp, "Iteration:  NumOfCars:  CarsComing:  CarsThatDontFit:  CarsLeaving:  FreeSpace:  PercentFreeSpace:\n" );

	FILE *fp_aggr;  //pointer to aggriate data
	fp_aggr = fopen("data_aggr.out","a");

	FILE *fp_anim;
	if (ANIMOUT) {
		fp_anim = fopen("anim/data/data_anim.out","wb");
		fprintf(fp_anim, "%f\n", street_length);
	}
	/*
	switch (argc) {
		case 2:
			sscanf(argv[1], "%lf", &street_length);
			break;
		case 3: 
			sscanf(argv[1], "%lf", &street_length);
			sscanf(argv[2], "%d", &sim_num);
			break;
		case 4: 
			sscanf(argv[1], "%lf", &street_length);
			sscanf(argv[2], "%d", &sim_num);
			sscanf(argv[3], "%lf", &type_percentage);
			break;
		default:
			street_length = STREET_LENGTH;
			sim_num = 0;
			break;
	}
	*/

	//int street_length = 5;
	//int rate_of_arrival;
	//int rate_of_departure;

	//initialization
		srand(seed);
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
	double avg_free_space_percent = 0; // minimum free space
	double min_free_space_percent = 0;
	int cars_at_min = 0;
	int total_didnt_fit = 0;
	double total_free; //free space (written at the END of each iteration)
	double total_free_percent = 100;

	int completed_iters = DAY_LENGTH;


	int i; //loop variable

	/*initalization, u can use calloc.
	
	for (i=0; i<street_length; i++) {
		street[i].is_occupied = 0;
	}
	*/

	

	//main iteration loop:   /7Old condition i < DAY_LENGTH	
	if (preinit_switch && PREINIT_PERCENT) {
		printf("Initializing street to %d percent\n", (int) PREINIT_PERCENT);
	}

	for (i = 0; i==i; i++)	{
		//printf("did this lineSTAR %d \n", i);
		/*
		if (PREINIT_PERCENT) {
			if (total_free_percent > 100 - PREINIT_PERCENT) {
				i = 0;
			}
		}
		*/
		int j;
		int cars_coming = poisson(NO_OF_CARS_POISSON);
		int didnt_fit_counter = 0;
		int leave_counter = 0;


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
			new_car.length = normal(car_length_mu, car_length_sigma);
			new_car.depart_time = i + normal(car_stay_time_mu, car_stay_time_sigma);
			

			//COMPARE DRIVERS
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

			//bugcheck
			for (k = 0; k<n_cars+1; k++) {
				if ((distances[k][1] - distances[k][0]) < 0) {
					printf("negative dist");
				}

			}

			//possible distances stores INDECES.
			//The indeces are potential slots for inserting a new car.
			int *possible_distances = malloc(sizeof(int)*(n_cars+1));

			int m = 0;
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
				int index; // = possible_distances[random_spot];


				//NOTe THE DOUBLE CHANGE HERE
				if (new_car.spot_strategy == 0) {
					index = possible_distances[0];
				} else if (new_car.spot_strategy == 1) {
					index = possible_distances[random_spot];
				} else if (new_car.spot_strategy == 2) {
					index = possible_distances[0];
					int l;
					int max_len = distances[index][1] - distances[index][0];
					for (l = 0; l<m; l++) {
					 	if (distances[possible_distances[l]][1] - distances[possible_distances[l]][0] > max_len) {
					 		max_len = distances[possible_distances[l]][1] - distances[possible_distances[l]][0];
					 		index = possible_distances[l];
					 	}
					}
				} else if (new_car.spot_strategy == 3) {
					index = possible_distances[0];
					int l;
					int min_len = distances[index][1] - distances[index][0];
					for (l = 0; l<m; l++) {
					 	if (distances[possible_distances[l]][1] - distances[possible_distances[l]][0] < min_len) {
					 		min_len = distances[possible_distances[l]][1] - distances[possible_distances[l]][0];
					 		index = possible_distances[l];
					 	}
					}
				}

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
							new_car.x = distances[index][0]  + safe_dist;
						}
						break;
					case 1: //random x in random avaibale
						if (n_cars == 0 ) {
							allowed_random_x = (distances[index][1] - distances[index][0] - new_car.length)*((double) rand() / (double) (RAND_MAX));
							new_car.x = allowed_random_x + distances[index][0];
							//printf("Putting car in only spot\n");
						} else if (distances[index][1] == street_length /*STREET_LENGTH*/ ) {
							allowed_random_x = (distances[index][1] - distances[index][0] - new_car.length - safe_dist)*((double) rand() / (double) (RAND_MAX));
							new_car.x = allowed_random_x + distances[index][0] + safe_dist;
							//printf("Putting car in last spot\n");
						} else if (distances[index][0] == 0) {
							allowed_random_x = (distances[index][1] - distances[index][0] - new_car.length - safe_dist)*((double) rand() / (double) (RAND_MAX));
							new_car.x = allowed_random_x + distances[index][0];
							//printf("Putting car in first spot\n");
						} else {
							allowed_random_x = (distances[index][1] - distances[index][0] - new_car.length - 2*safe_dist)*((double) rand() / (double) (RAND_MAX));
							new_car.x = allowed_random_x + distances[index][0]  + safe_dist;
						}
						break;
					case 2: //always in middle, if you are on the edges, then shift to not break 2m rule
						new_car.x = ((distances[index][1] - distances[index][0]) - new_car.length) / 2 + distances[index][0];
						if (n_cars > 0 && distances[index][0] == 0) {
							if (cars_array[index+1].x - (new_car.x + new_car.length)  < safe_dist) {
								//printf("trying first, too close, dist= %f\n", cars_array[index+1].x - (new_car.x + new_car.length));
								
								//printf("moved the car by %f\n",(SAFE_DISTANCE - (cars_array[index+1].x - (new_car.x + new_car.length))) );
								new_car.x = new_car.x - (safe_dist - (cars_array[index+1].x - (new_car.x + new_car.length)));
							}
						} else if (n_cars > 0 && distances[index][1] == street_length /*STREET_LENGTH*/ ) {
							if (new_car.x - (cars_array[index-1].x + cars_array[index-1].length) < safe_dist) {
								//printf("trying last too close, dist= %f\n", new_car.x - (cars_array[index-1].x + cars_array[index-1].length) );
								
								//printf("moved the car by %f\n",(SAFE_DISTANCE - (new_car.x - (cars_array[index-1].x + cars_array[index-1].length)) ) ) ;
								new_car.x = new_car.x + (safe_dist - (new_car.x - (cars_array[index-1].x + cars_array[index-1].length)) );
							}
						}
							
						break;
					/*
					case 3:  //first spot it finds.  //THIS CHENGED INDEX ABOVE
						index = possible_distances[0];
						if (distances[index][0] == 0) {
							new_car.x = distances[index][0];
						} else {
							new_car.x = distances[index][0]  + safe_dist;
						}
						break;
					*/
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
			if (ANIMOUT) {
				int permanent_car;
				if (m != 0) {
					//fprintf(fp_anim, "%d %d %f %f %f \n", i, new_car.car_nr, new_car.length, new_car.x, street_length);
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
					//fprintf(fp_anim, "%d %d %f %d %f \n", i, new_car.car_nr, new_car.length, -1, street_length);
				}
			}

			


			free(possible_distances);
			//printf("Ive freed pinter\n");
			free(distances);
			//free(distances);
						
			
			//END OF for each car loop
		}
		

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
		//printf("did this lineEND %d \n", i);

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
			//if (PREINIT_PERCENT & cars_array[j].depart_time == 100000) {continue;}

			total_free = total_free - (cars_array[j].length);
		}

		//bugcheck
		for (j=1; j<n_cars; j++) {
			if ((cars_array[j].x - cars_array[j-1].x) < cars_array[j-1].length )
			printf("Cars overlap\n");
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

		avg_free_space_percent += total_free_percent; // minimum free space


		total_didnt_fit += didnt_fit_counter;

		/*
		if (START_AT_FIRST_LEAVE && start_afl_switch) {
			if (leave_counter == 0) {
				printf("value of counter %d \n", leave_counter);
				printf("did this\n");
				
			} else {
				printf("did this\n");
				start_afl_switch = 0;
				i = 0;
			}	
		}
		*/

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
		
		//BREAKCODE
		if (total_didnt_fit > BREAK_CANNOT_FIT) 
			{completed_iters = i; break;}

		
		//END OF SIMULATION LOOP
	}

	//bugcheck print street at the end iff
	if (0) {
	//if (min_free_space_percent > 50) {
	//if (sim_num == 100) {
		printf("Percent over 6\n");
		for (i=0; i<n_cars; i++) {

			printf("%d %f\n", i, cars_array[i].x);
		}

	} 


	avg_free_space_percent = avg_free_space_percent / ((double) completed_iters) ;
	/*else if (sim_num == 1) {
		printf("Here is normal array\n");
		for (i=0; i<n_cars; i++) {

			printf("%d %f\n",i, cars_array[i].x);
		}

	}
	*/

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

	

	//free(street);
	free(cars_array);
	
	if (ANIMOUT) {
		fclose(fp_anim);
	}

	fclose(fp);
	
	fclose(fp_aggr);
	//getch();
	return 0;
	//END OF MAIN

}
