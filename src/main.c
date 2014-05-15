#define ALLEGRO_STATICLINK

//standard libs
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <limits.h>

//allegro libs
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

//OS X libs
#ifdef __APPLE__
#include <asl.h>
#endif

//constants
//==============================================================================

//you idiot you are skipping 7/8ths of the bits

//actor flags
#define FLAG_STATIC            0x0000000000000001
#define FLAG_ALWAYSRELEVENT    0x0000000000000010
#define FLAG_HIDDEN            0x0000000000000100
#define FLAG_DEAD              0x0000000000001000
#define FLAG_MOVEABLE          0x0000000000010000
#define FLAG_ERPING            0x0000000000100000
#define FLAG_ROTERPING         0x0000000001000000
#define FLAG_INHERITLOCATION   0x0000000010000000
#define FLAG_INHERITROTATION   0x0000000100000000
#define FLAG_TRAVEL            0x0000001000000000
#define FLAG_COLLIDEACTORS     0x0000010000000000
#define FLAG_BLOCKACTORS       0x0000100000000000
#define FLAG_LIFESPAN          0x0001000000000000
#define FLAG_TICKCOLLISION     0x1000000000000000

//input flags //note -- in future version switch these away from specific keys
#define IFLAG_1                0x0000000000000001
#define IFLAG_2                0x0000000000000010
#define IFLAG_3                0x0000000000000100
#define IFLAG_4                0x0000000000001000
#define IFLAG_5                0x0000000000010000
#define IFLAG_W                0x0000000000100000
#define IFLAG_A                0x0000000001000000
#define IFLAG_S                0x0000000010000000
#define IFLAG_D                0x0000000100000000
#define IFLAG_SPACE            0x0000001000000000
#define IFLAG_E                0x0000010000000000
#define IFLAG_ENTER            0x0000100000000000

//dungeon tiles
#define T_WALL                 0x00
#define T_FLOOR                0x01
#define T_HARDWALL             0x02
#define T_NONE                 0x03
#define T_CLEAR                0x10

//macros
//==============================================================================
#define SMOOTHSTEP(x) (x * x * (3.0 - 2.0 * x))

#define LOG(x) printf(x)

//typedefs
//==============================================================================
typedef struct ActorStruct* Actor;
typedef struct Vec2Struct* Vec2;
typedef struct LevelStruct* Level;

//enums defs
//==============================================================================

//struct defs
//==============================================================================

//Vec2Struct def
struct Vec2Struct {
  float x;
  float y;
};

//LevelStruct def
struct LevelStruct {
  int uid;
  char* name;
  char* desc;
  int depth;
  int height, width;
  char* tiles;
  Actor me;
  float friction;
};

//ActorStruct def
struct ActorStruct {
  struct Vec2Struct location;
  float rotation;
  struct Vec2Struct velocity, accelleration;
  float rotVelocity, rotAccelleration;
  struct Vec2Struct erpStart, erpEnd;
  float erpAlpha, erpRotAlpha, erpRotStart, erpRotEnd;
  uint64_t flags;
  float lifespan, bounciness, frictionMultiplier;
  Level myLevel;
  Actor owner, attachedTo;
  Actor* children;
  Actor** master;
  int *mLen;
  int cLen;
  int drawPrecedence;
  float height, width, mass, erpSpeed, drawScale;
  void (*tick)(Actor, double, uint64_t);
  void (*draw)(Actor, double, ALLEGRO_DISPLAY*, struct Vec2Struct, float);
  struct Vec2Struct (*erp)(struct Vec2Struct, struct Vec2Struct, float);
  float (*erpRot)(float, float, float);
  void (*collided)(Actor, Actor);
  int drawOp, drawSrc, drawDest;
  ALLEGRO_BITMAP* myGFX;
  ALLEGRO_COLOR drawColor;
};

//function defs
//==============================================================================

//vec2 functions

struct Vec2Struct vec2Add(struct Vec2Struct, struct Vec2Struct);
struct Vec2Struct vec2Scale(struct Vec2Struct, float);
struct Vec2Struct vec2Rotate(struct Vec2Struct, float);

//sparse functions

int sparseAdd(void*, void***, int*);
void* sparseRemove(void*, void***, int*);
void* sparseIndexRemove(int, void***, int*);
void* sparseGet(int, void***, int*);
int sparseIndexGet(void*, void***, int*);
int sparseUsageGet(void***, int*);
void sparseGrow(void***, int*);
void sparseShrink(void***, int*);

//event handler

void eventHandler(ALLEGRO_EVENT_QUEUE*, ALLEGRO_EVENT*, int*, double, double,
    Actor**, int*, ALLEGRO_DISPLAY*);

//ticking functions

void masterTick(double, Actor**, int*);

//drawing functions

void masterDraw(double, Actor**, int*, ALLEGRO_DISPLAY*);
void drawActor(Actor, double, ALLEGRO_DISPLAY*, struct Vec2Struct, float);
void drawTile(Actor, double, ALLEGRO_DISPLAY*, struct Vec2Struct, float);

//actor functions

Actor actorMake(Actor**, int*);
int actorRelevenceGet(Actor);
struct Vec2Struct actorLocationGet(Actor);
float actorRotGet(Actor);
struct Vec2Struct actorVelocityGet(Actor);
struct Vec2Struct actorAccellerationGet(Actor);
int actorFlagsCheck(Actor, uint64_t);
void actorErpStart(Actor, struct Vec2Struct, float);
int actorCompar(const void*, const void*);

//tick functions
void tickPlayer(Actor, double, uint64_t);
void tickCamera(Actor, double, uint64_t);

//erp functions

struct Vec2Struct defaultErp(struct Vec2Struct, struct Vec2Struct, float);
struct Vec2Struct shakeyErp(struct Vec2Struct, struct Vec2Struct, float);
struct Vec2Struct lerpErp(struct Vec2Struct, struct Vec2Struct, float);

float defaultErpRot(float, float, float);

//collide function

void collideDefault(Actor, Actor);
void collidePlayer(Actor, Actor);

//generator functions

Level dgen();
Actor lgen(Level, Actor**, int*);

//game functions

void reset(Actor**, int*);

//misc functions
int myRand();

//global defs
//==============================================================================
float disableAlpha;
float gameSpeed;
Actor player;
Actor camera;
uint64_t iFlags;
ALLEGRO_BITMAP** gfx;

