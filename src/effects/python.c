#include "python.h"
#include "common.h"

void python_kill_process(effect_t *self) {
  // Kill python process here
  puts("b4 kill python");
  system("ps | grep python | awk '{print $1}' | xargs kill ");
  puts("python killed");
}

int dummy_python_run(effect_runner_t* self) {
  return 0;
}

effect_t *get_dummy_python_effect(void * obj) {
  effect_t *effect = calloc(1, sizeof(effect_t));
  if(effect == NULL){
    return NULL;
  }
  effect->time_delta = (struct timespec) {0, 50 * MILLI_TO_NANO};
  effect->run = &dummy_python_run;
  effect->remove = &python_kill_process;

  return effect;
}
