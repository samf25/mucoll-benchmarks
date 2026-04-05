from Configurables import PixelHitBDTTestAlg


input_file = "reco_output.edm4hep.root"
output_file = "reco_output_with_bdt.edm4hep.root"

bdt = PixelHitBDTTestAlg(
    "PixelHitBDT",
    WeightsFile="../options/TMVAClassification_BDT.weights.xml",
    MethodName="BDT",
    InputHitCollectionName=["VXDBarrelHits"],
    InputRawHitRelationCollectionName=["VXDBarrelRawHitsRelations"],
    InputRawSimHitCollectionName=["VertexBarrelHits_Passed"],
    InputTruthRelationCollectionName=["VXDBarrelHitsRelations"],
    InputTruthSimHitCollectionName=["OverlayVertexBarrelCollection"],
    OutputScoredHitCollectionName=["VXDBarrelHitsWithBDT"],
    OutputBdtScoreCollectionName=["VXDBarrelHitBDTScores"],
    OutputTruthLabelCollectionName=["VXDBarrelHitTruthLabels"],
    MinBdtScore=0.0,
    NumThreads=4,
)

from Configurables import EventDataSvc
evtsvc = EventDataSvc("EventDataSvc") 
from k4FWCore import ApplicationMgr, IOSvc  
svc = IOSvc(
    "IOSvc",
    Input = ["digi_output.edm4hep.root"],  # Input file from simulation
    Output = "digi_output_with_bdt.edm4hep.root", # Output file for digitization
    # Limit materialization to the collections consumed by PixelHitBDTTestAlg.
    # This avoids crashing on unrelated broken relations present in some BIB files.
    CollectionNames = [
        "VXDBarrelHits",
        "VXDBarrelRawHitsRelations",
        "VertexBarrelHits_Passed",
        "VXDBarrelHitsRelations",
        "OverlayVertexBarrelCollection",
    ]
)
svc.outputCommands = ["drop *", 
    "keep VXDBarrelHitsWithBDT",
    "keep VXDBarrelHitBDTScores",
    "keep VXDBarrelHitTruthLabels",
]

# Run the Application Manager
ApplicationMgr(
    TopAlg = [bdt],
    EvtSel = 'NONE',
    EvtMax   = -1,
    ExtSvc = [evtsvc],
)