//function impls
//==============================================================================

//vec2Add impl
struct Vec2Struct vec2Add(struct Vec2Struct a, struct Vec2Struct b) {
  return (struct Vec2Struct){a.x + b.x, a.y + b.y};
}

//vec2Scale impl
struct Vec2Struct vec2Scale(struct Vec2Struct v, float s) {
  return (struct Vec2Struct){v.x * s, v.y * s};
}

//vec2Rotate impl
struct Vec2Struct vec2Rotate(struct Vec2Struct v, float r) {
  float nx = cos(r) * v.x - sin(r) * v.y;
  float ny = sin(r) * v.x + cos(r) * v.y;
  return (struct Vec2Struct){nx, ny};
}

//sparseAdd impl
int sparseAdd(void* object, void*** sparse, int* len) {
  if (object == NULL || sparse == NULL || len == NULL) return -1;
  if (*sparse == NULL) {
    *sparse = calloc(5, sizeof(void**));
    //warn if malloc failed
    if (*sparse == NULL) return -1; //malloc failed
    (*sparse)[0] = object;
    *len = 5;
    return 0;
  } else {
    //find first available spot and JAM IT IN
    int i;
    for (i = 0; i < *len; i++) {
      if ((*sparse)[i] == NULL) {//found a spot!
        (*sparse)[i] = object;
        return i;
      }
    }
    //no free spots, have to grow sparse array first
    i = *len;
    sparseGrow(sparse, len);
    if (*len == i) return -1; //length is same as before... failed to grow arr
    for (i = 0; i < *len; i++) {//lets try again to add
      if ((*sparse)[i] == NULL) {//found a spot!
        (*sparse)[i] = object;
        return i;
      }
    }
    return -1; //this should only happen on a very bad day
  }
  return -1;
}

//sparseRemove impl
void* sparseRemove(void* object, void*** sparse, int* len) {
  if (object == NULL) return NULL;
  if (sparse == NULL || *sparse == NULL || len == NULL) return NULL;
  int i;
  for (i = 0; i < *len; i++) {
    if ((*sparse)[i] == object) {
      (*sparse)[i] = NULL;
      return object;
    }
  }
  return NULL;
}

//sparseIndexRemove impl
void* sparseIndexRemove(int index, void*** sparse, int* len) {
  //laziest implementation ever
  return sparseRemove(sparseGet(index, sparse, len), sparse, len);
}

//sparseGet impl
void* sparseGet(int index, void*** sparse, int* len) {
  if (sparse == NULL || *sparse == NULL || len == NULL) return NULL;
  if (index < 0 || index >= *len) return NULL;
  return (*sparse)[index];
}

//sparseIndexGet impl
int sparseIndexGet(void* object, void*** sparse, int* len) {
  if (sparse == NULL || *sparse == NULL || len == NULL) return -1;
  if (object == NULL) return -1;
  int i;
  for (i = 0; i < *len; i++) {
    if ((*sparse)[i] == object) return i;
  }
  return -1;
}

//spraseUsageGet impl
int sparseUsageGet(void*** sparse, int* len) {
  if (sparse == NULL || *sparse == NULL || len == NULL) return -1;
  int i, j = 0;
  for (i = 0; i < *len; i++) {
    if ((*sparse)[i] != NULL) j++;
  }
  return j;
}

//sparseGrow impl
void sparseGrow(void*** sparse, int* len) {
  if (sparse == NULL || *sparse == NULL || len == NULL) return;
  int newlen = 2 * (*len);
  void** bigger = calloc(newlen, sizeof(void**));
  if (bigger == NULL) return; //uh oh
  int i, j = 0;
  for (i = 0; i < *len; i++) {
    if ((*sparse)[i] != NULL) {
      //bigger[i] = (*sparse)[i]; //forward copy
      bigger[newlen - ++j] = (*sparse)[i]; //backward copy
    }
  }
  *len = newlen;
  free(*sparse);
  *sparse = bigger;
}

//spraseShrink impl
void sparseShrink(void*** sparse, int* len) {
  if (sparse == NULL || *sparse == NULL || len == NULL) return;
  int use = sparseUsageGet(sparse, len);
  if (use == *len) return; //can't shrink a full array
  void** smaller = malloc(sizeof(void**) * ((use > 0) ? use : 1));
  if (smaller == NULL) return;
  int i, j = 0;
  for (i = 0; i < *len; i++) {
    if ((*sparse)[i] != NULL) smaller[j++] = (*sparse)[i];
  }
  free(*sparse);
  *sparse = smaller;
  *len = use;
}

//eventHandler impl
void eventHandler(ALLEGRO_EVENT_QUEUE* events, ALLEGRO_EVENT* e, int* exit,
    double delta, double gameDelta,
    Actor** master, int* mLen, ALLEGRO_DISPLAY* display) {
  while (al_get_next_event(events, e)) {
    switch (e->type) {
      case ALLEGRO_EVENT_DISPLAY_RESIZE:
        al_acknowledge_resize(display);
        break;
      case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
        //TODO figure out how to prevent click spam
        break;
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        *exit = 1;
        break;
      case ALLEGRO_EVENT_KEY_DOWN:
        switch(e->keyboard.keycode) {
          case ALLEGRO_KEY_W:
          case ALLEGRO_KEY_UP:
            iFlags |= IFLAG_W;
            break;
          case ALLEGRO_KEY_A:
          case ALLEGRO_KEY_LEFT:
            iFlags |= IFLAG_A;
            break;
          case ALLEGRO_KEY_S:
          case ALLEGRO_KEY_DOWN:
            iFlags |= IFLAG_S;
            break;
          case ALLEGRO_KEY_D:
          case ALLEGRO_KEY_RIGHT:
            iFlags |= IFLAG_D;
            break;
          case ALLEGRO_KEY_R:
            reset(master, mLen);
            break;
          case ALLEGRO_KEY_ESCAPE:
            *exit = 1;
            break;
        }
        break;
      case ALLEGRO_EVENT_KEY_UP:
        switch(e->keyboard.keycode) {
          case ALLEGRO_KEY_W:
          case ALLEGRO_KEY_UP:
            iFlags &= ~IFLAG_W;
            break;
          case ALLEGRO_KEY_A:
          case ALLEGRO_KEY_LEFT:
            iFlags &= ~IFLAG_A;
            break;
          case ALLEGRO_KEY_S:
          case ALLEGRO_KEY_DOWN:
            iFlags &= ~IFLAG_S;
            break;
          case ALLEGRO_KEY_D:
          case ALLEGRO_KEY_RIGHT:
            iFlags &= ~IFLAG_D;
            break;
        }
        break;
    } //TODO other events... might not need em
  }
}

