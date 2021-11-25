#include <stdlib.h>
#include <string.h>
#include <stdio.h>


// include NESLIB header
#include "neslib.h"
// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

// include CC65 NES Header (PPU)
#include <nes.h>

extern const byte build_logo[];
extern const byte cover[];
extern const byte slide_background[];

// link the pattern table into CHR ROM
//#link "chr_generic.s"
//#link "build_logo.s"
//#link "slide_background.s"


/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x03,			// screen color

  0x11,0x30,0x27,0x0,	// background palette 0
  0x1c,0x20,0x2c,0x0,	// background palette 1
  0x00,0x10,0x20,0x0,	// background palette 2
  0x06,0x16,0x26,0x0,   // background palette 3

  0x16,0x35,0x24,0x0,	// sprite palette 0
  0x00,0x37,0x25,0x0,	// sprite palette 1
  0x0d,0x2d,0x3a,0x0,	// sprite palette 2
  0x0d,0x27,0x2a	// sprite palette 3
};

const int number_slides = 6;
const char* header = "Making Games For The NES";

const char* slide1[] = { "Agenda:", " ", "- What's the NES", "- Why", "- Components", "- Tools", "- Questions", "- Live demo (maybe?)", "" };
const char* slide2[] = { "What's the NES:", " ", "- History", "- 6502 CPU", "" };
const char* slide3[] = { "Why:", " ", "- Curiosity", "- Understanding Low Level", "- Fun", "" };
const char* slide4[] = { "Components:", " ", "- CPU, PPU, APU", "- Tables:", "  \xba\xbbNameTables", "  \xba\xbbPattern Tables", "  \xba\xbb""Attribute Tables", "- Sprites and Metasprites", "" };
const char* slide5[] = { "Tools:", " ", "- 8BitWorkshop.com", "- NES Screen Tool", "- FamiTracker", "" };
const char* slide6[] = { " ", " ", "QUESTIONS?", "" };

const char** slides[] = { slide1, slide2, slide3, slide4, slide5, slide6 };


// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_clear();
  // set palette colors
  pal_all(PALETTE);
}

void show_title_screen(const byte* pal, const byte* rle) {
  
  // disable rendering
  ppu_off();
  // set palette, virtual bright to 0 (total black)
  pal_bg(pal);
  pal_bright(3);
  // unpack nametable into the VRAM
  vram_adr(NAMETABLE_A);
  vram_unrle(rle);
  // enable rendering
  ppu_on_all();
  // fade in from black
  //fade_in();
}

void build_slide(int slide_index)
{
  const char** slide = slides[slide_index-1];
  int i;
  
  for (i = 0; slide[i] != ""; i++)
  {
    vram_adr(NTADR_A(4, 8 + i*2));  
    vram_write(slide[i], strlen(slide[i]));
  }
  
}

void show_slide(int slide)
{
  char title[10]; 
  ppu_off();
  pal_bg(PALETTE);
  pal_bright(3);
  vram_adr(NAMETABLE_A);
  vram_fill(0x0, 64*8*8);
  //vram_unrle(slide_background);
  
  //header
  vram_adr(NTADR_B(1,2));  
  vram_write(header, strlen(header));
  
  //build slide
  build_slide(slide);
  
  //footer
  sprintf(title, "Slide# %d", slide);
  vram_adr(NTADR_B(54,25));  
  vram_write(title, strlen(title));
  ppu_on_all();
}

void draw_slide(const char i)
{
  if(i == 0)
  {
    show_title_screen(PALETTE, build_logo);
    return;
  }
  
  show_slide(i);
  ppu_wait_frame();
}

void main(void)
{
  char pad;
  int slide=0;
  setup_graphics();
  
  // enable rendering
  
  draw_slide(slide);
  
  ppu_on_all();

  // infinite loop
  while(1) {
    pad_trigger(0);
    ppu_wait_frame();
    ppu_wait_frame();
    pad = pad_state(0);
    
    if(pad&PAD_LEFT)
    {
      slide--;
      if(slide < 0)
      {
        slide = 0;
      }
      draw_slide(slide);
    }
    
    if(pad&PAD_RIGHT)
    {
      slide++;
      if(slide > number_slides)
      {
        slide = number_slides;
      }
      
      draw_slide(slide);
    }
  }
}
