#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>
#include <time.h>

int getNFromArguments(int argc, char **argv);
int getRPositionForPixel(int x, int y, int width);
int getGPositionForPixel(int x, int y, int width);
int getBPositionForPixel(int x, int y, int width);
int resolveDestinationForStep(int step);
int compute_julia_pixel(int x, int y, int width, int height, float tint_bias, unsigned char *rgb);
int write_bmp_header(FILE *f, int width, int height);


int size, rank;
int processesPerRow, rowsPerProcess;

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    processesPerRow = sqrt(size);
    int processY = rank/processesPerRow;
    int processX = rank - (processY * processesPerRow);

    int height = getNFromArguments(argc, argv);
    int width = height*2;

    rowsPerProcess = height/sqrt(size);
    int firstRow = processY * rowsPerProcess;
    int lastRow = ((processY+1) * rowsPerProcess) - 1;

    int columnsPerProcess = rowsPerProcess*2;
    int firstColumn = processX * columnsPerProcess;
    int lastColumn = ((processX+1) * columnsPerProcess) - 1;

    int localWidth = lastColumn-firstColumn+1;
    int localHeight = lastRow-firstRow+1;

    // printf("I am process %d of %d. My tile coordinates are (%d,%d). I will process [%d:%d, %d:%d]\n", 
    //     rank, size-1, processX, processY, firstRow, lastRow, firstColumn, lastColumn);

    char* juliaElements = malloc(sizeof(char) * 3 * localWidth * localHeight);
    char* rgb = malloc(sizeof(char) * 3);

    struct timeval start, end;
    gettimeofday(&start, NULL);    int x, y;
    for (y=0; y<localHeight; y++) {
        for (x=0; x<localWidth; x++) {
            int globalX = processX * columnsPerProcess + x;
            int globalY = processY * rowsPerProcess + y;
            compute_julia_pixel(globalX, globalY, width, height, rank*rank, rgb);
            juliaElements[getRPositionForPixel(x, y, localWidth)] = rgb[0];
            juliaElements[getGPositionForPixel(x, y, localWidth)] = rgb[1];
            juliaElements[getBPositionForPixel(x, y, localWidth)] = rgb[2];
        }
    }
    gettimeofday(&end, NULL);
    printf("[%d] - took %ld ms\n", rank, ((end.tv_sec*1000000+end.tv_usec)-(start.tv_sec*1000000+start.tv_usec)));

    // printf("********** %d finished calculating **********\n", rank);

    if (rank == 0) {
        FILE *outputFile = fopen("output.bmp", "wb");
        write_bmp_header(outputFile, width, height);
        fwrite(juliaElements, sizeof(char)*3, localWidth, outputFile);
        fclose(outputFile);
        int nextStep = 1;
        // printf("Sending to rank %d...\n", resolveDestinationForStep(nextStep));
        MPI_Send(&nextStep, 1, MPI_INT, resolveDestinationForStep(nextStep), 0, MPI_COMM_WORLD);
        int rowsWritten = 1;
        while (rowsWritten < rowsPerProcess) {
            int currentStep;
            MPI_Status status;
            MPI_Recv(&currentStep, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,MPI_COMM_WORLD, &status);
            outputFile = fopen("output.bmp", "ab");
            fwrite(&juliaElements[rowsWritten*localWidth*3], sizeof(char)*3, localWidth, outputFile);
            fclose(outputFile);
            nextStep = currentStep + 1;
            // printf("Sending to rank %d...\n", resolveDestinationForStep(nextStep));
            MPI_Send(&nextStep, 1, MPI_INT, resolveDestinationForStep(nextStep), 0, MPI_COMM_WORLD);
            rowsWritten++;
        }
    } else {
        int rowsWritten = 0;
        while (rowsWritten < rowsPerProcess) {
            int currentStep;
            MPI_Status status;
            MPI_Recv(&currentStep, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            FILE *outputFile = fopen("output.bmp", "ab");
            fwrite(&juliaElements[rowsWritten*localWidth*3], sizeof(char)*3, localWidth, outputFile);
            fclose(outputFile);
            rowsWritten++;
            if(rowsWritten >= rowsPerProcess && rank == size-1) {
                break;
            } else {
                int nextStep = currentStep + 1;
                // printf("Sending to rank %d...\n", resolveDestinationForStep(nextStep));
                MPI_Send(&nextStep, 1, MPI_INT, resolveDestinationForStep(nextStep), 0, MPI_COMM_WORLD);
            }
        }
    }
    MPI_Finalize();
}

