// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SORTIE_API MarchingConst
{
	// Values from http://paulbourke.net/geometry/polygonise/
public:
 	static const int Edges[256];
	static const int Triangulation[256][16];
	static const int CornerIndexAFromEdge[12];
	static const int CornerIndexBFromEdge[12];
};
