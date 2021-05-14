/* Projeto 1 IAED: Martim Rosa Monis: ist199281*/
#include <stdio.h>
#include <string.h>

/* Defines, including constants and preset messages */

#define ON 1
#define OFF 0
#define OTHER -1
#define MAX_USERN 20
#define MAX_ACTV 20
#define MAX_DESC 50
#define MAX_USERS 50
#define MAX_ACTIVITIES 10
#define MAX_TASKS 10000
#define TO_DO "TO DO"
#define IN_PROGRESS "IN PROGRESS"
#define DONE "DONE"
#define T_OUTPUT "task %d\n"
#define L_OUTPUT "%d %s #%d %s\n"
#define N_OUPUT "%d\n"
#define U_OUTPUT "%s\n"
#define M_OUTPUT "duration=%d slack=%d\n"
#define D_OUPUT "%d %d %s\n"
#define A_OUTPUT "%s\n"
#define ERR_DUP_DESC "duplicate description\n"
#define ERR_TOO_MANY_TASKS "too many tasks\n"
#define ERR_NO_SUCH_TASK "%d: no such task\n"
#define ERR_NO_SUCH_TASK2 "no such task\n"
#define ERR_INVALID_TIME "invalid time\n"
#define ERR_TOO_MANY_USERS "too many users\n"
#define ERR_USER_EXISTS "user already exists\n"
#define ERR_NO_SUCH_USER "no such user\n"
#define ERR_TASK_STARTED "task already started\n"
#define ERR_NO_SUCH_ACTV "no such activity\n"
#define ERR_INVALID_DESC "invalid description\n"
#define ERR_DUP_ACTV "duplicate activity\n"
#define ERR_TOO_MANY_ACTV "too many activities\n"
#define ERR_INVALID_DURATION "invalid duration\n"
#define ERR_COMMAND "ERROR: this command isnt supported\n"

/* New types */

struct v_check {
	int value;
	int check;
};

struct task {
	int eta, start, id;
	char user[MAX_USERN+1], description[MAX_DESC+1], activity[MAX_ACTV+1];
};

/* Global Variables */

int time=0, taskCount=0, userCount=0, activityCount=3;
int c; /*last character read by getchar/first character in the input buffer*/
/* without it the project didnt work in mooshak */
struct task tasks[MAX_TASKS]; /*all the tasks*/
int task_order[MAX_TASKS]; /*array with the order of tasks by id*/ 
char users[MAX_USERS][MAX_USERN+1]; /*all the users*/
char activities[MAX_ACTIVITIES][MAX_ACTV+1] = { {TO_DO}, {IN_PROGRESS}, {DONE} }; /*all the activities*/

/* Auxiliary functions */

void skipSpaces() { /* Function that skips through all the empty spaces in the input buffer */
	if (c != '\n') {
		c = getchar();
	}
	while (c==' ' || c=='\t') {
		c=getchar();
	}
	return;
}

void readDescription(char string[MAX_DESC]) {
	int i;
	skipSpaces();
	for (i=0; i<MAX_DESC && c!='\n'; c=getchar(), i++) {
		string[i] = c;
	}
	string[i] = '\0';
}

void readActivity(char string[MAX_ACTV]) {
	int i;
	skipSpaces();
	for (i=0; i<MAX_ACTV && c!='\n'; c=getchar(), i++) {
		string[i] = c;
	}
    	string[i] = '\0';
}

void readUser(char newUser[MAX_USERN]) {
	int i;
	skipSpaces();
	for (i=0; i<MAX_USERN && c!='\n' && c!=' '; c=getchar(), i++) {
		newUser[i] = c;
	}
	newUser[i] = '\0';
}

int readNumber() {
	int number=0, invert=OFF;
	if (c=='-') { /*if its a negative number*/
		invert = ON;
		c = getchar();
	}
	while ( c >= '0' && c <= '9' ) {
		number = (number * 10) + (c - '0');
		c = getchar();
	}
	if (invert) {
		number *= -1;
	}
	return number;
}
/* binary sorting, used to create tasks */
int cursedsort(char key[]) {
	int low=0, high=taskCount-1, middle, comp;
	if ( taskCount == 0 ) {
		return 0;
	}
	while (low < high) {
		middle = (high+low)/2;
		comp = strcmp(key, tasks[middle].description);
		if (comp > 0) {
			low = middle+1;
		} else {
			high = middle-1;
		}
	}
	if (strcmp(key, tasks[low].description) > 0) {
		return low+1;
	} else {
		return low;
	}
}
/* pushes the tasks to make up space for the new task */
void push_and_set(int pos, int t_eta, char t_desc[]) {
	int i;
	for (i=0; i<taskCount; i++) {
		if (task_order[i] >= pos) {
			++task_order[i]; /* update the task order value of pushed tasks */
		}
	}
	task_order[taskCount] = pos;
	for (i=taskCount-1; i>=pos; i--) { /* push the tasks in front */
		tasks[i+1] = tasks[i];
	} /* create the new task */
	strcpy(tasks[pos].description, t_desc);
	strcpy(tasks[pos].activity, TO_DO);
	tasks[pos].start = 0, tasks[pos].eta = t_eta, tasks[pos].id = taskCount++;
}

