#include <stdio.h>
#include <math.h>

void main(void)
{
 int initial_roaches,week;
 long roaches;
 float breeding_rate;

 printf("Roaches at beginning of week: ");
 scanf("%i",&initial_roaches);
 printf("Roaches at end of week: ");
 scanf("%l",&roaches);
 breeding_rate=(roaches-initial_roaches)/(float)initial_roaches;
 week=2;
 printf("Week          Roaches\n");
 printf(" %i           %i\n",week,roaches);
 do
  {
   roaches=(floor(roaches*breeding_rate)+roaches);
   week++;
   printf(" %i           %i\n",week,roaches);
  }
 while (roaches<=1000000);
 return;
}