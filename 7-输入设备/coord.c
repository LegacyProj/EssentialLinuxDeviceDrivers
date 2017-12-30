#include <fcntl.h>

int main()
{
	int sim_fd;
	int x, y;
	char buffer[10];
	
	//
	sim_fd = open("/sys/devices/platform/vms/coordinates", O_RDWR);
	if(sim_fd < 0)
	{
		perror("couldn't open vms coordinates file\n");
		exit(-1);
	}
	
	while(1)
	{
		
		x = random() % 20;
		y = random() % 20;
		if (x % 2) x = -x;
		if (y % 2) y = -y;
			
		sprintf(buffer, "%d %d %d", x, y, 0);
		write(sim_fd, buffer, strlen(buffer));
		fsync(sim_fd);
		sleep(1);
	}
	
	close(sim_fd);
}