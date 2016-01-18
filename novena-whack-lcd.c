#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <linux/input.h>

#define IPUMEM_OFFSET 0x2600000
static volatile uint32_t *ipumem;

static void whack_lcd(void) {
        static int event_count = 0;

	fprintf(stderr, "Run %d: Whacking LCD\n", event_count++);
	ipumem[0] = 0;
	ipumem[0] = 0x660;
}

static int open_by_name(const char *name) {

        DIR *devinput = opendir("/dev/input");
        char devname[256];
        struct dirent *fil;
        int fd = -1;
        
        while ((fil = readdir(devinput)) != NULL) {
                char fullname[256];

                snprintf(fullname, sizeof(fullname)-1, "%s/%s",
                                "/dev/input",fil->d_name);
                fd = open(fullname, O_RDONLY);
                if (fd == -1)
                        continue;

                ioctl(fd, EVIOCGNAME(sizeof(devname)), devname);

                if (!strcmp(name, devname))
                        break;
                close(fd);
        }

        closedir(devinput);

        return fd;
}

int main(int argc, char **argv) {
        int fd = open_by_name("gpio-keys");
        struct input_event event;
	int memfd = open("/dev/mem", O_RDWR | O_SYNC);

	if (memfd == -1) {
		perror("Unable to open /dev/mem");
		exit(1);
	}

        if (-1 == fd) {
                perror("Unable to open gpio-keys");
                return 1;
        }

	ipumem = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, IPUMEM_OFFSET);
	if (ipumem == (void *) -1) {
		perror("Unable to mmap IPUMEM");
		exit(1);
	}

        while (read(fd, &event, sizeof(event)) == sizeof(event)) {
		if ((event.type == EV_KEY)
//		 && (event.code == KEY_DISPLAYTOGGLE)
		 && (event.value)) {
			whack_lcd();
		}
        }

        close(fd);

        return 0;
}
