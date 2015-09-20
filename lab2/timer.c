#include <minix/syslib.h>
#include <minix/drivers.h>
#include <time.h>
#include "i8254.h"

#define BYTE_MASK 0xff 	// 0xff permite seleccionar apenas os 8 bits mais significativos de um número
#define TIMER0_CLOCK 60 // frequência padrão das interrupções do Timer 0 (60 Hz)

int count; // contador de interrupções
int id = 4; // variável de entrada/saída para as funções de interrupções

int timer_set_square(unsigned long timer, unsigned long freq) {
	unsigned char ADDR;	// port do timer a seleccionar
	unsigned char FLAGS = timer_get_conf(timer); // l� registo de status do timer
	unsigned int TIMER_DIV = TIMER_FREQ / freq; // o resultado representa o valor de contagem

	// TIMER_DIV não pode ultrapassar USHRT_MAX (evita overflow)
	// valor de contagem é um inteiro sem sinal de 16 bits, valor máximo: 0xffff
	if (TIMER_DIV > USHRT_MAX) {
		printf("test failed: frequency is too low!\n");
		return 1;
	}

	// extrai o valor do BIT 0
	FLAGS &= TIMER_BCD;

	// selecciona timer e respetiva port
	switch (timer) {
	case 0:
		FLAGS |= TIMER_SEL0;
		ADDR = TIMER_0;
		break;
	case 1:
		FLAGS |= TIMER_SEL1;
		ADDR = TIMER_1;
		break;
	case 2:
		FLAGS |= TIMER_SEL2;
		ADDR = TIMER_2;
		break;
	default:
		// timer inválido
		printf("ERROR: invalid timer selected.");
		return 1;
	}

	FLAGS |= TIMER_SQR_WAVE;	// selecciona o modo de operação (Mode 3)
	FLAGS |= TIMER_LSB_MSB;	// selecciona o modo de acesso (MSB após LSB)

	// envia as flags para a command word
	if (sys_outb(TIMER_CTRL, FLAGS) != 0) {
		printf("test failed: kernel call didn't return 0\n");
		return 1;
	}
	// coloca no timer seleccionado o LSB do valor de contagem
	if (sys_outb(ADDR, TIMER_DIV & BYTE_MASK) != 0) {
		printf("test failed: kernel call didn't return 0\n");
		return 1;
	}
	// coloca no timer seleccionado o MSB do valor de contagem
	if (sys_outb(ADDR, (TIMER_DIV >> 8) & BYTE_MASK) != 0) {
		printf("test failed: kernel call didn't return 0\n");
		return 1;
	}
	return 0;
}

int timer_subscribe_int(void) {
	// subscreve os interrupts do timer 0
	int BIT_ORDER = id;
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &id) != 0) {
		return -1;
	}
	if (sys_irqenable(&id) != 0) {
		return -1;
	}
	return BIT_ORDER;
}

int timer_unsubscribe_int() {
	 // desativa as interrup��es e remove a policy associadas ao timer 0
	if (sys_irqdisable(&id) != 0) {
		return 1;
	}
	if (sys_irqrmpolicy(&id) != 0) {
		return 1;
	}
	return 0;
}

void timer_int_handler() {
	// incrementa o contador
	count++;
}

int timer_get_conf(unsigned long timer, unsigned char *st) {
	unsigned char FLAGS = 0; // bits do command word
	unsigned char ADDR; 	// port do timer a seleccionar

	// selecciona timer e respetiva port
	switch (timer) {
	case 0:
		FLAGS |= TIMER_RB_SEL(0);
		ADDR = TIMER_0;
		break;
	case 1:
		FLAGS |= TIMER_RB_SEL(1);
		ADDR = TIMER_1;
		break;
	case 2:
		FLAGS |= TIMER_RB_SEL(2);
		ADDR = TIMER_2;
		break;
	default:
		// timer inválido
		printf("ERROR: invalid timer selected.");
		return 1;
	}

	FLAGS |= TIMER_RB_CMD; // selecciona o modo readback do timer
	FLAGS |= TIMER_RB_COUNT_; // nega o bit COUNT (apenas interessa ler o valor de STATUS)

	// envia as flags para a command word
	if (sys_outb(TIMER_CTRL, FLAGS) != 0) {
		printf("test failed: kernel call didn't return 0\n");
		return 1;
	}
	// lê o registo de STATUS do timer seleccionado
	if (sys_inb(ADDR, (unsigned long*) st) != 0) {
		printf("test failed: kernel call didn't return 0\n");
		return 1;
	}
	return 0;
}

int timer_display_conf(unsigned char conf) {
	// Bit 7 - Output
	printf("Output: %s\n", conf & BIT(7) ? "1 (enabled)" : "0 (disabled)");

	// Bit 6 - Null Count
	printf("Null count: %s\n", conf & BIT(6) ? "Yes" : "No");

	// Bit 5,4 - Modo de acesso
	switch (conf & (BIT(5) | BIT(4))) {
	case TIMER_LSB:
		printf("Type of access: LSB only\n");
		break;
	case TIMER_MSB:
		printf("Type of access: MSB only\n");
		break;
	case TIMER_LSB_MSB:
		printf("Type of access: LSB followed by MSB\n");
		break;
	default:
		printf("Type of access: invalid\n");
	}

	// Bit 3,2,1 - Modo de operação
	switch (conf & (BIT(3) | BIT(2) | BIT(1))) {
	case TIMER_SQR_WAVE:
		printf("Operating mode: Mode 3 (square wave generator)\n");
		break;
	case TIMER_RATE_GEN:
		printf("Operating mode: Mode 2 (rate generator)\n");
		break;
	default:
		printf("Operating mode: invalid\n");
	}

	// Bit 0 - modo de contagem
	printf("Counting mode: %s\n", conf & BIT(0) ? "BCD" : "Binary");
	return 0;
}

int timer_test_square(unsigned long freq) {
	return timer_set_square(0, freq);
}

int timer_test_int(unsigned long time) {
	int ipc_status;
	int irq_set;
	int r;
	message msg;
	count = 0;
	irq_set = timer_subscribe_int();
	if (irq_set == -1) {
		printf("test failed: subscribe timer didn't return 0\n");
		return 1;
	}
	while (count < time * 60) {
		/* Get a request message. */
		if (r = driver_receive(ANY, &msg, &ipc_status) != 0) {
			/* received notification */
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & BIT(irq_set)) {
					/* subscribed interrupt, process it */
					timer_int_handler();
					// verifica se passou um segundo (a cada 60 interrupções)
					if (count % TIMER0_CLOCK == 0) {
						printf("Another second has passed...\n");
					}
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	timer_unsubscribe_int();
	return 0;
}

int timer_test_config(unsigned long timer) {
	// verifica se o timer pedido existe
	if (timer == 0 || timer == 1 || timer == 2) {
		unsigned char conf = 0; // guarda resultado da chamada da função
		timer_get_conf(timer, &conf);
		timer_display_conf(conf);
		return 0;
	}
	printf("test failed: invalid timer selected\n");
	return 1;
}
