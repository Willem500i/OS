#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {

	srand(time(NULL));

	int num = rand() % 149;

	printf("Hello World! This is Introduction to Operating Systems, Spring 2025!\n");
	printf("Student: Willem Neuefeind Lessig has %d pennies in his pocket\n", num);

	return 0;
	
}
