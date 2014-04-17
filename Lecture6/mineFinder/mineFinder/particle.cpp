
#include "stdafx.h"
#include "particle.h"
#include <set>

float randFloat()
{
	int x = rand() % 1000;
	x -= 500;
	float xx = (float)x / 500.f; 
	return xx;
}

cParticle::cParticle(const POINT &pos)
{
	//       (-30,-30)  ----------------- (+30, -30)
	//       |                                                       |
	//       |                         +                           |
	//       |                                                       |
	//       (-30,+30)  ----------------- (+30, +30)
	const float w = 15.f;
	m_vertices.push_back( Vector3(-w,-w,1) );
	m_vertices.push_back( Vector3(w,-w,1) );
	m_vertices.push_back( Vector3(w,w,1) );
	m_vertices.push_back( Vector3(-w,w,1) );
	m_vertices.push_back( Vector3(-w,-w,1) );

	m_localTm.SetIdentity();
	m_tm.SetIdentity();
	m_tm.Translate(Vector3((float)pos.x, (float)pos.y, 0));

	const float x = randFloat() * 1000.f;
	const float y = randFloat() * 1000.f;
	m_Velocity = Vector3(x,y,0);
	m_Pos = Vector3((float)pos.x, (float)pos.y, 0);

	m_Torq = randFloat() * 30.f;
}

cParticle::~cParticle()
{

}


/**
 @brief 
 @date 2014-04-17
*/
void RenderVertices(HDC hdc, const vector<Vector3> &vertices, const Matrix44 &tm)
{
	for (unsigned int i=0; i < vertices.size(); ++i)
	{
		Vector3 p = vertices[ i];
		p = p * tm;

		if (0 == i)
			MoveToEx(hdc, (int)p.x, (int)p.y, NULL);
		else
			LineTo(hdc, (int)p.x, (int)p.y);
	}
}


/**
 @brief 
 @date 2014-04-17
*/
void cParticle::Render(HDC hdc)
{
	RenderVertices(hdc, m_vertices, m_localTm*m_tm);
}


/**
 @brief 
 @date 2014-04-17
*/
bool cParticle::Move(int elapseTime)
{
	const float dt = (float)elapseTime * 0.001f;
	m_Velocity += Vector3(0, 3000.f * dt, 0);
	m_Pos += m_Velocity * dt;
	m_tm.SetWorld(m_Pos);

	Matrix44 mat;
	mat.SetRotationZ(m_Torq * dt);
	m_localTm *= mat;

	if (m_Pos.y > 1000.f)
		return false;

	return true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cParticleManager

cParticleManager::cParticleManager()
{

}

cParticleManager::~cParticleManager()
{
	for (int i=0; i < (int)m_particles.size(); ++i)
		delete m_particles[ i];
	m_particles.clear();
}


void cParticleManager::AddParticle(cParticle *partcle)
{
	m_particles.push_back(partcle);
}


void cParticleManager::Render(HDC hdc)
{
	if (m_particles.empty())
		return;

	for (int i=0; i < (int)m_particles.size(); ++i)
		m_particles[ i]->Render(hdc);
}


bool cParticleManager::Move(int elapseTime)
{
	if (m_particles.empty())
		return false;

	vector<int> rm;
	for (int i=0; i < (int)m_particles.size(); ++i)
	{
		if (!m_particles[ i]->Move(elapseTime))
			rm.push_back(i);
	}

	for (int i=rm.size()-1; i >= 0; --i)
	{
		delete m_particles[ rm[ i]];
		m_particles.erase( m_particles.begin()+rm[ i] );
	}

	return true;
}