// I'm coding in C / and reinventing the wheel-- / Segmentation fault

//masterTick impl
void masterTick(double delta, Actor** master, int* mLen) {
  Actor a;
  int i;
  for (i = 0; i < *mLen; i++) {
    a = sparseGet(i, (void***)master, mLen);
    if (!a) continue;
    if (actorFlagsCheck(a, FLAG_STATIC | FLAG_DEAD)) continue;
    if (!actorRelevenceGet(a)) continue;

    //general ticking
    a->flags &= ~FLAG_TICKCOLLISION; //reset tick collision flag
    //check lifespan and tick it down if the flag is set, kill if it is time
    if (actorFlagsCheck(a, FLAG_LIFESPAN)) {
      a->lifespan -= delta;
      if(a->lifespan < 0) {
        a->flags |= FLAG_DEAD;
        continue;
      }
    }

    //movement
    if (actorFlagsCheck(a, FLAG_MOVEABLE)) {
      if (actorFlagsCheck(a, FLAG_ERPING)) {
        a->erpAlpha += (float)delta * a->erpSpeed;
        if (a->erpAlpha > 1) a->flags &= ~FLAG_ERPING;
      }
      if (actorFlagsCheck(a, FLAG_ROTERPING)) a->erpRotAlpha += (float)delta;
      //get accelleration from friction
      float mu = (a->myLevel) ? mu = a->myLevel->friction *
          a->frictionMultiplier * a->mass : 0;
      struct Vec2Struct fAccell = vec2Scale(a->velocity, -1.0 * mu);

      if (!actorFlagsCheck(a, FLAG_ERPING)) {
        a->location.x += a->velocity.x * (float)delta;
        a->location.y += a->velocity.y * (float)delta;
      } else {
        a->location = a->erp(a->erpStart, a->erpEnd, a->erpAlpha);
        //FIXME instead of setting location, find deritivitive and set vel
      }
      a->velocity = vec2Add(a->velocity, vec2Scale(vec2Add(a->accelleration,
          fAccell), (float)delta));
      //a->velocity.x += a->accelleration.x * (float)delta;
      //a->velocity.y += a->accelleration.y * (float)delta;
      a->rotation += a->rotVelocity * (float)delta;
      a->rotVelocity += a->rotAccelleration * (float)delta;
    }

    //collision detection and resolution
    if (actorFlagsCheck(a, FLAG_COLLIDEACTORS)) {
      int j;
      for (j = 0; j < *mLen; j++) {
        Actor b = sparseGet(j, (void***)master, mLen);
        if (!b) continue;
        if (a == b) continue;
        if (!actorFlagsCheck(b, FLAG_COLLIDEACTORS)) continue;
        if (!actorRelevenceGet(a)) continue;
        struct Vec2Struct dist = vec2Add(a->location,
            vec2Scale(b->location, -1));
        dist.x = abs(dist.x);
        dist.y = abs(dist.y);
        if ((dist.x < a->width / 2 + b->width / 2) &&
            (dist.y < a->height / 2 + b->height / 2)) {
              //we have a hit!
              if (a->collided) a->collided(a, b);
              else collideDefault(a, b);
        }
      }
    }

    if (a->tick) a->tick(a, delta, iFlags);
  }
}

//masterDraw impl
void masterDraw(double delta, Actor** master, int* mLen, ALLEGRO_DISPLAY* d) {
  //obligatory epilepsy to unsure delta is correct
  static double omega = 0;
  omega += delta;
  if (omega > 2 * ALLEGRO_PI) omega -= 2 * ALLEGRO_PI;
  al_clear_to_color(al_map_rgb_f(
     0.8 + 0.1 * (1.0 + sin(4 * omega)),
     0.8 + 0.1 * (1.0 + cos(5 * omega)),
     0.5 + 0.25 * (1.0 + sin(omega))));

  //find center point;
  struct Vec2Struct drawOffset = (struct Vec2Struct){
      al_get_display_width(d) / 2,
      al_get_display_height(d) / 2};

  //determine scale based on vertical size of window
  float scale = al_get_display_height(d) / 480.0;

  //draw background
  al_set_blender(ALLEGRO_DEST_MINUS_SRC, ALLEGRO_ALPHA, ALLEGRO_ONE);
  al_draw_tinted_scaled_rotated_bitmap(gfx[0], al_map_rgba(255, 255, 255, 255),
      al_get_bitmap_width(gfx[0]) / 2, al_get_bitmap_height(gfx[0]) / 2,
      drawOffset.x, drawOffset.y,
      scale * 2, scale * 2, -camera->rotation * 1.1, 0);
  al_draw_tinted_scaled_rotated_bitmap(gfx[0], al_map_rgba(155, 155, 155, 255),
      al_get_bitmap_width(gfx[0]) / 2, al_get_bitmap_height(gfx[0]) / 2,
      drawOffset.x, drawOffset.y,
      scale * 2, scale * 4, 0.7 - camera->rotation * 1.1, 0);

  //draw actors relative to camera
  int i;
  for (i = 0; i < *mLen; i++) {
    Actor a = (*master)[i];
    if (!a) continue;
    if (actorFlagsCheck(a, FLAG_HIDDEN)) continue;
    if (!actorRelevenceGet(a)) continue;
    if (!camera) continue;
    struct Vec2Struct drawPoint = vec2Add(camera->location,
        vec2Scale(actorLocationGet(a), -1 * scale));
    drawPoint = vec2Rotate(drawPoint, -camera->rotation);
    drawPoint = vec2Add(drawPoint, drawOffset);
   if (abs(drawPoint.x) > al_get_display_width(d) * 1.1) continue;
   if (abs(drawPoint.y) > al_get_display_height(d) * 1.1) continue;
   if (abs(drawPoint.x) < - al_get_display_width(d) * 0.1) continue;
   if (abs(drawPoint.y) < - al_get_display_height(d) * 0.1) continue;

    //now that we have a draw point, lets draw
    if (a->draw) a->draw(a, delta, d, drawPoint, scale);
    else drawActor(a, delta, d, drawPoint, scale);
  }

  al_flip_display();
}

