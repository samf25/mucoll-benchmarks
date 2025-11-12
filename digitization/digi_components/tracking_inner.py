from GaudiKernel.Constants import INFO, WARNING
from Configurables import DDPlanarDigi
from Configurables import MuonCVXDDigitiser

def new_ITBarrel(args):
    """
    Create a new inner barrel digitiser instance with the given parameters.
    """
    if args.doOverlayFull:
        inputHitCollections = ["OverlayInnerTrackerBarrelCollection"]
    else:
        inputHitCollections = ["InnerTrackerBarrelCollection"]
    return DDPlanarDigi(
        "InnerBarrelDigitiser",
        CorrectTimesForPropagation = True,
        IsStrip = True,
        ResolutionT = [0.06],
        ResolutionU = [0.007],
        ResolutionV = [0.09],
        SubDetectorName = "InnerTrackers",
        TimeWindowMax = [0.3],
        TimeWindowMin = [-0.18],
        UseTimeWindow = True,
        SimTrackHitCollectionName = inputHitCollections,
        SimTrkHitRelCollection = ["ITBarrelHitsRelations"],
        TrackerHitCollectionName = ["ITBarrelHits"],
        OutputLevel = INFO
    )

def new_ITEndcap(args):
    """
    Create a new inner endcap digitiser instance with the given parameters.
    """
    if args.doOverlayFull:
        inputHitCollections = ["OverlayInnerTrackerEndcapCollection"]
    else:
        inputHitCollections = ["InnerTrackerEndcapCollection"]
    return DDPlanarDigi(
        "InnerEndcapDigitiser",
        CorrectTimesForPropagation = True,
        IsStrip = False,
        ResolutionT = [0.06],
        ResolutionU = [0.007],
        ResolutionV = [0.09],
        SubDetectorName = "InnerTrackers",
        TimeWindowMax = [0.3],
        TimeWindowMin = [-0.18],
        UseTimeWindow = True,
        SimTrackHitCollectionName = inputHitCollections,
        SimTrkHitRelCollection = ["ITEndcapHitsRelations"],
        TrackerHitCollectionName = ["ITEndcapHits"],
        OutputLevel = INFO
    )

def new_ITBarrel_Realistic(args):
    """
    Create a new inner barrel digitiser instance with realistic digitization.
    """
    if args.doOverlayFull:
        inputHitCollections = ["OverlayInnerTrackerBarrelCollection"]
    else:
        inputHitCollections = ["InnerTrackerBarrelCollection"]
    return MuonCVXDDigitiser(
        "InnerBarrelDigitiser",
        PoissonSmearing = 1,
        PixelSizeY = 1.0,
        PixelSizeX = 0.050,
        TanLorentzY = 0.0,
        TanLorentz = 0.8,
        # Diffusion = 0.07,
        Threshold = 1000.0,
        DigitizeTime = 0,
        DigitizeCharge = 1,
        EnergyLoss = 280.0,
        SegmentLength = 0.005,
        MaxTrackLength = 10.0,
        MaxEnergyDelta = 100.0,
        CutOnDeltaRays = 0.030,
        ChargeMaximum = 60000.0,
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
        TimeSmearingSigma = 0.060,
        LayerIDs = [0, 1, 2],
        SubDetectorName = "InnerTrackerBarrel",
        CollectionName = inputHitCollections,
        RelationColName = ["ITBarrelHitsRelations"],
        SimHitLocCollectionName = ["ITBarrelHits_Passed"],
        RawHitsLinkColName = ["ITBarrelRawHitsRelations"],
        OutputCollectionName = ["ITBarrelHits"],
        OutputLevel = INFO
    )

def new_ITEndcap_Realistic(args):
    """
    Create a new inner endcap digitiser instance with realistic digitization.
    """
    if args.doOverlayFull:
        inputHitCollections = ["OverlayInnerTrackerEndcapCollection"]
    else:
        inputHitCollections = ["InnerTrackerEndcapCollection"]
    return MuonCVXDDigitiser(
        "InnerEndcapDigitiser",
        PoissonSmearing = 1,
        PixelSizeY = 1.0,
        PixelSizeX = 0.050,
        TanLorentzY = 0.0,
        TanLorentz = 0.0,
        # Diffusion = 0.07,
        Threshold = 1000.0,
        DigitizeTime = 0,
        DigitizeCharge = 1,
        EnergyLoss = 280.0,
        SegmentLength = 0.005,
        MaxTrackLength = 10.0,
        MaxEnergyDelta = 100.0,
        CutOnDeltaRays = 0.030,
        ChargeMaximum = 60000.0,
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
        TimeSmearingSigma = 0.060,
        LayerIDs = [0, 1, 2, 3, 4, 5, 6],
        SubDetectorName = "InnerTrackerEndcap",
        CollectionName = inputHitCollections,
        RelationColName = ["ITEndcapHitsRelations"],
        SimHitLocCollectionName = ["ITEndcapHits_Passed"],
        RawHitsLinkColName = ["ITEndcapRawHitsRelations"],
        OutputCollectionName = ["ITEndcapHits"],
        OutputLevel = INFO
    )