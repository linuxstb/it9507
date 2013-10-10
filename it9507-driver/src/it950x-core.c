/**
 * Copyright (c) 2013 ITE Corporation. All rights reserved. 
 *
 * Module Name:
 *   it950x-core.c
 *
 * Abstract:
 *   The core of IT950x serial driver.
 */

#include "it950x-core.h"

/* Get a minor range for devices from the usb maintainer */
#define USB_it913x_MINOR_RANGE 47
#define USB_it950x_MINOR_RANGE 16
#ifdef CONFIG_USB_DYNAMIC_MINORS
#define USB_it913x_MINOR_BASE	0
#define USB_it950x_MINOR_BASE	USB_it913x_MINOR_RANGE
#else
#define USB_it913x_MINOR_BASE	192
#define USB_it950x_MINOR_BASE	192 + USB_it913x_MINOR_RANGE
#endif

#if URB_TEST
unsigned int loop_cnt = 0;
unsigned int diff_time_tx_write = 0;
unsigned int min_1 = 0;
struct timeval now, start;	
#endif
	
static DEFINE_MUTEX(it950x_mutex);
static DEFINE_MUTEX(it950x_urb_kill);
static DEFINE_MUTEX(it950x_rb_mutex);

/* Structure for urb context */
typedef struct it950x_urb_context{
	struct it950x_dev *dev;
	u8 index;
} URBContext;

/* Structure to hold all of our device specific stuff */
struct it950x_dev {
	struct usb_device *	usbdev;			    /* the usb device for this device */
	struct usb_interface *	interface;		/* the interface for this device */
//	unsigned char *		bulk_in_buffer;		/* the buffer to receive data */
//	size_t			bulk_in_size;		    /* the size of the receive buffer */
//	__u8			bulk_in_endpointAddr;	/* the address of the bulk in endpoint */
//	__u8			bulk_out_endpointAddr;	/* the address of the bulk out endpoint */
	struct kref		kref;
	struct file *tx_file;
        struct it950x_state state;
	u8 tx_on;
	u8 tx_chip_minor;	
	u8 g_AP_use;
	atomic_t g_AP_use_tx;
	atomic_t tx_pw_on;	
	bool DeviceReboot, DevicePower;	
	bool TunerInited0, TunerInited1;	

	/* USB URB Related for TX*/
	int	   tx_urb_streaming;
	struct urb *tx_urbs[URB_COUNT_TX];
	URBContext tx_urb_context[URB_COUNT_TX];
	u8 tx_urbstatus[URB_COUNT_TX];
	u8 tx_urb_index;
	atomic_t tx_urb_counter;
        wait_queue_head_t TxQueue;
	spinlock_t TxRBKeyLock;		
	u8* pTxRingBuffer;           // Tx Ring Buffer Address.
	u32 TxCurrBuffPointAddr;     // Output urb addr.
	u32 TxWriteBuffPointAddr;	   // Entry of Ring Buffer.
	u32 dwTxWriteTolBufferSize;  // Total ringbuffer size.
	u32 dwTxRemaingBufferSize;   // Remaining size in buffer.
};

#define to_afa_dev(d) container_of(d, struct it950x_dev, kref)

static struct usb_driver  it950x_driver;