//drawActor impl
void drawActor(Actor a, double delta, ALLEGRO_DISPLAY* display,
    struct Vec2Struct drawPoint, float scale) {
  //default actor drawing
  al_set_blender(a->drawOp, a->drawSrc, a->drawDest);
  if (a->myGFX) {
      al_draw_tinted_scaled_rotated_bitmap(
          a->myGFX,
          a->drawColor,
          al_get_bitmap_width(a->myGFX) / 2,
          al_get_bitmap_height(a->myGFX) / 2,
          drawPoint.x, drawPoint.y,
          scale * a->drawScale, scale * a->drawScale,
          actorRotGet(a) - camera->rotation, 0);
  } else {
    al_draw_filled_circle(drawPoint.x, drawPoint.y,
        5 * scale * a->drawScale,
        a->drawColor);
    al_draw_circle(drawPoint.x, drawPoint.y, 5 * scale,
        al_map_rgb(0, 0, 0), 2);
  }
}

//drawTile impl
void drawTile(Actor a, double delta, ALLEGRO_DISPLAY* display,
    struct Vec2Struct drawPoint, float scale) {
  /*
  al_set_blender(ALLEGRO_ADD, ALLEGRO_DEST_COLOR, ALLEGRO_DEST_COLOR);
  al_draw_filled_circle(drawPoint.x, drawPoint.y, 8 * scale,
      al_map_rgb(64, 64, 64));
  //al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_DEST_COLOR);
  al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_DEST_COLOR);
  al_draw_filled_circle(drawPoint.x, drawPoint.y, 15 * scale,
      al_map_rgb(27, 27, 27));
  */
  drawActor(a, delta, display, drawPoint, scale);
}

//actorMake impl
Actor actorMake(Actor** master, int* mLen) {
  Actor a = malloc(sizeof(struct ActorStruct));
  a->drawOp = ALLEGRO_ADD;
  a->drawSrc = ALLEGRO_ONE;
  a->drawDest = ALLEGRO_INVERSE_ALPHA;
  a->drawPrecedence = 10;
  a->drawScale = 1;
  a->flags = 0;
  a->myGFX = NULL;
  a->mass = 10;
  a->bounciness = 0.8;
  a->frictionMultiplier = 1;
  a->myLevel = NULL;
  a->owner = NULL;
  a->attachedTo = NULL;
  a->master = master;
  a->mLen = mLen;
  a->children = NULL;
  a->cLen = 0;
  a->drawColor = al_map_rgba(255, 255, 255, 255);
  a->tick = NULL;
  a->draw = drawActor;
  a->erp = defaultErp;
  a->erpRot = defaultErpRot;
  a->collided = collideDefault;
  a->velocity = (struct Vec2Struct){0, 0};
  a->accelleration = (struct Vec2Struct){0, 0};
  a->location = (struct Vec2Struct){0, 0};
  a->rotation = 0;
  a->rotVelocity = 0;
  a->rotAccelleration = 0;
  a->height = 10;
  a->width = 10;

  sparseAdd(a, (void***)master, mLen);

  return a;
}

//actorRelevenceGet impl
int actorRelevenceGet(Actor a) {
  if (!a) return 0;
  if (actorFlagsCheck(a, FLAG_ALWAYSRELEVENT)) return 1;
  if (a->owner) {
    //most cases
    if (!player) return 0;
    if (sparseIndexGet(player, (void***)&(a->myLevel->me->children),
        &(a->myLevel->me->cLen)) >= 0) {
      return 1 & actorRelevenceGet(a->owner);
    } else return 0;
  } else return 1;
}

//actorLocationGet impl
struct Vec2Struct actorLocationGet(Actor a) {
  if (!a) return (struct Vec2Struct) {0.0, 0.0};
  if (!a->attachedTo | !actorFlagsCheck(a, FLAG_INHERITLOCATION)) {
    return a->location;
  } else return vec2Add(vec2Rotate(a->location, a->attachedTo->rotation),
      actorLocationGet(a->attachedTo));
}

//actorRotGet impl
float actorRotGet(Actor a) {
  if (!a) return 0.0;
  if (!a->attachedTo | !actorFlagsCheck(a, FLAG_INHERITROTATION)) {
    return a->rotation;
  } else return a->rotation + actorRotGet(a->attachedTo);
}

//actorVelocityGet impl
struct Vec2Struct actorVelocityGet(Actor a) {
  if (!a) return (struct Vec2Struct){0, 0};
  return a->velocity;
}

//actorAccellerationGet impl
struct Vec2Struct actorAccellerationGet(Actor a) {
  if (!a) return (struct Vec2Struct){0, 0};
  return a->accelleration;
}

//actorFlagsCheck impl
int actorFlagsCheck(Actor a, uint64_t flags) {
  if (a == NULL) return 0;
  return (a->flags & flags) != 0;
}

//actorErpStart impl
void actorErpStart(Actor a, struct Vec2Struct dest, float spd) {
  if (actorFlagsCheck(a, FLAG_ERPING)) {
    a->location = defaultErp(a->erpStart, a->erpEnd, a->erpAlpha);
  }
  a->erpStart = a->location;
  a->erpEnd = dest;
  a->erpAlpha = 0;
  a->erpSpeed = spd;
  a->flags |= FLAG_ERPING;
}

//actorCompar impl
int actorCompar(const void* p1, const void* p2) {
   Actor a = *((Actor*) p1);
   Actor b = *((Actor*) p2);
   if (a == NULL && b != NULL) return -1;
   else if (a != NULL && b == NULL) return 1;
   else if (a == NULL && b == NULL) return 0;
   else if (a->drawPrecedence > b->drawPrecedence) return -1;
   else if (a->drawPrecedence < b->drawPrecedence) return 1;
   else return 0;
}

