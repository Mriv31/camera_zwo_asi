#include "zwo_asi/camera.hpp"

namespace zwo_asi
{


class VideoThread
{

public:
  VideoThread(Camera *cam);
  //~VideoThread(){};
  int get_ci();
  void addBuffer(unsigned char* buffer, int size);
  void Start();



  int get_w();
  int get_h();
  unsigned char **get_buffer_list();
  int get_tracking_buffer_size();
  Camera *get_camera() {return cam;};
  void set_frames_to_analyze(int a){*_frames_to_analyze=a;};
  void Resume();
  void Pause();
  void Stop();



  int* get_frames_to_analyze(){return _frames_to_analyze;};

  void increment_frames_to_analyze(){ if (defined_image_counter) {*_frames_to_analyze = *_frames_to_analyze+1;};};
  void define_image_counter(int *buffer, int size);

  void define_time_buffer(uint64_t *buffer);



  int ci;
  long int waiting_time_ms;
  int stop;
  int pause;
  uint64_t *time_buffer;



private:
  int *_frames_to_analyze;
  int defined_time_buffer;
  int defined_image_counter;
  int w;
  int h;
  int started = 0;
  int buffer_size;
  unsigned char **buffer_list;
  const int MAX_TRACKING_BUFFER_SIZE = 1024;
  Camera *cam;

};
}
