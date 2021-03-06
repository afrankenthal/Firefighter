#include "../utils/json.hpp"
using json = nlohmann::json;
using namespace ROOT::VecOps;

void PlotMuonObjectDr(TString config_filename, bool fSave=0, bool fShow=1) {
    gROOT->LoadMacro("../utils/tdrstyle.C");
    gROOT->ProcessLine("setTDRStyle();");

    gROOT->SetBatch(!fShow);
    //std::vector<TString> observables = { "pt", "pt_zoom", "eta", "phi", "dr", "vxy" };

    std::map<TString, map<TString, TH1F*>> nums, denoms;
    std::map<TString, map<TString, TEfficiency*>> effs;
    std::map<TString, TCanvas*> canvases;
    std::map<TString, std::vector<float>> bins;
    std::map<TString, TString> vars;
    std::map<TString, TString> axis_label;
    std::map<TString, TString> plot_filenames;

    TString collection = "dsa";
    TString region = "inclusive";

    bins["pt"] = { 0, 2, 4, 6, 8, 10,
        15, 20, 25, 30, 50, 
        70 };
    //vars["pt"] = "genPt";
    vars["pt"] = "mu_gen_pt";
    axis_label["pt"] = "Muon p_{T} [GeV]";
    plot_filenames["pt"] = TString::Format("%s_pt_%s.pdf", collection.Data(), region.Data());

    bins["pt_zoom"] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12,
        14, 16, 18, 20};
    //vars["pt_zoom"] = "genPt";
    vars["pt_zoom"] = "mu_gen_pt";
    axis_label["pt_zoom"] = "Muon p_{T} [GeV]";
    plot_filenames["pt_zoom"] = TString::Format("%s_ptzoom_%s.pdf", collection.Data(), region.Data());

    bins["eta"] = {};
    for (auto i = -2.5; i <= 2.5; i += 0.2) bins["eta"].push_back(i);
    //vars["eta"] = "genEta";
    vars["eta"] = "mu_gen_eta";
    axis_label["eta"] = "Muon #eta";
    plot_filenames["eta"] = TString::Format("%s_eta_%s.pdf", collection.Data(), region.Data());

    bins["phi"] = {};
    for (auto i = -3.2; i <= 3.2; i += 0.2) bins["phi"].push_back(i);
    //vars["phi"] = "genPhi";
    vars["phi"] = "mu_gen_phi";
    axis_label["phi"] = "Muon #phi";
    plot_filenames["phi"] = TString::Format("%s_phi_%s.pdf", collection.Data(), region.Data());

    bins["dr"] = { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 };
        //1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.5, 4.0, 4.5,
        //5.0, 6.0};
    //vars["dr"] = "genDr";
    vars["dr"] = "mu_gen_dR";
    axis_label["dr"] = "Muon pair dR (gen)";
    plot_filenames["dr"] = TString::Format("%s_dr_%s.pdf", collection.Data(), region.Data());

    bins["vxy"] = { 0, 25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 325, 350};
        //550, 600, 650, 700, 750, 800, 850, 900, 950, 1000};
    //vars["vxy"] = "genVxy";
    vars["vxy"] = "mu_gen_vxy";
    axis_label["vxy"] = "Muon pair vertex v_{xy} (gen) [cm]";
    plot_filenames["vxy"] = TString::Format("%s_vxy_%s.pdf", collection.Data(), region.Data());

    bins["vz"] = { 0, 25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 325, 350};
        //550, 600, 650, 700, 750, 800, 850, 900, 950, 1000};
    //vars["vxy"] = "genVxy";
    vars["vz"] = "mu_gen_vz";
    axis_label["vz"] = "Muon pair vertex v_{z} (gen) [cm]";
    plot_filenames["vz"] = TString::Format("%s_vz_%s.pdf", collection.Data(), region.Data());
    
    //bins["gen_reco_dR"] = { 0, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09,
    //    0.10, 0.11, 0.12, 0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.20,
    //    0.21, 0.22, 0.23, 0.24, 0.25, 0.26, 0.27, 0.28, 0.29, 0.30 };
    //bins["gen_reco_dR"] = { 0, 0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.007, 0.008, 0.009,
    //    0.010, 0.011, 0.012, 0.013, 0.014, 0.015, 0.016, 0.017, 0.018, 0.019, 0.020,
    //    0.021, 0.022, 0.023, 0.024, 0.025, 0.026, 0.027, 0.028, 0.029, 0.030 };
    bins["gen_reco_dR"] = {};
    for (auto i = 0.f; i <= 0.31; i += 0.01) bins["gen_reco_dR"].push_back(i);
    vars["gen_reco_dR"] = "gen_reco_dR";
    axis_label["gen_reco_dR"] = "Gen-reco dR";
    plot_filenames["gen_reco_dR"] = TString::Format("%s_gen_reco_dR_%s.pdf", collection.Data(), region.Data());
    
    std::ifstream config_file(config_filename.Data());
    json config;
    config_file >> config;

    cout << "before loop " << endl;

    for (auto const & [obs_cppstr, obs_cfg] : config["observables"].items()) {
        TString obs(obs_cppstr.c_str());

        float bins_temp[bins[obs].size()];
        std::copy(bins[obs].begin(), bins[obs].end(), bins_temp);

        canvases[obs] = new TCanvas();
        TH1F * frame = canvases[obs]->DrawFrame(bins[obs][0], 0, bins[obs][bins[obs].size()-1], 1.0);
        frame->GetXaxis()->SetTitle(axis_label[obs].Data());
        frame->GetYaxis()->SetTitle("A. U.");
        canvases[obs]->Update();
        TLegend * legend = new TLegend(0.55, 0.75, 0.8, 0.9);

        for (auto const & [sample, sample_cfg] : config["samples"].items()) {

            nums[obs][sample] = new TH1F(Form("hnum_%s_%s", obs.Data(), sample.c_str()), Form("hnum_%s_%s", obs.Data(), sample.c_str()), sizeof(bins_temp)/sizeof(float) - 1, bins_temp);
            denoms[obs][sample] = new TH1F(Form("hdenom_%s_%s", obs.Data(), sample.c_str()), Form("hdenom_%s_%s", obs.Data(), sample.c_str()), sizeof(bins_temp)/sizeof(float) - 1, bins_temp);

            for (auto filename : sample_cfg["files"].get<std::vector<std::string>>()) {

                TFile * file = TFile::Open(filename.c_str(), "READ");
                //TTree * tree = (TTree*)file->Get(Form("RECO_%s/recoEffiForMuTrack", sample_cfg["collection"].get<std::string>().c_str()));
                TTree * tree = (TTree*)file->Get("ntuples_gbm/recoT");
                TTree * gen_tree = (TTree*)file->Get("ntuples_gbm/genT");
                tree->AddFriend(gen_tree);

                ROOT::RDataFrame df(*tree);

                auto findMatch = [&](RVec<float> gen_eta, RVec<float> gen_phi, RVec<float> reco_eta, RVec<float> reco_phi, RVec<int> gen_charge, RVec<int> reco_charge) {
                    // assume only 2 gen muons
                    RVec<float> match_dR(2, 1000.f);
                    if (reco_eta.size() == 0)
                        return match_dR;
                    RVec<float> dR_grid_0(reco_eta.size()), dR_grid_1(reco_eta.size());
                    for (auto j = 0; j < reco_eta.size(); j++) {
                        auto dphi = std::abs(gen_phi[0] - reco_phi[j]);
                        if (dphi > 3.141592) dphi -= 2 * 3.141592;
                        auto deta = std::abs(gen_eta[0] - reco_eta[j]);
                        auto dr = std::sqrt(dphi * dphi + deta * deta);
                        dR_grid_0[j] = dr;
                        if (gen_charge[0] != reco_charge[j])
                            dR_grid_0[j] = 1000.f;
                        dphi = std::abs(gen_phi[1] - reco_phi[j]);
                        if (dphi > 3.141592) dphi -= 2 * 3.141592;
                        deta = std::abs(gen_eta[1] - reco_eta[j]);
                        dr = std::sqrt(dphi * dphi + deta * deta);
                        dR_grid_1[j] = dr;
                        if (gen_charge[1] != reco_charge[j])
                            dR_grid_1[j] = 1000.f;
                    }
                    auto best_match_0 = ArgMin(dR_grid_0);
                    auto best_match_1 = ArgMin(dR_grid_1);
                    if (best_match_0 == best_match_1) { // need to find best global match
                        if (dR_grid_0[best_match_0] < dR_grid_1[best_match_1]) {
                            dR_grid_1[best_match_1] = 1000.f; // set to infinity
                            best_match_1 = ArgMin(dR_grid_1);
                        }
                        else {
                            dR_grid_0[best_match_0] = 1000.f; // set to infinity
                            best_match_0 = ArgMin(dR_grid_0);
                        }
                    }
                    match_dR[0] = dR_grid_0[best_match_0];// < 0.2;
                    match_dR[1] = dR_grid_1[best_match_1];// < 0.2;
                    return match_dR;
                };

                auto findMatchSimple = [&](RVec<float> gen_eta, RVec<float> gen_phi, RVec<float> reco_eta, RVec<float> reco_phi) {
                    RVec<bool> matched(2, 0);
                    if (reco_eta.size() > 0) {
                        auto dphi = std::abs(gen_phi[0] - reco_phi[0]);
                        if (dphi > 3.141592) dphi -= 2 * 3.141592;
                        auto deta = std::abs(gen_eta[0] - reco_eta[0]);
                        auto dr = std::sqrt(dphi * dphi + deta * deta);
                        matched[0] = dr < 0.3;
                    }
                    if (reco_eta.size() > 1) {
                        auto dphi = std::abs(gen_phi[1] - reco_phi[1]);
                        if (dphi > 3.141592) dphi -= 2 * 3.141592;
                        auto deta = std::abs(gen_eta[1] - reco_eta[1]);
                        auto dr = std::sqrt(dphi * dphi + deta * deta);
                        matched[1] = dr < 0.3;
                    }
                    //matched[0] = reco_eta.size() > 0;
                    //matched[1] = reco_eta.size() > 1;
                    return matched;
                };

                auto calcGenMudR = [&](RVec<float> gen_eta, RVec<float> gen_phi) {
                    auto dphi = std::abs(gen_phi[1] - gen_phi[0]);
                    if (dphi > 3.141592) dphi -= 2 * 3.141592;
                    auto deta = std::abs(gen_eta[1] - gen_eta[0]);
                    auto dr = std::sqrt(dphi * dphi + deta * deta);
                    RVec<float> gen_dR(gen_eta.size());
                    gen_dR[0] = dr;
                    gen_dR[1] = dr;
                    return gen_dR;
                };

                TString histo_name = Form("hnum_%s_%s", obs.Data(), sample.c_str());
                ROOT::RDF::TH1DModel model(histo_name, histo_name, sizeof(bins_temp)/sizeof(float) - 1, bins_temp);

                //auto df_new = df.Define("match_gen", findMatch, {"genEta", "genPhi", "recoEta", "recoPhi"});
                //auto df_new = df.Define("match_gen", findMatchSimple, {"genEta", "genPhi", "recoEta", "recoPhi"});
                auto df_new = df.Define("mu_gen_pt", "gen_pt[abs(gen_ID)==13]").
                                Define("mu_gen_eta", "gen_eta[abs(gen_ID)==13]").
                                Define("mu_gen_phi", "gen_phi[abs(gen_ID)==13]").
                                Define("mu_gen_vxy", "gen_vxy[abs(gen_ID)==13]").
                                Define("mu_gen_vz", "gen_vz[abs(gen_ID)==13]").
                                Define("mu_gen_charge", "gen_charge[abs(gen_ID)==13]").
                                Define("mu_gen_dR", calcGenMudR, {"mu_gen_eta", "mu_gen_phi"}).
                                Define("gen_reco_dR", findMatch, {"mu_gen_eta", "mu_gen_phi", "reco_dsa_eta", "reco_dsa_phi", "mu_gen_charge", "reco_dsa_charge"}).
                                Define("match_gen", "gen_reco_dR < 0.3");

                auto hdenom_temp = df_new.Define("filter_by_eta", Form("%s[abs(mu_gen_eta)<3.0]", vars[obs].Data())).Histo1D<RVec<float>>(model, "filter_by_eta"); //vars[obs].Data());
                auto hnum_temp = df_new.Define("matched_obs", Form("%s[match_gen==1&&abs(mu_gen_eta)<3.0]", vars[obs].Data())).Histo1D<RVec<float>>(model, "matched_obs");

                //TH1F * num_temp = (TH1F*)nums[obs][sample]->Clone();
                //num_temp->Reset();
                //tree->Draw(Form("%s[0]>>+hnum_%s_%s", vars[obs].Data(), obs.Data(), sample.c_str()), "nMatched>0", "goff");
                //tree->Draw(Form("%s[1]>>+hnum_%s_%s", vars[obs].Data(), obs.Data(), sample.c_str()), "nMatched>1", "goff");
                nums[obs][sample]->Add(hnum_temp.GetPtr());
                //TH1F * denom_temp = (TH1F*)denoms[obs][sample]->Clone();
                //denom_temp->Reset();
                //tree->Draw(Form("%s>>+hdenom_%s_%s", vars[obs].Data(), obs.Data(), sample.c_str()), "", "goff");
                denoms[obs][sample]->Add(hdenom_temp.GetPtr());
            }

            effs[obs][sample] = new TEfficiency(*nums[obs][sample], *denoms[obs][sample]);
            nums[obs][sample]->SetName(sample_cfg["legend"].get<std::string>().c_str());
            nums[obs][sample]->SetMarkerColor(sample_cfg["color"].get<int>());
            nums[obs][sample]->SetLineColor(sample_cfg["color"].get<int>());
            nums[obs][sample]->DrawNormalized("ZE SAME");
            legend->AddEntry(nums[obs][sample], sample_cfg["legend"].get<std::string>().c_str(), "lep");
        }
        legend->Draw("SAME");

        if (fSave) {
            canvases[obs]->SaveAs(plot_filenames[obs].Data());
        }
    }

    gROOT->SetBatch(0);
}
