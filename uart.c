#include <linux/module.h>
#include<linux/fs.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <asm/arch-s3c2410/regs-serial.h>
#include <asm/io.h>

//MODULE_AUTHOR("uart");
//MODULE_DESCRIPTION("serial module test");
//MODULE_LICENSE("GPL");

#define iobase           S3C24XX_VA_UART1 

#define UART_ULCON	 (iobase + 0x00)
#define UART_UCON	 (iobase + 0x04)
#define UART_UTRSTAT  	 (iobase + 0x10)
#define UART_UTXH	 (iobase + 0x20)
#define UART_URXH	 (iobase + 0x24)
#define UART_UBRDIV	 (iobase + 0x28)
//#define UART_MAJOR  240
//size_t uart_write ();
//size_t uart_read ();
static struct class *uart_class;
static struct class_device *uart_class_dev;
static int UART_MAJOR;
static ssize_t uart_write (struct file *filp,const char __user *buf,size_t count, loff_t *f_pos)
{
	struct inode *inode = filp->f_dentry->d_inode;
	int major,minor;
	char wbuf[count];
	int str = 0;
	int state;
        int cp_flag;
	minor = MINOR(inode->i_rdev);
	major = MAJOR(inode->i_rdev);

	printk("\n uart_write ok major= %d, minor=%d\n", major,minor);

//	if(count >20)
//		count = 20;



       cp_flag=copy_from_user(wbuf,buf,count);
       if(cp_flag!=0) 
{
	printk("uart write error\n");
	return 0;
}
	wbuf[count-1] = '\0';



	while('\0' != wbuf[str])
	{
		state = readl(UART_UTRSTAT);
		if((state & 2)  ==  0x02)
		{
			writel(wbuf[str], UART_UTXH);
			str++;
		}
	}
return count;
}

static ssize_t uart_read(struct file *filp,char __user *buf,size_t count,loff_t *f_pos)

{
	struct inode *inode = filp->f_dentry->d_inode;
	int major,minor;
	char rbuf[count];
      //	char rbuf=0;
	int state;
	int read_count = count;
	//int read_count=1;
        int i=0;
	int cp_flag;
	minor = MINOR(inode->i_rdev);
	major = MAJOR(inode->i_rdev);

	printk("uart_read ok major=%d,minor=%d\n",major,minor);

	while(read_count)
	{
		state = readl(UART_UTRSTAT);

		if((state & 1)  ==  0x01)	
		{
			char ch;
			ch = readl(UART_URXH);
			rbuf[i++] = ch;
			read_count --;
		}
		else
		{		
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(1);
                       // return 0;
		}
	} 
	rbuf[count-1]='\0';
//	if(count>20)
//		count=20;

	cp_flag=copy_to_user(buf,rbuf,count);
        if(cp_flag!=0) 
	{
	  printk("uart_read error");
 	 return 0;
	}
//	buf[count-1] = '\0';

	printk("\n");
	return count;
}
static int uart_release(struct inode *inode, struct file *filp)
{
	return 0;
}
static int uart_open(struct inoder *inode,struct file *flip)
{
	//int rc;
	/*8N1*/
	writel(3, UART_ULCON);	// 00000011

	/*poll mode */
	writel(5, UART_UCON);	// 00000101

	/*115200*/
	writel(26, UART_UBRDIV);	//this set to 26 ,depend on pclk
        return 0;
}
static struct file_operations uart_fops={
	.owner = THIS_MODULE,
	.read  = uart_read,
	.write = uart_write,
	.release = uart_release,
	.open =uart_open,
};


static int __init uart_init(void)
{
	
        UART_MAJOR = register_chrdev(0,"uart", &uart_fops);
	if(UART_MAJOR <0)
	{
		printk("register %s char dev error\n","uart");
		return -1;
	}
	printk("uart driver register ok!\n");
	uart_class = class_create(THIS_MODULE,"uart");
        if(IS_ERR(uart_class))
	return PTR_ERR(uart_class);
	uart_class_dev = class_device_create(uart_class,NULL,MKDEV(UART_MAJOR,0),NULL,"uart"); //dev/uart
	if(unlikely(IS_ERR(uart_class_dev)))
	return PTR_ERR(uart_class_dev);
	return 0;
}




static void __exit uart_exit(void)
{
        unregister_chrdev(UART_MAJOR,"uart");
	printk("module exit\n");
	class_device_unregister(uart_class_dev);
	class_destroy(uart_class);
	return;
}

module_init(uart_init);
module_exit(uart_exit);
MODULE_AUTHOR("huang");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("serial module test");
MODULE_LICENSE("GPL");
