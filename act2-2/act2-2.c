/*correr el programa
gcc -fopenmp nombre.c
a.exe >> prueba.txt
*/

#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 200 // 4 o 20

unsigned char* image_gray;
unsigned char* image_gray_flipped;
unsigned char original_image[54];

long ancho, alto;
int padding, n;

void blurring(int num, int flipped, int matrix_size){
   printf("Empezando %d\n", num);
   FILE *outputImage;
   int middle = matrix_size / 2;
   int weight = matrix_size * matrix_size;
   //printf("%d\t", weight);
   int sum = 0;
   char str[5];
   sprintf(str, "%d", num);
   if (flipped == 0){
      outputImage = fopen(strcat(str,"_normal.bmp"),"wb");
      for(int i=0; i<54; i++) 
         fputc(original_image[i], outputImage);   //Copia cabecera a nueva imagen
      for(int k = 0; k < alto; k++){
         for(int j = 0; j < ancho; j++){
            sum = 0;
            for(int m = 0; m < matrix_size; m++)
               for(int l = 0; l < matrix_size; l++){
                  int index = 3*(k+m-middle)*ancho + (k+m-middle)*padding + 3*(ancho-1) - 3*j - 3*middle + 3*l;
                  if(index > 0 && index < n){
                     sum += *(image_gray + index);
                  }
               }
            fputc((unsigned char)(sum/weight), outputImage);
            fputc((unsigned char)(sum/weight), outputImage);
            fputc((unsigned char)(sum/weight), outputImage);
         }
         for(int i = 0; i<padding; i++)
            fputc(0, outputImage);
      }
   }else{ 
      outputImage = fopen(strcat(str,"_horizontal.bmp"),"wb");
      for(int i=0; i<54; i++) 
         fputc(original_image[i], outputImage);   //Copia cabecera a nueva imagen
      for(int k = 0; k < alto; k++){
         for(int j = 0; j < ancho; j++){
            sum = 0;
            for(int m = 0; m < matrix_size; m++)
               for(int l = 0; l < matrix_size; l++){
                  int index = 3*(k+m-middle)*ancho + (k+m-middle)*padding + 3*(ancho-1) - 3*j - 3*middle + 3*l;
                  if(index > 0 && index < n){
                     sum += *(image_gray_flipped + index);
                  }
               }
            weight=matrix_size*matrix_size;
            // printf("%d/%d=%d\t", sum, weight, sum/(matrix_size*matrix_size));
            fputc((unsigned char)(sum/weight), outputImage);
            fputc((unsigned char)(sum/weight), outputImage);
            fputc((unsigned char)(sum/weight), outputImage);
         }
         for(int i = 0; i<padding; i++)
            fputc(0, outputImage);
      }
   }
   fclose(outputImage);
}

int main(){
   char image_name[] = "daftypunk3";
   FILE *image;

   image = fopen(strcat(image_name,".bmp"),"rb");          //Imagen original a transformar

   unsigned char r, g, b;               //Pixel

   for(int i=0; i<54; i++) {
      //fputc(fgetc(image), outputImage);
      original_image[i] = fgetc(image);
      // fputc(original_image[i], outputImage);   //Copia cabecera a nueva imagen
   }
   
   
   ancho = (long)original_image[20]*65536 + (long)original_image[19]*256 + (long)original_image[18];
   alto = (long)original_image[24]*65536 + (long)original_image[23]*256 + (long)original_image[22];
   padding = ancho%4;

   unsigned char* image_input = (unsigned char*) malloc((ancho*alto*3+alto*padding)*sizeof(unsigned char));
   image_gray = (unsigned char*) malloc((ancho*alto*3+alto*padding)*sizeof(unsigned char));
   image_gray_flipped = (unsigned char*) malloc((ancho*alto*3+alto*padding)*sizeof(unsigned char));


   printf("Padding= %d\n",padding);
   printf("Ancho= %ld\nAlto= %ld\n", ancho, alto);

   omp_set_num_threads(NUM_THREADS);
   n = (3*(int)alto*(int)(ancho)) + (padding*(int)alto);

   printf("n= %d\n", n);

   const double startTime = omp_get_wtime();
   #pragma omp parallel 

     #pragma omp single
         for(int i = 0; i<n; i++)
            *(image_input + i) = fgetc(image);
      
     #pragma for collapse(2)
         for(int k = 0; k < alto; k++)
            for(int j = 0; j < ancho; j++){
               int pixel = *(image_input + 3*k*ancho + k*padding + 3*j +2)*0.21+ *(image_input + 3*k*ancho + k*padding + 3*j + 1)*0.72 + *(image_input + 3*k*ancho + k*padding + 3*j)*0.07;
               *(image_gray + 3*k*ancho + k*padding + 3*j) = pixel;
               *(image_gray_flipped + 3*k*ancho + k*padding + 3*(ancho-1) - 3*j) = pixel;
               *(image_gray + 3*k*ancho + k*padding + 3*j + 1) = pixel;
               *(image_gray_flipped + 3*k*ancho + k*padding + 3*(ancho-1) - 3*j + 1) = pixel;
               *(image_gray + 3*k*ancho + k*padding + 3*j + 2) = pixel;
               *(image_gray_flipped + 3*k*ancho + k*padding + 3*(ancho-1) - 3*j + 2) = pixel;
            }
     #pragma for collapse(2)
         for(int k = 0; k< alto; k++)
            for(int l=0; l<padding; l++){
               *(image_gray + (3*k*ancho) + (k*padding)+l+(ancho*3)) = 0;
               *(image_gray_flipped + (3*k*ancho) + (k*padding)+l+(ancho*3)) = 0;
            }

      printf("Empezando sections\n");
      // for(int i = 0; i<n; i++)
      //    printf("%d\t", *(image_gray_ + i));

      #pragma sections
         // blurring(0, 1, 3);
         blurring(0, 0, 1);
         #pragma section
            blurring(1, 0, 5);
         #pragma section
            blurring(2, 0, 9);
         #pragma section
            blurring(3, 0, 13);
         #pragma section
            blurring(4, 0, 17);
         #pragma section
            blurring(5, 0, 21);
         #pragma section
            blurring(6, 0, 25);
         #pragma section
            blurring(7, 0, 29);
         #pragma section
            blurring(8, 0, 33);
         #pragma section
            blurring(9, 0, 37);
         #pragma section
            blurring(9, 1, 37);
         #pragma section
            blurring(8, 1, 33);
         #pragma section
            blurring(7, 1, 29);
         #pragma section
            blurring(6, 1, 25);
         #pragma section
            blurring(5, 1, 21);
         #pragma section
            blurring(4, 1, 17);
         #pragma section
            blurring(3, 1, 13);
         #pragma section
            blurring(2, 1, 9);
         #pragma section
            blurring(1, 1, 5);
         #pragma section
            blurring(0, 1, 1);


   const double endTime = omp_get_wtime();
	printf("Ha demorado (%lf) segundos en ejecutar.\n", (endTime - startTime));
   fclose(image);
   return 0;

}