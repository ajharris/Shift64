// Measure.cpp: implementation of the CMeasure class.
//

//////////////////////////////////////////////////////////////////////
////#include "stdafx.h"


//visualizer classes
#include "VisLib.h"
#include "geometry.h"
#include "measure.h"

#include <float.h>
#include <algorithm>

bool compare1(const Vector3 &p, const Vector3 &q);
bool compare4(const MPoly2D &p, const MPoly2D &q);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------
// CMeasure constructor
//
//----------------------------------------------------------------------
CMeasure::CMeasure()
{
	// initializing measurement IDs
	m_pointID  =  1;
	m_lineID   =  1;
	m_areaID   =  1;
	m_volumeID =  1;
	m_angleID  =  1;
	m_segID    =  1;

	m_curObj   = -1;
	m_curVol   = -1;

//initializing this gets rid of the little greeen monster artifact
	m_orientMark.endX = Vector3(0, 0, 0);
    m_orientMark.endY = Vector3(0, 0, 0);
    m_orientMark.endZ = Vector3(0, 0, 0);
    m_orientMark.label = Vector3(0, 0, 0);
    m_orientMark.start = Vector3(0, 0, 0);

    minDist = 0.01f;

}


//----------------------------------------------------------------------
// CMeasure destructor
//
//----------------------------------------------------------------------
CMeasure::~CMeasure()
{
	// put everything in one list
	while(!m_undoList.empty()) 
	{
		m_measureList.push_back(m_undoList.back());
		m_undoList.pop_back();
	};

	// delete measurement list
	// For each measurement type, the data saving format is different
	while (!m_measureList.empty()) 
	{
		switch (m_measureList.back()->type) 
		{
			case MPOINT: // point 
                delete reinterpret_cast<MPoint *>(m_measureList.back());
                break;
			case MLINE:  // line
                delete reinterpret_cast<MLine *>(m_measureList.back());
                break;
			case MANGLE: // angle
				delete reinterpret_cast<MAngle *>(m_measureList.back());
				break;
		
			case MAREA: // area
				delete reinterpret_cast<MPoly2D *>(m_measureList.back());
				break;
		
			case MVOLUME: // volume
				delete reinterpret_cast<MPoly3D *>(m_measureList.back());
				break;
		}

		m_measureList.pop_back();
	};

}


//----------------------------------------------------------------------
// Select
//
// Select an item from the list
//
// Input:
// index in the measurement list
//
// Output:
// none
//----------------------------------------------------------------------

bool CMeasure::Select(long index)
{
	ClearAttrib(m_curObj, MATTR_SELECT);
	m_curObj = index;

	return SetAttrib(index, MATTR_SELECT);
}


//----------------------------------------------------------------------
// SetAttrib
//
// Set the specified attribute. Don't disturb others 
//
// Input:
// index in the measurement list
// new attribute value to set
//
// Output:
// none
//----------------------------------------------------------------------
bool CMeasure::SetAttrib(long index, unsigned short attrib)
{
	if (index >= 0 && index < m_measureList.size())
	{
		m_measureList[index]->attrib |= attrib;
		return true;
	}
	else
	{
		return false;
	}
}


//----------------------------------------------------------------------
// ClearAttrib
//
// Clear the specified attribute. Don't disturb others
//
// index in the measurement list
// new attribute value to clear
//
// Output:
// none
//----------------------------------------------------------------------
bool CMeasure::ClearAttrib(long index, unsigned short attrib)
{
	if (index >= 0 && index < m_measureList.size())
	{
		m_measureList[index]->attrib &= ~attrib;
		return true;
	}
	else
	{
		return false;
	}
}


//----------------------------------------------------------------------
// Delete
//
// Deleting a selected item from the list
//
// Input:
// index in the measurement list
//
// Output:
// none 
//----------------------------------------------------------------------
long CMeasure::Delete(long index)
{
	if (m_measureList.empty() || index <0 || index >= m_measureList.size())
	{
		index = -1;
	}
	else
	{
		ClearAttrib(index, MATTR_INCOMPLETE);
		m_undoList.push_back(m_measureList[index]);

		if (index == m_curVol)
		{
			m_curVol = -1;
		}

		m_measureList.erase(m_measureList.begin()+index);
	}

	return index;
}


//----------------------------------------------------------------------
// Undo
//
// Undo last deletion 
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::Undo()
{
	if (!m_undoList.empty()) 
	{
		m_measureList.push_back(m_undoList.back());
		m_undoList.pop_back();
	}
}


//----------------------------------------------------------------------
// Redo
//
// Undo last measurement 
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::Redo()
{
	if (!m_measureList.empty()) 
	{
		m_undoList.push_back(m_measureList.back());
		m_measureList.pop_back();
	}
}


//----------------------------------------------------------------------
// Clear
//
// Clear all the measurements 
//
// Input:
// none
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::Clear()
{
	// initialize the IDs
	m_pointID = m_lineID = m_areaID = m_volumeID = m_angleID = m_segID = 1;
	
	// put everything in one list
	while(!m_undoList.empty()) 
	{
		m_measureList.push_back(m_undoList.back());
		m_undoList.pop_back();
	};

	// delete measurement list
	// For each measurement type, the data saving format is different
	while(!m_measureList.empty()) 
	{
		switch (m_measureList.back()->type) 
		{
			case MPOINT: // point 
			case MLINE:  // line
			case MANGLE: // angle
				delete m_measureList.back();
				break;
			case MAREA: // area
				delete (MPoly2D *)(m_measureList.back());
				break;
			case MVOLUME: // volume
				delete (MPoly3D *)(m_measureList.back());
				break;
		}

		m_measureList.pop_back();

	};

	m_curObj = -1;

	//empty the curVol vector
	if (m_curVol >= 0 && m_curVol < m_measureList.size())
	{
		((MPoly3D *)m_measureList[m_curVol])->poly3d.erase(((MPoly3D *)m_measureList[m_curVol])->poly3d.begin(), ((MPoly3D *)m_measureList[m_curVol])->poly3d.end());
	}

	m_curVol = -1;
}


//----------------------------------------------------------------------
// Clear
//
// Delete all objects of a specified type and/or subtype
//
// Input:
// type specifies if its a point, line, circle, polygon, volume etc
// measurement subtype - allows 2^16 subtypes
//
// Output:
// none 
//----------------------------------------------------------------------
void CMeasure::Clear(unsigned short type, unsigned short subtype)
{
	if (type || subtype)
	{
		for (MObjpVec::iterator i = m_measureList.begin(); i != m_measureList.end();)
		{
			if ((type && (*i)->type != type) || (subtype && (*i)->subtype != subtype))
			{
				i++;
				continue;
			}

			if (m_measureList.begin() + m_curObj == i) // if we happen to delete current selection, reset it
			{
				m_curObj = -1;
			}

			switch ((*i)->type) 
			{
				case MPOINT: // point 
				case MLINE:  // line
				case MANGLE: // angle
					delete *i;
					break;

				case MAREA: // area
					delete (MPoly2D *)*i;
					break;

				case MVOLUME: // volume
					delete (MPoly3D *)*i;
					break;
			}

			m_measureList.erase(i);
		}
	}
	else
	{
		Clear(); // clear all
	}
}


//----------------------------------------------------------------------
// SetName
//
// Give a name to measurement in the measurement list 
//
// Input:
// index in the measurement list
// new name
//
// Output:
// none
//----------------------------------------------------------------------
bool CMeasure::SetName(long index, const char *name)
{
	bool ret = false;

	if (index >= 0 && index < m_measureList.size())
	{
		memcpy(m_measureList[index]->name, name, MAX_VIEW_NAME+1);
		ret = true;
	}

	return ret;
}


//----------------------------------------------------------------------
// GetName
//
// Get name of measurement from the measurement list
//
// Input:
// index in the measurement list
//
// Output:
// string contains name 
//----------------------------------------------------------------------
bool CMeasure::GetName(long index, char *name)
{
	bool ret = false;

	if (index >= 0 && index < m_measureList.size())
	{
		memcpy(name, m_measureList[index]->name, MAX_VIEW_NAME+1);
		ret = true;
	}

	return ret;
}


//----------------------------------------------------------------------
// SetOrientation
//
// Set the orientation markers 
//
// Input:
// parameters for the orientation marker
//
// Output:
// none
//----------------------------------------------------------------------

void CMeasure::SetOrientation(OrientMarker m)
{
	m_orientMark.endX = m.endX;
	m_orientMark.endY = m.endY;
	m_orientMark.endZ = m.endZ;
	m_orientMark.label = m.label;
	m_orientMark.start = m.start;
}


//----------------------------------------------------------------------
// MovePt
//
// Move point from a 2D polygon or 3D volume. We assume that the offset 
// causes the point to move in-plane. So, no checking is done 
//
// Input:
// Vertex3 structure contains the offset for the move
// vertex is the index of the point to move
// section is the index of the contour with the point to move
//
// Output:
// none
//---------------------------------------------------------------------- 
bool CMeasure::MovePt(Vector3 offset, long vertex, long section)
{
	if (m_curObj < 0 || m_curObj >= m_measureList.size())
	{
		return false;
	}

	MObj *obj = m_measureList[m_curObj];
	bool ret = false;
	MPoly2D *poly = 0;

	switch (obj->type)
	{
		case MAREA:
			poly = (MPoly2D *)obj;
			break;

		case MVOLUME:
			if (section >= 0 && section < ((MPoly3D *)obj)->poly3d.size())
			{
				poly =  &((MPoly3D *)obj)->poly3d[section];
			}
			break;
	}

	if (poly && vertex >= 0 && vertex < poly->poly2d.size())
	{
		if (vertex == poly->poly2d.size() -1)
		{
			vertex = 0;
		}
		
		poly->poly2d[vertex] += offset;
		poly->poly2d[vertex].a = true;

		if (vertex == 0) // shift the end points as well
		{
			poly->poly2d[poly->poly2d.size() -1] += offset;
		}

		polyParam(poly);

		if (obj->type == MVOLUME)
		{
			((MPoly3D *)obj)->param[0] = calcVolume((MPoly3D *)obj); // calculate the volume
		}

		ret = true;
	}

	return ret;
}


//----------------------------------------------------------------------
// AnchorPt
//
// Set anchor point flag on/off
//
// Input:
// vertex is the index of the point to anchor
// section is the index of the contour with the point to anchor
// fix is true to anchor, false to unanchor
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::AnchorPt(long vertex, long section, bool fix)
{
	if (m_curObj < 0 || m_curObj >= m_measureList.size())
	{
		return;
	}

	MObj *obj = m_measureList[m_curObj];
	bool ret = false;
	MPoly2D *poly = 0;

	switch (obj->type)
	{
		case MAREA:
			poly = (MPoly2D *)obj;
			break;

		case MVOLUME:
			if (section >= 0 && section < ((MPoly3D *)obj)->poly3d.size())
			{
				poly =  &((MPoly3D *)obj)->poly3d[section];
			}
			break;
	} 

	if (poly && vertex >= 0 && vertex < poly->poly2d.size())
	{
		if (vertex == poly->poly2d.size() -1)
		{
			vertex = 0;
		}

		poly->poly2d[vertex].a = fix;
	}

}


//----------------------------------------------------------------------
// AnchorPts
//
// Set anchor point flags on/off
//
// Input:
// section is the index of the contour with the point to anchor
// fix is true to anchor, false to unanchor
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::AnchorPts(long section, bool fix)
{
	if (m_curObj < 0 || m_curObj >= m_measureList.size())
	{
		return;
	}

	MObj *obj = m_measureList[m_curObj];
	bool ret = false;
	MPoly2D *poly = 0;

	switch (obj->type)
	{
		case MAREA:
			poly = (MPoly2D *)obj;
			break;

		case MVOLUME:
			if (section >= 0 && section < ((MPoly3D *)obj)->poly3d.size())
			{
				poly =  &((MPoly3D *)obj)->poly3d[section];
			}
			break;
	} 

	for(long i = 0; i < poly->poly2d.size(); i++)
	{
		poly->poly2d[i].a = fix;
	}

}


//----------------------------------------------------------------------
// DeletePt
//
// Delete point from a 2D polygon or 3D volume
//
// Input:
// Vector3 contains the point to delete if found in the measurements
// tolerance offset in mm
//
// Output:
// Vector3 contains the actual deleted point
//----------------------------------------------------------------------
bool CMeasure::DeletePt(Vector3 pt, Vector3 *pt1, float delta)
{

	if (m_curObj < 0 || m_curObj >= m_measureList.size())
	{
		return false;
	}

	MObj *obj = m_measureList[m_curObj];
	bool ret = false;
	MPoly2D *poly = 0;
	long index = 0;

	switch (obj->type)
	{
		case MAREA:
			poly = (MPoly2D *)obj;
			break;

		case MVOLUME:
			{
				float min_dist = delta;
				MPoly2D* isection = 0;
			
				for (long ip = 0; ip < ((MPoly3D *)obj)->poly3d.size(); ip++)
				{
					MPoly2D* ipa = &((MPoly3D *)obj)->poly3d[ip];

					float dist = fabs((pt % ipa->planeNormal) - ipa->planeD);

					if (dist < min_dist)
					{
						isection = ipa;
						min_dist = dist;
					}
				}
				poly = isection;
			}
			
			break;
	} 

	if (poly)
	{
		long i = getClosest(pt, poly->poly2d, delta);
		if (i >= 0 && i < poly->poly2d.size() && poly->poly2d.size() > 4) // must have 3 or more points (end points are duplicated)
		{
			*pt1 = poly->poly2d[i];
			if (i == poly->poly2d.size() - 1 || i == 0)
			{
				
				// erase both first and last points and copy the (now) first element back
				poly->poly2d.erase(poly->poly2d.begin());
				poly->poly2d.pop_back();
				poly->poly2d.push_back(poly->poly2d.front());
			}
			else
			{
				poly->poly2d.erase(poly->poly2d.begin()+i);
			}

			polyParam(poly);

			if (obj->type == MVOLUME)
			{
				((MPoly3D *)obj)->param[0] = calcVolume((MPoly3D *)obj); // calculate the volume
			}

			ret = true;
		}
	}

	return ret;
}


//----------------------------------------------------------------------
// GetVertex
//
// Returns (true if successfull) the index of the vertex closest to point. 
// 'section' is the section no. if its a volume.
//
// Input:
// Vector3 point contains the point to search
// long section is the contour index
// float delta is tolerance offset in mm
//
// Output:
// long vertex contains vertex index of actual point
//----------------------------------------------------------------------
bool CMeasure::GetVertex(const Vector3 point, long *vertex, long *section, float delta)
{
	if (m_curObj < 0 || m_curObj >= m_measureList.size())
	{
		return false;
	}

	MObj *obj = m_measureList[m_curObj];
	long index = -1;
	bool ret = false;
	MPoly2D *poly = 0;

	switch (obj->type)
	{
		case MAREA:
			poly = (MPoly2D *)obj;
			break;

		case MVOLUME:
			
			{
				float min_dist = delta;

				for (long ip = 0; ip < ((MPoly3D *)obj)->poly3d.size(); ip++)
				{
					MPoly2D* ipa = &((MPoly3D *)obj)->poly3d[ip];

					float dist = fabs((point % ipa->planeNormal) - ipa->planeD);

					if (dist < min_dist)
					{
						poly = ipa;
						min_dist = dist;
						*section = ip;
					}
				}
			}

			break;
	} 
	
	if (poly)
	{
		*vertex = getClosest(point, poly->poly2d, delta);
		ret = (*vertex >=0 && *vertex < poly->poly2d.size());
	}

	return ret;
}


//----------------------------------------------------------------------
// GetEdge
//
// Returns (true if successfull) the index of the first point of the edge closest 
// to point. 'section' is the section no. if its a volume 
//
// Input:
// Vector3 point contains the point to search
// long section is the contour index
// float delta is tolerance offset in mm
//
// Output:
// long vertex contains vertex index of actual point
//----------------------------------------------------------------------
bool CMeasure::GetEdge(const Vector3 point, long *vertex, long *section, float delta)
{
	if (m_curObj < 0 || m_curObj >= m_measureList.size())
	{
		return false;
	}

	MObj *obj = m_measureList[m_curObj];
	long index = -1;
	bool ret = false;
	MPoly2D *poly = 0;

	switch (obj->type)
	{
		case MAREA:
			poly = (MPoly2D *)obj;
			break;

		case MVOLUME:
			*section = getClosestSection(point, (MPoly3D *)obj, delta);
			poly = &(((MPoly3D *)obj)->poly3d[*section]);
			break;
	} 

	if (poly)
	{
		*vertex = getClosestEdge(point, poly->poly2d, delta);
		ret = (*vertex >=0 && *vertex < poly->poly2d.size());
	}

	return ret;
}


