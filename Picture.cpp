#include <iostream>
#include <stdio.h> 
#include <png.h> // libpng library - used for file handling of a png file
#include "Picture.h"

using namespace std;

/**
                       .-^-.
                     .'=^=^='.
                    /=^=^=^=^=\
            .-~-.  :^= HAPPY =^;
          .'~~*~~'.|^ EASTER! ^|
         /~~*~~~*~~\^=^=^=^=^=^:
        :~*~~~*~~~*~;\.-*))`*-,/
        |~~~*~~~*~~|/ * ((*   *'.
        :~*~~~*~~~*|   *))  *  * \
         \~~*~~~*~~| *  ((*   *  /
          `.~~*~~.' \  *))  *  .'
            `~~~`    '-.((*_.-'
**/

Picture::Picture() {}

/*  Function to open up a PNG picture file and read in its information. This met
 * hod takes in a character array as input to represent the name of the file tha
 * t is to be
opened (the input must be a character array since the libpng libra
 * ry is written in C and cannot handle strings). The function contains no outpu
 * t. It simply opens the PNG
file, and creates a Picture object based on the in
 * formation inside the file. 
 */
void Picture::readPNGFile(char * fileName) {
    
  // Create an array to read in the first 8 bits of a png file. The first 8 bits represents the header portion for a PNG picture.
  png_byte header[8];  //This will take the first 8 bytes of the file.  Libpng uses the first 8 bytes to determine if a file is a png.
  
  //Open a file based on the name passed in
  FILE * x = fopen(fileName, "rb");  //r opens a file for reading, b opens it in binary.
  
  //Read in the header signature of the PNG file
  fread(header, 1, 8, x); //fread(pointer to put stuff in, how many bytes per element, how many elements total, file pointer)
  


  // Check if the file opened is indeed a PNG file
  	/* png_sig_cmp is a library function that checks if a file opened is a PNG picture based on its header.
  	It returns 0 if the first 8 bits of the file indicate that the file is a PNG picture */
  if(!png_sig_cmp(header, 0, 8)) //png_sig_cmp returns false if file is png, true otherwise.  8 = number of bytes read, I dunno what 0 is but I think it will always be 0.
  {
    isPNG = true;  //for verification purposes.
    //ERROR - BREAK FROM CODE
    cout << "png_sig_cmp worked" << endl;  //This is for bug handling.
  }

  //Using the libpng library - two structures need to be initialized to read in the info of a PNG picture

  //Initialize the first struct to read in the PNG fule
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);  /*Initializing png_struct, one of the structures we need.*/
  
  //Check if the info structure has been properly created
  if(!png_ptr){  //checks to make sure png_struct was created.
    //ERROR - BREAK FROM CODE
    cout << "png_struct not made" << endl;
    return;
  }

  // Initalize the second structure to contain the information from the PNG picture
  info_ptr = png_create_info_struct(png_ptr);  /*Initializing png_info, which is the second structure.*/
  
  png_init_io(png_ptr, x);  //Passes file pointer to function that sets up input code.  Needs to use fread.
  
  png_set_sig_bytes(png_ptr, 8);  //png_sig_cmp took bytes from the start of the file, this gives them back to the file.
  //png_set_user_limits(png_ptr, width_max, heigth_max);  //Used to set limits on png input size.  It defaults to 1 million by 1 million.
  
  png_read_info(png_ptr, info_ptr);  //Reads all file information except actual image data.
  // if(color_type == PNG_COLOR_TYPE_PALETTE){  //Expands paletted images to RGB?
  //   png_set_palette_to_rgb(png_ptr);
  // }
 

  // Once the picture information has been read into the program - a Picture object can be established based on the information from the picture
  width = png_get_image_width(png_ptr, info_ptr);  //Gets image width
  height = png_get_image_height(png_ptr, info_ptr);  //Gets image height
  color_type = png_get_color_type(png_ptr, info_ptr);  //Color type is the type of color.  (ex. RGB, greyscale, etc.)
  bit_depth = png_get_bit_depth(png_ptr, info_ptr);  /*Bit depth is the amount of bits needed to contain information
  about 1 pixel of an image.  RGB is normally 8.*/

  /* Perform several checks to ensure that the picture information is of RGB format */

  // Check if the bit depth needs to be altered so that it is compatable with RGB

  if(bit_depth == 16){ //Changes anything with bit depth of 16 to RGB.
    png_set_strip_16(png_ptr);
  }

  // Check if the color type needs to be altered from pallette to RGB
  if(color_type == PNG_COLOR_TYPE_PALETTE)  //Changes color_type from palette to rgb.
   png_set_palette_to_rgb(png_ptr);

  // Check if color type is of grayscale format and has less than 8 bits - in which case, the bit count needs to be expanded
  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8){  //Changes grayscale images with less than 8 bits to 8 bits.
   png_set_expand_gray_1_2_4_to_8(png_ptr);
  }


  // Check if the image contains transparency information - which case, an alpha channel needs to be added
  if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)){  //If image has transparency info, adds alpha channel.
    png_set_tRNS_to_alpha(png_ptr);
  }

   
  if(color_type == PNG_COLOR_TYPE_RGB ||
     color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_PALETTE){
       png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
  }


  // Check if the picture is of grayscale format - in which case, it needs to be adjusted to RGB format
  if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA){
    png_set_gray_to_rgb(png_ptr);
  }

  // Update picture structs now that all checks have been performed to ensure that the picture is in RGB format
  png_read_update_info(png_ptr, info_ptr);


  if(width != 0 && height != 0){  //prints image width and height.
    cout << "Width is " << width << "!" << endl;
    cout << "Height is " << height << "!" << endl;
  }


  /* Officially begin to read in the raw picture data from the file */
  	// Allocate memory for the picture data based on its size
  	// Dynamically establish a two-dimensional array to hold each pixel in the file
  //READING IT FINALLY OMG
  row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
  for (int y=0; y<height; y++)
          row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

  try{
    png_read_image(png_ptr, row_pointers);  //Actually reading the image.
  }catch(...){
    cout << "png_read_image did not work" << endl;
    readTrue = false;
  }

  if(readTrue){
    cout << "png_read_image worked" << endl;
  }

  fclose(x);
} 