int task_err_check(char activity[], char user[], int id) {
	int i, check=0;
	if (!strcmp(tasks[id].activity, activity) ) { /*Case1: If both are equal, then quit*/
		return 1;
	}
	if (!strcmp(activity, TO_DO)) { /*Case2: Any task -> to do, then quit*/
		printf(ERR_TASK_STARTED);
		return 1;
	}
	for (i=0; i<userCount && !check ; i++) { /* Verify that the user exists */
		check = ( !strcmp(users[i], user) ? 1 : 0 );
	} if (check == 0) { /* If he doesnt then give error */
		printf(ERR_NO_SUCH_USER);
		return 1;
	}
	for (i=1, check=0; i<activityCount && !check; i++) { /*Case4:Any actv->Any but TO DO or DO_NE*/
		check = ( !strcmp(activities[i], activity) ? 1 : 0 );
	} if (check == 0) { /* if no activity is found, error */
		printf(ERR_NO_SUCH_ACTV);
		return 1;
	}
	return 0;
}
/* Function that checks for invalid characters in activity name */
int actv_err_check(char activity[]) {
	int i;
	for (i=0; activity[i]!='\0'; i++) {
		if (activity[i]>='a' && activity[i]<='z') {
			printf(ERR_INVALID_DESC);
			return 1;
		}
	}
	return 0;
}
/* Auxiliary function to listActivity, finds and prints tasks, by start or alphabetical order */
void find_and_print(struct v_check box[], int c) {
	int i, less, id, checked=0;
	while (checked < c) { /*while checked tasks are lower than total of tasks selected */
		i=0; /* find the first unchecked task */
		while ( box[i].check == 1 ) {
			i++;
		}
		less = box[i].value, id=i;
		for (i=0; i<c; i++) { /* if current task started before, then update less */
			if ( box[i].check == 0 && tasks[box[i].value].start < tasks[less].start ) {
				less = box[i].value, id=i;
			} else if ( box[i].check == 0 && tasks[box[i].value].start == tasks[less].start ) {
				if (box[i].value < less) { /* if started at the same time and alpha. before */
					less = box[i].value, id=i; /* update less */
				}
			}
		} /* print the selected task */
		printf(D_OUPUT, tasks[less].id+1, tasks[less].start, tasks[less].description);
		++checked;
		box[id].check=1;
	}

}

/* Command functions */

void addTask() {
	int i, t_eta; /* t_eta is the duration */
	char t_desc[MAX_DESC+1];
	if (taskCount >= MAX_TASKS) { /* if we've reached maximum task number */
		printf(ERR_TOO_MANY_TASKS);
		return;
	}
	skipSpaces();
	t_eta = readNumber();

	readDescription(t_desc);
	for (i=0; i<taskCount; i++) {
		if (!(strcmp(tasks[i].description, t_desc))) { /* theres task with same description */
			printf(ERR_DUP_DESC);
			return;
		}
	}
	if (t_eta <= 0) { /* if the duration is null or negative */
		printf(ERR_INVALID_DURATION);
		return;
	}
	push_and_set(cursedsort(t_desc), t_eta, t_desc); /* create task into ordered array */
	printf(T_OUTPUT, taskCount);
}


/* lists all the tasks or the tasks with given ids */
void listTasks() {
	int num, i=0, index, v[MAX_TASKS];

	skipSpaces();
	while (c!='\n') {
		v[i] = readNumber();
		++i;
		skipSpaces();
	}
	num=i;
	if (num==0) { /* if there were given no ids, print all */
		for (i=0; i<taskCount; i++) {
			printf(L_OUTPUT, tasks[i].id+1, tasks[i].activity, tasks[i].eta, tasks[i].description);
		}
	} else { /* print all the ids given */
		for (i=0; i<num; i++) {
			if ( v[i] > taskCount || v[i] <= 0 ) { /* if task with given id doesnt exist */
				printf(ERR_NO_SUCH_TASK, v[i]);
			} else {
				index = task_order[v[i]-1];
				printf(L_OUTPUT, v[i], tasks[index].activity, tasks[index].eta, tasks[index].description);
			}
		}
	}
}

