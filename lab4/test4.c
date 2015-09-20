#include "mouse.h"

#define TIMER_HOOK_ID 1

int test_packet(unsigned short cnt) {
	int ipc_status;
	unsigned short i = 0;
	message msg;
	packet_t state;
	state.synced = 0;
	state.count = 0;
	short irq_set = mouse_subscribe_int();
	if (irq_set == ERROR) {
		printf("test_packet()::kernel call didn't return 0\n");
		return ERROR;
	}
	while (i < cnt * 3) {
		if (driver_receive(ANY, &msg, &ipc_status) != OK) {
			printf("test_packet()::driver_receive failed\n");
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set) {
					mouse_int_handler(&state);
					if (state.count == 3) {
						state.count = 0;
						mouse_get_conf(state.packet);
					}
					i++;
				}
				break;
			}
		}
	}
	mouse_unsubscribe_int();
	return 0;
}

int test_async(unsigned short idle_time) {
	int ipc_status;
	message msg;
	packet_t state;
	int count = 0;
	state.synced = 0;
	state.count = 0;
	short timer_mask = timer_subscribe_int();
	short mouse_mask = mouse_subscribe_int();
	if (mouse_mask == ERROR) {
		printf("test_packet()::kernel call didn't return 0\n");
		return ERROR;
	}
	while (count < (idle_time * 60)) {
		if (driver_receive(ANY, &msg, &ipc_status) != OK) {
			printf("test_packet()::driver_receive failed\n");
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: {
				if (msg.NOTIFY_ARG & mouse_mask) {
					count = 0;
					mouse_int_handler(&state);
					if (state.count == 3) {
						state.count = 0;
						mouse_get_conf(state.packet);
					}
				}
				if (msg.NOTIFY_ARG & timer_mask) {
					count++;
					if (count >= idle_time * 60) {
						printf("test_async()::time out\n");
					}
				}
			}
			}
		}
	}
	timer_unsubscribe_int();
	mouse_unsubscribe_int();
	return 0;
}

int test_config(void) {
	unsigned char status = 0;
	short irq_set = mouse_subscribe_int();
	if (irq_set == ERROR) {
		printf("test_packet()::kernel call didn't return 0\n");
		return ERROR;
	}
	if (mouse_write(STREAM_OFF) != ACK) {
		return ERROR;
	}
	if (mouse_write(STATUS_REQUEST) != ACK) {
		return ERROR;
	}
	if (mouse_read(&status) != OK) {
		return ERROR;
	}
	printf("\n");
	printf("  ------- BYTE 1 : 0x%02X  --------\n", status);
	printf("  Operating Mode : %s\n",
			status & BIT(6) ? "Remote mode" : "Stream mode");
	printf("    Scaling Mode : %s\n", status & BIT(4) ? "2:1" : "1:1");
	printf("  Data Reporting : %s\n", status & BIT(5) ? "Enabled" : "Disabled");
	printf("     Left Button : %s\n", status & BIT(0) ? "Pressed" : "Released");
	printf("   Middle Button : %s\n", status & BIT(2) ? "Pressed" : "Released");
	printf("    Right Button : %s\n", status & BIT(1) ? "Pressed" : "Released");
	if (mouse_read(&status) != OK) {
		return ERROR;
	}
	printf("  ------- BYTE 2 : 0x%02X  --------\n", status);
	printf("      Resolution : %d counts per mm\n", BIT(status));
	if (mouse_read(&status) != OK) {

		return ERROR;
	}
	printf("  ------- BYTE 3 : 0x%02X  --------\n", status);
	printf("     Sample Rate : %d samples per second\n", status);
	if (mouse_write(STREAM_ON) != ACK) {
		printf("test_config()::mouse_write didn't return ACK\n");
		return ERROR;
	}
	mouse_unsubscribe_int();
	return OK;
}

int test_gesture(short length, unsigned short tolerance) {
	int ipc_status;
	unsigned short i = 0;
	message msg;
	packet_t packet;
	ev_type_t state = -1;

	packet.synced = 0;
	packet.accu = 0;
	packet.count = 0;
	packet.tolerance = tolerance;

	short irq_set = mouse_subscribe_int();
	if (irq_set == ERROR) {
		printf("test_packet()::kernel call didn't return 0\n");
		return ERROR;
	}

	while (length > 0 ? packet.accu <= length : packet.accu >= length) {
		if (driver_receive(ANY, &msg, &ipc_status) != OK) {
			printf("test_packet()::driver_receive failed\n");
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set) {
					mouse_int_handler(&packet);
					if (packet.count == 3) {
						packet.count = 0;
						mouse_get_conf(packet.packet);
						mouse_check_line(&state, &packet);
					}
				}
				break;
			}
		}
	}
	printf("test_gesture()::horizontal line recognized!\n");
	mouse_unsubscribe_int();
	return 0;
}
