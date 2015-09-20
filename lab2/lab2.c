#include <minix/syslib.h>
#include <minix/drivers.h>
#include "timer.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {
	sef_startup();
	if (argc == 1) {
		print_usage(argv);
		return 0;
	} else {
		proc_args(argc, argv);
	}
	return 0;

}

static void print_usage(char *argv[]) {
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"test_square <freq>\" \n"
			"\t service run %s -args \"test_config <timer>\" \n"
			"\t service run %s -args \"test_int <time>\" \n", argv[0], argv[0],
			argv[0]);
}

static int proc_args(int argc, char *argv[]) {

	unsigned long freq, timer, time;
	char *str;
	long num;

	if (strncmp(argv[1], "test_square", strlen("test_square")) == 0) {
		if (argc != 3) {
			printf(
					"lab2: wrong no of arguments for test of timer_test_square() \n");
			return 1;
		}
		if ((freq = parse_ulong(argv[2], 10)) == ULONG_MAX)
			return 1;
		printf("lab2::timer_test_square(%d)\n", (unsigned) freq);
		timer_test_square(freq);
		return 0;
	} else if (strncmp(argv[1], "test_config", strlen("test_config")) == 0) {
		if (argc != 3) {
			printf(
					"lab2: wrong no of arguments for test of timer_test_config() \n");
			return 1;
		}
		if ((timer = parse_ulong(argv[2], 16)) == ULONG_MAX)
			return 1;
		printf("lab2::timer_test_config(0x%X)\n", (unsigned) timer);
		timer_test_config(timer);
		return 0;
	} else if (strncmp(argv[1], "test_int", strlen("test_int")) == 0) {
		if (argc != 3) {
			printf(
					"lab2: wrong no of arguments for test of timer_set_square() \n");
			return 1;
		}
		if ((time = parse_ulong(argv[2], 10)) == ULONG_MAX)
			return 1;
		printf("lab2::timer_set_square(%d)\n", (unsigned) time);
		timer_test_int(time);
		return 0;
	} else {
		printf("lab2: non valid function \"%s\" to test\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base) {
	char *endptr;
	unsigned long val;
	val = strtoul(str, &endptr, base);
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtol");
		return ULONG_MAX;
	}
	if (endptr == str) {
		printf("lab2: parse_ulong: no digits were found in %s \n", str);
		return ULONG_MAX;
	}
	return val;
}
