#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {
	struct reg86u reg86;
	mmap_t mem;

	if (lm_init() == NULL) {
		printf("vbe_get_mode_info()::lower memory initialization failed.\n");
		return ERROR;
	}

	lm_alloc(sizeof(vbe_mode_info_t), &mem);
	phys_bytes buf = mem.phys;

	reg86.u.b.ah = VBE_CALL;
	reg86.u.b.al = VBE_GET_MODE_INFO;
	reg86.u.w.es = PB2BASE(buf);
	reg86.u.w.di = PB2OFF(buf);
	reg86.u.w.cx = mode;
	reg86.u.b.intno = VIDEO_INTERRUPT;

	if (sys_int86(&reg86) != OK) {
		printf("vbe_get_mode_info()::bios call didn't return 0\n");
		return ERROR;
	}

    *vmi_p = *(vbe_mode_info_t*) mem.virtual;
	lm_free(&mem);
	return OK;
}

void* vbe_get_controller_info(vbe_info_t *vbe_p) {
	struct reg86u reg86;
	mmap_t mem;

	void* addr = lm_init();
	if (addr == NULL) {
		printf("vbe_get_controller_info()::lower memory initialization failed.\n");
		return NULL;
	}

	if (lm_alloc(sizeof(vbe_info_t), &mem) == NULL) {
		printf("vbe_get_controller_info()::lower memory initialization failed.\n");
		return NULL;
	}

	phys_bytes buf = mem.phys;

	vbe_p->VbeSignature[0] = 'V';
	vbe_p->VbeSignature[1] = 'B';
	vbe_p->VbeSignature[2] = 'E';
	vbe_p->VbeSignature[3] = '2';

	reg86.u.b.ah = VBE_CALL;
	reg86.u.b.al = VBE_GET_CONTROLLER_INFO;
	reg86.u.w.es = PB2BASE(buf);
	reg86.u.w.di = PB2OFF(buf);
	reg86.u.b.intno = VIDEO_INTERRUPT;

	if (sys_int86(&reg86) != OK) {
		printf("vbe_get_controller_info()::bios call didn't return 0\n");
		return NULL;
	}

    *vbe_p = *(vbe_info_t*) mem.virtual;
	lm_free(&mem);
	return addr;
}
