#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//#include "linkedlist.h"





#define DAY_LENGTH 1//12*12
#define CAR_STAY_TIME 3
#define STREET_LENGTH 200.0

int poisson(int lambda);
void start_input(int *street_length_units);
double normal(double mu, double sigma);


typedef struct {
	int is_occupied;
	int arrival_time;
	int departure_time;
} Spot;


typedef struct {
	double x;
	double y;
	int depart_time;
	int strategy;
	double length;
} Car;

// A linked list node
struct Node
{
    Car data;
    struct Node *next;
    struct Node *prev;
};
 
/* Given a reference (pointer to pointer) to the head of a list
   and an int, inserts a new node on the front of the list. */
void push(struct Node** head_ref, Car new_data)
{
    /* 1. allocate node */
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
 
    /* 2. put in the data  */
    new_node->data  = new_data;
 
    /* 3. Make next of new node as head and previous as NULL */
    new_node->next = (*head_ref);
    new_node->prev = NULL;
 
    /* 4. change prev of head node to new node */
    if((*head_ref) !=  NULL)
      (*head_ref)->prev = new_node ;
 
    /* 5. move the head to point to the new node */
    (*head_ref)    = new_node;
}
 
/* Given a node as prev_node, insert a new node after the given node */
void insertAfter(struct Node* prev_node, Car new_data)
{
    /*1. check if the given prev_node is NULL */
    if (prev_node == NULL)
    {
        printf("the given previous node cannot be NULL");
        return;
    }
 
    /* 2. allocate new node */
    struct Node* new_node =(struct Node*) malloc(sizeof(struct Node));
 
    /* 3. put in the data  */
    new_node->data  = new_data;
 
    /* 4. Make next of new node as next of prev_node */
    new_node->next = prev_node->next;
 
    /* 5. Make the next of prev_node as new_node */
    prev_node->next = new_node;
 
    /* 6. Make prev_node as previous of new_node */
    new_node->prev = prev_node;
 
    /* 7. Change previous of new_node's next node */
    if (new_node->next != NULL)
      new_node->next->prev = new_node;
}
 
/* Given a reference (pointer to pointer) to the head
   of a DLL and an int, appends a new node at the end  */
void append(struct Node** head_ref, Car new_data)
{
    /* 1. allocate node */
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
 
    struct Node *last = *head_ref;  /* used in step 5*/
 
    /* 2. put in the data  */
    new_node->data  = new_data;
 
    /* 3. This new node is going to be the last node, so
          make next of it as NULL*/
    new_node->next = NULL;
 
    /* 4. If the Linked List is empty, then make the new
          node as head */
    if (*head_ref == NULL)
    {
        new_node->prev = NULL;
        *head_ref = new_node;
        return;
    }
 
    /* 5. Else traverse till the last node */
    while (last->next != NULL)
        last = last->next;
 
    /* 6. Change the next of last node */
    last->next = new_node;
 
    /* 7. Make last node as previous of new node */
    new_node->prev = last;
 
    return;
}

int countList(struct Node *node)
{
    int n = 0;
    struct Node *last;
    while (node != NULL)
    {
        n++;

        last = node;
        node = node->next;
    }
}


void printList(struct Node *node)
{
    struct Node *last;
    printf("\nTraversal in forward direction \n");
    while (node != NULL)
    {
        printf(" %f ", node->data.x);
        last = node;
        node = node->next;
    }
 
    printf("\nTraversal in reverse direction \n");
    while (last != NULL)
    {
        printf(" %f ", last->data.x);
        last = last->prev;
    }
}


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


	int street_length = 3;
	int rate_of_arrival;
	int rate_of_departure;

	//initialization
	srand((unsigned) time(NULL));
	//start_input(&street_length);


	//head of the linked list containing cars.
	//struct Node *head = NULL; 



	//reformulate in terms of a pointer to car.
	Spot *street = malloc(sizeof(Spot) * street_length);

	int n_cars = 0;
	int used_space = 1;
	Car * cars_array = malloc(sizeof(Car) * used_space);

	//initalization, u can use calloc.
	int i;
	for (i=0; i<street_length; i++) {
		street[i].is_occupied = 0;
	}


	//main iteration loop:	
	for (i = 0; i < DAY_LENGTH; i++)	{

		int j;
		int cars_coming = 15; //poisson(3);

		if (n_cars + cars_coming >= used_space) {
			used_space = n_cars + cars_coming;
			cars_array = realloc(cars_array, sizeof(Car) * used_space);
			if (cars_array == NULL) {
				printf("Eroor\n");
			}
		}

		printf("Iteration: %d: \n", i);
		printf("  Cars coming for this iteration: %d \n", cars_coming);

		//here calculate empty spaces

		for (j = 0; j<cars_coming;j++) {
			
			printf("Heres a new car\n\n");

			Car new_car;
			new_car.length = 5.0;
			new_car.depart_time = 5;
			
			//new_car.x = 50.0;

			printf("    Car: %d \n", j);

			double (*distances)[2] = malloc(sizeof(double)*2 * (n_cars +1));
			if (distances == NULL) {
				printf("Malloc Failed\n");
			}
			distances[0][0] = 0;
			distances[n_cars][1] = STREET_LENGTH;

			int k = 0; //variable for loops

			for (k = 0; k<n_cars; k++) {
				distances[k][1] = cars_array[k].x;
				distances[k+1][0] = cars_array[k].x + cars_array[k].length;
				if (k == 1 ) { printf("k == 1 cars_array[1].x is then %f\n", distances[k][1]);} 

			}

			//cars_array[0] = new_car;

			printf("Here are the dists: \n");
			for (k = 0; k<n_cars+1; k++) {
				printf("%f %f \n" , distances[k][0], distances[k][1]);

			}

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
					
					printf("moving slots, carrs[%d] is assigned carrs[%d] \n", k,k-1);
					cars_array[k] = cars_array[k-1];
					printf("value of k:%d \n", k);
				}

				new_car.x = (distances[index][1] - distances[index][0] - new_car.length)*((double) rand() / (double) (RAND_MAX)) + distances[index][0];
				cars_array[index] = new_car;
				printf("Putting in slot %d\n", index);
			} else {
				printf("Cannot find spot\n");
			}
			

			n_cars++;
			printf("Ive Incresed cars\n");

			free(possible_distances);
			printf("Ive freed pinter\n");
			free(distances);
			//free(distances);
						
			

		}

		printf(" \n\n");


	}

	printf("n_cars is %d\n", n_cars );
	printf("The street at the end of iteration in array %d: ", i);
	
	

	free(street);
	free(cars_array);
	
	getch();
	return 0;

}