struct usb_device_id it950x_usb_id_table[] = {
		{ USB_DEVICE(0x048D,0x9507) },
		{ 0},		/* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, it950x_usb_id_table);

/* AirHD */
u32 Tx_RMRingBuffer(struct it950x_urb_context *context, u32 dwDataFrameSize)
{
	struct it950x_dev *dev = context->dev;
	u32 dwBuffLen = 0;
	unsigned long flags = 0;	

#if RB_DEBUG
	deb_data("Enter %s", __func__);
	deb_data("Tx_RMRingBuffer: dev->TxCurrBuffPointAddr %lu", dev->TxCurrBuffPointAddr);
#endif
	spin_lock_irqsave(&dev->TxRBKeyLock, flags);
		dev->TxCurrBuffPointAddr = (dev->TxCurrBuffPointAddr + dwDataFrameSize) % (dev->dwTxWriteTolBufferSize);
		dev->dwTxRemaingBufferSize += dwDataFrameSize;
	spin_unlock_irqrestore(&dev->TxRBKeyLock, flags);

	dev->tx_urbstatus[context->index] = 0;
	atomic_add(1, &dev->tx_urb_counter);

        wake_up_interruptible(&dev->TxQueue);
	return 0;
}

static void
tx_free_urbs(struct it950x_dev *dev)
{
	int i;
	//deb_data("Enter %s Function\n",__FUNCTION__);
	
	for (i = 0; i < URB_COUNT_TX; i++) {
		usb_free_urb(dev->tx_urbs[i]);
		dev->tx_urbs[i] = NULL;
	}
	deb_data("%s() end\n", __func__);
}
/*
static void
rx_kill_busy_urbs(struct it950x_dev *dev)
{
	int i;
	//deb_data("Enter %s Function\n",__FUNCTION__);
	
	for (i = 0; i < URB_COUNT_RX; i++) {
		if(dev->urbstatus_rx[i] == 1){
			usb_kill_urb(dev->urbs_rx[i]);
			deb_data("kill rx urb index %d\n", i);
		}
	}
	
	deb_data("%s() end\n", __func__);
}
static void
tx_kill_busy_urbs(struct it950x_dev *dev)
{
	int i;
	//deb_data("Enter %s Function\n",__FUNCTION__);
	
	for (i = 0; i < URB_COUNT_TX; i++) {
		if(dev->tx_urbstatus[i] == 1){
			usb_kill_urb(dev->tx_urbs[i]);
			deb_data("kill tx urb index %d\n", i);
		}
	}
	
	deb_data("%s() end\n", __func__);
}
*/

static int tx_stop_urb_transfer(struct it950x_dev *dev)
{
	//deb_data("%s()\n", __func__);

	if (!dev->tx_urb_streaming) {
		deb_data("%s: iso xfer already stop!\n", __func__);
		return 0;
	}
#if URB_TEST
	min_1 = 0;
#endif	
	dev->tx_urb_streaming = 0;
	
	/*DM368 usb bus error when using kill urb */
#if 0
	for (i = 0; i < URB_COUNT_TX; i++) {
		if(dev->tx_urbstatus[i] == 1) {
			dev->tx_urbstatus[i] = 0;
			usb_kill_urb(dev->tx_urbs[i]);
		}
		usb_free_urb(dev->tx_urbs[i]);
		dev->tx_urbs[i] = NULL;
	}

	mutex_lock(&it950x_urb_kill);
	tx_kill_busy_urbs(dev);
	mutex_unlock(&it950x_urb_kill);
#endif

	deb_data("%s() end\n", __func__);

	return 0;
}

u32 fabs_self(u32 a, u32 b)
{
	u32 c = 0;
	
	c = a - b;
	if(c >= 0) return c;
	else return c * -1;
}

/**
 * Tx_RingBuffer - Eagle the core of write Ring-Buffer.
 * Description:
 * Successful submissions return 0(Error_NO_ERROR) and submised buffer length.
 * Otherwise this routine returns a negative error number.
 */
int Tx_RingBuffer(
	struct it950x_dev *dev,
    u8* pBuffer,
    int non_blocking,
    u32* pBufferLength)
{
    u32 dwBuffLen = 0;
    u32 dwCurrBuffAddr = dev->TxCurrBuffPointAddr;
    u32 dwWriteBuffAddr = dev->TxWriteBuffPointAddr;
    int ret = -ENOMEM;
	unsigned long flags = 0;    

#if RB_DEBUG
	deb_data("RemaingBufferSize: {%lu}\n", dev->dwTxRemaingBufferSize);		
	deb_data("Tx_RingBuffer-CPLen %lu, dwCurrBuffAddr %lu, dwWriteBuffAddr %lu\n", *pBufferLength, dwCurrBuffAddr, dwWriteBuffAddr);
#endif
    /* RingBuffer full */
    if ((non_blocking) && (dev->dwTxRemaingBufferSize == 0))
      return -EWOULDBLOCK;

  while ((dev->dwTxRemaingBufferSize) == 0) {
#if RB_DEBUG		
	deb_data("dwTxRemaingBufferSize = 0\n");
#endif
        /* Block until wake_up_interruptible is called (from the USB completion handler) */
        ret = wait_event_interruptible(dev->TxQueue, dev->dwTxRemaingBufferSize > 0);

        /* Return if we were interrupted with a signal */
        if (ret < 0)
          return ret;
    }

    if ((dev->dwTxRemaingBufferSize) < *pBufferLength) {
                //DAVE*pBufferLength = 0;
#if RB_DEBUG
		deb_data("dwTxRemaingBufferSize < *pBufferLength\n");
#endif
		//DAVEreturn Error_NO_ERROR;
      *pBufferLength = dev->dwTxRemaingBufferSize;
    }

    if (*pBufferLength == 0) {
      return -Error_BUFFER_INSUFFICIENT;
    }
	
	/* memory must enough because checking at first in this function */
	if(dwWriteBuffAddr >= dwCurrBuffAddr){      
		/* To_kernel_urb not run a cycle or both run a cycle */
		dwBuffLen = dev->dwTxWriteTolBufferSize - dwWriteBuffAddr; //remaining memory (to buffer end), not contain buffer beginning to return urb
		if(dwBuffLen >= *pBufferLength){    // For not cycle case.
			/* end remaining memory is enough */
			memcpy(dev->pTxRingBuffer + dwWriteBuffAddr, pBuffer, *pBufferLength);
		}
		else{   /* For cycle case */
			/* use all end memory, run a cycle and need use beginning memory */
			memcpy(dev->pTxRingBuffer + dwWriteBuffAddr, pBuffer, dwBuffLen); //using end memory
			memcpy(dev->pTxRingBuffer, pBuffer + dwBuffLen, *pBufferLength - dwBuffLen); //using begining memory
		}
	}
	else{       
		/* To_kernel_urb run a cycle and Return_urb not */
		memcpy(dev->pTxRingBuffer + dwWriteBuffAddr, pBuffer, *pBufferLength);
	}

	spin_lock_irqsave(&dev->TxRBKeyLock, flags);
		dev->TxWriteBuffPointAddr = (dev->TxWriteBuffPointAddr + *pBufferLength) % (dev->dwTxWriteTolBufferSize);
		dev->dwTxRemaingBufferSize -= *pBufferLength;
	spin_unlock_irqrestore(&dev->TxRBKeyLock, flags);

	if(dev->tx_urb_streaming == 1){
		/* allow submit urb */
		while((dwWriteBuffAddr - (dev->tx_urb_index * URB_BUFSIZE_TX)) >= URB_BUFSIZE_TX && atomic_read(&dev->tx_urb_counter) > 0){
			/* while urb full and not submit */
			ret = usb_submit_urb(dev->tx_urbs[dev->tx_urb_index], GFP_ATOMIC);
			if (ret != 0) {
				tx_stop_urb_transfer(dev);
#if RB_DEBUG				
				deb_data("%s: failed urb submission, err = %d\n", __func__, ret);
#endif
				return ret;
			}
			
			dev->tx_urbstatus[dev->tx_urb_index] = 1;
			dev->tx_urb_index = (dev->tx_urb_index + 1) % URB_COUNT_TX;
			atomic_sub(1, &dev->tx_urb_counter);
		}
	}
   
    return Error_NO_ERROR;
}

/******************************************************************/

static void tx_urb_completion(struct urb *purb)
{
	struct it950x_urb_context *context = purb->context;
	int ptype = usb_pipetype(purb->pipe);
	
#if RB_DEBUG	
	deb_data("Enter %s", __func__);

	deb_data("urb id: %d. '%s' urb completed. status: %d, length: %d/%d, pack_num: %d, errors: %d\n",
		context->index,
		ptype == PIPE_ISOCHRONOUS ? "isoc" : "bulk",
		purb->status,purb->actual_length,purb->transfer_buffer_length,
		purb->number_of_packets,purb->error_count);
#endif		
	//context->dev->tx_urbstatus[context->index] = 0;
	switch (purb->status) {
		case 0:         /* success */
		case -ETIMEDOUT:    /* NAK */
			break;
		case -ECONNRESET:   /* kill */
		case -ENOENT:
			//context->dev->urb_use_count++;
			deb_data("TX ENOENT-urb completition error %d.\n", purb->status);
		case -ESHUTDOWN:
			return;
		default:        /* error */
			deb_data("TX urb completition error %d.\n", purb->status);
			break;
	}

	if (!context->dev) return;
	
	if (context->dev->tx_urb_streaming == 0) return;

	if (ptype != PIPE_BULK) {
		deb_data("TX %s() Unsupported URB type %d\n", __func__, ptype);
		return;
	}
	
	//ptr = (u8 *)purb->transfer_buffer;
	/* Feed the transport payload into the kernel demux */
	//dvb_dmx_swfilter_packets(&dev->dvb.demux,
	//	purb->transfer_buffer, purb->actual_length / 188);
	//if (purb->actual_length > 0)
	
	Tx_RMRingBuffer(context, URB_BUFSIZE_TX);
	
	return;
}

/* AirHD */
static int tx_start_urb_transfer(struct it950x_dev *dev)
{
	struct urb *purb;
	int i, ret = -ENOMEM;

	//deb_data("%s()\n", __func__);
#if URB_TEST
	min_1 = 0;
#endif	
	if (dev->tx_urb_streaming) {
		deb_data("%s: iso xfer already running!\n", __func__);
		return 0;
	}
	
	dev->TxCurrBuffPointAddr	= 0;
	dev->TxWriteBuffPointAddr = 0;

	for (i = 0; i < URB_COUNT_TX; i++) {
		purb = dev->tx_urbs[i];

		purb->transfer_buffer = dev->pTxRingBuffer + (URB_BUFSIZE_TX*i);
		if (!purb->transfer_buffer) {
			usb_free_urb(purb);
			dev->tx_urbs[i] = NULL;
			goto err;
		}
		
		dev->tx_urb_context[i].index = i;
		dev->tx_urb_context[i].dev = dev;
		dev->tx_urbstatus[i] = 0;
		
		purb->status = -EINPROGRESS;
		usb_fill_bulk_urb(purb,
				  dev->usbdev,
				  usb_sndbulkpipe(dev->usbdev, 0x06),
				  purb->transfer_buffer,
				  URB_BUFSIZE_TX,
				  tx_urb_completion,
				   &dev->tx_urb_context[i]);
		
		purb->transfer_flags = 0;
	}
	dev->dwTxRemaingBufferSize = dev->dwTxWriteTolBufferSize;
	dev->tx_urb_index = 0;
	dev->tx_urb_streaming = 1;
	atomic_set(&dev->tx_urb_counter, URB_COUNT_TX);

	ret = 0;

	deb_data("%s() end\n", __func__);

err:
	return ret;
}

static int it950x_usb_tx_open(struct inode *inode, struct file *file)
{
	struct it950x_dev *dev;
	struct usb_interface *interface;
	int mainsubminor, subminor;
	int retval = 0;
	int error;
	int order, order_cmd;

	deb_data("it950x_usb_tx_open function\n");
	
	mainsubminor = iminor(inode);	
	subminor = iminor(inode);
	interface = usb_find_interface(&it950x_driver, subminor);

try:
	while (!interface) {
		subminor++;
		interface = usb_find_interface(&it950x_driver, subminor);
		if (subminor >= mainsubminor + USB_it950x_MINOR_RANGE)
			break;
	}		
	
	if (!interface) {
		deb_data("%s - error, can't find device for minor %d",
		     __FUNCTION__, subminor);
		retval = -ENODEV;
		goto exit;
	}
	dev = usb_get_intfdata(interface);
	
	if (!dev) {
		deb_data("usb_get_intfdata fail!\n");
		retval = -ENODEV;
		goto exit;
	}
	
	if (subminor != dev->tx_chip_minor) {
		interface = NULL;
		goto try;
	}	
	deb_data("open TX subminor: %d\n", subminor);			
	

	atomic_add(1, &dev->g_AP_use_tx);
#if URB_TEST		
	do_gettimeofday(&start);	
#endif

	/*kzalloc will limit by Embedded system*/
#if 0
	dev->dwTxWriteTolBufferSize = URB_BUFSIZE_TX * URB_COUNT_TX;
	dev->pTxRingBuffer = kzalloc(dev->dwTxWriteTolBufferSize + 8, GFP_KERNEL);
	if (dev->pTxRingBuffer) {
		dev->pWriteFrameBuffer = dev->pTxRingBuffer + 8;
		dev->pTxCurrBuffPointAddr = dev->pTxRingBuffer;
		dev->pTxWriteBuffPointAddr = dev->pTxRingBuffer + 4;
		dev->dwTxRemaingBufferSize = dev->dwTxWriteTolBufferSize;
		dev->tx_urb_index = 0;
		//dev->urb_use_count = URB_COUNT_TX;
	}
#endif

	if( atomic_read(&dev->g_AP_use_tx) == 1) {      // Allocate buffer just for first user.
		init_waitqueue_head(&dev->TxQueue);
		atomic_set(&dev->tx_urb_counter, URB_COUNT_TX);
		/*Write Ring buffer alloc*/
		dev->dwTxWriteTolBufferSize = URB_BUFSIZE_TX * URB_COUNT_TX;
		order = get_order(dev->dwTxWriteTolBufferSize);
		dev->pTxRingBuffer = (u8*)__get_free_pages(GFP_KERNEL, order);
		if (dev->pTxRingBuffer) {
	//		dev->pWriteFrameBuffer = dev->pTxRingBuffer + 8;
			dev->TxCurrBuffPointAddr = 0;
			dev->TxWriteBuffPointAddr = 0;
			dev->dwTxRemaingBufferSize = dev->dwTxWriteTolBufferSize;
			dev->tx_urb_index = 0;
			//dev->urb_use_count = URB_COUNT_TX;
		}
		
		/* increment our usage count for the device */
		//kref_get(&dev->kref);

#if !(defined(DTVCAM_POWER_CTRL) && defined(AVSENDER_POWER_CTRL))
		if(atomic_read(&dev->tx_pw_on) == 0) {
			error = DL_ApPwCtrl(&dev->state, 1);
			atomic_set(&dev->tx_pw_on, 1);	
		}
#endif	

	}
	/* save our object in the file's private structure */
	dev->tx_file = file;
	file->private_data = dev;

exit:
	return retval;
}

static int it950x_usb_tx_release(struct inode *inode, struct file *file)
{
	struct it950x_dev *dev;
	int error;
	int order;
	//deb_data("it950x_usb_release function\n");	
	dev = (struct it950x_dev *)file->private_data;
	if (dev == NULL) {
		deb_data("dev is NULL\n");
		return -ENODEV;
	}	

	if(	atomic_read(&dev->g_AP_use_tx) == 1) {
/*		dev = (struct it950x_dev *)file->private_data;
		if (dev == NULL)
			return -ENODEV;
*/
		tx_stop_urb_transfer(dev);

		/* decrement the count on our device */
		//kref_put(&dev->kref, afa_delete);
		
		if (dev->pTxRingBuffer){
			order = get_order(dev->dwTxWriteTolBufferSize);
			free_pages((long unsigned int)dev->pTxRingBuffer, order);
		}

#if !(defined(DTVCAM_POWER_CTRL) && defined(AVSENDER_POWER_CTRL))		
		if(atomic_read(&dev->tx_pw_on) == 1) {
//DAVE			if(atomic_read(&dev->g_AP_use_rx) == 0) {   // RX not used, suspend tx.
				error = DL_ApPwCtrl(&dev->state, 0);
				atomic_set(&dev->tx_pw_on, 0);	
//DAVE			} else {
//DAVE				deb_data("RX lock TX_PowerSaving\n");
//DAVE			}
		}
#endif		

	}
	atomic_sub(1, &dev->g_AP_use_tx);

	return 0;
}

long it950x_usb_tx_unlocked_ioctl(
	struct file *file,
	unsigned int cmd,
	unsigned long parg)
{
	struct it950x_dev *dev;
	PCmdRequest pRequest;	
	//deb_data("it950x_usb_ioctl function\n");

	dev = (struct it950x_dev *)file->private_data;
	if (dev == NULL) {
		deb_data("dev is NULL\n");
		return -ENODEV;
	}

	/*
	* extract the type and number bitfields, and don't decode
	* wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	*/

	//if (_IOC_TYPE(cmd) != AFA_IOC_MAGIC) return -ENOTTY;
	//if (_IOC_NR(cmd) > AFA_IOC_MAXNR) return -ENOTTY;

	switch (cmd)
	{
		case IOCTL_ITE_DEMOD_STARTTRANSFER_TX:
			tx_start_urb_transfer(dev);
			return 0;
					
		case IOCTL_ITE_DEMOD_STOPTRANSFER_TX:
			tx_stop_urb_transfer(dev);
			return 0;
	}
	return DL_DemodIOCTLFun(&dev->state, cmd, parg);
}


static ssize_t it950x_tx_write(
	struct file *file,
	const char __user *user_buffer,
	size_t count, 
	loff_t *ppos)
{
	struct it950x_dev *dev;

	u32 Len = count;
        int res;

	/*AirHD RingBuffer*/
	dev = (struct it950x_dev *)file->private_data;
	if (dev == NULL)
		return -ENODEV;
#if URB_TEST
	loop_cnt++;	
#endif
	res = Tx_RingBuffer(dev, (u8*)user_buffer, file->f_flags & O_NONBLOCK, &Len);
	//printk("[%lu]\n", Len);
        if (res < 0) {
          return res;
        } else {
          return Len;
        }
}


static struct file_operations it950x_usb_tx_fops ={
	.owner =		THIS_MODULE,
	.open =		it950x_usb_tx_open,
	//.read =		it950x_read,
	.write =	it950x_tx_write, /*AirHD*/ 
	.release =	it950x_usb_tx_release,
	.unlocked_ioctl = it950x_usb_tx_unlocked_ioctl,
};

/*
 * usb class driver info in order to get a minor number from the usb core,
 * and to have the device registered with devfs and the driver core
 */
static struct usb_class_driver it950x_class_tx = {
	.name =			"dvbmod%d",
	.fops =			&it950x_usb_tx_fops,
	.minor_base = 	USB_it950x_MINOR_BASE
};


// Register a USB device node end


static int it950x_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	struct it950x_dev *dev = NULL;
	int retval = -ENOMEM;
	int i;

