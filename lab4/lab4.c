#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "mouse.h"

static int proc_args(int argc, char *argv[]);
static long parse_long(char *str, int base);
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
			"\t service run %s -args \"test_packet <packet_count>\" \n"
			"\t service run %s -args \"test_asynch <timeout>\" \n"
			"\t service run %s -args \"test_config <blank>\" \n"
			"\t service run %s -args \"test_gesture <length, tolerance>\" \n",
			argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

	unsigned long args;
	short aux;
	char *str;
	long num;

	if (strncmp(argv[1], "test_packet", strlen("test_packet")) == 0) {
		if (argc != 3) {
			printf("lab4::wrong no of arguments for test_packet(unsigned short) \n");
			return 1;
		}
		if ((args = parse_ulong(argv[2], 10)) == ULONG_MAX) {
			return 1;
		}
		printf("lab4::test_packet(%d)\n", (unsigned) args);
		return test_packet(args);
	} else if (strncmp(argv[1], "test_async", strlen("test_async")) == 0) {
		if (argc != 3) {
			printf("lab4::wrong no of arguments for test_async(unsigned short)\n");
			return 1;
		}
		if ((args = parse_ulong(argv[2], 10)) == ULONG_MAX) {
			return 1;
		}
		printf("lab4::test_async(%d)\n", (unsigned) args);
		return test_async(args);
	} else if (strncmp(argv[1], "test_config", strlen("test_config")) == 0) {
		if (argc != 2) {
			printf("lab4::wrong no of arguments for test of test_config(void)\n");
			return 1;
		}
		printf("lab4::test_config()");
		test_config();
		return 0;
	} else if (strncmp(argv[1], "test_gesture", strlen("test_gesture")) == 0) {
		if (argc != 4) {
			printf("lab4::wrong no of arguments for test_gesture(unsigned short, short)\n");
			return 1;
		}
		if ((args = parse_ulong(argv[2], 10)) == ULONG_MAX) {
			return 1;
		}
		if ((aux = parse_long(argv[3], 10)) == LONG_MAX) {
			return 1;
		}
		printf("lab4::test_gesture(%d, %d)\n", args, aux);
		test_gesture(args, aux);
		return 0;
	} else {
		printf("lab4::non valid function \"%s\" to test\n", argv[1]);
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
		printf("lab4::parse_ulong(): no digits were found in %s \n", str);
		return ULONG_MAX;
	}
	return val;
}

static long parse_long(char *str, int base) {
  char *endptr;
  unsigned long val;
  val = strtol(str, &endptr, base);
  if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
	  perror("strtol");
	  return LONG_MAX;
  }
  if (endptr == str) {
	  printf("lab4::parse_long(): no digits were found in %s \n", str);
	  return LONG_MAX;
  }
  return val;
}
