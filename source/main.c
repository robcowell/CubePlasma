#include <grrlib.h>
#include <stdlib.h>
#include <math.h>
#include <ogcsys.h>

#define PI 3.14159265358979323846 // Define the value of PI


// Linear interpolation between two colors
u32 lerpColor(u32 color1, u32 color2, float t) {
    u8 r1 = (color1 >> 24) & 0xFF;
    u8 g1 = (color1 >> 16) & 0xFF;
    u8 b1 = (color1 >> 8) & 0xFF;
    u8 a1 = color1 & 0xFF;
    u8 r2 = (color2 >> 24) & 0xFF;
    u8 g2 = (color2 >> 16) & 0xFF;
    u8 b2 = (color2 >> 8) & 0xFF;
    u8 a2 = color2 & 0xFF;
    u8 r = (u8)(r1 * (1.0f - t) + r2 * t);
    u8 g = (u8)(g1 * (1.0f - t) + g2 * t);
    u8 b = (u8)(b1 * (1.0f - t) + b2 * t);
    u8 a = (u8)(a1 * (1.0f - t) + a2 * t);
    return (r << 24) | (g << 16) | (b << 8) | a;
}

float map(float value, float inMin, float inMax, float outMin, float outMax){
  return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

// Define a function to calculate the value of a sine wave at a given x position.
float sineWave(float x, float amplitude, float frequency) {
  return amplitude * sin(2.0f * PI * frequency * x);
}

float generatePlasma(float x, float y) {
    float noise = 0.0f;

    for (int oct = 0; oct < 4; oct++) {
        float freq = pow(2.0f, oct);
        float amp = pow(0.5f, oct);

        noise += amp * sin(freq * x + y);
    }

    return noise;
}

void HSBtoRGB(float hue, float saturation, float brightness, int* red, int* green, int* blue) {
  if(saturation == 0) {
    *red = *green = *blue = (int)(brightness * 255 + 0.5);
  } else {
    float huePrime = fmod(hue, 360) / 60;
    int i = (int)floor(huePrime);
    float f = huePrime - i;
    float p = brightness * (1 - saturation);
    float q = brightness * (1 - saturation * f);
    float t = brightness * (1 - saturation * (1 - f));
    switch(i) {
      case 0:
        *red = (int)(brightness * 255 + 0.5);
        *green = (int)(t * 255 + 0.5);
        *blue = (int)(p * 255 + 0.5);
        break;
      case 1:
        *red = (int)(q * 255 + 0.5);
        *green = (int)(brightness * 255 + 0.5);
        *blue = (int)(p * 255 + 0.5);
        break;
      case 2:
        *red = (int)(p * 255 + 0.5);
        *green = (int)(brightness * 255 + 0.5);
        *blue = (int)(t * 255 + 0.5);
        break;
      case 3:
        *red = (int)(p * 255 + 0.5);
        *green = (int)(q * 255 + 0.5);
        *blue = (int)(brightness * 255 + 0.5);
        break;
      case 4:
        *red = (int)(t * 255 + 0.5);
        *green = (int)(p * 255 + 0.5);
        *blue = (int)(brightness * 255 + 0.5);
        break;
      default:
        *red = (int)(brightness * 255 + 0.5);
        *green = (int)(p * 255 + 0.5);
        *blue = (int)(q * 255 + 0.5);
        break;
    }
  }
}

int toColor(int red, int green, int blue) {
    return (red << 16) | (green << 8) | blue;
}

int main(int argc, char **argv) {
//Declare and initialise all the variables
int offset1, offset2, offset3, offset4;
    int period1, period2, period3, period4;
    int length1, length2, length3, length4;
    int amp1, amp2, amp3, amp4;
    int origin1, origin2, origin3, origin4;
    int adc1, adc2, adc3, adc4;
    float siny1, siny2, siny3, siny4;
    int x;
    float pas1, pas2, pas3, pas4;

    float amplitude = 50.0f; // set the amplitude of the wave
    float frequency = 0.5f; // set the frequency of the wave
    float minY = -amplitude; // set the minimum y value
    float maxY = amplitude; // set the maximum y value

    int width = 640;
    int height = 480;
    float zoom = 0.02f;
    float xOffset = 0.0f;
    float yOffset = 0.0f;
  
    int pixels[width * height];

    //Define the colors for the color ramp
    u32 color1 = 0xff0000ff; // Blue
    u32 color2 = 0xff00ff00; // Green
    u32 color3 = 0xffffff00; // Yellow
    u32 color4 = 0xffff0000; // Red

    // Initialise the Graphics & Video subsystem
    GRRLIB_Init();
    PAD_Init();


    adc1=0;
    offset1=0;
    origin1=0;
    length1=1280;
    amp1=100;
    period1=1;
    pas1=(period1*360.0F)/length1;
    siny1 = offset1*pas1;

    adc2=1;
    offset2=0;
    origin2=0;
    length2=1280;
    amp2=40;
    period2=2;
    pas2=(period2*360.0F)/length2;
    siny2 = offset2*pas2;

    adc3=-3;
    offset3=0;
    origin3=0;
    length3=1280;
    amp3=30;
    period3=1;
    pas3=(period3*360.0F)/length3;
    siny3 = offset3*pas3;

    adc4=-7;
    offset4=0;
    origin4=0;
    length4=1280;
    amp4=70;
    period4=1;
    pas4=(period4*360.0F)/length4;
    siny4 = offset4*pas4;


    while (1) {
        GRRLIB_FillScreen(0x000000FF);
        PAD_ScanPads();
        if (PAD_ButtonsDown(0) & PAD_BUTTON_START)  break;
        float old1=siny1;
        float old2=siny2;
        float old3=siny3;
        float old4=siny4;

        // Loop through all the pixels in the screen horizontally
        for (x=0; x<=640; x++) {
             // Calculate the y positions for each of the waves
            siny1+=pas1;
            siny2+=pas2;
            siny3+=pas3;
            siny4+=pas4;
            float y1 = (float)origin1 + (float)amp1 * sin(siny1);
            float y2 = (float)origin2 + (float)amp2 * sin(siny2);
            float y3 = (float)origin3 + (float)amp3 * sin(siny3);
            float y4 = (float)origin4 + (float)amp4 * sin(siny4);

            // Call the sineWave function with the current x position to get the wave value.
            int y = (int)sineWave(x, amplitude, frequency);
/*
            // Calculate the color based on the y positions
            u32 color = lerpColor(lerpColor(color1, color2, (y1 + y2)/200),
                              lerpColor(color3, color4, (y3 + y4)/200),
                              0.5f);
*/
            // Define the start and end hue values for the rainbow
            float startHue = 0.0f;
            float endHue = 360.0f;

            // Calculate the hue based on the y position
            //float hue = map(y, minY, maxY, startHue, endHue);
            float value = generatePlasma(x * zoom + xOffset, y * zoom + yOffset);
            float hue = value / 6.0f; // Map value to hue range of 0.0 - 1.0


            // Convert the hue to an RGB color
            float saturation = 1.0f;
            float brightness = 1.0f;

            int red, green, blue; // Output RGB values

            // Call HSBtoRGB with hue, saturation, and brightness
            HSBtoRGB(hue, saturation, brightness, &red, &green, &blue);
            pixels[y * width + x] = toColor(red, green, blue);
            //GRRLIB_SetPixelToFB(x, y, pixels[x + y*width]);



            
            GX_Begin(GX_LINES, GX_VTXFMT0, 2);
                GX_Position3f32(x, 0, 0);
                GX_Color1u32(0x000000FF);
                GX_Position3f32(x, (sin(DegToRad(siny1))*amp1+origin1)+(sin(DegToRad(siny2))*amp2+origin2)+(sin(DegToRad(siny3))*amp3+origin3)+(sin(DegToRad(siny4))*amp4+origin4)+240,  0);
                GX_Color1u32(pixels[x + y]);
                //GX_Color4u8(red, green, blue, 1.0f);

            GX_End();
            GX_Begin(GX_LINES, GX_VTXFMT0, 2);
                GX_Position3f32(x, (sin(DegToRad(siny1))*amp1+origin1)+(sin(DegToRad(siny2))*amp2+origin2)+(sin(DegToRad(siny3))*amp3+origin3)+(sin(DegToRad(siny4))*amp4+origin4)+240,  0);
                GX_Color1u32(pixels[x + y]);
                //GX_Color4u8(red, green, blue, 1.0f);
                GX_Position3f32(x, 480, 0);
                GX_Color1u32(0x000000FF);
            GX_End();
        

        }
        siny1=old1+(adc1*pas1);
        siny2=old2+(adc2*pas2);
        siny3=old3+(adc3*pas3);
        siny4=old4+(adc4*pas4);

        

        GRRLIB_Render();  // Render the frame buffer to the TV
    }

    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB
    free(pixels);

    exit(0);  // Use exit() to exit a program, do not use 'return' from main()
}