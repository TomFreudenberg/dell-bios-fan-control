/*
 * dell-bios-fan-control
 * user space utility to set control of fans by bios on Dell 9560 Laptops.
 *
 * SMM Management code from i8k. See file drivers/char/i8k.c at Linux kernel.
 *
 * Copyright (C) 2001  Massimo Dal Zotto <dz@debian.org>
 * Copyright (C) 2014  Carlos Alberto Lopez Perez <clopez@igalia.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

/*
 *
 * Most of the code was taken from https://github.com/clopez/dellfan
 *
 */

#define DISABLE_BIOS_METHOD2 0x34a3
#define ENABLE_BIOS_METHOD2  0x35a3

int init_ioperm(void)
{
    if (ioperm(0xb2, 4, 1))
        perror("ioperm:");
    if (ioperm(0x84, 4, 1))
        perror("ioperm:");
}

struct smm_regs {
    unsigned int eax;
    unsigned int ebx __attribute__ ((packed));
    unsigned int ecx __attribute__ ((packed));
    unsigned int edx __attribute__ ((packed));
    unsigned int esi __attribute__ ((packed));
    unsigned int edi __attribute__ ((packed));
};

static int i8k_smm(struct smm_regs *regs)
{
    int rc;
    int eax = regs->eax;


    asm volatile("pushq %%rax\n\t"
            "movl 0(%%rax),%%edx\n\t"
            "pushq %%rdx\n\t"
            "movl 4(%%rax),%%ebx\n\t"
            "movl 8(%%rax),%%ecx\n\t"
            "movl 12(%%rax),%%edx\n\t"
            "movl 16(%%rax),%%esi\n\t"
            "movl 20(%%rax),%%edi\n\t"
            "popq %%rax\n\t"
            "out %%al,$0xb2\n\t"
            "out %%al,$0x84\n\t"
            "xchgq %%rax,(%%rsp)\n\t"
            "movl %%ebx,4(%%rax)\n\t"
            "movl %%ecx,8(%%rax)\n\t"
            "movl %%edx,12(%%rax)\n\t"
            "movl %%esi,16(%%rax)\n\t"
            "movl %%edi,20(%%rax)\n\t"
            "popq %%rdx\n\t"
            "movl %%edx,0(%%rax)\n\t"
            "pushfq\n\t"
            "popq %%rax\n\t"
            "andl $1,%%eax\n"
            :"=a"(rc)
            :    "a"(regs)
            :    "%ebx", "%ecx", "%edx", "%esi", "%edi");

    if (rc != 0 || (regs->eax & 0xffff) == 0xffff || regs->eax == eax)
            return -1;

    return 0;
}


int send(unsigned int cmd, unsigned int arg) {

    struct smm_regs regs = { .eax = cmd, };

    regs.ebx = arg;

    i8k_smm(&regs);
    return regs.eax ;

}

int main(int argc, char **argv) {

    int enable;

    if (geteuid() != 0) {
        printf("need root privileges\n");
        exit(EXIT_FAILURE);
    }

    if (argc != 2) {
        printf ("Use: %s enable\n", argv[0]);
        printf ("\tenable = {0,1}\n");
        exit(EXIT_FAILURE);
    }

    init_ioperm();

    enable = atoi(argv[1]);
    if (enable == 1) {
        send(ENABLE_BIOS_METHOD2, 0);
        printf ("BIOS CONTROL ENABLED\n");
    }
    else
    if (enable == 0) {
        send(DISABLE_BIOS_METHOD2, 0);
        printf ("BIOS CONTROL DISABLED\n");
    }
    else  {
        printf ("Use 1 to enable bios control or 0 to disable\n");
        exit(EXIT_FAILURE);
    }

}
