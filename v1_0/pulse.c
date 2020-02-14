#include <stdio.h>
#include <unistd.h>
#include "summa.h"
#define MaxWord 255
#define step 6.4
// time beatwean points 6.4 us
void exit(int);
void getData(uint8_t cr,uint8_t nr,uint8_t adcCh,uint8_t moduleN)
{
  unsigned int hh,i,j,buf,num;
  unsigned int k=adcCh ;
  double time=0;
  uint16_t d,ddd,g[16];
  FILE *fout ;
  unsigned char fnames[80];
  sprintf(fnames,"data//m%d_ch%d.dat",moduleN,adcCh);
  fout=fopen(fnames,"w") ;
//  clrscr() ;
  ccamac(cr,nr,0,9) ;
  ccamac(cr,nr,1,9) ;
  ddd=0;
//  fprintf(fout,"\nNum=%d  ch=%d\n",nr,k) ;
  for(i=0;i<=17;i++)
  {
    hh=rcam16(cr,nr,0,0,&d);
    if(hh!=0) { printf("\nSumma error") ; exit(1) ; }
  }
  j=0 ;
// ccamac(cr,nr,0,9) ;
// ccamac(cr,nr,1,9) ;
  
  while(j<MaxWord)
  {
    ddd=0;
    for(i=0;i<16;i++)
    {
      ddd<<=1 ;
//      delay(1) ;
      hh=rcam16(cr,nr,0,0,&d);
	if(hh!=0) { printf("\nSumma error %x",0xff&hh) ; exit(1) ; }
      buf=(d>>k)&(1) ;
      ddd|=buf ;
    }
    ddd>>=1 ; ddd>>=1;
//    delay(1) ;
    g[k]=ddd&0xfff ;
//    fprintf(fout,"\n%3u  ",j) ;
//    fprintf(fout," %d %6.3lf ",g[k],((g[k]) & 0xfff)*5/4095.0-5) ;
      fprintf(fout,"%-6.1lf %6.3lf\n",time,((g[k]) & 0xfff)*5/4095.0-5) ;
      time+=step;
    j++;
  }
  fclose(fout) ;

}
int main(int argc,char *argv[])
{
  uint8_t cr=1,nr=16,adcCh=0,moduleN=1;
  uint16_t i,d;
  int opt;
  while ((opt = getopt(argc, argv, "c:n:a:m:")) != -1) {
    switch(opt) {
    case 'c':
      cr = atoi(optarg);
      printf("Crate=%d\n", cr);
    break;
    case 'n':
      nr = atoi(optarg);
      printf("Number=%d\n", nr);
    break;
    case 'a':
      adcCh = atoi(optarg);
      printf("Test ADC ch=%d\n", adcCh);
    break;
    case 'm':
      moduleN = atoi(optarg);
      printf("Module index=%d\n", moduleN);
    break;
    case '?':
    /* Case when user enters the command as
     * $ ./cmd_exe -i
     */
      if (optopt == 'c') {
      printf("\nMissing mandatory input option");
      /* Case when user enters the command as
       * # ./cmd_exe -o
       */
       } 
       else if (optopt == 'n') {
         printf("\nMissing mandatory output option");
       } 
       else if (optopt == 'a') {
         printf("\nMissing mandatory output option");
       } 
//       else if (optopt == 'm') {
//         printf("\nMissing mandatory output option");
//       } 
       else {
         printf("\nInvalid option received");
       }
    break;
    }
  } 
  
  printf("Begin meas\n");
  if(init_K331(cr)) { printf("Error open K331. Program was terminated\n"); exit(0);}
  uint8_t n=7,newc=0;
  uint16_t t=0,cy=0;
  d=0;
  while(1){
    rcam16(cr,7,0,0,&d);
    if(d) { 
      if((d==1)&&(newc==0)) { newc=1;  ccamac(cr,nr,0,9); printf("INT %d\n",d);}
      if((d==2)&&(newc==1)){
        printf("INT %d\n",d);
        sleep(1);
        getData(cr,nr,adcCh,moduleN);
        exit(0);
      }
    }
  }
/*  while(1){
    rcam16(cr,7,0,0,&d);
    if(d) { 
      if(newc) { cy++; newc=0; }
      printf("INT %d in cycle %d %d\n",d,cy,t++);
      sleep(2);
      if(cy==1) ccamac(cr,nr,0,9);
      if(cy==2){
        getData(cr,nr,adcCh,moduleN);
        exit(0);
      }
    }
    else {
      t=0;newc=1;
    }
  }
*/  
}
