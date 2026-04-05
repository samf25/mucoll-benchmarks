#include "TBox.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH2F.h"
#include "TColor.h"
#include "TPad.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLine.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TTree.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

R__LOAD_LIBRARY(libpodio)
R__LOAD_LIBRARY(libpodioDict)
R__LOAD_LIBRARY(libpodioRootIO)
R__LOAD_LIBRARY(libedm4hep)
R__LOAD_LIBRARY(libedm4hepDict)

namespace {

struct RocPoint {
  double threshold = 0.0;
  double tpr = 0.0;
  double fpr = 0.0;
};

struct ConfusionCounts {
  std::uint64_t trueNegative = 0;
  std::uint64_t falsePositive = 0;
  std::uint64_t falseNegative = 0;
  std::uint64_t truePositive = 0;
};

struct ConfusionFractions {
  double trueNegative = 0.0;
  double falsePositive = 0.0;
  double falseNegative = 0.0;
  double truePositive = 0.0;
};

struct ScoreLabelEntry {
  float score = 0.0f;
  int label = 0;
};

struct LabelCounts {
  std::uint64_t zeros = 0;
  std::uint64_t ones = 0;
  std::uint64_t other = 0;
};

bool loadScoreLabelData(const char* inputFile,
                        const char* scoreBranch,
                        const char* labelBranch,
                        std::vector<ScoreLabelEntry>& entries,
                        LabelCounts& labelCounts) {
  TFile* file = TFile::Open(inputFile, "READ");
  if (!file || file->IsZombie()) {
    std::cerr << "Failed to open input file: " << inputFile << std::endl;
    return false;
  }

  TTree* tree = static_cast<TTree*>(file->Get("events"));
  if (!tree) {
    std::cerr << "Could not find TTree 'events' in file: " << inputFile << std::endl;
    file->Close();
    return false;
  }

  std::vector<float>* scores = nullptr;
  std::vector<float>* labels = nullptr;

  if (tree->SetBranchAddress(scoreBranch, &scores) < 0) {
    std::cerr << "Could not bind branch: " << scoreBranch << std::endl;
    file->Close();
    return false;
  }
  if (tree->SetBranchAddress(labelBranch, &labels) < 0) {
    std::cerr << "Could not bind branch: " << labelBranch << std::endl;
    file->Close();
    return false;
  }

  const Long64_t nEvents = tree->GetEntries();
  entries.clear();
  labelCounts = {};
  entries.reserve(static_cast<std::size_t>(nEvents) * 128);

  std::size_t mismatchedEvents = 0;
  for (Long64_t iEvt = 0; iEvt < nEvents; ++iEvt) {
    tree->GetEntry(iEvt);

    if (!scores || !labels) {
      continue;
    }
    if (scores->size() != labels->size()) {
      ++mismatchedEvents;
      continue;
    }

    for (std::size_t i = 0; i < scores->size(); ++i) {
      const float rawLabel = labels->at(i);
      const int label = static_cast<int>(std::lround(rawLabel));
      if (label == 0) {
        ++labelCounts.zeros;
      } else if (label == 1) {
        ++labelCounts.ones;
      } else {
        ++labelCounts.other;
      }
      entries.push_back({scores->at(i), label});
    }
  }

  if (mismatchedEvents > 0) {
    std::cerr << "Skipped " << mismatchedEvents << " events because score/label collection sizes differed." << std::endl;
  }

  file->Close();
  return !entries.empty();
}

std::vector<RocPoint> buildRocCurve(const std::vector<ScoreLabelEntry>& entries,
                                    int signalLabelValue,
                                    std::uint64_t& nSignal,
                                    std::uint64_t& nBackground,
                                    double& auc) {
  std::vector<ScoreLabelEntry> sortedEntries = entries;
  std::sort(sortedEntries.begin(), sortedEntries.end(), [](const auto& a, const auto& b) {
    return a.score > b.score;
  });

  nSignal = 0;
  for (const auto& entry : sortedEntries) {
    nSignal += static_cast<std::uint64_t>(entry.label == signalLabelValue);
  }
  nBackground = static_cast<std::uint64_t>(sortedEntries.size()) - nSignal;

  std::vector<RocPoint> rocPoints;
  auc = 0.0;
  if (nSignal == 0 || nBackground == 0) {
    return rocPoints;
  }

  rocPoints.push_back({std::numeric_limits<double>::infinity(), 0.0, 0.0});

  std::uint64_t runningTP = 0;
  std::uint64_t runningFP = 0;
  double prevTPR = 0.0;
  double prevFPR = 0.0;

  std::size_t i = 0;
  while (i < sortedEntries.size()) {
    const float threshold = sortedEntries[i].score;
    while (i < sortedEntries.size() && sortedEntries[i].score == threshold) {
      if (sortedEntries[i].label == signalLabelValue) {
        ++runningTP;
      } else {
        ++runningFP;
      }
      ++i;
    }

    const double tpr = static_cast<double>(runningTP) / static_cast<double>(nSignal);
    const double fpr = static_cast<double>(runningFP) / static_cast<double>(nBackground);
    auc += 0.5 * (tpr + prevTPR) * (fpr - prevFPR);
    rocPoints.push_back({threshold, tpr, fpr});
    prevTPR = tpr;
    prevFPR = fpr;
  }

  return rocPoints;
}

ConfusionCounts buildConfusionCounts(const std::vector<ScoreLabelEntry>& entries, double cut, int signalLabelValue) {
  ConfusionCounts counts;
  for (const auto& entry : entries) {
    const bool predictSignal = entry.score >= cut;
    const bool truthSignal = entry.label == signalLabelValue;

    if (truthSignal && predictSignal) {
      ++counts.truePositive;
    } else if (truthSignal && !predictSignal) {
      ++counts.falseNegative;
    } else if (!truthSignal && predictSignal) {
      ++counts.falsePositive;
    } else {
      ++counts.trueNegative;
    }
  }
  return counts;
}

ConfusionFractions buildConfusionFractions(const ConfusionCounts& counts) {
  // Normalize by truth class (columns):
  // - True BIB column: TN + FP
  // - True Signal column: FN + TP
  const double trueBibTotal = static_cast<double>(counts.trueNegative + counts.falsePositive);
  const double trueSignalTotal = static_cast<double>(counts.falseNegative + counts.truePositive);
  return {
      (trueBibTotal > 0.0) ? 100.0 * static_cast<double>(counts.trueNegative) / trueBibTotal : 0.0,
      (trueBibTotal > 0.0) ? 100.0 * static_cast<double>(counts.falsePositive) / trueBibTotal : 0.0,
      (trueSignalTotal > 0.0) ? 100.0 * static_cast<double>(counts.falseNegative) / trueSignalTotal : 0.0,
      (trueSignalTotal > 0.0) ? 100.0 * static_cast<double>(counts.truePositive) / trueSignalTotal : 0.0,
  };
}

void styleConfusionHistogram(TH2F& hist, const ConfusionFractions& fractions, double maxPercent) {
  hist.SetStats(false);
  // Use a tiny negative minimum so 0-valued bins are color-filled by COL.
  hist.SetMinimum(-1e-6);
  hist.SetMaximum(std::max(1.0, maxPercent));
  hist.SetContour(128);
  hist.SetLineColor(kWhite);
  hist.SetLineWidth(2);
  hist.SetMarkerSize(2.1);
  hist.SetTitle("");
  hist.GetXaxis()->SetTitle("Truth class");
  hist.GetYaxis()->SetTitle("Predicted class");
  hist.GetXaxis()->SetBinLabel(1, "BIB");
  hist.GetXaxis()->SetBinLabel(2, "Signal");
  hist.GetYaxis()->SetBinLabel(1, "BIB");
  hist.GetYaxis()->SetBinLabel(2, "Signal");
  hist.GetXaxis()->CenterLabels(true);
  hist.GetYaxis()->CenterLabels(true);
  hist.GetXaxis()->SetNdivisions(2, false);
  hist.GetYaxis()->SetNdivisions(2, false);
  hist.GetXaxis()->SetLabelSize(0.062);
  hist.GetYaxis()->SetLabelSize(0.062);
  hist.GetXaxis()->SetTitleSize(0.060);
  hist.GetYaxis()->SetTitleSize(0.060);
  hist.GetXaxis()->SetTitleOffset(1.10);
  hist.GetYaxis()->SetTitleOffset(1.65);
  hist.GetXaxis()->SetLabelOffset(0.012);
  hist.GetYaxis()->SetLabelOffset(0.012);
  hist.GetXaxis()->SetTickLength(0.0);
  hist.GetYaxis()->SetTickLength(0.0);
  hist.GetZaxis()->SetLabelSize(0.0);
  hist.GetZaxis()->SetTickLength(0.0);

  hist.SetBinContent(1, 1, fractions.trueNegative);
  hist.SetBinContent(1, 2, fractions.falsePositive);
  hist.SetBinContent(2, 1, fractions.falseNegative);
  hist.SetBinContent(2, 2, fractions.truePositive);
}

void drawConfusionPercentLabels(const ConfusionFractions& fractions,
                                const ConfusionCounts& counts,
                                double maxPercent) {
  struct CellLabel {
    double x;
    double y;
    double percent;
    std::uint64_t count;
  };

  const std::vector<CellLabel> labels = {
      {0.5, 0.5, fractions.trueNegative, counts.trueNegative},
      {0.5, 1.5, fractions.falsePositive, counts.falsePositive},
      {1.5, 0.5, fractions.falseNegative, counts.falseNegative},
      {1.5, 1.5, fractions.truePositive, counts.truePositive},
  };

  for (const auto& cell : labels) {
    const double darknessThreshold = 0.35 * std::max(1.0, maxPercent);
    const int textColor = (cell.percent <= darknessThreshold) ? kWhite : kBlack;

    TLatex percentText;
    percentText.SetTextAlign(22);
    percentText.SetTextFont(62);
    percentText.SetTextSize(0.092);
    percentText.SetTextColor(textColor);
    percentText.DrawLatex(cell.x, cell.y + 0.09, Form("%.1f%%", cell.percent));

    TLatex countText;
    countText.SetTextAlign(22);
    countText.SetTextFont(42);
    countText.SetTextSize(0.055);
    countText.SetTextColor(textColor);
    countText.DrawLatex(cell.x, cell.y - 0.15, Form("(%llu)", static_cast<unsigned long long>(cell.count)));
  }
}

void setConfusionBluePalette() {
  // Map low values (including 0) to dark blue and high values to light cyan.
  constexpr int nRGB = 3;
  constexpr int nCont = 128;
  Double_t stops[nRGB] = {0.0, 0.55, 1.0};
  Double_t red[nRGB] = {0.03, 0.10, 0.72};
  Double_t green[nRGB] = {0.10, 0.34, 0.92};
  Double_t blue[nRGB] = {0.35, 0.78, 0.98};
  TColor::CreateGradientColorTable(nRGB, stops, red, green, blue, nCont);
  gStyle->SetNumberContours(nCont);
}

void drawMatrixOutline() {
  auto* outline = new TBox(0.0, 0.0, 2.0, 2.0);
  outline->SetFillStyle(0);
  outline->SetLineColor(kBlack);
  outline->SetLineWidth(3);
  outline->Draw("same");
}

void drawSimulationLabelTopRight(double xRight, double yTop, double textSize) {
  TLatex line1;
  line1.SetNDC(true);
  line1.SetTextAlign(31);
  line1.SetTextFont(42);
  line1.SetTextSize(textSize);
  line1.SetTextColor(kGray + 2);
  line1.DrawLatex(xRight, yTop, "Muon Collider Simulation");

  TLatex line2;
  line2.SetNDC(true);
  line2.SetTextAlign(31);
  line2.SetTextFont(42);
  line2.SetTextSize(textSize);
  line2.SetTextColor(kGray + 2);
  line2.DrawLatex(xRight, yTop - 0.028, "MAIA Geometry");
}

void drawCanvasBorder(TPad& pad) {
  auto* border = new TBox(0.012, 0.012, 0.988, 0.988);
  border->SetFillStyle(0);
  border->SetLineColor(kBlack);
  border->SetLineWidth(2);
  border->Draw();
}

void drawRocCanvasOverlay(TCanvas& canvas) {
  canvas.cd();

  auto* overlay = new TPad("roc_canvas_overlay", "roc_canvas_overlay", 0.0, 0.0, 1.0, 1.0);
  overlay->SetFillStyle(0);
  overlay->SetFrameFillStyle(0);
  overlay->SetMargin(0.0, 0.0, 0.0, 0.0);
  overlay->Draw();
  overlay->cd();
  overlay->Range(0.0, 0.0, 1.0, 1.0);

  drawCanvasBorder(*overlay);
  drawSimulationLabelTopRight(0.968, 0.964, 0.022);

  canvas.cd();
}

void drawConfusionCanvasOverlay(TCanvas& canvas) {
  canvas.cd();

  auto* overlay = new TPad("confusion_canvas_overlay", "confusion_canvas_overlay", 0.0, 0.0, 1.0, 1.0);
  overlay->SetFillStyle(0);
  overlay->SetFrameFillStyle(0);
  overlay->SetMargin(0.0, 0.0, 0.0, 0.0);
  overlay->Draw();
  overlay->cd();
  overlay->Range(0.0, 0.0, 1.0, 1.0);

  drawCanvasBorder(*overlay);
  drawSimulationLabelTopRight(0.968, 0.964, 0.022);

  canvas.cd();
}

} // namespace

