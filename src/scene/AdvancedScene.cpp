////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2019 SANOU A. K. Landry
////////////////////////////////////////////////////////////
///////////////////////////HEADERS//////////////////////////
#include <Nero/scene/AdvancedScene.h>
////////////////////////////////////////////////////////////
namespace nero
{
    ////////////////////////////////////////////////////////////
    //DestructionListener
    void DestructionListener::SayGoodbye(b2Fixture* fixture)
    {
        B2_NOT_USED(fixture);
    }

    void DestructionListener::SayGoodbye(b2Joint* joint)
    {
        if (scene->m_MouseJoint == joint)
        {
            scene->m_MouseJoint = NULL;
        }
        else
        {
            scene->jointDestroyed(joint);
        }
    }

    ////////////////////////////////////////////////////////////
    //AdvancedScene
    AdvancedScene::AdvancedScene(Context context):
        m_Scene(nullptr)
        ,m_DestructionListener()
        //
        ,m_Message("")
        ,m_StatMessage("")
        ,m_ProfileMessage("")
        ,m_Bomb(nullptr)
        ,m_BombSpawning(false)
        ,m_BombSpawnPoint(0.f, 0.f)
        ,m_MouseJoint(nullptr)
        ,m_GroundBody(nullptr)
        ,m_WorldAABB()
        ,m_MouseWorld(0.f, 0.f)
        ,m_StepCount(0)
        ,m_MaxProfile()
        ,m_TotalProfile()
        ,m_SceneSetting()
        ,m_CameraSetting()
        ,m_SoundSetting()
    {
        //Manager
        m_DestructionListener.scene = AdvancedScene::Ptr(this);
        m_UndoManger                = UndoManager::Ptr(new UndoManager());
        m_SceneBuilder              = SceneBuilder::Ptr(new SceneBuilder(context.renderCanvas, context.resourceManager));

        //void * memset (void * ptr, int value, size_t num);
        //Sets the first num bytes of the block of memory pointed by ptr to the specified value (interpreted as an unsigned char).
        memset(&m_MaxProfile, 0, sizeof(b2Profile));
        memset(&m_TotalProfile, 0, sizeof(b2Profile));

        //Add the default layer
        m_SceneBuilder->addLayer();
    }

    ////////////////////////////////////////////////////////////
    AdvancedScene::~AdvancedScene()
    {

    }

    ////////////////////////////////////////////////////////////
    void AdvancedScene::init()
    {
        m_Scene->m_PhysicWorld->SetDestructionListener(&m_DestructionListener);

        //Setup the ground
        b2BodyDef bodyDef;
        m_GroundBody = m_Scene->m_PhysicWorld->CreateBody(&bodyDef);
    }

    ////////////////////////////////////////////////////////////
    void AdvancedScene::setScene(Scene::Ptr scene)
    {
        m_Scene = scene;
    }

    ////////////////////////////////////////////////////////////
    SceneBuilder::Ptr AdvancedScene::getSceneBuilder()
    {
        return m_SceneBuilder;
    }

    ////////////////////////////////////////////////////////////
    UndoManager::Ptr AdvancedScene::getUndoManager()
    {
        return m_UndoManger;
    }

    ////////////////////////////////////////////////////////////
    void AdvancedScene::setName(std::string name)
    {
        m_SceneName = name;
    }

    ////////////////////////////////////////////////////////////
    void AdvancedScene::update(const sf::Time& timeStep)
    {
        if(m_Scene)
        {
            m_Scene->m_SceneSetting = m_SceneSetting;
            m_Scene->m_PhysicWorld->SetGravity(m_SceneSetting.gravity);
            m_Scene->update(timeStep);

            if(m_SceneSetting.pause && m_SceneSetting.singleStep)
                m_SceneSetting.singleStep = false;
        }

        float32 b2TimeStep = m_SceneSetting.hz > 0.0f ? 1.0f / m_SceneSetting.hz : float32(0.0f);

        if (b2TimeStep > 0.0f)
        {
            ++m_StepCount;
        }

        m_Message = m_StatMessage + m_ProfileMessage + m_Scene->m_PauseMessage;
        m_Scene->m_Text.setString(m_Message);
    }

