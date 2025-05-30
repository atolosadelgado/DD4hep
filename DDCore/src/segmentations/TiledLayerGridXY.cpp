//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
/*
 * TiledLayerGridXY.cpp
 *
 *  Created on: November 12, 2015
 *      Author: Shaojun Lu, DESY
 */

#include <DDSegmentation/TiledLayerGridXY.h>
#include <DD4hep/Printout.h>

// C/C++ includes
#include <cmath>
#include <string>

namespace dd4hep {
namespace DDSegmentation {

/// default constructor using an encoding string
TiledLayerGridXY::TiledLayerGridXY(const std::string& cellEncoding) :
		CartesianGrid(cellEncoding) {
	// define type and description
	_type = "TiledLayerGridXY";
	_description = "Cartesian segmentation in the local XY-plane using optimal tiling depending on the layer dimensions";
    
	printout(INFO, "TiledLayerGridXY", " ######### dd4hep::DDSegmentation::TiledLayerGridXY() "); 
      
	// register all necessary parameters
	registerParameter("grid_size_x", "Cell size in X", _gridSizeX, 1., SegmentationParameter::LengthUnit);
	registerParameter("grid_size_y", "Cell size in Y", _gridSizeY, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_x", "Cell offset in X", _offsetX, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_y", "Cell offset in Y", _offsetY, 0., SegmentationParameter::LengthUnit, true);
	registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "x");
	registerIdentifier("identifier_y", "Cell ID identifier for Y", _yId, "y");
	registerIdentifier("identifier_layer", "Cell encoding identifier for layer", _identifierLayer, "layer");
	registerParameter("layer_offsetX", "List of layer x offset", _layerOffsetX, std::vector<double>(),
			SegmentationParameter::NoUnit, true);
	registerParameter("layer_offsetY", "List of layer y offset", _layerOffsetY, std::vector<double>(),
			SegmentationParameter::NoUnit, true);
}

/// Default constructor used by derived classes passing an existing decoder
TiledLayerGridXY::TiledLayerGridXY(const BitFieldCoder* decode) : CartesianGrid(decode) {
	// define type and description
	_type = "TiledLayerGridXY";
	_description = "Cartesian segmentation in the local XY-plane using optimal tiling depending on the layer dimensions";
    
	printout(INFO, "TiledLayerGridXY", " ######### dd4hep::DDSegmentation::TiledLayerGridXY() "); 
      
	// register all necessary parameters
	registerParameter("grid_size_x", "Cell size in X", _gridSizeX, 1., SegmentationParameter::LengthUnit);
	registerParameter("grid_size_y", "Cell size in Y", _gridSizeY, 1., SegmentationParameter::LengthUnit);
	registerParameter("offset_x", "Cell offset in X", _offsetX, 0., SegmentationParameter::LengthUnit, true);
	registerParameter("offset_y", "Cell offset in Y", _offsetY, 0., SegmentationParameter::LengthUnit, true);
	registerIdentifier("identifier_x", "Cell ID identifier for X", _xId, "x");
	registerIdentifier("identifier_y", "Cell ID identifier for Y", _yId, "y");
	registerIdentifier("identifier_layer", "Cell encoding identifier for layer", _identifierLayer, "layer");
	registerParameter("layer_offsetX", "List of layer x offset", _layerOffsetX, std::vector<double>(),
			SegmentationParameter::NoUnit, true);
	registerParameter("layer_offsetY", "List of layer y offset", _layerOffsetY, std::vector<double>(),
			SegmentationParameter::NoUnit, true);
}

/// destructor
TiledLayerGridXY::~TiledLayerGridXY() {

}

/// determine the position based on the cell ID
Vector3D TiledLayerGridXY::position(const CellID& cID) const {
	unsigned int _layerIndex;
	Vector3D cellPosition;

	// AHcal: _layerIndex is [1,48], _layerOffsetX is [0,47]
	_layerIndex = _decoder->get(cID,_identifierLayer);

	if ( _layerOffsetX.size() != 0 && _layerIndex <=_layerOffsetX.size() ) {
	  cellPosition.X = binToPosition(_decoder->get(cID,_xId), _gridSizeX, _layerOffsetX[_layerIndex - 1]*_gridSizeX/2.);
	  // check the integer cell boundary in x,
	  if ( ( _layerDimX.size() != 0 && _layerIndex <= _layerDimX.size() )
	       &&( _fractCellSizeXPerLayer.size() != 0 && _layerIndex <=  _fractCellSizeXPerLayer.size() )
	       &&(std::fabs( cellPosition.X ) > ( _layerDimX.at(_layerIndex - 1) - _fractCellSizeXPerLayer.at(_layerIndex - 1) ))
	       )
	    { 
	      // and correct the fraction cell center position in x.
	      cellPosition.X = cellPosition.X/std::fabs(cellPosition.X)
		*(_layerDimX.at(_layerIndex - 1) - _fractCellSizeXPerLayer.at(_layerIndex - 1)/2.0) ;
	    }
	} else {
	  cellPosition.X = binToPosition(_decoder->get(cID,_xId), _gridSizeX, _offsetX);
	}
	cellPosition.Y = binToPosition(_decoder->get(cID,_yId), _gridSizeY, _offsetY);
	return cellPosition;
}

/// determine the cell ID based on the position
  CellID TiledLayerGridXY::cellID(const Vector3D& localPosition, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
	CellID cID = vID ;
	unsigned int _layerIndex;

	// AHcal: _layerIndex is [1,48], _layerOffsetX is [0,47]
	_layerIndex = _decoder->get(cID,_identifierLayer);

	if ( _layerOffsetX.size() != 0 && _layerIndex <=_layerOffsetX.size() ) {
	  _decoder->set(cID,_xId,positionToBin(localPosition.X, _gridSizeX, _layerOffsetX[_layerIndex - 1]*_gridSizeX/2.));
	} else {
	  _decoder->set(cID,_xId,positionToBin(localPosition.X, _gridSizeX, _offsetX));
	}
	_decoder->set(cID,_yId, positionToBin(localPosition.Y, _gridSizeY, _offsetY));
	return cID;
}

std::vector<double> TiledLayerGridXY::cellDimensions(const CellID&) const {
  return {_gridSizeX, _gridSizeY};
}


} /* namespace DDSegmentation */
} /* namespace dd4hep */