/*  This function opens up a file and writes a PNG file to it. The method takes 
 * in a character array as input that will be used to represent the file name (a
 *  character
array must be used instead of a string because libpng is written i
 * n C and cannot function with strings). The function contains no ouutput. 
 */
    void Picture::writePNGFile(char * fileName) {
    
    // Open up a new file that will be used to write a PNG picture to
    //http://www.libpng.org/pub/png/book/chapter15.html was of especial use.
    FILE * output = fopen(fileName, "wb"); //Creates new file to write to.
    
/*Two structures need to be created as part of the libpng library. One structure will be used to hold the information from the picture and the
	other structure will be used to write the information to the file.*/

	// Establish the struct for writing the PNG image


    png_ptr       = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); //Creates png and info struct for writing.
    
    // Establish the struct for handling the information of the PNG image
    info_ptr      = png_create_info_struct(png_ptr);
    

    png_init_io(png_ptr, output); //Initializes output.
    
    // Set basic parameters for the image
    png_set_IHDR(
        png_ptr,
        info_ptr,
        width,
        height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    ); //Sets basic image parameters.
    
    png_write_info(png_ptr, info_ptr); //Writes all non-image data to file.
    
    cout << "got up to png_write_image" << endl;
    
    png_write_image(png_ptr, row_pointers); //Writes image data to file.
    
    cout << "got up to png_write_end" << endl;
    
    png_write_end(png_ptr, NULL); //Finishes writing file.  Change NULL to the info pointer if you want comments or time.
    
    cout << "got up to deleting array" << endl;
    
    // for(int i = 0; i < height; i++){  Deletes 2d array so no memory leak.  This
    // currently causes the program to crash.   delete [] row_pointers[i]; } delete
    // [] row_pointers;
    
    cout << "got up to fclose()" << endl;
    fclose(output);

    cout << "well, it didn't crash" << endl;
}

void Picture::convertToHSV(
    int r,
    int g,
    int b,
    double & hue,
    double & sat,
    double & val
) {
    double RGB[3] = {
        b / 255.0,
        g / 255.0,
        r / 255.0
    };
    float min, max, diff;

    //find min / max
    min  = RGB[0] < RGB[1]
        ? RGB[0]
        : RGB[1];
    min  = min < RGB[2]
        ? min
        : RGB[2];

    max  = RGB[0] > RGB[1]
        ? RGB[0]
        : RGB[1];
    max  = max > RGB[2]
        ? max
        : RGB[2];

    //find value
    val  = max;

    //find hue and sat
    diff = max - min;
    if (diff < 0.00001) {
        sat = 0;
        hue = 0;
        return;
    }

    if (max > 0) {
        sat = diff / max;
    } else {
        sat = 0;
        hue = 0;
        return;
    }

    if (RGB[0] == max) {
        hue = (RGB[1] - RGB[2]) / diff;
    } else if (RGB[1] == max) {
        hue = 2.0 + (RGB[3] - RGB[0]) / diff;
    } else {
        hue = 4.0 + (RGB[0] - RGB[1]) / diff;
    }

    //clamp hue
    hue *= 60;
    if (hue < 0) {
        hue += 360;
    }

}

