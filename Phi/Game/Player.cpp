#include <PreCompile.h>
#include <Game/Level.h>
#include <Game/Player.h>
#include <Render/AABB.h>
#include <Utils/Utils.h>

namespace Game
{
    
    void Player::initialize(Initialize &initialize)
    {
        Render::AABB aabb;

        aabb.reset();
        aabb.add(glm::vec3(0.f, 0.f, 0.f));
        aabb.add(glm::vec3(2000.f, 2000.f, 2000.f));
        
        std::vector<float> vs;
        
        Utils::GenerateCube(aabb, vs);
        Utils::GenerateNormals(&vs[0], 6, vs.size() / 6, 0, 3);
        
        mRenderable.vertexBuffer.create(GL_STATIC_DRAW, vs.size() * sizeof(float));
        mRenderable.vertexBuffer.update(&vs[0], 0, vs.size() * sizeof(float));
        mRenderable.count = vs.size() / 6;
        Level* level = (Level*) initialize.level;
        
        mPhysic.position  = level->getPosition(0.f)
        + level->getNormal(0.f) * (level->getRadius(0.f) * 1.2f);
        mPhysic.direction = level->getPosition(level->smallestDelta())
        + level->getNormal(level->smallestDelta()) * (level->getRadius(level->smallestDelta()) * 1.2f) - mPhysic.position;
        
        mPhysic.sideBoostForce = 0.f;
        mPhysic.sideBoostVelocity = glm::vec3(0.f);
        
        mPhysic.gravityForce = 0.f;
        mPhysic.gravityVelocity = glm::vec3(0.f);
        
        mPhysic.normal = glm::vec3(0.f, 0.f, 1.f);
        
        mPhysic.boosterVelocity = glm::vec3(0.f);
        mPhysic.boosterForce = 0.f;
    }
    
    void Player::destroy(Destroy &destroy)
    {
        
    }
    
    void Player::setShader(Render::IShaderDirectionalLightNoTex *shader)
    {
        mShader = shader;
        mRenderable.vertexArray.destroy();
        mRenderable.vertexArray.create();
        {
            Engine::VertexArray::Binder     bind1(mRenderable.vertexArray);
            Engine::VertexBuffer::Binder    bind2(mRenderable.vertexBuffer);
            
            mRenderable.vertexArray.attrib(shader->getVsPosition(), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
            mRenderable.vertexArray.attrib(shader->getVsNormal(), 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), 3 * sizeof(float));
        }
    }

    
    void Player::render(RenderArg &render)
    {
        if (render.passElement == Engine::Solid)
        {
            Render::Material material;
            material.MaterialAmbient = glm::vec4(0.2f);
            material.MaterialDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            material.MaterialSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            material.MaterialShininess = 64.f;
            
            mShader->bind();
            mShader->setMaterial(material);
            mShader->setLightDirection(render.sunDirection);

            mShader->setMatrices(render.projection, render.view, mMatrix);

            Engine::VertexArray::Binder  bind1(mRenderable.vertexArray);
            glCullFace(GL_FRONT);
            glDrawArrays(GL_TRIANGLES, 0, mRenderable.count);
        }
    }
    
    void Player::update(Update& update)
    {
        if (update.level == NULL)
            return;
        update.player = this;
        Level* level = (Level*) update.level;
        float deltaTime = update.delta;
        
        mPhysic.boosterVelocity += mPhysic.direction *  (mPhysic.boosterForce) * deltaTime;
        mPhysic.boosterVelocity *= 0.98f;
        
        mPhysic.sideBoostVelocity += mPhysic.right * mPhysic.sideBoostForce * deltaTime;
        mPhysic.sideBoostVelocity *= 0.98f;
        
        mPhysic.gravityVelocity += mPhysic.gravity * mPhysic.gravityForce * deltaTime;
        mPhysic.gravityVelocity *= 0.98f;
        glm::vec3 previous = mPhysic.position;
        mPhysic.position = mPhysic.position + (mPhysic.boosterVelocity + mPhysic.sideBoostVelocity + mPhysic.gravityVelocity) * deltaTime;
        float deltaOnSpline = mPreviousDeltaOnSpline;
        
        level->findNearestDelta(mPhysic.position, deltaOnSpline, 1);
        if (deltaOnSpline > 1.f)
        {
            deltaOnSpline -= 1.f;
            level->findNearestDelta(mPhysic.position, deltaOnSpline, 1);
            // float deltaToStart = 1.f - mPreviousDeltaOnSpline;
            // float deltaTotal = deltaTime + deltaToStart;
            // float percent = deltaToStart / deltaTotal;
            // double lapTime = currentTime + (time - currentTime) * percent;
            // printf("(%f -> %f) !!! LAP %0.4lf\n", mPreviousDeltaOnSpline, deltaOnSpline, lapTime - lapStartTime);
            // lapStartTime = lapTime;
        }
        
        // time = currentTime;
        
        mPreviousDeltaOnSpline = deltaOnSpline;
        glm::vec3	pointOnSpline = level->getPosition(deltaOnSpline);
        
        mPhysic.direction = glm::normalize(mPhysic.direction + glm::normalize(level->getPosition(deltaOnSpline + level->mSmallestDelta * 2.f) - pointOnSpline));
        float radius = level->getRadius(deltaOnSpline);
        
        glm::vec3 vecToPoint = glm::normalize(mPhysic.position - pointOnSpline);
        glm::vec3 collisionPoint	= vecToPoint * radius * 1.2f + pointOnSpline;
        glm::vec3 hittingPoint		= vecToPoint * radius + pointOnSpline;
        float distancePlayerToSpline = glm::distance(pointOnSpline, mPhysic.position);
        float distanceTrackToSpline = glm::distance(pointOnSpline, hittingPoint);
        
        mPhysic.gravityForce = glm::distance(collisionPoint, mPhysic.position);
        if (mPhysic.gravityForce > std::numeric_limits<float>::epsilon())
            mPhysic.gravity = glm::normalize(collisionPoint - mPhysic.position);
        else
            mPhysic.gravity = glm::vec3(0);
        
        mPhysic.gravityForce *= 12.f;
        if (distanceTrackToSpline > distancePlayerToSpline)
            mPhysic.position = hittingPoint;
        
        mPhysic.normal = glm::normalize(vecToPoint);
        mPhysic.right = glm::normalize(glm::cross(mPhysic.direction, mPhysic.normal));
        
        if (glfwGetKey(update.window, GLFW_KEY_UP) == GLFW_PRESS)
            mPhysic.boosterForce = glm::clamp(mPhysic.boosterForce + 10.f, 0.f, 3000.f);
        else
            mPhysic.boosterForce = 0;
        
        bool stopForce = true;
        if (glfwGetKey(update.window, GLFW_KEY_LEFT) == GLFW_PRESS)
            mPhysic.sideBoostForce = -mPhysic.boosterForce * 1.2f, stopForce = false;
        if (glfwGetKey(update.window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            mPhysic.sideBoostForce = mPhysic.boosterForce * 1.2f, stopForce = false;
        if (stopForce)
            mPhysic.sideBoostForce = 0.f;
        mMatrix = glm::colMajor4(glm::vec4(mPhysic.direction, 0.f),
                                 glm::vec4(mPhysic.right, 0.f),
                                 glm::vec4(mPhysic.normal,0.f),
                                 glm::vec4(mPhysic.position,1.f));
    }
};