from GaudiKernel.Constants import INFO, WARNING
from Configurables import DDPlanarDigi
from Configurables import MuonCVXDDigitiser

def new_VXDBarrel(args):
    """
    Create a new vertex barrel instance with the given parameters.
    """
    if args.doOverlayFull:
        inputHitCollections = ["OverlayVertexBarrelCollection"]
    else:
        inputHitCollections = ["VertexBarrelCollection"]
    return DDPlanarDigi(
        "VXDBarrelDigitiser",
        CorrectTimesForPropagation = True,
        IsStrip = False,
        ResolutionT = [0.03],
        ResolutionU = [0.005],
        ResolutionV = [0.005],
        SubDetectorName = "Vertex",
        TimeWindowMax = [0.15],
        TimeWindowMin = [-0.09],
        UseTimeWindow = True,
        SimTrackHitCollectionName = inputHitCollections,
        SimTrkHitRelCollection = ["VXDBarrelHitsRelations"],
        TrackerHitCollectionName = ["VXDBarrelHits"],
        OutputLevel = INFO
    )

def new_VXDEndcap(args):
    """
    Create a new vertex endcap instance with the given parameters.
    """
    if args.doOverlayFull:
        inputHitCollections = ["OverlayVertexEndcapCollection"]
    else:
        inputHitCollections = ["VertexEndcapCollection"]
    return DDPlanarDigi(
        "VXDEndcapDigitiser",
        CorrectTimesForPropagation = True,
        IsStrip = False,
        ResolutionT = [0.03],
        ResolutionU = [0.005],
        ResolutionV = [0.005],
        SubDetectorName = "Vertex",
        TimeWindowMax = [0.15],
        TimeWindowMin = [-0.09],
        UseTimeWindow = True,
        SimTrackHitCollectionName = inputHitCollections,
        SimTrkHitRelCollection = ["VXDEndcapHitsRelations"],
        TrackerHitCollectionName = ["VXDEndcapHits"],
        OutputLevel = INFO
    )

def new_VXDBarrel_Realistic(args):
    """
    Create a new vertex barrel digitiser instance with realistic digitization.
    """
    if args.doOverlayFull:
        inputHitCollections = ["OverlayVertexBarrelCollection"]
    else:
        inputHitCollections = ["VertexBarrelCollection"]
    return MuonCVXDDigitiser(
        "VertexBarrelDigitiser",
        PoissonSmearing = 1,
        PixelSizeY = 0.025,
        PixelSizeX = 0.025,
        TanLorentzY = 0.0,
        TanLorentz = 0.8,
        # Diffusion = 0.07,
        Threshold = 500,
        DigitizeTime = 0,
        DigitizeCharge = 1,
        EnergyLoss = 280.0,
        SegmentLength = 0.005,
        MaxTrackLength = 10.0,
        MaxEnergyDelta = 100.0,
        CutOnDeltaRays = 0.030,
        ChargeMaximum = 15000.0,
        ElectronsPerKeV = 270.3,
        TimeMaximum = 15.0,
        ElectronicNoise = 80,
        StoreFiredPixels = 1,
        ElectronicEffects = 1,
        TimeDigitizeBinning = 0,
        ThresholdSmearSigma = 25,
        TimeDigitizeNumBits = 10,
        ChargeDigitizeBinning = 1,
        ChargeDigitizeNumBits = 4,
        TimeSmearingSigma = 0.03,
        LayerIDs = [0, 1, 2, 4, 6],
        SubDetectorName = "VertexBarrel",
        CollectionName = inputHitCollections,
        RelationColName = ["VXDBarrelHitsRelations"],
        SimHitLocCollectionName = ["VertexBarrelHits_Passed"],
        RawHitsLinkColName = ["VXDBarrelRawHitsRelations"],
        OutputCollectionName = ["VXDBarrelHits"],
        OutputLevel = INFO
    )

def new_VXDEndcap_Realistic(args):
    """
    Create a new vertex endcap digitiser instance with realistic digitization.
    """
    if args.doOverlayFull:
        inputHitCollections = ["OverlayVertexEndcapCollection"]
    else:
        inputHitCollections = ["VertexEndcapCollection"]
    return MuonCVXDDigitiser(
        "VertexEndcapDigitiser",
        PoissonSmearing = 1,
        PixelSizeY = 0.025,
        PixelSizeX = 0.025,
        TanLorentzY = 0.0,
        TanLorentz = 0.0,
        # Diffusion = 0.07,
        Threshold = 500,
        DigitizeTime = 0,
        DigitizeCharge = 1,
        EnergyLoss = 280.0,
        SegmentLength = 0.005,
        MaxTrackLength = 10.0,
        MaxEnergyDelta = 100.0,
        CutOnDeltaRays = 0.030,
        ChargeMaximum = 15000.0,
        ElectronsPerKeV = 270.3,
        TimeMaximum = 15.0,
        ElectronicNoise = 80,
        StoreFiredPixels = 1,
        ElectronicEffects = 1,
        TimeDigitizeBinning = 0,
        ThresholdSmearSigma = 25,
        TimeDigitizeNumBits = 10,
        ChargeDigitizeBinning = 1,
        ChargeDigitizeNumBits = 4,
        TimeSmearingSigma = 0.03,
        LayerIDs = [0, 1, 2, 3, 4, 5, 6, 7],
        SubDetectorName = "VertexEndcap",
        CollectionName = inputHitCollections,
        RelationColName = ["VXDEndcapHitsRelations"],
        SimHitLocCollectionName = ["VertexEndcapHits_Passed"],
        RawHitsLinkColName = ["VXDEndcapRawHitsRelations"],
        OutputCollectionName = ["VXDEndcapHits"],
        OutputLevel = INFO
    )