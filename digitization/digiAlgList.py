from GaudiKernel.Constants import INFO, WARNING

def makeDigiAlgList(the_args):
    '''-------------------------------------------------------------'''
    '''    Add the Digitization Algorithms to the Algorithm List    '''
    '''-------------------------------------------------------------'''
    algList = []
    # BIB Overlay
    if the_args.doOverlayFull:
        from digi_components.overlay_full import new_overlay_full
        algList.append(new_overlay_full(the_args))

    # Tracker Digitization
    if (the_args.doRealisticDigi):
        from digi_components.tracking_vertex import new_VXDBarrel_Realistic, new_VXDEndcap_Realistic
        from digi_components.tracking_inner import new_ITBarrel_Realistic, new_ITEndcap_Realistic
        from digi_components.tracking_outer import new_OTBarrel_Realistic, new_OTEndcap_Realistic
        algList.append(new_VXDBarrel_Realistic(the_args))
        algList.append(new_VXDEndcap_Realistic(the_args))
        algList.append(new_ITBarrel_Realistic(the_args))
        algList.append(new_ITEndcap_Realistic(the_args))
        algList.append(new_OTBarrel_Realistic(the_args))
        algList.append(new_OTEndcap_Realistic(the_args))
    else:
        from digi_components.tracking_vertex import new_VXDBarrel, new_VXDEndcap
        from digi_components.tracking_inner import new_ITBarrel, new_ITEndcap
        from digi_components.tracking_outer import new_OTBarrel, new_OTEndcap
        algList.append(new_VXDBarrel(the_args))
        algList.append(new_VXDEndcap(the_args))
        algList.append(new_ITBarrel(the_args))
        algList.append(new_ITEndcap(the_args))
        algList.append(new_OTBarrel(the_args))
        algList.append(new_OTEndcap(the_args))

    # EM, Hadronic, Muon Calorimeter Digitization
    from digi_components.calorimetry_EM import new_ECalBarrelDigi, new_ECalBarrelReco
    from digi_components.calorimetry_EM import new_ECalPlugDigi, new_ECalPlugReco
    from digi_components.calorimetry_EM import new_ECalEndcapDigi, new_ECalEndcapReco
    algList.append(new_ECalBarrelDigi(the_args))
    algList.append(new_ECalBarrelReco())
    #algList.append(new_ECalPlugDigi(the_args))
    #algList.append(new_ECalPlugReco())
    algList.append(new_ECalEndcapDigi(the_args))
    algList.append(new_ECalEndcapReco())
    from digi_components.calorimetry_HAD import new_HCalBarrelDigi, new_HCalBarrelReco
    from digi_components.calorimetry_HAD import new_HCalEndcapDigi, new_HCalEndcapReco
    from digi_components.calorimetry_HAD import new_HCalRingDigi, new_HCalRingReco
    algList.append(new_HCalBarrelDigi(the_args))
    algList.append(new_HCalBarrelReco())
    algList.append(new_HCalEndcapDigi(the_args))
    algList.append(new_HCalEndcapReco())
    #algList.append(new_HCalRingDigi(the_args))
    #algList.append(new_HCalRingReco())
    from digi_components.calorimetry_MU import new_MuonBarrelDigi, new_MuonEndcapDigi
    algList.append(new_MuonBarrelDigi(the_args))
    algList.append(new_MuonEndcapDigi(the_args))

    # Vertex Filtering
    if the_args.doFilterDL:
        from digi_components.filterDL_vertex import new_filterDL_vertexBarrel, new_filterDL_vertexEndcap
        algList.append(new_filterDL_vertexBarrel())
        algList.append(new_filterDL_vertexEndcap())

    return algList
