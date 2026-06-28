#include "data_type.h"

using namespace std;

string GetDataTypeName(DataType dt) {
  switch(dt) {
    case dtNotInitialize: return "dont_inited";
    case dtNotApplicable: return "not_applicable";
    case dtByte: return "byte";
    case dtInt: return "int";
    default: return "unk_type";
  }
}
