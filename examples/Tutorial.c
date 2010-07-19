/* -*- c -*- *****************************************************************
** Copyright (C) 2007 Sandia Corporation
** Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
** license for use of this work by or on behalf of the U.S. Government.
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that this Notice and any statement
** of authorship are reproduced on all copies.
**
** This is a simple example of using the IceT library.  It demonstrates the
** techniques described in the Tutorial chapter of the IceT User's Guide.
*****************************************************************************/

#include <stdlib.h>

/* IceT does not come with the facilities to create windows/OpenGL contexts.
 * we will use glut for that. */
#ifndef __APPLE__
#include <GL/glut.h>
#include <GL/gl.h>
#else
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#endif

#include <IceT.h>
#include <IceTGL.h>
#include <IceTMPI.h>

#define NUM_TILES_X 2
#define NUM_TILES_Y 2
#define TILE_WIDTH 300
#define TILE_HEIGHT 300

static void InitIceT();
static void DoFrame();
static void Draw();

static int winId;
static IceTContext icetContext;

int main(int argc, char **argv)
{
  int rank, numProc;
  IceTCommunicator icetComm;

  /* Setup MPI. */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &numProc);

  /* Setup a window and OpenGL context.  Normally you would just place all the
   * windows at 0, 0 (and probably full screen in tile display mode) to a local
   * display, but since this is an example we are assuming that they are all
   * going to one screen for display. */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
  glutInitWindowPosition((rank%NUM_TILES_X)*(TILE_WIDTH+10),
                         (rank/NUM_TILES_Y)*(TILE_HEIGHT+50));
  glutInitWindowSize(TILE_WIDTH, TILE_HEIGHT);
  winId = glutCreateWindow("IceT Example");

  /* Setup an IceT context.  Since we are only creating one, this context will
   * always be current. */
  icetComm = icetCreateMPICommunicator(MPI_COMM_WORLD);
  icetContext = icetCreateContext(icetComm);
  icetDestroyMPICommunicator(icetComm);

  /* Prepare for using the OpenGL layer. */
  icetGLInitialize();

  glutDisplayFunc(InitIceT);
  glutIdleFunc(DoFrame);

  /* Glut will only draw in the main loop.  This will simply call our idle
   * callback which will in turn call icetGLDrawFrame. */
  glutMainLoop();

  return 0;
}

static void InitIceT()
{
  IceTInt rank, num_proc;

  /* We could get these directly from MPI, but it's just as easy to get them
   * from IceT. */
  icetGetIntegerv(ICET_RANK, &rank);
  icetGetIntegerv(ICET_NUM_PROCESSES, &num_proc);

  /* We should be able to set any color we want, but we should do it BEFORE
   * icetGLDrawFrame() is called, not in the callback drawing function.
   * There may also be limitations on the background color when performing
   * color blending. */
  glClearColor(0.2f, 0.5f, 0.1f, 1.0f);

  /* Give IceT a function that will issue the OpenGL drawing commands. */
  icetGLDrawCallback(Draw);

  /* Give IceT the bounds of the polygons that will be drawn.  Note that
   * we must take into account any transformation that happens within the
   * draw function (but IceT will take care of any transformation that
   * happens before icetGLDrawFrame). */
  icetBoundingBoxf(-0.5f+rank, 0.5f+rank, -0.5, 0.5, -0.5, 0.5);

  /* Set up the tiled display.  Normally, the display will be fixed for a
   * given installation, but since this is a demo, we give two specific
   * examples. */
  if (num_proc < 4)
    {
    /* Here is an example of a "1 tile" case.  This is functionally
     * identical to a traditional sort last algorithm. */
    icetResetTiles();
    icetAddTile(0, 0, TILE_WIDTH, TILE_HEIGHT, 0);
    }
  else
    {
    /* Here is an example of a 4x4 tile layout.  The tiles are displayed
     * with the following ranks:
     *
     *               +---+---+
     *               | 0 | 1 |
     *               +---+---+
     *               | 2 | 3 |
     *               +---+---+
     *
     * Each tile is simply defined by grabing a viewport in an infinite
     * global display screen.  The global viewport projection is
     * automatically set to the smallest region containing all tiles.
     *
     * This example also shows tiles abutted against each other.
     * Mullions and overlaps can be implemented by simply shifting tiles
     * on top of or away from each other.
     */
    icetResetTiles();
    icetAddTile(0,          TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, 0);
    icetAddTile(TILE_WIDTH, TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, 1);
    icetAddTile(0,          0,           TILE_WIDTH, TILE_HEIGHT, 2);
    icetAddTile(TILE_WIDTH, 0,           TILE_WIDTH, TILE_HEIGHT, 3);
    }

  /* Tell IceT what strategy to use.  The REDUCE strategy is an all-around
   * good performer. */
  icetStrategy(ICET_STRATEGY_REDUCE);

  /* Set up the projection matrix as you normally would. */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-0.75, 0.75, -0.75, 0.75, -0.75, 0.75);

  /* Other normal OpenGL setup. */
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  if (rank%8 != 0)
    {
    GLfloat color[4];
    color[0] = (float)(rank%2);
    color[1] = (float)((rank/2)%2);
    color[2] = (float)((rank/4)%2);
    color[3] = 1.0;
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    }
}

static void DoFrame()
{
  /* In this idle callback, we do a simple animation loop and then exit. */
  static float angle = 0;

  IceTInt rank, num_proc;

  /* We could get these directly from MPI, but it's just as easy to get them
   * from IceT. */
  icetGetIntegerv(ICET_RANK, &rank);
  icetGetIntegerv(ICET_NUM_PROCESSES, &num_proc);

  if (angle <= 360)
    {
    /* We can set up a modelview matrix here and IceT will factor this
     * in determining the screen projection of the geometry.  Note that
     * there is further transformation in the draw function that IceT
     * cannot take into account.  That transformation is handled in the
     * application by deforming the bounds before giving them to
     * IceT. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(angle, 0.0, 1.0, 0.0);
    glScalef(1.0f/num_proc, 1.0, 1.0);
    glTranslatef(-(num_proc-1)/2.0f, 0.0, 0.0);

    /* Instead of calling Draw() directly, call it indirectly through
     * icetGLDrawFrame().  IceT will automatically handle image compositing. */
    icetGLDrawFrame();

    /* For obvious reasons, IceT should be run in double-buffered frame
     * mode.  After calling icetGLDrawFrame, the application should do a
     * synchronize (a barrier is often about as good as you can do) and
     * then a swap buffers. */
    glutSwapBuffers();

    angle += 1;
    }
  else
    {
    /* We are done with the animation.  Bail out of the program here.  Clean
     * up IceT and the other libraries we used. */
    icetDestroyContext(icetContext);

    glutDestroyWindow(winId);

    MPI_Finalize();

    exit(0);
    }
}

static void Draw()
{
  IceTInt rank, num_proc;

  /* We could get these directly from MPI, but it's just as easy to get them
   * from IceT. */
  icetGetIntegerv(ICET_RANK, &rank);
  icetGetIntegerv(ICET_NUM_PROCESSES, &num_proc);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* When changing the modelview matric in the draw function, you must be
   * wary of two things.  First, make sure the modelview matrix is restored
   * to what is was when the function is called.  Remember, the draw
   * function may be called multiple times and transformations may be
   * commuted.  Also, the bounds of the drawn geometry must be correctly
   * transformed before given to IceT.  IceT has no way of knowing about
   * transformations done here.  It is an error to change the projection
   * matrix in the draw function. */
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef((float)rank, 0, 0);
  glutSolidSphere(0.5, 100, 100);
  glPopMatrix();
}
