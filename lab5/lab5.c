#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include "pixmap.h"
#include "test5.h"

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
	printf(
			"Usage: one of the following:\n"
					"\t service run %s -args \"test_init <mode, delay>\" \n"
					"\t service run %s -args \"test_controller <blank>\" \n"
					"\t service run %s -args \"test_square <x, y, size, color>\" \n"
					"\t service run %s -args \"test_line <x0, y0, x1, 1, color>\" \n"
					"\t service run %s -args \"test_xpm <xi, yi, xpm>\" \n"
					"\t service run %s -args \"test_move <xi, yi, xpm, hor, delta, time>\" \n",
			argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

	if (strncmp(argv[1], "test_init", strlen("test_init")) == 0) {
		unsigned short mode;
		unsigned short delay;
		if (argc != 4) {
			printf("lab5::wrong no of arguments for test_init() \n");
			return 1;
		}
		if ((mode = parse_ulong(argv[2], 16)) == ULONG_MAX) {
			return 1;
		}
		if ((delay = parse_ulong(argv[3], 10)) == ULONG_MAX) {
			return 1;
		}
		printf("lab5::test_init(0x%X, %d)\n", mode, delay);
		test_init(mode, delay);
	} else if (strncmp(argv[1], "test_controller", strlen("test_controller")) == 0) {
		if (argc != 2) {
			printf("lab5::wrong no of arguments for test_controller()\n");
			return 1;
		}
		printf("lab5::test_controller()\n");
		return test_controller();
	} else if (strncmp(argv[1], "test_square", strlen("test_square")) == 0) {
		unsigned short size;
		unsigned short color;
		unsigned short x;
		unsigned short y;
		if (argc != 6) {
			printf("lab5::wrong no of arguments for test of test_square()\n");
			return 1;
		}
		if ((x = parse_ulong(argv[2], 10)) == ULONG_MAX) {
			return 1;
		}
		if ((y = parse_ulong(argv[3], 10)) == ULONG_MAX) {
			return 1;
		}
		if ((size = parse_ulong(argv[4], 10)) == ULONG_MAX) { // size
			return 1;
		}
		if ((color = parse_ulong(argv[5], 16)) == ULONG_MAX) { // color
			return 1;
		}
		printf("lab5::test_square(%d, %d, %d, 0x%X)\n", x, y, size, color);
		return test_square(x, y, size, color);

	} else if (strncmp(argv[1], "test_line", strlen("test_line")) == 0) {
		unsigned short color;
		unsigned short xi;
		unsigned short yi;
		unsigned short xf;
		unsigned short yf;
		if (argc != 7) {
			printf("lab5::wrong no of arguments for test_line()\n");
			return 1;
		}
		if ((xi = parse_ulong(argv[2], 10)) == ULONG_MAX) {
			return 1;
		}
		if ((yi = parse_ulong(argv[3], 10)) == ULONG_MAX) {
			return 1;
		}
		if ((xf = parse_ulong(argv[4], 10)) == ULONG_MAX) {
			return 1;
		}
		if ((yf = parse_ulong(argv[5], 10)) == ULONG_MAX) {
			return 1;
		}
		if ((color = parse_ulong(argv[6], 16)) == ULONG_MAX) {
			return 1;
		}
		printf("lab5::test_line(%d, %d, %d, %d, 0x%X)\n", xi, yi, xf, yf, color);
		return test_line(xi, yi, xf, yf, color);

	} else if (strncmp(argv[1], "test_xpm", strlen("test_xpm")) == 0) {
		unsigned short x;
		unsigned short y;
		char** array;

		if (argc != 5) {
			printf("lab5::wrong no of arguments for test_xpm()\n");
			return 1;
		}
		if ((x = parse_ulong(argv[2], 10)) == ULONG_MAX) {
			return 1;
		}
		if ((y = parse_ulong(argv[3], 10)) == ULONG_MAX) {
			return 1;
		}
		if (strncmp(argv[4], "pic1", strlen("pic1")) == 0) {
			array = pic1;
		} else if (strncmp(argv[4], "pic2", strlen("pic2")) == 0) {
			array = pic2;
		} else if (strncmp(argv[4], "pic3", strlen("pic3")) == 0) {
			array = pic3;
		} else if (strncmp(argv[4], "cross", strlen("cross")) == 0) {
			array = cross;
		} else if (strncmp(argv[4], "penguin", strlen("penguin")) == 0) {
			array = penguin;
		} else {
			printf("lab5::array not found\n");
			return 1;
		}
		printf("lab5::test_xpm(%d, %d, %s)\n", x, y, argv[4]);
		return test_xpm(x, y, array);
	} else if (strncmp(argv[1], "test_move", strlen("test_move")) == 0) {
		unsigned short x;
		unsigned short y;
		unsigned short hor;
		unsigned short time;
		short delta;
		char** array;

		if (argc != 8) {
			printf("lab5::wrong no of arguments for test_move()\n");
			return 1;
		}
		if ((x = parse_ulong(argv[2], 10)) == ULONG_MAX) {
			return 1;
		}
		if ((y = parse_ulong(argv[3], 10)) == ULONG_MAX) {
			return 1;
		}
		if ((hor = parse_ulong(argv[5], 10)) == ULONG_MAX) {
			return 1;
		}
		if ((delta = parse_long(argv[6], 10)) == ULONG_MAX) {
			return 1;
		}
		if ((time = parse_ulong(argv[7], 10)) == ULONG_MAX) {
			return 1;
		}
		printf("lab5::test_move(%d, %d, *xpm, %s, %d, %d)\n", x, y,
				hor ? "Horizontal" : "Vertical", delta, time);
		if (strncmp(argv[4], "pic1", strlen("pic1")) == 0) {
			array = pic1;
		} else if (strncmp(argv[4], "pic2", strlen("pic2")) == 0) {
			array = pic2;
		} else if (strncmp(argv[4], "pic3", strlen("pic3")) == 0) {
			array = pic3;
		} else if (strncmp(argv[4], "cross", strlen("cross")) == 0) {
			array = cross;
		} else if (strncmp(argv[4], "penguin", strlen("penguin")) == 0) {
			array = penguin;
		} else {
			printf("lab5::array not found\n");
			return 1;
		}
		return test_move(x, y, array, hor, delta, time);
	} else {
		printf("lab5::non valid function \"%s\" to test\n", argv[1]);
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
		printf("lab5::parse_ulong(): no digits were found in %s \n", str);
		return ULONG_MAX;
	}
	return val;
}

static long parse_long(char *str, int base) {
	char *endptr;
	unsigned long val;
	val = strtol(str, &endptr, base);
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		return LONG_MAX;
	}
	if (endptr == str) {
		printf("lab5::parse_long(): no digits were found in %s \n", str);
		return LONG_MAX;
	}
	return val;
}