void plot_bdt_roc(const char* inputFile = "digi_output_with_bdt.edm4hep.root",
                  const char* outputPrefix = "pixel_hit_bdt",
                  const char* scoreBranch = "VXDBarrelHitBDTScores",
                  const char* labelBranch = "VXDBarrelHitTruthLabels",
                  int signalLabelValue = 1) {
  std::vector<ScoreLabelEntry> entries;
  LabelCounts labelCounts;
  if (!loadScoreLabelData(inputFile, scoreBranch, labelBranch, entries, labelCounts)) {
    std::cerr << "No score/label entries were loaded. Aborting." << std::endl;
    return;
  }

  std::uint64_t nSignal = 0;
  std::uint64_t nBackground = 0;
  double auc = 0.0;
  auto rocPoints = buildRocCurve(entries, signalLabelValue, nSignal, nBackground, auc);
  bool usedSingleClassFallback = false;

  if (rocPoints.empty()) {
    if (nSignal > 0 && nBackground == 0) {
      // Signal-only sample: draw vertical line at FPR=0.
      rocPoints = {{std::numeric_limits<double>::infinity(), 0.0, 0.0},
                   {-std::numeric_limits<double>::infinity(), 1.0, 0.0}};
      auc = 1.0;
      usedSingleClassFallback = true;
    } else if (nBackground > 0 && nSignal == 0) {
      // Background-only sample: draw horizontal line at TPR=0.
      rocPoints = {{std::numeric_limits<double>::infinity(), 0.0, 0.0},
                   {-std::numeric_limits<double>::infinity(), 0.0, 1.0}};
      auc = 0.0;
      usedSingleClassFallback = true;
    }
  }

  std::cout << "Loaded " << entries.size() << " hits from " << inputFile << std::endl;
  std::cout << "Label counts: 0 -> " << labelCounts.zeros << ", 1 -> " << labelCounts.ones;
  if (labelCounts.other > 0) {
    std::cout << ", other -> " << labelCounts.other;
  }
  std::cout << std::endl;
  std::cout << "Treating label value " << signalLabelValue << " as signal/non-overlay." << std::endl;
  std::cout << "Signal hits: " << nSignal << ", background hits: " << nBackground << std::endl;
  if (!rocPoints.empty()) {
    std::cout << "AUC: " << auc << std::endl;
  }

  gROOT->SetBatch(true);
  gStyle->SetOptStat(0);
  setConfusionBluePalette();

  std::vector<double> cuts = {-0.50, 0.00, 0.25, 0.50};

  TCanvas* rocCanvas = new TCanvas("rocCanvas", "ROC Curve", 900, 750);
  rocCanvas->SetFillColor(kWhite);
  rocCanvas->SetMargin(0.12, 0.05, 0.12, 0.08);

  TGraph* rocGraph = nullptr;
  if (!rocPoints.empty()) {
    rocGraph = new TGraph(static_cast<int>(rocPoints.size()));
    for (int i = 0; i < static_cast<int>(rocPoints.size()); ++i) {
      rocGraph->SetPoint(i, rocPoints[static_cast<std::size_t>(i)].fpr, rocPoints[static_cast<std::size_t>(i)].tpr);
    }

    rocGraph->SetTitle(";False positive rate;True positive rate");
    rocGraph->SetLineColor(kBlue + 1);
    rocGraph->SetLineWidth(3);
    rocGraph->SetMarkerStyle(20);
    rocGraph->SetMarkerSize(0.8);
    rocGraph->Draw("AL");
    rocGraph->GetXaxis()->SetLimits(0.0, 1.0);
    rocGraph->SetMinimum(0.0);
    rocGraph->SetMaximum(1.0);

    TLine* diagonal = new TLine(0.0, 0.0, 1.0, 1.0);
    diagonal->SetLineStyle(2);
    diagonal->SetLineColor(kGray + 2);
    diagonal->Draw("same");

    TLegend* legend = new TLegend(0.18, 0.18, 0.48, 0.28);
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->AddEntry(rocGraph, Form("ROC (AUC = %.4f)", auc), "l");
    legend->Draw();

      (void)usedSingleClassFallback;
  } else {
      TH2F* frame = new TH2F("roc_unavailable", ";False positive rate;True positive rate", 10, 0.0, 1.0, 10, 0.0, 1.0);
    frame->SetStats(false);
    frame->Draw();
    TLatex message;
    message.SetNDC(true);
    message.SetTextSize(0.038);
    message.DrawLatex(0.17, 0.55, "ROC unavailable: only one label class found");
    message.DrawLatex(0.17, 0.49, Form("label 0 count = %llu, label 1 count = %llu",
                                       static_cast<unsigned long long>(labelCounts.zeros),
                                       static_cast<unsigned long long>(labelCounts.ones)));
    std::cerr << "Could not build ROC curve. Need both label values 0 and 1 in the input." << std::endl;
  }

  TLatex rocLabel;
  rocLabel.SetNDC(true);
  rocLabel.SetTextSize(0.032);
  rocLabel.SetTextFont(62);
  rocLabel.DrawLatex(0.14, 0.935, "ROC Curve");
  rocLabel.SetTextFont(42);
  rocLabel.DrawLatex(0.18, 0.87, Form("Entries: %zu", entries.size()));
  rocLabel.DrawLatex(0.18, 0.82, Form("Signal(label=%d): %llu, Other: %llu", signalLabelValue,
                                      static_cast<unsigned long long>(nSignal),
                                      static_cast<unsigned long long>(nBackground)));

  drawRocCanvasOverlay(*rocCanvas);

  rocCanvas->SaveAs(Form("%s_roc.png", outputPrefix));
  rocCanvas->SaveAs(Form("%s_roc.pdf", outputPrefix));

  const int nCuts = static_cast<int>(cuts.size());
  const int nCols = static_cast<int>(std::ceil(std::sqrt(static_cast<double>(nCuts))));
  const int nRows = static_cast<int>(std::ceil(static_cast<double>(nCuts) / static_cast<double>(nCols)));

  std::vector<ConfusionCounts> confusionCounts;
  std::vector<ConfusionFractions> confusionFractions;
  confusionCounts.reserve(cuts.size());
  confusionFractions.reserve(cuts.size());
  // Truth-normalized percentages are in [0, 100] by construction.
  double maxMatrixPercent = 100.0;
  for (const auto cut : cuts) {
    const auto counts = buildConfusionCounts(entries, cut, signalLabelValue);
    const auto fractions = buildConfusionFractions(counts);
    confusionCounts.push_back(counts);
    confusionFractions.push_back(fractions);
  }

  TCanvas* confusionCanvas = new TCanvas("confusionCanvas", "Confusion Matrices", 820 * nCols, 720 * nRows);
  confusionCanvas->SetFillColor(kWhite);
  confusionCanvas->Divide(nCols, nRows, 0.035, 0.045);

  std::vector<TH2F*> confusionHists;
  confusionHists.reserve(cuts.size());

  for (int iCut = 0; iCut < nCuts; ++iCut) {
    confusionCanvas->cd(iCut + 1);
    gPad->SetRightMargin(0.045);
    gPad->SetLeftMargin(0.28);
    gPad->SetBottomMargin(0.22);
    gPad->SetTopMargin(0.18);
    gPad->SetTicks(0, 0);
    gPad->SetFillColor(kWhite);

    const auto& counts = confusionCounts[static_cast<std::size_t>(iCut)];
    const auto& fractions = confusionFractions[static_cast<std::size_t>(iCut)];
    auto* hist = new TH2F(Form("confusion_%d", iCut), "", 2, 0.0, 2.0, 2, 0.0, 2.0);
    styleConfusionHistogram(*hist, fractions, maxMatrixPercent);
    hist->Draw("COL");
    drawConfusionPercentLabels(fractions, counts, maxMatrixPercent);

    const double signalEff = (counts.truePositive + counts.falseNegative) > 0
                                 ? static_cast<double>(counts.truePositive) /
                                       static_cast<double>(counts.truePositive + counts.falseNegative)
                                 : 0.0;
    const double bibReject = (counts.trueNegative + counts.falsePositive) > 0
                                 ? static_cast<double>(counts.trueNegative) /
                                       static_cast<double>(counts.trueNegative + counts.falsePositive)
                                 : 0.0;

    TLatex title;
    title.SetNDC(true);
    title.SetTextSize(0.048);
    title.SetTextFont(62);
    title.DrawLatex(0.10, 0.935, Form("BDT cut = %.3f", cuts[static_cast<std::size_t>(iCut)]));

    TLatex metrics;
    metrics.SetNDC(true);
    metrics.SetTextFont(42);
    metrics.SetTextColor(kGray + 3);
    metrics.SetTextSize(0.036);
    metrics.DrawLatex(0.10, 0.865, Form("#varepsilon_{sig} = %.3f   r_{BIB} = %.3f", signalEff, bibReject));

    drawMatrixOutline();

    confusionHists.push_back(hist);
    std::cout << "Cut " << cuts[static_cast<std::size_t>(iCut)]
              << ": TP=" << counts.truePositive
              << ", FP=" << counts.falsePositive
              << ", TN=" << counts.trueNegative
              << ", FN=" << counts.falseNegative << std::endl;
  }

  drawConfusionCanvasOverlay(*confusionCanvas);

  confusionCanvas->SaveAs(Form("%s_confusion.png", outputPrefix));
  confusionCanvas->SaveAs(Form("%s_confusion.pdf", outputPrefix));

  TFile* outFile = TFile::Open(Form("%s_plots.root", outputPrefix), "RECREATE");
  if (rocGraph != nullptr) {
    rocGraph->Write("roc_curve");
  }
  for (auto* hist : confusionHists) {
    hist->Write();
  }
  outFile->Close();

  std::cout << "Wrote plots to: " << outputPrefix << "_roc.[png,pdf], "
            << outputPrefix << "_confusion.[png,pdf], "
            << outputPrefix << "_plots.root" << std::endl;
}