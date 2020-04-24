#!/usr/bin/env python
"""
_Utils_
Module containing some utility tools
"""

def Customize(process):
	
	import FWCore.ParameterSet.Config as cms

	_seedsFlagsCleaner_ECAL = cms.PSet(
	    algoName = cms.string("FlagsCleanerECAL"),
    	    RecHitFlagsToBeExcluded= cms.vstring('kNeighboursRecovered')
	)
	
	process.particleFlowClusterECALUncorrected.seedCleaners = cms.VPSet(_seedsFlagsCleaner_ECAL) 

	return process
