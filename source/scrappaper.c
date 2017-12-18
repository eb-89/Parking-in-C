//some old code
typedef struct {
	int is_occupied;
	int arrival_time;
	int departure_time;
} Spot;



// where to park

			//inerate the list
			//for each get car - car(-1)
			//save carx and car.x 

			//new_car.x = 100*((double) rand() / (double) (RAND_MAX));


			//printf("\n new car.x %f\n", new_car.x );
			//new_car.strategy = normal(5,1);
			//new_car.parking_time = normal(5,1);
			//

			//append(&head, new_car);



			// printf("    Car: %d \n", j);

			/* SOME OLD STUFF
			int empty_slots = 0;
			int empty_slot_index = 0;

			
			for (k = 0; k<street_length; k++) {
				if (!street[k].is_occupied) {
					empty_slots++;
				}
			}

			printf("     Empty slots to choose from: %d\n", empty_slots);

			//check where you fit and pick one randomly
			//place randomly on x in fitting spot
			//recalculate empty space. 

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
							//street[k].departure_time = i + CAR_STAY_TIME;
							int stay_time = (int) ceil(normal(5,1));
							printf("     Staying for: %d \n", stay_time);
							street[k].departure_time = i + stay_time;
							printf("     Filled slot at index: %d\n", k);
						}

					}

				}

			} else {
				printf("     No empty spots left!\n");
			}


			}

				
			for (j=0; j<n_cars; j++) {
					//if (n_cars[j].depart_time)
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
		*/

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