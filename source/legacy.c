//some old code

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