//tickPlayer impl
void tickPlayer(Actor a, double delta, uint64_t input) {
  float rot = 0;
  int moving = 1;
  if (disableAlpha > 0) {
    moving = 0;
    disableAlpha -= delta;
  } else if (IFLAG_W & input && ~IFLAG_S & input) {
    if (IFLAG_A & input && ~IFLAG_D & input) {
      rot = 1.75 * ALLEGRO_PI;
    } else if (IFLAG_D & input && ~IFLAG_A & input) {
      rot = 0.25 * ALLEGRO_PI;
    } else rot = 0;
  } else if (IFLAG_S & input && ~IFLAG_W & input) {
    if (IFLAG_A & input && ~IFLAG_D & input) {
      rot = 1.25 * ALLEGRO_PI;
    } else if (IFLAG_D & input && ~IFLAG_A & input) {
      rot = 0.75 * ALLEGRO_PI;
    } else rot = 1.0 * ALLEGRO_PI;
  } else if  (IFLAG_A & input && ~IFLAG_D & input) {
    rot = 1.5 * ALLEGRO_PI;
  } else if  (IFLAG_D & input && ~IFLAG_A & input) {
    rot = 0.5 * ALLEGRO_PI;
  } else moving = 0;

  if (moving) a->accelleration = vec2Rotate((struct Vec2Struct){0, 4500}, rot);
  else a->accelleration = (struct Vec2Struct){0, 0};
  //TODO left click should do things
  //also animation? idk
}

//todo stub
void tickCamera(Actor a, double delta, uint64_t input) {
  if (!player) return;
  /*
  static struct Vec2Struct mt = (struct Vec2Struct){0, 0};
  struct Vec2Struct dist = vec2Add(player->location, vec2Scale(mt, -1));
  if (dist.x * dist.x + dist.y * dist.y > 400) {
    actorErpStart(a, player->location, 4);
    mt = player->location;
  }
  */
  a->velocity = vec2Add(player->location, vec2Scale(a->location, -1));
  a->velocity = vec2Scale(a->velocity, 10); //feels good enough
  //a->velocity = vec2Scale(a->velocity,fmax(0, 1.0 - 9.0 * (d / 500) * delta));

  if ((input & IFLAG_D) | (input & IFLAG_W)) {
    a->rotVelocity = 0.3;
  } else if ((input & IFLAG_A) | (input & IFLAG_S)) {
    a->rotVelocity = -0.3;
  } else a->rotVelocity = a->rotation * -1 * 0.9;
}

//defaultErp impl
struct Vec2Struct defaultErp(struct Vec2Struct a, struct Vec2Struct b,
    float alpha) {
  alpha = SMOOTHSTEP(alpha);
  return vec2Add(a, vec2Scale(vec2Add(b, vec2Scale(a, -1)), alpha));
}

//shakeyErp impl
struct Vec2Struct shakeyErp(struct Vec2Struct a, struct Vec2Struct b,
    float alpha) {
  return vec2Add(a, (struct Vec2Struct){
      (b.x - a.x) * cos(10 * ALLEGRO_PI * alpha) * alpha,
      (b.y - a.y) * sin(14 * ALLEGRO_PI * alpha) * alpha});
      
}

//lerpErp impl
struct Vec2Struct lerpErp(struct Vec2Struct a, struct Vec2Struct b,
    float alpha) {
  return vec2Add(a, vec2Scale(vec2Add(b, vec2Scale(a, -1)), alpha));
}

//defaultErpRot impl
float defaultErpRot(float a, float b, float alpha) { 
  alpha = SMOOTHSTEP(alpha);
  return a + (b - a) * alpha;
}

//collideDefault impl
void collideDefault(Actor a, Actor b) {
  //check if other actor exists Actually wait there will be a wall actor
  //don't collide with dead actors
  if (actorFlagsCheck(a, FLAG_DEAD) || actorFlagsCheck(b, FLAG_DEAD)) return;

  //evil hacking to prevent the collision resolution from lasting forever
  if (actorFlagsCheck(b, FLAG_TICKCOLLISION)) return;

  //one of the two won't collide, skip this interaction
  if (!actorFlagsCheck(a, FLAG_COLLIDEACTORS) ||
      !actorFlagsCheck(b, FLAG_COLLIDEACTORS)) {
    return;
  }
  //last we check if the other blocks. It's collision function still is called
  if (actorFlagsCheck(a, FLAG_BLOCKACTORS) &&
      actorFlagsCheck(b, FLAG_BLOCKACTORS)) {
    //determine angle of impact
    //redirect velocity based on that
    //omg this was the laziest implementation ever
    float dx = a->location.x - b->location.x;
    float dy = a->location.y - b->location.y;
    if (abs(dx) > abs(dy)) { //horizontal collision
      if (a->location.x > b->location.x) a->velocity.x += 100 + abs(a->velocity.x);
      else a->velocity.x -= 100 + abs(a->velocity.x);
    } else { //vertical collision
      if (a->location.y > b->location.y) a->velocity.y += 100 + abs(a->velocity.y);
      else a->velocity.y -= 100 + abs(a->velocity.y);
    }
    a->velocity = vec2Scale(a->velocity, a->bounciness);
    float ratio = b->mass / a->mass * a->bounciness;
    a->velocity = vec2Add(a->velocity, vec2Scale(b->velocity, ratio));
  }

  //set our collision flag
  a->flags |= FLAG_TICKCOLLISION;

  if (b->collided) b->collided(b, a);
}

//collidePlayer impl
void collidePlayer(Actor a, Actor b) {
  if (actorFlagsCheck(b, FLAG_COLLIDEACTORS) &&
      actorFlagsCheck(b, FLAG_BLOCKACTORS)) {
    disableAlpha = 0.20;
  if (actorFlagsCheck(a, FLAG_DEAD) || actorFlagsCheck(b, FLAG_DEAD)) return;
  }

  collideDefault(a, b);
}

