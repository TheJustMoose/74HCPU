#include <fstream>
#include <iostream>
#include <stdint.h>

#include "read_hex.h"

using namespace std;

bool ReadHex(string fname, vector<uint16_t>& buf) {
  ifstream f;
  f.open(fname, ios::binary);
  if (!f) {
    cout << "Error. Can't open file " << fname << endl;
    return false;
  }

  f.seekg(0, ios::end);
  streamsize fileSize = f.tellg();
  f.seekg(0, ios::beg);

  if (fileSize % sizeof(uint16_t)) {
    cout << "The file size should be even." << endl;
    return false;
  }

  buf.resize(fileSize / sizeof(uint16_t), 0U);

  f.read(reinterpret_cast<char *>(buf.data()),
         buf.size()*sizeof(uint16_t));
  f.close();

  cout << fname << " file was loaded. Size: " << fileSize << endl << endl;

  return true;
}
