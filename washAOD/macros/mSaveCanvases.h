#ifndef mSAVECANVASES_HH
#define mSAVECANVASES_HH

#include <algorithm>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string.h>

#include <TApplication.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TCollection.h>
#include <TCut.h>
#include <TDatime.h>
#include <TFile.h>
#include <TH1F.h>
#include <TImage.h>
#include <TROOT.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

#include "utils/common.h"
using namespace common;
#include "utils/cxxopts.hpp"
#include "utils/json.hpp"
using json = nlohmann::json;

using std::cout, std::endl, std::map, std::vector;

namespace macro {

    bool mSaveCanvases([[maybe_unused]] map<TString, SampleInfo> samples, json cfg);

}

#endif // mSAVECANVASES_HH
