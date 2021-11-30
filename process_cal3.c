/*
 * process_cal3.c
 *
 * Starter file provided to students for Assignment #3, SENG 265,
 * Fall 2021.
 *
 * Author: Jonathan Cote V00962634
 * Last Modified: 2021-11-19
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emalloc.h"
#include "ics.h"
#include "listy.h"

FILE* open_file(char*);
event_t* mem_alloc(event_t*, int);
event_t* data_collect(FILE*, int*);
event_t* expand_rep(event_t*, int*);
node_t* build_list(event_t*, node_t*, int*);
int date_builder(int, int, int);
void time_format(char*, char*, char*);
void dt_format(char*, const char*, const int);
void print_events(node_t*, int, int);
void dt_increment(char*, const char*, const int);



int main(int argc, char *argv[])
{

    int from_y = 0, from_m =0, from_d = 0;
    int to_y = 0, to_m = 0, to_d = 0;
    char *filename = NULL;
    int i;
    
    for(i = 0; i < argc; i++){
        if(strncmp(argv[i], "--start=", 8) == 0){
            sscanf(argv[i], "--start=%d/%d/%d", &from_y, &from_m, &from_d);
        }
        else if(strncmp(argv[i], "--end=", 6) == 0){
            sscanf(argv[i], "--end=%d/%d/%d", &to_y, &to_m, &to_d);
        }
        else if(strncmp(argv[i], "--file=", 7) == 0){
            filename = argv[i]+7;
        }
    }

    if(from_y == 0 || to_y == 0 || filename == NULL){
        fprintf(stderr, "usage: %s --start=yyyy/mm/dd --end=yyyy/mm/dd --file=icsfile\n", argv[0]);
        exit(1);
    }

    FILE *f_ptr;
    node_t *list = NULL;
    int size = 0; 
    event_t *ev = NULL;
    
    f_ptr = open_file(filename);
    ev = data_collect(f_ptr, &size);
    ev = expand_rep(ev, &size);
    list = build_list(ev, list, &size);


    int from_date = date_builder(from_y, from_m, from_d);
    int to_date = date_builder(to_y, to_m, to_d);
    
    print_events(list, to_date, from_date);
        

    list = free_list(list);

         
    free(ev); 
    fclose(f_ptr);
    exit(0);
}



/*
 * Function: print_events
 * Purpose: print events connected to sorted linklist to stdout
 * Input: node_t *list, int to_date, int from_date
 *
 * print format:
 *
 * <month> <day>, <year> (<day>)
 * -----------------------------
 * <start time> to <end time>: <summary> {{<location>}}
 *
 */
void print_events(node_t *list, int to_date, int from_date){
    int temp_dt = 0;
    int temp_dt_prev = 0;
    char formatted_dt[80];
    char start_time[17];
    char end_time[17];
    char start_am_pm[5];
    char end_am_pm[5];
    char temp[80];
    
    while(list != NULL){
        sscanf(list->val->dtstart, "%8dT%[0-9]", &temp_dt, temp);
        
        if(temp_dt >= from_date && temp_dt <= to_date){
            dt_format(formatted_dt, list->val->dtstart, 80);            
            time_format(start_time, start_am_pm, list->val->dtstart);
            time_format(end_time, end_am_pm, list->val->dtend);

            if(temp_dt_prev == temp_dt){
                printf("%s %s to %s %s: %s {{%s}}\n", start_time, start_am_pm,
                            end_time, end_am_pm, list->val->summary, list->val->location);                    
            }
            else if(temp_dt_prev != 0){
                printf("\n");            
                printf("%s\n", formatted_dt);            

                for(int j = 0; j <strlen(formatted_dt); j++){
                    printf("-");
                }
                printf("\n");
                printf("%s %s to %s %s: %s {{%s}}\n", start_time, start_am_pm,
                                end_time, end_am_pm, list->val->summary, list->val->location);

            }
            else{
                printf("%s\n", formatted_dt);
                
                for(int j = 0; j<strlen(formatted_dt); j++){
                    printf("-");
                }
                printf("\n");        
                printf("%s %s to %s %s: %s {{%s}}\n", start_time, start_am_pm,
                            end_time, end_am_pm, list->val->summary, list->val->location);

            }
  
            temp_dt_prev = temp_dt;
        }

        list = list->next;
    }
}