	/* allocate memory for our device state and intialize it */
	dev = kzalloc(sizeof(struct it950x_dev), GFP_KERNEL);
	dev->g_AP_use = 0;
	//dev->DC.modulator.ptrIQtableEx = kzalloc(sizeof(IQtable)*91, GFP_KERNEL);
	atomic_set(&dev->g_AP_use_tx, 0);

	if (dev == NULL) {
		deb_data("Out of memory\n");
		return retval;
	}
	
	dev->usbdev = interface_to_usbdev(intf);

	/* we can register the device now, as it is ready */
	usb_set_intfdata(intf, dev);

	//memset(&DC, 0, sizeof(DC));
	deb_data("===it950x usb device pluged in ===\n");
	retval = Device_init(interface_to_usbdev(intf), &dev->state, true);
	if (retval) {
		deb_data("Device_init Fail: 0x%08x\n", retval);
		return retval;
	}
	
	intf->minor = -1;
	retval = usb_register_dev(intf, &it950x_class_tx);
	if (retval) {
		deb_data("Not able to get a minor for this device.");
		usb_set_intfdata(intf, NULL);
		//goto error;
		return -ENOTTY;
	}
	dev->tx_chip_minor = intf->minor;
	deb_data("tx minor %d \n", dev->tx_chip_minor);