//----------------------------------------------------------------------
// getClosestSection
//
// Find the closest section of 3D object to a given point 
//
// Input:
// Vector3 point contains the point to search
// MPoly3D vol is the 3D volume measurement to search
// float delta is the tolerance in mm for the search 
//
// Output:
// index to the contour that is closest to the point
//----------------------------------------------------------------------
long CMeasure::getClosestSection(Vector3 point, MPoly3D *vol, float delta)
{
	float min_dist = delta;
	long section = -1;

	for (MPoly2DVec::iterator ipa = vol->poly3d.begin(); ipa != vol->poly3d.end(); ipa++)
	{
		float dist = fabs((point % ipa->planeNormal) - ipa->planeD);

		if (dist < min_dist)
		{
			min_dist = dist;
			section = ipa - vol->poly3d.begin();
		}
	}

	return section;
}


//----------------------------------------------------------------------
// AddPt
//
// Add point to an area or volume object
//
// Input:
// Vector3 pt is the point to add
// long insert is the index in the contour where to add the point
// long section is contour index
//
// Output:
// true if success, false if not 
//----------------------------------------------------------------------
bool CMeasure::AddPt(Vector3 pt, long insert, long section)
{
	if (m_curObj < 0 || m_curObj >= m_measureList.size())
	{
		return false;
	}

	MObj *obj = m_measureList[m_curObj];
	bool ret = false;
	MPoly2D *poly = 0;
	float PlaneD;
	Vector3 Plnormal;

	switch (obj->type)
	{
		case MAREA:
			poly = (MPoly2D *)obj;
			break;

		case MVOLUME:
			if (section >= 0 && section < ((MPoly3D *)obj)->poly3d.size())
			{
				PlaneD = ((MPoly3D *)obj)->poly3d[section].planeD;
				Plnormal = ((MPoly3D *)obj)->poly3d[section].planeNormal;

				if (fabs(Plnormal.x * pt.x + Plnormal.y * pt.y + Plnormal.z * pt.z - PlaneD) < 0.5)
				{
					poly = &(((MPoly3D *)obj)->poly3d[section]);
				}
			}
			break;
	} 
	

	if (poly)
	{
		// worry only about inserting before the first. We can never insert after the last
		if (insert == 0) 
		{
			poly->poly2d.back() = pt;
		}

		poly->poly2d.insert(poly->poly2d.begin()+insert, pt);

		polyParam(poly);

		if (obj->type == MVOLUME)
		{
			((MPoly3D *)obj)->param[0] = calcVolume((MPoly3D *)obj); // re-calculate the volume
		}

		ret = true;
	}

	return ret;
}