/*
 * Function: mem_alloc
 * Inputs: event_t*, int
 * Purpose: allocate memory for a event_t struct if not already allocated, else realloc with more memory
 * Return: event_t* to new memory allocation
 */
event_t* mem_alloc(event_t *ev, int size){
    event_t *temp = NULL;
    if(ev == NULL){
        temp = (event_t *)emalloc(sizeof(event_t)*size);
        return temp;
    }
    else{
        temp = (event_t *)realloc(ev, sizeof(event_t)*size);
        if(temp != NULL){
            return temp;
        }
        else{
            printf("realloc failed");
            exit(1);
        }
    }

}



/*
 * Function: build_list
 * Inputs: event_t*, node_t*, int
 * Purpose: build linked list with event_t as the val
 * Return: linked list head pointer
 */
node_t* build_list(event_t* ev, node_t* list, int *size){
    for(int i = 0; i<=*size; i++){
        list = add_inorder(list, new_node(&ev[i]));
    }
    return list;

}


/*
 * Function: open_file
 * Inputs: char*
 * Purpose: open file for given name
 * Return: FILE*
 */
FILE* open_file(char *filename){
    FILE *f_ptr = fopen(filename, "r");
    if(f_ptr == NULL){
        printf("File not found\n");
        exit(1);
    }
    return f_ptr;
}


/*
 * Function: date_builder
 * Inputs: int, int, int
 * Purpose: build a date from giving values in format YYYYMMDD
 * Return: int of date in format YYYYMMDD
 */
int date_builder(int year, int month, int day){
    int date = 0;
    year = year * 10000;
    month = month * 100;
    date = year + month + day;
    return date;
}


/*
 * Funtion: dt_format
 * Input: char*, const char*, cont int
 * Purpose: format inputted datetime from YYYYMMDDTHHMMSS -> Month, day, year (day of week)
 */
