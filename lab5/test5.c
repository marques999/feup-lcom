#include "timer.h"
#include "sprite.h"
#include "vbe.h"
#include "video_gr.h"

#define MODE_640X480_8BPP 0x100
#define MODE_1024X768_8BPP 0x105
#define MODE_1280X1024_24BPP 0x11b

#define BASE(x) (((x) >> 12) & 0xF0000)
#define OFFSET(x) (x & 0x0FFFF)
#define REAL2VIRTUAL(addr) (vp + BASE(addr) + OFFSET(addr))

#define KEY_ESC 0x81

void* test_init(unsigned short mode, unsigned short delay) {

	// verifica se o utilizador escolheu um modo grafico valido
	if (mode < MODE_640X480_8BPP || mode > MODE_1280X1024_24BPP) {
		printf("test_init()::video mode not supported by VBE 2.0 standard.\n");
		return NULL;
	}

	// inicializa o modo grafico
	void* vp = vg_init(mode);
	if (vp == NULL) {
		printf("test_init()::couldn't switch to video mode 0x%x\n", mode);
		return NULL;
	}

	// espera "delay" segundos
	timer_wait(delay * 60);

	// regressa ao modo texto e imprime
	vg_exit();
	vg_print_mode();
	return vp;
}

int test_controller(unsigned short mode, unsigned short delay) {
	struct mem_range mr;
	vbe_info_t* vbe_p = malloc(sizeof(vbe_info_t));

	// inicializa a struct
	void* vp = (void*) vbe_get_controller_info(vbe_p);
	if (vp == NULL) {
		printf("test_controller():get_controller_info failed, couldn't get video controller information.\n");
		return ERROR;
	}

	// mostra a informacao do controlador
	printf("------------------- VESA CONTROLLER INFORMATION -------------------\n\n");
	printf("          Capabilities = 0x%x\n", vbe_p->Capabilities[0]);
	printf("              DAC Mode = %s\n", vbe_p->Capabilities[0] & BIT(0) ?
			"switchable to 8 bits per primary color" :
			"fixed width, 6 bits per primary color");
	printf("            Controller = %s\n", vbe_p->Capabilities[0] & BIT(1) ?
			"controller is not VGA compatible" :
			"controller is VGA compatible");
	printf("                RAMDAC = %s\n", vbe_p->Capabilities[0] & BIT(2) ?
			"large blocks" :
			"normal operation");
	printf("            OEM String = %s\n", REAL2VIRTUAL((phys_bytes)vbe_p->OemStringPtr));
	printf("           Vendor Name = %s\n", REAL2VIRTUAL((phys_bytes)vbe_p->OemVendorNamePtr));
	printf("          Product Name = %s\n", REAL2VIRTUAL((phys_bytes)vbe_p->OemProductNamePtr));
	printf("      Product Revision = %s\n", REAL2VIRTUAL((phys_bytes)vbe_p->OemProductRevPtr));
	printf("      Memory Installed = %d KB\n", vbe_p->TotalMemory * 64);

	// mostra uma lista com os modos de video suportados
	short* VideoModePtr = REAL2VIRTUAL(vbe_p->VideoModePtr);
	int i = 0;
	printf("\n---------------------- SUPPORTED VIDEO MODES ----------------------\n");
	while (*VideoModePtr != -1) {
		if (i % 10 == 0) {
			printf("\n");
		}
		printf("0x%x, ",*VideoModePtr++);
		i++;
	}

	// termina o programa
	printf("\nPress any key to continue...");
	return OK;
}

int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {

	// verifica se o argumento "time" � valido (diferente de zero)
	if (size == 0) {
		printf("test_square()::parameter 'size' must not be zero.\n");
		return ERROR;
	}

	// inicializa o modo grafico
	vg_init(MODE_1024X768_8BPP);

	// verifica as coordenadas do canto superior esquerdo
	if (!vg_check_bounds(x, y)) {
		vg_exit();
		printf("test_square()::screen location (%d, %d) out of bounds.\n", x, y);
		return ERROR;
	}

	// verifica as coordenadas do canto inferior direito
	if (!vg_check_bounds(size + x, size + y)) {
		vg_exit();
		printf("test_square()::screen location (%d, %d) out of bounds.\n", x + size, y + size);
		return ERROR;
	}

	// desenha o quadrado
	vg_draw_rectangle(x, y, x + size, y + size, color);

	// espera ate o utilizador pressionar 'ESC'
	while (kbd_wait_input() != KEY_ESC) {
		// do nothing
	}

	// regressa ao modo texto
	vg_exit();
	return OK;
}