	for (i = 0; i < URB_COUNT_TX; i++) {

		dev->tx_urbs[i] = usb_alloc_urb(0, GFP_KERNEL);
		if (!dev->tx_urbs[i])
			retval = -ENOMEM;
	}

#if !(defined(DTVCAM_POWER_CTRL) && defined(AVSENDER_POWER_CTRL))
	//TODO:
	atomic_set(&dev->tx_pw_on, 0);	
//DAVE	atomic_set(&dev->rx_pw_on, 0);	
//DAVE	DL_ApPwCtrl(&dev->DC, 1, 0);	
	DL_ApPwCtrl(&dev->state, 0);	
#endif	
	deb_data("USB ITEtech device now attached to USBSkel-%d \n", intf->minor);

	return retval;
}

static int it950x_suspend(struct usb_interface *intf, pm_message_t state)
{
	int error;
	struct it950x_dev *dev;

	dev = usb_get_intfdata(intf);
	if (!dev) 
		deb_data("dev = NULL");
	//deb_data("Enter %s Function\n", __FUNCTION__);

#ifdef EEEPC
	error = DL_Reboot();
#else
    if (dev->DevicePower) {
		error = DL_CheckTunerInited(&dev->state, &dev->TunerInited0);

		dev->DeviceReboot = true;
    }
#endif
	
	return 0;
}

