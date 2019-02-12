/*
 * ---------------------------------------------------------------------------
 *	 
 *   Collaborative movie recommendation (item based)
 *
 *   Developed by: Maria Luisa Costa Pinto
 *   www.homepages.dcc.ufmg.br/~maria.luisa/
 *
 *   main.c
 *   All the algorithm developed is in this file
 * 
 * ---------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define MAXUSERS 40000
#define MAXEVALUATION 5000
#define MAXITEMS 6000

// DATA TYPES

typedef struct type_item {
  char id[12];
  int users_evaluation[MAXUSERS];
  int ratings;
} type_item; 

typedef struct similar_item {
  char id[12];
  float correlation;
  int rating;
} similar_item; 

typedef struct user_items {
  char id[12];
  int size;
  type_item *item[MAXEVALUATION];
} user_items; 


struct array_items {
  type_item item[MAXITEMS];
  int id;
  int size;
  struct array_items *prox;
} array_items; 

typedef struct array_items list;

// 	DATA STRUCTURE
// 	Functions related to the linked list

list* init(void)
{
 return NULL;
}

list* insert(list* lst, int id)
{
 	list* newItem = (list*) malloc(sizeof(list));
 	newItem->id = id;
 	newItem->size = 0;

	newItem->prox = lst;
  return newItem;
}

void writeList(list* lst)
{
	list* p;
 	for (p = lst; p != NULL; p = p->prox){
	 	printf("%d ", p->id);
	}
	printf("\n");
}

//  SIMILARITY COMPUTATION
//	Functions related to the calculation of similarity based on Pearson's correlation

int n = MAXUSERS;

int total(int input_array[]){
	int i;
	int total_xy = 0.0;
	i = 0;
	while(i < n && input_array[i] > 0){
		total_xy = total_xy + input_array[i];
		i++;
	}
	return total_xy;
}

int xy(int x[], int y[]){
	int i;	
	int xy_product[n], xy_total = 0.0;

	i = 0;
	while(i < n){
		xy_product[i] = x[i] * y[i];
		xy_total = xy_total + xy_product[i];
		i++;
	}
	return xy_total; 
}

int squaredt(int input_array[]){
	int i;
	int squared[n], squared_total = 0.0;
	i = 0;
	while(i < n && input_array[i] > 0){
		squared[i] = pow(input_array[i], 2);
		squared_total = squared_total + squared[i];
		i++;
	}
	return squared_total; 
}

float corr(int x_tot, int y_tot, int xy_tot, int x2, int y2){
  float nm = ((n * xy_tot) - (x_tot * y_tot)),
        dn = sqrt(((n * x2) - (pow(x_tot, 2))) * ((n * y2) - (pow(y_tot, 2)))),
        result = nm / dn;
  
  if(dn != 0 && result <= 1.0 && result >= -1.0) 
    return  nm / dn;
  else 
    return 0.0;
}

float sim(int a1[], int a2[]){
  float correlation;
  correlation = corr(total(a1), total(a2), xy(a1, a2), squaredt(a1), squaredt(a2));
  return correlation;
}

//  INPUT
// 	Reading of input file and pre-listing list

void input(char* file, list* lst, user_items* users[]){
	char str[60];
	FILE *fp = fopen(file, "r");
	int line = 0, i;

	list* first  = lst;

  if(fp == NULL) 
  {
    perror("Error opening file");
    return;
  }
  while(fgets(str, 60, fp) != NULL) 
  {  
  	if(line != 0){
	  	int t, vItem, nUser, rating, position;
	  	char char_item[12], char_user[12], char_rating[3], char_position[3];

	  	t = 1;
	  	char_user[0] = '0';
	  	while(t < 8){
	  		char_user[t] = str[t];
	  		 t++;
	  	}
	  	char_user[8] = '\0';
	  	t = 10;
	  	char_item[0] = '0';
	  	while(t < 17){
	  		if((t-10) < 2)
	  			char_position[t-10] = str[t];

	  		char_item[t-9] = str[t];
	  		 t++;
	  	}
	  	char_position[2] = '\0';
	  	char_item[8] = '\0';
	  	t = 18;
	  	while(str[t] != ',' ){
	  		char_rating[t-18] = str[t];
	  		 t++;
	  	}
	  	char_rating[t-18] = '\0';

	  	vItem = atoi(char_item);
	  	char_item[0] = 'i';
	  	nUser = atoi(char_user);
	  	char_user[0] = 'u';
	  	rating = atoi(char_rating);
	  	position = atoi(char_position);
	  	
	  	while(lst->id < position && lst->prox != NULL){
	  		lst = lst->prox;
	  	}

	  	i = 0;
			int fnd = 0;
			while( i < lst->size && strcmp(lst->item[i].id, char_item) != 0){
				i++;
			}
      lst->item[i].users_evaluation[nUser] = rating;  // evaluation
			lst->item[i].ratings = 1;  // ratings
			strcpy(lst->item[i].id, char_item); // id
			if(i == lst->size)			
				lst->size++;

			if(!users[nUser]->size){
				users[nUser]->size = 0;
			}
			int nSize = users[nUser]->size;

			users[nUser]->item[nSize] = &lst->item[i];
			users[nUser]->size++;	  	
			lst = first;
  	}
  	line++;
  }
  fclose(fp);
	return;
}

//  OUTPUT
// 	In this function, the computation necessary to generate the recommendations

void output(char* file, list* lst, user_items* users[]){
	list* first  = lst;

	float correlation = -1.0;
	printf("UserId:ItemId,Prediction\n");

  int q = 0, r = 0, l =0, i=0;
  similar_item neighbors[2];

	lst = first;

	char str[60];
	FILE *tar;
	  		tar = fopen(file, "r");
  			q = 0;
  
  if(tar == NULL) 
  {
    perror("Error opening file");
    return;
  }
  while(fgets(str, 60, tar) != NULL) 
  {  
  	if(l != 0){
	  	int t, vItem, nUser, rating, position;
	  	char char_item[12], char_user[12], char_rating[3], char_position[3];

			// Read the file to know the evaluated item and the user for the recommendation
	  	t = 1;
	  	char_user[0] = '0';
	  	while(t < 8){
	  		char_user[t] = str[t];
	  		 t++;
	  	}
	  	char_user[8] = '\0';
	  	t = 10;
	  	char_item[0] = '0';
	  	while(t < 17){
	  		if((t-10) < 2)
	  			char_position[t-10] = str[t];

	  		char_item[t-9] = str[t];
	  		 t++;
	  	}
	  	char_position[2] = '\0';
	  	char_item[8] = '\0';
	  	t = 18;
	  	while(str[t] != ',' ){
	  		char_rating[t-18] = str[t];
	  		 t++;
	  	}
	  	char_rating[t-18] = '\0';

	  	vItem = atoi(char_item);
	  	char_item[0] = 'i';
	  	nUser = atoi(char_user);
	  	char_user[0] = 'u';
	  	rating = atoi(char_rating);
	  	position = atoi(char_position);
	  	
	  	for (i = 0; i < 2; ++i)
		  	neighbors[i].correlation = -1.0;

			lst = first;
			while(lst->id < position && lst->prox != NULL){
	  		lst = lst->prox;
	  	}
	  	t = 0;
	  	
	  	// scroll through the list looking for the item
			while( t < lst->size && strcmp(lst->item[t].id, char_item) != 0){
				t++;
			} 	
			i = 0;			
			lst = first;
			r = 0;
			int end = 0;

			int m, fd;
			if(users[nUser]->size > 1){
				i = 0;
				// user evaluates items for correlation calculation
		  	while(i < users[nUser]->size && end == 0 && users[nUser]->item[i]->users_evaluation[nUser] > 0 && neighbors[1].correlation < 0.0){
		   		correlation = sim(users[nUser]->item[i]->users_evaluation, lst->item[t].users_evaluation);
		  		m = 0;
		  		fd = 0;
		  		while(m < 2 && fd == 0){
		  			if(correlation > neighbors[m].correlation){
		  				neighbors[m].correlation = correlation;
		  				strcpy(neighbors[m].id, users[nUser]->item[i]->id);
		  				neighbors[m].rating = users[nUser]->item[i]->users_evaluation[nUser];
		  				fd = 1;
		  			}
		  			m++;
		  		}
		   		if(correlation > 0.0)
		   			end = 1;
		   		i++;
		  	}
		  }
		  float recomendation_item_based = 0.0, num = 0, dem = 0, aux = 0;
		  int recomendation = 0;

		  for(i =0; i< 2; i++){
		  	num += neighbors[i].correlation * neighbors[i].rating; 
		  	aux += neighbors[i].rating;
		  	dem += neighbors[i].correlation;
		  }
		  if(dem != 0)
		  	recomendation_item_based = num/dem;
		  else
		  	recomendation_item_based = aux / 2;
		  
		  printf("%s:%s,%f\n", char_user, char_item, recomendation_item_based);
			lst = first;
		}
		l++;
  }
  fclose(tar);
}

//  MAIN FUNCTION
// 	It only instantiates the structures and makes the calls of the functions

int main(int argc, char *argv[]){

	char str[60], user[12];

	list* LIST;
	LIST = init();

  int i;
  int num_array = MAXUSERS;
  user_items **users = malloc(num_array * sizeof *users);
  for (i = 0; i < num_array; i++)
    users[i] = malloc(num_array * sizeof *users[i]);

	for(i = 99; i >= 0; i--)
		LIST = insert(LIST, i);
	
	input(argv[1], LIST, users);
	output(argv[2], LIST, users);

	return 0;		
}
