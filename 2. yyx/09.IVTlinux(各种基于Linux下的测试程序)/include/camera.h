#ifndef _CAMERA_H
#define _CAMERA_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <linux/types.h>
#include <linux/videodev2.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <assert.h>
#include "config.h"

#define BUFFER_COUNT 	4

typedef struct _frameBuf_tag{
    void *start;
	int length;
}frameBuf_t;

typedef struct _cameraObject_tag
{
	int index;					//��ˮ��
	cameraPara_t cameraPara;	//����
	devName_t devName;			//�豸����
	int fdCam;					//������ 	
	frameBuf_t* framebuf;		//֡������	
	struct v4l2_requestbuffers reqbuf;
	
	//struct v4l2_buffer buf;
}cameraObject_t;

int init_mmap(cameraObject_t* cameraObject);
int open_camera(char *videoDev);
int init_camera(cameraObject_t* cameraObject);
int start_capture(cameraObject_t* cameraObject);
void stop_capture(cameraObject_t* cameraObject);	
int close_camera_device(cameraObject_t* cameraObject);
int read_frame(cameraObject_t* cameraObject);


#endif