void dt_format(char *formatted_time, const char *dt_time, const int len){
    struct tm temp_time;
    time_t full_time;

    memset(&temp_time,0, sizeof(struct tm));
    sscanf(dt_time, "%4d%2d%2d", &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    full_time = mktime(&temp_time);
    strftime(formatted_time, len, "%B %d, %Y (%a)", localtime(&full_time));

}



/*
 * Function: time_format
 * Input: char*, char*, char*
 * Purpose: format inputted datetime to extract the time and if am or pm
 */
void time_format(char *formatted_time, char *am_pm, char *dt_time){
    struct tm temp_time;
    time_t full_time;
    char temp[80];

    sscanf(dt_time, "%[0-9]%[T]%2d%2d%2d", temp, temp,
                &temp_time.tm_hour, &temp_time.tm_min, &temp_time.tm_sec);

    full_time = mktime(&temp_time);
    
    strftime(formatted_time, 80, "%I:%M%p", localtime(&full_time));

    sscanf(formatted_time, "%[0-9]:%[0-9]%[A-Z]", temp, temp, am_pm);
    sscanf(formatted_time, "%[0-9:]", formatted_time);

    if(formatted_time[0] == 48){
        int a = 32;
        formatted_time[0] = a;
    }
}



/*
 * Function: data_collect
 * Input: FILE*, int*
 * Purpose: scan file line by line and parse out relevant data into event_t struct
 * return: event_t*
 */
event_t* data_collect(FILE* f_ptr, int *i){
    char line[160];
    char temp_key[80];
    char temp_value[80];
    int event_start = 0;
    event_t *ev = NULL;

    ev = mem_alloc(ev, 50);    
 
    while(fgets(line, 160, f_ptr) != NULL){        
       
        sscanf(line, "%[A-Za-z0-9]:%[A-Za-z0-9=;:,-' ]", temp_key, temp_value);
         
        if(!strcmp(temp_key, "BEGIN") && !strcmp(temp_value, "VEVENT")){
            event_start = 1;
        }
    
        if(event_start == 1){                                       

            if(!strcmp(temp_key, "DTSTART")){
                strcpy(ev[*i].dtstart, temp_value);
           }
            else if(!strcmp(temp_key, "DTEND")){
                strcpy(ev[*i].dtend, temp_value);
           }
            else if(!strcmp(temp_key, "SUMMARY")){
                strcpy(ev[*i].summary, temp_value); 
            }
            else if(!strcmp(temp_key, "LOCATION")){
                strcpy(ev[*i].location, temp_value);
            }
            else if(!strcmp(temp_key, "RRULE")){
                strcpy(ev[*i].rrule, temp_value);
            }
            else if(!strcmp(temp_key, "END")){
                event_start = 0;
                *i = *i + 1;
                if(*i%25 == 0){
                    ev = mem_alloc(ev, *i+50);
                } 
            } 
            
        }
        memset(temp_value, '\0', 80);
        memset(temp_key, '\0', 80);   
   }
    
    
    return ev;
}
  
    


/*
 * Function: dt_incremet
 * Input: char*, const char*, const int
 * Purpose: increment datetime by given number of days
 */
void dt_increment(char *after, const char *before, int const num_days){
    struct tm temp_time;
    time_t full_time;

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(before, "%4d%2d%2dT%2d%2d%2d", &temp_time.tm_year,
            &temp_time.tm_mon, &temp_time.tm_mday, &temp_time.tm_hour,
            &temp_time.tm_min, &temp_time.tm_sec);

    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    temp_time.tm_mday += num_days;

    full_time = mktime(&temp_time);
    strftime(after, 16, "%Y%m%dT%H%M%S", localtime(&full_time));   

}



/*
 * Function: expand_rep
 * Inputs: event_t*, int*
 * Purpose: expand to create new events in event_t* if the event repeats weekly
 * return: event_t*
 */
event_t* expand_rep(event_t *ev, int *size){ 
    int temp_dt_until, temp_dt_current;
    char temp_until[80];
    char temp[80];
    char temp_dt[80];
    char temp_dt_shift[80];
    int control = 0;
    int i = 1;    
    
    for(int j=0; j<*size; j++){
        if(ev[j].rrule == NULL){
            break;
        }
        sscanf(ev[j].rrule, "%s", temp);
        control = 0;  
    
        char *token;
        token = strtok(temp, ";=");

        while(token != NULL){
            
            if(!strcmp(token, "UNTIL")){
                token = strtok(NULL, "=;");
                sscanf(token, "%s", temp_until);
                control = 1;
            }
            token = strtok(NULL, ";=");
        }
        
        if(control == 1){
            sscanf(ev[j].dtstart, "%s", temp_dt);
            sscanf(temp_dt, "%8dT%[0-9]", &temp_dt_current, temp);
            sscanf(temp_until, "%8dT%[0-9]", &temp_dt_until, temp);
     
            do{
                               
                dt_increment(temp_dt_shift, temp_dt, 7);
                sscanf(temp_dt_shift, "%8dT%[0-9]", &temp_dt_current, temp);
                sscanf(temp_dt_shift, "%s", temp_dt);
                
                if(temp_dt_current < temp_dt_until){
                        
                    int z = *size + i;
                    if(i%25 == 0){
                        ev = mem_alloc(ev, z+50);
                    }                    
       
                    sscanf(temp_dt, "%s", ev[z].dtstart);
                    sscanf(ev[j].dtend, "%s", ev[z].dtend);
                    sscanf(ev[j].summary, "%[A-Za-z0-9:;=., ]", ev[z].summary);
                    sscanf(ev[j].location, "%[A-Za-z0-9:;=., ]", ev[z].location);
  
                    i++;
                }
                
            }while(temp_dt_current < temp_dt_until);

        } 

    }
    
    *size += i;

    return ev;
}









