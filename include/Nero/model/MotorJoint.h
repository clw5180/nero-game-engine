////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2019 SANOU A. K. Landry
////////////////////////////////////////////////////////////
#ifndef MOTORJOINT_H
#define MOTORJOINT_H
///////////////////////////HEADERS//////////////////////////
//NERO
#include <Nero/model/PhysicJoint.h>
//BOX2D
#include <Box2D/Dynamics/Joints/b2MotorJoint.h>
////////////////////////////////////////////////////////////
namespace nero
{
    class MotorJoint : public PhysicJoint
    {
         public:
            typedef std::shared_ptr<MotorJoint>     Ptr;
            static  Ptr                             Cast(PhysicJoint::Ptr joint);

                                    MotorJoint();
            virtual                ~MotorJoint();

            void                    setJoint(b2MotorJoint* joint);
            b2MotorJoint*           getJoint() const;
            virtual b2Joint*        getGenericJoint();

        private:
            b2MotorJoint*           m_Joint;
    };
}
#endif // MOTORJOINT_H