void Picture::convertToRGB(
    double hue,
    double sat,
    double val,
    int & r,
    int & g,
    int & b
) {

    if (sat == 0.0) {
        r = val * 255;
        g = val * 255;
        b = val * 255;
        return;
    }

    double sector = hue;
    if (sector >= 360) {
        sector = 0.0;
    }
    sector     /= 60.0;
    long rem   = (long)sector;
    double REM = sector - rem;

    double p   = val * (1.0 - sat) * 255;
    double q   = val * (1.0 - (sat * REM)) * 255;
    double t   = val * (1.0 - (sat * (1.0 - REM))) * 255;
    val        *= 255;
    p          += 0.5;
    q          += 0.5;
    t          += 0.5;
    val        += 0.5;

    switch (rem) {
        case 0:
            r = val;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = val;
            b = p;
            break;
        case 2:
            r = p;
            g = val;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = val;
            break;
        case 4:
            r = t;
            g = p;
            b = val;
            break;
        case 5:
        default:
            r = val;
            g = p;
            b = q;
            break;
    }

    printf("%i, %i, %i\n", r, g, b);
}

//HUEVAL CAN GO FROM 0 -> 360.
void Picture::changeHue(int hueVal) {
  //restricts input to a closed interval (slider)
  if(hueVal > 360 || hueVal < -359){
    cout << "INVALID" << endl;
    return;
  }
  int r, g, b;
  double h, s, v;
  for (int y = 0; y < height; y++) {
    png_byte* row = row_pointers[y];
    for (int x = 0; x < width; x++) {
      png_byte* ptr = &(row[x*4]);
      r = ptr[0];
      g = ptr[1];
      b = ptr[2];
      convertToHSV(r, g, b, h, s, v);
      h = h + (double)hueVal;
      //fixes the max and min values of the possible hue. anything that may go over is stopped exactly at the max.
      if(h > 360.0)
        h = 359.0;
      if(h < 0)
        h = 0;
      convertToRGB(h, s, v, r, g, b);
      ptr[0] = r;
      ptr[1] = g;
      ptr[2] = b;
    }
  }
}

//Can only go from 0 to 1, the value is a percentage so must be inbetween -1 and 1.
void Picture::changeSat(double satVal) {
  if(satVal > 1 || satVal < -1){
    cout << "INVALID" << endl;
    return;
  }
  int r, g, b;
  double h, s, v;
  for (int y = 0; y < height; y++) {
    png_byte* row = row_pointers[y];
    for (int x = 0; x < width; x++) {
      png_byte* ptr = &(row[x*4]);
      r = ptr[0];
      g = ptr[1];
      b = ptr[2];
      convertToHSV(r, g, b, h, s, v);
      s = s + satVal;
      //anything set over the possible max is made to be exactly at the min/max, which in this case is 0 and 1.
      if(s > 1.0)
        s = 1.0;
      if(s < 0)
        s = 0;
      convertToRGB(h, s, v, r, g, b);
      ptr[0] = r;
      ptr[1] = g;
      ptr[2] = b;
    }
  }
}

//Adjusts brightness of image.Likewise to saturation, value can only go from 0 to 1 so input must be restricted from -1 to 1.
void Picture::changeBright(double brightVal) {
  if(brightVal > 1 || brightVal < -1){
    cout << "INVALID" << endl;
    return;
  }
  int r, g, b;
  double h, s, v;
  for (int y = 0; y < height; y++) {
    png_byte* row = row_pointers[y];
    for (int x = 0; x < width; x++) {
      png_byte* ptr = &(row[x*4]);
      r = ptr[0];
      g = ptr[1];
      b = ptr[2];
      convertToHSV(r, g, b, h, s, v);
      double factor = v;
      if(factor > 0.5){
		  factor = 0.5 - factor;
	  }
	  factor /= 0.5;
      v = v + (brightVal * factor);
      //anything set over the possible max is made to be exactly at the max instead of over/under the max.
      if(v > 1.0)
        v = 1.0;
      if(v <= 0.0)
        v = 0.0;
      convertToRGB(h, s, v, r, g, b);
      ptr[0] = r;
      ptr[1] = g;
      ptr[2] = b;
    }
  }
}

