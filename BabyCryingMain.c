#include <unistd.h>
#include <fcntl.h>
#include<sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/soundcard.h>
#include <math.h>

#define LENGTH 16
#define RATE 44100
#define SIZE 16
#define CHANNELS 2



void isCrying(char *filename,int Crying)
{       
 	
        int fd ,ret;
	char *string = "the baby is crying\n";
	fd =open(filename,O_RDWR);
	if(fd<0)
	{
		printf("open %s  error",filename);
	 }
        else 
        {
		if(Crying)
		{
			ret=write(fd,string,20);
			if(ret!=20)
			{
				printf("NOTICE:write filename  file,get %d Bytes",filename ,ret);
			 }
		 }
         }
    close(fd);
}





int main()
{
 
  unsigned char buf[LENGTH*RATE*SIZE*CHANNELS/8];
  int fd_dev_r;	/* �����豸���ļ������� */
  int status;   /* ϵͳ���õķ���ֵ */
  int i;
  int crying;
  char *file2;
  file2="/dev/uart";
  fd_dev_r = open("/dev/dsp", O_RDWR);
  if (fd_dev_r < 0) {
    perror("open of /dev/dsp failed");
    exit(1);
  }
   system("rm -f crying"); //��������ᵼ�����ݻ���
   int fd_f;
    if(( fd_f = open("./crying", O_CREAT|O_RDWR,0777))==-1)//����һ��pwm��ʽ�����ļ�
    {
        perror("cannot creat the sound file");
    }
	
    if (read(fd_dev_r, buf, sizeof(buf)) != sizeof(buf))
    {
      perror("read wrong number of bytes");
    }
    if(write(fd_f, buf, sizeof(buf))==-1)
    {
          perror("write to sound wrong!!");
    }
    close(fd_f);	
    crying=checkCrying("./crying");
    printf("checked\n");
    isCrying(file2,crying);
	memset(buf,'\0',sizeof(buf));
  close(fd_dev_r);//�ر�ֻ����ʽ������
  close(fd_f);//�ر�wave�ļ�
}