static int it950x_resume(struct usb_interface *intf)
{
	int retval = -ENOMEM;
	int error;
	struct it950x_dev *dev;

	//deb_data("Enter %s Function\n",__FUNCTION__);
	dev = usb_get_intfdata(intf);
	if (!dev) 
		deb_data("dev = NULL");


#ifdef EEEPC
#else
    if (dev->DeviceReboot) {
		retval = Device_init(interface_to_usbdev(intf),&dev->state, false);
		if(retval)
			deb_data("Device_init Fail: 0x%08x\n", retval);

    }
#endif
	
    return 0;
}

static void it950x_disconnect(struct usb_interface *intf)
{
	struct it950x_dev *dev;
	int minor = intf->minor;

	deb_data("%s()\n", __func__);
	/* prevent afa_open() from racing afa_disconnect() */
	//lock_kernel();
	mutex_lock(&it950x_mutex);

	dev = usb_get_intfdata(intf);
	if (!dev) {
		deb_data("dev = NULL");
	}
	
	/*DM368 usb bus error when using kill urb */
#if 0	
	mutex_lock(&it950x_urb_kill);
	
	if(dev->tx_on) tx_kill_busy_urbs(dev);
	if(dev->rx_on) rx_kill_busy_urbs(dev);
	
	mutex_unlock(&it950x_urb_kill);
#endif

	tx_free_urbs(dev);
//	rx_free_urbs(dev);
	
	usb_set_intfdata(intf, NULL);

	/* give back our minor */
	intf->minor = dev->tx_chip_minor;
	usb_deregister_dev(intf, &it950x_class_tx);

	mutex_unlock(&it950x_mutex);
	//unlock_kernel();
	
	/* decrement our usage count */
	//	kref_put(&dev->kref, afa_delete);

	if (dev){
		if(dev->tx_file) dev->tx_file->private_data = NULL;
		kfree(dev);
	}
	
	deb_data("USB ITEtech #%d now disconnected", minor);
}


static struct usb_driver it950x_driver = {
	.name       = "usb-it950x",
	.probe      = it950x_probe,
	.disconnect = it950x_disconnect,//dvb_usb_device_exit,
	.id_table   = it950x_usb_id_table,
	.suspend    = it950x_suspend,
	.resume     = it950x_resume,
};

static int __init it950x_module_init(void)
{
	int result;

	//info("%s",__FUNCTION__);
	//deb_data("usb_it950x Module is loaded \n");

	if ((result = usb_register(&it950x_driver))) {
		pr_err("usb_register failed. Error number %d",result);
		return result;
	}
	
	return 0;
}

static void __exit it950x_module_exit(void)
{
	deb_data("usb_it950x Module is unloaded!\n");
	usb_deregister(&it950x_driver);
}

module_init (it950x_module_init);
module_exit (it950x_module_exit);

MODULE_AUTHOR("Jason Dong <jason.dong@ite.com.tw>");
MODULE_DESCRIPTION("DTV Native Driver for Device Based on ITEtech it950x");
MODULE_VERSION(DRIVER_RELEASE_VERSION);
MODULE_LICENSE("GPL");
