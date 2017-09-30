/*
 ******************************************************************
 * HISTORY
 * 15-Oct-94  Jeff Shufelt (js), Carnegie Mellon University
 *      Prepared for 15-681, Fall 1994.
 *
 ******************************************************************
 */

#include <stdio.h>
#include <pgmimage.h>
#include <backprop.h>

extern void exit();

#define TARGET_HIGH 0.9
#define TARGET_LOW 0.1


/*** This is the target output encoding for a network with one output unit.
     It scans the image name, and if it's an image of me (js) then
     it sets the target unit to HIGH; otherwise it sets it to LOW.
     Remember, units are indexed starting at 1, so target unit 1
     is the one to change....  ***/

load_target(img, net)
IMAGE *img;
BPNN *net;
{
  int scale, i;
  char userid[40], head[40], expression[40], eyes[40], photo[40];

  userid[0] = head[0] = expression[0] = eyes[0] = photo[0] = '\0';

  /*** scan in the image features ***/
  sscanf(NAME(img), "%[^_]_%[^_]_%[^_]_%[^_]_%d.%[^_]",
    userid, head, expression, eyes, &scale, photo);

  /* if (!strcmp(userid, "glickman"))*/
  /* if (!strcmp(eyes, "sunglasses"))*/ {
  /*   net->target[1] = TARGET_HIGH;*/  /* it's me, set target to HIGH */
  /* } else {*/
  /*  net->target[1] = TARGET_LOW;*/   /* not me, set it to LOW */
  /* }*/
  for (i = 1; i <=20; i++){
    net->target[i] = TARGET_LOW;
  }
  if (!strcmp(userid, "an2i")){
    net->target[1] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "at33")){ 
    net->target[2] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "boland")){ 
    net->target[3] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "bpm")){ 
    net->target[4] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "ch4f")){ 
    net->target[5] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "cheyer")){ 
    net->target[6] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "choon")){ 
    net->target[7] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "danieln")){ 
    net->target[8] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "glickman")){ 
    net->target[9] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "karyadi")){ 
    net->target[10] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "kawamura")){ 
    net->target[11] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "kk49")){ 
    net->target[12] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "megak")){ 
    net->target[13] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "mitchell")){ 
    net->target[14] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "night")){ 
    net->target[15] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "phoebe")){ 
    net->target[16] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "saavik")){ 
    net->target[17] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "steffi")){ 
    net->target[18] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "sz24")){ 
    net->target[19] = TARGET_HIGH;
  }
  else if (!strcmp(userid, "tammo")){ 
    net->target[20] = TARGET_HIGH;
  }
}
}


/***********************************************************************/
/********* You shouldn't need to change any of the code below.   *******/
/***********************************************************************/

load_input_with_image(img, net)
IMAGE *img;
BPNN *net;
{
  double *units;
  int nr, nc, imgsize, i, j, k;

  nr = ROWS(img);
  nc = COLS(img);
  imgsize = nr * nc;;
  if (imgsize != net->input_n) {
    printf("LOAD_INPUT_WITH_IMAGE: This image has %d pixels,\n", imgsize);
    printf("   but your net has %d input units.  I give up.\n", net->input_n);
    exit (-1);
  }

  units = net->input_units;
  k = 1;
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++) {
      units[k] = ((double) img_getpixel(img, i, j)) / 255.0;
      k++;
    }
  }
}
