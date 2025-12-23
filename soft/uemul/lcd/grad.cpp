#include <stdio.h>
#include <stdint.h>

const int w = 480;
const int h = 272;
const int d = w / 4;

uint16_t video[w*h] {};

// F E D C B A 9 8 7 6 5 4 3 2 1 0
// 0 b b b b b g g g g g r r r r r
uint16_t rgb(int r, int g, int b) {
  uint16_t res;
  res = r;
  res |= g << 5;
  res |= b << 10;
  return res;
}

void grad(bool r, bool g, bool b, int strip) {
  for (int y = 0; y < 256; y++) {
    int br = y / 8;  // only 5 bits are required
    for (int x = 0; x < d; x++) {
      int offs = y*w + d*strip + x;
      if (offs > w*h)
        printf("offs is too much: %d: %d, %d\n", offs, x, y);
      video[offs] = rgb(r*br, g*br, b*br);
    }
    int offs = y*w + d*strip;
    printf("%04X\n", video[offs]);
  }
  printf("end of grad\n");
}

int main() {
  printf("%04X\n", rgb(1, 1, 1));
  printf("%04X\n", rgb(10, 10, 10));
  printf("%04X\n", rgb(31, 31, 31));
  printf("pixels: %d\n", w*h);
  printf("bytes: %d\n", w*h*2);

  grad(1, 0, 0, 0);
  grad(0, 1, 0, 1);
  grad(0, 0, 1, 2);
  grad(1, 1, 1, 3);

  printf("Filled...\n");

  FILE *f = fopen("dump.bin", "wb");
  if (!f) {
    printf("Can't open dump.bin for write.\n");
    return 1;
  }

  size_t res = fwrite(video, 2, w*h, f);
  if (res != w*h)
    printf("Wrote only %d items.\n", res);

  fclose(f);
  printf("Done!\n");
  return 0;
}