void clock() {
	int duration;
	skipSpaces();
	duration = readNumber();
	if (duration < 0) { /* passage of time can't be a negative value */
		printf(ERR_INVALID_TIME);
	} else {
		time += duration; /* add the duration of passed time to the clock */
		printf(N_OUPUT, time); /* if duration is 0 then time hasn't passed, prints current time */
	}
}

void listUsers() {
	int i=0;
	char newUser[MAX_USERN+1];
	readUser(newUser);
	if (strlen(newUser)>0) {
		for (i=0; i<userCount; i++) {
			if (!(strcmp(users[i], newUser))) { /* if user already exists */
				printf(ERR_USER_EXISTS);
				return;
		  	}
		}
		if (userCount == MAX_USERS) { /* if we're at the maximum user count */
			printf(ERR_TOO_MANY_USERS);
			return;
		}


		strcpy(users[userCount], newUser); /* otherwise, create user */
		++userCount;
	} else {  /* if no user was given to make, print all existing users */
		for (i=0; i<userCount ; i++) {
			printf(U_OUTPUT, users[i]);
		}
	}
}

void updateTask() {
	int id, duration;
	char user[MAX_USERN+1], activity[MAX_ACTV+1];

	skipSpaces();
	id = readNumber();
	if (id-1 >= taskCount || id < 1) { /* if task doesnt exist */
		printf(ERR_NO_SUCH_TASK2);
		return;
	}
	readUser(user);
	readActivity(activity);
 	id=task_order[id-1];

	if (!task_err_check(activity, user, id)) {
		if (!strcmp(tasks[id].activity, TO_DO)) { /*If task leaves TO DO, update start time */
			tasks[id].start = time;
		}
		if (!strcmp(activity, DONE)) { /* if task reaches DONE */
			duration = (time - tasks[id].start);
			printf(M_OUTPUT, duration, (duration - tasks[id].eta));
		}
		strcpy(tasks[id].activity, activity); /* otherwise update its activity only */
	}
}

void listActivity() {
	int i, c=0, less=0;
	struct v_check box[MAX_TASKS];
	char t_act[MAX_ACTV+1];

	readActivity(t_act);
	for (i=0; i < activityCount && !less; i++) { /* verify that given activity exists */
		if  (!(strcmp(t_act, activities[i]))) {
			less=1; /* if so, set 'less' to 1 */
		}
	}
	if (less) { /* if condition above is checked, proceed */
		for (i = 0; i<taskCount; i++) { /* save all tasks that belong to given activity */
			if (!strcmp(tasks[i].activity, t_act)) {
				box[c].value = i; /* save its id */
				box[c++].check = 0; /* set corresponding checkmark to 0 */
			}
		}
		find_and_print(box, c); /* all the hardwork */
	} else {
		printf(ERR_NO_SUCH_ACTV); /* in case the activity doesnt exist */
	}
}

void manageActivities() {
	int i;
	char t_act[MAX_ACTV+1];
	readActivity(t_act);
	if (!actv_err_check(t_act)) { /* in case there are no errors with the input */
		if (strlen(t_act)>0) { /* if there was given an activity */
			for (i=0; i<activityCount; i++) { /* check if it already exists */
				if (!(strcmp(t_act, activities[i]))) {
					printf(ERR_DUP_ACTV);
					return;
				}
			}
			if (activityCount == MAX_ACTIVITIES) { /* if we already reached maximum task count */
				printf(ERR_TOO_MANY_ACTV);
				return;
			} else { /* otherwise, create the new activity */
				strcpy(activities[activityCount], t_act);
				++activityCount;
			}
		} else { /* if no activity was given, print all existing ones by creation order */
			for (i=0; i<activityCount; i++) {
				printf(A_OUTPUT, activities[i]);
			}
		}
	}
}

/* a main() :pray: :pavaobless: */

int main() {
	c = getchar(); /* get the first input */
	while (c!='q' && c!=EOF) {
		switch (c) {
			case 't':
				addTask();
				break;
			case 'l':
				listTasks();
				break;
			case 'n':
				clock();
				break;
			case 'u':
				listUsers();
				break;
			case 'm':
				updateTask();
				break;
			case 'd':
				listActivity();
	  			break;
			case 'a':
	  			manageActivities();
	    	}
		while (c!='\n') {
			c = getchar(); /* clear the input buffer(until it finds a newline char, which is then deleted)*/
		}
		c=getchar(); /* get new input line from user */
	}
	return 0;
}

