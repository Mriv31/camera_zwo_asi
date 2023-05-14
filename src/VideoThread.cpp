#include "zwo_asi/VideoThread.hpp"
#include <thread>
#include <chrono>
namespace zwo_asi
{

uint64_t get_timestamp()
{
    std::chrono::time_point<std::chrono::steady_clock> ts = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(ts.time_since_epoch()).count();
}


void RunVideoThread(VideoThread *vt)
{
  Camera *cam = vt->get_camera();
  while(1)
  {
    if (vt->pause)
    {
     std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    else
    {
     if (cam != NULL)
     {
       if (cam->get_video_data(vt->get_buffer_list()[vt->ci],(long int) vt->get_w() * vt->get_h(), vt->waiting_time_ms) == 1) return;
       vt->time_buffer[vt->ci] = get_timestamp();
     }

    vt->ci+=1;
    if (vt->ci == vt->get_tracking_buffer_size()) vt->ci = 0;
    vt->increment_frames_to_analyze() ;
    if (vt->stop)
    {
        vt->set_frames_to_analyze(-1);
        delete vt;
        return;

    }
  };
  }
  }

  void VideoThread::increment_frames_to_analyze()
  {
    if (defined_image_counter)
    {
    if (mcv)
    {
      std::unique_lock lock(mcv->m_mutex);
      *_frames_to_analyze = *_frames_to_analyze+1;
      mcv->cv.notify_one();
      lock.unlock();
    }

    else
    {

      *_frames_to_analyze = *_frames_to_analyze+1;

    }


  }


    };


void VideoThread::define_time_buffer(uint64_t *buffer)
{
  if (defined_time_buffer) return;

  defined_time_buffer = 1;
  time_buffer= buffer;
}


void VideoThread::addBuffer(unsigned char* buffer, int size)
{
  if (size != w*h) return;
  if (started) return;
  if (buffer_size>=MAX_TRACKING_BUFFER_SIZE) return;

  buffer_list[buffer_size] = buffer;
  buffer_size+=1;
}

void VideoThread::define_image_counter(int *buffer,int size)
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
  printf("start\n");
}

void VideoThread::Stop()
{
  stop = 1;
}

void VideoThread::Pause()
{
  pause = 1;
}

void VideoThread::Resume()
{
  pause = 0;
}

VideoThread::VideoThread(Camera *camera)
{
  ROI roi;
  defined_image_counter = 0;
  cam = camera;
  stop = 0;
  mcv = NULL;

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
  pause = 0;
  started = 0;
  waiting_time_ms = 1000;
  defined_time_buffer = 0;

}

int VideoThread::get_ci() {return ci;};
int VideoThread::get_w() {return w;};
int VideoThread::get_h() {return h;};
unsigned char **VideoThread::get_buffer_list() {return buffer_list;};
int VideoThread::get_tracking_buffer_size() {return buffer_size;};
}
