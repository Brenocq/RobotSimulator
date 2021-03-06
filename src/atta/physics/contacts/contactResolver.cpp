//--------------------------------------------------
// Atta Physics
// contactResolver.cpp
// Date: 2020-12-05
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <atta/physics/contacts/contactResolver.h>
#include <limits>
#include <atta/helpers/log.h>

namespace atta::phy
{
	ContactResolver::ContactResolver():
		_positionIterations(10), _positionEpsilon(0.001f),
		_velocityIterations(10), _velocityEpsilon(0.001f)
	{

	}

	ContactResolver::~ContactResolver()
	{

	}

	void ContactResolver::resolveContacts(std::vector<Contact> &contacts, float dt)
	{
		if(contacts.size()==0) return;

		// Prepare the contacts for processing
		prepareContacts(contacts, dt);

		// Resolve bodies interpenetrations from contacts
		adjustPositions(contacts, dt);

		// Resolve bodies velocities from contacts
		//adjustVelocities(contacts, dt);
	}

	void ContactResolver::prepareContacts(std::vector<Contact> &contacts, float dt)
	{
		for(auto& contact : contacts)
		{
			// Calculate the internal contact data (inertia, basis, etc)
			contact.calculateInternals(dt);
		}
	}

	void ContactResolver::adjustPositions(std::vector<Contact> &contacts, float dt)
	{
		unsigned i,index;
		vec3 linearChange[2], angularChange[2];
		float max;
		vec3 deltaPosition;
		unsigned numContacts = contacts.size();

		// Iteratively resolve interpenetrations in order of severity
		unsigned positionIterationsUsed = 0;
		while(positionIterationsUsed < _positionIterations)
		{
			//----- Find the greatest penetration -----//
			max = _positionEpsilon;
			index = numContacts;
			for(i=0; i<numContacts; i++)
			{
				if(contacts[i].penetration > max)
				{
					max = contacts[i].penetration;
					index = i;
				}
			}
			if(index == numContacts) break;
			Log::debug("ContactResolver", "maxPen:$0", contacts[index].penetration);

			// Match the awake state at the contact
			contacts[index].matchAwakeState();

			//----- Resolve the penetration -----//
			contacts[index].applyPositionChange(
				linearChange,
				angularChange,
				max);
			Log::debug("ContactResolver", "linChange:$0\tangChange:$1", linearChange[0].toString(), angularChange[0].toString());

			//----- Update contacts for other bodies -----//
			for(i = 0; i < numContacts; i++)
			{
				// Check each body in the contact
				for(unsigned b = 0; b < 2; b++) if(contacts[i].bodies[b])
				{
					// Check for a match with each body in the newly
					// resolved contact
					for(unsigned d = 0; d < 2; d++)
					{
						if(contacts[i].bodies[b] == contacts[index].bodies[d])
						{
							// Calculate contact point velocity
							deltaPosition = linearChange[d] + cross(angularChange[d], contacts[i].relativeContactPosition[b]);

							// The sign of the change is positive if we're
							// dealing with the second body in a contact
							// and negative otherwise (because we're
							// subtracting the resolution)
							contacts[i].penetration += dot(deltaPosition, contacts[i].contactNormal) * (b?1:-1);
						}
					}
				}
			}
			positionIterationsUsed++;
		}

		if(positionIterationsUsed>=2)
			exit(0);
	}

	void ContactResolver::adjustVelocities(std::vector<Contact> &contacts, float dt)
	{
		vec3 velocityChange[2], rotationChange[2];
		vec3 deltaVel;
		unsigned numContacts = contacts.size();

		// Iteratively handle impacts in order of severity
		unsigned velocityIterationsUsed = 0;
		while(velocityIterationsUsed < _velocityIterations)
		{
			// Find contact with maximum magnitude of probable velocity change.
			float max = _velocityEpsilon;
			unsigned index = numContacts;
			for (unsigned i = 0; i < numContacts; i++)
			{
				if (contacts[i].desiredDeltaVelocity > max)
				{
					max = contacts[i].desiredDeltaVelocity;
					index = i;
				}
			}
			if (index == numContacts) break;

			// Match the awake state at the contact
			contacts[index].matchAwakeState();

			// Do the resolution on the contact that came out top.
			contacts[index].applyVelocityChange(velocityChange, rotationChange);

			// With the change in velocity of the two bodies, the update of
			// contact velocities means that some of the relative closing
			// velocities need recomputing.
			for (unsigned i = 0; i < numContacts; i++)
			{
				// Check each body in the contact
				for (unsigned b = 0; b < 2; b++) if (contacts[i].bodies[b])
				{
					// Check for a match with each body in the newly
					// resolved contact
					for (unsigned d = 0; d < 2; d++)
					{
						if (contacts[i].bodies[b] == contacts[index].bodies[d])
						{
							deltaVel = velocityChange[d] + cross(rotationChange[d], contacts[i].relativeContactPosition[b]);

							// The sign of the change is negative if we're dealing
							// with the second body in a contact.
							contacts[i].contactVelocity += contacts[i].contactToWorld.transformTranspose(deltaVel) * (b?-1:1);
							contacts[i].calculateDesiredDeltaVelocity(dt);
						}
					}
				}
			}
			velocityIterationsUsed++;
		}
	}
}
