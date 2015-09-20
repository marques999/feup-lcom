#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "kbd.h"

static int proc_args(int argc, char *argv[]);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char *argv[]);

int main(int argc, char **argv) {
	sef_startup();
	sys_enable_iop(SELF);
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
			"\t service run %s -args \"test_scan <0 - use C, 1 - use ASM>\" \n"
			"\t service run %s -args \"test_timed_scan <timeout>\" \n"
			"\t service run %s -args \"test_leds <blank>\" \n", argv[0],
			argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

	unsigned long args;
	unsigned short array_len;
	char *str;
	long num;

	if (strncmp(argv[1], "test_scan", strlen("test_scan")) == 0) {
		if (argc != 3) {
			printf("lab3::wrong no of arguments for test of kbd_test_scan() \n");
			return 1;
		}
		if ((args = parse_ulong(argv[2], 16)) == ULONG_MAX) {
			return 1;
		}
		printf("lab3::kbd_test_scan(%X)\n", (unsigned) args);
		kbd_test_scan(args);
		return 0;
	} else if (strncmp(argv[1], "test_timed_scan", strlen("test_timed_scan"))
			== 0) {
		if (argc != 3) {
			printf("lab3::wrong no of arguments for test of kbd_test_timed_scan() \n");
			return 1;
		}
		if ((args = parse_ulong(argv[2], 10)) == ULONG_MAX) {
			return 1;
		}
		printf("lab3::kbd_test_timed_scan(%d)\n", (unsigned) args);
		kbd_test_timed_scan(args);
		return 0;
	} else if (strncmp(argv[1], "test_leds <array>", strlen("test_int")) == 0) {
		if (argc < 3) {
			printf("lab3::wrong no of arguments for test of kbd_test_leds() \n");
			return 1;
		}
		array_len = argc - 2;
		int i;
		unsigned short* test_array = malloc(sizeof(unsigned short)*array_len);
		for (i = 0; i < array_len; i++) {
			test_array[i] = (unsigned short) parse_ulong(argv[i+2], 10);
		}
		printf("array_len: %d\n", array_len);
		printf("lab3::kbd_test_leds( ");
		for (i = 0; i < array_len; i++) {
			printf("%d ", test_array[i]);
		}
		printf(")\n");
		kbd_test_leds(array_len, test_array);
		return 0;
	} else {
		printf("lab3::non valid function \"%s\" to test\n", argv[1]);
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
		printf("lab3::parse_ulong: no digits were found in %s \n", str);
		return ULONG_MAX;
	}
	return val;
}
