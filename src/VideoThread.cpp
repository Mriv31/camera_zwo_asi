#include "zwo_asi/VideoThread.hpp"
#include <thread>

namespace zwo_asi
{
void RunVideoThread(VideoThread *vt)
{
  Camera *cam = vt->get_camera();
  while(1)
  {
     if (cam != NULL)
     {
       if (cam->get_video_data(vt->get_buffer_list()[vt->ci],(long int) vt->get_w() * vt->get_h(), vt->waiting_time_ms) == 1) return;
     }
    vt->ci+=1;
    if (vt->ci == vt->get_tracking_buffer_size()) vt->ci = 0;
    vt->increment_frames_to_analyze() ;
  }
  }


void VideoThread::addBuffer(unsigned char* buffer, int size)
{
  if (size != w*h) return;
  if (started) return;
  if (buffer_size>=MAX_TRACKING_BUFFER_SIZE) return;

  buffer_list[buffer_size] = buffer;
  buffer_size+=1;
}

void VideoThread::define_image_counter(long int *buffer,int size)
{
  if (defined_image_counter) return;
  if (size !=1) return;
  defined_image_counter = 1;
  _frames_to_analyze = buffer;
}

void VideoThread::Start()
{
  if (cam != NULL)  cam->start_video_capture();
  std::thread t(RunVideoThread,this);
  t.detach();
  started = 1;
}

VideoThread::VideoThread(Camera *camera)
{
  ROI roi;
  defined_image_counter = 0;
  cam = camera;

  if (cam != NULL)
  {
    roi = cam->get_roi();
    w = roi.width;
    h = roi.height;
  }

  else{
    w = 128;
    h = 128;
  }

  buffer_list = (unsigned char**)calloc(MAX_TRACKING_BUFFER_SIZE,sizeof(unsigned char*));
  buffer_size = 0;
  ci = 0;
  started = 0;
  waiting_time_ms = 10000;

}

int VideoThread::get_ci() {return ci;};
int VideoThread::get_w() {return w;};
int VideoThread::get_h() {return h;};
unsigned char **VideoThread::get_buffer_list() {return buffer_list;};
int VideoThread::get_tracking_buffer_size() {return buffer_size;};
}
