/* -*-c++-*- IfcPlusPlus - www.ifcplusplus.com  - Copyright (C) 2011 Fabian Gerold
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/

#pragma once

#include <ifcpp/model/shared_ptr.h>
#include <ifcpp/model/StatusCallback.h>
#include <ifcpp/model/UnitConverter.h>
#include <ifcpp/IFC4/include/IfcCartesianPoint.h>
#include <ifcpp/IFC4/include/IfcCurve.h>
#include <ifcpp/IFC4/include/IfcLengthMeasure.h>
#include <ifcpp/IFC4/include/IfcLoop.h>
#include <ifcpp/IFC4/include/IfcPolyline.h>
#include <ifcpp/IFC4/include/IfcTrimmingSelect.h>

#include "IncludeCarveHeaders.h"
#include "GeometrySettings.h"

//\brief class to convert IFC point representations into carve input geometry
class PointConverter : public StatusCallback
{
public:
	shared_ptr<UnitConverter>		m_unit_converter;

	PointConverter( shared_ptr<UnitConverter>& uc ): m_unit_converter( uc )
	{
	}

	virtual ~PointConverter(){}

	static void convertIfcCartesianPoint( const shared_ptr<IfcCartesianPoint>& ifc_point, carve::geom::vector<3> & point, double length_factor )
	{
		std::vector<shared_ptr<IfcLengthMeasure> >& coords1 = ifc_point->m_Coordinates;
		if( coords1.size() > 2 )
		{
#ifdef ROUND_IFC_COORDINATES
			double x = round( coords1[0]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
			double y = round( coords1[1]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
			double z = round( coords1[2]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
#else
			double x = coords1[0]->m_value*length_factor;
			double y = coords1[1]->m_value*length_factor;
			double z = coords1[2]->m_value*length_factor;
#endif
			point = carve::geom::VECTOR( x, y, z );
		}
		else if( coords1.size() > 1 )
		{
#ifdef ROUND_IFC_COORDINATES
			double x = round( coords1[0]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
			double y = round( coords1[1]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
#else
			double x = coords1[0]->m_value*length_factor;
			double y = coords1[1]->m_value*length_factor;
#endif
			point = carve::geom::VECTOR( x, y, 0.0 );
		}
	}
	void convertIfcCartesianPointVector( const std::vector<shared_ptr<IfcCartesianPoint> >& points, std::vector<carve::geom::vector<3> >& loop ) const
	{
		const double length_factor = m_unit_converter->getLengthInMeterFactor();
		const size_t num_points = points.size();
		for( size_t i_point = 0; i_point < num_points; ++i_point )
		{
			const shared_ptr<IfcCartesianPoint>& ifc_cartesian_point = points[i_point];
			if( !ifc_cartesian_point )
			{
				continue;
			}

			const std::vector<shared_ptr<IfcLengthMeasure> >& coords = ifc_cartesian_point->m_Coordinates;
			if( coords.size() > 2 )
			{
#ifdef ROUND_IFC_COORDINATES
				double x = round( coords[0]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
				double y = round( coords[1]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
				double z = round( coords[2]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
#else
				double x = coords[0]->m_value*length_factor;
				double y = coords[1]->m_value*length_factor;
				double z = coords[2]->m_value*length_factor;
#endif
				loop.push_back( carve::geom::VECTOR( x, y, z ) );
			}
			else if( coords.size() > 1 )
			{
#ifdef ROUND_IFC_COORDINATES
				double x = round( coords[0]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
				double y = round( coords[1]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
#else
				double x = coords[0]->m_value*length_factor;
				double y = coords[1]->m_value*length_factor;
#endif
				loop.push_back( carve::geom::VECTOR( x, y, 0.0 ) );
			}
			else
			{
#ifdef _DEBUG
				std::cout << "convertIfcCartesianPointVector: ifc_pt->m_Coordinates.size() != 2" << std::endl;
#endif
			}
		}
	}
	void convertIfcCartesianPointVector2D( const std::vector<std::vector<shared_ptr<IfcCartesianPoint> > >& vec_points_in, std::vector<carve::geom::vector<3> >& vertices )
	{
		const double length_factor = m_unit_converter->getLengthInMeterFactor();
		for( size_t ii = 0; ii < vec_points_in.size(); ++ii )
		{
			const std::vector<shared_ptr<IfcCartesianPoint> >& points_inner = vec_points_in[ii];
			for( size_t jj = 0; jj < points_inner.size(); ++jj )
			{
				const shared_ptr<IfcCartesianPoint>& cartesian_point = points_inner[jj];

				if( !cartesian_point )
				{
					continue;
				}

				const std::vector<shared_ptr<IfcLengthMeasure> >& coords = cartesian_point->m_Coordinates;
				if( coords.size() > 2 )
				{
					vertices.push_back( carve::geom::VECTOR( coords[0]->m_value*length_factor, coords[1]->m_value*length_factor, coords[2]->m_value*length_factor ) );
				}
				else if( coords.size() > 1 )
				{
					vertices.push_back( carve::geom::VECTOR( coords[0]->m_value*length_factor, coords[1]->m_value*length_factor, 0.0 ) );
				}
			}
		}
	}
	void convertIfcCartesianPointVectorSkipDuplicates( const std::vector<shared_ptr<IfcCartesianPoint> >& vec_ifc_points, std::vector<carve::geom::vector<3> >& loop ) const
	{
		const double length_factor = m_unit_converter->getLengthInMeterFactor();
		carve::geom::vector<3>  vertex_previous;
		for( size_t ii = 0; ii < vec_ifc_points.size(); ++ii )
		{
			const shared_ptr<IfcCartesianPoint>& cartesian_point = vec_ifc_points[ii];
			double x = 0.0, y = 0.0, z = 0.0;
			const std::vector<shared_ptr<IfcLengthMeasure> >& coords = cartesian_point->m_Coordinates;

			if( coords.size() > 2 )
			{
#ifdef ROUND_IFC_COORDINATES
				x = round( coords[0]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
				y = round( coords[1]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
				z = round( coords[2]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
#else
				x = coords[0]->m_value*length_factor;
				y = coords[1]->m_value*length_factor;
				z = coords[2]->m_value*length_factor;
#endif
			}
			else if( coords.size() > 1 )
			{
#ifdef ROUND_IFC_COORDINATES
				x = round( coords[0]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
				y = round( coords[1]->m_value*length_factor*ROUND_IFC_COORDINATES_UP )*ROUND_IFC_COORDINATES_DOWN;
#else
				x = coords[0]->m_value*length_factor;
				y = coords[1]->m_value*length_factor;
#endif
			}

			// skip duplicate vertices
			if( ii > 0 )
			{
				if( std::abs( x - vertex_previous.x ) < 0.00000001 )
				{
					if( std::abs( y - vertex_previous.y ) < 0.00000001 )
					{
						if( std::abs( z - vertex_previous.z ) < 0.00000001 )
						{
							continue;
						}
					}
				}
			}
			loop.push_back( carve::geom::VECTOR( x, y, z ) );
			vertex_previous.x = x;
			vertex_previous.y = y;
			vertex_previous.z = z;
		}
	}

	//\brief: returns the corresponding angle (radian, 0 is to the right) if the given point lies on the circle. If the point does not lie on the circle, -1 is returned.
	static double getAngleOnCircle( const carve::geom::vector<3>& circle_center, double circle_radius, const carve::geom::vector<3>& trim_point )
	{
		double result_angle = -1.0;
		carve::geom::vector<3> center_trim_point = trim_point - circle_center;
		if( std::abs( center_trim_point.length() - circle_radius ) < 0.0001 )
		{
			carve::geom::vector<3> center_trim_point_direction = center_trim_point;
			center_trim_point_direction.normalize();
			double cos_angle = carve::geom::dot( center_trim_point_direction, carve::geom::vector<3>( carve::geom::VECTOR( 1.0, 0, 0 ) ) );

			if( std::abs( cos_angle ) < 0.0001 )
			{
				if( center_trim_point.y > 0 )
				{
					result_angle = M_PI_2;
				}
				else if( center_trim_point.y < 0 )
				{
					result_angle = M_PI*1.5;
				}
			}
			else
			{
				if( center_trim_point.y > 0 )
				{
					result_angle = acos( cos_angle );
				}
				else if( center_trim_point.y < 0 )
				{
					result_angle = 2.0*M_PI - acos( cos_angle );
				}
			}
		}
		return result_angle;
	}
};