int getNFromArguments(int argc, char **argv) {
    if (argc != 2) {
        printf("Number of arguments must be 2!");
        exit;
    }
    return atoi(argv[1]);
}

int getRPositionForPixel(int x, int y, int width) { return (x*3) + (y*width*3); }
int getGPositionForPixel(int x, int y, int width) { return (x*3 + 1) + (y*width*3); }
int getBPositionForPixel(int x, int y, int width) { return (x*3 + 2) + (y*width*3); }

int resolveDestinationForStep(int step) {
    int processX = step % processesPerRow;
    int processY = step / rowsPerProcess / processesPerRow;
    return processY * processesPerRow + processX;
}

int compute_julia_pixel(int x, int y, int width, int height, float tint_bias, unsigned char *rgb) {

  // Check coordinates
  if ((x < 0) || (x >= width) || (y < 0) || (y >= height)) {
    fprintf(stderr,"Invalid (%d,%d) pixel coordinates in a %d x %d image\n", x, y, width, height);
    return -1;
  }

  // "Zoom in" to a pleasing view of the Julia set
  float X_MIN = -1.6, X_MAX = 1.6, Y_MIN = -0.9, Y_MAX = +0.9;
  float float_y = (Y_MAX - Y_MIN) * (float)y / height + Y_MIN ;
  float float_x = (X_MAX - X_MIN) * (float)x / width  + X_MIN ;

  // Point that defines the Julia set
  float julia_real = -.79;
  float julia_img = .15;

  // Maximum number of iteration
  int max_iter = 300;

  // Compute the complex series convergence
  float real=float_y, img=float_x;
  int num_iter = max_iter;
  while (( img * img + real * real < 2 * 2 ) && ( num_iter > 0 )) {
    float xtemp = img * img - real * real + julia_real;
    real = 2 * img * real + julia_img;
    img = xtemp;
    num_iter--;
  }

  // Paint pixel based on how many iterations were used, using some funky colors
  float color_bias = (float) num_iter / max_iter;
  rgb[0] = (num_iter == 0 ? 200 : - 500.0 * pow(tint_bias, 1.2) *  pow(color_bias, 1.6));
  rgb[1] = (num_iter == 0 ? 100 : -255.0 *  pow(color_bias, 0.3));
  rgb[2] = (num_iter == 0 ? 100 : 255 - 255.0 * pow(tint_bias, 1.2) * pow(color_bias, 3.0));

  return 0;
}

int write_bmp_header(FILE *f, int width, int height) {

  unsigned int row_size_in_bytes = width * 3 + 
	  ((width * 3) % 4 == 0 ? 0 : (4 - (width * 3) % 4));

  // Define all fields in the bmp header
  char id[2] = "BM";
  unsigned int filesize = 54 + (int)(row_size_in_bytes * height * sizeof(char));
  short reserved[2] = {0,0};
  unsigned int offset = 54;

  unsigned int size = 40;
  unsigned short planes = 1;
  unsigned short bits = 24;
  unsigned int compression = 0;
  unsigned int image_size = width * height * 3 * sizeof(char);
  int x_res = 0;
  int y_res = 0;
  unsigned int ncolors = 0;
  unsigned int importantcolors = 0;

  // Write the bytes to the file, keeping track of the
  // number of written "objects"
  size_t ret = 0;
  ret += fwrite(id, sizeof(char), 2, f);
  ret += fwrite(&filesize, sizeof(int), 1, f);
  ret += fwrite(reserved, sizeof(short), 2, f);
  ret += fwrite(&offset, sizeof(int), 1, f);
  ret += fwrite(&size, sizeof(int), 1, f);
  ret += fwrite(&width, sizeof(int), 1, f);
  ret += fwrite(&height, sizeof(int), 1, f);
  ret += fwrite(&planes, sizeof(short), 1, f);
  ret += fwrite(&bits, sizeof(short), 1, f);
  ret += fwrite(&compression, sizeof(int), 1, f);
  ret += fwrite(&image_size, sizeof(int), 1, f);
  ret += fwrite(&x_res, sizeof(int), 1, f);
  ret += fwrite(&y_res, sizeof(int), 1, f);
  ret += fwrite(&ncolors, sizeof(int), 1, f);
  ret += fwrite(&importantcolors, sizeof(int), 1, f);

  // Success means that we wrote 17 "objects" successfully
  return (ret != 17);
}