//----------------------------------------------------------------------
// inPolygon
//
// Find out whether a given point is inside of 2D polygon (area or 3D volume
// contour)
//
// Input:
// MPoly2D ipa is the polygon to search
// Vector3 pt is the point to search
//
// Output:
// true if found, false if not
//----------------------------------------------------------------------
bool CMeasure::inPolygon(MPoly2D *ipa, Vector3 pt)
{
	if (ipa->max_coord.z <= pt.z && ipa->min_coord.z >= pt.z && ipa->max_coord.x <= pt.x 
		&& ipa->min_coord.x >= pt.x && ipa->max_coord.y <= pt.y && ipa->min_coord.y >= pt.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}


//----------------------------------------------------------------------
// SetPoints
//
// Input measurements to the list 
//
// Input:
// ViewORTHO structure - gives all the information of the current state.
// measuremode (long) - Type of the current measurement.
// Vector structure poly - each element consists of x, y and z model coordinates
// face is active plane in the cube model  
// subtype of the measurement 
// new attribute value to set
//
// Output:
// returns the ID of the newly inserted object
//----------------------------------------------------------------------
long CMeasure::SetPoints(const viewORTHO& view, unsigned short measuremode, const Vector3Vec& poly, long face,  unsigned short subtype, unsigned short attrib)
{
	return SetPoints(view, NULL, measuremode, poly, face, subtype, attrib);
}


//----------------------------------------------------------------------
// SetPoints
//
// Input measurements to the list 
//
// ViewORTHO structure - gives all the information of the current state
// measuremode (long) - Type of the current measurement
// Vector structure poly - each element consists of x, y and z model coordinates
// face is active plane in the cube model  
// subtype of the measurement 
// new attribute value to set
//
// Output:
// returns the ID of the newly inserted object
//----------------------------------------------------------------------
long CMeasure::SetPoints(const viewORTHO& view, const mvState* state, unsigned short measuremode, const Vector3Vec& poly, long face, unsigned short subtype, unsigned short attrib)
{
	MPoint *pt;
	MLine *pl;
	MAngle *pan;
	Vector3 Pk, Pk1;
	MPoly2D *ipa;
	float D1, D2;

	long ret = 0;

	switch (measuremode) 
	{
	
		case MPOINT:
			assert(!poly.empty());
			pt = new MPoint;
			pt->type = MPOINT;
			ret = pt->id = m_pointID++;
			pt->subtype = subtype;
			pt->attrib = attrib;
			pt->param[0] = poly[0].x;
			pt->param[1] = poly[0].y;
			pt->param[2] = poly[0].z;
			
			if (state)
			{
				pt->state = *state;
			}
			
			m_measureList.push_back(pt);

			break;
		
		case MLINE:
			assert(poly.size() >= 2);
			pl = new MLine;
			pl->type = MLINE;
			ret = pl->id = m_lineID++;
			pl->subtype = subtype;
			pl->attrib = attrib;
			pl->start = poly[0];
			pl->end = poly[1];
			pl->param[0] = ~(pl->start - pl->end); // length of line
			
			if (state)
			{
				pl->state = *state;
			}
			
			m_measureList.push_back(pl);

			break;
		
		case MANGLE:
			assert(poly.size() >= 3); // reject if no. of points >= 3 : angle has only 3 pts
			pan = new MAngle;
			pan->type = MANGLE;
			ret = pan->id = m_angleID++;
			pan->subtype = subtype;
			pan->attrib = attrib;
			pan->start = poly[0];
			pan->middle = poly[1];
			pan->end = poly[2];	
			
			if (state)
			{
				pan->state = *state;
			}
			
			// calculation of the angle
			// 2nd point of the vector is the middle point
			Pk = pan->middle - pan->start;
			Pk1 = pan->middle - pan->end;
			D1 = ~Pk * ~Pk1;
			
			if (D1 == 0) 
			{
				pan->param[0] = 0;
			}
			else
			{
				D2 = (Pk % Pk1)/D1;
				if (fabs(D2) > 1.0) pan->param[0] = 0;
				else
					pan->param[0] = geom::RadToDeg(acos(D2));
			}
			
			m_measureList.push_back(pan);
			break;
		
		// area measurements
		case MAREA:
			if (poly.size() < 3) // reject if no. of points < 3 : not a polygon
			{
				break;
			}
			ipa = new MPoly2D;
			ipa->type = MAREA;
			ret = ipa->id = m_areaID++;
			ipa->subtype = subtype;
			ipa->attrib = attrib;
			ipa->poly2d = poly;
			ipa->poly2d.push_back(poly.front());
			
			// copy the cube state
			if (state)
			{
				ipa->state = *state;
			}
			
			// convert plane equation to model coords
			geom::TransformPlane(view, &ipa->planeNormal, &ipa->planeD, face); 
			
			// calculate the polygon parameters
			polyParam(ipa);
			
			// store in the list
			m_measureList.push_back(ipa); 
			break;
		
		// Volume measurements
		case MVOLUME:
			if (poly.empty() && m_curVol != -1) // end of a volume measurement
			{
				ret = m_measureList[m_curVol]->id;
				
				ClearAttrib(m_curVol, MATTR_INCOMPLETE);
				
				// sort the polygons
				if (subtype != MSUB_SEED && subtype != MSUB_NEEDLE && subtype != MSUB_MARKER)
				{
					STL::sort(((MPoly3D *)m_measureList[m_curVol])->poly3d.begin(), ((MPoly3D *)m_measureList[m_curVol])->poly3d.end(), compare4);
					((MPoly3D *)m_measureList[m_curVol])->param[0] = calcVolume(((MPoly3D *)m_measureList[m_curVol]));
				}// calculate the volume
				
				calculateEnclosedCube(((MPoly3D *)m_measureList[m_curVol]));
				m_curVol = -1;
			}
			else
			{
				if (poly.size() < 3) // if no of nodes < 3 : not a polygon
				{
					break;
				}
				
				if (m_curVol == -1) // starting a new volume
				{
					MPoly3D *ipv = new MPoly3D;
					ipv->type = MVOLUME;
					ret = ipv->id = m_volumeID++;
					ipv->subtype = subtype;
					ipv->attrib = attrib | MATTR_INCOMPLETE;
					m_curVol = m_measureList.size();
					m_measureList.push_back(ipv);
				}
				
				InsertPolygon(m_curVol, view, state, poly, face, subtype, attrib);
				
			}
			break;
	} 

	return ret;
}


//----------------------------------------------------------------------
// GetMeasureObjects
//
// Output the measurements (not the points) of each element with the types 
// and the ids   (data from the base structure of MObj).  
// The MeasureList which contains all these data is returned and the  
// information is displayed  in a separate window when the measurement mode 
// is called. 
//
// Input:
// none
//
// Output:
// return the measurement list
//----------------------------------------------------------------------
const MObjpVec& CMeasure::GetMeasureObjects()
{
	return m_measureList;
}


//----------------------------------------------------------------------
// GetPoints
//
// Output the (point) data for visualization purpose.  
// Current viewORTHO structure is given as an input so that the measurement 
// class gets the information of the current view. Point structure is devided 
// into  line segments (e.g. if it is a point, line segment with two points 
// which are the same ; if it is a line, only one line segment; if it is an 
// angle, two line segments;if it is a polygon with n vertices, n line 
// segments)  and  processed using routines (after converting to current view 
// coordinates) "DeterminPoints", "Determinpts_diff_pl", "Determinpts_same_pl" 
// and/or "DeterminVolpts" according to the type of the measurement and location 
// of the measurement.
// All the points that are needed to be plotted are stored in the structure 
// "Plot" and returned the final information.
// If a valid index is given, only the plot vectors for that measurement are given.
//
// Input:
// viewORTHO structure is given as an input so that the measurement 
// class gets the information of the current view
// index of mesurment 
//
// Output:
// none
//----------------------------------------------------------------------
const PlotVec& CMeasure::GetPoints(const viewORTHO& view, long index)
{
	bool valid_index = (index >=0 && index < m_measureList.size());

	Vector3 lmeas[4], temppt, offset, offset2, Plnormal;
	long k, m;
	m_outPoints.erase(m_outPoints.begin(), m_outPoints.end());
	m_cutPoints.erase(m_cutPoints.begin(), m_cutPoints.end());
	bool parallel_plane = 0; 
	MPoly2D curVol1, *opa;
	MPoly3D *opv;
	float ang_val, PlaneD;
	Vector3Vec cutPoly2;

	// draw the orientation markers
	if (!valid_index && m_orientMark.start.x != 0.0f)
	{
		Vector3 start, end;
		Plot pt1, pt2;

		pt1.id = pt2.id = 0xffff;
		pt2.plotSpecial = pt1.plotSpecial = MSUB_ORIENTLINE;

		start = m_orientMark.start * view.Model2View;
		pt1.x = (long) start.x; pt1.y = (long) start.y;
		pt1.plotCode = MOVE_TO;
		pt2.plotCode = LINE_TO;

		end = m_orientMark.endX * view.Model2View;
		pt2.x = (long) end.x; pt2.y = (long) end.y;
		m_outPoints.push_back(pt1);
		m_outPoints.push_back(pt2);

		end = m_orientMark.endY * view.Model2View;
		pt2.x = (long) end.x; pt2.y = (long) end.y;
		m_outPoints.push_back(pt1);
		m_outPoints.push_back(pt2);
		
		end = m_orientMark.endZ * view.Model2View;
		pt2.x = (long) end.x; pt2.y = (long) end.y;
		m_outPoints.push_back(pt1);
		m_outPoints.push_back(pt2);

		end = m_orientMark.label * view.Model2View;
		pt2.x = (long) end.x; pt2.y = (long) end.y;
		pt2.plotCode = MOVE_TO;
		pt2.plotSpecial = MSUB_ORIENTTEXT;
		m_outPoints.push_back(pt2);

		const float OFFSET = 0.07f; // offset from the end of axis to the label
		start = m_orientMark.endX *(1+OFFSET) - m_orientMark.start * OFFSET;
		end = start * view.Model2View;
		pt2.x = (long) end.x; pt2.y = (long) end.y;
		pt2.plotCode = MOVE_TO;
		pt2.plotSpecial = MSUB_ORIENT_X_LABEL;
		m_outPoints.push_back(pt2);

		start = m_orientMark.endY *(1+OFFSET) - m_orientMark.start * OFFSET;
		end = start * view.Model2View;
		pt2.x = (long) end.x; pt2.y = (long) end.y;
		pt2.plotCode = MOVE_TO;
		pt2.plotSpecial = MSUB_ORIENT_Y_LABEL;
		m_outPoints.push_back(pt2);

		start = m_orientMark.endZ *(1+OFFSET) - m_orientMark.start * OFFSET;
		end = start * view.Model2View;
		pt2.x = (long) end.x; pt2.y = (long) end.y;
		pt2.plotCode = MOVE_TO;
		pt2.plotSpecial = MSUB_ORIENT_Z_LABEL;
		m_outPoints.push_back(pt2);

	}

	// output the information to draw the measurements
	for (long i = 0; i < m_measureList.size(); i++)
	{
		// skip others if an index is given
		if (valid_index && i != index)
		{
			continue;
		}

		MLine *p;
		MAngle *q;

		MObj * obj = m_measureList[i];
		long type = obj->type;

		switch (obj->type) 
		{
			case MPOINT:
				lmeas[1].x = obj->param[0];
				lmeas[1].y = obj->param[1];
				lmeas[1].z = obj->param[2];
				
				// convert model coords to view coords
				lmeas[0] = lmeas[1] * view.Model2View;
				
				// Calculate the graphics information on all the visible faces of the cube. 
				for (k = 0; k < view.face.size(); k++)
				{
					if (view.face[k].vert.size() < 3 || view.face[k].plane.normal.z >= -minDist) continue;
					determinPoints(view, &lmeas[0], &lmeas[0], k, MPOINT, i);
				}
				break;
		
			case MLINE:
				p = reinterpret_cast<MLine*>(obj);
				
				// convert model coords to view coords
				lmeas[0] = p->start * view.Model2View;
				lmeas[1] = p->end * view.Model2View;
				
				// Calculate the graphics information on all the visible faces of the cube. 
				for (k = 0; k < view.face.size(); k++)
				{
					if (view.face[k].vert.size() < 3 || view.face[k].plane.normal.z >= -minDist) continue;
					determinPoints(view, &lmeas[0], &lmeas[1], k, MLINE, i);
				}
				break;
		
			case MANGLE:
				q = reinterpret_cast<MAngle*>(obj);
				
				// convert model coords to view coords
				lmeas[0] = q->start * view.Model2View;
				lmeas[1] = q->middle * view.Model2View;
				
				// Calculate the graphics information on all the visible faces of the cube. 
				for (k = 0; k < view.face.size(); k++)
				{
					if (view.face[k].vert.size() < 3 || view.face[k].plane.normal.z >= -minDist) continue;
					determinPoints(view, &lmeas[0], &lmeas[1], k, MANGLE, i);
				}
				
				lmeas[0] = q->end * view.Model2View;
				
				for (k = 0; k < view.face.size(); k++)
				{
					if (view.face[k].vert.size() < 3 || view.face[k].plane.normal.z >= -minDist) continue;
					determinPoints(view, &lmeas[1], &lmeas[0], k, MANGLE, i);
				}
				break;

			case MAREA:
				opa = reinterpret_cast<MPoly2D*>(obj);
				
				for (k = 0; k < view.face.size(); k++)
				{	
					parallel_plane = 0;
					if (view.face[k].vert.size() < 3 || view.face[k].plane.normal.z >= -minDist) continue;
					
					// transform the plane eqn. from view to model
					geom::TransformPlane(view, &Plnormal, &PlaneD, k);
					ang_val = Plnormal % opa->planeNormal;
					
					if (ang_val < 0)
					{
						ang_val *= -1;
						Plnormal *= -1.0;
						PlaneD *= -1;
					}
					
					// if polygon is parallel to the plane
					if (1.0 - ang_val < 0.001)
					{
						// if polygon is on the plane
						if (fabs(opa->planeD - PlaneD) < 0.05)
						{
							parallel_plane = 1;
						}
						else
						{
							continue;
						}
					}
					
					m_prevSide = -2;
					m_prevX = m_prevY = m_prevZ = -1.0;
					m_cutPoly.erase(m_cutPoly.begin(), m_cutPoly.end());
					
					if (parallel_plane)
					{
						
						for (long j = opa->poly2d.size() - 1; j >= 1; j--)
						{
							// convert model coords to view coords	
							lmeas[0] = opa->poly2d[j] * view.Model2View;
							lmeas[1] = opa->poly2d[j - 1] * view.Model2View;
							
							// determine the graphics details
							determinPts_same_pl(view, &lmeas[0], &lmeas[1], k, i);
						}
					}
					
					else
					{
						// if polygon is not parallel to the plane 
						for (long j = opa->poly2d.size() - 1; j >= 1; j--)
						{
							// convert model coords to view coords			
							lmeas[0] = opa->poly2d[j] * view.Model2View;
							lmeas[1] = opa->poly2d[j - 1] * view.Model2View;
							
							
							if (j == opa->poly2d.size() - 1)
							{
								m_prevX = lmeas[0].x;
								m_prevY = lmeas[0].y;
								m_prevZ = lmeas[0].z;
							}
							
							// determine the plane and polygon cutting points
							determinPts_diff_pl(view, &lmeas[0], &lmeas[1], k);
						}
						
						if (m_cutPoly.size() > 1)
						{
							STL::sort(m_cutPoly.begin(), m_cutPoly.end(), compare1);
							
							for (long n = 0; n < m_cutPoly.size() - 1; n+=2)
							{
								// determine the graphics details
								determinPoints(view, &m_cutPoly[n], &m_cutPoly[n + 1], k, MAREA, i);
							}
						}
					}
				}
				break;

			// volume measurements
			case MVOLUME:

				opv = reinterpret_cast<MPoly3D*>(obj);
				
				if (opv->poly3d.empty())
				{
					break;
				}
				
				for (k = 0; k < view.face.size(); k++)
				{
					
					long size;
					size = opv->poly3d.size();
					
					for (m = 0; m < size; m++)
					{
						
						opa = &opv->poly3d[m];
						
						parallel_plane = 0;
						if (view.face[k].vert.size() < 3 || view.face[k].plane.normal.z >= -minDist) continue;
						
						// transform the plane eqn. from view to model
						geom::TransformPlane(view, &Plnormal, &PlaneD, k);
						ang_val = Plnormal % opa->planeNormal;
						
						if (ang_val < 0)
						{
							ang_val *= -1;
							Plnormal *= -1.0;
							PlaneD *= -1;
						}
						
						// if polygon is parallel to the plane
						if (1.0 - ang_val < 0.0001)
						{
							// if polygon is on the plane
							if (fabs(opa->planeD - PlaneD) < 0.005)
							{
								parallel_plane = 1;
							}
							else
							{
								continue;
							}
						}
						
						m_prevSide = -2;
						m_prevX = m_prevY = m_prevZ = -1.0;
						m_cutPoly.erase(m_cutPoly.begin(), m_cutPoly.end());
						
						if (parallel_plane)
						{
							
							for (long j = opa->poly2d.size() - 1; j >= 1; j--)
							{
								// convert model coords to view coords	
								lmeas[0] = opa->poly2d[j] * view.Model2View;
								lmeas[1] = opa->poly2d[j - 1] * view.Model2View;
								
								// determine the graphics details
								determinPts_same_pl(view, &lmeas[0], &lmeas[1], k, i);
							}
						}
						
						else
						{
							// if polygon is not parallel to the plane 
							for (long j = opa->poly2d.size() - 1; j >= 1; j--)
							{
								// convert model coords to view coords			
								lmeas[0] = opa->poly2d[j] * view.Model2View;
								lmeas[1] = opa->poly2d[j - 1] * view.Model2View;
								
								
								if (j == opa->poly2d.size() - 1)
								{
									m_prevX = lmeas[0].x;
									m_prevY = lmeas[0].y;
									m_prevZ = lmeas[0].z;
								}
								
								// determine the plane and polygon cutting points
								determinPts_diff_pl(view, &lmeas[0], &lmeas[1], k);
							}
							
							if (m_cutPoly.size() > 1)
							{
								STL::sort(m_cutPoly.begin(), m_cutPoly.end(), compare1);
								
								for (long n = 0; n < m_cutPoly.size() - 1; n+=2)
								{
									// determine the graphics details
									determinPoints(view, &m_cutPoly[n], &m_cutPoly[n + 1], k, MAREA, i);
								}
							}
						}
						
					}
					
				}	
				break;
		}
	}

	return m_outPoints;
}


//----------------------------------------------------------------------
// GetContourPoints
//
// The same as in GetPoints() only for 3D volume measurements
//
// Input:
// viewORTHO structure is given as an input so that the measurement 
// class gets the information of the current view
//
// Output:
// PlotVec contains intersection points of all contours with the given
// current view
//----------------------------------------------------------------------
const PlotVec& CMeasure::GetContourPoints(const viewORTHO& view)
{

	Vector3 lmeas[4], temppt, offset, offset2, Plnormal;
	long k, m;
	m_outPoints.erase(m_outPoints.begin(), m_outPoints.end() );
	m_cutPoints.erase(m_cutPoints.begin(), m_cutPoints.end() );
	bool parallel_plane = 0, emptypts; 
	MPoly2D curVol1;
	MPoly3D *opv;
	float D2, ang_val, PlaneD;
	Vector3Vec cutPoly2;


	// output the information to draw the measurements
	for (long i = 0; i < m_measureList.size(); i++)
	{

		MObj * obj = m_measureList[i];
		long type = obj->type;

		if (type != MVOLUME)
		{
			continue;
		}

		opv = reinterpret_cast<MPoly3D*>(obj);

		if (opv->poly3d.empty())
		{
			break;
		}
		
		
		for (k = 0; k < view.face.size(); k++)
		{
			emptypts = 1;
			if (view.face[k].vert.size() < 3 || view.face[k].plane.normal.z >= -minDist) continue;
			m_cutPoints.erase(m_cutPoints.begin(), m_cutPoints.end() );
			m_prevSide = -2;
			m_prevX = m_prevY = m_prevZ = -1;
			m_cutPoly.erase(m_cutPoly.begin(), m_cutPoly.end());
			// transform the plane eqn. from view to model
			geom::TransformPlane(view, &Plnormal, &PlaneD, k);
			parallel_plane = 0;
			ang_val = Plnormal % opv->poly3d[0].planeNormal;

			if (ang_val < 0)
			{
				ang_val *= -1;
				Plnormal *= -1.0;
				PlaneD *= -1;
			}

			// if volume is parallel to the plane
			if (fabs(1.0 - ang_val) < 0.001)
			{
				parallel_plane = 1;
				// if the volume does not intersect with the plane 
				if ((PlaneD + 0.02) < opv->poly3d[0].planeD ||  (PlaneD - 0.02) > opv->poly3d[opv->poly3d.size() - 1].planeD)
				{
					continue;
				}
			}

		
			D2 = 100000;  

			for (m = 0; m < opv->poly3d.size(); m++)
			{
				m_curVol3 = opv->poly3d[m];
				m_cutPoints.erase(m_cutPoints.begin(), m_cutPoints.end() );
				m_prevSide = -2;
				m_prevX = m_prevY = m_prevZ = -1;
				m_cutPoly.erase(m_cutPoly.begin(), m_cutPoly.end());

				if (fabs(PlaneD - m_curVol3.planeD) >= 0.05 && parallel_plane)
				{
					continue;
				}

				if (parallel_plane)
				{

					for (long j = m_curVol3.poly2d.size() - 1; j >= 1; j--)
					{
						// convert model coords to view coords	
						lmeas[0] = m_curVol3.poly2d[j] * view.Model2View;
						lmeas[1] = m_curVol3.poly2d[j - 1] * view.Model2View;
					
						// determine the graphics details
						determinPts_same_pl(view, &lmeas[0], &lmeas[1], k, i);
					}
				}
				
				else
				{
					// if polygon is not parallel to the plane 
					for (long j = m_curVol3.poly2d.size() - 1; j >= 1; j--)
					{
						// convert model coords to view coords			
						lmeas[0] = m_curVol3.poly2d[j] * view.Model2View;
						lmeas[1] = m_curVol3.poly2d[j - 1] * view.Model2View;
					
				
						if (j == m_curVol3.poly2d.size() - 1)
						{
							m_prevX = lmeas[0].x;
							m_prevY = lmeas[0].y;
							m_prevZ = lmeas[0].z;
						}

						// determine the plane and polygon cutting points
						determinPts_diff_pl(view, &lmeas[0], &lmeas[1], k);
					}
					
					if (m_cutPoly.size() > 1)
					{
						STL::sort(m_cutPoly.begin(), m_cutPoly.end(), compare1);
		
						for (long n = 0; n < m_cutPoly.size() - 1; n+=2)
						{
							// determine the graphics details
							determinPoints(view, &m_cutPoly[n], &m_cutPoly[n + 1], k, MAREA, i);
						}
					}
				}
			}
		}
	}
	return m_outPoints;
}


//----------------------------------------------------------------------
// determinVolpts
//
// When four points of a quadralateral is given, this routine checks whether a given 
// plane cuts this polygon (in case where all four points 
// are not on one side of a plane).  If there is any (it should be either one, 
// two or none) those points are fed to DeterminPoints for final drawing
//
// viewORTHO structure to get the information on current view.
// screen coordinates of the two points that form the line.
// plane number in question.
// measurement type.
// index on the measureList (this is only used to store in the Plot structure).
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::determinVolpts(const viewORTHO& view, Vector3 offset, Vector3 offset2, long face, long listno)
{
	Vector3 lmeas[6], inter_pt[2];

	long left1, right1, left2, right2, count;

	for (long j = m_curVol3.poly2d.size() - 1; j >= 1; j--)
	{
		count = 0;

		if (!m_tubeVol)
		{
			lmeas[2] = offset + m_curVol3.poly2d[j];
			lmeas[3] = offset2 + m_curVol3.poly2d[j];
		}

		else
		{
			lmeas[2] = m_curVol3.poly2d[j];
			lmeas[3] = m_curVol4.poly2d[j];
		}
		
		// convert pts from model to view
		lmeas[0] = lmeas[2] * view.Model2View;
		lmeas[1] = lmeas[3] * view.Model2View;
	
		// determine which side of the plane the points are
		left1 = detSide(&lmeas[0], &(view.face[face].plane));
		right1 = detSide(&lmeas[1], &(view.face[face].plane));
		
		// if both pts are on the plane 
		if (left1 == right1 && left1 == 0)
		{
			determinPoints(view, &lmeas[0], &lmeas[1], face, MVOLUME, listno);
			continue;
		}

		/* if points are on different sides of the plane we can have only one 
			intersection pt
		*/
		if (left1 != right1)
		{
			 
			if (left1 == 0)
			{
				inter_pt[count] = lmeas[0];
				count++;
			}

			else
			{
				if (right1 == 0)
				{
					inter_pt[count] = lmeas[1];
					count++;
				}
				else
				{
					geom::intersect(&lmeas[0], &lmeas[1], &(view.face[face].plane), &inter_pt[count]);
						count++;
				}
			}	
		}

		// consider the next set of points and do the same
		if (!m_tubeVol)
		{
			lmeas[2] = offset + m_curVol3.poly2d[j - 1];
			lmeas[3] = offset2 + m_curVol3.poly2d[j - 1];
		}

		else
		{
			lmeas[2] = m_curVol3.poly2d[j - 1];
			lmeas[3] = m_curVol4.poly2d[j - 1];
		}

		lmeas[4] = lmeas[2] * view.Model2View;
		lmeas[5] = lmeas[3] * view.Model2View;

		left2 = detSide(&lmeas[4], &(view.face[face].plane));
		right2 = detSide(&lmeas[5], &(view.face[face].plane));
		
		
		if (left1 == left2 && right1 == right2 && left2 == right2 && left2 != 0)
		{
			continue;
		}

		if (left2 == right2 && left2 == 0)
		{
			determinPoints(view, &lmeas[4], &lmeas[5], face, MVOLUME, listno);
			continue;
		}


		if (left2 != right2)
		{
			if (left2 == 0)
			{
				inter_pt[count] = lmeas[4];
				count++;
		
				// as soon as you get two points try to draw a line 
				if (count == 2)
				{
					determinPoints(view, &inter_pt[0], &inter_pt[1], face, MVOLUME, listno);
					continue;
				}
			}

			else
			{
				if (right2 == 0)
				{
					inter_pt[count] = lmeas[5];
					count++;
				}

				else
				{
					geom::intersect(&lmeas[4], &lmeas[5], &(view.face[face].plane), &inter_pt[count]);
					count++;
				}
			
				if (count == 2)
				{
					determinPoints(view, &inter_pt[0], &inter_pt[1], face, MVOLUME, listno);
					continue;
				}
			}
		}
		
		if (left2 == left1 && left2 == 0)
		{
			determinPoints(view, &lmeas[4], &lmeas[0], face, MVOLUME, listno);
			continue;
		}

		if (left2 != left1)
		{
			if (left2 == 0)
			{
				inter_pt[count] = lmeas[4];
				count++;

				if (count == 2)
				{
					determinPoints(view, &inter_pt[0], &inter_pt[1], face, MVOLUME, listno);
					continue;
				}
			}
		
			else
			{
 				if (left1 == 0)
				{
					inter_pt[count] = lmeas[0];
					count++;
				}

				else
				{
					geom::intersect(&lmeas[4], &lmeas[0], &(view.face[face].plane), &inter_pt[count]);
					count++;
				}
			
				if (count == 2)
				{
					determinPoints(view, &inter_pt[0], &inter_pt[1], face, MVOLUME, listno);
					continue;
				}
			}
		}
		
		if (right2 == right1 && right2 == 0)
		{
			determinPoints(view, &lmeas[5], &lmeas[1], face, MVOLUME, listno);
			continue;
		}

		if (right2 != right1)
		{
			if (right2 == 0)
			{
				inter_pt[count] = lmeas[5];
				count++;

				if (count == 2)
				{
					determinPoints(view, &inter_pt[0], &inter_pt[1], face, MVOLUME, listno);
					continue;
				}
			}
			else
			{
				if (right1 == 0)
				{
					inter_pt[count] = lmeas[1];
					count++;
				}

				else
				{
					geom::intersect(&lmeas[5], &lmeas[1], &(view.face[face].plane), &inter_pt[count]);
					count++;
				}
			
				if (count == 2)
				{
					determinPoints(view, &inter_pt[0], &inter_pt[1], face, MVOLUME, listno);
					continue;
				}
			}
		}
	}
}


//----------------------------------------------------------------------
// determinPts_same_pl
//
// When the polygon plane and the face in question is the same plane, 
// this routine is used instead of   DeterminPoints routine. The main reason for that is,  
// if we consider line segments (which makes the polygon) discretely, they appear and 
// disappear randomly due to round off error. (Since we deal with floating points here, 
// when checking to see whether the line segment is on the plane by using the plane equation
// there is a round off error which can be different for each line.) So that the 
// routine "PtInPoly" which checks only the out of boundary situation is used instead of 
// "PtInFace" which checks for both out of boundary and on the plane conditions.
// The output is written to the structure Out as in DeterminPoints.
//
// Input:
// viewORTHO structure to get the information on current view.
// screen coordinates of the two points that form the line.
// plane number in question.
// measurement type.
// index on the measureList (this is only used to store in the Plot structure).
//
// Output:
// output is written in the structure Plot
//----------------------------------------------------------------------
void CMeasure::determinPts_same_pl(const viewORTHO& view, Vector3 *point1, Vector3 *point2, long face, long listno)
{
	Vector3 mpoint[2], temppt;


	if (long (point1->x) == long (point2->x) && long (point1->y) == long (point2->y))
	{
		if (ptInPoly(view.face[face].vert, point1->x, point1->y)) 
		{	
			m_out.id = listno;
			m_out.x = long (point1->x);
			m_out.y = long (point1->y);
			m_out.plotCode = MOVE_TO;
			m_outPoints.push_back(m_out);
		}

		return;
	}
	
	// check whether the point is inside the polygon
	bool cond3 = ptInPoly(view.face[face].vert, point1->x, point1->y);
	bool cond4 = ptInPoly(view.face[face].vert, point2->x, point2->y);
	
	// save data in "Plot" structure			
	if (cond3 && cond4)
	{
		m_out.id = listno;
		m_out.x = long (point1->x);
		m_out.y = long (point1->y);
		m_out.plotCode = MOVE_TO;
		m_outPoints.push_back(m_out); 				
	
		m_out.id = listno;
		m_out.x = long (point2->x);
		m_out.y = long (point2->y);
		m_out.plotCode = LINE_TO;
		m_outPoints.push_back(m_out); 
	}
	else
	{
		long k, m;
		for (k = 0, m = 0; k < view.face[face].vert.size() - 1; k++)
		{
			if (m == 1 && long (view.face[face].vert[k].x) == long(mpoint[0].x) 
					&& long (view.face[face].vert[k].y) == long(mpoint[0].y))
			{
				continue;
			}
							
			if (m == 2)
			{
				continue;
			}
							
			else
			{
				// calculate the line - plane intersection points
				if (lineIntersect(&(view.face[face].vert[k]), &(view.face[face].vert[k+1]), point1, point2, &mpoint[m]))
				{
					if (m != 0)
					{
						if (mpoint[m].x != mpoint[m-1].x  || mpoint[m].y != mpoint[m-1].y || mpoint[m].z != mpoint[m-1].z ) 
						{
							m++;
						}
					}
					else
					{
						m++;
					}
				}
			}
		}
						
		if (m < 2)
		{
			if (lineIntersect(&(view.face[face].vert[view.face[face].vert.size()-1]), &(view.face[face].vert[0]), point1, point2, &mpoint[m]))
			{
				m++;
			}
		}
						
		if (!cond3 && cond4)
		{
			// save data
			if (m >= 1)
			{
				m_out.id = listno;
				m_out.x = long (point2->x);
				m_out.y = long (point2->y);
				m_out.plotCode = MOVE_TO; 
				m_outPoints.push_back(m_out); 
				
				m_out.id = listno;
			 	m_out.x = long (mpoint[0].x);
				m_out.y = long (mpoint[0].y);
				m_out.plotCode = LINE_TO; 
				m_outPoints.push_back(m_out);				
			}
		}			
	
		if (cond3 && !cond4)
		{
			if (m >= 1)
			{
				// save data
				m_out.id = listno;
				m_out.x = long (point1->x);
				m_out.y = long (point1->y);
				m_out.plotCode = MOVE_TO; 
				m_outPoints.push_back(m_out);
				
				m_out.id = listno;
				m_out.x = long (mpoint[0].x);
				m_out.y = long (mpoint[0].y);
				m_out.plotCode = LINE_TO; 
				m_outPoints.push_back(m_out);
			}
		}
						
		if (!cond3 && !cond4)
		{
			if (m == 2)
			{
				m_out.id = listno;
				m_out.x = long (mpoint[0].x);
				m_out.y = long (mpoint[0].y);
				m_out.plotCode = MOVE_TO; 
				m_outPoints.push_back(m_out);
				
				m_out.id = listno;
				m_out.x = long (mpoint[1].x);
				m_out.y = long (mpoint[1].y);
				m_out.plotCode = LINE_TO; 
				m_outPoints.push_back(m_out);
			}
		}
	}
}


//----------------------------------------------------------------------
// determinPoints
//
// When two points for a line segment is given with a face, this function determines
// whether the line segment is on or not on that face, whether the line segment cuts 
// the face creating only a point on that plane or whether the line segment is partially 
// on that face.
// This is achieved by using inhouse mathematical  functions "ptInFace", "ptInPoly", 
// "ptInFaceGiven" and  "lineintersect" and some help from geometry class. 
//
// Input:
// viewORTHO structure to get the information on current view
// screen coordinates of the two points that form the line
// plane number in question
// measurement type
// index in the measureList (this is only used to store in the Plot structure).
//
// Output:
// output is written in the structure Plot
//----------------------------------------------------------------------
void CMeasure::determinPoints(const viewORTHO& view, Vector3 *point1, Vector3 *point2, long face, long type, long listno)
{
	Vector3 mpoint[2], temppt;
	long cond1, cond2;
	bool cond3, cond4;


	if (long (point1->x) == long (point2->x) && long (point1->y) == long (point2->y))
	{
		if (type != MAREA && type != MVOLUME)
		{
			if (ptInFaceGiven(view, point1->x, point1->y, point1->z, face)) 
			{
				m_out.id = listno;
				m_out.x = long (point1->x);
				m_out.y = long (point1->y);
				m_out.plotCode = MOVE_TO;
				m_outPoints.push_back(m_out); 
			}
		}

		else
		{
			if (ptInPoly(view.face[face].vert, point1->x, point1->y)) 
			{	
				m_out.id = listno;
				m_out.x = long (point1->x);
				m_out.y = long (point1->y);
				m_out.plotCode = MOVE_TO;
				m_outPoints.push_back(m_out); 
			}
		}
	}
		
	
	if (type != MAREA && type != MVOLUME)
	{
		cond1 = geom::detside(point1, &(view.face[face].plane));
		cond2 = geom::detside(point2, &(view.face[face].plane));
	}
	else
	{
		cond1 = cond2 = 0;
	}

	if (cond1 == cond2 && cond1 != 0)
	{
		return;
	}

	if (cond1 != cond2 )
	{
		if (cond1 == 0)
		{
			if (ptInFaceGiven(view, point1->x, point1->y, point1->z, face))
			{
				m_out.id = listno;
				m_out.x = long (point1->x);
				m_out.y = long (point1->y);
				m_out.plotCode = MOVE_TO;
				m_outPoints.push_back(m_out);
			}
			return;
		}
		
		if (cond2 == 0)
		{
			if (ptInFaceGiven(view, point2->x, point2->y, point2->z, face))
			{
				m_out.id = listno;
				m_out.x = long (point2->x);
				m_out.y = long (point2->y);
				m_out.plotCode = MOVE_TO;
				m_outPoints.push_back(m_out);
			}
			return;
		}

		geom::intersect(point1, point2, &(view.face[face].plane), &mpoint[0]);

		if (ptInFaceGiven(view, mpoint[0].x, mpoint[0].y, mpoint[0].z, face))
		{
			m_out.id = listno;
			m_out.x = long (mpoint[0].x);
			m_out.y = long (mpoint[0].y);
			m_out.plotCode = MOVE_TO;
			m_outPoints.push_back(m_out);
		}

		return;
	}
	
	else
	{
		if (type != MAREA && type != MVOLUME)
		{
			cond3 = ptInFaceGiven(view, point1->x, point1->y, point1->z, face);
			cond4 = ptInFaceGiven(view, point2->x, point2->y, point2->z, face);
		}
		else
		{
			cond3 = ptInPoly(view.face[face].vert, point1->x, point1->y );
			cond4 = ptInPoly(view.face[face].vert, point2->x, point2->y );
		}

		if (cond3 && cond4)
		{
			m_out.id = listno;
			m_out.x = long (point1->x);
			m_out.y = long (point1->y);
			m_out.plotCode = MOVE_TO;
			m_outPoints.push_back(m_out); 				
		
			m_out.id = listno;
			m_out.x = long (point2->x);
			m_out.y = long (point2->y);
			m_out.plotCode = LINE_TO;
			m_outPoints.push_back(m_out); 
		}
		else
		{
			long k, m;
			for (k = 0, m = 0; k < view.face[face].vert.size()-1; k++)
			{
				if (m == 1 && long (view.face[face].vert[k].x) == long(mpoint[0].x) 
					       && long (view.face[face].vert[k].y) == long(mpoint[0].y))
				{
					continue;
				}
							
				if (m == 2) 
				{
					continue;
				}					
				else
				{
					if (lineIntersect(&(view.face[face].vert[k]), &(view.face[face].vert[k+1]), point1, point2, &mpoint[m]))
					{
						m++;
					}
				}
			}
						
			if (m < 2)
			{
				if (lineIntersect(&(view.face[face].vert[view.face[face].vert.size()-1]), &(view.face[face].vert[0]), point1, point2, &mpoint[m]))
					m++;
			}
							
			if (!cond3 && cond4)
			{
				if (m >= 1)
				{
					m_out.id = listno;
					m_out.x = long (point2->x);
					m_out.y = long (point2->y);
					m_out.plotCode = MOVE_TO; 
					m_outPoints.push_back(m_out); 
					
					m_out.id = listno;
			 		m_out.x = long (mpoint[0].x);
					m_out.y = long (mpoint[0].y);
					m_out.plotCode = LINE_TO; 
					m_outPoints.push_back(m_out);		
				}
			}		
			if (cond3 && !cond4)
			{
				if (m >= 1)
				{
					m_out.id = listno;
					m_out.x = long (point1->x);
					m_out.y = long (point1->y);
					m_out.plotCode = MOVE_TO; 
					m_outPoints.push_back(m_out);
					
					m_out.id = listno;
					m_out.x = long (mpoint[0].x);
					m_out.y = long (mpoint[0].y);
					m_out.plotCode = LINE_TO; 
					m_outPoints.push_back(m_out);
				}
			}
						
			if (!cond3 && !cond4)
			{
				if (m == 2)
				{
					m_out.id = listno;
					m_out.x = long (mpoint[0].x);
					m_out.y = long (mpoint[0].y);
					m_out.plotCode = MOVE_TO; 
					m_outPoints.push_back(m_out);
					
					m_out.id = listno;
					m_out.x = long (mpoint[1].x);
					m_out.y = long (mpoint[1].y);
					m_out.plotCode = LINE_TO; 
					m_outPoints.push_back(m_out);
				}
			}
		}
	}
}


//----------------------------------------------------------------------
// determinPts_diff_pl
//
// In volume or area mode, when the polygon is not parallel to the face 
// in question the intersection points of the polygon and the plane on which 
// the face lies are calculated and stored in the structure "Intersect" 
// Once all the intersecting points of a polygon are calculated, they are 
// sorted in x,y or z order (to eliminate round off errors maximum of x,y or z 
// difference is selected for sorting).
// When calculating points of intersection, when there is an end point (of the 
// line segment) which is on the plane, it is stored twice. 
// After the sorting is done, all the even numbered points are to be joined
// to the next (odd numbered) point and these line segments are fed to the 
// DeterminPoints routine to check whether they lie on the face. (face  has 
// its boundaries which is defined by its vertices and  is a subset of the plane). 
// Each joined line segment represents the inside of the polygon.
// For a polyhedron:
// Polyhedron consists of  a series of polygons. After drawing is completed, 
// the polygons are sorted in order.  Each polygon drawn  is extended halfway to 
// the front and back (if it is the first polygon there is no extension to the back
// and if it is the last one,  no extension to the front) to form a cylinder and 
// the polygon series converts into one polyhedron.
// Once all the intersecting points for each polygon is calculated (each drawn 
// polygon forms two polygons. The front polygon overlaps with the back polygon 
// of the front drawn polygon and the back polygon overlaps with the front polygon 
// of the back drawn polygon ) .
// The points of intersections vectors are merged together at overlapping polygon 
// positions (first and last entry does not have any merging. After the sorting is done, 
// all the even numbered points are to be joined to the next (odd numbered) point and 
// these line segments are fed to the DeterminPoints routine to check whether they lie 
// on the face. 
// (face  has its boundaries which is defined by its vertices and  is a subset of the plane). 
// Each joined line segment represents the inside of the polygon.
// If the cutting plane is perpendicular to the polygon plane, the two relevant point 
// vectors (of two end polygons created from the drawn polygon) are joined together one 
// to one basis. (1st element of vector1 to 1st element of vector2 etc).
// Joined means the two points are fed to the routine DeterminPoints.
//
// Input:
// viewORTHO structure to get the information on current view
// screen coordinates of the two points that form the line
// plane number in question
//
// Output:
// output is written to m_cutPoly
//----------------------------------------------------------------------
void CMeasure::determinPts_diff_pl(const viewORTHO& view, Vector3 *point1, Vector3 *point2, long face)
{
	Vector3 mpoint[2], temppt;

	long cond1 = detSide(point1, &(view.face[face].plane));
	long cond2 = detSide(point2, &(view.face[face].plane));

	if (cond1 == cond2)
	{
		if (cond1 != 0)
		{
			return;
		}
		else
		{
	
			m_cutPoly.push_back(*point1);
			m_cutPoly.push_back(*point2);
		}
	}
	else
	{
		
		if (cond1 == 0)
		{
			if (cond2 == m_prevSide || m_prevSide == -2)
			{
				temppt = *point1;
				m_cutPoly.push_back(temppt);
			}
			
			if (m_prevSide == -2)
			{
				m_firstSide = cond2;
			}
			
			m_prevSide = cond2;

			return;
		}
		
		if (cond2 == 0)
		{
		
			if (m_prevSide == -2 || long (point2->x) != m_prevX || long (point2->y) != m_prevY || long(point2->z) != m_prevZ)
			{
				temppt = *point2;
				m_cutPoly.push_back(temppt);
			}
			else
			{
				if (cond1 == m_firstSide)
				{
					temppt = *point2;
					m_cutPoly.push_back(temppt);
				}
			}
		
			if (m_prevSide == -2)
			{
				m_firstSide = cond1;
			}
			
			m_prevSide = cond1;

			return;
		}

		geom::intersect(point1, point2, &(view.face[face].plane), &mpoint[0]);

		temppt = mpoint[0];
		m_cutPoly.push_back(temppt);
		
		if (m_prevSide == -2)
		{
			m_firstSide = cond2;
		}

		m_prevSide = cond2;

		return;
	}
}


//----------------------------------------------------------------------
// lineIntersect
//
// When the end points of two line segments are given, this routine calculates whether 
// the two lines  intersect and if so, calculates the point of intersection , 
// returns true and if not so, returns false. 
//
// Input:
// vec1, vec2, vec3, vec4 are two line segments end points
// res contains intersection point
//
// Output:
// true if intersects, otherwise returns false
//----------------------------------------------------------------------
bool CMeasure::lineIntersect(const Vector3 *vec1, const Vector3 *vec2, Vector3 *vec3, Vector3 *vec4, Vector3 *res)
{
	
	float minx = getMax(getMin(vec1->x, vec2->x),getMin(vec3->x, vec4->x));
	float maxx = getMin(getMax(vec1->x, vec2->x), getMax(vec3->x, vec4->x));

	if (minx  > maxx)
	{
		return false;
	}

	float miny = getMax(getMin(vec1->y, vec2->y),getMin(vec3->y, vec4->y));
	float maxy = getMin(getMax(vec1->y, vec2->y), getMax(vec3->y, vec4->y));

	if (miny > maxy)
	{
		return false;
	}
	
	if (vec1->x == vec2->x)
	{
		if (vec3->x == vec4->x)
		{
			return false;
		}
		else
		{
			if (vec1->x < minx || vec1->x > maxx)
			{
				return false;
			}

			res->x = vec1->x;
			res->y = (vec4->y - vec3->y) * (res->x - vec3->x)/(vec4->x - vec3->x) + vec3->y;
		
			if (res->y < miny || res->y > maxy)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
	
	else
	{
		if (vec3->x == vec4->x)	
		{
			if (vec3->x < minx || vec3->x > maxx)
			{
				return false;
			}

			res->x = vec3->x;
			res->y = (vec1->y - vec2->y) * (res->x - vec2->x)/(vec1->x - vec2->x) + vec2->y;
		
			if (res->y < miny || res->y > maxy)
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		else
		{
			float m1 = (vec1->y - vec2->y)/(vec1->x - vec2->x);
			float m2 = (vec3->y - vec4->y)/(vec3->x - vec4->x);
			
			if (m1 == m2)
			{
				return false;
			}
			
			float c1 = vec1->y - m1 * vec1->x;
			float c2 = vec3->y - m2 * vec3->x;

			res->x = (c2 - c1)/ (m1 - m2);
			
			if (res->x < minx || res->x > maxx)
			{
				return false;
			}
			
			res->y = m1 * res->x + c1;

			if (res->y < miny || res->y > maxy)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
}


//----------------------------------------------------------------------
// ptInFace
//
// When the ORTHOview structure and a point is given, this routine checks whether the 
// point is on a face and if so it returns the face number and if not so it returns -1. 
//
// Input:
// viewORTHO structure to get the information on current view
// x, y, z coordinates of the point
//
// Output:
// face id
//----------------------------------------------------------------------	
long CMeasure::ptInFace(const viewORTHO& view, float x, float y, float z, long measuremode)
{
	for (long i = view.face.size() - 1; i > -1; --i)
	{
		if (view.face[i].vert.size() < 3 || view.face[i].plane.normal.z > 0)
		{
			continue;
		}

		if ( ptInPoly(view.face[i].vert, x, y))
		{
			if (fabs(view.face[i].plane.normal.x * x + view.face[i].plane.normal.y * y + view.face[i].plane.normal.z  * z  - view.face[i].plane.D) < 1.0)
			{
				return i;
			}
		}
	}

	return -1;
}


//----------------------------------------------------------------------
// ptInPoly
//
// When a point and a face number is given, this routine checks whether the 
// point is inside the face or not and returns true or false.
//
// Input:
// Vector3Vec vertex contains the model face in the view coordinates
// x coordinate of the point in the view coordinates
// y coordinate of the point in the view coordinates
//
// Output:
// none
//----------------------------------------------------------------------
bool CMeasure::ptInPoly(const Vector3Vec& vertex, float ptX,  float ptY)
{

	if (((vertex.back().x  - ptX) * (vertex.front().y - ptY) - (vertex.back().y - ptY) * (vertex.front().x - ptX ))  < -1.0)     
	{
		return false;
	}
	
	for (Vector3Vec::const_iterator iVec = vertex.begin(); iVec != vertex.end() - 1; iVec++)
	{
		if (((iVec->x - ptX) * ( (iVec+1)->y - ptY) - (iVec->y - ptY) * ((iVec+1)->x - ptX ))  < -1.0)
		{
			return false;
		}	
	}
	
    return true;
}


//----------------------------------------------------------------------
// ptInFaceGiven
//
// When ORTHOview structure, a point coordinates and a face number is given, this routine checks whether the point 
// is on the given face or not and returns true or false.
//
// Input:
// viewORTHO structure to get the information on current view
// x, y, z coordinates of the point
// face id
//
// Output:
// true if yes, false otherwise
//----------------------------------------------------------------------
bool CMeasure::ptInFaceGiven(const viewORTHO& view, float x, float y, float z, long face)
{
	if (view.face[face].vert.size() < 3 || view.face[face].plane.normal.z > 0)
	{
		return false;
	}

	if (ptInPoly(view.face[face].vert, x, y )) 
	{
		if (fabs(view.face[face].plane.normal.x * x + view.face[face].plane.normal.y * y + view.face[face].plane.normal.z  * z  - view.face[face].plane.D) < 1.0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}


//----------------------------------------------------------------------
// compare1
//
// Vector sorting comparison function
//
//----------------------------------------------------------------------

bool compare1(const Vector3 &p, const Vector3 &q)
{
	float max_grad;

	float A = fabs(p.x - q.x);
	float B = fabs(p.y - q.y);
	float C = fabs(p.z - q.z);

	if (A > B)
	{
		max_grad = A;
	}
	else 
	{
		max_grad = B;
	}

	if (C > max_grad)
	{
		max_grad = C;
	}

	if (max_grad == A)
	{
		return(p.x < q.x);
	}
	
	if (max_grad == B)
	{
		return(p.y < q.y);
	}
	else
	{
		return(p.z < q.z);
	}
}


//----------------------------------------------------------------------
// compare4
//
// Vector sorting comparison function
//
//----------------------------------------------------------------------
bool compare4(const MPoly2D &p, const MPoly2D &q)
{
	return (p.planeD < q.planeD);
}


//----------------------------------------------------------------------
// calcVolume
//
// In case of a polyhedron, when the area of each polygon is given, the volume caused by 
// each polygon is calculated by considering the distances from front and back polygons.
// If the distance from the previos polygon is D1 and distance to the next polygon is D2, 
// the volume is (D1 + D2)/2.
//
// Input:
// MPoly3D structure contains 3D volume measurement
//
// Output:
// volume in cubic mm
//----------------------------------------------------------------------
float CMeasure::calcVolume(MPoly3D * cVol)
{
	float D1, D2, D3, vol = 0;
	D2 = 100000;
	Vector3 Pk;

	if (cVol->type != MVOLUME)
	{
		return 0.0f;
	}

	if (cVol->subtype != MSUB_SEED && cVol->subtype != MSUB_NEEDLE)
	{
		for (long i = 0; i < cVol->poly3d.size(); i++)
		{
			
			if (D2 != cVol->poly3d[i].planeD)
			{
				D1 = D2 = D3 = cVol->poly3d[i].planeD;
						
				long d = 0;
				
				while (D3 == D2 && (i + d) < cVol->poly3d.size() - 1) 
				{
					D3 = cVol->poly3d[i + 1 + d].planeD;
					d++;
				}

				if (i != 0)
				{
					D1 = cVol->poly3d[i - 1].planeD;
				}
			}

			vol += (D3 - D1) *  0.5 * cVol->poly3d[i].area;
		}
	}
	else
	{
		D1 = 0;

		for (long i = 0; i < cVol->poly3d.size() - 1; i++)
		{
			Pk = cVol->poly3d[i].cgravity - cVol->poly3d[i + 1].cgravity;
			D1 += ~Pk;
		}

		vol = cVol->poly3d[0].area * D1;
	}

	return vol;
}


//----------------------------------------------------------------------
// calculateEnclosedCube
//
// Return an enclosing cube around one 3D volume measurement
//
// Input:
// MPoly3D structure contains 3D volume measurement
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::calculateEnclosedCube(MPoly3D * cVol)
{
	float minx = 10000, miny = 10000, minz = 10000, maxx = -10000, maxy = -10000, maxz = -10000;

	for (long i = 0; i < cVol->poly3d.size(); i++)
	{	
		minx = getMin(minx, cVol->poly3d[i].min_coord.x);
		maxx = getMax(maxx, cVol->poly3d[i].max_coord.x);
		miny = getMin(miny, cVol->poly3d[i].min_coord.y);
		maxy = getMax(maxy, cVol->poly3d[i].max_coord.y);
		minz = getMin(minz, cVol->poly3d[i].min_coord.z);
		maxz = getMax(maxz, cVol->poly3d[i].max_coord.z);
	}

	cVol->min_coord.x = minx;
	cVol->min_coord.y = miny;
	cVol->min_coord.z = minz;

	cVol->max_coord.x = maxx;
	cVol->max_coord.y = maxy;
	cVol->max_coord.z = maxz;
}


//----------------------------------------------------------------------
// polyParam
//
// When all the vertices of a polygon is given (in a vector form)  with the normal of the plane it lies on,  
// the area, perimeter and center of gravity is calculated.
//
// Input:
// MPoly2D structure contains 2D area measurement
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::polyParam(MPoly2D *obj)
{
	assert(obj->poly2d.size() >= 3);
	float minx = 10000, miny = 10000, minz = 10000, maxx = -10000, maxy = -10000, maxz = -10000;
	Vector3 ctr(0,0,0), prod(0,0,0);
	float a = 0, p = 0;
	
	long j = obj->poly2d.size();

	for(long i = 0; i < obj->poly2d.size() - 1; i++)
	{
		ctr += obj->poly2d[i];
		p += ~(obj->poly2d[i] - obj->poly2d[i + 1]);
		prod += obj->poly2d[i] * obj->poly2d[i + 1];
		minx = getMin(minx, obj->poly2d[i].x);
		maxx = getMax(maxx, obj->poly2d[i].x);
		miny = getMin(miny, obj->poly2d[i].y);
		maxy = getMax(maxy, obj->poly2d[i].y);
		minz = getMin(minz, obj->poly2d[i].z);
		maxz = getMax(maxz, obj->poly2d[i].z);
	}

	obj->param[0] = obj->area = 0.5 * fabs(obj->planeNormal % prod);
	obj->param[1] = p; // perimeter
	obj->cgravity = ctr / (obj->poly2d.size() - 1);
	obj->min_coord = Vector3(minx, miny, minz);
	obj->max_coord = Vector3(maxx, maxy, maxz);
}


//----------------------------------------------------------------------
// detSide
//
// Determine whether a given point is inside, outside or on a given plane 
//
// Input:
// Vector3 structure contains the input point
// Plane structure contains the input plane parameters
//
// Output:
// return 1 if yes, 0 otherwise
//----------------------------------------------------------------------
long CMeasure::detSide(Vector3* point, const Plane* plane)
{
	float disp = (*point) % plane->normal - plane->D;

	if (disp < 0)
	{
		return -1;
	}
	else if (disp > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//----------------------------------------------------------------------
// copyParam
//
// Copy parameters of specified object in the measurement list
//
// Input:
// index of the object in the measurement list
//
// Output:
// parameters copied to MObj mobj measurement object
//----------------------------------------------------------------------
void CMeasure::copyParam(long index, MObj &mobj)
{
	mobj.id = index;
	mobj.type = m_measureList[index]->type;
	mobj.subtype = m_measureList[index]->subtype;
	mobj.attrib = m_measureList[index]->attrib;
}


//----------------------------------------------------------------------
// ScaleObject
//
// Scale the given object arround its center of gravity 
//
// Input:
// Vector3 structure contains scaling factor for x-y-z directions
// index of the object to scale in the measurement list
//
// Output:
// true if success, false otherwise
//----------------------------------------------------------------------
bool CMeasure::ScaleObject(Vector3 scale, long index)
{
	if (index < 0 || index >= m_measureList.size())
	{
		return false;
	}

	Vector3 origin(0,0,0);
	MObj *obj = m_measureList[index];

	switch (obj->type)
	{
		case MPOINT:
			return false; // not meaningful
			break;

		case MLINE:
			origin = (((MLine *)obj)->start + ((MLine *)obj)->end)/2.0;
			break;

		case MANGLE:
			origin = ((MAngle *)obj)->middle;
			break;

		case MAREA:
			origin = ((MPoly2D *)obj)->cgravity;
			break;

		case MVOLUME:
			{
				// crude way to get the middle
				long mid = ((MPoly3D *)obj)->poly3d.size()/2;
				origin = ((MPoly3D *)obj)->poly3d[mid].cgravity;
			}
			break;
	}

	return ScaleObject(scale, origin, index);
}


//----------------------------------------------------------------------
// ScaleObject
//
// Scale an object in the measurement list
//
// Input:
// scale Vector3 structure contains scaling factor for x-y-z directions
// origin Vector3 structure contains the center of gravity
// index of the object to scale in the measurement list
//
// Output:
// true if success, false otherwise
//----------------------------------------------------------------------
bool CMeasure::ScaleObject(Vector3 scale, Vector3 Origin, long index)
{

	if (index < 0 || index >= m_measureList.size())
	{
		return false;
	}

	MObj *obj = m_measureList[index];

	switch (obj->type)
	{
		case MPOINT:
			obj->param[0] = (obj->param[1] - Origin.x) * scale.x + Origin.x;
			obj->param[1] = (obj->param[1] - Origin.y) * scale.y + Origin.y;
			obj->param[2] = (obj->param[2] - Origin.z) * scale.z + Origin.z;
			break;
		
		case MLINE:
			((MLine *)obj)->start = geom::TransformPointScale(scale, ((MLine *)obj)->start , Origin);
			((MLine *)obj)->end = geom::TransformPointScale(scale, ((MLine *)obj)->end , Origin);
			obj->param[0] = ~(((MLine *)obj)->start - ((MLine *)obj)->end);
			break;
		
		case MANGLE:
			((MAngle *)obj)->start = geom::TransformPointScale(scale, ((MAngle *)obj)->start , Origin);
			((MAngle *)obj)->middle = geom::TransformPointScale(scale, ((MAngle *)obj)->middle , Origin);
			((MAngle *)obj)->end = geom::TransformPointScale(scale, ((MAngle *)obj)->end , Origin);
			break;
		
		case MAREA:
			scalePoly2D((MPoly2D *)obj, scale, Origin);
			break;
		
		case MVOLUME:
			for (long i = 0; i < ((MPoly3D *)obj)->poly3d.size(); i++)
			{
				MPoly2D* ipa = &((MPoly3D *)obj)->poly3d[i];
				scalePoly2D(ipa, scale, Origin);
			}

			obj->param[0] = calcVolume((MPoly3D *)obj); // calculate the volume
			break;
	}

	return true;
}


//----------------------------------------------------------------------
// scalePoly2D
//
// Scale 2D poly object when the scaling factor and the origin is given
//
// Input:
// MPoly2D obj is 2D measurement polygon to scale
// scale Vector3 structure contains scaling factor for x-y-z directions
// origin Vector3 structure contains the center of gravity
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::scalePoly2D(MPoly2D *obj, Vector3 scale, Vector3 Origin)
{
	for (Vector3Vec::iterator i = obj->poly2d.begin(); i != obj->poly2d.end(); i++)
	{
		*i = geom::TransformPointScale(scale, *i, Origin);
	}

	if (fabs(Origin%obj->planeNormal - obj->planeD) > 0.05) // origin is not in the same plane as the contour
	{
		obj->planeD = ~(geom::TransformPointScale(scale, obj->planeD*obj->planeNormal, Origin))*geom::sgn(obj->planeD);
	}

	polyParam(obj);
}


//----------------------------------------------------------------------
// ScaleObject
//
// Scale all objects of a specified subtype and type when the scaling 
// factor and the origin is given
//
// Input:
// scale Vector3 structure contains scaling factor for x-y-z directions
// origin Vector3 structure contains the center of gravity
// type specifies if its a point, line, circle, polygon, volume etc
// measurement subtype - allows 2^16 subtypes
//
// Output:
// true if success, false otherwise
//----------------------------------------------------------------------

bool CMeasure::ScaleObject(Vector3 scale, Vector3 Origin, unsigned short type, unsigned short subtype)
{
	bool ret = false;

	for (long i=0; i <m_measureList.size(); i++)
	{
		MObj *obj = m_measureList[i];

		if ((type == MSUB_UNDEFINED || obj->type == type) && (type == MSUB_UNDEFINED || obj->subtype == subtype))
		{
			ret = ScaleObject(scale, Origin, i);
		}
	}

	return ret;
}


//----------------------------------------------------------------------
// ScaleObject
//
// Scale all objects of a specified subtype and type when the scaling 
// factor and the origin is given 
//
// Input:
// scale Vector3 structure contains scaling factor for x-y-z directions
// type specifies if its a point, line, circle, polygon, volume etc
// measurement subtype - allows 2^16 subtypes
//
// Output:
// true if success, false otherwise 
//----------------------------------------------------------------------
bool CMeasure::ScaleObject(Vector3 scale, unsigned short type, unsigned short subtype)
{
	bool ret = false;

	for (long i=0; i <m_measureList.size(); i++)
	{
		MObj *obj = m_measureList[i];

		if ((type == MSUB_UNDEFINED || obj->type == type) && (type == MSUB_UNDEFINED || obj->subtype == subtype))
		{
			ret = ScaleObject(scale, i);
		}
	}

	return ret;
}


//----------------------------------------------------------------------
// RotateObject
//
// Rotate one of measurements around a when the angle of rotation 
// and the origin is given
//
// Input:
// double sintheta, double costheta - is angle of rotation
// origin Vector3 structure contains the center of rotation
// index of the measurement object in the measurement list to rotate
//
// Output:
// true if success, false otherwise
//----------------------------------------------------------------------
bool CMeasure::RotateObject(double sintheta, double costheta, Vector3 Origin, long index)
{
	if (index < 0 || index >= m_measureList.size())
	{
		return false;
	}

	MObj *obj = m_measureList[index];

	switch (obj->type)
	{
		case MPOINT:
			{
				Vector3 pt(obj->param[0],obj->param[1],obj->param[2]);
				pt = geom::TransformPointAngle(sintheta, costheta, pt , Origin);
				obj->param[0] = pt.x; obj->param[1] = pt.y; obj->param[2] = pt.z;
			}
			break;

		case MLINE:
			((MLine *)obj)->start = geom::TransformPointAngle(sintheta, costheta, ((MLine *)obj)->start , Origin);
			((MLine *)obj)->end = geom::TransformPointAngle(sintheta, costheta, ((MLine *)obj)->end , Origin);
			obj->param[0] = ~(((MLine *)obj)->start - ((MLine *)obj)->end);
			break;

		case MANGLE:
			((MAngle *)obj)->start = geom::TransformPointAngle(sintheta, costheta, ((MAngle *)obj)->start , Origin);
			((MAngle *)obj)->middle = geom::TransformPointAngle(sintheta, costheta, ((MAngle *)obj)->middle , Origin);
			((MAngle *)obj)->end = geom::TransformPointAngle(sintheta, costheta, ((MAngle *)obj)->end , Origin);
			break;

		case MAREA:
			rotatePoly2D((MPoly2D *)obj, sintheta, costheta, Origin);
			break;

		case MVOLUME:
			for (long i = 0; i < ((MPoly3D *)obj)->poly3d.size(); i++)
			{
				MPoly2D* ipa = &((MPoly3D *)obj)->poly3d[i];
				rotatePoly2D(ipa, sintheta, costheta, Origin);
			}

			obj->param[0] = calcVolume((MPoly3D *)obj); // calculate the volume
			break;
	}

	return true;
}


//----------------------------------------------------------------------
// RotateObject
//
// Rotate (on x plane) all objects of a specified id and type when the angle 
// of rotation and the origin is given 
//
// Input:
// double sintheta, double costheta - is angle of rotation
// origin Vector3 structure contains the center of rotation
// type specifies if its a point, line, circle, polygon, volume etc
// measurement subtype - allows 2^16 subtypes
//
// Output:
// true if success, false otherwise
//----------------------------------------------------------------------
bool CMeasure::RotateObject(double sintheta, double costheta, Vector3 Origin, unsigned short type, unsigned short subtype)
{
	bool ret = false;
	for (long i=0; i < m_measureList.size(); i++)
	{
		MObj *obj = m_measureList[i];

		if ((type == MSUB_UNDEFINED || obj->type == type) && (type == MSUB_UNDEFINED || obj->subtype == subtype))
		{
			ret = RotateObject(sintheta, costheta, Origin, i);
		}
	}

	return ret;
}


//----------------------------------------------------------------------
// rotatePoly2D
//
// Rotate one of measurements around when the angle of rotation 
// and the origin is given  
//
// Input:
// MPoly2D obj is the polygon to rotate
// double sintheta, double costheta - is angle of rotation
// origin Vector3 structure contains the center of rotation
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::rotatePoly2D(MPoly2D *obj, double sintheta, double costheta, Vector3 Origin)
{
	for (Vector3Vec::iterator i = obj->poly2d.begin(); i != obj->poly2d.end(); i++)
	{
		*i = geom::TransformPointAngle(sintheta, costheta, *i, Origin);
	}

	if (fabs((obj->planeNormal/~(obj->planeNormal))%Vector3(1,0,0) - 1.0) > 0.001)
	{
		Vector3 p = geom::TransformPointAngle(sintheta, costheta, obj->planeD*obj->planeNormal, Origin);
		obj->planeD = ~(p);
		obj->planeNormal = p/obj->planeD;
	}

	polyParam(obj);
}


//----------------------------------------------------------------------
// GetObject
//
// Get the index of the object with matching type and ID (there can only be one) 
//
// Input:
// type specifies if its a point, line, circle, polygon, volume etc
// id is an unique id of the measurement object
//
// Output:
// index of the found object, -1 if not found
//----------------------------------------------------------------------
long CMeasure::GetObject(unsigned short type, unsigned short id)
{
	for (long i=0; i <m_measureList.size(); i++)
	{
		if (m_measureList[i]->type == type && m_measureList[i]->id == id)
		{
			return i;
		}
	}

	// no match
	return -1;
}


//----------------------------------------------------------------------
// GetObject
//
// Get the index of the object with matching type and ID that satisfies distance
// tolerance from the given point  
//
// Input:
// Vector3 structure contains the given point
// type specifies if its a point, line, circle, polygon, volume etc
// measurement subtype - allows 2^16 subtypes
// delta is the distance tolerance
//
// Output:
// index of the found object, -1 if not found
//----------------------------------------------------------------------
long CMeasure::GetObject(Vector3 point, unsigned short type, unsigned short subtype, float delta)
{
	if (delta < 0)
	{
		delta = FLT_MAX;
	}

	float min_dist = FLT_MAX;
	long ret = -1, i, j;
	float this_dist, tmp;

	for (i = 0; i < m_measureList.size(); i++)
	{
		MObj *obj = m_measureList[i];

		if ((type == MSUB_UNDEFINED || obj->type == type) && (type == MSUB_UNDEFINED || obj->subtype == subtype))
		{
			switch (obj->type)
			{
				case MPOINT:
					this_dist = ~(point - Vector3(obj->param[0], obj->param[1], obj->param[2]));
					break;

				// For all other types, the criteria is the perpendicular distance from point to any edge
				case MLINE:
					this_dist = geom::distance(point, ((MLine *)obj)->start, ((MLine *)obj)->end);
					break;

				case MANGLE:
					this_dist = geom::distance(point, ((MAngle *)obj)->start, ((MAngle *)obj)->middle);
					tmp = geom::distance(point, ((MAngle *)obj)->end, ((MAngle *)obj)->middle);
					if (tmp < this_dist)
					{
						this_dist = tmp;
					}
					break;

				case MAREA:
					this_dist = geom::distance(point, ((MPoly2D *)obj)->poly2d);
					break;

				case MVOLUME:
					this_dist = geom::distance(point, ((MPoly3D *)obj)->poly3d[0].poly2d);
					for (j=0; j < ((MPoly3D *)obj)->poly3d.size(); j++)
					{
						tmp = geom::distance(point, ((MPoly3D *)obj)->poly3d[i].poly2d);
						if (tmp < this_dist)
						{
							this_dist = tmp;
						}
					}
					break;

				default:
					this_dist =  FLT_MAX;
					break;

			} 

			if (this_dist < delta && this_dist < min_dist)
			{
				min_dist = this_dist;
				ret = i;
			}
		} 
	} 

	return ret;
}


//----------------------------------------------------------------------
// getClosest
//
// Returns -1 if no points are within the tolerance 
//
// Input:
// Vector3 structure contains the given point
// Vector3Vec poly2d contains the polygon to inspect
// delta is the distance tolerance
//
// Output:
// index of the point in the polygon, -1 if not found
//----------------------------------------------------------------------
long CMeasure::getClosest(const Vector3 point, const Vector3Vec& poly2d, float delta)
{
	float min_dist = delta;
	long closest = -1;

	for (long i = 0; i < poly2d.size(); i++)
	{
		float dist = ~(point - poly2d[i]);

		if (dist < min_dist)
		{
			min_dist = dist;
			closest = i;
		}
	}

	return closest;
}


//----------------------------------------------------------------------
// getClosestEdge
//
// Returns -1 if no points are within the tolerance. 
// Edge is return value and next vertex
//
// Input:
// Vector3 structure contains the given point
// Vector3Vec poly2d contains the polygon to inspect
// delta is the distance tolerance
//
// Output:
// index of the point in the polygon, -1 if not found
//----------------------------------------------------------------------
long CMeasure::getClosestEdge(const Vector3 point, const Vector3Vec& poly2d, float delta)
{
	float min_dist = delta;
	long closest = -1;

	for (long i = 1; i < poly2d.size(); i++)
	{
		// pick the edge that subtends the biggest angle. This seems to work the best for most cases
		Vector3 u = poly2d[i-1] - point;
		Vector3 v = poly2d[i] - point;

		float dist = fabs(-1 - u%v/(~u * ~v));

		if (dist < min_dist)
		{
			min_dist = dist;
			closest = i;
		}
	}

	return closest;
}


//----------------------------------------------------------------------
// movePoly2D
//
// Move all points of 2D poly object by a specified offset
//
// Input:
// MPoly2D obj is the polygon to move
// Vector3 offset contains the offset to move by
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::movePoly2D(MPoly2D* obj, const Vector3 offset)
{
	for (Vector3Vec::iterator i = obj->poly2d.begin(); i != obj->poly2d.end(); i++)
	{
		*i += offset;
	}

	obj->max_coord += offset;
	obj->min_coord += offset;
	obj->cgravity += offset;
}


//----------------------------------------------------------------------
// CopyObject
//
// Make a copy of the selected measurement at the given offset 
//
// Input:
// Vector3 structure contains offset to move the newly created object by
// long index - is the index in the measurement list to make copy from 
//
// Output:
// the unique id of the new object
//----------------------------------------------------------------------
long CMeasure::CopyObject(const Vector3 offset, long index)
{

	if (index >=0 || index < m_measureList.size())
	{
		MObj *copy = 0, *obj = m_measureList[index];

		switch(obj->type)
		{
			case MPOINT: // point 
				{
					MPoint *tmp = new MPoint;
					*tmp = *(MPoint *)obj;
					tmp->id = m_pointID++;
					copy = tmp;
				}
				break;

			case MLINE:  // line
				{
					MLine *tmp = new MLine;
					*tmp = *(MLine *)obj;
					tmp->id = m_lineID++;
					copy = tmp;
				}
				break;

			case MANGLE: // angle
				{
					MAngle *tmp = new MAngle;
					*tmp = *(MAngle *)obj;
					tmp->id = m_angleID++;
					copy = tmp;
				}
				break;

			case MAREA: // area
				{
					MPoly2D *tmp = new MPoly2D;
					*tmp = *(MPoly2D *)obj;
					tmp->id = m_areaID++;
					copy = tmp;
				}
				break;

			case MVOLUME: // volume
				{
					MPoly3D *tmp = new MPoly3D;
					*tmp = *(MPoly3D *)obj;
					tmp->id = m_volumeID++;
					copy = tmp;
				}
				break;
		}

		if (copy)
		{
			m_measureList.push_back(copy);

			if (m_curObj == index) // clear the selection flag
			{
				ClearAttrib(m_measureList.size()-1, MATTR_SELECT);
			}

			MoveObject(offset, m_measureList.size()-1);

			return copy->id;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

}


//----------------------------------------------------------------------
// MoveObject
//
// Move the selected measurement at the given offset 
//
// Input:
// Vector3 structure contains offset
// long index - is the index in the measurement list
//
// Output:
// true if success, false otherwise
//----------------------------------------------------------------------
bool CMeasure::MoveObject(const Vector3 offset, long index)
{
	if (index < 0 || index >= m_measureList.size())
	{
		return false;
	}

	MObj *obj = m_measureList[index];

	switch(obj->type)
	{
		case MPOINT: // point
			obj->param[0] += offset.x; obj->param[1] += offset.y; obj->param[2] += offset.z;
			break;

		case MLINE:  // line
			((MLine *)obj)->start += offset; ((MLine *)obj)->end += offset;
			break;

		case MANGLE: // angle
			((MAngle *)obj)->start += offset; ((MAngle *)obj)->end += offset; ((MAngle *)obj)->middle += offset;
			break;

		case MAREA: // area
			movePoly2D((MPoly2D *)obj, offset);
			break;

		case MVOLUME: // volume
			for (long i = 0; i < ((MPoly3D *)obj)->poly3d.size(); i++)
			{
				MPoly2D* ipa = &((MPoly3D *)obj)->poly3d[i];
				movePoly2D(ipa, offset);
			}

			break;
	}

	return true;
}


//----------------------------------------------------------------------
// MoveObject
//
// Move 3D volume measurement object given a type and a subtype 
// by a given offset 
//
// Input:
// Vector3 structure contains offset
// type specifies if its a point, line, circle, polygon, volume etc
// measurement subtype - allows 2^16 subtypes
//
// Output:
// true if success, false otherwise
//----------------------------------------------------------------------
bool CMeasure::MoveObject(const Vector3 offset, unsigned short type, unsigned short subtype)
{
	bool ret = false;

	for (long i=0; i < m_measureList.size(); i++)
	{
		MObj *obj = m_measureList[i];

		if ((type == MSUB_UNDEFINED || obj->type == type) && (type == MSUB_UNDEFINED || obj->subtype == subtype))
		{
			ret = MoveObject(offset, i);
		}
	}
	return ret;
}


//----------------------------------------------------------------------
// GetPoints
//
// Return the PlotVec structure for the input triangle list 
//
// Input:
// ViewORTHO structure - gives all the information of the current state
// TriangleVec surface contains triangles that specify existing surfaces
//
// Output:
// PlotVec structure contains intersection points of the surfaces with
// the cube model faces
//----------------------------------------------------------------------
const PlotVec& CMeasure::GetPoints(const viewORTHO& view, const TriangleVec *surface)	
{
	Vector3 corner1, corner2, corner3;
	
	m_tubeVol = 0;

	// cleanup previous plot
	m_outPoints.erase(m_outPoints.begin(), m_outPoints.end() );
	m_cutPoints.erase(m_cutPoints.begin(), m_cutPoints.end() );

	for (long i = 0; i < view.face.size(); i++)
	{
		if (view.face[i].vert.size() < 3 || view.face[i].plane.normal.z >= -minDist)
		{
			continue;
		}

		for (long j = 0; j <  surface->size(); j++)
		{
			corner1 = (*surface)[j].V1 * view.Model2View;
			corner2 = (*surface)[j].V2 * view.Model2View;
			corner3 = (*surface)[j].V3 * view.Model2View;
			
			determinTrianglePts(corner1, corner2, corner3, view, i, (*surface)[j].contour);
		}
	}

	return m_outPoints;
}


//----------------------------------------------------------------------
// determinTrianglePts
//
// Find tringle intersections with a given plane
//
// Input:
// Vector3 corner1, Vector3 corner2, Vector3 corner3 contain the tringle vertives
// ViewORTHO structure - gives all the information of the current state
// long face id 
// long contid is index in the measurement list
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::determinTrianglePts(Vector3 corner1, Vector3 corner2, Vector3 corner3, const viewORTHO& view, long face, long contid)	
{
	long side1, side2, side3;
	Vector3 inter_pt[2];

	side1 = detSide(&corner1, &(view.face[face].plane));
	side2 = detSide(&corner2, &(view.face[face].plane));
	side3 = detSide(&corner3, &(view.face[face].plane));

	if (side1 == side2 && side1 == side3)
	{
		if (side1 == 0)
		{
			determinPoints(view, &corner1, &corner2, face, MLINE, contid);
			determinPoints(view, &corner1, &corner3, face, MLINE, contid);
			determinPoints(view, &corner3, &corner2, face, MLINE, contid);
			return;
		}
		else
		{
			return;
		}
	}

	if (side1 == 0)
	{
		if (side2 == 0)
		{
			determinPoints(view, &corner1, &corner2, face, MLINE, contid);
			return;
		}

		if (side3 == 0)
		{
			determinPoints(view, &corner1, &corner2, face, MLINE, contid);
			return;
		}

		determinPoints(view, &corner1, &corner1, face, MLINE, contid);
		return;
	}

	if (side2 == 0)
	{
		if (side3 == 0)
		{
			determinPoints(view, &corner3, &corner2, face, MLINE, contid);
			return;
		}

		determinPoints(view, &corner2, &corner2, face, MLINE, contid);
		return;
	}

	long count = 0;

	if (side1 != side2)
	{
		geom::intersect(&corner1, &corner2, &(view.face[face].plane), &inter_pt[count]);
		count++;
	}

	if (side1 != side3)
	{
		geom::intersect(&corner1, &corner3, &(view.face[face].plane), &inter_pt[count]);
		count++;
	}
	
	if (count == 2)
	{
		determinPoints(view, &inter_pt[0], &inter_pt[1], face, MLINE, contid);
		return;
	}

	if (side2 != side3)
	{
		geom::intersect(&corner2, &corner3, &(view.face[face].plane), &inter_pt[count]);
		count++;
	}
	
	if (count == 2)
	{
		determinPoints(view, &inter_pt[0], &inter_pt[1], face, MLINE, contid);
		return;
	}

	return;
}


//----------------------------------------------------------------------
// DeletePolygon
//
// Delete contour from 3D volume measurement
//
// Input:
// obj_no is the index in the measurement list
// contour_no is index of the contour to delete in the 3D volume measurement object
//
// Output:
// none
//----------------------------------------------------------------------
bool CMeasure::DeletePolygon(long obj_no, long contour_no)
{
	bool ret = false;

	if (obj_no < m_measureList.size() && m_measureList[obj_no]->type == MVOLUME)
	{
		MPoly3D *vol = (MPoly3D *)m_measureList[obj_no];

		if (contour_no < vol->poly3d.size())
		{
			m_undoPoly.push_back(vol->poly3d[contour_no]);
			vol->poly3d.erase(vol->poly3d.begin() + contour_no);
			m_measureList[obj_no]->param[0] = calcVolume((MPoly3D *)m_measureList[obj_no]); // calculate the volume
	
			ret = true;
		}
	}
	return ret;
}


//----------------------------------------------------------------------
// UndoLastDeletePolygon
//
// Undo last delete polygon operation
//
// Input:
// none
//
// Output:
// true if success, false otherwise
//----------------------------------------------------------------------
bool CMeasure::UndoLastDeletePolygon()
{
	bool ret = false;

	 // applicable only to volumes
	if (m_undoPoly.size() > 0 && m_curObj >= 0 &&  m_curObj < m_measureList.size() && m_measureList[m_curObj]->type == MVOLUME)
	{
		// can't undelete radial volume 
		if(m_measureList[m_curObj]->subtype == MSUB_MARKER)	
		{
			return ret;
		}

		MPoly3D *vol = (MPoly3D *)m_measureList[m_curObj];

		// Only undo deleted polygons that belong to the selected object
		unsigned short ownerID = m_measureList[GetSelect()]->id;

		for (long i = m_undoPoly.size() - 1 ; i >=0; i--)
		{
			if (m_undoPoly[i].id == ownerID) // found a deleted contour
			{
				bool overlap = 0;

				for (long j = 0; j < vol->poly3d.size(); j++)
				{
					if (fabs(vol->poly3d[j].planeD - m_undoPoly[i].planeD) < 0.05)
					{
						if (   getMin(vol->poly3d[j].max_coord.x,m_undoPoly[i].max_coord.x) + 0.1 >= getMax(vol->poly3d[j].min_coord.x,m_undoPoly[i].min_coord.x)
							&& getMin(vol->poly3d[j].max_coord.y,m_undoPoly[i].max_coord.y) + 0.1 >= getMax(vol->poly3d[j].min_coord.y,m_undoPoly[i].min_coord.y)
							&& getMin(vol->poly3d[j].max_coord.z,m_undoPoly[i].max_coord.z) + 0.1 >= getMax(vol->poly3d[j].min_coord.z,m_undoPoly[i].min_coord.z))
						{
								overlap = 1;
						}
					}
				}

				if (!overlap)
				{
				
					vol->poly3d.push_back(m_undoPoly[i]);
					STL::sort(vol->poly3d.begin(), vol->poly3d.end(), compare4);
					vol->param[0] = calcVolume(vol); // calculate the volume
				
					m_undoPoly.erase(m_undoPoly.begin() + i);
					m_measureList[m_curObj]->param[0] = calcVolume((MPoly3D *)m_measureList[m_curObj]); // calculate the volume
	
					ret = true;
					break;
				}
			}
		}
	}

	return ret;
}


//----------------------------------------------------------------------
// MovePolygon
//
// Move one of contours of one of 3D volume measurements by a given offset
//
// Input:
// Vector3 offset contains the offset to move the polygon by
// obj_no is the index in the measurement list
// contour_no is index of the contour to delete in the 3D volume measurement object
//
// Output:
// none 
//----------------------------------------------------------------------
bool CMeasure::MovePolygon(const Vector3 offset, long obj_no, long contour_no)
{
	bool ret = false;

	if (obj_no < m_measureList.size() && m_measureList[obj_no]->type == MVOLUME)
	{
		MPoly3D *vol = (MPoly3D *)m_measureList[obj_no];

		if (contour_no < vol->poly3d.size())
		{
			movePoly2D(&(vol->poly3d[contour_no]), offset);
			m_measureList[obj_no]->param[0] = calcVolume((MPoly3D *)m_measureList[obj_no]); // calculate the volume
	
			ret = true;
		}
	}

	return ret;
}


//----------------------------------------------------------------------
// InsertPolygon
//
// Insert a contour into one of 3D volume measurements
//
// Input:
// obj_no is the index of the object in the measurement list
// ViewORTHO structure - gives all the information of the current state
// mvState structure contains the current state of the visualizer to save
// Vector3Vec poly contains the contour points
// long face is the active face
// subtype - allows 2^16 subtypes
// attrib - attribute value to set
//
// Output:
// true if success, false otherwise
//----------------------------------------------------------------------
bool CMeasure::InsertPolygon(long obj_no, const viewORTHO& view, const mvState *state, const Vector3Vec& poly, long face, unsigned short subtype, unsigned short attrib)
{
	bool ret = false;

	if (poly.size() >= 3 && obj_no < m_measureList.size() && m_measureList[obj_no]->type == MVOLUME)
	{
		m_curPoly.type = MAREA;
		// all contours have the same id as the volume object they belong to
		m_curPoly.id = m_measureList[obj_no]->id; 
		m_curPoly.poly2d = poly;
		m_curPoly.poly2d.push_back(poly.front());
		
		// convert plane equation to model coords
		geom::TransformPlane(view, &m_curPoly.planeNormal, &m_curPoly.planeD, face);
		
		// calculate parameters
		polyParam(&m_curPoly);

		// check whether the polygon overlaps with any other polygons of the volume 
		bool poly_overlap = false;

		if (m_measureList[obj_no]->subtype != MSUB_SEED && m_measureList[obj_no]->subtype != MSUB_NEEDLE && m_measureList[obj_no]->subtype != MSUB_MARKER)
		{
			for (long j = 0; j < ((MPoly3D *)m_measureList[obj_no])->poly3d.size(); j++)
			{
				float D1 = ((MPoly3D *)m_measureList[obj_no])->poly3d[j].planeD;
				float D2 = m_curPoly.planeD;
			
				if (fabs(((MPoly3D *)m_measureList[obj_no])->poly3d[j].planeD - m_curPoly.planeD) < 0.02)
				{
					if (   getMin(((MPoly3D *)m_measureList[obj_no])->poly3d[j].max_coord.x,m_curPoly.max_coord.x) + 0.1 >= getMax(((MPoly3D *)m_measureList[obj_no])->poly3d[j].min_coord.x,m_curPoly.min_coord.x)
						&& getMin(((MPoly3D *)m_measureList[obj_no])->poly3d[j].max_coord.y,m_curPoly.max_coord.y) + 0.1 >= getMax(((MPoly3D *)m_measureList[obj_no])->poly3d[j].min_coord.y,m_curPoly.min_coord.y)
						&& getMin(((MPoly3D *)m_measureList[obj_no])->poly3d[j].max_coord.z,m_curPoly.max_coord.z) + 0.1 >= getMax(((MPoly3D *)m_measureList[obj_no])->poly3d[j].min_coord.z,m_curPoly.min_coord.z))
					{
						poly_overlap = true;
						break;
					}
				}
			}
		}

		if (!poly_overlap) 
		{
			m_curPoly.area = m_curPoly.param[0];

			// copy the cube state
			if (state)
			{
				m_curPoly.state = *state;
			}

			((MPoly3D *)m_measureList[obj_no])->poly3d.push_back(m_curPoly);
		
			if (m_measureList[obj_no]->subtype != MSUB_SEED && m_measureList[obj_no]->subtype != MSUB_NEEDLE && m_measureList[obj_no]->subtype != MSUB_MARKER)	
			{
				STL::sort(((MPoly3D *)m_measureList[obj_no])->poly3d.begin(), ((MPoly3D *)m_measureList[obj_no])->poly3d.end(), compare4);
			}

			m_measureList[obj_no]->param[0] = calcVolume((MPoly3D *)m_measureList[obj_no]); // calculate the volume
		
			ret = true;
		}

		m_curPoly.poly2d.erase(m_curPoly.poly2d.begin(), m_curPoly.poly2d.end() );
	}

	return ret;
}


//----------------------------------------------------------------------
// ScalePolygon
//
// Scale one of contours of one of 3D volume measurements
//
// Input:
// Vector3 structure contains scaling factor for x-y-z directions
// obj_no is the index in the measurement list
// contour_no is index of the contour to delete in the 3D volume measurement object
//
// Output:
// true if success, false otherwise
//----------------------------------------------------------------------
bool CMeasure::ScalePolygon(Vector3 scale, long obj_no, long contour_no)
{
	bool ret = false;

	if (obj_no < m_measureList.size() && m_measureList[obj_no]->type == MVOLUME)
	{
		MPoly3D *vol = (MPoly3D *)m_measureList[obj_no];

		if (contour_no < vol->poly3d.size())
		{
			scalePoly2D(&(vol->poly3d[contour_no]), scale, vol->poly3d[contour_no].cgravity);
			m_measureList[obj_no]->param[0] = calcVolume((MPoly3D *)m_measureList[obj_no]); // calculate the volume
	
			ret = true;
		}
	}

	return ret;
}


//----------------------------------------------------------------------
// RotatePolygon
//
// Rotate one of contours of one of 3D volume measurements around a given
// point by a given degree
//
// Input:
// double sintheta, double costheta specify the angle of rotation
// origin Vector3 structure contains the center of rotation
// obj_no is the index in the measurement list
// contour_no is index of the contour to delete in the 3D volume measurement object
//
// Output:
// true if success, false otherwise
//----------------------------------------------------------------------
bool CMeasure::RotatePolygon( double sintheta, double costheta, Vector3 Origin, long obj_no, long contour_no)
{
	bool ret = false;

	if (obj_no < m_measureList.size() && m_measureList[obj_no]->type == MVOLUME)
	{
		MPoly3D *vol = (MPoly3D *)m_measureList[obj_no];

		if (contour_no < vol->poly3d.size())
		{
			rotatePoly2D(&(vol->poly3d[contour_no]), sintheta, costheta, Origin);
			ret = true;
		}
	}

	return ret;
}


//----------------------------------------------------------------------
// ReplacePolygon
//
// Replace one contour by another in the 3D volume measurement list
//
// Input:
// obj_no is the index in the measurement list
// contour_no is index of the contour to delete in the 3D volume measurement object
// Vector3Vec poly contains the new polygon
//
// Output:
// true if success, false otherwise
//----------------------------------------------------------------------
bool CMeasure::ReplacePolygon(long obj_no, long contour_no, const Vector3Vec& poly)
{
	bool ret = false;

	if (poly.size() >= 3 && obj_no < m_measureList.size() && m_measureList[obj_no]->type == MVOLUME)
	{
		MObj *obj = m_measureList[obj_no];

		MPoly2D *polygon = &((MPoly3D *)obj)->poly3d[contour_no];
	
		polygon->poly2d.clear();
		polygon->poly2d = poly;
		polygon->poly2d.push_back(poly.front());	
	}

	return ret;
}


//----------------------------------------------------------------------
// ReplaceArea
//
// Replace one area measurement by another one inside the measurement list
//
// Input:
// obj_no is the index in the measurement list
// Vector3Vec poly contains the new area
//
// Output:
// true if success, false otherwise
//----------------------------------------------------------------------
bool CMeasure::ReplaceArea(long obj_no, const Vector3Vec& poly)
{
	bool ret = false;

	if (poly.size() >= 3 && obj_no < m_measureList.size() && m_measureList[obj_no]->type == MAREA)
	{
		MPoly2D *ipa = reinterpret_cast<MPoly2D*>(m_measureList[obj_no]);
	
		ipa->poly2d.clear();
		ipa->poly2d = poly;
		ipa->poly2d.push_back(poly.front());	
	}

	return ret;
}


//----------------------------------------------------------------------
// PointInsideTarget
//
// Find out whether a point is inside a 3D organ 
//
// Input:
// ViewORTHO structure - gives all the information of the current state
// face1 is the face id in the cube model
// Vector3 structure contains the point in question
// TargetID is the unique id of the measurement object 
//
// Output:
// true if success, false otherwise
//----------------------------------------------------------------------
bool CMeasure::PointInsideTarget(const viewORTHO& view1, long face1, Vector3 point, short TargetID)
{
	float PlaneD, D1, D2, D3;
	Vector3 Plnormal, prev, point1, point2, point3, point4;
	int count = 0, d;
	MPoly3D *opv;

	long index = GetObject(MVOLUME, TargetID);
	geom::TransformPlane(view1, &Plnormal, &PlaneD, face1);
	MObj * obj = m_measureList[index];

	opv = reinterpret_cast<MPoly3D*> (obj);

	if ((PlaneD + minDist) < opv->poly3d[0].planeD || (PlaneD - minDist) > opv->poly3d[opv->poly3d.size() -1].planeD)
	{
		return false;
	}

	for (long m = 0; m < opv->poly3d.size(); m++)
	{
		m_curVol3 = opv->poly3d[m];

		D1 = D2 = D3 = m_curVol3.planeD;
		d = 0;

		while (D3 == D2 && (m + d) < opv->poly3d.size() - 1) 
		{
			D3 = opv->poly3d[m + 1 + d].planeD;
			d++;
		}

		if (m != 0)
		{
			D1 = opv->poly3d[m - 1].planeD;
		}
		
		if ((PlaneD + minDist) < (D1 + D2)/ 2.0 || (PlaneD - minDist) > (D2 + D3))
		{
			continue;
		}

		m_cutPoly.erase(m_cutPoly.begin(), m_cutPoly.end());
		m_prevSide = -2;
		m_prevX = m_prevY = m_prevZ = 100000;

		for (long j = m_curVol3.poly2d.size() - 1; j >= 1; j--)
		{
			point1 = m_curVol3.poly2d[j];
			point2 = m_curVol3.poly2d[j-1];

			point3 = point1 - point;
			point4 = point2 - point;

			if (~point4 < minDist || ~point3 < minDist)
			{
				return(1);
			}

			if (j == m_curVol3.poly2d.size() - 1)
			{
				m_prevX = point1.x;
				m_prevY = point1.y;
				m_prevZ = point1.z;
			}

			findIntersections(point1, point2, point);
		}

		if (m_cutPoly.size() > 1)
		{
			STL::sort(m_cutPoly.begin(), m_cutPoly.end(), compare1);
			
			float A = fabs(m_cutPoly[0].y - m_cutPoly[m_cutPoly.size() - 1].y);
	
			if (A == 0)
			{
				if (point.x != m_cutPoly[0].x || point.y != m_cutPoly[0].y
					|| point.z != m_cutPoly[0].z) 
					return false;

				else
					return true;
			}
	
			for (long k = 0; k < m_cutPoly.size(); k++)
			{
			
				if (m_cutPoly[k].y <= point3.y)
				{
					count++;
				}
				else
				{
					break;
				}
			
			}
			
			if ((count % 2) == 0)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}

	}

	return false;
}


//----------------------------------------------------------------------
// findIntersections
//
// Find intersection between two lines. First line specified with first poins.
// Second line is a vertical line that intersects third point. 
//
// Input:
// point1, point2 are endpoints of the first line
// point is the point that lies on the verical line
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::findIntersections(Vector3 point1, Vector3 point2, Vector3 point)
{
	Vector3 respt;

	long cond1, cond2;

	if (point1.z < point.z)
	{
		cond1 = -1;
	}
	else
	{
		if (point1.z > point.z)
		{
			cond1 = 1;
		}
		else
		{
			cond1 = 0;
		}
	}

	if (point2.z < point.z)
	{
		cond2 = -1;
	}
	else
	{
		if (point2.z > point.z)
		{
			cond2 = 1;
		}
		else
		{
			cond2 = 0;
		}
	}

	if (cond1 == cond2)
	{
		if (cond1 != 0)
		{
			return;
		}
		else
		{
	
			m_cutPoly.push_back(point1);
			m_cutPoly.push_back(point2);
		}
	}
	else
	{
		
		if (cond1 == 0)
		{
			if (cond2 == m_prevSide || m_prevSide == -2)
			{
				m_cutPoly.push_back(point1);
			}
			
			if (m_prevSide == -2)
			{
				m_firstSide = cond2;
			}
			
			m_prevSide = cond2;

			return;
		}
		
		if (cond2 == 0)
		{
		
			if (m_prevSide == -2 || long (point2.x) != m_prevX || long (point2.y) != m_prevY ||	long(point.z) != m_prevZ)
			{
				m_cutPoly.push_back(point2);
			}
			else
			{
				if (cond1 == m_firstSide)
				{
					m_cutPoly.push_back(point2);
				}
			}

			if (m_prevSide == -2)
			{
				m_firstSide = cond1;
			}
			
			m_prevSide = cond1;

			return;
		}

		respt = point;

		respt.y =  point1.y + (point2.y - point1.y) * (point.z - point1.z) / (point2.z - point1.z);

		m_cutPoly.push_back(respt);
		
		if (m_prevSide == -2)
		{
			m_firstSide = cond2;
		}

		m_prevSide = cond2;

		return;
	}
}


//----------------------------------------------------------------------
// writeField
//
// Write a field (tag, value and data) to an ostream object
// if data is given, value must contain the length of data buffer 
//
// Input:
// opened ostream object to write to
// tag is tag to write
// value contains the length of data buffer
// char data is data to write
//
// Output:
// none
//----------------------------------------------------------------------

void CMeasure::writeField(STD::ostream& out, const DTag tag, const long value, const char *data)
{
	assert(sizeof(DTag) == 4);

	out.write((char *)&tag, 4);
	out.write((char *)&value, 4);

	if (data && value)
	{
		out.write(data, value);
	}
}


//----------------------------------------------------------------------
// writeField
//
// Write a field (tag, value and data) to an ostream object
// if data is given, value must contain the length of data buffer  
//
// Input:
// opened ostream object to write to
// tag is tag to write
// value contains the length of data buffer
// char data is data to write
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::writeField(STD::ostream& out, const DTag tag, const long value, const long data)
{
	assert(sizeof(DTag) == 4);

	out.write((char *)&tag, 4);
	out.write((char *)&value, 4);
	out.write((char *)&data, 4);
}


//----------------------------------------------------------------------
// writeField
//
// Write a field (tag, value and data) to an ostream object
// if data is given, value must contain the length of data buffer  
//
// Input:
// opened ostream object to write to
// tag1 is tag to write
// tag2 is tag to write
// value contains the length of data buffer
// char data is data to write
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::writeField(STD::ostream &out, long tag1, long tag2, long value, const char *data)
{
	out.write((char *)&tag1, 4);
	out.write((char *)&tag2, 4);

	out.write((char *)&value, 4);

	if (data && value)
	{
		out.write(data, value);
	}
}


//----------------------------------------------------------------------
// SaveMeasurements
//
// Save 2d or 3D measurements to ldd file
//
// Input:
// opened ostream object to write to
// char instanceName contains "m_measurements" to save 2D measurements or 
// "m_contours" to save 3D measurements
//
// Output:
// none
//----------------------------------------------------------------------								
long CMeasure::SaveMeasurements(STD::ofstream& out, const char *instanceName)
{
	DTag tag;
	Vector3 vec;
	const char *version = "LIS Dynamic Data Version 1.0"; // char. string lengths must be even
	
	// write the header
	tag.group = SVF_TYPE; tag.element = SVE_TYPE;

	writeField(out, tag, strlen(version)+1, version);

	tag.group = MEASURE_DATA;

	tag.element = MEASURE_INSTANCE; 
	writeField(out, tag, (instanceName)?strlen(instanceName)+1:0, instanceName);

	for (MObjpVec::iterator i = m_measureList.begin(); i != m_measureList.end(); i++) 
	{
		tag.element = MEASURE_TYPE; 
		writeField(out, tag, sizeof((*i)->type), (char*)&((*i)->type));

		tag.element = MEASURE_SUBTYPE; 
		writeField(out, tag, sizeof((*i)->subtype), (char*)&((*i)->subtype));

		tag.element = MEASURE_ATTRIB; 
		writeField(out, tag, sizeof((*i)->attrib), (char*)&((*i)->attrib));

		tag.element = MEASURE_VA_FIRST; 
		writeField(out, tag, sizeof((*i)->axis_first), (char*)&((*i)->axis_first));

		tag.element = MEASURE_VA_SECOND; 
		writeField(out, tag, sizeof((*i)->axis_second), (char*)&((*i)->axis_second));

		tag.element = MEASURE_NAME; 
		writeField(out, tag, sizeof((*i)->name), (char*)&((*i)->name));
		
		tag.element = MEASURE_ID; 
		writeField(out, tag, sizeof((*i)->id), (char*)&((*i)->id));

		tag.element = MEASURE_PARAM; 
		writeField(out, tag, sizeof((*i)->param), (char*)&((*i)->param));

		tag.element = MEASURE_CUSTOM; 
		writeField(out, tag, sizeof((*i)->custom), (char*)&((*i)->custom));

		switch ((*i)->type) 
		{
			case MPOINT: // nothing to do as the data is in param[]
				{
					MPoint *p = (MPoint *)(*i);
					saveViewsToFile(out, p->state);
					break;
				}
		
			case MLINE:
				{
					tag.element = MEASURE_VEC3; 
					MLine *p = (MLine *)(*i);
					writeField(out, tag, sizeof(p->start), (char*)&(p->start));
					writeField(out, tag, sizeof(p->end), (char*)&(p->end));
					saveViewsToFile(out, p->state);
					break;
				}
		
			case MANGLE:
				{
					tag.element = MEASURE_VEC3; 
					MAngle *q = (MAngle *)(*i);
					writeField(out, tag, sizeof(q->start), (char*)&(q->start));
					writeField(out, tag, sizeof(q->middle), (char*)&(q->middle));
					writeField(out, tag, sizeof(q->end), (char*)&(q->end));
					saveViewsToFile(out, q->state);
					break;
				}

			case MAREA:
				{
					MPoly2D *mp2 = (MPoly2D *)(*i);
					tag.element = MEASURE_PLANE_NORMAL; 
					writeField(out, tag, sizeof(mp2->planeNormal), (char*)&(mp2->planeNormal));
					tag.element = MEASURE_PLANE_DEPTH; 
					writeField(out, tag, sizeof(mp2->planeD), (char*)&(mp2->planeD));
					saveViewsToFile(out, mp2->state);
					tag.element = MEASURE_VEC3; 
					
					for (long in = 0; in < mp2->poly2d.size(); in++)
					{
						Vector3 current = mp2->poly2d[in];

						writeField(out, tag, sizeof(current), (char*)&current);
					}

					break;
				}

			case MVOLUME:

				int count=0;
				MPoly3D *mp3 = (MPoly3D *)(*i);
//				TRACE("Volume\n");

				for (MPoly2DVec::iterator i = mp3->poly3d.begin(); i != mp3->poly3d.end(); i++)
				{
//					TRACE("C%1d: norm:(%.2f, %.2f %.2f), depth: %.2f Vertices: ", count+1, i->planeNormal.x, i->planeNormal.y, i->planeNormal.z, i->planeD);
					tag.element = MEASURE_PLANE_NORMAL; 
					writeField(out, tag, sizeof(i->planeNormal), (char*)&(i->planeNormal));
					tag.element = MEASURE_PLANE_DEPTH; 
					writeField(out, tag, sizeof(i->planeD), (char*)&(i->planeD));
					saveViewsToFile(out, i->state);
					tag.element = MEASURE_VEC3; 
					
					for (long in = 0; in < i->poly2d.size(); in++)
					{
						Vector3 vertex = i->poly2d[in];
						writeField(out, tag, sizeof(vertex), (char*)&vertex);
					}
				 
					tag.element = MEASURE_END_CONTOUR;
					writeField(out, tag, 0);
					count++;
//					TRACE("\n");
				}
				break;
		} 

		tag.element = MEASURE_END_OBJECT;
		writeField(out, tag, 0);

	}

	return 0;
}


//----------------------------------------------------------------------
// LoadMeasurements
//
// Read in 2D or 3D measurements from ldd file
//
// Input:
// path_name is the name of the input file
// char instanceName contains "m_measurements" to open 2D measurements or 
// "m_contours" to open 3D measurements
//
// Output:
// the size of the measurement list after reading all saved measurements
//----------------------------------------------------------------------
long CMeasure::LoadMeasurements(const char *path_name, const char *instance_name)
{
	MObj  *newobj = 0;
	Face face;
	MPoly2D area;
	mvState view;
	Vector3 vec;
	Vector3Vec v_array;
	short status=0;
	unsigned short obj_type;
	long length;
	DTag tag;
	char buf[256];
	bool old = false;

	bool instance_found = false;
	area.type = MAREA;

	assert(sizeof(DTag) == 4);

	// Since this is a read ony file, the open will fail if there is not one to read...
	// the 'ios::nocreate' is redundant, and is no longer part of the standard library
	// for that reason.
	STD::ifstream in(path_name, STD::ios::in | STD::ios::binary /*| ios::nocreate*/ ); 

	if (in.good()) 
	{
		in.read((char *)&tag, 4); in.read((char *)&length, 4); in.read(buf, length);

		if (tag.group != SVF_TYPE || tag.element != SVE_TYPE || in.eof()) // should probably throw an exception here
		{
			return m_measureList.size();
		}

		while (!in.eof()) 
		{
			in.read((char *)&tag, 4); in.read((char *)&length, 4);
			if (in.eof()) break;

			switch (tag.group) 
			{
				case MEASURE_DATA:
					if ((tag.element == MEASURE_INSTANCE ) || instance_found && (tag.element == MEASURE_TYPE || newobj))
					{
						switch (tag.element)
						{
							case MEASURE_INSTANCE:           
								instance_found = false;
								assert(length <255); in.read(buf, length); buf[length] = 0;
							//	TRACE("Inst name: %s\n", buf);
								if (!instance_name || strncmp(instance_name, buf, strlen(instance_name)) == 0)
								{
									instance_found = true;
								}
								break;

							case MEASURE_TYPE:	
								// cleanup structures
								area.poly2d.erase(area.poly2d.begin(), area.poly2d.end());
								view.model.erase(view.model.begin(), view.model.end());
								status = 0;
								newobj = 0;
							
								assert(length == sizeof(unsigned short)); in.read((char *)&obj_type, length); 
						
								switch (obj_type)
								{
									case MPOINT:  newobj = new MPoint; break;
									case MLINE:   newobj = new MLine; break;
									case MANGLE:  newobj = new MAngle; break;
									case MAREA:   newobj = new MPoly2D; break;
									case MVOLUME: newobj = new MPoly3D; break;
								}
							
								if (newobj)
								{
									newobj->type = obj_type;
									newobj->subtype = newobj->attrib = 0;
									m_measureList.push_back(newobj);
								}
								break;

							case MEASURE_NAME:
								assert(length <= sizeof(newobj->name));
								in.read((char *)&newobj->name, length);
								break;

							case MEASURE_ID:
								assert(length == sizeof(newobj->id));
								in.read((char *)&newobj->id, length);
								break;

							case MEASURE_SUBTYPE:
								assert(length == sizeof(newobj->subtype));
								in.read((char *)&newobj->subtype, length);
								break;

							case MEASURE_ATTRIB:	
								assert(length == sizeof(newobj->attrib));
								in.read((char *)&newobj->attrib, length);

								if(newobj->attrib & MATTR_NEW)
								{
									old = false;
								}
								else
								{
									old = true;
									newobj->attrib = newobj->attrib | MATTR_NEW;
								}
								break;

							case MEASURE_VA_FIRST:
								//	assert(length == sizeof(newobj->axis_first));
								in.read((char *)&newobj->axis_first, length);
								newobj->axis_first.a = false;
								break;

							case MEASURE_VA_SECOND:
								//	assert(length == sizeof(newobj->axis_second));
								in.read((char *)&newobj->axis_second, length);
								newobj->axis_second.a = false;
								break;

							case MEASURE_PARAM:
								assert(length <= sizeof(newobj->param));
								in.read((char *)&newobj->param, length);
								break;

							case MEASURE_CUSTOM:
								assert(length <= sizeof(newobj->custom));
								in.read((char *)&newobj->custom, length);
								break;

							case MEASURE_END_OBJECT: 
								assert(length == 0);
								switch (newobj->type)
								{
									case MPOINT:
										m_pointID = getMax(newobj->id + 1, m_pointID);
										((MPoint*)newobj)->state = view;
										break;

									case MLINE:
										assert(v_array.size() == 2);
										((MLine*)newobj)->start = v_array[0];
										((MLine*)newobj)->end = v_array[1];
										((MLine*)newobj)->state = view;
										m_lineID = getMax(newobj->id + 1, m_lineID);
										break;

									case MANGLE:
										assert(v_array.size() == 3);
										((MAngle*)newobj)->start = v_array[0];
										((MAngle*)newobj)->middle = v_array[1];
										((MAngle*)newobj)->end = v_array[2];
										((MAngle*)newobj)->state = view;
										m_angleID = getMax(newobj->id + 1, m_angleID);
										break;

									case MAREA:
										((MPoly2D*)newobj)->poly2d = v_array;
										((MPoly2D*)newobj)->state = view;
										((MPoly2D*)newobj)->planeNormal = area.planeNormal;
										((MPoly2D*)newobj)->planeD = area.planeD;

										polyParam((MPoly2D*)newobj);
										m_areaID = getMax(newobj->id + 1, m_areaID);
										break;

									case MVOLUME:
										calculateEnclosedCube(((MPoly3D *)newobj));
										m_volumeID = getMax(newobj->id + 1, m_volumeID);
										break;
								}

								face.vert.clear();
								v_array.clear();
								view.model.clear();
								area.poly2d.clear();
								newobj = 0;
								//TRACE("\n");
								break;

							case MEASURE_VEC3:
								//	assert(length == sizeof(vec));
								in.read((char *)&vec, length);
								vec.a = false;
								v_array.push_back(vec);
								//TRACE(", (%.2f, %.2f, %.2f)", vec.x, vec.y, vec.z);
								break;

							case MEASURE_PLANE_NORMAL:
								assert(length == sizeof(area.planeNormal));
								in.read((char *)&area.planeNormal, length);
								//TRACE(" norm:[%.2f, %.2f, %.2f]", area.planeNormal.x, area.planeNormal.y, area.planeNormal.z);
								break;

							case MEASURE_PLANE_DEPTH:
								assert(length == sizeof(area.planeD));
								in.read((char *)&area.planeD, length);
								//TRACE(" depth: %.2f", area.planeD);
								break;

							case MEASURE_END_CONTOUR:
								if (newobj->type == MVOLUME)
								{
									area.state = view;
									area.poly2d = v_array;
									area.id = newobj->id;
									polyParam(&area);
									((MPoly3D*)newobj)->poly3d.push_back(area);
								}
								//TRACE(" - %1d verts, C:%1d\n", area.poly2d.size(), ((MPoly3D*)newobj)->poly3d.size());
								area.poly2d.clear();
								view.model.clear();
								v_array.clear();
								break;

							case SVE_NAME:
								assert(length <256); in.read(buf, length); 
								memcpy(&view.name, buf, sizeof(view.name));
								break;

							case SVE_VIEW2MODEL:
								assert(length == sizeof(view.View2ModelRot));
								in.read((char *)&view.View2ModelRot, length);
								break;

							case SVE_MODEL2VIEW:
								assert(length == sizeof(view.Model2ViewRot));
								in.read((char *)&view.Model2ViewRot, length);
								break;

							case SVE_SCALE:
								assert(length == sizeof(view.scaleAbs));
								in.read((char *)&view.scaleAbs, length);
								break;

							case SVE_SHIFT:
								assert(length == sizeof(view.modelShift));
								in.read((char *)&view.modelShift, length);
								break;

							case SVE_FACEPLANE:
								if(old)
								{
									struct Vector3Old
									{
										float x,y,z;
									};	
									struct PlaneOld
									{
										Vector3Old normal;	
										float D;		
									};
									PlaneOld oldplane;

									assert(length == sizeof(PlaneOld));
									assert(face.vert.empty());
									in.read((char *)&oldplane, length);
									face.plane.normal.x = oldplane.normal.x;
									face.plane.normal.y = oldplane.normal.y;
									face.plane.normal.z = oldplane.normal.z;
									face.plane.normal.a = 0;
									face.plane.D =  oldplane.D;		
								}
								else
								{
									assert(length == sizeof(face.plane));
									assert(face.vert.empty());
									in.read((char *)&face.plane, length);
								}
								break;

							case SVE_FACEVERTEX:
								//	assert(length == sizeof(vec));
								in.read((char *)&vec, length);
								vec.a = false; 
								face.vert.push_back(vec);
								break;

							case SVE_ENDFACE:
								assert(length == 0);
								view.model.push_back(face);
								face.vert.erase(face.vert.begin(), face.vert.end());
								break;

							default: // discard unknown tags
								in.seekg(length, STD::ios::cur);
								break;
						}
					} 
					else // unknown type. Discard it.
					{
						in.seekg(length, STD::ios::cur);
					}
					break; // case MEASURE_DATA

				default: // discard unknown tags
					in.seekg(length, STD::ios::cur);
					break;
			}
		} 
	} 

	in.close();

	return m_measureList.size();
}


//----------------------------------------------------------------------
// saveViewsToFile
//
// Save the saved views list to ldd file
//
// Input:
// opened ostream object to write to
// mvState structure contains the visualizer state to save together with
// views
//
// Output:
// none
//----------------------------------------------------------------------
void CMeasure::saveViewsToFile(STD::ostream& out, const mvState& state)
{
	DTag tag;
	tag.group = MEASURE_DATA;

	tag.element = SVE_NAME; 
	writeField(out, tag, sizeof(state.name), (char*)&(state.name));
		
	tag.element = SVE_VIEW2MODEL; 
	writeField(out, tag, sizeof(state.View2ModelRot), (char*)&(state.View2ModelRot));

	tag.element = SVE_MODEL2VIEW; 
	writeField(out, tag, sizeof(state.Model2ViewRot), (char*)&(state.Model2ViewRot));

	tag.element = SVE_SCALE; 
	writeField(out, tag, sizeof(state.scaleAbs), (char*)&(state.scaleAbs));

	tag.element = SVE_SHIFT; 
	writeField(out, tag, sizeof(state.modelShift), (char*)&(state.modelShift));

	for(FaceVec::const_iterator jFace = state.model.begin(); jFace != state.model.end(); jFace++)
	{
		tag.element = SVE_FACEPLANE;  
		writeField(out, tag, sizeof(jFace->plane), (char*)&(jFace->plane));

		Vector3Vec vertices = jFace->vert;
		for (long k = 0; k < jFace->vert.size(); k++)
		{
			tag.element = SVE_FACEVERTEX;

			Vector3 vertex = vertices[k];
			writeField(out, tag, sizeof(vertex), (char*)&vertex);
		}

		tag.element = SVE_ENDFACE;
		writeField(out, tag, 0);
	}

	tag.element = SVE_ENDVIEW;
	writeField(out, tag, 0);
}