void Picture::changeExposure(double expoVal) {
  if(brightVal > 1 || brightVal < -1){
    cout << "INVALID" << endl;
    return;
  }
  int r, g, b;
  double h, s, v;
  for (int y = 0; y < height; y++) {
    png_byte* row = row_pointers[y];
    for (int x = 0; x < width; x++) {
      png_byte* ptr = &(row[x*4]);
      r = ptr[0];
      g = ptr[1];
      b = ptr[2];
      convertToHSV(r, g, b, h, s, v);
      v = v + expoVal;
      //anything set over the possible max is made to be exactly at the max instead of over/under the max.
      if(v > 1.0)
        v = 1.0;
      if(v <= 0.0)
        v = 0.0;
      convertToRGB(h, s, v, r, g, b);
      ptr[0] = r;
      ptr[1] = g;
      ptr[2] = b;
    }
  }
}

void Picture::changeHighlights(double value) {
  if(brightVal > 1 || brightVal < -1){
    cout << "INVALID" << endl;
    return;
  }
  int r, g, b;
  double h, s, v;
  for (int y = 0; y < height; y++) {
    png_byte* row = row_pointers[y];
    for (int x = 0; x < width; x++) {
      png_byte* ptr = &(row[x*4]);
      r = ptr[0];
      g = ptr[1];
      b = ptr[2];
      convertToHSV(r, g, b, h, s, v);
      v = v + (value * v);
      //anything set over the possible max is made to be exactly at the max instead of over/under the max.
      if(v > 1.0)
        v = 1.0;
      if(v <= 0.0)
        v = 0.0;
      convertToRGB(h, s, v, r, g, b);
      ptr[0] = r;
      ptr[1] = g;
      ptr[2] = b;
    }
  }
}

void Picture::changeShadows(double value) {
  if(brightVal > 1 || brightVal < -1){
    cout << "INVALID" << endl;
    return;
  }
  int r, g, b;
  double h, s, v;
  for (int y = 0; y < height; y++) {
    png_byte* row = row_pointers[y];
    for (int x = 0; x < width; x++) {
      png_byte* ptr = &(row[x*4]);
      r = ptr[0];
      g = ptr[1];
      b = ptr[2];
      convertToHSV(r, g, b, h, s, v);
      v = v + (value(1-v));
      //anything set over the possible max is made to be exactly at the max instead of over/under the max.
      if(v > 1.0)
        v = 1.0;
      if(v <= 0.0)
        v = 0.0;
      convertToRGB(h, s, v, r, g, b);
      ptr[0] = r;
      ptr[1] = g;
      ptr[2] = b;
    }
  }
}

void Picture::changeTemp(int value){
  if(value > 255 || value < -255){
    //cout << "Error" << endl;
    return;
  }
  int r, g, b;
  for (int y = 0; y < height; y++) {
    png_byte* row = row_pointers[y];
    for (int x = 0; x < width; x++) {
      png_byte* ptr = &(row[x*4]);
      ptr[0] = clamp(ptr[0] + value);
      ptr[2] = clamp(ptr[2] - value);
    }
  }
}

//this method checks if the color values are between 0 and 255
int Picture::clamp(int x)
{
  if (x < 0)
  {
    return 0;
  }
  else if (x > 255)
  {
    return 255;
  }
  else
  {
    return x;
  }
}


//this method will adjust the contrast
//int c is to be between the values of -255 to 255. "-" reduces contrast,
// "+" increases it

//This is what I found on github about a contrast method. It had a couple errors so I think I fixed them (hopefully without messing up the logic
//However I don't think that c should be allowed to go up to 255, since it messes up the image pretty badly after around 50 (as you might see if you try it yourself)
//Don't even get me started on what happens when you set c to 200 (my eyes!)
void Picture::adjustContrast(int c){
  double cFactor = (259.0*((double)c + 255.0)) / (255.0*(259.0 - (double)c));
  int r, g, b;
  for (int y = 0; y < height; y++) {
    png_byte* row = row_pointers[y];
    for (int x = 0; x < width; x++) {
      png_byte* ptr = &(row[x*4]);
      r = ptr[0];
      g = ptr[1];
      b = ptr[2];

      r = (int)(cFactor*(r - 128.0) + 128);
      g = (int)(cFactor*(g - 128.0) + 128);
      b = (int)(cFactor*(b - 128.0) + 128);

      ptr[0] = r;
      ptr[1] = g;
      ptr[2] = b;

    }
  }
}

int main() {
    char fileName[100]; //file name is put in here.  Must be char * because libpng is in c.
    cout << "Please enter a file name to open: ";
    cin >> fileName;
    Picture pleaseGodWork;
    pleaseGodWork.readPNGFile(fileName);
    cout << "Please enter a file name to save the file: ";
    cin >> fileName;
    pleaseGodWork.writePNGFile(fileName);
    return 0;
}