//dgen impl
Level dgen() {
  Level l = malloc(sizeof(struct LevelStruct));

  l->width = 50 + myRand() % 100;
  l->height = 50 + myRand() % 100;
  l->friction = 0.9;
  l->tiles = calloc(l->width * l->height, sizeof(char));

  struct TilePair {
    int x;
    int y;
  };

  //get the center point
  int xx = (int)((float)(l->width / 2.0));
  int yy = (int)((float)(l->height / 2.0));
  l->tiles[(xx - 1) + l->width * (yy - 1)] = T_FLOOR;
  l->tiles[(xx - 0) + l->width * (yy - 1)] = T_FLOOR;
  l->tiles[(xx + 1) + l->width * (yy - 1)] = T_FLOOR;
  l->tiles[(xx - 1) + l->width * (yy - 0)] = T_FLOOR;
  l->tiles[(xx - 0) + l->width * (yy - 0)] = T_FLOOR;
  l->tiles[(xx + 1) + l->width * (yy - 0)] = T_FLOOR;
  l->tiles[(xx - 1) + l->width * (yy + 1)] = T_FLOOR;
  l->tiles[(xx - 0) + l->width * (yy + 1)] = T_FLOOR;
  l->tiles[(xx + 1) + l->width * (yy + 1)] = T_FLOOR;

  l->tiles[(xx + 0) + l->width * (yy - 2)] = T_FLOOR;
  l->tiles[(xx + 0) + l->width * (yy - 3)] = T_FLOOR;

  l->tiles[(xx - 2) + l->width * (yy + 0)] = T_FLOOR;
  l->tiles[(xx - 3) + l->width * (yy + 0)] = T_FLOOR;

  l->tiles[(xx + 2) + l->width * (yy + 0)] = T_FLOOR;
  l->tiles[(xx + 3) + l->width * (yy + 0)] = T_FLOOR;

  l->tiles[(xx + 0) + l->width * (yy + 2)] = T_FLOOR;
  l->tiles[(xx + 0) + l->width * (yy + 3)] = T_FLOOR;

  l->tiles[(xx + 2) + l->width * (yy + 1)] = T_HARDWALL;
  l->tiles[(xx + 2) + l->width * (yy + 2)] = T_HARDWALL;
  l->tiles[(xx + 1) + l->width * (yy + 2)] = T_HARDWALL;

  l->tiles[(xx - 2) + l->width * (yy + 1)] = T_HARDWALL;
  l->tiles[(xx - 2) + l->width * (yy + 2)] = T_HARDWALL;
  l->tiles[(xx - 1) + l->width * (yy + 2)] = T_HARDWALL;

  l->tiles[(xx + 2) + l->width * (yy - 1)] = T_HARDWALL;
  l->tiles[(xx + 2) + l->width * (yy - 2)] = T_HARDWALL;
  l->tiles[(xx + 1) + l->width * (yy - 2)] = T_HARDWALL;

  l->tiles[(xx - 2) + l->width * (yy - 1)] = T_HARDWALL;
  l->tiles[(xx - 2) + l->width * (yy - 2)] = T_HARDWALL;
  l->tiles[(xx - 1) + l->width * (yy - 2)] = T_HARDWALL;

  int i;
  for (i = 0; i < l->width; i++) {
    l->tiles[i] = T_HARDWALL;
    l->tiles[l->height * l->width - 1 - i] = T_HARDWALL;
    l->tiles[l->height * i + l->width - 1] = T_HARDWALL;
    l->tiles[l->height * i] = T_HARDWALL;
  }

  //now to add tiles to a list of possible expansions
  //struct TilePair* tpl = NULL;
  //int tplLen = 0;
  //ok going to use a dumb algorithm
  for (i = 0; i < l->width * l->height; i++) { //initial iteration
    if ((l->tiles[i] == T_WALL) && (myRand() % 9 > 8)) {
      l->tiles[i] = T_FLOOR;
    }
  }
  int j;
  for (j = 0; j < 15; j++) {
  for (i = 0; i < l->width * l->height; i++) {
    char t = l->tiles[i];
    int surround = 0;
    if (t == T_WALL) {
      if ((l->tiles[i - 1]) == T_WALL) surround++; //left
      if ((l->tiles[i + 1]) == T_WALL) surround++; //right
      if ((l->tiles[i - l->width]) == T_WALL) surround++; //top
      if ((l->tiles[i + l->width]) == T_WALL) surround++; //bottom
      //if ((l->tiles[i - 1 + l->width]) == T_WALL) surround++; //b left
      //if ((l->tiles[i + 1 + l->width]) == T_WALL) surround++; //b right
      //if ((l->tiles[i - 1 - l->width]) == T_WALL) surround++; //t left
      //if ((l->tiles[i + 1 - l->width]) == T_WALL) surround++; //t right

      if (myRand() % 12 < 4 - surround) l->tiles[i] = T_FLOOR;
    }
  }
  } //end outer loop

  //REALLY made sure that the center is ok
  l->tiles[(xx - 1) + l->width * (yy - 1)] = T_FLOOR;
  l->tiles[(xx - 0) + l->width * (yy - 1)] = T_FLOOR;
  l->tiles[(xx + 1) + l->width * (yy - 1)] = T_FLOOR;
  l->tiles[(xx - 1) + l->width * (yy - 0)] = T_FLOOR;
  l->tiles[(xx - 0) + l->width * (yy - 0)] = T_FLOOR;
  l->tiles[(xx + 1) + l->width * (yy - 0)] = T_FLOOR;
  l->tiles[(xx - 1) + l->width * (yy + 1)] = T_FLOOR;
  l->tiles[(xx - 0) + l->width * (yy + 1)] = T_FLOOR;
  l->tiles[(xx + 1) + l->width * (yy + 1)] = T_FLOOR;

  l->tiles[(xx + 0) + l->width * (yy - 2)] = T_FLOOR;
  l->tiles[(xx + 0) + l->width * (yy - 3)] = T_FLOOR;

  l->tiles[(xx - 2) + l->width * (yy + 0)] = T_FLOOR;
  l->tiles[(xx - 3) + l->width * (yy + 0)] = T_FLOOR;

  l->tiles[(xx + 2) + l->width * (yy + 0)] = T_FLOOR;
  l->tiles[(xx + 3) + l->width * (yy + 0)] = T_FLOOR;

  l->tiles[(xx + 0) + l->width * (yy + 2)] = T_FLOOR;
  l->tiles[(xx + 0) + l->width * (yy + 3)] = T_FLOOR;

  l->tiles[(xx + 2) + l->width * (yy + 1)] = T_HARDWALL;
  l->tiles[(xx + 2) + l->width * (yy + 2)] = T_HARDWALL;
  l->tiles[(xx + 1) + l->width * (yy + 2)] = T_HARDWALL;

  l->tiles[(xx - 2) + l->width * (yy + 1)] = T_HARDWALL;
  l->tiles[(xx - 2) + l->width * (yy + 2)] = T_HARDWALL;
  l->tiles[(xx - 1) + l->width * (yy + 2)] = T_HARDWALL;

  l->tiles[(xx + 2) + l->width * (yy - 1)] = T_HARDWALL;
  l->tiles[(xx + 2) + l->width * (yy - 2)] = T_HARDWALL;
  l->tiles[(xx + 1) + l->width * (yy - 2)] = T_HARDWALL;

  l->tiles[(xx - 2) + l->width * (yy - 1)] = T_HARDWALL;
  l->tiles[(xx - 2) + l->width * (yy - 2)] = T_HARDWALL;
  l->tiles[(xx - 1) + l->width * (yy - 2)] = T_HARDWALL;
  //finish up by pruning tiles where all eight adjacents are walls
  for (i = 0; i < l->width * l->height; i++) {
    if (l->tiles[i] == T_HARDWALL) l->tiles[i] = T_WALL;
  }

  for (i = 0; i < l->width * l->height; i++) {
    char t = l->tiles[i];
    int surround = 0;
    if (t == T_WALL) {
      if (i == 0) { //corner case
        surround += 5;
        if ((l->tiles[i + 1] & 0x0F) == T_WALL) surround++; //right
        if ((l->tiles[i + l->width] & 0x0F) == T_WALL) surround++; //bottom
        if ((l->tiles[i + 1 + l->width] & 0x0F) == T_WALL) surround++; //b right
      } else if (i == l->width * l->height - 1) { //corner case
        surround += 5;
        if ((l->tiles[i - l->width] & 0x0F) == T_WALL) surround++; //top
        if ((l->tiles[i - 1] & 0x0F) == T_WALL) surround++; //left
        if ((l->tiles[i - 1 - l->width] & 0x0F) == T_WALL) surround++; //t left
      } else if (i == l->width - 1) { //coner case
        surround += 5;
        if ((l->tiles[i - 1] & 0x0F) == T_WALL) surround++; //left
        if ((l->tiles[i + l->width] & 0x0F) == T_WALL) surround++; //bottom
        if ((l->tiles[i - 1 + l->width] & 0x0F) == T_WALL) surround++; //b left
      } else if (i == l->width * (l->height -1)) { //corner case
        surround += 5;
        if ((l->tiles[i - l->width] & 0x0F) == T_WALL) surround++; //top
        if ((l->tiles[i + 1] & 0x0F) == T_WALL) surround++; //right
        if ((l->tiles[i + 1 - l->width] & 0x0F) == T_WALL) surround++; //t right
      } else if (i < l->width) { //DO NOT test top 3
        surround += 3;
        if ((l->tiles[i - 1] & 0x0F) == T_WALL) surround++; //left
        if ((l->tiles[i + 1] & 0x0F) == T_WALL) surround++; //right
        if ((l->tiles[i + l->width] & 0x0F) == T_WALL) surround++; //bottom
        if ((l->tiles[i - 1 + l->width] & 0x0F) == T_WALL) surround++; //b left
        if ((l->tiles[i + 1 + l->width] & 0x0F) == T_WALL) surround++; //b right
      } else if (i % l->width == 0) { //left side
        surround += 3;
        if ((l->tiles[i + 1] & 0x0F) == T_WALL) surround++; //right
        if ((l->tiles[i - l->width] & 0x0F) == T_WALL) surround++; //top
        if ((l->tiles[i + l->width] & 0x0F) == T_WALL) surround++; //bottom
        if ((l->tiles[i + 1 + l->width] & 0x0F) == T_WALL) surround++; //b right
        if ((l->tiles[i + 1 - l->width] & 0x0F) == T_WALL) surround++; //t right
      } else if (i % l->width == l->width - 1) { //right side
        surround += 3;
        if ((l->tiles[i - 1] & 0x0F) == T_WALL) surround++; //left
        if ((l->tiles[i - l->width] & 0x0F) == T_WALL) surround++; //top
        if ((l->tiles[i + l->width] & 0x0F) == T_WALL) surround++; //bottom
        if ((l->tiles[i - 1 + l->width] & 0x0F) == T_WALL) surround++; //b left
        if ((l->tiles[i - 1 - l->width] & 0x0F) == T_WALL) surround++; //t left
      } else if (i > l->width * (l->height - 1)) { //bottom
        surround += 3;
        if ((l->tiles[i - 1] & 0x0F) == T_WALL) surround++; //left
        if ((l->tiles[i + 1] & 0x0F) == T_WALL) surround++; //right
        if ((l->tiles[i - l->width] & 0x0F) == T_WALL) surround++; //top
        if ((l->tiles[i - 1 - l->width] & 0x0F) == T_WALL) surround++; //t left
        if ((l->tiles[i + 1 - l->width] & 0x0F) == T_WALL) surround++; //t right
      } else {
        if ((l->tiles[i - 1] & 0x0F) == T_WALL) surround++; //left
        if ((l->tiles[i + 1] & 0x0F) == T_WALL) surround++; //right
        if ((l->tiles[i - l->width] & 0x0F) == T_WALL) surround++; //top
        if ((l->tiles[i + l->width] & 0x0F) == T_WALL) surround++; //bottom
        if ((l->tiles[i - 1 + l->width] & 0x0F) == T_WALL) surround++; //b left
        if ((l->tiles[i + 1 + l->width] & 0x0F) == T_WALL) surround++; //b right
        if ((l->tiles[i - 1 - l->width] & 0x0F) == T_WALL) surround++; //t left
        if ((l->tiles[i + 1 - l->width] & 0x0F) == T_WALL) surround++; //t right
      }
      if (surround >= 8) {
        l->tiles[i] |= T_CLEAR;
      }
    }
  }

  for (i = 0; i < l->width * l->height; i++) {
    //doesn't make sense but it works
    if (l->tiles[i] & ~T_CLEAR == T_WALL) l->tiles[i] = T_NONE;
  }

  return l;
}

