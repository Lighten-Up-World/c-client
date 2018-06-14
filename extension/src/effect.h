#ifndef EFFECT_H
#define EFFECT_H

#include "pixel.h"

//TEMP DECLARATIONS
typedef void layout_t;
typedef void vector_t;
typedef void * any_t;

//Forward Declarations
typedef struct pixel_info pixel_info_t;
typedef struct frame_info frame_info_t;
typedef struct debug_info debug_info_t;
typedef struct effect effect_t;

typedef pixel_info_t *(*pixel_info_constructor_t)(int index, layout_t *layout);
typedef frame_info_t *(*frame_info_constructor_t)(layout_t *layout);
typedef debug_info_t *(*debug_info_constructor_t)(any_t runner);

struct pixel_info{
  pixel_info_constructor_t new;
  // grid_t point;
  // int index; Wouldn't these be in a pixel file
  layout_t *layout;
  // // Is this pixel being used, or is it a placeholder?
  // bool isMapped() const;
  //
  // // Look up data from the JSON layout
  // const rapidjson::Value& get(const char *attribute) const;
  // double getNumber(const char *attribute) const;
  // double getArrayNumber(const char *attribute, int index) const;
  // Vec2 getVec2(const char *attribute) const;
  // Vec3 getVec3(const char *attribute) const;
};

struct frame_info{
  frame_info_constructor_t new;
  double time_delta;
  vector_t *pixels;
  // // Model axis-aligned bounding box
  // Vec3 modelMin, modelMax;
  //
  // // Radius measured from center
  // Real modelRadius;
  //
  // // Calculated model info
  // Vec3 modelCenter() const;
  // Vec3 modelSize() const;
  // Real distanceOutsideBoundingBox(Vec3 p) const;
  //
  // // K-D Tree, for fast spatial lookups
  //
  // typedef nanoflann::KDTreeSingleIndexAdaptor<
  //     nanoflann::L2_Simple_Adaptor< Real, FrameInfo >,
  //     FrameInfo, 3> IndexTree;
  //
  // typedef std::vector<std::pair<size_t, Real> > ResultSet_t;
  //
  // void radiusSearch(ResultSet_t& hits, Vec3 point, float radius) const;
  //
  // IndexTree tree;
  //
  // // Adapter functions for the K-D tree implementation
  //
  // inline size_t kdtree_get_point_count() const {
  //     return pixels.size();
  // }
  //
  // inline Real kdtree_distance(const Real *p1, const size_t idx_p2, size_t size) const {
  //     Real d0 = p1[0] - pixels[idx_p2].point[0];
  //     Real d1 = p1[1] - pixels[idx_p2].point[1];
  //     Real d2 = p1[2] - pixels[idx_p2].point[2];
  //     return d0*d0 + d1*d1 + d2*d2;
  // }
  //
  // Real kdtree_get_pt(const size_t idx, int dim) const {
  //     return pixels[idx].point[dim];
  // }
  //
  // template <class BBOX> bool kdtree_get_bbox(BBOX &bb) const {
  //     bb[0].low  = modelMin[0];
  //     bb[1].low  = modelMin[1];
  //     bb[2].low  = modelMin[2];
  //     bb[0].high = modelMax[0];
  //     bb[1].high = modelMax[1];
  //     bb[2].high = modelMax[2];
  //     return true;
  // }
};

struct debug_info{
  debug_info_constructor_t new;
  any_t runner;
};

typedef void (*shader_func_t)(colour_t rgb, pixel_info_t *p);
typedef void (*frame_func_t)(frame_info_t *f);
typedef void (*debug_func_t)(debug_info_t *d);
typedef effect_t *(*effect_constructor_t)(void);

struct effect {
  effect_constructor_t new;

  pixel_info_t pixel_info;
  frame_info_t frame_info;
  debug_info_t debug_info;

  shader_func_t shader;
  frame_func_t beginFrame;
  frame_func_t endFrame;
  debug_func_t debug;

  long number_frames;
  long frame_count;
};

// Default pixel info functions
pixel_info_t *pixel_info_constructor (int index, layout_t *layout);

// Default frame info functions
frame_info_t *frame_info_constructor (layout_t *layout);

// Default debug info functions
debug_info_t *debug_info_constructor (any_t runner);

effect_t* e_new(void);
void e_free(effect_t *self);

frame_info_t* fi_new(void);
void fi_free(frame_info_t *self);

#endif
