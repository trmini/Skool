struct listtype
 {
  int x[100];
  int currentsize;
 };

void square_cell(listtype&);
int min_pos(listtype);
void move_down(listtype&);

void square_cell(listtype&list)
 {
  int index=0;
  for (;index<list.currentsize;index++) list.x[index]=index*index;
  return;
 }

int min_pos(listtype list)
 {
  int index=1,index_min=0;
  int min;
  min=list.x[0];
  for (;index<list.currentsize;index++)
   {
    if (min>list.x[index])
     {
      index_min=index;
      min=list.x[index_min];
     }
   }
  return index_min;
 }

void move_down(listtype&list)
 {
  int index=0;
  list.x[0]=list.x[list.currentsize-1];
  for (;index<(list.currentsize-1);index++) list.x[index]=list.x[index+1];
  return;
 }