//lgen impl
Actor lgen(Level l, Actor** master, int* mLen) {
  Actor a = actorMake(master, mLen);
  a->myLevel = l;
  l->me = a;
  a->flags |= FLAG_STATIC | FLAG_HIDDEN;

  //test object
  //Actor b = actorMake(master, mLen);
  //b->flags |= FLAG_COLLIDEACTORS | FLAG_BLOCKACTORS | FLAG_MOVEABLE;
  //b->location.x = 30;
  //b->location.y = 30;
  //b->drawColor = al_map_rgb(255, 0, 0);

  //now we need to make tile actors
  int i;
  for (i = 0; i < l->width * l->height; i++) {
    char tile = (l->tiles)[i];
    if (tile == T_FLOOR) {
      Actor t = actorMake(master, mLen);
      t->drawPrecedence = 100;
      t->drawOp = ALLEGRO_DEST_MINUS_SRC;
      t->drawSrc = ALLEGRO_ALPHA;
      t->drawDest = ALLEGRO_ONE;
      t->myLevel = l;
      t->owner = a;
      t->flags |= FLAG_STATIC;
      t->draw = drawTile;
      //set tile graphic
      t->myGFX = gfx[1 + myRand() % 7];
      t->drawScale = 0.45;
      t->drawColor = al_map_rgba(255, 255, 255, 50);

      //set tile location
      t->location.x = ((i % l->width) - (float)(l->width / 2.0)) * 25;
      t->location.y = -((i / l->width) - (float)(l->height / 2.0)) * 25;
    } else if (tile == T_WALL) {
      Actor t = actorMake(master, mLen);
      t->drawPrecedence = 8;
      t->myLevel = l;
      t->owner = a;
      t->flags |= FLAG_STATIC | FLAG_COLLIDEACTORS | FLAG_BLOCKACTORS;
      t->height = 27;
      t->width = 27;
      t->myGFX = gfx[8];
      t->drawScale = 0.45;
      t->drawColor = al_map_rgba(255, 255, 255, 150);
      t->drawOp = ALLEGRO_DEST_MINUS_SRC;
      t->drawSrc = ALLEGRO_ALPHA;
      t->drawDest = ALLEGRO_ONE;
      t->draw = drawTile; //FIXME
      t->location.x = ((i % l->width) - (float)(l->width / 2.0)) * 25;
      t->location.y = -((i / l->width) - (float)(l->height / 2.0)) * 25;
    }
  }

  return a;
}