int test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {

	// inicializa o modo grafico
	vg_init(MODE_1024X768_8BPP);

	// verifica as coordenadas do ponto inicial da semi-recta
	if (!vg_check_bounds(xi, yi)) {
		vg_exit();
		printf("test_line()::line start point (%d, %d) out of bounds.\n", xi, yi);
		return ERROR;
	}

	// verifica as coordenadas do ponto final da semi-recta
	if (!vg_check_bounds(xf, yf)) {
		vg_exit();
		printf("test_line()::line end point (%d, %d) out of bounds.\n", xf, yf);
		return ERROR;
	}

	// desenha a linha
	vg_draw_line(xi, yi, xf, yf, color);

	// espera ate o utilizador pressionar 'ESC'
	while (kbd_wait_input() != KEY_ESC) {
		// do nothing
	}

	// regressa ao modo texto
	vg_exit();
	return OK;
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {

	// inicializa o modo grafico
	vg_init(MODE_1024X768_8BPP);

	// verifica as coordenadas do canto superior esquerdo
	if (!vg_check_bounds(xi, yi)) {
		vg_exit();
		printf("test_xpm()::starting point (%d, %d) out of bounds.\n", xi, yi);
		return ERROR;
	}

	// valores temporarios, para serem utilizados na read_xpm()
	int width = 0;
	int height = 0;

	// converte xpm para um pixmap
	char* x = (char*) vg_read_xpm(xpm, &width, &height);

	// desenha o pixmap
	vg_draw_xpm(xi, yi, width, height, x);

	// espera ate o utilizador pressionar 'ESC'
	while (kbd_wait_input() != KEY_ESC) {
		// do nothing
	}

	// regressa ao modo texto
	vg_exit();
	return OK;
}

int test_move(unsigned short xi, unsigned short yi, char *xpm[],
		unsigned short hor, short delta, unsigned short time) {

	// verifica se o argumento "time" e valido (diferente de zero)
	if (time == 0) {
		printf("test_move()::parameter 'time' must not be zero.\n");
		return ERROR;
	}

	// verifica se o argumento "delta" e valido (diferente de zero)
	if (abs(delta) == 0) {
			printf("test_move()::parameter 'delta' must not be zero.\n");
			return ERROR;
	}
	// inicializa o modo grafico
	vg_init(MODE_1024X768_8BPP);

	// verifica as coordenadas do canto superior esquerdo
	if (!vg_check_bounds(xi, yi)) {
		vg_exit();
		printf("test_move()::screen location (%d, %d) out of bounds.\n", xi, yi);
		return ERROR;
	}

	// verifica se o deslocamento horizontal se encontra dentro dos limites
	if (hor == 1 && !vg_check_bounds(xi + delta, yi)) {
		vg_exit();
		printf("test_move()::screen location (%d, %d) out of bounds.\n", xi + delta, yi);
		return ERROR;
	}

	// verifica se o deslocamento vertical se encontra dentro dos limites
	if (hor == 0 && !vg_check_bounds(xi, yi + delta)) {
		vg_exit();
		printf("test_move()::screen location (%d, %d) out of bounds.\n", xi, yi + delta);
		return ERROR;
	}

	// inicializa sprite
	Sprite* dummy = sprite_create(xpm);

	// calcula o numero de frames por segundo
	// (numero de interrupts do timer p/segundo)
	int fps = 60; // 60 Hz

	// velocidade do sprite
	// (em pixeis/frame ou pixeis/interrupt do timer)
	float speed = (float) delta / (time * fps);
	float abs_speed = abs(speed);
	// para sprites com velocidade inferior a 1
	// numero de interrupts do timer por frame
	float ticks_per_frame = (time * fps) / abs(delta);

	// posiciona o sprite no ponto (xi,yi)
	dummy->x = xi;
	dummy->y = yi;

	// velocidade inferior a 1
	// sprite desloca-se 1 unidade na direccao dada de x em x interrupts do timer
	if (abs_speed <= 1) {
		if (hor) {
			dummy->xspeed = (speed < 0) ? -1 : 1;
			dummy->yspeed = 0;
		} else {
			dummy->xspeed = 0;
			dummy->yspeed = (speed < 0) ? -1 : 1;
		}
	// velocidade igual ou superior a 1
	// sprite desloca-se x unidades em cada interrupt do timer
	} else {
		if (hor) {
			dummy->xspeed = speed;
			dummy->yspeed = 0;
		} else {
			dummy->xspeed = 0;
			dummy->yspeed = speed;
		}
	}

	// desenha o sprite na posicao inicial
	sprite_draw(dummy);

	int i = 0;
	unsigned char key = 0;

	int ipc_status;
	message msg;

	// subscreve ambos os interrupts
	int kbd_interrupt = kbd_subscribe_int();
	int timer_interrupt = timer_subscribe_int();

	if (kbd_interrupt == ERROR || timer_interrupt == ERROR) {
		printf("test_move()::kernel call didn't return 0\n");
		return ERROR;
	}

	// processa o deslocamento enquanto o utilizador nao interromper
	while (key != KEY_ESC && i <= time * fps) {
		if (driver_receive(ANY, &msg, &ipc_status) != OK) {
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & timer_interrupt) {
					if (abs_speed < 1) {
						if (i % (int) ticks_per_frame == 0) {
							sprite_move(dummy);
						}
					} else {
						sprite_move(dummy);
					}
					i++;
				}
				if (msg.NOTIFY_ARG & kbd_interrupt) {
					key = kbd_int_handler();
				}
				break;
			}
		}
	}

	//
	timer_unsubscribe_int();
	kbd_unsubscribe_int();
	sprite_destroy(dummy);

	// regressa ao modo texto
	vg_exit();
	return OK;
}