    ////////////////////////////////////////////////////////////
    void AdvancedScene::renderDebug()
    {
        if (m_SceneSetting.drawStats)
        {
            //bodies/contacts/joints
            int32 bodyCount     = m_Scene->m_PhysicWorld->GetBodyCount();
            int32 contactCount  = m_Scene->m_PhysicWorld->GetContactCount();
            int32 jointCount    = m_Scene->m_PhysicWorld->GetJointCount();

            //proxies/height/balance/quality
            int32 proxyCount    = m_Scene->m_PhysicWorld->GetProxyCount();
            int32 height        = m_Scene->m_PhysicWorld->GetTreeHeight();
            int32 balance       = m_Scene->m_PhysicWorld->GetTreeBalance();
            float32 quality     = m_Scene->m_PhysicWorld->GetTreeQuality();

            m_StatMessage = "body/contact/joint = " + toString(bodyCount) + " / " +  toString(contactCount) + " / " + toString(jointCount) + "\n" \
                            "proxy/height/balance/quality = " + toString(proxyCount) + " / " + toString(height) + " / " +  toString(balance) + " / " + toString(quality) + "\n";
        }
        else
        {
            m_StatMessage = "";
        }

        // Track maximum profile times
        {
            const b2Profile& p = m_Scene->m_PhysicWorld->GetProfile();

            m_MaxProfile.step               = b2Max(m_MaxProfile.step, p.step);
            m_MaxProfile.collide            = b2Max(m_MaxProfile.collide, p.collide);
            m_MaxProfile.solve              = b2Max(m_MaxProfile.solve, p.solve);
            m_MaxProfile.solveInit          = b2Max(m_MaxProfile.solveInit, p.solveInit);
            m_MaxProfile.solveVelocity      = b2Max(m_MaxProfile.solveVelocity, p.solveVelocity);
            m_MaxProfile.solvePosition      = b2Max(m_MaxProfile.solvePosition, p.solvePosition);
            m_MaxProfile.solveTOI           = b2Max(m_MaxProfile.solveTOI, p.solveTOI);
            m_MaxProfile.broadphase         = b2Max(m_MaxProfile.broadphase, p.broadphase);

            m_TotalProfile.step             += p.step;
            m_TotalProfile.collide          += p.collide;
            m_TotalProfile.solve            += p.solve;
            m_TotalProfile.solveInit        += p.solveInit;
            m_TotalProfile.solveVelocity    += p.solveVelocity;
            m_TotalProfile.solvePosition    += p.solvePosition;
            m_TotalProfile.solveTOI         += p.solveTOI;
            m_TotalProfile.broadphase       += p.broadphase;
        }

        if (m_SceneSetting.drawProfile)
        {
            const b2Profile& p = m_Scene->m_PhysicWorld->GetProfile();

            b2Profile aveProfile;
            memset(&aveProfile, 0, sizeof(b2Profile));

            if (m_StepCount > 0)
            {
                float32 scale = 1.0f / m_StepCount;

                aveProfile.step             = scale * m_TotalProfile.step;
                aveProfile.collide          = scale * m_TotalProfile.collide;
                aveProfile.solve            = scale * m_TotalProfile.solve;
                aveProfile.solveInit        = scale * m_TotalProfile.solveInit;
                aveProfile.solveVelocity    = scale * m_TotalProfile.solveVelocity;
                aveProfile.solvePosition    = scale * m_TotalProfile.solvePosition;
                aveProfile.solveTOI         = scale * m_TotalProfile.solveTOI;
                aveProfile.broadphase       = scale * m_TotalProfile.broadphase;
            }

            m_ProfileMessage =  "step [ave] (max) = "           + toString(p.step)          + " [" + toString(aveProfile.step)          + "]" +  "(" + toString(m_MaxProfile.step)          + ") " + "\n" \
                                "collide [ave] (max) = "        + toString(p.collide)       + " [" + toString(aveProfile.collide)       + "]" +  "(" + toString(m_MaxProfile.collide)       + ")" + "\n" \
                                "solve [ave] (max) = "          + toString(p.solve)         + " [" + toString(aveProfile.solve)         + "]" +  "(" + toString(m_MaxProfile.solve)         + ")" + "\n" \
                                "solve init [ave] (max) = "     + toString(p.solveInit)     + " [" + toString(aveProfile.solveInit)     + "]" +  "(" + toString(m_MaxProfile.solveInit)     + ")" + "\n" \
                                "solve velocity [ave] (max) = " + toString(p.solveVelocity) + " [" + toString(aveProfile.solveVelocity) + "]" +  "(" + toString(m_MaxProfile.solveVelocity) + ")" + "\n" \
                                "solve position [ave] (max) = " + toString(p.solvePosition) + " [" + toString(aveProfile.solvePosition) + "]" +  "(" + toString(m_MaxProfile.solvePosition) + ")" + "\n" \
                                "solveTOI [ave] (max) = "       + toString(p.solveTOI)      + " [" + toString(aveProfile.solveTOI)      + "]" +  "(" + toString(m_MaxProfile.solveTOI)      + ")" + "\n" \
                                "broad-phase [ave] (max) = "    + toString(p.broadphase)    + " [" + toString(aveProfile.broadphase)    + "]" +  "(" + toString(m_MaxProfile.broadphase)    + ")" + "\n";
        }
        else
        {
            m_ProfileMessage = "";
        }


        if (m_MouseJoint)
        {
            b2Vec2 p1 = m_MouseJoint->GetAnchorB();
            b2Vec2 p2 = m_MouseJoint->GetTarget();

            b2Color c;
            c.Set(0.0f, 1.0f, 0.0f);
            m_Scene->m_ShapeRenderer.DrawPoint(p1, 4.0f, c);
            m_Scene->m_ShapeRenderer.DrawPoint(p2, 4.0f, c);

            c.Set(0.8f, 0.8f, 0.8f);
            m_Scene->m_ShapeRenderer.DrawSegment(p1, p2, c);
        }

        if (m_BombSpawning)
        {
            b2Color c;
            c.Set(0.0f, 0.0f, 1.0f);
            m_Scene->m_ShapeRenderer.DrawPoint(m_BombSpawnPoint, 4.0f, c);

            c.Set(0.8f, 0.8f, 0.8f);
            m_Scene->m_ShapeRenderer.DrawSegment(m_MouseWorld, m_BombSpawnPoint, c);
        }

        if (m_SceneSetting.drawContactPoints)
        {
            const float32 k_impulseScale = 0.1f;
            const float32 k_axisScale = 0.3f;

            for (int32 i = 0; i < m_Scene->m_PointCount; ++i)
            {
                ContactPoint* point = m_Scene->m_Points + i;

                if (point->state == b2_addState)
                {
                    // Add
                    m_Scene->m_ShapeRenderer.DrawPoint(point->position, 10.0f, b2Color(0.3f, 0.95f, 0.3f));
                }
                else if (point->state == b2_persistState)
                {
                    // Persist
                    m_Scene->m_ShapeRenderer.DrawPoint(point->position, 5.0f, b2Color(0.3f, 0.3f, 0.95f));
                }

                if (m_SceneSetting.drawContactNormals == 1)
                {
                    b2Vec2 p1 = point->position;
                    b2Vec2 p2 = p1 + k_axisScale * point->normal;
                    m_Scene->m_ShapeRenderer.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.9f));
                }
                else if (m_SceneSetting.drawContactImpulse == 1)
                {
                    b2Vec2 p1 = point->position;
                    b2Vec2 p2 = p1 + k_impulseScale * point->normalImpulse * point->normal;
                    m_Scene->m_ShapeRenderer.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
                }

                if (m_SceneSetting.drawFrictionImpulse == 1)
                {
                    b2Vec2 tangent = b2Cross(point->normal, 1.0f);
                    b2Vec2 p1 = point->position;
                    b2Vec2 p2 = p1 + k_impulseScale * point->tangentImpulse * tangent;
                    m_Scene->m_ShapeRenderer.DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////
    void AdvancedScene::shiftOrigin(const b2Vec2& newOrigin)
    {
        m_Scene->m_PhysicWorld->ShiftOrigin(newOrigin);
    }
     ////////////////////////////////////////////////////////////
    void AdvancedScene::shiftMouseDown(const b2Vec2& p)
    {
        m_MouseWorld = p;

        if (m_MouseJoint != NULL)
        {
            return;
        }

        spawnBomb(p);
    }

    ////////////////////////////////////////////////////////////
    void AdvancedScene::mouseDown(const b2Vec2& p)
    {
        m_MouseWorld = p;

        if (m_MouseJoint != NULL)
        {
            return;
        }

        // Make a small box.
        b2AABB aabb;
        b2Vec2 d;
        d.Set(0.001f, 0.001f);
        aabb.lowerBound = p - d;
        aabb.upperBound = p + d;

        // Query the world for overlapping shapes.
        QueryCallback callback(p);
        m_Scene->m_PhysicWorld->QueryAABB(&callback, aabb);

        if (callback.m_Fixture)
        {
            b2Body* body = callback.m_Fixture->GetBody();
            b2MouseJointDef md;
            md.bodyA = m_GroundBody;
            md.bodyB = body;
            md.target = p;
            md.maxForce = 1000.0f * body->GetMass();
            m_MouseJoint = (b2MouseJoint*)m_Scene->m_PhysicWorld->CreateJoint(&md);
            body->SetAwake(true);
        }
    }

    ////////////////////////////////////////////////////////////
    void AdvancedScene::mouseUp(const b2Vec2& p)
    {
        if (m_MouseJoint)
        {
            m_Scene->m_PhysicWorld->DestroyJoint(m_MouseJoint);
            m_MouseJoint = NULL;
        }

        if (m_BombSpawning)
        {
            completeBombSpawn(p);
        }
    }

    ////////////////////////////////////////////////////////////
    void AdvancedScene::mouseMove(const b2Vec2& p)
    {
        m_MouseWorld = p;

        if (m_MouseJoint)
        {
            m_MouseJoint->SetTarget(p);
        }
    }

    ////////////////////////////////////////////////////////////
    void AdvancedScene::spawnBomb(const b2Vec2& worldPt)
    {
        m_BombSpawnPoint = worldPt;
        m_BombSpawning = true;
    }

    ////////////////////////////////////////////////////////////
    void AdvancedScene::completeBombSpawn(const b2Vec2& p)
    {
        if (m_BombSpawning == false)
        {
            return;
        }

        const float multiplier = 30.0f;
        b2Vec2 vel = m_BombSpawnPoint - p;
        vel *= multiplier;
        launchBomb(m_BombSpawnPoint,vel);
        m_BombSpawning = false;
    }

    ////////////////////////////////////////////////////////////
    void AdvancedScene::launchBomb()
    {
        b2Vec2 p(randomFloat(-15.0f, 15.0f), -30.0f);
        b2Vec2 v = -5.0f * p;
        launchBomb(p, v);
    }

    ////////////////////////////////////////////////////////////
    void AdvancedScene::launchBomb(const b2Vec2& position, const b2Vec2& velocity)
    {
        if (m_Bomb)
        {
            m_Scene->m_PhysicWorld->DestroyBody(m_Bomb);
            m_Bomb = NULL;
        }

        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position = position;
        bd.bullet = true;
        m_Bomb = m_Scene->m_PhysicWorld->CreateBody(&bd);
        m_Bomb->SetLinearVelocity(velocity);

        b2CircleShape circle;
        circle.m_radius = 0.3f;

        b2FixtureDef fd;
        fd.shape = &circle;
        fd.density = 20.0f;
        fd.restitution = 0.0f;

        b2Vec2 minV = position - b2Vec2(0.3f,0.3f);
        b2Vec2 maxV = position + b2Vec2(0.3f,0.3f);

        b2AABB aabb;
        aabb.lowerBound = minV;
        aabb.upperBound = maxV;

        m_Bomb->CreateFixture(&fd);
    }

    ////////////////////////////////////////////////////////////
    void AdvancedScene::jointDestroyed(b2Joint* joint)
    {
        B2_NOT_USED(joint);
    }
}