//reset impl
void reset(Actor** master, int* mLen) {
  //step 1: clean up old mess
  int i;
  Level* levels = NULL;
  int lLen = 0;
  //collecto up all the level and free all actors
  for (i = 0; i < *mLen; i++) {
    Actor a = sparseIndexRemove(i, (void***)master, mLen);
    if (a) {
      //collect level
      if (sparseIndexGet(a, (void***)(&levels), &lLen) != -1) {
        sparseAdd(a->myLevel, (void***)(&levels), &lLen);
      }
      free(a); //be free my little angel
    }
  }
  if (*master) free(*master);
  *master = NULL;
  *mLen = 0;
  for (i = 0; i < lLen; i++) {
    Level l = sparseIndexRemove(i, (void***)(&levels), &lLen);
    if (l) free (l);
  }
  if (levels) free(levels);

  //step 2: make a new mess
  camera = actorMake(master, mLen);
  camera->flags |= FLAG_HIDDEN | FLAG_ALWAYSRELEVENT | FLAG_MOVEABLE;
  camera->frictionMultiplier = 0;
  camera->mass = 0;
  camera->tick = tickCamera;
  camera->erp = lerpErp;
  //camera->rotVelocity = 1;

  player = actorMake(master, mLen);
  player->flags |= FLAG_TRAVEL | FLAG_ALWAYSRELEVENT | FLAG_MOVEABLE;
  player->flags |= FLAG_COLLIDEACTORS | FLAG_BLOCKACTORS;
  player->mass = 10;
  player->bounciness = 0.9;
  player->height = 8;
  player->width = 8;
  player->drawPrecedence = 9;
  player->collided = collidePlayer;
  player->tick = tickPlayer;

  Level l = dgen();
  Actor d = lgen(l, master, mLen);
  sparseAdd(player, (void***)(&(d->children)), &(d->cLen));
  player->owner = d;
  player->myLevel = l;

  //initialize globals
  gameSpeed = 1;
  disableAlpha = 0;
}

//myRand impl
int myRand() {
  static int count = 1001;
  if (count > 1000) {
    srand(time(0));
    count = 0;
  } else count++;
  return rand();
}

//main impl
int main(int argc, char** argv) {
  //initialization
  al_init();
  al_install_keyboard();
  al_install_mouse();
  //al_install_audio();
  //al_init_acodec_addon();
  al_init_primitives_addon();
  al_init_image_addon();

  //TODO load options
  //or not...

  //allegro display
  ALLEGRO_DISPLAY* display;
  //al_set_new_display_flags(ALLEGRO_RESIZABLE);
  display = al_create_display(640, 480);
  al_set_window_position(display, 25, 25);

  //allegro event queue
  ALLEGRO_EVENT_QUEUE* events = al_create_event_queue();
  ALLEGRO_EVENT e;

  //register allegro event sources
  al_register_event_source(events, al_get_display_event_source(display));
  al_register_event_source(events, al_get_keyboard_event_source());
  al_register_event_source(events, al_get_mouse_event_source());

  //load all the things
  gfx = calloc(9, sizeof(ALLEGRO_BITMAP*));
  gfx[0] = al_load_bitmap("background.jpg");
  gfx[1] = al_load_bitmap("sq0.gif");
  gfx[2] = al_load_bitmap("sq1.gif");
  gfx[3] = al_load_bitmap("sq2.gif");
  gfx[4] = al_load_bitmap("sq3.gif");
  gfx[5] = al_load_bitmap("sq4.gif");
  gfx[6] = al_load_bitmap("sq5.gif");
  gfx[7] = al_load_bitmap("sq6.gif");
  gfx[8] = al_load_bitmap("wall.gif");
  
  int t;
  for (t = 0; t < 9; t++) {
    if (!gfx[t]) {
      printf("failed to load graphics!\n");
      exit(0);
    }
  }

  //master actor list
  Actor* master = NULL;
  int mLen = 0;

  //game reset
  reset(&master, &mLen);


  //it *really* isn't be right to sort on EVERY frame, but idk what is right
  qsort(master, mLen, sizeof(Actor), actorCompar);

  //delta time
  clock_t current, last, diff;
  double delta, gameDelta;

  int exit = 0;
  last = clock();

  //main loop
  while(!exit) {
    //delta resolution
    current = clock();
    diff = current - last;
    delta = ((double)diff) / CLOCKS_PER_SEC;
    gameDelta = delta * gameSpeed;
    last = current;

    //event handling
    eventHandler(events, &e, &exit, delta, gameDelta, &master, &mLen, display);

    //ticking
    masterTick(gameDelta, &master, &mLen);

    //drawing
    masterDraw(delta, &master, &mLen, display);
  }

  //destruction
  al_destroy_display(display);
  //al_uninstall_audio();
  al_uninstall_keyboard();
  al_uninstall_mouse();

  free(gfx);

  return 0;
}
