from GaudiKernel.Constants import INFO, WARNING
from Configurables import DDPlanarDigi
from Configurables import MuonCVXDDigitiser

def new_OTBarrel(args):
    """
    Create a new outer barrel digitiser instance with the given parameters.
    """
    if args.doOverlayFull:
        inputHitCollections = ["OverlayOuterTrackerBarrelCollection"]
    else:
        inputHitCollections = ["OuterTrackerBarrelCollection"]
    return DDPlanarDigi(
        "OTBarrelDigitiser",
        CorrectTimesForPropagation = True,
        IsStrip = False,
        ResolutionT = [0.06],
        ResolutionU = [0.007],
        ResolutionV = [0.09],
        SubDetectorName = "OuterTrackers",
        TimeWindowMax = [0.3],
        TimeWindowMin = [-0.18],
        UseTimeWindow = True,
        SimTrackHitCollectionName = inputHitCollections,
        SimTrkHitRelCollection = ["OTBarrelHitsRelations"],
        TrackerHitCollectionName = ["OTBarrelHits"],
        OutputLevel = INFO
    )

def new_OTEndcap(args):
    """
    Create a new outer endcap digitiser instance with the given parameters.
    """
    if args.doOverlayFull:
        inputHitCollections = ["OverlayOuterTrackerEndcapCollection"]
    else:
        inputHitCollections = ["OuterTrackerEndcapCollection"]
    return DDPlanarDigi(
        "OTEndcapDigitiser",
        CorrectTimesForPropagation = True,
        IsStrip = True,
        ResolutionT = [0.06],
        ResolutionU = [0.007],
        ResolutionV = [0.09],
        SubDetectorName = "OuterTrackers",
        TimeWindowMax = [0.3],
        TimeWindowMin = [-0.18],
        UseTimeWindow = True,
        SimTrackHitCollectionName = inputHitCollections,
        SimTrkHitRelCollection = ["OTEndcapHitsRelations"],
        TrackerHitCollectionName = ["OTEndcapHits"],
        OutputLevel = INFO
    )

def new_OTBarrel_Realistic(args):
    """
    Create a new outer barrel digitiser instance with realistic digitization.
    """
    if args.doOverlayFull:
        inputHitCollections = ["OverlayOuterTrackerBarrelCollection"]
    else:
        inputHitCollections = ["OuterTrackerBarrelCollection"]
    return MuonCVXDDigitiser(
        "OuterBarrelDigitiser",
        PoissonSmearing = 1,
        PixelSizeY = 10.0,
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
        SubDetectorName = "OuterTrackerBarrel",
        CollectionName = inputHitCollections,
        RelationColName = ["OTBarrelHitsRelations"],
        SimHitLocCollectionName = ["OTBarrelHits_Passed"],
        RawHitsLinkColName = ["OTBarrelRawHitsRelations"],
        OutputCollectionName = ["OTBarrelHits"],
        OutputLevel = INFO
    )

def new_OTEndcap_Realistic(args):
    """
    Create a new outer endcap digitiser instance with realistic digitization.
    """
    if args.doOverlayFull:
        inputHitCollections = ["OverlayOuterTrackerEndcapCollection"]
    else:
        inputHitCollections = ["OuterTrackerEndcapCollection"]
    return MuonCVXDDigitiser(
        "OuterEndcapDigitiser",
        PoissonSmearing = 1,
        PixelSizeY = 10.0,
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
        LayerIDs = [0, 1, 2, 3],
        SubDetectorName = "OuterTrackerEndcap",
        CollectionName = inputHitCollections,
        RelationColName = ["OTEndcapHitsRelations"],
        SimHitLocCollectionName = ["OTEndcapHits_Passed"],
        RawHitsLinkColName = ["OTEndcapRawHitsRelations"],
        OutputCollectionName = ["OTEndcapHits"],
        OutputLevel = INFO
    )